// Press button to print hello to terminal

#include  <msp430.h> // System define for the micro that I am using

volatile unsigned int i;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT


  P3DIR |= BIT4;                        // P5.4= MCLK option select
  P3OUT |= BIT4;
                                            // P5.6= ACLK option select
  while(1)
  {
    P3OUT ^= BIT4;                      // P1.0 set
    for (i = 100000; i > 0; i--);               // Delay 1x
  }
}
