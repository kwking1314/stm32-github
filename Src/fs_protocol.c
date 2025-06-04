#include "fs_crc.h"
#include "fs_protocol.h"
#include <stdio.h>
#include "app_system.h"
#include "bsp_beep.h"
#include "bsp_P9813.h"

// ����ͽ��

/**
 * @brief  �����ݰ�
 * @note
 * @param  PacketBuff: ���ݻ�����
 * @param  SensorAddress: �������豸��ַ
 * @param  ValidData: ��Ч����
 * @param  ValidLen: ��Ч���ݳ���
 * @retval ���ݰ��ĳ���
 */
int packet_data(uint8_t *PacketBuff, uint16_t SensorAddress, uint8_t SensorType, uint8_t *ValidData, uint8_t ValidLen)
{
  uint8_t i = 0;

  if (PacketBuff == NULL && ValidData == NULL)
  {
    return -1;
  }

  PacketBuff[PROTOCOL_HEAD_OFF] = FSP_PACKET_HEAD;
  PacketBuff[PROTOCOL_DATA_LEN_OFF] = ValidLen;
  PacketBuff[PROTOCOL_DATA_OFFSET_OFF] = FSP_DATA_OFFSET_BYTES;
  PacketBuff[PROTOCOL_DATA_TYPE_OFF] = FSP_UPLOAD_DATA;
  PacketBuff[PROTOCOL_NETWORK_TYPE_OFF] = FSP_WIFI;
  PacketBuff[PROTOCOL_SENSOR_ID_H_OFF] = (uint8_t)(SensorAddress >> 8);
  PacketBuff[PROTOCOL_SENSOR_ID_L_OFF] = (uint8_t)SensorAddress;
  PacketBuff[PROTOCOL_SENSOR_TYPE_OFF] = SensorType;
  PacketBuff[PROTOCOL_BATTERY_PERCENT_OFF] = global_Data.PowerValue;

  //���������
  for (i = 0; i < ValidLen; i++)
  {
    PacketBuff[PROTOCOL_VALID_DATA_OFF + i] = *(ValidData + i);
  }
  // crc
  PacketBuff[ValidLen + FSP_DATA_OFFSET_BYTES] = crc8(PacketBuff, ValidLen + FSP_DATA_OFFSET_BYTES);
  return FSP_DATA_OFFSET_BYTES + ValidLen + FSP_CRC_BYTES;
}

/**
 * @brief  �������ݰ�
 * @note
 * @param  PacketBuff: ���ݻ�����
 * @param  PacketLen: ���ݻ������ĳ���
 * @retval None
 */
int analysis_Packet(uint8_t *PacketBuff, uint16_t PacketLen)
{
  if (PacketBuff == NULL)
    return -1;

  // �ж�����ͷ
  if (PacketBuff[PROTOCOL_HEAD_OFF] != FSP_PACKET_HEAD)
    return -2;

  // �ж���������
  if (PacketBuff[PROTOCOL_NETWORK_TYPE_OFF] != FSP_WIFI)
    return -3;

  // �ж��豸��ַ - ��λ
  if (PacketBuff[PROTOCOL_SENSOR_ID_H_OFF] != (uint8_t)(global_Data.DeviceAddr >> 8))
    return -4;

  // �ж��豸��ַ - ��λ
  if (PacketBuff[PROTOCOL_SENSOR_ID_L_OFF] != (uint8_t)global_Data.DeviceAddr)
    return -5;

  // �жϴ���������
  if (PacketBuff[PROTOCOL_SENSOR_TYPE_OFF] != global_Data.DeviceType)
    return -6;

  // ���CRCУ��
  if (PacketBuff[PacketLen - 1] != crc8(PacketBuff,
                                        PacketBuff[PROTOCOL_DATA_OFFSET_OFF] + PacketBuff[PROTOCOL_DATA_LEN_OFF]))
    return -7;
  else
  {
    printf("21 Protocol CRC8 is OK.\r\n");
    if (PacketBuff[PROTOCOL_DATA_TYPE_OFF] == FSP_CONTROL_CMD)
    {
      switch (PacketBuff[PROTOCOL_SENSOR_TYPE_OFF])
      {
      case DEVICE_TYPE_RELAY:
        if (SWITCH_ON == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_ON;
          RELAY_ON;
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
          printf("===== RELAY_ON =====\r\n");
        }
        else if (SWITCH_OFF == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_OFF;
          RELAY_OFF;
          printf("===== RELAY_OFF =====\r\n");
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
        }
        break;

      case DEVICE_TYPE_FAN:
        if (SWITCH_ON == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_ON;
          FAN_ON;
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
          printf("===== FAN_ON =====\r\n");
        }
        else if (SWITCH_OFF == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_OFF;
          FAN_OFF;
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
          printf("===== FAN_OFF =====\r\n");
        }
        break;

      case DEVICE_TYPE_BEEP:
        if (SWITCH_ON == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_ON;
          set_Beep_Status(1);
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
          printf("===== BEEP_ON =====\r\n");
        }
        else if (SWITCH_OFF == PacketBuff[PROTOCOL_VALID_DATA_OFF])
        {
          global_Data.SwitchValue = SWITCH_OFF;
          set_Beep_Status(0);
					Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
          printf("===== BEEP_OFF =====\r\n");
        }
        break;

      case DEVICE_TYPE_LAMP:
        global_Data.RedValue = PacketBuff[PROTOCOL_VALID_DATA_OFF];       // �������
        global_Data.GreenValue = PacketBuff[PROTOCOL_VALID_DATA_OFF + 1]; // �̵�����
        global_Data.BlueValue = PacketBuff[PROTOCOL_VALID_DATA_OFF + 2];  // ��������
				Blink_Leds(LED_RECV_GPIO_Port, LED_RECV_Pin, 100);
        printf("===== Set RGB =====\r\n");

        rgb_setValue(global_Data.RedValue, global_Data.GreenValue, global_Data.BlueValue);
        break;

      default:
        break;
      }
    }
  }

  return 0;
}
