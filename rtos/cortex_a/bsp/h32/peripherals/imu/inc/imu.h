#ifndef __IMU_H__
#define __IMU_H__

typedef struct _OFFSET_DATA_s_ {
    short x;
    short y;
    short z;
} OFFSET_DATA_s;

typedef struct _GYRO_DATA_s_ {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    float x_float;
    float y_float;
    float z_float;
    unsigned short x_float16;
    unsigned short y_float16;
    unsigned short z_float16;
} GYRO_DATA_s;

typedef struct _ACCEL_DATA_s_ {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    float x_float;
    float y_float;
    float z_float;
    unsigned short x_float16;
    unsigned short y_float16;
    unsigned short z_float16;
    unsigned int sensor_time;
} ACCEL_DATA_s;

typedef struct _MAG_DATA_s_ {
    unsigned short x;
    unsigned short y;
    unsigned short z;
} MAG_DATA_s;

typedef struct _TEMP_DATA_s_ {
    unsigned short temp;
    float temp_float;
    unsigned short temp_float16;
} TEMP_DATA_s;

typedef struct _IMU_DATA_s_ {
    GYRO_DATA_s gyro;
    ACCEL_DATA_s accel;
    MAG_DATA_s mag;
} IMU_DATA_s;

typedef struct _IMU_ACCEL_SETTING_s_ {
    int range;
    int odr;
    int swap_polarity;
    int power_bw;
    int calibrationed;
    float calibration_data[6];
} IMU_ACCEL_SETTING_s;

typedef struct _IMU_GYRO_SETTING_s_ {
    int range;
    int odr;
    int calibrationed;
    float calibration_data[6];
} IMU_GYRO_SETTING_s;

typedef struct _IMU_SETTING_s_ {
    IMU_ACCEL_SETTING_s accel_setting;
    IMU_GYRO_SETTING_s gyro_setting;
} IMU_SETTING_s;

typedef struct _IMU_CALIBRATION_DATA_s_ {
    int imu_calibration_flag;
    OFFSET_DATA_s accel_offset_2g;
    OFFSET_DATA_s accel_offset_4g;
    OFFSET_DATA_s accel_offset_8g;
    OFFSET_DATA_s accel_offset_16g;
    OFFSET_DATA_s gyro_offset;
} IMU_CALIBRATION_DATA_s;

typedef struct _IMU_OBJECT_s_ {
    char *name;
    int (*init)(void);
    int (*read)(unsigned char, unsigned char *, int);
    int (*write)(unsigned char, unsigned char);
    int (*modify)(unsigned char, unsigned char, unsigned char);
    int (*get_gyro_accel_data)(GYRO_DATA_s *, ACCEL_DATA_s *, int);
    int (*get_mag_data)(MAG_DATA_s *);
    int (*get_temperature_data)(TEMP_DATA_s *);
    int (*power_off)(int);
    int (*set_accel_data)(unsigned char, unsigned char);
    int (*set_gyro_data)(unsigned char, unsigned char);
    int (*set_calibration)(unsigned char);
    int (*set_axis_polarity)(unsigned char);
    int (*enable_int)(void);
    int (*get_setting)(IMU_SETTING_s *);
    int (*set_calibration_data)(IMU_CALIBRATION_DATA_s *);
    int (*get_sensor_time)(unsigned int *time);
    int (*set_enable)(unsigned char enable);
} IMU_OBJECT_s;

enum imu_accel {
    IMU_ACCEL_ODR = 1,
    IMU_ACCEL_RANGE,
};
enum imu_gyro {
    IMU_GYRO_ODR = 1,
    IMU_GYRO_RANGE,
};

int Imu_Init(void);
const char *Imu_GetName(void);
int Imu_Read(unsigned char addr, unsigned char *data);
int Imu_Write(unsigned char addr, unsigned char data);
int Imu_Modify(unsigned char addr, unsigned char mask, unsigned char data);
int Imu_GetData(IMU_DATA_s *data, int raw_data);
int Imu_GetTemperature(TEMP_DATA_s *data);
int Imu_PowerOff(unsigned char parking_level);
int Imu_SetAccelPara(unsigned char para_id, unsigned char para);
int Imu_SetGyroPara(unsigned char para_id, unsigned char para);
int Imu_SetCalibration(unsigned char para);
int Imu_SetAxisPolarity(unsigned char polarity);
int Imu_EnableInt(void);
int Imu_GetSetting(IMU_SETTING_s *data);
int Imu_SetCalibrationData(IMU_CALIBRATION_DATA_s *data);
int Imu_GetSensorTime(unsigned int *time);
int Imu_SetEnable(unsigned char enable);

#endif//__IMU_H__

