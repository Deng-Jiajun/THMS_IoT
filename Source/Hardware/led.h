#ifndef __LED_H
#define __LED_H

#include "sys.h"

/* LED1：PA7    LED2：PA8 */

#define LED1_GPIO GPIOA
#define LED1_PIN GPIO_Pin_7
#define LED2_GPIO GPIOA
#define LED2_PIN GPIO_Pin_8

void LED_Init(void);
void LED_On(uint8_t led_num);
void LED_Off(uint8_t led_num);
void LED_Toggle(uint8_t led_num);

#endif
