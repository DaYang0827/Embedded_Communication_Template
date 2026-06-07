USART establish：
1. 开启 GPIO 时钟
2. 开启 USART 外设时钟
3. 配置 TX/RX 引脚为复用功能 AF7
4. 配置 USART 参数：波特率、8位数据、无校验、1停止位
5. 开启 USART 接收中断
6. 在中断回调函数里处理接收数据
7. 重新开启下一次接收

USART = GPIO复用 + USART参数 + NVIC中断 + 回调函数

在stm32f407中常见的USART搭配
USART1：PA9 / PA10，挂在 APB2
USART2：PA2 / PA3，挂在 APB1
USART3：PB10 / PB11，挂在 APB1
UART4 ：PA0 / PA1，挂在 APB1
UART5 ：PC12 / PD2，挂在 APB1
USART6：PC6 / PC7，挂在 APB2

USART.h
