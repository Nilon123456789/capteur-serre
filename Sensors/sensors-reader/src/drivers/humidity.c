/**
 * humidity.c
 * 
 * Driver for capacitive humidity sensor
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "humidity.h"

LOG_MODULE_REGISTER(HUMIDITY, CONFIG_HUMIDITY_LOG_LEVEL); /* Register the module for log */

static const struct device *humidity_adc = DEVICE_DT_GET(ADC_NODE); /* Get the ADC device */

static int16_t sample_buffer[BUFFER_SIZE] = {0}; /* Buffer for the samples */

static struct adc_channel_cfg channel_cfg = { /* Configuration of the ADC channel */
    .gain             = HUM_ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = HUM_CHANNEL_ID,
	.differential	  = 0,
	.input_positive   = HUM_ADC_PORT,
};

static const struct adc_sequence sequence = {
    .options	    = NULL,
    .channels	    = BIT(HUM_CHANNEL_ID),
    .buffer		    = sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
    .resolution	    = ADC_RESOLUTION,
    .oversampling	= 0,
    .calibrate	    = false,
};

static uint8_t ret;

static bool isInisialized = false;

/**
 * @brief Initalise the capacitive humidity sensor
 * 
 * @return int 0 if success, 1 if device not found, 
 * 2 if setup failed
*/
int humidity_init(void)
{

    LOG_INF("init");

    if(!humidity_adc) {
        LOG_ERR("device not found");
        return 1;
    }

    ret = adc_channel_setup(humidity_adc, &channel_cfg);
    if(ret) {
        LOG_ERR("setup failed (%d)", ret);
        return 2;
    }

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief Read the humidity value (0-100)
 * 
 * @param humidity Pointer to the humidity value
 * @return int 0 if success, 1 if device not initialized, 
 * 2 if read failed
*/
int humidity_read(float *humidity)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    ret = adc_read(humidity_adc, &sequence);
    if(ret) {
        LOG_WRN("read failed (%d)", ret);
        return 2;
    }

    *humidity = mapRange(sample_buffer[0], DRY_VAL, WET_VAL, 0, 100);

    LOG_DBG("Humidity raw: %d \t Humidity: %d.%d%%", sample_buffer[0], (int)*humidity, (int)(*humidity * 100) % 100);

    LOG_INF("read done");

    return 0;
}
