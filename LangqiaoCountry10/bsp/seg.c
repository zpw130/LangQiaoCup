#include "seg.h"
uint8_t disp_dict[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00};

uint8_t show_num[3]={1,2,3};
uint8_t point[3];

void SEG_disp_single(uint8_t disp)
{
	uint8_t temp=disp;
	for(int i=0;i<8;i++)
	{
		if(temp&0x80) HAL_GPIO_WritePin(SER_GPIO_Port,SER_Pin,GPIO_PIN_SET);
		else HAL_GPIO_WritePin(SER_GPIO_Port,SER_Pin,GPIO_PIN_RESET);
		temp=temp<<1;
		HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_RESET);
	}
}

void SEG_disp(void)
{
	for(int i=2;i>=0;i--)
	{
		if(point[i])SEG_disp_single(disp_dict[show_num[i]]|0x80);
		else SEG_disp_single(disp_dict[show_num[i]]);
	}
	HAL_GPIO_WritePin(RCK_GPIO_Port,RCK_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(RCK_GPIO_Port,RCK_Pin,GPIO_PIN_RESET);
}

void SEG_chg_disp(uint8_t b0,uint8_t b1,uint8_t b2)
{
	if(b0<18) show_num[0]=b0;
	if(b1<18) show_num[1]=b1;
	if(b2<18) show_num[2]=b2;
	SEG_disp();
}

void SEG_chg_point(uint8_t b0,uint8_t b1,uint8_t b2)
{
	if(b0<2) point[0]=b0;
	if(b1<2) point[1]=b1;
	if(b2<2) point[2]=b2;
	SEG_disp();
}

