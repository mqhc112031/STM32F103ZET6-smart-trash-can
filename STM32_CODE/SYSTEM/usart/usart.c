#include "sys.h"
#include "usart.h"
//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
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
    #include "includes.h"					//ucos ʹ��
#endif
vu8 key = 0;
vu8 key1 = 0;
u8 uart3flag=0;
u8 SG90Flag=0; //��� ����������Ͱһ�� ��־   
extern const unsigned char gImage_ganlaji[];
extern const unsigned char gImage_shilaji[];
extern const unsigned char gImage_youhailaji[];
extern const unsigned char gImage_kehuishouwu[];
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
//#if 1
//#pragma import(__use_no_semihosting)
////��׼����Ҫ��֧�ֺ���
//struct __FILE
//{
//	int handle;

//};

//FILE __stdout;
////����_sys_exit()�Ա���ʹ�ð�����ģʽ
//void _sys_exit(int x)
//{
//	x = x;
//}
////�ض���fputc����
//int fputc(int ch, FILE *f)
//{
//	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
//    USART1->DR = (u8) ch;
//	return ch;
//}
//#endif

///*ʹ��microLib�ķ���*/
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
//#if EN_USART1_RX   //���ʹ���˽���
////����1�жϷ������
////ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
//u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART_RX_STA=0;       //����״̬���
/*����1����CH340��debug����			*/
/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA9		RX-PA10	USBתTTL��PA10��TX
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
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
    usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE);														//ʹ�ܴ���

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�

    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvicInitStruct);

}

//void USART1_IRQHandler(void)                	//����1�жϷ������
//	{
//	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//
//		if((USART_RX_STA&0x8000)==0)//����δ���
//			{
//			if(USART_RX_STA&0x4000)//���յ���0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//				else USART_RX_STA|=0x8000;	//���������
//				}
//			else //��û�յ�0X0D
//				{
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����
//					}
//				}
//			}
//     }
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntExit();
//#endif
//}
//#endif
/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����
************************************************************
*/
void USART1_IRQHandler(void)
{

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
        {
            USART_ClearFlag(USART1, USART_FLAG_RXNE);
        }

}
/*����2����WIFIģ���ͨ�Ź���			*/
/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
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
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
    usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
    usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
    usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�

    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);

}
//u8 IRQURA3 = 0;							// ����3 ����������ϱ�־
//u8 USART3_RX_BUF[USART3_REC_LEN] = {0}; // ����3���ݷ���ȥ

void usart3_init(u32 band)
{
	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��GPIODʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ��AFIOʱ��

	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE); 

 
	//USART3_TX    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ�� 
   
  //USART3_RX	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//P  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��   

  //Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3 ԭ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3     ԭ2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = band;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3
}
void usart3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	 
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	 
}
 /*����һ���ֽ�����*/
 void USART3SendByte(unsigned char SendData)
{	 
   
        USART_SendData(USART3,SendData);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
} 

/*
���ݳ����򴮿�3 ��������
*/
void usart3_SendStringByLen(unsigned char * data,u8 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART3, data[i]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		//i++;
	}
	//i++;
}
/*����3 �������ݵ���λ�� data ��\0��β*/
void usart3_SendString(unsigned char * data)
{
	u16 i=0;
	while(data[i]!='\0')
	{
		USART_SendData(USART3, data[i]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		i++;
	}
	i++;
}
/*
����3 ��ѯ����ģʽ

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

 //['��', '�ɻ���', '�к�', '����', '����']   0-1-2-3-4 
void getResultTemp()
{
	
  static unsigned char cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	POINT_COLOR=BLUE;
			LCD_Fill(10,50,130, 70, WHITE);
//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
		Show_Str(10, 70, BLUE, WHITE, "ͼ��ʶ����", 16, 1);//ϵͳ����
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
//					SG90_PWM1=OPEN;  //������Ͱ�ǿɻ������� 
//					printf("�ɻ�������\r\n");
							TIM_SetCompare4(TIM3,1850); 			
							LCD_Clear(WHITE);
					//		printf("�ɻ�����\n\r\n");
							Show_Str(10, 110, BLUE, WHITE, "�ɻ�����", 16, 1);
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
							Show_Str(10, 110, BLUE, WHITE, "�ɻ�����", 16, 1);

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
//				  SG90_PWM2=OPEN;  //������Ͱ���к�����
//					printf("�к�����\r\n");
							TIM_SetCompare3(TIM3,1850);
							LCD_Clear(WHITE);
							Show_Str(10, 110, BLUE, WHITE, "�к�����", 16, 1);
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
							Show_Str(10, 110, BLUE, WHITE, "�к�����", 16, 1);

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
//					SG90_PWM3=OPEN;   //������Ͱ�ǳ������� 
//					printf("��������\r\n");
					TIM_SetCompare2(TIM3,1850);
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "ʪ����", 16, 1);
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
					Show_Str(10, 110, BLUE, WHITE, "ʪ����", 16, 1);
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
//					SG90_PWM4=OPEN;   //������Ͱ���������� 
//					printf("��������\r\n");
					TIM_SetCompare1(TIM3,1850);
					LCD_Clear(WHITE);
					Show_Str(10, 110, BLUE, WHITE, "������", 16, 1);
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
					Show_Str(10, 110, BLUE, WHITE, "������", 16, 1);
				}
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF)); 
			}
else if(strstr((char*)USART3_RX_BUF,"result:0")!=NULL) //
			{
				cnt1=0;cnt2=0;cnt3=0;cnt4=0;
//				closedj();
//			  printf("������\r\n");//������ 
 	      memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF)); 
			} 
 }
   
}
/*
����3�жϺ���
*/
void USART3_IRQHandler(void)                	//����2�жϷ������
	{
 
		 static u8 i;
 	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{	
  	 
				USART3_RX_BUF[i] = USART_ReceiveData(USART3);
				  i++; 
			if(i > 10) //���� 
			{
				  i=0;
			}
 
 
  }   
 }

/*����4��������ģ���ͨ�Ź���			*/
u16 USART4_RX_STA=0;  //����״̬���	  
u16 USART4_RX_Flag=0; //�������ݱ�־λ
u8  USART4_RX_Tmr=0;  // ������ʱ
u8	USART4_RX_data;
char USART4_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
void Uart4_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��USART1��GPIOA������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	//USART4_TX   GPIOC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10

	//USART4_RX	  GPIOC.11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.11  

	//Usart4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(UART4, &USART_InitStructure); //��ʼ������4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4 
}
void UART4_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(UART4);	//��ȡ���յ�������
		
		if((USART4_RX_STA&0x8000)==0)//����δ���
			{
			if(USART4_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART4_RX_STA=0;//���մ���,���¿�ʼ
				else USART4_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART4_RX_STA|=0x4000;
				else
					{
					USART4_RX_BUF[USART4_RX_STA&0X3FFF]=Res ;
					USART4_RX_STA++;
					if(USART4_RX_STA>(USART_REC_LEN-1))USART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
} 

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    unsigned short count = 0;

    for(; count < len; count++)
        {
            USART_SendData(USARTx, *str++);									//��������

            while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
        }

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{

    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
    va_end(ap);

    while(*pStr != 0)
        {
            USART_SendData(USARTx, *pStr++);

            while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
        }

}

