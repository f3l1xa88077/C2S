/*
 * img_core.h
 *
 *  Created Aug 11, 2026
 *      Author: gabe
 */
#ifndef DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_
#define DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_

#include <stdint.h>
#include "../../JPEGENC/JPEGENC.h"
#include "AT25xF2561C.h"

int img_encode_jpeg(QSPI_HandleTypeDef *qspi,
                      uint32_t src_qspi_addr,
                      uint16_t width,
                      uint16_t height,
                      uint8_t  quality,
                      uint8_t *out,
                      uint32_t out_cap,
                      uint32_t *out_len);

#endif /* DRIVERS_CUSTOM_SRC_IMAGING_IMG_CORE_H_ */
