#include "platform.h"
#include <imu.h>
#include "../imu_bmi160.h"

static struct bmi160_dev sensor;
static IMU_CALIBRATION_DATA_s calibration_data;

static unsigned char imu_axis_polarity = 0;
static char imu_bmi160_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char *rx_data, unsigned short size)
{
    if (i2c_read(IMU_I2C_CHANNEL, slave_addr, reg_addr, rx_data, size) >= 0) {
        return 0;
    }

    return -1;
}

static char imu_bmi160_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char *data, unsigned short len)
{
    unsigned char tx_buf[1024] = {0};

    tx_buf[0] = (unsigned char)reg_addr;
    memcpy(tx_buf + 1, data, len);

    return i2c_write(IMU_I2C_CHANNEL, slave_addr, tx_buf, len + 1);
}

static int imu_bmi160_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    return 0;
}

static int imu_bmi160_config_data_ready_int(void)
{
    struct bmi160_int_settg config;

    /* Select the Interrupt channel/pin */
    config.int_channel = BMI160_INT_CHANNEL_2;
     /* Select the Interrupt type */
    config.int_type = BMI160_ACC_GYRO_DATA_RDY_INT;
    /* Select the interrupt channel/pin settings */
    config.int_pin_settg.output_en = BMI160_ENABLE;// Enabling interrupt pins to act as output pin
    config.int_pin_settg.output_mode = BMI160_DISABLE;// Choosing push-pull mode for interrupt pin
    config.int_pin_settg.output_type = BMI160_ENABLE;// Choosing active high output
    config.int_pin_settg.edge_ctrl = BMI160_ENABLE;// Choosing edge triggered output
    config.int_pin_settg.input_en = BMI160_DISABLE;// Disabling interrupt pin to act as input
    config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE;// non-latched output
    bmi160_set_int_config(&config, &sensor);

    return 0;
}

static int imu_bmi160_disable_int2(void)
{
    struct bmi160_int_settg config;

    /* Select the Interrupt channel/pin */
    config.int_channel = BMI160_INT_CHANNEL_2;
     /* Select the Interrupt type */
    config.int_type = BMI160_ACC_GYRO_DATA_RDY_INT;
    /* Select the interrupt channel/pin settings */
    config.int_pin_settg.output_en = BMI160_DISABLE;// Enabling interrupt pins to act as output pin
    config.int_pin_settg.output_mode = BMI160_DISABLE;// Choosing push-pull mode for interrupt pin
    config.int_pin_settg.output_type = BMI160_DISABLE;// Choosing active high output
    config.int_pin_settg.edge_ctrl = BMI160_DISABLE;// Choosing edge triggered output
    config.int_pin_settg.input_en = BMI160_DISABLE;// Disabling interrupt pin to act as input
    config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE;// non-latched output
    bmi160_set_int_config(&config, &sensor);

    return 0;
}

#if 1
static void generatetables(void);
#endif

static int imu_bmi160_init(void)
{
    int8_t rslt = BMI160_OK;
    struct bmi160_pmu_status pmu_status;

    memset(&calibration_data, 0, sizeof(calibration_data));

    sensor.id = BMI160_I2C_ADDR << 1;
    sensor.intf = BMI160_I2C_INTF;
    sensor.read = imu_bmi160_read;
    sensor.write = imu_bmi160_write;
    sensor.delay_ms = msleep;
    rslt = bmi160_init(&sensor);
    debug_line("bmi160_init: %d, chip id: %02x", rslt, sensor.chip_id);

    /* Select the Output data rate, range of accelerometer sensor */
    sensor.accel_cfg.odr = BMI160_ACCEL_ODR_200HZ;
    sensor.accel_cfg.range = BMI160_ACCEL_RANGE_16G;
    sensor.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;
    sensor.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    sensor.gyro_cfg.odr = BMI160_GYRO_ODR_200HZ;
    sensor.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;
    sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&sensor);
    bmi160_get_power_mode(&pmu_status, &sensor);
#if 1
    generatetables();
#endif
    imu_bmi160_config_data_ready_int();

    return 0;
}

#if 1
static unsigned short basetable[0x200] = {0};
static unsigned short shifttable[0x200] = {0};

static void generatetables(void)
{
    unsigned int i = 0;
    int e = 0;
    for (i = 0; i < 256; ++i) {
        e = i - 127;
        if (e <- 24) { // Very small numbers map to zero
            basetable[i | 0x000] = 0x0000;
            basetable[i | 0x100] = 0x8000;
            shifttable[i | 0x000] = 24;
            shifttable[i | 0x100] = 24;
        }  else if (e < -14) { // Small numbers map to denorms
            basetable[i | 0x000] = (0x0400 >> (-e-14));
            basetable[i | 0x100] = (0x0400 >> (-e-14)) | 0x8000;
            shifttable[i | 0x000] = -e - 1;
            shifttable[i | 0x100] = -e - 1;
        } else if (e <= 15) { // Normal numbers just lose precision
            basetable[i | 0x000] = ((e + 15) << 10);
            basetable[i | 0x100] = ((e + 15) << 10) | 0x8000;
            shifttable[i | 0x000] = 13;
            shifttable[i | 0x100] = 13;
        } else if (e < 128) { // Large numbers map to Infinity
            basetable[i | 0x000] = 0x7C00;
            basetable[i | 0x100] = 0xFC00;
            shifttable[i | 0x000] = 24;
            shifttable[i | 0x100] = 24;
        } else { // Infinity and NaN's stay Infinity and NaN's
            basetable[i | 0x000] = 0x7C00;
            basetable[i | 0x100] = 0xFC00;
            shifttable[i | 0x000] = 13;
            shifttable[i | 0x100] = 13;
        }
    }
}

static unsigned short f32_2_f16(float f)
{
    unsigned int value = *((unsigned int *)&f);
    return basetable[(value >> 23) & 0x1ff] + ((value & 0x007fffff) >> shifttable[(value >> 23) & 0x1ff]);
    //return ((value >> 16) & 0x8000) | ((((value & 0x7f800000) - 0x38000000) >> 13) & 0x7c00) | ((value >> 13) & 0x03ff);
}
#endif

static void ACCEL_CONVERT(unsigned short value, unsigned short *x, float *x_float, unsigned short *x_float16)
{
    int val = value;
    float val_float = val * 2000 * 1.0;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    switch (sensor.accel_cfg.range) {
    case BMI160_ACCEL_RANGE_2G:
        val_float = (val * 9.8) / (0x8000 / 2);
        break;
    case BMI160_ACCEL_RANGE_4G:
        val_float = (val * 9.8) / (0x8000 / 4);
        break;
    case BMI160_ACCEL_RANGE_8G:
        val_float = (val * 9.8) / (0x8000 / 8);
        break;
    case BMI160_ACCEL_RANGE_16G:
        val_float = (val * 9.8) / (0x8000 / 16);
        break;
    default:
        val_float = (val * 9.8) / (0x8000 / 16);
        break;
    }
    if (x != NULL) *x = value;
    if (x_float != NULL) *x_float = val_float;
    if (x_float16 != NULL) *x_float16 = f32_2_f16(val_float);
}

static void GYRO_CONVERT(unsigned short value, unsigned short *x, float *x_float, unsigned short *x_float16)
{
    int val = value;
    float val_float = val * 2000 * 1.0;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    switch (sensor.gyro_cfg.range) {
    case BMI160_GYRO_RANGE_2000_DPS:
        val_float = val * 2000 * 1.0 / 0x7fff;
        break;
    case BMI160_GYRO_RANGE_1000_DPS:
        val_float = val * 1000 * 1.0 / 0x7fff;
        break;
    case BMI160_GYRO_RANGE_500_DPS:
        val_float = val * 500 * 1.0 / 0x7fff;
        break;
    case BMI160_GYRO_RANGE_250_DPS:
        val_float = val * 250 * 1.0 / 0x7fff;
        break;
    case BMI160_GYRO_RANGE_125_DPS:
        val_float = val * 125 * 1.0 / 0x7fff;
        break;
    default:
        val_float = val * 2000 * 1.0 / 0x7fff;
        break;
    }
    if (x != NULL) *x = value;
    if (x_float != NULL) *x_float = val_float;
    if (x_float16 != NULL) *x_float16 = f32_2_f16(val_float);
}

static void TEMP_CONVERT(unsigned short value, unsigned short *x, float *x_float, unsigned short *x_float16)
{
    int val = value;
    float val_float = val * 2000 * 1.0;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    val_float = 23 + val * 1.0 / 512;
    if (x != NULL) *x = value;
    if (x_float != NULL) *x_float = val_float;
    if (x_float16 != NULL) *x_float16 = f32_2_f16(val_float);
}

static int imu_bmi160_get_gyro_accel_data(GYRO_DATA_s *gyro, ACCEL_DATA_s *accel, int raw_data)
{
    static struct bmi160_sensor_data gyro_data;
    static struct bmi160_sensor_data accel_data;

    if (gyro == NULL || accel == NULL) {
        return -1;
    }
    bmi160_get_sensor_data(BMI160_ACCEL_SEL | BMI160_GYRO_SEL | BMI160_TIME_SEL, &accel_data, &gyro_data, &sensor);
    if (raw_data == 0) {
        if (calibration_data.imu_calibration_flag) {
            switch (sensor.accel_cfg.range) {
            case BMI160_ACCEL_RANGE_2G:
                accel_data.x += calibration_data.accel_offset_2g.x;
                accel_data.y += calibration_data.accel_offset_2g.y;
                accel_data.z += calibration_data.accel_offset_2g.z;
                break;
            case BMI160_ACCEL_RANGE_4G:
                accel_data.x += calibration_data.accel_offset_4g.x;
                accel_data.y += calibration_data.accel_offset_4g.y;
                accel_data.z += calibration_data.accel_offset_4g.z;
                break;
            case BMI160_ACCEL_RANGE_8G:
                accel_data.x += calibration_data.accel_offset_8g.x;
                accel_data.y += calibration_data.accel_offset_8g.y;
                accel_data.z += calibration_data.accel_offset_8g.z;
                break;
            case BMI160_ACCEL_RANGE_16G:
                accel_data.x += calibration_data.accel_offset_16g.x;
                accel_data.y += calibration_data.accel_offset_16g.y;
                accel_data.z += calibration_data.accel_offset_16g.z;
                break;
            default:
                accel_data.x += calibration_data.accel_offset_16g.x;
                accel_data.y += calibration_data.accel_offset_16g.y;
                accel_data.z += calibration_data.accel_offset_16g.z;
                break;
            }
            gyro_data.x += calibration_data.gyro_offset.x;
            gyro_data.y += calibration_data.gyro_offset.y;
            gyro_data.z += calibration_data.gyro_offset.z;
        }
    }
    gyro_data.z = ((imu_axis_polarity & (1 << 5)) > 0) ? (0 - gyro_data.z) : gyro_data.z;
    gyro_data.y = ((imu_axis_polarity & (1 << 6)) > 0) ? (0 - gyro_data.y) : gyro_data.y;
    gyro_data.x = ((imu_axis_polarity & (1 << 7)) > 0) ? (0 - gyro_data.x) : gyro_data.x;

    if (imu_axis_polarity & (1 << 4)) {
        gyro_data.y = gyro_data.y + gyro_data.x;
        gyro_data.x = gyro_data.y - gyro_data.x;
        gyro_data.y = gyro_data.y - gyro_data.x;
    }
    GYRO_CONVERT(gyro_data.x, &gyro->x, &gyro->x_float, &gyro->x_float16);
    GYRO_CONVERT(gyro_data.y, &gyro->y, &gyro->y_float, &gyro->y_float16);
    GYRO_CONVERT(gyro_data.z, &gyro->z, &gyro->z_float, &gyro->z_float16);

    accel_data.z = ((imu_axis_polarity & (1 << 1)) > 0) ? (0 - accel_data.z) : accel_data.z;
    accel_data.y = ((imu_axis_polarity & (1 << 2)) > 0) ? (0 - accel_data.y) : accel_data.y;
    accel_data.x = ((imu_axis_polarity & (1 << 3)) > 0) ? (0 - accel_data.x) : accel_data.x;
    if (imu_axis_polarity & (1 << 0)) {
        accel_data.y = accel_data.y + accel_data.x;
        accel_data.x = accel_data.y - accel_data.x;
        accel_data.y = accel_data.y - accel_data.x;
    }
    ACCEL_CONVERT(accel_data.x, &accel->x, &accel->x_float, &accel->x_float16);
    ACCEL_CONVERT(accel_data.y, &accel->y, &accel->y_float, &accel->y_float16);
    ACCEL_CONVERT(accel_data.z, &accel->z, &accel->z_float, &accel->z_float16);
    accel->sensor_time = accel_data.sensortime * 39 / 1000;

    return 0;
}

static int imu_bmi160_get_mag_data(MAG_DATA_s *data)
{
    return 0;
}

static int imu_bmi160_get_temperature_data(TEMP_DATA_s *data)
{
#define BMI160_TEMP_DATA_ADDR           0x20

    unsigned char rx_buffer[2] = {0};

    if (data == NULL) {
        return -1;
    }
    bmi160_get_regs(BMI160_TEMP_DATA_ADDR, rx_buffer, 2, &sensor);
    TEMP_CONVERT((rx_buffer[1] << 8) | rx_buffer[0], &data->temp, &data->temp_float, &data->temp_float16);

    return 0;
}

static int imu_bmi160_power_off(int parking_level)
{
    if (parking_level != 0) {
        struct bmi160_int_settg config;

        /* Select the Interrupt channel/pin */
        config.int_channel = BMI160_INT_CHANNEL_1;
         /* Select the Interrupt type */
        config.int_type = BMI160_ACC_ANY_MOTION_INT;
        /* Select the interrupt channel/pin settings */
        config.int_pin_settg.output_en = BMI160_ENABLE;// Enabling interrupt pins to act as output pin
        config.int_pin_settg.output_mode = BMI160_DISABLE;// Choosing push-pull mode for interrupt pin
        config.int_pin_settg.output_type = BMI160_ENABLE;// Choosing active high output
        config.int_pin_settg.edge_ctrl = BMI160_ENABLE;// Choosing edge triggered output
        config.int_pin_settg.input_en = BMI160_DISABLE;// Disabling interrupt pin to act as input
        config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_160_MILLI_SEC;
        /* Select the Any-motion interrupt parameters */
        config.int_type_cfg.acc_any_motion_int.anymotion_en = BMI160_ENABLE;// 1- Enable the any-motion, 0- disable any-motion
        config.int_type_cfg.acc_any_motion_int.anymotion_x = BMI160_ENABLE;// Enabling x-axis for any motion interrupt
        config.int_type_cfg.acc_any_motion_int.anymotion_y = BMI160_ENABLE;// Enabling y-axis for any motion interrupt
        config.int_type_cfg.acc_any_motion_int.anymotion_z = BMI160_ENABLE;// Enabling z-axis for any motion interrupt
        config.int_type_cfg.acc_any_motion_int.anymotion_dur = 0;// any-motion duration
        config.int_type_cfg.acc_any_motion_int.anymotion_thr = parking_level;// (2-g range) -> (slope_thr) * 3.91 mg, (4-g range) -> (slope_thr) * 7.81 mg, (8-g range) ->(slope_thr) * 15.63 mg, (16-g range) -> (slope_thr) * 31.25 mg
        bmi160_set_int_config(&config, &sensor);

        imu_bmi160_disable_int2();

        sensor.accel_cfg.odr = BMI160_ACCEL_ODR_12_5HZ;
        sensor.accel_cfg.power = BMI160_ACCEL_LOWPOWER_MODE;
        sensor.gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE;
    } else {
        sensor.gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE;
        sensor.accel_cfg.power = BMI160_ACCEL_SUSPEND_MODE;
    }
    /* Set the sensor configuration */
    bmi160_set_sens_conf(&sensor);

    return 0;
}

static int imu_bmi160_set_accel_data(unsigned char para_id, unsigned char para)
{
    if (para_id == IMU_ACCEL_ODR) {
        sensor.accel_cfg.odr = para;
    } else if (para_id == IMU_ACCEL_RANGE) {
        sensor.accel_cfg.range = para;
    } else {
        return -1;
    }
    bmi160_set_sens_conf(&sensor);

    return 0;
}

static int imu_bmi160_set_gyro_data(unsigned char para_id, unsigned char para)
{
    if (para_id == IMU_GYRO_ODR) {
        sensor.gyro_cfg.odr = para;
    } else if(para_id == IMU_GYRO_RANGE) {
        sensor.gyro_cfg.range = para;
    } else {
        return -1;
    }
    bmi160_set_sens_conf(&sensor);

    return 0;
}

static int imu_bmi160_set_calibration(unsigned char effect)
{
    struct bmi160_foc_conf foc_conf;
    struct bmi160_offsets offset;
    uint8_t foc_acc_x_en = (effect & (1 << 3)) | (effect & 1<<0);
    uint8_t foc_acc_y_en = (effect & (1 << 2)) | (effect & 1<<0);
    uint8_t foc_acc_z_en = (effect & (1 << 1)) | (effect & 1<<0);
    foc_conf.foc_gyr_en = 1;
    foc_conf.foc_acc_x = (foc_acc_x_en > 0) ? 0x03 : 0;
    foc_conf.foc_acc_y = (foc_acc_y_en > 0) ? 0x03 : 0;
    foc_conf.foc_acc_z = (foc_acc_z_en > 0) ? 0x03 : 0;
    foc_conf.acc_off_en = 1;
    foc_conf.gyro_off_en = 1;
    bmi160_start_foc(&foc_conf, &offset, &sensor);
    bmi160_update_nvm(&sensor);

    return 0;
}

static int imu_bmi160_set_axis_polarity(unsigned char polarity)
{
    imu_axis_polarity = polarity;

    return 0;
}

static int imu_bmi160_get_setting(IMU_SETTING_s *data)
{
    if (data == NULL) {
        return -1;
    }
    data->accel_setting.calibrationed = calibration_data.imu_calibration_flag;
    data->accel_setting.calibration_data[0] = 0.0;
    data->accel_setting.calibration_data[1] = 0.0;
    data->accel_setting.calibration_data[2] = 0.0;
    data->accel_setting.calibration_data[3] = 0.0;
    data->accel_setting.calibration_data[4] = 0.0;
    data->accel_setting.calibration_data[5] = 0.0;
    data->accel_setting.range = sensor.accel_cfg.range;
    data->accel_setting.odr = sensor.accel_cfg.odr;
    data->accel_setting.swap_polarity = imu_axis_polarity;
    data->accel_setting.power_bw = sensor.accel_cfg.bw;

    data->gyro_setting.calibrationed = calibration_data.imu_calibration_flag;
    data->gyro_setting.calibration_data[0] = 0.0;
    data->gyro_setting.calibration_data[1] = 0.0;
    data->gyro_setting.calibration_data[2] = 0.0;
    data->gyro_setting.calibration_data[3] = 0.0;
    data->gyro_setting.calibration_data[4] = 0.0;
    data->gyro_setting.calibration_data[5] = 0.0;
    data->gyro_setting.range = sensor.gyro_cfg.range;
    data->gyro_setting.odr = sensor.gyro_cfg.odr;

    return 0;
}

static int imu_bmi160_set_calibration_data(IMU_CALIBRATION_DATA_s *data)
{
    if (data != NULL) {
        memcpy(&calibration_data, data, sizeof(IMU_CALIBRATION_DATA_s));
    }

    return 0;
}

static int imu_bmi160_get_sensor_time(unsigned int *time)
{
    if (time != NULL) {
        unsigned char data[3] = {0};

        bmi160_get_regs(0x18, data, sizeof(data), &sensor);
        *time = (unsigned int )((data[2] << 16 | data[1] << 8 | data[0]) * 39);//convert microseconds
        //debug_line("%d", *time);
    }

    return 0;
}

IMU_OBJECT_s imu_bmi160_object = {
    .name = "bmi160",
    .init = imu_bmi160_init,
    .read = NULL,//imu_bmi160_read,
    .write = NULL,//imu_bmi160_write,
    .modify = imu_bmi160_modify,
    .get_gyro_accel_data = imu_bmi160_get_gyro_accel_data,
    .get_mag_data = imu_bmi160_get_mag_data,
    .get_temperature_data = imu_bmi160_get_temperature_data,
    .power_off = imu_bmi160_power_off,
    .set_accel_data = imu_bmi160_set_accel_data,
    .set_gyro_data = imu_bmi160_set_gyro_data,
    .set_calibration = imu_bmi160_set_calibration,
    .set_axis_polarity = imu_bmi160_set_axis_polarity,
    .enable_int = NULL,
    .get_setting = imu_bmi160_get_setting,
    .set_calibration_data = imu_bmi160_set_calibration_data,
    .get_sensor_time = imu_bmi160_get_sensor_time,
};

