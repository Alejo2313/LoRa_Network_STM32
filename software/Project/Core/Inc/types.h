/*
 * types.h
 *
 *  Created on: 21 oct. 2018
 *      Author: Alejo
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <stdint.h>

/* Types */


typedef struct{
	bool Joined;
	uint16_t DevAddr;
	uint16_t GateAddr;
	uint32_t SleepTime;
	//ohters.....

	uint8_t RXWIndow;
	uint8_t NetAddr;
	
} config_t;



typedef enum {
	SLEEP_S,
	MEASURE,
	TX_S,
	RX_S,
	ERROR_HANDLE,
	CONFIG_S,
	JOIN_S
} State_t;



typedef enum {
	RX_ERROR,
	RX_TIMEOUT,
	RX_NACK,
	TX_TIMEOUT,
	NO_ERROR,
	RX_REJOIN

}Error_t;



typedef enum {
	LIGTH,
	PRESURE,
	HUMIDITY,
	TEMPERATURE,
	CONFIG_SF,
	CONFIG_BW,
	CONFIG_POWER,
	RSSI,
	RX_PWR
}Data_t;

 typedef enum {
	TX_T,
	ACK,
	NACK,
	JOIN
 }Mac_t;

typedef enum{
	DATA,
	CONFIG,
	REQ_INFO
}payload_t;


 typedef struct {
	uint8_t NettAddr;
 	uint16_t devAddr;
	uint16_t devDest;
 	Mac_t MacType;
	uint8_t flags;
 	payload_t type;
 	uint8_t* pData;
 	uint8_t pSize;
 	uint8_t* crc;
 } Packet_t;

#endif /* SRC_TYPES_H_ */
