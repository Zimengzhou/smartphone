#include "lcd_init.h"
#include "tools.h"
#include "lcd.h"


//1.47寸屏幕 x: 34 - 205
//           y: 0 - 319  包含端点
// Bitu16(0, LCD__DATA);

/******************************************************************************
      函数说明：LCD串行数据写入函数,模拟SPI
      入口数据：dat  要写入的串行数据
******************************************************************************/
void LCD_GPIO_INIT(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

//void LCD_Write(uint8_t dat)
//{
//	//gpio_bits_reset(GPIOB, GPIO_PINS_7);
//	spi_i2s_data_transmit(SPI2, dat);
//	while (spi_i2s_flag_get(SPI2,SPI_I2S_TDBE_FLAG) == RESET);

//// 	 uint8_t i;
//// 	 for(i=0;i<8;i++)
//// 	 {
//// 	 	SCLK_LOW();
//// 	 	if(dat&0x80)
//// 	 	{
////             LCD_DATA_HIGH();
//// 			// LCD_DATA_HIGH();
////         }
//// 		else
//// 		{
////             LCD_DATA_LOW();
//// 			// LCD_DATA_LOW();
////         }
//// 		SCLK_HIGH();
//// 	
//// 		dat<<=1;
//// 	}
//	//gpio_bits_set(GPIOB, GPIO_PINS_7);

//}

//写8位数据
void LCD_Write8(uint8_t data)
{
	// LCD_Write(data);
    LL_SPI_DisableIT_EOT(SPI1);
	LL_SPI_Disable(SPI1);
	LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
	LL_SPI_SetTransferSize(SPI1, 1);
	LL_SPI_Enable(SPI1);
    // LL_SPI_EnableIT_EOT(SPI1);
	LL_SPI_StartMasterTransfer(SPI1);
	spi_complete = 0;
	// while (LL_SPI_IsActiveFlag_TXP(SPI1) == 0);
	LL_SPI_TransmitData8(SPI1, data);
	while (LL_SPI_IsActiveFlag_EOT(SPI1) == 0);
	// while(spi_complete == 0);
	LL_SPI_ClearFlag_EOT(SPI1);
	LL_SPI_ClearFlag_TXTF(SPI1);
	LL_SPI_Disable(SPI1);
	LL_SPI_Disable(SPI1);
	LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_16BIT);

	// HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	// HAL_SPI_Transmit_DMA(&hspi1, &data, 1);

	// for (uint8_t i = 0; i < 8; i++)
	// {	
	// 	SCLK_LOW();
	// 	if (data & 0x80)
	// 		LCD_DATA_HIGH();
	// 	else
	// 		LCD_DATA_LOW();
	// 	SCLK_HIGH();
	// 	data <<= 1;
	// }

}
 
/******************************************************************************
 * 写16位数据
******************************************************************************/
void LCD_Write16(uint16_t dat)
{

	LCD_Write8(dat>>8);
	LCD_Write8(dat);

	// LL_SPI_SetTransferSize(SPI1, 2);
	// LL_SPI_Enable(SPI1);
	// LL_SPI_StartMasterTransfer(SPI1);
	// while(LL_SPI_IsActiveFlag_TXP(SPI1) == 0);
	// LL_SPI_TransmitData8(SPI1, dat>>8);

	// while(LL_SPI_IsActiveFlag_TXP(SPI1) == 0);
	// LL_SPI_TransmitData8(SPI1, dat);

	// while (!LL_SPI_IsActiveFlag_EOT(SPI1));
	// LL_SPI_ClearFlag_EOT(SPI1);
	// LL_SPI_ClearFlag_TXTF(SPI1);
	// LL_SPI_Disable(SPI1);

	// int8_t size = 2;
	// uint8_t data[2];
	// uint8_t* data_addr = (uint8_t*)data;
	// data[0] = dat >> 8;
	// data[1] = dat;
	// LL_SPI_SetTransferSize(SPI1, size);
	// // LL_SPI_Enable(SPI1);
	// LL_SPI_StartMasterTransfer(SPI1);
	// while (size > 0)
	// {
	// 	if(LL_SPI_IsActiveFlag_TXP(SPI1))
	// 	{
	// 		LL_SPI_TransmitData8(SPI1, *data_addr);
	// 		data_addr++;
	// 		size--;
	// 	}
	// }
	// while (!LL_SPI_IsActiveFlag_EOT(SPI1));
	// LL_SPI_ClearFlag_EOT(SPI1);
	// LL_SPI_ClearFlag_TXTF(SPI1);
	// // LL_SPI_Disable(SPI1);
}


void LCD_CMD(uint8_t dat)
{
	DC_LOW(); //写命令
	LCD_Write8(dat);
	DC_HIGH(); //写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_CMD(0x2a);//列地址设置
	LCD_Write16(x1);
	LCD_Write16(x2);
	LCD_CMD(0x2b);//行地址设置
	LCD_Write16(y1+20);
	LCD_Write16(y2+20);
	LCD_CMD(0x2c);//储存器写

	// LCD_CMD(0x2a);//列地址设置
	// LCD_Write16(x1+20);
	// LCD_Write16(x2+20);
	// LCD_CMD(0x2b);//行地址设置
	// LCD_Write16(y1);
	// LCD_Write16(y2);
	// LCD_CMD(0x2c);//储存器写
}

/******************************************************************************
      函数说明：LCD初始化函数
      入口数据：无
      返回值：  无
******************************************************************************/
void Lcd_Init(void)
{
    // LCD_GPIO_INIT();//Init the GPIOs that LCD will use.
	lcd_buffer = lcd_buffer_1;

	LCD_Reset_LOW();
    
	LL_mDelay(10);
	LCD_Reset_HIGH();
    LL_mDelay(10);
	//************* Start Initial Sequence **********//
	LCD_CMD(0x11); //Sleep out
	LL_mDelay(10);
	
//	LCD_Reset_LOW();
//	delay_ms(100);
//	LCD_Reset_HIGH();
//	delay_ms(100);
//	//************* Start Initial Sequence **********//
//	LCD_CMD(0x11); //Sleep out 
//	delay_ms(100);
//	//************* Start Initial Sequence **********// 
//	LCD_CMD(0x36);
	//************* Start Initial Sequence **********// 
	LCD_CMD(0x36);
	if(USE_HORIZONTAL==0){LCD_Write8(0x00);}
	else if(USE_HORIZONTAL==1){LCD_Write8(0xC0);}
	else if(USE_HORIZONTAL==2){LCD_Write8(0x70);}
	else {LCD_Write8(0xA0);}
	
	LCD_CMD(0x3A);
	LCD_Write8(0x05);

	LCD_CMD(0xB2);			
	LCD_Write8(0x00);
	LCD_Write8(0x00); 
	LCD_Write8(0x00); 
	LCD_Write8(0x33); 
	LCD_Write8(0x33); 			

	LCD_CMD(0xB7);			
	LCD_Write8(0x35);

	LCD_CMD(0xBB);			
	LCD_Write8(0x32); //Vcom=1.35V
					
	LCD_CMD(0xC2);
	LCD_Write8(0x01);

	LCD_CMD(0xC3);			
	LCD_Write8(0x15); //GVDD=4.8V  颜色深度
				
	LCD_CMD(0xC4);			
	LCD_Write8(0x20); //VDV, 0x20:0v

	LCD_CMD(0xC6);			
    //LCD_Write8(0x0F); //0x0F:60Hz        	
	LCD_Write8(0x0F); //0x00:120Hz      0x05 90Hz  	
    //   60 62 64 67 69 72 75 78 82 86 90 94 99 105 111 119  58 57 55 53 52 50 49 48 46 45 44 43 42 41 40 39
	//   0F 0E 0D 0C 0B 0A 09 08 07 06 05 04 03 02  01  00   10 11 12 13 14 15 ...
	LCD_CMD(0xD0);			
	LCD_Write8(0xA4);
	LCD_Write8(0xA1); 

	LCD_CMD(0xE0);
	LCD_Write8(0xD0);   
	LCD_Write8(0x08);   
	LCD_Write8(0x0E);   
	LCD_Write8(0x09);   
	LCD_Write8(0x09);   
	LCD_Write8(0x05);   
	LCD_Write8(0x31);   
	LCD_Write8(0x33);   
	LCD_Write8(0x48);   
	LCD_Write8(0x17);   
	LCD_Write8(0x14);   
	LCD_Write8(0x15);   
	LCD_Write8(0x31);   
	LCD_Write8(0x34);   

	LCD_CMD(0xE1);     
	LCD_Write8(0xD0);   
	LCD_Write8(0x08);   
	LCD_Write8(0x0E);   
	LCD_Write8(0x09);   
	LCD_Write8(0x09);   
	LCD_Write8(0x15);   
	LCD_Write8(0x31);   
	LCD_Write8(0x33);   
	LCD_Write8(0x48);   
	LCD_Write8(0x17);   
	LCD_Write8(0x14);   
	LCD_Write8(0x15);   
	LCD_Write8(0x31);   
	LCD_Write8(0x34);
	LCD_CMD(0x21); 

	LCD_CMD(0x29);
} 

