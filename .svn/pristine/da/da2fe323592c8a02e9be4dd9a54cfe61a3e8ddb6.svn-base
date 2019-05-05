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
#include "hw.h"
#include "math.h"

/* Defines ------------------------------------------------------------*/

#define NUM_CHANNELS        4

#define WRITE_MODE_REG 			0x08
#define WRITE_CONF_REG 			0x10

#define READ_MODE_REG				0x48
#define READ_CONF_REG				0x50
#define READ_DATA_REG				0x58
#define READ_ID_REG					0x60
#define READ_GPOCON_REG			0x68
#define READ_OFFSET_REG			0x70
#define READ_FULLSCALE_REG	0x78

#define AIN0_DIFF_CONF      0x01
#define AIN1_DIFF_CONF      0x23
#define AIN2_DIFF_CONF      0x45
#define AIN3_DIFF_CONF      0x67
#define AIN4_DIFF_CONF      0x89
#define AIN5_DIFF_CONF      0xAB
#define AIN6_DIFF_CONF      0xCD
#define AIN7_DIFF_CONF      0xEF

#define AIN1_PSEUDO_CONF		0x00
#define AIN2_PSEUDO_CONF		0x10
#define AIN3_PSEUDO_CONF		0x20
#define AIN4_PSEUDO_CONF		0x30
#define AIN5_PSEUDO_CONF		0x40
#define AIN6_PSEUDO_CONF		0x50
#define AIN7_PSEUDO_CONF		0x60
#define AIN8_PSEUDO_CONF		0x70
#define AIN9_PSEUDO_CONF		0x80
#define AIN10_PSEUDO_CONF		0x90
#define AIN11_PSEUDO_CONF		0xA0
#define AIN12_PSEUDO_CONF		0xB0
#define AIN13_PSEUDO_CONF		0xC0
#define AIN14_PSEUDO_CONF		0xD0
#define AIN15_PSEUDO_CONF		0xE0
#define AIN16_PSEUDO_CONF		0xF0

#define EXTADC_GAIN_1				0x00 //+- 2.5V
#define EXTADC_GAIN_8				0x03 //+- 312.5 mV
#define EXTADC_GAIN_16			0x04 //+- 156.2 mV
#define EXTADC_GAIN_32			0x05 //+- 78.125 mV
#define EXTADC_GAIN_64			0x06 //+- 39.06 mV
#define EXTADC_GAIN_128			0x07 //+- 19.53 mV

#define EXTADC_REFERENCE		2.5
#define EXTADC_BITS					24
#define EXTADC_RESOL				16777216 /* 2^EXTADC_BITS */
#define EXTADC_HALF_RESOL		8388608 /* 2^(EXTADC_BITS-1) */

/* Typedef -----------------------------------------------------------*/
typedef union{
	int32_t all;
	//int all;
	struct t_bits{
		uint8_t byte0;
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
	}bytes;
}t_u32_in_4;

/* Variables ---------------------------------------------------------*/
static uint8_t uChannelGain[NUM_CHANNELS];
static uint8_t uChannelDifferential[NUM_CHANNELS];

/* Function prototypes -----------------------------------------------*/
extern void ExtADC_Init ( void );
extern void ExtADC_DeInit ( void );

/**	EXTADC_EnableDRDYInt()
 *  Habilita la interrupción externa que ataca a la línea DRDY
 */
extern void ExtADC_EnableDRDYInt();

/**	EXTADC_DisableDRDYInt()
 *  Habilita la interrupción externa que ataca a la línea DRDY
 */
extern void ExtADC_DisableDRDYInt();

/**	EXTADC_SpiOn()
 *  Activa el bus SPI para recepción de datos del ADC externo
 */
extern void ExtADC_SpiOn();

/**	EXTADC_SpiOff()
 *  Desactiva el bus SPI para recepción de datos del ADC externo
 */
extern void ExtADC_SpiOff();

extern void ExtADC_NSSOn();
extern void ExtADC_NSSOff();

extern uint8_t 				ExtADC_GetFlag ();
extern t_u32_in_4 ExtADC_GetData ();

extern void			ExtADC_Reset						( void );

/* Returns the adc code */
extern uint32_t ExtADC_ReadAnalogInput 	( uint8_t channel );
/* Returns the adc voltage */
extern float		ExtADC_ReadVoltageInput	( uint8_t channel );
extern uint8_t	ExtADC_GetDiffConf			( uint8_t channel );

extern float ExtADC_ReadTempSensor 	( void );
extern uint32_t ExtADC_ReadOffsetReg 		( void );
extern uint32_t ExtADC_ReadConfReg 			( void );
extern uint32_t ExtADC_ReadDataReg 			( void );
extern uint32_t ExtADC_ReadModeReg 			( void );
extern uint32_t	ExtADC_Read24bitsReg		( uint8_t reg );

extern uint8_t 	ExtADC_ReadGPOCONReg 		( void );
extern uint8_t 	ExtADC_ReadIDReg 				( void );
extern uint8_t	ExtADC_Read8bitsReg			( uint8_t reg );

extern void			ExtADC_SetGain					( uint8_t channel, uint8_t gain );
extern uint8_t 	ExtADC_GetGain					( uint8_t channel);
extern uint8_t 	ExtADC_CalcRealGain 		( uint8_t gain );

extern void			ExtADC_SetDifferential	( uint8_t channel, uint8_t mode );
extern uint8_t 	ExtADC_GetDifferential	( uint8_t channel);

extern t_u32_in_4 ExtADC_CalcCONFReg		( uint8_t channel );
extern t_u32_in_4 ExtADC_CalcMODEReg		( uint8_t channel );

#endif //_EXT_ADC_H_

