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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include "interrupt.h"
#include "stdio.h"
#include "i2c_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern struct keys Key[];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void key_proc(void);
void lcd_proc(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uchar Choose=0;
uchar Mod=0; //0:Standby 1:Setting 2:Running 3:Pause 
uchar Time_set; //0:hour 1:minute 2:second

struct times
{
	uchar hour;
	uchar minute;
	uchar second;
}Time[5]={0,1,0};
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
	LED_Disp(0x00);
	
	//数据初始化
	uchar temp=eeprom_read(114);
	HAL_Delay(10);
	if(temp!=15)
	{
		eeprom_write(114,15);
		HAL_Delay(10);
		for(int i=0;i<5;i++)
		{
			eeprom_write(3*i,Time[i].hour);
			HAL_Delay(10);
			eeprom_write(3*i+1,Time[i].minute);
			HAL_Delay(10);
			eeprom_write(3*i+2,Time[i].second);
			HAL_Delay(10);
		}
	}
	else
	{
		for(int i=0;i<5;i++)
		{
			Time[i].hour=eeprom_read(3*i);
			HAL_Delay(10);
			Time[i].minute=eeprom_read(3*i+1);
			HAL_Delay(10);
			Time[i].second=eeprom_read(3*i+2);
			HAL_Delay(10);
		}
	}
  while (1)
  {
		key_proc();
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
void key_proc(void)
{
	if(Key[0].single_flag)
	{
		Choose++;
		if(Choose==5) Choose=0;
		Key[0].single_flag=0;
	}
	if(Key[1].single_flag)
	{
		if(Mod!=1)
		{
			Mod=1;
			Time_set=0;
		}
		else
		{
			Time_set++;
			if(Time_set==3) Time_set=0;
		}
		Key[1].single_flag=0;
	}
	if(Key[1].long_flag) //保存
	{
		eeprom_write(Choose*3,Time[Choose].hour);
		HAL_Delay(10);
		eeprom_write(Choose*3+1,Time[Choose].minute);
		HAL_Delay(10);
		eeprom_write(Choose*3+2,Time[Choose].second);
		HAL_Delay(10);
		Mod=0;
	}
	if(Key[2].single_flag || Key[2].long_flag)
	{
		if(Mod==1)
		{
			if(Time_set==0)
			{
				Time[Choose].hour++;
				if(Time[Choose].hour==24) Time[Choose].hour=0;
			}
			else if(Time_set==1)
			{
				Time[Choose].minute++;
				if(Time[Choose].minute==60) Time[Choose].minute=0;
			}
			else
			{
				Time[Choose].second++;
				if(Time[Choose].second==60) Time[Choose].second=0;
			}
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag)
	{
		if(Mod!=2) Mod=2;
		else if(Mod==2) Mod=3;
		Key[3].single_flag=0;
	}
	if(Key[3].long_flag)
	{
		if(Mod==2 || Mod==3) Mod=0;
	}
}
void lcd_proc(void)
{
	char text[30];
	sprintf(text,"   No %d",Choose+1);
	LCD_DisplayStringLine(Line2, (unsigned char *)text);
	LCD_DisplayChar(Line4,Column9,':');
	LCD_DisplayChar(Line4,Column12,':');
	
	if(0==Time_set && Mod==1) LCD_SetTextColor(Red);
	LCD_DisplayChar(Line4,Column7,Time[Choose].hour/10+'0');
	LCD_DisplayChar(Line4,Column8,Time[Choose].hour%10+'0');
	if(0==Time_set && Mod==1) LCD_SetTextColor(White);
	if(1==Time_set && Mod==1) LCD_SetTextColor(Red);
	LCD_DisplayChar(Line4,Column10,Time[Choose].minute/10+'0');
	LCD_DisplayChar(Line4,Column11,Time[Choose].minute%10+'0');
	if(1==Time_set && Mod==1) LCD_SetTextColor(White);
	if(2==Time_set && Mod==1) LCD_SetTextColor(Red);
	LCD_DisplayChar(Line4,Column13,Time[Choose].second/10+'0');
	LCD_DisplayChar(Line4,Column14,Time[Choose].second%10+'0');
	if(2==Time_set && Mod==1) LCD_SetTextColor(White);
	
	if(Mod==0)
	{
		sprintf(text,"       Standby      ");
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
	else if(Mod==1)
	{
		sprintf(text,"       Setting      ");
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
	else if(Mod==2)
	{
		sprintf(text,"       Running      ");
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"        Pause      ");
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
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
