/*
 * img_core.h
 *
 * Created on Aug 16, 2026
 *      Author: gabe
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
