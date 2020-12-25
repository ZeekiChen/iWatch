#include "Sys.h"
#include "LSM6DSM.h"
#include "math.h"

void LSM6DSMWriteByte(unsigned char addr, unsigned char dat)
{
	Single_WriteIIC(LSM6DSM_IIC_ADDR, addr, dat);
}
unsigned char LSM6DSMReadByte(unsigned char addr)
{
	unsigned char temp;
	temp = Single_ReadIIC(LSM6DSM_IIC_ADDR, addr);
	return temp;
}
void LSM6DSMRSetRegisterBits(unsigned char addr, unsigned char dat, unsigned char SC)
{
	unsigned char temp;
	temp = LSM6DSMReadByte(addr);
	if(SC == 1)
		temp |= dat;
	else if(SC == 0)
		temp &= ~dat;
	LSM6DSMWriteByte(addr, temp);
}
unsigned char LSM6DSMCheckConnection()
{
	unsigned char temp;
	temp = LSM6DSMReadByte(WHO_AM_I);
	if(temp == 0x6a)
		return 1;
	else
		return 0;
}
void LSM6DSMConfigAcc(unsigned char acc_odr, unsigned char acc_scale)
{
	LSM6DSMWriteByte(CTRL1_XL, acc_odr | acc_scale);
}
void LSM6DSMConfigGyr(unsigned char gyr_odr, unsigned char gyr_scale)
{
	LSM6DSMWriteByte(CTRL2_G, gyr_odr | gyr_scale);
}
void LSM6DSMReadGYRAndACC(struct lsm6dsm_data *p)
{
	unsigned char buf[12];
	I2C_Read_MultiBytes(LSM6DSM_IIC_ADDR, OUTX_L_G, 12, buf);
	//gyr
	p->gyr_x = (buf[3] << 8) | buf[2];// / 65.536;
	p->gyr_y = -(buf[1] << 8) | buf[0];// / 65.536;
	p->gyr_z = (buf[5] << 8) | buf[4];// / 65.536;
	//acc
	p->acc_x = (buf[9] << 8) | buf[8];// / 8192.0;
	p->acc_y = -(buf[7] << 8) | buf[6];// / 8192.0;
	p->acc_z = (buf[11] << 8) | buf[10];// / 8192.0;
}
/*
int LSM6DSMReadTemperature(void)
{
	unsigned char buf[2];
	int temp;
	I2C_Read_MultiBytes(LSM6DSM_IIC_ADDR, OUT_TEMP_L, 2, buf);
	temp = (buf[1] << 8) | buf[0];
	return temp;
}
*/
void LSM6DSMSoftReset(void)
{
	LSM6DSMRSetRegisterBits(CTRL3_C, 0x01, 1);
	Delay1ms(50);
}
void LSM6DSMEnableEMbeddedFunc(void)
{
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x04, 1);		// Enable embedded functions
}
/*
void LSM6DSMDisableEMbeddedFunc(void)
{
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x04, 0);		// Disable embedded functions
}*/
void LSM6DSMEnableAWT(int angle, int delay)
{
	unsigned char set_delay;
	unsigned char set_angle;
	set_delay = (unsigned char)(delay / 40);
	set_angle = (unsigned char)(sin(angle * 3.1415926 / 180) * 64);
	if(set_delay > 0xff)
		set_delay = 0xff;
	if(set_angle > 0xff)
		set_angle = 0xff;
	LSM6DSMWriteByte(FUNC_CFG_ACCESS, 	0xa0);				// Enable access to embedded registers (bank B) 
	LSM6DSMWriteByte(A_WRIST_TILT_LAT, 	set_delay);		// Set new latency in A_WRIST_TILT_LAT 
	LSM6DSMWriteByte(A_WRIST_TILT_THS, 	set_angle);		// Set new threshold in A_WRIST_TILT_THS 
	LSM6DSMWriteByte(A_WRIST_TILT_Mask, 0x40);				// Set new mask in A_WRIST_TILT_Mask 
	LSM6DSMWriteByte(FUNC_CFG_ACCESS, 	0x00);				// Disable access to embedded registers (bank B) 
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x80, 1);				// Enable embedded functions(0x04)
																										// Enable AWT detection (0x80)
	LSM6DSMRSetRegisterBits(DRDY_PULSE_CFG, 0x01, 1);	// AWT interrupt driven to INT2 pin
}
/*
void LSM6DSMDisableAWT(void)
{
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x80, 0);				// Disable AWT detection (0x80)
	LSM6DSMRSetRegisterBits(DRDY_PULSE_CFG, 0x01, 0);	// AWT interrupt disconnect to INT2 pin
}*/
void LSM6DSMEnableTapDetection(void)
{
	LSM6DSMRSetRegisterBits(TAP_CFG, 0x82, 1);		// Enable interrupts(0x80) and tap detection on X-axis(0x08), Y-axis(0x04), Z-axis(0x02)
	LSM6DSMRSetRegisterBits(TAP_THS_6D, 0x8c, 1);	// Set tap threshold(LSB0 - MSB4, dafult value 00000) 
	LSM6DSMWriteByte(INT_DUR2, 0x7f);							// Set Duration, Quiet and Shock time windows 
	LSM6DSMRSetRegisterBits(WAKE_UP_THS, 0x80, 1);// Single & double-tap enabled (0x80)SINGLE_DOUBLE_TAP = 1, if = 0, SINGLE_TAP only)
	LSM6DSMRSetRegisterBits(MD1_CFG, 0x08, 1);		// (0x40)Single-tap interrupt driven to INT1 pin
																								// (0x08)Double-tap interrupt driven to INT1 pin
	//LSM6DSMRSetRegisterBits(MD2_CFG, 0x08, 1);	// (0x40)Single-tap interrupt driven to INT2 pin
																								// (0x08)Double-tap interrupt driven to INT2 pin
}
/*
void LSM6DSMDisableTapDetection(void)
{
	LSM6DSMRSetRegisterBits(TAP_CFG, 0x08|0x04|0x02, 0);	//tap detection on X-axis(0x08), Y-axis(0x04), Z-axis(0x02)
}*/
void LSM6DSMEnablePedometer(unsigned int debounce_time, unsigned char debounce_step)
{
	LSM6DSMWriteByte(FUNC_CFG_ACCESS, 0x80);							// Enable access to embedded functions registers (bank A)
	LSM6DSMWriteByte(CONFIG_PEDO_THS_MIN, 0x8e);					// PEDO_FS = ±4 g and configure pedometer minimum threshold value
	LSM6DSMWriteByte(PEDO_DEB_REG, ((unsigned char)(debounce_time / 80) << 3) | (debounce_step & 0x07));
	LSM6DSMWriteByte(FUNC_CFG_ACCESS, 0x00);							// Disable access to embedded functions registers (bank A)
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x10, 1);						// Enable embedded functions and pedometer algorithm
	//LSM6DSMRSetRegisterBits(INT1_CTRL, 0x80, 1);					// Step detector interrupt driven to INT1 pin
}
unsigned int LSM6DSMGetCurrentStep(void)
{
	unsigned char tempL, tempH;
	tempL = LSM6DSMReadByte(STEP_COUNTER_L);
	tempH = LSM6DSMReadByte(STEP_COUNTER_H);
	return ((tempH << 8) | tempL);
}
void LSM6DSMResetStepCounter(void)
{
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x02, 1);
	Delay1ms(1);
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x02, 0);
}
/*
void LSM6DSMDisablePedometer(void)
{
	LSM6DSMRSetRegisterBits(CTRL10_C, 0x10, 0);						// Disable pedometer algorithm
}
*/
unsigned char LSM6DSMInit(void)
{
	unsigned int time_out = 200;
	Delay1ms(50);
	while(LSM6DSMCheckConnection() == 0)
	{
		Delay1ms(1);
		if(--time_out == 0)
			return 0;
	}
	LSM6DSMSoftReset();
	LSM6DSMConfigAcc(ACC_ODR_416_HZ, ACC_SCALE_4_G);
	LSM6DSMConfigGyr(GYR_POWER_DOWN, GYR_SCALE_500_DPS);
	//LSM6DSMSetScale(ACC_SCALE_4_G, GYR_SCALE_500_DPS);
	//LSM6DSMSetODR(ACC_ODR_416_HZ, GYR_POWER_DOWN);
	//LSM6DSMRSetRegisterBits(CTRL3_C, 0x20, 1);	//interrupt output pads active low
	//LSM6DSMWriteByte(CTRL7_G, 	0x04);					//Source register rounding function enable
	LSM6DSMEnableAWT(10, 100);							//20 degree, 400ms
	LSM6DSMEnableTapDetection();
	LSM6DSMEnablePedometer(1040, 6);				//debounce time = 1040ms, debounce step = 6 steps
	LSM6DSMEnableEMbeddedFunc();
	return 1;
}
/******************************姿态解算相关************************************/
#define	pi		3.14159265f                           
#define	Kp		0.8f                        
#define	Ki		0.001f                         
#define	halfT	0.004f           
float idata q0=1,q1=0,q2=0,q3=0;   
float idata exInt=0,eyInt=0,ezInt=0; 
void IMUupdate(struct lsm6dsm_data *p)
{
	float data norm;
	float idata ax, ay, az;
	float idata gx, gy, gz;
	float idata vx, vy, vz;
	float idata ex, ey, ez;
	
	ax = p->acc_x;
	ay = p->acc_y;
	az = p->acc_z;
	
	gx = p->gyr_x * 0.0174533f / 65.536;
	gy = p->gyr_y * 0.0174533f / 65.536;
	gz = p->gyr_z * 0.0174533f / 65.536;

	norm = sqrt(ax*ax + ay*ay + az*az);	//把加速度计的三维向量转成单维向量   
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

		//	下面是把四元数换算成《方向余弦矩阵》中的第三列的三个元素。 
		//	根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，正好是这三个元素
		//	所以这里的vx vy vz，其实就是当前的欧拉角（即四元数）的机体坐标参照系上，换算出来的
		//	重力单位向量。
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3 ;

	ex = (ay*vz - az*vy) ;
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;

	exInt = exInt + ex * Ki;
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	q0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
	q1 = q1 + ( q0*gx + q2*gz - q3*gy) * halfT;
	q2 = q2 + ( q0*gy - q1*gz + q3*gx) * halfT;
	q3 = q3 + ( q0*gz + q1*gy - q2*gx) * halfT;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	p->AngleX = asin(2*(q0*q2 - q1*q3 )) * 57.2957795f; // 俯仰   换算成度
	p->AngleY = asin(2*(q0*q1 + q2*q3 )) * 57.2957795f; // 横滚
	p->AngleZ = atan((2*(q1*q2+q0*q3))/(q0*q0+q1*q1-q2*q2-q3*q3)) * 57.2957795f; // 偏航
}