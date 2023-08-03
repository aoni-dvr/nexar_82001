#include "imu_icm42670.h"
#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <imu.h>
#include "platform.h"
#include "AmbaSPI.h"

#define SPI_CTRL_TimeOut  1000U

static AMBA_SPI_CONFIG_s imu42670_spi_ctrl = {
    .BaudRate       = 1000000U,                                 /* Transfer BaudRate in Hz  1000000*/
    .CsPolarity     = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,          /* Slave select polarity */
    .ClkMode        = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,             /* SPI Protocol mode */
    .DataFrameSize  = 8U,                                       /* Data Frame Size in Bit */
    .FrameBitOrder  = AMBA_SPI_TRANSFER_MSB_FIRST,
};

typedef struct {
    UINT8 SpiChannel;
    UINT8 SpiSlaveID;
} IMU_SPI_CTRL_SELECTs;

static IMU_SPI_CTRL_SELECTs spi_ctrl_select = {
    .SpiChannel = AMBA_SPI_MASTER2,
    .SpiSlaveID = 0x1U,
};

static int imu_icm42670_read(unsigned char addr, unsigned char *data, int size)
{
    UINT8 reg[2];
    UINT8 id[2] = {0};
    UINT32 actual_size = 0U;
    UINT32 num = 2U;
#define OP_READ             (1U << 7U)

    reg[0] = (UINT8)(addr | OP_READ);
    if (0U != AmbaSPI_MasterTransferD8((UINT32)spi_ctrl_select.SpiChannel, (UINT32)spi_ctrl_select.SpiSlaveID, &imu42670_spi_ctrl, (UINT32)num, reg, id, &actual_size, SPI_CTRL_TimeOut)){
        debug_line("imu_icm42670_read failed. reg=%.2x", addr);
        return -1;
    }
    //debug_line("imu_icm42670_read 0x%.2x=0x%.2x", addr, id[1]);
    if (data != NULL) {
        *data = id[1];
    }

    return 0;
}

static int imu_icm42670_write(unsigned char addr, unsigned char data)
{
    UINT8 reg[2];
    UINT32 actual_size = 0U;
    UINT32 num = 2U;

    reg[0] = addr;
    reg[1] = data;
    if(0U != AmbaSPI_MasterTransferD8((UINT32)spi_ctrl_select.SpiChannel, (UINT32)spi_ctrl_select.SpiSlaveID, &imu42670_spi_ctrl, (UINT32)num, reg, NULL, &actual_size, SPI_CTRL_TimeOut)){
        debug_line("imu_icm42670_write failed. reg=%.2x, value=%.2x", addr, data);
        return -1;
    }
    //debug_line("imu_icm42670_write 0x%.2x=0x%.2x", addr, data);

    return 0;
}

static int imu_icm42670_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    return 0;
}

static void inv_set_idle(void)
{
    unsigned char data = 0;
    unsigned char value = 0;

    imu_icm42670_read(REG_PWR_MGMT_0, &data, 1);
    value = data;
    value |= BIT_IDLE;
    if ((value & BIT_ACCEL_MODE_MASK) == BIT_ACCEL_MODE_LPM && (value & BIT_GYRO_MODE_MASK) == 0) {
        value |= BIT_ACCEL_LP_CLK_SEL;
    } else  {
        imu_icm42670_write(REG_PWR_MGMT_0, value);
        msleep(1);
    }
}

static void icm42670_mreg1_register_write(unsigned char reg, unsigned char value)
{
    unsigned char data = 0;

    imu_icm42670_read(REG_PWR_MGMT_0, &data, 1);
    inv_set_idle();

    imu_icm42670_write(REG_BLK_SEL_W, 0x00);
    msleep(1);
    imu_icm42670_write(REG_MADDR_W, reg);
    msleep(1);
    imu_icm42670_write(REG_M_W, value);
    msleep(1);
    imu_icm42670_write(REG_BLK_SEL_W, 0x00);
    imu_icm42670_write(REG_BLK_SEL_R, 0x00);
    imu_icm42670_write(REG_PWR_MGMT_0, data);
}

static int imu_icm42670_init(void)
{
    unsigned char data = 0;

    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_9_SPI2_SCLK);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_10_SPI2_MOSI);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_11_SPI2_MISO);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_12_SPI2_SS0);
    imu_icm42670_read(REG_WHO_AM_I, &data, 1);
    if (data != 0x60) {
        debug_line("icm42670 read id fail: 0x%.2x != 0x60", data);
    }
    imu_icm42670_read(REG_INT_STATUS2, &data, 1);
    if ((data & BIT_INT_WOM_XYZ_INT1_EN) != 0) {
        debug_line("imu wakeup, data=0x%.2x..........................", data);
    }
    imu_icm42670_write(REG_SIGNAL_PATH_RESET, BIT_SOFT_RESET_CHIP_CONFIG);
    msleep(500);

    imu_icm42670_write(REG_INTF_CONFIG0, BIT_FIFO_COUNT_ENDIAN | BIT_SENSOR_DATA_ENDIAN);
    imu_icm42670_write(REG_INTF_CONFIG1, BIT_CLK_SEL_PLL);

    imu_icm42670_write(REG_APEX_CONFIG0, BIT_DMP_SRAM_RESET_APEX);
    msleep(1);
    imu_icm42670_write(REG_APEX_CONFIG1, BIT_DMP_ODR_50HZ);

    imu_icm42670_write(REG_GYRO_CONFIG0, 0x0A);
    imu_icm42670_write(REG_ACCEL_CONFIG0, 0x0A);

    icm42670_mreg1_register_write(REG_TMST_CONFIG1_MREG_TOP1, BIT_TMST_EN);
    imu_icm42670_write(REG_FIFO_CONFIG1, BIT_FIFO_MODE_BYPASS);
    icm42670_mreg1_register_write(REG_FIFO_CONFIG5_MREG_TOP1, 0x00);
    imu_icm42670_write(REG_PWR_MGMT_0, 0x00);
    imu_icm42670_write(REG_PWR_MGMT_0, 0x0f);

    imu_icm42670_write(REG_INT_CONFIG_REG, (INT_POLARITY << SHIFT_INT1_POLARITY) | (INT_DRIVE_CIRCUIT << SHIFT_INT1_DRIVE_CIRCUIT) | (INT_MODE << SHIFT_INT1_MODE));

    return 0;
}

static void ACCEL_CONVERT(unsigned short value, unsigned short *x, float *x_float)
{
    int val = value;
    float val_float = val * 2000 * 1.0;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    val_float = (val * 9.8) / (0x8000 / 16);
    if (x != NULL) *x = value;
    if (x_float != NULL) *x_float = val_float;
}

static void GYRO_CONVERT(unsigned short value, unsigned short *x, float *x_float)
{
    int val = value;
    float val_float = val * 2000 * 1.0;

    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    val_float = val * 2000 * 1.0 / 0x7fff;
    if (x != NULL) *x = value;
    if (x_float != NULL) *x_float = val_float;
}

static int imu_icm42670_get_gyro_accel_data(GYRO_DATA_s *gyro, ACCEL_DATA_s *accel, int raw_data)
{
    UINT8 high = 0, low = 0;
    unsigned short gyro_x = 0, gyro_y = 0, gyro_z = 0;
    unsigned short accel_x = 0, accel_y = 0, accel_z = 0;

    if (gyro == NULL || accel == NULL) {
        return -1;
    }
    imu_icm42670_read(REG_GYRO_DATA_X0_UI, &high, 1);
    imu_icm42670_read(REG_GYRO_DATA_X1_UI, &low, 1);
    gyro_x = (high << 8) | low;

    imu_icm42670_read(REG_GYRO_DATA_Y0_UI, &high, 1);
    imu_icm42670_read(REG_GYRO_DATA_Y1_UI, &low, 1);
    gyro_y = (high << 8) | low;

    imu_icm42670_read(REG_GYRO_DATA_Z0_UI, &high, 1);
    imu_icm42670_read(REG_GYRO_DATA_Z1_UI, &low, 1);
    gyro_z = (high << 8) | low;

    imu_icm42670_read(REG_ACCEL_DATA_X0_UI, &high, 1);
    imu_icm42670_read(REG_ACCEL_DATA_X1_UI, &low, 1);
    accel_x = (high << 8) | low;

    imu_icm42670_read(REG_ACCEL_DATA_Y0_UI, &high, 1);
    imu_icm42670_read(REG_ACCEL_DATA_Y1_UI, &low, 1);
    accel_y = (high << 8) | low;

    imu_icm42670_read(REG_ACCEL_DATA_Z0_UI, &high, 1);
    imu_icm42670_read(REG_ACCEL_DATA_Z1_UI, &low, 1);
    accel_z = (high << 8) | low;

    GYRO_CONVERT(gyro_x, &gyro->x, &gyro->x_float);
    GYRO_CONVERT(gyro_y, &gyro->y, &gyro->y_float);
    GYRO_CONVERT(gyro_z, &gyro->z, &gyro->z_float);

    ACCEL_CONVERT(accel_x, &accel->x, &accel->x_float);
    ACCEL_CONVERT(accel_y, &accel->y, &accel->y_float);
    ACCEL_CONVERT(accel_z, &accel->z, &accel->z_float);

    return 0;
}

static int imu_icm42670_get_temperature_data(TEMP_DATA_s *data)
{
    UINT8 high = 0, low = 0;
    int temp = 0;

    if (data == NULL) {
        return -1;
    }
    imu_icm42670_read(REG_TEMP_DATA0_UI, &high, 1);
    imu_icm42670_read(REG_TEMP_DATA1_UI, &low, 1);
    data->temp = (high << 8) | low;
    if (data->temp > 0x7fff) {
        temp = -(0xffff - data->temp);
    } else {
        temp = data->temp;
    }
    data->temp_float = (temp / 128) + 25;

    return 0;
}

static int imu_icm42670_power_off(int parking_level)
{
    imu_icm42670_write(REG_WOM_CONFIG, 0x00);
    if (parking_level > 0) {
        imu_icm42670_write(REG_INT_SOURCE1, BIT_INT_WOM_XYZ_INT1_EN);
        icm42670_mreg1_register_write(REG_ACCEL_WOM_X_THR_MREG_TOP1, parking_level & 0xff);
        icm42670_mreg1_register_write(REG_ACCEL_WOM_Y_THR_MREG_TOP1, parking_level & 0xff);
        icm42670_mreg1_register_write(REG_ACCEL_WOM_Z_THR_MREG_TOP1, parking_level & 0xff);
        icm42670_mreg1_register_write(REG_INT_SOURCE9_MREG_TOP1, 0x0e);
        imu_icm42670_write(REG_WOM_CONFIG, BIT_WOM_EN_ON | BIT_WOM_MODE_PREV);
    }

    return 0;
}

static int imu_icm42670_enable_int(void)
{
    imu_icm42670_write(REG_INT_SOURCE3, BIT_INT_DRDY_INT_EN);

    return 0;
}

IMU_OBJECT_s imu_icm42670_object = {
    .name = "icm42670",
    .init = imu_icm42670_init,
    .read = imu_icm42670_read,
    .write = imu_icm42670_write,
    .modify = imu_icm42670_modify,
    .get_gyro_accel_data = imu_icm42670_get_gyro_accel_data,
    .get_mag_data = NULL,
    .get_temperature_data = imu_icm42670_get_temperature_data,
    .power_off = imu_icm42670_power_off,
    .set_accel_data = NULL,
    .set_gyro_data = NULL,
    .set_calibration = NULL,
    .set_axis_polarity = NULL,
    .enable_int = imu_icm42670_enable_int,
    .get_setting = NULL,
    .set_calibration_data = NULL,
    .get_sensor_time = NULL,
};

