/*
 * main.c
 *
 *  Created on: Dec 24, 2020
 *      Author: nemanja
 */

#include <stdio.h>
#include <string.h>
#include "main.h"

void SystemClockConfig(uint8_t clock_freq);
void Error_Handler(void);
void GPIO_Init(void);
void UART2_Init(void);
void TIMER6_Init(void);
void CAN1_Init(void);
void CAN1_Tx(void);
void CAN_Filter_Config(void);
void Send_Response(uint32_t StdId);
void LED_Manage_Output(uint8_t led_no);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;
CAN_RxHeaderTypeDef RxHeader;
TIM_HandleTypeDef htimer6;

uint8_t led_no = 0;
uint8_t req_counter = 0;

char *sent_data = "This application is running!\r\n";

int main(void)
{
    HAL_Init();
    SystemClockConfig(SYS_CLK_FREQ_50MHz);
    GPIO_Init();
    UART2_Init();
    TIMER6_Init();
    CAN1_Init();
    CAN_Filter_Config();

    if( HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK )
    {
        Error_Handler();
    }

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
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef ledgpio;
    ledgpio.Pin = GPIO_PIN_5;
    ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &ledgpio);

    ledgpio.Pin = GPIO_PIN_8;
    ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &ledgpio);

    ledgpio.Pin = GPIO_PIN_5;
    ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &ledgpio);

    ledgpio.Pin = GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9;
    ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &ledgpio);

    ledgpio.Pin = GPIO_PIN_13;
    ledgpio.Mode = GPIO_MODE_IT_FALLING;
    ledgpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC,&ledgpio);

    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
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


void TIMER6_Init(void)
{
    htimer6.Instance = TIM6;
    htimer6.Init.Prescaler = 4999;
    htimer6.Init.Period = 10000-1;

    if( HAL_TIM_Base_Init(&htimer6) != HAL_OK )
    {
        Error_Handler();
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    CAN_TxHeaderTypeDef TxHeader;

    uint8_t msg;
    uint32_t TxMailbox;

    if(req_counter == 4)
    {
        /* Node 1 is sending Remote frame to Node 2 */
        TxHeader.DLC = 2; /* Node 1 is demanding 2 bytes of reply */
        TxHeader.StdId = 0x651;
        TxHeader.IDE   = CAN_ID_STD;
        TxHeader.RTR = CAN_RTR_REMOTE;

        if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, &msg, &TxMailbox) != HAL_OK )
        {
            Error_Handler();
        }
        req_counter = 0;
    }
    else
    {
        CAN1_Tx();
        req_counter++;
    }
}


void CAN1_Init(void)
{
    hcan1.Instance = CAN1;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    /* CAN bit timing settings */
    hcan1.Init.Prescaler = 3;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

    if( HAL_CAN_Init(&hcan1) != HAL_OK )
    {
        Error_Handler();
    }
}


void CAN1_Tx(void)
{
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    uint8_t msg;

    TxHeader.DLC = 5;
    TxHeader.StdId = 0x65D;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;

    msg = ++led_no;

    if(led_no == 4)
    {
        led_no = 0;
    }

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    if( HAL_CAN_AddTxMessage(&hcan1, &TxHeader, &msg, &TxMailbox) != HAL_OK )
    {
        Error_Handler();
    }
}


void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef can1_filter_init;

    can1_filter_init.FilterActivation = ENABLE;
    can1_filter_init.FilterBank = 0;
    can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
    can1_filter_init.FilterIdHigh = 0x0000;
    can1_filter_init.FilterIdLow = 0x0000;
    can1_filter_init.FilterMaskIdHigh = 0X01C0;
    can1_filter_init.FilterMaskIdLow = 0x0000;
    can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
    can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

    if( HAL_CAN_ConfigFilter(&hcan1, &can1_filter_init) != HAL_OK )
    {
        Error_Handler();
    }
}


void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    char msg[50];

    sprintf(msg, "Message Transmitted:M0\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
    char msg[50];

    sprintf(msg, "Message Transmitted:M1\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
    char msg[50];

    sprintf(msg, "Message Transmitted:M2\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t msgRx[8];

    char msg[50];

    if( HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, msgRx) != HAL_OK )
    {
        Error_Handler();
    }

    if(RxHeader.StdId == 0x65D && RxHeader.RTR == 0)
    {
        /* Data Frame sent by Node 1 to Node 2 */
        LED_Manage_Output(msgRx[0]);
        sprintf(msg,"Message Received : #%x\r\n", msgRx[0]);
    }
    else if(RxHeader.StdId == 0x651 && RxHeader.RTR == 1)
    {
        /* Remote frame sent by Node 1 to Node 2 */
        Send_Response(RxHeader.StdId);
        return;
    }
    else if(RxHeader.StdId == 0x651 && RxHeader.RTR == 0)
    {
        //its a reply ( data frame) by n2 to n1
        /* Reply (Data Frame) by Node 2 to Node 1 */
        sprintf(msg,"Reply Received : %#X\r\n", msgRx[0] << 8 | msgRx[1]);
    }

     HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    char msg[50];

    sprintf(msg, "CAN Error Detected\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


void Send_Response(uint32_t StdId)
{
    CAN_TxHeaderTypeDef TxHeader;

    uint32_t TxMailbox;

    uint8_t response[2] = {0xAB, 0XCD};

    TxHeader.DLC = 2;
    TxHeader.StdId = StdId;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;

    if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,response,&TxMailbox) != HAL_OK )
    {
        Error_Handler();
    }
}


void LED_Manage_Output(uint8_t led_no)
{
    switch(led_no)
    {
        case 1:
            HAL_GPIO_WritePin(LED1_PORT,LED1_PIN_NO,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED2_PORT,LED2_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED3_PORT,LED3_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED4_PORT,LED4_PIN_NO,GPIO_PIN_RESET);
            break;

        case 2:
            HAL_GPIO_WritePin(LED1_PORT,LED1_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_PORT,LED2_PIN_NO,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED3_PORT,LED3_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED4_PORT,LED4_PIN_NO,GPIO_PIN_RESET);
            break;

        case 3:
            HAL_GPIO_WritePin(LED1_PORT,LED1_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_PORT,LED2_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED3_PORT,LED3_PIN_NO,GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED4_PORT,LED4_PIN_NO,GPIO_PIN_RESET);
            break;

        case 4 :
            HAL_GPIO_WritePin(LED1_PORT,LED1_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_PORT,LED2_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED3_PORT,LED3_PIN_NO,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED4_PORT,LED4_PIN_NO,GPIO_PIN_SET);
            break;
    }
}


void Error_Handler(void)
{
    while (1);
}
