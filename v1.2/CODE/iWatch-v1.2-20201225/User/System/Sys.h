#ifndef __SYS_H
#define __SYS_H

#include "stc8a.h"
#include "Buzzer.h"
#include "Key.h"
#include "IIC.h"
#include "Delay.h"
#include "EEPROM.h"

/**********************LED*************************/
#define	LED1	P27
/*********************POWER************************/
#define	PROG		P17						//TP4054的充电使能引脚
#define	EN_3V3	P40						//LDO TPS78233的使能引脚
/*********************OTHER************************/
#define EE_ADDRESS1	0x0180		//EEPROM写入起始地址
#define	ON	1
#define	OFF	0

void PinInit(void);
void UartInit(void);
int ADCRead(unsigned char adc_ch);
float GetBatteryVoltage(void);
void Power3V3En(unsigned char k);
void BatChargeEn(unsigned char k);
void MCUSoftReset(void);
void FeedWatchDog(void);
void LED(unsigned char k);
unsigned char EEPROMReadConfiguration(struct sys_config *config);
void EEPROMWriteConfiguration(struct sys_config *config);
void SystemSleep(void);
void SystemPowerDown(void);
void SystemWake(void);
void SysInit(void);

#endif