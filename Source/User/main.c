#include "sys.h"

int main(void)
{
    /* 温湿度数据 */
    uint8_t temperature = 0;
    uint8_t humidity = 0;

    /* Topic Name */
    const char *topic_name = "/sys/h6wxLg1Xd3V/DHT11_01/thing/event/property/post";

    /* 包含温湿度的 json 字符串 */
    char message_json[1024] = {0};

    /* 各种模块初始化 */
    delay_init();
    DHT11_Init();
    NVIC_Config();
    USART1_Init();
    USART2_Init();
    WiFi_Init();

    /* 连接 MQTT 服务器 */
    MQTT_CONNECT("DHT11_01", "DHT11_01", "h6wxLg1Xd3V", "7f004d6f1724fd41ffc67a8d5a61236d", 3, "hmacsha1");

    while (1)
    {
        /* 获取温湿度数据 */
        DHT11_Read_Data(&temperature, &humidity);

        /* 构建 message_json 字符串（publish 报文的有效载荷） */
        sprintf(message_json, "{\"id\":\"1234\",\"version\":\"1.0\",\"method\":\"thing.event.property.post\",\"params\":{\"CurrentHumidity\":%d,\"CurrentTemperature\":%d}}", humidity, temperature);

        /* 上传温湿度数据 */
        if (MQTT_PUBLISH(topic_name, message_json, 0)) // 发送报文后会有 5 秒的等待时间
        {
            printf("Publish success(temperature: %d, humidity: %d)\r\n", temperature, humidity);
        }

        /* 延时 5 秒 */
        delay_ms(1800);
        delay_ms(1800);
        delay_ms(1400);
    }
}
