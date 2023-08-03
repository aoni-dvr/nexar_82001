/**
 *  @file SvcCmdPbk.c
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
 *  @details svc playback command functions
 *
 */

#include "AmbaDMA_Def.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDef.h"
#include "AmbaDSP_Event.h"
#include "AmbaCache.h"
#include "AmbaVfs.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"
#include "AmbaDCF.h"
#include "AmbaStreamWrap.h"
#include "AmbaPlayer.h"
#include "AmbaGDMA.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcPbkCtrl.h"
#include "SvcCmd.h"
#include "SvcTask.h"
#include "SvcPbkPictDisp.h"
#include "SvcPbkThmDisp.h"
#include "SvcStgMonitor.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"
#include "SvcClock.h"
#include "SvcCmdPbk.h"

#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
#include "RefCmptPlayerImpl.h"
#endif

#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
#include "SvcRegressionTask.h"
#include "AmbaIPC_Regression.h"
#endif

static void CmdPbkUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc(" svc pbk commands:   \n");
    PrintFunc(" create   [ decoderID ]: latest file / forward / 1X / from the beginning\n");
    PrintFunc(" create   [ decoderID ]: \n");
    PrintFunc("          [ fileIndex ]: \n");
    PrintFunc("          [  voutBits ]: \n");
    PrintFunc("          [ direction ]: 0: Forward, 1: Backward\n");
    PrintFunc("          [   speed   ]: 1X, 2X, 4X, 8X, 16X (0~4)\n");
    PrintFunc("          [    mode   ]: 0: Start from the beginning\n");
    PrintFunc("                         1: start from last play\n");
    PrintFunc("                         2: Start from assigned [start_time].\n");
    PrintFunc("          [ start_time]: Starting time(sec)\n");
    PrintFunc(" create   [ FileName  ] [ decoderID ]: \n");
    PrintFunc(" delete   [ decoderID ]: \n");
    PrintFunc(" adjust   [ decoderID ] [direction] [ speed ] [ mode  ] [start_time]\n");
    PrintFunc(" start    [DecoderBits]: 1 => Decoder0, 2 => Decoder1, 3 => Decoder0 and Decoder1\n");
    PrintFunc(" stop     [DecoderBits]: \n");
    PrintFunc(" pause    [DecoderBits]: \n");
    PrintFunc(" resume   [DecoderBits]: \n");
    PrintFunc(" step     [DecoderBits]: \n");
    PrintFunc(" zoom_in  [DecoderBits]  \n");
    PrintFunc(" zoom_out [DecoderBits]  \n");
    PrintFunc(" rotate   [DecoderBits]  \n");
    PrintFunc("          [   Angle   ]: 0, 90, 180, 270\n");
    PrintFunc("          [   h_Flip  ]: 0: disable, 1: enable\n");
    PrintFunc(" audio    [   enable  ]: 0: disable, 1: enable \n");
    PrintFunc(" text     [   enable  ]: 0: disable, 1: enable \n");
}

#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
static void RefPbk(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8  PipeId = 0U;
    static void   *pStrHdlr;

    UINT32  Error = 0U;

    if (0 == SvcWrap_strcmp("pipenum", pArgVector[2U])) {
        UINT32 PipeNum;

        if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &PipeNum)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            if (SVC_OK != RefCmptPlayer_SetPipeNum((UINT8)PipeNum)) {
                SvcLog_NG("CMDPBK", "RefCmptPlayer_SetPipeNum failed!!", 0U, 0U);
                // Error = 1U;
            }
        }
    } else if (0 == SvcWrap_strcmp("gop", pArgVector[2U])) {
        UINT32 IdrInterval, TimeScale, TimePerFrame, N, M;

        if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &IdrInterval)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &TimeScale)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (SVC_OK != SvcWrap_strtoul(pArgVector[5U], &TimePerFrame)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (SVC_OK != SvcWrap_strtoul(pArgVector[6U], &N)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (SVC_OK != SvcWrap_strtoul(pArgVector[7U], &M)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            if (SVC_OK != RefCmptPlayer_SetGopConfig(IdrInterval, TimeScale, TimePerFrame, N, M)) {
                SvcLog_NG("CMDPBK", "RefCmptPlayer_SetGopConfig failed!!", 0U, 0U);
                // Error = 1U;
            }
        }
    } else if (0 == SvcWrap_strcmp("create", pArgVector[2U])) {
        UINT32                          FormatCount, FormatType;
        static REF_PLAYER_DEMUXER_FILE_INFO_s  FileInfo[AMBA_DEMUXER_MAX_FORMAT_PER_PIPE] = {0};

        if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &FormatCount)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &FormatType)) {
            SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            if (SVC_OK != AmbaStreamWrap_GetHdlr(&pStrHdlr)) {
                SvcLog_NG("CMDPBK", "AmbaStreamWrap_GetHdlr failed!!", 0U, 0U);
                Error = 1U;
            }
        }

        FileInfo[0].FormatType = (UINT8)FormatType;
        AmbaUtility_StringCopy(FileInfo[0].Prefix, 64U, pArgVector[5U]);

        if (Error == 0U) {
            if (SVC_OK != RefCmptPlayer_Create((UINT8)FormatCount, FileInfo, &PipeId, &pStrHdlr)) {
                SvcLog_NG("CMDPBK", "RefCmptPlayer_Create failed!!", 0U, 0U);
                // Error = 1U;
            }

            #ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
            SvcCvRegressionSetCmptPipeNum(PipeId);
            #endif
        }

#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
        SvcCvRegressionPlayMethod(2U);
        SvcCvRegressionSetPlayFrameNum(0U);
        (void)AmbaIPC_RegrStart();
#endif
    } else if (0 == SvcWrap_strcmp("start", pArgVector[2U])) {

        SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));

        if (SVC_OK != RefCmptPlayer_Start(PipeId, 0U, 0U, 256UL)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Start failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else if (0 == SvcWrap_strcmp("stop", pArgVector[2U])) {

        if (SVC_OK != RefCmptPlayer_Stop(PipeId)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Stop failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else if (0 == SvcWrap_strcmp("delete", pArgVector[2U])) {

        if (SVC_OK != RefCmptPlayer_Delete(PipeId)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Delete failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else if (0 == SvcWrap_strcmp("pause", pArgVector[2U])) {

        if (SVC_OK != RefCmptPlayer_Pause(PipeId)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Pause failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else if (0 == SvcWrap_strcmp("resume", pArgVector[2U])) {

        if (SVC_OK != RefCmptPlayer_Resume(PipeId)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Resume failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else if (0 == SvcWrap_strcmp("step", pArgVector[2U])) {

        if (SVC_OK != RefCmptPlayer_Step(PipeId)) {
            SvcLog_NG("CMDPBK", "RefCmptPlayer_Step failed!!", 0U, 0U);
            // Error = 1U;
        }
    } else {
        AmbaMisra_TouchUnused(&PrintFunc);
        AmbaMisra_TouchUnused(&ArgCount);
    }
}
#endif

static void CmdPbkEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32         DecoderID = 0U, Error = 0U, DecoderBits = 0U;
    UINT32         Direction = 0U, Speed = 0U;
    char           *FileName = NULL;

    if (2U < ArgCount) {
        if (0 == SvcWrap_strcmp("create", pArgVector[1U])) {
            UINT32                       FileIndex = SVC_PBK_CTRL_IDX_LATEST, Mode = 0U;
            UINT32                       StartTime = 0U, Start = 0U, VoutBits = 3U;

            if (8U < ArgCount) {
                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderID)) {
                    SvcLog_NG("CMDPBK", "Get DecoderID failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &FileIndex)) {
                    SvcLog_NG("CMDPBK", "Get FileIndex failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &VoutBits)) {
                    SvcLog_NG("CMDPBK", "Get VoutBits failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[5U], &Direction)) {
                    SvcLog_NG("CMDPBK", "Get Direction failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[6U], &Speed)) {
                    SvcLog_NG("CMDPBK", "Get Speed failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[7U], &Mode)) {
                    SvcLog_NG("CMDPBK", "Get Mode failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[8U], &StartTime)) {
                    SvcLog_NG("CMDPBK", "Get StartTime failed!!", 0U, 0U);
                    Error = 1U;
                }

                if (DecoderID >= (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
                    SvcLog_NG("CMDPBK", "DecoderID %u is invalid!!", DecoderID, 0U);
                    Error = 1U;
                }

                if (Speed > AMBA_PLAYER_SPEED_SIXTEENTH) {
                    SvcLog_NG("CMDPBK", "Speed %u is invalid!!", Speed, 0U);
                    Error = 1U;
                }

                if (Error == 0U) {
                    Start = 1U;
                }

            } else if (ArgCount == 3U) {
                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderID)) {
                    SvcLog_NG("CMDPBK", "Get DecoderID failed!!", 0U, 0U);
                    Error = 1U;
                }

                if (DecoderID >= (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
                    SvcLog_NG("CMDPBK", "DecoderID %u is invalid!!", DecoderID, 0U);
                    Error = 1U;
                }

                if (Error == 0U) {
                    Start = 1U;
                }

            } else if (ArgCount == 4U) {
                FileName = pArgVector[2U];

                if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &DecoderID)) {
                    SvcLog_NG("CMDPBK", "Get DecoderID failed!!", 0U, 0U);
                    Error = 1U;
                }

                if (DecoderID >= (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
                    SvcLog_NG("CMDPBK", "DecoderID %u is invalid!!", DecoderID, 0U);
                    Error = 1U;
                }

                if (Error == 0U) {
                    Start    = 1U;
                }
            } else {
                CmdPbkUsage(PrintFunc);
            }

            if (1U == Start) {
                SVC_PBK_CTRL_CREATE_s Create;

                Create.FileName  = FileName;
                Create.FileIndex = FileIndex;
                Create.Loop      = SVC_PBK_CTRL_LOOP_NONE;
                Create.Seamless  = 0U;
                Create.Speed     = Speed;
                Create.Direction = Direction;
                Create.StartTime = StartTime * 1000U;

                if (SVC_OK != SvcPbkCtrl_TaskCreate(DecoderID, &Create)) {
                    Error = 1U;
                }
            }

#ifdef CONFIG_ICAM_ENABLE_REGRESSION_FLOW
            SvcCvRegressionPlayMethod(1U);
            SvcCvRegressionSetPlayFrameNum(0);
            (void)AmbaIPC_RegrStart();
#endif

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "create failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "create successfully!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "Get DecoderID failed!!", 0U, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_Start(DecoderBits)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "start failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "start successfully!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_Stop(DecoderBits)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "stop failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "stop successfully!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("delete", pArgVector[1U])) {

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderID)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if (DecoderID >= (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
                SvcLog_NG("CMDPBK", "DecoderID %u is invalid!!", DecoderID, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_TaskDelete(DecoderID)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "delete failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "delete successfully!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("adjust", pArgVector[1U])) {

            if (6U < ArgCount) {
                UINT32               Mode = 0U, StartTime = 0U, FileIndex = 0U;
                SVC_PBK_CTRL_INFO_s  *pInfo = NULL;

                if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderID)) {
                    SvcLog_NG("CMDPBK", "Get DecoderID failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Direction)) {
                    SvcLog_NG("CMDPBK", "Get Direction failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &Speed)) {
                    SvcLog_NG("CMDPBK", "Get Speed failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[5U], &Mode)) {
                    SvcLog_NG("CMDPBK", "Get Mode failed!!", 0U, 0U);
                    Error = 1U;
                }
                if (SVC_OK != SvcWrap_strtoul(pArgVector[6U], &StartTime)) {
                    SvcLog_NG("CMDPBK", "Get StartTime failed!!", 0U, 0U);
                    Error = 1U;
                }

                if (DecoderID >= (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
                    SvcLog_NG("CMDPBK", "DecoderID %u is invalid!!", DecoderID, 0U);
                    Error = 1U;
                }

                if (Speed > AMBA_PLAYER_SPEED_SIXTEENTH) {
                    SvcLog_NG("CMDPBK", "Speed %u is invalid!!", Speed, 0U);
                    Error = 1U;
                }

                SvcPbkCtrl_InfoGet(DecoderID, &pInfo);
                if (pInfo != NULL) {
                    FileIndex = pInfo->FileIndex;

                    if (Mode == 1U) {
                        StartTime = pInfo->pPlayer->LatestTime;
                    } else {
                        StartTime *= 1000U;
                    }
                }

                if ((Error == 0U)) {
                    if (SVC_OK != SvcPbkCtrl_TaskDelete(DecoderID)) {
                        Error = 1U;
                    }
                }

                if ((Error == 0U)) {
                    SVC_PBK_CTRL_CREATE_s Create;

                    Create.FileName  = NULL;
                    Create.FileIndex = FileIndex;
                    Create.Loop      = SVC_PBK_CTRL_LOOP_NONE;
                    Create.Seamless  = 0U;
                    Create.Speed     = Speed;
                    Create.Direction = Direction;
                    Create.StartTime = StartTime;

                    if (SVC_OK != SvcPbkCtrl_TaskCreate(DecoderID, &Create)) {
                        Error = 1U;
                    }
                }

                if (Error != 0U) {
                    SvcLog_NG("CMDPBK", "adjust failed!!", 0U, 0U);
                } else {
                    SvcLog_OK("CMDPBK", "adjust success!!", 0U, 0U);
                }
            } else {
                CmdPbkUsage(PrintFunc);
            }

        } else if (0 == SvcWrap_strcmp("pause", pArgVector[1U])) {
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_PAUSE)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "pause failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "pause success!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("resume", pArgVector[1U])) {
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_RESUME)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "resume failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "resume success!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("step", pArgVector[1U])) {
            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if ((Error == 0U)) {
                if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_STEP)) {
                    Error = 1U;
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "step failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "step success!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("zoom_in", pArgVector[1U])) {
            UINT32                   i;
            SVC_PBK_CTRL_INFO_s      *pInfo = NULL;
            AMBA_PLAYER_DISP_CTRL_s  Disp = {0};

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }


            for (i = 0U; i < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; i++) {
                if (((1UL << i) & (DecoderBits)) != 0U) {
                    SvcPbkCtrl_InfoGet(i, &pInfo);
                    if (pInfo != NULL) {
                        AmbaSvcWrap_MisraMemcpy(&Disp, &(pInfo->pPlayer->Disp), sizeof(AMBA_PLAYER_DISP_CTRL_s));
                    }

                    Disp.ZoomRatio += 20U;

                    if (Disp.ZoomRatio > 200U) {
                        Disp.ZoomRatio = 200U;
                    }

                    if (SVC_OK != SvcPbkCtrl_PostCtrl(i, &Disp)) {
                        Error = 1U;
                    }
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "zoom_in failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "zoom_in success!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("zoom_out", pArgVector[1U])) {
            UINT32                   i;
            SVC_PBK_CTRL_INFO_s      *pInfo = NULL;
            AMBA_PLAYER_DISP_CTRL_s  Disp = {0};

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }


            for (i = 0U; i < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; i++) {
                if (((1UL << i) & (DecoderBits)) != 0U) {
                    SvcPbkCtrl_InfoGet(i, &pInfo);
                    if (pInfo != NULL) {
                        AmbaSvcWrap_MisraMemcpy(&Disp, &(pInfo->pPlayer->Disp), sizeof(AMBA_PLAYER_DISP_CTRL_s));
                    }

                    Disp.ZoomRatio -= 20U;

                    if (Disp.ZoomRatio < 50U) {
                        Disp.ZoomRatio = 50U;
                    }

                    if (SVC_OK != SvcPbkCtrl_PostCtrl(i, &Disp)) {
                        Error = 1U;
                    }
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "zoom_out failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "zoom_out success!!", 0U, 0U);
            }

        } else if (0 == SvcWrap_strcmp("rotate", pArgVector[1U])) {
            UINT32                   i, Angle = 0U, Flip = 0U;
            SVC_PBK_CTRL_INFO_s      *pInfo = NULL;
            AMBA_PLAYER_DISP_CTRL_s  Disp = {0};

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &DecoderBits)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if (SVC_OK != SvcWrap_strtoul(pArgVector[3U], &Angle)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            if (SVC_OK != SvcWrap_strtoul(pArgVector[4U], &Flip)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
                Error = 1U;
            }

            for (i = 0U; i < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; i++) {
                if (((1UL << i) & (DecoderBits)) != 0U) {
                    SvcPbkCtrl_InfoGet(i, &pInfo);
                    if (pInfo != NULL) {
                        AmbaSvcWrap_MisraMemcpy(&Disp, &(pInfo->pPlayer->Disp), sizeof(AMBA_PLAYER_DISP_CTRL_s));
                    }

                    Disp.RotateFlip = ((Angle / 90U) * 2U) + Flip;

                    if (SVC_OK != SvcPbkCtrl_PostCtrl(i, &Disp)) {
                        Error = 1U;
                    }
                }
            }

            if (Error != 0U) {
                SvcLog_NG("CMDPBK", "rotate failed!!", 0U, 0U);
            } else {
                SvcLog_OK("CMDPBK", "rotate success!!", 0U, 0U);
            }
        } else if (0 == SvcWrap_strcmp("pictdisp", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("init", pArgVector[2U])) {
                (void)SvcPbkPictDisp_TaskCreate();
                (void)SvcThmDisp_ThmViewStart(AMBA_DCF_FILE_TYPE_VIDEO, 0U);
            } else if (0 == SvcWrap_strcmp("key_left", pArgVector[2U])) {
                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_LEFT);
            } else if (0 == SvcWrap_strcmp("key_right", pArgVector[2U])) {
                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_RIGHT);
            } else if (0 == SvcWrap_strcmp("key_ok", pArgVector[2U])) {
                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_OK);
            } else if (0 == SvcWrap_strcmp("key_up", pArgVector[2U])) {
                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_UP);
            } else {
                // Do nothing
            }
        } else if (0 == SvcWrap_strcmp("cmpt", pArgVector[1U])) {
#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
            RefPbk(ArgCount, pArgVector, PrintFunc);
#else
            SvcLog_NG("CMDPBK", "cmpt cmd not exist!!", 0U, 0U);
#endif
        } else if (0 == SvcWrap_strcmp("audio", pArgVector[1U])) {
            UINT32 Enable;

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &Enable)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            } else {
                SvcPbkCtrl_TrackCtrl(SVC_PBK_CTRL_TRACK_AUDIO, Enable);
            }

        } else if (0 == SvcWrap_strcmp("text", pArgVector[1U])) {
            UINT32 Enable;

            if (SVC_OK != SvcWrap_strtoul(pArgVector[2U], &Enable)) {
                SvcLog_NG("CMDPBK", "SvcWrap_strtoul failed!!", 0U, 0U);
            } else {
                SvcPbkCtrl_TrackCtrl(SVC_PBK_CTRL_TRACK_TEXT, Enable);
            }

        } else {
            SvcLog_NG("CMDPBK", "## no match command", 0U, 0U);
        }
    } else {
        CmdPbkUsage(PrintFunc);
    }
}

/**
* Install svc playback command
*/
void SvcCmdPbk_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdPlayback;

    UINT32  Rval;

    SvcCmdPlayback.pName    = "svc_pbk";
    SvcCmdPlayback.MainFunc = CmdPbkEntry;
    SvcCmdPlayback.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdPlayback);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG("CMDPBK", "## fail to install svc playback command", 0U, 0U);
    }
}
