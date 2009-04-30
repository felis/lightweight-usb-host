/* USART IO Functions */

#define _USARTIO_C_

#include "project_config.h"

extern volatile BYTE USART_Tx_buf [ USART_RX_BUFSIZE ];
extern volatile BYTE USART_Tx_head;
extern volatile BYTE USART_Tx_tail;

extern volatile BYTE USART_Rx_buf [ USART_RX_BUFSIZE ];
extern volatile BYTE USART_Rx_head;
extern volatile BYTE USART_Rx_tail;

/*	Init USART Function for PIC18s with 1 USART */
/*	96000 8N1:-) @64MHz* - SPBRG 103 */
 
void USART_init(void)
{
	SPBRG = 103;		//baud rate divisor 9600@64MHz low speed
	//SPBRG = 25;			//baud rate generator 9600@16MHz
	TRISCbits.TRISC7 = 1;				//receive pin
	TRISCbits.TRISC6 = 0;				//transmit pin
  	
	RCSTA = 0x80;		//enable serial port and serial port pins
	IPR1bits.RCIP = 0;				//receive interrupt = low priority
	IPR1bits.TXIP = 0;				//transmit interrupt = low priority
	PIE1bits.RCIE = 1;				//enable receive interrupt
	RCSTAbits.CREN = 1;				//enable USART1 receiver
	PIE1bits.TXIE = 0;				//disable USART1 transmit interrupt
	TXSTAbits.TXEN = 1;				//transmitter enabled
}

/* get character */
BYTE recvchar(void)
{
 BYTE tmptail;
                                
 while ( USART_Rx_head == USART_Rx_tail );  							// wait for incoming data 
 tmptail = ( USART_Rx_tail + 1 ) & USART_RX_BUF_MASK;			// calculate buffer index  
 USART_Rx_tail = tmptail;        													// store new index  

 return USART_Rx_buf[tmptail];   													// return data  
}

/* send character 															*/
/* blocks program execution while txbuf is full */
/* use with caution															*/
BYTE sendchar(BYTE data)
{
  BYTE tmphead;
  tmphead = ( USART_Tx_head + 1 ) & USART_TX_BUF_MASK;	// calculate buffer index
  while ( tmphead == USART_Tx_tail );					// wait for free space in buffer
  USART_Tx_buf[tmphead] = data;							// store data in buffer
  USART_Tx_head = tmphead;								// store new index
  PIE1bits.TXIE = 1;									// enable TX interrupt
  return data;
}

/* Check for char in Rx queue */
BOOL CharInQueue(void)
{
  return(USART_Rx_head != USART_Rx_tail);
}

/* sends a NULL-terminated ROM string to sendchar() */
void send_string (const rom char *str_ptr)
{
	while (*str_ptr) {
		sendchar(*str_ptr);
		str_ptr++;
	}
}

/* sends a NULL-terminated RAM string to sendchar() */
void send_ram_string (char *str_ptr)
{
	while (*str_ptr) {
		sendchar(*str_ptr);
		str_ptr++;
	}
}

/* sending 2 ASCII symbols representing input byte in hex */
void send_hexbyte ( BYTE data )
{
	
	BYTE temp = data>>4;			//prepare first output character

	if ( temp > 9 ) temp+=7;		//jump to letters in ASCII table
	sendchar ( temp + 0x30 );
	
	data = data & 0x0f;				// mask 4 high bits
	if ( data > 9 ) data+=7;
	sendchar ( data + 0x30 );	
}
/* itoa variation for positive 16-bit integers	*/
void send_decword ( WORD number )
{
 BYTE buf[6], next = 0;
 WORD k, r, flag = 0;
 
 	if( number == 0 ) {
 		buf[ next ] = '0';
 		next++;
 	}
	else {
		k = 10000;
		while( k > 0 ) {
			r = number / k;
			if( flag || r > 0 ) {
				buf[ next ] = '0' + r;
				next++;
				flag = 1;
			}   
 			number -= r * k;
 			k = k / 10;
 		}
 	}
 	buf[ next ] = 0;		//add NULL termination
 	send_ram_string( buf );
}

/* outputs 4 consecutive bytes in dotted-decimal format */
void send_dotted_decimal( char *octet_ptr )
{
	BYTE i;
	
		for( i = 0; i < 4; i++) {
			send_decword(( WORD ) *octet_ptr );
			octet_ptr++;
			if( i < 3 ) {								//print dots between octets
				sendchar( 0x2e );					
			}
		}
	return;
}

