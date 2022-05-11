#include "sys.h"

bool __MQTT_Build_Connect_Packet(const char *clientId, const char *deviceName, const char *productKey, const char *deviceSecret, uint8_t securemode, const char *signmethod);
bool __MQTT_Build_Subscribe_Packet(const char *topicFilter, uint8_t requestedQoS);
bool __MQTT_Build_UnSubscribe_Packet(const char *topicFilter);
bool __MQTT_Build_Publish_Packet(const char *topicName, const char *message, uint8_t QoS);
uint8_t __Size_Of_Remaining_Length(uint8_t remaining_length, char *fixed_header);
void __MQTT_Send_Packet(void);

/**
 * @brief MQTT 发送 CONNECT 报文，连接服务器
 *
 * @param clientId 「客户端 ID 建议mac或sn，64字符内」
 * @param deviceName 设备名称，从物联网平台获取
 * @param productKey 产品密钥，从物联网平台获取
 * @param deviceSecret 设备密钥，从物联网平台获取
 * @param securemode 安全模式，可选值有 2 （wss协议）和 3（ws协议）
 * @param signmethod 签名算法类型
 *
 * @retval true 连接服务器成功
 * @retval false 连接服务器失败
 *
 * @see __MQTT_Build_Connect_Packet
 * @note 参数填写实例：MQTT_CONNECT("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1")
 */
bool MQTT_CONNECT(const char *clientId, const char *deviceName, const char *productKey, const char *DeviceSecret, uint8_t securemode, const char *signmethod)
{
    uint8_t i = 0;

    /* 输入缓冲区清零 */
    memset(MQTT_Rx_Buffer, 0, MQTT_RX_BUFFER_SIZE);
    MQTT_Rx_Length = 0;

    /* 构建 CONNECT 报文（并写入 MQTT 输出缓冲区） */
    if (__MQTT_Build_Connect_Packet(clientId, deviceName, productKey, DeviceSecret, securemode, signmethod))
    {
        /* 发送报文 */
        __MQTT_Send_Packet();

        /* 等 5 秒 */
        for (i = 0; i < 5; i++)
            delay_ms(1000);

        /* 检查输入缓冲区 */
        if (MQTT_Rx_Length && MQTT_Rx_Buffer[0] == 0x20 && MQTT_Rx_Buffer[1] == 0x02 && MQTT_Rx_Buffer[2] == 0x00 && MQTT_Rx_Buffer[3] == 0x00)
        {
            printf("connect success\r\n");
            return true;
        }
        else
        {
            printf("connect fail\r\n");
        }
    }
    else
    {
        printf("MQTT_CONNECT: build packet failed\r\n");
    }

    return false;
}

/**
 * @brief MQTT 发送 PINGREQ 报文，检查连接状态
 *
 * @retval true 连接正常
 * @retval false 连接已断开
 */
bool MQTT_PINGREQ(void)
{
    uint8_t i;
    char PINGREQ_Packet[2] = {0xC0, 0x00}; /* PINGREQ 报文 */

    /* 输入缓冲区清零 */
    memset(MQTT_Rx_Buffer, 0, MQTT_RX_BUFFER_SIZE);
    MQTT_Rx_Length = 0;

    /* 直接把数据写入 MQTT 输出缓冲区 */
    MQTT_Tx_Buffer[0] = PINGREQ_Packet[0];
    MQTT_Tx_Buffer[1] = PINGREQ_Packet[1];
    MQTT_Tx_Length = 2;

    /* 发送报文 */
    __MQTT_Send_Packet();

    /* 等 5 秒 */
    for (i = 0; i < 5; i++)
        delay_ms(1000);

    /* 检查输入缓冲区 */
    if (MQTT_Rx_Length && MQTT_Rx_Buffer[0] == 0xD0 && MQTT_Rx_Buffer[1] == 0x00)
        return true;
    else
        return false;
}

/**
 * @brief MQTT 发送 SUBSCRIBE 报文，订阅主题
 *
 * @param topicFilter 主题过滤器
 * @param requestedQoS 服务质量要求
 *
 * @retval true 订阅成功
 * @retval false 订阅失败
 */
bool MQTT_SUBSCRIBE(const char *topicFilter, uint8_t requestedQoS)
{
    uint8_t i = 0;

    /* 输入缓冲区清零 */
    memset(MQTT_Rx_Buffer, 0, MQTT_RX_BUFFER_SIZE);
    MQTT_Rx_Length = 0;

    /* 构建 SUBSCRIBE 报文（并写入 MQTT 输出缓冲区） */
    if (__MQTT_Build_Subscribe_Packet(topicFilter, requestedQoS))
    {
        /* 发送报文 */
        __MQTT_Send_Packet();

        /* 等 5 秒 */
        for (i = 0; i < 5; i++)
            delay_ms(1000);

        /* 检查输入缓冲区 */
        if (MQTT_Rx_Length && MQTT_Rx_Buffer[0] == 0x90 && MQTT_Rx_Buffer[1] == 0x03 && MQTT_Rx_Buffer[2] == 0x00 && MQTT_Rx_Buffer[3] == 0x0A && MQTT_Rx_Buffer[4] == 0x01)
            return true;
    }

    return false;
}

/**
 * @brief MQTT 发送 UNSUBSCRIBE 报文，取消订阅主题
 *
 * @param topicFilter 主题过滤器
 *
 * @retval true 取消订阅成功
 * @retval false 取消订阅失败
 */
bool MQTT_UNSUBSCRIBE(const char *topicFilter)
{
    uint8_t i = 0;

    /* 输入缓冲区清零 */
    memset(MQTT_Rx_Buffer, 0, MQTT_RX_BUFFER_SIZE);
    MQTT_Rx_Length = 0;

    /* 构建 SUBSCRIBE 报文（并写入 MQTT 输出缓冲区） */
    if (__MQTT_Build_UnSubscribe_Packet(topicFilter))
    {
        /* 发送报文 */
        __MQTT_Send_Packet();

        /* 等 5 秒 */
        for (i = 0; i < 5; i++)
            delay_ms(1000);

        /* 检查输入缓冲区 */
        if (MQTT_Rx_Length && MQTT_Rx_Buffer[0] == 0xB0 && MQTT_Rx_Buffer[1] == 0x02 && MQTT_Rx_Buffer[2] == 0x00 && MQTT_Rx_Buffer[3] == 0x0A)
            return true;
    }

    return false;
}

/**
 * @brief MQTT 发送 PUBLISH 报文，发布消息
 *
 * @param topicName 主题名
 * @param message 要发布的消息（json 格式）
 * @param QoS QoS 等级
 *
 * @retval true 发布成功
 * @retval false 发布失败
 */
bool MQTT_PUBLISH(const char *topicName, const char *message, uint8_t QoS)
{
    uint8_t i = 0;

    /* 输入缓冲区清零 */
    memset(MQTT_Rx_Buffer, 0, MQTT_RX_BUFFER_SIZE);
    MQTT_Rx_Length = 0;

    /* 构建 PUBLISH 报文（并写入 MQTT 输出缓冲区） */
    if (__MQTT_Build_Publish_Packet(topicName, message, QoS))
    {
        /* 发送报文 */
        __MQTT_Send_Packet();

        /* 等 5 秒 */
        for (i = 0; i < 5; i++)
            delay_ms(1000);

        /* 检查输入缓冲区 */
        if (QoS == 0)
        {
            /* QoS = 0，没有响应报文，直接返回 true */
            return true;
        }
        else
        {
            /* QoS = 1 或 QoS = 2 */
        }
    }

    return false;
}

/**
 * @brief 构建 MQTT CONNECT 报文，并写入 MQTT 输出缓冲区
 *
 * @param clientId 「客户端 ID 建议mac或sn，64字符内」
 * @param deviceName 设备名称，从物联网平台获取
 * @param productKey 产品密钥，从物联网平台获取
 * @param deviceSecret 设备密钥，从物联网平台获取
 * @param securemode 安全模式，可选值有2 （wss协议）和3（ws协议）
 * @param signmethod 签名算法类型
 *
 * @retval true 构建成功
 * @retval false 构建失败
 *
 * @note 字符串比较用到了 string.h 中的 strcmp() 函数
 * @note password 加密用到了 utils_hmac_sha1() 函数
 *
 * @note 参数填写实例：__MQTT_Build_Connect_Packet("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1")
 */
bool __MQTT_Build_Connect_Packet(const char *clientId, const char *deviceName, const char *productKey, const char *deviceSecret, uint8_t securemode, const char *signmethod)
{
    uint8_t index_fixed_header = 0;    // 固定报头的索引
    uint8_t index_variable_header = 0; // 可变报头的索引
    uint8_t index_payload = 0;         // 有效载荷部分的索引

    uint8_t size_fixed_header = 0;           // 固定报头部分的长度
    const uint8_t size_variable_header = 10; // 可变报头部分的长度（2 字节）
    uint8_t size_payload;                    // 有效载荷部分的长度

    /* 💡为作区分，在本函数中，「length」 只作为剩余长度字段的名称，
        真正的「长度」变量都使用 「size」进行命名 */
    uint8_t remaining_length;          // 剩余长度
    uint8_t size_remaining_length = 0; // 「剩余长度」字段的长度（占几个字节）

    /* 固定报头部分（2 ~ 5 字节） */
    char fixed_header[5] = {0};

    /* 可变报头部分（10 字节） */
    const char variable_header[size_variable_header] = {0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0xC2, 0x00, 0x64};

    /* 有效载荷部分（长度不定） */
    char payload_client_identifier[128] = {0}; // 有效载荷的 Client Identifier 部分
    char payload_user_name[128] = {0};         // 有效载荷的 User Name 部分
    char payload_password_raw[128] = {0};      // 有效载荷的 Password 部分（加密前）
    char payload_password_encoded[128] = {0};  // 有效载荷的 Password 部分（加密后）


    /* ⚒️构建有效载荷部分（Client Identifier、User Name、Password）*/
    {
        /* 构建 Client Identifier 字段
            Client Identifier 格式：clientId+"|securemode=3,signmethod=hmacsha1|"*/
        sprintf(payload_client_identifier, "%s|securemode=%d,signmethod=%s|", clientId, securemode, signmethod);

        /* 构建 User Name 字段
            User Name 格式：deviceName+"&"+productKey */
        sprintf(payload_user_name, "%s&%s", deviceName, productKey);

        /* 构建 Password 字段（目前只有 hmacsha1 一种加密方式）
            Password 格式："clientId"+clientId+"sdeviceName"+sdeviceName+"productKey"+productKey */
        sprintf(payload_password_raw, "clientId%sdeviceName%sproductKey%s", clientId, deviceName, productKey);

        /* password 加密 */
        if (strcmp(signmethod, "hmacsha1") == 0) // hmacsha1 加密
        {
            utils_hmac_sha1(payload_password_raw, strlen(payload_password_raw),
                            payload_password_encoded, deviceSecret,
                            strlen(deviceSecret));
        }
        else // 未定义的加密方式
        {
            printf("signmethod is not defined!\r\n");
            return false;
        }
    }

    /* 计算有效载荷部分的长度（Client Identifier、User Name、Password
        三部分的长度，加上每部分 2 个字节的长度前缀） */
    size_payload = 6 + strlen(payload_client_identifier) + strlen(payload_user_name) + strlen(payload_password_encoded);

    /* 计算剩余长度（可变报头的长度（10字节）加上有效载荷的长度） */
    remaining_length = size_variable_header + size_payload;

    /* ⚒️构建固定报头部分 */
    {
        /* 第一个字节为 0x10 */
        fixed_header[0] = 0x10;

        /* 计算剩余长度的长度（占几个字节），并写入 fixed_header */
        size_remaining_length = __Size_Of_Remaining_Length(remaining_length, fixed_header);
    }


    /* ⚒️构建可变报头部分 已在 variable_header 声明 + 初始化时完成 */

    /* 固定报头的长度 =「剩余长度」字段的大小 +1（固定报头的第一个字节） */
    size_fixed_header = size_remaining_length + 1;

    /* 🗂️确定固定报头的索引（0） */
    index_fixed_header = 0;


    /* 先清空 MQTT 输出缓冲区 
    memset(MQTT_Tx_Buffer, 0, MQTT_TX_BUFFER_SIZE);
    MQTT_Tx_Length = 0;*/


    /* ✍️向 MQTT 输出缓冲区写入 CONNECT 报文的固定报头部分 */
    memcpy(MQTT_Tx_Buffer + index_fixed_header, fixed_header, size_fixed_header);

    /* 🗂️确定可变报头的索引（=固定报头的长度） */
    index_variable_header = size_fixed_header;

    /* ✍️向 MQTT 输出缓冲区写入 CONNECT 报文的可变报头部分 */
    memcpy(MQTT_Tx_Buffer + index_variable_header, variable_header, size_variable_header);

    /* 🗂️确定有效载荷的索引（= 可变报头的索引 + 可变报头的长度(10)） */
    index_payload = index_variable_header + size_variable_header;

    /* ✍️向 MQTT 输出缓冲区写入 CONNECT 报文的有效载荷部分 */
    {
        /* 写入 Client Identifier 的长度前缀 */
        /* payload_client_identifier 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_payload + 0] = (strlen(payload_client_identifier) >> 8) & 0xFF;
        /* payload_client_identifier 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_payload + 1] = strlen(payload_client_identifier) & 0xFF;

        index_payload += 2; // 索引前移

        /* 写入 Client Identifier */
        memcpy(MQTT_Tx_Buffer + index_payload, payload_client_identifier, strlen(payload_client_identifier));

        index_payload += strlen(payload_client_identifier); // 索引前移

        /* 写入 User Name 的长度前缀 */
        /* payload_user_name 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_payload + 0] = (strlen(payload_user_name) >> 8) & 0xFF;
        /* payload_user_name 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_payload + 1] = strlen(payload_user_name) & 0xFF;

        index_payload += 2; // 索引前移

        /* 写入 User Name */
        memcpy(MQTT_Tx_Buffer + index_payload, payload_user_name, strlen(payload_user_name));

        index_payload += strlen(payload_user_name); // 索引前移

        /* 写入 Password 的长度前缀 */
        /* payload_password_encoded 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_payload + 0] = (strlen(payload_password_encoded) >> 8) & 0xFF;
        /* payload_password_encoded 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_payload + 1] = strlen(payload_password_encoded) & 0xFF;

        index_payload += 2; // 索引前移

        /* 写入 Password */
        memcpy(MQTT_Tx_Buffer + index_payload, payload_password_encoded, strlen(payload_password_encoded));
    }

    /* 最后修改输出缓冲区的大小 */
    MQTT_Tx_Length = size_fixed_header + size_variable_header + size_payload;

    return true;
}

/**
 * @brief 构建 MQTT SUBSCRIBE 报文，并写入 MQTT 输出缓冲区
 *
 * @param topicFilter  主题过滤器
 * @param requestedQoS 服务质量要求
 *
 * @retval true 构建成功
 * @retval false 构建失败
 * @note 参数填写实例：__MQTT_Build_Subscribe_Packet("/sys/h6wxLg1Xd3V/DHT11_01/thing/service/property/set", 0)
 */
bool __MQTT_Build_Subscribe_Packet(const char *topicFilter, uint8_t requestedQoS)
{
    uint8_t index_fixed_header = 0;    // 固定报头的索引
    uint8_t index_variable_header = 0; // 可变报头的索引
    uint8_t index_payload = 0;         // 有效载荷部分的索引

    uint8_t size_fixed_header = 0;          // 固定报头部分的长度
    uint8_t size_payload;                   // 有效载荷部分的长度
    const uint8_t size_variable_header = 2; // 可变报头部分的长度(2 字节)

    /* 💡为作区分，在本函数中，「length」 只作为剩余长度字段的名称，
        真正的「长度」变量都使用 「size」进行命名 */
    uint8_t remaining_length;          // 剩余长度
    uint8_t size_remaining_length = 0; // 「剩余长度」字段的长度（占几个字节）

    /* 固定报头部分（2 ~ 5 字节） */
    char fixed_header[5] = {0};

    /* 可变报头部分（2 字节） 「报文标识符」 */
    const char variable_header[size_variable_header] = {0x00, 0x0A};

    /* 有效载荷部分为：2 字节长度前缀 + Topic Filter + Requested QoS */

    /* 计算有效载荷部分的长度（Topic Filter + 2 字节长度前缀 + QosS（1字节）*/
    size_payload = strlen(topicFilter) + 3;

    /* 计算剩余长度（可变报头的长度（2字节）加上有效载荷的长度） */
    remaining_length = size_variable_header + size_payload;

    /* ⚒️构建固定报头部分 */
    {
        /* 第一个字节为 0x82 */
        fixed_header[0] = 0x82;

        /* 计算剩余长度的长度（占几个字节），并写入 fixed_header */
        size_remaining_length = __Size_Of_Remaining_Length(remaining_length, fixed_header);
    }

    /* ⚒️构建可变报头部分 已在 variable_header 声明 + 初始化时完成 */

    /* 固定报头的长度 =「剩余长度」字段的大小 +1（固定报头的第一个字节） */
    size_fixed_header = size_remaining_length + 1;

    /* 🗂️确定固定报头的索引（0） */
    index_fixed_header = 0;

    /* 先清空 MQTT 输出缓冲区 
    memset(MQTT_Tx_Buffer, 0, MQTT_TX_BUFFER_SIZE);
    MQTT_Tx_Length = 0;*/

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的固定报头部分 */
    memcpy(MQTT_Tx_Buffer + index_fixed_header, fixed_header, size_fixed_header);

    /* 🗂️确定可变报头的索引（=固定报头的长度） */
    index_variable_header = size_fixed_header;

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的可变报头部分 */
    memcpy(MQTT_Tx_Buffer + index_variable_header, variable_header, size_variable_header);

    /* 🗂️确定有效载荷的索引（= 可变报头的索引 + 可变报头的长度(2)） */
    index_payload = index_variable_header + size_variable_header;

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的有效载荷部分 */
    {
        /* 写入 Topic Filter 的长度前缀*/
        /* topicFilter 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_payload + 0] = (uint8_t)(strlen(topicFilter) >> 8);
        /* topicFilter 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_payload + 1] = (uint8_t)(strlen(topicFilter));

        index_payload += 2; // 索引前移

        /* 写入 Topic Filter */
        memcpy(MQTT_Tx_Buffer + index_payload, topicFilter, strlen(topicFilter));

        index_payload += strlen(topicFilter); // 索引前移

        /* 写入 Requested QoS */
        if (requestedQoS == 0 || requestedQoS == 1 || requestedQoS == 2)
        {
            MQTT_Tx_Buffer[index_payload] = requestedQoS;
        }
        else // 非法 QoS 值
        {
            return false;
        }
    }

    /* 最后修改输出缓冲区的大小 */
    MQTT_Tx_Length = size_fixed_header + size_variable_header + size_payload;

    return true;
}

/**
 * @brief 构建 MQTT UNSUBSCRIBE 报文，并写入 MQTT 输出缓冲区
 *
 * @param topicFilter  主题过滤器
 *
 * @retval true 构建成功
 * @retval false 构建失败
 */
bool __MQTT_Build_UnSubscribe_Packet(const char *topicFilter)
{
    uint8_t index_fixed_header = 0;    // 固定报头的索引
    uint8_t index_variable_header = 0; // 可变报头的索引
    uint8_t index_payload = 0;         // 有效载荷部分的索引

    uint8_t size_fixed_header = 0;          // 固定报头部分的长度
    uint8_t size_payload;                   // 有效载荷部分的长度
    const uint8_t size_variable_header = 2; // 可变报头部分的长度(2 字节)

    /* 💡为作区分，在本函数中，「length」 只作为剩余长度字段的名称，
        真正的「长度」变量都使用 「size」进行命名 */
    uint8_t remaining_length;          // 剩余长度
    uint8_t size_remaining_length = 0; // 「剩余长度」字段的长度（占几个字节）

    /* 固定报头部分（2 ~ 5 字节） */
    char fixed_header[5] = {0};

    /* 可变报头部分（2 字节） 「报文标识符」 */
    const char variable_header[size_variable_header] = {0x00, 0x0A};

    /* 有效载荷部分为：2 字节长度前缀 + Topic Filter */
    size_payload = 2 + strlen(topicFilter);

    /* 计算剩余长度（可变报头的长度（2字节）加上有效载荷的长度） */
    remaining_length = size_variable_header + size_payload;

    /* ⚒️构建固定报头部分 */
    {
        /* 第一个字节为 0xA2 */
        fixed_header[0] = 0xA2;

        /* 计算剩余长度的长度（占几个字节），并写入 fixed_header */
        size_remaining_length = __Size_Of_Remaining_Length(remaining_length, fixed_header);
    }

    /* ⚒️构建可变报头部分 已在 variable_header 声明 + 初始化时完成 */

    /* 固定报头的长度 =「剩余长度」字段的大小 +1（固定报头的第一个字节） */
    size_fixed_header = size_remaining_length + 1;

    /* 🗂️确定固定报头的索引（0） */
    index_fixed_header = 0;

    /* 先清空 MQTT 输出缓冲区 
    memset(MQTT_Tx_Buffer, 0, MQTT_TX_BUFFER_SIZE);
    MQTT_Tx_Length = 0;*/

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的固定报头部分 */
    memcpy(MQTT_Tx_Buffer + index_fixed_header, fixed_header, size_fixed_header);

    /* 🗂️确定可变报头的索引（=固定报头的长度） */
    index_variable_header = size_fixed_header;

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的可变报头部分 */
    memcpy(MQTT_Tx_Buffer + index_variable_header, variable_header, size_variable_header);

    /* 🗂️确定有效载荷的索引（= 可变报头的索引 + 可变报头的长度(2)） */
    index_payload = index_variable_header + size_variable_header;

    /* ✍️向 MQTT 输出缓冲区写入  SUBSCRIBE 报文的有效载荷部分 */
    {
        /* 写入 Topic Filter 的长度前缀*/
        /* topicFilter 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_payload + 0] = (uint8_t)(strlen(topicFilter) >> 8);
        /* topicFilter 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_payload + 1] = (uint8_t)(strlen(topicFilter));

        index_payload += 2; // 索引前移

        /* 写入 Topic Filter */
        memcpy(MQTT_Tx_Buffer + index_payload, topicFilter, strlen(topicFilter));
    }

    /* 最后修改输出缓冲区的大小 */
    MQTT_Tx_Length = size_fixed_header + size_variable_header + size_payload;

    return true;
}

/**
 * @brief 构建 MQTT PUBLISH 报文，并将报文写入 MQTT 输出缓冲区
 *
 * @param topicName 主题名
 * @param message 要发布的消息（json 格式）
 * @param qos QoS 等级
 *
 * @retval true 构建成功
 * @retval false 构建失败
 *
 * @note 💡QoS=0 时，没有响应报文、没有 Packet Identifier 字段、首字节为 0x30
 */
bool __MQTT_Build_Publish_Packet(const char *topicName, const char *message, uint8_t QoS)
{
    uint8_t index_fixed_header = 0;    // 固定报头的索引
    uint8_t index_variable_header = 0; // 可变报头的索引
    uint8_t index_payload = 0;         // 有效载荷部分的索引

    uint8_t size_fixed_header = 0;    // 固定报头部分的长度
    uint8_t size_payload;             // 有效载荷部分的长度
    uint8_t size_variable_header = 0; // 可变报头部分的长度

    /* 💡为作区分，在本函数中，「length」 只作为剩余长度字段的名称，
        真正的「长度」变量都使用 「size」进行命名 */
    uint8_t remaining_length;          // 剩余长度
    uint8_t size_remaining_length = 0; // 「剩余长度」字段的长度（占几个字节）

    /* 固定报头部分（2 ~ 5 字节） */
    char fixed_header[5] = {0};

    /* 可变报头部分（当 Qos=0 时）为：2 字节长度前缀 + Topic Name */
    /* 计算可变报头部分的长度 */
    size_variable_header = 2 + strlen(topicName);

    /* 有效载荷部分为：message（json 字符串） */
    /* 计算有效载荷部分的长度 */
    size_payload = strlen(message);

    /* 计算剩余长度（可变报头的长度 + 有效载荷的长度） */
    remaining_length = size_variable_header + size_payload;

    /* ⚒️构建固定报头部分 */
    {
        /* 第一个字节为 0x30（高 4 位：第 3 种报文；低 4 位：与 QoS 有关） */
        if (QoS == 0)
        {
            fixed_header[0] = 0x30;
        }
        else if (QoS == 1)
        {
            fixed_header[0] = 0x32;
        }
        else if (QoS == 2)
        {
            fixed_header[0] = 0x34;
        }
        else // 非法 QoS 值
        {
            return false;
        }

        /* 计算剩余长度的长度（占几个字节），并写入 fixed_header */
        size_remaining_length = __Size_Of_Remaining_Length(remaining_length, fixed_header);
    }

    /* ⚒️构建可变报头部分 就是 长度前缀 + Topic Name，不需要构建 */

    /* 固定报头的长度 =「剩余长度」字段的大小 +1（固定报头的第一个字节） */
    size_fixed_header = size_remaining_length + 1;

    /* 🗂️确定固定报头的索引（0） */
    index_fixed_header = 0;

    /* 先清空 MQTT 输出缓冲区 
    memset(MQTT_Tx_Buffer, 0, MQTT_TX_BUFFER_SIZE);
    MQTT_Tx_Length = 0;*/

    /* ✍️向 MQTT 输出缓冲区写入  PUBLISH 报文的固定报头部分 */
    memcpy(MQTT_Tx_Buffer + index_fixed_header, fixed_header, size_fixed_header);

    /* 🗂️确定可变报头的索引（=固定报头的长度） */
    index_variable_header = size_fixed_header;

    /* ✍️向 MQTT 输出缓冲区写入  PUBLISH 报文的可变报头部分 */
    {
        /* 写入 Topic Name 的长度前缀 */
        /* Topic Name 长度的最高有效字节（MSB）（高 8 位） */
        MQTT_Tx_Buffer[index_variable_header + 0] = (uint8_t)(strlen(topicName) >> 8);
        /* Topic Name 长度的最低有效字节（LSB）（低 8 位） */
        MQTT_Tx_Buffer[index_variable_header + 1] = (uint8_t)(strlen(topicName) & 0xFF);

        index_variable_header += 2; // 索引前移

        /* 写入 Topic Name */
        memcpy(MQTT_Tx_Buffer + index_variable_header, topicName, strlen(topicName));

        index_variable_header += strlen(topicName); // 索引前移

        if (QoS == 0)
        {
            /* Qos 为 0 时，没有报文标识符 Packet Identifier */
        }
        else if (QoS == 1)
        {
            /* 写入 Packet Identifier */
        }
        else if (QoS == 2)
        {
            /* 写入 Packet Identifier */
        }
        else // 非法 QoS 值
        {
            return false;
        }
    }

    /* 🗂️确定有效载荷的索引（= 可变报头的索引 index_variable_header（索引在前面前移了）） */
    index_payload = index_variable_header;

    /* ✍️向 MQTT 输出缓冲区写入  PUBLISH 报文的有效载荷部分（messag） */
    memcpy(MQTT_Tx_Buffer + index_payload, message, strlen(message));

    /* 最后修改输出缓冲区的大小 */
    MQTT_Tx_Length = size_fixed_header + size_variable_header + size_payload;

    return true;
}

/**
 * @brief 返回剩余长度（Remaining Length）要占几个字节，把这几字节写入 fixed_header
 *        从 fixed_header[1] 开始写
 *
 * @param remaining_length 要计算的剩余长度
 * @param fixed_header 指向固定报头的指针（将写入剩余长度字段）
 *
 * @return uint8_t 要占的字节数
 */
uint8_t __Size_Of_Remaining_Length(uint8_t remaining_length, char *fixed_header)
{
    uint8_t size_remaining_length = 0;
    uint8_t current_byte = 0;
    while (remaining_length)
    {
        /* 先取出当前字节的有效数据(7 位) */
        current_byte = remaining_length & 0x7F;

        /* 然后右移 7 位 */
        remaining_length >>= 7;

        /* 如果 remaining_length 不为 0，则当前字节最高位（标志位）置 1，表示下一个字节也表示剩余长度字段 */
        if (remaining_length)
        {
            current_byte |= 0x80;
        }

        /* 写入 fixed_header */
        fixed_header[++size_remaining_length] = current_byte;
    }

    return size_remaining_length;
}

/**
 * @brief 把 MQTT 输出缓冲区的报文（通过 USART 发出去）
 */
void __MQTT_Send_Packet(void)
{
    uint8_t i = 0;
    while (i < MQTT_Tx_Length)
    {
        USART_SendByte(MQTT_USART, MQTT_Tx_Buffer[i++]);
    }
    while (USART_GetFlagStatus(MQTT_USART, USART_FLAG_TC) == RESET)
        ;
}
