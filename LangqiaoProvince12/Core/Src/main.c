/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include "interrupt.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern struct keys Key[];
extern uchar led1_on;
extern uchar led2_on;
extern char rx_data[];
extern uchar rx_pointer;
extern uint8_t rx;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void key_proc(void);
void lcd_proc(void);
void led_proc(void);
void UART_proc(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uchar view=0;
uchar PWM_Mod=0;

uchar CNBR=0,VNBR=0,IDLE=8;
double CNBR_fee=3.50,VNBR_fee=2.00;

struct Part_data
{
	char car_type[5];
	char stop_type[5];
	char time[13];
	char flag; //标志是否被占用
}Car_Part[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	HAL_UART_Receive_IT(&huart1,&rx,1);
	
	LCD_Init();
	LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
	
	LED_Disp(0x00);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		key_proc();
		led_proc();
		lcd_proc();
		if(rx_pointer!=0)
		{
			uchar temp=rx_pointer;
			HAL_Delay(1);
			if(temp==rx_pointer) UART_proc();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void key_proc(void)
{
	if(Key[0].single_flag==1)
	{
		view++;
		if(view==2) view=0;
		LCD_Clear(Black);
		Key[0].single_flag=0;
	}
	if(Key[1].single_flag==1)
	{
		CNBR_fee+=0.5;
		VNBR_fee+=0.5;
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag==1)
	{
		CNBR_fee-=0.5;
		VNBR_fee-=0.5;
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag==1)
	{
		PWM_Mod=~PWM_Mod;
		if(PWM_Mod)
		{
			__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,20);
		}
		else
		{
			__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,0);
		}
		Key[3].single_flag=0;
	}
}

void lcd_proc(void)
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"       Data      ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"   CNBR:%d      ",CNBR);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"   VNBR:%d      ",VNBR);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
		sprintf(text,"   IDLE:%d      ",IDLE);
		LCD_DisplayStringLine(Line8, (unsigned char *)text);
		
		
//		sprintf(text,"StopType:%s      ",stop_type);
//		LCD_DisplayStringLine(Line5, (unsigned char *)text);
//		sprintf(text,"CarType:%s      ",car_type);
//		LCD_DisplayStringLine(Line7, (unsigned char *)text);
//		sprintf(text,"Time:%s      ",time);
//		LCD_DisplayStringLine(Line9, (unsigned char *)text);
		
	}
	else if(view==1)
	{
		sprintf(text,"       Para      ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"   CNBR:%.2f      ",CNBR_fee);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"   VNBR:%.2f      ",VNBR_fee);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
	
}
void led_proc(void)
{
	if(IDLE) led1_on=1;
	else led1_on=0;
	if(PWM_Mod) led2_on=1;
	else led2_on=0;
}

bool Judge_Time_Legal(char *time)
{
	//判断时间是否错误
	for(int i=0;i<12;i++)
	{
		if(time[i]<'0' || time[i]>'9')
		{
			return 0;
		}
	}
	int year=(time[0]-'0')*10+(time[1]-'0')+2000;
	int mouth=(time[2]-'0')*10+(time[3]-'0');
	int day=(time[4]-'0')*10+(time[5]-'0');
	int hour=(time[6]-'0')*10+(time[7]-'0');
	int minute=(time[8]-'0')*10+(time[9]-'0');
	int second=(time[10]-'0')+(time[11]-'0');
	//判断时间是否错误
	if(hour>24 || minute>60 || second>60 || mouth>12)
		return 0;
	//判断日期是否错误
	if(mouth==1 || mouth==3 || mouth==5 || mouth==7 || mouth==8 || mouth==10 || mouth==12)
	{
		if(day>=1 && day<=31)
			return 1;
		else 
			return 0;
	}
	else if(mouth==4 || mouth==6 || mouth==9 || mouth==11)
	{
		if(day>=1 && day<=30)
			return 1;
		else 
			return 0;
	}
	else if((year%100!=0 && year%4==0) || year%400==0)
	{
		if(day>=1 && day<=29)
			return 1;
		else 
			return 0;
	}
	else
	{
		if(day>=1 && day<=28)
			return 1;
		else 
			return 0;
	}
}
int Calculate_Time_diff(char* time1,char* time2)
{
//	int year,mouth,day;
	int hour,minute,seconds;
//	year=(time1[0]-time2[0])*10+(time1[1]-time2[1]);
//	mouth=(time1[2]-time2[2])*10+(time1[3]-time2[3]);
//	day=(time1[4]-time2[4])*10+(time1[5]-time2[5]);
	hour=(time1[6]-time2[6])*10+(time1[7]-time2[7]);
	minute=(time1[8]-time2[8])*10+(time1[9]-time2[9]);
	seconds=(time1[10]-time2[10])*10+(time1[11]-time2[11]);
	
	int time_diff =hour*3600+minute*60+seconds;
	if(time_diff<0)
		return -1;
	int hour_diff=time_diff/3600;
	return hour_diff + (hour_diff*3600<time_diff);
}
void UART_proc(void)
{
	char car_type[5];
	char stop_type[5];
	char time[13];
	char text[30];
	uchar flag=1;
	uchar count=0;
	if(rx_pointer==22)
	{
		count=sscanf(rx_data,"%4s:%4s:%12s",stop_type,car_type,time);
		if(count!=3) flag=0;
		else
		{
			if(strcmp(stop_type,"CNBR")!=0 && strcmp(stop_type,"VNBR")!=0) //若不属于两种类型，则报错
			{
				flag=0;
			}
			if(!Judge_Time_Legal(time)) //若日期错误则报错
			{
				flag=0;
			}
		}
	}
	else flag=0;
	if(flag)
	{
		//检查是否为出站
		uchar out=0;
		for(int i=0;i<8;i++)
		{
			if(Car_Part[i].flag && strcmp(Car_Part[i].car_type,car_type)==0 && strcmp(Car_Part[i].stop_type,stop_type)==0)
			{
				int time_diff=Calculate_Time_diff(time,Car_Part[i].time);
				if(time_diff==-1)
				{
					sprintf(text,"Error\r\n");
					HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
				}
				else
				{
					sprintf(text,"%4s:%4s:%12s\r\n",Car_Part[i].stop_type,Car_Part[i].car_type,Car_Part[i].time);
					HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
					
					sprintf(text,"%4s:%4s:%12s\r\n",stop_type,car_type,time);
					HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
					if(strcmp(stop_type,"VNBR")==0)
					{
						sprintf(text,"%4s:%4s:%d:%.2f \r\n",Car_Part[i].stop_type,Car_Part[i].car_type,time_diff,time_diff*VNBR_fee);
						HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
						VNBR-=1;
					}
					else
					{
						sprintf(text,"%4s:%4s:%d:%.2f \r\n",Car_Part[i].stop_type,Car_Part[i].car_type,time_diff,(float)time_diff*CNBR_fee);
						HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
						CNBR-=1;
					}
					Car_Part[i].flag=0; //成功出站
					IDLE+=1;
					out=1;
					break;
				}
			}
		}
		if(out==0)
		{
			if(IDLE==0) //没有位置了
			{
				sprintf(text,"Error\r\n");
				HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
			}
			else //入站
			{
				for(int i=0;i<8;i++)
				{
					if(Car_Part[i].flag==0) 
					{
						strcpy(Car_Part[i].car_type,car_type);
						strcpy(Car_Part[i].stop_type,stop_type);
						strcpy(Car_Part[i].time,time);
						Car_Part[i].flag=1;
						IDLE-=1;
						if(strcmp(stop_type,"VNBR")==0) VNBR+=1;
						else CNBR+=1;
						break;
					}
				}
			}
		}
	}
	else
	{
		sprintf(text,"Error\r\n");
		HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
	}
	rx_pointer=0;
	memset(rx_data,0,30);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
