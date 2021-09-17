/******************************************************************************
 *
 * Module: TIMER0
 *
 * File Name: timer0.h
 *
 * Description: Header file for the timer0 driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#ifndef _TIMER0_H_
#define _TIMER0_H_
#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	NOCLK, CLK, CLK_8, CLK_64, CLK_256, CLK_1024, EXCLKFALLING, EXCLKRISING
}prescalar;
typedef enum
{
	norm, cmpr
}timer_mode;  /*Normal or Compare*/

typedef struct
{
	prescalar clock;
	timer_mode mode;
	uint8 initial_value;
	uint8 compare_value;
}timer0_config;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void TIMER0_init(const timer0_config * config); /*Initialization*/
void setInitialValue(uint8);/*set the initial value*/
void setCallbackNormal(const void(*fun));/*set call back function for normal mode*/
void setCallbackCompare(const void (*fun));/*set call back function for compare mode*/
void timerStart(const timer0_config * config);/*start the timer*/
void timerStop(void);/*stop the timer*/

#endif
