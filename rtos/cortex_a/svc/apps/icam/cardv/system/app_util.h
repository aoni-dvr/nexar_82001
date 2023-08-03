#ifndef __APP_UTIL_H__
#define __APP_UTIL_H__

typedef enum _ADDON_TYPE_e_ {
    ADDON_TYPE_NONE = 0,
    ADDON_TYPE_USB_LINE,
    ADDON_TYPE_SENSOR_BOARD,
    ADDON_TYPE_LTE_WIFI_BOARD,
} ADDON_TYPE_e;

typedef enum _POWER_SOURCE_e_ {
    POWER_SOURCE_BATTERY = 0,
    POWER_SOURCE_ADDON_LINE,
    POWER_SOURCE_MOUNT,
    POWER_SOURCE_NUM
} POWER_SOURCE_e;

typedef enum _POWER_OFF_MODE_e_ {
    POWER_OFF_MODE_FULL_SHUTDOWN = 0,
    POWER_OFF_MODE_LTE_LOW_POWER,
    POWER_OFF_MODE_MCU_TIMER,
    POWER_OFF_MODE_IMU_WAKEUP,
    POWER_OFF_MODE_NUM
} POWER_OFF_MODE_e;

int app_util_init(void);
int app_util_check_external_sensor_device(void);
int app_util_check_lte_usb_mass_storage(void);
int app_util_delay_power_off_start(void);
int app_util_delay_power_off_stop(void);
int app_util_deinit(void);
int app_util_do_factory_reset(void);
int app_util_do_hard_reset(void);
int app_util_cancel_power_off(void);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
int app_util_external_camera_monitor_start(void);
#endif
int app_util_check_external_camera_enabled(void);
int app_util_logo_led_fade_start(void);
int app_util_logo_led_fade_set_param(unsigned int v1, unsigned int v2);

#endif//__APP_UTIL_H__

