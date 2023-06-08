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

#define ADC_NODE    DT_NODELABEL(adc) // Get ADC node

#define NTC_POWER_NODE DT_ALIAS(ground_temp) // Get thermistor power node

#define ADC_RESOLUTION          10 // ADC resolution
#define NTC_CHANNEL_ID              1 // ADC channel ID
#define NTC_ADC_PORT                SAADC_CH_PSELP_PSELP_AnalogInput1 // ADC port
#define NTC_ADC_GAIN                ADC_GAIN_1_5 // ADC gain
#define ADC_REFERENCE           ADC_REF_INTERNAL // ADC reference
#define ADC_ACQUISITION_TIME    ADC_ACQ_TIME_DEFAULT // ADC acquisition time
#define BUFFER_SIZE             1 // ADC buffer size

#define RESISTOR			10000 // 10kOhm

int ntc_init(void);

static void getTempC(int voltage, float *temperature);

int ntc_read(float *temperature);

#endif /* NTC_THERMISTOR_H_ */