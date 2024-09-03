#include "interrupt.h"

struct keys Key[4]={0,0,0,0,0};

uchar view=0;
uint TIM7_count=0;
int freq=1000;
int duty=50;
uchar B1='@',B2='@',B3='@';
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM16)
	{
		Key[0].key_stat=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
		Key[1].key_stat=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
		Key[2].key_stat=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
		Key[3].key_stat=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		for(int i=0;i<4;i++)
		{
			switch(Key[i].judge_stat)
			{
				case 0:
				{
					if(Key[i].key_stat==0)
					{
						Key[i].judge_stat=1;
						Key[i].key_time=0;
					}
				}
				break;
				case 1:
				{
					if(Key[i].key_stat==0)
					{
						Key[i].judge_stat=2;
						Key[i].single_flag=1;
					}
					else
					{
						Key[i].judge_stat=0;
					}
				}
				break;
				case 2:
				{
					if(Key[i].key_stat==1)
					{
						Key[i].judge_stat=0;
						if(Key[i].key_time>100)
						{
							Key[i].long_flag=1;
						}
					}
					else
					{
						Key[i].key_time++;
					}
				}
				break;
			}
		}
	}
	if(htim->Instance==TIM7)
	{
		TIM7_count+=1;
		if(TIM7_count==50)
		{
			view=0;
			B1='@',B2='@',B3='@';
			freq=1000;
			duty=50;
			__HAL_TIM_SET_PRESCALER(&htim2,(80000000)/100/freq);
			__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,duty);
			HAL_TIM_Base_Stop_IT(&htim7);
			TIM7_count=0;
		}
	}
}

uchar rx;
char rx_data[10];
uchar rx_pointer;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_data[rx_pointer++]=rx;
	HAL_UART_Receive_IT(&huart1,&rx,1);
}
