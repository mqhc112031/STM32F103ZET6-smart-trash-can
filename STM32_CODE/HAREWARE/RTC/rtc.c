#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "gui.h"
#include "lcd.h"
#include "test.h"
#include "jdq.h"
_calendar_obj calendar;//ʱ�ӽṹ��
/*
void set_clock(u16 divx)
{
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ�������

	RTC_EnterConfigMode();/// ��������

	RTC_SetPrescaler(divx); //����RTCԤ��Ƶ��ֵ
	RTC_ExitConfigMode();//�˳�����ģʽ
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
}
*/
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�1λ,�����ȼ�3λ
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
    NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������

u8 RTC_Init(void)
{
    //����ǲ��ǵ�һ������ʱ��
    u8 temp = 0;

    //��ʵֻ��Ҫ����һ�Σ���һ�ξͿ���ֱ��ʹ��
    //��ȡ�Ĵ���1����ֵ---�������0x5050�����Ѿ������ˣ�������0x5050�ͱ�ʾû����
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
        {
            //ʹ������������ʱ�ӣ����ʺ�����
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��
            //ʹ�ܺ������ʣ�ÿ�������Ҫ�޸ĵĻ���Ҫ��ʹ�ܣ�
            PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������

            //��������֮ǰ��Ҫ��λ��������
            BKP_DeInit();	//��λ��������
            //����������ʱ�ӣ�����Ҫ����ʹ���������ʱ�ӣ�ʹ���ⲿ��32.876khz
            //���ⲿ�Ǹ�ʱ��ʹ���˲ſ�����
            RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���

            //���������õȴ������2.5���ӻ�û��������ɣ��ڱ�ʾ��ʼ��ʧ�ܣ���Ȼ�����ˣ����ǻ�δ����
            while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
                {
                    temp++;
                    delay_ms(10);
                }

            if(temp >= 250)return 1; //��ʼ��ʱ��ʧ��,����������


            //��������⣬����Ĵ��벻��ִ��
            //û����Ļ������ⲿʱ������ΪRTCʱ��Դ
            //ѡ��RTC��ʱ��Դ
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
            //ʹ��
            RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��
            //���ϱ�ʾRTCʱ���Ѿ�������


            //��������Ϊ�ȴ���һ��д�Ĵ������
            RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
            RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��
            //������ ���жϣ�ÿһ�붼��ȥִ�з����жϺ���
            RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
            RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������



            //���ù���
            RTC_EnterConfigMode();/// ��������
            //����ʹ�õ���32.768khz����32.768khz/(32767+1)=1hz��1Sһ������
            RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
            RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
            RTC_Set(2020, 10, 19, 20, 50, 30); //���õ�ǰʱ�䣨�ص������üĴ���CNT��ֵ��
            RTC_ExitConfigMode(); //�˳�����ģʽ

            //���������Ѿ������ˣ���������Ҫд��0x5050Ϊ��������жϣ������´ε���RTC��ʱ��ȥ�ж��Ƿ��Ѿ����ù�
            BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
        }
    //���֮ǰ�Ѿ����ù��ˣ��ͻ�������������ò��裬��������ִ�м�����ʱ
    else//ϵͳ������ʱ
        {

            RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
            RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
            RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
        }

    RTC_NVIC_Config();//RCT�жϷ�������
    RTC_Get();//����ʱ��
    return 0; //ok

}
//RTCʱ���ж�
//ÿ�봥��һ��
//extern u16 tcnt;
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����ж�
        {
            RTC_Get();//����ʱ��
        }

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET) //�����ж�
        {
            RTC_ClearITPendingBit(RTC_IT_ALR);		//�������ж�
        }

    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);		//�������ж�
    RTC_WaitForLastTask();
}
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{
    if(year % 4 == 0) //�����ܱ�4����
        {
            if(year % 100 == 0)
                {
                    if(year % 400 == 0)return 1; //�����00��β,��Ҫ�ܱ�400����
                    else return 0;
                }
            else return 1;
        }
    else return 0;
}
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�
u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //���������ݱ�
//ƽ����·����ڱ�
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0;

    if(syear < 1970 || syear > 2099)return 1;

    for(t = 1970; t < syear; t++)	//��������ݵ��������
        {
            if(Is_Leap_Year(t))seccount += 31622400; //�����������
            else seccount += 31536000;			 //ƽ���������
        }

    smon -= 1;

    for(t = 0; t < smon; t++)	 //��ǰ���·ݵ����������
        {
            seccount += (u32)mon_table[t] * 86400; //�·����������

            if(Is_Leap_Year(syear) && t == 1)seccount += 86400; //����2�·�����һ���������
        }

    seccount += (u32)(sday - 1) * 86400; //��ǰ�����ڵ����������
    seccount += (u32)hour * 3600; //Сʱ������
    seccount += (u32)min * 60;	 //����������
    seccount += sec; //�������Ӽ���ȥ

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��
    PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ�������
    RTC_SetCounter(seccount);	//����RTC��������ֵ

    RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
    return 0;
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
    static u16 daycnt = 0;
    u32 timecount = 0;
    u32 temp = 0;
    u16 temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400; //�õ�����(��������Ӧ��)

    if(daycnt != temp) //����һ����
        {
            daycnt = temp;
            temp1 = 1970;	//��1970�꿪ʼ

            while(temp >= 365)
                {
                    if(Is_Leap_Year(temp1))//������
                        {
                            if(temp >= 366)temp -= 366; //�����������
                            else
                                {
                                    temp1++;
                                    break;
                                }
                        }
                    else temp -= 365;	 //ƽ��

                    temp1++;
                }

            calendar.w_year = temp1; //�õ����
            temp1 = 0;

            while(temp >= 28) //������һ����
                {
                    if(Is_Leap_Year(calendar.w_year) && temp1 == 1) //�����ǲ�������/2�·�
                        {
                            if(temp >= 29)temp -= 29; //�����������
                            else break;
                        }
                    else
                        {
                            if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //ƽ��
                            else break;
                        }

                    temp1++;
                }

            calendar.w_month = temp1 + 1;	//�õ��·�
            calendar.w_date = temp + 1;  	//�õ�����
        }

    temp = timecount % 86400;     		//�õ�������
    calendar.hour = temp / 3600;     	//Сʱ
    calendar.min = (temp % 3600) / 60; 	//����
    calendar.sec = (temp % 3600) % 60; 	//����
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //��ȡ����
    return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;

    // ���Ϊ21����,�������100
    if (yearH > 19)yearL += 100;

    // ����������ֻ��1900��֮���
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)temp2--;

    return(temp2 % 7);
}


void show_nowtime(void)
{
    POINT_COLOR = BLUE; //��������Ϊ��ɫ
    LCD_ShowString(30, 30, 16, ":", 1);
    LCD_ShowString(60, 30, 16, ":", 1);

    if(calendar.min <= 9)
        {
            LCD_ShowNum(39, 30, 0, 1, 16);
            LCD_ShowNum(48, 30, calendar.min, 1, 16);
        }

    if(calendar.min >= 10)
        {
            LCD_ShowNum(39, 30, calendar.min, 2, 16);
        }

    if(calendar.sec <= 9)
        {
            LCD_ShowNum(70, 30, 0, 1, 16);
            LCD_ShowNum(79, 30, calendar.sec, 1, 16);
        }

    if(calendar.sec >= 10)
        {
            LCD_ShowNum(70, 30, calendar.sec, 2, 16);
        }

    LCD_ShowNum(12, 30, calendar.hour, 2, 16);
    LCD_ShowString(30, 30, 16, ":", 1);
    LCD_ShowString(60, 30, 16, ":", 1);
//  OLED_Refresh();
//	delay_ms(1000);
//  OLED_Clear();
}

void slecttime(void)
{
    if((calendar.hour == 7) || (calendar.hour == 8) || (calendar.hour == 11) || (calendar.hour == 12) || (calendar.hour == 18) || (calendar.hour == 19))
        {
            LCD_Fill(10, 70, 130, 130, WHITE);
//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
            Show_Str(10, 50, BLUE, WHITE, "��ʱϵͳ����", 16, 1);//ϵͳ����

        }
    else
        {
            LCD_Fill(10, 70, 130, 130, WHITE);

//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
            Show_Str(10, 50, BLUE, WHITE, "δ��Ͷ��ʱ��", 16, 1);//δ��Ͷ��ʱ��
            //δ��Ͷ��ʱ��ͽ�������
            relay_on();
            delay_ms(1000);
            relay_off();

        }
}

void alltime(void)
{
    LCD_Fill(10, 50, 130, 90, WHITE);
//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 90, BLUE, WHITE, "ȫ�쿪��ģʽ", 16, 1); //ȫ�쿪��ģʽ

}

void hello(void)
{
    DrawTestPage("������������ϵͳ");
}

void openall(void)
{
    LCD_Clear(WHITE);
    Show_Str(10, 70, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 50, BLUE, WHITE, "����Ͱ�ѿ���", 16, 1);

}

void closeall(void)
{
    LCD_Clear(WHITE);
    Show_Str(10, 70, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 50, BLUE, WHITE, "����Ͱ�ѹر�", 16, 1);

}

//	else  if((calendar.hour==11)&&(calendar.hour==12))
//	{
//	 if(calendar.hour==11)
//	 {
//		OLED_ShowChinese(30,24,13,16,1);//ϵͳ����
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//	 }
//	else if(calendar.hour==12 )
//	 {
//		if(calendar.min<=30)
//		 {
//		OLED_ShowChinese(30,24,13,16,1);//ϵͳ����
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//		 }
//		  else
//		{
//		 OLED_ShowChinese(14,24,44,16,1);//δ��Ͷ��ʱ��
//		OLED_ShowChinese(30,24,45,16,1);
//	  OLED_ShowChinese(46,24,46,16,1);
//		OLED_ShowChinese(62,24,47,16,1);
//		OLED_ShowChinese(78,24,48,16,1);
//		OLED_ShowChinese(94,24,49,16,1);
//		}
//	 }
//	 }
//	 	else  if((calendar.hour==18)&&(calendar.hour==19))
//	{
//	 if(calendar.hour==18)
//	 {
//		OLED_ShowChinese(30,24,13,16,1);//ϵͳ����
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//	 }
//	 if(calendar.hour==19 )
//	 {
//		if(calendar.min<=30)
//		 {
//		OLED_ShowChinese(30,24,13,16,1);//ϵͳ����
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//		 }
//		   else
//		{
//		 OLED_ShowChinese(14,24,44,16,1);//δ��Ͷ��ʱ��
//		OLED_ShowChinese(30,24,45,16,1);
//	  OLED_ShowChinese(46,24,46,16,1);
//		OLED_ShowChinese(62,24,47,16,1);
//		OLED_ShowChinese(78,24,48,16,1);
//		OLED_ShowChinese(94,24,49,16,1);
//		}
//
//	 }
//
//	}










