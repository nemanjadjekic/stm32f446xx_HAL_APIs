/*
 * main.h
 *
 *  Created on: Dec 24, 2020
 *      Author: nemanja
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stm32f4xx_hal.h"

#define TRUE  1
#define FALSE 0

#define LED1_PORT GPIOC
#define LED2_PORT GPIOC
#define LED3_PORT GPIOB
#define LED4_PORT GPIOC

#define LED1_PIN_NO GPIO_PIN_9
#define LED2_PIN_NO GPIO_PIN_8
#define LED3_PIN_NO GPIO_PIN_8
#define LED4_PIN_NO GPIO_PIN_6

#define SYS_CLK_FREQ_50MHz  50
#define SYS_CLK_FREQ_84MHz  84
#define SYS_CLK_FREQ_120MHz 120

#endif /* INC_MAIN_H_ */
