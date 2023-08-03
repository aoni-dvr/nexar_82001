#include <app_base.h>
#include "AmbaDSP_Liveview.h"
#include "SvcResCfg.h"
#include "SvcStillCap.h"
#include "SvcStillTask.h"
#include "SvcStillMux.h"
#include "SvcStill.h"
#include "SvcDSP.h"
#include "SvcRecMain.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AeAwbAdj.h"

rec_dvr_status_s rec_dvr_status;
static void rec_dvr_record_timer_handler(int eid)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    static int cnt = 0;

    SvcUserPref_Get(&pSvcUserPref);
    if (eid == TIMER_UNREGISTER) {
        if (dqa_test_script.product_line_mode) {
            app_helper.set_led_fade(0);
            app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
        } else {
            if (pSvcUserPref->LedRtosControl == OPTION_ON) {
                app_helper.set_led_fade(1);
                app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
            }
        }
        return;
    }
    if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_RECORD) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time += 1;
    }
    if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_time += 1;
    }
    if (dqa_test_script.product_line_mode) {
        if (app_helper.low_battery == 0) {
            app_helper.set_led_fade(0);
            if (cnt == 0) {
                cnt = 1;
                app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
            } else {
                cnt = 0;
                app_helper.set_led_color(0, 0, 0);
            }
        }
    } else {
        if (app_helper.low_battery == 0) {
            app_helper.set_led_fade(0);
            if (pSvcUserPref->LedRtosControl == OPTION_ON) app_helper.set_led_color(pSvcUserPref->LedColor.R, pSvcUserPref->LedColor.G, pSvcUserPref->LedColor.B);
        }
    }
}

int rec_dvr_init(void)
{
    memset(&rec_dvr_status, 0, sizeof(rec_dvr_status));
    rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_IDLE;
    rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time = 0;
    rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_IDLE;
    rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_time = 0;

    return 0;
}

int rec_dvr_check_card_status(void)
{
    if (tolower(REC_DEST_DISK[0]) == tolower(SD_SLOT[0])
        && app_helper.check_sd_exist() == 0) {
        debug_line("%s: No sd card or card is bad", __func__, 0U);
        return -1;
    }
    if (AmbaDCF_IsReady() == 0) {
        debug_line("%s: dcf is not ready", __func__, 0U);
        return -1;
    }

    return 0;
}

int rec_dvr_record_started_handler(unsigned char bits)
{
    SVC_RES_CFG_s *resCfg = SvcResCfg_Get();

    if (bits & 0x01) {
        if (app_helper.internal_camera_auto_record && resCfg->RecStrm[0].RecSetting.DestBits & SVC_REC_DST_FILE) {
            rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_RECORD;
        } else {
            rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_IDLE;
        }
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time = 0;
    }
    if (bits & 0x02) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status = REC_STATUS_VF;
    }

    if (bits & 0x04) {
        if (app_helper.external_camera_auto_record && resCfg->RecStrm[2].RecSetting.DestBits & SVC_REC_DST_FILE) {
            rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_RECORD;
        } else {
            rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_IDLE;
        }
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_time = 0;
    }
    if (bits & 0x08) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status = REC_STATUS_VF;
    }

    if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_RECORD
        || rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD) {
        app_timer_unregister(TIMER_1HZ, rec_dvr_record_timer_handler);
        app_timer_register(TIMER_1HZ, rec_dvr_record_timer_handler);
    }

    return 0;
}

static record_stop_cb_func record_stop_cb = NULL;
int rec_dvr_record_stopped_handler(unsigned char bits)
{
    if (bits & 0x01) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_IDLE;
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time = 0;
    }
    if (bits & 0x02) {
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status = REC_STATUS_IDLE;
    }
    if (bits & 0x04) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_IDLE;
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_time = 0;
    }
    if (bits & 0x08) {
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status = REC_STATUS_IDLE;
    }
    if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_RECORD
        && rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status != REC_STATUS_RECORD) {
        app_timer_unregister(TIMER_1HZ, rec_dvr_record_timer_handler);
    }

    if (record_stop_cb != NULL) {
        (*record_stop_cb)();
    }
    return 0;
}

int rec_dvr_record_auto_start(void)
{
    ir_task_set_check_enable(1);
    if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_IDLE
        && rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status == REC_STATUS_IDLE
        && rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_IDLE
        && rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].preview_status == REC_STATUS_IDLE) {
        //record start
        rec_dvr_record_start(CAMERA_CHANNEL_BOTH);
    } else {
        rec_dvr_hot_config();
    }
#if defined(CONFIG_BSP_CV25_NEXAR_D161) || defined(CONFIG_BSP_CV25_NEXAR_D161V2)
    if (app_helper.external_camera_enabled) {
        if (app_helper.check_gc2053_left_connected() || app_helper.check_gc2053_right_connected()) {
            app_util_external_camera_monitor_start();
        } else {
            Pmic_NormalSoftReset();
        }
    }
#endif

    return 0;
}

static int cache_bits = 0x00;
int rec_dvr_record_start(CAMERA_CHANNEL_e channel)
{
#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    int internal_camera = 0, external_camera = 0;
#endif

    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return -1;
    }

    if (app_helper.linux_booted == 0) {
        debug_line("<%s> fail, linux not booted", __func__);
        return -1;
    }

    if (dqa_test_script_is_block_recording()) {
        debug_line("<%s> fail, dqa_test_script block record", __func__);
        return -1;
    }

#if !defined(CONFIG_BSP_H32_NEXAR_D081)
    if (channel == CAMERA_CHANNEL_INTERNAL || channel == CAMERA_CHANNEL_BOTH) {
        internal_camera = 1;
    }
    if (channel == CAMERA_CHANNEL_EXTERNAL || channel == CAMERA_CHANNEL_BOTH) {
        external_camera = 1;
    }

    {
        AWB_CONTROL_s AwbCtrl;
        AE_CONTROL_s AeCtrl;
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        AMBA_SENSOR_CHANNEL_s Channel;

        SvcUserPref_Get(&pSvcUserPref);
        if (internal_camera && app_helper.internal_camera_enabled) {
            //set rotation
            Channel.VinID = 0;
            AmbaSensor_SetRotation(&Channel, pSvcUserPref->InternalCamera.ImageRotation);
            //set awb
            AmbaImgProc_AWBGetAWBCtrlCap(Channel.VinID, &AwbCtrl);
            AwbCtrl.MenuMode = pSvcUserPref->AwbMode;
            AmbaImgProc_AWBSetAWBCtrlCap(Channel.VinID, &AwbCtrl);
            //set ae
            AmbaImgProc_AEGetAEControlCap(Channel.VinID, &AeCtrl);
            AeCtrl.EvBias = (pSvcUserPref->AeBias - 3) * 32;
            AmbaImgProc_AESetAEControlCap(Channel.VinID, &AeCtrl);
        }
        if (external_camera && app_helper.external_camera_enabled) {
            Channel.VinID = 1;
            AmbaSensor_SetRotation(&Channel, pSvcUserPref->ExternalCamera.ImageRotation);
            //set awb
            if (app_helper.internal_camera_enabled) {
                Channel.VinID = 1;
            } else {
                Channel.VinID = 0;
            }
            AmbaImgProc_AWBGetAWBCtrlCap(Channel.VinID, &AwbCtrl);
            AwbCtrl.MenuMode = pSvcUserPref->AwbMode;
            AmbaImgProc_AWBSetAWBCtrlCap(Channel.VinID, &AwbCtrl);
            //set ae
            AmbaImgProc_AEGetAEControlCap(Channel.VinID, &AeCtrl);
            AeCtrl.EvBias = (pSvcUserPref->AeBias - 3) * 32;
            AmbaImgProc_AESetAEControlCap(Channel.VinID, &AeCtrl);
        }
    }
#endif

    if (channel == CAMERA_CHANNEL_BOTH) {
        if (app_helper.external_camera_enabled) {
            if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_IDLE
                && rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status != REC_STATUS_IDLE) {
                debug_line("<%s:%d> not idle mode", __func__, __LINE__);
                return -1;
            }
        } else {
            channel = CAMERA_CHANNEL_INTERNAL;
            if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_IDLE) {
                debug_line("<%s:%d> not idle mode", __func__, __LINE__);
                return -1;
            }
        }
    } else if (channel == CAMERA_CHANNEL_INTERNAL) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status != REC_STATUS_IDLE) {
            debug_line("<%s:%d> not idle mode", __func__, __LINE__);
            return -1;
        }
    } else if (channel == CAMERA_CHANNEL_EXTERNAL) {
        if (app_helper.external_camera_enabled == 0) {
            debug_line("<%s:%d> external camera not enabled ", __func__, __LINE__);
            return -1;
        }
        if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status != REC_STATUS_IDLE) {
            debug_line("<%s:%d> not idle mode", __func__, __LINE__);
            return -1;
        }
    }

    if (app_helper.check_sd_exist() == 0) {
        app_helper.internal_camera_auto_record = 0;
        app_helper.external_camera_auto_record = 0;
    }
    debug_line("<%s:%d> internal_camera_auto_record: %d, external_camera_auto_record: %d", __func__, __LINE__,
                                app_helper.internal_camera_auto_record, app_helper.external_camera_auto_record);
    if (app_helper.internal_camera_auto_record || app_helper.external_camera_auto_record) {
        if (rec_dvr_check_card_status() < 0) {
            debug_line("<%s> card check fail", __func__);
            return -1;
        }
    }
    cache_bits = rec_dvr_channel_to_bits(channel);
    SvcRecMain_Start(cache_bits, 0);

#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_SCRIPT_MODULE)
    if (dqa_test_script.audio_test) {
        extern int product_line_cmd_record_playback(int len);
        product_line_cmd_record_playback(90);
    }
#if defined(CONFIG_ICAM_USE_DIAG_COMMANDS)
    if (dqa_test_script.ddr_shmoo_test) {
        extern int dqa_test_script_ddr_shmoo_task_start(void);
        dqa_test_script_ddr_shmoo_task_start();
    }
#endif
#endif

    return 0;
}

static int hot_config_busy = 0;
static void rec_dvr_hot_config_auto_record_handler(int tid)
{
    static int cnt = 0;
    if (tid == TIMER_UNREGISTER) {
        cnt = 0;
        return;
    }
    cnt++;
    if (cnt >= 5 || SvcControlTask_CheckLiveviewSwitchFinish()) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        cnt = 0;
        app_timer_unregister(TIMER_1HZ, rec_dvr_hot_config_auto_record_handler);
        rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_IDLE;
        rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_IDLE;
        if (pSvcUserPref->CvOnOff == OPTION_ON) {
            UINT32 SvcCvMainTask_Ctrl(const char *pCmd, void *pParam);
            SvcCvMainTask_Ctrl("continue", NULL);
        }
        rec_dvr_record_start(CAMERA_CHANNEL_BOTH);
        hot_config_busy = 0;
    }
}

static int rec_dvr_hot_config_impl(void)
{
    SvcResCfgTask_LoadSetting();
    SvcControlTask_SetLiveviewSwitchFinish(0);
    SvcFlowControl_Exec("liveview_switch");
    app_timer_register(TIMER_1HZ, rec_dvr_hot_config_auto_record_handler);

    return 0;
}

int rec_dvr_hot_config_busy_check(void)
{
    return hot_config_busy;
}

int rec_dvr_hot_config(void)
{
    hot_config_busy = 1;
    rec_dvr_record_stop(CAMERA_CHANNEL_BOTH, rec_dvr_hot_config_impl);
    rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status = REC_STATUS_BUSY;
    rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status = REC_STATUS_BUSY;

    return 0;
}

#if 0
static void rec_dvr_external_camera_stream_restart_timer_handler(int tid)
{
    static int cnt = 0;
    if (tid == TIMER_UNREGISTER) {
        cnt = 0;
        return;
    }
    cnt++;
    if (cnt >= 3) {
        cnt = 0;
        app_timer_unregister(TIMER_1HZ, rec_dvr_external_camera_stream_restart_timer_handler);
        SvcRecMain_Start(0x4);
    }
}
#endif

int rec_dvr_camera_record_control(CAMERA_CHANNEL_e channel, int record)
{
    debug_line("<%s> channel(%d) record(%d)", __func__, channel, record);

    if (channel >= CAMERA_CHANNEL_BOTH) {
        debug_line("<%s> invalid channel(%d)", __func__, channel);
        return -1;
    }
    if (record) {
        if (rec_dvr_status.channels[channel].rec_status != REC_STATUS_IDLE) {
            debug_line("<%s> camera start record fail, state:%s", __func__, rec_dvr_rec_status_2_str(rec_dvr_get_rec_status(channel)));
            return -1;
        }
        if (rec_dvr_check_card_status() < 0) {
            debug_line("<%s> record card check fail", __func__);
            return -1;
        }
    } else {
        if (rec_dvr_status.channels[channel].rec_status != REC_STATUS_RECORD) {
            debug_line("<%s> camera stop record fail, state:%s", __func__, rec_dvr_rec_status_2_str(rec_dvr_get_rec_status(channel)));
            return -1;
        }
    }
    rec_dvr_status.channels[channel].rec_status = REC_STATUS_BUSY;
    if (channel == CAMERA_CHANNEL_INTERNAL) {
        app_helper.internal_camera_auto_record = record;
        if (record) {
            cache_bits |= 0x01;
            SvcRecMain_Start(0x1, 0);
        } else {
            record_stop_cb = NULL;
            cache_bits &= ~0x01;
            SvcRecMain_Stop(0x1, 1U);
        }
    } else if (channel == CAMERA_CHANNEL_EXTERNAL) {
        //record_stop_cb = NULL;
        //SvcRecMain_Stop(0x4, 0U);
        app_helper.external_camera_auto_record = record;
        rec_dvr_hot_config();
        //SvcResCfgTask_LoadSetting();
        //SvcFlowControl_Exec("liveview_switch");
        //app_timer_register(TIMER_1HZ, rec_dvr_external_camera_stream_restart_timer_handler);
    }

    return 0;
}

int rec_dvr_record_stop(CAMERA_CHANNEL_e channel, record_stop_cb_func func)
{
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return -1;
    }
    record_stop_cb = func;
#if 0
    if (rec_dvr_is_recording(channel) == 0) {
        if (rec_dvr_is_event_recording(channel)) {
            rec_dvr_event_record_stop(channel);
        }
        if (record_stop_cb != NULL) {
            (*record_stop_cb)();
        }
        return 0;
    }

    if ((rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_RECORD && rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_time <= 2)
        || (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD && rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_time <= 2)) {
        if (rec_dvr_is_event_recording(channel)) {
            rec_dvr_event_record_stop(channel);
        }
        if (record_stop_cb != NULL) {
            (*record_stop_cb)();
        }
        return 0;
    }
 #else
    if (cache_bits == 0U) {
        if (record_stop_cb != NULL) {
            (*record_stop_cb)();
        }
        return 0;
    }
 #endif
    debug_line("%s record_stop bits: %d", __func__, cache_bits);
    SvcRecMain_Stop(cache_bits, 0U);

    return 0;
}

int rec_dvr_is_recording(CAMERA_CHANNEL_e channel)
{
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return 0;
    }
    if (channel != CAMERA_CHANNEL_BOTH) {
        return (rec_dvr_status.channels[channel].rec_status == REC_STATUS_RECORD);
    }
    return (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_RECORD) ||
            (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_RECORD);
}

rec_status_e rec_dvr_get_rec_status(CAMERA_CHANNEL_e channel)
{
    return rec_dvr_status.channels[channel].rec_status;
}

rec_status_e rec_dvr_get_preview_status(CAMERA_CHANNEL_e channel)
{
    return rec_dvr_status.channels[channel].preview_status;
}

unsigned int rec_dvr_get_rec_time(CAMERA_CHANNEL_e channel)
{
    return rec_dvr_status.channels[channel].rec_time;
}

const char *rec_dvr_rec_status_2_str(rec_status_e status)
{
    if (status == REC_STATUS_IDLE) {
        return "Idle";
    } else if (status == REC_STATUS_RECORD) {
        return "Record";
    } else if (status == REC_STATUS_VF) {
        return "Preview";
    } else if (status == REC_STATUS_BUSY) {
        return "Busy";
    }
    return "";
}

int rec_dvr_event_record_start(CAMERA_CHANNEL_e channel, unsigned int pre_seconds, unsigned int length)
{
    UINT32 after_seconds = 0;

    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return 0;
    }

    if (channel == CAMERA_CHANNEL_BOTH) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_IDLE
            || rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_IDLE) {
            debug_line("<%s:%d> channel:%d not vf/record mode", __func__, __LINE__, channel);
            return -1;
        }
    } else if (channel == CAMERA_CHANNEL_INTERNAL) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].rec_status == REC_STATUS_IDLE) {
            debug_line("<%s:%d> channel:%d not vf/record mode", __func__, __LINE__, channel);
            return -1;
        }
    } else if (channel == CAMERA_CHANNEL_EXTERNAL) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_EXTERNAL].rec_status == REC_STATUS_IDLE) {
            debug_line("<%s:%d> channel:%d not vf/record mode", __func__, __LINE__, channel);
            return -1;
        }
    }

    if (rec_dvr_is_event_recording(channel)) {
        debug_line("%s channel:%d already event record", __func__, channel);
        return -1;
    }

    if (rec_dvr_check_card_status() < 0) {
        debug_line("<%s> card check fail", __func__);
        return -1;
    }

    if (length == 0) {
        SVC_USER_PREF_s *pSvcUserPref = NULL;
        SvcUserPref_Get(&pSvcUserPref);
        after_seconds = 10;//pSvcUserPref->FileSplitTimeSeconds - abs(pre_seconds);
    } else {
        after_seconds = length - pre_seconds;
    }
    debug_line("<%s:%d> pre_seconds=%d,after_seconds=%d", __func__, __LINE__, pre_seconds, after_seconds);
    if (AmbaRdtEmgEvt_EvtStart(rec_dvr_channel_to_event_record_bits(channel), pre_seconds, after_seconds) == SVC_OK) {
#if defined(CONFIG_APP_FLOW_AONI_DQA_TEST_UART_MODULE)
        dqa_test_uart_event_record_done();
#endif
        return 0;
    }
    debug_line("%s failed", __func__);

    return -1;
}

int rec_dvr_is_event_recording(CAMERA_CHANNEL_e channel)
{
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return 0;
    }
    return 0;//AmbaRdtEmgEvt_EvtChecking(rec_dvr_channel_to_event_record_bits(channel)) ? 1 : 0;
}

int rec_dvr_event_record_stop(CAMERA_CHANNEL_e channel)
{
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return -1;
    }
    if (rec_dvr_is_event_recording(channel) == 0) {
        debug_line("%s channel: %d not event record", __func__, channel);
        return -1;
    }
    return 0;//SvcRecMain_EvtStop(rec_dvr_channel_to_event_record_bits(channel));
}

static int capture_piv_busy = 0;
static int capture_thm_busy = 0;
static int capture_thm_block = 0;
int rec_dvr_capture_piv(CAMERA_CHANNEL_e channel, int is_mjpg)
{
    UINT32 Rval = SVC_OK;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->PivOnOff == OPTION_OFF) {
        debug_line("%s not support", __func__);
        return -1;
    }
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return -1;
    }
    if (capture_piv_busy) {
        debug_line("%s busy, ignore", __func__);
        return -1;
    }
    if (capture_thm_busy) {
        debug_line("%s thm_busy, do piv later", __func__);
        capture_piv_busy = 1 | (channel << 1) | (is_mjpg << 4);
        return 0;
    }
    if (rec_dvr_check_card_status() < 0) {
        debug_line("<%s> card check fail", __func__);
        return -1;
    }
    if (channel == CAMERA_CHANNEL_BOTH && app_helper.external_camera_enabled) {
        capture_piv_busy = 2;
    } else {
        capture_piv_busy = 1;
    }
    Rval = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
    if (Rval == SVC_OK) {
        SVC_CAP_CTRL_s Ctrl;
        Ctrl.Cfg.CapNum = 1;
        Ctrl.Cfg.AebNum = 1;
        Ctrl.Cfg.StrmMsk = rec_dvr_channel_to_piv_bits(channel) & 0xFFFF;
        Ctrl.Cfg.PicType = is_mjpg ? SVC_STL_TYPE_JPEG : SVC_STL_TYPE_YUV;
        Ctrl.Cfg.SyncEncStart = (UINT8)0;
        Ctrl.Cfg.CapOnly = is_mjpg ? 0 : 1;
        Ctrl.Cfg.CapType = (UINT8)SVC_FRAME_TYPE_YUV;
        Ctrl.Cfg.StopLiveview = (UINT8)0;
        Rval = SvcStillTask_Capture(&Ctrl);
        if (Rval != SVC_OK) {
            debug_line("%s fail to yuv cap", __func__);
            capture_piv_busy = 0;
        }
    } else {
        debug_line("%s fail to create task", __func__);
        capture_piv_busy = 0;
    }

    return (Rval == SVC_OK) ? 0 : -1;
}

int rec_dvr_capture_piv_done(void)
{
    capture_piv_busy--;
    if (capture_piv_busy <= 0) {
        if (capture_thm_busy) {
            CAMERA_CHANNEL_e channel = (CAMERA_CHANNEL_e)(capture_thm_busy >> 1);
            capture_thm_busy = 0;
            //capture piv
            rec_dvr_capture_thm(channel);
        }
    }

    return 0;
}

int rec_dvr_check_piv_finish(void)
{
    return capture_piv_busy ? 0 : 1;
}

int rec_dvr_capture_thm(CAMERA_CHANNEL_e channel)
{
    UINT32 Rval = SVC_OK;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->PivOnOff == OPTION_OFF) {
        debug_line("%s not support", __func__);
        return -1;
    }
    if (channel >= CAMERA_CHANNEL_NUM) {
        debug_line("%s invalid channel: %d", __func__, channel);
        return 0;
    }
    if (capture_thm_busy) {
        debug_line("%s busy, ignore", __func__);
        capture_thm_block = 1 | (channel << 1);
        return -1;
    }
    if (capture_piv_busy) {
        debug_line("%s piv_busy, do thm later", __func__);
        capture_thm_busy = 1 | (channel << 1);
        return 0;
    }
    if (rec_dvr_check_card_status() < 0) {
        debug_line("<%s> card check fail", __func__);
        return -1;
    }
    capture_thm_busy = 1;
    Rval = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
    if (Rval == SVC_OK) {
        SVC_CAP_CTRL_s Ctrl;
        Ctrl.Cfg.CapNum = 1;
        Ctrl.Cfg.AebNum = 1;
        Ctrl.Cfg.StrmMsk = rec_dvr_channel_to_piv_bits(channel) & 0xFFFF;
        Ctrl.Cfg.PicType = (UINT8)SVC_STL_TYPE_THM;
        Ctrl.Cfg.SyncEncStart = (UINT8)0;
        Ctrl.Cfg.CapOnly = (UINT8)0;
        Ctrl.Cfg.CapType = (UINT8)SVC_FRAME_TYPE_YUV;
        Ctrl.Cfg.StopLiveview = (UINT8)0;
        Rval = SvcStillTask_Capture(&Ctrl);
        if (Rval != SVC_OK) {
            debug_line("%s fail to yuv cap", __func__);
            capture_thm_busy = 0;
        }
    } else {
        debug_line("%s fail to create task", __func__);
        capture_thm_busy = 0;
    }

    return (Rval == SVC_OK) ? 0 : -1;
}

int rec_dvr_capture_thm_done(void)
{
    capture_thm_busy = 0;
    if (capture_thm_block) {
        CAMERA_CHANNEL_e channel = (CAMERA_CHANNEL_e)(capture_thm_block >> 1);
        capture_thm_block = 0;
        //capture piv
        rec_dvr_capture_thm(channel);
    } else {
        if (capture_piv_busy) {
            CAMERA_CHANNEL_e channel = (CAMERA_CHANNEL_e)((capture_piv_busy >> 1) & 0x7);
            int is_mjpg = (capture_piv_busy >> 4) & 0x1;
            capture_piv_busy = 0;
            //capture piv
            rec_dvr_capture_piv(channel, is_mjpg);
        }
    }

    return 0;
}

int rec_dvr_update_thm(CAMERA_CHANNEL_e channel, const char *path)
{
    (void)path;
    app_msg_queue_send(APP_MSG_ID_CAM_CAPTURE_THM, channel, 0, 0);

    return 0;
}

CAMERA_CHANNEL_e rec_dvr_stream_id_to_channel(UINT32 Id)
{
    if (Id >= 2) {
        return CAMERA_CHANNEL_EXTERNAL;
    }
    return CAMERA_CHANNEL_INTERNAL;
}

CAMERA_CHANNEL_e rec_dvr_bits_to_channel(UINT32 Bits)
{
    if ((Bits & 0x3) && (Bits & 0xC)) {
        return CAMERA_CHANNEL_BOTH;
    }
    if (Bits & 0x3) {
        return CAMERA_CHANNEL_INTERNAL;
    }
    if (Bits & 0xC) {
        return CAMERA_CHANNEL_EXTERNAL;
    }
    return CAMERA_CHANNEL_INTERNAL;
}

unsigned int rec_dvr_channel_to_bits(CAMERA_CHANNEL_e channel)
{
    if (app_helper.internal_camera_enabled == 0 && app_helper.external_camera_enabled == 0) {
        return 0x0;
    }
    if (app_helper.internal_camera_enabled == 0 && app_helper.external_camera_enabled) {
        return app_helper.external_camera_auto_record ? 0x3 : 0x2;
    }
    if (app_helper.internal_camera_enabled && app_helper.external_camera_enabled == 0) {
        if (rec_dvr_status.channels[CAMERA_CHANNEL_INTERNAL].preview_status == REC_STATUS_VF) {
            return 0x1;
        } else {
            return app_helper.internal_camera_auto_record ? 0x3 : 0x2;
        }
    }
    if (channel == CAMERA_CHANNEL_INTERNAL) {
        return app_helper.internal_camera_auto_record ? 0x3 : 0x2;
    }
    if (channel == CAMERA_CHANNEL_EXTERNAL) {
        return 0x4;
    }
    if (channel == CAMERA_CHANNEL_BOTH) {
        return app_helper.internal_camera_auto_record ? 0x7 : 0x6;
    }

    return 0x2;
}

unsigned int rec_dvr_channel_to_piv_bits(CAMERA_CHANNEL_e channel)
{
    if (app_helper.external_camera_enabled) {
        if (channel == CAMERA_CHANNEL_INTERNAL) {
            return 0x1;
        }
        if (channel == CAMERA_CHANNEL_EXTERNAL) {
            return 0x4;
        }
        if (channel == CAMERA_CHANNEL_BOTH) {
            return 0x5;
        }
    }
    return 0x1;
}

unsigned int rec_dvr_channel_to_event_record_bits(CAMERA_CHANNEL_e channel)
{
    if (app_helper.external_camera_enabled) {
        if (channel == CAMERA_CHANNEL_INTERNAL) {
            return 0x1;
        }
        if (channel == CAMERA_CHANNEL_EXTERNAL) {
            return 0x4;
        }
        if (channel == CAMERA_CHANNEL_BOTH) {
            return 0x5;
        }
    }
    return 0x1;
}

unsigned int rec_dvr_get_channel_second_stream_zone_id(CAMERA_CHANNEL_e channel)
{
    if (channel == CAMERA_CHANNEL_INTERNAL) {
        return 0x80000001;
    }
    if (channel == CAMERA_CHANNEL_EXTERNAL) {
        return 0x80000002;
    }
    return 0x80000001;
}

int rec_dvr_get_event_pre_record_buffer_length(void)
{
    SVC_USER_PREF_s *pSvcUserPref = NULL;
    SvcUserPref_Get(&pSvcUserPref);
    if (pSvcUserPref->EventBufferLength > EVENT_RECORD_MAX_PRE_SECONDS) {
        return EVENT_RECORD_MAX_PRE_SECONDS;
    }
    return pSvcUserPref->EventBufferLength;
}

static void adj_set_ir_mode(UINT32 ViewId, UINT32 enable, INT32 sceneMode)
{
    if (enable > 0UL) {
        (void)Amba_Set_MADJ(ViewId, 1U, 1U);
        (void)AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_VIDEO, &sceneMode);
        (void)AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_STILL, &sceneMode);
    } else {
        (void)Amba_Set_MADJ(ViewId, 0U, 0U);
        (void)AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_VIDEO, &sceneMode);
        (void)AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_STILL, &sceneMode);
    }
}

int rec_dvr_switch_black_white_mode(CAMERA_CHANNEL_e channel, int on)
{
    UINT32 ViewID;
    static int mode[2] = {0, 0};

    if (channel == CAMERA_CHANNEL_EXTERNAL || channel == CAMERA_CHANNEL_BOTH) {
        if (app_helper.external_camera_enabled) {
            ViewID = 1;
            if (mode[1] != on) {
                mode[1] = on;
                adj_set_ir_mode(ViewID, on, SCENE_OFF);
            }
        }
    }

    return 0;
}

