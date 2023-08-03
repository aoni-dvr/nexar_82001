/**
 *  @file SvcPbkStillDec.c
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
 *  @details still dec related APIs
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaSensor.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaCache.h"
#include "AmbaUtility.h"
#include "AmbaGDMA.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcPbkPictDisp.h"
#include "SvcDisplay.h"
#include "SvcIsoCfg.h"
#include "SvcIK.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcPbkCtrl.h"
#include "SvcPbkStillDec.h"
#include "SvcPlat.h"
#include "SvcStill.h"
#include "SvcStillProc.h"

#define SVC_LOG_STL_DEC    "STLDEC"

static SDEC_MGR_s          G_SDecMgr;
static SVC_SDEC_CTRL_s     SvcSDecCtrl;
SVC_SDEC_CONFIG_s          SvcSDecConfig;

static void PbkDebugUInt5(const char *FmtStr, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    if (pDispInfo->ShowDbgLog > 0U) {
        SvcWrap_PrintUL(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 StillYuvDataReady(const void *pEventData)
{
    UINT32                          ActualFlags = 0U, Err;
    const AMBA_DSP_YUV_IMG_BUF_s    *pInfo;

    Err = AmbaKAL_EventFlagGet(&(G_SDecMgr.EventId),
                               SDEC_FLAG_DECODE_IDLE,
                               AMBA_KAL_FLAGS_ANY,
                               AMBA_KAL_FLAGS_CLEAR_NONE,
                               &ActualFlags,
                               AMBA_KAL_NO_WAIT);
    if (Err != KAL_ERR_NONE) {
        /* nothing */
    }

    if((ActualFlags & SDEC_FLAG_DECODE_IDLE) == 0U) {        /* Not Idle */
        AmbaMisra_TypeCast(&pInfo, &pEventData);

        PbkDebugUInt5("[YuvDataReady] Y[0x%llX] UV[0x%llX]", pInfo->BaseAddrY, pInfo->BaseAddrUV, 0, 0, 0);
        PbkDebugUInt5("               Win[%dx%d] P[%d] Fmt[%d]", pInfo->Window.Width, pInfo->Window.Height, pInfo->Pitch, pInfo->DataFmt, 0U);


        G_SDecMgr.DecReport.YAddr     = pInfo->BaseAddrY;
        G_SDecMgr.DecReport.UVAddr    = pInfo->BaseAddrUV;
        G_SDecMgr.DecReport.YuvPitch  = pInfo->Pitch;
        G_SDecMgr.DecReport.YuvWidth  = pInfo->Window.Width;
        G_SDecMgr.DecReport.YuvHeight = pInfo->Window.Height;
        G_SDecMgr.DecReport.YuvFormat = pInfo->DataFmt;
        G_SDecMgr.DecStatus[0]        = 0U;

        Err = AmbaKAL_EventFlagSet(&G_SDecMgr.EventId, SDEC_FLAG_DECODE_DONE);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet SDEC_FLAG_DECODE_DONE failed %u", Err, 0U);
        }

        PbkDebugUInt5("StillDecDspEventHandler Dec done event", 0U, 0U, 0U, 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 StillDecDspEventHandler(const void *pEventData)
{
    UINT32 ActualFlags = 0, Err;
    const AMBA_DSP_STLDEC_STATUS_s *pPicInfo = NULL;

    AmbaMisra_TypeCast(&pPicInfo, &pEventData);
    {
        PbkDebugUInt5("[StlDecRdy][%d] Status[%d] Op[%d] Y[0x%llX] UV[0x%llX]",
                                pPicInfo->Index, pPicInfo->Status,
                                pPicInfo->Operation,
                                pPicInfo->YAddr, pPicInfo->UVAddr);
        PbkDebugUInt5("               Win[%dx%d] P[%d] Fmt[%d]",
                                pPicInfo->Width, pPicInfo->Height, pPicInfo->Pitch, pPicInfo->DataFmt, 0U);
    }

    Err = AmbaKAL_EventFlagGet(&(G_SDecMgr.EventId),
                             SDEC_FLAG_DECODE_IDLE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             AMBA_KAL_NO_WAIT);
    if (Err != KAL_ERR_NONE) {
        /* nothing */
    }

    if((ActualFlags & SDEC_FLAG_DECODE_IDLE) == 0U) {        /* Not Idle */
        G_SDecMgr.DecReport.YAddr     = pPicInfo->YAddr;
        G_SDecMgr.DecReport.UVAddr    = pPicInfo->UVAddr;
        G_SDecMgr.DecReport.YuvPitch  = pPicInfo->Pitch;
        G_SDecMgr.DecReport.YuvWidth  = pPicInfo->Width;
        G_SDecMgr.DecReport.YuvHeight = pPicInfo->Height;
        G_SDecMgr.DecReport.YuvFormat = pPicInfo->DataFmt;
        G_SDecMgr.DecStatus[0]        = pPicInfo->Status;

        Err = AmbaKAL_EventFlagSet(&G_SDecMgr.EventId, SDEC_FLAG_DECODE_DONE);
        if (Err != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet SDEC_FLAG_DECODE_DONE failed %u", Err, 0U);
        }
        PbkDebugUInt5("StillDecDspEventHandler Dec done event", 0U, 0U, 0U, 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 PrepareIsoCfg(UINT32 WidthIn, UINT32 HeightIn, UINT32 WidthOut, UINT32 HeightOut, AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg)
{
#ifdef CONFIG_SOC_H22
    AmbaMisra_TouchUnused(&pIsoCfg);
    AmbaMisra_TouchUnused(&WidthIn);
    AmbaMisra_TouchUnused(&HeightIn);
    AmbaMisra_TouchUnused(&WidthOut);
    AmbaMisra_TouchUnused(&HeightOut);
    return SVC_OK;
#else
    AMBA_IK_MODE_CFG_s           ImgMode = {0};
    AMBA_IK_ABILITY_s            ImgAbility = {0};
    AMBA_IK_EXECUTE_CONTAINER_s  CfgExecInfo = {0};
    UINT32                       Rval = SVC_OK, Err;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    AmbaMisra_TouchUnused(&Rval);

    ImgMode.ContextId    = pDispInfo->Y2YCtxId;

    ImgAbility.Pipe      = (UINT8)AMBA_IK_PIPE_VIDEO;
    ImgAbility.VideoPipe = (UINT32)AMBA_IK_VIDEO_Y2Y;

    if (Rval == SVC_OK) {
        Err = AmbaIK_InitContext(&ImgMode, &ImgAbility);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaIK_InitContext failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        SvcIsoCfg_Fixed(&ImgMode, 1);
    }

    if (Rval == SVC_OK) {
        AMBA_IK_WINDOW_SIZE_INFO_s VinSizeInfo;

        Err = AmbaWrap_memset(&VinSizeInfo, 0, sizeof(VinSizeInfo));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memset failed %u", Err, 0U);
        }

        VinSizeInfo.VinSensor.StartX               = 0U;
        VinSizeInfo.VinSensor.StartY               = 0U;
        VinSizeInfo.VinSensor.HSubSample.FactorDen = (UINT32)1U;
        VinSizeInfo.VinSensor.HSubSample.FactorNum = (UINT32)1U;
        VinSizeInfo.VinSensor.VSubSample.FactorDen = (UINT32)1U;
        VinSizeInfo.VinSensor.VSubSample.FactorNum = (UINT32)1U;
        VinSizeInfo.VinSensor.Width                = WidthIn;
        VinSizeInfo.VinSensor.Height               = HeightIn;
        VinSizeInfo.Main.Width                     = WidthIn;
        VinSizeInfo.Main.Height                    = HeightIn;

        Err = AmbaIK_SetWindowSizeInfo(&ImgMode, &VinSizeInfo);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaIK_SetWindowSizeInfo failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
        Err = AmbaIK_SetWarpEnb(&ImgMode, 0);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaIK_SetWarpEnb failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        }
    }

#if (defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV2FS)) && defined(CONFIG_ICAM_STLCAP_USED)
    if (Rval == SVC_OK) {
        UINT32 SliceType = (UINT32)3U << 12U;

        Err = SvcStillProc_SetIkTile(&ImgMode, (UINT16) SliceType
                                             , (UINT16) WidthIn
                                             , (UINT16) HeightIn
                                             , (UINT16) WidthOut
                                             , (UINT16) HeightOut);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcStillProc_SetIkTile failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        }
    }
#else
    AmbaMisra_TouchUnused(&WidthOut);
    AmbaMisra_TouchUnused(&HeightOut);
#endif

#if defined(CONFIG_SOC_CV2FS)
    if (Rval == SVC_OK) {
        /* Dec2Vproc shall be 420 all the time */
        UINT32 YuvMode = AMBA_IK_YUV420;

        Err = AmbaIK_SetYuvMode(&ImgMode, YuvMode);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaIK_SetYuvMode failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        }
    }
#endif

    if (Rval == SVC_OK) {
        Err = AmbaIK_ExecuteConfig(&ImgMode, &CfgExecInfo);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaIK_ExecuteConfig failed! return 0x%x", Err, 0U);
            Rval = SVC_NG;
        } else {
            pIsoCfg->CtxIndex = ImgMode.ContextId;
            pIsoCfg->CfgIndex = CfgExecInfo.IkId;
            AmbaMisra_TypeCast(&pIsoCfg->CfgAddress, &CfgExecInfo.pIsoCfg);
        }
    }

    return Rval;
#endif
}

static void StillDecSendYuv2YuvCmd(const SVC_SDEC_DEC_SETUP_s *pDecConfig, const SVC_SDEC_DEC_REPORT_s *pDecResult)
{
    UINT32                       Err;
    AMBA_DSP_YUV_IMG_BUF_s       SrcYuv;
    AMBA_DSP_YUV_IMG_BUF_s       DstYuv;
    AMBA_DSP_STLDEC_YUV2YUV_s    YuvOp;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    SrcYuv.DataFmt        = (UINT8)pDecResult->YuvFormat;
    SrcYuv.Pitch          = (UINT16)pDecResult->YuvPitch;
    SrcYuv.BaseAddrY      = pDecResult->YAddr;
    SrcYuv.BaseAddrUV     = pDecResult->UVAddr;
    SrcYuv.Window.OffsetX = 0;
    SrcYuv.Window.OffsetY = 0;
    SrcYuv.Window.Width   = (UINT16)pDecResult->YuvWidth;
    SrcYuv.Window.Height  = (UINT16)pDecResult->YuvHeight;

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        /* align for ik window */
        SrcYuv.Window.Width   = (SrcYuv.Window.Width  / ((UINT16)AMBA_CACHE_LINE_SIZE)) * ((UINT16)AMBA_CACHE_LINE_SIZE);
        SrcYuv.Window.Height  = (SrcYuv.Window.Height / 8U) * 8U;
    }

    DstYuv.DataFmt        = SVCAG_SDEC_DISP_FORMAT;
    DstYuv.Pitch          = pDecConfig->SubBufPitch;
    DstYuv.BaseAddrY      = pDecConfig->SubBufYBase;
    DstYuv.BaseAddrUV     = pDecConfig->SubBufUVBase;
    DstYuv.Window.OffsetX = pDecConfig->SubYuvOffsetX;
    DstYuv.Window.OffsetY = pDecConfig->SubYuvOffsetY;
    DstYuv.Window.Width   = pDecConfig->SubYuvWidth;
    DstYuv.Window.Height  = pDecConfig->SubYuvHeight;

    YuvOp.RotateFlip      = AMBA_DSP_ROTATE_0;
    YuvOp.LumaGain        = 128U;
    SvcPbkStillDec_CheckYuv2YuvParam(1U, &SrcYuv, &DstYuv, &YuvOp);

    PbkDebugUInt5("SrcYuv.DataFmt %d, Pitch %d, BaseAddrY %llx, BaseAddrUV %llx, Width %d", \
            SrcYuv.DataFmt, SrcYuv.Pitch,SrcYuv.BaseAddrY, SrcYuv.BaseAddrUV, SrcYuv.Window.Width);
    PbkDebugUInt5("DstYuv Pitch %d, BaseAddrY %llx, BaseAddrUV %llx, Width %d, Height %d", \
            DstYuv.Pitch, DstYuv.BaseAddrY, DstYuv.BaseAddrUV, DstYuv.Window.Width, DstYuv.Window.Height);
    PbkDebugUInt5("OffsetX %d, OffsetY %d", DstYuv.Window.OffsetX, DstYuv.Window.OffsetY, 0U, 0U, 0U);

    Err = SvcPbkStillDec_StartY2Y(&SrcYuv, &DstYuv, &YuvOp);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_StartY2Y failed %u", Err, 0U);
    }
}

static UINT32 StillDecLoadJpegBitstream(const SVC_SDEC_DEC_SETUP_s *pDecConfig, SDEC_RUNTIME_PARAM_s *pParam)
{
    #define FFD8_MARKER_SIZE    2U
    AMBA_FS_FILE  *pFile;
    ULONG         Wp = pParam->BitsBufWp;
    UINT32        HeaderSize = FFD8_MARKER_SIZE;
    UINT32        Rval = SVC_OK, NumSuccess = 0U, Err;
    UINT8         *pU8;

    /* 1. Open file */
    if (AMBA_FS_ERR_NONE != AmbaFS_FileOpen(pDecConfig->FName, "r", &pFile)) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaFS_FileOpen failed", 0U, 0U);
        Rval = SVC_NG;
    } else {
        /* 2. check if bitstream is over bitstream buffer */
        if((((Wp + pDecConfig->Info.Jpeg.BitsSize) + FFD8_MARKER_SIZE) - 1U) > pParam->BitsBufLimit) {
            Rval = SVC_NG;
        } else {
            /* 3. load bitstream from file to buffer */
            AmbaMisra_TypeCast(&pU8, &Wp);
            pU8[0] = 0xffU;
            pU8[1] = 0xd8U;
            Wp += HeaderSize;

            Err = AmbaFS_FileSeek(pFile, (INT64)pDecConfig->Info.Jpeg.BitsOffset, AMBA_FS_SEEK_START);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaFS_FileSeek failed %u", Err, 0U);
            }

            AmbaMisra_TypeCast(&pU8, &Wp);
            if (AMBA_FS_ERR_NONE != AmbaFS_FileRead(pU8, 1U, pDecConfig->Info.Jpeg.BitsSize, pFile, &NumSuccess)) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaFS_FileRead failed", 0U, 0U);
                Rval = SVC_NG;
            } else {
                pParam->pBitsDataBase = pParam->BitsBufWp;
                pParam->BitsDataSize = pDecConfig->Info.Jpeg.BitsSize + HeaderSize;
                pParam->BitsBufWp = pParam->pBitsDataBase + pParam->BitsDataSize;
            }
        }
        Err = AmbaFS_FileClose(pFile);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaFS_FileClose failed %u", Err, 0U);
        }
    }

    return Rval;
}

/**
* Create still decode task
* @return ErrorCode
*/
UINT32 SvcPbkStillDec_TaskCreate(void)
{
    static char                  StillDecEvtName[] = "StillDecFlgMgr";
    UINT32                       RetVal = SVC_OK, Err;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    PbkDebugUInt5("SvcPbkStillDec_TaskCreate start", 0U, 0U, 0U, 0U, 0U);
    Err = AmbaWrap_memset(&G_SDecMgr, 0, sizeof(SDEC_MGR_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memset failed %u", Err, 0U);
    }

    if (AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STILL_DEC_STATUS,
                                        &StillDecDspEventHandler) != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskCreate: failed to reg event handler", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        if (AmbaKAL_EventFlagCreate(&(G_SDecMgr.EventId), StillDecEvtName) != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskCreate: failed to EventFlagCreate", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            Err = AmbaKAL_EventFlagClear(&(G_SDecMgr.EventId), 0xFFFFFFFFU);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
            }

            Err = AmbaKAL_EventFlagSet(&(G_SDecMgr.EventId), SDEC_FLAG_DECODE_IDLE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
            }
            G_SDecMgr.DecReport.pStatus = &(G_SDecMgr.DecStatus[0]);
        }
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        if (AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, &StillYuvDataReady) != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskCreate: failed to reg event handler", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    PbkDebugUInt5("SvcPbkStillDec_TaskCreate done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Still decode task delete
* @return ErrorCode
*/
UINT32 SvcPbkStillDec_TaskDelete(void)
{
    UINT32 ActualFlags = 0U, RetVal = SVC_OK;
    const SVC_PICT_DISP_INFO_s*  pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    PbkDebugUInt5("SvcPbkStillDec_TaskDelete start", 0U, 0U, 0U, 0U, 0U);

    if (AmbaKAL_EventFlagGet(&(G_SDecMgr.EventId),
                             SDEC_FLAG_DECODE_IDLE | SDEC_FLAG_DECODE_DONE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             5000) != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskDelete: Task is not idle", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        if (SVC_OK != AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STILL_DEC_STATUS, &StillDecDspEventHandler)) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskDelete: failed to unregister event handler", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            if (SVC_OK != AmbaKAL_EventFlagDelete(&(G_SDecMgr.EventId))) {
                SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskDelete: failed to EventFlagDelete", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        if (AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STL_YUV_DATA_RDY, &StillYuvDataReady) != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_TaskCreate: failed to unregister event handler", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    PbkDebugUInt5("SvcPbkStillDec_TaskDelete Done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Still decode setup
* @param [out]  pSetup setup struct
*/
void SvcPbkStillDec_Setup(const SVC_SDEC_CTRL_s *pSetup)
{
    UINT32 Err;

    Err = AmbaWrap_memcpy(&SvcSDecCtrl, pSetup, sizeof(SVC_SDEC_CTRL_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memcpy failed %u", Err, 0U);
    }
}

/**
* Start still decode
* @return ErrorCode
*/
UINT32 SvcPbkStillDec_StartDecode(void)
{
    UINT32                          i, j, Rval = SVC_OK, PlayerId = 0U;
    UINT32                          ImgNum = SvcSDecConfig.ImgNum;
    UINT32                          ActualFlags = 0U, Err;
    AMBA_DSP_STLDEC_START_CONFIG_s  DecConfig;
    const SVC_SDEC_DEC_SETUP_s      *pConfig;
    SVC_PBK_CTRL_DEC_FRAME_s        Input;
    SVC_PBK_CTRL_INFO_s             *pCtrlInfo;
    char                            FileName[64U];

    PbkDebugUInt5("SvcPbkStillDec_StartDecode start", 0U, 0U, 0U, 0U, 0U);
    if(NULL != SvcSDecCtrl.pFbEventId) {
        Err = AmbaKAL_EventFlagClear(SvcSDecCtrl.pFbEventId, SDEC_EVENT_DECODE_DONE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
        }
    }

    Err = AmbaKAL_EventFlagClear(&G_SDecMgr.EventId, SDEC_FLAG_DECODE_IDLE | SDEC_FLAG_DECODE_DONE);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
    }

    /* start decode */
    G_SDecMgr.Param.BitsBufWp     = SvcSDecCtrl.BitsBufBase;
    G_SDecMgr.Param.BitsBufLimit  = (SvcSDecCtrl.BitsBufBase + SvcSDecCtrl.BitsBufSize) - 1U;
    G_SDecMgr.Param.BitsDataSize  = 0U;
    G_SDecMgr.Param.pBitsDataBase = 0U;
    G_SDecMgr.DecReport.StatusNum = SvcSDecConfig.ImgNum;
    PbkDebugUInt5("StillDec_StartDecode, BitsBufWp 0x%llx", G_SDecMgr.Param.BitsBufWp, 0U, 0U, 0U, 0U);

    for (i = 0; i < ImgNum; i++) {
        pConfig = &(SvcSDecConfig.Config[i]);
        if (Rval == SVC_NG) {
            break;
        }

        if (1U == G_SDecMgr.Param.Abort) {
            for(j = i; j < ImgNum; j++) {
                G_SDecMgr.DecStatus[j] = SDEC_MARKER_DECODE_ABORT;
            }

            PbkDebugUInt5("Decode abort[%d]", i, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
            continue;
        }

        if (((pConfig->Type == SDEC_IMG_TYPE_HEVC_IFRAME) ||
            (pConfig->Type == SDEC_IMG_TYPE_H264_IFRAME)) ||
            (pConfig->Type == SDEC_IMG_TYPE_MJPEG)) {
            SvcWrap_strcpy(FileName, 64, pConfig->FName);
            Input.FileName  = FileName;
            Rval = SvcPbkCtrl_DecOneFrame(PlayerId, &Input);
            if (AmbaKAL_EventFlagGet(&G_SDecMgr.EventId,
                                    SDEC_FLAG_DECODE_DONE,
                                    AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlags,
                                    5000U) != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "StillDec_StartDecode: timeout", 0U, 0U);
                Rval = SVC_NG;
                continue;
            }
        } else {
            Rval = StillDecLoadJpegBitstream(pConfig, &G_SDecMgr.Param);
        }

        if (1U == G_SDecMgr.Param.Abort) {
            for(j = i; j < ImgNum; j++) {
                G_SDecMgr.DecStatus[j] = SDEC_MARKER_DECODE_ABORT;
            }

            PbkDebugUInt5("Decode abort[%d]", i, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
            continue;
        }

        if (Rval == SVC_OK) {
            if ((pConfig->Type != SDEC_IMG_TYPE_HEVC_IFRAME) &&
                (pConfig->Type != SDEC_IMG_TYPE_H264_IFRAME) &&
                (pConfig->Type != SDEC_IMG_TYPE_MJPEG)) {

                DecConfig.BitsFormat = 0;
                DecConfig.BitsSize   = G_SDecMgr.Param.BitsDataSize;
                DecConfig.BitsAddr   = G_SDecMgr.Param.pBitsDataBase;
                DecConfig.YuvBufSize = SvcSDecCtrl.MainBufSize;
                DecConfig.YuvBufAddr = SvcSDecCtrl.MainBufBase;

                #if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
                DecConfig.ImageWidth       = pConfig->Info.Jpeg.Width;
                DecConfig.ComplianceOption = 1U;
                #endif

                {
                    ULONG  AlignStart, AlignSize;

                    AlignStart = DecConfig.BitsAddr & AMBA_CACHE_LINE_MASK;
                    AlignSize  = (DecConfig.BitsAddr + DecConfig.BitsSize) - AlignStart;
                    AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                    AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                    Err = SvcPlat_CacheClean(AlignStart, AlignSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheClean failed %u", Err, 0U);
                    }
                }

                Err = AmbaKAL_EventFlagSet(&G_SDecMgr.EventId, SDEC_FLAG_DECODING);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
                }

                Rval = AmbaDSP_StillDecStart(0U, &DecConfig);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "AmbaDSP_StillDecStart failed %u", Rval, 0U);
                }

                Err = AmbaKAL_EventFlagClear(&G_SDecMgr.EventId, SDEC_FLAG_DECODING);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
                }

                if (AmbaKAL_EventFlagGet(&G_SDecMgr.EventId,
                                        SDEC_FLAG_DECODE_DONE,
                                        AMBA_KAL_FLAGS_ANY,
                                        AMBA_KAL_FLAGS_CLEAR_NONE,
                                        &ActualFlags,
                                        5000U) != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "StillDec_StartDecode: timeout", 0U, 0U);
                    Rval = SVC_NG;
                    continue;
                }
            }
        }

        if (Rval == SVC_OK) {
            if ((pConfig->Type == SDEC_IMG_TYPE_H264_IFRAME) || (pConfig->Type == SDEC_IMG_TYPE_HEVC_IFRAME)) {
                SvcPbkCtrl_InfoGet(PlayerId, &pCtrlInfo);
                G_SDecMgr.DecReport.YuvHeight = pCtrlInfo->pPlayer->ExpMov.VideoTrack[0].Height;
            }

            if ((((G_SDecMgr.DecReport.YuvPitch * G_SDecMgr.DecReport.YuvHeight) * 3U) / 2U) > SvcSDecCtrl.MainBufSize) {
                G_SDecMgr.DecStatus[i] = SDEC_MARKER_MAIN_BUF_ERROR;
                SvcLog_NG(SVC_LOG_STL_DEC, "SStillDec_StartDecode: Main buf not enough[%d]", i, 0U);
                Rval = SVC_NG;
                continue;
            }

            /* rescaling */
            if (1U == pConfig->SubEnable) {
                StillDecSendYuv2YuvCmd(pConfig, &G_SDecMgr.DecReport);
            }
        } else {
            G_SDecMgr.DecStatus[i] = SDEC_MARKER_LOAD_BITS_ERROR;
            SvcLog_NG(SVC_LOG_STL_DEC, "StillDec_StartDecode: LoadBitstream error[%d]", 0U, 0U);
        }
    }

    Err = AmbaKAL_EventFlagSet(&(G_SDecMgr.EventId), SDEC_FLAG_DECODE_IDLE);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet SDEC_FLAG_DECODE_IDLE failed %u", 0U, 0U);
    }
    /* Feedback to user */
    if(NULL != SvcSDecCtrl.pFbEventId) {
        Err = AmbaKAL_EventFlagSet(SvcSDecCtrl.pFbEventId, SDEC_EVENT_DECODE_DONE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagSet SDEC_EVENT_DECODE_DONE failed %u", 0U, 0U);
        }
    }

    return Rval;
}

/**
* Stop still decode
* @return ErrorCode
*/
UINT32 SvcPbkStillDec_StopDecode(void)
{
    UINT32 ActualFlags = 0U, RetVal = SVC_OK;

    PbkDebugUInt5("SvcPbkStillDec_StopDecode start", 0U, 0U, 0U, 0U, 0U);
    G_SDecMgr.Param.Abort = 1U;

    if (AmbaKAL_EventFlagGet(&G_SDecMgr.EventId,
                             SDEC_FLAG_DECODING,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             AMBA_KAL_NO_WAIT) != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_StopDecode, time out", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        if (0U != (ActualFlags & SDEC_FLAG_DECODING)) {
            PbkDebugUInt5("SDEC_FLAG_DECODING", 0U, 0U, 0U, 0U, 0U);
        }

        if (AmbaKAL_EventFlagGet(&G_SDecMgr.EventId,
                                 SDEC_FLAG_DECODE_IDLE,
                                 AMBA_KAL_FLAGS_ANY,
                                 AMBA_KAL_FLAGS_CLEAR_NONE,
                                 &ActualFlags,
                                 5000U) != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_StopDecode: failed", 0U, 0U);
            RetVal = SVC_NG;
        }

        G_SDecMgr.Param.Abort = 0U;
    }

    return RetVal;
}

/**
* QueryDecResult
* @param [out]  pReport Report
*/
void SvcPbkStillDec_QueryDecResult(SVC_SDEC_DEC_REPORT_s **pReport)
{
    *pReport = &(G_SDecMgr.DecReport);
}

/**
* CheckYuv2YuvParam
* @param [in]  NumImg
* @param [in]  pSrcYuvBufAddr SrcYuvBufAddr
* @param [in]  pDestYuvBufAddr DestYuvBufAddr
* @param [in]  pOperation Operation
* @return ErrorCode
*/
void SvcPbkStillDec_CheckYuv2YuvParam(UINT32 NumImg,
                        const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                        AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                        const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    UINT32 i;
    const AMBA_DSP_YUV_IMG_BUF_s *pSrc;
    AMBA_DSP_YUV_IMG_BUF_s *pDst;
    UINT32 IdspWidth = 0U, YuvWidth = 0U, YuvWidthAlign = 0U;

    AmbaMisra_TouchUnused(&pOperation);

    for (i = 0U; i < NumImg; i++) {
        pSrc = &(pSrcYuvBufAddr[i]);
        pDst = &(pDestYuvBufAddr[i]);
        IdspWidth = ((UINT32)pSrc->Window.Width > (UINT32)pDst->Window.Width) ? (UINT32)pSrc->Window.Width : (UINT32)pDst->Window.Width;
        YuvWidth  = (UINT32)pDst->Window.Width;

        /* Align pDst->Window.Width */
        if (IdspWidth <= 1920U) {
            YuvWidthAlign = (YuvWidth & 0xfffffffeU);
        } else if (IdspWidth <= 3840U) {
            YuvWidthAlign = (YuvWidth & 0xfffffffcU);
        } else if (IdspWidth <= 7680U) {
            YuvWidthAlign = (YuvWidth & 0xfffffff8U);
        } else {
            YuvWidthAlign = (YuvWidth & 0xfffffff0U);
        }

        if (YuvWidth != YuvWidthAlign) {
            pDst->Window.Width = (UINT16)YuvWidthAlign;
            PbkDebugUInt5("Align Y2Y dst width %d to %d", YuvWidth, YuvWidthAlign, 0U, 0U, 0U);
        }
    }
}

/**
* StartY2Y
* @param [in]  pSrcYuvBufAddr SrcYuvBufAddr
* @param [in]  pDestYuvBufAddr DestYuvBufAddr
* @param [in]  pOperation Operation
* @return ErrorCode
*/
UINT32 SvcPbkStillDec_StartY2Y(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    UINT32                       ActualFlags = 0U, RetVal = SVC_OK, Err;
    AMBA_DSP_ISOCFG_CTRL_s       IsoCfg;
    SVC_PICT_DISP_MEM_s          *pMem;
    const SVC_PICT_DISP_INFO_s   *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    Err = AmbaKAL_EventFlagClear(&G_SDecMgr.EventId, SDEC_FLAG_DECODE_IDLE | SDEC_FLAG_DECODE_DONE);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DEC, "AmbaKAL_EventFlagClear SDEC_FLAG_DECODE_IDLE | SDEC_FLAG_DECODE_DONE failed! return 0x%x", Err, 0U);
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        {
            ULONG  AlignStart;
            ULONG  AlignSize;

            AlignStart = (ULONG)pDestYuvBufAddr->BaseAddrY & AMBA_CACHE_LINE_MASK;
            AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrY + (((ULONG)pDestYuvBufAddr->Pitch * (ULONG)pDestYuvBufAddr->Window.Height) / 2UL)) - AlignStart;
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
            }

            AlignStart = (ULONG)pDestYuvBufAddr->BaseAddrUV & AMBA_CACHE_LINE_MASK;
            if (pDestYuvBufAddr->DataFmt == AMBA_DSP_YUV420) {
                AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrUV + (((ULONG)pDestYuvBufAddr->Pitch * (ULONG)pDestYuvBufAddr->Window.Height) / 2UL)) - AlignStart;
            } else {
                AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrUV + ((ULONG)pDestYuvBufAddr->Pitch * (ULONG)pDestYuvBufAddr->Window.Height)) - AlignStart;
            }
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
            }
        }
        Err = AmbaDSP_StillDecYuv2Yuv(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaDSP_StillDecYuv2Yuv failed! return 0x%x", Err, 0U);
            RetVal = SVC_NG;
        }
    } else {
        AMBA_DSP_YUV_IMG_BUF_s TempSrcBuf, TempDstBuf;

        Err = AmbaWrap_memset(&IsoCfg, 0, sizeof(IsoCfg));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memset failed %u", Err, 0U);
        }

        Err = PrepareIsoCfg(pSrcYuvBufAddr->Window.Width,  pSrcYuvBufAddr->Window.Height,
                            pDestYuvBufAddr->Window.Width, pDestYuvBufAddr->Window.Height, &IsoCfg);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "PrepareIsoCfg failed! return 0x%x", Err, 0U);
            RetVal = SVC_NG;
        }

        SvcPbkPictDisp_GetPictDispMem(&pMem);

        /* still y2y source pitch must be 64 aligned, so we need to copy it to another buffer first */
        {
            ULONG NewPitch = (((ULONG)(pSrcYuvBufAddr->Pitch) + (CACHE_LINE_SIZE - 1U)) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE;

            Err = AmbaWrap_memcpy(&TempSrcBuf, pSrcYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memcpy failed %u", Err, 0U);
            }

            {
                UINT32 NeedSize;

                if (TempSrcBuf.DataFmt == AMBA_DSP_YUV422) {
                    NeedSize    = (UINT32)NewPitch * (UINT32)TempSrcBuf.Window.Height * 2U;
                } else {
                    NeedSize    = ((UINT32)NewPitch * (UINT32)TempSrcBuf.Window.Height * 3U) / 2U;
                }

                if (pMem->Y2YSrcBuf.BufSize < NeedSize) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_STL_DEC, "Y2YSrcBuf is not enough %u/%u", pMem->Y2YSrcBuf.BufSize, NeedSize);
                }
            }

            if ((RetVal == SVC_OK) && ((TempSrcBuf.Pitch % (UINT16)CACHE_LINE_SIZE) != 0U)) {
                AMBA_GDMA_BLOCK_s   BCopy;
                ULONG               TmpAddr;

                {
                    ULONG  AlignStart;
                    ULONG  AlignSize;

                    AlignStart = (ULONG)pMem->Y2YSrcBuf.BufBase & AMBA_CACHE_LINE_MASK;
                    AlignSize  = (ULONG)pMem->Y2YSrcBuf.BufSize;
                    Err = SvcPlat_CacheInvalidate(AlignStart, AlignSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheInvalidate failed %u", Err, 0U);
                    }
                }

                AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(TempSrcBuf.BaseAddrY));
                AmbaMisra_TypeCast(&(BCopy.pDstImg), &(pMem->Y2YSrcBuf.BufBase));
                BCopy.SrcRowStride = TempSrcBuf.Pitch;
                BCopy.DstRowStride = (UINT32)NewPitch;
                BCopy.BltWidth     = TempSrcBuf.Window.Width;
                BCopy.BltHeight    = TempSrcBuf.Window.Height;
                BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
                Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                    RetVal = SVC_NG;
                }

                TmpAddr = pMem->Y2YSrcBuf.BufBase + (NewPitch * TempSrcBuf.Window.Height);
                AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(TempSrcBuf.BaseAddrUV));
                AmbaMisra_TypeCast(&(BCopy.pDstImg), &TmpAddr);
                BCopy.SrcRowStride = TempSrcBuf.Pitch;
                BCopy.DstRowStride = (UINT32)NewPitch;
                BCopy.BltWidth     = TempSrcBuf.Window.Width;
                BCopy.BltHeight    = TempSrcBuf.Window.Height;
                if (TempSrcBuf.DataFmt == AMBA_DSP_YUV420) {
                    BCopy.BltHeight    = BCopy.BltHeight / 2U;
                }
                BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
                Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                    RetVal = SVC_NG;
                }

                Err = AmbaGDMA_WaitAllCompletion(500U);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "AmbaGDMA_WaitAllCompletion failed %u", Err, 0U);
                }

                TempSrcBuf.BaseAddrY  = pMem->Y2YSrcBuf.BufBase;
                TempSrcBuf.BaseAddrUV = TmpAddr;
                TempSrcBuf.Pitch      = (UINT16)NewPitch;

            }
        }

        /* still y2y cannot assign the dst uv addr, so we y2y to another buffer first then copy it to dst addr  */
        {
            Err = AmbaWrap_memcpy(&TempDstBuf, pDestYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaWrap_memcpy failed %u", Err, 0U);
            }

            {
                UINT32 NeedSize;

                if (TempSrcBuf.DataFmt == AMBA_DSP_YUV422) {
                    NeedSize    = (UINT32)TempDstBuf.Pitch * (UINT32)TempDstBuf.Window.Height * 2U;
                } else {
                    NeedSize    = ((UINT32)TempDstBuf.Pitch * (UINT32)TempDstBuf.Window.Height * 3U) / 2U;
                }

                if (pMem->Y2YDstBuf.BufSize < NeedSize) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_STL_DEC, "Y2YDstBuf is not enough %u/%u", pMem->Y2YDstBuf.BufSize, NeedSize);
                }
            }

            TempDstBuf.BaseAddrY  = pMem->Y2YDstBuf.BufBase;
            TempDstBuf.Pitch      = TempDstBuf.Window.Width;
            TempDstBuf.BaseAddrUV = TempDstBuf.BaseAddrY + ((ULONG)TempDstBuf.Pitch * (ULONG)TempDstBuf.Window.Height);

            {
                ULONG  AlignStart;
                ULONG  AlignSize;

                AlignStart = (ULONG)TempDstBuf.BaseAddrY & AMBA_CACHE_LINE_MASK;
                AlignSize  = ((ULONG)TempDstBuf.BaseAddrY + ((ULONG)TempDstBuf.Pitch * (ULONG)TempDstBuf.Window.Height)) - AlignStart;
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
                }

                AlignStart = (ULONG)TempDstBuf.BaseAddrUV & AMBA_CACHE_LINE_MASK;
                if (TempDstBuf.DataFmt == AMBA_DSP_YUV420) {
                    AlignSize  = ((ULONG)TempDstBuf.BaseAddrUV + (((ULONG)TempDstBuf.Pitch * (ULONG)TempDstBuf.Window.Height) / 2UL)) - AlignStart;
                } else {
                    AlignSize  = ((ULONG)TempDstBuf.BaseAddrUV + ((ULONG)TempDstBuf.Pitch * (ULONG)TempDstBuf.Window.Height)) - AlignStart;
                }
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
                }
            }
        }

        if (RetVal == SVC_OK) {
            Err = AmbaDSP_StillYuv2Yuv(&TempSrcBuf, &TempDstBuf, &IsoCfg, 0U, NULL);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaDSP_StillYuv2Yuv failed! return 0x%x", Err, 0U);
                RetVal = SVC_NG;
            }
        }
    }

    if (RetVal == SVC_OK) {
        Err = AmbaKAL_EventFlagGet(&G_SDecMgr.EventId, SDEC_FLAG_DECODE_DONE, AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 5000U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkStillDec_StartY2Y Timeout %u", Err, 0U);
            RetVal = SVC_NG;
        }
    }

    /*  copy yuv data to the assigned address */
    if (RetVal == SVC_OK) {
        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
            AMBA_GDMA_BLOCK_s   BCopy;
            {
                ULONG  AlignStart;
                ULONG  AlignSize;

                AlignStart = (ULONG)pDestYuvBufAddr->BaseAddrY & AMBA_CACHE_LINE_MASK;
                AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrY + ((ULONG)pDestYuvBufAddr->Pitch * (ULONG)G_SDecMgr.DecReport.YuvHeight)) - AlignStart;
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheInvalidate(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
                }

                AlignStart = (ULONG)pDestYuvBufAddr->BaseAddrUV & AMBA_CACHE_LINE_MASK;
                if (pDestYuvBufAddr->DataFmt == AMBA_DSP_YUV420) {
                    AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrUV + (((ULONG)pDestYuvBufAddr->Pitch * (ULONG)G_SDecMgr.DecReport.YuvHeight) / 2UL)) - AlignStart;
                } else {
                    AlignSize  = ((ULONG)pDestYuvBufAddr->BaseAddrUV + ((ULONG)pDestYuvBufAddr->Pitch * (ULONG)G_SDecMgr.DecReport.YuvHeight)) - AlignStart;
                }
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheInvalidate(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DEC, "SvcPlat_CacheFlush failed %u", Err, 0U);
                }
            }

            AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(G_SDecMgr.DecReport.YAddr));
            AmbaMisra_TypeCast(&(BCopy.pDstImg), &(pDestYuvBufAddr->BaseAddrY));
            BCopy.SrcRowStride = G_SDecMgr.DecReport.YuvPitch;
            BCopy.DstRowStride = pDestYuvBufAddr->Pitch;
            BCopy.BltWidth     = G_SDecMgr.DecReport.YuvWidth;
            BCopy.BltHeight    = G_SDecMgr.DecReport.YuvHeight;
            BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
            Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                RetVal = SVC_NG;
            }

            AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(G_SDecMgr.DecReport.UVAddr));
            AmbaMisra_TypeCast(&(BCopy.pDstImg), &(pDestYuvBufAddr->BaseAddrUV));
            BCopy.SrcRowStride = G_SDecMgr.DecReport.YuvPitch;
            BCopy.DstRowStride = pDestYuvBufAddr->Pitch;
            BCopy.BltWidth     = G_SDecMgr.DecReport.YuvWidth;
            BCopy.BltHeight    = G_SDecMgr.DecReport.YuvHeight;
            if (G_SDecMgr.DecReport.YuvFormat == AMBA_DSP_YUV420) {
                BCopy.BltHeight = BCopy.BltHeight / 2U;
            }
            BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
            Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                RetVal = SVC_NG;
            }

            Err = AmbaGDMA_WaitAllCompletion(500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DEC, "AmbaGDMA_WaitAllCompletion failed %u", Err, 0U);
            }

            G_SDecMgr.DecReport.YAddr = pDestYuvBufAddr->BaseAddrY;
            G_SDecMgr.DecReport.UVAddr = pDestYuvBufAddr->BaseAddrUV;
        }
    }

    return RetVal;
}

