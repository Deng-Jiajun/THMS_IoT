#ifndef __TEST_H
#define __TEST_H
#include "sys.h"

void TEST_LED(void);
void TEST_USART1(void);
void TEST_Redirect_Printf(void);
void TEST_KEY(void);
void TEST_OLED(void);
void TEST_DHT11(void);
void TEST_ASSERT_FAIL(void);
void TEST_IIC(void);
void TEST_WiFi(void);
void TEST_utils_hmac_sha1(void);
void TEST_MQTT_CONNECT(void);
void TEST_MQTT_BUILD_PACKET(void);
void TEST_MQTT(void);
void TEST_MQTT_PUBLISH_Temperature_and_Humidity(void);
void TEST_String_Concatenation(void);

#endif
