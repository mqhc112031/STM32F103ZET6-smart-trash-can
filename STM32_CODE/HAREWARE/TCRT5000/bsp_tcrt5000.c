#include "bsp_tcrt5000.h"
#include "usart.h"
u8 TRCT1=0;
u8 TRCT2=0;
u8 TRCT3=0;
u8 TRCT4=0;
void TRC5000_Init(void)
{
	GPIO_InitTypeDef GPIO_InitTypeStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitTypeStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitTypeStructure);
}
int trc5000D01(void)
{
			if(Tracking_DO1 == 1)
		{
//			UsartPrintf(USART_DEBUG,"TRCD010\r\n");//没满
			return 0;
		}
		else 
		{
//			UsartPrintf(USART_DEBUG,"TRCD011\r\n");//满了
			return 1;
		}	
}
int trc5000D02(void)
{
			if(Tracking_DO2 == 1)
		{
//			UsartPrintf(USART_DEBUG,"TRCD020\r\n");//没满
			return 0;
		}
		else 
		{
//			UsartPrintf(USART_DEBUG,"TRCD021\r\n");//满了
			return 1;
		}	
}
int trc5000D03(void)
{
			if(Tracking_DO3 == 1)
		{
//			UsartPrintf(USART_DEBUG,"TRCD030\r\n");//没满
			return 0;
		}
		else 
		{
//			UsartPrintf(USART_DEBUG,"TRCD031\r\n");//满了
			return 1;
		}	
}
int trc5000D04(void)
{
			if(Tracking_DO4 == 1)
		{
//			UsartPrintf(USART_DEBUG,"TRCD040\r\n");//没满
			return 0;
		}
		else 
		{
//			UsartPrintf(USART_DEBUG,"TRCD041\r\n");//满了
			return 1;
		}	
}
