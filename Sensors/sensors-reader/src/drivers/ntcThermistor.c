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

static const struct adc_dt_spec temp_adc_spec = ADC_DT_SPEC_GET(DT_NODELABEL(ground_temperature));
static const struct gpio_dt_spec temp_enable_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(ground_temperature), power_gpios);

static const uint16_t RESISTOR = DT_PROP(DT_NODELABEL(ground_temperature), output_ohms);

static int16_t sample_buffer;
static struct adc_sequence sequence = {
    .buffer		    = &sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
};

static bool isInisialized = false;

/**
 * @brief Initalise the ntc sensor
 * 
 * @return int 0 if success, error code otherwise
*/
int ntc_init(void)
{
    if(isInisialized) {
        LOG_WRN("device already initialized");
        return 0;
    }

    LOG_INF("init");

    RET_IF_ERR(!device_is_ready(temp_adc_spec.dev), "ADC device not ready");
    RET_IF_ERR(adc_channel_setup_dt(&temp_adc_spec), "ADC channel setup failed");
    RET_IF_ERR(!device_is_ready(temp_enable_spec.port), "GPIO device not ready");
    RET_IF_ERR(gpio_pin_configure_dt(&temp_enable_spec, GPIO_OUTPUT), "GPIO pin configuration failed");

    /* Disable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&temp_enable_spec, 0), "GPIO pin set failed");

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
 * @return int 0 if success, error code otherwise
*/
int ntc_read(float *temperature)
{
    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    LOG_INF("read");

    /* Enable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&temp_enable_spec, 1), "GPIO pin set failed");
    k_sleep(K_MSEC(10));

    /* Read the sensor */
    RET_IF_ERR(adc_sequence_init_dt(&temp_adc_spec, &sequence), "ADC sequence init failed");
    RET_IF_ERR(adc_read(temp_adc_spec.dev, &sequence), "ADC read failed");

    /* Disable the sensor */
    RET_IF_ERR(gpio_pin_set_dt(&temp_enable_spec, 0), "GPIO pin set failed");

    getTempC(sample_buffer, temperature);

    LOG_DBG("Temperature raw: %d \t Temperature: %d.%dC", sample_buffer, (int)*temperature, (int)(*temperature * 100) % 100);

    LOG_INF("read done");

    return 0;
}

