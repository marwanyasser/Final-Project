/******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.c
 *
 * Description: Source file for the motor driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/
#include "motor.h"

/*******************************************************************************
 *                      Function Definitions                                    *
 *******************************************************************************/

void motor_control(direction motor) { /*Start/Stop the motor in a certain direction*/
	switch (motor) {
	    case stop:
	    	CLEAR_BIT(PORTC, MOTOR_IN1);
	    	CLEAR_BIT(PORTC, MOTOR_IN2);
	    	break;
	    case clockwise:
	    	SET_BIT(PORTC, MOTOR_IN1);
	    	CLEAR_BIT(PORTC, MOTOR_IN2);
	    	break;
	    case anticlockwise:
	    	CLEAR_BIT(PORTC, MOTOR_IN1);
	    	SET_BIT(PORTC, MOTOR_IN2);
	    	break;
	}
}

void motor_init(void) { /*Just initializes the motor but doesn't give it a direction*/
	DDRC |= (1<<MOTOR_ENABLE_PORT) | (1<<MOTOR_IN1) | (1<<MOTOR_IN2);
	PORTC |= (1<<MOTOR_ENABLE_PORT);
	CLEAR_BIT(PORTC, MOTOR_IN1);
	CLEAR_BIT(PORTC, MOTOR_IN2);
}
