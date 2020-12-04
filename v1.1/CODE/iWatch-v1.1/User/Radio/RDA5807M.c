#include "IIC.h"
#include "stc8a.h"
#include "RDA5807M.h"
#include "intrins.h"
#include "Delay.h"

//RDA5807M设置寄存器组，每个寄存器两字节
//设置寄存器
unsigned int REG02 = 0x0000, REG03 = 0x0000,	REG04 = 0x0040,	REG05 = 0x9088;
//状态寄存器
unsigned int REG0A = 0x0000, REG0B = 0x0000;
/******************************设置参数*************************************/
unsigned int CHAN = 0;
unsigned char BAND = 0;			//Banc Select
														//00 = 87-108 MHz(Default)
														//01 = 76-91 MHz
														//10 = 76-108MHz
														//11 = 65-76 MHz
unsigned char SPACE = 0;		//Channel Spacing
														//00 = 100kHz(Default)	01 = 200kHz;
														//10 = 50kHz;						11 = 25kHz;
unsigned char SEEKTH = 8;		//Seek SNR threshold value
unsigned char VOLUME = 8;		//DAC Gain Control Bit(Volume)
														//0000 = min;		1111 = max
float current_frequency;	//当前频率
unsigned char RSSI;				//当前频道信号强度值
/**************************************************************************/

/**
	* @brief 	RDA5807M写寄存器
	*					RDA5807M的的寄存器为16位，写寄存器时只能连续写入
	*					不能够单独写一个16位寄存器，这里使用IIC连续写入8字节
	*					即连续写RDA5807M中的4个主要设置寄存器REG02 ~ REG05
	* @param  无
	* @retval 无
	*/
void RDA5807MWriteRegs(void)
{
	unsigned char reg[8];
	reg[0] = REG02 >> 8;
	reg[1] = REG02;
	reg[2] = REG03 >> 8;
	reg[3] = REG03;
	reg[4] = REG04 >> 8;
	reg[5] = REG04;
	reg[6] = REG05 >> 8;
	reg[7] = REG05;
	P_SW2 |= 0x10;
	I2C_NoAddr_Write_MultiBytes(RDA5807M_IIC_ADDR, 8, reg);
	P_SW2 &= ~0x10;
}
/**
	* @brief 	RDA5807M读寄存器
	*					RDA5807M的的寄存器为16位，读寄存器时只能连续读出
	*					不能够单独读一个16位寄存器，这里使用IIC连续读出4字节
	*					即连续读出RDA5807M中的2个主要状态寄存器REG0A,REG0B
	* @param  无
	* @retval 无
	*/
void RDA5807MReadRges(void)
{
	unsigned char read_buf[4];
	P_SW2 |= 0x10;
	I2C_NoAddr_Read_MultiBytes(RDA5807M_IIC_ADDR, 4, read_buf);
	P_SW2 &= ~0x10;
	REG0A = read_buf[0];
	REG0A <<= 8;
	REG0A |= read_buf[1];
	REG0B = read_buf[2];
	REG0B <<= 8;
	REG0B |= read_buf[3];
}
/**
	* @brief 	RDA5807M初始化函数
	*					初始化后RDA5807M默认在睡眠模式
	* @param  无
	* @retval 0：失败
	*					1：成功
	*/
unsigned char RDA5807MInit(void)
{
  // 发送软件复位指令
  REG02 |= SOFT_RST;
  RDA5807MWriteRegs();
  Delay1ms(10);
	//REG02 = 0xd001;	  //禁止高阻输出，禁止静音，双声道，Bass boost enabled ，seek up,seek enable,循环搜索模式，时钟源频率：32.768khz，Power Up Enable
	REG02 = 0x9000 | DMUTE;
	REG03 = (CHAN << 6) | (BAND << 2) | SPACE;
	REG04 = 0x0000;
	REG05 = 0x8080 | (SEEKTH << 8) | VOLUME;
  RDA5807MWriteRegs();
	return 1;
}
/**
	* @brief 	RDA5807M设定音量
	* @param  VOLUME：音量（0~15）
	* @retval 无
	*/
void RDA5807MSetVOLUME(unsigned int VOLUME)
{
	if(VOLUME > 0x0f)				//限幅
		VOLUME = 0x0f;
	REG05 &= ~0x000f;				//先清空原来的值
	REG05 |= VOLUME & 0x0f;	//写入现有值
  RDA5807MWriteRegs();		//写入寄存器
}
/**
	* @brief 	RDA5807M自动搜台
	* @param  direction：搜索方向
	*						UPWARD		向上搜台
	*						DOWNWARD	向下搜台
	* @retval 无
	*/
float RDA5807MSEEK(unsigned char direction)
{
	static unsigned char step = 0;
	if(step == 0)
	{
		REG03 &= ~TUNE; 		//disable tune
		if(direction == UPWARD) //seek up
			REG02 |= SEEK_UP;
		else								//seek down
			REG02 &= ~SEEK_UP;
		REG02 |= SEEK;			//enable seek
		RDA5807MWriteRegs();
		
		step++;
	}
	else if(step == 1)
	{
		RDA5807MReadRges();
		if((REG0A & 0x4000))//等待STC标志置位
		{
			REG02 &= ~SEEK;					//disable seek
			CHAN = REG0A & 0x03ff;
			current_frequency = (float)CHAN * 0.1 + 87;
			RSSI = (REG0B >> 9)&0x0003f;
			step = 0;
			return current_frequency;
		}
		else
			return 0;
  }
	return 0;
}
/**
	* @brief 	RDA5807M设定频率（调频）
	* @param  fq：电台的频率（87 ~ 108）
	* @retval 无
	*/
void RDA5807MSetFq(float fq)
{
	REG02 &= ~SEEK;			//disable seek
	REG03 |= TUNE; 			//enable tune
	CHAN = fq * 10 - 870;
	REG03 &= 0x003f;
	REG03 |= (CHAN << 6);
	RDA5807MWriteRegs();
	RDA5807MReadRges();
	while((REG0A & 0x4000) == 0)		//等待STC 标志置位
  {
		Delay1ms(3);
    RDA5807MReadRges();	// 读取状态寄存器
  }
	REG03 &= ~TUNE;				//disable tune
	current_frequency = fq;
	RSSI = (REG0B >> 9)&0x0003f;
}
/**
	* @brief 	获取当前收听的频道的频率
	* @param  无
	* @retval current_frequency：当前收听频道的频率
	*/
float RDA5807MGetCurrentFq(void)
{
	RDA5807MReadRges();
	CHAN = REG0A & 0x03ff;
	current_frequency = (float)CHAN * 0.1 + 87;
	return current_frequency;
}
/**
	* @brief 	获取当前收听的频道的信号强度值
	* @param  无
	* @retval RSSI：当前收听频道的信号强度值
	*/
unsigned char RDA5807MGetCurrentRSSI(void)
{
	RDA5807MReadRges();
	RSSI = (REG0B >> 9)&0x0003f;
	RSSI = (float)CHAN * 0.1 + 87;
	return RSSI;
}
/**
	* @brief 	使RDA5807M音频输出静音
	* @param  无
	* @retval 无
	*/
void RDA5807MSetMute(void)
{
	REG02 &= ~DMUTE;
	RDA5807MWriteRegs();
}
/**
	* @brief 	RDA5807M上电开机
	* @param  无
	* @retval 无
	*/
void RDA5807MPowerUp(void)
{
	REG02 |= POWER_EN;
	RDA5807MWriteRegs();
}
/**
	* @brief 	RDA5807M关机掉电（睡眠模式）
	* @param  无
	* @retval 无
	*/
void RDA5807MPowerDown(void)
{
	REG02 &= ~POWER_EN;
	RDA5807MWriteRegs();
}