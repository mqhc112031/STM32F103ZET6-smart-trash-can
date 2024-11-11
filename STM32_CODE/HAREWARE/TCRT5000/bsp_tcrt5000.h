#ifndef __BSP_TCRT5000_H
#define __BSP_TCRT5000_H

#include "stm32f10x.h"

#define Tracking_DO1		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define Tracking_DO2		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define Tracking_DO3		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)
#define Tracking_DO4		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)

void TRC5000_Init(void);
int trc5000D01(void);
int trc5000D02(void);
int trc5000D03(void);
int trc5000D04(void);
#endif /*__BSP_TCRT5000_H*/
