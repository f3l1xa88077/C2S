/*
 * img_core.h
 *
 * Created on Aug 16, 2026
 *      Author: gabe
 */
#ifndef DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_
#define DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_

#include <stdint.h>
#include "fx_api.h"

// Encode an RGB565 frame, streamed band-by-band from an open SD file, into a
// JPEG in 'out'. The source never has to be resident in RAM, so it scales to
// large frames (up to IMG_MAX_WIDTH wide) that don't fit in flash/RAM.
//   src         : SD file opened FX_OPEN_FOR_READ, raw RGB565 pixels
//   width,height: frame size in pixels (whole 16x16 MCUs -> multiples of 16)
//   quality     : JPEGE_Q_BEST/_HIGH/_MED/_LOW
//   out,out_cap : output buffer for the JPEG and its capacity
//   out_len     : [out] number of JPEG bytes produced
// Returns JPEGE_SUCCESS (0) on success, or a JPEGE_* error code.
int img_encode_jpeg(FX_FILE *src,
                    uint16_t width,
                    uint16_t height,
                    uint8_t  quality,
                    uint8_t *out,
                    uint32_t out_cap,
                    uint32_t *out_len);

#endif /* DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_ */
