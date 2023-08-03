#include "md5.h"
#include <rtos_api_lib.h>
#include <sys/socket.h>

static int cmd_do_ota_update(int client, const char *data)
{
    //manufacturer=HaloCam&version=NEXAR_W1_EN-v3.3.0&md5=33e12222da3af17f4c5756a9d2b9d864&file=/mnt/extsd/upfile_0.tmp
    char manufacturer[64] = {0};
    char version[64] = {0};
    char md5[64] = {0};
    char path[64] = {0};
    char buf[128] = {0};
    int rval = 0, i = 0;
    unsigned char digest[16] = {0};
    char calc_md5[33] = {0};

    if (global_data.authed == 0) {
        return RESPONSE_CODE_UNAUTHORIZED;
    }
    memset(manufacturer, 0, sizeof(manufacturer));
    memset(version, 0, sizeof(version));
    memset(md5, 0, sizeof(md5));
    memset(path, 0, sizeof(path));
    // parse value
    parse_key(data, "manufacturer=", manufacturer, sizeof(manufacturer) - 1);
    parse_key(data, "version=", version, sizeof(version) - 1);
    parse_key(data, "md5=", md5, sizeof(md5) - 1);
    parse_key(data, "file=", path, sizeof(path) - 1);
    // show info
    debug_line("manufacturer: %s", manufacturer);
    debug_line("version: %s", version);
    debug_line("md5: %s", md5);
    debug_line("file: %s", path);

    // calc md5
    MD5File(path, digest);
    memset(calc_md5, 0, sizeof(calc_md5));
    for (i = 0; i < 16; i++) {
        sprintf(calc_md5 + i * 2, "%02x", digest[i]);
    }
    // compare md5
    if (strncasecmp(calc_md5, md5, 32) != 0) {
        debug_line("%s md5 verify fail", path);
        return RESPONSE_CODE_SYSTEM_ERROR;
    }
    // send response
    send_code(client, 0);
    rename(path, SD_MOUNT_PATH"nexar.bin");
    debug_line("reboot to do fwupdate");
    rtos_api_lib_set_fwupdate_flag();
    sleep(5);
    system("/usr/local/share/script/wifi_stop.sh unload");
    sleep(2);
    rtos_api_lib_reboot();

    return 0;
}

