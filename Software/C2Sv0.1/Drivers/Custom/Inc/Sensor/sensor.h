/*
 * sensor.h
 *
 *  Created on: Mar 7, 2026
 *      Author: felix
 *
 *  Functions are named according to purpose [DRIVER]_[OEPRATION]_[OBJECT]
 *  	DRIVER: What the function is for, in this case, AR0141CS
 *  	OPERATION: Read, Write, Reset, etc.
 *  	OBJECT: What it is operating on
 *
 */

#ifndef CUSTOM_INC_SENSOR_SENSOR_H_
#define CUSTOM_INC_SENSOR_SENSOR_H_

#include"main.h"
#include"config.h"

// Address depends on whether SADDR is pulled LOW or HIGH
#define AR_SLAVE_WRITE_LOW 0x20
#define AR_SLAVE_READ_LOW 0x21
#define AR_SLAVE_WRITE_HIGH 0x30
#define AR_SLAVE_READ_HIGH 0x31

#define AR_TIMEOUT_GENERAL 1

#define AR_TEMP_REGISTER_INPUT 0x30B4
#define AR_TEMP_REGISTER_OUTPUT 0x30B2
#define AR_TEMP_CALIBRATION1 0x30C6
#define AR_TEMP_CALIBRATION2 0x30C8
#define AR_TEMP_SLOPE 1
#define AR_TEMP_OFFSET 1

#define AR_TP_REGISTER 0x3070
#define AR_TP_REGISTER_GREEN1 0x3074
#define AR_TP_REGISTER_GREEN2 0x3078
#define AR_TP_REGISTER_BLUE 0x3076
#define AR_TP_REGISTER_RED 0x3072
#define AR_TP_OFF 0
#define AR_TP_SOLID 1
#define AR_TP_VERTICAL 2
#define AR_TP_GRADIENT 3
#define AR_TP_WALKING 256

#define AR_MIRROR_REGISTER 0x3040

#define AR_CLK_REGISTER 0x
#define AR_CLK_EXTCLK 24 // External Clock Frequency (MHz)
#define AR_CLK_PLLDIV 2 // Pre PLL Clock Divider
#define AR_CLK_PLLMULT 48 // PLL Multiplier
#define AR_CLK_SYSDIV 2 // System Clock Divider
#define AR_CLK_PIXDIV 4 // Pixel Clock Divider
#define AR_CLK_MINVCO 384
#define AR_CLK_MAXVCO 384
#define AR_CLK_MAXF 74.25

#define AR_PARALLEL_REGISTER 0x301A

HAL_StatusTypeDef AR_Init_ImageSensor(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AR_Init_Clock(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AR_Init_Temperature(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AR_Init_Parallel(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef AR_Write_Register(I2C_HandleTypeDef *hi2c, uint16_t reg_addr, uint16_t *pData);
HAL_StatusTypeDef AR_Write_Mirror(I2C_HandleTypeDef *hi2c, uint8_t state);

HAL_StatusTypeDef AR_Read_Register(I2C_HandleTypeDef *hi2c, uint16_t reg_addr, uint16_t *pData);
HAL_StatusTypeDef AR_Read_Temperature(I2C_HandleTypeDef *hi2c, uint16_t *pData);


#endif /* CUSTOM_INC_SENSOR_SENSOR_H_ */
