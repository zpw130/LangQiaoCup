#include "my_main.h"
extern struct keys Key[];
uint8_t view;

float V1=0,V2=0;
uint32_t F1=2030,F2=300;

uint8_t VD=1,FD=2;
uint8_t VD_last,FD_last;

uint16_t adc2_val[2];

uint8_t disp;
uint8_t follow_mod;
void Data_proc(void)
{
	V1 = adc2_val[0]*3.3f/4096.f;
	V2 = adc2_val[1]*3.3f/4096.f;
	
	F1 = 1000000/(HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2)+1);
	F2 = 1000000/(HAL_TIM_ReadCapturedValue(&htim15,TIM_CHANNEL_1)+1);
	
	int counter;
	if(follow_mod == 0) counter = 10000000/F1 - 1;
	else counter = 10000000/F2 - 1;
	__HAL_TIM_SetAutoreload(&htim3,counter);
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,(counter+1)/2);
}

void Key_proc(void)
{
	if(Key[1].flag)
	{
		view+=1;
		if(view>1) view=0;
		LCD_Clear(Black);
		if(view==1)
		{
			VD_last = VD;
			FD_last = FD;
		}
		else
		{
			VD = VD_last;
			FD = FD_last;
		}
		Key[1].flag=0;
	}
	if(Key[2].flag)
	{
		if(view==1)
		{
			VD_last+=1;
			if(VD_last>8) VD_last=1;
			if(VD_last==FD_last) VD_last+=1;
			if(VD_last>8) VD_last=1;
		}
		Key[2].flag=0;
	}
	if(Key[3].flag)
	{
		if(view==1)
		{
			FD_last+=1;
			if(FD_last>8) FD_last=1;
			if(FD_last==VD_last) FD_last+=1;
			if(FD_last>8) FD_last=1;
		}
		Key[3].flag=0;
	}
	if(Key[4].flag)
	{
		follow_mod+=1;
		if(follow_mod>1) follow_mod=0;
		Key[4].flag=0;
	}
}

void LCD_proc(void)
{
	char text[25];
	if(view==0)
	{
		sprintf(text,"    DATA     ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"    V1:%.1fV     ",V1);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"    V2:%.1fV     ",V2);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"    F1:%dHz     ",F1);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"    F2:%dHz     ",F2);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"    PARA     ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"    VD:LD%d     ",VD_last);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"    FD:LD%d     ",FD_last);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
	}
}

void LED_Disp(uint8_t disp)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,disp<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void LED_proc(void)
{
	disp = 0x00;
	if(V1>V2) disp |= (0x01<<(VD-1));
	else disp &= ~(0x01<<(VD-1));
	if(F1>F2) disp |= (0x01<<(FD-1));
	else disp &= ~(0x01<<(FD-1));
	LED_Disp(disp);
}

void setup(void)
{
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim15,TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&htim15,TIM_CHANNEL_2);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2_val,2);
	HAL_TIM_Base_Start_IT(&htim6);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);

}
void loop(void)
{
	Data_proc();
	Key_proc();
	LCD_proc();
	LED_proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
		key_serv();
	}
}
