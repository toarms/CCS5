//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430.h>

volatile unsigned int j;

void delay()
{
	volatile unsigned int i;	// volatile to prevent optimization

	i = 100000;					// SW Delay
	do i--;
	while(i != 0);

}
int main(void) {
	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
	//P1DIR |= 0xff;					// Set P1.0 to output direction
	//P1OUT |= 0xff;
	  P1DIR = 0xFF;                             // All P1.x outputs
	  P1OUT = 0;                                // All P1.x reset

	  P3DIR = 0xFF;                             // All P3.x outputs
	  P3OUT = 0;                                // All P3.x reset
	  P4DIR = 0xFF;                             // All P4.x outputs
	  P4OUT = 0;                                // All P4.x reset

	  P5DIR = 0xFF;                             // All P5.x outputs
	  P5OUT = 0;                                // All P5.x reset
	  P6DIR = 0xFF;                             // All P6.x outputs
	  P6OUT = 0;                                // All P6.x reset

    P2DIR |= (BIT0 + BIT4 + BIT5 + BIT6  + BIT7);
    P2OUT &= ~(BIT0 + BIT4 + BIT5 + BIT6  + BIT7);

	P2DIR |=  BIT1 + BIT2 + BIT3;
	P2OUT |=  BIT1 + BIT2 + BIT3;

	delay();

	j = 0;
	for(;;) {


		//P2OUT |= BIT2 + BIT3;
		//delay();

		P2OUT |= BIT2;
		P2OUT &= ~BIT3;
		delay();

		P2OUT |= BIT3;
		P2OUT &= ~BIT2;
		delay();



		//P2OUT &= ~(BIT2 + BIT3);
		//delay();

		j++;
		if(j==20)
			P2OUT &= ~BIT1;
	}
}
