/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define DEBUG_Printf 1

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Blink_Leds(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t ms);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_NET_Pin GPIO_PIN_0
#define LED_NET_GPIO_Port GPIOB
#define LED_SEND_Pin GPIO_PIN_1
#define LED_SEND_GPIO_Port GPIOB
#define LED_RECV_Pin GPIO_PIN_2
#define LED_RECV_GPIO_Port GPIOB
#define LCD_CSS_Pin GPIO_PIN_15
#define LCD_CSS_GPIO_Port GPIOA
#define LCD_SCLK_Pin GPIO_PIN_3
#define LCD_SCLK_GPIO_Port GPIOB
#define LCD_MISO_Pin GPIO_PIN_4
#define LCD_MISO_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_5
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_6
#define LCD_LED_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_8
#define D2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define LED_NET_ON   HAL_GPIO_WritePin(LED_NET_GPIO_Port, LED_NET_Pin, GPIO_PIN_RESET)
#define LED_NET_OFF  HAL_GPIO_WritePin(LED_NET_GPIO_Port, LED_NET_Pin, GPIO_PIN_SET)
#define LED_SEND_ON  HAL_GPIO_WritePin(LED_SEND_GPIO_Port, LED_SEND_Pin, GPIO_PIN_RESET)
#define LED_SEND_OFF HAL_GPIO_WritePin(LED_SEND_GPIO_Port, LED_SEND_Pin, GPIO_PIN_SET)
#define LED_RECV_ON  HAL_GPIO_WritePin(LED_RECV_GPIO_Port, LED_RECV_Pin, GPIO_PIN_RESET)
#define LED_RECV_OFF HAL_GPIO_WritePin(LED_RECV_GPIO_Port, LED_RECV_Pin, GPIO_PIN_SET)

#define D1_READ_VAL  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)
#define D2_READ_VAL  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)

#define RELAY_ON     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define RELAY_OFF    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)

#define FAN_ON     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define FAN_OFF    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
