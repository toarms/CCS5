
#include <msp430.h>

#define PB_PIN		BIT1	// push button:		P1.1
#define IR_RX_PIN	BIT2	// ir red led rx: 	p1.2
#define RED_RX_PIN	BIT3	// red led rx: 		p1.3

#define RED_LED_FLAG	0
#define IR_LED_FLAG		1

// the result
unsigned int start_counter;
unsigned int last_counter;
unsigned int pulse_counts;
unsigned int rcounter, icounter;
unsigned int rcounts=999, icounts=777;

void UART_TX(char * tx_data);

int main(void)
{
  volatile unsigned int i;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

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
  //TACCR0 = 32767;                            // Set CCR0 to 32767(ACLK=32.768Hz)
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


  /*
  {
	  P3SEL |= BIT4+BIT5;                       // P3.4,5 = USCI_A0 TXD/RXD
	  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	  UCA0BR0 = 0x45;                              // 1MHz 115200
	  UCA0BR1 = 0x00;                              // 1MHz 115200
	 // UCA1MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5
	  UCA0MCTL = 0x00;
	  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  }
  */

	while (!(UC1IFG&UCA1TXIFG));
  	  UCA1TXBUF = 0x09;

  // Init ports
  P3DIR |= BIT4;                            // Set led to output direction
  P4SEL |= BIT1;				//P4.1 TimerB capture

  // RED an IR RED RX PINs
  P1DIR &= ~(RED_RX_PIN + IR_RX_PIN);
  P1REN |= (RED_RX_PIN + IR_RX_PIN);
  P1OUT |= (RED_RX_PIN + IR_RX_PIN);

  P1IES |= (RED_RX_PIN + IR_RX_PIN);
  P1IE  |= (RED_RX_PIN + IR_RX_PIN);
  P1IFG = 0;


  _BIS_SR(GIE);
  while(1)
  {
	__delay_cycles(1000000);
  }
}


void get_sensor_data(unsigned int led)
{
	unsigned int counter = 0x0009;
	unsigned int temp;

	TBCTL = TBSSEL_2 + MC_2 + TBCLR;
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
    // stop timer
    TBCCTL1 &= ~CCIFG;
    TBCTL = 0;

    if(led == RED_LED_FLAG){
    	rcounter = counter;
    	rcounts++;
    }
    else {
    	icounter = counter;
    	icounts++;
    }
  }


void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{

	unsigned int counter, counts;
	static unsigned int last_counter_high;

	/* filter some unexpected value. */
	if(counter > 1000)
	{
		counter =  last_counter_high;
	} else
		last_counter_high = counter ;


	if(tx_data[0] == 'i')
	{
		counter = icounter;
		counts = icounts;
	} else {
		counter = rcounter;
		counts = rcounts;
	}

    //unsigned int i=0;
	while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0x00;				// Byte 1 - 0x00 (synchronization byte)
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = 0xFF;				// Byte 2 - 0xFF (synchronization byte)

    while (!(UC1IFG&UCA1TXIFG));	// "ro" / "io"
    UCA1TXBUF = tx_data[0];
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = tx_data[1];

    while (!(UC1IFG&UCA1TXIFG));	// counter
    UCA1TXBUF = counter & 0x00FF;
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = (counter >> 8) & 0x00FF;

    while (!(UC1IFG&UCA1TXIFG));	// counts
    UCA1TXBUF = counts & 0x00FF;
    while (!(UC1IFG&UCA1TXIFG));
    UCA1TXBUF = (counts >> 8) & 0x00FF;

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
 // _BIC_SR_IRQ(LPM3_bits+GIE);               // Exit LPM3 upon ISR exit
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
	if(P1IN & RED_RX_PIN){
		get_sensor_data(RED_LED_FLAG);
	}
	else
	{
		UART_TX("rf");
	}

    // clear interrupt and change interrupt transition
    P1IFG &= ~RED_RX_PIN;
    P1IES ^= RED_RX_PIN;
  }
  // PC_RX_PIN state change
  else if(P1IFG & IR_RX_PIN)
  {
	// change output pin
	if(P1IN & IR_RX_PIN)
		get_sensor_data(IR_LED_FLAG);
    else{
	    UART_TX("if");
    }

	// clear interrupt and change interrupt transition
	P1IFG &= ~IR_RX_PIN;
    P1IES ^= IR_RX_PIN;
  }
}
