#include "sys.h"


/**
 * @brief 根据 GPIOx 返回对应 RCC 寄存器的地址
 *
 * @param GPIOx 要查询的 GPIO 端口，可填 GPIOx（x=A..E）
 *
 * @return uint32_t 返回对应的 RCC 寄存器的地址，如果参数错误，返回 0
 * 
 * @retval RCC_APB2Periph_GPIOx(x=A..E) 对应的 RCC 寄存器的地址 
 * @retval FAILED(0) 参数错误
 */
uint32_t Get_RCC_APB2Periph_GPIOx(GPIO_TypeDef *GPIOx)
{
    /* 要返回的 RCC 寄存器地址 */
    uint32_t RCC_APB2Periph_GPIOx = 0;

    /* 参数检查 */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* 有考虑过用 switch，但是 GPIOx 不是一个整形常量表达式，
    要算出偏移地址来作为 case，实际上更不直观，不如直接一排 if 还更清晰一点 */
    if (GPIOx == GPIOA)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA;
    }
    else if (GPIOx == GPIOB)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOB;
    }
    else if (GPIOx == GPIOC)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOC;
    }
    else if (GPIOx == GPIOD)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOD;
    }
    else if (GPIOx == GPIOE)
    {
        RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOE;
    }
    else
    {
        RCC_APB2Periph_GPIOx = FAILED;
    }

    return RCC_APB2Periph_GPIOx;
}
