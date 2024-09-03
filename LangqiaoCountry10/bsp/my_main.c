#include "my_main.h"
extern struct keys Key[];

uint8_t view;

float AO1=2.12,AO2=3.00;
int PWM2=10;
float temp = 26.56;
uint16_t N = 0;

uint8_t T=30; //20-40
uint8_t X=1;
uint8_t T_temp;
uint8_t X_temp;

uint8_t Para_choose;
uint16_t adc2_rx[2];



void Data_Proc(void)
{
//	HAL_ADC_Start(&hadc2);
//	AO1 = HAL_ADC_GetValue(&hadc2)*3.3f/4096.f;
//	HAL_ADC_Start(&hadc2);
//	AO2 = HAL_ADC_GetValue(&hadc2)*3.3f/4096.f;
	AO1 = adc2_rx[0]*3.3f/4096.f;
	AO2 = adc2_rx[1]*3.3f/4096.f;
	
	PWM2 = (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1)+1)*100/(HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2)+1);
}

void Key_Proc(void)
{
	if(Key[1].single_flag)
	{
		view+=1;
		if(view>1) view=0;
		if(view==1)
		{
			T_temp=T;
			X_temp=X;
		}
		else
		{
			if(T_temp!=T || X_temp!=X)
			{
				N+=1;
				//保存eeprom
				T = T_temp;
				X = X_temp;
				eeprom_write(0,N>>8);
				HAL_Delay(10);
				eeprom_write(1,N&0xFF);
			}
		}
		Para_choose=0;
		LCD_Clear(Black);
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag)
	{
		if(view==1) Para_choose+=1;
		if(Para_choose>1) Para_choose=0;
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag)
	{
		if(view==1)
		{
			if(Para_choose==0)
			{
				T_temp+=1;
				if(T>40) T_temp=40;
			}
			else
			{
				X_temp+=1;
				if(X>2) X_temp=1;
			}
		}
		Key[3].single_flag=0;
	}
	if(Key[3].long_flag)
	{
		if(view==1 && Para_choose==0)
		{
			T_temp+=1;
			if(T>40) T_temp=40;
		}
	}
	if(Key[4].single_flag)
	{
		if(view==1)
		{
			if(Para_choose==0)
			{
				T_temp-=1;
				if(T_temp<20) T_temp=20;
			}
			else
			{
				X_temp+=1;
				if(X_temp>2) X_temp=1;
			}
		}
		Key[4].single_flag=0;
	}
	if(Key[4].long_flag)
	{
		if(view==1 && Para_choose==0)
		{
			T_temp-=1;
			if(T<20) T_temp=20;
		}
	}
}

void LCD_Proc(void)
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"       Main      ");
		LCD_DisplayStringLine(Line1,(unsigned char *)text);
		sprintf(text,"  A01:%.2fV      ",AO1);
		LCD_DisplayStringLine(Line2,(unsigned char *)text);
		sprintf(text,"  A02:%.2fV      ",AO2);
		LCD_DisplayStringLine(Line3,(unsigned char *)text);
		sprintf(text,"  PWM2:%d%%      ",PWM2);
		LCD_DisplayStringLine(Line4,(unsigned char *)text);
		sprintf(text,"  Temp:%.2fC      ",temp);
		LCD_DisplayStringLine(Line5,(unsigned char *)text);
		sprintf(text,"  N:%d      ",N);
		LCD_DisplayStringLine(Line6,(unsigned char *)text);
	}
	else
	{
		sprintf(text,"       Para      ");
		LCD_DisplayStringLine(Line1,(unsigned char *)text);
		if(Para_choose==0) LCD_SetBackColor(Yellow);
		sprintf(text,"  T:%d                  ",T_temp);
		LCD_DisplayStringLine(Line2,(unsigned char *)text);
		if(Para_choose==0) LCD_SetBackColor(Black);
		if(Para_choose==1) LCD_SetBackColor(Yellow);
		sprintf(text,"  X:A0%d                ",X_temp);
		LCD_DisplayStringLine(Line3,(unsigned char *)text);
		if(Para_choose==1) LCD_SetBackColor(Black);
	}
}

void setup(void)
{
	HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2_rx,2);
	I2CInit();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	//判断是否首次开机
	uint8_t temp1 = eeprom_read(2);
	HAL_Delay(10);
	uint8_t temp2 = eeprom_read(3);
	HAL_Delay(10);
	
	if(temp1==7 && temp2 ==10)
	{
		N = (eeprom_read(0)<<8) + eeprom_read(1);
	}
	else
	{
		eeprom_write(0,0);
		HAL_Delay(10);
		eeprom_write(1,0);
		HAL_Delay(10);
		eeprom_write(2,7);
		HAL_Delay(10);
		eeprom_write(3,10);
		HAL_Delay(10);
	}
}

void loop(void)
{
	Data_Proc();
	Key_Proc();
	LCD_Proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t count=0,seg_view;
	if(htim->Instance==TIM6)
	{
		key_long_serv();
		count+=1;
		if(count>=200)
		{
			if(seg_view==0)
			{
				SEG_chg_disp(12,T/10,T%10);
				seg_view=1;
			}
			else
			{
				SEG_chg_disp(10,0,X);
				seg_view=0;
			}
			count=0;
		}
	}
}
