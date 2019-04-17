/*
 * Platform specific configuration defaults.
 */

#ifdef TARGET_ESP32
/* i2c */
#define CFG_I2C_CONTEXT     NULL
#define CFG_I2C_FREQUENCY   400000
#define CFG_I2C_SCL         23
#define CFG_I2C_SDA         18
#endif
