#include "iWatch.h"

extern bit action;
/********************按键相关变量************************/
unsigned char Trg = 0;			//按键触发值
unsigned char Cont = 0;			//按键持续值

unsigned char KeyScan(void)					//按键扫描函数
{
	static unsigned int t_key_press = 0;	//按键持续按下的时间
	unsigned char dat = 0x00;
	if(K1 == 0)
		dat |= KEY1;
	if(K2 == 0)
		dat |= KEY2;
	if(K3 == 0)
		dat |= KEY3;
	Trg = dat&(dat^Cont);
	Cont = dat;
	if(Trg)
	{
		Bee();
		action = 1;
		if((iWatchGetStatus()&IWATCH_ACTIVE) == 0)
			Trg = 0;
	}
	if(Cont)
	{
		if(++t_key_press > (T_KEYLONGPRESS/T_KEYSCAN))	//判断是否长按
		{
			Trg = Cont;																		//重复触发
		}
	}
	else
		t_key_press = 0;
	return Trg;
}
void KeyHandle(void)
{
	if(Trg)
	{
		PageEventTransmit(Trg);
		Trg = 0;
	}
}
void INT2_Isr() interrupt 10 using 1 //双击中断
{
	action = 1;
	Bee();
	if(iWatchGetStatus()&IWATCH_ACTIVE)
	{
		Trg = DOUBLE_TAP;
	}
}
void INT1_Isr() interrupt 2 using 1	//抬腕唤醒中断
{
	action = 1;
	if(iWatchGetStatus()&IWATCH_ACTIVE)
	{
		Trg = AWT;
	}
}
void INT3_Isr() interrupt 11					//按键2中断
{
	action = 1;
}