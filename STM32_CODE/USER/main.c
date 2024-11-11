#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "beep.h"
#include "adc.h"
#include "usart.h"
#include "math.h"
#include "lcd.h"
#include "test.h"
#include "gui.h"
#include "bsp_tcrt5000.h"
#include "timer.h"
#include "rtc.h"
#include "stdio.h"
#include "esp8266.h"
#include "onenet.h"
#include "key.h"
#include "driver.h"
#include "jdq.h"
#define SOFTWARE_VERSION "V10.0"
u16 ad = 0;
u8 i = 0;
float temp;
extern u8 relay_mode;
extern int relay_flag;


u8 IRQURA3 = 0;							// ����3 ����������ϱ�־
u8 USART3_RX_BUF[USART3_REC_LEN] = {0}; // ����2���ݷ���ȥ
char PUB_BUF[256];
const char *devSubTopic[] = {"/solitary/sub"};
const char devPubTopic[] = "/solitary/pub";

void showPPM(float PPM01)
{
    int ppm0;
    int ppm1;
    int ppm2;
    ppm0 = (int)(PPM01 * 100);
    ppm1 = ppm0 / 100;
    ppm2 = ppm0 % 100;
    LCD_ShowString(0, 130, 16, "smokescope:", 1);
    LCD_ShowNum(90, 130, ppm1, 3, 16);
    LCD_ShowString(115, 130, 16, ".", 0);//��ʾ����Ũ��

    if(ppm2 >= 10)
        {
            LCD_ShowNum(125, 130, ppm2, 2, 16);
        }
    else
        {
            //��λ��
            LCD_ShowNum(125, 130, 0, 1, 16); //��8
            LCD_ShowNum(133, 130, ppm2, 1, 16);
        }

    LCD_ShowString(142, 130, 16, "PPM", 0);
    UsartPrintf(USART_DEBUG, "��������PPMΪ��%.2fPPM\r\n", PPM01);


}


int main(void)
{
    float ppm;
    unsigned short timeCount = 0;	//���ͼ������
    unsigned char *dataPtr = NULL;
    vu8 key = 0;
    vu8 key1 = 0;
    int TRCT1;
    int TRCT2;
    int TRCT3;
    int TRCT4;
    int LAX;
    delay_init();
    KEY_Init();
    LED_Init();//���Դ����Ƿ��н��г�ʼ��
    LED0 = !LED0;
    BEEP_Init();
    relay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    LCD_Init();	   //Һ������ʼ��
    RTC_Init();	  			  //RTC��ʼ��
    TIM3_PWM_Init(1999, 719);                         //PWMƵ��=72000000/(719+1)/(1999+1)=50hz=20mS	Ԥ��ƵPrescaler����Ϊ720-1��������Ϊ72M/��Prescaler + 1�� = 100Khz��0.01ms��
    TIM_SetCompare1(TIM3, 1940);                      //�ĸ������ʼ�� 45��   ��״̬����1940���Ǵ�45��ת���ӽ�0��
    TIM_SetCompare2(TIM3, 1940);											//PWM2ģʽ����Ч��ƽģʽΪ�ߵ�ƽ(ǰ��Ч��ƽΪ�͵�ƽ������Ч��ƽΪ�ߵ�ƽ)��������T=20ms,0�ȵ� T/(arr+1)=0.5ms/(arr+1-Z),Z=1950
    TIM_SetCompare3(TIM3, 1940);											//45�ȵ� T/(arr+1)=0.5ms/(arr+1-Z),Z=1900 90�ȵ� T/(arr+1)=0.5ms/(arr+1-Z),Z=1850
    TIM_SetCompare4(TIM3, 1940);											//135�ȵ� T/(arr+1)=0.5ms/(arr+1-Z),Z=1800 180�ȵ� T/(arr+1)=0.5ms/(arr+1-Z),Z=1750
    //	180�ȶ���ο����ӣ�https://blog.csdn.net/weixin_45930808/article/details/119117499?spm=1001.2014.3001.5506
    TIM2_Int_Init(9999, 7199); //��ʼ����ʱ��TIM2��1S��ʱˢ��,PWMƵ��=72000000/(7199+1)/(9999+1)=1hz=1S

    Usart1_Init(115200);	 	//���ڳ�ʼ��Ϊ115200,//debug����
    RTC_Set(2024, 3, 27, 11, 56, 50); //����ʱ��    ȥ����ע������¼��Ȼ��Ƭ����Ҫ�ϵ罫��һ��ע�͵�����¼һ��Ϳ�����
    ADC_Pin_Init();
    UsartPrintf(USART_DEBUG, "Usart1 OK\r\n");
    Uart4_Init(9600);//����ģ��ͨ��
    Usart2_Init(115200);  //stm32-8266ͨѶ����
    usart3_init(9600);//K210
    TRC5000_Init();
    driver_Init();

    UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");

    ESP8266_Init();					//��ʼ��ESP8266
    UsartPrintf(USART_DEBUG, " ESP8266 init OK\r\n");

    while(OneNet_DevLink())			//����OneNET
        delay_ms(500);

    OneNet_Subscribe(devSubTopic, 1);//���ĵ�topic

    hello();
    BEEP = 0;
    delay_ms(100);
    BEEP = 1;

    while(1)
        {
            if(timeCount % 40 == 0)
                {

                    POINT_COLOR = BLUE; //��������Ϊ��ɫ
                    ppm = Adc_ppm(); //MQ2�ɼ����ݣ���ת��ΪPPM��λ
                    showPPM(ppm);

                    //����Ũ�ȳ���30�ͱ���
                    if(ppm > 20)
                        {
                            BEEP = 0;
                        }
                    else
                        {
                            BEEP = 1;
                        }

                }

            show_nowtime();      //��ʾʱ��
            TRCT1 = trc5000D01();
            TRCT2 = trc5000D02();
            TRCT3 = trc5000D03();
            TRCT4 = trc5000D04();
            LAX = sensors();

            if(++timeCount >= 200)									//���ͼ��5s
                {

                    UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
                    sprintf(PUB_BUF, "{\"smoke\":%.2f,\"TRCT1\":%d,\"TRCT2\":%d,\"TRCT3\":%d,\"TRCT4\":%d,\"LAX\":%d}", ppm, TRCT1, TRCT2, TRCT3, TRCT4, LAX);
                    OneNet_Publish(devPubTopic, PUB_BUF);

                    timeCount = 0;
                    ESP8266_Clear();
                }

            dataPtr = ESP8266_GetIPD(3);

            if(dataPtr != NULL)
                OneNet_RevPro(dataPtr);
							
            BEEP_onoff(alarm);
            relay_onoff(relay_mode);
            key = KEY_Scan(0);	//�õ���ֵ

            if(key)
                {
                    POINT_COLOR = BLUE; //��������Ϊ��ɫ
                    key1 = key;
                    //
//                    Show_Str(10, 150, BLACK, WHITE, "��������ϵͳ", 16, 1);
                    key = 0;

                }
            switch(key1)           //��ѯ��ֵ
                {

                    case KEY1_PRES:	//��ʱͶ��ģʽ
                        if((calendar.hour == 7) || (calendar.hour == 8) || (calendar.hour == 11) || (calendar.hour == 12) || (calendar.hour == 18) || (calendar.hour == 19)) //��ǰΪ��Ͷ��ʱ���
                            {
                                if(LAX == 1)
                                    {
                                        if(USART4_RX_STA & 0x8000) //���ڽ��յ�����ģ������
                                            {
                                                open();             //�жϴ�������-�������ͣ������ƶ�Ӧ���
                                                close();            //�ر����ж��
                                                USART4_RX_STA = 0;   //����жϱ�־λ

                                            }

                                    }
                                else
                                    {
																			 USART4_RX_STA = 0;
                                        show_nowtime();
                                        slecttime();		     //�ж�ʱ��
                                    }

                            }
                        else
                            {
                                USART4_RX_STA = 0;
                                show_nowtime();
                                slecttime();//��ʱͶ��
                            }

                        break;

                    case KEY2_PRES:	//ȫ�쿪��ģʽ
                        if(LAX == 1)
                            {
                                if(USART4_RX_STA & 0x8000)
                                    {
//                                LED1 = !LED1;
                                        open();
                                        close();
                                        USART4_RX_STA = 0;

                                    }
                            }
                        else
                            {
                                USART4_RX_STA = 0;
                                show_nowtime();
                                alltime();//ȫ��
                            }

                        break;

                    case KEY3_PRES:	//������������Ͱ
                        opendj();
                        openall();     //OLED��ʾ������Ѵ򿪡�
                        key1 = 0;
                        break;

                    case KEY4_PRES:	//�ر���������Ͱ
                        closedj();
                        closeall();   //OLED��ʾ������ѹرա�
                        key1 = 0;
                        break;

                    case KEY5_PRES:
                        show_nowtime();
                        getResultTemp();
//                        delay_ms(1000);
//                        key1 = 0;
                        break;

                    case KEY6_PRES:
                        relay_on();
                        delay_ms(1000);
                        relay_off();
                        key1 = 0;
                        break;
                }


            delay_ms(10);


        }

}

