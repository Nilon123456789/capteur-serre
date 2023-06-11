/**
 * pt19.h
 * 
 * Driver for PT19 light sensor
 * 
 * Author: Nils Lahaye 2023
 * 
*/
#ifndef PT19_H_
#define PT19_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "../utils.h"

int pt19_init(void);

int pt19_read(float *intensity);

#endif /* PT19_H_ */