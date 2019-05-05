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

/**
 * @brief Gain Select bits ( Table 21,  AD7194 datasheet REV.B)
 * 
 */
typedef enum{					// ADC Input Range
	EXTADC_GAIN_1 	= 0x00,		// ±2.5v
	EXTADC_GAIN_8 	= 0x03,		// ±312.5mV
	EXTADC_GAIN_16 	= 0x04,		// ±156.2 mV
	EXTADC_GAIN_32	= 0x05,     // ±78.125 mV
	EXTADC_GAIN_64	= 0x06,     // ±39.06 mV
	EXTADC_GAIN_128	= 0x07      // ±19.53 mV
}ExtGain_t;

/**
 * @brief Operation modes, (Table 20, AD7194 datasheet REV.B)
 * 
 */
typedef enum{
	CONTINUOUS,					// The ADC continuously performs conversions and places the result in the data register
	SINGLE,						// The ADC powers up and performs a single conversion on the selected channel.
	IDLE,						// The ADC filter and modulator are held in a reset state
	POWER_DOWN,					// All AD7194 circuitry is powered down. 
	INT_ZERO_SCALE_CALIB,		// Internal zero-scale calibration.
	INT_FULL_SCALE_CALIB,		// Internal full-scale calibration.
	SYS_ZERO_SCALE_CALIB,		// System zero-scale calibration.
	SYS_FULL_SCALE_CALIB		//System full-scale calibration
}ExtOpMode_t;

/**
 * @brief select the clock source for the AD7194. 
 * 
 */
typedef enum 
{
	EXTERNAL_MCLK12, //External crystal. The external crystal is connected from MCLK1 to MCLK2.
	EXTERNAL_MCLK2,  //External clock. The external clock is applied to the MCLK2 pin.
	INTERNAL,	     //Internal 4.92 MHz clock. Pin MCLK2 is tristated.
	INTERNAL_MCLK2	 //Internal 4.92 MHz clock. The internal clock is available on MCLK2.
}ExtClockSRC_t;

/**
 * @brief Channel config options 
 * 
 */
typedef enum{
	UNIPOLAR	= 1 << 3,		//unipolar operation is selected, 
									//DEFAULT: BIPOLAR
	REFAIN_PINS	= 1 << 20,		//External reference applied between the AIN3/P1/REFIN2(+) and AIN4/P0/REFIN2(−) pins.
									// DEFAULT: External reference applied between REFIN1(+) and REFIN1(−).
	BURN 		= 1 << 7,		//The 500 nA current sources in the signal path are enabled
									// DEFAULT: disabled
	REFDET  	= 1 << 6,		//Enables the reference detect function.
									// DEFAULT: disabled
	BUF 		= 1 << 4		//Enables the buffer on the analog inputs. 
									// DEFAULT: disabled	
}ExtConfigOptions_t;

/**
 * @brief Operation mode options 
 * 
 */
typedef enum{
	DAT_STA		= 1 << 20, 		//enables the transmission of status register contents after each data register read
									//DEFAULT: Disabled
	SINC3 		= 1 << 15,		//Sinc3 filter select
									//DEFAULT: Sinc4 selected
	ENPAR		= 1 << 13,		//Enable parity bit
									//DEFAULT: Disabled
	CLK_DIV		= 1 << 12,		//Clock divide-by-2.  When performing internal full-scale calibrations, this bit must be set when AVDD is less than 4.75 V.
									//DEFAULT: no div
	SINGLE_C	= 1 << 11, 		//Single cycle conversion enable bit.
									//DEFAULT: disabled
	REJ60		= 1 << 10		//This allows simultaneous 50 Hz/ 60 Hz rejection
}ExtModeOptions_t;

/**
 * @brief reduces the output data rate 
 * 
 */
typedef enum{
	AVG_0,			//No averaging (fast settling mode disabled)
	AVG_2,			//Average by 2
	AVG_8,			//Average by 8
	AVG_16			//Average by 16
}ExtAverage_t;
/**
 * @brief Input modes
 * 
 */
typedef enum{
	DIFFERENTIAL,	//Diferential input
	PSEUDO			//Single/pseudo diferential (refered with AINCOM port)
}ExtMode_t;

/**
 * @brief Channel configuration structure
 * 
 */
typedef struct
{
	ExtGain_t gain;					// Input gain @see ExtGain_t
	ExtMode_t mode;					// Input mode @see ExtMode_t
	ExtChannel_t negative;			// Negative channel in differential mode (In pseudo mode, it don't mind)
	uint32_t ExtConfigOptions; 		// Other options @see ExtConfigOptions_t
}extADC_t;

/**
 * @brief Mode register configuration structure
 * 
 */
typedef struct
{
	ExtOpMode_t mode;				// Operation mode, @see ExtOpMode_t
	ExtClockSRC_t clk;				// Clock source, @see ExtClockSRC_t
	ExtAverage_t average;			// Average, @see ExtAverage_t
	uint16_t filterSelect;			// Filter select 
	uint32_t ExtModeOptions;		//
}ExtModeReg_t;


typedef union{
	int32_t all;
	uint8_t bytes[4];
}t_u32_in_4;



/* Function prototypes -----------------------------------------------*/

/* Init structures and system*/
void ExtADC_Init ( void );

/* deInit */
void ExtADC_DeInit ( void );

/* ADC soft reset*/
void ExtADC_Reset ( void );

/* channel configuration*/
void ExtADC_ConfigChannel ( ExtChannel_t channel, extADC_t* config );

/* Configure operation mode*/
void ExtADC_setModeConfig( ExtModeReg_t* config );

/* Returns the adc code */
uint32_t ExtADC_ReadAnalogInput ( ExtChannel_t channel );

/* Returns the adc voltage */
float ExtADC_ReadVoltageInput ( ExtChannel_t channel);

/* Returns internal temp*/
float ExtADC_ReadTempSensor	( void );

/* Return register value */
uint32_t ExtADC_ReadRegister(uint8_t reg);

/* Write to register */
void ExtADC_WriteRegister(uint8_t reg, uint8_t* data);

/* Change channel operation mode*/
void ExtADC_SetMode ( ExtChannel_t channel,ExtMode_t mode, ExtChannel_t negative);

/* Set channel gain */
void ExtADC_SetGain	( ExtChannel_t channel, ExtGain_t gain );

/* Get ADC gain*/
ExtGain_t ExtADC_GetGain ( ExtChannel_t channel);

/* Get operation mode*/
ExtMode_t ExtADC_GetMode ( ExtChannel_t channel );

/* Check if is data available */
int ExtADC_DataReady (ExtChannel_t channel);


#endif //_EXT_ADC_H_

