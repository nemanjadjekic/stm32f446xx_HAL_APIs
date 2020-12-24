/*
 * it.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include <stdio.h>
#include "main.h"

extern UART_HandleTypeDef huart2;

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}
