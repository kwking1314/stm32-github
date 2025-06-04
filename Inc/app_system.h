#ifndef __APP_SYSTEM_H__
#define __APP_SYSTEM_H__

#include <stdint.h>
#include "fs_protocol.h"

typedef enum ADC_INx
{
  ADC_IN0 = 0, //��ص���
  ADC_IN1,     // ���򰴼�
               //    ADC_IN2,
               //    ADC_IN3,
  ADC_IN4,     //������ADֵ
  ADC_IN5,     //������������
  ADC_NUM      // ADCת���ĸ���
} ADC_INx;


// ������ģ���AD����ֵ mv
typedef enum {
  DEVICE_ADC_FAN = 2100, // ����
  DEVICE_ADC_LIGHT = 61, // ���մ�����
  DEVICE_ADC_TEMPHUMI = 180, // ��ʪ��
  DEVICE_ADC_GAS = 2235,  // ��ȼ����

  DEVICE_ADC_PHOTOACTOR = 420, // ���
  DEVICE_ADC_LAMP = 540,  // ���
  DEVICE_ADC_RELAY = 660,  // �̵���
  DEVICE_ADC_BEEP = 780,  // ������

  DEVICE_ADC_INFRARED = 1140,  // ������⴫����
  DEVICE_ADC_ULTRASONIC = 1260, // ������������
  DEVICE_ADC_TOUCH = 1380, // ���������� ���� 'T' - 0x20
  DEVICE_ADC_FLAME = 300,  // ����  'F' - 0x20

  DEVICE_ADC_ERR = 0xFF,  // ʶ�����Ĵ�����
} FSP_Device_ADC_t;

#define ADC_ERRORS 60
#define ADCCOMP(c, x, y) (((x <= c) && (c <= y)) ? 1 : 0)

// ȫ�ֲ�����ϵͳ����
typedef struct __Global_Data__
{
  FSP_Device_Type_t DeviceType; // ������ģ�������
  FSP_Device_Addr_t DeviceAddr;  // ���������豸��ַ

  uint8_t SwitchValue;  // ����������
  uint8_t RedValue, GreenValue, BlueValue;  // RGB������

  uint8_t TempValue, HumiValue;//  ��ʪ�ȵ���������
  uint8_t PowerValue;  // ���ʣ�����
  uint8_t SendDataTime_Flag;  // ��ʱ���ж�
  
  uint16_t ADCValue;  // AD������
  uint16_t Distance;  // ����������������
} Global_Data_t;
extern Global_Data_t global_Data;

// �弶֧�ְ���ʼ��
void BSP_Init(void);
// ��ʾ�������ʼ��
void DisplayDeviceLogo(void);
// ��ȡ������������
void get_SensorData(void);
// LCD��ʾ����������
void DisplayDeviceData(void);

#endif
