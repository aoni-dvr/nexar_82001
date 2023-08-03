#include "product_line.h"
#include "AmbaShell.h"
#include "SvcCmd.h"
#if defined(CONFIG_BSP_H32_NEXAR_D081)
#include "../../../../../../../bsp/h32/peripherals/imu/bmi160_defs.h"
#else
#include "../../../../../../../bsp/cv25/peripherals/imu/bmi160_defs.h"
#endif
//#if defined(CONFIG_APP_FIRMWARE_FOR_MP)
//#include "mp/secure_boot_otp_pubkey.h"
//#else
//#include "secure_boot_otp_pubkey.h"
//#endif

static product_line_cmd_cb_func product_line_cmd_func = NULL;
static void product_line_cmd_main_func(unsigned int argc, char * const *argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)PrintFunc;
    if (product_line_cmd_func) {
        (*product_line_cmd_func)(argc, (char **)argv, NULL);
    }
}

int product_line_cmd_init_impl(const char *key, product_line_cmd_cb_func func)
{
    AMBA_SHELL_COMMAND_s  Cmd;
    UINT32  Rval = 0;

    product_line_cmd_func = func;
    Cmd.pName    = key;
    Cmd.MainFunc = product_line_cmd_main_func;
    Cmd.pNext    = NULL;
    Rval = SvcCmd_CommandRegister(&Cmd);
    if (SHELL_ERR_SUCCESS != Rval) {
        debug_line("%s: fail to register cmd", __func__);
        return -1;
    }
    AmbaShell_EnableEcho(0);

    return 0;
}

int product_line_cmd_burn_mac_sn2(const char *wifi_mac, const char *sn,
    char *confirm_wifi_mac, char *confirm_sn)
{
    char cmd[1024] = {0};
    unsigned int i = 0, index = 0;
    char new_wifi_mac[32] = {0};

    if (sn == NULL || strlen(sn) <= 0 || strlen(sn) > 16 || strlen(wifi_mac) != 12) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    if (app_helper.lte_booted == 0) {
        return -1;
    }
    memset(new_wifi_mac, 0, sizeof(new_wifi_mac));
    memcpy(new_wifi_mac, wifi_mac, 2);
    new_wifi_mac[2] = ':';
    memcpy(new_wifi_mac + 3, wifi_mac + 2, 2);
    new_wifi_mac[5] = ':';
    memcpy(new_wifi_mac + 6, wifi_mac + 4, 2);
    new_wifi_mac[8] = ':';
    memcpy(new_wifi_mac + 9, wifi_mac + 6, 2);
    new_wifi_mac[11] = ':';
    memcpy(new_wifi_mac + 12, wifi_mac + 8, 2);
    new_wifi_mac[14] = ':';
    memcpy(new_wifi_mac + 15, wifi_mac + 10, 2);
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd) - 1, "burn_info lte flash %s %s", sn, new_wifi_mac);
    if (app_helper.linux_system(cmd) < 0) {
        return -1;
    }
    memset(cmd, 0, sizeof(cmd));
    if (app_helper.linux_system_with_response("burn_info lte read", cmd) < 0) {
        return -1;
    }
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == '|') {
            index = i;
            break;
        }
    }
    if (index >= strlen(cmd)) {
        return -1;
    }
    memcpy(confirm_sn, cmd, index);
    memcpy(confirm_wifi_mac, cmd + index + 1, strlen(cmd) - index - 1);

    return 0;
}

int product_line_cmd_get_sn2(char *sn,  int size)
{
    char response[1024] = {0};

    if (app_helper.lte_booted == 0) {
        return -1;
    }
    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("burn_info lte read sn", response) < 0) {
        return -1;
    }
    if (!((response[0] >= '0' && response[0] <= '9')
        || (response[0] >= 'a' && response[0] <= 'z')
        || (response[0] >= 'A' && response[0] <= 'Z'))) {
        memset(response, 0, sizeof(response));
    }
    if (strlen(response) <= 0 && dqa_test_script.sn_fake) {
        snprintf(sn, size - 1, "%s", "XXXXXXXXXXXXX");
    } else {
        snprintf(sn, size - 1, "%s", response);
    }

    return 0;
}

int product_line_cmd_switch_to_wifi_conduct_test(void)
{
    app_helper.touch(SD_SLOT":\\WifiTestFile_auto_delete.txt");
    Pmic_NormalSoftReset();

    return 0;
}

int product_line_cmd_switch_to_cvbs(int index)
{
    (void)index;
    return 0;
}

int product_line_cmd_switch_to_usb_camera(int index)
{
    if (index == 0) {
        app_helper.touch(SD_SLOT":\\tiaojiao_auto_delete.txt");
    } else if (index == 1) {
        app_helper.touch(SD_SLOT":\\tiaojiao2_auto_delete.txt");
    } else {
        return -1;
    }
    Pmic_NormalSoftReset();

    return 0;
}

int product_line_cmd_get_wifi_rtsp_url(int index, const char *ip, char *url)
{
    char tmp[128] = {0};

    memset(tmp, 0, sizeof(tmp));
    if (index == 0) {
        snprintf(tmp, sizeof(tmp) - 1, "rtsp://%s/live-internal", ip);
    } else if (index == 1) {
        snprintf(tmp, sizeof(tmp) - 1, "rtsp://%s/live-external", ip);
    } else {
        return -1;
    }
    memcpy(url, tmp, strlen(tmp));

    return 0;
}

static void product_line_led_timer_handler(int eid)
{
    static int cnt = 0;

    if (eid == TIMER_UNREGISTER) {
        cnt = 0;
        return;
    }
    cnt += 1;
    if (cnt == 1) {
        app_helper.lock_led(0);
        app_helper.set_led_color(0, 255, 0);
        app_helper.lock_led(1);
    } else if (cnt == 2) {
        app_helper.lock_led(0);
        app_helper.set_led_color(0, 0, 255);
        app_helper.lock_led(1);
    } else if (cnt == 3) {
        app_helper.lock_led(0);
        app_helper.set_led_color(255, 255, 255);
        app_helper.lock_led(1);
    } else if (cnt == 4) {
        app_helper.lock_led(0);
        app_helper.set_led_color(0, 0, 0);
        app_helper.lock_led(1);
    } else if (cnt == 5) {
        app_helper.lock_led(0);
        app_helper.set_led_color(255, 0, 0);
        app_helper.lock_led(1);
        cnt = 0;
    }
}

static int led_busy = 0;
int product_line_cmd_switch_led_on_off(int on)
{
    if (on) {
        if (led_busy) {
            return 0;
        }
        led_busy = 1;
        app_helper.lock_ir_led(0);
        app_helper.set_ir_brightness(100);
        app_helper.lock_ir_led(1);
        app_helper.lock_led(0);
        app_helper.set_led_color(255, 0, 0);
        app_helper.lock_led(1);
        app_timer_register(TIMER_1HZ, product_line_led_timer_handler);
    } else {
        app_timer_unregister(TIMER_1HZ, product_line_led_timer_handler);
        app_helper.lock_ir_led(0);
        app_helper.set_ir_brightness(0);
        app_helper.lock_ir_led(1);
        app_helper.set_led_onoff(0);
        led_busy = 0;
    }

    return 0;
}

#define AUDIO_TEST_FILENAME "audio_test.pcm"
#define AUDIO_TEST_FILENAME1 "audio_test1.pcm"
#define AUDIO_TEST_FILENAME2 "audio_test2.pcm"
#define AUDIO_TEST_PATH SD_SLOT":\\"AUDIO_TEST_FILENAME
#define AUDIO_TEST_PATH1 SD_SLOT":\\"AUDIO_TEST_FILENAME1
#define AUDIO_TEST_PATH2 SD_SLOT":\\"AUDIO_TEST_FILENAME2
static void product_line_rec_audio_timer_handler(int eid)
{
    static int cnt = 0;

    if (eid == TIMER_UNREGISTER) {
        cnt = 0;
        return;
    }
    cnt += 1;
    if (cnt == dqa_test_script.audio_test_seconds) {
        pcm_record_stop();
        product_line_cmd_record_done();
    } else if (cnt > (dqa_test_script.audio_test_seconds + 1)) {
        app_timer_unregister(TIMER_1HZ, product_line_rec_audio_timer_handler);
        beep_file(AUDIO_TEST_PATH);
    }
}

int product_line_cmd_record_playback(int len)
{
    if (len <= 0) {
        debug_line("%s invalid len: %d", __func__, len);
        return -1;
    }
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
    dqa_test_script.audio_test_seconds = len;
#endif
    remove(AUDIO_TEST_PATH);
    pcm_record_set_path(AUDIO_TEST_PATH);
    if (pcm_record_start() >= 0) {
        app_timer_register(TIMER_1HZ, product_line_rec_audio_timer_handler);
    } else {
        return -1;
    }

    return 0;
}

int product_line_cmd_get_gsensor_data(int *x, int *y, int *z)
{
    IMU_DATA_s data;

    if (Imu_GetData(&data, 0) < 0) {
        return -1;
    }
    *x = data.gyro.x;
    *y = data.gyro.y;
    *z = data.gyro.z;

    return 0;
}

int product_line_cmd_get_imu_data(IMU_DATA_s *data)
{
    if (data == NULL) {
        return -1;
    }
    if (Imu_GetData(data, 0) < 0) {
        return -1;
    }

    return 0;
}

int product_line_cmd_get_thermal_data(int *value)
{
    (void)value;

    return 0;
}

int product_line_cmd_get_brightness_data(int *value)
{
    if (value != NULL) {
        *value = ir_task_get_brightness();
    }

    return 0;
}

int product_line_cmd_get_gps_data(int *num, product_line_gps_info_s *infos, int max_num)
{
    gnss_data_s gnss_data;
    int i = 0;

    gnss_parser_get_data(&gnss_data);
    if (gnss_data.m_nSatNumInView[GNSS_GPS] <= 0) {
        *num = 0;
        return 0;
    }
    if (gnss_data.m_nSatNumInView[GNSS_GPS] > max_num) {
        gnss_data.m_nSatNumInView[GNSS_GPS] = max_num;
    }
    *num = gnss_data.m_nSatNumInView[GNSS_GPS];
    for (i = 0; i < gnss_data.m_nSatNumInView[GNSS_GPS]; i++) {
        infos[i].sat_index = gnss_data.m_Sat[GNSS_GPS][i].PRN;
        infos[i].sat_db = (int)gnss_data.m_Sat[GNSS_GPS][i].ss;
    }

    return -1;
}

int product_line_cmd_get_gps_str(char *gps_str)
{
    (void)gps_str;

    return -1;
}

int product_line_cmd_set_time(const char *time)
{
    AMBA_RTC_DATE_TIME_s Calendar;
    char tmp[16] = {0};

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 0, 4);
    Calendar.Year = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 5, 2);
    Calendar.Month = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 8, 2);
    Calendar.Day = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 11, 2);
    Calendar.Hour = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 14, 2);
    Calendar.Minute = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, time + 17, 2);
    Calendar.Second = atoi(tmp);
    if (AmbaRTC_SetSysTime(&Calendar) == RTC_ERR_NONE) {
#if defined(CONFIG_ENABLE_AMBALINK)
        app_helper.time_update_for_linux();
#endif
    } else {
        debug_line("% set time fail", __func__);
        return -1;
    }
    return 0;
}

int product_line_cmd_get_time(char *p_time, int size)
{
    time_s cur_time;

    time_now(&cur_time);
    snprintf(p_time, size - 1, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", cur_time.year,
                                                              cur_time.month,
                                                              cur_time.day,
                                                              cur_time.hour,
                                                              cur_time.minute,
                                                              cur_time.second);

    return 0;
}

int product_line_cmd_get_build_time(char *p_time, int size)
{
    snprintf(p_time, size - 1, "%s", PROJECT_INFO_BUILD_TIME);

    return 0;
}

int product_line_cmd_burn_mac_sn(const char *wifi_mac, const char *bt_mac, const char *sn,
    char *confirm_wifi_mac, char *confirm_bt_mac, char *confirm_sn)
{
    unsigned char sn_u8[16] = {0};
    metadata_s *metadata = NULL;
    unsigned char key_u8[32] = {0};
    int i = 0;
    char tmp[3] = {0};

    if (sn == NULL || strlen(sn) <= 0 || strlen(sn) > 16 || strlen(wifi_mac) != 12 || strlen(bt_mac) != 12) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    metadata_get(&metadata);
    memset(sn_u8, 0, sizeof(sn_u8));
    memcpy(sn_u8, sn, strlen(sn));
    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_CUSTOMER_ID, sn_u8, 16, 0) < 0) {
        if (metadata->SerialNumber[0] == 0) {
            return -1;
        }
    } else {
        memset(metadata->SerialNumber, 0, sizeof(metadata->SerialNumber));
        memcpy(metadata->SerialNumber, sn_u8, 16);
    }
    memcpy(confirm_sn, sn_u8, 16);

    memset(key_u8, 0, sizeof(key_u8));
    for (i = 0; i < 6; i++) {
        tmp[0] = wifi_mac[i * 2];
        tmp[1] = wifi_mac[i * 2 + 1];
        tmp[2] = 0;
        key_u8[i] = strtoul(tmp, NULL, 16) & 0xFF;
    }
    for (i = 0; i < 6; i++) {
        tmp[0] = bt_mac[i * 2];
        tmp[1] = bt_mac[i * 2 + 1];
        tmp[2] = 0;
        key_u8[6 + i] = strtoul(tmp, NULL, 16) & 0xFF;
    }
    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_AES_KEY, key_u8, 32, 0) < 0) {
        if (metadata->WifiMac[0] == 0 || metadata->BtMac[0] == 0) {
            return -1;
        }
    } else {
        memset(metadata->WifiMac, 0, sizeof(metadata->WifiMac));
        memcpy(metadata->WifiMac, key_u8, 6);
        memset(metadata->BtMac, 0, sizeof(metadata->BtMac));
        memcpy(metadata->BtMac, key_u8 + 6, 6);
    }
    for (i = 0; i < 6; i++) {
        snprintf(confirm_wifi_mac + 2 * i, 3, "%.2X", metadata->WifiMac[i]);
        snprintf(confirm_bt_mac + 2 * i, 3, "%.2X", metadata->BtMac[i]);
    }

    return 0;
}

int product_line_cmd_burn_key(const char *key)
{
    char tmp[3] = {0};
    unsigned char key_u8[32] = {0};
    int i = 0;
    metadata_s *metadata = NULL;

    if (key == NULL || strlen(key) != 64) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    //parse key 1234567812345678123456781234567812345678123456781234567812345678
    memset(key_u8, 0, sizeof(key_u8));
    for (i = 0; i < 32; i++) {
        tmp[0] = key[i * 2];
        tmp[1] = key[i * 2 + 1];
        tmp[2] = 0;
        key_u8[i] = strtoul(tmp, NULL, 16) & 0xFF;
    }
    metadata_get(&metadata);
    if (dqa_test_script.burn_debug == 0) {
        if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_AES_KEY, key_u8, 32, 0) < 0) {
            for (i = 0; i < 32; i++) {
                if (metadata->AesKeys[0].Key[i] != 0) {
                    return 0;
                }
            }
            return -1;
        }
    }
    memset(metadata->AesKeys[0].Key, 0, sizeof(metadata->AesKeys[0].Key));
    memcpy(metadata->AesKeys[0].Key, key_u8, 32);

    return 0;
}

int product_line_cmd_enable_secure_boot(void)
{
#if defined(CONFIG_APP_FIRMWARE_FOR_MP)
    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_SECURE_BOOT_PUBLIC_KEY, secure_boot_otp_key, 512, 0) < 0) {
        debug_line("%s write otp public key failed", __func__);
        return -1;
    }
    if (metadata_enable_otp_secure_boot() < 0) {
        debug_line("%s enable otp failed", __func__);
        return -1;
    }
    return 0;
#else
    //(void)secure_boot_otp_key;
    return -1;
#endif
}

int product_line_cmd_rssi_get(const char *ssid, int *rssi)
{
    char cmd[1024] = {0};
    char szResponse[1024] = {0};

    if (ssid == NULL || rssi == NULL) {
        return -1;
    }
    memset(cmd, 0, sizeof(cmd));
    //snprintf(cmd, sizeof(cmd) - 1, "wpa_cli scan");
    snprintf(cmd, sizeof(cmd) - 1, "wl scan");
    if (app_helper.linux_system(cmd) < 0) {
        return -1;
    }
    sleep(3);
    memset(cmd, 0, sizeof(cmd));
    //snprintf(cmd, sizeof(cmd) - 1, "wpa_cli scan_result | grep \"%s\" | head -1 | awk -F' ' '{print $3}' | tr -d '\n'", ssid);
    snprintf(cmd, sizeof(cmd) - 1, "wl scanresults | grep -A1 '^SSID: \"%s\"$' | grep RSSI: | awk -F' ' '{print $4}'", ssid);
    memset(szResponse, 0, sizeof(szResponse));
    if (app_helper.linux_system_with_response(cmd, szResponse) >= 0) {
        debug_line("**%s**,len=%d", szResponse, strlen(szResponse));
        if (strlen(szResponse) > 0) {
            if (strspn(szResponse, "-0123456789") == strlen(szResponse)) {
                *rssi = atoi(szResponse);
                return 0;
            }
        }
    }

    return -1;
}

int product_line_cmd_speech_auth(const char *router_ssid, const char *router_password)
{
    (void)router_ssid;
    (void)router_password;

    return 0;
}

int product_line_cmd_get_version(char *version, int size)
{
    snprintf(version, size - 1, "%s", PROJECT_INFO_SW_VERSION);

    return 0;
}

int product_line_cmd_get_wifi_mac(char *wifi_mac, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --wifi_mac", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(wifi_mac, size, "%s", response);

    return 0;
}

int product_line_cmd_get_bt_mac(char *bt_mac, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --bt_mac", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(bt_mac, size, "%s", response);

    return 0;
}

int product_line_cmd_get_usb_wifi_mac(char *wifi_mac, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --usb_wifi_mac", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(wifi_mac, size, "%s", response);

    return 0;
}

int product_line_cmd_get_lte_imei(char *imei, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --imei", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(imei, size, "%s", response);

    return 0;
}

int product_line_cmd_get_lte_sim_iccid(char *iccid, int size)
{
    char response[1024] = {0};

    if (app_helper.lte_booted == 0) {
        return -1;
    }
    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --iccid", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(iccid, size, "%s", response);

    return 0;
}

int product_line_cmd_get_lte_sim_imsi(char *imsi, int size)
{
    char response[1024] = {0};

    if (app_helper.lte_booted == 0) {
        return -1;
    }
    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --imsi", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(imsi, size, "%s", response);

    return 0;
}


int product_line_cmd_get_lte_version(char *lte_ver, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
    if (app_helper.linux_system_with_response("devid --lte_ver", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(lte_ver, size, "%s", response);

    return 0;
}

int product_line_cmd_get_lte_csq(char *csq, int size)
{
    char response[1024] = {0};

    memset(response, 0, sizeof(response));
#if 1
    if (app_helper.linux_system_with_response("devid --csq", response) < 0) {
        return -1;
    }
    if (strcmp(response, "N/A") == 0) {
        return -1;
    }
    snprintf(csq, size, "%s", response);
#else
    if (app_helper.linux_system_with_response("cat /sys/kernel/debug/usb/devices | grep \"EG95-EX\"", response) < 0) {
        return -1;
    }
    if (strstr(response, "EG95-EX") != NULL) {
        memset(response, 0, sizeof(response));
        if (app_helper.linux_system_with_response("devid --csq", response) < 0) {
            return -1;
        }
        if (strcmp(response, "N/A") == 0) {
            return -1;
        }
        snprintf(csq, size, "%s", response);
    } else {
        snprintf(csq, size, "%s", "-1,-1");
    }
#endif

    return 0;
}

int product_line_cmd_get_battery(unsigned int *adc)
{
    if (adc != NULL) {
        *adc = battery_task_get_adc();
    }

    return battery_task_get_percentage();
}

int product_line_cmd_get_sn(char *sn,  int size)
{
    metadata_s *data = NULL;

    metadata_get(&data);
    if (strlen((char *)data->SerialNumber) <= 0 && dqa_test_script.sn_fake) {
        snprintf(sn, size - 1, "%s", "XXXXXXXXXXXXX");
    } else {
        snprintf(sn, size - 1, "%s", (char *)data->SerialNumber);
    }

    return 0;
}

int product_line_cmd_get_speech_auth(void)
{
    return 0;
}

int product_line_cmd_format_sd(void)
{
    app_helper.internal_camera_auto_record = 0;
    app_helper.external_camera_auto_record = 0;
    rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, NULL);
    //app_msg_queue_send(APP_MSG_ID_FORMAT_SD, 0, 0, 0);
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    if (AmbaFS_Format(EMMC_SLOT[0]) != AMBA_FS_ERR_NONE) {
        return -1;
    }
#endif
    if (AmbaFS_Format(SD_SLOT[0]) != AMBA_FS_ERR_NONE) {
        return -1;
    }

    return 0;
}

int product_line_cmd_default_setting(void)
{
    user_setting_to_default();

    return 0;
}

int product_line_cmd_power_off(void)
{
    AmbaPrint_Flush();
    app_helper.force_power_off = 1;
    app_helper.do_power_off();

    return 0;
}

int product_line_cmd_change_lcd_color(int r, int g, int b)
{
    (void)r;
    (void)g;
    (void)b;

    return 0;
}

int product_line_cmd_lcd_test_done(void)
{
    return 0;
}

int product_line_cmd_get_wifi_info(char *ssid, char *password, char *ip)
{
    wifi_ap_conf_items_s wifi_info;

    if (ssid == NULL || password == NULL) {
        return -1;
    }
    if (wifi_get_ap_info(&wifi_info) < 0) {
        return -1;
    }
    memcpy(ssid, wifi_info.ssid, strlen(wifi_info.ssid));
    memcpy(password, wifi_info.password, strlen(wifi_info.password));
    if (ip != NULL) {
        memcpy(ip, wifi_info.ip, strlen(wifi_info.ip));
    }
    return 0;
}

int product_line_cmd_get_wifi2_info(char *ip)
{
    if (app_helper.usb_wifi_booted == 0) {
        return -1;
    }
    app_helper.get_interface_ip("rtl0", ip);
    if (strlen(ip) <= 0) {
        return -1;
    }
    return 0;
}

int product_line_cmd_get_iperf_port(void)
{
    return 5001;
}

int product_line_cmd_iperf_run_impl(void)
{
    app_helper.linux_system("killall iperf");
    msleep(500);
    app_helper.linux_system("iperf -s&");

    return 0;
}

int product_line_cmd_get_4g_info(char *sim, char *imei, int *csq, int *ping_ok)
{
    char response[1024] = {0};
    //char ip[32] = {0};

    if (sim == NULL || imei == NULL || csq == NULL || ping_ok == NULL) {
        return -1;
    }
    if (app_helper.lte_booted == 0) {
        return -1;
    }
    //get ping
#if 1
    *ping_ok = 1;
#else
    memset(ip, 0, sizeof(ip));
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    if (app_helper.get_interface_ip("usb0", ip) >= 0 && strlen(ip) > 0) 
#else
    if (app_helper.get_interface_ip("wwan0", ip) >= 0 && strlen(ip) > 0) {
#endif
	{
        *ping_ok = 1;
    } else {
        *ping_ok = 0;
    }
#endif
    //get sim
    memset(response, 0, sizeof(response));
    if (product_line_cmd_get_lte_sim_iccid(response, sizeof(response) - 1) >= 0) {
        memcpy(sim, response, strlen(response));
    }
    //get imei
    memset(response, 0, sizeof(response));
    if (product_line_cmd_get_lte_imei(response, sizeof(response) - 1) >= 0) {
        memcpy(imei, response, strlen(response));
    }
    //get csq
    memset(response, 0, sizeof(response));
    if (product_line_cmd_get_lte_csq(response, sizeof(response) - 1) >= 0) {
        unsigned int i = 0;
        char tmp[32] = {0};
        memset(tmp, 0, sizeof(tmp));
        for (i = 0; i < strlen(response); i++) {
            if (response[i] == ',') {
                break;
            }
            tmp[i] = response[i];
        }
        *csq = atoi(tmp);
    }

    return 0;
}

typedef void (*print_func)(char *);
static print_func _print_func = NULL;

int product_line_cmd_bt_check(void)
{
    if (app_helper.bt_booted == 0) {
        return -2;
    }
    if (app_helper.check_bt_is_ready() == 0) {
        return -1;
    }
    return 0;
}

int product_line_cmd_get_all_product_info(char *response, int size)
{
    char wifi_mac[32] = {0};
    char bt_mac[32] = {0};
    char usb_wifi_mac[32] = {0};
    char lte_version[64] = {0};
    char sn[32] = {0};
    char sn2[32] = {0};
    char time[32] = {0};
    char imei[64] = {0};
    device_info_s *device_info = NULL;
    char iccid[64] = {0};
    char imsi[64] = {0};

    memset(time, 0, sizeof(time));
    memset(sn, 0, sizeof(sn));
    memset(wifi_mac, 0, sizeof(wifi_mac));
    memset(bt_mac, 0, sizeof(bt_mac));
    memset(sn2, 0, sizeof(sn2));
    memset(usb_wifi_mac, 0, sizeof(usb_wifi_mac));
    memset(lte_version, 0, sizeof(lte_version));
    memset(imei, 0, sizeof(imei));
    memset(iccid, 0, sizeof(iccid));
    memset(imsi, 0, sizeof(imsi));
    if (dqa_test_script.product_line_mode == 2) {
        if (app_helper.lte_booted == 0
            /*|| app_helper.usb_wifi_booted == 0*/) {
            return 0;
        }
        product_line_cmd_get_sn2(sn2, sizeof(sn2));
        product_line_cmd_get_usb_wifi_mac(usb_wifi_mac, sizeof(usb_wifi_mac));
        product_line_cmd_get_lte_version(lte_version, sizeof(lte_version));
        product_line_cmd_get_lte_imei(imei, sizeof(imei));
        product_line_cmd_get_lte_sim_iccid(iccid, sizeof(iccid));
        product_line_cmd_get_lte_sim_imsi(imsi, sizeof(imsi));
        device_info = app_helper.get_device_info();
        snprintf(response, size, "%s %s %s %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\"",
                                    "factory", "product_info", "all",
                                    "mcu_version",  device_info->mcu_version2,
                                    "4g_version",   lte_version,
                                    "wifi_mac",     usb_wifi_mac,
                                    "imei",         imei,
                                    "sim",          iccid,
                                    "imsi",         imsi,
                                    "sn",           sn2
                                    );
    } else {
        unsigned int adc = 0;
        SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom;
        char tmp_buf[128] = {0};

        if (app_helper.wifi_booted == 0
            || app_helper.bt_booted == 0) {
            return 0;
        }
        product_line_cmd_get_time(time, sizeof(time));
        product_line_cmd_get_wifi_mac(wifi_mac, sizeof(wifi_mac));
        product_line_cmd_get_bt_mac(bt_mac, sizeof(bt_mac));
        product_line_cmd_get_sn(sn, sizeof(sn));
        device_info = app_helper.get_device_info();
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (app_helper.check_lte_left_connected() || app_helper.check_lte_right_connected()) 
#endif
		{
            if (app_helper.lte_booted == 0
                /*|| app_helper.usb_wifi_booted == 0*/) {
                return 0;
            }
            product_line_cmd_get_sn2(sn2, sizeof(sn2));
            product_line_cmd_get_usb_wifi_mac(usb_wifi_mac, sizeof(usb_wifi_mac));
            product_line_cmd_get_lte_version(lte_version, sizeof(lte_version));
            product_line_cmd_get_lte_imei(imei, sizeof(imei));
            product_line_cmd_get_lte_sim_iccid(iccid, sizeof(iccid));
            product_line_cmd_get_lte_sim_imsi(imsi, sizeof(imsi));
        }
        SvcUserPrefCustom_Get(&pSvcUserPrefCustom);

        {
            IMU_DATA_s imu_data;
            product_line_cmd_get_imu_data(&imu_data);
            memset(tmp_buf, 0, sizeof(tmp_buf));
            snprintf(tmp_buf, sizeof(tmp_buf) - 1, "%s|accel:%.2f,%.2f,%.2f|gyro:%.2f,%.2f,%.2f",
                                                            pSvcUserPrefCustom->ImuCalibrationData.imu_calibration_flag ? "ok" : "fail",
                                                            imu_data.accel.x_float, imu_data.accel.y_float, imu_data.accel.z_float,
                                                            imu_data.gyro.x_float, imu_data.gyro.y_float, imu_data.gyro.z_float);
        }

        snprintf(response, size, "%s %s %s %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:\"%s\" %s:%s %s:%d:%d",
                                            "factory", "product_info", "all",
                                            "version",      device_info->sw_version,
                                            "build_time",   device_info->build_time,
                                            "mcu_version",  device_info->mcu_version,
                                            "time",         time,
                                            "4g_version",   lte_version,
                                            "wifi_mac",     wifi_mac,
                                            "bt_mac",       bt_mac,
                                            "wifi2_mac",    usb_wifi_mac,
                                            "imei",         imei,
                                            "sim",          iccid,
                                            "imsi",         imsi,
                                            "sn",           sn,
                                            "sn2",          sn2,
                                            "mcu2_version", device_info->mcu_version2,
                                            "imu_calibration", tmp_buf,
                                            "battery",      product_line_cmd_get_battery(&adc), adc
                                            );
    }

    return 0;
}

int product_line_cmd_camera_check(int index)
{
    if (index == 0) {
        return (app_helper.check_camera_is_working(CAMERA_CHANNEL_INTERNAL) == 1) ? 0 : -1;
    }
    if (index == 1) {
        return (app_helper.check_camera_is_working(CAMERA_CHANNEL_EXTERNAL) == 1) ? 0 : -1;
    }

    return -1;
}

static int do_audio_test(int len, int *channels, int *sample_width, int *sample_rate, char *url, char *url1,char *url2)
{
    if (len <= 0) {
        debug_line("%s invalid len: %d", __func__, len);
        return -1;
    }
    remove(AUDIO_TEST_PATH);
    remove(AUDIO_TEST_PATH1);
    remove(AUDIO_TEST_PATH2);
    AmbaAudio_CodecSetInput(0, AUCODEC_AMIC_IN);
    msleep(500);
    beep_play(BEEP_ID_1K);
    pcm_record_set_path(AUDIO_TEST_PATH);
    pcm_record_start();
    sleep(len);
    pcm_record_stop();
    beep_stop();
    beep_wait_finish(2 * len * 1000);
    pcm_record_wait_finish();

    AmbaAudio_CodecSetInput(0, AUCODEC_DMIC_IN);
    msleep(500);
    beep_play(BEEP_ID_1K);
    pcm_record_set_path(AUDIO_TEST_PATH1);
    pcm_record_start();
    sleep(len);
    pcm_record_stop();
    beep_stop();
    beep_wait_finish(2 * len * 1000);
    pcm_record_wait_finish();

    AmbaAudio_CodecSetInput(0, AUCODEC_DMIC2_IN);
    msleep(500);
    beep_play(BEEP_ID_1K);
    pcm_record_set_path(AUDIO_TEST_PATH2);
    pcm_record_start();
    sleep(len);
    pcm_record_stop();
    beep_stop();
    beep_wait_finish(2 * len * 1000);
    pcm_record_wait_finish();

    *channels = 2;
    *sample_width = 2;
    *sample_rate = 48000;
    snprintf(url, 64, "http://%s:%d/mnt/extsd/%s", WIFI_AP_IP, 8081, AUDIO_TEST_FILENAME);
    snprintf(url1, 64, "http://%s:%d/mnt/extsd/%s", WIFI_AP_IP, 8081, AUDIO_TEST_FILENAME1);
    snprintf(url2, 64, "http://%s:%d/mnt/extsd/%s", WIFI_AP_IP, 8081, AUDIO_TEST_FILENAME2);

    return 0;
}

static int product_line_do_audio_test(int len)
{
    char ssid[64] = {0};
    char password[64] = {0};

    app_helper.linux_system("killall amba_http_server;amba_http_server /etc/config.ini");
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    if (product_line_cmd_get_wifi_info(ssid, password, NULL) >= 0) {
        char url[64] = {0};
        char url1[64] = {0};
        char url2[64] = {0};
        int channels = 0, sample_width = 0, sample_rate = 0;
        memset(url, 0, sizeof(url));
        memset(url1, 0, sizeof(url1));
        memset(url2, 0, sizeof(url2));
        if (do_audio_test(len, &channels, &sample_width, &sample_rate, url, url1, url2) >= 0) {
            char response[1024] = {0};
            memset(response, 0, sizeof(response));
            snprintf(response, sizeof(response) - 1, "%s %s %s channels:%d sample_width:%d sample_rate:%d ssid:\"%s\" password:\"%s\" url:\"%s\" url2:\"%s\" url3:\"%s\"",
                                                                                                    "factory", "record_playback_auto", "ok",
                                                                                                    channels,
                                                                                                    sample_width,
                                                                                                    sample_rate,
                                                                                                    ssid,
                                                                                                    password,
                                                                                                    url, url1, url2);
            if (_print_func) {
                (*_print_func)(response);
            }
            return 0;
        }
    }
    return -1;
}

static int busy = 0;
static int sound_len = 0;
static void product_line_rec_audio_auto_timer_handler(int eid)
{
    char ssid[64] = {0};
    char password[64] = {0};

    if (eid == TIMER_UNREGISTER) {
        return;
    }
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    if (product_line_cmd_get_wifi_info(ssid, password, NULL) >= 0) {
        app_timer_unregister(TIMER_1HZ, product_line_rec_audio_auto_timer_handler);
        product_line_do_audio_test(sound_len);
        busy = 0;
        return;
    }
}

int product_line_cmd_record_playback_auto(void (*func)(char *), int len)
{
    char ssid[64] = {0};
    char password[64] = {0};
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));

    if (busy) {
        return 0;
    }
    _print_func = (print_func)func;
    if (product_line_cmd_get_wifi_info(ssid, password, NULL) >= 0) {
        product_line_do_audio_test(len);
        return 0;
    }
    sound_len = len;
    app_timer_register(TIMER_1HZ, product_line_rec_audio_auto_timer_handler);
    busy = 1;

    return 0;
}

static int convert(unsigned short value)
{
    int val = value;
    if (value > 0x7fff) {
        val = -(0xffff - value);
    }
    return val;
}

static int imu_get_accel_offset(unsigned char range, unsigned char level, OFFSET_DATA_s *data)
{
    int i = 0, x_sum = 0, y_sum = 0, z_sum = 0;
    IMU_DATA_s imu_data;

    Imu_SetAccelPara(IMU_ACCEL_RANGE, range);
    msleep(200);
    for (i = 0; i < 200; i++) {
        Imu_GetData(&imu_data, 1);
        x_sum += convert(imu_data.accel.x);
        y_sum += convert(imu_data.accel.y);
        z_sum += convert(imu_data.accel.z);
        msleep(10);
    }
    data->x = 0 - (x_sum / 200);
    data->y = (0x8000 / level) - (y_sum / 200);
    data->z = 0 - (z_sum / 200);

    return 0;
}

static int imu_get_gyro_offset(OFFSET_DATA_s *data)
{
    int i = 0, x_sum = 0, y_sum = 0, z_sum = 0;
    IMU_DATA_s imu_data;

    for (i = 0; i < 100; i++) {
        Imu_GetData(&imu_data, 1);
        x_sum += convert(imu_data.gyro.x);
        y_sum += convert(imu_data.gyro.y);
        z_sum += convert(imu_data.gyro.z);
        msleep(5);
    }
    data->x = 0 - x_sum / 100;
    data->y = 0 - y_sum / 100;
    data->z = 0 - z_sum / 100;

    return 0;
}

int product_line_cmd_do_imu_calibration(const char *direction)
{
    SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom = NULL;

    (void)direction;
    SvcUserPrefCustom_Get(&pSvcUserPrefCustom);
    imu_get_accel_offset(BMI160_ACCEL_RANGE_2G, 2, &(pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g));
    imu_get_accel_offset(BMI160_ACCEL_RANGE_4G, 4, &(pSvcUserPrefCustom->ImuCalibrationData.accel_offset_4g));
    imu_get_accel_offset(BMI160_ACCEL_RANGE_8G, 8, &(pSvcUserPrefCustom->ImuCalibrationData.accel_offset_8g));
    imu_get_accel_offset(BMI160_ACCEL_RANGE_16G, 16, &(pSvcUserPrefCustom->ImuCalibrationData.accel_offset_16g));
    imu_get_gyro_offset(&(pSvcUserPrefCustom->ImuCalibrationData.gyro_offset));
    pSvcUserPrefCustom->ImuCalibrationData.imu_calibration_flag = 1;
    SvcUserPrefCustom_Save();
    debug_line("imu calibration done!");

    return 0;
}

int product_line_cmd_usb_port_test(const char *left_right, const char *up_down)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (strcmp(left_right, "switch") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->UsbMassStorage = OPTION_ON;
        user_setting_save();
        Pmic_DoPowerOff(0);
        return -1;
    }
    if (strcmp(left_right, "left") == 0) {
        if (app_helper.check_usb_left_connected() && app_helper.usb_mass_storage_mode) {
            return 0;
        }
    } else if (strcmp(left_right, "right") == 0) {
        if (app_helper.check_usb_right_connected() && app_helper.usb_mass_storage_mode) {
            return 0;
        }
    }
#endif
    return -1;
}

int product_line_cmd_addon_port_test(const char *left_right)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (strcmp(left_right, "left") == 0) {
        if (app_helper.check_lte_left_connected() && app_helper.lte_on
            && app_helper.check_gc2053_right_connected() && app_helper.check_camera_is_working(CAMERA_CHANNEL_EXTERNAL) == 1) {
            return 0;
        }
    } else if (strcmp(left_right, "right") == 0) {
        if (app_helper.check_lte_right_connected() && app_helper.lte_on
            && app_helper.check_gc2053_left_connected() && app_helper.check_camera_is_working(CAMERA_CHANNEL_EXTERNAL) == 1) {
            return 0;
        }
    }
#endif
    return -1;
}

int product_line_cmd_timer_wakeup_test(void)
{
    app_helper.force_power_off = 1;
    app_helper.timer_wakeup_test = 1;
    app_helper.do_power_off();

    return 0;
}

int product_line_cmd_lte_wakeup_test(void)
{
    app_helper.force_power_off = 1;
    app_helper.lte_wakeup_test = 1;
    app_helper.do_power_off();

    return 0;
}

int product_line_cmd_is_lte_wakeup(void)
{
    return (Pmic_GetBootReason() == BOOT_REASON_LTE_TRIGGER) ? 1 : 0;
}

int product_line_cmd_is_timer_wakeup(void)
{
    return (Pmic_GetBootReason() == BOOT_REASON_TIMER_TRIGGER) ? 1 : 0;
}

