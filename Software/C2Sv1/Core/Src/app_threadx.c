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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void INA219_ThreadEntry();
void USB_ThreadEntry();

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

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */



	// Current Sense Amplifier
	tx_thread_create(&ina219_thread, "INA219 Thread",
					INA219_ThreadEntry, 0,
					ina219_stack_ptr, 1024,
					16, 16, TX_NO_TIME_SLICE, TX_AUTO_START
					);

	// TinyUSB
	if (OPERATIONAL_MODE == USB_MODE) {

		tx_thread_create(&usb_thread, "TinyUSB Task Thread",
							USB_ThreadEntry, 0,
							usb_stack_ptr, 1024,
							20, 20, TX_NO_TIME_SLICE, TX_AUTO_START);

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

extern uint32_t *atat;

void INA219_ThreadEntry()
{
	// Setup
	extern I2C_HandleTypeDef hi2c2;
	uint32_t delay_ticks = (500 * TX_TIMER_TICKS_PER_SECOND) / 1000 ;
	INA219_HandleTypeDef INA219_Chip;
	INA219_Setup(&INA219_Chip, &hi2c2);

	// Continuous Operation
	while(1)
	{
		// Read CSA Data
		INA219_ReadAll(&INA219_Chip);

		// Send data over COM if in USB Mode
		if (OPERATIONAL_MODE == USB_MODE) {

			// Attach data to string
			char tmp[20];
			snprintf(tmp, sizeof(tmp), "%d\n", INA219_Chip.shunt_voltage);

			// Send each character to buffer
//			for (int i=0; i<strlen(tmp); i++)
//			{
//				tud_cdc_n_write_char(0, tmp[i]);
//			}
			for (int i=0; i<2; i++)
			{
				tud_cdc_n_write_char(0, atat[i]);
			}
			// Send buffer
			tud_cdc_n_write_flush(0);

		}

		// Wait
		tx_thread_sleep(delay_ticks);
	}
}

void USB_ThreadEntry()
{
	// Initialize the TinyUSB stack
	tusb_rhport_init_t dev_init = {
				.role = TUSB_ROLE_DEVICE,
				.speed = TUSB_SPEED_FULL
	};
	tusb_init(0, &dev_init);

	while(1)
	{
		tud_task();
		tx_thread_sleep(1);
	}
}

/* USER CODE END 1 */
