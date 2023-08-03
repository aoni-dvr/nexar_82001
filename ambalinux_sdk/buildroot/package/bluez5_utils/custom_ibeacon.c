#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <rtos_api_lib.h>

#define IBEACON_ADVERTISING_INTERVAL_MS (100)
#define IBEACON_ADVERTISING_UUID "8a21c76b74e3434192be33fcfdefdd63"
#define IBEACON_MAJOR_NUMBER 10001
#define IBEACON_MINOR_NUMBER 2018
#define IBEACON_RSSI_VALUE -29
//#define cmd_opcode_pack(ogf, ocf) (uint16_t)((ocf & 0x03ff) | (ogf << 10))

#define EIR_FLAGS 0X01
#define EIR_NAME_SHORT 0x08
#define EIR_NAME_COMPLETE 0x09
#define EIR_MANUFACTURE_SPECIFIC 0xFF

static unsigned int *uuid_str_to_data(char *uuid)
{
    char conv[] = "0123456789ABCDEF";
    int len = strlen(uuid);
    unsigned int *data = (unsigned int *)malloc(sizeof(unsigned int) * len);
    unsigned int *dp = data;
    char *cu = uuid;

    for (; cu < uuid + len; dp++, cu += 2) {
        *dp = ((strchr(conv, toupper(*cu)) - conv) * 16) +
              (strchr(conv, toupper(*(cu + 1))) - conv);
    }

    return data;
}

static unsigned int twoc(int in, int t)
{
    return (in < 0) ? (in + (2 << (t - 1))) : in;
}

int ibeacon_enable_advertising(void)
{
    int device_id = hci_get_route(NULL);
    info_s info;

    int device_handle = 0;
    if ((device_handle = hci_open_dev(device_id)) < 0) {
        perror("Could not open device");
        exit(1);
    }

    le_set_advertising_parameters_cp adv_params_cp;
    memset(&adv_params_cp, 0, sizeof(adv_params_cp));
    adv_params_cp.min_interval = htobs(IBEACON_ADVERTISING_INTERVAL_MS);
    adv_params_cp.max_interval = htobs(IBEACON_ADVERTISING_INTERVAL_MS);
    adv_params_cp.chan_map = 7;

    uint8_t status;
    struct hci_request rq;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
    rq.cparam = &adv_params_cp;
    rq.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    int ret = hci_send_req(device_handle, &rq, 1000);
    if (ret < 0) {
        hci_close_dev(device_handle);
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
            errno);
        return (1);
    }

    rtos_api_lib_get_info(&info);

    le_set_advertise_enable_cp advertise_cp;
    memset(&advertise_cp, 0, sizeof(advertise_cp));
    advertise_cp.enable = 0x01;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
    rq.cparam = &advertise_cp;
    rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    ret = hci_send_req(device_handle, &rq, 1000);
    if (ret < 0) {
        hci_close_dev(device_handle);
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
            errno);
        return (1);
    }

    le_set_advertising_data_cp adv_data_cp;
    memset(&adv_data_cp, 0, sizeof(adv_data_cp));

    uint8_t segment_length = 1;
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(EIR_FLAGS);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] = htobs(0x06);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length] = htobs(segment_length - 1);

    adv_data_cp.length += segment_length;

    segment_length = 1;
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(EIR_MANUFACTURE_SPECIFIC);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] = htobs(0x4C);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] = htobs(0x00);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] = htobs(0x02);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] = htobs(0x15);
    segment_length++;

    unsigned int *uuid = uuid_str_to_data(IBEACON_ADVERTISING_UUID);
    int i;
    for (i = 0; i < strlen(IBEACON_ADVERTISING_UUID) / 2; i++) {
        adv_data_cp.data[adv_data_cp.length + segment_length] =
            htobs(uuid[i]);
        segment_length++;
    }

    // Major number
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(info.ibeacon_major >> 8 & 0x00FF);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(info.ibeacon_major & 0x00FF);
    segment_length++;

    // Minor number
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(info.ibeacon_minor >> 8 & 0x00FF);
    segment_length++;
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(info.ibeacon_minor & 0x00FF);
    segment_length++;

    // RSSI calibration
    adv_data_cp.data[adv_data_cp.length + segment_length] =
        htobs(twoc(IBEACON_RSSI_VALUE, 8));
    segment_length++;

    adv_data_cp.data[adv_data_cp.length] = htobs(segment_length - 1);

    adv_data_cp.length += segment_length;

    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISING_DATA;
    rq.cparam = &adv_data_cp;
    rq.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    ret = hci_send_req(device_handle, &rq, 1000);

    hci_close_dev(device_handle);

    if (ret < 0) {
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
            errno);
        return (1);
    }

    if (status) {
        fprintf(stderr, "LE set advertise returned status %d\n",
            status);
        return (1);
    }
}

int ibeacon_disable_advertising(void)
{
    int device_id = hci_get_route(NULL);

    int device_handle = 0;
    if ((device_handle = hci_open_dev(device_id)) < 0) {
        perror("Could not open device");
        return (1);
    }

    le_set_advertise_enable_cp advertise_cp;
    uint8_t status;

    memset(&advertise_cp, 0, sizeof(advertise_cp));

    struct hci_request rq;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
    rq.cparam = &advertise_cp;
    rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    int ret = hci_send_req(device_handle, &rq, 1000);

    hci_close_dev(device_handle);

    if (ret < 0) {
        fprintf(stderr, "Can't set advertise mode: %s (%d)\n",
            strerror(errno), errno);
        return (1);
    }

    if (status) {
        fprintf(stderr,
            "LE set advertise enable on returned status %d\n",
            status);
        return (1);
    }
}

