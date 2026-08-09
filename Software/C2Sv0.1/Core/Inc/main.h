/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "led.h"
#include "csa.h"
#include "encoder.h"
#include "interrupt.h"
#include "liquidcrystal_i2c.h"
#include "lcd.h"
#include "AT25SF2561C.h"
#include "config.h"
#include "sensor.h"
#include "uSD.h"
#include "fx_stm32_sd_driver.h"


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
void MX_SDMMC1_SD_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SCROLL_A_Pin GPIO_PIN_2
#define SCROLL_A_GPIO_Port GPIOE
#define SCROLL_A_EXTI_IRQn EXTI2_IRQn
#define SCROLL_B_Pin GPIO_PIN_13
#define SCROLL_B_GPIO_Port GPIOC
#define SCROLL_B_EXTI_IRQn EXTI13_IRQn
#define PWR_STATE_Pin GPIO_PIN_0
#define PWR_STATE_GPIO_Port GPIOC
#define PWR_STATE_EXTI_IRQn EXTI0_IRQn
#define PWR_ACTION_Pin GPIO_PIN_1
#define PWR_ACTION_GPIO_Port GPIOC
#define CSA_Pin GPIO_PIN_2
#define CSA_GPIO_Port GPIOC
#define SENSOR_LED_2_Pin GPIO_PIN_13
#define SENSOR_LED_2_GPIO_Port GPIOE
#define SENSOR_LED_1_Pin GPIO_PIN_14
#define SENSOR_LED_1_GPIO_Port GPIOE
#define sLCD_SCL_Pin GPIO_PIN_10
#define sLCD_SCL_GPIO_Port GPIOB
#define sLCD_SDA_Pin GPIO_PIN_11
#define sLCD_SDA_GPIO_Port GPIOB
#define MCU_LED_1_Pin GPIO_PIN_14
#define MCU_LED_1_GPIO_Port GPIOB
#define MCU_LED_2_Pin GPIO_PIN_15
#define MCU_LED_2_GPIO_Port GPIOB
#define READ_LED_1_Pin GPIO_PIN_8
#define READ_LED_1_GPIO_Port GPIOD
#define READ_LED_2_Pin GPIO_PIN_9
#define READ_LED_2_GPIO_Port GPIOD
#define WRITE_LED_1_Pin GPIO_PIN_10
#define WRITE_LED_1_GPIO_Port GPIOD
#define WRITE_LED_2_Pin GPIO_PIN_11
#define WRITE_LED_2_GPIO_Port GPIOD
#define DEBUG_LED_Pin GPIO_PIN_12
#define DEBUG_LED_GPIO_Port GPIOD
#define PIN81_Pin GPIO_PIN_9
#define PIN81_GPIO_Port GPIOH
#define PIN84_Pin GPIO_PIN_10
#define PIN84_GPIO_Port GPIOH
#define PIN85_Pin GPIO_PIN_11
#define PIN85_GPIO_Port GPIOH
#define PIN97_Pin GPIO_PIN_3
#define PIN97_GPIO_Port GPIOI
#define STANDBY_Pin GPIO_PIN_15
#define STANDBY_GPIO_Port GPIOG
#define RESETBAR_Pin GPIO_PIN_3
#define RESETBAR_GPIO_Port GPIOB
#define TRIGGER_Pin GPIO_PIN_4
#define TRIGGER_GPIO_Port GPIOB
#define SHUTTER_Pin GPIO_PIN_6
#define SHUTTER_GPIO_Port GPIOB
#define SHUTTER_EXTI_IRQn EXTI6_IRQn
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOH
#define BOOT0_EXTI_IRQn EXTI3_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
