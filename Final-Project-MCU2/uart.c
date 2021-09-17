 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/
#include "uart.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void UART_init(const UART_config* config) {
	//RESET
	UCSRA = 0;
	UCSRB = 0;
	UCSRC = 0;

	//BAUDRATE
	UBRRL= (BAUD_PRESCALE);
	UBRRH= (BAUD_PRESCALE>>8);
	//Asynchronus or Synchronus
	if(config->USART_MODE) {SET_BIT(UCSRC, UMSEL);}

	//Stop bit
	if(config->stopbit) {SET_BIT(UCSRC, USBS);}

	//Parity Error
	if(config->ParityError) {SET_BIT(UCSRA, PE);}

	//Data Overrun Error
	if(config->DataOverrunError) {SET_BIT(UCSRA, DOR);}

	//Frame Error
	if(config->FrameError) {SET_BIT(UCSRA, FE);}

	//Transmission Speed
	if(config->DoubleTransmission) {SET_BIT(UCSRA, U2X);}

	//Multi-Processor Communication
	if(config->MPCMmode) {SET_BIT(UCSRA, MPCM);}

	UCSRB = (1<<RXEN) | (1<<TXEN);   //Enabling Transmission and Recieving

	//Character Size
	switch (config->characterSize) {
		case six:
			SET_BIT(UCSRC, UCSZ0);
			break;
		case seven:
			SET_BIT(UCSRC, UCSZ1);
			break;
		case eight:
			SET_BIT(UCSRC, UCSZ1);
			SET_BIT(UCSRC, UCSZ0);
			break;
		case nine:
			SET_BIT(UCSRC, UCSZ1);
			SET_BIT(UCSRC, UCSZ0);
			SET_BIT(UCSRC, UCSZ2);
			break;
		default:
			break;
	}

	//Parity Mode
	if(config->parity) {SET_BIT(UCSRC, UPM1);}
	else if(config->parity) {SET_BIT(UCSRC, UPM1); SET_BIT(UCSRC, UPM0);}

	//Clock Polarity
	if(config->clockPolarity == RISING) {SET_BIT(UCSRC, UCPOL);}

}

void UART_sendByte(const uint8 data) {
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	UDR = data;
}


void UART_sendString(uint8 *str) {
	uint8 i = 0;
	while(str[i] != '\0')
	{
		UART_sendByte(str[i]);
		i++;
	}
	UART_sendByte('*');
}

uint8 UART_recieveByte() {
	while(BIT_IS_CLEAR(UCSRA,RXC)){}
    return UDR;
}


void UART_receiveString(uint8 *str) {
	uint8 i = 0;
	str[i] = UART_recieveByte();
	while(str[i] != '*')
	{
		i++;
		str[i] = UART_recieveByte();
	}
	str[i] = '\0';
}

