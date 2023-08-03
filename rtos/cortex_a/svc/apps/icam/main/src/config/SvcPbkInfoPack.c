/**
 *  @file SvcPbkInfoPack.c
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
 *  @details svc playback information pack
 *
 */

#include ".svc_autogen"

#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaFPD.h"
#include "AmbaVfs.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaGDMA.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcOsd.h"
#include "SvcPbkCtrl.h"
#include "SvcPbkPictDisp.h"
#include "SvcTaskList.h"
#include "SvcResCfg.h"
#include "SvcBRateCalc.h"
#include "SvcVinSrc.h"
#include "SvcUserPref.h"
#include "SvcIKCfg.h"
#include "SvcPbkInfoPack.h"

#define SVC_LOG_PINFO   "PINFO"

/**
 * Playback configuration function
 * @param [in] DecoderID decoder id
 * @param [out] pInfo pointer to playback control info
 */
void SvcInfoPack_PbkConfig(UINT32 DecoderID, SVC_PBK_CTRL_INFO_s *pInfo)
{
    UINT32                 i, Bit = 1U, Err, VoutCnt = 0U;
    const SVC_RES_CFG_s    *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s  *pDisp;
    SVC_USER_PREF_s        *pUserPref;

    if (SVC_OK != AmbaWrap_memset(pInfo, 0, sizeof(SVC_PBK_CTRL_INFO_s))) {
        SvcLog_NG(SVC_LOG_PINFO, "AmbaWrap_memset failed", 0U, 0U);
    }

    Err = SvcUserPref_Get(&pUserPref);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PINFO, "SvcUserPref_Get failed", 0U, 0U);
    }
    /* Drive name */
    pInfo->Drive = pUserPref->MainStgDrive[0];

    /* Vout */
    for (i = 0; i < pResCfg->DispNum; i++) {
        if ((pResCfg->DispBits & (Bit << i)) == 0U) {
            continue;
        } else {
            pDisp = &(pResCfg->DispStrm[i]);

            {
                const AMBA_FPD_OBJECT_s  *pFpdObj;
                AMBA_FPD_INFO_s          FpdInfo;

                AmbaMisra_TypeCast(&pFpdObj, &(pResCfg->DispStrm[i].pDriver));
                Err = pFpdObj->FpdGetInfo(&FpdInfo);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_PINFO, "FpdGetInfo failed", 0U, 0U);
                }

                pInfo->VoutInfo[VoutCnt].VoutIdx            = (UINT8)pDisp->VoutID;
                pInfo->VoutInfo[VoutCnt].VoutRotateFlip     = 0U;
                pInfo->VoutInfo[VoutCnt].VoutWindow.OffsetX = 0U;
                pInfo->VoutInfo[VoutCnt].VoutWindow.OffsetY = 0U;
                pInfo->VoutInfo[VoutCnt].VoutWindow.Width   = pDisp->StrmCfg.Win.Width;
                pInfo->VoutInfo[VoutCnt].VoutWindow.Height  = pDisp->StrmCfg.Win.Height;
                pInfo->VoutInfo[VoutCnt].IsInterlace        = pDisp->FrameRate.Interlace;
                if ((FpdInfo.AspectRatio.X != 0U) && (FpdInfo.AspectRatio.Y != 0U)) {
                    pInfo->VoutInfo[VoutCnt].VoutWindowAR   = ((UINT32)FpdInfo.AspectRatio.X << 16UL) / (UINT32)FpdInfo.AspectRatio.Y;
                } else {
                    pInfo->VoutInfo[VoutCnt].VoutWindowAR   = ((UINT32)pDisp->StrmCfg.Win.Width << 16UL) / (UINT32)pDisp->StrmCfg.Win.Height;
                }

                VoutCnt++;
            }
        }
    }

    pInfo->VoutNum = VoutCnt;

    /* Playback Task */
    if (DecoderID == 0U) {
        pInfo->PbkTaskInfo.TaskPriority = SVC_PBK_CTRL_TASK_PRI;
        pInfo->PbkTaskInfo.TaskCpuBits  = SVC_PBK_CTRLK_TASK_CPU_BITS;
    }

    pInfo->DspMode = SVC_PBK_CTRL_MODE_PBK;

    if (pUserPref->OperationMode == 1U) {
        UINT32 FovSrc = 0U;
        for (i = 0U; i < pResCfg->FovNum; i++) {
            Err = SvcResCfg_GetFovSrc(i, &FovSrc);
            if ((Err == SVC_OK) && (FovSrc == SVC_VIN_SRC_MEM_DEC)) {
                UINT32 ID = 0U;
                Err = SvcResCfg_GetDecIDOfFovIdx(i, &ID);
                if ((Err == SVC_OK) && (ID == DecoderID)) {
                    pInfo->DspMode  = SVC_PBK_CTRL_MODE_DPX;
                    pInfo->FeedBits = pResCfg->FovCfg[i].DecFeedBits;
                    break;
                }
            }
        }
    }
}

/**
 * Still playback configuration function
 */
void SvcInfoPack_StlPbkConfig(void)
{
    UINT32               Rval, i, VinSrc = 0U, Err, FovId = 0U, Y2YId = 0U;
    SVC_USER_PREF_s      *pSvcUserPref;
    SVC_PICT_DISP_INIT_s Init;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    AmbaSvcWrap_MisraMemset(&Init, 0, sizeof(Init));
    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_PICT_DISP, &(Init.BufInit.PictDisp.BufBase), &(Init.BufInit.PictDisp.BufSize));
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_PINFO, "SvcBuffer_Request() %u failed", SMEM_PF0_ID_PICT_DISP, 0U);
    }

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VDEC_BS, &(Init.BufInit.JpegDecBits.BufBase), &(Init.BufInit.JpegDecBits.BufSize));
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_PINFO, "SvcBuffer_Request() %u failed", SMEM_PF0_ID_VDEC_BS, 0U);
    }

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STLDEC_YUV, &(Init.BufInit.JpegDecOut.BufBase), &(Init.BufInit.JpegDecOut.BufSize));
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_PINFO, "SvcBuffer_Request() %u failed", SMEM_PF0_ID_STLDEC_YUV, 0U);
    }

    Err = SvcIKCfg_GetTotalCtxNum(&Y2YId);
    if (Err != SVC_OK) {
        SvcLog_NG(SVC_LOG_PINFO, "SvcIKCfg_GetTotalCtxNum() failed %u", Err, 0U);
    }
    Init.Y2YCtxId = Y2YId - 1U;

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &VinSrc);
                if ((Err == SVC_OK) && (VinSrc == SVC_VIN_SRC_MEM_DEC)) {
                    FovId  = i;
                    break;
                }
            }
            Init.FovId       = FovId;
            Init.Mode        = SVC_PICT_DISP_MODE_DUPLEX;
            Init.ShowDbgLog  = pSvcUserPref->ShowThmViewLog;
            SvcLog_OK(SVC_LOG_PINFO, "PictDisp Mode: Duplex, FovId %u, Y2YCtxId %u", FovId, Y2YId);
        } else {
            Init.Mode        = SVC_PICT_DISP_MODE_PLAYBACK;
            Init.ShowDbgLog  = pSvcUserPref->ShowThmViewLog;
            SvcLog_OK(SVC_LOG_PINFO, "PictDisp Mode: Playback, FovId %u, Y2YCtxId %u", FovId, Y2YId);
        }
    }

    SvcPbkPictDisp_Init(pCfg, &Init);
}

/**
 * Playback resource configuration function
 * @param [in] DecoderID decoder id
 * @param [out] pDspRes pointer to dsp resource configuration
 */
void SvcInfoPack_PbkResConfig(AMBA_DSP_RESOURCE_s *pDspRes)
{
    UINT32                 MaxDecBrate, Err, VinSrc = 0U, IsDuplex = 0U, IsVin = 0U, Width, Height, i;
    UINT16                 MaxDecWidth, MaxDecHeight, MaxDecRatioOfGopNM, DpxNum = 0U;
    UINT32                 DpxFovId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
    const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();
    const SVC_STRM_CFG_s   *pStrmCfg;
    SVC_USER_PREF_s        *pSvcUserPref;

    AmbaMisra_TouchUnused(&DpxFovId);

    /* if it's in duplex mode */
    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        if (pSvcUserPref->OperationMode == 1U) {
            for (i = 0U; i < pCfg->FovNum; i++) {
                Err = SvcResCfg_GetFovSrc(i, &VinSrc);
                if ((Err == SVC_OK) && (VinSrc == SVC_VIN_SRC_MEM_DEC)) {
                    IsDuplex  = 1U;
                    DpxFovId[DpxNum] = i;
                    DpxNum++;
                } else {
                    IsVin     = 1U;
                }
            }
        }
    }

    for (i = 0U; i < (UINT16)(pCfg->DispNum); i++) {
        pStrmCfg = &(pCfg->DispStrm[i].StrmCfg);
        pDspRes->DisplayResource.MaxVoutWidth[pCfg->DispStrm[i].VoutID] = pStrmCfg->Win.Width;

        Err = SvcOsd_GetOsdBufSize(i, &Width, &Height);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_PINFO, "SvcOsd_GetOsdBufSize failed", Err, 0U);
        }
        pDspRes->DisplayResource.MaxOsdBufWidth[pCfg->DispStrm[i].VoutID] = (UINT16)(Width << (UINT32)SVC_OSD_PIXEL_SIZE_SFT);
    }

    if (IsDuplex == 1U) {
        pDspRes->DecodeResource.MaxDecodeStreamNum    = DpxNum;
    } else {
        pDspRes->DecodeResource.MaxDecodeStreamNum    = 1U;
    }

    for (i = 0U; i < (UINT32)(pDspRes->DecodeResource.MaxDecodeStreamNum); i++) {
#if defined(SVCAG_H22_DUPLEX_MAX_STRM_FMT)
        if (IsDuplex == 1U) {
            pDspRes->DecodeResource.MaxStrmFmt[i]         = 1U; /* Bit[0]:H264, Bit[1]:H265, Bit[2]:MJPG */
        } else {
            pDspRes->DecodeResource.MaxStrmFmt[i]         = 7U;
        }
#elif defined(CONFIG_SOC_CV2FS)
        pDspRes->DecodeResource.MaxStrmFmt[i]         = 1U;
#else
        pDspRes->DecodeResource.MaxStrmFmt[i]         = 7U;
#endif
    }

    MaxDecWidth         = 1920U;
    MaxDecHeight        = 1080U;
    MaxDecBrate         = 150000000U;

#if defined(CONFIG_ICAM_B_FRAME_DECODE)
    MaxDecRatioOfGopNM  = 3U;    /* only need to assign GopM */
#else
    MaxDecRatioOfGopNM  = 1U;
#endif


    if ((IsDuplex == 1U) && (IsVin == 1U)) {
        MaxDecWidth         = 1920U;
        MaxDecHeight        = 1080U;
    } else if (IsDuplex == 1U) {
        for (i = 0U; i < (UINT32)DpxNum; i++) {
            if (MaxDecWidth < pCfg->FovCfg[DpxFovId[i]].RawWin.Width) {
                MaxDecWidth = pCfg->FovCfg[DpxFovId[i]].RawWin.Width;
            }
            if (MaxDecHeight < pCfg->FovCfg[DpxFovId[i]].RawWin.Height) {
                MaxDecHeight = pCfg->FovCfg[DpxFovId[i]].RawWin.Height;
            }
        }
    } else {
        /* playback mode */
        MaxDecWidth         = 3840U;
        MaxDecHeight        = 2160U;
    }

#if defined(CONFIG_ICAM_RECORD_USED)
    for (i = 0U; i < pCfg->RecNum; i++) {
        UINT32 Brate;

        if (MaxDecWidth < pCfg->RecStrm[i].StrmCfg.MaxWin.Width) {
            MaxDecWidth = pCfg->RecStrm[i].StrmCfg.MaxWin.Width;
        }
        if (MaxDecHeight < pCfg->RecStrm[i].StrmCfg.MaxWin.Height) {
            MaxDecHeight = pCfg->RecStrm[i].StrmCfg.MaxWin.Height;
        }
        Brate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
        if (MaxDecBrate < Brate) {
            MaxDecBrate = Brate;
        }
    }
#endif

    pDspRes->DecodeResource.MaxFrameWidth         = MaxDecWidth;
    pDspRes->DecodeResource.MaxFrameHeight        = MaxDecHeight;
    pDspRes->DecodeResource.MaxBitRate            = MaxDecBrate;
    pDspRes->DecodeResource.MaxVideoPlaneWidth    = MaxDecWidth;
    pDspRes->DecodeResource.MaxVideoPlaneHeight   = MaxDecHeight;
    pDspRes->DecodeResource.MaxRatioOfGopNM       = MaxDecRatioOfGopNM;
    pDspRes->DecodeResource.BackwardTrickPlayMode = 1U;
    pDspRes->DecodeResource.MaxPicWidth           = MaxDecWidth;
    pDspRes->DecodeResource.MaxPicHeight          = MaxDecHeight;
}
