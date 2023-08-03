#include "rtos_api_lib.h"
#include "serial_port.h"

#define AT_DEV "/dev/ttyUSB3"
#define LTE_BURN_INFO_NAME "burn.ini"

static int lte_burn_info(const char *data)
{
	int fd = -1, rval = -1;
	char buf[512] = {0};
	int timeout = 3;

	if (serial_init(AT_DEV, &fd, 0, 115200, 0) < 0) {
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf) - 1, "AT+QFUPL=\"%s\",%d\r\n", LTE_BURN_INFO_NAME, (int)(strlen(data)));
	serial_write(fd, buf, strlen(buf));
	while (timeout-- >= 0) {
		memset(buf, 0, sizeof(buf));
		serial_read(fd, (unsigned char *)buf, sizeof(buf) - 1, 1000);
		if (strstr(buf, "CONNECT") != NULL) {
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf) - 1, "%s\n", data);
			serial_write(fd, buf, strlen(buf));
		} else if (strstr(buf, "+QFUPL") != NULL) {
			if (strstr(buf, "OK") != NULL) {
				rval = 0;
			} else {
				rval = -1;
			}
			break;
		} else if (strstr(buf, "+CME ERROR:") != NULL) {
			rval = -1;
			break;
		}
	}
	serial_close(fd);

	return rval;
}

static int lte_read_burn_info(char *data)
{
	int fd = -1, rval = -1;
	char buf[256] = {0};
	int timeout = 3;

	if (serial_init(AT_DEV, &fd, 0, 115200, 0) < 0) {
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf) - 1, "AT+QFDWL=\"%s\"\r\n", LTE_BURN_INFO_NAME);
	serial_write(fd, buf, strlen(buf));
	while (timeout-- >= 0) {
		memset(buf, 0, sizeof(buf));
		serial_read(fd, (unsigned char *)buf, sizeof(buf) - 1, 1000);
		if (strstr(buf, "CONNECT") != NULL) {
			memset(buf, 0, sizeof(buf));
			serial_read(fd, (unsigned char *)buf, sizeof(buf) - 1, 2000);
			if (data != NULL && strlen(buf) > 0) {
				memcpy(data, buf, strlen(buf));
			}
			rval = strlen(buf);
		} else if (strstr(buf, "+QFDWL") != NULL) {
			if (strstr(buf, "OK") != NULL) {
				rval = 0;
			} else {
				rval = -1;
			}
			break;
		} else if (strstr(buf, "+CME ERROR:") != NULL) {
			rval = -1;
			break;
		}
	}
	serial_close(fd);

	return rval;
}

static int lte_erase_burn_info(void)
{
	int fd = -1, rval = -1;
	char buf[256] = {0};
	int timeout = 3;
	
	if (serial_init(AT_DEV, &fd, 0, 115200, 0) < 0) {
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf) - 1, "AT+QFDEL=\"%s\"\r\n", LTE_BURN_INFO_NAME);
	serial_write(fd, buf, strlen(buf));
	while (timeout-- >= 0) {
		memset(buf, 0, sizeof(buf));
		serial_read(fd, (unsigned char *)buf, sizeof(buf) - 1, 1000);
		if (strstr(buf, "OK") != NULL) {
			rval = 0;
			break;
		} else if (strstr(buf, "+CME ERROR:") != NULL) {
			rval = -1;
			break;
		}
	}
	serial_close(fd);

	return rval;
}

int main(int argc, char **argv)
{
	unsigned char wifi_mac[6] = {0};
	unsigned char bt_mac[6] = {0};

	if (argc >= 2 && strcmp(argv[1], "wifi_mac") == 0) {
		rtos_api_lib_get_burn_info(wifi_mac, bt_mac);
		printf("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", wifi_mac[0], wifi_mac[1], wifi_mac[2], wifi_mac[3], wifi_mac[4], wifi_mac[5]);
	} else if (argc >= 2 && strcmp(argv[1], "bt_mac") == 0) {
		rtos_api_lib_get_burn_info(wifi_mac, bt_mac);
		printf("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);
	} else if (argc >= 2 && strcmp(argv[1], "lte") == 0) {
		if (argc == 5 && strcmp(argv[2], "flash") == 0) {
			char data[256] = {0};
			if (strlen(argv[3]) <= 0) {
				printf("invalid sn,%s\n", argv[3]);
				return -1;
			}
			if (strlen(argv[4]) != 17) {
				printf("invalid mac,%s\n", argv[4]);
				return -1;
			}
			lte_erase_burn_info();
			memset(data, 0, sizeof(data));
			snprintf(data, sizeof(data) - 1, "sn:%s\nmac:%s\n", argv[3], argv[4]);
			for (int i = 0; i < 3; i++) {
				if (lte_burn_info(data) >= 0) {
					return 0;
				}
			}
			return -1;
		} else if (strcmp(argv[2], "read") == 0) {
			char data[256] = {0};
			for (int i = 0; i < 3; i++) {
				memset(data, 0, sizeof(data));
				if (lte_read_burn_info(data) >= 0) {
					break;
				}
			}
			if (strlen(data) > 0) {
				char usb_wifi_mac[32] = {0};
				char usb_wifi_sn[32] = {0};
				memset(usb_wifi_mac, 0, sizeof(usb_wifi_mac));
				memset(usb_wifi_sn, 0, sizeof(usb_wifi_sn));
				sscanf(data, "sn:%s\nmac:%s\n", usb_wifi_sn, usb_wifi_mac);
				if (argc == 4 && strcmp(argv[3], "sn") == 0) {
					printf("%s", usb_wifi_sn);
				} else if (argc == 4 && strcmp(argv[3], "wifi_mac") == 0) {
					printf("%s", usb_wifi_mac);
				} else {
					printf("%s|%s", usb_wifi_sn, usb_wifi_mac);
				}
			}
		} else if (strcmp(argv[2], "erase") == 0) {
			lte_erase_burn_info();
		}
	}

    return 0;
}
