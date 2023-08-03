#include "rtos_api_lib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int (*get_item_handler_func_s)(int client, const char *path, const char *query_str);
typedef struct _cmd_handler_item_s_ {
    char path[64];
    get_item_handler_func_s handler;
} get_handler_item_s;

static int cmd_get_addon_left_check(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;
    basic_info_s basic_info;
    int sensor_active = 0;
    int lte_active = 0;

    if (obj == NULL) {
        return -1;
    }
    rtos_api_lib_get_basic_info(&basic_info);
    if (basic_info.addon_type.left_value == ADDON_TYPE_LTE_WIFI_BOARD
        && basic_info.addon_type.right_value == ADDON_TYPE_SENSOR_BOARD) {
        rtos_api_lib_check_sensor_working(1, &sensor_active);
        rtos_api_lib_check_lte_working(&lte_active);
    }
    cJSON_AddNumberToObject(obj, "code", (sensor_active && lte_active) ? 0 : -1);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_addon_right_check(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;
    basic_info_s basic_info;
    int sensor_active = 0;
    int lte_active = 0;

    if (obj == NULL) {
        return -1;
    }
    rtos_api_lib_get_basic_info(&basic_info);
    if (basic_info.addon_type.left_value == ADDON_TYPE_SENSOR_BOARD
        && basic_info.addon_type.right_value == ADDON_TYPE_LTE_WIFI_BOARD) {
        rtos_api_lib_check_sensor_working(1, &sensor_active);
        rtos_api_lib_check_lte_working(&lte_active);
    }
    cJSON_AddNumberToObject(obj, "code", (sensor_active && lte_active) ? 0 : -1);

    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_lte_enter_wakeup(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;

    if (obj == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", rtos_api_lib_lte_enter_wakeup_mode() == 0 ? 0 : -1);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_get_lte_left_wakeup_check(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;
    basic_info_s basic_info;

    if (obj == NULL) {
        return -1;
    }
    rtos_api_lib_get_basic_info(&basic_info);
    if (basic_info.addon_type.left_value == ADDON_TYPE_LTE_WIFI_BOARD && basic_info.wakeup_source.value == WAKEUP_SOURCE_LTE) {
        cJSON_AddNumberToObject(obj, "code", 0);
    } else {
        cJSON_AddNumberToObject(obj, "code", -1);
    }
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

static int cmd_power_off(int client, const char *url, const char *query_str)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;

    if (obj == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", 0);
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);
    sleep(3);
    rtos_api_lib_power_off();

    return 0;
}

static get_handler_item_s get_handler_items[] = {
    {"/api/factory/addon_left_check",                           cmd_get_addon_left_check},
    {"/api/factory/addon_right_check",                          cmd_get_addon_right_check}, 
    {"/api/factory/lte_enter_wakeup",                           cmd_get_lte_enter_wakeup},
    {"/api/factory/lte_left_wakeup_check",                      cmd_get_lte_left_wakeup_check},
    {"/api/factory/power_off",                                  cmd_power_off},
    //{"/api/factory/lte_right_wakeup_check",                  cmd_get_lte_right_wakeup_check},
};

