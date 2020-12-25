#include "iWatch.h"

extern unsigned char xdata main_cache[];
extern unsigned char xdata sub_cache[];

static PageExecuteRate_TypeDef Rate_50hz = {20,0};

extern PageList_TypeDef pagelist[PAGE_MAX];
static unsigned char page_index = PAGE_MENU_MIN + 1;
static char page_shift_direction = 0;

extern pcf8563_time time;
extern float battery_life;					//电池电量
extern unsigned char code BATTERY_LIFE_ICON[];
extern unsigned char battery[24];
static unsigned char str[5] = "  :  ";
static unsigned char n, m;

void IconShiftAnim(char direction);
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	unsigned char n = 0;
	ClearCache(sub_cache);
	sprintf(str, "%02d:%02d", (int)time.hour, (int)time.minute);
	ShowString(0, 0, str, sub_cache, FONT6X8, NO_INVERSED);
	BMPToCache(104, 0, 24, 8, battery, sub_cache, 0);
	BMPToCache(40, 1, 48, 48, pagelist[page_index].Icon, sub_cache, COVER);
	ShowString(64 - strlen(pagelist[page_index].Text) * 3, 7, pagelist[page_index].Text, sub_cache, FONT6X8, NO_INVERSED);
	PageOpenAnim(0);
}
/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	PageCloseAnim(1);
}
/**
  * @brief  页面循环执行的内容
  * @param  无
  * @retval 无
  */
static void Loop()
{
	if(PageExecuteRate(&Rate_50hz) == 1)
	{
		ClearCache(main_cache);
		PCF8563ReadTime(&time);
		sprintf(str, "%02d:%02d", (int)time.hour, (int)time.minute);
		ShowString(0, 0, str, main_cache, FONT6X8, NO_INVERSED);
		//显示电量
		battery_life = iWatchGetBatteryLife();
		for(n = 0; n < 24; n++)
			battery[n] = BATTERY_LIFE_ICON[n];
		m = 18 * battery_life;
		for(n = 2; n < 2 + m; n++)
			battery[n] |= 0x3c;
		BMPToCache(104, 0, 24, 8, battery, main_cache, 0);
		//显示应用图标
		BMPToCache(40, 1, 48, 48, pagelist[page_index].Icon, main_cache, COVER);
		//显示应用名
		ShowString(64 - strlen(pagelist[page_index].Text) * 3, 7, pagelist[page_index].Text, main_cache, FONT6X8, NO_INVERSED);
		ScreenRefreshAll(main_cache);
	}
	if(page_shift_direction != 0)
	{
		if(page_shift_direction == 1)
		{
			if(++page_index == PAGE_MENU_MAX)
				page_index = PAGE_MENU_MIN + 1;
			IconShiftAnim(page_shift_direction);
		}
		else
		{
			if(--page_index == PAGE_MENU_MIN)
				page_index = PAGE_MENU_MAX - 1;
			IconShiftAnim(page_shift_direction);
		}
		page_shift_direction = 0;
	}
}
/**
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(unsigned char event)
{
	if(event == KEY1)
	{
		PageShift(page_watch);
	}
	else if(event == KEY2)
	{
		page_shift_direction = 1;
	}
	else if(event == KEY3)
	{
		page_shift_direction = -1;
	}
	else if(event == DOUBLE_TAP)
		PageShift(page_index);
	else if(event == AWT)
		PageShift(page_watch);
}

void PageRegister_page_menu(unsigned char pageID)
{
	PageRegister(pageID, 0, 0, Setup, Loop, Exit, Event);
}
void IconShiftAnim(char direction)
{
	unsigned char data step;
	unsigned char data i,j;
	unsigned int temp1;
	if(direction == 1)
	{
		for(step = 0; step < 88; step++)
		{
			for(i = 1; i < 7; i++)
			{
				temp1 = i * 128;
				for(j = 0; j < 127; j++)
				{
					main_cache[temp1 + j] = main_cache[temp1 + j + 1];
				}
				if(step < 48)
					main_cache[i * 128 + j] = pagelist[page_index].Icon[(i - 1) * 48 + step];
				else
					main_cache[i * 128 + j] = 0;
			}
			OLED_Refresh(main_cache);
		}
	}
	else if(direction == -1)
	{
		for(step = 0; step < 88; step++)
		{
			for(i = 1; i < 7; i++)
			{
				temp1 = i * 128;
				for(j = 127; j > 0; j--)
				{
					main_cache[temp1 + j] = main_cache[temp1 + j - 1];
				}
				if(step < 48)
					main_cache[i * 128 + j] = pagelist[page_index].Icon[(i - 1) * 48 + (47 - step)];
				else
					main_cache[i * 128 + j] = 0;
			}
			OLED_Refresh(main_cache);
		}
	}
}