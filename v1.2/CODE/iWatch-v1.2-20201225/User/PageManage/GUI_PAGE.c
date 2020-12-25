#include "GUI_PAGE.h"
#include "iWatch.h"

extern unsigned char xdata main_cache[];
extern unsigned char xdata sub_cache[];

PageList_TypeDef pagelist[PAGE_MAX];
static unsigned int page_timestamp = 0;
static bit page_do_anim = 0;
static char page_open_anim = 0;
static char page_close_anim = 0;
static bit page_busy = 0;
static int now_page = PAGE_NULL;
static int new_page = page_watch;

/**
  * @brief  提供页面时钟，每1毫秒调用一次
  * @param  无
  * @retval 无
  */
void PageTick(void)
{
	page_timestamp ++;
}
/**
  * @brief  在页面循环中通过查询此函数的结果来获得指定的执行频率
  * @param  执行频率相关的结构体指针
  * @retval 无
  */
unsigned char PageExecuteRate(PageExecuteRate_TypeDef *er)
{
	if(page_timestamp - er->last_timestamp >= er->executeT)
	{
		er->last_timestamp = page_timestamp;
		return 1;
	}
	else
		return 0;
}
void PageOpenAnim(unsigned char direction)
{
	page_open_anim = direction;
	page_do_anim = 1;
	page_busy = 1;
}
void PageCloseAnim(unsigned char direction)
{
	page_close_anim = direction;
	page_do_anim = 1;
	page_busy = 1;
}
void PageDoAnimation(void)
{
	static unsigned int step_close = 0;
	static unsigned char step_open = 0;
	int data i = 0, j = 0;
	unsigned char data temp;
	unsigned int data temp1;
	static bit lock = 0; 
	if(page_close_anim != -1)
	{
		if(page_close_anim == 0)//push up
		{
			temp = 7 - step_close / 8;
			for(i = 0; i < temp; i++)
			{
				temp1 = i * 128;
				for(j = 0; j < 128; j++)
				{
					main_cache[temp1 + j] = (main_cache[temp1 + j] >> 2) | (main_cache[temp1 + 128 + j] << 6);
				}
			}
			temp1 = i * 128;
			for(j = 0; j < 128; j++)
			{
				main_cache[temp1 + j] >>= 2;
			}
			if(++step_close == 32)
			{
				step_close = 0;
				page_close_anim = -1;
			}
		}
		else if(page_close_anim == 1)//down
		{
			temp = step_close / 8;
			for(i = 7; i > temp; i--)
			{
				temp1 = i * 128;
				for(j = 0; j < 128; j++)
				{
					main_cache[temp1 + j] = (main_cache[temp1 + j] << 2) | (main_cache[temp1 - 128 + j] >> 6);
				}
			}
			temp1 = i * 128;
			for(j = 0; j < 128; j++)
			{
				main_cache[temp1 + j] <<= 2;
			}
			if(++step_close == 32)
			{
				step_close = 0;
				page_close_anim = -1;
			}
		}
		else if(page_close_anim == 2)//left
		{
			for(i = 0; i < 8; i++)
			{
				temp = 126 - step_close * 2;
				temp1 = i * 128;
				for(j = 0; j < temp; j++)
				{
					main_cache[temp1 + j] = main_cache[temp1 + j + 2];
				}
				main_cache[temp1 + temp] = 0;
				main_cache[temp1 + temp + 1] = 0;
			}
			if(++step_close == 64)
			{
				step_close = 0;
				page_close_anim = -1;
			}
		}
		else if(page_close_anim == 3)//right
		{
			for(i = 0; i < 8; i++)
			{
				temp = step_close * 2;
				temp1 = i * 128;
				for(j = 127; j > 1; j--)
				{
					main_cache[temp1 + j] = main_cache[temp1 + j - 2];
				}
				main_cache[temp1 + temp] = 0;
				main_cache[temp1 + temp + 1] = 0;
			}
			if(++step_close == 64)
			{
				step_close = 0;
				page_close_anim = -1;
			}
		}
	}
	if(page_open_anim != -1)
	{
		unsigned char data n = 0;
		if(page_open_anim == 0)
		{
			step_open += 2;
			n = step_open / 8;
			temp = step_open % 8;
			if(temp == 0)
			{
				for(i = 0; i < n; i++)
				{
					temp1 = i * 128;
					for(j = 0; j < 128; j++)
						main_cache[(8 - n) * 128 + temp1 + j] = sub_cache[temp1 + j];
				}
			}
			else
			{
				for(i = 0; i <= n; i++)
				{
					temp1 = i * 128;
					if(i == 0)
					{
						for(j = 0; j < 128; j++)
						{
							main_cache[(7 - n) * 128 + j] &= (0xff >> temp);
							main_cache[(7 - n) * 128 + j] |= (sub_cache[j] << (8 - temp));
						}
					}
					else
					{
						for(j = 0; j < 128; j++)
							main_cache[(7 - n) * 128 + temp1 + j] = (sub_cache[temp1 + j] << (8 - temp));
					}
				}
				if(n != 0)
				{
					for(i = 0; i < n; i++)
					{
						temp1 = i * 128;
						for(j = 0; j < 128; j++)
							main_cache[(8 - n) * 128 + temp1 + j] |= sub_cache[temp1 + j] >> temp;
					}
				}
			}
			if(step_open >= 64)
			{
				step_open = 0;
				page_open_anim = -1;
			}
		}
		else if(page_open_anim == 1) //down
		{
			step_open += 2;
			n = step_open / 8;
			temp = step_open % 8;
			if(temp == 0)
			{
				for(i = 0; i < n; i++)
				{
					temp1 = i * 128;
					for(j = 0; j < 128; j++)
						main_cache[temp1 + j] = sub_cache[(8 - n) * 128 + temp1 + j];
				}
			}
			else
			{
				for(i = n; i >= 0; i--)
				{
					temp1 = i * 128;
					if(i == n)
					{
						for(j = 0; j < 128; j++)
						{
							main_cache[temp1 + j] &= (0xff << temp);
							main_cache[temp1 + j] |= (sub_cache[896 + j] >> (8 - temp));
						}
					}
					else
					{
						for(j = 0; j < 128; j++)
							main_cache[temp1 + j] = (sub_cache[(7 - n) * 128 + temp1 + j] >> (8 - temp));
					}
				}
				/*
				for(j = 0; j < 128; j++)
				{
					main_cache[j] &= (0xff << temp);
					main_cache[j] |= (sub_cache[(7 - n) * 128 + temp1 + j] >> (8 - temp));
				}
				*/
				if(n != 0)
				{
					for(i = 0; i < n; i++)
					{
						temp1 = i * 128;
						for(j = 0; j < 128; j++)
							main_cache[temp1 + j] |= sub_cache[(8 - n) * 128 + temp1 + j] << temp;
					}
				}
			}
			if(step_open >= 64)
			{
				step_open = 0;
				page_open_anim = -1;
			}
		}
		else if(page_open_anim == 2) //left
		{
			step_open += 2;
			for(i = 0; i < 8; i++)
			{
				temp1 = i * 128;
				for(j = 0; j < step_open; j++)
				{
					main_cache[temp1 + j] = sub_cache[temp1 + (127 - step_open) + j];
				}
			}
			if(step_open >= 128)
			{
				step_open = 0;
				page_open_anim = -1;
			}
		}
		else if(page_open_anim == 3) //right
		{
			step_open += 2;
			for(i = 0; i < 8; i++)
			{
				temp1 = i * 128;
				for(j = 0; j < step_open; j++)
				{
					main_cache[temp1 + (127 - step_open) + j] = sub_cache[temp1 + j];
				}
			}
			if(step_open >= 128)
			{
				step_open = 0;
				page_open_anim = -1;
			}
		}
	}
	if(page_close_anim == 2 || page_close_anim == 3)
	{
		if(page_open_anim == 0 || page_open_anim == 1)
		{
			lock = 1;
		}
	}
	if(lock)
	{
		if(step_open == 0)
		{
			step_close = 0;
			page_close_anim = -1;
			lock = 0;
		}
	}
	if(page_open_anim != -1 || page_close_anim != -1)
		OLED_Refresh(main_cache);
	else
	{
		page_do_anim = 0;
		page_busy = 0;
	}
}
/**
  * @brief  注册一个基本页面，包含一个初始化函数，循环函数，退出函数，事件函数
  * @param  pageID: 页面编号
	*	@param	pageText: 页面标题的指针
	*	@param	pageIcon: 页面图标的指针
  * @param  setupCallback: 初始化函数回调
  * @param  loopCallback: 循环函数回调
  * @param  exitCallback: 退出函数回调
  * @param  eventCallback: 事件函数回调
  * @retval 无
  */
void PageRegister(
    unsigned char pageID,
		unsigned char *pageText,
		unsigned char *pageIcon,
    CallbackFunction_t setupCallback,
    CallbackFunction_t loopCallback,
    CallbackFunction_t exitCallback,
    EventFunction_t eventCallback
)
{
		pagelist[pageID].Text = pageText;
		pagelist[pageID].Icon = pageIcon;
    pagelist[pageID].SetupCallback = setupCallback;
    pagelist[pageID].LoopCallback = loopCallback;
    pagelist[pageID].ExitCallback = exitCallback;
    pagelist[pageID].EventCallback = eventCallback;
}
/**
  * @brief  页面事件传递
  * @param  event: 事件编号
  * @retval 无
  */
void PageEventTransmit(unsigned char event)
{
	/*将事件传递到当前页面*/
  if(pagelist[now_page].EventCallback != 0)
		pagelist[now_page].EventCallback(event);
}
/**
  * @brief  页面切换
  * @param  pageID：页面号
	* @retval 1：成功  0：失败
  */
void PageShift(unsigned char pageID)
{
	if(page_busy == 0)
	{		
		new_page = pageID;
	}
}
void PageCloseCurrentPage()
{
	pagelist[now_page].ExitCallback();
}
void PageOpenCurrentPage()
{
	pagelist[now_page].SetupCallback();
}
unsigned char PageGetStatus(void)
{
	if(page_busy)
		return 1;
	else
		return 0;
}
void PageRun(void)
{
	if(now_page != new_page)
	{
		if((pagelist[now_page].ExitCallback != 0) && (now_page != PAGE_NULL))
			pagelist[now_page].ExitCallback();
		if(pagelist[new_page].SetupCallback != 0)
			pagelist[new_page].SetupCallback();
		now_page = new_page;
	}
	if(page_busy == 0 && (iWatchGetStatus()&IWATCH_ACTIVE))
	{
		pagelist[now_page].LoopCallback();
	}
	if(page_do_anim)
	{
		PageDoAnimation();
	}
}
void PageInit(void)
{
	PAGE_REG(page_watch);
	PAGE_REG(page_menu);
	PAGE_REG(page_bme280);
	PAGE_REG(page_lsm6dsm);
	PAGE_REG(page_snake);
	PAGE_REG(page_flashlight);
	PAGE_REG(page_pedometer);
	PAGE_REG(page_stopwatch);
	PAGE_REG(page_compass);
	PAGE_REG(page_mag_cal);
	PAGE_REG(page_setting);
	PAGE_REG(page_settime);
	PAGE_REG(page_cyberpunk);
	PageShift(page_watch);
	
}
