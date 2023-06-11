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
#include "drivers/humidity.h"
#include "drivers/batterie.h"
#include "utils.h"

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

int ret;

void main(void)
{
	float temperature, humidity, luminosity, charge;

	RET_IF_ERR(aht20_init(), "AHT20 init failed");

	RET_IF_ERR(pt19_init(), "PT19 init failed");

	RET_IF_ERR(ntc_init(), "NTC init failed");

	RET_IF_ERR(humidity_init(), "Humidity init failed");

	RET_IF_ERR(batterie_init(), "Batterie init failed");

	while(1) {
		RET_IF_ERR(aht20_read(&temperature, &humidity), "AHT20 read failed");
		LOG_INF("Temperature: %d.%dC, Humidity: %d.%d%%", (int) temperature, (int) (temperature * 100) % 100, (int) humidity, (int) (humidity * 100) % 100);

		RET_IF_ERR(pt19_read(&luminosity), "PT19 read failed");
		LOG_INF("Luminosity: %d.%d%%", (int) luminosity, (int) (luminosity * 100) % 100);

		RET_IF_ERR(ntc_read(&temperature), "NTC read failed");
		LOG_INF("Temperature: %d.%dC", (int) temperature, (int) (temperature * 100) % 100);

		RET_IF_ERR(humidity_read(&humidity), "Humidity read failed");
		LOG_INF("Humidity: %d.%d%%", (int) humidity, (int) (humidity * 100) % 100);

		RET_IF_ERR(batterie_read(&charge), "Batterie read failed");
		LOG_INF("Batterie: %d.%d%%", (int) charge, (int) (charge * 100) % 100);

		k_sleep(K_MSEC(1000));
	}
}
