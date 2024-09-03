#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#include "main.h"
#include "usart.h"
struct keys
{
	uchar key_stat;
	uchar judge_stat;
	uchar single_flag;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#endif
