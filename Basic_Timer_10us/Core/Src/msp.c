/*
 * msp.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
    /* Low Level Processor specific inits. */
    /* Setting up the priority of the ARM Cortex-Mx processor */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Enable the required system exceptions of the ARM Cortex-Mx processor */
    SCB->SHCSR |= 0x7 << 16;

    /* Configure the priority for the system exceptions */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{
    /* Enable clock for TIM6 peripheral */
    __HAL_RCC_TIM6_CLK_ENABLE();

    /* Enable IRQ of TIM6 */
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    /* Setup the priority for TIM6_DAC_IRQn */
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
}
