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


void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_CAN1_CLK_ENABLE();
    //__HAL_RCC_GPIOA_CLK_ENABLE();

    /*
     * CAN1 GPIO Configuration
     * PA11 --> CAN1_RX
     * PA12 --> CAN1_TX
     */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 15, 0);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 15, 0);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 15, 0);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 15, 0);

    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_uart;

    /* UART2 Low Level Initialization */
    /* Enable the clock of UART2 and GPIOA peripherals */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Pin MUXing configuration */
    gpio_uart.Pin = GPIO_PIN_2;
    gpio_uart.Mode = GPIO_MODE_AF_PP;
    gpio_uart.Pull = GPIO_PULLUP;
    gpio_uart.Alternate = GPIO_AF7_USART2;
    gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &gpio_uart);

    /* UART2_Rx */
    gpio_uart.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &gpio_uart);

    /* IRQ enabling and setting up the priority (NVIC Settings) */
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}
