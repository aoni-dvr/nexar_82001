#include "pmic_amba_pwc.h"
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include <pmic.h>

static int pmic_amba_pwc_dump_regs(void)
{
    return 0;
}

static int pmic_amba_pwc_get_lcd_backlight_enable(void)
{
    return 0;
}

static int pmic_amba_pwc_set_lcd_backlight_enable(int enable)
{
    (void)enable;
    return 0;
}

static int pmic_amba_pwc_do_system_power_off(void)
{
    extern UINT32 AmbaSYS_TrigPowerDownSequence(UINT32 Option);
    AmbaSYS_TrigPowerDownSequence(0);

    return 0;
}

static int pmic_amba_pwc_get_sram_reg(void)
{
    return -1;
}

static int pmic_amba_pwc_set_sram_reg(int val)
{
    (void)val;
    return -1;
}

static int pmic_amba_pwc_set_softreset_flag(void)
{
    return 0;
}

static int pmic_amba_pwc_reset(void)
{
    extern void AmbaRTSL_PwcReboot(void);
    AmbaRTSL_PwcReboot();

    return 0;
}

static int pmic_amba_pwc_check_usb_connected(void)
{
    extern UINT32 AmbaRTSL_UsbVbusVicRawStaus(void);
    return AmbaRTSL_UsbVbusVicRawStaus();
}

static int pmic_amba_pwc_set_led(int on)
{
    (void)on;
    return 0;
}

static int pmic_amba_pwc_get_version(char *version)
{
    (void)version;
    return -1;
}

static int pmic_amba_pwc_init(void)
{
    return 0;
}

static int pmic_amba_pwc_check_power_key_pressed(void)
{
    return 0;
}

static int pmic_amba_pwc_switch_sd_power(int level)
{
    (void)level;
    return 0;
}

static int pmic_amba_pwc_check_battery_temp(void)
{
    return 0;
}

static int pmic_amba_pwc_get_battery_temp(void)
{
    return 0;
}

static boot_reason_e pmic_amba_pwc_get_boot_reason(void)
{
    return BOOT_FROM_NORMAL_POWER_ON;
}

PMIC_OBJECT_s pmic_amba_pwc_object = {
    .name = "amba_pwc",
    .init = pmic_amba_pwc_init,
    .set_lcd_backlight = pmic_amba_pwc_set_lcd_backlight_enable,
    .get_lcd_backlight = pmic_amba_pwc_get_lcd_backlight_enable,
    .check_usb_connected = pmic_amba_pwc_check_usb_connected,
    .set_sram_reg = pmic_amba_pwc_set_sram_reg,
    .get_sram_reg = pmic_amba_pwc_get_sram_reg,
    .set_softreset_flag = pmic_amba_pwc_set_softreset_flag,
    .reset = pmic_amba_pwc_reset,
    .power_off = pmic_amba_pwc_do_system_power_off,
    .set_led = pmic_amba_pwc_set_led,
    .dump_regs = pmic_amba_pwc_dump_regs,
    .read = NULL,
    .write = NULL,
    .modify = NULL,
    .switch_sd_power = pmic_amba_pwc_switch_sd_power,
    .check_battery_temp = pmic_amba_pwc_check_battery_temp,
    .get_battery_temp = pmic_amba_pwc_get_battery_temp,
    .check_power_key_pressed = pmic_amba_pwc_check_power_key_pressed,
    .get_boot_reason = pmic_amba_pwc_get_boot_reason,
    .get_version = pmic_amba_pwc_get_version,
};


