/*
 * @Description: 串口1 的业务逻辑代码
 * @Author: TOTHTOT
 * @Date: 2022-11-12 01:09:09
 * @LastEditTime: 2022-11-12 01:20:45
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Learn\stm32\CubeMX_Project\STM32F103C8T6\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\USART\usart1.c
 */
#include "main.h"
#include "usart.h"
#include "usart1.h"
#include <cmsis_os.h>
#include <string.h>
#include "semphr.h"
#include "stdarg.h"
#include "stdio.h"

uint8_t USART_RX_BUF[USART_REC_LEN];  //接收缓冲,最大USART_REC_LEN个字节.
uint8_t USART_TX_BUF[USART_SEND_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
// bit15，	接收完成标志
// bit14，	接收到0x0d
// bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA = 0; //接收状态标记

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

/**
 * @name: fputc
 * @msg: 重写 printf
 * @param {int} ch
 * @param {FILE} *f
 * @return {*}
 */
int fputc(int ch, FILE *f)
{
    // while ((USART1->SR & 0X40) == 0)
    //     ;
    // USART1->DR = (unsigned char)ch;
    uint8_t ch1[1] = {ch};
    HAL_UART_Transmit(&huart1, ch1, 1, 0XFFFF);
    return ch;
}
extern osSemaphoreId Usart1_Receive_BinSemaphoreHandle;
/**
 * @name: HAL_UART_RxCpltCallback
 * @msg: usart1's callback function
 * @note:
 *          Receiving status
            bit15， 	 Receiving completion flag
            bit14， 	 0x0D received
            bit13~0，  Number of valid bytes received
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t pxHigherPriorityTaskWoken;
    BaseType_t err = pdFALSE;
    if (huart->Instance == USART1) // is usart1?
    {
        if ((USART_RX_STA & 0x8000) == 0) // receive success?
        {
            if (USART_RX_STA & 0x4000) // receive 0x0d
            {
                if (USART_RX_BUF[0] != 0x0a)
                    USART_RX_STA = 0; // receive error restart
                else
                {
                    USART_RX_STA |= 0x8000; // receive compelete
                    err = xSemaphoreGiveFromISR(Usart1_Receive_BinSemaphoreHandle, &pxHigherPriorityTaskWoken);
                    if (err == pdFALSE)
                    {
                        printf("USART1 give semaphore failed\r\n");
                    }
                }
            }
            else // steal not receive 0x0d
            {
                if (USART_RX_BUF[0] == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = USART_RX_BUF[0];
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; // Receiving data error, restart receiving
                }
            }
        }
    }
}
// 发送完成标志
uint8_t USART_DMA_TX_OVER = 1;
/**
 * @name: HAL_UART_TxCpltCallback
 * @msg: 发送完成中断
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        USART_DMA_TX_OVER = 1;
        __HAL_DMA_DISABLE(&hdma_usart1_tx);
    }
}

/**
 * @name: USAR_UART_IDLECallback
 * @msg: 串口空闲中断回调函数,接收完成时执行.次函数用来处理业务逻辑
 * 不知道为什么接收到的第一次数据是无效的
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    BaseType_t pxHigherPriorityTaskWoken;
    BaseType_t err = pdFALSE;
    // 停止本次DMA传输
    HAL_UART_DMAStop(&huart1);

    // 计算接收到的数据长度
    // uint8_t data_length = USART_REC_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

    err = xSemaphoreGiveFromISR(Usart1_Receive_BinSemaphoreHandle, &pxHigherPriorityTaskWoken);
    if (err == pdFALSE)
    {
        printf("USART1 give semaphore failed\r\n");
    }

    // 重启开始DMA传输 每次255字节数据
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)USART_RX_BUF, USART_REC_LEN);
}
/**
 * @name: u1_printf
 * @msg: 串口1的发送函数使用DMA方式
 * @param {char} *format
 * @return {*}
 */
void u1_printf(char *format, ...)
{
    va_list ap;
    while (!USART_DMA_TX_OVER)
        ;
    va_start(ap, format);
    vsprintf((char *)USART_TX_BUF, format, ap);
    va_end(ap);
    HAL_UART_Transmit_DMA(&huart1, USART_TX_BUF, strlen((char *)USART_TX_BUF));
    USART_DMA_TX_OVER = 0;
    __HAL_DMA_ENABLE(&hdma_usart1_tx);
}

/**
 * @name: USER_UART_IRQHandler
 * @msg: 串口1空闲中断回调函数, 添加在串口的中断中 USART1_IRQHandler
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{ // 判断是否是串口1
    if (USART1 == huart1.Instance)
    { // 判断是否是空闲中断
        if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
        { // 清除空闲中断标志（否则会一直不断进入中断）
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);
            // printf("\r\nUART1 Idle IQR Detected\r\n");
            //  调用中断处理函数
            USAR_UART_IDLECallback(huart);
        }
    }
}
