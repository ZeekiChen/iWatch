#ifndef __OLED_H
#define __OLED_H

#include "stc8a.h"

#define  u8 unsigned char 
#define  u32 unsigned int 
	
//OLEDÄ£Ê½ÉèÖÃ
//0:4Ïß´®ÐÐÄ£Ê½
//1:²¢ÐÐ8080Ä£Ê½
#define OLED_MODE 1

#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

#if OLED_MODE == 0

sbit OLED_CS = P2^0; 		//Æ¬Ñ¡
sbit OLED_RST =P2^2;		//¸´Î»
sbit OLED_DC = P2^1;		//Êý¾Ý/ÃüÁî¿ØÖÆ
sbit OLED_SCL = P2^4;		//Ê±ÖÓ D0£¨SCLK£
sbit OLED_SDIN = P2^3;	//D1£¨MOSI£© Êý¾Ý

#define OLED_CS_Clr()  OLED_CS=0
#define OLED_CS_Set()  OLED_CS=1

#define OLED_RST_Clr() OLED_RST=0
#define OLED_RST_Set() OLED_RST=1

#define OLED_DC_Clr() OLED_DC=0
#define OLED_DC_Set() OLED_DC=1

#define OLED_SCLK_Clr() OLED_SCL=0
#define OLED_SCLK_Set() OLED_SCL=1

#define OLED_SDIN_Clr() OLED_SDIN=0
#define OLED_SDIN_Set() OLED_SDIN=1;

#else

#define OLED_DAT	P0
sbit OLED_RST = P1^3;		//¸´Î»
sbit OLED_DC =	P1^2;		//Êý¾Ý/ÃüÁî¿ØÖÆ
sbit OLED_WR =	P4^3;
sbit OLED_RD =	P4^4;
//sbit OLED_CS=P2^0; //Æ¬Ñ¡

#define OLED_RST_Clr() OLED_RST=0
#define OLED_RST_Set() OLED_RST=1

#define OLED_DC_Clr() OLED_DC=0
#define OLED_DC_Set() OLED_DC=1

#define OLED_WR_Clr() OLED_WR=0
#define OLED_WR_Set() OLED_WR=1

#define OLED_RD_Clr() OLED_RD=0
#define OLED_RD_Set() OLED_RD=1

#endif

#define XLevelL			0x02
#define XLevelH			0x10
#define Max_Column	128
#define Max_Row			64
#define X_WIDTH 		128
#define Y_WIDTH 		64	    						  
//-----------------OLED¶Ë¿Ú¶¨Òå----------------  					   

void delay_ms(unsigned int ms);

//OLED¿ØÖÆÓÃº¯Êý
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	
void OLED_Set_Brightness(unsigned char brightness);
void OLED_Horizental_Reverse(unsigned char reverse);
void OLED_Vertical_Reverse(unsigned char reverse);
void OLED_Inverse(unsigned char k);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_DrawBMP(unsigned char *buf);


#endif  
	 



