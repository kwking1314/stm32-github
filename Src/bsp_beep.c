#include "bsp_beep.h"
#include "tim.h"

/**
 * ���÷�������״̬
 * 1:����������   0:�رշ�����
 */
void set_Beep_Status(uint8_t status)
{
	status ? HAL_TIMEx_PWMN_Start(&htim17, TIM_CHANNEL_1) : HAL_TIMEx_PWMN_Stop(&htim17, TIM_CHANNEL_1) ;
}

