#include "iWatch.h"

extern unsigned char xdata main_cache[];
extern unsigned char xdata sub_cache[];

static PageExecuteRate_TypeDef Rate_1000hz = {1, 0};
static PageExecuteRate_TypeDef Rate_125hz = {8, 0};

static unsigned char code Text[] = "S.N.A.K.E";
static unsigned char code Icon[] = {
0x00,0x00,0xE0,0xE0,0x38,0x38,0x38,0x38,0x38,0x38,0x00,0xF8,0xF8,0xF0,0xC0,0xC0,
0x00,0x00,0xF8,0xF8,0x00,0xC0,0xF8,0xF8,0x38,0x38,0x38,0xF8,0xF8,0xC0,0x00,0xF8,
0xF8,0x00,0x00,0xC0,0xF8,0x38,0x00,0xC0,0xF0,0xF8,0x38,0x38,0x38,0x38,0x38,0x00,
0x00,0x00,0x07,0x07,0x0C,0x0C,0x0C,0x0C,0xF8,0xF8,0x00,0xFF,0xFF,0xFF,0x00,0x03,
0x0F,0x0C,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x0C,0x0C,0x0C,0xFF,0xFF,0xFF,0x00,0xFF,
0xFF,0x1C,0x7F,0xF3,0xC0,0x00,0x00,0xFF,0xFF,0xFF,0x0C,0x0C,0x0C,0x0C,0x00,0x00,
0x00,0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x03,0x03,0x00,0x07,0x07,0x07,0x00,0x00,
0x00,0x00,0x07,0x07,0x00,0x07,0x07,0x07,0x00,0x00,0x00,0x07,0x07,0x07,0x00,0x07,
0x07,0x00,0x00,0x01,0x07,0x07,0x00,0x01,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,
0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0x00,0xF8,0xF8,0xF8,0x38,0x38,0xF8,0xF8,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x0E,
0x0E,0x00,0x00,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0xFE,0xFE,0xFE,0xC0,0xC0,0xFF,0xFF,
0xFF,0x01,0x01,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x00,0x00,0x0F,0x0F,0x0F,0x0E,0x0E,0x0E,0x0F,0x0F,0x0F,0x00,0x00,0x7F,0x7F
/* (48 X 48 )*/
};


#define PANEL_WIDTH		92
#define PANEL_HEIGHT	64
#define	MAX_SNAKE_SPEED	50
#define	CONTROL_SENSITIVITY	10	//控制的灵敏度，越小越灵敏
static unsigned char i = 0;
static unsigned char str[6];
static unsigned char snake_direction = 0;	//蛇当前的前进方向
static unsigned char snake_speed = 10;		//蛇当前的前进速度
static unsigned char past_x[256];
static unsigned char past_y[256];
static unsigned char snake_head_x = 64, snake_head_y = 32, snake_len = 20;
static unsigned char test_now = 0, delicious_x= 80,delicious_y = 40;
static unsigned char best_score = 0;	//最佳成绩
static bit game_loop = 0;
static bit game_over = 0;
static bit game_pause = 0;

extern struct lsm6dsm_data LSM6DSM;			//LSM6DSM数据结构体
/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
	ClearCache(sub_cache);
	PageOpenAnim(ANIM_RIGHT);
	snake_direction = 0;
	snake_head_x = 64;
	snake_head_y = 32;
	snake_len = 20;
	game_over = 0;
	game_pause = 0;
	for(i = 0; i < snake_len; i++)
	{
		past_x[i] = 64;
		past_y[i] = 32;
	}
	LSM6DSMConfigAcc(ACC_ODR_208_HZ, ACC_SCALE_4_G);
	LSM6DSMConfigGyr(GYR_ODR_208_HZ, GYR_SCALE_500_DPS);
}
/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
	LSM6DSMConfigAcc(ACC_ODR_416_HZ, ACC_SCALE_4_G);
	LSM6DSMConfigGyr(GYR_POWER_DOWN, GYR_SCALE_500_DPS);
	PageCloseAnim(ANIM_RIGHT);
}
/**
  * @brief  页面循环执行的内容
  * @param  无
  * @retval 无
  */
static void Loop()
{
	if(PageExecuteRate(&Rate_125hz) == 1)
	{
		LSM6DSMReadGYRAndACC(&LSM6DSM);
		IMUupdate(&LSM6DSM);
	}
	if(PageExecuteRate(&Rate_1000hz) == 1)
	{
		static unsigned char t_cnt = 0;
		if(++t_cnt == (MAX_SNAKE_SPEED - snake_speed))
		{
			t_cnt = 0;
			game_loop = 1;
		}
	}
	if(game_loop)
 	{
		game_loop = 0;
		iWatchKeepActive();
		if(game_pause != 1)
		{
			ClearCache(main_cache);
			for(i = 0; i < PANEL_WIDTH; i++)	//画一个框
			{
				DrawDot(i, 0);
				DrawDot(i, PANEL_HEIGHT - 1);
			}
			for(i = 0; i < PANEL_HEIGHT; i++)
			{
				DrawDot(0, i);
				DrawDot(PANEL_WIDTH - 1, i);
			}
			ShowString(PANEL_WIDTH, 1, "Score:", main_cache, FONT6X8, NO_INVERSED);
			sprintf(str, "%3d", (int)snake_len);
			ShowString(PANEL_WIDTH, 2, str, main_cache, FONT6X8, NO_INVERSED);
			ShowString(PANEL_WIDTH, 5, "Best:", main_cache, FONT6X8, NO_INVERSED);
			if(snake_len > best_score)
				best_score = snake_len;
			sprintf(str, "%3d", (int)best_score);
			ShowString(PANEL_WIDTH, 6, str, main_cache, FONT6X8, NO_INVERSED);
			if(snake_direction == 0)
			{
				if(LSM6DSM.AngleY > CONTROL_SENSITIVITY)
					snake_direction = 1;
				else if(LSM6DSM.AngleY < -CONTROL_SENSITIVITY)
					snake_direction = 3;
			}
			else if(snake_direction == 1)
			{
				if(LSM6DSM.AngleX > CONTROL_SENSITIVITY)
					snake_direction = 0;
				else if(LSM6DSM.AngleX < -CONTROL_SENSITIVITY)
					snake_direction = 2;
			}
			else if(snake_direction == 2)
			{
				if(LSM6DSM.AngleY > CONTROL_SENSITIVITY)
					snake_direction = 1;
				else if(LSM6DSM.AngleY < -CONTROL_SENSITIVITY)
					snake_direction = 3;
			}
			else if(snake_direction == 3)
			{
				if(LSM6DSM.AngleX > CONTROL_SENSITIVITY)
					snake_direction = 0;
				else if(LSM6DSM.AngleX < -CONTROL_SENSITIVITY)
					snake_direction = 2;
			}
			//这段是按键程序解析后转换为方向以及点位置的程序，
			//sneak_direction是方向，test_x是头部的x轴值，test_y是头部的y轴值
			switch(snake_direction)
			{
				case 0:		//右
					snake_head_x++;
					break;
				case 1:		//下
					snake_head_y++;
					break;
				case 2:		//左
					snake_head_x--;
					break;
				case 3:		//上
					snake_head_y--;
					break;
			}
			if(snake_head_x >= PANEL_WIDTH)
				snake_head_x = 1;
			else if(snake_head_x <= 0)
				snake_head_x = PANEL_WIDTH - 1;
			if(snake_head_y >= PANEL_HEIGHT)
				snake_head_y = 1;
			else if(snake_head_y <= 0)
				snake_head_y = PANEL_HEIGHT - 1;
			//检查是否碰到自己的身体
			for(i = 0; i < snake_len; i++)
			{
				if((snake_head_x == past_x[i]) && (snake_head_y == past_y[i]))
				{
					game_over = 1;
					game_pause = 1;
					break;
				}
			}
			//这段是可以让贪吃蛇有可变长度尾巴的测试程序，test_num就是尾巴的数量，最大50，
			//程序原理是把每次过去的坐标都记录在数组里面。显示的时候就把过去的点也都显示出来
			test_now++;
			test_now = test_now % snake_len;
			past_x[test_now] = snake_head_x;
			past_y[test_now] = snake_head_y;
			for(i = 0; i < snake_len; i++)
			{
				DrawDot(past_x[i], past_y[i]);
			}
			//这段程序放被吃掉的小东西，设计delicious_x,delicious_y为食物的坐标
			//吃掉食物以后会放一个新的食物，并且test_num也就是尾巴的数量会大1
			if((snake_head_x <= delicious_x+1) && (snake_head_y <= delicious_y+1) && (snake_head_x >= delicious_x-1) && (snake_head_y>=delicious_y-1))
			{
				snake_len++;
				past_x[snake_len - 1] = past_x[snake_len - 2];//解决小白点的问题
				past_y[snake_len - 1] = past_y[snake_len - 2];
				Bee();
				delicious_x = rand() % 92;
				if(delicious_x > PANEL_WIDTH - 2)
					delicious_x = PANEL_WIDTH - 2;
				else if(delicious_x < 2)
					delicious_x  = 2;
				delicious_y = rand() % 64;
				if(delicious_y > PANEL_HEIGHT - 2)
					delicious_y = PANEL_HEIGHT - 2;
				else if(delicious_y < 2)
					delicious_y  = 2;
			}
			DrawDot(delicious_x,delicious_y);
			DrawDot(delicious_x+1,delicious_y);
			DrawDot(delicious_x-1,delicious_y);
			DrawDot(delicious_x,delicious_y+1);
			DrawDot(delicious_x+1,delicious_y+1);
			DrawDot(delicious_x-1,delicious_y+1);
			DrawDot(delicious_x,delicious_y-1);
			DrawDot(delicious_x+1,delicious_y-1);
			DrawDot(delicious_x-1,delicious_y-1);
			if(snake_len < 30)
				snake_speed = 10;
			else if(snake_len < 40)
				snake_speed = 20;
			else if(snake_len < 60)
				snake_speed = 30;
			else if(snake_len < 90)
				snake_speed = 35;
			else if(snake_len < 130)
				snake_speed = 40;
			else
				snake_speed = 45;
		}
		if(game_over)
		{
			ShowString(30, 2, "GAME", main_cache, FONT8X16, NO_INVERSED);
			ShowString(30, 4, "OVER", main_cache, FONT8X16, NO_INVERSED);
		}
		ScreenRefreshAll(main_cache);
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
		PageShift(page_menu);
	else if(event == KEY2)
	{
		if(game_over != 1)
		{
			if(game_pause)
				game_pause = 0;
			else
				game_pause = 1;
		}
	}
}

void PageRegister_page_snake(unsigned char pageID)
{
	PageRegister(pageID, Text, Icon, Setup, Loop, Exit, Event);
}