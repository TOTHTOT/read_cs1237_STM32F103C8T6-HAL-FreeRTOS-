/*
 * @Description: 卡尔曼滤波代码
 * @Author: TOTHTOT
 * @Date: 2023-03-28 10:14:16
 * @LastEditTime: 2023-03-28 10:33:27
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Work\MCU\stm32\read_cs1237_STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\KALMAN\kalman.h
 */
#ifndef __KALMAN_H__
#define __KALMAN_H__

#include "main.h"

typedef struct
{
    float Last_P; // 上次估算协方差 不可以为0 ! ! ! ! !
    float Now_P;  // 当前估算协方差
    float out;    // 卡尔曼滤波器输出
    float Kg;     // 卡尔曼增益
    float Q;      // 过程噪声协方差
    float R;      // 观测噪声协方差
} kalman;

extern kalman g_kfp_st;

/* 外部调用函数 */
void kalman_init(kalman *kfp);
float kalman_filter(kalman *kfp, float input);

#endif /* __KALMAN_H__ */
