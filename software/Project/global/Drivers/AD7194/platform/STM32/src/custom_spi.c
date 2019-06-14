
/*.......................INCLUDES......................................*/
#include "custom_spi.h"

/*.........................PRIVATE_VARIABLES...........................*/

SPI_InitTypeDef SPI_InitStructure;
  /* Create SPIs struct */
SPI_HandleTypeDef SPI_Handle;
	/* Create GPIOs struct */
GPIO_InitTypeDef GPIO_InitStructure;



void spi_init ( void ){


	/* Activate Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_SPI2_CLK_ENABLE();

  /* Config GPIOs */

  /* Configure spi GPIOs */
	GPIO_InitStructure.Pin 				= EXT_ADC_MISO_PIN | EXT_ADC_MOSI_PIN;
  GPIO_InitStructure.Speed 			= GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Mode 			= GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull 			= GPIO_PULLDOWN;
	GPIO_InitStructure.Alternate 	= EXT_ADC_SPI_ALT;


  HAL_GPIO_Init(EXT_ADC_SPI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin  	=	EXT_ADC_SCK_PIN;
	GPIO_InitStructure.Pull 	= GPIO_PULLUP;

	HAL_GPIO_Init(EXT_ADC_SPI_PORT, &GPIO_InitStructure);

		
	/*Configure NSS port*/

	GPIO_InitStructure.Pin  = EXT_ADC_NSS_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
  
	HAL_GPIO_Init(EXT_ADC_NSS_PORT, &GPIO_InitStructure);
	spi_disNSS();



  SPI_Handle.Instance 			= EXT_ADC_SPI;
  SPI_Handle.Init.Mode 			= SPI_MODE_MASTER;
  SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;
  SPI_Handle.Init.DataSize 	= SPI_DATASIZE_8BIT;
  SPI_Handle.Init.CLKPolarity 	= SPI_POLARITY_HIGH;
  SPI_Handle.Init.CLKPhase 	= SPI_PHASE_2EDGE;
  SPI_Handle.Init.NSS 			= SPI_NSS_SOFT;
  SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  SPI_Handle.Init.FirstBit 	= SPI_FIRSTBIT_MSB;
  SPI_Handle.Init.TIMode 		= SPI_TIMODE_DISABLE;
  SPI_Handle.Init.CRCCalculation 	= SPI_CRCCALCULATION_DISABLE;
  SPI_Handle.Init.CRCPolynomial 	= 7;

  if (HAL_SPI_Init(&SPI_Handle) != HAL_OK)
  {
    while(1);
		
  }


}

void spi_deInit ( void )
{

  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_SPI2_CLK_DISABLE();

  __HAL_RCC_SPI2_FORCE_RESET();
  __HAL_RCC_SPI2_RELEASE_RESET();


  if(HAL_SPI_DeInit(&SPI_Handle) != HAL_OK)
  {
    while(1);
  }

  

  GPIO_InitTypeDef initStruct={0};
    
  initStruct.Mode =GPIO_MODE_OUTPUT_PP;

  initStruct.Pull =GPIO_NOPULL  ; 
  HW_GPIO_Init ( EXT_ADC_SPI_PORT, EXT_ADC_MOSI_PIN, &initStruct ); 
  HW_GPIO_Write( EXT_ADC_SPI_PORT, EXT_ADC_MOSI_PIN, 0 );
  
  initStruct.Pull =GPIO_PULLDOWN; 
  HW_GPIO_Init ( EXT_ADC_SPI_PORT, EXT_ADC_MISO_PIN, &initStruct ); 
  HW_GPIO_Write( EXT_ADC_SPI_PORT, EXT_ADC_MISO_PIN, 0 );
  
  initStruct.Pull =GPIO_NOPULL  ; 
  HW_GPIO_Init ( EXT_ADC_SPI_PORT, EXT_ADC_SCK_PIN, &initStruct ); 
  HW_GPIO_Write(  EXT_ADC_SPI_PORT, EXT_ADC_SCK_PIN, 0 );

  initStruct.Pull =GPIO_NOPULL  ; 
  HW_GPIO_Init ( EXT_ADC_SPI_PORT, EXT_ADC_NSS_PIN , &initStruct ); 
  HW_GPIO_Write( EXT_ADC_SPI_PORT, EXT_ADC_NSS_PIN , 0 );
}



void spi_enNSS(){
	HAL_GPIO_WritePin(EXT_ADC_NSS_PORT, EXT_ADC_NSS_PIN, GPIO_PIN_RESET);
}

void spi_disNSS(){
	HAL_GPIO_WritePin(EXT_ADC_NSS_PORT, EXT_ADC_NSS_PIN, GPIO_PIN_SET);
}

void spi_Transmit(uint8_t* data, uint8_t size){
  uint8_t input[8];
	HAL_SPI_TransmitReceive(&SPI_Handle, data, input, size, HAL_MAX_DELAY);
}

void spi_Receive(uint8_t* data, uint8_t size){
  uint8_t input[8];
  memset1(input, 0, size);
	HAL_SPI_TransmitReceive(&SPI_Handle,input, data, size, HAL_MAX_DELAY);
}

void spi_delay(uint32_t millis){
	HW_RTC_DelayMs(millis);
}
