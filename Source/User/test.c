#include "sys.h"

/**
 * @brief LED 测试函数
 *
 * @note 预期效果：
 *       LED1 打开，0.5秒后 LED2 打开，0.5秒后 LED1 关闭，0.5秒后 LED2 关闭，
 *       0.5秒后 LED1 和 LED2 同时打开，0.5秒后 LED1 和 LED2 同时关闭
 */
void TEST_LED(void)
{
    /* 只有两个灯，搞不了流水灯 */

    /* delay 初始化 */
    delay_init();

    /* LED 初始化 */
    LED_Init();

    /* 开启 LED1 */
    LED_On(1);
    delay_ms(500);

    /* 开启 LED2 */
    LED_On(2);
    delay_ms(500);

    /* 关闭 LED1 */
    LED_Off(1);
    delay_ms(500);

    /* 关闭 LED2 */
    LED_Off(2);
    delay_ms(500);

    /* 同时开启 LED1 和 LED2 */
    LED_Toggle(1);
    LED_Toggle(2);

    delay_ms(500);

    /* 同时关闭 LED1 和 LED2 */
    LED_Toggle(1);
    LED_Toggle(2);
}

/* 按键按下标志位 */
extern uint8_t flag;
/**
 * @brief 按键测试函数
 * @note 预期效果：按下 KEY1，放开后 LED1 亮 2 秒后关闭
 */
void TEST_KEY(void)
{
    /*  delay 初始化 */
    delay_init();

    /* 初始化 LED */
    LED_Init();

    /* 初始化 delay */
    delay_init();

    /* 初始化按键 */
    KEY_GPIO_Config();

    while (1)
    {
        /* 按键扫描 */
        KEY_Scan();
        /* 如果按键按下 */
        if (flag == 1)
        {
            /* 点亮 LED1 2秒 */
            LED_On(1);
            delay_ms(500);
            delay_ms(500);
            delay_ms(500);
            delay_ms(500);

            /* 然后关闭 LED1 */
            LED_Off(1);

            flag = 0;
        }
    }
}

uint8_t USART_ReceieByte(USART_TypeDef *USARTx);
/**
 * @brief USART 测试函数
 * @note 预期效果：串口接收到数据后（通过串口）原样发出
 */
void TEST_USART1(void)
{
    char temp;
    NVIC_Config();
    USART1_Init();

    USART1_Printf("print string by USART1_Printf()\r\n");
    USART1_Printf("print int: %d by printf()\r\n", 123);
    USART1_Printf("print float: %f by printf()\r\n", 123.456);

    while (1)
    {
        // scanf("%c",&temp);
        temp = USART_ReceieByte(USART1);
        // USART_SendByte(USART1, temp);
        printf("%c", temp);
    }
}

/**
 * @brief printf 重定向测试函数（重定向到 USART1）
 * @note 预期效果：每秒发送两行字符串
 */
void TEST_Redirect_Printf(void)
{
    int count = 0;

    /*  delay 初始化 */
    delay_init();

    /* USART1 配置（初始化） */
    USART1_Init();
    while (1)
    {
        printf("Hello World!\r\n");
        printf("count = %d\r\n", count++);
        delay_ms(1000);
    }
}

/**
 * @brief OLED 测试函数
 * @note 预期效果：显示三行文字
 */
void TEST_OLED(void)
{
    int count = 0;

    /*  delay 初始化 */
    delay_init();

    /* OLED 初始化 */
    OLED_Init();

    /* OLED 清屏 */
    OLED_Clear();
    OLED_ShowStr(0, 0, "Hello World!");
    OLED_ShowStr(1, 0, "count: ");

    /* 中文显示测试 */
    OLED_ShowChineseChar(2, 0, 0); /* "中", 0 */
    OLED_ShowChineseChar(2, 1, 1); /* "文", 1 */
    OLED_ShowChineseChar(2, 2, 2); /* "显", 2 */
    OLED_ShowChineseChar(2, 3, 3); /* "示", 3 */
    OLED_ShowChineseChar(2, 4, 4); /* "测", 4 */
    OLED_ShowChineseChar(2, 5, 5); /* "试", 5 */
    OLED_ShowChineseChar(2, 6, 6); /* "！", 6 */

    OLED_ShowStr(3, 0, "abcdefgh12345678");

    while (count != 10)
    {
        OLED_ShowNum(1, 7, count++);
        delay_ms(500);
    }
    /* OLED 清屏 */
    OLED_Clear();

    /* 数字显示测试 */
    OLED_ShowNum(0, 0, 0);
    OLED_ShowNum(0, 1, 123456789);
    OLED_ShowNum(1, 0, -2147483647);
    OLED_ShowNum(2, 0, INT32_MIN);
    OLED_ShowNum(3, 0, INT32_MAX);

    while (count != 30)
    {
        OLED_ShowNum(1, 14, count++);
        delay_ms(500);
    }
    /* OLED 清屏 */
    OLED_Clear();
}

/**
 * @brief DHT11 测试函数
 * @note 预期结果：串口 和 OLED 显示温湿度
 */
void TEST_DHT11(void)
{
    uint8_t temper;
    uint8_t humi;

    /*  delay 初始化 */
    delay_init();

    /* USART1 配置（初始化） */
    USART1_Init();

    /* DHT11 初始化 */
    DHT11_Init();

    /* OLED 初始化 */
    OLED_Init();
    /* OLED 清屏 */
    OLED_Clear();

    OLED_ShowStr(0, 0, "temperature: ");
    OLED_ShowStr(1, 0, "humidity: ");

    while (1)
    {
        if (DHT11_Read_Data(&temper, &humi))
        {
            printf("temperature is %d\r\n", temper);
            printf("humidity is %d\r\n", humi);
            OLED_ShowNum(0, 13, temper);
            OLED_ShowNum(1, 10, humi);
        }
        else
        {
            printf("temp != 0\r\n");
        }

        /* 读取间隔不小于 1 秒 */
        delay_ms(1500);
    }
}

/**
 * @brief 参数检查测试函数
 * @note 预期结果：串口输出错误提示信息
 */
void TEST_ASSERT_FAIL(void)
{
    /*  delay 初始化 */
    delay_init();

    /* USART1 配置（初始化） */
    USART1_Init();

    /* printf 输出测试 */
    printf("----- test assert fail -----\r\n");

    /* 打开一个非法地址的外设的时钟 */
    RCC_APB2PeriphClockCmd(0x10200000, ENABLE);

    /* 报错信息👇 */
    // Wrong parameter svalue: file ..\Source\Libraries\STM32F10x_StdPeriph_Driver\src\stm32f10x_rcc.c on line 1098
    /* 没有预想中的提示在 test.c 的某一行，但是可以知道是外设地址出错 */

    /* 这个可以指定在具体的位置👇 */
    // ! 想看到这句的报错需要注释上面那句，因为 assert_failed 会执行死循环
    assert_param(IS_BOOL_TYPE_PARAM(12345));
    /* 说明 assert_failed 只能指定到具体是哪句参数判断的语句引发的报错 */
}

/**
 * @brief IIC 测试函数
 * 由于 IIC 本身内容不多，所以只测试能不能找到 IIC 总线上的 OLED(从机地址 0x3C)
 * @note 预期结果：串口输出 0x3C ACK，其他两个为 NACK
 */
void TEST_IIC(void)
{
    delay_init();
    USART1_Init();
    IIC_Init();

    if (IIC_Check_Address(0x3C) == IIC_ACK)
        printf("IICAddress 0x3C ACK\r\n");
    else
        printf("IICAddress 0x3C NACK\r\n");

    if (IIC_Check_Address(0x3D) == IIC_ACK)
        printf("IICAddress 0x3D ACK\r\n");
    else
        printf("IICAddress 0x3D NACK\r\n");

    if (IIC_Check_Address(0x3E) == IIC_ACK)
        printf("IICAddress 0x3E ACK\r\n");
    else
        printf("IICAddress 0x3E NACK\r\n");

    while (1)
    {
    }
}

/**
 * @brief 测试向 WiFi 模块发送 AT 指令（通过串口 USART2）
 *
 */
void TEST_WiFi(void)
{
    delay_init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();
    WiFi_Init();

    /* 发送数据 */
    USART2_Printf("I'm client x1\r\n");
    delay_ms(100);
    USART2_Printf("I'm client x2\r\n");
    delay_ms(100);
    USART2_Printf("I'm client x3\r\n");
    delay_ms(100);

    /* 手动退出透传模式 */
    USART2_Printf("+++");
    delay_ms(1000);
    USART2_Printf("AT+CIPMODE=0\r\n");
}

/**
 * @brief 测试函数 utils_hmac_sha1
 * @note 预期结果：串口输出 sha1 加密后的字符串
 */
void TEST_utils_hmac_sha1(void)
{
    /* clientId deviceName productKey */
    char password[] = "clientIdDHT11_01deviceNameDHT11_01productKeyh6wxLg1Xd3V";
    int password_length = strlen(password);

    char password_encoded[1024] = {0};

    char device_secret[] = "7f004d6f1724fd41ffc67a8d5a61236d";
    int device_secret_length = strlen(device_secret);

    USART1_Init(); // 不要忘了开串口

    printf("TEST_utils_hmac_sha1\r\n");

    utils_hmac_sha1(password, password_length, password_encoded, device_secret, device_secret_length);

    printf("password_encoded length: %d\r\n", strlen(password_encoded));
    printf(password_encoded); /* a16dd043e157e8fc5f2d6f2edafcd37034931dea */
}

/**
 * @brief MQTT 测试函数(测试发送 CONNECT 报文 hardcode)
 *
 */
void TEST_MQTT_CONNECT(void)
{
    char connect_packet[] = {0x10, 0x76, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0xC2, 0x00, 0x64, 0x00, 0x2A, 0x44, 0x48, 0x54, 0x31, 0x31, 0x5F, 0x30, 0x31, 0x7C, 0x73, 0x65, 0x63, 0x75, 0x72, 0x65, 0x6D, 0x6F, 0x64, 0x65, 0x3D, 0x33, 0x2C, 0x73, 0x69, 0x67, 0x6E, 0x6D, 0x65, 0x74, 0x68, 0x6F, 0x64, 0x3D, 0x68, 0x6D, 0x61, 0x63, 0x73, 0x68, 0x61, 0x31, 0x7C, 0x00, 0x14, 0x44, 0x48, 0x54, 0x31, 0x31, 0x5F, 0x30, 0x31, 0x26, 0x68, 0x36, 0x77, 0x78, 0x4C, 0x67, 0x31, 0x58, 0x64, 0x33, 0x56, 0x00, 0x28, 0x61, 0x31, 0x36, 0x64, 0x64, 0x30, 0x34, 0x33, 0x65, 0x31, 0x35, 0x37, 0x65, 0x38, 0x66, 0x63, 0x35, 0x66, 0x32, 0x64, 0x36, 0x66, 0x32, 0x65, 0x64, 0x61, 0x66, 0x63, 0x64, 0x33, 0x37, 0x30, 0x33, 0x34, 0x39, 0x33, 0x31, 0x64, 0x65, 0x61};
    int connect_packet_length = sizeof(connect_packet);
    int i;

    delay_init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();

    if (WiFi_Init()) // 需要保证初始化成功
    {
        /* WiFi 初始化之后应该已经连接上了 阿里云的 MQTT 服务器，并进入透传模式 */

        /* 串口输入缓冲区清零 */
        memset(USART2_Rx_Buffer, 0, USART2_RX_BUFFER_SIZE);
        USART2_Rx_Length = 0;

        /* 手动发送 CONNECT 报文 */
        {
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
                ;
            for (i = 0; i < connect_packet_length; i++)
            {
                USART_SendData(USART2, connect_packet[i]);
                while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
                    ;
            }
        }

        /* 等 5 秒 */
        i = 5;
        while (i--)
        {
            delay_ms(1000);
            printf("%d\r\n", i);
        }

        /* 读取输出缓冲区内容 */
        if (USART2_Rx_Length)
        {
            printf("USART2_Rx_Length:%d\r\n", USART2_Rx_Length);

            if (USART2_Rx_Buffer[0] == 0x20 && USART2_Rx_Buffer[1] == 0x02 && USART2_Rx_Buffer[2] == 0x00 && USART2_Rx_Buffer[3] == 0x00)
            {
                printf("MQTT CONNECT ACK\r\n");
            }
            else
            {
                printf("MQTT CONNECT NACK\r\n");
            }

            printf("USART2_Rx_Buffer: ");
            for (i = 0; i < USART2_Rx_Length; i++)
            {
                printf("%02X ", USART2_Rx_Buffer[i]);
            }
            printf("\r\n%s\r\n", USART2_Rx_Buffer);
        }
        else
        {
            printf("No data\r\n");
        }
    }
}

/**
 * @brief  构建 MQTT 报文测试函数
 * 
 */
void TEST_MQTT_BUILD_PACKET(void)
{
    int i = 0;

    delay_init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();

    printf("connect packet:\r\n");
    if (__MQTT_Build_Connect_Packet("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1"))
    {
        i = 0;
        /* 看一下构建好的 CONNECT 报文*/
        while (i < MQTT_Tx_Length)
        {
            printf("%02X ", MQTT_Tx_Buffer[i++]);
        }
        printf("\r\n");
    }

    printf("subscribe packet:\r\n");
    if (__MQTT_Build_Subscribe_Packet("/sys/h6wxLg1Xd3V/DHT11_01/thing/service/property/set", 0))
    {
        i = 0;
        /* 看一下构建好的 SUBSCRIBE 报文*/
        while (i < MQTT_Tx_Length)
        {
            printf("%02X ", MQTT_Tx_Buffer[i++]);
        }
        printf("\r\n");
    }

    printf("unsubscribe packet:\r\n");
    if (__MQTT_Build_UnSubscribe_Packet("/sys/h6wxLg1Xd3V/DHT11_01/thing/service/property/set"))
    {
        i = 0;
        /* 看一下构建好的 UNSUBSCRIBE 报文*/
        while (i < MQTT_Tx_Length)
        {
            printf("%02X ", MQTT_Tx_Buffer[i++]);
        }
        printf("\r\n");
    }

    printf("publish packet:\r\n");
    if (__MQTT_Build_Publish_Packet("/sys/h6wxLg1Xd3V/DHT11_01/thing/event/property/post", "{\"id\":\"1234\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":{\"CurrentHumidity\":12.34,\"CurrentTemperature\":56.78}}", 0))
    {
        i = 0;
        /* 看一下构建好的 PUBLISH 报文*/
        while (i < MQTT_Tx_Length)
        {
            printf("%02X ", MQTT_Tx_Buffer[i++]);
        }
        printf("\r\n");
    }
}

/**
 * @brief 发送 MQTT 报文 测试函数
 */
void TEST_MQTT(void)
{
    int i = 0;

    delay_init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();

    if (WiFi_Init()) // WiFi 初始化成功
    {
        if (MQTT_CONNECT("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1")) // MQTT 建立连接成功
        {
            printf("MQTT CONNECT ACK\r\n");

            if (MQTT_PINGREQ()) // MQTT PING 成功
            {
                printf("MQTT PING ACK\r\n");

                if (MQTT_SUBSCRIBE("/sys/h6wxLg1Xd3V/DHT11_01/thing/service/property/set", 0)) // 订阅主题
                {
                    printf("MQTT SUBSCRIBE ACK\r\n");
                    if (MQTT_UNSUBSCRIBE("/sys/h6wxLg1Xd3V/DHT11_01/thing/service/property/set")) // 取消订阅主题
                    {
                        printf("MQTT UNSUBSCRIBE ACK\r\n");
                    }
                    else
                    {
                        printf("MQTT UNSUBSCRIBE NACK\r\n");
                    }
                }
                else
                {
                    printf("MQTT SUBSCRIBE NACK\r\n");
                }

                // {"id":"1234","version":"1.0","method":"thing.event.property.post","params":{"CurrentHumidity":12.34,"CurrentTemperature":56.78}}
                if (MQTT_PUBLISH("/sys/h6wxLg1Xd3V/DHT11_01/thing/event/property/post", "{\"id\":\"1234\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":{\"CurrentHumidity\":22.33,\"CurrentTemperature\":77.88}}", 0))
                {
                    printf("MQTT PUBLISH OK\r\n");
                }
            }
            else
            {
                printf("MQTT PING NACK\r\n");
            }
        }
        else
        {
            printf("MQTT CONNECT NACK\r\n");
        }
    }
}

/**
 * @brief 测试发送温湿度数据到服务器（通过 MQTT PUBLISH 报文）
 * 
 */
void TEST_MQTT_PUBLISH_Temperature_and_Humidity(void)
{
    uint8_t temperature = 0;
    uint8_t humidity = 0;

    /* Topic Name */
    const char *topic_name = "/sys/h6wxLg1Xd3V/DHT11_01/thing/event/property/post";

    /* 包含温湿度的 json 字符串 */
    char message_json[1024] = {0};

    delay_init();
    DHT11_Init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();

    printf("Hello World!\r\n");

    // while (1)
    {
        /* 获取温湿度数据 */
        while (!DHT11_Read_Data(&temperature, &humidity))
        {
            delay_ms(1500); // 读取间隔不小于 1 秒
        }

        /* 构建 message_json 字符串 */
        sprintf(message_json, "{\"id\":\"1234\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":{\"CurrentHumidity\":%d,\"CurrentTemperature\":%d}}", humidity, temperature);

        /* 看一下构建好的 messsage_json 字符串 */
        printf("%s\r\n", message_json);

        delay_ms(1500);
    }

    if (WiFi_Init()) // WiFi 初始化成功
    {
        printf("mqtt_connect\r\n");
        if (MQTT_CONNECT("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1")) // MQTT 建立连接成功
        {
            printf("MQTT CONNECT ACK\r\n");
            if (MQTT_PUBLISH(topic_name, message_json, 0)) // 发送温湿度数据到服务端
            {
                printf("MQTT PUBLISH success\r\n");
            }
            else
            {
                printf("MQTT PUBLISH failed\r\n");
            }
        }
        else
        {
            printf("MQTT CONNECT NACK\r\n");
        }
    }
    else
    {
        printf("WiFi init failed\r\n");
    }
}

void TEST_String_Concatenation(void)
{
    USART1_Init();
    {
       /* 拼接用于连接 AP 的 AT 指令 */
        char at_command_connect_ap[256]={0};
        sprintf(at_command_connect_ap, "AT+CWJAP_DEF=\"%s\",\"%s\"", AP_SSID, AP_PWD);
        printf("%s\r\n", at_command_connect_ap);
    }
    {
        /* 拼接用于建立 TCP 通信的 AT 指令 */
        char at_command_establish_tcp_connection[256]={0};
        sprintf(at_command_establish_tcp_connection, "AT+CIPSTART=\"TCP\",\"%s\",%d", TCP_SERVER_IP, TCP_SERVER_PORT);
        printf("%s\r\n", at_command_establish_tcp_connection);
    }
}
