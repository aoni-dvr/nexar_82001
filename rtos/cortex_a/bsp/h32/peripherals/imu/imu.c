#include <imu.h>
#include "platform.h"
#include "../imu_bmi160.h"
#include "../imu_sh3001.h"
#include "../imu_icm42670.h"
#include "../imu_icm42631.h"

static IMU_OBJECT_s *imu_object = NULL;
static AMBA_KAL_MUTEX_t mutex;

#define CHECK_IMU_OBJECT {if (imu_object == NULL) {debug_line("IMU No Register");return -1;}}
static inline void imu_register(IMU_OBJECT_s *imu_obj)
{
    imu_object = imu_obj;
    if (imu_object != NULL) {
        debug_line("%s: %s", __func__, imu_object->name);
    } else {
        debug_line("%s: No PMU", __func__);
    }
}

int Imu_Init(void)
{
#if defined(CONFIG_ENABLE_IMU)
#if defined(CONFIG_IMU_ICM20948)
    extern IMU_OBJECT_s imu_icm20948_object;
    imu_register(&imu_icm20948_object);
#endif
#if defined(CONFIG_IMU_BMI160)
    extern IMU_OBJECT_s imu_bmi160_object;
    //imu_register(&imu_bmi160_object);
#endif
#if defined(CONFIG_IMU_SH3001)
    extern IMU_OBJECT_s imu_sh3001_object;
    //imu_register(&imu_sh3001_object);
#endif
#if defined(CONFIG_IMU_ICM42670)
    extern IMU_OBJECT_s imu_icm42670_object;
    imu_register(&imu_icm42670_object);
#endif
#if defined(CONFIG_IMU_ICM42631)
    extern IMU_OBJECT_s imu_icm42631_object;
    imu_register(&imu_icm42631_object);
#endif

#if defined(CONFIG_BSP_CV25_NEXAR_D161)
    {
        unsigned char value = 0;
        if (i2c_read_reg(IMU_I2C_CHANNEL, BMI160_I2C_ADDR << 1, 0x00, &value) >= 0 && value == 0xd1) {
            debug_line("detected bmi160 imu");
            imu_register(&imu_bmi160_object);
        } else if (i2c_read_reg(IMU_I2C_CHANNEL, 0x6c, 0x0f, &value) >= 0 && value == 0x61) {
            debug_line("detected sh3001 imu");
            imu_register(&imu_sh3001_object);
        } else {
            debug_line("detected other imu, treat as bmi160");
            imu_register(&imu_bmi160_object);
        }
    }
#endif
    if (AmbaKAL_MutexCreate(&mutex, "imu_mutex") != OK) {
        debug_line("%s create mutex fail", __func__);
        return -1;
    }
    if (imu_object->init != NULL) {
        imu_object->init();
    }
#endif

    return 0;
}

const char *Imu_GetName(void)
{
    static char *null_name = "";

    if (imu_object == NULL || imu_object->name == NULL) {
        return null_name;
    }

    return imu_object->name;
}

int Imu_Read(unsigned char addr, unsigned char *data)
{
    CHECK_IMU_OBJECT;
    if (imu_object->read == NULL) {
        return -1;
    }
    imu_object->read(addr, data, 1);

    return 0;
}

int Imu_Write(unsigned char addr, unsigned char data)
{
    CHECK_IMU_OBJECT;
    if (imu_object->write == NULL) {
        return -1;
    }
    imu_object->write(addr, data);

    return 0;
}

int Imu_Modify(unsigned char addr, unsigned char mask, unsigned char data)
{
    CHECK_IMU_OBJECT;
    if (imu_object->modify == NULL) {
        return -1;
    }
    imu_object->modify(addr, mask, data);

    return 0;
}

int Imu_GetTemperature(TEMP_DATA_s *data)
{
    CHECK_IMU_OBJECT;
    if (imu_object->get_temperature_data != NULL) {
        imu_object->get_temperature_data(data);
    }

    return 0;
}

int Imu_GetSetting(IMU_SETTING_s *data)
{
    CHECK_IMU_OBJECT;
    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);
    if (imu_object->get_setting != NULL) {
        imu_object->get_setting(data);
    }
    AmbaKAL_MutexGive(&mutex);

    return 0;
}

int Imu_GetData(IMU_DATA_s *data, int raw_data)
{
    CHECK_IMU_OBJECT;
    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);
    if (imu_object->get_gyro_accel_data != NULL) {
        imu_object->get_gyro_accel_data(&(data->gyro), &(data->accel), raw_data);
    }
    /*if (imu_object->get_mag_data != NULL) {
        imu_object->get_mag_data(&(data->mag));
    }*/
    AmbaKAL_MutexGive(&mutex);

    return 0;
}

int Imu_PowerOff(unsigned char parking_level)
{
    CHECK_IMU_OBJECT;
    if (imu_object->power_off != NULL) {
        imu_object->power_off(parking_level);
    }

    return 0;
}

int Imu_SetAccelPara(unsigned char para_id, unsigned char para)
{
    if (imu_object->set_accel_data != NULL) {
        imu_object->set_accel_data(para_id, para);
    }
    
    return 0;
}

int Imu_SetGyroPara(unsigned char para_id, unsigned char para)
{
    if (imu_object->set_gyro_data != NULL) {
        imu_object->set_gyro_data(para_id, para);
    }

    return 0;
}

int Imu_SetCalibration(unsigned char effect)
{
    if (imu_object->set_calibration != NULL) {
        imu_object->set_calibration(effect);
    }

    return 0;
}

int Imu_SetAxisPolarity(unsigned char polarity)
{
    if (imu_object->set_axis_polarity != NULL) {
        imu_object->set_axis_polarity(polarity);
    }

    return 0;
}

int Imu_EnableInt(void)
{
    if (imu_object->enable_int != NULL) {
        imu_object->enable_int();
    }

    return 0;
}

int Imu_SetCalibrationData(IMU_CALIBRATION_DATA_s *data)
{
    if (imu_object->set_calibration_data != NULL) {
        imu_object->set_calibration_data(data);
    }

    return 0;
}

int Imu_GetSensorTime(unsigned int *time)
{
    if (imu_object->get_sensor_time != NULL) {
        imu_object->get_sensor_time(time);
    }

    return 0;
}

int Imu_SetEnable(unsigned char enable)
{
    if (imu_object->set_enable != NULL) {
        imu_object->set_enable(enable);
    }

    return 0;
}

