/* USB transfers support header */

#ifndef _transfer_h_
#define _transfer_h_

/* Targeted peripheral list table */
#define USB_NUMTARGETS  4       //number of targets in TPL, not counting uninitialized device
#define USB_NUMDEVICES  8       //number of supported devices
#define USB_NUMCLASSES  3       //number of device classes in class callback table
#define UNINIT          0       //uninitialized
#define HID_K           1       //HID Keyboard driver number in DEV_RECORD
#define HID_M           2       //HID Mouse driver number in DEV_RECORD
#define MSD             3       //Mass storage class driver number in DEV_RECORD


/* Standard Device Requests */

#define USB_REQUEST_GET_STATUS                  0       // Standard Device Request - GET STATUS
#define USB_REQUEST_CLEAR_FEATURE               1       // Standard Device Request - CLEAR FEATURE
#define USB_REQUEST_SET_FEATURE                 3       // Standard Device Request - SET FEATURE
#define USB_REQUEST_SET_ADDRESS                 5       // Standard Device Request - SET ADDRESS
#define USB_REQUEST_GET_DESCRIPTOR              6       // Standard Device Request - GET DESCRIPTOR
#define USB_REQUEST_SET_DESCRIPTOR              7       // Standard Device Request - SET DESCRIPTOR
#define USB_REQUEST_GET_CONFIGURATION           8       // Standard Device Request - GET CONFIGURATION
#define USB_REQUEST_SET_CONFIGURATION           9       // Standard Device Request - SET CONFIGURATION
#define USB_REQUEST_GET_INTERFACE               10      // Standard Device Request - GET INTERFACE
#define USB_REQUEST_SET_INTERFACE               11      // Standard Device Request - SET INTERFACE
#define USB_REQUEST_SYNCH_FRAME                 12      // Standard Device Request - SYNCH FRAME

#define USB_FEATURE_ENDPOINT_HALT               0       // CLEAR/SET FEATURE - Endpoint Halt
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP        1       // CLEAR/SET FEATURE - Device remote wake-up
#define USB_FEATURE_TEST_MODE                   2       // CLEAR/SET FEATURE - Test mode

/* Setup Data Constants */

#define USB_SETUP_HOST_TO_DEVICE                0x00    // Device Request bmRequestType transfer direction - host to device transfer
#define USB_SETUP_DEVICE_TO_HOST                0x80    // Device Request bmRequestType transfer direction - device to host transfer
#define USB_SETUP_TYPE_STANDARD                 0x00    // Device Request bmRequestType type - standard
#define USB_SETUP_TYPE_CLASS                    0x20    // Device Request bmRequestType type - class
#define USB_SETUP_TYPE_VENDOR                   0x40    // Device Request bmRequestType type - vendor
#define USB_SETUP_RECIPIENT_DEVICE              0x00    // Device Request bmRequestType recipient - device
#define USB_SETUP_RECIPIENT_INTERFACE           0x01    // Device Request bmRequestType recipient - interface
#define USB_SETUP_RECIPIENT_ENDPOINT            0x02    // Device Request bmRequestType recipient - endpoint
#define USB_SETUP_RECIPIENT_OTHER               0x03    // Device Request bmRequestType recipient - other

/* USB descriptors  */

#define USB_DESCRIPTOR_DEVICE           0x01    // bDescriptorType for a Device Descriptor.
#define USB_DESCRIPTOR_CONFIGURATION    0x02    // bDescriptorType for a Configuration Descriptor.
#define USB_DESCRIPTOR_STRING           0x03    // bDescriptorType for a String Descriptor.
#define USB_DESCRIPTOR_INTERFACE        0x04    // bDescriptorType for an Interface Descriptor.
#define USB_DESCRIPTOR_ENDPOINT         0x05    // bDescriptorType for an Endpoint Descriptor.
#define USB_DESCRIPTOR_DEVICE_QUALIFIER 0x06    // bDescriptorType for a Device Qualifier.
#define USB_DESCRIPTOR_OTHER_SPEED      0x07    // bDescriptorType for a Other Speed Configuration.
#define USB_DESCRIPTOR_INTERFACE_POWER  0x08    // bDescriptorType for Interface Power.
#define USB_DESCRIPTOR_OTG              0x09    // bDescriptorType for an OTG Descriptor.

/* OTG SET FEATURE Constants    */
#define OTG_FEATURE_B_HNP_ENABLE                3       // SET FEATURE OTG - Enable B device to perform HNP
#define OTG_FEATURE_A_HNP_SUPPORT               4       // SET FEATURE OTG - A device supports HNP
#define OTG_FEATURE_A_ALT_HNP_SUPPORT           5       // SET FEATURE OTG - Another port on the A device supports HNP

/* USB Endpoint Transfer Types  */
#define USB_TRANSFER_TYPE_CONTROL               0x00    // Endpoint is a control endpoint.
#define USB_TRANSFER_TYPE_ISOCHRONOUS           0x01    // Endpoint is an isochronous endpoint.
#define USB_TRANSFER_TYPE_BULK                  0x02    // Endpoint is a bulk endpoint.
#define USB_TRANSFER_TYPE_INTERRUPT             0x03    // Endpoint is an interrupt endpoint.
#define bmUSB_TRANSFER_TYPE                     0x03    // bit mask to separate transfer type from ISO attributes


/* Standard Feature Selectors for CLEAR_FEATURE Requests    */
#define USB_FEATURE_ENDPOINT_STALL              0       // Endpoint recipient
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP        1       // Device recipient
#define USB_FEATURE_TEST_MODE                   2       // Device recipient

/* MSD class requests. Not part of chapter 9    */        
#define USB_MSD_GET_MAX_LUN                 0xFE            // Device Request code to get the maximum LUN.
#define USB_MSD_RESET                       0xFF            // Device Request code to reset the device.

/* HID constants. Not part of chapter 9 */
/* Class-Specific Requests */
#define HID_REQUEST_GET_REPORT      0x01
#define HID_REQUEST_GET_IDLE        0x02
#define HID_REQUEST_GET_PROTOCOL    0x03
#define HID_REQUEST_SET_REPORT      0x09
#define HID_REQUEST_SET_IDLE        0x0A
#define HID_REQUEST_SET_PROTOCOL    0x0B

/* Class Descriptor Types */
#define HID_DESCRIPTOR_HID      0x21
#define HID_DESCRIPTOR_REPORT   0x22
#define HID_DESRIPTOR_PHY       0x23

/* Protocol Selection */
#define BOOT_PROTOCOL   0x00
#define RPT_PROTOCOL    0x01
/* HID Interface Class Code */
#define HID_INTF                    0x03
/* HID Interface Class SubClass Codes */
#define BOOT_INTF_SUBCLASS          0x01
/* HID Interface Class Protocol Codes */
#define HID_PROTOCOL_NONE           0x00
#define HID_PROTOCOL_KEYBOAD        0x01
#define HID_PROTOCOL_MOUSE          0x02




/* USB Setup Packet Structure   */
typedef struct {
    union {                          // offset   description
        BYTE bmRequestType;         //   0      Bit-map of request type
        struct {
            BYTE    recipient:  5;  //          Recipient of the request
            BYTE    type:       2;  //          Type of request
            BYTE    direction:  1;  //          Direction of data X-fer
        };
    }ReqType_u;
    BYTE    bRequest;               //   1      Request
    union {
        WORD    wValue;             //   2      Depends on bRequest
        struct {
        BYTE    wValueLo;
        BYTE    wValueHi;
        };
    }wVal_u;
    WORD    wIndex;                 //   4      Depends on bRequest
    WORD    wLength;                //   6      Depends on bRequest
} SETUP_PKT, *PSETUP_PKT;

/* Endpoint information structure               */
/* bToggle of endpoint 0 initialized to 0xff    */
/* during enumeration bToggle is set to 00      */
typedef struct {        
    BYTE epAddr;        //copy from endpoint descriptor. Bit 7 indicates direction ( ignored for control endpoints )
    BYTE Attr;          // Endpoint transfer type.
    WORD MaxPktSize;    // Maximum packet size.
    BYTE Interval;      // Polling interval in frames.
    BYTE sndToggle;     //last toggle value, bitmask for HCTL toggle bits
    BYTE rcvToggle;     //last toggle value, bitmask for HCTL toggle bits
    /* not sure if both are necessary */
} EP_RECORD;
/* device record structure */
typedef struct {
    EP_RECORD* epinfo;      //device endpoint information
    BYTE devclass;          //device class    
} DEV_RECORD;


/* targeted peripheral list element */
typedef struct {
    union {
        DWORD val;
        struct {
            WORD idVendor;
            WORD idProduct;
        };
        struct {
            BYTE bClass;
            BYTE bSubClass;
            BYTE bProtocol;
        };
    }dev_u;
    BYTE bConfig;               //configuration
    BYTE numep;                 //number of endpoints
    EP_RECORD* epinfo;          //endpoint information structure
    BYTE CltDrv;                //client driver
    const rom char * desc;      //device description
}USB_TPL_ENTRY;
/* control transfer */
typedef BYTE (* CTRL_XFER )( BYTE addr, BYTE ep, WORD nbytes, char* dataptr, BOOL direction );
/* class driver initialization */
typedef BOOL (* rom CLASS_INIT)( BYTE address, DWORD flags );
/* class driver event handler	*/
typedef BOOL (* rom CLASS_EVENT_HANDLER)( BYTE address, BYTE event, void *data, DWORD size );
/* Client Driver Table Structure */
typedef struct {
    CLASS_INIT          Initialize;     // Initialization routine
    CLASS_EVENT_HANDLER EventHandler;   // Event routine
    DWORD                    flags;     // Initialization flags
} CLASS_CALLBACK_TABLE;

/* Common setup data constant combinations  */
#define bmREQ_GET_DESCR     USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     //get descriptor request type
#define bmREQ_SET           USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_STANDARD|USB_SETUP_RECIPIENT_DEVICE     //set request type for all but 'set feature' and 'set interface'
#define bmREQ_CL_GET_INTF   USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE     //get interface request type

#define bmREQ_HIDOUT        USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE
#define bmREQ_HIDIN         USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE 

/* Function macros */

//char XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr )

/* Set address request macro. Human-readable form of bXferCtrlReq   */ 
/* won't necessarily work for device in 'Configured' state          */
#define XferSetAddr( oldaddr, ep, newaddr ) \
        XferCtrlReq( oldaddr,  ep, bmREQ_SET, USB_REQUEST_SET_ADDRESS, newaddr, 0x00, 0x0000, 0x0000, NULL )
/* Set Configuration Request  */
#define XferSetConf( addr, ep, conf_value )  \
        XferCtrlReq( addr, ep, bmREQ_SET, USB_REQUEST_SET_CONFIGURATION, conf_value, 0x00, 0x0000, 0x0000, NULL )
///* Get configuration request  */
//#define bXferGetConf( addr, ep, urb_ptr ) bXferCtrlReq( addr, ep, 1, ( bmREQ_GET_DESCR ), USB_REQUEST_GET_CONFIGURATION, 0x00, 0x00, 0x00, urb_ptr );
/* Get device descriptor request macro */
#define XferGetDevDescr( addr, ep, nbytes, dataptr )    \
        XferCtrlReq( addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, 0x00, USB_DESCRIPTOR_DEVICE, 0x0000, nbytes, dataptr )
///* Get configuration descriptor request macro */  
#define XferGetConfDescr( addr, ep, nbytes, conf, dataptr )  \
        XferCtrlReq( addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, conf, USB_DESCRIPTOR_CONFIGURATION, 0x0000, nbytes, dataptr )
///* Get string descriptor request macro    */ 
//#define bXferGetStrDescr( addr, ep, nbytes, index, langid, urb_ptr ) bXferCtrlReq( addr, ep, nbytes, ( bmREQ_GET_DESCR ), USB_REQUEST_GET_DESCRIPTOR,     index,      USB_DESCRIPTOR_STRING,          langid, urb_ptr )
///* Get MAX LUN MSD class request macro */
//#define bXferGetMaxLUN( addr, intf, urb_ptr ) bXferCtrlReq( addr, 0, 1, ( bmREQ_CL_GET_INTF ), USB_MSD_GET_MAX_LUN, 0, 0, intf, urb_ptr )
/* class requests */
#define XferSetProto( addr, ep, interface, protocol ) \
        XferCtrlReq( addr, ep, bmREQ_HIDOUT, HID_REQUEST_SET_PROTOCOL, protocol, 0x00, interface, 0x0000, NULL )
#define XferGetProto( addr, ep, interface, dataptr ) \
        XferCtrlReq( addr, ep, bmREQ_HIDIN, HID_REQUEST_GET_PROTOCOL, 0x00, 0x00, interface, 0x0001, dataptr )        
#define XferGetIdle( addr, ep, interface, reportID, dataptr ) \
        XferCtrlReq( addr, ep, bmREQ_HIDIN, HID_REQUEST_GET_IDLE, reportID, 0, interface, 0x0001, dataptr )       



/* Function prototypes */

BYTE XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr );
BYTE XferCtrlData( BYTE addr, BYTE ep, WORD nbytes, char* dataptr, BOOL direction );
BYTE XferCtrlND( BYTE addr, BYTE ep, WORD nbytes, char* dataptr, BOOL direction );
//BYTE startCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr );
BYTE XferDispatchPkt( BYTE token, BYTE ep );
BYTE XferInTransfer( BYTE addr, BYTE ep, WORD nbytes, char* data, BYTE maxpktsize );
//BYTE XferInTransfer_mps( BYTE ep, char* data, BYTE maxpktsize );
void USB_init( void );
void USB_Task( void );
BYTE GetUsbTaskState( void );
DEV_RECORD* GetDevtable( BYTE index );

/* Client driver routines */
BOOL MSDProbe( BYTE address, DWORD flags );
BOOL MSDEventHandler( BYTE address, BYTE event, void *data, DWORD size );
BOOL CDCProbe( BYTE address, DWORD flags );
BOOL CDCEventHandler( BYTE address, BYTE event, void *data, DWORD size );
BOOL DummyProbe( BYTE address , DWORD flags );
BOOL DummyEventHandler( BYTE address, BYTE event, void *data, DWORD size );


#endif //_transfer_h_ 