#ifndef __ASSERT_PARAM_H
#define __ASSERT_PARAM_H

#include "sys.h"

#ifdef USE_FULL_ASSERT

/**
 * @brief 参数检查报错函数
 * @note 在 stm32f10x_conf.h 中有一些说明
 * @warning 要启用该函数，需要在 stm32f10x_conf.h 第 55 行取消对 USE_FULL_ASSERT 宏定义的注释
 */
void assert_failed(u8 *file, u32 line)
{
    printf("Wrong parametersvalue: file %s on line %d\r\n", file, line);
    while (1)
        ;
}

/* 据说是官方给出的实现（没查证）👇*/
// void assert_failed(u8* file, u32 line)
// {

//     /* User can add his ownimplementation to report the file name and line number,
//        ex: printf("Wrong parametersvalue: file %s on line %drn", file, line) */

//     /* Infinite loop */
//     while (1) { }
// }
#endif

#endif
