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
void SystemClockConfig_HSE(uint8_t clock_freq);
void Error_Handler(void);
void GPIO_Init(void);
void UART2_Init(void);
void TIMER2_Init(void);
void TIMER6_Init(void);
void LSE_Configuration(void);

TIM_HandleTypeDef htimer2;
TIM_HandleTypeDef htimer6;
UART_HandleTypeDef huart2;

uint32_t input_captures[2] = {0};
uint8_t count = 1;
volatile uint8_t is_capture_done = FALSE;

int main(void)
{
    uint32_t capture_diff = 0;
    double timer2_cnt_freq = 0;
    double timer2_cnt_resoultion = 0;
    double user_signal_time_period = 0;
    double user_signal_freq = 0;
    char usr_msg[100];

    HAL_Init();
    //SystemClockConfig(SYS_CLK_FREQ_50MHz);
    SystemClockConfig_HSE(SYS_CLK_FREQ_50MHz);
    GPIO_Init();
    UART2_Init();
    TIMER6_Init();
    TIMER2_Init();
    LSE_Configuration();

    char *sent_data = "This application is running!\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)sent_data, strlen(sent_data), HAL_MAX_DELAY);

    HAL_TIM_Base_Start_IT(&htimer6);
    HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);

    while(1)
    {
        if(is_capture_done)
        {
            if(input_captures[1] > input_captures[0])
            {
                capture_diff = input_captures[1] - input_captures[0];
            }
            else
            {
                capture_diff = (0xFFFFFFFF - input_captures[0]) + input_captures[1];
            }

            timer2_cnt_freq = (HAL_RCC_GetPCLK1Freq() * 2) / (htimer2.Init.Prescaler + 1);
            timer2_cnt_resoultion = 1/timer2_cnt_freq;
            user_signal_time_period = capture_diff * timer2_cnt_resoultion;
            user_signal_freq = 1/user_signal_time_period;

            sprintf(usr_msg, "Frequency of the signal applied = %.2f Hz\r\n", user_signal_freq);
            HAL_UART_Transmit(&huart2, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);

            is_capture_done = FALSE;
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


void SystemClockConfig_HSE(uint8_t clock_freq)
{
    RCC_OscInitTypeDef osc_init;
    RCC_ClkInitTypeDef clk_init;

    uint32_t FLatency = 0;

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI;
    osc_init.HSIState = RCC_HSI_ON;
    osc_init.HSEState = RCC_HSE_ON;
    osc_init.LSEState = RCC_LSE_ON;
    osc_init.PLL.PLLState = RCC_PLL_ON;
    osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    switch(clock_freq)
    {
        case SYS_CLK_FREQ_50MHz:
        {
            osc_init.PLL.PLLM = 4;
            osc_init.PLL.PLLN = 50;
            osc_init.PLL.PLLP = 2;
            osc_init.PLL.PLLQ = 2;
            osc_init.PLL.PLLR = 2;

            clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
            clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
            clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

            FLatency = FLASH_ACR_LATENCY_1WS;

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

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
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
    TIM_IC_InitTypeDef timer2IC_Config;

    htimer2.Instance = TIM2;
    htimer2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimer2.Init.Period = 0xFFFFFFFF;
    htimer2.Init.Prescaler = 1;
    if( HAL_TIM_IC_Init(&htimer2) != HAL_OK )
    {
        Error_Handler();
    }

    timer2IC_Config.ICFilter = 0;
    timer2IC_Config.ICPolarity = TIM_ICPOLARITY_RISING;
    timer2IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
    timer2IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
    if( HAL_TIM_IC_ConfigChannel(&htimer2, &timer2IC_Config, TIM_CHANNEL_1) != HAL_OK )
    {
        Error_Handler();
    }
}


void TIMER6_Init(void)
{
    htimer6.Instance = TIM6;
    htimer6.Init.Prescaler = 9;
    htimer6.Init.Period = 50-1;
    if( HAL_TIM_Base_Init(&htimer6) != HAL_OK )
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


void LSE_Configuration(void)
{
#if 0
    RCC_OscInitTypeDef osc_init;

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    osc_init.LSEState = RCC_LSE_ON;
    if( HAL_RCC_OscConfig(&osc_init) != HAL_OK )
    {
        Error_Handler();
    }
#endif

   HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(!is_capture_done)
    {
        if(count == 1)
        {
            input_captures[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
            count++;
        }
        else if(count == 2)
        {
            input_captures[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
            count = 1;
            is_capture_done = TRUE;
        }
    }
}


void Error_Handler(void)
{
    while (1);
}
