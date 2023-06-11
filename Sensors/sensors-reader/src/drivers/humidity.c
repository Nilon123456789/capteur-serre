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

static const struct adc_dt_spec hum_adc_spec = ADC_DT_SPEC_GET(DT_NODELABEL(ground_humidity));
static const struct gpio_dt_spec hum_enable_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(ground_humidity), power_gpios);

static const int dry_value[3] = DT_PROP(DT_NODELABEL(ground_humidity), dry);
static const int wet_value[3] = DT_PROP(DT_NODELABEL(ground_humidity), wet);

static int16_t sample_buffer;
static struct adc_sequence sequence = {
    .buffer		    = &sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
};

static bool isInisialized = false;

/**
 * @brief Initalise the capacitive humidity sensor
 * 
 * @return int 0 if success, error code otherwise
*/
int humidity_init(void)
{
    if(isInisialized) {
        LOG_WRN("device already initialized");
        return 0;
    }

    LOG_INF("init");

    RET_IF_ERR(!device_is_ready(hum_adc_spec.dev), "ADC device not ready");
    RET_IF_ERR(adc_channel_setup_dt(&hum_adc_spec), "ADC channel setup failed");
    RET_IF_ERR(!device_is_ready(hum_enable_spec.port), "GPIO device not ready");
    RET_IF_ERR(gpio_pin_configure_dt(&hum_enable_spec, GPIO_OUTPUT), "GPIO pin configuration failed");

    /* Disable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&hum_enable_spec, 0), "GPIO pin set failed");

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief Read the humidity value (0-100)
 * 
 * @param humidity Pointer to the humidity value
 * @return int 0 if success, error code otherwise
*/
int humidity_read(float *humidity)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    LOG_INF("read");

    /* Enable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&hum_enable_spec, 1), "GPIO pin set failed");
    k_sleep(K_MSEC(40)); /* Wait for the sensor to be ready */

    /* Read the value */
    RET_IF_ERR(adc_sequence_init_dt(&hum_adc_spec, &sequence), "ADC sequence init failed");
    RET_IF_ERR(adc_read(hum_adc_spec.dev, &sequence), "ADC read failed");

    /* Disable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&hum_enable_spec, 0), "GPIO pin set failed");

    /* Convert the value to a percentage */
    float dry = evaluate_polynomial(0, dry_value); // TODO : replace 0 by batterie voltage
    float wet = evaluate_polynomial(0, wet_value); // TODO : replace 0 by batterie voltage

    *humidity = mapRange(sample_buffer, dry, wet, 0, 100);

    LOG_DBG("Humidity raw: %d \t Humidity: %d.%d%%", sample_buffer, (int)*humidity, (int)(*humidity * 100) % 100);

    LOG_INF("read done");

    return 0;
}
