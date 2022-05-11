#ifndef __MQTT_H
#define __MQTT_H

#include "sys.h"

#define MQTT_USART WIFI_USART // MQTT 传输数据使用的串口

#define MQTT_RX_BUFFER_SIZE WIFI_RX_BUFFER_SIZE // MQTT 输入缓冲区大小
#define MQTT_Rx_Buffer WiFi_Rx_Buffer           // MQTT 输入缓冲区
#define MQTT_Rx_Length WiFi_Rx_Length           // MQTT 输入缓冲区当前缓冲规模

#define MQTT_TX_BUFFER_SIZE WIFI_TX_BUFFER_SIZE // MQTT 输出缓冲区大小
#define MQTT_Tx_Buffer WiFi_Tx_Buffer           // MQTT 输出缓冲区
#define MQTT_Tx_Length WiFi_Tx_Length           // MQTT 输出缓冲区当前缓冲规模

/* ❗这个 MQTT 模块默认的 QoS 为 0，基本没有对 QoS 1 和 QoS 2 进行适配 */

bool MQTT_CONNECT(const char *clientId, const char *deviceName, const char *productKey, const char *DeviceSecret, uint8_t securemode, const char *signmethod);
bool MQTT_PINGREQ(void);
bool MQTT_SUBSCRIBE(const char *topicFilter, uint8_t requestedQoS);
bool MQTT_UNSUBSCRIBE(const char *topicFilter);
bool MQTT_PUBLISH(const char *topicName, const char *message, uint8_t QoS);

#endif
