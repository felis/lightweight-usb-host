/* CLI functions */

#define _CLI_C_

#include "project_config.h"
#include "cli_constants.h"

extern DWORD uptime;


#define NUM_MENUS 6     //number of top-level menus
/* menu position in function pointer array */
static enum {
    CLI_MAIN_MENU = 0,
    CLI_SHOW_MENU = 1,
    CLI_SET_MENU =  2,
    CLI_USBQ_MENU = 3,
    CLI_USBT_MENU = 4,
    CLI_UTIL_MENU = 5
} cli_state = CLI_MAIN_MENU;	//CLI states
/* array of function pointers to top-level menus */	
static void ( * const rom menu[ NUM_MENUS ] )( void ) = {
    CLI_main_menu,
    CLI_show_menu,
    CLI_set_menu,
    CLI_usbq_menu,
    CLI_usbt_menu,
    CLI_util_menu
};
    

/* CLI main loop state machine */
void CLI_Task( void )
{
    menu[ cli_state ]();    //call top-level menu according to current state
    return;
}

/* root level CLI */
/* switches state to next level menus */	
void CLI_main_menu( void )
{
	BYTE temp;
	
	if(!CharInQueue())
		return;
		
	temp = recvchar();
	
	if ( temp > 0x1f )
		sendchar( temp );						//echo back
	
	switch( temp ) {
		case( 0x0d ):							//"Enter"				
			send_string(cli_prompt_main);
			break;
		case( 0x1b ):							//"ESC"
			cli_state = CLI_MAIN_MENU;
			send_string(cli_prompt_main);
			break;
		case( 0x31 ):							//"1 - show menu"
			cli_state = CLI_SHOW_MENU;
			send_string(cli_prompt_show);
			break;
		case( 0x32 ):							//"2 - set menu"
			cli_state = CLI_SET_MENU;
			send_string(cli_prompt_set);
			break;
//		case( 0x33 ):							//"3 - USB queries menu"
//			cli_state = CLI_USBQ_MENU;
//			send_string(cli_prompt_usbq);
//			break;
//		case( 0x34 ):							//"4 - USB transfers menu"
//			cli_state = CLI_USBT_MENU;
//			send_string(cli_prompt_usbt);
//			break;
		case( 0x35 ):							//"5 - Utilities menu"
			cli_state = CLI_UTIL_MENU;
			send_string(cli_prompt_util);
			break;
		case( 0x3f ):							//Question mark
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
 BYTE temp,i;
	
	if(!CharInQueue())
		return;
		
	temp = recvchar();
	
	if ( temp > 0x1f )
		sendchar( temp );					//echo back
	
	switch( temp ) {
		case( 0x0d ):						//"Enter"				
			send_string(cli_prompt_show);
			break;
		case( 0x1b ):						//"ESC"
			cli_state = CLI_MAIN_MENU;		//change state to "Main"
			send_string(cli_prompt_main);
			break;
		case( 0x31 ):						//"1 - print MAX3421E registers"
			for( i = 0; i < 23; i++ ) {
				send_string( register_format[i].name );
				send_hexbyte( MAXreg_rd( register_format[i].number ));
			}
			send_string(cli_prompt_show);			
			break;
		case( 0x32 ):						//2 - Print uptime
			uptime_print();
			send_string(cli_prompt_show);
			break;
		case( 0x33 ):						//3 - Print USB task state
			send_string("\r\nUSB task state: ");
			send_hexbyte( Get_UsbTaskState());
			send_string(cli_prompt_show);
			break;
		case( 0x3f ):													//Question mark
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
	
	if(!CharInQueue())
		return;
		
	temp = recvchar();
	
	if ( temp > 0x1f )
		sendchar( temp );						//echo back
	
	switch( temp ) {
		case( 0x0d ):							//"Enter"				
			send_string(cli_prompt_set);
			break;
		case( 0x1b ):							//"ESC"
			cli_state = CLI_MAIN_MENU;			//change state to "Main"
			send_string(cli_prompt_main);
			break;
		case( 0x31 ):							//"1" - reset MAX3421E
			MAX3421E_reset();
			send_string ("\r\nMAX3421E reset\r\n");
			break;
		case( 0x32 ):							//"2" - turn Vbus on
			if (!Vbus_power( ON )) {
				send_string ("\r\nVbus overload");
			}
			else {
				send_string("\r\nVbus is on\r\n");
			}
			break;
		case( 0x33 ):							//"3" - turn Vbus off
			Vbus_power( OFF );
			send_string("\r\nVbus is off\r\n");
			break;
		case( 0x3f ):							//Question mark
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
void CLI_usbq_menu( void )
{
}
void CLI_usbt_menu( void )
{
}
/* show level CLI */
/* state CLI_SHOW_MENU */
void CLI_util_menu( void )
{
 BYTE temp;
	
	if(!CharInQueue())
		return;
		
	temp = recvchar();
	
	if ( temp > 0x1f )
		sendchar( temp );					//echo back
	
	send_string( cli_prompt_util);
	switch( temp ) {
		case( 0x0d ):						//"Enter"				
			//send_string(cli_prompt_util);
			break;
		case( 0x1b ):						//"ESC"
			cli_state = CLI_MAIN_MENU;		//change state to "Main"
			send_string(cli_prompt_main);
			break;
		case( 0x31 ):						//"1 - test SPI"
			SPI_test();
			send_string( cli_prompt_util );
			break;
		case( 0x32 ):
			//print_registers();													//"2 - show registers"
			//send_string(cli_prompt_show);
			break;
		case( 0x3f ):													//Question mark
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
/* prints uptime in human readable format	*/
void uptime_print( void )
{
 BYTE i;
 WORD tmp;
 DWORD uptime_copy;
 	send_string("\r\nSystem uptime is "); 
	uptime_copy = uptime;
	for( i = 0; i < 4; i++) {
		tmp = uptime_copy / uptime_format[ i ].divider;
		if( tmp ) {
			send_decword( tmp );
			send_string( uptime_format[ i ].name );
			uptime_copy = uptime_copy - ( tmp * uptime_format[ i ].divider );
		}
	}
	send_string( crlf );
}
	

/* tests SPI transfers for errors. Cycles indefinitely until a key is pressed.	*/
/* Prints "." every 64K of transferred data										*/
void SPI_test( void )
{
 BYTE i, j, gpinpol_copy;
	gpinpol_copy = MAXreg_rd( rGPINPOL );	//save GPINPOL
	send_string("Testing SPI transfers. Press any key to stop.\r\n");
	while( !CharInQueue()) {				//loop until any key is pressed
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
