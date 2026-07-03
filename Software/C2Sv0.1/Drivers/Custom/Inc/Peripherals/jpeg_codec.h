/*
 * jpeg_codec.h
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */

 #ifndef CUSTOM_INC_PERIPHERALS_JPEG_CODEC_H_
 #define CUSTOM_INC_PERIPHERALS_JPEG_CODEC_H_

 #include "main.h"

 HAL_StatusTypeDef JPEG_Encode_Gray(JPEG_HandleTypeDef *hjpeg, 
    const uint8_t *img, 
    uint16_t w, 
    uint16_t h, 
    uint8_t quality, 
    uint8_t *out, 
    uint32_t out_cap, 
    uint32_t *out_len
);

 #endif