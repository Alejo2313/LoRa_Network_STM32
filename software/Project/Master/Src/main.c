/**                             _____________
 *              /\      /\     /             \
 *             //\\____//\\   |   MAUUUU!!    |
 *            /     '      \   \  ___________/
 *           /   /\ '  /\    \ /_/			      / /  ___    / __\ |__   __ _| |_ 
 *          |    == o ==     |        /|	     / /  / _ \  / /  | '_ \ / _` | __|
 *           \      '        /       | |	    / /__|  __/ / /___| | | | (_| | |_ 
 *             \           /         \ \	    \____/\___| \____/|_| |_|\__,_|\__|
 *             /****<o>**** \         / /
 *             |            ' \       \ \
 *             |    |    | '   '\      \ \
 *  _________  | ´´ |  ' |     '  \    / /
 *  |  MAYA  | |  ' |    | '       |__/ /
 *   \______/   \__/ \__/ \_______/____/
 * 
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#include "main.h"
#include "stm32l0xx_hal.h"
#include "queue_c.h"

/*varibles*/

//buffers
queue_c RxRAW;
static Packet_t* RxData = NULL;
static Packet_t* TxData = NULL;

//Radio
static RadioEvents_t RadioEvents;

static int8_t   Rssi    = 0;							//RX power
static int8_t   Snr     = 0;							//Signal/Noise ratio
static int8_t	SfRx    = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor000
static int8_t	SfTx    = LORA_SPREADING_FACTOR_DF;		//Actual Spreading factor
static uint16_t Bw      = LORA_BANDWIDTH_DF;	        //Actual bandwidth		
static uint8_t	TxPower = TX_OUTPUT_POWER_DF;           //Actual out power

//Private

static config_t  Configuration;						    // Main configuration 

//FSM
volatile uint32_t state_flags = 1;

enum {
    STARTED,
    JOIN_OK,
    MEASURE_OK,
    TX_OK,      
    RX_OK,
    RX_CONFIG,
    ERROR_OK,
    CONFIG_OK,
    JOIN_ERROR,
    TX_TIMEOUT,
    RX_TIMEOUT,
    RX_ERROR,
    RX_NACK,
    RX_REQ,
    CONF_ERROR,
	RX_DATA,
	PR_ERROR,
	PR_OK

} flags;


void second_task(void* param);

void listen(fsm_t* fsm);
void errorHandle(fsm_t* fsm);
void send(fsm_t* fsm);

void nop(fsm_t* fsm);
void process(fsm_t* fsm);
void prErrorHandle(fsm_t* fsm);



//                          ORIGIN      BITCHECK    NEXT        OUTFUNC
fsm_trans_t table_radio[] = {
                            {START,     STARTED,	RX_S,       listen },
							{RX_S,     	RX_ERROR,   ER_HANDLE,  errorHandle}, 
							{RX_S,     	TX_REQ,   	TX_S,   	send},
							{TX_S,     	TX_OK,   	RX_S,   	listen},
							{TX_S,     	TX_TIMEOUT, ER_HANDLE,  errorHandle},
							{ER_HANDLE, ERROR_OK, 	RX_S,  		listen},
      
                            {-1, NULL, -1, NULL}
                            };


fsm_trans_t table_process[] = 	{
							  	{START, 	STARTED, 	WAIT, 		nop},
								{WAIT, 		RX_DATA, 	PROCESS, 	process},
								{PROCESS, 	PR_ERROR, 	WAIT, 		prErrorHandle},
								{PROCESS, 	PR_OK, 		WAIT, 		nop},

								{-1, NULL, -1, NULL}	
								};


//--------------> begin main function <--------------//

int main(void){

	HAL_Init();
	wakeUpSystem();


	create_queue(&RxRAW, 10);

	xTaskCreate(main_task, "PWM", 128, NULL, osPriorityNormal, NULL);
	xTaskCreate(second_task, "SECOND", 256, NULL, osPriorityNormal, NULL);
	xTaskCreate(led_blink, "LED", 64, NULL, osPriorityNormal, NULL);
	vTaskStartScheduler();
	while(1){
	}
	
    return 0;

}

//--------------> end main function <--------------//

//--------------> begin radio functions <--------------//

/**
 *  @biref  radio driver initi
 *  @param  none
 *  @return none
 * 
 * */
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

/**
 * 	@brief	Tx done callback
 * 	@param	none
 * 	@retval	none
 */

void onTxDone(){
	bitSet(&state_flags, TX_OK);

#ifdef debug
	Trace_send("-->TX done! \r \n\r");
#endif

}

/**
 *	@brief 		RX timeout callback
 * 	@param		none
 * 	@pretval	none
 * 
 */
void onTxTimeout(){
	bitSet(&state_flags, TX_TIMEOUT);
	Radio.Sleep( );

#ifdef debug
    Trace_send("-->TX error: Timeout \n\r");
#endif
}

/**
 * 	@brief	TimeOut Callback 
 * 	@param	none
 * 	@retval	none
 */

void onRxTimeout(){
	bitSet(&state_flags, RX_TIMEOUT);
#ifdef debug
    Trace_send("Error: Timeout \n\r");
#endif

}

/**
 * 	@brief	Rx Error handle
 * 	@param 	none
 * 	@retval	none
 */

void onRxError(){
	bitSet(&state_flags, RX_ERROR);
#ifdef debug
	Trace_send("RX ERROR \r \n\r");	
#endif
}

void onFhssChangeChannel(uint8_t currentChannel){};
void onCadDone(bool channelActivityDetected){};



void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){

	if(size < HEAD_SIZE)
		return;
	

	bitSet(&state_flags, RX_DATA);
	push(&RxRAW, parser(payload, size));

	return;
}

//--------------> end radio functions <--------------//

//--------------> begin private functions <--------------//

void wakeUpSystem(){
	SystemClock_Config();
	HW_Init();
	radio_init();
	Config_Load();
	Trace_Init();
}

/**
 * 	@brief	Convert RAW payload to Packet_t type
 * 	@param	[pData]	pointer to recived data
 * 	@para	[size]	pData size (in bytes)
 * 	@retval	none 
 */

Packet_t* parser(uint8_t* pData, uint16_t size){

	Packet_t* inPack = (Packet_t*)pvPortMalloc(sizeof(Packet_t));
	RxData->pData	= NULL;
	
	inPack->NetAddr		= *(pData++);
	inPack->devAddr 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	inPack->devDest 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	inPack->MacType 	= (*(pData)>>4)&0xF;
	inPack->flags 		= *(pData++)&0xF;
	inPack->pSize 		= *(pData++);

	inPack->pData = (uint8_t*)pvPortMalloc(inPack->pSize*sizeof(uint8_t)); 
	memcpy(inPack->pData, pData ,inPack->pSize);

	return inPack;
}

/**
 * 	@brief	DEBUG FUNCTION -> print a package
 * 	@param	[data]	pointer to data struct
 * 	@retval	none
 * 
 */
//TODO -> REPAIR THIS FUNCTION ->OK
void print(Packet_t *data){
	if(data == NULL){
		Trace_send("Error printing: DATA NULL \r \n\r");
		return;
	}

	uint8_t cnt = 0;

	Trace_send("\t  Net Address: %X\n\r \t device Address: %X \n\r \t Source Address: %X \n\r \t mac type: %X  \n\r \t Size: %X  \n\r \t RAW data  ->", 
			data->NetAddr, data->devAddr, data->devDest, data->MacType, data->pSize);

	while(cnt < data->pSize){
		Trace_send("%X ", *(data->pData + cnt++));
	}
	Trace_send("\n\r");

}

/**
 * @brief Load default values to config structure
 * 
 */

void Config_Load(){
	Configuration.devAddr 	= DEV_ADDR;
	Configuration.gateAddr	= CONFIG_ADDR;
	Configuration.RXWIndow	= RX_TIMEOUT_VALUE;
	Configuration.sleepTime	= SLEEP_TIME;
	Configuration.netAddr	= 0;
	bitClear(&state_flags, JOIN_OK);
}

/**
 * @brief 
 * 
 */
void loadInfo(){

};

void processJoin(Packet_t* inData){

	static uint8_t cnt = 0;
	
	if(TxData == NULL){

#ifdef debug
		Trace_send("TxData is NULL!!!!!!");
#endif
		TxData = (Packet_t*)pvPortMalloc(sizeof(Packet_t));
		TxData->pData = (uint8_t*)pvPortMalloc(64);
	}


	TxData->NetAddr = NET_ADDR;
	TxData->devAddr = DEV_ADDR;
	TxData->devDest = 0x1234; 		//ToDo have to be random 
	TxData->MacType = CONFIG;
	TxData->flags = 0;

	cnt = 0;
	*(TxData->pData + cnt++) = *(inData->pData);
	*(TxData->pData + cnt++) = *(inData->pData + 1);

	*(TxData->pData + cnt++) =  (SLEEP_TIME >> 8) & 0xFF;
	*(TxData->pData + cnt++) = 	(SLEEP_TIME)& 0xFF;

	*(TxData->pData + cnt++) =  TX_OUTPUT_POWER_DF;

	*(TxData->pData + cnt++) = ((LORA_BANDWIDTH_DF + 1) << 4) | ((LORA_SPREADING_FACTOR_DF - 6) & 0xF);
	
	*(TxData->pData + cnt++) = RX_TIMEOUT_VALUE/1000;

	TxData->pSize = cnt;



}

/**
 * @brief Clear a single bit 
 * 
 * @param flag all flags
 * @param bit bit to clear
 */
void bitClear(uint32_t* flag, uint8_t bit){
	*flag &= ~(1 << bit);
}
/**
 * @brief Set a single bit
 * 
 * @param flags group o flags
 * @param bit bit to set
 */
void bitSet(uint32_t* flags, uint8_t bit){
	*flags |= (1 << bit);
}

int bitTest(uint32_t* flags, uint8_t bit){
	return *flags&(1 << bit);
}

void clearFlags(uint32_t* flags){
	*flags = 0;
}

//--------------> end private functions <--------------//

//--------------> begin fsm functions <--------------//

void listen(fsm_t* fsm){
	bitClear(fsm->flags, TX_OK);
	bitClear(fsm->flags, ERROR_OK);

	Radio.Rx(0);
}

void process(fsm_t* fsm){
	static Packet_t* raw;

	raw = (Packet_t*)pop(&RxRAW);


#ifdef debug					
	print(raw);
#endif
	//TODO -> complete this
	switch(raw->MacType){
		case ACK:
			bitSet(&state_flags, RX_OK);
			break;

		case NACK:
			bitSet(&state_flags, RX_NACK);
			break;

		case TX_T:
			bitSet(fsm->flags, PR_OK);
			break;
		case JOIN_T:
			bitSet(fsm->flags, PR_OK);
			processJoin(raw);
			bitSet(fsm->flags, TX_REQ);

			break;

		default:
			bitSet(&state_flags, PR_OK);
			break;
	}


	if(empty(&RxRAW))
		bitClear(fsm->flags, RX_DATA);

	vPortFree(raw->pData);
	vPortFree(raw);


#ifdef debug
    if( bitTest(fsm->flags, RX_OK)) //TODO -> puede no recibir ACK
        Trace_send("-->ACK OK \n\r");
    if( bitTest(fsm->flags, RX_NACK))
        Trace_send("--> ERROR: NACK \n\r");
    if( bitTest(fsm->flags, RX_CONFIG))
        Trace_send("--> CONFIG MODE\n\r");
    if( bitTest(fsm->flags, RX_REQ))
        Trace_send("--> DATA REQUEST \n\r");
	if( bitTest(fsm->flags, PR_OK))
        Trace_send("--> DATA PROCESSED \n\r");
#endif
}

void send(fsm_t* fsm){
	static uint8_t totalSize;
	static uint8_t* sendData;

	//Clear flags
	bitClear(fsm->flags, TX_REQ);
	bitClear(fsm->flags, RX_OK);

	
	//Alloc memory
	totalSize = TxData->pSize + HEAD_SIZE;

	if(sendData == NULL)
		sendData  = (uint8_t*)malloc(64);

	//Load data
	*(sendData++) = (TxData->NetAddr);
	*(sendData++) = (TxData->devAddr>>8)&0xFF;
	*(sendData++) = (TxData->devAddr)&0xFF;
	*(sendData++) = (TxData->devDest >> 8)&0xFF;
	*(sendData++) = (TxData->devDest)&0xFF;
	*(sendData++) = (TxData->MacType << 4)| TxData->flags;
	*(sendData++) = (TxData->pSize);

	memcpy(sendData, TxData->pData, TxData->pSize);
	//send data
	Radio.Send(sendData - HEAD_SIZE , totalSize);
	return;

#ifdef debug
	Trace_send("Sending data: \n\r");
	print(TxData);
#endif

}

void prErrorHandle(fsm_t* fsm){
	//TODO -> complete this

	bitClear(fsm->flags, PR_ERROR);
}

void errorHandle(fsm_t* fsm){
	if(bitTest(fsm->flags, RX_ERROR)){
		bitClear(fsm->flags, RX_ERROR);
	}
	if(bitTest(fsm->flags, TX_TIMEOUT)){
		bitClear(fsm->flags, TX_TIMEOUT);
	}

	bitSet(fsm->flags, ERROR_OK);
}

void nop(fsm_t* fsm){
	//TODO -> test this

	bitClear(fsm->flags,PR_OK);
}
//--------------> end fsm functions <--------------//

//--------------> begin System task functions <--------------//

void main_task(void* param){

	fsm_t* fsm_lora = fsm_new(table_radio, &state_flags);

	while(1){
		fsm_fire(fsm_lora);
	}
	
	//TODO -> complete this function
}
void second_task(void* param){
	fsm_t* fsm_second = fsm_new(table_process, &state_flags);

	while(1){
		fsm_fire(fsm_second);
	}
	
}

void led_blink(void* param){
	while(1){
		BSP_LED_Toggle(LED1);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
	
}
//--------------> begin System task functions <--------------//
