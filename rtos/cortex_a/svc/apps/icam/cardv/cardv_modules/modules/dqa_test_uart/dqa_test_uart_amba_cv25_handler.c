#include "dqa_test_uart.h"
#include "AmbaShell.h"
#include "SvcCmd.h"

static dqa_test_uart_cmd_cb_func uart_cmd_func = NULL;
static void amba_uart_cmd_main_func(unsigned int argc, char * const *argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)PrintFunc;
    if (uart_cmd_func) {
        (*uart_cmd_func)(argc, (char **)argv, NULL);
    }
}

int dqa_test_uart_init_impl(const char *key, dqa_test_uart_cmd_cb_func func)
{
    AMBA_SHELL_COMMAND_s  Cmd;
    UINT32  Rval = 0;

    uart_cmd_func = func;
    Cmd.pName    = key;
    Cmd.MainFunc = amba_uart_cmd_main_func;
    Cmd.pNext    = NULL;
    Rval = SvcCmd_CommandRegister(&Cmd);
    if (SHELL_ERR_SUCCESS != Rval) {
        debug_line("%s: fail to register cmd", __func__);
        return -1;
    }
    AmbaShell_EnableEcho(0);

    return 0;
}

int dqa_test_uart_get_sw_version_impl(char *ver)
{
    device_info_s *info = app_helper.get_device_info();
    memcpy(ver, info->sw_version, strlen(info->sw_version));
    return 0;
}

int dqa_test_uart_get_mcu_version_impl(char *ver)
{
    device_info_s *info = app_helper.get_device_info();
    memcpy(ver, info->mcu_version, strlen(info->mcu_version));
    return 0;
}

int dqa_test_uart_get_sd_exist_impl(void)
{
    return app_helper.check_sd_exist();
}

int dqa_test_uart_get_sd_space_impl(unsigned int *free_size_mb, unsigned int *total_size_mb)
{
    return app_helper.get_sd_space(total_size_mb, free_size_mb);
}

int dqa_test_uart_get_dcf_impl(unsigned int *loop_num, unsigned int *event_num, unsigned int *photo_num)
{
    (*loop_num) = 10;
    (*event_num) = 20;
    (*photo_num) = 30;

    return 0;
}

int dqa_test_uart_record_start_impl(void)
{
    return 0;
}

int dqa_test_uart_record_stop_impl(void)
{
    return 0;
}

int dqa_test_uart_event_record_start_impl(void)
{
    app_msg_queue_send(APP_MSG_ID_CAM_START_EVENT_RECORD, 10, 25, 0);
    return 0;
}

dqa_test_uart_record_state_e dqa_test_uart_get_record_state_impl(void)
{
    if (rec_dvr_is_recording(CAMERA_CHANNEL_BOTH)) {
        return DQA_TEST_UART_RECORD_STATE_LOOP_RECORD;
    }
    return DQA_TEST_UART_RECORD_STATE_IDLE;
}

int dqa_test_uart_take_photo_impl(int vin_index)
{
    (void)vin_index;
    app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_PIV_MJPG, 0, 0, 0);
    return 0;
}

int dqa_test_uart_format_sd_impl(void)
{
    app_msg_queue_send(APP_MSG_ID_FORMAT_SD, 0, 0, 0);
    return 0;
}

int dqa_test_uart_default_setting_impl(void)
{
    app_msg_queue_send(APP_MSG_ID_DEFAULT_SETTING, 0, 0, 0);
    return 0;
}

int dqa_test_uart_reboot_impl(void)
{
    app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
    return 0;
}

int dqa_test_uart_wifi_on_impl(void)
{
    return -1;
}

int dqa_test_uart_wifi_off_impl(void)
{
    return -1;
}

dqa_test_uart_wifi_state_e dqa_test_uart_get_wifi_state_impl(void)
{
    return DQA_TEST_UART_WIFI_STATE_ON;
}

int dqa_test_uart_get_wifi_info(char *ssid, char *password, char *ip)
{
    wifi_ap_conf_items_s wifi_info;

    if (ssid == NULL || password == NULL || ip == NULL) {
        return -1;
    }
    if (wifi_get_ap_info(&wifi_info) < 0) {
        return -1;
    }
    memcpy(ssid, wifi_info.ssid, strlen(wifi_info.ssid));
    memcpy(password, wifi_info.password, strlen(wifi_info.password));
    memcpy(ip, wifi_info.ip, strlen(wifi_info.ip));

    return 0;
}

int dqa_test_uart_get_ota_path(char *path)
{
    sprintf(path, "http://%s:8080/ota", WIFI_AP_IP);

    return 0;
}

int dqa_test_uart_check_lte_impl(void)
{
    return app_helper.check_lte_is_ready();
}

int dqa_test_uart_get_lte_ip_impl(char *ip)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D080) || defined(CONFIG_BSP_H32_NEXAR_D081)
    return app_helper.get_interface_ip("usb0", ip);
#else
    return app_helper.get_interface_ip("wwan0", ip);
#endif
}

int dqa_test_uart_check_bt_impl(void)
{
    return app_helper.check_bt_is_ready();
}

