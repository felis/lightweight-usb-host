/* USB task support header */

#ifndef _USB_h_
#define _USB_h_


// *****************************************************************************
// Section: State Machine Constants
// *****************************************************************************


/* States are defined by 4 high bits
	Substates are defined by 4 low bits */


#define USB_STATE_MASK                                      0xf0  //
#define USB_SUBSTATE_MASK                                   0x0f  //

#define SUBSUBSTATE_MASK                                0x000F  //

#define NEXT_STATE                                      0x0100  //
#define NEXT_SUBSTATE                                   0x0010  //
#define NEXT_SUBSUBSTATE                                0x0001  //

#define SUBSUBSTATE_ERROR                               0x000F  //

#define NO_STATE                                        0xFFFF  //

/*
*******************************************************************************
DETACHED state machine values

This state machine handles the condition when no device is attached.
*/

#define USB_STATE_DETACHED									0x00
#define USB_DETACHED_SUBSTATE_INITIALIZE					0x01
#define USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE				0x03
#define	USB_DETACHED_SUBSTATE_ILLEGAL						0x04


/*
*******************************************************************************
ATTACHED state machine values

This state machine gets the device descriptor of the remote device.  We get the
size of the device descriptor, and use that size to get the entire device
descriptor.  Then we check the VID and PID and make sure they appear in the TPL.
*/

#define USB_STATE_ATTACHED										0x10
#define USB_ATTACHED_SUBSTATE_RESET_DEVICE						0x11
#define USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE				0x12
#define USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE        0x13
#define USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR				0x14
#define USB_ATTACHED_SUBSTATE_VALIDATE_VID_PID					0x15

/*
*******************************************************************************
ADDRESSING state machine values

This state machine sets the address of the remote device.
*/

#define USB_STATE_ADDRESSING							0x20
/*
*******************************************************************************
CONFIGURING state machine values

This state machine sets the configuration of the remote device, and sets up
internal variables to support the device.
*/
#define USB_STATE_CONFIGURING							0x30

/*
*******************************************************************************
RUNNING state machine values

*/

#define USB_STATE_RUNNING									0x40
//#define RUNNING_SUBSTATE_NORMAL_RUN							0x0000  //
//#define RUNNING_SUBSTATE_SUSPEND_AND_RESUME					0x0010  //
//#define RUNNING_SUBSUBSTATE_SUSPEND							0x0000  //
//#define RUNNING_SUBSUBSTATE_RESUME							0x0001  //
//#define RUNNING_SUBSUBSTATE_RESUME_WAIT						0x0002  //
//#define RUNNING_SUBSUBSTATE_RESUME_RECOVERY					0x0003  //
//#define RUNNING_SUBSUBSTATE_RESUME_RECOVERY_WAIT			0x0004  //
//#define RUNNING_SUBSUBSTATE_RESUME_COMPLETE					0x0005  //


/*
*******************************************************************************
HOLDING state machine values

*/

#define STATE_HOLDING                                   0x50  //

#define HOLDING_SUBSTATE_HOLD_INIT                              0x0000  //
#define HOLDING_SUBSTATE_HOLD                                   0x0001  //

/* Error state machine state. Non-recoverable */

#define USB_STATE_ERROR							0xff								

// *****************************************************************************
// Section: Token State Machine Constants
// *****************************************************************************

#define TSTATE_MASK                             0x00F0  //
#define TSUBSTATE_MASK                          0x000F  //

#define TSUBSTATE_ERROR                         0x000F  //

#define TSTATE_IDLE                             0x0000  //

#define TSTATE_CONTROL_NO_DATA                  0x0010  //
#define TSUBSTATE_CONTROL_NO_DATA_SETUP         0x0000  //
#define TSUBSTATE_CONTROL_NO_DATA_ACK           0x0001  //
#define TSUBSTATE_CONTROL_NO_DATA_COMPLETE      0x0002  //

#define TSTATE_CONTROL_READ                     0x0020  //
#define TSUBSTATE_CONTROL_READ_SETUP            0x0000  //
#define TSUBSTATE_CONTROL_READ_DATA             0x0001  //
#define TSUBSTATE_CONTROL_READ_ACK              0x0002  //
#define TSUBSTATE_CONTROL_READ_COMPLETE         0x0003  //

#define TSTATE_CONTROL_WRITE                    0x0030  //
#define TSUBSTATE_CONTROL_WRITE_SETUP           0x0000  //
#define TSUBSTATE_CONTROL_WRITE_DATA            0x0001  //
#define TSUBSTATE_CONTROL_WRITE_ACK             0x0002  //
#define TSUBSTATE_CONTROL_WRITE_COMPLETE        0x0003  //

#define TSTATE_INTERRUPT_READ                   0x0040  //
#define TSUBSTATE_INTERRUPT_READ_DATA           0x0000  //
#define TSUBSTATE_INTERRUPT_READ_COMPLETE       0x0001  //

#define TSTATE_INTERRUPT_WRITE                  0x0050  //
#define TSUBSTATE_INTERRUPT_WRITE_DATA          0x0000  //
#define TSUBSTATE_INTERRUPT_WRITE_COMPLETE      0x0001  //

#define TSTATE_ISOCHRONOUS_READ                 0x0060  //
#define TSUBSTATE_ISOCHRONOUS_READ_DATA         0x0000  //
#define TSUBSTATE_ISOCHRONOUS_READ_COMPLETE     0x0001  //

#define TSTATE_ISOCHRONOUS_WRITE                0x0070  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_DATA        0x0000  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_COMPLETE    0x0001  //

#define TSTATE_BULK_READ                        0x0080  //
#define TSUBSTATE_BULK_READ_DATA                0x0000  //
#define TSUBSTATE_BULK_READ_COMPLETE            0x0001  //

#define TSTATE_BULK_WRITE                       0x0090  //
#define TSUBSTATE_BULK_WRITE_DATA               0x0000  //
#define TSUBSTATE_BULK_WRITE_COMPLETE           0x0001  //

// ************************
// Standard USB Requests
#define SR_GET_STATUS			0x00	// Get Status
#define SR_CLEAR_FEATURE		0x01	// Clear Feature
#define SR_RESERVED				0x02	// Reserved
#define SR_SET_FEATURE			0x03	// Set Feature
#define SR_SET_ADDRESS			0x05	// Set Address
#define SR_GET_DESCRIPTOR		0x06	// Get Descriptor
#define SR_SET_DESCRIPTOR		0x07	// Set Descriptor
#define SR_GET_CONFIGURATION	0x08	// Get Configuration
#define SR_SET_CONFIGURATION	0x09	// Set Configuration
#define SR_GET_INTERFACE		0x0a	// Get Interface
#define SR_SET_INTERFACE		0x0b	// Set Interface

// Get Descriptor codes	
#define GD_DEVICE			0x01	// Get device descriptor: Device
#define GD_CONFIGURATION	0x02	// Get device descriptor: Configuration
#define GD_STRING			0x03	// Get device descriptor: String
#define GD_HID	            0x21	// Get descriptor: HID
#define GD_REPORT	        0x22	// Get descriptor: Report

// HID bRequest values
#define GET_REPORT		1
#define GET_IDLE		2
#define GET_PROTOCOL    3
#define SET_REPORT		9
#define SET_IDLE		0x0A
#define SET_PROTOCOL    0x0B
#define INPUT_REPORT    1


//******************************************************************************
//******************************************************************************
// Section: Macros
//
// These macros are all internal to the host layer.
//******************************************************************************
//******************************************************************************

#define _USB_InitErrorCounters()        { numCommandTries   = USB_NUM_COMMAND_TRIES; }
#define _USB_SetDATA01(x)               { pCurrentEndpoint->status.bfNextDATA01 = x; }
#define _USB_SetErrorCode(x)            { usbDeviceInfo.errorCode = x; }
#define _USB_SetHoldState()             { usbHostState = STATE_HOLDING; }
#define _USB_SetNextState()             { usbHostState = (usbHostState & STATE_MASK) + NEXT_STATE; }
#define _host_tasks_SetNextSubState()   { host_tasks_state =( host_tasks_state & (STATE_MASK | SUBSTATE_MASK)) + NEXT_SUBSTATE; }
#define _USB_SetNextSubSubState()       { usbHostState =  usbHostState + NEXT_SUBSUBSTATE; }
#define _USB_SetNextTransferState()     { pCurrentEndpoint->transferState ++; }
#define _USB_SetPreviousSubSubState()   { usbHostState =  usbHostState - NEXT_SUBSUBSTATE; }
#define _USB_SetTransferErrorState(x)   { x->transferState = (x->transferState & TSTATE_MASK) | TSUBSTATE_ERROR; }
#define freez(x)                        { free(x); x = NULL; }

/* data structures */

/* inspired by Linux URB */
/* number of bytes in URB and another in setup packet - redundant */
typedef struct _USB_REQUEST_BLOCK {
	BYTE addr;						//peripheral address
	BYTE ep;						//endpoint
	WORD nbytes;					//bytes to transfer
	BYTE *status_ptr;				//completion status
	// xQueueHandle USBdataQ;			//transfer data queue
	struct {
    	union {							// offset   description
        	BYTE bmRequestType;			//   0      Bit-map of request type
        	struct {
            	BYTE    recipient:  5;	//          Recipient of the request
            	BYTE    type:       2;	//          Type of request
            	BYTE    direction:  1;	//          Direction of data X-fer
        		};
    	}reqtype_u;
		BYTE	bRequest;				//   1      Request
		union {
    		WORD	wValue;				//   2      Depends on bRequest
    		struct {
    			BYTE	wValueLo;
    			BYTE	wValueHi;
    		};
		}wValue_u;
    	WORD	wIndex;					//   4      Depends on bRequest
    	WORD	wLength;				//   6      Depends on bRequest
	}setup_pkt;
}URB;

//#define STATUS_DATA_READY	0x01
//#define STATUS_XFER_DONE	0x00
//#define ERROR				0x80
//#define UNDEFINED			0xff

/* Endpoint information structure 				*/
/* bToggle of endpoint 0 initialized to 0xff	*/
/* during enumeration bToggle is set to 00		*/
typedef struct _EP_RECORD {		
	BYTE bEndpointAddress;		//copy from endpoint descriptor. Bit 7 indicates direction ( ignored for control endpoints )
	BYTE bmAttributes;      	// Endpoint transfer type.
    WORD wMaxPacketSize;    	// Maximum packet size.
    BYTE bInterval;         	// Polling interval in frames.
    BYTE bToggle;				//last toggle value, may change to a pointer in future versions
} EP_RECORD;

/* Device information structure */
typedef struct _DEV_RECORD {			
	// WORD idVendor;					//VID
	// WORD idProduct;					//PID
	BYTE config;
	EP_RECORD *eplist_ptr;				//pointer to the list of endpoint parameters
} DEV_RECORD;

/* targeted peripheral list */
typedef struct _USB_TPL {
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
	}device_u;
	BYTE bConfiguration;
	BYTE ClientDriver;
}USB_TPL;

// *****************************************************************************
/* USB Mass Storage Device Information

This structure is used to hold all the information about an attached Mass Storage device.
*/
typedef struct _USB_MSD_DEVICE_INFO
{
    BYTE                                blockData[31];          // Data buffer for device communication.
    BYTE                                deviceAddress;          // Address of the device on the bus.
    BYTE                                errorCode;              // Error code of last error.
    BYTE                                state;                  // State machine state of the device.
    BYTE                                returnState;            // State to return to after performing error handling.
    union
    {
        struct
        {
            BYTE                        bfDirection     : 1;    // Direction of current transfer (0=OUT, 1=IN).
            BYTE                        bfReset         : 1;    // Flag indicating to perform Mass Storage Reset.
            BYTE                        bfClearDataIN   : 1;    // Flag indicating to clear the IN endpoint.
            BYTE                        bfClearDataOUT  : 1;    // Flag indicating to clear the OUT endpoint.
        };
        BYTE                            val;
    }                                   flags;
    BYTE                                maxLUN;                 // The maximum Logical Unit Number of the device.
    BYTE                                interface;              // Interface number we are using.
    BYTE                                epin_idx;             	// Bulk IN endpoint index in devinfo.epinfo_ptr[].
    BYTE                                epout_idx;            	// Bulk OUT endpoint index in devinfo.epinfo_ptr[].
    BYTE                                endpointDATA;           // Endpoint to use for the current transfer.
    BYTE                                *userData;              // Pointer to the user's data buffer.
    DWORD                               userDataLength;         // Length of the user's data buffer.
    DWORD                               bytesTransferred;       // Number of bytes transferred to/from the user's data buffer.
    DWORD                               dCBWTag;                // The value of the dCBWTag to verify against the dCSWtag.
    BYTE                                attemptsCSW;            // Number of attempts to retrieve the CSW.
} USB_MSD_DEVICE_INFO;

/* class driver event handler	*/
typedef BOOL (* rom CLASS_EVENT_HANDLER) ( BYTE address, BYTE event, void *data, DWORD size );

/* class driver initialization */
typedef BOOL (* rom CLASS_INIT)   ( BYTE address, DWORD flags );

// *****************************************************************************
/* Client Driver Table Structure

 */

typedef struct _CLASS_CALLBACK_TABLE
{
    CLASS_INIT          Initialize;     // Initialization routine
    CLASS_EVENT_HANDLER EventHandler;   // Event routine
    DWORD                    flags;          // Initialization flags

} CLASS_CALLBACK_TABLE;

//* Functions	*/
//
//void vUSBtask_init( void );
//void vUSB_task( void *pvParameters );
//void prvUSBdata_init( void );
//char bUSB_Control_Write_ND( BYTE addr, BYTE ep );
//char bUSB_Control_Read( BYTE addr, BYTE ep );
//char bUSB_IN_Transfer( BYTE ep, WORD nbytes, BYTE maxpktsize, BYTE * data );
//char bUSB_Dispatch_Pkt( BYTE token, BYTE ep );
//BOOL prvMSDInit( BYTE address, DWORD flags );
//BOOL prvMSDEventHandler( BYTE address, BYTE event, void *data, DWORD size );
//BOOL prvCDCProbe( BYTE address, DWORD flags );
//BOOL prvCDCEventHandler( BYTE address, BYTE event, void *data, DWORD size );
//BOOL prvDummyProbe( BYTE address , DWORD flags );
//BOOL prvDummyEventHandler( BYTE address, BYTE event, void *data, DWORD size );
//BYTE flush_Q( xQueueHandle QueueH );


#endif //_USB_h_