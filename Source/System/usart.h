#ifndef __USART_H
#define __USART_H

#define USART1_GPIO GPIOA
#define USART1_TX_PIN GPIO_Pin_9
#define USART1_RX_PIN GPIO_Pin_10

#define USART2_GPIO GPIOA
#define USART2_TX_PIN GPIO_Pin_2
#define USART2_RX_PIN GPIO_Pin_3

#define USART1_TX_BUFFER_SIZE 1024 // USART1 输出缓冲区大小
// USART1 会把收到的数据直接发送出去，所以没有设置输入缓冲区

#define USART2_TX_BUFFER_SIZE 1024 // USART2 输出缓冲区大小
#define USART2_RX_BUFFER_SIZE 1024 // USART2 输入缓冲区大小

/* 向外暴露缓冲区信息 */

extern char USART1_Rx_Buffer[USART2_RX_BUFFER_SIZE];
extern uint8_t USART1_Rx_Length;

extern char USART2_Rx_Buffer[USART2_RX_BUFFER_SIZE];
extern uint8_t USART2_Rx_Length;

extern char USART2_Tx_Buffer[USART2_TX_BUFFER_SIZE];
extern uint8_t USART2_Tx_Length;

/* ----- USART1 用于重定向 printf 函数 ----- */

void USART1_Init(void);
void USART1_Printf(char *fmt, ...);

/* USART2 用于连接 WiFi 模块，作为向 esp8266(esp-01s) 发送 AT 指令的串口。
   同时，MQTT 也在此基础之上通过 USART2 串口，由 WiFi 模块发送 MQTT 报文 */

void USART2_Init(void);
void USART2_Printf(char *fmt, ...);

/* 💡USART1 和 USART2 的中断处理函数位于 usart.c */

/* ----------------- USART 基础函数 ----------------- */

void USART_SendByte(USART_TypeDef *USARTx, uint16_t Data);
void USART_SendString(USART_TypeDef *USARTx, char *str);
uint8_t USART_ReceieByte(USART_TypeDef *USARTx);

#endif
