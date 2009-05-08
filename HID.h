/* HID support header */

#ifndef _HID_h_
#define _HID_h

/* HID device structure */
typedef struct {
    BYTE addr;
    BYTE interface;
} HID_DEVICE;
/* Boot mouse report 8 bytes */
typedef struct {
//    struct {
//        unsigned one:1;
//        unsigned two:1;
//        unsigned three:1;
//        unsigned :5;
//        } button;
    BYTE button;        
    BYTE Xdispl;
    BYTE Ydispl;
    BYTE bytes3to7[ 5 ] ;   //optional bytes
} BOOT_MOUSE_REPORT;
/* boot keyboard report 8 bytes */
typedef struct {
    struct {
        unsigned numlock:1;
        unsigned capslock:1;
        unsigned scrolllock:1;
        unsigned compose:1;
        unsigned kana:1;
        unsigned constant:3;
        } mod;
    BYTE reserved;
    BYTE keycode1;
    BYTE keycode2;
    BYTE keycode3;
    BYTE keycode4;
    BYTE keycode5;
    BYTE keycode6;
} BOOT_KBD_REPORT;         
 

/* Function prototypes */
BOOL HIDMProbe( BYTE address, DWORD flags );
void HIDM_init( void );
BYTE mousePoll( BOOT_MOUSE_REPORT* buf );
BOOL HIDMEventHandler( BYTE addr, BYTE event, void *data, DWORD size );

#endif // _HID_h_