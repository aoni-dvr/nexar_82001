/**
 *  @file SvcPbkPictDisp.c
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
 *  @details picture display related APIs
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaFS.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaMMU.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"
#include "AmbaGDMA.h"

#include "SvcPbkThmDisp.h"
#include "SvcTask.h"
#include "SvcErrCode.h"
#include "SvcPbkCtrl.h"
#include "SvcStgMonitor.h"
#include "SvcPbkStillDec.h"
#include "SvcPbkStillDisp.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcTask.h"
#include "SvcOsd.h"
#include "SvcBRateCalc.h"
#include "SvcClock.h"
#include "SvcLog.h"
#include "SvcPbkPictDisp.h"
#include "SvcVinSrc.h"
#include "SvcMem.h"
#include "SvcBuffer.h"

#define SVC_LOG_PBK_PICDISP     "PICDISP"

static SVC_PICT_DISP_MGR_s   PicDispMgr;
AMBA_KAL_EVENT_FLAG_t        SvcPictDispEventId;
static UINT32                SvcPictDispState = SVC_PICT_DISP_NONE_STATE;
static SVC_TASK_CTRL_s       PictDispTaskCtrl GNU_SECTION_NOZEROINIT;
static SVC_PICT_DISP_MEM_s   SvcPictDispMem;
static UINT8                 IsTaskCreated = 0U;
static SVC_PICT_DISP_INFO_s  PictDispInfo;

static UINT32 NextIndex(UINT32 Index, UINT32 Amount);
static UINT32 NextFileIndex(UINT32 Index, UINT32 Amount);
static UINT32 PrevFileIndex(UINT32 Index, UINT32 Amount);
static void* PictDispTaskEntry(void* EntryArg);
static UINT32 PictDispSingleViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex);
static UINT32 PictDispDecodeScreennail(UINT32 NumFile, const UINT32 *pFileIndex);
static UINT32 PictDispDisplayScreennail(UINT32 FileIndex, UINT32 ImgRotate);
static void   PictDispGetScreennailSize(const SVC_PICT_DISP_FILE_INFO_s *pFileInfo, UINT32 ImgRotate, UINT32 *Width, UINT32 *Height);
static UINT32 PictDispDisplayErrorPic(void);
static UINT32 PictDispParseFile(UINT32 FileIndex, SVC_PICT_DISP_FILE_INFO_s *pFileInfo);

static void PbkDebugUInt5(const char *FmtStr, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    if (pDispInfo->ShowDbgLog > 0U) {
        SvcWrap_PrintUL(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 NextIndex(UINT32 Index, UINT32 Amount)
{
    return ((Index == (Amount - 1U)) ? 0U : (Index + 1U));
}

static UINT32 NextFileIndex(UINT32 Index, UINT32 Amount)
{
    return ((Index == Amount) ? 1U : (Index + 1U));
}

static UINT32 PrevFileIndex(UINT32 Index, UINT32 Amount)
{
    return ((Index == 1U) ? Amount : (Index - 1U));
}

static UINT32 GetYUVBufSize(UINT32 Width, UINT32 Height)
{
    UINT32 BufSize;

#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
    BufSize =  (ALIGN64(Width) * ALIGN16(Height) * 2U);
#else
    BufSize =  ((ALIGN64(Width) * ALIGN16(Height) * 3U) / 2U);
#endif

    return BufSize;
}

void PictDispVideoHandler(UINT32 ParamType, const void *pNewVal)
{
    static UINT32                   VideoStart = 0U, VideoPause = 0U;
    UINT32                          DecoderID = 0U, DecoderBits = 1U, Error = 0U;
    const UINT32                    *pVal;
    SVC_VIDPBK_INFO_s               *pInfo;
    SVC_PBK_CTRL_INFO_s             *pPbkCtrlInfo;
    SVC_PBK_CTRL_CREATE_s           Create;
    static UINT32                   Speed = 0U, Direction = 0U, Mode = SVC_VIDPBK_MODE_BEGIN, LatestTime = 0U;

    AmbaMisra_TouchUnused(&pVal);
    AmbaMisra_TouchUnused(&pInfo);
    AmbaMisra_TouchUnused(&Error);

    switch (ParamType) {
    case SVC_VIDPBK_PMT_START:
        if (VideoStart != 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is still running", 0U, 0U);
            Error = 1U;
        }

        if ((Error == 0U)) {
            Create.FileName  = NULL;
            Create.Seamless  = 0U;
            Create.Speed     = Speed;
            Create.Direction = Direction;
            Create.StartTime = 0U;
            Create.FileIndex = PicDispMgr.CurFileIndex;
            Create.Loop      = SVC_PBK_CTRL_LOOP_NONE;

            if (Mode == SVC_VIDPBK_MODE_CONT) {
                SvcPbkCtrl_InfoGet(DecoderID, &pPbkCtrlInfo);
                Create.StartTime = LatestTime;
            }

            if (SVC_OK != SvcPbkCtrl_TaskCreate(DecoderID, &Create)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkCtrl_TaskCreate failed!!", 0U, 0U);
                Error = 1U;
            }
        }

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_Start(DecoderBits)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkCtrl_Start failed!!", 0U, 0U);
                Error = 1U;
            }
        }

        if (Error != 0U) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "start failed!!", 0U, 0U);
        } else {
            VideoStart       = 1U;
            Mode             = SVC_VIDPBK_MODE_CONT;
            SvcPbkPictDisp_SetState(SVC_PICT_DISP_VIDEO_PLAY_STATE);
            SvcLog_OK(SVC_LOG_PBK_PICDISP, "start successfully!!", 0U, 0U);
        }

        break;
    case SVC_VIDPBK_PMT_STOP:
        if (VideoStart == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not running", 0U, 0U);
            Error = 1U;
        }

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_Stop(DecoderBits)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkCtrl_Stop failed!!", 0U, 0U);
                Error = 1U;
            }
        }

        SvcPbkCtrl_InfoGet(DecoderID, &pPbkCtrlInfo);
        LatestTime = pPbkCtrlInfo->pPlayer->LatestTime;

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_TaskDelete(DecoderID)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkCtrl_TaskDelete failed!!", 0U, 0U);
                Error = 1U;
            }
        }

        if (Error != 0U) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "stop failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_PBK_PICDISP, "stop successfully!!", 0U, 0U);
            VideoStart = 0U;
            VideoPause = 0U;
        }

        AmbaMisra_TypeCast(&pVal, &pNewVal);

        if ((Error == 0U) && (*pVal == SVC_VIDPBK_STOP_BACK)) {
            /* Go back to single view */
            SvcClock_FeatureCtrl(1U, (SVC_CLK_FEA_BIT_CODEC | SVC_CLK_FEA_BIT_HEVC));
            if (SVC_OK != SvcPbkPictDisp_SingleViewStart(PicDispMgr.DcfScanType, PicDispMgr.CurFileIndex)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_SingleViewStart failed!!", 0U, 0U);
            }
        }
        break;
    case SVC_VIDPBK_PMT_PAUSE:
        if (VideoStart == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not running", 0U, 0U);
            Error = 1U;
        }

        if (VideoPause != 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is already paused", 0U, 0U);
            Error = 1U;
        }

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_PAUSE)) {
                Error = 1U;
            }
        }

        if (Error != 0U) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "pause failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_PBK_PICDISP, "pause success!!", 0U, 0U);
            VideoPause = 1U;
        }
        break;
    case SVC_VIDPBK_PMT_RESUME:
        if (VideoStart == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not running", 0U, 0U);
            Error = 1U;
        }

        if (VideoPause == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not paused", 0U, 0U);
            Error = 1U;
        }

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_RESUME)) {
                Error = 1U;
            }
        }

        if (Error != 0U) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "resume failed!!", 0U, 0U);
        } else {
            SvcLog_OK(SVC_LOG_PBK_PICDISP, "resume successfully!!", 0U, 0U);
            VideoPause = 0U;
        }
        break;
    case SVC_VIDPBK_PMT_STEP:
        if (VideoStart == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not running", 0U, 0U);
            Error = 1U;
        }

        if (VideoPause == 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is not paused", 0U, 0U);
            Error = 1U;
        }

        if ((Error == 0U)) {
            if (SVC_OK != SvcPbkCtrl_TrickPlay(DecoderBits, AMBA_PLAYER_TRICK_STEP)) {
                Error = 1U;
            }
        }
        break;
    case SVC_VIDPBK_PMT_DIRECT:
        if (VideoStart != 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is still running", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            AmbaMisra_TypeCast(&pVal, &pNewVal);
            if (*pVal > (UINT32)AMBA_PLAYER_DIR_BACKWARD) {
                SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback direction should not exceed %u", AMBA_PLAYER_DIR_BACKWARD, 0U);
                Error = 1U;
            }
        }

        if (Error == 0U) {
            Direction = (UINT16)*pVal;
        }
        break;
    case SVC_VIDPBK_PMT_SPEED:
        if (VideoStart != 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is still running", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            AmbaMisra_TypeCast(&pVal, &pNewVal);
            if (*pVal > (UINT32)AMBA_PLAYER_SPEED_16X) {
                SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback speed should not exceed %u", AMBA_PLAYER_SPEED_16X, 0U);
                Error = 1U;
            }
        }

        if (Error == 0U) {
            Speed = (UINT16)*pVal;
        }
        break;
    case SVC_VIDPBK_PMT_MODE:
        if (VideoStart != 0U) {
            SvcLog_DBG(SVC_LOG_PBK_PICDISP, "Playback is still running", 0U, 0U);
            Error = 1U;
        }

        if (Error == 0U) {
            AmbaMisra_TypeCast(&pVal, &pNewVal);
            Mode = (UINT16)*pVal;
        }
        break;
    case SVC_VIDPBK_PMT_GETINFO:
        AmbaMisra_TypeCast(&pInfo, &pNewVal);
        pInfo->VideoStart  = VideoStart;
        pInfo->VideoPause  = VideoPause;
        pInfo->VideoSpeed  = Speed;
        pInfo->VideoDirect = Direction;
        break;
    default:
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "Unknown parameter!!", 0U, 0U);
        break;
    }

    AmbaMisra_TouchUnused(&Error);
}

static void* PictDispTaskEntry(void* EntryArg)
{
    UINT32                     KeyCode = 0U, State, LoopFlag = 1U, InputVal = 0U, Err;
    AMBA_KAL_MSG_QUEUE_INFO_s  QueInfo = {0};
    SVC_VIDPBK_INFO_s          VidInfo = {0};

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopFlag);
    while (LoopFlag == 1U) {
        Err = AmbaKAL_MsgQueueReceive(&(PicDispMgr.MsgQueueId), &KeyCode, AMBA_KAL_WAIT_FOREVER);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_MsgQueueReceive failed %u", Err, 0U);
            LoopFlag = 0U;
        }

        State = SvcPbkPictDisp_GetState();
        switch(State) {
        case SVC_PICT_DISP_THM_VIEW_STATE:
            Err = SvcThmDisp_Handler(KeyCode);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcThmDisp_Handler failed %u", Err, 0U);
            }
            break;
        case SVC_PICT_DISP_VIDEO_PLAY_STATE:
            if (KeyCode == SVC_PICT_DISP_KEY_OK) {
                PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);
                if (VidInfo.VideoStart == 0U) {
                    PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
                } else {
                    InputVal = SVC_VIDPBK_STOP_BACK;
                    PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);
                }
            }
            break;
        default:
            Err = SvcPbkPictDisp_Handler(KeyCode);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Handler failed %u", Err, 0U);
            }
            break;
        }

        Err = AmbaKAL_MsgQueueQuery(&(PicDispMgr.MsgQueueId), &QueInfo);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_MsgQueueQuery failed %u", Err, 0U);
        }
        if (QueInfo.NumEnqueued == 0U) {
            Err = AmbaKAL_EventFlagSet(&SvcPictDispEventId, SVC_PICT_DISP_EVENT_TASK_IDLE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
            }
        }
    }

    return NULL;
}

static UINT32 PictDispSingleViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex)
{
    UINT32                          FileAmount = 0U;
    PBK_STILL_DISP_VOUT_INFO_s      DispInfo[AMBA_DSP_MAX_VOUT_NUM];
    SVC_SDEC_CTRL_s                 SDecCtrl = {0};
    SVC_PBK_CTRL_INFO_s             *pInfo = NULL;
    const SVC_PBK_CTRL_VOUT_INFO_s  *pVout;
    SVC_STG_MONI_DRIVE_INFO_s       DriveInfo;
    UINT32                          RetVal = SVC_OK, Err, i, j, DispNum = 0U;
    const SVC_PICT_DISP_INFO_s      *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    SvcPbkStillDisp_WaitDspDispIdle();
    SvcSDecConfig.ImgNum = 0;
    SDecCtrl.MainBufBase = SvcPictDispMem.JpegDecOutBuf.BufBase;
    SDecCtrl.MainBufSize = SvcPictDispMem.JpegDecOutBuf.BufSize;
    SDecCtrl.BitsBufBase = SvcPictDispMem.JpegDecBitsBuf.BufBase;
    SDecCtrl.BitsBufSize = SvcPictDispMem.JpegDecBitsBuf.BufSize;
    SDecCtrl.pFbEventId  = &SvcPictDispEventId;
    SvcPbkStillDec_Setup(&SDecCtrl);

    PbkDebugUInt5("PictDispSingleViewStart MainBufBase 0x%llx, MainBufSize %d, BitsBufBase 0x%llx, BitsBufSize %d", \
                  SDecCtrl.MainBufBase, SDecCtrl.MainBufSize, SDecCtrl.BitsBufBase, SDecCtrl.BitsBufSize, 0U);

    SvcPbkCtrl_InfoGet(0U, &pInfo);

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        for (i = 0U; i < PictDispInfo.DispNum; i++) {
            for (j = 0U; j < pInfo->VoutNum; j++) {
                pVout = &(pInfo->VoutInfo[j]);
                if (PictDispInfo.Disp[i].VoutId == pVout->VoutIdx) {
                    DispInfo[DispNum].VoutId      = pVout->VoutIdx;
                    DispInfo[DispNum].Width       = pVout->VoutWindow.Width;
                    DispInfo[DispNum].Height      = pVout->VoutWindow.Height;
                    DispInfo[DispNum].VoutAR      = pVout->VoutWindowAR;
                    DispInfo[DispNum].Pitch       = ALIGN64(DispInfo[DispNum].Width);
                    DispInfo[DispNum].RotateFlip  = pVout->VoutRotateFlip;
                    DispNum++;
                    break;
                }
            }
        }
    } else {
        DispInfo[DispNum].VoutId      = pDispInfo->Disp[0].VoutId;
        DispInfo[DispNum].Width       = SVC_PICT_DISP_SCRNAIL_BUF_WIDTH;
        DispInfo[DispNum].Height      = SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT;
        DispInfo[DispNum].VoutAR      = (DispInfo[DispNum].Width << 16U) / DispInfo[DispNum].Height;
        DispInfo[DispNum].Pitch       = ALIGN64(pDispInfo->Disp[0].DispWin.Width);
        DispInfo[DispNum].RotateFlip  = 0U;
        DispNum++;
    }

    Err = SvcPbkStillDisp_Setup(&SvcPictDispEventId, DispNum, DispInfo);  /* Register feedback event id */
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDisp_Setup failed %u", Err, 0U);
    }

    PicDispMgr.DcfScanType = DcfScanType;

    if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
        if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, PicDispMgr.DcfScanType, &FileAmount)) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "fail to get file amount", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "fail to get GetDriveInfo", 0U, 0U);
    }

    if (RetVal != SVC_NG) {
        if (FileAmount > 0U) {
            PicDispMgr.CurFileIndex = ((FileIndex == 0U) || (FileIndex > FileAmount)) ? FileAmount : FileIndex;
            PicDispMgr.ImgRotate = AMBA_DSP_ROTATE_0;

            PbkDebugUInt5("PictDispSingleViewStart famount %d, findex %d", FileAmount, PicDispMgr.CurFileIndex, 0U, 0U, 0U);

            Err = PictDispDecodeScreennail(1U, &PicDispMgr.CurFileIndex);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDecodeScreennail failed %u", Err, 0U);
            }
            if (PicDispMgr.FileInfo.Error == 0U) {
                Err = PictDispDisplayScreennail(PicDispMgr.CurFileIndex, PicDispMgr.ImgRotate);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayScreennail failed %u", Err, 0U);
                }
                PicDispMgr.ZoomRatio = SVC_PICT_DISP_ZOOM_RATIO_1X;
                PicDispMgr.ZoomCenterX = (PicDispMgr.FileInfo.Info.Jpeg.Width >> 1U);
                PicDispMgr.ZoomCenterY = (PicDispMgr.FileInfo.Info.Jpeg.Height >> 1U);
            } else {
                Err = PictDispDisplayErrorPic();
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayErrorPic failed %u", Err, 0U);
                }
            }
        }
        SvcPbkPictDisp_SetState(SVC_PICT_DISP_SINGLE_VIEW_STATE);
    }

    return RetVal;
}

static UINT32 PictDispDecodeScreennail(UINT32 NumFile, const UINT32 *pFileIndex)
{
    UINT32                       i, ActualFlags, Err, ValidCount = 0U, RetVal = SVC_OK;
    ULONG                        YSize, UVSize;
    UINT8                        *pBuff;
    AMBA_DSP_YUV_IMG_BUF_s       ClrBuf;
    SVC_SDEC_DEC_REPORT_s        *pDecReport;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    PbkDebugUInt5("PictDispDecodeScreennail FileIndex %d", *pFileIndex, 0U, 0U, 0U, 0U);

    for (i = 0U; i < NumFile; i++) {
        PicDispMgr.SCRBufIndex = NextIndex(PicDispMgr.SCRBufIndex, SVC_PICT_DISP_SCRNAIL_BUF_NUM);

        YSize   = (ULONG)SVC_PICT_DISP_SCRNAIL_BUF_PITCH * (ULONG)ALIGN16(SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
        #if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
        UVSize  = YSize;
        #else
        UVSize  = YSize / 2U;
        #endif


        ClrBuf.BaseAddrY  = SvcPictDispMem.ScrYuvBuf.BufBase + (PicDispMgr.SCRBufIndex * (YSize + UVSize));
        ClrBuf.BaseAddrUV = SvcPictDispMem.ScrYuvBuf.BufBase + (PicDispMgr.SCRBufIndex * (YSize + UVSize)) + YSize;

        AmbaMisra_TypeCast(&pBuff, &(ClrBuf.BaseAddrY));
        AmbaUtility_MemorySetU8(pBuff, 0x00, (UINT32)YSize);

        AmbaMisra_TypeCast(&pBuff, &(ClrBuf.BaseAddrUV));
        AmbaUtility_MemorySetU8(pBuff, 0x80, (UINT32)UVSize);

        if (SVC_OK != PictDispParseFile(*pFileIndex, &PicDispMgr.FileInfo)) {
            RetVal = SVC_NG;
            break;
        }

        SvcSDecConfig.ImgNum++;
        SvcWrap_strcpy(&(SvcSDecConfig.Config[ValidCount].FName[0]), 64, &PicDispMgr.FileInfo.FName[0]);

        if (PicDispMgr.FileInfo.FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
            if ((PicDispMgr.FileInfo.Info.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_H264) || (PicDispMgr.FileInfo.Info.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_AVC)) {
                SvcSDecConfig.Config[ValidCount].Type = SDEC_IMG_TYPE_H264_IFRAME;
            } else if ((PicDispMgr.FileInfo.Info.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_H265) || (PicDispMgr.FileInfo.Info.Mov.VideoTrack[0].MediaId == AMBA_FORMAT_MID_HVC)) {
                SvcSDecConfig.Config[ValidCount].Type = SDEC_IMG_TYPE_HEVC_IFRAME;
            } else {
                SvcSDecConfig.Config[ValidCount].Type = SDEC_IMG_TYPE_MJPEG;
            }
            Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[ValidCount].Info.Mov, &PicDispMgr.FileInfo.Info.Mov, sizeof(PicDispMgr.FileInfo.Info.Mov));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
            }
        } else {
            SvcSDecConfig.Config[ValidCount].Type                  = SDEC_IMG_TYPE_MAIN;
            SvcSDecConfig.Config[ValidCount].Info.Jpeg.Width       = (UINT16)PicDispMgr.FileInfo.Info.Jpeg.Width;
            SvcSDecConfig.Config[ValidCount].Info.Jpeg.Height      = (UINT16)PicDispMgr.FileInfo.Info.Jpeg.Height;
            SvcSDecConfig.Config[ValidCount].Info.Jpeg.BitsOffset  = PicDispMgr.FileInfo.Info.Jpeg.FullviewPos;
            SvcSDecConfig.Config[ValidCount].Info.Jpeg.BitsSize    = PicDispMgr.FileInfo.Info.Jpeg.FullviewSize;
            SvcSDecConfig.Config[ValidCount].Info.Jpeg.Orientation = 0;

            PbkDebugUInt5("ScreenWidth %d, ScreenHeight %d, ScreenPos %d, ScreenSize %d", \
                        PicDispMgr.FileInfo.Info.Jpeg.ScreenWidth, PicDispMgr.FileInfo.Info.Jpeg.ScreenHeight, PicDispMgr.FileInfo.Info.Jpeg.ScreenPos, PicDispMgr.FileInfo.Info.Jpeg.ScreenSize, 0U);
            PbkDebugUInt5("thumb ScreenWidth %d, ScreenHeight %d, ScreenPos %d, ScreenSize %d", \
                        PicDispMgr.FileInfo.Info.Jpeg.ThumbWidth, PicDispMgr.FileInfo.Info.Jpeg.ThumbHeight, PicDispMgr.FileInfo.Info.Jpeg.ThumbPos, PicDispMgr.FileInfo.Info.Jpeg.ThumbSize, 0U);

            PbkDebugUInt5("full ScreenWidth %d, ScreenHeight %d, ScreenPos %d, ScreenSize %d", \
                        PicDispMgr.FileInfo.Info.Jpeg.Width, PicDispMgr.FileInfo.Info.Jpeg.Height, PicDispMgr.FileInfo.Info.Jpeg.FullviewPos, PicDispMgr.FileInfo.Info.Jpeg.FullviewSize, 0U);

            if (PicDispMgr.FileInfo.Info.Jpeg.ScreenDHTStat == -1) {
                SvcSDecConfig.Config[0].Info.Jpeg.EnableJpegTrans = 1;
                Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[0].Info.Jpeg.JpegHeaderInfo,
                                       &PicDispMgr.FileInfo.Info.Jpeg.JPEGScreenHeaderInfo,
                                       sizeof(AMBA_EXIF_JPEG_HEADER_INFO_s));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                }
                Err = AmbaWrap_memcpy(&SvcSDecConfig.Config[0].Info.Jpeg.JpegInfo,
                                       &PicDispMgr.FileInfo.Info.Jpeg.JPEGScreenInfo,
                                       sizeof(AMBA_EXIF_JPEG_INFO_s));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                }
            } else {
                SvcSDecConfig.Config[0].Info.Jpeg.EnableJpegTrans = 0;
            }
        }

        SvcSDecConfig.Config[ValidCount].SubEnable     = 1;
        SvcSDecConfig.Config[ValidCount].SubRotateFlip = 0;
        SvcSDecConfig.Config[ValidCount].SubYuvOffsetX = 0;
        SvcSDecConfig.Config[ValidCount].SubYuvOffsetY = 0;
        SvcSDecConfig.Config[ValidCount].SubBufYBase   = ClrBuf.BaseAddrY;
        SvcSDecConfig.Config[ValidCount].SubBufUVBase  = ClrBuf.BaseAddrUV;
        SvcSDecConfig.Config[ValidCount].SubYuvWidth   = (UINT16)SVC_PICT_DISP_SCRNAIL_BUF_WIDTH;
        SvcSDecConfig.Config[ValidCount].SubYuvHeight  = (UINT16)SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT;
        SvcSDecConfig.Config[ValidCount].SubBufPitch   = (UINT16)SVC_PICT_DISP_SCRNAIL_BUF_PITCH;
        SvcSDecConfig.Config[ValidCount].SubBufHeight  = (UINT16)ALIGN16(SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
        ValidCount++;
    }

    if (RetVal != SVC_NG) {
        if (0U != ValidCount) {
            SvcSDecConfig.ImgNum = (UINT16)ValidCount;
            Err = SvcPbkStillDec_StartDecode();
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDec_StartDecode failed %u", Err, 0U);
            }

            if (AmbaKAL_EventFlagGet(&SvcPictDispEventId,
                                     SDEC_EVENT_DECODE_DONE,
                                     AMBA_KAL_FLAGS_ANY,
                                     AMBA_KAL_FLAGS_CLEAR_AUTO,
                                     &ActualFlags,
                                     AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDecodeScreennail, timeout", 0U, 0U);
                RetVal = SVC_NG;
            } else {
                SvcPbkStillDec_QueryDecResult(&pDecReport);
                if (pDecReport->pStatus[0] != 0U) {
                    PicDispMgr.FileInfo.Error = 1U;
                } else {
                    PicDispMgr.FileInfo.Error = 0U;
                }
            }
        } else {
            RetVal = SVC_NG;
        }
    }

    PbkDebugUInt5("PictDispDecodeScreennail done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

static UINT32 PictDispDisplayScreennail(UINT32 FileIndex, UINT32 ImgRotate)
{
    UINT32                          ImgWidth = 0U, ImgHeight = 0U;
    ULONG                           YSize, UVSize;
    PBK_STILL_DISP_CONFIG_s         DispConfig[AMBA_DSP_MAX_VOUT_NUM];
    PBK_STILL_DISP_PARAM_s          DispImg[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                          DispWidth1X = 0U, DispHeight1X = 0U;
    UINT32                          ActualFlags, ValidVoutNum = 0U;
    PBK_STILL_DISP_VOUT_FRM_BUF_s   VoutBuf;
    UINT32                          RetVal = SVC_OK, Err, i;

    PbkDebugUInt5("PictDispDisplayScreennail FileIndex %d", FileIndex, 0U, 0U, 0U, 0U);
    PictDispGetScreennailSize(&PicDispMgr.FileInfo, ImgRotate, &ImgWidth, &ImgHeight);

    for (i = 0U; i < PictDispInfo.DispNum; i++) {
        if (i < AMBA_DSP_MAX_VOUT_NUM) {
            SvcPbkStillDisp_GetCurrentVoutBuf(PictDispInfo.Disp[i].VoutId, &VoutBuf);
            SvcPbkPictDisp_CalcBaseDispSize(VoutBuf.Width, VoutBuf.Height, VoutBuf.BufAR,
                                            ((ImgWidth << 16U) / ImgHeight),
                                            &DispWidth1X, &DispHeight1X);
            PbkDebugUInt5("PictDispDisplayScreennail VoutBuf.Width %d, Height %d, BufAR %d, Width %d, Height %d", \
                           VoutBuf.Width, VoutBuf.Height, VoutBuf.BufAR, ImgWidth, ImgHeight);
            PbkDebugUInt5("DispWidth1X %d, DispHeight1X %d", DispWidth1X, DispHeight1X, 0U, 0U, 0U);

            YSize   = (ULONG)SVC_PICT_DISP_SCRNAIL_BUF_PITCH * (ULONG)ALIGN16(SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
            #if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            UVSize  = YSize;
            #else
            UVSize  = YSize / 2U;
            #endif

            DispImg[ValidVoutNum].SrcBufYBase   = SvcPictDispMem.ScrYuvBuf.BufBase + (PicDispMgr.SCRBufIndex * (YSize + UVSize));
            DispImg[ValidVoutNum].SrcBufUVBase  = SvcPictDispMem.ScrYuvBuf.BufBase + (PicDispMgr.SCRBufIndex * (YSize + UVSize)) + YSize;
            DispImg[ValidVoutNum].SrcOffsetX    = 0U;
            DispImg[ValidVoutNum].SrcOffsetY    = 0U;
            DispImg[ValidVoutNum].SrcWidth      = SVC_PICT_DISP_SCRNAIL_BUF_WIDTH;
            DispImg[ValidVoutNum].SrcHeight     = SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT;
            DispImg[ValidVoutNum].SrcBufPitch   = SVC_PICT_DISP_SCRNAIL_BUF_PITCH;
            DispImg[ValidVoutNum].SrcBufHeight  = ALIGN16(SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
            DispImg[ValidVoutNum].SrcChromaFmt  = SVCAG_SDEC_DISP_FORMAT;
            DispImg[ValidVoutNum].RotateFlip    = ImgRotate;
            DispImg[ValidVoutNum].DstOffsetX    = ((VoutBuf.Width - DispWidth1X) >> 1U);
            DispImg[ValidVoutNum].DstOffsetX    = DispImg[ValidVoutNum].DstOffsetX & 0xFFFFFFFEU;

            DispImg[ValidVoutNum].DstOffsetY    = ((VoutBuf.Height - DispHeight1X) >> 1U);
            DispImg[ValidVoutNum].DstOffsetY    = DispImg[ValidVoutNum].DstOffsetY & 0xFFFFFFFEU;
            DispImg[ValidVoutNum].DstWidth      = DispWidth1X;
            DispImg[ValidVoutNum].DstHeight     = DispHeight1X;

            DispConfig[ValidVoutNum].VoutIdx      = PictDispInfo.Disp[i].VoutId;
            DispConfig[ValidVoutNum].FlushVoutBuf = 1U;
            DispConfig[ValidVoutNum].NumImg       = 1U;
            DispConfig[ValidVoutNum].pParam       = &(DispImg[ValidVoutNum]);
            ValidVoutNum++;
        }

    }

    Err = SvcPbkStillDisp_SingleFrame(ValidVoutNum, &(DispConfig[0]));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDisp_SingleFrame failed %u", Err, 0U);
    }

    if (AmbaKAL_EventFlagGet(&SvcPictDispEventId,
                             STILL_DISP_EVENT_DISP_TO_VOUT,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_AUTO,
                             &ActualFlags,
                             AMBA_KAL_WAIT_FOREVER) != SVC_OK) {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PicDisp_DisplayFullviewAndPIP, timeout", 0U, 0U);
        RetVal = SVC_NG;
    }

    PbkDebugUInt5("PictDispDisplayScreennail done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

static void PictDispGetScreennailSize(const SVC_PICT_DISP_FILE_INFO_s *pFileInfo, UINT32 ImgRotate, UINT32 *Width, UINT32 *Height)
{
    if (((((ImgRotate == AMBA_DSP_ROTATE_0) ||
        (ImgRotate == AMBA_DSP_ROTATE_180)) ||
        (ImgRotate == AMBA_DSP_ROTATE_0_HORZ_FLIP)) ||
        (ImgRotate == AMBA_DSP_ROTATE_180_HORZ_FLIP))) {
        if (pFileInfo->FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
            *Width  = pFileInfo->Info.Jpeg.ScreenWidth;
            *Height = pFileInfo->Info.Jpeg.ScreenHeight;
        } else {
            *Width  = pFileInfo->Info.Mov.VideoTrack[0].Width;
            *Height = pFileInfo->Info.Mov.VideoTrack[0].Height;
        }
    } else {
        if (pFileInfo->FileType == AMBA_DCF_FILE_TYPE_IMAGE) {
            *Width  = pFileInfo->Info.Jpeg.ScreenHeight;
            *Height = pFileInfo->Info.Jpeg.ScreenWidth;
        } else {
            *Width  = pFileInfo->Info.Mov.VideoTrack[0].Height;
            *Height = pFileInfo->Info.Mov.VideoTrack[0].Width;
        }
    }
}

static UINT32 PictDispDisplayErrorPic(void)
{
    PBK_STILL_DISP_CONFIG_s  DispConfig[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                   ValidVoutNum = 0U, RetVal = SVC_OK, i, VoutId;

    for (i = 0U; i < PictDispInfo.DispNum; i++) {
        VoutId = PictDispInfo.Disp[i].VoutId;
        if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
            DispConfig[ValidVoutNum].VoutIdx       = VoutId;
            DispConfig[ValidVoutNum].FlushVoutBuf  = 1;
            DispConfig[ValidVoutNum].NumImg        = 0;
            DispConfig[ValidVoutNum].pParam        = NULL;
            ValidVoutNum++;
        }
    }

    RetVal = SvcPbkStillDisp_SingleFrame(ValidVoutNum, &DispConfig[0]);

    return RetVal;
}

static UINT32 PictDispParseFile(UINT32 FileIndex, SVC_PICT_DISP_FILE_INFO_s *pFileInfo)
{
    AMBA_FS_FILE               *MediaFile;
    AMBA_FS_FILE_INFO_s        pStat = {0};
    SVC_PBK_CTRL_INFO_s        *pInfo = NULL;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;
    UINT32                     Rval = SVC_OK, Err;
    AMBA_PLAYER_s              Player;
    void                       *pHdlr = NULL;

    pFileInfo->Error = 1;
    SvcPbkCtrl_InfoGet(0U, &pInfo);
    if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
        PbkDebugUInt5("PictDispParseFile ftype %d", (UINT32)PicDispMgr.DcfScanType, 0U, 0U, 0U, 0U);

        if (SVC_OK != AmbaDCF_GetFileName(DriveInfo.DriveID, 0U, PicDispMgr.DcfScanType, &pFileInfo->FName[0], FileIndex)) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispParseFile, AmbaDCF_GetFileName failed!!", 0U, 0U);
            Rval = SVC_NG;
        } else {
            Err = SvcPbkPictDisp_GetFileType(&pFileInfo->FName[0], &pFileInfo->FileType);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_GetFileType failed %u", Err, 0U);
            }
            if (pFileInfo->FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                Err = AmbaStreamWrap_GetHdlr(&pHdlr);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                    Rval = SVC_NG;
                }

                Err = AmbaPlayer_Parse(&Player, pHdlr, &pFileInfo->FName[0], AMBA_PLAYER_MTYPE_MOVIE);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaPlayer_Parse failed %u", Err, 0U);
                    Rval = SVC_NG;
                }

                Err = AmbaWrap_memcpy(&(pFileInfo->Info.Mov), &(Player.ExpMov), sizeof(Player.ExpMov));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                }
            } else {
                /* parse jpeg file */
                Err = AmbaFS_FileOpen(&pFileInfo->FName[0], "r", &MediaFile);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaFS_Open failed %u", Err, 0U);
                    Rval = SVC_NG;
                }

                Err = AmbaFS_GetFileInfo(&pFileInfo->FName[0], &pStat);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaFS_GetFileInfo failed %u", Err, 0U);
                    Rval = SVC_NG;
                }

                Err = AmbaWrap_memset(&pFileInfo->Info.Jpeg, 0x0, sizeof(AMBA_EXIF_IMAGE_INFO_s));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
                }

                Err = AmbaExifFullParse(MediaFile, &pFileInfo->Info.Jpeg, &pStat);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaExifFullParse failed %u", Err, 0U);
                    Rval = SVC_NG;
                } else {
                    if ((pFileInfo->Info.Jpeg.YUVType != 0x21U) && (pFileInfo->Info.Jpeg.YUVType != 0x22U)) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispParseFile: Unsupported YUV fmt %x", pFileInfo->Info.Jpeg.YUVType, 0U);
                        Rval = SVC_NG;
                    }
                }

                Err = AmbaFS_FileClose(MediaFile);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaFS_Close failed %u", Err, 0U);
                    Rval = SVC_NG;
                }
            }
        }
    }

    return Rval;
}


/**
* Querey pic display memory size
* @param [in]  Mode playback mode or duplec mode
* @param [in]  pCfg resolution config setting
* @param [out]  pMemSize buffer size
*/
void SvcPbkPictDisp_QuerryMem(UINT32 Mode, const SVC_RES_CFG_s* pCfg, UINT32 *pMemSize)
{
    UINT32                 i, Rsize = 0U, Err, FovSrc = 0U, DpxId = 0U, BufSize;
    const SVC_STRM_CFG_s   *pStrmCfg;

    if (Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        /* DispYuvBuf */
        for (i = 0U; i < pCfg->DispNum; i++) {
            pStrmCfg = &(pCfg->DispStrm[i].StrmCfg);

            BufSize = GetYUVBufSize(pStrmCfg->Win.Width, pStrmCfg->Win.Height);

            Rsize += BufSize * (UINT32)STILL_DISP_VOUT_FRAME_NUM;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }

        /* ThmYuvBuf */
        {
            BufSize = GetYUVBufSize(SVC_PICT_DISP_THM_BUF_PITCH, SVC_PICT_DISP_THM_BUF_HEIGHT);

            Rsize += BufSize * SVC_PICT_DISP_THM_BUF_NUM;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }

        /* ScrYuvBuf */
        {
            BufSize = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);

            Rsize += BufSize * SVC_PICT_DISP_SCRNAIL_BUF_NUM;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }

    } else if (Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        /* DispYuvBuf */
        for (i = 0U; i < pCfg->FovNum; i++) {
            Err = SvcResCfg_GetFovSrc(i, &FovSrc);
            if ((Err == SVC_OK) && (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                DpxId = i;

                BufSize = GetYUVBufSize(pCfg->FovCfg[DpxId].RawWin.Width, pCfg->FovCfg[DpxId].RawWin.Height);

                Rsize += BufSize;
                Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
            }
        }

        /* ThmYuvBuf */
        {
            BufSize = GetYUVBufSize(SVC_PICT_DISP_THM_BUF_DPX_WIDTH, SVC_PICT_DISP_THM_BUF_DPX_HEIGHT);

            Rsize += BufSize * SVC_PICT_DISP_THM_BUF_NUM;
            Rsize  = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }

        /* ScrYuvBuf */
        {
            BufSize = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);

            Rsize += BufSize * SVC_PICT_DISP_SCRNAIL_BUF_NUM;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }


        /* Y2YSrcBuf */
        {
            BufSize = GetYUVBufSize((UINT32)pCfg->FovCfg[DpxId].RawWin.Width + (UINT32)AMBA_CACHE_LINE_SIZE, pCfg->FovCfg[DpxId].RawWin.Height);

            Rsize += BufSize;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }

        /* Y2YDstBuf */
        {
            BufSize = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);

            Rsize += BufSize;
            Rsize = GetAlignedValU32(Rsize, (UINT32)AMBA_CACHE_LINE_SIZE);
        }
    } else {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_QuerryMem unknown mode %u", Mode, 0U);
    }

    SvcLog_OK(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_QuerryMem 0x%x", Rsize, 0U);

    *pMemSize = Rsize;
}

/**
* Set pic display buffer base
* @param [in]  pCfg resolution config setting
* @param [in]  pInit intial setting
*/
void SvcPbkPictDisp_Init(const SVC_RES_CFG_s* pCfg, const SVC_PICT_DISP_INIT_s *pInit)
{
    UINT32                 i, Rsize, Size, Err, Rval = SVC_OK, FovSrc = 0U;
    ULONG                  Base;
    SVC_PICT_DISP_MEM_s    *pMem = &SvcPictDispMem;
    const SVC_STRM_CFG_s   *pStrmCfg;

    Err = AmbaWrap_memset((&PictDispInfo), 0, sizeof(SVC_PICT_DISP_INFO_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    if (pInit->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        PictDispInfo.Mode        = pInit->Mode;
        PictDispInfo.ShowDbgLog  = pInit->ShowDbgLog;
    } else if (pInit->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        PictDispInfo.Mode        = pInit->Mode;
        PictDispInfo.ShowDbgLog  = pInit->ShowDbgLog;
        PictDispInfo.FovId       = pInit->FovId;
        PictDispInfo.Y2YCtxId    = pInit->Y2YCtxId;
    } else {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init unknown mode %u", pInit->Mode, 0U);
        Rval = SVC_NG;
    }

    if (Rval == SVC_OK) {
        PictDispInfo.DispNum = 0U;

        pMem->JpegDecBitsBuf = pInit->BufInit.JpegDecBits;
        pMem->JpegDecOutBuf  = pInit->BufInit.JpegDecOut;

        Base  = pInit->BufInit.PictDisp.BufBase;
        Rsize = pInit->BufInit.PictDisp.BufSize;

        if (PictDispInfo.Mode  == SVC_PICT_DISP_MODE_PLAYBACK) {
            /* DispYuvBuf */
            for (i = 0U; i < pCfg->DispNum; i++) {
                pStrmCfg = &(pCfg->DispStrm[i].StrmCfg);

                Size  = GetYUVBufSize((UINT32)pStrmCfg->Win.Width, pStrmCfg->Win.Height);
                Size  = Size * (UINT32)STILL_DISP_VOUT_FRAME_NUM;
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if ((Rsize >= Size) && (PictDispInfo.DispNum < AMBA_DSP_MAX_VOUT_NUM)) {
                    pMem->DispYuvBuf[pCfg->DispStrm[i].VoutID].BufBase  = Base;
                    pMem->DispYuvBuf[pCfg->DispStrm[i].VoutID].BufSize  = Size;

                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.OffsetX = 0U;
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.OffsetY = 0U;
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.Width   = pStrmCfg->Win.Width;
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.Height  = pStrmCfg->Win.Height;
                    PictDispInfo.Disp[PictDispInfo.DispNum].VoutId          = pCfg->DispStrm[i].VoutID;
                    PictDispInfo.DispNum++;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }

            /* ThmYuvBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize(SVC_PICT_DISP_THM_BUF_PITCH, SVC_PICT_DISP_THM_BUF_HEIGHT);
                Size  = Size * SVC_PICT_DISP_THM_BUF_NUM;
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->ThmYuvBuf.BufBase = Base;
                    pMem->ThmYuvBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }

            /* SrcYuvBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
                Size  = Size * SVC_PICT_DISP_SCRNAIL_BUF_NUM;
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->ScrYuvBuf.BufBase = Base;
                    pMem->ScrYuvBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }
        } else if (PictDispInfo.Mode == SVC_PICT_DISP_MODE_DUPLEX) {
            /* DispYuvBuf */
            Err = SvcResCfg_GetFovSrc(PictDispInfo.FovId, &FovSrc);
            if ((Err == SVC_OK) && (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                Size  = GetYUVBufSize(pCfg->FovCfg[PictDispInfo.FovId].RawWin.Width, pCfg->FovCfg[PictDispInfo.FovId].RawWin.Height);
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);

                if ((Rsize >= Size) && (PictDispInfo.DispNum < AMBA_DSP_MAX_VOUT_NUM)) {
                    pMem->DispYuvBuf[0U].BufBase  = Base;
                    pMem->DispYuvBuf[0U].BufSize  = Size;

                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.OffsetX = 0U;
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.OffsetY = 0U;
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.Width   = (UINT16)ALIGN64(pCfg->FovCfg[PictDispInfo.FovId].RawWin.Width);
                    PictDispInfo.Disp[PictDispInfo.DispNum].DispWin.Height  = (UINT16)ALIGN16(pCfg->FovCfg[PictDispInfo.FovId].RawWin.Height);
                    PictDispInfo.Disp[PictDispInfo.DispNum].VoutId          = 0U;
                    PictDispInfo.DispNum++;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            } else {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "FovId %u is not SVC_VIN_SRC_MEM_DEC", PictDispInfo.FovId, 0U);
                Rval = SVC_NG;
            }

            /* ThmYuvBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize(SVC_PICT_DISP_THM_BUF_DPX_WIDTH, SVC_PICT_DISP_THM_BUF_DPX_HEIGHT);
                Size  = Size * SVC_PICT_DISP_THM_BUF_NUM;
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->ThmYuvBuf.BufBase = Base;
                    pMem->ThmYuvBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }

            /* SrcYuvBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
                Size  = Size * SVC_PICT_DISP_SCRNAIL_BUF_NUM;
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->ScrYuvBuf.BufBase = Base;
                    pMem->ScrYuvBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }

            /* Y2YSrcBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize((UINT32)pCfg->FovCfg[PictDispInfo.FovId].RawWin.Width + (UINT32)AMBA_CACHE_LINE_SIZE, pCfg->FovCfg[PictDispInfo.FovId].RawWin.Height);
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->Y2YSrcBuf.BufBase = Base;
                    pMem->Y2YSrcBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }

            /* Y2YDstBuf */
            if (Rval == SVC_OK) {
                Size  = GetYUVBufSize(SVC_PICT_DISP_SCRNAIL_BUF_PITCH, SVC_PICT_DISP_SCRNAIL_BUF_HEIGHT);
                Size  = GetAlignedValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE);
                if (Rsize >= Size) {
                    pMem->Y2YDstBuf.BufBase = Base;
                    pMem->Y2YDstBuf.BufSize = Size;

                    Base  += Size;
                    Rsize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init BufSize too small (%u/%u)", Size, Rsize);
                    Rval = SVC_NG;
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init unknown mode %u", pInit->Mode, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        SvcLog_OK(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_Init done", 0U, 0U);
        AmbaMisra_TouchUnused(&Rsize);
        AmbaMisra_TouchUnused(&Base);
    }
}

/**
* Create pict display task
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_TaskCreate(void)
{
    static char PictDispMsgQName[32U];
    static char PictDispEvtName[]  = "PictDispFlgMgr";
    static UINT8 PictDispStack[SVC_PICT_DISP_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 RetVal = SVC_OK, Err;

    if (IsTaskCreated == 0U) {
        Err = AmbaWrap_memset((&PicDispMgr), 0, sizeof(SVC_PICT_DISP_MGR_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memset failed %u", Err, 0U);
        }
        if (SVC_OK != AmbaKAL_EventFlagCreate(&SvcPictDispEventId, PictDispEvtName)) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagCreate SvcPictDispEventId failed", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            ULONG  Num;

            Err = AmbaWrap_memset(PictDispMsgQName, 0, sizeof(PictDispMsgQName));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memset failed %u", Err, 0U);
            }

            AmbaMisra_TypeCast(&Num, &(PicDispMgr.MsgQueueId));
            if (0U < AmbaUtility_UInt32ToStr(PictDispMsgQName, 32U, (UINT32)Num, 16U)) {
                AmbaUtility_StringAppend(PictDispMsgQName, 32, "PicDispMgrMQ");
            }

            RetVal = AmbaKAL_MsgQueueCreate(&(PicDispMgr.MsgQueueId),
                                            PictDispMsgQName,
                                            (UINT32)sizeof(PicDispMgr.MsgQueue[0]),
                                            PicDispMgr.MsgQueue,
                                            (UINT32)sizeof(PicDispMgr.MsgQueue));

            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "QueueCreate failed %u", RetVal, 0U);
                RetVal = SVC_NG;
            } else {
                Err = AmbaKAL_EventFlagClear(&SvcPictDispEventId, 0xFFFFFFFFU);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
                }
                Err = AmbaKAL_EventFlagSet(&SvcPictDispEventId, SVC_PICT_DISP_EVENT_TASK_IDLE | SVC_PICT_DISP_EVENT_VOUT_IDLE);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
                }

                PictDispTaskCtrl.Priority    = SVC_PBK_PICT_DISP_TASK_PRI;
                PictDispTaskCtrl.EntryFunc   = PictDispTaskEntry;
                PictDispTaskCtrl.EntryArg    = 0U;
                PictDispTaskCtrl.pStackBase  = PictDispStack;
                PictDispTaskCtrl.StackSize   = SVC_PICT_DISP_STACK_SIZE;
                PictDispTaskCtrl.CpuBits     = SVC_PBK_PICT_DISP_TASK_CPU_BITS;

                if (SVC_OK != SvcTask_Create("PictDispTask", &PictDispTaskCtrl)) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispTask created failed!", 0U, 0U);
                    RetVal = SVC_NG;
                } else {
                    Err = SvcPbkStillDec_TaskCreate();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDec_TaskCreate failed %u", Err, 0U);
                    }
                    Err = SvcPbkStillDisp_TaskCreate();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDisp_TaskCreate failed %u", Err, 0U);
                    }

                    PicDispMgr.DcfScanType  = AMBA_DCF_FILE_TYPE_IMAGE;
                    PicDispMgr.ZoomRatio    = SVC_PICT_DISP_ZOOM_RATIO_1X;
                    PicDispMgr.CurFileIndex = 1;
                    PicDispMgr.ImgRotate    = AMBA_DSP_ROTATE_0;

                    Err = SvcThmDisp_Create();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcThmDisp_Create failed %u", Err, 0U);
                    }
                    SvcPbkPictDisp_SetState(SVC_PICT_DISP_INIT_STATE);
                    IsTaskCreated = 1U;
                }
            }
        }
    }

    return RetVal;
}

/**
* Delete pict display task
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_TaskDelete(void)
{
    UINT32 RetVal = SVC_OK, Err;

    if (IsTaskCreated == 1U) {
        Err = SvcPbkPictDisp_WaitTaskIdle();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_WaitTaskIdle failed %u", Err, 0U);
        }
        SvcPbkPictDisp_SetState(SVC_PICT_DISP_NONE_STATE);

        if (SVC_OK != SvcTask_Destroy(&PictDispTaskCtrl)) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_TaskDelete, failed to destroy task", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            if (SVC_OK != AmbaKAL_MsgQueueDelete(&(PicDispMgr.MsgQueueId))) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_TaskDelete, failed to delete msg que", 0U, 0U);
                RetVal = SVC_NG;
            } else {
                if (SVC_OK != AmbaKAL_EventFlagDelete(&SvcPictDispEventId)) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SSvcPbkPictDisp_TaskDelete, failed to delete event", 0U, 0U);
                    RetVal = SVC_NG;
                } else {
                    Err = SvcPbkStillDec_TaskDelete();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDec_TaskDelete failed %u", Err, 0U);
                    }

                    Err = SvcPbkStillDisp_TaskDelete();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkStillDisp_TaskDelete failed %u", Err, 0U);
                    }

                    Err = SvcThmDisp_Delete();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcThmDisp_Delete failed %u", Err, 0U);
                    }
                    IsTaskCreated = 0U;
                }
            }
        }
    }

    return RetVal;
}

/**
* Send key event
* @param [in]  KeyCode key event
*/
void SvcPbkPictDisp_KeyEventSend(UINT32 KeyCode)
{
    UINT32 FileAmount = 0U, RetVal = SVC_OK, Err;
    SVC_PBK_CTRL_INFO_s  *pInfo = NULL;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;

    if (SvcPictDispState == SVC_PICT_DISP_NONE_STATE) {
        RetVal = SVC_NG;
        PbkDebugUInt5("SvcPbkPictDisp_KeyEventSend , SVC_PICT_DISP_NONE_STATE __LINE__ %d", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        SvcPbkCtrl_InfoGet(0U, &pInfo);
        if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
            if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, PicDispMgr.DcfScanType, &FileAmount)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "fail to get file amount", 0U, 0U);
                RetVal = SVC_NG;
            }
        }

        if ((RetVal == SVC_NG) || (FileAmount == 0U)) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "FileAmount %d or RetVal %d error", FileAmount, RetVal);
        } else {
            Err = AmbaKAL_EventFlagClear(&SvcPictDispEventId, SVC_PICT_DISP_EVENT_TASK_IDLE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
            }
            if (SVC_OK != AmbaKAL_MsgQueueSend(&(PicDispMgr.MsgQueueId), &KeyCode, AMBA_KAL_NO_WAIT)) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "PicDispMgr queue send failed", 0U, 0U);
            }
        }
    }
    AmbaMisra_TouchUnused(&RetVal);
}

/**
* set picture disp state
* @param [in]  State state
*/
void SvcPbkPictDisp_SetState(UINT32 State)
{
    SvcPictDispState = State;
}

/**
* get picture disp state
* @return state
*/
UINT32 SvcPbkPictDisp_GetState(void)
{
    return SvcPictDispState;
}

/**
* wait task idle
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_WaitTaskIdle(void)
{
    UINT32 ActualFlags = 0, Rval = SVC_OK;

    if (AmbaKAL_EventFlagGet(&SvcPictDispEventId,
                             SVC_PICT_DISP_EVENT_TASK_IDLE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             1000) != SVC_OK) {
        PbkDebugUInt5("SvcPbkPictDisp_WaitTaskIdle failed", 0U, 0U, 0U, 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}

/**
* picture disp handler
* @param [in]  KeyCode key code
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_Handler(UINT32 KeyCode)
{
    UINT32                     FileAmount = 0U, RetVal = SVC_OK, InputVal = 0U, Err;
    AMBA_DCF_FILE_TYPE_e       FileType = AMBA_DCF_FILE_TYPE_ALL;
    SVC_PBK_CTRL_INFO_s        *pInfo = NULL;
    SVC_STG_MONI_DRIVE_INFO_s  DriveInfo;

    PbkDebugUInt5("SvcPbkPictDisp_Handler start", 0U, 0U, 0U, 0U, 0U);
    switch (KeyCode) {
    case SVC_PICT_DISP_KEY_UP:
        Err = SvcThmDisp_ThmViewStart(PicDispMgr.DcfScanType, PicDispMgr.CurFileIndex);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcThmDisp_ThmViewStart failed %u", Err, 0U);
        }
        break;
    case SVC_PICT_DISP_KEY_RIGHT:
        if (PicDispMgr.ZoomRatio == SVC_PICT_DISP_ZOOM_RATIO_1X) {
            SvcPbkCtrl_InfoGet(0U, &pInfo);
            if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
                if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, PicDispMgr.DcfScanType, &FileAmount)) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "fail to get file amount", 0U, 0U);
                    RetVal = SVC_NG;
                }
            } else {
                RetVal = SVC_NG;
            }

            if (RetVal != SVC_NG) {
                PicDispMgr.CurFileIndex = NextFileIndex(PicDispMgr.CurFileIndex, FileAmount);
                Err = PictDispDecodeScreennail(1, &PicDispMgr.CurFileIndex);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcThmDisp_ThmViewStart failed %u", Err, 0U);
                }
                PicDispMgr.ImgRotate = AMBA_DSP_ROTATE_0;

                if (1U == PicDispMgr.FileInfo.Error) {
                    Err = PictDispDisplayErrorPic();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayErrorPic failed %u", Err, 0U);
                    }
                } else {
                    Err = PictDispDisplayScreennail(PicDispMgr.CurFileIndex, PicDispMgr.ImgRotate);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayScreennail failed %u", Err, 0U);
                    }
                }
            }
        }
        break;
    case SVC_PICT_DISP_KEY_LEFT:
        if (PicDispMgr.ZoomRatio == SVC_PICT_DISP_ZOOM_RATIO_1X) {
            SvcPbkCtrl_InfoGet(0U, &pInfo);
            if (SVC_OK == SvcStgMonitor_GetDriveInfo(pInfo->Drive, &DriveInfo)) {
                if (SVC_OK != AmbaDCF_GetFileAmount(DriveInfo.DriveID, 0U, PicDispMgr.DcfScanType, &FileAmount)) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "fail to get file amount", 0U, 0U);
                    RetVal = SVC_NG;
                }
            }

            if (RetVal != SVC_NG) {
                PicDispMgr.CurFileIndex = PrevFileIndex(PicDispMgr.CurFileIndex, FileAmount);
                Err = PictDispDecodeScreennail(1, &PicDispMgr.CurFileIndex);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDecodeScreennail failed %u", Err, 0U);
                }
                PicDispMgr.ImgRotate = AMBA_DSP_ROTATE_0;

                if (1U == PicDispMgr.FileInfo.Error) {
                    Err = PictDispDisplayErrorPic();
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayErrorPic failed %u", Err, 0U);
                    }
                } else {
                    Err = PictDispDisplayScreennail(PicDispMgr.CurFileIndex, PicDispMgr.ImgRotate);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispDisplayScreennail failed %u", Err, 0U);
                    }
                }
            }
        }
        break;
    case SVC_PICT_DISP_KEY_OK:
       if (0U == PicDispMgr.FileInfo.Error) {
            Err = SvcPbkPictDisp_GetFileType(&PicDispMgr.FileInfo.FName[0], &FileType);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_GetFileType failed %u", Err, 0U);
            }
            if (FileType == AMBA_DCF_FILE_TYPE_VIDEO) {
                SvcPbkStillDisp_WaitDspDispIdle();
                PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
            }
        }
        break;
    default:
        // Do nothing
        break;
    }

    PbkDebugUInt5("SvcPbkPictDisp_Handler done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* start single view
* @param [in]  DcfScanType type
* @param [in]  FileIndex file index
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_SingleViewStart(AMBA_DCF_FILE_TYPE_e DcfScanType, UINT32 FileIndex)
{
    UINT32 ActualFlags = 0, RetVal = SVC_OK, Err;

    PbkDebugUInt5("SvcPbkPictDisp_SingleViewStart start", 0U, 0U, 0U, 0U, 0U);
    if (AmbaKAL_EventFlagGet(&SvcPictDispEventId,
                             SVC_PICT_DISP_EVENT_VOUT_IDLE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_AUTO,
                             &ActualFlags,
                             5000U) != SVC_OK) {
        SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcPbkPictDisp_SingleViewStart, timeout", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        Err = PictDispSingleViewStart(DcfScanType, FileIndex);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "PictDispSingleViewStart failed %u", Err, 0U);
        }
        Err = AmbaKAL_EventFlagSet(&SvcPictDispEventId, SVC_PICT_DISP_EVENT_VOUT_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
        }
    }

    PbkDebugUInt5("SvcPbkPictDisp_SingleViewStart done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Set current file index
* @param [in]  Index index
*/
void SvcPbkPictDisp_SetCurFileIndex(UINT32 Index)
{
    PicDispMgr.CurFileIndex = Index;
}

/**
* Get current file index
* @param [out]  pIndex index
*/
void SvcPbkPictDisp_GetCurFileIndex(UINT32 *pIndex)
{
    *pIndex = PicDispMgr.CurFileIndex;
}

static char *SvcPbkPictDisp_StrChr(const char *str, char c)
{
    char       *ret_str = NULL;
    const char *check_str = str;
    UINT32     is_return = 0U;

    AmbaMisra_TouchUnused(&is_return);

    while (is_return == 0U) {
        if (*check_str == c) {
            AmbaMisra_TypeCast(&ret_str, &check_str);
            is_return = 1U;
        }

        if (*check_str == '\0') {
            ret_str = NULL;
            is_return = 1U;
        }

        if(is_return == 1U){
            break;
        }

        ++check_str;
    }
    return ret_str;
}

/**
* GetFileType
* @param [in]  FileName
* @param [in]  FileType
* @return ErrorCode
*/
UINT32 SvcPbkPictDisp_GetFileType(const char *FileName, AMBA_DCF_FILE_TYPE_e *FileType)
{
    const char        *Ext;
    UINT32            rval = SVC_OK;
    static const char FileExt[3][5] = {".JPG",".MP4",".AAC"};

    Ext = SvcPbkPictDisp_StrChr(FileName, '.');
    if (Ext == NULL) {
        rval = SVC_NG;
    }

    if(rval != SVC_NG) {
        if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_IMAGE], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_IMAGE;
        } else if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_VIDEO], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_VIDEO;
        } else if (AmbaUtility_StringCompare(Ext, FileExt[AMBA_DCF_FILE_TYPE_AUDIO], 3) == OK_INT32) {
            *FileType = AMBA_DCF_FILE_TYPE_AUDIO;
        } else {
            rval = SVC_NG;
        }
    }

    return rval;
}

/**
* CalcBaseDispSize
* @param [in]   ActiveWidth
* @param [in]   ActiveHeight
* @param [in]   ActiveWinAR
* @param [in]   InputImgAR
* @param [out]  pResultDispW1X
* @param [out]  pResultDispH1X
*/
void SvcPbkPictDisp_CalcBaseDispSize(UINT32 ActiveWidth, UINT32 ActiveHeight, UINT32 ActiveWinAR,
                                 UINT32 InputImgAR, UINT32 *pResultDispW1X, UINT32 *pResultDispH1X)
{
    UINT32 DispW1X, DispH1X;

    if (InputImgAR < ActiveWinAR) {
        DispW1X = (ActiveWidth * InputImgAR) / ActiveWinAR;
        DispH1X = ActiveHeight;
    } else if (InputImgAR > ActiveWinAR) {
        DispW1X = ActiveWidth;
        DispH1X = (ActiveHeight * ActiveWinAR) / InputImgAR;
    } else {
        DispW1X = ActiveWidth;
        DispH1X = ActiveHeight;
    }
    *pResultDispW1X = (DispW1X + 1U) & 0xFFFFFFE0U;
    *pResultDispH1X = (DispH1X + 1U) & 0xFFFFFFFEU;
}

/**
* GetPictDispMem
* @param [out]  ppPictDispMem PictDispMem
*/
void SvcPbkPictDisp_GetPictDispMem(SVC_PICT_DISP_MEM_s **ppPictDispMem)
{
    *ppPictDispMem = &SvcPictDispMem;
}

/**
* Get pict disp info
* @param [out]  ppInfo pict disp info
*/
void SvcPbkPictDisp_GetInfo(const SVC_PICT_DISP_INFO_s** ppInfo)
{
    *ppInfo = &PictDispInfo;
}


/**
 *  SvcPbkPictDisp_BlockCopy - Schedule a block copy transaction
 *  @param[in] pBlockBlit The control block of the block copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 SvcPbkPictDisp_BlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32             Err, Rval = SVC_OK;
    AMBA_GDMA_BLOCK_s  Block;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == SVC_OK) {
        Err = AmbaWrap_memcpy(&Block, pBlockBlit, sizeof(AMBA_GDMA_BLOCK_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaWrap_memcpy failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

#if !defined (CONFIG_THREADX)
    /* for qnx and linux, we need to transfer the virtual address to physical address */
    if (Rval == SVC_OK) {
        ULONG Vir, Phy;

        AmbaMisra_TypeCast(&Vir, &(Block.pSrcImg));
        Err =  SvcBuffer_Vir2Phys(Vir, &Phy);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcBuffer_Vir2Phys failed %u", Err, 0U);
            Rval = SVC_NG;
        }
        AmbaMisra_TypeCast(&(Block.pSrcImg), &Phy);

        AmbaMisra_TypeCast(&Vir, &(Block.pDstImg));
        Err =  SvcBuffer_Vir2Phys(Vir, &Phy);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "SvcBuffer_Vir2Phys failed %u", Err, 0U);
            Rval = SVC_NG;
        }
        AmbaMisra_TypeCast(&(Block.pDstImg), &Phy);
    }
#endif

    if (Rval == SVC_OK) {
        Err = AmbaGDMA_BlockCopy(&Block, NotifierFunc, NotifierFuncArg, TimeOut);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PBK_PICDISP, "AmbaGDMA_BlockCopy failed %u", Err, 0U);
            Rval = SVC_NG;
        }
    }

    return Rval;
}
