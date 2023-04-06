/*
 * @Description: ����1 ��ҵ���߼�����
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

uint8_t USART_RX_BUF[USART_REC_LEN];  //���ջ���,���USART_REC_LEN���ֽ�.
uint8_t USART_TX_BUF[USART_SEND_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
uint16_t USART_RX_STA = 0; //����״̬���

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

/**
 * @name: fputc
 * @msg: ��д printf
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
            bit15�� 	 Receiving completion flag
            bit14�� 	 0x0D received
            bit13~0��  Number of valid bytes received
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
// ������ɱ�־
uint8_t USART_DMA_TX_OVER = 1;
/**
 * @name: HAL_UART_TxCpltCallback
 * @msg: ��������ж�
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
 * @msg: ���ڿ����жϻص�����,�������ʱִ��.�κ�����������ҵ���߼�
 * ��֪��Ϊʲô���յ��ĵ�һ����������Ч��
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    BaseType_t pxHigherPriorityTaskWoken;
    BaseType_t err = pdFALSE;
    // ֹͣ����DMA����
    HAL_UART_DMAStop(&huart1);

    // ������յ������ݳ���
    // uint8_t data_length = USART_REC_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

    err = xSemaphoreGiveFromISR(Usart1_Receive_BinSemaphoreHandle, &pxHigherPriorityTaskWoken);
    if (err == pdFALSE)
    {
        printf("USART1 give semaphore failed\r\n");
    }

    // ������ʼDMA���� ÿ��255�ֽ�����
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)USART_RX_BUF, USART_REC_LEN);
}
/**
 * @name: u1_printf
 * @msg: ����1�ķ��ͺ���ʹ��DMA��ʽ
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
 * @msg: ����1�����жϻص�����, ����ڴ��ڵ��ж��� USART1_IRQHandler
 * @param {UART_HandleTypeDef} *huart
 * @return {*}
 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{ // �ж��Ƿ��Ǵ���1
    if (USART1 == huart1.Instance)
    { // �ж��Ƿ��ǿ����ж�
        if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
        { // ��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);
            // printf("\r\nUART1 Idle IQR Detected\r\n");
            //  �����жϴ�����
            USAR_UART_IDLECallback(huart);
        }
    }
}
