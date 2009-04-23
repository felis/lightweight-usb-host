/* USB transfers    */


#define _transfer_c_

#include "project_config.h"

/* external variables */
extern DWORD uptime;

/* Control transfer. Sets address, endpoint, fills control packet with necessary data, dispatches control packet, and initiates bulk IN transfer,   */
/* depending on request. Actual requests are defined as macros                                                              */
char XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr )
{
 SETUP_PKT setup_pkt;
    MAXreg_wr( rPERADDR, addr );                    //set peripheral address
    /* fill in setup packet */
    setup_pkt.ReqType_u.bmRequestType = bmReqType;
    setup_pkt.bRequest = bRequest;
    setup_pkt.wVal_u.wValueLo = wValLo;
    setup_pkt.wVal_u.wValueHi = wValHi;
    setup_pkt.wIndex = wInd;
    setup_pkt.wLength = nbytes;
    MAXbytes_wr( rSUDFIFO, 8, (BYTE *)&setup_pkt ); //transfer to setup packet FIFO
}
/* Dispatch a packet. Assumes peripheral address is set and, if necessary, sudFIFO-sendFIFO loaded. */
/* Result code: 0 success, nonzero = error condition                                                */
/* If NAK, tries to re-send up to USB_NAK_LIMIT times                                               */
/* If bus timeout, re-sends up to USB_RETRY_LIMIT times                                             */
/* return codes 0x00-0x0f are HRSLT( 0x00 being success ), 0xff means timeout                       */    
char XferDispatchPkt ( BYTE token, BYTE ep )
{
 DWORD timeout = uptime + USB_XFER_TIMEOUT;
 BYTE tmpdata;   
 char rcode;
 char retry_count = 0;
 BYTE nak_count = 0;

    while( 1 ) {
        MAXreg_wr( rHXFR, ( token|ep ));    //launch the transfer
        rcode = 0xff;   
        while( uptime < timeout ) {
            tmpdata = MAXreg_rd( rHIRQ );
            if( tmpdata & bmHXFRDNIRQ ) {
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
