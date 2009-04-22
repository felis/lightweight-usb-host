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
"2 - Show uptime\r\n"
"3 - Show USB task state\r\n";

const rom char *const rom cli_set_menu_help =
"\r\n1 - Reset MAX3421E\r\n"
"2 - turn Vbus on\r\n"
"3 - turn Vbus off\r\n";

const rom char *const rom cli_util_menu_help =
"\r\n1 - Test SPI transfers\r\n"
"2 - Util menu item 2\r\n"
"3 - Util menu item 3\r\n";

const rom char *const rom crlf = "\r\n";
const rom char *const rom angle = ">";
const rom char *const rom cli_prompt_main = "\r\n(main).Press `?` for help>";
const rom char *const rom cli_prompt_show = "\r\n(show)>";
const rom char *const rom cli_prompt_set = "\r\n(set).Press `?` for help>";

const rom char *const rom esc_prev_lvl = "<ESC> - previous level\r\n";
const rom char *const rom cli_invalid_key = "\r\nInvalid key pressed\r\n";

const rom char *const rom cli_prompt_usbq = "";
const rom char *const rom cli_prompt_usbt = "";
const rom char *const rom cli_prompt_util = "\r\n(util)>";

/* Regiser names/numbers for MAX3421E_reg_dump()	*/
typedef struct _REG_OUTPUT_FORMAT {
	const rom char *const rom name;
	const rom char number;
}REGISTER_OUTPUT_FORMAT;

const rom REGISTER_OUTPUT_FORMAT register_format[ 23 ] = {
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
	{ "\r\nHRSL:\t\t",		rHRSL }
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