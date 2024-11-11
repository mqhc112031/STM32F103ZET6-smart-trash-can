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


u8 IRQURA3 = 0;							// 串口3 接受数据完毕标志
u8 USART3_RX_BUF[USART3_REC_LEN] = {0}; // 串口2数据反冲去
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
    LCD_ShowString(115, 130, 16, ".", 0);//显示烟雾浓度

    if(ppm2 >= 10)
        {
            LCD_ShowNum(125, 130, ppm2, 2, 16);
        }
    else
        {
            //个位数
            LCD_ShowNum(125, 130, 0, 1, 16); //加8
            LCD_ShowNum(133, 130, ppm2, 1, 16);
        }

    LCD_ShowString(142, 130, 16, "PPM", 0);
    UsartPrintf(USART_DEBUG, "检测烟雾的PPM为：%.2fPPM\r\n", PPM01);


}


int main(void)
{
    float ppm;
    unsigned short timeCount = 0;	//发送间隔变量
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
    LED_Init();//测试代码是否有进行初始化
    LED0 = !LED0;
    BEEP_Init();
    relay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    LCD_Init();	   //液晶屏初始化
    RTC_Init();	  			  //RTC初始化
    TIM3_PWM_Init(1999, 719);                         //PWM频率=72000000/(719+1)/(1999+1)=50hz=20mS	预分频Prescaler设置为720-1，则单周期为72M/（Prescaler + 1） = 100Khz（0.01ms）
    TIM_SetCompare1(TIM3, 1940);                      //四个舵机初始化 45度   打开状态，而1940则是从45度转到接近0度
    TIM_SetCompare2(TIM3, 1940);											//PWM2模式，有效电平模式为高电平(前无效电平为低电平，后有效电平为高电平)，周期是T=20ms,0度的 T/(arr+1)=0.5ms/(arr+1-Z),Z=1950
    TIM_SetCompare3(TIM3, 1940);											//45度的 T/(arr+1)=0.5ms/(arr+1-Z),Z=1900 90度的 T/(arr+1)=0.5ms/(arr+1-Z),Z=1850
    TIM_SetCompare4(TIM3, 1940);											//135度的 T/(arr+1)=0.5ms/(arr+1-Z),Z=1800 180度的 T/(arr+1)=0.5ms/(arr+1-Z),Z=1750
    //	180度舵机参考链接：https://blog.csdn.net/weixin_45930808/article/details/119117499?spm=1001.2014.3001.5506
    TIM2_Int_Init(9999, 7199); //初始化定时器TIM2，1S定时刷新,PWM频率=72000000/(7199+1)/(9999+1)=1hz=1S

    Usart1_Init(115200);	 	//串口初始化为115200,//debug串口
    RTC_Set(2024, 3, 27, 11, 56, 50); //设置时间    去掉备注编译烧录，然后单片机不要断电将这一行注释掉再烧录一遍就可以了
    ADC_Pin_Init();
    UsartPrintf(USART_DEBUG, "Usart1 OK\r\n");
    Uart4_Init(9600);//语音模块通信
    Usart2_Init(115200);  //stm32-8266通讯串口
    usart3_init(9600);//K210
    TRC5000_Init();
    driver_Init();

    UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");

    ESP8266_Init();					//初始化ESP8266
    UsartPrintf(USART_DEBUG, " ESP8266 init OK\r\n");

    while(OneNet_DevLink())			//接入OneNET
        delay_ms(500);

    OneNet_Subscribe(devSubTopic, 1);//订阅的topic

    hello();
    BEEP = 0;
    delay_ms(100);
    BEEP = 1;

    while(1)
        {
            if(timeCount % 40 == 0)
                {

                    POINT_COLOR = BLUE; //设置字体为蓝色
                    ppm = Adc_ppm(); //MQ2采集数据，并转换为PPM单位
                    showPPM(ppm);

                    //烟雾浓度超过30就报警
                    if(ppm > 20)
                        {
                            BEEP = 0;
                        }
                    else
                        {
                            BEEP = 1;
                        }

                }

            show_nowtime();      //显示时间
            TRCT1 = trc5000D01();
            TRCT2 = trc5000D02();
            TRCT3 = trc5000D03();
            TRCT4 = trc5000D04();
            LAX = sensors();

            if(++timeCount >= 200)									//发送间隔5s
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
            key = KEY_Scan(0);	//得到键值

            if(key)
                {
                    POINT_COLOR = BLUE; //设置字体为蓝色
                    key1 = key;
                    //
//                    Show_Str(10, 150, BLACK, WHITE, "智能垃圾系统", 16, 1);
                    key = 0;

                }
            switch(key1)           //查询键值
                {

                    case KEY1_PRES:	//限时投放模式
                        if((calendar.hour == 7) || (calendar.hour == 8) || (calendar.hour == 11) || (calendar.hour == 12) || (calendar.hour == 18) || (calendar.hour == 19)) //当前为可投放时间段
                            {
                                if(LAX == 1)
                                    {
                                        if(USART4_RX_STA & 0x8000) //串口接收到语音模块数据
                                            {
                                                open();             //判断串口数据-垃圾类型，并控制对应舵机
                                                close();            //关闭所有舵机
                                                USART4_RX_STA = 0;   //清除中断标志位

                                            }

                                    }
                                else
                                    {
																			 USART4_RX_STA = 0;
                                        show_nowtime();
                                        slecttime();		     //判断时间
                                    }

                            }
                        else
                            {
                                USART4_RX_STA = 0;
                                show_nowtime();
                                slecttime();//限时投放
                            }

                        break;

                    case KEY2_PRES:	//全天开启模式
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
                                alltime();//全天
                            }

                        break;

                    case KEY3_PRES:	//开启所有垃圾桶
                        opendj();
                        openall();     //OLED显示“舵机已打开”
                        key1 = 0;
                        break;

                    case KEY4_PRES:	//关闭所有垃圾桶
                        closedj();
                        closeall();   //OLED显示“舵机已关闭”
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

