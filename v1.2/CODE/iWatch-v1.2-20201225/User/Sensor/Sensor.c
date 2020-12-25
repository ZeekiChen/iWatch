#include "iWatch.h"

extern unsigned char xdata main_cache[];

void SensorInit(void)
{
	unsigned char y = 0;
	unsigned int time_out = 700;
	unsigned char error_count = 0;
	if(BME280Init() == 0)
	{
		y += ShowString(0, y, "BME280 ERROR", main_cache, FONT8X16, NO_INVERSED);
		error_count++;
	}
	if(LIS3MDL_Init() == 0)
	{
		y += ShowString(0, y, "LIS3MDL ERROR", main_cache, FONT8X16, NO_INVERSED);
		error_count++;
	}
	if(LSM6DSMInit() == 0)
	{
		y += ShowString(0, y, "LSM6DSM ERROR", main_cache, FONT8X16, NO_INVERSED);
		error_count++;
	}
	ScreenRefreshAll(main_cache);
	if(error_count != 0)
	{
		while(--time_out != 0)
			Delay1ms(1);
	}
}