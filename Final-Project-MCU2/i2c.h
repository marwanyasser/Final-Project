/******************************************************************************
 *
 * Module: I2C
 *
 * File Name: i2c.h
 *
 * Description: Header file for the i2c driver
 *
 * Author: Marwan Yasser
 *
 *******************************************************************************/


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_
#include "common_macros.h"
#include "std_types.h"
#include "micro_config.h"
#define TW_START         0x08 // start has been sent
#define TW_REP_START     0x10 // repeated start
#define TW_MT_SLA_W_ACK  0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define TW_MT_SLA_R_ACK  0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define TW_MT_DATA_ACK   0x28 // Master transmit data and ACK has been received from Slave.
#define TW_MR_DATA_ACK   0x50 // Master received data and send ACK to slave
#define TW_MR_DATA_NACK  0x58 // Master received data but doesn't send ACK to slave


/*******************************************************************************
 *                          Type Defining                                      *
 *******************************************************************************/
typedef enum
{
	NoPrescalar, CLK_4, CLK_16, CLK_64_
}prescalar_i2c;


typedef struct
{
	//TWCR
	uint8 bitrate;
	bool ack;
	bool interruptEnable;

	//TWSR
	prescalar_i2c pre;

	//TWAR
	uint8 slaveAddressRegister;
}I2C_config;



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void TWI_init(const I2C_config* config);
void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8 data);
uint8 TWI_readWithACK(void);
uint8 TWI_readWithNACK(void);
uint8 TWI_getStatus(void);

#endif
