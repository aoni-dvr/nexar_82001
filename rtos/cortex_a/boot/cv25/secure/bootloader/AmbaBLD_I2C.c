#include "AmbaRTSL_I2C.h"
#include "AmbaCSL_RCT.h"

static void delay_us(unsigned int us)
{
    unsigned int cnt = us * 24;

    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
    AmbaCSL_RctTimer0Enable();  /* enable RCT Timer */
    while (AmbaCSL_RctTimer0GetCounter() < cnt);
    AmbaCSL_RctTimer0Reset();   /* reset RCT Timer */
}

void delay_ms(unsigned int ms)
{
    unsigned int i = 0;
    for (i = ms; i > 0; i--) {
        delay_us(1000);
    }
}

static int finished = 0;
static void AmbaI2c_Hook_Handler(UINT32 ChannelNum)
{
    (void)ChannelNum;
    finished = 1;
}

static int bld_i2c_init(void)
{
    static int init = 0;
    
    if (init == 0) {
        init = 1;
        AmbaRTSL_I2cInit();
        AmbaRTSL_I2cIntHookHandler(AmbaI2c_Hook_Handler);
        finished = 0;
    }

    return 0;
}

int bld_i2c_write(unsigned char slave_addr, unsigned char reg, unsigned char value)
{
    AMBA_I2C_TRANSACTION_s i2c_config;
    unsigned char tx_buf[2] = {0};

    bld_i2c_init();
    tx_buf[0] = reg;
    tx_buf[1] = value;
    i2c_config.SlaveAddr = slave_addr;
    i2c_config.pDataBuf = (unsigned char *)tx_buf;
    i2c_config.DataSize = 2;
    finished = 0;
    AmbaRTSL_I2cWrite(MAIN_MCU_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, &i2c_config);
    while (1) {
        while (AmbaRTSL_I2cGetIrqStatus(MAIN_MCU_I2C_CHANNEL) == 0) {
            //AmbaPrint_PrintStr5Fwprog("\r\n- 11111111111111111111111111 -\r\n", NULL, NULL, NULL, NULL, NULL);
        };
        AmbaRTSL_I2cMasterIntHandler(0, MAIN_MCU_I2C_CHANNEL);
        if (finished) {
            break;
        }
    }
    AmbaRTSL_I2cGetResponse(MAIN_MCU_I2C_CHANNEL);
    AmbaRTSL_I2cStop(MAIN_MCU_I2C_CHANNEL);

    return 0;
}

int bld_i2c_read(unsigned char slave_addr, unsigned char reg, unsigned char *value)
{

    AMBA_I2C_TRANSACTION_s i2c_tx_config;
    AMBA_I2C_TRANSACTION_s i2c_rx_config;
    unsigned char rx_data_buf[1] = {0};

    bld_i2c_init();

    finished = 0;
    i2c_tx_config.SlaveAddr = slave_addr;
    i2c_tx_config.pDataBuf = &reg;
    i2c_tx_config.DataSize  = 1;
    AmbaRTSL_I2cWrite(MAIN_MCU_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, &i2c_tx_config);
    while (1) {
        while (AmbaRTSL_I2cGetIrqStatus(MAIN_MCU_I2C_CHANNEL) == 0) {
            //AmbaPrint_PrintStr5Fwprog("\r\n- 11111111111111111111111111 -\r\n", NULL, NULL, NULL, NULL, NULL);
        };
        AmbaRTSL_I2cMasterIntHandler(0, MAIN_MCU_I2C_CHANNEL);
        if (finished) {
            break;
        }
    }
    AmbaRTSL_I2cGetResponse(MAIN_MCU_I2C_CHANNEL);
    AmbaRTSL_I2cStop(MAIN_MCU_I2C_CHANNEL);

    delay_ms(10);

    i2c_rx_config.SlaveAddr = slave_addr | 0x01;
    i2c_rx_config.pDataBuf = rx_data_buf;
    i2c_rx_config.DataSize  = 1;            
    finished = 0;
    AmbaRTSL_I2cRead(MAIN_MCU_I2C_CHANNEL, AMBA_I2C_SPEED_STANDARD, &i2c_rx_config);
    while (1) {
        while (AmbaRTSL_I2cGetIrqStatus(MAIN_MCU_I2C_CHANNEL) == 0) {
            //AmbaPrint_PrintUInt5Fwprog("\r\n- Bert AmbaRTSL_I2cGetValue, Status=%d -\r\n", AmbaRTSL_I2cGetIrqStatus(AMBA_I2C_CHANNEL2), 0, 0, 0, 0);
        }
        AmbaRTSL_I2cMasterIntHandler(0, MAIN_MCU_I2C_CHANNEL);
        if (finished) {
            break;
        }
    }
    AmbaRTSL_I2cGetResponse(MAIN_MCU_I2C_CHANNEL);
    AmbaRTSL_I2cStop(MAIN_MCU_I2C_CHANNEL);
    if (value != NULL) {
        *value = rx_data_buf[0];
    }

    return 0;
}
