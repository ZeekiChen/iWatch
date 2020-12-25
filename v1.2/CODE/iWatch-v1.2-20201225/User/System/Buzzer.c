#include "stc8a.h"
#include "Buzzer.h"

sbit	buzzer = P2^0;

#define	BUZZER_FQ	4000		//蜂鸣器谐振频率4kHz
#define	Focs	24000000		//系统频率24mHz
#define	Fdiv	1						//PWM计数器分频系数
#define	PWM_CNT	(Focs / (BUZZER_FQ * Fdiv))
#define PWM_duty	0.5

bit bee_en = 0;
unsigned char bee_mode = 0;
unsigned int bee_time = 0;


void BuzzerInit(void)
{
	P_SW2 |= 0x80;
	//设置PWM的频率
	PWMCKS = 0x00;				//PWM计数器时钟为系统时钟1分频
	PWMC = PWM_CNT;				//设置PWM计数器
	//使能PWM引脚
	PWM0CR = 0x80;
	//设置PWM占空比
	PWM0T1 = 0x0000;
	PWM0T2 = (unsigned int)(PWM_CNT * PWM_duty);
	//PWMCR = 0x80;		//PWM计数器开始计数
	P_SW2 &= ~0x80;
	
	T4T3M |= 0x20;		//定时器时钟1T模式
	T4L = 0x40;		//设置定时初值
	T4H = 0xA2;		//设置定时初值
	IE2 |= ET4;
	bee_en = 0;
}
void BuzzerOnOff(unsigned char b)
{
	P_SW2 |= 0x80;
	if(b)
	{
		PWMCR |= 0x80;		//PWM计数器开始计数
	}
	else
	{
		PWMCR &= ~0x80;		//PWM计数器停止计数
		buzzer = 0;
	}
	P_SW2 &= ~0x80;
}
void TM4_Isr() interrupt 20 using 1
{
	AUXINTIF &= ~T4IF;   //清中断标志
	if(bee_mode == 0)
	{
		if(++bee_time == 200)
		{
			bee_time = 0;
			BuzzerOnOff(0);
			T4T3M &= ~0x80;
		}
	}
	else if(bee_mode == 1)
	{
		if(bee_time < 600)
		{
			if(bee_time % 100 == 0)
			{
				BuzzerOnOff(((bee_time / 100) % 2) ^ 0x01);
				P27 = ((bee_time / 100) % 2);
			}
		}
		if(++bee_time == 1000)
		{
			bee_time = 0;
		}
	}
}
void Bee(void)
{
	bee_mode = 0;
	bee_time = 0;
	if(bee_en)
	{
		BuzzerOnOff(1);
		T4T3M |= 0x80;
	}
}
void Beebeebee(void)
{
	bee_mode = 1;
	bee_time = 0;
	//BuzzerOnOff(1);
	T4T3M |= 0x80;
}
void EnableBuzzer(unsigned char k)
{
	if(k)
		bee_en = 1;
	else
		bee_en = 0;
}