/*
 * jpeg_codec.c
 * a wrapper around the STM32 hardware JPEG codec
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */
#include "jpeg_codec.h"

HAL_StatusTypeDef JPEG_Encode_Gray(JPEG_HandleTypeDef *hjpeg, 
    const uint8_t *img, 
    uint16_t w, 
    uint16_t h, 
    uint8_t quality, 
    uint8_t *out, 
    uint32_t out_cap, 
    uint32_t *out_len
)
{
    (void)hjpeg; (void)img; (void)w; (void)h;
    (void)quality; (void)out; (void)out_cap;
    if (out_len) *out_len = 0;
    return HAL_OK;
}