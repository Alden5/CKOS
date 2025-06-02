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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PUSH_DOWN_Pin GPIO_PIN_13
#define PUSH_DOWN_GPIO_Port GPIOC
#define PUSH_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define PUSH_B_Pin GPIO_PIN_1
#define PUSH_B_GPIO_Port GPIOA
#define PUSH_B_EXTI_IRQn EXTI1_IRQn
#define PUSH_LEFT_Pin GPIO_PIN_2
#define PUSH_LEFT_GPIO_Port GPIOA
#define PUSH_LEFT_EXTI_IRQn EXTI2_IRQn
#define DISP_PWR_Pin GPIO_PIN_3
#define DISP_PWR_GPIO_Port GPIOA
#define DISPLAY_CS_Pin GPIO_PIN_4
#define DISPLAY_CS_GPIO_Port GPIOA
#define DISP_RST_Pin GPIO_PIN_6
#define DISP_RST_GPIO_Port GPIOA
#define BAT_REF_Pin GPIO_PIN_0
#define BAT_REF_GPIO_Port GPIOB
#define DISPLAY_DC_Pin GPIO_PIN_2
#define DISPLAY_DC_GPIO_Port GPIOB
#define DISP_RED_Pin GPIO_PIN_12
#define DISP_RED_GPIO_Port GPIOB
#define DISP_GREEN_Pin GPIO_PIN_13
#define DISP_GREEN_GPIO_Port GPIOB
#define DISP_BLUE_Pin GPIO_PIN_14
#define DISP_BLUE_GPIO_Port GPIOB
#define PUSH_A_Pin GPIO_PIN_5
#define PUSH_A_GPIO_Port GPIOB
#define PUSH_A_EXTI_IRQn EXTI9_5_IRQn
#define PUSH_UP_Pin GPIO_PIN_6
#define PUSH_UP_GPIO_Port GPIOB
#define PUSH_UP_EXTI_IRQn EXTI9_5_IRQn
#define PUSH_RIGHT_Pin GPIO_PIN_7
#define PUSH_RIGHT_GPIO_Port GPIOB
#define PUSH_RIGHT_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
