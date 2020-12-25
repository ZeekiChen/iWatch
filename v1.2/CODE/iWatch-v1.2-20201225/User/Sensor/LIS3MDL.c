#include "LIS3MDL.h"
#include "Sys.h"
#include "math.h"

/*****************************校准参数************************************/											
int mag_x_offset = 0, mag_y_offset = 0, mag_z_offset = 0;		//零位校准参数
float mag_y_scale = 1, mag_z_scale = 1;											//幅度校准参数
/*************************************************************************/

/**
  * @brief  使用IIC总线往LIS3MDL的寄存器中写一字节数据
  * @param  addr: 寄存器的地址
  * @param  dat: 	待写入的数据
  * @retval 无
  */
void LIS3MDL_Write_Byte(unsigned char addr, unsigned char dat)
{
	Single_WriteIIC(LIS3MDL_IIC_ADDR, addr, dat);
}
/**
  * @brief  使用IIC总线从LIS3MDL的寄存器中读一字节数据
  * @param  addr: 寄存器的地址
  * @retval 读出的一字节数据
  */
unsigned char LIS3MDL_Read_Byte(unsigned char addr)
{
	unsigned char temp;
	temp = Single_ReadIIC(LIS3MDL_IIC_ADDR, addr);
	return temp;
}
/**
  * @brief  检查与LIS3MDL的连接是否正常
  * @param  无
  * @retval 1（成功），0（失败）
  */
unsigned char CheckLIS3MDLConnection(void)
{
	if(LIS3MDL_Read_Byte(0x0f) != 0x3d)
		return 0;
	else
		return 1;
}
/**
	* @brief 	从LIS3MDL中读取读传感器原始数据
	* @param  buf：原始数据数组的指针，int*型
	* @retval 无
	*/
void Read_LIS3MDL(int *buf)
{
	unsigned char tp[6];
	tp[0] = LIS3MDL_Read_Byte(0x29);
	tp[1] = LIS3MDL_Read_Byte(0x28);
	tp[2] = LIS3MDL_Read_Byte(0x2b);
	tp[3] = LIS3MDL_Read_Byte(0x2a);
	tp[4] = LIS3MDL_Read_Byte(0x2d);
	tp[5] = LIS3MDL_Read_Byte(0x2c);
	//I2C_Read_MultiBytes(LIS3MDL_IIC_ADDR, 0x03, 6, tp);
	buf[0] = tp[2] << 8 | tp[3]; //Combine MSB and LSB of Y Data output register  最高有效位
  buf[1] = -(tp[0] << 8 | tp[1]); //Combine MSB and LSB of X Data output register
  buf[2] = tp[4] << 8 | tp[5]; //Combine MSB and LSB of Z Data output register
	buf[0] -= mag_x_offset;
	buf[1] -= mag_y_offset;
	buf[2] -= mag_z_offset;
	buf[1] *= mag_y_scale;
	buf[2] *= mag_z_scale;
}
/**
	* @brief 	设置LIS3MDL原始数据的校准参数
	* @param  x_offset, x_offset, x_offset：对应轴的偏移
	*					y_scale：y轴对x轴的比例校准
	*					z_scale：z轴对x轴的比例校准
	* @retval 无
	*/
void LIS3MDL_Set_Calibration_Value(int x_offset, int y_offset, int z_offset, float y_scale, float z_scale)
{
	mag_x_offset = x_offset;
	mag_y_offset = y_offset;
	mag_z_offset = z_offset;
	mag_y_scale = y_scale;
	mag_z_scale = z_scale;
}
/**
	* @brief 	设置LIS3MDL的工作模式
	* @param  mode：0	连续测量模式
	*								1	单次测量模式
	*								2	休眠模式
	* @retval 无
	*/
void LIS3MDL_Set_Mode(unsigned char mode)
{
	unsigned char CTRL_REG3 = 0x00;
	CTRL_REG3 = LIS3MDL_Read_Byte(0x22);
	CTRL_REG3 &= (~0x03);
	CTRL_REG3 |= mode;
	LIS3MDL_Write_Byte(0x22, CTRL_REG3);
}
/**
	* @brief 	LIS3MDL初始化函数
	* @param  无
	* @retval 0：失败
	*					1：成功
	*/
unsigned char LIS3MDL_Init(void)
{
	unsigned char time_out = 200;
	while(CheckLIS3MDLConnection() == 0)
	{
		Delay1ms(1);
		if(--time_out == 0)
			return 0;
	}
	LIS3MDL_Write_Byte(0x20, 0x7c);//CTRL_REG1
	LIS3MDL_Write_Byte(0x21, 0x00);//CTRL_REG2
	LIS3MDL_Write_Byte(0x22, 0x02);//CTRL_REG3
	LIS3MDL_Write_Byte(0x23, 0x0c);//CTRL_REG4
	LIS3MDL_Write_Byte(0x24, 0x00);//CTRL_REG5
	LIS3MDL_Write_Byte(0x30, 0x00);//INT_CFG
	//HMC5883L_Set_Calibration_Value(-83, 10, -74, 0.99041592, 1.16104081);
	return 1;
}
/**
	* @brief 	根据x轴和y轴的倾角补偿磁偏角
	* @param  mag_data：三轴磁力数据
	*					ax，ay：x轴和y轴的倾角
	* @retval Yaw：补偿后的磁偏角
	*/
float LIS3MDL_Get_AngleXY(int *mag_data, float ax, float ay)
{
	float Yaw, RadX, RadY, Xh, Yh;
	RadX = -ax / 57.2957795f;
	RadY = -ay / 57.2957795f;
	Xh = mag_data[0] * cos(RadX) + mag_data[1] * sin(RadY) * sin(RadX) - mag_data[2] * cos(RadY) * sin(RadX);
	Yh = mag_data[1] * cos(RadY) + mag_data[2] * sin(RadY);
	Yaw = atan2(Yh, Xh) * 180 / 3.14159 + 180;
	return Yaw;
}