/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F552x Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1048576hz = 1048576/115200 = ~9.1 (009h|01h)
//   ACLK = REFO = ~32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//   See User Guide for baud rate divider table
//
//                 MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |     P3.3/UCA0TXD|------------>
//            |                 | 115200 - 8N1
//            |     P3.4/UCA0RXD|<------------
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************

#include <msp430.h>
#include <msp430f5529.h>

#define PB_PIN		BIT1	// push button:		P1.1
#define IR_RX_PIN	BIT2	// ir red led rx: 	p1.2
#define RED_RX_PIN	BIT3	// red led rx: 		p1.3

int i;


void UART_TX(char * tx_data);

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  P4DIR |= BIT7;
  P4OUT |= BIT7;
  P1DIR |= BIT0;
  P1OUT &= ~BIT0;

  P1DIR |= BIT5;
  P1OUT |= BIT5;

  // Uart
  P4SEL |= BIT4+BIT5;                       // P4.3,4 = USCI_A1 TXD/RXD
  UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
  UCA1BR1 = 0;                              // 1MHz 115200
  UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

  // TimeA

  TA0CTL   = TASSEL_1 + TACLR;				// ACLK, up mode
  TA0CCR0  = 31;
  TA0CCTL0 = CCIE;
  TA0CTL 	  |= MC_1;							// TimerA up

  // init push button pin
  P1DIR &= ~PB_PIN;
  P1REN |= PB_PIN;
  P1OUT |= PB_PIN;

  // set GPIO interrupts
  P1IES |= (RED_RX_PIN + IR_RX_PIN);
  P1IE |= (RED_RX_PIN + IR_RX_PIN);
  P1IFG = 0;


  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
  __no_operation();                         // For debugger
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;

    UCA1TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)

    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
    	while (!(UCA1IFG & UCTXIFG)); // Wait if line TX/RX module is busy with data
        UCA1TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	//  P4OUT ^= BIT7;
//	  P1OUT ^= BIT0;

  switch(__even_in_range(UCA1IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA1TXBUF = UCA1RXBUF;                  // TX -> RXed character
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_a(void)
{
	P1OUT ^= BIT5;
	/*
    UCA1TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 0x11;				// Byte 3 - IR Heart signal (AC only)
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 0x12;				// Byte 4 - Heart rate data
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 0x13;					// Byte 5 - SaO2 data
    */
}



/**************************************************************************//**
*
* Port1_Isr
*
* @brief         Port 1 interrupt
*
* @param         -
*
* @return        -
*
******************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void Port1_Isr(void)
{
  // MSP_RX_PIN state change
  if(P1IFG & RED_RX_PIN)
  {
    // change output pin
	if(P1IN & RED_RX_PIN)
		UART_TX("r   on");
	else
		UART_TX("r  off");

    // clear interrupt and change interrupt transition
    P1IFG &= ~RED_RX_PIN;
    P1IES ^= RED_RX_PIN;
  }
  // PC_RX_PIN state change
  else if(P1IFG & IR_RX_PIN)
  {
	    // change output pin
	    if(P1IN & IR_RX_PIN)
	    	UART_TX("ir  on");
	    else
	    	UART_TX("ir off");

	    // clear interrupt and change interrupt transition
	    P1IFG &= ~IR_RX_PIN;
	    P1IES ^= IR_RX_PIN;
  }
}
