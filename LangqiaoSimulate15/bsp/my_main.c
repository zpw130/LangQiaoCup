#include "my_main.h"
extern struct keys Key[];

uint8_t disp;

char N='S';
uint8_t car_stat=0; //0 直行 1转弯 2偏航
float D=80.3;

uint8_t clear_flag;
char uart_rx[5];

void Data_proc(void)
{
	HAL_ADC_Start(&hadc2);
	float adc_val = HAL_ADC_GetValue(&hadc2)*3.3f/4096.f;
	if(adc_val>3) D=300;
	else D=adc_val*100;
}
void key_proc(void)
{
	char text[10];
	if(Key[1].flag)
	{
		if(car_stat==2)
		{
			N='S';
			car_stat=0;
			clear_flag=1;
			sprintf(text,"Success\r\n");
			HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
		}
		Key[1].flag=0;
	}
	if(Key[3].flag)
	{
		if(car_stat==1)
		{
			if(N=='L')
			{
				N='S';
				car_stat=0;
				sprintf(text,"Success\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),100);
			}
			else
			{
				sprintf(text,"Warn\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),100);
			}
		}
		else if(car_stat==0)
		{
			
			sprintf(text,"Warn\r\n");
			HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),100);
		}
		Key[3].flag=0;
	}
	if(Key[4].flag)
	{
		if(car_stat==1)
		{
			if(N=='R')
			{
				N='S';
				car_stat=0;
				sprintf(text,"Success\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
			}
			else
			{
				sprintf(text,"Warn\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
			}
		}
		else if(car_stat==0)
		{
			sprintf(text,"Warn\r\n");
			HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
		}
		Key[4].flag=0;
	}
}

void LCD_proc(void)
{
	char text[30];
	if(clear_flag)
	{
		LCD_Clear(Black);
		clear_flag=0;
	}
	if(car_stat!=2)
	{
		sprintf(text,"        DATA      ");
		LCD_DisplayStringLine(Line1,(unsigned char*)text);
		sprintf(text,"       N:%c      ",N);
		LCD_DisplayStringLine(Line3,(unsigned char*)text);
		sprintf(text,"       D:%.1f      ",D);
		LCD_DisplayStringLine(Line4,(unsigned char*)text);
	}
	else
	{
		sprintf(text,"        WARN      ");
		LCD_DisplayStringLine(Line5,(unsigned char*)text);
	}
}

void LED_disp(uint8_t disp)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,disp<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void LED_proc(void)
{
	if(N=='L' && car_stat==1) disp ^=(0x01);
	else disp &=~(0x01);
	
	if(N=='R' && car_stat==1) disp ^=(0x02);
	else disp &=~(0x02);
	
	if(car_stat==2) disp |=(0x01<<7);
	else disp &=~(0x01<<7);
	LED_disp(disp);
}

void setup(void)
{
//	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t*)uart_rx,5);
	HAL_TIM_Base_Start_IT(&htim6);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_DisplayStringLine(Line4, (unsigned char *)"    Hello,world.   ");
	LED_disp(0x00);
}

void loop(void)
{
	Data_proc();
	key_proc();
	LCD_proc();
	LED_proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
		key_serv();
	}
	if(htim->Instance==TIM7)
	{
		//进行5s计时
		static uint16_t count=0;
		if(car_stat==1)
		{
			count+=1;
			if(count==500) //时间超过5秒变成偏航
			{
				car_stat = 2;
				count=0;
				clear_flag=1;
				HAL_TIM_Base_Stop_IT(&htim7);
			}
		}
		else if(car_stat==0) //完成转弯
		{
			count = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
		}
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	char text[10];
	if(N != 'S')
	{
		sprintf(text,"WAIT\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),100);
	}
	else if(Size!=1)
	{
		sprintf(text,"ERROR\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),100);
	}
	else
	{
		if(uart_rx[0]=='L')
		{
			N = 'L';
			car_stat=1;
			HAL_TIM_Base_Start_IT(&htim7);
		}
		else if(uart_rx[0]=='R')
		{
			N = 'R';
			car_stat=1;
			HAL_TIM_Base_Start_IT(&htim7);
		}
		else
		{
			sprintf(text,"ERROR\r\n");
			HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),100);
		}
	}
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t*)uart_rx,5);
}
