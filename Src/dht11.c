#include "dht11.h"

#define DHT11_GPIO_PORT 		GPIOB
#define DHT11_GPIO_PIN 			GPIO_PIN_8
#define	DHT11_DQ_IN  		    HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

static uint32_t fac_us = 0; //us延时倍乘数
uint8_t ucharT_data_H=0,ucharT_data_L=0,ucharRH_data_H=0,ucharRH_data_L=0,ucharcheckdata=0;

void delay_init(uint8_t SYSCLK)
{
  fac_us = SYSCLK;
}

void delay_us(uint32_t nus)
{
  uint32_t ticks;
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD; //LOAD的值
  ticks = nus * fac_us;            //需要的节拍数
  told = SysTick->VAL;             //刚进入时的计数器值
  while (1)
  {
    tnow = SysTick->VAL;
    if (tnow != told)
    {
      if (tnow < told)
        tcnt += told - tnow; //这里注意一下SYSTICK是一个递减的计数器就可以了.
      else
        tcnt += reload - tnow + told;
      told = tnow;
      if (tcnt >= ticks)
        break; //时间超过/等于要延迟的时间,则退出.
    }
  };
}
void delay_ms(uint16_t nms)
{
  uint32_t i;
  for (i = 0; i < nms; i++)
    delay_us(1000);
}

static void DHT11_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}

static void DHT11_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}

//复位DHT11
void DHT11_Rst(void)
{
    DHT11_IO_OUT(); 	//SET OUTPUT
    HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET); 	//拉低DQ
    HAL_Delay(20);    	//拉低至少18ms
    HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);		//DQ=1
    delay_us(30);     	//主机拉高20~40us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_Check(void)
{
    uint8_t retry=0;
    DHT11_IO_IN();//SET INPUT
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;
    else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;
    return 0;
}

//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry=0;
    while(DHT11_DQ_IN&&retry<100)//等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!DHT11_DQ_IN&&retry<100)//等待变高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us
    if(DHT11_DQ_IN)return 1;
    else return 0;
}

//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i,dat;
    dat=0;
    for (i=0; i<8; i++)
    {
        dat<<=1;
        dat|=DHT11_Read_Bit();
    }
    return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：HAL_OK,正常;1,读取失败
uint8_t DHT11_Read_Data()
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if(DHT11_Check()==0)
    {
        for(i=0; i<5; i++) //读取40位数据
        {
            buf[i]=DHT11_Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
			ucharRH_data_H=buf[0];
            ucharRH_data_L=buf[1];          
            ucharT_data_H=buf[2];
            ucharT_data_L=buf[3];

        }
    } else
        return HAL_ERROR;

    return HAL_OK;
}


//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在
uint8_t FS_DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);	// 输出高
    DHT11_Rst();  //复位DHT11
    return DHT11_Check();//等待DHT11的回应
}

