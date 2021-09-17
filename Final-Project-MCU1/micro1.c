/******************************************************************************
 *
 * Module: MICRO1
 *
 * File Name: micro1.c
 *
 * Description: Source file for the micro1 application layer
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "common_macros.h"
#include "mydelay.h"


#define PASSWORD_LENGTH 5
#define LCD_WIDTH 16
#if PASSWORD_LENGTH > LCD_WIDTH
#error Passcode cant be more than 16     //lcd maximum row capacity
#endif

#define CONFIRM 1
#define ERROR 0
#define REPEATED_MISTAKE 2
#define MAIN_MENU 3
#define OPEN_DOOR 4
#define NEW_PASSCODE 5

#define DONE 7

#define PW_FOUND 20
#define PW_NOTFOUND 30

#define DOOR_UNLOCKING 8
#define DOOR_LOCKING 9
#define DOOR_STOP 10

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
//-----------------------------------------------------------------------------------------------------------------///
/*
 * Description : Function to output a question/message that requires user to input a passcode
 *     1. display the message
 *     2. keep looping until u get a number between 1 and 10
 *     3.
 */
void printoutmessage_takeinput(char* string, uint8* arr) { //printout the message on the lcd screen then take input from user
	int i = 0;
	LCD_displayString(string);
	LCD_goToRowColumn(1,0);// changing the lcd position
	for(i= 0; i < PASSWORD_LENGTH; i++) {
not_number:                //if user pressed any key we go back here
		if(KeyPad_getPressedKey() < 10) { //no + or - or weird characters
			arr[i] = KeyPad_getPressedKey();
			LCD_displayCharacter('*');//to make it more user friendly and give feedback that a click is saved
			delay_10xN(60);
		}
		else {
			goto not_number;
		}
	}
}
/*
 * Description : Function to send password to MC2
 *     1. send byte by byte the password
 */
void sendPassword(const uint8* arr) { //loop and send the entire password to microcontroller2
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		UART_sendByte(arr[i]);
	}
}
/*
 * Description : Function to get password from MC2
 *     1. recieve byte by byte the password
 */
void recievePassword(uint8* arr) { //loop and send the entire password to microcontroller2
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		arr[i]= UART_recieveByte();
	}
}
/*
 * Description : Function to communicate with MC1 and ask the user to confirm the passcode
 * MC1:
 *	   1. output a message demanding user confirmation
 *	   2. send the password to MC2
 *
 * MC2
 *     1. recieve the password from MC1
 *     2. get the current password from the eeprom
 *     3. if they are the same return true else false
 */
void confirmPasscode(uint8* arr) {
	LCD_clearScreen();
	LCD_goToRowColumn(0,0);
	printoutmessage_takeinput("CONFIRM PASSCODE:", arr);
	sendPassword(arr);
}
/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/
int main(void) {
	//variables declarations
	uint8 arr[PASSWORD_LENGTH];
	uint8 uC_Message;
	uint8 i;
	// preferals initializations
	LCD_init(); //initialize the lcd
	UART_config uart_config = {FALSE, FALSE, FALSE, TRUE, FALSE, Even, Async, onebit, FALLING, eight, 9600}; //uart configuration structure
	UART_init(&uart_config); // initialize the uart

	//microcontrollers loop code
	//while(1){                  Figured i dont need a while 1 since my program is like a FSM only jumping between states never ending loop :)
	//first time the user uses our door lock system [No Password on the EEPROM]


//state0              ///checking here if we already saved a password on the EEPROM or this is the first time

		recievePassword(arr);
		for(i = 0; i < PASSWORD_LENGTH; i++) { //checking if the passcodes are the same
			if(arr[i] > 10) {					//MAXIMUM KEYPAD NUMBER
				UART_sendByte(PW_NOTFOUND);     //IF IT'S WRONG ONLY ONCE IS ENOUGH TO TELL THERE IS NO PASSWORD (0XFF) EMPTY EEPROM
				goto state1;					//GO TO FIRST STAGE TO SETUP THE FIRST PASSWORD OF THIS DOOR
			}
		}
		UART_sendByte(PW_FOUND);				//IF WE DIDNT JUMP TO STATE1 SO WE KNOW THE INITIAL PASSWORD IS SAVED ON EEPROM
		goto main_menu;							//GO TO MAIN MENU (WE CONFIRM PASSWORD UPON ACTION NOT BOOTUP TO AVOID THEFT AFTER POWER OUTAGE


state1:                //treating the door like a finite-state machine throughout my code
		printoutmessage_takeinput("NEW PASSCODE:", arr);
		sendPassword(arr);
		LCD_clearScreen();
		LCD_goToRowColumn(0,0);
		printoutmessage_takeinput("CONFIRM PASSCODE:", arr);
		LCD_clearScreen();
		LCD_goToRowColumn(0,0);
		LCD_displayString("CHECKING..."); // MAKING IT MORE USER FRIENDLY SHOWING MESSAGES THROUGHT THE RUNTIME USING DELAY BELOW
		sendPassword(arr);
		delay_10xN(200);					// DELAY TO ALERT THE USER ACTION IS BEING TAKEN


		//First Password Transaction is Complete and we are doing verification now
		uC_Message = UART_recieveByte(); //Microcontroller2 is ready verifying
		LCD_clearScreen();
		if(uC_Message == CONFIRM) {
			LCD_displayString("CONFIRM"); //SHOWING THE USER THE PASSWORDS INDEED MATCH
			delay_10xN(200);
			goto main_menu;					//JUMP TO MAIN MENU
		}
		else if(uC_Message == ERROR) {
			LCD_displayString("PASSCODE");
			LCD_goToRowColumn(1,0);
			LCD_displayString("MISMATCH");  //TELLING THE USER THERE IS A MISMATCH
			delay_10xN(200);				// DELAY TO ALERT THE USER ACTION IS BEING TAKEN
			LCD_clearScreen();
			goto state1;					//GO BACK TO INITIAL STATE
		}
		else if(uC_Message == REPEATED_MISTAKE) {	//REPEATED MISTAKE FREEZE THE TIMER UNTIL BUZZER IS OFF
			LCD_displayString("ERROR");				//ALERTING THE USER THAT THERE IS A MISTAKE
			while(UART_recieveByte() != DONE) {}
			LCD_clearScreen();
			goto state1;							//PEOPLE ARE ALERT WE CAN GO BACK TO INITIAL STATE
		}

main_menu:										//OUR MAIN STATE
		LCD_clearScreen();
		LCD_displayString("+ NEW PASSCODE");
		LCD_goToRowColumn(1,0);
		LCD_displayString("- OPEN DOOR");
		while(1) {
		if(KeyPad_getPressedKey() == '+') {
			UART_sendByte(NEW_PASSCODE);
NEWPASSCODE_CONFIRMATION:						//GO BACK LABEL IF THERE IS A MISMATCH
			confirmPasscode(arr);
			uC_Message = UART_recieveByte();
			if(uC_Message == CONFIRM) {
				LCD_clearScreen();
				goto state1;					//GO BACK TO INITIAL OUT OF THE BOX EXPERIENCE
			}
			else if(uC_Message == ERROR) {
				LCD_clearScreen();
				LCD_displayString("WRONG");
				LCD_goToRowColumn(1,0);
				LCD_displayString("PASSCODE");
				delay_10xN(200);
				goto NEWPASSCODE_CONFIRMATION;		//GO BACK LABEL IF THERE IS A MISMATCH
			}
			else if(uC_Message == REPEATED_MISTAKE) {	//REPEATED MISTAKE BUZZER ON
				LCD_clearScreen();
				LCD_displayString("ERROR");
				while(UART_recieveByte() != DONE) {}	//FREEZE UNTIL BUZZER IS OFF AND MC2 IS DONE
				LCD_clearScreen();
				goto main_menu;							//BACK TO MAIN MENU
			}
		}
		else if(KeyPad_getPressedKey() == '-') {
			UART_sendByte(OPEN_DOOR);
OPENDOOR_CONFIRMATION:									//GO BACK LABEL IF THERE IS A MISMATCH
			confirmPasscode(arr);
			uC_Message = UART_recieveByte();
			if(uC_Message == CONFIRM) {
				while(UART_recieveByte()!= DOOR_UNLOCKING) {}     //WAIT HANDSHAKE FROM MC2 THAT DOOR IS UNLOCKING
				LCD_clearScreen();
				LCD_goToRowColumn(0,0);
				LCD_displayString("DOOR UNLOCKING");				//BETTER USER EXPERIENCE SHOWING IT THE DOOR IS UNLOCKING


				while(UART_recieveByte()!= DOOR_STOP) {}			 //WAIT HANDSHAKE FROM MC2 THAT DOOR IS FULLY OPEN
				LCD_clearScreen();
				LCD_goToRowColumn(0,0);
				LCD_displayString("DOOR OPEN");							//BETTER USER EXPERIENCE SHOWING IT THE DOOR IS FULLY OPEN

				while(UART_recieveByte()!= DOOR_LOCKING) {}				//WAIT HANDSHAKE FROM MC2 THAT DOOR IS CLOSING
				LCD_clearScreen();
				LCD_goToRowColumn(0,0);
				LCD_displayString("DOOR LOCKING");					//BETTER USER EXPERIENCE SHOWING IT THE DOOR IS CLOSING
				while(UART_recieveByte()!= DONE) {}
				goto main_menu;										//DOOR IS OPEN THEN CLOSED WE CAN GO BACK TO OUR MENU
			}

			else if(uC_Message == ERROR) {							//NORMAL ERROR
				LCD_clearScreen();
				LCD_displayString("WRONG");
				LCD_goToRowColumn(1,0);
				LCD_displayString("PASSCODE");
				delay_10xN(200);
				goto OPENDOOR_CONFIRMATION;							//REPEAT STEPS THERE IS ANOTHER CHANCES
			}

			else if(uC_Message == REPEATED_MISTAKE) {				//REPEATED MISTAKE BUZZER IS ON
				LCD_clearScreen();
				LCD_displayString("ERROR");
				while(UART_recieveByte() != DONE) {}        		//MC1 FREEZE UNTILL BUZZER IS OFF
				LCD_clearScreen();
				goto main_menu;										//WE CAN GO BACK TO OUR MAIN MENU
			}
		}
	}
}





