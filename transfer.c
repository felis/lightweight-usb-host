/* USB transfers    */


#define _transfer_c_

#include "project_config.h"

EP_RECORD dev0ep = {{ 0 }};         //Endpoint data structure for uninitialized device during enumeration
EP_RECORD msd_ep[ 3 ] = {{ 0 }};    //Mass storage bulk-only transport endpoints: 1 control and 2 bulk, IN and OUT
//ep records for other classes are defined in class-specific modules

/* macros to aid filling in TPL */
#define INIT_VID_PID(v,p) 0x##p##v
#define INIT_CL_SC_P(c,s,p) 0x##00##p##s##c

//const rom USB_TPL_ENTRY TplTable[ USB_NUMTARGETS + 1 ] = {
//// VID & PID or Client
//// Class, Subclass & Protocol Config Numep Eprecord Driver
//{ INIT_VID_PID( 0000, 0000 ), 0, 1, &dev0ep, 0, "Uninitialized" },
//{ INIT_VID_PID( 0781, 5406 ), 0, 3, msd_ep, MSD_DRIVER, "Mass storage"  },	    //Sandisk U3 Cruzer Micro
////{ INIT_VID_PID( 0CF2, 6220 ), 0, 0 },	                                        //ENE UB6220
//{ INIT_CL_SC_P( 03, 01, 02 ), 0, 3, hid_ep, HIDM_DRIVER, "HID Mouse with Boot protocol" },	                //
//{ INIT_VID_PID( aaaa, 5555 ), 0, 1, NULL, 0, NULL },	                //
//{ INIT_CL_SC_P( 08, 06, 50 ), 0, 3, msd_ep, MSD_DRIVER, "Mass storage" }	    //Mass storage bulk only class
//};

/* control transfers function pointers */
const rom CTRL_XFER ctrl_xfers[ 2 ] = {
    XferCtrlND,
    XferCtrlData
};

/* device table. Filled during enumeration              */
/* index corresponds to device address                  */
/* each entry contains pointer to endpoint structure    */
/* and device class to use in various places            */             
DEV_RECORD devtable[ USB_NUMDEVICES + 1 ];

/* Client Driver Function Pointer Table	*/
CLASS_CALLBACK_TABLE ClientDriverTable[ USB_NUMCLASSES ] = {
    {
        MSDProbe,				//Mass storage class device init
        MSDEventHandler,
        0
    },
    {
    	HIDMProbe,				//HID class device init
    	HIDMEventHandler,
    	0
    },
    {
        HIDKProbe,
        HIDKEventHandler,
        0
    }, 
    {
    	DummyProbe,
    	DummyEventHandler,
    	0
    }
};
/* Control transfer stages function pointer table */


/* USB state machine related variables */

BYTE usb_task_state = USB_DETACHED_SUBSTATE_INITIALIZE;
BYTE usb_error;
BYTE last_usb_task_state = 0;

/* external variables */
extern DWORD uptime;

/* Control transfer. Sets address, endpoint, fills control packet with necessary data, dispatches control packet, and initiates bulk IN transfer,   */
/* depending on request. Actual requests are defined as macros                                                                                      */
/* return codes:                */
/* 00       =   success         */
/* 01-0f    =   non-zero HRSLT  */
BYTE XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr )
{
 BOOL direction = FALSE;        //request direction, IN or OUT
 BYTE datastage = 1; //request data stage present or absent
 BYTE rcode;   
 SETUP_PKT setup_pkt;
    if( dataptr == NULL ) {
        datastage = 0;
    }
    MAXreg_wr( rPERADDR, addr );                    //set peripheral address
    /* fill in setup packet */
    if( bmReqType & 0x80 ) {
        direction = TRUE;                   //determine request direstion
    }
    /* fill in setup packet */
    setup_pkt.ReqType_u.bmRequestType = bmReqType;
    setup_pkt.bRequest = bRequest;
    setup_pkt.wVal_u.wValueLo = wValLo;
    setup_pkt.wVal_u.wValueHi = wValHi;
    setup_pkt.wIndex = wInd;
    setup_pkt.wLength = nbytes;
    MAXbytes_wr( rSUDFIFO, 8, (BYTE *)&setup_pkt );     //transfer to setup packet FIFO
    rcode = XferDispatchPkt( tokSETUP, ep );            //dispatch packet
    if( rcode ) {                                       //return HRSLT if not zero
        return( rcode );
    }
    rcode = ctrl_xfers[ datastage ]( addr, ep, nbytes, dataptr, direction );    //call data stage or no data stage transfer
    return( rcode );
}
/* Control transfer with data stage */
BYTE XferCtrlData( BYTE addr, BYTE ep, WORD nbytes, char* dataptr, BOOL direction )
{
  BYTE rcode;
    
    //MAXreg_wr( rHCTL, bmRCVTOG1 );       //set toggle to DATA1
    if( direction ) {   //IN transfer
        devtable[ addr ].epinfo[ ep ].rcvToggle = bmRCVTOG1;
        rcode = XferInTransfer( addr, ep, nbytes, dataptr, devtable[ addr ].epinfo[ ep ].MaxPktSize );
        if( rcode ) {
        return( rcode );
        }
        rcode = XferDispatchPkt( tokOUTHS, ep );
        return( rcode );
    }
    else {              //OUT not implemented
        return( 0xff );
    }    
}
/* Control transfer with status stage and no data stage */
BYTE XferCtrlND( BYTE addr, BYTE ep, WORD nbytes, char* dataptr, BOOL direction )
{
  BYTE rcode;
    if( direction ) { //GET
        rcode = XferDispatchPkt( tokOUTHS, ep );
    }
    else {
        rcode = XferDispatchPkt( tokINHS, ep );
    }
    return( rcode );
}
/* Dispatch a packet. Assumes peripheral address is set and, if necessary, sudFIFO-sendFIFO loaded. */
/* Result code: 0 success, nonzero = error condition                                                */
/* If NAK, tries to re-send up to USB_NAK_LIMIT times                                               */
/* If bus timeout, re-sends up to USB_RETRY_LIMIT times                                             */
/* return codes 0x00-0x0f are HRSLT( 0x00 being success ), 0xff means timeout                       */    
BYTE XferDispatchPkt( BYTE token, BYTE ep )
{
 DWORD timeout = uptime + USB_XFER_TIMEOUT;
 BYTE tmpdata;   
 BYTE rcode;
 char retry_count = 0;
 BYTE nak_count = 0;

    while( 1 ) {
        MAXreg_wr( rHXFR, ( token|ep ));            //launch the transfer
        rcode = 0xff;   
        while( uptime < timeout ) {
            tmpdata = MAXreg_rd( rHIRQ );
            if( tmpdata & bmHXFRDNIRQ ) {
                MAXreg_wr( rHIRQ, bmHXFRDNIRQ );    //clear the interrupt
                rcode = 0x00;
                break;
            }
        }
        if( rcode != 0x00 ) {               //exit if timeout
            return( rcode );
        }
        rcode = ( MAXreg_rd( rHRSL ) & 0x0f );
        if( rcode == hrNAK ) {
            nak_count++;
            if( nak_count == USB_NAK_LIMIT ) {
                break;
            }
            else {
                continue;
            }
        }
        if( rcode == hrTIMEOUT ) {
            retry_count++;
            if( retry_count == USB_RETRY_LIMIT ) {
                break;
            }
            else {
                continue;
            }
        }
        else break;
    }//while( 1 )
    return( rcode );
}
/* IN transfer to arbitrary endpoint. Assumes PERADDR is set. Handles multiple packets if necessary. Transfers 'nbytes' bytes.
    Keep sending INs and writes data to memory area pointed by 'data' */
/* rcode 0 if no errors. rcode 01-0f is relayed from prvXferDispatchPkt(). Rcode f0 means RCVDAVIRQ error,
            fe USB xfer timeout */
BYTE XferInTransfer( BYTE addr/* not sure if it's necessary */, BYTE ep, WORD nbytes, char* data, BYTE maxpktsize )
{
 BYTE rcode;
 BYTE i;
 BYTE tmpbyte;
 BYTE pktsize;
 WORD xfrlen = 0;
    MAXreg_wr( rHCTL, devtable[ addr ].epinfo[ ep ].rcvToggle );    //set toggle value
    while( 1 ) { // use a 'return' to exit this loop
        rcode = XferDispatchPkt( tokIN, ep );           //IN packet to EP-'endpoint'. Function takes care of NAKS.
        if( rcode ) {
            return( rcode );                            //should be 0, indicating ACK. Else return error code.
        }
        /* check for RCVDAVIRQ and generate error if not present */ 
        /* the only case when absense of RCVDAVIRQ makes sense is when toggle error occured. Need to add handling for that */
        if(( MAXreg_rd( rHIRQ ) & bmRCVDAVIRQ ) == 0 ) {
            return ( 0xf0 );                            //receive error
        }
        pktsize = MAXreg_rd( rRCVBC );                  //number of received bytes
        data = MAXbytes_rd( rRCVFIFO, pktsize, data );
        MAXreg_wr( rHIRQ, bmRCVDAVIRQ );                // Clear the IRQ & free the buffer
        xfrlen += pktsize;                              // add this packet's byte count to total transfer length
        /* The transfer is complete under two conditions:           */
        /* 1. The device sent a short packet (L.T. maxPacketSize)   */
        /* 2. 'nbytes' have been transferred.                       */
        if (( pktsize < maxpktsize ) || (xfrlen >= nbytes )) {    // have we transferred 'nbytes' bytes?
            if( MAXreg_rd( rHRSL ) & bmRCVTOGRD ) {                 //save toggle value
                devtable[ addr ].epinfo[ ep ].rcvToggle = bmRCVTOG1;
            }
            else {
                devtable[ addr ].epinfo[ ep ].rcvToggle = bmRCVTOG0;
            }
            return( 0 );
        }
  }//while( 1 )
}
/* initialization of USB data structures */
void USB_init( void )
{
  BYTE i;
    for( i = 0; i < ( USB_NUMDEVICES + 1 ); i++ ) {
        devtable[ i ].epinfo = NULL;       //clear device table
        devtable[ i ].devclass = 0;
    }
    devtable[ 0 ].epinfo = &dev0ep;  //set single ep for uninitialized device  
    dev0ep.MaxPktSize = 0;          	
    dev0ep.sndToggle = bmSNDTOG0;   //set DATA0/1 toggles to 0
    dev0ep.rcvToggle = bmRCVTOG0;
}
/* USB state machine. Connect/disconnect, enumeration, initialization   */
/* error codes: 01-0f HRSLT        */
/* ff - unsupported device         */
/* fe - no address available       */
/* fd - no client driver available */
void USB_Task( void )
{
 static DWORD usb_delay = 0;
 static BYTE tmp_addr;
 USB_DEVICE_DESCRIPTOR buf;

 BYTE rcode, tmpdata;
 char i;
 
    switch( usb_task_state & USB_STATE_MASK ) {
        /* Detached state - when nothing is connected to ( or just disconnected from) USB bus   */
        case( USB_STATE_DETACHED ):
            switch( usb_task_state ) {
                case( USB_DETACHED_SUBSTATE_INITIALIZE ): 
                    /* cleanup device data structures */
                    USB_init();
                    usb_task_state = USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE; 
                    break;
                case( USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE ):
                    /* Do nothing */
                    MAXreg_wr(rHCTL,bmSAMPLEBUS);
                    break;
                case( USB_DETACHED_SUBSTATE_ILLEGAL ):
                    /* don't know what to do yet */
                    break;
            }//switch( usb_task_state )     
            break;//( USB_STATE_DETACHED ):
            /**/
        case( USB_STATE_ATTACHED ):                     //prepare for enumeration
            switch( usb_task_state ) {
                case( USB_STATE_ATTACHED ):
                    usb_delay = uptime + 200;           //initial settle 200ms
                    usb_task_state = USB_ATTACHED_SUBSTATE_SETTLE;
                    break;//case( USB_STATE_ATTACHED )
                case( USB_ATTACHED_SUBSTATE_SETTLE ):   //waiting for settle timer to expire
                    if( uptime > usb_delay ) {
                        usb_task_state = USB_ATTACHED_SUBSTATE_RESET_DEVICE;
                    }
                    break;//case( USB_ATTACHED_SUBSTATE_SETTLE )    
                case( USB_ATTACHED_SUBSTATE_RESET_DEVICE ):
                    MAXreg_wr( rHIRQ, bmBUSEVENTIRQ );                               //clear bus event IRQ
                    MAXreg_wr( rHCTL, bmBUSRST );                                    //issue bus reset
                    usb_task_state = USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE;
                    break;//case( USB_ATTACHED_SUBSTATE_RESET_DEVICE )
                case( USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE ):                  //wait for bus reset and first SOF
                    if(( MAXreg_rd( rHCTL ) & bmBUSRST ) == 0 ) {
                        tmpdata = MAXreg_rd( rMODE ) | bmSOFKAENAB;                 //start SOF generation
                        MAXreg_wr( rMODE, tmpdata );
                        usb_task_state = USB_ATTACHED_SUBSTATE_WAIT_SOF;
                    }
                    break;//case( USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE )
                case( USB_ATTACHED_SUBSTATE_WAIT_SOF ):
                    if( MAXreg_rd( rHIRQ ) | bmFRAMEIRQ ) {                     //when first SOF received we can continue
                        usb_task_state = USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE;
                    }
                    break;//case( USB_ATTACHED_SUBSTATE_WAIT_SOF )
                case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE ):   //send request for first 8 bytes of device descriptor
                    devtable[ 0 ].epinfo->MaxPktSize = 0x0008;             //fill max packet size with minimum allowed
                    rcode = XferGetDevDescr( 0, 0, 8, (char *)&buf );       //get device descriptor size
                    if( rcode == 0 ) {
                        devtable[ 0 ].epinfo->MaxPktSize = buf.bMaxPacketSize0;
                        usb_task_state = USB_STATE_ADDRESSING;
                    }
                    else {
                        usb_error = rcode;
                        last_usb_task_state = usb_task_state;
                        usb_task_state = USB_STATE_ERROR;
                    }                                                      
                    break;//case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE ):   
            }//switch( usb_task_state )
            break;//case ( USB_STATE_ATTACHED )
        case( USB_STATE_ADDRESSING ):   //give device an address
            for( i = 1; i < USB_NUMDEVICES; i++ ) {
                if( devtable[ i ].epinfo == NULL ) {
                    devtable[ i ].epinfo = devtable[ 0 ].epinfo;        //set correct MaxPktSize
                    //devtable[ i ].epinfo->MaxPktSize = devtable[ 0 ].epinfo->MaxPktSize;  //copy unitialized device record to have correct MaxPktSize
                    rcode = XferSetAddr( 0, 0, i );
                    if( rcode == 0 ) {
                        tmp_addr = i;
                        usb_task_state = USB_STATE_CONFIGURING;
                    }
                    else {
                        usb_error = rcode;          //set address error
                        last_usb_task_state = usb_task_state;
                        usb_task_state = USB_STATE_ERROR;
                    }
                    break;  //break if address assigned or error occured during address assignment attempt                      
                }
            }
            if( usb_task_state == USB_STATE_ADDRESSING ) {
                usb_error = 0xfe;
                last_usb_task_state = usb_task_state;
                usb_task_state = USB_STATE_ERROR;
            }    
            break;//case ( USB_STATE_ADDRESSING )
        case( USB_STATE_CONFIGURING ):              //checking for driver
            /* run device class probes until one returns TRUE    */
            for( i = 0; i < USB_NUMCLASSES; i++ ) {
                rcode = ClientDriverTable[ i ].Initialize( tmp_addr, 0 );
                if( rcode == TRUE ) {
			        usb_task_state = USB_STATE_RUNNING;  
                    break;  
                }
            }
            if( usb_task_state == USB_STATE_CONFIGURING ) {
                usb_error = 0xfd;
                last_usb_task_state = usb_task_state;
                usb_task_state = USB_STATE_ERROR;
            }   
            break;//( USB_STATE_CONFIGURING )
        case( USB_STATE_RUNNING ):
          //vTaskDelay( LED_RATE );
            break;//( USB_STATE_RUNNING )
        case( USB_STATE_ERROR ):
            //vTaskDelay( LED_RATE ); //stay here if error
            break;//( USB_STATE_ERROR ) 
        default:
            //Should never get here
            break;  
    }//switch( usb_task_state & STATE_MASK )
}
/* returns TRUE if device is successfuly identified and configured, otherwise returns FALSE */
BOOL MSDProbe( BYTE addr, DWORD flags )
{
    return( FALSE );
}				

BOOL MSDEventHandler( BYTE address, BYTE event, void *data, DWORD size )
{

	return( FALSE );

}

BOOL CDCProbe( BYTE address, DWORD flags )
{
	
	return( FALSE );
	
}

BOOL CDCEventHandler( BYTE address, BYTE event, void *data, DWORD size )
{
	return( FALSE );
}

BOOL DummyProbe( BYTE address , DWORD flags )
{
	return( FALSE );
}

BOOL DummyEventHandler( BYTE address, BYTE event, void *data, DWORD size )
{
	return( FALSE );
}
/* Function to access usb_task_state variable from outside */
BYTE GetUsbTaskState( void )
{
    return( usb_task_state );
}
/* Function to access devtable[] from outside */
DEV_RECORD* GetDevtable( BYTE index )
{
    return( &devtable[ index ] );
}