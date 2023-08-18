#include "rtos_api_lib.h"
#include <signal.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include <ctype.h>
#include <syslog.h>
#include <stdbool.h>
#include <unistd.h>
#include "debug.h"

static int rtos_api_usage(void)
{
    printf("RTOS API demonstration program\n");
    printf("Commands:\n");
    printf("\t default_setting\n");
    printf("\t format sd|emmc\n");
    printf("\t reboot\n");
    printf("\t audio_volume get|set vol\n");
    printf("\t play_sound start <path>|stop|state\n");
    printf("\t event_record |start <0|1> <pre_seconds> <length>|stop|state\n");
    printf("\t record start|stop <0|1>\n");
    printf("\t audio_record |start <path>|stop\n");
    printf("\t piv [int|ext|all]\n");
    printf("\t sn\n");
    printf("\t fwupdate_flag set|clear|read\n");
    printf("\t ext_cam_state\n");
    printf("\t dump_log <path>\n");
    printf("\t led_reg set <reg> <value>|get <reg>\n");
    printf("\t event_test\n");
    printf("\t soft_reset\n");
    printf("\t factory_reset\n");
    printf("\t usb_line_state\n");
    printf("\t set_button_param <x> <y> <z>\n");
    printf("\t get_addon\n");
    printf("\t image_rotation <0|1> <0|1|2|3>\n");
    printf("\t watermark <0-2>\n");
    printf("\t burn_info\n");
    printf("\t imu_fifo [time_between_iterations_nano]\n");
    printf("\t basic_info\n");
    printf("\t power_off\n");
    printf("\t play_beep_id <id>\n");
    printf("\t led_color set <r> <g> <b>|get\n");
    printf("\t led_rtos_control set <0|1>|get\n");
    printf("\t usb_mass_storage set <0|1>|get\n");
    printf("\t speed_unit set <kph|mph>|get\n");
    printf("\t power_onoff_sound set <0|1>|get\n");
    printf("\t video_encryption_enable set <0|1>|get\n");
    printf("\t video_encryption_key set <0|1>|get\n");
    printf("\t authorization_key set <0|1>|get\n");
    printf("\t auto_record set <0|1|2> <0|1>|get\n");
    printf("\t timer_wakeup set <minutes>|get\n");
    printf("\t mic set <amic|dmic|dmic2>|get\n");
    printf("\t parking_level set <value>|get\n");
    printf("\t lte_wakeup set <0|1>|get\n");
    printf("\t api_files set <0|1>|get\n");
    printf("\t rtos_log_enabled <0|1>\n");
    printf("\t mcu_update\n");
    printf("\t restart_lte_usb_wifi\n");
    printf("\t get_acc_state\n");
    printf("\t cancel_shutdown\n");
    printf("\t lte_power set <0|1>\n");
    printf("\t record_bitrate set <0|1> <value>\n");
    printf("\t preview_bitrate set <0|1> <value>\n");
    printf("\t record_quality_bitrate set <0|1> <quality> <bitrate>\n");
    printf("\t preview_quality_bitrate set <0|1> <quality> <bitrate>\n");
    printf("\t cv set <0|1>|get\n");
    printf("\t shell <cmdline>\n");
    printf("\t power_source\n");
    printf("\t gnss_power <on|off|reset>\n");
    printf("\t hw <wifi|bt|gnss|lte|logo_led|state_led|ir_led|charge|imu> <on|off>\n");
    printf("\t hw <road_camera|cabin_camera> off\n");
    printf("\t imu accel range set 2g|4g|8g|16g\n");
    printf("\t imu gyro range set 125|250|500|1000|2000\n");

    printf("\n");

    return 0;
}

static volatile int signal_should_zero_this = 0;

static void signal_handler(int signal)
{
    if (signal_should_zero_this == 1)
    {
        signal_should_zero_this = 0;
        return;
    }
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        debug_line("Application exit by signal:%d\n", signal);
        rtos_api_lib_event_deinit();
        exit(1);
    }
}

static void capture_all_signal()
{
    int i = 0;
    for(i = 0; i < 32; i ++) {
        if ( (i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM) || (i == SIGPROF)) {
            signal(i, SIG_IGN);
        } else {
            signal(i, signal_handler);
        }
    }
}

static int rtos_api_event_cb(ipc_event_s event)
{
    if (event.event_id == NOTIFY_FILE_CREATED) {
        debug_line("file_created, path: %s, size: %lld time: %d", event.arg.file_arg.path,
                                                                event.arg.file_arg.file_size,
                                                                event.arg.file_arg.file_time);
    } else if (event.event_id == NOTIFY_FILE_DELETED) {
        debug_line("file_deleted, path: %s, size: %lld time: %d", event.arg.file_arg.path,
                                                                event.arg.file_arg.file_size,
                                                                event.arg.file_arg.file_time);
    } else if (event.event_id == NOTIFY_BUTTON_STATE_LONG_PRESS) {
        if (event.param == 0) {
            debug_line("long_press->start. event_length: %d", event.param2);
        } else if (event.param == 1) {
            debug_line("long_press->still_pressed. event_length: %d", event.param2);
        } else if (event.param == 2) {
            debug_line("long_press->release. event_length: %d", event.param2);
        }
    } else if (event.event_id == NOTIFY_BUTTON_STATE_SHORT_PRESS) {
        debug_line("short_presses. taps_count: %d", event.param);
    } else if (event.event_id == NOTIFY_MAIN_MCU_UPDATE) {
        debug_line("main_mcu update: %s", (event.param == 0) ? "start" : "fail");
    } else if (event.event_id == NOTIFY_LTE_MCU_UPDATE) {
        debug_line("lte_mcu update: %s", (event.param == 0) ? "start" : "fail");
    } else if (event.event_id == NOTIFY_ACC_CONNECTED) {
        debug_line("ACC connected");
    } else if (event.event_id == NOTIFY_ACC_DISCONNECTED) {
        debug_line("ACC disconnected");
    } else {
        debug_line("event_id: %d, param1: %d param2: %d", event.event_id, event.param, event.param2);
    }

    return 0;
}

static sem_t main_sem;
int main(int argc, char **argv)
{
    capture_all_signal();

    if (argc >= 2) {
        if (strcmp(argv[1], "default_setting") == 0) {
            rtos_api_lib_default_setting();
        } else if (strcmp(argv[1], "format") == 0) {
            if (strcmp(argv[2], "sd") == 0) {
                rtos_api_lib_format_disk(1);
            } else if (strcmp(argv[2], "emmc") == 0) {
                rtos_api_lib_format_disk(0);
            } else {
                printf("invalid param\n");
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "reboot") == 0) {
            rtos_api_lib_reboot();
        } else if (strcmp(argv[1], "audio_volume") == 0) {
            if(argc < 3) {
                printf("Subcommand required\n");
                rtos_api_usage();
                goto cleanup;
            }
            if (strcmp(argv[2], "set") == 0) {
                if(argc < 4) {
                    printf("usage example: %s audio_volume set 5\n", argv[0]);
                    goto cleanup;
                }
                rtos_api_lib_set_audio_volume(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                unsigned char volume = 0;
                rtos_api_lib_get_audio_volume(&volume);
                printf("cur audio volume: %d\n", volume);
            } else {
                printf("invalid param\n");
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "play_sound") == 0) {
            if(argc < 3) {
                printf("Subcommand required\n");
                rtos_api_usage();
                goto cleanup;
            }
            if (strcmp(argv[2], "start") == 0) {
                rtos_api_lib_play_sound_start(argv[3]);
            } else if (strcmp(argv[2], "stop") == 0) {
                rtos_api_lib_play_sound_stop();
            } else if (strcmp(argv[2], "state") == 0) {
                unsigned char is_playing = 0;
                unsigned int remaining_ms = 0;
                rtos_api_lib_is_sound_playing(&is_playing, &remaining_ms);
                printf("is_sound_playing: %d, remaining_ms=%d\n", is_playing, remaining_ms);
            } else {
                printf("invalid param\n");
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "event_record") == 0) {
            if (strcmp(argv[2], "start") == 0) {
                rtos_api_lib_start_event_record(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), NULL, NULL);
            } else if (strcmp(argv[2], "stop") == 0) {
                rtos_api_lib_stop_event_record(atoi(argv[3]));
            } else if (strcmp(argv[2], "state") == 0) {
                char *path = NULL;
                if (rtos_api_lib_is_event_recording(atoi(argv[3]), &path)) {
                    printf("is_event_recoding: %d, path=%s\n", 1, (path == NULL) ? "" : path);
                } else {
                    printf("is_event_recoding: %d\n", 0);
                }
                if (path) free(path);
            }
        } else if (strcmp(argv[1], "record") == 0) {
            if (strcmp(argv[2], "start") == 0) {
                rtos_api_lib_start_record(atoi(argv[3]));
            } else if (strcmp(argv[2], "stop") == 0) {
                rtos_api_lib_stop_record(atoi(argv[3]));
            }
        } else if (strcmp(argv[1], "audio_record") == 0) {
            if (strcmp(argv[2], "start") == 0) {
                rtos_api_lib_capture_audio_start(argv[3]);
            } else if (strcmp(argv[2], "stop") == 0) {
                rtos_api_lib_capture_audio_stop();
            }
        } else if (strcmp(argv[1], "piv") == 0) {
            //Check whether first option is int, ext or all
            if (strcmp(argv[2], "int") == 0) {
                rtos_api_lib_capture_piv(0, NULL, NULL);
            } else if (strcmp(argv[2], "ext") == 0) {
                rtos_api_lib_capture_piv(1, NULL, NULL);
            } else if (strcmp(argv[2], "all") == 0) {
                rtos_api_lib_capture_piv(2, NULL, NULL);
            } else {
                printf("file saving path required\n");
            }
        } else if (strcmp(argv[1], "sn") == 0) {
            char sn[128] = {0};
            memset(sn, 0, sizeof(sn));
            rtos_api_lib_get_hw_sn(sn);
            printf("hw_sn: %s\n", sn);
        } else if (strcmp(argv[1], "fwupdate_flag") == 0) {
            if(argc < 3) {
                printf("Subcommand required\n");
                rtos_api_usage();
                goto cleanup;
            }
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_fwupdate_flag();
            } else if (strcmp(argv[2], "clear") == 0) {
                rtos_api_lib_clear_fwupdate_flag();
            } else if (strcmp(argv[2], "read") == 0) {
                unsigned char flag = 0;
                rtos_api_lib_get_fwupdate_flag(&flag);
                printf("fwupdate_flag: %d\n", flag);
            } else {
                printf("invalid param\n");
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "ext_cam_state") == 0) {
            int connected = 0;
            rtos_api_lib_check_external_camera_connected(&connected);
            printf("external_camera_connected: %d\n", connected);
        } else if (strcmp(argv[1], "help") == 0) {
            rtos_api_usage();
        } else if (strcmp(argv[1], "dump_log") == 0) {
            if (argv[2] != NULL) {
                rtos_api_lib_dump_rtos_log(argv[2]);
            } else {
                printf("log path required\n");
            }
        } else if (strcmp(argv[1], "led_reg") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                unsigned char reg = atoi(argv[3]) & 0xFF;
                unsigned char value = atoi(argv[4]) & 0xFF;
                rtos_api_lib_set_led_reg_value(reg, value);
            } else if (strcmp(argv[2], "get") == 0) {
                unsigned char reg = atoi(argv[3]) & 0xFF;
                unsigned char value = 0;
                rtos_api_lib_get_led_reg_value(reg, &value);
                printf("led reg: 0x%.2X=0x%.2X\n", reg, value);
            } else {
                printf("invalid param\n");
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "event_test") == 0) {
            rtos_api_lib_register_event_cb(rtos_api_event_cb);
            rtos_api_lib_event_init();
            //wait
            sem_init(&main_sem, 0, 0);
            sem_wait(&main_sem);
            sem_destroy(&main_sem);
        } else if (strcmp(argv[1], "soft_reset") == 0) {
            rtos_api_lib_soft_reset();
        } else if (strcmp(argv[1], "factory_reset") == 0) {
            rtos_api_lib_factory_reset();
        } else if (strcmp(argv[1], "usb_line_state") == 0) {
            printf("usb line state: %d", rtos_api_lib_get_usb_line_connected());
        } else if (strcmp(argv[1], "set_button_param") == 0) {
            rtos_api_lib_set_button_param(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        } else if (strcmp(argv[1], "get_addon") == 0) {
            ADDON_TYPE_e left_type, right_type;
            rtos_api_lib_get_addon_type(&left_type, &right_type);
            printf("addon left: %d, right: %d\n", left_type, right_type);
        } else if (strcmp(argv[1], "image_rotation") == 0) {
            rtos_api_lib_set_image_rotation(atoi(argv[2]) ? 1 : 0, atoi(argv[3]));
        } else if (strcmp(argv[1], "watermark") == 0) {
            rtos_api_lib_set_watermark(atoi(argv[2]));
        } else if (strcmp(argv[1], "rtos_log_enabled") == 0) {
            int enable = atoi(argv[2]);
            system(enable ? "killall rtos_log;sleep 1;rtos_log&" : "killall rtos_log;");
            rtos_api_lib_set_rtos_log_enable(atoi(argv[2]));
        } else if (strcmp(argv[1], "burn_info") == 0) {
            unsigned char wifi_mac[6] = {0};
            unsigned char bt_mac[6] = {0};
            rtos_api_lib_get_burn_info(wifi_mac, bt_mac);
            printf("wifi_mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", wifi_mac[0], wifi_mac[1], wifi_mac[2], wifi_mac[3], wifi_mac[4], wifi_mac[5]);
            printf("  bt_mac: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);
        } else if (strcmp(argv[1], "imu_fifo") == 0) {
            mmapInfo_s map;
            stream_share_info_s share_info;
            imu_share_frame_data_s *fifo;
            int frame_num = 0;
            rtos_api_lib_get_imu_fifo_share_info(&share_info);
            fifo = (imu_share_frame_data_s *)rtos_api_lib_convert_memory2linux(share_info, &map);
            //in your app, you can loop call rtos_api_lib_get_imu_fifo_data
            //0-3: seconds
            //4-7: useconds
            //8-11: sensor time
            //12-13: accel x [high low]
            //14-15: accel y [high low]
            //16-17: accel z [high low]
            //18-19: gyro x [high low]
            //20-21: gyro y [high low]
            //22-23: gyro z [high low]
            //24-25: temp [high low]
            unsigned int sleep_between = 0;
            if (argc == 3)
            {
                signal_should_zero_this = 1;
                sleep_between = atoi(argv[2]);
            }
            while (signal_should_zero_this != 0)
            {
                rtos_api_lib_get_imu_fifo_data(200, &frame_num);
                printf("frame_num: %d\n", frame_num);
                if (frame_num > 0) {
                    int i = 0, j = 0;
                    for (i = 0; i < frame_num; i++) {
                        char buf[256] = {0};
                        memset(buf, 0, sizeof(buf));
                        for (j = 0; j < 32; j++) {
                            snprintf(buf + strlen(buf), 4, "%.2X ", fifo[i].data[j]);
                        }
                        printf("%.3d: %s\n", i, buf);
                    }
                }
                if (sleep_between != 0)
                {
                    nanosleep((const struct timespec[]){{0, sleep_between}}, NULL);
                }
            }
            //when your app exit, you need call rtos_api_lib_munmap
            rtos_api_lib_munmap(&map);
        } else if (strcmp(argv[1], "basic_info") == 0) {
            basic_info_s basic_info;
            rtos_api_lib_get_basic_info(&basic_info);
            printf("model: %s\n", basic_info.model);
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
            printf("battery adc: %d, raw_adc: %d mcu_adc: %d, cable_adc: %d, percentage: %d%%, voltage: %.2fV, is_charge: %d, power_good: %d, thermal_protect: %d, imu_temp: %.2f\n",
                                        basic_info.battery_info.adc, basic_info.battery_info.raw_adc, basic_info.battery_info.mcu_adc, basic_info.battery_info.cable_adc,
                                        basic_info.battery_info.percentage, basic_info.battery_info.voltage_mv * 1.0 / 1000,
                                        basic_info.battery_info.charger_ic_is_charge, basic_info.battery_info.charger_ic_power_good, basic_info.battery_info.charger_ic_thermal_protect,
                                        basic_info.battery_info.imu_temperature);
            printf("factory_reset: %d\n", basic_info.factory_reset);
            printf("hard_reset: %d\n", basic_info.hard_reset);
            printf("acc_state: %d\n", basic_info.acc_state);
        }  else if (strcmp(argv[1], "power_off") == 0) {
            rtos_api_lib_power_off();
        } else if (strcmp(argv[1], "play_beep_id") == 0) {
            rtos_api_lib_play_beep_id(atoi(argv[2]));
        } else if (strcmp(argv[1], "led_color") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                color_s led_color;
                led_color.R = atoi(argv[3]) & 0xff;
                led_color.G = atoi(argv[4]) & 0xff;
                led_color.B = atoi(argv[5]) & 0xff;
                rtos_api_lib_set_led_color(led_color);
            } else if (strcmp(argv[2], "get") == 0) {
                color_s led_color;
                rtos_api_lib_get_led_color(&led_color);
                printf("led color: %d %d %d (#%02X%02X%02X)\n", led_color.R, led_color.G, led_color.B,
                                                            led_color.R, led_color.G, led_color.B);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "led_rtos_control") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_led_rtos_control_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_led_rtos_control_enable(&enable);
                printf("led_rtos_control_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "usb_mass_storage") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_usb_mass_storage_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_usb_mass_storage_enable(&enable);
                printf("usb_mass_storage_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "speed_unit") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                if (strcmp(argv[3], "kph") == 0) {
                    rtos_api_lib_set_speed_unit(SPEED_UNIT_KMH);
                } else if (strcmp(argv[3], "mph") == 0) {
                    rtos_api_lib_set_speed_unit(SPEED_UNIT_MPH);
                } else {
                    rtos_api_usage();
                    goto cleanup;
                }
            } else if (strcmp(argv[2], "get") == 0) {
                SPEED_UNIT_e speed_unit;
                rtos_api_lib_get_speed_unit(&speed_unit);
                printf("speed_unit: %s\n", (speed_unit == SPEED_UNIT_KMH) ? "kph" : "mph");
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "power_onoff_sound") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_power_onoff_sound_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {            
                int enable = 0;
                rtos_api_lib_get_power_onoff_sound_enable(&enable);
                printf("power_onoff_sound_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "video_encryption_enable") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_video_encryption_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_video_encryption_enable(&enable);
                printf("video_encryption_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "video_encryption_key") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_video_encryption_key(argv[3]);
            } else if (strcmp(argv[2], "get") == 0) {
                char key[64] = {0};
                memset(key, 0, sizeof(key));
                rtos_api_lib_get_video_encryption_key(key);
                printf("video_encryption_key: %s\n", key);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "authorization_key") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_authorization_key(argv[3]);
            } else if (strcmp(argv[2], "get") == 0) {
                char key[64] = {0};
                memset(key, 0, sizeof(key));
                rtos_api_lib_get_authorization_key(key);
                printf("authorization_key: %s\n", key);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "auto_record") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_camera_boot_auto_record(atoi(argv[3]), atoi(argv[4]));
            } else if (strcmp(argv[2], "get") == 0) {
                int auto_record1 = 0, auto_record2 = 0;
                rtos_api_lib_get_camera_boot_auto_record(&auto_record1, &auto_record2);
                printf("auto record: internal(%d) external(%d)\n", auto_record1, auto_record2);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "timer_wakeup") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_mcu_timer_wakeup_time(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int minutes = 0;
                rtos_api_lib_get_mcu_timer_wakeup_time(&minutes);
                printf("mcu_wakeup_time: %dminutes\n", minutes);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "mic") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                if (strcmp(argv[3], "amic") == 0) {
                    rtos_api_lib_set_mic(MIC_TYPE_AMIC);
                } else if (strcmp(argv[3], "dmic") == 0) {
                    rtos_api_lib_set_mic(MIC_TYPE_DMIC);
                } else if (strcmp(argv[3], "dmic2") == 0) {
                    rtos_api_lib_set_mic(MIC_TYPE_DMIC2);
                } else {
                    rtos_api_usage();
                    goto cleanup;
                }
            } else if (strcmp(argv[2], "get") == 0) {
                MIC_TYPE_e mic_type;
                rtos_api_lib_get_mic(&mic_type);
                if (mic_type == MIC_TYPE_AMIC) {
                    printf("mic_type: amic\n");
                } else if (mic_type == MIC_TYPE_DMIC) {
                    printf("mic_type: dmic\n");
                } else if (mic_type == MIC_TYPE_DMIC2) {
                    printf("mic_type: dmic2\n");
                } else {
                    printf("mic_type: unknown\n");
                }
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "parking_level") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_parking_level(atoi(argv[3]) & 0xff);
            } else if (strcmp(argv[2], "get") == 0) {
                unsigned char value = 0;
                rtos_api_lib_get_parking_level(&value);
                printf("parking_level: %d\n", value);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "lte_wakeup") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_lte_wakeup_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_lte_wakeup_enable(&enable);
                printf("lte_wakeup_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "api_files") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_api_files_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_api_files_enable(&enable);
                printf("api_files_enable: %d\n", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "mcu_update") == 0) {
            rtos_api_lib_mcu_update();
        } else if (strcmp(argv[1], "restart_lte_usb_wifi") == 0) {
            rtos_api_lib_restart_lte_usb_wifi();
        } else if (strcmp(argv[1], "get_acc_state") == 0) {
            unsigned char state = 0;
            rtos_api_lib_get_acc_state(&state);
            printf("acc_state: %d\n", state);
        } else if (strcmp(argv[1], "cancel_shutdown") == 0) {
            rtos_api_lib_cancel_shutdown();
        } else if (strcmp(argv[1], "lte_power") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_lte_power(atoi(argv[3]));
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "record_bitrate") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_record_bitrate(atoi(argv[3]), atoi(argv[4]));
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "preview_bitrate") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_preview_bitrate(atoi(argv[3]), atoi(argv[4]));
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "record_quality_bitrate") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_record_quality_and_bitrate(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "preview_quality_bitrate") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_preview_quality_and_bitrate(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "cv") == 0) {
            if (strcmp(argv[2], "set") == 0) {
                rtos_api_lib_set_cv_enable(atoi(argv[3]));
            } else if (strcmp(argv[2], "get") == 0) {
                int enable = 0;
                rtos_api_lib_get_cv_enable(&enable);
                printf("cv enable: %d", enable);
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "shell") == 0) {
            rtos_api_lib_exec_shell_command(argv[2]);
        } else if (strcmp(argv[1], "power_source") == 0) {
            basic_info_s basic_info;
            rtos_api_lib_get_battery_info(&basic_info);
            printf("power_source: %d(%s)\n", basic_info.power_source.value, basic_info.power_source.name);
        } else if (strcmp(argv[1], "gnss_power") == 0) {
            if (strcmp(argv[2], "on") == 0) {
                rtos_api_lib_gnss_power_on();
            } else if (strcmp(argv[2], "off") == 0) {
                rtos_api_lib_gnss_power_off();
            } else if (strcmp(argv[2], "reset") == 0) {
                rtos_api_lib_gnss_reset();
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "hw") == 0) {
            if (strcmp(argv[2], "wifi") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    system("/usr/local/share/script/wifi_start.sh");
                } else if (strcmp(argv[3], "off") == 0) {
                    system("/usr/local/share/script/wifi_stop.sh");
                }
            } else if (strcmp(argv[2], "bt") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_bt_power(1);
                    system("/usr/local/share/script/bt_start.sh");
                } else if (strcmp(argv[3], "off") == 0) {
                    system("/usr/local/share/script/bt_stop.sh");
                    rtos_api_lib_set_bt_power(0);
                }
            } else if (strcmp(argv[2], "gnss") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_gnss_power_off();
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_gnss_power_on();
                }
            } else if (strcmp(argv[2], "lte") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_lte_power(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_lte_power(0);
                }
            } else if (strcmp(argv[2], "logo_led") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_logo_led_power(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_logo_led_power(0);
                }
            } else if (strcmp(argv[2], "state_led") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_state_led_power(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_state_led_power(0);
                }
            } else if (strcmp(argv[2], "ir_led") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_ir_led_power(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_ir_led_power(0);
                }
            } else if (strcmp(argv[2], "charge") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_charge_enable(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_charge_enable(0);
                }
            } else if (strcmp(argv[2], "imu") == 0) {
                if (strcmp(argv[3], "on") == 0) {
                    rtos_api_lib_set_imu_enable(1);
                } else if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_imu_enable(0);
                }
            } else if (strcmp(argv[2], "road_camera") == 0) {
                if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_road_camera_enable(0);
                }
            } else if (strcmp(argv[2], "cabin_camera") == 0) {
                if (strcmp(argv[3], "off") == 0) {
                    rtos_api_lib_set_cabin_camera_enable(0);
                }
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else if (strcmp(argv[1], "imu") == 0) {
            if (strcmp(argv[2], "accel") == 0) {
                if (strcmp(argv[3], "range") == 0) {
                    if (strcmp(argv[4], "set") == 0) {
                        if (strcmp(argv[5], "2g") == 0) {
                            rtos_api_lib_set_accel_range(IMU_ACCEL_RANGE_2G);
                        } else if (strcmp(argv[5], "4g") == 0) {
                            rtos_api_lib_set_accel_range(IMU_ACCEL_RANGE_4G);
                        } else if (strcmp(argv[5], "8g") == 0) {
                            rtos_api_lib_set_accel_range(IMU_ACCEL_RANGE_8G);
                        } else if (strcmp(argv[5], "16g") == 0) {
                            rtos_api_lib_set_accel_range(IMU_ACCEL_RANGE_16G);
                        } else {
                            rtos_api_usage();
                            goto cleanup;
                        }
                    } else {
                        rtos_api_usage();
                        goto cleanup;
                    }
                } else {
                    rtos_api_usage();
                    goto cleanup;
                }
            } else if (strcmp(argv[2], "gyro") == 0) {
                if (strcmp(argv[3], "range") == 0) {
                    if (strcmp(argv[4], "set") == 0) {
                        if (strcmp(argv[5], "125") == 0) {
                            rtos_api_lib_set_gyro_range(IMU_GYRO_RANGE_125DPS);
                        } else if (strcmp(argv[5], "250") == 0) {
                            rtos_api_lib_set_gyro_range(IMU_GYRO_RANGE_250DPS);
                        } else if (strcmp(argv[5], "500") == 0) {
                            rtos_api_lib_set_gyro_range(IMU_GYRO_RANGE_500DPS);
                        } else if (strcmp(argv[5], "1000") == 0) {
                            rtos_api_lib_set_gyro_range(IMU_GYRO_RANGE_1000DPS);
                        } else if (strcmp(argv[5], "2000") == 0) {
                            rtos_api_lib_set_gyro_range(IMU_GYRO_RANGE_2000DPS);
                        } else {
                            rtos_api_usage();
                            goto cleanup;
                        }
                    } else {
                        rtos_api_usage();
                        goto cleanup;
                    }
                } else {
                    rtos_api_usage();
                    goto cleanup;
                }
            } else {
                rtos_api_usage();
                goto cleanup;
            }
        } else {
            rtos_api_usage();
            goto cleanup;
        }
    } else {
        rtos_api_usage();
    }
cleanup:
    return 0;
}
