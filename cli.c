/* CLI functions */

#define _CLI_C_

#include "project_config.h"
#include "cli_constants.h"

extern DWORD uptime;
extern HID_DEVICE hid_device;
extern DEV_RECORD devtable[];


#define NUM_MENUS 6     //number of top-level menus
/* menu position in function pointer array */
static enum {
    CLI_MAIN_MENU = 0,
    CLI_SHOW_MENU = 1,
    CLI_SET_MENU =  2,
    CLI_USBQ_MENU = 3,
    CLI_USBT_MENU = 4,
    CLI_UTIL_MENU = 5
} cli_state = CLI_MAIN_MENU;    //CLI states
/* array of function pointers to top-level menus */ 
static void ( * const rom top_menu[ NUM_MENUS ] )( void ) = {
    CLI_main_menu,
    CLI_show_menu,
    CLI_set_menu,
    CLI_usbq_menu,
    CLI_usbt_menu,
    CLI_util_menu
};

char bigbuf[ 256 ];      
static BYTE devaddr = 1;    //temporary assignment for the only device supported atm     

/* CLI main loop state machine */
void CLI_Task( void )
{
    top_menu[ cli_state ]();    //call top-level menu according to current state
    return;
}

/* root level CLI */
/* switches state to next level menus */    
void CLI_main_menu( void )
{
 BYTE temp;
    
    if(!CharInQueue()) {
        return;
    }
    temp = recvchar();
    if ( temp > 0x1f ) {
        sendchar( temp );                       //echo back
    }
    switch( temp ) {
        case( 0x0d ):                           //"Enter"               
            send_string(cli_prompt_main);
            break;
        case( 0x1b ):                           //"ESC"
            cli_state = CLI_MAIN_MENU;
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                           //"1 - show menu"
            cli_state = CLI_SHOW_MENU;
            send_string(cli_prompt_show);
            break;
        case( 0x32 ):                           //"2 - set menu"
            cli_state = CLI_SET_MENU;
            send_string(cli_prompt_set);
            break;
      case( 0x33 ):                           //"3 - USB queries menu"
          cli_state = CLI_USBQ_MENU;
          send_string(cli_prompt_usbq);
          break;
      case( 0x34 ):                           //"4 - USB transfers menu"
          cli_state = CLI_USBT_MENU;
          send_string(cli_prompt_usbt);
          break;
        case( 0x35 ):                           //"5 - Utilities menu"
            cli_state = CLI_UTIL_MENU;
            send_string(cli_prompt_util);
            break;
        case( 0x3f ):                           //Question mark
            send_string(cli_root_menu_help);
            send_string(cli_prompt_main);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_main);
            break;      
    } //end switch( temp )
}

/* show level CLI */
/* state CLI_SHOW_MENU */
void CLI_show_menu( void )
{
 BYTE i;
 BYTE temp;
 DEV_RECORD* tpl_ptr;
    if(!CharInQueue()) {
        return;
    }
    temp = recvchar();
    if ( temp > 0x1f ) {
        sendchar( temp );                   //echo back
    }
    switch( temp ) {
        case( 0x0d ):                       //"Enter"               
            send_string(cli_prompt_show);
            break;
        case( 0x1b ):                       //"ESC"
            cli_state = CLI_MAIN_MENU;      //change state to "Main"
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                       //"1 - print MAX3421E registers"
            i = 0;
            while( register_format[ i ].name != NULL ) {
                send_string( register_format[i].name );
                send_hexbyte( MAXreg_rd( register_format[i].number ));
                i++;
            }
            send_string(cli_prompt_show);           
            break;
        case( 0x32 ):                       //2 - Print USB task state
            send_string("\r\nUSB task state: ");
            send_hexbyte( GetUsbTaskState());
            send_string(cli_prompt_show);
            break;
        case( 0x33 ):                       //3 - Print device table
            for( i = 1; i < USB_NUMDEVICES; i++ ) {
                tpl_ptr = GetDevtable( i );
                    if( tpl_ptr->epinfo != NULL ) {
                        send_string( crlf );
                        send_string("Device: ");
                        send_decword( i );
                        send_string( devclasses[ tpl_ptr->devclass ] );
                        send_string( crlf );
                    }
            }//for( i = 1; i < USB_NUMDEVICES; i++     
            send_string(cli_prompt_show);
            break;
        case( 0x3f ):                                                   //Question mark
            send_string(cli_show_menu_help);
            send_string(esc_prev_lvl);
            send_string(cli_prompt_show);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_show);
            break;      
    } //end switch( temp )
}

/* SET level CLI */
/* state CLI_SET_MENU */
void CLI_set_menu( void )
{
 BYTE temp;
    if(!CharInQueue()) {
        return;
    }
    temp = recvchar();
    if ( temp > 0x1f ) {
        sendchar( temp );                       //echo back
    }
    switch( temp ) {
        case( 0x0d ):                           //"Enter"               
            send_string(cli_prompt_set);
            break;
        case( 0x1b ):                           //"ESC"
            cli_state = CLI_MAIN_MENU;          //change state to "Main"
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                           //"1" - reset MAX3421E
            MAX3421E_reset();
            send_string ("\r\nMAX3421E reset\r\n");
            break;
        case( 0x32 ):                           //"2" - turn Vbus on
            if (!Vbus_power( ON )) {
                send_string ("\r\nVbus overload");
            }
            else {
                send_string("\r\nVbus is on\r\n");
            }
            break;
        case( 0x33 ):                           //"3" - turn Vbus off
            Vbus_power( OFF );
            send_string("\r\nVbus is off\r\n");
            break;
        case( 0x34 ):                           //HID boot protocol on
            break;
        case( 0x35 ):                           //HID boot protocol off
            break;
        case( 0x3f ):                           //Question mark
            send_string(cli_set_menu_help);
            send_string(esc_prev_lvl);
            send_string(cli_prompt_set);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_set);
            break;      
    } //end switch( temp )
}
/* usb queries menu */
void CLI_usbq_menu( void )
{
  BYTE temp;
    if(!CharInQueue()) {
        return;
    }
    temp = recvchar();
    if ( temp > 0x1f ) {
        sendchar( temp );                       //echo back
    }
    switch( temp ) {
        case( 0x0d ):                           //"Enter"               
            send_string(cli_prompt_usbq);
            break;
        case( 0x1b ):                           //"ESC"
            cli_state = CLI_MAIN_MENU;          //change state to "Main"
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                           //"1" - print device descriptor
            printDevDescr( 1 );
            send_string(cli_prompt_usbq);
            break;
        case( 0x32 ):
            printConfDescr( 1, 0 );             //"2" - print configuration
            send_string(cli_prompt_usbq);
            break;
        case( 0x33 ):                           //"3" - 
            break;
        case( 0x3f ):                           //Question mark
            send_string(cli_usbq_menu_help);
            send_string(esc_prev_lvl);
            send_string(cli_prompt_usbq);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_usbq);
            break;      
    } //end switch( temp )
}
void CLI_usbt_menu( void )
{
  BYTE temp;
    if(!CharInQueue()) {
        return;
    }
    temp = recvchar();
    if ( temp > 0x1f ) {
        sendchar( temp );                       //echo back
    }
    switch( temp ) {
        case( 0x0d ):                           //"Enter"               
            send_string(cli_prompt_usbt);
            break;
        case( 0x1b ):                           //"ESC"
            cli_state = CLI_MAIN_MENU;          //change state to "Main"
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                           //"1" - test mouse
            testMouse( 1 );
            break;
        case( 0x32 ):                           //"2" - test keyboard
            testKbd( 1 );
            break;
        case( 0x33 ):                           //"3" -
            break;
        case( 0x3f ):                           //Question mark
            send_string(cli_usbt_menu_help);
            send_string(esc_prev_lvl);
            send_string(cli_prompt_usbt);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_usbt);
            break;      
    } //end switch( temp )
}
/* util level CLI */
/* state CLI_UTIL_MENU */
void CLI_util_menu( void )
{
 BYTE temp;
    
    if(!CharInQueue())
        return;
        
    temp = recvchar();
    
    if ( temp > 0x1f )
        sendchar( temp );                   //echo back
    
    send_string( cli_prompt_util);
    switch( temp ) {
        case( 0x0d ):                       //"Enter"               
            //send_string(cli_prompt_util);
            break;
        case( 0x1b ):                       //"ESC"
            cli_state = CLI_MAIN_MENU;      //change state to "Main"
            send_string(cli_prompt_main);
            break;
        case( 0x31 ):                       //"1 - test SPI"
            SPI_test();
            send_string( cli_prompt_util );
            break;
        case( 0x32 ):
            //print_registers();                                                    //"2 - show registers"
            //send_string(cli_prompt_show);
            break;
        case( 0x3f ):                                                   //Question mark
            send_string(cli_util_menu_help);
            send_string(esc_prev_lvl);
            send_string(cli_prompt_util);
            break;
        default:
            send_string(cli_invalid_key);
            send_string(cli_prompt_util);
            break;      
    } //end switch( temp )
}

void CLI_init( void )
{
    send_string(cli_banner);
    send_string(cli_root_menu_help);
    send_string(cli_prompt_main);
}
/* prints device descriptor */
void printDevDescr( BYTE addr )
{
 BYTE rcode;
 USB_DEVICE_DESCRIPTOR buf;
    send_string( crlf );
    rcode = XferGetDevDescr( addr, 0, DEV_DESCR_LEN, ( char *)&buf );   //get device descriptor
    if( rcode ) {
        send_string("\r\nDevice descriptor request error. Return code: ");
        send_hexbyte( rcode );
        send_string( crlf );
        return;
    }
    /* First line */
    send_string("Vendor ID: ");
    send_hexbyte( HIBYTE( buf.idVendor ));
    send_hexbyte( LOBYTE( buf.idVendor ));
    send_string(" Product ID: ");
    send_hexbyte( HIBYTE( buf.idProduct ));
    send_hexbyte( LOBYTE( buf.idProduct ));
    send_string(" Rev.: ");
    send_decword(( WORD )HIBYTE( buf.bcdDevice ));
    sendchar('.');
    send_decword(( WORD )LOBYTE( buf.bcdDevice ));
    send_string(" USB version: ");
    send_decword(( WORD )HIBYTE( buf.bcdUSB ));
    sendchar('.');
    send_decword(( WORD )LOBYTE( buf.bcdUSB ));
    /* Second line */
    send_string("\r\nClass: ");
    send_hexbyte( buf.bDeviceClass );
    send_string(" Subclass: ");
    send_hexbyte( buf.bDeviceSubClass );
    send_string(" Protocol: ");
    send_hexbyte( buf.bDeviceProtocol );
    /* Third line */
    send_string("\r\nMax.packet size: ");
    send_decword(( WORD )buf.bMaxPacketSize0 );
    send_string(" bytes. Number of configurations: ");
    send_decword(( WORD )buf.bNumConfigurations );
}
/* Configuraton descriptor parser/printer                                       */
/* Makes sure configuration is less than 256 bytes which may not be the case    */
/* for very complex devices                                                     */
void printConfDescr( BYTE addr, BYTE conf )
{ 
  WORD totallength;
  BYTE rcode;  
  USB_DESCR *data_ptr = ( USB_DESCR * )bigbuf;  //pointer to any descriptor
  char *byte_ptr = bigbuf;
    rcode = XferGetConfDescr( addr, 0, CONF_DESCR_LEN, conf, bigbuf );  //get configuration descriptor
    if( rcode ) {
        send_string("\r\nConfiguration descriptor request error 01. Return code: ");
        send_hexbyte( rcode );
        send_string( crlf );
        return;
    }
    //data_ptr = ( USB_DESCR * )bigbuf;
    totallength = data_ptr->descr.config.wTotalLength;                  //get total length of configuration
    if( totallength > 256 ) {
        send_string("\r\nConfiguration descriptor data length error. Total length: ");
        send_decword( totallength );
        send_string( crlf );
        return;
    }
    rcode = XferGetConfDescr( addr, 0, totallength, conf, bigbuf );     //get the whole configuration
    if( rcode ) {
        send_string("\r\nConfiguration descriptor request error 02. Return code: ");
        send_hexbyte( rcode );
        send_string( crlf );
        return;
    }
    /* Print configuration descriptor */
    send_string("\r\nConfiguration descriptor\r\n");
    /* First line */
    send_string("\r\nTotal configuration size: ");
    send_decword( totallength );
    send_string(" bytes. Max.power: ");
    send_decword( 2 * data_ptr->descr.config.bMaxPower );    //max.value of this field is 250
    send_string(" ma. Number of interfaces: ");
    send_decword( data_ptr->descr.config.bNumInterfaces );
    /* Second line */
    send_string("\r\nAttributes: ");
    if( data_ptr->descr.config.bmAttributes & USB_CFG_DSC_SELF_PWR ) {
        send_string("Self-powered ");
    }
    if( data_ptr->descr.config.bmAttributes & USB_CFG_DSC_REM_WAKE ) {
        send_string("Remote wakeup ");
    }
    send_string(". Configuration value: ");
    send_decword( data_ptr->descr.config.bConfigurationValue );
    send_string( crlf );
    /* Finish printing configuration descriptor. Parse the rest */
    byte_ptr = byte_ptr + CONF_DESCR_LEN;
    while( byte_ptr < ( bigbuf + totallength )) {
        data_ptr = ( USB_DESCR * )byte_ptr;
        switch( data_ptr->descr.config.bDescriptorType ) {
            case( USB_DESCRIPTOR_INTERFACE ):
                printIntrDescr( byte_ptr );
                byte_ptr = byte_ptr + INTR_DESCR_LEN;
                break;
            case( USB_DESCRIPTOR_ENDPOINT ):
                printEpDescr( byte_ptr );
                byte_ptr = byte_ptr + EP_DESCR_LEN;     
                break;
            case( HID_DESCRIPTOR_HID ):                        //HID descriptor is variable length
                printHIDdescr( byte_ptr );
                byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
                break;
            default:                        //unknown descriptor
                send_string("Unknown descriptor: ");
                send_hexbyte( data_ptr->descr.config.bDescriptorType );
                send_string( crlf );
                byte_ptr = byte_ptr + data_ptr->descr.config.bLength;
                break;
        }//switch( data_ptr->swcr.config.bDescriptorType
    }    
}
/* prints interface descriptor */
void printIntrDescr( char *byteptr )
{
  USB_DESCR* dataptr = ( USB_DESCR* )byteptr;     
    send_string("\r\nInterface Descriptor\r\n");
    /* First line */
    send_string("Interface number: ");
    send_decword( dataptr->descr.interface.bInterfaceNumber );
    send_string(" Alternate setting: ");
    send_decword( dataptr->descr.interface.bAlternateSetting );
    send_string(" Number of endpoints: ");
    send_decword( dataptr->descr.interface.bNumEndpoints );
    /* Second line */
    send_string("\r\nClass: ");
    send_hexbyte( dataptr->descr.interface.bInterfaceClass );
    send_string(" Subclass: ");
    send_hexbyte( dataptr->descr.interface.bInterfaceSubClass );
    send_string(" Protocol: ");
    send_hexbyte( dataptr->descr.interface.bInterfaceProtocol );
    send_string( crlf );
}
/* prints endpoint descriptor */
void printEpDescr( char* byteptr )
{
  USB_DESCR* dataptr = ( USB_DESCR* )byteptr;
    send_string("\r\nEndpoint Descriptor\r\n");
    /* First line */
    send_string("Endpoint number: ");
    send_hexbyte( dataptr->descr.endpoint.bEndpointAddress & 0x0f );
    send_string(" Direction: ");
    if( dataptr->descr.endpoint.bEndpointAddress & 0x80 ) {
        send_string("IN");
    }
    else {
        send_string("OUT");
    }
    /* Second line */
    send_string("\r\nAttributes: ");
    send_hexbyte( dataptr->descr.endpoint.bmAttributes );
    send_string(" Max.packet size: ");
    send_decword( dataptr->descr.endpoint.wMaxPacketSize );
    send_string(" Polling interval: ");
    send_decword( dataptr->descr.endpoint.bInterval );
    send_string( crlf );
}
/* prints HID descriptor. Won't parse past first( i.e. report ) descriptor fields, */
/* but will show correct length and descriptor number */
void printHIDdescr( char* byteptr )
{
  USB_DESCR* dataptr = ( USB_DESCR* )byteptr;
    send_string("\r\nHID Descriptor\r\n");
    /* First line */
    send_string("Descriptor length: ");
    send_decword( dataptr->descr.HID.bLength );
    send_string(" bytes. HID version: ");
    send_decword(( WORD )HIBYTE( dataptr->descr.HID.bcdHID ));
    sendchar('.');
    send_decword(( WORD )LOBYTE( dataptr->descr.HID.bcdHID ));
    send_string(" Country code: ");
    send_hexbyte( dataptr->descr.HID.bCountryCode );
    /* Second line */
    send_string("\r\nNumber of descriptors: ");
    send_decword( dataptr->descr.HID.bNumDescriptors );
    /* Third line */
    send_string("\r\nDescriptor type: ");
    send_hexbyte( dataptr->descr.HID.bDescrType );
    send_string(" Descriptor length: ");
    send_decword( dataptr->descr.HID.wDescriptorLength );
    if( dataptr->descr.HID.bLength > 9 ) {
        send_string("Skipping the rest of the descriptor...\r\n");
    }
    send_string( crlf );
}
/* test mouse communication */
/* in boot protocol mode buttons are not reported. Button press, however, generates an update. I can't find boot protocol descrition for mouse, */
/* so maybe it's the way it should be */
void testMouse ( BYTE addr )
{
  DWORD delay;
  BYTE rcode;
  char tmpbyte;
  BOOT_MOUSE_REPORT buf;

    rcode = XferGetIdle( addr, 0, hid_device.interface, 0, &tmpbyte );
    if( rcode ) {   //error handling
        send_string("\r\nGetIdle Error. Error code ");
        send_hexbyte( rcode );
    }
    else {
        send_string("\r\nUpdate rate: ");
        send_decword( tmpbyte );
    }
    send_string("\r\nProtocol: ");
    rcode = XferGetProto( addr, 0, hid_device.interface, &tmpbyte );
    if( rcode ) {   //error handling
        send_string("\r\nGetProto Error. Error code ");
        send_hexbyte( rcode );
    }
    else {
        send_decword( tmpbyte );
        send_string( crlf );
    }
    /* Polling interrupt endpoint */
    while( !CharInQueue() ) {
        delay = uptime + 5;
        while( uptime < delay );    //wait polling interval
        rcode = mousePoll( &buf );
        if( rcode == hrNAK ) {
            continue;
        }
        if( rcode ) {
            send_string("\r\nRcode: ");
            send_hexbyte( rcode );
        }
        send_string("\r\nX displacement: ");
        send_decword( buf.Xdispl );
        send_string(" Y displacement: "); 
        send_decword( buf.Ydispl );
        send_string(" Buttons: ");
        send_hexbyte( buf.button );
    } 
}
void testKbd( BYTE addr )
{
}
    

/* tests SPI transfers for errors. Cycles indefinitely until a key is pressed.  */
/* Prints "." every 64K of transferred data                                     */
void SPI_test( void )
{
 BYTE i, j, gpinpol_copy;
    gpinpol_copy = MAXreg_rd( rGPINPOL );   //save GPINPOL
    send_string("Testing SPI transfers. Press any key to stop.\r\n");
    while( !CharInQueue()) {                //loop until any key is pressed
        for( i = 0; i < 255; i++ ) {
            MAXreg_wr( rGPINPOL, i );
            if( MAXreg_rd( rGPINPOL ) != i ) {
                send_string("SPI transmit/receive mismatch\r\n");
                return;
            }
        }
        j++;
        if ( j == 0 )
            send_string(".");   
    }
    i = recvchar();
    MAXreg_wr( rGPINPOL, gpinpol_copy );
}

//end cli.c
