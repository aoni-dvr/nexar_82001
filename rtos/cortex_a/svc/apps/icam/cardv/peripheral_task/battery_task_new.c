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
    unsigned char value = 0;

    i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x0A, &value);
    if (is_charge != NULL) {
        if ((value & 0x30) == 0x10 || (value & 0x30) == 0x20) {
            *is_charge = 1;
        } else {
            *is_charge = 0;
        }
    }    
    if (charge_full != NULL) {
        if ((value & 0x30) == 0x30) {
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

    return 0;
}

int battery_power_on_check(void)
{
#if 0
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

void battery_task_get_adc_impl(void)
{
}

unsigned int battery_task_get_cable_adc(void)
{
    unsigned int data = 0, i = 0, sum = 0;

    for (i = 0; i < 5; i++) {
        AmbaADC_SingleRead(AMBA_ADC_CHANNEL2, &data);
        sum += data;
        msleep(10);
    }

    return sum / 5;
}

unsigned int battery_task_get_mcu_adc(void)
{
    return 4096;
}

unsigned int battery_task_get_adc(void)
{
    return 4096;
}

unsigned int battery_task_get_raw_adc(void)
{
    return 4096;
}

int battery_task_get_percentage(void)
{
    return battery_percentage;
}

int battery_task_get_voltage_mv(void)
{
    return 4200;
}

unsigned int max_adc = 0, min_adc = 4096;
int battery_task_reset_param(void)
{
    max_adc = 0;
    min_adc = 4096;

    return 0;
}

int max17055_i2c_write(unsigned char reg, unsigned short value)
{
    unsigned char tx_buf[3] = {0};

    tx_buf[0] = (unsigned char)reg;
    tx_buf[1] = (unsigned char)(value & 0xff);
    tx_buf[2] = (unsigned char)(value >> 8);
    i2c_write(BATTERY_METER_I2C_CHANNEL, BATTERY_METER_I2C_SLAVE_ADDR, tx_buf, 3);

    return 0;
}

unsigned short max17055_i2c_read(unsigned char reg)
{
    unsigned char rx_data[2] = {0};

    i2c_read(BATTERY_METER_I2C_CHANNEL, BATTERY_METER_I2C_SLAVE_ADDR, reg, rx_data, 2);

    return (unsigned short)(rx_data[1] << 8) | rx_data[0];
}

int max17055_init(void)
{
    unsigned short u16DesignCap = 0x1450;
    unsigned short u16dQAcc = 0;
    //====IChgTerm Register (1Eh):  Register Type: Current  //Initial Value: 0x0640 (250mA on 10mOhm)
    unsigned short u16Ichgterm = 0x333;
    //====VEmpty Register (3Ah): VEmpty  //Initial Value: 0xA561 (VE3.3V/VR3.88V)  //0xB461 (VE3.6V/VR3.88V)
    unsigned short u16VEmpty = 0xAF61;  //0xA561; //VEmpty Register (3Ah): 0xA561 (VE3.3V/VR3.88V),  0xB461 (VE3.6V/VR3.88V),  0xAF61 (VE3.5V/VR3.88V)
    //====HibCfg Register (BAh):  Set HibCfg to 0x870C for most applications.  //Initial Value: 0x870C (0x890C)
    unsigned short u16HibCFG = 0x890C;  //default: 0x890C
    float u16ChargeVoltage = 4.1;  // 4.275;

    unsigned short u16RegData;
    unsigned char i, j;

    debug_line("Max17055 Init start");

    //After power-up, wait for the MAX17055 to complete its startup operations
    i = 0;
    while (i++ < 100) {
        u16RegData = max17055_i2c_read(0x3D) & 0x0001;
        //do not continue until FSTAT.DNR == 0
        if (u16RegData == 0) {
            debug_line("Max17055 Init: i = %d, FSTAT.DNR == 0, MAX17055 complete its startup", i);
            break;
        }
        msleep(10);  // 10 ms
    }
    //Check for POR
    //====Status Register (00h):  Initial Value: 0x0002
    u16RegData = max17055_i2c_read(0x00) & 0x0002;
    debug_line("Max17055 Init #1: StatusPOR = 0x%x", u16RegData);
    if (1) {//(u16RegData) {
        for (j = 0; j < 3; j++) {
            // Initialize configuration 
            //====HibCfg Register (BAh):  Set HibCfg to 0x870C for most applications.  //Initial Value: 0x870C (0x890C)
            u16HibCFG = max17055_i2c_read(0xBA); //Store original HibCFG value
            debug_line("Max17055 Init #2: j = %d, u16HibCFG = 0x%x", j, u16HibCFG);

            //====Soft-Wakeup (Command Register) (60h):  
            //To wake and exit hibernate, do the following sequence:
            // > 1. Write HibCfg = 0x0000.
            // > 2. Soft-Wakeup Command. Write Command register (60h) to 0x0090.
            // > 3. Clear Command. Write Command register (60h) to 0x0000.
            // > 4. Eventually restore HibCfg to again allow automatic hibernate decisions.
            max17055_i2c_write(0x60 , 0x90);  // Exit Hibernate Mode step 1
            max17055_i2c_write(0xBA , 0x0);  // Exit Hibernate Mode step 2
            max17055_i2c_write(0x60 , 0x0);  // Exit Hibernate Mode step 3
            // OPTION EZ Config
            //====DesignCap Register(18h):  The DesignCap register holds the expected capacity of the cell. 
            max17055_i2c_write(0x18 , u16DesignCap);  //u16DesignCap = 0x1450
            u16dQAcc = u16DesignCap/32;
            max17055_i2c_write(0x45 , u16DesignCap/32);  //Write dQAcc
            //====IChgTerm Register (1Eh):  Register Type: Current  //Initial Value: 0x0640 (250mA on 10mOhm)
            max17055_i2c_write(0x1E , u16Ichgterm);
            //====VEmpty Register (3Ah): VEmpty  //Initial Value: 0xA561 (VE3.3V/VR3.88V)  //0xB461 (VE3.6V/VR3.88V)
            max17055_i2c_write(0x3A , u16VEmpty);  //0xAF61 (VE3.5V/VR3.88V)
            //Only use integer portion of dQAcc=int(u16DesignCap/32) in the calculation of dPAcc to avoid quantization of FullCapNom
            if (u16ChargeVoltage > 4.275) {
                max17055_i2c_write(0x46,u16dQAcc*51200/u16DesignCap); // Write dPAcc  //51200 / 32 / 16 = 100 (100%)
                max17055_i2c_write(0xDB, 0x8400); // Write ModelCFG
            } else {
                //====dPAcc Register (46h):  Percentage (1/16% per LSB)  //Initial Value: 0x0190 (25%)
                //max17055_i2c_write(0x46 , u16dQAcc*44138/u16DesignCap); //Write dPAcc  //44138 / 32 / 16 = 86.2 (86.2%)
                max17055_i2c_write(0x46,u16dQAcc*51200/u16DesignCap); // Write dPAcc  //51200 / 32 / 16 = 100 (100%)
                //====ModelCfg Register (DBh): The ModelCFG register controls basic options of the EZ algorithm
                max17055_i2c_write(0xDB, 0x8000);  // Write ModelCFG
            }
            //Poll ModelCFG.Refresh(highest bit), wait ModelCFG.Refresh = 0
            i = 0;
            while (i++ < 150) {
                u16RegData = max17055_i2c_read(0xDB) & 0x8000;
                //Do not continue until ModelCFG.Refresh == 0
                if (u16RegData == 0) {
                    debug_line("Max17055 Init: i = %d, ModelCFG.Refresh = 0", i);
                    break;
                }
                msleep(10);  // 10 ms
            }
            debug_line("Max17055 Init #3: j = %d, u16HibCFG = 0x%x", j, u16HibCFG);
            max17055_i2c_write(0xBA, u16HibCFG); // Restore Original HibCFG value

            //Initialization complete
            //Clear the POR bit to indicate that the custom model and parameters were successfully loaded
            u16RegData = max17055_i2c_read(0x00);  //Read Status
            debug_line("Max17055 Init #4: Register[0x00] = 0x%x, WrittenData = 0x%x", u16RegData, u16RegData & 0xFFFD);
            // Write and Verify Status with POR bit cleared
            max17055_i2c_write(0x00, u16RegData & 0xFFFD);
            i = 0;
            while (i++ < 100) {
                u16RegData = max17055_i2c_read(0x00) & 0x0002;
                if (u16RegData == 0) {
                    debug_line("Max17055 Init: i = %d, j = %d, StatusPOR = 0x%x, the custom model and parameters are successfully loaded", i, j, u16RegData);
                    j = 10;
                    break;
                }
                msleep(10);  // 10 ms
            }
            debug_line("Max17055 Init #5: Initialization complete: i = %d, j = %d, StatusPOR = 0x%x", i, j, u16RegData);
        }
    }
    debug_line("Max17055 Init finish");

    return 0;
}

static void *battery_task_entry(void *arg)
{
#define ADC_SAMPLE_NUM (5)
    unsigned int low_battery_count = 0, cnt = 0, value = 0;
    int is_charge = 0;
    unsigned char charge = 0, power_good = 0, thermal_protect = 0, full = 0;
    unsigned char reg_value = 0;
    i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, &reg_value);
    reg_value |= 0x08;
    reg_value &= ~0x60;
    i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, reg_value);
    i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x01, 0x23);
    i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x02, 0xc1);
    max17055_init();

    while (running) {
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
        if (Pmic_CheckUsbConnected()) {
            is_charge = 1;
        } else {
            is_charge = 0;
        }
        if (is_charge) {
            battery_task_check_charger_ic_stat(&charge, &power_good, &thermal_protect, &full);
        } else {
            charge = 0;
            power_good = 0;
            thermal_protect = 0;
            full = 0;
        }
        value = max17055_i2c_read(0x06);
        battery_percentage = value >> 8;
        if (battery_percentage > 100) {
            battery_percentage = 100;
        }
        if (debug_enable) {
            debug_line("cable_adc: %d percentage: %d(%d.%d), charge: %d, power_good: %d, thermal_protect: %d, full: %d",
                        battery_task_get_cable_adc(),
                        battery_percentage, value >> 8, value & 0xff,
                        charge, power_good, thermal_protect, full);
        }
        if (low_battery_check) {
            if (is_charge == 0) {
                if (battery_percentage < 10) {
                    low_battery_count++;
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

