/*******************************************************************************************
 * [FILE NAME]:		ext_int.h
 * [AUTHOR]:		Omar Yousry
 * [DATE CREATED]:	10 Jan 2020
 * [DESCRIPTION]:	This header file contains static configurations and function prototypes
 * 					for external interrupts in AVR ATMEGA-16 Micro-controller
 *******************************************************************************************/
#ifndef EXT_INT_H_
#define EXT_INT_H_

/******************************************************************
 * 				Common Header Files Inclusion					  *
 ******************************************************************/
#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/******************************************************************
 * 				    User-defined Data Types					      *
 ******************************************************************/
typedef enum{
	LOW_LEVEL,ANY_CHANGE,FALLING_EDGE,RISING_EDGE
}EXT_INT_Logic;

typedef enum{
	EXTERNAL_RESISTOR,INTERNAL_PULL_UP
}EXT_INT_Resistor;

typedef struct{
	uint8 intNum;
	EXT_INT_Logic logic;
	EXT_INT_Resistor resistor;
}EXT_INT_ConfigType;

/******************************************************************
 * 				  Public Functions Prototypes					  *
 ******************************************************************/
/*Description: This function initialises External Interrupt:
 * 1. Determines which interrupt to be used INT0 or INT1 or INT2
 * 2. Determines logic to work with either on low level/any change in logic/falling or rising edge
 * 3. Activates internal pull-up resistor if needed*/
void EXT_INT_init(const EXT_INT_ConfigType * Config_Ptr);

/*Description: This function disable External Interrupt:*/
void EXT_INT_DeInit(const uint8 intNum);

/*Description: This function sets call back function for INT0*/
void EXT_INT0_setCallBack (void(*a_ptr)(void));

/*Description: This function sets call back function for INT1*/
void EXT_INT1_setCallBack (void(*a_ptr)(void));

/*Description: This function sets call back function for INT2*/
void EXT_INT2_setCallBack (void(*a_ptr)(void));

#endif /* EXT_INT_H_ */
