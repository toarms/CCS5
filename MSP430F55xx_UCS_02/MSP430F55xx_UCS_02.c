/*
 * This functions drives all the outputs low which may not be optimal for a
 * different board. This function needs to be modified as per the connections
 * to the GPIO pins.
 */
void initPorts(void)
{
    P1OUT = 0x00;
    P1DIR = 0xFF;
    P2OUT = 0x00;
    P2DIR = 0xFF;
    P3OUT = 0x00;
    P3DIR = 0xFF;
    P4OUT = 0x00;
    P4DIR = 0xFF;
    P5OUT = 0x00;
    P5DIR = 0xFF;
    P6OUT = 0x00;
    P6DIR = 0xFF;
    P7OUT = 0x00;
    P7DIR = 0xFF;
    P8OUT = 0x00;
    P8DIR = 0xFF;
}

void initClocks(DWORD mclkFreq)
{
    // Assign the REFO as the FLL reference clock
	UCS_clockSignalInit(
	   UCS_BASE,
	   UCS_FLLREF,
	   UCS_REFOCLK_SELECT,
	   UCS_CLOCK_DIVIDER_1);

	// Assign the REFO as the source for ACLK
	UCS_clockSignalInit(
	   UCS_BASE,
	   UCS_ACLK,
	   UCS_REFOCLK_SELECT,
	   UCS_CLOCK_DIVIDER_1);

    UCS_initFLLSettle(
        UCS_BASE,
        mclkFreq/1000,
        mclkFreq/32768);
        //use REFO for FLL and ACLK
        UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

}

// Initializes the USCI_A1 module as a UART, using baudrate settings in
// bcUart.h.  The baudrate is dependent on SMCLK speed.
void bcUartInit(void)
{
    // Always use the step-by-step init procedure listed in the USCI chapter of
    // the F5xx Family User's Guide
    UCA1CTL1 |= UCSWRST;        // Put the USCI state machine in reset
    UCA1CTL1 |= UCSSEL_2;  // Use SMCLK as the bit clock

    // Set the baudrate
    UCA1BR0 = UCA1_BR0;
    UCA1BR1 = UCA1_BR1;
    UCA1MCTL = (UCA1_BRF << 4) | (UCA1_BRS << 1) | (UCA1_OS);

    P4SEL |= BIT4+BIT5;         // Configure these pins as TXD/RXD

    UCA1CTL1 &= ~UCSWRST;       // Take the USCI out of reset
    UCA1IE |= UCRXIE;           // Enable the RX interrupt.  Now, when bytes are
                                // rcv'ed, the USCI_A1 vector will be generated.
}

