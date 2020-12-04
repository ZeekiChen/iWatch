#include "stc8a.h"
#include "Delay.h"
#include "PW02.h"
#include "stdio.h"
#include "string.h"
#include "Display.h"

sbit PW02_SLEEP = P1^6;
bit busy = 0;
unsigned char rx_buf[128];
unsigned char rx_count = 0;
bit receive_flag = 0;

extern unsigned char xdata main_cache[];

void Uart2SendStr(unsigned char *p)
{
	while(*p)
  {
		while(busy);
		busy = 1;
		S2BUF = *p++;
  }
	rx_count = 0;
}
void UART2_Isr() interrupt 8 using 1
{
	if(S2CON & 0x02)
	{
		S2CON &= ~0x02;                         //清中断标志
    busy = 0;
  }
	if(S2CON & 0x01)
	{
		S2CON &= ~0x01;                         //清中断标志
		rx_buf[rx_count++] = S2BUF;
		if(rx_buf[rx_count - 1] == '\n')
		{
			if(rx_buf[rx_count - 2] == '\r')
				rx_buf[rx_count - 2] = '\0';
			else
				rx_buf[rx_count - 1] = '\0';
			receive_flag = 1;
			rx_count = 0;
		}
  }
}
unsigned char PW02GetRxData(unsigned char *buf)
{
	if(receive_flag == 1)
	{
		receive_flag = 0;
		strcpy(buf, rx_buf);
		return 1;
	}
	else
		return 0;
}
unsigned char SendATCommand(unsigned char *command, unsigned int retry_times, unsigned char *expected_response, unsigned char response_len)
{
	receive_flag = 0;
	Uart2SendStr(command);
	Delay1ms(10);
	while(receive_flag == 0)
	{
		if(--retry_times == 0)
			return 0;
		Uart2SendStr(command);
		Delay1ms(10);
	}	
	receive_flag = 0;
	if(memcmp(rx_buf, expected_response, response_len) == 0)
		return 1;
	else
		return 0;
}
unsigned char PW02EnterATMode(void)
{
	if(SendATCommand("AT:STR\r\n", 20, "AT:OK", 5))
		return 1;
	else
		return 0;
}
unsigned char PW02ExitATMode(void)
{
	if(SendATCommand("AT:END\r\n", 20, "AT:END", 6))
		return 1;
	else
		return 0;
}
unsigned char PW02SetBuad(unsigned char baud_num)
{
	unsigned char str[12] = {0};
	sprintf(str, "AT:BAUD=%d\r\n", (int)baud_num);
	if(SendATCommand(str, 5, "OK+Set", 6))
		return 1;
	else
		return 0;
}
unsigned char PW02SetADVT(unsigned int time_in_ms)
{
	unsigned char str[16] = {0};
	sprintf(str, "AT:ADVT=%d\r\n", (int)time_in_ms);
	if(SendATCommand(str, 5, "adv_time:", 9))
		return 1;
	else
		return 0;
}
unsigned char PW02SetName(unsigned char *name)
{
	unsigned char str[28] = "AT:NAME=";
	strcat(str, name);
	strcat(str, "\r\n");
	if(SendATCommand(str, 5, "Changed Name to:", 16))
		return 1;
	else
		return 0;
}
unsigned char PW02GetRSSI(void)
{
	if(SendATCommand("AT:RSSI?\r\n", 5, "RSSI:", 5))
		return 1;
	else
		return 0;
}
void PW02Rest(void)
{
	Uart2SendStr("AT+REST\r\n");
}
void PW02SetMode(unsigned char mode)
{
	if(mode == 1)
		PW02_Clr_EN_PIN();
	else
		PW02_Set_EN_PIN();
}
unsigned char PW02CheckConnection(void)
{
	unsigned char y = 0;
	PW02_SLEEP = 0;
	Delay1ms(50);
	ClearCache(main_cache);
	PW02EnterATMode();
	y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 0);
	if(PW02GetRSSI())
	{
		y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 0);
		PW02ExitATMode();
		y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
		return 1;
	}
	else
	{
		y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
		return 0;
	}
}
unsigned char PW02Init(void)
{
	unsigned char y = 0;
	PW02_Set_EN_PIN();
	//9600bps@24.000MHz
	//串口2
	PW02_Clr_EN_PIN();
	Delay1ms(100);
	if(PW02EnterATMode())		//进入AT模式
		y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
	else
		return 0;
	PW02SetBuad(BAUD9600);				//设置波特率为9600（默认）
	y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
	PW02SetName("My bluetooth");	//设置蓝牙名称为My bluetooth
	y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
	PW02SetADVT(3000);						//设置广播周期为3000ms
	y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
	if(PW02ExitATMode())					//退出AT模式
		y = y + ShowString(0, y, rx_buf, main_cache, FONT8X16, NO_INVERSED, 1);
	else
		return 0;
	PW02_Set_EN_PIN();
	return 1;
}