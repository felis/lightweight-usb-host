/* pic18f26k20-based MAX3421E based Lightweight USB host */

#define _LwUSBhost_c_


#pragma config FOSC = INTIO7, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDTEN = OFF
#pragma config MCLRE = ON, HFOFST = OFF, PBADEN = OFF, STVREN = ON, LVP = OFF, XINST = OFF, DEBUG = ON

#include "project_config.h"

/* Global vars  */
volatile DWORD uptime = 0;                      //system uptime. Gets updated every millisecond

volatile BYTE USART_Tx_buf [ USART_RX_BUFSIZE ];
volatile BYTE USART_Tx_head = 0;
volatile BYTE USART_Tx_tail = 0;
volatile BYTE USART_Rx_buf [ USART_RX_BUFSIZE ];
volatile BYTE USART_Rx_head = 0;
volatile BYTE USART_Rx_tail = 0;

/* Prototypes   */
void highPriorityISR(void);
void lowPriorityISR(void);
void Board_init( void );

/* ISRs */
#pragma code high_vector=0x08
void interruptAtHighVector(void)
{
    _asm GOTO highPriorityISR _endasm
}
#pragma code
#pragma code low_vector=0x18
void interruptAtLowVector(void)
{
    _asm GOTO lowPriorityISR _endasm
}
#pragma code
#pragma interruptlow lowPriorityISR
void lowPriorityISR(void)
{
 BYTE data,tmphead,tmptail;           //USART vars
/* USART handler start */
    if(PIR1bits.RCIF) {
        data = RCREG;                                                   // read the received data 
        tmphead = ( USART_Rx_head + 1 ) & USART_RX_BUF_MASK;            // calculate buffer index 
        USART_Rx_head = tmphead;                                        // store new index 
        if ( tmphead == USART_Rx_tail ) {
        // ERROR! Receive buffer overflow 
        }
        USART_Rx_buf[ tmphead ] = data;                                 // store received data in buffer 
  }
    //? need to check for Tx IF
    if(TXSTAbits.TRMT) {    
                                                                        // check if all data is transmitted  
        if ( USART_Tx_head != USART_Tx_tail ) {                                     
            tmptail = ( USART_Tx_tail + 1 ) & USART_TX_BUF_MASK;        // calculate buffer index  
            USART_Tx_tail = tmptail;                                    // store new index  
            TXREG = USART_Tx_buf[ tmptail ];                            // start transmition  
        }
        else {
            PIE1bits.TXIE = 0;         // disable TX interrupt  
        }
    } 
/* USART handler end */
}

/* Timer1 interrupt handler */
#pragma interrupt highPriorityISR
void highPriorityISR(void)
{
    uptime++;                       
    PIR2bits.CCP2IF = 0;
}



void main ( void )
{
    Board_init();
    USART_init();
    CLI_init();
    SPI_init (SPI_FOSC_4, MODE_00, SMPMID);
    MAX3421E_init();
    USB_init();
    while(1) {
        CLI_Task();
        MAX3421E_Task();
        USB_Task();
    }
}

void Board_init( void )
{
 DWORD init_delay;
    /* Internal clock is 1MHz by default    */
    INTCONbits.GIEH = 0;    //disable interrupts
    RCONbits.IPEN = 1;      //enable interrupt priority
    /* Configure all port pins as digital outputs   */
    TRISA   = 0x00;
    LATA    = 0x00;
    ADCON1  = 0x0f;
    PORTA = 0x00;

    TRISB = 0x00;
    LATB = 0x00;
    PORTB = 0x00;

    TRISC = 0x00;
    LATC = 0x00;
    PORTC = 0x00;
    /* Configure inputs */
    MAX3421E_INT_TR = 1;
    MAX3421E_GPX_TR = 1;
    /* Initial pin states   */
    MAX3421E_SS = 1;
    MAX3421E_RESET = 1;     //real reset stays low for next 200ms
    /* Clock configuration  */
    /* Internal High-speed with 4xPLL 64MHz */
    OSCCON |= 0x70;         //IRCF = 111
    OSCTUNE |= 0x40;        //PLL
    /* Timer3 configuration */
    TMR3L = 0;
    TMR3H = 0;
    T3CONbits.T3CCP2 = 1;   //CCP2 compare source
    T3CON |= 0x00;          //1:1 prescale
    T3CONbits.TMR3CS = 0;   //internal clock
    T3CONbits.TMR3ON = 1;   //start timer
    /* CCP2 configuration. CCP2 reloads Timer3 and generates interrupt  */
    CCPR2H = 0x3e;          // 16000 cycles, 62.5ns cycle, 1ms between interrupts
    CCPR2L = 0x80;
    CCP2CON |= 0x0b;        //CCP mode 1011, compare triggers special event
    PIE2bits.CCP2IE = 1;    //enable CCP2 interrupt
    IPR2bits.CCP2IP = 1;    //set high priority
            
    INTCONbits.GIEL = 1;        //enable low-priority interrupts
    INTCONbits.GIEH = 1;        //enable all interrupts
    init_delay = uptime + 200;
    while( init_delay > uptime );   //wait 200ms
}
