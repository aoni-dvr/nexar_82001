#include "rtos_api_lib.h"
#include "debug.h"
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_R_LinuxApiService.h"
#include "AmbaIPC_RpcProg_R_LinuxApiService2.h"
#include "AmbaIPC_RpcProg_LU_LinuxApiService.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "rtos_api_event.c"
#include "lock.h"

#define DEFAULT_AMBAIPC_TIMEOUT 2000

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

typedef struct _response_path_s_ {
    char path1[128];
    char path2[128];
} response_path_s;

typedef struct _burn_info_s_ {
    unsigned char wifi_mac[6];
    unsigned char bt_mac[6];
} burn_info_s;

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
    LINUX_API_CMD_NUM
} linux_api_cmd_e;

static int rtos_api_lib_send_cmd(linux_api_cmd_s cmd, linux_api_response_s *response, unsigned int timeout)
{
    int res = 0;
    int host = 1;
    CLIENT_ID_t clnt = 0;
    int prog_id = AMBA_RPC_PROG_R_LINUX_API_SERVICE_PROG_ID;
    int ver = AMBA_RPC_PROG_R_LINUX_API_SERVICE_VER;
    int ipc = AMBA_RPC_PROG_R_LINUX_API_SERVICE_IPC;

    if (cmd.msg_id == LINUX_API_CMD_GET_IMU_FIFO_DATA) {
        prog_id = AMBA_RPC_PROG_R_LINUX_API_SERVICE2_PROG_ID;
        ver = AMBA_RPC_PROG_R_LINUX_API_SERVICE2_VER;
        ipc = AMBA_RPC_PROG_R_LINUX_API_SERVICE2_IPC;
    }
    pthread_mutex_lock(&(mutex_info->lock));
    clnt = ambaipc_clnt_create(host, prog_id, ver);
    if (!clnt) {
        printf("%s, ambaipc_clnt_create fail\n", __func__);
        pthread_mutex_unlock(&(mutex_info->lock));
        return -1;
    }
    res = ambaipc_clnt_call(clnt,
                    ipc,
                    (void *)&cmd,
                    sizeof(linux_api_cmd_s) + 1,
                    response,
                    sizeof(linux_api_response_s),
                    timeout);
    ambaipc_clnt_destroy(clnt);
    pthread_mutex_unlock(&(mutex_info->lock));
    if (res) {
        printf("%s, ambaipc_clnt_call failed: %d\n", __func__, res);
    }
    return res;
}

int rtos_api_lib_reboot(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_REBOOT;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_format_disk(unsigned int sd_disk)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_FORMAT_SD;
    if (sd_disk) {
        cmd.item.sd_disk = 1;
    } else {
        cmd.item.sd_disk = 0;
    }
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_default_setting(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_DEFAULT_SETTING;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_play_sound_start(const char *path)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_PLAY_SOUND_START;
    memcpy(cmd.item.arg_obj.data, path, strlen(path));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_play_sound_stop(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_PLAY_SOUND_STOP;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_is_sound_playing(unsigned char *is_playing, unsigned int *remaining_ms)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_IS_SOUND_PLAYING;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (is_playing != NULL) {
            *is_playing = response.item.sound_play_status.is_playing;
        }
        if (remaining_ms != NULL) {
            *remaining_ms = response.item.sound_play_status.remaining_ms;
        }
    }

    return response.rval;
}

int rtos_api_lib_get_audio_volume(unsigned char *volume)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (volume == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_AUDIO_VOLUME;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    *volume = response.item.audio_volume;

    return response.rval;
}

int rtos_api_lib_set_audio_volume(unsigned char volume)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_AUDIO_VOLUME;
    cmd.item.audio_volume = volume;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_start_event_record(CAMERA_CHANNEL_e channel, unsigned int pre_seconds, unsigned int length, char *path_internal, char *path_external)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_START_EVENT_RECORD;
    cmd.item.event_record_param.channel = channel;
    cmd.item.event_record_param.pre_seconds = pre_seconds;
    cmd.item.event_record_param.length = length;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (path_internal != NULL) {
            memcpy(path_internal, response.item.filepath.path1, strlen(response.item.filepath.path1));
        }
        if (path_external != NULL) {
            memcpy(path_external, response.item.filepath.path2, strlen(response.item.filepath.path2));
        }
    }

    return response.rval;
}

int rtos_api_lib_stop_event_record(CAMERA_CHANNEL_e channel)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;
    char rtos_path[128] = {0};

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_STOP_EVENT_RECORD;
    cmd.item.event_record_param.channel = channel;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_is_event_recording(CAMERA_CHANNEL_e channel, char **path_to_video)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (channel != CAMERA_CHANNEL_INTERNAL
        && channel != CAMERA_CHANNEL_EXTERNAL) {
        return -2;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_IS_EVENT_RECORDING;
    cmd.item.event_record_param.channel = channel;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval < 0) {
        return -1;
    }
    if (response.rval && path_to_video) {
        char *linux_path = response.item.filepath.path1;
        if (*path_to_video == NULL) {
            int len = strlen(linux_path) + 1;
            *path_to_video = malloc(len);
            if (*path_to_video == NULL) {
                return -2;
            }
            memset(*path_to_video, 0, len);
        }
        memcpy(*path_to_video, linux_path, strlen(linux_path));
    }

    return response.rval ? 1 : 0;
}

int rtos_api_lib_capture_piv(CAMERA_CHANNEL_e channel, char *path_internal, char *path_external)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CAPTURE_PIV;
    cmd.item.piv_param.channel = channel;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (path_internal != NULL) {
            memcpy(path_internal, response.item.filepath.path1, strlen(response.item.filepath.path1));
        }
        if (path_external != NULL) {
            memcpy(path_external, response.item.filepath.path2, strlen(response.item.filepath.path2));
        }
    }

    return response.rval;
}

int rtos_api_lib_capture_audio_start(const char *path)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CAPTURE_AUDIO_START;
    memcpy(cmd.item.arg_obj.data, path, strlen(path));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_capture_audio_stop(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CAPTURE_AUDIO_STOP;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_hw_sn(char *sn)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (sn == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_HW_SN;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        memcpy(sn, response.item.sn, strlen((char *)response.item.sn));
    }

    return response.rval;
}

int rtos_api_lib_get_fwupdate_flag(unsigned char *flag)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (flag == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_FWUPDATE_FLAG;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *flag = response.item.fwupdate_flag;
    }

    return response.rval;
}

int rtos_api_lib_set_fwupdate_flag(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_FWUPDATE_FLAG;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_clear_fwupdate_flag(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CLEAR_FWUPDATE_FLAG;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_check_external_camera_connected(int *connected)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (connected == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CHECK_EXTERNAL_CAMERA_CONNECTED;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *connected = response.item.external_camera_connected;
    }

    return response.rval;
}

int rtos_api_lib_get_gpio_level(int gpio, int *level)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (level == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_GPIO_LEVEL;
    cmd.item.gpio_id = gpio;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *level = response.item.gpio_level;
    }

    return response.rval;
}

int rtos_api_lib_dump_rtos_log(const char *path)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_DUMP_RTOS_LOG;
    memcpy(cmd.item.arg_obj.data, path, strlen(path));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_file_list_info(unsigned char *is_ready, unsigned int *amount, stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_FILE_LIST_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    *is_ready = response.item.filelist_info.ready;
    if (response.item.filelist_info.ready) {
        memcpy(amount, response.item.filelist_info.file_amount, sizeof(response.item.filelist_info.file_amount));
    }
    if (stream_info != NULL) {
        stream_info->share_addr = response.item.filelist_info.share_addr;
        stream_info->phy_addr = response.item.filelist_info.phy_addr;
        stream_info->share_size = response.item.filelist_info.share_size;
    }

    return 0;
}

int rtos_api_lib_get_config(config_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_CONFIG;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    memcpy(info, &response.item.config, sizeof(config_s));

    return 0;
}

int rtos_api_lib_get_state(state_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_STATE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    memcpy(info, &response.item.state, sizeof(state_s));
    //set time
    time_t t = time(NULL);
    info->timenow = t;
    //info->timezone = mktime(localtime(&t)) - mktime(gmtime(&t));
    info->rtctime = t + info->timezone;

    return 0;
}

int rtos_api_lib_get_tfcard(tfcard_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_TFCARD;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    memcpy(info, &response.item.tfcard, sizeof(tfcard_s));

    return 0;
}

int rtos_api_lib_get_gnss(gnss_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_GNSS;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    memcpy(info, &response.item.gnss, sizeof(gnss_s));

    return 0;
}

int rtos_api_lib_get_info(info_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    memcpy(info, &response.item.info, sizeof(info_s));

    return 0;
}

int rtos_api_lib_get_netcard_info(const char *iface, char *ip, char *mac)
{
    int i = 0;
    int sockfd = 0;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512] = {0};

    if (iface == NULL) {
        return -1;
    }
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    if (ioctl(sockfd, SIOCGIFCONF, &ifconf) < 0) {
    }
    ifreq = (struct ifreq *)buf;
    for (i = (ifconf.ifc_len / sizeof(struct ifreq)); i >= 0; i--) {
        if (strncmp(ifreq->ifr_name, "p2p-wlan", strlen("p2p-wlan")) == 0) {
            if (ip != NULL) {
                strcpy(ip, inet_ntoa(((struct sockaddr_in *)&(ifreq->ifr_addr))->sin_addr));
            }
            if (mac != NULL) {
                if (ioctl(sockfd, SIOCGIFHWADDR, ifreq) >= 0) {
                    for (int j = 0; j < 6; j++) {
                        sprintf(mac + 3 * j, "%02x:", ifreq->ifr_hwaddr.sa_data[j]);
                    }
                }
                mac[strlen(mac) - 1] = '\0';
            }
            close(sockfd);
            return 0;
        }
        ifreq++;
    }

    ifreq = (struct ifreq *)buf;
    for (i = (ifconf.ifc_len / sizeof(struct ifreq)); i >= 0; i--) {
        if (strcmp(ifreq->ifr_name, iface) == 0) {
            if (ip != NULL) {
                strcpy(ip, inet_ntoa(((struct sockaddr_in *)&(ifreq->ifr_addr))->sin_addr));
            }
            if (mac != NULL) {
                if (ioctl(sockfd, SIOCGIFHWADDR, ifreq) >= 0) {
                    for (int j = 0; j < 6; j++) {
                        sprintf(mac + 3 * j, "%02x:", ifreq->ifr_hwaddr.sa_data[j]);
                    }
                }
                mac[strlen(mac) - 1] = '\0';
            }
            close(sockfd);
            return 0;
        }
        ifreq++;
    }
    close(sockfd);
    return -1;
}

int rtos_api_lib_set_ev(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_EV_BIAS;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wb(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WB;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_start_record(int channel)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_START_STOP_RECORD;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = 1;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_stop_record(int channel)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_START_STOP_RECORD;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_delete_file(const char *path)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_DELETE_FILE;
    memcpy(cmd.item.arg_obj.data, path, strlen(path));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_mute(int mute)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_MUTE;
    cmd.item.arg_obj.arg = mute ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_watermark(int mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WATERMARK;
    cmd.item.arg_obj.arg = mode;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_watermark_pos(int mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WATERMARK_POS;
    cmd.item.arg_obj.arg = mode;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_battery_power_time(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_BATTERY_POWER_TIME;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_shutdown_wait_time(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_SHUTDOWN_WAIT_TIME;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_record_duration(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RECORD_DURATION;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_record_quality(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RECORD_QUALITY;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_record_fps(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RECORD_FPS;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_record_bitrate(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RECORD_BITRATE;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_record_quality_and_bitrate(int channel, int quality, int bitrate)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RECORD_QUALITY_AND_BITRATE;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = quality;
    cmd.item.arg_obj.arg2 = bitrate;

    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_preview_quality(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PREVIEW_QUALITY;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_preview_fps(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PREVIEW_FPS;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_preview_bitrate(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PREVIEW_BITRATE;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_preview_quality_and_bitrate(int channel, int quality, int bitrate)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PREVIEW_QUALITY_AND_BITRATE;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = quality;
    cmd.item.arg_obj.arg2 = bitrate;

    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_image_rotation(int channel, int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_IMAGE_ROTATION;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_time_format(int is_24_mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_TIME_FORMAT;
    cmd.item.arg_obj.arg = is_24_mode ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_time_zone(int zone)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_TIME_ZONE;
    cmd.item.arg_obj.arg = zone;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_time_zone(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_TIME_ZONE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.item.timezone;
}

int rtos_api_lib_set_debug_mode(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_DEBUG_MODE;
    cmd.item.arg_obj.arg = enable ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_ir_threshold(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_IR_THRESHOLD;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_ir_sampling(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_IR_SAMPLING;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_rtsp_cam(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RTSP_CAM;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_lock_file(const char *path, int lock)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (path == NULL || strlen(path) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_LOCK_FILE;
    memcpy(cmd.item.arg_obj.data, path, strlen(path));
    cmd.item.arg_obj.arg = lock;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_lock_file_time_range(unsigned int start_time, unsigned int end_time, int lock)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_LOCK_FILE_TIME_RANGE;
    snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%d", start_time);
    snprintf(cmd.item.arg_obj.data2, sizeof(cmd.item.arg_obj.data2) - 1, "%d", end_time);
    cmd.item.arg_obj.arg = lock;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_boot_mode(int mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_BOOT_MODE;
    cmd.item.arg_obj.arg = mode;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_connect_mode(int mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_CONNECT_MODE;
    cmd.item.arg_obj.arg = mode;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_channel(int channel)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_CHANNEL;
    cmd.item.arg_obj.arg = channel;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_type(int is_5g, int channel)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_TYPE;
    cmd.item.arg_obj.arg = is_5g;
    if (channel > 0) {
        snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%d", channel);
    }
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_ap_password(const char *password)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_AP_PASSWORD;
    snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%s", password);
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_sta_info(const char *ssid, const char *password)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_STA_INFO;
    snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%s", ssid);
    snprintf(cmd.item.arg_obj.data2, sizeof(cmd.item.arg_obj.data2) - 1, "%s", password);
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_wifi_p2p_wait_time(int seconds)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_WIFI_P2P_WAIT_TIME;
    cmd.item.arg_obj.arg = seconds;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_bt_imu_stream_info(stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (stream_info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_BT_IMU_STREAM_SHARE_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    stream_info->share_addr = response.item.stream_share_info.share_addr;
    stream_info->phy_addr = response.item.stream_share_info.phy_addr;
    stream_info->share_size = response.item.stream_share_info.share_size;

    return 0;
}

int rtos_api_lib_get_bt_gnss_stream_info(stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (stream_info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_BT_GNSS_STREAM_SHARE_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    stream_info->share_addr = response.item.stream_share_info.share_addr;
    stream_info->phy_addr = response.item.stream_share_info.phy_addr;
    stream_info->share_size = response.item.stream_share_info.share_size;

    return 0;
}

int rtos_api_lib_set_bt_imu_stream_enable(unsigned char enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_BT_IMU_STREAM_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_bt_gnss_stream_enable(unsigned char enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_BT_GNSS_STREAM_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

static unsigned char *do_mmap(unsigned long long phy_addr, unsigned int size)
{
    unsigned char *map_base = NULL;
    int fd = 0;

    fd = open("/dev/ppm", O_RDWR | O_SYNC);
    if (fd == -1) {
        return NULL;
    }
    map_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phy_addr);
    if (map_base == MAP_FAILED) {
        close(fd);
        return NULL;
    }
    close(fd);

    return map_base;
}

unsigned long long rtos_api_lib_convert_memory2linux(stream_share_info_s stream_info, mmapInfo_s *map_info)
{
    char *ptr = NULL;
    unsigned long long b_addr = stream_info.phy_addr;
    unsigned int size = stream_info.share_size;

    //do 4K alignment
    if ((size & 0x0fff) != 0) { //must be 4K aligned
        size = (size + 0x1000) & (~0x0fff);
    }
    if ((b_addr & 0x0fff) != 0) { //must be 4K align
        size += 0x1000;
        b_addr = b_addr & (~0x0fff); //aligned to 4k for mmap;
    }
    map_info->base = do_mmap(b_addr, size);
    map_info->offset = (unsigned long long)(uintptr_t)map_info->base - b_addr;
    map_info->offset -= (unsigned long long)stream_info.share_addr - (unsigned long long)stream_info.phy_addr;
    map_info->size = size;

    ptr = (char *)(uintptr_t)stream_info.share_addr;
    ptr += map_info->offset;
    return (unsigned long long)(uintptr_t)ptr;
}

int rtos_api_lib_munmap(mmapInfo_s *finfo)
{
    if (finfo->base != NULL) {
        if (munmap(finfo->base, finfo->size) != 0) {
            perror("Fail to do munmap:");
            return -1;
        }
        finfo->base = NULL;
        finfo->size = 0;
        finfo->offset = 0;
    }

    return 0;
}

int rtos_api_lib_do_gsensor_calibrate(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_DO_GSENSOR_CALIBRATE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return 0;
}

int rtos_api_lib_set_gsensor_calibration_effect(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GSENSOR_CALIBRATION_EFFECT;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return 0;
}

int rtos_api_lib_set_gsensor_debug(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GSENSOR_DEBUG;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return 0;
}

int rtos_api_lib_set_gsensor_wakeup_value(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GSENSOR_WAKEUP_VALUE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return 0;
}

int rtos_api_lib_set_gsensor_impact_value(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GSENSOR_IMPACT_VALUE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return 0;
}

int rtos_api_lib_set_gsensor_freq(unsigned char value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GSENSOR_FREQ;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_accel_odr(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_ACCEL_ODR;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_accel_range(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_ACCEL_RANGE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gyro_odr(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GYRO_ODR;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gyro_range(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GYRO_RANGE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gyro_calibration_effect(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GYRO_CALIBRATION_EFFECT;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_imu_swap_polarity(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GYRO_CALIBRATION_EFFECT;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_pairing_reminder_mode(int mode, const char *voice_path)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PAIRING_REMINDER_MODE;
    cmd.item.arg_obj.arg = mode;
    snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%s", voice_path);
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_pairing_reminder_voice(int play_count, int delay_ms)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PAIRING_REMINDER_VOICE;
    cmd.item.arg_obj.arg = play_count;
    snprintf(cmd.item.arg_obj.data, sizeof(cmd.item.arg_obj.data) - 1, "%d", delay_ms);
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_reset_gnss(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_RESET_GNSS;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gnss_datarate(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSS_DATARATE;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gnssid(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSSID;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_gnss_mgaoff(int value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSS_MGAOFF;
    cmd.item.arg_obj.arg = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

#define DEV_INFO_PATH "/pref/devinfo"
int rtos_api_lib_get_dev_info(dev_info_s *dev_info)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    fp = fopen(DEV_INFO_PATH, "r");
    if (fp == NULL) {
        return -1;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }
        if (strncmp(line, "token=", strlen("token=")) == 0) {
            memcpy(dev_info->token, line + strlen("token="), strlen(line) - strlen("token="));
        } else if (strncmp(line, "first_access=", strlen("first_access=")) == 0) {
            dev_info->first_access = atoi(line + strlen("first_access=")) ? 1 : 0;
        }
    }
    if (line) {
        free(line);
    }
    fclose(fp);

    return 0;
}

int rtos_api_lib_set_dev_info_first_accessed(void)
{
    system("sed -i -e 's|\\(.*\\)first_access=\\(.*\\)|first_access=0|g' "DEV_INFO_PATH);

    return 0;
}

int rtos_api_lib_set_led_reg_value(unsigned char reg, unsigned char value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LED_REG;
    cmd.item.reg_value.reg = reg;
    cmd.item.reg_value.value = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_led_reg_value(unsigned char reg, unsigned char *value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (value == NULL) {
        return  -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_LED_REG;
    cmd.item.reg_value.reg = reg;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *value = response.item.reg_value;
    }

    return response.rval;
}

int rtos_api_lib_soft_reset(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SOFT_RESET;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_factory_reset(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;
    
    system("/usr/local/share/script/ubifs_unmount.sh");
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_FACTORY_RESET;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_usb_line_connected(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_USB_LINE_CONNECTED;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_button_param(unsigned int x, unsigned int y, unsigned int z)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_BUTTON_PARAM;
    cmd.item.button_param.x = x;
    cmd.item.button_param.y = y;
    cmd.item.button_param.z = z;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_addon_type(ADDON_TYPE_e *left_type, ADDON_TYPE_e *right_type)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_ADDON_TYPE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (left_type != NULL) {
            *left_type = response.item.addon_type.left_value;
        }
        if (right_type != NULL) {
            *right_type = response.item.addon_type.right_value;
        }
    }

    return response.rval;
}

int rtos_api_lib_get_wakeup_source(WAKEUP_SOURCE_e *source, char *name)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_WAKEUP_SOURCE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (source != NULL) {
            *source = response.item.wakeup_source.value;
        }
        if (name != NULL) {
            memcpy(name, response.item.wakeup_source.name, strlen(response.item.wakeup_source.name));
        }
    }

    return response.rval;
}

int rtos_api_lib_set_mcu_timer_wakeup_time(int minutes)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_MCU_WAKEUP_TIME;
    cmd.item.mcu_wakeup_time = minutes;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_mcu_timer_wakeup_time(int *minutes)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (minutes == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_MCU_WAKEUP_TIME;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *minutes = response.item.mcu_wakeup_time;
    }

    return response.rval;
}

int rtos_api_lib_set_power_off_mode(POWER_OFF_MODE_e mode)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_POWER_OFF_MODE;
    cmd.item.power_off_mode = mode;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_stream_share_info(stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (stream_info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_STREAM_SHARE_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    stream_info->share_addr = response.item.stream_share_info.share_addr;
    stream_info->phy_addr = response.item.stream_share_info.phy_addr;
    stream_info->share_size = response.item.stream_share_info.share_size;

    return 0;
}

int rtos_api_lib_set_stream_share_enable(int index, int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_STREAM_SHARE_ENABLE;
    cmd.item.stream_param.index = index;
    cmd.item.stream_param.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_timestamp_from_filepath(const char *filepath, unsigned int *timestamp)
{
    int i = 0, index = -1;
    struct tm tm = {0};
    char tmp[16] = {0};

    for (i = strlen(filepath) - 1; i >= 0; i--) {
        if (*(filepath + i) == '/') {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return -1;
    }
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 1, 4);
    tm.tm_year = atoi(tmp) - 1900;

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 5, 2);
    tm.tm_mon = atoi(tmp) - 1;

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 7, 2);
    tm.tm_mday = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 10, 2);
    tm.tm_hour = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 12, 2);
    tm.tm_min = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, filepath + index + 14, 2);
    tm.tm_sec = atoi(tmp);
    tm.tm_isdst = 0;

    *timestamp = mktime(&tm);

    return 0;
}

int rtos_api_lib_get_burn_info(unsigned char *wifi_mac, unsigned char *bt_mac)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_FACTORY_BURN_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    if (wifi_mac != NULL) {
        memcpy(wifi_mac, response.item.burn_info.wifi_mac, 6);
    }
    if (bt_mac != NULL) {
        memcpy(bt_mac, response.item.burn_info.bt_mac, 6);
    }

    return response.rval;
}

int rtos_api_lib_get_rtos_log_share_info(stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (stream_info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_RTOS_LOG_SHARE_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    stream_info->share_addr = response.item.stream_share_info.share_addr;
    stream_info->phy_addr = response.item.stream_share_info.phy_addr;
    stream_info->share_size = response.item.stream_share_info.share_size;

    return 0;
}

int rtos_api_lib_set_rtos_log_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_RTOS_LOG_SHARE_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_imu_fifo_share_info(stream_share_info_s *stream_info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (stream_info == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_IMU_FIFO_SHARE_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval != 0) {
        return -1;
    }
    stream_info->share_addr = response.item.stream_share_info.share_addr;
    stream_info->phy_addr = response.item.stream_share_info.phy_addr;
    stream_info->share_size = response.item.stream_share_info.share_size;

    return 0;
}

int rtos_api_lib_get_imu_fifo_data(int max_frame_num, int *frame_num)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_IMU_FIFO_DATA;
    cmd.item.max_frame_num = max_frame_num;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (frame_num != NULL) {
            *frame_num = response.item.frame_num;
        }
    }

    return response.rval;
}

int rtos_api_lib_get_power_source(POWER_SOURCE_e *source, char *name)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_POWER_SOURCE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (source != NULL) {
            *source = response.item.power_source.value;
        }
        if (name != NULL) {
            memcpy(name, response.item.power_source.name, strlen(response.item.power_source.name));
        }
    }

    return response.rval;
}

int rtos_api_lib_get_basic_info(basic_info_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_BASIC_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (info != NULL) {
            memcpy(info, &response.item.basic_info, sizeof(response.item.basic_info));
        }
    }

    return response.rval;
}

int rtos_api_lib_power_off(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_POWER_OFF;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_play_beep_id(BEEP_ID_e beep_id)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_PLAY_BEEP_ID;
    cmd.item.beep_id = beep_id;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_led_color(color_s color)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LED_COLOR;
    cmd.item.led_color.R = color.R;
    cmd.item.led_color.G = color.G;
    cmd.item.led_color.B = color.B;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_led_color(color_s *color)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (color == NULL) {
        return 0;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_LED_COLOR;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        color->R = response.item.led_color.R;
        color->G = response.item.led_color.G;
        color->B = response.item.led_color.B;
    }

    return response.rval;
}

int rtos_api_lib_set_led_rtos_control_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LED_RTOS_CONTROL_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_led_rtos_control_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_LED_RTOS_CONTROL_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_set_usb_mass_storage_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_USB_MASS_STORAGE_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_usb_mass_storage_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_USB_MASS_STORAGE_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_set_speed_unit(SPEED_UNIT_e speed_unit)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_SPEED_UNIT;
    cmd.item.speed_unit = speed_unit;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_speed_unit(SPEED_UNIT_e *speed_unit)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (speed_unit == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_SPEED_UNIT;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *speed_unit = response.item.speed_unit;
    }

    return response.rval;
}

int rtos_api_lib_set_power_onoff_sound_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_POWER_ONOFF_SOUND_ENABLE;
    cmd.item.enable = enable ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_power_onoff_sound_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_POWER_ONOFF_SOUND_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_set_video_encryption_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_VIDEO_ENCRYPTION_ENABLE;
    cmd.item.enable = enable ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_video_encryption_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_VIDEO_ENCRYPTION_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_set_video_encryption_key(const char *key)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (key == NULL || strlen(key) >= 64) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_VIDEO_ENCRYPTION_KEY;
    memset(cmd.item.key, 0, sizeof(cmd.item.key));
    memcpy(cmd.item.key, key, strlen(key));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_video_encryption_key(char *key)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (key == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_VIDEO_ENCRYPTION_KEY;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        memcpy(key, response.item.key, strlen(response.item.key));
    }

    return response.rval;
}

int rtos_api_lib_set_authorization_key(const char *key)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (key == NULL || strlen(key) >= 64) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_AUTHORIZATION_KEY;    
    memset(cmd.item.key, 0, sizeof(cmd.item.key));
    memcpy(cmd.item.key, key, strlen(key));
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_authorization_key(char *key)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (key == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_AUTHORIZATION_KEY;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        memcpy(key, response.item.key, strlen(response.item.key));
    }

    return response.rval;
}

int rtos_api_lib_set_camera_boot_auto_record(CAMERA_CHANNEL_e channel, int record)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_CAMERA_AUTO_RECORD;
    cmd.item.arg_obj.channel = channel;
    cmd.item.arg_obj.arg = record;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_camera_boot_auto_record(int *internal_record, int *external_record)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_CAMERA_AUTO_RECORD;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (internal_record != NULL) {
            *internal_record = (response.item.enable & 0x01) ? 1 : 0;
        }
        if (external_record != NULL) {
            *external_record = (response.item.enable & 0x02) ? 1 : 0;
        }
    }
    return response.rval;
}

int rtos_api_lib_set_mic(MIC_TYPE_e type)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_MIC;
    cmd.item.mic_type = type;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_mic(MIC_TYPE_e *type)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (type == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_MIC;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *type = response.item.mic_type;
    }

    return response.rval;
}

int rtos_api_lib_set_parking_level(unsigned char value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_PARKING_LEVEL;
    cmd.item.parking_level = value;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_parking_level(unsigned char *value)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (value == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_PARKING_LEVEL;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *value = response.item.parking_level;
    }

    return response.rval;
}

int rtos_api_lib_set_lte_wakeup_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LTE_WAKEUP_ENABLE;
    cmd.item.enable = enable ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_lte_wakeup_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_LTE_WAKEUP_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_set_api_files_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_API_FILES_ENABLE;
    cmd.item.enable = enable ? 1 : 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_api_files_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_API_FILES_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_mcu_update(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_MCU_UPDATE_START;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_restart_lte_usb_wifi(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_RESTART_LTE_USB_WIFI_MODULE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_acc_state(unsigned char *state)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (state == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_ACC_STATE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *state = response.item.acc_state;
    }

    return response.rval;
}

int rtos_api_lib_cancel_shutdown(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CANCEL_SHUTDOWN;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_lte_power(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LTE_POWER;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_check_lte_working(int *active)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CHECK_LTE_WORKING;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *active = response.item.active;
    }
    return response.rval;
}

int rtos_api_lib_check_sensor_working(int channel, int *active)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (active == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_CHECK_SENSOR_WORKING;    
    cmd.item.arg_obj.channel = channel;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *active = response.item.active;
    }

    return response.rval;
}

int rtos_api_lib_lte_enter_wakeup_mode(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_LTE_ENTER_WAKEUP_MODE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_cv_enable(int enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_CV_ENABLE;
    cmd.item.enable = enable;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_cv_enable(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_CV_ENABLE;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.active;
    }

    return response.rval;
}

int rtos_api_lib_exec_shell_command(const char *cmdline)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (cmdline == NULL || strlen(cmdline) <= 0) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_EXEC_SHELL_COMMAND;
    snprintf(cmd.item.cmdline, 128, "%s", cmdline);
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_get_battery_info(basic_info_s *info)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_BATTERY_INFO;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        if (info != NULL) {
            memcpy(info, &response.item.basic_info, sizeof(response.item.basic_info));
        }
    }

    return response.rval;
}

int rtos_api_lib_get_hard_reset_flag(int *enable)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    if (enable == NULL) {
        return -1;
    }
    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_GET_HARD_RESET_FLAG;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);
    if (response.rval == 0) {
        *enable = response.item.enable;
    }

    return response.rval;
}

int rtos_api_lib_gnss_power_on(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSS_POWER;
    cmd.item.enable = 1;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_gnss_power_off(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSS_POWER;
    cmd.item.enable = 0;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_gnss_reset(void)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_GNSS_POWER;
    cmd.item.enable = 2;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_bt_power(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_BT_POWER;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_state_led_power(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_STATE_LED_POWER;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_logo_led_power(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_LOGO_LED_POWER;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_ir_led_power(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_IR_LED_POWER;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_charge_enable(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_CHARGE_ENABLE;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_imu_enable(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_IMU_ENABLE;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_road_camera_enable(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_ROAD_CAMERA_ENABLE;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}

int rtos_api_lib_set_cabin_camera_enable(int on)
{
    linux_api_cmd_s cmd;
    linux_api_response_s response;

    memset(&cmd, 0, sizeof(cmd));
    cmd.msg_id = LINUX_API_CMD_SET_CABIN_CAMERA_ENABLE;
    cmd.item.enable = on;
    memset(&response, 0, sizeof(response));
    rtos_api_lib_send_cmd(cmd, &response, DEFAULT_AMBAIPC_TIMEOUT);

    return response.rval;
}


