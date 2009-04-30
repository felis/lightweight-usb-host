/* USB transfers support header */

#ifndef _transfer_h_
#define _transfer_h_

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

/* Device descriptor structure */
typedef struct {
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // DEVICE descriptor type (USB_DESCRIPTOR_DEVICE).
    union {
        WORD bcdUSB;            // USB Spec Release Number (BCD).
        struct {
            BYTE bcdUSBLo;
            BYTE bcdUSBHi;
        };
    } bcdUSB_u;
    BYTE bDeviceClass;          // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
    BYTE bDeviceSubClass;       // Subclass code (assigned by the USB-IF).
    BYTE bDeviceProtocol;       // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
    BYTE bMaxPacketSize0;       // Maximum packet size for endpoint 0.
    WORD idVendor;              // Vendor ID (assigned by the USB-IF).
    WORD idProduct;             // Product ID (assigned by the manufacturer).
    union {
        WORD bcdDevice;             // Device release number (BCD).
        struct {
            BYTE bcdDeviceLo;
            BYTE bcdDeviceHi;
        };
    }bcdDevice_u;   
    BYTE iManufacturer;         // Index of String Descriptor describing the manufacturer.
    BYTE iProduct;              // Index of String Descriptor describing the product.
    BYTE iSerialNumber;         // Index of String Descriptor with the device's serial number.
    BYTE bNumConfigurations;    // Number of possible configurations.
} USB_DEVICE_DESCRIPTOR;

/* Misc.USB constants */
#define DEV_DESCR_LEN   18      //device descriptor length


/* Targeted peripheral list table */
#define USB_NUMTARGETS 4        //number of targets in TPL, not counting uninitialized device
#define USB_NUMDEVICES 8        //number of supported devices
#define USB_NUMCLASSES 3        //number of device classes in class callback table
#define MSD_DRIVER     0        //Mass storage class driver number in TPL

/* Endpoint information structure               */
/* bToggle of endpoint 0 initialized to 0xff    */
/* during enumeration bToggle is set to 00      */
typedef struct {        
    BYTE Addr;          //copy from endpoint descriptor. Bit 7 indicates direction ( ignored for control endpoints )
    BYTE Attr;          // Endpoint transfer type.
    WORD MaxPktSize;    // Maximum packet size.
    BYTE Interval;      // Polling interval in frames.
    BYTE Toggle;        //last toggle value, may change to a pointer in future versions
} EP_RECORD;

/* targeted peripheral list */
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
    BYTE bConfig;       //configuration
    BYTE numep;         //number of endpoints
    EP_RECORD* epinfo;
    BYTE CltDrv;        //client driver 
}USB_TPL;

/* class driver event handler	*/
typedef BOOL (* rom CLASS_EVENT_HANDLER) ( BYTE address, BYTE event, void *data, DWORD size );

/* class driver initialization */
typedef BOOL (* rom CLASS_INIT)   ( BYTE address, DWORD flags );

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

/* Function macros */

//char XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr )

/* Set address request macro. Human-readable form of bXferCtrlReq   */ 
/* won't necessarily work for device in 'Configured' state          */
#define XferSetAddr( oldaddr, ep, newaddr ) \
        XferCtrlReq( oldaddr,  ep, bmREQ_SET, USB_REQUEST_SET_ADDRESS, newaddr, 0x00, 0x0000, 0x0000, NULL )
///* Set Configuration Request  */
//#define bXferSetConf( addr, ep, confvalue, urb_ptr )      bXferCtrlReq( addr,     ep, 0x0000, ( bmREQ_SET ),          USB_REQUEST_SET_CONFIGURATION,  confvalue,  0x00,                           0x0000, urb_ptr )
///* Get configuration request  */
//#define bXferGetConf( addr, ep, urb_ptr ) bXferCtrlReq( addr, ep, 1, ( bmREQ_GET_DESCR ), USB_REQUEST_GET_CONFIGURATION, 0x00, 0x00, 0x00, urb_ptr );
/* Get device descriptor request macro */
#define XferGetDevDescr( addr, ep, nbytes, dataptr )    \
        XferCtrlReq( addr, ep, bmREQ_GET_DESCR, USB_REQUEST_GET_DESCRIPTOR, 0x00, USB_DESCRIPTOR_DEVICE, 0x0000, nbytes, dataptr )
///* Get configuration descriptor request macro */  
//#define bXferGetConfDescr( addr,ep,nbytes,conf,urb_ptr )  bXferCtrlReq( addr,     ep, nbytes, ( bmREQ_GET_DESCR ),    USB_REQUEST_GET_DESCRIPTOR,     conf,       USB_DESCRIPTOR_CONFIGURATION,   0x0000, urb_ptr )
///* Get string descriptor request macro    */ 
//#define bXferGetStrDescr( addr, ep, nbytes, index, langid, urb_ptr ) bXferCtrlReq( addr, ep, nbytes, ( bmREQ_GET_DESCR ), USB_REQUEST_GET_DESCRIPTOR,     index,      USB_DESCRIPTOR_STRING,          langid, urb_ptr )
///* Get MAX LUN MSD class request macro */
//#define bXferGetMaxLUN( addr, intf, urb_ptr ) bXferCtrlReq( addr, 0, 1, ( bmREQ_CL_GET_INTF ), USB_MSD_GET_MAX_LUN, 0, 0, intf, urb_ptr )



/* Function prototypes */

char XferCtrlReq( BYTE addr, BYTE ep, BYTE bmReqType, BYTE bRequest, BYTE wValLo, BYTE wValHi, WORD wInd, WORD nbytes, char* dataptr );
char XferDispatchPkt( BYTE token, BYTE ep );
char XferInTransfer( BYTE ep, WORD nbytes, BYTE *data, BYTE maxpktsize );
void USB_init( void );
void USB_Task( void );
BYTE Get_UsbTaskState( void );

/* Client driver routines */
BOOL MSDProbe( BYTE address, DWORD flags );
BOOL MSDEventHandler( BYTE address, BYTE event, void *data, DWORD size );
BOOL CDCProbe( BYTE address, DWORD flags );
BOOL CDCEventHandler( BYTE address, BYTE event, void *data, DWORD size );
BOOL DummyProbe( BYTE address , DWORD flags );
BOOL DummyEventHandler( BYTE address, BYTE event, void *data, DWORD size );


#endif //_transfer_h_ 