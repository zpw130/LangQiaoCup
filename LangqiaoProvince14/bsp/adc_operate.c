#include "adc_operate.h"
double GetADCValue(ADC_HandleTypeDef* pin)
{
	unsigned int value = 0,i = 0;
	HAL_ADC_Start(pin);
	for(i=0;i<10;++i)
	{
	  HAL_ADC_PollForConversion(pin,10);
		value += HAL_ADC_GetValue(pin);
	}
	return value/10*3.3/4096;

}
