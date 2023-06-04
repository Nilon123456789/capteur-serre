#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>

#define AHT20_I2C_ADDR	0x38

#define AHT20_CMD_RESET		     	 0xBA /* Reset command */
#define AHT20_CMD_TRIGGER_MEASURE    0xAC /* Trigger measure command */
#define AHT20_TRIGGER_MEASURE_BYTE_0 0x33 /* Trigger measure command byte 0 */
#define AHT20_TRIGGER_MEASURE_BYTE_1 0x00 /* Trigger measure command byte 1 */
#define AHT20_CMD_GET_STATUS	     0x71 /* Get status command */
#define AHT20_CMD_INITIALIZE	     0xBE /* Initialize command */

#define I2C1_NODE DT_NODELABEL(i2c1)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C1_NODE);

void main(void)
{
	if(!device_is_ready(i2c_dev)) {
		printk("I2C device not ready\n");
		return;
	}

	uint8_t cmdBuff[4], dataBuff[7], ret;

	cmdBuff[0] = AHT20_CMD_RESET;
	ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
	if(ret) {
		printk("Error while resetting %d\n", ret);
		return;
	}

	cmdBuff[0] = AHT20_CMD_INITIALIZE;
	ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
	if(ret) {
		printk("Error while initializing %d\n", ret);
		return;
	}

	cmdBuff[0] = AHT20_CMD_GET_STATUS;
	ret = i2c_write_read(i2c_dev, AHT20_I2C_ADDR, cmdBuff, 1, dataBuff, 1);
	if(ret) {
		printk("Error while getting status %d\n", ret);
		return;
	}
	if(!dataBuff[3]) { /* Check if the sensor is calibrated */
		printk("Sensor not calibrated\n");
		cmdBuff[0] = AHT20_CMD_INITIALIZE;
		ret = i2c_write(i2c_dev, cmdBuff, 1, AHT20_I2C_ADDR);
		if(ret) {
			printk("Error while initializing %d\n", ret);
			return;
		}
		k_sleep(K_MSEC(10));
	}

	printk("Sensor calibrated\n");

	while(1) {
		cmdBuff[0] = AHT20_CMD_TRIGGER_MEASURE;
		cmdBuff[1] = AHT20_TRIGGER_MEASURE_BYTE_0;
		cmdBuff[2] = AHT20_TRIGGER_MEASURE_BYTE_1;

		ret = i2c_write(i2c_dev, cmdBuff, 3, AHT20_I2C_ADDR);
		if(ret) {
			printk("Error while triggering measure %d\n", ret);
			return;
		}

		k_sleep(K_MSEC(40));
		while (1) {
			ret = i2c_read(i2c_dev, dataBuff, 7, AHT20_I2C_ADDR);
			if(ret) {
				printk("Error while reading data %d\n", ret);
				return;
			}
			/* Check if the data is ready */
			if(dataBuff[7]) k_sleep(K_MSEC(5)); 
			else break;
		}
		uint32_t hum = dataBuff[1];
		hum <<= 8;
		hum |= dataBuff[2];
		hum <<= 4;
		hum |= dataBuff[3] >> 4;
		float humidity = ((float)hum * 100) / 0x100000;

		uint32_t temp = dataBuff[3] & 0x0F;
		temp <<= 8;
		temp |= dataBuff[4];
		temp <<= 8;
		temp |= dataBuff[5];
		float temperature = ((float)temp * 200 / 0x100000) - 50;

		uint8_t crc = crc8(dataBuff, 6, 0x31, 0xff, false);
		if(crc != dataBuff[6]) {
			printk("CRC error\n");
			return;
		}

		printk("Temperature: %d.%d C\n", (int) temperature, (int) (temperature * 10) % 10);
		printk("Humidity: %d.%d %%\n", (int) humidity, (int) (humidity * 10) % 10);
	}

}
