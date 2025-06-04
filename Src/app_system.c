#include "app_system.h"
#include "adc.h"
#include "gpio.h"
#include "bsp_beep.h"
#include "bsp_lcd.h"
#include "bsp_dht11.h"
#include "bsp_ultrasonic.h"
#include "tim.h"


// ȫ�ֲ�����ʼ��
Global_Data_t global_Data = {.SwitchValue = SWITCH_OFF};

// DMA���ݻ�����
uint32_t ADC_DMA_Value_Buff[ADC_NUM] = {0};

// M0��ȡ���а�D7���ţ����İ�P0_1����ADֵ��ͨ��ADֵ�����жϺ��İ�����
static uint32_t Read_ADC_CoreType(void)
{
  uint32_t adcCoreTypes = 0;
  //��ʼDMAת��ADC
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DMA_Value_Buff, ADC_NUM);

  HAL_Delay(200);

  adcCoreTypes = (uint16_t)ADC_DMA_Value_Buff[ADC_IN5]; //���ͨ��5��ADCֵ

  adcCoreTypes = (adcCoreTypes * 3300) / 4096; //����ADC��ֵ%d mV,

  adcCoreTypes = adcCoreTypes - 70; //��ADֵ��Ƚ�ֵ��ƫ��

  printf("CoreADC:%d mV\n", adcCoreTypes);
  return adcCoreTypes;
}

// �弶��ʼ��
void BSP_Init(void)
{
  uint32_t ADC_CoreType_Val = Read_ADC_CoreType();
  if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_FAN - ADC_ERRORS, DEVICE_ADC_FAN + ADC_ERRORS)) //���� ����AD�ɼ�ֵ 16
  {
    global_Data.DeviceType = DEVICE_TYPE_FAN;
    global_Data.DeviceAddr = DEVICE_ADDR_FAN;
    D1_OUT_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_LIGHT - ADC_ERRORS, DEVICE_ADC_LIGHT + ADC_ERRORS)) //��ǿ ����AD�ɼ�ֵ 65
  {
    global_Data.DeviceType = DEVICE_TYPE_LIGHT;
    global_Data.DeviceAddr = DEVICE_ADDR_LIGHT;
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_TEMPHUMI - ADC_ERRORS, DEVICE_ADC_TEMPHUMI + ADC_ERRORS)) //��ʪ�� ����AD�ɼ�ֵ105
  {
    global_Data.DeviceType = DEVICE_TYPE_TEMPHUMI;
    global_Data.DeviceAddr = DEVICE_ADDR_TEMPHUMI;
    D2_OUT_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_GAS - ADC_ERRORS, DEVICE_ADC_GAS + ADC_ERRORS)) //��ȼ���� ����AD�ɼ�ֵ300
  {
    global_Data.DeviceType = DEVICE_TYPE_GAS;
    global_Data.DeviceAddr = DEVICE_ADDR_GAS;
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_FLAME - ADC_ERRORS, DEVICE_ADC_FLAME + ADC_ERRORS)) //����  ����AD�ɼ�ֵ 430
  {
    global_Data.DeviceType = DEVICE_TYPE_FLAME;
    global_Data.DeviceAddr = DEVICE_ADDR_FLAME;
    D2_IN_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_PHOTOACTOR - ADC_ERRORS, DEVICE_ADC_PHOTOACTOR + ADC_ERRORS)) //��翪�� ����AD�ɼ�ֵ 550
  {
    global_Data.DeviceType = DEVICE_TYPE_PHOTOACTOR;
    global_Data.DeviceAddr = DEVICE_ADDR_PHOTOACTOR;
    D2_IN_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_LAMP - ADC_ERRORS, DEVICE_ADC_LAMP + ADC_ERRORS)) // RGB��ɫ�� ����AD�ɼ�ֵ627
  {
    global_Data.DeviceType = DEVICE_TYPE_LAMP;
    global_Data.DeviceAddr = DEVICE_ADDR_LAMP;
    D1_OUT_GPIO_Init();
    D2_OUT_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_RELAY - ADC_ERRORS, DEVICE_ADC_RELAY + ADC_ERRORS)) //�̵��� ����AD�ɼ�ֵ700
  {
    global_Data.DeviceType = DEVICE_TYPE_RELAY;
    global_Data.DeviceAddr = DEVICE_ADDR_RELAY;
    D1_OUT_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_BEEP - ADC_ERRORS, DEVICE_ADC_BEEP + ADC_ERRORS)) //������ ����AD�ɼ�ֵ818
  {
    global_Data.DeviceType = DEVICE_TYPE_BEEP;
    global_Data.DeviceAddr = DEVICE_ADDR_BEEP;
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_INFRARED - ADC_ERRORS, DEVICE_ADC_INFRARED + ADC_ERRORS)) //������� ����AD�ɼ�ֵ1185
  {
    global_Data.DeviceType = DEVICE_TYPE_INFRARED;
    global_Data.DeviceAddr = DEVICE_ADDR_INFRARED;
    D2_IN_GPIO_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_ULTRASONIC - ADC_ERRORS, DEVICE_ADC_ULTRASONIC + ADC_ERRORS)) //������  ����AD�ɼ�ֵ1336
  {
    global_Data.DeviceType = DEVICE_TYPE_ULTRASONIC;
    global_Data.DeviceAddr = DEVICE_ADDR_ULTRASONIC;
    ultrasonicUsing_Init();
  }
  else if (ADCCOMP(ADC_CoreType_Val, DEVICE_ADC_TOUCH - ADC_ERRORS, DEVICE_ADC_TOUCH + ADC_ERRORS)) //�������� ����AD�ɼ�ֵ1487
  {
    global_Data.DeviceType = DEVICE_TYPE_TOUCH;
    global_Data.DeviceAddr = DEVICE_ADDR_TOUCH;
    D1_IN_GPIO_Init();
  }
  else
  {
    global_Data.DeviceType = DEVICE_TYPE_ERR;
  }
}

// LCD�����ʼ��
void DisplayDeviceLogo(void)
{
  Lcd_Init();
  Lcd_Clear(YELLOW);
  Gui_DrawFont_GBK16(0, 0, RED, CYAN, (uint8_t *)"  WiFi Topology ");
  Gui_DrawFont_GBK16(0, 16, RED, CYAN, (uint8_t *)"     Station    ");
  Gui_DrawFont_GBK16(0, 32, BLACK, YELLOW, (uint8_t *)"SSID:");
  Gui_DrawFont_GBK16(0, 48, BLACK, YELLOW, (uint8_t *)"IP:");

  switch (global_Data.DeviceType)
  {
  case DEVICE_TYPE_FAN:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Fan:");
    break;

  case DEVICE_TYPE_LIGHT:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Light:");
    break;

  case DEVICE_TYPE_TEMPHUMI:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Temp:");
    Gui_DrawFont_GBK16(0, 80, BLACK, YELLOW, (uint8_t *)"Humidity:");
    break;

  case DEVICE_TYPE_GAS:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Gas:");
    break;

  case DEVICE_TYPE_PHOTOACTOR:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Photoactor:");
    break;

  case DEVICE_TYPE_LAMP:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"RED:");
    Gui_DrawFont_GBK16(0, 80, BLACK, YELLOW, (uint8_t *)"GREEN:");
    Gui_DrawFont_GBK16(0, 96, BLACK, YELLOW, (uint8_t *)"BLUE:");
    break;

  case DEVICE_TYPE_RELAY:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Relay:");
    break;

  case DEVICE_TYPE_BEEP:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Beep:");
    break;

  case DEVICE_TYPE_INFRARED:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Infrared:");
    break;

  case DEVICE_TYPE_ULTRASONIC:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Ultrasonic:");
    break;

  case DEVICE_TYPE_TOUCH:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Touch:");
    break;

  case DEVICE_TYPE_FLAME:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"Flame:");
    break;

  default:
    Gui_DrawFont_GBK16(0, 64, BLACK, YELLOW, (uint8_t *)"  No Sensor  ");
    break;
  }
}

// LCD��ʾ����������
void DisplayDeviceData(void)
{
  char DisplayBuff[30] = {0};

  switch (global_Data.DeviceType)
  {
  case DEVICE_TYPE_FAN: // ����
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_LIGHT: // ����
    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%04d", global_Data.ADCValue);
    Gui_DrawFont_GBK16(94, 64, BLACK, YELLOW, (uint8_t *)DisplayBuff);
    break;

  case DEVICE_TYPE_TEMPHUMI: // ��ʪ��
    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%02d", global_Data.TempValue);
    Gui_DrawFont_GBK16(94, 64, BLACK, YELLOW, (uint8_t *)DisplayBuff);

    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%02d", global_Data.HumiValue);
    Gui_DrawFont_GBK16(94, 80, BLACK, YELLOW, (uint8_t *)DisplayBuff);
    break;

  case DEVICE_TYPE_GAS: // ��ȼ��
    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%04d", global_Data.ADCValue);
    Gui_DrawFont_GBK16(94, 64, BLACK, YELLOW, (uint8_t *)DisplayBuff);
    break;

  case DEVICE_TYPE_PHOTOACTOR: // ��翪��
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_LAMP: // RGB��
    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%03d", global_Data.RedValue);
    Gui_DrawFont_GBK16(94, 64, BLACK, YELLOW, (uint8_t *)DisplayBuff);

    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%03d", global_Data.GreenValue);
    Gui_DrawFont_GBK16(94, 80, BLACK, YELLOW, (uint8_t *)DisplayBuff);

    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%03d", global_Data.BlueValue);
    Gui_DrawFont_GBK16(94, 96, BLACK, YELLOW, (uint8_t *)DisplayBuff);
    break;

  case DEVICE_TYPE_RELAY: // �̵���
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_BEEP: // ������
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_INFRARED: // ������⴫����
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_ULTRASONIC: // ������
    memset(DisplayBuff, 0, sizeof(DisplayBuff));
    sprintf(DisplayBuff, "%04d", global_Data.Distance);
    Gui_DrawFont_GBK16(94, 64, BLACK, YELLOW, (uint8_t *)DisplayBuff);
    break;

  case DEVICE_TYPE_TOUCH: // ����
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  case DEVICE_TYPE_FLAME: // ����
    (global_Data.SwitchValue == SWITCH_ON) ? Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"ON ") : Gui_DrawFont_GBK16(102, 64, BLACK, YELLOW, (uint8_t *)"OFF");
    break;

  default:
    break;
  }
}

// ��ȡ������������
void get_SensorData(void)
{
	uint8_t humiH, humiL, tempH, tempL;
  uint32_t Adc_tmp;
  Adc_tmp = ADC_DMA_Value_Buff[ADC_IN0]; // ��ȡ��صĵ�ѹֵ
  Adc_tmp = (Adc_tmp * 3300) / 4096;     //����ADC��ֵ%d mV,
  /*���ǹ涨��ͷŵ��ѹΪ3.51v ,������ѹAD��⵽��Ӧ����2.34v*/
  /*���ǹ涨��ͷŵ��ѹΪ4.20v ,������ѹAD��⵽��Ӧ����2.8v*/
  //����ٷֱ�   �����ʱ����2.8v  ,�ó���ʱ����2.34v, 460������2800��û��2340�Ĳ�
  global_Data.PowerValue = ((Adc_tmp - 2340) * 100) / 460;

  switch (global_Data.DeviceType)
  {
    // ����ģ��
  case DEVICE_TYPE_FLAME:
    global_Data.SwitchValue = (D2_READ_VAL == GPIO_PIN_SET) ? SWITCH_ON : SWITCH_OFF;
    break;

    // �������
  case DEVICE_TYPE_INFRARED:
    global_Data.SwitchValue = (D2_READ_VAL == GPIO_PIN_SET) ? SWITCH_ON : SWITCH_OFF;
    break;

    // ����
  case DEVICE_TYPE_TOUCH:
    global_Data.SwitchValue = (D1_READ_VAL == GPIO_PIN_SET) ? SWITCH_ON : SWITCH_OFF;
    break;

    // ��翪��
  case DEVICE_TYPE_PHOTOACTOR:
    global_Data.SwitchValue = (D2_READ_VAL == GPIO_PIN_SET) ? SWITCH_ON : SWITCH_OFF;
    break;

    // ��ȼ����
  case DEVICE_TYPE_GAS:
    Adc_tmp = ADC_DMA_Value_Buff[ADC_IN4];
    global_Data.ADCValue = (Adc_tmp * 3300) / 4096; //����ADC��ֵ%d mV,
    break;

    // ����
  case DEVICE_TYPE_LIGHT:
    Adc_tmp = ADC_DMA_Value_Buff[ADC_IN4];
    global_Data.ADCValue = (Adc_tmp * 3300) / 4096; //����ADC��ֵ%d mV,
    break;

    // ��ʪ��
  case DEVICE_TYPE_TEMPHUMI:
    if(HAL_OK == DHT11_Read_Data(&humiH, &humiL, &tempH, &tempL))
    {
      global_Data.TempValue = tempH;
      global_Data.HumiValue = humiH;
    }
    break;

    // ������
  case DEVICE_TYPE_ULTRASONIC:
    global_Data.Distance = get_ultrasonic_val() / 10; // ת��Ϊcm
    break;

  default:
    break;
  }
}

// 1S����һ���ж��ź�
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)
  {
    global_Data.SendDataTime_Flag = 1;
    
  }
}





