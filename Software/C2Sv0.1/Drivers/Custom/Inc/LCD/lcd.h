/*
 * lcd.h
 *
 *  Created on: Dec 24, 2025
 *      Author: felix
 */

#ifndef CUSTOM_INC_LCD_H_
#define CUSTOM_INC_LCD_H_

#include "liquidcrystal_i2c.h"
#include <string.h>
#include <stdio.h>

void sLCD_Init(I2C_HandleTypeDef *hi2c);
void sLCD_CFG_DISPLAY(int cfg_id, int val);
void sLCD_CSA_DISPLAY(int cfg_id, float *vals);
void sLCD_CLEAR();

#endif /* CUSTOM_INC_LCD_H_ */
