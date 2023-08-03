#include "imu_icm20948.h"
#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <imu.h>

#define I2C_SLAVE_ADDR 0xD0

static int imu_icm20948_read(unsigned char addr, unsigned char *data)
{
    unsigned char rx_data = 0;

    if (i2c_read_reg(AMBA_I2C_CHANNEL3, I2C_SLAVE_ADDR, addr, &rx_data) >= 0) {
        if (data != NULL) {
            *data = rx_data;
            return 0;
        }
    }

    return -1;
}

static int imu_icm20948_write(unsigned char addr, unsigned char data)
{
    unsigned char tx_buf[2] = {0};

    tx_buf[0] = (unsigned char)addr;
    tx_buf[1] = (unsigned char)data;

    return i2c_write(AMBA_I2C_CHANNEL3, I2C_SLAVE_ADDR, tx_buf, 2);
}

static int imu_icm20948_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    unsigned char val = 0;

    imu_icm20948_read(Addr, &val);
    val &= ~Mask;
    val |= Data;
    imu_icm20948_write(Addr, val);

    return 0;
}

static void invmsICM20948ReadSecondary(unsigned char u8I2CAddr, unsigned char u8RegAddr, unsigned char u8Len, unsigned char *pu8data)
{
    unsigned char i = 0;
    unsigned char u8Temp = 0;

    imu_icm20948_write(REG_ADD_REG_BANK_SEL,  REG_VAL_REG_BANK_3); //swtich bank3
    imu_icm20948_write(REG_ADD_I2C_SLV0_ADDR, u8I2CAddr);
    imu_icm20948_write(REG_ADD_I2C_SLV0_REG,  u8RegAddr);
    imu_icm20948_write(REG_ADD_I2C_SLV0_CTRL, REG_VAL_BIT_SLV0_EN|u8Len);

    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0); //swtich bank0

    imu_icm20948_read(REG_ADD_USER_CTRL, &u8Temp);
    u8Temp |= REG_VAL_BIT_I2C_MST_EN;
    imu_icm20948_write(REG_ADD_USER_CTRL, u8Temp);
    msleep(5);
    u8Temp &= ~REG_VAL_BIT_I2C_MST_EN;
    imu_icm20948_write(REG_ADD_USER_CTRL, u8Temp);

    for (i = 0; i < u8Len; i++) {
        imu_icm20948_read(REG_ADD_EXT_SENS_DATA_00 + i, pu8data + i);
    }
    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_3); //swtich bank3

    imu_icm20948_read(REG_ADD_I2C_SLV0_CTRL, &u8Temp);
    u8Temp &= ~((REG_VAL_BIT_I2C_MST_EN)&(REG_VAL_BIT_MASK_LEN));
    imu_icm20948_write(REG_ADD_I2C_SLV0_CTRL, u8Temp);

    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0); //swtich bank0
}

static void invmsICM20948WriteSecondary(unsigned char u8I2CAddr, unsigned char u8RegAddr, unsigned char u8data)
{
    unsigned char u8Temp;
    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_3); //swtich bank3
    imu_icm20948_write(REG_ADD_I2C_SLV1_ADDR, u8I2CAddr);
    imu_icm20948_write(REG_ADD_I2C_SLV1_REG, u8RegAddr);
    imu_icm20948_write(REG_ADD_I2C_SLV1_DO, u8data);
    imu_icm20948_write(REG_ADD_I2C_SLV1_CTRL, REG_VAL_BIT_SLV0_EN|1);

    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0); //swtich bank0

    imu_icm20948_read(REG_ADD_USER_CTRL, &u8Temp);
    u8Temp |= REG_VAL_BIT_I2C_MST_EN;
    imu_icm20948_write(REG_ADD_USER_CTRL, u8Temp);
    msleep(5);
    u8Temp &= ~REG_VAL_BIT_I2C_MST_EN;
    imu_icm20948_write(REG_ADD_USER_CTRL, u8Temp);

    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_3); //swtich bank3

    imu_icm20948_read(REG_ADD_I2C_SLV0_CTRL, &u8Temp);
    u8Temp &= ~((REG_VAL_BIT_I2C_MST_EN)&(REG_VAL_BIT_MASK_LEN));
    imu_icm20948_write(REG_ADD_I2C_SLV0_CTRL, u8Temp);

    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0); //swtich bank0
}

static int icm20948_mag_check(void)
{
    unsigned char buf[2] = {0};

    invmsICM20948ReadSecondary(I2C_ADD_ICM20948_AK09916 | I2C_ADD_ICM20948_AK09916_READ,
                                REG_ADD_MAG_WIA1, 2, buf);
    debug_line("%s, buf[0] = %.2X, buf[1] = %.2X", __func__, buf[0], buf[1]);
    if ((buf[0] == REG_VAL_MAG_WIA1) && (buf[1] == REG_VAL_MAG_WIA2)) {
        return 0;
    }
    debug_line("%s failed", __func__);

    return -1;
}

static int imu_icm20948_init(void)
{
    unsigned char data = 0;

    /* user bank 0 register */
    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0);
    imu_icm20948_read(REG_ADD_WIA, &data);
    debug_line("icm20948 device_id=0x%x", data);
    imu_icm20948_write(REG_ADD_PWR_MIGMT_1,  REG_VAL_ALL_RGE_RESET);
    msleep(10);
    imu_icm20948_write(REG_ADD_PWR_MIGMT_1,  REG_VAL_RUN_MODE);

    /* user bank 2 register */
    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_2);
    imu_icm20948_write(REG_ADD_GYRO_SMPLRT_DIV, 0x07);
    imu_icm20948_write(REG_ADD_GYRO_CONFIG_1, REG_VAL_BIT_GYRO_DLPCFG_6 | REG_VAL_BIT_GYRO_FS_1000DPS | REG_VAL_BIT_GYRO_DLPF);
    imu_icm20948_write(REG_ADD_ACCEL_SMPLRT_DIV_2, 0x07);
    imu_icm20948_write(REG_ADD_ACCEL_CONFIG, REG_VAL_BIT_ACCEL_DLPCFG_6 | REG_VAL_BIT_ACCEL_FS_4g | REG_VAL_BIT_ACCEL_DLPF);

    /* user bank 0 register */
    imu_icm20948_write(REG_ADD_REG_BANK_SEL, REG_VAL_REG_BANK_0);
    msleep(100);
    icm20948_mag_check();
    invmsICM20948WriteSecondary(I2C_ADD_ICM20948_AK09916|I2C_ADD_ICM20948_AK09916_WRITE,
                                 REG_ADD_MAG_CNTL2, REG_VAL_MAG_MODE_20HZ);

    return 0;
}

static int imu_icm20948_get_gyro_data(GYRO_DATA_s *data)
{
    UINT8 high = 0, low = 0;

    if (data == NULL) {
        return -1;
    }
    imu_icm20948_read(REG_ADD_GYRO_XOUT_H, &high);
    imu_icm20948_read(REG_ADD_GYRO_XOUT_L, &low);
    data->x = (high << 8) | low;

    imu_icm20948_read(REG_ADD_GYRO_YOUT_H, &high);
    imu_icm20948_read(REG_ADD_GYRO_YOUT_L, &low);
    data->y = (high << 8) | low;

    imu_icm20948_read(REG_ADD_GYRO_ZOUT_H, &high);
    imu_icm20948_read(REG_ADD_GYRO_ZOUT_L, &low);
    data->z = (high << 8) | low;

    return 0;
}

static int imu_icm20948_get_accel_data(ACCEL_DATA_s *data)
{
    UINT8 high = 0, low = 0;

    if (data == NULL) {
        return -1;
    }
    imu_icm20948_read(REG_ADD_ACCEL_XOUT_H, &high);
    imu_icm20948_read(REG_ADD_ACCEL_XOUT_L, &low);
    data->x = (high << 8) | low;

    imu_icm20948_read(REG_ADD_ACCEL_YOUT_H, &high);
    imu_icm20948_read(REG_ADD_ACCEL_YOUT_L, &low);
    data->y = (high << 8) | low;

    imu_icm20948_read(REG_ADD_ACCEL_ZOUT_H, &high);
    imu_icm20948_read(REG_ADD_ACCEL_ZOUT_L, &low);
    data->z = (high << 8) | low;

    return 0;
}

static int imu_icm20948_get_mag_data(MAG_DATA_s *data)
{
    unsigned char counter = 20;
    unsigned char u8Data[6] = {0};

    if (data == NULL) {
        return -1;
    }

    while (counter > 0) {
        msleep(10);
        invmsICM20948ReadSecondary(I2C_ADD_ICM20948_AK09916|I2C_ADD_ICM20948_AK09916_READ,
                                    REG_ADD_MAG_ST2, 1, u8Data);
        if ((u8Data[0] & 0x01) != 0)
            break;
        counter--;
    }

    if (counter != 0) {
        invmsICM20948ReadSecondary(I2C_ADD_ICM20948_AK09916|I2C_ADD_ICM20948_AK09916_READ,
                                    REG_ADD_MAG_DATA,
                                    6,
                                    u8Data);
        data->x = ((int16_t)u8Data[1] << 8) | u8Data[0];
        data->y = ((int16_t)u8Data[3] << 8) | u8Data[2];
        data->z = ((int16_t)u8Data[5] << 8) | u8Data[4];
        data->x -= 3200;
        data->y -= 600;
        data->z += 600;
    } else {
        return -1;
    }

    return 0;
}

static int imu_icm20948_get_temperature_data(unsigned short *data)
{
    UINT8 high = 0, low = 0;

    if (data == NULL) {
        return -1;
    }
    imu_icm20948_read(REG_ADD_TEMP_OUT_H, &high);
    imu_icm20948_read(REG_ADD_TEMP_OUT_L, &low);
    *data = (high << 8) | low;

    return 0;
}

IMU_OBJECT_s imu_icm20948_object = {
    .name = "icm20948",
    .init = imu_icm20948_init,
    .read = imu_icm20948_read,
    .write = imu_icm20948_write,
    .modify = imu_icm20948_modify,
    .get_gyro_data = imu_icm20948_get_gyro_data,
    .get_accel_data = imu_icm20948_get_accel_data,
    .get_mag_data = imu_icm20948_get_mag_data,
    .get_temperature_data = imu_icm20948_get_temperature_data,
    .get_sensor_time = NULL,
};

