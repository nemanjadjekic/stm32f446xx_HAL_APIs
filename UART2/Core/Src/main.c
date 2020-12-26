/*
 * main.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include <string.h>
#include "main.h"
#include "stm32f4xx_hal.h"

void SystemClockConfig(void);
void UART2_Init(void);
void Error_Handler(void);
uint8_t convert_to_capital(uint8_t data);

UART_HandleTypeDef huart2;

char *sent_data = "This application is running!\r\n";

int main(void)
{
    HAL_Init();
    SystemClockConfig();
    UART2_Init();

    uint16_t sent_data_length = strlen(sent_data);
    if( HAL_UART_Transmit(&huart2, (uint8_t*) sent_data, sent_data_length, HAL_MAX_DELAY) != HAL_OK )
    {
        Error_Handler();
    }


    uint8_t rcvd_data;
    uint8_t data_buffer[100];
    uint32_t count = 0;

    while(1)
    {
        HAL_UART_Receive(&huart2, &rcvd_data, 1, HAL_MAX_DELAY);
        if(rcvd_data == '\r')
        {
            break;
        }
        else
        {
            data_buffer[count++] = rcvd_data;
        }
    }
    data_buffer[count++]= '\r';

    /* Capitalize first character of every word in a string */
    data_buffer[0] = convert_to_capital(data_buffer[0]);
    for(int i = 0; i < count; i++)
    {
        if(data_buffer[i] == ' ')
        {
            ++i;
            data_buffer[i] = convert_to_capital(data_buffer[i]);
        }
    }

    HAL_UART_Transmit(&huart2, data_buffer, count, HAL_MAX_DELAY);

    return 0;
}


void SystemClockConfig(void)
{

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


uint8_t convert_to_capital(uint8_t data)
{
    if(data >= 'a' && data <= 'z')
    {
        data -= ('a' - 'A');
    }

    return data;
}


void Error_Handler(void)
{
    while (1);
}
