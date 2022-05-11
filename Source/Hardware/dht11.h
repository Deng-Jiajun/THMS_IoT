#ifndef __DHT11_H
#define __DHT11_H

#include "sys.h"

/* DHT11 的数据线接在 PA5 */

/* （根据位带操作）定义 DHT11 的读写操作 */
#define DHT11_WRITE PAout(5)
#define DHT11_READ PAin(5)
#define DHT11_PIN GPIO_Pin_5
#define DHT11_PORT GPIOA

/* 配置 DHT11 对应引脚（PA5）的输入/输出模式 */
#define DHT11_IN()                                               \
    {                                                            \
        /* 清零 GPIOA_CLR 的 CNF5 和 MODE5（位 23:20） */ \
        GPIOA->CRL &= 0XFF0FFFFF;                                \
        /* 配置为上拉/下拉输入模式（10 00） */      \
        GPIOA->CRL |= 8 << 5 * 4;                                \
    }
#define DHT11_OUT()                                                       \
    {                                                                     \
        /* 清零 GPIOA_CLR 的 CNF5 和 MODE5（位 23:20） */          \
        GPIOA->CRL &= 0XFF0FFFFF;                                         \
        /* 配置为通用推挽输出，最大速度 50MHz（00 11） */ \
        GPIOA->CRL |= 3 << 5 * 4;                                         \
    }


uint8_t DHT11_Read_Data(uint8_t *temperature, uint8_t *humidity);
void DHT11_Init(void);

#endif
