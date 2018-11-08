/*
 * main_2.c
 *
 *  Created on: 16 oct. 2018
 *      Author: Alejo
 */


#include "main.h"
#include "cmsis_os.h"
#include "hw.h"									//Hardware configuration
#include "radio.h"								//phy mac implementation
#include "types.h"
#include "LowPower.h"


/************ PRIVATE VARIABLES ********************************/

Packet_t* RxData;
Packet_t* TxData;

State_t State = SLEEP_S;						// Actual state

int8_t Rssi = 0;								//RX power
int8_t Snr = 0;									//Signal/Noise ratio
int8_t	SfRx = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor
int8_t	SfTx = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor
uint16_t Bw = LORA_BANDWIDTH_DF;			
uint8_t	TxPower = TX_OUTPUT_POWER_DF;



bool exe = false;								//Flag -> used on TX and RX state
static Error_t ErroType = NO_ERROR;				//Needed for ErrorHandleer
Error_t	LastError = NO_ERROR;
config_t  Configuration;						// Main configuration 



/***************** RADIO ********************************/
//Variables
static RadioEvents_t RadioEvents;

void onTxDone();																	//Tx Done callback
void onTxTimeout();																	//Tx Timeout callback prototype
void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );			// Rx Done callback prototype
void onRxTimeout();																	//Rx Timeout callback prototype.
void onRxError();																	//Rx Error callback prototype.
void onFhssChangeChannel(uint8_t currentChannel);									//HSS Change Channel callback prototype;
void onCadDone(bool channelActivityDetected);


/******************** private prototypes **************/
void radio_init();
void loadInfo();
void configure();
void errorHandle();
void measure();
void sendPack();
void print();
void fire();
void sendAck();
void Config_Load();
void parser(uint8_t* pData, uint16_t size);
void Join();



/***********************************/

int main(){

	 
	HAL_Init();

	BSP_LED_Init(LED_BLUE);
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_RED1);

	SystemClock_Config();
	HW_Init();
	radio_init();
	Config_Load();


#ifdef NODE
	osThreadDef(FSM, fire, osPriorityRealtime,0, 512);
	osThreadCreate(osThread(FSM), NULL);
	BSP_LED_On(LED_BLUE);

#else
	Radio.Rx( 0);
	BSP_LED_On(LED_RED1);
#endif

	osKernelStart();
	while(1){
	}

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
	while(1){
		switch(State){

			case JOIN_S:
				Join();
				State = SLEEP_S;
			break;

			case SLEEP_S:
				PrintIfDebug(">Sleep state \r \n");
				GoBed(Configuration.SleepTime);
				if(Configuration.Joined)
					State = MEASURE;
				else
					State = JOIN;
				break;

			case MEASURE:
				PrintIfDebug(">Measure state \r \n");
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

			case CONFIG_S:
				configure();
				State = SLEEP_S;
			break;

			case ERROR_HANDLE:
				errorHandle();
			break;

			default:
				PrintIfDebug("> Unknow erro \r \r");
				State = SLEEP_S;
				break;
		}
	}
}




/**
 * @brief	Join into a network
 * @param	none
 * @retval	none
 * 
 */ 

//TODO  -> send empty pack

void Join(){
	TxData->devAddr 	= Configuration.DevAddr;
	TxData->devDest 	= Configuration.GateAddr;
	TxData->MacType 	= JOIN;
	TxData->NettAddr	= Configuration.NetAddr;

	//TODO -> Complete this

	sendPack();
}


/**
 * 	@brief	Tx done callback
 * 	@param	none
 * 	@retval	none
 */

void onTxDone(){

	PrintIfDebug("-->TX done! \r \t");

#ifdef NODE

	Radio.Sleep( );
	State = RX_S;
	exe = false;

#else
	Radio.Rx(0);
#endif

}

/**
 *	@brief 		RX timeout callback
 * 	@param		none
 * 	@pretval	none
 * 
 */
void onTxTimeout(){


	PrintIfDebug("-->TX error: Timeout \t \n");
	State = ERROR_HANDLE;
	ErroType = TX_TIMEOUT;
	exe = false;

#ifdef NODE
	Radio.Sleep( );
#endif
}



/**
 *	@brief	RX data callback
 *	@param	RX
 */

void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){

#ifdef NODE
	Radio.Sleep( );
#endif

	if(size < HEAD_SIZE)
		return;


	exe = false;
	parser(payload, size);					
	print(RxData);



	//TODO -> YOU MUST BE JOIN

	switch(RxData->MacType){
		case ACK:
			State = SLEEP_S;
			PrintIfDebug("-->ACK ok \n \r");
			break;


		case NACK:
			State = ERROR_HANDLE;
			ErroType = RX_NACK;
			PrintIfDebug("--> ERROR: NACK \r \n");
			break;

		case TX_T:
			switch(RxData->type){
		#ifdef NODE
				case CONFIG:
					State = CONFIG;
					break;	
		#else
				case DATA:
					//print(RxData);
					break;
		#endif
				case REQ_INFO:
					loadInfo();
					State = TX_T;
					break;
			}
			if(USE_ACK){
				sendAck();
			}
			break;

		default:
			State = SLEEP_S;
			break;
	}
}

/**
 * 	@brief	TimeOut Callback 
 * 	@param	none
 * 	@retval	none
 */

void onRxTimeout(){

#ifdef DEBUG
	PRINTF("Error: Timeout \r \n");
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
/**
 * 	@brief	Convert RAW payload to Packet_t type
 * 	@param	[pData]	pointer to recived data
 * 	@para	[size]	pData size (in bytes)
 * 	@retval	none 
 */


void parser(uint8_t* pData, uint16_t size){

	if(RxData == NULL)
		RxData= (Packet_t*)malloc(sizeof(Packet_t));

	RxData->NettAddr	= *(pData++);
	RxData->devAddr 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->devDest 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->MacType 	= (*(pData)>>4)&0xF;
	RxData->flags 		= *(pData++)&0xF;
	RxData->pSize 		= *(pData++);
	RxData->type 		= *(pData++);

	free(RxData->pData);
	RxData->pData = (uint8_t*)malloc(RxData->pSize - 1); 
	memcpy(RxData->pData, pData ,RxData->pSize  - 1);
}

/**
 * @brief 	Send TxData payload
 * @param	none
 * @retval	none
 */
 
void sendPack(){
	static uint8_t* sendData = NULL;

	PRINTF("Sending data: \r\n");
	print(TxData);

	if(sendData !=  NULL)
		free(sendData);

	uint8_t totalSize = TxData->pSize + HEAD_SIZE;
	sendData = (uint8_t*)malloc(totalSize);
	*(sendData++) = (TxData->NettAddr);
	*(sendData++) = (TxData->devAddr>>8)&0xFF;
	*(sendData++) = (TxData->devAddr)&0xFF;
	*(sendData++) = (TxData->devDest >> 8)&0xFF;
	*(sendData++) = (TxData->devDest)&0xFF;
	*(sendData++) = (TxData->MacType << 4)| TxData->flags;
	*(sendData++) = (TxData->pSize);
	*(sendData++) = (TxData->type);
	memcpy(sendData, TxData->pData, TxData->pSize -1);
	
	Radio.Send(sendData - HEAD_SIZE -1, totalSize);

}


/**
 * 	@brief	Read sensor load the data
 * 	@param	none
 * 	@retval	none
 */

void measure(){
	if(TxData == 0){
		TxData = (Packet_t*)malloc(sizeof(Packet_t));
	}
	uint8_t* mess = "01234";
	TxData->NettAddr = 0xFF;
	TxData->devAddr = DEV_ADDR;
	TxData->devDest = 0xFFFF;
	TxData->MacType = 0;
	TxData->flags = 0;
	TxData->pData = mess;
	TxData->type = DATA;
	TxData->pSize = 5;
}


/**
 * 	@brief	DEBUG FUNCTION -> print a package
 * 	@param	[data]	pointer to data struct
 * 	@retval	none
 * 
 */


void print(Packet_t *data){
	if(data == NULL){
		PRINTF("Error printing: DATA NULL \r \n");
		return;
	}
	char str[512];
	sprintf(str, "\t  Net Address: %X \r\n\t device Address: %X \r\n\t Source Address: %X \r\n\t mac type: %X \r\n\t Size: %X \r\n", 
			data->NettAddr, data->devAddr, data->devDest, data->MacType, data->pSize);

	PRINTF(str);
}

/**
 * 	@brief	Rx Error handle
 * 	@param 	none
 * 	@retval	none
 */

void onRxError(){
	exe = false;

#ifdef DEBUG
	PRINTF("RX ERROR \r \n");
#endif
	Radio.Sleep( );
	State = SLEEP_S;
}


/**
 * 
 */




void errorHandle(){
	switch(ErroType){

		case RX_ERROR:
			State = SLEEP_S;
		break;


		case RX_TIMEOUT:
			if(LastError == RX_TIMEOUT){
				State = SLEEP_S;
				return;
			}
				
			if(USE_ACK){
				State = TX_S;
				LastError = RX_TIMEOUT;
				Configuration.SleepTime = (Configuration.SleepTime*(100+SLEEP_FACTOR))/100;
				Configuration.RXWIndow	= (Configuration.RXWIndow*(100 + RX_TIMEOUT_FACTOR))/100;
			}
		break;

		case RX_NACK:				//ToDo   -> REJOIN
			if(LastError == RX_NACK){
				State = TX_S;
				LastError = RX_REJOIN;
				return;
			}
			if(LastError == RX_REJOIN){
				State = JOIN;
				return;
			}
			
			State = TX_S;
			LastError = RX_NACK;

		break;

		case TX_TIMEOUT:
			State = SLEEP_S;
		break;

		default:
			State = SLEEP_S;
			return;
		break;
	}
}



void PrintIfDebug(uint8_t* srt){
	#ifdef DEBUG
	PRINTF(srt);
	#endif;

}



/**
 * 	@brief	Send ACK
 * 
 * 
 */ 


void sendAck(){
	if(TxData == 0){
		TxData = (Packet_t*)malloc(sizeof(Packet_t));
	}
	uint8_t* mess = "01234";
	TxData->NettAddr	= Configuration.NetAddr;
	TxData->devAddr 	= DEV_ADDR;
	TxData->devDest 	= RxData->devAddr;
	TxData->MacType 	= ACK;
	TxData->flags 		= 0;
	TxData->pData 		= mess;
	TxData->type 		= DATA;
	TxData->pSize 		= 5;
	sendPack();
} 


/**
 * @brief Load default values to config structure
 * 
 */

void Config_Load(){

	Configuration.DevAddr 	= DEV_ADDR;
	Configuration.GateAddr	= CONFIG_ADDR;
	Configuration.RXWIndow	= RX_TIMEOUT_VALUE;
	Configuration.SleepTime	= SLEEP_TIME;
	Configuration.Joined	= false;
}


//TODO -> Complete this function
void configure(){


};

void loadInfo(){

};
void onFhssChangeChannel(uint8_t currentChannel){};	//HSS Change Channel callback prototype;
void onCadDone(bool channelActivityDetected){}


