#include "pmic_chipon.h"
#include "AmbaI2C.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <pmic.h>
#include <bsp.h>

#define I2C_SLAVE_ADDR MAIN_MCU_I2C_SLAVE_ADDR

static int pmic_chipon_read(unsigned char addr, unsigned char *data)
{
    unsigned char rx_data = 0;

    if (i2c_read_reg(MAIN_MCU_I2C_CHANNEL, I2C_SLAVE_ADDR, addr, &rx_data) >= 0) {
        if (data != NULL) {
            *data = rx_data;
            return 0;
        }
    }

    return -1;
}

static int pmic_chipon_read_retry(unsigned char addr, unsigned char *data)
{
    unsigned char times = 0;
    unsigned char value = 0;

    while (times <= 10) {
        pmic_chipon_read(addr, &value);
        if (value != 0xff) {
            break;
        }
        times++;
        msleep(100); 
    }
    if (data != NULL) {
        *data = value;
    }

    return 0;
}

static int pmic_chipon_write(unsigned char addr, unsigned char data)
{
    unsigned char tx_buf[2] = {0};

    //debug_line("pmic_chipon_write:0x%.2X=0x%.2X", addr, data);
    tx_buf[0] = (unsigned char)addr;
    tx_buf[1] = (unsigned char)data;
    i2c_write(MAIN_MCU_I2C_CHANNEL, I2C_SLAVE_ADDR, tx_buf, 2);

    return 0;
}

static int pmic_chipon_modify(unsigned char Addr, unsigned char Mask, unsigned char Data)
{
    unsigned char val = 0;

    pmic_chipon_read_retry(Addr, &val);
    val &= ~Mask;
    val |= Data;
    pmic_chipon_write(Addr, val);

    return 0;
}

static int pmic_chipon_dump_regs(void)
{
    return 0;
}

static int pmic_chipon_get_lcd_backlight_enable(void)
{
    return 0;
}

static int pmic_chipon_set_lcd_backlight_enable(int enable)
{
    return 0;
}

static int pmic_chipon_do_system_power_off(int param)
{
    pmic_chipon_write(0x01, param & 0xFF);
    msleep(500);

    return 0;
}

static int pmic_chipon_get_sram_reg(void)
{
    unsigned char value = 0;

    pmic_chipon_read_retry(0x08, &value);

    return value;
}

static int pmic_chipon_set_sram_reg(int val)
{
    pmic_chipon_write(0x09, val & 0xFF);

    return 0;
}

static int pmic_chipon_set_softreset_flag(void)
{
    return 0;
}

static int pmic_chipon_reset(void)
{
    pmic_chipon_write(0x02, 0x01);

    return 0;
}

static int pmic_chipon_check_usb_connected(void)
{
    extern UINT32 AmbaRTSL_UsbVbusVicRawStaus(void);
    return AmbaRTSL_UsbVbusVicRawStaus();
}

static int pmic_chipon_set_led(int on)
{
    return 0;
}

static int pmic_chipon_get_version(char *version)
{
    static unsigned char tmp_ver[32] = {0};
    static int ver_getted = 0;
    unsigned char data = 0;

    if (ver_getted) {
        if (version != NULL) {
            //debug_line("tmp_ver:%s", (char *)tmp_ver);
            memcpy(version, (char *)tmp_ver, strlen((char *)tmp_ver));
        }
        return 0;
    }
    pmic_chipon_read_retry(0x00, &data);
    if (data != 0xff) {
        memset(tmp_ver, 0, sizeof(tmp_ver));
        snprintf((char *)tmp_ver, sizeof(tmp_ver) - 1, "V%d.%d", (data >> 4) & 0xF, data & 0xF);
        if (version != NULL) {
            //debug_line("tmp_ver:%s", (char *)tmp_ver);
            memcpy(version, (char *)tmp_ver, strlen((char *)tmp_ver));
        }
        ver_getted = 1;
    } else {
        debug_line("%s failed", __func__);
    }

    return 0;
}

static int pmic_chipon_init(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    unsigned char value = 0;

    pmic_chipon_read_retry(0x0f, &value);
    value |= 0x0d;
    value &= ~0x02;
    pmic_chipon_write(0x0e, value);
#endif

    return 0;
}

static int pmic_chipon_check_power_key_pressed(void)
{
    return 0;
}

static int pmic_chipon_switch_sd_power(int level)
{
    return 0;
}

static int pmic_chipon_check_battery_temp(void)
{
    return 0;
}

static int pmic_chipon_get_battery_temp(void)
{
    return 0;
}

static boot_reason_e pmic_chipon_get_boot_reason(void)
{
    unsigned char data1 = 0, data2 = 0;

    pmic_chipon_read_retry(0x05, &data1);
    pmic_chipon_read_retry(0x06, &data2);

    return (boot_reason_e)(data1 * 256 + data2);
}

static int pmic_chipon_get_acc_state(void)
{
    unsigned char data = 0;

    pmic_chipon_read(0x0d, &data);
    if (data == 0xff) {
        return 0;
    }
    return (data & 0x01) ? 1 : 0;
}

PMIC_OBJECT_s pmic_chipon_object = {
    .name = "chipon",
    .init = pmic_chipon_init,
    .set_lcd_backlight = pmic_chipon_set_lcd_backlight_enable,
    .get_lcd_backlight = pmic_chipon_get_lcd_backlight_enable,
    .check_usb_connected = pmic_chipon_check_usb_connected,
    .set_sram_reg = pmic_chipon_set_sram_reg,
    .get_sram_reg = pmic_chipon_get_sram_reg,
    .set_softreset_flag = pmic_chipon_set_softreset_flag,
    .reset = pmic_chipon_reset,
    .power_off = pmic_chipon_do_system_power_off,
    .set_led = pmic_chipon_set_led,
    .dump_regs = pmic_chipon_dump_regs,
    .read = pmic_chipon_read_retry,
    .write = pmic_chipon_write,
    .modify = pmic_chipon_modify,
    .switch_sd_power = pmic_chipon_switch_sd_power,
    .check_battery_temp = pmic_chipon_check_battery_temp,
    .get_battery_temp = pmic_chipon_get_battery_temp,
    .check_power_key_pressed = pmic_chipon_check_power_key_pressed,
    .get_boot_reason = pmic_chipon_get_boot_reason,
    .get_version = pmic_chipon_get_version,
    .get_acc_state = pmic_chipon_get_acc_state,
};

