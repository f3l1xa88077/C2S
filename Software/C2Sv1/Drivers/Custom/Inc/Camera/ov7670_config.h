/*
 * ov7670Reg.h
 *
 *  Created on: 2017/08/25
 *      Author: felix
 */

#ifndef OV7670_CONFIG_H_
#define OV7670_CONFIG_H_

#include <stdint.h>
#include "ov7670.h"

#define REG_EOF 0xFF

const uint8_t System_Config[][2] = {

		{OV7670_CLKRC_ADDRESS, 0x80}, 		// pre-scalar = 1/1
		{OV7670_MVFP_ADDRESS, 0x31}, 		// H & V Flip

//		{0x0C, 0x04},  // DCW enable
//		{0x3E, 0x19},  // manual scaling, pclk/=2
//		{0x70, 0x3A},  // scaling_xsc
//		{0x71, 0x35},  // scaling_ysc
//		{0x72, 0x11}, // down sample by 2
//		{0x73, 0xf1}, // DSP clock /= 2

		{REG_EOF, REG_EOF},
};

const uint8_t QVGA_Config[][2] = {

		// Windowing
		{OV7670_HSTART_ADDRESS, 0x14}, 		// HSTART
		{OV7670_HSTOP_ADDRESS, 0x02}, 		// HSTOP
		{OV7670_HREF_ADDRESS, 0x80}, 		// HREF

		{OV7670_VSTART_ADDRESS, 0x02},   	// VSTART
		{OV7670_VSTOP_ADDRESS, 0x7A},   	// VSTOP
		{OV7670_VREF_ADDRESS, 0x0A},   		// VREF

		{OV7670_COM4_ADDRESS, 0x00},
		{OV7670_COM17_ADDRESS, 0x00},

		{REG_EOF, REG_EOF},
};

const uint8_t VGA_Config[][2] = {

		// Windowing
//		{OV7670_HSTART_ADDRESS, 0x14}, 		// HSTART
//		{OV7670_HSTOP_ADDRESS, 0x02}, 		// HSTOP
//		{OV7670_HREF_ADDRESS, 0x80}, 		// HREF
//
//		{OV7670_VSTART_ADDRESS, 0x02},   	// VSTART
//		{OV7670_VSTOP_ADDRESS, 0x7A},   	// VSTOP
//		{OV7670_VREF_ADDRESS, 0x0A},   		// VREF
//
//		{OV7670_COM4_ADDRESS, 0x00},
//		{OV7670_COM17_ADDRESS, 0x00},

		{REG_EOF, REG_EOF},
};

const uint8_t RGB565_Config[][2] = {

		// Colour Configuration
		{OV7670_RGB444_ADDRESS, 0x00},   	// RGB444 Disable
		{OV7670_COM15_ADDRESS, 0xD0},   	// RGB565, 00 - FF
		{OV7670_TSLB_ADDRESS, 0x04},   		// UYVY, Dynamic Resolution Change (bits 1&2 reserved as 01)
		{OV7670_COM13_ADDRESS, 0x88},   	// Gamma Enable, UV auto adjust, UYVY

		// Auto White Balance
		{0x13, 0x84},
		{0x14, 0x0a},   // AGC Ceiling = 2x
		{0x5F, 0x2f},   // AWB B Gain Range (empirically decided). without this bright scene becomes yellow (purple). might be because of color matrix.
		{0x60, 0x98},   // AWB R Gain Range (empirically decided)
		{0x61, 0x70},   // AWB G Gain Range (empirically decided)
		{0x41, 0x38},   // edge enhancement, de-noise, AWG gain enabled

		// Colour Matrix
		{0x4f, 0x80},
		{0x50, 0x80},
		{0x51, 0x00},
		{0x52, 0x22},
		{0x53, 0x5e},
		{0x54, 0x80},
		{0x58, 0x9e},

		// Gamma Curve
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

		{REG_EOF, REG_EOF},
};

const uint8_t Grayscale_Config[][2] = { // YUV422 format, 2 input bytes, read one for grayscale. read every second byte in gpdma request?


		{REG_EOF, REG_EOF},
};

const uint8_t YCbCr_Config[][2] = {


		{REG_EOF, REG_EOF},
};

const uint8_t Test_Config[][2] = {

		// (XSC[7], YSC[7]) == (1,0) "8-bar colour bar)
		{OV7670_SCALING_XSC_ADDRESS, 0x3A | 0x80},
		{OV7670_SCALING_YSC_ADDRESS, 0x35 | 0x80},
		{OV7670_COM17_ADDRESS, 0x08}, // Colour Bar

		{REG_EOF, REG_EOF},
};

#endif /* OV7670_CONFIG_H_ */
