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

/* Standard includes. */
#include <stdio.h>
#include "extADC.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE 5
/* Private variables ---------------------------------------------------------*/

static extADC_t channelConfig[NUM_CHANNELS];
static ExtModeReg_t mode;

static uint8_t inBuffer[BUFFER_SIZE];
static uint8_t outBuffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/


void ExtADC_loadConfigReg(uint8_t* buffer, ExtChannel_t channel);
void ExtADC_loadModeReg(uint8_t* buffer);
uint8_t ExtADC_CalcRealGain 	( ExtGain_t gain );
uint8_t ExtADC_GetRegSize(uint8_t reg);



/* Private functions ---------------------------------------------------------*/

void ExtADC_Init ( void )
{
	uint8_t aux = 0;

	uint8_t buffer[3];

	for(aux = 0; aux < NUM_CHANNELS; aux++){
		channelConfig[aux].gain = EXTADC_GAIN_1;
		channelConfig[aux].mode = PSEUDO;
		channelConfig[aux].negative = AINCOM;
		channelConfig[aux].ExtConfigOptions = 0;
	}

	spi_init();

	mode.mode = SINGLE;
	mode.clk  = INTERNAL;
	mode.average = AVG_0;
	mode.filterSelect = 0x60;
	mode.ExtModeOptions = 0;


	ExtADC_loadModeReg(buffer);
	ExtADC_WriteRegister(MODE_REG, buffer);

	
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
void ExtADC_ConfigChannel(ExtChannel_t channel, extADC_t* config){

	channelConfig[channel].gain = config->gain;
	channelConfig[channel].mode = config->mode;
	channelConfig[channel].ExtConfigOptions = config->ExtConfigOptions;

	if(config->mode == DIFFERENTIAL)
		channelConfig[channel].negative = config->negative;
	else
		channelConfig[channel].negative = AINCOM;

}



/**
 * @brief ADC soft reset
 * 
 */
void ExtADC_Reset ( void )
{

	memset1(outBuffer, 0xFF, BUFFER_SIZE);
	/* Go for reset */
	spi_Transmit(outBuffer, 5); //ToDo revisar
	/* Disable NSS */
}



/**
 * @brief Read 24bit sampled data from channel
 * 
 * @param channel channel to be read
 * @return uint32_t readed data
 */
uint32_t ExtADC_ReadAnalogInput ( ExtChannel_t channel ) {

	//static uint8_t buffer[3];	//out buffer
	static  uint16_t counter;

	//Confgiure CONF_REG to allow channel reading

	ExtADC_loadConfigReg(outBuffer, channel);
	ExtADC_WriteRegister(CONF_REG, outBuffer);

	ExtADC_loadModeReg(outBuffer);
	ExtADC_WriteRegister(MODE_REG, outBuffer);

	counter = 0;


	while (ExtADC_DataReady(channel) == -1){
				spi_delay(1);
		counter++;

		if( counter > 500)
			return 0;

	}

	return ExtADC_ReadRegister(DATA_REG);

}



/**
 * @brief Read voltage 
 * 
 * @param channel channel to be read
 * @return float  floating point data readed 
 */

float ExtADC_ReadVoltageInput ( ExtChannel_t channel)
{
	static uint32_t 	code  	= 0;
	static double 		voltage = 0;

	code = 0;
	voltage = 0;
  
	code = ExtADC_ReadAnalogInput ( channel );
	/* CODE = 2^(N-1) * [(AIN * Gain/V_ref) +1]
	 * AIN 	= ((CODE / 2^(N-1)) -1 ) * (V_ref/Gain)
	 */
	if (!(channelConfig[channel].ExtConfigOptions & UNIPOLAR))
	{
		voltage = (float) code / (float) EXTADC_HALF_RESOL;
		voltage -= 1.0;
		voltage *= (float) EXTADC_REFERENCE;
		voltage /= (float) ExtADC_CalcRealGain(channelConfig[channel].gain);
		voltage *= 1000; //Go for mV

	}
	/* CODE = (2^N * AIN * Gain)/V_ref
	 * AIN 	= (CODE * V_ref)/(2^N * Gain) 
	 * */
	else
	{
		voltage = (float) code * (float) EXTADC_REFERENCE;
		voltage /= (float) EXTADC_RESOL;
		voltage /= (float) ExtADC_CalcRealGain(channelConfig[channel].gain);
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
	static uint32_t data = 0;
	static uint16_t counter = 0;
	static float temp = 0;

	data = 0;
	counter = 0;
	temp = 0;
	//static uint8_t buffer[3];

		/* Write communication register to allow a writing to the configuration register */ 

	
		/* Write data to the configuration REGISTER to read tmp: 
		 * Temp ON: bit 16
		 * BUF ON: bit 4
		 * GAIN MAX (111): bit 2, 1 and 0 
		 * TOTAL: 0000 0001 0000 0000 0001 0111 --> 0x010017
		 * */

	
	outBuffer[0] = 0x01;
	outBuffer[1] = 0x00;
	outBuffer[2] = 0x17;

	ExtADC_WriteRegister(CONF_REG, outBuffer);

	ExtADC_loadModeReg(outBuffer);
	ExtADC_WriteRegister(MODE_REG, outBuffer);

	counter = 0;
	while (ExtADC_DataReady(0) == -1){
		spi_delay(1);
		counter++;

		if( counter > 500)
			return 0;
	}
		
	data  = ExtADC_ReadRegister(DATA_REG);


	/* Convert data to degreees */
	temp = (float) (data - 0x800000) / 2815; //In Kelvin
	temp -= 273; //in celsius

	return temp;

}


/**
 * @brief Load configuratión register value
 * 
 * @param buffer Buffer to store value
 * @param channel Channel to be configured 
 */
void ExtADC_loadConfigReg(uint8_t* buffer, ExtChannel_t channel){
	static t_u32_in_4 res;

	res.all = 0;

	res.all = 1 << 23;
	res.all |= channelConfig[channel].gain & 0x7;
	res.all |= (channelConfig[channel].mode << 18);
	res.all |= (channel&0xF) << 12;
	res.all |= (channelConfig[channel].negative & 0xF) << 8;
	res.all |= channelConfig[channel].ExtConfigOptions;

	buffer[0] = res.bytes[2];
	buffer[1] = res.bytes[1];
	buffer[2] =	res.bytes[0];

}
/**
 * @brief 
 * 
 * @param buffer 
 * todo -> hacerlo bien 
 */
void ExtADC_loadModeReg(uint8_t* buffer){
	static t_u32_in_4 res;
	res.all = 0;
	res.all |= mode.mode << 21;
	res.all |= (mode.clk << 18);
	res.all |= (mode.average << 16);
	res.all |= mode.filterSelect & 0x3FF; 
	res.all |= mode.ExtModeOptions ;


	buffer[0] = res.bytes[2];
	buffer[1] = res.bytes[1];
	buffer[2] = res.bytes[0];
}
/**
 * @brief 
 * 
 * @param config 
 */
void ExtADC_setModeConfig(ExtModeReg_t* config){
	mode.mode = config->mode;
	mode.clk  = config->clk;
	mode.average = config->average;
	mode.filterSelect = config->filterSelect;
	mode.ExtModeOptions = config->ExtModeOptions;

	ExtADC_loadModeReg(outBuffer);
	ExtADC_WriteRegister(MODE_REG, outBuffer);
}

/**
 * @brief Update gain value for a giving channel
 * 
 * @param channel channel to update
 * @param gain Gain value
 */
void ExtADC_SetGain	( ExtChannel_t channel, ExtGain_t gain )
{
	channelConfig[channel].gain = gain;
}

/**
 * @brief read channel's gaim
 * 
 * @param channel Channel 
 * @return ExtGain_t  Channel's gain
 */
ExtGain_t ExtADC_GetGain ( ExtChannel_t channel)
{
	return channelConfig[channel].gain;
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

ExtMode_t ExtADC_GetMode	( ExtChannel_t channel ){
	return channelConfig[channel].mode;
}

/**
 * @brief Give real gain
 * 
 * @param gain gain
 * @return uint8_t real gain
 * (I'm to lazy)
 */

uint8_t ExtADC_CalcRealGain ( ExtGain_t gain )
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


uint32_t ExtADC_ReadRegister(uint8_t reg){
	//static uint8_t inBuffer[3] = {0,0,0};
	static uint32_t data = 0;
	static uint8_t aux = 0, size;

	memset1(inBuffer, 0x00, BUFFER_SIZE);
	size = ExtADC_GetRegSize(reg);

	data = 0;
	aux = 0;

	uint8_t comByte = 0x40 | ((reg << 3)&0x38) | 0x00;
	spi_enNSS();

	spi_Transmit(&comByte, 1U);
	spi_Receive(inBuffer, size);

	spi_disNSS();

	while(size){
		--size;
		data |= inBuffer[aux++] << (8*size);
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
void ExtADC_WriteRegister(uint8_t reg, uint8_t* data){

	uint8_t comByte = 0x00 | ((reg << 3)&0x38) | 0x00;

	spi_enNSS();
	spi_Transmit(&comByte, 1U);
	spi_Transmit(data, ExtADC_GetRegSize(reg));
	spi_disNSS();

}

/**
 * @brief 
 * 
 * @return int 
 */

int ExtADC_DataReady(ExtChannel_t channel){
	static uint8_t stReg;

	stReg = ExtADC_ReadRegister(STATUS_REG);

	// b 7:-> 0: data ready  3 - 0: channel
	//See status register -> Table 18, AD7194 REV.B

	if((~stReg)&0x80 && (stReg&0xF) == channel){
		return stReg & 0x0F;
	}

	return -1;
}


void ExtADC_Calibrate(){


}

void ExtADC_SetOpMode(ExtOpMode_t mode){

}

uint8_t ExtADC_GetRegSize(uint8_t reg){

	switch (reg)
	{
		case COM_REG:
		case ID_REG:
		case GPOCON_REG:
			return 1;
			break;

		case MODE_REG:
		case CONF_REG:
		case DATA_REG:
		case OFFSET_REF:
		case FULLSCALE_REG:
			return 3;
			break;

		default:
			return 0;
			break;
	}
}



/******************** Creative Commons -- Robolabo *****************************/
/***************************** END OF FILE *************************************/

