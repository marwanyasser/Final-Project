/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the uart driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#ifndef _UART_H_
#define _UART_H_
#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"

#define BAUD_PRESCALE (((F_CPU / ((config->USART_BAUDRATE)* 8UL))) - 1)
//#define USART_BAUDRATE 9600


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/


typedef enum
{
	Even, Odd
}parityMode;  /*parity check algorithm*/

typedef enum
{
	Async, Sync
}usartMode;  /*parity check algorithm*/

typedef enum
{
	onebit, twobit
}stopBit;  /*stop bit 1 or 2*/

typedef enum
{
	FALLING, RISING
}clkPolarity;  /*change clock polarity*/

typedef enum
{
	five, six, seven, eight, nine
}charSize;  /*sent or received character size*/

typedef struct
{
	bool ParityError;
	bool FrameError;
	bool DataOverrunError;
	bool DoubleTransmission;
	bool MPCMmode;
	parityMode parity;
	usartMode USART_MODE;
	stopBit stopbit;
	clkPolarity clockPolarity;
	charSize characterSize;
	uint16 USART_BAUDRATE;
}UART_config;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void UART_init(const UART_config*);
void UART_sendByte(const uint8);
uint8 UART_recieveByte();
void UART_sendString(uint8*);
uint8* UART_recieveString();



#endif
