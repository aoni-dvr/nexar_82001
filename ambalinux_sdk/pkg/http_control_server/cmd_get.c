#include "sqlite3.h"
#include "rtos_api_lib.h"
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "global.h"

typedef int (*get_item_handler_func_s)(int client, const char *path, const char *query_str);
typedef struct _cmd_handler_item_s_ {
    char path[64];
    get_item_handler_func_s handler;
} get_handler_item_s;

static int cmd_get_rtsp_time(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;

    if (obj == NULL || data == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "video0", 1576421625117);
    cJSON_AddNumberToObject(data, "video1", 0);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
typedef enum {
    AMBA_DCF_FILE_TYPE_IMAGE = 0,
    AMBA_DCF_FILE_TYPE_VIDEO,
    AMBA_DCF_FILE_TYPE_AUDIO,
    AMBA_DCF_FILE_TYPE_ALL,
    AMBA_DCF_NUM_FILE_TYPE
} AMBA_DCF_FILE_TYPE_e;

typedef struct {
    unsigned long long datetime;
    unsigned long long filesize;
    unsigned char readonly;
    char filename[128U];
    AMBA_DCF_FILE_TYPE_e obj_type;
} SvcDCF_DashcamRefEmem_t;

static int convert_rtos_path_to_linux_impl(const char *rtos_path, char *linux_path)
{
    unsigned int i = 0;

    if (linux_path == NULL || rtos_path == NULL || strlen(rtos_path) <= 0) {
        return -1;
    }
    if (strncasecmp(rtos_path, "b:\\", strlen("b:\\")) == 0) {
        sprintf(linux_path, "%s", "/data");
        strcat(linux_path, rtos_path + strlen("b:\\"));
    } else if (strncasecmp(rtos_path, "c:\\", strlen("c:\\")) == 0) {
        sprintf(linux_path, "%s", "/mnt/extsd/");
        strcat(linux_path, rtos_path + strlen("c:\\"));
    } else {
        debug_line("invalid path. not start with b:\\ or c:\\\n");
        return -1;
    }

    for (i = 0; i < strlen(linux_path); i++) {
        if (*(linux_path + i) == '\\') {
            *(linux_path + i) = '/';
        }
    }

    return 0;
}

static mmapInfo_s dcfMap = {0};
static SvcDCF_DashcamRefEmem_t *get_dcf_entry(unsigned int *amount)
{
    stream_share_info_s stream_info;
    unsigned char ready = 0;
    static SvcDCF_DashcamRefEmem_t *dcf_entry = NULL;

    if (rtos_api_lib_get_file_list_info(&ready, amount, &stream_info) < 0) {
        debug_line("rtos_api_lib_get_file_list_memory_map fail");
        return NULL;
    }
    if (ready == 0) {
        debug_line("filelist is not ready");
        return NULL;
    }
    debug_line("file amount: video[%d] sos[%d] photo[%d]", amount[0], amount[1], amount[2]);
    if (dcf_entry == NULL) {
        dcf_entry = (SvcDCF_DashcamRefEmem_t *)rtos_api_lib_convert_memory2linux(stream_info, &dcfMap);
    }
    return dcf_entry;
}


static int get_video_thumb_path(const char *video_path, char *thumb_path, int size)
{
    int i = 0, index = -1;

    for (i = strlen(video_path) - 1; i >= 0; i--) {
        if (*(video_path + i) == '/') {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return -1;
    }
    snprintf(thumb_path, size, "%.*s/.thumb/%.*s_net.jpg", index, video_path, strlen(video_path) - index - 1 - 4, video_path + index + 1);

    return 0;
}

static int cmd_get_files(int client, const char *url, const char *query_str)
{
    char tmp[32] = {0};
    int type = -1, order = -1, size = -1;
    SvcDCF_DashcamRefEmem_t *dcf_entry = NULL;
    unsigned int amount[3] = {0};
    unsigned int total_num = 0;
    unsigned int folder_id = 0, file_index = 0, count = 0, timestamp = 0;
    int is_video = 0;
    char linux_path[64] = {0};
    char thumb_path[64] = {0};
    int api_enable = 0;

    debug_line("%s, url=%s, query_str=%s", __func__, url, query_str);
    if (rtos_api_lib_get_api_files_enable(&api_enable) != 0 || api_enable == 0) {
        debug_line("%s, api disabled", __func__);
        return RESPONSE_CODE_NOT_FOUND;
    }

    dcf_entry = get_dcf_entry(amount);
    if (dcf_entry == NULL) {
        return -1;
    }
    total_num = amount[0] + amount[1] + amount[2];
    memset(tmp, 0, sizeof(tmp));
    if (parse_key(query_str, "type=", tmp, sizeof(tmp) - 1) >= 0) {
        type = atoi(tmp);
    }
    memset(tmp, 0, sizeof(tmp));
    if (parse_key(query_str, "order=", tmp, sizeof(tmp) - 1) >= 0) {
        order = atoi(tmp);
    }
    memset(tmp, 0, sizeof(tmp));
    if (parse_key(query_str, "size=", tmp, sizeof(tmp) - 1) >= 0) {
        size = atoi(tmp);
    }
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    cJSON *files = cJSON_CreateArray();
    char *out = NULL;

    if (obj == NULL || data == NULL || files == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "total", total_num);
    cJSON_AddNumberToObject(data, "offset", 0);
    cJSON_AddNumberToObject(data, "size", total_num);

    for (folder_id = 0; folder_id < 3; folder_id++) {
        if (amount[folder_id] <= 0) {
            continue;
        }
        count = 0;
        for (file_index = 1; file_index <= 10000; file_index++) {
            unsigned int index = folder_id * 10001 + file_index;
            if (dcf_entry[index].filename[0] != '\0') {
                int flag = 0;
                count++;
                memset(linux_path, 0, sizeof(linux_path));
                convert_rtos_path_to_linux_impl(dcf_entry[index].filename, linux_path);
                cJSON *file = cJSON_CreateObject();
                cJSON_AddStringToObject(file, "file", linux_path);
                if (strncasecmp(linux_path + strlen(linux_path) - 4, ".mp4", 4) == 0) {
                    is_video = 1;
                } else {
                    is_video = 0;
                }
                if (is_video) {
                    memset(thumb_path, 0, sizeof(thumb_path));
                    get_video_thumb_path(linux_path, thumb_path, sizeof(thumb_path) - 1);
                    cJSON_AddStringToObject(file, "thumb", thumb_path);
                }
                rtos_api_lib_get_timestamp_from_filepath(linux_path, &timestamp);
                cJSON_AddNumberToObject(file, "time", timestamp);
                cJSON_AddNumberToObject(file, "type", is_video ? 1 : 2);
                //front: 0x1, rear: 0x2, wonderful: 0x4, sos: 0x8, lock: 0x10 park: 0x20
                if (dcf_entry[index].readonly == 0xff) {
                    flag = -1;
                } else {
                    if (folder_id == 1) {
                        if (linux_path[strlen(linux_path) - 9] == 'A') {
                            flag |= 0x01;
                        } else {
                            flag |= 0x02;
                        }
                    } else {
                        if (linux_path[strlen(linux_path) - 5] == 'A') {
                            flag |= 0x01;
                        } else {
                            flag |= 0x02;
                        }
                    }
                    if (folder_id == 1) {
                        flag |= 0x08;
                    }
                    if (dcf_entry[index].readonly) {
                        flag |= 0x10;
                    }
                }
                cJSON_AddNumberToObject(file, "flag", flag);
                if (dcf_entry[index].filesize == 0xffffffffffffffffULL) {
                    cJSON_AddNumberToObject(file, "filesize", -1);
                } else {
                    cJSON_AddNumberToObject(file, "filesize", dcf_entry[index].filesize);
                }
                cJSON_AddItemToArray(files, file);
                if (count >= amount[folder_id]) {
                    break;
                }
            }
        }
    }

    cJSON_AddItemToObject(data, "files", files);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_config(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;
    config_s info;
    char mac[32] = {0};

    if (global_data.dev_info.first_access == 0
        && global_data.authed == 0) {
        return RESPONSE_CODE_UNAUTHORIZED;
    }

    if (obj == NULL || data == NULL) {
        return -1;
    }
    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_config(&info);
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "record_quality", info.record_quality);
    cJSON_AddNumberToObject(data, "record_bitrate", info.record_bitrate);
    cJSON_AddNumberToObject(data, "rear_quality", info.rear_quality);
    cJSON_AddNumberToObject(data, "rear_bitrate", info.rear_bitrate);
    cJSON_AddNumberToObject(data, "rear_frame", info.rear_frame);
    cJSON_AddNumberToObject(data, "record_duration", info.record_duration);
    cJSON_AddNumberToObject(data, "photo_quality", info.photo_quality);
    cJSON_AddNumberToObject(data, "white_balance", info.white_balance);
    cJSON_AddNumberToObject(data, "exposure", info.exposure);
    cJSON_AddNumberToObject(data, "boot_record", info.boot_record);
    cJSON_AddNumberToObject(data, "mute", info.mute);
    cJSON_AddNumberToObject(data, "rear_view_mirror", info.rear_view_mirror);
    cJSON_AddNumberToObject(data, "language", info.language);
    cJSON_AddNumberToObject(data, "smart_detect", info.smart_detect);
    cJSON_AddNumberToObject(data, "frontcar_crash_sensitivity", info.frontcar_crash_sensitivity);
    cJSON_AddNumberToObject(data, "impact_sensitivity", info.impact_sensitivity);
    cJSON_AddNumberToObject(data, "motion_detect", info.motion_detect);
    cJSON_AddNumberToObject(data, "watermark", info.watermark);
    cJSON_AddNumberToObject(data, "watermark_pos", info.watermark_pos);
    cJSON_AddNumberToObject(data, "record_switch", info.record_switch);
    cJSON_AddNumberToObject(data, "dev_type", info.dev_type);
    cJSON_AddNumberToObject(data, "curRecordTime", info.curRecordTime);
    cJSON_AddNumberToObject(data, "videoTime", info.videoTime);
    cJSON_AddNumberToObject(data, "shotRecord", info.shotRecord);
    cJSON_AddNumberToObject(data, "shutdown_mode", info.shutdown_mode);
    cJSON_AddNumberToObject(data, "debug_mode", info.debug_mode);
    cJSON_AddNumberToObject(data, "sound_vol", info.sound_vol);
    cJSON_AddNumberToObject(data, "wakeup_val", info.wakeup_val);
    cJSON_AddNumberToObject(data, "support_obd", info.support_obd);
    cJSON_AddNumberToObject(data, "support_backcamera", info.support_backcamera);
    cJSON_AddNumberToObject(data, "support_adas", info.support_adas);
    cJSON_AddNumberToObject(data, "only_gsensor_startup", info.only_gsensor_startup);
    cJSON_AddNumberToObject(data, "support_battery", info.support_battery);
    cJSON_AddNumberToObject(data, "support_gps", info.support_gps);
    cJSON_AddNumberToObject(data, "support_edog", info.support_edog);
    cJSON_AddNumberToObject(data, "support_gui", info.support_gui);
    cJSON_AddNumberToObject(data, "support_wifi", info.support_wifi);
    cJSON_AddNumberToObject(data, "support_mobile", info.support_mobile);
    cJSON_AddNumberToObject(data, "support_wifi_connect", info.support_wifi_connect);
    cJSON_AddNumberToObject(data, "support_low_voltage", info.support_low_voltage);
    cJSON_AddNumberToObject(data, "config_version", info.config_version);
    cJSON_AddNumberToObject(data, "adas_fcw_switch", info.adas_fcw_switch);
    cJSON_AddNumberToObject(data, "adas_fcw_sensitivity", info.adas_fcw_sensitivity);
    cJSON_AddNumberToObject(data, "adas_fcw_speed", info.adas_fcw_speed);
    cJSON_AddNumberToObject(data, "adas_ldw_switch", info.adas_ldw_switch);
    cJSON_AddNumberToObject(data, "adas_ldw_sensitivity", info.adas_ldw_sensitivity);
    cJSON_AddNumberToObject(data, "adas_tips_mode", info.adas_tips_mode);
    cJSON_AddNumberToObject(data, "gsp_data_version", info.gsp_data_version);
    cJSON_AddNumberToObject(data, "gps_tips_mode", info.gps_tips_mode);
    cJSON_AddNumberToObject(data, "startup_voice", info.startup_voice);
    cJSON_AddNumberToObject(data, "net_preview_quality", info.net_preview_quality);
    cJSON_AddNumberToObject(data, "rear_preview_quality", info.rear_preview_quality);
    cJSON_AddNumberToObject(data, "record_delayp", info.record_delayp);
    cJSON_AddNumberToObject(data, "photo_timetakephotos", info.photo_timetakephotos);
    cJSON_AddNumberToObject(data, "photo_automatictakephotos", info.photo_automatictakephotos);
    cJSON_AddNumberToObject(data, "image_rotation", info.image_rotation);
    cJSON_AddNumberToObject(data, "light_freq", info.light_freq);
    cJSON_AddNumberToObject(data, "led_on_off", info.led_on_off);
    cJSON_AddNumberToObject(data, "screen_sleep", info.screen_sleep);
    cJSON_AddNumberToObject(data, "measure_unit", info.measure_unit);
    cJSON_AddNumberToObject(data, "auto_shutdown_time", info.auto_shutdown_time);
    cJSON_AddNumberToObject(data, "record_quality_bitmap", info.record_quality_bitmap);
    cJSON_AddNumberToObject(data, "lang_bitmap", info.lang_bitmap);
    cJSON_AddNumberToObject(data, "voice_lang_bitmap", info.voice_lang_bitmap);
    cJSON_AddNumberToObject(data, "voice_lang", info.voice_lang);
    cJSON_AddNumberToObject(data, "take_photo_mode", info.take_photo_mode);
    cJSON_AddNumberToObject(data, "slow_record_mode", info.slow_record_mode);
    cJSON_AddNumberToObject(data, "motion_take_photo_mode", info.motion_take_photo_mode);
    cJSON_AddNumberToObject(data, "wifi_connect_mode", info.wifi_connect_mode);
    cJSON_AddNumberToObject(data, "wifi_boot_connect_mode", info.wifi_boot_connect_mode);
    cJSON_AddNumberToObject(data, "p2p_wait_time", info.p2p_wait_time);
    cJSON_AddNumberToObject(data, "switch_keytone", info.switch_keytone);
    cJSON_AddNumberToObject(data, "switch_takephoto_tone", info.switch_takephoto_tone);
    cJSON_AddNumberToObject(data, "wifi_type", info.wifi_type);
    cJSON_AddNumberToObject(data, "obd_low_voltage", info.obd_low_voltage);
    cJSON_AddNumberToObject(data, "obd_winter_low_voltage", info.obd_winter_low_voltage);
    cJSON_AddNumberToObject(data, "battery_power_time", info.battery_power_time);
    cJSON_AddNumberToObject(data, "wifi_channel", info.wifi_channel);
    cJSON_AddNumberToObject(data, "net_preview_bitrate", info.net_preview_bitrate);
    cJSON_AddNumberToObject(data, "net_preview_frame", info.net_preview_frame);
    cJSON_AddNumberToObject(data, "park_timelapse", info.park_timelapse);
    cJSON_AddNumberToObject(data, "time_format", info.time_format);
    cJSON_AddNumberToObject(data, "ir_threshold", info.ir_threshold);
    cJSON_AddNumberToObject(data, "ir_sampling", info.ir_sampling);
    cJSON_AddNumberToObject(data, "shutdown_wait_time", info.shutdown_wait_time);
    cJSON_AddStringToObject(data, "firmware_version", info.firmware_version);
    cJSON_AddStringToObject(data, "buildTime", info.buildTime);
    cJSON_AddStringToObject(data, "manufacturer", info.manufacturer);
    cJSON_AddStringToObject(data, "otaVersion", info.otaVersion);
    cJSON_AddStringToObject(data, "product_sn", info.product_sn);
    memset(mac, 0, sizeof(mac));
    rtos_api_lib_get_netcard_info("wlan0", NULL, mac);
    cJSON_AddStringToObject(data, "product_wifimac", mac);
    cJSON_AddStringToObject(data, "product_cpuid", info.product_cpuid);
    cJSON_AddStringToObject(data, "product_vsn", info.product_vsn);
    cJSON_AddStringToObject(data, "obd_version", info.obd_version);
    cJSON_AddStringToObject(data, "impact_value", info.impact_value);
    cJSON_AddStringToObject(data, "token", global_data.dev_info.token);
    cJSON_AddStringToObject(data, "wifiPsd", info.wifiPsd);
    cJSON_AddStringToObject(data, "wifi_ssid", info.wifi_ssid);
    cJSON_AddStringToObject(data, "wifi_ap_ssid", info.wifi_ap_ssid);
    cJSON_AddStringToObject(data, "wifi_ap_passwd", info.wifi_ap_passwd);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        if (global_data.dev_info.first_access) {
            rtos_api_lib_set_dev_info_first_accessed();
            global_data.dev_info.first_access = 0;
        }
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_state(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;
    state_s info;

    if (obj == NULL || data == NULL) {
        return -1;
    }
    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_state(&info);
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "record", info.record);
    cJSON_AddNumberToObject(data, "mute", info.mute);
    cJSON_AddNumberToObject(data, "obd", info.obd);
    cJSON_AddNumberToObject(data, "backcamera", info.backcamera);
    cJSON_AddNumberToObject(data, "battery", info.battery);
    cJSON_AddNumberToObject(data, "uptime", info.uptime);
    cJSON_AddNumberToObject(data, "debug_wakeup_val", info.debug_wakeup_val);
    cJSON_AddNumberToObject(data, "timenow", info.timenow);//utc time
    cJSON_AddNumberToObject(data, "timezone", info.timezone);
    cJSON_AddNumberToObject(data, "rtctime", info.rtctime);//local time
    cJSON_AddNumberToObject(data, "battery_voltage", info.battery_voltage);
    cJSON_AddNumberToObject(data, "battery_charging", info.battery_charging);
    cJSON_AddNumberToObject(data, "battery_online", info.battery_online);
    cJSON_AddNumberToObject(data, "rear_record_disable", info.rear_record_disable);
    cJSON_AddNumberToObject(data, "acc_state", info.acc_state);
    cJSON_AddNumberToObject(data, "usb_state", info.usb_state);
    cJSON_AddNumberToObject(data, "tf_state", info.tf_state);
    cJSON_AddNumberToObject(data, "tf_slow_times", info.tf_slow_times);
    cJSON_AddNumberToObject(data, "startup_index", info.startup_index);
    cJSON_AddNumberToObject(data, "valid_data_index", info.valid_data_index);
    cJSON_AddNumberToObject(data, "obd_voltage", info.obd_voltage);
    cJSON_AddNumberToObject(data, "obd_state", info.obd_state);
    cJSON_AddNumberToObject(data, "obd_rt_time", info.obd_rt_time);
    cJSON_AddNumberToObject(data, "gps", info.gps);
    cJSON_AddNumberToObject(data, "gps_time", info.gps_time);
    cJSON_AddNumberToObject(data, "gps_lng", info.gps_lng);
    cJSON_AddNumberToObject(data, "gps_lat", info.gps_lat);
    cJSON_AddNumberToObject(data, "gps_speed", info.gps_speed);
    cJSON_AddNumberToObject(data, "gps_degree", info.gps_degree);
    cJSON_AddNumberToObject(data, "gps_gga_time", info.gps_gga_time);
    cJSON_AddNumberToObject(data, "gps_gga_state", info.gps_gga_state);
    cJSON_AddNumberToObject(data, "gps_satellite", info.gps_satellite);
    cJSON_AddNumberToObject(data, "gps_msl", info.gps_msl);
    cJSON_AddNumberToObject(data, "gps_geoid", info.gps_geoid);
    cJSON_AddNumberToObject(data, "gps_hdop", info.gps_hdop);
    cJSON_AddNumberToObject(data, "ir_state", info.ir_state);
    cJSON_AddNumberToObject(data, "Ir_switch_time", info.ir_switch_time);
    cJSON_AddNumberToObject(data, "gsensor_temp", info.gsensor_temp);
    cJSON_AddStringToObject(data, "tripkey", info.tripkey);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_tfcard(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;
    tfcard_s info;

    if (obj == NULL || data == NULL) {
        return -1;
    }
    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_tfcard(&info);
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "state", info.state);
    cJSON_AddNumberToObject(data, "remain", info.remain);
    cJSON_AddNumberToObject(data, "total", info.total);
    cJSON_AddStringToObject(data, "oemid", info.oemid);
    cJSON_AddStringToObject(data, "cid", info.cid);
    cJSON_AddStringToObject(data, "csd", info.csd);
    cJSON_AddStringToObject(data, "fwrev", info.fwrev);
    cJSON_AddStringToObject(data, "hwrev", info.hwrev);
    cJSON_AddStringToObject(data, "serial", info.serial);
    cJSON_AddStringToObject(data, "scr", info.scr);
    cJSON_AddStringToObject(data, "name", info.name);
    cJSON_AddStringToObject(data, "manfid", info.manfid);
    cJSON_AddStringToObject(data, "date", info.date);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_gnss_info(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;
    gnss_s info;

    if (obj == NULL || data == NULL) {
        return -1;
    }
    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_gnss(&info);
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    cJSON_AddNumberToObject(data, "uart_bitrate", info.uart_bitrate);
    cJSON_AddNumberToObject(data, "support_gnssid", info.support_gnssid);
    cJSON_AddNumberToObject(data, "enable_gnssid", info.enable_gnssid);
    cJSON_AddNumberToObject(data, "aop_config", info.aop_config);
    cJSON_AddNumberToObject(data, "mga_type", info.mga_type);
    cJSON_AddNumberToObject(data, "mga_time_s", info.mga_time_s);
    cJSON_AddNumberToObject(data, "mga_time_e", info.mga_time_e);
    cJSON_AddNumberToObject(data, "data_rate", info.data_rate);
    cJSON_AddNumberToObject(data, "nmea_version", info.nmea_version);
    cJSON_AddStringToObject(data, "gnss_model", info.gnss_model);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_info(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();
    char *out = NULL;
    info_s info;
    char ip[32] = {0};
    char tmp[32] = {0};

    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_info(&info);
    cJSON_AddNumberToObject(obj, "code", 0);
    cJSON_AddStringToObject(obj, "msg", get_code_message(0));
    // add data
    snprintf(tmp, sizeof(tmp) - 1, "%d.%d", info.protocol_ver[1], info.protocol_ver[0]);
    cJSON_AddStringToObject(data, "protocol_ver", tmp);
    cJSON_AddStringToObject(data, "vendor", info.vendor);
    cJSON_AddStringToObject(data, "model", info.model);
    cJSON_AddStringToObject(data, "sn", info.sn);
    cJSON_AddStringToObject(data, "wifi_ssid", info.wifi_ssid);
    cJSON_AddNumberToObject(data, "wifi_status", info.wifi_status);
    cJSON_AddNumberToObject(data, "ibeacon_major", info.ibeacon_major);
    cJSON_AddNumberToObject(data, "ibeacon_minor", info.ibeacon_minor);
    memset(ip, 0, sizeof(ip));
    rtos_api_lib_get_netcard_info("wlan0", ip, NULL);
    cJSON_AddStringToObject(data, "ip_addr", ip);
    cJSON_AddStringToObject(data, "capabilities", info.capabilities);
    cJSON_AddItemToObject(obj, "data", data);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_file_db(int client, const char *url, const char *query_str)
{
    int ret = 0;
    unsigned int folder_id = 0, file_index = 0, index = 0, count = 0;
    sqlite3 *db = NULL;
    char *errmsg = NULL;
    char sql[256] = {0};
    char linux_path[64] = {0};
    SvcDCF_DashcamRefEmem_t *dcf_entry = NULL;
    unsigned int amount[3] = {0};
#define DB_PATH "/mnt/extsd/.file.db"

    remove(DB_PATH);
    dcf_entry = get_dcf_entry(amount);
    if (dcf_entry == NULL) {
        return RESPONSE_CODE_DATABASE_NOT_READY;
    }
    ret = sqlite3_open(DB_PATH, &db);
    if (ret != SQLITE_OK) {
        debug_line("sqlite open fail: %s", sqlite3_errmsg(db));
        return -1;
    }
    ret = sqlite3_exec(db, "begin;", 0, 0, &errmsg);
    if (ret != SQLITE_OK) {
        debug_line("sqlite begin fail: %s", errmsg);
        goto end;
    }
    //id integer primary key autoincrement
    ret = sqlite3_exec(db, "create table CdrFile(file varchar(48), timestamps integer, size integer, type integer);", NULL, NULL, &errmsg);
    if (ret != SQLITE_OK) {
        debug_line("sqlite create table fail: %s", errmsg);
        goto end;
    }
    for (folder_id = 0; folder_id < 3; folder_id++) {
        if (amount[folder_id] <= 0) {
            continue;
        }
        count = 0;
        for (file_index = 1; file_index <= 10000; file_index++) {
            index = folder_id * 10001 + file_index;
            if (dcf_entry[index].filename[0] != '\0') {
                count++;
                memset(linux_path, 0, sizeof(linux_path));
                convert_rtos_path_to_linux_impl(dcf_entry[index].filename, linux_path);
                memset(sql, 0, sizeof(sql));
                snprintf(sql, sizeof(sql) - 1, "INSERT INTO CdrFile(file, timestamps, size, type) VALUES('%s', %lld, %lld, %d);", linux_path,
                                                                                                                                dcf_entry[index].datetime,
                                                                                                                                dcf_entry[index].filesize,
                                                                                                                                folder_id);
                ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
                if (ret != SQLITE_OK) {
                    debug_line("sqlite insert data fail: %s", errmsg);
                    break;
                }
                if (count >= amount[folder_id]) {
                    break;
                }
            }
        }
        if (ret != SQLITE_OK) {
            break;
        }
    }
    ret = sqlite3_exec(db, "commit;", 0, 0, &errmsg);
    if (ret != SQLITE_OK) {
        debug_line("sqlite commit fail: %s", errmsg);
        goto end;
    }
    sqlite3_close(db);
    if (errmsg != NULL) {
        free(errmsg);
        errmsg = NULL;
    }

    {
        struct stat fs;
        int fd = 0;
        fd = open(DB_PATH, O_RDONLY);
        if (fd < 0) {
            debug_line("open %s fail", DB_PATH);
            goto end;
        }
        fstat(fd, &fs);
        send_header(client, "application/octet-stream", fs.st_size);
        sendfile(client, fd, NULL, fs.st_size);
        close(fd);
    }

    return 0;
end:
    sqlite3_close(db);
    if (errmsg != NULL) {
        free(errmsg);
        errmsg = NULL;
    }

    return -1;
}

static get_handler_item_s get_handler_items[] = {
    {"/api/rtsp_time",          cmd_get_rtsp_time},
    {"/api/files",              cmd_get_files},
    {"/api/config",             cmd_get_config},
    {"/api/state",              cmd_get_state},
    {"/api/tfcard",             cmd_get_tfcard},
    {"/api/gnss_info",          cmd_get_gnss_info},
    {"/api/info",               cmd_get_info},
    {"/data/sunxi.db",          cmd_get_file_db},
};

int cmd_get_release(void)
{
    rtos_api_lib_munmap(&dcfMap);
}

