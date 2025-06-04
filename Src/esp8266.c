#include "esp8266.h"
#include "usart.h"  // ���ڵ�ͷ�ļ�
#include "bsp_lcd.h"
uint8_t StationIPV4[14] = {0};  //�洢����IPv4

/***********************************************************************************************************
* ��������: void reset_rxbuffClear(void)
* �������: 
*               
* ���ز���: 
*
* ��    ��: --
*
* ��    ��: 2017/2/22, by SongLei
* ��    ��: 1170152143@qq.com
************************************************************************************************************/
void reset_rxbuffClear(void)
{
  Usart2type.UsartRecFlag = 0;//������
  
#if (DEBUG_PRINTF == TRUE) 
  printf("WiFi:%s\n",Usart2type.UsartRecBuffer);
#endif
  
  memset(Usart2type.UsartRecBuffer,0,strlen(Usart2type.UsartRecBuffer));
  
//  NetWorkLedstatusOnOff(NetWorkState_LED4, NetWorkState_LED4_Flag);
//  NetWorkState_LED4_Flag = (~NetWorkState_LED4_Flag);
}
/*
 * ��������cmdAT
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmdData�������͵�ָ��
 *         expReturn1��expReturn2��expReturn3���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
					 timeout �ȴ��ĳ�ʱʱ��  ms
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
_Bool cmdAT(char *cmdData,char *expReturn1,char *expReturn2,char *expReturn3, double timeout)
{
	char cmdDataTmp[40];
	sprintf(cmdDataTmp,"%s\r\n",cmdData);  //���ָ���׺\r\n
	reset_rxbuffClear(); 
	HAL_UART_Transmit(&huart2,(uint8_t *)cmdDataTmp,strlen(cmdDataTmp),0xff);	  //����ATָ��
  
	if(expReturn1==NULL && expReturn2==NULL && expReturn3==NULL)
		return 1;
	LED_3_ON;
	
	HAL_Delay(100);
  LED_3_OFF;
	uint8_t count=0;
	_Bool ok;
  for(count = 0; count < timeout/100; count++)
	{			
			if(Usart2type.UsartRecFlag)    
			{
		//		HAL_UART_Transmit(&huart1, Usart2type.UsartRecBuffer, Usart2type.UsartRecLen, 100);
				Usart2type.UsartRecBuffer[Usart2type.UsartRecLen]='\0';
				Usart2type.UsartRecLen=0;
				
				LED_2_ON;
				HAL_Delay(50);
				// �������У���Ƿ���ȷ
				ok = ((_Bool)strstr((const char *)Usart2type.UsartRecBuffer,expReturn1));
				if(expReturn2!=NULL)  
					ok = ( ok ||((_Bool)strstr((const char *)Usart2type.UsartRecBuffer,expReturn2)));
				if(expReturn3!=NULL)
					ok = ( ok ||((_Bool)strstr((const char *)Usart2type.UsartRecBuffer,expReturn3)));
				
				reset_rxbuffClear();
				if(ok)   // У��Ϊ��
					return ok;
			}
			LED_2_OFF;
					
		HAL_Delay(100);
	}

	return 0;
}
 //����AT���ܺ���
_Bool testAT( void )
{
	char count=0;
 
//	ESP8266_RST_H;	
  printf("\r\n����AT����\r\n");
	HAL_Delay ( 100 );
	while ( count < 5 )
	{
    printf("\r\n��%d�γ�������AT\r\n", count);
		if( cmdAT ( "AT", "OK",NULL, NULL,500) )
    {
      printf("\r\nAT�ɹ�����\r\n");
      return 1;
    }
		__HAL_UART_DISABLE(&huart2);
   __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE); 
		HAL_Delay(500);
		 __HAL_UART_ENABLE(&huart2);
   __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
		HAL_Delay(1000);
		++ count;
	}
  return 0;
}
 //����DHCP��ȡ
_Bool ESP8266_DHCP_CUR (void)
{
	return cmdAT( "AT+CWDHCP_CUR=1,1", "OK",NULL, NULL,500);
}
 
/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return cmdAT ( "AT+CWMODE=1", "OK",NULL, NULL,500); 
		
	  case AP:
		  return cmdAT ( "AT+CWMODE=2", "OK",NULL, NULL,500); 
		
		case STA_AP:
		  return cmdAT ( "AT+CWMODE=3", "OK",NULL, NULL,500); 
		
	  default:
		  return 0;
  }
	
}
 
/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];
 
	sprintf ( cCmd, "AT+CWJAP_DEF=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return cmdAT ( cCmd, "WIFI CONNECTED","WIFI GOT IP","OK",10000);
	
}
 
/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return cmdAT ( cStr, "OK",NULL, NULL,500);
	
}
 
/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];
 
  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s,%s", "UDP", ip, ComNum ,LOCAL_PORT);
		  break;
		
		default:
			break;
  }
 
  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);
 
  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );
 
	return cmdAT ( cCmd, "OK","ALREADY CONNECTED", NULL, 1000);
	
}

/*
 * ��������ESP8266_Read_IP
 * ����  ����ȡIP��ַ
 * ����  ����
 * ����  : 1����ȡ�ɹ�
 *         0����ȡʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_Read_IP(void)
{
	uint8_t cCmd[120] = "AT+CIFSR\r\n";       //��ȡ����IP
	uint8_t i=0;
	char ipv4[40] = {0};
  char mac[40] = {0};
		reset_rxbuffClear(); 
  HAL_UART_Transmit(&huart2, cCmd, strlen((char*)cCmd), 0xffff); // ����ATָ��
  
	HAL_Delay(200);
	Usart2type.UsartRecBuffer[Usart2type.UsartRecLen]='\0';
	Usart2type.UsartRecLen=0;
	//�ӽ��ս����ȡ��IP
	i = sscanf((char*)Usart2type.UsartRecBuffer, "AT+CIFSR\r\n+CIFSR:STAIP,\"%s\"\r\n+CIFSR:STAMAC,\"%s\"\r\n\r\nOK", 
				ipv4, mac);
	if(i > 0)
	{
		memcpy(StationIPV4, ipv4, strlen(ipv4)-1);
		return 1;  // �ɹ�
	}else 
		return 0;  // ʧ��
	
}
/*
 * ��������ESP8266_UnvarnishMode
 * ����  ������WF-ESP8266ģ�����͸��ģʽ  
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_UnvarnishMode ( void )
{
	if ( ! cmdAT ( "AT+CIPMODE=1", "OK", NULL , NULL, 500) )
		return 0;
	return 1;
}
/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�鿪��͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_UnvarnishSend ( void )
{
	return 	  cmdAT ( "AT+CIPSEND", "OK", ">" , "AT+CIPSEND\r\r\n\r\nOK\r\n\r\n>", 5000);
	
}
/*
 * ��������ESP8266_UnvarnishStop
 * ����  ������WF-ESP8266ģ�� �ر�͸������ �ر�͸��ģʽ
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_UnvarnishStop ( void )
{
	if ( ! cmdAT ( "+++", "+++", NULL , NULL, 2000) )  //�ر�͸������
		return 0;
	
	return 	  cmdAT ( "AT+CIPMODE=0", "OK", NULL , NULL, 500);  //�ر�͸��ģʽ
	
}
/*
 * ��������ESP8266_Link_Close
 * ����  ������WF-ESP8266ģ�� �Ͽ�����
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_Link_Close ( void )
{

	return 	  cmdAT ( "AT+CIPCLOSE", "OK", NULL , NULL, 500);  
	
}
/*
 * ��������ESP8266_CWQAP
 * ����  ������WF-ESP8266ģ�� �Ͽ�����AP
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
_Bool ESP8266_CWQAP ( void )
{

	return 	  cmdAT ( "AT+CWQAP", "OK", NULL , NULL, 500);  
	
}
//�ָ���������
_Bool ESP8266_RESTORE ( void )
{

	return 	  cmdAT ( "AT+RESTORE", "OK", NULL , NULL, 500);  
	
}


_Bool initEsp8266(void)
{
//	Gui_DrawFont_GBK16(0,30, BLACK, YELLOW, "INIT WIFI ...");
//	printf("\r\n�˳�͸��ģʽ��\r\n");
//	ESP8266_UnvarnishStop();
	printf("\r\n�ָ��������ã�\r\n");
	ESP8266_RESTORE();
	
	printf("\r\n��ʼ����AT������\r\n");
	if(testAT()==0)
	{
		printf("AT���Ӳ��ϣ�");
		return 0;
	}
//	printf("\r\n�ر�TCP/UDP���ӣ�\r\n");
//	ESP8266_Link_Close(); 
//	printf("\r\n�Ͽ�����AP��\r\n");
//  ESP8266_CWQAP();
	
	
//	printf("\r\n����DHCP��ȡ��\r\n");
//	_Bool dhcpreturn=ESP8266_DHCP_CUR();
//	printf("testdhcpreturn:%d",dhcpreturn);
//	if(dhcpreturn == 0)
//	{
//		printf("����DHCP��ȡʧ�ܣ�\r\n");
//	}
	
	printf("\r\nѡ��WF-ESP8266ģ��Ĺ���ģʽΪSTA��\r\n");
	if(ESP8266_Net_Mode_Choose(STA)==0)
	{
		printf("ѡ��WF-ESP8266ģ��Ĺ���ģʽʧ�ܣ�\r\n");
	}
	
//	Gui_DrawFont_GBK16(0,48, BLACK, YELLOW, "ADD WIFI ...");
	printf("\r\n����WiFi��\r\n");
	if(ESP8266_JoinAP (WIFI_SSID,WIFI_PASSWORD)==0)
	{
			printf("WiFi���Ӳ��ϣ�\r\n");
			return 0;
	}
//		HAL_Delay(2000);
//	Gui_DrawFont_GBK16(0,66, BLACK, YELLOW, "UDP Server ...");
	printf("\r\n�����ⲿ��������\r\n");
	while(ESP8266_Link_Server (enumUDP,SERVER_IP,SERVER_PORT,Single_ID_0)==0)
	{
		printf("���������Ӳ��ϣ�\r\n");
		HAL_Delay(1000);
	}
	
	HAL_Delay(2000);
	printf("\r\nESP8266ģ�����������ӣ�\r\n");
	if(ESP8266_Enable_MultipleId (DISABLE)==0)
	{
		printf("ESP8266ģ������������ʧ�ܣ�\r\n");
		return 0;
	}
	
	HAL_Delay(1000);
	printf("\r\n��ѯ����IP��ַ��\r\n");
	if( ESP8266_Read_IP()==0 )
	{
		printf("��ַ��ȡʧ�ܣ�\r\n");
		HAL_Delay(1000);
	}
//	LED_4_ON; 
	
	printf("\r\n����͸��ģʽ��\r\n");
	if(ESP8266_UnvarnishMode()==0)
	{
		printf("����͸��ģʽʧ�ܣ�\r\n");
		return 0;
	}
	HAL_Delay(2000);
	printf ( "\r\n����͸��......\r\n" );
	if(ESP8266_UnvarnishSend()==0)
	{
		printf("����͸��ģʽʧ�ܣ�\r\n");
		return 0;
	}
	
//	Gui_DrawFont_GBK16(0,84, BLACK, YELLOW, "WIFI OK ...");
	printf( "\r\n���� ESP8266 ���\r\n" );

//	HAL_Delay(1000);
//	Lcd_Clear(YELLOW) ;
	
  return 1;
}
 /*
 * ��������ESP8266_QueryJoinedAP
 * ����  ��WF-ESP8266ģ���ѯ������WiFi��Ϣ���Ա��Ƿ�Ϊָ����WiFi
 * ����  ��pSSID��WiFi�����ַ���
 *      
 * ����  : 1����ǰ����WiFi��ָ������ͬ
 *         0������ͬ
 * ����  �����ⲿ����
 */
/*_Bool ESP8266_QueryJoinedAP( char * pSSID )
{
	uint8_t i = 0;
	uint8_t cCmd [] = "AT+CWJAP_DEF?\r\n";   //���������Ϣ
	char ssid[40] = {0};  // wifi��
	char bssid[40] = {0}; // MAC��ַ
	int channel = 0; // Ƶ��
	int rssi = 0; // �ź�ǿ��
  HAL_UART_Transmit(&huart2, cCmd, strlen((char*)cCmd), 0xffff); // ����ATָ��
	
	HAL_Delay(100);
	UsartType2.usartDMA_rxBuf[UsartType2.Usart_rx_len]='\0';
	UsartType2.Usart_rx_len=0;
	//�ӽ��ս����ȡ����ǰwifi��Ϣ
	i = sscanf((char*)UsartType2.usartDMA_rxBuf, "AT+CWJAP_DEF=\"%s\",\"%s\",\"%d\",\"%d\"\r\n\r\nOK", 
				ssid, bssid, &channel, &rssi);
	if(i > 0)
	{
		if(strcmp(pSSID, ssid) == 0)
			return 1;
	}
	return 0;
}
*/

//��������������������������������
//��Ȩ����������ΪCSDN������ľ��3����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
//ԭ�����ӣ�https://blog.csdn.net/jx5631833/article/details/126371579