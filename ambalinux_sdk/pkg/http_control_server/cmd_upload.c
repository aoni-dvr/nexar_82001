#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "cJSON/cJSON.h"
#include "md5.h"
#include "global.h"

static int tcp_read(int client, unsigned char *buf, unsigned buf_size, unsigned int timeout)
{
    fd_set pending_data;
    struct timeval block_time;
    int rval = 0;

    FD_ZERO(&pending_data);
    FD_SET(client, &pending_data);
    block_time.tv_sec = timeout / 1000;
    block_time.tv_usec = 0;
    switch (select(client + 1, &pending_data, NULL, NULL, &block_time)) {
    case 0:
        rval = 0;
        break;
    case -1:
        rval = -1;
        break;
    default:
        rval = read(client, buf, buf_size);
        if (rval <= 0) {
            debug_line("%s, socket closed", __func__);
            rval = -1;
        }
        break;
    }

    return rval;
}

static int send_upload_response(int client, int code, const char *filepath)
{
    cJSON *obj = cJSON_CreateObject();
    char *out = NULL;

    if (obj == NULL) {
        return -1;
    }
    cJSON_AddNumberToObject(obj, "code", code);
    cJSON_AddStringToObject(obj, "msg", get_code_message(code));
    if (code == 0) {
        unsigned char digest[16] = {0};
        char calc_md5[33] = {0};
        struct stat fstat;
        int i = 0;
        cJSON *data = cJSON_CreateObject();
        // get filesize
        stat(filepath, &fstat);
        // cal md5
        MD5File(filepath, digest);
        memset(calc_md5, 0, sizeof(calc_md5));
        for (i = 0; i < 16; i++) {
            sprintf(calc_md5 + i * 2, "%02x", digest[i]);
        }
        // add data
        cJSON_AddStringToObject(data, "file", filepath);
        cJSON_AddNumberToObject(data, "size", fstat.st_size);
        cJSON_AddStringToObject(data, "md5", calc_md5);
        cJSON_AddItemToObject(obj, "data", data);
    }
    out = cJSON_PrintUnformatted(obj);
    if (out) {
        debug_line(out);
        send_json_response(client, out, strlen(out));
        free(out);
        out = NULL;
    }
    cJSON_Delete(obj);

}

int cmd_do_upload(int client, const char *filepath, unsigned int filesize)
{
    unsigned char buf[8 * 1024] = {0};
    int rval = 0, timeout_times = 0;
    FILE *pFile = NULL;
    unsigned int read_size = 0;

    if (global_data.authed == 0) {
        return RESPONSE_CODE_UNAUTHORIZED;
    }
    if (filepath == NULL || strncmp(filepath, "/mnt/extsd/", strlen("/mnt/extsd/")) != 0) {
        return RESPONSE_CODE_INVALID_PARAM;
    }
    debug_line("%s start", __func__);
    // open file
    pFile = fopen(filepath, "wb");
    if (pFile == NULL) {
        debug_line("%s open %s fail", __func__, filepath);
        return RESPONSE_CODE_NOT_FOUND;
    }
    // read data
    while (1) {
        rval = tcp_read(client, buf, sizeof(buf), 3000);
        if (rval < 0) {
            debug_line("read error");
            break;
        } else if (rval == 0) {
            timeout_times++;
        } else if (rval > 0) {
            timeout_times = 0;
            read_size += rval;
            fwrite(buf, rval, 1, pFile);
            if (read_size >= filesize) {
                rval = 0;
                break;
            }
        }
        if (timeout_times >= 3) {
            debug_line("read timeout reach max times");
            rval = -1;
            break;
        }
    }
    // close file
    fflush(pFile);
    fclose(pFile);
    debug_line("%s finish", __func__);
    if (rval == 0) {
        send_upload_response(client, 0, filepath);
    }

    return rval;
}

