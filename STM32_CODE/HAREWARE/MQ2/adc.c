#include "adc.h"
#include "delay.h"
#include "math.h"
/*******************************************************************************
*������ԭ�ͣ�void ADC_Pin_Init(void)
*�����Ĺ��ܣ�GPIO��ʼ��
*�����Ĳ�����None
*��������ֵ��None
*������˵����PA0 ADC1_IN0 ģ������
*������д�ߣ���
*������д���ڣ�2021/2/28
*�����İ汾�ţ�V1.0
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
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ


    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//����ת��
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//���ݶ���
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
    ADC_InitStruct.ADC_NbrOfChannel = 1;//ת������
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;//��ͨ��ɨ��
    ADC_Init(ADC1, &ADC_InitStruct);

    //ת��ͨ�� �ڼ���ת�� ����ʱ��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); //

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADC1);	 //����ADУ׼

    while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
}

/*******************************************************************************
*������ԭ�ͣ�u16 ADC_Trans(void)
*�����Ĺ��ܣ�ADC��ȡ����
*�����Ĳ�����None
*��������ֵ��
	@ u16������50�ε�ƽ��ֵ
*������˵����
*������д�ߣ���
*������д���ڣ�2021/2/28
*�����İ汾�ţ�V1.0
********************************************************************************/
//u16 ADC_Trans(void)
//{
//	u16 adc_value = 0;
//	u8 i = 0;
//
//	for(i = 0; i < 50; i++)
//	{
//		//��ʼת��
//		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
//
//		//ת���Ƿ����
//		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET);
//		adc_value = adc_value + ADC_GetConversionValue(ADC1);//��ADC�е�ֵ
//	}
//
//	return adc_value / 50;
//}
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

    return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
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
