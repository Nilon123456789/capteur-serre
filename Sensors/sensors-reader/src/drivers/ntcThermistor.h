/**
 * ntcThermistor.h
 * 
 * Driver for NTC thermistors.
 * 
 * Author: Nils Lahaye 2023
 * 
*/
#ifndef NTC_THERMISTOR_H_
#define NTC_THERMISTOR_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include "../utils.h"

int ntc_init(void);

static void getTempC(int voltage, float *temperature);

int ntc_read(float *temperature);

#endif /* NTC_THERMISTOR_H_ */