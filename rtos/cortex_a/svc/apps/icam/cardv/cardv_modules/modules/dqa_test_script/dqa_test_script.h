#ifndef __DQA_TEST_SCRIPT_H__
#define __DQA_TEST_SCRIPT_H__

#define MAX_SCRIPT_NAME_LEN 32

typedef int (*dqa_test_script_handler_func_s)(const char *path);
typedef struct _dqa_test_script_handler_s_ {
    char script_name[MAX_SCRIPT_NAME_LEN];
    dqa_test_script_handler_func_s handler;
} dqa_test_script_handler_s;

typedef struct _dqa_test_script_s_ {
    unsigned char check_done:1;
    unsigned char product_line_mode;
    unsigned char dqa_record_test:1;
    unsigned char dqa_uart_test:1;
    unsigned char telnet_enable:1;
    unsigned char wifi_mfg_test:1;
    unsigned char bt_mfg_test:1;
    unsigned char gps_debug:1;
    unsigned char usb_uvc_mode;
    unsigned char usb_uart_mode:1;
    unsigned char usb_ether_mode:1;
    unsigned char http_control_server_debug:1;
    unsigned char audio_test:1;
    unsigned char burn_debug:1;
    unsigned char sn_fake:1;
    unsigned char ir_debug:1;
    unsigned char disable_data_file:1;
    unsigned char ddr_shmoo_test:1;
    int audio_test_seconds;
} dqa_test_script_s;

extern dqa_test_script_s dqa_test_script;

int dqa_test_script_init(void);
int dqa_test_script_check(char disk);
int dqa_test_script_is_block_recording(void);
int dqa_test_script_run_uvc(int vin);
int dqa_test_script_is_uvc_mode(void);

#endif//__DQA_TEST_SCRIPT_H__

