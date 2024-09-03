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
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "interrupt.h"
#include "adc_operate.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void lcd_proc(void);
void key_proc(void);
void GetDuty(void);
void Led_Proc(void);

extern struct keys Key[];
extern int freq;
extern uchar Change_Mod;
extern uchar disp;
extern uchar led2_on;
extern double MH,ML,V;

uchar view=0;  //0:数据页面 1:参数页面 2:统计页面
extern char mod;
extern uchar R,K; //参数
uchar choose='R'; //改变的参数
uchar lock=0; //是否锁定duty

uchar duty=10; //输出的占空比
int N = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
  MX_ADC2_Init();
  MX_TIM17_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LED_Disp(0x00);
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		GetDuty();
		key_proc();
		lcd_proc();
		Led_Proc();
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
void GetDuty(void)
{
	if(!lock)
	{
		double V = GetADCValue(&hadc2);
		if(V<=1) duty=10;
		else if(V>3) duty=85;
		else duty=(uchar)(37.5*V-27.5);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,duty);
	}
}
void key_proc(void)
{
	if(Key[0].single_flag==1)
	{
		view++;
		if(view==3) view=0;
		if(view==1) choose='R';
		Key[0].single_flag=0;
		LCD_Clear(Black);
	}
	if(Key[1].single_flag==1)
	{
		if(view==0) //切换模式
		{
			if(Change_Mod) //在5秒外
			{
				if(mod=='H')
				{
					mod='L';
				}
				else
				{
					mod='H';
				}
				N+=1;
				Change_Mod = 0; //禁止改变
				HAL_TIM_Base_Start_IT(&htim6); //计时5秒
			}
		}
		if(view==1) //切换选择常数
		{
			if(choose=='R')
			{
				choose='K';
			}
			else
			{
				choose='R';
			}
		}
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag==1)
	{
		if(view==1) //加键
		{
			if(choose=='R')
			{
				R+=1;
				if(R==11) R=1;
			}
			else
			{
				K+=1;
				if(K==11) K=1;
			}
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag==1)
	{
		if(view==0)
		{
			if(lock) //如果锁定duty,解锁
			{
				lock=!lock;
			}
		}
		if(view==1) //减键
		{
			if(choose=='R')
			{
				R-=1;
				if(R==0) R=10;
			}
			else
			{
				K-=1;
				if(K==0) K=10;
			}
		}
		Key[3].single_flag=0;
	}
	if(Key[3].long_flag==1) //锁定键
	{
		if(view==0)
		{
			lock=1;
		}
		Key[3].long_flag=0;
	}
}
void lcd_proc(void)
{
	char text[30];
	if (view==0)
	{
		sprintf(text,"        DATA    ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"     M=%c    ",mod);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     P=%d%c    ",duty,'%');
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"     V=%.1f    ",V);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
		
//		sprintf(text,"     ADC=%f    ",GetADCValue(&hadc2));
//		LCD_DisplayStringLine(Line7, (unsigned char *)text);
//		sprintf(text,"     freq=%d    ",freq);
//		LCD_DisplayStringLine(Line7, (unsigned char *)text);
//		sprintf(text,"     ChangeMod=%d    ",Change_Mod);
//		LCD_DisplayStringLine(Line7, (unsigned char *)text);
	}
	else if(view==1)
	{
		sprintf(text,"        PARA    ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"     R=%d    ",R);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     K=%d    ",K);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"        RECD    ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"     N=%d    ",N);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     MH=%.1f    ",MH);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"     ML=%.1f    ",ML);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
}

void Led_Proc(void)
{
	if(view==0) disp|=0x01;
	else disp&=~0x01;
	led2_on=!Change_Mod;
	if(lock) disp|=0x04;
	else disp&=~0x04;
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
