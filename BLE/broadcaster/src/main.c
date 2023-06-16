/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "drivers/adc.h"
#include "drivers/aht20.h"
#include "drivers/ble.h"
#include "utils.h"

LOG_MODULE_REGISTER(MAIN, CONFIG_MAIN_LOG_LEVEL);

void main(void) {
	LOG_INF("Starting application");

	// Initialize the ADC driver
	RET_IF_ERR(adc_init(), "Unable to initialize ADC");
	// Initialize the AHT20 driver
	RET_IF_ERR(aht20_init(), "Unable to initialize AHT20");
	// Initialize the BLE driver
	RET_IF_ERR(ble_init(), "Unable to initialize BLE");

	float temp, hum, lum, gnd_temp, gnd_hum, bat;

	while(true) {
		// Read the temperature and humidity
		RET_IF_ERR(aht20_read(&temp, &hum), "Unable to read temperature and humidity");
		// Read the luminosity
		RET_IF_ERR(luminosity_read(&lum), "Unable to read luminosity");
		// Read the ground temperature
		RET_IF_ERR(ground_temperature_read(&gnd_temp), "Unable to read ground temperature");
		// Read the ground humidity
		RET_IF_ERR(ground_humidity_read(&gnd_hum), "Unable to read ground humidity");
		// Read the battery level
		RET_IF_ERR(battery_voltage_read(&bat), "Unable to read battery level");

		// Encode the data into the service data
		RET_IF_ERR(ble_encode_adv_data(&temp, &hum, &lum, &gnd_hum, &gnd_temp, &bat), "Unable to encode data");

		// Advertise the data
		RET_IF_ERR(ble_adv(), "Unable to advertise data");

		// Wait
		k_sleep(K_SECONDS(CONFIG_SENSOR_SLEEP_DURATION_SEC));
	}
}
