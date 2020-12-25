#include "iWatch.h"

extern unsigned char xdata main_cache[];
extern unsigned char xdata sub_cache[];

static PageExecuteRate_TypeDef Rate_50hz = {20, 0};

static unsigned char code ARROW_ICON[24] = {//箭头图标
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x07,0x0E,0x1C,0x38,
	0x70,0xFF,0xFF,0x70,0x38,0x1C,0x0E,0x07
};
#define	ARROW_ICON_W	12
#define	ARROW_ICON_H	16
typedef struct {
	unsigned char x;
	unsigned char y;
}ArrowCorrdinate_TypeDef;
static code ArrowCorrdinate_TypeDef ArrowCorrdinate[7] = {
	{34,0},{58,0},{82,0},{10,4},{42,4},{64,4},{98,4}
};
typedef struct {
	ArrowCorrdinate_TypeDef *ac;				//被设置的值的指针图标的坐标
	unsigned char *value;							//被设置的值的指针
	unsigned char value_max;					//设置的数值最大值+1
	unsigned char value_min;					//设置的数值最小值-1
}Setting_TypeDef;
static Setting_TypeDef setting_items[7] = {
	{&ArrowCorrdinate[0], 0, 24, 255},
	{&ArrowCorrdinate[1], 0, 60, 255},
	{&ArrowCorrdinate[2], 0, 60, 255},
	{&ArrowCorrdinate[3], 0, 100,255},
	{&ArrowCorrdinate[4], 0, 13, 0  },
	{&ArrowCorrdinate[5], 0, 32, 0  },
	{&ArrowCorrdinate[6], 0, 7,  255}
};
#define	MAX_SETTING_INDEX	7
static unsigned char setting_index = 0;
unsigned char str[16] = {0};
extern pcf8563_time time;		//时间信息结构体
extern unsigned char code WEEKDAY_IN_STR[7][6];
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	setting_items[0].value = &time.hour;
	setting_items[1].value = &time.minute;
	setting_items[2].value = &time.second;
	setting_items[3].value = &time.year;
	setting_items[4].value = &time.month;
	setting_items[5].value = &time.day;
	setting_items[6].value = &time.weekday;
	ClearCache(sub_cache);
	BMPToCache(setting_items[setting_index].ac->x, setting_items[setting_index].ac->y, ARROW_ICON_W, ARROW_ICON_H, ARROW_ICON, sub_cache, COVER);
	sprintf(str, "%02d:%02d:%02d", (int)time.hour, (int)time.minute, (int)time.second);
	ShowString(32, 2, str, sub_cache, FONT8X16, NO_INVERSED);
	sprintf(str, "%d/%2d/%2d", (int)time.year + 2000, (int)time.month, (int)time.day);
	ShowString(0, 6, str, sub_cache, FONT8X16, NO_INVERSED);
	ShowString(88, 6, WEEKDAY_IN_STR[time.weekday - 1], sub_cache, FONT8X16, NO_INVERSED);
	PageOpenAnim(ANIM_RIGHT);
}
/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	PCF8563WriteTime(&time);
	PageCloseAnim(ANIM_RIGHT);
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
		BMPToCache(setting_items[setting_index].ac->x, setting_items[setting_index].ac->y, ARROW_ICON_W, ARROW_ICON_H, ARROW_ICON, main_cache, COVER);
		sprintf(str, "%02d:%02d:%02d", (int)time.hour, (int)time.minute, (int)time.second);
		ShowString(32, 2, str, main_cache, FONT8X16, NO_INVERSED);
		sprintf(str, "%d/%2d/%2d", (int)time.year + 2000, (int)time.month, (int)time.day);
		ShowString(0, 6, str, main_cache, FONT8X16, NO_INVERSED);
		ShowString(88, 6, WEEKDAY_IN_STR[time.weekday - 1], main_cache, FONT8X16, NO_INVERSED);
		ScreenRefreshAll(main_cache);
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
		PageShift(page_setting);
	else if(event == DOUBLE_TAP)
	{
		if(++setting_index == MAX_SETTING_INDEX)
			setting_index = 0;
	}
	else if(event == KEY2)
	{
		if(++*setting_items[setting_index].value == setting_items[setting_index].value_max)
			*setting_items[setting_index].value = setting_items[setting_index].value_min + 1;
	}
	else if(event == KEY3)
	{
		if(--*setting_items[setting_index].value == setting_items[setting_index].value_min)
			*setting_items[setting_index].value = setting_items[setting_index].value_max - 1;
	}
}

void PageRegister_page_settime(unsigned char pageID)
{
	PageRegister(pageID, 0, 0, Setup, Loop, Exit, Event);
}