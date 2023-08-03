#include "dqa_test_uart.h"

extern int dqa_test_uart_init_impl(const char *key, dqa_test_uart_cmd_cb_func func);
extern int dqa_test_uart_get_sw_version_impl(char *ver);
extern int dqa_test_uart_get_mcu_version_impl(char *ver);
extern int dqa_test_uart_get_sd_exist_impl(void);
extern int dqa_test_uart_get_sd_space_impl(unsigned int *free_size_mb, unsigned int *total_size_mb);
extern int dqa_test_uart_get_dcf_impl(unsigned int *loop_num, unsigned int *event_num, unsigned int *photo_num);
extern int dqa_test_uart_record_start_impl(void);
extern int dqa_test_uart_record_stop_impl(void);
extern int dqa_test_uart_event_record_start_impl(void);
extern dqa_test_uart_record_state_e dqa_test_uart_get_record_state_impl(void);
extern int dqa_test_uart_take_photo_impl(int vin_index);
extern int dqa_test_uart_format_sd_impl(void);
extern int dqa_test_uart_default_setting_impl(void);
extern int dqa_test_uart_reboot_impl(void);
extern int dqa_test_uart_wifi_on_impl(void);
extern int dqa_test_uart_wifi_off_impl(void);
extern dqa_test_uart_wifi_state_e dqa_test_uart_get_wifi_state_impl(void);
extern int dqa_test_uart_get_wifi_info(char *ssid, char *password, char *ip);
extern int dqa_test_uart_get_ota_path(char *path);
extern int dqa_test_uart_check_lte_impl(void);
extern int dqa_test_uart_get_lte_ip_impl(char *ip);
extern int dqa_test_uart_check_bt_impl(void);

static int dqa_test_uart_cmd_handler(int argc, char **argv, dqa_test_uart_print_func func);

#define CMD_LINE_BUF_SIZE (1024)
static dqa_test_uart_print_func print_func = NULL;
static void dqa_test_uart_print(char *response)
{
    char output[1024] = {0};

    memset(output, 0, sizeof(output));
    snprintf(output, sizeof(output) - 1, "\n%s\n", response);
    if (print_func == NULL) {
        debug_line(output);AmbaPrint_Flush();
    } else {
        (*print_func)(output);
    }
}

#define DQA_TEST_CMD_KEY "dqa_test"
int dqa_test_uart_init(void)
{
    dqa_test_uart_init_impl(DQA_TEST_CMD_KEY, dqa_test_uart_cmd_handler);

    return 0;
}

static int take_photo_flag = 0;
int dqa_test_uart_get_take_photo_flag(void)
{
    return take_photo_flag;
}

int dqa_test_uart_set_take_photo_flag(int flag)
{
    take_photo_flag = flag;
    return 0;
}

int dqa_test_uart_take_photo_done(int success, int vin_index, const char *http_path)
{
    if (take_photo_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        if (success) {
            snprintf(response, sizeof(response) - 1, "%s %s ok %d:%s", DQA_TEST_CMD_KEY, "take_photo", vin_index, (http_path == NULL) ? "" : http_path);
        } else {
            snprintf(response, sizeof(response) - 1, "%s %s fail", DQA_TEST_CMD_KEY, "take_photo");
        }
        dqa_test_uart_print(response);
    }
    return 0;
}

static int default_setting_flag = 0;
int dqa_test_uart_default_setting_done(void)
{
    if (default_setting_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s ok", DQA_TEST_CMD_KEY, "default_setting");
        dqa_test_uart_print(response);
    }
    default_setting_flag = 0;
    return 0;
}

static int reboot_flag = 0;
int dqa_test_uart_reboot_done(void)
{
    if (reboot_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s ok", DQA_TEST_CMD_KEY, "reboot");
        dqa_test_uart_print(response);
    }    
    reboot_flag = 0;
    return 0;
}

static int event_record_flag = 0;
int dqa_test_uart_event_record_done(void)
{
    if (event_record_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s ok", DQA_TEST_CMD_KEY, "event_record");
        dqa_test_uart_print(response);
    }
    event_record_flag = 0;
    return 0;
}

static int format_sd_flag = 0;
int dqa_test_uart_format_sd_done(int success)
{
    if (format_sd_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s %s", DQA_TEST_CMD_KEY, "format_sd", success ? "ok" : "fail");
        dqa_test_uart_print(response);
    }    
    format_sd_flag = 0;
    return 0;
}

static int dqa_test_uart_cmd_handler(int argc, char **argv, dqa_test_uart_print_func func)
{
    char response[CMD_LINE_BUF_SIZE] = {0};

    print_func = func;
    memset(response, 0, sizeof(response));
    if (argc <= 1) {
        snprintf(response, sizeof(response) - 1, "%s invalid param", __func__);
        goto end;
    }

    if (strcmp(argv[0], DQA_TEST_CMD_KEY) != 0) {        
        snprintf(response, sizeof(response) - 1, "%s invalid cmd", __func__);
        goto end;
    }

    if (strcmp(argv[1], "build_info") == 0) {
        snprintf(response, sizeof(response) - 1, "%s %s build_time:\"%s\" git_branch:\"%s\" git_commit_id:\"%s\"", argv[0],
                                                                        argv[1],
                                                                        PROJECT_INFO_BUILD_TIME,
                                                                        PROJECT_INFO_GIT_BRANCH,
                                                                        PROJECT_INFO_GIT_COMMIT_ID);
    } else if (strcmp(argv[1], "ver") == 0) {
        char sw_ver[32] = {0};
        char mcu_ver[32] = {0};
        memset(sw_ver, 0, sizeof(sw_ver));
        dqa_test_uart_get_sw_version_impl(sw_ver);
        memset(mcu_ver, 0, sizeof(mcu_ver));
        dqa_test_uart_get_mcu_version_impl(mcu_ver);
        snprintf(response, sizeof(response) - 1, "%s %s sw_version:\"%s\" mcu_version:\"%s\" project:\"%s/%s/%s\"", argv[0],
                                                                    argv[1],
                                                                    sw_ver,
                                                                    mcu_ver,
                                                                    PLATFORM_NAME,
                                                                    CUSTOMER_NAME,
                                                                    PROJECT_NAME);
    } else if (strcmp(argv[1], "sd_exist") == 0) {
        snprintf(response, sizeof(response) - 1, "%s %s %d", argv[0], argv[1], dqa_test_uart_get_sd_exist_impl() ? 1 : 0);
    } else if (strcmp(argv[1], "sd_space") == 0) {
        unsigned int free_size = 0, total_size = 0;
        if (dqa_test_uart_get_sd_space_impl(&free_size, &total_size) >= 0) {
            snprintf(response, sizeof(response) - 1, "%s %s %s %d/%d", argv[0], argv[1], "ok", free_size, total_size);
        } else {
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "dcf") == 0) {
        unsigned int loop_num = 0, event_num = 0, photo_num = 0;
        if (dqa_test_uart_get_dcf_impl(&loop_num, &event_num, &photo_num) >= 0) {
            snprintf(response, sizeof(response) - 1, "%s %s loop_video:%d event_video:%d photo:%d", argv[0], argv[1], loop_num, event_num, photo_num);
        } else {
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "record") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            if (dqa_test_uart_record_start_impl() >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "ok");
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
            }
        } else if (strcmp(argv[2], "stop") == 0) {
            if (dqa_test_uart_record_stop_impl() >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "ok");
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
            }
        } else if (strcmp(argv[2], "state") == 0) {
            switch(dqa_test_uart_get_record_state_impl()) {
            case DQA_TEST_UART_RECORD_STATE_IDLE:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "idle");
                break;
            case DQA_TEST_UART_RECORD_STATE_LOOP_RECORD:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "loop_record");
                break;
            case DQA_TEST_UART_RECORD_STATE_EVENT_RECORD:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "event_record");
                break;
            case DQA_TEST_UART_RECORD_STATE_PARKING_RECORD:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "event_record");
                break;
            default:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "unknown");
                break;
            }
        }
    } else if (strcmp(argv[1], "event_record") == 0) {
        event_record_flag = 1;
        if (dqa_test_uart_event_record_start_impl() < 0) {
            event_record_flag = 0;
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "take_photo") == 0) {
        take_photo_flag = 1;
        if (dqa_test_uart_take_photo_impl(atoi(argv[2])) < 0) {
            take_photo_flag = 0;
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "format_sd") == 0) {
        format_sd_flag = 1;
        if (dqa_test_uart_format_sd_impl() < 0) {
            format_sd_flag = 0;
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "default_setting") == 0) {
        default_setting_flag = 1;
        if (dqa_test_uart_default_setting_impl() < 0) {
            default_setting_flag = 0;
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "reboot") == 0) {
        reboot_flag = 1;
        if (dqa_test_uart_reboot_impl() < 0) {
            reboot_flag = 0;            
            snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "wifi") == 0) {
        if (strcmp(argv[2], "on") == 0) {
            if (dqa_test_uart_wifi_on_impl() >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        } else if (strcmp(argv[2], "off") == 0) {
            if (dqa_test_uart_wifi_off_impl() >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        } else if (strcmp(argv[2], "state") == 0) {
            switch(dqa_test_uart_get_wifi_state_impl()) {
            case DQA_TEST_UART_WIFI_STATE_ON:            
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "on");
                break;
            case DQA_TEST_UART_WIFI_STATE_OFF:            
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "off");
                break;
            case DQA_TEST_UART_WIFI_STATE_BUSY:
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "busy");
                break;
            default:            
                snprintf(response, sizeof(response) - 1, "%s %s %s", argv[0], argv[1], "unknown");
                break;
            }
        } else if (strcmp(argv[2], "ap_info") == 0) {
            char ap_ssid[32] = {0};
            char ap_password[32] = {0};
            char ap_ip[32] = {0};
            if (dqa_test_uart_get_wifi_info(ap_ssid, ap_password, ap_ip) >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s ssid:\"%s\" password:%s ip:%s", argv[0], argv[1], argv[2], "ok", ap_ssid, ap_password, ap_ip);
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        } else if (strcmp(argv[2], "ota") == 0) {
            char path[128] = {0};
            memset(path, 0, sizeof(path));
            dqa_test_uart_get_ota_path(path);
            snprintf(response, sizeof(response) - 1, "%s %s url:\"%s\"", argv[0], argv[1], path);
        }
    } else if (strcmp(argv[1], "lte") == 0) {
        if (strcmp(argv[2], "check") == 0) {
            if (dqa_test_uart_check_lte_impl()) {
                char ip[32] = {0};
                memset(ip, 0, sizeof(ip));
                dqa_test_uart_get_lte_ip_impl(ip);
                snprintf(response, sizeof(response) - 1, "%s %s %s %s ip:%s", argv[0], argv[1], argv[2], "ok", ip);
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        }
    } else if (strcmp(argv[1], "bt") == 0) {
        if (strcmp(argv[2], "check") == 0) {
            if (dqa_test_uart_check_bt_impl()) {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
            } else {
                snprintf(response, sizeof(response) - 1, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        }
    } else {
        snprintf(response, sizeof(response) - 1, "%s not support", __func__);
    }

end:
    if (strlen(response) > 0) {
        dqa_test_uart_print(response);
    }

    return 0;
}

void show_build_info(void)
{
    debug_line("*****************************************************************");
#if defined(PROJECT_INFO_SW_VERSION)
    debug_line("* Device Version:     %s", PROJECT_INFO_SW_VERSION);
#endif
#if defined(PROJECT_INFO_BUILD_HOST) && defined(PROJECT_INFO_BUILD_TIME)
    debug_line("* Build Machine:      %s at %s", PROJECT_INFO_BUILD_HOST, PROJECT_INFO_BUILD_TIME);
#endif
#if defined(PROJECT_INFO_GIT_BRANCH) && defined(PROJECT_INFO_GIT_COMMIT_ID)
    debug_line("* Build Git Revison:  %s@%s", PROJECT_INFO_GIT_COMMIT_ID, PROJECT_INFO_GIT_BRANCH);
#endif
    debug_line("*****************************************************************");
    AmbaPrint_Flush();
}


