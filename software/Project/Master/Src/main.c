#include "main.h"
#include "stm32l0xx_hal.h"

/*varibles*/

//buffers

Packet_t* RxData;
Packet_t* TxData;

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

} flags;


//                          ORIGIN      BITCHECK    NEXT        OUTFUNC
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
							{TX_REQ,    TX_TIMEOUT, SLEEP_S,    goSleep},
                            {ER_HANDLE, ERROR_OK,   SLEEP_S,    goSleep},
                            {CONFIG,    CONFIG_OK,  SLEEP_S,    goSleep},
                            {CONFIG,    CONF_ERROR, ER_HANDLE,  errorHandle},

                            {-1, NULL, -1, NULL}
                            };



//--------------> begin main function <--------------//

int main(void){

	HAL_Init();
	wakeUpSystem();

	xTaskCreate(main_task, "PWM", 256, NULL, osPriorityNormal, NULL);
	xTaskCreate(led_blink, "LED", 128, NULL, osPriorityNormal, NULL);
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
	Radio.Sleep( );


#ifdef debug
	Trace_send("-->TX done! \r \n");
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
    Trace_send("-->TX error: Timeout \t \n");
#endif
}

/**
 * 	@brief	TimeOut Callback 
 * 	@param	none
 * 	@retval	none
 */

void onRxTimeout(){
	bitSet(&state_flags, RX_TIMEOUT);
	Radio.Sleep( );

#ifdef debug
    Trace_send("Error: Timeout \r \n");
#endif

}

/**
 * 	@brief	Rx Error handle
 * 	@param 	none
 * 	@retval	none
 */

void onRxError(){
	bitSet(&state_flags, RX_ERROR);
    state_flags |= RX_ERROR;
    Radio.Sleep( );

#ifdef debug
	Trace_send("RX ERROR \r \n");	
#endif
}

void onFhssChangeChannel(uint8_t currentChannel){};
void onCadDone(bool channelActivityDetected){};



void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){

	Radio.Sleep( );

	if(size < HEAD_SIZE)
		return;

	parser(payload, size);

#ifdef debug					
	print(RxData);
#endif


	//TODO -> YOU MUST BE JOIN

	switch(RxData->MacType){
		case ACK:
			bitSet(&state_flags, RX_OK);
			break;

		case NACK:
			bitSet(&state_flags, RX_NACK);
			break;

		case TX_T:
			switch(RxData->type){
				case CONFIG:
					bitSet(&state_flags, RX_CONFIG);
					break;	

				case REQ_INFO:
					bitSet(&state_flags, RX_REQ);
					loadInfo();
					break;
			}

		default:
			bitSet(&state_flags, RX_OK);
			break;
	}

#ifdef debug
    if( state_flags & RX_OK)
        Trace_send("-->ACK OK \n");
    if( state_flags & RX_NACK)
        Trace_send("--> ERROR: NACK \n");
    if( state_flags & RX_CONFIG)
        Trace_send("--> CONFIG MODE\n");
    if( state_flags & RX_REQ)
        Trace_send("--> DATA REQUEST \n");
#endif

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

void parser(uint8_t* pData, uint16_t size){

	if(RxData == NULL){
		RxData= (Packet_t*)malloc(sizeof(Packet_t));
		RxData->pData	= NULL;
	}
	RxData->NettAddr	= *(pData++);
	RxData->devAddr 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->devDest 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->MacType 	= (*(pData)>>4)&0xF;
	RxData->flags 		= *(pData++)&0xF;
	RxData->pSize 		= *(pData++);
	RxData->type 		= *(pData++);

	if(RxData->pData != NULL)
		free(RxData->pData);

	RxData->pData = (uint8_t*)malloc(RxData->pSize - 1); 
	memcpy(RxData->pData, pData ,RxData->pSize  - 1);
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
		Trace_send("Error printing: DATA NULL \r \n");
		return;
	}

	uint8_t cnt = 0;

	Trace_send("\t  Net Address: %X\n \t device Address: %X \n \t Source Address: %X \n \t mac type: %X  \n \t Size: %X  \n \t RAW data  ->", 
			data->NettAddr, data->devAddr, data->devDest, data->MacType, data->pSize);

	while(cnt < data->pSize){
		Trace_send("%X ", *(data->pData + cnt++));
	}
	Trace_send("\n");

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
	Configuration.NetAddr	= 0;
	bitClear(&state_flags, JOIN_OK);
}

/**
 * @brief 
 * 
 */
void loadInfo(){

};

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
	*flags |= bit;
}

void clearFlags(uint32_t* flags){
	*flags = 0;
}

//--------------> end private functions <--------------//

//--------------> begin fsm functions <--------------//
/**
 * 
 * @brief	join protocolo
 * @param	[fsm] pointer to state machine structure
 * @retval	none
 * 
 * */
void join(fsm_t* fsm){
	bitClear(fsm->flags, JOIN_OK) ;
	bitClear(fsm->flags, JOIN_ERROR);
	//TODO -> create this function!
}

/**
 * @brief  Sleep the system
 * 
 * @param fsm pointer to state machine structure
 */
void goSleep(fsm_t* fsm){
	//Clear flags
	bitClear(fsm->flags, TX_OK);
	bitClear(fsm->flags, RX_OK);
	bitClear(fsm->flags, ERROR_OK);
	bitClear(fsm->flags, CONFIG_OK);

#ifdef debug
	Trace_send("-> Sleep state \n");
	HAL_Delay(Configuration.SleepTime);
#else
	GoBed(Configuration.SleepTime);
	wakeUpSystem();		//TODO -> MAKE THIS FUNCTION!
#endif
}


/**
 * @brief capture sensor data
 * 
 * @param fsm pointer to state machine structure
 */
void measure(fsm_t* fsm){
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
 * @brief 
 * 
 * @param fsm pointer to state machine structure
 */
void send(fsm_t* fsm){
	static uint8_t totalSize;
	static uint8_t* sendData;

	//Clear flags
	bitClear(fsm->flags, MEASURE_OK);
	bitClear(fsm->flags, RX_OK);

	//Alloc memory
	totalSize = TxData->pSize + HEAD_SIZE;
	sendData  = (uint8_t*)malloc(totalSize);

	//Load data
	*(sendData++) = (TxData->NettAddr);
	*(sendData++) = (TxData->devAddr>>8)&0xFF;
	*(sendData++) = (TxData->devAddr)&0xFF;
	*(sendData++) = (TxData->devDest >> 8)&0xFF;
	*(sendData++) = (TxData->devDest)&0xFF;
	*(sendData++) = (TxData->MacType << 4)| TxData->flags;
	*(sendData++) = (TxData->pSize);
	*(sendData++) = (TxData->type);
	memcpy(sendData, TxData->pData, TxData->pSize -1);
	//send data
	Radio.Send(sendData - HEAD_SIZE -1, totalSize);
	free(sendData - HEAD_SIZE -1);

#ifdef debug
	Trace_send("Sending data: \r\n");
	print(TxData);
#endif

}
/**
 * @brief 
 * 
 * @param fsm pointer to state machine structure
 */
void receive(fsm_t* fsm){
	bitClear(fsm->flags, TX_OK);
	Radio.Rx(Configuration.RXWIndow);
}
/**
 * @brief 
 * 
 * @param fsm pointer to state machine structure
 */
//TODO -> complete this function
void errorHandle(fsm_t* fsm){

	if(*(fsm->flags)&TX_TIMEOUT){

		bitClear(fsm->flags, TX_TIMEOUT);
	}
	if(*(fsm->flags)&RX_TIMEOUT){

		bitClear(fsm->flags, RX_TIMEOUT);
	}
	if(*(fsm->flags)&RX_ERROR){

		bitClear(fsm->flags, RX_ERROR);
	}
	if(*(fsm->flags)&RX_NACK){

		bitClear(fsm->flags, RX_NACK);
	}
	if(*(fsm->flags)&CONF_ERROR){
		Config_Load();
		bitSet(fsm->flags, JOIN_ERROR);
		bitClear(fsm->flags, CONF_ERROR);
	}

	bitSet(fsm->flags, ERROR_OK);

}
/**
 * @brief 
 * 
 * @param fsm pointer to state machine structure
 */
void config(fsm_t* fsm){
	//TODO -> complete this function
	bitClear(fsm->flags, RX_CONFIG);
	
}
/**
 * @brief 
 * 
 * @param fsm 
 */
void restart(fsm_t* fsm){
	clearFlags(fsm->flags);
	bitSet(fsm->flags, STARTED);
}
//--------------> end fsm functions <--------------//

//--------------> begin System task functions <--------------//

void main_task(void* param){

	fsm_t* fsm_lora = fsm_new(trans_table, &state_flags);

	while(1){
		fsm_fire(fsm_lora);
	}
	
	//TODO -> complete this function
}

void led_blink(void* param){
	while(1){
		BSP_LED_Toggle(LED1);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
	
}
//--------------> begin System task functions <--------------//
