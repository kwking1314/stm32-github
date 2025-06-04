#ifndef __ESP_WIFI_H__
#define __ESP_WIFI_H__

#include "main.h"

//ATָ��״̬��
typedef enum
{
    ESP_IDIE = 0,
    ESP_SEND,
    ESP_WAIT,
    ESP_ACCESS
} teESP_TaskStatus;
//ATָ��ö�٣���ǰ��ATָ���Ӧ
typedef enum
{
	AT,
	AT_CWMODE,
  AT_CWJAP,
	AT_CIFSR,
	AT_CIPSTART,
	AT_CIPMUX,
	AT_CIPMODE_1,
	AT_CIPSEND,
	AT_QUITSEND,
	AT_CIPMODE_0,
  AT_CIPCLOSE,
} teATCmdNum;
// ATָ���״̬
typedef enum
{
    SUCCESS_REC = 0,    // ���ճɹ�
    TIME_OUT,           // ���ճ�ʱ
    NO_REC              // �޷���
} teATStatus;
//ATָ��ṹ��  5���ֶ�
typedef struct
{
    char *ATSendStr;
    char *ATRecStr;
    double TimeOut;       //�ȴ���ʱʱ�� ����λ��ms��
    teATStatus ATStatus;    //ָ��״̬
    uint8_t RtyNum;         //�ط�����
} tsATCmds;


extern teESP_TaskStatus ESP_TaskStatus; // ģ������״̬

void ESP8266_Init(void);
void ESP_TASK(void);
void SEND_TASK(void);


#endif



