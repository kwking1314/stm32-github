#include "bsp_esp8266.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bsp_lcd.h"
#include "bsp_softwareTimer.h"
#include "fs_protocol.h"
#include "app_system.h"


static tsTimeType timeESP, timeSendData; // �����ʱ��
teESP_TaskStatus ESP_TaskStatus;         // ģ������״̬
static teATCmdNum ATNextCmdNum;          // ����ָ��
static teATCmdNum ATCurrentCmdNum;       // ��ǰָ��
static teATCmdNum ATRecCmdNum;           // ����ָ��
static uint8_t CurrentRty;               // �ط�����

#define WIFI_SSID "1023"            // WiFi�ȵ��SSID��
#define WIFI_PASSWD "1234567890123" // WiFi�ȵ������

#define UDP_SER_IP "255.255.255.255" // IP��ַ

static char *LOCAL_WIFI_IP = NULL; // ����IP��ַ

tsATCmds ATCmds[] = {
    {"AT\r\n", "OK", 500, NO_REC, 20},                                      // �鿴ESP8266�Ƿ���������
    {"AT+CWMODE=1\r\n", "OK", 500, NO_REC, 20},                             // ����Ϊstationģʽ
    {"AT+CWJAP=\"farsight_%s\",\"%s\"\r\n", "OK", 3000, NO_REC, 20},        // ����AP�ȵ�
    {"AT+CIFSR\r\n", "OK", 500, NO_REC, 5},                                 // ��ѯ����IP��ַ
		
    {"AT+CIPSTART=\"UDP\",\"%s\",20000,20000,0\r\n", "OK", 1000, NO_REC, 20}, 	//����UDP�����ģʽ
		{"AT+CIPMUX=0\r\n", "OK", 500, NO_REC, 20},	// ����WiFiΪ������ģʽ
    {"AT+CIPMODE=1\r\n", "OK", 500, NO_REC, 20},                            // ����͸��ģʽ
    {"AT+CIPSEND\r\n", "AT+CIPSEND\r\r\n\r\nOK\r\n\r\n>", 500, NO_REC, 20}, // ��ʼ͸��
		
		{"+++", "+++", 2000, NO_REC, 3},						// �ر�͸������
    {"AT+CIPMODE=0\r\n", "OK", 500, NO_REC, 3},	// �ر�͸��ģʽ
    {"AT+CIPCLOSE\r\n", "OK", 500, NO_REC, 3}, 	// �Ͽ�����
};

/**
 * �������ܣ� ��ʼ�� ESP8266-WiFiģ�� ��ر���
 * ������
 *      ��
 * ����ֵ��
 *      ��
 */
void ESP8266_Init(void)
{
  printf("ESP8266_Init...\r\n");
	
	Gui_DrawFont_GBK16(94, 32, BLACK, YELLOW, (uint8_t *)WIFI_SSID);
	
  ESP_TaskStatus = ESP_SEND;                        //���뷢��ģʽ
  ATCurrentCmdNum = AT;                             //��ǰָ��
  ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1); //��һ��ָ��
}
/**
 * �������ܣ� ESP8266 ����ATָ������
 * ������
 *      @ATCmdNum�� ATָ��ö�٣���ǰ��ATָ���Ӧ
 * ����ֵ��
 *      ��
 */
static void ATSend(teATCmdNum ATCmdNum)
{
  memset(Usart2type.UsartRecBuffer, 0, USART_REC_SIE);
  ATCmds[ATCmdNum].ATStatus = NO_REC; //����Ϊ δ����
  ATRecCmdNum = ATCmdNum;
  char AT_SendBuff[100] = {0};

  // ����AP�ȵ�
  if (ATCmdNum == AT_CWJAP)
  {
    sprintf(AT_SendBuff, ATCmds[ATCmdNum].ATSendStr, WIFI_SSID, WIFI_PASSWD);
    HAL_UART_Transmit(&huart2, (uint8_t *)AT_SendBuff, strlen(AT_SendBuff), 100);
    printf("SendDataBuff:%s\r\n", AT_SendBuff);
  }
  // ����������
  else if (ATCmdNum == AT_CIPSTART)
  {
    sprintf(AT_SendBuff, ATCmds[ATCmdNum].ATSendStr, UDP_SER_IP);
    HAL_UART_Transmit(&huart2, (uint8_t *)AT_SendBuff, strlen(AT_SendBuff), 100);
    printf("SendDataBuff:%s\r\n", AT_SendBuff);
  }
  else
  {
    HAL_UART_Transmit(&huart2, (uint8_t *)ATCmds[ATCmdNum].ATSendStr, strlen(ATCmds[ATCmdNum].ATSendStr), 0xff);
    printf("SendDataBuff:%s\r\n", ATCmds[ATCmdNum].ATSendStr);
  }

  //�򿪳�ʱ��ʱ��
  setTime(&timeESP, ATCmds[ATCmdNum].TimeOut);
}
/**
 * �������ܣ� ESP8266 ��������
 * ������
 *      ��
 * ����ֵ��
 *      ��
 */
static void ATRecv(void)
{
  if (Usart2type.UsartRecFlag) // ���ڽ��յ�����
  {
    printf("RecvStr: %s, Rev: %s\r\n", ATCmds[ATRecCmdNum].ATRecStr, Usart2type.UsartRecBuffer);

    if (strstr((const char *)Usart2type.UsartRecBuffer, ATCmds[ATRecCmdNum].ATRecStr) != NULL) //���������ӦУ���
    {
      ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC; //����Ϊ�ɹ�����
    }

    // ����ǲ�ѯIP��ַ�ɹ���
    if ((ATRecCmdNum == AT_CIFSR) && (ATCmds[AT_CIFSR].ATStatus == SUCCESS_REC))
    {
      char *pStrstr = NULL;
      char *pStrtok = NULL;
      if ((pStrstr = strstr((const char *)Usart2type.UsartRecBuffer, "+CIFSR:STAIP")) != NULL)
      {
        pStrtok = strtok(pStrstr, "\"");
        printf("========= pStrtok: %s\r\n", pStrtok);

        LOCAL_WIFI_IP = strtok(NULL, "\"");
        printf("========= Local WiFi IP: %s\r\n", LOCAL_WIFI_IP);
        Gui_DrawFont_GBK16(30, 48, BLACK, YELLOW, (uint8_t *)LOCAL_WIFI_IP);

        LED_NET_ON;

        pStrstr = NULL;
        pStrtok = NULL;
      }
    }
    Usart2type.UsartRecFlag = 0; //��ս��ձ�־λ
    Usart2type.UsartRecLen = 0;  //��ս��ճ���
    memset(Usart2type.UsartRecBuffer, 0, sizeof(Usart2type.UsartRecBuffer));
  }
}

/**
 * �������ܣ� ESP8266 ����ɹ�������ʧ������
 * ������
 *      ��
 * ����ֵ��
 *      ��
 */
static void Rec_WaitAT(void)
{

  if (ATCmds[ATCurrentCmdNum].ATStatus == SUCCESS_REC) // �ɹ����յ����ݰ�
  {
    switch (ATCurrentCmdNum)
    {
    case AT: // ATָ����Գɹ�
      printf("\r\n AT success...\r\n");
      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CWMODE: // ����WiFi����ģʽ
      printf("\r\n AT_CWMODE success...\r\n");
      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CWJAP: // ����AP�ȵ�
      printf("\r\n AT_CWJAP success...\r\n");
      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CIFSR: // ��ѯ����IP��ַ
      printf("\r\n AT_CIFSR success...\r\n");

      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CIPSTART: // ��������
      printf("\r\n AT_CIPSTART success...\r\n");

      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CIPMODE_1: // ����͸��ģʽ
      printf("\r\n AT_CIPMODE success...\r\n");
      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      break;

    case AT_CIPSEND: // ����͸��ģʽ�ɹ�

      printf("\r\n AT_CIPSEND success...\r\n");
      ESP_TaskStatus = ESP_IDIE;
			
			LED_NET_ON;
			
      setTime(&timeSendData, 500);
      break;

    default:
      ATCurrentCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND; // ������һ������
      break;
    }
  }

  else if (compareTime(&timeESP)) //��û�յ�Ԥ������  ���ҳ�ʱ
  {
    printf("CurrentRty: %d, CmdNum: %d\r\n", CurrentRty, ATCurrentCmdNum);
		
		switch(ATCurrentCmdNum)
		{
			case AT_CWJAP:
				LED_NET_ON;
				HAL_Delay(100);
				LED_NET_OFF;
				LED_RECV_ON;
				HAL_Delay(50);
				LED_RECV_OFF;
				LED_SEND_ON;
				HAL_Delay(50);
				LED_SEND_OFF;
			break;
			default: 
			break;
		}
		
    ATCmds[ATCurrentCmdNum].ATStatus = TIME_OUT;
		LED_NET_OFF;
    if (CurrentRty > 0) //�ط�
    {
      CurrentRty--;
      printf("now:%d,trytimes:%d\r\n", ATCurrentCmdNum, CurrentRty);
      ATNextCmdNum = ATCurrentCmdNum; //��һ�� ���ǵ�ǰ����  ʵ���ط�Ч��
      ESP_TaskStatus = ESP_SEND;
      return;
    }
    else //�ط���������
    {
      ATCurrentCmdNum = AT;
      ATNextCmdNum = (teATCmdNum)(ATCurrentCmdNum + 1);
      ESP_TaskStatus = ESP_SEND;
      return;
    }
  }
}

/**
 * �������ܣ� ESP8266 �����״̬��
 * ������
 *      ��
 * ����ֵ��
 *      ��
 */
void ESP_TASK(void)
{
  while (1)
  {
    switch (ESP_TaskStatus)
    {
    case ESP_IDIE: // ����״̬

      SEND_TASK(); // ����״̬
      return;

    case ESP_SEND:                         // ����״̬ ����ESP8266��ATָ��
      if (ATCurrentCmdNum != ATNextCmdNum) // �����ط�״̬
      {
        CurrentRty = ATCmds[ATCurrentCmdNum].RtyNum; //�ط�������ֵ
      }

      ATSend(ATCurrentCmdNum); //��������
      ESP_TaskStatus = ESP_WAIT;
      return;
    case ESP_WAIT: //�ȴ�����״̬ ����ESP8266��ATָ��ص�����

      ATRecv();     // ��������
      Rec_WaitAT(); //����ɹ�������ʧ������
      return;
    default:
      // ����
      return;
    }
  }
}

static uint8_t SendBuff[50] = {0};
static int SendLenth = 0;
/**
 * �������ܣ� ESP8266 ������������
 * ������
 *      ��
 * ����ֵ��
 *      ��
 */
void SEND_TASK(void)
{
  if (ESP_TaskStatus == ESP_IDIE)
  {
    // ÿ��һ�룬�ϱ�һ������
    if (global_Data.SendDataTime_Flag)
    {
      global_Data.SendDataTime_Flag = 0;

      Blink_Leds(LED_SEND_GPIO_Port, LED_SEND_Pin, 50);

      // ��ȡ����������
      get_SensorData();

      // LCD��ʾ����ʾ
      DisplayDeviceData();

      // �������
      memset(SendBuff, 0, sizeof(SendBuff));
     if ((global_Data.DeviceType == DEVICE_TYPE_BEEP) || (global_Data.DeviceType == DEVICE_TYPE_FAN) || (global_Data.DeviceType == DEVICE_TYPE_RELAY) ||
         (global_Data.DeviceType == DEVICE_TYPE_INFRARED) || (global_Data.DeviceType == DEVICE_TYPE_PHOTOACTOR) || (global_Data.DeviceType == DEVICE_TYPE_FLAME) ||
         (global_Data.DeviceType == DEVICE_TYPE_TOUCH))
     {
       SendLenth = packet_data(SendBuff, (uint16_t)global_Data.DeviceAddr, (uint8_t)global_Data.DeviceType, &global_Data.SwitchValue, 1);
     }
     else if ((global_Data.DeviceType == DEVICE_TYPE_GAS) || (global_Data.DeviceType == DEVICE_TYPE_LIGHT))
     {
       uint8_t Buff[2] = {(uint8_t)(global_Data.ADCValue >> 8), (uint8_t)global_Data.ADCValue};
       SendLenth = packet_data(SendBuff, (uint16_t)global_Data.DeviceAddr, (uint8_t)global_Data.DeviceType, Buff, sizeof(Buff));
     }
     else if (global_Data.DeviceType == DEVICE_TYPE_TEMPHUMI)
     {
       uint8_t Buff[2] = {(global_Data.HumiValue), global_Data.TempValue};
       SendLenth = packet_data(SendBuff, (uint16_t)global_Data.DeviceAddr, (uint8_t)global_Data.DeviceType, Buff, sizeof(Buff));
     }
     else if (global_Data.DeviceType == DEVICE_TYPE_ULTRASONIC)
     {
       uint8_t Buff[2] = {(uint8_t)(global_Data.Distance >> 8), (uint8_t)global_Data.Distance};
       SendLenth = packet_data(SendBuff, (uint16_t)global_Data.DeviceAddr, (uint8_t)global_Data.DeviceType, Buff, sizeof(Buff));
     }
     else if (global_Data.DeviceType == DEVICE_TYPE_LAMP)
     {
       uint8_t Buff[3] = {global_Data.RedValue, global_Data.GreenValue, global_Data.BlueValue};
       SendLenth = packet_data(SendBuff, (uint16_t)global_Data.DeviceAddr, (uint8_t)global_Data.DeviceType, Buff, sizeof(Buff));
     }
      // ��������
      if (SendLenth != -1){
        HAL_UART_Transmit(&huart2, SendBuff, SendLenth, 100);

        HAL_UART_Transmit(&huart1, SendBuff, SendLenth, 100);

      }
    }

    if (Usart2type.UsartRecFlag)
    {
      HAL_UART_Transmit(&huart1, Usart2type.UsartRecBuffer, Usart2type.UsartRecLen, 100);

      // �������ݰ�
      int ret = analysis_Packet(Usart2type.UsartRecBuffer, Usart2type.UsartRecLen);
      if (ret != 0)
        printf("result code: %d\r\n", ret);

      Usart2type.UsartRecFlag = 0; //��ս��ձ�־λ
      Usart2type.UsartRecLen = 0;  //��ս��ճ���
      memset(Usart2type.UsartRecBuffer, 0, sizeof(Usart2type.UsartRecBuffer));
    }
  }
}
