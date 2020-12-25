#ifndef _IWATCH_H
#define _IWATCH_H

#include "Sys.h"
#include "Display.h"
#include "Sensor.h"
#include "GUI_PAGE.h"

#include "PCF8563.h"

#include "intrins.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define	IWATCH_ACTION		0X01
#define	IWATCH_SCREENON	0X02
#define	IWATCH_ACTIVE		0X04
#define	IWATCH_DILE			0X08
#define	IWATCH_GOINGTOSLEEP	0X10
#define	IWATCH_SLEEP		0X20

//设置信息的结构体
typedef struct{
	unsigned char t_inactive_max;				//最大不活跃的时间
	unsigned char wrist_up_wake;				//抬腕唤醒
	unsigned char always_on_disp;				//常亮显示
	unsigned char screen_brightness;		//屏幕亮度
	unsigned char screen_inverse;				//屏幕是否反色
	unsigned char key_sound;						//是否有按键音
	unsigned char alarm_hour;						//闹钟的时间
	unsigned char alarm_min;
	unsigned char alarm_day;
	unsigned char alarm_weekday;
	unsigned char alarm_mode;						//闹钟的模式
	float cal_anglex;										//x轴倾角偏移
	float cal_angley;										//y轴倾角偏移
	int cal_magnet_x0;									//磁力计校正系数
	int cal_magnet_y0;									//磁力计校正系数
	int cal_magnet_z0;									//磁力计校正系数
	float cal_magnet_ab;								//磁力计校正系数
	float cal_magnet_ac;								//磁力计校正系数
	unsigned char history_step[7][11];	//计步器的历史数据，存放过去七天的步数
	unsigned int check_sum;							//所有设置信息的求和结果，用于验证数据是否完整
}iWatch_config;
#define	CONFIG_SIZE	sizeof(iWatch_config)


void iWatchStatusUpdate(void);
void iWatchStatusHandle(void);
unsigned char iWatchGetStatus(void);
void iWatchKeepActive(void);
void iWatchAlwaysOn(void);
void iWatchAutoWakeHandle(void);
void iWatchSleep(void);
void iWatchPowerDown(void);
float iWatchGetBatteryLife(void);
void iWatchSaveConfig(iWatch_config *config);
unsigned char iWatchReadConfig(iWatch_config *config);
void iWatchInit(void);



#endif