/**
 * batterie.h
 * 
 * Driver for reading the current batterie level
 * 
 * Author: Nils Lahaye 2023
 * 
*/
#ifndef BATTERIE_H_
#define BATTERIE_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "../utils.h"

int batterie_init(void);

int batterie_voltage(float *voltage);

int batterie_read(float *charge);

#endif /* BATTERIE_H_ */