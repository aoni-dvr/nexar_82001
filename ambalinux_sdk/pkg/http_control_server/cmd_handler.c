#include "cmd_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include <unistd.h>
#include "cJSON/cJSON.h"
#include "cmd_response.h"
#include "cmd_ctrl.c"
#include "cmd_get.c"
#include <sys/socket.h>

int parse_key(const char *data, const char *key, char *value, unsigned int value_size)
{
    char buf[128] = {0};
    int index = 0, end = 0, rval = -1;

    while (1) {
        if (*data == '\0') {
            *(buf + index) = '\0';
            end = 2;
        } else if (*data == '&') {
            *(buf + index) = '\0';
            index = 0;
            end = 1;
        } else {
            *(buf + index) = *data;
            index += 1;
        }

        if (end) {
            if (strncmp(buf, key, strlen(key)) == 0) {
                snprintf(value, value_size, "%.*s", (int)(strlen(buf) - strlen(key)), buf + strlen(key));
                rval = 0;
                break;
            }
            if (end == 2) {
                break;
            }
        }
        end = 0;
        data += 1;
    }

    return rval;
}

int cmd_do_ctrl_handler(int client, const char *data)
{
    ctrl_item_s item;
    int i = 0, rval = -1;
    char value[32] = {0};

    memset(&item, 0, sizeof(item));
    memset(value, 0, sizeof(value));
    if (parse_key(data, "cmd=", value, sizeof(value) - 1) >= 0) {
        item.cmd = strtoul(value, NULL, 16) & 0xFFFF;
    }
    memset(value, 0, sizeof(value));
    if (parse_key(data, "arg=", value, sizeof(value) - 1) >= 0) {
        item.arg = atoi(value);
    }
    parse_key(data, "data=", item.data, sizeof(item.data) - 1);
    parse_key(data, "password=", item.password, sizeof(item.password) - 1);
    show_ctrl_item(item);
    //check auth
    if (global_data.authed == 0
        && item.cmd != NET_CMD_AUTH_TOKEN) {
        return RESPONSE_CODE_UNAUTHORIZED;
    }
    // do handler
    rval = cmd_ctrl_impl(item);
    // send response
    if (rval == 0) {
        send_code(client, 0);
        if (item.cmd == NET_CMD_REBOOT_DEVICE) {
            debug_line("reboot");
            sleep(2);
            rtos_api_lib_reboot();
        }
    }

    return rval;
}

int cmd_do_get_handler(int client, const char *path, const char *query_str)
{
    int i = 0, rval = -1;

    if (path == NULL) {
        return RESPONSE_CODE_INVALID_PARAM;
    }

    if (global_data.authed == 0
        && strncmp(path, "/api/info", strlen("/api/info")) != 0
        && strncmp(path, "/api/config", strlen("/api/config")) != 0) {
        return RESPONSE_CODE_UNAUTHORIZED;
    }

    for (i = 0; i < sizeof(get_handler_items) / sizeof(get_handler_item_s); i++) {
        if (strcmp(path, get_handler_items[i].path) == 0) {
            if (get_handler_items[i].handler) {
                rval = get_handler_items[i].handler(client, path, query_str);
            }
            break;
        }
    }

    return rval;
}

