/******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#ifndef _BUZZER_H
#define _BUZZER_H
#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"
#define BuzzerTerminal_1 PD6
#define BuzzerTerminal_2 PD7

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void buzzer_tone(uint8 frequency); //START THE BUZZER USING FREQUENCY IN KHZ
void buzzerOff();                  //STOP THE BUZZER


#endif
