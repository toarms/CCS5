#include <msp430.h>

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80
#define PCD8544_HPIXELS	84
#define PCD8544_VBANKS	6
#define PCD8544_MAXBYTES 504 // PCD8544_HPIXELS * PCD8544_VBANKS

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

//transform
#define NONE 0x00
#define FLIP_H 0x01
#define FLIP_V 0x02
#define ROTATE 0x04 // 90 deg CW
#define ROTATE_90_CW ROTATE
#define ROTATE_90_CCW (FLIP_H | FLIP_V | ROTATE)
#define ROTATE_180 (FLIP_H | FLIP_V)

static const char font[][5] = {		// basic font
	 {0x00, 0x00, 0x00, 0x00, 0x00} // 20
	,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
	,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
	,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
	,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
	,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
	,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
	,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
	,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
	,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
	,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
	,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
	,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
	,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
	,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
	,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
	,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
	,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
	,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
	,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
	,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
	,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
	,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
	,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
	,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
	,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
	,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
	,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
	,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
	,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
	,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
	,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
	,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
	,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
	,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
	,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
	,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
	,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
	,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
	,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
	,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
	,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
	,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
	,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
	,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
	,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
	,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
	,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
	,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
	,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
	,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
	,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
	,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
	,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
	,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
	,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
	,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
	,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
	,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
	,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
	,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
	,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
	,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
	,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
	,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
	,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
	,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
	,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
	,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
	,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
	,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
	,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
	,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
	,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
	,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
	,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
	,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
	,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
	,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
	,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
	,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
	,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
	,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
	,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
	,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
	,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
	,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
	,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
	,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
	,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
	,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
	,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
	,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
	,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
	,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
	,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f Deg Symbol
};

#define LCD5110_SCLK_PIN BIT5
#define LCD5110_DN_PIN BIT7
#define LCD5110_SCE_PIN BIT2
#define LCD5110_DC_PIN BIT1
#define LCD5110_SELECT P1OUT &= ~LCD5110_SCE_PIN
#define LCD5110_DESELECT P1OUT |= LCD5110_SCE_PIN
#define LCD5110_SET_COMMAND P1OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA P1OUT |= LCD5110_DC_PIN
#define LCD5110_COMMAND 0
#define LCD5110_DATA 1

#define SPI_MSB_FIRST UCB0CTL0 |= UCMSB // or UCA0CTL0 |= UCMSB (USCIA) or USICTL0 &= ~USILSB (USI)
#define SPI_LSB_FIRST UCB0CTL0 &= ~UCMSB // or UCA0CTL0 &= ~UCMSB or USICTL0 |= USILSB (USI)

void writeStringToLCD(const char *string);
void writeCharToLCD(char c);
void writeBlockToLCD(char *byte, unsigned char length);
void writeGraphicToLCD(char *byte, unsigned char transform);
void writeToLCD(unsigned char dataCommand, unsigned char data);
void clearLCD();
void clearBank(unsigned char bank);
void setAddr(unsigned char xAddr, unsigned char yAddr);
void initLCD();

unsigned char currXAddr = 0; //TODO this will be used for tracking current addr
unsigned char currYAddr = 0; //not implemented


void writeStringToLCD(const char *string) {
    while(*string) {
        writeCharToLCD(*string++);
    }
}

void writeCharToLCD(char c) {
    unsigned char i;
    for(i = 0; i < 5; i++) {
        writeToLCD(LCD5110_DATA, font[c - 0x20][i]);
    }
    writeToLCD(LCD5110_DATA, 0);
}

void writeBlockToLCD(char *byte, unsigned char length) {
    unsigned char c = 0;
    while(c < length) {
        writeToLCD(LCD5110_DATA, *byte++);
        c++;
    }
}

void writeGraphicToLCD(char *byte, unsigned char transform) {
    int c = 0;
    char block[8];

    if(transform & FLIP_V) {
        SPI_LSB_FIRST;
    }
    if(transform & ROTATE) {
        c = 1;
        while(c != 0) {
            (*byte & 0x01) ? (block[7] |= c) : (block[7] &= ~c);
            (*byte & 0x02) ? (block[6] |= c) : (block[6] &= ~c);
            (*byte & 0x04) ? (block[5] |= c) : (block[5] &= ~c);
            (*byte & 0x08) ? (block[4] |= c) : (block[4] &= ~c);
            (*byte & 0x10) ? (block[3] |= c) : (block[3] &= ~c);
            (*byte & 0x20) ? (block[2] |= c) : (block[2] &= ~c);
            (*byte & 0x40) ? (block[1] |= c) : (block[1] &= ~c);
            (*byte & 0x80) ? (block[0] |= c) : (block[0] &= ~c);
            *byte++;
            c <<= 1;
        }
    } else {
        while(c < 8) {
            block[c++] = *byte++;
        }
    }

    if(transform & FLIP_H) {
        c = 7;
        while(c > -1) {
            writeToLCD(LCD5110_DATA, block[c--]);
        }
    } else {
        c = 0;
        while(c < 8) {
            writeToLCD(LCD5110_DATA, block[c++]);
        }
    }
    SPI_MSB_FIRST;
}

void writeToLCD(unsigned char dataCommand, unsigned char data) {
    LCD5110_SELECT;
    if(dataCommand) {
        LCD5110_SET_DATA;
    } else {
        LCD5110_SET_COMMAND;
    }
    UCB0TXBUF = data;
    while(!(IFG2 & UCB0TXIFG))
        ;
    LCD5110_DESELECT;
}

void clearLCD() {
    setAddr(0, 0);
    int c = 0;
    while(c < PCD8544_MAXBYTES) {
        writeToLCD(LCD5110_DATA, 0);
        c++;
    }
    setAddr(0, 0);
}

void clearBank(unsigned char bank) {
    setAddr(0, bank);
    int c = 0;
    while(c < PCD8544_HPIXELS) {
        writeToLCD(LCD5110_DATA, 0);
        c++;
    }
    setAddr(0, bank);
}

void setAddr(unsigned char xAddr, unsigned char yAddr) {
    writeToLCD(LCD5110_COMMAND, PCD8544_SETXADDR | xAddr);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETYADDR | yAddr);
}

void lcd_pos(unsigned char x, unsigned char y)
{
	setAddr(x,y);
}

void lcd_puts(char *s, unsigned char x, unsigned char y)
{
	setAddr(x,y);
	writeStringToLCD(s);
}



void lcd_putc(char c, unsigned char x, unsigned char y)
{
	setAddr(x,y);
	writeCharToLCD(c);
}


void lcd_pd10(char n, int x, int y)
{
	setAddr(x,y);
	writeCharToLCD(n + 0x30);
}
static const unsigned long dv[] = {             // Base 10 digit weights
  10000000,                                 // 8 digit maximum count
   1000000,                                 //
    100000,                                 //
     10000,                                 //
      1000,                                 //
       100,                                 //
        10,                                 //
         1,                                 //
         0                                  //
};

static void print_freq(unsigned long n)
{
	const unsigned long *dp = dv;
	unsigned x = 0;
	char c;
	unsigned long d;

	while(n < *dp) {                            // Skip leading zeros
	    ++dp;                                   //
	   // lcd_pd10(0, x, 2);                     // Print space
	    x += 5;                                //
	}                                           //

	if(n) {                                     // Non-zero
		do {                                    //
        		d = *dp++;                          // Get digit value
        		c = 0;                              //
        		while(n >= d) ++c, n -= d;          // Divide
				if(d == 100000 || d == 100) x += 3; // Space between 3 digit groups
				lcd_pd10(c, x, 2);                  // Print digit
				x +=6;                            //
		} while(!(d & 1));                      // Until all digits done
	} else
		lcd_pd10(0, x-10, 2);
}

void initLCD() {
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    writeToLCD(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
}

static unsigned clock_input = 0;

void set_input(void)
{
	lcd_pos(0, 4);

	switch(clock_input) {
	default:
		clock_input = 0;
	case 0:
		TACTL = TASSEL_2;
		lcd_puts("Internal MCLK", 0, 4);
		break;
	case 1:
		TACTL = TASSEL_0;
		lcd_puts("Clock In P1.0", 0, 4);
		break;
	}
}

unsigned long freq = 12345678L;
void set_gate(unsigned long f)
{
	if(WDTCTL & WDTIS0) {                       // 250 ms gate currently in use
		if(f < 800000) {                        // Switch to 1 s gate if frequncy is below 800 kHz
			lcd_puts("1 Second Gate", 3, 5);
			clearBank(2);
			WDTCTL = WDTPW | WDTTMSEL | WDTSSEL;
		}
	} else {                                    // 1 s gate currently in use
		if(f > 900000) {                        // Switch to 250 ms gate if frequency above 900 kHz
			lcd_puts(" 250 ms Gate ", 3, 5);
			clearBank(2);
			WDTCTL = WDTPW | WDTTMSEL | WDTSSEL | WDTIS0;
		}
	}
}


unsigned volatile long i;

void main(void) {

    WDTCTL = WDTPW + WDTHOLD;         // disable WDT
    BCSCTL1 = CALBC1_16MHZ;            // 1MHz clock
    DCOCTL = CALDCO_16MHZ;

    BCSCTL1 &= ~XTS;					// LFXTCLK 0: Low Freq. / 1: High Freq.
    BCSCTL3 |= XCAP_3;					// XIN/XOUT Cap : 12.5 pF
    BCSCTL3 |= LFXT1S_0;				// LFXT1 = 32768 crystal

    while (IFG1 & OFIFG)
    {
    	IFG1 &= ~OFIFG;
    }

    // use DCO as system clock (MCLK)
    BCSCTL2 = SELM_0;

    P1OUT |= LCD5110_SCE_PIN + LCD5110_DC_PIN;
    P1DIR |= LCD5110_SCE_PIN + LCD5110_DC_PIN;

    // setup USIB
    P1SEL |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;
    P1SEL2 |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;

    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 |= 0x01;                          // 1:1
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // clear SW

    _delay_cycles(500000);
    // Initialize LCD
    initLCD();
    clearLCD();

    // set the port
    P1DIR &= ~BIT0;
    P1SEL |= BIT0;
    P1OUT |= BIT0;
    P1REN |= BIT0;

    // enable SM clock
    P1DIR |= BIT4;
    P1SEL |= BIT4;

    P1DIR &= ~BIT3;
    P1OUT |= BIT3;
    P1REN |= BIT3;


    // Default to 250 ms gate so that initial call to set_gate()
    // will switch to 1s gate and update the LCD
    WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS0; // use WDT as internal timer

    // LCD test
    writeStringToLCD("Frequency: ");

    // main loop
    for(;;) {
    	freq = 0;
    	TACTL |= TACLR;

    	IFG1 &= ~WDTIFG;		// wait for WDT period begin
    	while(!(IFG1 & WDTIFG));

    	TACTL |= MC_2;

    	IFG1 &= ~WDTIFG;
    	while(!(IFG1 &WDTIFG)) {
    		if(TACTL & TAIFG) {
    			freq += 0x10000L;
    			TACTL &= ~TAIFG;
    		}
    	}

    	TACTL &= ~MC_2;
    	if (TACTL & TAIFG ){
    		freq += 0x10000L;
    	}

    	//last check of overflow and stopping TimeA;
    	freq |= TAR;
    	if (WDTCTL & WDTIS0) freq <<= 2;
    	print_freq(freq);

    	set_gate(freq);

    	if(!((P1IN & BIT3) == BIT3)) {
    		++clock_input;
    		set_input();
    	}
    }

 }

