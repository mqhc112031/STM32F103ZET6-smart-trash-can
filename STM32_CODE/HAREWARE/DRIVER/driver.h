#ifndef _DRIVER_H
#define _DRIVER_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 
//#define LED1 PBout(14)// PB12
//#define LED2 PBout(15)// PB13	
//#define LED3 PBout(13)// PB14
//#define LED4 PBout(12)// PB15	
//#define sensor1 PBin(8)// PB15	
#define sensor PEin(15)// PE15

//#define LED PCout(13)// PE5	

void driver_Init(void);//≥ı ºªØ
int sensors(void);
void open(void);
void close(void);
void opendj(void);
void closedj(void);
#endif

