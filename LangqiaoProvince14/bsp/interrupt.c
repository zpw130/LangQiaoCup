#include "interrupt.h"

int count,freq,freq_last;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM17)
	{
		count = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		__HAL_TIM_SetCounter(htim,0); //½«¼ÆÊýÆ÷ÖÃ0
		freq = (80000000/80)/count;
	}
}

struct keys Key[4]={0,0,0,0,0};

int count_TIM6=0,count_TIM7=0;
uchar Change_Mod = 1;
char mod = 'L';
uint freq_Out=4000;
uint prescaler=200;

uchar R=1,K=1;
double MH,ML;
double V;
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
						if(Key[i].key_time>200)
						{
							Key[i].long_flag=1;
						}
						Key[i].judge_stat=0;
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
	if(htim->Instance==TIM6)
	{
		if(count_TIM6<500)
		{
			count_TIM6++;
			if(count_TIM6%10==0)
			{
				if(mod=='H') freq_Out += 80;
				else freq_Out-=80;
				prescaler = (80000000/100)/freq_Out;
				__HAL_TIM_SET_PRESCALER(&htim2,prescaler);
			}
		}
		else
		{
			count_TIM6=0;
			Change_Mod = 1;
			HAL_TIM_Base_Stop_IT(htim);
		}
	}
	
	if(htim->Instance==TIM7)
	{
		if(freq_last!=freq)
		{
			freq_last=freq;
			V=freq*2*3.14*R/(100*K);
			count_TIM7=0;
		}
		else
		{
			count_TIM7+=1;
			if(count_TIM7>=20)
			{
				if(mod=='H'&&MH<V)
				{
					MH=V;
				}
				else if(mod=='L'&&ML<V)
				{
					ML=V;
				}
			}
		}
	}
}


