/**
 *  @file AmbaRbxMP4.c
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
 *  @details svc record box - MP4
 */

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaDef.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaUtility.h"
#include "AmbaRTC.h"
#include "AmbaGDMA.h"
#include "AmbaPrint.h"
#include "AmbaCodecCom.h"
#include "SvcIso.h"
#include "AmbaSYS.h"

#include "AmbaUtc.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaRbxInf.h"
#include "AmbaRdtInf.h"
#include "AmbaRecMaster.h"
#include "AmbaSvcGdma.h"
#include "AmbaMux.h"
#include "AmbaRscData.h"
#include "AmbaSvcWrap.h"
#include "AmbaRbxMP4.h"

#define MP4B_DBG_PRN     1
static inline void MP4B_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if MP4B_DBG_PRN
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#else
    AmbaMisra_TouchUnused(&pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static inline void MP4B_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

typedef struct {
    AMBA_RBX_STATIS_s  Statis;

    UINT32             FragFrameNum;
    UINT32             FragFrameCnt;

    UINT32             SrcEosBits;

    MP4FMT_BUF_INFO_s  MP4BufInfo;

    ULONG              BufBase[CONFIG_AMBA_REC_RBX_MP4_BUF_NUM];
    UINT32             BufSize;
    UINT32             BufIdx;

    MP4FMT_CTRL_s      FmtCtrl;

    UINT32             TextBufIdx;
    char               TextBuf[AMBA_RBXC_MP4_TEXT_BUF_SIZE][CONFIG_AMBA_REC_DRSC_TEXTLEN];

    ULONG              TLapBufBase;
    UINT32             TLapBufSize;
    ULONG              TLapWr;

    AMBA_MP4_UDTA_SVCD_BOX_s UdtaBox;

    UINT32             MehdDuration;
} AMBA_RBX_MP4_PRIV_s;

static UINT32 Invert_UInt32(UINT32 Input)
{
    UINT32 Rval = 0U, Err;
    UINT8  W[4];

    W[0] = (UINT8)((Input & 0xFF000000U) >> 24);
    W[1] = (UINT8)((Input & 0xFF0000U) >> 16);
    W[2] = (UINT8)((Input & 0xFF00U) >> 8);
    W[3] = (UINT8)(Input & 0xFFU);

    Err = AmbaWrap_memcpy(&Rval, W, sizeof(Rval));
    if (RECODER_OK != Err) {
        MP4B_DBG("AmbaWrap_memcpy Error %u", Err, 0U);
    }

    return Rval;
}

static UINT32 BufCopy(ULONG Dst, ULONG Src, UINT32 Size)
{
    UINT8   *pSrc, *pDst;
    UINT32  Rval;

    AmbaMisra_TypeCast(&pDst, &Dst);
    AmbaMisra_TypeCast(&pSrc, &Src);

#if defined(CONFIG_AMBA_REC_GDMA_USED)
    Rval = AmbaSvcGdma_Copy(pDst, pSrc, Size);
    if (RECODER_OK != Rval) {
        MP4B_DBG("GdmaCopy Error", 0U, 0U);
    }
#else
    AmbaMisra_TouchUnused(pSrc);

    Rval = AmbaWrap_memcpy(pDst, pSrc, Size);
    if (RECODER_OK != Rval) {
        MP4B_DBG("AmbaWrap_memcpy Error", 0U, 0U);
    }
#endif

    return Rval;
}

static void MP4CtrlReset(AMBA_RBX_MP4_PRIV_s *pPriv, const AMBA_RBX_USR_CFG_s *pUsrCfg)
{
    UINT32                Rval, SysTime;
    MP4FMT_CTRL_s         *pMPCtrl = &(pPriv->FmtCtrl);
    AMBA_RTC_DATE_TIME_s  CurrentTime;

    /* config mp4 header */
    Rval = AmbaRTC_GetSysTime(&CurrentTime);
    if (Rval != RECODER_OK) {
        MP4B_DBG("AmbaRTC_GetSysTime Error", 0U, 0U);
    }
    Rval = AmbaUtcGetTotalSec(&CurrentTime, &SysTime);
    if (Rval != RECODER_OK) {
        SysTime = 0U;
    }

    pMPCtrl->Mp4Hdr.SubType          = pUsrCfg->SubType;
    pMPCtrl->Mp4Hdr.CreationTime     = SysTime + UNIX_MAC_TIME_DIFF;
    pMPCtrl->Mp4Hdr.ModificationTime = SysTime + UNIX_MAC_TIME_DIFF;

    /* video */
    pMPCtrl->Mp4Hdr.VideoCfg.VideoCoding = pUsrCfg->VdCoding;
    pMPCtrl->Mp4Hdr.VideoCfg.VdWidth     = pUsrCfg->VdWidth;
    pMPCtrl->Mp4Hdr.VideoCfg.VdHeight    = pUsrCfg->VdHeight;
    pMPCtrl->Mp4Hdr.VideoCfg.TimeScale   = pUsrCfg->VdTimeScale;
    pMPCtrl->Mp4Hdr.VideoCfg.M           = pUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_M];
    pMPCtrl->Mp4Hdr.VideoCfg.N           = pUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
    pMPCtrl->Mp4Hdr.VideoCfg.IdrInterval = pUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
    pMPCtrl->Mp4Hdr.VideoCfg.NumTile     = 1U;
    pMPCtrl->Mp4Hdr.VideoCfg.NumSlice    = (UINT8)pUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_SLICE];
    pMPCtrl->Mp4Hdr.VideoCfg.Orientation = SVC_ISO_ROTATION_NONE;
    pMPCtrl->Mp4Hdr.VideoCfg.ClosedGop   = 1U;
    pMPCtrl->TrafValid[TRACK_TYPE_VIDEO] = 1U;

    if (pMPCtrl->Mp4Hdr.VideoCfg.VideoCoding == AMBA_RSC_VID_SUBTYPE_HEVC) {
        pMPCtrl->Mp4Hdr.VideoCfg.NumTile = (UINT8)pUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_TILE];
    }

    pMPCtrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO] = pUsrCfg->VdNumUnitsInTick;

#ifdef CONFIG_ENABLE_CAPTURE_TIME_MODE
    {
        UINT32 AudioClk;

        Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioClk);
        if (Rval != RECODER_OK) {
            MP4B_DBG("AmbaSYS_GetClkFreq Error", 0U, 0U);
            AudioClk = 12288000U;
        }

        pMPCtrl->Mp4Hdr.VideoCfg.CaptureTimeMode   = 1U;
        pMPCtrl->Mp4Hdr.VideoCfg.CaptureTimeScale  = AudioClk;
    }
#endif

    /* audio */
    if (pUsrCfg->AuCoding != AMBA_RSC_AUD_SUBTYPE_NONE) {
        pMPCtrl->Mp4Hdr.AudioCfg.AudioCoding           = pUsrCfg->AuCoding;
        pMPCtrl->Mp4Hdr.AudioCfg.AuSample              = pUsrCfg->AuSample;
        pMPCtrl->Mp4Hdr.AudioCfg.AuBrate               = pUsrCfg->AuBrate;
        pMPCtrl->Mp4Hdr.AudioCfg.AuChannels            = pUsrCfg->AuChannels;
        pMPCtrl->Mp4Hdr.AudioCfg.AuVolume              = pUsrCfg->AuVolume;
        pMPCtrl->Mp4Hdr.AudioCfg.AuWBitsPerSample      = pUsrCfg->AuWBitsPerSample;
        pMPCtrl->Mp4Hdr.AudioCfg.AuFormat              = pUsrCfg->AuFormat;
        pMPCtrl->TrafValid[TRACK_TYPE_AUDIO]           = 1U;
        pMPCtrl->BufInfo.SampleTicks[TRACK_TYPE_AUDIO] = pUsrCfg->AuFrameSize;
    }

    /* text */
    if (pUsrCfg->TExist != 0U) {
        pMPCtrl->TrafValid[TRACK_TYPE_DATA]            = 1U;
        pMPCtrl->BufInfo.SampleTicks[TRACK_TYPE_DATA]  = pUsrCfg->VdNumUnitsInTick;
    }

    /* udat box */
    {
        const AMBA_MP4_UDTA_SVCD_BOX_s  *pUdta;

        Rval = AmbaWrap_memset(&(pPriv->UdtaBox), 0, sizeof(AMBA_MP4_UDTA_SVCD_BOX_s));
        if (Rval != RECODER_OK) {
            MP4B_DBG("AmbaWrap_memset Error", 0U, 0U);
        }

        pPriv->UdtaBox.BoxSize   = Invert_UInt32((UINT32)sizeof(AMBA_MP4_UDTA_SVCD_BOX_s));
        pPriv->UdtaBox.BoxType   = Invert_UInt32(UDTA_BOX_TYPE_SVCD);
        if (pUsrCfg->IsEncrypt > 0U) {
            pPriv->UdtaBox.IsEncrypt = Invert_UInt32(1U);
        } else {
            pPriv->UdtaBox.IsEncrypt = Invert_UInt32(0U);
        }

        pUdta = &(pPriv->UdtaBox);

        AmbaMisra_TypeCast(&pMPCtrl->Mp4Hdr.UserData, &pUdta);
        pMPCtrl->Mp4Hdr.UserDataSize = (UINT32)sizeof(AMBA_MP4_UDTA_SVCD_BOX_s);
    }

#ifdef CONFIG_AMBA_REC_FMP4_MEHD_BOX
    {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        pMPCtrl->Mp4Hdr.MehdCtrl.Enable      = 1U;
        pMPCtrl->Mp4Hdr.MehdCtrl.TimeScale   = pMPCtrl->Mp4Hdr.VideoCfg.TimeScale;
        pMPCtrl->Mp4Hdr.MehdCtrl.DefDuration = 0;
#else
        UINT32 TempU32;
        DOUBLE TempDoub;

        TempU32  = pUsrCfg->SplitTime * 60U * pUsrCfg->VdTimeScale;
        TempU32  = GetRoundUpValU32(TempU32, pUsrCfg->VdNumUnitsInTick * pMPCtrl->Mp4Hdr.VideoCfg.N * pMPCtrl->Mp4Hdr.VideoCfg.IdrInterval);
        TempU32  = TempU32 * pMPCtrl->Mp4Hdr.VideoCfg.N * pMPCtrl->Mp4Hdr.VideoCfg.IdrInterval;
        TempDoub = (DOUBLE)TempU32 * (DOUBLE)pUsrCfg->VdNumUnitsInTick;

        pMPCtrl->Mp4Hdr.MehdCtrl.Enable      = 1U;
        pMPCtrl->Mp4Hdr.MehdCtrl.TimeScale   = pMPCtrl->Mp4Hdr.VideoCfg.TimeScale;
        pMPCtrl->Mp4Hdr.MehdCtrl.DefDuration = (UINT32)TempDoub;
#endif
    }
#endif

    pMPCtrl->pfnBufCopy = BufCopy;

    pPriv->FragFrameCnt = 0U;
    pPriv->SrcEosBits = 0U;

    Rval = AmbaWrap_memset(&(pPriv->Statis), 0, sizeof(AMBA_RBX_STATIS_s));
    if (Rval != RECODER_OK) {
        MP4B_DBG("AmbaWrap_memset Error", 0U, 0U);
    }
}

static void BufInfoUpdate(ULONG BufBase, MP4FMT_BUF_INFO_s *pBufInfo)
{
    pBufInfo->BufferBase = BufBase;
}

static UINT32 DataProc(const AMBA_REC_BOX_s *pMP4Rbx, AMBA_REC_FRWK_DESC_s *pDesc)
{
    const UINT8                    *pPrivData = pMP4Rbx->PrivData;
    UINT32                         Rval = RECODER_OK, IsFlush, DataSize, BufNum, Err;
    AMBA_RBX_MP4_PRIV_s            *pPriv;

    AMBA_DSP_ENC_PIC_RDY_s         *pVDesc;
    AMBA_RSC_TEXT_DESC_s           *pTDesc;
    const AMBA_DATG_s              *pDataG;
    AMBA_RSC_DESC_s                RscDesc;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    IsFlush = AMBA_MP4FMT_FLUSH_NONE;
    if (0U < CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS)) {
        pPriv->SrcEosBits = SetBits(pPriv->SrcEosBits, pDesc->SrcBit);
        if (pPriv->SrcEosBits == pMP4Rbx->UsrCfg.InSrcBits) {
            MP4B_DBG("recv all eos", 0U, 0U);
        }
        if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            IsFlush = AMBA_MP4FMT_FLUSH_REACH_END;
        }
    } else if (0U < CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_SPLIT)) {
        if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            IsFlush = AMBA_MP4FMT_FLUSH_REACH_LIMIT;
        }
    } else {
        if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            AmbaMisra_TypeCast(&(pVDesc), &(pDesc->pSrcData));

            if ((pVDesc->TileIdx == 0U) &&
                (pVDesc->SliceIdx == 0U) &&
                (pVDesc->FrameType == PIC_FRAME_IDR)) {
                if (pMP4Rbx->UsrCfg.SubType == AMBA_SUB_TYPE_FMP4) {
                    if (0U < pPriv->FragFrameCnt) {
                        IsFlush = AMBA_MP4FMT_FLUSH_REACH_GOP;
                        pPriv->FragFrameCnt = 0U;
                    }
                }
            }
        }
    }

    /* copy bitsdata to local buffer for time-lapse stream */
    if ((pMP4Rbx->UsrCfg.IsTimeLapse == 1U) && (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO)) {
        UINT32 CpySize;
        ULONG  StartAddr, TmpUL;

        AmbaMisra_TypeCast(&(pVDesc), &(pDesc->pSrcData));

        if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
            StartAddr = pPriv->TLapWr;

            if (((pPriv->TLapBufBase + pPriv->TLapBufSize) - pPriv->TLapWr) >= pVDesc->PicSize) {
                if ((pVDesc->StartAddr + pVDesc->PicSize) <= (pDesc->SrcBufBase + pDesc->SrcBufSize)) {
                    CpySize = pVDesc->PicSize;
                    Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr, CpySize);
                    if (Err != RECODER_OK) {
                        MP4B_NG("BufCopy failed %u", Err, 0U);
                    }
                    pPriv->TLapWr += CpySize;
                } else {
                    TmpUL   = (pDesc->SrcBufBase + pDesc->SrcBufSize) - pVDesc->StartAddr;
                    CpySize = (UINT32)TmpUL;
                    Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr, CpySize);
                    if (Err != RECODER_OK) {
                        MP4B_NG("BufCopy failed %u", Err, 0U);
                    }
                    pPriv->TLapWr += CpySize;

                    CpySize = pVDesc->PicSize - CpySize;
                    Err = BufCopy(pPriv->TLapWr, pDesc->SrcBufBase, CpySize);
                    if (Err != RECODER_OK) {
                        MP4B_NG("BufCopy failed %u", Err, 0U);
                    }
                    pPriv->TLapWr += CpySize;
                }
            } else {
                if ((pVDesc->StartAddr + pVDesc->PicSize) <= (pDesc->SrcBufBase + pDesc->SrcBufSize)) {
                    TmpUL   = ((pPriv->TLapBufBase + pPriv->TLapBufSize) - pPriv->TLapWr);
                    CpySize = (UINT32)TmpUL;
                    Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr, CpySize);
                    if (Err != RECODER_OK) {
                        MP4B_NG("BufCopy failed %u", Err, 0U);
                    }
                    pPriv->TLapWr  = pPriv->TLapBufBase;

                    Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr + CpySize, pVDesc->PicSize - CpySize);
                    if (Err != RECODER_OK) {
                        MP4B_NG("BufCopy failed %u", Err, 0U);
                    }
                    pPriv->TLapWr = pPriv->TLapWr + pVDesc->PicSize;
                    pPriv->TLapWr = pPriv->TLapWr - CpySize;
                } else {
                    UINT32 SrcRSize, DstRSize;

                    TmpUL    = (pDesc->SrcBufBase + pDesc->SrcBufSize) - pVDesc->StartAddr;
                    SrcRSize = (UINT32)TmpUL;

                    TmpUL    = (pPriv->TLapBufBase + pPriv->TLapBufSize) - pPriv->TLapWr;
                    DstRSize = (UINT32)TmpUL;
                    if (SrcRSize <= DstRSize) {
                        Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr, SrcRSize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr += SrcRSize;

                        CpySize = DstRSize - SrcRSize;
                        Err = BufCopy(pPriv->TLapWr, pDesc->SrcBufBase, CpySize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr = pPriv->TLapBufBase;

                        CpySize = (pVDesc->PicSize - SrcRSize) - CpySize;
                        Err = BufCopy(pPriv->TLapWr, (pDesc->SrcBufBase + DstRSize) - SrcRSize, CpySize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr += CpySize;
                    } else {
                        Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr, DstRSize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr = pPriv->TLapBufBase;

                        CpySize = SrcRSize - DstRSize;
                        Err = BufCopy(pPriv->TLapWr, pVDesc->StartAddr + DstRSize, CpySize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr += CpySize;

                        CpySize = (pVDesc->PicSize - DstRSize) - CpySize;
                        Err = BufCopy(pPriv->TLapWr, pDesc->SrcBufBase, CpySize);
                        if (Err != RECODER_OK) {
                            MP4B_NG("BufCopy failed %u", Err, 0U);
                        }
                        pPriv->TLapWr += CpySize;
                    }
                }
            }

            pVDesc->StartAddr = StartAddr;

            if (pPriv->TLapWr == (pPriv->TLapBufBase + pPriv->TLapBufSize)) {
                pPriv->TLapWr = pPriv->TLapBufBase;
            }
        }
    }

    if (AMBA_MP4FMT_FLUSH_NONE != IsFlush) {
        AmbaMisra_TypeCast(&(pVDesc), &(pDesc->pSrcData));

        BufInfoUpdate(pPriv->BufBase[pPriv->BufIdx], &(pPriv->FmtCtrl.BufInfo));
        RscDesc.SrcType    = pDesc->SrcType;
        RscDesc.SrcSubType = pDesc->SrcSubType;
        RscDesc.pSrcData   = pDesc->pSrcData;
        if (pMP4Rbx->UsrCfg.IsTimeLapse == 1U) {
            RscDesc.SrcBufBase = pPriv->TLapBufBase;
            RscDesc.SrcBufSize = pPriv->TLapBufSize;
        } else {
            RscDesc.SrcBufBase = pDesc->SrcBufBase;
            RscDesc.SrcBufSize = pDesc->SrcBufSize;
        }

        if (pMP4Rbx->UsrCfg.IsTimeLapse == 1U) {
            /* we need to make sure all bits data are already copied to time-lapse buffer*/
#if defined(CONFIG_AMBA_REC_GDMA_USED)
            if (RECODER_OK != AmbaGDMA_WaitAllCompletion(1000U)) {
                MP4B_DBG("Fail to wait GDMA done", 0U, 0U);
            }
#endif
        }

        Rval = AmbaMux_Proc(&(pPriv->FmtCtrl), &RscDesc, IsFlush, &DataSize);

        {
            ULONG  AlignStart, AlignSize;

            AlignStart = pPriv->FmtCtrl.BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
            AlignSize  = (((pPriv->FmtCtrl.BufInfo.BufferBase + pPriv->FmtCtrl.BufInfo.BufferSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
            Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
            if (Err != RECODER_OK) {
                MP4B_DBG("AmbaSvcWrap_CacheClean failed %u", Err, 0U);
            }
        }

#if defined(CONFIG_AMBA_REC_GDMA_USED)
        if (RECODER_OK != AmbaGDMA_WaitAllCompletion(1000U)) {
            MP4B_DBG("Fail to wait GDMA done", 0U, 0U);
        }
#endif

        if ((0U < DataSize)) {
            if ((DataSize <= pPriv->BufSize)) {
                pDesc->BoxType    = pMP4Rbx->UsrCfg.BoxType;
                pDesc->BoxSubType = pMP4Rbx->UsrCfg.SubType;

                /* fill meta buffer */
                BufNum = 0U;
                pDesc->BoxBufChain[BufNum].Base = pPriv->FmtCtrl.BufInfo.BufferBase;
                pDesc->BoxBufChain[BufNum].Size = DataSize;
                BufNum++;

                pDesc->BoxBufNum = BufNum;

                pPriv->Statis.ProcCount++;
                pPriv->Statis.ProcSize += DataSize;
            } else {
                MP4B_NG( "[stream_%d]", pVDesc->StreamId, 0U);
                MP4B_NG( "buffer overflow, (%u/%u)", pPriv->BufSize, DataSize);
            }
        }

        /* switch buffer */
        pPriv->BufIdx = (pPriv->BufIdx + 1U) % (UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM;
        BufInfoUpdate(pPriv->BufBase[pPriv->BufIdx], &(pPriv->FmtCtrl.BufInfo));

        {
            ULONG   AlignStart, AlignSize;

            AlignStart = pPriv->FmtCtrl.BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
            AlignSize  = (((pPriv->FmtCtrl.BufInfo.BufferBase + pPriv->FmtCtrl.BufInfo.BufferSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
            Err = AmbaSvcWrap_CacheInvalidate(AlignStart, AlignSize);
            if (Err != RECODER_OK) {
                MP4B_DBG("AmbaSvcWrap_CacheInvalidate failed %u", Err, 0U);
            }
        }

        /* overwrite the mehd box */
        pDesc->BoxUpdateCnt = 0U;
        if ((IsFlush == AMBA_MP4FMT_FLUSH_REACH_LIMIT) || (IsFlush == AMBA_MP4FMT_FLUSH_REACH_END)) {
            if ((pPriv->FmtCtrl.Mp4Hdr.MehdCtrl.Enable > 0U) &&
                (pPriv->FmtCtrl.Mp4Hdr.MehdCtrl.DefDuration != pPriv->FmtCtrl.Mp4Hdr.MehdCtrl.FragDuration)) {
                const UINT32 *pDuration;

                pPriv->MehdDuration = Invert_UInt32(pPriv->FmtCtrl.Mp4Hdr.MehdCtrl.FragDuration);
                pDuration           = &(pPriv->MehdDuration);

                pDesc->BoxUpdate[pDesc->BoxUpdateCnt].Offset = pPriv->FmtCtrl.Mp4Hdr.MehdCtrl.FileOffset + 12U;
                pDesc->BoxUpdate[pDesc->BoxUpdateCnt].Size   = (UINT32)sizeof(pPriv->MehdDuration);
                AmbaMisra_TypeCast(&(pDesc->BoxUpdate[pDesc->BoxUpdateCnt].Base), &(pDuration));
                pDesc->BoxUpdateCnt++;
            }
        }

        IsFlush = AMBA_MP4FMT_FLUSH_NONE;
    }

    if (CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS) == 0U) {
        if (0U < CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_SPLIT)) {
            /* nothing */
        }

        /* format data frame to text string */
        if (pDesc->SrcType == AMBA_RSC_TYPE_DATA) {
            const char *Addr;

            AmbaMisra_TypeCast(&pTDesc, &(pDesc->pSrcData));
            AmbaMisra_TypeCast(&pDataG, &(pTDesc->pRawData));

            Addr = &(pPriv->TextBuf[0][0]);
            AmbaMisra_TypeCast(&(pDesc->SrcBufBase), &Addr);
            pDesc->SrcBufSize = (UINT32)sizeof(pPriv->TextBuf);

            pTDesc->pText = &(pPriv->TextBuf[pPriv->TextBufIdx][0]);
            Err = AmbaWrap_memset(pTDesc->pText, 0, CONFIG_AMBA_REC_DRSC_TEXTLEN);
            if (Err != RECODER_OK) {
                MP4B_DBG("AmbaWrap_memset failed %u", Err, 0U);
            }

            if (pDesc->SrcSubType == AMBA_RSC_DATA_SUBTYPE_CAN) {
                pTDesc->TexLen = AmbaRscData_CanToText(pTDesc->pText, pDataG);
            } else if (pDesc->SrcSubType == AMBA_RSC_DATA_SUBTYPE_PTP) {
                pTDesc->TexLen = AmbaRscData_PtpToText(pTDesc->pText, pDataG);
            } else {
                pTDesc->TexLen = 0U;
            }

            /* update text buf idx */
            pPriv->TextBufIdx++;
            if (pPriv->TextBufIdx >= AMBA_RBXC_MP4_TEXT_BUF_SIZE) {
                pPriv->TextBufIdx = 0U;
            }
        }

        /* process frame */
        BufInfoUpdate(pPriv->BufBase[pPriv->BufIdx], &(pPriv->FmtCtrl.BufInfo));
        RscDesc.SrcType    = pDesc->SrcType;
        RscDesc.SrcSubType = pDesc->SrcSubType;
        RscDesc.pSrcData   = pDesc->pSrcData;
        if ((pMP4Rbx->UsrCfg.IsTimeLapse == 1U) && (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO)) {
            RscDesc.SrcBufBase = pPriv->TLapBufBase;
            RscDesc.SrcBufSize = pPriv->TLapBufSize;
        } else {
            RscDesc.SrcBufBase = pDesc->SrcBufBase;
            RscDesc.SrcBufSize = pDesc->SrcBufSize;
        }
        if (0U < AmbaMux_Proc(&(pPriv->FmtCtrl), &RscDesc, IsFlush, &DataSize)) {
            MP4B_NG( "something wrong", 0U, 0U);
        }

        if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            AmbaMisra_TypeCast(&(pVDesc), &(pDesc->pSrcData));

            if ((pVDesc->TileIdx == (pVDesc->NumTile - 1U)) &&
                (pVDesc->SliceIdx == (pVDesc->NumSlice - 1U))) {
                if (pMP4Rbx->UsrCfg.SubType == AMBA_SUB_TYPE_FMP4) {
                    pPriv->FragFrameCnt++;
                }
            }
        }
    }

    return Rval;
}

static UINT32 CalcFragMemSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, MP4FMT_BUF_INFO_s *pBufInfo)
{
    #define SVC_ELEM_BRATE_TOLERANCE            (50U)    /*  50% */
    #define SVC_ELEM_BRATE_HGOP_TOLERANCE       (100U)   /* 100% */

    UINT32  Err, BufferSize, Rval = 0U;
    UINT64  TempSize, Extra;

    if (0U < pEval->IsHierGop) {
        Extra = (UINT64)SVC_ELEM_BRATE_HGOP_TOLERANCE;
    } else {
        Extra = (UINT64)SVC_ELEM_BRATE_TOLERANCE;
    }


    Err = AmbaMux_EvalFragSize(SubType, pEval, pBufInfo);

    if (RECODER_OK != Err) {
        MP4B_NG( "AmbaMux_EvalFragSize failed %u", Err, 0U);
    } else {
        TempSize   = ((UINT64)pBufInfo->BufferSize * (Extra + 100U)) / 100U;

        BufferSize = (UINT32)TempSize;
        BufferSize = GetRoundUpValU32(BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

        pBufInfo->BufferSize = BufferSize;
        Rval                 = BufferSize;
    }

    return Rval;
}

/**
* create svc record box
* @param [in]  pMP4Rbx pointer of mp4 record box information
* @return ErrorCode
*/
UINT32 AmbaRbxMP4_Create(AMBA_REC_BOX_s *pMP4Rbx)
{
    static UINT32  RbxMP4Priv = (UINT32)sizeof(AMBA_RBX_MP4_PRIV_s);

    const UINT8         *pPrivData = pMP4Rbx->PrivData;
    UINT32              i, Rval = RECODER_ERROR_GENERAL_ERROR, Reserved = CONFIG_AMBA_REC_RBX_PRIV_SIZE;
    UINT32              NeedSize, FragSize, N, IdrTr, DescSize = 0U, Err;
    ULONG               Base;
    AMBA_RBX_MP4_PRIV_s *pPriv;
    MP4FMT_BUF_INFO_s   BufInfo = {0};
    MP4FMT_CTRL_s       *pMPCtrl;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    pMPCtrl = &(pPriv->FmtCtrl);

    Err = AmbaWrap_memset(pMPCtrl, 0, sizeof(MP4FMT_CTRL_s));
    if (Err != RECODER_OK) {
        MP4B_NG("AmbaWrap_memset failed %u", Err, 0U);
    }

    if (RbxMP4Priv <= Reserved) {
        FragSize = CalcFragMemSize(pMP4Rbx->UsrCfg.SubType,
                                  &(pMP4Rbx->UsrCfg.MiaInfo),
                                  &BufInfo);

        Err = AmbaMux_EvalDescSize(pMP4Rbx->UsrCfg.SubType, &(pMP4Rbx->UsrCfg.MiaInfo), &DescSize);
        if (Err != RECODER_OK) {
            MP4B_DBG("AmbaMux_EvalDescSize failed %u", Err, 0U);
        }

        NeedSize  = FragSize + DescSize;
        NeedSize  = NeedSize * (UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM;
        if (pMP4Rbx->UsrCfg.IsTimeLapse == 1U) {
            NeedSize += FragSize;
        }

        if (NeedSize <= pMP4Rbx->UsrCfg.MemSize) {
            /* hook function pointer */
            pMP4Rbx->pfnProc = DataProc;

            /* reset parameters */
            Err = AmbaWrap_memset(pPriv, 0, sizeof(AMBA_RBX_MP4_PRIV_s));
            if (Err != RECODER_OK) {
                MP4B_DBG("AmbaWrap_memset failed %u", Err, 0U);
            }

            Base = pMP4Rbx->UsrCfg.MemBase;

            if (pMP4Rbx->UsrCfg.IsTimeLapse == 1U) {
                pPriv->TLapBufBase  = Base;
                pPriv->TLapBufSize  = FragSize;
                pPriv->TLapWr       = pPriv->TLapBufBase;

                Base += pPriv->TLapBufSize;
            }

            /* calc frag number */
            N = pMP4Rbx->UsrCfg.MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
            IdrTr = pMP4Rbx->UsrCfg.MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
            pPriv->FragFrameNum = N * IdrTr;

            /* reset buffer info */
            pPriv->BufIdx  = 0U;
            pPriv->BufSize = FragSize;

            MP4B_DBG(" frag size(%u)", FragSize, 0);

            for (i = 0U; i < (UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM; i++) {
                pPriv->BufBase[i] = Base;
                Base += FragSize;
            }

            pPriv->FmtCtrl.BufInfo.BufferSize = FragSize;
            pPriv->FmtCtrl.BufInfo.BufferBase = pPriv->BufBase[0];

            pPriv->FmtCtrl.DescBase = Base;
            pPriv->FmtCtrl.DescSize = DescSize;

            Rval = RECODER_OK;
        } else {
            MP4B_NG( "MemSize not enough, (%u/%u)", pMP4Rbx->UsrCfg.MemSize, NeedSize);
        }
    } else {
        MP4B_NG( "fail to create mp4 box, (%u/%u)", RbxMP4Priv, Reserved);
    }

    return Rval;

}

/**
* destroy svc record box
* @param [in]  pMP4Rbx pointer of mp4 record box information
* @return ErrorCode
*/
UINT32 AmbaRbxMP4_Destroy(const AMBA_REC_BOX_s *pMP4Rbx)
{
    const UINT8               *pPrivData = pMP4Rbx->PrivData;
    UINT32                     Rval = RECODER_OK;
    const AMBA_RBX_MP4_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    MP4B_DBG("rbx is destroyed, ProcCount/ProcSize(%X/%X)", (UINT32)pPriv->Statis.ProcCount,
                                                            (UINT32)pPriv->Statis.ProcSize);

    return Rval;
}

/**
* Control Function for Svc Mp4 Rec Box
* @param [in]  pMP4Rbx pointer of mp4 record box information
* @param [in]  CtrlType control type
* @param [in]  pParam pointer of control parameter
*/
void AmbaRbxMP4_Control(AMBA_REC_BOX_s *pMP4Rbx, UINT32 CtrlType, void *pParam)
{
    const UINT8         *pPrivData = pMP4Rbx->PrivData;
    UINT32              Rval, Err;
    const UINT32        *pNewVal;
    AMBA_RBX_MP4_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    switch (CtrlType) {
    case AMBA_RBXC_MP4_STATIS_GET:
        Rval = AmbaWrap_memcpy(pParam, &(pPriv->Statis), sizeof(AMBA_RBX_STATIS_s));
        if (Rval != RECODER_OK) {
            MP4B_DBG("AmbaWrap_memcpy failed %u", Rval, 0U);
        }
        break;
    case AMBA_RBXC_MP4_VDCODING:
        AmbaMisra_TypeCast(&pNewVal, &(pParam));
        pPriv->FmtCtrl.Mp4Hdr.VideoCfg.VideoCoding = (*pNewVal);
        pMP4Rbx->UsrCfg.VdCoding = (*pNewVal);
        break;
    case AMBA_RBXC_MP4_AUCODING:
        AmbaMisra_TypeCast(&pNewVal, &(pParam));
        if (pMP4Rbx->UsrCfg.AuCoding != AMBA_RSC_AUD_SUBTYPE_NONE) {
            pPriv->FmtCtrl.Mp4Hdr.AudioCfg.AudioCoding = (*pNewVal);
            pMP4Rbx->UsrCfg.AuCoding = (*pNewVal);
        }
        break;
    case AMBA_RBXC_MP4_HEADRESET:
        /* reset mp4 header */
        MP4CtrlReset(pPriv, &(pMP4Rbx->UsrCfg));
        {
            ULONG  AlignStart, AlignSize;

            AlignStart = pPriv->FmtCtrl.BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
            AlignSize  = (((pPriv->FmtCtrl.BufInfo.BufferBase + pPriv->FmtCtrl.BufInfo.BufferSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
            Err = AmbaSvcWrap_CacheInvalidate(AlignStart, AlignSize);
            if (Err != RECODER_OK) {
                MP4B_DBG("AmbaSvcWrap_CacheInvalidate failed %u", Err, 0U);
            }

            /* configure init delay */
            {
                const UINT32 InitDelay[TRACK_TYPE_MAX] = {0};

                if (AmbaMux_SetInitDelay(&(pPriv->FmtCtrl), InitDelay) != OK) {
                    MP4B_NG( "fail to configure InitDelay", 0U, 0U);
                }
            }
        }
        break;
    default:
        /* do nothing */
        break;
    }
}

/**
* eval buffer size for svc record box
* @param [in]  SubType record box subtype
* @param [in]  pEval pointer of record memory size evaluation information
* @param [in]  pSize pointer of evaluated memory size
*/
void AmbaRbxMP4_EvalMemSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *pSize)
{
    UINT32             MemSize = 0U, DescSize = 0U, FragSize, Err;
    MP4FMT_BUF_INFO_s  BufInfo = {0};

    if (SubType == AMBA_SUB_TYPE_FMP4) {
        /* fragment mp4 evaluation */
        FragSize  = CalcFragMemSize(SubType, pEval, &BufInfo);
        MemSize += FragSize;

        Err = AmbaMux_EvalDescSize(SubType, pEval, &DescSize);
        if (Err != RECODER_OK) {
            MP4B_DBG("AmbaSvcWrap_CacheInvalidate failed %u", Err, 0U);
        }
        MemSize += DescSize;

        /* two buffer */
        *pSize = MemSize * (UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM;

        if (pEval->IsTimeLapse == 1U) {
            /* timelapse record need extra buffer to store frag data */
            *pSize = *pSize + FragSize;
        }
        MP4B_DBG("eval. mp4box memory size %u", *pSize, 0U);
    } else {
        /* do nothing */
    }
}

