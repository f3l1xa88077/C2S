/*
 * jpeg_codec.h
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */

 #ifndef CUSTOM_INC_PERIPHERALS_JPEG_CODEC_H_
 #define CUSTOM_INC_PERIPHERALS_JPEG_CODEC_H_

 #include "main.h"
 #include "fx_api.h"

 // Encode an RGB565 frame, streamed band-by-band from an open SD file, into a
 // JPEG using the STM32 hardware codec (YCbCr 4:2:0). The source never has to be
 // resident, so it scales to frames too large for flash/RAM.
 // Requires JPEG_RGB_FORMAT == JPEG_RGB565 in jpeg_utils_conf.h.
 //   src : SD file opened FX_OPEN_FOR_READ, raw RGB565 
 HAL_StatusTypeDef JPEG_Encode_RGB(JPEG_HandleTypeDef *hjpeg,
    FX_FILE *src,
    uint16_t w,
    uint16_t h,
    uint8_t quality,
    uint8_t *out,
    uint32_t out_cap,
    uint32_t *out_len
);

 #endif
