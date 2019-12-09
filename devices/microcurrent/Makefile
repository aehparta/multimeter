
TARGET= ESP32
USE += WIFI I2C LOG GPIO
DEFINES = I2C_FREQUENCY=400000 I2C_SCL=23 I2C_SDA=18
# DEFINES_ESP32 += I2C_BITBANG I2C_BITBANG_SCL=23 I2C_BITBANG_SDA=18 I2C_BITBANG_DYNAMIC

# init build system
include $(LIBE_PATH)/init.mk

# our own sources etc
BUILD_BINS = microcurrent
PROGRAM_BIN = microcurrent
microcurrent_SRC = main.c $(libe_SRC)

# build
include $(LIBE_PATH)/build.mk

# programmable targets
include $(LIBE_PATH)/flash/program.mk

-include local.mk
