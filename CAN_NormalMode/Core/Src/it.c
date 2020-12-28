/*
 * it.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htimer6;

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}


void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}


void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}


void CAN1_RX1_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}


void CAN1_SCE_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}


void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htimer6);
}


void EXTI15_10_IRQHandler(void)
{
    HAL_TIM_Base_Start_IT(&htimer6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
