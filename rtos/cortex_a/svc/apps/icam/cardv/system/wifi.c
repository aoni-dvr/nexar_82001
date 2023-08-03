#include "wifi.h"
#include "app_base.h"

static char use_custom_ssid = 0;
static char custom_ssid[32] = {0};
int wifi_set_custom_ssid(const char *ssid)
{
    memset(custom_ssid, 0, sizeof(custom_ssid));    
    snprintf(custom_ssid, sizeof(custom_ssid) - 1, "%s", ssid);
    use_custom_ssid = 1;

    return 0;
}

int wifi_get_ap_info(wifi_ap_conf_items_s *wifi_conf)
{
    //char response[1024] = {0};
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (app_helper.wifi_booted == 0) {
        return -1;
    }
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_STA) {
        debug_line("wifi work sta mode");
        return -1;
    }
    //memset(response, 0, sizeof(response));
    //app_helper.linux_system_with_response("cat /tmp/hostapd.wlan0.conf | grep ^ssid=", response);
    //if (strncmp(response, "ssid=", strlen("ssid=")) != 0) {
        //return -1;
    //}
    memset(wifi_conf->ssid, 0, sizeof(wifi_conf->ssid));
    //snprintf(wifi_conf->ssid, sizeof(wifi_conf->ssid) - 1, "%s", response + strlen("ssid="));
    if (use_custom_ssid) {
        snprintf(wifi_conf->ssid, sizeof(wifi_conf->ssid) - 1, "%s%s", (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_P2P) ? "DIRECT-" : "", custom_ssid);
    } else {
        if (pSvcUserPref->WifiSetting.Use5G) {
            snprintf(wifi_conf->ssid, sizeof(wifi_conf->ssid) - 1, "%s%s(5G)-%s",
                (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_P2P) ? "DIRECT-" : "", WIFI_SSID_PREFIX, pSvcUserPref->WifiSetting.ApSSID);
        } else {
            snprintf(wifi_conf->ssid, sizeof(wifi_conf->ssid) - 1, "%s%s-%s",
                (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_P2P) ? "DIRECT-" : "", WIFI_SSID_PREFIX, pSvcUserPref->WifiSetting.ApSSID);
        }
    }

    //memset(response, 0, sizeof(response));
    //app_helper.linux_system_with_response("cat /tmp/hostapd.wlan0.conf | grep ^wpa_passphrase=", response);
    //if (strncmp(response, "wpa_passphrase=", strlen("wpa_passphrase=")) != 0) {
        //return -1;
    //}
    memset(wifi_conf->password, 0, sizeof(wifi_conf->password));
    //snprintf(wifi_conf->password, sizeof(wifi_conf->password) - 1, "%s", response + strlen("wpa_passphrase="));
    snprintf(wifi_conf->password, sizeof(wifi_conf->password) - 1, "%s", pSvcUserPref->WifiSetting.ApPassword);

    memset(wifi_conf->ip, 0, sizeof(wifi_conf->ip));
    snprintf(wifi_conf->ip, sizeof(wifi_conf->ip) - 1, "%s", pSvcUserPref->WifiSetting.ApIP);

    return 0;
}

