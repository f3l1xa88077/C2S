/*
 * ov7670Reg.h
 *
 *  Created on: 2017/08/25
 *      Author: take-iwiw
 */

#ifndef OV7670_CONFIG_H_
#define OV7670_CONFIG_H_

#include <stdint.h>
#include "ov7670.h"

#define REG_BATT 0xFF

const uint8_t OV7670_Config[][2] = {
  /* Color mode related */
  {OV7670_COM7_ADDRESS, 0x14},   						// QVGA, RGB
  {OV7670_RGB444_ADDRESS, 0X00},   						// RGB444 Disable
  {OV7670_COM15_ADDRESS, 0xD0},   						// RGB565, 00 - FF
  {OV7670_TSLB_ADDRESS, 0x04},   						// UYVY (bits 1&2 reserved 01)
  {OV7670_COM13_ADDRESS, 0x0C},   						// gamma enable, UV auto adjust, UYVY
  {OV7670_RSVD_B0_ADDRESS, OV7670_RSVD_B0_RESET_VALUE}, // DO NOT CLEAR (library mentions it is important, no reason)

  /* clock related */
//  {0x0C, 0x04},  // DCW enable
//  {0x3E, 0x19},  // manual scaling, pclk/=2
//  {0x70, 0x3A},  // scaling_xsc
//  {0x71, 0x35},  // scaling_ysc
//  {0x72, 0x11}, // down sample by 2
//  {0x73, 0xf1}, // DSP clock /= 2

  /* windowing (Standard QVGA / Scaled values) */
    {0x17, 0x16},   // HSTART
    {0x18, 0x04},   // HSTOP
    {0x32, 0xA4},   // HREF
    {0x19, 0x01},   // VSTART
    {0x1a, 0x79},   // VSTOP
    {0x03, 0x0A},   // VREF
    {0x0D, 0x00},   // COM4

  /* color matrix coefficient */
//#if 0
//  {0x4f, 0xb3},
//  {0x50, 0xb3},
//  {0x51, 0x00},
//  {0x52, 0x3d},
//  {0x53, 0xa7},
//  {0x54, 0xe4},
//  {0x58, 0x9e},
//#else
//  {0x4f, 0x80},
//  {0x50, 0x80},
//  {0x51, 0x00},
//  {0x52, 0x22},
//  {0x53, 0x5e},
//  {0x54, 0x80},
//  {0x58, 0x9e},
//#endif

  /* 3a */
//  {0x13, 0x84},
//  {0x14, 0x0a},   // AGC Ceiling = 2x
//  {0x5F, 0x2f},   // AWB B Gain Range (empirically decided)
//                  // without this bright scene becomes yellow (purple). might be because of color matrix
//  {0x60, 0x98},   // AWB R Gain Range (empirically decided)
//  {0x61, 0x70},   // AWB G Gain Range (empirically decided)
  {0x41, 0x38},   // edge enhancement, de-noise, AWG gain enabled


  /* gamma curve */
#if 1
  {0x7b, 16},
  {0x7c, 30},
  {0x7d, 53},
  {0x7e, 90},
  {0x7f, 105},
  {0x80, 118},
  {0x81, 130},
  {0x82, 140},
  {0x83, 150},
  {0x84, 160},
  {0x85, 180},
  {0x86, 195},
  {0x87, 215},
  {0x88, 230},
  {0x89, 244},
  {0x7a, 16},
#else
  /* gamma = 1 */
  {0x7b, 4},
  {0x7c, 8},
  {0x7d, 16},
  {0x7e, 32},
  {0x7f, 40},
  {0x80, 48},
  {0x81, 56},
  {0x82, 64},
  {0x83, 72},
  {0x84, 80},
  {0x85, 96},
  {0x86, 112},
  {0x87, 144},
  {0x88, 176},
  {0x89, 208},
  {0x7a, 64},
#endif

  /* fps */
//  {0x6B, 0x4a}, //PLL  x4
  {0x11, 0x01}, // pre-scalar = 1/2

  /* others */
  {0x1E, 0x01}, // Keeps bit 5 cleared (VSYNC Positive / Active High), retains mirror/flip bit 0
  {0x42, 0xc0}, // color bar

  {REG_BATT, REG_BATT},
};


#endif /* OV7670_CONFIG_H_ */
