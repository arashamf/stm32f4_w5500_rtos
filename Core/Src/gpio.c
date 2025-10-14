/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"
#include "stm32f4xx_ll_exti.h"
/* USER CODE BEGIN 0 */
leds_t leds[number] = {0};
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_Green_Pin|LED_Orange_Pin|LED_Red_Pin|LED_Blue_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);


  /*Configure GPIO pin : RESET_Pin */
  GPIO_InitStruct.Pin = RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Green_Pin LED_Orange_Pin LED_Red_Pin LED_Blue_Pin */
  GPIO_InitStruct.Pin = LED_Green_Pin|LED_Orange_Pin|LED_Red_Pin|LED_Blue_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_Pin */
  GPIO_InitStruct.Pin = INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 2 */
//-----------------------------------------
void leds_init (void)
{
    leds[green].port = LED_Green_GPIO_Port;
    leds[green].pin = LED_Green_Pin;   
    leds[orange].port = LED_Orange_GPIO_Port;
    leds[orange].pin = LED_Orange_Pin;
    leds[red].port = LED_Red_GPIO_Port;
    leds[red].pin = LED_Red_Pin;
    leds[blue].port = LED_Blue_GPIO_Port;
    leds[blue].pin = LED_Blue_Pin;

}

//-----------------------------------------
void hard_reset (uint32_t delay)
{
    W5500_RST(OFF);
    HAL_Delay(delay);
    W5500_RST(ON);
    HAL_Delay(delay);
}

//-----------------------------------------
void control_LED (uint8_t number_leds, uint8_t comm)
{
  led_ctl(comm, leds[number_leds].port, leds[number_leds].pin);
}

//-----------------------------------------
void W5500_Select (void)
{
  LL_GPIO_ResetOutputPin(CS_GPIO_Port, CS_Pin);
}

//-----------------------------------------
void W5500_Unselect (void)
{
  LL_GPIO_SetOutputPin(CS_GPIO_Port, CS_Pin);
}
/* USER CODE END 2 */
