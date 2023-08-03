/**
 *  @file AmbaAudio_AOUT.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Audio output process functions.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaI2S.h"
#include "AmbaRTSL_I2S.h"
#include "AmbaDMA.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaMisraFix.h"

#define AMBA_AOUT_TIMEOUT    1000U

/**
* Audio output query working buffer size function
* @param [in]  pInfo Audio output resource creation information
* @return ErrorCode
*/
UINT32 AmbaAOUT_QueryBufSize(AMBA_AOUT_IO_CREATE_INFO_s *pInfo)
{
    UINT32 RtVal = (UINT32)AOUT_OK;

    if (pInfo != NULL) {
        pInfo->CachedBufSize = (UINT32)sizeof(AMBA_AOUT_HDLR) +
                               ((UINT32)sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT) +
                               (pInfo->DmaFrameSize * pInfo->ChannelNum * (UINT32)sizeof(UINT32)) + /*Mix Buf*/
                               ((UINT32)sizeof(AMBA_AOUT_IO_NODE_s) * pInfo->IoNodeNum) +
                               (UINT32)AMBA_CACHE_LINE_SIZE + /* AMBA_CACHE_LINE_SIZE byte alignment padding */
                               (pInfo->DmaFrameSize * pInfo->ChannelNum * (UINT32)sizeof(UINT32) * pInfo->DmaDescNum);

        pInfo->NonCachedBufSize = 8U + /* 8 byte alignment padding */
                                  ((UINT32)sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum) +
                                  ((UINT32)sizeof(UINT32) * pInfo->DmaDescNum);

        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                                   "[AUD]AmbaAOUT_QueryBufSize Cached: %d, Non-cached: %d", pInfo->CachedBufSize, pInfo->NonCachedBufSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null pInfo.",__func__, NULL, NULL, NULL, NULL);
        RtVal = (UINT32)AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_QueryBufSize End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

static UINT32 AmbaAOUT_GetMem(AMBA_AOUT_MEM_INFO_s *pInfo, UINT32 Size, UINT32 **pMemAddr)
{
    UINT32 MemSize;
    UINT32 RtVal = (UINT32)AOUT_OK;
    UINT32 *pPtr;

    if ((Size % sizeof(UINT32)) != 0U) {
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Not 4-ByteAlignment size: %d", Size, 0U, 0U, 0U, 0U);
        RtVal = (UINT32)AOUT_ERR_0002;
    } else {
        MemSize = pInfo->CurrentSize + Size;
        if (MemSize > pInfo->MaxSize) {
            *pMemAddr = NULL;
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Out of memory. MaxSize: %d  NeedSize: %d", pInfo->MaxSize, MemSize, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0004;
        } else {
            pInfo->CurrentSize = MemSize;
            *pMemAddr = pInfo->pHead;
            pPtr = pInfo->pHead;
            pInfo->pHead = &(pPtr[Size/sizeof(UINT32)]);/*pInfo->pHead += (Size/sizeof(UINT32));*/
        }
    }
    return RtVal;
}

static UINT32 AmbaAOUT_CreateRescSysInfo(const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                                         AMBA_AOUT_HDLR *pAoutHdlr,
                                         AMBA_AOUT_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AOUT_OK, SubRtVal;
    UINT32 *pMemAddr;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    UINT32 Loop;
    static char AoutEventFlagName[30] = "AoutEventFlags";
    static char AoutEventMutexName[25] = "AoutEventMutex";
    static char AoutIoNodeMutexName[25] = "AoutIoNodeMutex";


    /* create the Audio task Status Flags */
    if (AmbaKAL_EventFlagCreate(&(pAoutHdlr->Flag), AoutEventFlagName) != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagCreate fail",__func__, NULL, NULL, NULL, NULL);
        RtVal = (UINT32)AOUT_ERR_0006;
    } else {
        if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), 0xffffffffU) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagClear fail",__func__, NULL, NULL, NULL, NULL);
            RtVal = (UINT32)AOUT_ERR_0006;
        }
    }

    /* Create event call back Mutex */
    if (RtVal == (UINT32)AOUT_OK) {
        if (AmbaKAL_MutexCreate(&(pAoutHdlr->EventMutex), AoutEventMutexName) != OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Event AmbaKAL_MutexCreate fail",__func__, NULL, NULL, NULL, NULL);
            RtVal = (UINT32)AOUT_ERR_0005;
        } else {
            RtVal = AmbaAOUT_GetMem(pMemInfo, (UINT32)sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create event call back event handler fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAoutHdlr->pEventHandlerCtrl), &pMemAddr);
                RtVal = AmbaWrap_memset(pAoutHdlr->pEventHandlerCtrl, 0, sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT);
            }
        }
    }

    /* Create mix buffer */
    if (RtVal == (UINT32)AOUT_OK) {
        RtVal = AmbaAOUT_GetMem(pMemInfo, pInfo->DmaFrameSize * pInfo->ChannelNum * (UINT32)sizeof(UINT32), &pMemAddr);
        if (RtVal != (UINT32)AOUT_OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create mix buffer fail",__func__, NULL, NULL, NULL, NULL);
        } else {
            AmbaMisra_TypeCast(&(pAoutHdlr->pMixPcmBuf), &pMemAddr);
        }
    }

    /* Create I/O node Mutex */
    if (RtVal == (UINT32)AOUT_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAoutHdlr->IoNodeMutex), AoutIoNodeMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AmbaAOUT_CreateResource] IoNode mutex create fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0006;
        } else {
            pAoutHdlr->IoNodeNum = pInfo->IoNodeNum;
            RtVal = AmbaAOUT_GetMem(pMemInfo, (UINT32)sizeof(AMBA_AOUT_IO_NODE_s) * pInfo->IoNodeNum, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create I/O node fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAoutHdlr->pIoNode), &pMemAddr);
                pIoNode = pAoutHdlr->pIoNode;
                for (Loop = 0; Loop < pInfo->IoNodeNum; Loop++) {
                    pIoNode->pCbHdlr = NULL;
                    pIoNode++;
                }
            }
        }
    }

    return RtVal;
}

static UINT32 AmbaAOUT_CreateResourceDMABuffer(const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                                               AMBA_AOUT_HDLR *pAoutHdlr,
                                               AMBA_AOUT_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AOUT_OK;
    UINT32 *pMemAddr;
    UINT32 Loop;
    ULONG  AlignmentChk;
    UINT32 DmaBufSize;

    for (Loop = 0; Loop < (AMBA_CACHE_LINE_SIZE / sizeof(UINT32)); Loop++) {
        AmbaMisra_TypeCast(&AlignmentChk, &pMemInfo->pHead);
        if ((AlignmentChk & (AMBA_CACHE_LINE_SIZE - 1U)) != 0U) {
            if ((pMemInfo->CurrentSize + sizeof(UINT32)) > pMemInfo->MaxSize) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + (UINT32)sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AOUT_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += (UINT32)sizeof(UINT32);
            }
        } else {
            pAoutHdlr->NumDescr = pInfo->DmaDescNum;
            pAoutHdlr->DmaFrameSize = pInfo->DmaFrameSize;
            pAoutHdlr->ChanNum = pInfo->ChannelNum;
            pAoutHdlr->LastDescr = 0;
            DmaBufSize = pInfo->DmaFrameSize * pInfo->ChannelNum * (UINT32)sizeof(UINT32) * pInfo->DmaDescNum;
            RtVal = AmbaAOUT_GetMem(pMemInfo, DmaBufSize, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA buffer fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAoutHdlr->pDmaBuffer), &pMemAddr);
            }
            break;
        }
    }
    return RtVal;
}

static UINT32 AmbaAOUT_CreateResourceDMADesc(const AMBA_AOUT_BUF_INFO_s *pNonCachedInfo,
                                             const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                                             AMBA_AOUT_HDLR *pAoutHdlr,
                                             AMBA_AOUT_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AOUT_OK;
    UINT32 *pMemAddr;
    UINT32 Loop;
    ULONG AlignmentChk;
    UINT32 DmaDescrSize;

    /* Non cached buffer */
    pMemInfo->CurrentSize = 0U;
    pMemInfo->MaxSize = pNonCachedInfo->MaxSize;
    pMemInfo->pHead = pNonCachedInfo->pHead;

    /* DMA Descriptors, 8 byte alignment for DMA */
    for (Loop = 0; Loop < (8U / sizeof(UINT32)); Loop++) {
        AmbaMisra_TypeCast(&AlignmentChk, &pMemInfo->pHead);
        if ((AlignmentChk & 0x7U) != 0U) {
            if ((pMemInfo->CurrentSize + sizeof(UINT32)) > pMemInfo->MaxSize) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][non-Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + (UINT32)sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AOUT_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += (UINT32)sizeof(UINT32);
            }
        } else {
            break;
        }
    }

    if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
        AmbaMisra_TypeCast(&AlignmentChk, &pMemInfo->pHead);
        if ((AlignmentChk & 0x7U) != 0U) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Not 8-ByteAlignment address: 0x%x", (UINT32)AlignmentChk, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0003;
        } else {
            DmaDescrSize = (UINT32)sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum;
            RtVal = AmbaAOUT_GetMem(pMemInfo, DmaDescrSize, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA descriptor fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAoutHdlr->pDesc), &pMemAddr);
            }
        }
    }

    /* DMA Descriptors report */
    if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
        RtVal = AmbaAOUT_GetMem(pMemInfo, (UINT32)sizeof(UINT32) * pInfo->DmaDescNum, &pMemAddr);
        if (RtVal != (UINT32)AOUT_OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA descriptor report fail",__func__, NULL, NULL, NULL, NULL);
        } else {
            AmbaMisra_TypeCast(&(pAoutHdlr->pReport), &pMemAddr);
        }
    }
    return RtVal;
}

/**
* Audio output resource creation function
* @param [in]  pInfo Audio output create information
* @param [in]  pCachedInfo Cached buffer information
* @param [in]  pNonCachedInfo Non-cached buffer information
* @param [out]  pHdlr Pointer of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_CreateResource(const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                               const AMBA_AOUT_BUF_INFO_s *pCachedInfo,
                               const AMBA_AOUT_BUF_INFO_s *pNonCachedInfo,
                               UINT32 **pHdlr)
{
    UINT32 RtVal = (UINT32)AOUT_OK;
    AMBA_AOUT_MEM_INFO_s MemInfo;
    AMBA_AOUT_HDLR *pAoutHdlr = NULL;
    UINT32 *pMemAddr;
    UINT32 TxChannel = AMBA_DMA_CHANNEL_I2S_TX;

    if ((pInfo != NULL) && (pCachedInfo != NULL) && (pNonCachedInfo != NULL)) {
        /* Cached buffer */
        MemInfo.CurrentSize = 0U;
        MemInfo.MaxSize = pCachedInfo->MaxSize;
        MemInfo.pHead = pCachedInfo->pHead;

        if (pInfo->HwIndex == (UINT32)AOUT_I2S_0) {
            TxChannel = AMBA_DMA_CHANNEL_I2S_TX;
        } else if (pInfo->HwIndex == (UINT32)AOUT_I2S_1) {
            TxChannel = AMBA_DMA_CHANNEL_I2S1_TX;
        } else {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_CreateResource] Wrong HwIndex setting: %d", pInfo->HwIndex, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0000;
        }

        /* create AOUT handler */
        if (RtVal == (UINT32)AOUT_OK) {
            RtVal = AmbaAOUT_GetMem(&(MemInfo), (UINT32)sizeof(AMBA_AOUT_HDLR), &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create AOUT handler fail",__func__, NULL, NULL, NULL, NULL);
                *pHdlr = NULL;
            } else {
                AmbaMisra_TypeCast(&pAoutHdlr, &pMemAddr);
                *pHdlr = pMemAddr;
                pAoutHdlr->HwIndex = pInfo->HwIndex;
            }
        }

        /* create the Audio task related info */
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            RtVal = AmbaAOUT_CreateRescSysInfo(pInfo, pAoutHdlr, &MemInfo);
        }

        /* DMA Buffer, AMBA_CACHE_LINE_SIZE byte alignment for data cache operations */
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            RtVal = AmbaAOUT_CreateResourceDMABuffer(pInfo, pAoutHdlr, &MemInfo);
        }

        /* DMA Tx channel allocation */
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            RtVal = AmbaDMA_ChannelAllocate(TxChannel, &(pAoutHdlr->TxDmaChanNo));
            if (RtVal != (UINT32)AOUT_OK) {
                RtVal = (UINT32)AOUT_ERR_0007;
            }
        }

        if (RtVal == (UINT32)AOUT_OK) {
            RtVal = AmbaAOUT_CreateResourceDMADesc(pNonCachedInfo, pInfo, pAoutHdlr, &MemInfo);
        }

        if (pAoutHdlr != NULL) {
            pAoutHdlr->ErrorCode = RtVal;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = (UINT32)AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_CreateResource End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output resource delete function
* @param [in]  pHdlr Handle of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_DeleteResource(const UINT32 *pHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = AOUT_OK;

        /* DMA Rx channel release */
        SubRtVal = AmbaDMA_ChannelRelease(pAoutHdlr->TxDmaChanNo);
        if (SubRtVal != DMA_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaDMA_ChannelRelease fail",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = AOUT_ERR_0007;
        }

        if (pAoutHdlr->ErrorCode == AOUT_OK) {
            SubRtVal = AmbaKAL_EventFlagDelete(&(pAoutHdlr->Flag));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_DeleteResource] event flag delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0006;
            }
        }

        if (pAoutHdlr->ErrorCode == AOUT_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAoutHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_DeleteResource] event mutex delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }

        /* delete I/O node Mutex */
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_DeleteResource] ionode mutex delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }

        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_DeleteResource End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

static UINT32 AmbaAOUT_CheckDMA(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 *pDmaCount, UINT32 *pDmaStop)
{
    UINT32 LookAhead, Loop = 0, SubRtVal;
    volatile UINT32 *pRpt;

    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
    *pDmaStop = 0;
    *pDmaCount = 0;

    SubRtVal = AmbaDMA_Wait(pAoutHdlr->TxDmaChanNo, AMBA_AOUT_TIMEOUT);
    if (SubRtVal != DMA_ERR_NONE) { /* Wait for SDK7 API */
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_CheckDMA] DMA wait fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        pAoutHdlr->ErrorCode = AOUT_ERR_0007;
    }
    if (pAoutHdlr->ErrorCode == AOUT_OK) {
        do {
            pRpt = &pAoutHdlr->pReport[pAoutHdlr->LastDescr];
            if ((*pRpt & (UINT32)0x08000000U) != 0U) {
                break;
            }
            Loop++;
            if (Loop > 5U) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_CheckDMA  Wait over time, Loop = %d", Loop, 0U, 0U, 0U, 0U);
                SubRtVal = AmbaKAL_TaskSleep(1);
            }
            if (Loop > 10U) {
                pAoutHdlr->ErrorCode = AOUT_ERR_0007;
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_CheckDMA  Wait over time, Loop = %d, Rtval: 0x%x", Loop, pAoutHdlr->ErrorCode, 0U, 0U, 0U);
            }
        } while (((*pRpt & (UINT32)0x08000000U) == 0U) &&
                 (pAoutHdlr->ErrorCode == AOUT_OK) &&
                 (SubRtVal == OK));

        for(LookAhead = 0; LookAhead < pAoutHdlr->NumDescr; LookAhead++) {
            pRpt = &pAoutHdlr->pReport[pAoutHdlr->LastDescr];
            if ((*pRpt & (UINT32)0x08000000U) == 0U) {
                break;
            }
            if ((*pRpt & (UINT32)0x08000000U) != 0U) {
                *pRpt &= (~0x08000000U);
                *pDmaCount = *pDmaCount + 1U;
                if ((*pRpt & (UINT32)0x10000000U) != 0U) {
                    *pRpt &= (~0x10000000U);
                    *pDmaStop = 1U;
                }
                if (++pAoutHdlr->LastDescr >= pAoutHdlr->NumDescr) {
                    pAoutHdlr->LastDescr = 0U;
                }
                pRpt = &pAoutHdlr->pReport[pAoutHdlr->LastDescr];
            }
        }
    }
    /*AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "  End!!  ErrorCode=%d", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);*/
    return pAoutHdlr->ErrorCode;
}

static UINT32 AmbaAOUT_GiveEvent(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 EventId, void *pEventInfo)
{
    const AMBA_AOUT_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    const AMBA_AOUT_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k, SubRtVal;

    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
    if (EventId >= (UINT32)AMBA_AOUT_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]EventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AOUT_NUM_EVENT, 0U, 0U, 0U);
        pAoutHdlr->ErrorCode = AOUT_ERR_0004;
    }
    if (pAoutHdlr->ErrorCode == AOUT_OK) {
        /* Take the Mutex */
        SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->EventMutex), AMBA_AOUT_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_GiveEvent] take mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0005;
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            pEventHandlerCtrl = &pAoutHdlr->pEventHandlerCtrl[EventId];/*pAoutHdlr->pEventHandlerCtrl + EventId;*/
            k = pEventHandlerCtrl->MaxNumHandler;
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler != NULL) {
                    (void)(*pWorkEventHandler)(pEventInfo);   /* invoke the Event Handler */
                }
                pWorkEventHandler++;
            }
            /* Release the Mutex */
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_GiveEvent] give mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }
    }
    AmbaMisra_TouchUnused(pEventInfo);
    return pAoutHdlr->ErrorCode;
}

static UINT32 AmbaAOUT_CheckStatus(AMBA_KAL_EVENT_FLAG_t *pFlag, UINT32 StatusFlag, UINT32 *pActualFlags)
{
    UINT32 RtVal = AOUT_OK, SubRtVal;

    *pActualFlags = 0;
    SubRtVal = AmbaKAL_EventFlagGet(pFlag, StatusFlag, 0U/*or*/, 0U/*not clear*/,
                                    pActualFlags, (UINT32)AMBA_KAL_NO_WAIT);
    if (SubRtVal == KAL_ERR_NONE) {
        *pActualFlags &= StatusFlag;
    } else {
        *pActualFlags = 0U;
    }

    return RtVal;
}

static UINT32 Audio_Qadd32(UINT32 Data1, UINT32 Data2)
{
#ifdef USE_ASM
    UINT32 Data3;
    __asm__ ("QADD %0, %1, %2\n":"=r"(Data3):"r"(Data1),"r"(Data2));
    AmbaMisra_TouchUnused(&Data1);
    AmbaMisra_TouchUnused(&Data2);
#else
    UINT32 Data3;
    INT32 DataS1, DataS2, DataS3;
    INT64 Sum;

    AmbaMisra_TypeCast32(&DataS1, &Data1);
    AmbaMisra_TypeCast32(&DataS2, &Data2);

    Sum = (INT64)DataS1 + (INT64)DataS2;

    if(Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x7fffffff;
    } else if(-Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x80000000U;
    } else {
        DataS3 = (INT32)Sum;
    }
    AmbaMisra_TypeCast32(&Data3, &DataS3);
#endif
    return Data3;
}

/* Only support 32bit data mixer */
static void AmbaAOUT_MixPCM(const UINT32 *pDst, const UINT32 *pSrc, UINT32 Size)
{
    UINT32 Loop;
    UINT32 Data1 = 0U, Data2 = 0U, Data3 = 0U;
    const UINT32 *pSrc32;
    UINT32 *pDst32;

    AmbaMisra_TypeCast(&pSrc32, &pSrc);
    AmbaMisra_TypeCast(&pDst32, &pDst);
    for (Loop = 0; Loop < Size; Loop++) {
        Data1 = pDst32[0];
        Data2 = pSrc32[0];
        pSrc32++;
        Data3 = Audio_Qadd32(Data1, Data2);
        *pDst32 = Data3;
        pDst32++;
    }
}

static UINT32 AmbaAOUT_UnderFlowCheck(AMBA_AOUT_HDLR *pAoutHdlr, const AMBA_AOUT_IO_NODE_s *pIoNode, UINT32 PreFillState, UINT32 Loop)
{
    UINT32 *pHdlr;
    AMBA_AOUT_EVENT_INFO_s EventInfo;

    if (PreFillState == 0U) {
        if (((pAoutHdlr->FrameIn > pAoutHdlr->FrameOut) &&
                ((pAoutHdlr->FrameIn - pAoutHdlr->FrameOut) ==
                 (pAoutHdlr->NumDescr - 1U))) ||
                ((pAoutHdlr->FrameOut > pAoutHdlr->FrameIn) &&
                 ((pAoutHdlr->FrameOut - pAoutHdlr->FrameIn) == 1U))) {
            /* Overflow checking, give event */
            AmbaMisra_TypeCast(&pHdlr, &pAoutHdlr);
            EventInfo.pHdlr = pHdlr;
            EventInfo.pCbHdlr = pIoNode->pCbHdlr;
            pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_UNDERFLOW, &EventInfo);
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_UNDERFLOW), ret (0x%x), i/o (%d), FI (%d), FO (%d)", pAoutHdlr->ErrorCode, Loop, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut, 0U);
        }
    }
    return pAoutHdlr->ErrorCode;
}

static void AmbaAOUT_ScanInputChainGetSize(AMBA_AOUT_HDLR *pAoutHdlr, const AMBA_AOUT_IO_NODE_s *pIoNode, UINT32 *pRemainSize, UINT32 Loop)
{
    if (pIoNode->pCbHdlr->GetSize != NULL) {
        if (pIoNode->pCbHdlr->GetSize(pRemainSize) != (UINT32)AOUT_OK) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]I/O node %d]callback GetSize fail!!  RemainSize=%d", Loop, *pRemainSize, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0008;
            //continue;
        } else {
            pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        }
    }
}

static void AmbaAOUT_ScanInputChainImp(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 *pStopFrame, UINT32 *pLof, UINT32 PreFillState, UINT32 *pConnectAbuNum)
{
    UINT32  DmaBufOffset;
    UINT32  DmaSize;
    UINT32  Loop;
    UINT32  RemainSize = 0U;
    UINT32  *pDmaBuffer;
    UINT32  *pPcmBuf;
    UINT32  FirstNode = 1U;
    ULONG   Reg;
    UINT32  IoNodeLof = 0U;
    UINT32  LofCounter = 0U;
    UINT32  *pHdlr;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    AMBA_AOUT_EVENT_INFO_s EventInfo;
    UINT32 SubRtVal;

    DmaBufOffset = pAoutHdlr->DmaFrameSize * pAoutHdlr->ChanNum;
    DmaSize = DmaBufOffset * (UINT32)sizeof(UINT32);
    pDmaBuffer = &pAoutHdlr->pDmaBuffer[pAoutHdlr->FrameIn * DmaBufOffset];
    SubRtVal = AmbaWrap_memset(pDmaBuffer, 0, DmaSize);
    if (SubRtVal == OK) {
        /* Scan input chain */
        pPcmBuf = pDmaBuffer;
        pIoNode = pAoutHdlr->pIoNode;
        for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
            IoNodeLof = 0U;
            if (pIoNode->NodeStatus == (UINT32)AOUT_IONODE_OPERATION) {
                (*pConnectAbuNum)++;
                if (pIoNode->BuffStatus == (UINT32)AOUT_CB_NORMAL) {
                    AmbaAOUT_ScanInputChainGetSize(pAoutHdlr, pIoNode, &RemainSize, Loop);
                    //AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT [I/O node %d]callback GetPcm %d", Loop, RemainSize, 0U, 0U, 0U);
                    if ((pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) && (RemainSize > 0U)) {
                        if (pIoNode->pCbHdlr->GetPcm != NULL) {
                            if (pIoNode->pCbHdlr->GetPcm(&pPcmBuf, DmaSize, &IoNodeLof) != (UINT32)AOUT_OK) {
                                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][I/O node %d]callback GetPcm (DmaSize) fail!!", Loop, 0U, 0U, 0U, 0U);
                                pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0008;
                                continue;
                            } else {
                                if (FirstNode == 0U) {
                                    AmbaAOUT_MixPCM(pDmaBuffer, pPcmBuf, DmaBufOffset);
                                } else {
                                    FirstNode = 0U;
                                    pPcmBuf = pAoutHdlr->pMixPcmBuf;
                                }
                            }
                        }
                        /* Lof check */
                        if (IoNodeLof == 1U) {
                            LofCounter++;
                            pIoNode->NodeStatus = (UINT32)AOUT_IONODE_PAUSE;
                            AmbaMisra_TypeCast(&pHdlr, &pAoutHdlr);
                            EventInfo.pHdlr = pHdlr;
                            EventInfo.pCbHdlr = pIoNode->pCbHdlr;
                            pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_GET_LOF, &EventInfo);
                            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_GET_LOF), ret (0x%x), i/o (%d)", pAoutHdlr->ErrorCode, Loop, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT [I/O node %d]callback GetPcm zero", Loop, 0U, 0U, 0U, 0U);
                    }
                    if (pAoutHdlr->ErrorCode == AOUT_OK) {
                        pAoutHdlr->ErrorCode = AmbaAOUT_UnderFlowCheck(pAoutHdlr, pIoNode, PreFillState, Loop);
                    }
                }
            }
            pIoNode++;
        }
        if ((LofCounter == *pConnectAbuNum) && (LofCounter > 0U)) {
            *pLof = 1U;
        }

        AmbaMisra_TypeCast(&Reg, &pDmaBuffer);
        SubRtVal = AmbaCache_DataClean(Reg, DmaBufOffset * sizeof(UINT32));
        if (SubRtVal != OK) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT AmbaCache_DataClean fail, ret (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }
        *pStopFrame = pAoutHdlr->FrameIn;
        if (++(pAoutHdlr->FrameIn) >=  pAoutHdlr->NumDescr) {
            pAoutHdlr->FrameIn = 0U;
        }
    }
}


static UINT32 AmbaAOUT_ScanInputChain(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 *pLof, UINT32 PreFillState, UINT32 *pStopFrame)
{
    UINT32   ConnectAbuNum = 0U;
    UINT32   SubRtVal;
    UINT32 ActualFlags;

    SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->IoNodeMutex), AMBA_AOUT_TIMEOUT);
    if (SubRtVal != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_ScanInputChain] take mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0005;
    } else {
        AmbaAOUT_ScanInputChainImp(pAoutHdlr, pStopFrame, pLof, PreFillState, &ConnectAbuNum);
        if (ConnectAbuNum == 0U) {
            SubRtVal = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_NOWOKIOSET, &ActualFlags);
            if (SubRtVal == OK) {
                if ((ActualFlags & AMBA_AOUT_FLG_NOWOKIOSET) == 0U) {
                    pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_NO_WORK_IO, pAoutHdlr);
                    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_NO_WORK_IO), ret (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                    if (AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_NOWOKIOSET) != KAL_ERR_NONE) {
                        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_NOWOKIOSET) AmbaKAL_EventFlagSet fail!!",__func__, NULL, NULL, NULL, NULL);
                        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
                    }
                }
            }
        }

        if (pAoutHdlr->ErrorCode == AOUT_OK) {
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_ScanInputChain] give mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0005;
            }
        }
    }
    //AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ScanInputChain: Err(0x%x), Lof(%d), Prefill_state(%d), FI(%d) FO(%d)", pAoutHdlr->ErrorCode, *pLof, PreFillState, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut);
    return pAoutHdlr->ErrorCode;
}

static UINT32 AmbaAOUT_ClearFlags(AMBA_AOUT_HDLR *pAoutHdlr)
{
    if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOP) != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_STOP) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
    }
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_EOCSET) != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_EOCSET) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
                    pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
        }
    }
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_NOWOKIOSET) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_NOWOKIOSET) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
        }
    }
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_PAUSE) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_PAUSE) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
        }
    }
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (AmbaKAL_EventFlagClear(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOPWITHLOF) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_STOPWITHLOF) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
        }
    }

    return pAoutHdlr->ErrorCode;
}

/**
* Audio output initialize and prepare the audio output process function
* @param [in]  pHdlr Handle of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_Prepare(const UINT32 *pHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 Loop;
    AMBA_DMA_DESC_s *pDesc;
    AMBA_DMA_DESC_CTRL_s DmaDescCtrl;
    UINT32 *pDmaBuffer;
    UINT32 *pTmp;
    UINT32 *pReport;
    UINT32 TransferCount;
    UINT32 DmaBufOffset;
    UINT32 I2sCh = 0;
    void   *pI2sTxDmaAddr;
    UINT32 StopFrame;
    UINT32 SubRtVal;

    AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
    pAoutHdlr->ErrorCode = AmbaAOUT_ClearFlags(pAoutHdlr);

    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (pAoutHdlr->HwIndex == (UINT32)AOUT_I2S_0) {
            I2sCh = (UINT32)AMBA_I2S_CHANNEL0;
        } else if (pAoutHdlr->HwIndex == (UINT32)AOUT_I2S_1) {
            I2sCh = (UINT32)AMBA_I2S_CHANNEL1;
        } else {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_Prepare] Wrong HwIndex setting: %d", pAoutHdlr->HwIndex, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0000;
        }
    }

    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        DmaDescCtrl.StopOnError = 1U;
        DmaDescCtrl.IrqOnError = 1U;
        DmaDescCtrl.IrqOnDone = 1U;
        DmaDescCtrl.Reserved0 = 0U;
        DmaDescCtrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
        DmaDescCtrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
        DmaDescCtrl.NoBusAddrInc = 1U;
        DmaDescCtrl.ReadMem = 1U;
        DmaDescCtrl.WriteMem = 0U;
        DmaDescCtrl.EndOfChain = 0U;
        DmaDescCtrl.Reserved1 = 0U;

        pDesc = pAoutHdlr->pDesc;
        pDmaBuffer = pAoutHdlr->pDmaBuffer;
        pReport = pAoutHdlr->pReport;
        DmaBufOffset = pAoutHdlr->DmaFrameSize * pAoutHdlr->ChanNum;
        TransferCount = DmaBufOffset * (UINT32)sizeof(UINT32);
        SubRtVal = AmbaWrap_memset(pReport, 0, sizeof(UINT32) * pAoutHdlr->NumDescr);
        if (SubRtVal == OK) {
            SubRtVal = AmbaRTSL_I2sTxGetDmaAddress(I2sCh, &pI2sTxDmaAddr);
            if (SubRtVal == OK) {
                for (Loop = 0U; Loop < (pAoutHdlr->NumDescr - 1U); Loop++) {
                    pDesc->pSrcAddr = pDmaBuffer;
                    pDesc->pDstAddr = pI2sTxDmaAddr;
                    pDesc->pNextDesc = &pDesc[1];
                    pDesc->pStatus = pReport;
                    SubRtVal = AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
                    pDesc->DataSize = TransferCount;
                    pDesc++;
                    pReport++;
                    pTmp = pDmaBuffer;
                    pDmaBuffer = &pTmp[DmaBufOffset];
                }
                if (SubRtVal == OK) {
                    pDesc->pSrcAddr = pDmaBuffer;
                    pDesc->pDstAddr = pI2sTxDmaAddr;
                    pDesc->pNextDesc = pAoutHdlr->pDesc;
                    pDesc->pStatus = pReport;
                    SubRtVal = AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
                    pDesc->DataSize = TransferCount;
                    if (SubRtVal != OK) {
                        pAoutHdlr->ErrorCode = AOUT_ERR_0006;
                    }
                }
            }
        }
    }
    pAoutHdlr->FrameIn  = 0U;
    pAoutHdlr->FrameOut = 0U;
    pAoutHdlr->ScanInputChainLof = 0U;
    pAoutHdlr->DmaStop = 0U;

    /* Pre-fill Output DMA buffer */
    /*pDmaBuffer = pAoutHdlr->pDmaBuffer;*/
    /*AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "  AOUT Pre-fill process!!!", NULL, NULL, NULL, NULL, NULL);*/
    for (Loop = 0; Loop < pAoutHdlr->NumDescr; Loop++) {
        pAoutHdlr->ErrorCode = AmbaAOUT_ScanInputChain(pAoutHdlr, &pAoutHdlr->ScanInputChainLof, 1U, &StopFrame);
        if ((pAoutHdlr->ErrorCode != (UINT32)AOUT_OK) || (pAoutHdlr->ScanInputChainLof == 1U)) {
            break;
        }
    }

    if (AmbaDMA_Transfer(pAoutHdlr->TxDmaChanNo, pAoutHdlr->pDesc) != OK) {  /*Wait for SDK7 DMA definition */
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AmbaDMA_Transfer) fail!!",__func__, NULL, NULL, NULL, NULL);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0007;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_Prepare End: ret: (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);
    return pAoutHdlr->ErrorCode;
}

static UINT32 AmbaAOUT_SetEocFrame(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 StopFrame)
{
    UINT32 ActualFlags;

    pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_EOCSET, &ActualFlags);
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if ((ActualFlags & AMBA_AOUT_FLG_EOCSET) == 0U) {
            pAoutHdlr->pDesc[StopFrame].Ctrl.EndOfChain = 1U;
            if (AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_EOCSET) != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_EOCSET) AmbaKAL_EventFlagSet fail!!",__func__, NULL, NULL, NULL, NULL);
                pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
            }
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_SetEocFrame: ret (0x%x) Stop(%d) FI(%d) FO(%d)", pAoutHdlr->ErrorCode, StopFrame, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut, 0U);
        }
    }
    return pAoutHdlr->ErrorCode;
}

static void AmbaAOUT_ProcDmaCheckLoop(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 DmaCount)
{
    UINT32  Loop, StopFrame, SubRtVal;
    UINT32  ActualFlags;
    UINT32  *pDmaBuffer;
    UINT32  DmaBufOffset;
    ULONG   Reg;
    const UINT32 *pTmp;

    for (Loop = 0U; Loop < DmaCount; Loop++) {
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_PAUSE, &ActualFlags);
            if (pAoutHdlr->ErrorCode == AOUT_OK) {
                if ((ActualFlags & AMBA_AOUT_FLG_PAUSE) != 0U) {
                    StopFrame = (pAoutHdlr->FrameOut + 2U) % pAoutHdlr->NumDescr; /* 2 is safe for DMA stop */
                    pAoutHdlr->ErrorCode = AmbaAOUT_SetEocFrame(pAoutHdlr, StopFrame);
                } else {
                    pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOP, &ActualFlags);
                    if (pAoutHdlr->ErrorCode == AOUT_OK) {
                        if ((ActualFlags & AMBA_AOUT_FLG_STOP) == 0U) {
                            pAoutHdlr->ErrorCode = AmbaAOUT_ScanInputChain(pAoutHdlr, &pAoutHdlr->ScanInputChainLof, 0U, &StopFrame);
                            if ((pAoutHdlr->ScanInputChainLof == 1U) && (pAoutHdlr->ErrorCode == AOUT_OK)) {
                                pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOPWITHLOF, &ActualFlags);
                                if (pAoutHdlr->ErrorCode == AOUT_OK) {
                                    if ((ActualFlags & AMBA_AOUT_FLG_STOPWITHLOF) != 0U) {
                                        pAoutHdlr->ErrorCode = AmbaAOUT_SetEocFrame(pAoutHdlr, StopFrame);
                                    }
                                }
                            }
                        } else {
                            StopFrame = pAoutHdlr->FrameIn;
                            DmaBufOffset = pAoutHdlr->DmaFrameSize * pAoutHdlr->ChanNum;
                            pDmaBuffer = pAoutHdlr->pDmaBuffer;
                            SubRtVal = AmbaWrap_memset(&pDmaBuffer[(StopFrame * DmaBufOffset)], 0, DmaBufOffset * sizeof(UINT32));
                            if (SubRtVal == OK) {
                                pTmp = &pDmaBuffer[StopFrame * DmaBufOffset];
                                AmbaMisra_TypeCast(&Reg, &pTmp);
                                SubRtVal = AmbaCache_DataClean(Reg, DmaBufOffset * sizeof(UINT32));
                                if (SubRtVal != OK) {
                                    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT AmbaCache_DataClean fail, ret (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                                }
                                pAoutHdlr->ErrorCode = AmbaAOUT_SetEocFrame(pAoutHdlr, StopFrame);
                            }
                        }
                    }
                }
            }
            if (++(pAoutHdlr->FrameOut) >=  pAoutHdlr->NumDescr) {
                pAoutHdlr->FrameOut = 0U;
            }
        } else {
            break;
        }
    }
}

/**
* Audio output main function
* @param [in]  pHdlr Handle of the audio output resource
* @param [out]  pLof Pointer of the last of frame flag
* @return ErrorCode
*/
UINT32 AmbaAOUT_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 DmaCount, DmaStop, SubRtVal;
    void *pEventInfo;

    AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;

    if (pAoutHdlr->DmaStop == 0U) {
        /* Check DMA interrupts */
        pAoutHdlr->ErrorCode = AmbaAOUT_CheckDMA(pAoutHdlr, &DmaCount, &DmaStop);
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (DmaCount == pAoutHdlr->NumDescr) {
                AmbaMisra_TypeCast(&pEventInfo, &pAoutHdlr);
                pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_OVERTIME, pEventInfo);
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_OVERTIME), ret (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);
            }
            if (DmaStop == 0U) {
                AmbaAOUT_ProcDmaCheckLoop(pAoutHdlr, DmaCount);
            } else {
                *pLof = 1U;
                /* Disable I2S TX Outside */
                if (++(pAoutHdlr->FrameOut) >=  pAoutHdlr->NumDescr) {
                    pAoutHdlr->FrameOut = 0U;
                }
                /* Give DMA Stop Event */
                AmbaMisra_TypeCast(&pEventInfo, &pAoutHdlr);
                pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, pEventInfo);
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_STOP), ret (0x%x), FI(%d) FO(%d)", pAoutHdlr->ErrorCode, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut, 0U, 0U);
                pAoutHdlr->DmaStop = 1U;
                /* Suspend task Outside */
            }
        }
    } else {
        SubRtVal = AmbaKAL_TaskSleep(10000);
        if (SubRtVal != OK) {
            pAoutHdlr->ErrorCode = AOUT_ERR_0006;
        }
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA: DMA is stopped: %d", pAoutHdlr->DmaStop, 0U, 0U, 0U, 0U);
    }

    /*AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA End!!, ErrorCode=%d, DmaStop: %d", pAoutHdlr->ErrorCode, pAoutHdlr->DmaStop, 0U, 0U, 0U);*/
    return pAoutHdlr->ErrorCode;
}

/**
* Audio output stop function
* @param [in]  pHdlr Handle of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_Stop(const UINT32 *pHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOP);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_Stop: set AMBA_AOUT_FLG_STOP failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0007;
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_Stop End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output stop until get Lof function
* @param [in]  pHdlr Handle of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_StopWithLof(const UINT32 *pHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOPWITHLOF);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_StopWithLof: set AMBA_AOUT_FLG_STOPWITHLOF failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0007;
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_StopWithLof End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output pause function
* @param [in]  pHdlr Handle of the audio output resource
* @return ErrorCode
*/
UINT32 AmbaAOUT_Pause(const UINT32 *pHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_PAUSE);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_Pause: set AMBA_AOUT_FLG_PAUSE failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0007;
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_Pause End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output register the input chain scanner callback functions
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAOUT_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 Loop = 0U;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->IoNodeMutex), AMBA_AOUT_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_RegisterCallBackFunc: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0005;
        } else {
            pIoNode = pAoutHdlr->pIoNode;
            for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == NULL) {
                    pIoNode->pCbHdlr = pCbHdlr;
                    pIoNode->NodeStatus = (UINT32)AOUT_IONODE_PAUSE;
                    pIoNode->BuffStatus = (UINT32)AOUT_CB_NORMAL;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_RegisterCallBackFunc: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (Loop == pAoutHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][Register] out of the max I/O node number: %d", pAoutHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0004;
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }


    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_RegisterCallBackFunc End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output de-register the input chain scanner callback functions
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAOUT_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 Loop= 0U;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->IoNodeMutex), AMBA_AOUT_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_DeRegisterCallBackFunc: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0005;
        } else {
            pIoNode = pAoutHdlr->pIoNode;
            for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->pCbHdlr = NULL;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_DeRegisterCallBackFunc: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (Loop == pAoutHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][DeRegister] out of the max I/O node number: %d", pAoutHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0004;
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_DeRegisterCallBackFunc End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output open the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAOUT_OpenIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AOUT_HDLR *pAoutHdlr;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->IoNodeMutex), AMBA_AOUT_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_OpenIoNode: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0005;
        } else {
            pIoNode = pAoutHdlr->pIoNode;
            for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AOUT_IONODE_OPERATION;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_OpenIoNode: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (Loop == pAoutHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][OpenIoNode] out of the max I/O node number: %d", pAoutHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0004;
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_OpenIoNode End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output close the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAOUT_CloseIoNode(const UINT32 *pHdlr, const AMBA_AOUT_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AOUT_HDLR *pAoutHdlr;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->IoNodeMutex), AMBA_AOUT_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_CloseIoNode: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = AOUT_ERR_0005;
        } else {
            pIoNode = pAoutHdlr->pIoNode;
            for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AOUT_IONODE_PAUSE;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_CloseIoNode: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (Loop == pAoutHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][CloseIoNode] out of the max I/O node number: %d", pAoutHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0004;
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_CloseIoNode End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio output configure control Settings for the AOUT event handler
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  EventId Event ID of the AOUT event
* @param [in]  MaxNumHandler Max number of Handlers
* @param [in]  pEventHandlers Pointer of the Event Handlers
* @return ErrorCode
*/
UINT32 AmbaAOUT_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                UINT32 MaxNumHandler,
                                AMBA_AOUT_EVENT_HANDLER_f *pEventHandlers)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    AMBA_AOUT_EVENT_HANDLER_CTRL_s *pWorkEventHandler;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if ((pHdlr != NULL) && (pEventHandlers != NULL) && (MaxNumHandler != 0U)) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        if (EventId >= (UINT32)AMBA_AOUT_NUM_EVENT) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Config eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AOUT_NUM_EVENT, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0004;
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->EventMutex), AMBA_AOUT_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ConfigEventHdlr: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
            if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
                pWorkEventHandler = &pAoutHdlr->pEventHandlerCtrl[EventId];
                pWorkEventHandler->MaxNumHandler = MaxNumHandler;   /* maximum number of Handlers */
                pWorkEventHandler->pEventHandler = pEventHandlers;  /* pointer to the Event Handlers */
                /* Release the Mutex */
                SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ConfigEventHdlr: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    pAoutHdlr->ErrorCode = AOUT_ERR_0005;
                }
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        RtVal = AOUT_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                                   "[AUD]AmbaAOUT_ConfigEventHdlr error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_ConfigEventHdlr End: ret: (0x%x), EventId(%d), MaxNumHandler(%d)",
                               RtVal,
                               EventId,
                               MaxNumHandler, 0U, 0U);

    return RtVal;
}

/**
* Audio output register a specified audio event handler
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  EventId Event ID of the AOUT event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAOUT_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                  AMBA_AOUT_EVENT_HANDLER_f EventHandler)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    const AMBA_AOUT_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AOUT_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;

    /*AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "Func: [%s]",__func__, NULL, NULL, NULL, NULL);*/
    /*AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "  Start!!", NULL, NULL, NULL, NULL, NULL);*/
    AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
    if (EventId >= (UINT32)AMBA_AOUT_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Register eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AOUT_NUM_EVENT, 0U, 0U, 0U);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0004;
    }
    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&(pAoutHdlr->EventMutex), AMBA_AOUT_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0005;
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            pEventHandlerCtrl = &pAoutHdlr->pEventHandlerCtrl[EventId];
            k = pEventHandlerCtrl->MaxNumHandler;

            /* check to see if it is already registered */
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler == EventHandler) {
                    AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Event already registered.",__func__, NULL, NULL, NULL, NULL);
                    pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0000;  /* it is already registered, why ? */
                }
                pWorkEventHandler++;
            }
            if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
                /* check to see if there is a room for this Handler */
                pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
                for (i = 0; i < k; i++) {
                    if (*pWorkEventHandler == NULL) {
                        *pWorkEventHandler = EventHandler;      /* register this Handler */
                        break;
                    }
                    pWorkEventHandler++;
                }
            }
            /* Release the Mutex */
            if (AmbaKAL_MutexGive(&(pAoutHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0005;
            }
        }
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_RegisterEventHdlr End: ret: (0x%x), EventId(%d)", pAoutHdlr->ErrorCode, EventId, 0U, 0U, 0U);
    return pAoutHdlr->ErrorCode;
}

/**
* Audio output de-register a specified audio event handler
* @param [in]  pHdlr Handle of the audio output resource
* @param [in]  EventId Event ID of the AOUT event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAOUT_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                    AMBA_AOUT_EVENT_HANDLER_f EventHandler)
{
    AMBA_AOUT_HDLR *pAoutHdlr;
    const AMBA_AOUT_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AOUT_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 RtVal = AOUT_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAoutHdlr, &pHdlr);
        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
        if (EventId >= (UINT32)AMBA_AOUT_NUM_EVENT) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]DeRegister eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AOUT_NUM_EVENT, 0U, 0U, 0U);
            pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0004;
        }
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAoutHdlr->EventMutex), AMBA_AOUT_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_DeRegisterEventHdlr: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAoutHdlr->ErrorCode = AOUT_ERR_0005;
            }
            if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
                pEventHandlerCtrl = &pAoutHdlr->pEventHandlerCtrl[EventId];
                k = pEventHandlerCtrl->MaxNumHandler;
                /* check to see if it is already registered */
                pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
                for (i = 0; i < k; i++) {
                    if (*pWorkEventHandler == EventHandler) {
                        *pWorkEventHandler = NULL;      /* unregister here */
                    }
                    pWorkEventHandler++;
                }
                /* Release the Mutex */
                SubRtVal = AmbaKAL_MutexGive(&(pAoutHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_DeRegisterEventHdlr: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    pAoutHdlr->ErrorCode = AOUT_ERR_0005;
                }
            }
        }
        RtVal = pAoutHdlr->ErrorCode;
    } else {
        RtVal = AOUT_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                                   "[AUD]AmbaAOUT_DeRegisterEventHdlr error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_DeRegisterEventHdlr End: ret: (0x%x), EventId(%d)", RtVal, EventId, 0U, 0U, 0U);

    return RtVal;
}

