#include "driver.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "gui.h"
//////////////////////////////////////////////////////////////////////////////////
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��
//LED IO��ʼ��
extern const unsigned char gImage_ganlaji[];
extern const unsigned char gImage_shilaji[];
extern const unsigned char gImage_youhailaji[];
extern const unsigned char gImage_kehuishouwu[];
void driver_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 �˿�����
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
// GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

// GPIO_SetBits(GPIOB,GPIO_Pin_13);		//PB.5 �����
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
// GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
// GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 �����


    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//sensor PE15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
    GPIO_ResetBits(GPIOE, GPIO_Pin_15); 						 //PE.5 �����
    //����E15�������Ӧ��PB12,13,14,15ΪLED��PC13Ҳ��LED

}
int sensors(void)
{
    if(sensor == 1)
        {
//			UsartPrintf(USART_DEBUG,"TRCD010\r\n");//����
            return 0;
        }
    else
        {
//			UsartPrintf(USART_DEBUG,"TRCD011\r\n");//����
            return 1;
        }


}

void open(void)                                 //�Խ��յ����ݽ����ж�
{
    if(USART4_RX_BUF[0] == '1')                   //��0λΪ1����Ե��������������
        {
            POINT_COLOR = BLUE; //��������Ϊ��ɫ
            TIM_SetCompare1(TIM3, 1850);
            LCD_Clear(WHITE);
//	  LED1=0;
//		printf("������\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "������", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '2')                                        //δ�Ե��ʹ�ܣ���رյ��
        {
//    LED2=0;
            POINT_COLOR = BLUE; //��������Ϊ��ɫ
            TIM_SetCompare2(TIM3, 1850);
            LCD_Clear(WHITE);
//		printf("ʪ����\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "ʪ����", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '4')                                        //δ�Ե��ʹ�ܣ���رյ��
        {
//    LED3=0;
            POINT_COLOR = BLUE; //��������Ϊ��ɫ
            TIM_SetCompare4(TIM3, 1850);
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "�ɻ�����", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '3')                                        //δ�Ե��ʹ�ܣ���رյ��
        {
//    LED4=0;
            POINT_COLOR = BLUE; //��������Ϊ��ɫ
            TIM_SetCompare3(TIM3, 1850);
            LCD_Clear(WHITE);
//		printf("�к�����\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "�к�����", 16, 1);
        }

}

void close(void)
{
    UsartPrintf(USART_DEBUG, "\r\n �ر�\n\r\n");
    TIM_SetCompare1(TIM3, 1940);
    TIM_SetCompare2(TIM3, 1940);
    TIM_SetCompare3(TIM3, 1940);
    TIM_SetCompare4(TIM3, 1940);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
    delay_ms(500);
//	LED1=1;
//	LED2=1;
//	LED3=1;
//	LED4=1;
}

void opendj(void)        //1900Ͱ�Ǵ�
{
    TIM_SetCompare1(TIM3, 1850);
    delay_ms(200);
    TIM_SetCompare2(TIM3, 1850);
    delay_ms(200);
    TIM_SetCompare3(TIM3, 1850);
    delay_ms(200);
    TIM_SetCompare4(TIM3, 1850);
    delay_ms(200);
}

void closedj(void)       //1940Ͱ�ǹر�
{
    TIM_SetCompare1(TIM3, 1940);
    delay_ms(200);
    TIM_SetCompare2(TIM3, 1940);
    delay_ms(200);
    TIM_SetCompare3(TIM3, 1940);
    delay_ms(200);
    TIM_SetCompare4(TIM3, 1940);
    delay_ms(200);
}
