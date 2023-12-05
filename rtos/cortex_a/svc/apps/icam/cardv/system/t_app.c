#include <app_base.h>

static void t_app_cmd_handler(unsigned int argc, char * const *argv, AMBA_SHELL_PRINT_f func);
static int show_camera_status(CAMERA_CHANNEL_e channel);

void t_app_init(void)
{
    AMBA_SHELL_COMMAND_s  Cmd;
    unsigned int ret = 0;

    Cmd.pName    = "t";
    Cmd.MainFunc = t_app_cmd_handler;
    Cmd.pNext    = NULL;
    ret = SvcCmd_CommandRegister(&Cmd);
    if (SHELL_ERR_SUCCESS != ret) {
        debug_line("%s: fail to register cmd", __func__);
    }
}

static void print_hex(unsigned char *cmd, int len)
{
    char tmp[64] = {0};
    int i = 0, j  = 0, num = 0;
    int lines = 0;

    lines = len / 16;
    if (len % 16 != 0) {
        lines += 1;
    }
    for (i = 0; i < lines; i++) {
        num = ((len - i * 16) >= 16) ? 16 : (len % 16);
        memset(tmp, 0, sizeof(tmp));
        for (j = 0; j < num; j++) {
            snprintf(tmp + j * 3, 4, "%.2X ", *(cmd + i * 16 + j));
        }
        debug_line("%s", tmp);
    }
}

static void pwm_debug(unsigned int base_clock, unsigned int period, int on)
{
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    (void)base_clock;
    (void)period;
    (void)on;
#else
    AmbaGPIO_SetFuncAlt(GPIO_PIN_44_PWM0);
    AmbaPWM_Config(AMBA_PWM_CHANNEL0, base_clock);
    AmbaPWM_Start(AMBA_PWM_CHANNEL0, period, on);
#endif
}

static UINT32 RefOD_PyramidDataRdyHandler(const void *pEventInfo)
{
    AMBA_DSP_PYMD_DATA_RDY_s *pYuvRdy;
    unsigned int i = 0;

    AmbaMisra_TypeCast32(&pYuvRdy, &pEventInfo);
    debug_line("\nFOV%d, CapSequence=%d", pYuvRdy->ViewZoneId, pYuvRdy->CapSequence);
    for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
        debug_line("YUV[%d]", i);
        debug_line("    pitch:%d", pYuvRdy->YuvBuf[i].Pitch);
        debug_line("    offset_x:%d", pYuvRdy->YuvBuf[i].Window.OffsetX);
        debug_line("    offset_y:%d", pYuvRdy->YuvBuf[i].Window.OffsetY);
        debug_line("    width:%d", pYuvRdy->YuvBuf[i].Window.Width);
        debug_line("    height:%d", pYuvRdy->YuvBuf[i].Window.Height);
    }

    return SVC_OK;
}

static void t_app_cmd_handler(unsigned int argc, char * const *argv, AMBA_SHELL_PRINT_f func)
{
    char output[1024] = {0};

    if (argc <= 1) {
        (*func)("invalid cmd");
        return;
    }

    memset(output, 0, sizeof(output));
    if (strcmp(argv[1], "pmic") == 0) {
        if (strcmp(argv[2], "r") == 0) {
            unsigned char reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned char value = 0;
            Pmic_Read(reg, &value);
            snprintf(output, sizeof(output) - 1, "Pmic: 0x%.2X=0x%.2X\n", reg, value);
            (*func)(output);
        } else if (strcmp(argv[2], "w") == 0) {
            unsigned char reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned char value = strtoul(argv[4], NULL, 16) & 0xFF;
            Pmic_Write(reg, value);
        } else if (strcmp(argv[2], "reboot") == 0) {
            Pmic_SoftReset();
        } else if (strcmp(argv[2], "power_off") == 0) {
            Pmic_DoPowerOff(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "power_off") == 0) {
        app_msg_queue_send(APP_MSG_ID_POWER_BUTTON, 0, 0, 0);
    }
#if defined(CONFIG_ENABLE_GYRO)
    else if (strcmp(argv[1], "gyro") == 0) {
        if (strcmp(argv[2], "r") == 0) {
            unsigned char reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned char value = 0;
            Gyro_Read(reg, &value);
            snprintf(output, sizeof(output) - 1, "Gyro: 0x%.2X=0x%.2X\n", reg, value);
            (*func)(output);
        } else if (strcmp(argv[2], "w") == 0) {
            unsigned char reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned char value = strtoul(argv[4], NULL, 16) & 0xFF;
            Gyro_Write(reg, value);
        } else if (strcmp(argv[2], "debug") == 0) {
            gyro_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "start") == 0) {
            gyro_task_start();
            gyro_task_set_debug(1);
        } else if (strcmp(argv[2], "stop") == 0) {
            gyro_task_stop();
        }
    }
#endif
    else if (strcmp(argv[1], "linux") == 0) {
        if (strcmp(argv[2], "exec") == 0) {
            app_helper.linux_system(argv[3]);
        } else if (strcmp(argv[2], "exec2") == 0) {
            char response[1024] = {0};
            memset(response, 0, sizeof(response));
            app_helper.linux_system_with_response(argv[3], response);
            debug_line("response:[%s]", response);
        } else if (strcmp(argv[2], "devid") == 0) {
            char cmd[512] = {0};
            char response[1024] = {0};
            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd) - 1, "devid %s", argv[3]);
            memset(response, 0, sizeof(response));
            app_helper.linux_system_with_response(cmd, response);
            debug_line("response:[%s]", response);
        }
    }
#if defined(CONFIG_ENABLE_THERMAL)
    else if (strcmp(argv[1], "thermal") == 0) {
        if (strcmp(argv[2], "debug") == 0) {
            thermal_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "read") == 0) {
            THERMAL_DATA_s data;
            Thermal_GetData(&data);
            debug_line("current thermal data: %d", data.value);
        }
    }
#endif
    else if (strcmp(argv[1], "beep") == 0) {
        if (strcmp(argv[2], "id") == 0) {
            beep_play(atoi(argv[3]));
        } else if (strcmp(argv[2], "file") == 0) {
            beep_file(argv[3]);
        } else if (strcmp(argv[2], "check") == 0) {
            debug_line("beep play state: %d", beep_is_playing());
        } else if (strcmp(argv[2], "stop") == 0) {
            beep_stop();
        }
    } else if (strcmp(argv[1], "magic") == 0) {
        if (strcmp(argv[2], "set") == 0) {
            //AmbaNVM_SetBldMagicCode(NVM_TYPE, 5000);
        } else if (strcmp(argv[2], "clear") == 0) {
            //AmbaNVM_EraseBldMagicCode(NVM_TYPE, 5000);
        } else if (strcmp(argv[2], "read") == 0) {
            //debug_line("magic flag: %d", AmbaNVM_IsBldMagicCodeSet(NVM_TYPE));
        }
    } else if (strcmp(argv[1], "audio") == 0) {
        if (strcmp(argv[2], "r") == 0) {
            unsigned int reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned int value = 0;
            AmbaAudio_CodecRead(0, reg, &value);
            snprintf(output, sizeof(output) - 1, "Audio: 0x%.2X=0x%.2X\n", reg, value);
            (*func)(output);
        } else if (strcmp(argv[2], "w") == 0) {
            unsigned int reg = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned int value = strtoul(argv[4], NULL, 16) & 0xFF;
            AmbaAudio_CodecWrite(0, reg, value);
        } else if (strcmp(argv[2], "dump") == 0) {
            //unsigned int reg = 0, value = 0;
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output) - 1, "Audio Reg List:\n");
            (*func)(output);
            /*for (reg = AK4951_00_POWER_MANAGEMENT1; reg < AK4951_REG_NUM; reg++) {
                AmbaAudio_CodecRead(0, reg, &value);
                memset(output, 0, sizeof(output));
                snprintf(output, sizeof(output) - 1, "0x%.2X=0x%.2X\n", reg, value);
                (*func)(output);
            }*/
        } else if (strcmp(argv[2], "mute") == 0) {
            AmbaAudioEnc_SetMute(atoi(argv[3]) & 0xFF);
        } else if (strcmp(argv[2], "amic") == 0) {
            AmbaAudio_CodecSetInput(0, AUCODEC_AMIC_IN);
            debug_line("set audio to amic");
        } else if (strcmp(argv[2], "dmic") == 0) {
            AmbaAudio_CodecSetInput(0, AUCODEC_DMIC_IN);            
            debug_line("set audio to dmic");
        } else if (strcmp(argv[2], "dmic2") == 0) {
            AmbaAudio_CodecSetInput(0, AUCODEC_DMIC2_IN);            
            debug_line("set audio to dmic2");
        }
    } else if (strcmp(argv[1], "metadata") == 0) {
        if (strcmp(argv[2], "load") == 0) {
            metadata_load();
        } else if (strcmp(argv[2], "dump") == 0) {
            metadata_dump();
        }
    } else if (strcmp(argv[1], "linux") == 0) {
        if (strcmp(argv[2], "cmd") == 0) {
            char response[2048] = {0};
            memset(response, 0, sizeof(response));
            app_helper.linux_system_with_response(argv[3], response);
            debug_line(response);
        }
    } else if (strcmp(argv[1], "piv") == 0) {
        if (strcmp(argv[2], "raw") == 0) {
            app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_PIV_RAW, atoi(argv[3]), 0, 0);
            return;
        }
        app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_PIV_MJPG, atoi(argv[2]), 0, 0);
    } else if (strcmp(argv[1], "thm") == 0) {
        app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_THM, atoi(argv[2]), 0, 0);
    } else if (strcmp(argv[1], "event_record") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            app_msg_queue_send(APP_MSG_ID_CAM_START_EVENT_RECORD, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        } else if (strcmp(argv[2], "stop") == 0) {
            app_msg_queue_send(APP_MSG_ID_CAM_STOP_EVENT_RECORD, atoi(argv[3]), 0, 0);
        }
    } else if (strcmp(argv[1], "record") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            if (strcmp(argv[3], "internal") == 0) {
                app_helper.internal_camera_auto_record = 1;
                rec_dvr_record_start(CAMERA_CHANNEL_INTERNAL);
            } else if (strcmp(argv[3], "external") == 0) {
                app_helper.external_camera_auto_record = 1;
                rec_dvr_record_start(CAMERA_CHANNEL_EXTERNAL);
            } else if (strcmp(argv[3], "all") == 0) {
                app_helper.internal_camera_auto_record = 1;
                app_helper.external_camera_auto_record = 1;
                rec_dvr_record_start(CAMERA_CHANNEL_BOTH);
            }
        } else if (strcmp(argv[2], "stop") == 0) {
            if (strcmp(argv[3], "internal") == 0) {
                app_helper.internal_camera_auto_record = 0;
                rec_dvr_record_stop(CAMERA_CHANNEL_INTERNAL, NULL);
            } else if (strcmp(argv[3], "external") == 0) {
                app_helper.external_camera_auto_record = 0;
                rec_dvr_record_stop(CAMERA_CHANNEL_EXTERNAL, NULL);
            } else if (strcmp(argv[3], "all") == 0) {
                app_helper.internal_camera_auto_record = 0;
                app_helper.external_camera_auto_record = 0;
                rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, NULL);
            }
        }
    } else if (strcmp(argv[1], "format_sd") == 0) {
        app_msg_queue_send(APP_MSG_ID_FORMAT_SD, 1, 0, 0);
    } else if (strcmp(argv[1], "default_setting") == 0) {
        app_msg_queue_send(APP_MSG_ID_DEFAULT_SETTING, 0, 0, 0);
    } else if (strcmp(argv[1], "pcm_record") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            pcm_record_set_path(argv[3]);
            pcm_record_start();
        } else if (strcmp(argv[2], "stop") == 0) {
            pcm_record_stop();
        } else if (strcmp(argv[2], "test") == 0) {
            remove(REC_DEST_DISK":\\audio_record.pcm");
            pcm_record_set_path(REC_DEST_DISK":\\audio_record.pcm");
            pcm_record_start();
            sleep(5);
            pcm_record_stop();
            while (pcm_record_is_busy()) msleep(100);
            beep_file(REC_DEST_DISK":\\audio_record.pcm");
        }
    } else if (strcmp(argv[1], "aac_record") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            aac_record_set_path(argv[3]);
            aac_record_start();
        } else if (strcmp(argv[2], "stop") == 0) {
            aac_record_stop();
        } else if (strcmp(argv[2], "test") == 0) {
            remove(REC_DEST_DISK":\\audio_record.aac");
            aac_record_set_path(REC_DEST_DISK":\\audio_record.aac");
            aac_record_start();
            sleep(5);
            aac_record_stop();
            while (aac_record_is_busy()) msleep(100);
            beep_file(REC_DEST_DISK":\\audio_record.aac");
        }
    } else if (strcmp(argv[1], "led") == 0) {
        if (strcmp(argv[2], "lock") == 0) {
            app_helper.lock_led(1);
        } else if (strcmp(argv[2], "unlock") == 0) {
            app_helper.lock_led(0);
        } else if (strcmp(argv[2], "set_color") == 0) {
            unsigned char r = strtoul(argv[3], NULL, 16) & 0xFF;
            unsigned char g = strtoul(argv[4], NULL, 16) & 0xFF;
            unsigned char b = strtoul(argv[5], NULL, 16) & 0xFF;
            app_helper.lock_led(0);
            app_helper.set_led_color(r, g, b);
            app_helper.lock_led(1);
        } else if (strcmp(argv[2], "set_brightness") == 0) {
            app_helper.set_led_brihtness(atoi(argv[3]) & 0xFF);
        } else if (strcmp(argv[2], "set_fade") == 0) {
            app_helper.set_led_fade(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "time") == 0) {
        if (strcmp(argv[2], "test") == 0) {
            time_s cur_time;
            AmbaRTSL_RtcSetSystemTime(0);
            debug_line("timestamp=%d", AmbaRTSL_RtcGetSystemTime());
            time_now(&cur_time);
            debug_line("timestr=%.4d-%.2d-%.2d %.2d:%.2d:%.2d", cur_time.year,
                                                          cur_time.month,
                                                          cur_time.day,
                                                          cur_time.hour,
                                                          cur_time.minute,
                                                          cur_time.second);
        }
    } else if (strcmp(argv[1], "ir") == 0) {
        if (strcmp(argv[2], "on") == 0) {
            app_helper.set_ir_brightness(100);
        } else if (strcmp(argv[2], "off") == 0) {
            app_helper.set_ir_brightness(0);
        } else if (strcmp(argv[2], "brightness") == 0) {
            app_helper.set_ir_brightness(atoi(argv[3]));
        } else if (strcmp(argv[2], "debug") == 0) {
            ir_task_set_debug(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "all") == 0) {
        device_info_s *info = app_helper.get_device_info();
        metadata_s *metadata = NULL;
        time_s cur_time;
        char ip[32] = {0};
        char ip2[32] = {0};
        unsigned char lte_ready = 0;
        SVC_USER_PREF_s *pSvcUserPref = NULL;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) || defined(CONFIG_BSP_CV25_NEXAR_D080)
        unsigned char value = 0;
#endif
        beep_play(BEEP_ID_EVENT_RECORD);
        SvcUserPref_Get(&pSvcUserPref);
        metadata_get(&metadata);
        time_now(&cur_time);

        debug_line("\r\n");
#if defined(CONFIG_BSP_CV25_NEXAR_D161)
        debug_line("model: N1V1");
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        debug_line("model: N1V2");
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
        if (app_helper.check_d080_with_2nd_sensor()) {
            debug_line("model: Beam2K+");
        } else {
            debug_line("model: Beam2K");
        }
#endif
#if defined(CONFIG_BSP_H32_NEXAR_D081)
        debug_line("model: N0");
#endif
#ifdef CONFIG_PCBA_DVT
        debug_line("fw: dvt");
#else
        debug_line("fw: evt");
#endif
        debug_line("current_time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d(UTC)", cur_time.year,
                                                      cur_time.month,
                                                      cur_time.day,
                                                      cur_time.hour,
                                                      cur_time.minute,
                                                      cur_time.second);
        debug_line("cpu_id: %s", (char *)metadata->CpuId);
        debug_line("device_sn: %s", (char *)metadata->SerialNumber);
        debug_line("vsn: %s", info->vsn);
        debug_line("trip_key: %s", info->trip_key);
#if defined(CONFIG_APP_FIRMWARE_FOR_MP)
        debug_line("sw_type: %s", "mp");
#else
        debug_line("sw_type: %s", "dev");
#endif
        debug_line("boot_part: %s", AmbaLink_GetBootFromA() ? "A" : "B");
        debug_line("sw_version: %s", info->sw_version);
        debug_line("build_time: %s", info->build_time);
        debug_line("mcu_version(main): %s", info->mcu_version);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        debug_line("mcu_version(lte): %s", info->mcu_version2);
#endif
        debug_line("wakeup_source: %s", app_helper.wakeup_source_2_str(Pmic_GetBootReason()));
        debug_line("power_source: %s", app_helper.power_source_2_str(app_helper.get_power_source()));
        debug_line("hard_reset: %d", app_helper.hard_reset_flag);
        debug_line("factory_reset: %d", app_helper.factory_reset_flag);
        debug_line("battery_adc: %d", battery_task_get_adc());
        debug_line("brightness_adc: %d", ir_task_get_brightness());
        debug_line("button_state: %s", app_helper.gpio_get(GPIO_PIN_21) ? "release" : "pressed");
        debug_line("power_status: %d", Pmic_CheckUsbConnected());
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) || defined(CONFIG_BSP_CV25_NEXAR_D080)
        i2c_read_reg(CHARGER_I2C_CHANNEL, CHARGER_I2C_SLAVE_ADDR, 0x00, &value);
        debug_line("charger_ic: %s", (value != 0xff) ? "ok" : "fail");
        i2c_read_reg(BATTERY_METER_I2C_CHANNEL, BATTERY_METER_I2C_SLAVE_ADDR, 0x00, &value);
        debug_line("battery_meter: %s", (value != 0xff) ? "ok" : "fail");
#endif
        if (AmbaSD_IsCardPresent(SD_CHANNEL)) {
            if (sd_bad_or_not_exist) {
                debug_line("sd_status: bad");
            } else {
                unsigned int free = 0, total = 0;
                app_helper.get_sd_space(&total, &free);
                debug_line("sd_status: %d/%d(MB), free:%dMB", total - free, total, free);
            }
        } else {
            debug_line("sd_status: no_card");
        }
        debug_line("imu: %s", Imu_GetName());
#if defined(CONFIG_SENSOR_JX_K351P)
        debug_line("k315p irq: %d, record: %dx%d, preview: %dx%d", AmbaDiag_GetIrqCounter(143),
                                            pSvcUserPref->InternalCamera.MainStream.width, pSvcUserPref->InternalCamera.MainStream.height,
                                            pSvcUserPref->InternalCamera.SecStream.width, pSvcUserPref->InternalCamera.SecStream.height);
#endif
#if defined(CONFIG_SENSOR_SONY_IMX415_MIPI)
        debug_line("imx415 irq: %d, record: %dx%d, preview: %dx%d", AmbaDiag_GetIrqCounter(143),
                                            pSvcUserPref->InternalCamera.MainStream.width, pSvcUserPref->InternalCamera.MainStream.height,
                                            pSvcUserPref->InternalCamera.SecStream.width, pSvcUserPref->InternalCamera.SecStream.height);
#endif
#if defined(CONFIG_SENSOR_GC4653)
        debug_line("gc4653 irq: %d, record: %dx%d, preview: %dx%d", AmbaDiag_GetIrqCounter(143),
                                            pSvcUserPref->InternalCamera.MainStream.width, pSvcUserPref->InternalCamera.MainStream.height,
                                            pSvcUserPref->InternalCamera.SecStream.width, pSvcUserPref->InternalCamera.SecStream.height);
#endif
#if defined(CONFIG_SENSOR_GC2053)
        debug_line("gc2053 irq: %d, record: %dx%d, preview: %dx%d", AmbaDiag_GetIrqCounter(168),
                                            pSvcUserPref->ExternalCamera.MainStream.width, pSvcUserPref->ExternalCamera.MainStream.height,
                                            pSvcUserPref->ExternalCamera.SecStream.width, pSvcUserPref->ExternalCamera.SecStream.height);
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
        debug_line("gc2053 connect: %d", app_helper.check_external_camera_connected());
#endif
        debug_line("imu_record: %d", pSvcUserPref->ImuRecord);
        debug_line("gnss_record: %d", pSvcUserPref->GnssRecord);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (app_helper.check_usb_left_connected()) {
            debug_line("left_port: %s", "usb_line");
        } else if (app_helper.check_gc2053_left_connected()) {
            debug_line("left_port: %s", "sensor_board");
        } else if (app_helper.check_lte_left_connected()) {
            debug_line("left_port: %s", "lte_board");
        } else {
            debug_line("left_port: %s", "none");
        }
        if (app_helper.check_usb_right_connected()) {
            debug_line("right_port: %s", "usb_line");
        } else if (app_helper.check_gc2053_right_connected()) {
            debug_line("right_port: %s", "sensor_board");
        } else if (app_helper.check_lte_right_connected()) {
            debug_line("right_port: %s", "lte_board");
        } else {
            debug_line("right_port: %s", "none");
        }
        i2c_read_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x02, &value);
        debug_line("led_ic_otp: %d", (value & 0x04) ? 1 : 0);
#endif
        debug_line("parking_level: %d", pSvcUserPref->ParkingLevel);
        memset(ip, 0, sizeof(ip));
        app_helper.get_interface_ip("wlan0", ip);
        memset(ip2, 0, sizeof(ip2));
        app_helper.get_interface_ip("p2p-wlan0-0", ip2);
        debug_line("wifi state: %s ip: %s %s", app_helper.check_interface_is_ready("wlan0") ? "ok" : "fail", ip, ip2);
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
		memset(ip, 0, sizeof(ip));
        app_helper.get_interface_ip("rtl0", ip);
        debug_line("usb_wifi state: %s ip: %s", app_helper.check_interface_is_ready("rtl0") ? "ok" : "fail", ip);
#endif
        debug_line("bt_state: %s", app_helper.check_bt_is_ready() ? "ok" : "fail");
        lte_ready = app_helper.check_lte_is_ready();
        if (lte_ready) {
            char response[128] = {0};
            memset(response, 0, sizeof(response));
            app_helper.linux_system_with_response("devid --sim", response);
            memset(ip, 0, sizeof(ip));
#if defined(CONFIG_BSP_CV25_NEXAR_D080) || defined(CONFIG_BSP_H32_NEXAR_D081)
            app_helper.get_interface_ip("usb0", ip);
#else
            app_helper.get_interface_ip("wwan0", ip);
#endif
            debug_line("lte_state: %s, ip: %s, sim_status: %s", "ok", ip, response);
        } else {
            debug_line("lte_state: %s", "fail");
        }
        debug_line("\r\n");
    } else if (strcmp(argv[1], "wifi") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "ap") == 0) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_AP;
            if (argc > 3) {
                if (strcmp(argv[3], "2.4g") == 0) {
                    pSvcUserPref->WifiSetting.Use5G = 0;
                } else if (strcmp(argv[3], "5g") == 0) {
                    pSvcUserPref->WifiSetting.Use5G = 1;
                }
            }
            if (argc > 4) {
                pSvcUserPref->WifiSetting.Channel = atoi(argv[4]);
            }
            user_setting_save();
            Pmic_NormalSoftReset();
        } else if (strcmp(argv[2], "sta") == 0) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_STA;
            if (argc > 3) {
                memset(pSvcUserPref->WifiSetting.StaSSID, 0, sizeof(pSvcUserPref->WifiSetting.StaSSID));
                snprintf(pSvcUserPref->WifiSetting.StaSSID, sizeof(pSvcUserPref->WifiSetting.StaSSID) - 1, "%s", argv[3]);
            }
            if (argc > 4) {
                memset(pSvcUserPref->WifiSetting.StaPassword, 0, sizeof(pSvcUserPref->WifiSetting.StaPassword));
                snprintf(pSvcUserPref->WifiSetting.StaPassword, sizeof(pSvcUserPref->WifiSetting.StaPassword) - 1, "%s", argv[4]);
            }
            user_setting_save();
            Pmic_NormalSoftReset();
        } else if (strcmp(argv[2], "p2p") == 0) {
            pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_P2P;
            if (argc > 3) {
                if (strcmp(argv[3], "2.4g") == 0) {
                    pSvcUserPref->WifiSetting.Use5G = 0;
                } else if (strcmp(argv[3], "5g") == 0) {
                    pSvcUserPref->WifiSetting.Use5G = 1;
                }
            }
            if (argc > 4) {
                pSvcUserPref->WifiSetting.Channel = atoi(argv[4]);
            }
            user_setting_save();
            Pmic_NormalSoftReset();
        } else {
            if (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_STA) {
                debug_line("wifi mode:%s wifi ssid:%s wifi password:%s", "sta", pSvcUserPref->WifiSetting.StaSSID, pSvcUserPref->WifiSetting.StaPassword);
            } else {
                wifi_ap_conf_items_s wifi_info;
                if (wifi_get_ap_info(&wifi_info) < 0) {
                    debug_line("wifi_get_ap_info failed");
                    return;
                }
                debug_line("wifi mode:%s wifi ssid:%s wifi password:%s",
                    (pSvcUserPref->WifiSetting.BootMode == WIFI_MODE_AP) ? "ap" : "p2p", wifi_info.ssid, wifi_info.password);
            }
        }
    } else if (strcmp(argv[1], "usb_wifi") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "sta") == 0) {
            pSvcUserPref->UsbWifiSetting.BootMode = WIFI_MODE_STA;
            if (argc > 3) {
                memset(pSvcUserPref->UsbWifiSetting.StaSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID));
                snprintf(pSvcUserPref->UsbWifiSetting.StaSSID, sizeof(pSvcUserPref->UsbWifiSetting.StaSSID) - 1, "%s", argv[3]);
            }
            if (argc > 4) {
                memset(pSvcUserPref->UsbWifiSetting.StaPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword));
                snprintf(pSvcUserPref->UsbWifiSetting.StaPassword, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword) - 1, "%s", argv[4]);
            }
            user_setting_save();
            Pmic_NormalSoftReset();
        } else {
            if (pSvcUserPref->UsbWifiSetting.BootMode == WIFI_MODE_STA) {
                debug_line("usb wifi mode:%s wifi ssid:%s wifi password:%s", "sta", pSvcUserPref->UsbWifiSetting.StaSSID, pSvcUserPref->UsbWifiSetting.StaPassword);
            }
        }
    } else if (strcmp(argv[1], "touch") == 0) {
        app_helper.touch(argv[2]);
    } else if (strcmp(argv[1], "bw") == 0) {
        rec_dvr_switch_black_white_mode(atoi(argv[2]), atoi(argv[3]));
    } else if (strcmp(argv[1], "watchdog") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            AmbaWDT_Start(5000U, 10U);
        } else if (strcmp(argv[2], "stop") == 0) {
            AmbaWDT_Stop();
        } else if (strcmp(argv[2], "feed") == 0) {
            AmbaWDT_Feed();
        } else if (strcmp(argv[2], "reboot") == 0) {
            extern void AmbaCSL_RctChipSoftReset(void);
            AmbaCSL_RctChipSoftReset();
        }
    } else if (strcmp(argv[1], "otp") == 0) {
        if (strcmp(argv[2], "sn") == 0) {
            if (strcmp(argv[3], "r") == 0) {
                UINT8 customer_id[32] = {0};
                memset(customer_id, 0, sizeof(customer_id));
                metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_CUSTOMER_ID, customer_id, 16, 0);
                debug_line("customer_id=%s", customer_id);
            } else if (strcmp(argv[3], "w") == 0) {
                if (strlen(argv[4]) > 0) {
                    UINT8 customer_id[16] = {0};
                    memcpy((void *)customer_id, argv[4], (strlen(argv[4]) > 16) ? 16 : strlen(argv[4]));
                    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_CUSTOMER_ID, customer_id, sizeof(customer_id), 0) >= 0) {
                        metadata_s *metadata = NULL;
                        metadata_get(&metadata);
                        memset(metadata->SerialNumber, 0, sizeof(metadata->SerialNumber));
                        memcpy(metadata->SerialNumber, customer_id, 16);
                        debug_line("metadata_write_otp_key ok");
                    } else {
                        debug_line("metadata_write_otp_key failed");
                    }
                } else {
                    debug_line("invalid sn");
                }
            }
        } else if (strcmp(argv[2], "aes") == 0) {
            if (strcmp(argv[3], "r") == 0) {
                UINT8 aes_key[32] = {0};
                memset(aes_key, 0, sizeof(aes_key));
                metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_AES_KEY, aes_key, 32, atoi(argv[4]));
                debug_line("\naes_key[%d]:", atoi(argv[4]));
                print_hex(aes_key, 32);
            } else if (strcmp(argv[3], "w") == 0) {
                if (strlen(argv[5]) == 64) {
                    int i = 0;
                    char tmp[3] = {0};
                    UINT8 aes_key[32] = {0};
                    int index = atoi(argv[4]);
                    memset((void *)aes_key, 0, sizeof(aes_key));
                    for (i = 0; i < 32; i++) {
                        tmp[0] = argv[5][i * 2];
                        tmp[1] = argv[5][i * 2 + 1];
                        tmp[2] = 0;
                        aes_key[i] = strtoul(tmp, NULL, 16) & 0xFF;
                    }
                    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_AES_KEY, aes_key, 32, index) >= 0) {
                        metadata_s *metadata = NULL;
                        metadata_get(&metadata);
                        memset(metadata->AesKeys[index].Key, 0, sizeof(metadata->AesKeys[index].Key));
                        memcpy(metadata->AesKeys[index].Key, aes_key, 32);
                        debug_line("metadata_write_otp_key ok");
                   } else {
                       debug_line("metadata_write_otp_key failed");
                   }
                } else {
                    debug_line("invalid aes key, len should be 64");
                }
            }
        } else if (strcmp(argv[2], "ecc") == 0) {
            if (strcmp(argv[3], "r") == 0) {
                UINT8 ecc_key[32] = {0};
                memset(ecc_key, 0, sizeof(ecc_key));
                metadata_read_otp_key(RTOS_AMBA_SIP_OTP_GET_ECC_KEY, ecc_key, 32, atoi(argv[4]));
                debug_line("ecc_key[%d]:", atoi(argv[4]));
                print_hex(ecc_key, 32);
            } else if (strcmp(argv[3], "w") == 0) {
                if (strlen(argv[5]) == 64) {
                    int i = 0;
                    char tmp[3] = {0};
                    UINT8 ecc_key[32] = {0};
                    int index = atoi(argv[4]);
                    memset((void *)ecc_key, 0, sizeof(ecc_key));
                    for (i = 0; i < 32; i++) {
                        tmp[0] = argv[5][i * 2];
                        tmp[1] = argv[5][i * 2 + 1];
                        tmp[2] = 0;
                        ecc_key[i] = strtoul(tmp, NULL, 16) & 0xFF;
                    }
                    if (metadata_write_otp_key(RTOS_AMBA_SIP_OTP_SET_ECC_KEY, ecc_key, 32, index) >= 0)  {
                        metadata_s *metadata = NULL;
                        metadata_get(&metadata);
                        memset(metadata->EccKeys[index].Key, 0, sizeof(metadata->EccKeys[index].Key));
                        memcpy(metadata->EccKeys[index].Key, ecc_key, 32);
                        debug_line("metadata_write_otp_key ok");
                   } else {
                        debug_line("metadata_write_otp_key failed");
                   }
                } else {
                    debug_line("invalid ecc key, len should be 64");
                }
            }
        } else if (strcmp(argv[2], "enable_secure_boot") == 0) {
            if (metadata_enable_otp_secure_boot() < 0) {
                debug_line("metadata_enable_otp_secure_boot failed");
            } else {
                debug_line("metadata_enable_otp_secure_boot ok");
            }
        }
    } else if (strcmp(argv[1], "hang") == 0) {
        int *p = NULL;
        *p = 5;
    } else if (strcmp(argv[1], "camera") == 0) {
        show_camera_status(CAMERA_CHANNEL_INTERNAL);
        show_camera_status(CAMERA_CHANNEL_EXTERNAL);
    } else if (strcmp(argv[1], "pwm") == 0) {
        pwm_debug(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    } else if (strcmp(argv[1], "ae") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            int i = 0, j = 0;
            char line[1024] = {0};
            AE_ALGO_INFO_s AeInfo;
            AmbaImgProc_AEGetAEAlgoInfo(atoi(argv[3]), &AeInfo);
            debug_line("\nRoiCnt=%d", AeInfo.RoiCnt);
            for (i = 0; i < 8; i++) {
                memset(line, 0, sizeof(line));
                for (j = 0; j < 12; j++) {
                    snprintf(line + strlen(line), 10, "%.3d ", AeInfo.RoiInfo[0][i * 12 + j]);
                }
                debug_line("\n%s\n", line);
            }
        } else if (strcmp(argv[2], "set") == 0) {
            int i = 0;
            AE_ALGO_INFO_s AeInfo;
            int row = atoi(argv[4]);
            int col = atoi(argv[5]);
            if (row > 8) {
                debug_line("invalid row %d, should be between 0 and 8", row);
                return;
            }
            if (col > 12) {
                debug_line("invalid col %d, should be between 0 and 12", col);
                return;
            }
            AmbaImgProc_AEGetAEAlgoInfo(atoi(argv[3]), &AeInfo);
            for (i = 0; i < 96; i++) {
                if (i == (row * 12 + col)) {
                    AeInfo.RoiInfo[0][i] = atoi(argv[6]);
                } else {
                    AeInfo.RoiInfo[0][i] = 1;
                }
            }
            AmbaImgProc_AESetAEAlgoInfo(atoi(argv[3]), &AeInfo);
        }
    } else if (strcmp(argv[1], "delete_dir") == 0) {
        delete_dir(argv[2]);
    } else if (strcmp(argv[1], "clean_dir") == 0) {
        clean_dir(argv[2]);
    } else if (strcmp(argv[1], "i2c") == 0) {
        if (strcmp(argv[2], "w") == 0) {
            int channel = atoi(argv[3]);
            unsigned char slave_addr = strtoul(argv[4], NULL, 16) & 0xFF;
            unsigned char reg = strtoul(argv[5], NULL, 16) & 0xFF;
            unsigned char value = strtoul(argv[6], NULL, 16) & 0xFF;
            i2c_write_reg(channel, slave_addr, reg, value);
        } else if (strcmp(argv[2], "r") == 0) {
            int channel = atoi(argv[3]);
            int slave_addr = strtoul(argv[4], NULL, 16) & 0xFF;
            unsigned char reg = strtoul(argv[5], NULL, 16) & 0xFF;
            unsigned char rx_data[32] = {0};
            int i = 0, read_num = 1;
            if (argc > 6) {
                read_num = atoi(argv[6]);
            }
            if (read_num <= 0) {
                read_num = 1;
            }
            if (read_num > 32) {
                read_num = 32;
            }
            i2c_read(channel, slave_addr, reg, rx_data, read_num);
            debug_line("");
            debug_line("0x%.2X=0x%.2X", reg, rx_data[0]);
            if (read_num > 1) {
                for (i = 1; i < read_num; i++) {
                    debug_line("     0x%.2X", rx_data[i]);
                }
            }
        } else if (strcmp(argv[2], "scan") == 0) {
            int channel = atoi(argv[3]);
            unsigned char reg = strtoul(argv[4], NULL, 16) & 0xFF;
            unsigned char rx_data = 0;
            int slave_addr = 0;
            for (slave_addr = 0; slave_addr <= 0xFF; slave_addr++) {
                if (i2c_read_reg(channel, slave_addr, reg, &rx_data) >= 0) {
                    debug_line("slave_addr: 0x%.2X, 0x%.2X=0x%.2X", slave_addr, reg, rx_data);
                }
            }
        } else if (strcmp(argv[2], "set_delay") == 0) {
            AmbaI2C_CustomSetDelay(atoi(argv[3]));
        } else {
            debug_line("t i2c w [channel] [slave_addr] [reg] [value]");
            debug_line("t i2c r [channel] [slave_addr] [reg] [num]");
        }
    } else if (strcmp(argv[1], "cv_test") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, RefOD_PyramidDataRdyHandler);
        } else if (strcmp(argv[2], "stop") == 0) {
            AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, RefOD_PyramidDataRdyHandler);
        }
    } else if (strcmp(argv[1], "dump_log") == 0) {
        offline_log_dump(argv[2]);
    } else if (strcmp(argv[1], "res_set") == 0) {
        extern void SvcResCfgTask_LoadSetting(void);
        extern UINT32 SvcFlowControl_Exec(const char *pCmd);
        int stream_id = atoi(argv[2]);
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        SVC_USER_PREF_s *pSvcUserPref = NULL;

        SvcUserPref_Get(&pSvcUserPref);
        if (stream_id == 0) {
            pSvcUserPref->InternalCamera.MainStream.width = width;
            pSvcUserPref->InternalCamera.MainStream.height = height;
        } else if (stream_id == 1) {
            pSvcUserPref->InternalCamera.SecStream.width = width;
            pSvcUserPref->InternalCamera.SecStream.height = height;
        } else if (stream_id == 2) {
            pSvcUserPref->ExternalCamera.MainStream.width = width;
            pSvcUserPref->ExternalCamera.MainStream.height = height;
        } else if (stream_id == 3) {
            pSvcUserPref->ExternalCamera.SecStream.width = width;
            pSvcUserPref->ExternalCamera.SecStream.height = height;
        } else {
            debug_line("invalid stream id");
            return;
        }
        SvcResCfgTask_LoadSetting();
        SvcFlowControl_Exec("liveview_switch");
        user_setting_save();
    }
#if 0
    else if (strcmp(argv[1], "imu_rtsp_notify") == 0) {
        extern unsigned char gsensor_buffer[64];
        static int cnt = 0;
        GSENSOR_DESC_s Desc;
        memset(gsensor_buffer, 0, sizeof(gsensor_buffer));
        snprintf((char *)gsensor_buffer, sizeof(gsensor_buffer) - 1, "i am %d", cnt);
        Desc.pBufAddr = &gsensor_buffer[0];
        Desc.DataSize = strlen((char *)gsensor_buffer);
        Desc.EncodedSamples = 100;
        SvcRtspLink_GSensorNotify(&Desc);
    }
#endif
    else if (strcmp(argv[1], "stamp") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->WaterMark = atoi(argv[2]);
    } else if (strcmp(argv[1], "24h_mode") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->Use24HMode = atoi(argv[2]);
    }
#ifdef CONFIG_ENABLE_IMU
    else if (strcmp(argv[1], "imu") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);

        if (strcmp(argv[2], "debug") == 0) {
            imu_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "gyro_odr") == 0) {
            pSvcUserPref->ImuSetting.GyroOdr = atoi(argv[3]);
            Imu_SetGyroPara(IMU_GYRO_ODR,pSvcUserPref->ImuSetting.GyroOdr);
            user_setting_save();
        } else if (strcmp(argv[2], "gyro_range") == 0) {
            pSvcUserPref->ImuSetting.GyroRange = atoi(argv[3]);
            Imu_SetGyroPara(IMU_GYRO_RANGE,pSvcUserPref->ImuSetting.GyroRange);
            user_setting_save();
        } else if (strcmp(argv[2], "accel_odr") == 0) {
            pSvcUserPref->ImuSetting.AccelOdr = atoi(argv[3]);
            Imu_SetAccelPara(IMU_ACCEL_ODR,pSvcUserPref->ImuSetting.AccelOdr);
            user_setting_save();
        } else if (strcmp(argv[2], "accel_range") == 0) {
            pSvcUserPref->ImuSetting.AccelRange = atoi(argv[3]);
            Imu_SetAccelPara(IMU_ACCEL_RANGE,pSvcUserPref->ImuSetting.AccelRange);
            user_setting_save();
        } else if (strcmp(argv[2], "calibration") == 0) {
            Imu_SetCalibration(atoi(argv[3]));
        } else if (strcmp(argv[2], "polarity") == 0) {
            pSvcUserPref->ImuSetting.AxisPolarity = atoi(argv[3]);
            Imu_SetAxisPolarity(pSvcUserPref->ImuSetting.AxisPolarity);
            user_setting_save();
        } else if (strcmp(argv[2], "read") == 0) {
            IMU_DATA_s imu_data;
            TEMP_DATA_s temp_data;
            Imu_GetData(&imu_data, atoi(argv[3]));
            Imu_GetTemperature(&temp_data);
            debug_line("imu gyro: %d %d %d", imu_data.gyro.x, imu_data.gyro.y, imu_data.gyro.z);
            debug_line("imu accel: %d %d %d", imu_data.accel.x, imu_data.accel.y, imu_data.accel.z);
            debug_line("imu temp: %d, %.1f", temp_data.temp, temp_data.temp_float);
            debug_line("");
        } else if (strcmp(argv[2], "power_off") == 0) {
            Imu_PowerOff(atoi(argv[3]));
        } else if (strcmp(argv[2], "calibration") == 0) {
            extern int product_line_cmd_do_imu_calibration(const char *direction);
            product_line_cmd_do_imu_calibration(argv[3]);
        } else if (strcmp(argv[2], "calibration_data") == 0) {
            SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom = NULL;
            SvcUserPrefCustom_Get(&pSvcUserPrefCustom);
            debug_line("imu calibration flag: %d", pSvcUserPrefCustom->ImuCalibrationData.imu_calibration_flag);
            debug_line("     accel 2g offset: %d %d %d", pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.x,
                                                         pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.y,
                                                    pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.z);
            debug_line("     accel 4g offset: %d %d %d", pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.x,
                                                     pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.y,
                                                    pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.z);
            debug_line("     accel 8g offset: %d %d %d", pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.x,
                                                     pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.y,
                                                    pSvcUserPrefCustom->ImuCalibrationData.accel_offset_2g.z);
            debug_line("     accel 16g offset: %d %d %d", pSvcUserPrefCustom->ImuCalibrationData.accel_offset_16g.x,
                                         pSvcUserPrefCustom->ImuCalibrationData.accel_offset_16g.y,
                                        pSvcUserPrefCustom->ImuCalibrationData.accel_offset_16g.z);
            debug_line("          gyro offset: %d %d %d", pSvcUserPrefCustom->ImuCalibrationData.gyro_offset.x,
                             pSvcUserPrefCustom->ImuCalibrationData.gyro_offset.y,
                            pSvcUserPrefCustom->ImuCalibrationData.gyro_offset.z);
        } else if (strcmp(argv[2], "calibration_enable") == 0) {
            SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom = NULL;
            SvcUserPrefCustom_Get(&pSvcUserPrefCustom);
            pSvcUserPrefCustom->ImuCalibrationData.imu_calibration_flag = atoi(argv[3]) ? 1 : 0;
            SvcUserPrefCustom_Save();
        } else if (strcmp(argv[2], "reg") == 0) {
            if (strcmp(argv[3], "w") == 0) {
                unsigned char reg = strtoul(argv[4], NULL, 16) & 0xFF;
                unsigned char value = strtoul(argv[5], NULL, 16) & 0xFF;
                Imu_Write(reg, value);
            } else if (strcmp(argv[3], "r") == 0) {
                unsigned char reg = strtoul(argv[4], NULL, 16) & 0xFF;
                unsigned char value = 0;
                Imu_Read(reg, &value);
                debug_line("imu reg, 0x%.2x=0x%.2x", reg, value);
            }
        }
    }
#endif
    else if (strcmp(argv[1], "gnss") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            gnss_task_start();
        } else if (strcmp(argv[2], "stop") == 0) {
            gnss_task_stop();
        } else if (strcmp(argv[2], "debug") == 0) {
            gnss_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "show_result") == 0) {
            gnss_parser_debug_result(NULL);
        } else if (strcmp(argv[2], "datarate") == 0) {
            SVC_USER_PREF_s *pSvcUserPref = NULL;
            SvcUserPref_Get(&pSvcUserPref);
            pSvcUserPref->GnssSetting.DataRate = atoi(argv[3]);
            user_setting_save();
            gnss_set_data_rate(pSvcUserPref->GnssSetting.DataRate);
            gnss_set_dsp_uart_baudrate(pSvcUserPref->GnssSetting.DataRate);
        } else if (strcmp(argv[2], "set_gst") == 0) {
            gnss_set_gst_enable(atoi(argv[3]));
        }
#if defined(CONFIG_GNSS_PPS_TIME_SYNC_ENABLE)
		else if (strcmp(argv[2], "pps") == 0) {
            if (strcmp(argv[3], "debug") == 0) {
                gnss_pps_task_set_debug(atoi(argv[4]));
            }
        }
#endif
    } else if (strcmp(argv[1], "button") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            button_task_start();
        } else if (strcmp(argv[2], "stop") == 0) {
            button_task_stop();
        } else if (strcmp(argv[2], "debug") == 0) {
            button_task_set_debug(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "battery") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            battery_task_start();
        } else if (strcmp(argv[2], "stop") == 0) {
            battery_task_stop();
        } else if (strcmp(argv[2], "debug") == 0) {
            battery_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "low_power_check") == 0) {
            battery_task_set_low_power_check(atoi(argv[3]));
        } else if (strcmp(argv[2], "percentage") == 0) {
            debug_line("battery_percentage = %d", battery_task_get_percentage());
        } else if (strcmp(argv[2], "adc") == 0) {
            debug_line("battery_adc = %d", battery_task_get_adc());
        }
    } else if (strcmp(argv[1], "lnx_time") == 0) {
        linux_time_s linux_time;
        app_helper.get_linux_time(&linux_time);
        debug_line("sec: %d, usec: %d, %d-%d-%d %.2d:%.2d:%.2d", linux_time.sec, linux_time.usec,
            linux_time.local_time.year, linux_time.local_time.month, linux_time.local_time.day,
            linux_time.local_time.hour, linux_time.local_time.minute, linux_time.local_time.second);
    } else if (strcmp(argv[1], "time_test") == 0) {
        linux_time_s linux_time;
        AMBA_RTC_DATE_TIME_s rtc_time, rtc_time2;
        app_helper.get_linux_time(&linux_time);
        AmbaTime_UtcTimeStamp2DateTime(linux_time.sec, &rtc_time);
        AmbaTime_TaiTimeStamp2DateTime(linux_time.sec, &rtc_time2);
        debug_line("sec: %d, usec: %d, %d-%d-%d %.2d:%.2d:%.2d, rtc:%d-%d-%d %.2d:%.2d:%.2d, %d-%d-%d %.2d:%.2d:%.2d", linux_time.sec, linux_time.usec,
            linux_time.utc_time.year, linux_time.utc_time.month, linux_time.utc_time.day,
            linux_time.utc_time.hour, linux_time.utc_time.minute, linux_time.utc_time.second,
            rtc_time.Year, rtc_time.Month, rtc_time.Day,
            rtc_time.Hour, rtc_time.Minute, rtc_time.Second,
            rtc_time2.Year, rtc_time2.Month, rtc_time2.Day,
            rtc_time2.Hour, rtc_time2.Minute, rtc_time2.Second);
    } else if (strcmp(argv[1], "double") == 0) {
        debug_line("%7f, %.5f, %.6f", 12.34, 10.0, 12.9956789);
    } else if (strcmp(argv[1], "adc") == 0) {
        unsigned int data = 0;
        AmbaADC_Config(10000);
        AmbaADC_SingleRead(atoi(argv[2]), &data);
        debug_line("adc channel: %d, value: %d", atoi(argv[2]), data);
    } else if (strcmp(argv[1], "msc") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            (void)AmbaSvcUsb_DeviceSysInit();
            AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MSC);
            AmbaSvcUsb_DeviceClassMscLoad(SD_SLOT[0]);
            AmbaSvcUsb_DeviceClassStart();
        } else if (strcmp(argv[2], "stop") == 0) {
            AmbaSvcUsb_DeviceClassStop();
            AmbaSvcUsb_DeviceSysDeInit();
        }
    } else if (strcmp(argv[1], "linux_boot") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->LinuxBoot = atoi(argv[2]);
        user_setting_save();
        Pmic_NormalSoftReset();
    } else if (strcmp(argv[1], "linux_console") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->LinuxConsole = atoi(argv[2]);
        user_setting_save();
        Pmic_NormalSoftReset();
    } else if (strcmp(argv[1], "setting") == 0) {
        if (strcmp(argv[2], "export") == 0) {
            app_helper.linux_system("cp -f /pref/settings.ini /mnt/extsd/settings.ini");
        } else if (strcmp(argv[2], "import") == 0) {
            app_helper.linux_system("cp -f /mnt/extsd/settings.ini /pref/settings.ini");
            Pmic_NormalSoftReset();
        } else if (strcmp(argv[2], "default") == 0) {
            remove(USER_SETTING_SAVE_PATH);
            Pmic_NormalSoftReset();
        }
    } else if (strcmp(argv[1], "switch_boot") == 0) {
        Pmic_SetBootFailTimes(3);
        Pmic_SoftReset();
    } else if (strcmp(argv[1], "parking_level") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "get") == 0) {
            debug_line("parking_level: %d", pSvcUserPref->ParkingLevel);
        } else if (strcmp(argv[2], "set") == 0) {
            pSvcUserPref->ParkingLevel = atoi(argv[3]);
            user_setting_save();
        }
    } else if (strcmp(argv[1], "timer_wakeup") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "get") == 0) {
            debug_line("timer_wakeup minutes: %d", pSvcUserPref->PowerOffWakeupMinutes);
        } else if (strcmp(argv[2], "set") == 0) {
            pSvcUserPref->PowerOffWakeupMinutes = atoi(argv[3]);
            user_setting_save();
        }
    } else if (strcmp(argv[1], "lte_wakeup") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "get") == 0) {
            debug_line("lte_wakeup enable: %d", pSvcUserPref->EnableLteRemoteWakeup ? 1 : 0);
        } else if (strcmp(argv[2], "set") == 0) {
            pSvcUserPref->EnableLteRemoteWakeup = atoi(argv[3]) ? 1 : 0;
            user_setting_save();
        }
    } else if (strcmp(argv[1], "resolution") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "4k") == 0) {
            pSvcUserPref->InternalCamera.MainStream.width = 3840;
            pSvcUserPref->InternalCamera.MainStream.height = 2160;
            pSvcUserPref->InternalCamera.MainStream.bitrate = 40;
            pSvcUserPref->InternalCamera.Fps = 25;
            pSvcUserPref->ExternalCamera.Fps = 25;
            user_setting_save();
            Pmic_SoftReset();
        } else if (strcmp(argv[2], "1080p") == 0) {
            pSvcUserPref->InternalCamera.MainStream.width = 1920;
            pSvcUserPref->InternalCamera.MainStream.height = 1080;
            pSvcUserPref->InternalCamera.MainStream.bitrate = 14;
            pSvcUserPref->InternalCamera.Fps = 30;
            pSvcUserPref->ExternalCamera.Fps = 30;
            user_setting_save();
            Pmic_SoftReset();
        } else if (strcmp(argv[2], "2k") == 0) {
            pSvcUserPref->InternalCamera.MainStream.width = 2560;
            pSvcUserPref->InternalCamera.MainStream.height = 1440;
            pSvcUserPref->InternalCamera.MainStream.bitrate = 18;
            pSvcUserPref->InternalCamera.Fps = 30;
            pSvcUserPref->ExternalCamera.Fps = 30;
            user_setting_save();
            Pmic_SoftReset();
        }
    }  else if (strcmp(argv[1], "piv_func") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->PivOnOff = atoi(argv[2]) ? OPTION_ON : OPTION_OFF;
        user_setting_save();
        Pmic_SoftReset();
    } else if (strcmp(argv[1], "speed_unit") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "kph") == 0) {
            pSvcUserPref->SpeedUnit = SPEED_UNIT_KMH;
        } else if (strcmp(argv[2], "mph") == 0) {
            pSvcUserPref->SpeedUnit = SPEED_UNIT_MPH;
        }
        user_setting_save();
    } else if (strcmp(argv[1], "udisk") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->UsbMassStorage = OPTION_ON;
        user_setting_save();
        Pmic_SoftReset();
    } else if (strcmp(argv[1], "mcu_update") == 0) {
        if (strcmp(argv[2], "debug") == 0) {
            mcu_update_task_set_debug(atoi(argv[3]));
        } else if (strcmp(argv[2], "set") == 0) {
            app_helper.mcu_update_flag = atoi(argv[3]);
        }
    } else if (strcmp(argv[1], "hard_reset") == 0) {
        if (strcmp(argv[2], "set") == 0) {
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0C, atoi(argv[3]) & 0xff);
        } else if (strcmp(argv[2], "get") == 0) {
            unsigned char value = 0;
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0B, &value);
            debug_line("hard reset time: %d seconds", value);
        }
    } else if (strcmp(argv[1], "dcf") == 0) {
        if (strcmp(argv[2], "list") == 0) {
            char FileName[64] = {0};
            SVC_STG_MONI_DRIVE_INFO_s  DriveInfos;
            SvcStgMonitor_GetDriveInfo(SD_SLOT[0], &DriveInfos);
            memset(FileName, 0, sizeof(FileName));
            AmbaDCF_GetFileName(DriveInfos.DriveID, 0U, AMBA_DCF_FILE_TYPE_VIDEO, FileName, atoi(argv[3]));
            debug_line("index: %d, %s", atoi(argv[3]), FileName);
        }
    } else if (strcmp(argv[1], "utc_offset") == 0) {
        if (strcmp(argv[2], "get") == 0) {
            debug_line("utc_offset: %d", AmbaFS_GetUtcOffsetMinutes());
        } else if (strcmp(argv[2], "set") == 0) {
            AmbaFS_SetUtcOffsetMinutes(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "uvc") == 0) {
        if (strcmp(argv[2], "debug") == 0) {
            uvc_set_debug(atoi(argv[3]));
            uvc_output_task_set_debug(atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "cv") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        pSvcUserPref->CvOnOff = atoi(argv[2]) ? OPTION_ON : OPTION_OFF;
        user_setting_save();
        Pmic_SoftReset();
    } else if (strcmp(argv[1], "sd_debug") == 0) {
        if (strcmp(argv[2], "set") == 0) {
            AmbaUserSD_PhyCtrl_SetDebug(atoi(argv[3]) ? 1 : 0);
        } else if (strcmp(argv[2], "get") == 0) {
            debug_line("sd_debug enable: %d", AmbaUserSD_PhyCtrl_GetDebug());
        }
    } else if (strcmp(argv[1], "api_debug") == 0) {
        if (strcmp(argv[2], "set") == 0) {
            linux_api_service_set_debug(atoi(argv[3]) ? 1 : 0);
        } else if (strcmp(argv[2], "get") == 0) {
            debug_line("api_debug enable: %d", linux_api_service_get_debug());
        }
    } else if (strcmp(argv[1], "auto_power_off") == 0) {
        if (strcmp(argv[2], "set") == 0) {
            app_helper.auto_power_off = atoi(argv[3]) ? 1 : 0;
        } else if (strcmp(argv[2], "get") == 0) {
            debug_line("auto_power_off: %d", app_helper.auto_power_off);
        }
    } else if (strcmp(argv[1], "ir_threshold") == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        if (strcmp(argv[2], "set") == 0) {
            int value = atoi(argv[3]);
            if (value < 0 || value > 5) {
                debug_line("invalid ir_threshold, range: %d-%d", 0, 5);
                return;
            }
            pSvcUserPref->IrThresHold = value;
            user_setting_save();
        } else if (strcmp(argv[2], "get") == 0) {
            debug_line("ir_threshold get: %d", pSvcUserPref->IrThresHold);
        }
    } else if (strcmp(argv[1], "rec_control") == 0) {
        if (strcmp(argv[2], "start") == 0) {
            rec_dvr_camera_record_control(atoi(argv[3]), 1);
        } else if (strcmp(argv[2], "stop") == 0) {
            rec_dvr_camera_record_control(atoi(argv[3]), 0);
        }
    }
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    else if (strcmp(argv[1], "logo_led") == 0) {
        app_helper.set_logo_led_color(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    } else if (strcmp(argv[1], "logo_fade") == 0) {
        app_util_logo_led_fade_start();
    } else if (strcmp(argv[1], "logo_set_param") == 0) {
        app_util_logo_led_fade_set_param(atoi(argv[2]), atoi(argv[3]));
    }
#endif
	else if (strcmp(argv[1], "sensor_log") == 0) {
        //svc_app res_cfg
        //svc_dsp dump_liv
        //sensor detail 0 0
        //sysinfo vin 0
        //dmesg dsp 4096
        debug_line("\nsvc_app res_cfg\nsvc_dsp dump_liv\nsensor detail 0 0\nsysinfo vin 0\ndmesg dsp 4096\n");
    } else if (strcmp(argv[1], "gnss_bitrate") == 0) {
        char cmd[128] = {0};
        memset(cmd, 0, sizeof(cmd));
        if (strcmp(argv[2], "1") == 0) {
            snprintf(cmd, sizeof(cmd) - 1, "$PAIR864,0,0,115200*1B\r\n");
        } else if (strcmp(argv[2], "2") == 0) {
            snprintf(cmd, sizeof(cmd) - 1, "$PAIR864,0,0,230400*19\r\n");
        } else {
            return;
        }
        gnss_task_uart_write2((const unsigned char *)cmd, strlen(cmd));
    } else if (strcmp(argv[1], "gnss_write") == 0) {
        char cmd[128] = {0};
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd) - 1, "%s\r\n", argv[2]);
        gnss_task_uart_write2((const unsigned char *)cmd, strlen(cmd));
    }
#if 0//defined(CONFIG_BSP_H32_NEXAR_D081)
	else if (strcmp(argv[1], "himax_ota") == 0) {
        extern int himax_do_ota(void);
        himax_do_ota();
    }
#endif
	else {
        debug_line("not support command");
    }
}

static void time_dump(unsigned int time, char *out, int size)
{
    unsigned int days = 0, hours = 0, minutes = 0, seconds = time;

    minutes = seconds / 60;
    seconds %= 60;
    if (minutes > 60) {
        hours = minutes / 60;
        minutes %= 60;
    }
    if (hours > 24) {
        days = hours / 24;
        hours %= 24;
    }
    if (days > 0) {
        snprintf(out, size, "%d days %d hours %d minutes %d seconds", days, hours, minutes, seconds);
    } else if (hours > 0) {
        snprintf(out, size, "%d hours %d minutes %d seconds", hours, minutes, seconds);
    } else if (minutes > 0) {
        snprintf(out, size, "%d minutes %d seconds", minutes, seconds);
    } else {
        snprintf(out, size, "%d seconds", seconds);
    }
}

static int show_camera_status(CAMERA_CHANNEL_e channel)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    char time[64] = {0};

    memset(time, 0, sizeof(time));
    time_dump(rec_dvr_get_rec_time(channel), time, sizeof(time) - 1);
    SvcUserPref_Get(&pSvcUserPref);
    debug_line("\r\n%s camera:\r\nEnabled:%d\r\nWorking: %d\r\nAuto Record:%d\r\nRecStatus:%s\r\nPreviewStatus:%s\r\nEvent Record:%d\r\nRec Time: %s\r\n", (channel == CAMERA_CHANNEL_INTERNAL) ? "internal" : "external",
                                                                    (channel == CAMERA_CHANNEL_INTERNAL) ? pSvcUserPref->InternalCamera.Enable : app_helper.external_camera_enabled,
                                                                    app_helper.check_camera_is_working(channel),
                                                                    (channel == CAMERA_CHANNEL_INTERNAL) ? pSvcUserPref->InternalCamera.AutoRecord : pSvcUserPref->ExternalCamera.AutoRecord,
                                                                    rec_dvr_rec_status_2_str(rec_dvr_get_rec_status(channel)),
                                                                    rec_dvr_rec_status_2_str(rec_dvr_get_preview_status(channel)),
                                                                    rec_dvr_is_event_recording(channel),
                                                                    time);

    return 0;
}

