#include "font.h"
#include "Display.h"

unsigned char xdata main_cache[1024] = {0};	//主显存
unsigned char xdata sub_cache[1024] = {0};	//次显存

extern iWatch_config config;			//设置信息

/**
	* @brief 	根据sys_config结构体的内容初始化显示
	* @param  config：sys_config型结构体的指针
	* @retval 无
	*/
void DisplayInit(void)
{
	OLED_Init();																		//初始化OLED
	ScreenSetBrightness(config.screen_brightness);	//设定屏幕亮度
	ScreenSetInverse(config.screen_inverse);				//设定屏幕是否反色
	ScreenOnOff(ON);																//屏幕开显示
}
/**
	* @brief 	将BMP图片写入指定显存中的指定位置
	* @param 	x：横坐标，y：纵坐标，width：图片的宽度，height：图片的高度，
	*					buf1：BMP图的指针，buf2：显存的指针
	*					k：选择覆盖在显存上还是重叠在显存上
	*						COVER	覆盖
	*						BLEND	叠加
	* @retval 无
	*/
void BMPToCache(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char *buf1, unsigned char *buf2, unsigned char k)
{
	unsigned char data i, j;
	unsigned int data num1, num2, num3;
	num1 = height / 8;
	if(k == 0)				
	{
		for(i = 0; i < num1; i++)
		{
			num2 = (i + y) * 128 + x;
			num3 = i * width;
			for(j = 0; j < width; j++)
				buf2[num2 + j] = buf1[num3 + j];
		}
	}
	else
	{
		for(i = 0; i < num1; i++)
		{
			num2 = (i + y) * 128 + x;
			num3 = i * width;
			for(j = 0; j < width; j++)
				buf2[num2 + j] |= buf1[num3 + j];
		}
	}
	for(i = 0; i < num1; i++)
	{
		num2 = (i + y) * 128 + x;
		num3 = i * width;
		for(j = 0; j < width; j++)
			buf2[num2 + j] = buf1[num3 + j];
	}
}
/**
	* @brief 	将指定显存清零
	* @param  buf：显存的指针
	* @retval 无
	*/
void ClearCache(unsigned char *buf)
{
	unsigned int data i;
	for(i = 0; i < 1024; i++)
		buf[i] = 0x00;
}
/**
	* @brief 	将指定显存的指定区域清零
	* @param  x：起始横坐标，y：起始纵坐标，width：区域的宽度，height：区域的高度，
	*					buf1：显存的指针
	* @retval 无
	*/
void ClearCacheArea(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char *buf1)
{
	unsigned char data i, j;
	unsigned int data num1, num2;
	num1 = height / 8;
	for(i = 0; i < num1; i++)
	{
		num2 = (i + y) * 128 + x;
		for(j = 0; j < width; j++)
		{
			buf1[num2 + j] = 0x00;
		}
	}
}
/**
	* @brief 	将指定显存刷新到OLED屏幕上
	* @param  cache_buf：显存的指针
	* @retval 无
	*/
void ScreenRefreshAll(unsigned char *cache_buf)
{
	OLED_Refresh(cache_buf);
}
/**
	* @brief  将主显存指定位置、指定宽度和指定长度的内容刷新到OLED屏幕上（局部刷新）
	* @param  x：起始横坐标（0~127），y：起始纵坐标（0~7）
	*					width：刷新区域的宽度（0~127），height：刷新区域的高度（0~7）
	* @retval 无
	*/
void ScreenRefreshArea(unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{
	unsigned char data i,j;
	unsigned int data n;	
	for(i = y; i < height; i++)  
	{
		OLED_Set_Pos(x, i);
		n = i * 128;
		for(j = 0; j < width; j++)
			OLED_WR_Byte(main_cache[n + j], OLED_DATA); 
	}
}
/**
	* @brief  实现当前屏幕内容滑动离开主屏显示，次显存的内容紧接进入主屏显示的动态效果
	* @param  cache：次显存的指针（显存的大小：1024bytes）
	*					direction：进入屏幕的方向
	*							UP		向上
	*							DOWN	向下
	*							LEFT	向左
	*							RIGHT	向右
	* @retval 无
	*/
/**
	* @brief 	开启和关闭屏幕显示
	* @param  k：ON		开启
	*						 OFF	关闭
	* @retval 无
	*/
void ScreenOnOff(unsigned char k)
{
	if(k)
		OLED_Display_On();
	else
		OLED_Display_Off();
}
/**
	* @brief 	设置屏幕的亮度
	* @param  screen_brightness：屏幕的亮度值的百分比*100
	* @retval 无
	*/
void ScreenSetBrightness(unsigned char screen_brightness)
{
	if(screen_brightness > 100)	//限幅
		screen_brightness = 100;
	OLED_Set_Brightness(screen_brightness * 255 / 100);
}
/**
	* @brief 	设置屏幕是否反色
	* @param  inverse：
	*						NO_INVERSED	不反色，黑底白字
	*						INVERSED		反色，白底黑字
	* @retval 无
	*/
void ScreenSetInverse(unsigned char k)
{
	OLED_Inverse(k);
}
/**
	* @brief 	在指定显存的指定位置上显示字符串，并能够自动换行
	*					当达到屏幕底部时，整屏向上滚动以继续显示
	* @param  x：字符串起始位置的横坐标（0~127）
	*					y：字符串起始位置的纵坐标（0~7）
	*					str：待显示的字符串的指针
	*					cache_buf：显存的指针
	*					front_size；选择字体，	FONT8X16		大号字体
	*																	FONT6X8			小号字体
	*					inverse：		是否反色，	NO_INVERSED	不反色
	*														 			INVERSED		反色
	*					do_refresh：是刷新屏幕，0						不刷新
	*																	1						刷新
	* @retval 当前字符串所占的行数
	*/
unsigned char ShowString(unsigned char x, unsigned char y, unsigned char *str, unsigned char *cache_buf, unsigned char front_size, unsigned char inverse)
{
	unsigned char data temp = 0;
	unsigned char data str_len = 0;
	unsigned char data line;
	unsigned char data n,i,j;
	unsigned int data num1, num2;
	if(inverse == INVERSED)
		temp = 0xff;
	while(str[str_len] != '\0')
	{
		str_len ++;
	}
	if(front_size == FONT8X16)
	{
		for(n = 0; n < str_len; n++)
		{
			line = y + (n / 16) * 2;
			for(i = 0; i < 2; i++)
			{
				num1 = (line + i) * 128 + x + (n % 16) * 8;
				num2 = i * 8;
				for(j = 0; j < 8; j++)
					cache_buf[num1 + j] = temp ^ F8X16[str[n] - ' '][num2 + j];
			}
		}
	}
	else
	{
		for(n = 0; n <str_len; n++)
		{
			line = y + (n / 21);
			num1 = line * 128 + x + (n % 21) * 6;
			for(j = 0; j < 6; j++)
				cache_buf[num1 + j] = temp ^ F6X8[str[n] - ' '][j];
		}
	}
	if(front_size == FONT8X16)
		return (str_len / 16 + 1) * 2;
	else
		return (str_len / 21 + 1);
}
void DrawDot(unsigned char x, unsigned char y)
{
	main_cache[(y / 8) * 128 + x] |= (0x01 << (y % 8));
}
void DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
	float k;
	float b;
	if(x1 != x2)
	{
		k = ((float)y2 - (float)y1) / ((float)x2 - (float)x1);
		b = y1 - k * x1;
		if(k < 1 && k > -1)
		{
			if(x1 <= x2)
			{
				for(x1; x1 <= x2; x1++)
					DrawDot(x1, k * x1 + b);
			}
			else
			{
				for(x2; x2 <= x1; x2++)
					DrawDot(x2, k * x2 + b);
			}
		}
		else
		{
			if(y1 <= y2)
			{
				for(y1; y1 <= y2; y1++)
					DrawDot((y1 - b) / k, y1);
			}
			else
			{
				for(y2; y2 <= y1; y2++)
					DrawDot((y2 - b) / k, y2);
			}
		}
	}
	else
	{
		if(y1 <= y2)
		{
			for(y1; y1 <= y2; y1++)
				DrawDot(x1, y1);
		}
		else
		{
			for(y2; y2 <= y1; y2++)
				DrawDot(x2, y2);
		}
	}
}
/*
void DrawCircle(unsigned char x, unsigned char y, unsigned char radius)
{
	float x0, y0, k, rad;
	for(rad = 0; rad <= 6.28; rad += 0.02)
	{
		k = tan(rad);
		if((rad >= 0) && (rad < 1.57))
			x0 = (float)radius / sqrt(k * k + 1);
		else if(rad < 3.14)
			x0 = - (float)radius / sqrt(k * k + 1);
		else if(rad < 4.71)
			x0 = - (float)radius / sqrt(k * k + 1);
		else
			x0 = (float)radius / sqrt(k * k + 1);
		y0 = x0 * k;
		DrawDot((x + x0), (y + y0));
	}
}
*/
void DrawArm(unsigned char x, unsigned char y, unsigned char radius, int angle)
{
	float k, rad, x0, y0;
	rad = angle * 0.0174;	//角度转弧度
	k = tan(rad);
	if((rad >= 0) && (rad < 1.57)) //1.57=pi/2
		x0 = (float)radius / sqrt(k * k + 1);
	//else if(rad < 3.14)
	//	x0 = - (float)radius / sqrt(k * k + 1);
	else if(rad < 4.71)
		x0 = - (float)radius / sqrt(k * k + 1);
	else
		x0 = (float)radius / sqrt(k * k + 1);
	y0 = x0 * k;
	DrawLine(x, y, x+x0, y+y0);
}