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
ULONG tx_time_ms(ULONG ms);

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
	if (1)
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
	tx_thread_create(&ina219_thread, "INA219 Thread",
					INA219_ThreadEntry, 0,
					ina219_stack_ptr, 1024,
					20, 20, TX_NO_TIME_SLICE, TX_AUTO_START
					);

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
		tx_thread_sleep(tx_time_ms(500));
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
		tx_thread_relinquish();
	}
}

void Cam_ThreadEntry()
{
	// External variables
	extern uint32_t pBuffer[MAX_PICTURE_BUFF];
	extern uint8_t FrameProcessed;
	extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
	extern DCMI_HandleTypeDef hdcmi;
	extern I2C_HandleTypeDef hi2c2;
	extern DMA_QListTypeDef DCMIQueue;

	// Set unused picture area to white
	memset(pBuffer, 0x8B, sizeof(pBuffer));

	// OV7670 Configuration
	HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_RESET); 	// Camera PWDN to GND (Enable Camera)
	ov7670_init(&hdcmi, &handle_GPDMA1_Channel0, &hi2c2);						// Basic interface test
	ov7670_config(OV7670_MODE_QVGA_RGB565);										// Register configuration
	//ov7670_testpattern(&hdcmi);												// RGB Test Pattern
	tx_thread_sleep(tx_time_ms(300));												// 300 ms setting time specified by datasheet (Table 4)
	HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_SET);	// Disable Camera

	// DCMI Setup
	MX_DCMIQueue_Config();
	HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel0, &DCMIQueue);
	__HAL_LINKDMA(&hdcmi, DMA_Handle, handle_GPDMA1_Channel0);

	// Pause DCMI until requested
	HAL_DCMI_Suspend(&hdcmi);

	while(1)
	{
		if (tud_cdc_n_connected(0) && tud_cdc_available())
		{
			// Read incoming data to check for start condition
			uint8_t buf[1] = {0};
			tud_cdc_read(buf, sizeof(buf));
			tud_cdc_read_flush();

			if ((char)buf[0] == 'S')
			{
				// Enable camera and wait for stabilisation
				HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_RESET);
				tx_thread_sleep(tx_time_ms(300));

				// Clear memory (for debugging)
				memset(pBuffer, 0x1F, sizeof(pBuffer));

				// Capture Image
				FrameProcessed = 0;
				HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)pBuffer, MAX_PICTURE_BUFF);

				// Block until hardware completes the capture
				while (!FrameProcessed)
				{
					tx_thread_sleep(5);
				}

				// IMAGE CAPTURED AT THIS POINT

				// Stop DCMI and shut down camera power until next request
				HAL_DCMI_Stop(&hdcmi);
				HAL_GPIO_WritePin(DCMI_PWRDWN_GPIO_Port, DCMI_PWRDWN_Pin, GPIO_PIN_SET);


				uint8_t *byteStream = (uint8_t *)pBuffer;
				uint32_t totalBytes = MAX_PICTURE_BUFF * 4;
				uint32_t bytesSent = 0;

				while (bytesSent < totalBytes)
				{
					uint32_t avail = tud_cdc_n_write_available(0);

					if (avail > 0)
					{
						uint32_t chunkSize = totalBytes - bytesSent;
						if (chunkSize > avail) chunkSize = avail; // Saturate the USB FIFO completely

						uint32_t written = tud_cdc_n_write(0, &byteStream[bytesSent], chunkSize);
						if (written > 0)
						{
							bytesSent += written;
						}
					}
					else
					{
						tud_cdc_n_write_flush(0);
						tx_thread_sleep(1);
					}
				}

				tud_cdc_n_write_flush(0);
			}
		}

	// Free up thread
	tx_thread_sleep(tx_time_ms(100));

	}
}

/**
  * @brief  Returns threadx sleep ticks for an input milliseconds.
  * @param ms: time to sleep in milliseconds
  * @retval ticks
  */
ULONG tx_time_ms(ULONG ms)
{
	return ((ms * TX_TIMER_TICKS_PER_SECOND) / 1000);
}

/* USER CODE END 1 */
