/*
 * sensor.c
 *
 *  Created on: Mar 7, 2026
 *      Author: felix
 *
 *  Functions are named according to purpose [DRIVER]_[OEPRATION]_[OBJECT]
 *  	DRIVER: What the function is for, in this case, AR0141CS
 *  	OPERATION: Read, Write, Reset, etc.
 *  	OBJECT: What it is operating on
 *
 */

#include "sensor.h"

/**
  * @brief  Initialises all subsystems of the AR0141CS image sensor
  *
  * @param  *hi2c I2C HAL handle
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Init_ImageSensor(I2C_HandleTypeDef *hi2c)
{
	if (AR_Init_Clock(hi2c) != HAL_OK) return HAL_ERROR;
	if (AR_Init_Temperature(hi2c) != HAL_OK) return HAL_ERROR;
	if (AR_Init_Parallel(hi2c) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}

/**
  * @brief  Writes data to a specified address
  *
  * @param  *hi2c I2C HAL handle
  * @param  reg_addr Address of the data register
  * @param	*pData Pointer to data
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Write_Register(I2C_HandleTypeDef *hi2c, uint16_t reg_addr, uint16_t *pData)
{
	uint8_t packet_size = 4;
	uint8_t packet[packet_size];

	// (MSB first)
	packet[0] = (reg_addr >> 8) & 0xFF;
	packet[1] = reg_addr & 0xFF;
	packet[2] = (uint8_t)(*pData >> 8);
	packet[3] = (uint8_t)(*pData & 0xFF);

	// Address must be 7 bits
	return HAL_I2C_Master_Transmit(hi2c, AR_SLAVE_WRITE_LOW, packet, packet_size, AR_TIMEOUT_GENERAL);
}

/**
  * @brief  Writes data to a specified address
  *
  * @param  *hi2c I2C HAL handle
  * @param  reg_addr Address of the data register
  * @param	*pData Pointer to data
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Read_Register(I2C_HandleTypeDef *hi2c, uint16_t reg_addr, uint16_t *pData)
{
    uint8_t addr_buf[2];
    uint8_t data_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xFF;
    addr_buf[1] = reg_addr & 0xFF;

    // 1. Write the register address pointer
    if (HAL_I2C_Master_Transmit(hi2c, AR_SLAVE_WRITE_LOW << 1, addr_buf, 2, AR_TIMEOUT_GENERAL) != HAL_OK) return HAL_ERROR;

    // 2. Read the 16-bit response (LSB automatically set to 1)
    if (HAL_I2C_Master_Receive(hi2c, AR_SLAVE_READ_LOW, data_buf, 2, AR_TIMEOUT_GENERAL) == HAL_OK) {
        *pData = (uint16_t)(data_buf[0] << 8) | data_buf[1];
        return HAL_OK;
    }

    return HAL_ERROR;
}

/**
  * @brief  Initialises the internal temperature sensor registers
  *
  * @param  *hi2c I2C HAL handle
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Init_Temperature(I2C_HandleTypeDef *hi2c)
{
	uint16_t reg_data;
	// Read existing config so we don't destroy other settings
	if (AR_Read_Register(hi2c, AR_TEMP_REGISTER_INPUT, &reg_data) == HAL_OK) {
	    if ((reg_data & 0x11) != 0x11) { // If the register has not already been set

	    	reg_data |= (1U << 0) | (1U << 4);

		    return AR_Write_Register(hi2c, AR_TEMP_REGISTER_INPUT, &reg_data);
	    }
	}
	return HAL_ERROR;
}

/**
  * @brief  Reads the internal die temperature sensor
  *
  * @param  *hi2c I2C HAL handle
  * @param	*pData Pointer to data
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Read_Temperature(I2C_HandleTypeDef *hi2c, uint16_t *pData)
{
	uint16_t temp_adc;
	// Read data from R0x30B2[0:0]
	if (AR_Read_Register(hi2c, AR_TEMP_REGISTER_OUTPUT, &temp_adc) != HAL_OK) return HAL_ERROR;
	// Mask bits [9:0]
	temp_adc &= 0x3FF;
	// Calculate temperature
	*(pData) = temp_adc * AR_TEMP_SLOPE + AR_TEMP_OFFSET;  // Need to adjust to account for float rounding

	return HAL_OK;
}

/**
  * @brief  Writes to the mirror image register
  * 		Enables/disables different mirroring options
  * 		Horizontal set on 0x3040[14], vertical set on 0x3040[15]
  *
  *	Could be optimised by checking the register before writing.
  *
  * @param  *hi2c I2C HAL handle
  * @param	state Mirror state: 0x00 (no mirror), 0b01 (horizontal mirror), 0b10 (vertical mirror), 0b11 (horiz. & vert. mirror)
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Write_Mirror(I2C_HandleTypeDef *hi2c, uint8_t state)
{
	uint16_t reg_data;

	uint16_t h_bit = (uint16_t)(state & 0b01);       	// Get bit 0
	uint16_t v_bit = (uint16_t)((state & 0b10) >> 1); 	// Get bit 1 and move to position 0

	if (AR_Read_Register(hi2c, AR_MIRROR_REGISTER, &reg_data) == HAL_OK) {
		// Clear ONLY bits 14 and 15
		reg_data &= ~((1U << 14) | (1U << 15));

		// Horizontal and Vertical bits
		reg_data |= (h_bit << 14);
		reg_data |= (v_bit << 15);
		return AR_Write_Register(hi2c, AR_MIRROR_REGISTER, &reg_data);
	}
	return HAL_ERROR;
}

/**
  * @brief  Initialises the internal clock dividers and multipliers
  *
  * @param  *hi2c I2C HAL handle
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Init_Clock(I2C_HandleTypeDef *hi2c)
{
	float fVCO = AR_CLK_EXTCLK / AR_CLK_PLLDIV * AR_CLK_PLLMULT;
	float fOUT = fVCO / AR_CLK_SYSDIV / AR_CLK_PIXDIV;

	// 1. External Clock: 6-50 MHz
	if (AR_CLK_EXTCLK < 6 || AR_CLK_EXTCLK > 50) return HAL_ERROR;
	// 2. Pre-PLL Divider: 1-64
	if (AR_CLK_PLLDIV < 1 || AR_CLK_PLLDIV > 64) return HAL_ERROR;
	// 3. PLL Multiplier: 32-384
	if (AR_CLK_PLLMULT < 32 || AR_CLK_PLLMULT > 384) return HAL_ERROR;
	// 4. System Divider: Must be 1 OR (Even between 2-16)
	if (AR_CLK_SYSDIV != 1 && (AR_CLK_SYSDIV % 2 != 0 || AR_CLK_SYSDIV > 16)) return HAL_ERROR;
	// 5. Pixel Divider: 4-16
	if (AR_CLK_PIXDIV < 4 || AR_CLK_PIXDIV > 16) return HAL_ERROR;
	// 6. Frequency Limits
	if (fVCO < AR_CLK_MAXF || fVCO > 768.0f) return HAL_ERROR;
	if (fOUT > AR_CLK_MINVCO) return HAL_ERROR;

	// I DON'T KNOW WHAT REGISTER TO WRITE TO

//	// Read existing config so we don't destroy other settings
//	uint16_t reg_data;
//	uint8_t h_state = (state & 0x01);
//	uint8_t v_state = (state & 0x10) >> 5;
//
//	if (AR_Read_Register(hi2c, AR_MIRROR_REGISTER, &reg_data) == HAL_OK) {
//		// Clear the bit status
//		reg_data &= ~(1U << 0);
//		reg_data &= ~(1U << 15);
//		// Horizontal and Vertical bits
//		reg_data |= (uint16_t)(h_state << 0);
//		reg_data |= (uint16_t)(v_state << 15);
//		return AR_Write_Register(hi2c, AR_MIRROR_REGISTER, &reg_data);
//	}
	return HAL_ERROR;
}

/**
  * @brief  Enables the DCMI Parallel Interface
  * 		Data on 0x301A[12] and 0x301A[7]
  *
  * @param  *hi2c I2C HAL handle
  *
  * @retval Confirmation of operation, HAL_OK for successful, HAL_ERROR for unsuccessful
  */
HAL_StatusTypeDef AR_Init_Parallel(I2C_HandleTypeDef *hi2c)
{
	uint16_t reg_data;
	uint16_t bit_7 = 1 << 7;
	uint16_t bit_12 = 1 << 12;

	if (AR_Read_Register(hi2c, AR_PARALLEL_REGISTER, &reg_data) == HAL_OK) {
		// Clear the bit status
		reg_data &= ~(1U << 7);
		reg_data &= ~(1U << 12);
		// Update bits
		reg_data |= bit_7;
		reg_data |= bit_12;
		return AR_Write_Register(hi2c, AR_PARALLEL_REGISTER, &reg_data);
	}
	return HAL_ERROR;
}
