#ifndef __WIFI_H__
#define __WIFI_H__

typedef struct _wifi_ap_conf_items_s {
    char version[64];
    char ssid[64];
    char password[64];
    char ip[32];
    int hide_ssid;
} wifi_ap_conf_items_s;

int wifi_get_ap_info(wifi_ap_conf_items_s *wifi_conf);
int wifi_set_custom_ssid(const char *ssid);

#endif//__WIFI_H__

