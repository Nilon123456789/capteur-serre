/**
 * pt19.c
 * 
 * Driver for the PT19 light sensor
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "pt19.h"

LOG_MODULE_REGISTER(PT19, CONFIG_PT19_LOG_LEVEL); /* Register the module for log */

static const struct device *pt19_adc = DEVICE_DT_GET(ADC_NODE); /* Get the ADC device */

static const struct gpio_dt_spec pt19_power_spec = GPIO_DT_SPEC_GET(PT19_POWER_NODE, gpios); /* PT19 power spec */

static int16_t sample_buffer[BUFFER_SIZE] = {0}; /* Buffer for the samples */

static struct adc_channel_cfg channel_cfg = { /* Configuration of the ADC channel */
    .gain             = PT_ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = PT_CHANNEL_ID,
	.differential	  = 0,
	.input_positive   = PT_ADC_PORT,
};

static const struct adc_sequence sequence = {
    .options	    = NULL,
    .channels	    = BIT(PT_CHANNEL_ID),
    .buffer		    = sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
    .resolution	    = ADC_RESOLUTION,
    .oversampling	= 0,
    .calibrate	    = false,
};

static uint8_t ret;

static bool isInisialized = false;

/**
 * @brief Initalise the PT19 sensor
 * 
 * @return int 0 if success, 1 if device not found, 
 * 2 if setup failed
*/
int pt19_init(void)
{

    LOG_INF("init");

    if(!pt19_adc) {
        LOG_ERR("device not found");
        return 1;
    }

    if(!device_is_ready(pt19_adc)) {
        LOG_ERR("device not ready");
        return 1;
    }

    if(!device_is_ready(pt19_power_spec.port)) {
        LOG_ERR("device not ready");
        return 1;
    }

    ret = gpio_pin_configure_dt(&pt19_power_spec, GPIO_OUTPUT);
    if(ret) {
        LOG_ERR("configure failed (%d)", ret);
        return 2;
    }

    ret = adc_channel_setup(pt19_adc, &channel_cfg);
    if(ret) {
        LOG_ERR("setup failed (%d)", ret);
        return 2;
    }

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief Read the intensity of the light (0-100)
 * 
 * @param intensity Pointer to the variable where the intensity will be stored
 * @return int 0 if success, 1 if device not initialized, 
 * 2 if read failed
*/
int pt19_read(float *intensity)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    ret = gpio_pin_set_dt(&pt19_power_spec, 1);
    if(ret) {
        LOG_ERR("set failed (%d)", ret);
        return 2;
    }

    ret = adc_read(pt19_adc, &sequence);
    if(ret) {
        LOG_WRN("read failed (%d)", ret);
        return 2;
    }

    ret = gpio_pin_set_dt(&pt19_power_spec, 0);
    if(ret) {
        LOG_ERR("set failed (%d)", ret);
        return 2;
    }

    *intensity = mapRange(sample_buffer[0], 0, 1023, 0, 100);

    LOG_DBG("Intensity raw: %d \t Intensity: %d.%d%%", sample_buffer[0], (int)*intensity, (int)(*intensity * 100) % 100);

    LOG_INF("read done");

    return 0;
}

