/*
 * @Description: 
 * @Author: TOTHTOT
 * @Date: 2022-07-15 11:08:54
 * @LastEditTime: 2022-07-15 13:47:44
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Learn\stm32\BASIC_PROJECT\STM32_F407ZGT6(FreeRTOS+HAL)\HARDWARE\LED\led.h
 */
#ifndef __LED_H
#define __LED_H

#include "gpio.h"

#define LED0_TOGGLE HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin)

#define LED0_OFF HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET)
#define LED0_ON HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET)


#endif



