/*
 * img_core.c  
 *
 * Created on Aug 16, 2026
 *      Author: gabe
 */
#include "img_core.h"
#include "../../JPEGENC/JPEGENC.h"
#include <string.h>

#define IMG_MAX_WIDTH 2560 // largest frame width to support

// One 16-row band of RGB565, read from SD per iteration.
static uint8_t s_strip[IMG_MAX_WIDTH * 16 * 2];

int img_encode_jpeg(FX_FILE *src,
                    uint16_t width,
                    uint16_t height,
                    uint8_t  quality,
                    uint8_t *out,
                    uint32_t out_cap,
                    uint32_t *out_len)
{
    JPEGE_IMAGE jpeg;
    JPEGENCODE  enc;
    int rc, mx, final_len;
    uint32_t pitch, strip_rows, strip_bytes, row;
    ULONG actual;

    if (out_len == NULL)
        return JPEGE_INVALID_PARAMETER;
    *out_len = 0;

    if (src == NULL || out == NULL || out_cap < 1024u ||
        width == 0u || height == 0u || width > IMG_MAX_WIDTH)
        return JPEGE_INVALID_PARAMETER;

    memset(&jpeg, 0, sizeof(jpeg));
    jpeg.pOutput     = out;
    jpeg.iBufferSize = (int)out_cap;
    jpeg.pHighWater  = &out[out_cap - 512u];

    // RGB565, 4:2:0 (16x16 MCU)
    rc = JPEGEncodeBegin(&jpeg, &enc, (int)width, (int)height,
                         JPEGE_PIXEL_RGB565, JPEGE_SUBSAMPLE_420, quality);
    if (rc != JPEGE_SUCCESS)
        return rc;

    if ((width % enc.cx) != 0 || (height % enc.cy) != 0)
        return JPEGE_INVALID_PARAMETER;

    pitch       = (uint32_t)width * 2u; // bytes per source row 
    strip_rows  = (uint32_t)enc.cy;     // one MCU tall (16 rows)
    strip_bytes = strip_rows * pitch;

    // rewind to the start of the raw RGB565 file
    if (fx_file_seek(src, 0) != FX_SUCCESS)
        return JPEGE_ENCODE_ERROR;

    // stream one band at a time from SD, top to bottom
    for (row = 0u; row < (uint32_t)height; row += strip_rows) {
        if (fx_file_read(src, s_strip, strip_bytes, &actual) != FX_SUCCESS ||
            actual != strip_bytes)
            return JPEGE_ENCODE_ERROR;

        for (mx = 0; mx < jpeg.iMCUWidth; mx++) {
            uint8_t *p = s_strip + (uint32_t)mx * (uint32_t)enc.cx * 2u;

            rc = JPEGAddMCU(&jpeg, &enc, p, (int)pitch);
            if (rc != JPEGE_SUCCESS)
                return rc;
        }
    }

    final_len = JPEGEncodeEnd(&jpeg);
    if (final_len <= 0)
        return JPEGE_ENCODE_ERROR;

    *out_len = (uint32_t)final_len;
    return JPEGE_SUCCESS;
}
