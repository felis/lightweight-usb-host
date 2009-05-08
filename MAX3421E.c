/* MAX3421E low-level functions                             */
/* reading, writing registers, reset, host transfer, etc.   */
/* GPIN, GPOUT are as per tutorial, reassign if necessary   */
/* USB power on is GPOUT7, USB power overload is GPIN7      */

#define _MAX3421E_C_

#include "project_config.h"

/* variables and data structures */



/* External variables */

extern DWORD uptime;
extern BYTE usb_task_state;


/* Functions    */

/* SPI initialization */
/* this routine was borrowed from Microchip peripheral library. It's been rumored that they're going to stop including
    peripherals with new releases of C18 so I make a copy just in case.
    
        sync_mode:
                            SPI_FOSC_4          SPI Master mode, clock = FOSC/4
                            SPI_FOSC_16         SPI Master mode, clock = FOSC/16
                            SPI_FOSC_64         SPI Master mode, clock = FOSC/64
                            SPI_FOSC_TMR2       SPI Master mode, clock = TMR2 output/2
                            SLV_SSON            SPI Slave mode, /SS pin control enabled
                            SLV_SSOFF           SPI Slave mode, /SS pin control disabled
        bus_mode:
                            MODE_00             SPI bus Mode 0,0
                            MODE_01             SPI bus Mode 0,1
                            MODE_10             SPI bus Mode 1,0
                            MODE_11             SPI bus Mode 1,1
        smp_phase:
                            SMPEND              Input data sample at end of data out
                            SMPMID              Input data sample at middle of data out
*/
void SPI_init( BYTE sync_mode, BYTE bus_mode, BYTE smp_phase )
{
    SSPSTAT &= 0x3f;        //power-on state
    SSPCON1 = 0x00;         //power-on state, SSP disabled
    SSPCON1 |= sync_mode;
    SSPSTAT |= smp_phase;
    
  switch( bus_mode ) {
    case 0:                       // SPI1 bus mode 0,0
      SSPSTATbits.CKE = 1;       // data transmitted on rising edge
      break;    
    case 2:                       // SPI1 bus mode 1,0
      SSPSTATbits.CKE = 1;       // data transmitted on falling edge
      SSPCON1bits.CKP = 1;       // clock idle state high
      break;
    case 3:                       // SPI1 bus mode 1,1
      SSPCON1bits.CKP = 1;       // clock idle state high
      break;
    default:                      // default SPI1 bus mode 0,1
      break;
  }

  switch( sync_mode ) {
    case 4:                       // slave mode w /SS1 enable
      TRISAbits.TRISA5 = 1;       // define /SS1 pin as input
    case 5:                       // slave mode w/o /SS1 enable
      TRISCbits.TRISC3 = 1;       // define clock pin as input
      SSPSTATbits.SMP = 0;          // must be cleared in slave SPI mode
      break;
    default:                      // master mode, define clock pin as output
      TRISCbits.TRISC3 = 0;       // define clock pin as output
      break;
  }

  TRISC &= 0xDF;                    // define SDO as output (master or slave)
  TRISC |= 0x10;                    // define SDI as input (master or slave)
  // IPR1bits.SSPIP = 0;                    //low-priority interrupt
  // PIE1bits.SSPIE = 1;                    //interrupt enable
  SSPCON1 |= SSPENB;                // enable synchronous serial port 
}

/* writes to SPI. BF is checked inside the procedure */
/* returns SSPBUF   */ 
BYTE SPI_wr( BYTE data )
{
    SSPBUF = data;              // write byte to SSP2BUF register
    while( !SSPSTATbits.BF );   // wait until bus cycle completes
    return ( SSPBUF );          //
}
/* Single host register write   */
void MAXreg_wr(BYTE reg, BYTE val)
{
    Select_MAX3421E;
    SPI_wr ( reg + 2 ); //set WR bit and send register number
    SPI_wr ( val );
    Deselect_MAX3421E;
}
/* multiple-byte write */
/* returns a pointer to a memory position after last written */
char* MAXbytes_wr( BYTE reg, BYTE nbytes, char* data )
{
    Select_MAX3421E;    //assert SS
    SPI_wr ( reg + 2 ); //set W/R bit and select register   
    while( nbytes ) {                
        SPI_wr( *data );    // send the next data byte
        data++;             // advance the pointer
        nbytes--;
    }
    Deselect_MAX3421E;  //deassert SS
    return( data );
}
/* Single host register read        */
BYTE MAXreg_rd( BYTE reg )    
{
 BYTE tmp;
    Select_MAX3421E;
    SPI_wr ( reg );         //send register number
    tmp = SPI_wr ( 0x00 );  //send empty byte, read register contents
    Deselect_MAX3421E; 
    return (tmp);
}
/* multiple-bytes register read                             */
/* returns a pointer to a memory position after last read   */
char* MAXbytes_rd ( BYTE reg, BYTE nbytes, char* data )
{
    Select_MAX3421E;    //assert SS
    SPI_wr ( reg );     //send register number
    while( nbytes ) {
        *data = SPI_wr ( 0x00 );    //send empty byte, read register contents
        data++;
        nbytes--;
    }
    Deselect_MAX3421E;  //deassert SS
    return( data );   
}
/* reset MAX3421E using chip reset bit. SPI configuration is not affected   */
void MAX3421E_reset( void )
{
 BYTE tmp = 0;
    MAXreg_wr( rUSBCTL,bmCHIPRES );                     //Chip reset. This stops the oscillator
    MAXreg_wr( rUSBCTL,0x00 );                          //Remove the reset
    while(!(MAXreg_rd( rUSBIRQ ) & bmOSCOKIRQ )) {      //wait until the PLL stabilizes
        tmp++;                                          //timeout after 256 attempts
        if( tmp == 0 ) break;
    }
}
/* turn USB power on/off                                                */
/* ON pin of VBUS switch (MAX4793 or similar) is connected to GPOUT7    */
/* OVERLOAD pin of Vbus switch is connected to GPIN7                    */
/* OVERLOAD state low. NO OVERLOAD or VBUS OFF state high.              */
BOOL Vbus_power ( BOOL action )
{
    BYTE tmp = MAXreg_rd( rIOPINS2 );       //copy of IOPINS2
    if( action ) {                              //turn on by setting GPOUT7
        tmp |= bmGPOUT7;
    }
    else {                                      //turn off by clearing GPOUT7
        tmp &= ~bmGPOUT7;
    }
    MAXreg_wr( rIOPINS2,tmp );                              //send GPOUT7
    if( action ) Delay10KTCYx( 60 );                            //wait 60ms for Vbus to stabilize
    if (!(MAXreg_rd( rIOPINS2 )&bmGPIN7)) return( FALSE );  // check if overload is present
    return( TRUE );                                             // power on/off successful                       
}

/* probe bus to determine device presense and speed */
void MAX_busprobe( void )
{
 BYTE bus_sample;
    
//  MAXreg_wr(rHCTL,bmSAMPLEBUS); 
    bus_sample = MAXreg_rd( rHRSL );            //Get J,K status
    bus_sample &= ( bmJSTATUS|bmKSTATUS );      //zero the rest of the byte

    switch( bus_sample ) {                          //start full-speed or low-speed host 
        case( bmJSTATUS ):
            /*kludgy*/
            if( usb_task_state != USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE ) { //bus reset causes connection detect interrupt
                if( !(MAXreg_rd( rMODE ) & bmLOWSPEED )) {
                    MAXreg_wr( rMODE, MODE_FS_HOST );           //start full-speed host
                }
                else {
                    MAXreg_wr( rMODE, MODE_LS_HOST);    //start low-speed host
                }
                usb_task_state = ( USB_STATE_ATTACHED );    //signal usb state machine to start attachment sequence
            }
            break;
        case( bmKSTATUS ):
            if( usb_task_state != USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE ) { //bus reset causes connection detect interrupt
                if( !(MAXreg_rd( rMODE ) & bmLOWSPEED )) {
                    MAXreg_wr( rMODE, MODE_LS_HOST );   //start low-speed host
                }
                else {
                    MAXreg_wr( rMODE, MODE_FS_HOST );               //start full-speed host
                }
                usb_task_state = ( USB_STATE_ATTACHED );    //signal usb state machine to start attachment sequence
            }
            break;
        case( bmSE1 ):              //illegal state
            usb_task_state = ( USB_DETACHED_SUBSTATE_ILLEGAL );
            break;
        case( bmSE0 ):              //disconnected state
            if( !(( usb_task_state & USB_STATE_MASK ) == USB_STATE_DETACHED ))          //if we came here from other than detached state
                usb_task_state = ( USB_DETACHED_SUBSTATE_INITIALIZE );  //clear device data structures
            else {  
              MAXreg_wr( rMODE, MODE_FS_HOST ); //start full-speed host
              usb_task_state = ( USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE );
            }
            break;
        }//end switch( bus_sample )
}
/* MAX3421E initialization after power-on   */
void MAX3421E_init( void )
{
 BYTE tmp;
    /* Configure full-duplex SPI, interrupt pulse   */
    MAXreg_wr( rPINCTL,(bmFDUPSPI+bmINTLEVEL+bmGPXB ));     //Full-duplex SPI, level interrupt, GPX
    MAX3421E_reset();                                       //stop/start the oscillator
    /* configure power switch   */
    Vbus_power( OFF );                                      //turn Vbus power off
    MAXreg_wr( rGPINIEN, bmGPINIEN7 );                      //enable interrupt on GPIN7 (power switch overload flag)
    Vbus_power( ON  );
    /* configure host operation */
    MAXreg_wr( rMODE, bmDPPULLDN|bmDMPULLDN|bmHOST|bmSEPIRQ );      // set pull-downs, SOF, Host, Separate GPIN IRQ on GPX
    //MAXreg_wr( rHIEN, bmFRAMEIE|bmCONDETIE|bmBUSEVENTIE );                      // enable SOF, connection detection, bus event IRQs
    MAXreg_wr( rHIEN, bmCONDETIE );                                             //connection detection
    /* HXFRDNIRQ is checked in Dispatch packet function */
    MAXreg_wr(rHCTL,bmSAMPLEBUS);                                               // update the JSTATUS and KSTATUS bits
    MAX_busprobe();                                                             //check if anything is connected
    MAXreg_wr( rHIRQ, bmCONDETIRQ );                                            //clear connection detect interrupt                 
    MAXreg_wr( rCPUCTL, 0x01 );                                                 //enable interrupt pin
}

/* MAX3421 state change task and interrupt handler */
void MAX3421E_Task( void )
{
    if( MAX3421E_INT == 0 ) {
        MaxIntHandler();
    }
    if( MAX3421E_GPX == 1 ) {
        MaxGpxHandler();
    }   
}   

void MaxIntHandler( void )
{
 BYTE HIRQ;
 BYTE HIRQ_sendback = 0x00;
        HIRQ = MAXreg_rd( rHIRQ );                  //determine interrupt source
//        if( HIRQ & bmFRAMEIRQ ) {                   //->1ms SOF interrupt handler
//                    HIRQ_sendback |= bmFRAMEIRQ;
//        }//end FRAMEIRQ handling
        if( HIRQ & bmCONDETIRQ ) {
            MAX_busprobe();
            HIRQ_sendback |= bmCONDETIRQ;
        }
        //if ( HIRQ & bmBUSEVENTIRQ ) {               //bus event is either reset or suspend
        //    usb_task_state++;                       //advance USB task state machine
        //    HIRQ_sendback |= bmBUSEVENTIRQ; 
        //}
        /* End HIRQ interrupts handling, clear serviced IRQs    */
        MAXreg_wr( rHIRQ, HIRQ_sendback );
}
void MaxGpxHandler( void )
{
 BYTE GPINIRQ;

    GPINIRQ = MAXreg_rd( rGPINIRQ );            //read both IRQ registers
}
