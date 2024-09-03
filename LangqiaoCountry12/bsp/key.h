#ifndef __KEY_H
#define __KEY_H
#include "main.h"

struct keys{
	uint8_t age;
	uint8_t press;
	uint8_t flag;
};

void key_serv(void);
void adc_read(ADC_HandleTypeDef* hadc);
void key_adc_serv(void);
#endif
