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
#include <zephyr/logging/log.h>
#include "../utils.h"

#define ADC_NODE    DT_NODELABEL(adc) // Get ADC node

#define ADC_RESOLUTION          10 // ADC resolution
#define CHANNEL_ID              0 // ADC channel ID
#define ADC_PORT                SAADC_CH_PSELP_PSELP_AnalogInput0 // ADC port
#define ADC_GAIN                ADC_GAIN_1_4 // ADC gain
#define ADC_REFERENCE           ADC_REF_INTERNAL // ADC reference
#define ADC_ACQUISITION_TIME    ADC_ACQ_TIME_DEFAULT // ADC acquisition time
#define BUFFER_SIZE             1 // ADC buffer size

int pt19_init(void);

int pt19_read(float *intensity);

#endif /* PT19_H_ */