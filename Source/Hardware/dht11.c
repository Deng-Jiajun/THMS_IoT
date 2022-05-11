#include "sys.h"

uint8_t DHT11_RST(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);



/**
 * @brief DTH11 复位（主机发出开始信号，等待 DHT11 响应）
 *
 * @return uint8_t 复位结果（是否收到 DHT11 的响应）
 *
 * @retval FAILED(0) 失败
 * @retval SUCCEED(1) 成功
 */
uint8_t DHT11_RST(void)
{
	// !这个持续时间的计算可能不太准，实测 80 μs 对应 cnt 大概在 30~35 之间
	/* 记录信号持续时间（单位μs） */
	uint8_t cnt = 0;

	/* 复位的结果（默认失败） */
	uint8_t result = FAILED;

	/* 主机（stm32）发送开始信号 */
	DHT11_OUT();	 // 配置引脚为输出模式
	DHT11_WRITE = 0; // 拉低电平（总线空闲状态为高电平）
	delay_ms(20);	 // 平持续至少 18ms
	DHT11_WRITE = 1; // 拉高电平
	delay_us(30);	 // 持续时间 20 ~ 40μs

	/* 主机等待接收 DHT 的响应信号（低电平 80μs，高电平 80μs） */
	DHT11_IN(); // 配置引脚为输入模式
	/* 等待 DHT 拉低电平（期望：低电平，持续 80μs） */
	while (!DHT11_READ && cnt < 100)
	{
		/* 记录低电平持续时间（单位μs） */
		cnt++;
		delay_us(1);
	};

	/* 如果 低电平持续时间超过 100μs或低于30μs，则认为 复位失败 */
	if (cnt >= 100 || cnt < 30)
	{
		result = FAILED;
	}
	else
	{
		cnt = 0; // 重新开始计时
		/* 等待 DHT 拉高电平（期望：高电平，持续 80μs） */
		while (DHT11_READ && cnt < 100)
		{
			/* 记录高电平持续时间（单位μs） */
			cnt++;
			delay_us(1);
		};

		/* 如果 高电平持续时间超过 100μs或低于30μs，则认为 复位失败 */
		if (cnt >= 100 || cnt < 30)
		{
			result = FAILED;
		}
		else
		{
			/* 否则，认为复位成功 */
			result = SUCCEED;
		}
	}
	/* 返回复位结果（是否收到 DHT11 的响应） */
	return result;
}

/**
 * @brief 从 DHT11 中读取 1 位数据
 *
 * @return uint8_t 读到的数据（1 bit）
 */
uint8_t DHT11_Read_Bit(void)
{
	/*
	'0'：低电平持续 50μs，高电平持续 26 ~ 28μs
	'1'：低电平持续 50μs，高电平持续 70μs
	*/

	/* 等 DHT11 拉低电平（开始传输数据） */
	while (DHT11_READ)
		;
	/* 等 DHT11 拉高电平 */
	while (!DHT11_READ)
		;
	/* DHT11 拉高电平后等待 35μs */
	delay_us(35);

	/* 再读一下电平状态，如果是低电平，则表示 '0'，否则表示 '1' */
	if (DHT11_READ)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief 从 DHT11 中读取 1 个字节的数据（8 位）
 * @note 其实就是循环调用 DHT11_Read_Bit() 函数，凑成 8 位
 *
 * @return uint8_t 读到的数据（1 byte）
 */
uint8_t DHT11_Read_Byte(void)
{
	uint8_t mask;
	uint8_t data = 0;

	/* DHT11 传输的数据 高位先出，所以从第 8 位（最高位）开始填 */
	for (mask = 0x80; mask != 0; mask >>= 1)
	{
		/* 读 1 位，写 1 位 */
		if (DHT11_Read_Bit() == 1)
		{
			data |= mask;
		}
		else
		{
			data &= ~mask;
		}
	}
	return data;
}

/**
 * 一次完整的数据传输为 40bit，高位先出。
 * 数据格式：8bit 湿度整数数据 + 8bit 湿度小数数据 +
 * 			 8bit 温度整数数据 + 8bit 温度小数数据 + 8bit 校验和
 *
 * 数据传送正确时，校验和数据等于「8bit 湿度整数数据 + 8bit 湿度小数数据 +
 * 				   8bit 温度整数数据 + 8bit 温度小数数据」所得结果的末 8 位
 */

/**
 * @brief 从 DHT11 中读取温湿度数据
 *
 * @param temperature 指向要返回的温度数据的指针（只包含整数部分）
 * @param humidity 指向要返回的湿度数据的指针（只包含整数部分）
 *
 * @return uint8_t
 * @retval FAILED(0) 失败
 * @retval SUCCEED(1) 成功
 */
uint8_t DHT11_Read_Data(uint8_t *temperature, uint8_t *humidity)
{
	uint8_t i;
	/* 保存读取到的数据 */
	uint8_t buf[5];

	/* 读取结果（默认失败） */
	uint8_t result = FAILED;

	/* 先复位 DHT11 */
	if (DHT11_RST() == SUCCEED)
	{
		/* 读 5 次 8bit 数据， 共 40bit，5 个 byte */
		for (i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();
		}

		/* 检验校验和，因为数据类型就是 8 位的，所以会直接截取和的末尾 8 位 */
		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			/* 第 0 块是湿度的整数部分 */
			*humidity = buf[0];
			/* 第 2 块是温度的整数部分 */
			*temperature = buf[2];

			/* 成功读到数据 */
			result = SUCCEED;
		}
		else
		{
			/* 校验和错误，数据传输出错 */
			result = FAILED;
		}
	}
	else
	{
		/* 复位失败，未收到 DHT11 的响应 */
		result = FAILED;
	}
	return result;
}

/**
 * @brief DHT11 引脚初始化（PA5）
 *
 */
void DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(Get_RCC_APB2Periph_GPIOx(DHT11_PORT), ENABLE);

	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}
