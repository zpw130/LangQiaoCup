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
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include "interrupt.h"
#include "stdio.h"
#include "adc_operate.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern struct keys Key[];
extern uchar Upper_led,Lower_led;
extern bool upper_led_on,lower_led_on;
extern uchar disp;
extern bool Clear;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uchar view=0;

float Volt=3.22;
int Max_Volt=24,Min_Volt=12;
uchar Choose=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void key_proc(void);
void lcd_proc(void);
void led_proc(void);
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
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();
	HAL_TIM_Base_Start_IT(&htim16);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  LCD_Clear(White);
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);
	LED_Disp(0x00);
  while (1)
  {
		Volt = Get_ADC_Value(&hadc2);
		key_proc();
		lcd_proc();
		led_proc();
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
void key_proc()
{
	if(Key[0].single_flag)
	{
		view+=1;
		if(view==2) view=0;
		LCD_Clear(White);
		Key[0].single_flag=0;
	}
	if(Key[1].single_flag)
	{
		if(view==1) Choose++;
		if(Choose==4) Choose=0;
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag)
	{
		if(Choose==0)
		{
			if(Max_Volt<=30) Max_Volt+=3;
		}
		if(Choose==1)
		{
			if(Min_Volt<=30) Min_Volt+=3;
			if(Min_Volt==Max_Volt) Min_Volt-=3;
		}
		if(Choose==2)
		{
			uchar temp=Upper_led;
			if(Upper_led<8) Upper_led+=1;
			if(Upper_led==Lower_led) Upper_led+=1;
			if(Upper_led==9) Upper_led=7;
			if(temp!=Upper_led) Clear=1;
		}
		if(Choose==3)
		{
			uchar temp=Lower_led;
			if(Lower_led<8) Lower_led+=1;
			if(Upper_led==Lower_led) Lower_led+=1;
			if(Lower_led==9) Lower_led=7;
			if(temp!=Lower_led) Clear=1;
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag)
	{
		if(Choose==0)
		{
			if(Max_Volt>0) Max_Volt-=3;
			if(Min_Volt>=Max_Volt) Max_Volt+=3;
		}
		if(Choose==1)
		{
			if(Min_Volt>0) Min_Volt-=3;
		}
		if(Choose==2)
		{
			uchar temp=Upper_led;
			if(Upper_led>1) Upper_led-=1;
			if(Upper_led==Lower_led) Upper_led-=1;
			if(Upper_led==0) Upper_led=2;
			if(temp!=Upper_led) Clear=1;
			
		}
		if(Choose==3)
		{
			uchar temp=Lower_led;
			if(Lower_led>1) Lower_led-=1;
			if(Upper_led==Lower_led) Lower_led-=1;
			if(Lower_led==0) Lower_led=2;
			if(temp!=Lower_led) Clear=1;
		}
		Key[3].single_flag=0;
	}
}
void lcd_proc()
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"        Main    ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"    Volt:%.2fV         ",Volt);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		if(Volt*10>Max_Volt)
		{
			sprintf(text,"    Status:Upper       ");
			LCD_DisplayStringLine(Line5, (unsigned char *)text);
		}
		else if(Volt*10<Min_Volt)
		{
			sprintf(text,"    Status:Lower       ");
			LCD_DisplayStringLine(Line5, (unsigned char *)text);
		}
		else
		{
			sprintf(text,"    Status:Normal       ");
			LCD_DisplayStringLine(Line5, (unsigned char *)text);
		}
		sprintf(text,"    disp:%d        ",disp);
		LCD_DisplayStringLine(Line7, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"       Setting    ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		if(Choose==0) LCD_SetBackColor(Green);
		sprintf(text,"    Max Volt:%.1fV     ",(double)Max_Volt/10);
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		if(Choose==0) LCD_SetBackColor(White);
		if(Choose==1) LCD_SetBackColor(Green);
		sprintf(text,"    Min Volt:%.1fV    ",(double)Min_Volt/10);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		if(Choose==1) LCD_SetBackColor(White);
		if(Choose==2) LCD_SetBackColor(Green);
		sprintf(text,"    Upper:LD%d        ",Upper_led);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
		if(Choose==2) LCD_SetBackColor(White);
		if(Choose==3) LCD_SetBackColor(Green);
		sprintf(text,"    Lower:LD%d        ",Lower_led);
		LCD_DisplayStringLine(Line8, (unsigned char *)text);
		if(Choose==3) LCD_SetBackColor(White);
	}
}

void led_proc(void)
{
	if(Volt*10>Max_Volt)
	{
		upper_led_on=1;
	}
	else upper_led_on=0;
	if(Volt*10<Min_Volt)
	{
		lower_led_on=1;
	}
	else lower_led_on=0;
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
