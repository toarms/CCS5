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
//   MSP430x249 Demo - XT2 Oscillator Fault Detection
//
//   Description: System runs normally in LPM3 with with basic timer clocked by
//   ACLK.  TimerA0 interrupt causes an exit from LPM3, and toggles P1.0.
//   If an XT2 oscillator fault occurs, NMI is requested, pausing execution and
//   flashing LED quickly until fault is resolved.  Assumed only XT2 as NMI
//   source - code does not check for other NMI sources. MCLK is buffered on
//   P5.4. If fault occurs for crystal osc sourcing the MCLK, MCLK is
//   automatically switched to DCO for its clock source
//   MCLK = XT2 = 8MHz normally, within ISRs MCLK = DCO freq ~1048kHz
//   //* An external 8MHx crystal is required between XT2 and XT2OUT , and
//   an external 32kHz crystal is required between XIN and XOUT.  *//
//
//                MSP430x249
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32KHz
//          --|RST          XOUT|-
//            |                 |
//            |            XT2IN|-
//            |                 | 8MHz
//            |           XT2OUT|-
//            |                 |
//            |             P1.0|-->LED
//            |        P5.4/MCLK|-->MCLK = XT2 HFXTAL
//
//  B. Nisarga
//  Texas Instruments Inc.
//  September 2007
//  Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.42A
//******************************************************************************
#include <msp430.h>

#define PB_PIN		BIT1	// push button:		P1.1
#define IR_RX_PIN	BIT2	// ir red led rx: 	p1.2
#define RED_RX_PIN	BIT3	// red led rx: 		p1.3

void UART_TX(char * tx_data);

int main(void)
{
  volatile unsigned int i;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  /*
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
*/


  BCSCTL1 &= ~XT2OFF;                       // Activate XT2 high freq xtal
  BCSCTL3 |= XT2S_2;                        // 3 ?16MHz crystal or resonator

  // Wait for xtal to stabilize
  do
  {
  IFG1 &= ~OFIFG;                           // Clear OSCFault flag
  for (i = 0x47FF; i > 0; i--);             // Time for flag to set
  }
  while ((IFG1 & OFIFG));                   // OSCFault flag still set?

 // BCSCTL2 |= SELM_2 + DIVM_3;                        // MCLK = XT2 HF XTAL (safe)
  BCSCTL2 |= (SELM_2 + SELS);                        // MCLK = XT2 HF XTAL (safe)


  P5DIR |= 0x10;                            // P5.4= output direction
  P5SEL |= 0x10;                            // P5.4= MCLK option select


  // Timer A
  //Initialize Timer A to generate one-second interrupt
 // TACCTL0 = CCIE;                           // TACCR0 interrupt enabled
  //TACCR0 = 32767;                           // Set CCR0 to 32767(ACLK=32.768Hz)
 // TACTL = TASSEL_1 + MC_1;                  // ACLK, up mode



// An immediate Osc Fault will occur next
 // IE1 |= OFIE;                              // Enable osc fault interrupt

  // Uart
  P3SEL |= BIT6+BIT7;                       // P3.6,7 = USCI_A1 TXD/RXD
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 0x45;                              // 1MHz 115200
  UCA1BR1 = 0x00;                              // 1MHz 115200
 // UCA1MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5
  UCA1MCTL = 0x00;
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
 // IE2 |= UCA1RXIE;                          // Enable USCI_A0 RX interrupt

  // Init ports
  P3DIR |= BIT4;                            // Set led to output direction
  // RED an IR RED RX PINs
  P1DIR &= ~(RED_RX_PIN + IR_RX_PIN);
  P1REN |= (RED_RX_PIN + IR_RX_PIN);
  P1OUT |= (RED_RX_PIN + IR_RX_PIN);

  P1IES |= (RED_RX_PIN + IR_RX_PIN);
  P1IE  |= (RED_RX_PIN + IR_RX_PIN);
  P1IFG = 0;

  while(1)
  {
    _BIS_SR(LPM3_bits + GIE);               // Enter LPM3, enable interrupts
    P3OUT ^= BIT4;                          // Toggle led
    //UART_TX("1234567890");
    //__delay_cycles(1000000);
  }
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
	unsigned int temp;
	unsigned int counter = 0x0009;

    //unsigned int i=0;
	while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)

    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = tx_data[0];
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = tx_data[1];



    if(tx_data[1] == 'o')
    {

    TBCTL = TBSSEL_2 + MC_2 + TBCLR;

	P4SEL |= BIT1;				//P4.1 TimerB capture

	TBCCTL1 = CM_1 + SCS + CAP;	// set for caputre on rising edge


// wait until capture event occurs
	while(!(TBCCTL1 & CCIFG));
	TBCCTL1 &= ~CCIFG;			// clear the interrupt flag
	temp = TBCCR1;				// take the CCR value

	// set P4.1 to capture the next rising edge
	TBCCTL1 = CM_1 + SCS + CAP;

	//wait until capture event occurs
	while(!(TBCCTL1 & CCIFG));

	// switch off timer capture and caculate the frequency
	counter = TBCCR1 - temp;

	TBCCTL1 &= ~CCIFG;
	TBCTL = 0;
    }


    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = counter & 0x00FF;
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0x12;
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0x13;
/*
    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
    	while (!(UC1IFG&UCA1TXIFG)); // Wait if line TX/RX module is busy with data
        UCA1TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
*/
}

#pragma vector=NMI_VECTOR
__interrupt void nmi_ (void)
{
  volatile unsigned int i;

  do
  {
    IFG1 &= ~OFIFG;                         // Clear OSC Fault flag
    for (i = 0x47FF; i > 0; i--);           // Time for flag to set
    P3OUT ^= BIT4;                          // Toggle P1.0
  }while (IFG1 & OFIFG);                    // OSC Fault flag still set?
  IE1 |= OFIE;                              // Enable Osc Fault
}


// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  _BIC_SR_IRQ(LPM3_bits+GIE);               // Exit LPM3 upon ISR exit
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
		UART_TX("ro");
	else
		UART_TX("rf");

    // clear interrupt and change interrupt transition
    P1IFG &= ~RED_RX_PIN;
    P1IES ^= RED_RX_PIN;
  }
  // PC_RX_PIN state change
  else if(P1IFG & IR_RX_PIN)
  {
	// change output pin
	if(P1IN & IR_RX_PIN)
	    UART_TX("io");
    else
	    UART_TX("if");

	// clear interrupt and change interrupt transition
	P1IFG &= ~IR_RX_PIN;
    P1IES ^= IR_RX_PIN;
  }
}
