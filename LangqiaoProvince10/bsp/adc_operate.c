#include "adc_operate.h"

double Get_ADC_Value(ADC_HandleTypeDef* pin)
{
	int adc;
	HAL_ADC_Start(pin);
	adc = HAL_ADC_GetValue(pin);
	return adc*3.3/4096;
}
