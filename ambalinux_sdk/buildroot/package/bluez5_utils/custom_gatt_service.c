#include "cmd_ctrl.c"

const char *customer_service1_uuid = "8a21c76b-74e3-4341-92be-33fcfdefdd64";
const char *customer_service1_control_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde0de64";
const char *customer_service1_config_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde1de64";
const char *customer_service1_state_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde2de64";
const char *customer_service1_ssid_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde3de64";
const char *customer_service1_info_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde4de64";

const char *customer_service2_uuid = "8a21c76b-74e3-4341-92be-33fcfdefdd65";
const char *customer_service2_imu_notify_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde2de65";
const char *customer_service2_gnss_notify_characteristic_uuid = "8a21c76b-74e3-4341-92be-33fcfde3de65";

static int send_imu_notification(uint8_t *data, uint16_t len);
static int send_gnss_notification(uint8_t *data, uint16_t len);

static void characteristic_config_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    config_s info;
    unsigned char value[1024];
    int index = 0;
    char mac[32] = {0};

    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_config(&info);
    value[index++] = 0x00;//code
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = 0x00;//payload length
    value[index++] = 0x00;
    value[index++] = info.support_wifi & 0xFF;
    value[index++] = info.support_wifi_connect & 0xFF;
    value[index++] = info.wifi_connect_mode & 0xFF;
    value[index++] = info.wifi_type & 0xFF;
    value[index++] = info.wifi_channel & 0xFF;
    value[index++] = info.wifi_boot_connect_mode & 0xFF;
    value[index++] = info.rtsp_flag & 0xFF;
    //ota version
    value[index++] = strlen(info.otaVersion) & 0xFF;
    memcpy(value + index, info.otaVersion, strlen(info.otaVersion));
    index += strlen(info.otaVersion);
    //token
    value[index++] = strlen(global_data.dev_info.token) & 0xFF;
    memcpy(value + index, global_data.dev_info.token, strlen(global_data.dev_info.token));
    index += strlen(global_data.dev_info.token);
    //manufacturer
    value[index++] = strlen(info.manufacturer) & 0xFF;
    memcpy(value + index, info.manufacturer, strlen(info.manufacturer));
    index += strlen(info.manufacturer);
    //wifi_ssid
    value[index++] = strlen(info.wifi_ssid) & 0xFF;
    memcpy(value + index, info.wifi_ssid, strlen(info.wifi_ssid));
    index += strlen(info.wifi_ssid);
    //product_sn
    value[index++] = strlen(info.product_sn) & 0xFF;
    memcpy(value + index, info.product_sn, strlen(info.product_sn));
    index += strlen(info.product_sn);
    //product_vsn
    value[index++] = strlen(info.product_vsn) & 0xFF;
    memcpy(value + index, info.product_vsn, strlen(info.product_vsn));
    index += strlen(info.product_vsn);
    //product_wifimac
    memset(mac, 0, sizeof(mac));
    rtos_api_lib_get_netcard_info("wlan0", NULL, mac);
    sscanf(mac, "%x:%x:%x:%x:%x:%x", &value[index++], &value[index++], &value[index++], &value[index++], &value[index++], &value[index++]);
    //product_cpuid
    value[index++] = strlen(info.product_cpuid) & 0xFF;
    memcpy(value + index, info.product_cpuid, strlen(info.product_cpuid));
    index += strlen(info.product_cpuid);
    //wifi_ap_ssid
    value[index++] = strlen(info.wifi_ap_ssid) & 0xFF;
    memcpy(value + index, info.wifi_ap_ssid, strlen(info.wifi_ap_ssid));
    index += strlen(info.wifi_ap_ssid);
    //wifi_ap_passwd
    value[index++] = strlen(info.wifi_ap_passwd) & 0xFF;
    memcpy(value + index, info.wifi_ap_passwd, strlen(info.wifi_ap_passwd));
    index += strlen(info.wifi_ap_passwd);
    //wifi_password
    value[index++] = strlen(info.wifiPsd) & 0xFF;
    memcpy(value + index, info.wifiPsd, strlen(info.wifiPsd));
    index += strlen(info.wifiPsd);

    value[4] = (index >> 8) & 0xFF;
    value[5] = index & 0xFF;

    gatt_db_attribute_read_result(attrib, id, 0, value, index);
}

static void characteristic_state_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    state_s info;
    unsigned char value[1024];
    int index = 0;

    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_state(&info);
    value[index++] = 0x00;//code
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = 0x00;//payload length
    value[index++] = 0x00;
    value[index++] = info.record;
    value[index++] = info.mute;
    value[index++] = info.tf_state;
    value[index++] = info.backcamera;
    value[index++] = info.battery;
    value[index++] = info.battery_charging;
    value[index++] = info.usb_state;
    value[index++] = info.acc_state;
    value[index++] = info.obd_voltage & 0xFF;
    value[index++] = info.gps & 0xFF;
    value[index++] = info.rear_record_disable & 0xFF;
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = 0x00;
    value[index++] = (info.uptime >> 24) & 0xff;
    value[index++] = (info.uptime >> 16) & 0xff;
    value[index++] = (info.uptime >> 8) & 0xff;
    value[index++] = (info.uptime >> 0) & 0xff;
    value[index++] = (info.timenow >> 24) & 0xff;
    value[index++] = (info.timenow >> 16) & 0xff;
    value[index++] = (info.timenow >> 8) & 0xff;
    value[index++] = (info.timenow >> 0) & 0xff;
    value[index++] = (info.timezone >> 24) & 0xff;
    value[index++] = (info.timezone >> 16) & 0xff;
    value[index++] = (info.timezone >> 8) & 0xff;
    value[index++] = (info.timezone >> 0) & 0xff;
    value[index++] = (info.battery_voltage >> 24) & 0xff;
    value[index++] = (info.battery_voltage >> 16) & 0xff;
    value[index++] = (info.battery_voltage >> 8) & 0xff;
    value[index++] = (info.battery_voltage >> 0) & 0xff;

    value[4] = (index >> 8) & 0xFF;
    value[5] = index & 0xFF;

    gatt_db_attribute_read_result(attrib, id, 0, value, index);
}

static void characteristic_ssid_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    info_s info;

    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_info(&info);
    gatt_db_attribute_read_result(attrib, id, 0, info.wifi_ssid, strlen(info.wifi_ssid));
}

static void characteristic_info_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    unsigned char value[256];
    info_s info;
    char ip[32] = {0};
    int index = 0, i = 0, capabilities_len = 0;

    memset(&info, 0, sizeof(info));
    rtos_api_lib_get_info(&info);
    value[index++] = ((info.protocol_ver[1] & 0xF) << 4) | (info.protocol_ver[0] & 0xF);
    value[index++] = (info.ibeacon_major >> 8) & 0xFF;
    value[index++] = info.ibeacon_major & 0xFF;
    value[index++] = (info.ibeacon_minor >> 8) & 0xFF;
    value[index++] = info.ibeacon_minor & 0xFF;
    value[index++] = info.wifi_status;
    memset(ip, 0, sizeof(ip));
    rtos_api_lib_get_netcard_info("wlan0", ip, NULL);
    sscanf(ip, "%d.%d.%d.%d", &value[index++], &value[index++], &value[index++], &value[index++]);
    //vendor
    value[index++] = strlen(info.vendor) & 0xFF;
    memcpy(value + index, info.vendor, strlen(info.vendor));
    index += strlen(info.vendor);
    //model
    value[index++] = strlen(info.model) & 0xFF;
    memcpy(value + index, info.model, strlen(info.model));
    index += strlen(info.model);
    //sn
    value[index++] = strlen(info.sn) & 0xFF;
    memcpy(value + index, info.sn, strlen(info.sn));
    index += strlen(info.sn);
    //wifi ssid
    value[index++] = strlen(info.wifi_ssid) & 0xFF;
    memcpy(value + index, info.wifi_ssid, strlen(info.wifi_ssid));
    index += strlen(info.wifi_ssid);
    //capabilities
    capabilities_len = strlen(info.capabilities) / 2;
    if (strlen(info.capabilities) % 2 != 0) {
        capabilities_len += 1;
    }
    value[index++] = capabilities_len & 0xFF;
    if (strlen(info.capabilities) / 2 > 0) {
        for (i = 0 ; i < strlen(info.capabilities) / 2; i++) {
            char tmp[3] = {0};
            memcpy(tmp, info.capabilities + 2 * i, 2);
            tmp[2] = '\0';
            value[index++] = atoi(tmp) & 0xFF;
        }
    }
    if (strlen(info.capabilities) % 2 != 0) {
        value[index++] = atoi(info.capabilities[strlen(info.capabilities) - 1]) * 16;
    }

    gatt_db_attribute_read_result(attrib, id, 0, value, index);
}

static unsigned char control_response[256] = {0};
static unsigned int control_response_len = 0;
static void characteristic_control_read_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    gatt_db_attribute_read_result(attrib, id, 0, control_response, control_response_len);
}

static void characteristic_control_write_cb(struct gatt_db_attribute *attrib,
                    unsigned int id, uint16_t offset,
                    const uint8_t *value, size_t len,
                    uint8_t opcode, struct bt_att *att,
                    void *user_data)
{
    uint8_t ecode = 0;
    ctrl_item_s item;
    int code = 0;

    if (!value || len <= 0) {
        printf("#%d BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN\n", __LINE__);
        ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
        goto done;
    }
    if (offset) {
        printf("#%d BT_ATT_ERROR_INVALID_OFFSET\n", __LINE__);
        ecode = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }
    memset(&item, 0, sizeof(item));
    item.cmd = (value[0] << 8) | value[1];
    item.arg = (value[2] << 24) | (value[3] << 16) | (value[4] << 8) | value[5];
    if (len > 6 && value[6] > 0) {
        memcpy(item.data, value + 7, value[6]);
    }
    code = cmd_ctrl_impl(item);
    control_response_len = 0;
    control_response[control_response_len++] = (code >> 24) & 0xFF;
    control_response[control_response_len++] = (code >> 16) & 0xFF;
    control_response[control_response_len++] = (code >> 8) & 0xFF;
    control_response[control_response_len++] = (code) & 0xFF;
    control_response[control_response_len++] = (item.cmd >> 8) & 0xFF;
    control_response[control_response_len++] = (item.cmd) & 0xFF;
    control_response[control_response_len++] = 0x00;    
    control_response[control_response_len++] = 0x00;
done:
    gatt_db_attribute_write_result(attrib, id, ecode);
}

static int send_imu_notification(uint8_t *data, uint16_t len)
{
    if (!G_server) {
        return -1;
    }
    bt_gatt_server_send_notification(G_server->gatt, G_server->notify_imu_handle, (uint8_t *)data, len);

    return 0;
}

static int send_gnss_notification(uint8_t *data, uint16_t len)
{
    if (!G_server) {
        return -1;
    }
    bt_gatt_server_send_notification(G_server->gatt, G_server->notify_gnss_handle, (uint8_t *)data, len);

    return 0;
}

static void customer_service2_imu_stream_write_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					const uint8_t *value, size_t len,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t ecode = 0;
    bool enabled = false;

	if (!value || len != 2) {
		ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
		goto done;
	}

	if (offset) {
		ecode = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	if (value[0] == 0x00) {
		enabled = false;
	} else if (value[0] == 0x01) {
		enabled = true;
	} else {
		ecode = 0x80;
    }
    rtos_api_lib_set_bt_imu_stream_enable(enabled ? 1 : 0);
	syslog(LOG_INFO, "imu stream: Enabled: %s\n", enabled ? "true" : "false");
done:
	gatt_db_attribute_write_result(attrib, id, ecode);
}

static void customer_service2_gnss_stream_write_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					const uint8_t *value, size_t len,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t ecode = 0;
    bool enabled = false;

	if (!value || len != 2) {
		ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
		goto done;
	}

	if (offset) {
		ecode = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	if (value[0] == 0x00) {
		enabled = false;
	} else if (value[0] == 0x01) {
		enabled = true;
	} else {
		ecode = 0x80;
    }
    rtos_api_lib_set_bt_gnss_stream_enable(enabled ? 1 : 0);
	syslog(LOG_INFO, "gnss stream: Enabled: %s\n", enabled ? "true" : "false");
done:
	gatt_db_attribute_write_result(attrib, id, ecode);
}

static void populate_custom_service(struct server *server)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *service, *attrib;

    /* Add basic Service */
    bt_string_to_uuid(&uuid, customer_service1_uuid);
    service = gatt_db_add_service(server->db, &uuid, true, 12);

    bt_string_to_uuid(&uuid, customer_service1_control_characteristic_uuid);
    gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                        BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_WRITE,
                        characteristic_control_read_cb, characteristic_control_write_cb, server);

    bt_string_to_uuid(&uuid, customer_service1_config_characteristic_uuid);
    gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_READ,
                        BT_GATT_CHRC_PROP_READ,
                        characteristic_config_read_cb, NULL, server);

    bt_string_to_uuid(&uuid, customer_service1_state_characteristic_uuid);
    gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_READ,
                        BT_GATT_CHRC_PROP_READ,
                        characteristic_state_read_cb, NULL, server);

    bt_string_to_uuid(&uuid, customer_service1_ssid_characteristic_uuid);
    gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_READ,
                        BT_GATT_CHRC_PROP_READ,
                        characteristic_ssid_read_cb, NULL, server);

    bt_string_to_uuid(&uuid, customer_service1_info_characteristic_uuid);
    gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_READ,
                        BT_GATT_CHRC_PROP_READ,
                        characteristic_info_read_cb, NULL, server);
    gatt_db_service_set_active(service, true);

    /* Add notify Service */
    bt_string_to_uuid(&uuid, customer_service2_uuid);
    service = gatt_db_add_service(server->db, &uuid, true, 12);

    bt_string_to_uuid(&uuid, customer_service2_imu_notify_characteristic_uuid);
    attrib = gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_NONE,
                        BT_GATT_CHRC_PROP_NOTIFY,
                        NULL, NULL, server);
    server->notify_imu_handle = gatt_db_attribute_get_handle(attrib);
    bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
    gatt_db_service_add_descriptor(attrib, &uuid,
                        BT_ATT_PERM_WRITE,
                        NULL,
                        customer_service2_imu_stream_write_cb, server);

    bt_string_to_uuid(&uuid, customer_service2_gnss_notify_characteristic_uuid);
    attrib = gatt_db_service_add_characteristic(service, &uuid,
                        BT_ATT_PERM_NONE,
                        BT_GATT_CHRC_PROP_NOTIFY,
                        NULL, NULL, server);
    server->notify_gnss_handle = gatt_db_attribute_get_handle(attrib);
    bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
    gatt_db_service_add_descriptor(attrib, &uuid,
                        BT_ATT_PERM_WRITE,
                        NULL,
                        customer_service2_gnss_stream_write_cb, server);

    gatt_db_service_set_active(service, true);
}

