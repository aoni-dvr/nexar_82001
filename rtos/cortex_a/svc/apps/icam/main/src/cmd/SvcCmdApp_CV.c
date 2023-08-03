/**
 *  @file SvcCmdApp_CV.c
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
 *  @details svc application cv commands functions
 *
 */

#include "AmbaShell.h"
#include "AmbaSYS.h"
#include "AmbaUtility.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaSvcWrap.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"

/* ssp */
#include "AmbaFS.h"


#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcPref.h"
#include "SvcUserPref.h"
#include "SvcCmd.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"

#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_AmbaSeg.h"
#include "SvcCvFlow_CnnTestbed.h"
#include "SvcCvFlow_CnnTestbedIpc.h"
#include "SvcCvFlow_RefSeg.h"
#include "SvcCvFlow_Stixel.h"

#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#include "SvcCvAppDef.h"
#include "SvcGui.h"
#include "SvcSegDrawTask.h"
#include "SvcODDrawTask.h"
#include "SvcExtOsdDrawTask.h"
#include "SvcCvFileInTask.h"
#include "SvcImgFIFO.h"
#include "SvcImgFeeder.h"
#if defined(CONFIG_ICAM_CV_LOGGER)
#include "AmbaVfs.h"
#include "SvcCvLogger.h"
#include "SvcCvLoggerTask.h"
#endif

#ifdef CONFIG_BUILD_AMBA_ADAS
#include "SvcCan.h"
#include "AmbaSR_Lane.h"
#include "AmbaAP_LKA.h"
#include "SvcLdwsTask.h"
#include "RefFlow_FC.h"
#include "SvcFcwsFcmdTask.h"
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCT_TextHdlr.h"
#include "AmbaCT_EmirTuner.h"
#include "AmbaCalib_AVMIF.h"
#include "SvcCalibAdas.h"
#endif
#ifdef CONFIG_ICAM_CV_STEREO
#include "SvcCvFlow_Stereo.h"
#include "SvcStereoTask.h"
#include "AmbaSTU_StereoBarCalibInfo.h"
#include "cvapi_flexidag_ambamvac_cv2.h"
#endif
#if (defined(CONFIG_ICAM_CV_STIXEL) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))
#include "SvcStixelTask.h"
#endif

#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
#include "SvcODDrawTask_Rebel.h"
#endif

#include "cvapi_amba_od37_fc_category.h"
#include "SvcDirtDetect.h"

#define SVC_LOG_CMDCV      "CmdCv"

extern void SvcCmdAppCV_Install(void);

static UINT32 AppCV_SetBootFlag(SVC_CV_MAIN_BOOT_FLAG_s *pCvBootFlag, const char *pArg, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = SVC_OK;

    if (0 == SvcWrap_strcmp("sd", pArg)) {
        pCvBootFlag->Source = SVC_CV_MAIN_BOOT_FLAG_SD;
        PrintFunc("SD on\n");
    } else if (0 == SvcWrap_strcmp("romfs", pArg)) {
        pCvBootFlag->Source = SVC_CV_MAIN_BOOT_FLAG_ROMFS;
        PrintFunc("ROMFS on\n");
    } else if (0 == SvcWrap_strcmp("cmdmode", pArg)) {
        pCvBootFlag->Source = SVC_CV_MAIN_BOOT_FLAG_CMDMODE;
        PrintFunc("CmdMode on\n");
    } else if (0 == SvcWrap_strcmp("auto_cont", pArg)) {
        pCvBootFlag->Flag = SVC_CV_MAIN_BOOT_FLAG_AUTO_CONTINUE;
        PrintFunc("CmdMode on\n");
    } else {
        Rval = SVC_NG;
    }

    return Rval;
}

static UINT32 AppCV_FeederCallback(SVC_IMG_FEEDER_PORT_s *pPort, UINT32 Event, void *pInfo)
{
    const SVC_CV_INPUT_IMG_INFO_s *pFrameInfo;
    const AMBA_DSP_PYMD_DATA_RDY_s *pPyramid;
    const AMBA_DSP_YUV_DATA_RDY_s *pYuv;
    UINT32 i = 0;

    AmbaMisra_TouchUnused(pInfo);

    if (Event == SVC_IMG_FEEDER_IMAGE_DATA_READY) {
        AmbaMisra_TypeCast(&pFrameInfo, &pInfo);

        AmbaPrint_PrintUInt5("Recv[%d]: Port(%d) NumInfo(%d) DataSrc(%d) StrmID(%d)",
                pPort->FeederID,
                pPort->PortID,
                pFrameInfo->NumInfo,
                pFrameInfo->Info[0].Content.DataSrc,
                pFrameInfo->Info[0].Content.StrmId);

        for (i = 0; i < pFrameInfo->NumInfo; i++) {
            if (pFrameInfo->Info[i].Content.DataSrc == SVC_IMG_FIFO_SRC_PYRAMID) {
                AmbaMisra_TypeCast(&pPyramid, &pFrameInfo->Info[i].pBase);
                AmbaPrint_PrintUInt5("Recv: ViewZoneID(%d) CapSeg(%d) P(%d) W(%d) H(%d)",
                        pPyramid->ViewZoneId,
                        (UINT32)pPyramid->CapSequence,
                        pPyramid->YuvBuf[0].Pitch,
                        pPyramid->YuvBuf[0].Window.Width,
                        pPyramid->YuvBuf[0].Window.Height);
            } else if (pFrameInfo->Info[i].Content.DataSrc == SVC_IMG_FIFO_SRC_MAIN_YUV) {
                AmbaMisra_TypeCast(&pYuv, &pFrameInfo->Info[i].pBase);
                AmbaPrint_PrintUInt5("Recv: ViewZoneID(%d) CapSeg(%d) P(%d) W(%d) H(%d)",
                        pYuv->ViewZoneId,
                        (UINT32)pYuv->CapSequence,
                        pYuv->Buffer.Pitch,
                        pYuv->Buffer.Window.Width,
                        pYuv->Buffer.Window.Height);
            } else {
                /* Do nothing */
            }
        }

        (void) SvcImgFeeder_MsgToFeeder(pPort, SVC_IMG_FEEDER_RECEIVER_READY, NULL);
    }

    return SVC_OK;
}

static UINT32 AppCV_FIFOCallback(SVC_IMG_FIFO_CHAN_s *pChan, UINT32 Event, void *pInfo)
{
    const SVC_IMG_FIFO_DATA_STATUS_s *pDataSt;
    UINT32 Offset;
    const UINT8 *pData;

    AmbaMisra_TouchUnused(pChan);
    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(pInfo);

    if (Event == SVC_IMG_FIFO_DATA_READY) {
        AmbaMisra_TypeCast(&pDataSt, &pInfo);
        Offset = pDataSt->Wp * pDataSt->ElementSize;
        pData = &pDataSt->pQueueBase[Offset];

        AmbaPrint_PrintUInt5("Recv: Wp = %d ElementSize = %d", pDataSt->Wp, pDataSt->ElementSize, 0U, 0U, 0U);

        if (pDataSt->Content.DataSrc == SVC_IMG_FIFO_SRC_PYRAMID) {
            const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pData);

            AmbaPrint_PrintUInt5("Recv: Chan(%d) User(%d) DataSrc(%d) StrmID(%d) Seq(%11u)",
                                  pChan->ChanID,
                                  pChan->UserID,
                                  (UINT32)pDataSt->Content.DataSrc,
                                  (UINT32)pYuvInfo->ViewZoneId,
                                  (UINT32)pYuvInfo->CapSequence);
            SvcWrap_PrintUL("    (P %u %u x %u) (0x%x, 0x%x)",
                                  (ULONG)pYuvInfo->YuvBuf[0].Pitch,
                                  (ULONG)pYuvInfo->YuvBuf[0].Window.Width,
                                  (ULONG)pYuvInfo->YuvBuf[0].Window.Height,
                                  pYuvInfo->YuvBuf[0].BaseAddrY,
                                  pYuvInfo->YuvBuf[0].BaseAddrUV);
        } else if ((pDataSt->Content.DataSrc == SVC_CV_DATA_SRC_MAIN_YUV) ||
                   (pDataSt->Content.DataSrc == SVC_CV_DATA_SRC_YUV_STRM)) {
            const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo;
            AmbaMisra_TypeCast(&pYuvInfo, &pData);

            AmbaPrint_PrintUInt5("Recv: Chan(%d) User(%d) DataSrc(%d) StrmID(%d) Seq(%11u)",
                                  pChan->ChanID,
                                  pChan->UserID,
                                  (UINT32)pDataSt->Content.DataSrc,
                                  (UINT32)pYuvInfo->ViewZoneId,
                                  (UINT32)pYuvInfo->CapSequence);

            SvcWrap_PrintUL("    (P %u %u x %u) (0x%x, 0x%x)",
                                  (ULONG)pYuvInfo->Buffer.Pitch,
                                  (ULONG)pYuvInfo->Buffer.Window.Width,
                                  (ULONG)pYuvInfo->Buffer.Window.Height,
                                  pYuvInfo->Buffer.BaseAddrY,
                                  pYuvInfo->Buffer.BaseAddrUV);
        } else {
            /* Do nothing */
        }
    }

    return SVC_OK;
}

static void CmdCvUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_cv commands:\n");
    PrintFunc(" bootflag [Param0] [Param1] .. :CV boot flag\n");
    PrintFunc("     Param0: 'sd': SD mode. 'romfs': ROMFS mode. 'cmdmode': Cmd mode.\n");
    PrintFunc("     Example: svc_cv bootflag sd\n");
    PrintFunc(" schdr [cmd] [ctrl]: scheduler commands\n");
    PrintFunc("     log [on/off]: Enable ARM scheduler debug log\n");
    PrintFunc(" mem: show memory usage\n");
    PrintFunc(" continue: Resume booting flow while in cmdmode\n");
    PrintFunc(" restart: restart all cv flow\n");
    PrintFunc(" stop: stop all cv flow\n");
    PrintFunc(" load flexibin [CvFlowChan] [path]: Load flexibin for specific CvFlow channel\n");
    PrintFunc(" fin config [CvFlowChan]: cv file in config\n");
    PrintFunc("     input_info [InputIdx] [DataIdx] [Width] [Height] [Pitch] [ScaleIdx]: cv input config\n");
    PrintFunc("     file_info [InputIdx] [DataIdx] [FileName]: cv input file config\n");
    PrintFunc("     start         : cv file in start\n");
    PrintFunc("     startspu      : spu yuv file in start\n");
    PrintFunc("     startspubinary: spu binary in start\n");
    PrintFunc("     startfusionbinary: fusion binary in start\n");
    PrintFunc(" drv [cmd] [CvFlowChan] [Param]: CvFlow driver control\n");
    PrintFunc("     debug [CvFlowChan] [0/1]: Enable debug log\n");
    PrintFunc("     dump [CvFlowChan]: Enable data dump \n");
    PrintFunc("     setrate [CvFlowChan] [divider]: Set source rate\n");
    PrintFunc("     enable [CvFlowChan] [0/1]: Enable/Disable the channel\n");
    PrintFunc("     setroi [CvFlowChan] [StartX] [StartY]: Configure ROI offset\n");
    PrintFunc("     ssdnms [CvFlowChan] [ClassNum] [PriorBoxPath] [ROI_W] [ROI_H] [ROI_X] [ROI_Y]\n");
    PrintFunc("         [NET_W] [NET_H] [VIS_W] [VIS_H] [ModelType]\n");
    PrintFunc("     segwin [CvFlowChan] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H]\n");
    PrintFunc(" oddraw [cmd] [ctrl]: ODDrawTask control\n");
    PrintFunc("     debug [on/off] : Enable debug log\n");
    PrintFunc("     category_label [on/off] : Show category label\n");
    PrintFunc("     score_label [on/off] [(Optional)MaxScore]: Show score label. MaxScore - To convert to percentage.\n");
    PrintFunc("     detres [on/off] : Print OD BBX result\n");
    PrintFunc("     roi [on/off] : Show ROI\n");
    PrintFunc(" cfg_num [num]: CvFlowNum and CvFlowBits\n");
    PrintFunc(" cfg [Idx] [Chan] [CvObjAddr] [CvModeID]: CvFlow structure.\n");
    PrintFunc("     Enter \"svc_cv cfg\" for the detail.\n");
    PrintFunc(" input_cfg: CvFlow InputCfg structure.\n");
    PrintFunc("     Enter \"svc_cv input_cfg\" for the detail.\n");
    PrintFunc(" output_cfg: CvFlow OutputCfg structure.\n");
    PrintFunc("     Enter \"svc_cv output_cfg\" for the detail.\n");
    PrintFunc(" feeder [cmd] [Param]: CvFlow OutputCfg structure.\n");
    PrintFunc("     debug [0/1]: Enable debug log\n");
    PrintFunc(" fifo [cmd] [Param]: CvFlow OutputCfg structure.\n");
    PrintFunc("     debug [0/1]: Enable debug log\n");
    PrintFunc(" stereo dump_fus_dsi [file_path][count] : dump DSI result to SD card \n");
    PrintFunc("        dump_fus_pgm [file_path][count] : dump PGM result to SD card \n");
    PrintFunc("        set_osd_switch [dsi/stixel] : show dsi / stixel result on osd \n");
    PrintFunc("        osd  [on/off] : dsi osd on/off \n");
    PrintFunc(" stixel set_road_estimate [cvflow] [auto/manual] : set road estimate mode as auto/manual \n");
    PrintFunc("        osd [on/off]  : stixel osd on/off \n");
}

static void CmdCvEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32               Rval = SVC_OK;

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("bootflag", pArgVector[1U])) {
            if (2U < ArgCount) {
                SVC_USER_PREF_s *pSvcUserPref;
                UINT32          PrefBufSize;
                ULONG           PrefBufAddr;
                SVC_CV_MAIN_BOOT_FLAG_s CVBootFlag = {0};
                UINT32          i = 0;

                Rval = SvcUserPref_Get(&pSvcUserPref);
                if (SVC_OK == Rval) {
                    Rval = AmbaWrap_memcpy(&CVBootFlag, &pSvcUserPref->CVBooFlag, sizeof(UINT32));
                    PrintFunc("Set Flag:\n");
                    for (i = 0; i < (ArgCount - 2U); i++) {
                        Rval |= AppCV_SetBootFlag(&CVBootFlag, pArgVector[2U + i], PrintFunc);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CMDCV, "SvcUserPref_Get() failed with %d", Rval, 0U);
                }

                if (SVC_OK == Rval) {
                    Rval = AmbaWrap_memcpy(&pSvcUserPref->CVBooFlag, &CVBootFlag, sizeof(UINT32));
                    SvcLog_DBG(SVC_LOG_CMDCV, "CVBooFlag = 0x%x", pSvcUserPref->CVBooFlag, 0U);
#ifndef CONFIG_QNX
                    AmbaPrint_Flush();
#endif

                    SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                    Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_CMDCV, "SvcUserPref_Save() failed with %d", Rval, 0U);
                    } else {
                        SvcLog_DBG(SVC_LOG_CMDCV, "Rebooting......", 0U, 0U);
                        if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                           SvcLog_NG(SVC_LOG_CMDCV, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                        }
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#if defined(CONFIG_ICAM_REBEL_CUSTOMER_NN)
        } else if (0 == SvcWrap_strcmp("all_od_info", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                SvcODDrawTask_Rebel_ShawAllODInfo(1U);
            } else {
                SvcODDrawTask_Rebel_ShawAllODInfo(0U);
            }
#endif
        } else if (0 == SvcWrap_strcmp("continue", pArgVector[1U])) {
            if (2U == ArgCount) {
                (void) SvcCvMainTask_Ctrl("continue", NULL);
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("schdr", pArgVector[1U])) {
            if (3U < ArgCount) {
                SVC_USER_PREF_s *pSvcUserPref;
                UINT32          PrefBufSize;
                ULONG           PrefBufAddr;

                if (0 == SvcWrap_strcmp("log", pArgVector[2U])) {
                    UINT32 ShowSchdrLog = 0U;

                    Rval = SvcUserPref_Get(&pSvcUserPref);
                    if (SVC_OK == Rval) {
                        if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                            ShowSchdrLog = 1U;
                        } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                            ShowSchdrLog = 0U;
                        } else {
                            Rval = SVC_NG;
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_CMDCV, "SvcUserPref_Get() failed with %d", Rval, 0U);
                    }

                    if (SVC_OK == Rval) {
                        pSvcUserPref->CVSchdrLog = (UINT8)ShowSchdrLog;
                        SvcLog_DBG(SVC_LOG_CMDCV, "ShowSchdrLog = 0x%x", ShowSchdrLog, 0U);
#ifndef CONFIG_QNX
                        AmbaPrint_Flush();
#endif

                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);
                        Rval = SvcPref_Save(PrefBufAddr, PrefBufSize);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_CMDCV, "SvcUserPref_Save() failed with %d", Rval, 0U);
                        } else {
                            SvcLog_DBG(SVC_LOG_CMDCV, "Rebooting......", 0U, 0U);
                            if (SYS_ERR_NONE != AmbaSYS_Reboot()) {
                               SvcLog_NG(SVC_LOG_CMDCV, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                            }
                        }
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("mem",pArgVector[1U])) {
            SvcCvFlow_DumpBufUsage();
            Rval = SVC_OK;
        } else if (0 == SvcWrap_strcmp("fin", pArgVector[1U])) {
            SVC_CV_FILE_IN_CONFIG_s Config = {0};
            SVC_CV_FILE_IN_INPUT_INFO_s InputInfo;
            SVC_CV_FILE_IN_FILE_INFO_s FileInfo;
            SVC_CV_FILE_IN_START_s StartCfg = {0};
            SVC_CV_FILE_IN_NAMING_CONFIG_s NamingCfg = {0};
            UINT32 i = 0U;

            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("config", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void) SvcODDrawTask_Stop();

                        (void) SvcWrap_strtoul(pArgVector[3U], &Config.CvFlowChan);
                        if (4U < ArgCount) {
                            (void) SvcWrap_strtoul(pArgVector[4U], &Config.Mode);
                        }
                        (void) SvcCvFileInTask_Config(&Config);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("input_info", pArgVector[2U])) {
                    if (9U < ArgCount) {
                        (void) SvcWrap_strtoul(pArgVector[3U], &InputInfo.InputIdx);
                        (void) SvcWrap_strtoul(pArgVector[4U], &InputInfo.DataIdx);
                        (void) SvcWrap_strtoul(pArgVector[5U], &InputInfo.DataType);
                        (void) SvcWrap_strtoul(pArgVector[6U], &InputInfo.DataInfo.Img.Width);
                        (void) SvcWrap_strtoul(pArgVector[7U], &InputInfo.DataInfo.Img.Height);
                        (void) SvcWrap_strtoul(pArgVector[8U], &InputInfo.DataInfo.Img.Pitch);
                        (void) SvcWrap_strtoul(pArgVector[9U], &InputInfo.DataInfo.Img.ScaleIdx);

                        Rval = SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_INPUT_INFO, &InputInfo);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("file_info", pArgVector[2U])) {
                    if (5U < ArgCount) {
                        (void) SvcWrap_strtoul(pArgVector[3U], &FileInfo.InputIdx);
                        (void) SvcWrap_strtoul(pArgVector[4U], &FileInfo.DataIdx);
                        SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[5U]);

                        Rval = SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_FILE_INFO, &FileInfo);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                    if (3U == ArgCount) {
                        (void) SvcCvFileInTask_Start(&StartCfg);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("dir_info", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        SvcWrap_strcpy(NamingCfg.FileNameBuf[0], sizeof(NamingCfg.FileNameBuf[0]), pArgVector[3U]);
                        SvcWrap_strcpy(NamingCfg.FileNameBuf[1], sizeof(NamingCfg.FileNameBuf[1]), pArgVector[4U]);
                        if (5U < ArgCount) {
                            (void) SvcWrap_strtoul(pArgVector[5U], &NamingCfg.MaxNumber);
                        } else {
                            NamingCfg.MaxNumber = 0xffffU;
                        }

                        Rval = SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_BATCH_FILE_INFO, &NamingCfg);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("output_info", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void) SvcWrap_strtoul(pArgVector[3U], &(NamingCfg.MaxNumber));
                        if (ArgCount == (4U + NamingCfg.MaxNumber)) {
                            for(i = 4U; i < ArgCount; i++) {
                                SvcWrap_strcpy(NamingCfg.FileNameBuf[(i - 4U)], sizeof(NamingCfg.FileNameBuf[(i - 4U)]), pArgVector[i]);
                            }

                            Rval = SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_OUPUT_INFO, &NamingCfg);
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "OutNumber and given output Name Number does not match",0U ,0U);
                        }
                    } else {
                        Rval = SVC_NG;
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("cfg_num", pArgVector[1U])) {
            if (2U < ArgCount) {
                SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
                UINT32 CvFlowNum = 0U, CvFlowBits;

                (void) SvcWrap_strtoul(pArgVector[2U], &CvFlowNum);
                CvFlowBits = ((UINT32) 1U << CvFlowNum) - 1U;

                pCfg->CvFlowNum = CvFlowNum;
                pCfg->CvFlowBits = CvFlowBits;

                Rval = SVC_OK;
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("cfg", pArgVector[1U])) {
            if (6U < ArgCount) {
                SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
                UINT32 Idx = 0U, Chan = 0U, CvModeID = 0U, CvFlowType = 0U;
                UINT64 CvObjAddr;
                void *U64ToPtr;

                AmbaMisra_TouchUnused(&U64ToPtr);

                (void) SvcWrap_strtoul(pArgVector[2U], &Idx);
                (void) SvcWrap_strtoul(pArgVector[3U], &Chan);
                (void) SvcWrap_strtoull(pArgVector[4U], &CvObjAddr);
                (void) SvcWrap_strtoul(pArgVector[5U], &CvModeID);
                (void) SvcWrap_strtoul(pArgVector[6U], &CvFlowType);

                AmbaMisra_TypeCast(&U64ToPtr, &CvObjAddr);

                pCfg->CvFlow[Idx].Chan = Chan;
                pCfg->CvFlow[Idx].pCvObj = U64ToPtr;
                pCfg->CvFlow[Idx].CvModeID = CvModeID;
                pCfg->CvFlow[Idx].CvFlowType = CvFlowType;

                Rval = SVC_OK;
            } else {
                UINT32 PtrToU32;
                const void *U32ToPtr;

                AmbaMisra_TouchUnused(&PtrToU32);
                AmbaMisra_TouchUnused(&U32ToPtr);

                SvcLog_DBG(SVC_LOG_CMDCV, "svc_cv cfg", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Idx]:       ""\033""[m""Index for SVC_CV_FLOW_s structure. Maximum will be %u.", SVC_RES_CV_FLOW_NUM, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Chan]:      ""\033""[m""Channel for CV", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [CvObjAddr]: ""\033""[m""Current support CvFlow driver address ...", 0U, 0U);
#if defined(CONFIG_ICAM_CV_FLEXIDAG)
                U32ToPtr = &SvcCvFlow_AmbaODObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_AmbaODObj        = ""\033""[m""0x%x", PtrToU32, 0U);
                U32ToPtr = &SvcCvFlow_AmbaSegObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_AmbaSegObj       = ""\033""[m""0x%x", PtrToU32, 0U);
                U32ToPtr = &SvcCvFlow_RefODObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_RefODObj         = ""\033""[m""0x%x", PtrToU32, 0U);
                U32ToPtr = &SvcCvFlow_RefSegObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_RefSegObj        = ""\033""[m""0x%x", PtrToU32, 0U);
#endif
#if defined(CONFIG_ICAM_PROJECT_CNNTESTBED)
                U32ToPtr = &SvcCvFlow_CnnTestbedObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_CnnTestbedObj    = ""\033""[m""0x%x", PtrToU32, 0U);
#if defined(CONFIG_AMBALINK_BOOT_OS) || defined(CONFIG_QNX)
                U32ToPtr = &SvcCvFlow_CnnTestbedIpcObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_CnnTestbedIpcObj = ""\033""[m""0x%x", PtrToU32, 0U);
#endif
#endif
#if defined(CONFIG_ICAM_CV_STEREO)
                U32ToPtr = &SvcCvFlow_StereoObj;
                AmbaMisra_TypeCast(&PtrToU32, &U32ToPtr);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SvcCvFlow_StereoObj        = ""\033""[m""0x%x", PtrToU32, 0U);
#endif
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [CvModeID]:  ""\033""[m"" Cv mode for the driver", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [CvFlowType]:""\033""[m"" Cv flow type. Current support type ...", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_REF_OD       =""\033""[m"" %u", SVC_CV_FLOW_REF_OD, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_REF_SEG      =""\033""[m"" %u", SVC_CV_FLOW_REF_SEG, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_AMBA_OD      =""\033""[m"" %u", SVC_CV_FLOW_AMBA_OD, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_AMBA_SEG     =""\033""[m"" %u", SVC_CV_FLOW_AMBA_SEG, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_STEREO       =""\033""[m"" %u", SVC_CV_FLOW_STEREO, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;34m""    SVC_CV_FLOW_CNN_TESTBED  =""\033""[m"" %u", SVC_CV_FLOW_CNN_TESTBED, 0U);

                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("input_cfg", pArgVector[1U])) {
            UINT32 ValidArgCnt = 5U;    /* "cv" + "input_cfg" + "Idx" + "Src_Rate" + "InputNum" */
            if (ValidArgCnt <= ArgCount) {
                SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
                UINT32 Idx = 0U, SrcRate = 0U, InputNum = 0U, DataSrc = 0U, StrmId = 0U, FrameWidth = 0U, FrameHeight = 0U, RoiNum = 0U, Index = 0U, StartX = 0U, StartY = 0U, Rsrv0 = 0U, Rsrv1 = 0U;
                UINT32 ArgIdx = 2U;

                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &Idx);
                ArgIdx = ArgIdx + 1U;
                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &SrcRate);
                ArgIdx = ArgIdx + 1U;
                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &InputNum);
                ArgIdx = ArgIdx + 1U;

                pCfg->CvFlow[Idx].InputCfg.SrcRate  = SrcRate;
                pCfg->CvFlow[Idx].InputCfg.InputNum = (UINT16) InputNum;

                ValidArgCnt += 5U * InputNum;   /* + "DataSrc" + "StrmId" + "FrameWidth" + "FrameHeight" + "RoiNum" */

                if (ValidArgCnt <= ArgCount) {
                    for (UINT32 i = 0U; i < InputNum; i++) {
                        (void) SvcWrap_strtoul(pArgVector[ArgIdx], &DataSrc);
                        ArgIdx = ArgIdx + 1U;
                        (void) SvcWrap_strtoul(pArgVector[ArgIdx], &StrmId);
                        ArgIdx = ArgIdx + 1U;
                        (void) SvcWrap_strtoul(pArgVector[ArgIdx], &FrameWidth);
                        ArgIdx = ArgIdx + 1U;
                        (void) SvcWrap_strtoul(pArgVector[ArgIdx], &FrameHeight);
                        ArgIdx = ArgIdx + 1U;
                        (void) SvcWrap_strtoul(pArgVector[ArgIdx], &RoiNum);
                        ArgIdx = ArgIdx + 1U;

                        pCfg->CvFlow[Idx].InputCfg.Input[i].DataSrc     = (UINT16) DataSrc;
                        pCfg->CvFlow[Idx].InputCfg.Input[i].StrmId      = StrmId;
                        pCfg->CvFlow[Idx].InputCfg.Input[i].FrameWidth  = (UINT16)  FrameWidth;
                        pCfg->CvFlow[Idx].InputCfg.Input[i].FrameHeight = (UINT16)  FrameHeight;
                        pCfg->CvFlow[Idx].InputCfg.Input[i].NumRoi      = (UINT16)  RoiNum;

                        ValidArgCnt += 5U * RoiNum; /* + "Index" + "StartX" + "StartY" + "Rsrv0" + "Rsrv1" */

                        if (ValidArgCnt <= ArgCount) {
                            for (UINT32 j = 0U; j < RoiNum; j++) {
                                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &Index);
                                ArgIdx = ArgIdx + 1U;
                                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &StartX);
                                ArgIdx = ArgIdx + 1U;
                                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &StartY);
                                ArgIdx = ArgIdx + 1U;
                                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &Rsrv0);
                                ArgIdx = ArgIdx + 1U;
                                (void) SvcWrap_strtoul(pArgVector[ArgIdx], &Rsrv1);
                                ArgIdx = ArgIdx + 1U;

                                pCfg->CvFlow[Idx].InputCfg.Input[i].Roi[j].Index     = (UINT16) Index;
                                pCfg->CvFlow[Idx].InputCfg.Input[i].Roi[j].StartX    = StartX;
                                pCfg->CvFlow[Idx].InputCfg.Input[i].Roi[j].StartY    = StartY;
                                pCfg->CvFlow[Idx].InputCfg.Input[i].Roi[j].Reserved0 = Rsrv0;
                                pCfg->CvFlow[Idx].InputCfg.Input[i].Roi[j].Reserved1 = Rsrv1;
                            }
                        }
                    }
                }

                Rval = SVC_OK;
            } else {
                SvcLog_DBG(SVC_LOG_CMDCV, "svc_cv input_cfg", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Idx]     : ""\033""[m"" Index for SVC_CV_FLOW_s structure. Maximum will be %u.", SVC_RES_CV_FLOW_NUM, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Src_Rate]: ""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [InputNum]: ""\033""[m""If InputNum == 2, Input structure must be specified like below. Maximum will be %u", SVC_RES_CV_MAX_INPUT_NUM, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[0].DataSrc]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[0].StrmId]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[0].FrameWidth]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[0].FrameHeight]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[0].RoiNum]: ""\033""[m""If RoiNum == n, Roi structure must be specified like below. Maximum will be %u", SVC_RES_CV_MAX_ROI, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].Index]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].StartX]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].StartY]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].Rsrv0]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].Rsrv1]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[1].Index]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[1].StartX]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[1].StartY]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[1].Rsrv0]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[1].Rsrv1]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi...]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[n-1].Rsrv1]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[1].DataSrc]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[1].StrmId]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[1].FrameWidth]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[1].FrameHeight]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""    [Input[1].RoiNum]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].Index]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[0].StartX]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi...]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""      [Roi[m].Rsrv1]""\033""[m", 0U, 0U);

                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("output_cfg", pArgVector[1U])) {
            if (4U < ArgCount) {    /* "cv" + "output_cfg" + "Idx" + "tag1" + "Tag1" */
                SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
                UINT32 Idx = 0U, Tag0 = 0U, Tag1 = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &Idx);
                (void) SvcWrap_strtoul(pArgVector[3U], &Tag0);
                (void) SvcWrap_strtoul(pArgVector[4U], &Tag1);

                pCfg->CvFlow[Idx].OutputCfg.OutputTag[0] = Tag0;
                pCfg->CvFlow[Idx].OutputCfg.OutputTag[1] = Tag1;
            } else {
                SvcLog_DBG(SVC_LOG_CMDCV, "svc_app cv output_cfg", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Idx]: ""\033""[m""Index for SVC_CV_FLOW_s structure. Maximum will be %u.", SVC_RES_CV_FLOW_NUM, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Tag0]""\033""[m", 0U, 0U);
                SvcLog_DBG(SVC_LOG_CMDCV, "\033""[0;32;32m""  [Tag1]""\033""[m", 0U, 0U);
                Rval = SVC_OK;
            }
        } else if (0 == SvcWrap_strcmp("load", pArgVector[1U])) {
            UINT32 CvfChan = 0, i;
            UINT32 BinType;
            SVC_CV_FLOW_LOAD_INFO_s LoadInfo;
            char *pFileName;

            if (4U < ArgCount) {
                if (0 == SvcWrap_strcmp("flexibin", pArgVector[2U])) {
                    BinType = SVC_CV_FLOW_FLEXI_BIN;
                } else if (0 == SvcWrap_strcmp("lcs", pArgVector[2U])) {
                    BinType = SVC_CV_FLOW_LICENSE_BIN;
                } else if (0 == SvcWrap_strcmp("ext", pArgVector[2U])) {
                    BinType = SVC_CV_FLOW_EXT_BIN;
                } else {
                    BinType = 0xffU;
                }

                if (BinType != 0xffU) {
                    AmbaSvcWrap_MisraMemset(&LoadInfo, 0, sizeof(SVC_CV_FLOW_LOAD_INFO_s));

                    (void) SvcWrap_strtoul(pArgVector[3U], &CvfChan);
                    LoadInfo.AlgoIdx     = 0U;
                    LoadInfo.StorageType = SVC_CV_FLOW_STORAGE_TYPE_SD;
                    for (i = 0; i < (ArgCount - 4U); i++) {
                        if (i < SVC_CV_FLOW_MAX_BIN_NUM) {
                            LoadInfo.BinType[i]  = BinType;
                            AmbaMisra_TypeCast(&pFileName, &pArgVector[4U + i]);
                            LoadInfo.pFileName[i] = pFileName;
                            LoadInfo.NumBin++;
                        }
                    }
                    (void) SvcCvFlow_Load(CvfChan, &LoadInfo);
                    Rval = SVC_OK;
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("config", pArgVector[1U])) {
                SvcLog_OK(SVC_LOG_CMDCV, "cv config flow start", 0U, 0U);

            if (SVC_OK != SvcCvCtrlTask_Config()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvCtrlTask_Config() failed", 0U, 0U);
            }

            if (SVC_OK != SvcCvCtrlTask_Load()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvCtrlTask_Load() failed", 0U, 0U);
            }

            if (SVC_OK != SvcCvMainTask_Load()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvMainTask_Load() failed", 0U, 0U);
            }

            /* Start CV scheduler */
            if (SVC_OK != SvcCvMainTask_Start()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvMainTask_Start() failed", 0U, 0U);
            }

            SvcLog_OK(SVC_LOG_CMDCV, "cv config flow done", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("restart", pArgVector[1U])) {
            SvcLog_OK(SVC_LOG_CMDCV, "cv restart flow start", 0U, 0U);

            (void) SvcCvMainTask_WaitSrcReady();

            if (SVC_OK != SvcCvCtrlTask_Start()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvCtrlTask_Start() failed", 0U, 0U);
            }

            if (SVC_OK != SvcODDrawTask_Config(0)) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcODDrawTask_Config() failed", 0U, 0U);
            }

            if (SVC_OK != SvcODDrawTask_Start()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcODDrawTask_Start() failed", 0U, 0U);
            }

            if (SVC_OK != SvcSegDrawTask_Init()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcSegDrawTask_Init() failed", 0U, 0U);
            }

            if (SVC_OK != SvcSegDrawTask_Start()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcSegDrawTask_Start() failed", 0U, 0U);
            }

            if (SVC_OK != SvcExtOsdDrawTask_Start()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcExtOsdDrawTask_Start() failed", 0U, 0U);
            }

            SvcLog_OK(SVC_LOG_CMDCV, "cv restart flow done", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            SvcLog_OK(SVC_LOG_CMDCV, "cv stop flow start", 0U, 0U);

            if (SVC_OK != SvcCvCtrlTask_Stop()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvCtrlTask_Stop() failed", 0U, 0U);
            }

            if (SVC_OK != SvcSegDrawTask_Stop()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcSegDrawTask_Start() failed", 0U, 0U);
            }

            if (SVC_OK != SvcODDrawTask_Stop()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcODDrawTask_Stop() failed", 0U, 0U);
            }

            if (SVC_OK != SvcExtOsdDrawTask_Stop()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcExtOsdDrawTask_Stop() failed", 0U, 0U);
            }

            if (SVC_OK != SvcCvMainTask_Stop()) {
                SvcLog_NG(SVC_LOG_CMDCV, "SvcCvMainTask_Stop() failed", 0U, 0U);
            }

            SvcLog_OK(SVC_LOG_CMDCV, "cv stop flow done", 0U, 0U);
        } else if (0 == SvcWrap_strcmp("drv", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Cvflowchan = 0U, Value, Debug = 0U;

                if (0 == SvcWrap_strcmp("dump", pArgVector[2U])) {
                    Debug = 1U;
                }

                if ((0 == SvcWrap_strcmp("debug", pArgVector[2U])) || (1U == Debug)) {
                    if (3U < ArgCount) {
                        #define MAX_PARAM_NUM  (3U)
                        UINT32 Param[MAX_PARAM_NUM] = {0};
                        UINT32 i;

                        /* svc_cv drvdbg [cmd] [cvflowchan] [Param0-2] */
                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        for (i = 0; (i < (ArgCount - 4U)) && (i < MAX_PARAM_NUM); i++) {
                            (void)SvcWrap_strtoul(pArgVector[4U + i], &Param[i]);
                        }

                        (void)SvcCvFlow_Debug(Cvflowchan, pArgVector[2U], Param[0], Param[1], Param[2]);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("setrate", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_SET_SRC_RATE, &Value);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): setrate = %d done", Cvflowchan, Value);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("enable", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_ENABLE, &Value);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): enable = %d done", Cvflowchan, Value);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("scene_mode", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_SET_SCECNE_MODE, &Value);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): SceneMode = %d done", Cvflowchan, Value);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("tuning", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_ENABLE_TUNING_MODE, &Value);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): TuningMode = %d done", Cvflowchan, Value);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("setroi", pArgVector[2U])) {
                    if (5U < ArgCount) {
                        SVC_CV_ROI_INFO_s RoiInfo = {0};

                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);

                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
                        if (SVC_OK == Rval) {
                            AmbaPrint_PrintUInt5("Roi[%u] = (%u, %u, %u, %u)",
                                0U, RoiInfo.Roi[0].StartX, RoiInfo.Roi[0].StartY, RoiInfo.Roi[0].Width, RoiInfo.Roi[0].Height);
                        }

                        (void)SvcWrap_strtoul(pArgVector[4U], &RoiInfo.Roi[0].StartX);
                        (void)SvcWrap_strtoul(pArgVector[5U], &RoiInfo.Roi[0].StartY);
                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_SET_ROI_INFO, &RoiInfo);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): SET_ROI_INFO done", Cvflowchan, 0U);
                            AmbaPrint_PrintUInt5("Roi[%u] = (%u, %u, %u, %u)",
                                0U, RoiInfo.Roi[0].StartX, RoiInfo.Roi[0].StartY, RoiInfo.Roi[0].Width, RoiInfo.Roi[0].Height);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("frate", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        SVC_CV_FRAME_TIME_INFO_s FTInfo = {0};

                        (void)SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);

                        Rval = SvcCvFlow_Control(Cvflowchan, SVC_CV_CTRL_GET_OUTPUT_FRAME_TIME, &FTInfo);
                        if (SVC_OK == Rval) {
                            DOUBLE FRate;
                            char StrFRate[32U];

                            if (FTInfo.FrameTime != 0U) {
                                FRate = (DOUBLE)1000000 / (DOUBLE)FTInfo.FrameTime;
                            } else {
                                FRate = 0.0;
                            }

                            StrFRate[0] = '\0';
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d):", Cvflowchan, 0U);
                            if (0U != AmbaUtility_DoubleToStr(&StrFRate[0U], (UINT32)sizeof(StrFRate), FRate, 2U)) { /* none */ }
                            AmbaPrint_PrintStr5("Frame Rate: %s FPS", StrFRate, NULL, NULL, NULL, NULL);

                            AmbaPrint_PrintUInt5("Proc Time(FD): %u ms, Proc Time(total): %u ms",
                                (FTInfo.ProcTimeFD / 1000U), (FTInfo.ProcTimeAll / 1000U), 0U, 0U, 0U);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("ssdnms", pArgVector[2U])) {
                    if (14U < ArgCount) {
                        SSD_NMS_CTRL_s SsdNmsCtrl;

                        (void) SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void) SvcWrap_strtoul(pArgVector[4U], &SsdNmsCtrl.ClassNum);
                        SvcWrap_strcpy(SsdNmsCtrl.PriorBox, MAX_PBOX_FN_LEN, pArgVector[5U]);
                        (void) SvcWrap_strtoul(pArgVector[6U], &SsdNmsCtrl.Win.RoiWidth);
                        (void) SvcWrap_strtoul(pArgVector[7U], &SsdNmsCtrl.Win.RoiHeight);
                        (void) SvcWrap_strtoul(pArgVector[8U], &SsdNmsCtrl.Win.RoiStartX);
                        (void) SvcWrap_strtoul(pArgVector[9U], &SsdNmsCtrl.Win.RoiStartY);
                        (void) SvcWrap_strtoul(pArgVector[10U], &SsdNmsCtrl.Win.NetworkWidth);
                        (void) SvcWrap_strtoul(pArgVector[11U], &SsdNmsCtrl.Win.NetworkHeight);
                        (void) SvcWrap_strtoul(pArgVector[12U], &SsdNmsCtrl.Win.VisWidth);
                        (void) SvcWrap_strtoul(pArgVector[13U], &SsdNmsCtrl.Win.VisHeight);
                        (void) SvcWrap_strtoul(pArgVector[14U], &SsdNmsCtrl.ModelType);

                        Rval = SvcCvFlow_Control(Cvflowchan, TESTBED_CVALGO_CTRL_SET_SSD_NMS, &SsdNmsCtrl);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): Set SSD NMS done", Cvflowchan, 0U);
                        }
                    }
                } else if (0 == SvcWrap_strcmp("segwin", pArgVector[2U])) {
                    if (9U < ArgCount) {
                        SEG_WIN_CTRL_s SegWinCtrl;

                        (void) SvcWrap_strtoul(pArgVector[3U], &Cvflowchan);
                        (void) SvcWrap_strtoul(pArgVector[4U], &SegWinCtrl.RoiWidth);
                        (void) SvcWrap_strtoul(pArgVector[5U], &SegWinCtrl.RoiHeight);
                        (void) SvcWrap_strtoul(pArgVector[6U], &SegWinCtrl.RoiStartX);
                        (void) SvcWrap_strtoul(pArgVector[7U], &SegWinCtrl.RoiStartY);
                        (void) SvcWrap_strtoul(pArgVector[8U], &SegWinCtrl.NetworkWidth);
                        (void) SvcWrap_strtoul(pArgVector[9U], &SegWinCtrl.NetworkHeight);

                        Rval = SvcCvFlow_Control(Cvflowchan, TESTBED_CVALGO_CTRL_SET_SEG_WIN, &SegWinCtrl);
                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChan(%d): Set SEG Window done", Cvflowchan, 0U);
                        }
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("oddraw", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32 CmdType, Value = 0;
                if (0 == SvcWrap_strcmp("category_label", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("score_label", pArgVector[2U])) {
                    CmdType = 1U;
                    if (4U < ArgCount) {
                        (void) SvcWrap_strtoul(pArgVector[4U], &Value);
                    }
                } else if (0 == SvcWrap_strcmp("detres", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                        CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("roi", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("kp", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("mask", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("category_clf", pArgVector[2U])) {
                    CmdType = 1U;
                } else if (0 == SvcWrap_strcmp("bbx_allow_bits", pArgVector[2U])) {
                    CmdType = 2U;
                    (void) SvcWrap_strtoul(pArgVector[3U], &Value);
                } else {
                    CmdType = 0U;
                }

                if (CmdType == 1U) {
                    if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                        if (4U < ArgCount) {
                            (void)SvcODDrawTask_Ctrl(pArgVector[2U], Value);
                        } else {
                            (void)SvcODDrawTask_Ctrl(pArgVector[2U], 1U);
                        }
                    } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                        (void)SvcODDrawTask_Ctrl(pArgVector[2U], 0U);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (CmdType == 2U) {
                    (void)SvcODDrawTask_Ctrl(pArgVector[2U], Value);
                } else {
                    /* Do nothing */
                }
            } else {
                Rval = SVC_NG;
            }
#if defined(CONFIG_ICAM_CV_LOGGER)
        } else if (0 == SvcWrap_strcmp("logger", pArgVector[1U])) {
            UINT32 CvFlowChanBits = 0x1;
            UINT32 Value = 0U;

            if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {
                if (3U < ArgCount) {
                    (void) SvcWrap_strtoul(pArgVector[3U], &CvFlowChanBits);
                }
                SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChanBits(%x)", CvFlowChanBits, 0U);
                (void) SvcCvLoggerTask_Start(CvFlowChanBits);
            } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])) {
                if (3U < ArgCount) {
                    (void) SvcWrap_strtoul(pArgVector[3U], &CvFlowChanBits);
                }
                SvcLog_OK(SVC_LOG_CMDCV, "CvFlowChanBits(%x)", CvFlowChanBits, 0U);
                (void) SvcCvLoggerTask_Stop(CvFlowChanBits);
            } else if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                if (3U < ArgCount) {
                    (void)SvcWrap_strtoul(pArgVector[3U], &Value);
                    (void)SvcCvLogger_DebugEnable(Value);
                }
            } else if (0 == SvcWrap_strcmp("seg_save_num", pArgVector[2U])) {
                if (3U < ArgCount) {
                    (void)SvcWrap_strtoul(pArgVector[3U], &Value);
                    if ((Value > 0U) && (Value <= 4U)) {
                        SvcSegDrawTask_SetSaveBufferNum(Value);
                    } else {
                        SvcLog_OK(SVC_LOG_CMDCV, "invalid number", Value, 0U);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#endif
        } else if (0 == SvcWrap_strcmp("feeder", pArgVector[1U])) {
            UINT32 FeederID = 0U;
            SVC_IMG_FEEDER_PORT_s Port;
            UINT32 Value = 0U;

            if (0 == SvcWrap_strcmp("create", pArgVector[2U])) {
                SVC_IMG_FEEDER_CREATE_CFG_s Cfg = {0};

                (void)SvcWrap_strtoul(pArgVector[3U], &FeederID);
                Cfg.TaskPriority = 80U;
                Cfg.FeederFlag   = 0U;
                (void)SvcImgFeeder_Create(FeederID, &Cfg);

            } else if (0 == SvcWrap_strcmp("enable", pArgVector[2U])) {
              (void)SvcWrap_strtoul(pArgVector[3U], &FeederID);
              (void)SvcWrap_strtoul(pArgVector[4U], &Value);

              (void)SvcImgFeeder_Ctrl(FeederID, SVC_IMG_FEEDER_CTRL_ENABLE, &Value);

            } else if (0 == SvcWrap_strcmp("open1", pArgVector[2U])) {
                SVC_IMG_FEEDER_PORT_CFG_s Cfg = {0};

                Cfg.NumPath = 1U;
                (void)SvcWrap_strtoul(pArgVector[3U], &FeederID);
                (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                Cfg.Content[0].DataSrc = (UINT16)Value;
                (void)SvcWrap_strtoul(pArgVector[5U], &Cfg.Content[0].StrmId);
                Cfg.SendFunc = AppCV_FeederCallback;

                (void)SvcImgFeeder_OpenPort(FeederID, &Cfg, &Port);
                AmbaPrint_PrintUInt5("Open1: Feeder(%d) Port(%d) DataSrc(%d) StrmId(%d)",
                    Port.FeederID, Port.PortID, Cfg.Content[0].DataSrc, Cfg.Content[0].StrmId, 0U);
            } else if (0 == SvcWrap_strcmp("open2", pArgVector[2U])) {
                SVC_IMG_FEEDER_PORT_CFG_s Cfg = {0};

                Cfg.NumPath = 2U;
                (void)SvcWrap_strtoul(pArgVector[3U], &FeederID);
                (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                Cfg.Content[0].DataSrc = (UINT16)Value;
                (void)SvcWrap_strtoul(pArgVector[5U], &Cfg.Content[0].StrmId);
                (void)SvcWrap_strtoul(pArgVector[6U], &Value);
                Cfg.Content[1].DataSrc = (UINT16)Value;
                (void)SvcWrap_strtoul(pArgVector[7U], &Cfg.Content[1].StrmId);
                Cfg.SendFunc = AppCV_FeederCallback;

                (void) SvcImgFeeder_OpenPort(FeederID, &Cfg, &Port);
                AmbaPrint_PrintUInt5("Open2: Feeder(%d) Port(%d) DataSrc(%d) StrmId(%d)",
                    Port.FeederID, Port.PortID, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("Open2: [0]: DataSrc(%d) StrmId(%d) [1]: DataSrc(%d) StrmId(%d)",
                    Cfg.Content[0].DataSrc, Cfg.Content[0].StrmId, Cfg.Content[1].DataSrc, Cfg.Content[1].StrmId, 0U);
            } else if (0 == SvcWrap_strcmp("close", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Port.FeederID);
                (void)SvcWrap_strtoul(pArgVector[4U], &Port.PortID);

                (void)SvcImgFeeder_ClosePort(&Port);
                AmbaPrint_PrintUInt5("Close: Feeder(%d) Port(%d)",
                    Port.FeederID, Port.PortID, 0U, 0U, 0U);

            } else if (0 == SvcWrap_strcmp("conn", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Port.FeederID);
                (void)SvcWrap_strtoul(pArgVector[4U], &Port.PortID);

                (void)SvcImgFeeder_Connect(&Port);
            } else if (0 == SvcWrap_strcmp("disconn", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Port.FeederID);
                (void)SvcWrap_strtoul(pArgVector[4U], &Port.PortID);

                (void) SvcImgFeeder_Disconnect(&Port);

            } else if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Value);
                (void) SvcImgFeeder_DebugEnable(Value);
            } else {
                Rval = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("fifo", pArgVector[1U])) {
            UINT32 Value[2] = {0};
            SVC_IMG_FIFO_INIT_CFG_s InitCfg = {0};
            SVC_IMG_FIFO_CFG_s FIFOCfg = {0};
            SVC_IMG_FIFO_CHAN_s Chan;

            if (0 == SvcWrap_strcmp("debug", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Value[0]);
                (void) SvcImgFIFO_DebugEnable(Value[0]);
            } else if (0 == SvcWrap_strcmp("init", pArgVector[2U])) {
                (void) SvcImgFIFO_Init(&InitCfg);
            } else if (0 == SvcWrap_strcmp("test", pArgVector[2U])) {
                (void)SvcWrap_strtoul(pArgVector[3U], &Value[0]);
                (void)SvcWrap_strtoul(pArgVector[4U], &Value[1]);

                FIFOCfg.Content.DataSrc = (UINT16)Value[0];
                FIFOCfg.Content.StrmId  = Value[1];
                FIFOCfg.pCallback       = AppCV_FIFOCallback;
                (void) SvcImgFIFO_Register(&FIFOCfg, &Chan);
            } else {
                /* Do nothing */
            }
#if defined(CONFIG_ICAM_CV_STEREO)
        } else if (0 == SvcWrap_strcmp("stereo", pArgVector[1U])) {
            SVC_CV_FILE_IN_FILE_INFO_s FileInfo;
            SVC_STEREO_CONFIG_s Stereo_Config = {0};
            UINT32 Value;
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("config_stereo", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void) SvcODDrawTask_Stop();

                        (void) SvcWrap_strtoul(pArgVector[3U], &Stereo_Config.CvFlowChan);
                        (void) SvcStereoTask_FileIn_Config(&Stereo_Config);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("startspubinary", pArgVector[2U])) {
                    if (2U < ArgCount) {
                        SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[3U]);
                        (void) SvcStereoTask_StartSpuFexBinary(FileInfo.FileName);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("startfusionbinary", pArgVector[2U])) {
                    UINT32 InputNum;
                    SVC_CV_FILE_IN_FILE_INFO_s FileInfo1, FileInfo2;

                    if (2U < ArgCount) {
                        (void) SvcWrap_strtoul(pArgVector[3U], &InputNum);
                        if (InputNum == 2U){
                            SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[4U]);
                            SvcWrap_strcpy(FileInfo1.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[5U]);
                        } else if (InputNum == 3U){
                            SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[4U]);
                            SvcWrap_strcpy(FileInfo1.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[5U]);
                            SvcWrap_strcpy(FileInfo2.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, pArgVector[6U]);
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "Invalid InputNum", 0U, 0U);
                        }
                        (void) SvcStereoTask_StartFusionBinary(InputNum, FileInfo.FileName, FileInfo1.FileName, FileInfo2.FileName);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("dump_fus_dsi", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        (void) SvcStereoTask_Ctrl(pArgVector[2U], pArgVector[3U], Value);
                    } else {
                        Rval = SVC_NG;
                    }
                }  else if (0 == SvcWrap_strcmp("dump_fus_pgm", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        (void) SvcStereoTask_Ctrl(pArgVector[2U], pArgVector[3U], Value);
                    } else {
                        Rval = SVC_NG;
                    }
                }   else if (0 == SvcWrap_strcmp("dump_spu", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value);
                        (void) SvcStereoTask_Ctrl(pArgVector[2U], pArgVector[3U], Value);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("10bto16b", pArgVector[2U])) {
                    const char *pFileNameIn = NULL, *pFileNameOut = NULL;
                    AMBA_FS_FILE *pFile;
                    ULONG  BufBase = 0U;
                    UINT32 BufSize = 0U;
                    void  *pSrc, *pDst;
                    UINT32 NumSuccess = 0U, SrcWidth = 0U, SrcHeight = 0U, SrcPitch = 0U, DstPitch = 0U;
                    extern UINT32 AmbaSTU_Unpack10BitsTo16Bits(const UINT8 *pIn10BitsTbl, UINT32 Width, UINT16 *pOut16BitsTbl);

                    if (7U < ArgCount) {
                        pFileNameIn = pArgVector[3U];
                        Rval = SvcWrap_strtoul(pArgVector[4U], &SrcWidth);
                        Rval = SvcWrap_strtoul(pArgVector[5U], &SrcHeight);
                        Rval = SvcWrap_strtoul(pArgVector[6U], &SrcPitch);
                        pFileNameOut = pArgVector[7U];

                        Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_TRANSIENT_BUF, &BufBase, &BufSize);
                        if (SVC_OK == Rval) {
                            Rval = AmbaFS_FileOpen(pFileNameIn, "r", &pFile);
                            if (SVC_OK == Rval) {
                                AmbaMisra_TypeCast(&pSrc, &BufBase);
                                Rval = AmbaFS_FileRead(pSrc, SrcPitch * SrcHeight, 1U, pFile, &NumSuccess);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDCV, "File read failed %d NumSuccess = %d", Rval, NumSuccess);
                                }
                                Rval = AmbaFS_FileClose(pFile);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDCV, "File close failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDCV, "File open failed %d", Rval, 0U);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "SvcBuffer_Request failed", Rval, 0U);
                        }
                    } else {
                        Rval = SVC_NG;
                    }

                    if (SVC_OK == Rval) {
                        BufBase += (BufSize >> 1U);
                        AmbaMisra_TypeCast(&pDst, &BufBase);
                        {
                            UINT32 k = 0;
                            UINT8 *pSrcUINT8;
                            UINT16 *pDstUINT16;
                            AmbaMisra_TypeCast(&pSrcUINT8, &pSrc);
                            AmbaMisra_TypeCast(&pDstUINT16, &pDst);
                            for(k = 0; k < SrcHeight; k++){
                                Rval = AmbaSTU_Unpack10BitsTo16Bits(pSrcUINT8, SrcWidth, pDstUINT16);
                                pSrcUINT8 = &pSrcUINT8[SrcPitch];
                                pDstUINT16 = &pDstUINT16[SrcWidth];
                                AmbaMisra_TypeCast(&pSrc, &pSrcUINT8);
                                AmbaMisra_TypeCast(&pDst, &pDstUINT16);
                            }
                        }
                        AmbaMisra_TypeCast(&pDst, &BufBase);
                        DstPitch  = ((((SrcWidth * 10U) / 8U) * 16U) / 10U);
                        if (SVC_OK == Rval) {
                            Rval = AmbaFS_FileOpen(pFileNameOut, "w+", &pFile);
                            if (SVC_OK == Rval) {
                                Rval = AmbaFS_FileWrite(pDst, DstPitch * SrcHeight, 1U, pFile, &NumSuccess);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDCV, "File Write failed %d NumSuccess = %d", Rval, NumSuccess);
                                }
                                Rval = AmbaFS_FileClose(pFile);
                                if (SVC_OK != Rval) {
                                    SvcLog_NG(SVC_LOG_CMDCV, "File close failed %d", Rval, 0U);
                                }
                            } else {
                                SvcLog_NG(SVC_LOG_CMDCV, "File open failed %d", Rval, 0U);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "10bCompactTo16b failed %d", Rval, 0U);
                        }

                        if (SVC_OK == Rval) {
                            SvcLog_OK(SVC_LOG_CMDCV, "Convert 10bto16b done", Rval, 0U);
                        }
                    }
#if (defined(CONFIG_ICAM_CV_STIXEL) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))
                } else if (0 == SvcWrap_strcmp("set_osd_switch", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        if (0 == SvcWrap_strcmp("dsi", pArgVector[3U])) {
                            SvcStereoTask_SetOsdEnable(SVC_STEREO_OSD_ENABLE);
                            SvcStixelTask_SetOsdEnable(SVC_STIXEL_OSD_DISABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "Switch OSD to Show DSI", 0U, 0U);
                        } else if (0 == SvcWrap_strcmp("stixel", pArgVector[3U])) {
                            SvcStereoTask_SetOsdEnable(SVC_STEREO_OSD_DISABLE);
                            SvcStixelTask_SetOsdEnable(SVC_STIXEL_OSD_ENABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "Switch OSD to Show Stixel", 0U, 0U);
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "Un-Support Mode", 0U, 0U);
                        }
                    }
#endif
                }  else if (0 == SvcWrap_strcmp("osd", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                            SvcStereoTask_SetOsdEnable(SVC_STEREO_OSD_ENABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "dsi osd on", 0U, 0U);
                        } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                            SvcStereoTask_SetOsdEnable(SVC_STEREO_OSD_DISABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "dsi osd off", 0U, 0U);
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "Un-Support Mode", 0U, 0U);
                        }
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
#endif
#if (defined(CONFIG_ICAM_CV_STIXEL) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))

        } else if (0 == SvcWrap_strcmp("stixel", pArgVector[1U])) {
            if (3U < ArgCount) {
                if (0 == SvcWrap_strcmp("send_dsi", pArgVector[2U])) {
                    SVC_STIXEL_TASK_FILE_IN_s FileIn = {0};
                    char FileName[64];

                    if (7U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &FileIn.CvFlowChan);
                        (void)SvcWrap_strcpy(FileName, 64, pArgVector[4U]);
                        FileIn.pFileName = FileName;
                        (void)SvcWrap_strtoul(pArgVector[5U], &FileIn.Width);
                        (void)SvcWrap_strtoul(pArgVector[6U], &FileIn.Height);
                        (void)SvcWrap_strtoul(pArgVector[7U], &FileIn.Pitch);

                        (void) SvcStixelTask_SendFileInput(&FileIn);
                    } else {
                        Rval = SVC_NG;
                    }
                } else if (0 == SvcWrap_strcmp("set_road_estimate", pArgVector[2U])) {
                    UINT32 CvFlowChan;
                    UINT16 RoadEstiMode;

                    (void)SvcWrap_strtoul(pArgVector[3U], &CvFlowChan);
                    if (0 == SvcWrap_strcmp("auto", pArgVector[4U])) {
                        RoadEstiMode = 0U;
                        SvcLog_NG(SVC_LOG_CMDCV, "auto mode", 0U, 0U);
                    } else if (0 == SvcWrap_strcmp("manual", pArgVector[4U])) {
                        RoadEstiMode = 1U;
                        SvcLog_NG(SVC_LOG_CMDCV, "manual mode", 0U, 0U);
                    } else {
                        RoadEstiMode = 0U;
                        SvcLog_NG(SVC_LOG_CMDCV, "unsupport mode", 0U, 0U);
                    }
                    Rval = StixelTask_SaveRoadEstimate(CvFlowChan, RoadEstiMode);
                } else if (0 == SvcWrap_strcmp("osd", pArgVector[2U])) {
                    if (3U < ArgCount) {
                        if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                            SvcStixelTask_SetOsdEnable(SVC_STIXEL_OSD_ENABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "stixel osd on", 0U, 0U);
                        } else if (0 == SvcWrap_strcmp("off", pArgVector[3U])) {
                            SvcStixelTask_SetOsdEnable(SVC_STIXEL_OSD_DISABLE);
                            SvcLog_OK(SVC_LOG_CMDCV, "stixel osd off", 0U, 0U);
                        } else {
                            SvcLog_NG(SVC_LOG_CMDCV, "Un-Support Mode", 0U, 0U);
                        }
                    }
                } else {
                    Rval = SVC_NG;
                }
            } else {
                Rval = SVC_NG;
            }
#endif
#if defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB)
        } else if (0 == SvcWrap_strcmp("stereo_ac", pArgVector[2U])) {
            if (3U < ArgCount) {
                UINT32 i, NumRun;
                SVC_CV_FILE_IN_FILE_INFO_s FileInfo;
                SVC_CV_FILE_IN_START_s StartCfg = {0};
                char FileNameLeft[] = {"c:\\201120_mvac_input\\warp_left_yaw_1d_2\\frames_"};
                char FileNameRight[] = {"c:\\201120_mvac_input\\warp_right_2\\frames_"};
                char IdxStr[8];
                extern UINT32 AmbaKAL_TaskSleep(UINT32 NumTicks);

                if (0 == SvcWrap_strcmp("filein", pArgVector[2U])) {
                    (void) SvcWrap_strtoul(pArgVector[3U], &NumRun);

                    for (i = 0; i < NumRun; i++) {
                        FileInfo.InputIdx = 0U;
                        FileInfo.DataIdx = 0U;

                        SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, FileNameLeft);
                        (void) AmbaWrap_memset(IdxStr, 0, sizeof(IdxStr));
                        (void) AmbaUtility_UInt32ToStr(IdxStr, sizeof(IdxStr), i, 10);
                        AmbaUtility_StringAppend(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, IdxStr);
                        AmbaUtility_StringAppend(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, ".y");
                        (void) SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_FILE_INFO, &FileInfo);

                        FileInfo.InputIdx = 1U;
                        FileInfo.DataIdx = 0U;

                        SvcWrap_strcpy(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, FileNameRight);
                        (void) AmbaWrap_memset(IdxStr, 0, sizeof(IdxStr));
                        (void)  AmbaUtility_UInt32ToStr(IdxStr, sizeof(IdxStr), i, 10);
                        AmbaUtility_StringAppend(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, IdxStr);
                        AmbaUtility_StringAppend(FileInfo.FileName, SVC_CV_FILE_IN_MAX_FILE_PATH, ".y");
                        (void) SvcCvFileInTask_SetInfo(SVC_CV_FILE_IN_SET_FILE_INFO, &FileInfo);

                        (void) SvcCvFileInTask_Start(&StartCfg);
                    }
                }
            } else {
                Rval = SVC_NG;
            }
#endif
        } else if (0 == SvcWrap_strcmp("pcpt_disp_mode", pArgVector[1U])) {
            #define MAX_DISP_MODE   (13U)
            #define DISP_GRP1       (((UINT32)1U << AMBANET_OD37_FC_CAT0_PERSON) |      \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT1_RIDER) |       \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT2_BICYCLE) |     \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT3_MOTORCYCLE) |  \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT4_CAR) |         \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT5_TRUCK))
            #define DISP_GRP2       (((UINT32)1U << AMBANET_OD37_FC_CAT6_RED) |         \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT7_GREEN) |       \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT8_SIGN) |        \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT9_OTHER))
            #define DISP_GRP3       (((UINT32)1U << AMBANET_OD37_FC_CAT10_ARROW) |      \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT11_SPEEDBUMP) |  \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT12_CROSSWALK) |  \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT13_STOPLINE) |   \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT14_YIELDLINE) |  \
                                     ((UINT32)1U << AMBANET_OD37_FC_CAT15_YIELDMARK))
            UINT32 DispMode = 0U;
            UINT32 Group1Bits, Group2Bits, Group3Bits;
             typedef struct {
                UINT32 ClsAllowBits;
                UINT8  DispKp;
                UINT8  DispMask;
            } OD_DISP_COMB_s;
            OD_DISP_COMB_s DispList[MAX_DISP_MODE] = {
                [0U]  = {                         DISP_GRP1, 0U, 0U },
                [1U]  = {                         DISP_GRP1, 0U, 0U },
                [2U]  = {                         DISP_GRP1, 1U, 1U },
                [3U]  = {                         DISP_GRP2, 0U, 0U },
                [4U]  = {                         DISP_GRP2, 1U, 1U },
                [5U]  = {                         DISP_GRP3, 0U, 0U },
                [6U]  = {                         DISP_GRP3, 1U, 1U },
                [7U]  = {             DISP_GRP1 | DISP_GRP2, 0U, 0U },
                [8U]  = {             DISP_GRP1 | DISP_GRP2, 1U, 1U },
                [9U]  = {             DISP_GRP1 | DISP_GRP3, 0U, 0U },
                [10U]  = {            DISP_GRP1 | DISP_GRP3, 1U, 1U },
                [11U] = { DISP_GRP1 | DISP_GRP2 | DISP_GRP3, 0U, 0U },
                [12U] = { DISP_GRP1 | DISP_GRP2 | DISP_GRP3, 1U, 1U },
            };

            AmbaMisra_TouchUnused(&DispList);

            if (2U < ArgCount) {
                (void)SvcWrap_strtoul(pArgVector[2U], &DispMode);

                (void)SvcODDrawTask_Ctrl("bbx_allow_bits", DispList[DispMode].ClsAllowBits);
                (void)SvcODDrawTask_Ctrl("kp", DispList[DispMode].DispKp);
                (void)SvcODDrawTask_Ctrl("mask", DispList[DispMode].DispMask);

                Group1Bits = (DispList[DispMode].ClsAllowBits & DISP_GRP1);
                Group2Bits = (DispList[DispMode].ClsAllowBits & DISP_GRP2);
                Group3Bits = (DispList[DispMode].ClsAllowBits & DISP_GRP3);

                AmbaPrint_PrintUInt5("Select DispMode = %d. Group1(0x%x) Group2(0x%x) Group3(0x%x) KP&Mask(0x%x)",
                    DispMode, Group1Bits, Group2Bits,Group3Bits,
                    (UINT32)((UINT32)DispList[DispMode].DispKp | (UINT32)DispList[DispMode].DispMask));
            }
        } else {
            Rval = SVC_NG;
        }
    } else {
        Rval = SVC_NG;
    }

    if (Rval != SVC_OK) {
        CmdCvUsage(PrintFunc);
    }
}

/**
 *  Install svc app cv command
 */
void SvcCmdAppCV_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdCv;

    UINT32  Rval;

    SvcCmdCv.pName    = "svc_cv";
    SvcCmdCv.MainFunc = CmdCvEntry;
    SvcCmdCv.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdCv);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_CMDCV, "## fail to install svc cv command", 0U, 0U);
    }
}
