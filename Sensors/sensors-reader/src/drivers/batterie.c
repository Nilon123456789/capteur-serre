/**
 * batterie.c
 * 
 * Driver for reading the current batterie level
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "batterie.h"

LOG_MODULE_REGISTER(BATTERIE, CONFIG_BATTERIE_LOG_LEVEL); /* Register the module for log */

static const struct adc_dt_spec bat_adc_spec = ADC_DT_SPEC_GET(DT_NODELABEL(battery_voltage));

static const float full_voltage = 3.0f;
static const float empty_voltage = 2.0f;

static int16_t sample_buffer;
static struct adc_sequence sequence = {
    .buffer		    = &sample_buffer,
    .buffer_size	= sizeof(sample_buffer),
};

static bool isInisialized = false;

/**
 * @brief Initalise the batterie voltage monitor
 * 
 * @return int 0 if success, error code otherwise
*/
int batterie_init(void)
{
    if(isInisialized) {
        LOG_WRN("device already initialized");
        return 0;
    }

    LOG_INF("init");

    RET_IF_ERR(!device_is_ready(bat_adc_spec.dev), "ADC device not ready");
    RET_IF_ERR(adc_channel_setup_dt(&bat_adc_spec), "ADC channel setup failed");

    isInisialized = true;

    LOG_INF("init done");

    return 0;
}

/**
 * @brief Read the voltage of the batterie
 * 
 * @param voltage Pointer to the voltage value
 * @return int 0 if success, error code otherwise
*/
int batterie_voltage(float *voltage) {

    if(!isInisialized) {
        LOG_ERR("device not initialized");
        return 1;
    }

    LOG_INF("read");

    /* Read the value */
    RET_IF_ERR(adc_sequence_init_dt(&bat_adc_spec, &sequence), "ADC sequence init failed");
    RET_IF_ERR(adc_read(bat_adc_spec.dev, &sequence), "ADC read failed");

    /* Convert the reading to volts */
    uint32_t milvolt = sample_buffer;
    RET_IF_ERR(adc_raw_to_millivolts_dt(&bat_adc_spec, &milvolt), "ADC raw to millivolts failed");
    *voltage = milvolt / 1000.0f;

    LOG_DBG("Raw: %d \t Millivolts: %d \t Voltage: %d.%dV", sample_buffer, milvolt, (int)volts, (int)(volts * 100) % 100);

    LOG_INF("read done");

    return 0;
}

/**
 * @brief Read the charge level of the batterie (not accurate)
 * 
 * @param charge Pointer to the charge value
 * @return int 0 if success, error code otherwise
*/
int batterie_read(float *charge)
{

    float volts;
    RET_IF_ERR(batterie_voltage(&volts), "Batterie voltage read failed");

    *charge = mapRange(volts, empty_voltage, full_voltage, 0, 100);

    return 0;
}
