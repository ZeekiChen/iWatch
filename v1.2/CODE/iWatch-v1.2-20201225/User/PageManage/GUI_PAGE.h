#ifndef GUI_PAGE_H
#define GUI_PAGE_H

enum PageID{
	PAGE_NULL = -1,
	page_watch,
	page_menu,
	PAGE_MENU_MIN,
	//......
	page_stopwatch,
	page_bme280,
	page_lsm6dsm,
	page_snake,
	page_flashlight,
	page_pedometer,
	page_compass,
	page_cyberpunk,
	page_setting,
	//......
	PAGE_MENU_MAX,
	page_mag_cal,
	page_settime,
	PAGE_MAX
};

#define	PAGE_HOR_SIZE	128
#define	PAGE_VER_SIZE	64
//页面从屏幕的（上下左右）边沿进入
#define	ANIM_TOP		0
#define	ANIM_BOTTOM	1
#define	ANIM_LEFT		2
#define	ANIM_RIGHT	3

typedef void(*CallbackFunction_t)(void);
typedef void(*EventFunction_t)(unsigned char);
typedef struct {
	unsigned char *Text;
	unsigned char *Icon;
	CallbackFunction_t SetupCallback;
  CallbackFunction_t LoopCallback;
  CallbackFunction_t ExitCallback;
  EventFunction_t EventCallback;
}PageList_TypeDef;
typedef struct {
	unsigned int executeT;
	unsigned int last_timestamp;
}PageExecuteRate_TypeDef;
void PageRegister(
    unsigned char pageID,
		unsigned char *pageText,
		unsigned char *pageIcon,
    CallbackFunction_t setupCallback,
    CallbackFunction_t loopCallback,
    CallbackFunction_t exitCallback,
    EventFunction_t eventCallback
);

#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(unsigned char pageID);\
    PageRegister_##name(##name);\
}while(0)
void PageOpenAnim(unsigned char direction);
void PageCloseAnim(unsigned char direction);
void PageTick(void);
unsigned char PageExecuteRate(PageExecuteRate_TypeDef *er);
void PageEventTransmit(unsigned char event);
void PageShift(unsigned char pageID);
void PageCloseCurrentPage(void);
void PageOpenCurrentPage(void);
unsigned char PageGetStatus(void);
void PageRun(void);
void PageInit(void);

#endif

