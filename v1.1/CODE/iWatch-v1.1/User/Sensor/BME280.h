#ifndef _BME280_H
#define _BME280_H

//IICµÿ÷∑‘ˆ¡ø£¨(SD0 = 0) £∫0xec£¨ (SDO = 1) £∫0xee
#define BME280_IIC_ADDR	0xec

#define HUM_LSB			0xFE
#define HUM_MSB			0xFD
#define	TEMP_XLSB		0xFC
#define	TEMP_LSB		0xFB
#define	TEMP_MSB		0xFA	
#define	PRESS_XLSB	0xF9
#define	PRESS_LSB		0xF8
#define	PRESS_MSB		0xF7
#define	CONFIG			0xF5
#define	CTRL_MEAS		0xF4
#define	STATUS			0xF3
#define	CTRL_HUM		0xF2
#define	CALIB26			0xE1
#define	RESET				0xE0
#define	BME280_ID		0xD0
#define	CALIB00			0x88

#define	SLEEP_MODE	0
#define	FORCED_MODE	1
#define	NORMAL_MODE	3
#define	MS_0_5	0
#define	MS_62_5	1
#define	MS_125	2
#define	MS_250	3
#define	MS_500	4
#define	MS_1000	5
#define	MS_10		6
#define	MS_20		7

struct bme280_data{
	float temperature;
	float pressure;
	float humidity;
};

void BME280WriteByte(unsigned char addr, unsigned char dat);
unsigned char BME280ReadByte(unsigned char addr);
unsigned char BME280CheckConnection(void);
unsigned int BME280_cal_meas_delay(void);
void BME280ReadCompensationParameter(void);
void BME280ReadSensorRawData(void);
float compensate_temperature(void);
float compensate_pressure(void);
float compensate_humidity(void);
void BME280GetSensorData(struct bme280_data *p);
void BME280SoftReset(void);
unsigned char BME280Init();
void BME280SetMode(unsigned char mode);
void BME280SingleMeasurement(struct bme280_data *p);
void BME280ContinuousMeasurement(unsigned char t);

#endif