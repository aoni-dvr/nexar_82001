#include "dqa_test_script.h"
#include "cardv_modules/cardv_include.h"

dqa_test_script_s dqa_test_script = {0};

extern int dqa_test_script_platform_init(void);
extern int dqa_test_script_factory_handler(const char *path);
extern int dqa_test_script_factory2_handler(const char *path);
extern int dqa_test_script_wifi_mfg_test_handler(const char *path);
extern int dqa_test_script_bt_mfg_test_handler(const char *path);
extern int dqa_test_script_telnet_handler(const char *path);
extern int dqa_test_script_no_power_off_handler(const char *path);
extern int dqa_test_script_tiaojiao_handler(const char *path);
extern int dqa_test_script_tiaojiao2_handler(const char *path);
extern int dqa_test_script_linux_usb2uart_handler(const char *path);
extern int dqa_test_script_linux_usb2eth_handler(const char *path);
extern int dqa_test_script_gps_debug_handler(const char *path);
extern int dqa_test_script_http_control_server_handler(const char *path);
extern int dqa_test_script_audio_test_handler(const char *path);
extern int dqa_test_script_enable_uart_handler(const char *path);
extern int dqa_test_script_burn_debug_handler(const char *path);
extern int dqa_test_script_sn_fake_handler(const char *path);
extern int dqa_test_script_dqa_record_test_handler(const char *path);
extern int dqa_test_script_uart_test_handler(const char *path);
extern int dqa_test_script_uart_onoff_handler(const char *path);
extern int dqa_test_script_factory3_handler(const char *path);
extern int dqa_test_script_factory4_handler(const char *path);
extern int dqa_test_script_ir_debug_handler(const char *path);
extern int dqa_test_script_amic_handler(const char *path);
extern int dqa_test_script_dmic_handler(const char *path);
extern int dqa_test_script_dmic2_handler(const char *path);
extern int dqa_test_script_disable_data_file_handler(const char *path);
extern int dqa_test_script_ddr_shmoo_test_handler(const char *path);
extern int dqa_test_script_custom_wifi_ssid_handler(const char *path);

static dqa_test_script_handler_s script_handler[] = {
    //{"tiaojiao_auto_delete.txt",    dqa_test_script_tiaojiao_handler},
    //{"tiaojiao2_auto_delete.txt",   dqa_test_script_tiaojiao2_handler},
    {"WifiTestFile.txt",            dqa_test_script_wifi_mfg_test_handler},
    //{"WifiTestFile_auto_delete.txt",dqa_test_script_wifi_mfg_test_handler},
    //{"BtTestFile.txt",              dqa_test_script_bt_mfg_test_handler},
    //{"Telnetd.txt",                 dqa_test_script_telnet_handler},
    //{"no_power_off.txt",            dqa_test_script_no_power_off_handler},
    {"tiaojiao.txt",                dqa_test_script_tiaojiao_handler},
    {"tiaojiao2.txt",               dqa_test_script_tiaojiao2_handler},
    //{"linux_usb2uart.txt",          dqa_test_script_linux_usb2uart_handler},
    //{"linux_usb2eth.txt",           dqa_test_script_linux_usb2eth_handler},
    {"gps_debug.txt",               dqa_test_script_gps_debug_handler},
    //{"http_control_server",         dqa_test_script_http_control_server_handler},
    {"audio_test.txt",              dqa_test_script_audio_test_handler},
    {"factory.txt",                 dqa_test_script_factory_handler},
    {"factory2.txt",                dqa_test_script_factory2_handler},
    //{"factory3.txt",                dqa_test_script_factory3_handler},
    //{"factory4.txt",                dqa_test_script_factory4_handler},
    //{"enable_uart.txt",             dqa_test_script_enable_uart_handler},
    //{"burn_debug.txt",              dqa_test_script_burn_debug_handler},
    {"sn_fake.txt",                 dqa_test_script_sn_fake_handler},
    //{"dqa_record_test.txt",         dqa_test_script_dqa_record_test_handler},
    //{"dqa_uart_test.txt",           dqa_test_script_uart_test_handler},
    //{"uart.txt",                    dqa_test_script_uart_onoff_handler},
    //{"ir_debug.txt",                dqa_test_script_ir_debug_handler},
    //{"amic.txt",                    dqa_test_script_amic_handler},
    //{"dmic.txt",                    dqa_test_script_dmic_handler},
    //{"dmic2.txt",                   dqa_test_script_dmic2_handler},
    //{"disable_data_file.txt",       dqa_test_script_disable_data_file_handler},
    {"ddr_shmoo.txt",               dqa_test_script_ddr_shmoo_test_handler},
    {"wifi_ssid.txt",                 dqa_test_script_custom_wifi_ssid_handler},
};

int dqa_test_script_init(void)
{
    memset(&dqa_test_script, 0, sizeof(dqa_test_script));
    dqa_test_script.audio_test_seconds = 180;
    dqa_test_script_platform_init();

    return 0;
}

int dqa_test_script_check(char disk)
{
    int i = 0;
    char path[128] = {0};
    struct stat file_stat;
    int script_num = (int)(sizeof(script_handler) / sizeof(dqa_test_script_handler_s));
    int script_exist = 0;

    if (dqa_test_script.check_done) {
        debug_line("[dqa_test_script] checked already");
        return 0;
    }
    disk = tolower((int)disk);
    for (i = 0; i < script_num; i++) {
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path) - 1, "%c:\\%s", disk, script_handler[i].script_name);
        if (stat(path, &file_stat) == 0) {
            debug_line("[dqa_test_script]%s found", path);
            if (strstr(path, "auto_delete") != NULL) {
                remove(path);
            }
            script_exist = 1;
            (*script_handler[i].handler)(path);
        }
    }
    if (script_exist == 0) {
        debug_line("[dqa_test_script] no script found");
    }
    dqa_test_script.check_done = 1;

    return 0;
}

int dqa_test_script_is_block_recording(void)
{
    return 0;
}

int dqa_test_script_is_uvc_mode(void)
{
    return dqa_test_script.usb_uvc_mode;
}

