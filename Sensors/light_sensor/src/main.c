#include <zephyr/kernel.h> 
#include <zephyr/init.h>
#include <zephyr/drivers/adc.h>

#define ADC_NODE		DT_NODELABEL(adc)

#define ADC_RESOLUTION		10
#define CHANNEL_ID 			0
#define ADC_PORT			SAADC_CH_PSELP_PSELP_AnalogInput0 // P0.02
#define ADC_GAIN			ADC_GAIN_1_4
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
#define BUFFER_SIZE			6

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

float mapRange(float minIn, float maxIn, float minOut, float maxOut, float val) {
	return (val - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
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
		printk("\t Map : %.\n", mapRange(0, 1023, 0, 100, m_sample_buffer[0]));
		k_sleep(K_MSEC(100));
	}
}