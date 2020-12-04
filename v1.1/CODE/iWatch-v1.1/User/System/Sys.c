#include "Sys.h"
#include "IIC.h"
#include "Delay.h"
#include "PowerManage.h"
#include "Buzzer.h"
#include "EEPROM.h"

//若EEPROM中没有数据或者数据不完整，则加载以下默认设置
const struct sys_config default_config = {
	//默认显示参数
	10,						//自动息屏时间
	5,						//自动待机时间
	50,						//屏幕亮度（0~255）
	NO_INVERSED,	//屏幕是否反色
	NORMAL,				//屏幕的显示方向
	OFF,					//关闭按键音
	//默认闹钟参数
	8,							//小时
	30,							//分钟
	1,							//1号
	1,							//周一
	ALARM_DISABLE,	//闹钟关闭
	//默认收音机参数
	1,			//收音机音量
	97.1,		//收音机频道
	//默认校正参数
	0,			//x轴倾角偏移
	0,			//y轴倾角偏移
	0,			//磁力计校正系数
	0,			//磁力计校正系数
	0,			//磁力计校正系数
	1,			//磁力计校正系数
	1,			//磁力计校正系数
	{{0}, {0}, {0}, {0}, {0}, {0}, {0}},	//计步器的历史数据，存放过去七天的步数
	0				//检验用求和项，不要修改
};

extern struct sys_config config;

//不要轻易改动这个函数，否则会影响功耗
void PinInit()
{
	//P0口为OLED屏幕并行数据口
	P0 = 0x00;
	P0M0 = 0x00;
	P0M1 = 0x00;
	//P10,P11为串口2引脚
	//P13,P12为显示屏RES和D/C引脚
	//P14,P15为IIC引脚，外部接了上拉电阻，故这里设置为开漏
	//P16为蓝牙使能引脚，高电平时进入低功耗
	//P17为充电使能引脚，高阻时使能充电
	P13 = 1;
	P16 = 1;
	P1M0 = 0x30;//00110000b
	P1M1 = 0xb0;//10110000b
	//P20为LED驱动引脚，强推挽模式
	//P21,P22,P23为外部拓展引脚
	//P24,P25为IIC2引脚，外部无上拉电阻，故这里设置为通用双向
	//P26为蜂鸣器驱动引脚，强推挽模式
	//P27为按键1，默认高电平
	P2 = 0xb1;
	P2M0 = 0x41;
	P2M1 = 0x00;
	//P37为按键2，外部中断3，默认高电平
	//P36为外部中断2
	//P34为T0脉冲输入引脚
	//P33为外部中断1
	//P32为外部中断0
	//P30,P31为串口1引脚
	P3 = 0x94;
	P3M0 = 0x00;
	P3M1 = 0x00;
	//P44,P43为显示屏E/RD和R/W引脚
	//P40为3.3v使能输出引脚
	P4 = 0x19;
	P4M0 = 0x00;
	P4M1 = 0x00;
	//P55为按键3，默认高电平
	P5 = 0x20;
	P5M0 = 0x00;
	P5M1 = 0x00;
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
#ifdef	badapple
extern unsigned char xdata sub_cache2[];
unsigned int rx_buf_num = 0;
bit frame_received_flag = 0;
bit serial_busy = 0;
void UartInit(void)		//576000bps@24.000MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xF6;		//??????
	TH1 = 0xFF;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
	ES = 1;					//开串口中断
	//TI = 1;
}
void UART1_Isr() interrupt 4 using 1
{
	if(RI)
	{
		RI = 0;      				//清中断标志
		if(SBUF == MCU_RESET_CMD)
			MCUSoftReset();		//软复位MCU
	}
	if(RI)
	{
		RI = 0;
		sub_cache2[rx_buf_num++] = SBUF;
		if(rx_buf_num == 1024)
		{
			rx_buf_num = 0;
			frame_received_flag = 1;
		}
	}
	if(TI)
	{
		TI = 0;
		serial_busy = 0;
	}
}
void UART1SendString(char *str)
{
	while(*str)
  {
		while(serial_busy);
		serial_busy = 1;
		SBUF = *str++;
  }
}
void StartFrameReceive()
{
	UART1SendString("ok");
	frame_received_flag = 0;
	rx_buf_num = 0;
}
unsigned char CheckFrameReceived()
{
	if(frame_received_flag)
	{
		frame_received_flag = 0;
		return 1;
	}
	else
		return 0;
}
#else
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
#endif
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
	//INTCLKO |= EX4;	//使能INT4中断
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
void EEPROMWriteConfiguration(struct sys_config *config)
{
	unsigned char i = 0;
	unsigned int temp = 0;
	for(i; i < CONFIG_SIZE - 2; i++)				//对结构体中每一字节求和
		temp += ((unsigned char *)config)[i];
	config->check_sum = temp;
	EEPROM_SectorErase(EE_ADDRESS1);				//将求和结果存放在结构体最后一个数字中
	EEPROM_write_n(EE_ADDRESS1, (unsigned char *)config, CONFIG_SIZE);
}
unsigned char EEPROMReadConfiguration(struct sys_config *config)
{
	unsigned char i = 0;
	unsigned int temp = 0;
	EEPROM_read_n(EE_ADDRESS1, (unsigned char *)config, CONFIG_SIZE);
	for(i; i < CONFIG_SIZE - 2; i++)				//对结构体中每一字节求和
		temp += ((unsigned char *)config)[i];
	if(temp == config->check_sum)				//检验数据是否正确完整，结构体最后两字节为检验求和字节
		return 1;
	else
	{
		for(i = 0; i < CONFIG_SIZE; i++)
			((unsigned char *)config)[i] = ((unsigned char *)(&default_config))[i];
		return 0;
	}
}
void LED(unsigned char k)
{
	LED1 = k^0x01;
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
	EEPROMReadConfiguration(&config);//读掉电保存数据
	BuzzerInit();							//蜂鸣器初始化，使用硬件PWM7
	EnableBuzzer(config.key_sound);
	LED(OFF);									//关闭LED
	BatteryChargeEnable(1);		//允许电池充电
	Delay1ms(5);
	Enable3V3Output(1);				//允许3.3v电源输出
	EA = 1;										//开全局中断
}