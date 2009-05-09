/* cli.c support file */

#ifndef _cli_h_
#define _cli_h_

//#include "GenericTypeDefs.h"

/* Function declarations */ 

void CLI_init( void );
void CLI_Task( void );
void CLI_main_menu( void );
void CLI_show_menu( void );
void CLI_set_menu( void );
void CLI_usbq_menu( void );
void CLI_usbt_menu( void );
void CLI_util_menu( void );
/**/
void printDevDescr( BYTE addr );
void printConfDescr( BYTE addr, BYTE conf );
void printIntrDescr( char *byteptr );
void printEpDescr( char *byteptr );
void printHIDdescr( char *byteptr );
void testMouse ( BYTE addr );
void testKbd( BYTE addr );
// prevCodeComp( BYTE data, BOOT_KBD_REPORT* buf );
void SPI_test( void );

#endif // _cli_h_
