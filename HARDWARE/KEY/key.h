/*
 * @Description: key.h
 * @Author: TOTHTOT
 * @Date: 2022-07-15 11:09:27
 * @LastEditTime: 2022-07-16 11:39:04
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Learn\stm32\BASIC_PROJECT\STM32_F407ZGT6(FreeRTOS+HAL)\HARDWARE\KEY\key.h
 */
#ifndef __KEY_H
#define __KEY_H

#include "gpio.h"

#define KEY0 HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)
#define WK_UP HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define KEY0_PRES 	1
#define WKUP_PRES   2

uint8_t Key_Scan(uint8_t mode);

#endif
