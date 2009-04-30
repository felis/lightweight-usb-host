/*******************************************************************************

    USB Chapter 9 Protocol (Header File)

Summary:
    This file defines data structures, constants, and macros that are used to
    to support the USB Device Framework protocol described in Chapter 9 of the
    USB 2.0 specification.

Description:
    This file defines data structures, constants, and macros that are used to
    to support the USB Device Framework protocol described in Chapter 9 of the
    USB 2.0 specification.
*/

#ifndef _USB_CH9_H_
#define _USB_CH9_H_



//// *****************************************************************************
///* USB Device Descriptor Structure
//
//This struct defines the structure of a USB Device Descriptor.  Note that this
//structure may need to be packed, or even accessed as bytes, to properly access
//the correct fields when used on some device architectures.
//*/
//typedef struct _USB_DEVICE_DESCRIPTOR {
//    BYTE bLength;               // Length of this descriptor.
//    BYTE bDescriptorType;       // DEVICE descriptor type (USB_DESCRIPTOR_DEVICE).
//    union {
//    	WORD bcdUSB;			// USB Spec Release Number (BCD).
//		struct {
//			BYTE bcdUSBLo;
//			BYTE bcdUSBHi;
//		};
//	} bcdUSB_u;
//    BYTE bDeviceClass;          // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
//    BYTE bDeviceSubClass;       // Subclass code (assigned by the USB-IF).
//    BYTE bDeviceProtocol;       // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
//    BYTE bMaxPacketSize0;       // Maximum packet size for endpoint 0.
//    union {
//    	WORD idVendor;			// Vendor ID (assigned by the USB-IF).
//    	struct {
//    		BYTE idVendorLo;
//    		BYTE idVendorHi;
//    	};
//    }idVendor_u;
//    union { 
//    	WORD idProduct;			// Product ID (assigned by the manufacturer).
//    	struct {
//    		BYTE idProductLo;
//    		BYTE idProductHi;
//    	};
//    }idProduct_u;
//    union {
//	    WORD bcdDevice;             // Device release number (BCD).
//    	struct {
//    		BYTE bcdDeviceLo;
//    		BYTE bcdDeviceHi;
//    	};
//    }bcdDevice_u;	
//    BYTE iManufacturer;         // Index of String Descriptor describing the manufacturer.
//    BYTE iProduct;              // Index of String Descriptor describing the product.
//    BYTE iSerialNumber;         // Index of String Descriptor with the device's serial number.
//    BYTE bNumConfigurations;    // Number of possible configurations.
//} USB_DEVICE_DESCRIPTOR;


// *****************************************************************************
/* USB Configuration Descriptor Structure

This struct defines the structure of a USB Configuration Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_CONFIGURATION_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // CONFIGURATION descriptor type (USB_DESCRIPTOR_CONFIGURATION).
    WORD wTotalLength;          // Total length of all descriptors for this configuration.
    BYTE bNumInterfaces;        // Number of interfaces in this configuration.
    BYTE bConfigurationValue;   // Value of this configuration (1 based).
    BYTE iConfiguration;        // Index of String Descriptor describing the configuration.
    BYTE bmAttributes;          // Configuration characteristics.
    BYTE bMaxPower;             // Maximum power consumed by this configuration.
} USB_CONFIGURATION_DESCRIPTOR;

// Attributes bits
#define USB_CFG_DSC_REQUIRED     0x80                       	// Required attribute
//#define USB_CFG_DSC_SELF_PWR    (0x40|USB_CFG_DSC_REQUIRED) // Device is self powered.
//#define USB_CFG_DSC_REM_WAKE    (0x20|USB_CFG_DSC_REQUIRED) // Device can request remote wakup
#define USB_CFG_DSC_SELF_PWR    (0x40) 							// Device is self powered.
#define USB_CFG_DSC_REM_WAKE    (0x20) 							// Device can request remote wakup


// *****************************************************************************
/* USB Interface Descriptor Structure

This struct defines the structure of a USB Interface Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_INTERFACE_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // INTERFACE descriptor type (USB_DESCRIPTOR_INTERFACE).
    BYTE bInterfaceNumber;      // Number of this interface (0 based).
    BYTE bAlternateSetting;     // Value of this alternate interface setting.
    BYTE bNumEndpoints;         // Number of endpoints in this interface.
    BYTE bInterfaceClass;       // Class code (assigned by the USB-IF).  0xFF-Vendor specific.
    BYTE bInterfaceSubClass;    // Subclass code (assigned by the USB-IF).
    BYTE bInterfaceProtocol;    // Protocol code (assigned by the USB-IF).  0xFF-Vendor specific.
    BYTE iInterface;            // Index of String Descriptor describing the interface.
} USB_INTERFACE_DESCRIPTOR;


// *****************************************************************************
/* USB Endpoint Descriptor Structure

This struct defines the structure of a USB Endpoint Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_ENDPOINT_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // ENDPOINT descriptor type (USB_DESCRIPTOR_ENDPOINT).
    BYTE bEndpointAddress;      // Endpoint address. Bit 7 indicates direction (0=OUT, 1=IN).
    BYTE bmAttributes;          // Endpoint transfer type.
    WORD wMaxPacketSize;        // Maximum packet size.
    BYTE bInterval;             // Polling interval in frames.
} USB_ENDPOINT_DESCRIPTOR;


// Endpoint Direction
#define EP_DIR_IN           0x80    // Data flows from device to host
#define EP_DIR_OUT          0x00    // Data flows from host to device


// ******************************************************************
// USB Endpoint Attributes
// ******************************************************************

// Section: Transfer Types
#define EP_ATTR_CONTROL     (0<<0)  // Endoint used for control transfers
#define EP_ATTR_ISOCH       (1<<0)  // Endpoint used for isochronous transfers
#define EP_ATTR_BULK        (2<<0)  // Endpoint used for bulk transfers
#define EP_ATTR_INTR        (3<<0)  // Endpoint used for interrupt transfers

// Section: Synchronization Types (for isochronous enpoints)
#define EP_ATTR_NO_SYNC     (0<<2)  // No Synchronization
#define EP_ATTR_ASYNC       (1<<2)  // Asynchronous
#define EP_ATTR_ADAPT       (2<<2)  // Adaptive synchronization
#define EP_ATTR_SYNC        (3<<2)  // Synchronous

// Section: Usage Types (for isochronous endpoints)
#define EP_ATTR_DATA        (0<<4)  // Data Endpoint
#define EP_ATTR_FEEDBACK    (1<<4)  // Feedback endpoint
#define EP_ATTR_IMP_FB      (2<<4)  // Implicit Feedback data EP

// Section: Max Packet Sizes
#define EP_MAX_PKT_INTR_LS  8       // Max low-speed interrupt packet
#define EP_MAX_PKT_INTR_FS  64      // Max full-speed interrupt packet
#define EP_MAX_PKT_ISOCH_FS 1023    // Max full-speed isochronous packet
#define EP_MAX_PKT_BULK_FS  64      // Max full-speed bulk packet
#define EP_LG_PKT_BULK_FS   32      // Large full-speed bulk packet
#define EP_MED_PKT_BULK_FS  16      // Medium full-speed bulk packet
#define EP_SM_PKT_BULK_FS   8       // Small full-speed bulk packet


// *****************************************************************************
/* USB OTG Descriptor Structure

This struct defines the structure of a USB OTG Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_OTG_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // OTG descriptor type (USB_DESCRIPTOR_OTG).
    BYTE bmAttributes;          // OTG attributes.
} USB_OTG_DESCRIPTOR;


// ******************************************************************
// Section: USB String Descriptor Structure
// ******************************************************************
// This structure describes the USB string descriptor.  The string
// descriptor provides user-readable information about various aspects of
// the device.  The first string desriptor (string descriptor zero (0)),
// provides a list of the number of languages supported by the set of
// string descriptors for this device instead of an actual string.
//
// Note: The strings are in 2-byte-per-character unicode, not ASCII.
//
// Note: This structure only describes the "header" of the string
// descriptor.  The actual data (either the language ID array or the
// array of unicode characters making up the string, must be allocated
// immediately following this header with no padding between them.

typedef struct _USB_STRING_DSC
{
	BYTE	bLength;			//size of this descriptor
	BYTE	bDescriptorType;	//type, USB_DSC_STRING
	BYTE	bString[64];		//buffer for string			

} USB_STRING_DESCRIPTOR;


// ******************************************************************
// Section: USB Device Qualifier Descriptor Structure
// ******************************************************************
// This structure describes the device qualifier descriptor.  The device
// qualifier descriptor provides overall device information if the device
// supports "other" speeds.
//
// Note: A high-speed device may support "other" speeds (ie. full or low).
// If so, it may need to implement the the device qualifier and other
// speed descriptors.

typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR
{
    BYTE bLength;               // Size of this descriptor
    BYTE bType;                 // Type, always USB_DESCRIPTOR_DEVICE_QUALIFIER
    WORD bcdUSB;                // USB spec version, in BCD
    BYTE bDeviceClass;          // Device class code
    BYTE bDeviceSubClass;       // Device sub-class code
    BYTE bDeviceProtocol;       // Device protocol
    BYTE bMaxPacketSize0;       // EP0, max packet size
    BYTE bNumConfigurations;    // Number of "other-speed" configurations
    BYTE bReserved;             // Always zero (0)

} USB_DEVICE_QUALIFIER_DESCRIPTOR;

/* USB Descriptor buffer structure						*/
/* every descriptor structure is included as a union	*/
typedef struct _USB_DESCRIPTOR_BUFFER {
	BYTE	bLength;			//every descriptor starts with Length and Type
	BYTE	bDescriptorType;
	union {
		BYTE bString[ 254 ];		//buffer for string descriptor
		struct {
			WORD bcdUSB;
			BYTE bDeviceClass;
			BYTE bDeviceSubclass;
			BYTE bDeviceProtocol;
			BYTE bMaxPacketSize0;
			WORD idVendor;
			WORD idProduct;
			WORD bcdDevice;
			BYTE iManufacturer;
			BYTE iProduct;
			BYTE iSerialNumber;
			BYTE bNumConfigurations;
		} device_descr;
		struct {
			WORD wTotalLength;
			BYTE bNumInterfaces;
			BYTE bConfigurationValue;
			BYTE iConfiguration;
			BYTE bmAttributes;
			BYTE bMaxPower;
		} config_descr;
	} desc_u;
	
} USB_DESCRIPTOR_BUFFER;





//typedef struct _DEVICE_INFO {
//	BYTE device_address;
//	BYTE num_config;
//	BYTE ep0_max_pktsize;
//	//BYTE ep0_toggle;
//	BYTE ep_bulkout_number;
//	BYTE ep_bulkout_maxpktsize;
//	BYTE eb_bulkout_toggle;
//	BYTE ep_bulkin_number;
//	BYTE ep_bulkin_maxpktsize;
//	BYTE ep_bulkin_toggle;
//} DEVICE_INFO;


// ******************************************************************
// ******************************************************************
// Section: USB Specification Constants
// ******************************************************************
// ******************************************************************

// Section: Valid PID Values
#define PID_OUT                                 0x1     // PID for an OUT token
#define PID_ACK                                 0x2     // PID for an ACK handshake
#define PID_DATA0                               0x3     // PID for DATA0 data
#define PID_PING                                0x4     // Special PID PING
#define PID_SOF                                 0x5     // PID for a SOF token
#define PID_NYET                                0x6     // PID for a NYET handshake
#define PID_DATA2                               0x7     // PID for DATA2 data
#define PID_SPLIT                               0x8     // Special PID SPLIT
#define PID_IN                                  0x9     // PID for a IN token
#define PID_NAK                                 0xA     // PID for a NAK handshake
#define PID_DATA1                               0xB     // PID for DATA1 data
#define PID_PRE                                 0xC     // Special PID PRE (Same as PID_ERR)
#define PID_ERR                                 0xC     // Special PID ERR (Same as PID_PRE)
#define PID_SETUP                               0xD     // PID for a SETUP token
#define PID_STALL                               0xE     // PID for a STALL handshake
#define PID_MDATA                               0xF     // PID for MDATA data

#define PID_MASK_DATA                           0x03    // Data PID mask
#define PID_MASK_DATA_SHIFTED                  (PID_MASK_DATA << 2) // Data PID shift to proper position

// Section: USB Token Types
/* defined in MAX3421E.h */

// Section: OTG Descriptor Constants

#define OTG_HNP_SUPPORT                         0x02    // OTG Descriptor bmAttributes - HNP support flag
#define OTG_SRP_SUPPORT                         0x01    // OTG Descriptor bmAttributes - SRP support flag

// Section: Endpoint Directions

#define USB_IN_EP                               0x80    // IN endpoint mask
#define USB_OUT_EP                              0x00    // OUT endpoint mask



// Section: USB Class Code Definitions
#define USB_HUB_CLASSCODE                       0x09    //  Class code for a hub.


#endif  // _USB_CH9_H_
/*************************************************************************
 * EOF
 */

