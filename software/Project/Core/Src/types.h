/*
 * types.h
 *
 *  Created on: 21 oct. 2018
 *      Author: Alejo
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

/* Types */

typedef enum {
	SLEEP_S,
	MEASURE,
	TX_S,
	RX_S,
	ERROR_HANDLE,
	CONFIG_S
} State_t;

typedef enum {
	RX_ERROR,
	RX_TIMEOUT,
	RX_NACK,
	TX_TIMEOUT,
	NO_ERROR

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
	uint16_t NettAddr;
 	uint16_t devAddr;
 	Mac_t MacType;
 	payload_t type;
 	uint8_t* pData;
 	uint8_t pSize;
 	uint8_t* crc;
 } Packet_t;

#endif /* SRC_TYPES_H_ */
