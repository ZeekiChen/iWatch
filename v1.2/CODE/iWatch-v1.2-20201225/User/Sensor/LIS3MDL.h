#ifndef __LIS3MDL_H
#define __LIS3MDL_H

#define	LIS3MDL_IIC_ADDR   0x38	  //定义器件在IIC总线中的从地址

unsigned char CheckLIS3MDLConnection(void);
void Read_LIS3MDL(int *buf);
float LIS3MDL_Get_AngleXY(int *buf, float ax, float ay);
void LIS3MDL_Set_Mode(unsigned char mode);
unsigned char LIS3MDL_Init(void);
void LIS3MDL_Set_Calibration_Value(int x_offset, int y_offset, int z_offset, float y_scale, float z_scale);

#endif