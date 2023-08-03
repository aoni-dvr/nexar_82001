#ifndef _PRODUCT_LINE_H__
#define _PRODUCT_LINE_H__

#include "cardv_modules/cardv_include.h"

#define CMD_LINE_MAX_PARAM 32
#define CMD_LINE_MAX_SIZE 128

typedef void (*product_line_cmd_print_func)(const char *);
typedef int (*product_line_cmd_cb_func)(int argc, char **argv, product_line_cmd_print_func func);

typedef struct _product_line_gps_info_s_ {
    int sat_index;
    int sat_db;
} product_line_gps_info_s;

int product_line_cmd_init(void);
int product_line_cmd_get_button_test_flag(void);
int product_line_cmd_button_test_key_pressed(int index);
int product_line_cmd_record_done(void);

#endif//_PRODUCT_LINE_H__

