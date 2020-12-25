#include "Sys.h"
#include "iWatch.h"

extern iWatch_config config;

//不要轻易改动这个函数，否则会影响功耗
void PinInit()
{
	//P0口为OLED屏幕并行数据口
	P0 = 0x00;
	P0M0 = 0x00;
	P0M1 = 0x00;
	//P17为外部拓展引脚
	//P16为外部拓展引脚
	//P14,P15为IIC引脚，外部接了上拉电阻，故这里设置为开漏
	//P13为外部拓展引脚
	//P12为3.3v使能引脚，默认高电平
	//P11为显示屏的RES引脚，默认高电平
	//P10为显示屏的RD引脚，默认高电平
	//P1 = 0x00;
	P1M0 = 0x30;
	P1M1 = 0x30;
	P12 = 1;
	P11 = 1;
	P10 = 1;
	//P27~P21为外部拓展引脚，其中P27为LED1引脚，强推挽，默认高电平
	//P20为蜂鸣器驱动引脚，强推挽，默认低电平
	P2 = 0x80;
	P2M0 = 0x81;
	P2M1 = 0x00;
	//P37为按键2，默认高电平
	//P36为外部中断2
	//P35为外部拓展引脚
	//P34为T0脉冲输入引脚
	//P33,P32为外部中断1和0
	//P30,P31为串口1引脚
	P3 = 0x97;
	P3M0 = 0x00;
	P3M1 = 0x00;
	//P44为KEY1引脚，默认高电平
	//P43为外部拓展引脚
	//P42为外部拓展引脚
	//P41为蓝牙使能引脚，默认高电平
	//P40为外部拓展引脚
	P4 = 0x00;
	P4M0 = 0x00;
	P4M1 = 0x00;
	P41 = 1;
	P44 = 1;
	//P55为按键3，默认高电平
	//P54为充电使能端，高阻为有效
	//P53为显示屏的DC引脚
	//P52为显示屏的WR引脚
	//P51,P50为蓝牙串口
	P5 = 0x00;
	P5M0 = 0x00;
	P5M1 = 0x10;
	P50 = 1;
	P52 = 1;
	P55 = 1;
}
void Timer0Init(void)
{
	TMOD |= 0x04;    //外部计数模式
  TL0 = 0x00;
  TH0 = 0x00;
  TR0 = 0;       	//关闭定时器
  ET0 = 1;       	//使能定时器中断
}
void Timer3Init(void)		//1毫秒@24.000MHz
{
	T4T3M |= 0x02;		//定时器时钟1T模式
	T3L = 0x40;				//设置定时初值
	T3H = 0xA2;				//设置定时初值
	T4T3M |= 0x08;		//定时器3开始计时
	IE2 |= ET3;				//定时器3开溢出中断
}
void UartInit(void)		//115200bps@24.000MHz
{
	SCON = 0x50;
	AUXR |= 0x40;
	AUXR &= 0xFE;
	TMOD &= 0x0F;
	TL1 = 0xCC;
	TH1 = 0xFF;
	ET1 = 0;
	TR1 = 1;
	TI = 1;
}
void Uart2Init(void)		//9600bps@24.000MHz
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0x8F;			//设定定时初值
	T2H = 0xFD;			//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2 |= ES2; 		//使能串口中断
}
void InitConfig(void)	//中断初始化
{
	IT0 = 1;       	//使能INT0下降沿中断，对应PCF8563的中断输出引脚
	EX0 = 1;       	//使能INT0中断
	IT1 = 1;       	//使能INT1下降沿中断，对应LSM6DSM的中断输出引脚1
  EX1 = 1;       	//使能INT1中断
	INTCLKO |= EX2;	//使能INT2中断				对应LSM6DSM的中断输出引脚2
	INTCLKO |= EX3; //使能INT3中断				对应按键2中断
}
void ADCInit(void)
{
	ADCCFG = 0x2f; 		// 设置 ADC时钟为系统时钟 /2/16/16
	ADC_CONTR = 0x8f; // 使能 ADC模块 , 并选择第16通道
}
/**
	* @brief 	采集某一模拟通道的ADC转换值
	* @param  ch：通道号
	* @retval res：转换结果，12位有效
	*/
int ADCRead(unsigned char adc_ch)
{
	int res;
	ADC_CONTR = 0x80 | adc_ch;
	ADC_CONTR |= 0x40; 								//启动 AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20)); 			//查询 ADC完成标志
	ADC_CONTR &= ~0x20; 							//清完成标志
	res = (ADC_RES << 8) | ADC_RESL; 	//读取 ADC结果
	return res;
}
/**
	* @brief 	测量MCU当前电源电压
	* @param 	无
	* @retval 电压值，单位v（伏特）
	*/
#define	BAT_CH	15
int *BGV = (int idata *)0xef;
float GetBatteryVoltage(void)
{
	long res = 0;
	int i = 0;
	int vcc = 0;
	ADCRead(BAT_CH);
	ADCRead(BAT_CH);			//丢弃前两次结果
	for(i = 0; i < 64; i++)
	{
		res += ADCRead(BAT_CH);
	}
	res >>= 6;		//res /= 64;
	vcc = (int)(4095L * *BGV / res);
	return vcc / 1000.0;
}
/**
	* @brief 	3.3V电源使能
	* @param  k：
							0	关闭3.3v电源
							1	打开3.3v电源
	* @retval 无
	*/
void Power3V3En(unsigned char k)
{
	EN_3V3 = k;
}
/**
	* @brief 	充电使能
	* @param  k：
							0	禁止充电
							1	允许充电
	* @retval 无
	*/
void BatChargeEn(unsigned char k)
{
	if(k)
	{
		P1M1 |= 0x80;		//P17设为高阻
	}
	else
	{
		P1M1 &= ~0x80;	//P17设为通用双向
		PROG = 0;				//拉低PROG引脚
	}
}
void MCUSoftReset(void)	//单片机复位
{
	IAP_CONTR = 0x60;			//单片机复位指令
}
void EnableWatchDog(void)
{
	WDT_CONTR = 0x27;			//如果超过4.194s没喂狗，MCU自动复位
}
void FeedWatchDog(void)
{
	WDT_CONTR |= 0x10;		//喂狗，清除看门狗计数器
}
void LED(unsigned char k)
{
	LED1 = k^0x01;
}
/**
	* @brief 	调用该函数使MCU进入睡眠模式
	*					不要轻易改动这个函数，否则会影响功耗
	* @param  无
	* @retval 无
	*/
void SystemSleep(void)
{
	P0 = 0x00;
	//P17为外部拓展引脚
	//P16为外部拓展引脚
	//P14,P15为IIC引脚，外部接了上拉电阻，故这里设置为开漏
	//P13为外部拓展引脚
	//P12为3.3v使能引脚，默认高电平
	//P11为显示屏的RES引脚，默认高电平
	//P10为显示屏的RD引脚，默认高电平
	//P1 = 0x00;
	P1M0 = 0x00;
	P1M1 = 0x00;
	//P27~P21为外部拓展引脚，其中P27为LED1引脚，强推挽，默认高电平
	//P20为蜂鸣器驱动引脚，强推挽，默认低电平
	P2 = 0x80;
	P2M0 = 0x00;
	P2M1 = 0x00;
	//P37为按键2，默认高电平
	//P36为外部中断2
	//P35为外部拓展引脚
	//P34为T0脉冲输入引脚
	//P33,P32为外部中断1和0
	//P30,P31为串口1引脚
	P3 = 0x84;
	//P44为KEY1引脚，默认高电平
	//P43为外部拓展引脚
	//P42为外部拓展引脚
	//P41为蓝牙使能引脚，默认高电平
	//P40为外部拓展引脚
	P4 = 0x02;
	//P55为按键3，默认高电平
	//P54为充电使能端，高阻为有效
	//P53为显示屏的DC引脚
	//P52为显示屏的WR引脚
	//P51,P50为蓝牙串口
	P5 = 0x05;
	
	ET0 = 0;
	IE2 &= ~ES2; 		//关闭串口2中断
	IE2 &= ~ET3;		//定时器3关溢出中断
	PCON = 0x02;		//MCU进入掉电模式
	_nop_();
	_nop_();
}
/**
	* @brief 	调用该函数使系统进入深度掉电模式
	*					不要轻易改动这个函数，否则会影响功耗
	* @param  无
	* @retval 无
	*/
void SystemPowerDown(void)
{
	//P0口为OLED屏幕并行数据口
	P0 = 0x00;
	//P17为外部拓展引脚
	//P16为外部拓展引脚
	//P14,P15为IIC引脚，外部接了上拉电阻，故这里设置为开漏
	//P13为外部拓展引脚
	//P12为3.3v使能引脚，默认高电平
	//P11为显示屏的RES引脚，默认高电平
	//P10为显示屏的RD引脚，默认高电平
	P1 = 0x00;
	//P27~P21为外部拓展引脚，其中P27为LED1引脚，强推挽，默认高电平
	//P20为蜂鸣器驱动引脚，强推挽，默认低电平
	P2 = 0x80;
	P2M0 = 0x00;
	P2M1 = 0x00;
	//P37为按键2，默认高电平
	//P36为外部中断2
	//P35为外部拓展引脚
	//P34为T0脉冲输入引脚
	//P33,P32为外部中断1和0
	//P30,P31为串口1引脚
	P3 = 0x84;
	//P44为KEY1引脚，默认高电平
	//P43为外部拓展引脚
	//P42为外部拓展引脚
	//P41为蓝牙使能引脚，默认高电平
	//P40为外部拓展引脚
	P4 = 0x00;
	//P55为按键3，默认高电平
	//P54为充电使能端，高阻为有效
	//P53为显示屏的DC引脚
	//P52为显示屏的WR引脚
	//P51,P50为蓝牙串口
	P5 = 0x00;
	
	ET0 = 0;
	IE2 &= ~ES2; 		//关闭串口2中断
	IE2 &= ~ET3;		//定时器3关溢出中断
	PCON = 0x02;		//MCU进入掉电模式
	_nop_();
	_nop_();
}
/**
	* @brief 	调用该函数将系统从睡眠模式或者掉电模式中唤醒过来
	*					不要轻易改动这个函数，否则会影响功耗
	* @param  无
	* @retval 无
	*/
void SystemWake(void)
{
	PinInit();
	ET0 = 1;				//开启计数器溢出中断
	IE2 |= ES2; 		//使能串口中断
	IE2 |= ET3;			//定时器3开溢出中断
}
void SysInit(void)
{
	PinInit();		//引脚初始化
	UartInit();		//串口1初始化，使用定时器1
	Uart2Init();	//串口2初始化，使用定时器3
	Timer0Init();	//定时器0初始化，外部计数模式
	Timer3Init();	//定时器3初始化，产生1ms溢出中断
	InitConfig();	//外部中断初始化
	ADCInit();		//ADC初始化，采集电源电压
	IIC_Init();		//硬件IIC初始化
	EnableWatchDog();					//使能看门狗，程序卡死4s后自动复位
	BuzzerInit();							//蜂鸣器初始化，使用硬件PWM0
	EnableBuzzer(config.key_sound);
	LED(OFF);									//关闭LED
	BatChargeEn(1);						//允许电池充电
	Delay1ms(5);
	Power3V3En(1);						//允许3.3v电源输出
	EA = 1;										//开全局中断
}