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

#define SELECTED_LCD 1 // 0 For Offline, 1 for Active

#define SHUNT_RESISTANCE 0.068f

// ------------ USER INPUT ENDS HERE ------------

#define QSPI_DUMMY_CYCLES_NUM 14 // Can be changed to 6 if speed is lower than 80MHz

#endif /* CUSTOM_INC_CONFIG_H_ */
