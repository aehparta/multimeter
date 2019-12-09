/*
 * Microcurrent measuring device.
 */

#include <libe/libe.h>
#include <pthread.h>
#include <esp_timer.h>

#ifndef TARGET_ESP32
#error "This app only works in ESP32"
#endif

struct i2c_master i2c;
struct i2c_device dev1, dev2;
int16_t v1 = 0, v2 = 0;
volatile int64_t ttt = 0;
volatile int samples = 0;

pthread_t adcs_thread;


static void *adcs_thread_func(void *data)
{
	int64_t t = esp_timer_get_time();

	while (1) {
		v1 = mcp3221_read(&dev1, 0);
		v2 = mcp3221_read(&dev2, 0);
		ttt++;

		while (t > esp_timer_get_time());
		t += 500;
	}

	return NULL;
}

// static void adcs_read(void *data)
// {
// 	int64_t ttt = esp_timer_get_time();
// 	tt = ttt - t;
// 	t = ttt;

// 	v1 = mcp3221_read(&dev1, 0);
// 	v2 = mcp3221_read(&dev2, 0);
// 	if (v1 < 0) {
// 		ERROR_MSG("failed reading mcp3221-a2");
// 	}
// 	if (v2 < 0) {
// 		ERROR_MSG("failed reading mcp3221-a5");
// 	}

// 	samples++;
// }

int app_main(int argc, char *argv[])
{
	float vref = 5.0;

	/* base init */
	os_init();
	log_init();
	/* wifi base init */
	nvm_init(NULL, 0);
	// ERROR_IF_R(wifi_init(), 1, "wifi initialization failed");

	/* open i2c */
	ERROR_IF_R(i2c_master_open(&i2c, I2C_NUM_0, 400000, I2C_SCL, I2C_SDA), 1, "unable to open i2c device");
	/* enable internal pullups */
	i2c_set_pin(I2C_NUM_0, I2C_SDA, I2C_SCL, GPIO_PULLUP_ENABLE, GPIO_PULLUP_ENABLE, I2C_MODE_MASTER);

	/* open mcp3221 devices */
	ERROR_IF_R(mcp3221_open(&dev1, &i2c, MCP3221_ADDR_A2), 1, "unable to find mcp3221-a2");
	ERROR_IF_R(mcp3221_open(&dev2, &i2c, MCP3221_ADDR_A5), 1, "unable to find mcp3221-a5");

	/* create adc reader thread */
	pthread_create(&adcs_thread, NULL, adcs_thread_func, NULL);

	/* create timer */
	// esp_timer_handle_t adcs_timer;
	// const esp_timer_create_args_t adcs_timer_args = {
	// 	.callback = &adcs_read,
	// };
	// esp_timer_create(&adcs_timer_args, &adcs_timer);
	// esp_timer_start_periodic(adcs_timer, 500);

	/* read mcp3221 devices */
	while (1) {
		INFO_MSG("A2: %.3fV (%04d), A5: %.3fV (%04d)", (float)v1 * vref / 4096.0, (int)v1, (float)v2 * vref / 4096.0, (int)v2);
		INFO_MSG("%6.2f mA, %6.1f uA, %8ld %d", ((float)v1 - 2054) / 20.0, ((float)v2 - 2066.0) / 1.774666, ttt, samples);
		samples = 0;
		os_sleepf(0.2);
	}

	/* quit */
	mcp3221_close(&dev1);
	mcp3221_close(&dev2);
	i2c_master_close(&i2c);
	// wifi_quit();
	nvm_quit();
	log_quit();
	os_quit();
	return 0;
}
