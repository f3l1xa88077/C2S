/*
 * AT25SF2561C.h
 *
 *  Created on: Feb 20, 2026
 *      Author: felix
 */

#ifndef CUSTOM_INC_AT25SF2561C_H_
#define CUSTOM_INC_AT25SF2561C_H_

#include"main.h"
#include"config.h"

#define QSPI_READ_SR1 0x05
#define QSPI_READ_SR2 0x35
#define QSPI_READ_SR3 0x15
#define QSPI_READ_ID 0x9F
#define QSPI_READ_CMD 0xEC // 0x13 for SPI, 0xEC for QSPI (each 4 byte address)

#define QSPI_WRITE_SR1 0x01
#define QSPI_WRITE_SR2 0x31
#define QSPI_WRITE_SR3 0x11
#define QSPI_WRITE_DISABLE 0x04
#define QSPI_WRITE_ENABLE 0x06
#define QSPI_WRITE_CMD 0x34 // 0x12 for SPI, 0x34 for QSPI (each 4 byte address)

#define QSPI_ERASE_4KB 0X21
#define QSPI_ERASE_32KB 0X5C
#define QSPI_ERASE_64KB 0XDC

#define QSPI_ENTER_SLEEP 0xB9
#define QSPI_ENTER_4BYTE_ADDRESS 0xB7

#define QSPI_EXIT_SLEEP 0xAB
#define QSPI_EXIT_4BYTE_ADDRESS 0xE9

#define QSPI_TIMEOUT_GENERAL 200
#define QSPI_TIMEOUT_WIPE 2500

#define QSPI_PAGE_SIZE 256

typedef struct {
	uint32_t 	data_size;
	void 		*tx_pData;
	void 		*rx_pData;
	uint16_t 	address_block_start;
	uint16_t 	address_block_end;
}QSPI_DataChunk_HandleTypeDef;

typedef struct {
	uint32_t 				occupied_data; 	// Data management tracker
	uint32_t 				memory_size;
	uint8_t 				ID_REG[3]; 		// ID Register
	OSPI_HandleTypeDef 		*hospi;
	OSPI_RegularCmdTypeDef 	*sCommand;
}QSPI_HandleTypeDef;

void QSPI_Reset_Struct(QSPI_HandleTypeDef *QSPI_Memory);
void QSPI_Init_DataBlock(QSPI_DataChunk_HandleTypeDef *DataBlock, uint32_t data_size, void *tx_pData, void *rx_pData);

// Initialisation
HAL_StatusTypeDef QSPI_Init_Memory(OSPI_HandleTypeDef *hospi, OSPI_RegularCmdTypeDef *sCommand, QSPI_HandleTypeDef *QSPI_Memory);

// Commands
HAL_StatusTypeDef QSPI_Command(QSPI_HandleTypeDef *QSPI_Memory, uint8_t cmd);
HAL_StatusTypeDef QSPI_Command_Sleep(QSPI_HandleTypeDef *QSPI_Memory, uint8_t status);

// Validation Commands
HAL_StatusTypeDef QSPI_Validate_Wel(QSPI_HandleTypeDef *QSPI_Memory, uint32_t Timeout);
HAL_StatusTypeDef QSPI_Validate_Rdy(QSPI_HandleTypeDef *QSPI_Memory, uint32_t Timeout);

// Read Commands
HAL_StatusTypeDef QSPI_Read_StatusRegister(QSPI_HandleTypeDef *QSPI_Memory, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef QSPI_Read_JedecId(QSPI_HandleTypeDef *QSPI_Memory);
HAL_StatusTypeDef QSPI_Read_Data(QSPI_HandleTypeDef *QSPI_Memory, QSPI_DataChunk_HandleTypeDef *DataBlock);

// Writing Commands
HAL_StatusTypeDef QSPI_Write_StatusRegister(QSPI_HandleTypeDef *QSPI_Memory, uint8_t reg, uint8_t bit, uint8_t status);
HAL_StatusTypeDef QSPI_Write_Page(QSPI_HandleTypeDef *QSPI_Memory, uint32_t address, uint8_t *pData, uint16_t size);
HAL_StatusTypeDef QSPI_Write_Data(QSPI_HandleTypeDef *QSPI_Memory, QSPI_DataChunk_HandleTypeDef *DataBlock);

// Erase Commands
HAL_StatusTypeDef QSPI_Erase_4kB(QSPI_HandleTypeDef *QSPI_Memory, uint32_t address);
HAL_StatusTypeDef QSPI_Erase_32kB(QSPI_HandleTypeDef *QSPI_Memory, uint32_t address);
HAL_StatusTypeDef QSPI_Erase_64kB(QSPI_HandleTypeDef *QSPI_Memory, uint32_t address);

HAL_StatusTypeDef QSPI_Erase_Data(QSPI_HandleTypeDef *QSPI_Memory, QSPI_DataChunk_HandleTypeDef *DataBlock);

uint32_t QSPI_Test_Reliability(QSPI_HandleTypeDef *QSPI_Memory);


#endif /* CUSTOM_INC_AT25SF2561C_H_ */
