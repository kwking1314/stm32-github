/**
  ******************************************************************************
  * File Name          : ultrasonic.c
  * Description        : ���ڳ��������ĳ�ʼ���;���Ĳ�����
  ******************************************************************************
  *

  *
  ******************************************************************************
  */
#include "bsp_ultrasonic.h"
#include "tim.h"
#include "gpio.h"
#include "bsp_delay.h"


// ������ģ�� �����
#define D1_Echo HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

// ������ģ�� ���ƿ�
#define D2_Trig_H HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define D2_Trig_L HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)

void ultrasonicUsing_Init(void)   
{
  MX_GPIO_Init();  

	MX_TIM3_Init();
}    

uint32_t get_ultrasonic_val(void)
{
	uint32_t TIM=0;
	
	D2_Trig_H;       
	delay_us(20);   
	D2_Trig_L;                   
	delay_us(10); 
	
	__HAL_TIM_SET_COUNTER(&htim3,0);

	while(D1_Echo == 0);        //�ȴ�Echo�ز����ű�ߵ�ƽ
	__HAL_TIM_ENABLE(&htim3);  //������ʱ��
	while(D1_Echo == 1);
	__HAL_TIM_DISABLE(&htim3);

	TIM = __HAL_TIM_GET_COUNTER(&htim3);         
    
	return ((TIM * 340)/2) /1000 + 3;   //����1000����λmm��3mm�����
}


