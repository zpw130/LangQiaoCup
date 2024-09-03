#include "key.h"

struct keys Key[5];

uint8_t key_read(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0) return 1;
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0) return 2;
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0) return 3;
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0) return 4;
	else return 0;
}
void key_serv(void)
{
	uint8_t key_stat = key_read();
	if(key_stat)
	{
		Key[key_stat].age++;
		if(Key[key_stat].age>2) Key[key_stat].press=1;
	}
	else
	{
		for(int i=0;i<5;i++)
		{
			if(Key[i].press) Key[i].flag=1;
			Key[i].age=0;
			Key[i].press=0;
		}
	}
}
