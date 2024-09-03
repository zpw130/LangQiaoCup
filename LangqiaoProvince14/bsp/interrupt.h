#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include "main.h"
#include "tim.h"
struct keys
{
	uchar key_stat;
	uchar judge_stat;
	uchar single_flag;
	uchar long_flag;
	int key_time;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
#endif
