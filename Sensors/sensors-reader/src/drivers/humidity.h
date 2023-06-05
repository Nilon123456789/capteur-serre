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
#include <zephyr/logging/log.h>
#include "../utils.h"

#define ADC_NODE    DT_NODELABEL(adc) // Get ADC node

#define ADC_RESOLUTION          10 // ADC resolution
#define HUM_CHANNEL_ID              2 // ADC channel ID
#define HUM_ADC_PORT                SAADC_CH_PSELP_PSELP_AnalogInput2 // ADC port
#define HUM_ADC_GAIN                ADC_GAIN_1_4 // ADC gain
#define ADC_REFERENCE           ADC_REF_INTERNAL // ADC reference
#define ADC_ACQUISITION_TIME    ADC_ACQ_TIME_DEFAULT // ADC acquisition time
#define BUFFER_SIZE             1 // ADC buffer size

#define DRY_VAL 1005 // Dry value
#define WET_VAL 825 // Wet value

int humidity_init(void);

int humidity_read(float *humidity);

#endif /* HUMIDITY_H_ */