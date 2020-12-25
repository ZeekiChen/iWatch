#ifndef __LSM6DSM_H_
#define __LSM6DSM_H_

#define LSM6DSM_IIC_ADDR	0xd4

//Register map
#define	FUNC_CFG_ACCESS					0x01 
#define	SENSOR_SYNC_TIME_FRAME	0x04
#define	SENSOR_SYNC_RES_RATIO		0x05
#define	FIFO_CTRL1 			0x06
#define	FIFO_CTRL2 			0x07
#define	FIFO_CTRL3 			0x08
#define	FIFO_CTRL4 			0x09
#define	FIFO_CTRL5 			0x0A
#define	DRDY_PULSE_CFG 	0x0B 
#define	INT1_CTRL 			0x0D
#define	INT2_CTRL 			0x0E
#define	WHO_AM_I 				0x0F
#define	CTRL1_XL 				0x10
#define	CTRL2_G 				0x11
#define	CTRL3_C 				0x12
#define	CTRL4_C 				0x13
#define	CTRL5_C 				0x14
#define	CTRL6_C 				0x15
#define	CTRL7_G 				0x16
#define	CTRL8_XL 				0x17
#define	CTRL9_XL 				0x18
#define	CTRL10_C 				0x19
#define	MASTER_CONFIG 	0x1A
#define	WAKE_UP_SRC 		0x1B
#define	TAP_SRC 				0x1C
#define	D6D_SRC 				0x1D
#define	STATUS_REG 			0x1E
#define	STATUS_SPIAux		0x1E
#define	OUT_TEMP_L 			0x20
#define	OUT_TEMP_H 			0x21
#define	OUTX_L_G 				0x22
#define	OUTX_H_G 				0x23
#define	OUTY_L_G 				0x24
#define	OUTY_H_G 				0x25
#define	OUTZ_L_G 				0x26
#define	OUTZ_H_G 				0x27
#define	OUTX_L_XL 			0x28
#define	OUTX_H_XL 			0x29
#define	OUTY_L_XL 			0x2A
#define	OUTY_H_XL 			0x2B
#define	OUTZ_L_XL 			0x2C
#define	OUTZ_H_XL 			0x2D
/*skip sensorhub secion1
#define ........
.
.
.
#define ........
*/
#define	FIFO_STATUS1 			0x3A
#define	FIFO_STATUS2 			0x3B
#define	FIFO_STATUS3 			0x3C
#define	FIFO_STATUS4 			0x3D
#define	FIFO_DATA_OUT_L 	0x3E
#define	FIFO_DATA_OUT_H 	0x3F
#define	TIMESTAMP0_REG 		0x40
#define	TIMESTAMP1_REG 		0x41
#define	TIMESTAMP2_REG 		0x42
#define	STEP_TIMESTAMP_L 	0x49
#define	STEP_TIMESTAMP_H 	0x4A
#define	STEP_COUNTER_L 		0x4B
#define	STEP_COUNTER_H 		0x4C
/*skip sensorhub secion2
#define ........
.
.
.
#define ........
*/
#define	FUNC_SRC1 				0x53
#define	FUNC_SRC2 				0x54
#define	WRIST_TILT_IA 		0x55
#define	TAP_CFG 					0x58
#define	TAP_THS_6D 				0x59
#define	INT_DUR2 					0x5A
#define	WAKE_UP_THS 			0x5B
#define	WAKE_UP_DUR 			0x5C
#define	FREE_FALL 				0x5D
#define	MD1_CFG 					0x5E
#define	MD2_CFG 					0x5F
#define	MASTER_CMD_CODE 	0x60
#define	SENS_SYNC_SPI_ERROR_CODE	0x61
#define	OUT_MAG_RAW_X_L 	0x66
#define	OUT_MAG_RAW_X_H 	0x67
#define	OUT_MAG_RAW_Y_L 	0x68
#define	OUT_MAG_RAW_Y_H 	0x69
#define	OUT_MAG_RAW_Z_L 	0x6A
#define	OUT_MAG_RAW_Z_H 	0x6B
#define	INT_OIS 		0x6F
#define	CTRL1_OIS 	0x70
#define	CTRL2_OIS 	0x71
#define	CTRL3_OIS 	0x72
#define	X_OFS_USR 	0x73
#define	Y_OFS_USR 	0x74
#define	Z_OFS_USR 	0x75
//Accessing the registers blow needs to write 0xa0 to FUNC_CFG_ACCESS
//Embedded function registers(bank A)
#define CONFIG_PEDO_THS_MIN 0x0F
#define	SM_THS							0x13
#define	PEDO_DEB_REG 				0x14
#define	STEP_COUNT_DELTA 		0x15
//Embedded function registers(bank B)
#define	A_WRIST_TILT_LAT 		0x50
#define	A_WRIST_TILT_THS 		0x54
#define	A_WRIST_TILT_Mask 	0x59

#define	ACC_POWER_DOWN		(0x00 << 4)
#define	ACC_ODR_1_6_HZ		(0x0b << 4)
#define	ACC_ODR_12_5_HZ		(0x01 << 4)
#define	ACC_ODR_26_HZ			(0x02 << 4)
#define	ACC_ODR_52_HZ			(0x03 << 4)
#define	ACC_ODR_104_HZ		(0x04 << 4)
#define	ACC_ODR_208_HZ		(0x05 << 4)
#define	ACC_ODR_416_HZ		(0x06 << 4)
#define	ACC_ODR_833_HZ		(0x07 << 4)
#define	ACC_ODR_1_66_KHZ	(0x08 << 4)
#define	ACC_ODR_3_33_KHZ	(0x09 << 4)
#define	ACC_ODR_6_66_KHZ	(0x0a << 4)
#define	ACC_SCALE_2_G			(0x00 << 2)
#define	ACC_SCALE_16_G		(0x01 << 2)
#define	ACC_SCALE_4_G			(0x02 << 2)
#define	ACC_SCALE_8_G			(0x03 << 2)

#define	GYR_POWER_DOWN		(0x00 << 4)	
#define	GYR_ODR_12_5_HZ		(0x01 << 4)
#define	GYR_ODR_26_HZ			(0x02 << 4)
#define	GYR_ODR_52_HZ			(0x03 << 4)
#define	GYR_ODR_104_HZ		(0x04 << 4)
#define	GYR_ODR_208_HZ		(0x05 << 4)
#define	GYR_ODR_416_HZ		(0x06 << 4)
#define	GYR_ODR_833_HZ		(0x07 << 4)
#define	GYR_ODR_1_66_KHZ	(0x08 << 4)
#define	GYR_ODR_3_33_KHZ	(0x09 << 4)
#define	GYR_ODR_6_66_KHZ	(0x01 << 4)
#define	GYR_SCALE_250_DPS		(0x00 << 2)
#define	GYR_SCALE_500_DPS		(0x01 << 2)
#define	GYR_SCALE_1000_DPS	(0x02	<< 2)
#define	GYR_SCALE_2000_DPS	(0x03	<< 2)

struct lsm6dsm_data{
	int acc_x;
	int acc_y;
	int acc_z;
	int gyr_x;
	int gyr_y;
	int gyr_z;
	float AngleX;
	float AngleY;
	float AngleZ;
};

void LSM6DSMWriteByte(unsigned char addr, unsigned char dat);
unsigned char LSM6DSMReadByte(unsigned char addr);
void LSM6DSMRSetRegisterBits(unsigned char addr, unsigned char dat, unsigned char SC);

unsigned char LSM6DSMCheckConnection(void);
void LSM6DSMConfigAcc(unsigned char acc_odr, unsigned char acc_scale);
void LSM6DSMConfigGyr(unsigned char gyr_odr, unsigned char gyr_scale);
void LSM6DSMReadGYRAndACC(struct lsm6dsm_data *p);
int LSM6DSMReadTemperature(void);
void LSM6DSMSoftReset(void);
void LSM6DSMEnableEMbeddedFunc(void);
void LSM6DSMDisableEMbeddedFunc(void);
void LSM6DSMEnableAWT(int angle, int Delay);
void LSM6DSMDisableAWT(void);
void LSM6DSMEnableTapDetection(void);
void LSM6DSMDisableTapDetection(void);
void LSM6DSMEnablePedometer(unsigned int debounce_time, unsigned char debounce_step);
unsigned int LSM6DSMGetCurrentStep(void);
void LSM6DSMResetStepCounter(void);
void LSM6DSMDisablePedometer(void);
unsigned char LSM6DSMInit(void);
void IMUupdate(struct lsm6dsm_data *p);

#endif