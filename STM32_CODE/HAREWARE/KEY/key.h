#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2


//#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1)//��ȡ����0
#define KEY2  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)//��ȡ����1
#define KEY3  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3)//��ȡ����2 
#define KEY4  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)//��ȡ����3 
#define KEY5  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����3 
#define KEY6  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����3 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����4(WK_UP) 

//#define KEY1 PEin(3)   	//PE3
//#define KEY1 PGin(1)   	//PG1
//#define KEY2 PGin(2)	//PE3 
//#define KEY3 PGin(3)	//PE2
//#define KEY4 PGin(4)	//PE2 
//#define KEY5 PEin(3)	//PE4
//#define KEY6 PEin(4)	//PE4
//#define WK_UP PAin(0)	//PA0  WK_UP ���Ѽ�

#define KEY1_PRES 1	//KEY0����
#define KEY2_PRES	2	//KEY1����
#define KEY3_PRES	3	//KEY2����
#define KEY4_PRES	4	//KEY2����
#define KEY5_PRES	5	//KEY2����
#define KEY6_PRES	6	//KEY2����
#define WKUP_PRES 7	//KEY_UP����(��WK_UP/KEY_UP)


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
