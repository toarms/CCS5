/*
 * main.c
 */
#include <msp430.h>

static void set_instruction(unsigned char,unsigned char);
static void disc(unsigned char X,unsigned char Y,unsigned char * c);
static void init_oled(void);

struct meta_b
{
	signed int x,y;
	unsigned char direction;
};


// P1 is Data-Port (Parallel-Interface)
#define CS		BIT3		// P6.3
#define RES		BIT4		// P6.4
#define DC		BIT5		// P6.5
#define WR		BIT6		// P6.6
#define RD		BIT7		// P6.7

#define lcd_width		128
#define lcd_height		64
#define lcd_height_b	8

#define space_char	1		// space between chars


void delay()
{
	volatile unsigned int i;	// volatile to prevent optimization

	i = 10000;					// SW Delay
	do i--;
	while(i != 0);

}

void wait_ms(unsigned int m_sec)
{
	while(m_sec--)
		__delay_cycles(1000);	//1000 für 1MHz
}

void wait_us(unsigned int u_sec)
{
	while(u_sec--)
		__delay_cycles(1);		//1 für 1MHz
}

void set_instruction(unsigned char register_sel, unsigned char number)
{
	if(register_sel)
		P6OUT|=DC;
	else P6OUT&=~DC;

	P6OUT |= CS;
	P6OUT |= WR;
	P6OUT |= RD;

	P6OUT &= ~CS;					// start condition
	P1OUT = number;
	P6OUT |= WR;
	P6OUT &= ~WR;
	P6OUT |= CS;
}

/* init bus */
void init_ports(void)
{
	P1DIR |= 0xFF;				// parallel port for OLED-Display
	P1OUT &= ~0xFF;
	P6DIR |= CS+DC+WR+RD+RES;
	P6OUT &= ~RES+WR+RD;			// reset, write, read LOW
	P6OUT |= CS;					// chip select HIGH
}

const char init_cmd_array[]={0xAE,			// DISPLAY OFF
							 0xD5,			// SET OSC FREQUENY
							 0x80,			// divide ratio = 1 (bit 3-0), OSC (bit 7-4)
							 0xA8,			// SET MUX RATIO
							 0x3F,			// 64MUX
							 0xD3,			// SET DISPLAY OFFSET
							 0x00,			// offset = 0
							 0x40,			// set display start line, start line = 0
							 0x8D,			// ENABLE CHARGE PUMP REGULATOR
							 0x14,			//
							 0x20,			// SET MEMORY ADDRESSING MODE
							 0x02,			// horizontal addressing mode
							 0xA1,			// set segment re-map, column address 127 is mapped to SEG0
							 0xC8,			// set COM/Output scan direction, remapped mode (COM[N-1] to COM0)
							 0xDA,			// SET COM PINS HARDWARE CONFIGURATION
							 0x12,			// alternative COM pin configuration
							 0x81,			// SET CONTRAST CONTROL
							 0xCF,			//
							 0xD9,			// SET PRE CHARGE PERIOD
							 0xF1,			//
							 0xDB,			// SET V_COMH DESELECT LEVEL
							 0x40,			//
							 0xA4,			// DISABLE ENTIRE DISPLAY ON
							 0xA6,			// NORMAL MODE (A7 for inverse display)
							 0xAF};			// DISPLAY ON

void set_cursor(unsigned char x, unsigned char y)
{
	set_instruction(0,0x0F&x);				// set lower nibble of the column start address
	set_instruction(0,0x10+(x>>4));			// set higher nibble of the column start address
	set_instruction(0,0xB0+y);
}

void fill_display(unsigned char width, unsigned char height, unsigned char byte)
{
	height/=8;
	while(height--)
	{
		set_cursor(0,height);
		while(width--)
			set_instruction(1,byte);
		width=lcd_width;
	}
}

void init_LCD(void)
{
	unsigned char byte=sizeof(init_cmd_array);
	wait_ms(100);
	P6OUT |= RES;

	while(byte)
	{
		// really dirty stuff
		set_instruction(0,init_cmd_array[sizeof(init_cmd_array)-byte]);
		byte--;
	}
	// display RAM is undefined after reset, clean dat shit
	fill_display(lcd_width,lcd_height,0x00);
}


void main(void) {
	WDTCTL	= WDTPW + WDTHOLD;		// Stop watchdog timer
//	BCSCTL1	= CALBC1_1MHZ;
 // 	DCOCTL	= CALDCO_1MHZ;
	P2DIR |= 0xff;
	P2OUT |=  BIT2 + BIT3;
	
  	init_ports();
  	init_LCD();

	while(1){
		P2OUT ^= BIT2;
		P2OUT ^= BIT3;
		delay();
	}
}
