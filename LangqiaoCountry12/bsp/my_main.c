#include "my_main.h"

uint8_t view;

extern struct keys Key[];
extern struct keys adc_key;
extern uint16_t adc_val;

float a=0,b=0;
uint16_t count;
float a_his[5],b_his[5];
float a_his_sort[5],b_his_sort[5];

uint16_t f=2326;
float PA6_duty,PA7_duty;
uint8_t ax=0,bx=0;
char mode='A';

uint16_t Pax=20,Pbx=20,Pf=1000;

uint8_t disp;

uint8_t PA6_stat,PA7_stat;
uint16_t PA6_Value1,PA6_Value2,PA6_High,PA6_Low;
uint16_t PA7_Value1,PA7_Value2,PA7_High,PA7_Low;

char rx_buf[10];

void Angle_Update(void)
{
	if(PA6_duty<0.1f) a=0;
	else if(PA6_duty>0.9f) a=180;
	else a = 225*(PA6_duty-0.1f);
	
	if(PA7_duty<0.1f) b=0;
	else if(PA7_duty>0.9f) b=90;
	else b = 112.5f*(PA7_duty-0.1f);
	
	if(count>=5)
	{
		for(int i=0;i<4;i++)
		{
			a_his[i] = a_his[i+1];
			b_his[i] = b_his[i+1];
		}
		a_his[4] = a;
		b_his[4] = b;
	}
	else
	{
		a_his[count] = a;
		b_his[count] = b;
	}
	if(count>=4)
	{
		ax = fabs(a_his[4] - a_his[3]);
		bx = fabs(b_his[4] - b_his[3]);
	}
	else if(count>=1)
	{
		ax = fabs(a_his[count] - a_his[count-1]);
		bx = fabs(b_his[count] - b_his[count-1]);
	}
	count+=1;
	
	float temp;
	for(int i=0;i<5;i++)
	{
		a_his_sort[i] = a_his[i];
		b_his_sort[i] = b_his[i];
	}
	for(int i=1;i<(count>=5?5:count);i++)
	{
		for(int j=0;j<(count>=5?5:count)-i;j++)
		{
			if(a_his_sort[j]>a_his_sort[j+1])
			{
				temp = a_his_sort[j+1];
				a_his_sort[j+1] = a_his_sort[j];
				a_his_sort[j] = temp;
			}
			if(b_his_sort[j]>b_his_sort[j+1])
			{
				temp = b_his_sort[j+1];
				b_his_sort[j+1] = b_his_sort[j];
				b_his_sort[j] = temp;
			}
		}
	}
}

void Key_proc(void)
{
	static uint8_t adc_flag=0;
	if(adc_key.flag==1)
	{
		if(!adc_flag)
		{
			if(mode=='B') Angle_Update();
			adc_flag=1;
		}
	}
	else adc_flag=0;
	if(Key[1].flag==1)
	{
		view+=1;
		if(view==2) view=0;
		LCD_Clear(Black);
		Key[1].flag=0;
	}
	if(Key[2].flag==1)
	{
		if(view==1)
		{
			Pax+=10;
			Pbx+=10;
			if(Pax>60) Pax=10;
			if(Pbx>60) Pbx=10;
		}
		Key[2].flag=0;
	}
	if(Key[3].flag==1)
	{
		if(view==0)
		{
			if(mode=='A') mode='B';
			else mode='A';
		}
		else if(view==1)
		{
			Pf+=1000;
			if(Pf>10000) Pf=1000;
		}
		Key[3].flag=0;
	}
	if(Key[4].flag==1)
	{
		if(mode=='A') Angle_Update();
		Key[4].flag=0;
	}
}

void LCD_proc(void)
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"        DATA      ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"   a:%.1f         ",a);
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"   b:%.1f         ",b);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"   f:%dHz         ",f);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"   ax:%d         ",ax);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
		sprintf(text,"   bx:%d         ",bx);
		LCD_DisplayStringLine(Line7, (unsigned char *)text);
		sprintf(text,"   mode:%c         ",mode);
		LCD_DisplayStringLine(Line8, (unsigned char *)text);
		sprintf(text,"   adc:%d         ",adc_val);
		LCD_DisplayStringLine(Line9, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"        PARA      ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"   Pax:%d         ",Pax);
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"   Pbx:%d         ",Pbx);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"   Pf:%d         ",Pf);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
	}
}

void LED_proc(void)
{
	if(ax>Pax) disp|=0x01;
	else disp&=~(0x01);
	if(bx>Pbx) disp|=0x02;
	else disp&=~(0x02);
	if(f>Pf) disp|=0x04;
	else disp&=~(0x04);
	if(mode=='A') disp|=0x08;
	else disp&=~(0x08);
	if(fabs(90+b-a)<10) disp |=(0x10);
	else disp&=~(0x10);
	LED_Disp(disp);
	
}

void setup(void)
{
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t*)rx_buf,10);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	LED_Disp(0x00);
}

void loop(void)
{
	Key_proc();
	LCD_proc();
	LED_proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
		key_serv();
		adc_read(&hadc2);
		key_adc_serv();
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		f = 1000000/HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
	}

	if(htim->Instance==TIM3)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			if(PA6_stat==0)
			{
				PA6_Value1 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
				PA6_stat+=1;
			}
			else if(PA6_stat==1)
			{
				PA6_Value2 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_1);
				if(PA6_Value1>PA6_Value2)
				{
					PA6_High = (0xFFFF - PA6_Value1) + PA6_Value2;
				}
				else PA6_High = PA6_Value2 - PA6_Value1;
				PA6_Value1 = PA6_Value2;
				PA6_stat+=1;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
			}
			else
			{
				PA6_Value2 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_1);
				if(PA6_Value1>PA6_Value2)
				{
					PA6_Low = (0xFFFF - PA6_Value1) + PA6_Value2;
				}
				else PA6_Low = PA6_Value2 - PA6_Value1;
				
				PA6_duty = PA6_High*1.0/(PA6_High + PA6_Low);
				PA6_stat=0;
			}
		}
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			if(PA7_stat==0)
			{
				PA7_Value1 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_2);
				__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
				PA7_stat+=1;
			}
			else if(PA7_stat==1)
			{
				PA7_Value2 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_2);
				if(PA7_Value1>PA7_Value2)
				{
					PA7_High = (0xFFFF - PA7_Value1) + PA7_Value2;
				}
				else PA7_High = PA7_Value2 - PA7_Value1;
				PA7_Value1 = PA7_Value2;
				PA7_stat+=1;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
			}
			else
			{
				PA7_Value2 = __HAL_TIM_GetCompare(htim,TIM_CHANNEL_2);
				if(PA7_Value1>PA7_Value2)
				{
					PA7_Low = (0xFFFF - PA7_Value1) + PA7_Value2;
				}
				else PA7_Low = PA7_Value2 - PA7_Value1;
				
				PA7_duty = PA7_High*1.0/(PA7_High + PA7_Low);
				PA7_stat=0;
			}
		}
	}
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	char text[20];
	if(strcmp(rx_buf,"a?")==0)
	{
		sprintf(text,"a:%.1f\r\n",a);
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	else if(strcmp(rx_buf,"b?")==0)
	{
		sprintf(text,"b:%.1f\r\n",b);
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	else if(strcmp(rx_buf,"aa?")==0)
	{
		sprintf(text,"aa:");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		for(int i=0;i<(count>=5?5:count);i++)
		{
			if(i==0) sprintf(text,"%.1f",a_his[i]);
			else sprintf(text,"-%.1f",a_his[i]);
			HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		}
		sprintf(text,"\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	else if(strcmp(rx_buf,"bb?")==0)
	{
		sprintf(text,"bb:");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		for(int i=0;i<(count>=5?5:count);i++)
		{
			if(i==0) sprintf(text,"%.1f",b_his[i]);
			else sprintf(text,"-%.1f",b_his[i]);
			HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		}
		sprintf(text,"\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	
	else if(strcmp(rx_buf,"qa?")==0)
	{
		sprintf(text,"qa:");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		for(int i=0;i<(count>=5?5:count);i++)
		{
			if(i==0) sprintf(text,"%.1f",a_his_sort[i]);
			else sprintf(text,"-%.1f",a_his_sort[i]);
			HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		}
		sprintf(text,"\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	else if(strcmp(rx_buf,"qb?")==0)
	{
		sprintf(text,"qb:");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		for(int i=0;i<(count>=5?5:count);i++)
		{
			if(i==0) sprintf(text,"%.1f",b_his_sort[i]);
			else sprintf(text,"-%.1f",b_his_sort[i]);
			HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
		}
		sprintf(text,"\r\n");
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	else
	{
		sprintf(text,"error:%s\r\n",rx_buf);
		HAL_UART_Transmit(huart,(uint8_t*)text,strlen(text),50);
	}
	memset(rx_buf,0,10);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t*)rx_buf,10);
}
