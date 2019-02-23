/**
 *  @brief  -> UART console output
 * 
 * 
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "hw.h"

UART_HandleTypeDef huart;
void Trace_Init(){

    //CLK config
    RCC_PeriphCLKInitTypeDef usartInit;


    if(USARTx == USART2){

        usartInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
        usartInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;

        if(HAL_RCCEx_PeriphCLKConfig(&usartInit) != HAL_OK){
            while(1);
        }
    }

    USARTx_CLK_ENABLE();
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();


    // GPIO config
    GPIO_InitTypeDef hgpio;

    hgpio.Pin           = USARTx_TX_PIN ;
    hgpio.Mode          = GPIO_MODE_AF_PP;
    hgpio.Alternate     = USARTx_TX_AF;
    hgpio.Speed         = GPIO_SPEED_HIGH;
    hgpio.Pull          = GPIO_NOPULL;  

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &hgpio);

    hgpio.Pin           = USARTx_RX_PIN;
    hgpio.Alternate     = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &hgpio);


    //UART config
    huart.Instance              = USARTx;
    huart.Init.BaudRate         = 115200;
    huart.Init.Parity           = UART_PARITY_NONE;
    huart.Init.Mode             = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl        = UART_HWCONTROL_NONE;
    huart.Init.StopBits         = UART_STOPBITS_1;
    huart.Init.WordLength       = UART_WORDLENGTH_8B;
    huart.Init.OneBitSampling   = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.Init.OverSampling     = UART_OVERSAMPLING_16;

   if( HAL_UART_Init(&huart) != HAL_OK){
       while(1);
   }
   HAL_NVIC_SetPriority(USARTx_IRQn, USARTx_Priority, 0);
   HAL_NVIC_EnableIRQ(USARTx_IRQn);

}


void Trace_send(const char* str, ...){
    static va_list argp;
    static char string[256];

    memset(string,0,sizeof(string));
    va_start(argp, str);
    if(vsprintf(string, str, argp) > 0)
        //HAL_UART_Transmit es bloqueante
        HAL_UART_Transmit(&huart, string, strlen(string), 100);
    va_end(argp);
}

void trace_read(){


}



void USARTx_IRQHandler(){
    HAL_UART_IRQHandler(&huart);
}