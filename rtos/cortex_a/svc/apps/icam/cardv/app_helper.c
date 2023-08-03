#include <app_base.h>
#include "AmbaPWM.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "app_helper.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "AmbaIPC_RpcProg_LU_LinuxTimeService.h"
#endif
#include "rec/rec_dvr.h"
#include "system/app_util.h"

static int led_lock_impl(int lock);
static int gpio_set_impl(int gpio, int level);
static int gpio_get_impl(int gpio);
static int set_led_onoff_impl(int on);
static int set_led_color_impl(unsigned char r, unsigned char g, unsigned char b);
static int set_led_brihtness_impl(unsigned char brightness);
static int set_led_enable_impl(unsigned char enable);
static int do_power_off_impl(void);
static int ir_led_control_impl(unsigned char brightness);
static int linux_system_impl(const char *cmd);
static int linux_system_with_response_impl(const char *cmd, char *response);
static int check_file_exist_impl(const char *path, unsigned long long *filesize);
static int check_sd_update_impl(void);
#if defined(CONFIG_ENABLE_EMMC_BOOT)
static int check_emmc_update_impl(void);
#endif
static int check_sd_exist_impl(void);
static int get_sd_space_impl(unsigned int *total_m, unsigned int *free_m);
static int touch_impl(const char *path);
static device_info_s *get_device_info_impl(void);
static void update_time_for_linux_impl(void);
static int check_external_camera_connected_impl(void);
static int check_enable_linux_uart_impl(char disk);
static int check_lte_is_ready_impl(void);
static int check_bt_is_ready_impl(void);
static int check_interface_is_ready_impl(const char *name);
static int get_interface_ip_impl(const char *name, char *ip);
static int set_digital_mode_impl(int view_id, int mode);
static int read_all_text_impl(const char *path, unsigned char *buf, unsigned int buf_size);
static int write_all_text_impl(const char *path, const unsigned char *buf, unsigned int buf_len);
static int set_watchdog_impl(int seconds);
static int check_camera_is_working_impl(CAMERA_CHANNEL_e channel);
static int set_ae_table_impl(CAMERA_CHANNEL_e channel, unsigned char ae_table[8][12]);
static int reset_ae_table_impl(CAMERA_CHANNEL_e channel);
static int convert_linux_path_to_rtos_impl(const char *linux_path, char *rtos_path);
static int convert_rtos_path_to_linux_impl(const char *rtos_path, char *linux_path);
static int set_file_readonly_impl(const char *path, int readonly);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
static int check_gc2053_left_connected_impl(void);
static int check_gc2053_right_connected_impl(void);
static int check_lte_left_connected_impl(void);
static int check_lte_right_connected_impl(void);
static int check_usb_left_connected_impl(void);
static int check_usb_right_connected_impl(void);
static int get_left_addon_type_impl(void);
static int get_right_addon_type_impl(void);
static const char *addon_type_2_str_impl(int type);
#endif
static int get_linux_time_impl(linux_time_s *linux_time);
static const char *wakeup_source_2_str_impl(int reason);
static int ir_led_lock_impl(int flag);
static int set_led_fade_in_out_impl(int enable);
static int load_dev_info_impl(void);
static int ir_check_onoff_impl(void);
static POWER_SOURCE_e get_power_source_impl(void);
static const char *power_source_2_str_impl(int reason);
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
static int set_logo_led_color_impl(int id, unsigned char r, unsigned char g, unsigned char b);
static int set_logo_led_fade_in_out_impl(int enable);
static int check_d080_with_2nd_sensor_impl(void);
#endif

app_helper_s app_helper;

int sd_bad_or_not_exist = 0;

static device_info_s device_info;
static AMBA_KAL_MUTEX_t led_mutex;
int app_helper_init(void)
{
    app_helper.linux_console_enable = -1;
    app_helper.internal_camera_auto_record = 0;
    app_helper.external_camera_auto_record = 0;
    app_helper.internal_camera_enabled = 1;
    app_helper.external_camera_enabled = 0;
    app_helper.mcu_update_flag = 0;
    app_helper.usb_mass_storage_on = 0;
    app_helper.usb_mass_storage_mode = 0;
    app_helper.power_on_with_sd_card = 0;
    app_helper.hard_reset_flag = 0;
    app_helper.factory_reset_flag = 0;
    app_helper.linux_booted = 0;
    app_helper.wifi_booted = 0;
    app_helper.bt_booted = 0;
    app_helper.usb_wifi_booted = 0;
    app_helper.lte_on = 0;
    app_helper.lte_booted = 0;
    app_helper.format_busy = 0;
    app_helper.force_power_off = 0;
    app_helper.lock_led = led_lock_impl;
    app_helper.lock_ir_led = ir_led_lock_impl;
    app_helper.gpio_set = gpio_set_impl;
    app_helper.gpio_get = gpio_get_impl;
    app_helper.set_led_onoff = set_led_onoff_impl;
    app_helper.set_led_color = set_led_color_impl;
    app_helper.set_led_brihtness = set_led_brihtness_impl;
    app_helper.set_led_fade = set_led_fade_in_out_impl;
    app_helper.set_led_enable = set_led_enable_impl;
    app_helper.do_power_off = do_power_off_impl;
    app_helper.set_ir_brightness = ir_led_control_impl;
    app_helper.linux_system = linux_system_impl;
    app_helper.linux_system_with_response = linux_system_with_response_impl;
    app_helper.check_file_exist = check_file_exist_impl;
    app_helper.check_sd_update = check_sd_update_impl;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    app_helper.check_emmc_update = check_emmc_update_impl;
#endif
    app_helper.touch = touch_impl;
    app_helper.check_sd_exist = check_sd_exist_impl;
    app_helper.get_sd_space = get_sd_space_impl;
    app_helper.get_device_info = get_device_info_impl;
    app_helper.time_update_for_linux = update_time_for_linux_impl;
    app_helper.check_external_camera_connected = check_external_camera_connected_impl;
    app_helper.check_enable_linux_uart = check_enable_linux_uart_impl;
    app_helper.check_lte_is_ready = check_lte_is_ready_impl;
    app_helper.check_interface_is_ready = check_interface_is_ready_impl;
    app_helper.get_interface_ip = get_interface_ip_impl;
    app_helper.check_bt_is_ready = check_bt_is_ready_impl;
    app_helper.set_digital_mode = set_digital_mode_impl;
    app_helper.read_all_text = read_all_text_impl;
    app_helper.write_all_text = write_all_text_impl;
    app_helper.set_watchdog = set_watchdog_impl;
    app_helper.check_camera_is_working = check_camera_is_working_impl;
    app_helper.set_ae_table = set_ae_table_impl;
    app_helper.reset_ae_table = reset_ae_table_impl;
    app_helper.convert_path_linux2rtos = convert_linux_path_to_rtos_impl;
    app_helper.convert_path_rtos2linux = convert_rtos_path_to_linux_impl;
    app_helper.set_file_readonly = set_file_readonly_impl;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    app_helper.check_gc2053_left_connected = check_gc2053_left_connected_impl;
    app_helper.check_gc2053_right_connected = check_gc2053_right_connected_impl;
    app_helper.check_lte_left_connected = check_lte_left_connected_impl;
    app_helper.check_lte_right_connected = check_lte_right_connected_impl;
    app_helper.check_usb_left_connected = check_usb_left_connected_impl;
    app_helper.check_usb_right_connected = check_usb_right_connected_impl;
    app_helper.get_left_addon_type = get_left_addon_type_impl;
    app_helper.get_right_addon_type = get_right_addon_type_impl;
    app_helper.addon_type_2_str = addon_type_2_str_impl;
#endif
    app_helper.get_linux_time = get_linux_time_impl;
    app_helper.wakeup_source_2_str = wakeup_source_2_str_impl;
    app_helper.load_dev_info = load_dev_info_impl;
    app_helper.ir_check_onoff = ir_check_onoff_impl;
    app_helper.get_power_source = get_power_source_impl;
    app_helper.power_source_2_str = power_source_2_str_impl;
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    app_helper.set_logo_led_color = set_logo_led_color_impl;
    app_helper.set_logo_led_fade = set_logo_led_fade_in_out_impl;
    app_helper.check_d080_with_2nd_sensor = check_d080_with_2nd_sensor_impl;
#endif
    app_helper.led_control = 1;
    app_helper.sd_init_done = 0;
    app_helper.low_battery = 0;
    app_helper.acc_connected = 0;
    app_helper.with_acc = 0;
    app_helper.lte_wakeup_test = 0;
    app_helper.timer_wakeup_test = 0;
    app_helper.lte_booted_tick = 0;
    app_helper.low_battery_power_off = 0;
    app_helper.is_doing_poweroff = 0;
    app_helper.sd_card_idle = 0;
    app_helper.auto_power_off = 1;

    memset(&device_info, 0, sizeof(device_info_s));
    memcpy(device_info.sw_version, PROJECT_INFO_SW_VERSION, strlen(PROJECT_INFO_SW_VERSION));
    memcpy(device_info.build_time, PROJECT_INFO_BUILD_TIME, strlen(PROJECT_INFO_BUILD_TIME));
    memcpy(device_info.build_timestamp, PROJECT_INFO_BUILD_TIMESTAMP, strlen(PROJECT_INFO_BUILD_TIMESTAMP));
    if (AmbaKAL_MutexCreate(&led_mutex, "led_mutex") != KAL_ERR_NONE) {
        debug_line("%s: led_mutex create fail", __func__);
    }

    return 0;
}

static int load_dev_info_impl(void)
{
    FILE *pFile = NULL;
    char line[512] = {0};

    pFile = fopen(USER_SETTING_DEVINFO_PATH, "r");
    if (pFile == NULL) {
        return -1;
    }
    memset(line, 0, sizeof(line));
    while (readline(pFile, line) >= 0) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }
        if (strncmp(line, "vsn=", strlen("vsn=")) == 0) {
            memcpy(device_info.vsn, line + strlen("vsn="), strlen(line) - strlen("vsn="));
        } else if (strncmp(line, "startup_index=", strlen("startup_index=")) == 0) {
            device_info.startup_index = atoi(line + strlen("startup_index="));
        } else if (strncmp(line, "ibeacon_major=", strlen("ibeacon_major=")) == 0) {
            device_info.ibeacon_major = atoi(line + strlen("ibeacon_major="));
        } else if (strncmp(line, "ibeacon_minor=", strlen("ibeacon_minor=")) == 0) {
            device_info.ibeacon_minor = atoi(line + strlen("ibeacon_minor="));
        } else if (strncmp(line, "trip_key=", strlen("trip_key=")) == 0) {
            memcpy(device_info.trip_key, line + strlen("trip_key="), strlen(line) - strlen("trip_key="));
        }
        memset(line, 0, sizeof(line));
    }
    fclose(pFile);

    return 0;
}

static int led_lock_flag = 0;
static int led_lock_impl(int flag)
{
    led_lock_flag = flag;

    return 0;
}

static int gpio_set_impl(int gpio, int level)
{
    if (level) {
        AmbaGPIO_SetFuncGPO(gpio, AMBA_GPIO_LEVEL_HIGH);
    } else {
        AmbaGPIO_SetFuncGPO(gpio, AMBA_GPIO_LEVEL_LOW);
    }

    return 0;
}

static int gpio_get_impl(int gpio)
{
    AMBA_GPIO_INFO_s info;

    AmbaGPIO_SetFuncGPI(gpio);
    if (AmbaGPIO_GetInfo(gpio, &info) != GPIO_ERR_NONE) {
        return -1;
    }
    return (info.PinState == AMBA_GPIO_LEVEL_HIGH) ? 1 : 0;
}

enum {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_IR,
};

//White - #FFFFFF
//Blue - #4766FF
//Purple - #A387F8
//Orange - #FFA514
//Red - #E4002E
//Green - #00B063
static int _set_led_color(int led, unsigned char color)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    if (led == LED_RED) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x34, color);
    } else if (led == LED_GREEN) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x35, color);
    } else if (led == LED_BLUE) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x36, color);
    } else {
        return -1;
    }
#endif

    return 0;
}
static unsigned char last_r = 0, last_g = 0, last_b = 0;
static unsigned char last_brightness = 0;
static unsigned char last_fade_enable = 0;
static unsigned char led_reset = 0;

static int set_led_enable_impl(unsigned char enable)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    if (enable == 0) {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x01, 0x00);
    } else {
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x01, 0x01);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x30, 0x07);
        led_reset = 1;
        app_helper.set_led_brihtness(last_brightness);
        app_helper.set_led_color(last_r, last_g, last_b);
        app_helper.set_led_fade(last_fade_enable);
        led_reset = 0;
    }
#endif
    return 0;
}

static int set_led_color_impl(unsigned char r, unsigned char g, unsigned char b)
{
    static int first_time = 1;

    if (led_reset == 0 && led_lock_flag) {
        return -1;
    }
    if (led_reset == 0 && first_time == 0) {
        if (last_r == r && last_g == g && last_b == b) {
            return 0;
        }
    }
    first_time = 0;
    last_r = r;last_g = g;last_b = b;
    AmbaKAL_MutexTake(&led_mutex, AMBA_KAL_WAIT_FOREVER);
    _set_led_color(LED_RED, r);
    _set_led_color(LED_GREEN, g);
    _set_led_color(LED_BLUE, b);
    AmbaKAL_MutexGive(&led_mutex);

    return 0;
}

static int set_led_brihtness_impl(unsigned char brightness)
{
    static int first_time = 1;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    unsigned char value[3] = {0};
    int i = 0;
#endif

    if (brightness >= 4) {
        brightness = 4;
    }

    if (led_reset == 0 && led_lock_flag) {
        return -1;
    }
    if (led_reset == 0 && first_time == 0) {
        if (last_brightness == brightness) {
            return 0;
        }
    }
    first_time = 0;
    last_brightness = brightness;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    for (i = 0; i < 3; i++) {
        i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31 + i, &value[i]);
    }
    for (i = 0; i < 3; i++) {
        value[i] &= ~0x07;
        value[i] |= (brightness & 0x07);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31 + i, value[i]);
    }
#endif

    return 0;
}

static int set_led_fade_in_out_impl(int enable)
{
    static int first_time = 1;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    int i = 0;
    unsigned char value[3] = {0};
#endif

    if (led_reset == 0 && first_time == 0) {
        if (last_fade_enable == enable) {
            return 0;
        }
    }
    first_time = 0;
    last_fade_enable = enable;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x4a, 0x04);
    for (i = 0; i < 3; i++) {
        i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31 + i, &value[i]);
    }
    for (i = 0; i < 3; i++) {
        value[i] &= ~0x10;
        value[i] |= (enable ? 0x10 : 0x00);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x31 + i, value[i]);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x37 + i * 3, 0x30);
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x37 + i * 3 + 1, 0x30);
    }
#endif
    return 0;
}

static int set_led_onoff_impl(int on)
{
    if (on) {
        led_lock_flag = 0;
    } else {
        led_lock_flag = 1;
        AmbaKAL_MutexTake(&led_mutex, AMBA_KAL_WAIT_FOREVER);
        _set_led_color(LED_RED, 0);
        _set_led_color(LED_GREEN, 0);
        _set_led_color(LED_BLUE, 0);
        AmbaKAL_MutexGive(&led_mutex);
    }

    return 0;
}

#if defined(CONFIG_BSP_CV25_NEXAR_D080)
static int set_logo_led_color_impl(int id, unsigned char r, unsigned char g, unsigned char b)
{
    //init 3 led
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x00, 0x01);
    if (id == 0) {
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x09, r);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x07, g);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x0b, b);
    } else if (id == 1) {
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x15, r);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x13, g);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x17, b);
    } else if (id == 2) {
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x0f, r);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x0d, g);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x11, b);
    } else if (id == 3) {
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x1b, r);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x19, g);
        i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x1d, b);
    }
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x49, 0x00);
    //set current
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4D, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4E, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4F, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x50, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x51, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x52, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x53, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x54, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x55, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x56, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x57, 0x20);
    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x58, 0x20);

    i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x6E, 0x20);//brightness

    return 0;
}

static int set_logo_led_fade_in_out_impl(int enable)
{
    return 0;
}
#endif

static int ir_check_onoff_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    unsigned char value = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
    unsigned int i2c_channel = get_ir_i2c_channel();
#else
    unsigned int i2c_channel = IR_I2C_CHANNEL;
#endif
#ifdef CONFIG_PCBA_DVT
    i2c_read_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x07, &value);
#else
    i2c_read_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x03, &value);
#endif
    return value ? 1 : 0;
#else
    return app_helper.gpio_get(GPIO_PIN_69) ? 1 : 0;
#endif
}

static int ir_led_lock_flag = 0;
static int ir_led_lock_impl(int flag)
{
    ir_led_lock_flag = flag;

    return 0;
}

static int ir_led_control_impl(unsigned char brightness)
{
    static int last_state = -1;
    if (ir_led_lock_flag) {
        return -1;
    }
    if (last_state == -1 || last_state != brightness) {
        last_state = brightness;
        app_helper.ir_last_changed_time = tick();
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
#ifdef CONFIG_PCBA_DVT
        {
        int ret = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
        unsigned int i2c_channel = get_ir_i2c_channel();
#else
        unsigned int i2c_channel = IR_I2C_CHANNEL;
#endif
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x00, 0x01);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x07, brightness);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x09, brightness);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x0B, brightness);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x0D, brightness);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x0F, 0x00);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x11, 0x00);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x49, 0x00);
        if (ret < 0) { return -1; }
        //set current
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x4D, 0xff);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x4E, 0xff);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x4F, 0xff);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x50, 0xff);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x51, 0x00);
        if (ret < 0) { return -1; }
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x52, 0x00);
        if (ret < 0) { return -1; }
        //set global current
        ret = i2c_write_reg(i2c_channel, IR_I2C_SLAVE_ADDR, 0x6e, 0xff);
        if (ret < 0) { return -1; }
        }
#else
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x00, 0x40);
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x02, 0x07);
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x03, brightness ? 0xFF : 0x00);
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x04, 0xFF);
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x05, 0xFF);
        i2c_write_reg(IR_I2C_CHANNEL, IR_I2C_SLAVE_ADDR, 0x06, 0xFF);
#endif
#else
        app_helper.gpio_set(GPIO_PIN_69, brightness ? 1 : 0);
#endif
    }

    return 0;
}

static int do_power_off_impl(void)
{
    int timeout_times = 0;
    SVC_USER_PREF_s *pSvcUserPref;
    int beep_power_off = 0;

    debug_line("do_power_off, low_battery_power_off=%d,force_power_off=%d", app_helper.low_battery_power_off, app_helper.force_power_off);
    app_util_delay_power_off_stop();
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->PowerOnOffSound == OPTION_ON
        && app_helper.low_battery_power_off == 0) {
        beep_power_off = 1;
        beep_play(BEEP_ID_POWER_OFF);
    }
    while (rec_dvr_is_event_recording(CAMERA_CHANNEL_BOTH) != 0 && timeout_times <= 20) {
        msleep(100);
        timeout_times += 1;
    }
    if (beep_power_off == 0) {
        sleep(2);
    }
    beep_wait_finish(4000);
    app_helper.is_doing_poweroff = 1;
    msleep(150);
#if 0
    if (app_helper.force_power_off == 0
        && ((app_helper.with_acc == 0 && Pmic_CheckUsbConnected())
            || (app_helper.with_acc && Pmic_GetAccState() && Pmic_CheckUsbConnected()))) {
        Pmic_NormalSoftReset();
    } else 
#endif
    {
        unsigned char power_off_param = 0x00;
        if (app_helper.timer_wakeup_test) {
            Pmic_Write(0x0A, 0xff);
        } else {
            Pmic_Write(0x0A, pSvcUserPref->PowerOffWakeupMinutes);
        }
        Imu_PowerOff(app_helper.low_battery_power_off ? 0 : pSvcUserPref->ParkingLevel);
        if ((pSvcUserPref->EnableLteRemoteWakeup || app_helper.lte_wakeup_test)
            && app_helper.lte_booted) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            if (app_helper.check_lte_left_connected()) {
                //keep left power
                power_off_param |= 0x80;
            } else if (app_helper.check_lte_right_connected()) {
                //keep right power
                power_off_param |= 0x40;
            }
#else
            power_off_param |= 0x40;
#endif
        }
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (app_helper.check_lte_left_connected() || app_helper.check_lte_right_connected()) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
        unsigned int i2c_channel = get_lte_i2c_channel();
#else
        unsigned int i2c_channel = LTE_MCU_I2C_CHANNEL;
#endif
            i2c_write_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x03, 0x00);
        }
#endif
        if (app_helper.force_power_off == 0) {//need detect vbus
            power_off_param |= 0x20;
        }
        if (app_helper.with_acc && Pmic_GetAccState() == 0) {
            power_off_param |= 0x10;
        }
        Pmic_DoPowerOff(power_off_param);
    }

    return 0;
}

static int linux_system_impl(const char *cmd)
{
    AMBA_IPC_REPLY_STATUS_e ret;
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    CLIENT_ID_t clnt;

    if (app_helper.linux_booted == 0) {
        debug_line("%s linux not boot yet", __func__);
        return -1;
    }

    if (cmd == NULL || strlen(cmd) <= 0) {
        debug_line("%s invalid param", __func__);
        return -1;
    }

    clnt = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_UTIL_HOST,
                                AMBA_RPC_PROG_LU_UTIL_PROG_ID,
                                AMBA_RPC_PROG_LU_UTIL_VER);
    if (clnt == NULL) {
        debug_line("%s client create fail", __func__);
        return -1;
    }
    AmbaMisra_TypeCast32(&pExec, &cmd);
    ret = AmbaRpcProg_Util_Exec2_Clnt(pExec, (int *)NULL, clnt);
    AmbaIPC_ClientDestroy(clnt);
    if (ret != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("%s exec cmd fail", __func__);
        return -1;
    }

    return 0;
}

static int linux_system_with_response_impl(const char *cmd, char *response)
{
    AMBA_IPC_REPLY_STATUS_e ret;
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    CLIENT_ID_t clnt;

    if (app_helper.linux_booted == 0) {
        debug_line("%s linux not boot yet", __func__);
        return -1;
    }

    if (cmd == NULL || strlen(cmd) <= 0) {
        debug_line("%s invalid param", __func__);
        return -1;
    }

    clnt = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_UTIL_HOST,
                                AMBA_RPC_PROG_LU_UTIL_PROG_ID,
                                AMBA_RPC_PROG_LU_UTIL_VER);
    if (clnt == NULL) {
        debug_line("%s client create fail", __func__);
        return -1;
    }
    AmbaMisra_TypeCast32(&pExec, &cmd);
    ret = AmbaRpcProg_Util_Exec1_Clnt(pExec, (int *)response, clnt);
    AmbaIPC_ClientDestroy(clnt);
    if (ret != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("%s exec cmd fail", __func__);
        return -1;
    }
    if (strlen(response) > 0) {
        if (response[strlen(response) - 1] == '\n') {
            response[strlen(response) - 1] = '\0';
        }
    }
    if (strlen(response) > 0) {
        if (response[strlen(response) - 1] == '\r') {
            response[strlen(response) - 1] = '\0';
        }
    }

    return 0;
}

static int check_file_exist_impl(const char *path, unsigned long long *filesize)
{
    struct stat fstat;

    if (stat(path, &fstat) == 0) {
        if (filesize != NULL) {
            *filesize = fstat.size;
        }
        return 1;
    }
    if (filesize != NULL) {
        *filesize = 0;
    }

    return 0;
}

static int check_sd_exist_impl(void)
{
    if (sd_bad_or_not_exist == 0 && AmbaSD_IsCardPresent(SD_CHANNEL)) {
        return 1;
    }
    return 0;
}

static int get_sd_space_impl(unsigned int *total_m, unsigned int *free_m)
{
    AMBA_FS_DRIVE_INFO_t DriveInfo;
    UINT64 FreeSize = 0, TotalSize = 0;

    if (check_sd_exist_impl() == 0) {
        return -1;
    }
    if (AMBA_FS_ERR_NONE != AmbaFS_GetDriveInfo(SD_SLOT[0], &DriveInfo)) {
        return -1;
    }
    TotalSize = (UINT64)DriveInfo.BytesPerSector * (UINT64)DriveInfo.SectorsPerCluster * (UINT64)DriveInfo.TotalClusters;
    FreeSize = (UINT64)DriveInfo.BytesPerSector * (UINT64)DriveInfo.SectorsPerCluster * (UINT64)DriveInfo.EmptyClusters;
    (*total_m) = TotalSize / 1024 / 1024;
    (*free_m) = FreeSize / 1024 / 1024;

    return 0;
}

struct _fw_path_s_ {
    char path[32];
} fw_paths[] = {
    {"x:\\"SD_FWUPDATE_BLD_NAME},
    {"x:\\"SD_FWUPDATE_NAME},
};
typedef struct _fw_path_s_ fw_path_s;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
static int check_emmc_update_impl(void)
{
    unsigned int i = 0;
    unsigned long long filesize = 0;

    for (i = 0; i < sizeof(fw_paths) / sizeof(fw_path_s); i++) {
        fw_paths[i].path[0] = EMMC_SLOT[0];
        if (check_file_exist_impl(fw_paths[i].path, &filesize) && filesize > 0) {
            return 1;
        }
    }

    return 0;
}
#endif

static int check_sd_update_impl(void)
{
    unsigned int i = 0;
    unsigned long long filesize = 0;

    for (i = 0; i < sizeof(fw_paths) / sizeof(fw_path_s); i++) {
        fw_paths[i].path[0] = SD_SLOT[0];
        if (check_file_exist_impl(fw_paths[i].path, &filesize) && filesize > 0) {
            return 1;
        }
    }

    return 0;
}

static int touch_impl(const char *path)
{
    void *pFile = NULL;

    pFile = fopen(path, "w");
    if (pFile == NULL) {
        return -1;
    }
    fclose(pFile);

    return 0;
}

static device_info_s *get_device_info_impl(void)
{
    Pmic_GetVersion(device_info.mcu_version);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    {
    static unsigned char data = 0;
    if (data == 0) {
        memset(device_info.mcu_version2, 0, sizeof(device_info.mcu_version2));
        if (app_helper.check_lte_left_connected() || app_helper.check_lte_right_connected()) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
        unsigned int i2c_channel = get_lte_i2c_channel();
#else
        unsigned int i2c_channel = LTE_MCU_I2C_CHANNEL;
#endif
            if (i2c_read_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x00, &data) >= 0) {
                snprintf(device_info.mcu_version2, sizeof(device_info.mcu_version2) - 1, "V%d.%d", (data >> 4) & 0xF, data & 0xF);
            }
        }
    }
    }
#endif

    return &device_info;
}

static void update_time_for_linux_impl(void)
{
    char cmd[256] = {0};
    time_s cur_time;

    memset(cmd, 0, sizeof(cmd));
    time_now(&cur_time);
    snprintf(cmd, sizeof(cmd) - 1, "date -s \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\"&",
                                    cur_time.year, cur_time.month, cur_time.day,
                                    cur_time.hour, cur_time.minute, cur_time.second);
    app_helper.linux_system(cmd);
}

static int check_external_camera_connected_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    static int connected = -1;

    if (connected < 0) {
        unsigned char data[2] = {0};
        AmbaUserGPIO_SensorResetCtrl(AMBA_VIN_CHANNEL1, AMBA_GPIO_LEVEL_LOW);
        msleep(2);
        AmbaUserGPIO_SensorResetCtrl(AMBA_VIN_CHANNEL1, AMBA_GPIO_LEVEL_HIGH);
        msleep(3);
		AmbaUserVIN_SensorClkCtrl(AMBA_VIN_CHANNEL1, 24000000U);
    	msleep(5);
        i2c_read(AMBA_I2C_CHANNEL1, 0x6e, 0xf0, data, 2);
        debug_line("check_external_camera_connected, chip_id: 0x%.2x%.2x", data[0], data[1]);
        if (data[0] == 0x20 && data[1] == 0x53) {
            connected = 1;
        } else {
            connected = 0;
        }
        AmbaUserGPIO_SensorResetCtrl(AMBA_VIN_CHANNEL1, AMBA_GPIO_LEVEL_LOW);
		AmbaUserVIN_SensorClkCtrl(AMBA_VIN_CHANNEL1, 0U);
    }

    return connected;
#else
    return 0;
#endif
}

#if defined(CONFIG_BSP_CV25_NEXAR_D080)
static int check_d080_with_2nd_sensor_impl(void)
{
    static unsigned int data = 0;

    if (check_external_camera_connected_impl() == 1) {
        return 1;
    }
    if (data > 100) {
        return 1;
    }
    //read ir adc
    AmbaADC_Config(10000);
    AmbaADC_SingleRead(AMBA_ADC_CHANNEL1, &data);
    if (data > 100) {
        return 1;
    }
    return 0;
}
#endif

//openssl dgst -sha256 -sign private.pem -out uart-[SERIAL]-[YYYY][MM].auth uart-[SERIAL]-[YYYY][MM].txt
static int check_enable_linux_uart_impl(char disk)
{
    (void)disk;
    return 0;
}

static int check_interface_is_ready_impl(const char *name)
{
    char cmd[128] = {0};
    char response[1024] = {0};
    char *p = NULL;

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "ifconfig %s | grep HWaddr", name);
    memset(response, 0, sizeof(response));
    app_helper.linux_system_with_response(cmd, response);
    p = strstr(response, "HWaddr ");
    if (p != NULL) {
        return 1;
    }
    return 0;
}

static int check_lte_is_ready_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D080) || defined(CONFIG_BSP_H32_NEXAR_D081)
    return check_interface_is_ready_impl("usb0");
#else
    return check_interface_is_ready_impl("wwan0");
#endif
}

static int check_bt_is_ready_impl(void)
{
    char response[1024] = {0};
    char *p = NULL;

    memset(response, 0, sizeof(response));
    app_helper.linux_system_with_response("hciconfig hci0 | grep \"BD Address\"", response);
    p = strstr(response, "BD Address:");
    if (p != NULL) {
        return 1;
    }
    return 0;
}

static int get_interface_ip_impl(const char *name, char *ip)
{
    char cmd[128] = {0};
    char response[1024] = {0};
    char *p = NULL;

    if (ip == NULL) {
        return -1;
    }
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "ifconfig %s | grep \"inet addr\"", name);
    memset(response, 0, sizeof(response));
    app_helper.linux_system_with_response(cmd, response);
    p = strstr(response, "inet addr:");
    if (p != NULL) {
        p = p + 10;
        while (*p != ' ') {
            *ip = *p;
            p += 1;
            ip += 1;
        }
        return 0;
    }

    return -1;
}

static int set_digital_mode_impl(int view_id, int mode)
{
    AMBA_IK_MODE_CFG_s ModeCfg;

    AmbaWrap_memset(&ModeCfg, 0, sizeof(AMBA_IK_MODE_CFG_s));
    Amba_IQParam_Load_CC_By_DE(view_id, mode);
    AmbaImgProc_DESetDEffect(view_id, &ModeCfg, (UINT8)mode);

    return 0;
}

static int read_all_text_impl(const char *path, unsigned char *buf, unsigned int buf_size)
{
    struct stat fstat;
    FILE *pFile = NULL;
    int read_size = 0;

    if (path == NULL || buf == NULL || buf_size == 0) {
        return -1;
    }

    if (stat(path, &fstat) != 0 || fstat.size == 0) {
        return -1;
    }

    if (fstat.size > buf_size) {
        read_size = buf_size;
    } else {
        read_size = fstat.size;
    }

    pFile = fopen(path, "rb");
    if (pFile == NULL) {
        return -1;
    }
    if (fread(buf, 1, read_size, pFile) == read_size) {
        fclose(pFile);
        return read_size;
    }
    fclose(pFile);

    return -1;
}

static int write_all_text_impl(const char *path, const unsigned char *buf, unsigned int buf_len)
{
    void *pFile = NULL;

    if (path == NULL || buf == NULL || buf_len == 0) {
        return -1;
    }

    pFile = fopen(path, "w");
    if (pFile == NULL) {
        return -1;
    }
    fwrite(buf, buf_len, 1, pFile);
    fclose(pFile);

    return 0;
}

//0 means disable watchdog
static int set_watchdog_impl(int seconds)
{
    (void)seconds;
    return 0;
}

static int check_camera_is_working_impl(CAMERA_CHANNEL_e channel)
{
    UINT32 IrqId = 0, count = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    int enable = 0;

    SvcUserPref_Get(&pSvcUserPref);
    if (channel == CAMERA_CHANNEL_INTERNAL) {
        IrqId = 143;
        enable = pSvcUserPref->InternalCamera.Enable;
    } else if (channel == CAMERA_CHANNEL_EXTERNAL) {
        IrqId = 168;
        enable = app_helper.external_camera_enabled;
    } else {
        return -1;
    }
    if (enable == 0) {
        return 0;
    }
    count = AmbaDiag_GetIrqCounter(IrqId);
    msleep(200);
    if (AmbaDiag_GetIrqCounter(IrqId) > count) {
#ifdef CONFIG_SENSOR_GC2053
        if (channel == CAMERA_CHANNEL_EXTERNAL) {
            UINT16 data[2] = {0};
            AMBA_SENSOR_CHANNEL_s sensor_channel;
            sensor_channel.VinID = 1;
            sensor_channel.SensorID = 1;
            AmbaSensor_RegisterRead(&sensor_channel, 0xF0, data);
            AmbaSensor_RegisterRead(&sensor_channel, 0xF1, data + 1);
            if (data[0] == 0x20 && data[1] == 0x53){
                return 1;
            }
            return 0;
        }
#endif
        return 1;
    }

    return 0;
}

static int set_ae_table_impl(CAMERA_CHANNEL_e channel, unsigned char ae_table[8][12])
{
    int vin = 0, i = 0, j = 0;
    AE_ALGO_INFO_s AeInfo;

    if (channel != CAMERA_CHANNEL_INTERNAL) {
        return -1;
    }
    if (app_helper.external_camera_enabled) {
        vin = 1;
    } else {
        vin = 0;
    }
    AmbaImgProc_AEGetAEAlgoInfo(vin, &AeInfo);
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 12; j++) {
            AeInfo.RoiInfo[0][i * 12 + j] = ae_table[i][j];
        }
    }
    AmbaImgProc_AESetAEAlgoInfo(vin, &AeInfo);

    return 0;
}

static unsigned int default_ae_table[96] = {
    01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01,
    01, 01, 01, 01, 01, 02, 02, 01, 01, 01, 01, 01,
    01, 01, 01, 01, 02, 03, 03, 02, 01, 01, 01, 01,
    01, 01, 01, 02, 03, 04, 04, 03, 02, 01, 01, 01,
    01, 01, 01, 02, 03, 05, 05, 03, 02, 01, 01, 01,
    01, 01, 02, 03, 04, 05, 05, 04, 03, 02, 01, 01,
    01, 02, 03, 04, 04, 04, 04, 04, 04, 03, 02, 01,
    01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01
};
static int reset_ae_table_impl(CAMERA_CHANNEL_e channel)
{
    int vin = 0, i = 0;
    AE_ALGO_INFO_s AeInfo;

    if (channel == CAMERA_CHANNEL_INTERNAL) {
        vin = 0;
    } else if (channel == CAMERA_CHANNEL_EXTERNAL) {
        vin = 1;
    } else {
        return -1;
    }
    AmbaImgProc_AEGetAEAlgoInfo(vin, &AeInfo);
    for (i = 0; i < 96; i++) {
        AeInfo.RoiInfo[0][i] = default_ae_table[i];
    }
    AmbaImgProc_AESetAEAlgoInfo(vin, &AeInfo);

    return 0;
}

static int convert_linux_path_to_rtos_impl(const char *linux_path, char *rtos_path)
{
    unsigned int i = 0;

    if (linux_path == NULL || strlen(linux_path) <= 0 || rtos_path == NULL) {
        return -1;
    }

    if (strncasecmp(linux_path, "/mnt/extsd", strlen("/mnt/extsd")) == 0) {
        sprintf(rtos_path, "%s", SD_SLOT":\\");
        strcat(rtos_path, linux_path + strlen("/mnt/extsd/"));
    } else {
        debug_line("invalid path. not start with /media/extsd\n");
        return -1;
    }

    for (i = 0; i < strlen(rtos_path); i++) {
        if (*(rtos_path + i) == '/') {
            *(rtos_path + i) = '\\';
        }
    }

    return 0;
}

static int convert_rtos_path_to_linux_impl(const char *rtos_path, char *linux_path)
{
    unsigned int i = 0;

    if (linux_path == NULL || rtos_path == NULL || strlen(rtos_path) <= 0) {
        return -1;
    }
    if (strncasecmp(rtos_path, SD_SLOT":\\", strlen(SD_SLOT":\\")) == 0) {
        sprintf(linux_path, "%s", "/mnt/extsd/");
        strcat(linux_path, rtos_path + strlen(SD_SLOT":\\"));
    } else {
        debug_line("invalid path. not start with b:\\ or c:\\ or d:\\\n");
        return -1;
    }

    for (i = 0; i < strlen(linux_path); i++) {
        if (*(linux_path + i) == '\\') {
            *(linux_path + i) = '/';
        }
    }

    return 0;
}

static int set_file_readonly_impl(const char *path, int readonly)
{
    AMBA_FS_FILE_INFO_s fileInfo;

    if (AmbaFS_GetFileInfo(path, &fileInfo) == AMBA_FS_ERR_NONE) {
        if (readonly) {
            if ((fileInfo.Attr & AMBA_FS_ATTR_RDONLY) == AMBA_FS_ATTR_RDONLY) {
                return 0;
            }
            fileInfo.Attr |= AMBA_FS_ATTR_RDONLY;
        } else {
            if ((fileInfo.Attr & AMBA_FS_ATTR_RDONLY) != AMBA_FS_ATTR_RDONLY) {
                return 0;
            }
            fileInfo.Attr &= ~AMBA_FS_ATTR_RDONLY;
        }
        if (AmbaFS_ChangeFileMode(path, fileInfo.Attr) == AMBA_FS_ERR_NONE) {
            return 0;
        }
    }

    return -1;
}

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
static int check_gc2053_left_connected_impl(void)
{
    return (gpio_get_impl(GPIO_PIN_37) == 0) && (gpio_get_impl(GPIO_PIN_41) == 1);
}

static int check_gc2053_right_connected_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161)
    return (gpio_get_impl(GPIO_PIN_36) == 0) && (gpio_get_impl(GPIO_PIN_34) == 1);
#else
    return (gpio_get_impl(GPIO_PIN_38) == 0) && (gpio_get_impl(GPIO_PIN_34) == 1);
#endif
}

static int check_lte_left_connected_impl(void)
{
    return (gpio_get_impl(GPIO_PIN_41) == 0) && (gpio_get_impl(GPIO_PIN_37) == 1);
}

static int check_lte_right_connected_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161)
    return (gpio_get_impl(GPIO_PIN_34) == 0) && (gpio_get_impl(GPIO_PIN_36) == 1);
#else
    return (gpio_get_impl(GPIO_PIN_34) == 0) && (gpio_get_impl(GPIO_PIN_38) == 1);
#endif
}

static int check_usb_left_connected_impl(void)
{
    return (gpio_get_impl(GPIO_PIN_41) == 0) && (gpio_get_impl(GPIO_PIN_37) == 0);
}

static int check_usb_right_connected_impl(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161)
    return (gpio_get_impl(GPIO_PIN_36) == 0) && (gpio_get_impl(GPIO_PIN_34) == 0);
#else
    return (gpio_get_impl(GPIO_PIN_38) == 0) && (gpio_get_impl(GPIO_PIN_34) == 0);
#endif
}
#endif

#if !defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
typedef struct _time_map_s_ {
    unsigned int tick;
    linux_time_s linux_time;
} time_map_s;
#endif
static int get_linux_time_impl(linux_time_s *linux_time)
{
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
    time_s time;
    AMBA_RTC_DATE_TIME_s rtc_time;
    unsigned int local_seconds = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (linux_time == NULL) {
        return -1;
    }
    gnss_time_now(&time);

    rtc_time.Year = time.year;
    rtc_time.Month = time.month;
    rtc_time.Day = time.day;
    rtc_time.Hour = time.hour;
    rtc_time.Minute = time.minute;
    rtc_time.Second = time.second;
    AmbaTime_UtcDateTime2TimeStamp(&rtc_time, &(linux_time->sec));
    linux_time->usec = time.msecond * 1000;

    linux_time->utc_time.year = time.year;
    linux_time->utc_time.month = time.month;
    linux_time->utc_time.day = time.day;
    linux_time->utc_time.hour = time.hour;
    linux_time->utc_time.minute = time.minute;
    linux_time->utc_time.second = time.second;

    SvcUserPref_Get(&pSvcUserPref);
    local_seconds = linux_time->sec + pSvcUserPref->TimeZone;
    AmbaTime_UtcTimeStamp2DateTime(local_seconds, &rtc_time);
    linux_time->local_time.year = rtc_time.Year;
    linux_time->local_time.month = rtc_time.Month;
    linux_time->local_time.day = rtc_time.Day;
    linux_time->local_time.hour = rtc_time.Hour;
    linux_time->local_time.minute = rtc_time.Minute;
    linux_time->local_time.second = rtc_time.Second;
#else
    AMBA_IPC_REPLY_STATUS_e Status;
    static response_time_s response_time = {0};
    static CLIENT_ID_t ClientID = NULL;
    int time_offset = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    AMBA_RTC_DATE_TIME_s rtc_time;
    static time_map_s time_map = {0};
    static unsigned char map_flag = 0;
    static unsigned int fail_tick = 0;

    if (linux_time == NULL) {
        return -1;
    }

    memset(linux_time, 0, sizeof(linux_time_s));
    if (app_helper.linux_booted == 0) {
        return -1;
    }
    SvcUserPref_Get(&pSvcUserPref);
    time_offset = pSvcUserPref->TimeZone;

    if (ClientID == NULL) {
        ClientID = AmbaIPC_ClientCreate((INT32)AMBA_IPC_HOST_LINUX,
                                         (INT32)AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_PROG_ID,
                                         (INT32)AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_VER);
        if (ClientID == NULL) {
            //debug_line("[%s] AmbaIPC_ClientCreate failed", __func__);
            return -1;
        }
    }
    Status = AmbaIPC_ClientCall(ClientID, (INT32)AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_IPC,
                                &time_offset, sizeof(time_offset) + 1,
                                &response_time, sizeof(response_time), 2000);
    if (Status != AMBA_IPC_REPLY_SUCCESS) {
        if(fail_tick == 0 || (tick() - fail_tick) > 5000) {
            fail_tick = tick();
            debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
        }
        //debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
        if (map_flag) {
            unsigned int cur_tick = tick();
            unsigned int pass_seconds = (cur_tick - time_map.tick) / 1000;
            unsigned int pass_useconds = (cur_tick - time_map.tick) % 1000 * 1000;
            if (time_map.linux_time.usec + pass_useconds > 1000000U) {
                pass_seconds += 1;
                pass_useconds = time_map.linux_time.usec + pass_useconds - 1000000U;
            }
            //debug_line("time map=%d.%.6d, tick=%d, cur_tick=%d, tick_passed:%d, ofsset=%d.%.6d", time_map.linux_time.sec, time_map.linux_time.usec, time_map.tick,
                //cur_tick, cur_tick - time_map.tick, pass_seconds, pass_useconds);
            linux_time->sec = time_map.linux_time.sec + pass_seconds;
            linux_time->usec = time_map.linux_time.usec + pass_useconds;
            AmbaTime_UtcTimeStamp2DateTime(linux_time->sec, &rtc_time);
            linux_time->utc_time.year = rtc_time.Year;
            linux_time->utc_time.month = rtc_time.Month;
            linux_time->utc_time.day = rtc_time.Day;
            linux_time->utc_time.hour = rtc_time.Hour;
            linux_time->utc_time.minute = rtc_time.Minute;
            linux_time->utc_time.second = rtc_time.Second;
            AmbaTime_UtcTimeStamp2DateTime(linux_time->sec + time_offset, &rtc_time);
            linux_time->local_time.year = rtc_time.Year;
            linux_time->local_time.month = rtc_time.Month;
            linux_time->local_time.day = rtc_time.Day;
            linux_time->local_time.hour = rtc_time.Hour;
            linux_time->local_time.minute = rtc_time.Minute;
            linux_time->local_time.second = rtc_time.Second;
            //debug_line("time map=%d-%d-%d %.2d:%.2d:%.2d, new_time:%d-%d-%d %.2d:%.2d:%.2d",
                //time_map.linux_time.local_time.year, time_map.linux_time.local_time.month, time_map.linux_time.local_time.day,
                //time_map.linux_time.local_time.hour, time_map.linux_time.local_time.minute, time_map.linux_time.local_time.second,
                //linux_time->local_time.year, linux_time->local_time.month, linux_time->local_time.day,
                //linux_time->local_time.hour, linux_time->local_time.minute, linux_time->local_time.second);
            return 0;
        }
    }
    time_map.tick = tick();
    linux_time->sec = response_time.tv_sec;
    linux_time->usec = response_time.tv_usec;
    linux_time->utc_time.year = response_time.utc_time.year;
    linux_time->utc_time.month = response_time.utc_time.month;
    linux_time->utc_time.day = response_time.utc_time.day;
    linux_time->utc_time.hour = response_time.utc_time.hour;
    linux_time->utc_time.minute = response_time.utc_time.minute;
    linux_time->utc_time.second = response_time.utc_time.second;
    linux_time->local_time.year = response_time.local_time.year;
    linux_time->local_time.month = response_time.local_time.month;
    linux_time->local_time.day = response_time.local_time.day;
    linux_time->local_time.hour = response_time.local_time.hour;
    linux_time->local_time.minute = response_time.local_time.minute;
    linux_time->local_time.second = response_time.local_time.second;
    memcpy(&(time_map.linux_time), linux_time, sizeof(linux_time_s));
    map_flag = 1;
#endif

    return 0;
}

static const char *wakeup_source_2_str_impl(int reason)
{
    switch (reason) {
    case BOOT_REASON_VBUS_TRIGGER:
        return "vbus";
    case BOOT_REASON_ACC_TRIGGER:
        return "acc";
    case BOOT_REASON_BUTTON_TIGGER:
        return "button";
    case BOOT_REASON_HARD_RESET_TRIGGER:
        return "hard_reset";
    case BOOT_REASON_IMU_TRIGGER:
        return "imu";
    case BOOT_REASON_LTE_TRIGGER:
        return "lte";
    case BOOT_REASON_TIMER_TRIGGER:
        return "timer";
    case BOOT_REASON_SOFT_RESET_TRIGGER:
        return "soft_reset";
    case BOOT_REASON_WATCHDOG_TRIGGER:
        return "watchdog";
    case BOOT_REASON_FACTORY_RESET_TRIGGER:
        return "factory_reset";
    default:
        break;
    }

    return "unknown";
}

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
static const char *addon_type_2_str_impl(int type)
{
    if (type == ADDON_TYPE_USB_LINE) {
        return "usb_line";
    }
    if (type == ADDON_TYPE_SENSOR_BOARD) {
        return "sensor_board";
    }
    if (type == ADDON_TYPE_LTE_WIFI_BOARD) {
        return "lte_board";
    }
    return "none";
}

static int get_left_addon_type_impl(void)
{
    int left_type = ADDON_TYPE_NONE;

    if (app_helper.check_usb_left_connected()) {
        left_type = ADDON_TYPE_USB_LINE;
    } else if (app_helper.check_gc2053_left_connected()) {
        left_type = ADDON_TYPE_SENSOR_BOARD;
    } else if (app_helper.check_lte_left_connected()) {
        left_type = ADDON_TYPE_LTE_WIFI_BOARD;
    }
    return left_type;
}

static int get_right_addon_type_impl(void)
{
    int right_type = ADDON_TYPE_NONE;

    if (app_helper.check_usb_right_connected()) {
        right_type = ADDON_TYPE_USB_LINE;
    } else if (app_helper.check_gc2053_right_connected()) {
        right_type = ADDON_TYPE_SENSOR_BOARD;
    } else if (app_helper.check_lte_right_connected()) {
        right_type = ADDON_TYPE_LTE_WIFI_BOARD;
    }    
    return right_type;
}
#endif

static POWER_SOURCE_e get_power_source_impl(void)
{
    if (Pmic_CheckUsbConnected() == 0) {
        return POWER_SOURCE_BATTERY;
    }
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (app_helper.check_usb_left_connected() || app_helper.check_usb_right_connected()) {
        return POWER_SOURCE_ADDON_LINE;
    }
    return POWER_SOURCE_MOUNT;
#else
    return POWER_SOURCE_ADDON_LINE;
#endif
}

static const char *power_source_2_str_impl(int reason)
{
    if (reason == POWER_SOURCE_BATTERY) {
        return "battery";
    }
    if (reason == POWER_SOURCE_ADDON_LINE) {
        return "addon_connector";
    }
    return "mount";
}

#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
unsigned int get_brightness_i2c_channel(void)
{
    if (app_helper.check_gc2053_left_connected()) {
        return AMBA_I2C_CHANNEL0;
    }
    return AMBA_I2C_CHANNEL1;
}

unsigned int get_ir_i2c_channel(void)
{
    if (app_helper.check_gc2053_left_connected()) {
        return AMBA_I2C_CHANNEL0;
    }
    return AMBA_I2C_CHANNEL1;
}

unsigned int get_lte_i2c_channel(void)
{
    if (app_helper.check_lte_left_connected()) {
        return AMBA_I2C_CHANNEL0;
    }
    return AMBA_I2C_CHANNEL1;
}

unsigned int get_sensor_i2c_channel(void)
{
    if (app_helper.check_gc2053_left_connected()) {
        return AMBA_I2C_CHANNEL0;
    }
    return AMBA_I2C_CHANNEL1;
}
#endif

