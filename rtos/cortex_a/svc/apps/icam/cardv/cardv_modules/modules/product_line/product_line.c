#include "product_line.h"

extern int product_line_cmd_init_impl(const char *key, product_line_cmd_cb_func func);
extern int product_line_cmd_process(unsigned char *buf, int buf_len);
extern int product_line_cmd_switch_to_wifi_conduct_test(void);
extern int product_line_cmd_switch_to_cvbs(int index);
extern int product_line_cmd_switch_to_usb_camera(int index);
extern int product_line_cmd_switch_led_on_off(int on);
extern int product_line_cmd_record_playback(int len);
extern int product_line_cmd_record_playback_auto(void (*func)(char *), int len);
extern int product_line_cmd_get_gsensor_data(int *x, int *y, int *z);
extern int product_line_cmd_get_gps_str(char *gps_str);
extern int product_line_cmd_get_gps_data(int *num, product_line_gps_info_s *infos, int max_num);
extern int product_line_cmd_get_thermal_data(int *value);
extern int product_line_cmd_get_brightness_data(int *value);
extern int product_line_cmd_set_time(const char *time);
extern int product_line_cmd_get_time(char *time, int size);
extern int product_line_cmd_burn_mac_sn(const char *wifi_mac, const char *bt_mac, const char *sn,
        char *confirm_wifi_mac, char *confirm_bt_mac, char *confirm_sn);
extern int product_line_cmd_burn_mac_sn2(const char *wifi_mac, const char *sn,
        char *confirm_wifi_mac, char *confirm_sn);
extern int product_line_cmd_burn_key(const char *key);
extern int product_line_cmd_rssi_get(const char *ssid, int *rssi);
extern int product_line_cmd_speech_auth(const char *router_ssid, const char *router_password);
extern int product_line_cmd_get_version(char *version, int size);
extern int product_line_cmd_get_wifi_mac(char *wifi_mac, int size);
extern int product_line_cmd_get_bt_mac(char *bt_mac, int size);
extern int product_line_cmd_get_usb_wifi_mac(char *wifi_mac, int size);
extern int product_line_cmd_get_lte_imei(char *imei, int size);
extern int product_line_cmd_get_battery(unsigned int *adc);
extern int product_line_cmd_get_sn(char *sn,  int size);
extern int product_line_cmd_get_sn2(char *sn,  int size);
extern int product_line_cmd_get_speech_auth(void);
extern int product_line_cmd_format_sd(void);
extern int product_line_cmd_default_setting(void);
extern int product_line_cmd_power_off(void);
extern int product_line_cmd_change_lcd_color(int r, int g, int b);
extern int product_line_cmd_lcd_test_done(void);
extern int product_line_cmd_get_wifi_info(char *ssid, char *password, char *ip);
extern int product_line_cmd_get_wifi2_info(char *ip);
extern int product_line_cmd_get_iperf_port(void);
extern int product_line_cmd_iperf_run_impl(void);
extern int product_line_cmd_get_4g_info(char *sim, char *imei, int *csq, int *ping_ok);
extern int product_line_cmd_get_build_time(char *time, int size);
extern int product_line_cmd_get_wifi_rtsp_url(int index, const char *ip, char *url);
extern int product_line_cmd_get_all_product_info(char *response, int size);
extern int product_line_cmd_bt_check(void);
extern int product_line_cmd_enable_secure_boot(void);
extern int product_line_cmd_camera_check(int index);
extern int product_line_cmd_do_imu_calibration(const char *direction);
extern int product_line_cmd_usb_port_test(const char *left_right, const char *up_down);
extern int product_line_cmd_addon_port_test(const char *left_right);
extern int product_line_cmd_get_imu_data(IMU_DATA_s *data);
extern int product_line_cmd_timer_wakeup_test(void);
extern int product_line_cmd_lte_wakeup_test(void);
extern int product_line_cmd_is_lte_wakeup(void);
extern int product_line_cmd_is_timer_wakeup(void);

static int product_line_cmd_handler(int argc, char **argv, product_line_cmd_print_func func);

#define CMD_LINE_BUF_SIZE (1024)
static product_line_cmd_print_func print_func = NULL;
static void product_line_cmd_print(char *response)
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

#define PRODUCT_LINE_CMD_KEY "factory"
int product_line_cmd_init(void)
{
    product_line_cmd_init_impl(PRODUCT_LINE_CMD_KEY, product_line_cmd_handler);

    return 0;
}

static int button_test_flag = 0;
int product_line_cmd_get_button_test_flag(void)
{
    return button_test_flag;
}

int product_line_cmd_button_test_key_pressed(int index)
{
    if (button_test_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s %d", PRODUCT_LINE_CMD_KEY, "button_test", index);
        product_line_cmd_print(response);
    }

    return 0;
}

static int record_test_flag = 0;
int product_line_cmd_record_done(void)
{
    if (record_test_flag) {
        char response[CMD_LINE_BUF_SIZE] = {0};
        memset(response, 0, sizeof(response));
        snprintf(response, sizeof(response) - 1, "%s %s %s", PRODUCT_LINE_CMD_KEY, "record_playback", "ok");
        product_line_cmd_print(response);
    }
    record_test_flag = 0;

    return 0;
}

static int product_line_cmd_handler(int argc, char **argv, product_line_cmd_print_func func)
{
    char response[CMD_LINE_BUF_SIZE] = {0};
    int size = sizeof(response) - 1;

    print_func = func;
    memset(response, 0, sizeof(response));
    if (argc <= 1) {
        goto end;
    }

    if (strcmp(argv[0], PRODUCT_LINE_CMD_KEY) != 0) {
        snprintf(response, sizeof(response) - 1, "%s invalid key[%s]", __func__, argv[0]);
        goto end;
    }

    if (strcmp(argv[1], "wifi_conduct") == 0) {
        if (product_line_cmd_switch_to_wifi_conduct_test() >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strncmp(argv[1], "usb_camera", strlen("usb_camera")) == 0) {
        int index = atoi(argv[1] + strlen("usb_camera")) - 1;
        if (product_line_cmd_switch_to_usb_camera(index) >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], "usb_camera", "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], "usb_camera", "fail");
        }
    } else if (strncmp(argv[1], "wifi_camera", strlen("wifi_camera")) == 0) {
        char ssid[128] = {0};
        char password[128] = {0};
        char ip[128] = {0};
        char url[128] = {0};
        int index = atoi(argv[1] + strlen("wifi_camera")) - 1;

        memset(ssid, 0, sizeof(ssid));
        memset(password, 0, sizeof(password));
        memset(ip, 0, sizeof(ip));
        if (product_line_cmd_get_wifi_info(ssid, password, ip) < 0) {
            goto end;
        }
        memset(url, 0, sizeof(url));
        if (product_line_cmd_get_wifi_rtsp_url(index, ip, url) < 0) {
            goto end;
        }
        snprintf(response, size, "%s %s ssid:\"%s\" password:\"%s\" url:\"%s\"", argv[0], "wifi_camera", ssid, password, url);
    } else if (strncmp(argv[1], "cvbs", strlen("cvbs")) == 0) {
        int index = atoi(argv[1] + strlen("cvbs"));
        if (product_line_cmd_switch_to_cvbs(index) >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], "cvbs", "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], "cvbs", "fail");
        }
    } else if (strcmp(argv[1], "led") == 0) {
        int rval = -1;
        if (strcmp(argv[3], "on") == 0) {
            rval = product_line_cmd_switch_led_on_off(1);
        } else if (strcmp(argv[3], "off") == 0) {
            rval = product_line_cmd_switch_led_on_off(0);
        }
        if (rval >= 0) {
            snprintf(response, size, "%s %s %s %s %s", argv[0], argv[1], argv[2], argv[3], "ok");
        } else {
            snprintf(response, size, "%s %s %s %s %s", argv[0], argv[1], argv[2], argv[3], "fail");
        }
    } else if (strcmp(argv[1], "record_playback") == 0) {
        record_test_flag = 1;
        product_line_cmd_record_playback(atoi(argv[2]));
    } else if (strcmp(argv[1], "record_playback_auto") == 0) {
        product_line_cmd_record_playback_auto(product_line_cmd_print, 2);
    } else if (strcmp(argv[1], "gsensor") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            int x = 0, y = 0, z = 0;
            product_line_cmd_get_gsensor_data(&x, &y, &z);
            snprintf(response, size, "%s %s %s x:%d y:%d z:%d", argv[0], argv[1], argv[2], x, y, z);
        }
    } else if (strcmp(argv[1], "imu") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            IMU_DATA_s imu_data;
            product_line_cmd_get_imu_data(&imu_data);
            snprintf(response, size, "%s %s %s accel_x:%.2f accel_y:%.2f accel_z:%.2f gyro_x:%.2f gyro_y:%.2f gyro_z:%.2f",
                                            argv[0], argv[1], argv[2],
                                            imu_data.accel.x_float, imu_data.accel.y_float, imu_data.accel.z_float,
                                            imu_data.gyro.x_float, imu_data.gyro.y_float, imu_data.gyro.z_float
                );
        }
    } else if (strcmp(argv[1], "bt") == 0) {
        int ret = product_line_cmd_bt_check();
        if (ret == 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else if (ret == -1) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "thermal") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            int value = 0;
            product_line_cmd_get_thermal_data(&value);
            snprintf(response, size, "%s %s %s %d", argv[0], argv[1], argv[2], value);
        }
    } else if (strcmp(argv[1], "brightness") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            int value = 0;
            product_line_cmd_get_brightness_data(&value);
            snprintf(response, size, "%s %s %s %d", argv[0], argv[1], argv[2], value);
        }
    } else if (strcmp(argv[1], "gps") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            char gps_info_str[256] = {0};
            memset(gps_info_str, 0, sizeof(gps_info_str));
            if (product_line_cmd_get_gps_str(gps_info_str) < 0) {
                int gps_num = 0;
                product_line_gps_info_s gps_info[12] = {0};
                product_line_cmd_get_gps_data(&gps_num, gps_info, 12);
                snprintf(response, size, "%s %s %s %d", argv[0], argv[1], argv[2], gps_num);
                if (gps_num > 0) {
                    int j = 0;
                    char tmp[32] = {0};
                    for (j = 0; j < gps_num; j++) {
                        memset(tmp, 0, sizeof(tmp));
                        snprintf(tmp, sizeof(tmp) - 1, " %d:%d", gps_info[j].sat_index, gps_info[j].sat_db);
                        strcat(response, tmp);
                    }
                }
            } else {
                if (strlen(gps_info_str) <= 0) {
                    snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "0|0");
                } else {
                    snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], gps_info_str);
                }
            }
        }
    } else if (strcmp(argv[1], "time") == 0) {
        char time[128] = {0};

        memset(time, 0, sizeof(time));
        if (strcmp(argv[2], "get") == 0) {
            product_line_cmd_get_time(time, sizeof(time));
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], time);
        } else if (strcmp(argv[2], "set") == 0) {
            snprintf(time, sizeof(time) - 1, "%s %s", argv[3], argv[4]);
            if (product_line_cmd_set_time(time) >= 0) {
                memset(time, 0, sizeof(time));
                product_line_cmd_get_time(time, sizeof(time));
                snprintf(response, size, "%s %s %s %s \"%s\"", argv[0], argv[1], argv[2], "ok", time);
            } else {
                snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
            }
        }
    } else if (strcmp(argv[1], "burn") == 0) {
        int i = 0;
        char sn[32] = {0};
        char mac[32] = {0};
        char bt_mac[32] = {0};
        char confirm_sn[64] = {0};
        char confirm_mac[32] = {0};
        char confirm_bt_mac[32] = {0};

        memset(sn, 0, sizeof(sn));
        memset(mac, 0, sizeof(mac));
        memset(bt_mac, 0, sizeof(bt_mac));
        for (i = 2; i < argc; i++) {
            if (strncmp(argv[i], "sn", 2) == 0) {
                snprintf(sn, sizeof(sn) - 1, "%s", argv[i] + 3);
            } else if (strncmp(argv[i], "mac", 3) == 0) {
                snprintf(mac, sizeof(mac) - 1, "%s", argv[i] + 4);
            } else if (strncmp(argv[i], "bt_mac", 6) == 0) {
                snprintf(bt_mac, sizeof(bt_mac) - 1, "%s", argv[i] + 7);
            }
        }
        memset(confirm_sn, 0, sizeof(confirm_sn));
        memset(confirm_mac, 0, sizeof(confirm_mac));
        memset(confirm_bt_mac, 0, sizeof(confirm_bt_mac));
        if (dqa_test_script.product_line_mode == 2) {
            if (product_line_cmd_burn_mac_sn2(mac, sn, confirm_mac, confirm_sn) >= 0) {
                snprintf(response, size, "%s %s %s mac:\"%s\" sn:\"%s\"", argv[0], argv[1], "ok", confirm_mac, confirm_sn);
            }
        } else {
            if (product_line_cmd_burn_mac_sn(mac, bt_mac, sn, confirm_mac, confirm_bt_mac, confirm_sn) >= 0) {
                snprintf(response, size, "%s %s %s mac:\"%s\" bt_mac:\"%s\" sn:\"%s\"", argv[0], argv[1], "ok", confirm_mac, confirm_bt_mac, confirm_sn);
            } else {
                snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
            }
        }
    } else if (strcmp(argv[1], "speech_auth") == 0) {
        int i = 0;
        char ssid[128] = {0};
        char password[128] = {0};

        memset(ssid, 0, sizeof(ssid));
        memset(password, 0, sizeof(password));
        for (i = 3; i < argc; i++) {
            if (strncmp(argv[i], "ssid", 4) == 0) {
                snprintf(ssid, sizeof(ssid) - 1, "%s", argv[i] + 5);
            } else if (strncmp(argv[i], "pwd", 3) == 0) {
                snprintf(password, sizeof(password) - 1, "%s", argv[i] + 4);
            }
        }
        if (product_line_cmd_speech_auth(ssid, password) >= 0) {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], "check", "ok");
        } else {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], "check", "fail");
        }
    } else if (strcmp(argv[1], "product_info") == 0) {
        if (strcmp(argv[2], "version") == 0) {
            char version[128] = {0};
            memset(version, 0, sizeof(version));
            product_line_cmd_get_version(version, sizeof(version));
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], version);
        } else if (strcmp(argv[2], "sn") == 0) {
            char sn[32] = {0};
            int rval = 0;
            memset(sn, 0, sizeof(sn));
            if (dqa_test_script.product_line_mode == 2) {
                rval = product_line_cmd_get_sn2(sn, sizeof(sn));
            } else {
                rval = product_line_cmd_get_sn(sn, sizeof(sn));
            }
            if (rval >= 0) {
                snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], sn);
            }
        } else if (strcmp(argv[2], "all") == 0) {
            product_line_cmd_get_all_product_info(response, sizeof(response) - 1);
        }
    } else if (strcmp(argv[1], "format_sd") == 0) {
        if (product_line_cmd_format_sd() >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
        product_line_cmd_print(response);
        product_line_cmd_power_off();
        return 0;
    } else if (strcmp(argv[1], "default_setting") == 0) {
        if (product_line_cmd_default_setting() >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "power_off") == 0) {
        if (product_line_cmd_power_off() >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "lcd") == 0) {
        int rval = 0;

        if (strcmp(argv[2], "done") == 0) {
            // lcd test finish
            product_line_cmd_lcd_test_done();
            rval = 0;
        } else if (strcmp(argv[2], "red") == 0) {
            rval = product_line_cmd_change_lcd_color(255, 0, 0);
        } else if (strcmp(argv[2], "green") == 0) {
            rval = product_line_cmd_change_lcd_color(0, 255, 0);
        } else if (strcmp(argv[2], "blue") == 0) {
            rval = product_line_cmd_change_lcd_color(0, 0, 255);
        } else {
            rval = -1;
        }
        if (rval >= 0) {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
        } else {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
        }
    } else if (strcmp(argv[1], "button_test") == 0) {
        button_test_flag = 1;
    } else if (strcmp(argv[1], "wifi_iperf") == 0) {
        if (dqa_test_script.product_line_mode == 2) {
            char sta_ip[32] = {0};
            if (product_line_cmd_get_wifi2_info(sta_ip) >= 0) {
                char wifi_mac[32] = {0};
                memset(wifi_mac, 0, sizeof(wifi_mac));
                if (product_line_cmd_get_usb_wifi_mac(wifi_mac, sizeof(wifi_mac)) >= 0) {
                    snprintf(response, sizeof(response) - 1, "%s %s ip:%s port:%d mac:%s", argv[0],
                                                                                    argv[1],
                                                                                    sta_ip,
                                                                                    product_line_cmd_get_iperf_port(),
                                                                                    wifi_mac);
                    product_line_cmd_iperf_run_impl();
                }
            }
        } else {
            char ap_ssid[32] = {0};
            char ap_password[32] = {0};
            char ap_ip[32] = {0};
            if (product_line_cmd_get_wifi_info(ap_ssid, ap_password, ap_ip) >= 0) {
                char wifi_mac[32] = {0};
                memset(wifi_mac, 0, sizeof(wifi_mac));
                if (product_line_cmd_get_wifi_mac(wifi_mac, sizeof(wifi_mac)) >= 0) {
                    snprintf(response, sizeof(response) - 1, "%s %s ssid:\"%s\" password:%s ip:%s port:%d mac:%s", argv[0],
                                                                                                            argv[1],
                                                                                                            ap_ssid,
                                                                                                            ap_password,
                                                                                                            ap_ip,
                                                                                                            product_line_cmd_get_iperf_port(),
                                                                                                            wifi_mac);
                    product_line_cmd_iperf_run_impl();
                }
            }
        }
    } else if (strcmp(argv[1], "4g") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            char sim[64] = {0};
            char imei[64] = {0};
            int csq = 0;
            int ping_ok = 0;
            memset(sim, 0, sizeof(sim));
            memset(imei, 0, sizeof(imei));
            if (product_line_cmd_get_4g_info(sim, imei, &csq, &ping_ok) >= 0) {
                snprintf(response, sizeof(response) - 1, "%s %s %s sim:\"%s\" imei:\"%s\" csq:%d ping:\"%s\"", argv[0],
                                                                                                                argv[1],
                                                                                                                argv[2],
                                                                                                                sim,
                                                                                                                imei,
                                                                                                                csq,
                                                                                                                ping_ok ? "ok" : "fail");
            }
        }
    } else if (strcmp(argv[1], "secure_boot_enable") == 0) {
        if (product_line_cmd_enable_secure_boot() >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "camera_check") == 0) {
        if (product_line_cmd_camera_check(atoi(argv[2])) >= 0) {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "fail");
        }
    } else if (strcmp(argv[1], "imu_calibration") == 0) {
        if (product_line_cmd_do_imu_calibration(argv[2]) >= 0) {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
        } else {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "fail");
        }
    } else if (strcmp(argv[1], "addon_port") == 0) {
        if (strcmp(argv[2], "switch") == 0) {
            char ap_ssid[32] = {0};
            char ap_password[32] = {0};
            char ap_ip[32] = {0};
            memset(ap_ssid, 0, sizeof(ap_ssid));
            memset(ap_password, 0, sizeof(ap_password));
            memset(ap_ip, 0, sizeof(ap_ip));
            if (product_line_cmd_get_wifi_info(ap_ssid, ap_password, ap_ip) < 0) {
                return 0;
            }
            snprintf(response, size, "%s %s %s %s ssid:\"%s\" password:%s left_url:\"http://%s:%d/%s\" right_url:\"http://%s:%d/%s\" power_off_url:\"http://%s:%d/%s\"",
                                        argv[0], argv[1], argv[2], "ok",
                                        ap_ssid, ap_password,
                                        ap_ip, 10000, "api/factory/addon_left_check",
                                        ap_ip, 10000, "api/factory/addon_right_check",
                                        ap_ip, 10000, "api/factory/power_off");
            product_line_cmd_print(response);
            sleep(3);
            app_helper.force_power_off = 1;
            app_helper.do_power_off();
        } else {
            if (product_line_cmd_addon_port_test(argv[2]) >= 0) {
                snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
            }
        }
    } else if (strcmp(argv[1], "usb_port") == 0) {
        if (product_line_cmd_usb_port_test(argv[2], argv[3]) >= 0) {
            snprintf(response, size, "%s %s %s %s %s", argv[0], argv[1], argv[2], argv[3], "ok");
        }
    } else if (strcmp(argv[1], "timer_wakeup") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], product_line_cmd_is_timer_wakeup() ? "ok" : "fail");
        } else {
            snprintf(response, size, "%s %s %s", argv[0], argv[1], "ok");
            product_line_cmd_print(response);sleep(2);
            product_line_cmd_timer_wakeup_test();
            return 0;
        }
    } else if (strcmp(argv[1], "lte_wakeup") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            char port[32] = {0};
            memset(port, 0, sizeof(port));
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            if (app_helper.check_lte_left_connected()) {
                snprintf(port, sizeof(port) - 1, "%s", "left");
            } else if (app_helper.check_lte_right_connected()) {
                snprintf(port, sizeof(port) - 1, "%s", "right");
            } else {
                snprintf(port, sizeof(port) - 1, "%s", "none");
            }
#endif
            snprintf(response, size, "%s %s %s %s %s", argv[0], argv[1], argv[2], product_line_cmd_is_lte_wakeup() ? "ok" : "fail", port);
        } else {
            if (app_helper.lte_booted == 0 || (tick() - app_helper.lte_booted_tick) < 10000) {
                return 0;
            }
            if (strcmp(argv[2], "left") == 0) {
                snprintf(response, size, "%s %s %s %s", argv[0], argv[1], argv[2], "ok");
            } else {
                char ap_ssid[32] = {0};
                char ap_password[32] = {0};
                char ap_ip[32] = {0};
                if (product_line_cmd_get_wifi_info(ap_ssid, ap_password, ap_ip) < 0) {
                    return 0;
                }
                snprintf(response, size, "%s %s %s %s ssid:\"%s\" password:%s left_enter_url:\"http://%s:%d/%s\" left_check_url:\"http://%s:%d/%s\"",
                                                    argv[0], argv[1], argv[2], "ok",
                                                    ap_ssid, ap_password,
                                                    ap_ip, 10000, "api/factory/lte_enter_wakeup",
                                                    ap_ip, 10000, "api/factory/lte_left_wakeup_check");
            }
            product_line_cmd_print(response);sleep(3);
            product_line_cmd_lte_wakeup_test();
            return 0;
        }
    } else {
        snprintf(response, sizeof(response) - 1, "%s not support", __func__);
    }
end:
    if (strlen(response) > 0) {
        product_line_cmd_print(response);
    }
    return 0;
}

