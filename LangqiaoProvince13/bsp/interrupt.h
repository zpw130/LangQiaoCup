#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include "main.h"
#include "tim.h"
#include "usart.h"

struct keys
{
	uchar key_stat;
	uchar judge_stat;
	uint key_time;
	uchar single_flag;
	uchar long_flag;
};

#endif
