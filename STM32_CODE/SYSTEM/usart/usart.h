#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "stm32f10x.h"
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
#define USART_REC_LEN     400  	//�����������ֽ��� 400 
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define USART3_REC_LEN 100
#define USART_DEBUG		USART1		//���Դ�ӡ��ʹ�õĴ�����	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

extern u8 USART3_RX_BUF[USART3_REC_LEN]; // ����3���ݻ�����
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
//����봮���жϽ��գ��벻Ҫע�����º궨��
void Usart1_Init(unsigned int baud);
void Usart2_Init(unsigned int baud);

void usart3_init(u32 band);//����2 ��ʼ������
void usart3_SendStringByLen(unsigned char * data,u8 len);//
void usart3_SendString(unsigned char * data);
u16 usart3_receive(u32 time);
void usart3_Send_Data(u8 *buf,u16 len);
void USART3SendByte(unsigned char SendData);
void getResultTemp(void);

extern char  USART4_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�   
extern u16 USART4_RX_STA; 
void Uart4_Init(u32 bound);
void UART4_IRQHandler(void);
#endif


