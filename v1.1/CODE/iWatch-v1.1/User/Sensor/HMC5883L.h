#ifndef __HMC5883L_H
#define __HMC5883L_H

#define	HMC5883L_IIC_ADDR   0x3C	  //定义器件在IIC总线中的从地址

unsigned char CheckHMC5883LConnection(void);
void Read_HMC5883L(int *buf);
float HMC5883L_Get_AngleXY(int *buf, float ax, float ay);
void HMC5883L_Set_Mode(unsigned char mode);
unsigned char HMC5883L_Init(void);
void HMC5883L_Set_Calibration_Value(int x_offset, int y_offset, int z_offset, float y_scale, float z_scale);

#endif