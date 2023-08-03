/**
 *  @file SvcPlaybackTask.c
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
 *  @details svc playback task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaVfs.h"
#include "AmbaCodecCom.h"
#include "AmbaDCF.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaStreamWrap.h"
#include "AmbaPlayer.h"
#include "AmbaGDMA.h"
#include "AmbaFPD.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcStgMgr.h"
#include "SvcPbkCtrl.h"
#include "SvcButtonCtrl.h"
#include "SvcPbkPictDisp.h"
#include "SvcPbkThmDisp.h"
#include "SvcSysStat.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcStgMonitor.h"
#include "SvcVinSrc.h"
#include "SvcResCfg.h"
#include "SvcResCfgTask.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcAppStat.h"
#include "SvcUserPref.h"
#include "SvcTaskList.h"
#include "SvcBufMap.h"
#include "SvcPbkInfoPack.h"
#include "SvcPlat.h"
#include "SvcPlaybackTask.h"
#if defined(CONFIG_ICAM_ENCRYPTION)
#include "SvcEncrypt.h"
#endif

#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
#include "RefCmptPlayerImpl.h"
#endif

#define SVC_LOG_PLAYBACK_TASK   "PLAYBACK_TASK"

#if defined(CONFIG_ICAM_32BITS_OSD_USED)
#define FONT_COLOR              (0xffffffffU)
#define BACKGRD_COLOR           (0x80202040U)
#else
#define FONT_COLOR              (255U)
#define BACKGRD_COLOR           (73U)
#endif

#define FONT_SIZE               (5U)

#define FONT_WIDTH              (FONT_SIZE * (SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP))
#define FONT_HEIGHT             (FONT_SIZE * SVC_OSD_CHAR_HEIGHT)
#define PBK_GUI_LEVEL           (18U)

#if (AMBA_DSP_MAX_VOUT_NUM > 1U)
static UINT32 VoutOfPbkGui = VOUT_IDX_B;
#else
static UINT32 VoutOfPbkGui = VOUT_IDX_A;
#endif

static SVC_SYS_STAT_ID        MenuCtrlID;
static UINT8                  LoopPbkStart = 0U;
static UINT32                 LoopPlayerBits = 0U;

#define TEXT_BUF_SIZE         (0xAF00U)
static UINT8                  TextBuf[TEXT_BUF_SIZE];

static void PbkTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32               State, InputVal = 0U;
    SVC_APP_STAT_MENU_s  *pStatus = NULL;
    SVC_VIDPBK_INFO_s    VidInfo = {0};

    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(pStatus);

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    State = SvcPbkPictDisp_GetState();

    if ((StatIdx == SVC_APP_STAT_MENU) && (pStatus->Type == SVC_APP_STAT_MENU_TYPE_PLAYBACK)) {
        switch(pStatus->FuncIdx) {
        case SVC_APP_STAT_MENU_PLAYBACK_START:
            if ((State == SVC_PICT_DISP_THM_VIEW_STATE) || (State == SVC_PICT_DISP_SINGLE_VIEW_STATE)) {
                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_OK);
            } else {
                PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);
                if (VidInfo.VideoStart == 1U) {
                    if (VidInfo.VideoPause == 0U) {
                        PictDispVideoHandler(SVC_VIDPBK_PMT_PAUSE, &InputVal);
                    } else {
                        PictDispVideoHandler(SVC_VIDPBK_PMT_RESUME, &InputVal);
                    }
                }
            }
            break;
        case SVC_APP_STAT_MENU_PLAYBACK_STOP:
            if ((State == SVC_PICT_DISP_THM_VIEW_STATE) || (State == SVC_PICT_DISP_SINGLE_VIEW_STATE)) {
                InputVal = SVC_VIDPBK_MODE_BEGIN;

                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_UP);
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
            } else {
                InputVal = SVC_VIDPBK_STOP_BACK;
                PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);
                if (VidInfo.VideoStart == 1U) {
                    if (VidInfo.VideoPause == 1U) {
                        PictDispVideoHandler(SVC_VIDPBK_PMT_RESUME, &InputVal);
                    }
                    PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);
                }

                InputVal = SVC_VIDPBK_MODE_CONT;
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
            }
            break;
        case SVC_APP_STAT_MENU_PLAYBACK_PREV:
            if ((State == SVC_PICT_DISP_THM_VIEW_STATE) || (State == SVC_PICT_DISP_SINGLE_VIEW_STATE)) {
                InputVal = SVC_VIDPBK_MODE_BEGIN;

                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_LEFT);
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
            }
            break;
        case SVC_APP_STAT_MENU_PLAYBACK_NEXT:
            if ((State == SVC_PICT_DISP_THM_VIEW_STATE) || (State == SVC_PICT_DISP_SINGLE_VIEW_STATE)) {
                InputVal = SVC_VIDPBK_MODE_BEGIN;

                SvcPbkPictDisp_KeyEventSend(SVC_PICT_DISP_KEY_RIGHT);
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
            }
            break;
        case SVC_APP_STAT_MENU_PLAYBACK_BACKWARD:
            PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);

            if (VidInfo.VideoStart == 1U) {
                InputVal = SVC_VIDPBK_STOP_STAY;
                PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);

                if (VidInfo.VideoDirect == AMBA_PLAYER_DIR_FORWARD) {
                    InputVal = AMBA_PLAYER_DIR_BACKWARD;
                    PictDispVideoHandler(SVC_VIDPBK_PMT_DIRECT, &InputVal);
                    InputVal = AMBA_PLAYER_SPEED_01X;
                } else {
                    InputVal = (VidInfo.VideoSpeed + 1U) % ((UINT32)AMBA_PLAYER_SPEED_16X + 1U);
                }
                PictDispVideoHandler(SVC_VIDPBK_PMT_SPEED, &InputVal);

                InputVal = SVC_VIDPBK_MODE_CONT;
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
                PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
            } else {
                if (VidInfo.VideoDirect == AMBA_PLAYER_DIR_FORWARD) {
                    InputVal = AMBA_PLAYER_DIR_BACKWARD;
                    PictDispVideoHandler(SVC_VIDPBK_PMT_DIRECT, &InputVal);
                    InputVal = AMBA_PLAYER_SPEED_01X;
                } else {
                    InputVal = (VidInfo.VideoSpeed + 1U) % ((UINT32)AMBA_PLAYER_SPEED_16X + 1U);
                }
                PictDispVideoHandler(SVC_VIDPBK_PMT_SPEED, &InputVal);
            }
            break;
        case SVC_APP_STAT_MENU_PLAYBACK_FORWARD:
            PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);

            if (VidInfo.VideoStart == 1U) {
                InputVal = SVC_VIDPBK_STOP_STAY;
                PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);

                if (VidInfo.VideoDirect == AMBA_PLAYER_DIR_BACKWARD) {
                    InputVal = AMBA_PLAYER_DIR_FORWARD;
                    PictDispVideoHandler(SVC_VIDPBK_PMT_DIRECT, &InputVal);
                    InputVal = AMBA_PLAYER_SPEED_01X;
                } else {
                    InputVal = (VidInfo.VideoSpeed + 1U) % ((UINT32)AMBA_PLAYER_SPEED_16X + 1U);
                }
                PictDispVideoHandler(SVC_VIDPBK_PMT_SPEED, &InputVal);

                InputVal = SVC_VIDPBK_MODE_CONT;
                PictDispVideoHandler(SVC_VIDPBK_PMT_MODE, &InputVal);
                PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
            } else {
                if (VidInfo.VideoDirect == AMBA_PLAYER_DIR_BACKWARD) {
                    InputVal = AMBA_PLAYER_DIR_FORWARD;
                    PictDispVideoHandler(SVC_VIDPBK_PMT_DIRECT, &InputVal);
                    InputVal = AMBA_PLAYER_SPEED_01X;
                } else {
                    InputVal = (VidInfo.VideoSpeed + 1U) % ((UINT32)AMBA_PLAYER_SPEED_16X + 1U);
                }
                PictDispVideoHandler(SVC_VIDPBK_PMT_SPEED, &InputVal);
            }
            break;
        default:
            /* nothing */
            break;
        }
    } else if ((StatIdx == SVC_APP_STAT_MENU) && (pStatus->Type == SVC_APP_STAT_MENU_TYPE_ADAS_DVR)) {
        switch(pStatus->FuncIdx) {
        case SVC_APP_STAT_MENU_ADAS_PLAYBACK:
            PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);

            if (VidInfo.VideoStart == 1U) {
                InputVal = SVC_VIDPBK_STOP_STAY;
                PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);
            } else {
                PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
            }

            break;
        default:
            /* nothing */
            break;
        }
    } else if ((StatIdx == SVC_APP_STAT_MENU) && (pStatus->Type == SVC_APP_STAT_MENU_TYPE_DMS)) {

        switch(pStatus->FuncIdx) {
        case SVC_APP_STAT_MENU_DMS_PLAYBACK:
            if (LoopPbkStart == 0U) {
                UINT32 FileIdx = 0U;
                SvcPlaybackTask_LoopPbkStart(1U, &FileIdx, 0U);
            } else {
                SvcPlaybackTask_LoopPbkStop();
            }

            break;
        default:
            /* nothing */
            break;
        }
    } else {
        /* nothing */
    }
}

static void PbkTask_GuiDraw_VidInfo(UINT32 VoutIdx, UINT32 Level)
{
#define PBKTSK_TEXT_NUM (12U)
    UINT32               RetVal = SVC_OK, Err;
    UINT32               BufWidth = 0U, BufHeight = 0U, StartX = 0U, StartY = 0U, Width = 0U, Height = 0U;
    SVC_VIDPBK_INFO_s    VidInfo = {0};
    char                 Text[PBKTSK_TEXT_NUM];

    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(&RetVal);

    PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);

    {
        /* text direction */
        Text[0] = 'D';
        Text[1] = ':';
        if (VidInfo.VideoDirect == AMBA_PLAYER_DIR_FORWARD) {
            Text[2] = 'F';
        } else {
            Text[2] = 'B';
        }
        Text[3] = 'W';
        Text[4] = 'D';
        Text[5] = ' ';

        /* text speed */
        Text[6] = 'S';
        Text[7] = ':';
        if (VidInfo.VideoSpeed == AMBA_PLAYER_SPEED_01X) {
            Text[8] = '0';
            Text[9] = '1';
        } else if (VidInfo.VideoSpeed == AMBA_PLAYER_SPEED_02X) {
            Text[8] = '0';
            Text[9] = '2';
        } else if (VidInfo.VideoSpeed == AMBA_PLAYER_SPEED_04X) {
            Text[8] = '0';
            Text[9] = '4';
        } else if (VidInfo.VideoSpeed == AMBA_PLAYER_SPEED_08X) {
            Text[8] = '0';
            Text[9] = '8';
        } else {
            Text[8] = '1';
            Text[9] = '6';
        }
        Text[10] = 'X';
        Text[11] = '\0';
    }

    if (RetVal == SVC_OK) {
        Err = SvcOsd_GetOsdBufSize(VoutIdx, &BufWidth, &BufHeight);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcOsd_GetOsdBufSize failed %d", Err, 0U);
        }

        Width  = PBKTSK_TEXT_NUM * FONT_WIDTH;
#ifdef CONFIG_SVC_LVGL_USED
        Width  = Width / 2U;    /* lvgl font width may be different to all the characters, FONT_WIDTH is the maximum one */
#endif
        Height = FONT_HEIGHT;
        StartX = (BufWidth - (Width)) / 2U;
    }

    /* text background color */
    if (RetVal == SVC_OK) {
        Err = SvcOsd_DrawSolidRect(VoutIdx, StartX, StartY, StartX + Width, StartY + Height, BACKGRD_COLOR);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcOsd_DrawSolidRect failed %d", Err, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        Err = SvcOsd_DrawString(VoutIdx, StartX, StartY, FONT_SIZE, FONT_COLOR, Text);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcOsd_DrawString failed %d", Err, 0U);
        }
    }
}

static void PbkTask_GuiDraw_Update(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    UINT32                   Err;
    static SVC_VIDPBK_INFO_s PrevVidInfo;
    SVC_VIDPBK_INFO_s        NewVidInfo;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    Err = AmbaWrap_memset(&NewVidInfo, 0, sizeof(NewVidInfo));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
    }

    PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &NewVidInfo);
    if ((NewVidInfo.VideoDirect != PrevVidInfo.VideoDirect) || (NewVidInfo.VideoSpeed != PrevVidInfo.VideoSpeed)) {
        PrevVidInfo.VideoDirect = NewVidInfo.VideoDirect;
        PrevVidInfo.VideoSpeed  = NewVidInfo.VideoSpeed;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

#if defined (CONFIG_ICAM_ENABLE_REGRESSION_FLOW)
static SVC_SYS_STAT_ID InitRPCServices_StatID;
static void InitRPCServices(UINT32 StatIdx, void *pInfo)
{
    AmbaMisra_TouchUnused(&StatIdx);
    AmbaMisra_TouchUnused(pInfo);

    AmbaStreamWrap_Init(NULL);
}
#endif

/**
* The function to init Playback
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_Init(void)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&MenuCtrlID);

    SvcLog_DBG(SVC_LOG_PLAYBACK_TASK, "@@ Init Begin", 0U, 0U);

    return RetVal;
}

/**
* The function to configure Playback information and resource
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_Config(void)
{
    UINT32               RetVal = SVC_OK, Err;
    UINT32               DecoderID;
    SVC_PBK_CTRL_INFO_s  *pInfo = NULL;
    AMBA_DSP_RESOURCE_s  DspRes;
    ULONG                BufBase;

    AmbaMisra_TouchUnused(&RetVal);

    /* video pbk config */
    for (DecoderID = 0U; DecoderID < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; DecoderID++) {
        SvcPbkCtrl_InfoGet(DecoderID, &pInfo);
        SvcInfoPack_PbkConfig(DecoderID, pInfo);
    }

    /* init vfs */
    if ((RetVal == SVC_OK)) {
        AMBA_VFS_INIT_s Cfg;

        AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(Cfg));

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_VFS, &BufBase, &(Cfg.BufSize));
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request %u failed", FMEM_ID_VFS, 0U);
        } else {
            Cfg.BufAddr      = BufBase;
            Cfg.TaskPriority = SVC_VFS_TASK_PRI;
            Cfg.TaskCpuBits  = SVC_VFS_TASK_CPU_BITS;
            if (g_pPlatCbEntry != NULL) {
                Cfg.pCbCacheChk = g_pPlatCbEntry->pCbCacheChk;
                Cfg.pCbCacheCln = g_pPlatCbEntry->pCbCacheCln;
                Cfg.pCbCacheInv = g_pPlatCbEntry->pCbCacheInv;
                Cfg.pCbVirToPhy = g_pPlatCbEntry->pCbVirToPhy;
            }

            RetVal = AmbaVFS_Init(&Cfg);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "## fail to init AmbaVfs", 0U, 0U);
            }
        }
    }

    /* still pbk config */
    SvcInfoPack_StlPbkConfig();

    /* dsp resource */
    if (SVC_OK == RetVal) {
        Err = AmbaWrap_memset(&DspRes, 0, sizeof(AMBA_DSP_RESOURCE_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
            RetVal = SVC_NG;
        }

        SvcInfoPack_PbkResConfig(&DspRes);

        Err = AmbaDSP_ResourceLimitConfig(&DspRes);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaDSP_ResourceLimitConfig failed(%u)", Err, 0U);
            RetVal = SVC_NG;
        }
    }

    /* svc stream */
    if (RetVal == SVC_OK) {
#if defined (CONFIG_ICAM_ENABLE_REGRESSION_FLOW)
        SvcSysStat_Register(SVC_APP_STAT_LINUX,InitRPCServices,&InitRPCServices_StatID);
#else
        AmbaStreamWrap_Init(SvcStgMgr_GetFreeSize);
#endif
    }

    /* bits fifo */
    if (RetVal == SVC_OK) {
        AMBA_BFIFO_INIT_s Init;

        Err = AmbaWrap_memset(&Init, 0, sizeof(AMBA_BFIFO_INIT_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Init.TaskPriority = SVC_BITS_FIFO_TASK_PRI;
        Init.TaskCpuBits  = SVC_BITS_FIFO_TASK_CPU_BITS;

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_BITS_FIFO, &BufBase, &(Init.BufSize));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_BITS_FIFO, 0U);
            RetVal = SVC_NG;
        } else {
            Init.BufAddr = BufBase;
        }

        if (RetVal == SVC_OK) {
            Err = AmbaBitsFifo_Init(&Init);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaBitsFifo_Init failed(%u)", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    /* svc pbk */
    if (RetVal == SVC_OK) {
        UINT32                 i, BufSize;
        AMBA_PLAYER_INIT_s     Init;
        const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

        Err = AmbaWrap_memset(&Init, 0, sizeof(AMBA_PLAYER_INIT_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Init.DispNum          = pCfg->DispNum;
        Init.PlayerNum        = CONFIG_AMBA_PLAYER_MAX_NUM;
        Init.TaskPriority     = SVC_PLAYER_TASK_PRI;
        Init.TaskCpuBits      = SVC_PLAYER_TASK_CPU_BITS;

        for (i = 0U; i < Init.DispNum; i++) {
            const AMBA_FPD_OBJECT_s  *pFpdObj;
            AMBA_FPD_INFO_s          FpdInfo = {0};

            AmbaMisra_TypeCast(&pFpdObj, &(pCfg->DispStrm[i].pDriver));
            Err = pFpdObj->FpdGetInfo(&FpdInfo);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "FpdGetInfo failed %u", Err, 0U);
            }

            Init.DispInfo[i].VoutIdx                = (UINT8)pCfg->DispStrm[i].VoutID;
            Init.DispInfo[i].VoutRotateFlip         = pCfg->DispStrm[i].VideoRotateFlip;
            Init.DispInfo[i].VoutWindow.OffsetX     = 0U;
            Init.DispInfo[i].VoutWindow.OffsetY     = 0U;
            Init.DispInfo[i].VoutWindow.Width       = pCfg->DispStrm[i].StrmCfg.Win.Width;
            Init.DispInfo[i].VoutWindow.Height      = pCfg->DispStrm[i].StrmCfg.Win.Height;
            if ((FpdInfo.AspectRatio.X == 0U) || (FpdInfo.AspectRatio.Y == 0U)) {
                Init.DispInfo[i].AspectRatioX           = pCfg->DispStrm[i].StrmCfg.Win.Width;
                Init.DispInfo[i].AspectRatioY           = pCfg->DispStrm[i].StrmCfg.Win.Height;
            } else {
                Init.DispInfo[i].AspectRatioX           = FpdInfo.AspectRatio.X;
                Init.DispInfo[i].AspectRatioY           = FpdInfo.AspectRatio.Y;
            }
            Init.DispInfo[i].IsInterlace            = pCfg->DispStrm[i].FrameRate.Interlace;
        }

        for (i = 0U; i < Init.PlayerNum; i++) {
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VDEC_BS, &BufBase, &BufSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_VDEC_BS, 0U);
                RetVal = SVC_NG;
            } else {
                if (BufSize < (Init.PlayerNum * (UINT32)CONFIG_ICAM_DSP_DEC_BS_SIZE)) {
                    SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SMEM_PF0_ID_VDEC_BS bufsize is not enough failed(%u/%u)", BufSize, Init.PlayerNum * (UINT32)CONFIG_ICAM_DSP_DEC_BS_SIZE);
                    RetVal = SVC_NG;
                } else {
                    Init.BitsBuf[i].VBitsBufBase = BufBase + ((ULONG)i * (ULONG)CONFIG_ICAM_DSP_DEC_BS_SIZE);
                    Init.BitsBuf[i].VBitsBufSize = CONFIG_ICAM_DSP_DEC_BS_SIZE;
                }
            }
#if defined(CONFIG_ICAM_AUDIO_USED)
            if (i == 0U) {
                Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_AUD_DEC_BS, &BufBase, &(Init.BitsBuf[i].ABitsBufSize));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_AUD_DEC_BS, 0U);
                    RetVal = SVC_NG;
                } else {
                    Init.BitsBuf[i].ABitsBufBase = BufBase;
                }
            }
#endif
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STLDEC_YUV, &BufBase, &(Init.BitsBuf[i].YuvBufSize));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_STLDEC_YUV, 0U);
                RetVal = SVC_NG;
            } else {
                Init.BitsBuf[i].YuvBufBase = BufBase;
            }

            {
                const UINT8 *pBuf;

                pBuf = &(TextBuf[0]);

                AmbaMisra_TypeCast(&(Init.BitsBuf[i].TBitsBufBase), &pBuf);
                Init.BitsBuf[i].TBitsBufSize = TEXT_BUF_SIZE;
            }
        }

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_PLAYER, &BufBase, &(Init.PlayerBuf.BufSize));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_PLAYER, 0U);
            RetVal = SVC_NG;
        } else {
            Init.PlayerBuf.BufAddr = BufBase;
        }

        if (RetVal == SVC_OK) {
            Err = AmbaPlayer_Init(&Init);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaPlayer_Init failed(%u)", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }

#if defined(CONFIG_ICAM_ENCRYPTION)
    if (RetVal == SVC_OK) {
        ULONG                 BufAddr = 0U;
        UINT32                BufSize = 0U;
        SVC_ENCRYPT_INIT_s    Init;

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED,
                                FMEM_ID_ENCRYPT,
                                &BufAddr,
                                &BufSize);

        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "## fail to get encryption buffer", 0U, 0U);
        } else {
            Init.EncryptType = SVC_ENCRYPT_TYPE_AES;
            Init.BufBase     = BufAddr;
            Init.BufSize     = BufSize;

            Err = SvcEncrypt_Init(&Init);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "## fail to init encryption", 0U, 0U);
            }
        }
    }
#endif

#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
    if (RetVal == SVC_OK) {
        UINT32 BufSize;
        ULONG  BufAddr;
        Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VDEC_BS, &BufAddr, &BufSize);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_VDEC_BS, 0U);
            RetVal = SVC_NG;
        }

        if (RetVal == SVC_OK) {
            Err = RefCmptPlayer_Init(REF_PLAYER_MODE_COMPATIBLE, BufAddr, BufSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "RefCmptPlayer_Init failed(%u)", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }
#endif

    return RetVal;
}

/**
* The function to configure Playback information and resource
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_DuplexConfig(void)
{
    UINT32                 RetVal = SVC_OK, Enable = 0U, i, Src = 0U, Err;
    UINT32                 DecoderID;
    SVC_PBK_CTRL_INFO_s    *pInfo = NULL;
    SVC_USER_PREF_s        *pSvcUserPref;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(&RetVal);

    /* if it's in duplex mode */
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &Src);
                if ((Err == SVC_OK) && (Src == SVC_VIN_SRC_MEM_DEC)) {
                    Enable  = 1U;
                    break;
                }
            }
        }
    }

    if (Enable == 1U) {
        for (DecoderID = 0U; DecoderID < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; DecoderID++) {
            SvcPbkCtrl_InfoGet(DecoderID, &pInfo);
            SvcInfoPack_PbkConfig(DecoderID, pInfo);
        }
    }

    /* svc stream */
    if ((Enable == 1U) && (RetVal == SVC_OK)) {
#if defined (CONFIG_ICAM_ENABLE_REGRESSION_FLOW)
        SvcSysStat_Register(SVC_APP_STAT_LINUX,InitRPCServices,&InitRPCServices_StatID);
#else
        AmbaStreamWrap_Init(SvcStgMgr_GetFreeSize);
#endif
    }

    /* svc pbk */
    if ((Enable == 1U) && (RetVal == SVC_OK)) {
        UINT32                  BufSize;
        ULONG                   BufBase;
        AMBA_PLAYER_INIT_s      Init;

        Err = AmbaWrap_memset(&Init, 0, sizeof(AMBA_PLAYER_INIT_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaWrap_memset failed(%u)", Err, 0U);
        }

        Init.DispNum          = pCfg->DispNum;
        Init.PlayerNum        = CONFIG_AMBA_PLAYER_MAX_NUM;
        Init.TaskPriority     = SVC_PLAYER_TASK_PRI;
        Init.TaskCpuBits      = SVC_PLAYER_TASK_CPU_BITS;

        for (i = 0U; i < Init.DispNum; i++) {
            const AMBA_FPD_OBJECT_s  *pFpdObj;
            AMBA_FPD_INFO_s          FpdInfo = {0};

            AmbaMisra_TypeCast(&pFpdObj, &(pCfg->DispStrm[i].pDriver));
            Err = pFpdObj->FpdGetInfo(&FpdInfo);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "FpdGetInfo failed %u", Err, 0U);
            }

            Init.DispInfo[i].VoutIdx                = (UINT8)pCfg->DispStrm[i].VoutID;
            Init.DispInfo[i].VoutRotateFlip         = pCfg->DispStrm[i].VideoRotateFlip;
            Init.DispInfo[i].VoutWindow.OffsetX     = 0U;
            Init.DispInfo[i].VoutWindow.OffsetY     = 0U;
            Init.DispInfo[i].VoutWindow.Width       = pCfg->DispStrm[i].StrmCfg.MaxWin.Width;
            Init.DispInfo[i].VoutWindow.Height      = pCfg->DispStrm[i].StrmCfg.MaxWin.Height;
            if ((FpdInfo.AspectRatio.X == 0U) || (FpdInfo.AspectRatio.Y == 0U)) {
                Init.DispInfo[i].AspectRatioX           = pCfg->DispStrm[i].StrmCfg.MaxWin.Width;
                Init.DispInfo[i].AspectRatioY           = pCfg->DispStrm[i].StrmCfg.MaxWin.Height;
            } else {
                Init.DispInfo[i].AspectRatioX           = FpdInfo.AspectRatio.X;
                Init.DispInfo[i].AspectRatioY           = FpdInfo.AspectRatio.Y;
            }
            Init.DispInfo[i].IsInterlace            = pCfg->DispStrm[i].FrameRate.Interlace;
        }

        for (i = 0U; i < Init.PlayerNum; i++) {
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VDEC_BS, &BufBase, &BufSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_VDEC_BS, 0U);
                RetVal = SVC_NG;
            } else {
                if (BufSize < (Init.PlayerNum * (UINT32)CONFIG_ICAM_DSP_DEC_BS_SIZE)) {
                    SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SMEM_PF0_ID_VDEC_BS bufsize is not enough failed(%u/%u)", BufSize, Init.PlayerNum * (UINT32)CONFIG_ICAM_DSP_DEC_BS_SIZE);
                    RetVal = SVC_NG;
                } else {
                    Init.BitsBuf[i].VBitsBufBase = BufBase + ((ULONG)i * (ULONG)CONFIG_ICAM_DSP_DEC_BS_SIZE);
                    Init.BitsBuf[i].VBitsBufSize = CONFIG_ICAM_DSP_DEC_BS_SIZE;
                }
            }
#if defined(CONFIG_ICAM_AUDIO_USED)
            if (i == 0U) {
                Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_AUD_DEC_BS, &BufBase, &(Init.BitsBuf[i].ABitsBufSize));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_AUD_DEC_BS, 0U);
                    RetVal = SVC_NG;
                } else {
                    Init.BitsBuf[i].ABitsBufBase = BufBase;
                }
            }
#endif
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STLDEC_YUV, &BufBase, &(Init.BitsBuf[i].YuvBufSize));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_STLDEC_YUV, 0U);
                RetVal = SVC_NG;
            } else {
                Init.BitsBuf[i].YuvBufBase = BufBase;
            }

            {
                const UINT8 *pBuf;

                pBuf = &(TextBuf[0]);

                AmbaMisra_TypeCast(&(Init.BitsBuf[i].TBitsBufBase), &pBuf);
                Init.BitsBuf[i].TBitsBufSize = TEXT_BUF_SIZE;
            }
        }

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_PLAYER, &BufBase, &(Init.PlayerBuf.BufSize));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", FMEM_ID_PLAYER, 0U);
            RetVal = SVC_NG;
        } else {
            Init.PlayerBuf.BufAddr = BufBase;
        }

        if (RetVal == SVC_OK) {
            Err = AmbaPlayer_Init(&Init);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaPlayer_Init failed(%u)", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }

#if defined(CONFIG_ICAM_THMVIEW_IN_DPX)
    /* still pbk config */
    {
        UINT32 ThmDisp = 0U, PbkFormatId = 0U;

        if (SVC_OK == SvcResCfgTask_GetPbkModeId(&PbkFormatId)) {
            if (PbkFormatId == pSvcUserPref->FormatId) {
                ThmDisp = 1U;
            }
        }

        if ((Enable == 1U) && (ThmDisp > 0U)) {
            SvcInfoPack_StlPbkConfig();
        }
    }
#endif

#ifdef CONFIG_BUILD_COMSVC_REFPLAYER
    if (RetVal == SVC_OK) {
        UINT32 BufSize;
        ULONG  BufAddr;
        Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VDEC_BS, &BufAddr, &BufSize);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcBuffer_Request failed(%u)", SMEM_PF0_ID_VDEC_BS, 0U);
            RetVal = SVC_NG;
        }

        if (RetVal == SVC_OK) {
            Err = RefCmptPlayer_Init(REF_PLAYER_MODE_COMPATIBLE, BufAddr, BufSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "RefCmptPlayer_Init failed(%u)", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }
#endif

    return RetVal;
}

/**
* The function to init Thumbnail view
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_ThumbnailStart(void)
{
    UINT32                     RetVal = SVC_OK, Err, DspBoot = 0U, DriveReady = 0U;
    SVC_APP_STAT_DSP_BOOT_s    DspStatus;
    SVC_STG_MONI_DRIVE_INFO_s  DriveStatus;
    SVC_PBK_CTRL_INFO_s        *pInfo;

    AmbaMisra_TouchUnused(&RetVal);
    SvcPbkCtrl_InfoGet(0U, &pInfo);

    /* wait dsp boot done and drive ready */
    while ((DspBoot == 0U) || (DriveReady == 0U)) {
        if (DspBoot == 0U) {
            Err = SvcSysStat_Get(SVC_APP_STAT_DSP_BOOT, &DspStatus);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcSysStat_Get failed %u", Err, 0U);
            } else {
                if (DspStatus.Status == SVC_APP_STAT_DSP_BOOT_DONE) {
                    DspBoot = 1U;
                }
            }
        }

        if (DriveReady == 0U) {
            Err = SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveStatus);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcStgMonitor_GetDriveInfo failed %u", Err, 0U);
            } else {
                if (DriveStatus.IsExist > 0U) {
                    DriveReady = 1U;
                }
            }
        }

        if ((DspBoot == 0U) || (DriveReady == 0U)) {
            Err = AmbaKAL_TaskSleep(200U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "AmbaKAL_TaskSleep failed %u", Err, 0U);
            }
        }
    }

    if (SVC_OK == RetVal) {
        Err = SvcSysStat_Register(SVC_APP_STAT_MENU, PbkTask_MenuStatusCallback, &MenuCtrlID);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcButtonCtrl_Register failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK == RetVal) {
        SvcGui_Register(VoutOfPbkGui, PBK_GUI_LEVEL, "PbkTask", PbkTask_GuiDraw_VidInfo, PbkTask_GuiDraw_Update);
    }

    if (RetVal == SVC_OK) {
        Err = SvcPbkPictDisp_TaskCreate();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkPictDisp_TaskCreate failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        Err = SvcThmDisp_ThmViewStart(AMBA_DCF_FILE_TYPE_VIDEO, 0U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkPictDisp_TaskCreate failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        SvcLog_DBG(SVC_LOG_PLAYBACK_TASK, "ThumbnailStart done", 0U, 0U);
    }

    return RetVal;
}

/**
* The function to stop Thumbnail view
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_ThumbnailStop(void)
{
    UINT32 RetVal = SVC_OK, Err;

    AmbaMisra_TouchUnused(&RetVal);

    if (RetVal == SVC_OK) {
        Err = SvcPbkPictDisp_TaskDelete();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkPictDisp_TaskDelete failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        SvcGui_Unregister(VoutOfPbkGui, PBK_GUI_LEVEL);
    }

    if (SVC_OK == RetVal) {
        Err = SvcSysStat_Unregister(SVC_APP_STAT_MENU, MenuCtrlID);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcSysStat_Unregister failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* The function to init duplex functions
* @return ErrorCode
*/
UINT32 SvcPlaybackTask_DuplexStart(void)
{
    UINT32                 RetVal = SVC_OK, Enable = 0U, Err, Src = 0U, i;
    SVC_USER_PREF_s        *pSvcUserPref;
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

    /* if it's in duplex mode */
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &Src);
                if ((Err == SVC_OK) && (Src == SVC_VIN_SRC_MEM_DEC)) {
                    Enable  = 1U;
                    break;
                }
            }
        }
    }

    if (Enable == 1U) {
        Err = SvcSysStat_Register(SVC_APP_STAT_MENU, PbkTask_MenuStatusCallback, &MenuCtrlID);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcButtonCtrl_Register failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* The function to start loop playback
* @param [in]  FileNum
* @param [in]  FileIdx
* @param [in]  FeatureBits: PBK_LOOP_FLOOP
*                           PBK_LOOP_SEAMLESS
*/
void SvcPlaybackTask_LoopPbkStart(UINT32 FileNum, const UINT32 *pFileIdx, UINT32 FeatureBits)
{
    SVC_PBK_CTRL_CREATE_s Create;
    UINT32                Err, Rval = SVC_OK, i, Bit = 1U;

    for (i = 0U; i < FileNum; i++) {
        Create.FileName  = NULL;
        Create.FileIndex = pFileIdx[i];
        Create.Seamless  = 0U;
        Create.Speed     = 0U;
        Create.Direction = 0U;
        Create.StartTime = 0U;
        Create.Loop      = SVC_PBK_CTRL_LOOP_ONE_FILE;
        LoopPlayerBits  |= (Bit << i);

        if ((FeatureBits & PBK_LOOP_SEAMLESS) != 0U) {
            Create.Seamless = 1U;
        }

        if ((FeatureBits & PBK_LOOP_FLOOP) != 0U) {
            Create.Loop = SVC_PBK_CTRL_LOOP_ALL;
        }

        Err = SvcPbkCtrl_TaskCreate(i, &Create);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkCtrl_TaskCreate failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Err = SvcPbkCtrl_Start(LoopPlayerBits);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkCtrl_Start failed!(%u)", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        LoopPbkStart = 1U;
    }
}

/**
* The function to stop loop playback
*/
void SvcPlaybackTask_LoopPbkStop(void)
{
    UINT32                Err, Rval = SVC_OK, i;

    Err = SvcPbkCtrl_Stop(LoopPlayerBits);
    if (SVC_OK != Err) {
        SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkCtrl_Stop failed!(%u)", Err, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        for (i = 0U; i < (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM; i++) {
            if (((1UL << i) & LoopPlayerBits) != 0U) {
                Err = SvcPbkCtrl_TaskDelete(i);
                if (SVC_OK != Err) {
                    SvcLog_NG(SVC_LOG_PLAYBACK_TASK, "SvcPbkCtrl_TaskDelete failed!(%u)", Err, 0U);
                    Rval = SVC_NG;
                }
            }
        }
    }

    if (Rval == SVC_OK) {
        LoopPbkStart   = 0U;
        LoopPlayerBits = 0U;
    }
}
