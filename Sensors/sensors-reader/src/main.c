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
#include "drivers/pt19.h"
#include "drivers/ntcThermistor.h"

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

int ret;

void main(void)
{
	float temperature, humidity, luminosity;

	ret = aht20_init();
	if(ret) {
		LOG_ERR("AHT20 init failed (%d)", ret);
		return;
	}

	ret = pt19_init();
	if(ret) {
		LOG_ERR("PT19 init failed (%d)", ret);
		return;
	}

	ret = ntc_init();
	if(ret) {
		LOG_ERR("NTC init failed (%d)", ret);
		return;
	}

	while(1) {
		ret = aht20_read(&temperature, &humidity);
		if(ret) {
			LOG_ERR("AHT20 read failed (%d)", ret);
			return;
		}
		LOG_INF("Temperature: %d.%dC, Humidity: %d.%d%%", (int) temperature, (int) (temperature * 100) % 100, (int) humidity, (int) (humidity * 100) % 100);

		ret = pt19_read(&luminosity);
		if(ret) {
			LOG_ERR("PT19 read failed (%d)", ret);
			return;
		}
		LOG_INF("Luminosity: %d.%d%%", (int) luminosity, (int) (luminosity * 100) % 100);

		ret = ntc_read(&temperature);
		if(ret) {
			LOG_ERR("NTC read failed (%d)", ret);
			return;
		}
		LOG_INF("Temperature: %d.%dC", (int) temperature, (int) (temperature * 100) % 100);

		k_sleep(K_MSEC(1000));
	}
}
