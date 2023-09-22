#include "dqa_test_script.h"
#ifdef CONFIG_ICAM_USB_USED
#include "AmbaSvcUsb.h"
#include "UVCDevice.h"
#endif
#include "cardv_modules/cardv_include.h"

int dqa_test_script_platform_init(void)
{
    return 0;
}

int dqa_test_script_do_on_linux(void)
{
    if (dqa_test_script.telnet_enable) {

    }

    if (dqa_test_script.usb_ether_mode) {
        app_helper.linux_system("/usr/local/share/script/usb_ether.sh&");
    }

    if (dqa_test_script.usb_uart_mode) {
        app_helper.linux_system("/usr/local/share/script/usb_console.sh&");
    }

    return 0;
}

int dqa_test_script_dqa_record_test_handler(const char *path)
{
    (void)path;
    dqa_test_script.dqa_record_test = 1;

    return 0;
}

int dqa_test_script_uart_test_handler(const char *path)
{
    (void)path;
    if (dqa_test_script.dqa_uart_test == 0) {
        dqa_test_script.dqa_uart_test = 1;
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
        dqa_test_uart_init();
#endif
    }

    return 0;
}

int dqa_test_script_disable_data_file_handler(const char *path)
{
    (void)path;
    dqa_test_script.disable_data_file = 1;

    return 0;
}

int dqa_test_script_ddr_shmoo_test_handler(const char *path)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    (void)path;
    dqa_test_script.ddr_shmoo_test = 1;
    SvcUserPref_Get(&pSvcUserPref);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (pSvcUserPref->InternalCamera.AutoRecord == 0
        || pSvcUserPref->ExternalCamera.AutoRecord == 0
        || pSvcUserPref->PivOnOff == OPTION_ON
        || pSvcUserPref->VideoEncryption.enable
        || pSvcUserPref->GnssRecord
        || pSvcUserPref->ImuRecord
        || pSvcUserPref->InternalCamera.MainStream.width != 3840
        || pSvcUserPref->InternalCamera.MainStream.height != 2160
        || pSvcUserPref->ExternalCamera.MainStream.width != 1280
        || pSvcUserPref->ExternalCamera.MainStream.height != 720
        ) {
        pSvcUserPref->InternalCamera.AutoRecord = 1;
        pSvcUserPref->ExternalCamera.AutoRecord = 1;
        pSvcUserPref->PivOnOff = OPTION_OFF;
        pSvcUserPref->VideoEncryption.enable = 0;
        pSvcUserPref->GnssRecord = 0;
        pSvcUserPref->ImuRecord = 0;
        pSvcUserPref->InternalCamera.MainStream.width = 3840;
        pSvcUserPref->InternalCamera.MainStream.height = 2160;
        pSvcUserPref->ExternalCamera.MainStream.width = 1280;
        pSvcUserPref->ExternalCamera.MainStream.height = 720;
        user_setting_save();
        Pmic_NormalSoftReset();
    }
#endif
    return 0;
}

int dqa_test_script_factory_handler(const char *path)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    //extern int SvcRecMain_SetDebug(int enable);

    (void)path;
    //SvcRecMain_SetDebug(1);
    SvcUserPref_Get(&pSvcUserPref);
    if (
        pSvcUserPref->InternalCamera.AutoRecord == 0 ||
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
        pSvcUserPref->ExternalCamera.AutoRecord == 0 ||
#endif
        //pSvcUserPref->PivOnOff == OPTION_ON ||
        pSvcUserPref->VideoEncryption.enable
        //pSvcUserPref->GnssRecord ||
        //pSvcUserPref->ImuRecord ||
        ) {
        pSvcUserPref->InternalCamera.AutoRecord = 1;
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
        pSvcUserPref->ExternalCamera.AutoRecord = 1;
#endif
        //pSvcUserPref->PivOnOff = OPTION_OFF;
        pSvcUserPref->VideoEncryption.enable = 0;
        //pSvcUserPref->GnssRecord = 0;
        //pSvcUserPref->ImuRecord = 0;
        user_setting_save();
        Pmic_NormalSoftReset();
    }
    if (dqa_test_script.product_line_mode == 0) {
        dqa_test_script.product_line_mode = 1;
        AmbaShell_SetPasswordEnable(0);
#if defined(CONFIG_APP_FLOW_AONI_PRODUCT_LINE_MODULE)
        product_line_cmd_init();
#endif
    }

    return 0;
}

int dqa_test_script_factory2_handler(const char *path)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    (void)path;
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->InternalCamera.AutoRecord == 0
        || pSvcUserPref->ExternalCamera.AutoRecord == 0
        || pSvcUserPref->PivOnOff == OPTION_ON
        || pSvcUserPref->VideoEncryption.enable
        || pSvcUserPref->GnssRecord
        || pSvcUserPref->ImuRecord) {
        pSvcUserPref->InternalCamera.AutoRecord = 1;
        pSvcUserPref->ExternalCamera.AutoRecord = 1;
        pSvcUserPref->PivOnOff = OPTION_OFF;
        pSvcUserPref->VideoEncryption.enable = 0;
        pSvcUserPref->GnssRecord = 0;
        pSvcUserPref->ImuRecord = 0;
        user_setting_save();
        Pmic_NormalSoftReset();
    }
    if (dqa_test_script.product_line_mode == 0) {
        dqa_test_script.product_line_mode = 2;
        AmbaShell_SetPasswordEnable(0);
#if defined(CONFIG_APP_FLOW_AONI_PRODUCT_LINE_MODULE)
        product_line_cmd_init();
#endif
    }

    return 0;
}

int dqa_test_script_factory3_handler(const char *path)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    (void)path;
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->InternalCamera.AutoRecord == 0
        || pSvcUserPref->ExternalCamera.AutoRecord
        || pSvcUserPref->PivOnOff == OPTION_ON
        || pSvcUserPref->VideoEncryption.enable) {//just internal camerea record
        pSvcUserPref->InternalCamera.AutoRecord = 1;
        pSvcUserPref->ExternalCamera.AutoRecord = 0;
        pSvcUserPref->PivOnOff = OPTION_OFF;
        pSvcUserPref->VideoEncryption.enable = 0;
        user_setting_save();
        Pmic_NormalSoftReset();
    }
    if (dqa_test_script.product_line_mode == 0) {
        dqa_test_script.product_line_mode = 1;
#if defined(CONFIG_APP_FLOW_AONI_PRODUCT_LINE_MODULE)
        product_line_cmd_init();
#endif
    }

    return 0;
}

int dqa_test_script_factory4_handler(const char *path)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    (void)path;
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->InternalCamera.AutoRecord
        || pSvcUserPref->ExternalCamera.AutoRecord == 0
        || pSvcUserPref->PivOnOff == OPTION_ON
        || pSvcUserPref->VideoEncryption.enable) {//just external camerea record
        pSvcUserPref->InternalCamera.AutoRecord = 0;
        pSvcUserPref->ExternalCamera.AutoRecord = 1;
        pSvcUserPref->PivOnOff = OPTION_OFF;
        pSvcUserPref->VideoEncryption.enable = 0;
        user_setting_save();
        Pmic_NormalSoftReset();
    }
    if (dqa_test_script.product_line_mode == 0) {
        dqa_test_script.product_line_mode = 1;
#if defined(CONFIG_APP_FLOW_AONI_PRODUCT_LINE_MODULE)
        product_line_cmd_init();
#endif
    }

    return 0;
}

int dqa_test_script_wifi_mfg_test_handler(const char *path)
{
    (void)path;
    dqa_test_script.wifi_mfg_test = 1;

    return 0;
}

int dqa_test_script_bt_mfg_test_handler(const char *path)
{
    (void)path;
    dqa_test_script.bt_mfg_test = 1;

    return 0;
}

int dqa_test_script_telnet_handler(const char *path)
{
    (void)path;
    dqa_test_script.telnet_enable = 1;

    return 0;
}

int dqa_test_script_no_power_off_handler(const char *path)
{
    (void)path;
    return 0;
}

static void modify_uvc_desc_frame_size_impl(unsigned char *p, int len, int w, int h)
{
    int i = 0, fps = 30;

    for (i = 0 ; i < (len - 4); i++) {
        if (*(p + i) == 0xAA && *(p + i + 1) == 0xBB
            && *(p + i + 2) == 0xCC && *(p + i + 3) == 0xDD) {
            //width
            *(p + i) = w & 0xFF;
            *(p + i + 1) = (w >> 8) & 0xFF;
            //height
            *(p + i + 2) = h & 0xFF;
            *(p + i + 3) = (h >> 8) & 0xFF;
            //min bitrate
            *(p + i + 4) = (w * h * 2 * fps) & 0xFF;
            *(p + i + 5) = (w * h * 2 * fps * 8 >> 8) & 0xFF;
            *(p + i + 6) = (w * h * 2 * fps * 8 >> 16) & 0xFF;
            *(p + i + 7) = (w * h * 2 * fps * 8 >> 24) & 0xFF;
            //max bitrate
            *(p + i + 8) = (w * h * 2 * fps) & 0xFF;
            *(p + i + 9) = (w * h * 2 * fps * 8 >> 8) & 0xFF;
            *(p + i + 10) = (w * h * 2 * fps * 8 >> 16) & 0xFF;
            *(p + i + 11) = (w * h * 2 * fps * 8 >> 24) & 0xFF;
            //frame size
            *(p + i + 12) = (w * h * 2) & 0xFF;
            *(p + i + 13) = (w * h * 2 >> 8) & 0xFF;
            *(p + i + 14) = (w * h * 2 >> 16) & 0xFF;
            *(p + i + 15) = (w * h * 2 >> 24) & 0xFF;
            break;
        }
    }
}

static void modify_uvc_desc_frame_size(int vin)
{
    extern USBD_DESC_CUSTOM_INFO_s UvcDescFramework;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    int w = 0, h = 0;

    SvcUserPref_Get(&pSvcUserPref);
    if (vin == 0) {
        w = pSvcUserPref->InternalCamera.UvcStream.width;
        h = pSvcUserPref->InternalCamera.UvcStream.height;
    } else {
        w = pSvcUserPref->ExternalCamera.UvcStream.width;
        h = pSvcUserPref->ExternalCamera.UvcStream.height;
    }
    modify_uvc_desc_frame_size_impl(UvcDescFramework.DescFrameworkFs, UvcDescFramework.DescSizeFs, w, h);
    modify_uvc_desc_frame_size_impl(UvcDescFramework.DescFrameworkHs, UvcDescFramework.DescSizeHs, w, h);
}

int dqa_test_script_run_uvc(int vin)
{
#ifdef CONFIG_ICAM_USB_USED
    (VOID)AmbaSvcUsb_DeviceSysInit();
    AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_UVC);
    AppUvcd_Init(vin);
    modify_uvc_desc_frame_size(vin);
    AmbaSvcUsb_DeviceClassStart();
#endif

    return 0;
}

int dqa_test_script_tiaojiao_handler(const char *path)
{
    (void)path;
    if (dqa_test_script.usb_uvc_mode == 0) {
        app_helper.touch(USER_SETTING_UVC_MODE);
        Pmic_NormalSoftReset();
        return 0;
    }

    return 0;
}

int dqa_test_script_tiaojiao2_handler(const char *path)
{
    (void)path;
    if (dqa_test_script.usb_uvc_mode == 0) {
        app_helper.touch(USER_SETTING_UVC_MODE2);
        Pmic_NormalSoftReset();
        return 0;
    }

    return 0;
}

int dqa_test_script_linux_usb2uart_handler(const char *path)
{
    (void)path;
    return 0;
}

int dqa_test_script_linux_usb2eth_handler(const char *path)
{
    (void)path;
    dqa_test_script.usb_ether_mode = 1;
    return 0;
}

int dqa_test_script_gps_debug_handler(const char *path)
{
    (void)path;
    if (dqa_test_script.gps_debug == 0) {
        dqa_test_script.gps_debug = 1;
    }

    return 0;
}

int dqa_test_script_http_control_server_handler(const char *path)
{
    (void)path;
    dqa_test_script.http_control_server_debug = 1;

    return 0;
}

int dqa_test_script_audio_test_handler(const char *path)
{
    (void)path;
    dqa_test_script.audio_test = 1;

    return 0;
}

int dqa_test_script_enable_uart_handler(const char *path)
{
    (void)path;
    return 0;
}

int dqa_test_script_burn_debug_handler(const char *path)
{
    (void)path;
    dqa_test_script.burn_debug = 1;

    return 0;
}

int dqa_test_script_sn_fake_handler(const char *path)
{
    (void)path;
    dqa_test_script.sn_fake = 1;

    return 0;
}

int dqa_test_script_uart_onoff_handler(const char *path)
{
    extern int AmbaLink_GetLinuxConsoleEnable(void);

    (void)path;
    if (AmbaUser_GetRtosUartEnable()) {
        AmbaUser_SetRtosUartEnable(0);
    } else {
        AmbaUser_SetRtosUartEnable(1);
    }
    if (AmbaLink_GetLinuxConsoleEnable()) {
        app_helper.linux_console_enable = 0;
    } else {
        app_helper.linux_console_enable = 1;
    }

    return 0;
}

int dqa_test_script_ir_debug_handler(const char *path)
{
    (void)path;
    dqa_test_script.ir_debug = 1;

    return 0;
}

int dqa_test_script_amic_handler(const char *path)
{
    (void)path;
    (void)AmbaAudio_CodecSetInput(0, AUCODEC_AMIC_IN);

    return 0;
}

int dqa_test_script_dmic_handler(const char *path)
{
    (void)path;
    (void)AmbaAudio_CodecSetInput(0, AUCODEC_DMIC_IN);

    return 0;
}

int dqa_test_script_dmic2_handler(const char *path)
{
    (void)path;
    (void)AmbaAudio_CodecSetInput(0, AUCODEC_DMIC2_IN);

    return 0;
}

int dqa_test_script_custom_wifi_ssid_handler(const char *path)
{
    char ssid[32] = {0};

    memset(ssid, 0, sizeof(ssid));
    app_helper.read_all_text(path, (unsigned char *)ssid, sizeof(ssid) - 1);
    wifi_set_custom_ssid(ssid);

    return 0;
}

#if defined(CONFIG_ICAM_USE_DIAG_COMMANDS)
#undef SEEK_SET
#define SEEK_SET PF_SEEK_SET

static int get_n_line(FILE *pFile, const unsigned int line_num, char *line_buf)
{
    unsigned int i = 0, num = 0;
    char ch = 0;

    if (pFile == NULL) {
        return -1;
    }
    fseek(pFile, 0, SEEK_SET);
    do
    {   
        if (fread(&ch, 1, 1, pFile) != 0) {
            if (line_num == num + 1) {
                if (ch == '\r' || ch == '\n') {
                    return 0;
                }
                line_buf[i++] = ch;
            }
            if (ch == '\n') {
                num++;
            }
        } else {
            return -1;
        }

    } while(1);
	
}

static int get_first_line_num(FILE *pFile)
{
    unsigned char line_num = 0;
    char ch_buf[256] = {0};
    char write_buf[8] = {0};

    if (pFile == NULL) {
        return -1;
    }
    get_n_line(pFile, 1, ch_buf);
    if (ch_buf[0] == '#') {
        if (ch_buf[1] >= '0' && ch_buf[1] <= '9') {
            if (ch_buf[2] >= '0' && ch_buf[2] <= '9') {
                write_buf[0] = ch_buf[1];
                write_buf[1] = ch_buf[2];
                line_num = atoi(write_buf);
                memset(write_buf, 0, sizeof(write_buf));
                snprintf(write_buf, strlen(write_buf), "%02d", line_num + 1);
                fseek(pFile, 1, SEEK_SET);
                fwrite(write_buf, 1, strlen(write_buf), pFile);
                return line_num;
            } else {
                debug_line("ddr_shmoo scrit format error, first line must be #00");
                return -1;
            }
        } else {
            debug_line("ddr_shmoo scrit format error, first line must be #00");
            return -1;
        }
    }
    debug_line("ddr_shmoo scrit format error, first line must be #00");
    
    return -1;
}

static AMBA_KAL_TASK_t ddr_shmoo_task;
#define DDR_SHMOO_TASK_STACK_SIZE (64 * 1024)
static unsigned char ddr_shmoo_task_stack[DDR_SHMOO_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int running = 1;

static void *ddr_shmoo_task_entry(void *arg)
{
    static int cnt = 0;
    UINT32 ret = 0;

    while (running) {
        cnt++;
        if (cnt == 15) {
            FILE *pFile = NULL;
            char line_buffer[128] = {0};
            int num = 0;
            pFile = fopen("c:\\ddr_shmoo.txt", "r+");
            if (pFile == NULL) {
                debug_line("ddr_shmoo.txt open fail");
                return NULL;
            }

            num = get_first_line_num(pFile);
            if (num != -1) {
                if (get_n_line(pFile, num + 2, line_buffer) != -1) {
                    debug_line("%s", line_buffer);
                    fclose(pFile);
                    ret = AmbaShell_ExecCommand(line_buffer);  
                } else {
                    fseek(pFile, 1, SEEK_SET);
                    fwrite("00", 1, 2, pFile);
                    fclose(pFile);
                }
            } else {
                fclose(pFile);
            }
        }

        if (ret != 0) {
            ret = 0;
            //rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, NULL);
            Pmic_SoftReset();
        }
        sleep(1);
    }

    return NULL;
}

int dqa_test_script_ddr_shmoo_task_start(void)
{
    unsigned int ret = 0;

    ret = AmbaKAL_TaskCreate(&ddr_shmoo_task,
                            "ddr_shmoo_task",
                            180,
                            (AMBA_KAL_TASK_ENTRY_f)ddr_shmoo_task_entry,
                            0,
                            ddr_shmoo_task_stack,
                            DDR_SHMOO_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("ddr_shmoo_task create fail");
        return -1;
    }

    return 0;
}
#endif

