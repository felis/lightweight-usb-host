/* strings and other constants for cli.c module	*/

#ifndef _cli_constants_h_
#define _cli_constants_h_

const rom char *const rom cli_banner =
"\r\nCircuits At Home"
"\r\n\nLightweight USB Host serial console\r\n"
"\r\n";

const rom char *const rom cli_root_menu_help =
"\r\n1 - Show device parameters\r\n"
"2 - Set device parameters\r\n"
"3 - USB queries\r\n"
"4 - USB transfers\r\n"
"5 - Utilities\r\n";

const rom char *const rom cli_show_menu_help =
"\r\n1 - print MAX3421E Registers\r\n"
"2 - Show USB task state\r\n"
"3 - Show devices\r\n";

const rom char *const rom cli_set_menu_help =
"\r\n1 - Reset MAX3421E\r\n"
"2 - turn Vbus on\r\n"
"3 - turn Vbus off\r\n"
"4 - set HID boot protocol\r\n"
"5 - set HID report protocol\r\n";

const rom char *const rom cli_usbq_menu_help =
"\r\n1 - print device descriptor\r\n"
"2 - print configuration\r\n"
"3 - Item 3\r\n";

const rom char *const rom cli_usbt_menu_help =
"\r\n1 - test mouse communication\r\n"
"2 - test keyboard communication\r\n"
"3 - Item 3\r\n";

const rom char *const rom cli_util_menu_help =
"\r\n1 - Test SPI transfers\r\n"
"2 - Util menu item 2\r\n"
"3 - Util menu item 3\r\n";

const rom char *const rom crlf = "\r\n";
const rom char *const rom angle = ">";
const rom char *const rom cli_prompt_main = "\r\n(main).Press `?` for help>";
const rom char *const rom cli_prompt_show = "\r\n(show)>";
const rom char *const rom cli_prompt_set = "\r\n(set).Press `?` for help>";
const rom char *const rom cli_prompt_usbq = "\r\n(usbq).Press `?` for help>";
const rom char *const rom cli_prompt_usbt = "\r\n(usbt)>";
const rom char *const rom cli_prompt_util = "\r\n(util)>";

const rom char *const rom esc_prev_lvl = "<ESC> - previous level\r\n";
const rom char *const rom cli_invalid_key = "\r\nInvalid key pressed\r\n";

/* Regiser names/numbers for MAX3421E register dump */
typedef struct {
	const rom char *const rom name;
	const rom char number;
}REGISTER_OUTPUT_FORMAT;

const rom REGISTER_OUTPUT_FORMAT register_format[] = {
	{ "\r\nRCVFIFO:\t",		rRCVFIFO },
	{ "\r\nSNDFIFO:\t",		rSNDFIFO },
	{ "\r\nSUDFIFO:\t",		rSUDFIFO },
	{ "\r\nRCVBC:\t\t",		rRCVBC },
	{ "\r\nSNDBC:\t\t",		rSNDBC },
	{ "\r\nUSBIRQ:\t\t",	rUSBIRQ },
	{ "\r\nUSBIEN:\t\t",	rUSBIEN },
	{ "\r\nUSBCTL:\t\t",	rUSBCTL },
	{ "\r\nCPUCTL:\t\t",	rCPUCTL },
	{ "\r\nPINCTL:\t\t",	rPINCTL },
	{ "\r\nREVISION:\t",	rREVISION },
	{ "\r\nIOPINS1:\t",		rIOPINS1 },
	{ "\r\nIOPINS2:\t",		rIOPINS2 },
	{ "\r\nGPINIRQ:\t",		rGPINIRQ },
	{ "\r\nGPINIEN:\t",		rGPINIEN },
	{ "\r\nGPINPOL:\t",		rGPINPOL },
	{ "\r\nHIRQ:\t\t",		rHIRQ },
	{ "\r\nHIEN:\t\t",		rHIEN },
	{ "\r\nMODE:\t\t",		rMODE },
	{ "\r\nPERADDR:\t",		rPERADDR },
	{ "\r\nHCTL:\t\t",		rHCTL },
	{ "\r\nHXFR:\t\t",		rHXFR },
	{ "\r\nHRSL:\t\t",		rHRSL },
	{ NULL, 0 } //end of array
};


const rom char* const rom devclasses[] = {
    " Uninitialized",
    " HID Keyboard",
    " HID Mouse",
    " Mass storage"
}; 


typedef struct _UPTIME_FORMAT {
	const rom DWORD divider;
	const rom char *const rom name;
}UPTIME_FORMAT;

const rom UPTIME_FORMAT uptime_format[ 4 ] = {
	{ 86400000, " days " },
	{ 3600000, " hours " },
	{ 60000, " minutes " },
	{ 1000, " seconds" }
};
	

#endif //_cli_constants_h_