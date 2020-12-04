#ifndef __IIC_H_
#define __IIC_H_

void IIC_Init(void);
void Wait(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char dat);
unsigned char I2C_RecvByte(void);
void I2C_RecvACK(void);
void I2C_SendACK(void);
void I2C_SendNAK(void);
void Single_WriteIIC(unsigned char SlaveAddress, unsigned char REG_Address, unsigned char REG_data);
unsigned char Single_ReadIIC(unsigned char SlaveAddress, unsigned char REG_Address);
//void I2C_NoAddr_WriteByte(unsigned char DeviceAddr, unsigned char dat);
void I2C_NoAddr_Write_MultiBytes(unsigned char DeviceAddr, unsigned char BytesNum, unsigned char *buf);
void I2C_NoAddr_Read_MultiBytes(unsigned char DeviceAddr, unsigned char BytesNum, unsigned char *buf);
void I2C_Read_MultiBytes(unsigned char DeviceAddr,unsigned char address, unsigned char BytesNum, unsigned char * OutDate);

#endif