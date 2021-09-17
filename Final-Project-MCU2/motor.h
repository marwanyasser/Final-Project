/******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.h
 *
 * Description: Header file for the motor driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#ifndef _MOTOR_H
#define _MOTOR_H
#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"

#define MOTOR_ENABLE_PORT PC7
#define MOTOR_IN1 PC5
#define MOTOR_IN2 PC6
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/


typedef enum
{
	stop, clockwise, anticlockwise
}direction;  /*parity check algorithm*/
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void motor_init(void);
void motor_control(direction);

#endif
