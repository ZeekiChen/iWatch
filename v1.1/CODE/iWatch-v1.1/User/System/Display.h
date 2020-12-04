#ifndef __DISPLAY_H
#define __DISPLAY_H

#define	ON				1
#define	OFF				0
#define	UP              0
#define DOWN            1
#define LEFT            2
#define RIGHT           3
#define	FONT6X8		8
#define	FONT8X16	16
#define	COVER			0
#define	BLEND			1
#define	NORMAL		0
#define	INVERSED	1
#define	NO_INVERSED	0
#define	UPSIDEDOWN	1

void BMPToCache(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char *buf1, unsigned char *buf2, unsigned char k);
void ClearCache(unsigned char *buf);
void ClearCacheArea(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char *buf1);
void SaveScreen(void);
void ScreenRefreshAll(unsigned char *cache_buf);
void ScreenRefreshArea(unsigned char x, unsigned char y, unsigned char width, unsigned char height);
void ScreenPushAnimation(unsigned char *cache, unsigned char direction);
void ScreeRollUp(unsigned char *buf2, unsigned char num);
void ScreeRollDown(unsigned char *buf2, unsigned char num);
void ScreenOnOff(unsigned char k);
void ScreenSetBrightness(unsigned char screen_brightness);
void ScreenSetDirection(unsigned char k);
void ScreenSetInverse(unsigned char k);
void DisplayInit(struct sys_config *config);

void DisplayTime(unsigned char hour, unsigned char min, unsigned char sec);
unsigned char ShowString(unsigned char x, unsigned char y, unsigned char *str, unsigned char *cache_buf, unsigned char front_size, unsigned char inverse, unsigned char do_refresh);

void DrawDot(unsigned char x, unsigned char y);
void DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void DrawCircle(unsigned char x, unsigned char y, unsigned char radius);
void DrawArm(unsigned char x, unsigned char y, unsigned char radius, int angle);
void DrawSelectionFrame(unsigned char x, unsigned char y);
#endif