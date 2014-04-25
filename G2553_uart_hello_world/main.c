// Press button to print hello to terminal

#include  <msp430.h> // System define for the micro that I am using

#define RXD        BIT1 //Check your launchpad rev to make sure this is the case. Set jumpers to hardware uart.
#define TXD        BIT2 // TXD with respect to what your sending to the computer. Sent data will appear on this line
#define BUTTON    BIT3

void UART_TX(char * tx_data);            // Function Prototype for TX

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;         // Stop Watch dog timer

  if (CALBC1_1MHZ == 0xFF)
  {
	  while(1);
  }

  DCOCTL = 0;
  BCSCTL1 = CALBC1_1MHZ;            // Set DCO to 1 MHz
  DCOCTL = CALDCO_1MHZ;

  P1SEL = RXD + TXD ;                // Select TX and RX functionality for P1.1 & P1.2
  P1SEL2 = RXD + TXD ;              //

  P1DIR |= BIT0 + BIT6;
  P1OUT &= ~(BIT0);
  P1OUT |= BIT6;

  /*
  UCA0CTL1 |= UCSSEL_2;             // Have USCI use System Master Clock: AKA core clk 1MHz
  UCA0BR0 = 104;                    // 1MHz 9600, see user manual
  UCA0BR1 = 0;                      //
  UCA0MCTL = UCBRS0;                // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;             // Start USCI state machine
  */

  UCA0CTL1 |= UCSSEL_2;
  UCA0BR0	= 8;
  UCA0BR1	= 0;
  UCA0MCTL &= UCBRS2 + UCBRS0;
  UCA0CTL1 &= ~UCSWRST;

  P1DIR &=~BUTTON;                  // Ensure button is input (sets a 0 in P1DIR register at location BIT3)
  P1OUT |=  BUTTON;                 // Enables pullup resistor on button
  P1REN |=  BUTTON;

  // Initialize TimeA to generate 1024/pcs interrupt
  TACCTL0 = CCIE;
  TACCR0 = 200;						// 1024/pcs
  TACTL = TASSEL_1 + MC_1;			// ACLK, up mode

  while(1)
  {
	  P1OUT ^= BIT0 + BIT6;
	  __delay_cycles(2000);
	  _BIS_SR(LPM3_bits + GIE);
  }

  while(1)                          // While 1 is equal to 1 (forever)
  {
      if(!((P1IN & BUTTON)==BUTTON)) // Was button pressed?
      {
          UART_TX("Hello World! \r\n");  // If yes, Transmit message & drink beer
          P1OUT ^= BIT0;
          P1OUT ^= BIT6;
          __delay_cycles(200000); //Debounce button so signal is not sent multiple times

          UCA0TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
          while ((UCA0STAT & UCBUSY));
          UCA0TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)
          while ((UCA0STAT & UCBUSY));
          UCA0TXBUF = 0x32;				// Byte 3 - IR Heart signal (AC only)
          while ((UCA0STAT & UCBUSY));
          UCA0TXBUF = 0x43;				// Byte 4 - Heart rate data
          while ((UCA0STAT & UCBUSY));
          UCA0TXBUF = 99;					// Byte 5 - SaO2 data

      }
  }
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
        while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
        UCA0TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        P1OUT ^= BIT6;
        i++; // Increment variable for array address
    }
}

// TimerA0 timer Interrupt Service Routing
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    UCA0TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
    while ((UCA0STAT & UCBUSY));
    UCA0TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)
    while ((UCA0STAT & UCBUSY));
    UCA0TXBUF = 0x32;				// Byte 3 - IR Heart signal (AC only)
    while ((UCA0STAT & UCBUSY));
    UCA0TXBUF = 0x43;				// Byte 4 - Heart rate data
    while ((UCA0STAT & UCBUSY));
    UCA0TXBUF = 98;					// Byte 5 - SaO2 data
}
