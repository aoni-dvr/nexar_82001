#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"
#include "debug.h"
#include "cmd_response.h"
#include <sys/types.h>
#include <sys/socket.h>

#define HTTP_RESPONSE_HEADER "HTTP/1.0 200 OK\r\n" \
                            SERVER_STRING \
                            "Connection: close\r\n" \
                            "Content-type: %s\r\n" \
                            "Content-Length: %d\r\n\r\n"

typedef struct _response_code_msg_s_ {
    int code;
    char msg[64];
} response_code_msg_s;

static response_code_msg_s code_messages[] = {
    {RESPONSE_CODE_OK,                  "Success"},
    {RESPONSE_CODE_SYSTEM_ERROR,        "System Error"},
    {RESPONSE_CODE_INVALID_PARAM,       "Invalid request param"},
    {RESPONSE_CODE_INVALID_DATA_PARAM,  "Invalid data param"},
    {RESPONSE_CODE_AUTH_FALSE,          "Auth False"},
    {RESPONSE_CODE_DATABASE_NOT_READY,  "database not initialized"},
    {RESPONSE_CODE_UNAUTHORIZED,        "Unauthorized"},
    {RESPONSE_CODE_NOT_FOUND,           "Not Found"},
};

const char *get_code_message(int code)
{
    int i = 0;

    for (i = 0; i < sizeof(code_messages) / sizeof(response_code_msg_s); i++) {
        if (code == code_messages[i].code) {
            return code_messages[i].msg;
        }
    }

    return "Unknown Error";
}

int send_json_response(int client, const char *content, int content_length)
{
    char buf[1024 * 1024] = {0};

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, HTTP_RESPONSE_HEADER"%s", "application/json", content_length, content);
    debug_line("send len=%d", send(client, buf, strlen(buf), 0));

    return 0;
}

int send_header(int client, const char *type, int content_length)
{
    char buf[256] = {0};

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, HTTP_RESPONSE_HEADER, type, content_length);
    send(client, buf, strlen(buf), 0);

    return 0;
}

int send_code(int client, int code)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;

    if (obj == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", code);
    cJSON_AddStringToObject(obj, "msg", get_code_message(code));
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        debug_line(out);
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

    return 0;
}

