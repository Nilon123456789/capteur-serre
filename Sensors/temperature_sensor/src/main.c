#include <zephyr/kernel.h> 
#include <zephyr/init.h>
#include <zephyr/drivers/adc.h>
#include <math.h>

#define ADC_NODE		DT_NODELABEL(adc)

#define ADC_RESOLUTION		10
#define CHANNEL_ID 			1
#define ADC_PORT			SAADC_CH_PSELP_PSELP_AnalogInput1 // P0.03
#define ADC_GAIN			ADC_GAIN_1_5
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
#define BUFFER_SIZE			6

#define RESISTOR			10000 // 10kOhm

static int16_t m_sample_buffer[BUFFER_SIZE];

// the channel configuration with channel not yet filled in
static struct adc_channel_cfg channel_cfg = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = CHANNEL_ID,
	.differential	  = 0,
	.input_positive   = ADC_PORT,
};

float getTempC(int voltage) {
	float resistance = (float)RESISTOR * (1023.0 / (float)voltage - 1.0);
	float temperature = log(resistance);
	temperature = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temperature * temperature ))* temperature );
	temperature = temperature - 273.15; // Convert Kelvin to Celcius
	return temperature;
}

void main(void)
{
	const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);
	int ret;

	if (!adc_dev) {
		printk("error: no adc device\n");
		return;
	}

	ret = adc_channel_setup(adc_dev, &channel_cfg);
	if (ret) {
		printk("error: channel setup failed %d\n", ret);
		return;
	}

	while (1) {
		const struct adc_sequence sequence = {
			.options	= NULL,
			.channels	= BIT(CHANNEL_ID),
			.buffer		= m_sample_buffer,
			.buffer_size	= sizeof(m_sample_buffer),
			.resolution	= ADC_RESOLUTION,
			.oversampling	= 0,
			.calibrate	= false,
		};

		ret = adc_read(adc_dev, &sequence);
		if (ret) {
			printk("error: read failed %d\n", ret);
			return;
		}

		printk("Raw : %d", m_sample_buffer[0]);
		printk("\t Temp : %f\n", getTempC(m_sample_buffer[0]));
		k_sleep(K_MSEC(100));
	}
}