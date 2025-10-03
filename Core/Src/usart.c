/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>


#ifdef DEBUG_MODE 
#define 	DBG_UART 			USART2
inline void dbg_putc (char c);
char dbg_buf[50];
#endif
/* USER CODE END 0 */

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 57600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/* USER CODE BEGIN 1 */
#ifdef DEBUG_MODE 
//-------------------------------передача символа по UART-----------------------------------//
inline void dbg_putc (char c)
{
  while(!(DBG_UART->SR & USART_SR_TC)) {}; 
  DBG_UART->DR = c; 
}

//-------------------------------передача строки по UART-----------------------------------//
//void dbg_putStr (const char *str, ...)
void dbg_putStr (const char *str)
{
	char c;

  //va_list args;
 // va_start(args, str);
 // vsnprintf( dbg_buf, sizeof( dbg_buf), str, args);

	while((c = *str++))
	{ dbg_putc(c);  }
 // va_end(args);
}
#endif

//-------------------------------передача символа по UART-----------------------------------//
int __io_putchar(int ch)
{
    char item = (char)ch;
    #ifdef USE_FREERTOS
        xQueueSendToBack(stdout_queue, &item, 0);
    #else
        (void)dbg_putc (item );
    #endif
    return ch;
}

//-------------------------------
int fputc(int ch, FILE *f)
{
    char item = (char)ch;
    #ifdef USE_FREERTOS
        xQueueSendToBack(stdout_queue, &item, 0);
    #else
        (void)dbg_putc (item );
    #endif
    return ch;
}
/* USER CODE END 1 */
