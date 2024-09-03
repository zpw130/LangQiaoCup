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
#include "stdio.h"
#include "led.h"
#include "interrupt.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern struct keys Key[];
extern uint16_t led_tick;
extern char rx_data[];
extern uchar rx_pointer;
extern uchar rx;

extern uchar view;
extern uchar B1,B2,B3;;
extern int freq;
extern int duty;

extern uchar led_on[];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void key_proc(void);
void lcd_proc(void);
void uart_rx_proc(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


char password[4]="123"; 
uchar failtime=0;



char old_password[4];
char new_password[4];
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
  MX_TIM2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
	LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
	led_disp(0x00);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_UART_Receive_IT(&huart1,&rx,1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		key_proc();
		lcd_proc();
		if(rx_pointer!=0)
		{
			int temp=rx_pointer;
			HAL_Delay(1);
			if(temp==rx_pointer) uart_rx_proc();
		}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
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
	if(view==0)
	{
		if(Key[0].single_flag==1)
		{
			if(B1=='@') B1='0';
			else B1+=1;
			if(B1=='9'+1) B1='0';
			Key[0].single_flag=0;
		}
		if(Key[1].single_flag==1)
		{
			if(B2=='@') B2='0';
			else B2+=1;
			if(B2=='9'+1) B2='0';
			Key[1].single_flag=0;
		}
		if(Key[2].single_flag==1)
		{
			if(B3=='@') B3='0';
			else B3+=1;
			if(B3=='9'+1) B3='0';
			Key[2].single_flag=0;
		}
		if(Key[3].single_flag==1)
		{
			if(B1==password[0] && B2==password[1] && B3==password[2])
			{
				view+=1;
				LCD_Clear(Black);
				led_on[0]=1;
				
				//设置PWM输出
				freq=2000;
				duty=10;
				__HAL_TIM_SET_PRESCALER(&htim2,(80000000)/100/freq);
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,duty);
				HAL_TIM_Base_Start_IT(&htim7); //开始5s计时
			}
			else
			{
				B1='@';B2='@';B3='@';
				failtime+=1;
				if(failtime>=3)
				{
					led_on[1]=1;
				}
			}
			Key[3].single_flag=0;
		}
	}
}

void lcd_proc(void)
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"       PSD    ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"    B1=%c    ",B1);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"    B2=%c    ",B2);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"    B3=%c    ",B3);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
//		sprintf(text,"    oldpass=%s      ",old_password);
//		LCD_DisplayStringLine(Line7, (unsigned char *)text);
//		sprintf(text,"    newpass=%s     ",new_password);
//		LCD_DisplayStringLine(Line8, (unsigned char *)text);
		sprintf(text,"    pass=%s      ",password);
		LCD_DisplayStringLine(Line7, (unsigned char *)text);
	}
	else if(view==1)
	{
		sprintf(text,"       STA    ");
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"    F=%d    ",freq);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"    D=%d    ",duty);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
}

void uart_rx_proc(void)
{
	if(rx_pointer>0)
	{
		if(rx_pointer==7)
		{
			sscanf(rx_data,"%3s-%3s",old_password,new_password);
			if(strcmp(old_password,password)==0)
			{
				int flag=1;
				for(int i=0;i<3;i++)
				{
					if(new_password[i]<'1' || new_password[i]>'9')
					{
						flag=0;
					}
				}
				if(flag)
				{
					for(int i=0;i<3;i++)
					{
						password[i]=new_password[i];
					}
				}
			}
		}
	}
	rx_pointer=0;
	memset(rx_data,0,20);
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
