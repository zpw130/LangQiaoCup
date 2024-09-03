#include "my_main.h"

extern struct keys Key[];
extern uint8_t key_lock;

uint8_t view;

uint8_t disp=0x00;
uint32_t led_tick;

int freq=3300;
int duty=42;
float A=3.1;
float T=23.8;

int FH=2000; //1000-10000
float AH=3; //0-3.3
int TH=30; //0-80

int FN=0;
int AN=0;
int TN=0;

int FP=1; //1-10
float VP=0.9; //0-3.3
int TT=6; //2-10,记录时长

uint8_t data_choose;
int F_record[1000];
int D_record[1000];
float V_record[1000];
int pre_TT;
uint8_t record=0;
uint8_t mod=0;

void Key_proc(void)
{
	if(Key[1].single_flag)
	{
		view+=1;
		if(view==4) view=0; 
		data_choose=0;
		LCD_Clear(Black);
		Key[1].single_flag=0;
	}
	if(Key[2].single_flag)
	{
		if(view==0)
		{
			mod=1;
			key_lock=1; //开始记录，锁定按键
			HAL_TIM_Base_Start_IT(&htim7); 
		}
		else if(view==1 || view==3)
		{
			data_choose++;
			if(data_choose==3) data_choose=0;
		}
		else //view=2
		{
			FN=0;AN=0;TN=0; //清空统计次数
		}
		Key[2].single_flag=0;
	}
	if(Key[3].single_flag)
	{
		if(view==1)
		{
			if(data_choose==0 && FH<10000) FH+=1000;
			else if(data_choose==1 && AH<3.2f) AH+=0.3f;
			else if(data_choose==2 && TH<80) TH+=1;
		}
		if(view==3)
		{
			if(data_choose==0 && FP<10) FP+=1;
			else if(data_choose==1 && VP<3.2f) VP+=0.3f;
			else if(data_choose==2 && TT<10) TT+=1;
		}
		Key[3].single_flag=0;
	}
	if(Key[4].single_flag)
	{
		if(view==1)
		{
			if(data_choose==0 && FH>1000) FH-=1000;
			else if(data_choose==1 && AH>0.1f) AH-=0.3f;
			else if(data_choose==2 && TH>0) TH-=1;
		}
		if(view==3)
		{
			if(data_choose==0 && FP>1) FP-=1;
			else if(data_choose==1 && VP>0.1f) VP-=0.3f;
			else if(data_choose==2 && TT>2) TT-=1;
		}
		Key[4].single_flag=0;
	}
}
void LCD_proc(void)
{
	char text[30];
	if(view==0)
	{
		sprintf(text,"        DATA       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     F=%d     ",freq);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     D=%d%%     ",duty);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     A=%.1f     ",A);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"     T=%.1f     ",T);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);
		
		sprintf(text,"V:%.1f,%.1f,%.1f,%.1f     ",V_record[0],V_record[200],V_record[300],V_record[500]);
		LCD_DisplayStringLine(Line8, (unsigned char *)text);
	}
	else if(view==1)
	{
		sprintf(text,"        PARA       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     FH=%d     ",FH);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     AH=%.1f     ",AH);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     TH=%d     ",TH);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
	else if(view==2)
	{
		sprintf(text,"        RECD       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     FN=%d     ",FN);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     AN=%d     ",AN);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     TN=%d     ",TN);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
	else
	{
		sprintf(text,"        FSET       ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"     FP=%d     ",FP);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"     VP=%.1f     ",VP);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text,"     TT=%d     ",TT);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
}

void Led_proc(void)
{
	if(uwTick-led_tick>=100)
	{
		if(mod==1) disp^=0x01;  //LED1
		else disp&=~0x01;
		
		if(mod==2) disp^=(0x01<<1); //LED2
		else disp&=~(0x01<<1);
		
		if(mod==3) disp^=(0x01<<2); //LED3
		else disp&=~(0x01<<2);
		
		if(freq>FH) disp|=(0x01<<3); //LED4
		else disp&=~(0x01<<3);
		
		if(A>AH) disp|=(0x01<<4); //LED5
		else disp&=~(0x01<<4);
		
		if(T>TH) disp|=(0x01<<5); //LED6
		else disp&=~(0x01<<5);
		
		LED_disp(disp);
		led_tick=uwTick;
	}
}

float ADC_Read(ADC_HandleTypeDef* pin)
{
	uint16_t value;
	HAL_ADC_Start(pin);
	value=HAL_ADC_GetValue(pin);
	return value*3.3f/65536.f;
}

void Get_data(void)
{
	//输入捕获频率和占空比
	freq = 1000000/(HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2)+1);
	duty = (HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1)+1)*100.f/(HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2)+1);
	
	//adc采样
	A=ADC_Read(&hadc2);
}
void setup(void)
{
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
	LCD_Init();
	LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
	LED_disp(0x00);
	led_tick=uwTick;
}
void loop(void)
{
	Get_data();
	Key_proc();
	LCD_proc();
	Led_proc();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
		key_serv_long();
	}
	static int count=0;
	if(htim->Instance==TIM7)
	{
		F_record[count]=freq;
		D_record[count]=duty;
		V_record[count]=A;
		count+=1;
		if(count==TT*100)
		{
			record=1; //成功记录
			mod=0;
			count=0;
			pre_TT=TT;
			key_lock=0;
			HAL_TIM_Base_Stop_IT(&htim7);
		}
	}
}

