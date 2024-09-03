#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "main.h"

struct keys
{
	uchar judge_stat;
	uchar key_stat;
	uchar single_flag;
	uint key_time;
	uint long_flag;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif
