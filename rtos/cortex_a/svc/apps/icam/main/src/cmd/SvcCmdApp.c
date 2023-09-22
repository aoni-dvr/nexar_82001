/**
 *  @file SvcCmdApp.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc application command functions
 *
 */

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaSYS.h"
#include "AmbaHDMI.h"
#include "AmbaFPD.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaUtility.h"
#include "AmbaSensor.h"
#include "AmbaFPD_HDMI.h"
#include "AmbaSD.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
#include "AmbaFTCM.h"
#include "SvcSwPll.h"
#endif

#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaCache.h"
#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaCalib_SimpleExtCalibIF.h"
#endif

#if defined(CONFIG_BUILD_COMMON_TUNE_CTUNER)
#include "AmbaCT_EmirTuner.h"
#include "AmbaCT_EmirTunerIF.h"
#include "AmbaCT_TextHdlr.h"
#endif

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "SvcTask.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaVfs.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"
#include "AmbaAEncFlow.h"
#include "AmbaVfs.h"
#include "SvcRebelGUI.h"
#ifdef CONFIG_ICAM_PLAYBACK_USED
#include "SvcPbkCtrl.h"
#include "SvcPbkInfoPack.h"
#endif
#include "SvcPref.h"
#include "SvcUserPref.h"
#include "SvcSysStat.h"
#include "SvcFlowControl.h"
#include "SvcCmdApp.h"
#include "SvcODDrawTask.h"
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
#include "SvcODDrawTask_Rebel.h"
#endif
#include "SvcVoutFrmCtrlTask.h"
#include "SvcCmd.h"
#if defined(CONFIG_ICAM_IMGCAL_USED)
#include "SvcCalibMgr.h"
#endif
#include "SvcCmdIK.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcAppStat.h"
#include "SvcTaskList.h"
#ifdef CONFIG_ICAM_RECORD_USED
#include "SvcRecQpCtrl.h"
#endif
#include "SvcResCfg.h"
#include "SvcResCfgTask.h"
#include "SvcVoutSrc.h"
#include "SvcPyramid.h"
#include "SvcDisplayTask.h"
#include "SvcLiveviewTask.h"
#include "SvcVinTree.h"

#ifdef CONFIG_ICAM_STLCAP_USED
#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillEnc.h"
#include "SvcStillMux.h"
#include "SvcStillProc.h"
#include "SvcStillTask.h"
#endif
#include "SvcIKCfg.h"
#include "SvcIsoCfg.h"
#include "SvcUtil.h"
#include "SvcOsd.h"
#include "SvcSafeStateMgr.h"

#ifdef CONFIG_ICAM_RECORD_USED
#include "SvcRecMain.h"
#include "SvcRecInfoPack.h"
#include "SvcRecTask.h"
#endif
#include "SvcStgMonitor.h"
#include "SvcVoutCtrlTask.h"
#include "SvcPlaybackTask.h"
#include "SvcImgTask.h"
#include "SvcShell.h"

#ifdef CONFIG_BUILD_AMBA_ADAS
#include "AmbaCT_AvmTuner.h"
#include "SvcCvFlow_Comm.h"
#include "SvcBsdTask.h"
#include "AmbaCalib_DetectionIF.h"
#include "AmbaCT_Logging.h"
#include "AmbaSR_Lane.h"
#include "RefFlow_AutoCal.h"
#include "SvcAdasAutoCal.h"

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
#include "RefFlow_FC.h"
#include "SvcCvCamCtrl.h"
#include "SvcAdasNotify.h"
#include "AmbaSR_Lane.h"
#include "AmbaAP_LKA.h"
#include "SvcFcwsFcmdTask.h"
#include "SvcFcwsFcmdTaskV2.h"
#include "SvcLdwsTask.h"
#include "SvcLdwsTaskV2.h"
#endif

#include "SvcBsdTask.h"
#include "AmbaSurround.h"
#include "SvcDirtDetect.h"
#include "SvcCalibAdas.h"
#include "SvcRmgTask.h"
#endif

#ifdef CONFIG_ICAM_PROJECT_SURROUND
#include "SvcAnimCalib.h"
#endif

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
#include "SvcVoutDefImgTask.h"
#endif

#ifdef CONFIG_BUILD_IMGFRW_AAA
#include "SvcColorBalance.h"
#endif

#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
#include "SvcCalibStitch.h"
#include "SvcEmrAdaptiveTask.h"
#endif

#ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
#include "SvcSensorCrcCheckTask.h"
#endif

#include "AmbaImg_AaaDef.h"
#include "AmbaImg_External_CtrlFunc.h"

#if defined(CONFIG_QNX)
#include "SvcAmageTask.h"
#endif

#ifdef CONFIG_ICAM_YUVFILE_FEED
#include "SvcLvFeedFileYuvTask.h"
#endif

#if 0//def CONFIG_ICAM_PROJECT_SHMOO
#include "SvcAdvancedShmooTask.h"
#endif

#ifdef CONFIG_BUILD_AMBA_ADAS
#ifndef ENABLE
#define ENABLE  1U
#define DISABLE 0U
#endif
#define rear  0U
#define left  1U
#define right 2U
#define none  255U
#endif

#define SVC_LOG_CMDAPP      "CAPP"

static void CmdAppUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_app commands:\n");
    PrintFunc(" rec load:                           load record framework\n");
    PrintFunc(" rec unload:                         unload record framework\n");
    PrintFunc(" boot2rec [timeout]:                 ms, boot2rec wait storage done timeout\n");
    PrintFunc(" pbk load:                           config playback\n");
    PrintFunc(" format_id [format id]:              change format id\n");
    PrintFunc(" usb [class]:                        0:off, 1:msc, 2:mtp, 3:host_msc\n");
    PrintFunc(" set_rec_maxbrate [max bitrate]:     Unit: Mbps\n");
//    PrintFunc(" dsp_log    [on|off]:         show dsp log\n");
//    PrintFunc(" ssp_log    [on|off]:         show ssp log\n");
//    PrintFunc(" sensor_log [on|off]:         show sensor log\n");
//    PrintFunc(" bridge_log [on|off]:         show bridge log\n");
//    PrintFunc(" log [module] [on|off]:       show bridge log\n");
//    PrintFunc(" dsp_dbg_lvl [level]:         set dsp log debug level\n");
//    PrintFunc(" dsp_dbg_thd [thd_mask]:      set dsp log thread disable mask, bit0 means thread0,\n");
//    PrintFunc("                              etc; 1-disable, 0-enable\n");
    PrintFunc(" data_gather [on|off]: on/off data gather\n");
    PrintFunc(" dcf [0|1]: [CameraDCF|DashcamDCF]\n");
    PrintFunc(" dbg_print: print dbg msg for SSP, IK, ... \n");
    PrintFunc(" yuvcap [sync_rec] [pic_type] [strm_msk] [cap_only]: liveview capture test cmd\n");
    PrintFunc(" cap [sync_rec] [pic_type] [vin_msk] [cap_only] [cap_type] [stop_liv] [cap_num] [aeb_num]: still capture test cmd\n");
    PrintFunc(" free_space_draw [on|off]:  Enable/Disable Seg free space draw \n");
    PrintFunc(" dmic [enable|disable]:   Enable / Disable DMIC \n");
    PrintFunc(" mctfcmpr [on|off]: on/off mctf compression\n");
    PrintFunc(" mctsdout [on|off]: on/off mcts dram out\n");
    PrintFunc(" mainstg [0|1]: configure main storage\n");
    PrintFunc(" warp file [FovID] [Enable] [Header] [File name]: Apply warp table from file\n");
    PrintFunc(" yuvfeed_file start [FovBits] [FileNum] [Interval] [FileName 1] ... [FileName N]: Start YUV file Feeding Task\n");
    PrintFunc(" yuvfeed_file stop [FovBits]: Stop YUV file Feeding Task\n");
    PrintFunc(" exec [File Name]: Execute script file\n");
#if defined(CONFIG_ATF_SUSPEND_SRAM)
    PrintFunc(" suspend:                            low-power suspend and resume\n");
#endif
}

#if defined(CONFIG_ICAM_RECORD_USED)
static void RecMonNotify(UINT32 NotifyCode, UINT32 StrmBits)
{
    if (NotifyCode == SVC_REC_MON_NCODE_QUEUE_EXCEP) {
        SvcLog_DBG(SVC_LOG_CMDAPP, "\n", 0U, 0U);
        SvcLog_DBG(SVC_LOG_CMDAPP, "#### storage or source queues are abnormal ...", 0U, 0U);
        SvcLog_DBG(SVC_LOG_CMDAPP, "####   stop record automatically ... 0x%X\n", StrmBits, 0U);
        SvcLog_DBG(SVC_LOG_CMDAPP, "\n", 0U, 0U);

        SvcRecMain_Stop(StrmBits, 1U);
    }
}
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
static void CmdAPP_strcat(char *dest, const char *src)
{
    UINT32 len1,len2;

    len1 = AmbaUtility_StringLength(src);
    len2 = AmbaUtility_StringLength(dest);

    AmbaUtility_StringAppend(dest, len1+len2+1U, src);
}
#endif

static UINT32 CmdRecEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#if defined(CONFIG_ICAM_RECORD_USED)
    UINT32             Rval = SVC_OK, NullWrite = 0U;
    SVC_APP_STAT_STG_s SdStatus = {0};

    AmbaMisra_TouchUnused(&NullWrite);

    if (ArgCount <= 3U) {
        SvcRecMain_Control(SVC_RCM_PMT_NLWR_GET, 0, NULL, &NullWrite);
        if (NullWrite == 0U) {
            Rval = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);
            if (SVC_OK == Rval) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                if ((SdStatus.Status & SVC_APP_STAT_SD_READY) > 0U) {
#else
                if ((0U < (SdStatus.Status & SVC_APP_STAT_STG_C_READY)) ||
                    (0U < (SdStatus.Status & SVC_APP_STAT_STG_D_READY)))  {
#endif
                    Rval = SVC_OK;
                } else {
                    Rval = SVC_NG;
                    SvcLog_NG(SVC_LOG_CMDAPP, "no storage is present ... ", 0U, 0U);
                }
            }
        }

        if (Rval == SVC_OK) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("load", pArgVector[2U])) {
                    SVC_ENC_INFO_s       EncInfo;
                    SVC_REC_MAIN_INFO_s  RecMainInfo;

                    PrintFunc("load record framework ... ");

                    /* load video encoder parameters */
                    SvcEnc_InfoGet(&EncInfo);
                    SvcInfoPack_EncConfig(&EncInfo);

                    /* load record framework */
                    SvcRecMain_InfoGet(&RecMainInfo);
                    SvcInfoPack_RecConfig(&RecMainInfo);
                    SvcRecMain_FwkLoad(RecMonNotify, SvcRecTask_CapThmCB);
                    SvcLog_OK(SVC_LOG_CMDAPP, "record framework loading is done", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("unload", pArgVector[2U])) {
                    PrintFunc("unload record framework ... ");
                    SvcRecMain_FwkUnload();
                    SvcLog_OK(SVC_LOG_CMDAPP, "record framework unloading is done", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
            }
        }
    } else {
        Rval = SVC_NG;
    }

    return Rval;
#else
    if (2U < ArgCount) {
        PrintFunc(pArgVector[2U]);
    }
    PrintFunc(" not support\n");

    return SVC_OK;
#endif
}

static UINT32 CmdPbkEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32               Rval = SVC_OK;
#if defined(CONFIG_ICAM_PLAYBACK_USED)
    UINT32               DecoderID;
    SVC_PBK_CTRL_INFO_s  *pInfo = NULL;

    if (2U < ArgCount) {
        if (0 == SvcWrap_strcmp("load", pArgVector[2U])) {
            for (DecoderID = 0U; DecoderID < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; DecoderID++) {
                SvcPbkCtrl_InfoGet(DecoderID, &pInfo);
                SvcInfoPack_PbkConfig(DecoderID, pInfo);
                PrintFunc("done\n");
            }
        } else if (0 == SvcWrap_strcmp("loopstart", pArgVector[2U])) {
            UINT32 FileNum = 0U, FileIdx[CONFIG_AMBA_PLAYER_MAX_NUM] = {0}, FeatureBits = 0U, i;

            if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &FileNum)) {
                Rval = SVC_NG;
            }

            if (Rval == SVC_OK) {
                for (i = 0U; i < FileNum; i++) {
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[4UL + i], &FileIdx[i])) {
                        Rval = SVC_NG;
                    }
                }
            }

            if (Rval == SVC_OK) {
                if (SVC_OK != SvcWrap_strtoul(pArgVector[4UL + FileNum], &FeatureBits)) {
                    Rval = SVC_NG;
                }
            }

            if (Rval == SVC_OK) {
                SvcPlaybackTask_LoopPbkStart(FileNum, FileIdx, FeatureBits);
            }
        } else if (0 == SvcWrap_strcmp("loopstop", pArgVector[2U])) {
            SvcPlaybackTask_LoopPbkStop();
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    return Rval;
#else
    if (2U < ArgCount) {
        PrintFunc(pArgVector[2U]);
    }
    PrintFunc(" not support\n");
    return Rval;
#endif
}

static void SetModuleLog(UINT16 Module, UINT8 *pSvcUserPrefParam, UINT8 Enable)
{
    UINT32  Rval, PrefBufSize;
    ULONG   PrefBufAddr;

    *pSvcUserPrefParam = Enable;
    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
    }
    Rval = AmbaPrint_ModuleSetAllowList(Module, (UINT8)(*pSvcUserPrefParam));
    if (Rval == OK) {
        SvcLog_OK(SVC_LOG_CMDAPP, "AmbaPrint_ModuleSetAllowList 0x%x %u", Module, Enable);
    } else {
        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaPrint_ModuleSetAllowList 0x%x return %u", Module, Rval);
    }
}

#if defined(CONFIG_ICAM_STLCAP_USED)
static UINT32 CapTest(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 Rval = SVC_NG;
    if (5U < ArgCount) {
        UINT32 StrmMsk = 0U, PicType = 0U, SyncEncStart = 0U, CapOnly = 0U, CapType = 0, LivStop = 0, CapNum = 0, AebNum = 0, ForceCapCe = 0xFFFFU;

        Rval = SvcWrap_strtoul(pArgVector[2], &SyncEncStart); AmbaMisra_TouchUnused(&Rval);
        Rval = SvcWrap_strtoul(pArgVector[3], &PicType); AmbaMisra_TouchUnused(&Rval);
        Rval = SvcWrap_strtoul(pArgVector[4], &StrmMsk); AmbaMisra_TouchUnused(&Rval);
        Rval = SvcWrap_strtoul(pArgVector[5], &CapOnly); AmbaMisra_TouchUnused(&Rval);
        SvcLog_OK(SVC_LOG_CMDAPP, "SyncEncStart %u PicType %u", SyncEncStart, PicType);
        SvcLog_OK(SVC_LOG_CMDAPP, "StrmMsk 0x%x CapOnly %u", StrmMsk, CapOnly);
        if (9U < ArgCount) {
            Rval = SvcWrap_strtoul(pArgVector[6], &CapType); AmbaMisra_TouchUnused(&Rval);
            Rval = SvcWrap_strtoul(pArgVector[7], &LivStop); AmbaMisra_TouchUnused(&Rval);
            Rval = SvcWrap_strtoul(pArgVector[8], &CapNum); AmbaMisra_TouchUnused(&Rval);
            Rval = SvcWrap_strtoul(pArgVector[9], &AebNum); AmbaMisra_TouchUnused(&Rval);
            SvcLog_OK(SVC_LOG_CMDAPP, "CapType %u LivStop %u", CapType, LivStop);
            SvcLog_OK(SVC_LOG_CMDAPP, "CapNum %u AebNum %u", CapNum, AebNum);
        }
        if (10U < ArgCount) {
            Rval = SvcWrap_strtoul(pArgVector[10], &ForceCapCe); AmbaMisra_TouchUnused(&Rval);
            SvcLog_OK(SVC_LOG_CMDAPP, "ForceCapCe %u", ForceCapCe, 0U);
        }

        if (CapOnly == 1U) {
            Rval = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_MUX);
        } else {
            Rval = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
        }
        if (Rval == SVC_OK) {
            SVC_CAP_CTRL_s Ctrl;
            UINT16 VinID;
            UINT8 SensorIdx;
            const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
            if (SVC_OK != AmbaWrap_memset(&Ctrl, 0, sizeof(Ctrl))) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset Ctrl failed", 0U, 0U);
            }
            if (CapNum == 0U) {
                CapNum = 1;
            }
            if (AebNum == 0U) {
                AebNum = 1;
            }
            Ctrl.Cfg.CapNum = CapNum;
            Ctrl.Cfg.AebNum = AebNum;
            Ctrl.Cfg.StrmMsk = (UINT16)StrmMsk;
            Ctrl.Cfg.PicType = (UINT8)PicType;
            Ctrl.Cfg.SyncEncStart = (UINT8)SyncEncStart;
            Ctrl.Cfg.CapOnly = (UINT8)CapOnly;
            Ctrl.Cfg.CapType = (UINT8)CapType;
            Ctrl.Cfg.StopLiveview = (UINT8)LivStop;
            if (SVC_OK != AmbaWrap_memcpy(&Ctrl.PivQview, &pCfg->StillCfg.PivQview, sizeof(SVC_PIV_QVIEW_CFG_s))) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy for PIV quickview configuration failed", 0U, 0U);
            }

            if (SVC_OK != AmbaWrap_memcpy(&Ctrl.Qview, &pCfg->StillCfg.Qview, sizeof(SVC_STILL_QVIEW_CFG_s))) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy for quickview configuration failed", 0U, 0U);
            }
            if (Ctrl.Cfg.CapType == SVC_FRAME_TYPE_RAW) {
                for (VinID = 0; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                    if ((Ctrl.Cfg.StrmMsk & ((UINT16)1 << VinID)) != 0U) {
                        if ((CapNum <= pCfg->StillCfg.RawCfg[VinID].MaxRaw.FrameNum) && (Rval == SVC_OK)) {
                            UINT8 ChanMask;
                            Ctrl.Vin[VinID].CapSensorModeID = (UINT8)pCfg->StillCfg.RawCfg[VinID].MaxRaw.SensorMode;
                            Ctrl.Vin[VinID].ForceCapCe = 0U;
                            if (ForceCapCe != 0xFFFFU) {
                                if (ForceCapCe > 0U) {
                                    // fource capture with CE
                                    Ctrl.Vin[VinID].ForceCapCe = (UINT8)1U;
                                } else {
                                    // fource capture without CE
                                    Ctrl.Vin[VinID].ForceCapCe = (UINT8)2U;
                                }
                            }
                            Ctrl.Vin[VinID].Cmpr = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Compressed;
                            Ctrl.Vin[VinID].CapW = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Width;
                            Ctrl.Vin[VinID].CapH = pCfg->StillCfg.RawCfg[VinID].MaxRaw.Height;
                            Ctrl.Vin[VinID].EffectW = pCfg->StillCfg.RawCfg[VinID].MaxRaw.EffectW;
                            Ctrl.Vin[VinID].EffectH = pCfg->StillCfg.RawCfg[VinID].MaxRaw.EffectH;
                            SvcLog_DBG(SVC_LOG_CMDAPP, "CapW %u CapH %u", Ctrl.Vin[VinID].CapW, Ctrl.Vin[VinID].CapH);
                            Ctrl.Vin[VinID].ChanMask = pCfg->StillCfg.RawCfg[VinID].ChanMask;
                            ChanMask = pCfg->StillCfg.RawCfg[VinID].ChanMask;
                            for (SensorIdx = 0; SensorIdx < SVC_CAP_MAX_SENSOR_PER_VIN; SensorIdx ++) {
                                if ((ChanMask & ((UINT8)1 << SensorIdx)) != 0U) {
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].Width = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].Width;
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].Height = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].Height;
                                    SvcLog_DBG(SVC_LOG_CMDAPP, "W %u H %u", Ctrl.Vin[VinID].ChanWin[SensorIdx].Width, Ctrl.Vin[VinID].ChanWin[SensorIdx].Height);
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].OffsetX = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].OffsetX;
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].OffsetY = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].OffsetY;
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].R2yScale = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].R2yScale;
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].OutWidth = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].OutWidth;
                                    Ctrl.Vin[VinID].ChanWin[SensorIdx].OutHeight = pCfg->StillCfg.RawCfg[VinID].ChanWin[SensorIdx].OutHeight;
                                }
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_CMDAPP, "CapNum %u > buffer number %u", CapNum, pCfg->StillCfg.RawCfg[VinID].MaxRaw.FrameNum);
                            Rval = SVC_NG;
                        }
                    }
                }
            } else {
                Ctrl.Cfg.CapNum = 1;
            }
#ifdef CONFIG_BUILD_COMSVC_IMGFRW
            if (Rval == SVC_OK) {
                Rval = SvcImgTask_WaitIqPreloadDone();
            }
#endif
            if (Rval == SVC_OK) {
                Rval = SvcStillTask_Capture(&Ctrl);
            }
        }
    }
    return Rval;
}
#endif

#ifdef CONFIG_BUILD_AMBA_ADAS
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) || defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
static UINT8 EmrQuickCal = DISABLE;
static UINT32 RefCalib_EmFeedLensSpec(const AMBA_CT_EM_LENS_s *In,
    AMBA_CAL_LENS_SPEC_s *Out,
    AMBA_CAL_LENS_DST_REAL_EXPECT_s *RealExpectBuf,
    AMBA_CAL_LENS_DST_ANGLE_s *AngleBuf,
    AMBA_CAL_LENS_DST_FORMULA_s *FormulaBuf)
{
    UINT32 Rval = 0U;
    Out->LensDistoType = In->LensDistoType;
    switch(In->LensDistoType) {
    case AMBA_CAL_LD_REAL_EXPECT_TBL:
        RealExpectBuf->Length = In->TableLen;
        RealExpectBuf->pRealTbl = In->pRealTable;
        RealExpectBuf->pExpectTbl = In->pExceptTable;
        Out->LensDistoSpec.pRealExpect = RealExpectBuf;
        break;
    case AMBA_CAL_LD_ANGLE_TBL:
        AngleBuf->Length = In->TableLen;
        AngleBuf->pRealTbl = In->pRealAngleTable;
        AngleBuf->pAngleTbl = In->pExceptAngleTable;
        Out->LensDistoSpec.pAngle = AngleBuf;
        break;
    case AMBA_CAL_LD_REAL_EXPECT_FORMULA:
        FormulaBuf->X1 = In->RealExpectFormula[0];
        FormulaBuf->X3 = In->RealExpectFormula[1];
        FormulaBuf->X5 = In->RealExpectFormula[2];
        FormulaBuf->X7 = In->RealExpectFormula[3];
        FormulaBuf->X9 = In->RealExpectFormula[4];
        Out->LensDistoSpec.pRealExpectFormula = FormulaBuf;
        break;
    case AMBA_CAL_LD_ANGLE_FORMULA:
        FormulaBuf->X1 = In->AngleFormula[0];
        FormulaBuf->X3 = In->AngleFormula[1];
        FormulaBuf->X5 = In->AngleFormula[2];
        FormulaBuf->X7 = In->AngleFormula[3];
        FormulaBuf->X9 = In->AngleFormula[4];
        Out->LensDistoSpec.pAngleFormula = FormulaBuf;
        break;
    default:
        Rval = 1U;
        break;
    }
    return Rval;
}

static inline UINT32 RefCalib_EmFeedAsphericPointMap(const AMBA_CT_EM_ASP_POINT_MAP_s *InAspPointMap, AMBA_CAL_EM_ASP_VIEW_CFG_s *Out)
{
    UINT32 Rval = 0;
    if ((InAspPointMap == NULL) || (Out == NULL)) {
        Rval = 1U;
    } else {
        for (UINT32 Idx = 0U; Idx < 6U; Idx++) {
            Out->PointMap[Idx].Src.X = InAspPointMap->SrcX[Idx];
            Out->PointMap[Idx].Src.Y = InAspPointMap->SrcY[Idx];
            Out->PointMap[Idx].Dst.X = InAspPointMap->DstX[Idx];
            Out->PointMap[Idx].Dst.Y = InAspPointMap->DstY[Idx];
        }
    }
    return Rval;
}

static inline void RefCalib_EmFeedPointMap(const AMBA_CT_EM_CALIB_POINTS_s *InCalibPoints,
    const AMBA_CT_EM_ASSISTANCE_POINTS_s *InAssistancePoints,
    AMBA_CAL_EM_POINT_MAP_s *Out)
{
    UINT32 Idx;
    for (Idx = 0U; Idx < EMIR_CALIB_POINT_NUM; Idx++) {
        Out->CalibPoints[Idx].WorldPos.X = InCalibPoints->WorldPositionX[Idx];
        Out->CalibPoints[Idx].WorldPos.Y = InCalibPoints->WorldPositionY[Idx];
        Out->CalibPoints[Idx].WorldPos.Z = InCalibPoints->WorldPositionZ[Idx];
        Out->CalibPoints[Idx].RawPos.X = InCalibPoints->RawPositionX[Idx];
        Out->CalibPoints[Idx].RawPos.Y = InCalibPoints->RawPositionY[Idx];

        SVC_WRAP_PRINT "WorldPos.X %5f Y %5f Z %5f RawPos.X %5f Y %5f"
        SVC_PRN_ARG_S "CalibPoints"
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  Out->CalibPoints[Idx].WorldPos.X
        SVC_PRN_ARG_DOUBLE  Out->CalibPoints[Idx].WorldPos.Y
        SVC_PRN_ARG_DOUBLE  Out->CalibPoints[Idx].WorldPos.Z
        SVC_PRN_ARG_DOUBLE  Out->CalibPoints[Idx].RawPos.X
        SVC_PRN_ARG_DOUBLE  Out->CalibPoints[Idx].RawPos.Y
        SVC_PRN_ARG_E
    }

    if (EmrQuickCal == ENABLE) {
        for (Idx = 0U; Idx < InAssistancePoints->Number; Idx++) {
            Out->AssistancePoints[Idx].WorldPos.X = InAssistancePoints->WorldPositionX[Idx];
            Out->AssistancePoints[Idx].WorldPos.Y = InAssistancePoints->WorldPositionY[Idx];
            Out->AssistancePoints[Idx].WorldPos.Z = InAssistancePoints->WorldPositionZ[Idx];
            Out->AssistancePoints[Idx].RawPos.X = InAssistancePoints->RawPositionX[Idx];
            Out->AssistancePoints[Idx].RawPos.Y = InAssistancePoints->RawPositionY[Idx];

            SVC_WRAP_PRINT "InAssistancePoints WorldPos.X %5f Y %5f Z %5f RawPos.X %5f Y %5f"
            SVC_PRN_ARG_S "CalibPoints"
            SVC_PRN_ARG_PROC SvcLog_OK
            SVC_PRN_ARG_DOUBLE  Out->AssistancePoints[Idx].WorldPos.X
            SVC_PRN_ARG_DOUBLE  Out->AssistancePoints[Idx].WorldPos.Y
            SVC_PRN_ARG_DOUBLE  Out->AssistancePoints[Idx].WorldPos.Z
            SVC_PRN_ARG_DOUBLE  Out->AssistancePoints[Idx].RawPos.X
            SVC_PRN_ARG_DOUBLE  Out->AssistancePoints[Idx].RawPos.Y
            SVC_PRN_ARG_E
        }
        Out->AssistancePointNumber = InAssistancePoints->Number;
    }

}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void RefCalib_ShowCalibCoordOutputV1(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *CalCoordOutput)
{
    char Buffer[50U];
    UINT8 k;
    UINT32 StrLen;

    AmbaPrint_Flush();
    AmbaPrint_PrintUInt5("Type = %u", (UINT32)CalCoordOutput->Type, 0U, 0U, 0U, 0U);

    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.X, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[1] RefCalib_ShowCalibCoordOutputV1 StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.X = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.Y, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[2] RefCalib_ShowCalibCoordOutputV1 StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.Y = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.Z, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[3] RefCalib_ShowCalibCoordOutputV1 StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.Z = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    AmbaPrint_PrintUInt5("Roi.StartX = %u", CalCoordOutput->Roi.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.StartY = %u", CalCoordOutput->Roi.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.Width = %u", CalCoordOutput->Roi.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.Height = %u", CalCoordOutput->Roi.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.StartX = %u", CalCoordOutput->VoutArea.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.StartY = %u", CalCoordOutput->VoutArea.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.Width = %u", CalCoordOutput->VoutArea.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.Height = %u", CalCoordOutput->VoutArea.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("==========================================================", 0U, 0U, 0U, 0U, 0U);

SVC_WRAP_PRINT "pCalibDataRaw2World %p pCurvedSurface %p"
SVC_PRN_ARG_S "MSG"
SVC_PRN_ARG_PROC SvcLog_OK
SVC_PRN_ARG_CPOINT CalCoordOutput->TransCfg.pCalibDataRaw2World SVC_PRN_ARG_POST
SVC_PRN_ARG_CPOINT CalCoordOutput->TransCfg.pCurvedSurface      SVC_PRN_ARG_POST
SVC_PRN_ARG_E

    for (k = 0U; k < 9U; k++) {
        StrLen = AmbaUtility_DoubleToStr(&Buffer[0U], 50U, CalCoordOutput->TransCfg.pCalibDataRaw2World->CalibMatrix[k], 5U);
        SvcLog_OK(SVC_LOG_CMDAPP, "[4] RefCalib_ShowCalibCoordOutputV1 StrLen = %d", StrLen, 0U);
        AmbaPrint_PrintStr5("pCalibDataRaw2World->CalibMatrix = %s,", &Buffer[0U], NULL, NULL, NULL, NULL);
    }
    StrLen = AmbaUtility_DoubleToStr(&Buffer[0U], 50U, CalCoordOutput->TransCfg.pCalibDataRaw2World->AssistPlaneDist, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[5] RefCalib_ShowCalibCoordOutputV1 StrLen = %d", StrLen, 0U);

    AmbaPrint_PrintStr5("pCalibDataRaw2World->AssistPlaneDist = %s,", &Buffer[0U], NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("==========================================================", 0U, 0U, 0U, 0U, 0U);
}
#endif

static void RefCalib_ShowCalibCoordOutput(const AMBA_CAL_EM_CALC_COORD_CFG_s *CalCoordOutput)
{
    char Buffer[50U];
    UINT8 k;
    UINT32 StrLen;

    AmbaPrint_Flush();
    AmbaPrint_PrintUInt5("Type = %u", (UINT32)CalCoordOutput->Type, 0U, 0U, 0U, 0U);
    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.X, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[1] RefCalib_ShowCalibCoordOutput StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.X = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.Y, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[2] RefCalib_ShowCalibCoordOutput StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.Y = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    StrLen = AmbaUtility_DoubleToStr(&Buffer[0], 50U, CalCoordOutput->CamPos.Z, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[3] RefCalib_ShowCalibCoordOutput StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("CamPos.Z = %s,", &Buffer[0], NULL, NULL, NULL, NULL);

    AmbaPrint_PrintUInt5("Roi.StartX = %u", CalCoordOutput->Roi.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.StartY = %u", CalCoordOutput->Roi.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.Width = %u", CalCoordOutput->Roi.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Roi.Height = %u", CalCoordOutput->Roi.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.StartX = %u", CalCoordOutput->VoutArea.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.StartY = %u", CalCoordOutput->VoutArea.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.Width = %u", CalCoordOutput->VoutArea.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("VoutArea.Height = %u", CalCoordOutput->VoutArea.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("==========================================================", 0U, 0U, 0U, 0U, 0U);

SVC_WRAP_PRINT "pCalibDataRaw2World %p pCurvedSurface %p"
SVC_PRN_ARG_S "MSG"
SVC_PRN_ARG_PROC SvcLog_OK
SVC_PRN_ARG_CPOINT CalCoordOutput->TransCfg.pCalibDataRaw2World SVC_PRN_ARG_POST
SVC_PRN_ARG_CPOINT CalCoordOutput->TransCfg.pCurvedSurface      SVC_PRN_ARG_POST
SVC_PRN_ARG_E

    for (k = 0U; k < 9U; k++) {
        StrLen = AmbaUtility_DoubleToStr(&Buffer[0U], 50U, CalCoordOutput->TransCfg.pCalibDataRaw2World->CalibMatrix[k], 5U);
        SvcLog_OK(SVC_LOG_CMDAPP, "[4] RefCalib_ShowCalibCoordOutput StrLen = %d", StrLen, 0U);
        AmbaPrint_PrintStr5("pCalibDataRaw2World->CalibMatrix = %s,", &Buffer[0U], NULL, NULL, NULL, NULL);
    }
    StrLen = AmbaUtility_DoubleToStr(&Buffer[0U], 50U, CalCoordOutput->TransCfg.pCalibDataRaw2World->AssistPlaneDist, 5U);
    SvcLog_OK(SVC_LOG_CMDAPP, "[5] RefCalib_ShowCalibCoordOutput StrLen = %d", StrLen, 0U);
    AmbaPrint_PrintStr5("pCalibDataRaw2World->AssistPlaneDist = %s,", &Buffer[0U], NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("==========================================================", 0U, 0U, 0U, 0U, 0U);
}

static void RefCalib_ConverToPixel(AMBA_CAL_CAM_s *Cam)
{
    UINT32 i;
    UINT32 TableSize = Cam->Lens.LensDistoSpec.pRealExpect->Length;
    DOUBLE CellSize = Cam->Sensor.CellSize;

#if 0
AmbaPrint_PrintUInt5("file value(mm)", 0U, 0U, 0U, 0U, 0U);
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pRealTable = %4.8f"
    SVC_PRN_ARG_S "pRealTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i]
    SVC_PRN_ARG_E
}
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pExceptTable = %4.8f"
    SVC_PRN_ARG_S "pExceptTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i]
    SVC_PRN_ARG_E
}
#endif

    Cam->Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
    for (i = 0; i < TableSize; i++) {
        if ((Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] >= 0.0) && (Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] >= 0.0)) {
            Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] = Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] / CellSize;
            Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] = Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] / CellSize;
        } else {
            AmbaPrint_PrintUInt5("[ERROR] Invalid table!", 0U, 0U, 0U, 0U, 0U);
        }
    }

#if 0
AmbaPrint_PrintUInt5("AMBA_CAL_LD_PIXEL value(/CellSize)", 0U, 0U, 0U, 0U, 0U);
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pRealTable = %4.8f"
    SVC_PRN_ARG_S "pRealTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i]
    SVC_PRN_ARG_E
}
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pExceptTable = %4.8f"
    SVC_PRN_ARG_S "pExceptTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i]
    SVC_PRN_ARG_E
}
#endif

}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static void RefCalib_ConverToPixelV3(AMBA_CAL_CAM_V3_s *Cam)
{
    UINT32 i;
    UINT32 TableSize = Cam->Lens.LensDistoSpec.pRealExpect->Length;
    DOUBLE CellSize = Cam->Sensor.CellSize;

#if 0
AmbaPrint_PrintUInt5("file value(mm)", 0U, 0U, 0U, 0U, 0U);
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pRealTable = %4.8f"
    SVC_PRN_ARG_S "pRealTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i]
    SVC_PRN_ARG_E
}
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pExceptTable = %4.8f"
    SVC_PRN_ARG_S "pExceptTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i]
    SVC_PRN_ARG_E
}
#endif

    Cam->Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
    for (i = 0; i < TableSize; i++) {
        if ((Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] >= 0.0) && (Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] >= 0.0)) {
            Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] = Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i] / CellSize;
            Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] = Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i] / CellSize;
        } else {
            AmbaPrint_PrintUInt5("[ERROR] Invalid table!", 0U, 0U, 0U, 0U, 0U);
        }
    }

#if 0
AmbaPrint_PrintUInt5("AMBA_CAL_LD_PIXEL value(/CellSize)", 0U, 0U, 0U, 0U, 0U);
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pRealTable = %4.8f"
    SVC_PRN_ARG_S "pRealTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pRealTbl[i]
    SVC_PRN_ARG_E
}
for (UINT32 i = 0; i< TableSize; i++) {
    SVC_WRAP_PRINT "pExceptTable = %4.8f"
    SVC_PRN_ARG_S "pExceptTable"
    SVC_PRN_ARG_PROC SvcLog_OK
    SVC_PRN_ARG_DOUBLE  Cam->Lens.LensDistoSpec.pRealExpect->pExpectTbl[i]
    SVC_PRN_ARG_E
}
#endif

}
#endif

static UINT32 AmbaCT_EmCbMsgReciver(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg)
{
    UINT32 Rval = CT_OK;

    switch (Type) {
    case AMBA_EM_MSG_GRID_STATUS:
        break;
    case AMBA_EM_MSG_PTN_ERR:
        for (UINT32 Idx = 0U; Idx < pMsg->pPatternError->AssistancePointNumber; Idx++) {
            SVC_WRAP_PRINT "AMBA_EM_MSG_PTN_ERR WorldPos.X %4.0f Y %4.0f"
            SVC_PRN_ARG_S "CalibPoints"
            SVC_PRN_ARG_PROC SvcLog_OK
            SVC_PRN_ARG_DOUBLE  pMsg->pPatternError->AssistancePointsErr[Idx].X
            SVC_PRN_ARG_DOUBLE  pMsg->pPatternError->AssistancePointsErr[Idx].Y
            SVC_PRN_ARG_E
        }
        break;
    case AMBA_EM_MSG_REPORT:
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }

    AmbaMisra_TouchUnused(&pMsg);
    return Rval;
}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static UINT32 RefCalib_ConvImgToWorldPlaneV1(AMBA_CAL_EM_SV_CFG_V1_s EmrCalibConfig,
                                             AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCalibCoordCfgV1,
                                             AMBA_CAL_EM_CALIB_INFO_DATA_V1_s *pCalOutput,
                                             DOUBLE FocalLength)
{
    UINT32 Rval;
    AMBA_CAL_EM_CALIB_INFO_CFG_V1_s EmCalibCfg;
    static ULONG  EmirCalibBuf;
    static UINT32 EmirCalibBufSize;
    static void *pEmirCalibWorkingBuf;
    SVC_USER_PREF_s  *pSvcUserPref;
    ULONG            PrefBufAddr;
    UINT32           PrefBufSize;;

    if (SVC_OK != AmbaWrap_memset(&EmCalibCfg, 0, sizeof(AMBA_CAL_EM_CALIB_INFO_CFG_V1_s))) {
        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmCalibCfg failed", 0U, 0U);
    }

#if defined(CONFIG_ICAM_PROJECT_EMIRROR)
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB_EMIRROR, &EmirCalibBuf, &EmirCalibBufSize);
    SvcLog_OK(SVC_LOG_CMDAPP, "Use CALIB_EMIRROR memory", 0U, 0U);
#elif defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ADAS_LDWS_CALIB, &EmirCalibBuf, &EmirCalibBufSize);
    SvcLog_OK(SVC_LOG_CMDAPP, "Use ADAS_CALIB memory", 0U, 0U);
#else
    Rval = SVC_NG;
    EmirCalibBufSize = 0U;
    SvcLog_NG(SVC_LOG_CMDAPP, "Something wrong, memory allocate for EmirCalibBuf NG!!", 0U, 0U);
#endif

    if (Rval == 0U) {
        AmbaMisra_TypeCast(&pEmirCalibWorkingBuf, &EmirCalibBuf);

        EmCalibCfg.Cam.Lens.LensDistoType = EmrCalibConfig.Cam.Lens.LensDistoType;
        EmCalibCfg.Cam.Lens.LensDistoUnit = EmrCalibConfig.Cam.Lens.LensDistoUnit;

        EmCalibCfg.Cam.Lens.LensDistoSpec.pRealExpect = EmrCalibConfig.Cam.Lens.LensDistoSpec.pRealExpect;
        EmCalibCfg.Cam.Lens.LensDistoSpec.pAngle = EmrCalibConfig.Cam.Lens.LensDistoSpec.pAngle;
        EmCalibCfg.Cam.Lens.LensDistoSpec.pRealExpectFormula = EmrCalibConfig.Cam.Lens.LensDistoSpec.pRealExpectFormula;
        EmCalibCfg.Cam.Lens.LensDistoSpec.pAngleFormula = EmrCalibConfig.Cam.Lens.LensDistoSpec.pAngleFormula;
        EmCalibCfg.Cam.Lens.LensDistoSpec.pUserDefPinhoFunc = NULL;
        EmCalibCfg.Cam.Lens.LensDistoSpec.pUserDefAngleFunc = NULL;



        AmbaSvcWrap_MisraMemcpy(&EmCalibCfg.Cam.Sensor, &EmrCalibConfig.Cam.Sensor, sizeof(AMBA_CAL_SENSOR_s));
        AmbaSvcWrap_MisraMemcpy(&EmCalibCfg.Cam.OpticalCenter, &EmrCalibConfig.Cam.OpticalCenter, sizeof(AMBA_CAL_POINT_DB_2D_s));
        AmbaSvcWrap_MisraMemcpy(&EmCalibCfg.Cam.Pos, &EmrCalibConfig.Cam.Pos, sizeof(AMBA_CAL_POINT_DB_3D_s));
        EmCalibCfg.Cam.Rotation = EmrCalibConfig.Cam.Rotation;
        EmCalibCfg.Cam.FocalLength.X = FocalLength;
        EmCalibCfg.Cam.FocalLength.Y = FocalLength;

        SVC_WRAP_PRINT "FocalLength = %5f, cell_size = %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  FocalLength
        SVC_PRN_ARG_DOUBLE  EmCalibCfg.Cam.Sensor.CellSize
        SVC_PRN_ARG_E

        /* Save FocalLength and cell size into pref */
        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
        }
        pSvcUserPref->CalibFocalLength = FocalLength;
        pSvcUserPref->CalibCellSize = EmCalibCfg.Cam.Sensor.CellSize;
        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
        }

        RefCalib_ConverToPixelV3(&EmCalibCfg.Cam);
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.Type, &EmrCalibConfig.View.Type, sizeof(AMBA_CAL_EM_VIEW_TYPE_e));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.VoutArea, &EmrCalibConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.Calibinfo, &EmrCalibConfig.Calibinfo, sizeof(AMBA_CAL_EM_CALIB_POINT_INFO_s));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.OptimizeLevel, &EmrCalibConfig.OptimizeLevel, sizeof(AMBA_CAL_EM_OPTIMIZE_LEVEL_e));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalibCfg.Type/VoutArea/Calibinfo/OptimizeLevel failed", 0U, 0U);
        }
        EmCalibCfg.PointMapHFlipEnable = 0U;
        EmCalibCfg.PlugIn.MsgReciverCfg.PatternError = 1;
        EmCalibCfg.PlugIn.MsgReciver = AmbaCT_EmCbMsgReciver;
        Rval = AmbaCal_EmGenCalibInfoV1(&EmCalibCfg, pEmirCalibWorkingBuf, pCalOutput);

        SVC_WRAP_PRINT "CameraRotationMatrix[0~4] %5f, %5f, %5f, %5f, %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[0]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[1]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[2]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[3]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[4]
        SVC_PRN_ARG_E

        SVC_WRAP_PRINT "CameraRotationMatrix[5~8] %5f, %5f, %5f, %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[5]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[6]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[7]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraRotationMatrix[8]
        SVC_PRN_ARG_E

        SVC_WRAP_PRINT "CameraTranslationMatrix[0~3] %5f, %5f, %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraTranslationMatrix[0]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraTranslationMatrix[1]
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraTranslationMatrix[2]
        SVC_PRN_ARG_E

        SVC_WRAP_PRINT "CameraEulerAngle Pitch: %5f, Roll: %5f, Yaw: %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraEulerAngle.Pitch
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraEulerAngle.Roll
        SVC_PRN_ARG_DOUBLE  pCalOutput->CameraEulerAngle.Yaw
        SVC_PRN_ARG_E

        SVC_WRAP_PRINT "CalculatedCameraPos X: %5f, Y: %5f, Z: %5f"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_OK
        SVC_PRN_ARG_DOUBLE  pCalOutput->CalculatedCameraPos.X
        SVC_PRN_ARG_DOUBLE  pCalOutput->CalculatedCameraPos.Y
        SVC_PRN_ARG_DOUBLE  pCalOutput->CalculatedCameraPos.Z
        SVC_PRN_ARG_E

        if (Rval == 0U) {
            pCalibCoordCfgV1->Type = EmrCalibConfig.View.Type;
            Rval |= AmbaWrap_memcpy(&pCalibCoordCfgV1->CamPos, &EmrCalibConfig.Cam.Pos, sizeof(AMBA_CAL_POINT_DB_3D_s));
            Rval |= AmbaWrap_memcpy(&pCalibCoordCfgV1->Roi, &EmrCalibConfig.ROI, sizeof(AMBA_CAL_ROI_s));
            Rval |= AmbaWrap_memcpy(&pCalibCoordCfgV1->VoutArea, &EmrCalibConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy pCalibCoordCfgV1->CamPos/Roi/VoutArea failed", 0U, 0U);
            }
            pCalibCoordCfgV1->TransCfg.pCalibDataRaw2World = &pCalOutput->CalibDataRaw2World;
        } else {
            AmbaPrint_PrintStr5("%s, AmbaCal_EmGenCalibInfo() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaCal_EmGetCalibInfoBufSize() failed!", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}
#endif

static UINT32 RefCalib_ConvImgToWorldPlane(AMBA_CAL_EM_SV_CFG_s EmrCalibConfig, AMBA_CAL_EM_CALC_COORD_CFG_s *CalibCoordCfg)
{
    UINT32 Rval;
    AMBA_CAL_EM_CALIB_INFO_CFG_s EmCalibCfg;
    static AMBA_CAL_EM_CALIB_INFO_DATA_s CalOutput;
    static ULONG  EmirCalibBuf;
    static UINT32 EmirCalibBufSize;
    static void *pEmirCalibWorkingBuf;
    //void *RawBuffer;

    if (SVC_OK != AmbaWrap_memset(&EmCalibCfg, 0, sizeof(AMBA_CAL_EM_CALIB_INFO_CFG_s))) {
        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmCalibCfg failed", 0U, 0U);
    }

#if defined(CONFIG_ICAM_PROJECT_EMIRROR)
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB_EMIRROR, &EmirCalibBuf, &EmirCalibBufSize);
    SvcLog_OK(SVC_LOG_CMDAPP, "Use CALIB_EMIRROR memory", 0U, 0U);
#elif defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ADAS_LDWS_CALIB, &EmirCalibBuf, &EmirCalibBufSize);
    SvcLog_OK(SVC_LOG_CMDAPP, "Use ADAS_CALIB memory", 0U, 0U);
#else
    Rval = SVC_NG;
    EmirCalibBufSize = 0U;
    SvcLog_NG(SVC_LOG_CMDAPP, "Something wrong, memory allocate for EmirCalibBuf NG!!", 0U, 0U);
#endif
    if (Rval == 0U) {
        AmbaMisra_TypeCast(&pEmirCalibWorkingBuf, &EmirCalibBuf);
        if (SVC_OK != AmbaWrap_memcpy(&EmCalibCfg.Cam, &EmrCalibConfig.Cam, sizeof(AMBA_CAL_CAM_s))) {
            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalibCfg.Cam failed", 0U, 0U);
        }
        RefCalib_ConverToPixel(&EmCalibCfg.Cam);
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.Type, &EmrCalibConfig.View.Type, sizeof(AMBA_CAL_EM_VIEW_TYPE_e));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.VoutArea, &EmrCalibConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.Calibinfo, &EmrCalibConfig.Calibinfo, sizeof(AMBA_CAL_EM_CALIB_POINT_INFO_s));
        Rval |= AmbaWrap_memcpy(&EmCalibCfg.OptimizeLevel, &EmrCalibConfig.OptimizeLevel, sizeof(AMBA_CAL_EM_OPTIMIZE_LEVEL_e));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalibCfg.Type/VoutArea/Calibinfo/OptimizeLevel failed", 0U, 0U);
        }
        EmCalibCfg.PointMapHFlipEnable = 0U;
        EmCalibCfg.PlugIn.MsgReciverCfg.PatternError = 1;
        EmCalibCfg.PlugIn.MsgReciver = AmbaCT_EmCbMsgReciver;
        Rval = AmbaCal_EmGenCalibInfo(&EmCalibCfg, pEmirCalibWorkingBuf, &CalOutput);
        if (Rval == 0U) {
            CalibCoordCfg->Type = EmrCalibConfig.View.Type;
            Rval |= AmbaWrap_memcpy(&CalibCoordCfg->CamPos, &EmrCalibConfig.Cam.Pos, sizeof(AMBA_CAL_POINT_DB_3D_s));
            Rval |= AmbaWrap_memcpy(&CalibCoordCfg->Roi, &EmrCalibConfig.ROI, sizeof(AMBA_CAL_ROI_s));
            Rval |= AmbaWrap_memcpy(&CalibCoordCfg->VoutArea, &EmrCalibConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy CalibCoordCfg->CamPos/Roi/VoutArea failed", 0U, 0U);
            }
            CalibCoordCfg->TransCfg.pCalibDataRaw2World = &CalOutput.CalibDataRaw2World;
        } else {
            AmbaPrint_PrintStr5("%s, AmbaCal_EmGenCalibInfo() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaCal_EmGetCalibInfoBufSize() failed!", __func__, NULL, NULL, NULL, NULL);
    }

    return Rval;
}

static AMBA_CT_EM_CALIB_POINTS_s TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_MAX] = {
                                                                            {
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                            {   {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                            {   {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                        };
static AMBA_CT_EM_CALIB_POINTS_s TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_MAX] = {
                                                                            {
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                            {   {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                            {   {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                                {0.0f, 0.0f, 0.0f, 0.0f},
                                                                            },
                                                                        };

static void SvcCalib_SearchRectangleFromYuv(UINT8 Source, UINT8 Fov)
{
    #define DeBug   0U
    #define MainYuv 1U
    //#define DetTectBufSize 58521264U
    #define YBufSize 1920*1080
    #define SearchPoint 4U

    char CmdYuvFailename[64U];
    char CmdRecRangeFN[64U];
    UINT32 Rval;
    AMBA_VFS_FILE_s Fid;
    AMBA_FS_FILE *pFile = NULL;
    void *pBuf = NULL;
    UINT32 NumSuccess = 0U;
    SVC_USER_PREF_s  *pSvcUserPref = NULL;
    AMBA_DSP_RAW_BUF_s RawInfo;
    UINT32 RecDetBufSize = 0U;
    ULONG RecDetWorkBuf = 0U;
    void *pRecDetWorkBuf = NULL;
    AMBA_CT_INITIAL_CONFIG_s InitCfg;
    UINT32 WorkBufSize = 0U;
    ULONG TunerWorkBuf = 0U;
    void *pTunerWorkBuf = NULL;
    static UINT8 SrcImg[YBufSize];

    AMBA_CAL_DET_RECT_REPORT_s *OutCorners = NULL;
    AMBA_CAL_DET_RECT_DET_CFG_s Cfg  = {
        .Src = {
            .pSrcAddr = &SrcImg[0],
            .ImgSize = {1920U, 1080U}
        },
        .DetectRegion = {
            .Area = {0U, 0U, 0U, 0U},
            .TuneCfg = {0.0, 0.0},
            .FilterCfg = {0U, 0U},
            .MaxEpsilon = 6U,
        },
        .pDebugReport = NULL,
        .DebugId = 1U
    };

    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
    } else {
        if(Fov == rear) {
            AmbaUtility_StringCopy(CmdYuvFailename, sizeof(CmdYuvFailename),  "c:\\0_1920x1080y");
            AmbaUtility_StringCopy(CmdRecRangeFN, sizeof(CmdRecRangeFN),      "C:\\rear_search_range.txt");
        } else if(Fov == left) {
            AmbaUtility_StringCopy(CmdYuvFailename, sizeof(CmdYuvFailename),  "c:\\1_1920x1080y");
            AmbaUtility_StringCopy(CmdRecRangeFN, sizeof(CmdRecRangeFN),      "C:\\left_search_range.txt");
        } else {
            AmbaUtility_StringCopy(CmdYuvFailename, sizeof(CmdYuvFailename),  "c:\\2_1920x1080y");
            AmbaUtility_StringCopy(CmdRecRangeFN, sizeof(CmdRecRangeFN),      "C:\\right_search_range.txt");
        }

        CmdRecRangeFN[0U] = pSvcUserPref->MainStgDrive[0];
        CmdYuvFailename[0U] = pSvcUserPref->MainStgDrive[0];
    }

    #if DeBug
    SVC_WRAP_PRINT "YuvFB %s RecRangeFN %s"
    SVC_PRN_ARG_S __func__
    SVC_PRN_ARG_PROC SvcLog_NG
    SVC_PRN_ARG_STR CmdYuvFailename SVC_PRN_ARG_POST
    SVC_PRN_ARG_STR CmdRecRangeFN   SVC_PRN_ARG_POST
    SVC_PRN_ARG_E
    #endif

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &TunerWorkBuf, &WorkBufSize)) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
    }
    InitCfg.TunerWorkingBufSize = WorkBufSize;
    AmbaMisra_TypeCast(&pTunerWorkBuf, &TunerWorkBuf);
    InitCfg.pTunerWorkingBuf = pTunerWorkBuf;
    if (SVC_OK != AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg)) {
        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init AMBA_CT_TYPE_EMIRROR failed!!", 0U, 0U);
    }

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_REC_DET, &RecDetWorkBuf, &RecDetBufSize)) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_REC_DET failed!!", 0U, 0U);
    }
    AmbaMisra_TypeCast(&pRecDetWorkBuf, &RecDetWorkBuf);

    if(Source == MainYuv){
#ifdef CONFIG_ICAM_STLCAP_USED
        AMBA_DSP_BUF_s CeInfo;

        Rval = SvcStillTask_GetCapBuffer(&RawInfo, &CeInfo, 0, 0);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
        }
#endif
        #if DeBug
        SVC_WRAP_PRINT "RawInfo BaseAddr 0x%llX "
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_NG
        SVC_PRN_ARG_UINT64 RawInfo.BaseAddr
        SVC_PRN_ARG_E
        #endif

        Rval = AmbaFS_FileOpen(CmdYuvFailename, "w", &pFile);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
        }
        AmbaMisra_TypeCast(&pBuf, &(RawInfo.BaseAddr));
        Rval = AmbaFS_FileWrite(pBuf, 1, YBufSize, pFile, &NumSuccess);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
        }
        Rval = AmbaFS_FileClose(pFile);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
        }
    } else {
        //
    }

    Rval = AmbaVFS_Open(CmdYuvFailename, "r", 1U, &Fid);
    if(Rval != SVC_OK){
        SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
    }
    Rval = AmbaVFS_Read(&SrcImg[0], 1, YBufSize, &Fid, &NumSuccess);
    if(Rval != SVC_OK){
        SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
    }
    Rval = AmbaVFS_Close(&Fid);
    if(Rval != SVC_OK){
        SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
    }
    Rval = AmbaCT_Load(CmdRecRangeFN);
    if(Rval != SVC_OK) {
        SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
    }

    for(UINT32 i=0; i<SearchPoint; i++) {
        AmbaCT_EmGetRoi(i, &Cfg.DetectRegion.Area);
        #if DeBug
        SVC_WRAP_PRINT "Rectange Search range (%d, %d, %d, %d)"
        SVC_PRN_ARG_S __func__
        SVC_PRN_ARG_PROC SvcLog_NG
        SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.StartX SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.StartY SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.Width  SVC_PRN_ARG_POST
        SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.Height SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
        #endif
        if (SVC_OK != AmbaWrap_memset(pRecDetWorkBuf, 0, RecDetBufSize)) {
            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset pRecDetWorkBuf failed!!", 0U, 0U);
        }
        Rval = AmbaCal_DetRectPattern(&Cfg, pRecDetWorkBuf, &OutCorners);
        if(Rval != SVC_OK) {
            SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
        }

        if (OutCorners != NULL) {
            TEMP_AMBA_CT_EM_CALIB_POINTS[Fov].RawPositionX[i]       = OutCorners->DetPattern[0U].Corners[0U].X + (DOUBLE)Cfg.DetectRegion.Area.StartX;
            TEMP_AMBA_CT_EM_CALIB_POINTS[Fov].RawPositionY[i]       = OutCorners->DetPattern[0U].Corners[0U].Y + (DOUBLE)Cfg.DetectRegion.Area.StartY;
            TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[Fov].RawPositionX[i]  = OutCorners->DetPattern[0U].Corners[2U].X + (DOUBLE)Cfg.DetectRegion.Area.StartX;
            TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[Fov].RawPositionY[i]  = OutCorners->DetPattern[0U].Corners[2U].Y + (DOUBLE)Cfg.DetectRegion.Area.StartY;
            if(OutCorners->DetectNum == 1U){
                SVC_WRAP_PRINT "(%1.1f, %1.1f)"
                SVC_PRN_ARG_S __func__
                SVC_PRN_ARG_DOUBLE TEMP_AMBA_CT_EM_CALIB_POINTS[Fov].RawPositionX[i]
                SVC_PRN_ARG_DOUBLE TEMP_AMBA_CT_EM_CALIB_POINTS[Fov].RawPositionY[i]
                //SVC_PRN_ARG_DOUBLE TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[Fov].RawPositionX[0]
                //SVC_PRN_ARG_DOUBLE TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[Fov].RawPositionY[0]
                SVC_PRN_ARG_E
            } else {
                SVC_WRAP_PRINT "Rectange Num %d Search range (%d, %d, %d, %d) need change"
                SVC_PRN_ARG_S __func__
                SVC_PRN_ARG_PROC SvcLog_NG
                SVC_PRN_ARG_UINT32 OutCorners->DetectNum        SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 Cfg.DetectRegion.Area.Height SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            }
        }
    }

    if(Rval != SVC_OK){
        SvcLog_NG(__func__, "Line %d Rval %d", __LINE__, Rval);
    }
}
#endif
#endif

#if 0
static void RefCalib_ShowCalibWarpOutput(const AMBA_CAL_EM_SV_DATA_s *CalWarpOutput)
{
    UINT8 i;
    AmbaPrint_PrintUInt5("Version = %u", CalWarpOutput->Version, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.Version = %u", CalWarpOutput->WarpTbl.Version, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.StartX = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.StartX, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.StartY = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.StartY, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.Width = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.Width, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.Height = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.Height, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.HSubSample.FactorNum = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.HSubSample.FactorNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.HSubSample.FactorDen = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.HSubSample.FactorDen, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.VSubSample.FactorNum = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.VSubSample.FactorNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.CalibSensorGeo.VSubSample.FactorDen = %u", CalWarpOutput->WarpTbl.CalibSensorGeo.VSubSample.FactorDen, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.HorGridNum = %u", CalWarpOutput->WarpTbl.HorGridNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.VerGridNum = %u", CalWarpOutput->WarpTbl.VerGridNum, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.TileWidthExp = %u", CalWarpOutput->WarpTbl.TileWidthExp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("WarpTbl.TileHeightExp = %u", CalWarpOutput->WarpTbl.TileHeightExp, 0U, 0U, 0U, 0U);
    /* Check first ten warp data */
    for (i = 0U; i < 10U; i++) {
        AmbaPrint_PrintInt5("WarpTbl.WarpVector[%d].X = %d", (INT16)i, CalWarpOutput->WarpTbl.WarpVector[i].X, 0, 0, 0);
        AmbaPrint_PrintInt5("WarpTbl.WarpVector[%d].Y = %d", (INT16)i, CalWarpOutput->WarpTbl.WarpVector[i].Y, 0, 0, 0);
    }
    AmbaPrint_PrintUInt5("---------------------------------------", 0U, 0U, 0U, 0U, 0U);
}
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
static UINT32 ParseAutoCalibScript(void)
{
    SVC_USER_PREF_s  *pSvcUserPref;
    UINT32 Rval = SVC_OK;
    UINT8  Misra_0;
    UINT32 SkipFlow = 0U;
    AMBA_CT_INITIAL_CONFIG_s InitCfg;
    static UINT32 EmirWorkBufSize;
    static ULONG  Emir3in3TunerWorkBuf;
    static void *pEmir3in3TunerWorkBuf;

    static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;

    static AMBA_CAL_EM_SV_CFG_s Emirror3in3WarpConfig GNU_SECTION_NOZEROINIT;

    static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CALC_COORD_CFG_s CalibCoordCfg GNU_SECTION_NOZEROINIT;

    static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;
    const char ADASCalibFileName[30] = ":\\adas_tuner_front_auto.txt";
    char ADASscrript[30];

    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
    if(Rval != 0U) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
    }
    AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
    InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
    AmbaMisra_TypeCast(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
    InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
    Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
    if(Rval != 0U) {
        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
    }

    AmbaSvcWrap_MisraMemset(&ADASscrript, 0, sizeof(ADASscrript));
    Rval = SvcUserPref_Get(&pSvcUserPref);
    if(Rval != 0U) {
        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get failed!!", 0U, 0U);
    }

    ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
    CmdAPP_strcat(ADASscrript, ADASCalibFileName);

    Rval = AmbaCT_Load(ADASscrript);
    if(Rval != 0U) {
        AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
        SkipFlow = 1U;
    } else {
        AmbaPrint_PrintStr5("AmbaCT_Load %s OK!", ADASscrript, NULL, NULL, NULL, NULL);
    }

    if(SkipFlow == 0U) {

        AmbaSvcWrap_MisraMemset(&LensData, 0, sizeof(LensData));
        AmbaSvcWrap_MisraMemset(&CameraData, 0, sizeof(CameraData));
        AmbaSvcWrap_MisraMemset(&AspPointMap, 0, sizeof(AspPointMap));
        AmbaSvcWrap_MisraMemset(&CalibPointData, 0, sizeof(CalibPointData));
        AmbaSvcWrap_MisraMemset(&AssistancePointData, 0, sizeof(AssistancePointData));
        AmbaSvcWrap_MisraMemset(&Emirror3in3WarpConfig, 0, sizeof(Emirror3in3WarpConfig));
        AmbaSvcWrap_MisraMemset(&Aspheric, 0, sizeof(Aspheric));
        AmbaSvcWrap_MisraMemset(&CurvedSurface, 0, sizeof(CurvedSurface));
        AmbaSvcWrap_MisraMemset(&CalibCoordCfg, 0, sizeof(CalibCoordCfg));
        AmbaSvcWrap_MisraMemset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
        AmbaSvcWrap_MisraMemset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
        AmbaSvcWrap_MisraMemset(&LensSpecFormula, 0, sizeof(LensSpecFormula));

        AmbaCT_EmGetLens(0U, &LensData);
        Rval = RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
        if(Rval != 0U) {
            SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_EmFeedLensSpec failed!!", 0U, 0U);
        }
        AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfig.Cam.Sensor);
        AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfig.Cam.OpticalCenter);
        AmbaCT_EmGetCamera(0U, &CameraData);
        Emirror3in3WarpConfig.Cam.Pos.X = CameraData.PositionX;
        Emirror3in3WarpConfig.Cam.Pos.Y = CameraData.PositionY;
        Emirror3in3WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
        Emirror3in3WarpConfig.Cam.Rotation = CameraData.RotateType;
        AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfig.VinSensorGeo);
        AmbaCT_EmGetSingleView(&Emirror3in3WarpConfig.View.Type);
        if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_LDC) {
            AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else {
            AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfig.View.VoutArea);
        Emirror3in3WarpConfig.View.PlugIn.PlugInMode = 0U; // TODO:
        //Emirror3in3WarpConfig.View.PlugIn.MsgReciver = RefCalib_EmSvCbMsgReciver;

        AmbaCT_EmGetCalibPoints(0U, &CalibPointData);
        AmbaCT_EmGetAssistancePoints(0U, &AssistancePointData);

        AmbaSvcWrap_MisraMemset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s));
        RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
        Emirror3in3WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
        Emirror3in3WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
        Emirror3in3WarpConfig.Calibinfo.pMultiPoint = NULL;
        AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
        AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
        Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
        AmbaCT_EmGetOptimize(0U, &Misra_0);
        AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);

        //mm to PIXEL
        {
            DOUBLE *real, *expect;

            real = Emirror3in3WarpConfig.Cam.Lens.LensDistoSpec.pRealExpect->pRealTbl;
            expect = Emirror3in3WarpConfig.Cam.Lens.LensDistoSpec.pRealExpect->pExpectTbl;
            for (UINT32 k = 0U; k< Emirror3in3WarpConfig.Cam.Lens.LensDistoSpec.pRealExpect->Length; k++) {
                real[k] /= Emirror3in3WarpConfig.Cam.Sensor.CellSize;
                expect[k] /= Emirror3in3WarpConfig.Cam.Sensor.CellSize;
            }
        }

        //Auto Calibration Info Setup
        {
            SvcAdasAutoCal_SetupRawRoi(&(Emirror3in3WarpConfig.ROI));
            SvcAdasAutoCal_SetupVoutView(&(Emirror3in3WarpConfig.View.VoutArea));
            SvcAdasAutoCal_SetupDefCalibPoints(&CalibPointData);
            SvcAdasAutoCal_SetupCam(&(Emirror3in3WarpConfig.Cam));
        }
    }

    return Rval;
}
#endif

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#define PI 3.14159265
static void QuickCalib(UINT32 Horizontal, DOUBLE FocalLength, INT32 warning_zone_x_offset)
{
    static UINT32 VoutHorizonY = 0U;
    INT32 ZoneX_Offset = warning_zone_x_offset;
    DOUBLE Radian, Degree;
    UINT8 Misra_0;
    UINT32 Rval = SVC_OK, WarningZoneXinPixel = 0U;
    static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_SV_CFG_s Emirror3in3WarpConfig GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CALC_COORD_CFG_s CalibCoordCfg GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;

    /* Get final CalibHorizontal line position */
    VoutHorizonY = SvcAdasNotify_SetCalibHorizontal(0U, Horizontal);
    SvcLog_OK(__func__, "---- UserVoutHorizonY = %d ----", VoutHorizonY, 0U);

    {
        AmbaSvcWrap_MisraMemset(&LensData, 0, sizeof(LensData));
        AmbaSvcWrap_MisraMemset(&CameraData, 0, sizeof(CameraData));
        AmbaSvcWrap_MisraMemset(&AspPointMap, 0, sizeof(AspPointMap));
        AmbaSvcWrap_MisraMemset(&CalibPointData, 0, sizeof(CalibPointData));
        AmbaSvcWrap_MisraMemset(&AssistancePointData, 0, sizeof(AssistancePointData));
        AmbaSvcWrap_MisraMemset(&Emirror3in3WarpConfig, 0, sizeof(Emirror3in3WarpConfig));
        AmbaSvcWrap_MisraMemset(&Aspheric, 0, sizeof(Aspheric));
        AmbaSvcWrap_MisraMemset(&CurvedSurface, 0, sizeof(CurvedSurface));
        AmbaSvcWrap_MisraMemset(&CalibCoordCfg, 0, sizeof(CalibCoordCfg));
        AmbaSvcWrap_MisraMemset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
        AmbaSvcWrap_MisraMemset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
        AmbaSvcWrap_MisraMemset(&LensSpecFormula, 0, sizeof(LensSpecFormula));

        AmbaCT_EmGetLens(0U, &LensData);
        Rval = RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
        if(Rval != 0U) {
            SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_EmFeedLensSpec failed!!", 0U, 0U);
        }
        AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfig.Cam.Sensor);
        AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfig.Cam.OpticalCenter);
        AmbaCT_EmGetCamera(0U, &CameraData);
        Emirror3in3WarpConfig.Cam.Pos.X = CameraData.PositionX;
        Emirror3in3WarpConfig.Cam.Pos.Y = CameraData.PositionY;
        Emirror3in3WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
        Emirror3in3WarpConfig.Cam.Rotation = CameraData.RotateType;
        AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfig.VinSensorGeo);
        AmbaCT_EmGetSingleView(&Emirror3in3WarpConfig.View.Type);
        if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_LDC) {
            AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else {
            AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfig.View.VoutArea);
        Emirror3in3WarpConfig.View.PlugIn.PlugInMode = 0U; // TODO:
        AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
        AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
        Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
        AmbaCT_EmGetOptimize(0U, &Misra_0);
        AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);

        /* Quick calibaration start from here */
        {
            char   DoubleBuf[32U], DoubleBuf2[32U];
            AMBA_CAL_EXT_PITCH_DATA_s Output;
            static void *pQuickCalibWorkBuf;
            AMBA_CAL_EXT_PITCH_CFG_s FcwsCfg;
            static UINT32 QuickCalibWorkBufSize;
            static ULONG  QuickCalibWorkBuf;
            UINT32 StrLen;

            AMBA_CAL_LENS_DST_REAL_EXPECT_s LensLdcTable = {
                .Length = LensData.TableLen,
                .pRealTbl = LensData.pRealTable,
                .pExpectTbl = LensData.pExceptTable
            };
            AMBA_CAL_CAM_V2_s CamInfo = {
                .Lens = {
                    .LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL,
                    .LensDistoUnit = AMBA_CAL_LD_MM,
                    .LensDistoSpec = {
                        .pRealExpect = &LensLdcTable,
                        .pAngle = NULL,
                        .pRealExpectFormula = NULL,
                        .pAngleFormula = NULL,
                        .pUserDefPinhoFunc = NULL,
                        .pUserDefAngleFunc = NULL
                    }
                },
                .Sensor = {
                    .CellSize = Emirror3in3WarpConfig.Cam.Sensor.CellSize,
                    .StartX = Emirror3in3WarpConfig.Cam.Sensor.StartX,
                    .StartY = Emirror3in3WarpConfig.Cam.Sensor.StartY,
                    .Width = Emirror3in3WarpConfig.Cam.Sensor.Width,
                    .Height = Emirror3in3WarpConfig.Cam.Sensor.Height
                },

                .OpticalCenter = {Emirror3in3WarpConfig.Cam.OpticalCenter.X,
                                  Emirror3in3WarpConfig.Cam.OpticalCenter.Y},

                .Pos = {Emirror3in3WarpConfig.Cam.Pos.X,
                        Emirror3in3WarpConfig.Cam.Pos.Y,
                        Emirror3in3WarpConfig.Cam.Pos.Z},
                .Rotation = Emirror3in3WarpConfig.Cam.Rotation
            };

            /* If user not fill-in CamInfo.Pos.Y in script, change Cam.Pos.Y to 1
             * => To let calibration module know that it's CAM_FRONT case
             */
            if (CamInfo.Pos.Y == 0.0) {
                CamInfo.Pos.Y = 1.0;
            }

            if (CamInfo.Lens.LensDistoUnit != AMBA_CAL_LD_PIXEL) {
                for (UINT32 K = 0; K < LensLdcTable.Length; K ++) {
                    LensData.pRealTable[K] /= CamInfo.Sensor.CellSize;
                    LensData.pExceptTable[K] /= CamInfo.Sensor.CellSize;
                }
                CamInfo.Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
            }

            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_QUICK_CALIB, &QuickCalibWorkBuf, &QuickCalibWorkBufSize);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_QUICK_CALIB failed!!", 0U, 0U);
            }
            AmbaMisra_TypeCast32(&pQuickCalibWorkBuf, &QuickCalibWorkBuf);

            AmbaSvcWrap_MisraMemset(&FcwsCfg, 0, sizeof(AMBA_CAL_EXT_PITCH_CFG_s));
            AmbaSvcWrap_MisraMemcpy(&FcwsCfg.CamInfo, &CamInfo, sizeof(AMBA_CAL_CAM_V2_s));
            FcwsCfg.FocalLength = FocalLength;
            FcwsCfg.RoiInfo.StartX = Emirror3in3WarpConfig.ROI.StartX;
            FcwsCfg.RoiInfo.StartY = Emirror3in3WarpConfig.ROI.StartY;
            FcwsCfg.RoiInfo.Width = Emirror3in3WarpConfig.ROI.Width;
            FcwsCfg.RoiInfo.Height = Emirror3in3WarpConfig.ROI.Height;
            FcwsCfg.ImageInfo.StartX = Emirror3in3WarpConfig.View.VoutArea.StartX;
            FcwsCfg.ImageInfo.StartY = Emirror3in3WarpConfig.View.VoutArea.StartY;
            FcwsCfg.ImageInfo.Width = Emirror3in3WarpConfig.View.VoutArea.Width;
            FcwsCfg.ImageInfo.Height = Emirror3in3WarpConfig.View.VoutArea.Height;

            FcwsCfg.WorldHorizonY = AMBA_CAL_INFINITE_DIST;
            FcwsCfg.ImageHorizonY = (DOUBLE)VoutHorizonY;

            if (warning_zone_x_offset < 0) {//Warning zone move left
                WarningZoneXinPixel = SvcAdasNotify_GetWarningZoneX_InPixel((ZoneX_Offset*(-1)), CamInfo.Sensor.Width);
                Rval = AmbaWrap_atan((((DOUBLE)WarningZoneXinPixel*CamInfo.Sensor.CellSize)/FocalLength), &Radian);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[1] AmbaWrap_atan failed!!", 0U, 0U);
                }
                Degree = Radian * (180.0 / PI);
                FcwsCfg.TuningHorRotDeg = Degree * (-1.0); //Left -> Degree is negative
                AmbaPrint_PrintUInt5("(warning_zone_x_offset < 0) Degree = %d", (UINT32)Degree, 0U, 0U, 0U, 0U);
            } else if (warning_zone_x_offset > 0) {//Warning zone move right
                WarningZoneXinPixel = SvcAdasNotify_GetWarningZoneX_InPixel(ZoneX_Offset, CamInfo.Sensor.Width);
                Rval = AmbaWrap_atan((((DOUBLE)WarningZoneXinPixel*CamInfo.Sensor.CellSize)/FocalLength), &Radian);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[2]AmbaWrap_atan failed!!", 0U, 0U);
                }
                Degree = Radian * (180.0 / PI);
                FcwsCfg.TuningHorRotDeg = Degree; //Right -> Degree is positive
                AmbaPrint_PrintUInt5("(warning_zone_x_offset > 0) Degree = %d", (UINT32)Degree, 0U, 0U, 0U, 0U);
            } else { //warning_zone_x_offset == 0
                //Do nothing
            }

            AmbaSvcWrap_MisraMemset(&Output, 0, sizeof(Output));
            Rval = AmbaCal_ExtSimpCorrPitch(&FcwsCfg, pQuickCalibWorkBuf, &Output);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCal_ExtSimpCorrPitch failed!!", 0U, 0U);
            }


            StrLen = AmbaUtility_DoubleToStr(DoubleBuf, 32U, FcwsCfg.WorldHorizonY, 0U);
            SvcLog_OK(SVC_LOG_CMDAPP, "[1] StrLen = %d", StrLen, 0U);
            StrLen = AmbaUtility_DoubleToStr(DoubleBuf2, 32U, FcwsCfg.ImageHorizonY, 0U);
            SvcLog_OK(SVC_LOG_CMDAPP, "[2] StrLen = %d", StrLen, 0U);
            AmbaPrint_PrintStr5(" ====== UserWorldY %s & ImageHorizonY %s======",DoubleBuf, DoubleBuf2, NULL, NULL, NULL);

            for (UINT32 K = 0U; K < 4U; K++) {
                char   RawPosX[32U], RawPosY[32U];
                char   WorldPosX[32U], WorldPosY[32U], WorldPosZ[32U];

                StrLen = AmbaUtility_DoubleToStr(RawPosX, 32U, Output.CalibPoints[K].RawPos.X, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[3] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(RawPosY, 32U, Output.CalibPoints[K].RawPos.Y, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[4] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosX, 32U, Output.CalibPoints[K].WorldPos.X, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[5] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosY, 32U, Output.CalibPoints[K].WorldPos.Y, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[6] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosZ, 32U, Output.CalibPoints[K].WorldPos.Z, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[7] StrLen = %d", StrLen, 0U);
                AmbaPrint_PrintStr5("RawPos(%s, %s) WorldPos(%s, %s, %s)",RawPosX, RawPosY, WorldPosX, WorldPosY, WorldPosZ);


                CalibPointData.RawPositionX[K] = Output.CalibPoints[K].RawPos.X;
                CalibPointData.RawPositionY[K] = Output.CalibPoints[K].RawPos.Y;
                CalibPointData.WorldPositionX[K] = Output.CalibPoints[K].WorldPos.X;
                CalibPointData.WorldPositionY[K] = Output.CalibPoints[K].WorldPos.Y;
                CalibPointData.WorldPositionZ[K] = Output.CalibPoints[K].WorldPos.Z;
            }

            AmbaSvcWrap_MisraMemset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s));
            RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
            Emirror3in3WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
            Emirror3in3WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
            Emirror3in3WarpConfig.Calibinfo.pMultiPoint = NULL;
            AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
            AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
            Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
            AmbaCT_EmGetOptimize(0U, &Misra_0);
            AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);
            Rval = RefCalib_ConvImgToWorldPlane(Emirror3in3WarpConfig, &CalibCoordCfg);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_ConvImgToWorldPlane failed!!", 0U, 0U);
            }
            RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);

            Rval = SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_FRONT, &CalibCoordCfg);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet failed!!", 0U, 0U);
            }
        }
    }

}

static void QuickCalib_V1(UINT32 Horizontal, DOUBLE FocalLength, INT32 warning_zone_x_offset)
{
    static UINT32 VoutHorizonY = 0U;
    INT32 ZoneX_Offset = warning_zone_x_offset;
    DOUBLE Radian, Degree;
    UINT8 Misra_0;
    UINT32 Rval = SVC_OK, WarningZoneXinPixel = 0U;
    static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
    static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_SV_CFG_V1_s Emirror3in3WarpConfig GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCoordCfg GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalOutput GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
    static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;

    /* Get final CalibHorizontal line position */
    VoutHorizonY = SvcAdasNotify_SetCalibHorizontal(0U, Horizontal);
    SvcLog_OK(__func__, "---- UserVoutHorizonY = %d ----", VoutHorizonY, 0U);

    {
        AmbaSvcWrap_MisraMemset(&LensData, 0, sizeof(LensData));
        AmbaSvcWrap_MisraMemset(&CameraData, 0, sizeof(CameraData));
        AmbaSvcWrap_MisraMemset(&AspPointMap, 0, sizeof(AspPointMap));
        AmbaSvcWrap_MisraMemset(&CalibPointData, 0, sizeof(CalibPointData));
        AmbaSvcWrap_MisraMemset(&AssistancePointData, 0, sizeof(AssistancePointData));
        AmbaSvcWrap_MisraMemset(&Emirror3in3WarpConfig, 0, sizeof(Emirror3in3WarpConfig));
        AmbaSvcWrap_MisraMemset(&Aspheric, 0, sizeof(Aspheric));
        AmbaSvcWrap_MisraMemset(&CurvedSurface, 0, sizeof(CurvedSurface));
        AmbaSvcWrap_MisraMemset(&CalibCoordCfg, 0, sizeof(CalibCoordCfg));
        AmbaSvcWrap_MisraMemset(&CalOutput, 0, sizeof(CalOutput));
        AmbaSvcWrap_MisraMemset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
        AmbaSvcWrap_MisraMemset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
        AmbaSvcWrap_MisraMemset(&LensSpecFormula, 0, sizeof(LensSpecFormula));

        AmbaCT_EmGetLens(0U, &LensData);
        Rval = RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
        if(Rval != 0U) {
            SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_EmFeedLensSpec failed!!", 0U, 0U);
        }
        AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfig.Cam.Sensor);
        AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfig.Cam.OpticalCenter);
        AmbaCT_EmGetCamera(0U, &CameraData);
        Emirror3in3WarpConfig.Cam.Pos.X = CameraData.PositionX;
        Emirror3in3WarpConfig.Cam.Pos.Y = CameraData.PositionY;
        Emirror3in3WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
        Emirror3in3WarpConfig.Cam.Rotation = CameraData.RotateType;
        AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfig.VinSensorGeo);
        AmbaCT_EmGetSingleView(&Emirror3in3WarpConfig.View.Type);
        if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_LDC) {
            AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
            Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
            AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
            Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
            }
            AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
            Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
        } else {
            AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfig.View.VoutArea);
        Emirror3in3WarpConfig.View.PlugIn.PlugInMode = 0U; // TODO:
        AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
        AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
        Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
        AmbaCT_EmGetOptimize(0U, &Misra_0);
        AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);

        /* Quick calibaration start from here */
        {
            char   DoubleBuf[32U], DoubleBuf2[32U];
            AMBA_CAL_EXT_PITCH_DATA_s Output;
            static void *pQuickCalibWorkBuf;
            AMBA_CAL_EXT_PITCH_CFG_s FcwsCfg;
            static UINT32 QuickCalibWorkBufSize;
            static ULONG  QuickCalibWorkBuf;
            UINT32 StrLen;

            AMBA_CAL_LENS_DST_REAL_EXPECT_s LensLdcTable = {
                .Length = LensData.TableLen,
                .pRealTbl = LensData.pRealTable,
                .pExpectTbl = LensData.pExceptTable
            };
            AMBA_CAL_CAM_V2_s CamInfo = {
                .Lens = {
                    .LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL,
                    .LensDistoUnit = AMBA_CAL_LD_MM,
                    .LensDistoSpec = {
                        .pRealExpect = &LensLdcTable,
                        .pAngle = NULL,
                        .pRealExpectFormula = NULL,
                        .pAngleFormula = NULL,
                        .pUserDefPinhoFunc = NULL,
                        .pUserDefAngleFunc = NULL
                    }
                },
                .Sensor = {
                    .CellSize = Emirror3in3WarpConfig.Cam.Sensor.CellSize,
                    .StartX = Emirror3in3WarpConfig.Cam.Sensor.StartX,
                    .StartY = Emirror3in3WarpConfig.Cam.Sensor.StartY,
                    .Width = Emirror3in3WarpConfig.Cam.Sensor.Width,
                    .Height = Emirror3in3WarpConfig.Cam.Sensor.Height
                },

                .OpticalCenter = {Emirror3in3WarpConfig.Cam.OpticalCenter.X,
                                  Emirror3in3WarpConfig.Cam.OpticalCenter.Y},

                .Pos = {Emirror3in3WarpConfig.Cam.Pos.X,
                        Emirror3in3WarpConfig.Cam.Pos.Y,
                        Emirror3in3WarpConfig.Cam.Pos.Z},
                .Rotation = Emirror3in3WarpConfig.Cam.Rotation
            };

            /* If user not fill-in CamInfo.Pos.Y in script, change Cam.Pos.Y to 1
             * => To let calibration module know that it's CAM_FRONT case
             */
            if (CamInfo.Pos.Y == 0.0) {
                CamInfo.Pos.Y = 1.0;
            }

            if (CamInfo.Lens.LensDistoUnit != AMBA_CAL_LD_PIXEL) {
                for (UINT32 K = 0; K < LensLdcTable.Length; K ++) {
                    LensData.pRealTable[K] /= CamInfo.Sensor.CellSize;
                    LensData.pExceptTable[K] /= CamInfo.Sensor.CellSize;
                }
                CamInfo.Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
            }

            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_QUICK_CALIB, &QuickCalibWorkBuf, &QuickCalibWorkBufSize);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_QUICK_CALIB failed!!", 0U, 0U);
            }
            AmbaMisra_TypeCast32(&pQuickCalibWorkBuf, &QuickCalibWorkBuf);

            AmbaSvcWrap_MisraMemset(&FcwsCfg, 0, sizeof(AMBA_CAL_EXT_PITCH_CFG_s));
            AmbaSvcWrap_MisraMemcpy(&FcwsCfg.CamInfo, &CamInfo, sizeof(AMBA_CAL_CAM_V2_s));
            FcwsCfg.FocalLength = FocalLength;
            FcwsCfg.RoiInfo.StartX = Emirror3in3WarpConfig.ROI.StartX;
            FcwsCfg.RoiInfo.StartY = Emirror3in3WarpConfig.ROI.StartY;
            FcwsCfg.RoiInfo.Width = Emirror3in3WarpConfig.ROI.Width;
            FcwsCfg.RoiInfo.Height = Emirror3in3WarpConfig.ROI.Height;
            FcwsCfg.ImageInfo.StartX = Emirror3in3WarpConfig.View.VoutArea.StartX;
            FcwsCfg.ImageInfo.StartY = Emirror3in3WarpConfig.View.VoutArea.StartY;
            FcwsCfg.ImageInfo.Width = Emirror3in3WarpConfig.View.VoutArea.Width;
            FcwsCfg.ImageInfo.Height = Emirror3in3WarpConfig.View.VoutArea.Height;

            FcwsCfg.WorldHorizonY = AMBA_CAL_INFINITE_DIST;
            FcwsCfg.ImageHorizonY = (DOUBLE)VoutHorizonY;

            if (warning_zone_x_offset < 0) {//Warning zone move left
                WarningZoneXinPixel = SvcAdasNotify_GetWarningZoneX_InPixel((ZoneX_Offset*(-1)), CamInfo.Sensor.Width);
                Rval = AmbaWrap_atan((((DOUBLE)WarningZoneXinPixel*CamInfo.Sensor.CellSize)/FocalLength), &Radian);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[1] AmbaWrap_atan failed!!", 0U, 0U);
                }
                Degree = Radian * (180.0 / PI);
                FcwsCfg.TuningHorRotDeg = Degree * (-1.0); //Left -> Degree is negative
                AmbaPrint_PrintUInt5("(warning_zone_x_offset < 0) Degree = %d", (UINT32)Degree, 0U, 0U, 0U, 0U);
            } else if (warning_zone_x_offset > 0) {//Warning zone move right
                WarningZoneXinPixel = SvcAdasNotify_GetWarningZoneX_InPixel(ZoneX_Offset, CamInfo.Sensor.Width);
                Rval = AmbaWrap_atan((((DOUBLE)WarningZoneXinPixel*CamInfo.Sensor.CellSize)/FocalLength), &Radian);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[2] AmbaWrap_atan failed!!", 0U, 0U);
                }
                Degree = Radian * (180.0 / PI);
                FcwsCfg.TuningHorRotDeg = Degree; //Right -> Degree is positive
                AmbaPrint_PrintUInt5("(warning_zone_x_offset > 0) Degree = %d", (UINT32)Degree, 0U, 0U, 0U, 0U);
            } else { //warning_zone_x_offset == 0
                //Do nothing
            }

            AmbaSvcWrap_MisraMemset(&Output, 0, sizeof(Output));
            Rval = AmbaCal_ExtSimpCorrPitch(&FcwsCfg, pQuickCalibWorkBuf, &Output);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCal_ExtSimpCorrPitch failed!!", 0U, 0U);
            }


            StrLen = AmbaUtility_DoubleToStr(DoubleBuf, 32U, FcwsCfg.WorldHorizonY, 0U);
            SvcLog_OK(SVC_LOG_CMDAPP, "[1] StrLen = %d", StrLen, 0U);
            StrLen = AmbaUtility_DoubleToStr(DoubleBuf2, 32U, FcwsCfg.ImageHorizonY, 0U);
            SvcLog_OK(SVC_LOG_CMDAPP, "[2] StrLen = %d", StrLen, 0U);

            AmbaPrint_PrintStr5(" ====== UserWorldY %s & ImageHorizonY %s======",DoubleBuf, DoubleBuf2, NULL, NULL, NULL);

            for (UINT32 K = 0U; K < 4U; K++) {
                char   RawPosX[32U], RawPosY[32U];
                char   WorldPosX[32U], WorldPosY[32U], WorldPosZ[32U];

                StrLen = AmbaUtility_DoubleToStr(RawPosX, 32U, Output.CalibPoints[K].RawPos.X, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[3] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(RawPosY, 32U, Output.CalibPoints[K].RawPos.Y, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[4] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosX, 32U, Output.CalibPoints[K].WorldPos.X, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[5] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosY, 32U, Output.CalibPoints[K].WorldPos.Y, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[6] StrLen = %d", StrLen, 0U);
                StrLen = AmbaUtility_DoubleToStr(WorldPosZ, 32U, Output.CalibPoints[K].WorldPos.Z, 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "[7] StrLen = %d", StrLen, 0U);
                AmbaPrint_PrintStr5("RawPos(%s, %s) WorldPos(%s, %s, %s)",RawPosX, RawPosY, WorldPosX, WorldPosY, WorldPosZ);


                CalibPointData.RawPositionX[K] = Output.CalibPoints[K].RawPos.X;
                CalibPointData.RawPositionY[K] = Output.CalibPoints[K].RawPos.Y;
                CalibPointData.WorldPositionX[K] = Output.CalibPoints[K].WorldPos.X;
                CalibPointData.WorldPositionY[K] = Output.CalibPoints[K].WorldPos.Y;
                CalibPointData.WorldPositionZ[K] = Output.CalibPoints[K].WorldPos.Z;
            }

            AmbaSvcWrap_MisraMemset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s));
            RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
            Emirror3in3WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
            Emirror3in3WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
            Emirror3in3WarpConfig.Calibinfo.pMultiPoint = NULL;
            AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
            AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
            Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
            AmbaCT_EmGetOptimize(0U, &Misra_0);
            AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);
            Rval = RefCalib_ConvImgToWorldPlaneV1(Emirror3in3WarpConfig, &CalibCoordCfg, &CalOutput, FocalLength);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_ConvImgToWorldPlaneV1 failed!!", 0U, 0U);
            }
            RefCalib_ShowCalibCoordOutputV1(&CalibCoordCfg);

            if (SVC_OK != SvcCalib_AdasCfgSetV1(SVC_CALIB_ADAS_TYPE_FRONT, &CalibCoordCfg, &CalOutput)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_FRONT failed", 0U, 0U);
            }
        }
    }

}
#endif


#if 0 //def CONFIG_ICAM_PROJECT_SHMOO
static UINT32 ShmooSetInfo(UINT32 ArgCount, char * const *pArgVector)
{

    UINT32 RetVal = SVC_OK, RecMode, CvMode, FileWrite, i, u32v;
    UINT8 tempV[19] = {0};
    UINT32 TempMisc[5]={0};

    if(ArgCount == 29U){

        RetVal |= SvcWrap_strtoul(pArgVector[2U], &RecMode);
        RetVal |= SvcWrap_strtoul(pArgVector[3U], &CvMode);
        RetVal |= SvcWrap_strtoul(pArgVector[4U], &FileWrite);

        for(i=0U; i < 19U; i++){
            RetVal |= SvcWrap_strtoul(pArgVector[i+5U], &u32v);
            if(RetVal == SVC_OK){
                tempV[i] = (UINT8) u32v & 0xFFU;
            } else {
                SvcLog_NG(__func__, "Invalid Shmoo Item Value", 0U, 0U);
                break;
            }
        }

        if(RetVal == SVC_OK){
            for(i=0U; i<5U; i++){
                RetVal |= SvcWrap_strtoul(pArgVector[i+24U], &TempMisc[i]);
                if(RetVal != SVC_OK){
                    SvcLog_NG(__func__, "Invalid Shmoo MisC Value", 0U, 0U);
                    break;
                }
            }

            if(RetVal == SVC_OK){
                TempMisc[3] = SVC_ADVANCED_SHMOO_TASK_PRI;
                AdvancedShmoo_SetInfo(RecMode, CvMode, FileWrite, &tempV[0], &TempMisc[0]);
            }
        }

    } else {
        RetVal = SVC_NG;
        SvcLog_NG(__func__, "Number of parameter is [%u]. Please give [25] parameters. ", ArgCount-2U, 0U);
    }

    return RetVal;


}
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
#if defined(CONFIG_ICAM_REBEL_USAGE)
#define HORIZONTAL_Y  (250U)
#else
#define HORIZONTAL_Y  (320U)
#endif
#endif
static void CmdAppEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32           Rval = SVC_OK, IsReboot = 0U, PrefBufSize = 0U;
    ULONG            PrefBufAddr = 0U;
    SVC_USER_PREF_s  *pSvcUserPref = NULL;
#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
    static UINT32 AdasEnable = 0U; //Disable by default
    static UINT32 OD_Info = 1U;    //Enable by default
    static UINT32 Horizontal;
    static INT32  Horizontal_MoveDir = 0; //+: Up, -: Down
    static UINT32 FocalLengthUINT32;//nm
    static DOUBLE FocalLength = (4.47);//Unit: mm, CAR53 Lens, imx424
    static INT32 warning_zone_x_offset = 0; //+:right; -:left
#endif

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("vintree", pArgVector[1U])) {
            Rval = SvcVinTree_DebugEnable(1U);
        } else if (0 == SvcWrap_strcmp("free_space_draw", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                SvcOsd_SetClut(41U, 0x800080FFU);
            } else {
                SvcOsd_SetClut(41U, 0x000080FFU);
            }
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
        } else if (0 == SvcWrap_strcmp("Rebel_GUI", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                if (0 == SvcWrap_strcmp("id", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawID(1U);
                } else if (0 == SvcWrap_strcmp("d_xy", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawDxy(1U);
                } else if (0 == SvcWrap_strcmp("v_xy", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawVxy(1U);
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[Rebel] Wrong cmd (on)", 0U, 0U);
                }
            } else {
                if (0 == SvcWrap_strcmp("id", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawID(0U);
                } else if (0 == SvcWrap_strcmp("d_xy", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawDxy(0U);
                } else if (0 == SvcWrap_strcmp("v_xy", pArgVector[3U])) {
                    SvcODDrawTask_Rebel_EnableDrawVxy(0U);
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "[Rebel] Wrong cmd (off)", 0U, 0U);
                }
            }
        } else if (0 == SvcWrap_strcmp("tune_line", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                SvcRebelGUI_TuneLineEnable(1U);
            } else {
                SvcRebelGUI_TuneLineEnable(0U);
            }
#endif
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        } else if (0 == SvcWrap_strcmp("set_focallength_nm", pArgVector[1U])) {
            Rval = SvcWrap_strtoul(pArgVector[2U], &FocalLengthUINT32);
            if(Rval != 0U) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
            }
            FocalLength = ((DOUBLE) FocalLengthUINT32)/1000000.0; //nm to mm

            if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
            }
            pSvcUserPref->CalibFocalLength = FocalLength;
            SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
            if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
            }

            //Auto Calibration FocalLength Setup
            SvcAdasAutoCal_SetupFocalLength(FocalLength);



        } else if (0 == SvcWrap_strcmp("adas_calib_line", pArgVector[1U])) {

            UINT32  CurrentHorizonY = 0U;
            // Check Horizontal value
            Horizontal = SvcAdasNotify_GetCalibHorizontal();
            if (0U == Horizontal) {
                Horizontal = HORIZONTAL_Y;
            }
            if (0 == SvcWrap_strcmp("down", pArgVector[2U])) {
                Horizontal_MoveDir--;//+: Up, -: Down
                Horizontal++;
                CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
            } else if (0 == SvcWrap_strcmp("up", pArgVector[2U])) {
                AmbaMisra_TouchUnused(&Horizontal);
                if (Horizontal > 0U) {
                    Horizontal_MoveDir++;//+: Up, -: Down
                    Horizontal--;
                    CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                    SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
                }
            } else if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
            } else if (0 == SvcWrap_strcmp("auto_tune", pArgVector[2U])) {
                AMBA_CT_INITIAL_CONFIG_s InitCfg;
                static ULONG Emir3in3TunerWorkBuf;
                static UINT32 EmirWorkBufSize;
                static void *pEmir3in3TunerWorkBuf;
                const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                char ADASscrript[30];

                if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                }
                Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request failed!!", 0U, 0U);
                }
                AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                }

                if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                }
                ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                Rval = AmbaCT_Load(ADASscrript);
                if(Rval != 0U) {
                    AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                } else {
                    QuickCalib(Horizontal, pSvcUserPref->CalibFocalLength, warning_zone_x_offset);
                }

                {
                    DOUBLE FocalLengthPrint = pSvcUserPref->CalibFocalLength;
                    INT64  DoubleToINT64;
                    UINT32 CalibFocalLengthIn_nm;
                    FocalLengthPrint *= 1000000.0;/*mm to nm */
                    DoubleToINT64 = (INT64)(FocalLengthPrint);
                    AmbaMisra_TypeCast32(&CalibFocalLengthIn_nm, &DoubleToINT64);
                    FocalLength = pSvcUserPref->CalibFocalLength;
                    pSvcUserPref->CalibFocalLength    = FocalLength;
                    pSvcUserPref->SkylineHeight       = Horizontal;
                    pSvcUserPref->WarningZoon_AutoRun = 1U;

                    SvcLog_OK(__func__, "CalibFocalLength %d (nm), WarningZoon_AutoRun %d",
                                            CalibFocalLengthIn_nm,
                                            pSvcUserPref->WarningZoon_AutoRun);

                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    IsReboot = 1U;
                }

                /* Record Horizontal_MoveDir value to SkylineHelght.txt */
                {
                    UINT32 RetVal;
                    char Value[128U] = "No need to move Skyline (Use default value)!!";
                    const char SkylineFileName[30] = ":\\SkylineHelght.txt";
                    char SkylineScrript[30];

                    AMBA_FS_FILE  *pOutputFile;
                    UINT32 StrEndIdx;

                    UINT32 NumSuccess = 0U;

                    if (SVC_OK != AmbaWrap_memset(&SkylineScrript, 0, sizeof(SkylineScrript))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                    }
                    SkylineScrript[0] = pSvcUserPref->MainStgDrive[0];
                    CmdAPP_strcat(SkylineScrript, SkylineFileName);

                    RetVal = AmbaFS_FileOpen(SkylineScrript, "wb", &pOutputFile);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Open failed", 0U, 0U);
                    }

                    //+: Up, -: Down
                    if (Horizontal_MoveDir > 0) {
                        UINT32 FileArg = (UINT32)Horizontal_MoveDir;

                        AmbaSvcWrap_MisraMemset(Value, 0, sizeof(char) * 128U);
                        StrEndIdx = AmbaUtility_StringPrintUInt32(Value, 128U, "UP %d  !!", 1U, &FileArg);
                        SvcLog_OK(SVC_LOG_CMDAPP, "[Up] StrEndIdx = %d", StrEndIdx, 0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "SkylineHelght UP %d  !!", FileArg, 0U);
                    } else if (Horizontal_MoveDir < 0) {
                        UINT32 FileArg;

                        AmbaSvcWrap_MisraMemset(Value, 0, sizeof(char) * 128U);
                        Horizontal_MoveDir = Horizontal_MoveDir * (-1);
                        FileArg = (UINT32)Horizontal_MoveDir;
                        StrEndIdx = AmbaUtility_StringPrintUInt32(Value, 128U, "Down %d  !!", 1U, &FileArg);
                        SvcLog_OK(SVC_LOG_CMDAPP, "[Down] StrEndIdx = %d", StrEndIdx, 0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "SkylineHelght Down %d  !!", FileArg, 0U);
                    } else {
                        //Do nothing
                    }

                    RetVal = AmbaFS_FileWrite(Value, 1U, 128U, pOutputFile, &NumSuccess);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Write failed", 0U, 0U);
                    }

                    RetVal = AmbaFS_FileClose(pOutputFile);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Close failed", 0U, 0U);
                    }

                }

                Rval = AmbaKAL_TaskSleep(1000);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("auto_tune_V1", pArgVector[2U])) {
                AMBA_CT_INITIAL_CONFIG_s InitCfg;
                static ULONG Emir3in3TunerWorkBuf;
                static UINT32 EmirWorkBufSize;
                static void *pEmir3in3TunerWorkBuf;
                const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                char ADASscrript[30];

                if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                }
                Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
                }
                AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                }
                if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                }
                ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                Rval = AmbaCT_Load(ADASscrript);
                if(Rval != 0U) {
                    AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                } else {
                    QuickCalib_V1(Horizontal, pSvcUserPref->CalibFocalLength, warning_zone_x_offset);
                }

                {
                    DOUBLE FocalLengthPrint = pSvcUserPref->CalibFocalLength;
                    INT64  DoubleToINT64;
                    UINT32 CalibFocalLengthIn_nm;
                    FocalLengthPrint *= 1000000.0;/*mm to nm */
                    DoubleToINT64 = (INT64)(FocalLengthPrint);
                    AmbaMisra_TypeCast32(&CalibFocalLengthIn_nm, &DoubleToINT64);
                    FocalLength = pSvcUserPref->CalibFocalLength;
                    pSvcUserPref->CalibFocalLength    = FocalLength;
                    pSvcUserPref->SkylineHeight       = Horizontal;
                    pSvcUserPref->WarningZoon_AutoRun = 1U;

                    SvcLog_OK(__func__, "CalibFocalLength %d (nm), WarningZoon_AutoRun %d",
                                            CalibFocalLengthIn_nm,
                                            pSvcUserPref->WarningZoon_AutoRun);

                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    IsReboot = 1U;
                }

                /* Record Horizontal_MoveDir value to SkylineHelght.txt */
                {
                    UINT32 RetVal;
                    char Value[128U] = "No need to move Skyline (Use default value)!!";
                    const char SkylineFileName[30] = ":\\SkylineHelght.txt";
                    char SkylineScrript[30];
                    AMBA_FS_FILE  *pOutputFile;
                    UINT32 StrEndIdx;

                    UINT32 NumSuccess = 0U;

                    if (SVC_OK != AmbaWrap_memset(&SkylineScrript, 0, sizeof(SkylineScrript))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                    }
                    SkylineScrript[0] = pSvcUserPref->MainStgDrive[0];
                    CmdAPP_strcat(SkylineScrript, SkylineFileName);
                    RetVal = AmbaFS_FileOpen(SkylineScrript, "wb", &pOutputFile);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Open failed", 0U, 0U);
                    }

                    //+: Up, -: Down
                    if (Horizontal_MoveDir > 0) {
                        UINT32 FileArg = (UINT32)Horizontal_MoveDir;

                        AmbaSvcWrap_MisraMemset(Value, 0, sizeof(char) * 128U);
                        StrEndIdx = AmbaUtility_StringPrintUInt32(Value, 128U, "UP %d  !!", 1U, &FileArg);
                        SvcLog_OK(SVC_LOG_CMDAPP, "[Up] StrEndIdx = %d", StrEndIdx, 0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "SkylineHelght UP %d  !!", FileArg, 0U);
                    } else if (Horizontal_MoveDir < 0) {
                        UINT32 FileArg;

                        AmbaSvcWrap_MisraMemset(Value, 0, sizeof(char) * 128U);
                        Horizontal_MoveDir = Horizontal_MoveDir * (-1);
                        FileArg = (UINT32)Horizontal_MoveDir;
                        StrEndIdx = AmbaUtility_StringPrintUInt32(Value, 128U, "Down %d  !!", 1U, &FileArg);
                        SvcLog_OK(SVC_LOG_CMDAPP, "[Down] StrEndIdx = %d", StrEndIdx, 0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "SkylineHelght Down %d  !!", FileArg, 0U);
                    } else {
                        //Do nothing
                    }

                    RetVal = AmbaFS_FileWrite(Value, 1U, 128U, pOutputFile, &NumSuccess);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Write failed", 0U, 0U);
                    }

                    RetVal = AmbaFS_FileClose(pOutputFile);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVFS_Close failed", 0U, 0U);
                    }

                }

                Rval = AmbaKAL_TaskSleep(1000);
                if(Rval != 0U) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed!!", 0U, 0U);
                }
            } else {
                //
            }
#endif
#ifndef CONFIG_SOC_CV28
        } else if (0 == SvcWrap_strcmp("set_flicker", pArgVector[1U])) {
            UINT32 view_id = 0;
            static AE_EV_LUT_s pEvLut_tmp;

            Rval = AmbaImgProc_AEGetAEEvLut(view_id, &pEvLut_tmp);AmbaMisra_TouchUnused(&Rval);
            if (0 == SvcWrap_strcmp("50hz", pArgVector[2U])) {
                /* Set 60 hz */
                pEvLut_tmp.FlickerMode = ANTI_FLICKER_NO_50HZ;
                SvcLog_OK(SVC_LOG_CMDAPP, "-- Set flicker = 50 Hz", 0U, 0U);
            } else if (0 == SvcWrap_strcmp("60hz", pArgVector[2U])) {
                /* Set 60 hz */
                pEvLut_tmp.FlickerMode = ANTI_FLICKER_NO_60HZ;
                SvcLog_OK(SVC_LOG_CMDAPP, "-- Set flicker = 60 Hz", 0U, 0U);
            } else {
                /* Wrong flicker setting, set to 50 hz */
                pEvLut_tmp.FlickerMode = ANTI_FLICKER_NO_50HZ;
                SvcLog_NG(SVC_LOG_CMDAPP, "!! Wrong flicker setting, set flicker = 50 Hz", 0U, 0U);
            }
            Rval = AmbaImgProc_AESetAEEvLut( view_id, &pEvLut_tmp);AmbaMisra_TouchUnused(&Rval);
#endif
        } else if (0 == SvcWrap_strcmp("rec", pArgVector[1U])) {
            Rval = CmdRecEntry(ArgCount, pArgVector, PrintFunc);

        #if defined(CONFIG_SVC_ENABLE_WDT)
        } else if (0 == SvcWrap_strcmp("wdtfeed", pArgVector[1U])) {
            extern UINT32 SvcWdt_FeedTest;
            UINT32 Ena = 0U;

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &Ena)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul() failed!!", 0U, 0U);
            } else {
                SvcLog_OK(SVC_LOG_CMDAPP, "## set SvcWdt_FeedTest = %u", Ena, 0U);
                SvcWdt_FeedTest = Ena;
            }

        } else if (0 == SvcWrap_strcmp("wdtfeedprint", pArgVector[1U])) {
            extern UINT32 SvcWdt_FeedPrint;
            SvcWdt_FeedPrint = ~SvcWdt_FeedPrint;

        } else if (0 == SvcWrap_strcmp("wdtfeedtime", pArgVector[1U])) {
            extern UINT32 SvcWdt_FeedInterval;
            UINT32 interval = 1000U;

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &interval)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul() failed!!", 0U, 0U);
            } else {
                SvcLog_OK(SVC_LOG_CMDAPP, "## set SvcWdt_FeedInterval = %u", interval, 0U);
                SvcWdt_FeedInterval = interval;
            }

        } else if (0 == SvcWrap_strcmp("wdtinfo", pArgVector[1U])) {
            extern UINT32 SvcWdt_Info(void);
            Rval = SvcWdt_Info();
            AmbaMisra_TouchUnused(&Rval);

        } else if (0 == SvcWrap_strcmp("wdtget", pArgVector[1U])) {
            extern UINT32 SvcWdt_GetFlag(UINT32 ID, UINT32* Ena);
            UINT32 ID, Ena = 0U;
            for (ID = 0; ID < 4U; ID++) {
                Rval = SvcWdt_GetFlag(ID, &Ena);
                SvcLog_OK(SVC_LOG_CMDAPP, "SvcWdt_GetFlag: ID[%u], Ena %u", ID, Ena);
                AmbaMisra_TouchUnused(&Rval);
            }

        } else if (0 == SvcWrap_strcmp("wdtset", pArgVector[1U])) {
            extern UINT32 SvcWdt_SetFlag(UINT32 ID, UINT32  Ena);
            UINT32 ID = 0U, Ena = 0U;
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &ID)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul() failed!!", 0U, 0U);
            }
            if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Ena)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul() failed!!", 0U, 0U);
            }

            Rval = SvcWdt_SetFlag(ID, Ena);
            SvcLog_OK(SVC_LOG_CMDAPP, "SvcWdt_SetFlag: ID %u, Ena %u", ID, Ena);
            AmbaMisra_TouchUnused(&Rval);

        // } else if (0 == SvcWrap_strcmp("wdtreboot", pArgVector[1U])) {
        //     extern void SvcWdt_TimeoutCb(UINT32 EntryArg);
        //     SvcLog_OK(SVC_LOG_CMDAPP, "##### call SvcWdt_TimeoutCb", 0U, 0U);
        //     SvcWdt_TimeoutCb(0U);
        #endif

        } else if (0 == SvcWrap_strcmp("pbk", pArgVector[1U])) {
            Rval = CmdPbkEntry(ArgCount, pArgVector, PrintFunc);
        } else if (0 == SvcWrap_strcmp("format_id", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32  FormatId;

                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &FormatId)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul() failed!!", 0U, 0U);
                } else {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->FormatId = FormatId;

                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                        /* If it is liveview mode, do not reboot */
                        if (pSvcUserPref->OperationMode == 1U) {
                            if (SVC_OK != SvcResCfgTask_Switch(FormatId)) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "SvcResCfgTask_Switch failed!!", 0U, 0U);
                            }
                        } else {
                            IsReboot = 1U;
                        }
                    }
                }
            } else {
                SvcResCfgTask_Dump();
                SvcLog_DBG(SVC_LOG_CMDAPP, "Reboot will occurred if there's no group name or switch between different group", 0U, 0U);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("print_test", pArgVector[1U])) {
                // AdvancedShmoo_DumpResult();
            #if 0
                char printBuf[512U];
                char space[20U];
                UINT32 RetVal, i;
                UINT32 remainLength;
                UINT32 NumSuccess;
                AMBA_FS_FILE *pFileTest=NULL;
                remainLength = 20U - SvcWrap_strlen(pArgVector[2U]);
                for(i=0; i<remainLength; i++){
                    space[i] = ' ';
                }
                space[remainLength] = '\0';
     AmbaPrint_PrintStr5("---------------------------------------------------------------------------------------------------", NULL,NULL,NULL,NULL,NULL);
    AmbaPrint_PrintStr5("ItemName                    Seeds                  Min                  Max", NULL,NULL,NULL,NULL,NULL);
    AmbaPrint_PrintStr5("---------------------------------------------------------------------------------------------------", NULL,NULL,NULL,NULL,NULL);

                SVC_WRAP_SNPRINT "%s%s     0x%08X / %3u        0x%08X / %3u         0x%08X / %3u"
                    SVC_SNPRN_ARG_S printBuf
                    SVC_SNPRN_ARG_CSTR  pArgVector[2U] SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_CSTR space SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 12  SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 13 SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 14 SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 12  SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 13 SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 14 SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_BSIZE  512
                    SVC_SNPRN_ARG_RLEN   &RetVal
                    SVC_SNPRN_ARG_E
                AmbaPrint_PrintStr5("%s",printBuf, NULL,NULL,NULL,NULL);

                RetVal = AmbaFS_FileOpen("c:\\DDR_Tuning Result.txt","w+",&pFileTest);
                if(RetVal != SVC_OK){
                    SvcLog_NG("SVC_LOG_SHMOO_TASK", "File Open Failed",0U,0U);
                    AmbaFS_FileClose(pFileTest);
                } else{
                    remainLength =SvcWrap_strlen(printBuf);
                    RetVal = AmbaFS_FileWrite(printBuf, remainLength, 1U, pFileTest, &NumSuccess);
                    if(RetVal != SVC_OK){
                        SvcLog_NG("SVC_LOG_SHMOO_TASK", "File Write failed %u NumSuccess = %u", RetVal, NumSuccess);
                    }
                    AmbaFS_FileClose(pFileTest);
                }
            #endif

         } else if (0 == SvcWrap_strcmp("dmic", pArgVector[1U])) {
            UINT32 RetVal = SVC_OK;

            RetVal = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed with %d", RetVal, 0U);
            } else {
                if (0 == SvcWrap_strcmp("enable", pArgVector[2U])) {
                    pSvcUserPref->EnableDMIC = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "Enable DMIC", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("disable", pArgVector[2U])) {
                    pSvcUserPref->EnableDMIC = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "Disable DMIC", 0U, 0U);
                } else {
                    //Do nothing
                }
                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Save() failed with %d", RetVal, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_CMDAPP, "Save operation mode \"Command\"(%d) to user preference", 0U, 0U);
                }
                /* Reboot to command mode if boot status is not command mode */
                if (SVC_OK != AmbaSYS_Reboot()) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                }
            }
        } else if (0 == SvcWrap_strcmp("res_cfg", pArgVector[1U])) {
            if (SVC_OK != SvcResCfg_Dump()) {
                SvcLog_NG(SVC_LOG_CMDAPP, "res_cfg dump failed", 0U, 0U);
            }
            Rval = SVC_OK;
        } else if (0 == SvcWrap_strcmp("res_cfg_max", pArgVector[1U])) {
            if (SVC_OK != SvcResCfg_DumpMax()) {
                SvcLog_NG(SVC_LOG_CMDAPP, "res_cfg_max dump failed", 0U, 0U);
            }
            Rval = SVC_OK;
        } else if (0 == SvcWrap_strcmp("dsp_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              ShowDspLog = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    ShowDspLog = 1U;
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    ShowDspLog = 0U;
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->ShowDspLog = (UINT8)ShowDspLog;
                        pSvcUserPref->ShowSspLog = (UINT8)ShowDspLog;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                        IsReboot = 1U;
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("ssp_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ssp_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ssp_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(SSP_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowSspLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("ssp_api_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U, Err;

                extern UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask);
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ssp_api_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ssp_api_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(SSP_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowSspApiLog), (UINT8)Enable);
                        Err = AmbaDSP_MainApiDumpInit((UINT8)Enable, 0U);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaDSP_MainApiDumpInit() failed!!", Err, 0U);
                        }
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("sensor_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "sensor_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "sensor_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(SENSOR_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowSensorLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("bridge_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "bridge_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "bridge_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(BRIDGE_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowBridgeLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("yuv_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "yuv_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "yuv_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(YUV_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowYuvLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("log", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32  Enable = 0U, Module = 0U;

                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &Module)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }
                if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    if (SVC_OK != AmbaPrint_ModuleSetAllowList((UINT16)Module, (UINT8)Enable)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaPrint_ModuleSetAllowList failed!!", 0U, 0U);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#if defined(IK_ERR_BASE)
        } else if (0 == SvcWrap_strcmp("ik_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ik_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ik_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(IK_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowIKLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#endif
        } else if (0 == SvcWrap_strcmp("vidthm", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "vidthm on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "vidthm off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->EnableVidThm = (UINT8)Enable;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("dsp_dbg_lvl", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Value;

                if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->DspDbgLvl = Value;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        } else {
                            SvcLog_OK(SVC_LOG_CMDAPP, "dsp_dbg_lvl %u saved", pSvcUserPref->DspDbgLvl, 0U);
                        }
                    }
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("dsp_dbg_thd", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Value;

                if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->DspDbgThd = Value;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        } else {
                            SvcLog_OK(SVC_LOG_CMDAPP, "dsp_dbg_thd %u saved", pSvcUserPref->DspDbgThd, 0U);
                        }
                    }
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("ipcdbg", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    SvcSafeStateMgr_Debug(1);
                } else {
                    SvcSafeStateMgr_Debug(0);
                }
            }
        } else if (0 == SvcWrap_strcmp("assert", pArgVector[1U])) {
            AmbaAssert();
#if defined(CONFIG_ICAM_SWPLL_CONTROL)
        } else if (0 == SvcWrap_strcmp("swpll_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32              Enable = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "swpll_log on", 0U, 0U);
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    Enable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "swpll_log off", 0U, 0U);
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    UINT16 Module = (UINT16)(FTCM_ERR_BASE >> 16U);
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        SetModuleLog(Module, &(pSvcUserPref->ShowSwpllLog), (UINT8)Enable);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("swpll_show", pArgVector[1U])) {
            SvcSwPll_PrintINT();
        } else if (0 == SvcWrap_strcmp("audclk", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 AudId;
                if (0 == SvcWrap_strcmp("get", pArgVector[2U])) {
                    UINT32 OrgClk;
                     if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &AudId)) {
                        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0 + AudId, &OrgClk);
                        if (Rval == OK) {
                            SvcLog_OK(SVC_LOG_CMDAPP, "aud %u clk %u", AudId, OrgClk);
                        }
                    }
                }
                if ((0 == SvcWrap_strcmp("set", pArgVector[2U])) && (4U < ArgCount)) {
                    UINT32 TargetClk, ActClk;
                    if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &AudId)) {
                        if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &TargetClk)) {
                            Rval = SvcSafeStateMgr_SetClkFreq(AMBA_SYS_CLK_AUD_0 + AudId, TargetClk, &ActClk);
                            if (Rval == OK) {
                                SvcLog_OK(SVC_LOG_CMDAPP, "aud %u target clk %u", AudId, TargetClk);
                                SvcLog_OK(SVC_LOG_CMDAPP, "      act clk %u", ActClk, 0);
                            }
                        }
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("voclk", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 VoId;
                if (0 == SvcWrap_strcmp("get", pArgVector[2U])) {
                    UINT32 OrgClk;
                     if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &VoId)) {
                        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VID_OUT0 + VoId, &OrgClk);
                        if (Rval == OK) {
                            SvcLog_OK(SVC_LOG_CMDAPP, "vo %u clk %u", VoId, OrgClk);
                        }
                    }
                }
                if ((0 == SvcWrap_strcmp("set", pArgVector[2U])) && (4U < ArgCount)) {
                    UINT32 TargetClk, ActClk;
                    if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &VoId)) {
                        if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &TargetClk)) {
                            Rval = SvcSafeStateMgr_SetClkFreq(AMBA_SYS_CLK_VID_OUT0 + VoId, TargetClk, &ActClk);
                            if (Rval == OK) {
                                SvcLog_OK(SVC_LOG_CMDAPP, "vo %u target clk %u", VoId, TargetClk);
                                SvcLog_OK(SVC_LOG_CMDAPP, "      act clk %u", ActClk, 0);
                            }
                        }
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("viclk", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 ViId, ClkSrcIdBase, SensorIdx;
                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &ViId)) {
                    Rval = SvcResCfg_GetSerdesIdxOfSensorIdx(ViId, 0, &SensorIdx);
                    if (Rval == OK) {
                        if (SensorIdx != 0xDeadbeafU) {
                            ClkSrcIdBase = AMBA_SYS_CLK_VID_IN0;
                        } else {
                            ClkSrcIdBase = AMBA_SYS_CLK_REF_OUT0;
                        }
                        if (0 == SvcWrap_strcmp("get", pArgVector[2U])) {
                            UINT32 OrgClk;
                            Rval = AmbaSYS_GetClkFreq(ClkSrcIdBase + ViId, &OrgClk);
                            if (Rval == OK) {
                                SvcLog_OK(SVC_LOG_CMDAPP, "vi %u clk %u", ViId, OrgClk);
                            }
                        }
                        if ((0 == SvcWrap_strcmp("set", pArgVector[2U])) && (4U < ArgCount)) {
                            UINT32 TargetClk, ActClk;
                            if (SVC_OK == SvcWrap_strtoul(pArgVector[4U], &TargetClk)) {
                                Rval = SvcSafeStateMgr_SetClkFreq(ClkSrcIdBase + ViId, TargetClk, &ActClk);
                                if (Rval == OK) {
                                    SvcLog_OK(SVC_LOG_CMDAPP, "vi %u target clk %u", ViId, TargetClk);
                                    SvcLog_OK(SVC_LOG_CMDAPP, "      act clk %u", ActClk, 0);
                                }
                            }
                        }
                    }
                }
            }
#endif
        } else if (0 == SvcWrap_strcmp("standby", pArgVector[1U])) {
            Rval = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed with %d", Rval, 0U);
            } else {
                pSvcUserPref->OperationMode = 0U;   /* Stand-by mode */
                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Save() failed with %d", Rval, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_CMDAPP, "Save operation mode \"Stand-by\"(%d) to user preference", 0U, 0U);
                }
                IsReboot = 1U;
            }
        } else if (0 == SvcWrap_strcmp("dcf", pArgVector[1U])) {
            Rval = SvcUserPref_Get(&pSvcUserPref);
            if (SVC_OK == Rval) {
                if (0 == SvcWrap_strcmp("0", pArgVector[2U])) {
                    pSvcUserPref->DcfFormatId = 0U;
                    SvcLog_OK("AmbaDCF_ConfigFormat", "%d", pSvcUserPref->DcfFormatId, 0U);
                } else if (0 == SvcWrap_strcmp("1", pArgVector[2U])) {
                    pSvcUserPref->DcfFormatId = 1U;
                    SvcLog_OK("AmbaDCF_ConfigFormat", "%d", pSvcUserPref->DcfFormatId, 0U);
                } else {
                    //pass vcast
                }

                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save return %u", Rval, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_CMDAPP, "line %u SvcUserPref_Get return %u", __LINE__, Rval);
            }
            IsReboot = 1U;
#if defined(CONFIG_BUILD_AMBA_ADAS)
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) || defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
        } else if (0 == SvcWrap_strcmp("emr_cal_load" , pArgVector[1U])) {
            if (2U < ArgCount) {
                #define AMBA_EMR  1U
                #define STCH 2U

                UINT8 Misra_0;
                // UINT32 Rval;
                UINT32 LoadType = 0U;
                AMBA_CT_INITIAL_CONFIG_s InitCfg;
                static UINT32 EmirWorkBufSize;
                static ULONG  Emir3in3TunerWorkBuf;
                static void *pEmir3in3TunerWorkBuf;
                //static void *pCalPointer;

                static UINT32 EmrWorkBufSize;
                static ULONG  EmrCalibWorkingBuf;
                static void *pEmrCalibWorkingBuf;

                static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_EM_SV_CFG_s Emirror3in3WarpConfig GNU_SECTION_NOZEROINIT;

//                static AMBA_CAL_EM_SV_DATA_s CalWarpOutputData;
                static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CALC_COORD_CFG_s CalibCoordCfg GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;

                static char emr_cal_load_left[]      = "C:\\3in3_emir_tuner_left.txt";
                static char emr_cal_load_right[]     = "C:\\3in3_emir_tuner_right.txt";
                static char emr_cal_load_rear[]      = "C:\\3in3_emir_tuner_rear.txt";
                static char emr_cal_load_stch[]      = "C:\\stitch.txt";

                if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                }

                emr_cal_load_left[0U]  = pSvcUserPref->MainStgDrive[0];
                emr_cal_load_right[0U] = pSvcUserPref->MainStgDrive[0];
                emr_cal_load_rear[0U]  = pSvcUserPref->MainStgDrive[0];
                emr_cal_load_stch[0U]  = pSvcUserPref->MainStgDrive[0];

                Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request failed!!", 0U, 0U);
                }
                if (SVC_OK != AmbaWrap_memset(&InitCfg, 0, sizeof(InitCfg))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset InitCfg failed!!", 0U, 0U);
                }
                InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                AmbaMisra_TypeCast(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;

                Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                }

                if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                    Rval = AmbaCT_Load(emr_cal_load_left);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("AmbaCT_Load 3in3_emir_tuner_left.txt fail", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        LoadType = AMBA_EMR;
                    }
                } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])) {
                    Rval = AmbaCT_Load(emr_cal_load_right);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("AmbaCT_Load 3in3_emir_tuner_right.txt fail", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        LoadType = AMBA_EMR;
                    }
                } else if (0 == SvcWrap_strcmp("rear", pArgVector[2U])) {
                    Rval = AmbaCT_Load(emr_cal_load_rear);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("AmbaCT_Load 3in3_emir_tuner_rear.txt fail", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        LoadType = AMBA_EMR;
                    }
                } else if (0 == SvcWrap_strcmp("script", pArgVector[2U])) {
                    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_3IN1_EMIRROR, &EmrCalibWorkingBuf, &EmrWorkBufSize);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("SvcBuffer_Request fail", 0U, 0U, 0U, 0U, 0U);
                    }

                    InitCfg.TunerWorkingBufSize = EmrWorkBufSize;
                    InitCfg.Emirror.CalibWorkingBufSize = EmrWorkBufSize;
                    AmbaMisra_TypeCast(&pEmrCalibWorkingBuf, &EmrCalibWorkingBuf);
                    InitCfg.pTunerWorkingBuf = pEmrCalibWorkingBuf;
                    InitCfg.Emirror.pCalibWorkingBuf = pEmrCalibWorkingBuf;
                    InitCfg.Emirror.CbFeedMaskFunc = NULL;
                    Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("AmbaCT_Init fail", 0U, 0U, 0U, 0U, 0U);
                    }

                    Rval = AmbaCT_Load(emr_cal_load_stch);
                    if (SVC_OK == Rval) {
                        LoadType = STCH;
                    } else {
                        AmbaPrint_PrintUInt5("AmbaCT_Load stitch.txt fail", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("Invalid FileId", 0U, 0U, 0U, 0U, 0U);
                }

                if (LoadType == AMBA_EMR) {
                    Rval |= AmbaWrap_memset(&LensData, 0, sizeof(LensData));
                    Rval |= AmbaWrap_memset(&CameraData, 0, sizeof(CameraData));
                    Rval |= AmbaWrap_memset(&AspPointMap, 0, sizeof(AspPointMap));
                    Rval |= AmbaWrap_memset(&CalibPointData, 0, sizeof(CalibPointData));
                    Rval |= AmbaWrap_memset(&AssistancePointData, 0, sizeof(AssistancePointData));
                    Rval |= AmbaWrap_memset(&Aspheric, 0, sizeof(Aspheric));
                    Rval |= AmbaWrap_memset(&CurvedSurface, 0, sizeof(CurvedSurface));
                    Rval |= AmbaWrap_memset(&CalibCoordCfg, 0, sizeof(CalibCoordCfg));
                    Rval |= AmbaWrap_memset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
                    Rval |= AmbaWrap_memset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
                    Rval |= AmbaWrap_memset(&LensSpecFormula, 0, sizeof(LensSpecFormula));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset in LoadType AMBA_EMR failed!!", 0U, 0U);
                    }

                    AmbaCT_EmGetLens(0U, &LensData);
                    Rval = RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("RefCalib_EmFeedLensSpec fail", 0U, 0U, 0U, 0U, 0U);
                    }

                    AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfig.Cam.Sensor);
                    AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfig.Cam.OpticalCenter);
                    AmbaCT_EmGetCamera(0U, &CameraData);
                    Emirror3in3WarpConfig.Cam.Pos.X = CameraData.PositionX;
                    Emirror3in3WarpConfig.Cam.Pos.Y = CameraData.PositionY;
                    Emirror3in3WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
                    Emirror3in3WarpConfig.Cam.Rotation = CameraData.RotateType;
                    AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfig.VinSensorGeo);
                    AmbaCT_EmGetSingleView(&Emirror3in3WarpConfig.View.Type);

                    if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_LDC) {
                        AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_ASPHERIC) {
                        Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
                        if (SVC_OK != Rval) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
                        }
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
                        Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
                        if (SVC_OK != Rval) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
                        }
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else {
                        AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
                    }

                    AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfig.View.VoutArea);
                    Emirror3in3WarpConfig.View.PlugIn.PlugInMode = 0U; // TODO:
                    //Emirror3in3WarpConfig.View.PlugIn.MsgReciver = RefCalib_EmSvCbMsgReciver;

                    AmbaCT_EmGetCalibPoints(0U, &CalibPointData);
                    AmbaCT_EmGetAssistancePoints(0U, &AssistancePointData);

                    if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                        for (UINT32 Idx = 0U; Idx < EMIR_CALIB_POINT_NUM; Idx++) {
                            if(EmrQuickCal == ENABLE) {
                                CalibPointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_LEFT].RawPositionX[Idx];
                                CalibPointData.RawPositionY[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_LEFT].RawPositionY[Idx];
                                AssistancePointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_LEFT].RawPositionX[Idx];
                                AssistancePointData.RawPositionY[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_LEFT].RawPositionY[Idx];
                            }
                        }
                    } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])){
                        for (UINT32 Idx = 0U; Idx < EMIR_CALIB_POINT_NUM; Idx++) {
                            if(EmrQuickCal == ENABLE) {
                                CalibPointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_RIGHT].RawPositionX[Idx];
                                CalibPointData.RawPositionY[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_RIGHT].RawPositionY[Idx];
                                AssistancePointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_RIGHT].RawPositionX[Idx];
                                AssistancePointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_RIGHT].RawPositionX[Idx];
                            }
                        }
                    } else if (0 == SvcWrap_strcmp("rear", pArgVector[2U])){
                        for (UINT32 Idx = 0U; Idx < EMIR_CALIB_POINT_NUM; Idx++) {
                            if(EmrQuickCal == ENABLE) {
                                CalibPointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_BACK].RawPositionX[Idx];
                                CalibPointData.RawPositionY[Idx] = TEMP_AMBA_CT_EM_CALIB_POINTS[AMBA_CAL_EM_CAM_BACK].RawPositionY[Idx];
                                AssistancePointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_BACK].RawPositionX[Idx];
                                AssistancePointData.RawPositionX[Idx] = TEMP_AMBA_CT_EM_ASSISTANCE_POINTS[AMBA_CAL_EM_CAM_BACK].RawPositionX[Idx];
                            }
                        }
                    } else {
                        //AmbaPrint_PrintUInt5("SvcCalib_AdasCfgSet Invalid FileId", 0U, 0U, 0U, 0U, 0U);
                    }
                    if (SVC_OK != AmbaWrap_memset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmirrorPointMap failed!!", 0U, 0U);
                    }
                    RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
                    Emirror3in3WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
                    Emirror3in3WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
                    Emirror3in3WarpConfig.Calibinfo.pMultiPoint = NULL;

                    AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
                    AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
                    Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
                    AmbaCT_EmGetOptimize(0U, &Misra_0);
                    AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);
//                    Rval = AmbaCal_EmGenSingleView(&Emirror3in3WarpConfig, pEmir3in3TunerWorkBuf, &CalWarpOutputData);
//                    if (Rval != 0U) {
//                        AmbaPrint_PrintStr5("%s, AmbaCal_EmGenSingleView() failed!", __func__, NULL, NULL, NULL, NULL);
//                    }

                    Rval = RefCalib_ConvImgToWorldPlane(Emirror3in3WarpConfig, &CalibCoordCfg);
                    if (SVC_OK != Rval) {
                        AmbaPrint_PrintUInt5("RefCalib_ConvImgToWorldPlane fail", 0U, 0U, 0U, 0U, 0U);
                    }

                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                    //RefCalib_ShowCalibWarpOutput(&CalWarpOutputData);

                    if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                        if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_LEFT, &CalibCoordCfg)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_LEFT failed!!", 0U, 0U);
                        }
                    } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])){
                        if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_RIGHT, &CalibCoordCfg)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_RIGHT failed!!", 0U, 0U);
                        }
                    } else if (0 == SvcWrap_strcmp("rear", pArgVector[2U])){
                        if (SVC_OK !=SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_BACK, &CalibCoordCfg)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_BACK failed!!", 0U, 0U);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("SvcCalib_AdasCfgSet Invalid FileId", 0U, 0U, 0U, 0U, 0U);
                    }
                } else if (LoadType == STCH) {
#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
                    UINT8  CamIdx;
                    UINT32 CamNum = 3U; // B, L, R
                    ULONG  EmrCalibBuf;
                    UINT32 EmrCalibBufSize;
                    ULONG  EmrCalib3in1Buf;
                    UINT32 EmrCalib3in1BufSize;
                    const void *pEmCalib3in1WorkingBuf;
                    AMBA_CAL_EM_CALIB_INFO_CFG_s  EmCalibCfg;
                    AMBA_CAL_EM_CALIB_INFO_DATA_s CalOutput;
                    AMBA_CAL_EM_SV_CFG_s          Emirror3in1WarpConfig;
                    AMBA_CT_EM_LENS_s EmrLensData[AMBA_DSP_MAX_VIEWZONE_NUM];

                    AMBA_CAL_EM_3IN1_DATA_s          Cal3in1Output;
                    AMBA_CAL_EM3IN1_GEN_TBL_CFG_V1_s EmCalib3in1Cfg;

                    AMBA_CAL_SIZE_s         CarData;
                    AMBA_CT_EM_3IN1_VIEW_s  Em3in1ViewCfg;
                    AMBA_CT_EM_3IN1_BLEND_s Em3in1Blend;
                    AMBA_CT_EM_MASK_s       *pEmMask = NULL;

                    SVC_CALIB_ST_LDC_TBL_DATA_s   EmrCalLdcTbl[AMBA_DSP_MAX_VIEWZONE_NUM];
                    SVC_CALIB_ST_BLEND_TBL_DATA_s EmrCalBldTbl;
                    SVC_CALIB_TBL_INFO_s          EmrCalTbl[AMBA_DSP_MAX_VIEWZONE_NUM];
                    SVC_CALIB_CHANNEL_s           CalibChan;

                    for (CamIdx = 0U; CamIdx < CamNum; CamIdx++) {
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB_EMIRROR, &EmrCalibBuf, &EmrCalibBufSize);

                        if (SVC_OK == Rval) {
                            AmbaMisra_TypeCast(&pEmrCalibWorkingBuf, &EmrCalibBuf);
                            Rval |= AmbaWrap_memset(&EmCalibCfg, 0, sizeof(AMBA_CAL_EM_CALIB_INFO_CFG_s));
                            Rval |= AmbaWrap_memset(&EmrLensData, 0, sizeof(EmrLensData));
                            Rval |= AmbaWrap_memset(&CameraData, 0, sizeof(CameraData));
                            Rval |= AmbaWrap_memset(&CalibPointData, 0, sizeof(CalibPointData));
                            Rval |= AmbaWrap_memset(&AssistancePointData, 0, sizeof(AssistancePointData));
                            Rval |= AmbaWrap_memset(&Emirror3in1WarpConfig, 0, sizeof(Emirror3in1WarpConfig));
                            Rval |= AmbaWrap_memset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
                            Rval |= AmbaWrap_memset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
                            Rval |= AmbaWrap_memset(&LensSpecFormula, 0, sizeof(LensSpecFormula));
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset in LoadType STCH failed!!", 0U, 0U);
                            }

                            /* prepare Emirror3in1WarpConfig */
                            AmbaCT_EmGetLens(CamIdx, &EmrLensData[CamIdx]);
                            Rval = RefCalib_EmFeedLensSpec(&EmrLensData[CamIdx], &Emirror3in1WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula);
                            if (SVC_OK != Rval) {
                                AmbaPrint_PrintUInt5("RefCalib_EmFeedLensSpec fail", 0U, 0U, 0U, 0U, 0U);
                            }

                            AmbaCT_EmGetSensor(CamIdx, &Emirror3in1WarpConfig.Cam.Sensor);
                            AmbaCT_EmGetOpticalCenter(CamIdx, &Emirror3in1WarpConfig.Cam.OpticalCenter);
                            AmbaCT_EmGetCamera(CamIdx, &CameraData);
                            Emirror3in1WarpConfig.Cam.Pos.X = CameraData.PositionX;
                            Emirror3in1WarpConfig.Cam.Pos.Y = CameraData.PositionY;
                            Emirror3in1WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
                            Emirror3in1WarpConfig.Cam.Rotation = CameraData.RotateType;

                            AmbaCT_EmGetVout(CamIdx, &Emirror3in1WarpConfig.View.VoutArea);
                            AmbaCT_EmGetCalibPoints(CamIdx, &CalibPointData);
                            AmbaCT_EmGetAssistancePoints(CamIdx, &AssistancePointData);

                            if (SVC_OK != AmbaWrap_memset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s))) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmirrorPointMap failed!!", 0U, 0U);
                            }
                            RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
                            Emirror3in1WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
                            Emirror3in1WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
                            Emirror3in1WarpConfig.Calibinfo.pMultiPoint = NULL;

                            /* fill in EmCalibCfg */
                            if (SVC_OK != AmbaWrap_memcpy(&EmCalibCfg.Cam, &Emirror3in1WarpConfig.Cam, sizeof(AMBA_CAL_CAM_s))) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalibCfg.Cam failed!!", 0U, 0U);
                            }
                            RefCalib_ConverToPixel(&EmCalibCfg.Cam);
                            Rval |= AmbaWrap_memcpy(&EmCalibCfg.VoutArea, &Emirror3in1WarpConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s));
                            Rval |= AmbaWrap_memcpy(&EmCalibCfg.Calibinfo, &Emirror3in1WarpConfig.Calibinfo, sizeof(AMBA_CAL_EM_CALIB_POINT_INFO_s));
                            Rval |= AmbaWrap_memcpy(&EmCalibCfg.OptimizeLevel, &Emirror3in1WarpConfig.OptimizeLevel, sizeof(AMBA_CAL_EM_OPTIMIZE_LEVEL_e));
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalibCfg.VoutArea/Calibinfo/OptimizeLevel failed!!", 0U, 0U);
                            }
                            EmCalibCfg.PointMapHFlipEnable = 0U;
                            Rval = AmbaCal_EmGenCalibInfo(&EmCalibCfg, pEmrCalibWorkingBuf, &CalOutput);
                            if (SVC_OK != Rval) {
                                AmbaPrint_PrintUInt5("AmbaCal_EmGenCalibInfo fail", 0U, 0U, 0U, 0U, 0U);
                            }

                            if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.Cam[CamIdx].Cam, &Emirror3in1WarpConfig.Cam, sizeof(AMBA_CAL_CAM_s))) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.Cam[CamIdx].Cam failed!!", 0U, 0U);
                            }
                            RefCalib_ConverToPixel(&EmCalib3in1Cfg.Cam[CamIdx].Cam);
                            if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.Cam[CamIdx].CalibDataWorld2Raw, &CalOutput.CalibDataWorld2Raw, sizeof(AMBA_CAL_EM_CAM_CALIB_DATA_s))) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.Cam[CamIdx].CalibDataWorld2Raw failed!!", 0U, 0U);
                            }
                        }
                    }

                    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_3IN1V_EMIRROR, &EmrCalib3in1Buf, &EmrCalib3in1BufSize);

                    if (Rval == SVC_OK) {
                        AmbaMisra_TypeCast(&pEmCalib3in1WorkingBuf, &EmrCalib3in1Buf);

                        /* fill in AMBA_CAL_EM_3IN1_GEN_TBL_CFG_s */
                        Rval |= AmbaWrap_memset(&CarData, 0, sizeof(AMBA_CAL_SIZE_s));
                        Rval |= AmbaWrap_memset(&Em3in1ViewCfg, 0, sizeof(AMBA_CT_EM_3IN1_VIEW_s));
                        Rval |= AmbaWrap_memset(&Em3in1Blend, 0, sizeof(AMBA_CT_EM_3IN1_BLEND_s));
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset CarData/Em3in1ViewCfg/Em3in1Blend failed!!", 0U, 0U);
                        }

                        AmbaCT_EmGetCar(&CarData);
                        AmbaCT_EmGet3in1View(&Em3in1ViewCfg);
                        AmbaCT_EmGet3in1Blend(&Em3in1Blend);

                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.Car, &CarData, sizeof(AMBA_CAL_SIZE_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.Car failed", 0U, 0U);
                        }
                        EmCalib3in1Cfg.View.ViewStartPos.X = Em3in1ViewCfg.StartPosX;
                        EmCalib3in1Cfg.View.ViewStartPos.Y = Em3in1ViewCfg.StartPosY;
                        EmCalib3in1Cfg.View.ViewStartPos.Z = Em3in1ViewCfg.StartPosZ;
                        EmCalib3in1Cfg.View.ViewWidth = Em3in1ViewCfg.Width;
                        EmCalib3in1Cfg.View.DistanceOfStitchPlane = Em3in1ViewCfg.DistanceOfStitchPlane;
                        EmCalib3in1Cfg.View.BackViewScale = Em3in1ViewCfg.BackViewScale;
                        EmCalib3in1Cfg.View.BackViewVerShift = Em3in1ViewCfg.BackViewVerShift;
                        EmCalib3in1Cfg.View.LeftViewHorShift = Em3in1ViewCfg.LeftViewHorShift;
                        EmCalib3in1Cfg.View.RightViewHorShift = Em3in1ViewCfg.RightViewHorShift;
                        EmCalib3in1Cfg.View.ValidVoutLowerBound = Em3in1ViewCfg.ValidVoutLowerBound;
                        EmCalib3in1Cfg.View.MirrorFlipCfg = Em3in1ViewCfg.MirrorFlipCfg;
                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.View.BlendLeft, &Em3in1Blend.Left, sizeof(AMBA_CAL_EM_BLEND_CFG_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.View.BlendLeft failed", 0U, 0U);
                        }
                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.View.BlendRight, &Em3in1Blend.Right, sizeof(AMBA_CAL_EM_BLEND_CFG_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.View.BlendRight failed", 0U, 0U);
                        }
                        AmbaCT_EmGetVout(0U, &Emirror3in1WarpConfig.View.VoutArea);
                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.View.VoutAreaBack, &Emirror3in1WarpConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.View.VoutAreaBack failed", 0U, 0U);
                        }
                        AmbaCT_EmGetVout(1U, &Emirror3in1WarpConfig.View.VoutArea);
                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.View.VoutAreaLeft, &Emirror3in1WarpConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.View.VoutAreaLeft failed", 0U, 0U);
                        }
                        AmbaCT_EmGetVout(2U, &Emirror3in1WarpConfig.View.VoutArea);
                        if (SVC_OK != AmbaWrap_memcpy(&EmCalib3in1Cfg.View.VoutAreaRight, &Emirror3in1WarpConfig.View.VoutArea, sizeof(AMBA_CAL_ROI_s))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmCalib3in1Cfg.View.VoutAreaRight failed", 0U, 0U);
                        }

                        AmbaCT_EmGetMask(0U, &pEmMask);
                        EmCalib3in1Cfg.View.MaskBack.Width    = pEmMask->Width;
                        EmCalib3in1Cfg.View.MaskBack.Height   = pEmMask->Height;
                        EmCalib3in1Cfg.View.MaskBack.pMaskTbl = pEmMask->Table;
                        AmbaCT_EmGetMask(1U, &pEmMask);
                        EmCalib3in1Cfg.View.MaskLeft.Width    = pEmMask->Width;
                        EmCalib3in1Cfg.View.MaskLeft.Height   = pEmMask->Height;
                        EmCalib3in1Cfg.View.MaskLeft.pMaskTbl = pEmMask->Table;
                        AmbaCT_EmGetMask(2U, &pEmMask);
                        EmCalib3in1Cfg.View.MaskRight.Width    = pEmMask->Width;
                        EmCalib3in1Cfg.View.MaskRight.Height   = pEmMask->Height;
                        EmCalib3in1Cfg.View.MaskRight.pMaskTbl = pEmMask->Table;

                        for (CamIdx = 0U; CamIdx < CamNum; CamIdx++) {
                            AmbaCT_EmGetVin(CamIdx, &EmCalib3in1Cfg.Cam[CamIdx].VinSensorGeo);
                            AmbaCT_EmGetRoi(CamIdx, &EmCalib3in1Cfg.Cam[CamIdx].ROI);
                            AmbaCT_EmGetTileSize(CamIdx, &EmCalib3in1Cfg.Cam[CamIdx].Tile);
                            AmbaCT_EmGetInternal(CamIdx, &EmCalib3in1Cfg.Cam[CamIdx].InternalCfg);
                            EmCalib3in1Cfg.Cam[CamIdx].PlugIn.PlugInMode = 0U;
                        }

                        Rval = AmbaCal_EmGen3in1VV1(&EmCalib3in1Cfg, pEmCalib3in1WorkingBuf, &Cal3in1Output);
                        if (Rval == SVC_OK) {

                            UINT32 VinId, SerDesIdx, FovIdx;
                            UINT32 i, BlendVin = 0U, BlendSensor = 0U;
                            const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

                            for (CamIdx = 0U; CamIdx < CamNum; CamIdx++) {

                                Rval |= AmbaWrap_memset(&EmrCalLdcTbl, 0, sizeof(EmrCalLdcTbl));
                                Rval |= AmbaWrap_memset(&CalibChan, 0, sizeof(CalibChan));
                                Rval |= AmbaWrap_memset(&EmrCalTbl, 0, sizeof(EmrCalTbl));
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmrCalLdcTbl/CalibChan/EmrCalTbl failed", 0U, 0U);
                                }

                                EmrCalLdcTbl[CamIdx].HorGridNum = Cal3in1Output.Cam[CamIdx].WarpTbl.HorGridNum;
                                EmrCalLdcTbl[CamIdx].VerGridNum = Cal3in1Output.Cam[CamIdx].WarpTbl.VerGridNum;
                                EmrCalLdcTbl[CamIdx].TileWidthExp  = Cal3in1Output.Cam[CamIdx].WarpTbl.TileWidthExp;
                                EmrCalLdcTbl[CamIdx].TileHeightExp = Cal3in1Output.Cam[CamIdx].WarpTbl.TileHeightExp;
                                if (SVC_OK != AmbaWrap_memcpy(&EmrCalLdcTbl[CamIdx].VinSensorGeo, &Cal3in1Output.Cam[CamIdx].WarpTbl.CalibSensorGeo, sizeof(AMBA_IK_VIN_SENSOR_GEOMETRY_s))) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmrCalLdcTbl[CamIdx].VinSensorGeo failed", 0U, 0U);
                                }
                                EmrCalLdcTbl[CamIdx].pStLdcVector = Cal3in1Output.Cam[CamIdx].WarpTbl.WarpVector;
                                EmrCalLdcTbl[CamIdx].StLdcVectorLength = MAX_WARP_TBL_LEN;

                                FovIdx = (UINT32) 2U - CamIdx;
                                Rval = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinId);
                                Rval |= SvcResCfg_GetSerdesIdxOfFovIdx(FovIdx, &SerDesIdx);
                                if (Rval == SVC_OK) {
                                    CalibChan.VinID = VinId;
                                    CalibChan.SensorID = (UINT32) 1U << SerDesIdx;
                                    CalibChan.VinSelectBits = (UINT32) 1U << VinId;
                                    CalibChan.SensorSelectBits = (UINT32) 1U << SerDesIdx;

                                    if (SVC_OK != AmbaWrap_memcpy(&EmrCalTbl[CamIdx].CalChan, &CalibChan, sizeof(SVC_CALIB_CHANNEL_s))) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmrCalTbl[CamIdx].CalChan failed", 0U, 0U);
                                    }
                                    EmrCalTbl[CamIdx].pBuf = &EmrCalLdcTbl[CamIdx];
                                    EmrCalTbl[CamIdx].BufSize = (UINT32) sizeof(SVC_CALIB_ST_LDC_TBL_DATA_s);
                                    EmrCalTbl[CamIdx].DbgMsgOn = 0U;

                                    Rval = SvcCalib_ItemTableSet(SVC_CALIB_STITCH_LDC_ID, CamIdx, &EmrCalTbl[CamIdx]);
                                    if (Rval != SVC_OK) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "fail to SvcCalib_ItemTableSet", 0U, 0U);
                                    }
                                }

                                /* get blending info */
                                for (i = 0; i < pCfg->DispNum; i++) {
                                    if (pCfg->DispStrm[i].StrmCfg.ChanCfg[CamIdx].BlendEnable == (UINT8) 1U) {
                                        BlendVin    = VinId;
                                        BlendSensor = SerDesIdx;
                                    }
                                }
                            }

                            Rval |= AmbaWrap_memset(&EmrCalBldTbl, 0, sizeof(EmrCalBldTbl));
                            Rval |= AmbaWrap_memset(&CalibChan, 0, sizeof(CalibChan));
                            Rval |= AmbaWrap_memset(&EmrCalTbl, 0, sizeof(EmrCalTbl));
                            if (SVC_OK != Rval) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmrCalBldTbl/CalibChan/EmrCalTbl failed", 0U, 0U);
                            }

                            EmrCalBldTbl.Width = Cal3in1Output.BlendTbl.Width;
                            EmrCalBldTbl.Height = Cal3in1Output.BlendTbl.Height;
                            EmrCalBldTbl.pTbl = Cal3in1Output.BlendTbl.Table;
                            EmrCalBldTbl.TblLength = SVC_CALIB_STITCH_BLEND_TBL_SZ;

                            CamIdx = 0U;
                            CalibChan.VinID = BlendVin;
                            CalibChan.SensorID = BlendSensor;
                            CalibChan.VinSelectBits = (UINT32) 1 << BlendVin;
                            CalibChan.SensorSelectBits = (UINT32) 1 << BlendSensor;

                            if (SVC_OK != AmbaWrap_memcpy(&EmrCalTbl[CamIdx].CalChan, &CalibChan, sizeof(SVC_CALIB_CHANNEL_s))) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy EmrCalTbl[CamIdx].CalChan failed", 0U, 0U);
                            }
                            EmrCalTbl[CamIdx].pBuf = &EmrCalBldTbl;
                            EmrCalTbl[CamIdx].BufSize = (UINT32) sizeof(SVC_CALIB_ST_BLEND_TBL_DATA_s);
                            EmrCalTbl[CamIdx].DbgMsgOn = 0U;
                            Rval = SvcCalib_ItemTableSet(SVC_CALIB_STITCH_BLEND_ID, CamIdx, &EmrCalTbl[CamIdx]);
                            if (Rval != SVC_OK) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "fail to SvcCalib_ItemTableSet", 0U, 0U);
                            }
                        }
                    }
#else
                    SvcLog_NG(SVC_LOG_CMDAPP, "Enable calibration stitch first!", 0U, 0U);
#endif
                } else {
                    Rval = SVC_NG;
                }
            }
#endif
#ifdef CONFIG_ICAM_PROJECT_SURROUND
        } else if (0 == SvcWrap_strcmp("avm_cal_load", pArgVector[1U])) {
            static UINT32 AvmWorkBufSize;
            static ULONG  AvmTunerWorkBuf;
            AMBA_CT_INITIAL_CONFIG_s InitCfg;
            if (2U < ArgCount) {
                SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;
                if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CAL_SUR_CT, &AvmTunerWorkBuf, &AvmWorkBufSize)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_CAL_SUR_CT failed", 0U, 0U);
                }
                if (SVC_OK != AmbaWrap_memset(&InitCfg, 0, sizeof(InitCfg))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset InitCfg failed", 0U, 0U);
                }
                InitCfg.TunerWorkingBufSize = AvmWorkBufSize;
                AmbaMisra_TypeCast(&InitCfg.pTunerWorkingBuf, &AvmTunerWorkBuf);
                if (SVC_OK != AmbaCT_Init(AMBA_CT_TYPE_AVM, &InitCfg)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init AMBA_CT_TYPE_AVM failed", 0U, 0U);
                }
                Rval = SvcStgMonitor_GetDriveInfo('C', &DriveInfo);
                if (Rval != SVC_NG && DriveInfo.IsExist == 1U) {
                    Rval = AmbaCT_Load("C:\\Calib_V0_3D_avm_tuner.txt");
                } else {
                    Rval = AmbaCT_Load("D:\\Calib_V0_3D_avm_tuner.txt");
                }
                if(Rval != 0U) {
                    AmbaPrint_PrintUInt5("AmbaCT_Load C:\\Calib_V0_3D_avm_tuner.txt fail", 0U, 0U, 0U, 0U, 0U);
                } else {
                    const AMBA_CT_AVM_USER_SETTING_s * CalibUserSetting;
                    static AMBA_CAL_AVM_POINT_MAP_s PointMap GNU_SECTION_NOZEROINIT;
                    static SVC_CALIB_ADAS_NAND_TABLE_s AvmCalibData GNU_SECTION_NOZEROINIT;
                    static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
                    static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
                    static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;
                    const SVC_CALIB_ADAS_NAND_TABLE_s *pAvmCalibData = &AvmCalibData;
                    UINT32 Chan;
                    void *pCfg;

                    Rval |= AmbaWrap_memset(&AvmCalibData, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
                    Rval |= AmbaWrap_memset(&LensSpecRealExpect, 0, sizeof(AMBA_CAL_LENS_DST_REAL_EXPECT_s));
                    Rval |= AmbaWrap_memset(&LensSpecAngle, 0, sizeof(AMBA_CAL_LENS_DST_ANGLE_s));
                    Rval |= AmbaWrap_memset(&LensSpecFormula, 0, sizeof(AMBA_CAL_LENS_DST_FORMULA_s));
                    Rval |= AmbaWrap_memset(&PointMap, 0, sizeof(AMBA_CAL_AVM_POINT_MAP_s));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset in avm_cal_load failed", 0U, 0U);
                    }

                    if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                        Chan = (UINT32)AMBA_CAL_AVM_CAM_LEFT;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.X = -1.0;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.Y = 0.0;
                    } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])) {
                        Chan = (UINT32)AMBA_CAL_AVM_CAM_RIGHT;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.X = 1.0;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.Y = 0.0;
                    } else if (0 == SvcWrap_strcmp("front", pArgVector[2U])) {
                        Chan = (UINT32)AMBA_CAL_AVM_CAM_FRONT;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.X = 0.0;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.Y = 1.0;
                    } else if (0 == SvcWrap_strcmp("back", pArgVector[2U])) {
                        Chan = (UINT32)AMBA_CAL_AVM_CAM_BACK;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.X = 0.0;
                        AvmCalibData.AdasAvmPaCfg.CamDirection.Y = -1.0;
                    }  else {
                        Chan = (UINT32)AMBA_CAL_AVM_CAM_FRONT;
                        AmbaPrint_PrintStr5("%s, channel type not support", __func__, NULL, NULL, NULL, NULL);
                    }
                    CalibUserSetting = AmbaCT_AvmGetUserSetting();
                    AvmCalibData.AdasAvmPaCfg.Cam.Pos.X = CalibUserSetting->Camera[Chan].PositionX;
                    AvmCalibData.AdasAvmPaCfg.Cam.Pos.Y = CalibUserSetting->Camera[Chan].PositionY;
                    AvmCalibData.AdasAvmPaCfg.Cam.Pos.Z = CalibUserSetting->Camera[Chan].PositionZ;
                    AvmCalibData.AdasAvmPaCfg.Cam.Rotation = CalibUserSetting->Camera[Chan].RotateType;
                    AvmCalibData.AdasAvmPaCfg.CamDirection.Z = 0.0;
                    if (SVC_OK != AmbaWrap_memcpy(&AvmCalibData.AdasAvmPaCfg.Cam.OpticalCenter, &CalibUserSetting->OpticalCenter[Chan], sizeof(AMBA_CAL_POINT_DB_2D_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy AvmCalibData.AdasAvmPaCfg.Cam.OpticalCenter failed", 0U, 0U);
                    }
                    if (SVC_OK != AmbaWrap_memcpy(&AvmCalibData.AdasAvmPaCfg.Cam.Sensor, &CalibUserSetting->Sensor[Chan], sizeof(AMBA_CAL_SENSOR_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy AvmCalibData.AdasAvmPaCfg.Cam.Sensor failed", 0U, 0U);
                    }
                    if (SVC_OK != SvcAnimCalib_AvmFeedLensSpec(&(CalibUserSetting->Lens[Chan]), &AvmCalibData.AdasAvmPaCfg.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcAnimCalib_AvmFeedLensSpec failed", 0U, 0U);
                    }
                    if (SVC_OK != AmbaWrap_memcpy(&AvmCalibData.AdasAvm2DCfg.Car, &CalibUserSetting->Car, sizeof(AMBA_CAL_SIZE_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memcpy AvmCalibData.AdasAvm2DCfg.Car failed", 0U, 0U);
                    }

                    AvmCalibData.AdasAvmPaCfg.RawArea.StartX = AvmCalibData.AdasAvmPaCfg.Cam.Sensor.StartX;
                    AvmCalibData.AdasAvmPaCfg.RawArea.StartY = AvmCalibData.AdasAvmPaCfg.Cam.Sensor.StartY;
                    AvmCalibData.AdasAvmPaCfg.RawArea.Width = AvmCalibData.AdasAvmPaCfg.Cam.Sensor.Width;
                    AvmCalibData.AdasAvmPaCfg.RawArea.Height = AvmCalibData.AdasAvmPaCfg.Cam.Sensor.Height;
                    AvmCalibData.AdasAvmPaCfg.Cfg.pLDC = &AvmCalibData.AdasExtData05;
                    AvmCalibData.AdasAvmPaCfg.Cfg.pLDC->CompensateRatio = 0U;
                    AvmCalibData.AdasAvmPaCfg.Type = AMBA_CAL_AVM_MV_LDC;
                    Rval = SvcAnimCalib_AvmFeedPointMap(&(CalibUserSetting->CalibPointMap[Chan]), &CalibUserSetting->AssistancePointMap[Chan], &PointMap);
                    if (Rval == SVC_OK) {
                        Rval = AmbaCal_DistortionTblMmToPixel(AvmCalibData.AdasAvmPaCfg.Cam.Sensor.CellSize, &AvmCalibData.AdasAvmPaCfg.Cam.Lens);
                        if (Rval == SVC_OK) {
                            Rval = AmbaCal_GenCalibrationInfo((INT32)Chan, AvmCalibData.AdasAvmPaCfg.Cam.Lens, AvmCalibData.AdasAvmPaCfg.Cam.Pos, AvmCalibData.AdasAvmPaCfg.Cam.OpticalCenter, &PointMap, 0, NULL, &AvmCalibData.AdasAvmCalibCfg);
                            if (Rval != SVC_OK) {
                                AmbaPrint_PrintStr5("%s, AmbaCal_GenCalibrationInfo Fail", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                AmbaMisra_TypeCast(&pCfg, &pAvmCalibData);
                                if (Chan == (UINT32)AMBA_CAL_AVM_CAM_FRONT) {
                                    if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_AVM_F, pCfg)) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_AVM_F failed", 0U, 0U);
                                    }
                                } else if (Chan == (UINT32)AMBA_CAL_AVM_CAM_BACK) {
                                    if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_AVM_B, pCfg)) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_AVM_B failed", 0U, 0U);
                                    }
                                } else if (Chan == (UINT32)AMBA_CAL_AVM_CAM_LEFT) {
                                    if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_AVM_L, pCfg)) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_AVM_L failed", 0U, 0U);
                                    }
                                } else if (Chan == (UINT32)AMBA_CAL_AVM_CAM_RIGHT) {
                                    if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_AVM_R, pCfg)) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_AVM_R failed", 0U, 0U);
                                    }
                                } else {
                                    AmbaPrint_PrintStr5("%s, channel type not support", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    }

                }
            }
#endif
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) || defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
        } else if (0 == SvcWrap_strcmp("calget", pArgVector[1U])) {
            static AMBA_CAL_EM_CALC_COORD_CFG_s CalibCoordCfg;
            static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;

            CalibCoordCfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;

            if (0 == SvcWrap_strcmp("front", pArgVector[2U])) {
                if(SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &CalibCoordCfg) == 0U) {
                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                } else {
                    SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                if(SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_LEFT, &CalibCoordCfg) == 0U) {
                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                } else {
                    SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])) {
                if(SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_RIGHT, &CalibCoordCfg) == 0U) {
                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                } else {
                    SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else if (0 == SvcWrap_strcmp("rear", pArgVector[2U])) {
                if(SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_BACK, &CalibCoordCfg) == 0U) {
                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                } else {
                    SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("Invalid FileId", 0U, 0U, 0U, 0U, 0U);
            }
#endif
#if defined(CONFIG_ICAM_PROJECT_EMIRROR)
        } else if (0 == SvcWrap_strcmp("bsd", pArgVector[1U])) {
            UINT32 Divisor = 0U;

            if (0 == SvcWrap_strcmp("can", pArgVector[2U])) {
                if (0 == SvcWrap_strcmp("0", pArgVector[3U])) {
                    SvcBsdTask_SetBsdWithClipCanBus(FALSE);
                }
            }

            if (0 == SvcWrap_strcmp("line", pArgVector[2U])) {
                SvcBsdTask_DisplayLine();
            }

            if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                if(ArgCount == 4U){
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Divisor)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed", 0U, 0U);
                    }
                }
                SvcBsdTask_SetBsdChan(BSD_LEFT, Divisor);
                IsReboot = 1U;
            } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])) {
                if(ArgCount == 4U){
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Divisor)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed", 0U, 0U);
                    }
                }
                SvcBsdTask_SetBsdChan(BSD_RIGHT, Divisor);
                IsReboot = 1U;
            } else if (0 == SvcWrap_strcmp("both", pArgVector[2U])) {
                if(ArgCount == 4U){
                    if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Divisor)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed", 0U, 0U);
                    }
                }
                SvcBsdTask_SetBsdChan(BSD_BOTH, Divisor);
                IsReboot = 1U;
#if defined(CONFIG_ICAM_PLAYBACK_USED)
            } else if (0 == SvcWrap_strcmp("loop", pArgVector[2U])) {
                UINT32 FileIdx = 1U;
                SvcPlaybackTask_LoopPbkStart(1U, &FileIdx, PBK_LOOP_FLOOP | PBK_LOOP_SEAMLESS);
#endif
            } else {
                //SvcBsdTask_SetBsdChan(BSD_BOTH, Divisor);
            }
        } else if (0 == SvcWrap_strcmp("rmg", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                Rval = SvcRmgTask_Start();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcRmgTask_Start() failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])) {
                Rval = SvcRmgTask_Stop();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcRmgTask_Stop() failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                UINT32 Level;
                Rval = SvcWrap_strtoul(pArgVector[3U], &Level);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcRmgTask_SetDebugBbx(Level);
            } else if (0 == SvcWrap_strcmp("osd", pArgVector[2U])) {
                UINT32 Enable;
                Rval = SvcWrap_strtoul(pArgVector[3U], &Enable);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcRmgTask_OsdEnable(Enable);
#if defined(CONFIG_ICAM_PLAYBACK_USED)
            } else if (0 == SvcWrap_strcmp("loop", pArgVector[2U])) {
                UINT32 FileIdx = 1U;
                SvcPlaybackTask_LoopPbkStart(1U, &FileIdx, PBK_LOOP_FLOOP | PBK_LOOP_SEAMLESS);
#endif
            } else {
                SvcLog_OK(SVC_LOG_CMDAPP, "svc_app rmg [start/stop/loop/debug/osd]", 0U, 0U);
            }
#endif
        } else if (0 == SvcWrap_strcmp("can", pArgVector[1U])) {
            extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;

            if (0 == SvcWrap_strcmp("light", pArgVector[2U])) {
                if (0 == SvcWrap_strcmp("n", pArgVector[3U])) {
                    CanbusRawData.CANBusTurnLightStatus = 0;
                } else if (0 == SvcWrap_strcmp("l", pArgVector[3U])) {
                    CanbusRawData.CANBusTurnLightStatus = 1;
                } else if (0 == SvcWrap_strcmp("r", pArgVector[3U])) {
                    CanbusRawData.CANBusTurnLightStatus = 2;
                } else {
                    //pass vcast
                }
            } else if (0 == SvcWrap_strcmp("gear", pArgVector[2U])) {
                if (0 == SvcWrap_strcmp("p", pArgVector[3U])) {
                    CanbusRawData.CANBusGearStatus = AMBA_SR_CANBUS_GEAR_P;
                } else if (0 == SvcWrap_strcmp("r", pArgVector[3U])) {
                    CanbusRawData.CANBusGearStatus = AMBA_SR_CANBUS_GEAR_R;
                } else if (0 == SvcWrap_strcmp("n", pArgVector[3U])) {
                    CanbusRawData.CANBusGearStatus = AMBA_SR_CANBUS_GEAR_N;
                } else if (0 == SvcWrap_strcmp("d", pArgVector[3U])) {
                    CanbusRawData.CANBusGearStatus = AMBA_SR_CANBUS_GEAR_D;
                } else {
                    //pass vcast
                }
#if 0//defined(CONFIG_BUILD_AMBA_ADAS)
            } else if (0 == SvcWrap_strcmp("fakespeed", pArgVector[2U])) {
                extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
                DOUBLE CanSpeed;
                SVC_DIRTDT_GET_SPEED  pfnGetCanSpeed = SvcDirtDetect_GetCanSpeed;

                if (pfnGetCanSpeed != NULL) {
                    CanbusRawData.RawSpeed = 11333U;//85km/h
                    pfnGetCanSpeed(&CanSpeed);
                }
#endif
            } else if (0 == SvcWrap_strcmp("speed", pArgVector[2U])) {
                extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
                UINT32 FixedSpeed;

                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &FixedSpeed)) {
                    DOUBLE CanSpeed;

                    CanSpeed = (DOUBLE)FixedSpeed /(0.0073725);
                    CanbusRawData.RawSpeed = (UINT32)CanSpeed + 1U;
                    CanbusRawData.FlagValidRawData = 1U;

                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    }
                    pSvcUserPref->CanSpeed = FixedSpeed;
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                    }

                    IsReboot = 1U;
                }
            } else {
                //pass vcast
            }
        } else if (0 == SvcWrap_strcmp("ODDraw", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                Rval = SvcODDrawTask_Start();
            } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                Rval = SvcODDrawTask_Stop();
            } else {
                SvcLog_NG(SVC_LOG_CMDAPP, "use 'svc_app ODDraw [on/off]' to turn on/off OD draw task", 0U, 0U);
            }
#if defined(CONFIG_ICAM_PROJECT_EMIRROR) || defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
        } else if (0 == SvcWrap_strcmp("emr_cal_yuv" , pArgVector[1U])) {
            UINT8 Source;
            UINT8 Fov;

            if (0 == SvcWrap_strcmp("sd" , pArgVector[3U])) {
                Source = 0U;
            } else {
                Source = 1U;
            }

            if (0 == SvcWrap_strcmp("rear" , pArgVector[2U])) {
                Fov = rear;
            } else if(0 == SvcWrap_strcmp("left" , pArgVector[2U])) {
                Fov = left;
            } else if(0 == SvcWrap_strcmp("right" , pArgVector[2U])) {
                Fov = right;
            } else {
                Fov = none;
                SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
            }

            if(Fov != none) {
                EmrQuickCal = ENABLE;
                SvcCalib_SearchRectangleFromYuv(Source, Fov);
            }
#endif
#endif
#if defined(CONFIG_ICAM_STLCAP_USED)
#ifndef CONFIG_SOC_H22
        } else if (0 == SvcWrap_strcmp("rawencctx", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 CeNeeded = 0U, IsHiso = 0U, IkCtxId = 0U;
                Rval |= SvcWrap_strtoul(pArgVector[2U], &CeNeeded);
                Rval |= SvcWrap_strtoul(pArgVector[3U], &IsHiso);
                Rval |= SvcStillTask_GetIkCtxId((UINT8)IsHiso, (UINT8)CeNeeded, &IkCtxId);
                SvcLog_OK(SVC_LOG_CMDAPP, "IsHiso %u CeNeeded %u", IsHiso, CeNeeded);
                SvcLog_OK(SVC_LOG_CMDAPP, "IkCtxId %u", IkCtxId, 0);
            }
        } else if (0 == SvcWrap_strcmp("rawenccfg", pArgVector[1U])) {
            if (3U < ArgCount) {
                AMBA_DSP_RAW_BUF_s RawInfo;
                AMBA_DSP_BUF_s CeInfo;
                UINT32 CeNeeded = 0U, IsHiso = 0U, AlignOutH = 0U;
                Rval |= SvcWrap_strtoul(pArgVector[2U], &CeNeeded);
                Rval |= SvcWrap_strtoul(pArgVector[3U], &IsHiso);
                Rval |= SvcWrap_strtoul(pArgVector[4U], &AlignOutH);
                if (CeNeeded == 0U) {
                    Rval |= SvcStillTask_GetCapBuffer(&RawInfo, NULL, (UINT8)IsHiso, (UINT8)AlignOutH);
                } else {
                    Rval |= SvcStillTask_GetCapBuffer(&RawInfo, &CeInfo, (UINT8)IsHiso, (UINT8)AlignOutH);
                }
            }
        } else if (0 == SvcWrap_strcmp("rawenc", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 SaveYUV = 0U, SaveJPEG = 0U;
                SVC_STL_RAWENC_CTRL_s Ctrl;
                AMBA_DSP_YUV_IMG_BUF_s YuvInfo;
                AMBA_DSP_ENC_PIC_RDY_s JpgInfo;
                AmbaSvcWrap_MisraMemset(&YuvInfo, 0, sizeof(YuvInfo));
                AmbaSvcWrap_MisraMemset(&JpgInfo, 0, sizeof(JpgInfo));
                Rval |= SvcWrap_strtoul(pArgVector[2U], &SaveYUV);
                Rval |= SvcWrap_strtoul(pArgVector[3U], &SaveJPEG);
                SvcLog_OK(SVC_LOG_CMDAPP, "SaveYUV %u SaveJPEG %u", SaveYUV, SaveJPEG);
                Ctrl.SaveYUV = (UINT8)SaveYUV;
                Ctrl.SaveJPEG = (UINT8)SaveJPEG;
                Rval |= SvcStillTask_RawEncode(&Ctrl, &YuvInfo, &JpgInfo);
                SvcStillLog("[CAPP] BaseAddr %p AuxDataBuf %p", YuvInfo.BaseAddrY, YuvInfo.BaseAddrUV, 0, 0, 0);
                SvcLog_OK(SVC_LOG_CMDAPP, " pitch %u fmt %u", YuvInfo.Pitch, YuvInfo.DataFmt);
                SvcLog_OK(SVC_LOG_CMDAPP, " w %u h %u", YuvInfo.Window.Width, YuvInfo.Window.Height);
                SvcStillLog("[CAPP] Jpeg buffer %p", JpgInfo.StartAddr, 0, 0, 0, 0);
                SvcLog_OK(SVC_LOG_CMDAPP, "Jpeg size %u", JpgInfo.PicSize, 0);
            }
#endif
        } else if (0 == SvcWrap_strcmp("yuvcap", pArgVector[1U])) {
            Rval = CapTest(ArgCount, pArgVector);
        } else if (0 == SvcWrap_strcmp("cap", pArgVector[1U])) {
            Rval = CapTest(ArgCount, pArgVector);
        } else if (0 == SvcWrap_strcmp("stltest", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 TestFlag;
                Rval = SvcWrap_strtoul(pArgVector[2U], &TestFlag);
                if (Rval == OK) {
                    SvcStillProc_SetTestFlag((UINT8)TestFlag);
                }
            }
        } else if (0 == SvcWrap_strcmp("capout", pArgVector[1U])) {
            if (5U < ArgCount) {
                UINT32 SetOutputSize = 0U, StreamID = 0U, Width = 0U, Height = 0U;
                Rval = SvcWrap_strtoul(pArgVector[2U], &SetOutputSize);
                Rval |= SvcWrap_strtoul(pArgVector[3U], &StreamID);
                Rval |= SvcWrap_strtoul(pArgVector[4U], &Width);
                Rval |= SvcWrap_strtoul(pArgVector[5U], &Height);
                SvcStillCap_SetOutputSize((UINT8)SetOutputSize, (UINT16)StreamID, (UINT16)Width, (UINT16)Height);
            }
        } else if (0 == SvcWrap_strcmp("stilldbg", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    SvcStillCap_Debug(1);
                    SvcStillProc_Debug(1);
                    SvcStillEnc_Debug(1);
                    SvcStillMux_Debug(1);
                } else if (0 == SvcWrap_strcmp("idsp", pArgVector[2U])) {
                    SvcStillProc_Debug(3);
                } else {
                    SvcStillCap_Debug(0);
                    SvcStillProc_Debug(0);
                    SvcStillEnc_Debug(0);
                    SvcStillMux_Debug(0);
                }
            }
        } else if (0 == SvcWrap_strcmp("cappipe", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 FixPipe = 0U, FixHiso = 0U;
                Rval = SvcWrap_strtoul(pArgVector[2U], &FixPipe);
                Rval |= SvcWrap_strtoul(pArgVector[3U], &FixHiso);
                SvcStillProc_SetPipe((UINT8)FixPipe, (UINT8)FixHiso);
            }
#endif
#if defined(CONFIG_BUILD_CV)
        } else if (0 == SvcWrap_strcmp("pyramid_save", pArgVector[1U])) {
            if (5U < ArgCount) {
                UINT32 FovIdx = 0U, Hier = 0U;
                SVC_PYRAMID_CP_INFO_s Info = {0};
                const char *pFileNameY, *pFileNameUV;
                AMBA_FS_FILE *pFileY, *pFileUV;
                UINT32 NumSuccess = 0U;

                Rval = SvcWrap_strtoul(pArgVector[2U], &FovIdx);
                Rval = SvcWrap_strtoul(pArgVector[3U], &Hier);
                pFileNameY  = pArgVector[4U];
                pFileNameUV = pArgVector[5U];

                Rval = SvcPyramid_Save(FovIdx, Hier);
                if (Rval == SVC_OK) {
                    Rval = SvcPyramid_InfoGet(&Info);
                    if (SVC_OK == Rval) {
                        /* Write Y */
                        if ((pFileNameY != NULL) && (Info.pYAddr != NULL)) {
                            Rval = AmbaFS_FileOpen(pFileNameY, "w+", &pFileY);
                            if (SVC_OK == Rval) {
                                Rval = AmbaFS_FileWrite(Info.pYAddr, Info.YWidth * Info.YHeight, 1U, pFileY, &NumSuccess);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileY Write failed %d NumSuccess = %d", Rval, NumSuccess);
                                }
                                Rval = AmbaFS_FileClose(pFileY);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileY close failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDAPP, "FileY open failed %d", Rval, 0U);
                            }
                        }

                        /* Write UV */
                        if ((pFileNameUV != NULL) && (Info.pUVAddr != NULL)) {
                            Rval = AmbaFS_FileOpen(pFileNameUV, "w+", &pFileUV);
                            if (SVC_OK == Rval) {
                                Rval = AmbaFS_FileWrite(Info.pUVAddr, Info.UVWidth * Info.UVHeight, 1U, pFileUV, &NumSuccess);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileUV Write failed %d NumSuccess = %d", Rval, NumSuccess);
                                }
                                Rval = AmbaFS_FileClose(pFileUV);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileUV close failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDAPP, "FileUV open failed %d", Rval, 0U);
                            }
                        }
                    }
                }
                Rval = SVC_OK;
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("conv", pArgVector[1U])) {
            if (3U < ArgCount) {
                const char *pFileNameYIn = NULL, *pFileNameYOut = NULL;
                AMBA_FS_FILE *pFileY = NULL;
                ULONG  BufBase = 0U;
                UINT32 BufSize = 0U;
                UINT8  *pPtr = NULL;
                UINT32 NumSuccess = 0U, SrcWidth = 0U, SrcHeight = 0U, SrcPitch = 0U, DstPicth = 0U;

                if (0 == SvcWrap_strcmp("12bto8b", pArgVector[2U])) {
                    SVC_PYRAMID_CP_INFO_s Info = {0};
                    extern UINT32 SvcCvImgUtil_12bCompactTo8b(UINT8 *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, UINT32 SrcPitch, UINT8 *pDstBuf, UINT32 DstPitch);

                    if (4U < ArgCount) {
                        if (0 == SvcWrap_strcmp("-pyramid", pArgVector[3U])) {

                            Rval = SvcPyramid_InfoGet(&Info);
                            if (SVC_OK == Rval) {
                                pFileNameYOut = pArgVector[4U];
                                SrcWidth  = Info.YWidth;
                                SrcHeight = Info.YHeight;
                                SrcPitch  = Info.YWidth;
                                AmbaMisra_TypeCast(&BufBase, &Info.pYAddr);
                            }
                        } else if (7U < ArgCount) {
                            pFileNameYIn = pArgVector[3U];
                            Rval = SvcWrap_strtoul(pArgVector[4U], &SrcWidth);
                            Rval = SvcWrap_strtoul(pArgVector[5U], &SrcHeight);
                            Rval = SvcWrap_strtoul(pArgVector[6U], &SrcPitch);
                            pFileNameYOut = pArgVector[7U];

                            Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &BufBase, &BufSize);
                            if (SVC_OK == Rval) {
                                Rval = AmbaFS_FileOpen(pFileNameYIn, "r", &pFileY);
                                if (SVC_OK == Rval) {
                                    AmbaMisra_TypeCast(&pPtr, &BufBase);
                                    Rval = AmbaFS_FileRead(pPtr, SrcPitch * SrcHeight, 1U, pFileY, &NumSuccess);
                                    if (SVC_OK != Rval) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "FileY read failed %d NumSuccess = %d", Rval, NumSuccess);
                                    }
                                    Rval = AmbaFS_FileClose(pFileY);
                                    if (SVC_OK != Rval) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "FileY close failed %d", Rval, 0U);
                                    }
                                } else {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileY open failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request failed", Rval, 0U);
                            }
                        } else {
                            Rval = SVC_NG;
                        }

                        if (SVC_OK == Rval) {
                            DstPicth  = ((SrcPitch / 3U) << 1U);
                            AmbaMisra_TypeCast(&pPtr, &BufBase);
                            Rval = SvcCvImgUtil_12bCompactTo8b(pPtr, SrcWidth, SrcHeight, SrcPitch, pPtr, DstPicth);
                            if (SVC_OK == Rval) {
                                Rval = AmbaFS_FileOpen(pFileNameYOut, "w+", &pFileY);
                                if (SVC_OK == Rval) {
                                    Rval = AmbaFS_FileWrite(pPtr, DstPicth * SrcHeight, 1U, pFileY, &NumSuccess);
                                    if (SVC_OK != Rval) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "FileY Write failed %d NumSuccess = %d", Rval, NumSuccess);
                                    }
                                    Rval = AmbaFS_FileClose(pFileY);
                                    if (SVC_OK != Rval) {
                                        SvcLog_NG(SVC_LOG_CMDAPP, "FileY close failed %d", Rval, 0U);
                                    }
                                } else {
                                    SvcLog_NG(SVC_LOG_CMDAPP, "FileY open failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDAPP, "12bCompactTo8b failed %d", Rval, 0U);
                            }

                            if (SVC_OK == Rval) {
                                SvcLog_OK(SVC_LOG_CMDAPP, "Convert 12bto8b done", Rval, 0U);
                            }
                        }
                    } else {
                        Rval = SVC_NG;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "Unsupported conversion", 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
#endif
#if defined(CONFIG_ICAM_HDMI_EXIST)
        } else if (0 == SvcWrap_strcmp("hdmi_res", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("1080p30", pArgVector[2U])) {
                SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
                UINT32 i;
                UINT32 ResSet = SVC_NG;

                for (i = 0U; i < pResCfg->DispNum; i++) {
                    if ((pResCfg->DispBits & ((UINT32) 1U << i)) > 0U) {
                        if (pResCfg->DispStrm[i].VoutID == VOUT_IDX_B) {
                            if (pResCfg->DispStrm[i].pDriver == &AmbaFPD_HDMI_Obj) {
                                pResCfg->DispStrm[i].DevMode = HDMI_VIC_1080P30;
                                pResCfg->DispStrm[i].FrameRate.TimeScale = 30000U;
                                pResCfg->DispStrm[i].FrameRate.NumUnitsInTick = 1001U;
                                ResSet = SVC_OK;
                                break;
                            }
                        }
                    }
                }
                if (SVC_OK != ResSet) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "hdmi_res 1080p30 failed, driver may be not HDMI.", 0U, 0U);
                }
                Rval = SVC_OK;
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("hdmi_diag", pArgVector[1U])) {
            AMBA_HDMI_SINK_INFO_s SinkInfo = {0};
            UINT32 HpdState = 0U, EdidDetect = 0U;

            if (SVC_OK != AmbaHDMI_TxGetSinkInfo(AMBA_HDMI_TX_PORT0, &HpdState, &EdidDetect, &SinkInfo)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaHDMI_TxGetSinkInfo failed", 0U, 0U);
            }

            for (UINT32 i = 0U; i < SinkInfo.NumVideoInfo; i++) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SinkInfo VideoInfoBuf[%lu]:%lu", i, (SinkInfo.VideoInfoBuf[i] & SVC_HDMI_VIC_MASK));
            }

            Rval = SVC_OK;
        } else if (0 == SvcWrap_strcmp("hdmi", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                Rval = AmbaHDMI_TxStart(AMBA_HDMI_TX_PORT0);

                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## fail to start hdmi tx, rval(%u)", Rval, 0U);
                }
            }

            if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                Rval = AmbaHDMI_TxStop(AMBA_HDMI_TX_PORT0);

                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## fail to stop hdmi tx, rval(%u)", Rval, 0U);
                }
            }

            Rval = SVC_OK;
#endif
#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
        } else if (0 == SvcWrap_strcmp("show_distance", pArgVector[1U])) {
            UINT32 ShwDisRev;
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                ShwDisRev = SvcODDrawTask_Ctrl("show_distance", 1U);
                SvcLog_OK(SVC_LOG_CMDAPP, "Show OD distance info, ShwDisRev = %d", ShwDisRev, 0U);
            } else {
                ShwDisRev = SvcODDrawTask_Ctrl("show_distance", 0U);
                SvcLog_OK(SVC_LOG_CMDAPP, "Do not show OD distance info, ShwDisRev = %d", ShwDisRev, 0U);
            }
        } else if (0 == SvcWrap_strcmp("od_info", pArgVector[1U])) {
            if ((0 == SvcWrap_strcmp("on", pArgVector[2U])) && ((0U == OD_Info)||(1U == AdasEnable))) {


                /* Need to return calibration version or don't launch FC */
                if (SVC_OK == SvcAdasNotify_CalibCheck()) {

                    Rval = SvcODDrawTask_Enable(0U);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcODDrawTask_Enable() (%u)", Rval, 0U);
                    }

                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        Rval = SvcFcTaskV2_Start();
                    } else {
                        Rval = SvcFcTask_Start();
                    }
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcFcTask_Start() (%u)", Rval, 0U);
                    }
                    if (1U == AdasEnable) {
                        SvcFcTask_WarningIcanEnable();
                        SvcAdasNotify_DrawFcMode(1U);

                        if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                            Rval = SvcLdwsTaskV2_Start();
                        } else {
                            Rval = SvcLdwsTask_Start();
                        }
                        if (Rval != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcLdwsTask_Start() (%u)", Rval, 0U);
                        }
                    }
                    OD_Info = 1U;
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## No calibration data!", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("off", pArgVector[2U])) && ((1U == OD_Info)||(1U == AdasEnable))) {

                if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                    Rval = SvcFcTaskV2_Stop();
                } else {
                    Rval = SvcFcTask_Stop();
                }


                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcFcTask_Stop() (%u)", Rval, 0U);
                }

                Rval = SvcODDrawTask_Enable(1U);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcODDrawTask_Enable() (%u)", Rval, 0U);
                }

                /* Disable WarningIcon display */
                SvcFcTask_WarningIcanDisable();

                /* Disable DrawFcMode */
                SvcAdasNotify_DrawFcMode(0U);

                if (1U == AdasEnable) {
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        Rval = SvcLdwsTaskV2_Stop();
                    } else {
                        Rval = SvcLdwsTask_Stop();
                    }
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcLdwsTask_Stop() (%u)", Rval, 0U);
                    }
                    AdasEnable = 0U;
                }

                OD_Info = 0U;
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "Something wrong for od_info cmd. OD_Info = %d, AdasEnable = %d", OD_Info, AdasEnable);
                }
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        } else if (0 == SvcWrap_strcmp("adas_cal_load_V1", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT8 Misra_0;
                //UINT32 Rval;
                UINT32 SkipFlow = 0U;
                AMBA_CT_INITIAL_CONFIG_s InitCfg;
                static UINT32 EmirWorkBufSize = 0U;
                static ULONG  Emir3in3TunerWorkBuf = 0U;
                static void *pEmir3in3TunerWorkBuf = NULL;
                //static void *pCalPointer;

                static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_EM_SV_CFG_V1_s Emirror3in3WarpConfigV1 GNU_SECTION_NOZEROINIT;

//                static AMBA_CAL_EM_SV_DATA_s CalWarpOutputData;
                static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCoordCfgV1 GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalOutputV1 GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;
                const char ADASCalibFileName[30] = ":\\adas_tuner_front.txt";
                char ADASscrript[30];

                if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed", 0U, 0U);
                }
                if (SVC_OK != AmbaWrap_memset(&InitCfg, 0, sizeof(InitCfg))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset InitCfg failed", 0U, 0U);
                }
                InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                AmbaMisra_TypeCast(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                if (SVC_OK != AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init AMBA_CT_TYPE_EMIRROR failed", 0U, 0U);
                }

                if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                }
                if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get failed", 0U, 0U);
                }


                ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                if (0 == SvcWrap_strcmp("front", pArgVector[2U])) {
                    Rval = AmbaCT_Load(ADASscrript);
                    if(Rval != 0U) {
                        AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        SkipFlow = 1U;
                    } else {
                        AmbaPrint_PrintStr5("AmbaCT_Load %s OK!", ADASscrript, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintUInt5("Invalid FileId", 0U, 0U, 0U, 0U, 0U);
                    SkipFlow = 1U;
                }


                if(SkipFlow == 0U) {

                    Rval |= AmbaWrap_memset(&LensData, 0, sizeof(LensData));
                    Rval |= AmbaWrap_memset(&CameraData, 0, sizeof(CameraData));
                    Rval |= AmbaWrap_memset(&AspPointMap, 0, sizeof(AspPointMap));
                    Rval |= AmbaWrap_memset(&CalibPointData, 0, sizeof(CalibPointData));
                    Rval |= AmbaWrap_memset(&AssistancePointData, 0, sizeof(AssistancePointData));
                    Rval |= AmbaWrap_memset(&Emirror3in3WarpConfigV1, 0, sizeof(Emirror3in3WarpConfigV1));
                    Rval |= AmbaWrap_memset(&Aspheric, 0, sizeof(Aspheric));
                    Rval |= AmbaWrap_memset(&CurvedSurface, 0, sizeof(CurvedSurface));
                    Rval |= AmbaWrap_memset(&CalibCoordCfgV1, 0, sizeof(CalibCoordCfgV1));
                    Rval |= AmbaWrap_memset(&CalOutputV1, 0, sizeof(AMBA_CAL_EM_CALIB_INFO_DATA_V1_s));
                    Rval |= AmbaWrap_memset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
                    Rval |= AmbaWrap_memset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
                    Rval |= AmbaWrap_memset(&LensSpecFormula, 0, sizeof(LensSpecFormula));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset in SkipFlow failed", 0U, 0U);
                    }

                    AmbaCT_EmGetLens(0U, &LensData);
                    if (SVC_OK != RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfigV1.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_EmFeedLensSpec failed", 0U, 0U);
                    }

                    AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfigV1.Cam.Sensor);
                    AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfigV1.Cam.OpticalCenter);
                    AmbaCT_EmGetCamera(0U, &CameraData);
                    Emirror3in3WarpConfigV1.Cam.Pos.X = CameraData.PositionX;
                    Emirror3in3WarpConfigV1.Cam.Pos.Y = CameraData.PositionY;
                    Emirror3in3WarpConfigV1.Cam.Pos.Z = CameraData.PositionZ;



                    Emirror3in3WarpConfigV1.Cam.Rotation = CameraData.RotateType;
                    AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfigV1.VinSensorGeo);
                    AmbaCT_EmGetSingleView(&Emirror3in3WarpConfigV1.View.Type);
//                    Emirror3in3WarpConfigV1.View.Cfg.pKeepLDC = NULL;
//                    Emirror3in3WarpConfigV1.View.Cfg.pKeepAspheric = NULL;
                    if (Emirror3in3WarpConfigV1.View.Type == AMBA_CAL_EM_LDC) {
                        AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
                    } else if (Emirror3in3WarpConfigV1.View.Type == AMBA_CAL_EM_ASPHERIC) {
                        Emirror3in3WarpConfigV1.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfigV1.View.Cfg.pAspheric);
                        if (Rval != 0U) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfigV1.View.Type, 0U, 0U, 0U, 0U);
                        }
                    } else if (Emirror3in3WarpConfigV1.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfigV1.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else if (Emirror3in3WarpConfigV1.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
                        Emirror3in3WarpConfigV1.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfigV1.View.Cfg.pAspheric);
                        if (Rval != 0U) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfigV1.View.Type, 0U, 0U, 0U, 0U);
                        }
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfigV1.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else {
                        AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
                    }
                    AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfigV1.View.VoutArea);
                    Emirror3in3WarpConfigV1.View.PlugIn.PlugInMode = 0U; // TODO:
                    //Emirror3in3WarpConfigV1.View.PlugIn.MsgReciver = RefCalib_EmSvCbMsgReciver;


                    AmbaCT_EmGetCalibPoints(0U, &CalibPointData);
                    AmbaCT_EmGetAssistancePoints(0U, &AssistancePointData);


                    if (SVC_OK != AmbaWrap_memset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmirrorPointMap failed", 0U, 0U);
                    }
                    RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
                    Emirror3in3WarpConfigV1.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
                    Emirror3in3WarpConfigV1.Calibinfo.p4Point = &EmirrorPointMap;
                    Emirror3in3WarpConfigV1.Calibinfo.pMultiPoint = NULL;
                    AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfigV1.Tile);
                    AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfigV1.ROI);
                    Misra_0 = (UINT8)Emirror3in3WarpConfigV1.OptimizeLevel;
                    AmbaCT_EmGetOptimize(0U, &Misra_0);

                    AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfigV1.InternalCfg);

                    FocalLength = pSvcUserPref->CalibFocalLength;
                    if (SVC_OK != RefCalib_ConvImgToWorldPlaneV1(Emirror3in3WarpConfigV1, &CalibCoordCfgV1, &CalOutputV1, FocalLength)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_ConvImgToWorldPlane failed", 0U, 0U);
                    }
                    RefCalib_ShowCalibCoordOutputV1(&CalibCoordCfgV1);


                    if (SVC_OK != SvcCalib_AdasCfgSetV1(SVC_CALIB_ADAS_TYPE_FRONT, &CalibCoordCfgV1, &CalOutputV1)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_FRONT failed", 0U, 0U);
                    }
                }
            }
#endif
        } else if (0 == SvcWrap_strcmp("adas_cal_get_V1", pArgVector[1U])) {

            static SVC_CALIB_ADAS_INFO_GET_s AdasInfo GNU_SECTION_NOZEROINIT;
            static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
            static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface;

            AdasInfo.AdasCfgV1.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
            AdasInfo.AdasCfgV1.TransCfg.pCurvedSurface = &CurvedSurface;

            if (SVC_OK != SvcCalib_AdasCfgGetV1(SVC_CALIB_ADAS_TYPE_FRONT, &AdasInfo)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_FRONT failed", 0U, 0U);
            }


        } else if (0 == SvcWrap_strcmp("adas_cal_load", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT8 Misra_0;
                //UINT32 Rval;
                UINT32 SkipFlow = 0U;
                AMBA_CT_INITIAL_CONFIG_s InitCfg;
                static UINT32 EmirWorkBufSize = 0U;
                static ULONG  Emir3in3TunerWorkBuf = 0U;
                static void *pEmir3in3TunerWorkBuf = NULL;
                //static void *pCalPointer;

                static AMBA_CT_EM_LENS_s LensData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CAMERA_s CameraData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASP_POINT_MAP_s AspPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_CALIB_POINTS_s CalibPointData GNU_SECTION_NOZEROINIT;
                static AMBA_CT_EM_ASSISTANCE_POINTS_s AssistancePointData GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_EM_SV_CFG_s Emirror3in3WarpConfig GNU_SECTION_NOZEROINIT;

//                static AMBA_CAL_EM_SV_DATA_s CalWarpOutputData;
                static AMBA_CAL_EM_ASP_VIEW_CFG_s Aspheric GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_POINT_MAP_s EmirrorPointMap GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_EM_CALC_COORD_CFG_s CalibCoordCfg GNU_SECTION_NOZEROINIT;

                static AMBA_CAL_LENS_DST_REAL_EXPECT_s LensSpecRealExpect GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_ANGLE_s LensSpecAngle GNU_SECTION_NOZEROINIT;
                static AMBA_CAL_LENS_DST_FORMULA_s LensSpecFormula GNU_SECTION_NOZEROINIT;
                const char ADASCalibFileName[30] = ":\\adas_tuner_front.txt";
                char ADASscrript[30];

                if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed", 0U, 0U);
                }
                if (SVC_OK != AmbaWrap_memset(&InitCfg, 0, sizeof(InitCfg))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset InitCfg failed", 0U, 0U);
                }
                InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                AmbaMisra_TypeCast(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                if (SVC_OK != AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init AMBA_CT_TYPE_EMIRROR failed", 0U, 0U);
                }

                if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                }
                if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get failed", 0U, 0U);
                }
                ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                if (0 == SvcWrap_strcmp("front", pArgVector[2U])) {
                    Rval = AmbaCT_Load(ADASscrript);
                    if(Rval != 0U) {
                        AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        SkipFlow = 1U;
                    } else {
                        AmbaPrint_PrintStr5("AmbaCT_Load %s OK!", ADASscrript, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_PrintUInt5("Invalid FileId", 0U, 0U, 0U, 0U, 0U);
                    SkipFlow = 1U;
                }

                if(SkipFlow == 0U) {

                    Rval |= AmbaWrap_memset(&LensData, 0, sizeof(LensData));
                    Rval |= AmbaWrap_memset(&CameraData, 0, sizeof(CameraData));
                    Rval |= AmbaWrap_memset(&AspPointMap, 0, sizeof(AspPointMap));
                    Rval |= AmbaWrap_memset(&CalibPointData, 0, sizeof(CalibPointData));
                    Rval |= AmbaWrap_memset(&AssistancePointData, 0, sizeof(AssistancePointData));
                    Rval |= AmbaWrap_memset(&Emirror3in3WarpConfig, 0, sizeof(Emirror3in3WarpConfig));
                    Rval |= AmbaWrap_memset(&Aspheric, 0, sizeof(Aspheric));
                    Rval |= AmbaWrap_memset(&CurvedSurface, 0, sizeof(CurvedSurface));
                    Rval |= AmbaWrap_memset(&CalibCoordCfg, 0, sizeof(CalibCoordCfg));
                    Rval |= AmbaWrap_memset(&LensSpecRealExpect, 0, sizeof(LensSpecRealExpect));
                    Rval |= AmbaWrap_memset(&LensSpecAngle, 0, sizeof(LensSpecAngle));
                    Rval |= AmbaWrap_memset(&LensSpecFormula, 0, sizeof(LensSpecFormula));
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset in SkipFlow failed", 0U, 0U);
                    }

                    AmbaCT_EmGetLens(0U, &LensData);
                    if (SVC_OK != RefCalib_EmFeedLensSpec(&LensData, &Emirror3in3WarpConfig.Cam.Lens, &LensSpecRealExpect, &LensSpecAngle, &LensSpecFormula)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_EmFeedLensSpec failed", 0U, 0U);
                    }
                    AmbaCT_EmGetSensor(0U, &Emirror3in3WarpConfig.Cam.Sensor);
                    AmbaCT_EmGetOpticalCenter(0U, &Emirror3in3WarpConfig.Cam.OpticalCenter);
                    AmbaCT_EmGetCamera(0U, &CameraData);
                    Emirror3in3WarpConfig.Cam.Pos.X = CameraData.PositionX;
                    Emirror3in3WarpConfig.Cam.Pos.Y = CameraData.PositionY;
                    Emirror3in3WarpConfig.Cam.Pos.Z = CameraData.PositionZ;
                    Emirror3in3WarpConfig.Cam.Rotation = CameraData.RotateType;
                    AmbaCT_EmGetVin(0U, &Emirror3in3WarpConfig.VinSensorGeo);
                    AmbaCT_EmGetSingleView(&Emirror3in3WarpConfig.View.Type);
                    if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_LDC) {
                        AmbaPrint_PrintStr5("Type : AMBA_CAL_EM_LDC!", NULL, NULL, NULL, NULL, NULL);
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_ASPHERIC) {
                        Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
                        if (Rval != 0U) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
                        }
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_SURFACE) {
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else if (Emirror3in3WarpConfig.View.Type == AMBA_CAL_EM_CURVED_ASPHERIC) {
                        Emirror3in3WarpConfig.View.Cfg.pAspheric = &Aspheric;
                        AmbaCT_EmGetSingleViewAspCfg(&AspPointMap);
                        Rval = RefCalib_EmFeedAsphericPointMap(&AspPointMap, Emirror3in3WarpConfig.View.Cfg.pAspheric);
                        if (Rval != 0U) {
                            AmbaPrint_PrintUInt5("RefCalib_EmFeedAsphericPointMap() failed! Type = %u", (UINT32)Emirror3in3WarpConfig.View.Type, 0U, 0U, 0U, 0U);
                        }
                        AmbaCT_EmGetSingleViewCurvedCfg(&CurvedSurface);
                        Emirror3in3WarpConfig.View.Cfg.pCurvedSurface = &CurvedSurface;
                    } else {
                        AmbaPrint_PrintUInt5("Wrong view type!", 0U, 0U, 0U, 0U, 0U);
                    }
                    AmbaCT_EmGetVout(0U, &Emirror3in3WarpConfig.View.VoutArea);
                    Emirror3in3WarpConfig.View.PlugIn.PlugInMode = 0U; // TODO:
                    //Emirror3in3WarpConfig.View.PlugIn.MsgReciver = RefCalib_EmSvCbMsgReciver;

                    AmbaCT_EmGetCalibPoints(0U, &CalibPointData);
                    AmbaCT_EmGetAssistancePoints(0U, &AssistancePointData);

                    if (SVC_OK != AmbaWrap_memset(&EmirrorPointMap, 0, sizeof(AMBA_CAL_EM_POINT_MAP_s))) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset EmirrorPointMap failed", 0U, 0U);
                    }
                    RefCalib_EmFeedPointMap(&CalibPointData, &AssistancePointData, &EmirrorPointMap);
                    Emirror3in3WarpConfig.Calibinfo.Type = AMBA_EM_CALIB_4_POINT;
                    Emirror3in3WarpConfig.Calibinfo.p4Point = &EmirrorPointMap;
                    Emirror3in3WarpConfig.Calibinfo.pMultiPoint = NULL;
                    AmbaCT_EmGetTileSize(0U, &Emirror3in3WarpConfig.Tile);
                    AmbaCT_EmGetRoi(0U, &Emirror3in3WarpConfig.ROI);
                    Misra_0 = (UINT8)Emirror3in3WarpConfig.OptimizeLevel;
                    AmbaCT_EmGetOptimize(0U, &Misra_0);
                    AmbaCT_EmGetInternal(0U, &Emirror3in3WarpConfig.InternalCfg);
//                    Rval = AmbaCal_EmGenSingleView(&Emirror3in3WarpConfig, pEmir3in3TunerWorkBuf, &CalWarpOutputData);
//                    if (Rval != 0U) {
//                        AmbaPrint_PrintStr5("%s, AmbaCal_EmGenSingleView() failed!", __func__, NULL, NULL, NULL, NULL);
//                    }
                    if (SVC_OK != RefCalib_ConvImgToWorldPlane(Emirror3in3WarpConfig, &CalibCoordCfg)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "RefCalib_ConvImgToWorldPlane failed", 0U, 0U);
                    }
                    RefCalib_ShowCalibCoordOutput(&CalibCoordCfg);
                    //RefCalib_ShowCalibWarpOutput(&CalWarpOutputData);

                    if (SVC_OK != SvcCalib_AdasCfgSet(SVC_CALIB_ADAS_TYPE_FRONT, &CalibCoordCfg)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcCalib_AdasCfgSet SVC_CALIB_ADAS_TYPE_FRONT failed", 0U, 0U);
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("adas", pArgVector[1U])) {
            if ((0 == SvcWrap_strcmp("on", pArgVector[2U])) && (0U == AdasEnable)) {
                UINT32  LdwsRval;


                if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                    if (SVC_OK != ParseAutoCalibScript()) {
//                        SvcLdwsTaskV2_EnableAutoCal(0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "V2 ParseAutoCalibScript execute have some problem, no need to do auto calibration.", 0U, 0U);
                    } else {
//                        SvcLdwsTaskV2_EnableAutoCal(1U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "V2 ParseAutoCalibScript execute OK!", 0U, 0U);
                    }
                } else {
                    if (SVC_OK != ParseAutoCalibScript()) {
//                        SvcLdwsTask_EnableAutoCal(0U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "ParseAutoCalibScript execute have some problem, no need to do auto calibration.", 0U, 0U);
                    } else {
//                        SvcLdwsTask_EnableAutoCal(1U);
                        SvcLog_OK(SVC_LOG_CMDAPP, "ParseAutoCalibScript execute OK!", 0U, 0U);
                    }
                }

                /* Need to return calibration version or don't launch FC */
                if (SVC_OK == SvcAdasNotify_CalibCheck()) {
//                    if(OD_Info == 0U){
//                        Rval = SvcODDrawTask_Enable(0U);
//                        if (Rval != SVC_OK) {
//                            SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcODDrawTask_Enable() (%u)", Rval, 0U);
//                        }
//                        if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
//                            Rval = SvcFcTaskV2_Start();
//                        } else {
//                            Rval = SvcFcTask_Start();
//                        }
//                        if (Rval != SVC_OK) {
//                            SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcFcTask_Start() (%u)", Rval, 0U);
//                        }
//                    }
                    SvcAdasNotify_DrawFcMode(1U);
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        LdwsRval = SvcLdwsTaskV2_Start();
                    } else {
                        LdwsRval = SvcLdwsTask_Start();
                    }
                    if (LdwsRval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcLdwsTask_Start() (%u)", LdwsRval, 0U);
                    } else {
                        SvcLog_OK(SVC_LOG_CMDAPP, "ADAS on", 0U, 0U);
                    }

                    /* Enable WarningIcon display */
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcFcTaskV2_WarningIcanEnable();
                    } else {
                        SvcFcTask_WarningIcanEnable();
                    }

                    /* Reset ACC RequiredSpeed and ACCAcceleration */
                    SvcAdasNotify_SetACCRequiredSpeed(0.0);
                    SvcAdasNotify_SetACCAcceleration(0.0);
                    AdasEnable = 1U;

                    /* Enable LKA if ADAS cmd on */
//                    {
//                        static char AdasLkaCmd[] = "svc_app adas lka on";
//
//                        Rval = AmbaShell_ExecCommand(AdasLkaCmd);
//                        AmbaMisra_TouchUnused(&Rval);
//                    }

                    /* Enable draw warning_zone if ADAS cmd on */
                    {
                        static char AdasLkaCmd[] = "svc_app adas warning_zone on";

                        Rval = AmbaShell_ExecCommand(AdasLkaCmd);
                        AmbaMisra_TouchUnused(&Rval);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## No calibration data!", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("off", pArgVector[2U])) && (1U == AdasEnable)) {
                if (SVC_OK == SvcAdasNotify_CalibCheck()) {

                    /* Disable WarningIcon display */
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcFcTaskV2_WarningIcanDisable();
                    } else {
                        SvcFcTask_WarningIcanDisable();
                    }

                    /* Disable DrawFcMode */
                    SvcAdasNotify_DrawFcMode(0U);
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        Rval = SvcLdwsTaskV2_Stop();
                    } else {
                        Rval = SvcLdwsTask_Stop();
                    }
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to do SvcLdwsTask_Stop() (%u)", Rval, 0U);
                    }

                    /* Reset ACC RequiredSpeed and ACCAcceleration*/
                    SvcAdasNotify_SetACCRequiredSpeed(-1.0);
                    SvcAdasNotify_SetACCAcceleration(9999.0);
                    AdasEnable = 0U;
                    SvcLog_OK(SVC_LOG_CMDAPP, "ADAS off", 0U, 0U);

                    /* Disable LKA if ADAS cmd off */
                    {
                        static char AdasLkaCmd[] = "svc_app adas lka off";

                        Rval = AmbaShell_ExecCommand(AdasLkaCmd);
                        AmbaMisra_TouchUnused(&Rval);
                    }

                    /* Disable draw warning_zone if ADAS cmd on */
                    {
                        static char AdasLkaCmd[] = "svc_app adas warning_zone off";

                        Rval = AmbaShell_ExecCommand(AdasLkaCmd);
                        AmbaMisra_TouchUnused(&Rval);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "## No calibration data!", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("warning_zone", pArgVector[2U]))) {
                static INT32 ZoneX[4], ZoneY[4];

                /* Enable draw WarningZone */
                SvcAdasNotify_WarningZoneEnable(1);
                if (0 == SvcWrap_strcmp("auto_tune", pArgVector[3U])) {
                    UINT32  OsdBufWidth, OsdBufHeight;
                    #if defined(CONFIG_ICAM_REBEL_USAGE)
                    Rval = SvcOsd_GetOsdBufSize(VOUT_IDX_A, &OsdBufWidth, &OsdBufHeight);
                    #else
                    Rval = SvcOsd_GetOsdBufSize(VOUT_IDX_B, &OsdBufWidth, &OsdBufHeight);
                    #endif
                    if(Rval != 0U) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcOsd_GetOsdBufSize failed!!", 0U, 0U);
                    }
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);

                    // Check Horizontal value
                    Horizontal = SvcAdasNotify_GetCalibHorizontal();
                    if (0U == Horizontal) {
                        Horizontal = HORIZONTAL_Y;
                    }

                    warning_zone_x_offset = (((INT32)OsdBufWidth/2) - ((ZoneX[1] + ZoneX[2])/2));
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 0U);
                    SvcAdasNotify_WarningZoneEnable(0U);

                    AmbaPrint_PrintInt5("WarningZone auto tune, x_offset = %d, OsdBufWidth/2U = %d",
                            warning_zone_x_offset, ((INT32)OsdBufWidth/2), 0, 0, 0);
                    AmbaPrint_PrintInt5("ZoneX[0] = %d, ZoneX[1] = %d, ZoneX[2] = %d, ZoneX[3] = %d",
                                         ZoneX[0], ZoneX[1], ZoneX[2], ZoneX[3], 0);

                    /* Adjust adas_calib_line (Horizontal line) */
                    {
                        UINT32  CurrentHorizonY = 0U;
                        AMBA_CT_INITIAL_CONFIG_s InitCfg;
                        static ULONG  Emir3in3TunerWorkBuf = 0U;
                        static UINT32 EmirWorkBufSize = 0U;
                        static void *pEmir3in3TunerWorkBuf = NULL;
                        const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                        char ADASscrript[30];


                        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                        } else {
                            DOUBLE FocalLengthPrint = pSvcUserPref->CalibFocalLength;
                            INT64  DoubleToINT64;
                            UINT32 CalibFocalLengthIn_nm = 0U;
                            FocalLengthPrint *= 1000000.0;/*mm to nm */
                            DoubleToINT64 = (INT64)(FocalLengthPrint);
                            AmbaMisra_TypeCast32(&CalibFocalLengthIn_nm, &DoubleToINT64);
                            FocalLength = pSvcUserPref->CalibFocalLength;
                            Horizontal  = pSvcUserPref->SkylineHeight;

                            SvcLog_OK(__func__, "SvcUserPref_Get()[Warning Zone Auto Tune] CalibFocalLength %d (nm), SkylineHeight %d",
                                                    CalibFocalLengthIn_nm,
                                                    pSvcUserPref->SkylineHeight);

                        }

                        CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                        SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
                        }
                        AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                        InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                        AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                        InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                        Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                        }
                        if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                        }
                        ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                        CmdAPP_strcat(ADASscrript, ADASCalibFileName);
                        Rval = AmbaCT_Load(ADASscrript);
                        if(Rval != 0U) {
                            AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        } else {
                            QuickCalib(Horizontal, FocalLength, warning_zone_x_offset);
                            IsReboot = 1U;
                        }
                    }
                    Rval = AmbaKAL_TaskSleep(1000);
                    if(Rval != 0U) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed!!", 0U, 0U);
                    }

                } else if (0 == SvcWrap_strcmp("auto_tune_V1", pArgVector[3U])) {
                    UINT32  OsdBufWidth = 0U, OsdBufHeight = 0U;
                    #if defined(CONFIG_ICAM_REBEL_USAGE)
                    Rval = SvcOsd_GetOsdBufSize(VOUT_IDX_A, &OsdBufWidth, &OsdBufHeight);
                    #else
                    Rval = SvcOsd_GetOsdBufSize(VOUT_IDX_B, &OsdBufWidth, &OsdBufHeight);
                    #endif
                    if(Rval != 0U) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcOsd_GetOsdBufSize failed!!", 0U, 0U);
                    }
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);

                    // Check Horizontal value
                    Horizontal = SvcAdasNotify_GetCalibHorizontal();
                    if (0U == Horizontal) {
                        Horizontal = HORIZONTAL_Y;
                    }

                    warning_zone_x_offset = (((INT32)OsdBufWidth/2) - ((ZoneX[1] + ZoneX[2])/2));
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 0U);
                    SvcAdasNotify_WarningZoneEnable(0U);

                    AmbaPrint_PrintInt5("WarningZone auto tune, x_offset = %d, OsdBufWidth/2U = %d",
                            warning_zone_x_offset, ((INT32)OsdBufWidth/2), 0, 0, 0);
                    AmbaPrint_PrintInt5("ZoneX[0] = %d, ZoneX[1] = %d, ZoneX[2] = %d, ZoneX[3] = %d",
                                         ZoneX[0], ZoneX[1], ZoneX[2], ZoneX[3], 0);

                    /* Adjust adas_calib_line (Horizontal line) */
                    {
                        UINT32  CurrentHorizonY = 0U;
                        AMBA_CT_INITIAL_CONFIG_s InitCfg;
                        static ULONG  Emir3in3TunerWorkBuf = 0U;
                        static UINT32 EmirWorkBufSize = 0U;
                        static void *pEmir3in3TunerWorkBuf = NULL;
                        const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                        char ADASscrript[30];

                        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                        } else {
                            DOUBLE FocalLengthPrint = pSvcUserPref->CalibFocalLength;
                            INT64  DoubleToINT64;
                            UINT32 CalibFocalLengthIn_nm = 0U;
                            FocalLengthPrint *= 1000000.0;/*mm to nm */
                            DoubleToINT64 = (INT64)(FocalLengthPrint);
                            AmbaMisra_TypeCast32(&CalibFocalLengthIn_nm, &DoubleToINT64);
                            FocalLength = pSvcUserPref->CalibFocalLength;
                            Horizontal  = pSvcUserPref->SkylineHeight;

                            SvcLog_OK(__func__, "SvcUserPref_Get()[Warning Zone Auto Tune] CalibFocalLength %d (nm), SkylineHeight %d",
                                                    CalibFocalLengthIn_nm,
                                                    pSvcUserPref->SkylineHeight);

                        }

                        CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                        SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
                        }
                        AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                        InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                        AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                        InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                        Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                        }
                        if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                        }
                        ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                        CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                        Rval = AmbaCT_Load(ADASscrript);
                        if(Rval != 0U) {
                            AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        } else {
                            QuickCalib_V1(Horizontal, FocalLength, warning_zone_x_offset);
                            IsReboot = 1U;
                        }
                    }
                    Rval = AmbaKAL_TaskSleep(1000);
                    if(Rval != 0U) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed!!", 0U, 0U);
                    }

                } else if (0 == SvcWrap_strcmp("left", pArgVector[3U])) {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    warning_zone_x_offset--;
                    for (INT32 i = 0; i<4; i++) {
                        ZoneX[i] -= 1;
                    }
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 1U);
                } else if (0 == SvcWrap_strcmp("right", pArgVector[3U])) {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    warning_zone_x_offset++;
                    for (INT32 i = 0; i<4; i++) {
                        ZoneX[i] += 1;
                    }
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 1U);
                } else if (0 == SvcWrap_strcmp("done", pArgVector[3U])) {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 0U);
                    SvcAdasNotify_WarningZoneEnable(0U);
                    /* Adjust adas_calib_line (Horizontal line) */
                    {
                        UINT32  CurrentHorizonY = 0U;
                        AMBA_CT_INITIAL_CONFIG_s InitCfg;
                        static ULONG  Emir3in3TunerWorkBuf = 0U;
                        static UINT32 EmirWorkBufSize = 0U;
                        static void *pEmir3in3TunerWorkBuf = NULL;
                        const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                        char ADASscrript[30];

                        CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                        SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
                        }
                        AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                        InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                        AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                        InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                        Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                        }

                        if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                        }
                        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                        }
                        ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                        CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                        Rval = AmbaCT_Load(ADASscrript);
                        if(Rval != 0U) {
                            AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        } else {
                            QuickCalib(Horizontal, FocalLength, warning_zone_x_offset);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("done_V1", pArgVector[3U])) {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 0U);
                    SvcAdasNotify_WarningZoneEnable(0U);
                    /* Adjust adas_calib_line (Horizontal line) */
                    {
                        UINT32  CurrentHorizonY = 0U;
                        AMBA_CT_INITIAL_CONFIG_s InitCfg;
                        static ULONG  Emir3in3TunerWorkBuf = 0U;
                        static UINT32 EmirWorkBufSize = 0U;
                        static void *pEmir3in3TunerWorkBuf = NULL;
                        const char ADASCalibFileName[30] = ":\\adas_tuner_front_quick.txt";
                        char ADASscrript[30];

                        CurrentHorizonY = SvcAdasNotify_SetCalibHorizontal(1U, Horizontal);
                        SvcLog_OK(SVC_LOG_CMDAPP, "CurrentHorizonY = %d", CurrentHorizonY, 0U);
                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_EMIRROR_CT, &Emir3in3TunerWorkBuf, &EmirWorkBufSize);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcBuffer_Request FMEM_ID_EMIRROR_CT failed!!", 0U, 0U);
                        }
                        AmbaSvcWrap_MisraMemset(&InitCfg, 0, sizeof(InitCfg));
                        InitCfg.TunerWorkingBufSize = EmirWorkBufSize;
                        AmbaMisra_TypeCast32(&pEmir3in3TunerWorkBuf, &Emir3in3TunerWorkBuf);
                        InitCfg.pTunerWorkingBuf = pEmir3in3TunerWorkBuf;
                        Rval = AmbaCT_Init(AMBA_CT_TYPE_EMIRROR, &InitCfg);
                        if(Rval != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaCT_Init failed!!", 0U, 0U);
                        }

                        if (SVC_OK != AmbaWrap_memset(&ADASscrript, 0, sizeof(ADASscrript))) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaWrap_memset ADASscrript failed", 0U, 0U);
                        }
                        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                        }
                        ADASscrript[0] = pSvcUserPref->MainStgDrive[0];
                        CmdAPP_strcat(ADASscrript, ADASCalibFileName);

                        Rval = AmbaCT_Load(ADASscrript);
                        if(Rval != 0U) {
                            AmbaPrint_PrintStr5("AmbaCT_Load %s fail", ADASscrript, NULL, NULL, NULL, NULL);
                        } else {
                            QuickCalib_V1(Horizontal, FocalLength, warning_zone_x_offset);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 1U);
                    SvcLog_OK(SVC_LOG_CMDAPP, "Enable draw warning zone", 0U, 0U);
                } else {
                    SvcAdasNotify_WarningZoneGet(ZoneX, ZoneY);
                    SvcAdasNotify_WarningZoneSet(ZoneX, ZoneY, 0U);
                    SvcAdasNotify_WarningZoneEnable(0U);
                    SvcLog_OK(SVC_LOG_CMDAPP, "Disable draw warning zone", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("mode", pArgVector[2U]))) {
                IsReboot = 0U;
                if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
                    if (0 == SvcWrap_strcmp("ttc", pArgVector[3U])) {
                        pSvcUserPref->FcwsMode = AMBA_WS_FCWS_MODE_TTC;
                        SvcLog_OK(SVC_LOG_CMDAPP, "Set to TTC mode, mode = %d", AMBA_WS_FCWS_MODE_TTC, 0U);
                        IsReboot = 1;
                    } else if (0 == SvcWrap_strcmp("position", pArgVector[3U])) {
                        pSvcUserPref->FcwsMode = AMBA_WS_FCWS_MODE_POSITION;
                        SvcLog_OK(SVC_LOG_CMDAPP, "Set to POSITION mode, mode = %d", AMBA_WS_FCWS_MODE_POSITION, 0U);
                        IsReboot = 1;
                    } else if (0 == SvcWrap_strcmp("hor", pArgVector[3U])) {
                        pSvcUserPref->FcV2_SR_mode = RF_FC_SR_MODE_HOR;
                        SvcLog_OK(SVC_LOG_CMDAPP, "Set to horizontal mode", 0U, 0U);
                        IsReboot = 1;
                    } else if (0 == SvcWrap_strcmp("ver", pArgVector[3U])) {
                        pSvcUserPref->FcV2_SR_mode = RF_FC_SR_MODE_VER;
                        SvcLog_OK(SVC_LOG_CMDAPP, "Set to vertical mode", 0U, 0U);
                        IsReboot = 1;
                    } else {
                        SvcLog_NG(SVC_LOG_CMDAPP, "adas cmd of mode error", 0U, 0U);
                    }
                    if (1U == IsReboot) {
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Save() 'FcwsMode' failed %d ", Rval, 0U);
                        } else {
                            SvcLog_DBG(SVC_LOG_CMDAPP, "Rebooting......", 0U, 0U);
                            if (SVC_OK != AmbaKAL_TaskSleep(1000)) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed", 0U, 0U);
                            }
                            if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                            }
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("ldws", pArgVector[2U]))) {
                if (0 == SvcWrap_strcmp("speed", pArgVector[3U])) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    }
                    Rval = SvcWrap_strtoul(pArgVector[4U], &(pSvcUserPref->LdwsActiveSpeed)); AmbaMisra_TouchUnused(&Rval);
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                    }
                    IsReboot = 1U;
                }
            } else if ((0 == SvcWrap_strcmp("lka", pArgVector[2U]))) {
                if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcLdwsTaskV2_EnableLKA(1U);
                    } else {
                        SvcLdwsTask_EnableLKA(1U);
                    }
                    SvcLog_OK(SVC_LOG_CMDAPP, "ADAS LKA on", 0U, 0U);
                } else {
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcLdwsTaskV2_EnableLKA(0U);
                    } else {
                        SvcLdwsTask_EnableLKA(0U);
                    }
                    SvcLog_OK(SVC_LOG_CMDAPP, "ADAS LKA off", 0U, 0U);
                }
            } else if ((0 == SvcWrap_strcmp("lka_debug", pArgVector[2U]))) {
                if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcLdwsTaskV2_LKADebugEnable(1U);
                    } else {
                        SvcLdwsTask_LKADebugEnable(1U);
                    }
                } else {
                    if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                        SvcLdwsTaskV2_LKADebugEnable(0U);
                    } else {
                        SvcLdwsTask_LKADebugEnable(0U);
                    }
                }
            } else {
                SvcLog_NG(SVC_LOG_CMDAPP, "Something wrong for adas cmd. AdasEnable = %d", AdasEnable, 0U);
            }
            Rval = SVC_OK;
#endif
        } else if (0 == SvcWrap_strcmp("ssrdis", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 SensorBits = 0U, SensorID, VinID = 0U;
                AMBA_SENSOR_CHANNEL_s Chan;
                Rval = SvcWrap_strtoul(pArgVector[2], &SensorBits); AmbaMisra_TouchUnused(&Rval);
                Rval = SvcWrap_strtoul(pArgVector[3], &VinID); AmbaMisra_TouchUnused(&Rval);
                AmbaSvcWrap_MisraMemset(&Chan, 0, sizeof(Chan));
                Chan.VinID = VinID;
                for (SensorID = 0; SensorID < 4U; SensorID ++) {
                    if ((((UINT32)1 << SensorID) & SensorBits) != 0U) {
                        Chan.SensorID |= (UINT32) 0x10 << (SensorID << 2U);
                    }
                }
                SvcLog_OK(SVC_LOG_CMDAPP, "vin id %u sensor id 0x%x", Chan.VinID, Chan.SensorID);
                Rval = AmbaSensor_Disable(&Chan);
            }
#ifdef CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL
        } else if (0 == SvcWrap_strcmp("vfctrl", pArgVector[1U])) {
            SVC_VOUT_FRM_CTRL_CFG_s Cfg = {0};
            UINT32 DlyCount = 0, Value = 0;
            UINT32 YuvStrmIdx = 0U;

            if (3U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    if (SVC_OK != SvcVoutFrmCtrlTask_Init()) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVoutFrmCtrlTask_Init failed", 0U, 0U);
                    }
                    Rval = SvcWrap_strtoul(pArgVector[3U], &DlyCount);
                    if (4U < ArgCount) {
                        Rval |= SvcWrap_strtoul(pArgVector[4U], &YuvStrmIdx);
                    } else {
                        YuvStrmIdx = 0x1;
                    }
                    if (5U < ArgCount) {
                        Rval |= SvcWrap_strtoul(pArgVector[5U], &Value);
                    } else {
                        Value = 0x0;
                    }
                    Cfg.YuvStrmIdx = YuvStrmIdx;
                    Cfg.DlyCount = (UINT8) DlyCount;
                    Cfg.SyncMode = (UINT8) Value;
                    Rval = SvcVoutFrmCtrlTask_Config(&Cfg);
                    Rval |= SvcVoutFrmCtrlTask_Start();
                } else if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                    Rval = SvcWrap_strtoul(pArgVector[3U], &Value);
                    SvcVoutFrmCtrlTask_DebugEnable(Value);
                } else {
                    /* Do nothing */
                }
            }
#endif
#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
        } else if (0 == SvcWrap_strcmp("vout_def_img", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    if (SVC_OK != SvcVoutDefImgTask_Init()) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVoutDefImgTask_Init failed", 0U, 0U);
                    }
                }
                Rval = SVC_OK;
            }
            if (3U < ArgCount) {
                UINT32 DispAlt = 0U;
                if (0 == SvcWrap_strcmp("ctrl", pArgVector[2U])) {
                    Rval = SvcWrap_strtoul(pArgVector[3U], &DispAlt);
                    if (SVC_OK != SvcVoutDefImgTask_Control(DispAlt)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcVoutDefImgTask_Control failed", 0U, 0U);
                    }
                }
                Rval = SVC_OK;
            }
#endif
        } else if (0 == SvcWrap_strcmp("usb", pArgVector[1U])) {
            UINT32           RetVal = SVC_NG, UsbClass = 0U;

            if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get failed", 0U, 0U);
            }

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2], &UsbClass)) {
                SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed", 0U, 0U);
            }
            if (UsbClass < SVC_STG_USB_CLASS_NUM) {
                pSvcUserPref->UsbClass = (UINT8)UsbClass;

                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Save() failed with %d", RetVal, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_CMDAPP, "Save UsbClass = %d to preference", pSvcUserPref->UsbClass, 0U);
                }
                IsReboot = 1U;
                Rval = SVC_OK;
            }
            SvcLog_OK(SVC_LOG_CMDAPP, "UsbClass = %d", pSvcUserPref->UsbClass, 0U);
        } else if (0 == SvcWrap_strcmp("set_rec_maxbrate", pArgVector[1U])) {
            UINT32 Value = 0U;

            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
                    pSvcUserPref->MaxBitrate = Value; /* Unit: Mbps*/
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Save() 'MaxBitrate' failed %d ", Rval, 0U);
                    } else {
                        SvcLog_OK(SVC_LOG_CMDAPP, "SvcUserPref_Save() 'MaxBitrate' = %d to preference", pSvcUserPref->MaxBitrate, 0U);
                        SvcLog_DBG(SVC_LOG_CMDAPP, "Rebooting......", 0U, 0U);
                        if (SVC_OK != AmbaKAL_TaskSleep(1000)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaKAL_TaskSleep failed", 0U, 0U);
                        }
                        if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed", 0U, 0U);
                }
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("set_rec_fsplit", pArgVector[1U])) {
            UINT32 Value = 0U;

            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                    pSvcUserPref->FileSplitTimeSeconds = Value; /* Unit: Second */
#else
                    pSvcUserPref->FileSplitTimeMin = Value; /* Unit: Mins */
#endif
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save() 'FileSplitTimeMin' failed %d ", Rval, 0U);
                    } else {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        SvcLog_OK(SVC_LOG_CMDAPP, "SvcPref_Save() 'FileSplitTimeSeconds' = %d to preference", pSvcUserPref->FileSplitTimeSeconds, 0U);

#else
                        SvcLog_OK(SVC_LOG_CMDAPP, "SvcPref_Save() 'FileSplitTimeMin' = %d to preference", pSvcUserPref->FileSplitTimeMin, 0U);
#endif
                    }
                    IsReboot = 1U;
                    Rval = SVC_OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("ssrp", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 SensorBits = 0U, SensorID;
                AMBA_SENSOR_CHANNEL_s Chan;
                AMBA_SENSOR_CONFIG_s  SsCfg;
                const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
                Rval = SvcWrap_strtoul(pArgVector[2], &SensorBits); AmbaMisra_TouchUnused(&Rval);
                AmbaSvcWrap_MisraMemset(&Chan, 0, sizeof(Chan));
                for (SensorID = 0; SensorID < 4U; SensorID ++) {
                    if ((((UINT32)1 << SensorID) & SensorBits) != 0U) {
                        Chan.SensorID |= (UINT32) 0x10 << (SensorID << 2U);
                    }
                }
                SvcLog_OK(SVC_LOG_CMDAPP, "vin id %u sensor id 0x%x", Chan.VinID, Chan.SensorID);
                AmbaSvcWrap_MisraMemset(&SsCfg, 0, sizeof(AMBA_SENSOR_CONFIG_s));
                SsCfg.ModeID = pCfg->SensorCfg[0][0].SensorMode | 0x10000000U;
                Rval = AmbaSensor_Config(&Chan, &SsCfg);
            }
//        } else if (0 == SvcWrap_strcmp("brdgchk", pArgVector[1U])) {
//            if (2U < ArgCount) {
//                UINT32 SensorBits, SensorID;
//                AMBA_SENSOR_CHANNEL_s Chan;
//                UINT32 Lock;
//                Rval = SvcWrap_strtoul(pArgVector[2], &SensorBits); AmbaMisra_TouchUnused(&Rval);
//                AmbaSvcWrap_MisraMemset(&Chan, 0, sizeof(Chan));
//                for (SensorID = 0; SensorID < 4U; SensorID ++) {
//                    if ((((UINT32)1 << SensorID) & SensorBits) != 0U) {
//                        Chan.SensorID |= (UINT32) 0x10 << (SensorID << 2U);
//                    }
//                }
//                SvcLog_OK(SVC_LOG_CMDAPP, "vin id %u sensor id 0x%x", Chan.VinID, Chan.SensorID);
//                Rval = AmbaSensor_GetSerdesLinkStatus(&Chan, &Lock);
//                SvcLog_OK(SVC_LOG_CMDAPP, "lock stat %u", Lock, 0U);
//            }
        } else if (0 == SvcWrap_strcmp("dbg_print", pArgVector[1U])) {
            extern void AmbaPrint_StopAndFlush(void);
            AmbaPrint_StopAndFlush();
#ifdef CONFIG_BUILD_IMGFRW_AAA
        } else if (0 == SvcWrap_strcmp("cb", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                Rval = SvcColorBalance_Start();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcColorBalance_Start() failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])) {
                Rval = SvcColorBalance_Stop();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcColorBalance_Stop() failed!!", 0U, 0U);
                }
            } else {
                // do nothing.
            }
#endif
#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
        } else if (0 == SvcWrap_strcmp("ema", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                Rval = SvcEmrAdaptiveTask_Start();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcEmrAdaptiveTask_Start failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])){
                Rval = SvcEmrAdaptiveTask_Stop();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcEmrAdaptiveTask_Stop failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("default", pArgVector[2U])) {
                Rval = SvcEmrAdaptiveTask_SetDefaultBldTbl();
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcEmrAdaptiveTask_SetDefaultBldTbl failed!!", 0U, 0U);
                }
            } else if (0 == SvcWrap_strcmp("osd", pArgVector[2U])) {
                UINT32 Enb = 0xFU;

                if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                    Enb = 0xFU;
                } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                    Enb = 0U;
                } else {
                    Rval = SvcWrap_strtoul(pArgVector[3], &Enb);
                    if (Rval != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                    }
                }
                SvcEmrAdaptiveTask_SetOsd(Enb);
            } else if (0 == SvcWrap_strcmp("log", pArgVector[2U])) {
                UINT32 Enb = 0U;

                Rval = SvcWrap_strtoul(pArgVector[3], &Enb);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcEmrAdaptiveTask_SetPrint(Enb);
            } else if (0 == SvcWrap_strcmp("filter", pArgVector[2U])) {
                UINT32 Width = 0U;

                Rval = SvcWrap_strtoul(pArgVector[3], &Width);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcEmrAdaptiveTask_SetFilterWidth(Width);
            } else if (0 == SvcWrap_strcmp("stch", pArgVector[2U])) {
                UINT32 Width = 0U;

                Rval = SvcWrap_strtoul(pArgVector[3], &Width);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcEmrAdaptiveTask_SetMaxStitchZone(Width);
            } else if (0 == SvcWrap_strcmp("margin", pArgVector[2U])) {
                UINT32 Width = 0U;

                Rval = SvcWrap_strtoul(pArgVector[3], &Width);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                SvcEmrAdaptiveTask_SetBbxRoiMargin(Width);
            } else {
                SvcLog_OK(SVC_LOG_CMDAPP, "svc_app ema [start/stop/default/osd/log/filter/stch/margin]", 0U, 0U);
            }
#endif
        } else if (0 == SvcWrap_strcmp("nvm_tsk", pArgVector[1U])) {
            extern void SvcNvmTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector);
            SvcNvmTask_ShellEntry(ArgCount - 1U, &pArgVector[1U]);
#ifdef CONFIG_ICAM_PROJECT_SURROUND
        } else if (0 == SvcWrap_strcmp("anim", pArgVector[1U])) {
            SvcAnimCalib_CmdEntry(ArgCount, pArgVector, PrintFunc);
#endif
        } else if (0 == SvcWrap_strcmp("mctfcmpr", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32           MctfCmprCtrl = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    MctfCmprCtrl = 1U;
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    MctfCmprCtrl = 0U;
                } else {
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->MctfCmprCtrl = MctfCmprCtrl;

                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                        IsReboot = 1U;
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("mctsdout", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32  MctsDramOutCtrl = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    MctsDramOutCtrl = 1U;
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    MctsDramOutCtrl = 0U;
                } else {
                    Rval = SVC_NG;
                }

                if (SVC_OK == Rval) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->MctsDOutCtrl = MctsDramOutCtrl;

                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                        IsReboot = 1U;
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("mainstg", pArgVector[1U])) {
            Rval = SVC_NG;

            if (2U < ArgCount) {
                UINT32  StgChan = 0U;

                if (SvcWrap_strtoul(pArgVector[2U], &StgChan) == SVC_OK) {
                    if (SvcUserPref_Get(&pSvcUserPref) == SVC_OK) {
                        switch (StgChan) {
                        case 0:
                            pSvcUserPref->MainStgDrive[0] = 'c';
                            pSvcUserPref->MainStgChan  = AMBA_SD_CHANNEL0;
                            break;
                        case 1:
                            pSvcUserPref->MainStgDrive[0] = 'd';
                            pSvcUserPref->MainStgChan  = AMBA_SD_CHANNEL1;
                            break;
                        default:
                            /* do nothing */
                            break;
                        }

                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SvcPref_Save(PrefBufAddr, PrefBufSize) == SVC_OK) {
                            IsReboot = 1U;
                            Rval = SVC_OK;
                        }
                    }
                }
            }
#ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
        } else if (0 == SvcWrap_strcmp("sensor_crc_chk_dbg_lvl", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Lvl = 0U;
                if (SvcWrap_strtoul(pArgVector[2U], &Lvl) == SVC_OK) {
                    SvcSensorCrcCheckTask_DebugLvl(Lvl);
                }
            }
#endif
#if defined(CONFIG_ICAM_CV_STEREO)
        } else if (0 == SvcWrap_strcmp("warp", pArgVector[1U])) {
            UINT32 FovID = 0U, Enable = 0U, Value = 0U;
            extern UINT32 Svc_ApplyWarpFile(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 Header,
                UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, const char *FileName);

            Rval = SVC_NG;
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("file", pArgVector[2U])) {
                    if (5U < ArgCount) {
                        Rval = SvcWrap_strtoul(pArgVector[3], &FovID);
                        Rval |= SvcWrap_strtoul(pArgVector[4], &Enable);
                        Rval |= SvcWrap_strtoul(pArgVector[5], &Value);
                        if (Rval != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                        }

                        Rval = Svc_ApplyWarpFile(FovID,   /* FovID */
                                                 Enable,  /* Enable */
                                                 0U, 0U,  /* Width, Height */
                                                 Value,   /* Header */
                                                 0U, 0U,  /* HGdNum, VGdNum */
                                                 0U, 0U,  /* TileWExp, TileHExp */
                                                 pArgVector[6U] /* warp file with header */);
                    }
                }
            }
#endif
#if defined(CONFIG_QNX)
        } else if (0 == SvcWrap_strcmp("amage_srv_create", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 Port = 0U;

                Rval = SvcWrap_strtoul(pArgVector[3], &Port);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                }

                Rval = SvcAmageTask_ServerCreate(pArgVector[2U], (UINT16)Port);
            }
        } else if (0 == SvcWrap_strcmp("amage_srv_delete", pArgVector[1U])) {
            Rval = SvcAmageTask_ServerDelete();
#endif
#ifdef CONFIG_ICAM_YUVFILE_FEED
        } else if (0 == SvcWrap_strcmp("yuvfeed_file", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32                Err, FovBits = 0U, i, j, FovNum = 0U, BufNum = 0U, Interval = 0U;
                static UINT32         RunBits = 0U;
                UINT16                FovIdxArr[CONFIG_ICAM_MAX_REC_STRM];
                SVC_FEEDYUV_CREATE_s  CreateInfo = {0};
                if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                    if (5U < ArgCount) {
                        Err = SvcWrap_strtoul(pArgVector[3], &FovBits);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                            Rval = SVC_NG;
                        }

                        Err = SvcWrap_strtoul(pArgVector[4], &BufNum);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                            Rval = SVC_NG;
                        }

                        Err = SvcWrap_strtoul(pArgVector[5], &Interval);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                            Rval = SVC_NG;
                        }

                        if (RunBits != 0U) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "RunBits %u != 0", RunBits, 0U);
                            Rval = SVC_NG;
                        }

                        if (Rval == SVC_OK) {
                            SvcUtil_BitsToArr(AMBA_DSP_MAX_VIEWZONE_NUM, FovBits, &FovNum, FovIdxArr);
                            CreateInfo.FovNum         = FovNum;
                            CreateInfo.UpdateInterval = Interval;

                            if (BufNum > (UINT32)SVC_FEEDYUV_MAX_BUF) {
                                BufNum = SVC_FEEDYUV_MAX_BUF;
                            }

                            for (i = 0U; i < FovNum; i++) {
                                CreateInfo.FovInfo[i].FovIdx         = FovIdxArr[i];
                                CreateInfo.FovInfo[i].BufNum         = BufNum;
                                for (j = 0U; j < BufNum; j++) {
                                    if ((6UL + j) < ArgCount) {
                                        SvcWrap_strcpy(&(CreateInfo.FovInfo[i].FileName[j][0]), 64U, pArgVector[6UL + j]);
                                    } else {
                                        CreateInfo.FovInfo[i].FileName[j][0] = '\0';
                                    }
                                }
                            }

                            Err = SvcLvFeedFileYuvTask_TaskCreate(&CreateInfo);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "SvcLvFeedFileYuvTask_TaskCreate failed!!", Err, 0U);
                            }

                            Err = SvcLvFeedFileYuvTask_Start(FovBits);
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "SvcLvFeedFileYuvTask_Start failed!!", Err, 0U);
                            } else {
                                RunBits |= FovBits;
                            }
                        }
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])) {
                    Err = SvcWrap_strtoul(pArgVector[3], &FovBits);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                        Rval = SVC_NG;
                    }

                    if (Rval == SVC_OK) {
                        Err = SvcLvFeedFileYuvTask_Stop(FovBits);
                        if (Err != SVC_OK) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcLvFeedFileYuvTask_Stop failed!!", Err, 0U);
                        } else {
                            RunBits = RunBits & (~FovBits);
                        }

                        if (RunBits == 0U) {
                            Err = SvcLvFeedFileYuvTask_TaskDelete();
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_CMDAPP, "SvcLvFeedFileYuvTask_TaskDelete failed!!", Err, 0U);
                            }
                        }
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
#endif
#if 0 //CONFIG_ICAM_PROJECT_SHMOO
        } else if (0 == SvcWrap_strcmp("shmoo_info", pArgVector[1U])) {
            Rval = ShmooSetInfo(ArgCount, pArgVector);
            IsReboot = 1U;

#endif
#ifdef CONFIG_ICAM_AVE_RAW_CAP
        } else if (0 == SvcWrap_strcmp("ave_raw", pArgVector[1U])) {
            extern UINT32 SvcAveRawCapTask_Init(void);
            Rval = SvcAveRawCapTask_Init();
            if (Rval == SVC_OK) {
                SvcLog_DBG(SVC_LOG_CMDAPP, "SvcAveRawCapTask is ready. Type \"svc_ave_raw_task\" to check.", 0U, 0U);
            }
#endif
        } else if (0 == SvcWrap_strcmp("vin_buf_mon", pArgVector[1U])) {
            extern void SvcVinBufMonTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
            SvcVinBufMonTask_ShellEntry(ArgCount, pArgVector, PrintFunc);
        } else if (0 == SvcWrap_strcmp("boot2rec", pArgVector[1U])) {
            UINT32 TimeOut = 0U, Err;

            if (2U < ArgCount) {
                Err = SvcWrap_strtoul(pArgVector[2], &TimeOut);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CMDAPP, "SvcWrap_strtoul failed!!", 0U, 0U);
                    Rval = SVC_NG;
                }

                if (Rval == SVC_OK) {
                    Err = SvcUserPref_Get(&pSvcUserPref);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get failed!!", 0U, 0U);
                        Rval = SVC_NG;
                    } else {
                        pSvcUserPref->BootToRecTimeOut = TimeOut;
                    }
                }

                if (Rval == SVC_OK) {
                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    Err = SvcPref_Save(PrefBufAddr, PrefBufSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save() 'BootToRecTimeOut' failed %d ", Rval, 0U);
                    } else {
                        SvcLog_OK(SVC_LOG_CMDAPP, "SvcPref_Save() 'BootToRecTimeOut' = %d to preference", TimeOut, 0U);
                        IsReboot = 1U;
                    }
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("exec", pArgVector[1U])) {
            if (2U < ArgCount) {
                Rval = SvcShell_ExecScript(pArgVector[2U]);
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("system_cfg", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 ParIdx = 0U;
                UINT32 Val = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &ParIdx);
                (void) SvcWrap_strtoul(pArgVector[3U], &Val);

                if (SVC_OK == SvcLiveviewTask_SetDspSystemCfg(ParIdx, Val)) {
                    SvcLog_DBG(SVC_LOG_CMDAPP, "@@ Set SvcCmdDspSysCfg .Idx = %u, .Val = %u", ParIdx, Val);
                } else {
                    SvcLog_NG(SVC_LOG_CMDAPP, "@@ Set SvcCmdDspSysCfg .Idx = %u, .Val = %u", ParIdx, Val);
                }
            }
            Rval = OK;
        } else if (0 == SvcWrap_strcmp("thmview_log", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 ShowLog = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    ShowLog = 1U;
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    ShowLog = 0U;
                } else {
                    Rval = SVC_NG;
                }
                if (SVC_OK == Rval) {
                    if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
                        SvcLog_NG(SVC_LOG_CMDAPP, "SvcUserPref_Get() failed!!", 0U, 0U);
                    } else {
                        pSvcUserPref->ShowThmViewLog = (UINT8)ShowLog;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        if (SVC_OK != SvcPref_Save(PrefBufAddr, PrefBufSize)) {
                            SvcLog_NG(SVC_LOG_CMDAPP, "SvcPref_Save failed!!", 0U, 0U);
                        }
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#if defined(CONFIG_ATF_SUSPEND_SRAM)
        } else if (0 == SvcWrap_strcmp("suspend", pArgVector[1U])) {
            Rval = SvcFlowControl_Exec("suspend_resume");
#endif
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval != SVC_OK) {
        CmdAppUsage(PrintFunc);
    } else {
        if (0U < IsReboot) {
            SvcLog_DBG(SVC_LOG_CMDAPP, "Rebooting......", 0U, 0U);
            if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                SvcLog_NG(SVC_LOG_CMDAPP, "AmbaSYS_Reboot() failed!!", 0U, 0U);
            }
        }
    }
}

/**
* install svc app command
*/
void SvcCmdApp_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdApp;

    UINT32  Rval;

    SvcCmdApp.pName    = "svc_app";
    SvcCmdApp.MainFunc = CmdAppEntry;
    SvcCmdApp.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdApp);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_CMDAPP, "## fail to install svc app command", 0U, 0U);
    }

#if defined(CONFIG_ICAM_IMGITUNER_USED)
    {
        ULONG  BufBase = 0U;
        UINT32 BufSize = 0U;

        SvcCmdIK_Install();

        if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ITUNER, &BufBase, &BufSize)) {
            SvcLog_DBG(SVC_LOG_CMDAPP, "## fail to request ituner memory", 0U, 0U);
        } else {
            SvcCmdIK_ItunerOn(BufBase, BufSize);
        }
    }
#endif

#if defined(CONFIG_BUILD_CV)
    {
        extern void SvcCmdAppCV_Install(void);
        SvcCmdAppCV_Install();
    }
#endif
}
