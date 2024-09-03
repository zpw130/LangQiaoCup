/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "i2c_hal.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern struct keys Key[];
extern bool led[];
extern uchar view;
extern uint time[];
extern uint time_tick;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void key_proc(void);
void lcd_proc(void);
void led_proc(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int Alarm[3]={10,0,0};
int time_last[3];
int Alarm_last[3];
uchar time_choose=0;

int time_change,Alarm_change;
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();
	I2CInit();
	LED_Disp(0x00);
	HAL_TIM_Base_Start_IT(&htim16);
	//∂¡»°eeprom
	uchar temp=eeprom_read(15);
	if(temp!=7)
	{
		eeprom_write(15,7);
		HAL_Delay(10);
		eeprom_write(1,12);
		HAL_Delay(10);
		eeprom_write(2,0);
		HAL_Delay(10);
		eeprom_write(3,0);
		HAL_Delay(10);
		eeprom_write(4,0);
		HAL_Delay(10);
		eeprom_write(5,0);
		HAL_Delay(10);
	}
	else
	{
		Alarm[0]=eeprom_read(1);
		HAL_Delay(10);
		Alarm[1]=eeprom_read(2);
		HAL_Delay(10);
		Alarm[2]=eeprom_read(3);
		HAL_Delay(10);
		time_change=eeprom_read(4);
		HAL_Delay(10);
		Alarm_change=eeprom_read(5);
		HAL_Delay(10);
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);

  while (1)
  {
		key_proc();
		led_proc();
		lcd_proc();
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
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
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
void HighLight_disp(bool mod)
{
	if(mod==0)
	{
		LCD_DisplayChar(Line3,Column4,'R');
		LCD_DisplayChar(Line3,Column5,'T');
		LCD_DisplayChar(Line3,Column6,'C');
		LCD_DisplayChar(Line3,Column7,':');
		LCD_DisplayChar(Line3,Column10,':');
		LCD_DisplayChar(Line3,Column13,':');
		for(int i=0;i<3;i++)
		{
			if(i==time_choose) LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,Column8-3*i*16,(time[i]/10)+'0');
			LCD_DisplayChar(Line3,Column9-3*i*16,time[i]%10+'0');
			if(i==time_choose) LCD_SetTextColor(White);
		}
	}
	else
	{
		LCD_DisplayChar(Line3,Column2,'A');
		LCD_DisplayChar(Line3,Column3,'l');
		LCD_DisplayChar(Line3,Column4,'a');
		LCD_DisplayChar(Line3,Column5,'r');
		LCD_DisplayChar(Line3,Column6,'m');
		LCD_DisplayChar(Line3,Column7,':');
		LCD_DisplayChar(Line3,Column10,':');
		LCD_DisplayChar(Line3,Column13,':');
		for(int i=0;i<3;i++)
		{
			if(i==time_choose) LCD_SetTextColor(Red);
			LCD_DisplayChar(Line3,Column8-3*i*16,(Alarm[i]/10)+'0');
			LCD_DisplayChar(Line3,Column9-3*i*16,Alarm[i]%10+'0');
			if(i==time_choose) LCD_SetTextColor(White);
		}
	}
}
void lcd_proc(void)
{
	char text[30];
	if(view==0)
	{		
		sprintf(text,"       MAIN     ");
		LCD_DisplayStringLine(Line0,(unsigned char *)text);
		LCD_DisplayChar(Line3,Column4,'R');
		LCD_DisplayChar(Line3,Column5,'T');
		LCD_DisplayChar(Line3,Column6,'C');
		LCD_DisplayChar(Line3,Column7,':');
		LCD_DisplayChar(Line3,Column10,':');
		LCD_DisplayChar(Line3,Column13,':');
		for(int i=0;i<3;i++)
		{
			LCD_DisplayChar(Line3,Column8-3*i*16,(time[i]/10)+'0');
			LCD_DisplayChar(Line3,Column9-3*i*16,time[i]%10+'0');
		}
	}
	else if(view==1)
	{
		sprintf(text,"    RTC-SETTING     ");
		LCD_DisplayStringLine(Line0,(unsigned char *)text);
		HighLight_disp(0);
	}
	else
	{
		sprintf(text,"   ALARM-SETTING     ");
		LCD_DisplayStringLine(Line0,(unsigned char *)text);
		HighLight_disp(1);
	}
}

void key_proc(void)
{
	char text[30];
	if(Key[0].single_flag)
	{
		if(view==0) 
		{
			view=1;
			time_choose=0;
			LCD_Clear(Black);
			for(int i=0;i<3;i++) time_last[i]=time[i];
		}
		else if(view==1) 
		{
			view=0;
			LCD_Clear(Black);
			for(int i=0;i<3;i++)
			{
				if(time[i]!=time_last[i])
				{
					time_change+=1;
					eeprom_write(4,time_change);
					HAL_Delay(10);
					sprintf(text,"New RTC:%d%d:%d%d:%d%d\r\n",time[0]/10,time[0]%10,time[1]/10,time[1]%10,time[2]/10,time[2]%10);
					HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
					break;
				}
			}
		}
		else
		{
			time_choose+=1;
			if(time_choose==3) time_choose=0;
		}
		Key[0].single_flag=0;
	}
	if(Key[1].single_flag)
	{
		if(view==0) 
		{
			view=2;
			time_choose=0;
			LCD_Clear(Black);
			for(int i=0;i<3;i++) Alarm_last[i]=Alarm[i];
		}
		else if(view==2) 
		{
			view=0;
			LCD_Clear(Black);
			for(int i=0;i<3;i++)
			{
				if(Alarm_last[i]!=Alarm[i])
				{
					Alarm_change+=1;
					eeprom_write(1,Alarm[0]);
					HAL_Delay(10);
					eeprom_write(2,Alarm[1]);
					HAL_Delay(10);
					eeprom_write(3,Alarm[2]);
					HAL_Delay(10);
					eeprom_write(5,Alarm_change);
					HAL_Delay(10);
					sprintf(text,"New Alarm:%d%d:%d%d:%d%d\r\n",Alarm[0]/10,Alarm[0]%10,Alarm[1]/10,Alarm[1]%10,Alarm[2]/10,Alarm[2]%10);
					HAL_UART_Transmit(&huart1,(uint8_t*)text,strlen(text),50);
					break;
				}
			}
		}
		else
		{
			time_choose+=1;
			if(time_choose==3) time_choose=0;
		}
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag || Key[2].long_flag)
	{
		if(view==1)
		{
			time[time_choose]++;
			if(time[0]==24) time[0]=0;
			if(time[1]==60) time[1]=0;
			if(time[2]==60) time[2]=0;
			
		}
		else if(view==2)
		{
			Alarm[time_choose]++;
			if(Alarm[0]==24) Alarm[0]=0;
			if(Alarm[1]==60) Alarm[1]=0;
			if(Alarm[2]==60) Alarm[2]=0;
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag || Key[3].long_flag)
	{
		if(view==1)
		{
			time[time_choose]--;
			if(time[0]==-1) time[0]=23;
			if(time[1]==-1) time[1]=59;
			if(time[2]==-1) time[2]=59;
		}
		else if(view==2)
		{
			Alarm[time_choose]--;
			if(time[0]==-1) Alarm[0]=23;
			if(time[1]==-1) Alarm[1]=59;
			if(time[2]==-1) Alarm[2]=59;
		}
		Key[3].single_flag=0;
	}
}
void led_proc(void)
{
	for(int i=0;i<3;i++)
	{
		if(i!=view) led[i]=0;
		else led[i]=1;
	}
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
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
