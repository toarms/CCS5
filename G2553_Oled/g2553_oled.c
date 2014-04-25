#include<msp430.h>

struct meta_b
{
	signed int x,y;
	unsigned char direction;
};

// P1 is Data-Port (Parallel-Interface)
#define CS		BIT4		// P2.4
#define RES		BIT3		// P2.3
#define DC		BIT2		// P2.2
#define WR		BIT1		// P2.1
#define RD		BIT0		// P2.0

#define lcd_width		128
#define lcd_height		64
#define lcd_height_b	8

#define max_f_size		8
#define meta_w_size		64

unsigned char byte,bit_num;		// will be used for different functions
unsigned int h_index,w_index;
#define space_char	1		// space between chars
#define f_width		5

// 128 chars = 640 bytes (128*5bytes)
const char ascii_table[][f_width]={0xFF,0xFF,0xFF,0xFF,0xFF,	// ASCII 00: all pixel
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 01: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 02: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 03: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 04: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 05: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 06: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 07: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 08: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 09: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0a: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0b: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0c: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0d: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0e: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 0f: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 10: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 11: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 12: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 13: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 14: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 15: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 16: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 17: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 18: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 19: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1a: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1b: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1c: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1d: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1e: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 1f: reserved
							 0x00,0x00,0x00,0x00,0x00,			// ASCII 20: SP (Space)
							 0x00,0x00,0x5f,0x00,0x00, 			// ASCII 21: !
							 0x00,0x07,0x00,0x07,0x00, 			// ASCII 22: "
							 0x14,0x7f,0x14,0x7f,0x14,  		// ASCII 23: #
							 0x24,0x2a,0x7f,0x2a,0x12,  		// ASCII 24: $
							 0x23,0x13,0x08,0x64,0x62,  		// ASCII 25: %
							 0x36,0x49,0x55,0x22,0x50,  		// ASCII 26: &
							 0x00,0x05,0x03,0x00,0x00,  		// ASCII 27: '
							 0x00,0x1c,0x22,0x41,0x00,  		// ASCII 28: (
							 0x00,0x41,0x22,0x1c,0x00,  		// ASCII 29: )
							 0x14,0x08,0x3e,0x08,0x14,  		// ASCII 2a: *
							 0x08,0x08,0x3e,0x08,0x08,  		// ASCII 2b: +
							 0x00,0x50,0x30,0x00,0x00,  		// ASCII 2c: ,
							 0x08,0x08,0x08,0x08,0x08,  		// ASCII 2d: -
							 0x00,0x60,0x60,0x00,0x00,  		// ASCII 2e: .
							 0x20,0x10,0x08,0x04,0x02,  		// ASCII 2f: /
							 0x3e,0x51,0x49,0x45,0x3e,  		// ASCII 30: 0
							 0x00,0x42,0x7f,0x40,0x00,  		// ASCII 31: 1
							 0x42,0x61,0x51,0x49,0x46,  		// ASCII 32: 2
							 0x21,0x41,0x45,0x4b,0x31,  		// ASCII 33: 3
							 0x18,0x14,0x12,0x7f,0x10,  		// ASCII 34: 4
							 0x27,0x45,0x45,0x45,0x39,  		// ASCII 35: 5
							 0x3c,0x4a,0x49,0x49,0x30,  		// ASCII 36: 6
							 0x01,0x71,0x09,0x05,0x03,  		// ASCII 37: 7
							 0x36,0x49,0x49,0x49,0x36,  		// ASCII 38: 8
							 0x06,0x49,0x49,0x29,0x1e,  		// ASCII 39: 9
							 0x00,0x36,0x36,0x00,0x00,  		// ASCII 3a: :
							 0x00,0x56,0x36,0x00,0x00,  		// ASCII 3b: ;
							 0x08,0x14,0x22,0x41,0x00,  		// ASCII 3c: <
							 0x14,0x14,0x14,0x14,0x14,  		// ASCII 3d: =
							 0x00,0x41,0x22,0x14,0x08,  		// ASCII 3e: >
							 0x02,0x01,0x51,0x09,0x06,  		// ASCII 3f: ?
							 0x32,0x49,0x79,0x41,0x3e,  		// ASCII 40: @
							 0x7e,0x11,0x11,0x11,0x7e,  		// ASCII 41: A
							 0x7f,0x49,0x49,0x49,0x36,  		// ASCII 42: B
							 0x3e,0x41,0x41,0x41,0x22,  		// ASCII 43: C
							 0x7f,0x41,0x41,0x22,0x1c,  		// ASCII 44: D
							 0x7f,0x49,0x49,0x49,0x41,  		// ASCII 45: E
							 0x7f,0x09,0x09,0x09,0x01,  		// ASCII 46: F
							 0x3e,0x41,0x49,0x49,0x7a,  		// ASCII 47: G
							 0x7f,0x08,0x08,0x08,0x7f,  		// ASCII 48: H
							 0x00,0x41,0x7f,0x41,0x00,  		// ASCII 49: I
							 0x20,0x40,0x41,0x3f,0x01,  		// ASCII 4a: J
							 0x7f,0x08,0x14,0x22,0x41,  		// ASCII 4b: K
							 0x7f,0x40,0x40,0x40,0x40,  		// ASCII 4c: L
							 0x7f,0x02,0x0c,0x02,0x7f,  		// ASCII 4d: M
							 0x7f,0x04,0x08,0x10,0x7f,  		// ASCII 4e: N
							 0x3e,0x41,0x41,0x41,0x3e,  		// ASCII 4f: O
							 0x7f,0x09,0x09,0x09,0x06,  		// ASCII 50: P
							 0x3e,0x41,0x51,0x21,0x5e,  		// ASCII 51: Q
							 0x7f,0x09,0x19,0x29,0x46,  		// ASCII 52: R
							 0x46,0x49,0x49,0x49,0x31,  		// ASCII 53: S
							 0x01,0x01,0x7f,0x01,0x01,  		// ASCII 54: T
							 0x3f,0x40,0x40,0x40,0x3f,  		// ASCII 55: U
							 0x1f,0x20,0x40,0x20,0x1f,  		// ASCII 56: V
							 0x3f,0x40,0x38,0x40,0x3f,  		// ASCII 57: W
							 0x63,0x14,0x08,0x14,0x63,  		// ASCII 58: X
							 0x07,0x08,0x70,0x08,0x07,  		// ASCII 59: Y
							 0x61,0x51,0x49,0x45,0x43,  		// ASCII 5a: Z
							 0x00,0x7f,0x41,0x41,0x00,  		// ASCII 5b: [
							 0x02,0x04,0x08,0x10,0x20,  		// ASCII 5c: ?
							 0x00,0x41,0x41,0x7f,0x00,  		// ASCII 5d: ]
							 0x04,0x02,0x01,0x02,0x04,  		// ASCII 5e: ^
							 0x40,0x40,0x40,0x40,0x40,  		// ASCII 5f: _
							 0x00,0x01,0x02,0x04,0x00,  		// ASCII 60: `
							 0x20,0x54,0x54,0x54,0x78,  		// ASCII 61: a
							 0x7f,0x48,0x44,0x44,0x38,  		// ASCII 62: b
							 0x38,0x44,0x44,0x44,0x20,  		// ASCII 63: c
							 0x38,0x44,0x44,0x48,0x7f,  		// ASCII 64: d
							 0x38,0x54,0x54,0x54,0x18,  		// ASCII 65: e
							 0x08,0x7e,0x09,0x01,0x02, 			// ASCII 66: f
							 0x0c,0x52,0x52,0x52,0x3e,  		// ASCII 67: g
							 0x7f,0x08,0x04,0x04,0x78,  		// ASCII 68: h
							 0x00,0x44,0x7d,0x40,0x00,  		// ASCII 69: i
							 0x20,0x40,0x44,0x3d,0x00,  		// ASCII 6a: j
							 0x7f,0x10,0x28,0x44,0x00,  		// ASCII 6b: k
							 0x00,0x41,0x7f,0x40,0x00,  		// ASCII 6c: l
							 0x7c,0x04,0x18,0x04,0x78,  		// ASCII 6d: m
							 0x7c,0x08,0x04,0x04,0x78,  		// ASCII 6e: n
							 0x38,0x44,0x44,0x44,0x38,  		// ASCII 6f: o
							 0x7c,0x14,0x14,0x14,0x08,  		// ASCII 70: p
							 0x08,0x14,0x14,0x18,0x7c,  		// ASCII 71: q
							 0x7c,0x08,0x04,0x04,0x08,  		// ASCII 72: r
							 0x48,0x54,0x54,0x54,0x20,  		// ASCII 73: s
							 0x04,0x3f,0x44,0x40,0x20,  		// ASCII 74: t
							 0x3c,0x40,0x40,0x20,0x7c,  		// ASCII 75: u
							 0x1c,0x20,0x40,0x20,0x1c,  		// ASCII 76: v
							 0x3c,0x40,0x30,0x40,0x3c,  		// ASCII 77: w
							 0x44,0x28,0x10,0x28,0x44,  		// ASCII 78: x
							 0x0c,0x50,0x50,0x50,0x3c,  		// ASCII 79: y
							 0x44,0x64,0x54,0x4c,0x44,  		// ASCII 7a: z
							 0x00,0x08,0x36,0x41,0x00,  		// ASCII 7b: {
							 0x00,0x00,0x7f,0x00,0x00,  		// ASCII 7c: |
							 0x00,0x41,0x36,0x08,0x00,  		// ASCII 7d: }
							 0x10,0x08,0x08,0x10,0x08,  		// ASCII 7e: ->
							 0x78,0x46,0x41,0x46,0x78   		// ASCII 7f: <-
							 };

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

static void set_instruction(unsigned char register_sel, unsigned char number);
static void set_cursor(unsigned char x, unsigned char y);
static void fill_display(unsigned char width, unsigned char height, unsigned char byte);
static void init_LCD(void);
static void init_ports(void);
static void wait_us(unsigned int u_sec);
static void wait_ms(unsigned int m_sec);
static void send_data_array(const char *d_array, unsigned char size);
static void write_char(unsigned char x, unsigned char y, unsigned char character, unsigned char f_size);
static void convert_font_size(unsigned char x, unsigned char y, unsigned char character, unsigned char f_size);


static void fill_display(unsigned char width, unsigned char height, unsigned char byte)
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

static void set_instruction(unsigned char register_sel, unsigned char number)
{
	if(register_sel)
		P2OUT|=DC;
	else P2OUT&=~DC;

	P2OUT&=~CS;					// start condition
	P1OUT=number;
	P2OUT|=WR;
	P2OUT&=~WR;
	P2OUT|=CS;
}

static void set_cursor(unsigned char x, unsigned char y)
{
	set_instruction(0,0x0F&x);				// set lower nibble of the column start address
	set_instruction(0,0x10+(x>>4));			// set higher nibble of the column start address
	set_instruction(0,0xB0+y);
}

static void init_LCD(void)
{
	unsigned char byte=sizeof(init_cmd_array);
	wait_ms(100);
	P2OUT|=RES;

	while(byte)
	{
		// really dirty stuff
		set_instruction(0,init_cmd_array[sizeof(init_cmd_array)-byte]);
		byte--;
	}
	// display RAM is undefined after reset, clean dat shit
	fill_display(lcd_width,lcd_height,0x00);
}

static void init_ports(void)
{
	P1DIR|=0xFF;				// parallel port for OLED-Display
	P1OUT&=~0xFF;
	P2DIR|=CS+DC+WR+RD+RES;
	P2OUT&=~RES+WR+RD;			// reset, write, read LOW
	P2OUT|=CS;					// chip select HIGH
}

static void wait_ms(unsigned int m_sec)
{
	while(m_sec--)
		__delay_cycles(16000);	//1000 für 1MHz
}

static void wait_us(unsigned int u_sec)
{
	while(u_sec--)
		__delay_cycles(16);		//1 für 1MHz
}

static void write_char(unsigned char x, unsigned char y, unsigned char character, unsigned char f_size)
{
	x*=(f_width+space_char);
	set_cursor(x,y);
	if(f_size)
		convert_font_size(x,y,character,f_size);
	else
		send_data_array(ascii_table[character],f_width);
}

static void send_data_array(const char *d_array, unsigned char size)
{
	while(size--)
		set_instruction(1,*d_array++);
}

// horizontal resize with cache (every bit in one byte will be resized to f_size, for example:
// f_size=2, every pixel will have a new size of 2*2 pixel, f_size=3, pixel-size: 3*3 pixel etc.
// f_size=1, double height font, only horizontal duplication of pixels (bits), pixel-size: 1*2 pixel
// the vertical duplication is running over a cache (every horizontal row (resized byte to f_size),
// will be write "f_size"-times to the display)
static void convert_font_size(unsigned char x, unsigned char y, unsigned char character, unsigned char f_size)
{
	unsigned char x_char=0,bit_num_b=0,size=0,px_size=0,y_pos_new=0,x_pos_new=0;
	unsigned char cache[max_f_size],i=0;
	byte=0;
	bit_num=0;

	if (f_size==1) size=2;
	else size=f_size;

	while(x_char<f_width)													// test byte, starting at 0 to f_width (font width)
	{
		while(bit_num<8)													// test bit 0..7 of current byte
		{
			if(ascii_table[character][x_char]&(1<<bit_num))					// if bit=1 in byte...
			{
				while(px_size<size)											// duplicate bits (f_size*f_size)
				{
					if(bit_num_b>7&&px_size>0)								// byte overflow, new byte
					{
						set_cursor(x+x_pos_new,y+y_pos_new++);				// set cursor (increment y-new position)
						set_instruction(1,byte);							// send byte
						bit_num_b=0;										// reset bit counter (buffer)
						cache[i++]=byte;									// save byte in cache
						byte=0;												// reset byte
					}
					byte|=(1<<bit_num_b);									// set bit in byte
					px_size++;												// increment pixel duplicate counter
					bit_num_b++;											// increment bit position for buffer
				}
				px_size=0;													// reset pixel duplicate counter
			}
			else bit_num_b+=size;											// bit=0, calculate new bit position in byte
																			// if bit=0, remaining bits are 0, too

			if(bit_num_b>7)													// byte overflow, new byte
			{
				set_cursor(x+x_pos_new,y+y_pos_new++);
				set_instruction(1,byte);
				bit_num_b-=8;
				cache[i++]=byte;
				byte=0;
			}
			bit_num++;														// test next byte in array
		}
		y_pos_new=0;														// reset y-offset
		x_pos_new++;														// increment x-position
		i=0;																// reset cache counter
		if(f_size==1) size=0;												// double height font (only for f_size=1)
		else size--;														// first row is ready, only size-1
		while(size--)
		{
			while(i<f_size)
			{
				set_cursor(x+x_pos_new,y+y_pos_new++);
				set_instruction(1,cache[i++]);								// horizontal cache write
			}
			i=0;
			y_pos_new=0;
			x_pos_new++;
		}
		x_char++;					 // increment byte position
		if(f_size==1) size=2;		 // size correction
		else size=f_size;
		i=0;						// reset cache counter
		bit_num=0;
	}
}

void write_string(unsigned char x, unsigned char y, const char *string, unsigned char f_size)
{
	x*=(f_width+space_char);
	set_cursor(x,y);
	while(*string!=0)
	{
		if(f_size)	// font converter enabled
		{
			convert_font_size(x,y,*string++,f_size);
			x+=(f_size*f_width+space_char);		// new x-position
		}
		else
		{
			send_data_array(ascii_table[*string++],f_width);
			y=space_char;	// space between chars
			while(y--)
				set_instruction(1,0);
		}
	}
}

void main(void)
{
	WDTCTL=WDTPW+WDTHOLD;
	//BCSCTL1=CALBC1_8MHZ;
  	//DCOCTL=CALDCO_8MHZ;

  	init_ports();
  	init_LCD();

  	write_string(0,0,"hello,oled!",2);
  	write_string(0,3,"good",1);
  	write_string(0,5,"9876543210", 1);

  	while(1);
}


