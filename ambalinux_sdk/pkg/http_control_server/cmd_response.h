#ifndef __CMD_RESPONSE_H__
#define __CMD_RESPONSE_H__

#define SERVER_STRING "Server: AiHttpd/1.1.0\r\n"

enum _response_code_e_ {
    RESPONSE_CODE_OK = 0,
    RESPONSE_CODE_SYSTEM_ERROR = -101,
    RESPONSE_CODE_INVALID_PARAM = -502,
    RESPONSE_CODE_INVALID_DATA_PARAM = -503,
    RESPONSE_CODE_AUTH_FALSE = 101,
    RESPONSE_CODE_DATABASE_NOT_READY = 102,
    RESPONSE_CODE_UNAUTHORIZED = 401,
    RESPONSE_CODE_NOT_FOUND = 404,
};

int send_header(int client, const char *type, int content_length);
int send_json_response(int client, const char *content, int content_length);
int send_code(int client, int code);
const char *get_code_message(int code);

#endif//__CMD_RESPONSE_H__

