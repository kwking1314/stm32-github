#ifndef ESP8266_H 
#define ESP8266_H 

#include <stdio.h>
#include <string.h>
#include "usart.h"

/* USER CODE BEGIN Private defines */
#define LED_4_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define LED_4_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)

#define LED_3_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define LED_3_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)

#define LED_2_ON  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)
#define LED_2_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)


#define  ESP8266_RST_L __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE)
#define  ESP8266_RST_H __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE)
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;
 
typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	
 
typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
_Bool cmdAT(char *cmdData,char *expReturn1,char *expReturn2,char *expReturn3, double timeout);
_Bool testAT( void );
_Bool ESP8266_DHCP_CUR (void);
_Bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode );
_Bool ESP8266_JoinAP ( char * pSSID, char * pPassWord );
_Bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx );
_Bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
_Bool ESP8266_Read_IP(void);
_Bool ESP8266_UnvarnishSend ( void );
_Bool ESP8266_UnvarnishStop ( void );
_Bool ESP8266_Link_Close ( void );

#ifdef __cplusplus
extern "C" {
#endif
 
#define WIFI_SSID                "fssy7052"                //要连接的热点的名称
#define WIFI_PASSWORD            "fssy7052"           //要连接的热点的密钥
 
#define SERVER_IP                "192.168.31.196"      //要连接的服务器的 IP
#define SERVER_PORT              "8080"               //要连接的服务器的端口
#define LOCAL_PORT				 "8081"				//本机的端口号
 
_Bool initEsp8266(void);  //初始化函数
extern uint8_t StationIPV4[14];  // 存储本地IPv4
#ifdef __cplusplus
}
#endif

#endif
 

