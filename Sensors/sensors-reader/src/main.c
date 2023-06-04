/**
 * main.c
 * 
 * Main file of the project
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "drivers/aht20.h"

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

int ret;

void main(void)
{
	float temperature, humidity;

	ret = aht20_init();
	if(ret) {
		LOG_ERR("AHT20 init failed (%d)", ret);
		return;
	}

	while(1) {
		ret = aht20_read(&temperature, &humidity);
		if(ret) {
			LOG_ERR("AHT20 read failed (%d)", ret);
			return;
		}
		LOG_INF("Temperature: %d.%dC, Humidity: %d.%d%%", (int) temperature, (int) (temperature * 100) % 100, (int) humidity, (int) (humidity * 100) % 100);
		k_sleep(K_MSEC(1000));
	}
}
