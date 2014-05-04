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

 int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
/*
	 //1Mhz
	  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	  {
	    while(1);                               // do not load, trap CPU!!
	  }
	  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	  BCSCTL1 = CALBC1_1MHZ;                    // Set range
	  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation
*/


	P1DIR |= 0x01;					// Set P1.0 to output direction

	P1OUT |= BIT4;
	P1SEL |= BIT4;

	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization

		P1OUT ^= 0x01;				// Toggle P1.0 using exclusive-OR



		i = 1;					// SW Delay
		do i--;
		while(i != 0);

	}
	
	return 0;
}
