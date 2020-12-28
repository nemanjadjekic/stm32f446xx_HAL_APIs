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
void CAN1_Init(void);
void CAN1_Tx(void);
void CAN1_Rx(void);
void CAN_Filter_Config(void);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;

char *sent_data = "This application is running!\r\n";

int main(void)
{
    HAL_Init();
    SystemClockConfig(SYS_CLK_FREQ_50MHz);
    GPIO_Init();
    UART2_Init();
    CAN1_Init();
    CAN_Filter_Config();

    uint16_t sent_data_length = strlen(sent_data);
    if( HAL_UART_Transmit(&huart2, (uint8_t*) sent_data, sent_data_length, HAL_MAX_DELAY) != HAL_OK )
    {
        Error_Handler();
    }

    if( HAL_CAN_Start(&hcan1) != HAL_OK )
    {
        Error_Handler();
    }

    CAN1_Tx();
    CAN1_Rx();

    while(1);

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


void CAN1_Init(void)
{
    hcan1.Instance = CAN1;
    hcan1.Init.Mode = CAN_MODE_LOOPBACK;
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    /* CAN bit timing settings */
    hcan1.Init.Prescaler = 5;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

    if( HAL_CAN_Init(&hcan1) != HAL_OK )
    {
        Error_Handler();
    }
}


void CAN1_Tx(void)
{
    CAN_TxHeaderTypeDef TxHeader;

    uint32_t TxMailbox;

    char msg[50];
    uint8_t msgTx[5] = {'H', 'E', 'L', 'L', 'O'};

    TxHeader.DLC = 5;
    TxHeader.StdId = 0x65D;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;

    if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, msgTx, &TxMailbox) != HAL_OK )
    {
        Error_Handler();
    }

    while(HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox));

    sprintf(msg, "Message Transmitted!\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


void CAN1_Rx(void)
{
    CAN_RxHeaderTypeDef RxHeader;

    char msg[50];
    uint8_t msgRx[5];

    /* Waiting for at least one message in to the RX FIFO */
    while(!HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0));

    if( HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, msgRx) != HAL_OK )
    {
        Error_Handler();
    }

    sprintf(msg, "Message received! %s\r\n", msgRx);

    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}


void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef can1_filter_init;

    can1_filter_init.FilterActivation = ENABLE;
    can1_filter_init.FilterBank = 0;
    can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
    can1_filter_init.FilterIdHigh = 0x0000;
    can1_filter_init.FilterIdLow = 0x0000;
    can1_filter_init.FilterMaskIdHigh = 0x0000;
    can1_filter_init.FilterMaskIdLow = 0x0000;
    can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
    can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

    if( HAL_CAN_ConfigFilter(&hcan1, &can1_filter_init) != HAL_OK )
    {
        Error_Handler();
    }
}


void Error_Handler(void)
{
    while (1);
}
