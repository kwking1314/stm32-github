#include "gpio.h"
#include "bsp_lcd.h"
#include "bsp_font_lcd.h"

 
//static uint8_t lcd_send_data = 0;



//extern  void HalLcd_HW_WaitUs(uint16 i);  
  
/*
 * Initialize LCD Service
 */
 void HalLcdInit(void);
 
 //LCD Init For 1.44Inch LCD Panel with ST7735R.
void Lcd_Init(void)
{
  HAL_Delay(100);
	Lcd_WriteIndex(0x11);
	HAL_Delay(120);//Sleep exit HAL_Delay(10);

	Lcd_WriteIndex(0x36);
	Lcd_WriteData(0xC8); 

	Lcd_WriteIndex(0x3A);
  Lcd_WriteData(0x05); 
  Lcd_WriteIndex(0x29);//Display on//������		
}
 
 
void Delay_ms(int time)
{
	int i,j;
	for(i=0;i<time*10;i++)
	{
		for(j=0;j<100;j++)
		{
		
		}
	}
}

void SPI_WriteData(uint8_t Data)
{
	unsigned char i;
	for(i=8; i>0; i--)
	{
		if(Data & 0x80)
		{
			LCD_SDA_SET;              //��������ߵ�ƽ
		}
		else
		{
			LCD_SDA_CLR;               //��������͵�ƽ
		}	
			LCD_SCL_SET;              //ʱ�Ӹ�
		  LCD_SCL_CLR;              //ʱ�ӵ�
		  Data  <<=  1;
	}
}

void LCD_WriteData_16Bit(uint16_t Data)
{
	 LCD_CS_CLR;
	 LCD_RS_SET;
	 SPI_WriteData(Data>>8); 	//д���8λ����
	 SPI_WriteData(Data); 			//д���8λ����	
	 LCD_CS_SET;
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(uint8_t Index)
{
   //SPI д����ʱ��ʼ               
	 LCD_CS_CLR;       
	 LCD_RS_CLR;           //LCD_RS_CLR
	 SPI_WriteData(Index);
	 LCD_CS_SET;          
}
//��Һ����дһ��8λ����
void Lcd_WriteData(uint8_t Data)
{
	LCD_CS_CLR;
	LCD_RS_SET;
  SPI_WriteData(Data);
	LCD_CS_SET;
}

void Lcd_WriteReg(uint8_t Index,uint8_t Data)
{
	LCD_CS_CLR;
	Lcd_WriteIndex(Index);
  Lcd_WriteData(Data);
	LCD_CS_SET;
}



////LCD Init For 1.44Inch LCD Panel with ST7735R.
//void Lcd_Init(void)
//{	
//	Lcd_WriteIndex(0x11);//Sleep exit 
//	HAL_Delay(120);
// 
//	Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
//	Lcd_WriteData(0xC8); 
//	
//	Lcd_WriteIndex(0x3A); //65k mode 
//	Lcd_WriteData(0x05); 
//	
//	Lcd_WriteIndex(0x29);//Display on		
//}

/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+3);
	
	Lcd_WriteIndex(0x2c);

}
/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}    

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
//unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y)
//{
//  unsigned int Data;
//  Lcd_SetXY(x,y);
//
//  //Lcd_ReadData();//���������ֽ�
//  //Data=Lcd_ReadData();
//  Lcd_WriteData(Data);
//  return Data;
//}
/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}
//ȡģ��ʽ ˮƽɨ�� ������ ��λ��ǰ
void showimage_farsight(const unsigned char *p) //��ʾ128*35 QQͼƬ
{
  int i; 
	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //����  
	Lcd_SetRegion(0,0,127,34);		//��������
	for(i=0;i<128*35;i++)
 {	
	picL=*(p+i*2);	//���ݵ�λ��ǰ
	picH=*(p+i*2+1);				
	LCD_WriteData_16Bit(picH<<8|picL);  						
 }		
}

void showimage(const unsigned char *p) //��ʾ128*35 QQͼƬ
{
  int i; 
	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //����  
	Lcd_SetRegion(0,0,127,127);		//��������
	for(i=0;i<128*128;i++)
 {	
	picL=*(p+i*2);	//���ݵ�λ��ǰ
	picH=*(p+i*2+1);				
	LCD_WriteData_16Bit(picH<<8|picL);  						
 }		
}


void Gui_DrawFont_GBK16(uint16_t x0, uint16_t y0, uint16_t fc, uint16_t bc, uint8_t *s)
{ 
  int i,j,k,x,y,xx;
  
  unsigned char qm;
  
  long int ulOffset;
  
  char  ywbuf[32];
 // char   temp[2];
  
  for(i = 0; i<strlen((char*)s);i++)
  {
    if(((unsigned char)(*(s+i))) >= 161)
    {
//      temp[0] = *(s+i);
//      temp[1] = '\0';
      return;
    }
    
    else
    {
      qm = *(s+i);
      
      ulOffset = (long int)(qm) * 16;
      
      for (j = 0; j < 16; j ++)
      {
        ywbuf[j]=Zk_ASCII8X16[ulOffset+j];
      }
      
      for(y = 0;y < 16;y++)
      {
        for(x=0;x<8;x++) 
        {
          k=x % 8;
          
          if(ywbuf[y]&(0x80 >> k))
          {
            xx=x0+x+i*8;     
            Gui_DrawPoint(xx,y+y0,fc);
          }
					else
					{
						xx=x0+x+i*8;     
						Gui_DrawPoint(xx,y+y0,bc);
					}  
        }
      }
      
    }
  }  
}

