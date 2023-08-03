#include "rtos_api_lib.h"
#include "debug.h"
#include "global.h"
#include "cmd_response.h"

typedef struct _ctrl_item_s_ {
    unsigned short cmd;
    int arg;
    char data[128];
    char password[64];
} ctrl_item_s;

typedef int (*ctrl_item_handler_func_s)(const ctrl_item_s item);
typedef struct _ctrl_handler_item_s_ {
    unsigned short cmd;
    ctrl_item_handler_func_s handler;
} ctrl_handler_item_s;

typedef enum _net_cmd_e_ {
    NET_CMD_AUTH_TOKEN = 0xA0EE,
    NET_CMD_SET_NET_PREVIEW_QUALITY = 0xA0B6,
    NET_CMD_SET_NET_PREVIEW_BITRATE = 0xA0D6,
    NET_CMD_SET_NET_PREVIEW_FRAME = 0xA0D8,
    NET_CMD_SET_WIFI_CHANNEL = 0xA0D4,
    NET_CMD_SET_WIFI_TYPE = 0xA0D2,
    NET_CMD_SET_MUTE = 0xA00D,
    NET_CMD_SET_WATERMARK = 0xA01D,
    NET_CMD_SET_DEVICE_TIME = 0xA0B8,
    NET_CMD_REBOOT_DEVICE = 0xA080,
    NET_CMD_SET_WIFI_PWD = 0xA031,
    NET_CMD_SET_BATTERY_POWER_TIME = 0xA0E2,
    NET_CMD_SET_RECORDER_QUALITY = 0xA001,
    NET_CMD_SET_RECORDER_DURATION = 0xA003,
    NET_CMD_SET_WHITE_BALANCE = 0xA007,
    NET_CMD_SET_EXPOSURE = 0xA009,
    NET_CMD_FORMAT_TF_CARD = 0xA01F,
    NET_CMD_RECORD_ON_OFF = 0xA03A,
    NET_CMD_FACTORY_RESET = 0xA054,
    NET_CMD_SET_SOUND_VOL = 0xA084,
    NET_CMD_SET_WAKEUP_VAL = 0xA086,
    NET_CMD_SET_STARTUP_VOICE = 0xA0B4,
    NET_CMD_HEARTBEAT = 0xA0E0,
    NET_CMD_DELETE_FILE = 0xA02C,
    NET_CMD_PLAY_SOUND_PATH = 0xA0E6,
    NET_CMD_SET_GSENSOR_IMPACT_VAL = 0xA0E4,
    NET_CMD_TAKE_PHOTO = 0xA038,
    NET_CMD_CHANGE_WIFI_CONNECT_MODE = 0xA0E8,
    NET_CMD_SET_WIFI_BOOT_CONNECT_MODE = 0xA0EA,
    NET_CMD_SET_P2P_WAIT_TIME = 0xA0EC,
    NET_CMD_SET_DEBUG_MODE = 0xA082,
    NET_CMD_SET_TIME_12HOUR = 0xA0F0,
    NET_CMD_SET_WATERMARK_POS = 0xA0F2,
    NET_CMD_CHANGE_CAM = 0xA036,
    NET_CMD_SET_REARCAM_QUALITY = 0xA0F4,
    NET_CMD_SET_REARCAM_BITRATE = 0xA0F6,
    NET_CMD_SET_REARCAM_FRAME = 0xA0F8,
    NET_CMD_SET_GSENSOR_FREQ = 0xA0FA,
    NET_CMD_SET_REMINDER_MODE = 0xA0FC,
    NET_CMD_SET_REMINDER_PLAY_VOICE = 0xA0FE,
    NET_CMD_SET_IR_THRESHOLD = 0xA102,
    NET_CMD_SET_ADAS_FCW_SWITCH = 0xA090,
    NET_CMD_SET_ADAS_LDW_SWITCH = 0xA096,
    NET_CMD_SET_ADAS_LDW_TIPSMODE = 0xA09A,
    NET_CMD_SET_WIFI_AP_INFO = 0xA0C2,
    NET_CMD_REAR_RECORD_ON_OFF = 0xA116,
    NET_CMD_CALIBRATE_GSENSOR = 0xA106,
    NET_CMD_SET_GSENSOR_CALIBRATION_EFFECT = 0xA10A,
    NET_CMD_SET_GSENSOR_DEBUG = 0xA10C,
    NET_CMD_SET_GSENSOR_RANGE = 0xA10E,
    NET_CMD_SET_GSENSOR_ODR = 0xA110,
    NET_CMD_SET_GSENSOR_SWAPPOLARITY = 0xA112,
    NET_CMD_SET_GSENSOR_POWERBW = 0xA114,
    NET_CMD_SET_IMAGE_ROTATION = 0xA056,
    NET_CMD_SET_DEVICE_MS_TIME = 0xA118,
    NET_CMD_SET_RECORDER_BITRATE = 0xA104,
    NET_CMD_SET_IR_SAMPLING = 0xA11A,
    NET_CMD_LOCK_FILE = 0xA0DC,
    NET_CMD_SET_REAR_PREVIEW_QUALITY = 0xA120,
    NET_CMD_SET_REAR_PREVIEW_BITRATE = 0xA122,
    NET_CMD_SET_REAR_PREVIEW_FRAME = 0xA124,
    NET_CMD_SET_GSENSOR_BMI_ODR = 0xA126,
    NET_CMD_SET_GYRO_ODR = 0xA128,
    NET_CMD_SET_GYRO_RANGE = 0xA12A,
    NET_CMD_SET_GYRO_CALIBRATION_EFFECT = 0xA12C,
    NET_CMD_SET_REAR_ROTATION = 0xA12E,
    NET_CMD_SET_SHOTDOWN_WAIT_TIME = 0xA134,
    NET_CMD_LOCK_TIME_FILE = 0xA138,
    NET_CMD_RESET_GNSS = 0xA144,
    NET_CMD_SET_GNSS_DATA_RATE = 0xA146,
    NET_CMD_ENABLE_GNSSID = 0xA14A,
    NET_CMD_SET_UBX_MGAOFF = 0xA14C,
    NET_CMD_SOFT_RESET = 0xA055,
} net_cmd_e;

static int cmd_ctrl_heart_beat(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return 0;
}

static int cmd_ctrl_set_preview_quality(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_quality(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_preview_bitrate(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_bitrate(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_preview_fps(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_fps(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_recorder_quality(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_quality(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_recorder_bitrate(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_bitrate(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_image_rotation(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_image_rotation(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_record_on_off(const ctrl_item_s item)
{
    int rval = 0;

    debug_line("%s, arg=%d", __func__, item.arg);
    if (item.arg) {
        rval = rtos_api_lib_start_record(CAMERA_CHANNEL_INTERNAL);
    } else {
        rval = rtos_api_lib_stop_record(CAMERA_CHANNEL_INTERNAL);
    }
    return rval;
}

static int cmd_ctrl_set_rearcam_preview_quality(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_quality(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_preview_bitrate(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_bitrate(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_preview_fps(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_preview_fps(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_quality(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_quality(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_bitrate(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_bitrate(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_fps(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_fps(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_rearcam_on_off(const ctrl_item_s item)
{
    int rval = 0;

    debug_line("%s, arg=%d", __func__, item.arg);
    if (item.arg) {
        rval = rtos_api_lib_start_record(CAMERA_CHANNEL_EXTERNAL);
    } else {
        rval = rtos_api_lib_stop_record(CAMERA_CHANNEL_EXTERNAL);
    }
    return rval;
}

static int cmd_ctrl_set_rearcam_rotation(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_image_rotation(CAMERA_CHANNEL_EXTERNAL, item.arg);
}

static int cmd_ctrl_set_recorder_duration(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_record_duration(CAMERA_CHANNEL_BOTH, item.arg);
}

static int cmd_ctrl_take_photo(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_capture_piv(CAMERA_CHANNEL_BOTH, NULL, NULL);
}

static int cmd_ctrl_change_rtsp_cam(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_rtsp_cam(item.arg);
}

static int cmd_ctrl_set_mute(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_mute(item.arg);
}

static int cmd_ctrl_set_watermark(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_watermark(item.arg);
}

static int cmd_ctrl_set_watermark_pos(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_watermark_pos(item.arg);
}

static int cmd_ctrl_set_white_balance(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wb(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_exposure(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_ev(CAMERA_CHANNEL_INTERNAL, item.arg);
}

static int cmd_ctrl_set_power_onoff_sound(const ctrl_item_s item)
{
    int rval = 0;

    debug_line("%s, arg=%d", __func__, item.arg);
    if (item.arg) {
        rval = rtos_api_lib_set_power_onoff_sound_enable(1);
    } else {
        rval = rtos_api_lib_set_power_onoff_sound_enable(0);
    }
    return rval;
}

static int cmd_ctrl_set_sound_vol(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_audio_volume(atoi(item.data));
}

static int cmd_ctrl_play_sound(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_play_sound_start(item.data);
}

static int cmd_ctrl_set_reminder_mode(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_pairing_reminder_mode(item.arg, item.data);
}

static int cmd_ctrl_set_reminder_voice(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_pairing_reminder_voice(item.arg, atoi(item.data));
}

static int cmd_ctrl_set_battery_power_time(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_battery_power_time(item.arg);
}

static int cmd_ctrl_set_shutdown_wait_time(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_shutdown_wait_time(item.arg);
}

static int cmd_ctrl_set_device_time(const ctrl_item_s item)
{
    time_t value = item.arg;
    int timezone = atoi(item.data);

    debug_line("%s, arg=%d,data=%s", __func__, item.arg, item.data);
#if 1
    //int rval = stime(&value);
    //if (rval != 0) {
    //    debug_line("%s, stime fail: %d", __func__, rval);
    //    return -1;
    //}
    //system("hwclock -w");
#else
    char cmd[128] = {0};
    struct tm *p;
    p = gmtime(&value);
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "date -s \"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\" && hwclock -w",
                                                        1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
                                                        p->tm_hour, p->tm_min, p->tm_sec);
    debug_line(cmd);
    system(cmd);
#endif
    return rtos_api_lib_set_time_zone(timezone);
}

static int cmd_ctrl_set_device_ms_time(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    //system("hwclock -w");

    return 0;
}

static int cmd_ctrl_set_time_12hour(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_time_format(item.arg);
}

static int cmd_ctrl_set_ir_threshold(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_ir_threshold(item.arg);
}

static int cmd_ctrl_set_ir_sampling(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_ir_sampling(item.arg);
}

static int cmd_ctrl_set_debug_mode(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_debug_mode(item.arg);
}

static int cmd_ctrl_format(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_format_disk(1);
}

static int cmd_ctrl_factory_reset(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_factory_reset();
}

static int cmd_ctrl_reboot(const ctrl_item_s item)
{
    //do nothing
    return 0;
}

static int cmd_ctrl_lock_file(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_lock_file(item.data, item.arg);
}

static int cmd_ctrl_lock_time_file(const ctrl_item_s item)
{
    unsigned int start_time = 0, end_time = 0;
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_lock_file_time_range(start_time, end_time, item.arg);
}

static int cmd_ctrl_delete_file(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_delete_file(item.data);
}

static int cmd_ctrl_calibrate_gsensor(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_do_gsensor_calibrate(item.arg);
}

static int cmd_ctrl_set_gsensor_calibration_effect(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gsensor_calibration_effect(item.arg);
}

static int cmd_ctrl_set_gsensor_debug(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gsensor_debug(item.arg);
}

static int cmd_ctrl_set_gsensor_range(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_accel_range(item.arg);
}

static int cmd_ctrl_set_gsensor_wakeup_val(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gsensor_wakeup_value(item.arg);
}

static int cmd_ctrl_set_gsensor_impact_val(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gsensor_impact_value(item.arg);
}

static int cmd_ctrl_set_gsensor_freq(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gsensor_freq(item.arg);
}

static int cmd_ctrl_set_gsensor_swap_polarity(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_imu_swap_polarity(item.arg);
}

static int cmd_ctrl_set_accel_odr(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_accel_odr(item.arg);
}

static int cmd_ctrl_set_gyro_odr(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gyro_odr(item.arg);
}

static int cmd_ctrl_set_gyro_range(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gyro_range(item.arg);
}

static int cmd_ctrl_set_gyro_calibration_effect(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gyro_calibration_effect(item.arg);
}

static int cmd_ctrl_reset_gnss(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_reset_gnss(item.arg);
}

static int cmd_ctrl_set_gnss_datarate(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gnss_datarate(item.arg);
}

static int cmd_ctrl_enable_gnssid(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gnssid(item.arg);
}

static int cmd_ctrl_set_gnss_mgaoff(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_gnss_mgaoff(item.arg);
}

static int cmd_ctrl_set_wifi_boot_mode(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_boot_mode(item.arg);
}

static int cmd_ctrl_set_wifi_connect_mode(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_connect_mode(item.arg);
}

static int cmd_ctrl_set_wifi_channel(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_channel(item.arg);
}

static int cmd_ctrl_set_wifi_type(const ctrl_item_s item)//2.4/5g
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_type(item.arg, (strlen(item.data) > 0) ? atoi(item.data) : -1);
}

static int cmd_ctrl_set_wifi_ap_password(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_ap_password(item.data);
}

static int cmd_ctrl_set_wifi_sta_info(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    if (rtos_api_lib_set_wifi_sta_info(item.data, item.password) < 0) {
        return -1;
    }
    if (item.arg == 1) {
        system("sed -i -e 's|\\(.*\\)boot_mode=\\(.*\\)|boot_mode=sta|g' /pref/settings.ini");
        system("/usr/local/share/script/wifi_stop.sh;/usr/local/share/script/sta_start.sh&");
    }
    return 0;
}

static int cmd_ctrl_set_wifi_p2p_wait_time(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_set_wifi_p2p_wait_time(item.arg);
}

static int cmd_ctrl_soft_reset(const ctrl_item_s item)
{
    debug_line("%s, arg=%d", __func__, item.arg);
    return rtos_api_lib_soft_reset();
}

static int cmd_ctrl_do_auth(const ctrl_item_s item)
{
    if (item.arg == 1) {
        if (strlen(item.data) < 6) {
            return RESPONSE_CODE_INVALID_DATA_PARAM;
        }
        if (strcmp(item.data, global_data.dev_info.token) == 0) {
            global_data.authed = 1;
            return 0;
        } else {
            global_data.authed = 0;
            return RESPONSE_CODE_AUTH_FALSE;
        }
    }

    return RESPONSE_CODE_UNAUTHORIZED;
}

static ctrl_handler_item_s ctrl_handler_items[] = {
    {NET_CMD_AUTH_TOKEN,                        cmd_ctrl_do_auth},
    {NET_CMD_SET_NET_PREVIEW_QUALITY,           cmd_ctrl_set_preview_quality},
    {NET_CMD_SET_NET_PREVIEW_BITRATE,           cmd_ctrl_set_preview_bitrate},
    {NET_CMD_SET_NET_PREVIEW_FRAME,             cmd_ctrl_set_preview_fps},
    {NET_CMD_SET_WIFI_CHANNEL,                  cmd_ctrl_set_wifi_channel},
    {NET_CMD_SET_WIFI_TYPE,                     cmd_ctrl_set_wifi_type},
    {NET_CMD_SET_MUTE,                          cmd_ctrl_set_mute},
    {NET_CMD_SET_WATERMARK,                     cmd_ctrl_set_watermark},
    {NET_CMD_SET_DEVICE_TIME,                   cmd_ctrl_set_device_time},
    {NET_CMD_REBOOT_DEVICE,                     cmd_ctrl_reboot},
    {NET_CMD_SET_WIFI_PWD,                      cmd_ctrl_set_wifi_ap_password},
    {NET_CMD_SET_BATTERY_POWER_TIME,            cmd_ctrl_set_battery_power_time},
    {NET_CMD_SET_RECORDER_QUALITY,              cmd_ctrl_set_recorder_quality},
    {NET_CMD_SET_RECORDER_DURATION,             cmd_ctrl_set_recorder_duration},
    {NET_CMD_SET_WHITE_BALANCE,                 cmd_ctrl_set_white_balance},
    {NET_CMD_SET_EXPOSURE,                      cmd_ctrl_set_exposure},
    {NET_CMD_FORMAT_TF_CARD,                    cmd_ctrl_format},
    {NET_CMD_RECORD_ON_OFF,                     cmd_ctrl_record_on_off},
    {NET_CMD_FACTORY_RESET,                     cmd_ctrl_factory_reset},
    {NET_CMD_SET_SOUND_VOL,                     cmd_ctrl_set_sound_vol},
    {NET_CMD_SET_WAKEUP_VAL,                    cmd_ctrl_set_gsensor_wakeup_val},
    {NET_CMD_SET_STARTUP_VOICE,                 cmd_ctrl_set_power_onoff_sound},
    {NET_CMD_HEARTBEAT,                         cmd_ctrl_heart_beat},
    {NET_CMD_DELETE_FILE,                       cmd_ctrl_delete_file},
    {NET_CMD_PLAY_SOUND_PATH,                   cmd_ctrl_play_sound},
    {NET_CMD_SET_GSENSOR_IMPACT_VAL,            cmd_ctrl_set_gsensor_impact_val},
    {NET_CMD_TAKE_PHOTO,                        cmd_ctrl_take_photo},
    {NET_CMD_CHANGE_WIFI_CONNECT_MODE,          cmd_ctrl_set_wifi_connect_mode},
    {NET_CMD_SET_WIFI_BOOT_CONNECT_MODE,        cmd_ctrl_set_wifi_boot_mode},
    {NET_CMD_SET_P2P_WAIT_TIME,                 cmd_ctrl_set_wifi_p2p_wait_time},
    {NET_CMD_SET_DEBUG_MODE,                    cmd_ctrl_set_debug_mode},
    {NET_CMD_SET_TIME_12HOUR,                   cmd_ctrl_set_time_12hour},
    {NET_CMD_SET_WATERMARK_POS,                 cmd_ctrl_set_watermark_pos},
    {NET_CMD_CHANGE_CAM,                        cmd_ctrl_change_rtsp_cam},
    {NET_CMD_SET_REARCAM_QUALITY,               cmd_ctrl_set_rearcam_quality},
    {NET_CMD_SET_REARCAM_BITRATE,               cmd_ctrl_set_rearcam_bitrate},
    {NET_CMD_SET_REARCAM_FRAME,                 cmd_ctrl_set_rearcam_fps},
    {NET_CMD_SET_GSENSOR_FREQ,                  cmd_ctrl_set_gsensor_freq},
    {NET_CMD_SET_REMINDER_MODE,                 cmd_ctrl_set_reminder_mode},
    {NET_CMD_SET_REMINDER_PLAY_VOICE,           cmd_ctrl_set_reminder_voice},
    {NET_CMD_SET_IR_THRESHOLD,                  cmd_ctrl_set_ir_threshold},
    {NET_CMD_SET_ADAS_FCW_SWITCH,               NULL},
    {NET_CMD_SET_ADAS_LDW_SWITCH,               NULL},
    {NET_CMD_SET_ADAS_LDW_TIPSMODE,             NULL},
    {NET_CMD_SET_WIFI_AP_INFO,                  cmd_ctrl_set_wifi_sta_info},
    {NET_CMD_REAR_RECORD_ON_OFF,                cmd_ctrl_set_rearcam_on_off},
    {NET_CMD_CALIBRATE_GSENSOR,                 cmd_ctrl_calibrate_gsensor},
    {NET_CMD_SET_GSENSOR_CALIBRATION_EFFECT,    cmd_ctrl_set_gsensor_calibration_effect},
    {NET_CMD_SET_GSENSOR_DEBUG,                 cmd_ctrl_set_gsensor_debug},
    {NET_CMD_SET_GSENSOR_RANGE,                 cmd_ctrl_set_gsensor_range},
    {NET_CMD_SET_GSENSOR_ODR,                   NULL},
    {NET_CMD_SET_GSENSOR_SWAPPOLARITY,          cmd_ctrl_set_gsensor_swap_polarity},
    {NET_CMD_SET_GSENSOR_POWERBW,               NULL},
    {NET_CMD_SET_IMAGE_ROTATION,                cmd_ctrl_set_image_rotation},
    {NET_CMD_SET_DEVICE_MS_TIME,                cmd_ctrl_set_device_ms_time},
    {NET_CMD_SET_RECORDER_BITRATE,              cmd_ctrl_set_recorder_bitrate},
    {NET_CMD_SET_IR_SAMPLING,                   cmd_ctrl_set_ir_sampling},
    {NET_CMD_LOCK_FILE,                         cmd_ctrl_lock_file},
    {NET_CMD_SET_REAR_PREVIEW_QUALITY,          cmd_ctrl_set_rearcam_preview_quality},
    {NET_CMD_SET_REAR_PREVIEW_BITRATE,          cmd_ctrl_set_rearcam_preview_bitrate},
    {NET_CMD_SET_REAR_PREVIEW_FRAME,            cmd_ctrl_set_rearcam_preview_fps},
    {NET_CMD_SET_GSENSOR_BMI_ODR,               cmd_ctrl_set_accel_odr},
    {NET_CMD_SET_GYRO_ODR,                      cmd_ctrl_set_gyro_odr},
    {NET_CMD_SET_GYRO_RANGE,                    cmd_ctrl_set_gyro_range},
    {NET_CMD_SET_GYRO_CALIBRATION_EFFECT,       cmd_ctrl_set_gyro_calibration_effect},
    {NET_CMD_SET_REAR_ROTATION,                 cmd_ctrl_set_rearcam_rotation},
    {NET_CMD_SET_SHOTDOWN_WAIT_TIME,            cmd_ctrl_set_shutdown_wait_time},
    {NET_CMD_LOCK_TIME_FILE,                    cmd_ctrl_lock_time_file},
    {NET_CMD_RESET_GNSS,                        cmd_ctrl_reset_gnss},
    {NET_CMD_SET_GNSS_DATA_RATE,                cmd_ctrl_set_gnss_datarate},
    {NET_CMD_ENABLE_GNSSID,                     cmd_ctrl_enable_gnssid},
    {NET_CMD_SET_UBX_MGAOFF,                    cmd_ctrl_set_gnss_mgaoff},
    {NET_CMD_SOFT_RESET,                        cmd_ctrl_soft_reset},
};

static void show_ctrl_item(const ctrl_item_s item)
{
    debug_line("ctrl item:");
    debug_line("\tcmd: 0x%X", item.cmd);    
    debug_line("\targ: %d", item.arg);
    debug_line("\tdata: %s", item.data);
}

static int cmd_ctrl_impl(ctrl_item_s item)
{
    int i = 0, rval = -1;

    for (i = 0; i < sizeof(ctrl_handler_items) / sizeof(ctrl_handler_item_s); i++) {
        if (item.cmd == ctrl_handler_items[i].cmd) {
            if (ctrl_handler_items[i].handler) {
                rval = ctrl_handler_items[i].handler(item);
            }
            break;
        }
    }

    return rval;
}

