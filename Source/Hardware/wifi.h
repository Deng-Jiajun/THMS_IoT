#ifndef __WIFI_H
#define __WIFI_H

#include "sys.h"

extern bool AT_Instruction_Mode;

#define WIFI_USART USART2 // WiFi 模块传输数据使用的串口

#define WIFI_RX_BUFFER_SIZE USART2_RX_BUFFER_SIZE // WiFi 模块 输入缓冲区大小
#define WiFi_Rx_Buffer USART2_Rx_Buffer           // WiFi 模块 输入缓冲区
#define WiFi_Rx_Length USART2_Rx_Length           // WiFi 模块 输入缓冲区当前缓冲规模

/* WiFi 模块并没有实际使用到输出缓冲区，AT 指令的格式相对简单，直接通过字符串拼凑即可。
   但是 MQTT 需要用到输出缓冲区，所以提供给 MQTT 使用（放着不用也是浪费） */
#define WIFI_TX_BUFFER_SIZE USART2_TX_BUFFER_SIZE // WiFi 模块 输出缓冲区大小
#define WiFi_Tx_Buffer USART2_Tx_Buffer           // WiFi 模块 输出缓冲区
#define WiFi_Tx_Length USART2_Tx_Length           // WiFi 模块 输出缓冲区当前缓冲规模

#define AP_SSID "Tenda_6BDFF0"  // AP 的 SSID
#define AP_PWD "12345678" // AP 的 密码


/* 二选一👇(本地 TCP 调试、连接阿里云互联网平台) */
// #define LOCAL_TCP_SERVER
#define Ali_TCP_SERVER

#ifdef LOCAL_TCP_SERVER // 连接本地 TCP 服务器，用于本地调试
#define TCP_SERVER_IP "192.168.0.195"
#define TCP_SERVER_PORT 8080
/* 👆用上面两个参数凑出下面这个 AT 指令👇（上面两个宏定义暂时没有实际作用）*/
#define AT_COMMAND_ESTABLISH_TCP_CONNECTION "AT+CIPSTART=\"TCP\",\"192.168.0.195\",8080"
/* 👆 用于建立 TCP 通信的 AT 指令 */
#endif

#ifdef Ali_TCP_SERVER // 连接阿里云 MQTT 服务器，用于收发数据
#define TCP_SERVER_IP "iot-06z00irxznx4d2u.mqtt.iothub.aliyuncs.com"
#define TCP_SERVER_PORT 1883
#endif

bool WiFi_Init(void);

#endif
