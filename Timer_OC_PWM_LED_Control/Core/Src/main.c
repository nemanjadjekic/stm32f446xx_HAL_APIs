/*
 * main.c
 *
 *  Created on: Dec 24, 2020
 *      Author: nemanja
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx_hal.h"

void SystemClockConfig(uint8_t clock_freq);
void Error_Handler(void);
void GPIO_Init(void);
void UART2_Init(void);
void TIMER2_Init(void);

TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart2;


int main(void)
{
    uint16_t brightness = 0;

    HAL_Init();
    SystemClockConfig(SYS_CLK_FREQ_50MHz);
    GPIO_Init();
    UART2_Init();
    TIMER2_Init();

    if( HAL_TIM_PWM_Start(&htimer2, TIM_CHANNEL_1) != HAL_OK )
    {
        Error_Handler();
    }

    while(1)
    {
        while(brightness < htimer2.Init.Period)
        {
            brightness += 10;
            __HAL_TIM_SET_COMPARE(&htimer2, TIM_CHANNEL_1, brightness);
            HAL_Delay(1);
        }

        while(brightness > 0)
        {
            brightness -= 10;
            __HAL_TIM_SET_COMPARE(&htimer2, TIM_CHANNEL_1, brightness);
            HAL_Delay(1);
        }
    }

    return 0;
}


void SystemClockConfig(uint8_t clock_freq)
{
    RCC_OscInitTypeDef osc_init;
    RCC_ClkInitTypeDef clk_init;

    uint32_t FLatency = 0;

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
    osc_init.HSIState = RCC_HSI_ON;
    osc_init.LSEState = RCC_LSE_ON;
    osc_init.HSICalibrationValue = 16;
    osc_init.PLL.PLLState = RCC_PLL_ON;
    osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;

    switch(clock_freq)
    {
        case SYS_CLK_FREQ_50MHz:
        {
            osc_init.PLL.PLLM = 16;
            osc_init.PLL.PLLN = 100;
            osc_init.PLL.PLLP = 2;
            osc_init.PLL.PLLQ = 2;
            osc_init.PLL.PLLR = 2;

            clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
            clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
            clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

            FLatency = FLASH_ACR_LATENCY_2WS;

            break;
        }

        case SYS_CLK_FREQ_84MHz:
        {
            osc_init.PLL.PLLM = 16;
            osc_init.PLL.PLLN = 168;
            osc_init.PLL.PLLP = 2;
            osc_init.PLL.PLLQ = 2;
            osc_init.PLL.PLLR = 2;

            clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
            clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
            clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

            FLatency = FLASH_ACR_LATENCY_2WS;

            break;
        }

        case SYS_CLK_FREQ_120MHz:
        {
            osc_init.PLL.PLLM = 16;
            osc_init.PLL.PLLN = 240;
            osc_init.PLL.PLLP = 2;
            osc_init.PLL.PLLQ = 2;
            osc_init.PLL.PLLR = 2;

            clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
            clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
            clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

            FLatency = FLASH_ACR_LATENCY_3WS;

            break;
        }

        default:
            return;
    }

    if( HAL_RCC_OscConfig(&osc_init) != HAL_OK )
    {
        Error_Handler();
    }

    if( HAL_RCC_ClockConfig(&clk_init, FLatency) != HAL_OK )
    {
        Error_Handler();
    }

    /* SysTick Configuration */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}


void GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef ledgpio;
    ledgpio.Pin = GPIO_PIN_5;
    ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &ledgpio);
}


void TIMER2_Init(void)
{
    TIM_OC_InitTypeDef tim2PWM_Config;

    htimer2.Instance = TIM2;
    htimer2.Init.Period = 10000-1;
    htimer2.Init.Prescaler = 4;
    if( HAL_TIM_PWM_Init(&htimer2) != HAL_OK )
    {
        Error_Handler();
    }

    memset(&tim2PWM_Config, 0, sizeof(tim2PWM_Config));

    tim2PWM_Config.OCMode = TIM_OCMODE_PWM1;
    tim2PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
    tim2PWM_Config.Pulse = 0;
    if( HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2PWM_Config, TIM_CHANNEL_1) != HAL_OK )
    {
        Error_Handler();
    }
}


void UART2_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.Parity = UART_PARITY_NONE;

    if(HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}


void Error_Handler(void)
{
    while (1);
}
