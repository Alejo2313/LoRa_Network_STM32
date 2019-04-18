/** 
 *	@file extADC.c

		@brief Functions that handle the external ADC

		@author Alvaro Gutierrez
		@author Robolabo
		@author www.robolabo.etsit.upm.es
		@date 2013/05/22 

    CREATIVE COMMONS PUBLIC LICENSE:
		
		THE WORK (AS DEFINED BELOW) IS PROVIDED UNDER THE TERMS OF THIS CREATIVE COMMONS PUBLIC LICENSE ("CCPL" OR "LICENSE"). 
		THE WORK IS PROTECTED BY COPYRIGHT AND/OR OTHER APPLICABLE LAW. 
		ANY USE OF THE WORK OTHER THAN AS AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.

		BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE TERMS OF THIS LICENSE. 
		TO THE EXTENT THIS LICENSE MAY BE CONSIDERED TO BE A CONTRACT, 
		THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF SUCH TERMS AND CONDITIONS.
*/

/* Includes ------------------------------------------------------------------*/

/* Board Configuration File */
#include "project_config.h"  

/* Standard includes. */
#include <stdio.h>
#include "extADC.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

	SPI_InitTypeDef SPI_InitStructure;
  /* Create SPIs struct */
  SPI_HandleTypeDef SPI_Handle;
	/* Create GPIOs struct */
	GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void ExtADC_loadConfigReg(uint8_t* buffer, ExtChannel_t channel);
void ExtADC_loadModeReg(uint8_t* buffer);
uint8_t 	ExtADC_CalcRealGain 	( ExtGain_t gain );
void ExtADC_EnableDRDYInt();
void ExtADC_DisableDRDYInt();



/* Private functions ---------------------------------------------------------*/

void ExtADC_Init ( void )
{
	uint8_t aux = 0;

	for(aux = 0; aux < NUM_CHANNELS; aux++){
		channelConfig[aux].gain = EXTADC_GAIN_1;
		channelConfig[aux].mode = PSEUDO;
		channelConfig[aux].negative = AINCOM;
	}

	spi_init();

}

/**
 * @brief 
 * 
 */

void ExtADC_DeInit ( void )
{
	spi_deInit();
}


/**
 * @brief 
 * 
 * @param channel 
 * @param Gain 
 * @param mode 
 * @param negativeInput 
 */
void ExtADC_ConfigChannel(ExtChannel_t channel, ExtGain_t Gain, ExtMode_t mode, ExtChannel_t negativeInput){

	channelConfig[channel].gain = gain;
	channelConfig[channel].mode = mode;

	if(Mode)
		channelConfig[channel].negative = negativeInput;
	else
		channelConfig[Channel].negative = AINCOM;

}



/**
 * @brief ADC soft reset
 * 
 */
void ExtADC_Reset ( void )
{
  uint8_t out = 0xFF;
	/* Enable NSS */
	spi_enNSS();
	/* Go for reset */

	spi_Transmit(&out, 1);
	spi_Transmit(&out, 1);
	spi_Transmit(&out, 1);
	spi_Transmit(&out, 1);
	spi_Transmit(&out, 1);


	/* Disable NSS */
	spi_disNSS();
}



/**
 * @brief Read 24bit sampled data from channel
 * 
 * @param channel channel to be read
 * @return uint32_t readed data
 */
uint32_t ExtADC_ReadAnalogInput ( ExtChannel_t channel ) {

	uint8_t buffer[3];	//out buffer

	//Confgiure CONF_REG to allow channel reading

	ExtADC_loadConfigReg(buffer, channel);
	ExtADC_WriteRegister(CONF_REG, buffer, 3);

	ExtADC_loadModeReg(buffer);
	ExtADC_WriteRegister(MODE_REG, buffer, 3):
	
	/* Wait for RDY line down */   //ToDo -> paea que sirve esto
	//while (HAL_GPIO_ReadPin(EXT_ADC_MISO_PORT, EXT_ADC_MISO_PIN) == GPIO_PIN_SET)
	//	vTaskDelay(1);
		
		/* Read Data REG */	

	return ExtADC_ReadRegister(DATA_REG, 3);
}



/**
 * @brief Read voltage 
 * 
 * @param channel channel to be read
 * @return float  floating point data readed 
 */

float ExtADC_ReadVoltageInput ( ExtChannel_t channel)
{
	uint32_t 	code  	= 0;
	float 		voltage = 0;

	code = ExtADC_ReadAnalogInput ( channel );
	/* CODE = 2^(N-1) * [(AIN * Gain/V_ref) +1]
	 * AIN 	= ((CODE / 2^(N-1)) -1 ) * (V_ref/Gain)
	 */
	if (channelConfig[channel].mode == DIFFERENTIAL)
	{
		voltage = (float) code / (float) EXTADC_HALF_RESOL;
		voltage -= 1.0;
		voltage *= (float) EXTADC_REFERENCE;
		voltage /= (float) ExtADC_CalcRealGain(uChannelGain[channel]);
		voltage *= 1000; //Go for mV

	}
	/* CODE = (2^N * AIN * Gain)/V_ref
	 * AIN 	= (CODE * V_ref)/(2^N * Gain) 
	 * */
	else
	{
		voltage = (float) code * (float) EXTADC_REFERENCE;
		voltage /= (float) EXTADC_RESOL;
		voltage /= (float) ExtADC_CalcRealGain(uChannelGain[channel]);
		voltage *= 1000; //Go for mV
	}

	return voltage;
}


/**
 * @brief Read internal temperature sensor
 * 
 * @return float data readed
 */
float ExtADC_ReadTempSensor ( void )
{
    uint8_t out;
		uint32_t 	data;
		float temp;


		uint8_t buffer[3];

		/* Write communication register to allow a writing to the configuration register */ 

	
		/* Write data to the configuration REGISTER to read tmp: 
		 * Temp ON: bit 16
		 * BUF ON: bit 4
		 * GAIN MAX (111): bit 2, 1 and 0 
		 * TOTAL: 0000 0001 0000 0000 0001 0111 --> 0x010017
		 * */

		buffer[0] = 0x01;
		buffer[1] = 0x00;
		buffer[2] = 0x17;

		ExtADC_WriteRegister(CONF_REG, buffer, 3);
		/* Write communication register to allow writing to the Mode register */
		ExtADC_loadModeReg(buffer)
		/* Write data to the MODE REG, to allow a single reading*/

    ExtADC_WriteRegister(MODE_REG, buffer, 3);

/*
		while (HAL_GPIO_ReadPin(EXT_ADC_MISO_PORT, EXT_ADC_MISO_PIN) == GPIO_PIN_SET)
			vTaskDelay(1);
		*/


		/* Read Data REG */	
		data = ExtADC_ReadRegister(DATA_REG, 3);

		/* Convert data to degreees */
		temp = (float) (data - 0x800000) / 2815; //In Kelvin
		temp -= 273; //in celsius

		return temp;
}


// We will change this!!!

/**
 * @brief Read Full sacale register
 * 
 * @return uint32_t register value
 */
uint32_t ExtADC_ReadFullScaleReg ( void )
{

	return ExtADC_ReadRegister(FULLSCALE_REG, 3);
}

/**
 * @brief Read offset register
 * 
 * @return uint32_t register value
 */

uint32_t ExtADC_ReadOffsetReg ( void )
{
	return ExtADC_ReadRegister(OFFSET_REF, 3);
}

/**
 * @brief Read config register
 * 
 * @return uint32_t register value
 */

uint32_t ExtADC_ReadConfReg ( void )
{
	return ExtADC_ReadRegister(CONF_REG, 3); 
}

/**
 * @brief Read data register
 * 
 * @return uint32_t register value
 */

uint32_t ExtADC_ReadDataReg ( void )
{
	return ExtADC_ReadRegister(DATA_REG, 3); 
}

/**
 * @brief Read mode register
 * 
 * @return uint32_t register value
 */

uint32_t ExtADC_ReadModeReg ( void )
{
	return ExtADC_ReadRegister(MODE_REG, 3); 
}

/**
 * @brief Read GPOCON register
 * 
 * @return uint8_t register value
 */

uint8_t ExtADC_ReadGPOCONReg ( void )
{	
	return (uint8_t)ExtADC_ReadRegister(GPOCON_REG, 1); 
}

/**
 * @brief Read ID register
 * 
 * @return uint8_t 
 */
uint8_t ExtADC_ReadIDReg ( void )
{
	return (uint8_t)ExtADC_ReadRegister(ID_REG, 1); 
}



/**
 * @brief Load configuratión register value
 * 
 * @param buffer Buffer to store value
 * @param channel Channel to be configured 
 */
void ExtADC_loadConfigReg(uint8_t* buffer, ExtChannel_t channel){

	if( channelConfig[channel].mode == PSEUDO){
		buffer[0] = 0x84;
		buffer[1] = (channel << 4) | (channelConfig[channel].negative & 0x0F);
		buffer[2] =	channelConfig[channel].gain;
	}
	else{
		buffer[0] = 0x80;
		buffer[1] = (channel << 4) | (channelConfig[channel].negative & 0x0F);
		buffer[2] = 0x08 | channelConfig[channel].gain;
	}	

}
/**
 * @brief 
 * 
 * @param buffer 
 * todo -> hacerlo bien 
 */
void ExtADC_loadModeReg(uint8_t* buffer){
	buffer[0] = 0x28;
	buffer[1] = 0x00;
	buffer[2] = 0x60;
}


/**
 * @brief Update gain value for a giving channel
 * 
 * @param channel channel to update
 * @param gain Gain value
 */
void ExtADC_SetGain	( ExtChannel_t channel, ExtGain_t gain )
{
	channelConfig[Channel].gain = gain;
}

/**
 * @brief read channel's gaim
 * 
 * @param channel Channel 
 * @return ExtGain_t  Channel's gain
 */
ExtGain_t ExtADC_GetGain ( ExtChannel_t channel)
{
	return channelConfig[Channel].gain;
}

/**
 * @brief update channel mode 
 * 
 * @param channel  channel to update
 * @param mode 		 channel mode: PSEUDO -> single mode, DIFFERENTIAL -> differential mode
 * @param negative Negative channel in differential mode. if you set PSEUDO mode, this values is set to GND
 */

void ExtADC_SetMode	( ExtChannel_t channel,ExtMode_t mode, ExtChannel_t negative)
{	
	if(mode == DIFFERENTIAL){
		channelConfig[channel].mode = DIFFERENTIAL;
		channelConfig[channel].negative = negative;
	}
	else{
		channelConfig[channel].mode = PSEUDO;
		channelConfig[channel].negative = AINCOM;
	}
}

/**
 * @brief Get actual channel's mode
 * 
 * @param channel Channel
 * @return ExtMode_t Actual mode
 */

ExtMode_t ExtADC_GetMode	( ExtChannel_t channel )
{
	return channelConfig[channel].mode;
}

/**
 * @brief Give real gain
 * 
 * @param gain gain
 * @return uint8_t real gain
 * (I'm to lazy)
 */

uint8_t 	ExtADC_CalcRealGain 		( ExtGain_t gain )
{
	switch (gain)
	{
		case EXTADC_GAIN_1:
			return 1;
			break;
		case EXTADC_GAIN_8:	
			return 8;
			break;
		case EXTADC_GAIN_16:
			return 16;
			break;
		case EXTADC_GAIN_32:
			return 32;
			break;
		case EXTADC_GAIN_64:
			return 64;
			break;
		case EXTADC_GAIN_128:
			return 128;
			break;
		default: 
			return 0;
			break;
	}
}



/**
 * @brief Read register value
 * 
 * @param reg  register to be read
 * @param size amount of data to be read in bytes (1, 8 or 24 bits)
 * @return uint32_t data readed 
 */


uint32_t ExtADC_ReadRegister(uint8_t reg, uint8_t size){
	static uint8_t inBuffer[4];
	static uint32_t data = 0;
	static uint8_t aux = 0;

	memset(inBuffer, 0, 4);
	data = 0;
	aux = 0;

	uint8_t comByte = 0x40 | ((reg << 3)&0x38) | 0x00;

	spi_enNSS();
	spi_Transmit(&comByte, 1U);
	spi_Receive(inBuffer, size);
	spi_disNSS();

	while(--size){
		data |= inBuffer[aux] << (8*val);
	}
	return data;
}

/**
 * @brief Write register
 * 
 * @param reg register to be writed 
 * @param data data pointer
 * @param size amount of data in bytes
 */
void ExtADC_WriteRegister(uint8_t reg, uint8_t* data, uint8_t size){

	uint8_t comByte = 0x00 | ((reg << 3)&0x38) | 0x00;

	spi_enNSS();
	spi_Transmit(&comByte, 1U);
	spi_Transmit(data, size);
	spi_disNSS();
}





//<------------------------------------------------------------------------------>


void spi_init ( void )
{

	//ToDo ->  añadir init de configuracion 

	/* Activate Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  EXT_ADC_SPI_CLK();

  /* Config GPIOs */

  /* Configure spi GPIOs */
	GPIO_InitStructure.Pin 				= EXT_ADC_MISO_PIN | EXT_ADC_MOSI_PIN | EXT_ADC_SCK_PIN;
  GPIO_InitStructure.Speed 			= GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Mode 			= GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull 			= GPIO_PULLDOWN;
	GPIO_InitStructure.Alternate 	= EXT_ADC_SPI_ALT;


  if(HAL_GPIO_Init(EXT_ADC_SPI_PORT, &GPIO_InitStructure) != HAL_OK)
		while(1);  //somethinng wrong
		
	/*Configure NSS port*/

	GPIO_InitStruct.Pin  = EXT_ADC_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  
	if(HAL_GPIO_Init(GPIOA, &GPIO_InitStruct) != HAL_OK)
		while(1);	



  /* Config SPI mode */
  SPI_Handle.Instance 					= EXT_ADC_SPI;
  SPI_Handle.Init.Direction 		= SPI_DIRECTION_2LINES;
  SPI_Handle.Init.Mode 					= SPI_MODE_MASTER;
  SPI_Handle.Init.DataSize 			= SPI_DATASIZE_8BIT;
  SPI_Handle.Init.CLKPolarity 	= SPI_POLARITY_HIGH;
  SPI_Handle.Init.CLKPhase 			= SPI_PHASE_2EDGE; 
  SPI_Handle.Init.NSS 					= SPI_NSS_SOFT;
  SPI_Handle.Init.NSSPMode 			= SPI_NSS_PULSE_DISABLE;
  SPI_Handle.Init.FirstBit 			= SPI_FIRSTBIT_MSB;
  SPI_Handle.Init.CRCPolynomial = 7;
  SPI_Handle.Init.CRCCalculation= SPI_CRCCALCULATION_DISABLE;
  SPI_Handle.Init.CRCLength 		= SPI_CRC_LENGTH_DATASIZE;
  SPI_Handle.Init.TIMode 				= SPI_TIMODE_DISABLE;
	SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;


  if(HAL_SPI_Init(&SPI_Handle) != HAL_OK)
  	while(1);

}

void spi_deInit ( void )
{

  /* Deconfig GPIOs */
  /* Deconfigure SCK as Alternative Function PP */
  HAL_GPIO_DeInit(EXT_ADC_SCK_PORT, EXT_ADC_SCK_PIN);

  /* Deinitialize previous SPI configurations */
  if(HAL_SPI_DeInit(&SPI_Handle) != HAL_OK)
  {
    while(1);
  }

  /* Deselect extADC module on SPI bus */
  HAL_GPIO_WritePin(EXT_ADC_NSS_PORT, EXT_ADC_NSS_PIN, GPIO_PIN_SET);
}

void spi_enNSS(){
	HAL_GPIO_WritePin(EXT_ADC_NSS_PORT, EXT_ADC_NSS_PIN, GPIO_PIN_RESET);
}

void spi_disNSS(){
	HAL_GPIO_WritePin(EXT_ADC_NSS_PORT, EXT_ADC_NSS_PIN, GPIO_PIN_SET);
}

void spi_Transmit(uint8_t* data, uint8_t size){
	HAL_SPI_Transmit(&SPI_Handle, data, size, HAL_MAX_DELAY);
}

void spi_Receive(uint8_t* data, uint8_t size){
	HAL_SPI_Receive(&SPI_Handle, data, size, HAL_MAX_DELAY);
}
void spi_TransmitReceive(uint8_t* data, uint8_t size){

}

/******************** Creative Commons -- Robolabo *****************************/
/***************************** END OF FILE *************************************/

