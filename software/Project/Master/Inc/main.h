#ifndef __MAIN_H__

#define __MAIN_H__

#include "fsm.h"
#include "types.h"
#include "cmsis_os.h"
#include "hw.h"									//Hardware configuration
#include "radio.h"								//phy mac implementation
#include "LowPower.h"
#include "system_config.h"
#include "trace.h"


/*Prototypes*/

//Radio
void radio_init();
//-> Radio callbacks
void onTxTimeout();
void onTxDone();
void onRxTimeout();
void onRxError();
void onFhssChangeChannel(uint8_t currentChannel);									//HSS Change Channel callback prototype;
void onCadDone(bool channelActivityDetected);
void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

//utils
void wakeUpSystem();
Packet_t* parser(uint8_t* pData, uint16_t size);
void print(Packet_t *data);
void Config_Load();
void bitClear(uint32_t* flag, uint8_t bit);
void bitSet(uint32_t* flags, uint8_t bit);
void clearFlags(uint32_t* flags);
void processJoin(Packet_t* inData);

//FSM


//Task
void main_task(void* param);
void led_blink(void* param);



#endif
