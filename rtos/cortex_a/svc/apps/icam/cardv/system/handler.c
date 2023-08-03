#include <app_base.h>
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
#include "AmbaSensor_IMX415.h"
#include "AmbaSensor_GC2053.h"
#endif

extern void t_app_init(void);
static void *app_handler_entry(void *argv);

static void svc_sys_stat_callback(unsigned int stat_idx, void *p_info)
{
    AmbaMisra_TouchUnused(&stat_idx);
    AmbaMisra_TouchUnused(p_info);
    if (stat_idx == SVC_APP_STAT_STG) {
        SVC_APP_STAT_STG_s *pSdStatus = NULL;
        AmbaMisra_TypeCast32(&pSdStatus, &p_info);
        if (pSdStatus->Status == SVC_APP_STAT_SD_IDLE) {
            app_msg_queue_send(APP_MSG_ID_SD_IDLE, 0, 0, 0);
        } else if (pSdStatus->Status == SVC_APP_STAT_SD_READY) {
            app_msg_queue_send(APP_MSG_ID_SD_INSERT, 0, 0, 0);
        } else if (pSdStatus->Status == SVC_APP_STAT_SD_NONE) {
            app_msg_queue_send(APP_MSG_ID_SD_REMOVE, 0, 0, 0);
        }
    } else if (stat_idx == SVC_APP_STAT_LINUX) {
        SVC_APP_STAT_LINUX_s *pLinuxStatus = NULL;
        AmbaMisra_TypeCast32(&pLinuxStatus, &p_info);
        if ((pLinuxStatus->Status & SVC_APP_STAT_LINUX_READY) > 0U) {
            app_msg_queue_send(APP_MSG_ID_LINUX_BOOT_DONE, 0, 0, 0);
        } else if ((pLinuxStatus->Status & SVC_APP_STAT_WIFI_READY) > 0U) {
            app_msg_queue_send(APP_MSG_ID_WIFI_BOOT_DONE, 0, 0, 0);
        } else if ((pLinuxStatus->Status & SVC_APP_STAT_BT_READY) > 0U) {
            app_msg_queue_send(APP_MSG_ID_BT_BOOT_DONE, 0, 0, 0);
        } else if ((pLinuxStatus->Status & SVC_APP_STAT_LTE_ON) > 0U) {
            app_msg_queue_send(APP_MSG_ID_LTE_ON, 0, 0, 0);
        } else if ((pLinuxStatus->Status & SVC_APP_STAT_LTE_READY) > 0U) {
            app_msg_queue_send(APP_MSG_ID_LTE_BOOT_DONE, 0, 0, 0);
        } else if ((pLinuxStatus->Status & SVC_APP_STAT_USB_WIFI_READY) > 0U) {
            app_msg_queue_send(APP_MSG_ID_USB_WIFI_BOOT_DONE, 0, 0, 0);
        }
    }
}

static void gpio_init(void)
{
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    //init gc4653 power order
    app_helper.gpio_set(GPIO_PIN_0, 1);
    msleep(10);
    app_helper.gpio_set(GPIO_PIN_95, 1);
    msleep(10);
    app_helper.gpio_set(GPIO_PIN_16, 1);
    //init gc2653 power order
    app_helper.gpio_set(GPIO_PIN_107, 1);
    msleep(5);
    app_helper.gpio_set(GPIO_PIN_106, 1);
    msleep(5);
    app_helper.gpio_set(GPIO_PIN_15, 1);
#endif
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    //init led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x03, 0x15);//disable charge led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x01, 0x01);//turn on led
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x30, 0x07);//enable 3 leds
#endif
#if defined(CONFIG_BSP_CV25_NEXAR_D080)
    app_helper.set_logo_led_color(0, 255, 255, 255);
    app_helper.set_logo_led_color(1, 255, 255, 255);
    app_helper.set_logo_led_color(2, 255, 255, 255);
    app_helper.set_logo_led_color(3, 255, 255, 255);
    app_helper.set_logo_led_fade(1);
#endif
}

static int check_a_disk_accessable(const char *dir, const char *path)
{
    struct stat fstat;

    if (AmbaFS_ChangeDir(dir) != AMBA_FS_ERR_NONE) {
        debug_line("can not access %s", dir);
        if (AmbaFS_MakeDir(dir) != AMBA_FS_ERR_NONE) {
            debug_line("create %s failed", dir);
            return 0;
        }
    }
    if (stat(path, &fstat) == 0) {        
        debug_line("%s exist", path);
        return 1;
    }
    debug_line("%s not exist. create it", path);
    app_helper.touch(path);
    if (stat(path, &fstat) == 0) {
        debug_line("%s exist after create", path);
        return 1;
    }
    debug_line("%s still not exist", path);

    return 0;
}

static void wait_disk_ready(void)
{
#define DIR    "a:\\pref"
#define PATH   DIR"\\.exist"

    //check a disk
    if (check_a_disk_accessable(DIR, PATH) == 0) {
        unsigned int NvmID;
        debug_line("can not access a disk, format it.");
        AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_FAT_DRIVE_A);
        AmbaNVM_ErasePartition(NvmID, 1U, AMBA_USER_PARTITION_FAT_DRIVE_A, AMBA_KAL_WAIT_FOREVER);
        if (NAND_ERR_NONE != AmbaNAND_FtlInit(AMBA_USER_PARTITION_FAT_DRIVE_A)) {
            debug_line("Drive A isn't enabled", 0U, 0U);
        }
        AmbaFS_Format(PATH[0]);
        AmbaFS_MakeDir(DIR);
        if (check_a_disk_accessable(DIR, PATH) == 0) {
            debug_line("can not access a disk, this should not happen.");
        } else {
            debug_line("drive a is ready to use");
        }
    } else {
        debug_line("drive a is ready to use");
    }
#if 0
    //check b disk
    if (AmbaFS_ChangeDir("b:\\") != AMBA_FS_ERR_NONE) {
        debug_line("can not access b disk, format it.");
        AmbaFS_Format('b');
    }
    debug_line("drive b is ready to use");    
#endif
#if defined(CONFIG_ENABLE_EMMC_BOOT)
    if (AmbaFS_ChangeDir(EMMC_SLOT":\\") != AMBA_FS_ERR_NONE) {
        debug_line("can not access "EMMC_SLOT" disk, format it.");
        AmbaFS_Format(EMMC_SLOT[0]);
    }
    debug_line("drive "EMMC_SLOT" is ready to use");
#endif
}

void app_init_res_cfg(SVC_RES_CFG_s *resCfg)
{
    int i = 0;
    SVC_USER_PREF_s *pSvcUserPref = NULL;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    int is_above_1080p = 0;
#endif

    debug_line("%s enter", __func__);
    SvcUserPref_Get(&pSvcUserPref);
    //handler internal camera
    if (app_helper.internal_camera_enabled) {
        stream_size_s internal_frame_sizes[2];
        if (dqa_test_script.usb_uvc_mode == 1) {
            internal_frame_sizes[0].width = pSvcUserPref->InternalCamera.UvcStream.width;
            internal_frame_sizes[0].height = pSvcUserPref->InternalCamera.UvcStream.height;
        } else {
            internal_frame_sizes[0].width = pSvcUserPref->InternalCamera.MainStream.width;
            internal_frame_sizes[0].height = pSvcUserPref->InternalCamera.MainStream.height;
        }
        internal_frame_sizes[1].width = pSvcUserPref->InternalCamera.SecStream.width;
        internal_frame_sizes[1].height = pSvcUserPref->InternalCamera.SecStream.height;
        for (i = 0; i < 2; i++) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            if (internal_frame_sizes[i].width > 3840) {
                internal_frame_sizes[i].width = 3840;
            }
            if (internal_frame_sizes[i].height > 2160) {
                internal_frame_sizes[i].height = 2160;
            }
#elif defined(CONFIG_BSP_CV25_NEXAR_D080)
            if (internal_frame_sizes[i].width > 2560) {
                internal_frame_sizes[i].width = 2560;
            }
            if (internal_frame_sizes[i].height > 1440) {
                internal_frame_sizes[i].height = 1440;
            }
#elif defined(CONFIG_BSP_H32_NEXAR_D081)
            if (internal_frame_sizes[i].width > 1984) {
                internal_frame_sizes[i].width = 1984;
            }
            if (internal_frame_sizes[i].height > 1984) {
                internal_frame_sizes[i].height = 1984;
            }
#endif
            if (internal_frame_sizes[i].width < 640) {
                internal_frame_sizes[i].width = 640;
            }
            if (internal_frame_sizes[i].height < 360) {
                internal_frame_sizes[i].height = 360;
            }
        }
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (internal_frame_sizes[0].width > 1920 || internal_frame_sizes[0].height > 1080) {
            is_above_1080p = 1;
        }
#if 1
        if (is_above_1080p) {
            resCfg->VinCfg[0].CapWin.Width = 3840U;
            resCfg->VinCfg[0].CapWin.Height = 2160U;
            resCfg->FovCfg[0].RawWin.Width = 3840U;
            resCfg->FovCfg[0].RawWin.Height = 2160U;
            resCfg->FovCfg[0].MainWin.Width = 3840U;
            resCfg->FovCfg[0].MainWin.Height = 2160U;
            resCfg->RecStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width = 3840U;
            resCfg->RecStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height = 2160U;
            resCfg->RecStrm[1].StrmCfg.ChanCfg[0].SrcWin.Width = 3840U;
            resCfg->RecStrm[1].StrmCfg.ChanCfg[0].SrcWin.Height = 2160U;

            resCfg->SensorCfg[0][0].SensorMode = IMX415_3840_2160_25P;
            resCfg->VinCfg[0].FrameRate.TimeScale = 25000U;
            resCfg->VinCfg[0].FrameRate.NumUnitsInTick = 1000U;
            //resCfg->DispStrm[0].FrameRate.TimeScale = 50000U;
            //resCfg->DispStrm[0].FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[0].RecSetting.FrameRate.TimeScale = 25000U;
            resCfg->RecStrm[0].RecSetting.FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[1].RecSetting.FrameRate.TimeScale = 25000U;
            resCfg->RecStrm[1].RecSetting.FrameRate.NumUnitsInTick = 1000U;

            resCfg->CvFlow[0].InputCfg.Input[0].FrameWidth = 3840U;
            resCfg->CvFlow[0].InputCfg.Input[0].FrameHeight = 2160U;
            pSvcUserPref->InternalCamera.Fps = 25;
            pSvcUserPref->ExternalCamera.Fps = 25;
        } else {
            resCfg->VinCfg[0].CapWin.Width = 1920U;
            resCfg->VinCfg[0].CapWin.Height = 1080U;
            resCfg->FovCfg[0].RawWin.Width = 1920U;
            resCfg->FovCfg[0].RawWin.Height = 1080U;
            resCfg->FovCfg[0].MainWin.Width = 1920U;
            resCfg->FovCfg[0].MainWin.Height = 1080U;
            resCfg->RecStrm[0].StrmCfg.ChanCfg[0].SrcWin.Width = 1920U;
            resCfg->RecStrm[0].StrmCfg.ChanCfg[0].SrcWin.Height = 1080U;
            resCfg->RecStrm[1].StrmCfg.ChanCfg[0].SrcWin.Width = 1920U;
            resCfg->RecStrm[1].StrmCfg.ChanCfg[0].SrcWin.Height = 1080U;

            resCfg->SensorCfg[0][0].SensorMode = IMX415_1920_1080_30P_BINNING;
            resCfg->VinCfg[0].FrameRate.TimeScale = 30000U;
            resCfg->VinCfg[0].FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[0].RecSetting.FrameRate.TimeScale = 30000U;
            resCfg->RecStrm[0].RecSetting.FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[1].RecSetting.FrameRate.TimeScale = 30000U;
            resCfg->RecStrm[1].RecSetting.FrameRate.NumUnitsInTick = 1000U;

            resCfg->CvFlow[0].InputCfg.Input[0].FrameWidth = 1920U;
            resCfg->CvFlow[0].InputCfg.Input[0].FrameHeight = 1080U;
            pSvcUserPref->InternalCamera.Fps = 30;
            pSvcUserPref->ExternalCamera.Fps = 30;
        }
#endif
#endif
        resCfg->FovCfg[0].PipeCfg.RawCompression = (pSvcUserPref->PivOnOff == OPTION_ON) ? 1 : 0;

        resCfg->RecStrm[0].RecSetting.SrcBits = SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO;
        resCfg->RecStrm[1].RecSetting.SrcBits = SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO;
        resCfg->RecStrm[0].RecSetting.DestBits = SVC_REC_DST_FILE;
        resCfg->RecStrm[1].RecSetting.DestBits = 0;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (is_above_1080p == 0) {
            resCfg->RecStrm[0].RecSetting.DestBits |= SVC_REC_DST_EMGEVT;
        }
#endif
        resCfg->RecStrm[0].RecSetting.DestBits &= ~SVC_REC_DST_NET;
#if defined(CONFIG_AMBA_RTSP)
        resCfg->RecStrm[1].RecSetting.DestBits |= SVC_REC_DST_NET;
#else
        resCfg->RecStrm[1].RecSetting.DestBits &= ~SVC_REC_DST_NET;
#endif
        for (i = 0; i < 2; i++) {
            resCfg->RecStrm[i].StrmCfg.Win.Width = internal_frame_sizes[i].width;
            resCfg->RecStrm[i].StrmCfg.Win.Height = internal_frame_sizes[i].height;
            resCfg->RecStrm[i].StrmCfg.MaxWin.Width = internal_frame_sizes[i].width;
            resCfg->RecStrm[i].StrmCfg.MaxWin.Height = internal_frame_sizes[i].height;
            resCfg->RecStrm[i].StrmCfg.ChanCfg[0].DstWin.Width = internal_frame_sizes[i].width;
            resCfg->RecStrm[i].StrmCfg.ChanCfg[0].DstWin.Height = internal_frame_sizes[i].height;
        }
        if (dqa_test_script.usb_uvc_mode == 1) {
            resCfg->RecStrm[0].RecSetting.DestBits = SVC_REC_DST_NET;
            resCfg->RecStrm[0].RecSetting.BitRate = 20 * (UINT64)1000000U;
            resCfg->RecNum = 1;
            resCfg->RecBits = 0x1;
        } else {
            resCfg->RecStrm[0].RecSetting.BitRate = pSvcUserPref->InternalCamera.MainStream.bitrate * (UINT64)1000000U;
        }
        resCfg->RecStrm[1].RecSetting.BitRate = pSvcUserPref->InternalCamera.SecStream.bitrate * (UINT64)1000000U;
        resCfg->RecStrm[0].RecSetting.N = pSvcUserPref->InternalCamera.MainStream.gop_size;
        resCfg->RecStrm[1].RecSetting.N = pSvcUserPref->InternalCamera.SecStream.gop_size;
        if (pSvcUserPref->InternalCamera.MainStream.is_h265) {
            resCfg->RecStrm[0].RecSetting.MVInfoFlag = 1U;
            resCfg->RecStrm[0].RecSetting.BitRate /= 2;
        } else {
            resCfg->RecStrm[0].RecSetting.MVInfoFlag = 0U;
        }
        if (pSvcUserPref->InternalCamera.SecStream.is_h265 && dqa_test_script.usb_uvc_mode != 1) {
            resCfg->RecStrm[1].RecSetting.MVInfoFlag = 1U;
            resCfg->RecStrm[1].RecSetting.BitRate /= 2;
        } else {
            resCfg->RecStrm[1].RecSetting.MVInfoFlag = 0U;
        }
    } else {
        app_helper.internal_camera_auto_record = 0;
    }

    //hander external camera
    if (app_helper.external_camera_enabled) {
        stream_size_s external_frame_sizes[2];
        int rec_stream_start_index = 2;
        int fov_index = 1;
        if (app_helper.internal_camera_enabled == 0) {
            rec_stream_start_index = 0;
            fov_index = 0;
        }
        if (dqa_test_script.usb_uvc_mode == 2) {
            external_frame_sizes[0].width = pSvcUserPref->ExternalCamera.UvcStream.width;
            external_frame_sizes[0].height = pSvcUserPref->ExternalCamera.UvcStream.height;
        } else {
            external_frame_sizes[0].width = pSvcUserPref->ExternalCamera.MainStream.width;
            external_frame_sizes[0].height = pSvcUserPref->ExternalCamera.MainStream.height;
        }
        external_frame_sizes[1].width = pSvcUserPref->ExternalCamera.SecStream.width;
        external_frame_sizes[1].height = pSvcUserPref->ExternalCamera.SecStream.height;
        for (i = 0; i < 2; i++) {
            if (external_frame_sizes[i].width > 1280) {
                external_frame_sizes[i].width = 1280;
            }
            if (external_frame_sizes[i].height > 720) {
                external_frame_sizes[i].height = 720;
            }
            if (external_frame_sizes[i].width < 640) {
                external_frame_sizes[i].width = 640;
            }
            if (external_frame_sizes[i].height < 360) {
                external_frame_sizes[i].height = 360;
            }
        }

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
        if (is_above_1080p) {
            resCfg->SensorCfg[1][0].SensorMode = GC2053_1920_1080_25P;
            resCfg->VinCfg[1].FrameRate.TimeScale = 25000U;
            resCfg->VinCfg[1].FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[rec_stream_start_index].RecSetting.FrameRate.TimeScale = 25000U;
            resCfg->RecStrm[rec_stream_start_index].RecSetting.FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.FrameRate.TimeScale = 25000U;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.FrameRate.NumUnitsInTick = 1000U;
        } else {
            resCfg->SensorCfg[1][0].SensorMode = GC2053_1920_1080_30P;
            resCfg->VinCfg[1].FrameRate.TimeScale = 30000U;
            resCfg->VinCfg[1].FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[rec_stream_start_index].RecSetting.FrameRate.TimeScale = 30000U;
            resCfg->RecStrm[rec_stream_start_index].RecSetting.FrameRate.NumUnitsInTick = 1000U;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.FrameRate.TimeScale = 30000U;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.FrameRate.NumUnitsInTick = 1000U;
        }
        if (1) {//(is_above_1080p == 0) {
            resCfg->VinCfg[1].CapWin.Width = 1920U;
            resCfg->VinCfg[1].CapWin.Height = 1080U;
            resCfg->FovCfg[fov_index].RawWin.Width = 1920U;
            resCfg->FovCfg[fov_index].RawWin.Height = 1080U;
            resCfg->FovCfg[fov_index].MainWin.Width = 1920U;
            resCfg->FovCfg[fov_index].MainWin.Height = 1080U;
            resCfg->RecStrm[rec_stream_start_index].StrmCfg.ChanCfg[0].SrcWin.Width = 1920U;
            resCfg->RecStrm[rec_stream_start_index].StrmCfg.ChanCfg[0].SrcWin.Height = 1080U;
            resCfg->RecStrm[rec_stream_start_index + 1].StrmCfg.ChanCfg[0].SrcWin.Width = 1920U;
            resCfg->RecStrm[rec_stream_start_index + 1].StrmCfg.ChanCfg[0].SrcWin.Height = 1080U;
        }
#endif
        resCfg->FovCfg[fov_index].PipeCfg.RawCompression = (pSvcUserPref->PivOnOff == OPTION_ON) ? 1 : 0;

        resCfg->RecStrm[rec_stream_start_index].RecSetting.SrcBits = SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO;
        resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.SrcBits = SVC_REC_SRC_VIDEO | SVC_REC_SRC_AUDIO;
        if (app_helper.external_camera_auto_record) {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.DestBits = SVC_REC_DST_FILE;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.DestBits = 0;
        } else {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.DestBits = 0;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.DestBits = 0;
        }
        if (app_helper.internal_camera_enabled) {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.DestBits |= SVC_REC_DST_NET;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.DestBits &= ~SVC_REC_DST_NET;
        } else {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.DestBits &= ~SVC_REC_DST_NET;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.DestBits |= SVC_REC_DST_NET;
        }
        for (i = 0; i < 2; i++) {
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.Win.Width = external_frame_sizes[i].width;
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.Win.Height = external_frame_sizes[i].height;
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.MaxWin.Width = external_frame_sizes[i].width;
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.MaxWin.Height = external_frame_sizes[i].height;
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.ChanCfg[0].DstWin.Width = external_frame_sizes[i].width;
             resCfg->RecStrm[i + rec_stream_start_index].StrmCfg.ChanCfg[0].DstWin.Height = external_frame_sizes[i].height;
        }
        resCfg->RecStrm[rec_stream_start_index].RecSetting.BitRate = pSvcUserPref->ExternalCamera.MainStream.bitrate * (UINT64)1000000U;
        if (dqa_test_script.usb_uvc_mode == 2) {
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.BitRate = 0;
        } else {
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.BitRate = pSvcUserPref->ExternalCamera.SecStream.bitrate * (UINT64)1000000U;
        }
        resCfg->RecStrm[rec_stream_start_index].RecSetting.N = pSvcUserPref->ExternalCamera.MainStream.gop_size;
        resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.N = pSvcUserPref->ExternalCamera.SecStream.gop_size;
        if (pSvcUserPref->ExternalCamera.MainStream.is_h265) {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.MVInfoFlag = 1U;
            resCfg->RecStrm[rec_stream_start_index].RecSetting.BitRate /= 2;
        } else {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.MVInfoFlag = 0U;
        }
        if (pSvcUserPref->ExternalCamera.SecStream.is_h265 && dqa_test_script.usb_uvc_mode != 2) {
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.MVInfoFlag = 1U;
            resCfg->RecStrm[rec_stream_start_index + 1].RecSetting.BitRate /= 2;
        } else {
            resCfg->RecStrm[rec_stream_start_index].RecSetting.MVInfoFlag = 0U;
        }
    }

    if (pSvcUserPref->CvOnOff == OPTION_ON) {
        if (app_helper.internal_camera_enabled && app_helper.external_camera_enabled == 0) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            resCfg->FovCfg[0].PyramidBits = is_above_1080p ? 0x14 : 0x05;
#else
            resCfg->FovCfg[0].PyramidBits = 0x14;
#endif
            resCfg->CvFlowNum = 1;
            resCfg->CvFlowBits = 0x01;
        } else if (app_helper.internal_camera_enabled && app_helper.external_camera_enabled) {
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
            resCfg->FovCfg[0].PyramidBits = is_above_1080p ? 0x14 : 0x05;
#else
            resCfg->FovCfg[0].PyramidBits = 0x14;
#endif
            resCfg->FovCfg[1].PyramidBits = 0x06;
            resCfg->CvFlowNum = 2;
            resCfg->CvFlowBits = 0x03;
        } else {
            resCfg->CvFlowNum = 0;
            resCfg->CvFlowBits = 0x00;
        }
    }
    if (pSvcUserPref->PivOnOff == OPTION_ON) {
        resCfg->StillCfg.EnableStill = 1;
    } else {
        resCfg->StillCfg.EnableStill = 0;
    }
    debug_line("%s leave", __func__);
}

static int init_finished = 0;
int app_init_check(void)
{
    return init_finished;
}

int app_pre_init(void)
{
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    app_helper_init();
#endif
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
    dqa_test_script_init();
#endif
    //init gpio
    gpio_init();

    //dvr init
    rec_dvr_init();
    //basic init
    Pmic_Init();
    app_helper.with_acc = app_helper.acc_connected = Pmic_GetAccState();
    debug_line("power on %s acc ", app_helper.with_acc ? "with" : "without");
    debug_line("wakeup_source: %s", app_helper.wakeup_source_2_str(Pmic_GetBootReason()));
    battery_power_on_check();

    return 0;
}

int app_init(void)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    SVC_USER_PREF_CUSTOM_s *pSvcUserPrefCustom = NULL;
    unsigned int CtrlID = 0;
    SVC_APP_STAT_STG_s SdStatus;
    int value = 0;

    AmbaADC_Config(10000);
    //set watchdog to 20s
#if defined(CONFIG_PMIC_CHIPON)
    Pmic_Write(0x07, 20);
#endif
    value = Pmic_GetSramRegister();
    if (value != 0) {
        if (value & 0x01) {
            app_helper.hard_reset_flag = 1;
        }
        if (value & 0x02) {
            app_helper.factory_reset_flag = 1;
        }
        if (value & 0x10) {
            app_helper.factory_reset_flag = 2;//api reset
        }
        value &= 0xec;
        Pmic_SetSramRegister(value);
    }
    metadata_load();
    app_timer_init();
    t_app_init();
    //init gnss parser
#if defined(CONFIG_APP_FLOW_AONI_GNSS_MODULE)
    gnss_parser_init();
#endif

    //hardware init
    //load setting
    wait_disk_ready();
    user_setting_init();
    SvcUserPref_Get(&pSvcUserPref);

#if 0
    pSvcUserPref->GnssRecord = 1;
    pSvcUserPref->ImuRecord = 1;
    pSvcUserPref->InternalCamera.AutoRecord = 1;
    pSvcUserPref->ExternalCamera.AutoRecord = 1;
    pSvcUserPref->PivOnOff = OPTION_ON;
    pSvcUserPref->InternalCamera.MainStream.width = 3840;
    pSvcUserPref->InternalCamera.MainStream.width = 2160;
#endif

    if (strlen(pSvcUserPref->WifiSetting.ApSSID) <= 0) {
        char post_ssid[32] = {0};
        metadata_s *data;
        metadata_get(&data);
        memset(post_ssid, 0, sizeof(post_ssid));
        if (strlen((char *)data->SerialNumber) >= 6) {
            int i = 0;
            memcpy(post_ssid, data->SerialNumber + strlen((char *)data->SerialNumber) - 6, 6);
            for (i = 0; i < 6; i++) {
                post_ssid[i] = tolower(post_ssid[i]);
            }
            snprintf(pSvcUserPref->WifiSetting.ApSSID, sizeof(pSvcUserPref->WifiSetting.ApSSID) - 1, "%s", post_ssid);
        }
        if (strlen(pSvcUserPref->WifiSetting.ApPassword) <= 0) {
            snprintf(pSvcUserPref->WifiSetting.ApPassword, sizeof(pSvcUserPref->WifiSetting.ApPassword) - 1, "%s", WIFI_AP_DEFAULT_PASSWORD);
        }
    }
    if (strlen(pSvcUserPref->WifiSetting.ApIP) <= 0) {
        snprintf(pSvcUserPref->WifiSetting.ApIP, sizeof(pSvcUserPref->WifiSetting.ApIP) - 1, "%s", WIFI_AP_IP);
    }

    if (strlen(pSvcUserPref->UsbWifiSetting.ApSSID) <= 0) {
        char post_ssid[32] = {0};
        metadata_s *data;
        metadata_get(&data);
        memset(post_ssid, 0, sizeof(post_ssid));
        if (strlen((char *)data->SerialNumber) >= 6) {
            int i = 0;
            memcpy(post_ssid, data->SerialNumber + strlen((char *)data->SerialNumber) - 6, 6);
            for (i = 0; i < 6; i++) {
                post_ssid[i] = tolower(post_ssid[i]);
            }
            snprintf(pSvcUserPref->UsbWifiSetting.ApSSID, sizeof(pSvcUserPref->UsbWifiSetting.ApSSID) - 1, "%s", post_ssid);
        }
        if (strlen(pSvcUserPref->UsbWifiSetting.ApPassword) <= 0) {
            snprintf(pSvcUserPref->UsbWifiSetting.ApPassword, sizeof(pSvcUserPref->UsbWifiSetting.ApPassword) - 1, "%s", WIFI_AP_DEFAULT_PASSWORD);
        }
    }
    if (strlen(pSvcUserPref->UsbWifiSetting.ApIP) <= 0) {
        snprintf(pSvcUserPref->UsbWifiSetting.ApIP, sizeof(pSvcUserPref->UsbWifiSetting.ApIP) - 1, "%s", WIFI_AP_IP);
    }
    //pSvcUserPref->ImuRecord = 0;
    //pSvcUserPref->GnssRecord = 0;
    SvcControlTask_SetCvRun((pSvcUserPref->CvOnOff == OPTION_ON) ? 1 : 0);
    AmbaFS_SetUtcOffsetMinutes(pSvcUserPref->TimeZone / 60);
    if (pSvcUserPref->CvOnOff == OPTION_ON) {
        pSvcUserPref->CVBooFlag = 6;
    }

    if (pSvcUserPref->UsbMassStorage == OPTION_ON) {
        pSvcUserPref->UsbMassStorage = OPTION_OFF;
        pSvcUserPref->OperationMode = 0;
        app_helper.usb_mass_storage_on = 1;
    } else {
        app_helper.usb_mass_storage_on = 0;
    }
    SvcUserPrefCustom_Get(&pSvcUserPrefCustom);
    init_finished = 1;

#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    i2c_write_reg(MAIN_MCU_I2C_CHANNEL, MAIN_MCU_I2C_SLAVE_ADDR, 0x03, 0x00);//power off all
    msleep(50);
#endif
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    if (app_helper.usb_mass_storage_on == 0) {
        app_util_check_external_sensor_device();
    }
#endif
    app_helper.internal_camera_auto_record = pSvcUserPref->InternalCamera.AutoRecord;
    app_helper.external_camera_auto_record = pSvcUserPref->ExternalCamera.AutoRecord;
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    app_helper.left_addon_type = app_helper.get_left_addon_type();
    app_helper.right_addon_type = app_helper.get_right_addon_type();
#endif
    if (app_helper.external_camera_enabled == 0) {
        pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_INTERNAL;
    }
    if (strlen(pSvcUserPref->VideoEncryption.encryption_key) == 0) {
        snprintf(pSvcUserPref->VideoEncryption.encryption_key, sizeof(pSvcUserPref->VideoEncryption.encryption_key) - 1, "%s", "16709222346338273533791789265665");
    }
    //AmbaFS_SetVideoEncryptionKey(pSvcUserPref->VideoEncryption.encryption_key);
    //AmbaFS_SetVideoEncryptionDebug(pSvcUserPref->VideoEncryption.debug);
    //AmbaFS_SetVideoEncryptionEnable(pSvcUserPref->VideoEncryption.enable);
    user_setting_save();

#if defined(CONFIG_ENABLE_IMU)
    Imu_Init();
    Imu_SetCalibrationData(&(pSvcUserPrefCustom->ImuCalibrationData));
    Imu_SetAccelPara(IMU_ACCEL_ODR, pSvcUserPref->ImuSetting.AccelOdr);
    Imu_SetAccelPara(IMU_ACCEL_RANGE, pSvcUserPref->ImuSetting.AccelRange);
    Imu_SetGyroPara(IMU_GYRO_ODR, pSvcUserPref->ImuSetting.GyroOdr);
    Imu_SetGyroPara(IMU_GYRO_RANGE, pSvcUserPref->ImuSetting.GyroRange);
    Imu_SetAxisPolarity(pSvcUserPref->ImuSetting.AxisPolarity);
#endif

#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
    if (app_helper.check_file_exist(USER_SETTING_UVC_MODE, NULL)) {
        debug_line("device work on uvc mode");
        remove(USER_SETTING_UVC_MODE);
        dqa_test_script.usb_uvc_mode = 1;
        app_helper.external_camera_enabled = 0;
    } else if (app_helper.check_file_exist(USER_SETTING_UVC_MODE2, NULL)) {
        debug_line("device work on uvc mode2");
        remove(USER_SETTING_UVC_MODE2);
        dqa_test_script.usb_uvc_mode = 2;
    } else
#endif
    app_helper.power_on_with_sd_card = AmbaSD_IsCardPresent(SD_CHANNEL);
    if (tolower(REC_DEST_DISK[0]) == tolower(SD_SLOT[0])) {
        if (app_helper.check_sd_exist() == 0) {
            debug_line("no sd card. disable auto record");
            app_helper.internal_camera_auto_record = 0;
            app_helper.external_camera_auto_record = 0;
        }
    }
    if (pSvcUserPref->WatchDogOnOff == OPTION_ON) {
        app_helper.set_watchdog(0);//set to 10Seconds
    } else {
        app_helper.set_watchdog(0);
    }
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    app_helper.set_led_onoff((pSvcUserPref->LedOnOff == OPTION_ON) ? 1 : 0);
    app_helper.set_led_brihtness(pSvcUserPref->LedBrightness);
#if !defined(CONFIG_BSP_CV25_NEXAR_D080)
    app_helper.set_led_fade(1);
#endif
    app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
#endif
    AmbaAudioEnc_SetMute((pSvcUserPref->RecordSoundOnOff == OPTION_ON) ? 0 : 1);
    AmbaAudio_CodecSetVolume(0, pSvcUserPref->AudioVolume);
    if (pSvcUserPref->PowerOnOffSound == OPTION_ON
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
        && dqa_test_script.usb_uvc_mode == 0
#endif
    ) {
        if (app_helper.hard_reset_flag || app_helper.factory_reset_flag) {
            beep_play(BEEP_ID_RESET);
        } else {
            beep_play(BEEP_ID_POWER_ON);
        }
    }

    //create msg queue
    app_msg_queue_init(app_handler_entry);
    async_task_start();
    //button_task_start();
    pcm_record_task_start();
    //aac_record_task_start();
    if (app_helper.external_camera_enabled) {
        ir_task_start();
    }
    button_task_start();
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    usb_device_task_start();
#endif
    battery_task_start();
#if defined(CONFIG_ENABLE_IMU)
    imu_task_start();
#endif
    mcu_update_task_start();
    gnss_record_task_start();
    imu_record_task_start();
    scan_file_info_task_start();

    app_util_init();
    SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);
    if (SdStatus.Status == SVC_APP_STAT_SD_READY) {
        app_msg_queue_send(APP_MSG_ID_SD_INSERT, 0, 0, 0);
    } else if (SdStatus.Status == SVC_APP_STAT_SD_IDLE) {
        app_msg_queue_send(APP_MSG_ID_SD_INSERT, 0, 0, 0);
        app_msg_queue_send(APP_MSG_ID_SD_IDLE, 0, 0, 0);
    }
    SvcSysStat_Register(SVC_APP_STAT_STG, svc_sys_stat_callback, &CtrlID);
    SvcSysStat_Register(SVC_APP_STAT_LINUX, svc_sys_stat_callback, &CtrlID);

    return 0;
}

static void *app_handler_entry(void *argv)
{
    int ret = 0;
    app_message_s msg = {0};
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
    AmbaMisra_TouchUnused(&argv);
    while (1) {
        ret = app_msg_queue_recv(&msg);
        if (ret < 0) {
            continue;
        }
        switch (msg.msg_id) {
        case APP_MSG_ID_TIMER_1HZ:
        case APP_MSG_ID_TIMER_2HZ:
        case APP_MSG_ID_TIMER_30S:
            app_timer_callback_handler(msg.param1);
            break;
        case APP_MSG_ID_CAM_LIVEVIEW:
            debug_line("APP_MSG_ID_CAM_LIVEVIEW");
            break;
        case APP_MSG_ID_CAM_START_EVENT_RECORD:
            debug_line("APP_MSG_ID_CAM_START_EVENT_RECORD");
            rec_dvr_event_record_start(msg.param1, msg.param2, msg.param3);
            break;
        case APP_MSG_ID_CAM_STOP_EVENT_RECORD:
            debug_line("APP_MSG_ID_CAM_STOP_EVENT_RECORD");
            rec_dvr_event_record_stop(msg.param1);
            break;
        case APP_MSG_ID_CAM_CAPTURE_PIV_MJPG:
            debug_line("APP_MSG_ID_CAM_CAPTURE_PIV->MJPEG");
            rec_dvr_capture_piv(msg.param1, 1);
            break;
        case APP_MSG_ID_CAM_CAPTURE_PIV_RAW:
            debug_line("APP_MSG_ID_CAM_CAPTURE_PIV->RAW");
            rec_dvr_capture_piv(msg.param1, 0);
            break;
        case APP_MSG_ID_CAM_CAPTURE_PIV_DONE:
            debug_line("APP_MSG_ID_CAM_CAPTURE_PIV_DONE");
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
            dqa_test_uart_take_photo_done(1, 0, NULL);
            dqa_test_uart_set_take_photo_flag(0);
#endif
            rec_dvr_capture_piv_done();
            break;
        case APP_MSG_ID_CAM_CAPTURE_THM:
            debug_line("APP_MSG_ID_CAM_CAPTURE_THM");
            rec_dvr_capture_thm(msg.param1);
            break;
        case APP_MSG_ID_CAM_CAPTURE_THM_DONE:
            debug_line("APP_MSG_ID_CAM_CAPTURE_THM_DONE");
            rec_dvr_capture_thm_done();
            break;
        case APP_MSG_ID_POWER_BUTTON:
            debug_line("APP_MSG_ID_POWER_BUTTON");
            app_helper.force_power_off = 1;
            rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, app_helper.do_power_off);
            break;
        case APP_MSG_ID_LOW_BATTERY:
            debug_line("APP_MSG_ID_LOW_BATTERY");
            app_helper.low_battery_power_off = 1;
            rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, app_helper.do_power_off);
            break;
        case APP_MSG_ID_SD_INSERT:
            debug_line("APP_MSG_ID_SD_INSERT");
            if (app_helper.check_sd_update()) {
                Pmic_SetSramRegister(0x20);
                //AmbaNVM_SetBldMagicCode(NVM_TYPE, 5000);
                while (beep_is_playing()) msleep(500);
                Pmic_NormalSoftReset();
                break;
            }
            app_helper.check_enable_linux_uart(SD_SLOT[0]);
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
            dqa_test_script_check(SD_SLOT[0]);
#endif
            app_helper.sd_init_done = 1;//this should after dqa test script check
            if (app_helper.linux_booted) {
                mcu_update_set_run();
            }
            break;
        case APP_MSG_ID_SD_REMOVE:
            debug_line("APP_MSG_ID_SD_REMOVE");
            app_helper.sd_card_idle = 0;
            if (app_helper.force_power_off == 0) {
                app_helper.force_power_off = 1;
                app_helper.do_power_off();
            }
            break;
        case APP_MSG_ID_SD_IDLE:
            debug_line("APP_MSG_ID_SD_IDLE");
            app_helper.sd_card_idle = 1;
            app_helper.internal_camera_auto_record = pSvcUserPref->InternalCamera.AutoRecord;
            app_helper.external_camera_auto_record = pSvcUserPref->ExternalCamera.AutoRecord;
            if (app_helper.linux_booted) {
                rec_dvr_record_auto_start();
            }
            if (dqa_test_script.product_line_mode == 0) {
                //scan_file_info_task_run();
            }
            break;
        case APP_MSG_ID_SD_ERROR:
            debug_line("APP_MSG_ID_SD_ERROR");
            break;
        case APP_MSG_ID_ACC_CONNECTED:
            {
                ipc_event_s event;
                debug_line("APP_MSG_ID_ACC_CONNECTED");
                memset(&event, 0, sizeof(event));
                event.event_id = NOTIFY_ACC_CONNECTED;
                event.param = msg.param2;
                linux_api_service_notify_event(event);
                app_util_delay_power_off_stop();
            }
            break;
        case APP_MSG_ID_ACC_DISCONNECTED:
            {
                ipc_event_s event;
                debug_line("APP_MSG_ID_ACC_DISCONNECTED");
                memset(&event, 0, sizeof(event));
                event.event_id = NOTIFY_ACC_DISCONNECTED;
                event.param = msg.param2;
                linux_api_service_notify_event(event);
                if (app_helper.auto_power_off == 0) {
                    debug_line("auto_power_off disabled");
                    break;
                }
                app_util_delay_power_off_start();
            }
            break;
        case APP_MSG_ID_USB_INSERT:
            debug_line("APP_MSG_ID_USB_INSERT");
            app_helper.low_battery = 0;
            app_util_delay_power_off_stop();
            break;
        case APP_MSG_ID_USB_REMOVE:
            debug_line("APP_MSG_ID_USB_REMOVE");
            if (app_helper.auto_power_off == 0) {
                debug_line("auto_power_off disabled");
                break;
            }
            if (app_helper.usb_mass_storage_mode
                || dqa_test_script.product_line_mode
                || dqa_test_script.usb_uvc_mode) {
                app_helper.do_power_off();
            } else {
                app_util_delay_power_off_start();
            }
            break;
        case APP_MSG_ID_LINUX_BOOT_DONE:
            debug_line("APP_MSG_ID_LINUX_BOOT_DONE");
            app_helper.linux_booted = 1;
            if (app_helper.sd_init_done) {
                mcu_update_set_run();
            }
#if defined(CONFIG_ENABLE_EMMC_BOOT)
            if (app_helper.check_emmc_update()) {
                Pmic_SetSramRegister(0x20);
                while (beep_is_playing()) msleep(500);
                Pmic_NormalSoftReset();
                break;
            }
#endif
            linux_api_service_start();
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
            {
                extern int dqa_test_script_do_on_linux(void);
                dqa_test_script_do_on_linux();
            }
#endif
#if defined(CONFIG_APP_FLOW_AONI_CV_RUN)
            app_helper.linux_system("/usr/local/share/script/cv_run.sh");
            //flexidag_schdr -t 20000
#endif
            app_helper.load_dev_info();
            if (AmbaSD_IsCardPresent(SD_CHANNEL) == 0 || app_helper.sd_card_idle) {
                rec_dvr_record_auto_start();
            }
            battery_task_reset_param();
            break;
        case APP_MSG_ID_WIFI_BOOT_DONE:
            if (app_helper.wifi_booted) {
                break;
            }
            debug_line("APP_MSG_ID_WIFI_BOOT_DONE");
            app_helper.wifi_booted = 1;
            break;
        case APP_MSG_ID_BT_BOOT_DONE:
            if (app_helper.bt_booted) {
                break;
            }
            debug_line("APP_MSG_ID_BT_BOOT_DONE");
            app_helper.bt_booted = 1;
            break;
        case APP_MSG_ID_USB_WIFI_BOOT_DONE:
            if (app_helper.usb_wifi_booted) {
                break;
            }
            debug_line("APP_MSG_ID_USB_WIFI_BOOT_DONE");
            app_helper.usb_wifi_booted = 1;
            break;
        case APP_MSG_ID_LTE_ON:
            if (app_helper.lte_on) {
                break;
            }
            debug_line("APP_MSG_ID_LTE_ON");
            app_helper.lte_on = 1;
            break;
        case APP_MSG_ID_LTE_BOOT_DONE:
            if (app_helper.lte_booted) {
                break;
            }
            debug_line("APP_MSG_ID_LTE_BOOT_DONE");
            app_helper.linux_system("/usr/local/share/script/lte_csq_dump.sh&");
            app_helper.lte_booted_tick = tick();
            app_helper.lte_booted = 1;
            break;
        case APP_MSG_ID_FORMAT_SD:
            if (app_helper.format_busy) {
                debug_line("format sd is busy, ignore");
                break;
            }
            debug_line("APP_MSG_ID_FORMAT_SD");
            app_helper.format_busy = 1;
            if (msg.param1) {//format sd
                if (app_helper.check_sd_exist() == 0) {
                    debug_line("no sd. skip");
                    app_helper.format_busy = 0;
                    break;
                }
                rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, NULL);
            }
            async_msg_queue_send(ASYNC_MSG_ID_FORAMT_SD, msg.param1, 0, 0);
            break;
        case APP_MSG_ID_FORMAT_EMMC_DONE:
            debug_line("APP_MSG_ID_FORMAT_EMMC_DONE");
            app_helper.format_busy = 0;
            if (msg.param1 == 0) {
                debug_line("format emmc success");
            } else {
                debug_line("format emmc failed");
            }
            break;
        case APP_MSG_ID_FORMAT_SD_DONE:
            debug_line("APP_MSG_ID_FORMAT_SD_DONE");
            app_helper.format_busy = 0;
            if (AmbaSD_IsCardPresent(SD_CHANNEL) == 0) {
                app_helper.do_power_off();
                break;
            }
            if (msg.param1 == 0) {
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
                dqa_test_uart_format_sd_done(1);
#endif
                debug_line("format sd success");
                rec_dvr_record_auto_start();
            } else {
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
                dqa_test_uart_format_sd_done(0);
#endif
                debug_line("format sd failed");
            }
            break;
        case APP_MSG_ID_DEFAULT_SETTING:
            debug_line("APP_MSG_ID_DEFAULT_SETTING");
            user_setting_for_hard_reset();
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
            dqa_test_uart_default_setting_done();
#endif
            rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, app_util_do_hard_reset);
            break;
        case APP_MSG_ID_REBOOT:
            debug_line("APP_MSG_ID_REBOOT");
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
            dqa_test_uart_reboot_done();
#endif
            rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, Pmic_NormalSoftReset);
            break;
        case APP_MSG_ID_FACTORY_RESET:
            rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, app_util_do_factory_reset);
            break;
        case APP_MSG_ID_BUTTON_EVENT:
            if (msg.param1 == 0) {
                ipc_event_s event;
                debug_line("button short presss. taps_count: %d", msg.param2);
                if (dqa_test_script.product_line_mode && msg.param2 == 5) {
                    beep_play(BEEP_ID_DUMP_LOG);
                    offline_log_dump(REC_DEST_DISK":\\rtos.log");
                }
#if defined(CONFIG_APP_FLOW_AONI_PRODUCT_LINE_MODULE)
                product_line_cmd_button_test_key_pressed(0);
#endif
                memset(&event, 0, sizeof(event));
                event.event_id = NOTIFY_BUTTON_STATE_SHORT_PRESS;
                event.param = msg.param2;
                linux_api_service_notify_event(event);
            } else {
                ipc_event_s event;
                if (msg.param2 == 0) {
                    debug_line("button long press. press time: %d", msg.param3);
                    if (dqa_test_script.product_line_mode) {
                        if (app_helper.force_power_off == 0) {
                            app_helper.force_power_off = 1;
                            app_helper.do_power_off();
                        }
                    }
                } else if (msg.param2 == 1) {
                    debug_line("button long continue press. press time: %d", msg.param3);
                } else if (msg.param2 == 2) {
                    debug_line("button long press end. press time: %d", msg.param3);
                }
                memset(&event, 0, sizeof(event));
                event.event_id = NOTIFY_BUTTON_STATE_LONG_PRESS;
                event.param = msg.param2;
                event.param2 = msg.param3;
                linux_api_service_notify_event(event);
            }
            break;
        default:
            break;
        }
    }

    return NULL;
}

