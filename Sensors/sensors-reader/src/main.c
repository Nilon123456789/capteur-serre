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
#include "drivers/adc.h"
#include "drivers/aht20.h"
#include "utils.h"

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

int ret;

void main(void)
{
	float temperature, humidity, luminosity, voltage;

	RET_IF_ERR(aht20_init(), "AHT20 init failed");

	RET_IF_ERR(adc_init(), "ADC init failed");

	while(1) {
		RET_IF_ERR(aht20_read(&temperature, &humidity), "AHT20 read failed");
		LOG_INF("Temperature: %d.%dC, Humidity: %d.%d%%", (int) temperature, (int) (temperature * 100) % 100, (int) humidity, (int) (humidity * 100) % 100);

		RET_IF_ERR(luminosity_read(&luminosity), "Luminosity read failed");
		LOG_INF("Luminosity: %d.%d%%", (int) luminosity, (int) (luminosity * 100) % 100);

		RET_IF_ERR(ground_temperature_read(&temperature), "Ground temperature read failed");
		LOG_INF("Temperature: %d.%dC", (int) temperature, (int) (temperature * 100) % 100);

		RET_IF_ERR(ground_humidity_read(&humidity), "Ground humidity read failed");
		LOG_INF("Humidity: %d.%d%%", (int) humidity, (int) (humidity * 100) % 100);

		RET_IF_ERR(battery_voltage_read(&voltage), "Batterie read failed");
		LOG_INF("Batterie: %d.%dV", (int) voltage, (int) (voltage * 100) % 100);

		k_sleep(K_MSEC(1000));
	}
}
