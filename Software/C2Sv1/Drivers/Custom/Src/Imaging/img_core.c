/*
 * img_core.c  
 *
 * Created on Aug 11, 2026
 *      Author: gabe
 */
#include "img_core.h"
#include <string.h>

// Largest frame width to support. Will be used to allocate a temp buffer for reading strips of the image
#define IMG_MAX_WIDTH 640

// One MCU band: IMG_MAX_WIDTH pixels wide * 16 rows * 2 bytes per pixel. 
static uint8_t s_strip[IMG_MAX_WIDTH * 16 * 2];

// Encodes an RGB565 frame into a JPEG in 'out'.
// Streams the frame one band at a time so peak RAM is a single band, no entire image
// qspi: pointer to the QSPI memory handle
// src_qspi_addr: address of the frame in QSPI memory
// width, height: dimensions of the frame
// quality: JPEGE_Q_BEST/_HIGH/_MEDIUM/_LOW/_WORST (enum)
// out, out_cap: output buffer and its capacity
// out_len: [out] number of JPEG bytes produced
// Returns JPEGE_SUCCESS (0) on success, or JPEGE_* error code on failure
int img_encode_jpeg(QSPI_HandleTypeDef *qspi,
                      uint32_t src_qspi_addr,
                      uint16_t width,
                      uint16_t height,
                      uint8_t  quality,
                      uint8_t *out,
                      uint32_t out_cap,
                      uint32_t *out_len)
{
    JPEGE_IMAGE jpeg; // JPEGENC encoder state 
    JPEGENCODE  enc; // tracks the current MCU encoding position and size
    int rc;
    uint32_t pitch, strip_rows, strip_bytes, row;
    int mx, final_len;

    // Validate params
    if (out_len == NULL)
        return JPEGE_INVALID_PARAMETER;
    *out_len = 0;

    if (qspi == NULL || out == NULL || out_cap < 1024u ||
        width == 0u || height == 0u || width > IMG_MAX_WIDTH)
        return JPEGE_INVALID_PARAMETER;

    // Point encoder at the caller's buffer. 
    // pHighWater leaves a 512-byte margin for overflow protection
    memset(&jpeg, 0, sizeof(jpeg));
    jpeg.pOutput = out;
    jpeg.iBufferSize = (int)out_cap;
    jpeg.pHighWater  = &out[out_cap - 512u];

// Write the JPEG header and initialise the encoder.
    rc = JPEGEncodeBegin(&jpeg, &enc, (int)width, (int)height,
                         JPEGE_PIXEL_RGB565, JPEGE_SUBSAMPLE_420, quality);
    if (rc != JPEGE_SUCCESS)
        return rc;

    if ((width % enc.cx) != 0 || (height % enc.cy) != 0)
        return JPEGE_INVALID_PARAMETER;


    pitch = (uint32_t)width * 2u; // bytes per row
    strip_rows  = (uint32_t)enc.cy; // rows per band
    strip_bytes = strip_rows * pitch; // bytes to read per band

    // Walks the frame top to bottom, one band at a time.
    for (row = 0u; row < (uint32_t)height; row += strip_rows) {
        QSPI_DataBlock_HandleTypeDef blk;

        // Reads band's pixels out of QSPI into s_strip.
        QSPI_Init_DataBlock(&blk, strip_bytes, NULL, s_strip);
        blk.address_block_start = src_qspi_addr + row * pitch;

        if (QSPI_Read_Data(qspi, &blk) != HAL_OK)
            return JPEGE_ENCODE_ERROR;
        
        // Feed each MCU in the band to the encoder.
        for (mx = 0; mx < jpeg.iMCUWidth; mx++) {
            uint8_t *p = s_strip + (uint32_t)mx * (uint32_t)enc.cx * 2u;

            rc = JPEGAddMCU(&jpeg, &enc, p, (int)pitch);
            if (rc != JPEGE_SUCCESS)
                return rc;
        }
    }

    // Append the marker and finalise. 
    // Returns final length of the JPEG in bytes.
    final_len = JPEGEncodeEnd(&jpeg);
    if (final_len <= 0)
        return JPEGE_ENCODE_ERROR;

    *out_len = (uint32_t)final_len;
    return JPEGE_SUCCESS;
}
