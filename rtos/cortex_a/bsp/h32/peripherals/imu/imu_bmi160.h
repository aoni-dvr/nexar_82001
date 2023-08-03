#ifndef __IMU_BMI160_H__
#define __IMU_BMI160_H__

#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include "bmi160.h"
#include <bsp.h>

#if 0
#define BMI160_CHIP_ID_ADDR             0x00
#define BMI160_ERROR_REG_ADDR           0x02
#define BMI160_AUX_DATA_ADDR            0x04
#define BMI160_GYRO_DATA_ADDR           0x0C
#define BMI160_ACCEL_DATA_ADDR          0x12
#define BMI160_STATUS_ADDR              0x1B
#define BMI160_INT_STATUS_ADDR          0x1C
#define BMI160_TEMP_DATA_ADDR           0x20
#define BMI160_FIFO_LENGTH_ADDR         0x22
#define BMI160_FIFO_DATA_ADDR           0x24
#define BMI160_ACCEL_CONFIG_ADDR        0x40
#define BMI160_ACCEL_RANGE_ADDR         0x41
#define BMI160_GYRO_CONFIG_ADDR         0x42
#define BMI160_GYRO_RANGE_ADDR          0x43
#define BMI160_AUX_ODR_ADDR             0x44
#define BMI160_FIFO_DOWN_ADDR           0x45
#define BMI160_FIFO_CONFIG_0_ADDR       0x46
#define BMI160_FIFO_CONFIG_1_ADDR       0x47
#define BMI160_AUX_IF_0_ADDR            0x4B
#define BMI160_AUX_IF_1_ADDR            0x4C
#define BMI160_AUX_IF_2_ADDR            0x4D
#define BMI160_AUX_IF_3_ADDR            0x4E
#define BMI160_AUX_IF_4_ADDR            0x4F
#define BMI160_INT_ENABLE_0_ADDR        0x50
#define BMI160_INT_ENABLE_1_ADDR        0x51
#define BMI160_INT_ENABLE_2_ADDR        0x52
#define BMI160_INT_OUT_CTRL_ADDR        0x53
#define BMI160_INT_LATCH_ADDR           0x54
#define BMI160_INT_MAP_0_ADDR           0x55
#define BMI160_INT_MAP_1_ADDR           0x56
#define BMI160_INT_MAP_2_ADDR           0x57
#define BMI160_INT_DATA_0_ADDR          0x58
#define BMI160_INT_DATA_1_ADDR          0x59
#define BMI160_INT_LOWHIGH_0_ADDR       0x5A
#define BMI160_INT_LOWHIGH_1_ADDR       0x5B
#define BMI160_INT_LOWHIGH_2_ADDR       0x5C
#define BMI160_INT_LOWHIGH_3_ADDR       0x5D
#define BMI160_INT_LOWHIGH_4_ADDR       0x5E
#define BMI160_INT_MOTION_0_ADDR        0x5F
#define BMI160_INT_MOTION_1_ADDR        0x60
#define BMI160_INT_MOTION_2_ADDR        0x61
#define BMI160_INT_MOTION_3_ADDR        0x62
#define BMI160_INT_TAP_0_ADDR           0x63
#define BMI160_INT_TAP_1_ADDR           0x64
#define BMI160_INT_ORIENT_0_ADDR        0x65
#define BMI160_INT_ORIENT_1_ADDR        0x66
#define BMI160_INT_FLAT_0_ADDR          0x67
#define BMI160_INT_FLAT_1_ADDR          0x68
#define BMI160_FOC_CONF_ADDR            0x69
#define BMI160_CONF_ADDR                0x6A
#define BMI160_IF_CONF_ADDR             0x6B
#define BMI160_SELF_TEST_ADDR           0x6D
#define BMI160_OFFSET_ADDR              0x71
#define BMI160_OFFSET_CONF_ADDR         0x77
#define BMI160_INT_STEP_CNT_0_ADDR      0x78
#define BMI160_INT_STEP_CONFIG_0_ADDR   0x7A
#define BMI160_INT_STEP_CONFIG_1_ADDR   0x7B
#define BMI160_COMMAND_REG_ADDR         0x7E
#define BMI160_SPI_COMM_TEST_ADDR       0x7F
#define BMI160_INTL_PULLUP_CONF_ADDR    0x85

/** BMI160 unique chip identifier */
#define BMI160_CHIP_ID                   0xD1

/** Soft reset command */
#define BMI160_SOFT_RESET_CMD           0xb6
#define BMI160_SOFT_RESET_DELAY_MS      15
/** Start FOC command */
#define BMI160_START_FOC_CMD            0x03
/** NVM backup enabling command */
#define BMI160_NVM_BACKUP_EN            0xA0

/* Delay in ms settings */
#define BMI160_ACCEL_DELAY_MS            5
#define BMI160_GYRO_DELAY_MS             81
#define BMI160_ONE_MS_DELAY              1
#define BMI160_AUX_COM_DELAY            10
#define BMI160_GYRO_SELF_TEST_DELAY     20
#define BMI160_ACCEL_SELF_TEST_DELAY    50

/** Self test configurations */
#define BMI160_ACCEL_SELF_TEST_CONFIG   0x2C
#define BMI160_ACCEL_SELF_TEST_POSITIVE_EN  0x0D
#define BMI160_ACCEL_SELF_TEST_NEGATIVE_EN  0x09
#define BMI160_ACCEL_SELF_TEST_LIMIT    UINT16_C(8192

/** Power mode settings */
/* Accel power mode */
#define BMI160_ACCEL_NORMAL_MODE         0x11
#define BMI160_ACCEL_LOWPOWER_MODE       0x12
#define BMI160_ACCEL_SUSPEND_MODE        0x10

/* Gyro power mode */
#define BMI160_GYRO_SUSPEND_MODE         0x14
#define BMI160_GYRO_NORMAL_MODE          0x15
#define BMI160_GYRO_FASTSTARTUP_MODE     0x17

/* Aux power mode */
#define BMI160_AUX_SUSPEND_MODE   0x18
#define BMI160_AUX_NORMAL_MODE    0x19
#define BMI160_AUX_LOWPOWER_MODE  0x1A

/** Range settings */
/* Accel Range */
#define BMI160_ACCEL_RANGE_2G            0x03
#define BMI160_ACCEL_RANGE_4G            0x05
#define BMI160_ACCEL_RANGE_8G            0x08
#define BMI160_ACCEL_RANGE_16G           0x0C

/* Gyro Range */
#define BMI160_GYRO_RANGE_2000_DPS       0x00
#define BMI160_GYRO_RANGE_1000_DPS       0x01
#define BMI160_GYRO_RANGE_500_DPS        0x02
#define BMI160_GYRO_RANGE_250_DPS        0x03
#define BMI160_GYRO_RANGE_125_DPS        0x04

/** Bandwidth settings */
/* Accel Bandwidth */
#define BMI160_ACCEL_BW_OSR4_AVG1        0x00
#define BMI160_ACCEL_BW_OSR2_AVG2        0x01
#define BMI160_ACCEL_BW_NORMAL_AVG4      0x02
#define BMI160_ACCEL_BW_RES_AVG8         0x03
#define BMI160_ACCEL_BW_RES_AVG16        0x04
#define BMI160_ACCEL_BW_RES_AVG32        0x05
#define BMI160_ACCEL_BW_RES_AVG64        0x06
#define BMI160_ACCEL_BW_RES_AVG128       0x07

#define BMI160_GYRO_BW_OSR4_MODE         0x00
#define BMI160_GYRO_BW_OSR2_MODE         0x01
#define BMI160_GYRO_BW_NORMAL_MODE       0x02

/* Output Data Rate settings */
/* Accel Output data rate */
#define BMI160_ACCEL_ODR_RESERVED        0x00
#define BMI160_ACCEL_ODR_0_78HZ          0x01
#define BMI160_ACCEL_ODR_1_56HZ          0x02
#define BMI160_ACCEL_ODR_3_12HZ          0x03
#define BMI160_ACCEL_ODR_6_25HZ          0x04
#define BMI160_ACCEL_ODR_12_5HZ          0x05
#define BMI160_ACCEL_ODR_25HZ            0x06
#define BMI160_ACCEL_ODR_50HZ            0x07
#define BMI160_ACCEL_ODR_100HZ           0x08
#define BMI160_ACCEL_ODR_200HZ           0x09
#define BMI160_ACCEL_ODR_400HZ           0x0A
#define BMI160_ACCEL_ODR_800HZ           0x0B
#define BMI160_ACCEL_ODR_1600HZ          0x0C
#define BMI160_ACCEL_ODR_RESERVED0       0x0D
#define BMI160_ACCEL_ODR_RESERVED1       0x0E
#define BMI160_ACCEL_ODR_RESERVED2       0x0F

/* Gyro Output data rate */
#define BMI160_GYRO_ODR_RESERVED         0x00
#define BMI160_GYRO_ODR_25HZ             0x06
#define BMI160_GYRO_ODR_50HZ             0x07
#define BMI160_GYRO_ODR_100HZ            0x08
#define BMI160_GYRO_ODR_200HZ            0x09
#define BMI160_GYRO_ODR_400HZ            0x0A
#define BMI160_GYRO_ODR_800HZ            0x0B
#define BMI160_GYRO_ODR_1600HZ           0x0C
#define BMI160_GYRO_ODR_3200HZ           0x0D

/* Auxiliary sensor Output data rate */
#define BMI160_AUX_ODR_RESERVED        0x00
#define BMI160_AUX_ODR_0_78HZ          0x01
#define BMI160_AUX_ODR_1_56HZ          0x02
#define BMI160_AUX_ODR_3_12HZ          0x03
#define BMI160_AUX_ODR_6_25HZ          0x04
#define BMI160_AUX_ODR_12_5HZ          0x05
#define BMI160_AUX_ODR_25HZ            0x06
#define BMI160_AUX_ODR_50HZ            0x07
#define BMI160_AUX_ODR_100HZ           0x08
#define BMI160_AUX_ODR_200HZ           0x09
#define BMI160_AUX_ODR_400HZ           0x0A
#define BMI160_AUX_ODR_800HZ           0x0B

/* Maximum limits definition */
#define BMI160_ACCEL_ODR_MAX             15
#define BMI160_ACCEL_BW_MAX              2
#define BMI160_ACCEL_RANGE_MAX           12
#define BMI160_GYRO_ODR_MAX              13
#define BMI160_GYRO_BW_MAX               2
#define BMI160_GYRO_RANGE_MAX            4

/** FIFO_CONFIG Definitions */
#define BMI160_FIFO_TIME_ENABLE          0x02
#define BMI160_FIFO_TAG_INT2_ENABLE      0x04
#define BMI160_FIFO_TAG_INT1_ENABLE      0x08
#define BMI160_FIFO_HEAD_ENABLE          0x10
#define BMI160_FIFO_M_ENABLE             0x20
#define BMI160_FIFO_A_ENABLE             0x40
#define BMI160_FIFO_M_A_ENABLE           0x60
#define BMI160_FIFO_G_ENABLE             0x80
#define BMI160_FIFO_M_G_ENABLE           0xA0
#define BMI160_FIFO_G_A_ENABLE           0xC0
#define BMI160_FIFO_M_G_A_ENABLE         0xE0


/* Accel, gyro and aux. sensor length and also their combined
 * length definitions in FIFO */
#define BMI160_FIFO_G_LENGTH             6
#define BMI160_FIFO_A_LENGTH             6
#define BMI160_FIFO_M_LENGTH             8
#define BMI160_FIFO_GA_LENGTH            12
#define BMI160_FIFO_MA_LENGTH            14
#define BMI160_FIFO_MG_LENGTH            14
#define BMI160_FIFO_MGA_LENGTH           20

/** FIFO Header Data definitions */
#define BMI160_FIFO_HEAD_SKIP_FRAME      0x40
#define BMI160_FIFO_HEAD_SENSOR_TIME     0x44
#define BMI160_FIFO_HEAD_INPUT_CONFIG    0x48
#define BMI160_FIFO_HEAD_OVER_READ       0x80
#define BMI160_FIFO_HEAD_A               0x84
#define BMI160_FIFO_HEAD_G               0x88
#define BMI160_FIFO_HEAD_G_A             0x8C
#define BMI160_FIFO_HEAD_M               0x90
#define BMI160_FIFO_HEAD_M_A             0x94
#define BMI160_FIFO_HEAD_M_G             0x98
#define BMI160_FIFO_HEAD_M_G_A           0x9C

/** FIFO sensor time length definitions */
#define BMI160_SENSOR_TIME_LENGTH        3

/** FIFO DOWN selection */
/* Accel fifo down-sampling values*/
#define  BMI160_ACCEL_FIFO_DOWN_ZERO     0x00
#define  BMI160_ACCEL_FIFO_DOWN_ONE      0x10
#define  BMI160_ACCEL_FIFO_DOWN_TWO      0x20
#define  BMI160_ACCEL_FIFO_DOWN_THREE    0x30
#define  BMI160_ACCEL_FIFO_DOWN_FOUR     0x40
#define  BMI160_ACCEL_FIFO_DOWN_FIVE     0x50
#define  BMI160_ACCEL_FIFO_DOWN_SIX      0x60
#define  BMI160_ACCEL_FIFO_DOWN_SEVEN    0x70

/* Gyro fifo down-smapling values*/
#define  BMI160_GYRO_FIFO_DOWN_ZERO      0x00
#define  BMI160_GYRO_FIFO_DOWN_ONE       0x01
#define  BMI160_GYRO_FIFO_DOWN_TWO       0x02
#define  BMI160_GYRO_FIFO_DOWN_THREE     0x03
#define  BMI160_GYRO_FIFO_DOWN_FOUR      0x04
#define  BMI160_GYRO_FIFO_DOWN_FIVE      0x05
#define  BMI160_GYRO_FIFO_DOWN_SIX       0x06
#define  BMI160_GYRO_FIFO_DOWN_SEVEN     0x07

/* Accel Fifo filter enable*/
#define  BMI160_ACCEL_FIFO_FILT_EN       0x80

/* Gyro Fifo filter enable*/
#define  BMI160_GYRO_FIFO_FILT_EN        0x08

/** Definitions to check validity of FIFO frames */
#define FIFO_CONFIG_MSB_CHECK            0x80
#define FIFO_CONFIG_LSB_CHECK            0x00

/*! BMI160 accel FOC configurations */
#define BMI160_FOC_ACCEL_DISABLED        0x00
#define BMI160_FOC_ACCEL_POSITIVE_G      0x01
#define BMI160_FOC_ACCEL_NEGATIVE_G      0x02
#define BMI160_FOC_ACCEL_0G              0x03

/** Array Parameter DefinItions */
#define BMI160_SENSOR_TIME_LSB_BYTE      0
#define BMI160_SENSOR_TIME_XLSB_BYTE     1
#define BMI160_SENSOR_TIME_MSB_BYTE      2

/** Interface settings */
#define BMI160_SPI_INTF                  1
#define BMI160_I2C_INTF                  0
#define BMI160_SPI_RD_MASK               0x80
#define BMI160_SPI_WR_MASK               0x7F

/* Sensor & time select definition*/
#define BMI160_ACCEL_SEL    0x01
#define BMI160_GYRO_SEL     0x02
#define BMI160_TIME_SEL     0x04

/* Sensor select mask*/
#define BMI160_SEN_SEL_MASK   0x07

/* Error code mask */
#define BMI160_ERR_REG_MASK   0x0F

/* BMI160 I2C address */
#define BMI160_I2C_ADDR                 0x68

/* BMI160 secondary IF address */
#define BMI160_AUX_BMM150_I2C_ADDR    0x10

/** BMI160 Length definitions */
#define BMI160_ONE                       1
#define BMI160_TWO                       2
#define BMI160_THREE                     3
#define BMI160_FOUR                      4
#define BMI160_FIVE                      5

/** BMI160 fifo level Margin */
#define BMI160_FIFO_LEVEL_MARGIN         16

/** BMI160 fifo flush Command */
#define BMI160_FIFO_FLUSH_VALUE          0xB0

/** BMI160 offset values for xyz axes of accel */
#define BMI160_ACCEL_MIN_OFFSET         -128
#define BMI160_ACCEL_MAX_OFFSET         127

/** BMI160 offset values for xyz axes of gyro */
#define BMI160_GYRO_MIN_OFFSET         -512
#define BMI160_GYRO_MAX_OFFSET         511

/** BMI160 fifo full interrupt position and mask */
#define BMI160_FIFO_FULL_INT_POS  5
#define BMI160_FIFO_FULL_INT_MSK  0x20
#define BMI160_FIFO_WTM_INT_POS   6
#define BMI160_FIFO_WTM_INT_MSK   0x40

#define BMI160_FIFO_FULL_INT_PIN1_POS 5
#define BMI160_FIFO_FULL_INT_PIN1_MSK 0x20
#define BMI160_FIFO_FULL_INT_PIN2_POS 1
#define BMI160_FIFO_FULL_INT_PIN2_MSK 0x02

#define BMI160_FIFO_WTM_INT_PIN1_POS  6
#define BMI160_FIFO_WTM_INT_PIN1_MSK  0x40
#define BMI160_FIFO_WTM_INT_PIN2_POS  2
#define BMI160_FIFO_WTM_INT_PIN2_MSK  0x04

#define BMI160_MANUAL_MODE_EN_POS 7
#define BMI160_MANUAL_MODE_EN_MSK 0x80
#define BMI160_AUX_READ_BURST_POS 0
#define BMI160_AUX_READ_BURST_MSK 0x03

#define BMI160_GYRO_SELF_TEST_POS 4
#define BMI160_GYRO_SELF_TEST_MSK 0x10
#define BMI160_GYRO_SELF_TEST_STATUS_POS  1
#define BMI160_GYRO_SELF_TEST_STATUS_MSK  0x02

#define BMI160_GYRO_FOC_EN_POS  6
#define BMI160_GYRO_FOC_EN_MSK  0x40

#define BMI160_ACCEL_FOC_X_CONF_POS 4
#define BMI160_ACCEL_FOC_X_CONF_MSK 0x30

#define BMI160_ACCEL_FOC_Y_CONF_POS 2
#define BMI160_ACCEL_FOC_Y_CONF_MSK 0x0C

#define BMI160_ACCEL_FOC_Z_CONF_MSK 0x03

#define BMI160_FOC_STATUS_POS 3
#define BMI160_FOC_STATUS_MSK 0x08

#define BMI160_GYRO_OFFSET_X_MSK  0x03

#define BMI160_GYRO_OFFSET_Y_POS  2
#define BMI160_GYRO_OFFSET_Y_MSK  0x0C

#define BMI160_GYRO_OFFSET_Z_POS  4
#define BMI160_GYRO_OFFSET_Z_MSK  0x30

#define BMI160_GYRO_OFFSET_EN_POS 7
#define BMI160_GYRO_OFFSET_EN_MSK 0x80

#define BMI160_ACCEL_OFFSET_EN_POS  6
#define BMI160_ACCEL_OFFSET_EN_MSK  0x40

#define BMI160_GYRO_OFFSET_POS          8
#define BMI160_GYRO_OFFSET_MSK          0x0300

#define BMI160_NVM_UPDATE_POS         1
#define BMI160_NVM_UPDATE_MSK         0x02

#define BMI160_NVM_STATUS_POS         4
#define BMI160_NVM_STATUS_MSK         0x10
#endif

#endif//__IMU_BMI160_H__

