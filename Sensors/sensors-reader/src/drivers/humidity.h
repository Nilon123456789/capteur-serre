/**
 * humidity.h
 * 
 * Driver for capacitive humidity sensor
 * 
 * Author: Nils Lahaye 2023
 * 
*/
#ifndef HUMIDITY_H_
#define HUMIDITY_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "../utils.h"

int humidity_init(void);

int humidity_read(float *humidity);

#endif /* HUMIDITY_H_ */