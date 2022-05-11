#include "sys.h"


/**
 * @brief NVIC 配置函数
 */
void NVIC_Config(void)
{
    /* NVIC 初始化结构体 */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* NVIC 中断优先级分组（2 位抢占优先级，2 位响应优先级） */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ------------------ USART1 NVIC 配置（接收中断） ------------------ */
    /* NVIC 中断通道 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    /* 抢占优先级 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    /* 响应优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    /* 中断使能 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* NVIC 初始化 */
    NVIC_Init(&NVIC_InitStructure);

    /* ------------------ USART2 NVIC 配置（接收中断） ------------------ */
    /* NVIC 中断通道 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    /* 抢占优先级 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* 响应优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    /* 中断使能 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* NVIC 初始化 */
    NVIC_Init(&NVIC_InitStructure);
}

