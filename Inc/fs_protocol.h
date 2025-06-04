#ifndef __FS_PROTOCOL_H__
#define __FS_PROTOCOL_H__

#include "main.h"

// 21Э�� ÿһλ�ĺ���
#define PROTOCOL_HEAD_OFF 0            // ���ݱ�ʶ
#define PROTOCOL_DATA_LEN_OFF 1        // ������-���ݳ���
#define PROTOCOL_DATA_OFFSET_OFF 2     // ������-����ƫ��
#define PROTOCOL_DATA_TYPE_OFF 3       // ��������
#define PROTOCOL_NETWORK_TYPE_OFF 4    // ��������
#define PROTOCOL_SENSOR_ID_H_OFF 5     // �������豸��ַ-��λ
#define PROTOCOL_SENSOR_ID_L_OFF 6     // �������豸��ַ-��λ
#define PROTOCOL_SENSOR_TYPE_OFF 7     // ����������
#define PROTOCOL_BATTERY_PERCENT_OFF 8 // ��ص���
#define PROTOCOL_VALID_DATA_OFF 9      // ������

#define FSP_PACKET_HEAD 0x21                          // ����Э��ͷ
#define FSP_DATA_OFFSET_BYTES PROTOCOL_VALID_DATA_OFF // ����ƫ���ֽ�

#define FSP_UPLOAD_DATA 0x00 // �ϱ�����
#define FSP_CONTROL_CMD 0x01 // �·�����
#define FSP_CRC_BYTES 1      // CRCУ��λ���ݳ���

#define SWITCH_ON '1'  // ��
#define SWITCH_OFF '0' // ��

#define FSP_WIFI 'W'           // ��������
// ���������豸����
typedef enum {
  DEVICE_TYPE_FAN = 0x66, // ����
  DEVICE_TYPE_LIGHT = 0x4C, // ���մ�����
  DEVICE_TYPE_TEMPHUMI = 0x54, // ��ʪ��
  DEVICE_TYPE_GAS = 0x47,  // ��ȼ����

  DEVICE_TYPE_PHOTOACTOR = 0x50, // ���
  DEVICE_TYPE_LAMP = 0x6C,  // ���
  DEVICE_TYPE_RELAY = 0x72,  // �̵���
  DEVICE_TYPE_BEEP = 0x62,  // ������

  DEVICE_TYPE_INFRARED = 0x49,  // ������⴫����
  DEVICE_TYPE_ULTRASONIC = 0x55, // ������������
  DEVICE_TYPE_TOUCH = 0x34, // ���������� ���� 'T' - 0x20
  DEVICE_TYPE_FLAME = 0x26,  // ����  'F' - 0x20

  DEVICE_TYPE_ERR = 0xFF,  // ʶ�����Ĵ�����
} FSP_Device_Type_t;

// ���������豸��ַ
typedef enum {
  DEVICE_ADDR_FAN = 0x402C, // ����
  DEVICE_ADDR_LIGHT = 0x002A, // ���մ�����
  DEVICE_ADDR_TEMPHUMI = 0x002B, // ��ʪ��
  DEVICE_ADDR_GAS = 0x002D,  // ��ȼ����

  DEVICE_ADDR_PHOTOACTOR = 0x002F, // ���
  DEVICE_ADDR_LAMP = 0x402D,  // ���
  DEVICE_ADDR_RELAY = 0x402E,  // �̵���
  DEVICE_ADDR_BEEP = 0x402F,  // ������

  DEVICE_ADDR_INFRARED = 0x0032,  // ������⴫����
  DEVICE_ADDR_ULTRASONIC = 0x0033, // ������������
  DEVICE_ADDR_TOUCH = 0x0034, // ���������� 
  DEVICE_ADDR_FLAME = 0x002E,  // ���� 

} FSP_Device_Addr_t;


/**
 * @brief  �����ݰ�
 * @note
 * @param  PacketBuff: ���ݻ�����
 * @param  SensorAddress: �������豸��ַ
 * @param  ValidData: ��Ч����
 * @param  ValidLen: ��Ч���ݳ���
 * @retval ���ݰ��ĳ���
 */
int packet_data(uint8_t *PacketBuff, uint16_t SensorAddress, uint8_t SensorType, uint8_t *ValidData, uint8_t ValidLen);
/**
 * @brief  �������ݰ�
 * @note
 * @param  PacketBuff: ���ݻ�����
 * @param  PacketLen: ���ݻ������ĳ���
 * @retval None
 */
int analysis_Packet(uint8_t *PacketBuff, uint16_t PacketLen);

#endif
