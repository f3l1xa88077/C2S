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
#include "fx_stm32_sd_driver.h"
#include <stdio.h>
#include <string.h>



extern FX_MEDIA sd_disk;
extern FX_FILE         my_file;
extern USART_HandleTypeDef husart2;
uint32_t Init_Output_File(FX_FILE* output_file, const char* filename);
uint32_t uSD_Init(void);
void uSD_Test(void);

uint32_t SD_Stream_Data(FX_FILE* file, char* filename, void* data, uint32_t size_in_bytes, uint8_t is_last_chunk);
uint32_t SD_Read_Data(char* filename, void* data, uint32_t size_in_bytes);
uint32_t SD_Open_YUV_Files(char* filenames[], FX_FILE* y_file, FX_FILE* u_file, FX_FILE* v_file);
void Test_Image_Stream(void);

#endif /* CUSTOM_INC_PERIPHERALS_USD_H_ */
