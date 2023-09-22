#include "ini_helper.h"
#include "user_setting.h"
#include "AmbaImg_AaaDef.h"
#if 1//defined(CONFIG_IMU_BMI160)
#if defined(CONFIG_BSP_H32_NEXAR_D081)
#include "../../../../../bsp/h32/peripherals/imu/bmi160_defs.h"
#else
#include "../../../../../bsp/cv25/peripherals/imu/bmi160_defs.h"
#endif
#endif

static ini_section_item_s ini_internal_camera_section_items[INI_SECTION_ITEM_INTERNAL_CAMERA_NUM] = {
    [INI_SECTION_ITEM_INTERNAL_CAMERA_ENABLE] = {
        "enable",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_AUTO_RECORD] = {
        "auto_record",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_FPS] = {
        "fps",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_SIZE] = {
        "record_size",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_SIZE] = {
        "preview_size",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_BITRATE] = {
        "record_bitrate",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_BITRATE] = {
        "preview_bitrate",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_GOP] = {
        "record_gop",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_GOP] = {
        "preview_gop",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_INTERNAL_CAMERA_IMAGE_ROTATION] = {
        "image_rotation",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_external_camera_section_items[INI_SECTION_ITEM_EXTERNAL_CAMERA_NUM] = {
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_ENABLE] = {
        "enable",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_AUTO_RECORD] = {
        "auto_record",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_FPS] = {
        "fps",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_SIZE] = {
        "record_size",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_SIZE] = {
        "preview_size",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_BITRATE] = {
        "record_bitrate",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_BITRATE] = {
        "preview_bitrate",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_GOP] = {
        "record_gop",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_GOP] = {
        "preview_gop",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_EXTERNAL_CAMERA_IMAGE_ROTATION] = {
        "image_rotation",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_system_section_items[INI_SECTION_ITEM_SYSTEM_NUM] = {
    [INI_SECTION_ITEM_SYSTEM_VER] = {
        "version",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_AUDIO_VOLUME] = {
        "audio_volume",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_RECORD_SOUND] = {
        "record_sound",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_LED_ONOFF] = {
        "led_on",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_SET_LED_BRIGHTNESS] = {
        "led_brightness",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_IR_BRIGHTNESS] = {
        "ir_brightness",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_WATCHDOG_ONOFF] = {
        "watchdog_on",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_EVENT_BUFFER_LENGTH] = {
        "event_buffer_length",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_DEBUG_MODE] = {
        "debug_mode",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_TIME_ZONE] = {
        "time_zone",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_WATERMARK] = {
        "watermark",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_WATERMARK_POS] = {
        "watermark_pos",
        0,
        {
            {"top", WATER_MARK_POS_TOP},
            {"bottom", WATER_MARK_POS_BOTTOM},
        }
    },
    [INI_SECTION_ITEM_SYSTEM_SPEED_UNIT] = {
        "speed_unit",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_RTSP_LIVE_CAMERA] = {
        "rtsp_live",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_IR_SAMPLING] = {
        "ir_sampling",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_IR_THRESHOLD] = {
        "ir_threshold",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_MOTION_DETECT] = {
        "motion_detect",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_AWB_MODE] = {
        "awb_mode",
        0,
        {
            {"auto", WB_AUTOMATIC},
            {"sunny", WB_SUNNY},
            {"cloudy", WB_CLOUDY},
            {"incandescent", WB_INCANDESCENT},
            {"fluorescent", WB_FLUORESCENT},
        }
    },
    [INI_SECTION_ITEM_SYSTEM_AE_BIAS] = {
        "ae_bias",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_POWER_ONOFF_SOUND] = {
        "power_onoff_sound",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_NO_ACTIVE_SHUTDOWN_TIME] = {
        "no_active_shutdown_seconds",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_USE_24H_MODE] = {
        "use_24h_mode",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_IMU_RECORD] = {
        "imu_record",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_GNSS_RECORD] = {
        "gnss_record",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_LINUX_BOOT] = {
        "linux_boot",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_LINUX_CONSOLE] = {
        "linux_console",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_RTOS_CONSOLE] = {
        "rtos_console",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_USB_MASS_STORAGE] = {
        "usb_mass_storage",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_LED_RTOS_CONTROL] = {
        "led_rtos_control",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_KEY] = {
        "encryption_key",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_ENABLE] = {
        "encryption_enable",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_DEBUG] = {
        "encryption_debug",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_VIDEO_DECRYPTION_KEY] = {
        "decryption_key",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_AUTHORIZATION_TOKEN] = {
        "authorization_token",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_PARKING_LEVEL] = {
        "parking_level",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_LTE_WAKEUP_ENABLE] = {
        "lte_wakeup",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_CV_RUN] = {
        "cv_run",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_PIV] = {
        "piv_func",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_SYSTEM_H265_FORMAT] = {
        "h265_format",
        1,
        {{{0}, 0}}
    }
};

static ini_section_item_s ini_wifi_section_items[INI_SECTION_ITEM_WIFI_NUM] = {
    [INI_SECTION_ITEM_WIFI_BOOT_MODE] = {
        "boot_mode",
        0,
        {
            {"ap", WIFI_MODE_AP},
            {"sta", WIFI_MODE_STA},
            {"p2p", WIFI_MODE_P2P},
        }
    },
    [INI_SECTION_ITEM_WIFI_CONNECT_MODE] = {
        "connect_mode",
        0,
        {
            {"ap", WIFI_MODE_AP},
            {"sta", WIFI_MODE_STA},
            {"p2p", WIFI_MODE_P2P},
        }
    },
    [INI_SECTION_ITEM_WIFI_CHANNEL] = {
        "wifi_channel",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_MODE] = {
        "wifi_mode",
        0,
        {
            {"2.4g", 0},
            {"5g", 1},
        }
    },
    [INI_SECTION_ITEM_WIFI_AP_SSID] = {
        "ap_post_ssid",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_AP_PASSWORD] = {
        "ap_password",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_AP_IP] = {
        "local_ip",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_STA_SSID] = {
        "sta_ssid",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_STA_PASSWORD] = {
        "sta_password",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_WIFI_P2P_WAIT_TIME] = {
        "p2p_wait_time",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_BOOT_MODE] = {
        "usb_boot_mode",
        0,
        {
            {"ap", WIFI_MODE_AP},
            {"sta", WIFI_MODE_STA},
            {"p2p", WIFI_MODE_P2P},
        }
    },
    [INI_SECTION_ITEM_USB_WIFI_CHANNEL] = {
        "usb_wifi_channel",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_AP_SSID] = {
        "usb_ap_post_ssid",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_AP_PASSWORD] = {
        "usb_ap_password",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_AP_IP] = {
        "usb_local_ip",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_STA_SSID] = {
        "usb_sta_ssid",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_USB_WIFI_STA_PASSWORD] = {
        "usb_sta_password",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_gnss_section_items[INI_SECTION_ITEM_GNSS_NUM] = {
    [INI_SECTION_ITEM_GNSS_UART_BITRATE] = {
        "uart_bitrate",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_GNSS_UART_CHANNEL] = {
        "uart_channel",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_GNSS_ID] = {
        "gnss_id",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_GNSS_DATARATE] = {
        "datarate",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_imu_section_items[INI_SECTION_ITEM_IMU_NUM] = {
    [INI_SECTION_ITEM_IMU_ACCEL_ODR] = {
        "accel_odr",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_IMU_ACCEL_RANGE] = {
        "accel_range",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_IMU_GYRO_ODR] = {
        "gyro_odr",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_IMU_GYRO_RANGE] = {
        "gyro_range",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_IMU_AXIS_POLARITY] = {
        "axis_polarity",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_led_section_items[INI_SECTION_ITEM_LED_NUM] = {
    [INI_SECTION_ITEM_LED_R] = {
        "r",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_LED_G] = {
        "g",
        1,
        {{{0}, 0}}
    },
    [INI_SECTION_ITEM_LED_B] = {
        "b",
        1,
        {{{0}, 0}}
    },
};

static ini_section_item_s ini_api_section_items[INI_SECTION_ITEM_LED_NUM] = {
    [INI_SECTION_ITEM_API_FILES] = {
        "files",
        1,
        {{{0}, 0}}
    },
};

static ini_section_s ini_settings[] = {
    [INI_SECTION_INTERNAL_CAMERA] = {
        "Internal",
        ini_internal_camera_section_items,
        sizeof(ini_internal_camera_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_EXTERNAL_CAMERA] = {
        "External",
        ini_external_camera_section_items,
        sizeof(ini_external_camera_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_SYSTEM] = {
        "System",
        ini_system_section_items,
        sizeof(ini_system_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_WIFI] = {
        "WiFi",
        ini_wifi_section_items,
        sizeof(ini_wifi_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_GNSS] = {
        "Gnss",
        ini_gnss_section_items,
        sizeof(ini_gnss_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_IMU] = {
        "Imu",
        ini_imu_section_items,
        sizeof(ini_imu_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_LED] = {
        "Led",
        ini_led_section_items,
        sizeof(ini_led_section_items) / sizeof(ini_section_item_s)
    },
    [INI_SECTION_API] = {
        "Api",
        ini_api_section_items,
        sizeof(ini_api_section_items) / sizeof(ini_section_item_s)
    },
};

static void split_size(const char *buf, stream_size_s *size)
{
    int xing_index = -1;
    int i = 0;
    char tmp[32] = {0};

    if (buf == NULL || size == NULL) {
        return;
    }

    for (i = 0; i < (int)strlen(buf); i++) {
        if (buf[i] == '*' || buf[i] == 'x') {
            xing_index = i;
            break;
        }
    }
    if (xing_index <= 0) {
        return;
    }
    if ((strlen(buf) - xing_index - 1) <= 0) {
        return;
    }
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, buf, xing_index);
    size->width = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, buf + xing_index + 1, strlen(buf) - xing_index - 1);
    size->height = atoi(tmp);
}

static int user_setting_section_item_handler(int section_index, int item_index, const char *value)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref == NULL) {
        debug_line("%s get pref fail", __func__);
        return -1;
    }
    if (section_index == INI_SECTION_INTERNAL_CAMERA) {
        switch (item_index) {
        case INI_SECTION_ITEM_INTERNAL_CAMERA_AUTO_RECORD:
            debug_line("internal camera auto_record=%s", value);
            pSvcUserPref->InternalCamera.AutoRecord = atoi(value) ? 1 : 0;
            break;
        //case INI_SECTION_ITEM_INTERNAL_CAMERA_FPS:
          //  debug_line("internal camera fps=%s", value);
            //pSvcUserPref->InternalCamera.Fps = atoi(value);
            //break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_SIZE:
            debug_line("internal camera record_size=%s", value);
            split_size(value, &pSvcUserPref->InternalCamera.MainStream);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_SIZE:
            debug_line("internal camera preview_size=%s", value);
            split_size(value, &pSvcUserPref->InternalCamera.SecStream);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_BITRATE:
            debug_line("internal camera record_bitrate=%s", value);
            pSvcUserPref->InternalCamera.MainStream.bitrate = atof(value);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_BITRATE:
            debug_line("internal camera preview_bitrate=%s", value);
            pSvcUserPref->InternalCamera.SecStream.bitrate = atof(value);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_GOP:
            debug_line("internal camera record_gop=%s", value);
            pSvcUserPref->InternalCamera.MainStream.gop_size = atoi(value);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_GOP:
            debug_line("internal camera preview_gop=%s", value);
            pSvcUserPref->InternalCamera.SecStream.gop_size = atoi(value);
            break;
        case INI_SECTION_ITEM_INTERNAL_CAMERA_IMAGE_ROTATION:
            debug_line("internal camera image_rotation=%s", value);
            pSvcUserPref->InternalCamera.ImageRotation = atoi(value);
            break;
        default:
            break;
        }
    } else if (section_index == INI_SECTION_EXTERNAL_CAMERA) {
        switch (item_index) {
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_ENABLE:
            debug_line("external camera enabled=%s", value);
            pSvcUserPref->ExternalCamera.Enable = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_AUTO_RECORD:
            debug_line("external camera auto_record=%s", value);
            pSvcUserPref->ExternalCamera.AutoRecord = atoi(value) ? 1 : 0;
            break;
        //case INI_SECTION_ITEM_EXTERNAL_CAMERA_FPS:
            //debug_line("external camera fps=%s", value);
            //pSvcUserPref->ExternalCamera.Fps = atoi(value);
            //break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_SIZE:
            debug_line("external camera record_size=%s", value);
            split_size(value, &pSvcUserPref->ExternalCamera.MainStream);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_SIZE:
            debug_line("external camera preview_size=%s", value);
            split_size(value, &pSvcUserPref->ExternalCamera.SecStream);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_BITRATE:
            debug_line("external camera record_bitrate=%s", value);
            pSvcUserPref->ExternalCamera.MainStream.bitrate = atof(value);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_BITRATE:
            debug_line("external camera preview_bitrate=%s", value);
            pSvcUserPref->ExternalCamera.SecStream.bitrate = atof(value);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_GOP:
            debug_line("external camera record_gop=%s", value);
            pSvcUserPref->ExternalCamera.MainStream.gop_size = atoi(value);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_GOP:
            debug_line("external camera preview_gop=%s", value);
            pSvcUserPref->ExternalCamera.SecStream.gop_size = atoi(value);
            break;
        case INI_SECTION_ITEM_EXTERNAL_CAMERA_IMAGE_ROTATION:
            debug_line("external camera image_rotation=%s", value);
            pSvcUserPref->ExternalCamera.ImageRotation = atoi(value);
            break;
        default:
            break;
        }
    } else if (section_index == INI_SECTION_SYSTEM) {
        switch (item_index) {
        case INI_SECTION_ITEM_SYSTEM_VER:
            pSvcUserPref->SettingVersion = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_AUDIO_VOLUME:
            pSvcUserPref->AudioVolume = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_RECORD_SOUND:
            pSvcUserPref->RecordSoundOnOff = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_LED_ONOFF:
            pSvcUserPref->LedOnOff = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_SET_LED_BRIGHTNESS:
            pSvcUserPref->LedBrightness = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_IR_BRIGHTNESS:
            pSvcUserPref->IrBrightness = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_WATCHDOG_ONOFF:
            pSvcUserPref->WatchDogOnOff = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_EVENT_BUFFER_LENGTH:
            pSvcUserPref->EventBufferLength = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_DEBUG_MODE:
            pSvcUserPref->DebugMode = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_SYSTEM_TIME_ZONE:
            pSvcUserPref->TimeZone = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_WATERMARK:
            pSvcUserPref->WaterMark = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_WATERMARK_POS:
            pSvcUserPref->WaterMarkPos = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_SPEED_UNIT:
            pSvcUserPref->SpeedUnit = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_RTSP_LIVE_CAMERA:
            pSvcUserPref->RtspLiveCamera = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_IR_SAMPLING:
            pSvcUserPref->IrSampling = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_IR_THRESHOLD:
            pSvcUserPref->IrThresHold = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_MOTION_DETECT:
            pSvcUserPref->MotionDetect = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_AWB_MODE:
            pSvcUserPref->AwbMode = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_AE_BIAS:
            pSvcUserPref->AeBias = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_POWER_ONOFF_SOUND:
            pSvcUserPref->PowerOnOffSound = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_NO_ACTIVE_SHUTDOWN_TIME:
            pSvcUserPref->NoActiveShutdownTime = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_USE_24H_MODE:
            pSvcUserPref->Use24HMode = atoi(value);
            break;
        case INI_SECTION_ITEM_SYSTEM_IMU_RECORD:
            pSvcUserPref->ImuRecord = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_SYSTEM_GNSS_RECORD:
            pSvcUserPref->GnssRecord = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_SYSTEM_LINUX_BOOT:
            pSvcUserPref->LinuxBoot = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_LINUX_CONSOLE:
            pSvcUserPref->LinuxConsole = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_RTOS_CONSOLE:
            //pSvcUserPref->RtosConsole = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_USB_MASS_STORAGE:
            pSvcUserPref->UsbMassStorage = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_LED_RTOS_CONTROL:
            pSvcUserPref->LedRtosControl = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_KEY:
            memset(pSvcUserPref->VideoEncryption.encryption_key, 0, sizeof(pSvcUserPref->VideoEncryption.encryption_key));
            if (strlen(value) >= sizeof(pSvcUserPref->VideoEncryption.encryption_key)) {
                memcpy(pSvcUserPref->VideoEncryption.encryption_key, value, sizeof(pSvcUserPref->VideoEncryption.encryption_key) - 1);
            } else {
                memcpy(pSvcUserPref->VideoEncryption.encryption_key, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_ENABLE:
            pSvcUserPref->VideoEncryption.enable = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_DEBUG:
            pSvcUserPref->VideoEncryption.debug = atoi(value) ? 1 : 0;
            break;
        case INI_SECTION_ITEM_SYSTEM_VIDEO_DECRYPTION_KEY:
            memset(pSvcUserPref->VideoEncryption.decryption_key, 0, sizeof(pSvcUserPref->VideoEncryption.decryption_key));
            if (strlen(value) >= sizeof(pSvcUserPref->VideoEncryption.decryption_key)) {
                memcpy(pSvcUserPref->VideoEncryption.decryption_key, value, sizeof(pSvcUserPref->VideoEncryption.decryption_key) - 1);
            } else {
                memcpy(pSvcUserPref->VideoEncryption.decryption_key, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_SYSTEM_AUTHORIZATION_TOKEN:
            memset(pSvcUserPref->AuthorizationToken, 0, sizeof(pSvcUserPref->AuthorizationToken));
            if (strlen(value) >= sizeof(pSvcUserPref->AuthorizationToken)) {
                memcpy(pSvcUserPref->AuthorizationToken, value, sizeof(pSvcUserPref->AuthorizationToken) - 1);
            } else {
                memcpy(pSvcUserPref->AuthorizationToken, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_SYSTEM_PARKING_LEVEL:
            pSvcUserPref->ParkingLevel = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_LTE_WAKEUP_ENABLE:
            pSvcUserPref->EnableLteRemoteWakeup = atoi(value) ? 1 : 0;            
            break;
        case INI_SECTION_ITEM_SYSTEM_CV_RUN:
            pSvcUserPref->CvOnOff = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_PIV:
            pSvcUserPref->PivOnOff = atoi(value) ? OPTION_ON : OPTION_OFF;
            break;
        case INI_SECTION_ITEM_SYSTEM_H265_FORMAT:
            if (atoi(value)) {
                pSvcUserPref->InternalCamera.MainStream.is_h265 = 1;
                pSvcUserPref->InternalCamera.SecStream.is_h265 = 1;
                pSvcUserPref->ExternalCamera.MainStream.is_h265 = 1;
                pSvcUserPref->ExternalCamera.SecStream.is_h265 = 1;
            } else {
                pSvcUserPref->InternalCamera.MainStream.is_h265 = 0;
                pSvcUserPref->InternalCamera.SecStream.is_h265 = 0;
                pSvcUserPref->ExternalCamera.MainStream.is_h265 = 0;
                pSvcUserPref->ExternalCamera.SecStream.is_h265 = 0;
            }
            break;
        default:
            break;
        }
    } else if (section_index == INI_SECTION_WIFI) {
        switch (item_index) {
        case INI_SECTION_ITEM_WIFI_BOOT_MODE:
            pSvcUserPref->WifiSetting.BootMode = atoi(value);
            break;
        case INI_SECTION_ITEM_WIFI_CONNECT_MODE:
            pSvcUserPref->WifiSetting.ConnectMode = atoi(value);
            break;
        case INI_SECTION_ITEM_WIFI_CHANNEL:
            pSvcUserPref->WifiSetting.Channel = atoi(value);
            break;
        case INI_SECTION_ITEM_WIFI_MODE:
            pSvcUserPref->WifiSetting.Use5G = atoi(value);
            break;
        case INI_SECTION_ITEM_WIFI_AP_SSID:
            memset(pSvcUserPref->WifiSetting.ApSSID, 0, sizeof(pSvcUserPref->WifiSetting.ApSSID));
            if (strlen(value) >= sizeof(pSvcUserPref->WifiSetting.ApSSID)) {
                memcpy(pSvcUserPref->WifiSetting.ApSSID, value, sizeof(pSvcUserPref->WifiSetting.ApSSID) - 1);
            } else {
                memcpy(pSvcUserPref->WifiSetting.ApSSID, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_WIFI_AP_PASSWORD:
            memset(pSvcUserPref->WifiSetting.ApPassword, 0, sizeof(pSvcUserPref->WifiSetting.ApPassword));
            if (strlen(value) >= sizeof(pSvcUserPref->WifiSetting.ApPassword)) {
                memcpy(pSvcUserPref->WifiSetting.ApPassword, value, sizeof(pSvcUserPref->WifiSetting.ApPassword) - 1);
            } else {
                memcpy(pSvcUserPref->WifiSetting.ApPassword, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_WIFI_AP_IP:
            memset(pSvcUserPref->WifiSetting.ApIP, 0, sizeof(pSvcUserPref->WifiSetting.ApIP));
            if (strlen(value) >= sizeof(pSvcUserPref->WifiSetting.ApIP)) {
                memcpy(pSvcUserPref->WifiSetting.ApIP, value, sizeof(pSvcUserPref->WifiSetting.ApIP) - 1);
            } else {
                memcpy(pSvcUserPref->WifiSetting.ApIP, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_WIFI_STA_SSID:
            memset(pSvcUserPref->WifiSetting.StaSSID, 0, sizeof(pSvcUserPref->WifiSetting.StaSSID));
            if (strlen(value) >= sizeof(pSvcUserPref->WifiSetting.StaSSID)) {
                memcpy(pSvcUserPref->WifiSetting.StaSSID, value, sizeof(pSvcUserPref->WifiSetting.StaSSID) - 1);
            } else {
                memcpy(pSvcUserPref->WifiSetting.StaSSID, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_WIFI_STA_PASSWORD:
            memset(pSvcUserPref->WifiSetting.StaPassword, 0, sizeof(pSvcUserPref->WifiSetting.StaPassword));
            if (strlen(value) >= sizeof(pSvcUserPref->WifiSetting.StaPassword)) {
                memcpy(pSvcUserPref->WifiSetting.StaPassword, value, sizeof(pSvcUserPref->WifiSetting.StaPassword) - 1);
            } else {
                memcpy(pSvcUserPref->WifiSetting.StaPassword, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_WIFI_P2P_WAIT_TIME:
            pSvcUserPref->WifiSetting.P2pWaitTime = atoi(value);
            break;
        case INI_SECTION_ITEM_USB_WIFI_BOOT_MODE:
            pSvcUserPref->UsbWifiSetting.BootMode = atoi(value);
            break;
        case INI_SECTION_ITEM_USB_WIFI_CHANNEL:
            pSvcUserPref->UsbWifiSetting.Channel = atoi(value);
            break;
        case INI_SECTION_ITEM_USB_WIFI_AP_SSID:
            memset(pSvcUserPref->UsbWifiSetting.ApSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApSSID));
            if (strlen(value) >= sizeof(pSvcUserPref->UsbWifiSetting.ApSSID)) {
                memcpy(pSvcUserPref->UsbWifiSetting.ApSSID, value, sizeof(pSvcUserPref->UsbWifiSetting.ApSSID) - 1);
            } else {
                memcpy(pSvcUserPref->UsbWifiSetting.ApSSID, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_USB_WIFI_AP_PASSWORD:
            memset(pSvcUserPref->UsbWifiSetting.ApPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApPassword));
            if (strlen(value) >= sizeof(pSvcUserPref->UsbWifiSetting.ApPassword)) {
                memcpy(pSvcUserPref->UsbWifiSetting.ApPassword, value, sizeof(pSvcUserPref->UsbWifiSetting.ApPassword) - 1);
            } else {
                memcpy(pSvcUserPref->UsbWifiSetting.ApPassword, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_USB_WIFI_AP_IP:
            memset(pSvcUserPref->UsbWifiSetting.ApIP, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApIP));
            if (strlen(value) >= sizeof(pSvcUserPref->UsbWifiSetting.ApIP)) {
                memcpy(pSvcUserPref->UsbWifiSetting.ApIP, value, sizeof(pSvcUserPref->UsbWifiSetting.ApIP) - 1);
            } else {
                memcpy(pSvcUserPref->UsbWifiSetting.ApIP, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_USB_WIFI_STA_SSID:
            memset(pSvcUserPref->UsbWifiSetting.StaSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID));
            if (strlen(value) >= sizeof(pSvcUserPref->UsbWifiSetting.StaSSID)) {
                memcpy(pSvcUserPref->UsbWifiSetting.StaSSID, value, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID) - 1);
            } else {
                memcpy(pSvcUserPref->UsbWifiSetting.StaSSID, value, strlen(value));
            }
            break;
        case INI_SECTION_ITEM_USB_WIFI_STA_PASSWORD:
            memset(pSvcUserPref->UsbWifiSetting.StaPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword));
            if (strlen(value) >= sizeof(pSvcUserPref->UsbWifiSetting.StaPassword)) {
                memcpy(pSvcUserPref->UsbWifiSetting.StaPassword, value, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword) - 1);
            } else {
                memcpy(pSvcUserPref->UsbWifiSetting.StaPassword, value, strlen(value));
            }
            break;
        default:
            break;
        }
    } else if (section_index == INI_SECTION_GNSS) {
        switch (item_index) {
        case INI_SECTION_ITEM_GNSS_UART_BITRATE:
            pSvcUserPref->GnssSetting.UartBitRate = atoi(value);
            break;
        case INI_SECTION_ITEM_GNSS_UART_CHANNEL:
            //pSvcUserPref->GnssSetting.UartChannel = atoi(value);
            break;
        case INI_SECTION_ITEM_GNSS_ID:
            pSvcUserPref->GnssSetting.Id = atoi(value);
            break;
        case INI_SECTION_ITEM_GNSS_DATARATE:
            pSvcUserPref->GnssSetting.DataRate = atoi(value);
            break;
        default:
            break;
        }
    } else if(section_index == INI_SECTION_IMU) {
        switch (item_index) {
        case INI_SECTION_ITEM_IMU_ACCEL_ODR:
            pSvcUserPref->ImuSetting.AccelOdr = atoi(value);
            break;
        case INI_SECTION_ITEM_IMU_ACCEL_RANGE:
            pSvcUserPref->ImuSetting.AccelRange = atoi(value);
            break;
        case INI_SECTION_ITEM_IMU_GYRO_ODR:
            pSvcUserPref->ImuSetting.GyroOdr = atoi(value);
            break;
        case INI_SECTION_ITEM_IMU_GYRO_RANGE:
            pSvcUserPref->ImuSetting.GyroRange = atoi(value);
            break;
        case INI_SECTION_ITEM_IMU_AXIS_POLARITY:
            pSvcUserPref->ImuSetting.AxisPolarity = atoi(value);
            break;
        default:
            break;   
        }
    } else if(section_index == INI_SECTION_LED) {
        switch (item_index) {
        case INI_SECTION_ITEM_LED_R:
            pSvcUserPref->LedColor.R = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_LED_G:
            pSvcUserPref->LedColor.G = atoi(value) & 0xFF;
            break;
        case INI_SECTION_ITEM_LED_B:
            pSvcUserPref->LedColor.B = atoi(value) & 0xFF;
            break;
        default:
            break;
        }
    } else if(section_index == INI_SECTION_API) {
        switch (item_index) {
        case INI_SECTION_ITEM_API_FILES:
            pSvcUserPref->ApiSettings.files_enable = atoi(value) ? 1 : 0;
            break;
        default:
            break;
        }
    }
    return 0;
}

int user_setting_save(void)
{
    void *pFile = NULL;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    char tmp[64] = {0};
    static AMBA_KAL_MUTEX_t mutex;
    static int flag = 0;

    if (flag == 0) {
        if (AmbaKAL_MutexCreate(&mutex, "user_setting") != OK) {
            debug_line("%s create mutex fail", __func__);
            return -1;
        }        
        flag = 1;
    }
    AmbaKAL_MutexTake(&mutex, AMBA_KAL_WAIT_FOREVER);
    debug_line("%s start", __func__);
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref == NULL) {
        debug_line("%s get pref fail", __func__);AmbaPrint_Flush();        
        AmbaKAL_MutexGive(&mutex);
        return -1;
    }

    pFile = fopen(USER_SETTING_SAVE_PATH, "w");
    if (pFile == NULL) {
        debug_line("%s open %s fail", __func__, USER_SETTING_SAVE_PATH);AmbaPrint_Flush();        
        AmbaKAL_MutexGive(&mutex);
        return -1;
    }
    //add section internal
    ini_save_section_name(pFile, INI_SECTION_INTERNAL_CAMERA);
    ini_save_section_item_int(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_AUTO_RECORD, pSvcUserPref->InternalCamera.AutoRecord ? 1 : 0);
    //ini_save_section_item_int(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_FPS, pSvcUserPref->InternalCamera.Fps);
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "%dx%d", pSvcUserPref->InternalCamera.MainStream.width, pSvcUserPref->InternalCamera.MainStream.height);
    ini_save_section_item_str(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_SIZE, tmp);
    ini_save_section_item_double(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_BITRATE, pSvcUserPref->InternalCamera.MainStream.bitrate);
    ini_save_section_item_int(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_MAIN_STREAM_GOP, pSvcUserPref->InternalCamera.MainStream.gop_size);
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "%dx%d", pSvcUserPref->InternalCamera.SecStream.width, pSvcUserPref->InternalCamera.SecStream.height);
    ini_save_section_item_str(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_SIZE, tmp);
    ini_save_section_item_double(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_BITRATE, pSvcUserPref->InternalCamera.SecStream.bitrate);
    ini_save_section_item_int(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_SEC_STREAM_GOP, pSvcUserPref->InternalCamera.SecStream.gop_size);
    ini_add_line(pFile, "# 0->normal, 1->h_flip, 2->v_flip, 3->h+v_flip");
    ini_save_section_item_int(pFile, INI_SECTION_INTERNAL_CAMERA, INI_SECTION_ITEM_INTERNAL_CAMERA_IMAGE_ROTATION, pSvcUserPref->InternalCamera.ImageRotation);
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    //add section external
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_EXTERNAL_CAMERA);
    ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_ENABLE, pSvcUserPref->ExternalCamera.Enable ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_AUTO_RECORD, pSvcUserPref->ExternalCamera.AutoRecord ? 1 : 0);
    //ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_FPS, pSvcUserPref->ExternalCamera.Fps);
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "%dx%d", pSvcUserPref->ExternalCamera.MainStream.width, pSvcUserPref->ExternalCamera.MainStream.height);
    ini_save_section_item_str(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_SIZE, tmp);
    ini_save_section_item_double(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_BITRATE, pSvcUserPref->ExternalCamera.MainStream.bitrate);
    ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_MAIN_STREAM_GOP, pSvcUserPref->ExternalCamera.MainStream.gop_size);
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp) - 1, "%dx%d", pSvcUserPref->ExternalCamera.SecStream.width, pSvcUserPref->ExternalCamera.SecStream.height);
    ini_save_section_item_str(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_SIZE, tmp);
    ini_save_section_item_double(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_BITRATE, pSvcUserPref->ExternalCamera.SecStream.bitrate);
    ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_SEC_STREAM_GOP, pSvcUserPref->ExternalCamera.SecStream.gop_size);
    ini_add_line(pFile, "# 0->normal, 1->h_flip, 2->v_flip, 3->h+v_flip");
    ini_save_section_item_int(pFile, INI_SECTION_EXTERNAL_CAMERA, INI_SECTION_ITEM_EXTERNAL_CAMERA_IMAGE_ROTATION, pSvcUserPref->ExternalCamera.ImageRotation);
#endif
    //add section system
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_SYSTEM);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_VER, pSvcUserPref->SettingVersion);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_AUDIO_VOLUME, pSvcUserPref->AudioVolume);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_RECORD_SOUND, (pSvcUserPref->RecordSoundOnOff == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_LED_ONOFF, (pSvcUserPref->LedOnOff == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_SET_LED_BRIGHTNESS, pSvcUserPref->LedBrightness);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_IR_BRIGHTNESS, pSvcUserPref->IrBrightness);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_WATCHDOG_ONOFF, (pSvcUserPref->WatchDogOnOff == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_EVENT_BUFFER_LENGTH, pSvcUserPref->EventBufferLength);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_DEBUG_MODE, pSvcUserPref->DebugMode);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_TIME_ZONE, pSvcUserPref->TimeZone);
    ini_add_line(pFile, "#0:none, 1:datetime, 2:speed, 4: logo");
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_WATERMARK, pSvcUserPref->WaterMark);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_WATERMARK_POS, pSvcUserPref->WaterMarkPos);
    ini_add_line(pFile, "#0:kph, 1:mph");
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_SPEED_UNIT, pSvcUserPref->SpeedUnit);
    ini_add_line(pFile, "#0:internal, 1:external");
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_RTSP_LIVE_CAMERA, pSvcUserPref->RtspLiveCamera);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_IR_SAMPLING, pSvcUserPref->IrSampling);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_IR_THRESHOLD, pSvcUserPref->IrThresHold);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_MOTION_DETECT, pSvcUserPref->MotionDetect);
    ini_add_line(pFile, "#awb_mode auto|incandescent|sunny|cloudy|fluorescent");
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_AWB_MODE, pSvcUserPref->AwbMode);
    ini_add_line(pFile, "#ae_bias range: (0-6)");
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_AE_BIAS, pSvcUserPref->AeBias);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_POWER_ONOFF_SOUND, pSvcUserPref->PowerOnOffSound);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_NO_ACTIVE_SHUTDOWN_TIME, pSvcUserPref->NoActiveShutdownTime);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_USE_24H_MODE, pSvcUserPref->Use24HMode);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_IMU_RECORD, pSvcUserPref->ImuRecord);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_GNSS_RECORD, pSvcUserPref->GnssRecord);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_LINUX_BOOT, (pSvcUserPref->LinuxBoot == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_LINUX_CONSOLE, (pSvcUserPref->LinuxConsole == OPTION_ON) ? 1 : 0);
    //ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_RTOS_CONSOLE, (pSvcUserPref->RtosConsole == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_USB_MASS_STORAGE, (pSvcUserPref->UsbMassStorage == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_LED_RTOS_CONTROL, (pSvcUserPref->LedRtosControl == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_ENABLE, pSvcUserPref->VideoEncryption.enable);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_DEBUG, pSvcUserPref->VideoEncryption.debug);
    ini_save_section_item_str(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_VIDEO_ENCRYPTION_KEY, pSvcUserPref->VideoEncryption.encryption_key);
    ini_save_section_item_str(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_VIDEO_DECRYPTION_KEY, pSvcUserPref->VideoEncryption.decryption_key);
    ini_save_section_item_str(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_AUTHORIZATION_TOKEN, pSvcUserPref->AuthorizationToken);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_PARKING_LEVEL, pSvcUserPref->ParkingLevel);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_LTE_WAKEUP_ENABLE, pSvcUserPref->EnableLteRemoteWakeup ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_CV_RUN, (pSvcUserPref->CvOnOff == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_PIV, (pSvcUserPref->PivOnOff == OPTION_ON) ? 1 : 0);
    ini_save_section_item_int(pFile, INI_SECTION_SYSTEM, INI_SECTION_ITEM_SYSTEM_H265_FORMAT, (pSvcUserPref->InternalCamera.MainStream.is_h265) ? 1 : 0);
    //add section wifi
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_WIFI);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_BOOT_MODE, pSvcUserPref->WifiSetting.BootMode);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_CONNECT_MODE, pSvcUserPref->WifiSetting.ConnectMode);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_CHANNEL, pSvcUserPref->WifiSetting.Channel);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_MODE, pSvcUserPref->WifiSetting.Use5G);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_AP_SSID, pSvcUserPref->WifiSetting.ApSSID);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_AP_PASSWORD, pSvcUserPref->WifiSetting.ApPassword);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_AP_IP, pSvcUserPref->WifiSetting.ApIP);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_STA_SSID, pSvcUserPref->WifiSetting.StaSSID);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_STA_PASSWORD, pSvcUserPref->WifiSetting.StaPassword);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_WIFI_P2P_WAIT_TIME, pSvcUserPref->WifiSetting.P2pWaitTime);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_BOOT_MODE, pSvcUserPref->UsbWifiSetting.BootMode);
    ini_save_section_item_int(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_CHANNEL, pSvcUserPref->UsbWifiSetting.Channel);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_AP_SSID, pSvcUserPref->UsbWifiSetting.ApSSID);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_AP_PASSWORD, pSvcUserPref->UsbWifiSetting.ApPassword);
    //ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_AP_IP, pSvcUserPref->UsbWifiSetting.ApIP);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_STA_SSID, pSvcUserPref->UsbWifiSetting.StaSSID);
    ini_save_section_item_str(pFile, INI_SECTION_WIFI, INI_SECTION_ITEM_USB_WIFI_STA_PASSWORD, pSvcUserPref->UsbWifiSetting.StaPassword);
#endif
    //add section gnss
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_GNSS);
    ini_save_section_item_int(pFile, INI_SECTION_GNSS, INI_SECTION_ITEM_GNSS_UART_BITRATE, pSvcUserPref->GnssSetting.UartBitRate);
    //ini_save_section_item_int(pFile, INI_SECTION_GNSS, INI_SECTION_ITEM_GNSS_UART_CHANNEL, pSvcUserPref->GnssSetting.UartChannel);
    ini_save_section_item_int(pFile, INI_SECTION_GNSS, INI_SECTION_ITEM_GNSS_ID, pSvcUserPref->GnssSetting.Id);
    ini_save_section_item_int(pFile, INI_SECTION_GNSS, INI_SECTION_ITEM_GNSS_DATARATE, pSvcUserPref->GnssSetting.DataRate);
    //add section imu
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_IMU);
    ini_save_section_item_int(pFile, INI_SECTION_IMU, INI_SECTION_ITEM_IMU_ACCEL_ODR, pSvcUserPref->ImuSetting.AccelOdr);
    ini_save_section_item_int(pFile, INI_SECTION_IMU, INI_SECTION_ITEM_IMU_ACCEL_RANGE, pSvcUserPref->ImuSetting.AccelRange);
    ini_save_section_item_int(pFile, INI_SECTION_IMU, INI_SECTION_ITEM_IMU_GYRO_ODR, pSvcUserPref->ImuSetting.GyroOdr);
    ini_save_section_item_int(pFile, INI_SECTION_IMU, INI_SECTION_ITEM_IMU_GYRO_RANGE, pSvcUserPref->ImuSetting.GyroRange);
    ini_save_section_item_int(pFile, INI_SECTION_IMU, INI_SECTION_ITEM_IMU_AXIS_POLARITY, pSvcUserPref->ImuSetting.AxisPolarity);
    //add section led
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_LED);
    ini_save_section_item_int(pFile, INI_SECTION_LED, INI_SECTION_ITEM_LED_R, pSvcUserPref->LedColor.R);
    ini_save_section_item_int(pFile, INI_SECTION_LED, INI_SECTION_ITEM_LED_G, pSvcUserPref->LedColor.G);
    ini_save_section_item_int(pFile, INI_SECTION_LED, INI_SECTION_ITEM_LED_B, pSvcUserPref->LedColor.B);
    //add section api
    ini_add_line(pFile, "");
    ini_save_section_name(pFile, INI_SECTION_API);
    ini_save_section_item_int(pFile, INI_SECTION_API, INI_SECTION_ITEM_API_FILES, pSvcUserPref->ApiSettings.files_enable);

    fflush(pFile);
    fclose(pFile);
    debug_line("%s finish", __func__);
    AmbaKAL_MutexGive(&mutex);

    return 0;
}

int user_setting_for_hard_reset(void)
{
#define FILE_MAX_LINE_NUM (64)
#define FILE_MAX_LINE_SIZE (128)
    char lines[FILE_MAX_LINE_NUM][FILE_MAX_LINE_SIZE] = {0};
    int line = 0;
    FILE *pFile = NULL;

    pFile = fopen(USER_SETTING_DEVINFO_PATH, "r");
    if (pFile) {
        while(line < FILE_MAX_LINE_NUM) {
            memset(lines[line], 0, FILE_MAX_LINE_SIZE);
            if (readline(pFile, lines[line]) < 0) {
                break;
            }
            line += 1;
        }
        fclose(pFile);
    }
    debug_line("clean %s", USER_SETTING_PREF_DIR);
    clean_dir(USER_SETTING_PREF_DIR);
    if (line > 0) {
        pFile = fopen(USER_SETTING_DEVINFO_PATH, "w");
        if (pFile) {
            int index = 0;
            while (index < line) {
                if (strncmp(lines[index], "first_access=", strlen("first_access=")) == 0) {
                    memset(lines[index], 0, FILE_MAX_LINE_SIZE);
                    snprintf(lines[index], FILE_MAX_LINE_SIZE - 1, "%s", "first_access=1");
                }
                fwrite(lines[index], strlen(lines[index]), 1, pFile);
                fwrite("\n", 1, 1, pFile);
                index += 1;
            }
            fflush(pFile);
            fclose(pFile);
        }
    }

    return 0;
}

int user_setting_to_default(void)
{
    AmbaFS_Format('a');
    AmbaFS_MakeDir("a:\\pref");

    return 0;
}

int user_setting_init(void)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    ini_register_setting(sizeof(ini_settings) / sizeof(ini_section_s), ini_settings);
    ini_register_callback(user_setting_section_item_handler);
    ini_load(USER_SETTING_SAVE_PATH);
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->SettingVersion < 22) {
        pSvcUserPref->SettingVersion = USER_SETTING_VER_NUM;
        pSvcUserPref->AudioVolume = 2U;
        SvcUserPref_Reset();
    }

    return 0;
}

