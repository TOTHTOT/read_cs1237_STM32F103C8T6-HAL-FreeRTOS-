/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>
#include "usart1.h"
#include "cs1237.h"
#include "kalman.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LED_TASK */
osThreadId_t LED_TASKHandle;
const osThreadAttr_t LED_TASK_attributes = {
  .name = "LED_TASK",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for KEY_TASK */
osThreadId_t KEY_TASKHandle;
const osThreadAttr_t KEY_TASK_attributes = {
  .name = "KEY_TASK",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for THREAD_USART */
osThreadId_t THREAD_USARTHandle;
const osThreadAttr_t THREAD_USART_attributes = {
  .name = "THREAD_USART",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh6,
};
/* Definitions for Usart1_Receive_BinSemaphore */
osSemaphoreId_t Usart1_Receive_BinSemaphoreHandle;
const osSemaphoreAttr_t Usart1_Receive_BinSemaphore_attributes = {
  .name = "Usart1_Receive_BinSemaphore"
};
/* Definitions for usart_event */
osEventFlagsId_t usart_eventHandle;
const osEventFlagsAttr_t usart_event_attributes = {
  .name = "usart_event"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void led_task(void *argument);
void key_task(void *argument);
void thread_usart_fun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of Usart1_Receive_BinSemaphore */
  Usart1_Receive_BinSemaphoreHandle = osSemaphoreNew(1, 1, &Usart1_Receive_BinSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    xSemaphoreTake(Usart1_Receive_BinSemaphoreHandle, portMAX_DELAY);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LED_TASK */
  LED_TASKHandle = osThreadNew(led_task, NULL, &LED_TASK_attributes);

  /* creation of KEY_TASK */
  KEY_TASKHandle = osThreadNew(key_task, NULL, &KEY_TASK_attributes);

  /* creation of THREAD_USART */
  THREAD_USARTHandle = osThreadNew(thread_usart_fun, NULL, &THREAD_USART_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of usart_event */
  usart_eventHandle = osEventFlagsNew(&usart_event_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    kalman_init(&g_kfp_st);
    // 这个任务进行初始化, 在完成后删除自身

    // 设置cs1237参数
    cs1237_init(&g_cs1237_device_st, DEV_FREQUENCY_640, DEV_PGA_1, DEV_CH_A);
    vTaskDelete(NULL);
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_led_task */
/**
 * @brief Function implementing the LED_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_led_task */
void led_task(void *argument)
{
  /* USER CODE BEGIN led_task */
    /* Infinite loop */
    for (;;)
    {
        // INFO_PRINT("led\r\n");
        LED0_TOGGLE;

        // 输出计算的数据, 前一个原始值,后一个卡尔曼滤波值
        printf("%d %f\r\n", g_cs1237_device_st.get_adc_data(&g_cs1237_device_st), kalman_filter(&g_kfp_st, (float)g_cs1237_device_st.adc_calculate_deal_data));
        
        delay_ms(1);
    }
  /* USER CODE END led_task */
}

/* USER CODE BEGIN Header_key_task */
/**
 * @brief Function implementing the KEY_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_key_task */
void key_task(void *argument)
{
  /* USER CODE BEGIN key_task */
    /* Infinite loop */
    for (;;)
    {

        osDelay(1);
    }
  /* USER CODE END key_task */
}

/* USER CODE BEGIN Header_thread_usart_fun */
/**
* @brief Function implementing the THREAD_USART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread_usart_fun */
void thread_usart_fun(void *argument)
{
  /* USER CODE BEGIN thread_usart_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END thread_usart_fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

