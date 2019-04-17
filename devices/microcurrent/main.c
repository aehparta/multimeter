/*
 * Microcurrent measuring device.
 */

#include <libe/libe.h>
#include "config.h"


#ifdef TARGET_ESP32
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	struct i2c_master i2c;
	struct i2c_device dev1, dev2;
	void *context = CFG_I2C_CONTEXT;
	float vref = 5.0;

	/* base init */
	os_init();
	log_init(NULL, 0);
	nvm_init(NULL, 0);
	/* wifi base init */
#ifdef USE_WIFI
	ERROR_IF_R(wifi_init(), 1, "wifi initialization failed");
#endif

	/* open i2c */
	ERROR_IF_R(i2c_master_open(&i2c, context, CFG_I2C_FREQUENCY, CFG_I2C_SCL, CFG_I2C_SDA), 1, "unable to open i2c device");

	/* open mcp3221 devices */
	ERROR_IF_R(mcp3221_open(&dev1, &i2c, MCP3221_ADDR_A2), 1, "unable to find mcp3221-a2");
	ERROR_IF_R(mcp3221_open(&dev2, &i2c, MCP3221_ADDR_A5), 1, "unable to find mcp3221-a5");

	/* read mcp3221 devices */
	while (1) {
		int16_t v1, v2;
		v1 = mcp3221_read(&dev1);
		v2 = mcp3221_read(&dev2);
		if (v1 < 0) {
			ERROR_MSG("failed reading mcp3221-a2");
		}
		if (v2 < 0) {
			ERROR_MSG("failed reading mcp3221-a5");
		}

		if (!wifi_connected()) {
			INFO_MSG("wifi not connected");
		}
		INFO_MSG("A2: %.3fV (%04d), A5: %.3fV (%04d)", (float)v1 * vref / 4096.0, (int)v1, (float)v2 * vref / 4096.0, (int)v2);
		os_sleepf(0.2);
	}

	/* quit */
	mcp3221_close(&dev1);
	mcp3221_close(&dev2);
	i2c_master_close(&i2c);
#ifdef USE_WIFI
	wifi_quit();
#endif
	nvm_quit();
	log_quit();
	os_quit();
	return 0;
}
