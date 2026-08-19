/*
 * img_core.c
 *
 * Created on Aug 16, 2026
 *      Author: gabe
 */
#include "img_core.h"
#include "../../JPEGENC/JPEGENC.h"
#include <string.h>

#define IMG_MAX_WIDTH 1024 // Largest frame width to support.

// Temporary buffer for one band: 16-row MCU (4:2:0) x max width x 2 bytes/pixel (RGB565).
static uint8_t s_strip[IMG_MAX_WIDTH * 16 * 2];

// Encodes an RGB565 frame into a JPEG in 'out'.
// Streams the frame one band at a time so peak RAM is a single band.
// src: pointer to the RGB565 source frame in memory (2 bytes/pixel)
// width, height: dimensions of the frame
// quality: JPEGE_Q_BEST/_HIGH/_MED/_LOW
// out, out_cap: output buffer and its capacity
// out_len: [out] number of JPEG bytes produced
// Returns JPEGE_SUCCESS (0) on success, or JPEGE_* error code on failure
int img_encode_jpeg(const uint8_t *src,
                    uint16_t width,
                    uint16_t height,
                    uint8_t  quality,
                    uint8_t *out,
                    uint32_t out_cap,
                    uint32_t *out_len)
{
    JPEGE_IMAGE jpeg; // JPEGENC encoder state
    JPEGENCODE  enc;  // tracks the current MCU encoding position and size
    int rc, mx, final_len;
    uint32_t pitch, strip_rows, strip_bytes, row;

    // Validate params
    if (out_len == NULL)
        return JPEGE_INVALID_PARAMETER;
    *out_len = 0;

    if (src == NULL || out == NULL || out_cap < 1024u ||
        width == 0u || height == 0u || width > IMG_MAX_WIDTH)
        return JPEGE_INVALID_PARAMETER;

    // Point encoder at the caller's buffer.
    // pHighWater leaves a 512-byte margin for overflow protection
    memset(&jpeg, 0, sizeof(jpeg));
    jpeg.pOutput     = out;
    jpeg.iBufferSize = (int)out_cap;
    jpeg.pHighWater  = &out[out_cap - 512u];

    // Write the JPEG header and initialise the encoder (RGB565, 4:2:0).
    rc = JPEGEncodeBegin(&jpeg, &enc, (int)width, (int)height,
                         JPEGE_PIXEL_RGB565, JPEGE_SUBSAMPLE_420, quality);
    if (rc != JPEGE_SUCCESS)
        return rc;

    if ((width % enc.cx) != 0 || (height % enc.cy) != 0)
        return JPEGE_INVALID_PARAMETER;


    pitch       = (uint32_t)width * 2u; // Bytes per row (RGB565 = 2 bytes/pixel)
    strip_rows  = (uint32_t)enc.cy;     // Rows per band
    strip_bytes = strip_rows * pitch;   // Bytes to read per band

    // Walks the frame top to bottom, one band at a time.
    for (row = 0u; row < (uint32_t)height; row += strip_rows) {
        memcpy(s_strip, src + row * pitch, strip_bytes);

        for (mx = 0; mx < jpeg.iMCUWidth; mx++) {
            // Feed each MCU in the band to the encoder.
            uint8_t *p = s_strip + (uint32_t)mx * (uint32_t)enc.cx * 2u;

            rc = JPEGAddMCU(&jpeg, &enc, p, (int)pitch);
            if (rc != JPEGE_SUCCESS)
                return rc;
        }
    }

    // Returns final length of the JPEG in bytes.
    final_len = JPEGEncodeEnd(&jpeg);
    if (final_len <= 0)
        return JPEGE_ENCODE_ERROR;

    *out_len = (uint32_t)final_len;
    return JPEGE_SUCCESS;
}
