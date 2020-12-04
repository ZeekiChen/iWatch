#include "stc8a.h"
#include "PowerManage.h"
#include "sys.h"
#include "intrins.h"

int *BGV = (int idata *)0xef;

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
	* @brief 	3.3V电源使能
	* @param  k：
							0	关闭3.3v电源
							1	打开3.3v电源
	* @retval 无
	*/
void Enable3V3Output(unsigned char k)
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
void BatteryChargeEnable(unsigned char k)
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
/**
	* @brief 	测量MCU当前电源电压
	* @param 	无
	* @retval 电压值，单位v（伏特）
	*/
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
	* @brief 	测量当前电池剩余电量，测量结果已经过窗口滤波
	* @param  无
	* @retval percentage：剩余电量的百分比
	*/
#define	V_SHUTDOWN		3.4		//放电截止电压
#define	V_FULLCHARGE	4.13	//满电电压
#define	WINDOW_WIDTH	20		//窗口滤波宽度
float GetBatteryLife(void)
{
	static unsigned char first_time_flag = 0;
	static float queue[WINDOW_WIDTH];		//窗口滤波队列
	float queue_average = 0;						//队列均值
	float percentage;
	unsigned char i = 0;
	percentage = (GetBatteryVoltage() - V_SHUTDOWN) / (V_FULLCHARGE - V_SHUTDOWN);
	if(percentage > 1)
		percentage = 1;
	else if(percentage < 0)
		percentage = 0;
	if(first_time_flag == 0)						//若队列中没数据，初始化队列
	{
		first_time_flag = 1;
		for(i = 0; i < WINDOW_WIDTH; i++)
			queue[i] = percentage;
	}
	for(i = WINDOW_WIDTH - 1; i > 0; i--)
	{
		queue[i] = queue[i - 1];
		queue_average += queue[i];
	}
	queue_average += percentage;
	percentage = queue_average / WINDOW_WIDTH;
	queue[0] = percentage;
	return percentage;
}
/**
	* @brief 	调用该函数使MCU进入掉电模式
	*					不要轻易改动这个函数，否则会影响功耗
	* @param  无
	* @retval 无
	*/
void SystemPowerDown(void)
{
	P0 = 0x00;
	//P1 = 0x78;
	P1M0 = 0x00;
	P1M1 = 0x80;
	P2 = 0x31;
	P2M0 = 0x00;
	P2M1 = 0x00;
	//P3 = 0x84;
	P5 = 0x00;
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
void SystemDeepPowerDown(void)
{
	unsigned char temp = P1;
	P0 = 0x00;
	P1 = 0x00;
	P1M0 = 0x30;
	P1M1 = 0xb0;
	P2 = 0x01;
	P2M0 = 0x00;
	P2M1 = 0x00;
	P3 = 0x84;
	P4 = 0x00;
	P5 = 0x00;
	ET0 = 0;
	IE2 &= ~ES2; 		//关闭串口2中断
	IE2 &= ~ET3;		//定时器3关溢出中断
	PCON = 0x02;		//MCU进入掉电模式
	_nop_();
	_nop_();
	P1 = temp;
}
/**
	* @brief 	调用该函数将系统从掉电模式或者深度掉电模式中恢复过来
	*					不要轻易改动这个函数，否则会影响功耗
	* @param  无
	* @retval 无
	*/
void SystemPowerOn(void)
{
	PinInit();

	ET0 = 1;
	IE2 |= ES2; 		//使能串口中断
	IE2 |= ET3;			//定时器3开溢出中断
}