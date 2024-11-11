#include "sys.h"
#include "usart.h"
//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#include "key.h"
#include "driver.h"
#include "rtc.h"
#include "led.h"
#include "lcd.h"
#include "gui.h"
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "gui.h"
#if SYSTEM_SUPPORT_OS
    #include "includes.h"					//ucos 使用
#endif
vu8 key = 0;
vu8 key1 = 0;
u8 uart3flag=0;
u8 SG90Flag=0; //舵机 触发打开垃圾桶一次 标志   
extern const unsigned char gImage_ganlaji[];
extern const unsigned char gImage_shilaji[];
extern const unsigned char gImage_youhailaji[];
extern const unsigned char gImage_kehuishouwu[];
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
//#if 1
//#pragma import(__use_no_semihosting)
////标准库需要的支持函数
//struct __FILE
//{
//	int handle;

//};

//FILE __stdout;
////定义_sys_exit()以避免使用半主机模式
//void _sys_exit(int x)
//{
//	x = x;
//}
////重定义fputc函数
//int fputc(int ch, FILE *f)
//{
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
//    USART1->DR = (u8) ch;
//	return ch;
//}
//#endif

///*使用microLib的方法*/
// /*
//int fputc(int ch, FILE *f)
//{
//	USART_SendData(USART1, (uint8_t) ch);

//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
//
//    return ch;
//}
//int GetKey (void)  {

//    while (!(USART1->SR & USART_FLAG_RXNE));

//    return ((int)(USART1->DR & 0x1FF));
//}
//*/
//
//#if EN_USART1_RX   //如果使能了接收
////串口1中断服务程序
////注意,读取USARTx->SR能避免莫名其妙的错误
//u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
////接收状态
////bit15，	接收完成标志
////bit14，	接收到0x0d
////bit13~0，	接收到的有效字节数目
//u16 USART_RX_STA=0;       //接收状态标记
/*串口1用于CH340的debug功能			*/
/*
************************************************************
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA9		RX-PA10	USB转TTL，PA10接TX
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //PA9	TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    //PA10	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE);														//使能串口

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvicInitStruct);

}

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//	{
//	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntEnter();
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//
//		if((USART_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(USART_RX_STA&0x4000)//接收到了0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
//				else USART_RX_STA|=0x8000;	//接收完成了
//				}
//			else //还没收到0X0D
//				{
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收
//					}
//				}
//			}
//     }
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntExit();
//#endif
//}
//#endif
/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void USART1_IRQHandler(void)
{

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
        {
            USART_ClearFlag(USART1, USART_FLAG_RXNE);
        }

}
/*串口2用于WIFI模块的通信功能			*/
/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{

    GPIO_InitTypeDef gpioInitStruct;
    USART_InitTypeDef usartInitStruct;
    NVIC_InitTypeDef nvicInitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PA2	TXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    //PA3	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);														//使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);

}
//u8 IRQURA3 = 0;							// 串口3 接受数据完毕标志
//u8 USART3_RX_BUF[USART3_REC_LEN] = {0}; // 串口3数据反冲去

void usart3_init(u32 band)
{
	//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIOD时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能AFIO时钟

	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE); 

 
	//USART3_TX    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化 
   
  //USART3_RX	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//P  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化   

  //Usart NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3 原3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3     原2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = band;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口3
}
void usart3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	 
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	 
}
 /*发送一个字节数据*/
 void USART3SendByte(unsigned char SendData)
{	 
   
        USART_SendData(USART3,SendData);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
} 

/*
根据长度向串口3 发送数据
*/
void usart3_SendStringByLen(unsigned char * data,u8 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART3, data[i]);         //向串口1发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		//i++;
	}
	//i++;
}
/*串口3 发送数据到上位机 data 以\0结尾*/
void usart3_SendString(unsigned char * data)
{
	u16 i=0;
	while(data[i]!='\0')
	{
		USART_SendData(USART3, data[i]);         //向串口1发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		i++;
	}
	i++;
}
/*
串口3 查询接受模式

*/
u16 usart3_receive(u32 time)  
{
	  u16 i=0;
    u32 time0=0;
    i = USART_ReceiveData(USART3);
    while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == RESET)
    { 
        delay_us(1);
        time0++;
        if( time0 > time )
        {
           // printf("\r\ntime=%d\r\n",time);
            return 0;
        } 
    }
    USART3_RX_BUF[0] = USART_ReceiveData(USART3); 
    i = 1;
    time0 = 0;
    while(1)
    { 
        if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) != RESET)
        {	 
            time0 = 0;
            USART3_RX_BUF[i] = USART_ReceiveData(USART3);
            i++;
//            if( (i == len) && (len != 0) )  
//                break; 
        }
        else 
        {
            if( time0 > 50000 ) 
                break;
            else 
                time0++;
        }
    }
    USART3_RX_BUF[i]=0;
    return i;

}

 //['无', '可回收', '有害', '厨余', '其他']   0-1-2-3-4 
void getResultTemp()
{
	
  static unsigned char cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	POINT_COLOR=BLUE;
			LCD_Fill(10,50,130, 70, WHITE);
//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
		Show_Str(10, 70, BLUE, WHITE, "图像识别开启", 16, 1);//系统开启
	LCD_Fill(10,90,130, 130, WHITE);
	if(SG90Flag==0)
 {
      if(strstr((char*)USART3_RX_BUF,"result:1")!=NULL) //  
			{
				cnt1++;
				if(cnt1>3)
				{
					cnt1=0;
					SG90Flag=0; 
//					SG90_PWM1=OPEN;  //打开垃圾桶盖可回收垃圾 
//					printf("可回收垃圾\r\n");
							TIM_SetCompare4(TIM3,1850); 			
							LCD_Clear(WHITE);
					//		printf("可回收物\n\r\n");
							Show_Str(10, 110, BLUE, WHITE, "可回收物", 16, 1);
							Gui_Drawbmp16(30, 80, gImage_kehuishouwu);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							closedj();
							LCD_Clear(WHITE);
							Show_Str(10, 110, BLUE, WHITE, "可回收物", 16, 1);

				}
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
			}
else  if(strstr((char*)USART3_RX_BUF,"result:2")!=NULL) //  
			{
				cnt2++;
				if(cnt2>3)
				{
					cnt2=0;				
				  SG90Flag=0;
//				  SG90_PWM2=OPEN;  //打开垃圾桶盖有害垃圾
//					printf("有害垃圾\r\n");
							TIM_SetCompare3(TIM3,1850);
							LCD_Clear(WHITE);
							Show_Str(10, 110, BLUE, WHITE, "有害垃圾", 16, 1);
							Gui_Drawbmp16(30, 80, gImage_youhailaji);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							closedj();
							LCD_Clear(WHITE);
							Show_Str(10, 110, BLUE, WHITE, "有害垃圾", 16, 1);

				}
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));            
			}
else if(strstr((char*)USART3_RX_BUF,"result:3")!=NULL) //  
			{
				cnt3++;
				if(cnt3>3)
				{
					cnt3=0;				
					SG90Flag=0;
//					SG90_PWM3=OPEN;   //打开垃圾桶盖厨余垃圾 
//					printf("厨余垃圾\r\n");
					TIM_SetCompare2(TIM3,1850);
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "湿垃圾", 16, 1);
					Gui_Drawbmp16(30, 80, gImage_shilaji);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
					closedj();
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "湿垃圾", 16, 1);
				}
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF)); 
			}
else if(strstr((char*)USART3_RX_BUF,"result:4")!=NULL) //
			{
				cnt4++;
				if(cnt4>3)
				{
					cnt4=0;				
					SG90Flag=0;
//					SG90_PWM4=OPEN;   //打开垃圾桶盖其他垃圾 
//					printf("其他垃圾\r\n");
					TIM_SetCompare1(TIM3,1850);
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "干垃圾", 16, 1);
					Gui_Drawbmp16(30, 80, gImage_ganlaji);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
							delay_ms(500);
					closedj();
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "干垃圾", 16, 1);
				}
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF)); 
			}
else if(strstr((char*)USART3_RX_BUF,"result:0")!=NULL) //
			{
				cnt1=0;cnt2=0;cnt3=0;cnt4=0;
//				closedj();
//			  printf("无垃圾\r\n");//无垃圾 
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF)); 
			} 
 }
   
}
/*
串口3中断函数
*/
void USART3_IRQHandler(void)                	//串口2中断服务程序
	{
 
		 static u8 i;
 	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{	
  	 
				USART3_RX_BUF[i] = USART_ReceiveData(USART3);
				  i++; 
			if(i > 10) //接收 
			{
				  i=0;
			}
 
 
  }   
 }

/*串口4用于语音模块的通信功能			*/
u16 USART4_RX_STA=0;  //接收状态标记	  
u16 USART4_RX_Flag=0; //接收数据标志位
u8  USART4_RX_Tmr=0;  // 接收延时
u8	USART4_RX_data;
char USART4_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
void Uart4_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能USART1，GPIOA，复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	//USART4_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10

	//USART4_RX	  GPIOC.11初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.11  

	//Usart4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART4, &USART_InitStructure); //初始化串口4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);                    //使能串口4 
}
void UART4_IRQHandler(void)                	//串口4中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(UART4);	//读取接收到的数据
		
		if((USART4_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART4_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART4_RX_STA=0;//接收错误,重新开始
				else USART4_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART4_RX_STA|=0x4000;
				else
					{
					USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
					USART4_RX_STA++;
					if(USART4_RX_STA>(USART_REC_LEN-1))USART4_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
} 

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    unsigned short count = 0;

    for(; count < len; count++)
        {
            USART_SendData(USARTx, *str++);									//发送数据

            while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
        }

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{

    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
    va_end(ap);

    while(*pStr != 0)
        {
            USART_SendData(USARTx, *pStr++);

            while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
        }

}

