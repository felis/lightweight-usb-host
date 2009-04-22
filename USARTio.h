/* USART support header file */
/* accompanies USARTio.c 		 */

#ifndef	_USARTio_h_
#define _USARTio_h_

/* USARTio.c function declarations */
void USART_init( void );
BYTE recvchar( void );
BYTE sendchar( BYTE data );
BOOL CharInQueue( void );
void send_string( const rom char *str_ptr );
void send_ram_string (char *str_ptr);
void send_hexbyte( BYTE data );
void send_decword ( WORD number );
void send_dotted_decimal( char *octet );

#endif // _USARTIO_h_
