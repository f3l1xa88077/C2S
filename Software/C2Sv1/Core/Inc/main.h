/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "AT25xF2561C.h"
#include <tinyusb/tusb.h>
#include "interrupt.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_GPDMA1_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SHUTTER_Pin GPIO_PIN_13
#define SHUTTER_GPIO_Port GPIOC
#define SHUTTER_EXTI_IRQn EXTI13_IRQn
#define LED_MCU_Pin GPIO_PIN_15
#define LED_MCU_GPIO_Port GPIOC
#define LED_ERR_Pin GPIO_PIN_1
#define LED_ERR_GPIO_Port GPIOH
#define DCMI_GPIO0_Pin GPIO_PIN_5
#define DCMI_GPIO0_GPIO_Port GPIOA
#define DCMI_GPIO1_Pin GPIO_PIN_4
#define DCMI_GPIO1_GPIO_Port GPIOC
#define DCMI_GPIO2_Pin GPIO_PIN_5
#define DCMI_GPIO2_GPIO_Port GPIOC
#define DCMI_GPIO3_Pin GPIO_PIN_0
#define DCMI_GPIO3_GPIO_Port GPIOB
#define DCMI_MCLK_Pin GPIO_PIN_8
#define DCMI_MCLK_GPIO_Port GPIOA
#define WAKEUP_Pin GPIO_PIN_9
#define WAKEUP_GPIO_Port GPIOA
#define WAKEUP_EXTI_IRQn EXTI9_IRQn
#define SPI1_CS_Pin GPIO_PIN_10
#define SPI1_CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

void SystemClock_Config(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
