/*
 * digital_stopwatch.c
 *
 *  Created on: 11 Dec 2019
 *      Author: Omar Yousry
 *  Digital Stopwatch using:
 *  1- TIMER1 in CTC Mode in AVR ATMEGA16
 *  2- F_CPU = 1 MHz and N=64 (Pre-scalar)
 *  3- Multiplexed 6 Common Anode 7-segments display with 2 7-segment displays for each of
 *     seconds, minutes, hours and BCD to 7-segment decoder (7447)
 *  4- Switch on INT0 to reset the clock (Falling edge using internal pull-up resistor)
 *  5- Switch on INT1 to pause the clock (Rising edge using external pull-down resistor)
 *  6- Switch on INT2 to resume the clock (Falling edge using internal pull-up resistor)
 *  7- PORTA from PA0 to PA5 to Base pins for each transistor to enable 7-segment displays
 *  8- PORTC from PC0 to PC3 to 7447 decoder to output the time digits
 */


#include "ext_int.h"
#include "timer1.h"

/*[HR2,HR1,MIN2,MIN1,SEC2,SEC1]
 *[ 0 , 1 ,  2 , 3  , 4  , 5  ]*/
#define HR2 	0
#define HR1 	1
#define MIN2 	2
#define MIN1 	3
#define SEC2 	4
#define SEC1 	5


/*Global array for clock elements*/
uint8 time[6] = {0,0,0,0,0,0};
/*Global variable for seconds count*/
uint8 g_sec=0;
/*Global variable for minutes count*/
uint8 g_min=0;
/*Global variable for hours count*/
uint8 g_hr=0;


void reset(void)
{
	/*if switch is pressed, clear all values of clock*/
	g_sec=0;
	g_min=0;
	g_hr=0;
	/*To display seconds on 2 7-segment displays*/
	time[SEC2]=g_sec/10;
	time[SEC1]=g_sec%10;
	/*To display minutes on 2 7-segment displays*/
	time[MIN2]=g_min/10;
	time[MIN1]=g_min%10;
	/*To display hours on 2 7-segment displays*/
	time[HR2]=g_hr/10;
	time[HR1]=g_hr%10;
}

void pause(void)
{
	/*if switch is pressed, PORTC pins holds the same value
	Turn-off timer1 by clearing Output Compare A Match Interrupt Enable pin in TIMSK Register*/
	TIMSK &= ~(1<<OCIE1A);
}

void resume(void)
{
	/*if switch is pressed, PORTC pins resumes
	Turn-on timer1 by setting Output Compare A Match Interrupt Enable pin in TIMSK Register*/
	TIMSK |= (1<<OCIE1A);
}

void stopwatch(void)
{
	/*Increment seconds count every ISR*/
	g_sec++;
	/*Increment minutes count every 60 seconds*/
	if(g_sec == 60)
	{
		g_sec=0;
		g_min++;
	}
	/*Increment hours count every 60 minutes*/
	if(g_min == 60)
	{
		g_min=0;
		g_hr++;
	}
	/*Reset clock if hours count reached 24*/
	if(g_hr == 24)
	{
		g_hr=0;
	}
	/*To display seconds on 2 7-segment displays*/
	time[SEC2]=g_sec/10;
	time[SEC1]=g_sec%10;
	/*To display minutes on 2 7-segment displays*/
	time[MIN2]=g_min/10;
	time[MIN1]=g_min%10;
	/*To display hours on 2 7-segment displays*/
	time[HR2]=g_hr/10;
	time[HR1]=g_hr%10;
}


int main()
{
	/*Configuration structure to initialise TIMER1 module:
	 * 1. CTC (Compare) mode
	 * 2. Prescaler  = 64
	 * 3. Initial value = 0
	 * 4. Compare value = 15625 (to generate 1 sec)*/
	TIMER1_ConfigType Timer1_Config={CTC,F_CPU_64,0,15625};
	/*Configuration structure to initialise INT0:
	 * 1. INT number = 0
	 * 2. Event detection is at Falling edge
	 * 3. Internal Pull-up resistor is activated*/
	EXT_INT_ConfigType INT0_Config={0,FALLING_EDGE,INTERNAL_PULL_UP};
	/*Configuration structure to initialise INT1:
	 * 1. INT number = 1
	 * 2. Event detection is at Rising edge
	 * 3. No Pull-up resistor is activated*/
	EXT_INT_ConfigType INT1_Config={1,RISING_EDGE,EXTERNAL_RESISTOR};
	/*Configuration structure to initialise INT2:
	 * 1. INT number = 2
	 * 2. Event detection is at Rising edge
	 * 3. Internal Pull-up resistor is activated*/
	EXT_INT_ConfigType INT2_Config={2,FALLING_EDGE,INTERNAL_PULL_UP};
	/*Initialise TIMER1 with the specifications in configuration structure*/
	TIMER1_init(&Timer1_Config);
	/*Initialise INT0 with the specifications in configuration structure*/
	EXT_INT_init(&INT0_Config);
	/*Initialise INT1 with the specifications in configuration structure*/
	EXT_INT_init(&INT1_Config);
	/*Initialise INT2 with the specifications in configuration structure*/
	EXT_INT_init(&INT2_Config);
	/*Set callback function for INT0 --> reset function*/
	EXT_INT0_setCallBack(reset);
	/*Set callback function for INT1 --> pause function*/
	EXT_INT1_setCallBack(pause);
	/*Set callback function for INT2 --> resume function*/
	EXT_INT2_setCallBack(resume);
	/*Set callback function for TIMER1 --> stopwatch function*/
	TIMER1_setCallBack(stopwatch);

	/*Enable I-bit (Global Interrupt Enable)*/
	sei();
	/*Configure PA0 --> PA5 to be output for transistors drivers of 7-segment*/
	DDRA |= ((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4) | (1<<PA5));
	/*Configure PA0 --> PA5 to be initially OFF*/
	PORTA &= ~((1<<PA0) | (1<<PA1) | (1<<PA2) | (1<<PA3) | (1<<PA4) | (1<<PA5));
	/*Configure PC0 --> PC3 to be output for BCD to 7-segment decoder*/
	DDRC |= ((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3));
	/*Configure PC0 --> PC3 to be initially OFF*/
	PORTC &= ~((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3));
	/*Variable used in while(1) to loop on the 6 7-segment displays*/
	uint8 loop_idx=0;

	while(1)
	{
		/*Loop to make all the 7-segment displays active all time*/
		for(loop_idx=0;loop_idx<6;loop_idx++)
		{
			/*Enable 7-segment*/
			PORTA = (1<<loop_idx);
			/*Display digit*/
			PORTC = time[5-loop_idx];
			/*Delay to see the digit*/
			_delay_ms(4);
			/*Disable 7-segment*/
			PORTA = (0<<loop_idx);
		}
	}
}
