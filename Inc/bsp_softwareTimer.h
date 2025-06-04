#ifndef __SOFTWARE_TIMER_H__
#define __SOFTWARE_TIMER_H__

#include <stdint.h>

/*��ʱ�����*/
typedef struct
{
    uint32_t TimeStart;
    uint32_t TimeInter;
} tsTimeType;

void setTime(tsTimeType *timeType, uint32_t timeInter);
uint8_t compareTime(tsTimeType *timeType);

#endif

