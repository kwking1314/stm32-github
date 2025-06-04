#include "bsp_P9813.h"
#include "bsp_delay.h"




#define RGB_DATA_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define RGB_DATA_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)

#define RGB_CLK_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define RGB_CLK_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)

void rgb_Gpio_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
uint32_t Color_Data(uint8_t Red, uint8_t Green, uint8_t Blue) //д��������λ��ֵ������һ��32λ����ֵ
{
  uint32_t temp = 0;

  temp |= (0x03 << 30);           // 0xc0--> 1100 0000     1010 1010
  temp |= ((~Green) & 0xc0) << 22; //ȡ��ɫ�����λȡ��,�õ�B7�� B6��
  temp |= ((~Blue) & 0xc0) << 20; //ȡ��ɫ�����λȡ��,�õ�G7��G6��
  temp |= ((~Red) & 0xc0) << 18; //ȡ��ɫ�����λȡ��,�õ�R7��R6��
  temp |= Green << 16;
  temp |= Blue << 8;
  temp |= Red << 0;

  return temp;
}

void RGB_Send_Data(uint32_t data)
{
  unsigned int i;
  for (i = 0; i < 32; i++)
  {
    //ȡdata�����λ�������1��д1��ע�ⰴλ������д16���ƣ�
    if (data & 0x80000000) //׼��һλ����
      RGB_DATA_H;
    else
      RGB_DATA_L;            //����д0

    data <<= 1; //ÿȡ��һ�����ݣ��ôθ�λ������λ����������32λ��dataֵ
    RGB_CLK_L;
    delay_us(10);
    RGB_CLK_H;
    delay_us(10);
    /*�����Ĳ�������CLK����������*/
  }
}

void rgb_setValue(uint8_t Red, uint8_t Green, uint8_t Blue)
{
  uint32_t color_data;
  color_data = Color_Data(Red, Green, Blue);

  RGB_Send_Data(0);          //�ȷ���32λ�͵�ƽ��ʼ�ź�
  RGB_Send_Data(color_data); //���͵�һ��32λ�Ҷ�����
  RGB_Send_Data(color_data); //���͵ڶ���32λ�Ҷ�����
}
