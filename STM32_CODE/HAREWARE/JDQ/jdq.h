#ifndef __JDQ_H
#define __JDQ_H
#include "sys.h"

#define GPIO_PINI GPIO_Pin_6
extern u8 relay_mode;
void relay_init(void);
void relay_on(void);
void relay_off(void);
void relay_onoff(u8 mode);
#endif

