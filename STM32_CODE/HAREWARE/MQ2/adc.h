#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f10x.h"

void ADC_Pin_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
float Adc_ppm(void);
#endif
