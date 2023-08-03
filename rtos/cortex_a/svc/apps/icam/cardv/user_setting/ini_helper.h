#ifndef __INI_SETTING_H__
#define __INI_SETTING_H__

#include <app_base.h>

typedef int (*ini_item_callback_func)(int, int, const char *);

typedef struct _value_pair_s_ {
    char value_str[32];
    int value_int;
} value_pair_s;

typedef struct _ini_section_item_s_ {
    char key[32];
    unsigned char use_value_str;
    value_pair_s pairs[64];
} ini_section_item_s;

typedef struct _ini_section_s_ {
    char name[32];
    ini_section_item_s *items;
    int items_num;
} ini_section_s;

int ini_register_setting(int section_num, ini_section_s *setting);
int ini_register_callback(ini_item_callback_func func);
int ini_load(const char *path);
int ini_add_line(void *pFile, const char *line);
int ini_save_section_name(void *pFile, int section_index);
int ini_save_section_item_int(void *pFile, int section_index, int item_index, int value);
int ini_save_section_item_str(void *pFile, int section_index, int item_index, const char *value);
int ini_save_section_item_double(void *pFile, int section_index, int item_index, double value);

#endif//__INI_SETTING_H__

