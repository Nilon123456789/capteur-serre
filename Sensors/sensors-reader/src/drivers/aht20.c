/**
 * ath20.c
 * 
 * This file is used to define the AHT20 sensor constants and functions.
 * 
 * Author: Nils Lahaye 2023
 * 
*/

#include "aht20.h"

LOG_MODULE_REGISTER(AHT20, CONFIG_AHT20_LOG_LEVEL); /* Register the module for log */

static const struct device *i2c_dev; /* I2C device */

static bool isInitialized = false; /* Is the sensor initialized? */

static uint8_t cmdBuff[4]; /* Command buffer */
static uint8_t dataBuff[7]; /* Data buffer */
static uint8_t ret; /* Return value */
static uint32_t humidity_raw; /* Humidity raw value */
static uint32_t temperature_raw; /* Temperature raw value */

/**
 * @brief Initalise the AHT20 sensor on i2c bus 1
 * 
 * @return 0 on success, 1 if the i2c device is not ready, 
 * 2 if the reset failed, 3 if the initialization failed, 
 * 4 if the status check failed, 5 if the calibration failed
*/
int aht20_init()
{
    LOG_INF("init");

    i2c_dev = DEVICE_DT_GET(I2C1_NODE);
    if (!device_is_ready(i2c_dev))
    {
        LOG_ERR("I2C device not ready");
        return 1;
    }

    cmdBuff[0] = AHT20_CMD_RESET;
    ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
    if (ret)
    {
        LOG_ERR("reset failed (%d)", ret);
        return 2;
    }

    cmdBuff[0] = AHT20_CMD_INITIALIZE;
	ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
	if(ret) {
		LOG_ERR("initialization failed (%d)", ret);
		return 3;
	}

	cmdBuff[0] = AHT20_CMD_GET_STATUS;
	ret = i2c_write_read(i2c_dev, AHT20_I2C_ADDR, cmdBuff, 1, dataBuff, 1);
	if(ret) {
		LOG_ERR("status check failed (%d)", ret);
		return 4;
	}
	if(!dataBuff[3]) { /* Check if the sensor is calibrated */
		LOG_INF("Not calibrated, calibrating...");
		cmdBuff[0] = AHT20_CMD_INITIALIZE;
		ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
		if(ret) {
			LOG_ERR("Initialization failed (%d)", ret);
			return 5;
		}
		k_sleep(K_MSEC(10));
	}

    LOG_INF("Init done");

    isInitialized = true;

    return 0;
}

/**
 * @brief Read the temperature and humidity from the AHT20 sensor
 * 
 * @param temperature pointer to the variable where the temperature will be stored
 * @param humidity pointer to the variable where the humidity will be stored
 * 
 * @return 0 on success, 1 if the sensor is not initialized,
 * 2 if the trigger measure failed, 3 if the read failed,
 * 4 if the data is not ready
*/
int aht20_read(float *temperature, float *humidity)
{
    LOG_INF("Reading sensor");

    if (!isInitialized)
    {
        LOG_ERR("Not initialized");
        return 1;
    }

    cmdBuff[0] = AHT20_CMD_TRIGGER_MEASURE;
    cmdBuff[1] = AHT20_TRIGGER_MEASURE_BYTE_0;
    cmdBuff[2] = AHT20_TRIGGER_MEASURE_BYTE_1;
    ret = i2c_write(i2c_dev, cmdBuff, 3, AHT20_I2C_ADDR);
    if(ret) {
        LOG_WRN("Trigger measure failed (ret %d)", ret);
        return 2;
    }

    k_sleep(K_MSEC(40));
    while (1) {
        ret = i2c_read(i2c_dev, dataBuff, 7, AHT20_I2C_ADDR);
        if(ret) {
            LOG_WRN("Read failed (ret %d)", ret);
            return 3;
        }
        /* Check if the data is ready */
        if(dataBuff[7]) k_sleep(K_MSEC(5)); 
        else break;
    }

    humidity_raw = dataBuff[1];
    humidity_raw <<= 8;
    humidity_raw |= dataBuff[2];
    humidity_raw <<= 4;
    humidity_raw |= dataBuff[3] >> 4;
    *humidity = ((float)humidity_raw * 100) / 0x100000;

    temperature_raw = dataBuff[3] & 0x0F;
    temperature_raw <<= 8;
    temperature_raw |= dataBuff[4];
    temperature_raw <<= 8;
    temperature_raw |= dataBuff[5];
    *temperature = ((float)temperature_raw * 200 / 0x100000) - 50;

    uint8_t crc = crc8(dataBuff, 6, 0x31, 0xff, false);
    if(crc != dataBuff[6]) {
        LOG_WRN("CRC check failed (%02x != %02x)", crc, dataBuff[6]);
        return 4;
    }

    LOG_DBG("Raw data: %02x %02x %02x %02x %02x %02x %02x", 
        dataBuff[0], dataBuff[1], dataBuff[2], dataBuff[3], dataBuff[4], dataBuff[5], dataBuff[6]);
    LOG_DBG("Temperature raw: %d \t converted : %d.%dC", temperature_raw, (int) *temperature, (int) (*temperature * 10) % 10);
    LOG_DBG("Humidity raw: %d \t converted : %d.%d%%", humidity_raw, (int) *humidity, (int) (*humidity * 10) % 10);

    LOG_INF("Read done");

    return 0;
}