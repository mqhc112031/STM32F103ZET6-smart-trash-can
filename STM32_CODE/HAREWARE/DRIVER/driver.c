#include "driver.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "gui.h"
//////////////////////////////////////////////////////////////////////////////////
//初始化PB5和PE5为输出口.并使能这两个口的时钟
//LED IO初始化
extern const unsigned char gImage_ganlaji[];
extern const unsigned char gImage_shilaji[];
extern const unsigned char gImage_youhailaji[];
extern const unsigned char gImage_kehuishouwu[];
void driver_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);	 //使能PB,PE端口时钟

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
// GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
// GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

// GPIO_SetBits(GPIOB,GPIO_Pin_13);		//PB.5 输出高
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 端口配置, 推挽输出
// GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
// GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 输出高


    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//sensor PE15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
    GPIO_ResetBits(GPIOE, GPIO_Pin_15); 						 //PE.5 输出高
    //这里E15是人体感应，PB12,13,14,15为LED，PC13也是LED

}
int sensors(void)
{
    if(sensor == 1)
        {
//			UsartPrintf(USART_DEBUG,"TRCD010\r\n");//有人
            return 0;
        }
    else
        {
//			UsartPrintf(USART_DEBUG,"TRCD011\r\n");//满了
            return 1;
        }


}

void open(void)                                 //对接收的数据进行判断
{
    if(USART4_RX_BUF[0] == '1')                   //第0位为1，则对电机进行驱动操作
        {
            POINT_COLOR = BLUE; //设置字体为蓝色
            TIM_SetCompare1(TIM3, 1850);
            LCD_Clear(WHITE);
//	  LED1=0;
//		printf("干垃圾\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "干垃圾", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '2')                                        //未对电机使能，则关闭电机
        {
//    LED2=0;
            POINT_COLOR = BLUE; //设置字体为蓝色
            TIM_SetCompare2(TIM3, 1850);
            LCD_Clear(WHITE);
//		printf("湿垃圾\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "湿垃圾", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '4')                                        //未对电机使能，则关闭电机
        {
//    LED3=0;
            POINT_COLOR = BLUE; //设置字体为蓝色
            TIM_SetCompare4(TIM3, 1850);
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "可回收物", 16, 1);
        }

    else if(USART4_RX_BUF[0] == '3')                                        //未对电机使能，则关闭电机
        {
//    LED4=0;
            POINT_COLOR = BLUE; //设置字体为蓝色
            TIM_SetCompare3(TIM3, 1850);
            LCD_Clear(WHITE);
//		printf("有害垃圾\n\r\n");
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
            LCD_Clear(WHITE);
            Show_Str(10, 110, BLUE, WHITE, "有害垃圾", 16, 1);
        }

}

void close(void)
{
    UsartPrintf(USART_DEBUG, "\r\n 关闭\n\r\n");
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

void opendj(void)        //1900桶盖打开
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

void closedj(void)       //1940桶盖关闭
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
