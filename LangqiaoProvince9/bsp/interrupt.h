#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#include "main.h"
struct keys
{
	bool key_stat;
	uchar judge_stat;
	bool single_flag;
	uint count;
	bool long_flag;
};
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif
