/**
 *  @file SvcPbkStillDisp.c
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
 *  @details still display related APIs
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaSensor.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaGDMA.h"
#include "AmbaShell.h"
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
#include "SvcPbkStillDisp.h"
#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcDisplay.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcPbkPictDisp.h"
#include "SvcPbkStillDec.h"
#include "SvcWrap.h"
#include "SvcIsoCfg.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcPlat.h"

#define SVC_LOG_STL_DISP    "STLDISP"

static PBK_STILL_DISP_MGR_s     G_SDispMgr;
static PBK_STILL_DISP_CTRL_s    AmbaSDisp_Ctrl;
static SVC_TASK_CTRL_s          StillDispTaskCtrl GNU_SECTION_NOZEROINIT;

static void PbkDebugUInt5(const char *FmtStr, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    if (pDispInfo->ShowDbgLog > 0U) {
        SvcWrap_PrintUL(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static UINT32 StillDispSingleFrame(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig);

static void* StillDispTaskEntry(void* EntryArg)
{
    UINT32 Msg = 0U, LoopFlag = 1U, Err;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopFlag);

    while (LoopFlag == 1U) {
        Err = AmbaKAL_MsgQueueReceive(&(G_SDispMgr.MsgQueueId), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_MsgQueueReceive failed %u", Err, 0U);
            LoopFlag = 0U;
        }

        if (Msg == STILL_DISP_MSG_SINGLE_FRAME) {
            Err = StillDispSingleFrame(G_SDispMgr.NumVout, &G_SDispMgr.DispConfig[0]);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "StillDispSingleFrame failed %u", Err, 0U);
            }
        }
    }

    return NULL;
}

static UINT32 StillDispDspEventHandler(const void *pEventData)
{
    UINT32                                      VoutIdx, Err;
    AMBA_KAL_SEMAPHORE_INFO_s                   SemaphoreInfo = {0};
    const AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s  *pEventInfo = NULL;

    AmbaMisra_TypeCast(&pEventInfo, &pEventData);
    for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
        Err = AmbaKAL_SemaphoreQuery(&(G_SDispMgr.VoutBufSemId[VoutIdx]), &SemaphoreInfo);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreQuery failed %u", Err, 0U);
        }

        {
            if ((pEventInfo->YAddr != G_SDispMgr.CurVoutYAddr[pEventInfo->VoutIdx]) &&
                (SemaphoreInfo.CurrentCount < STILL_DISP_SEM_INIT_COUNT)) {

                Err = AmbaKAL_SemaphoreGive(&(G_SDispMgr.VoutBufSemId[VoutIdx]));
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreGive failed %u", Err, 0U);
                }

                G_SDispMgr.CurVoutYAddr[pEventInfo->VoutIdx] = pEventInfo->YAddr;
                G_SDispMgr.PendingDispNum--;
                PbkDebugUInt5("Disp event: disp Vout[%d]", pEventInfo->VoutIdx, 0U, 0U, 0U, 0U);
                if (G_SDispMgr.PendingDispNum == 0U) {
                    Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_DONE);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
                    }
                }

                PbkDebugUInt5("[StlDecDispRdy] VoutIdx[%d] Y[0x%llX] UV[0x%llX]",
                            pEventInfo->VoutIdx, pEventInfo->YAddr, pEventInfo->UVAddr, 0U, 0U);
                PbkDebugUInt5("                Win[%dx%d] P[%d] Fmt[%d]",
                            pEventInfo->Width, pEventInfo->Height, pEventInfo->Pitch, pEventInfo->DataFmt, 0U);
            }
        }
    }

    return SVC_OK;
}

static void DrawLine(ULONG YBufAddr, ULONG UVBufAddr, UINT32 Pitch, UINT32 Height,
                     UINT32 OffsetX, UINT32 OffsetY,
                     UINT32 LineWidth, UINT32 LineHeight,
                     UINT8 Y, UINT8 U, UINT8 V)
{
    UINT16 *pYAddr = NULL;
    UINT16 *pUVAddr = NULL;
    UINT32 i = 0U, j, k;
    ULONG  SrcU32;
    UINT16 YValue;
    UINT16 UVValue;
    UINT32 LineWidthPtr16, PaddingPtr16;
    UINT32 LineWidth0, LineHeight0, OffsetX0, OffsetY0;

    SrcU32 = (UINT32)Y;
    SrcU32 = SrcU32 << 8U;
    SrcU32 = SrcU32 | (UINT32)Y;
    YValue = (UINT16)SrcU32;

    SrcU32 = (UINT32)V;
    SrcU32 = SrcU32 << 8U;
    SrcU32 = SrcU32 | (UINT32)U;
    UVValue = (UINT16)SrcU32;

    OffsetX0 = OffsetX;
    OffsetY0 = OffsetY;
    OffsetX0 &= 0xFFFFFFFEU;
    OffsetY0 &= 0xFFFFFFFEU;
    LineWidth0 = (LineWidth + 1U) & 0xFFFFFFFEU;
    LineHeight0 = (LineHeight + 1U) & 0xFFFFFFFEU;

    if ((OffsetX0 + LineWidth0) > Pitch) {
        LineWidth0 = Pitch - OffsetX0;
    }

    if ((OffsetY0 + LineHeight0) > Height) {
        LineHeight0 = Height - OffsetY0;
    }

    SrcU32 = (YBufAddr + ((ULONG)OffsetY0 * (ULONG)Pitch)) + OffsetX0;
    AmbaMisra_TypeCast(&pYAddr, &SrcU32);

#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
    SrcU32 = (UVBufAddr + ((ULONG)OffsetY0 * (ULONG)Pitch)) + OffsetX0;
#else
    SrcU32 = (UVBufAddr + (((ULONG)OffsetY0 *(ULONG) Pitch) / 2UL)) + OffsetX0;
#endif
    AmbaMisra_TypeCast(&pUVAddr, &SrcU32);

    LineWidthPtr16 = (LineWidth0 >> 1U);
    PaddingPtr16 = ((Pitch - LineWidth0) >> 1U);

    k = 0U;
    for (j = 0U; j < LineHeight0; j++) {
        for (i = 0U; i < LineWidthPtr16; i++) {
            pYAddr[k] = YValue;
            k++;
        }
        k += PaddingPtr16;
    }

    k = 0U;

#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV420)
    LineHeight0 = LineHeight0/2U;
#endif

    for (j = 0U; j < LineHeight0; j++) {
        for (i = 0U; i < LineWidthPtr16; i++) {
            pUVAddr[k] = UVValue;
            k++;
        }
        k += PaddingPtr16;
    }
}

static void DuplexDispUpdate(UINT32 DpxFovId, const AMBA_DSP_WINDOW_s *pROI)
{
    UINT32               Err;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();
    AMBA_IK_MODE_CFG_s   ImgMode = {0};

    ImgMode.ContextId = DpxFovId;

    {
        AMBA_IK_WINDOW_SIZE_INFO_s  WindowSizeInfo = {0};

        Err = AmbaIK_GetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaIK_GetWindowSizeInfo failed %u", Err, 0U);
        }

        WindowSizeInfo.VinSensor.StartX = pROI->OffsetX;
        WindowSizeInfo.VinSensor.StartY = pROI->OffsetY;
        WindowSizeInfo.VinSensor.Width  = pROI->Width;
        WindowSizeInfo.VinSensor.Height = pROI->Height;
        WindowSizeInfo.Main.Width       = pROI->Width;
        WindowSizeInfo.Main.Height      = pROI->Height;
        Err = AmbaIK_SetWindowSizeInfo(&ImgMode, &WindowSizeInfo);
        if (Err != IK_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaIK_SetWindowSizeInfo failed %u", Err, 0U);
        }
    }

    {
        UINT64                           AttachedRawSeq = 0ULL;
        AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s  IsoCfgCtrl;
        AMBA_IK_EXECUTE_CONTAINER_s      CfgExecInfo;

        AmbaSvcWrap_MisraMemset(&IsoCfgCtrl, 0, sizeof(IsoCfgCtrl));
        AmbaSvcWrap_MisraMemset(&CfgExecInfo, 0, sizeof(CfgExecInfo));
        Err = AmbaIK_ExecuteConfig(&ImgMode, &CfgExecInfo);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaIK_ExecuteConfig failed", Err, 0U);
        }

        IsoCfgCtrl.ViewZoneId = (UINT16)ImgMode.ContextId;
        IsoCfgCtrl.CfgIndex   = CfgExecInfo.IkId;
        IsoCfgCtrl.CtxIndex   = ImgMode.ContextId;
        AmbaMisra_TypeCast(&IsoCfgCtrl.CfgAddress, &CfgExecInfo.pIsoCfg);
        Err = AmbaDSP_LiveviewUpdateIsoCfg(1, &IsoCfgCtrl, &AttachedRawSeq);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_LiveviewUpdateIsoCfg failed", Err, 0U);
        }
    }

    {
        UINT32         i, j;
        SVC_LIV_INFO_s LivInfo;

        SvcLiveview_InfoGet(&LivInfo);

        for (i = 0U; i < pCfg->DispNum; i++) {
            for (j = 0U; j < pCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == DpxFovId) {
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.OffsetX = pROI->OffsetX;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.OffsetY = pROI->OffsetY;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.Width   = pROI->Width;
                    LivInfo.pStrmCfg[i].pChanCfg[j].ROI.Height  = pROI->Height;
                }
            }
        }
        SvcLiveview_Update();
    }

}

static UINT32 StillDispSingleFrame(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig)
{
    UINT32                              i, j, NumImg = 0U, Err;
    UINT32                              VoutIdx, CurFrmBufIndex, ActualFlags = 0U;
    const PBK_STILL_DISP_VOUT_INFO_s    *pVoutInfo;
    const PBK_STILL_DISP_VOUT_FRM_BUF_s *pVoutBuf;
    const PBK_STILL_DISP_PARAM_s        *pParam;
    AMBA_DSP_YUV_IMG_BUF_s              SrcYuv;
    AMBA_DSP_YUV_IMG_BUF_s              DstYuv;
    AMBA_DSP_STLDEC_YUV2YUV_s           YuvOp;
    AMBA_DSP_YUV_IMG_BUF_s              DispYuv;
    AMBA_DSP_VOUT_VIDEO_CFG_s           VoutConfig;
    SVC_DISP_INFO_s                     DispInfo;
    UINT32                              NumDisp;
    const AMBA_DSP_VOUT_VIDEO_CFG_s     *pVideo = NULL;
    UINT8                               *pBuff;
    const SVC_PICT_DISP_INFO_s          *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    Err = AmbaWrap_memset(&DstYuv, 0, sizeof(DstYuv));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    /* Draw images to Vout frame buffer */
    for (i = 0; i < NumVout; i++) {
        VoutIdx = pDispConfig[i].VoutIdx;
        pVoutInfo = (PBK_STILL_DISP_VOUT_INFO_s *)&(AmbaSDisp_Ctrl.VoutInfo[VoutIdx]);

        /* Prepare vout frame buffer */
        if (pDispConfig[i].FlushVoutBuf == 0U) {
            pVoutBuf = &(pVoutInfo->VoutBuf[G_SDispMgr.CurFrmBufIndex[VoutIdx]]);
        } else {
            /* If flush is enabled, increase CurFrmBufIndex */
            G_SDispMgr.CurFrmBufIndex[VoutIdx]++;
            if (G_SDispMgr.CurFrmBufIndex[VoutIdx] >= pVoutInfo->FrmBufNum) {
                G_SDispMgr.CurFrmBufIndex[VoutIdx] = 0U;
            }

            CurFrmBufIndex = G_SDispMgr.CurFrmBufIndex[VoutIdx];
            pVoutBuf = &(pVoutInfo->VoutBuf[CurFrmBufIndex]);
            PbkDebugUInt5("VoutIdx %d", VoutIdx, 0U, 0U, 0U, 0U);

            /* wait vout buffer is valid */
            if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                if (AmbaKAL_SemaphoreTake(&(G_SDispMgr.VoutBufSemId[(UINT32)VoutIdx]), 500U) == KAL_ERR_NONE) {
                } else {
                    SvcLog_NG(SVC_LOG_STL_DISP, "StillDispSingleFrame SemaphoreTake timeout ", 0U, 0U);
                }
            }

            /* set back ground color => black (0x00, 0x80, 0x80) */
            AmbaMisra_TypeCast(&pBuff, &(pVoutBuf->YBase));
            AmbaUtility_MemorySetU8(pBuff, 0x00, pVoutBuf->Pitch*pVoutBuf->Height);

            AmbaMisra_TypeCast(&pBuff, &(pVoutBuf->UVBase));
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            AmbaUtility_MemorySetU8(pBuff, 0x80, pVoutBuf->Pitch*pVoutBuf->Height);
#else
            AmbaUtility_MemorySetU8(pBuff, 0x80, (pVoutBuf->Pitch * pVoutBuf->Height) / 2U);
#endif

            {
                ULONG  AlignStart;
                ULONG  AlignSize;

                AlignStart = (ULONG)pVoutBuf->YBase & AMBA_CACHE_LINE_MASK;
                AlignSize  = ((ULONG)pVoutBuf->YBase + ((ULONG)pVoutBuf->Pitch * pVoutBuf->Height)) - AlignStart;
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheClean(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
                }

                AlignStart = (ULONG)pVoutBuf->UVBase & AMBA_CACHE_LINE_MASK;
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
                AlignSize  = ((ULONG)pVoutBuf->UVBase + ((ULONG)pVoutBuf->Pitch * pVoutBuf->Height)) - AlignStart;
#else
                AlignSize  = ((ULONG)pVoutBuf->UVBase + (((ULONG)pVoutBuf->Pitch * pVoutBuf->Height) / 2U)) - AlignStart;
#endif
                AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                Err = SvcPlat_CacheClean(AlignStart, AlignSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
                }
            }
        }

        /* Retrive disp parameter and set y2y cmd */
        pParam = pDispConfig[i].pParam;
        NumImg = pDispConfig[i].NumImg;
        if ((pParam == NULL) || (NumImg == 0U)) {
            continue;
        }

        {
            SrcYuv.DataFmt        = (UINT8)pParam->SrcChromaFmt;
            SrcYuv.BaseAddrY      = pParam->SrcBufYBase;
            SrcYuv.BaseAddrUV     = pParam->SrcBufUVBase;
            SrcYuv.Pitch          = (UINT16)pParam->SrcBufPitch;
            SrcYuv.Window.OffsetX = (UINT16)pParam->SrcOffsetX;
            SrcYuv.Window.OffsetY = (UINT16)pParam->SrcOffsetY;
            SrcYuv.Window.Width   = (UINT16)pParam->SrcWidth;
            SrcYuv.Window.Height  = (UINT16)pParam->SrcHeight;

            DstYuv.DataFmt        = SVCAG_SDEC_DISP_FORMAT;
            DstYuv.BaseAddrY      = (pVoutBuf->YBase + ((ULONG)pParam->DstOffsetY * (ULONG)pVoutBuf->Pitch)) + pParam->DstOffsetX;
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            DstYuv.BaseAddrUV     = (pVoutBuf->UVBase + ((ULONG)pParam->DstOffsetY * (ULONG)pVoutBuf->Pitch)) + pParam->DstOffsetX;
#else
            DstYuv.BaseAddrUV     = (pVoutBuf->UVBase + (((ULONG)pParam->DstOffsetY * (ULONG)pVoutBuf->Pitch) / 2UL)) + pParam->DstOffsetX;
#endif
            DstYuv.Pitch          = (UINT16)pVoutBuf->Pitch;
            DstYuv.Window.OffsetX = 0;
            DstYuv.Window.OffsetY = 0;
            DstYuv.Window.Width   = (UINT16)pParam->DstWidth;
            DstYuv.Window.Height  = (UINT16)pParam->DstHeight;
            YuvOp.RotateFlip      = (UINT8)pParam->RotateFlip;
            YuvOp.LumaGain        = 128;
            pParam++;
        }

        /* only need to do y2y for Screennail disp for playback mode */
        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            SvcPbkStillDec_CheckYuv2YuvParam(1, &SrcYuv, &DstYuv, &YuvOp);
            PbkDebugUInt5("SrcYuv Fmt %d, Y 0x%llx, UV 0x%llx, P %d, X %d", \
                    SrcYuv.DataFmt, SrcYuv.BaseAddrY, SrcYuv.BaseAddrUV, SrcYuv.Pitch, SrcYuv.Window.OffsetX);
            PbkDebugUInt5("OffsetY %d, Width %d, Height %d", \
                    SrcYuv.Window.OffsetY, SrcYuv.Window.Width, SrcYuv.Window.Height, 0U, 0U);

            PbkDebugUInt5("DstYuv Fmt %d, Y 0x%llx, UV 0x%llx, P %d, X %d", \
                    DstYuv.DataFmt, DstYuv.BaseAddrY, DstYuv.BaseAddrUV, DstYuv.Pitch, DstYuv.Window.OffsetX);
            PbkDebugUInt5("OffsetY %d, Width %d, Height %d flip %d", \
                    DstYuv.Window.OffsetY, DstYuv.Window.Width, DstYuv.Window.Height, YuvOp.RotateFlip, 0U);

            Err = SvcPbkStillDec_StartY2Y(&SrcYuv, &DstYuv, &YuvOp);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
            }
        } else {
            AMBA_GDMA_BLOCK_s   BCopy;

            AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(SrcYuv.BaseAddrY));
            AmbaMisra_TypeCast(&(BCopy.pDstImg), &(DstYuv.BaseAddrY));
            BCopy.SrcRowStride = SrcYuv.Pitch;
            BCopy.DstRowStride = DstYuv.Pitch;
            BCopy.BltWidth     = SrcYuv.Window.Width;
            BCopy.BltHeight    = SrcYuv.Window.Height;
            BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
            Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
            }

            AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(SrcYuv.BaseAddrUV));
            AmbaMisra_TypeCast(&(BCopy.pDstImg), &(DstYuv.BaseAddrUV));
            BCopy.SrcRowStride = SrcYuv.Pitch;
            BCopy.DstRowStride = DstYuv.Pitch;
            BCopy.BltWidth     = SrcYuv.Window.Width;
            BCopy.BltHeight    = SrcYuv.Window.Height;
            if (SrcYuv.DataFmt == AMBA_DSP_YUV420) {
                BCopy.BltHeight = BCopy.BltHeight / 2U;
            }
            BCopy.PixelFormat  = AMBA_GDMA_8_BIT;
            Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
            }
        }
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
        Err = AmbaGDMA_WaitAllCompletion(500U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaGDMA_WaitAllCompletion failed %u", Err, 0U);
        }
    }

    /* Display Vout frame buffer */
    for (i = 0U; i < NumVout; i++) {
        VoutIdx = pDispConfig[i].VoutIdx;
        pVoutInfo = &(AmbaSDisp_Ctrl.VoutInfo[VoutIdx]);
        pVoutBuf = &(pVoutInfo->VoutBuf[G_SDispMgr.CurFrmBufIndex[VoutIdx]]);

        Err = AmbaWrap_memset(&DispYuv, 0, sizeof(DispYuv));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
        }
        Err = AmbaWrap_memset(&VoutConfig, 0, sizeof(VoutConfig));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
        }

        DispYuv.DataFmt        = SVCAG_SDEC_DISP_FORMAT;
        DispYuv.BaseAddrY      = pVoutBuf->YBase;
        DispYuv.BaseAddrUV     = pVoutBuf->UVBase;
        DispYuv.Pitch          = (UINT16)pVoutBuf->Pitch;
        DispYuv.Window.OffsetX = 0;
        DispYuv.Window.OffsetY = 0;
        DispYuv.Window.Width   = (UINT16)pVoutBuf->Width;
        DispYuv.Window.Height  = (UINT16)pVoutBuf->Height;

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            ULONG  AlignStart;
            ULONG  AlignSize;

            AlignStart = (ULONG)DispYuv.BaseAddrY & AMBA_CACHE_LINE_MASK;
            AlignSize  = ((ULONG)DispYuv.BaseAddrY + ((ULONG)DispYuv.Pitch * DispYuv.Window.Height)) - AlignStart;
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
            }

            AlignStart = (ULONG)DispYuv.BaseAddrUV & AMBA_CACHE_LINE_MASK;
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            AlignSize  = ((ULONG)DispYuv.BaseAddrUV + ((ULONG)DispYuv.Pitch*DispYuv.Window.Height)) - AlignStart;
#else
            AlignSize  = ((ULONG)DispYuv.BaseAddrUV + (((ULONG)DispYuv.Pitch*DispYuv.Window.Height) / 2U)) - AlignStart;
#endif
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        }

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            if ((pVoutInfo->RotateFlip == AMBA_DSP_ROTATE_90) || (pVoutInfo->RotateFlip == AMBA_DSP_ROTATE_270)) {
                VoutConfig.Window.OffsetX = 0;
                VoutConfig.Window.OffsetY = 0;
                VoutConfig.Window.Width   = (UINT16)pVoutInfo->Width;
                VoutConfig.Window.Height  = (UINT16)pVoutInfo->Height;
                VoutConfig.RotateFlip     = (UINT8)pVoutInfo->RotateFlip;
                Err = AmbaDSP_StillDecDispYuvImg((UINT8)VoutIdx, &DispYuv, &VoutConfig);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_StillDecDispYuvImg failed %u", Err, 0U);
                }
            } else {
                SvcDisplay_InfoGet(&DispInfo);
                NumDisp = *(DispInfo.pNumDisp);
                for (j = 0U; j < NumDisp; j++) {
                    if (DispInfo.pDispCfg[j].VoutID == VoutIdx) {
                        pVideo = &(DispInfo.pDispCfg[j].VideoCfg);
                        PbkDebugUInt5("SvcPbkStill_Y2Disp, Found DispCfg[%d]", j, 0U, 0U, 0U, 0U);
                        break;
                    }
                }

                if (NULL != pVideo) {
                    VoutConfig.Window.Width = pVideo->Window.Width;
                    VoutConfig.Window.Height = pVideo->Window.Height;
                    VoutConfig.Window.OffsetX = pVideo->Window.OffsetX;
                    VoutConfig.Window.OffsetY = pVideo->Window.OffsetY;

                    VoutConfig.RotateFlip = pVideo->RotateFlip;
                    VoutConfig.VideoSource = pVideo->VideoSource;

                    PbkDebugUInt5("VoutIdx %d", VoutIdx, 0U, 0U, 0U, 0U);
                    PbkDebugUInt5("DataFmt %d, BaseAddrY 0x%llx, BaseAddrUV 0x%llx, Pitch %d", \
                                        DispYuv.DataFmt, DispYuv.BaseAddrY, DispYuv.BaseAddrUV, DispYuv.Pitch, 0U);
                    PbkDebugUInt5("OffsetX %d, OffsetY %d, Width %d, Height %d", \
                                        DispYuv.Window.OffsetX, DispYuv.Window.OffsetY, DispYuv.Window.Width, DispYuv.Window.Height, 0U);
                    PbkDebugUInt5("2 OffsetX %d, OffsetY %d, Width %d, Height %d", \
                                        VoutConfig.Window.OffsetX, VoutConfig.Window.OffsetY, VoutConfig.Window.Width, VoutConfig.Window.Height, 0U);
                    PbkDebugUInt5("RotateFlip %d, VideoSource %d", VoutConfig.RotateFlip, VoutConfig.VideoSource, 0U, 0U, 0U);
                    Err = AmbaDSP_StillDecDispYuvImg((UINT8)VoutIdx, &DispYuv, &VoutConfig);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_StillDecDispYuvImg failed %u", Err, 0U);
                    }
                }
            }
        } else {
            {
                UINT16                   FovId = (UINT16)pDispInfo->FovId;
                AMBA_DSP_EXT_YUV_BUF_s   ExtYuv = {0};

                ExtYuv.ExtYuvBuf.DataFmt        = DispYuv.DataFmt;
                ExtYuv.ExtYuvBuf.Pitch          = DispYuv.Pitch;
                ExtYuv.ExtYuvBuf.Window         = DispYuv.Window;
                ExtYuv.pExtME1Buf               = NULL;
                ExtYuv.pExtME0Buf               = NULL;
                ExtYuv.ExtYuvBuf.BaseAddrY      = DispYuv.BaseAddrY;
                ExtYuv.ExtYuvBuf.BaseAddrUV     = DispYuv.BaseAddrUV;

                DuplexDispUpdate(pDispInfo->FovId, &(DispYuv.Window));

                Err = AmbaDSP_LiveviewFeedYuvData(1U, &FovId, &ExtYuv);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_LiveviewFeedYuvData failed %u", Err, 0U);
                }
            }
        }
        G_SDispMgr.PendingDispNum++;
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        Err = AmbaKAL_EventFlagGet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_DONE, AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 5000U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagGet STILL_DISP_FLAG_DONE failed %u", Err, 0U);
        }

        if ((ActualFlags & STILL_DISP_FLAG_DONE) > 0U) {
            Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_FLAG_IDLE failed %u", Err, 0U);
            }
            /* Feedback to user */
            if(NULL != AmbaSDisp_Ctrl.pFbEventId) {
                Err = AmbaKAL_EventFlagSet(AmbaSDisp_Ctrl.pFbEventId, STILL_DISP_EVENT_DISP_TO_VOUT);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_EVENT_DISP_TO_VOUT failed %u", Err, 0U);
                }
            }
        }
    } else {
        Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_FLAG_IDLE failed %u", Err, 0U);
        }
        /* Feedback to user */
        if(NULL != AmbaSDisp_Ctrl.pFbEventId) {
            Err = AmbaKAL_EventFlagSet(AmbaSDisp_Ctrl.pFbEventId, STILL_DISP_EVENT_DISP_TO_VOUT);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_EVENT_DISP_TO_VOUT failed %u", Err, 0U);
            }
        }
    }

    return SVC_OK;
}

/**
* SvcPbkStillDisp_TaskCreate
* @return ErrorCode
*/
UINT32 SvcPbkStillDisp_TaskCreate(void)
{
    static char                StillDispMsgQName[32];
    static char                StillDispEvtName[] = "StillDispMgrFlg";
    static UINT8               StillDispStack[SVC_STILL_DISP_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32                     RetVal = SVC_OK, Err, i, j, VoutId;
    SVC_PICT_DISP_MEM_s        *PictDispMem = NULL;
    const SVC_PICT_DISP_INFO_s *pDispInfo;

    PbkDebugUInt5("SvcPbkStillDisp_TaskCreate start", 0U, 0U, 0U, 0U, 0U);
    Err = AmbaWrap_memset(&G_SDispMgr, 0, sizeof(PBK_STILL_DISP_MGR_s));
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
    }

    if (AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT,
                                        &StillDispDspEventHandler) != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskCreate: failed to reg event handler ", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        ULONG  Num;

        AmbaMisra_TypeCast(&Num, &(G_SDispMgr.MsgQueueId));
        if (0U < AmbaUtility_UInt32ToStr(StillDispMsgQName, 32U, (UINT32)Num, 16U)) {
            AmbaUtility_StringAppend(StillDispMsgQName, 32, "StlDispMgrMQ");
        }

        if (SVC_OK != AmbaKAL_MsgQueueCreate(&(G_SDispMgr.MsgQueueId),
                                            StillDispMsgQName,
                                            (UINT32)sizeof(G_SDispMgr.MsgQueue[0]),
                                            &(G_SDispMgr.MsgQueue[0]),
                                            (UINT32)sizeof(G_SDispMgr.MsgQueue))) {
            SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskCreate: failed to MsgQueueCreate", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            if (SVC_OK != AmbaKAL_EventFlagCreate(&(G_SDispMgr.EventId), StillDispEvtName)) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskCreate: failed to EventFlagCreate", 0U, 0U);
                RetVal = SVC_NG;
            } else {

                for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
                    Err = AmbaWrap_memset(&(G_SDispMgr.VoutBufSemId[i]), 0, sizeof(AMBA_KAL_SEMAPHORE_t));
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
                    }

                    Err = AmbaKAL_SemaphoreCreate(&(G_SDispMgr.VoutBufSemId[i]), NULL, STILL_DISP_SEM_INIT_COUNT);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreCreate failed %u", Err, 0U);
                        RetVal = SVC_NG;
                    }
                }

                if (RetVal == SVC_OK) {
                    Err = AmbaKAL_EventFlagClear(&(G_SDispMgr.EventId), 0xFFFFFFFFU);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
                    }

                    Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet failed %u", Err, 0U);
                    }

                    StillDispTaskCtrl.Priority    = SVC_PBK_STILL_DISP_TASK_PRI;
                    StillDispTaskCtrl.EntryFunc   = StillDispTaskEntry;
                    StillDispTaskCtrl.EntryArg    = 0U;
                    StillDispTaskCtrl.pStackBase  = StillDispStack;
                    StillDispTaskCtrl.StackSize   = SVC_STILL_DISP_STACK_SIZE;
                    StillDispTaskCtrl.CpuBits     = SVC_PBK_STILL_DISP_TASK_CPU_BITS;
                    if (SVC_OK != SvcTask_Create("StillDispTask", &StillDispTaskCtrl)) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "PictDispTask created failed!", 0U, 0U);
                        RetVal = SVC_NG;
                    } else {
                        SvcPbkPictDisp_GetInfo(&pDispInfo);
                        SvcPbkPictDisp_GetPictDispMem(&PictDispMem);

                        G_SDispMgr.NumVout = 0;

                        for (i = 0U; i < pDispInfo->DispNum; i++) {
                            VoutId = pDispInfo->Disp[i].VoutId;
                            if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
                                ULONG YSize, UVSize;
                                if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                                    AmbaSDisp_Ctrl.VoutInfo[VoutId].FrmBufNum = STILL_DISP_VOUT_FRAME_NUM;
                                } else {
                                    AmbaSDisp_Ctrl.VoutInfo[VoutId].FrmBufNum = 1U;
                                }
                                for (j = 0U; j < AmbaSDisp_Ctrl.VoutInfo[VoutId].FrmBufNum; j++) {
                                    YSize   = (ULONG)ALIGN64(pDispInfo->Disp[i].DispWin.Width) * (ULONG)ALIGN16(pDispInfo->Disp[i].DispWin.Height);
                                    #if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
                                    UVSize  = YSize;
                                    #else
                                    UVSize  = YSize / 2U;
                                    #endif

                                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].YBase  = (ULONG)PictDispMem->DispYuvBuf[VoutId].BufBase +
                                                                                        ((YSize + UVSize) * j);
                                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].UVBase = (ULONG)PictDispMem->DispYuvBuf[VoutId].BufBase +
                                                                                        (((YSize + UVSize) * j) + YSize);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    PbkDebugUInt5("SvcPbkStillDisp_TaskCreate Done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* SvcPbkStillDisp_TaskDelete
* @return ErrorCode
*/
UINT32 SvcPbkStillDisp_TaskDelete(void)
{
    UINT32 ActualFlags = 0U, RetVal = SVC_OK, i, Err;

    PbkDebugUInt5("SvcPbkStillDisp_TaskDelete start", 0U, 0U, 0U, 0U, 0U);
    if (AmbaKAL_EventFlagGet(&(G_SDispMgr.EventId),
                             STILL_DISP_FLAG_IDLE,
                             AMBA_KAL_FLAGS_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             5000U) != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskDelete: Task is not idle", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        if (SVC_OK != AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT, &StillDispDspEventHandler)) {
            SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskDelete: failed to unregister event handler", 0U, 0U);
            RetVal = SVC_NG;
        } else {
            if (SVC_OK != SvcTask_Destroy(&StillDispTaskCtrl)) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskDelete: failed to destroy task", 0U, 0U);
                RetVal = SVC_NG;
            } else {
                if (SVC_OK != AmbaKAL_MsgQueueDelete(&(G_SDispMgr.MsgQueueId))) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskDelete: failed to MsgQueueDelete", 0U, 0U);
                    RetVal = SVC_NG;
                } else {
                    if (SVC_OK != AmbaKAL_EventFlagDelete(&(G_SDispMgr.EventId))) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_TaskDelete: failed to EventFlagDelete", 0U, 0U);
                        RetVal = SVC_NG;
                    } else {
                        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
                            Err = AmbaKAL_SemaphoreDelete(&(G_SDispMgr.VoutBufSemId[i]));
                            if (Err != SVC_OK) {
                                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreDelete failed %u", Err, 0U);
                                RetVal = SVC_NG;
                            }
                        }
                    }
                }
            }
        }
    }

    PbkDebugUInt5("SvcPbkStillDisp_TaskDelete done", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* SvcPbkStillDisp_Setup
* @param [in]  pFeedbackEventID FeedbackEventID
* @param [in]  DispNum the number of display
* @param [in]  pDispInfo display Info array
* @return ErrorCode
*/
UINT32 SvcPbkStillDisp_Setup(AMBA_KAL_EVENT_FLAG_t *pFeedbackEventID, UINT32 DispNum,
                            const PBK_STILL_DISP_VOUT_INFO_s *pDispInfo)
{
    UINT32 BufferSize = 0U, RetVal = SVC_OK, BufferAR, i, j, VoutId;

    AmbaSDisp_Ctrl.pFbEventId = pFeedbackEventID;

    for (i = 0U; i < DispNum; i++) {
        VoutId = pDispInfo[i].VoutId;
        if (VoutId < AMBA_DSP_MAX_VOUT_NUM) {
            AmbaSDisp_Ctrl.VoutInfo[VoutId].Width      = pDispInfo[i].Width;
            AmbaSDisp_Ctrl.VoutInfo[VoutId].Height     = pDispInfo[i].Height;
            AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutAR     = pDispInfo[i].VoutAR;
            AmbaSDisp_Ctrl.VoutInfo[VoutId].RotateFlip = pDispInfo[i].RotateFlip;

            if ((pDispInfo[i].RotateFlip == AMBA_DSP_ROTATE_90) || (pDispInfo[i].RotateFlip == AMBA_DSP_ROTATE_270)) {
                for (j = 0U; j < AmbaSDisp_Ctrl.VoutInfo[VoutId].FrmBufNum; j++) {
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Width  = pDispInfo[i].Height;
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Height = pDispInfo[i].Width;
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Pitch  = pDispInfo[i].Pitch;

                    if (pDispInfo[VoutId].VoutAR == AR_4X3) {
                        BufferAR = (((UINT32)3U << 16U) / 4U);
                        AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].BufAR = BufferAR;
                    }
                }
            } else {
                for (j = 0U; j < AmbaSDisp_Ctrl.VoutInfo[VoutId].FrmBufNum; j++) {
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Width  = pDispInfo[i].Width;
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Height = pDispInfo[i].Height;
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].Pitch  = pDispInfo[i].Pitch;
                    AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[j].BufAR  = pDispInfo[i].VoutAR;
                }
            }

            BufferSize = AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[0].Pitch * AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[0].Height;
            if ((AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[0].YBase + BufferSize) > AmbaSDisp_Ctrl.VoutInfo[VoutId].VoutBuf[0].UVBase) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDisp_Setup Disp%u out of memory", VoutId, 0U);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_STL_DISP, "DispNum %u should not exceed AMBA_DSP_MAX_VOUT_NUM %u", DispNum, AMBA_DSP_MAX_VOUT_NUM);
            RetVal = SVC_NG;
        }
    }

    return RetVal;
}

/**
* SvcPbkStillDisp_SingleFrame
* @param [in]  NumVout NumVout
* @param [in]  pDispConfig DispConfig
* @return ErrorCode
*/
UINT32 SvcPbkStillDisp_SingleFrame(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig)
{
    UINT32   Msg = STILL_DISP_MSG_SINGLE_FRAME, RetVal = SVC_OK;
    UINT32   i, Err;

    PbkDebugUInt5("SvcPbkStillDisp_SingleFrame start", 0U, 0U, 0U, 0U, 0U);
    if(NULL != AmbaSDisp_Ctrl.pFbEventId) {
        Err = AmbaKAL_EventFlagClear(AmbaSDisp_Ctrl.pFbEventId, STILL_DISP_EVENT_DISP_TO_VOUT);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
        }
    }

    Err = AmbaKAL_EventFlagClear(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagClear failed %u", Err, 0U);
    }

    G_SDispMgr.NumVout = NumVout;
    for (i = 0U; i < NumVout; i++) {
        if(pDispConfig[i].NumImg > STILL_DISP_MAX_IMG_PER_FRAME) {
            RetVal = SVC_NG;
            break;
        }

        G_SDispMgr.DispConfig[i].VoutIdx      = pDispConfig[i].VoutIdx;
        G_SDispMgr.DispConfig[i].FlushVoutBuf = pDispConfig[i].FlushVoutBuf;
        G_SDispMgr.DispConfig[i].NumImg       = pDispConfig[i].NumImg;
        G_SDispMgr.DispConfig[i].pParam       = &(G_SDispMgr.DispParam[pDispConfig[i].VoutIdx][0]);
        Err = AmbaWrap_memcpy(G_SDispMgr.DispConfig[i].pParam, pDispConfig[i].pParam, pDispConfig[i].NumImg * sizeof(PBK_STILL_DISP_PARAM_s));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memcpy failed %u", Err, 0U);
        }
    }

    if (RetVal != SVC_NG) {
        Err = AmbaKAL_MsgQueueSend(&(G_SDispMgr.MsgQueueId), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_MsgQueueSend failed %u", Err, 0U);
        }
    }

    return RetVal;
}

/**
* SvcPbkStillDisp_GetCurrentVoutBuf
* @param [in]  VoutIdx VoutIdx
* @param [out]  pVoutBuf VoutBuf
*/
void SvcPbkStillDisp_GetCurrentVoutBuf(UINT32 VoutIdx, PBK_STILL_DISP_VOUT_FRM_BUF_s *pVoutBuf)
{
    const PBK_STILL_DISP_VOUT_INFO_s *pVoutInfo = &AmbaSDisp_Ctrl.VoutInfo[VoutIdx];
    UINT32 CurFrmBufIndex = G_SDispMgr.CurFrmBufIndex[VoutIdx];

    pVoutBuf->YBase = pVoutInfo->VoutBuf[CurFrmBufIndex].YBase;
    pVoutBuf->UVBase = pVoutInfo->VoutBuf[CurFrmBufIndex].UVBase;
    pVoutBuf->Width = pVoutInfo->VoutBuf[CurFrmBufIndex].Width;
    pVoutBuf->Height = pVoutInfo->VoutBuf[CurFrmBufIndex].Height;
    pVoutBuf->Pitch = pVoutInfo->VoutBuf[CurFrmBufIndex].Pitch;
    pVoutBuf->BufAR = pVoutInfo->VoutBuf[CurFrmBufIndex].BufAR;
}

/**
* SvcPbkStillDisp_WaitDspDispIdle
*/
void SvcPbkStillDisp_WaitDspDispIdle(void)
{
    UINT32                        Err, VoutIdx;
    const SVC_PICT_DISP_INFO_s    *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo = {0};

        PbkDebugUInt5("SvcPbkStillDisp_WaitDspDispIdle start", 0U, 0U, 0U, 0U, 0U);

        for (VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
            Err = AmbaKAL_SemaphoreQuery(&G_SDispMgr.VoutBufSemId[VoutIdx], &SemaphoreInfo);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreQuery failed %u", Err, 0U);
            }

            while (SemaphoreInfo.CurrentCount < STILL_DISP_SEM_INIT_COUNT) {
                Err = AmbaKAL_TaskSleep(1);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_TaskSleep failed %u", Err, 0U);
                }
                Err = AmbaKAL_SemaphoreQuery(&G_SDispMgr.VoutBufSemId[VoutIdx], &SemaphoreInfo);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreQuery failed %u", Err, 0U);
                }
                PbkDebugUInt5("VoutBufSemId[%u] non-zero", VoutIdx, 0U, 0U, 0U, 0U);
            }
        }

        PbkDebugUInt5("SvcPbkStillDisp_WaitDspDispIdle done", 0U, 0U, 0U, 0U, 0U);
    }
}

/**
* SvcPbkStillDisp_RescaleFrmToVoutBuf
* @param [in]  NumVout
* @param [in]  pDispConfig DispConfig
*/
void SvcPbkStillDisp_RescaleFrmToVoutBuf(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig)
{
    UINT32                               i, j = 0, NumImg = 0, Err;
    UINT32                               VoutIdx;
    const PBK_STILL_DISP_VOUT_INFO_s     *pVoutInfo;
    const PBK_STILL_DISP_VOUT_FRM_BUF_s  *pVoutBuf;
    const PBK_STILL_DISP_PARAM_s         *pParam;
    AMBA_DSP_YUV_IMG_BUF_s               SrcYuv;
    AMBA_DSP_YUV_IMG_BUF_s               DstYuv;
    AMBA_DSP_STLDEC_YUV2YUV_s            YuvOp;
    UINT8                                *pBuff;
    const SVC_PICT_DISP_INFO_s           *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    for (i = 0U; i < NumVout; i++) {
        VoutIdx = pDispConfig[i].VoutIdx;
        pVoutInfo = (PBK_STILL_DISP_VOUT_INFO_s *)&(AmbaSDisp_Ctrl.VoutInfo[VoutIdx]);

        G_SDispMgr.CurFrmBufIndex[VoutIdx]++;
        if (G_SDispMgr.CurFrmBufIndex[VoutIdx] >= pVoutInfo->FrmBufNum) {
            G_SDispMgr.CurFrmBufIndex[VoutIdx] = 0U;
        }

        /* set back ground color => black (0x00, 0x80, 0x80) */
        pVoutBuf = &(pVoutInfo->VoutBuf[G_SDispMgr.CurFrmBufIndex[VoutIdx]]);
        AmbaMisra_TypeCast(&pBuff, &(pVoutBuf->YBase));
        AmbaUtility_MemorySetU8(pBuff, 0x00, pVoutBuf->Pitch*pVoutBuf->Height);

        AmbaMisra_TypeCast(&pBuff, &(pVoutBuf->UVBase));
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
        AmbaUtility_MemorySetU8(pBuff, 0x80, pVoutBuf->Pitch * pVoutBuf->Height);
#else
        AmbaUtility_MemorySetU8(pBuff, 0x80, (pVoutBuf->Pitch * pVoutBuf->Height) / 2U);
#endif
    }

    for (i = 0U; i < NumVout; i++) {
        VoutIdx = pDispConfig[i].VoutIdx;
        pVoutInfo = &(AmbaSDisp_Ctrl.VoutInfo[VoutIdx]);
        pVoutBuf = &(pVoutInfo->VoutBuf[G_SDispMgr.CurFrmBufIndex[VoutIdx]]);

        pParam = pDispConfig[i].pParam;
        NumImg = pDispConfig[i].NumImg;
        if ((pParam == NULL) || (NumImg == 0U)) {
            continue;
        }

        for (j = 0U; j < NumImg; j++)
        {
            SrcYuv.DataFmt        = (UINT8)pParam->SrcChromaFmt;
            SrcYuv.BaseAddrY      = pParam->SrcBufYBase;
            SrcYuv.BaseAddrUV     = pParam->SrcBufUVBase;
            SrcYuv.Pitch          = (UINT16)pParam->SrcBufPitch;
            SrcYuv.Window.OffsetX = (UINT16)pParam->SrcOffsetX;
            SrcYuv.Window.OffsetY = (UINT16)pParam->SrcOffsetY;
            SrcYuv.Window.Width   = (UINT16)pParam->SrcWidth;
            SrcYuv.Window.Height  = (UINT16)pParam->SrcHeight;

            DstYuv.DataFmt        = SVCAG_SDEC_DISP_FORMAT;
            DstYuv.Pitch          = (UINT16)pVoutBuf->Pitch;
            DstYuv.BaseAddrY      = (pVoutBuf->YBase + ((ULONG)pParam->DstOffsetY * (ULONG)DstYuv.Pitch)) + pParam->DstOffsetX;
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            DstYuv.BaseAddrUV     = (pVoutBuf->UVBase + ((ULONG)pParam->DstOffsetY * (ULONG)DstYuv.Pitch)) + pParam->DstOffsetX;
#else
            DstYuv.BaseAddrUV     = (pVoutBuf->UVBase + (((ULONG)pParam->DstOffsetY * (ULONG)DstYuv.Pitch) / 2UL)) + pParam->DstOffsetX;
#endif

            DstYuv.Window.OffsetX = 0;
            DstYuv.Window.OffsetY = 0;
            DstYuv.Window.Width   = (UINT16)pParam->DstWidth;
            DstYuv.Window.Height  = (UINT16)pParam->DstHeight;
            YuvOp.RotateFlip      = (UINT8)pParam->RotateFlip;
            YuvOp.LumaGain        = 128;
            pParam++;

            SvcPbkStillDec_CheckYuv2YuvParam(1U, &SrcYuv, &DstYuv, &YuvOp);

            PbkDebugUInt5("SrcYuv Fmt %d, Y 0x%llx, UV 0x%llx, P %d, X %d", \
                    SrcYuv.DataFmt, SrcYuv.BaseAddrY, SrcYuv.BaseAddrUV, SrcYuv.Pitch, SrcYuv.Window.OffsetX);
            PbkDebugUInt5("OffsetY %d, Width %d, Height %d", \
                    SrcYuv.Window.OffsetY, SrcYuv.Window.Width, SrcYuv.Window.Height, 0U, 0U);

            PbkDebugUInt5("DstYuv Fmt %d, Y 0x%llx, UV 0x%llx, P %d, X %d", \
                    DstYuv.DataFmt, DstYuv.BaseAddrY, DstYuv.BaseAddrUV, DstYuv.Pitch, DstYuv.Window.OffsetX);
            PbkDebugUInt5("OffsetY %d, Width %d, Height %d flip %d", \
                    DstYuv.Window.OffsetY, DstYuv.Window.Width, DstYuv.Window.Height, YuvOp.RotateFlip, 0U);
            PbkDebugUInt5("SvcPbkStillDisp_RescaleFrmToVoutBuf, __LINE__ %d, pVoutBuf->YBase 0x%llx, UVBase 0x%xll", \
                                __LINE__, pVoutBuf->YBase, pVoutBuf->UVBase, 0U, 0U);

            /* we don't need to do rescale for liveview mode, just copy the data to vout buffer */
            if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
                Err = SvcPbkStillDec_StartY2Y(&SrcYuv, &DstYuv, &YuvOp);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStillDec_StartY2Y failed %u", Err, 0U);
                }
            } else {
                AMBA_GDMA_BLOCK_s   BCopy;

                AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(SrcYuv.BaseAddrY));
                AmbaMisra_TypeCast(&(BCopy.pDstImg), &(DstYuv.BaseAddrY));
                BCopy.SrcRowStride = SrcYuv.Pitch;
                BCopy.DstRowStride = DstYuv.Pitch;
                BCopy.BltWidth     = SrcYuv.Window.Width;
                BCopy.BltHeight    = SrcYuv.Window.Height;
                BCopy.PixelFormat  = AMBA_GDMA_8_BIT;

                {
                    ULONG  AlignStart;
                    ULONG  AlignSize;

                    AlignStart = (ULONG)DstYuv.BaseAddrY & AMBA_CACHE_LINE_MASK;
                    AlignSize  = ((ULONG)DstYuv.BaseAddrY + ((ULONG)BCopy.DstRowStride * (ULONG)BCopy.BltHeight)) - AlignStart;
                    AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                    AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                    Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheFlush failed %u", Err, 0U);
                    }
                }

                Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                }

                AmbaMisra_TypeCast(&(BCopy.pSrcImg), &(SrcYuv.BaseAddrUV));
                AmbaMisra_TypeCast(&(BCopy.pDstImg), &(DstYuv.BaseAddrUV));
                BCopy.SrcRowStride = SrcYuv.Pitch;
                BCopy.DstRowStride = DstYuv.Pitch;
                BCopy.BltWidth     = SrcYuv.Window.Width;
                BCopy.BltHeight    = SrcYuv.Window.Height;
                if (SrcYuv.DataFmt == AMBA_DSP_YUV420) {
                    BCopy.BltHeight = BCopy.BltHeight / 2U;
                }
                BCopy.PixelFormat  = AMBA_GDMA_8_BIT;

                {
                    ULONG  AlignStart;
                    ULONG  AlignSize;

                    AlignStart = (ULONG)DstYuv.BaseAddrUV & AMBA_CACHE_LINE_MASK;
                    AlignSize  = ((ULONG)DstYuv.BaseAddrUV + ((ULONG)BCopy.DstRowStride * (ULONG)BCopy.BltHeight)) - AlignStart;
                    AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
                    AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
                    Err = SvcPlat_CacheFlush(AlignStart, AlignSize);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheFlush failed %u", Err, 0U);
                    }
                }

                Err = SvcPbkPictDisp_BlockCopy(&BCopy, NULL, 0U, 500U);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkPictDisp_BlockCopy failed %u", Err, 0U);
                }
            }
        }

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_DUPLEX) {
            Err = AmbaGDMA_WaitAllCompletion(500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaGDMA_WaitAllCompletion failed %u", Err, 0U);
            }
        }
    }
}

/**
* SvcPbkStillDisp_FlushVoutBuf
* @param [in]  NumVout
* @param [in]  pDispConfig DispConfig
*/
void SvcPbkStillDisp_FlushVoutBuf(UINT32 NumVout, const PBK_STILL_DISP_CONFIG_s *pDispConfig)
{
    UINT32                               i, j, Err;
    UINT32                               VoutIdx, ActualFlags = 0U;
    const PBK_STILL_DISP_VOUT_INFO_s     *pVoutInfo;
    const PBK_STILL_DISP_VOUT_FRM_BUF_s  *pVoutBuf;
    AMBA_DSP_YUV_IMG_BUF_s               DispYuv;
    AMBA_DSP_VOUT_VIDEO_CFG_s            VoutConfig;
    SVC_DISP_INFO_s                      DispInfo;
    UINT32                               NumDisp;
    const AMBA_DSP_VOUT_VIDEO_CFG_s      *pVideo = NULL;
    const SVC_PICT_DISP_INFO_s           *pDispInfo;

    SvcPbkPictDisp_GetInfo(&pDispInfo);

    for (i = 0U; i < NumVout; i++) {
        VoutIdx = pDispConfig[i].VoutIdx;

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            PbkDebugUInt5("SvcPbkStillDisp_FlushVoutBuf AmbaKAL_SemaphoreTake, VoutIdx %d", VoutIdx, 0U, 0U, 0U, 0U);
            Err = AmbaKAL_SemaphoreTake(&(G_SDispMgr.VoutBufSemId[(UINT32)VoutIdx]), 500U);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_SemaphoreTake failed %u", Err, 0U);
            }
        }

        pVoutInfo = &(AmbaSDisp_Ctrl.VoutInfo[VoutIdx]);
        pVoutBuf = &(pVoutInfo->VoutBuf[G_SDispMgr.CurFrmBufIndex[VoutIdx]]);

        Err = AmbaWrap_memset(&DispYuv, 0, sizeof(DispYuv));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
        }
        Err = AmbaWrap_memset(&VoutConfig, 0, sizeof(VoutConfig));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaWrap_memset failed %u", Err, 0U);
        }

        DispYuv.DataFmt        = SVCAG_SDEC_DISP_FORMAT;
        DispYuv.BaseAddrY      = pVoutBuf->YBase;
        DispYuv.BaseAddrUV     = pVoutBuf->UVBase;
        DispYuv.Pitch          = (UINT16)pVoutBuf->Pitch;
        DispYuv.Window.OffsetX = 0;
        DispYuv.Window.OffsetY = 0;
        DispYuv.Window.Width   = (UINT16)pVoutBuf->Width;
        DispYuv.Window.Height  = (UINT16)pVoutBuf->Height;

        {
            ULONG  AlignStart;
            ULONG  AlignSize;

            AlignStart = (ULONG)DispYuv.BaseAddrY & AMBA_CACHE_LINE_MASK;
            AlignSize  = ((ULONG)DispYuv.BaseAddrY + ((ULONG)DispYuv.Pitch * DispYuv.Window.Height)) - AlignStart;
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
            }

            AlignStart = (ULONG)DispYuv.BaseAddrUV & AMBA_CACHE_LINE_MASK;
#if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
            AlignSize  = ((ULONG)DispYuv.BaseAddrUV + ((ULONG)DispYuv.Pitch*DispYuv.Window.Height)) - AlignStart;
#else
            AlignSize  = ((ULONG)DispYuv.BaseAddrUV + (((ULONG)DispYuv.Pitch*DispYuv.Window.Height) / 2UL)) - AlignStart;
#endif
            AlignSize += (AMBA_CACHE_LINE_SIZE - 1U);
            AlignSize  = AlignSize  & AMBA_CACHE_LINE_MASK;
            Err = SvcPlat_CacheClean(AlignStart, AlignSize);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPlat_CacheClean failed %u", Err, 0U);
            }
        }

        SvcDisplay_InfoGet(&DispInfo);
        NumDisp = *(DispInfo.pNumDisp);
        for (j = 0U; j < NumDisp; j++) {
            if (DispInfo.pDispCfg[j].VoutID == VoutIdx) {
                pVideo = &(DispInfo.pDispCfg[j].VideoCfg);
                PbkDebugUInt5("SvcPbkStill_Y2Disp, Found DispCfg[%d]", j, 0U, 0U, 0U, 0U);
                break;
            }
        }

        if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
            if (NULL != pVideo) {
                VoutConfig.Window.Width = pVideo->Window.Width;
                VoutConfig.Window.Height = pVideo->Window.Height;
                VoutConfig.Window.OffsetX = pVideo->Window.OffsetX;
                VoutConfig.Window.OffsetY = pVideo->Window.OffsetY;

                VoutConfig.RotateFlip = pVideo->RotateFlip;
                VoutConfig.VideoSource = pVideo->VideoSource;

                PbkDebugUInt5("VoutIdx %d", VoutIdx, 0U, 0U, 0U, 0U);
                PbkDebugUInt5("DataFmt %d, BaseAddrY 0x%llx, BaseAddrUV 0x%llx, Pitch %d", \
                                    DispYuv.DataFmt, DispYuv.BaseAddrY, DispYuv.BaseAddrUV, DispYuv.Pitch, 0U);
                PbkDebugUInt5("OffsetX %d, OffsetY %d, Width %d, Height %d", \
                                    DispYuv.Window.OffsetX, DispYuv.Window.OffsetY, DispYuv.Window.Width, DispYuv.Window.Height, 0U);
                PbkDebugUInt5("2 OffsetX %d, OffsetY %d, Width %d, Height %d", \
                                    VoutConfig.Window.OffsetX, VoutConfig.Window.OffsetY, VoutConfig.Window.Width, VoutConfig.Window.Height, 0U);
                PbkDebugUInt5("RotateFlip %d, VideoSource %d", VoutConfig.RotateFlip, VoutConfig.VideoSource, 0U, 0U, 0U);

                PbkDebugUInt5("VoutConfig.VideoSource %d", VoutConfig.VideoSource, 0U, 0U, 0U, 0U);
                Err = AmbaDSP_StillDecDispYuvImg((UINT8)VoutIdx, &DispYuv, &VoutConfig);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_StillDecDispYuvImg failed %u", Err, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STL_DISP, "SvcPbkStill_Y2Disp, Found DispCfg[%d] failed", j, 0U);
            }
        } else {
            {
                UINT16                   FovId = (UINT16)pDispInfo->FovId;
                AMBA_DSP_EXT_YUV_BUF_s   ExtYuv = {0U};

                ExtYuv.ExtYuvBuf.DataFmt        = DispYuv.DataFmt;
                ExtYuv.ExtYuvBuf.Pitch          = DispYuv.Pitch;
                ExtYuv.ExtYuvBuf.Window         = DispYuv.Window;
                ExtYuv.pExtME1Buf               = NULL;
                ExtYuv.pExtME0Buf               = NULL;
                ExtYuv.ExtYuvBuf.BaseAddrY      = DispYuv.BaseAddrY;
                ExtYuv.ExtYuvBuf.BaseAddrUV     = DispYuv.BaseAddrUV;

                DuplexDispUpdate(pDispInfo->FovId, &(ExtYuv.ExtYuvBuf.Window));

                Err = AmbaDSP_LiveviewFeedYuvData(1U, &FovId, &ExtYuv);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaDSP_LiveviewFeedYuvData failed %u", Err, 0U);
                }
            }
        }
        G_SDispMgr.PendingDispNum++;
    }

    if (pDispInfo->Mode == SVC_PICT_DISP_MODE_PLAYBACK) {
        Err = AmbaKAL_EventFlagGet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_DONE, AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 5000U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagGet STILL_DISP_FLAG_DONE failed %u", Err, 0U);
        }
        if ((ActualFlags & STILL_DISP_FLAG_DONE) > 0U) {
            Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_FLAG_IDLE failed %u", Err, 0U);
            }
            /* Feedback to user */
            if(NULL != AmbaSDisp_Ctrl.pFbEventId) {
                Err = AmbaKAL_EventFlagSet(AmbaSDisp_Ctrl.pFbEventId, STILL_DISP_EVENT_DISP_TO_VOUT);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_EVENT_DISP_TO_VOUT failed %u", Err, 0U);
                }
            }
        }
    } else {
        Err = AmbaKAL_EventFlagSet(&(G_SDispMgr.EventId), STILL_DISP_FLAG_IDLE);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_FLAG_IDLE failed %u", Err, 0U);
        }
        /* Feedback to user */
        if(NULL != AmbaSDisp_Ctrl.pFbEventId) {
            Err = AmbaKAL_EventFlagSet(AmbaSDisp_Ctrl.pFbEventId, STILL_DISP_EVENT_DISP_TO_VOUT);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_STL_DISP, "AmbaKAL_EventFlagSet STILL_DISP_EVENT_DISP_TO_VOUT failed %u", Err, 0U);
            }
        }
    }
}

/**
* SvcPbkStillDisp_DrawRectangle
* @param [in]  pRectInfo RectInfo
*/
void SvcPbkStillDisp_DrawRectangle(const PBK_STILL_DISP_DRAW_RECT_s *pRectInfo)
{
    ULONG  YBufAddr = pRectInfo->YBufAddr;
    ULONG  UVBufAddr = pRectInfo->UVBufAddr;
    UINT32 Pitch = pRectInfo->Pitch;
    UINT32 Height = pRectInfo->Height;
    UINT32 StartX = pRectInfo->StartX;
    UINT32 StartY = pRectInfo->StartY;
    UINT32 RectWidth = pRectInfo->RectWidth;
    UINT32 RectHeight = pRectInfo->RectHeight;
    UINT32 LineWidth = pRectInfo->LineWidth;
    UINT8 Y = pRectInfo->Y;
    UINT8 U = pRectInfo->U;
    UINT8 V = pRectInfo->V;

    PbkDebugUInt5("__LINE__ %d Pitch %d, Height %d, StartX %d, StartY %d", __LINE__, Pitch, Height, StartX, StartY);
    PbkDebugUInt5("__LINE__ %d RectWidth %d, LineWidth %d, YBufAddr 0x%llx, UVBufAddr0x%llx", \
            __LINE__, RectWidth, LineWidth, YBufAddr, UVBufAddr);

    /* Top line */
    DrawLine(YBufAddr, UVBufAddr, Pitch, Height,
             StartX, StartY, RectWidth, LineWidth,
             Y, U, V);
    /* Left line*/
    DrawLine(YBufAddr, UVBufAddr, Pitch, Height,
             StartX, StartY, LineWidth, RectHeight,
             Y, U, V);
    /* Right line*/
    DrawLine(YBufAddr, UVBufAddr, Pitch, Height,
            (StartX + RectWidth - LineWidth), StartY, LineWidth, RectHeight,
            Y, U, V);
    /* Bottom line*/
    DrawLine(YBufAddr, UVBufAddr, Pitch, Height,
             StartX, (StartY + RectHeight - LineWidth), RectWidth, LineWidth,
             Y, U, V);
}
