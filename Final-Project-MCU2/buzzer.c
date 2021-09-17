/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the buzzer driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/
#include "buzzer.h"



/*******************************************************************************
 *                      Global Variables Declarations                          *
 *******************************************************************************/
bool buzzerOn;       //BOOLEAN TO CONTROL THE INFINIE LOOP OF THE BUZZER
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
/*
 * Description : Function to generate clock for the buzzer
 *     1. get the time period
 *     2. Set the pins as output
 *     3. set one pin as ground
 *     4. switch the other pin between high and low according to frequency
 */
void buzzer_tone(uint8 frequency_khz) {
	uint8 timeperiod = (1000/frequency_khz);        //TIMER PERIOD = 1/FREQUENCY the 1k is to get an integer value
	SET_BIT(DDRD, BuzzerTerminal_1);				//SET BUZZER TERMINAL ONE AS OUTPUT
	SET_BIT(DDRD, BuzzerTerminal_2);				//SET BUZZER TERMINAL TWO AS OUTPUT
	CLEAR_BIT(PORTD, BuzzerTerminal_2); 			//ONE TERMINAL IS GROUNDED
	buzzerOn = TRUE;
	while(buzzerOn) {								//INFINITE LOOP SWITCHING BETWEEN HIGH AND LOW ON ONE TERMINAL (CLOCK GENERATION)
		SET_BIT(PORTD, BuzzerTerminal_1);
		_delay_us(timeperiod/2);                    // DELAY BY HALF THE TIME PERIOD IN uS to get the desired Frequency in KHZ
		CLEAR_BIT(PORTD, BuzzerTerminal_1);			// DELAY BY HALF THE TIME PERIOD IN uS to get the desired Frequency in KHZ
		_delay_us(timeperiod/2);
	}
}
/*
 * Description : Function to stop the buzzer
 *     1. Set the boolean to off to stop the loop
 */
void buzzerOff(void) {
	buzzerOn = FALSE;					//SETTING THE BUZZER GLOBAL BOOLEAN TO OFF WILL CLOSE THE LOOP AFTER CONTEXT SWITCH
}
