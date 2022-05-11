#ifndef __OLED_IIC_H
#define __OLED_IIC_H

#include "sys.h"

/**
 * @brief 基于 IIC 总线的 OLED 显示模块
 * 
 */


#define OLED_Slave_Address_Write ((uint8_t)0x78)


void OLED_Init(void);
void OLED_Clear(void);
void OLED_On(void);

void OLED_ShowChar(uint8_t row, uint8_t col, char ch);
void OLED_ShowStr(uint8_t row, uint8_t col, char *str);
void OLED_ShowNum(uint8_t row, uint8_t col, int num);
void OLED_ShowChineseChar(uint8_t row, uint8_t col, uint8_t index);


#endif
