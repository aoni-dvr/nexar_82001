#include "cmd_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include <unistd.h>
#include "cJSON.h"
#include "cmd_response.h"
#include "cmd_get.c"
#include <sys/socket.h>

int cmd_do_get_handler(int client, const char *path, const char *query_str)
{
    int i = 0;

    if (path == NULL) {
        return RESPONSE_CODE_INVALID_PARAM;
    }

    for (i = 0; i < sizeof(get_handler_items) / sizeof(get_handler_item_s); i++) {
        if (strcmp(path, get_handler_items[i].path) == 0) {
            if (get_handler_items[i].handler) {
                return get_handler_items[i].handler(client, path, query_str);
            } else {
                return RESPONSE_CODE_SYSTEM_ERROR;
            }
        }
    }

    return RESPONSE_CODE_NOT_FOUND;
}

