#include "rtos_api_lib.h"

int main(int argc, char **argv)
{
    basic_info_s basic_info;

    memset(&basic_info, 0, sizeof(basic_info));
    rtos_api_lib_get_basic_info(&basic_info);
    printf("cpu_id: %s\n", basic_info.cpu_id);
    printf("device_sn: %s\n", basic_info.device_sn);
    printf("burn_wifi_mac: %02x:%02x:%02x:%02x:%02x:%02x\n", basic_info.wifi_mac[0], basic_info.wifi_mac[1], basic_info.wifi_mac[2],
                                                            basic_info.wifi_mac[3], basic_info.wifi_mac[4], basic_info.wifi_mac[5]);
    
    printf("burn_bt_mac: %02x:%02x:%02x:%02x:%02x:%02x\n", basic_info.bt_mac[0], basic_info.bt_mac[1], basic_info.bt_mac[2],
                                                            basic_info.bt_mac[3], basic_info.bt_mac[4], basic_info.bt_mac[5]);
    printf("sw_version: %s\n", basic_info.sw_version);
    printf("build_time: %s\n", basic_info.build_time);
    printf("mcu_version(main): %s\n", basic_info.mcu_main_version);
    printf("mcu_version(lte): %s\n", basic_info.mcu_lte_version);
    printf("imu_type: %s\n", basic_info.imu_type);
    printf("left_addon: %d(%s)\n", basic_info.addon_type.left_value, basic_info.addon_type.left_name);
    printf("right_addon: %d(%s)\n", basic_info.addon_type.right_value, basic_info.addon_type.right_name);
    printf("sd_status: %d, used/total: (%d/%d)MB, free: %dMB\n", basic_info.sd_card_info.status,
                                                                basic_info.sd_card_info.used_mb, basic_info.sd_card_info.total_mb,
                                                                basic_info.sd_card_info.free_mb);
    printf("wakeup_source: 0x%x(%s)\n", basic_info.wakeup_source.value, basic_info.wakeup_source.name);
    printf("power_source: %d(%s)\n", basic_info.power_source.value, basic_info.power_source.name);
    printf("battery adc: %d, percentage: %d%%, voltage: %.2fV\n",  basic_info.battery_info.adc, basic_info.battery_info.percentage, basic_info.battery_info.voltage_mv * 1.0 / 1000);
    printf("factory_reset: %d\n", basic_info.factory_reset);
    printf("hard_reset: %d\n", basic_info.hard_reset);

    return 0;
}

