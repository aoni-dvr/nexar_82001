#ifndef __APP_HELPER_H__
#define __APP_HELPER_H__

#include "rec/rec_dvr.h"
#include "system/app_util.h"
#include "platform.h"

#if defined(CONFIG_ENABLE_EMMC_BOOT)
#define NVM_TYPE        (AMBA_NVM_eMMC)
#define SD_CHANNEL      (AMBA_SD_CHANNEL1)
#define EMMC_SLOT       "C"
#define SD_SLOT         "D"
#define REC_DEST_DISK   "D"
#define FILE_LOG_DISK   "C"
#else
#define NVM_TYPE (AMBA_NVM_NAND)
#define SD_CHANNEL      (AMBA_SD_CHANNEL0)
#define SD_SLOT         "C"
#define REC_DEST_DISK   "C"
#define FILE_LOG_DISK   "C"
#endif

typedef struct _device_info_s_ {
    char sw_version[64];
    char build_time[64];
    char build_timestamp[32];
    char mcu_version[64];
    char mcu_version2[64];
    char vsn[64];
    unsigned int startup_index;
    unsigned int ibeacon_major;
    unsigned int ibeacon_minor;
    char trip_key[64];
} device_info_s;

typedef struct _linux_time_s_ {
    unsigned int sec;//utc
    unsigned int usec;
    time_s utc_time;
    time_s local_time;
} linux_time_s;

typedef struct _app_helper_s_ {
    int sd_init_done;
    int sd_card_idle;
    int acc_connected;
    int with_acc;
    int linux_console_enable;
    unsigned char internal_camera_auto_record;
    unsigned char external_camera_auto_record;
    unsigned char internal_camera_enabled;
    unsigned char external_camera_enabled;
    unsigned char mcu_update_flag;
    unsigned int ir_last_changed_time;
    int usb_mass_storage_on;
    int usb_mass_storage_mode;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    int left_addon_type;
    int right_addon_type;
#endif
    unsigned char hard_reset_flag;
    unsigned char factory_reset_flag;
    unsigned char power_on_with_sd_card;
    unsigned char linux_booted;
    unsigned char wifi_booted;
    unsigned char bt_booted;
    unsigned char usb_wifi_booted;
    unsigned char lte_on;
    unsigned char lte_booted;
    unsigned int lte_booted_tick;
    unsigned char format_busy;
    unsigned char force_power_off;
    unsigned char external_camera_connected;
    unsigned char low_battery;
    unsigned char new_led_board;
    unsigned char led_control;
    int (*linux_system)(const char *cmd);
    int (*linux_system_with_response)(const char *cmd, char *response);
    int (*lock_led)(int flag);
    int (*lock_ir_led)(int flag);
    int (*gpio_set)(int gpio, int level);
    int (*gpio_get)(int gpio);
    int (*set_led_onoff)(int on);
    int (*set_led_color)(unsigned char r, unsigned char g, unsigned char b);
    int (*set_led_brihtness)(unsigned char brightness);
    int (*set_led_fade)(int enable);
    int (*set_ir_brightness)(unsigned char brightness);
    int (*set_led_enable)(unsigned char enable);
    int (*do_power_off)(void);
    int (*check_file_exist)(const char *path, unsigned long long *filesize);
    int (*check_sd_update)(void);
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    int (*check_emmc_update)(void);
#endif
    int (*touch)(const char *path);
    int (*check_sd_exist)(void);
    int (*get_sd_space)(unsigned int *total_m, unsigned int *free_m);
    device_info_s *(*get_device_info)(void);
    void (*time_update_for_linux)(void);
    int (*check_external_camera_connected)(void);
    int (*check_enable_linux_uart)(char disk);
    int (*check_lte_is_ready)(void);
    int (*check_interface_is_ready)(const char *name);
    int (*get_interface_ip)(const char *name, char *ip);
    int (*check_bt_is_ready)(void);
    int (*set_digital_mode)(int view_id, int mode);
    int (*read_all_text)(const char *path, unsigned char *buf, unsigned int buf_size);
    int (*write_all_text)(const char *path, const unsigned char *buf, unsigned int buf_len);
    int (*set_watchdog)(int seconds);
    int (*check_camera_is_working)(CAMERA_CHANNEL_e channel);
    int (*set_ae_table)(CAMERA_CHANNEL_e channel, unsigned char ae_table[8][12]);
    int (*reset_ae_table)(CAMERA_CHANNEL_e channel);
    int (*convert_path_linux2rtos)(const char *, char *);
    int (*convert_path_rtos2linux)(const char *, char *);
    int (*set_file_readonly)(const char *path, int readonly);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    int (*check_gc2053_left_connected)(void);
    int (*check_gc2053_right_connected)(void);
    int (*check_lte_left_connected)(void);
    int (*check_lte_right_connected)(void);
    int (*check_usb_left_connected)(void);
    int (*check_usb_right_connected)(void);
    int (*get_left_addon_type)(void);
    int (*get_right_addon_type)(void);
    const char *(*addon_type_2_str)(int type);
#endif
    int (*get_linux_time)(linux_time_s *linux_time);
    const char *(*wakeup_source_2_str)(int reason);
    POWER_SOURCE_e (*get_power_source)(void);
    const char *(*power_source_2_str)(int reason);
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    int (*set_logo_led_color)(int id, unsigned char r, unsigned char g, unsigned char b);
    int (*set_logo_led_fade)(int enable);
    int (*check_d080_with_2nd_sensor)(void);
#endif
    int (*load_dev_info)(void);
    int (*ir_check_onoff)(void);
    int timer_wakeup_test;
    int lte_wakeup_test;
    int low_battery_power_off;
    int is_doing_poweroff;
    int auto_power_off;
} app_helper_s;

int app_helper_init(void);

extern app_helper_s app_helper;

#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
unsigned int get_brightness_i2c_channel(void);
unsigned int get_ir_i2c_channel(void);
unsigned int get_lte_i2c_channel(void);
unsigned int get_sensor_i2c_channel(void);
#endif

#endif//__APP_HELPER_H__

