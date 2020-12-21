/*
 * main.c
 *
 *  Created on: Dec 21, 2020
 *      Author: nemanja
 */

#include "main.h"
#include "stm32f4xx_hal.h"

void SystemClockConfig(void);

int main(void)
{
	HAL_Init();
	SystemClockConfig();

	return 0;
}


void SystemClockConfig(void)
{

}
