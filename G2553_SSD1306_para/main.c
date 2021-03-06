#include <msp430g2553.h>
#include "init_SSD1306.h"
#include "lib_lcd.h"
#include "lib_math.h"
#include "delay.h"
#include "fuck.h"

#define meta_size	3

void scene_05_scroller_help(const char*,const char*);
void scene_00_typer_help(unsigned char);

signed int x=128,y=0,x1=128,y1=0,x2=128,y2=127;
unsigned char time_ms=80;
struct meta_b m_balls[meta_size];

void main(void)
{
	WDTCTL=WDTPW+WDTHOLD;
	BCSCTL1=CALBC1_16MHZ;
  	DCOCTL=CALDCO_16MHZ;

	m_balls[0].x=10; m_balls[0].y=20; m_balls[0].direction=4;
	m_balls[1].x=30; m_balls[1].y=50; m_balls[1].direction=2;
	m_balls[2].x=45; m_balls[2].y=25; m_balls[2].direction=3;

  	init_ports();
  	init_LCD();


  	// SCENE 00 - TYPER INTRO ---------------------------------------
  	x=0;
  	scene_00_typer_help(0);
  	scene_00_typer_help(4);
  	wait_ms(time_ms);
  	write_h_string(4,0,str_scene00_3,0);
  	wait_ms(time_ms);
  	write_h_string(17,0,str_scene00_3,0);
  	wait_ms(500);
  	string_typer(5,1,str_scene00_0,3,1500);

	x=lcd_width;
	while(x!=-60)
	{
		draw_string_sin(x--,44,str_scene00_1,3);
		wait_ms(30);
		clean_area(0,lcd_width,5,lcd_height_b);
	}
	wait_ms(2000);
  	fill_display(lcd_width,lcd_height,0x00);
  	wait_ms(1000);
  	// --------------------------------------------------------------

	// SCENE 01 - CITY-SCROLLER -------------------------------------
	x=lcd_width,y=0,x1=lcd_width,y1=0,x2=lcd_width,y2=lcd_width-1;
	while(x>=-400)
	{
		if(x<1) 			// start normal scroller
		{
			draw_string(x1,y,str_scene01_2);
			if(y1) y--; 	// y1 = direction
			else y++;
			if(y==7) y1=1;	// moving up
			if(!y) y1=0;	// moving down
			x1-=2;
		}
		draw_bitmap(x2--,16,cityb_w,cityb_h,cityb);
		if(x2+(signed int)cityb_w<lcd_width)
			draw_bitmap(y2--,16,cityb_w,cityb_h,cityb);
		x--;
		wait_ms(30);
		clean_area(0,lcd_width,0,1);	// scroller area
	}
	// --------------------------------------------------------------

	// SCENE 02 - BOUNCE DAT BMP ------------------------------------
	bouncy_bmp(85,64,10,baabis_w,baabis_h,baabis);
	wait_ms(1000);
	write_h_string(12,0,str_scene02_hoo,0);
	wait_ms(1000);
	// --------------------------------------------------------------

	// SCENE 03 - METABALLS PHYSICS ---------------------------------
	y=0;
	while(y<=14000)
	{
		draw_metaballs(m_balls,meta_size,y);
		y+=100;
	}

	y1=0;y2=3;x=1;
	while(y2)
	{
		draw_metaballs(m_balls,meta_size,y);
		meta_ball_physics(m_balls,meta_size,x,5);
		if(y<=2000)
		{
			y1=1;
			y2--;	// cycles
			x++;	// pos_change
			set_instruction(0,0xA7);	// invert
		}
		if(y>=25000)
		{
			y1=0;
			set_instruction(0,0xA6);	// normal
		}
		if(!y1) y-=200;	// bigger
		else y+=200;	// smaller
	}
	// --------------------------------------------------------------

	set_instruction(0,0xA6);
	clean_area(0,84,0,8);
	wait_ms(1000);

	// SCENE 04 - BMP SCROLL OUT + SINE SCROLLER --------------------
	x=85;x1=lcd_width;x2=0;
	while(x!=-75)
	{
		draw_bitmap(x--,0,baabis_w,baabis_h,baabis);
		if(x<40)
			draw_string_sin_fixed(x1--,24,str_scene05_4,x2++,2);

		wait_ms(20);
		clean_area(x1,lcd_width,3,5);
	}
	// --------------------------------------------------------------

	// SCENE 05 - FIXED SINE STRING + NORMAL SCROLLER ---------------
	scene_05_scroller_help(str_scene05_4,str_scene05_5);

		// transition 00
	y1=24;y2=lcd_height+10;
	while(y2>=24)
	{
		draw_string_sin_fixed(x1,y1--,str_scene05_4,x2++,2);
		if(y1<10)
			draw_string_sin_fixed(x1,y2--,str_scene05_6,x2++,2);
		wait_ms(20);
		clean_area(x1,lcd_width,0,lcd_height_b);
	}

	scene_05_scroller_help(str_scene05_6,str_scene05_5);

		// transition 01
	y1=-(6*(sizeof(str_scene05_6)-1));
	while(x1>=y1)
	{
		draw_string_sin_fixed(x1--,y2,str_scene05_6,x2++,2);
		wait_ms(15);
		clean_area(0,lcd_width,2,5);
	}
	// --------------------------------------------------------------

	// SCENE 06 - OUTRO SINE SCROLLER -------------------------------
	bouncy_bmp(1,64,5,smiley1b_w,smiley1b_h,smiley1b);
	draw_bitmap(1,32,smiley1b_w,smiley1b_h,smiley1b); // move 1px down
	x=lcd_width;x1=lcd_width;x2=-(6*(sizeof(str_scene06_7)-1));y=0;
	while(x1>=x2)
	{
		if(x>=40)
		{
			draw_string(x,32,str_scene06_8);
			draw_string(x,40,str_scene06_9);
			draw_string(x,48,str_scene06_10);
			draw_string(x,56,str_scene06_11);
			x--;
		}
		draw_string_sin_fixed(x1--,0,str_scene06_7,y++,2);
		wait_ms(17);
		clean_area(0,lcd_width,0,3);
	}
	wait_ms(1000);
	fill_display(lcd_width,lcd_height,0x00);
	// --------------------------------------------------------------

	write_string(2,3,str_scene06_12,3);
	wait_ms(4000);
	fill_display(lcd_width,lcd_height,0x00);

	while(1);
}

// HELP FUNCTIONS for smaller code size (a liitle bit dirty :D)--

// SCENE 05
void scene_05_scroller_help(const char *str_mid, const char *str_up_down)
{
	x=lcd_width;y=-(6*(sizeof(str_scene05_5)-1));y1=2;
	while(y1)
	{
		draw_string_sin_fixed(x1,24,str_mid,x2++,2);
		draw_string(x--,0,str_up_down);
		draw_string(y++,56,str_up_down);
		if(y>lcd_width)
		{
			x=lcd_width;
			y=-(6*sizeof(str_scene05_5)-1);
			y1--;
		}
		wait_ms(15);
		clean_area(x1,lcd_width,3,5);
	}
}

// SCENE 00
void scene_00_typer_help(unsigned char y_pos)
{
  	while(x<3)
  	{
  		string_typer(5+(x*(sizeof(str_scene00_0)-1)),y_pos,str_scene00_0,0,time_ms);
  		x++;
  	}
  	x=0;
}
// --------------------------------------------------------------
