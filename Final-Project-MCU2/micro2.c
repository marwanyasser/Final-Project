/******************************************************************************
 *
 * Module: MICRO2
 *
 * File Name: micro2.c
 *
 * Description: Source file for the micro2 application layer
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#include "uart.h"
#include "mydelay.h"
#include "buzzer.h"
#include "motor.h"
#include "timer0.h"
#include "external_eeprom.h"

#define PASSWORD_LENGTH 5

#if PASSWORD_LENGTH > 16
#error Passcode cant be more than 16          //lcd capacity
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


#define DOOR_MOVING 15
#define DOOR_STOPPED 3
#define BUZZTIME 60
#define OVERFLOWS_per_SEC (31)
#define PASSWORD_INITIAL_LOCATION (0x00)
/*******************************************************************************
 *                         Global Variables                                     *
 *******************************************************************************/

volatile uint8 ticks = 0;
uint8 seconds = 0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to get password from MC1
 *     1. recieve byte by byte the password
 */
void recievePassword(uint8* arr) { //loop and send the entire password to microcontroller2
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		arr[i]= UART_recieveByte();
	}
}
/*
 * Description : Function to send password to MC1
 *     1. send byte by byte the password
 */
void sendPassword(const uint8* arr) { //loop and send the entire password to microcontroller2
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		UART_sendByte(arr[i]);
	}
}
/*
 * Description : Function to increment ticks and seconds using ISR callback
 *     1. increment ticks
 *     2. increment seconds if it get enough ticks
 */
void tickIncrement() {                           //tick increment function to use in timer ISR to count time freely
	ticks++;
	if(ticks == OVERFLOWS_per_SEC) {
		seconds++;
		ticks = 0;
	}
}
/*
 * Description : Function to use inside the ISR	since it will be in an infinite loop so it have to be closed inside the ISR
 *     1. increment ticks
 *     2. increment seconds if it get enough ticks
 *     3. stop the buzzer if we meet the defined buzztime
 */
void buzzerTickIncrement() {                     // function to count seconds for the buzzer to pass to the ISR
	tickIncrement();
	if(seconds == BUZZTIME) {
		buzzerOff();
	}
}
/*
 * Description : Function to get saved Passcode from EEPROM
 *     1. read byte by byte the stored values inside the defined initial location
 */
void getPasscode(uint8* data) {                  // function to get passcode from eeprom on start address PASSWORD_INITIAL_LOCATION
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		EEPROM_readByte((PASSWORD_INITIAL_LOCATION | i), &(data[i]));
		delay_10xN(10);
	}
}
/*
 * Description : Function save Passcode on EEPROM
 *     1. write byte by byte the stored values inside the defined initial location
 */
void savePasscode(uint8* data) {                // function to save passcode on eeprom on start address PASSWORD_INITIAL_LOCATION
	int i;
	for(i = 0; i < PASSWORD_LENGTH; i++) {
		EEPROM_writeByte((PASSWORD_INITIAL_LOCATION | i), data[i]);
		delay_10xN(10);
	}
}
/*
 * Description : Function to check if two given passwords are the same
 *     1. iterate and check each element if it equals the other
 *     2. returns the boolean value if true or false
 */
bool samePassword(uint8* pass1, uint8* pass2) {
	bool samepassword = TRUE;
	int i = 0;
	for(i = 0; i < PASSWORD_LENGTH; i++) { //checking if the passcodes are the same
		if(pass1[i] != pass2[i]) {
			samepassword = FALSE;
		}
	}
	return samepassword;
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
bool confirmPasscode(uint8* arr) {
	recievePassword(arr);
	uint8 EEPROM_DATA[PASSWORD_LENGTH];
	getPasscode(EEPROM_DATA);
	if(samePassword(arr, EEPROM_DATA)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
/*******************************************************************************
 *                              Main Function                                  *
 *******************************************************************************/

int main(void) {
	//variables declarations
	UART_config uart_config = {FALSE, FALSE, FALSE, TRUE, FALSE, Even, Async, onebit, FALLING, eight, 9600}; //uart configuration structure
	timer0_config timer0 = {CLK_1024, norm, 0, 0};
	I2C_config i2c = {0x02, FALSE, FALSE, NoPrescalar, 0x01};
	uint8 passcode[PASSWORD_LENGTH];
	uint8 passcodeConfirm[PASSWORD_LENGTH];
	uint8 errorCounter = 0;
	uint8 uC_Message;

	// ----------------------  preferals initializations   ----------------------//
	// initialize the uart
	UART_init(&uart_config);
	//I2C INITIALIZATION
	TWI_init(&i2c);
	EEPROM_init(&i2c);
	//MOTOR INITIALIZATION
	motor_init();
	//TIMER0 INITIALIZATION
	TIMER0_init(&timer0);

	//// STATE0         WILL ONLY BE RUN ON BOOT SO NO NEED TO BE AN ACTUAL LABEL (STATE)
	getPasscode(passcode); //GETTING SAVED PASSCODE FROM EEPROM
	sendPassword(passcode);//SEND PASSWORD TO THE MC1 TO KNOW IF ITS THE FIRST TIME THE DOOR LOCK IS USED TO GET AN INITIAL PASSWORD
	uC_Message = UART_recieveByte(); //GET MC1 MESSAGE
	if(uC_Message == PW_FOUND) {  //A PASSWORD HAD BEEN SET BEFORE IN THE EEPROM
		goto main_menu;
	}
	else if(uC_Message == PW_NOTFOUND) {   //NO PASSWORD HAD BEEN SET BEFORE IN THE EEPROM
		goto state1;
	}

		// ----------------- microcontroller 2 loop code  ---------------------//
	while(1){
state1:
		recievePassword(passcode); //waiting for the first entry
		recievePassword(passcodeConfirm); //waiting for the second entry

		if(samePassword(passcode, passcodeConfirm)) {
			UART_sendByte(CONFIRM);             // handshake to confirm they are the same and proceed to main menu
			savePasscode(passcode);				// save password to EEPROM
			errorCounter = 0;                   // reset error counter because we want it only on 3 consecutive mistakes
			goto main_menu;                  // main menu status
		}
		else {
			errorCounter++;
			if(errorCounter == 3) { // Buzzer will start there is an intrusion
				errorCounter = 0;   //RESET ERROR COUNTER
				UART_sendByte(REPEATED_MISTAKE); //TELL MC1 THERE IS A DEFEATED MISTAKE SO WE CAN FREEZE MC1 FOR 1 MINUTE
				sei();       //ENABLING GLOBAL INTERRUPTS
				setCallbackNormal(buzzerTickIncrement);   //SETTING TIMER NORMAL MODE ISR TO INCREMENT BUZZER TIME
				seconds = 0; ticks = 0;    //RESET TICKS AND SECONDS
				timerStart(&timer0);         //START THE TIMER (GIVE ITS PRESCALAR TO THE CLOCK
				buzzer_tone(1);              //START THE BUZZER WITH 1KZ CLOCK SOUND
				timerStop();				//Since buzzer tone is an infinite loop we will get here when its invoked by buzzerOff so we need to stop the timer
				cli();                     //Disable global interrupts
				UART_sendByte(DONE);       //Handshake to let MC1 get out of freeze state
			}
			else {                         // NOT A REPEATED MISTAKE WE JUST NEED TO REI
				UART_sendByte(ERROR);      //Handshake to let MC1 require another password
				goto state1;               //head to the earlier state to take a new password
			}
		}

main_menu:
		uC_Message = UART_recieveByte();      //GET THE USER INPUT (+ OR -) USING POLLING

		// IF USER CHOOSE TO OPEN THE DOOR
		if(uC_Message == OPEN_DOOR) {
			////////////////////CONFIRM PASSWORD////////////////////
OPENDOOR_CONFIRMATION:
			if(confirmPasscode(passcode)) {     //WE USE confirmPasscode FUNCTION THAT IS SYNCHRONIZED WITH MC1
				errorCounter = 0;				//RESET THE COUNTER SINCE IT'S ALREADY CORRECT
				UART_sendByte(CONFIRM);			//HANDSHAKE TO CONFIRM THAT PASSWORD IS CONFIRMED AND MATCHES THE EEPROM SAVED PW

				// now we can open the door
				/// motor unlocking for 15 seconds
				ticks = 0;                      //RESET TICKS
				seconds = 0;                    //RESET SECONDS
				UART_sendByte(DOOR_UNLOCKING);  //SYNCHRONIZE DOOR UNLOCKING WITH MC1
				sei();                          //ENABLE GLOBAL INTERRUPTS
				timerStart(&timer0);			//START TIMER0
				setCallbackNormal(tickIncrement);//SET CALLBACK TO INCREMENT TICKS
				motor_control(clockwise);		//OPEN THE DOOR
				while(seconds < DOOR_MOVING) {}          				//WAIT FOR 15 SECONDS

				/// motor stops for 3 seconds
				UART_sendByte(DOOR_STOP);		//SYNCHRONIZE DOOR STOP WITH MC1
				timerStart(&timer0);			//START TIMER0
				motor_control(stop);			//STOP THE DOOR
				while(seconds < (DOOR_MOVING+DOOR_STOPPED)) {}        		//WAIT ANOTHER 3 SECONDS

				/// motor locking for 15 seconds
				UART_sendByte(DOOR_LOCKING);  	//SYNCHRONIZE DOOR LOCKING WITH MC1
				timerStart(&timer0);			//START TIMER0
				motor_control(anticlockwise);	//CLOSE THE DOOR
				while(seconds < (DOOR_MOVING+DOOR_STOPPED+DOOR_MOVING)) {}	//WAIT ANOTHER 15 SECONDS
				timerStop();


				//disable interrupts and reset ticks and stopping the motor
				UART_sendByte(DONE);			//HANDSHAKE THAT WE ARE DONE
				motor_control(stop);			//STOP THE DOOR
				cli();							//DISABLE GLOBAL INTERRUPTS
				ticks = 0;
				seconds = 0;
				goto main_menu;				//GO TO MAIN MENU
			}
			else {								//REPEATED MISTAKE SAME AS PREVIOUS ALGORITHM
				errorCounter++;
				if(errorCounter == 3) {
					errorCounter = 0;
					UART_sendByte(REPEATED_MISTAKE);
					sei();
					setCallbackNormal(buzzerTickIncrement);
					seconds = 0; ticks = 0;
					timerStart(&timer0);
					buzzer_tone(1);
					timerStop();
					cli();
					UART_sendByte(DONE);
					goto main_menu;           //BACK TO MAIN MENU
				}
				UART_sendByte(ERROR);			//NORMAL MISTAKE
				goto OPENDOOR_CONFIRMATION;		//GO BACK AND GET ANOTHER USER INPUT HE STILL HAS CHANCES
			}
		}


		// IF USER CHOOSE TO CHANGE THE PASSCODE
		else if(uC_Message == NEW_PASSCODE) {          //MESSAGE IS THE DEFINED NEW PASSCODE CODE SAME ALGORITHM LIKE PREVIOUS
NEWPASSCODE_CONFIRMATION:
			if(confirmPasscode(passcode)) {
				errorCounter = 0;
				UART_sendByte(CONFIRM);
				goto state1;
			}
			else {
				errorCounter++;
				if(errorCounter == 3) {
					errorCounter = 0;
					UART_sendByte(REPEATED_MISTAKE);
					sei();
					setCallbackNormal(buzzerTickIncrement);
					seconds = 0; ticks = 0;
					timerStart(&timer0);
					buzzer_tone(1);
					timerStop();
					cli();
					UART_sendByte(DONE);
					goto main_menu;
				}
				UART_sendByte(ERROR);
				goto NEWPASSCODE_CONFIRMATION;
			}
		}

	}
}
