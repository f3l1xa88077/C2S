/*
 * jpeg_codec.c
 * hardware JPEG codec: RGB565 (streamed from SD) -> YCbCr 4:2:0
 *
 *  Created on: Jul 2, 2026
 *      Author: gabe
 */
#include "jpeg_codec.h"
#include "jpeg_utils.h"

#define MAX_WIDTH 2560

static uint32_t s_jpeg_out_len; // running total of encoded JPEG bytes

// one 16-row RGB565 band read from SD (2 bytes/pixel)
static uint8_t s_strip[MAX_WIDTH * 16 * 2];
// one MCU-row of YCbCr blocks (each 4:2:0 MCU = 6 blocks * 64 = 384 bytes)
static uint8_t s_mcu[(MAX_WIDTH / 16) * 384];

static struct {
    FX_FILE *src;
    uint16_t w;
    uint16_t h;
    uint32_t next_row;
    JPEG_RGBToYCbCr_Convert_Function convert;
} s_prog;

// codec calls this each time output is ready
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
    (void)hjpeg;    //unused
    (void)pDataOut; //unused
    s_jpeg_out_len += OutDataLength;
}

// Read one 16-row band from SD and convert it to a row of YCbCr MCU blocks in
// s_mcu. BlockIndex 0 makes the converter treat s_strip as its own MCU-row
// (rows 0..15); DataCount is sized for exactly w/16 MCUs. Returns output bytes
// (0 on read error).
static uint32_t fill_next_band(void)
{
    ULONG actual;
    uint32_t strip_bytes = 16u * (uint32_t)s_prog.w * 2u;

    if (fx_file_read(s_prog.src, s_strip, strip_bytes, &actual) != FX_SUCCESS ||
        actual != strip_bytes) {
        return 0;
    }

    uint32_t converted = 0;
    s_prog.convert(s_strip, s_mcu, 0, ((uint32_t)s_prog.w / 16u) * 512u, &converted);
    s_prog.next_row += 16u;
    return converted;
}

// codec calls this each time it needs more input (one band per call)
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData)
{
    (void)NbDecodedData;

    if (s_prog.next_row >= s_prog.h) {
        HAL_JPEG_ConfigInputBuffer(hjpeg, NULL, 0); // no bands left: input finished
        return;
    }

    uint32_t n = fill_next_band();
    HAL_JPEG_ConfigInputBuffer(hjpeg, s_mcu, n);
}

HAL_StatusTypeDef JPEG_Encode_RGB(JPEG_HandleTypeDef *hjpeg,
    FX_FILE *src,
    uint16_t w,
    uint16_t h,
    uint8_t quality,
    uint8_t *out,
    uint32_t out_cap,
    uint32_t *out_len
)
{
    // 4:2:0 MCUs are 16x16: dimensions must be whole MCUs and fit s_strip.
    if ((w % 16u) != 0u || (h % 16u) != 0u || w > MAX_WIDTH) {
        *out_len = 0;
        return HAL_ERROR;
    }

    // configure the codec for YCbCr 4:2:0 colour
    JPEG_ConfTypeDef conf;
    conf.ImageWidth = w;
    conf.ImageHeight = h;
    conf.ImageQuality = quality;
    conf.ColorSpace = JPEG_YCBCR_COLORSPACE;
    conf.ChromaSubsampling = JPEG_420_SUBSAMPLING;

    HAL_StatusTypeDef status = HAL_JPEG_ConfigEncoding(hjpeg, &conf);
    if (status != HAL_OK) {
        return status;
    }

    JPEG_InitColorTables();

    // fetch the RGB565 -> YCbCr MCU converter (also sets up jpeg_utils' params)
    uint32_t mcu_total;
    if (JPEG_GetEncodeColorConvertFunc(&conf, &s_prog.convert, &mcu_total) != HAL_OK) {
        *out_len = 0;
        return HAL_ERROR;
    }
    (void)mcu_total;

    s_prog.src = src;
    s_prog.w = w;
    s_prog.h = h;
    s_prog.next_row = 0;

    // rewind to the start of the raw RGB565 file
    if (fx_file_seek(src, 0) != FX_SUCCESS) {
        *out_len = 0;
        return HAL_ERROR;
    }

    s_jpeg_out_len = 0; // reset output length counter for callback

    // feed band 0 to start (GetDataCallback pulls the rest)
    uint32_t first = fill_next_band();
    if (first == 0u) {
        *out_len = 0;
        return HAL_ERROR;
    }

    status = HAL_JPEG_Encode(hjpeg, s_mcu, first, out, out_cap, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        *out_len = 0;   // encode failed - report zero valid bytes
        return status;
    }

    *out_len = s_jpeg_out_len;
    return status;
}
