#ifndef __DQA_TEST_UART_H__
#define __DQA_TEST_UART_H__

#include "cardv_modules/cardv_include.h"

typedef enum _dqa_test_uart_record_state_e_ {
    DQA_TEST_UART_RECORD_STATE_IDLE = 0,
    DQA_TEST_UART_RECORD_STATE_LOOP_RECORD,
    DQA_TEST_UART_RECORD_STATE_EVENT_RECORD,
    DQA_TEST_UART_RECORD_STATE_PARKING_RECORD,
    DQA_TEST_UART_RECORD_STATE_NUM
} dqa_test_uart_record_state_e;

typedef enum _dqa_test_uart_wifi_state_e_ {
    DQA_TEST_UART_WIFI_STATE_ON = 0,
    DQA_TEST_UART_WIFI_STATE_OFF,
    DQA_TEST_UART_WIFI_STATE_BUSY,
    DQA_TEST_UART_WIFI_STATE_NUM
} dqa_test_uart_wifi_state_e;

typedef void (*dqa_test_uart_print_func)(const char *);
typedef int (*dqa_test_uart_cmd_cb_func)(int argc, char **argv, dqa_test_uart_print_func func);

int dqa_test_uart_init(void);
int dqa_test_uart_get_take_photo_flag(void);
int dqa_test_uart_set_take_photo_flag(int flag);
int dqa_test_uart_take_photo_done(int success, int vin_index, const char *http_path);
int dqa_test_uart_default_setting_done(void);
int dqa_test_uart_reboot_done(void);
int dqa_test_uart_format_sd_done(int success);
int dqa_test_uart_event_record_done(void);
void show_build_info(void);

#endif//__DQA_TEST_UART_H__

