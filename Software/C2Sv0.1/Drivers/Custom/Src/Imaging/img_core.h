/*
 * img_core.h
 *
 * v0.1 TEST COPY of the v1 software JPEG module.
 * Adapted for on-board testing on the C2Sv0.1 hardware:
 *   - source is a GRAYSCALE frame already in RAM/flash (the const test_image),
 *     not RGB565 staged in QSPI as on v1
 *   - no QSPI / AT25 dependency
 * Same JPEGENC encode + strip/MCU loop being exercised; only the pixel source
 * and pixel type differ from the v1 module.
 *
 *  Created: 2026-08-16
 */
#ifndef DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_
#define DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_

#include <stdint.h>

int img_encode_jpeg(const uint8_t *src,
                    uint16_t width,
                    uint16_t height,
                    uint8_t  quality,
                    uint8_t *out,
                    uint32_t out_cap,
                    uint32_t *out_len);

#endif /* DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_ */
