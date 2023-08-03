#include "app_base.h"
#include "AmbaIPC_RpcProg_R_LinuxApiService.h"
#include "AmbaIPC_RpcProg_R_LinuxApiService2.h"
#include "AmbaIPC_RpcProg_LU_LinuxApiService.h"
#include "AmbaIPC_RpcProg_R_IpcCommunication.h"
#ifdef CONFIG_ENABLE_AMBALINK
#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#endif
#include "AmbaMMU.h"
#include "AmbaDCF.h"
#include "AmbaDCF_Dashcam.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "linux_api_event.c"

static void linux_api_service_msg_handler(linux_api_cmd_s *cmd, AMBA_IPC_SVC_RESULT_s *pRet);
static void linux_api_service2_msg_handler(linux_api_cmd_s *cmd, AMBA_IPC_SVC_RESULT_s *pRet);
extern int product_line_cmd_get_4g_info(char *sim, char *imei, int *csq, int *ping_ok);
extern int product_line_cmd_camera_check(int index);
extern int product_line_cmd_lte_wakeup_test(void);

int linux_api_service_start(void)
{
#define STACK_SIZE (1024 * 1024)
    static unsigned char service_stack[STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static unsigned char service2_stack[STACK_SIZE] GNU_SECTION_NOZEROINIT;
    AMBA_IPC_PROG_INFO_s prog_info;
    AMBA_IPC_PROC_s proc[2];
	int status = -1;

    prog_info.ProcNum = 1;
    prog_info.pProcInfo = (AMBA_IPC_PROC_s *)&(proc[0]);
    prog_info.pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[0].Proc = (AMBA_IPC_PROC_f)&linux_api_service_msg_handler;
    status = AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_LINUX_API_SERVICE_PROG_ID,
                                AMBA_RPC_PROG_R_LINUX_API_SERVICE_VER,
                                "linux_api_service",
                                180, service_stack, STACK_SIZE, &prog_info, 1);
    if (status != 0) {
        debug_line("%s: AmbaIPC_SvcRegister fail.status=%d", __func__, status);
        return -1;
    }
    prog_info.ProcNum = 1;
    prog_info.pProcInfo = (AMBA_IPC_PROC_s *)&(proc[1]);
    prog_info.pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[0].Proc = (AMBA_IPC_PROC_f)&linux_api_service2_msg_handler;
    status = AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_LINUX_API_SERVICE2_PROG_ID,
                                AMBA_RPC_PROG_R_LINUX_API_SERVICE2_VER,
                                "imu_service",
                                200, service2_stack, STACK_SIZE, &prog_info, 1);
    if (status != 0) {
        debug_line("%s: AmbaIPC_SvcRegister fail.status=%d", __func__, status);
        return -1;
    }

    return 0;
}

typedef struct _cmd_info_s_ {
    unsigned int cmd_id;
    char cmd_name[64];
} cmd_info_s;

static cmd_info_s cmd_info[] = {
    {LINUX_API_CMD_REBOOT,                          "reboot"},
    {LINUX_API_CMD_FORMAT_SD,                       "format_sd"},
    {LINUX_API_CMD_DEFAULT_SETTING,                 "default_setting"},
    {LINUX_API_CMD_GET_BUTTON_PRESSED,              "get_button_status"},
    {LINUX_API_CMD_TURN_ONOFF_LED,                  "turn_onoff_led"},
    {LINUX_API_CMD_LOCK_LED,                        "lock_led"},
    {LINUX_API_CMD_GET_IMU_DATA,                    "get_imu"},
    {LINUX_API_CMD_GET_THERMAL_DATA,                "get_thermal"},
    {LINUX_API_CMD_PLAY_SOUND_START,                "play_sound_start"},
    {LINUX_API_CMD_PLAY_SOUND_STOP,                 "play_sound_stop"},
    {LINUX_API_CMD_IS_SOUND_PLAYING,                "check_sound_playing"},
    {LINUX_API_CMD_GET_AUDIO_VOLUME,                "get_audio_volume"},
    {LINUX_API_CMD_SET_AUDIO_VOLUME,                "set_audio_volume"},
    {LINUX_API_CMD_START_EVENT_RECORD,              "start_event_record"},
    {LINUX_API_CMD_STOP_EVENT_RECORD,               "stop_event_record"},
    {LINUX_API_CMD_IS_EVENT_RECORDING,              "check_event_record"},
    {LINUX_API_CMD_CAPTURE_PIV,                     "capture_piv"},
    {LINUX_API_CMD_CAPTURE_AUDIO_START,             "capture_audio_start"},
    {LINUX_API_CMD_CAPTURE_AUDIO_STOP,              "capture_audio_stop"},
    {LINUX_API_CMD_GET_POWER_STATE,                 "get_power_state"},
    {LINUX_API_CMD_GET_HW_SN,                       "get_hw_sn"},
    {LINUX_API_CMD_LIST_ECC_KEYS,                   "list_ecc_keys"},
    {LINUX_API_CMD_WRITE_ECC_KEY,                   "write_ecc_key"},
    {LINUX_API_CMD_EXPORT_ECC_PUBKEY,               "export_ecc_pubkey"},
    {LINUX_API_CMD_SIGN_MESSAGE,                    "sign_message"},
    {LINUX_API_CMD_LIST_AES_KEYS,                   "list_aes_keys"},
    {LINUX_API_CMD_WRITE_AES_KEY,                   "write_aes_key"},
    {LINUX_API_CMD_READ_AES_KEY,                    "read_aes_key"},
    //{LINUX_API_CMD_SET_TIME,                        "set_time"},
    //{LINUX_API_CMD_GET_TIME,                        "get_time"},
    {LINUX_API_CMD_GET_FWUPDATE_FLAG,               "get_fwupdate_flag"},
    {LINUX_API_CMD_SET_FWUPDATE_FLAG,               "set_fwupdate_flag"},
    {LINUX_API_CMD_CLEAR_FWUPDATE_FLAG,             "clear_fwupdate_flag"},
    {LINUX_API_CMD_SET_IR_BRIGHTNESS,               "set_ir_brightness"},
    {LINUX_API_CMD_CHECK_EXTERNAL_CAMERA_CONNECTED, "check_external_camera_state"},
    {LINUX_API_CMD_CHECK_CARD_TRAY_EXIST,           "check_card_tray_exist"},
    {LINUX_API_CMD_GET_GPIO_LEVEL,                  "get_gpio_level"},
    {LINUX_API_CMD_SET_EXPOSURE_AREA,               "set_exposure_area"},
    {LINUX_API_CMD_RESET_EXPOSURE_AREA,             "reset_exposure_area"},
    {LINUX_API_CMD_SET_LED_BRIGHTNESS,              "set_led_brightness"},
    {LINUX_API_CMD_DUMP_RTOS_LOG,                   "dump_log"},
    {LINUX_API_CMD_GET_FILE_LIST_INFO,              "get_file_list"},
    {LINUX_API_CMD_GET_CONFIG,                      "get_config"},
    {LINUX_API_CMD_GET_STATE,                       "get_state"},
    {LINUX_API_CMD_GET_TFCARD,                      "get_tfcard"},
    {LINUX_API_CMD_GET_GNSS,                        "get_gnss"},
    {LINUX_API_CMD_GET_INFO,                        "get_info"},
    {LINUX_API_CMD_SET_EV_BIAS,                     "set_ev_bias"},
    {LINUX_API_CMD_SET_WB,                          "set_wb"},
    {LINUX_API_CMD_START_STOP_RECORD,               "start_stop_record"},
    {LINUX_API_CMD_DELETE_FILE,                     "delete_file"},
    {LINUX_API_CMD_SET_MUTE,                        "set_mute"},
    {LINUX_API_CMD_SET_WATERMARK,                   "set_watermark"},
    {LINUX_API_CMD_SET_WATERMARK_POS,               "set_watermark_pos"},
    {LINUX_API_CMD_SET_BATTERY_POWER_TIME,          "set_battery_power_time"},
    {LINUX_API_CMD_SET_SHUTDOWN_WAIT_TIME,          "set_shutdown_wait_time"},
    {LINUX_API_CMD_SET_RECORD_DURATION,             "set_record_duration"},
    {LINUX_API_CMD_SET_RECORD_QUALITY,              "set_record_quality"},
    {LINUX_API_CMD_SET_RECORD_FPS,                  "set_record_fps"},
    {LINUX_API_CMD_SET_RECORD_BITRATE,              "set_record_bitrate"},
    {LINUX_API_CMD_SET_RECORD_QUALITY_AND_BITRATE,  "set_record_quality_bitrate"},
    {LINUX_API_CMD_SET_PREVIEW_QUALITY,             "set_preview_quality"},
    {LINUX_API_CMD_SET_PREVIEW_FPS,                 "set_preview_fps"},
    {LINUX_API_CMD_SET_PREVIEW_BITRATE,             "set_preview_bitrate"},
    {LINUX_API_CMD_SET_PREVIEW_QUALITY_AND_BITRATE, "set_preview_quality_bitrate"},
    {LINUX_API_CMD_SET_IMAGE_ROTATION,              "set_image_rotation"},
    {LINUX_API_CMD_SET_TIME_FORMAT,                 "set_time_format"},
    {LINUX_API_CMD_SET_TIME_ZONE,                   "set_timezone"},
    {LINUX_API_CMD_GET_TIME_ZONE,                   "get_timezone"},
    {LINUX_API_CMD_SET_DEBUG_MODE,                  "set_debug_mode"},
    {LINUX_API_CMD_SET_IR_THRESHOLD,                "set_ir_threshold"},
    {LINUX_API_CMD_SET_IR_SAMPLING,                 "set_ir_sampling"},
    {LINUX_API_CMD_SET_RTSP_CAM,                    "set_rtsp_cam"},
    {LINUX_API_CMD_LOCK_FILE,                       "lock_file"},
    {LINUX_API_CMD_LOCK_FILE_TIME_RANGE,            "lock_file_range"},
    {LINUX_API_CMD_SET_WIFI_BOOT_MODE,              "set_wifi_boot_mode"},
    {LINUX_API_CMD_SET_WIFI_CONNECT_MODE,           "set_wifi_connect_mode"},
    {LINUX_API_CMD_SET_WIFI_CHANNEL,                "set_wifi_channel"},
    {LINUX_API_CMD_SET_WIFI_TYPE,                   "set_wifi_type"},
    {LINUX_API_CMD_SET_WIFI_AP_PASSWORD,            "set_wifi_ap_passwrd"},
    {LINUX_API_CMD_SET_WIFI_STA_INFO,               "set_wifi_sta_info"},
    {LINUX_API_CMD_SET_WIFI_P2P_WAIT_TIME,          "set_wifi_p2p_wait_time"},
    {LINUX_API_CMD_GET_BT_IMU_STREAM_SHARE_INFO,    "get_imu_stream"},
    {LINUX_API_CMD_GET_BT_GNSS_STREAM_SHARE_INFO,   "get_gnss_stream"},
    {LINUX_API_CMD_SET_BT_IMU_STREAM_ENABLE,        "set_bt_imu_stream"},
    {LINUX_API_CMD_SET_BT_GNSS_STREAM_ENABLE,       "set_bt_gnss_stream"},
    {LINUX_API_CMD_DO_GSENSOR_CALIBRATE,            "do_gsensor_calibrate"},
    {LINUX_API_CMD_SET_GSENSOR_CALIBRATION_EFFECT,  "set_gsensor_calib_effect"},
    {LINUX_API_CMD_SET_GSENSOR_DEBUG,               "set_gsensor_debug"},
    {LINUX_API_CMD_SET_GSENSOR_WAKEUP_VALUE,        "set_gsensor_wakeup_val"},
    {LINUX_API_CMD_SET_GSENSOR_IMPACT_VALUE,        "set_gsensor_impact_val"},
    {LINUX_API_CMD_SET_GSENSOR_FREQ,                "set_gsensor_freq"},
    {LINUX_API_CMD_SET_ACCEL_ODR,                   "set_accel_odr"},
    {LINUX_API_CMD_SET_ACCEL_RANGE,                 "set_accel_range"},
    {LINUX_API_CMD_SET_GYRO_ODR,                    "set_gyro_odr"},
    {LINUX_API_CMD_SET_GYRO_RANGE,                  "set_gyro_range"},
    {LINUX_API_CMD_SET_GYRO_CALIBRATION_EFFECT,     "set_gyro_calib_effect"},
    {LINUX_API_CMD_SET_IMU_SWAP_POLARITY,           "set_imu_swap_polarity"},
    {LINUX_API_CMD_SET_PAIRING_REMINDER_MODE,       "set_reminder_mode"},
    {LINUX_API_CMD_SET_PAIRING_REMINDER_VOICE,      "set_reminder_voice"},
    {LINUX_API_CMD_RESET_GNSS,                      "reset_gnss"},
    {LINUX_API_CMD_SET_GNSS_DATARATE,               "set_gnss_datarate"},
    {LINUX_API_CMD_SET_GNSSID,                      "set_gnssid"},
    {LINUX_API_CMD_SET_GNSS_MGAOFF,                 "set_gnss_mgaoff"},
    {LINUX_API_CMD_SET_LED_REG,                     "set_led_reg"},
    {LINUX_API_CMD_GET_LED_REG,                     "get_led_reg"},
    {LINUX_API_CMD_SOFT_RESET,                      "soft_reset"},
    {LINUX_API_CMD_FACTORY_RESET,                   "factory_reset"},
    {LINUX_API_CMD_GET_USB_LINE_CONNECTED,          "get_usb_line_state"},
    {LINUX_API_CMD_SET_BUTTON_PARAM,                "set_button_param"},
    {LINUX_API_CMD_GET_ADDON_TYPE,                  "get_addon_type"},
    {LINUX_API_CMD_GET_WAKEUP_SOURCE,               "get_wakeup_source"},
    {LINUX_API_CMD_SET_MCU_WAKEUP_TIME,             "set_mcu_wakeup_time"},
    {LINUX_API_CMD_GET_MCU_WAKEUP_TIME,             "get_mcu_wakeup_time"},
    {LINUX_API_CMD_SET_POWER_OFF_MODE,              "set_power_off_mode"},
    {LINUX_API_CMD_GET_STREAM_SHARE_INFO,           "get_stream_share_info"},
    {LINUX_API_CMD_SET_STREAM_SHARE_ENABLE,         "set_stream_share_enable"},
    {LINUX_API_CMD_GET_FACTORY_BURN_INFO,           "get_burn_info"},
    {LINUX_API_CMD_GET_RTOS_LOG_SHARE_INFO,         "get_rtos_log_info"},
    {LINUX_API_CMD_SET_RTOS_LOG_SHARE_ENABLE,       "set_rtos_log_enable"},
    {LINUX_API_CMD_GET_IMU_FIFO_SHARE_INFO,         "get_imu_fifo_info"},
    {LINUX_API_CMD_GET_IMU_FIFO_DATA,               "get_imu_fifo_data"},
    {LINUX_API_CMD_GET_POWER_SOURCE,                "get_power_source"},
    {LINUX_API_CMD_GET_BASIC_INFO,                  "get_basic_info"},
    {LINUX_API_CMD_POWER_OFF,                       "power_off"},
    {LINUX_API_CMD_PLAY_BEEP_ID,                    "play_beep_id"},
    {LINUX_API_CMD_SET_LED_COLOR,                   "set_led_color"},
    {LINUX_API_CMD_GET_LED_COLOR,                   "get_led_color"},
    {LINUX_API_CMD_SET_LED_RTOS_CONTROL_ENABLE,     "set_led_control"},
    {LINUX_API_CMD_GET_LED_RTOS_CONTROL_ENABLE,     "get_led_control"},
    {LINUX_API_CMD_SET_USB_MASS_STORAGE_ENABLE,     "set_usc_enable"},
    {LINUX_API_CMD_GET_USB_MASS_STORAGE_ENABLE,     "get_usc_enable"},
    {LINUX_API_CMD_SET_SPEED_UNIT,                  "set_speed_unit"},
    {LINUX_API_CMD_GET_SPEED_UNIT,                  "get_speed_unit"},
    {LINUX_API_CMD_SET_POWER_ONOFF_SOUND_ENABLE,    "set_power_onoff_sound"},
    {LINUX_API_CMD_GET_POWER_ONOFF_SOUND_ENABLE,    "get_power_onoff_sound"},
    {LINUX_API_CMD_SET_VIDEO_ENCRYPTION_ENABLE,     "set_encryption_enable"},
    {LINUX_API_CMD_GET_VIDEO_ENCRYPTION_ENABLE,     "get_encryption_enable"},
    {LINUX_API_CMD_SET_VIDEO_ENCRYPTION_KEY,        "set_encryption_key"},
    {LINUX_API_CMD_GET_VIDEO_ENCRYPTION_KEY,        "get_encryption_key"},
    {LINUX_API_CMD_SET_AUTHORIZATION_KEY,           "set_authorization_key"},
    {LINUX_API_CMD_GET_AUTHORIZATION_KEY,           "get_authorization_key"},
    {LINUX_API_CMD_SET_CAMERA_AUTO_RECORD,          "set_camera_auto_record"},
    {LINUX_API_CMD_GET_CAMERA_AUTO_RECORD,          "get_camera_auto_record"},
    {LINUX_API_CMD_SET_MIC,                         "set_mic"},
    {LINUX_API_CMD_GET_MIC,                         "get_mic"},
    {LINUX_API_CMD_SET_PARKING_LEVEL,               "set_parking_level"},
    {LINUX_API_CMD_GET_PARKING_LEVEL,               "get_parking_level"},
    {LINUX_API_CMD_SET_LTE_WAKEUP_ENABLE,           "set_lte_wakeup_enable"},
    {LINUX_API_CMD_GET_LTE_WAKEUP_ENABLE,           "get_lte_wakeup_enable"},
    {LINUX_API_CMD_SET_API_FILES_ENABLE,            "set_api_files_enable"},
    {LINUX_API_CMD_GET_API_FILES_ENABLE,            "get_api_files_enable"},
    {LINUX_API_CMD_MCU_UPDATE_START,                "mcu_update_start"},
    {LINUX_API_CMD_RESTART_LTE_USB_WIFI_MODULE,     "restart_lte_module"},
    {LINUX_API_CMD_GET_ACC_STATE,                   "get_acc_state"},
    {LINUX_API_CMD_CANCEL_SHUTDOWN,                 "cancel_shutdown"},
    {LINUX_API_CMD_SET_LTE_POWER,                   "set_lte_power"},
    {LINUX_API_CMD_CHECK_LTE_WORKING,               "check_lte_working"},
    {LINUX_API_CMD_CHECK_SENSOR_WORKING,            "check_sensor_working"},
    {LINUX_API_CMD_LTE_ENTER_WAKEUP_MODE,           "lte_enter_wakeup"},
    {LINUX_API_CMD_SET_CV_ENABLE,                   "set_cv_enable"},
    {LINUX_API_CMD_GET_CV_ENABLE,                   "get_cv_enable"},
    {LINUX_API_CMD_EXEC_SHELL_COMMAND,              "shell"},
    {LINUX_API_CMD_GET_BATTERY_INFO,                "get_battery_info"},
    {LINUX_API_CMD_GET_HARD_RESET_FLAG,             "get_hard_reset_flag"},
    {LINUX_API_CMD_SET_GNSS_POWER,                  "set_gnss_power"},
    {LINUX_API_CMD_SET_BT_POWER,                    "set_bt_power"},
    {LINUX_API_CMD_SET_STATE_LED_POWER,             "set_state_led_power"},
    {LINUX_API_CMD_SET_LOGO_LED_POWER,              "set_logo_led_power"},
    {LINUX_API_CMD_SET_IR_LED_POWER,                "set_ir_led_power"},
    {LINUX_API_CMD_SET_CHARGE_ENABLE,               "set_charge_enable"},
    {LINUX_API_CMD_SET_IMU_ENABLE,                  "set_imu_enable"},
    {LINUX_API_CMD_SET_ROAD_CAMERA_ENABLE,          "set_road_camera_enable"},
    {LINUX_API_CMD_SET_CABIN_CAMERA_ENABLE,         "set_cabin_camera_enable"},
    //{LINUX_API_CMD_SET_TIME,                        "set_time"},
    //{LINUX_API_CMD_GET_TIME,                        "get_time"},
};

static cmd_info_s cmd_info2[] = {
    {LINUX_API_CMD_GET_IMU_FIFO_DATA,               "get_imu_fifo_data"},
    {LINUX_API_CMD_SET_TIME,                        "set_time"},
    {LINUX_API_CMD_GET_TIME,                        "get_time"},
};

static UINT8 crc7_cal(const unsigned int *src, const unsigned int num)
{
    unsigned int i = 0, a = 0, cnt = 0;
    unsigned char data[64] = {0};
    unsigned char crc = 0, tmp = 0;

    if (num > 16) {
        return 0;
    }
    for (i = 0; i < num; i++) {
        data[i * 4 + 0] = (src[i] >> 24) & 0xFF;
        data[i * 4 + 1] = (src[i] >> 16) & 0xFF;
        data[i * 4 + 2] = (src[i] >> 8) & 0xFF;
        data[i * 4 + 3] = src[i] & 0xFF;
    }
    cnt = num * 4 - 1;

    crc = 0;
    for (a = 0; a < cnt; a++) {
        tmp = data[a];
        for (i = 0; i < 8; i++) {
            crc <<= 1;
            if ((tmp & 0x80) ^ (crc & 0x80)) {
                crc ^= 0x09;
            }
            tmp <<= 1;
        }
	}
	crc = (crc << 1) | 1;

	return crc;
}

static int debug = 1;
void linux_api_service_set_debug(int enable)
{
    debug =  enable;
}

int linux_api_service_get_debug(void)
{
    return debug;
}

static void app_timer_lte_enter_wakeup_handler(int eid)
{
    static int cnt = 0;

    (void)eid;
    cnt += 1;
    if (cnt >= 3) {
        cnt = 0;
        product_line_cmd_lte_wakeup_test();
    }
}

static void linux_api_service_msg_handler(linux_api_cmd_s *cmd, AMBA_IPC_SVC_RESULT_s *pRet)
{
    linux_api_response_s *response = NULL;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    unsigned int i = 0;
    unsigned int start_tick = 0, end_tick = 0;

    AmbaMisra_TypeCast32(&response, &(pRet->pResult));
    response->msg_id = cmd->msg_id;
    for (i = 0; i < sizeof(cmd_info) / sizeof(cmd_info_s); i++) {
        if (cmd_info[i].cmd_id == cmd->msg_id) {
            if (debug) debug_line("linux api service msg_id=%d, %s", cmd->msg_id, cmd_info[i].cmd_name);
            break;
        }
    }
    if (i >= (sizeof(cmd_info) / sizeof(cmd_info_s))) {
        debug_line("linux api service msg_id=%d, %s", cmd->msg_id, "not support");
        response->rval = -1;
        goto end;
    }
    response->rval = 0;
    start_tick = tick();
    SvcUserPref_Get(&pSvcUserPref);
    switch (cmd->msg_id) {
    case LINUX_API_CMD_REBOOT:
        app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
        break;
    case LINUX_API_CMD_FORMAT_SD:
        if (app_helper.format_busy) {
            debug_line("format is busy");
            response->rval = -1;
            break;
        }
        if (cmd->item.sd_disk) {
            if (app_helper.check_sd_exist() == 0) {
                debug_line("sd not exist or bad");
                response->rval = -1;
                break;
            }
        }
        app_msg_queue_send(APP_MSG_ID_FORMAT_SD, cmd->item.sd_disk, 0, 0);
        break;
    case LINUX_API_CMD_DEFAULT_SETTING:
        app_msg_queue_send(APP_MSG_ID_DEFAULT_SETTING, 0, 0, 0);
        break;
    case LINUX_API_CMD_PLAY_SOUND_START:
        {
            char rtos_path[128] = {0};
            memset(rtos_path, 0, sizeof(rtos_path));
            if (app_helper.convert_path_linux2rtos((const char *)cmd->item.arg_obj.data, rtos_path) < 0) {
                response->rval = -2;
                break;
            }
            debug_line("linux play sound path: %s", rtos_path);
            response->rval = beep_file(rtos_path);
        }
        break;
    case LINUX_API_CMD_PLAY_SOUND_STOP:
        response->rval = beep_stop();
        break;
    case LINUX_API_CMD_IS_SOUND_PLAYING:
        response->item.sound_play_status.is_playing = beep_is_playing();
        response->item.sound_play_status.remaining_ms = beep_remain_ms();
        break;
    case LINUX_API_CMD_GET_AUDIO_VOLUME:
        response->item.audio_volume = pSvcUserPref->AudioVolume;
        break;
    case LINUX_API_CMD_SET_AUDIO_VOLUME:
        if (cmd->item.audio_volume > 3) {
            response->rval = -1;
            break;
        }
        pSvcUserPref->AudioVolume = cmd->item.audio_volume;
        AmbaAudio_CodecSetVolume(0, cmd->item.audio_volume);
        break;
    case LINUX_API_CMD_START_EVENT_RECORD:
        if (cmd->item.event_record_param.channel >= CAMERA_CHANNEL_BOTH) {
            debug_line("<%s:%d> invalid channel", __func__, __LINE__);
            response->rval = -1;
            break;
        }
        if (rec_dvr_is_event_recording(cmd->item.event_record_param.channel)) {
            debug_line("<%s:%d> event record is busy", __func__, __LINE__);
            response->rval = -1;
            break;
        }
        response->rval = rec_dvr_event_record_start(cmd->item.event_record_param.channel, cmd->item.event_record_param.pre_seconds, cmd->item.event_record_param.length);
        break;
    case LINUX_API_CMD_STOP_EVENT_RECORD:
        if (cmd->item.event_record_param.channel >= CAMERA_CHANNEL_BOTH) {
            response->rval = -1;
            break;
        }
        response->rval = rec_dvr_event_record_stop(cmd->item.event_record_param.channel);
        break;
    case LINUX_API_CMD_IS_EVENT_RECORDING:
        if (cmd->item.event_record_param.channel >= CAMERA_CHANNEL_BOTH) {
            response->rval = -1;
            break;
        }
        response->rval = rec_dvr_is_event_recording(cmd->item.event_record_param.channel) ? 1 : 0;
        if (response->rval) {
            memset(response->item.filepath.path1, 0, sizeof(response->item.filepath.path1));
        }
        break;
    case LINUX_API_CMD_CAPTURE_PIV:
        response->rval = rec_dvr_capture_piv(cmd->item.piv_param.channel, 1);
        if (response->rval >= 0) {
            int timeout = 0;
            while (timeout < 100) {
                if (rec_dvr_check_piv_finish()) {
                    break;
                }
                timeout++;
                msleep(30);
            }
            if (timeout >= 100) {
                debug_line("piv waiting timeout");
                response->rval = -2;
            } else {
                if (cmd->item.piv_param.channel == CAMERA_CHANNEL_INTERNAL
                    || cmd->item.piv_param.channel == CAMERA_CHANNEL_BOTH) {
                    memset(response->item.filepath.path1, 0, sizeof(response->item.filepath.path1));
                    app_helper.convert_path_rtos2linux(AmbaDCF_GetPivPath(0), response->item.filepath.path1);
                } else if (cmd->item.piv_param.channel == CAMERA_CHANNEL_EXTERNAL
                    || cmd->item.piv_param.channel == CAMERA_CHANNEL_BOTH) {
                    memset(response->item.filepath.path2, 0, sizeof(response->item.filepath.path2));
                    app_helper.convert_path_rtos2linux(AmbaDCF_GetPivPath(2), response->item.filepath.path2);
                }
            }
        }
        break;
    case LINUX_API_CMD_CAPTURE_AUDIO_START:
        {
            char rtos_path[128] = {0};
            if (pcm_record_is_busy() || aac_record_is_busy()) {
                response->rval = -3;
                break;
            }
            memset(rtos_path, 0, sizeof(rtos_path));
            if (app_helper.convert_path_linux2rtos((const char *)cmd->item.arg_obj.data, rtos_path) < 0) {
                response->rval = -2;
                break;
            }
            debug_line("linux audio record path: %s", rtos_path);
            if (strncasecmp(rtos_path + strlen(rtos_path) - 4, ".aac", 4) == 0) {
                if (aac_record_set_path(rtos_path) < 0) {
                    response->rval = -1;
                    break;
                }
                response->rval = aac_record_start();
            } else {
                if (pcm_record_set_path(rtos_path) < 0) {
                    response->rval = -1;
                    break;
                }
                response->rval = pcm_record_start();
            }
        }
        break;
    case LINUX_API_CMD_CAPTURE_AUDIO_STOP:
        {
            int timeout = 50;
            pcm_record_stop();
            aac_record_stop();
            while ((pcm_record_is_busy() || aac_record_is_busy()) && (--timeout >= 0)) msleep(100);
        }
        break;
    case LINUX_API_CMD_GET_HW_SN:
        {
            metadata_s *data = NULL;
            metadata_get(&data);
            memset(response->item.sn, 0, sizeof(response->item.sn));
            memcpy(response->item.sn, data->SerialNumber, sizeof(response->item.sn));
        }
        break;
    case LINUX_API_CMD_GET_FWUPDATE_FLAG:
        {
            int value = Pmic_GetSramRegister();
            if ((value & 0x20) == 0x20) {
                response->item.fwupdate_flag = 1;
            } else {
                response->item.fwupdate_flag = 0;
            }
        }
        //response->item.fwupdate_flag = AmbaNVM_IsBldMagicCodeSet(NVM_TYPE) ? 1 : 0;
        break;
    case LINUX_API_CMD_SET_FWUPDATE_FLAG:
        //AmbaNVM_SetBldMagicCode(NVM_TYPE, 5000);
        Pmic_SetSramRegister(0x20);
        break;
    case LINUX_API_CMD_CLEAR_FWUPDATE_FLAG:
        {
            int value = Pmic_GetSramRegister();
            value &= ~0x20;
            Pmic_SetSramRegister(value);
        }
        //AmbaNVM_EraseBldMagicCode(NVM_TYPE, 5000);
        break;
    case LINUX_API_CMD_CHECK_EXTERNAL_CAMERA_CONNECTED:
        response->item.external_camera_connected = app_helper.external_camera_connected;
        break;
    case LINUX_API_CMD_CHECK_CARD_TRAY_EXIST:
        response->item.card_tray_exist = AmbaSD_IsCardPresent(SD_CHANNEL) ? 1 : 0;
        break;
    case LINUX_API_CMD_GET_GPIO_LEVEL:
        {
            int level = app_helper.gpio_get(cmd->item.gpio_id);
            if (level < 0) {
                response->rval = -1;
                break;
            }
            response->item.gpio_level = level;
        }
        break;
    case LINUX_API_CMD_DUMP_RTOS_LOG:
        {
            char rtos_path[128] = {0};
            memset(rtos_path, 0, sizeof(rtos_path));
            if (app_helper.convert_path_linux2rtos((const char *)cmd->item.arg_obj.data, rtos_path) < 0) {
                response->rval = -2;
                break;
            }
            response->rval = offline_log_dump(rtos_path);
        }
        break;
    case LINUX_API_CMD_GET_FILE_LIST_INFO:
        {
#define CAR_DCF_MAX_CNT 10000U
            static UINT32 PhyAddr = 0, ShareAddr = 0;
            if (ShareAddr == 0) {
                ShareAddr = (UINT32)AmbaDCF_GetFileList(0);
                if ((void *)ShareAddr == NULL) {
                    response->rval = -1;
                    break;
                }
            }
            if (PhyAddr == 0) {
                AmbaMMU_Virt32ToPhys32((UINT32)ShareAddr, &PhyAddr);
            }
            response->item.filelist_info.ready = AmbaDCF_IsReady() ? 1 : 0;
            AmbaDCF_GetFileAmount(0, 0, AMBA_DCF_FILE_TYPE_VIDEO, &response->item.filelist_info.file_amount[0]);
            AmbaDCF_GetFileAmount(0, 1, AMBA_DCF_FILE_TYPE_VIDEO, &response->item.filelist_info.file_amount[1]);
            AmbaDCF_GetFileAmount(0, 2, AMBA_DCF_FILE_TYPE_IMAGE, &response->item.filelist_info.file_amount[2]);
            response->item.filelist_info.share_addr = CAST_TO_UINTPRT(ShareAddr);
            response->item.filelist_info.phy_addr = (UINT64)PhyAddr;
            response->item.filelist_info.share_size = sizeof(SvcDCF_DashcamRefEmem_t) * AMBA_DCF_DRIVER_MAX_NUM * AMBA_DCF_FOLDER_MAX_NUM * AMBA_DCF_FOLDER_MAX_NUM * (CAR_DCF_MAX_CNT + 1);
        }
        break;
    case LINUX_API_CMD_GET_CONFIG:
        {
            config_s info;
            metadata_s *data = NULL;
            device_info_s *device_info =  app_helper.get_device_info();
            metadata_get(&data);
            memset(&info, 0, sizeof(config_s));
            if (pSvcUserPref->InternalCamera.MainStream.width == 1920
                && pSvcUserPref->InternalCamera.MainStream.height == 1080) {
                info.record_quality = 1;
            } else if (pSvcUserPref->InternalCamera.MainStream.width == 1280
                && pSvcUserPref->InternalCamera.MainStream.height == 720) {
                info.record_quality = 3;
            } else if (pSvcUserPref->InternalCamera.MainStream.width == 3840
                && pSvcUserPref->InternalCamera.MainStream.height == 2160) {
                info.record_quality = 5;
            } else if (pSvcUserPref->InternalCamera.MainStream.width == 960
                && pSvcUserPref->InternalCamera.MainStream.height == 540) {
                info.record_quality = 7;
            } else {
                info.record_quality = 5;
            }
            info.record_bitrate = pSvcUserPref->InternalCamera.MainStream.bitrate * 10;
            if (pSvcUserPref->FileSplitTimeSeconds == 120) {
                info.record_duration = 1;
            } else if (pSvcUserPref->FileSplitTimeSeconds == 180) {
                info.record_duration = 2;
            } else if (pSvcUserPref->FileSplitTimeSeconds == 300) {
                info.record_duration = 3;
            } else if (pSvcUserPref->FileSplitTimeSeconds == 60) {
                info.record_duration = 4;
            } else if (pSvcUserPref->FileSplitTimeSeconds == 20) {
                info.record_duration = 5;
            } else if (pSvcUserPref->FileSplitTimeSeconds == 40) {
                info.record_duration = 6;
            } else {
                info.record_duration = 4;
            }
            if (pSvcUserPref->ExternalCamera.MainStream.width == 1280
                && pSvcUserPref->ExternalCamera.MainStream.height == 720) {
                info.rear_quality = 1;
            } else if (pSvcUserPref->ExternalCamera.MainStream.width == 960
                && pSvcUserPref->ExternalCamera.MainStream.height == 540) {
                info.rear_quality = 3;
            } else {
                info.rear_quality = 0;
            }
            info.rear_bitrate = pSvcUserPref->ExternalCamera.MainStream.bitrate * 10;
            info.rear_frame = pSvcUserPref->ExternalCamera.Fps;
            info.photo_quality = 0;
            if (pSvcUserPref->AwbMode == WB_AUTOMATIC) {
                info.white_balance = 0;
            } else if (pSvcUserPref->AwbMode == WB_SUNNY) {
                info.white_balance = 1;
            } else if (pSvcUserPref->AwbMode == WB_CLOUDY) {
                info.white_balance = 2;
            } else if (pSvcUserPref->AwbMode == WB_INCANDESCENT) {
                info.white_balance = 3;
            } else if (pSvcUserPref->AwbMode == WB_FLUORESCENT) {
                info.white_balance = 4;
            } else {
                info.white_balance = 0;
            }
            info.exposure = pSvcUserPref->AeBias;
            if (pSvcUserPref->InternalCamera.AutoRecord
                || (app_helper.external_camera_enabled && pSvcUserPref->ExternalCamera.AutoRecord)) {
                info.boot_record = 1;
            } else {
                info.boot_record = 0;
            }
            info.mute = (pSvcUserPref->RecordSoundOnOff == OPTION_ON) ? 0 : 1;
            info.rear_view_mirror = 0;
            info.language = 0;
            info.smart_detect = 0;
            info.frontcar_crash_sensitivity = 0;
            info.impact_sensitivity = 0;
            info.motion_detect = (pSvcUserPref->MotionDetect == OPTION_ON) ? 1 : 0;
            if (pSvcUserPref->WaterMark == WATER_MARK_NONE) {
                info.watermark = 0;
            } else if (pSvcUserPref->WaterMark == (WATER_MARK_LOGO | WATER_MARK_DATETIME | WATER_MARK_SPEED)) {
                info.watermark = 1;
            } else if (pSvcUserPref->WaterMark == (WATER_MARK_LOGO | WATER_MARK_DATETIME)) {
                info.watermark = 2;
            } else {
                info.watermark = 0;
            }
            if (pSvcUserPref->WaterMarkPos == WATER_MARK_POS_BOTTOM) {
                info.watermark_pos = 0;
            } else {
                info.watermark_pos = 1;
            }
            info.record_switch = 0;
            info.dev_type = 0;
            info.curRecordTime = rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time;
            info.videoTime = 0;
            info.shotRecord = 0;
            info.shutdown_mode = 0;
            info.debug_mode = pSvcUserPref->DebugMode ? 1 : 0;
            info.sound_vol = pSvcUserPref->AudioVolume;
            info.wakeup_val = 0;
            info.support_obd = 0;
            info.support_backcamera = 1;
            info.support_adas = 0;
            info.only_gsensor_startup = 0;
            info.support_battery = 1;
            info.support_gps = 1;
            info.support_edog = 0;
            info.support_gui = 0;
            info.support_wifi = 1;
            info.support_mobile = 0;
            info.support_wifi_connect = 1;
            info.support_low_voltage = 1;
            info.config_version = pSvcUserPref->SettingVersion;
            info.adas_fcw_switch = 0;
            info.adas_fcw_sensitivity = 0;
            info.adas_fcw_speed = 0;
            info.adas_ldw_switch = 0;
            info.adas_ldw_sensitivity = 0;
            info.adas_tips_mode = 0;
            info.gsp_data_version = 0;
            info.gps_tips_mode = 0;
            info.startup_voice = (pSvcUserPref->PowerOnOffSound == OPTION_ON) ? 1 : 0;
            if (pSvcUserPref->InternalCamera.SecStream.width == 1280
                && pSvcUserPref->InternalCamera.SecStream.height == 720) {
                info.net_preview_quality = 0;
            } else if (pSvcUserPref->InternalCamera.SecStream.width == 960
                && pSvcUserPref->InternalCamera.SecStream.height == 540) {
                info.net_preview_quality = 1;
            } else if (pSvcUserPref->InternalCamera.SecStream.width == 1920
                && pSvcUserPref->InternalCamera.SecStream.height == 1080) {
                info.net_preview_quality = 2;
            } else {
                info.net_preview_quality = 0;
            }
            if (pSvcUserPref->ExternalCamera.SecStream.width == 1280
                && pSvcUserPref->ExternalCamera.SecStream.height == 720) {
                info.rear_preview_quality = 0;
            } else if (pSvcUserPref->ExternalCamera.SecStream.width == 960
                && pSvcUserPref->ExternalCamera.SecStream.height == 540) {
                info.rear_preview_quality = 1;
            } else if (pSvcUserPref->ExternalCamera.SecStream.width == 1920
                && pSvcUserPref->ExternalCamera.SecStream.height == 1080) {
                info.rear_preview_quality = 2;
            } else {
                info.rear_preview_quality = 0;
            }
            info.record_delayp = 0;
            info.photo_timetakephotos = 0;
            info.photo_automatictakephotos = 0;
            info.image_rotation = 0;
            info.light_freq = 0;
            info.led_on_off = (pSvcUserPref->LedOnOff == OPTION_ON) ? 1 : 0;
            info.screen_sleep = 0;
            info.measure_unit = 0;
            info.auto_shutdown_time = 0;
            info.record_quality_bitmap = 0;
            info.lang_bitmap = 0;
            info.voice_lang_bitmap = 0;
            info.voice_lang = 0;
            info.take_photo_mode = 0;
            info.slow_record_mode = 0;
            info.motion_take_photo_mode = 0;
            info.wifi_connect_mode = 0;
            if (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_AP) {
                info.wifi_boot_connect_mode = 0;
            } else if (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_STA) {
                info.wifi_boot_connect_mode = 1;
            } else if (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_P2P) {
                info.wifi_boot_connect_mode = 2;
            } else {
                info.wifi_boot_connect_mode = 0;
            }
            info.p2p_wait_time = pSvcUserPref->WifiSetting.P2pWaitTime;
            info.switch_keytone = 0;
            info.switch_takephoto_tone = 0;
            info.wifi_type = pSvcUserPref->WifiSetting.Use5G ? 1 : 0;
            info.obd_low_voltage = 0;
            info.obd_winter_low_voltage = 0;
            info.battery_power_time = pSvcUserPref->BatteryPowerTime;
            info.wifi_channel = pSvcUserPref->WifiSetting.Channel;
            info.net_preview_bitrate = pSvcUserPref->InternalCamera.SecStream.bitrate * 10;
            info.net_preview_frame = pSvcUserPref->InternalCamera.Fps;
            info.park_timelapse = 0;
            info.time_format = pSvcUserPref->Use24HMode ? 0 : 1;
            info.ir_threshold = pSvcUserPref->IrThresHold;
            info.ir_sampling = pSvcUserPref->IrSampling;
            info.shutdown_wait_time = pSvcUserPref->NoActiveShutdownTime;
            snprintf(info.firmware_version, sizeof(info.firmware_version) - 1, "%s", device_info->sw_version);
            snprintf(info.buildTime, sizeof(info.buildTime) - 1, "%s", device_info->build_timestamp);
            snprintf(info.manufacturer, sizeof(info.manufacturer) - 1, "%s", PROJECT_VENDOR);
            snprintf(info.otaVersion, sizeof(info.otaVersion) - 1, "%s", "NEXAR_W1_EN-v3.3.17");
            snprintf(info.product_sn, sizeof(info.product_sn) - 1, "%s", (char *)data->SerialNumber);
            snprintf(info.product_cpuid, sizeof(info.product_cpuid) - 1, "%s", (char *)data->CpuId);
            snprintf(info.product_vsn, sizeof(info.product_vsn) - 1, "%s", device_info->vsn);
            snprintf(info.obd_version, sizeof(info.obd_version) - 1, "%s", "");
            snprintf(info.impact_value, sizeof(info.impact_value) - 1, "%s", "0,80,50,80");
            //snprintf(info.token, sizeof(info.token) - 1, "%s", device_info->token);
            if (pSvcUserPref->WifiSetting.Use5G) {
                snprintf(info.wifi_ssid, sizeof(info.wifi_ssid) - 1, "Nexar(5G)-%s", pSvcUserPref->WifiSetting.ApSSID);
            } else {
                snprintf(info.wifi_ssid, sizeof(info.wifi_ssid) - 1, "Nexar-%s", pSvcUserPref->WifiSetting.ApSSID);
            }
            snprintf(info.wifiPsd, sizeof(info.wifiPsd) - 1, "%s", pSvcUserPref->WifiSetting.ApPassword);
            snprintf(info.wifi_ap_ssid, sizeof(info.wifi_ap_ssid) - 1, "%s", pSvcUserPref->UsbWifiSetting.StaSSID);
            snprintf(info.wifi_ap_passwd, sizeof(info.wifi_ap_passwd) - 1, "%s", pSvcUserPref->UsbWifiSetting.StaPassword);
            memcpy(&response->item.config, &info, sizeof(info));
        }
        break;
    case LINUX_API_CMD_GET_STATE:
        {
            state_s info;
            TEMP_DATA_s temp_data;
            device_info_s *device_info = app_helper.get_device_info();
            memset(&info, 0, sizeof(state_s));
            info.record = (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_RECORD) || (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD);
            info.mute = (pSvcUserPref->RecordSoundOnOff == OPTION_ON) ? 0 : 1;
            info.obd = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2) 
            info.backcamera = app_helper.check_gc2053_left_connected() || app_helper.check_gc2053_right_connected();
#else
            info.backcamera = app_helper.check_external_camera_connected();
#endif
            info.battery = battery_task_get_percentage();
            info.uptime = tick() / 1000;
            info.debug_wakeup_val = 0;
            info.timezone = pSvcUserPref->TimeZone;
            info.battery_voltage = battery_task_get_voltage_mv();
            battery_task_check_charger_ic_stat(&(info.battery_charging), NULL, NULL, NULL);
            info.battery_online = Pmic_CheckUsbConnected() ? 0 : 1;
            info.acc_state = Pmic_GetAccState() ? 1 : 0;
            info.usb_state = Pmic_CheckUsbConnected() ? 1 : 0;
            info.rear_record_disable = (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD) ? 0 : 1;
            if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
                if (sd_bad_or_not_exist) {
                    info.tf_state = 2;
                } else {
                    info.tf_state = 1;
                }
            } else {
                info.tf_state = 0;
            }
            info.tf_slow_times = 0;
            info.startup_index = device_info->startup_index;
            info.valid_data_index = 0;
            info.obd_voltage = 0;
            info.obd_state = 0;
            info.obd_rt_time = 0;
            info.gps = 0;
            info.gps_time = 0;
            info.gps_lng = 0;
            info.gps_lat = 0;
            info.gps_speed = 0;
            info.gps_degree = 0;
            info.gps_gga_time = 0;
            info.gps_gga_state = 0;
            info.gps_satellite = 0;
            info.gps_msl = 0;
            info.gps_geoid = 0;
            info.gps_hdop = 0;
            if (info.backcamera == 0) {
                info.ir_state = -1;
            } else {
                info.ir_state = app_helper.ir_check_onoff() ? 1 : 0;
            }
            info.ir_switch_time = tick() - app_helper.ir_last_changed_time;
            Imu_GetTemperature(&temp_data);
            info.gsensor_temp = temp_data.temp_float;
            snprintf(info.tripkey, sizeof(info.tripkey) - 1, "%s", device_info->trip_key);
            memcpy(&response->item.state, &info, sizeof(info));
        }
        break;
    case LINUX_API_CMD_GET_TFCARD:
        {
            tfcard_s info;
            metadata_s *data = NULL;
            metadata_get(&data);
            memset(&info, 0, sizeof(tfcard_s));
            if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
                if (sd_bad_or_not_exist) {
                    info.state = 2;
                } else {
                    SD_RawRegs_s RawRegs;
                    const AMBA_SD_INFO_REG_s *pSdReg = AmbaSD_GetCardRegs(SD_CHANNEL);
                    info.state = 1;
                    app_helper.get_sd_space(&info.total, &info.remain);
                    AmbaSD_GetRawRegs(SD_CHANNEL, &RawRegs);
                    snprintf(info.oemid, sizeof(info.oemid) - 1, "0x%x", pSdReg->CardID.OemID);
                    RawRegs.CidRegs[3] |= crc7_cal(&RawRegs.CidRegs[0], 4);
                    snprintf(info.cid, sizeof(info.cid) - 1, "%.8x%.8x%.8x%.8x", RawRegs.CidRegs[0], RawRegs.CidRegs[1],
                                                                                RawRegs.CidRegs[2], RawRegs.CidRegs[3]);
                    RawRegs.CsdRegs[3] |= crc7_cal(&RawRegs.CsdRegs[0], 4);
                    snprintf(info.csd, sizeof(info.csd) - 1, "%.8x%.8x%.8x%.8x", RawRegs.CsdRegs[0], RawRegs.CsdRegs[1],
                                                                                RawRegs.CsdRegs[2], RawRegs.CsdRegs[3]);
                    snprintf(info.fwrev, sizeof(info.fwrev) - 1, "0x%x", pSdReg->CardID.ProductRevision & 0xF);
                    snprintf(info.hwrev, sizeof(info.hwrev) - 1, "0x%x", (pSdReg->CardID.ProductRevision >> 4) & 0xF);
                    snprintf(info.serial, sizeof(info.serial) - 1, "0x%x", pSdReg->CardID.ProductSerialNo);
                    snprintf(info.scr, sizeof(info.scr) - 1, "%.8x%.8x", RawRegs.ScrRegs[0], RawRegs.ScrRegs[1]);
                    memcpy(info.name, pSdReg->CardID.ProductName, sizeof(pSdReg->CardID.ProductName));
                    snprintf(info.manfid, sizeof(info.manfid) - 1, "0x%.6x", pSdReg->CardID.ManufacturerID);
                    snprintf(info.date, sizeof(info.date) - 1, "%.2d/%.4d", pSdReg->CardID.ManufacturingMonth, pSdReg->CardID.ManufacturingYear + 2000);
                }
            } else {
                info.state = 0;
            }
            memcpy(&response->item.tfcard, &info, sizeof(info));
        }
        break;
    case LINUX_API_CMD_GET_GNSS:
        {
            gnss_s info;
            memset(&info, 0, sizeof(gnss_s));
            info.uart_bitrate = pSvcUserPref->GnssSetting.UartBitRate;
            info.support_gnssid = GNSS_TYPE_GPS | GNSS_TYPE_GALILEO | GNSS_TYPE_QZSS | GNSS_TYPE_SBAS | GNSS_TYPE_QLONASS;
            info.enable_gnssid = pSvcUserPref->GnssSetting.Id;
            info.aop_config = 0;
            info.mga_type = 0;
            info.mga_time_s = 0;
            info.mga_time_e = 0;
            info.data_rate = pSvcUserPref->GnssSetting.DataRate;
            info.nmea_version = 65;
            snprintf(info.gnss_model, sizeof(info.gnss_model) - 1, "%s", "HD8040_JS-2118AH4S");
            memcpy(&response->item.gnss, &info, sizeof(info));
        }
        break;
    case LINUX_API_CMD_GET_INFO:
        {
            int has_2nd_camera = 0, has_mobile_data = 0;
            info_s info;
            metadata_s *data = NULL;
            device_info_s *device_info = app_helper.get_device_info();
            metadata_get(&data);
            memset(&info, 0, sizeof(info_s));
            info.protocol_ver[1] = 1;
            info.protocol_ver[0] = 0;
            info.wifi_status = 1;
            info.ibeacon_major = device_info->ibeacon_major;
            info.ibeacon_minor = device_info->ibeacon_minor;
            snprintf(info.vendor, sizeof(info.vendor) - 1, "%s", PROJECT_VENDOR);
            snprintf(info.model, sizeof(info.model) - 1, "%s", PROJECT_MODEL);
            snprintf(info.sn, sizeof(info.sn) - 1, "%s", (char *)data->SerialNumber);
            if (pSvcUserPref->WifiSetting.Use5G) {
                snprintf(info.wifi_ssid, sizeof(info.wifi_ssid) - 1, "Nexar(5G)-%s", pSvcUserPref->WifiSetting.ApSSID);
            } else {
                snprintf(info.wifi_ssid, sizeof(info.wifi_ssid) - 1, "Nexar-%s", pSvcUserPref->WifiSetting.ApSSID);
            }
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            if (app_helper.check_gc2053_left_connected() || app_helper.check_gc2053_right_connected()) {
                has_2nd_camera = 1;
            }
            if (app_helper.check_lte_left_connected() || app_helper.check_lte_right_connected()) {
                has_mobile_data = 1;
            }
#else
            has_2nd_camera = app_helper.check_external_camera_connected();
            has_mobile_data = 1;
#endif
            snprintf(info.capabilities, sizeof(info.capabilities) - 1, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                                                        1,//has_acc
                                                                        1,//has_gyro
                                                                        1,//acc_over_ble
                                                                        1,//gyro_over_ble
                                                                        has_2nd_camera,//has_2nd_camera
                                                                        0,//has_3rd_camera
                                                                        1,//has_parking_mode
                                                                        0,//is_hardwired
                                                                        1,//has_ble
                                                                        1,//has_ble_2mbps
                                                                        1,//has_wifi_client
                                                                        1,//has_wifi_p2p
                                                                        0,//has_adas
                                                                        gnss_task_get_connected() ? 1 : 0,//has_gps
                                                                        0,//has_pir
                                                                        has_mobile_data,//has_mobile_data
                                                                        1,//has_speaker
                                                                        1,//has_microphone
                                                                        0,//has_alexa
                                                                        0,//has_obd
                                                                        0,//has_ms_time
                                                                        1,//has_ext_sdcard
                                                                        0,//has_ext_ir_leds
                                                                        has_2nd_camera ? 1 : 0,//has_int_ir_leds
                                                                        0,//has_display
                                                                        1//has_battery
                                                                        );
            memcpy(&response->item.info, &info, sizeof(info));
        }
        break;
    case LINUX_API_CMD_SET_EV_BIAS:
        {
            AE_CONTROL_s AeCtrl;
            if (cmd->item.arg_obj.arg < 0 || cmd->item.arg_obj.arg > 6) {
                response->rval = -1;
                break;
            }
            AmbaImgProc_AEGetAEControlCap(cmd->item.arg_obj.channel, &AeCtrl);
            pSvcUserPref->AeBias = cmd->item.arg_obj.arg;
            AeCtrl.EvBias = (pSvcUserPref->AeBias - 3) * 32;
            AmbaImgProc_AESetAEControlCap(cmd->item.arg_obj.channel, &AeCtrl);
            user_setting_save();
            break;
        }
        break;
    case LINUX_API_CMD_SET_WB:
        {
            AWB_CONTROL_s AwbCtrl;
            UINT8 mode = 0xFF;
            AmbaImgProc_AWBGetAWBCtrlCap(cmd->item.arg_obj.channel, &AwbCtrl);
            switch (cmd->item.arg_obj.arg) {
            case 0:
                mode = WB_AUTOMATIC;
                break;
            case 1:
                mode = WB_SUNNY;
                break;
            case 2:
                mode = WB_CLOUDY;
                break;
            case 3:
                mode = WB_INCANDESCENT;
                break;
            case 4:
                mode = WB_FLUORESCENT;
                break;
            default:
                response->rval = -1;
                goto end;
            }
            pSvcUserPref->AwbMode = mode;
            AwbCtrl.MenuMode = mode;
            AmbaImgProc_AWBSetAWBCtrlCap(cmd->item.arg_obj.channel, &AwbCtrl);
            user_setting_save();
            break;
        }
        break;
    case LINUX_API_CMD_START_STOP_RECORD:
        response->rval = rec_dvr_camera_record_control(cmd->item.arg_obj.channel, cmd->item.arg_obj.arg);
        break;
    case LINUX_API_CMD_DELETE_FILE:
        {
            char rtos_path[128] = {0};
            memset(rtos_path, 0, sizeof(rtos_path));
            if (app_helper.convert_path_linux2rtos((const char *)cmd->item.arg_obj.data, rtos_path) < 0) {
                response->rval = -2;
                break;
            }
            if (is_file_readonly(rtos_path)) {
                response->rval = -1;
                break;
            }
            if (AmbaDCF_DeleteFile(rtos_path) != OK_UINT32) {
                response->rval = -1;
                break;
            }
        }
        break;
    case LINUX_API_CMD_LOCK_FILE:
        {
            char rtos_path[128] = {0};
            memset(rtos_path, 0, sizeof(rtos_path));
            if (app_helper.convert_path_linux2rtos((const char *)cmd->item.arg_obj.data, rtos_path) < 0) {
                response->rval = -2;
                break;
            }
            if (AmbaDCF_SetFileReadOnly(rtos_path, cmd->item.arg_obj.arg) != OK_UINT32) {
                response->rval = -1;
            }
        }
        break;
    case LINUX_API_CMD_LOCK_FILE_TIME_RANGE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_MUTE:
        pSvcUserPref->RecordSoundOnOff = cmd->item.arg_obj.arg ? OPTION_OFF : OPTION_ON;
        AmbaAudioEnc_SetMute((pSvcUserPref->RecordSoundOnOff == OPTION_ON) ? 0 : 1);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WATERMARK:
        if (cmd->item.arg_obj.arg == 0) {
            pSvcUserPref->WaterMark = WATER_MARK_NONE;
        } else if (cmd->item.arg_obj.arg == 1) {
            pSvcUserPref->WaterMark = WATER_MARK_LOGO | WATER_MARK_DATETIME | WATER_MARK_SPEED;
        } else if (cmd->item.arg_obj.arg == 2) {
            pSvcUserPref->WaterMark = WATER_MARK_LOGO | WATER_MARK_DATETIME;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WATERMARK_POS:
        if (cmd->item.arg_obj.arg == 0) {
            pSvcUserPref->WaterMarkPos = WATER_MARK_POS_BOTTOM;
        } else if (cmd->item.arg_obj.arg == 1) {
            pSvcUserPref->WaterMarkPos = WATER_MARK_POS_TOP;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_BATTERY_POWER_TIME:
        pSvcUserPref->BatteryPowerTime = cmd->item.arg_obj.arg;
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_SHUTDOWN_WAIT_TIME:
        pSvcUserPref->NoActiveShutdownTime = cmd->item.arg_obj.arg;
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_RECORD_DURATION:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        {
            UINT32 Seconds = 0;
            if (cmd->item.arg_obj.arg == 1) {
                Seconds = 120;
            } else if (cmd->item.arg_obj.arg == 2) {
                Seconds = 180;
            } else if (cmd->item.arg_obj.arg == 3) {
                Seconds = 300;
            } else if (cmd->item.arg_obj.arg == 4) {
                Seconds = 60;
            } else if (cmd->item.arg_obj.arg == 5) {
                Seconds = 20;
            } else if (cmd->item.arg_obj.arg == 6) {
                Seconds = 40;
            } else {
                response->rval = -1;
                break;
            }
            pSvcUserPref->FileSplitTimeSeconds = Seconds;
            user_setting_save();
            rec_dvr_hot_config();
        }
        break;
    case LINUX_API_CMD_SET_RECORD_QUALITY:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            int old_width = pSvcUserPref->InternalCamera.MainStream.width;
            int old_height = pSvcUserPref->InternalCamera.MainStream.height;
            if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->InternalCamera.MainStream.width = 1920;
                pSvcUserPref->InternalCamera.MainStream.height = 1080;
            } else if (cmd->item.arg_obj.arg == 3) {
                pSvcUserPref->InternalCamera.MainStream.width = 1280;
                pSvcUserPref->InternalCamera.MainStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 5) {
                pSvcUserPref->InternalCamera.MainStream.width = 3840;
                pSvcUserPref->InternalCamera.MainStream.height = 2160;
            } else if (cmd->item.arg_obj.arg == 7) {
                pSvcUserPref->InternalCamera.MainStream.width = 960;
                pSvcUserPref->InternalCamera.MainStream.height = 540;
            } else {
                response->rval = -1;
                break;
            }
            if (old_width == pSvcUserPref->InternalCamera.MainStream.width
                && old_height == pSvcUserPref->InternalCamera.MainStream.height) {
                response->rval = 0;
                break;
            }
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            int old_width = pSvcUserPref->ExternalCamera.MainStream.width;
            int old_height = pSvcUserPref->ExternalCamera.MainStream.height;
            if (cmd->item.arg_obj.arg == 0 || cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->ExternalCamera.MainStream.width = 1280;
                pSvcUserPref->ExternalCamera.MainStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 3) {
                pSvcUserPref->ExternalCamera.MainStream.width = 960;
                pSvcUserPref->ExternalCamera.MainStream.height = 540;
            } else {
                response->rval = -1;
                break;
            }
            if (old_width == pSvcUserPref->ExternalCamera.MainStream.width
                && old_height == pSvcUserPref->ExternalCamera.MainStream.height) {
                response->rval = 0;
                break;
            }
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_RECORD_FPS:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_RECORD_BITRATE:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            pSvcUserPref->InternalCamera.MainStream.bitrate = cmd->item.arg_obj.arg * 1.0 / 10;
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            pSvcUserPref->ExternalCamera.MainStream.bitrate = cmd->item.arg_obj.arg * 1.0 / 10;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_RECORD_QUALITY_AND_BITRATE:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            int old_width = pSvcUserPref->InternalCamera.MainStream.width;
            int old_height = pSvcUserPref->InternalCamera.MainStream.height;
            double old_bitrate = pSvcUserPref->InternalCamera.MainStream.bitrate;
            if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->InternalCamera.MainStream.width = 1920;
                pSvcUserPref->InternalCamera.MainStream.height = 1080;
            } else if (cmd->item.arg_obj.arg == 3) {
                pSvcUserPref->InternalCamera.MainStream.width = 1280;
                pSvcUserPref->InternalCamera.MainStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 5) {
                pSvcUserPref->InternalCamera.MainStream.width = 3840;
                pSvcUserPref->InternalCamera.MainStream.height = 2160;
            } else if (cmd->item.arg_obj.arg == 7) {
                pSvcUserPref->InternalCamera.MainStream.width = 960;
                pSvcUserPref->InternalCamera.MainStream.height = 540;
            } else {
                response->rval = -1;
                break;
            }
            pSvcUserPref->InternalCamera.MainStream.bitrate = cmd->item.arg_obj.arg2 * 1.0 / 10;
            if (old_width == pSvcUserPref->InternalCamera.MainStream.width
                && old_height == pSvcUserPref->InternalCamera.MainStream.height
                && old_bitrate == pSvcUserPref->InternalCamera.MainStream.bitrate) {
                response->rval = 0;
                break;
            }
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            int old_width = pSvcUserPref->ExternalCamera.MainStream.width;
            int old_height = pSvcUserPref->ExternalCamera.MainStream.height;
            double old_bitrate = pSvcUserPref->ExternalCamera.MainStream.bitrate;
            if (cmd->item.arg_obj.arg == 0 || cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->ExternalCamera.MainStream.width = 1280;
                pSvcUserPref->ExternalCamera.MainStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 3) {
                pSvcUserPref->ExternalCamera.MainStream.width = 960;
                pSvcUserPref->ExternalCamera.MainStream.height = 540;
            } else {
                response->rval = -1;
                break;
            }            
            pSvcUserPref->ExternalCamera.MainStream.bitrate = cmd->item.arg_obj.arg2 * 1.0 / 10;
            if (old_width == pSvcUserPref->ExternalCamera.MainStream.width
                && old_height == pSvcUserPref->ExternalCamera.MainStream.height
                && old_bitrate == pSvcUserPref->ExternalCamera.MainStream.bitrate) {
                response->rval = 0;
                break;
            }
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_PREVIEW_QUALITY:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            if (cmd->item.arg_obj.arg == 0) {
                pSvcUserPref->InternalCamera.SecStream.width = 1280;
                pSvcUserPref->InternalCamera.SecStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->InternalCamera.SecStream.width = 960;
                pSvcUserPref->InternalCamera.SecStream.height = 540;
            } else if (cmd->item.arg_obj.arg == 2) {
                pSvcUserPref->InternalCamera.SecStream.width = 1920;
                pSvcUserPref->InternalCamera.SecStream.height = 1080;
            } else {
                response->rval = -1;
                break;
            }
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            if (cmd->item.arg_obj.arg == 0) {
                pSvcUserPref->ExternalCamera.SecStream.width = 1280;
                pSvcUserPref->ExternalCamera.SecStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->ExternalCamera.SecStream.width = 960;
                pSvcUserPref->ExternalCamera.SecStream.height = 540;
            } /*else if (cmd->item.arg_obj.arg == 2) {
                pSvcUserPref->ExternalCamera.SecStream.width = 1920;
                pSvcUserPref->ExternalCamera.SecStream.height = 1080;
            } */else {
                response->rval = -1;
                break;
            }
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_PREVIEW_FPS:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_PREVIEW_BITRATE:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            pSvcUserPref->InternalCamera.SecStream.bitrate = cmd->item.arg_obj.arg * 1.0 / 10;
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            pSvcUserPref->ExternalCamera.SecStream.bitrate = cmd->item.arg_obj.arg * 1.0 / 10;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_PREVIEW_QUALITY_AND_BITRATE:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            if (cmd->item.arg_obj.arg == 0) {
                pSvcUserPref->InternalCamera.SecStream.width = 1280;
                pSvcUserPref->InternalCamera.SecStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->InternalCamera.SecStream.width = 960;
                pSvcUserPref->InternalCamera.SecStream.height = 540;
            } else if (cmd->item.arg_obj.arg == 2) {
                pSvcUserPref->InternalCamera.SecStream.width = 1920;
                pSvcUserPref->InternalCamera.SecStream.height = 1080;
            } else {
                response->rval = -1;
                break;
            }
            pSvcUserPref->InternalCamera.SecStream.bitrate = cmd->item.arg_obj.arg2 * 1.0 / 10;
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            if (cmd->item.arg_obj.arg == 0) {
                pSvcUserPref->ExternalCamera.SecStream.width = 1280;
                pSvcUserPref->ExternalCamera.SecStream.height = 720;
            } else if (cmd->item.arg_obj.arg == 1) {
                pSvcUserPref->ExternalCamera.SecStream.width = 960;
                pSvcUserPref->ExternalCamera.SecStream.height = 540;
            } /*else if (cmd->item.arg_obj.arg == 2) {
                pSvcUserPref->ExternalCamera.SecStream.width = 1920;
                pSvcUserPref->ExternalCamera.SecStream.height = 1080;
            } */else {
                response->rval = -1;
                break;
            }
            pSvcUserPref->ExternalCamera.SecStream.bitrate = cmd->item.arg_obj.arg2 * 1.0 / 10;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        rec_dvr_hot_config();
        break;
    case LINUX_API_CMD_SET_IMAGE_ROTATION:
        {
            UINT8 rotation = IMAGE_ROTATION_NONE;
            if (cmd->item.arg_obj.arg == 0) {
                rotation = IMAGE_ROTATION_NONE;
            } else if (cmd->item.arg_obj.arg == 1) {
                rotation = IMAGE_ROTATION_HORIZONTAL;
            } else if (cmd->item.arg_obj.arg == 2) {
                rotation = IMAGE_ROTATION_VERTICAL;
            } else if (cmd->item.arg_obj.arg == 3) {
                rotation = IMAGE_ROTATION_HORIZONTAL_VERTICAL;
            }
            if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
                AMBA_SENSOR_CHANNEL_s channel;
                channel.VinID = 0;
                AmbaSensor_SetRotation(&channel, rotation);
                pSvcUserPref->InternalCamera.ImageRotation = rotation;
            } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
                AMBA_SENSOR_CHANNEL_s channel;
                channel.VinID = 1;
                AmbaSensor_SetRotation(&channel, rotation);
                pSvcUserPref->ExternalCamera.ImageRotation = rotation;
            } else {
                response->rval = -1;
                break;
            }
            user_setting_save();
        }
        break;
    case LINUX_API_CMD_SET_TIME_FORMAT:
        pSvcUserPref->Use24HMode = cmd->item.arg_obj.arg ? 0 : 1;
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_TIME_ZONE:
        pSvcUserPref->TimeZone = cmd->item.arg_obj.arg;
        AmbaFS_SetUtcOffsetMinutes(pSvcUserPref->TimeZone / 60);
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_TIME_ZONE:
        response->item.timezone = pSvcUserPref->TimeZone;
        break;
    case LINUX_API_CMD_SET_DEBUG_MODE:
        pSvcUserPref->DebugMode = cmd->item.arg_obj.arg ? 1 : 0;
        if (pSvcUserPref->DebugMode == 0) {
            file_log_close();
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_IR_THRESHOLD:
        pSvcUserPref->IrThresHold = cmd->item.arg_obj.arg;
        if (pSvcUserPref->IrThresHold > 5) {
            pSvcUserPref->IrThresHold = 5;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_IR_SAMPLING:
        pSvcUserPref->IrSampling = cmd->item.arg_obj.arg;
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_RTSP_CAM:
        if (cmd->item.arg_obj.arg == 1) {
            if (pSvcUserPref->RtspLiveCamera == CAMERA_CHANNEL_INTERNAL) {
                if (app_helper.external_camera_enabled == 0) {
                    response->rval = -1;
                    break;
                }
                pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_EXTERNAL;
            } else {
                pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_INTERNAL;
            }
        } else if (cmd->item.arg_obj.arg == 2) {
            pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_INTERNAL;
        } else if (cmd->item.arg_obj.arg == 3) {
            if (app_helper.external_camera_enabled == 0) {
                response->rval = -1;
                break;
            }
            pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_EXTERNAL;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WIFI_BOOT_MODE:
        if (cmd->item.arg_obj.arg == -1) {
        } else if (cmd->item.arg_obj.arg == 0) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_AP;
        } else if (cmd->item.arg_obj.arg == 1) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_STA;
        } else if (cmd->item.arg_obj.arg == 2) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_P2P;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WIFI_CONNECT_MODE:
        if (cmd->item.arg_obj.arg == -1) {
        } else if (cmd->item.arg_obj.arg == 0) {
            pSvcUserPref->WifiSetting.ConnectMode = WIFI_MODE_AP;
        } else if (cmd->item.arg_obj.arg == 1) {
            pSvcUserPref->WifiSetting.ConnectMode = WIFI_MODE_STA;
        } else if (cmd->item.arg_obj.arg == 2) {
            pSvcUserPref->WifiSetting.ConnectMode = WIFI_MODE_P2P;
        } else {
            response->rval = -1;
            break;
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WIFI_CHANNEL:
        pSvcUserPref->WifiSetting.Channel = cmd->item.arg_obj.arg;
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WIFI_TYPE:
        pSvcUserPref->WifiSetting.Use5G = cmd->item.arg_obj.arg;
        if (strlen(cmd->item.arg_obj.data) > 0) {
            pSvcUserPref->WifiSetting.Channel = atoi(cmd->item.arg_obj.data);
        }
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_WIFI_AP_PASSWORD:
        {
            int len = strlen(cmd->item.arg_obj.data);
            if (len < 6 || len > 31) {
                response->rval = -1;
                break;
            }
            memset(pSvcUserPref->WifiSetting.ApPassword, 0, sizeof(pSvcUserPref->WifiSetting.ApPassword));
            snprintf(pSvcUserPref->WifiSetting.ApPassword, sizeof(pSvcUserPref->WifiSetting.ApPassword) - 1, "%s", cmd->item.arg_obj.data);
            user_setting_save();
        }
        break;
    case LINUX_API_CMD_SET_WIFI_STA_INFO:
        {
            int len = strlen(cmd->item.arg_obj.data2);
            if (len < 6 || len > 31) {
                response->rval = -1;
                break;
            }
            memset(pSvcUserPref->UsbWifiSetting.StaSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID));
            snprintf(pSvcUserPref->UsbWifiSetting.StaSSID, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID) - 1, "%s", cmd->item.arg_obj.data);
            memset(pSvcUserPref->UsbWifiSetting.StaPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword));
            snprintf(pSvcUserPref->UsbWifiSetting.StaPassword, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword) - 1, "%s", cmd->item.arg_obj.data2);
            user_setting_save();
        }
        break;
    case LINUX_API_CMD_SET_WIFI_P2P_WAIT_TIME:
        pSvcUserPref->WifiSetting.P2pWaitTime = cmd->item.arg_obj.arg;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_BT_IMU_STREAM_SHARE_INFO:
        bt_imu_stream_get_share_info(&response->item.stream_share_info.share_addr,
                                &response->item.stream_share_info.phy_addr,
                                &response->item.stream_share_info.share_size);
        break;
    case LINUX_API_CMD_GET_BT_GNSS_STREAM_SHARE_INFO:
        bt_gnss_stream_get_share_info(&response->item.stream_share_info.share_addr,
                                &response->item.stream_share_info.phy_addr,
                                &response->item.stream_share_info.share_size);
        break;
    case LINUX_API_CMD_SET_BT_IMU_STREAM_ENABLE:
        bt_stream_share_set_imu_stream_enable(cmd->item.enable);
        break;
    case LINUX_API_CMD_SET_BT_GNSS_STREAM_ENABLE:
        bt_stream_share_set_gnss_stream_enable(cmd->item.enable);
        break;
    case LINUX_API_CMD_DO_GSENSOR_CALIBRATE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_GSENSOR_CALIBRATION_EFFECT:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_GSENSOR_DEBUG:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_GSENSOR_WAKEUP_VALUE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_GSENSOR_IMPACT_VALUE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_GSENSOR_FREQ:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_ACCEL_ODR:
        pSvcUserPref->ImuSetting.AccelOdr = cmd->item.arg_obj.arg;
        Imu_SetAccelPara(IMU_ACCEL_ODR, pSvcUserPref->ImuSetting.AccelOdr);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_ACCEL_RANGE:
        pSvcUserPref->ImuSetting.AccelRange = cmd->item.arg_obj.arg;
        Imu_SetAccelPara(IMU_ACCEL_RANGE, pSvcUserPref->ImuSetting.AccelRange);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_GYRO_ODR:
        pSvcUserPref->ImuSetting.GyroOdr = cmd->item.arg_obj.arg;
        Imu_SetGyroPara(IMU_GYRO_ODR, pSvcUserPref->ImuSetting.GyroOdr);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_GYRO_RANGE:
        pSvcUserPref->ImuSetting.GyroRange = cmd->item.arg_obj.arg;
        Imu_SetGyroPara(IMU_GYRO_RANGE, pSvcUserPref->ImuSetting.GyroRange);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_GYRO_CALIBRATION_EFFECT:
        Imu_SetCalibration(cmd->item.arg_obj.arg);
        break;
    case LINUX_API_CMD_SET_IMU_SWAP_POLARITY:
        pSvcUserPref->ImuSetting.AxisPolarity = cmd->item.arg_obj.arg;
        Imu_SetAxisPolarity(pSvcUserPref->ImuSetting.AxisPolarity);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_PAIRING_REMINDER_MODE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_SET_PAIRING_REMINDER_VOICE:
        response->rval = -1;
        break;
    case LINUX_API_CMD_RESET_GNSS:
        gnss_set_boot_mode(cmd->item.arg_obj.arg);
        break;
    case LINUX_API_CMD_SET_GNSS_DATARATE:
        pSvcUserPref->GnssSetting.DataRate = cmd->item.arg_obj.arg;
        gnss_set_data_rate(pSvcUserPref->GnssSetting.DataRate);
        gnss_set_dsp_uart_baudrate(pSvcUserPref->GnssSetting.DataRate);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_GNSSID:
        pSvcUserPref->GnssSetting.Id = cmd->item.arg_obj.arg;
        gnss_set_satellite_id(pSvcUserPref->GnssSetting.Id);
        user_setting_save();
        break;
    case LINUX_API_CMD_SET_GNSS_MGAOFF:
        response->rval = -1;
        break;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    case LINUX_API_CMD_SET_LED_REG:
        if (app_helper.led_control) {
            app_helper.led_control = 0;
        }
        i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, cmd->item.reg_value.reg, cmd->item.reg_value.value);
        break;
    case LINUX_API_CMD_GET_LED_REG:
        {
            unsigned char value = 0;
            i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, cmd->item.reg_value.reg, &value);
            response->item.reg_value = value;
        }
        break;
#endif
    case LINUX_API_CMD_SOFT_RESET:
        if (rec_dvr_hot_config_busy_check()) {
            response->rval = -1;
            break;
        }
        SvcUserPref_ResetCameraSetting(pSvcUserPref);
        pSvcUserPref->InternalCamera.AutoRecord = 1;
        pSvcUserPref->ExternalCamera.AutoRecord = 1;
        pSvcUserPref->RecordSoundOnOff = OPTION_ON;
        user_setting_save();
        //rec_dvr_hot_config();        
        app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
        break;
    case LINUX_API_CMD_FACTORY_RESET:
        //SvcUserPref_ResetApiSetting(pSvcUserPref);
        //SvcUserPref_ResetCameraSetting(pSvcUserPref);
        //SvcUserPref_ResetNetworkSetting(pSvcUserPref);
        //user_setting_save();
        app_msg_queue_send(APP_MSG_ID_FACTORY_RESET, 0, 0, 0);
        break;
    case LINUX_API_CMD_GET_USB_LINE_CONNECTED:
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        response->rval = app_helper.check_usb_left_connected() | app_helper.check_usb_right_connected();
#else
        response->rval = 0;
#endif
        break;
    case LINUX_API_CMD_SET_BUTTON_PARAM:
        response->rval = button_task_set_time(cmd->item.button_param.x, cmd->item.button_param.y, cmd->item.button_param.z);
        break;
    case LINUX_API_CMD_GET_ADDON_TYPE:
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        {
            int value = 0;
            char *name = NULL;
            value = app_helper.get_left_addon_type();
            name = (char *)app_helper.addon_type_2_str(value);
            response->item.addon_type.left_value = value;
            memcpy(response->item.addon_type.left_name, name, strlen(name));

            value = app_helper.get_right_addon_type();
            name = (char *)app_helper.addon_type_2_str(value);
            response->item.addon_type.right_value = value;
            memcpy(response->item.addon_type.right_name, name, strlen(name));
        }
#endif
        break;
    case LINUX_API_CMD_GET_WAKEUP_SOURCE:
        {
            boot_reason_e value = Pmic_GetBootReason();
            const char *name = app_helper.wakeup_source_2_str(value);
            response->item.wakeup_source.value = value;
            memset(response->item.wakeup_source.name, 0, sizeof(response->item.wakeup_source.name));
            memcpy(response->item.wakeup_source.name, name, strlen(name));
        }
        break;
    case LINUX_API_CMD_SET_MCU_WAKEUP_TIME:
        debug_line("set mcu wakeup time: %dminutes", cmd->item.mcu_wakeup_time);
        pSvcUserPref->PowerOffWakeupMinutes = cmd->item.mcu_wakeup_time;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_MCU_WAKEUP_TIME:
        response->item.mcu_wakeup_time = pSvcUserPref->PowerOffWakeupMinutes;
        break;
    case LINUX_API_CMD_SET_POWER_OFF_MODE:
        debug_line("set power off mode: %d", cmd->item.power_off_mode);
        break;
    case LINUX_API_CMD_GET_STREAM_SHARE_INFO:
        response->rval = stream_share_get_info(
                                &response->item.stream_share_info.share_addr,
                                &response->item.stream_share_info.phy_addr,
                                &response->item.stream_share_info.share_size);
        break;
    case LINUX_API_CMD_SET_STREAM_SHARE_ENABLE:
        response->rval = stream_share_set_enable(cmd->item.stream_param.index, cmd->item.stream_param.enable);
        break;
    case LINUX_API_CMD_GET_FACTORY_BURN_INFO:
        {
            metadata_s *metadata = NULL;
            metadata_get(&metadata);
            memcpy(response->item.burn_info.wifi_mac, metadata->WifiMac, 6);
            memcpy(response->item.burn_info.bt_mac, metadata->BtMac, 6);
        }
        break;
    case LINUX_API_CMD_GET_RTOS_LOG_SHARE_INFO:
        response->rval = rtos_log_share_get_info(
                                &response->item.stream_share_info.share_addr,
                                &response->item.stream_share_info.phy_addr,
                                &response->item.stream_share_info.share_size);
        break;
    case LINUX_API_CMD_SET_RTOS_LOG_SHARE_ENABLE:
        response->rval = rtos_log_share_set_enable(cmd->item.enable);
        break;
    case LINUX_API_CMD_GET_IMU_FIFO_SHARE_INFO:
        response->rval = imu_task_get_fifo_share_info(
                                &response->item.stream_share_info.share_addr,
                                &response->item.stream_share_info.phy_addr,
                                &response->item.stream_share_info.share_size);
        break;
    case LINUX_API_CMD_GET_IMU_FIFO_DATA:
        response->rval = imu_task_get_fifo_data(cmd->item.max_frame_num, &response->item.frame_num);
        break;
    case LINUX_API_CMD_GET_POWER_SOURCE:
        {
            POWER_SOURCE_e value = app_helper.get_power_source();
            const char *name = app_helper.power_source_2_str(value);
            response->item.power_source.value = value;
            memset(response->item.power_source.name, 0, sizeof(response->item.power_source.name));
            memcpy(response->item.power_source.name, name, strlen(name));
        }
        break;
    case LINUX_API_CMD_GET_BASIC_INFO:
        {
            basic_info_s *basic_info = &response->item.basic_info;
            device_info_s *device_info =  app_helper.get_device_info();
            metadata_s *metadata = NULL;
            POWER_SOURCE_e power_source_value = app_helper.get_power_source();
            const char *power_source_name = app_helper.power_source_2_str(power_source_value);
            boot_reason_e wakeup_source_value = Pmic_GetBootReason();
            const char *wakeup_source_name = app_helper.wakeup_source_2_str(wakeup_source_value);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            char *name = NULL;
#endif
            TEMP_DATA_s temp_data;

            metadata_get(&metadata);
            memset(basic_info, 0, sizeof(basic_info_s));

#if defined(CONFIG_BSP_CV25_NEXAR_D161)
            snprintf(basic_info->model, sizeof(basic_info->model) - 1, "%s", "N1V1");
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            snprintf(basic_info->model, sizeof(basic_info->model) - 1, "%s", "N1V2");
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
            if (app_helper.check_d080_with_2nd_sensor()) {
                snprintf(basic_info->model, sizeof(basic_info->model) - 1, "%s", "Beam2K+");
            } else {
                snprintf(basic_info->model, sizeof(basic_info->model) - 1, "%s", "Beam2K");
            }
#endif
            snprintf(basic_info->cpu_id, sizeof(basic_info->cpu_id) - 1, "%s", (char *)metadata->CpuId);
            snprintf(basic_info->device_sn, sizeof(basic_info->device_sn) - 1, "%s", (char *)metadata->SerialNumber);
            memcpy(basic_info->wifi_mac, metadata->WifiMac, 6);
            memcpy(basic_info->bt_mac, metadata->BtMac, 6);
            snprintf(basic_info->sw_version, sizeof(basic_info->sw_version) - 1, "%s", (char *)device_info->sw_version);
            snprintf(basic_info->build_time, sizeof(basic_info->build_time) - 1, "%s", (char *)device_info->build_time);
            snprintf(basic_info->mcu_main_version, sizeof(basic_info->mcu_main_version) - 1, "%s", (char *)device_info->mcu_version);
            snprintf(basic_info->mcu_lte_version, sizeof(basic_info->mcu_lte_version) - 1, "%s", (char *)device_info->mcu_version2);
            snprintf(basic_info->imu_type, sizeof(basic_info->imu_type) - 1, "%s", Imu_GetName());
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            // left
            basic_info->addon_type.left_value = app_helper.get_left_addon_type();
            name = (char *)app_helper.addon_type_2_str(basic_info->addon_type.left_value);
            memcpy(basic_info->addon_type.left_name, name, strlen(name));
            // right
            basic_info->addon_type.right_value = app_helper.get_right_addon_type();
            name = (char *)app_helper.addon_type_2_str(basic_info->addon_type.right_value);
            memcpy(basic_info->addon_type.right_name, name, strlen(name));
#endif
            if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
                if (sd_bad_or_not_exist) {
                    basic_info->sd_card_info.status = 2;
                } else {
                    unsigned int free = 0, total = 0;
                    app_helper.get_sd_space(&total, &free);
                    basic_info->sd_card_info.status = 1;
                    basic_info->sd_card_info.used_mb = total - free;
                    basic_info->sd_card_info.total_mb = total;
                    basic_info->sd_card_info.free_mb = free;
                }
            } else {
                basic_info->sd_card_info.status = 0;
            }
            basic_info->wakeup_source.value = wakeup_source_value;
            memcpy(basic_info->wakeup_source.name, wakeup_source_name, strlen(wakeup_source_name));
            basic_info->power_source.value = power_source_value;
            memcpy(basic_info->power_source.name, power_source_name, strlen(power_source_name));
            basic_info->battery_info.adc = battery_task_get_adc();
            basic_info->battery_info.raw_adc = battery_task_get_raw_adc();
            basic_info->battery_info.mcu_adc = battery_task_get_mcu_adc();
            basic_info->battery_info.percentage = battery_task_get_percentage();
            basic_info->battery_info.voltage_mv = battery_task_get_voltage_mv();
            battery_task_check_charger_ic_stat(&(basic_info->battery_info.charger_ic_is_charge),
                                                    &(basic_info->battery_info.charger_ic_power_good),
                                                    &(basic_info->battery_info.charger_ic_thermal_protect),
                                                    NULL);
            Imu_GetTemperature(&temp_data);
            basic_info->battery_info.imu_temperature = temp_data.temp_float;
            basic_info->battery_info.cable_adc = battery_task_get_cable_adc();
            basic_info->factory_reset = app_helper.factory_reset_flag ? 1 : 0;
            basic_info->hard_reset = app_helper.hard_reset_flag;
            basic_info->acc_state = Pmic_GetAccState();
        }
        break;
    case LINUX_API_CMD_POWER_OFF:
        app_msg_queue_send(APP_MSG_ID_POWER_BUTTON, 0, 0, 0);
        break;
    case LINUX_API_CMD_PLAY_BEEP_ID:
        response->rval = beep_play(cmd->item.beep_id);
        break;
    case LINUX_API_CMD_SET_LED_COLOR:
        pSvcUserPref->LedColor.R = cmd->item.led_color.R;
        pSvcUserPref->LedColor.G = cmd->item.led_color.G;
        pSvcUserPref->LedColor.B = cmd->item.led_color.B;
        app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_LED_COLOR:
        response->item.led_color.R = pSvcUserPref->LedColor.R;
        response->item.led_color.G = pSvcUserPref->LedColor.G;
        response->item.led_color.B = pSvcUserPref->LedColor.B;
        break;
    case LINUX_API_CMD_SET_LED_RTOS_CONTROL_ENABLE:
        pSvcUserPref->LedRtosControl = cmd->item.enable ? OPTION_ON : OPTION_OFF;        
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_LED_RTOS_CONTROL_ENABLE:
        response->item.enable = (pSvcUserPref->LedRtosControl == OPTION_ON) ? 1 : 0;
        break;
    case LINUX_API_CMD_SET_USB_MASS_STORAGE_ENABLE:
        pSvcUserPref->UsbMassStorage = cmd->item.enable ? OPTION_ON : OPTION_OFF;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_USB_MASS_STORAGE_ENABLE:        
        response->item.enable = (pSvcUserPref->UsbMassStorage == OPTION_ON) ? 1 : 0;
        break;
    case LINUX_API_CMD_SET_SPEED_UNIT:
        pSvcUserPref->SpeedUnit = cmd->item.speed_unit;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_SPEED_UNIT:
        response->item.speed_unit = pSvcUserPref->SpeedUnit;
        break;
    case LINUX_API_CMD_SET_POWER_ONOFF_SOUND_ENABLE:
        pSvcUserPref->PowerOnOffSound = cmd->item.enable ? OPTION_ON : OPTION_OFF;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_POWER_ONOFF_SOUND_ENABLE:
        response->item.enable = (pSvcUserPref->PowerOnOffSound == OPTION_ON) ? 1 : 0;
        break;
    case LINUX_API_CMD_SET_VIDEO_ENCRYPTION_ENABLE:
        pSvcUserPref->VideoEncryption.enable = cmd->item.enable ? 1 : 0;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_VIDEO_ENCRYPTION_ENABLE:
        response->item.enable = pSvcUserPref->VideoEncryption.enable;
        break;
    case LINUX_API_CMD_SET_VIDEO_ENCRYPTION_KEY:
        memset(pSvcUserPref->VideoEncryption.encryption_key, 0, sizeof(pSvcUserPref->VideoEncryption.encryption_key));
        memcpy(pSvcUserPref->VideoEncryption.encryption_key, cmd->item.key, strlen(cmd->item.key));        
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_VIDEO_ENCRYPTION_KEY:
        memset(response->item.key, 0, sizeof(response->item.key));
        memcpy(response->item.key, pSvcUserPref->VideoEncryption.encryption_key, strlen(pSvcUserPref->VideoEncryption.encryption_key));
        break;
    case LINUX_API_CMD_SET_AUTHORIZATION_KEY:
        memset(pSvcUserPref->AuthorizationToken, 0, sizeof(pSvcUserPref->AuthorizationToken));
        memcpy(pSvcUserPref->AuthorizationToken, cmd->item.key, strlen(cmd->item.key));
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_AUTHORIZATION_KEY:
        memset(response->item.key, 0, sizeof(response->item.key));
        memcpy(response->item.key, pSvcUserPref->AuthorizationToken, strlen(pSvcUserPref->AuthorizationToken));
        break;
    case LINUX_API_CMD_SET_CAMERA_AUTO_RECORD:
        if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_INTERNAL) {
            pSvcUserPref->InternalCamera.AutoRecord = cmd->item.arg_obj.arg ? 1 : 0;            
            user_setting_save();
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_EXTERNAL) {
            pSvcUserPref->ExternalCamera.AutoRecord = cmd->item.arg_obj.arg ? 1 : 0;            
            user_setting_save();
        } else if (cmd->item.arg_obj.channel == CAMERA_CHANNEL_BOTH) {
            pSvcUserPref->InternalCamera.AutoRecord = cmd->item.arg_obj.arg ? 1 : 0;
            pSvcUserPref->ExternalCamera.AutoRecord = cmd->item.arg_obj.arg ? 1 : 0;            
            user_setting_save();
        } else {
            response->rval = -1;
            break;
        }
        break;
    case LINUX_API_CMD_GET_CAMERA_AUTO_RECORD:
        response->item.enable = 0;
        if (pSvcUserPref->InternalCamera.AutoRecord) {
            response->item.enable |= 0x01;
        }
        if (pSvcUserPref->ExternalCamera.AutoRecord) {
            response->item.enable |= 0x02;
        }
        break;
    case LINUX_API_CMD_SET_MIC:
        if (cmd->item.mic_type == MIC_TYPE_AMIC) {
            AmbaAudio_CodecSetInput(0, AUCODEC_AMIC_IN);
        } else if (cmd->item.mic_type == MIC_TYPE_DMIC) {
            AmbaAudio_CodecSetInput(0, AUCODEC_DMIC_IN);
        } else if (cmd->item.mic_type == MIC_TYPE_DMIC2) {
            AmbaAudio_CodecSetInput(0, AUCODEC_DMIC2_IN);
        } else {
            response->rval = -1;
        }  
        break;
    case LINUX_API_CMD_GET_MIC:
        {
            UINT32 type = AmbaAudio_CodecGetMicType(0);
            if (type == AUCODEC_AMIC_IN) {
                response->item.mic_type = MIC_TYPE_AMIC;
            } else if (type == AUCODEC_DMIC_IN) {
                response->item.mic_type = MIC_TYPE_DMIC;
            } else if (type == AUCODEC_DMIC2_IN) {
                response->item.mic_type = MIC_TYPE_DMIC2;
            } else {
                response->rval = -1;
            }
        }
        break;
    case LINUX_API_CMD_SET_PARKING_LEVEL:
        pSvcUserPref->ParkingLevel = cmd->item.parking_level;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_PARKING_LEVEL:
        response->item.parking_level = pSvcUserPref->ParkingLevel;
        break;
    case LINUX_API_CMD_SET_LTE_WAKEUP_ENABLE:
        pSvcUserPref->EnableLteRemoteWakeup = cmd->item.enable ? 1 : 0;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_LTE_WAKEUP_ENABLE:
        response->item.enable = pSvcUserPref->EnableLteRemoteWakeup ? 1 : 0;
        break;
    case LINUX_API_CMD_SET_API_FILES_ENABLE:
        pSvcUserPref->ApiSettings.files_enable = cmd->item.enable ? 1 : 0;
        user_setting_save();
        break;
    case LINUX_API_CMD_GET_API_FILES_ENABLE:        
        response->item.enable = pSvcUserPref->ApiSettings.files_enable ? 1 : 0;
        break;
    case LINUX_API_CMD_MCU_UPDATE_START:
        mcu_update_set_run();
        break;
    case LINUX_API_CMD_RESTART_LTE_USB_WIFI_MODULE:
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        {
            unsigned char value = 0, new_value = 0;
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            new_value = value;
            if (app_helper.check_lte_left_connected()) {
                new_value &= ~0x04;
            } else if (app_helper.check_lte_right_connected()) {
                new_value |= 0x04;
            } else {
                response->rval = -1;
                break;
            }
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, new_value);
            msleep(500);
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
        }
#else
        response->rval = -1;
#endif
        break;
    case LINUX_API_CMD_GET_ACC_STATE:
        response->item.acc_state = Pmic_GetAccState();
        break;
    case LINUX_API_CMD_CANCEL_SHUTDOWN:
        response->rval = app_util_cancel_power_off();
        break;
    case LINUX_API_CMD_SET_LTE_POWER:
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (app_helper.check_lte_left_connected())  {
            unsigned char value = 0;
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            if (cmd->item.enable) {
                value |= 0x01;
            } else {
                value &= ~0x01;
            }
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);            
            msleep(50);
            if (cmd->item.enable) {
                //turn on lte board mcu
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
                unsigned int i2c_channel = get_lte_i2c_channel();
#else
                unsigned int i2c_channel = LTE_MCU_I2C_CHANNEL;
#endif
                i2c_write_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x03, 0x0F);
            }
        } else if (app_helper.check_lte_right_connected()) {
            unsigned char value = 0;
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            if (cmd->item.enable) {
                value |= 0x02;
            } else {
                value &= ~0x02;
            }
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);            
            msleep(50);
            if (cmd->item.enable) {
                //turn on lte board mcu
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
                unsigned int i2c_channel = get_lte_i2c_channel();
#else
                unsigned int i2c_channel = LTE_MCU_I2C_CHANNEL;
#endif
                i2c_write_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x03, 0x0F);
            }
        } else
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
        if (cmd->item.enable == 0) {//power off
            unsigned char value = 0;
            debug_line("lte power off");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value &= ~0x02;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
            app_helper.gpio_set(GPIO_PIN_75, 0);//4g_en
            app_helper.gpio_set(GPIO_PIN_41, 0);//4g_vbus_en
            app_helper.gpio_set(GPIO_PIN_37, 0);//4g_pwr_key
        } else if (cmd->item.enable == 1) {//power on
            unsigned char value = 0;
            debug_line("lte power on");
            app_helper.gpio_set(GPIO_PIN_37, 1);//4g_pwr_key
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value |= 0x02;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
            app_helper.gpio_set(GPIO_PIN_75, 1);//4g_en
            app_helper.gpio_set(GPIO_PIN_41, 1);//4g_vbus_en
            //enable pwr_key
            app_helper.gpio_set(GPIO_PIN_37, 0);//4g_pwr_key
            msleep(500);
            app_helper.gpio_set(GPIO_PIN_37, 1);//4g_pwr_key
        } else
#endif
		{
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_CHECK_LTE_WORKING:
        response->item.active = app_helper.lte_on ? 1 : 0;
        break;
    case LINUX_API_CMD_CHECK_SENSOR_WORKING:
        if (product_line_cmd_camera_check(cmd->item.arg_obj.channel) >= 0) {
            response->item.active = 1;
        } else {
            response->item.active = 0;
        }
        break;
    case LINUX_API_CMD_LTE_ENTER_WAKEUP_MODE:
        if (app_helper.lte_booted == 0 || (tick() - app_helper.lte_booted_tick) < 10000) {
            response->rval = -1;
            break;
        }
        app_timer_register(TIMER_1HZ, app_timer_lte_enter_wakeup_handler);
        break;
    case LINUX_API_CMD_SET_CV_ENABLE:
        pSvcUserPref->CvOnOff = cmd->item.enable ? OPTION_ON : OPTION_OFF;
        user_setting_save();
        app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
        break;
    case LINUX_API_CMD_GET_CV_ENABLE:
        response->item.active = (pSvcUserPref->CvOnOff == OPTION_ON);
        break;
    case LINUX_API_CMD_EXEC_SHELL_COMMAND:
        AmbaShell_ExecThirdCommand(cmd->item.cmdline);
        break;
    case LINUX_API_CMD_GET_BATTERY_INFO:
        {
            basic_info_s *basic_info = &response->item.basic_info;
            TEMP_DATA_s temp_data;
            POWER_SOURCE_e power_source_value = app_helper.get_power_source();
            const char *power_source_name = app_helper.power_source_2_str(power_source_value);
            basic_info->power_source.value = power_source_value;
            memcpy(basic_info->power_source.name, power_source_name, strlen(power_source_name));
            basic_info->battery_info.adc = battery_task_get_adc();
            basic_info->battery_info.raw_adc = battery_task_get_raw_adc();
            basic_info->battery_info.mcu_adc = battery_task_get_mcu_adc();
            basic_info->battery_info.percentage = battery_task_get_percentage();
            basic_info->battery_info.voltage_mv = battery_task_get_voltage_mv();
            battery_task_check_charger_ic_stat(&(basic_info->battery_info.charger_ic_is_charge),
                                                    &(basic_info->battery_info.charger_ic_power_good),
                                                    &(basic_info->battery_info.charger_ic_thermal_protect),
                                                    NULL);
            Imu_GetTemperature(&temp_data);
            basic_info->battery_info.imu_temperature = temp_data.temp_float;
            basic_info->battery_info.cable_adc = battery_task_get_cable_adc();
        }
        break;
    case LINUX_API_CMD_GET_HARD_RESET_FLAG:
    	response->item.enable = app_helper.hard_reset_flag;
    	break;
#if defined(CONFIG_BSP_CV25_NEXAR_D080) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
	case LINUX_API_CMD_SET_GNSS_POWER:
        if (cmd->item.enable == 0) {//power off
            unsigned char value = 0;
            debug_line("gnss power off");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value &= ~0x01;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
        } else if (cmd->item.enable == 1) {//power on
            unsigned char value = 0;
            debug_line("gnss power on");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value |= 0x01;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
        } else if (cmd->item.enable == 2) {//reset
            unsigned char value = 0;
            debug_line("gnss power reset");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);            
            value &= ~0x01;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
            msleep(50);
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value |= 0x01;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
        } else {
            response->rval = -1;
        }
        break;
#endif
    case LINUX_API_CMD_SET_STATE_LED_POWER:
        debug_line("state led power %s", cmd->item.enable ? "on" : "off");
        app_helper.set_led_enable(cmd->item.enable);
        break;
    case LINUX_API_CMD_SET_IR_LED_POWER:
        if (app_helper.external_camera_enabled && cmd->item.enable == 0) {//power off
            debug_line("ir led disable");
            app_helper.set_ir_brightness(0);
            app_helper.lock_ir_led(1);
            rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_EXTERNAL, 0);
        } else if (app_helper.external_camera_enabled && cmd->item.enable == 1) {//power on
            debug_line("ir led enable");
            app_helper.lock_ir_led(0);
        } else {
            response->rval = -1;
        }
        break;
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    case LINUX_API_CMD_SET_BT_POWER:
        if (cmd->item.enable == 0) {//power off
            unsigned char value = 0;
            debug_line("bt power off");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value &= ~0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
        } else if (cmd->item.enable == 1) {//power on
            unsigned char value = 0;
            debug_line("bt power on");
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
            value |= 0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
        } else {
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_SET_LOGO_LED_POWER:
        if (cmd->item.enable == 0) {//power off
            debug_line("logo led power off");
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x00, 0x00);
        } else if (cmd->item.enable == 1) {//power on
            debug_line("logo led power on");
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x00, 0x01);
        } else {
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_SET_CHARGE_ENABLE:
        if (cmd->item.enable == 0) {//power off
            unsigned char value = 0;
            debug_line("charge enable");
            i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, &value);
            value &= ~0x08;
            i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, value);
        } else if (cmd->item.enable == 1) {//power on
            unsigned char value = 0;
            debug_line("charge disable");
            i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, &value);
            value |= 0x08;
            i2c_write_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, value);
        } else {
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_SET_IMU_ENABLE:
        if (cmd->item.enable == 0) {//power off
            debug_line("imu disable");
            Imu_SetEnable(0);
        } else if (cmd->item.enable == 1) {//power on
            debug_line("imu enable");
            Imu_SetEnable(1);
        } else {
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_SET_ROAD_CAMERA_ENABLE:
        if (cmd->item.enable == 0) {//power off
            debug_line("road camera disable");
            app_helper.internal_camera_enabled = 0;
            app_helper.internal_camera_auto_record = 0;
            SvcRecMain_Stop(0x3, 0);
            app_helper.gpio_set(GPIO_PIN_0, 0);
            app_helper.gpio_set(GPIO_PIN_95, 0);
            app_helper.gpio_set(GPIO_PIN_16, 0);
        } else {
            response->rval = -1;
        }
        break;
    case LINUX_API_CMD_SET_CABIN_CAMERA_ENABLE:
        if (cmd->item.enable == 0) {//power off
            debug_line("cabin camera disable");
            app_helper.external_camera_connected = 0;
            app_helper.external_camera_enabled = 0;
            app_helper.external_camera_auto_record = 0;
            ir_task_set_check_enable(0);
            SvcRecMain_Stop(0x4, 0);
            app_helper.gpio_set(GPIO_PIN_107, 0);
            app_helper.gpio_set(GPIO_PIN_106, 0);
            app_helper.gpio_set(GPIO_PIN_15, 0);
        } else {
            response->rval = -1;
        }
        break;
#endif
    default:
        break;
    }
    end_tick = tick();
    if (debug) debug_line("handler[%s] use time: %dms, rval=%d", cmd_info[i].cmd_name, end_tick - start_tick, response->rval);
end:
    pRet->Length = sizeof(linux_api_response_s);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void linux_api_service2_msg_handler(linux_api_cmd_s *cmd, AMBA_IPC_SVC_RESULT_s *pRet)
{
    linux_api_response_s *response = NULL;
    unsigned int i = 0;
    //unsigned int start_tick = 0, end_tick = 0;

    AmbaMisra_TypeCast32(&response, &(pRet->pResult));
    response->msg_id = cmd->msg_id;
    for (i = 0; i < sizeof(cmd_info2) / sizeof(cmd_info_s); i++) {
        if (cmd_info2[i].cmd_id == cmd->msg_id) {
            //if (debug) debug_line("linux api service msg_id=%d, %s", cmd->msg_id, cmd_info2[i].cmd_name);
            break;
        }
    }
    if (i >= (sizeof(cmd_info2) / sizeof(cmd_info_s))) {
        debug_line("linux api service msg_id=%d, %s", cmd->msg_id, "not support");
        response->rval = -1;
        goto end;
    }
    response->rval = 0;
    //start_tick = tick();
    switch (cmd->msg_id) {
    case LINUX_API_CMD_GET_IMU_FIFO_DATA:
        response->rval = imu_task_get_fifo_data(cmd->item.max_frame_num, &response->item.frame_num);
        break;
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
    case LINUX_API_CMD_SET_TIME:
        {
            cur_timestamp_s timestamp = cmd->item.cur_timestamp;
            if (timestamp.tv_usec > 999999) {
                response->rval = -1;
                goto end;
            }
            gnss_set_time(timestamp.tv_sec, timestamp.tv_usec);
        }
        break;
    case LINUX_API_CMD_GET_TIME:
        {
            linux_time_s time;
            app_helper.get_linux_time(&time);
            response->item.cur_timestamp.tv_sec = time.sec;
            response->item.cur_timestamp.tv_usec = time.usec;
        }
        break;
#endif
    default:
        break;
    }
    //end_tick = tick();
    //if (debug) debug_line("handler[%s] use time: %dms, rval=%d", cmd_info2[i].cmd_name, end_tick - start_tick, response->rval);
end:
    pRet->Length = sizeof(linux_api_response_s);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}


