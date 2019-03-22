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
void parser(uint8_t* pData, uint16_t size);
void print(Packet_t *data);
void Config_Load();
void bitClear(uint32_t* flag, uint8_t bit);
void bitSet(uint32_t* flags, uint8_t bit);
void clearFlags(uint32_t* flags);

//FSM
void join(fsm_t* fsm);
void goSleep(fsm_t* fsm);
void send(fsm_t* fsm);
void receive(fsm_t* fsm);
void errorHandle(fsm_t* fsm);
void config(fsm_t* fsm);
void measure(fsm_t* fsm);
void restart(fsm_t* fsm);
void retryoin(fsm_t* fsm);

//Task
void main_task(void* param);
void led_blink(void* param);



#endif
/*
fsm_trans_t trans_table[] = {
                            {START,     STARTED,    JOIN,       join },
                            {JOIN,      JOIN_OK,    SLEEP_S,    goSleep},
                            {JOIN,      JOIN_ERROR, JOIN,       join},
                            {SLEEP_S,   JOIN_OK,    MEASURE,    measure},
							{SLEEP_S,   JOIN_ERROR, START,    	restart},
                            {MEASURE,   MEASURE_OK, TX_S,       send},
                            {TX_S,      TX_OK,      RX_S,       receive},
                            {TX_S,      TX_TIMEOUT, ER_HANDLE,  errorHandle},
                            {RX_S,      RX_OK,      SLEEP_S,    goSleep},
                            {RX_S,      RX_ERROR,   ER_HANDLE,  errorHandle},
                            {RX_S,      RX_NACK,    ER_HANDLE,  errorHandle},
                            {RX_S,      RX_TIMEOUT, ER_HANDLE,  errorHandle},
                            {RX_S,      RX_REQ,     TX_REQ,     send},
                            {RX_S,      RX_CONFIG,  CONFIG,     config},
                            {TX_REQ,    TX_OK,      SLEEP_S,    goSleep},
                            {ER_HANDLE, ERROR_OK,   SLEEP_S,    goSleep},
                            {CONFIG,    CONFIG_OK,  SLEEP_S,    goSleep},
                            {CONFIG,    CONF_ERROR, ER_HANDLE,  errorHandle},

                            {-1, NULL, -1, NULL}
                            };

*/