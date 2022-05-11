#include "sys.h"

bool AT_Instruction_Mode = true; // 目前将接收到的是什么响应，true：AT 指令，false：MQTT 消息

uint8_t __WiFi_Init(void);
bool __WiFi_Send_AT_Instruction(char *command, char *positive_response, uint8_t wait_time);
/* 不对外暴露的函数👆 */

/**
 * @brief WiFi 模块初始化
 *        重启 WiFi 模块，连接到 WiFi，与 TCP 服务器建立连接（并进入透传模式）
 *
 * @see __WiFi_Init()
 *
 * @retval true(1) WiFi 模块初始化成功
 * @retval false(0) WiFi 模块初始化失败
 *
 * @warning 初始化 WiFi 模块前，至少需要进行以下三部分的初始化：
    delay_init();
    NVIC_Config();
    USART2_Init(); 👈不一定是 USART2，具体看 WiFi 模块用的是哪个串口
 */
bool WiFi_Init(void)
{
    AT_Instruction_Mode = true;

    /* 应该给 __WiFi_Init() 的返回值安排一个 switch，但是也不知道该做些什么 */

    if (__WiFi_Init() == 0)
    {
        printf("WiFi init success\r\n");
        AT_Instruction_Mode = false;
        return true;
    }
    else
    {
        printf("WiFi init failed\r\n");
        return false;
    }
}

/**
 * @brief WiFi 模块初始化
 *        一共 9 步，具体内容见源码实现
 *
 * @retval 0 WiFi 模块初始化成功
 * @retval 1~9 初始化失败，某一步执行出错，串口将输出错误信息
 */
uint8_t __WiFi_Init(void)
{
    /* 💡 手动按单片机上的复位键进行复位 */
    {
        uint8_t times = 0;
        uint8_t max_times=10;
        while (++times < max_times)
        {
            printf("Waiting Wfi module reset...%d\r\n", times);
            delay_ms(1000);
            if (strstr(WiFi_Rx_Buffer, "ready"))
            {
                break;
            }
        }
        /* 在规定时间内收到 「ready」，则复位成功，否则复位失败 */
        if (times < max_times)
        {
            printf("WiFi module rested successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module rested failed!\r\n\r\n");
            return 1;
        }
    }

    // /* WiFi 模块复位(如果一直失败试试手动复位) */
    // printf("reseting WiFi module...\r\n");
    // if (__WiFi_Send_AT_Instruction("AT+RST", "ready", 15))
    // {
    //     printf("WiFi module rested successfully!\r\n\r\n");
    // }
    // else
    // {
    //     printf("WiFi module rested failed!\r\n\r\n");
    //     return 1;
    // }

    /* 配置 WiFi 模式 */
    {
        printf("configuring WiFi module...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+CWMODE=1", "OK", 5))
        {
            printf("WiFi module configured successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module configured failed!\r\n\r\n");
            return 2;
        }
    }

    /* 重启 WiFi 模块 */
    {
        printf("restarting WiFi module...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+RST", "ready", 10))
        {
            printf("WiFi module restarted successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module restarted failed!\r\n\r\n");
            return 3;
        }
    }

    /* 取消自动连接 */
    {
        printf("disabling auto-connect...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+CWAUTOCONN=0", "OK", 5))
        {
            printf("auto-connect disabled successfully!\r\n\r\n");
        }
        else
        {
            printf("auto-connect disabled failed!\r\n\r\n");
            return 4;
        }
    }

    /* 连接到 AP */
    {
        /* 拼接用于连接 AP 的 AT 指令 */
        char at_command_connect_ap[256] = {0};
        sprintf(at_command_connect_ap, "AT+CWJAP_DEF=\"%s\",\"%s\"", AP_SSID, AP_PWD);

        printf("connecting to AP...\r\n");

        if (__WiFi_Send_AT_Instruction(at_command_connect_ap, "OK", 10))
        {
            printf("WiFi module connected to AP successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module connected to AP failed!\r\n\r\n");
            return 5;
        }
    }

    /* 设置为单连接模式 */
    {
        printf("setting WiFi module to single-connection mode...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+CIPMUX=0", "OK", 5))
        {
            printf("WiFi module set to single-connection mode successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module set to single-connection mode failed!\r\n\r\n");
            return 6;
        }
    }

    /* 设置为透传模式 */
    {
        printf("enabling WiFi module to pass-through mode...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+CIPMODE=1", "OK", 5))
        {
            printf("WiFi module enabled to pass-through mode successfully!\r\n\r\n");
        }
        else
        {
            printf("WiFi module enabled to pass-through mode failed!\r\n\r\n");
            return 7;
        }
    }

    /* 建立 TCP 通信 */
    {
        /* 拼接用于建立 TCP 通信的 AT 指令 */
        char at_command_establish_tcp_connection[256] = {0};
        sprintf(at_command_establish_tcp_connection, "AT+CIPSTART=\"TCP\",\"%s\",%d", TCP_SERVER_IP, TCP_SERVER_PORT);

        printf("establishing TCP connection...\r\n");
        if (__WiFi_Send_AT_Instruction(at_command_establish_tcp_connection, "OK", 10))
        {
            printf("TCP connection established successfully!\r\n\r\n");
        }
        else
        {
            printf("TCP connection established failed!\r\n\r\n");
            return 8;
        }
    }

    /* 进入透传模式（准备开始发送数据） */
    {
        printf("entering pass-through mode...\r\n");
        if (__WiFi_Send_AT_Instruction("AT+CIPSEND", "\r\nOK\r\n\r\n>", 5))
        {
            printf("pass-through mode enabled successfully!\r\n\r\n");
        }
        else
        {
            printf("pass-through mode enabled failed!\r\n\r\n");
            return 9;
        }
    }

    return 0;
}

/**
 * @brief 向 WiFi 模块（通过串口）发送 AT 指令
 *
 * @param command 要发送的 AT 指令
 * @param positive_response 预期响应包含的字符串
 * @param wait_time 等待时间（单位：s）
 *
 * @retval true 得到预期响应
 * @retval false 未得到预期响应
 */
bool __WiFi_Send_AT_Instruction(char *command, char *positive_response, uint8_t wait_time)
{
    /* 等待时间（wait_time） 以秒为单位，每 100ms 检查一次缓冲区，所以 ×10 */
    wait_time *= 10;

    /* 输入缓冲区清零 */
    memset(WiFi_Rx_Buffer, 0, WIFI_RX_BUFFER_SIZE);
    WiFi_Rx_Length = 0;

    /* 发送 AT 指令 */
    USART2_Printf("%s\r\n", command);

    /* wait_time == 0 时跳出循环 */
    while (--wait_time)
    {
        // printf("Waiting respond...%d\r\n", wait_time);
        // printf("%d ", wait_time);
        delay_ms(100);
        if (strstr(WiFi_Rx_Buffer, positive_response))
        {
            break;
        }
    }

    /* 在倒计时前内收到预期响应，则返回 true，否则返回 false */
    if (wait_time)
    {
        return true;
    }
    else
    {
        return false;
    }
}
