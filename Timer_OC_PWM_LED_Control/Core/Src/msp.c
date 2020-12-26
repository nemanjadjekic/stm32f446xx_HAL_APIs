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


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef tim2OC_ch_gpios;

    /* Enable clock for TIM2 */
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configuring a GPIOs to behave as TIM2 Channel 1/2/3/4 */
    /**
     * PAO  -->  TIM2_CH1
     * PA1  -->  TIM2_CH2
     * PB10 -->  TIM2_CH3
     * PB2  -->  TIM2_CH4
     */
    tim2OC_ch_gpios.Pin = GPIO_PIN_5;
    tim2OC_ch_gpios.Mode = GPIO_MODE_AF_PP;
    tim2OC_ch_gpios.Pull = GPIO_NOPULL;
    tim2OC_ch_gpios.Speed = GPIO_SPEED_FREQ_LOW;
    tim2OC_ch_gpios.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &tim2OC_ch_gpios);

    /* NVIC Settings */
    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
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
