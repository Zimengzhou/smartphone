#ifndef ___LCD_H
#define ___LCD_H	
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stm32h750xx.h"                 // Device header

extern uint16_t (*lcd_buffer)[240];
extern __attribute__((section(".bss.SDRAM"))) uint16_t lcd_buffer_1[280][240];
extern __attribute__((section(".bss.SDRAM"))) uint16_t lcd_buffer_2[280][240];
// __attribute__((section(".RAM_D1"))) uint16_t lcd_buffer[280][240];
extern __IO uint8_t LCD_DMA_INDEX; // LCD buffer数据分两段发送, to determine which segment to send next
extern __IO uint8_t LCD_BUFFER_INDEX;// 双缓存,0: lcd_buffer, 1: lcd_buffer_2
extern __IO uint8_t spi_complete;
void LCD_Present_Buffer_DMA_IT(void);

void LCD_Present_Buffer(void);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);	// 指定区域填充颜色
// void LCD_FillRect_(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color); // 指定区域填充颜色
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void LCD_DrawChar(uint16_t x, uint16_t y, char character, uint16_t color, uint8_t sizey);
void LCD_DrawString(uint16_t x, uint16_t y, const char *p, uint16_t color, uint8_t sizey);

__STATIC_INLINE void LCD_Clear(uint16_t color)
{
	for (uint16_t i = 0; i <= 239; i++)
	{
		for (uint16_t j = 0; j <= 279; j++)  lcd_buffer[i][j] = color;
	}

}


// void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示汉字串
// void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个12x12汉字
// void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个16x16汉字
// void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个24x24汉字
// void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个32x32汉字

// void LCD_ShowChar(uint16_t x,uint16_t y,char num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示一个字符
// void LCD_ShowString(uint16_t x,uint16_t y,const char *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示字符串
// u32 mypow(u8 m,u8 n);//求幂
// void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//显示整数变量
// void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//显示两位小数变量

// void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

typedef union
{
	uint16_t color;
	struct
	{
		uint16_t b:    5;
		uint16_t g:	   6;
		uint16_t r:    5;
	}color_rgb;

}color_t;

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#ifdef __cplusplus
}
#endif
#endif
