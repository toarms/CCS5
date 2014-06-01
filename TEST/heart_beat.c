/* MSP430F247
 * sensor Pin: P4.1, timer B, capture mode.
 * Led control pin: p2.2, P2.3
 *
 */


#define MALENGTH 64

// the result
unsigned int start_counter;
unsigned int last_counter;
unsigned int pulse_counts;
unsigned int rcounter, icounter;
unsigned int rcounts=999, icounts=777; 


void led_on()
{
    /*
     * 红外LED 每秒钟脉冲亮120次, 每次持续1ms. 用timer来做此操作。
     * 在这亮的1ms内, 调用函数 get_sensor_data() 来获取sensor的值. 
     * 然后调用 calculate_heart_beats() 计算每分钟心跳次数。
     * 函数间通过全局变量传递参数。
     * 
     */
}

/* caputure the sensor data, from P4.1 */
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

/*
 * return the heart beats
 */
int calculate_heart_beat()
{
      static unsigned int buf[MALENGTH];
      static int offset = 0, up =0, heart_beats = 0, ir_counts =0, beats_per_second=0;
      static int last_counter = 0, last_z=0;
      unsigned int counter, counts;
      int i=0;
      int z=0;

      buf[offset] = icounter > 1000 ? last_counter : icounter;
      last_counter = icounter;

      ir_counts++;
      for (i=0; i<MALENGTH; i++)
	      z += buf[i];
      z = z/MALENGTH;

      if(up){
	      if (z < last_z) // 3 heart beat
	      {
		      up = 0;
		      if(++heart_beats == 3)
		      {
			      beats_per_second = (21600)/ir_counts;  // 21600 = 120*60*3
			      ir_counts = 0;
			      heart_beats = 0;
		      }
	      }
      } else if (z > last_z) {
	      up = 1;
      }

      // prepare
      last_z = z;
      if(++offset==MALENGTH)
	      offset = 0;

    return beats_per_second;
}
