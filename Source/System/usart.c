#include "sys.h"

/* ❗USART1_Printf() 需要引入以下三个头文件：
    #include "stdio.h"
    #include "stdarg.h"
    #include "string.h"
*/

/* USART1 输出缓冲区 */
__align(8) char USART1_Tx_Buffer[USART1_TX_BUFFER_SIZE];

/* USART2 输入缓冲区 */

char USART2_Rx_Buffer[USART2_RX_BUFFER_SIZE]; // USART2 输入缓冲区
uint8_t USART2_Rx_Length = 0;                 // USART2 输入缓冲区当前缓冲规模

/* USART2 输出缓冲区 */
__align(8) char USART2_Tx_Buffer[USART2_TX_BUFFER_SIZE]; // USART2 输出缓冲区
uint8_t USART2_Tx_Length = 0;                            // USART2 输出缓冲区当前缓冲规模

/**
 * @brief USART1 初始化函数
 */
void USART1_Init(void)
{
    /* USART 初始化结构体 */
    USART_InitTypeDef USART_InitStructure;

    /* GPIO 初始化结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 串口时钟 和 GPIO 时钟 使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* ------------------ 串口参数配置 ------------------  */
    /* 串口波特率 9600 */
    USART_InitStructure.USART_BaudRate = 9600;
    /* 8 位数据位 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* 1 位停止位 */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* 无奇偶校验 */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* 无硬件数据流控制 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* 收/发模式 */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* USART1 串口初始化 */
    USART_Init(USART1, &USART_InitStructure);

    /* ------------------ 串口 GPIO 配置 ------------------ */
    /*  USART1_TX 初始化 */
    GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 推挽复用输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART1_GPIO, &GPIO_InitStructure);

    /* USART1_TX 已经初始化结束了，GPIO_InitStructure 可以赋别的值继续用 */

    /* USART1_RX 初始化 */
    GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /* 浮空输入 */
    GPIO_Init(USART1_GPIO, &GPIO_InitStructure);

    /* USART1 接收中断使能（USART_IT_RXNE: Receive Data register not empty interrupt） */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* USART1 使能 */
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief USART2 初始化函数
 */
void USART2_Init(void)
{
    /* USART 初始化结构体 */
    USART_InitTypeDef USART_InitStructure;

    /* GPIO 初始化结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 串口时钟使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* GPIO 时钟 使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* ------------------ 串口参数配置 ------------------  */
    /* 串口波特率 115200 */
    USART_InitStructure.USART_BaudRate = 115200;
    /* 8 位数据位 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    /* 1 位停止位 */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    /* 无奇偶校验 */
    USART_InitStructure.USART_Parity = USART_Parity_No;
    /* 无硬件数据流控制 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* 收/发模式 */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* USART2 串口初始化 */
    USART_Init(USART2, &USART_InitStructure);

    /* ------------------ 串口 GPIO 配置 ------------------ */
    /* USART2_TX 初始化 */
    GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 推挽复用输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART2_GPIO, &GPIO_InitStructure);

    /* USART2_TX 已经初始化结束了，GPIO_InitStructure 可以赋别的值继续用 */

    /* USART2_RX 初始化 */
    GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /* 浮空输入 */
    GPIO_Init(USART2_GPIO, &GPIO_InitStructure);

    /* USART2 接收中断使能（USART_IT_RXNE: Receive Data register not empty interrupt） */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* USART2 使能 */
    USART_Cmd(USART2, ENABLE);
}

/* ----------------- USART 接收中断处理函数 ----------------- */

/**
 * @brief USART1 的 接收中断 处理函数（就只是收到数据再发出去）
 *
 * @warning 由于没有设置缓冲机制，一旦数据量过大，会出现丢包的情况
 */
void USART1_IRQHandler(void)
{
    /*  接收到的数据 */
    uint8_t data;

    /*  如果接收到数据（中断标志位不为 0） */
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* 读取数据（接收） */
        data = USART_ReceiveData(USART1);

        /* 发送数据（再发出去） */
        USART_SendByte(USART1, data);
    }
}

/**
 * @brief USART2 的 接收中断 处理函数
 */
void USART2_IRQHandler(void)
{

    /*  如果引发了接收中断（接收中断标志位不为 0，接收数据寄存器不为空） */
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {

        /* 如果输入缓冲区满了，就覆盖最早的数据 */
        if (USART2_Rx_Length >= USART2_RX_BUFFER_SIZE)
        {
            USART2_Rx_Length = 0;
        }

        /* 若接收的是 AT 指令响应 */
        if (AT_Instruction_Mode)
        {
            /* 需要保证数据不为 0 */
            if (USART_ReceiveData(USART2) == 0)
                return;
        }

        /* 把接收到的数据存到输入缓冲区里 */
        USART2_Rx_Buffer[USART2_Rx_Length++] = USART_ReceiveData(USART2);
    }
}

/* --------------------------- USART 收发数据函数 --------------------------- */

/* 💡USART 的数据都会与上 0x01FF 是因为有效位就只有 9 位 */

/**
 * @brief 通过 USART 发送一个字符
 *
 * @param USARTx 通过哪个 USART 发送。
 * 				 可以填：USART1, USART2, USART3, UART4 和 UART5
 * @param Data 发送的数据（一个字符）
 *
 * @note 发送一个字符后会等待，直到发送数据寄存器为空（USART_FLAG_TXE）
 *
 * @see USART_SendData
 */
void USART_SendByte(USART_TypeDef *USARTx, uint16_t Data)
{
    /* Check the parameters */
    assert_param(IS_USART_ALL_PERIPH(USARTx));
    assert_param(IS_USART_DATA(Data));

    /* Transmit Data */
    USARTx->DR = (Data & (uint16_t)0x01FF); // 0x01FF：有效位9位

    /* TXE: Transmit data register Empty */
    /* 以上部分照搬自 USART_SendData，然后下面加上一个对标志位 TXE 的等待，
       目的是避免出现数据覆盖（这一个字符还没发出去就被新字符覆盖了） */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
        ;
}

/**
 * @brief 通过 USART 发送字符串（实际上就是循环调用 USART_SendByte）
 *
 * @param USARTx 通过哪个 USART 发送。
 * 				 可以填：USART1, USART2, USART3, UART4 和 UART5
 * @param str 发送的字符串
 *
 * @note 发送完字符串后会等待，直到整个字符串发送完毕（USART_FLAG_TC）
 *
 * @see USART_SendByte
 */
void USART_SendString(USART_TypeDef *USARTx, char *str)
{
    while (*str != '\0')
    {
        USART_SendByte(USARTx, *str++);
    }

    /* 这里对标志位 TC 的等待，作用与 USART_SendByte 中对标志位 TXE 的等待类似，
       此处是确保整个字符串都传输完毕 (Transmission Complete) */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
        ;
}

/**
 * @brief 从 USART 接收字符
 *
 * @param USARTx 通过哪个 USART 接收。
 * 				 可以填：USART1, USART2, USART3, UART4 和 UART5
 * @return uint8_t 接收到的数据（一个字符）
 * @see USART_ReceiveData
 * @warning 这里会死等，直到接收到数据
 */
uint8_t USART_ReceieByte(USART_TypeDef *USARTx)
{
    /* Check the parameters */
    assert_param(IS_USART_ALL_PERIPH(USARTx));

    /* RXNE: Read data register not empty */
    /* USART_ReceiveData 总共两句，一句参数判断，一句返回数据
       为了确认数据的有效性，加一个对标志位 RXNE 的等待 */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
        ;

    /* 最高位是停止位，直接丢掉不要了 */
    return (uint8_t)(USARTx->DR & (uint16_t)0x01FF); // 0x01FF：有效位9位
}

/* ---------- printf 和 scanf 重定向到 USART1 ---------- */

/* 实测不能使用 scanf，编译不通过，注释下面这块内容可以编译通过，但是无法执行 */
/* .\Objects\Project.axf: Error: L6200E: Symbol __stdout multiply defined (by stdio_streams.o and usart.o). */
typedef struct __FILE FILE;
#pragma import(__use_no_semihosting)
struct __FILE
{
    int handle;
};
FILE __stdout;
_sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    /* 感觉多加一句判断 TC 好像有点用 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
    return (ch);
}

int fgetc(FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        ;
    return (int)USART_ReceiveData(USART1);
}

/* --------------------  USART 串口格式化输出 -------------------- */
/**
 * @brief USART1 格式化输出函数
 *
 * @param fmt 遵循 printf 函数的格式化输出规则填写
 */
void USART1_Printf(char *fmt, ...)
{
    uint8_t i = 0, length = 0;

    va_list ap;
    va_start(ap, fmt);
    vsprintf(USART1_Tx_Buffer, fmt, ap);
    va_end(ap);

    length = strlen((const char *)USART1_Tx_Buffer);
    while (i < length)
    {
        USART_SendByte(USART1, USART1_Tx_Buffer[i++]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
}

/**
 * @brief USART2 格式化输出函数
 *
 * @param fmt 遵循 printf 函数的格式化输出规则填写
 */
void USART2_Printf(char *fmt, ...)
{
    uint8_t i = 0, length = 0;

    va_list ap;
    va_start(ap, fmt);
    vsprintf(USART2_Tx_Buffer, fmt, ap);
    va_end(ap);

    length = strlen((const char *)USART2_Tx_Buffer);
    while (i < length)
    {
        USART_SendByte(USART2, USART2_Tx_Buffer[i++]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
        ;
}
