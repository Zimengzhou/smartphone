#include "lcd.h"
#include "lcd_init.h"
#include "tools.h"

uint16_t (*lcd_buffer)[240];
__IO uint16_t counter = 0;
__attribute__((section(".bss.SDRAM"))) uint16_t lcd_buffer_1[280][240];
__attribute__((section(".bss.SDRAM"))) uint16_t lcd_buffer_2[280][240];
// __attribute__((section(".RAM_D1"))) uint16_t lcd_buffer[280][240];
__IO uint8_t LCD_DMA_INDEX = 0; // LCD buffer数据分两段发送, to determine which segment to send next
__IO uint8_t LCD_BUFFER_INDEX = 0;// 双缓存,0: lcd_buffer, 1: lcd_buffer_2
__IO uint8_t spi_complete = 0;

extern __IO uint32_t time_stamp_1, time_stamp_3, time_stamp_2, time_stamp_4;
void LCD_Present_Buffer_DMA_IT(void)
{
	if (LCD_DMA_INDEX >= 2)
	{
		LCD_DMA_INDEX = 0;
		LCD_BUFFER_INDEX = (LCD_BUFFER_INDEX == 0) ? 1 : 0; // 切换缓冲区
        counter++;
		Get_TSG_VALUE(time_stamp_1, time_stamp_2);
		time_stamp_3 = time_stamp_1 - time_stamp_4;
		time_stamp_4 = time_stamp_1;
	}
	uint32_t buffer_addr = (LCD_BUFFER_INDEX == 0) ?
		((uint32_t)&lcd_buffer_1[LCD_DMA_INDEX * 140][0]):((uint32_t)&lcd_buffer_2[LCD_DMA_INDEX * 140][0]);
	uint8_t pos_y = LCD_DMA_INDEX * 140;
	LCD_Address_Set(0, pos_y, 239, 139 + pos_y);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, buffer_addr);
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetTxRegAddr(SPI1));
	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 240 * 140);
	LL_SPI_SetTransferSize(SPI1, 240 * 140);
	LL_SPI_Enable(SPI1);
    LL_SPI_EnableIT_EOT(SPI1);
	LL_SPI_StartMasterTransfer(SPI1);
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
    
	LCD_DMA_INDEX++;
}


void LCD_Present_Buffer(void)
{
	SCB_CleanInvalidateDCache();

	for (uint8_t i = 0; i < 2; i++)
	{
		uint8_t pos_y = i * 140;
		LCD_Address_Set(0, pos_y, 239, 139 + pos_y);
		LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, ((uint32_t)&lcd_buffer[i*140][0]));
		LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetTxRegAddr(SPI1));
		LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 240 * 140);

		LL_SPI_SetTransferSize(SPI1, 240 * 140);
		LL_SPI_Enable(SPI1);
		LL_SPI_StartMasterTransfer(SPI1);
        spi_complete = 0;
		LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
		
		// while (LL_SPI_IsActiveFlag_EOT(SPI1) == 0);
		while(spi_complete == 0);
		// LL_SPI_ClearFlag_EOT(SPI1);
		// LL_SPI_ClearFlag_TXTF(SPI1);
		// LL_SPI_Disable(SPI1);
	}
	// for (uint8_t i = 0; i < 2; i++)
	// {
	// 	uint8_t pos_y = i * 120;
	// 	// uint32_t lcd_buffer_addr = (uint32_t)lcd_buffer + i * 280 * 120;
	// 	SCB_CleanInvalidateDCache();
	// 	LCD_Address_Set(0, pos_y, 279, 119 + pos_y);
	// 	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, ((uint32_t)&lcd_buffer[i*120][0]));
	// 	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetTxRegAddr(SPI1));
	// 	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 280 * 120);

	// 	LL_SPI_SetTransferSize(SPI1, 280 * 120);
	// 	LL_SPI_Enable(SPI1);
	// 	LL_SPI_StartMasterTransfer(SPI1);
	// 	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
	// 	while (LL_SPI_IsActiveFlag_EOT(SPI1) == 0);
	// 	LL_SPI_ClearFlag_EOT(SPI1);
	// 	LL_SPI_ClearFlag_TXTF(SPI1);
	// 	LL_SPI_Disable(SPI1);
	// }
    
}

// void LCD_FillRect_(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)//指定区域填充颜色
// {
// 	uint16_t i, j;
// 	uint16_t x_end = x + w, y_end = y + h;
// 	LCD_Address_Set(x, y, x + w - 1, y + h - 1);
// 	for (i = y; i <= y_end; i++)
// 	{
// 		for (j = x; j <= x_end; j++)
// 			LCD_Write16(color);
// 	}
// }

void LCD_FillRect(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)//指定区域填充颜色
{
	uint16_t i, j;
	uint16_t x_end = x + w;
	uint16_t y_end = 240 - y, y_start = 240 - y - h;
	for (i = x; i < x_end; i++)
	{
		for (j = y_start; j < y_end; j++)  lcd_buffer[i][j] = color;
	}
}

void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	lcd_buffer[x][239-y] = color;
}

void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint16_t color)
{
	uint16_t t; 
	int16_t xerr=0,yerr=0,delta_x,delta_y,distance;
	int16_t incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t color)
{
	for (uint16_t x1 = x; x1 < x+w; x1++)  lcd_buffer[y][x1] = color;
	for (uint16_t x1 = x; x1 < x+w; x1++)  lcd_buffer[y+h-1][x1] = color;
	for (uint16_t y1 = y; y1 < y+h; y1++)  lcd_buffer[y1][x] = color;
	for (uint16_t y1 = y; y1 < y+h; y1++)  lcd_buffer[y1][x+w-1] = color;
}

void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

// /******************************************************************************
//       函数说明：显示汉字串
//       入口数据：x,y显示坐标
//                 *s 要显示的汉字串
//                 fc 字的颜色
//                 bc 字的背景色
//                 sizey 字号 可选 16 24 32
//                 mode:  0非叠加模式  1叠加模式
//       返回值：  无
// ******************************************************************************/
// //void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
// //{
// //	while(*s!=0)
// //	{
// //		if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
// //		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
// //		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
// //		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
// //		else return;
// //		s+=2;
// //		x+=sizey;
// //	}
// //}

// ///******************************************************************************
// //      函数说明：显示单个12x12汉字
// //      入口数据：x,y显示坐标
// //                *s 要显示的汉字
// //                fc 字的颜色
// //                bc 字的背景色
// //                sizey 字号
// //                mode:  0非叠加模式  1叠加模式
// //      返回值：  无
// //******************************************************************************/
// //void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
// //{
// //	u8 i,j,m=0;
// //	u16 k;
// //	u16 HZnum;//汉字数目
// //	u16 TypefaceNum;//一个字符所占字节大小
// //	u16 x0=x;
// //	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
// //	                         
// //	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计汉字数目
// //	for(k=0;k<HZnum;k++) 
// //	{
// //		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
// //		{ 	
// //			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
// //			for(i=0;i<TypefaceNum;i++)
// //			{
// //				for(j=0;j<8;j++)
// //				{	
// //					if(!mode)//非叠加方式
// //					{
// //						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
// //						else LCD_WR_DATA(bc);
// //						m++;
// //						if(m%sizey==0)
// //						{
// //							m=0;
// //							break;
// //						}
// //					}
// //					else//叠加方式
// //					{
// //						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
// //						x++;
// //						if((x-x0)==sizey)
// //						{
// //							x=x0;
// //							y++;
// //							break;
// //						}
// //					}
// //				}
// //			}
// //		}				  	
// //		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
// //	}
// //} 

// ///******************************************************************************
// //      函数说明：显示单个16x16汉字
// //      入口数据：x,y显示坐标
// //                *s 要显示的汉字
// //                fc 字的颜色
// //                bc 字的背景色
// //                sizey 字号
// //                mode:  0非叠加模式  1叠加模式
// //      返回值：  无
// //******************************************************************************/
// //void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
// //{
// //	u8 i,j,m=0;
// //	u16 k;
// //	u16 HZnum;//汉字数目
// //	u16 TypefaceNum;//一个字符所占字节大小
// //	u16 x0=x;
// //  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
// //	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计汉字数目
// //	for(k=0;k<HZnum;k++) 
// //	{
// //		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
// //		{ 	
// //			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
// //			for(i=0;i<TypefaceNum;i++)
// //			{
// //				for(j=0;j<8;j++)
// //				{	
// //					if(!mode)//非叠加方式
// //					{
// //						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
// //						else LCD_WR_DATA(bc);
// //						m++;
// //						if(m%sizey==0)
// //						{
// //							m=0;
// //							break;
// //						}
// //					}
// //					else//叠加方式
// //					{
// //						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
// //						x++;
// //						if((x-x0)==sizey)
// //						{
// //							x=x0;
// //							y++;
// //							break;
// //						}
// //					}
// //				}
// //			}
// //		}				  	
// //		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
// //	}
// //} 


// ///******************************************************************************
// //      函数说明：显示单个24x24汉字
// //      入口数据：x,y显示坐标
// //                *s 要显示的汉字
// //                fc 字的颜色
// //                bc 字的背景色
// //                sizey 字号
// //                mode:  0非叠加模式  1叠加模式
// //      返回值：  无
// //******************************************************************************/
// //void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
// //{
// //	u8 i,j,m=0;
// //	u16 k;
// //	u16 HZnum;//汉字数目
// //	u16 TypefaceNum;//一个字符所占字节大小
// //	u16 x0=x;
// //	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
// //	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字数目
// //	for(k=0;k<HZnum;k++) 
// //	{
// //		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
// //		{ 	
// //			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
// //			for(i=0;i<TypefaceNum;i++)
// //			{
// //				for(j=0;j<8;j++)
// //				{	
// //					if(!mode)//非叠加方式
// //					{
// //						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
// //						else LCD_WR_DATA(bc);
// //						m++;
// //						if(m%sizey==0)
// //						{
// //							m=0;
// //							break;
// //						}
// //					}
// //					else//叠加方式
// //					{
// //						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
// //						x++;
// //						if((x-x0)==sizey)
// //						{
// //							x=x0;
// //							y++;
// //							break;
// //						}
// //					}
// //				}
// //			}
// //		}				  	
// //		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
// //	}
// //} 

// ///******************************************************************************
// //      函数说明：显示单个32x32汉字
// //      入口数据：x,y显示坐标
// //                *s 要显示的汉字
// //                fc 字的颜色
// //                bc 字的背景色
// //                sizey 字号
// //                mode:  0非叠加模式  1叠加模式
// //      返回值：  无
// //******************************************************************************/
// //void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
// //{
// //	u8 i,j,m=0;
// //	u16 k;
// //	u16 HZnum;//汉字数目
// //	u16 TypefaceNum;//一个字符所占字节大小
// //	u16 x0=x;
// //	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
// //	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字数目
// //	for(k=0;k<HZnum;k++) 
// //	{
// //		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
// //		{ 	
// //			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
// //			for(i=0;i<TypefaceNum;i++)
// //			{
// //				for(j=0;j<8;j++)
// //				{	
// //					if(!mode)//非叠加方式
// //					{
// //						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
// //						else LCD_WR_DATA(bc);
// //						m++;
// //						if(m%sizey==0)
// //						{
// //							m=0;
// //							break;
// //						}
// //					}
// //					else//叠加方式
// //					{
// //						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
// //						x++;
// //						if((x-x0)==sizey)
// //						{
// //							x=x0;
// //							y++;
// //							break;
// //						}
// //					}
// //				}
// //			}
// //		}				  	
// //		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
// //	}
// //}


// /******************************************************************************
//       函数说明：显示单个字符
//       入口数据：x,y显示坐标
//                 num 要显示的字符
//                 fc 字的颜色
//                 bc 字的背景色
//                 sizey 字号
//                 mode:  0非叠加模式  1叠加模式
//       返回值：  无
// ******************************************************************************/
// void LCD_ShowChar(uint16_t x,uint16_t y,char num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
//  {
//  	uint8_t temp,sizex,t,m=0;
//  	uint16_t i,TypefaceNum;//一个字符所占字节大小
//  	uint16_t x0=x;
//  	sizex=sizey/2;
//  	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
//  	num=num-' ';    //得到偏移后的值
//  	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
//  	for(i=0;i<TypefaceNum;i++)
//  	{ 
//  		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
//  		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
//  		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
//  		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
//  		else return;
//  		for(t=0;t<8;t++)
//  		{
//  			if(!mode)//非叠加模式
//  			{
//  				if(temp&(0x01<<t))LCD_Write16(fc);
//  				else LCD_Write16(bc);
//  				m++;
//  				if(m%sizex==0)
//  				{
//  					m=0;
//  					break;
//  				}
//  			}
//  			else//叠加模式
//  			{
//  				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
//  				x++;
//  				if((x-x0)==sizex)
//  				{
//  					x=x0;
//  					y++;
//  					break;
//  				}
//  			}
//  		}
//  	}   	 	  
//  }

void LCD_DrawChar(uint16_t x, uint16_t y,
				  char character,
				  uint16_t color,
				  uint8_t sizey)
{
	uint8_t temp, sizex, m = 0;
	uint16_t TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	character = character - ' '; // 得到偏移后的值
	for (uint16_t i = 0; i < TypefaceNum; i++)
	{

		if (sizey == 12)
			temp = ascii_1206_[character][i]; // 调用6x12字体
		else if (sizey == 16)
			temp = ascii_1608_[character][i]; // 调用8x16字体
		else if (sizey == 24)
			temp = ascii_2412_[character][i]; // 调用12x24字体
		else if (sizey == 32)
			temp = ascii_3216_[character][i]; // 调用16x32字体
		else
			return;
		for (uint8_t t = 0; t < 8; t++)
		{
			if (temp & (0x01 << t))
				LCD_DrawPoint(x, y, color); // 画一个点
			x++;
			if ((x - x0) == sizex)
			{
				x = x0;
				y++;
				break;
			}
		}
	}
}

// /******************************************************************************
//       函数说明：显示字符串
//       入口数据：x,y显示坐标
//                 *p 要显示的字符串
//                 fc 字的颜色
//                 bc 字的背景色
//                 sizey 字号
//                 mode:  0非叠加模式  1叠加模式
//       返回值：  无
// ******************************************************************************/
//  void LCD_ShowString(uint16_t x,uint16_t y, const char *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
//  {         
//  	while(*p!='\0')
//  	{       
//  		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
//  		x+=sizey/2;
//  		p++;
//  	}  
//  }

void LCD_DrawString(uint16_t x, uint16_t y, const char *p, uint16_t color, uint8_t sizey)
{
	while (*p != '\0')
	{
		LCD_DrawChar(x, y, *p, color, sizey);
		x += sizey / 2;
		p++;
	}
}

// /******************************************************************************
//       函数说明：显示数字
//       入口数据：m底数，n指数
//       返回值：  无
// ******************************************************************************/
// u32 mypow(u8 m,u8 n)
// {
// 	u32 result=1;	 
// 	while(n--)result*=m;
// 	return result;
// }


// /******************************************************************************
//       函数说明：显示整数变量
//       入口数据：x,y显示坐标
//                 num 要显示整数变量
//                 len 要显示的位数
//                 fc 字的颜色
//                 bc 字的背景色
//                 sizey 字号
//       返回值：  无
// ******************************************************************************/
// void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
// {         	
// 	u8 t,temp;
// 	u8 enshow=0;
// 	u8 sizex=sizey/2;
// 	for(t=0;t<len;t++)
// 	{
// 		temp=(num/mypow(10,len-t-1))%10;
// 		if(enshow==0&&t<(len-1))
// 		{
// 			if(temp==0)
// 			{
// 				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
// 				continue;
// 			}else enshow=1; 
		 	 
// 		}
// 	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
// 	}
// } 


// /******************************************************************************
//       函数说明：显示两位小数变量
//       入口数据：x,y显示坐标
//                 num 要显示小数变量
//                 len 要显示的位数
//                 fc 字的颜色
//                 bc 字的背景色
//                 sizey 字号
//       返回值：  无
// ******************************************************************************/
// void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
// {         	
// 	u8 t,temp,sizex;
// 	u16 num1;
// 	sizex=sizey/2;
// 	num1=num*100;
// 	for(t=0;t<len;t++)
// 	{
// 		temp=(num1/mypow(10,len-t-1))%10;
// 		if(t==(len-2))
// 		{
// 			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
// 			t++;
// 			len+=1;
// 		}
// 	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
// 	}
// }


// /******************************************************************************
//       函数说明：显示图片
//       入口数据：x,y起点坐标
//                 length 图片长度
//                 width  图片宽度
//                 pic[]  图片数组    
//       返回值：  无
// ******************************************************************************/
// void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
// {
// 	u16 i,j;
// 	u32 k=0;
// 	LCD_Address_Set(x,y,x+length-1,y+width-1);
// 	for(i=0;i<length;i++)
// 	{
// 		for(j=0;j<width;j++)
// 		{
// 			LCD_Write(pic[k*2]);
// 			LCD_Write(pic[k*2+1]);
// 			k++;
// 		}
// 	}			
// }


