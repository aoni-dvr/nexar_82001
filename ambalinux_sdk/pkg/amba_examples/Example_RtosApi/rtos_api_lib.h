#ifndef __RTOS_API_LIB_H__
#define __RTOS_API_LIB_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _CAMERA_CHANNEL_e_ {
    CAMERA_CHANNEL_INTERNAL = 0,
    CAMERA_CHANNEL_EXTERNAL,
    CAMERA_CHANNEL_BOTH
} CAMERA_CHANNEL_e;

typedef enum _ADDON_TYPE_e_ {
    ADDON_TYPE_NONE = 0,
    ADDON_TYPE_USB_LINE,
    ADDON_TYPE_SENSOR_BOARD,
    ADDON_TYPE_LTE_WIFI_BOARD,
} ADDON_TYPE_e;

typedef enum _WAKEUP_SOURCE_e_ {
    WAKEUP_SOURCE_VBUS = 0x01,
    WAKEUP_SOURCE_BUTTON = 0x04,
    WAKEUP_SOURCE_IMU = 0x10,
    WAKEUP_SOURCE_LTE = 0x20,
    WAKEUP_SOURCE_TIMER = 0x40,
    WAKEUP_SOURCE_WATCHDOG = 0x100,
} WAKEUP_SOURCE_e;

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

typedef enum _SPEED_UNIT_e_ {
    SPEED_UNIT_KMH = 0,
    SPEED_UNIT_MPH = 1,
} SPEED_UNIT_e;

typedef enum _BEEP_ID_e_ {
    BEEP_ID_POWER_ON = 0,
    BEEP_ID_POWER_OFF,
    BEEP_ID_RESET,
    BEEP_ID_PIV,
    BEEP_ID_EVENT_RECORD,
    BEEP_ID_BUTTON_PRESS,
    BEEP_ID_1K,
    BEEP_ID_SAMPLE_AAC,
    BEEP_ID_NUM
} BEEP_ID_e;

typedef enum _MIC_TYPE_e_ {
    MIC_TYPE_AMIC = 0,
    MIC_TYPE_DMIC,
    MIC_TYPE_DMIC2,
    MIC_TYPE_NUM
} MIC_TYPE_e;

typedef struct _stream_share_info_s {
    unsigned long long share_addr;
    unsigned long long phy_addr;
    unsigned int share_size;
} stream_share_info_s;

typedef struct _config_s_ {
    unsigned int rtsp_flag;
    unsigned int record_quality;
    unsigned int record_bitrate;
    unsigned int record_duration;
    unsigned int rear_quality;
    unsigned int rear_bitrate;
    unsigned int rear_frame;
    unsigned int photo_quality;
    unsigned int white_balance;
    unsigned int exposure;
    unsigned int boot_record;
    unsigned int mute;
    unsigned int rear_view_mirror;
    unsigned int language;
    unsigned int smart_detect;
    unsigned int frontcar_crash_sensitivity;
    unsigned int impact_sensitivity;
    unsigned int motion_detect;
    unsigned int watermark;
    unsigned int watermark_pos;
    unsigned int record_switch;
    unsigned int dev_type;
    unsigned int curRecordTime;
    unsigned int videoTime;
    unsigned int shotRecord;
    unsigned int shutdown_mode;
    unsigned int debug_mode;
    unsigned int sound_vol;
    unsigned int wakeup_val;
    unsigned int support_obd;
    unsigned int support_backcamera;
    unsigned int support_adas;
    unsigned int only_gsensor_startup;
    unsigned int support_battery;
    unsigned int support_gps;
    unsigned int support_edog;
    unsigned int support_gui;
    unsigned int support_wifi;
    unsigned int support_mobile;
    unsigned int support_wifi_connect;
    unsigned int support_low_voltage;
    unsigned int config_version;
    unsigned int adas_fcw_switch;
    unsigned int adas_fcw_sensitivity;
    unsigned int adas_fcw_speed;
    unsigned int adas_ldw_switch;
    unsigned int adas_ldw_sensitivity;
    unsigned int adas_tips_mode;
    unsigned int gsp_data_version;
    unsigned int gps_tips_mode;
    unsigned int startup_voice;
    unsigned int net_preview_quality;
    unsigned int rear_preview_quality;
    unsigned int record_delayp;
    unsigned int photo_timetakephotos;
    unsigned int photo_automatictakephotos;
    unsigned int image_rotation;
    unsigned int light_freq;
    unsigned int led_on_off;
    unsigned int screen_sleep;
    unsigned int measure_unit;
    unsigned int auto_shutdown_time;
    unsigned int record_quality_bitmap;
    unsigned int lang_bitmap;
    unsigned int voice_lang_bitmap;
    unsigned int voice_lang;
    unsigned int take_photo_mode;
    unsigned int slow_record_mode;
    unsigned int motion_take_photo_mode;
    unsigned int wifi_connect_mode;
    int wifi_boot_connect_mode;
    unsigned int p2p_wait_time;
    unsigned int switch_keytone;
    unsigned int switch_takephoto_tone;
    unsigned int wifi_type;
    unsigned int obd_low_voltage;
    unsigned int obd_winter_low_voltage;
    unsigned int battery_power_time;
    unsigned int wifi_channel;
    unsigned int net_preview_bitrate;
    unsigned int net_preview_frame;
    unsigned int park_timelapse;
    unsigned int time_format;
    unsigned int ir_threshold;
    unsigned int ir_sampling;
    unsigned int shutdown_wait_time;
    char firmware_version[32];
    char buildTime[32];
    char manufacturer[32];
    char otaVersion[32];
    char product_sn[32];
    char product_cpuid[64];
    char product_vsn[64];
    char obd_version[32];
    char impact_value[32];
    //char token[64];
    char wifiPsd[32];
    char wifi_ssid[32];
    char wifi_ap_ssid[32];
    char wifi_ap_passwd[32];
} config_s;

typedef struct _state_s_ {
    unsigned char record;
    unsigned char mute;
    unsigned char obd;
    unsigned char backcamera;
    unsigned char battery;
    unsigned int uptime;
    unsigned char debug_wakeup_val;
    unsigned int timenow;
    int timezone;
    unsigned int rtctime;
    unsigned int battery_voltage;
    unsigned char battery_charging;
    unsigned char battery_online;
    unsigned char rear_record_disable;
    int acc_state;
    int usb_state;
    int tf_state;
    unsigned int tf_slow_times;
    unsigned int startup_index;
    unsigned int valid_data_index;
    unsigned int obd_voltage;
    int obd_state;
    unsigned int obd_rt_time;
    unsigned int gps;
    unsigned int gps_time;
    double gps_lng;
    double gps_lat;
    unsigned int gps_speed;
    unsigned int gps_degree;
    unsigned int gps_gga_time;
    unsigned int gps_gga_state;
    unsigned int gps_satellite;
    unsigned int gps_msl;
    unsigned int gps_geoid;
    unsigned int gps_hdop;
    unsigned int ir_state;
    unsigned int ir_switch_time;
    double gsensor_temp;
    char tripkey[32];
} state_s;

typedef struct _tfcard_s_ {
    unsigned char state;
    unsigned int remain;
    unsigned int total;
    char oemid[16];
    char cid[64];
    char csd[64];
    char fwrev[16];
    char hwrev[16];
    char serial[32];
    char scr[32];
    char name[16];
    char manfid[16];
    char date[16];
} tfcard_s;

typedef struct _gnss_s_ {
    unsigned int uart_bitrate;
    unsigned int support_gnssid;
    unsigned int enable_gnssid;
    unsigned int aop_config;
    unsigned int mga_type;
    unsigned int mga_time_s;
    unsigned int mga_time_e;
    unsigned int data_rate;
    unsigned int nmea_version;
    char gnss_model[64];
} gnss_s;

typedef struct _info_s_ {
    unsigned char protocol_ver[2];
    unsigned char wifi_status;
    unsigned int ibeacon_major;
    unsigned int ibeacon_minor;
    char vendor[16];
    char model[16];
    char sn[32];
    char wifi_ssid[32];
    char capabilities[32];
} info_s;

typedef struct _mmapInfo_s_ {
    unsigned char *base;
    unsigned int size;
    long long offset;
} mmapInfo_s;

typedef struct _dev_info_s_ {
    char token[64];
    unsigned char first_access;
} dev_info_s;

typedef struct _imu_share_frame_data_s_ {
    unsigned char data[32];
} imu_share_frame_data_s;

typedef struct _addon_type_s_ {
    int left_value;
    char left_name[32];
    int right_value;
    char right_name[32];
} addon_type_s;

typedef struct _source_info_s_ {
    int value;
    char name[32];
} source_info_s;

typedef struct _battery_info_s_ {
    int adc;
    int raw_adc;
    int mcu_adc;
    int percentage;
    int voltage_mv;
    unsigned char charger_ic_is_charge;
    unsigned char charger_ic_power_good;
    unsigned char charger_ic_thermal_protect;
    float imu_temperature;
    int cable_adc;
} battery_info_s;

typedef struct _sd_card_info_s_ {
    int status;
    int used_mb;
    int total_mb;
    int free_mb;
} sd_card_info_s;

typedef struct _basic_info_s_ {
    char model[16];
    char cpu_id[64];
    char device_sn[32];
    unsigned char wifi_mac[6];
    unsigned char bt_mac[6];
    char sw_version[64];
    char build_time[64];
    char mcu_main_version[64];
    char mcu_lte_version[64];
    char imu_type[16];
    addon_type_s addon_type;
    sd_card_info_s sd_card_info;
    source_info_s wakeup_source;
    source_info_s power_source;
    battery_info_s battery_info;
    int factory_reset;
    int hard_reset;
    unsigned char acc_state;
} basic_info_s;

typedef struct _color_s_ {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} color_s;

int rtos_api_lib_reboot(void);
int rtos_api_lib_format_disk(unsigned int sd_disk);
int rtos_api_lib_default_setting(void);
int rtos_api_lib_play_sound_start(const char *path);
int rtos_api_lib_play_sound_stop(void);
int rtos_api_lib_is_sound_playing(unsigned char *is_playing, unsigned int *remaining_ms);
int rtos_api_lib_get_audio_volume(unsigned char *volume);
int rtos_api_lib_set_audio_volume(unsigned char volume);
int rtos_api_lib_start_event_record(CAMERA_CHANNEL_e channel, unsigned int pre_seconds, unsigned int length, char *path_internal, char *path_external);
int rtos_api_lib_stop_event_record(CAMERA_CHANNEL_e channel);
int rtos_api_lib_is_event_recording(CAMERA_CHANNEL_e channel, char **path_to_video);
int rtos_api_lib_capture_piv(CAMERA_CHANNEL_e channel, char *path_internal, char *path_external);
int rtos_api_lib_capture_audio_start(const char *path);
int rtos_api_lib_capture_audio_stop(void);
int rtos_api_lib_get_hw_sn(char *sn);
int rtos_api_lib_get_fwupdate_flag(unsigned char *flag);
int rtos_api_lib_set_fwupdate_flag(void);
int rtos_api_lib_clear_fwupdate_flag(void);
int rtos_api_lib_check_external_camera_connected(int *connected);
int rtos_api_lib_get_gpio_level(int gpio, int *level);
int rtos_api_lib_dump_rtos_log(const char *path);
int rtos_api_lib_get_file_list_info(unsigned char *is_ready, unsigned int *amount, stream_share_info_s *stream_info);
int rtos_api_lib_get_config(config_s *info);
int rtos_api_lib_get_state(state_s *info);
int rtos_api_lib_get_tfcard(tfcard_s *info);
int rtos_api_lib_get_gnss(gnss_s *info);
int rtos_api_lib_get_info(info_s *info);
int rtos_api_lib_get_netcard_info(const char *iface, char *ip, char *mac);
int rtos_api_lib_set_ev(int channel, int value);
int rtos_api_lib_set_wb(int channel, int value);
int rtos_api_lib_set_record_quality(int channel, int value);
int rtos_api_lib_set_record_fps(int channel, int value);
int rtos_api_lib_set_record_bitrate(int channel, int value);
int rtos_api_lib_set_record_duration(int channel, int value);
int rtos_api_lib_set_record_quality_and_bitrate(int channel, int quality, int bitrate);
int rtos_api_lib_set_preview_quality(int channel, int value);
int rtos_api_lib_set_preview_fps(int channel, int value);
int rtos_api_lib_set_preview_bitrate(int channel, int value);
int rtos_api_lib_set_preview_quality_and_bitrate(int channel, int quality, int bitrate);
int rtos_api_lib_set_image_rotation(int channel, int value);
int rtos_api_lib_start_record(int channel);
int rtos_api_lib_stop_record(int channel);
int rtos_api_lib_delete_file(const char *path);
int rtos_api_lib_set_mute(int mute);
int rtos_api_lib_set_watermark(int mode);
int rtos_api_lib_set_watermark_pos(int mode);
int rtos_api_lib_set_time_format(int is_24_mode);
int rtos_api_lib_set_time_zone(int zone);
int rtos_api_lib_get_time_zone(void);
int rtos_api_lib_set_debug_mode(int enable);
int rtos_api_lib_set_rtsp_cam(int value);
int rtos_api_lib_lock_file(const char *path, int lock);
int rtos_api_lib_lock_file_time_range(unsigned int start_time, unsigned int end_time, int lock);
int rtos_api_lib_set_battery_power_time(int value);
int rtos_api_lib_set_shutdown_wait_time(int value);
int rtos_api_lib_set_ir_threshold(int value);
int rtos_api_lib_set_ir_sampling(int value);
int rtos_api_lib_set_wifi_boot_mode(int mode);
int rtos_api_lib_set_wifi_connect_mode(int mode);
int rtos_api_lib_set_wifi_channel(int channel);
int rtos_api_lib_set_wifi_type(int is_5g, int channel);
int rtos_api_lib_set_wifi_ap_password(const char *password);
int rtos_api_lib_set_wifi_sta_info(const char *ssid, const char *password);
int rtos_api_lib_set_wifi_p2p_wait_time(int time);
int rtos_api_lib_get_bt_imu_stream_info(stream_share_info_s *stream_info);
int rtos_api_lib_get_bt_gnss_stream_info(stream_share_info_s *stream_info);
int rtos_api_lib_set_bt_imu_stream_enable(unsigned char enable);
int rtos_api_lib_set_bt_gnss_stream_enable(unsigned char enable);
unsigned long long rtos_api_lib_convert_memory2linux(stream_share_info_s stream_info, mmapInfo_s *map_info);
int rtos_api_lib_munmap(mmapInfo_s *finfo);
int rtos_api_lib_do_gsensor_calibrate(int value);
int rtos_api_lib_set_gsensor_calibration_effect(int value);
int rtos_api_lib_set_gsensor_debug(int value);
int rtos_api_lib_set_gsensor_range(int value);
int rtos_api_lib_set_gsensor_wakeup_value(int value);
int rtos_api_lib_set_gsensor_impact_value(int value);
int rtos_api_lib_set_gsensor_freq(unsigned char value);
int rtos_api_lib_set_accel_odr(int value);
int rtos_api_lib_set_accel_range(int value);
int rtos_api_lib_set_gyro_odr(int value);
int rtos_api_lib_set_gyro_range(int value);
int rtos_api_lib_set_gyro_calibration_effect(int value);
int rtos_api_lib_set_imu_swap_polarity(int value);
int rtos_api_lib_set_pairing_reminder_mode(int mode, const char *voice_path);
int rtos_api_lib_set_pairing_reminder_voice(int play_count, int delay_ms);
int rtos_api_lib_reset_gnss(int value);
int rtos_api_lib_set_gnss_datarate(int value);
int rtos_api_lib_set_gnssid(int value);
int rtos_api_lib_set_gnss_mgaoff(int value);
int rtos_api_lib_get_dev_info(dev_info_s *dev_info);
int rtos_api_lib_set_dev_info_first_accessed(void);
int rtos_api_lib_set_led_reg_value(unsigned char reg, unsigned char value);
int rtos_api_lib_get_led_reg_value(unsigned char reg, unsigned char *value);
int rtos_api_lib_soft_reset(void);
int rtos_api_lib_factory_reset(void);
int rtos_api_lib_get_usb_line_connected(void);
int rtos_api_lib_set_button_param(unsigned int x, unsigned int y, unsigned int z);
int rtos_api_lib_get_addon_type(ADDON_TYPE_e *left_type, ADDON_TYPE_e *right_type);
int rtos_api_lib_get_wakeup_source(WAKEUP_SOURCE_e *source, char *name);
int rtos_api_lib_set_mcu_timer_wakeup_time(int minutes);
int rtos_api_lib_get_mcu_timer_wakeup_time(int *minutes);
int rtos_api_lib_set_power_off_mode(POWER_OFF_MODE_e mode);
int rtos_api_lib_get_stream_share_info(stream_share_info_s *stream_info);
int rtos_api_lib_set_stream_share_enable(int index, int enable);
int rtos_api_lib_get_timestamp_from_filepath(const char *filepath, unsigned int *timestamp);
int rtos_api_lib_get_burn_info(unsigned char *wifi_mac, unsigned char *bt_mac);
int rtos_api_lib_get_rtos_log_share_info(stream_share_info_s *stream_info);
int rtos_api_lib_set_rtos_log_enable(int enable);
int rtos_api_lib_get_imu_fifo_share_info(stream_share_info_s *stream_info);
int rtos_api_lib_get_imu_fifo_data(int max_frame_num, int *frame_num);
int rtos_api_lib_get_power_source(POWER_SOURCE_e *source, char *name);
int rtos_api_lib_get_basic_info(basic_info_s *info);
int rtos_api_lib_power_off(void);
int rtos_api_lib_play_beep_id(BEEP_ID_e beep_id);
int rtos_api_lib_set_led_color(color_s color);
int rtos_api_lib_get_led_color(color_s *color);
int rtos_api_lib_set_led_rtos_control_enable(int enable);
int rtos_api_lib_get_led_rtos_control_enable(int *enable);
int rtos_api_lib_set_usb_mass_storage_enable(int enable);
int rtos_api_lib_get_usb_mass_storage_enable(int *enable);
int rtos_api_lib_set_speed_unit(SPEED_UNIT_e spped_unit);
int rtos_api_lib_get_speed_unit(SPEED_UNIT_e *speed_unit);
int rtos_api_lib_set_power_onoff_sound_enable(int enable);
int rtos_api_lib_get_power_onoff_sound_enable(int *enable);
int rtos_api_lib_set_video_encryption_enable(int enable);
int rtos_api_lib_get_video_encryption_enable(int *enable);
int rtos_api_lib_set_video_encryption_key(const char *key);
int rtos_api_lib_get_video_encryption_key(char *key);
int rtos_api_lib_set_authorization_key(const char *key);
int rtos_api_lib_get_authorization_key(char *key);
int rtos_api_lib_set_camera_boot_auto_record(CAMERA_CHANNEL_e channel, int record);
int rtos_api_lib_get_camera_boot_auto_record(int *internal_record, int *external_record);
int rtos_api_lib_set_mic(MIC_TYPE_e type);
int rtos_api_lib_get_mic(MIC_TYPE_e *type);
int rtos_api_lib_set_parking_level(unsigned char value);
int rtos_api_lib_get_parking_level(unsigned char *value);
int rtos_api_lib_set_lte_wakeup_enable(int enable);
int rtos_api_lib_get_lte_wakeup_enable(int *enable);
int rtos_api_lib_set_api_files_enable(int enable);
int rtos_api_lib_get_api_files_enable(int *enable);
int rtos_api_lib_mcu_update(void);
int rtos_api_lib_restart_lte_usb_wifi(void);
int rtos_api_lib_get_acc_state(unsigned char *state);
int rtos_api_lib_cancel_shutdown(void);
int rtos_api_lib_set_lte_power(int on);
int rtos_api_lib_check_lte_working(int *active);
int rtos_api_lib_check_sensor_working(int channel, int *active);
int rtos_api_lib_lte_enter_wakeup_mode(void);
int rtos_api_lib_set_cv_enable(int enable);
int rtos_api_lib_get_cv_enable(int *enable);
int rtos_api_lib_exec_shell_command(const char *cmdline);
int rtos_api_lib_get_battery_info(basic_info_s *info);
int rtos_api_lib_get_hard_reset_flag(int *enable);
int rtos_api_lib_gnss_power_on(void);
int rtos_api_lib_gnss_power_off(void);
int rtos_api_lib_gnss_reset(void);
int rtos_api_lib_set_bt_power(int on);
int rtos_api_lib_set_state_led_power(int on);
int rtos_api_lib_set_logo_led_power(int on);
int rtos_api_lib_set_ir_led_power(int on);
int rtos_api_lib_set_charge_enable(int on);
int rtos_api_lib_set_imu_enable(int on);
int rtos_api_lib_set_road_camera_enable(int on);
int rtos_api_lib_set_cabin_camera_enable(int on);

// event api
typedef enum _event_id_e_ {
    NOTIFY_POWER_STATE_CHANGE_ID = 0,
    NOTIFY_LED_IRQ_STATE_CHANGE_ID,
    NOTIFY_USB_LINE_STATE_CHANGE_ID,
    NOTIFY_BUTTON_STATE_LONG_PRESS,
    NOTIFY_BUTTON_STATE_SHORT_PRESS,
    NOTIFY_FILE_CREATED,
    NOTIFY_FILE_DELETED,
    NOTIFY_CAMERA_SHUTDOWN,
    NOTIFY_MAIN_MCU_UPDATE,
    NOTIFY_LTE_MCU_UPDATE,
    NOTIFY_ACC_CONNECTED,
    NOTIFY_ACC_DISCONNECTED,
} event_id_e;

typedef struct _file_arg_s_ {
    char path[64];
    unsigned int file_time;
    unsigned long long file_size;
} file_arg_s;

typedef union _ipc_arg_u_ {
    file_arg_s file_arg;
} ipc_arg_u;

typedef struct _ipc_event_s {
    int event_id;
    int param;
    int param2;
    ipc_arg_u arg;
} ipc_event_s;

typedef int (*event_cb_func)(ipc_event_s);

int rtos_api_lib_event_init(void);
int rtos_api_lib_event_deinit(void);
int rtos_api_lib_register_event_cb(event_cb_func func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif//__RTOS_API_LIB_H__

