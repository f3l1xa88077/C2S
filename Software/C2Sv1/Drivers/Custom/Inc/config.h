/*
 * config.h
 *
 *  Created on: Feb 23, 2026
 *      Author: felix
 */

#ifndef CUSTOM_INC_CONFIG_H_
#define CUSTOM_INC_CONFIG_H_

#include "main.h"

#define NO_MODE 				0
#define USB_MODE 				1
#define SLAVE_MODE 				2

// ------------ USER INPUT STARTS HERE ------------

#define VERIFY_CFG 				0

#define OPERATIONAL_MODE 		USB_MODE

// ------------ USER INPUT ENDS HERE ------------

// QSPI
#define QSPI_DUMMY_CYCLES_NUM 	14 					// Can be changed to 6 if speed is lower than 80MHz

// CSA
#define SHUNT_RESISTANCE 		1.6f
#define INA219_GAIN 			8 					// The dividing factor for the gain. Either 1, 2, 4, or 8.
#define INA219_ADDRESS 			0b1000000 			// A0 = GND, A1 = GND

// SPI2 PM for Custom SD Library
//extern SPI_HandleTypeDef hspi2;
//#define SD_CS_Pin 				SD_CS_Pin
#define SD_CS_PORT 				SD_CS_GPIO_Port
//#define SD_SPI_HANDLE 			hspi2

#endif /* CUSTOM_INC_CONFIG_H_ */
