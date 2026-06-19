/*
 * config.h
 *
 *  Created on: Feb 23, 2026
 *      Author: felix
 */

#ifndef CUSTOM_INC_CONFIG_H_
#define CUSTOM_INC_CONFIG_H_

#include "main.h"

// ------------ USER INPUT STARTS HERE ------------

#define VERIFY_CFG 0

// ------------ USER INPUT ENDS HERE ------------

// QSPI
#define QSPI_DUMMY_CYCLES_NUM 14 // Can be changed to 6 if speed is lower than 80MHz

// CSA
#define SHUNT_RESISTANCE 1.6f
#define INA219_GAIN 8 // The dividing factor for the gain. Either 1, 2, 4, or 8.
#define INA219_ADDRESS 0b1000000 // A0 = GND, A1 = GND

#endif /* CUSTOM_INC_CONFIG_H_ */
