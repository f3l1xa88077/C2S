/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/* Private includes ----------------------------------------------------------*/
#include "fx_stm32_spi_driver.h"

/* USER CODE BEGIN Includes */
UINT  _fx_partition_offset_calculate(void  *partition_sector, UINT partition, ULONG *partition_start, ULONG *partition_size);
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN USER_CODE_SECTION_0 */

/* USER CODE END USER_CODE_SECTION_0 */

VOID  fx_stm32_spi_driver(FX_MEDIA *media_ptr)
{
  /* USER CODE BEGIN USER_CODE_SECTION_1 */
	UCHAR *source_buffer;
	UCHAR *destination_buffer;
	UINT status;
	ULONG partition_start;
	ULONG partition_size;
  /* USER CODE END USER_CODE_SECTION_1 */

  switch (media_ptr->fx_media_driver_request)
  {
    case FX_DRIVER_INIT:
    {

     /* USER CODE BEGIN DRIVER_INIT */
     USER_SPI_initialize(0);
     /* USER CODE END DRIVER_INIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_INIT */

     /* USER CODE END POST_DRIVER_INIT */
      break;
    }

    case FX_DRIVER_UNINIT:
    {
     /* USER CODE BEGIN DRIVER_UNINIT */

     /* USER CODE END DRIVER_UNINIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_UNINIT */

     /* USER CODE END POST_DRIVER_UNINIT */
      break;
    }

    case FX_DRIVER_BOOT_READ:
    {
    /* USER CODE BEGIN DRIVER_BOOT_READ */
    	//source_buffer = ((media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors) * media_ptr->fx_media_bytes_per_sector);
    	USER_SPI_read (0,media_ptr -> fx_media_driver_buffer, 0, media_ptr->fx_media_driver_sectors);

	/* Check if the sector 0 is the actual boot sector, otherwise calculate the offset into it.
		  Please note that this should belong to higher level of MW to do this check and it is here
		  as a temporary work solution */

		  partition_start =  0;

		  status =  _fx_partition_offset_calculate(media_ptr -> fx_media_driver_buffer, 0, &partition_start, &partition_size);

		  /* Now determine if there is a partition...   */
		  if (partition_start){

			/*	if (check_sd_status(FX_STM32_SD_INSTANCE) != 0)
				{
				  media_ptr->fx_media_driver_status =  FX_IO_ERROR;
				  break;
				}

				/* Yes, now lets read the actual boot record.  */
			    USER_SPI_read (0,media_ptr -> fx_media_driver_buffer, partition_start, media_ptr->fx_media_driver_sectors);
				//sd_read_data(media_ptr, partition_start, media_ptr->fx_media_driver_sectors, media_ptr -> fx_media_driver_buffer)

		     /* USER CODE END DRIVER_READ */
		  }
     /* USER CODE END DRIVER_BOOT_READ */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_BOOT_READ */

     /* USER CODE END POST_DRIVER_BOOT_READ */
      break;
    }

    case FX_DRIVER_READ:
    {
    /* USER CODE BEGIN DRIVER_READ */
    //source_buffer = ((media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors) * media_ptr->fx_media_bytes_per_sector);
    USER_SPI_read (0,media_ptr -> fx_media_driver_buffer, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors, media_ptr->fx_media_driver_sectors);
     /* USER CODE END DRIVER_READ */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_READ */

     /* USER CODE END POST_DRIVER_READ */
      break;
    }

    case FX_DRIVER_BOOT_WRITE:
    {
    /* USER CODE BEGIN DRIVER_BOOT_WRITE */

     /* USER CODE END DRIVER_BOOT_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_BOOT_WRITE */

     /* USER CODE END POST_DRIVER_BOOT_WRITE */
      break;
    }

    case FX_DRIVER_WRITE:
    {

    /* USER CODE BEGIN DRIVER_WRITE */
    USER_SPI_write (0, media_ptr->fx_media_driver_buffer, media_ptr->fx_media_driver_logical_sector +  media_ptr->fx_media_hidden_sectors, media_ptr->fx_media_driver_sectors);
     /* USER CODE END DRIVER_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_WRITE */

     /* USER CODE END POST_DRIVER_WRITE */
      break;
    }

    case FX_DRIVER_FLUSH:
    {
    /* USER CODE BEGIN DRIVER_FLUSH */

     /* USER CODE END DRIVER_FLUSH */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_FLUSH */

     /* USER CODE END POST_DRIVER_FLUSH */
      break;
    }

    case FX_DRIVER_ABORT:
    {

    /* USER CODE BEGIN DRIVER_ABORT */

     /* USER CODE END DRIVER_ABORT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_ABORT */

     /* USER CODE END POST_DRIVER_ABORT */
      break;
    }

    default:
    {
        media_ptr->fx_media_driver_status = FX_IO_ERROR;
        break;
    }
  }
}

/* USER CODE BEGIN USER_CODE_SECTION_2 */

/* USER CODE END USER_CODE_SECTION_2 */

