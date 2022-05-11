#ifndef __IIC_H
#define __IIC_H
#include "sys.h"

//配置PA7的输入输出模式
#define SDA_IN()                         \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)8 << 28; \
    }
#define SDA_OUT()                        \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)3 << 28; \
    }

#define IIC_SCL PBout(6) // 写 SCL
#define IIC_SDA PBout(7) // 写 SDA
#define READ_SDA PBin(7) // 读 SDA

#define IIC_ACK 0
#define IIC_NACK 1

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Send_Data(uint8_t data);
uint8_t IIC_Check_Address(uint8_t address);

#endif
