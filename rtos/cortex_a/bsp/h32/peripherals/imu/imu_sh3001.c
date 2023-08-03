#include "platform.h"
#include <imu.h>
#include "../imu_sh3001.h"

static IMU_CALIBRATION_DATA_s calibration_data;

#define SLAVE_ADDR 0x6c
static char imu_sh3001_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char *rx_data, unsigned short size)
{
    if (i2c_read(IMU_I2C_CHANNEL, slave_addr, reg_addr, rx_data, size) >= 0) {
        return 0;
    }

    return -1;
}

static char imu_sh3001_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char *data, unsigned short len)
{
    unsigned char tx_buf[1024] = {0};

    tx_buf[0] = (unsigned char)reg_addr;
    memcpy(tx_buf + 1, data, len);

    return i2c_write(IMU_I2C_CHANNEL, slave_addr, tx_buf, len + 1);
}

#if 0
static char imu_sh3001_write_reg(unsigned char slave_addr, unsigned char reg_addr, unsigned char value)
{
    unsigned char tx_buf[2] = {0};

    tx_buf[0] = (unsigned char)reg_addr;
    tx_buf[1] = (unsigned char)value;

    return i2c_write(IMU_I2C_CHANNEL, slave_addr, tx_buf, 2);
}
#endif

static int imu_sh3001_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    return 0;
}

#include "sh3001.c"

static int imu_sh3001_config_data_ready_int(void)
{
    SH3001_pre_INT_config();
    SH3001_INT_Config(SH3001_INT0_LEVEL_HIGH, SH3001_INT_NO_LATCH, SH3001_INT1_LEVEL_LOW, SH3001_INT_CLEAR_ANY,
                    SH3001_INT1_NORMAL, SH3001_INT0_NORMAL, 1);
    SH3001_INT_Enable(SH3001_INT_GYRO_READY, SH3001_INT_ENABLE, SH3001_INT_MAP_INT1);

    return 0;
}

static int imu_sh3001_init(void)
{
    SH3001_init();

    return 0;
}

static int imu_sh3001_get_gyro_accel_data(GYRO_DATA_s *gyro, ACCEL_DATA_s *accel, int raw_data)
{
    short acc_data[3] = {0};
    short gyro_data[3] = {0};

    SH3001_GetImuData(acc_data, gyro_data);
    accel->x = acc_data[0];
    accel->y = acc_data[1];
    accel->z = acc_data[2];
    gyro->x = gyro_data[0];
    gyro->y = gyro_data[1];
    gyro->z = gyro_data[2];

    return 0;
}

static int imu_sh3001_get_mag_data(MAG_DATA_s *mag)
{
    return 0;
}

static int imu_sh3001_get_temperature_data(TEMP_DATA_s *temp)
{
    unsigned char data[2] = {0};
    unsigned short root_temp = 0;

    imu_sh3001_read(SLAVE_ADDR, SH3001_TEMP_ZL, data, 2);
    temp->temp = ((data[1] & 0xf) << 8) | data[0];

    imu_sh3001_read(SLAVE_ADDR, SH3001_TEMP_CONF0, data, 2);
    root_temp = ((data[0] & 0xf) << 8) | data[1];

    if (temp->temp - root_temp >= 0) {
        temp->temp_float = (temp->temp - root_temp)  / 16 + 25;
    } else {
        temp->temp_float = (root_temp - temp->temp)  / 16 + 25;
    }

    return 0;
}

static int imu_sh3001_power_off(int parking_level)
{
	SH3001_Temp_Config(SH3001_TEMP_ODR_125, SH3001_TEMP_DIS);
    SH3001_pre_INT_config();
    SH3001_INT_Config(SH3001_INT0_LEVEL_HIGH, SH3001_INT_NO_LATCH, SH3001_INT0_LEVEL_HIGH, SH3001_INT_CLEAR_ANY,
                SH3001_INT1_NORMAL, SH3001_INT0_NORMAL, 50);
    SH3001_INT_Enable(SH3001_INT_GYRO_READY, SH3001_INT_DISABLE, SH3001_INT_MAP_INT1);
    if (parking_level != 0) {
        SH3001_INT_Tap_Config(SH3001_TAP_X_INT_EN, SH3001_TAP_Y_INT_EN, SH3001_TAP_Z_INT_EN,
                            parking_level & 0xff, 100, 100, 100);
        SH3001_INT_Enable(SH3001_INT_SINGLE_TAP, SH3001_INT_ENABLE, SH3001_INT_MAP_INT);        
    } else {
        SH3001_INT_Enable(SH3001_INT_SINGLE_TAP, SH3001_INT_DISABLE, SH3001_INT_MAP_INT);        
    }

    return 0;
}

static int imu_sh3001_set_accel_data(unsigned char para_id, unsigned char para)
{
    return 0;
}

static int imu_sh3001_set_gyro_data(unsigned char para_id, unsigned char para)
{
    return 0;
}

static int imu_sh3001_set_calibration(unsigned char effect)
{
    return 0;
}

static int imu_sh3001_set_axis_polarity(unsigned char polarity)
{
    return 0;
}

static int imu_sh3001_enable_int(void)
{
    imu_sh3001_config_data_ready_int();

    return 0;
}

static int imu_sh3001_get_setting(IMU_SETTING_s *data)
{
    return 0;
}

static int imu_sh3001_set_calibration_data(IMU_CALIBRATION_DATA_s *data)
{
    if (data != NULL) {
        memcpy(&calibration_data, data, sizeof(IMU_CALIBRATION_DATA_s));
    }

    return 0;
}

IMU_OBJECT_s imu_sh3001_object = {
    .name = "sh3001",
    .init = imu_sh3001_init,
    .read = NULL,//imu_sh3001_read,
    .write = NULL,//imu_sh3001_write,
    .modify = imu_sh3001_modify,
    .get_gyro_accel_data = imu_sh3001_get_gyro_accel_data,
    .get_mag_data = imu_sh3001_get_mag_data,
    .get_temperature_data = imu_sh3001_get_temperature_data,
    .power_off = imu_sh3001_power_off,
    .set_accel_data = imu_sh3001_set_accel_data,
    .set_gyro_data = imu_sh3001_set_gyro_data,
    .set_calibration = imu_sh3001_set_calibration,
    .set_axis_polarity = imu_sh3001_set_axis_polarity,
    .enable_int = imu_sh3001_enable_int,
    .get_setting = imu_sh3001_get_setting,
    .set_calibration_data = imu_sh3001_set_calibration_data,
    .get_sensor_time = NULL,
};

