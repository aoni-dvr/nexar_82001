#include "thermal_tmp103.h"
#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <thermal.h>

#define I2C_SLAVE_ADDR 0xE0

static int thermal_tmp103_read(unsigned char addr, unsigned char *data)
{
    unsigned char rx_data = 0;

#if defined(CONFIG_BOARD_CV25YANDEX_FOR_EVT)
    AmbaGPIO_SetFuncAlt(GPIO_PIN_103_I2C2_CLK);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_104_I2C2_DATA);
#endif
    if (i2c_read_reg(AMBA_I2C_CHANNEL2, I2C_SLAVE_ADDR, addr, &rx_data) >= 0) {
        if (data != NULL) {
            *data = rx_data;
            return 0;
        }
    }

    return -1;
}

static int thermal_tmp103_write(unsigned char addr, unsigned char data)
{
    unsigned char tx_buf[2] = {0};

    tx_buf[0] = (unsigned char)addr;
    tx_buf[1] = (unsigned char)data;
#if defined(CONFIG_BOARD_CV25YANDEX_FOR_EVT)
    AmbaGPIO_SetFuncAlt(GPIO_PIN_103_I2C2_CLK);
    AmbaGPIO_SetFuncAlt(GPIO_PIN_104_I2C2_DATA);
#endif
    i2c_write(AMBA_I2C_CHANNEL2, I2C_SLAVE_ADDR, tx_buf, 2);

    return 0;
}

static int thermal_tmp103_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    unsigned char val = 0;

    thermal_tmp103_read(Addr, &val);
    val &= ~Mask;
    val |= Data;
    thermal_tmp103_write(Addr, val);

    return 0;
}

static int thermal_tmp103_init(void)
{
    return 0;
}

static int thermal_tmp103_get_data(THERMAL_DATA_s *thermal_data)
{
    unsigned char value = 0;

    if (thermal_data == NULL) {
        return -1;
    }
    thermal_tmp103_read(THERMAL_TMP103_TEMPERTURE_REG, &value);
    if (value > 128) {
        thermal_data->value = value - 255 - 1;
    } else {
        thermal_data->value = value;
    }

    return 0;
}

THERMAL_OBJECT_s thermal_tmp103_object = {
    .name = "tmp103",
    .init = thermal_tmp103_init,
    .read = thermal_tmp103_read,
    .write = thermal_tmp103_write,
    .modify = thermal_tmp103_modify,
    .get_data = thermal_tmp103_get_data,
};

