#include "app_base.h"

static int power_state = -1;
static int sd_state = -1;

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
static int sensor_left_connected = 0, sensor_right_connected = 0;
#endif
static void app_util_sys_timer_handler(int eid)
{
    int cur_usb_state = 0;
    int cur_sd_state = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    int cur_left_addon_type = 0, cur_right_addon_type = 0;
#endif

    if (eid == TIMER_UNREGISTER) {
        return;
    }

    //feed watchdog
    if ((app_helper.mcu_update_flag & 0x01) != 0x01) {//not main mcu update
        unsigned char state = Pmic_GetAccState();
        if (state != app_helper.acc_connected) {
            if (state == 0) {
                app_msg_queue_send(APP_MSG_ID_ACC_DISCONNECTED, 0, 0, 0);
            } else {
                app_helper.with_acc = 1;
                app_msg_queue_send(APP_MSG_ID_ACC_CONNECTED, 0, 0, 0);
            }
            app_helper.acc_connected = state;
        }
    }

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    cur_left_addon_type = app_helper.get_left_addon_type();
    if (cur_left_addon_type != app_helper.left_addon_type) {
        if (((cur_left_addon_type == ADDON_TYPE_SENSOR_BOARD || cur_left_addon_type == ADDON_TYPE_LTE_WIFI_BOARD) && app_helper.left_addon_type == ADDON_TYPE_NONE)
            || ((app_helper.left_addon_type == ADDON_TYPE_SENSOR_BOARD || app_helper.left_addon_type == ADDON_TYPE_LTE_WIFI_BOARD) && cur_left_addon_type == ADDON_TYPE_NONE)) {
            app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
            app_timer_unregister(TIMER_1HZ, app_util_sys_timer_handler);
            return;
        }
    }
    app_helper.left_addon_type = cur_left_addon_type;

    cur_right_addon_type = app_helper.get_right_addon_type();
    if (cur_right_addon_type != app_helper.right_addon_type) {
        if (((cur_right_addon_type == ADDON_TYPE_SENSOR_BOARD || cur_right_addon_type == ADDON_TYPE_LTE_WIFI_BOARD) && app_helper.right_addon_type == ADDON_TYPE_NONE)
            || ((app_helper.right_addon_type == ADDON_TYPE_SENSOR_BOARD || app_helper.right_addon_type == ADDON_TYPE_LTE_WIFI_BOARD) && cur_right_addon_type == ADDON_TYPE_NONE)) {
            app_msg_queue_send(APP_MSG_ID_REBOOT, 0, 0, 0);
            app_timer_unregister(TIMER_1HZ, app_util_sys_timer_handler);
            return;
        }
    }
    app_helper.right_addon_type = cur_right_addon_type;
#endif

    cur_sd_state = AmbaSD_IsCardPresent(SD_CHANNEL);
    if (cur_sd_state != sd_state) {
        if (cur_sd_state == 0) {
            app_timer_unregister(TIMER_1HZ, app_util_sys_timer_handler);
            if (app_helper.force_power_off == 0) {
                app_helper.force_power_off = 1;
                app_helper.do_power_off();
            }
        }
        sd_state = cur_sd_state;
    }
    cur_usb_state = Pmic_CheckUsbConnected();
    if (cur_usb_state != power_state) {
        ipc_event_s event;
        memset(&event, 0, sizeof(event));
        event.event_id = NOTIFY_POWER_STATE_CHANGE_ID;
        event.param = cur_usb_state;
        linux_api_service_notify_event(event);
        if (cur_usb_state == 0) {
            app_msg_queue_send(APP_MSG_ID_USB_REMOVE, 0, 0, 0);
        } else {
            app_msg_queue_send(APP_MSG_ID_USB_INSERT, 0, 0, 0);
        }
        power_state = cur_usb_state;
    }
}

static int cancel_power_off_wait_started = 0;
static void app_util_do_power_off_cancel_wait_timer_handler(int eid)
{
    static int power_off_counter = 0;
    if (eid == TIMER_UNREGISTER) {
        power_off_counter = 0;
        cancel_power_off_wait_started = 0;
        return;
    }
    power_off_counter++;
    if (power_off_counter >= 5) {
        power_off_counter = 0;
        app_timer_unregister(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);
        rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, app_helper.do_power_off);
    }
}

int app_util_cancel_power_off(void)
{
    if (cancel_power_off_wait_started == 0) {
        return -1;
    }
    debug_line("do_power_off_cancel");
    app_timer_unregister(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);

    return 0;
}

static void app_util_send_power_off_notify(void)
{
    ipc_event_s event;
    memset(&event, 0, sizeof(event));
    event.event_id = NOTIFY_CAMERA_SHUTDOWN;
    linux_api_service_notify_event(event);
}

static void app_util_delay_power_off_timer_handler(int eid)
{
    static int count = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (eid == TIMER_UNREGISTER) {
        count = 0;
        return;
    }
    count++;
    SvcUserPref_Get(&pSvcUserPref);
    if (count >= pSvcUserPref->BatteryPowerTime) {
        count = 0;
        app_timer_unregister(TIMER_1HZ, app_util_delay_power_off_timer_handler);
        cancel_power_off_wait_started = 1;
        app_timer_unregister(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);
        app_timer_register(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);
        app_util_send_power_off_notify();
    }
}

int app_util_delay_power_off_start(void)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->BatteryPowerTime == 0) {
        cancel_power_off_wait_started = 1;
        app_timer_unregister(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);
        app_timer_register(TIMER_1HZ, app_util_do_power_off_cancel_wait_timer_handler);
        app_util_send_power_off_notify();
    } else {
        app_timer_unregister(TIMER_1HZ, app_util_delay_power_off_timer_handler);
        app_timer_register(TIMER_1HZ, app_util_delay_power_off_timer_handler);
    }

    return 0;
}

int app_util_delay_power_off_stop(void)
{
    app_timer_unregister(TIMER_1HZ, app_util_delay_power_off_timer_handler);

    return 0;
}

int app_util_check_lte_usb_mass_storage(void)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    unsigned char value = 0;
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    int lte_connected = 0;
#endif

    if (app_helper.usb_mass_storage_on) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        int usb_connected = 0;
        if (app_helper.check_usb_left_connected()) {
            //switch usb
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            value |= 0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
            usb_connected = 1;
        } else if (app_helper.check_usb_right_connected()) {
            //switch usb
            i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
            value &= ~0x04;
            i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
            usb_connected = 1;
        }
        if (usb_connected) 
#endif
		{
            unsigned int result = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
            app_helper.gpio_set(GPIO_PIN_33, 0);
#endif
            AmbaSvcUsb_SetConfigured(0);
            AmbaFS_SetVolumeLabel(SD_SLOT[0], USB_MASS_STORAGE_LABEL);
            (void)AmbaSvcUsb_DeviceSysInit();
            AmbaSvcUsb_DeviceClassSet(AMBA_USBD_CLASS_MSC);
            AmbaSvcUsb_DeviceClassMscLoad(SD_SLOT[0]);
            AmbaSvcUsb_DeviceClassStart();
            while (AmbaSvcUsb_GetConfigured(&result) == 0) {
                msleep(300);
            }
            debug_line("usb configured value: 0x%X", result);AmbaPrint_Flush();
            if (result == 1) {//usb msc
                app_helper.usb_mass_storage_mode = 1;
                app_helper.set_led_onoff(1);
                app_helper.set_led_brihtness(1);
                app_helper.set_led_fade(0);
                app_helper.set_led_color(0, 0xb0, 0x63);
                return 1;
            } else {
                AmbaSvcUsb_DeviceClassStop();
            }
        }
        Pmic_NormalSoftReset();
    }
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    app_helper.gpio_set(GPIO_PIN_33, 1);
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    //switch usb
    if (app_helper.check_lte_left_connected()) {
        debug_line("detect left lte connected");
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
        value |= 0x04;
        value |= 0x01;
        lte_connected = 1;
    } else if (app_helper.check_lte_right_connected()) {
        debug_line("detect right lte connected");
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
        value &= ~0x04;
        value |= 0x02;
        lte_connected = 1;
    } else {
        i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
        if (app_helper.check_usb_left_connected()) {
            value |= 0x04;
            debug_line("no lte connected, left usb connected");
        } else if (app_helper.check_usb_right_connected()) {
            value &= ~0x04;
            debug_line("no lte connected, right usb connected");
        } else {
            debug_line("no lte connected");
            return 0;
        }
    }
    debug_line("set main mcu value: 0x03=0x%.2x", value);
    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
    msleep(20);
    if (lte_connected) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2) && defined(CONFIG_PCBA_DVT2)
        unsigned int i2c_channel = get_lte_i2c_channel();
#else
        unsigned int i2c_channel = LTE_MCU_I2C_CHANNEL;
#endif
        //turn on lte board mcu
        i2c_write_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x03, 0x00);
        msleep(20);
        //0x01:wifi 0x02:lte 0x04:power 0x08:lte_vbus
        value = 0x0F;
        debug_line("turn on for lte and wifi: 0x%.2x", value);
        i2c_write_reg(i2c_channel, LTE_MCU_I2C_SLAVE_ADDR, 0x03, value);//power on
    }
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
    value &= ~0x02;
    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
    app_helper.gpio_set(GPIO_PIN_75, 0);//4g_en
    app_helper.gpio_set(GPIO_PIN_41, 0);//4g_vbus_en
    msleep(10);
    //turn on 4g
    app_helper.gpio_set(GPIO_PIN_37, 1);//4g_pwr_key
    i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0f, &value);
    value |= 0x02;
    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x0e, value);
    app_helper.gpio_set(GPIO_PIN_75, 1);//4g_en
    app_helper.gpio_set(GPIO_PIN_41, 1);//4g_vbus_en
    //enable pwr_key
    app_helper.gpio_set(GPIO_PIN_37, 0);//4g_pwr_key
    msleep(500);
    app_helper.gpio_set(GPIO_PIN_37, 1);//4g_pwr_key
#endif
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    //turn on 4g
    app_helper.gpio_set(GPIO_PIN_30, 1);//4g_reset_key
    msleep(10);
    app_helper.gpio_set(GPIO_PIN_30, 0);//4g_reset_key
    msleep(50);
    //enable pwr_key
    app_helper.gpio_set(GPIO_PIN_29, 1);//4g_pwr_key
    msleep(500);
    app_helper.gpio_set(GPIO_PIN_29, 0);//4g_pwr_key
#endif

    return 0;
}

int app_util_check_external_sensor_device(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    unsigned char value = 0;
    unsigned char left_connected = 0, right_connected = 0;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (app_helper.check_gc2053_left_connected()) {
        app_helper.external_camera_connected = 1;
        pSvcUserPref->ExternalCamera.ImageRotation = IMAGE_ROTATION_HORIZONTAL_VERTICAL;
        if (pSvcUserPref->ExternalCamera.Enable) {
            debug_line("detect left gc2053 connected and enabled");
            app_helper.external_camera_enabled = 1;
            left_connected = 1;
            sensor_left_connected = 1;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            app_helper.gpio_set(GC2053_SEL_PIN, 1);
#endif
        } else {
            debug_line("detect left gc2053 connected, but not enable");
            app_helper.external_camera_enabled = 0;
        }
    } else if (app_helper.check_gc2053_right_connected()) {
        pSvcUserPref->ExternalCamera.ImageRotation = IMAGE_ROTATION_NONE;
        app_helper.external_camera_connected = 1;
        if (pSvcUserPref->ExternalCamera.Enable) {
            debug_line("detect right gc2053 connected and enabled");
            app_helper.external_camera_enabled = 1;
            right_connected = 1;
            sensor_right_connected = 1;
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            app_helper.gpio_set(GC2053_SEL_PIN, 0);
#endif
        } else {
            debug_line("detect left gc2053 connected, but not enable");
            app_helper.external_camera_enabled = 0;
        }
    } else {
        debug_line("no gc2053 connected");
        app_helper.external_camera_connected = 0;
        app_helper.external_camera_enabled = 0;
    }
#if defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (left_connected || right_connected) {
        app_helper.gpio_set(GC2053_OEB_PIN, 0);
    } else {
        app_helper.gpio_set(GC2053_OEB_PIN, 1);
    }
#endif
    if (left_connected) {
        value |= 0x01;
    }
    if (right_connected) {
        value |= 0x02;
    }
    value |= 0x04;//switch usb to left default
    debug_line("turn on sensor power: 0x%.2x", value);
    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);//turn on sensor
#else
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (app_helper.check_external_camera_connected()) {
        app_helper.external_camera_connected = 1;
        if (pSvcUserPref->ExternalCamera.Enable) {
            debug_line("detect gc2053 connected and enabled");
            app_helper.external_camera_enabled = 1;
        } else {
            debug_line("detect gc2053 connected, but not enable");
            app_helper.external_camera_enabled = 0;
        }
    } else {
        debug_line("no gc2053 connected");
        app_helper.external_camera_connected = 0;
        app_helper.external_camera_enabled = 0;
    }
#endif
    return 0;
}

int app_util_init(void)
{
    sd_state = app_helper.power_on_with_sd_card;
    power_state = Pmic_CheckUsbConnected();
    if (power_state == 0) {
        if (Pmic_GetBootReason() == BOOT_REASON_VBUS_TRIGGER) {
            debug_line("vbus trigger power on, but no vbus connect!!!");
            app_msg_queue_send(APP_MSG_ID_USB_REMOVE, 0, 0, 0);
        }
    }
    app_timer_register(TIMER_1HZ, app_util_sys_timer_handler);

    return 0;
}

int app_util_deinit(void)
{
    app_timer_unregister(TIMER_1HZ, app_util_sys_timer_handler);

    return 0;
}

int app_util_do_hard_reset(void)
{
    Pmic_SetSramRegister(0x01);
    Pmic_NormalSoftReset();

    return 0;
}

int app_util_do_factory_reset(void)
{
    AmbaFS_Format('a');
    //AmbaFS_Format('b');
    AmbaFS_Format(SD_SLOT[0]);
    AmbaNAND_SwitchBoot(1);
    Pmic_SetSramRegister(0x10);
    Pmic_NormalSoftReset();

    return 0;
}

int app_util_check_external_camera_enabled(void)
{
    return app_helper.external_camera_enabled;
}

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
static void *external_camera_monitor_task_entry(void *arg)
{
    unsigned int last_irq_count = 0, cur_irq_count = 0;

    debug_line("external_camera_monitor_task running...");
    while (1) {
        last_irq_count = AmbaDiag_GetIrqCounter(168);
        msleep(100);
        cur_irq_count = AmbaDiag_GetIrqCounter(168);
        if (cur_irq_count <= last_irq_count) {
            if (rec_dvr_hot_config_busy_check() == 0) {
                if (sensor_left_connected) {
                    unsigned char value = 0x00;
                    debug_line("left external_camera lost--------------------------------------");
                    sensor_left_connected = 0;
                    app_helper.external_camera_connected = 0;
                    app_helper.external_camera_enabled = 0;
                    app_helper.external_camera_auto_record = 0;
                    ir_task_set_check_enable(0);
                    SvcRecMain_Stop(0x4, 0);
                    i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
                    value &= ~0x01;
                    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
                }
                if (sensor_right_connected) {
                    unsigned char value = 0x00;
                    debug_line("right external_camera lost--------------------------------------");
                    sensor_right_connected = 0;
                    app_helper.external_camera_connected = 0;
                    app_helper.external_camera_enabled = 0;
                    app_helper.external_camera_auto_record = 0;
                    ir_task_set_check_enable(0);
                    SvcRecMain_Stop(0x4, 0);
                    i2c_read_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x04, &value);
                    value &= ~0x02;
                    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, value);
                }
                break;
            }
        }
    }

    return NULL;
}

static AMBA_KAL_TASK_t external_camera_monitor_task;
#define EXTERNAL_CAMERA_TASK_STACK_SIZE (32 * 1024)
static unsigned char external_camera_monitor_task_stack[EXTERNAL_CAMERA_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));

int app_util_external_camera_monitor_start(void)
{
    int ret = 0;

    ret = AmbaKAL_TaskCreate(&external_camera_monitor_task,
                            "external_camera_monitor_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)external_camera_monitor_task_entry,
                            0,
                            external_camera_monitor_task_stack,
                            EXTERNAL_CAMERA_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("external_camera_monitor_task create fail");
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&external_camera_monitor_task, 0x02);
    AmbaKAL_TaskResume(&external_camera_monitor_task);

    return 0;
}
#endif

#if defined(CONFIG_BSP_CV25_NEXAR_D080)
static unsigned int value1 = 50, value2 = 200;
int app_util_logo_led_fade_set_param(unsigned int v1, unsigned int v2)
{
    value1 = v1;
    value2 = v2;

    return 0;
}

static void *logo_led_fade_task_entry(void *arg)
{
    unsigned int i = 0, value = 0x00;
    unsigned char brightness_array[] = {
#if 1
        0x01,0x08,0x1a,0x34,0x55,0x7d,
        0x7d,0x55,0x34,0x1a,0x08,0x01
#else
        0x00,
        0x04,
        0x08,
        0x12,
        0x16,
        0x1A,
        0x20,
#endif
    };

    debug_line("logo_led_fade_task_entry running...");
    while (1) {
        //set current
        for (i = 0; i < sizeof(brightness_array); i++) {
            value = brightness_array[i];
#if 1
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x6E, value >> 2);
#else
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4D, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4E, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x4F, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x50, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x51, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x52, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x53, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x54, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x55, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x56, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x57, value);
            i2c_write_reg(LOGO_LED_I2C_CHANNEL, LOGO_LED_I2C_SLAVE_ADDR, 0x58, value);
#endif
            msleep(value1);
        }
        msleep(value2);
    }

    return NULL;
}

static AMBA_KAL_TASK_t logo_led_fade_task;
#define LOGO_LED_FADE_TASK_STACK_SIZE (32 * 1024)
static unsigned char logo_led_fade_task_stack[LOGO_LED_FADE_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));

int app_util_logo_led_fade_start(void)
{
    int ret = 0;

    ret = AmbaKAL_TaskCreate(&logo_led_fade_task,
                            "logo_led_fade_task",
                            160,
                            (AMBA_KAL_TASK_ENTRY_f)logo_led_fade_task_entry,
                            0,
                            logo_led_fade_task_stack,
                            LOGO_LED_FADE_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("logo_led_fade_task create fail");
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&logo_led_fade_task, 0x02);
    AmbaKAL_TaskResume(&logo_led_fade_task);

    return 0;
}
#endif

