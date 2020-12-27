/*
 * it.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include "main.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

