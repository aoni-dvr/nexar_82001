/**
 *  @file AmbaRdtEmgEvt.c
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
 *  @details svc record destination - emergency event
 */

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaDef.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaUtility.h"
#include "AmbaAudio_AENC.h"
#include "AmbaGDMA.h"
#include "AmbaRTC.h"
#include "AmbaPrint.h"
#include "AmbaCodecCom.h"
#include "AmbaSYS.h"

#include "AmbaSvcGdma.h"
#include "AmbaVfs.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaRscData.h"
#include "AmbaRbxInf.h"
#include "AmbaRdtInf.h"
#include "AmbaRecMaster.h"
#include "AmbaMux.h"
#include "AmbaRbxMP4.h"
#include "AmbaSvcWrap.h"
#include "AmbaRdtEmgEvt.h"
#include "AmbaUtc.h"
#include "SvcIso.h"

#define SVC_EMGEVT_MAX_STREAM_NUM    (4U)

#define SVC_EMGEVT_DTYPE_DATA        (0U)   /* source data */
#define SVC_EMGEVT_DTYPE_DESC        (1U)   /* source descriptor */
#define SVC_EMGEVT_DTYPE_MAX_NUM     (2U)

#define SVC_EMGEVT_FLAG_EVENT_START  (1U)   /* event happens */
#define SVC_EMGEVT_FLAG_DATA_READY   (2U)   /* data(next) is ready */

#define SVC_EMGEVT_STACK_SIZE        (0x8000U)

static inline void SVC_EMG_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void SVC_EMG_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void SvcUtil_BitsToArr(UINT32 MaxNum, UINT32 Bits, UINT32 *pNum, UINT16 *pArr)
{
    UINT32  i, Num = 0;

    for (i = 0; i < MaxNum; i++) {
        if (0U < (Bits & ((UINT32)0x01U << i))) {
            pArr[Num] = (UINT16)i;

            Num += 1U;
        }
    }
    *pNum = Num;
}

typedef struct {
    UINT32                 IsCreat;
    UINT8                  TaskStack[SVC_EMGEVT_STACK_SIZE];
    AMBA_KAL_TASK_t        TaskCtrl;
    AMBA_KAL_EVENT_FLAG_t  TaskFlag;
} AMBA_RDT_EMG_TASK_s;

typedef struct {
    AMBA_RDT_EMG_TASK_s  TaskInfo;
    UINT32              IsRunning;
    UINT32              MaxRecPeriod;   /* in second */
    AMBA_REC_EVAL_s     *pRecEval;
    AMBA_REC_DST_s      *pRdtCtrl;

    ULONG               FrameBufBase;
    UINT32              FrameBufSize;
    ULONG               FrameWritePtr;
    UINT32              FrameWSize;
    UINT32              FrameRSize;
    UINT32              FrameRSizeTmp;
    ULONG               DescBufBase;
    UINT32              DescBufSize;
    ULONG               DescWritePtr;
    ULONG               DescReadPtr;
    UINT32              FramePerFrag;
    UINT32              DescCount;
    UINT32              IdrCount;

    UINT32              EventStart;
    UINT32              PrevSec;
    UINT32              NextSec;
    UINT32              IdrTotalNum;
    UINT32              IdrPrevNum;
    UINT32              NextStart;

    MP4FMT_CTRL_s       MP4Ctrl;
    MP4FMT_BUF_INFO_s   MP4BufInfo;
    UINT32              FragSize;
    UINT32              WriteSize;

    UINT32              IsFifoOpen;
    AMBA_VFS_FILE_s     File;
    char                FileName[AMBA_REC_MAX_FILE_NAME_LEN];
    UINT32              FileSyncSize;
    UINT32              FileUnsyncSize;

    UINT32              AllSrcBits;
    UINT32              FlushBits;
    ULONG               SrcReadPtr[AMBA_REC_MIA_MAX];
    UINT32              FrameNum[AMBA_REC_MIA_MAX];
    UINT32              FrameThr[AMBA_REC_MIA_MAX];
    UINT32              LastDescIdx[AMBA_REC_MIA_MAX];
} AMBA_RDT_EMG_CTRL_s;

typedef struct {
    UINT32               IsValid;
    UINT32               IsIdr;
    UINT32               DescNumber;
    AMBA_REC_FRWK_DESC_s  SrcDesc;
} AMBA_RDT_EMG_DESC_s;


static AMBA_RDT_EMG_CTRL_s  EmgCtrl[SVC_EMGEVT_MAX_STREAM_NUM] GNU_SECTION_NOZEROINIT;

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
        SVC_EMG_NG("AmbaWrap_memcpy Error %u", Err, 0U);
    }

    return Rval;
}

static UINT32 MemCopy(ULONG Dst, ULONG Src, UINT32 Size)
{
    UINT8   *pSrc, *pDst;
    UINT32  Rval;

    AmbaMisra_TypeCast(&pDst, &Dst);
    AmbaMisra_TypeCast(&pSrc, &Src);

#if defined(CONFIG_AMBA_REC_GDMA_USED)
    Rval = AmbaSvcGdma_Copy(pDst, pSrc, Size);
    if (RECODER_OK != Rval) {
        SVC_EMG_NG("GdmaCopy Error", 0U, 0U);
    }
#else
    AmbaMisra_TouchUnused(pSrc);

    Rval = AmbaWrap_memcpy(pDst, pSrc, Size);
    if (RECODER_OK != Rval) {
        SVC_EMG_NG("AmbaWrap_memcpy Error", 0U, 0U);
    }
#endif

    return Rval;
}

static void MemWait(const AMBA_RDT_EMG_CTRL_s *pCtrl, UINT32 Type)
{
#define WAIT_FRAME_BUF   (0U)
#define WAIT_FRAG_BUF    (1U)
    UINT32 Err;

    if (Type <= WAIT_FRAG_BUF) {
        ULONG   AlignStart, AlignSize;

        if (Type == WAIT_FRAME_BUF) {
            AlignStart = pCtrl->FrameBufBase & AMBA_CACHE_LINE_MASK;
            AlignSize  = pCtrl->FrameBufSize & AMBA_CACHE_LINE_MASK;
        } else {
            AlignStart = pCtrl->MP4BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
            AlignSize  = pCtrl->FragSize & AMBA_CACHE_LINE_MASK;
        }
        Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaSvcWrap_CacheClean failed %u ", Err, 0U);
        }
    }
#if defined(CONFIG_AMBA_REC_GDMA_USED)
    if (GDMA_ERR_NONE != AmbaGDMA_WaitAllCompletion(1000U)) {
        SVC_EMG_NG("Fail to wait GDMA done", 0U, 0U);
    }
#endif
}

static ULONG CopyFunc(ULONG DstAddr, ULONG SrcAddr, UINT32 Size, ULONG DstBufBase, UINT32 DstBufSize)
{
    UINT32 Err;
    ULONG  Addr, WriteAddr, CacheAddr, CacheSize, CpySize, TmpUL;

    if ((DstAddr + Size) <= (DstBufBase + DstBufSize)) {
        CacheAddr = DstAddr & AMBA_CACHE_LINE_MASK;
        CacheSize = GetRoundUpValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE) * AMBA_CACHE_LINE_SIZE;
        Err       = AmbaSvcWrap_CacheInvalidate(CacheAddr, CacheSize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaSvcWrap_CacheInvalidate failed %u ", Err, 0U);
        }

        Err = MemCopy(DstAddr, SrcAddr, Size);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("MemCopy failed %u ", Err, 0U);
        }
        WriteAddr = DstAddr + Size;
    } else {
        CacheAddr = DstAddr & AMBA_CACHE_LINE_MASK;
        TmpUL     = (DstBufBase + DstBufSize) - DstAddr;
        CacheSize = GetRoundUpValU32((UINT32)TmpUL, (UINT32)AMBA_CACHE_LINE_SIZE) * AMBA_CACHE_LINE_SIZE;
        Err       = AmbaSvcWrap_CacheInvalidate(CacheAddr, CacheSize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaSvcWrap_CacheInvalidate failed %u ", Err, 0U);
        }

        CpySize = (DstBufBase + DstBufSize) - DstAddr;
        Err     = MemCopy(DstAddr, SrcAddr, (UINT32)CpySize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("MemCopy failed %u ", Err, 0U);
        }

        CacheAddr = DstBufBase & AMBA_CACHE_LINE_MASK;
        TmpUL     = (ULONG)Size - ((DstBufBase + DstBufSize) - DstAddr);
        CacheSize = GetRoundUpValU32((UINT32)TmpUL, (UINT32)AMBA_CACHE_LINE_SIZE) * AMBA_CACHE_LINE_SIZE;
        Err       = AmbaSvcWrap_CacheInvalidate(CacheAddr, CacheSize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaSvcWrap_CacheInvalidate failed %u ", Err, 0U);
        }

        Addr    = (SrcAddr + DstBufBase + DstBufSize) - DstAddr;
        CpySize = Size - (DstBufBase + DstBufSize - DstAddr);
        Err     = MemCopy(DstBufBase, Addr, (UINT32)CpySize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("MemCopy failed %u ", Err, 0U);
        }
        WriteAddr = Size + DstAddr - DstBufSize;
    }

    if (WriteAddr == (DstBufBase + DstBufSize)) {
        WriteAddr = DstBufBase;
    }

    return WriteAddr;
}

static UINT32 CheckAddr(ULONG *Addr, ULONG BufBase, UINT32 BufSize, UINT32 AlignBase)
{
    ULONG  Diff;
    UINT32 Rval = RECODER_OK, Padding;

    Padding = ((UINT32)*Addr % AlignBase);

    if (Padding > 0U) {
        Padding =  AlignBase - Padding;
        *Addr   += Padding;
    }

    if (*Addr < BufBase) {
        Diff  = BufBase - *Addr;
        if (Diff < (ULONG)BufSize) {
            *Addr = (BufBase + BufSize) - Diff;
        } else {
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (*Addr >= (BufBase + BufSize)) {
        Diff  = *Addr - (BufBase + BufSize);
        if (Diff < (ULONG)BufSize) {
            *Addr = BufBase + Diff;
        } else {
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

static UINT32 DataStore(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pSrcDesc)
{
    UINT32                         Err, Rval = RECODER_OK;
    ULONG                          WriteAddr, Left;
    UINT32                         Type, Done, SubType = 0U, IsIdr = 0U, Break = 0U;
    UINT32                         SrcBufSize = 0U, SrcSize = 0U, DstBufSize = 0U;
    ULONG                          SrcBufBase = 0U, SrcStart = 0U, DstBufBase = 0U, DstStart = 0U;
    AMBA_RDT_EMG_CTRL_s            *pCtrl;
    AMBA_DSP_ENC_PIC_RDY_s         VDesc = {0};
    const AMBA_DSP_ENC_PIC_RDY_s   *pVideoDesc;
    AMBA_AENC_AUDIO_DESC_s         ADesc = {0};
    const AMBA_AENC_AUDIO_DESC_s   *pAudioDesc;
    AMBA_RSC_TEXT_DESC_s           TDesc;
    AMBA_RSC_TEXT_DESC_s           *pTDesc;
    AMBA_RDT_EMG_DESC_s            SDecs;
    const AMBA_RDT_EMG_DESC_s      *pSDesc;
    void                           *pDataDesc = NULL;
    char                           TextBuf[CONFIG_AMBA_REC_DRSC_TEXTLEN];
    const char                     *pTextBuf;

    AmbaMisra_TouchUnused(&Rval);
    pCtrl = &(EmgCtrl[pRdtCtrl->UsrCfg.StreamID]);

    if ((Rval == RECODER_OK) && (pCtrl->IsRunning == 1U)) {
        if ((0U < CheckBits(pRdtCtrl->UsrCfg.InSrcBits, pSrcDesc->SrcBit))) {
            Type = SVC_EMGEVT_DTYPE_DATA;
            while (Type < SVC_EMGEVT_DTYPE_MAX_NUM) {
                Done = 0U;
                if (Type == SVC_EMGEVT_DTYPE_DATA) {
                    if (pSrcDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {    /* video */
                        if (SubType == 0U) {    /* store video descriptor AMBA_DSP_ENC_PIC_RDY_s */
                            /* copy to local structure first */
                            Err = AmbaWrap_memcpy(&VDesc, pSrcDesc->pSrcData, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
                            if (RECODER_OK != Err) {
                                SVC_EMG_NG("AmbaWrap_memcpy failed(%u)", Err, 0U);
                            }

                            /* break the loop if it's eos */
                            if (VDesc.PicSize == AMBA_DSP_ENC_END_MARK) {
                                Break = 1U;
                            }

                            /* update video descriptor info (update pointer to video data) */
                            if ((pCtrl->FrameWritePtr + sizeof(AMBA_DSP_ENC_PIC_RDY_s)) >= (pCtrl->FrameBufBase + pCtrl->FrameBufSize)) {
                                pCtrl->FrameWritePtr = pCtrl->FrameBufBase;
                            }
                            VDesc.StartAddr = pCtrl->FrameWritePtr + sizeof(AMBA_DSP_ENC_PIC_RDY_s);
                            Err = CheckAddr(&(VDesc.StartAddr), pCtrl->FrameBufBase, pCtrl->FrameBufSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                            if (Err != RECODER_OK) {
                                SVC_EMG_NG("CheckAddr failed(%u)", Err, 0U);
                            }

                            pVideoDesc = &VDesc;
                            AmbaMisra_TypeCast(&SrcBufBase, &pVideoDesc);
                            SrcBufSize  = (UINT32)sizeof(AMBA_DSP_ENC_PIC_RDY_s);
                            SrcStart    = SrcBufBase;
                            SrcSize     = SrcBufSize;

                            /* record the video descriptor start address for source descriptor */
                            AmbaMisra_TypeCast(&pDataDesc, &(pCtrl->FrameWritePtr));

                            /* if it's IDR frame */
                            if ((VDesc.TileIdx == 0U) &&
                                (VDesc.SliceIdx == 0U) &&
                                (VDesc.FrameType == PIC_FRAME_IDR)) {
                                IsIdr = 1U;
                                pCtrl->IdrCount++;
                            }
                        } else {
                            /* store video frame data */
                            AmbaMisra_TypeCast(&pVideoDesc, &(pSrcDesc->pSrcData));
                            SrcBufBase  = pSrcDesc->SrcBufBase;
                            SrcBufSize  = pSrcDesc->SrcBufSize;
                            SrcStart    = pVideoDesc->StartAddr;
                            SrcSize     = pVideoDesc->PicSize;
                            Done = 1U;
                        }
                    } else if (pSrcDesc->SrcType == AMBA_RSC_TYPE_AUDIO) {    /* audio */
                        if (SubType == 0U) {
                            ULONG Addr;

                            /* copy to local structure first */
                            Err = AmbaWrap_memcpy(&ADesc, pSrcDesc->pSrcData, sizeof(AMBA_AENC_AUDIO_DESC_s));
                            if (RECODER_OK != Err) {
                                SVC_EMG_NG("AmbaWrap_memcpy failed(%u)", Err, 0U);
                            }

                            /* break the loop if it's eos */
                            if (ADesc.Eos != 0U) {
                                Break = 1U;
                            }

                            /* update audio descriptor info (update pointer to audio data) */
                            if ((pCtrl->FrameWritePtr + sizeof(AMBA_AENC_AUDIO_DESC_s)) >= (pCtrl->FrameBufBase + pCtrl->FrameBufSize)) {
                                pCtrl->FrameWritePtr = pCtrl->FrameBufBase;
                            }
                            Addr = pCtrl->FrameWritePtr + sizeof(AMBA_AENC_AUDIO_DESC_s);
                            Err = CheckAddr(&Addr, pCtrl->FrameBufBase, pCtrl->FrameBufSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                            if (Err != RECODER_OK) {
                                SVC_EMG_NG("CheckAddr failed(%u)", Err, 0U);
                            }
                            AmbaMisra_TypeCast(&(ADesc.pBufAddr), &Addr);

                            pAudioDesc  = &ADesc;
                            AmbaMisra_TypeCast(&SrcBufBase, &pAudioDesc);
                            SrcBufSize  = (UINT32)sizeof(AMBA_AENC_AUDIO_DESC_s);
                            SrcStart    = SrcBufBase;
                            SrcSize     = SrcBufSize;

                            /* record the video descriptor start address for source descriptor */
                            AmbaMisra_TypeCast(&pDataDesc, &(pCtrl->FrameWritePtr));
                        } else {
                            /* store audio frame data */
                            AmbaMisra_TypeCast(&pAudioDesc, &(pSrcDesc->pSrcData));
                            SrcBufBase  = pSrcDesc->SrcBufBase;
                            SrcBufSize  = pSrcDesc->SrcBufSize;
                            AmbaMisra_TypeCast(&SrcStart, &(pAudioDesc->pBufAddr));
                            SrcSize     = pAudioDesc->DataSize;
                            Done = 1U;
                        }
                    } else if (pSrcDesc->SrcType == AMBA_RSC_TYPE_DATA) {    /* text */
                        ULONG                       Addr;
                        const AMBA_DATG_s           *pDataG;

                        if (SubType == 0U) {
                            Err = AmbaWrap_memcpy(&TDesc, pSrcDesc->pSrcData, sizeof(AMBA_RSC_TEXT_DESC_s));
                            if (RECODER_OK != Err) {
                                SVC_EMG_NG("AmbaWrap_memcpy failed(%u)", Err, 0U);
                            }

                            pTDesc = &TDesc;
                            AmbaMisra_TypeCast(&pDataG, &(TDesc.pRawData));

                            if ((pCtrl->FrameWritePtr + sizeof(AMBA_RSC_TEXT_DESC_s)) >= (pCtrl->FrameBufBase + pCtrl->FrameBufSize)) {
                                pCtrl->FrameWritePtr = pCtrl->FrameBufBase;
                            }
                            Addr = pCtrl->FrameWritePtr + sizeof(AMBA_RSC_TEXT_DESC_s);
                            Err = CheckAddr(&Addr, pCtrl->FrameBufBase, pCtrl->FrameBufSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                            if (Err != RECODER_OK) {
                                SVC_EMG_NG("CheckAddr failed(%u)", Err, 0U);
                            }

                            /* transfer DataG to text */
                            AmbaMisra_TypeCast(&(pTDesc->pText), &Addr);

                            Err = AmbaWrap_memset(TextBuf, 0, CONFIG_AMBA_REC_DRSC_TEXTLEN);
                            if (Err != RECODER_OK) {
                                SVC_EMG_NG("AmbaWrap_memset failed(%u)", Err, 0U);
                            }
                            if (pSrcDesc->SrcSubType == AMBA_RSC_DATA_SUBTYPE_CAN) {
                                pTDesc->TexLen = AmbaRscData_CanToText(TextBuf, pDataG);
                            } else if (pSrcDesc->SrcSubType == AMBA_RSC_DATA_SUBTYPE_PTP) {
                                pTDesc->TexLen = AmbaRscData_PtpToText(TextBuf, pDataG);
                            } else {
                                SVC_EMG_NG("Unknown text type (%u)", pSrcDesc->SrcSubType, 0U);
                            }

                            AmbaMisra_TypeCast(&SrcBufBase, &pTDesc);
                            SrcBufSize  = (UINT32)sizeof(AMBA_RSC_TEXT_DESC_s);
                            SrcStart    = SrcBufBase;
                            SrcSize     = SrcBufSize;

                            AmbaMisra_TypeCast(&pDataDesc, &(pCtrl->FrameWritePtr));
                        } else {
                            pTextBuf = &(TextBuf[0]);
                            AmbaMisra_TypeCast(&SrcBufBase, &pTextBuf);
                            SrcBufSize  = (UINT32)sizeof(TextBuf);
                            SrcStart    = SrcBufBase;
                            SrcSize     = SrcBufSize;
                            Done        = 1U;
                        }
                    } else {
                        SVC_EMG_NG("Unknown data type(%u)", pSrcDesc->SrcType, 0U);
                        Break = 1U;
                    }
                    DstBufBase = pCtrl->FrameBufBase;
                    DstBufSize = pCtrl->FrameBufSize;
                    DstStart   = pCtrl->FrameWritePtr;
                } else if (Type == SVC_EMGEVT_DTYPE_DESC) {    /* source descriptor */
                    /* copy to local structure first */
                    Err = AmbaWrap_memcpy(&(SDecs.SrcDesc), pSrcDesc, sizeof(AMBA_REC_FRWK_DESC_s));
                    if (RECODER_OK != Err) {
                        SVC_EMG_NG("AmbaWrap_memcpy failed(%u)", Err, 0U);
                    }

                    /* update source descriptor info*/
                    SDecs.SrcDesc.pSrcData   = pDataDesc;
                    SDecs.DescNumber         = pCtrl->DescCount;
                    SDecs.IsIdr              = IsIdr;
                    SDecs.IsValid            = 1U;
                    SDecs.SrcDesc.SrcBufBase = pCtrl->FrameBufBase;
                    SDecs.SrcDesc.SrcBufSize = pCtrl->FrameBufSize;

                    pSDesc = &SDecs;
                    AmbaMisra_TypeCast(&SrcBufBase, &pSDesc);
                    SrcBufSize  = (UINT32)sizeof(AMBA_RDT_EMG_DESC_s);
                    SrcStart    = SrcBufBase;
                    SrcSize     = SrcBufSize;

                    DstBufBase = pCtrl->DescBufBase;
                    DstBufSize = pCtrl->DescBufSize;
                    DstStart   = pCtrl->DescWritePtr;
                    Done = 1U;

                    pCtrl->DescCount++;
                } else {
                    /* nothing */
                }

                if (Break == 1U) {
                    break;
                }

                /* update write size */
                if ((pCtrl->EventStart == 1U) && (Type == SVC_EMGEVT_DTYPE_DATA)) {
                    pCtrl->FrameWSize += GetRoundUpValU32(SrcSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                }

                /* copy data to buffer */
                if ((SrcStart + SrcSize) <= (SrcBufBase + SrcBufSize)) {
                    WriteAddr = CopyFunc(DstStart, SrcStart, SrcSize, DstBufBase, DstBufSize);
                } else {
                    Left = SrcSize - (SrcBufBase + SrcBufSize - SrcStart);
                    WriteAddr = CopyFunc(DstStart, SrcStart, SrcSize - (UINT32)Left, DstBufBase, DstBufSize);
                    WriteAddr = CopyFunc(WriteAddr, SrcBufBase, (UINT32)Left, DstBufBase, DstBufSize);
                }

                /* update write pointer */
                if (Type == SVC_EMGEVT_DTYPE_DATA) {
                    /* every data start address should align 64 */
                    Err = CheckAddr(&WriteAddr, pCtrl->FrameBufBase, pCtrl->FrameBufSize, (UINT32)AMBA_CACHE_LINE_SIZE);
                    if (Err != RECODER_OK) {
                        SVC_EMG_NG("CheckAddr failed(%u)", Err, 0U);
                    }
                    pCtrl->FrameWritePtr = WriteAddr;
                } else {
                    pCtrl->DescWritePtr  = WriteAddr;
                }

                if (Done == 1U) {
                    Type++;
                }

                SubType++;
            }

            /* notify the task to write the fragment to file */
            if ((pCtrl->EventStart == 1U) && (pCtrl->NextStart == 1U)) {
                UINT32 ReadNum, WriteNum;

                AmbaMisra_TypeCast(&pSDesc, &(pCtrl->DescReadPtr));
                ReadNum  = pSDesc->DescNumber;

                WriteAddr = pCtrl->DescWritePtr - sizeof(AMBA_RDT_EMG_DESC_s);
                AmbaMisra_TypeCast(&pSDesc, &WriteAddr);
                WriteNum = pSDesc->DescNumber;

                /* only notify when the (write - read) frame number is larger than 1 fragment frames */
                if ((ReadNum + pCtrl->FramePerFrag) < WriteNum) {
                    Err = AmbaKAL_EventFlagSet(&(pCtrl->TaskInfo.TaskFlag), SVC_EMGEVT_FLAG_DATA_READY);
                    if (Err != KAL_ERR_NONE) {
                        SVC_EMG_NG("AmbaKAL_EventFlagSet failed(%u)", Err, 0U);
                        Rval = RECODER_ERROR_GENERAL_ERROR;
                    }
                }
            }
        }
    }

    return Rval;
}

static UINT32 SearchStartReadPtr(AMBA_RDT_EMG_CTRL_s *pCtrl)
{
    UINT32                     Err, Rval = RECODER_OK, FrameCount = 0U, i;
    const AMBA_RDT_EMG_DESC_s   *pDecs;

    AmbaMisra_TouchUnused(&Rval);

    /* search from the (writing frame number - (PrevSec * Total FrameRate)) */
    if (Rval == RECODER_OK) {
        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
            if (0U < CheckBits(pCtrl->pRdtCtrl->UsrCfg.InSrcBits, ((UINT32)1U << i))) {
                FrameCount += pCtrl->pRdtCtrl->UsrCfg.RecEval.MiaCfg[i].FrameRate;
            }
        }

        FrameCount *= pCtrl->PrevSec;

        pCtrl->DescReadPtr = pCtrl->DescWritePtr - (((ULONG)FrameCount + 1UL) * sizeof(AMBA_RDT_EMG_DESC_s));
        Err = CheckAddr(&(pCtrl->DescReadPtr), pCtrl->DescBufBase, pCtrl->DescBufSize, 1U);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("data is overwritten", 0U, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    /* then search the video idr frame as the beginning frame */
    if (Rval == RECODER_OK) {
        AmbaMisra_TypeCast(&pDecs, &(pCtrl->DescReadPtr));
        if (pDecs->IsValid == 1U) {
            while (pDecs->IsIdr == 0U) {
                pCtrl->DescReadPtr += sizeof(AMBA_RDT_EMG_DESC_s);
                Err = CheckAddr(&(pCtrl->DescReadPtr), pCtrl->DescBufBase, pCtrl->DescBufSize, 1U);
                if (Err != RECODER_OK) {
                    /* nothing */
                }
                AmbaMisra_TypeCast(&pDecs, &(pCtrl->DescReadPtr));
            }
        } else {
            /* num of the data is not enough for PrevSec, we just store all the data we have */
            pCtrl->DescReadPtr  = pCtrl->DescBufBase;
            pCtrl->IdrTotalNum -= (pCtrl->IdrPrevNum - pCtrl->IdrCount);
            pCtrl->IdrPrevNum   = pCtrl->IdrCount;
        }
    }

    return Rval;
}

static void UpdateMehd(const AMBA_RDT_EMG_CTRL_s *pCtrl)
{
    UINT32 Err, Rval = RECODER_OK;

    if (pCtrl->MP4Ctrl.Mp4Hdr.MehdCtrl.DefDuration != pCtrl->MP4Ctrl.Mp4Hdr.MehdCtrl.FragDuration) {
        UINT32 Duration, NumSuccess = 0U;
        UINT64 Offset;

        Duration = Invert_UInt32(pCtrl->MP4Ctrl.Mp4Hdr.MehdCtrl.FragDuration);
        Offset   = pCtrl->MP4Ctrl.Mp4Hdr.MehdCtrl.FileOffset + 12U;

        Err = AmbaVFS_Seek(&(pCtrl->File), (INT64)Offset, AMBA_CFS_SEEK_START);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaVFS_Seek failed %u", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (Rval == RECODER_OK) {
            Err = AmbaVFS_Write(&Duration, (UINT32)sizeof(Duration), 1U, &(pCtrl->File), &NumSuccess);
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaVFS_Write failed %u", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }
    }

    AmbaMisra_TouchUnused(&Rval);
}

static void WriteFragToFile(AMBA_RDT_EMG_CTRL_s *pCtrl, UINT32 IsEnd)
{
    UINT32 Err, *pBuf, NumSuccess;

    /* open file */
    if (pCtrl->IsFifoOpen == 0U) {
        Err = AmbaVFS_Open(pCtrl->FileName, "w", 1U, &(pCtrl->File));
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaVFS_Open failed", 0U, 0U);
        } else {
            pCtrl->IsFifoOpen     = 1U;
            pCtrl->FileUnsyncSize = 0U;
        }
    }

    if (pCtrl->IsFifoOpen == 1U) {
        if (IsEnd == 0U) {
            /* write fragment */
            AmbaMisra_TypeCast(&pBuf, &(pCtrl->MP4BufInfo.BufferBase));
            Err = AmbaVFS_Write(pBuf, pCtrl->WriteSize, 1U, &(pCtrl->File), &NumSuccess);
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaVFS_Write failed", 0U, 0U);
            }

            pCtrl->FileUnsyncSize += pCtrl->WriteSize;

            if ((pCtrl->FileSyncSize > 0U) && (pCtrl->FileUnsyncSize >= pCtrl->FileSyncSize)) {
                Err = AmbaVFS_Sync(&(pCtrl->File));
                if (Err != RECODER_OK) {
                    SVC_EMG_NG("AmbaVFS_Sync failed", 0U, 0U);
                }
                pCtrl->FileUnsyncSize = 0U;
            }
        } else {
            if (pCtrl->MP4Ctrl.Mp4Hdr.MehdCtrl.Enable > 0U) {
                UpdateMehd(pCtrl);
            }

            /* flush fifo buffer */
            Err = AmbaVFS_Sync(&(pCtrl->File));
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaVFS_Sync failed", 0U, 0U);
            }

            /* close file */
            Err = AmbaVFS_Close(&(pCtrl->File));
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaVFS_Close failed", 0U, 0U);
            } else {
                pCtrl->IsFifoOpen = 0U;
            }
        }
    }
}

static void MP4Reset(const AMBA_REC_DST_s *pEmgEvtRdt)
{
    UINT32                     Rval, SysTime, Err;
    AMBA_RDT_EMG_CTRL_s         *pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);
    const AMBA_RBX_USR_CFG_s    *pRbxUsrCfg = pEmgEvtRdt->UsrCfg.pRbxUsrCfg;
    MP4FMT_CTRL_s              *pMP4Ctrl;
    AMBA_RTC_DATE_TIME_s       CurrentTime;

    pMP4Ctrl = &(pCtrl->MP4Ctrl);

    /* config mp4 header */
    Rval = AmbaRTC_GetSysTime(&CurrentTime);
    if (Rval != RECODER_OK) {
        SVC_EMG_NG("AmbaRTC_GetSysTime failed", Rval, 0U);
    }
    Rval = AmbaUtcGetTotalSec(&CurrentTime, &SysTime);
    if (Rval != RECODER_OK) {
        SysTime = 0U;
    }

    pMP4Ctrl->Mp4Hdr.SubType              = pRbxUsrCfg->SubType;
    pMP4Ctrl->Mp4Hdr.CreationTime         = SysTime + UNIX_MAC_TIME_DIFF;
    pMP4Ctrl->Mp4Hdr.ModificationTime     = SysTime + UNIX_MAC_TIME_DIFF;
    pMP4Ctrl->BufInfo                     = pCtrl->MP4BufInfo;
    pMP4Ctrl->pfnBufCopy                  = MemCopy;

    {
        ULONG   AlignStart, AlignSize;

        AlignStart = pMP4Ctrl->BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
        AlignSize  = (((pMP4Ctrl->BufInfo.BufferBase + pMP4Ctrl->BufInfo.BufferSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
        Err = AmbaSvcWrap_CacheInvalidate(AlignStart, AlignSize);
        if (Err != RECODER_OK) {
            SVC_EMG_NG("AmbaSvcWrap_CacheInvalidate failed %u", Err, 0U);
        }
    }

    /* video */
    {
        pMP4Ctrl->Mp4Hdr.VideoCfg.VideoCoding = pRbxUsrCfg->VdCoding;
        pMP4Ctrl->Mp4Hdr.VideoCfg.VdWidth     = pRbxUsrCfg->VdWidth;
        pMP4Ctrl->Mp4Hdr.VideoCfg.VdHeight    = pRbxUsrCfg->VdHeight;
        pMP4Ctrl->Mp4Hdr.VideoCfg.TimeScale   = pRbxUsrCfg->VdTimeScale;
        pMP4Ctrl->Mp4Hdr.VideoCfg.M           = pRbxUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_M];
        pMP4Ctrl->Mp4Hdr.VideoCfg.N           = pRbxUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
        pMP4Ctrl->Mp4Hdr.VideoCfg.IdrInterval = pRbxUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
        pMP4Ctrl->Mp4Hdr.VideoCfg.NumTile     = 1U;
        pMP4Ctrl->Mp4Hdr.VideoCfg.NumSlice    = (UINT8)pRbxUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_SLICE];
        pMP4Ctrl->Mp4Hdr.VideoCfg.Orientation = SVC_ISO_ROTATION_NONE;
        pMP4Ctrl->Mp4Hdr.VideoCfg.ClosedGop   = 1U;
        pMP4Ctrl->TrafValid[TRACK_TYPE_VIDEO] = 1U;

        if (pMP4Ctrl->Mp4Hdr.VideoCfg.VideoCoding == AMBA_RSC_VID_SUBTYPE_HEVC) {
            pMP4Ctrl->Mp4Hdr.VideoCfg.NumTile = (UINT8)pRbxUsrCfg->MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_TILE];
        }

        if (pMP4Ctrl->Mp4Hdr.VideoCfg.M > 0U) {
            pMP4Ctrl->Mp4Hdr.VideoCfg.ClosedGop = 0U;
        }

#ifdef CONFIG_ENABLE_CAPTURE_TIME_MODE
        {
            UINT32 AudioClk;

            Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioClk);
            if (Rval != RECODER_OK) {
                SVC_EMG_NG("AmbaSYS_GetClkFreq Error", 0U, 0U);
                AudioClk = 12288000U;
            }

            pMP4Ctrl->Mp4Hdr.VideoCfg.CaptureTimeMode   = 1U;
            pMP4Ctrl->Mp4Hdr.VideoCfg.CaptureTimeScale  = AudioClk;
        }
#endif

        pMP4Ctrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO] = pRbxUsrCfg->VdNumUnitsInTick;
    }

    /* audio */
    if (pRbxUsrCfg->AuCoding != AMBA_RSC_AUD_SUBTYPE_NONE) {
        pMP4Ctrl->Mp4Hdr.AudioCfg.AudioCoding           = pRbxUsrCfg->AuCoding;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuSample              = pRbxUsrCfg->AuSample;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuBrate               = pRbxUsrCfg->AuBrate;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuChannels            = pRbxUsrCfg->AuChannels;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuVolume              = pRbxUsrCfg->AuVolume;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuWBitsPerSample      = pRbxUsrCfg->AuWBitsPerSample;
        pMP4Ctrl->Mp4Hdr.AudioCfg.AuFormat              = pRbxUsrCfg->AuFormat;
        pMP4Ctrl->TrafValid[TRACK_TYPE_AUDIO]           = 1U;
        pMP4Ctrl->BufInfo.SampleTicks[TRACK_TYPE_AUDIO] = pRbxUsrCfg->AuFrameSize;
    }

    /* text */
    if (pRbxUsrCfg->TExist != 0U) {
        pMP4Ctrl->TrafValid[TRACK_TYPE_DATA]            = 1U;
        pMP4Ctrl->BufInfo.SampleTicks[TRACK_TYPE_DATA]  = pRbxUsrCfg->VdNumUnitsInTick;
    }

#ifdef CONFIG_AMBA_REC_FMP4_MEHD_BOX
    {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        pMP4Ctrl->Mp4Hdr.MehdCtrl.Enable      = 1U;
        pMP4Ctrl->Mp4Hdr.MehdCtrl.TimeScale   = pMP4Ctrl->Mp4Hdr.VideoCfg.TimeScale;
        pMP4Ctrl->Mp4Hdr.MehdCtrl.DefDuration = 0;
#else
        UINT32 TempU32;
        DOUBLE TempDoub;

        TempU32  = pCtrl->IdrTotalNum * pMP4Ctrl->Mp4Hdr.VideoCfg.N * pMP4Ctrl->Mp4Hdr.VideoCfg.IdrInterval;
        TempDoub = (DOUBLE)TempU32 * (DOUBLE)pMP4Ctrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO];

        pMP4Ctrl->Mp4Hdr.MehdCtrl.Enable      = 1U;
        pMP4Ctrl->Mp4Hdr.MehdCtrl.TimeScale   = pMP4Ctrl->Mp4Hdr.VideoCfg.TimeScale;
        pMP4Ctrl->Mp4Hdr.MehdCtrl.DefDuration = (UINT32)TempDoub;

#endif
    }
#endif
}

static void* RdtEmgEvtTaskEntry(void* EntryArg)
{
    UINT32                     Err, Rval, ActualFlags, IdrNum, FragSize = 0U, LoopU = 1U, i, SrcType, Flush, ForceBreak;
    const AMBA_REC_DST_s       *pEmgEvtRdt;
    AMBA_RDT_EMG_CTRL_s        *pCtrl;
    const AMBA_RDT_EMG_DESC_s  *pDesc;
    AMBA_RSC_DESC_s             RscDesc;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pEmgEvtRdt, &EntryArg);

    pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);

    while (LoopU == 1U) {
        Rval = RECODER_OK;

        /* waiting for event start */
        ActualFlags = 0U;
        Err  = AmbaKAL_EventFlagGet(&(pCtrl->TaskInfo.TaskFlag), SVC_EMGEVT_FLAG_EVENT_START, AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            SVC_EMG_NG("RdtEmgEvtTaskEntry: get flag failed %u", Err, 0U);
            LoopU = 0U;
        }

        ForceBreak = 0U;

        if ((pCtrl->IsRunning == 1U) && ((ActualFlags & SVC_EMGEVT_FLAG_EVENT_START) > 0U)) {
            /* get the file name */
            if (pEmgEvtRdt->UsrCfg.pfnFetchFName != NULL) {
                Err = pEmgEvtRdt->UsrCfg.pfnFetchFName(pCtrl->FileName, AMBA_REC_MAX_FILE_NAME_LEN, pEmgEvtRdt->UsrCfg.StreamID);
                if (Err != RECODER_OK) {
                    SVC_EMG_NG("get file name failed", 0U, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            } else {
                Rval = RECODER_ERROR_GENERAL_ERROR;
                SVC_EMG_NG("pfnFetchFName is null", 0U, 0U);
            }

            /* search the start feeding frame */
            if (Rval == RECODER_OK) {
                Err = SearchStartReadPtr(pCtrl);
                if (Err != RECODER_OK) {
                    SVC_EMG_NG("SearchStartReadPtr failed(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            /* update write size */
            if (Rval == RECODER_OK) {
                ULONG StartAddr, WSize;
                AmbaMisra_TypeCast(&pDesc, &(pCtrl->DescReadPtr));
                AmbaMisra_TypeCast(&StartAddr, &(pDesc->SrcDesc.pSrcData));
                if (StartAddr <= pCtrl->FrameWritePtr) {
                    WSize             = pCtrl->FrameWritePtr - StartAddr;
                    pCtrl->FrameWSize = (UINT32)WSize;
                } else {
                    WSize             = ((ULONG)pCtrl->FrameBufSize + pCtrl->FrameWritePtr) - StartAddr;
                    pCtrl->FrameWSize = (UINT32)WSize;
                }
            }

            if (Rval == RECODER_OK) {
                pCtrl->EventStart = 1U;

                /* reset mp4 box */
                MP4Reset(pEmgEvtRdt);
                IdrNum = 1U;
                pCtrl->FlushBits   = 0U;
                pCtrl->AllSrcBits  = pEmgEvtRdt->UsrCfg.InSrcBits;

                for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
                    pCtrl->SrcReadPtr[i]  = pCtrl->DescReadPtr;
                    pCtrl->FrameNum[i]    = 0U;
                    pCtrl->LastDescIdx[i] = 0U;
                }

                MemWait(pCtrl, WAIT_FRAME_BUF);

                while (IdrNum <= pCtrl->IdrTotalNum) {
                    /* if prev data (frame before event happen) process done */
                    if (IdrNum >= pCtrl->IdrPrevNum) {
                        if (pCtrl->NextStart == 0U) {
                            pCtrl->NextStart = 1U;
                        }


                        /* clear ready flag if the number of frames is not enough for a fragment */
                        {
                            UINT32 ReadNum, WriteNum;
                            ULONG  WriteAddr;

                            AmbaMisra_TypeCast(&pDesc, &(pCtrl->DescReadPtr));
                            ReadNum  = pDesc->DescNumber;

                            WriteAddr = pCtrl->DescWritePtr - sizeof(AMBA_RDT_EMG_DESC_s);
                            AmbaMisra_TypeCast(&pDesc, &WriteAddr);
                            WriteNum = pDesc->DescNumber;

                            if ((ReadNum + pCtrl->FramePerFrag) >= WriteNum) {
                                Err = AmbaKAL_EventFlagClear(&(pCtrl->TaskInfo.TaskFlag), SVC_EMGEVT_FLAG_DATA_READY);
                                if (Err != KAL_ERR_NONE) {
                                    SVC_EMG_NG("RdtEmgEvtTaskEntry: clear flag failed %u", Err, 0U);
                                }
                            }
                        }

                        /* waiting for there are enough frames for writing */
                        Err = AmbaKAL_EventFlagGet(&(pCtrl->TaskInfo.TaskFlag), SVC_EMGEVT_FLAG_DATA_READY, AMBA_KAL_FLAGS_ANY,
                               AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
                        if (Err != KAL_ERR_NONE) {
                            SVC_EMG_NG("RdtEmgEvtTaskEntry: get flag failed %u", Err, 0U);
                        }
                    }

                    for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
                        if (((pCtrl->AllSrcBits & ((UINT32)1U << i)) == 0U) || ((pCtrl->FlushBits & ((UINT32)1U << i)) > 0U)) {
                            continue;
                        }

                        if (i == AMBA_REC_MIA_VIDEO) {
                            SrcType = AMBA_RSC_TYPE_VIDEO;
                        } else if (i == AMBA_REC_MIA_AUDIO) {
                            SrcType = AMBA_RSC_TYPE_AUDIO;
                        } else {
                            SrcType = AMBA_RSC_TYPE_DATA;
                        }

                        AmbaMisra_TypeCast(&pDesc, &(pCtrl->SrcReadPtr[i]));

                        while (pDesc->SrcDesc.SrcType != SrcType) {
                            pCtrl->SrcReadPtr[i] += sizeof(AMBA_RDT_EMG_DESC_s);
                            Err = CheckAddr(&(pCtrl->SrcReadPtr[i]), pCtrl->DescBufBase, pCtrl->DescBufSize, 1U);
                            AmbaMisra_TypeCast(&pDesc, &(pCtrl->SrcReadPtr[i]));
                        }

                        /* frame buffer overwrite check */
                        {
                            UINT32 DescDiff = 0U, MaxDescNum;

                            /* DescNumber diff should not larger than the desc buffer size */
                            MaxDescNum = pCtrl->DescBufSize / (UINT32)sizeof(AMBA_RDT_EMG_DESC_s);
                            if (pCtrl->LastDescIdx[i] > 0U) {
                                DescDiff   = pDesc->DescNumber - pCtrl->LastDescIdx[i];
                                if (DescDiff > MaxDescNum) {
                                    ForceBreak = 1U;
                                    SVC_EMG_NG("DescDiff %u >  MaxDescNum %u", DescDiff, MaxDescNum);
                                }
                            }

                            if (pCtrl->FrameWSize > pCtrl->FrameRSize) {
                                if ((pCtrl->FrameWSize - pCtrl->FrameRSize) > pCtrl->FrameBufSize) {
                                    ForceBreak = 1U;
                                    SVC_EMG_NG("emg frame buffer is overwritten %u/%u", pCtrl->FrameWSize - pCtrl->FrameRSize, pCtrl->FrameBufSize);
                                }
                            }

                            if (ForceBreak == 1U) {
                                break;
                            }
                        }

                        /* insert data to mp4 */
                        RscDesc.SrcType    = pDesc->SrcDesc.SrcType;
                        RscDesc.SrcSubType = pDesc->SrcDesc.SrcSubType;
                        RscDesc.pSrcData   = pDesc->SrcDesc.pSrcData;
                        RscDesc.SrcBufBase = pDesc->SrcDesc.SrcBufBase;
                        RscDesc.SrcBufSize = pDesc->SrcDesc.SrcBufSize;
                        Err = AmbaMux_Proc(&(pCtrl->MP4Ctrl), &(RscDesc), AMBA_MP4FMT_FLUSH_NONE, &(pCtrl->WriteSize));
                        if (Err != RECODER_OK) {
                            SVC_EMG_NG("AmbaMux_Proc failed %u", Err, 0U);
                        }

                        pCtrl->SrcReadPtr[i] += sizeof(AMBA_RDT_EMG_DESC_s);
                        pCtrl->LastDescIdx[i] = pDesc->DescNumber;
                        Err = CheckAddr(&(pCtrl->SrcReadPtr[i]), pCtrl->DescBufBase, pCtrl->DescBufSize, 1U);

                        if (pDesc->SrcDesc.SrcType == AMBA_RSC_TYPE_VIDEO) {
                            const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;
                            AmbaMisra_TypeCast(&(pVDesc), &(pDesc->SrcDesc.pSrcData));
                            if ((pVDesc->TileIdx == 0U) &&
                                (pVDesc->SliceIdx == 0U)) {
                                pCtrl->FrameNum[i]++;
                            }
                        } else {
                            pCtrl->FrameNum[i]++;
                        }

                        if (pCtrl->FrameNum[i] >= pCtrl->FrameThr[i]) {
                            pCtrl->FlushBits |= ((UINT32)1U << i);
                        }

                        /* update temp read size */
                        if (pDesc->SrcDesc.SrcType == AMBA_RSC_TYPE_VIDEO) {
                            const AMBA_DSP_ENC_PIC_RDY_s   *pVDesc;
                            AmbaMisra_TypeCast(&(pVDesc), &(pDesc->SrcDesc.pSrcData));

                            pCtrl->FrameRSizeTmp += GetRoundUpValU32((UINT32)sizeof(AMBA_DSP_ENC_PIC_RDY_s), (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                            pCtrl->FrameRSizeTmp += GetRoundUpValU32(pVDesc->PicSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                        } else if (pDesc->SrcDesc.SrcType == AMBA_RSC_TYPE_AUDIO) {
                            const AMBA_AENC_AUDIO_DESC_s   *pADesc;
                            AmbaMisra_TypeCast(&(pADesc), &(pDesc->SrcDesc.pSrcData));

                            pCtrl->FrameRSizeTmp += GetRoundUpValU32((UINT32)sizeof(AMBA_AENC_AUDIO_DESC_s), (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                            pCtrl->FrameRSizeTmp += GetRoundUpValU32(pADesc->DataSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                        } else {
                            pCtrl->FrameRSizeTmp += GetRoundUpValU32((UINT32)sizeof(AMBA_RSC_TEXT_DESC_s), (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                            pCtrl->FrameRSizeTmp += GetRoundUpValU32(CONFIG_AMBA_REC_DRSC_TEXTLEN, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;
                        }
                    }

                    if (ForceBreak > 0U) {
                        SVC_EMG_NG("force to stop emg event!", 0U, 0U);
                        break;
                    }

                    if (pCtrl->FlushBits == pCtrl->AllSrcBits) {
                        AmbaMisra_TypeCast(&pDesc, &(pCtrl->SrcReadPtr[AMBA_REC_MIA_VIDEO]));

                        while (pDesc->SrcDesc.SrcType != AMBA_RSC_TYPE_VIDEO) {
                            pCtrl->SrcReadPtr[AMBA_REC_MIA_VIDEO] += sizeof(AMBA_RDT_EMG_DESC_s);
                            Err = CheckAddr(&(pCtrl->SrcReadPtr[AMBA_REC_MIA_VIDEO]), pCtrl->DescBufBase, pCtrl->DescBufSize, 1U);
                            AmbaMisra_TypeCast(&pDesc, &(pCtrl->SrcReadPtr[AMBA_REC_MIA_VIDEO]));
                        }

                        RscDesc.SrcType    = pDesc->SrcDesc.SrcType;
                        RscDesc.SrcSubType = pDesc->SrcDesc.SrcSubType;
                        RscDesc.pSrcData   = pDesc->SrcDesc.pSrcData;
                        RscDesc.SrcBufBase = pDesc->SrcDesc.SrcBufBase;
                        RscDesc.SrcBufSize = pDesc->SrcDesc.SrcBufSize;

                        if (IdrNum == pCtrl->IdrTotalNum) {
                            Flush = AMBA_MP4FMT_FLUSH_REACH_END;
                        } else {
                            Flush = AMBA_MP4FMT_FLUSH_REACH_GOP;
                        }

                        MemWait(pCtrl, WAIT_FRAME_BUF);

                        Err = AmbaMux_Proc(&(pCtrl->MP4Ctrl), &(RscDesc), Flush, &(pCtrl->WriteSize));
                        if (Err != RECODER_OK) {
                            SVC_EMG_NG("AmbaMux_Proc failed %u", Err, 0U);
                        }

                        MemWait(pCtrl, WAIT_FRAG_BUF);

                        /* update read size */
                        pCtrl->FrameRSize   += pCtrl->FrameRSizeTmp;
                        pCtrl->FrameRSizeTmp = 0U;

                        if (FragSize <= pCtrl->FragSize) {
                            WriteFragToFile(pCtrl, 0U);
                        } else {
                            SVC_EMG_NG("MP4 frag buffer overflow, (%u/%u)", FragSize, pCtrl->FragSize);
                        }

                        IdrNum++;
                        pCtrl->FlushBits = 0U;
                        pCtrl->DescReadPtr = pCtrl->SrcReadPtr[AMBA_REC_MIA_VIDEO];
                        pCtrl->LastDescIdx[AMBA_REC_MIA_VIDEO] = pDesc->DescNumber;

                        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
                            pCtrl->FrameNum[i]   = 0U;
                        }

                        {
                            ULONG   AlignStart, AlignSize;

                            AlignStart = pCtrl->MP4Ctrl.BufInfo.BufferBase & AMBA_CACHE_LINE_MASK;
                            AlignSize  = (((pCtrl->MP4Ctrl.BufInfo.BufferBase + pCtrl->MP4Ctrl.BufInfo.BufferSize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK) - AlignStart;
                            Err = AmbaSvcWrap_CacheInvalidate(AlignStart, AlignSize);
                            if (Err != RECODER_OK) {
                                SVC_EMG_NG("AmbaSvcWrap_CacheInvalidate failed %u", Err, 0U);
                            }
                        }
                    }
                }
                /* close the file */
                WriteFragToFile(pCtrl, 1U);

                /* update file name */
                if (pEmgEvtRdt->UsrCfg.pfnUpdateFName != NULL) {
                    Err = pEmgEvtRdt->UsrCfg.pfnUpdateFName(pCtrl->FileName,
                                                            pEmgEvtRdt->UsrCfg.StreamID);
                    if (Err != RECODER_OK) {
                        SVC_EMG_NG("fail to update filename", 0U, 0U);
                    }
                } else {
                    SVC_EMG_NG("pfnUpdateFName is null", 0U, 0U);
                }
            }
            /* event stop */
            pCtrl->EventStart = 0U;
        }
    }

    return NULL;
}

static UINT32 CalcFragMemSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, MP4FMT_BUF_INFO_s *pBufInfo)
{
    #define SVC_ELEM_BRATE_TOLERANCE    (50U)   /* 50% */

    UINT32 Err, BufferSize, Rval = 0U;

    Err = AmbaMux_EvalFragSize(SubType, pEval, pBufInfo);

    if (RECODER_OK != Err) {
        SVC_EMG_NG("AmbaMux_EvalFragSize failed %u", Err, 0U);
    } else {
        BufferSize = (pBufInfo->BufferSize * (SVC_ELEM_BRATE_TOLERANCE + 100U)) / 100U;
        BufferSize = GetRoundUpValU32(BufferSize, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

        pBufInfo->BufferSize = BufferSize;
        Rval                 = BufferSize;
    }

    return Rval;
}

/**
* create emergency event record task
* @param [in]  pEmgEvtRdt record destination control
* @return ErrorCode
*/
UINT32 AmbaRdtEmgEvt_Create(AMBA_REC_DST_s *pEmgEvtRdt)
{
    UINT32                Err, Rval = RECODER_OK, i, Size, DescSize;
    ULONG                 Base, TmpUL;
    static UINT32         IsInit = 0U;
    AMBA_RDT_EMG_CTRL_s    *pCtrl;
    static char           FlagName[] = "SvcEmgEvtFlag";

    if (pEmgEvtRdt->UsrCfg.StreamID >= (UINT32)SVC_EMGEVT_MAX_STREAM_NUM) {
        SVC_EMG_NG("Invalid StreamId(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
        Rval  = RECODER_ERROR_GENERAL_ERROR;
    }

    if ((pEmgEvtRdt->UsrCfg.DstType != AMBA_RDT_TYPE_EVT)
        && (pEmgEvtRdt->UsrCfg.SubType != AMBA_RDT_STYPE_EMG)) {
        SVC_EMG_NG("Wrong destination type (%u)", pEmgEvtRdt->UsrCfg.DstType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (IsInit == 0U) {
        Err = AmbaWrap_memset(EmgCtrl, 0, sizeof(EmgCtrl));
        if (RECODER_OK != Err) {
            SVC_EMG_NG("AmbaWrap_memset failed(%u)", Err, 0U);
        }
        IsInit = 1U;
    }

    if (Rval == RECODER_OK) {
        AmbaMisra_TouchUnused(&Rval);
        pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);

        if (pCtrl->TaskInfo.IsCreat == 0U) {
            /* create flag */
            if (Rval == RECODER_OK) {
                Err = AmbaKAL_EventFlagCreate(&(pCtrl->TaskInfo.TaskFlag), FlagName);
                if (Err != RECODER_OK) {
                    SVC_EMG_NG("AmbaKAL_EventFlagCreate failed(%u)", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                static char Name[] = "SvcEmgEvt";
                Err = AmbaKAL_TaskCreate(&(pCtrl->TaskInfo.TaskCtrl),
                                        Name,
                                        pEmgEvtRdt->UsrCfg.TaskPriority,
                                        RdtEmgEvtTaskEntry,
                                        pEmgEvtRdt,
                                        pCtrl->TaskInfo.TaskStack,
                                        SVC_EMGEVT_STACK_SIZE,
                                        0);
                if (Err != KAL_ERR_NONE) {
                    SVC_EMG_NG("AmbaKAL_TaskCreate failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskSetSmpAffinity(&(pCtrl->TaskInfo.TaskCtrl), pEmgEvtRdt->UsrCfg.TaskCpuBits);
                if (Err != KAL_ERR_NONE) {
                    SVC_EMG_NG("AmbaKAL_TaskSetSmpAffinity failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskResume(&(pCtrl->TaskInfo.TaskCtrl));
                if (Err != KAL_ERR_NONE) {
                    SVC_EMG_NG("AmbaKAL_TaskResume failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                pCtrl->TaskInfo.IsCreat = 1U;
            }
        }
    }

    /* init parameters */
    if (Rval == RECODER_OK) {
        pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);

        /* buffer */
        AmbaRdtEmgEvt_EvalMemSize(&(pEmgEvtRdt->UsrCfg.RecEval), pEmgEvtRdt->UsrCfg.MaxRecPeriod, &Size);
        if (Size <= pEmgEvtRdt->UsrCfg.MemSize) {
            /*  source descriptor buffer */
            pCtrl->MaxRecPeriod   = pEmgEvtRdt->UsrCfg.MaxRecPeriod;
            pCtrl->DescBufBase    = pEmgEvtRdt->UsrCfg.MemBase;
            pCtrl->DescWritePtr   = pEmgEvtRdt->UsrCfg.MemBase;
            pCtrl->DescBufSize    = 0U;
            for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
                pCtrl->DescBufSize += (pEmgEvtRdt->UsrCfg.MaxRecPeriod * pEmgEvtRdt->UsrCfg.RecEval.MiaCfg[i].FrameRate) * (UINT32)sizeof(AMBA_RDT_EMG_DESC_s);
            }

            /* mp4 frag buffer */
            pCtrl->MP4BufInfo.BufferBase = pCtrl->DescBufBase + pCtrl->DescBufSize;
            pCtrl->MP4BufInfo.BufferBase = (((pCtrl->MP4BufInfo.BufferBase) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            pCtrl->FragSize = CalcFragMemSize(pEmgEvtRdt->UsrCfg.pRbxUsrCfg->SubType, &(pEmgEvtRdt->UsrCfg.RecEval), &(pCtrl->MP4BufInfo));

            /* descriptor buffer */
            Err = AmbaMux_EvalDescSize(pEmgEvtRdt->UsrCfg.pRbxUsrCfg->SubType, &(pEmgEvtRdt->UsrCfg.RecEval), &DescSize);
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaMux_EvalDescSize failed %u ", Err, 0U);
            }
            pCtrl->MP4Ctrl.DescBase = pCtrl->MP4BufInfo.BufferBase + pCtrl->FragSize;
            pCtrl->MP4Ctrl.DescBase = (((pCtrl->MP4Ctrl.DescBase) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            pCtrl->MP4Ctrl.DescSize = DescSize;

            /* bits data buffer */
            Base                 = pCtrl->MP4Ctrl.DescBase + pCtrl->MP4Ctrl.DescSize;
            Base                 = (((Base) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            pCtrl->FrameBufBase  = Base;
            TmpUL                = pEmgEvtRdt->UsrCfg.MemSize - (pCtrl->FrameBufBase - pEmgEvtRdt->UsrCfg.MemBase);
            pCtrl->FrameBufSize  = (UINT32)TmpUL;
            pCtrl->FrameWritePtr = pCtrl->FrameBufBase;
            pCtrl->pRecEval      = &(pEmgEvtRdt->UsrCfg.RecEval);

            SVC_EMG_DBG("DescBufBase/DescBufSize(0x%08X/%u)", (UINT32)pCtrl->DescBufBase, pCtrl->DescBufSize);
            SVC_EMG_DBG("FrameBufBase/FrameBufSize(0x%08X/%u)", (UINT32)pCtrl->FrameBufBase, pCtrl->FrameBufSize);
        } else {
            SVC_EMG_NG("Buffer is not enough(%u/%u)", pEmgEvtRdt->UsrCfg.MemSize, Size);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        /* reset parameters */
        pCtrl->IsFifoOpen     = 0U;
        pCtrl->DescCount      = 0U;
        pCtrl->NextStart      = 0U;
        pCtrl->EventStart     = 0U;
        pCtrl->pRdtCtrl       = pEmgEvtRdt;

        /* store function */
        pEmgEvtRdt->pfnStore = DataStore;

        /* rec source bits */
        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
            if (pEmgEvtRdt->UsrCfg.RecEval.MiaCfg[i].FrameRate != 0U) {
                pCtrl->FrameThr[i] = pEmgEvtRdt->UsrCfg.RecEval.MiaCfg[i].FrameRate;
            }
        }

    }

    return Rval;
}

/**
* destroy emergency event record task
* @param [in]  pEmgEvtRdt record destination control
* @return ErrorCode
*/
UINT32 AmbaRdtEmgEvt_Destroy(const AMBA_REC_DST_s *pEmgEvtRdt)
{
    UINT32                Err, Rval = RECODER_OK;
    AMBA_RDT_EMG_CTRL_s    *pCtrl;

    if (pEmgEvtRdt->UsrCfg.StreamID >= (UINT32)SVC_EMGEVT_MAX_STREAM_NUM) {
        SVC_EMG_NG("Invalid StreamId(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (Rval == RECODER_OK) {
        pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);
        if (pCtrl->TaskInfo.IsCreat == 1U) {
            /* delete the task */
            {
                Err = AmbaKAL_TaskTerminate(&(pCtrl->TaskInfo.TaskCtrl));
                if (Err != KAL_ERR_NONE) {
                    SVC_EMG_NG("AmbaKAL_TaskTerminate failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == RECODER_OK) {
                Err = AmbaKAL_TaskDelete(&(pCtrl->TaskInfo.TaskCtrl));
                if (Err != KAL_ERR_NONE) {
                    SVC_EMG_NG("AmbaKAL_TaskDelete failed %u ", Err, 0U);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }
            }

            /* delete event flag */
            Err = AmbaKAL_EventFlagDelete(&(pCtrl->TaskInfo.TaskFlag));
            if (Err != RECODER_OK) {
                SVC_EMG_NG("AmbaKAL_EventFlagDelete failed(%u)", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }

            if (Rval == RECODER_OK) {
                pCtrl->TaskInfo.IsCreat = 0U;
            }

        } else {
            SVC_EMG_DBG("The task is not existed(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
        }
    }

    return Rval;
}

/**
* emergency event record control
* @param [in]  pEmgEvtRdt record destination control
* @param [in]  CtrlType control type
* @param [in]  pParam control parameter
*/
void AmbaRdtEmgEvt_Control(const AMBA_REC_DST_s *pEmgEvtRdt, UINT32 CtrlType, void *pParam)
{
    UINT32                Err = RECODER_OK;
    AMBA_RDT_EMG_CTRL_s    *pCtrl;

    AmbaMisra_TouchUnused(pParam);

    if (pEmgEvtRdt->UsrCfg.StreamID >= (UINT32)SVC_EMGEVT_MAX_STREAM_NUM) {
        SVC_EMG_NG("Invalid StreamId(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
        Err   = RECODER_ERROR_GENERAL_ERROR;
    }

    if (Err == RECODER_OK) {
        pCtrl = &(EmgCtrl[pEmgEvtRdt->UsrCfg.StreamID]);

        switch (CtrlType) {
        case AMBA_RDTC_EMGEVT_STATIS_GET:
            break;
        case AMBA_RDTC_EMGEVT_OPEN:
            if (pCtrl->IsRunning == 0U) {
                void         *Ptr;
                const UINT32 *pSyncSize;

                AmbaMisra_TypeCast(&pSyncSize, &(pParam));

                pCtrl->DescWritePtr  = pCtrl->DescBufBase;
                pCtrl->FrameWritePtr = pCtrl->FrameBufBase;
                pCtrl->DescCount     = 0U;
                pCtrl->IdrCount      = 0U;

                if (pParam != NULL) {
                    pCtrl->FileSyncSize = *pSyncSize;
                } else {
                    pCtrl->FileSyncSize = 0U;
                }

                /* clean the buffer */
                AmbaMisra_TypeCast(&Ptr, &(pCtrl->DescBufBase));
                Err = AmbaWrap_memset(Ptr, 0, pCtrl->DescBufSize);
                if (RECODER_OK != Err) {
                    SVC_EMG_NG("AmbaWrap_memset failed(%u)", Err, 0U);
                }

                AmbaMisra_TypeCast(&Ptr, &(pCtrl->FrameBufBase));
                Err = AmbaWrap_memset(Ptr, 0, pCtrl->FrameBufSize);
                if (RECODER_OK != Err) {
                    SVC_EMG_NG("AmbaWrap_memset failed(%u)", Err, 0U);
                }

                /* enable data store */
                pCtrl->IsRunning = 1U;
            } else {
                SVC_EMG_DBG("EmgEvt is opened already(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
            }
            break;
        case AMBA_RDTC_EMGEVT_CLOSE:
            if (pCtrl->IsRunning == 1U) {
                /* disable data store */
                pCtrl->IsRunning = 0U;
            } else {
                SVC_EMG_DBG("EmgEvt is closed already(%u)", pEmgEvtRdt->UsrCfg.StreamID, 0U);
            }
            break;
        default:
            SVC_EMG_NG("Unknown control type(%u)", CtrlType, 0U);
            break;
        }
    }

    AmbaMisra_TouchUnused(&pParam);
}

/**
* eval buffer size for emergency event
* @param [in]  pEval record information
* @param [in]  MaxRecPeriod max record period
* @param [out]  pSize pointer of evaluated memory size
*/
void AmbaRdtEmgEvt_EvalMemSize(const AMBA_REC_EVAL_s *pEval, UINT32 MaxRecPeriod, UINT32 *pSize)
{
    UINT32             i, DescSize = 0U, Size = 0U, RecPeriod, MuxSize, Err;
    MP4FMT_BUF_INFO_s  MP4BufInfo;

    for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
        if (i == AMBA_REC_MIA_VIDEO) {
            DescSize = (UINT32)sizeof(AMBA_DSP_ENC_PIC_RDY_s);
        } else if (i == AMBA_REC_MIA_AUDIO) {
            DescSize = (UINT32)sizeof(AMBA_AENC_AUDIO_DESC_s);
        } else if (i == AMBA_REC_MIA_DATA) {
            DescSize = (UINT32)sizeof(AMBA_RSC_TEXT_DESC_s);
            DescSize += (UINT32)CONFIG_AMBA_REC_DRSC_TEXTLEN;
        } else {
            /* should never enter here */
        }

        #define SVC_EMGEVT_BUF_TOLERANCE    (10U)   /* 10% */
        RecPeriod = (MaxRecPeriod *  (100U + SVC_EMGEVT_BUF_TOLERANCE)) / 100U;

        Size += (RecPeriod * pEval->MiaCfg[i].BitRate) / 8U;
        Size += (RecPeriod * pEval->MiaCfg[i].FrameRate) * ((UINT32)sizeof(AMBA_RDT_EMG_DESC_s) + DescSize);
    }
    Size =  GetRoundUpValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

    Err = AmbaMux_EvalDescSize(AMBA_SUB_TYPE_FMP4, pEval, &MuxSize);
    if (Err != RECODER_OK) {
        SVC_EMG_NG("AmbaMux_EvalDescSize failed %u ", Err, 0U);
    }
    Size += MuxSize;
    Size =  GetRoundUpValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

    Size += CalcFragMemSize(AMBA_SUB_TYPE_FMP4, pEval, &MP4BufInfo);
    Size =  GetRoundUpValU32(Size, (UINT32)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

    SVC_EMG_DBG("EvalMemSize(%u)", Size, 0U);

    *pSize = Size;
}

/**
* emergency event start
* @param [in]  StreamBits start stream bits
* @param [in]  PrevSec how many secs to record berfore event happening
* @param [in]  NextSec how many secs to record after event happening
* @return ErrorCode
*/
UINT32 AmbaRdtEmgEvt_EvtStart(UINT32 StreamBits, UINT32 PrevSec, UINT32 NextSec)
{
    UINT32              Err, Rval = RECODER_OK;
    UINT32              StreamNum, i, j;
    UINT16              StrmIdxArr[SVC_EMGEVT_MAX_STREAM_NUM];
    AMBA_RDT_EMG_CTRL_s  *pCtrl;

    AmbaMisra_TouchUnused(&Rval);
    SvcUtil_BitsToArr(SVC_EMGEVT_MAX_STREAM_NUM, StreamBits, &StreamNum, StrmIdxArr);

    for (i = 0U; i < StreamNum; i++) {
        pCtrl = &(EmgCtrl[StrmIdxArr[i]]);

        if (Rval == RECODER_OK) {
            if (pCtrl->IsRunning != 1U) {
                SVC_EMG_NG("Stream(%u) is not opened", StrmIdxArr[i], 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == RECODER_OK) {
            if (pCtrl->pRdtCtrl->UsrCfg.pfnCheckStorage(StrmIdxArr[i]) != RECODER_OK) {
                SVC_EMG_NG("Storage is not ready", 0U, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == RECODER_OK) {
            /* calculate how many frames existed in one fragment */
            pCtrl->FramePerFrag = 0U;
            for (j = 0U; j < AMBA_REC_MIA_MAX; j++) {
                if (0U < CheckBits(pCtrl->pRdtCtrl->UsrCfg.InSrcBits, ((UINT32)1U << j))) {
                    pCtrl->FramePerFrag   += pCtrl->pRdtCtrl->UsrCfg.RecEval.MiaCfg[j].FrameRate;
                }
            }
            pCtrl->FramePerFrag  *= pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N] *
                                   pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
            pCtrl->FramePerFrag  /= pCtrl->pRdtCtrl->UsrCfg.RecEval.MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate;

            #define SVC_EMGEVT_FRAG_TOLERANCE    (20U)   /* 20% */
            pCtrl->FramePerFrag   = (pCtrl->FramePerFrag *  (100U + SVC_EMGEVT_FRAG_TOLERANCE)) / 100U;

            if (pCtrl->MaxRecPeriod < (PrevSec + NextSec)) {
                SVC_EMG_NG("RecPeriod is over than threshold(%u/%u)", PrevSec + NextSec, pCtrl->MaxRecPeriod);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else if ((PrevSec + NextSec) == 0U) {
                SVC_EMG_NG("RecPeriod is over than threshold(%u/%u)", PrevSec + NextSec, pCtrl->MaxRecPeriod);
                Rval = RECODER_ERROR_GENERAL_ERROR;

            } else {
                if (pCtrl->EventStart == 0U) {
                    pCtrl->PrevSec     = PrevSec;
                    pCtrl->NextSec     = NextSec;

                    /* claculate how many idr frames need to record before event happen */
                    pCtrl->IdrPrevNum  = PrevSec * pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate;
                    pCtrl->IdrPrevNum  = GetRoundUpValU32(pCtrl->IdrPrevNum, pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N] *
                                                          pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT]);

                    /* claculate the total idr frames need to record */
                    pCtrl->IdrTotalNum = (PrevSec + NextSec) * pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate;
                    pCtrl->IdrTotalNum = GetRoundUpValU32(pCtrl->IdrTotalNum, pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N] *
                                                          pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT]);

                    pCtrl->FrameWSize    = 0U;
                    pCtrl->FrameRSize    = 0U;
                    pCtrl->FrameRSizeTmp = 0U;
                } else {
                    /* if the event is still on, extends record period */
                    UINT32 ExtendNum;

                    ExtendNum          = NextSec * pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate;
                    ExtendNum          = GetRoundUpValU32(ExtendNum, pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N] *
                                                          pCtrl->pRecEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT]);
                    pCtrl->IdrTotalNum += ExtendNum;
                }
            }
        }

        if (Rval == RECODER_OK) {
            Err = AmbaKAL_EventFlagSet(&(pCtrl->TaskInfo.TaskFlag), SVC_EMGEVT_FLAG_EVENT_START);
            if (Err != KAL_ERR_NONE) {
                SVC_EMG_NG("AmbaKAL_EventFlagSet failed(%u)", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}
