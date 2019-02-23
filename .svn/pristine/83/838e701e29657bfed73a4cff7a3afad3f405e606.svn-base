#include "main.h"
#include "uartDriver.h"

static UART_HandleTypeDef huart;
volatile xQueueHandle   syncQueue;
volatile xSemaphoreHandle syncSem;

void usartInit(){


  huart.Instance        = USART2;
  huart.Init.BaudRate   = 115200;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits   = UART_STOPBITS_1;
  huart.Init.Parity     = UART_PARITY_NONE;
  huart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  huart.Init.Mode       = UART_MODE_TX_RX;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;

  HAL_UART_Init(&huart);
  HAL_NVIC_SetPriority(USARTx_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
  syncQueue = xQueueCreate(50,sizeof(uint8_t));
  syncSem = xSemaphoreCreateBinary();
  xSemaphoreGive(syncSem);
}

void setUsartIRQ()
{
  SET_BIT(huart.Instance->CR1, USART_CR1_TE);
  SET_BIT(huart.Instance->CR1, USART_CR1_TXEIE);
  CLEAR_BIT(huart.Instance->CR1,USART_CR1_TCIE);
}

void USARTx_IRQHandler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  uint8_t data;
  uint32_t isrflags   = READ_REG(huart.Instance->ISR);
  uint32_t cr1its     = READ_REG(huart.Instance->CR1);
  portBASE_TYPE xTaskWoken = pdFALSE;

  if(((isrflags & USART_ISR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    if(xQueueReceiveFromISR(syncQueue,&data,&xTaskWoken)==pdPASS){

      huart.Instance->TDR = (uint16_t)(data & 0x01FF);
      xSemaphoreGiveFromISR( syncSem, &xHigherPriorityTaskWoken );
      /* Because there is a switch from ISR */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      return;

    }else{
      CLEAR_BIT(huart.Instance->CR1, USART_CR1_TXEIE);

      // Enable the UART Transmit Complete Interrupt 
      SET_BIT(huart.Instance->CR1, USART_CR1_TCIE);
      xSemaphoreGiveFromISR( syncSem, &xHigherPriorityTaskWoken );
      /* Because there is a switch from ISR */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      return;
    }
  }
  if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    CLEAR_BIT(huart.Instance->CR1, USART_CR1_TCIE);
  }
}

void println(uint8_t* p, uint32_t size)
{
  while(xSemaphoreTake(syncSem,10) != pdTRUE);
  uint8_t i;
  for(i=0; i<size; i++){
    xQueueSend(syncQueue,(p+i),portMAX_DELAY);
  }
  setUsartIRQ();
}
