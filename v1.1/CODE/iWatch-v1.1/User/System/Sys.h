#ifndef __SYS_H
#define __SYS_H

#include "stc8a.h"
#include "Display.h"
#include "PCF8563.h"

/**********************宏定义***************************/
#define	LED1	P20
#define	K1	P27
#define	K2	P37
#define	K3	P55
#define	KEY1				0x01
#define	KEY2				0x02
#define	KEY3				0x04
#define	KEY12				0x03
#define	KEY13				0x05
#define	KEY23				0x06
#define	DOUBLE_TAP	0x08
#define	AWT					0x09
#define	ON	1
#define	OFF	0

#define EE_ADDRESS1	0x0180

//枚举功能号
enum function{
	WATCH = -3,
	MENU,
	SUB_MENU,
	STOPWATCH,
	PEDOMETER,
	THPMETER,
	RADIO,
	COMPASS,
	BLUETOOTH,
	SPIRIT_LEVEL,
	FLASH_LIGHT,
	SNAKES,
	SETTING
};
#define	FIRST_FUNC	STOPWATCH		//第一个功能
#define	LAST_FUNC		SETTING			//最后一个功能
#define	FUNC_NUM_MAX	10					//功能数量
//设置信息的结构体
struct sys_config{
	unsigned char t_inactive_max;				//自动息屏时间
	unsigned char t_sleep_max;					//自动待机时间
	unsigned char screen_brightness;		//屏幕亮度
	unsigned char screen_inverse;				//屏幕是否反色
	unsigned char screen_direction;			//屏幕的显示方向
	unsigned char key_sound;						//是否有按键音
	unsigned char alarm_hour;						//闹钟的时间
	unsigned char alarm_min;
	unsigned char alarm_day;
	unsigned char alarm_weekday;
	unsigned char alarm_mode;						//闹钟的模式
	unsigned char radio_volume;					//收音机音量
	float	radio_channel;								//收音机频道
	float cal_anglex;										//x轴倾角偏移
	float cal_angley;										//y轴倾角偏移
	int cal_magnet_x0;									//磁力计校正系数
	int cal_magnet_y0;									//磁力计校正系数
	int cal_magnet_z0;									//磁力计校正系数
	float cal_magnet_ab;								//磁力计校正系数
	float cal_magnet_ac;								//磁力计校正系数
	unsigned char history_step[7][11];	//计步器的历史数据，存放过去七天的步数
	unsigned int check_sum;							//所有设置信息的求和结果，用于验证数据是否完整
};
#define	CONFIG_SIZE	sizeof(struct sys_config)

void PinInit(void);
void MCUSoftReset(void);
void FeedWatchDog(void);
void UART1SendString(char *str);
void StartFrameReceive();
unsigned char CheckFrameReceived();
void LED(unsigned char k);
unsigned char EEPROMReadConfiguration(struct sys_config *config);
void EEPROMWriteConfiguration(struct sys_config *config);
void SysInit(void);

#endif