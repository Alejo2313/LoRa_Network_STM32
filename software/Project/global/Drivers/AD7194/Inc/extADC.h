/******************** (C) COPYRIGHT 2007 RBZ Robot Design S.L.******************
* File Name          : extADC.h
* Author             : Alvaro Gutierrez
* Author             : Robolabo
* Date First Issued  : 04/07/2013
* Description        : Cabezeras de funciones de adc.c
********************************************************************************
* History:
* 10/10/2012 : Created
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef _EXT_ADC_H_
#define _EXT_ADC_H_

/* Includes ------------------------------------------------------------------*/
#include "math.h"
#include "custom_spi.h"

/* Defines ------------------------------------------------------------*/

#define NUM_CHANNELS        	16


//Register selection ( Table 17, AD7194 datasheet REV.B)


#define COM_REG			0x0		// 8 bits 		// W
#define STATUS_REG		0x0		// 8 bits		// R
#define MODE_REG		0x1		// 24 bits		// R/W
#define CONF_REG		0x2		// 24 bits		// R/W
#define DATA_REG		0x3		// 24 bits		// R
#define ID_REG			0x4		// 8 bits		// R
#define GPOCON_REG		0x5		// 8 bits		// R/W
#define OFFSET_REF		0x6		// 24 bits		// R/W
#define FULLSCALE_REG	0x7		// 24 bits		// R/W


#define EXTADC_REFERENCE		2.5
#define EXTADC_BITS				24
#define EXTADC_RESOL			16777216 /* 2^EXTADC_BITS */
#define EXTADC_HALF_RESOL		8388608 /* 2^(EXTADC_BITS-1) */

#define AINCOM 					0x00





/* Typedef -----------------------------------------------------------*/

typedef enum{
	CHANNEL_1	= 0x00,
	CHANNEL_2	= 0x01,
	CHANNEL_3	= 0x02,
	CHANNEL_4	= 0x03,
	CHANNEL_5	= 0x04,
	CHANNEL_6	= 0x05,
	CHANNEL_7	= 0x06,
	CHANNEL_8	= 0x07,
	CHANNEL_9	= 0x08,
	CHANNEL_10	= 0x09,
	CHANNEL_11	= 0x0A,
	CHANNEL_12	= 0x0B,
	CHANNEL_13	= 0x0C,
	CHANNEL_14	= 0x0D,
	CHANNEL_15	= 0x0E,
	CHANNEL_16	= 0x0F,
}ExtChannel_t;	

// Gain Select bits ( Table 21,  AD7194 datasheet REV.B)
typedef enum{
	EXTADC_GAIN_1 	= 0x00,
	EXTADC_GAIN_8 	= 0x03,
	EXTADC_GAIN_16 	= 0x04,
	EXTADC_GAIN_32	= 0x05,
	EXTADC_GAIN_64	= 0x06,
	EXTADC_GAIN_128	= 0x07
}ExtGain_t;

/**
 * @brief Operation modes, (Table 20, AD7194 datasheet REV.B)
 * 
 */
typedef enum{
	CONTINUOUS,
	SINGLE,
	IDLE,
	POWER_DOWN,
	INT_ZERO_SCALE_CALIB,
	INT_FULL_SCALE_CALIB,
	SYS_ZERO_SCALE_CALIB,
	SYS_FULL_SCALE_CALIB
}ExtOpMode_t;

typedef enum{
	DIFFERENTIAL,
	PSEUDO
}ExtMode_t;


typedef struct
{
	ExtGain_t gain;
	ExtMode_t mode;
	ExtChannel_t negative;
}extADC_t;

typedef struct 
{
	/* data */
}Ext_functions_t;




/* Variables ---------------------------------------------------------*/

static extADC_t channelConfig[NUM_CHANNELS];


/* Function prototypes -----------------------------------------------*/
void ExtADC_Init ( void );
void ExtADC_DeInit ( void );
void ExtADC_Reset ( void );


void ExtADC_ConfigChannel(ExtChannel_t channel, ExtGain_t Gain, ExtMode_t mode, ExtChannel_t negativeInput);

/* Returns the adc code */
uint32_t ExtADC_ReadAnalogInput ( ExtChannel_t channel );
/* Returns the adc voltage */
float ExtADC_ReadVoltageInput ( ExtChannel_t channel);
float ExtADC_ReadTempSensor	( void );

void ExtADC_WriteRegister(uint8_t reg, uint8_t* data, uint8_t size);
uint32_t ExtADC_ReadRegister(uint8_t reg);


void ExtADC_SetMode			( ExtChannel_t channel,ExtMode_t mode, ExtChannel_t negative);
void ExtADC_SetGain			( ExtChannel_t channel, ExtGain_t gain );
ExtGain_t ExtADC_GetGain	( ExtChannel_t channel);
ExtMode_t ExtADC_GetMode	( ExtChannel_t channel );

int ExtADC_DataReady(ExtChannel_t channel);


#endif //_EXT_ADC_H_

