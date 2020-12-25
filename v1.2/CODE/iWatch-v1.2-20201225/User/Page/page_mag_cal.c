#include "iWatch.h"

extern unsigned char xdata main_cache[];
extern unsigned char xdata sub_cache[];

static PageExecuteRate_TypeDef Rate_50hz = {20, 0};


extern iWatch_config config;		//设置信息结构体
extern int magnet_data[3];
static unsigned char str[16];
static unsigned int data_cnt = 0;
struct cal_data magnet_cal_data;		//磁力计校准数据结构体
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	//ClearCache(sub_cache);
	LIS3MDL_Set_Mode(0);	//设置连续测量模式
	LIS3MDL_Set_Calibration_Value(0, 0, 0, 1, 1);
}
/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	Ellipsoid_fitting_Process(&magnet_cal_data);		//椭球校准算法
	LIS3MDL_Set_Calibration_Value(magnet_cal_data.X0, magnet_cal_data.Y0, magnet_cal_data.Z0, 
																magnet_cal_data.A / magnet_cal_data.B, 
																magnet_cal_data.A / magnet_cal_data.C);//设置校准参数
	config.cal_magnet_x0 = magnet_cal_data.X0;
	config.cal_magnet_y0 = magnet_cal_data.Y0;
	config.cal_magnet_z0 = magnet_cal_data.Z0;
	config.cal_magnet_ab = magnet_cal_data.A / magnet_cal_data.B;
	config.cal_magnet_ac = magnet_cal_data.A / magnet_cal_data.C;
	iWatchSaveConfig(&config);
	data_cnt = 0;
	LIS3MDL_Set_Mode(2);
	//PageCloseAnim(1);
}
/**
  * @brief  页面循环执行的内容
  * @param  无
  * @retval 无
  */
static void Loop()
{
	if(PageExecuteRate(&Rate_50hz) == 1)
	{
		Read_LIS3MDL(magnet_data);
		CalcData_Input(magnet_data[0], magnet_data[1], magnet_data[2]);
		ClearCache(main_cache);
		sprintf(str, "x:%d", magnet_data[0]);
		ShowString(0, 0, str, main_cache, FONT8X16, NO_INVERSED);
		sprintf(str, "y:%d", magnet_data[1]);
		ShowString(0, 2, str, main_cache, FONT8X16, NO_INVERSED);
		sprintf(str, "z:%d", magnet_data[2]);
		ShowString(0, 4, str, main_cache, FONT8X16, NO_INVERSED);
		sprintf(str, "data_cnt:%d", data_cnt);
		ShowString(0, 6, str, main_cache, FONT8X16, NO_INVERSED);
		ScreenRefreshAll(main_cache);
		if(++data_cnt == 5000)
		{
			Bee();
			PageShift(page_compass);
		}
	}
}
/**
  * @brief  页面事件
  * @param  btn:发出事件的按键
  * @param  event:事件编号
  * @retval 无
  */
static void Event(unsigned char event)
{
	if(event == KEY1)
	{
		PageShift(page_compass);
	}
}

void PageRegister_page_mag_cal(unsigned char pageID)
{
	PageRegister(pageID, 0, 0, Setup, Loop, Exit, Event);
}