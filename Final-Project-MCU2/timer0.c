 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer0.c
 *
 * Description: Source file for the timer0 driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/

#include "timer0.h"
/*******************************************************************************
 *                            Global Variables                                  *
 *******************************************************************************/

static volatile void (*ISR_ptr_normal)(void) = NULL_PTR;
static volatile void (*ISR_ptr_compare)(void) = NULL_PTR;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR (TIMER0_OVF_vect) {
	if(ISR_ptr_normal != NULL_PTR) {
		ISR_ptr_normal();
	}
}
ISR (TIMER0_COMP_vect) {
	if(ISR_ptr_compare != NULL_PTR) {
		ISR_ptr_compare();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the TIMER0 driver
 *     1. reset timer control register to start clean
 *     2. set non pwm mode
 *     3. set mode to normal or compare
 *     4. set inital and compare values
 */
void TIMER0_init(const timer0_config *config) {
	TCCR0 = 0;
	SET_BIT(TCCR0, FOC0);
	if(config->mode) {
		TCCR0 |= (1<<WGM01);
		SET_BIT(TIMSK, OCIE0);
		TCNT0 = config->initial_value;
		OCR0 = config->compare_value;
	}
	else {
		SET_BIT(TIMSK, TOIE0);
		TCNT0 = config->initial_value;
	}
}
/*
 * Description : Function to set the initial value of the timer
 *     1. put given value inside the initial count register
 */
void setInitialValue(uint8 value) {
	TCNT0 = value;
}
/*
 * Description : Function to set the call back function of the normal mode
 *     1. set a global function pointer pointing to given function
 */
void setCallbackNormal(const void(*fun)) {
	ISR_ptr_normal = fun;
}
/*
 * Description : Function to set the call back function of the compare mode
 *     1. set a global function pointer pointing to given function
 */
void setCallbackCompare(const void (*fun)) {
	ISR_ptr_compare = fun;
}
/*
 * Description : Function to start the timer
 *     1. give the timer prescaler clock
 */
void timerStart(const timer0_config *config) {
	TCCR0 |= config->clock;
}
/*
 * Description : Function to start the timer
 *     1. stop the timer (NO CLOCK)
 */
void timerStop(void) {
	TCCR0 &= ~(3);
}
