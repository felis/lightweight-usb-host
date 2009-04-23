/* MAX3421E low-level functions                             */
/* reading, writing registers, reset, host transfer, etc.   */
/* GPIN, GPOUT are as per tutorial, reassign if necessary   */
/* USB power on is GPOUT7, USB power overload is GPIN7      */

#define _MAX3421E_C_

#include "project_config.h"

/* variables and data structures */

#define NUM_EP  16
EP_RECORD ep_data[ NUM_EP ];    //endpoint data

static BYTE usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;

/* External variables */

extern DWORD uptime;


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
/* RTOS-aware multiple-byte write */
void MAXbytes_wr( BYTE reg, BYTE nbytes, BYTE * data )
{
    Select_MAX3421E;    //assert SS
    SPI_wr ( reg + 2 ); //set W/R bit and select register   
    while( nbytes ) {                
        SPI_wr( *data );    // send the next data byte
        data++;             // advance the pointer
        nbytes--;
    }
    Deselect_MAX3421E;  //deassert SS   
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
/* RTOS-aware multiple-bytes register read */
void MAX_bytes_rd ( BYTE reg, BYTE nbytes, BYTE *data )
{
    Select_MAX3421E;    //assert SS
    SPI_wr ( reg );     //send register number
    while( nbytes ) {
        *data = SPI_wr ( 0x00 );    //send empty byte, read register contents
        data++;
        nbytes--;
    }
    Deselect_MAX3421E;  //deassert SS   
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
            if( !(MAXreg_rd( rMODE ) & bmLOWSPEED )) {
            MAXreg_wr( rMODE, MODE_FS_HOST );           //start full-speed host
            }
            else {
            MAXreg_wr( rMODE, MODE_LS_HOST);    //start low-speed host
            }
            usb_task_state = ( USB_STATE_ATTACHED );    //signal usb state machine to start attachment sequence
            break;
        case( bmKSTATUS ):
            if( !(MAXreg_rd( rMODE ) & bmLOWSPEED )) {
            MAXreg_wr( rMODE, MODE_LS_HOST );   //start low-speed host
            }
            else {
            MAXreg_wr( rMODE, MODE_FS_HOST );               //start full-speed host
            }
            usb_task_state = ( USB_STATE_ATTACHED );    //signal usb state machine to start attachment sequence
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
    MAXreg_wr( rMODE, bmDPPULLDN|bmDMPULLDN|bmSOFKAENAB|bmHOST|bmSEPIRQ );      // set pull-downs, SOF, Host, Separate GPIN IRQ on GPX
    MAXreg_wr( rHIEN, bmFRAMEIE|bmCONDETIE|bmBUSEVENTIE );                      // enable SOF, connection detection, bus event IRQs
    /* HXFRDNIRQ is checked in Dispatch packet function */
    MAXreg_wr(rHCTL,bmSAMPLEBUS);                                               // update the JSTATUS and KSTATUS bits
    MAX_busprobe();                                                             //check if anything is connected
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
        if( HIRQ & bmFRAMEIRQ ) {                   //->1ms SOF interrupt handler
                    HIRQ_sendback |= bmFRAMEIRQ;
        }//end FRAMEIRQ handling
        if( HIRQ & bmCONDETIRQ ) {
            MAX_busprobe();
            HIRQ_sendback |= bmCONDETIRQ;
        }
        if ( HIRQ & bmBUSEVENTIRQ ) {               //bus event is either reset or suspend
            usb_task_state++;                       //advance USB task state machine
            HIRQ_sendback |= bmBUSEVENTIRQ; 
        }
        /* End HIRQ interrupts handling, clear serviced IRQs    */
        MAXreg_wr( rHIRQ, HIRQ_sendback );
}
void MaxGpxHandler( void )
{
 BYTE GPINIRQ;

    GPINIRQ = MAXreg_rd( rGPINIRQ );            //read both IRQ registers
}

/* USB state machine. Connect/disconnect, enumeration, initialization   */
void USB_Task( void )
{
// static DWORD usb_delay = 0;
// 
//  switch( usb_task_state & USB_STATE_MASK ) {
//      case( USB_STATE_DETACHED ):
//          switch( usb_task_state ) {
//              case( USB_DETACHED_SUBSTATE_INITIALIZE ): 
//              /* cleanup device data structures */
//                  USBdata_init();
//                  usb_task_state = USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE; 
//                  break;
//              case( USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE ):
//                  /* Do nothing */
//                  break;
//              case( USB_DETACHED_SUBSTATE_ILLEGAL ):
//                  /* don't know what to do yet */
//                  break;
//          }//switch( usb_task_state )     
//          break;//( USB_STATE_DETACHED ):
//      case( USB_STATE_ATTACHED ):                         //prepare for enumeration
//              switch( usb_task_state ) {
//                  case( USB_STATE_ATTACHED ):
//                      usb_delay = uptime + 200;           //initial settle 200ms
//                      if( uptime > usb_delay ) {
//                          usb_task_state = USB_ATTACHED_SUBSTATE_RESET_DEVICE;
//                      }
//                      break;
//                  case( USB_ATTACHED_SUBSTATE_RESET_DEVICE ):
//                      MAXreg_wr( rHIRQ, bBUSEVENTIRQ );                               //clear bus event IRQ
//                      MAXreg_wr( rHCTL, bBUSRST );                                    //issue bus reset
//                      usb_delay = uptime + 200;
//                      usb_task_state = USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE;
//                      break;//case( USB_ATTACHED_SUBSTATE_RESET_DEVICE )
//                  case( USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE ):                  //MAX3421E task makes advance to the next state
//                      if( uptime > usb_delay ) {
//                          usb_task_state = USB_ATTACHED_SUBSTATE_RESET_DEVICE;        //go back and reset again if timeout
//                      }
//                      break;  
//                  case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE ):   //send request for first 8 bytes of device descriptor
//                      //Nop();                                                //to get the right max packet size
//                      last_usb_task_state = usb_task_state;
//                      devinfo[ 0 ].eplist_ptr->wMaxPacketSize = 0x0008;       //fill max packet size with minimum allowed
//                      tmp_status = bXferGetDevDescr( 0, 0, 8, &urb_usbt );    //fill the rest and queue the request
//                      if( tmp_status == 0 ) {                                 //if success
//                          tmp_status = xQueueReceive( xUsbXferDataQ, &tmp_data, USB_XFERQ_TIMEOUT );  //get descriptor length in tmp_data
//                          if( tmp_status == pdTRUE ) {
//                              for( i = 0; i < 7; i++ ) {                                              //get max packet size
//                                  tmp_status = xQueueReceive( xUsbXferDataQ, &tmp_maxps, USB_XFERQ_TIMEOUT );
//                              }
//                              if( tmp_status == pdTRUE ) {
//                                  devinfo[ 0 ].eplist_ptr->wMaxPacketSize = tmp_maxps;            //copy max packet size to endpoint record
//                                  usb_task_state = USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR;       
//                              }
//                              else {
//                                  usbt_errorcode = USB_DATAQ;
//                                  usb_task_state = USB_STATE_ERROR;
//                              }
//                          }
//                          else {
//                              usbt_errorcode = USB_DATAQ;
//                              usb_task_state = USB_STATE_ERROR;
//                          }   
//                      }//if success
//                      else {
//                          usbt_errorcode = USB_URBQ;
//                          usb_task_state = USB_STATE_ERROR;
//                      }
////                        while( i = uxQueueMessagesWaiting( xUsbXferDataQ ))                                 //flush data queue
////                                tmp_status = xQueueReceive( xUsbXferDataQ, NULL, USB_XFERQ_TIMEOUT );
//                      break;//case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE ):   
//                  case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR ):    //send the same request with correct length and max packet size
//                      //Nop();                                            //check VID,PID and/or Class, subclass, protocol against TPL
//                      last_usb_task_state = usb_task_state;
//                      usb_task_state = ( USB_ATTACHED_SUBSTATE_VALIDATE_VID_PID );                                            //read back VID,PID to use in address allocation
////                        tmp_status = bXferGetDevDescr( 0, 0, tmp_data, &urb_usbt );
////                        if( tmp_status == 0 ) {
////                            tmpbyte_ptr = ( BYTE * )tmp_vidpid;
////                            for( i = 0; i < 9; i++ ) {
////                                tmp_status = xQueueReceive( xUsbXferDataQ, tmpbyte_ptr, USB_XFERQ_TIMEOUT );    //read first byte of VID 
////                            }
////                            if( tmp_status == pdTRUE ) {
////                                for( i = 0; i < 3; i++ ) {
////                                    tmpbyte_ptr++;
////                                    tmp_status = xQueueReceive( xUsbXferDataQ, tmpbyte_ptr, USB_XFERQ_TIMEOUT );    //read the rest of VID-PID
////                                }
////                                if( tmp_status == pdTRUE ) {
////                                    usb_task_state = USB_ATTACHED_SUBSTATE_VALIDATE_VID_PID;
////                                }
////                                else {
////                                    usbt_errorcode = USB_DATAQ;
////                                    usb_task_state = USB_STATE_ERROR;
////                                }           
////                            }
////                            else {
////                                usbt_errorcode = USB_DATAQ;
////                                usb_task_state = USB_STATE_ERROR;
////                            }
////                        }
////                        else {
////                            usbt_errorcode = USB_URBQ;
////                            usb_task_state = USB_STATE_ERROR;
////                        }
////                        while( i = uxQueueMessagesWaiting( xUsbXferDataQ )) {                                   //flush data queue
////                            //Nop();
////                            tmp_status = xQueueReceive( xUsbXferDataQ, NULL, USB_XFERQ_TIMEOUT );
////                        }
//                      break;//case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR ):
//                  case( USB_ATTACHED_SUBSTATE_VALIDATE_VID_PID ):                     //looking for device record, determining device address
//                      last_usb_task_state = usb_task_state;
////                        tmp_tplentry = 0x00000000;
////                        tmp_tplidx = 0xff;
////                        tmpbyte_ptr = ( BYTE * )tmp_tplentry;
////                        tmp_status = bXferGetDevDescr( 0, 0, 0x12, &urb_usbt );                                 //request device descriptor
////                        tmp_status = xQueueReceive( xUsbXferDataQ, tmpbyte_ptr, USB_XFERQ_TIMEOUT );
////                        for( i = 0; i < 9; i++ ) {
////                                tmp_status = xQueueReceive( xUsbXferDataQ, tmpbyte_ptr, USB_XFERQ_TIMEOUT );    //read first byte of VID 
////                            }
////                        for( i = 0; i < 3; i++ ) {
////                            tmpbyte_ptr++;
////                            tmp_status = xQueueReceive( xUsbXferDataQ, tmpbyte_ptr, USB_XFERQ_TIMEOUT );        //read the rest of VID-PID
////                            }
////                        for( i = 0; i < USB_NUMTARGETS; i++ ) {
////                            if( TplTable[ i ].device_u.val == tmp_tplentry )    
////                            tmp_tplidx = i;
////                            break;
////                        }
////                        if( tmp_tplidx == 0xff ) {                                                              //no VID,PID in the table
////                            tmp_tplentry = 0x00000000;
////                                                                                    
//                      
//                      
//                      usb_task_state = ( USB_STATE_ADDRESSING );
//                      
//                      /* class.c validates device */
//                      
////                        /* looking for pre-filled record */
////                        tmp_data = 0;
////                        i = 1;
////                        while((( i < USB_NUMDEVICES ) && ( tmp_data == 0 ))) {  //looking for pre-configured vacant record
////                            if(( devinfo[ i ].idVendor == tmp_vidpid[ 0 ] ) && ( devinfo[ i ].idProduct == tmp_vidpid[ 1 ]) && ( devinfo[ i ].eplist_ptr->bToggle == 0xff )) {                          
////                                tmp_data = i;
////                                devinfo[ i ].eplist_ptr->bToggle = 0x00;
////                            }
////                            i++;
////                        }   
////                        /* looking for vacant record */
////                        i = 1;
////                        while((( tmp_data == 0 ) && ( i < USB_NUMDEVICES ))) {
////                            if(( devinfo[ i ].idVendor == 0xffff ) && ( devinfo[ i ].eplist_ptr->bToggle == 0xff )) {                                           //vacant
////                                tmp_data = i;
////                                devinfo[ i ].idVendor = tmp_vidpid[ 0 ];
////                                devinfo[ i ].idProduct = tmp_vidpid[ 1 ];
////                                devinfo[ i ].eplist_ptr->bToggle = 0x00;
////                            }
////                            i++;
////                        }
////                        if( tmp_data == 0 ) {
////                            usbt_errorcode = NO_ADDRESS;
////                            usb_task_state = USB_STATE_ERROR;
////                        }
////                        else
////                            usb_task_state = ( USB_STATE_ADDRESSING );
//                      break;//( USB_ATTACHED_SUBSTATE_VALIDATE_VID_PID ) 
//                  }//switch( usb_task_state )
//              break;//( USB_STATE_ATTACHED )
//          case( USB_STATE_ADDRESSING ):
//              last_usb_task_state = usb_task_state;
//              /* finding vacant address */
//              i = 0;
//              tmp_addr = 0;
//              do {
//                  if( devinfo[ i ].eplist_ptr->bToggle == 0xff )      //looking for device with Toggle value of ep0 .eq. 0xff
//                      tmp_addr = i;
//                  i++;
//              } while((( tmp_addr == 0 ) && ( i < USB_NUMDEVICES )));
//              if( tmp_addr == 0 ) {                                   //if all device addresses are taken
//                  usbt_errorcode = NO_ADDRESS;    
//                  usb_task_state = USB_STATE_ERROR;
//                  break;
//              }
//              /* fill in  and queue setup packet */
//              tmp_status = bXferSetAddr( 0, 0, tmp_addr, &urb_usbt ); //old address, endpoint, new address, ponter to urb
//              if( tmp_status == 0 ) {
//                  devinfo[ tmp_addr ].eplist_ptr->wMaxPacketSize = devinfo[ 0 ].eplist_ptr->wMaxPacketSize;   //set ep0 max packet size for new address
//                  devinfo[ tmp_addr ].eplist_ptr->bToggle = 0x00;                                             //clear Toggle to show that address is taken
//                  usb_task_state = USB_STATE_CONFIGURING;
//              }
//              else {
//                  usbt_errorcode = USB_URBQ;  
//                  usb_task_state = USB_STATE_ERROR;
//              }   
//              break;//( USB_STATE_ADDRESSING )
//          case( USB_STATE_CONFIGURING ):              //checking for driver
//              last_usb_task_state = usb_task_state;
//
//              /* run device class probes until on returns TRUE    */
//              for( i = 0; i < USB_NUMCLASSES; i++ ) {
//                  tmp_status = ClientDriverTable[ i ].Initialize( tmp_addr, 0 );
//                  if( tmp_status == TRUE ) {
//                      break;  
//                  }
//              }
//              Nop();
//              if( devinfo[ tmp_addr ].config != 0xff ) {      //configure device
//                  tmp_status = bXferGetConfDescr( tmp_addr, 0, 9, devinfo[ tmp_addr ].config, &urb_usbt );
//                  /* get bConfigurationValue  */
//                  for( i = 0; i < 6; i++ ) {
//                      tmp_status = xQueueReceive( xUsbXferDataQ, &tmp_data, USB_XFERQ_TIMEOUT );
//                  }
//                  flush_Q( xUsbXferDataQ );
//                  tmp_status = bXferSetConf( tmp_addr, 0, tmp_data , &urb_usbt );
////                        if( tmp_status == 0 ) {
////                            usb_task_state = USB_STATE_RUNNING;
////                        }
//              }
//              usb_task_state = USB_STATE_RUNNING;     
//                      
////                if( usb_task_state != USB_STATE_RUNNING ) {
////                    usbt_errorcode = NO_DRIVER;
////                    usb_task_state = USB_STATE_ERROR;    
////                }
//              break;//( USB_STATE_CONFIGURING )
//          case( USB_STATE_RUNNING ):
//              vTaskDelay( LED_RATE );
//              break;//( USB_STATE_RUNNING )
//          case( USB_STATE_ERROR ):
//              vTaskDelay( LED_RATE ); //stay here if error
//              break;//( USB_STATE_ERROR ) 
//          default:
//              //Should never get here
//              break;  
//      }//switch( usb_task_state & STATE_MASK )
}

/* USB data structure initialization. Called when task is started and when device is disconnected from the bus */
void USBdata_init( void )
{
 BYTE i;
    for( i = 0; i < NUM_EP; i++ ) {
        ep_data[ i ].bEndpointAddress = 0xff;
    }
}

BYTE Get_UsbTaskState( void )
{
    return( usb_task_state );
}