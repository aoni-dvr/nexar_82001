#include "imu_icm42631.h"
#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <imu.h>
#include "platform.h"
#include "AmbaSPI.h"
#include "Icm426xxDriver_HL.h"
#include "Icm426xxDriver_HL_apex.h"

#define DEBUG_TAG "[imu_62631]"
#define SPI_CTRL_TimeOut  1000U

static AMBA_SPI_CONFIG_s spi_ctrl = {
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

static int imu_icm42631_read(unsigned char addr, unsigned char *data, int size)
{
    UINT8 reg[2];
    UINT8 id[2] = {0};
    UINT32 actual_size = 0U;
    UINT32 num = 2U;
#define OP_READ             (1U << 7U)

    reg[0] = (UINT8)(addr | OP_READ);
    if (0U != AmbaSPI_MasterTransferD8((UINT32)spi_ctrl_select.SpiChannel, (UINT32)spi_ctrl_select.SpiSlaveID, &spi_ctrl, (UINT32)num, reg, id, &actual_size, SPI_CTRL_TimeOut)){
        debug_line(DEBUG_TAG"imu_icm42631_read failed. reg=%.2x", addr);
        return -1;
    }
    //debug_line(DEBUG_TAG"imu_icm42631_read 0x%.2x=0x%.2x", addr, id[1]);
    if (data != NULL) {
        *data = id[1];
    }

    return 0;
}

static int imu_icm42631_write(unsigned char addr, unsigned char data)
{
    UINT8 reg[2];
    UINT32 actual_size = 0U;
    UINT32 num = 2U;

    reg[0] = addr;
    reg[1] = data;
    if(0U != AmbaSPI_MasterTransferD8((UINT32)spi_ctrl_select.SpiChannel, (UINT32)spi_ctrl_select.SpiSlaveID, &spi_ctrl, (UINT32)num, reg, NULL, &actual_size, SPI_CTRL_TimeOut)){
        debug_line(DEBUG_TAG"imu_icm42631_write failed. reg=%.2x, value=%.2x", addr, data);
        return -1;
    }
    //debug_line(DEBUG_TAG"imu_icm42631_write 0x%.2x=0x%.2x", addr, data);

    return 0;
}

static int imu_icm42631_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    return 0;
}

struct inv_icm426xx icm_driver;

static int inv_io_hal_read_reg(struct inv_icm426xx_serif * serif, uint8_t reg, uint8_t * buf, unsigned int len)
{
    unsigned int i = 0;

    for (i = 0; i < len; i++) {
        imu_icm42631_read(reg + i, buf + i, 1);
    }

    return 0;
}
static int inv_io_hal_write_reg(struct inv_icm426xx_serif * serif, uint8_t reg, const uint8_t * buf, unsigned int len)
{
    unsigned int i = 0;

    for (i = 0; i < len; i++) {
        imu_icm42631_write(reg + i, *(buf + i));
    }

    return 0;
}

static int imu_icm42631_init(void)
{
    unsigned char who_am_i = 0;
    struct inv_icm426xx_serif icm_serif;
    int rc = 0;

    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_9_SPI2_SCLK);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_10_SPI2_MOSI);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_11_SPI2_MISO);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_12_SPI2_SS0);
	icm_serif.context   = 0;        /* no need */
	icm_serif.read_reg  = inv_io_hal_read_reg;
	icm_serif.write_reg = inv_io_hal_write_reg;
	icm_serif.max_read  = 32;  /* maximum number of bytes allowed per serial read */
	icm_serif.max_write = 32;  /* maximum number of bytes allowed per serial write */
	icm_serif.serif_type = ICM426XX_UI_SPI4;
	rc = inv_icm426xx_init(&icm_driver, &icm_serif, NULL);
	if (rc != INV_ERROR_SUCCESS) {
		debug_line(DEBUG_TAG"!!! ERROR : failed to initialize Icm426xx. rc=%d", rc);
		return rc;
    }
	rc = inv_icm426xx_get_who_am_i(&icm_driver, &who_am_i);
	if (rc != INV_ERROR_SUCCESS) {
		debug_line("!!! ERROR : failed to read Icm426xx whoami value. rc=%d", rc);
		return rc;
	}
	if (who_am_i != ICM_WHOAMI) {
		debug_line(DEBUG_TAG"!!! ERROR :  bad WHOAMI value. Got 0x%.2x (expected: 0x%.2x)", who_am_i, ICM_WHOAMI);
		return INV_ERROR;
	}
	rc |= inv_icm426xx_set_accel_fsr(&icm_driver, ICM426XX_ACCEL_CONFIG0_FS_SEL_16g);
	rc |= inv_icm426xx_set_gyro_fsr(&icm_driver, ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps);
	rc |= inv_icm426xx_set_accel_frequency(&icm_driver, ICM426XX_ACCEL_CONFIG0_ODR_50_HZ);
	rc |= inv_icm426xx_set_gyro_frequency(&icm_driver, ICM426XX_GYRO_CONFIG0_ODR_50_HZ);
	rc |= inv_icm426xx_enable_accel_low_noise_mode(&icm_driver);
	rc |= inv_icm426xx_enable_gyro_low_noise_mode(&icm_driver);
    inv_icm426xx_reset_fifo(&icm_driver);

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

static int imu_icm42631_get_gyro_accel_data(GYRO_DATA_s *gyro, ACCEL_DATA_s *accel, int raw_data)
{
    inv_icm426xx_sensor_event_t sensor_event;

    if (gyro == NULL || accel == NULL) {
        return -1;
    }
    inv_icm426xx_get_data_from_registers(&icm_driver, &sensor_event);
    GYRO_CONVERT(sensor_event.gyro[0], &gyro->x, &gyro->x_float);
    GYRO_CONVERT(sensor_event.gyro[1], &gyro->y, &gyro->y_float);
    GYRO_CONVERT(sensor_event.gyro[2], &gyro->z, &gyro->z_float);

    ACCEL_CONVERT(sensor_event.accel[0], &accel->x, &accel->x_float);
    ACCEL_CONVERT(sensor_event.accel[1], &accel->y, &accel->y_float);
    ACCEL_CONVERT(sensor_event.accel[2], &accel->z, &accel->z_float);

    return 0;
}

static int imu_icm42631_get_temperature_data(TEMP_DATA_s *data)
{
    UINT8 high = 0, low = 0;
    int temp = 0;

    if (data == NULL) {
        return -1;
    }
    imu_icm42631_read(MPUREG_TEMP_DATA0_UI, &high, 1);
    imu_icm42631_read(MPUREG_TEMP_DATA0_UI + 1, &low, 1);
    data->temp = (high << 8) | low;
    if (data->temp > 0x7fff) {
        temp = -(0xffff - data->temp);
    } else {
        temp = data->temp;
    }
    data->temp_float = (temp / 128) + 25;

    return 0;
}

static int imu_icm42631_power_off(int parking_level)
{
    inv_icm426xx_disable_gyro(&icm_driver);
    if (parking_level > 0) {
        unsigned char value = 0;
        inv_icm426xx_interrupt_parameter_t interrupt_parameter;
        inv_icm426xx_configure_smd_wom(&icm_driver, parking_level & 0xff, parking_level & 0xff, parking_level & 0xff, ICM426XX_SMD_CONFIG_WOM_INT_MODE_ORED, ICM426XX_SMD_CONFIG_WOM_MODE_CMP_PREV);
        interrupt_parameter.INV_ICM426XX_UI_FSYNC = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_UI_DRDY = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FIFO_THS = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FIFO_FULL = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_SMD = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_WOM_X = INV_ICM426XX_ENABLE;
        interrupt_parameter.INV_ICM426XX_WOM_Y = INV_ICM426XX_ENABLE;
        interrupt_parameter.INV_ICM426XX_WOM_Z = INV_ICM426XX_ENABLE;
        interrupt_parameter.INV_ICM426XX_STEP_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_STEP_CNT_OVFL = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_TILT_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FF_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_LOWG_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_TAP_DET = INV_ICM426XX_DISABLE;
        inv_icm426xx_set_config_int1(&icm_driver, &interrupt_parameter);
        inv_icm426xx_enable_accel_low_power_mode(&icm_driver);
        imu_icm42631_read(MPUREG_INT_CONFIG, &value, 1);
        value |= 0x04;
        imu_icm42631_write(MPUREG_INT_CONFIG, value);
        inv_icm426xx_enable_wom(&icm_driver);
    } else {
        inv_icm426xx_interrupt_parameter_t interrupt_parameter;
        interrupt_parameter.INV_ICM426XX_UI_FSYNC = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_UI_DRDY = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FIFO_THS = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FIFO_FULL = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_SMD = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_WOM_X = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_WOM_Y = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_WOM_Z = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_STEP_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_STEP_CNT_OVFL = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_TILT_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_FF_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_LOWG_DET = INV_ICM426XX_DISABLE;
        interrupt_parameter.INV_ICM426XX_TAP_DET = INV_ICM426XX_DISABLE;
        inv_icm426xx_set_config_int1(&icm_driver, &interrupt_parameter);
        inv_icm426xx_disable_accel(&icm_driver);
    }
    return 0;
}

static int imu_icm42631_enable_int(void)
{
    inv_icm426xx_interrupt_parameter_t interrupt_parameter;

    interrupt_parameter.INV_ICM426XX_UI_FSYNC = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_UI_DRDY = INV_ICM426XX_ENABLE;
    interrupt_parameter.INV_ICM426XX_FIFO_THS = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_FIFO_FULL = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_SMD = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_WOM_X = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_WOM_Y = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_WOM_Z = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_STEP_DET = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_STEP_CNT_OVFL = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_TILT_DET = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_FF_DET = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_LOWG_DET = INV_ICM426XX_DISABLE;
    interrupt_parameter.INV_ICM426XX_TAP_DET = INV_ICM426XX_DISABLE;
    inv_icm426xx_set_config_int1(&icm_driver, &interrupt_parameter);
    interrupt_parameter.INV_ICM426XX_UI_DRDY = INV_ICM426XX_DISABLE;
    inv_icm426xx_set_config_int2(&icm_driver, &interrupt_parameter);

    return 0;
}

static int imu_icm42631_set_enable(unsigned char enable)
{
    if (enable) {
        imu_icm42631_init();
    } else {
        inv_icm426xx_disable_gyro(&icm_driver);
        inv_icm426xx_disable_accel(&icm_driver);
    }

    return 0;
}

IMU_OBJECT_s imu_icm42631_object = {
    .name = "icm42631",
    .init = imu_icm42631_init,
    .read = imu_icm42631_read,
    .write = imu_icm42631_write,
    .modify = imu_icm42631_modify,
    .get_gyro_accel_data = imu_icm42631_get_gyro_accel_data,
    .get_mag_data = NULL,
    .get_temperature_data = imu_icm42631_get_temperature_data,
    .power_off = imu_icm42631_power_off,
    .set_accel_data = NULL,
    .set_gyro_data = NULL,
    .set_calibration = NULL,
    .set_axis_polarity = NULL,
    .enable_int = imu_icm42631_enable_int,
    .get_setting = NULL,
    .set_calibration_data = NULL,
    .get_sensor_time = NULL,
    .set_enable = imu_icm42631_set_enable,
};

