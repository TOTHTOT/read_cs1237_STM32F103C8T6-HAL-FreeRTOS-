/*
 * @Description: ����1��ͷ�ļ�
 * @Author: TOTHTOT
 * @Date: 2022-11-12 01:09:18
 * @LastEditTime: 2022-11-12 01:18:16
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Learn\stm32\CubeMX_Project\STM32F103C8T6\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\USART\usart1.h
 */
#ifndef __USART1_H
#define __USART1_H

#include "main.h"

#define USART_REC_LEN 200
#define USART_SEND_LEN 200
#define EN_USART1_RX 1
#define RXBUFFERSIZE 1

extern uint8_t USART_RX_BUF[USART_REC_LEN];
extern uint16_t USART_RX_STA;
extern uint8_t aRxBuffer[RXBUFFERSIZE];

void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void u1_printf(char *format, ...);

// �Ƿ��� DEBUG ģʽ, �����ͻᴮ�������־
#define DEBUG_ON

#ifdef DEBUG_ON
#define INFO_PRINT(fmt, ...)                                                                  \
    do                                                                                        \
    {                                                                                         \
        printf("Info %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define WARNING_PRINT(fmt, ...)                                                                  \
    do                                                                                           \
    {                                                                                            \
        printf("Warning %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define DEBUG_PRINT(fmt, ...)                                                                  \
    do                                                                                         \
    {                                                                                          \
        printf("Debug %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ERROR_PRINT(fmt, ...)                                                                  \
    do                                                                                         \
    {                                                                                          \
        printf("Error %s,%s,%d: " fmt "", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#else

#define INFO_PRINT(fmt, ...) \
    do                       \
    {                        \
    } while (0)

#define WARNING_PRINT(fmt, ...) \
    do                          \
    {                           \
    } while (0)

#define DEBUG_PRINT(fmt, ...) \
    do                        \
    {                         \
    } while (0)

#define ERROR_PRINT(fmt, ...) \
    do                        \
    {                         \
    } while (0)

#endif  /* DEBUG_ON */


#endif
