/* HID class support functions */

#include "project_config.h"

extern char bigbuf[];   //256 bytes
extern DEV_RECORD devtable[];

HID_DEVICE hid_device = {{ 0 }};
EP_RECORD hid_ep[ 2 ] = {{ 0 }};    //HID class endpoints, 1 control, 1 interrupt-IN
                                    //the third endpoint is not implemented     

/* HID Mouse probe. Called from USB state machine.                          */
/* assumes configuration length is less than 256 bytes                      */
/* looks for Class:03, Subclass: 01, Protocol: 02 in interface descriptor   */
/* sets mouse in boot protocol                                              */
/* assumes single configuration and interface configuration 0               */
BOOL HIDMProbe( BYTE addr, DWORD flags )
{
  BYTE tmpbyte;
  BYTE rcode;
  BYTE confvalue;
  WORD total_length;  
  USB_DESCR* data_ptr = ( USB_DESCR * )&bigbuf;
  char* byte_ptr = bigbuf; 
    rcode = XferGetConfDescr( addr, 0, CONF_DESCR_LEN, 0, bigbuf );   //get configuration descriptor
    if( rcode ) {   //error handling           
        return( FALSE );
    }
    if( data_ptr->descr.config.wTotalLength > 256 ) {
        total_length = 256;
    }
    else {
        total_length = data_ptr->descr.config.wTotalLength;
    }
    rcode = XferGetConfDescr( addr, 0, total_length, 0, bigbuf );   //get the whole configuration
    if( rcode ) {   //error handling
        return( FALSE );
    }
    confvalue = data_ptr->descr.config.bConfigurationValue;
    while( byte_ptr < bigbuf + total_length ) {
        if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_INTERFACE ) {
            byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
            data_ptr = ( USB_DESCR* )byte_ptr;
        }// if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_INTERFACE
        else {  //interface descriptor
            if( data_ptr->descr.interface.bInterfaceClass == HID_INTF &&
                data_ptr->descr.interface.bInterfaceSubClass == BOOT_INTF_SUBCLASS &&
                data_ptr->descr.interface.bInterfaceProtocol == HID_PROTOCOL_MOUSE ) {
                    devtable[ addr ].devclass = HID_M;                  //device class
                    tmpbyte = devtable[ addr ].epinfo->MaxPktSize;
                    HID_init();                                        //initialize data structures
                    devtable[ addr ].epinfo = hid_ep;                   //switch endpoint information structure
                    devtable[ addr ].epinfo[ 0 ].MaxPktSize = tmpbyte;
                    hid_device.interface = data_ptr->descr.interface.bInterfaceNumber;
                    hid_device.addr = addr;
                    byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
                    data_ptr = ( USB_DESCR* )byte_ptr;
                    while( byte_ptr < bigbuf + total_length ) {                
                        if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_ENDPOINT ) {   //skip to endpoint descriptor
                            byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
                            data_ptr = ( USB_DESCR* )byte_ptr;
                        }
                        else {
                            /* fill endpoint information structure */
                            devtable[ addr ].epinfo[ 1 ].epAddr = data_ptr->descr.endpoint.bEndpointAddress;
                            devtable[ addr ].epinfo[ 1 ].Attr = data_ptr->descr.endpoint.bmAttributes;
                            devtable[ addr ].epinfo[ 1 ].MaxPktSize = data_ptr->descr.endpoint.wMaxPacketSize;
                            devtable[ addr ].epinfo[ 1 ].Interval = data_ptr->descr.endpoint.bInterval;
                            // devtable[ addr ].epinfo[ 1 ].rcvToggle = bmRCVTOG0;
                            /* configure device */
                            rcode = XferSetConf( addr, 0, confvalue );  //set configuration
                            if( rcode ) {   //error handling
                                return( FALSE );
                            }
                            rcode = XferSetProto( addr, 0, hid_device.interface, BOOT_PROTOCOL );
                            if( rcode ) {   //error handling
                                return( FALSE );
                            }
                            else {
                                return( TRUE );
                            }
                        }
                    }//while( byte_ptr.... 
            }//if (Class matches
            else { //if class don't match; die on first interface. Not really correct
                return( FALSE );
            }     
        }//else if( data_ptr->
    }// while( byte_ptr < &buf + total_length
    return( FALSE );
}
/* HID Keyboard probe. Called from USB state machine.                           */
/* assumes configuration length is less than 256 bytes                          */
/* looks for Class:03, Subclass: 01, Protocol: 01 in interface descriptor       */
/* sets keyboard in boot protocol                                               */
/* assumes single configuration, single endpoint, and interface configuration 0 */
BOOL HIDKProbe( BYTE addr, DWORD flags )
{
  BYTE tmpbyte;
  BYTE rcode;
  BYTE confvalue;
  WORD total_length;  
  USB_DESCR* data_ptr = ( USB_DESCR * )&bigbuf;
  char* byte_ptr = bigbuf; 
    rcode = XferGetConfDescr( addr, 0, CONF_DESCR_LEN, 0, bigbuf );   //get configuration descriptor
    if( rcode ) {   //error handling           
        return( FALSE );
    }
    if( data_ptr->descr.config.wTotalLength > 256 ) {
        total_length = 256;
    }
    else {
        total_length = data_ptr->descr.config.wTotalLength;
    }
    rcode = XferGetConfDescr( addr, 0, total_length, 0, bigbuf );   //get the whole configuration
    if( rcode ) {   //error handling
        return( FALSE );
    }
    confvalue = data_ptr->descr.config.bConfigurationValue;         //save configuration value to use later
    while( byte_ptr < bigbuf + total_length ) {                     //parse configuration
        if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_INTERFACE ) {  //skip to the next descriptor
            byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
            data_ptr = ( USB_DESCR* )byte_ptr;
        }// if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_INTERFACE
        else {  //interface descriptor
            if( data_ptr->descr.interface.bInterfaceClass == HID_INTF &&
                data_ptr->descr.interface.bInterfaceSubClass == BOOT_INTF_SUBCLASS &&
                data_ptr->descr.interface.bInterfaceProtocol == HID_PROTOCOL_KEYBOARD ) {
                    devtable[ addr ].devclass = HID_K;                                      //fill device class
                    tmpbyte = devtable[ addr ].epinfo->MaxPktSize;                          //save max.packet size
                    HID_init();                                                             //initialize data structures
                    devtable[ addr ].epinfo = hid_ep;                                       //switch endpoint information structure
                    devtable[ addr ].epinfo[ 0 ].MaxPktSize = tmpbyte;                      //fill in max.packet size
                    hid_device.interface = data_ptr->descr.interface.bInterfaceNumber;      //fill in interface number to be used in HID requests
                    hid_device.addr = addr;                                                 //fill in address
                    byte_ptr = byte_ptr + data_ptr->descr.config.bLength;                   //skip to the next descriptor
                    data_ptr = ( USB_DESCR* )byte_ptr;
                    while( byte_ptr < bigbuf + total_length ) {                             
                        if( data_ptr->descr.config.bDescriptorType != USB_DESCRIPTOR_ENDPOINT ) {   //skip to endpoint descriptor
                            byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
                            data_ptr = ( USB_DESCR* )byte_ptr;
                        }
                        else {
                            /* fill endpoint information structure */
                            devtable[ addr ].epinfo[ 1 ].epAddr = data_ptr->descr.endpoint.bEndpointAddress;
                            devtable[ addr ].epinfo[ 1 ].Attr = data_ptr->descr.endpoint.bmAttributes;
                            devtable[ addr ].epinfo[ 1 ].MaxPktSize = data_ptr->descr.endpoint.wMaxPacketSize;
                            devtable[ addr ].epinfo[ 1 ].Interval = data_ptr->descr.endpoint.bInterval;
                            /* configure device */
                            rcode = XferSetConf( addr, 0, confvalue );  //set configuration
                            if( rcode ) {   //error handling
                                return( FALSE );
                            }
                            rcode = XferSetProto( addr, 0, hid_device.interface, BOOT_PROTOCOL );
                            if( rcode ) {   //error handling
                                return( FALSE );
                            }
                            else {
                                return( TRUE );
                            }
                        }
                    }//while( byte_ptr.... 
            }//if (Class matches
            else { //if class don't match; stop processing after first interface. Not really correct
                return( FALSE );
            }     
        }//else if( data_ptr->
    }// while( byte_ptr < &buf + total_length
    return( FALSE );
}
/* HID data structures initialization */
void HID_init( void )
{
    hid_ep[ 1 ].sndToggle = bmSNDTOG0;
    hid_ep[ 1 ].rcvToggle = bmRCVTOG0;
}
/* poll boot mouse */
BYTE mousePoll( BOOT_MOUSE_REPORT* buf )
{
  BYTE rcode;
    MAXreg_wr( rPERADDR, hid_device.addr );    //set peripheral address
    rcode = XferInTransfer( hid_device.addr, 1, 8, ( char* )buf, devtable[ hid_device.addr ].epinfo[ 1 ].MaxPktSize );
    return( rcode );
}
/* poll boot keyboard */
BYTE kbdPoll( BOOT_KBD_REPORT* buf )
{
  BYTE rcode;
    MAXreg_wr( rPERADDR, hid_device.addr );    //set peripheral address
    rcode = XferInTransfer( hid_device.addr, 1, 8, ( char* )buf, devtable[ hid_device.addr ].epinfo[ 1 ].MaxPktSize );
    return( rcode );
}
BOOL HIDMEventHandler( BYTE address, BYTE event, void *data, DWORD size )
{
    return( FALSE );
}
BOOL HIDKEventHandler( BYTE address, BYTE event, void *data, DWORD size )
{
    return( FALSE );
}
