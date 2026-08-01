/* USER CODE BEGIN Header */
/**
 ******************************************************************************
* @file    app_threadx.c
* @author  MCD Application Team
* @brief   ThreadX applicative file
******************************************************************************
	* @attention
*
* Copyright (c) 2026 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

TX_THREAD ina219_thread;
TX_THREAD usb_thread;
TX_THREAD cam_thread;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void INA219_ThreadEntry();
void USB_ThreadEntry();
void Cam_ThreadEntry();
void tx_thread_sleep_ms(ULONG ms);
void tud_transmit_data(uint8_t* image_data, uint32_t total_bytes);
void tud_transmit(uint8_t packet_id, uint8_t * data, uint32_t total_bytes);
static void CUSTOM_DCMI_DMAXferCplt(DMA_HandleTypeDef *hdma);

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

	TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

	// Current Sense Amplifier
	void *ina219_stack_ptr;
	if (CSA_ACTIVE)
	{
		if (tx_byte_allocate(byte_pool, &ina219_stack_ptr, 1024, TX_NO_WAIT) != TX_SUCCESS)
		{
			return TX_NOT_DONE;
		}
	}

	// TinyUSB
	void *usb_stack_ptr;
	if (OPERATIONAL_MODE == USB_MODE)
	{
		if (tx_byte_allocate(byte_pool, &usb_stack_ptr, 1024, TX_NO_WAIT) != TX_SUCCESS)
		{
		return TX_NOT_DONE;
		}
	}

	// OV7670
	void *cam_stack_ptr;
	if (OPERATIONAL_MODE == USB_MODE)
	{
		if (tx_byte_allocate(byte_pool, &cam_stack_ptr, 4096, TX_NO_WAIT) != TX_SUCCESS)
		{
		return TX_NOT_DONE;
		}
	}

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */



	// Current Sense Amplifier
	if (CSA_ACTIVE)
	{
		tx_thread_create(&ina219_thread, "INA219 Thread",
							INA219_ThreadEntry, 0,
							ina219_stack_ptr, 1024,
							20, 20, TX_NO_TIME_SLICE, TX_AUTO_START
							);
	}

	// TinyUSB
	if (OPERATIONAL_MODE == USB_MODE) {

		tx_thread_create(&usb_thread, "TinyUSB Task Thread",
							USB_ThreadEntry, 0,
							usb_stack_ptr, 1024,
							16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

	}

	// Camera
	if (OPERATIONAL_MODE == USB_MODE) {

		tx_thread_create(&cam_thread, "Camera Task Thread",
							Cam_ThreadEntry, 0,
							cam_stack_ptr, 4096,
							22, 22, TX_NO_TIME_SLICE, TX_AUTO_START);

	}

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

void INA219_ThreadEntry()
{
	// Setup
	extern I2C_HandleTypeDef hi2c2;
	INA219_HandleTypeDef INA219_Chip;
	INA219_Setup(&INA219_Chip, &hi2c2);

	// Continuous Operation
	while(1)
	{
		// Read CSA Data
		INA219_ReadAll(&INA219_Chip);

		if (tud_cdc_n_connected(0) && tud_cdc_available())
		{
			uint16_t power_to_send = (uint16_t)INA219_Chip.power_mW;
			tud_transmit(0x02, (uint8_t*)(&power_to_send), sizeof(uint16_t));
		}

		tx_thread_sleep_ms(500);
	}
}

void USB_ThreadEntry()
{

	HAL_PWREx_EnableVddUSB(); // Isolate VDDUSB

	// Initialize the TinyUSB stack
	tusb_rhport_init_t dev_init = {
				.role = TUSB_ROLE_DEVICE,
				.speed = TUSB_SPEED_FULL
	};
	tusb_init(0, &dev_init);

	while(1)
	{
		// Poll PC
		tud_task();
		//tx_thread_sleep_ms(20));
		tx_thread_relinquish();
	}
}

void Cam_ThreadEntry()
{
	// External variables
	extern uint32_t pBuffer[LL_MAX_NODE_SIZE*2];
	extern uint32_t bufferA[DB_SIZE_WORDS];
	extern uint32_t bufferB[DB_SIZE_WORDS];
	extern uint32_t bufferC[LL_MAX_NODE_SIZE];

	extern volatile uint8_t FrameProcessed;
	extern volatile uint32_t processedRows;

	extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
	extern DCMI_HandleTypeDef hdcmi;
	extern I2C_HandleTypeDef hi2c2;
	extern DMA_QListTypeDef DCMIQueue;

	extern OSPI_HandleTypeDef hospi1;
	extern QSPI_HandleTypeDef QSPI_Memory;
	extern OSPI_RegularCmdTypeDef sCommand;

	// Setup QSPI
	QSPI_Init_Memory(&hospi1, &sCommand, &QSPI_Memory);

	// Reset & Disable Camera
	HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_RESET);
	OV7670_Init(&hdcmi, &handle_GPDMA1_Channel0, &hi2c2);
	HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_SET);

	// DCMI Setup
	MX_DCMIQueue_Config();
	HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel0, &DCMIQueue);
	__HAL_LINKDMA(&hdcmi, DMA_Handle, handle_GPDMA1_Channel0);

	// Pause DCMI until requested
	HAL_DCMI_Suspend(&hdcmi);

	QSPI_DataBlock_HandleTypeDef DataBlock;
	QSPI_Init_DataBlock(&DataBlock, DB_SIZE_BYTES, NULL, bufferC);

	QSPI_Command(&QSPI_Memory, QSPI_ERASE_CHIP);

	while(1)
	{
		if (tud_cdc_n_connected(0) && tud_cdc_available())
		{
			// Read incoming data to check for start condition
			uint8_t buf[5];
			tud_cdc_read(buf, sizeof(buf));
			tud_cdc_read_flush();

			if ((buf[0] == 0xFF) && (buf[4] == 0xFE)) // Check start and end condition
			{
				// Extract data
				uint8_t colour_mode = buf[1];
				uint8_t resolution_mode = buf[2];
				uint8_t test_pattern_mode = buf[3];

				// Enable camera and wait for stabilisation
				HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_RESET);
				OV7670_Config(colour_mode, resolution_mode, test_pattern_mode);
				tx_thread_sleep_ms(300); // Pause for register configuration

				/*
				 * 	----------------------
				 * 		IMAGE CAPTURE
				 * 	----------------------
				 */

				// Reset Flags
				FrameProcessed = 0;
				processedRows = 0;

				// Declare Variables
				uint32_t volatile rowCnt = 0;
				uint32_t volatile dataCount = 0;

				// Create Pseudo Data Block
				DataBlock.address_block_end = DataBlock.address_block_start;
				uint32_t volatile curAddress = DataBlock.address_block_start;
				uint16_t volatile pageRemaining = QSPI_PAGE_SIZE - (curAddress % QSPI_PAGE_SIZE);
				if (pageRemaining == 0) pageRemaining = QSPI_PAGE_SIZE;

				DataBlock.data_size = LL_MAX_NODE_SIZE;

				// Start Image Capture
				//HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)bufferA, DB_SIZE_WORDS);
				HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)pBuffer, LL_MAX_NODE_SIZE/4*2);
				hdcmi.DMA_Handle->XferCpltCallback = CUSTOM_DCMI_DMAXferCplt;
				__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME); // Ensure VSYNC/Frame callback fires!


				// Write Data to QSPI Memory
				while (!FrameProcessed) {
					if (processedRows > rowCnt)
					{
						// Declare the transmitting buffer
						uint8_t *activeBuffer = (uint8_t*)((processedRows % 2 == 0) ? pBuffer + (LL_MAX_NODE_SIZE/4) : pBuffer);
						uint32_t bufferRemaining = LL_MAX_NODE_SIZE;

						// Keep writing until buffer has been fully written to memory
						while (bufferRemaining > 0)
						{
							// Calculate how much data left in current page
							uint16_t dataRemaining = (bufferRemaining >= pageRemaining) ? pageRemaining : bufferRemaining;
							pageRemaining = QSPI_PAGE_SIZE - dataRemaining;

							// Write [dataRemaining] bytes to memory and increment address to reflect memory change
							QSPI_Write_Page(&QSPI_Memory, curAddress, activeBuffer, dataRemaining);
							curAddress += dataRemaining;		// Increment QSPI address
							activeBuffer += (dataRemaining);	// Increment buffer position.
							bufferRemaining -= dataRemaining;	// Decrement buffer counter
							dataCount += dataRemaining;

							// If the page has been completely written to, reset bytes left for page.
							if (pageRemaining == 0) pageRemaining = QSPI_PAGE_SIZE;
						}
						// If the buffer has been fully written, increment row and reset buffer counter.
						rowCnt++;
						bufferRemaining = LL_MAX_NODE_SIZE;
						DataBlock.address_block_end += LL_MAX_NODE_SIZE;
						QSPI_Memory.occupied_data += LL_MAX_NODE_SIZE;

					}
				}

				// Stop DCMI and shut down camera power until next request
				HAL_DCMI_Stop(&hdcmi);
				HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_SET);

				//QSPI_Memory.occupied_data += ((dataCount + 4095) & ~4095);

				// Sanity Check
				//if (DataBlock.address_block_end != curAddress) Error_Handler();

				/*  ----------------------------
				 *  IMAGE CAPTURED AT THIS POINT
				 *  ----------------------------
				 */

				// Temporarily put in memory
				//QSPI_Init_DataBlock(&DataBlock, LL_MAX_NODE_SIZE*4, pBuffer, bufferC);
				//QSPI_Write_Data(&QSPI_Memory, &DataBlock);
				//rowCnt = IMAGE_ROWS / DB_ROWS;

				// Read information from QSPI memory
				uint32_t origin = DataBlock.address_block_start;
				DataBlock.data_size = DB_SIZE_BYTES;
				uint32_t transmittedData = 0;
				rowCnt = rowCnt * (LL_MAX_NODE_SIZE / DB_SIZE_BYTES);
				while (rowCnt > 0) {
					QSPI_Read_Data(&QSPI_Memory, &DataBlock);
					tud_transmit(0x01, (uint8_t*)bufferC, DB_SIZE_BYTES);
					DataBlock.address_block_start += DB_SIZE_BYTES;
					transmittedData += DB_SIZE_BYTES;
					rowCnt--;
				}

				// Clear LIFO buffer for next image
				DataBlock.address_block_start = origin;
				DataBlock.data_size = DataBlock.address_block_end - DataBlock.address_block_start;
				QSPI_Erase_Data(&QSPI_Memory, &DataBlock);
			}

		}

	// Free up thread
	tx_thread_sleep_ms(50);

	}
}

/**
  * @brief  Transmits given data over TinyUSB to the dashboard
  *
  * @param image_data: 8-bit array of data to be sent
  * @param total_bytes: Total Bytes of image
  */
void tud_transmit_data(uint8_t* data, uint32_t total_bytes)
{
    uint32_t bytes_sent = 0;

    while (bytes_sent < total_bytes)
    {
        uint32_t available_space = tud_cdc_write_available();

        if (available_space > 0)
        {
            uint32_t remaining_bytes = total_bytes - bytes_sent;
            uint32_t chunk_size = (remaining_bytes < available_space) ? remaining_bytes : available_space;
            uint32_t written = tud_cdc_write(&data[bytes_sent], chunk_size);
            bytes_sent += written;
        }

        tud_cdc_write_flush();

    }
}

/**
  * @brief  Threadx sleep ticks for an input milliseconds.
  *
  * @param ms: time to sleep in milliseconds
  */
void tx_thread_sleep_ms(ULONG ms)
{
	tx_thread_sleep((ms * TX_TIMER_TICKS_PER_SECOND) / 1000);
}

/**
  * @brief Sends data in packet format
  *
  * @param packet_id: Identifier of the data being sent (0x01: Image, 0x02: CSA)
  * @param data: Data to be sent in 8-bit pointer format
  * @param total_bytes: Total Bytes of image
  */
void tud_transmit(uint8_t packet_id, uint8_t * data, uint32_t total_bytes)
{
	// Definitions
	uint16_t start_packet = 0xAA55;
	uint8_t end_packet = 0xBB;

	// Start of packet
	tud_transmit_data((uint8_t*)(&start_packet), 2);

	// Packet identifier
	tud_transmit_data(&packet_id, 1);

	// Data length
	tud_transmit_data((uint8_t*)(&total_bytes), 4);

	// Send data
	tud_transmit_data(data, total_bytes);

	// End of packet
	tud_transmit_data((uint8_t*)(&end_packet), 1);
}

static void CUSTOM_DCMI_DMAXferCplt(DMA_HandleTypeDef *hdma)
{

  DCMI_HandleTypeDef *hdcmi = (DCMI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  uint32_t tmp1;
  uint32_t tmp2;
  DMA_NodeTypeDef *pnode;
  uint32_t pbuff;
  uint32_t transfernumber;
  uint32_t transfercount;
  uint32_t transfersize ;

	extern volatile uint32_t processedRows;
	processedRows++;

  /* Update Nodes destinations */
  if (hdcmi->XferSize != 0U)
  {
    pbuff          = hdcmi->pBuffPtr;
    transfernumber = hdcmi->XferTransferNumber;
    transfercount  = hdcmi->XferCount;
    transfersize   = hdcmi->XferSize;

    tmp1 = hdcmi->DMA_Handle->Instance->CLLR & DMA_CLLR_LA;
    tmp2 = hdcmi->DMA_Handle->Instance->CLBAR & DMA_CLBAR_LBA;
    pnode = (DMA_NodeTypeDef *)(uint32_t)(tmp1 | tmp2);

    if (hdcmi->XferCount > 1U)
    {
      pnode->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = pbuff + ((transfernumber - transfercount + 2U) * transfersize);
      hdcmi->XferCount--;
    }

    else if (hdcmi->XferCount == 1U)
    {
      pnode->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = hdcmi->pBuffPtr;
      hdcmi->XferCount--;
    }
    else
    {
      pnode->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = hdcmi->pBuffPtr + hdcmi->XferSize;

      /* When Continuous mode, re-set dcmi XferCount */
      if ((hdcmi->Instance->CR & DCMI_CR_CM) == DCMI_MODE_CONTINUOUS)
      {
        hdcmi->XferCount = hdcmi->XferTransferNumber ;
      }
      /* When snapshot mode, set dcmi state to ready */
      else
      {
        hdcmi->State = HAL_DCMI_STATE_READY;
      }

      __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);
    }
  }
  else  /* Snapshot Mode */
  {
    /* Enable the Frame interrupt */
    __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);

    /* When snapshot mode, set dcmi state to ready */
    if ((hdcmi->Instance->CR & DCMI_CR_CM) == DCMI_MODE_SNAPSHOT)
    {
      hdcmi->State = HAL_DCMI_STATE_READY;
    }
  }
}

/* USER CODE END 1 */
