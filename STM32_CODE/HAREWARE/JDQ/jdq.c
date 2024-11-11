#include "jdq.h"
#include "delay.h"
#include "stm32f10x.h"

void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_PINI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
}          

void relay_on(void)
{
  GPIO_SetBits(GPIOF,GPIO_PINI);
}

void relay_off(void)
{ 
  GPIO_ResetBits(GPIOF,GPIO_PINI);
}

void relay_onoff(u8 mode)
{
	if(relay_mode==1)
	{
		relay_on();
	}
	else
		relay_off();
}
