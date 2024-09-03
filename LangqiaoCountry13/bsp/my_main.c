#include "my_main.h"

extern struct keys Key[];

uint8_t view=0;

float V_PA4=3.02,V_PA5=1.86;
int16_t F_PA1=2046;

int8_t X=1,Y=1;

int8_t N_PA4=0,N_PA5=0;
float A_PA4=0,T_PA4=0,H_PA4=0;
float A_PA5=0,T_PA5=0,H_PA5=0;
float PA4_data[100],PA5_data[100];

uint8_t mod=0;
uint8_t Channel_Choose=0;

uint8_t disp;
uint8_t show_mod=0,show_mod_change=0;

uint32_t led_tick;

char uart_tx[30];
char uart_rx[30];

int16_t adc2_rx[2];

void V_Update(void)
{
	V_PA4 = adc2_rx[0]*3.3/4096.f;
	V_PA5 = adc2_rx[1]*3.3/4096.f;
	
	PA4_data[N_PA4%100]=V_PA4;
	PA5_data[N_PA5%100]=V_PA5;
	N_PA4++;
	N_PA5++;
	
	float sum_PA4,A_PA4_temp=0,T_PA4_temp=10;
	for(int i=0;i<(N_PA4>=100?100:N_PA4);i++)
	{
		if(PA4_data[i]>A_PA4_temp) A_PA4_temp=PA4_data[i];
		if(PA4_data[i]<T_PA4_temp) T_PA4_temp=PA4_data[i];
		sum_PA4+=PA4_data[i];
	}
	A_PA4=A_PA4_temp;
	T_PA4=T_PA4_temp;
	H_PA4=sum_PA4/(N_PA4>=100?100:N_PA4);
	
	float sum_PA5,A_PA5_temp=0,T_PA5_temp=10;
	for(int i=0;i<(N_PA5>=100?100:N_PA5);i++)
	{
		if(PA5_data[i]>A_PA5_temp) A_PA5_temp=PA5_data[i];
		if(PA5_data[i]<T_PA5_temp) T_PA5_temp=PA5_data[i];
		sum_PA5+=PA5_data[i];
	}
	A_PA5=A_PA5_temp;
	T_PA5=T_PA5_temp;
	H_PA5=sum_PA5/(N_PA5>=100?100:N_PA5);
}

void key_proc(void)
{
	if(Key[1].single_flag)
	{
		view+=1;
		if(view==3) view=0;
		LCD_Clear(Black);
		Channel_Choose=0;
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag)
	{
		if(view==1)
		{
			X+=1;
			if(X>=5) X=1;
			eeprom_write(0,X);
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag)
	{
		if(view==1)
		{
			Y+=1;
			if(Y>=5) Y=1;
			eeprom_write(1,Y);
		}
		Key[3].single_flag=0;
	}
	if(Key[4].single_flag)
	{
		if(view==0)
		{
			V_Update();
		}
		else if(view==1)
		{
			mod+=1;
			if(mod>1) mod = 0;
		}
		else
		{
			Channel_Choose+=1;
			if(Channel_Choose>1) Channel_Choose=0;
		}
		Key[4].single_flag=0;
	}
	if(Key[4].long_flag)
	{
		if(view==2)
		{
			if(Channel_Choose==0)
			{
				N_PA4=0;A_PA4=0;T_PA4=0;H_PA4=0;
			}
			else
			{
				N_PA5=0;A_PA5=0;T_PA5=0;H_PA5=0;
			}
		}
		Key[4].long_flag=0;
	}
}

void LCD_proc(void)
{
	if(show_mod_change)
	{
		if(show_mod==1)
		{
			REG_932X_Init_Reverse();
			LCD_Clear(Black);
		}
		else
		{
			LCD_Init();
			LCD_Clear(Black);
		}
		show_mod_change=0;
	}
	char text[30];
	if(view==0)
	{
		sprintf(text,"        DATA       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     PA4=%.2f       ",V_PA4);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     PA5=%.2f       ",V_PA5);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     PA1=%d       ",F_PA1);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
	else if(view==1)
	{
		sprintf(text,"        PARA       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     X=%d       ",X);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     Y=%d       ",Y);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
	}
	else 
	{
		if(Channel_Choose==0)
		{
			sprintf(text,"        REC-PA4       ");
			LCD_DisplayStringLine(Line1, (unsigned char *)text);
			sprintf(text,"     N=%d       ",(N_PA4>=100?100:N_PA4));
			LCD_DisplayStringLine(Line3, (unsigned char *)text);
			sprintf(text,"     A=%.2f       ",A_PA4);
			LCD_DisplayStringLine(Line4, (unsigned char *)text);
			sprintf(text,"     T=%.2f       ",T_PA4);
			LCD_DisplayStringLine(Line5, (unsigned char *)text);
			sprintf(text,"     H=%.2f       ",H_PA4);
			LCD_DisplayStringLine(Line6, (unsigned char *)text);
		}
		else
		{
			sprintf(text,"        REC-PA5       ");
			LCD_DisplayStringLine(Line1, (unsigned char *)text);
			sprintf(text,"     N=%d       ",(N_PA5>=100?100:N_PA5));
			LCD_DisplayStringLine(Line3, (unsigned char *)text);
			sprintf(text,"     A=%.2f       ",A_PA5);
			LCD_DisplayStringLine(Line4, (unsigned char *)text);
			sprintf(text,"     T=%.2f       ",T_PA5);
			LCD_DisplayStringLine(Line5, (unsigned char *)text);
			sprintf(text,"     H=%.2f       ",H_PA5);
			LCD_DisplayStringLine(Line6, (unsigned char *)text);
		}
	}
}

void Led_proc(void)
{
	if(uwTick-led_tick>=100)
	{
		if(mod==0) disp|=0x01;
		else disp &=~0x01;
		if(mod==1) disp|=0x02;
		else disp &=~0x02;
		if(V_PA4>V_PA5*X) disp ^= 0x04;
		else disp &=~0x04;
		if(show_mod==0) disp |= 0x08;
		else disp &=~0x08;
		LED_Disp(disp);
		led_tick=uwTick;
	}
}
void setup(void)
{
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,30);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t*)adc2_rx,2);
	
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	I2CInit();
	
	LED_Disp(0x00);
	led_tick=uwTick;
	
	//判断是否首次启动
	uint8_t temp1,temp2;
	temp1 = eeprom_read(2);
	HAL_Delay(10);
	temp2 = eeprom_read(3);
	HAL_Delay(10);
	if(temp1==3 && temp2==6)
	{
		X = eeprom_read(0);
		HAL_Delay(10);
		Y = eeprom_read(1);
	}
	else
	{
		eeprom_write(0,1);
		HAL_Delay(10);
		eeprom_write(1,1);
		HAL_Delay(10);
		eeprom_write(2,3);
		HAL_Delay(10);
		eeprom_write(3,6);
	}
	
}

void loop(void)
{
	key_proc();
	LCD_proc();
	Led_proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
		key_long_serv();
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(Size==1)
	{
		if(strcmp(uart_rx,"X")==0)
		{
			sprintf(uart_tx,"X:%d\r\n",X);
			HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
		}
		else if(strcmp(uart_rx,"Y")==0)
		{
			sprintf(uart_tx,"Y:%d\r\n",Y);
			HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
		}
		else if(strcmp(uart_rx,"#")==0)
		{
			show_mod+=1;
			show_mod_change=1;
			
			if(show_mod==2) show_mod=0;
		}
	}
	if(Size==3)
	{
		if(strcmp(uart_rx,"PA1")==0)
		{
			sprintf(uart_tx,"PA1:%d\r\n",F_PA1);
			HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
		}
		if(strcmp(uart_rx,"PA4")==0)
		{
			sprintf(uart_tx,"PA4:%.2f\r\n",V_PA4);
			HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
		}
		if(strcmp(uart_rx,"PA5")==0)
		{
			sprintf(uart_tx,"PA5:%.2f\r\n",V_PA5);
			HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
		}
	}
	HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,30);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	int count;
	if(htim->Instance==TIM2)
	{
		count=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2);
		F_PA1 = (1000000)/count;
		int f_out;
		if(mod==0) f_out=F_PA1*X;
		else f_out=F_PA1/X;
		int new_reload=10000000/f_out - 1;
		__HAL_TIM_SetAutoreload(&htim17,new_reload);
		__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(new_reload+1)/2);
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
	}
}
