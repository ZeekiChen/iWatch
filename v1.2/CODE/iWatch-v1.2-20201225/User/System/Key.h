#ifndef __KEY_H
#define __KEY_H

/**********************KEY*************************/
#define	K1	P44
#define	K2	P37
#define	K3	P55
#define	KEY1				0x01
#define	KEY2				0x02
#define	KEY3				0x04
#define	KEY12				0x03
#define	KEY13				0x05
#define	KEY23				0x06
#define	KEY123			0x07
#define	DOUBLE_TAP	0x08
#define	AWT					0x09
#define	T_KEYSCAN				20		//按键扫描周期，20ms
#define	T_KEYLONGPRESS	500		//按键长按识别时间，500ms	

unsigned char KeyScan(void);
void KeyHandle(void);

#endif