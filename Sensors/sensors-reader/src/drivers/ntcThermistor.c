/**
 * ntcThermistor.c
 * 
 * Driver for a ntc thermistor
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "ntcThermistor.h"

LOG_MODULE_REGISTER(NTC, CONFIG_NTC_LOG_LEVEL); /* Register the module for log */

static const struct device *ntc_adc = DEVICE_DT_GET(ADC_NODE); /* Get the ADC device */

static int16_t sample_buffer[BUFFER_SIZE] = {0}; /* Buffer for the samples */

static struct adc_channel_cfg channel_cfg = { /* Configuration of the ADC channel */
    .gain             = NTC_ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = NTC_CHANNEL_ID,
	.differential	  = 0,
	.input_positive   = NTC_ADC_PORT,
};

static const struct adc_sequence sequence = {
    .options	    = NULL,
    .channels	    = BIT(NTC_CHANNEL_ID),
    .buffer		    = sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
    .resolution	    = ADC_RESOLUTION,
    .oversampling	= 0,
    .calibrate	    = false,
};

static uint8_t ret;

static bool isInisialized = false;

/**
 * @brief Initalise the ntc sensor
 * 
 * @return int 0 if success, 1 if device not found, 
 * 2 if setup failed
*/
int ntc_init(void)
{

    LOG_INF("init");

    if(!ntc_adc) {
        LOG_ERR("device not found");
        return 1;
    }

    ret = adc_channel_setup(ntc_adc, &channel_cfg);
    if(ret) {
        LOG_ERR("setup failed (%d)", ret);
        return 2;
    }

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief calculate the temperature from the voltage
 * 
 * @param voltage The voltage to calculate the temperature from
 * @param temperature Pointer to the variable where the temperature will be stored
 * 
*/
static void getTempC(int voltage, float *temperature) {
	float resistance = (float)RESISTOR * (1023.0 / (float)voltage - 1.0);
	*temperature = log(resistance);
	*temperature = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * *temperature * *temperature ))* *temperature );
	*temperature = *temperature - 273.15; // Convert Kelvin to Celcius
}


/**
 * @brief Read the current temperature
 * 
 * @param temperature Pointer to the variable where the temperature will be stored
 * @return int 0 if success, 1 if device not initialized, 
 * 2 if read failed
*/
int ntc_read(float *temperature)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    ret = adc_read(ntc_adc, &sequence);
    if(ret) {
        LOG_WRN("read failed (%d)", ret);
        return 2;
    }

    getTempC(sample_buffer[0], temperature);

    LOG_DBG("Temperature raw: %d \t Temperature: %d.%dC", sample_buffer[0], (int)*temperature, (int)(*temperature * 100) % 100);

    LOG_INF("read done");

    return 0;
}

