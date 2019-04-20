#include "main.h"
#include "stm32l0xx_hal.h"

#include "extADC.h"

#include "hw_gpio.h"
/*varibles*/

static item_t items[] ={
						{LIGTH			,0},
						{TEMPERATURE	,1},
						{HUMIDITY		,1},
						{RSSI			,0},
						{RSSI			,0},
						{RSSI			,0},
						{RSSI			,0},
						{RSSI			,0},
						{RSSI			,0},
						{RSSI			,0},

						{255, 255}
					   };


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
							{JOIN, 		TX_OK,		JOIN,		receive},
							{JOIN, 		RX_TIMEOUT,	START,		retryoin},
							{JOIN, 		RX_ERROR,	START,		retryoin},
							{JOIN, 		RX_CONFIG,	JOIN,		config},
                            {JOIN,      JOIN_OK,    SLEEP_S,    goSleep},
							{JOIN,      CONF_ERROR, START,    	retryoin},
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

	UUID = (STM32_UUID[0]^STM32_UUID[1]^STM32_UUID[2])&0xFFFF;
	HAL_Init();

	ExtADC_Init();
	ExtADC_Reset();

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
	Radio.Sleep( );

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
    state_flags |= RX_ERROR;
    Radio.Sleep( );

#ifdef debug
	Trace_send("RX ERROR \r \n\r");	
#endif
}

void onFhssChangeChannel(uint8_t currentChannel){};
void onCadDone(bool channelActivityDetected){};
/**
 * @brief 
 * 
 * @param payload 
 * @param size 
 * @param rssi 
 * @param snr 
 */


void onRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){

	Radio.Sleep( );

	if(size < HEAD_SIZE)
		return;

	parser(payload, size);

#ifdef debug					
	print(RxData);
#endif

	if(RxData->devDest == Configuration.devAddr && RxData->NetAddr == Configuration.netAddr)

		switch(RxData->MacType){
			case ACK:
				if(RxData->flags & (1 << REQ_DATA)){
					bitSet(&state_flags, RX_REQ);		
					loadInfo();
					break;
				}
				bitSet(&state_flags, RX_OK);
				break;

			case NACK:
				bitSet(&state_flags, RX_NACK);
				break;

			case CONFIG:
				bitSet(&state_flags, RX_CONFIG);
				break;
							
			default:
				bitSet(&state_flags, RX_OK);
				break;
		}
	else{
		if(RxData->MacType == CONFIG){
			if(((*(RxData->pData) << 8) | (*(RxData->pData+1)&0xFF)) == UUID){
				bitSet(&state_flags, RX_CONFIG);			
			}
		}
		else
		{
			bitSet(&state_flags, RX_ERROR);
			return;
		}
		
	}
#ifdef debug
  	if(bitTest(&state_flags, RX_OK))
        Trace_send("-->ACK OK \n\r");

    if( bitTest(&state_flags,  RX_NACK))
        Trace_send("--> ERROR: NACK \n\r");
    if(bitTest(&state_flags,  RX_CONFIG))
        Trace_send("--> CONFIG MODE\n\r");
    if(bitTest(&state_flags,  RX_REQ))
        Trace_send("--> DATA REQUEST \n\r");
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
		RxData= (Packet_t*)pvPortMalloc(sizeof(Packet_t));
		RxData->pData	= (uint8_t*)pvPortMalloc(MAX_PAYLOAD);
	}
	RxData->NetAddr	= *(pData++);
	RxData->devAddr 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->devDest 	= (*(pData++)<< 8) | (*(pData++)&0xFF);
	RxData->MacType 	= (*(pData)>>4)&0xF;
	RxData->flags 		= *(pData++)&0xF;
	RxData->pSize 		= *(pData++);


	memcpy(RxData->pData, pData ,RxData->pSize);
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
	//Todo -> complete this
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
/**
 * 
 * @brief	join protocol
 * @param	[fsm] pointer to state machine structure
 * @retval	none
 * 
 * */
void join(fsm_t* fsm){
	if(TxData == NULL){
		TxData = (Packet_t*)pvPortMalloc(sizeof(Packet_t));
		TxData->pData = (uint8_t*)pvPortMalloc(MAX_PAYLOAD);
	}

	item_t* it;
	uint8_t cnt;

	bitClear(fsm->flags, JOIN_OK) ;
	bitClear(fsm->flags, JOIN_ERROR);

	TxData->NetAddr = Configuration.netAddr;
	TxData->devAddr = Configuration.devAddr;
	TxData->devDest = Configuration.masterAddr;
	TxData->MacType = JOIN_T;
	TxData->flags 	= 0;
	TxData->pSize 	= 2;

	//Paylaod size calculation

	*(TxData->pData) = (UUID >> 8)&0xFF;
	*(TxData->pData+1) = (UUID)&0xFF;



	for(it = items; it->size != 255; it++){
		TxData->pSize++;
	}

	size_t size = xPortGetFreeHeapSize() ;

	cnt = 2;
	for(it = items; it->size != 255; it++){
		*(TxData->pData+ cnt++) = ((it->item&0xF) << 2) | (it->size & 0x3);
	}

	send(fsm);
	//TODO -> check this

#ifdef debug
	Trace_send("-> Join state \n\r");
#endif
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
	Trace_send("-> Sleep state \n\r");
	vTaskDelay(Configuration.sleepTime/portTICK_RATE_MS);
#else
	GoBed(Configuration.sleepTime);
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
		TxData = (Packet_t*)pvPortMalloc(sizeof(Packet_t));
		TxData->pData = (uint8_t*)pvPortMalloc(MAX_PAYLOAD);
	}

//TODO -> DELETE THIIIIIS!!!!!

	static uint8_t cnt = 0;
	
	TxData->NetAddr =  Configuration.netAddr;
	TxData->devAddr = Configuration.devAddr;
	TxData->devDest = Configuration.gateAddr;
	TxData->MacType = TX_T;
	TxData->flags = 0;

	TxData->pSize = 12; 

	for(cnt = 0; cnt < 12 ; cnt++){
		TxData->pData[cnt] = cnt;
	}



	bitSet(fsm->flags, MEASURE_OK);

#ifdef debug
	Trace_send("->Measure state \n\r");
#endif
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
	sendData  = (uint8_t*)pvPortMalloc(totalSize);

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
	vPortFree(sendData - HEAD_SIZE);

#ifdef debug
	Trace_send("Sending data: \n\r");
	print(TxData);
#endif

}
/**
 * @brief 
 * 
 * @param fsm pointer to state machine structure
 */
void receive(fsm_t* fsm){
	//Todo -> set offset 
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

	if(bitTest(fsm->flags, TX_TIMEOUT)){

		bitClear(fsm->flags, TX_TIMEOUT);
	}
	if(bitTest(fsm->flags, RX_TIMEOUT)){

		bitClear(fsm->flags, RX_TIMEOUT);
	}
	if(bitTest(fsm->flags, RX_ERROR)){

		bitClear(fsm->flags, RX_ERROR);
	}
	if(bitTest(fsm->flags, RX_NACK)){

		bitClear(fsm->flags, RX_NACK);
	}
	if(bitTest(fsm->flags, CONF_ERROR)){
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
	uint16_t aux = 0;
	

#ifdef debug
	Trace_send("Configuring.... \n");
#endif
	aux = (*(RxData->pData++) << 8) | (*(RxData->pData++)&0xFF);

	if(aux != UUID){
		bitSet(fsm->flags, CONF_ERROR);
		return;
	}

	aux = (*(RxData->pData++) << 8) | (*(RxData->pData++)&0xFF);
	if(aux != 0)
		Configuration.sleepTime = aux;
	
	aux =(*(RxData->pData++)&0xFF);
	if(aux != 0 && aux <= MAX_POWER)
		Configuration.power = aux;

	aux =(*(RxData->pData)>>4)&0XF;
	if(aux != 0 && aux <= 3){
		Configuration.bw = aux - 1;
	}
	aux =(*(RxData->pData++))&0XF;
	if(aux != 0 && aux <=6){
		Configuration.sf = aux + 6;
	}
	aux =(*(RxData->pData++)&0xFF);
	if(aux != 0){
		Configuration.RXWIndow = aux * 1000 ;
	}

	Configuration.devAddr = RxData->devDest;
	Configuration.gateAddr = RxData->devAddr;
	Configuration.netAddr = RxData->NetAddr;


	bitSet(fsm->flags, CONFIG_OK);
	bitSet(fsm->flags, JOIN_OK);
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

void retryoin(fsm_t* fsm){
	
	bitClear(fsm->flags, RX_TIMEOUT);
	bitClear(fsm->flags, RX_ERROR);
	bitClear(fsm->flags, CONF_ERROR);

#ifdef debug


	//void ExtADC_ConfigChannel(ExtChannel_t channel, ExtGain_t Gain, ExtMode_t mode, ExtChannel_t negativeInput);

	ExtADC_ConfigChannel(CHANNEL_1, EXTADC_GAIN_1, DIFFERENTIAL, CHANNEL_2);

	float val1 = ExtADC_ReadVoltageInput(CHANNEL_7);
	float val2 = ExtADC_ReadVoltageInput(CHANNEL_1);
	float val3 = ExtADC_ReadTempSensor();

	 Trace_send("Value %lf %lf  %lf \n", val1, val2, val3);
	//val2 = ExtADC_ReadVoltageInput(CHANNEL_1);
	//val3 = ExtADC_ReadVoltageInput(CHANNEL_4);
	//uint8_t id = ExtADC_ReadIDReg();

//	vTaskDelay(500/portTICK_RATE_MS);


#else
	GoBed(JOIN_RETRY);
	wakeUpSystem();
#endif
};
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

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_0;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	while(1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);

		vTaskDelay(100/portTICK_RATE_MS);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,  GPIO_PIN_RESET);

		vTaskDelay(100/portTICK_RATE_MS);

	}
	
}
//--------------> begin System task functions <--------------//
