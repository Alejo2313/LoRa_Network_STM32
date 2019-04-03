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

typedef struct{
	uint16_t devAddr;
	uint16_t masterAddr;
	uint16_t gateAddr;
	uint32_t sleepTime;
	uint8_t  power;
	uint8_t  bw;
	uint8_t  sf;
	//ohters.....

	uint8_t RXWIndow;
	uint8_t netAddr;
	
} config_t;


typedef struct 
{
	Data_t 	item;
	uint8_t size;
}item_t;

typedef struct 
{
	uint16_t 	UUID;		//Identificador de quien pertenece la estructura
	uint8_t  	iSize;		// Número de items
	item_t* 	items;		// puntero a items
}data_format;	



typedef enum {
	JOIN,
	START,
	WAIT,
	SLEEP_S,
	MEASURE,
	TX_S,
	RX_S,
	ER_HANDLE,
	CONFIG_S,
	JOIN_S,
	TX_REQ,
	PROCESS
} State_t;

typedef enum{
	REQ_DATA
	//others...
}mac_flags;



 typedef enum {
	TX_T,
	ACK, //REQ INFO IMPLISITO EN FLAGS

	NACK,
	JOIN_T,
	CONFIG

 }Mac_t;



 typedef struct {
	uint8_t NetAddr;
 	uint16_t devAddr;
	uint16_t devDest;
 	Mac_t MacType;
	uint8_t flags;
 	uint8_t* pData;
 	uint8_t pSize;
 	uint8_t* crc;
 } Packet_t;

#endif /* SRC_TYPES_H_ */
