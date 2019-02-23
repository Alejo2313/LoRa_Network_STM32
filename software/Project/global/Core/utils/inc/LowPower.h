#ifndef __LPM_H__
#define __LPM_H__

//debug

#include "hw.h"



#define	TRC_CLK			LSI_VALUE
#define	ASYNPREDIV		124										//Caledar_CLK = (RTC_CLK)/((ASYNPREDIV+1)*(SYNPREDIV+1))
#define	SYNPREDIV 		295
#define TOWAKEUPTIME(MILLIS) (uint32_t)((MILLIS*TRC_CLK)/(16*1000))		//Converts millis to CLK ticks



void InitLPM();
void GoBed(uint32_t millis);
void GoodMorning();


#endif


