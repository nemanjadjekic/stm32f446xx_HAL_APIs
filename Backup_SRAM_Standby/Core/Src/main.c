/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"

void SystemClock_Config_HSE(uint8_t clock_freq);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

UART_HandleTypeDef huart2;

void printmsg(char *format, ...)
{
    char str[80];

    /* Extract the the argument list using VA APIs */
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), HAL_MAX_DELAY);
    va_end(args);
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint32_t *pBackupSRAMbase = 0;

    char write_buf[] = "Hello";

    HAL_Init();

    MX_GPIO_Init();

    SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

    MX_USART2_UART_Init();

    /* Turn on the clock in RCC register for SRAM backup */
    __HAL_RCC_BKPSRAM_CLK_ENABLE();

    /* Enable Write access to the backup domain */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    pBackupSRAMbase = (uint32_t*) BKPSRAM_BASE;

    /* Enable clock for PWR Controller block */
    __HAL_RCC_PWR_CLK_ENABLE();

    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
    {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

        printmsg("Woke up from the standby mode\r\n");
        uint8_t data = (uint8_t) *pBackupSRAMbase;
        if (data != 'H')
        {
            printmsg("Backup SRAM data is lost\r\n");
        } else
        {
            printmsg("Backup SRAM data is safe \r\n");
        }

    } else
    {
        for (uint32_t i = 0; i < strlen(write_buf) + 1; i++)
        {
            *(pBackupSRAMbase + i) = write_buf[i];
        }

    }

    printmsg("Press the user button to enter standby mode\r\n");
    while (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) != GPIO_PIN_RESET)
    {
    }

    /* When user pushes the user button this message is shown */
    printmsg("Going to Standby mode\r\n");

    /* Enable the WakeUp PIN 1 in pwr_csr register */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    /* Enable backup voltage reg. */
    HAL_PWREx_EnableBkUpReg();

    HAL_PWR_EnterSTANDBYMode();

    while (1)
        ;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config_HSE(uint8_t clock_freq)
{
    RCC_OscInitTypeDef Osc_Init;
    RCC_ClkInitTypeDef Clock_Init;
    uint8_t flash_latency = 0;

    Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    Osc_Init.HSEState = RCC_HSE_ON;
    Osc_Init.PLL.PLLState = RCC_PLL_ON;
    Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    switch (clock_freq)
    {
    case SYS_CLOCK_FREQ_50_MHZ:
        Osc_Init.PLL.PLLM = 4;
        Osc_Init.PLL.PLLN = 50;
        Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
        Osc_Init.PLL.PLLQ = 2;
        Osc_Init.PLL.PLLR = 2;
        Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
        Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
        Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
        flash_latency = 1;
        break;

    case SYS_CLOCK_FREQ_84_MHZ:
        Osc_Init.PLL.PLLM = 4;
        Osc_Init.PLL.PLLN = 84;
        Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
        Osc_Init.PLL.PLLQ = 2;
        Osc_Init.PLL.PLLR = 2;
        Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
        Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
        Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
        flash_latency = 2;
        break;

    case SYS_CLOCK_FREQ_120_MHZ:
        Osc_Init.PLL.PLLM = 4;
        Osc_Init.PLL.PLLN = 120;
        Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
        Osc_Init.PLL.PLLQ = 2;
        Osc_Init.PLL.PLLR = 2;
        Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
        Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
        Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
        flash_latency = 3;
        break;

    default:
        return;
    }

    if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_RCC_ClockConfig(&Clock_Init, flash_latency) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the systick timer interrupt frequency (for every 1 ms) */
    uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
    HAL_SYSTICK_Config(hclk_freq / 1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    /* Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
        ;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
