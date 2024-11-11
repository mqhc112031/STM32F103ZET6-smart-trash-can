#include "adc.h"
#include "delay.h"
#include "math.h"
/*******************************************************************************
*函数的原型：void ADC_Pin_Init(void)
*函数的功能：GPIO初始化
*函数的参数：None
*函数返回值：None
*函数的说明：PA0 ADC1_IN0 模拟输入
*函数编写者：宇
*函数编写日期：2021/2/28
*函数的版本号：V1.0
********************************************************************************/
u16 value = 0;
float RS;
float R0;
float vol;
float ppm;
void ADC_Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                           | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值


    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//单次转换
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStruct.ADC_NbrOfChannel = 1;//转换总数
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;//单通道扫描
    ADC_Init(ADC1, &ADC_InitStruct);

    //转换通道 第几次转换 采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);	//使能复位校准

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

    ADC_StartCalibration(ADC1);	 //开启AD校准

    while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}

/*******************************************************************************
*函数的原型：u16 ADC_Trans(void)
*函数的功能：ADC读取数据
*函数的参数：None
*函数返回值：
	@ u16：采样50次的平均值
*函数的说明：
*函数编写者：宇
*函数编写日期：2021/2/28
*函数的版本号：V1.0
********************************************************************************/
//u16 ADC_Trans(void)
//{
//	u16 adc_value = 0;
//	u8 i = 0;
//
//	for(i = 0; i < 50; i++)
//	{
//		//开始转换
//		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
//
//		//转换是否结束
//		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET);
//		adc_value = adc_value + ADC_GetConversionValue(ADC1);//读ADC中的值
//	}
//
//	return adc_value / 50;
//}
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

    return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    for(t = 0; t < times; t++)
        {
            temp_val += Get_Adc(ch);
            delay_ms(5);
        }

    return temp_val / times;
}
float Adc_ppm(void)
{
    value = Get_Adc_Average(ADC_Channel_1, 20);

    vol = (float)value * (3.3 / 4096);

    RS = (5 - vol) / vol * 0.5;
    R0 = 6.64;
    ppm = pow(11.5428 * R0 / RS, 0.6549f);
    return ppm;
}
