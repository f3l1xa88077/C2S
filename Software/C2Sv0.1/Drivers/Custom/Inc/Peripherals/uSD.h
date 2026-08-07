/*
 * uSD.h
 *
 *  Created on: Mar 8, 2026
 *      Author: felix
 */

#ifndef CUSTOM_INC_PERIPHERALS_USD_H_
#define CUSTOM_INC_PERIPHERALS_USD_H_

#include "main.h"
#include "fx_api.h"
#include "fx_stm32_spi_sd_driver.h"
#include <stdio.h>
#include <string.h>

uint32_t uSD_Init(void);
void uSD_Test(void);

uint32_t SD_Stream_Data(char* filename, void* data, uint32_t size_in_bytes, uint8_t is_last_chunk);
void Test_Image_Stream(void);

#endif /* CUSTOM_INC_PERIPHERALS_USD_H_ */
