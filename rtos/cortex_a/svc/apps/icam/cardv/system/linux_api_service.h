#ifndef __LINUX_API_SERVICE_H__
#define __LINUX_API_SERVICE_H__

#include "linux_api_event.h"

typedef struct _event_record_param_s {
    unsigned char channel;
    unsigned int pre_seconds;
    unsigned int length;
} event_record_param_s;

typedef struct _piv_param_s {
    unsigned char channel;
} piv_param_s;

typedef struct _arg_obj_s {
    unsigned char channel;
    int arg;
    int arg2;
    char data[64];
    char data2[64];
} arg_obj_s;

typedef struct _reg_value_s {
    unsigned char reg;
    unsigned char value;
} reg_value_s;

typedef struct _button_param_s {
    unsigned int x;
    unsigned int y;
    unsigned int z;
} button_param_s;

typedef struct _stream_param_s {
    int index;
    int enable;
} stream_param_s;

typedef struct _imu_share_frame_data_s_ {
    unsigned char data[32];
} imu_share_frame_data_s;

typedef struct _cur_timestamp_s_ {
    unsigned int tv_sec;
    unsigned int tv_usec;
} cur_timestamp_s;

typedef enum _MIC_TYPE_e_ {
    MIC_TYPE_AMIC = 0,
    MIC_TYPE_DMIC,
    MIC_TYPE_DMIC2,
    MIC_TYPE_NUM
} MIC_TYPE_e;

typedef struct _linux_api_cmd_s_ {
    unsigned int msg_id;
    union cmd_item {
        unsigned char audio_volume;
        int enable;
        event_record_param_s event_record_param;
        piv_param_s piv_param;
        unsigned char ir_brightness;
        unsigned char gpio_id;
        unsigned char sd_disk;
        arg_obj_s arg_obj;
        reg_value_s reg_value;
        button_param_s button_param;
        int mcu_wakeup_time;
        int power_off_mode;
        stream_param_s stream_param;
        unsigned char speed_unit;
        int max_frame_num;
        int beep_id;
        color_s led_color;
        char key[64];
        int mic_type;
        unsigned char parking_level;
        char cmdline[128];
        cur_timestamp_s cur_timestamp;
    } item;
} linux_api_cmd_s;

typedef struct _sound_play_status_s_ {
    unsigned char is_playing;
    unsigned int remaining_ms;
} sound_play_status_s;

typedef struct _filelist_info_s {
    unsigned char ready;
    unsigned int file_amount[3];
    unsigned long long share_addr;
    unsigned long long phy_addr;
    unsigned int share_size;
} filelist_info_s;

typedef struct _stream_share_info_s {
    unsigned long long share_addr;
    unsigned long long phy_addr;
    unsigned int share_size;
} stream_share_info_s;

typedef struct _response_path_s_ {
    char path1[128];
    char path2[128];
} response_path_s;

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

typedef struct _addon_type_s_ {
    int left_value;
    char left_name[32];
    int right_value;
    char right_name[32];
} addon_type_s;

typedef struct _burn_info_s_ {
    unsigned char wifi_mac[6];
    unsigned char bt_mac[6];
} burn_info_s;

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

typedef struct _linux_api_response_s_ {
    unsigned int msg_id;
    int rval;
    union response_item {
        unsigned char button_pressed;
        unsigned char external_camera_connected;
        unsigned char audio_volume;
        unsigned char power_state;
        unsigned char sn[32];
        unsigned char fwupdate_flag;
        unsigned char card_tray_exist;
        unsigned char gpio_level;
        sound_play_status_s sound_play_status;
        response_path_s filepath;
        filelist_info_s filelist_info;
        stream_share_info_s stream_share_info;
        config_s config;
        state_s state;
        tfcard_s tfcard;
        gnss_s gnss;
        info_s info;
        int timezone;
        unsigned char reg_value;
        addon_type_s addon_type;
        source_info_s wakeup_source;
        source_info_s power_source;
        burn_info_s burn_info;
        basic_info_s basic_info;
        int frame_num;
        color_s led_color;
        int enable;
        unsigned char speed_unit;
        char key[64];
        int mcu_wakeup_time;
        int mic_type;
        unsigned char parking_level;
        unsigned char acc_state;
        unsigned char active;
        cur_timestamp_s cur_timestamp;
    } item;
} linux_api_response_s;

typedef enum _linux_api_cmd_e_ {
    LINUX_API_CMD_REBOOT = 0,
    LINUX_API_CMD_FORMAT_SD,
    LINUX_API_CMD_DEFAULT_SETTING,
    LINUX_API_CMD_GET_BUTTON_PRESSED,
    LINUX_API_CMD_TURN_ONOFF_LED,
    LINUX_API_CMD_LOCK_LED,
    LINUX_API_CMD_GET_IMU_DATA,
    LINUX_API_CMD_GET_THERMAL_DATA,
    LINUX_API_CMD_PLAY_SOUND_START,
    LINUX_API_CMD_PLAY_SOUND_STOP,//10
    LINUX_API_CMD_IS_SOUND_PLAYING,
    LINUX_API_CMD_GET_AUDIO_VOLUME,
    LINUX_API_CMD_SET_AUDIO_VOLUME,
    LINUX_API_CMD_START_EVENT_RECORD,
    LINUX_API_CMD_STOP_EVENT_RECORD,//15
    LINUX_API_CMD_IS_EVENT_RECORDING,
    LINUX_API_CMD_CAPTURE_PIV,
    LINUX_API_CMD_CAPTURE_AUDIO_START,
    LINUX_API_CMD_CAPTURE_AUDIO_STOP,
    LINUX_API_CMD_GET_POWER_STATE,//20
    LINUX_API_CMD_GET_HW_SN,
    LINUX_API_CMD_LIST_ECC_KEYS,
    LINUX_API_CMD_WRITE_ECC_KEY,
    LINUX_API_CMD_EXPORT_ECC_PUBKEY,
    LINUX_API_CMD_SIGN_MESSAGE,//25
    LINUX_API_CMD_LIST_AES_KEYS,
    LINUX_API_CMD_WRITE_AES_KEY,
    LINUX_API_CMD_READ_AES_KEY,
    LINUX_API_CMD_SET_TIME,
    LINUX_API_CMD_GET_TIME,//30
    LINUX_API_CMD_GET_FWUPDATE_FLAG,
    LINUX_API_CMD_SET_FWUPDATE_FLAG,
    LINUX_API_CMD_CLEAR_FWUPDATE_FLAG,
    LINUX_API_CMD_SET_IR_BRIGHTNESS,
    LINUX_API_CMD_CHECK_EXTERNAL_CAMERA_CONNECTED,//35
    LINUX_API_CMD_CHECK_CARD_TRAY_EXIST,
    LINUX_API_CMD_GET_GPIO_LEVEL,
    LINUX_API_CMD_SET_EXPOSURE_AREA,
    LINUX_API_CMD_RESET_EXPOSURE_AREA,
    LINUX_API_CMD_SET_LED_BRIGHTNESS,
    LINUX_API_CMD_DUMP_RTOS_LOG,
    LINUX_API_CMD_GET_FILE_LIST_INFO,
    LINUX_API_CMD_GET_CONFIG,
    LINUX_API_CMD_GET_STATE,
    LINUX_API_CMD_GET_TFCARD,
    LINUX_API_CMD_GET_GNSS,
    LINUX_API_CMD_GET_INFO,
    LINUX_API_CMD_SET_EV_BIAS,
    LINUX_API_CMD_SET_WB,
    LINUX_API_CMD_START_STOP_RECORD,
    LINUX_API_CMD_DELETE_FILE,
    LINUX_API_CMD_SET_MUTE,
    LINUX_API_CMD_SET_WATERMARK,
    LINUX_API_CMD_SET_WATERMARK_POS,
    LINUX_API_CMD_SET_BATTERY_POWER_TIME,
    LINUX_API_CMD_SET_SHUTDOWN_WAIT_TIME,
    LINUX_API_CMD_SET_RECORD_DURATION,
    LINUX_API_CMD_SET_RECORD_QUALITY,
    LINUX_API_CMD_SET_RECORD_FPS,
    LINUX_API_CMD_SET_RECORD_BITRATE,
    LINUX_API_CMD_SET_RECORD_QUALITY_AND_BITRATE,
    LINUX_API_CMD_SET_PREVIEW_QUALITY,
    LINUX_API_CMD_SET_PREVIEW_FPS,
    LINUX_API_CMD_SET_PREVIEW_BITRATE,
    LINUX_API_CMD_SET_PREVIEW_QUALITY_AND_BITRATE,
    LINUX_API_CMD_SET_IMAGE_ROTATION,
    LINUX_API_CMD_SET_TIME_FORMAT,
    LINUX_API_CMD_SET_TIME_ZONE,
    LINUX_API_CMD_GET_TIME_ZONE,
    LINUX_API_CMD_SET_DEBUG_MODE,
    LINUX_API_CMD_SET_IR_THRESHOLD,
    LINUX_API_CMD_SET_IR_SAMPLING,
    LINUX_API_CMD_SET_RTSP_CAM,
    LINUX_API_CMD_LOCK_FILE,
    LINUX_API_CMD_LOCK_FILE_TIME_RANGE,
    LINUX_API_CMD_SET_WIFI_BOOT_MODE,
    LINUX_API_CMD_SET_WIFI_CONNECT_MODE,
    LINUX_API_CMD_SET_WIFI_CHANNEL,
    LINUX_API_CMD_SET_WIFI_TYPE,
    LINUX_API_CMD_SET_WIFI_AP_PASSWORD,
    LINUX_API_CMD_SET_WIFI_STA_INFO,
    LINUX_API_CMD_SET_WIFI_P2P_WAIT_TIME,
    LINUX_API_CMD_GET_BT_IMU_STREAM_SHARE_INFO,
    LINUX_API_CMD_GET_BT_GNSS_STREAM_SHARE_INFO,
    LINUX_API_CMD_SET_BT_IMU_STREAM_ENABLE,
    LINUX_API_CMD_SET_BT_GNSS_STREAM_ENABLE,
    LINUX_API_CMD_DO_GSENSOR_CALIBRATE,
    LINUX_API_CMD_SET_GSENSOR_CALIBRATION_EFFECT,
    LINUX_API_CMD_SET_GSENSOR_DEBUG,
    LINUX_API_CMD_SET_GSENSOR_WAKEUP_VALUE,
    LINUX_API_CMD_SET_GSENSOR_IMPACT_VALUE,
    LINUX_API_CMD_SET_GSENSOR_FREQ,
    LINUX_API_CMD_SET_ACCEL_ODR,
    LINUX_API_CMD_SET_ACCEL_RANGE,
    LINUX_API_CMD_SET_GYRO_ODR,
    LINUX_API_CMD_SET_GYRO_RANGE,
    LINUX_API_CMD_SET_GYRO_CALIBRATION_EFFECT,
    LINUX_API_CMD_SET_IMU_SWAP_POLARITY,
    LINUX_API_CMD_SET_PAIRING_REMINDER_MODE,
    LINUX_API_CMD_SET_PAIRING_REMINDER_VOICE,
    LINUX_API_CMD_RESET_GNSS,
    LINUX_API_CMD_SET_GNSS_DATARATE,
    LINUX_API_CMD_SET_GNSSID,
    LINUX_API_CMD_SET_GNSS_MGAOFF,
    LINUX_API_CMD_SET_LED_REG,
    LINUX_API_CMD_GET_LED_REG,
    LINUX_API_CMD_SOFT_RESET,
    LINUX_API_CMD_FACTORY_RESET,
    LINUX_API_CMD_GET_USB_LINE_CONNECTED,
    LINUX_API_CMD_SET_BUTTON_PARAM,
    LINUX_API_CMD_GET_ADDON_TYPE,
    LINUX_API_CMD_GET_WAKEUP_SOURCE,
    LINUX_API_CMD_SET_MCU_WAKEUP_TIME,
    LINUX_API_CMD_GET_MCU_WAKEUP_TIME,
    LINUX_API_CMD_SET_POWER_OFF_MODE,
    LINUX_API_CMD_GET_STREAM_SHARE_INFO,
    LINUX_API_CMD_SET_STREAM_SHARE_ENABLE,
    LINUX_API_CMD_GET_FACTORY_BURN_INFO,
    LINUX_API_CMD_GET_RTOS_LOG_SHARE_INFO,
    LINUX_API_CMD_SET_RTOS_LOG_SHARE_ENABLE,
    LINUX_API_CMD_GET_IMU_FIFO_SHARE_INFO,
    LINUX_API_CMD_GET_IMU_FIFO_DATA,
    LINUX_API_CMD_GET_POWER_SOURCE,
    LINUX_API_CMD_GET_BASIC_INFO,
    LINUX_API_CMD_POWER_OFF,
    LINUX_API_CMD_PLAY_BEEP_ID,
    LINUX_API_CMD_SET_LED_COLOR,
    LINUX_API_CMD_GET_LED_COLOR,
    LINUX_API_CMD_SET_LED_RTOS_CONTROL_ENABLE,
    LINUX_API_CMD_GET_LED_RTOS_CONTROL_ENABLE,
    LINUX_API_CMD_SET_USB_MASS_STORAGE_ENABLE,
    LINUX_API_CMD_GET_USB_MASS_STORAGE_ENABLE,
    LINUX_API_CMD_SET_SPEED_UNIT,
    LINUX_API_CMD_GET_SPEED_UNIT,
    LINUX_API_CMD_SET_POWER_ONOFF_SOUND_ENABLE,
    LINUX_API_CMD_GET_POWER_ONOFF_SOUND_ENABLE,
    LINUX_API_CMD_SET_VIDEO_ENCRYPTION_ENABLE,
    LINUX_API_CMD_GET_VIDEO_ENCRYPTION_ENABLE,
    LINUX_API_CMD_SET_VIDEO_ENCRYPTION_KEY,
    LINUX_API_CMD_GET_VIDEO_ENCRYPTION_KEY,
    LINUX_API_CMD_SET_AUTHORIZATION_KEY,
    LINUX_API_CMD_GET_AUTHORIZATION_KEY,
    LINUX_API_CMD_SET_CAMERA_AUTO_RECORD,
    LINUX_API_CMD_GET_CAMERA_AUTO_RECORD,
    LINUX_API_CMD_SET_MIC,
    LINUX_API_CMD_GET_MIC,
    LINUX_API_CMD_SET_PARKING_LEVEL,
    LINUX_API_CMD_GET_PARKING_LEVEL,
    LINUX_API_CMD_SET_LTE_WAKEUP_ENABLE,
    LINUX_API_CMD_GET_LTE_WAKEUP_ENABLE,
    LINUX_API_CMD_SET_API_FILES_ENABLE,
    LINUX_API_CMD_GET_API_FILES_ENABLE,
    LINUX_API_CMD_MCU_UPDATE_START,
    LINUX_API_CMD_RESTART_LTE_USB_WIFI_MODULE,
    LINUX_API_CMD_GET_ACC_STATE,
    LINUX_API_CMD_CANCEL_SHUTDOWN,
    LINUX_API_CMD_SET_LTE_POWER,
    LINUX_API_CMD_CHECK_LTE_WORKING,
    LINUX_API_CMD_CHECK_SENSOR_WORKING,
    LINUX_API_CMD_LTE_ENTER_WAKEUP_MODE,
    LINUX_API_CMD_SET_CV_ENABLE,
    LINUX_API_CMD_GET_CV_ENABLE,
    LINUX_API_CMD_EXEC_SHELL_COMMAND,
    LINUX_API_CMD_GET_BATTERY_INFO,
    LINUX_API_CMD_GET_HARD_RESET_FLAG,
    LINUX_API_CMD_SET_GNSS_POWER,
    LINUX_API_CMD_SET_BT_POWER,
    LINUX_API_CMD_SET_STATE_LED_POWER,
    LINUX_API_CMD_SET_LOGO_LED_POWER,
    LINUX_API_CMD_SET_IR_LED_POWER,
    LINUX_API_CMD_SET_CHARGE_ENABLE,
    LINUX_API_CMD_SET_IMU_ENABLE,
    LINUX_API_CMD_SET_ROAD_CAMERA_ENABLE,
    LINUX_API_CMD_SET_CABIN_CAMERA_ENABLE,
    //LINUX_API_CMD_SET_TIME,
    //LINUX_API_CMD_GET_TIME,
    LINUX_API_CMD_NUM
} linux_api_cmd_e;

int linux_api_service_start(void);
void linux_api_service_set_debug(int enable);
int linux_api_service_get_debug(void);

#endif//__LINUX_API_SERVICE_H__

