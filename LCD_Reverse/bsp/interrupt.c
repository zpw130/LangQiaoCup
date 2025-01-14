#include "interrupt.h"

struct keys key[4] = {0,0,0,0,0};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		key[0].key_stat = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
		key[1].key_stat = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
		key[2].key_stat = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
		key[3].key_stat = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		
		for(int i=0;i<4;i++)
		{
			switch (key[i].judge_stat)
			{
				case 0:
				{
					if (key[i].key_stat == 0)
					{
						key[i].key_time=0;
						key[i].judge_stat=1;
					}
				}
				break;
				case 1:
				{
					if (key[i].key_stat == 0)
					{
						key[i].judge_stat=2;
					}
					else
					{
						key[i].judge_stat=0;
					}
				}
				break;
				case 2:
				{
					if (key[i].key_stat == 1)
					{
						key[i].judge_stat=0;
						if(key[i].key_time<80)
							key[i].single_flag=1;
					}
					else
					{
						key[i].key_time++;
						if(key[i].key_time>=80)
						{
							key[i].long_flag=1;
						}
					}
				}
				break;
			}
		}
	}
}
