/*
 * jpeg_codec.c
 * a wrapper around the STM32 hardware JPEG codec
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */
#include "jpeg_codec.h"

// running total of encoded JPEG bytes (shared with the callback below)
static uint32_t s_jpeg_out_len;

// codec calls this each time output is ready
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
    (void)hjpeg; //unused
    (void)pDataOut; //unused
    s_jpeg_out_len += OutDataLength; 
}

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
    // configure JPEG codec for grayscale encoding
    JPEG_ConfTypeDef conf;
    conf.ImageWidth = w;
    conf.ImageHeight = h;
    conf.ImageQuality = quality;
    conf.ColorSpace = JPEG_GRAYSCALE_COLORSPACE;
    conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;

    // Apply the config to the engine + error handling
    HAL_StatusTypeDef status = HAL_JPEG_ConfigEncoding(hjpeg, &conf);
    if (status != HAL_OK) {
        return status;
    }

    s_jpeg_out_len = 0; // reset output length counter for callback

    // Holds grayscale image in MCU order (8x8 blocks)
    static uint8_t mcu_buf[128*128];
    uint32_t idx = 0;
    for (uint16_t by = 0; by < h/8; by++) {
        for (uint16_t bx = 0; bx < w/8; bx++) {
            for (uint8_t r = 0; r < 8; r++) {
                for (uint8_t c = 0; c < 8; c++) {
                    uint16_t x = bx * 8 + c;
                    uint16_t y = by * 8 + r;
                    mcu_buf[idx++] = img[y * w + x];
                }
            }
        }
    }

    // Pass the MCU-ordered grayscale image to the JPEG encoder
    status = HAL_JPEG_Encode(hjpeg, mcu_buf, w*h, out, out_cap, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        *out_len = 0;   // encode failed - report zero valid bytes
        return status;
    }

    *out_len = s_jpeg_out_len; 
    return status;
}
