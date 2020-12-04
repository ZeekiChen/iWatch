#include "stc8a.h"
#include "IIC.h"
#include "Delay.h"

unsigned int data timeout;

void IIC_Init()
{
	P_SW2 |= 0x80;
	I2CCFG = 0xd0;                              //使能I2C主机模式
  I2CMSST = 0x00;
	P_SW2 &= 0x7f;
}
void Wait()
{
	timeout = 300;
	while (!(I2CMSST & 0x40))
	{
		if(--timeout == 0)
			break;
	}
  I2CMSST &= ~0x40;
}
void I2C_Start()
{
	I2CMSCR = 0x01;                             //发送START命令
	Wait();
}
void I2C_Stop()
{
	I2CMSCR = 0x06;                             //发送STOP命令
  Wait();
}
void I2C_SendByte(unsigned char dat)
{
	I2CTXD = dat;                               //写数据到数据缓冲区
  I2CMSCR = 0x02;                             //发送SEND命令
	Wait();
}
unsigned char I2C_RecvByte()
{
	I2CMSCR = 0x04;                             //发送RECV命令
  Wait();
  return I2CRXD;
}
void I2C_RecvACK()
{
	I2CMSCR = 0x03;                             //发送读ACK命令
  Wait();
}
void I2C_SendACK()
{
	I2CMSST = 0x00;                             //设置ACK信号
  I2CMSCR = 0x05;                             //发送ACK命令
  Wait();
}
void I2C_SendNAK()
{
	I2CMSST = 0x01;                             //设置NAK信号
  I2CMSCR = 0x05;                             //发送ACK命令
  Wait();
}
//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteIIC(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
	P_SW2 |= 0x80;
  I2C_Start();                  //起始信号
  I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
	I2C_RecvACK();
	I2C_SendByte(REG_Address);    //内部寄存器地址
	I2C_RecvACK();
	I2C_SendByte(REG_data);       //内部寄存器数据
	I2C_RecvACK();
	I2C_Stop();                   //发送停止信号
	P_SW2 &= 0x7f;
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
unsigned char Single_ReadIIC(unsigned char SlaveAddress,unsigned char REG_Address)
{
	unsigned char REG_data;
	P_SW2 |= 0x80;
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_RecvACK();
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始
	I2C_RecvACK();
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	I2C_RecvACK();
	REG_data=I2C_RecvByte();       //读出寄存器数据
	I2C_SendNAK();								 //发送应答信号
	I2C_Stop();                    //停止信号
	P_SW2 &= 0x7f;
	return REG_data;
}
/*
void I2C_NoAddr_WriteByte(unsigned char DeviceAddr,unsigned char dat)
{
	P_SW2 |= 0x80;
  I2C_Start();                  //起始信号
  I2C_SendByte(DeviceAddr);   //发送设备地址+写信号
	I2C_RecvACK();
	I2C_SendByte(dat);    
	I2C_RecvACK();
	I2C_Stop();                   //发送停止信号
	P_SW2 &= 0x7f;
}
*/
void I2C_NoAddr_Write_MultiBytes(unsigned char DeviceAddr, unsigned char BytesNum, unsigned char *buf)
{
	unsigned char i = 0;
	P_SW2 |= 0x80;
	I2C_Start();
  I2C_SendByte(DeviceAddr);
	I2C_RecvACK();
  for(i = 0; i < BytesNum; i++)
  {
		I2C_SendByte(buf[i]);
		I2C_RecvACK();
  }
  I2C_Stop();
	P_SW2 &= 0x7f;
}
void I2C_NoAddr_Read_MultiBytes(unsigned char DeviceAddr, unsigned char BytesNum, unsigned char *buf)
{
	unsigned char i = 0;
	P_SW2 |= 0x80;
	I2C_Start();
  I2C_SendByte(DeviceAddr + 1);
	I2C_RecvACK();
	for(i = 0; i< (BytesNum - 1); i++)
	{
		buf[i] = I2C_RecvByte();
		I2C_SendACK();
	}
  buf[i] = I2C_RecvByte();
	I2C_SendNAK();
  I2C_Stop();
	P_SW2 &= 0x7f;
}
void I2C_Read_MultiBytes(unsigned char DeviceAddr, unsigned char address, unsigned char BytesNum, unsigned char *buf)
{
	unsigned char i;
	P_SW2 |= 0x80;
  I2C_Start();                  //起始信号
  I2C_SendByte(DeviceAddr);   //发送设备地址+写信号
	I2C_RecvACK();
	I2C_SendByte(address);    //内部寄存器地址
	I2C_RecvACK();
	I2C_Start();
	I2C_SendByte(DeviceAddr+1);
	I2C_RecvACK();
	for(i = 0; i < (BytesNum - 1); i++)
	{
		buf[i] = I2C_RecvByte();
		I2C_SendACK();	
	}
	buf[i] = I2C_RecvByte();
	I2C_SendNAK();
	I2C_Stop();                   //发送停止信号
	P_SW2 &= 0x7f;
}