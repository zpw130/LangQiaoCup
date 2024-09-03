#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#include "main.h"

struct keys
{
	uchar key_stat;
	uchar judge_stat;
	bool single_flag;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif
