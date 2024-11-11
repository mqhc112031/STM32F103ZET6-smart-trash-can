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
_calendar_obj calendar;//时钟结构体
/*
void set_clock(u16 divx)
{
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问

	RTC_EnterConfigMode();/// 允许配置

	RTC_SetPrescaler(divx); //设置RTC预分频的值
	RTC_ExitConfigMode();//退出配置模式
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
}
*/
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级1位,从优先级3位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//先占优先级0位,从优先级4位
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
    NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//实时时钟配置
//初始化RTC时钟,同时检测时钟是否工作正常
//BKP->DR1用于保存是否第一次配置的设置
//返回0:正常
//其他:错误代码

u8 RTC_Init(void)
{
    //检查是不是第一次配置时钟
    u8 temp = 0;

    //其实只需要配置一次，下一次就可以直接使用
    //读取寄存器1的数值---如果等于0x5050表明已经配置了，不等于0x5050就表示没配置
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
        {
            //使能下面这两个时钟（访问后备区域）
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
            //使能后备区访问（每次如果需要修改的话都要打开使能）
            PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问

            //首先配置之前需要复位备份区域
            BKP_DeInit();	//复位备份区域
            //接下来配置时钟，首先要进行使能下面这个时钟，使能外部的32.876khz
            //把外部那个时钟使能了才可以用
            RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振

            //下面是配置等待，如果2.5秒钟还没有配置完成，在表示初始化失败，虽然开启了，但是还未就绪
            while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
                {
                    temp++;
                    delay_ms(10);
                }

            if(temp >= 250)return 1; //初始化时钟失败,晶振有问题


            //如果有问题，下面的代码不会执行
            //没问题的话，把外部时钟设置为RTC时钟源
            //选择RTC的时钟源
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
            //使能
            RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟
            //以上表示RTC时钟已经就绪了


            //下面四条为等待上一次写寄存器完成
            RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
            RTC_WaitForSynchro();		//等待RTC寄存器同步
            //开启了 秒中断，每一秒都会去执行服务中断函数
            RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
            RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成



            //配置过程
            RTC_EnterConfigMode();/// 允许配置
            //由于使用的是32.768khz晶振32.768khz/(32767+1)=1hz即1S一个周期
            RTC_SetPrescaler(32767); //设置RTC预分频的值
            RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
            RTC_Set(2020, 10, 19, 20, 50, 30); //设置当前时间（重点是设置寄存器CNT的值）
            RTC_ExitConfigMode(); //退出配置模式

            //由于我们已经配置了，所以我们要写入0x5050为了最初的判断，方便下次调用RTC的时候去判断是否已经设置过
            BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器中写入用户程序数据
        }
    //如果之前已经配置过了，就会跳过上面的配置步骤，进入下面执行继续计时
    else//系统继续计时
        {

            RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
            RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
            RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        }

    RTC_NVIC_Config();//RCT中断分组设置
    RTC_Get();//更新时间
    return 0; //ok

}
//RTC时钟中断
//每秒触发一次
//extern u16 tcnt;
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
        {
            RTC_Get();//更新时间
        }

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET) //闹钟中断
        {
            RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断
        }

    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);		//清闹钟中断
    RTC_WaitForLastTask();
}
//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{
    if(year % 4 == 0) //必须能被4整除
        {
            if(year % 100 == 0)
                {
                    if(year % 400 == 0)return 1; //如果以00结尾,还要能被400整除
                    else return 0;
                }
            else return 1;
        }
    else return 0;
}
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; //月修正数据表
//平年的月份日期表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0;

    if(syear < 1970 || syear > 2099)return 1;

    for(t = 1970; t < syear; t++)	//把所有年份的秒钟相加
        {
            if(Is_Leap_Year(t))seccount += 31622400; //闰年的秒钟数
            else seccount += 31536000;			 //平年的秒钟数
        }

    smon -= 1;

    for(t = 0; t < smon; t++)	 //把前面月份的秒钟数相加
        {
            seccount += (u32)mon_table[t] * 86400; //月份秒钟数相加

            if(Is_Leap_Year(syear) && t == 1)seccount += 86400; //闰年2月份增加一天的秒钟数
        }

    seccount += (u32)(sday - 1) * 86400; //把前面日期的秒钟数相加
    seccount += (u32)hour * 3600; //小时秒钟数
    seccount += (u32)min * 60;	 //分钟秒钟数
    seccount += sec; //最后的秒钟加上去

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问
    RTC_SetCounter(seccount);	//设置RTC计数器的值

    RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
    return 0;
}
//得到当前的时间
//返回值:0,成功;其他:错误代码.
u8 RTC_Get(void)
{
    static u16 daycnt = 0;
    u32 timecount = 0;
    u32 temp = 0;
    u16 temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400; //得到天数(秒钟数对应的)

    if(daycnt != temp) //超过一天了
        {
            daycnt = temp;
            temp1 = 1970;	//从1970年开始

            while(temp >= 365)
                {
                    if(Is_Leap_Year(temp1))//是闰年
                        {
                            if(temp >= 366)temp -= 366; //闰年的秒钟数
                            else
                                {
                                    temp1++;
                                    break;
                                }
                        }
                    else temp -= 365;	 //平年

                    temp1++;
                }

            calendar.w_year = temp1; //得到年份
            temp1 = 0;

            while(temp >= 28) //超过了一个月
                {
                    if(Is_Leap_Year(calendar.w_year) && temp1 == 1) //当年是不是闰年/2月份
                        {
                            if(temp >= 29)temp -= 29; //闰年的秒钟数
                            else break;
                        }
                    else
                        {
                            if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //平年
                            else break;
                        }

                    temp1++;
                }

            calendar.w_month = temp1 + 1;	//得到月份
            calendar.w_date = temp + 1;  	//得到日期
        }

    temp = timecount % 86400;     		//得到秒钟数
    calendar.hour = temp / 3600;     	//小时
    calendar.min = (temp % 3600) / 60; 	//分钟
    calendar.sec = (temp % 3600) % 60; 	//秒钟
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //获取星期
    return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;

    // 如果为21世纪,年份数加100
    if (yearH > 19)yearL += 100;

    // 所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)temp2--;

    return(temp2 % 7);
}


void show_nowtime(void)
{
    POINT_COLOR = BLUE; //设置字体为蓝色
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
            Show_Str(10, 50, BLUE, WHITE, "限时系统开启", 16, 1);//系统开启

        }
    else
        {
            LCD_Fill(10, 70, 130, 130, WHITE);

//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
            Show_Str(10, 50, BLUE, WHITE, "未到投放时间", 16, 1);//未到投放时间
            //未到投放时间就进行消毒
            relay_on();
            delay_ms(1000);
            relay_off();

        }
}

void alltime(void)
{
    LCD_Fill(10, 50, 130, 90, WHITE);
//		Show_Str(10, 50, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 90, BLUE, WHITE, "全天开启模式", 16, 1); //全天开启模式

}

void hello(void)
{
    DrawTestPage("智能垃圾分类系统");
}

void openall(void)
{
    LCD_Clear(WHITE);
    Show_Str(10, 70, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 50, BLUE, WHITE, "垃圾桶已开启", 16, 1);

}

void closeall(void)
{
    LCD_Clear(WHITE);
    Show_Str(10, 70, WHITE, WHITE, "           ", 16, 1);
    Show_Str(10, 50, BLUE, WHITE, "垃圾桶已关闭", 16, 1);

}

//	else  if((calendar.hour==11)&&(calendar.hour==12))
//	{
//	 if(calendar.hour==11)
//	 {
//		OLED_ShowChinese(30,24,13,16,1);//系统开启
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//	 }
//	else if(calendar.hour==12 )
//	 {
//		if(calendar.min<=30)
//		 {
//		OLED_ShowChinese(30,24,13,16,1);//系统开启
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//		 }
//		  else
//		{
//		 OLED_ShowChinese(14,24,44,16,1);//未到投放时间
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
//		OLED_ShowChinese(30,24,13,16,1);//系统开启
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//	 }
//	 if(calendar.hour==19 )
//	 {
//		if(calendar.min<=30)
//		 {
//		OLED_ShowChinese(30,24,13,16,1);//系统开启
//		OLED_ShowChinese(46,24,14,16,1);
//	  OLED_ShowChinese(62,24,17,16,1);
//		OLED_ShowChinese(78,24,18,16,1);
//		 }
//		   else
//		{
//		 OLED_ShowChinese(14,24,44,16,1);//未到投放时间
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











