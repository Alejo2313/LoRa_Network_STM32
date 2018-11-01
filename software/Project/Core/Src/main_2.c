/*
 * main_2.c
 *
 *  Created on: 16 oct. 2018
 *      Author: Alejo
 */





#define DEBUG
/*Includes*/
#include "cmsis_os.h"
#include "hw.h"						//Hardware configuration
#include "radio.h"					//phy mac implementation
#include "timeServer.h"				//Timer driver implementation
#include "low_power_manager.h"		//Power manager
#include "types.h"

//#define NODE

#define RF_FREQUENCY		868000000		//Europe band
#define TX_OUTPUT_POWER_DF		14				//dNm

#define RX_TIMEOUT_VALUE                            1000

/*Lora settings */

#define LORA_BANDWIDTH_DF                           0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR_DF                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false




/* MPU settings */

#define RX_WINDOW			100
#define BUFFER_SIZE			64   		//128, 256

#define USE_ACK				false
#define DEV_ADDR			0x1A2B


/* Variables */

Packet_t* RxData;
Packet_t* TxData;

State_t State = SLEEP_S;						// Actual state

int8_t Rssi = 0;								//RX power
int8_t Snr = 0;									//Signal/Noise ratio
int8_t	SfRx = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor
int8_t	SfTx = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor
uint16_t Bw = LORA_BANDWIDTH_DF;
uint8_t	TxPower = TX_OUTPUT_POWER_DF;

bool exe = false;
static int cont = 0;
static Error_t ErroType = NO_ERROR;

/* radio */
static RadioEvents_t RadioEvents;

void onTxDone();			//Tx Done callback
void onTxTimeout();			//Tx Timeout callback prototype
void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );			// Rx Done callback prototype
void onRxTimeout();			//Rx Timeout callback prototype.
void onRxError();			//Rx Error callback prototype.
void onFhssChangeChannel(uint8_t currentChannel);	//HSS Change Channel callback prototype;
void onCadDone(bool channelActivityDetected);


/* private prototypes */

void radio_init();
void loadInfo();
void measure();
void sendPack();
void printRx();
void fire();
void parser(uint8_t* pData, uint16_t size);




int main(){
	// init haardware
	HAL_Init();
	SystemClock_Config();
	HW_Init();
	radio_init();
#ifdef DEBUG
	DBG_Init();
#endif

#ifdef NODE
	while(1){
		fire();
	}
#else
	Radio.Rx( 0);
	while(1){}
#endif
}







void radio_init(){
	RadioEvents.TxDone = onTxDone;
	RadioEvents.TxTimeout = onTxTimeout;
	RadioEvents.RxDone = onRxDone;
	RadioEvents.RxTimeout = onRxTimeout;
	RadioEvents.RxError = onRxError;
	RadioEvents.FhssChangeChannel = onFhssChangeChannel;
	RadioEvents.CadDone = onCadDone;

	Radio.Init( &RadioEvents );
	Radio.SetChannel( RF_FREQUENCY );

	Radio.SetTxConfig( MODEM_LORA, TxPower, 0, Bw, SfRx, LORA_CODINGRATE,
	                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

	Radio.SetRxConfig( MODEM_LORA, Bw, SfTx,LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
	                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
	                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

}

/*
 *
 */

void fire(){
	switch(State){
		case SLEEP_S:
		#ifdef DEBUG
			PRINTF("--> SLEEP STATE \r \n");
		#endif
			//HAL_Delay(1000);
			State = MEASURE;
			break;
		case MEASURE:
		#ifdef DEBUG
			PRINTF("--> MEASURE STATE \r \n");
		#endif
			measure();
			State = TX_S;
			break;
		case TX_S:
			if(!exe){
				sendPack();
				exe = true;
			}
			break;
		case RX_S:
			if(!exe){
				Radio.Rx(1000);
				exe = true;
			}
			break;
		default:
		#ifdef DEBUG
			PRINTF("erro?! \r \n");
		#endif
			State = SLEEP_S;
			break;
	}
}


/*
 *
 */

void onTxDone(){
#ifdef DEBUG
	PRINTF("Transmition done! \r \n");
#endif
#ifdef NODE
	Radio.Sleep( );
#endif

	State = RX_S;
	exe = false;

}
/*
 *
 */
void onTxTimeout(){
#ifdef DEBUG
	PRINTF("Transmition fail! -> timeout \r \n");
#endif
#ifdef NODE
	Radio.Sleep( );
#endif

	State = ERROR_HANDLE;
	ErroType = TX_TIMEOUT;
	exe = false;
}
/*
 *
 */

void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){

#ifdef NODE
	Radio.Sleep( );
#endif

	if(size < 4)
		return;

	exe = false;
	parser(payload, size);
	printRx();
	switch(RxData->MacType){
		case ACK:
			State = SLEEP_S;
		#ifdef DEBUG
			PRINTF("RX ok! -> ACK \r \n");
		#endif
			break;


		case NACK:
			State = ERROR_HANDLE;
			ErroType = RX_NACK;
		#ifdef DEBUG
			PRINTF("RX ERROR! -> NACK \r \n");
		#endif
			break;

/*
		case ACK_CONFIG:
			State = CONFIG_S;
		#ifdef DEBUG
			PRINTF("RX ok! -> ACK+CONFIG \r \n");
		#endif
			break;

		case NACK_CONFIG:
			State = CONFIG_S;
		#ifdef DEBUG
			PRINTF("RX ERROR! -> NACK+CONFIG \r \n");
		#endif
			break;

		case REQ_NFO:
			loadInfo();
			State = TX_S;
		#ifdef DEBUG
			PRINTF("RX OK! -> REQ_NFO \r \n");
		#endif
			break;

*/
		case TX_T:
		#ifdef DEBUG
			switch(RxData->type){
				case DATA:
					printRx();
					break;
				case CONFIG:
					//set config
					break;
				case REQ_INFO:
					//load info
					State = TX_T;
					break;
			}
		//	sendAck();
		#endif
			break;

		default:
			State = SLEEP_S;
			break;
	}
}



void onRxTimeout(){

#ifdef DEBUG
	PRINTF("RX TIMEOUT \r \n");
#endif
#ifdef NODE
	Radio.Sleep( );
#endif
	exe = false;
	if(USE_ACK){
		State = ERROR_HANDLE;
		ErroType = RX_TIMEOUT;
	}
	else{
		State = SLEEP_S;
	}
}

void parser(uint8_t* pData, uint16_t size){

	RxData= (Packet_t*)malloc(sizeof(Packet_t));
	RxData->devAddr = (*pData << 8)|(*(pData+1)&0xFF);
	RxData->MacType = *(pData+2);
	RxData->pSize = *(pData+3);
	RxData->pData = (uint8_t*)malloc(RxData->pSize);
	memcpy(RxData->pData, pData + 4,RxData->pSize );

}


void measure(){
	if(TxData == 0){
		TxData = (Packet_t*)malloc(sizeof(Packet_t));
	}
	uint8_t* mess = "01234";
	TxData->devAddr = DEV_ADDR;
	TxData->MacType = TX_T;
	TxData->pData = mess;
	TxData->pSize = 5;
}


void sendPack(){

	uint8_t totalSize = TxData->pSize + 4;
	uint8_t* sendData = (uint8_t*)malloc(totalSize);
	sendData[0] = (TxData->devAddr &0xFF00)>>8;
	sendData[1] = (TxData->devAddr & 0xFF);
	sendData[2] = TxData->MacType;
	sendData[3] = TxData->pSize;
	memcpy(sendData + 4, TxData->pData, TxData->pSize);
#ifdef DEBUG
	char str[256];
	sprintf(str, "DevAddr= 0x%X%X \n\r MacType = %u \n\r pZise = %u \n \r pData =0x%X 0x%X 0x%X 0x%X 0x%X ",sendData[0],sendData[1],sendData[2],sendData[3],sendData[4], sendData[5],sendData[6],sendData[7],sendData[8] );
	PRINTF(str);
#endif
	Radio.Send(sendData, totalSize);

}
void printRx(){

	char str[256];
	if(RxData == 0){
		PRINTF("ERROR DATA NULL");
		return;
	}
	PRINTF("\t pData -> \n\r");
	sprintf(str, "CONT = %d \n DevAddr= %X \n\r MacType = %u \n\r pZise = %u \n \r pData = 0x%X 0x%X 0x%X 0x%X 0x%X ",cont,RxData->devAddr,RxData->MacType,RxData->pSize,RxData->pData[0],RxData->pData[1],RxData->pData[2],RxData->pData[3],RxData->pData[4] );
	PRINTF(str);
	PRINTF("\r \n");
	cont++;
}


void loadInfo(){

};
void onRxError(){
	exe = false;

#ifdef DEBUG
	PRINTF("RX ERROR \r \n");
#endif
	Radio.Sleep( );
	State = SLEEP_S;
}

void onFhssChangeChannel(uint8_t currentChannel){};	//HSS Change Channel callback prototype;
void onCadDone(bool channelActivityDetected){}
