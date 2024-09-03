#include "interrupt.h"

struct keys Key[4]={0,0,0};

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
					else Key[i].judge_stat=0;
				}
				break;
				case 2:
				{
					if(Key[i].key_stat==1)
					{
						Key[i].judge_stat=0;
					}
				}
				break;
			}
		}
	}
}

char rx_data[30];
uchar rx_pointer;
uint8_t rx;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_data[rx_pointer++]=rx;
	HAL_UART_Receive_IT(&huart1,&rx,1);
}

