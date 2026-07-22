/*
 * jpeg_codec.c
 * a wrapper around the STM32 hardware JPEG codec
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */
#include "jpeg_codec.h"

#define MAX_WIDTH 1280 // maximum width of the image to encode (in pixels)
static uint32_t s_jpeg_out_len; // running total of encoded JPEG bytes
static uint8_t s_band[MAX_WIDTH * 8]; // buffer for one MCU band (8 rows of pixels)

struct JPEG_Encode_Progress {
    const uint8_t *img;
    uint16_t w;
    uint16_t h;
    uint32_t next_row;
};

static struct JPEG_Encode_Progress s_progress;

// codec calls this each time output is ready
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
    (void)hjpeg; //unused
    (void)pDataOut; //unused
    s_jpeg_out_len += OutDataLength;
}

// tile one band into s_band, bump the cursor, return its size (8*w bytes)
static uint32_t fill_next_band(void)
{
    uint32_t idx = 0;
    for (uint16_t bx = 0; bx < s_progress.w / 8; bx++) {   // blocks across
        for (uint8_t r = 0; r < 8; r++) {                  // rows in band
            for (uint8_t c = 0; c < 8; c++) {              // cols in block
                uint16_t x = bx * 8 + c;
                uint16_t y = s_progress.next_row + r;      // band starts at next_row
                s_band[idx++] = s_progress.img[y * s_progress.w + x];
            }
        }
    }
    s_progress.next_row += 8;   // advance to the next band
    return idx;                 // 8 * w bytes
}

// codec calls this each time it needs more input (one band per call)
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData)
{
    (void)NbDecodedData;

    if (s_progress.next_row >= s_progress.h) {
        HAL_JPEG_ConfigInputBuffer(hjpeg, NULL, 0);  // no bands left: input finished
        return;
    }

    uint32_t n = fill_next_band();                   // build the next band
    HAL_JPEG_ConfigInputBuffer(hjpeg, s_band, n);    // hand it to the codec
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
    // reject sizes the tiler/band buffer can't handle:
    // dimensions must be whole 8x8 blocks, and width must fit s_band
    if ((w % 8u) != 0u || (h % 8u) != 0u || w > MAX_WIDTH) {
        *out_len = 0;
        return HAL_ERROR;
    }

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

    s_progress.img = img;
    s_progress.w = w;
    s_progress.h = h;
    s_progress.next_row = 0;

    // feed band 0 to start (callback pulls the rest)
    uint32_t first = fill_next_band();
    status = HAL_JPEG_Encode(hjpeg, s_band, first, out, out_cap, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        *out_len = 0;   // encode failed - report zero valid bytes
        return status;
    }

    *out_len = s_jpeg_out_len; 
    return status;
}
