#ifndef _PCF8563_H
#define _PCF8563_H

#define	PCF8563_IIC_ADDR	0xa2

#define	CONTROL_STATUS_1	0x00
#define	CONTROL_STATUS_2	0x01
#define	VL_SECONDS				0x02
#define	MINUTES						0x03
#define	HOURS							0x04
#define	DAYS							0x05
#define	WEEKDAYS					0x06
#define	CENTURY_MOUTHS		0x07
#define	YEARS							0x08
#define	MINUTE_ALARM			0x09
#define	HOUR_ALARM				0x0a
#define	DAY_ALARM					0x0b
#define	WEEKDAY_ALARM			0x0c
#define	CLKOUT_CONTROL		0x0d
#define	TIMER_CONTROL			0x0e
#define	TIMER							0x0f

#define	CLKOUT_32768_HZ	0
#define	CLKOUT_1024_HZ	1
#define	CLKOUT_32_HZ		2
#define	CLKOUT_1_HZ			3

#define	TIMERCLK_4096_HZ	0
#define	TIMERCLK_64_HZ		1
#define	TIMERCLK_1_HZ			2
#define	TIMERCLK_1_60_HZ	3		//(T = 1min)

#define	ENABLE_CLKOUT		1
#define	DISABLE_CLKOUT	0

#define	TIMER_INT	0x04
#define	ALARM_INT	0x08

#define	ALARM_DISABLE				0x00
#define	ALARM_ONCE					0x01
#define	ALARM_EVERYDAY			0x02
#define	ALARM_WORKDAY				0x03
#define	ALARM_SPECIFIC_DAY	0x04

struct pcf8563_time{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char weekday;
	unsigned char day;
	unsigned char month;
	unsigned int year;
	unsigned char century;
};

unsigned char PCF8563ReadByte(unsigned char addr);
void PCF8563ReadTime(struct pcf8563_time *t);
void PCF8563WriteTime(unsigned char hour, unsigned char min, unsigned char sec);
void PCF8563WriteDate(unsigned int year, unsigned char month, unsigned char day, unsigned char weekday);
void PCF8563SetAlarm(unsigned char hour, unsigned char min, unsigned char weekday, unsigned char day, unsigned char mode);
void PCF8563ClearAlarmFlag(void);
void PCF8563DisableAlarm(void);
void PCF8563EnableTimer(unsigned char clock_fq, unsigned char value);
void PCF8563ClearTimerFlag(void);
void PCF8563DisableTimer(void);
unsigned char PCF8563ReadIntSrc(void);
void PCF8563EnableClockOuput(unsigned char clock_fq, unsigned char en);
void PCF8563Init(void);

#endif