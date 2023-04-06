/*
 * @Description: key.h
 * @Author: TOTHTOT
 * @Date: 2022-07-15 11:09:12
 * @LastEditTime: 2022-07-15 11:38:37
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Learn\stm32\BASIC_PROJECT\STM32_F407ZGT6(FreeRTOS+HAL)\HARDWARE\KEY\key.c
 */
#include "key.h"
#include "delay.h"

uint8_t Key_Scan(uint8_t mode)
{
    static uint8_t key_up = 1; //按键松开标志
    if (mode == 1)
        key_up = 1; //支持连按
    if (key_up && (KEY0 == 1 || WK_UP == 1))
    {  
        delay_xms(10);
        key_up = 0;
        if (KEY0 == 1)
            return KEY0_PRES;
        else if (WK_UP == 1) 
            return WKUP_PRES;
    }
    else if (KEY0 == 0 && WK_UP == 0)
        key_up = 1;
    return 0; //无按键按下
}
