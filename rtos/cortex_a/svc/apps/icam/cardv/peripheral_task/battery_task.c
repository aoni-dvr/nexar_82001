#include "battery_task.h"
#include "AmbaADC.h"

//#define NEW_BATTERY_DEFINE

// msg task
static AMBA_KAL_TASK_t battery_task;
#define BATTERY_TASK_STACK_SIZE (32 * 1024)
static unsigned char battery_task_stack[BATTERY_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int debug_enable = 0;
static int running = 0;
static int low_battery_check = 1;
static int battery_percentage  = -1;
static int battery_voltage_mv  = -1;

static void *battery_task_entry(void *arg);

int battery_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&battery_task,
                            "battery_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)battery_task_entry,
                            0,
                            battery_task_stack,
                            BATTERY_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("battery_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int battery_task_stop(void)
{
    running = 0;

    return 0;
}

int battery_task_set_debug(int enable)
{
    debug_enable = enable;

    return 0;
}

int battery_task_set_low_power_check(int enable)
{
    low_battery_check = enable;

    return 0;
}

int battery_task_check_charger_ic_stat(unsigned char *is_charge, unsigned char *power_good, unsigned char *thermal_protect, unsigned char *charge_full)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    unsigned char value = 0;

    i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x08, &value);
    if (is_charge != NULL) {
        if ((value & 0x18) == 0x00 || (value & 0x18) == 0x18) {
            *is_charge = 0;
        } else {
            *is_charge = 1;
        }
    }
    if (charge_full != NULL) {
        if ((value & 0x18) == 0x18) {
           *charge_full = 1;
       } else {
           *charge_full = 0;
       }
    }
    if (power_good != NULL) {
        if ((value & 0x04) == 0x04) {
            *power_good = 1;
        } else {
            *power_good = 0;
        }
    }
    if (thermal_protect != NULL) {
        if ((value & 0x02) == 0x02) {
            *thermal_protect = 1;
        } else {
            *thermal_protect = 0;
        }
    }
#endif
    return 0;
}

int battery_power_on_check(void)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    if (Pmic_CheckUsbConnected() == 0) {
        unsigned int data = 0, i = 0, sum = 0;
        for (i = 0; i < 10; i++) {
            AmbaADC_SingleRead(AMBA_ADC_CHANNEL1, &data);
            sum += data;
            msleep(10);
        }
        debug_line("battery power on(adc=%d)", sum / 10);AmbaPrint_Flush();
        if (sum < 30800) {
            msleep(3000);
            if (Pmic_CheckUsbConnected() == 0) {
                app_helper.set_led_onoff(1);
                app_helper.set_led_brihtness(1);
                app_helper.set_led_fade(0);
                for (i = 0; i < 3; i++) {
                    app_helper.set_led_color(0xe4, 0x00, 0x2e);
                    msleep(100);
                    app_helper.set_led_color(0, 0, 0);
                    msleep(100);
                }
                debug_line("battery too low(adc=%d), power off", sum / 10);AmbaPrint_Flush();
                app_helper.low_battery_power_off = 1;
                if (Pmic_CheckUsbConnected()) {
                    Pmic_NormalSoftReset();
                } else {
                    Pmic_DoPowerOff(0);
                }
            }
        }
    }
#endif
    return 0;
}

static unsigned int cur_adc = 0;
static unsigned int raw_adc = 0;
static unsigned int cur_cable_adc = 0;

void battery_task_get_adc_impl(void)
{
    unsigned int data = 0, i = 0, adc_sum = 0, cable_adc_sum = 0;
    unsigned int max = 0, min = 4096;
    
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    for (i = 0; i < 5; i++) {
        AmbaADC_SingleRead(AMBA_ADC_CHANNEL1, &data);
        adc_sum += data;
        if (data > max) {
            max = data;
        }
        if (data < min) {
            min = data;
        }

        AmbaADC_SingleRead(AMBA_ADC_CHANNEL2, &data);
        cable_adc_sum += data;

        msleep(10);
    }
#else
    for (i = 0; i < 5; i++) {
       AmbaADC_SingleRead(AMBA_ADC_CHANNEL2, &data);
       adc_sum += data;
       if (data > max) {
           max = data;
       }
       if (data < min) {
           min = data;
       }
       msleep(10);
   }
#endif
    cur_adc = (adc_sum - max - min) / 3;
    cur_cable_adc = cable_adc_sum / 5;
}

unsigned int battery_task_get_cable_adc(void)
{
    return cur_cable_adc;
}

unsigned int battery_task_get_mcu_adc(void)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    unsigned char data[2] = {0};
    static unsigned char version = 0xff;

    if (version == 0xff) {
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x00, data);
        version = data[0];
    }

    if (version != 0xff && version >= 0x60) {
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0F, data);
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x10, data + 1);
        return (data[1] << 8) + data[0];
    }
#endif
    return 0;
}

unsigned int battery_task_get_adc(void)
{
    return cur_adc;
}

unsigned int battery_task_get_raw_adc(void)
{
    return raw_adc;
}

int battery_task_get_percentage(void)
{
    return battery_percentage;
}

int battery_task_get_voltage_mv(void)
{
    return battery_voltage_mv;
}

unsigned int max_adc = 0, min_adc = 4096;
int battery_task_reset_param(void)
{
    max_adc = 0;
    min_adc = 4096;

    return 0;
}

static void *battery_task_entry(void *arg)
{
#define ADC_SAMPLE_NUM (5)
    unsigned int low_battery_count = 0, i = 0, cnt = 0;
    int is_charge = 0;
    unsigned int adc_sample_array[ADC_SAMPLE_NUM] = {0};
    unsigned int adc_sample_num = 0;
    unsigned int adc_sample_sum = 0;
    unsigned int cur_battery_voltage_mv = 0;
    unsigned char charge = 0, power_good = 0, thermal_protect = 0, full = 0;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    //init charger ETA6953
    if (dqa_test_script.usb_uvc_mode
        || app_helper.usb_mass_storage_mode) {
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x01, 0x10, 0x00);
    } else {
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x01, 0x10, 0x10);
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x02, 0x3F, 0x06);
        i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x03, 0x11);
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x04, 0xF8, 0x40);
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x05, 0x30, 0x00);
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x06, 0x0F, 0x00);
        i2c_modify_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x07, 0x04, 0x00);
    }
#endif
    while (running) {
        //battery_value_s *battery_values = NULL;
        //int len = 0;

        if (app_helper.low_battery) {
            if (app_helper.is_doing_poweroff == 0) {
                app_helper.set_led_fade(0);
                if (cnt == 0) {
                    cnt = 1;
                    app_helper.set_led_color(0xe4, 0x00, 0x2e);
                } else {
                    cnt = 0;
                    app_helper.set_led_color(0, 0, 0);
                }
            }
            msleep(100);
            continue;
        }
        battery_task_get_adc_impl();
        raw_adc = cur_adc;

        if (Pmic_CheckUsbConnected()) {
            if (is_charge == 0) {
                for (i = 0; i < ADC_SAMPLE_NUM; i++) {
                    adc_sample_array[i] = 0;
                }
                adc_sample_num = 0;
                max_adc = 0;
                sleep(1);
            }
            is_charge = 1;
            //battery_values = battery_values_charge;
            //len = sizeof(battery_values_charge) / sizeof(battery_value_s) - 1;
        } else {
            if (is_charge) {
                for (i = 0; i < ADC_SAMPLE_NUM; i++) {
                    adc_sample_array[i] = 0;
                }
                adc_sample_num = 0;
                min_adc = 4096;
                sleep(1);
            }
            is_charge = 0;
            //battery_values = battery_values_discharge;
            //len = sizeof(battery_values_discharge) / sizeof(battery_value_s) - 1;
        }

        if (adc_sample_num < ADC_SAMPLE_NUM) {
            adc_sample_array[adc_sample_num] = raw_adc;
            adc_sample_num += 1;
        } else {
            for (i = 0; i < (ADC_SAMPLE_NUM - 1) ; i++) {
                adc_sample_array[i] = adc_sample_array[i + 1];
            }
            adc_sample_array[ADC_SAMPLE_NUM - 1] = cur_adc;
            adc_sample_num = ADC_SAMPLE_NUM;
        }
        adc_sample_sum = 0;
        for (i = 0; i < adc_sample_num; i++) {
            adc_sample_sum += adc_sample_array[i];
        }
        cur_adc = adc_sample_sum / adc_sample_num;
        if (is_charge) {
            if (cur_adc > max_adc) {
                max_adc = cur_adc;
            }
            if (cur_adc < max_adc) {
                cur_adc = max_adc;
            }
#if !defined(CONFIG_BSP_H32_NEXAR_D081)     
            cur_battery_voltage_mv = 3.3 * cur_adc * 1.0 / 4095 * 1000 * 3 / 2;
#else
            cur_battery_voltage_mv = 3.3 * cur_adc * 1.0 / 4095 * 1000 * 4.7 / 2 * 1036 / 1000;
#endif
            battery_task_check_charger_ic_stat(&charge, &power_good, &thermal_protect, &full);
        } else {
            if (cur_adc < min_adc) {
                min_adc = cur_adc;
            }
            if (cur_adc > min_adc) {
                cur_adc = min_adc;
            }
#if !defined(CONFIG_BSP_H32_NEXAR_D081)            
            cur_battery_voltage_mv = cur_adc * 1.244;
#else
            cur_battery_voltage_mv = cur_adc * 2.050;
#endif
            charge = 0;
            power_good = 0;
            thermal_protect = 0;
            full = 0;
        }
        battery_voltage_mv = cur_battery_voltage_mv;
        
#if !defined(CONFIG_BSP_H32_NEXAR_D081) 
        if (is_charge) {
            if (full) {
                battery_percentage = 100;
            } else {
                if (battery_voltage_mv <= 3880) {
                    battery_percentage = 0;
                } else if (battery_voltage_mv <= 4020) {
                    battery_percentage = (battery_voltage_mv - 3880) * 7 / 10;
                } else {
                    battery_percentage = 98;
                }
            }
        } else {
            if (battery_voltage_mv <= 3650) {
                battery_percentage = 0;
            } else if (battery_voltage_mv <= 4020) {
                battery_percentage = (battery_voltage_mv - 3650) * 10 / 37;
            } else {
                battery_percentage = 100;
            }
        }
#else
        if (is_charge) {
            if (full) {
                battery_percentage = 100;
            } else {
                if (battery_voltage_mv <= 3700) {
                    battery_percentage = 0;
                } else if (battery_voltage_mv <= 4100) {
                    battery_percentage = (battery_voltage_mv - 3700) * 24 / 100;
                } else {
                    battery_percentage = 98;
                }
            }
        } else {
            if (battery_voltage_mv <= 3650) {
                battery_percentage = 0;
            } else if (battery_voltage_mv <= 4095) {
                battery_percentage = (battery_voltage_mv - 3650) * 10 / 45;
            } else {
                battery_percentage = 100;
            }
        }
#endif
        if (debug_enable) {
            debug_line("battery adc: %d, raw_adc: %d, mcu_adc:%d cable_adc: %d percentage: %d, voltage: %dmv, charge: %d, power_good: %d, thermal_protect: %d, full: %d",
                        cur_adc, raw_adc, battery_task_get_mcu_adc(), battery_task_get_cable_adc(),
                        battery_percentage, battery_voltage_mv,
                        charge, power_good, thermal_protect, full);
        }
        if (low_battery_check) {
            if (is_charge == 0) {
                if (battery_voltage_mv <= 3650) {
                    low_battery_count++;
                } else if (battery_voltage_mv > 3660) {
                    low_battery_count = 0;
                }
            } else {
                low_battery_count = 0;
            }
            if (low_battery_count >= 10) {
                app_helper.low_battery = 1;
                app_msg_queue_send(APP_MSG_ID_LOW_BATTERY, 0, 0, 0);
            }
        }
        msleep(100);
    }

    return NULL;
}

