/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define USART_DMA_REC_SIE 256
#define USART_REC_SIE 512
typedef struct
{   
	uint8_t  UsartRecFlag;   
	uint16_t UsartDMARecLen; 
	uint16_t UsartRecLen;    
	uint8_t  UsartDMARecBuffer[USART_DMA_REC_SIE];  
	uint8_t  UsartRecBuffer[USART_REC_SIE];         
} ts_Usart_type;
extern ts_Usart_type Usart2type; 
extern ts_Usart_type Usart1type; 

#define BUFF_MAX_SIZE 255

typedef struct usartRecvType{
	uint8_t recvData;	// ��������
	uint8_t recvBuff[BUFF_MAX_SIZE];  // �������ݵĻ�����
	uint8_t recvFlag; // �ɹ�����һ�����ݵı�־λ
	uint16_t recvNum; // �������ݵ��ֽ���
} usartRecvType_t;

extern usartRecvType_t usart1Recv;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
