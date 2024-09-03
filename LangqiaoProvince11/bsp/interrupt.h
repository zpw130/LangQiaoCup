#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#include "main.h"

struct keys
{
	uchar key_stat;
	uchar judge_stat;
	uint count;
	bool single_flag;
	bool long_flag;
};

#endif
