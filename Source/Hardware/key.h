#ifndef __key_H
#define __key_H

#include "sys.h"

#define KEY_GPIO_PIN   GPIO_Pin_15
#define KEY_GPIO_PORT  GPIOB
#define KEY_GPIO_CLK  RCC_APB2Periph_GPIOB

void KEY_GPIO_Config(void);
void KEY_Scan(void);


#endif

