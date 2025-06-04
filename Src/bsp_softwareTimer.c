#include "bsp_softwareTimer.h"
#include "main.h"

/**
 * �������ܣ� ���������ʱ���Ķ�ʱֵ
 * ������
 *     @tsTimeType *TimeType �� �����ʱ���Ľṹ��
 *     @uint32_t TimeInter ��   �����ʱ����ʱ�䣨��λ��ms��
 * ����ֵ��
 *      ��
*/
void setTime(tsTimeType *TimeType, uint32_t TimeInter)
{
    TimeType->TimeStart = HAL_GetTick();    //��ȡϵͳ����ʱ��
    TimeType->TimeInter = TimeInter;      //���ʱ��
}

/**
 * �������ܣ� �Ƚ������ʱ����ֵ
 * ������
 *      @tsTimeType *timeType�� �����ʱ���ṹ��
 * ����ֵ��
 *      ����1������ʱ
*/
uint8_t  compareTime(tsTimeType *TimeType)  //�Ƚ�ʱ��
{
    return ((HAL_GetTick()-TimeType->TimeStart) >= TimeType->TimeInter);  //����1����ʱ
}


