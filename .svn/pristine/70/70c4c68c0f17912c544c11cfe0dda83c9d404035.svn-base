

#include "LowPower.h"


static RTC_HandleTypeDef RtcHandle ={0};
/**
 * 	@breif	Init the RTC device
 * 	@param	none
 * 	@retval	none
 * 
 */

void InitLPM(){

	RtcHandle.Instance				= RTC;
	RtcHandle.Init.AsynchPrediv		= ASYNPREDIV;
	RtcHandle.Init.SynchPrediv		= SYNPREDIV;
	RtcHandle.Init.HourFormat		= RTC_HOURFORMAT_24;			//24 hour format
	RtcHandle.Init.OutPut			= RTC_OUTPUT_WAKEUP;			// which output will be routed to the outpur
	RtcHandle.Init.OutPutPolarity	= RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType		= RTC_OUTPUT_TYPE_OPENDRAIN; 	// Why not?

	if(HAL_RTC_Init(&RtcHandle) != HAL_OK){
		while(1); //:( bad, so bad
	}

    HAL_NVIC_EnableIRQ(RTC_IRQn);
    HAL_NVIC_SetPriority(RTC_IRQn, 0,0);
}

/**
 * 	@Brief	Set the low-power mode
 * 	@param	[MilliSeconds] sleep time
 * 	@retval	none
 */ 

void GoBed(uint32_t MilliSeconds){
    HW_RTC_SetAlarm(HW_RTC_ms2Tick(MilliSeconds));

	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWREx_EnableUltraLowPower();			//Enable ultra low powe mode
	HAL_PWREx_EnableFastWakeUp();				//Enable fast wakeUp mode
    //ToDo ->  Poweroff all devices!
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}
/****/
void GoodMorning(){
    HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
}

