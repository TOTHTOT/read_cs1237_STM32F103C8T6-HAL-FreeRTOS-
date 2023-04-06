在移植串口时要在"usart.c"的"MX_USART1_UART_Init"函数中开启空闲中断, 加入以下代码
"__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能IDLE中断
    HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_RX_BUF, RXBUFFERSIZE);
    //该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
"
在"USART1_IRQHandler"中断函数中添加"USER_UART_IRQHandler(&huart1);"