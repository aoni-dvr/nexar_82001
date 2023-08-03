/*
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2016 Ambarella, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
/* Modified from tools/btgatt-server.c */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <syslog.h>

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/shared/mainloop.h"
#include "src/shared/util.h"
#include "src/shared/att.h"
#include "src/shared/queue.h"
#include "src/shared/timeout.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"

#include "global.h"

#define UUID_GAP            0x1800
#define UUID_GATT           0x1801

static char att_key[BT_ATT_DEFAULT_LE_MTU];

static int G_sd;

#define ATT_CID 4

#define COLOR_OFF   "\x1B[0m"
#define COLOR_RED   "\x1B[0;91m"
#define COLOR_GREEN "\x1B[0;92m"
#define COLOR_YELLOW    "\x1B[0;93m"
#define COLOR_BLUE  "\x1B[0;94m"
#define COLOR_MAGENTA   "\x1B[0;95m"
#define COLOR_BOLDGRAY  "\x1B[1;30m"
#define COLOR_BOLDWHITE "\x1B[1;37m"

static char test_device_name[32] = "gatt-server";

static bool verbose = false;
static bool daemon_arg = true;

struct server {
    int fd;
    struct bt_att *att;
    struct gatt_db *db;
    struct bt_gatt_server *gatt;

    uint8_t *device_name;
    size_t name_len;

    uint16_t gatt_svc_chngd_handle;
    bool svc_chngd_enabled;

    uint16_t notify_imu_handle;
    uint16_t notify_gnss_handle;
} *G_server;

#include "custom_gatt_service.c"
#include "custom_ibeacon.c"
#include "stream_notify.c"

static void att_disconnect_cb(int err, void *user_data)
{
    syslog(LOG_INFO, "#%d Device disconnected: %s\n", __LINE__, strerror(err));

    mainloop_quit();
}

static void att_debug_cb(const char *str, void *user_data)
{
    const char *prefix = user_data;

    printf(COLOR_BOLDGRAY "%s" COLOR_BOLDWHITE "%s\n" COLOR_OFF, prefix,
                                    str);
}

static void gatt_debug_cb(const char *str, void *user_data)
{
    const char *prefix = user_data;

    printf(COLOR_GREEN "%s%s\n" COLOR_OFF, prefix, str);
}

static void gap_device_name_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    struct server *server = user_data;
    uint8_t error = 0;
    size_t len = 0;
    const uint8_t *value = NULL;

    printf("GAP Device Name Read called\n");

    len = server->name_len;

    if (offset > len) {
        error = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    len -= offset;
    value = len ? &server->device_name[offset] : NULL;

done:
    gatt_db_attribute_read_result(attrib, id, error, value, len);
}

static void gap_device_name_write_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    const uint8_t *value, size_t len,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    struct server *server = user_data;
    uint8_t error = 0;

    printf("GAP Device Name Write called\n");

    /* If the value is being completely truncated, clean up and return */
    if (!(offset + len)) {
        free(server->device_name);
        server->device_name = NULL;
        server->name_len = 0;
        goto done;
    }

    /* Implement this as a variable length attribute value. */
    if (offset > server->name_len) {
        error = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    if (offset + len != server->name_len) {
        uint8_t *name;

        name = realloc(server->device_name, offset + len);
        if (!name) {
            error = BT_ATT_ERROR_INSUFFICIENT_RESOURCES;
            goto done;
        }

        server->device_name = name;
        server->name_len = offset + len;
    }

    if (value)
        memcpy(server->device_name + offset, value, len);

done:
    gatt_db_attribute_write_result(attrib, id, error);
}

static void gap_device_name_ext_prop_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    uint8_t value[2];

    printf("Device Name Extended Properties Read called\n");

    value[0] = BT_GATT_CHRC_EXT_PROP_RELIABLE_WRITE;
    value[1] = 0;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

static void gatt_service_changed_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    printf("Service Changed Read called\n");

    gatt_db_attribute_read_result(attrib, id, 0, NULL, 0);
}

static void gatt_svc_chngd_ccc_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    struct server *server = user_data;
    uint8_t value[2];

    printf("Service Changed CCC Read called\n");

    value[0] = server->svc_chngd_enabled ? 0x02 : 0x00;
    value[1] = 0x00;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

static void gatt_svc_chngd_ccc_write_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    const uint8_t *value, size_t len,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    struct server *server = user_data;
    uint8_t ecode = 0;

    printf("Service Changed CCC Write called\n");

    if (!value || len != 2) {
        ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
        goto done;
    }

    if (offset) {
        ecode = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    if (value[0] == 0x00)
        server->svc_chngd_enabled = false;
    else if (value[0] == 0x02)
        server->svc_chngd_enabled = true;
    else
        ecode = 0x80;

    printf("Service Changed Enabled: %s\n",
                server->svc_chngd_enabled ? "true" : "false");

done:
    gatt_db_attribute_write_result(attrib, id, ecode);
}

static void conf_cb(void *user_data)
{
    printf("Received confirmation\n");
}

static void confirm_write(struct gatt_db_attribute *attr, int err,
                            void *user_data)
{
    if (!err)
        return;

    syslog(LOG_INFO, "#%d Error caching attribute %p - err: %d\n", __LINE__, attr, err);
    exit(1);
}

static void populate_gap_service(struct server *server)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *service, *tmp;
    uint16_t appearance;

    /* Add the GAP service */
    bt_uuid16_create(&uuid, UUID_GAP);
    service = gatt_db_add_service(server->db, &uuid, true, 6);

    /*
     * Device Name characteristic. Make the value dynamically read and
     * written via callbacks.
     */
    bt_uuid16_create(&uuid, GATT_CHARAC_DEVICE_NAME);
    gatt_db_service_add_characteristic(service, &uuid,
                    BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                    BT_GATT_CHRC_PROP_READ |
                    BT_GATT_CHRC_PROP_EXT_PROP,
                    gap_device_name_read_cb,
                    gap_device_name_write_cb,
                    server);

    bt_uuid16_create(&uuid, GATT_CHARAC_EXT_PROPER_UUID);
    gatt_db_service_add_descriptor(service, &uuid, BT_ATT_PERM_READ,
                    gap_device_name_ext_prop_read_cb,
                    NULL, server);

    /*
     * Appearance characteristic. Reads and writes should obtain the value
     * from the database.
     */
    bt_uuid16_create(&uuid, GATT_CHARAC_APPEARANCE);
    tmp = gatt_db_service_add_characteristic(service, &uuid,
                            BT_ATT_PERM_READ,
                            BT_GATT_CHRC_PROP_READ,
                            NULL, NULL, server);

    /*
     * Write the appearance value to the database, since we're not using a
     * callback.
     */
    put_le16(128, &appearance);
    gatt_db_attribute_write(tmp, 0, (void *) &appearance,
                            sizeof(appearance),
                            BT_ATT_OP_WRITE_REQ,
                            NULL, confirm_write,
                            NULL);

    gatt_db_service_set_active(service, true);
}

static void populate_gatt_service(struct server *server)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *service, *svc_chngd;

    /* Add the GATT service */
    bt_uuid16_create(&uuid, UUID_GATT);
    service = gatt_db_add_service(server->db, &uuid, true, 4);

    bt_uuid16_create(&uuid, GATT_CHARAC_SERVICE_CHANGED);
    svc_chngd = gatt_db_service_add_characteristic(service, &uuid,
            BT_ATT_PERM_READ,
            BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_INDICATE,
            gatt_service_changed_cb,
            NULL, server);
    server->gatt_svc_chngd_handle = gatt_db_attribute_get_handle(svc_chngd);

    bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
    gatt_db_service_add_descriptor(service, &uuid,
                BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                gatt_svc_chngd_ccc_read_cb,
                gatt_svc_chngd_ccc_write_cb, server);

    gatt_db_service_set_active(service, true);
}

static void populate_db(struct server *server)
{
    populate_gap_service(server);
    populate_gatt_service(server);
    populate_custom_service(server);
}

static struct server *server_create(int fd, uint16_t mtu)
{
    struct server *server;
    size_t name_len = strlen(test_device_name);

    server = new0(struct server, 1);
    if (!server) {
        syslog(LOG_INFO, "#%d Failed to allocate memory for server\n", __LINE__);
        return NULL;
    }

    server->att = bt_att_new(fd, false);
    if (!server->att) {
        syslog(LOG_INFO, "#%d Failed to initialze ATT transport layer\n", __LINE__);
        goto fail;
    }

    if (!bt_att_set_close_on_unref(server->att, true)) {
        syslog(LOG_INFO, "#%d Failed to set up ATT transport layer\n", __LINE__);
        goto fail;
    }

    if (!bt_att_register_disconnect(server->att, att_disconnect_cb, NULL,
                                    NULL)) {
        syslog(LOG_INFO, "#%d Failed to set ATT disconnect handler\n", __LINE__);
        goto fail;
    }

    server->name_len = name_len + 1;
    server->device_name = malloc(name_len + 1);
    if (!server->device_name) {
        syslog(LOG_INFO, "#%d Failed to allocate memory for device name\n", __LINE__);
        goto fail;
    }

    memcpy(server->device_name, test_device_name, name_len);
    server->device_name[name_len] = '\0';

    server->fd = fd;
    server->db = gatt_db_new();
    if (!server->db) {
        syslog(LOG_INFO, "#%d Failed to create GATT database\n", __LINE__);
        goto fail;
    }

    server->gatt = bt_gatt_server_new(server->db, server->att, mtu, 0);
    if (!server->gatt) {
        syslog(LOG_INFO, "#%d Failed to create GATT server\n", __LINE__);
        goto fail;
    }

    if (verbose) {
        bt_att_set_debug(server->att, att_debug_cb, "att: ", NULL);
        bt_gatt_server_set_debug(server->gatt, gatt_debug_cb,
                            "server: ", NULL);
    }

    /* bt_gatt_server already holds a reference */
    populate_db(server);

    return server;

fail:
    gatt_db_unref(server->db);
    free(server->device_name);
    bt_att_unref(server->att);
    free(server);

    return NULL;
}

static void server_destroy(struct server *server)
{
    stream_notify_deinit();
    bt_gatt_server_unref(server->gatt);
    gatt_db_unref(server->db);
}

static void usage(void)
{
    printf("Options:\n"
        "\t-n, --name\t\t\tSet Device Name\n"
        "\t-v, --verbose\t\t\tEnable extra logging\n"
        "\t-f, --foreground\t\t\tRun in foreground\n"
        "\t-h, --help\t\t\tDisplay help\n");
}

static struct option main_options[] = {
    { "help",       0, 0, 'h' },
    { "name",       1, 0, 'n' },
    { "verbose",        0, 0, 'v' },
    { "forefround",     0, 0, 'f' },
    { }
};

static int l2cap_le_att_listen_and_accept(bdaddr_t *src, int sec,
                            uint8_t src_type)
{
    int sk, nsk;
    struct sockaddr_l2 srcaddr, addr;
    socklen_t optlen;
    struct bt_security btsec;
    char ba[18];

    sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (sk < 0) {
        syslog(LOG_INFO, "#%d Failed to create L2CAP socket", __LINE__);
        return -1;
    }

    /* Set up source address */
    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.l2_family = AF_BLUETOOTH;
    srcaddr.l2_cid = htobs(ATT_CID);
    srcaddr.l2_bdaddr_type = src_type;
    bacpy(&srcaddr.l2_bdaddr, src);

    if (bind(sk, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
        syslog(LOG_INFO, "#%d Failed to bind L2CAP socket", __LINE__);
        goto fail;
    }

    /* Set the security level */
    memset(&btsec, 0, sizeof(btsec));
    btsec.level = sec;
    if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &btsec,
                            sizeof(btsec)) != 0) {
        syslog(LOG_INFO, "#%d Failed to set L2CAP security level\n", __LINE__);
        goto fail;
    }

    if (listen(sk, 10) < 0) {
        syslog(LOG_INFO, "#%d Listening on socket failed", __LINE__);
        goto fail;
    }

    printf("#%d Started listening on ATT channel. Waiting for connections\n", __LINE__);

    memset(&addr, 0, sizeof(addr));
    optlen = sizeof(addr);
    nsk = accept(sk, (struct sockaddr *) &addr, &optlen);
    if (nsk < 0) {
        syslog(LOG_INFO, "#%d Accept failed", __LINE__);
        goto fail;
    }

    ba2str(&addr.l2_bdaddr, ba);
    syslog(LOG_INFO, "#%d Connect from %s", __LINE__, ba);
    close(sk);

    return nsk;

fail:
    close(sk);
    return -1;
}

global_s global_data;

int main(int argc, char *argv[])
{
    int opt;
    bdaddr_t src_addr;
    int fd;
    char cmd[64] = "\0";

    for (opt = 0; opt < argc; opt++) {
        strcat(cmd, argv[opt]);
        strcat(cmd, " ");
    }
    //read global data
    memset(&global_data, 0, sizeof(global_data));
    if (rtos_api_lib_get_dev_info(&global_data.dev_info) < 0) {
        syslog(LOG_INFO, "%s get dev info failed", argv[0]);
    }
    while ((opt = getopt_long(argc, argv, "+hfvn:", main_options, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return EXIT_SUCCESS;
        case 'v':
            verbose = true;
            break;
        case 'f':
            daemon_arg = false;
            break;
        case 'n':
            snprintf(test_device_name, sizeof(test_device_name), "%s", optarg);
            break;
        default:
            fprintf(stderr, "Invalid option: %c\n", opt);
            return EXIT_FAILURE;
        }
    }

    argc -= optind;
    argv -= optind;
    optind = 0;

    if (argc) {
        usage();
        return EXIT_SUCCESS;
    }

    if (daemon_arg) {
        if (daemon(0, 1)) {
            fprintf(stderr, "Couldn't start daemon!\n");
            exit(1);
        }
    }

    if (stream_notify_init() < 0) {
        syslog(LOG_INFO, "#%d Failed to int stream notify\n", __LINE__);
        return EXIT_FAILURE;
    }

    system("/usr/local/share/script/bt_le_connect_hook.sh leadv5");
    ibeacon_enable_advertising();
    bacpy(&src_addr, BDADDR_ANY);
    fd = l2cap_le_att_listen_and_accept(&src_addr, BT_SECURITY_LOW, BDADDR_LE_PUBLIC);
    if (fd < 0) {
        syslog(LOG_INFO, "#%d Failed to accept L2CAP ATT connection\n", __LINE__);
        return EXIT_FAILURE;
    }
    ibeacon_disable_advertising();

    mainloop_init();
    G_server = server_create(fd, 128);
    if (!G_server) {
        close(fd);
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "#%d Running GATT server\n", __LINE__);
    mainloop_run();
    printf("\n\nShutting down...\n");
    server_destroy(G_server);
    system(cmd);

    return EXIT_SUCCESS;
}
