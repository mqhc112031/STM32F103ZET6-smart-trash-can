
#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP PBout(8)	// PB8
extern u8 alarm;
void BEEP_onoff(u8 alarm);
void BEEP_Init(void);//≥ı ºªØ

		 				    
#endif
