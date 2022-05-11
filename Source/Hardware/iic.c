#include "iic.h"

/**
 * SCL 高电平代表数据有效，所以需要保证在 SCL 拉高前， SDA 要准备好
 * 而对于/止信号，要在 SDA 跳变前，先拉高 SCL
 * ! 但是，跳变前 SDA 的电平依然要在拉高 SCL 前就准备好
 */

/**
 * @brief IIC 初始化（初始化 SCL 和 SDA 的 GPIO）
 *
 * @note SCL 是 PB6，SDA 是 PB7
 */
void IIC_Init(void)
{
    /* 初始化 SCL 和 SDA 的 GPIO 为输出模式*/
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 拉高 SCL 和 SDA */
    IIC_SCL = 1;
    IIC_SDA = 1;
}

/**
 * @brief 发送起始信号
 *
 * @note SCL 高电平期间，SDA 由高到低跳变
 *
 */
void IIC_Start(void)
{
    /* 配置 SDA 为输出模式 */
    SDA_OUT();
    /* 拉高 SDA 和 SCL */
    IIC_SDA = 1; // 要在拉高 SCL 前让 SDA 为高电平
    IIC_SCL = 1;
    /* 持续一段时间 */
    delay_us(4);
    /* 拉低 SDA（高 → 低） */
    IIC_SDA = 0;
    /* 再持续一段时间（保证形成起始信号） */
    delay_us(4);
    /* 拉低 SCL，准备收发数据 */
    IIC_SCL = 0;
}

/**
 * @brief 发送停止信号
 *
 * @note SCL 高电平期间，SDA 由低到高跳变
 *
 */
void IIC_Stop(void)
{
    /* 配置 SDA 为输出模式 */
    SDA_OUT();
    /* 拉低 SCL 和 SDA */
    IIC_SCL = 0;
    IIC_SDA = 0; // 要在拉高 SCL 前让 SDA 为低电平
    /* 持续一段时间 */
    delay_us(4);

    /* 先拉高 SCL，再拉高 SDA（高 → 低） */
    IIC_SCL = 1;
    IIC_SDA = 1;
    /* 再持续一段时间（保证形成停止信号） */
    delay_us(4);
}

/**
 * @brief 写入一个字节（byte, 8bit）的数据
 *
 * @param data 要写入的数据
 * @return uint8_t 收到的应答信号
 * @retval 0 收到应答信号
 * @retval 1 没有收到应答信号
 */
uint8_t IIC_Send_Data(uint8_t data)
{
    /* 收到的应答信号 */
    uint8_t ack;

    uint8_t mask;

    /* 配置 SDA 为输出模式 */
    SDA_OUT();

    /* 逐位取出并发送（从最高位开始） */
    for (mask = 0x80; mask != 0; mask >>= 1)
    {
        if (mask & data)
        {
            /* 写入 1 */
            IIC_SDA = 1;
        }
        else
        {
            /* 写入 0 */
            IIC_SDA = 0;
        }
        /* 维持 SDA 一段时间 */
        delay_us(4);

        /* 然后拉高 SCL，维持一段时间 */
        IIC_SCL = 1;
        delay_us(4);

        /* 再拉低 SCL，这一位发送完毕 */
        IIC_SCL = 0;
    }
    /* 数据发送完毕，检查从机应答 */

    /* 配置 SDA 为输入模式 */
    SDA_IN();
    /* 释放 SDA总线 */
    IIC_SDA = 1;
    /* 等待从机应答 */
    delay_us(4);

    /* 拉高 SCL，读取应答信号 */
    IIC_SCL = 1;
    ack = READ_SDA;

    /* 维持一段时间再拉低 SCL（本轮传输结束，9 个位通过了 IIC 总线） */
    delay_us(4); // 这个时间应该可以一半在读 ack 之前，一半在之后，不过意义不大
    IIC_SCL = 0;

    /* 返回应答信号 */
    return ack;
}

/**
 * @brief 验证从机是否存在
 *
 * @param addr 从机的地址
 * @return uint8_t 收到的应答信号
 * @retval 0 收到应答信号，从机存在
 * @retval 1 没有收到应答信号，从机不存在
 */
uint8_t IIC_Check_Address(uint8_t address)
{
    /* 收到的应答信号 */
    uint8_t ack;

    /* 发送起始信号 */
    IIC_Start();

    /* 写入从机地址并读回应答信号 */
    /* 地址在高 7 位，所以左移一位，第 8 位为 0，表示写模式 */
    ack = IIC_Send_Data(address << 1);

    /* 发送停止信号 */
    IIC_Stop();

    /* 返回应答信号 */
    return ack;
}
