#ifndef _PW02_H
#define _PW02_H

#define PW02_EN_PIN	P16
#define	PW02_Set_EN_PIN()	PW02_EN_PIN = 1
#define	PW02_Clr_EN_PIN()	PW02_EN_PIN = 0

#define	BAUD2400		0
#define	BAUD9600		1
#define	BAUD14400		2
#define	BAUD19200		3
#define	BAUD28800		4
#define	BAUD38400		5
#define	BAUD57600		6
#define	BAUD115200	7
#define	BAUD230400	8
#define	BAUD460800	9
#define	BAUD500000	10
#define	BAUD921600	11
#define	BAUD1000000	12

unsigned char PW02GetRxData(unsigned char *buf);
unsigned char PW02EnterATMode(void);
unsigned char PW02ExitATMode(void);
unsigned char PW02SetBuad(unsigned char baud_num);
unsigned char PW02SetADVT(unsigned int time_in_ms);
unsigned char PW02SetName(unsigned char *name);
unsigned char PW02GetRSSI(void);
unsigned char PW02CheckConnection(void);
void PW02Rest(void);
void PW02SetMode(unsigned char mode);
unsigned char PW02Init(void);

#endif