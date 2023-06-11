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

static const struct adc_dt_spec pt19_adc_spec = ADC_DT_SPEC_GET(DT_NODELABEL(pt19));
static const struct gpio_dt_spec pt19_enable_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(pt19), power_gpios);

static int16_t sample_buffer;
static struct adc_sequence sequence = {
    .buffer		    = &sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
};

static bool isInisialized = false;

/**
 * @brief Initalise the PT19 sensor
 * 
 * @return int 0 if success, error code otherwise
*/
int pt19_init(void)
{
    if(isInisialized) {
        LOG_WRN("device already initialized");
        return 0;
    }

    LOG_INF("init");

    RET_IF_ERR(!device_is_ready(pt19_adc_spec.dev), "ADC device not ready");
    RET_IF_ERR(adc_channel_setup_dt(&pt19_adc_spec), "ADC channel setup failed");
    RET_IF_ERR(!device_is_ready(pt19_enable_spec.port), "GPIO device not ready");
    RET_IF_ERR(gpio_pin_configure_dt(&pt19_enable_spec, GPIO_OUTPUT), "GPIO pin configuration failed");

    /* Disable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&pt19_enable_spec, 0), "GPIO pin set failed");

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief Read the intensity of the light (0-100)
 * 
 * @param intensity Pointer to the variable where the intensity will be stored
 * @return int 0 if success, error code otherwise
*/
int pt19_read(float *intensity)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    LOG_INF("read");

    RET_IF_ERR(gpio_pin_set_dt(&pt19_enable_spec, 1), "GPIO pin set failed");
    k_sleep(K_MSEC(10));

    RET_IF_ERR(adc_sequence_init_dt(&pt19_adc_spec, &sequence), "ADC sequence init failed");
    RET_IF_ERR(adc_read(pt19_adc_spec.dev, &sequence), "ADC read failed");

    RET_IF_ERR(gpio_pin_set_dt(&pt19_enable_spec, 0), "GPIO pin set failed");

    *intensity = mapRange(sample_buffer, 0, 1023, 0, 100);

    LOG_DBG("Intensity raw: %d \t Intensity: %d.%d%%", sample_buffer, (int)*intensity, (int)(*intensity * 100) % 100);

    LOG_INF("read done");

    return 0;
}

