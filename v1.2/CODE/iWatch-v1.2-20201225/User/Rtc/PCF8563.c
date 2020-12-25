#include "IIC.h"
#include "PCF8563.h"

/**
  * @brief  使用IIC总线往PCF8563的寄存器中写一字节数据
  * @param  addr: 寄存器的地址
  * @param  dat: 	待写入的数据
  * @retval None
  */
void PCF8563WriteByte(unsigned char addr, unsigned char dat)
{
	Single_WriteIIC(PCF8563_IIC_ADDR, addr, dat);
}
/**
  * @brief  使用IIC总线从PCF8563的寄存器中读一字节数据
  * @param  addr: 寄存器的地址
  * @retval 读出的一字节数据
  */
unsigned char PCF8563ReadByte(unsigned char addr)
{
	unsigned char temp;
	temp = Single_ReadIIC(PCF8563_IIC_ADDR,addr);
	return temp;
}
/**
  * @brief  置位或清除PCF8563寄存器中的指定位
  * @param  addr: 寄存器的地址
	*					bit_mask：指定位
	*					SC：0	清除
	*							1	置位
  * @retval 无
  */
void PCF8563SetRegisterBits(unsigned char addr, unsigned char bit_mask, unsigned char SC)
{
	unsigned char temp;
	temp = PCF8563ReadByte(addr);
	if(SC == 1)
		temp |= bit_mask;
	else if(SC == 0)
		temp &= ~bit_mask;
	PCF8563WriteByte(addr, temp);
}
/**
  * @brief  读取PCF8563中的时间并存在时间结构体中
  * @param  t：pcf8563_time型结构体的指针
  * @retval None
  */
void PCF8563ReadTime(pcf8563_time *t)
{
	unsigned char buf[7];
	I2C_Read_MultiBytes(PCF8563_IIC_ADDR, VL_SECONDS, 7, buf);
	buf[0] &= 0x7f;
	t->second = (buf[0] >> 4) * 10 + (buf[0] & 0x0f);
	buf[1] &= 0x7f;
	t->minute = (buf[1] >> 4) * 10 + (buf[1] & 0x0f);
	buf[2] &= 0x3f;
	t->hour 	= (buf[2] >> 4) * 10 + (buf[2] & 0x0f);
	buf[3] &= 0x3f;
	t->day 		= (buf[3] >> 4) * 10 + (buf[3] & 0x0f);
	buf[4] &= 0x07;
	if(buf[4] == 0)
		buf[4] = 7;
	t->weekday = buf[4];
	if(buf[5] & 0x80 == 0x80)
		t->century += 1;
	buf[5] &= 0x1f;
	t->month = (buf[5] >> 4) * 10 + (buf[5] & 0x0f);
	t->year = (unsigned int)buf[6];
}
/**
  * @brief  设置时间
	* @param  pcf8563_time结构体的指针
  * @retval None
  */
void PCF8563WriteTime(pcf8563_time *time)
{
	PCF8563WriteByte(VL_SECONDS, ((time->second / 10) << 4) | (time->second % 10));
	PCF8563WriteByte(MINUTES, ((time->minute / 10) << 4) | (time->minute % 10));
	PCF8563WriteByte(HOURS, ((time->hour / 10) << 4) | (time->hour % 10));
	PCF8563WriteByte(YEARS, (unsigned char)time->year);
	PCF8563WriteByte(CENTURY_MOUTHS, ((time->month / 10) << 4) | (time->month % 10));
	PCF8563WriteByte(DAYS, ((time->day / 10) << 4) | (time->day % 10));
	if(time->weekday == 7)
		time->weekday = 0;
	PCF8563WriteByte(WEEKDAYS, time->weekday);
}
/**
  * @brief  设置闹钟
	* @param  hour,min：小时，分钟
	*					day,weekday：日，星期(周一至周七对应1~7)
	*					mode：闹钟的模式
	*						ALARM_DISABLE				关闭闹钟
	*						ALARM_ONCE					单次有效
	*						ALARM_EVERYDAY			每天都响
	*						ALARM_WORKDAY				工作日响
	*						ALARM_SPECIFIC_DAY	指定某一天响
  * @retval None
  */
void PCF8563SetAlarm(unsigned char hour, unsigned char min, unsigned char weekday, unsigned char day, unsigned char mode)
{
	unsigned char temp1 = 0;
	unsigned char temp2 = 0;
	unsigned char temp3 = 0;
	unsigned char temp4 = 0;
	temp1 = 0x80 | ((hour / 10) << 4) | (hour % 10);
	temp2 = 0x80 | ((min / 10) << 4) | (min % 10);
	temp3 = 0x80 | weekday;
	temp4 = 0x80 | ((day / 10) << 4) | (day % 10);
	if(mode == ALARM_EVERYDAY || mode == ALARM_ONCE)
	{
		temp1 &= ~0x80;
		temp2 &= ~0x80;
	}
	else if(mode == ALARM_WORKDAY)
	{
		temp1 &= ~0x80;
		temp2 &= ~0x80;
		temp3 &= ~0x80;
	}
	else if(mode == ALARM_SPECIFIC_DAY)
	{
		temp1 &= ~0x80;
		temp2 &= ~0x80;
		temp4 &= ~0x80;
	}
	PCF8563WriteByte(HOUR_ALARM, temp1);
	PCF8563WriteByte(MINUTE_ALARM, temp2);
	PCF8563WriteByte(WEEKDAY_ALARM, temp3);
	PCF8563WriteByte(DAY_ALARM, temp4);
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x02, 1);	//使能时钟中断输出
}
/**
  * @brief  清除PCF8563内部闹钟标志位
	*					闹钟中断产生后，必须清除标志位后下一次才能正常工作
	* @param  无
  * @retval 无
  */
void PCF8563ClearAlarmFlag(void)
{
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x08, 0);
}
/**
  * @brief  关闭闹钟
	* @param  无
  * @retval 无
  */
void PCF8563DisableAlarm(void)
{
	PCF8563SetRegisterBits(HOUR_ALARM, 0x80, 1);
	PCF8563SetRegisterBits(MINUTE_ALARM, 0x80, 1);
	PCF8563SetRegisterBits(WEEKDAY_ALARM, 0x80, 1);
	PCF8563SetRegisterBits(DAY_ALARM, 0x80, 1);
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x08, 0);
}
/**
  * @brief  使能PCF8563内部计时器，并打开中断
	* @param  clock_fq：PCF8563内部计时器的时钟源
	*						TIMERCLK_4096_HZ	4.096khz
	*						TIMERCLK_64_HZ		64hz
	*						TIMERCLK_1_HZ			1hz
	*						TIMERCLK_1_60_HZ	1/60hz
	*					value：倒计时的值
  * @retval 无
  */
void PCF8563EnableTimer(unsigned char clock_fq, unsigned char value)
{
	PCF8563SetRegisterBits(TIMER_CONTROL, 0x80, 0);	//关闭定时器
	PCF8563WriteByte(TIMER_CONTROL, clock_fq);			//设置定时器的时钟源
	PCF8563WriteByte(TIMER, value);									//设置定时器的初值
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x01, 1);	//打开中断输出
	PCF8563SetRegisterBits(TIMER_CONTROL, 0x80, 1);			//打开定时器
}
/**
  * @brief  清除PCF8563内部计时器标志位
	*					倒计时计时器中断产生后，必须清除标志位后下一次才能正常工作
	* @param  无
  * @retval 无
  */
void PCF8563ClearTimerFlag(void)
{
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x04, 0);
}
/**
  * @brief  关闭PCF8563内部计时器
	* @param  无
  * @retval 无
  */
void PCF8563DisableTimer(void)
{
	PCF8563SetRegisterBits(CONTROL_STATUS_2, 0x01, 0);	//关闭中断输出
	PCF8563SetRegisterBits(TIMER_CONTROL, 0x80, 0);			//关闭定时器
}
/**
  * @brief  读PCF8563中断源
	* @param  无
  * @retval temp：PCF8563内部状态寄存器的值
  */
unsigned char PCF8563ReadIntSrc(void)
{
	unsigned char temp;
	temp = PCF8563ReadByte(CONTROL_STATUS_2);
	temp &= 0x0c;
	return temp;
}
/**
	* @brief 	使能PCF8563的时钟输出
	* @param  clock_fq：输出的时钟频率
	*							CLKOUT_32768_HZ		32.768khz
	*							CLKOUT_1024_HZ		1.924khz
	*							CLKOUT_32_HZ			32hz
	*							CLKOUT_1_HZ				1hz
	*					en：ENABLE_CLKOUT			使能时钟输出
	*							DISABLE_CLKOUT		禁止时钟输出
	* @retval 无
	*/
void PCF8563EnableClockOuput(unsigned char clock_fq, unsigned char en)
{
	PCF8563WriteByte(CLKOUT_CONTROL, (en << 7) | clock_fq);
}
void PCF8563Init(void)
{
	PCF8563ClearAlarmFlag();
	PCF8563ClearTimerFlag();
	PCF8563EnableTimer(TIMERCLK_1_60_HZ, 1);
	//PCF8563DisableTimer();
	PCF8563EnableClockOuput(CLKOUT_1024_HZ, DISABLE_CLKOUT);
}