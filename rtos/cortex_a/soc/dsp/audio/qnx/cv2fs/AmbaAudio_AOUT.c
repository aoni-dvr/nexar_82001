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
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaMisraFix.h"
#ifdef CONFIG_THREADX
#include "AmbaRTSL_I2S.h"
#include "AmbaI2S.h"
#include "AmbaDMA.h"
#include "AmbaCache.h"
#endif
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
        pInfo->CachedBufSize = sizeof(AMBA_AOUT_HDLR) +
                               (sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT) +
                               (pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32)) + /*Mix Buf*/
                               (sizeof(AMBA_AOUT_IO_NODE_s) * pInfo->IoNodeNum) +
                               AMBA_CACHE_LINE_SIZE + /* AMBA_CACHE_LINE_SIZE byte alignment padding */
                               (pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32) * pInfo->DmaDescNum);
#ifdef CONFIG_THREADX
        pInfo->NonCachedBufSize = 8U + /* 8 byte alignment padding */
                                  (sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum) +
                                  (sizeof(UINT32) * pInfo->DmaDescNum);
#else
        pInfo->NonCachedBufSize = 0U;
#endif
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
            RtVal = AmbaAOUT_GetMem(pMemInfo, sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create event call back event handler fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAoutHdlr->pEventHandlerCtrl), &pMemAddr, sizeof(pMemAddr));
                (void)AmbaWrap_memset(pAoutHdlr->pEventHandlerCtrl, 0, sizeof(AMBA_AOUT_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AOUT_NUM_EVENT);
            }
        }
    }

    /* Create mix buffer */
    if (RtVal == (UINT32)AOUT_OK) {
        RtVal = AmbaAOUT_GetMem(pMemInfo, pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32), &pMemAddr);
        if (RtVal != (UINT32)AOUT_OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create mix buffer fail",__func__, NULL, NULL, NULL, NULL);
        } else {
            (void)AmbaWrap_memcpy(&(pAoutHdlr->pMixPcmBuf), &pMemAddr, sizeof(pMemAddr));
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
            RtVal = AmbaAOUT_GetMem(pMemInfo, sizeof(AMBA_AOUT_IO_NODE_s) * pInfo->IoNodeNum, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create I/O node fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAoutHdlr->pIoNode), &pMemAddr, sizeof(pMemAddr));
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
        (void)AmbaWrap_memcpy(&AlignmentChk, &pMemInfo->pHead, sizeof(pMemInfo->pHead));
        if ((AlignmentChk & (AMBA_CACHE_LINE_SIZE - 1U)) != 0U) {
            if ((pMemInfo->CurrentSize + sizeof(UINT32)) > pMemInfo->MaxSize) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AOUT_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += sizeof(UINT32);
            }
        } else {
            pAoutHdlr->NumDescr = pInfo->DmaDescNum;
            pAoutHdlr->DmaFrameSize = pInfo->DmaFrameSize;
            pAoutHdlr->ChanNum = pInfo->ChannelNum;
            pAoutHdlr->LastDescr = 0;
            DmaBufSize = pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32) * pInfo->DmaDescNum;
            RtVal = AmbaAOUT_GetMem(pMemInfo, DmaBufSize, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA buffer fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAoutHdlr->pDmaBuffer), &pMemAddr, sizeof(pMemAddr));
            }
            break;
        }
    }
    return RtVal;
}
#ifdef CONFIG_THREADX
static UINT32 AmbaAOUT_CreateResourceDMADesc(const AMBA_AOUT_BUF_INFO_s *pNonCachedInfo,
                                             const AMBA_AOUT_IO_CREATE_INFO_s *pInfo,
                                             AMBA_AOUT_HDLR *pAoutHdlr,
                                             AMBA_AOUT_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AOUT_OK;
    UINT32 *pMemAddr;
    UINT32 Loop;
    UINT32 AlignmentChk;
    UINT32 DmaDescrSize;

    /* Non cached buffer */
    pMemInfo->CurrentSize = 0U;
    pMemInfo->MaxSize = pNonCachedInfo->MaxSize;
    pMemInfo->pHead = pNonCachedInfo->pHead;

    /* DMA Descriptors, 8 byte alignment for DMA */
    for (Loop = 0; Loop < (8U / sizeof(UINT32)); Loop++) {
        (void)AmbaWrap_memcpy(&AlignmentChk, &pMemInfo->pHead, sizeof(pMemInfo->pHead));
        if ((AlignmentChk & 0x7U) != 0U) {
            if ((pMemInfo->CurrentSize + sizeof(UINT32)) > pMemInfo->MaxSize) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT][non-Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AOUT_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += sizeof(UINT32);
            }
        } else {
            break;
        }
    }

    if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
        (void)AmbaWrap_memcpy(&AlignmentChk, &pMemInfo->pHead, sizeof(pMemInfo->pHead));
        if ((AlignmentChk & 0x7U) != 0U) {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]Not 8-ByteAlignment address: 0x%x", AlignmentChk, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0003;
        } else {
            DmaDescrSize = sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum;
            RtVal = AmbaAOUT_GetMem(pMemInfo, DmaDescrSize, &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA descriptor fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAoutHdlr->pDesc), &pMemAddr, sizeof(pMemAddr));
            }
        }
    }

    /* DMA Descriptors report */
    if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
        RtVal = AmbaAOUT_GetMem(pMemInfo, sizeof(UINT32) * pInfo->DmaDescNum, &pMemAddr);
        if (RtVal != (UINT32)AOUT_OK) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create DMA descriptor report fail",__func__, NULL, NULL, NULL, NULL);
        } else {
            (void)AmbaWrap_memcpy(&(pAoutHdlr->pReport), &pMemAddr, sizeof(pMemAddr));
        }
    }
    return RtVal;
}
#endif
#ifdef CONFIG_QNX
static void AmbaAOUT_CreateQnx(AMBA_AOUT_HDLR *pAoutHdlr, AMBA_AOUT_IO_CREATE_INFO_s *pInfo)
{
    int     card = -1;
    int     dev = 0;
    int     rtn;
    int     mode = SND_PCM_OPEN_PLAYBACK;
    snd_pcm_channel_info_t pi;
    snd_mixer_group_t group;
    snd_pcm_channel_setup_t setup;
    int     mSampleRate;
    int     mSampleChannels;
    int rate_method = 0;

    card = 0;
    dev = 0;
    mSampleRate = 48000;
    mSampleChannels = 2;

    if ((rtn = snd_pcm_open (&pAoutHdlr->pcm_handle, card, dev, mode)) < 0)
    {
        printf("device open failed - %s\n", snd_strerror(rtn));
    }

    memset (&pi, 0, sizeof (pi));
    pi.channel = SND_PCM_CHANNEL_PLAYBACK;
    if ((rtn = snd_pcm_plugin_info (pAoutHdlr->pcm_handle, &pi)) < 0)
    {
        printf ("snd_pcm_plugin_info failed: %s\n", snd_strerror (rtn));
    }

    memset (&pAoutHdlr->pp, 0, sizeof (pAoutHdlr->pp));

    pAoutHdlr->pp.mode = SND_PCM_MODE_BLOCK;
    pAoutHdlr->pp.channel = SND_PCM_CHANNEL_PLAYBACK;
    pAoutHdlr->pp.start_mode = SND_PCM_START_FULL;
    pAoutHdlr->pp.stop_mode = SND_PCM_STOP_STOP;

    pAoutHdlr->pp.buf.block.frag_size = pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32);
    //printf ("Frag Size 00: %d \n", pAoutHdlr->pp.buf.block.frag_size);
    pAoutHdlr->pp.buf.block.frags_max = (pInfo->DmaDescNum - 1);
    pAoutHdlr->pp.buf.block.frags_min = 1;

    pAoutHdlr->pp.format.interleave = 1;
    pAoutHdlr->pp.format.rate = mSampleRate;
    pAoutHdlr->pp.format.voices = mSampleChannels;
    pAoutHdlr->pp.format.format = SND_PCM_SFMT_S32_LE;

    if ((rtn = snd_pcm_plugin_set_src_method(pAoutHdlr->pcm_handle, rate_method)) != rate_method)
    {
        printf("Failed to apply rate_method %d, using %d\n", rate_method, rtn);
    }

    if ((rtn = snd_pcm_plugin_params (pAoutHdlr->pcm_handle, &pAoutHdlr->pp)) < 0)
    {
        printf ("snd_pcm_plugin_params failed: %s, why_failed = %d\n", snd_strerror (rtn), pAoutHdlr->pp.why_failed);
    }

    memset (&setup, 0, sizeof (setup));
    memset (&group, 0, sizeof (group));
    setup.channel = SND_PCM_CHANNEL_PLAYBACK;
    setup.mixer_gid = &group.gid;
    if ((rtn = snd_pcm_plugin_setup (pAoutHdlr->pcm_handle, &setup)) < 0)
    {
        printf ("snd_pcm_plugin_setup failed: %s\n", snd_strerror (rtn));
    }

    pInfo->DmaFrameSize = ((setup.buf.block.frag_size / pInfo->ChannelNum) / sizeof(UINT32));

#if 0
    printf ("Format %s \n", snd_pcm_get_format_name (setup.format.format));
    printf ("Frag Size %d %d \n", setup.buf.block.frag_size, pAoutHdlr->pp.buf.block.frag_size);
    printf ("DMA frame Size %d\n", pInfo->DmaFrameSize);
    printf ("Total Frags %d \n", setup.buf.block.frags);
    printf ("Rate %d \n", setup.format.rate);
    printf ("Voices %d \n", setup.format.voices);
#endif
    pAoutHdlr->aout_bsize = setup.buf.block.frag_size;

}
#endif

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
#ifdef CONFIG_THREADX
    UINT32 TxChannel = AMBA_DMA_CHANNEL_I2S_TX;
#endif
    if ((pInfo != NULL) && (pCachedInfo != NULL) && (pNonCachedInfo != NULL)) {
        /* Cached buffer */
        MemInfo.CurrentSize = 0U;
        MemInfo.MaxSize = pCachedInfo->MaxSize;
        MemInfo.pHead = pCachedInfo->pHead;
#ifdef CONFIG_THREADX
        if (pInfo->HwIndex == (UINT32)AOUT_I2S_0) {
            TxChannel = AMBA_DMA_CHANNEL_I2S_TX;
        } else {
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AmbaAOUT_CreateResource] Wrong HwIndex setting: %d", pInfo->HwIndex, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AOUT_ERR_0000;
        }
#endif
        /* create AOUT handler */
        if (RtVal == (UINT32)AOUT_OK) {
            RtVal = AmbaAOUT_GetMem(&(MemInfo), sizeof(AMBA_AOUT_HDLR), &pMemAddr);
            if (RtVal != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Create AOUT handler fail",__func__, NULL, NULL, NULL, NULL);
                *pHdlr = NULL;
            } else {
                (void)AmbaWrap_memcpy(&pAoutHdlr, &pMemAddr, sizeof(pMemAddr));
                *pHdlr = pMemAddr;
                pAoutHdlr->HwIndex = pInfo->HwIndex;
            }
        }

#ifdef CONFIG_QNX
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            AmbaAOUT_CreateQnx(pAoutHdlr, pInfo);
        }
#endif
        /* create the Audio task related info */
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            RtVal = AmbaAOUT_CreateRescSysInfo(pInfo, pAoutHdlr, &MemInfo);
        }

        /* DMA Buffer, AMBA_CACHE_LINE_SIZE byte alignment for data cache operations */
        if ((RtVal == (UINT32)AOUT_OK) && (pAoutHdlr != NULL)) {
            RtVal = AmbaAOUT_CreateResourceDMABuffer(pInfo, pAoutHdlr, &MemInfo);
        }

#ifdef CONFIG_THREADX
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
#endif
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
        pAoutHdlr->ErrorCode = AOUT_OK;
#ifdef CONFIG_THREADX
        /* DMA Rx channel release */
        //SubRtVal = AmbaDMA_ChannelRelease(pAoutHdlr->TxDmaChanNo);
        if (SubRtVal != DMA_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] AmbaDMA_ChannelRelease fail",__func__, NULL, NULL, NULL, NULL);
            pAoutHdlr->ErrorCode = AOUT_ERR_0007;
        }
#endif
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

#ifdef CONFIG_QNX
        if (RtVal == AOUT_OK) {
            if (pAoutHdlr->pcm_handle) {
                snd_pcm_close (pAoutHdlr->pcm_handle);
            }
        }
#endif

    } else {
        AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AOUT_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_DeleteResource End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}
#ifdef CONFIG_THREADX
static UINT32 AmbaAOUT_CheckDMA(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 *pDmaCount, UINT32 *pDmaStop)
{
    UINT32 LookAhead, Loop = 0, SubRtVal;
    volatile UINT32 *pRpt;

    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
    *pDmaStop = 0;
    *pDmaCount = 0;

    //SubRtVal = AmbaDMA_Wait(pAoutHdlr->TxDmaChanNo, AMBA_AOUT_TIMEOUT);
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
                (void)AmbaKAL_TaskSleep(1);
            }
            if (Loop > 10U) {
                pAoutHdlr->ErrorCode = AOUT_ERR_0007;
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_CheckDMA  Wait over time, Loop = %d, Rtval: 0x%x", Loop, pAoutHdlr->ErrorCode, 0U, 0U, 0U);
            }
        } while (((*pRpt & (UINT32)0x08000000U) == 0U) && (pAoutHdlr->ErrorCode == AOUT_OK));

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
#endif
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

    (void)AmbaWrap_memcpy(&DataS1, &Data1, sizeof(UINT32));
    (void)AmbaWrap_memcpy(&DataS2, &Data2, sizeof(UINT32));

    Sum = (INT64)DataS1 + (INT64)DataS2;

    if(Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x7fffffff;
    } else if(-Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x80000000U;
    } else {
        DataS3 = (INT32)Sum;
    }
    (void)AmbaWrap_memcpy(&Data3, &DataS3, sizeof(UINT32));
#endif
    return Data3;
}

/* Only support 32bit data mixer */
static void AmbaAOUT_MixPCM(UINT32 *pDst, const UINT32 *pSrc, UINT32 Size)
{
    UINT32 Loop;
    UINT32 Data1 = 0U, Data2 = 0U, Data3 = 0U;

    for (Loop = 0; Loop < Size; Loop++) {
        Data1 = pDst[0];
        Data2 = pSrc[0];
        pSrc++;
        Data3 = Audio_Qadd32(Data1, Data2);
        *pDst = Data3;
        pDst++;
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
            (void)AmbaWrap_memcpy(&pHdlr, &pAoutHdlr, sizeof(pAoutHdlr));
            EventInfo.pHdlr = pHdlr;
            EventInfo.pCbHdlr = pIoNode->pCbHdlr;
            pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_UNDERFLOW, &EventInfo);
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_UNDERFLOW), ret (0x%x), i/o (%d), FI (%d), FO (%d)", pAoutHdlr->ErrorCode, Loop, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut, 0U);
        }
    }
    return pAoutHdlr->ErrorCode;
}

static void AmbaAOUT_ScanInputChainImp(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 *pStopFrame, UINT32 *pLof, UINT32 PreFillState, UINT32 *pConnectAbuNum)
{
    UINT32   DmaBufOffset;
    UINT32   DmaSize;
    UINT32   Loop;
    UINT32   RemainSize = 0U;
    UINT32   *pDmaBuffer;
    UINT32   *pPcmBuf;
    UINT32   FirstNode = 1U;
    UINT32   IoNodeLof = 0U;
    UINT32   LofCounter = 0U;
    UINT32   *pHdlr;
    AMBA_AOUT_IO_NODE_s  *pIoNode;
    AMBA_AOUT_EVENT_INFO_s EventInfo;
#ifdef CONFIG_THREADX
    UINT32   Reg;
#endif

    DmaBufOffset = pAoutHdlr->DmaFrameSize * pAoutHdlr->ChanNum;
    DmaSize = DmaBufOffset * sizeof(UINT32);
    pDmaBuffer = &pAoutHdlr->pDmaBuffer[pAoutHdlr->FrameIn * DmaBufOffset];
    (void)AmbaWrap_memset(pDmaBuffer, 0, DmaSize);
    /* Scan input chain */
    pPcmBuf = pDmaBuffer;
    pIoNode = pAoutHdlr->pIoNode;
    for (Loop = 0; Loop < pAoutHdlr->IoNodeNum; Loop++) {
        IoNodeLof = 0U;
        if (pIoNode->NodeStatus == (UINT32)AOUT_IONODE_OPERATION) {
            (*pConnectAbuNum)++;
            if (pIoNode->BuffStatus == (UINT32)AOUT_CB_NORMAL) {
                if (pIoNode->pCbHdlr->GetSize != NULL) {
                    if (pIoNode->pCbHdlr->GetSize(&RemainSize) != (UINT32)AOUT_OK) {
                        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD][AOUT]I/O node %d]callback GetSize fail!!  RemainSize=%d", Loop, RemainSize, 0U, 0U, 0U);
                        pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0008;
                        continue;
                    } else {
                        pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;
                    }
                }
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
                        (void)AmbaWrap_memcpy(&pHdlr, &pAoutHdlr, sizeof(pAoutHdlr));
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
#ifdef CONFIG_THREADX
    (void)AmbaWrap_memcpy(&Reg, &pDmaBuffer, sizeof(void*));
    (void)AmbaCache_DataClean(Reg, DmaBufOffset * sizeof(UINT32));
#endif
    *pStopFrame = pAoutHdlr->FrameIn;
    if (++(pAoutHdlr->FrameIn) >=  pAoutHdlr->NumDescr) {
        pAoutHdlr->FrameIn = 0U;
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
            (void)AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_NOWOKIOSET, &ActualFlags);
            if ((ActualFlags & AMBA_AOUT_FLG_NOWOKIOSET) == 0U) {
                pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_NO_WORK_IO, pAoutHdlr);
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_NO_WORK_IO), ret (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                if (AmbaKAL_EventFlagSet(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_NOWOKIOSET) != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintStr5(AOUT_MODULE_ID, "[AUD][%s] (AMBA_AOUT_FLG_NOWOKIOSET) AmbaKAL_EventFlagSet fail!!",__func__, NULL, NULL, NULL, NULL);
                    pAoutHdlr->ErrorCode = (UINT32)AOUT_ERR_0006;
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
#ifdef CONFIG_THREADX
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

    (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
    pAoutHdlr->ErrorCode = AmbaAOUT_ClearFlags(pAoutHdlr);

    if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
        if (pAoutHdlr->HwIndex == (UINT32)AOUT_I2S_0) {
            I2sCh = (UINT32)AMBA_I2S_CHANNEL0;
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
        TransferCount = DmaBufOffset * sizeof(UINT32);
        (void)AmbaWrap_memset(pReport, 0, sizeof(UINT32) * pAoutHdlr->NumDescr);
        //(void)AmbaRTSL_I2sTxGetDmaAddress(I2sCh, &pI2sTxDmaAddr);
        for (Loop = 0U; Loop < (pAoutHdlr->NumDescr - 1U); Loop++) {
            pDesc->pSrcAddr = pDmaBuffer;
            pDesc->pDstAddr = pI2sTxDmaAddr;
            pDesc->pNextDesc = &pDesc[1];
            pDesc->pStatus = pReport;
            (void)AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
            pDesc->DataSize = TransferCount;
            pDesc++;
            pReport++;
            pTmp = pDmaBuffer;
            pDmaBuffer = &pTmp[DmaBufOffset];
        }

        pDesc->pSrcAddr = pDmaBuffer;
        pDesc->pDstAddr = pI2sTxDmaAddr;
        pDesc->pNextDesc = pAoutHdlr->pDesc;
        pDesc->pStatus = pReport;
        (void)AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
        pDesc->DataSize = TransferCount;
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
#endif
#ifdef CONFIG_QNX
    AMBA_AOUT_HDLR *pAoutHdlr;
    int rtn;

    (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
    pAoutHdlr->ErrorCode = AmbaAOUT_ClearFlags(pAoutHdlr);

    pAoutHdlr->FrameIn  = 0U;
    pAoutHdlr->FrameOut = 0U;
    pAoutHdlr->ScanInputChainLof = 0U;
    pAoutHdlr->DmaStop = 0U;

    rtn = snd_pcm_plugin_prepare (pAoutHdlr->pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
    if (rtn < 0) {
        printf ("snd_pcm_plugin_prepare failed: %s\n", snd_strerror (rtn));
    }

    AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                               "[AUD]AmbaAOUT_Prepare End: ret: (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);

    return pAoutHdlr->ErrorCode;
#endif
}

static UINT32 AmbaAOUT_SetEocFrame(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 StopFrame)
{
#ifdef CONFIG_THREADX
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
#endif
    return pAoutHdlr->ErrorCode;
}

static void AmbaAOUT_ProcDmaCheckLoop(AMBA_AOUT_HDLR *pAoutHdlr, UINT32 DmaCount)
{
    UINT32 Loop, StopFrame;
    UINT32 ActualFlags;
    UINT32 *pDmaBuffer;
    UINT32 DmaBufOffset;
#ifdef CONFIG_THREADX
    UINT32 Reg;
    const UINT32 *pTmp;
#endif

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
                            (void)AmbaWrap_memset(&pDmaBuffer[(StopFrame * DmaBufOffset)], 0, DmaBufOffset * sizeof(UINT32));
#ifdef CONFIG_THREADX
                            pTmp = &pDmaBuffer[StopFrame * DmaBufOffset];
                            (void)AmbaWrap_memcpy(&Reg, &pTmp, sizeof(void*));
                            (void)AmbaCache_DataClean(Reg, DmaBufOffset * sizeof(UINT32));
#endif
                            pAoutHdlr->ErrorCode = AmbaAOUT_SetEocFrame(pAoutHdlr, StopFrame);
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
#ifdef CONFIG_THREADX
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 DmaCount, DmaStop;
    void *pEventInfo;

    (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;

    if (pAoutHdlr->DmaStop == 0U) {
        /* Check DMA interrupts */
        pAoutHdlr->ErrorCode = AmbaAOUT_CheckDMA(pAoutHdlr, &DmaCount, &DmaStop);
        if (pAoutHdlr->ErrorCode == (UINT32)AOUT_OK) {
            if (DmaCount == pAoutHdlr->NumDescr) {
                (void)AmbaWrap_memcpy(&pEventInfo, &pAoutHdlr, sizeof(pAoutHdlr));
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
                (void)AmbaWrap_memcpy(&pEventInfo, &pAoutHdlr, sizeof(pAoutHdlr));
                pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, pEventInfo);
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_STOP), ret (0x%x), FI(%d) FO(%d)", pAoutHdlr->ErrorCode, pAoutHdlr->FrameIn, pAoutHdlr->FrameOut, 0U, 0U);
                pAoutHdlr->DmaStop = 1U;
                /* Suspend task Outside */
            }
        }
    } else {
        (void)AmbaKAL_TaskSleep(10000);
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA: DMA is stopped: %d", pAoutHdlr->DmaStop, 0U, 0U, 0U, 0U);
    }

    /*AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA End!!, ErrorCode=%d, DmaStop: %d", pAoutHdlr->ErrorCode, pAoutHdlr->DmaStop, 0U, 0U, 0U);*/
    return pAoutHdlr->ErrorCode;
#endif
#ifdef CONFIG_QNX
    AMBA_AOUT_HDLR *pAoutHdlr;
    UINT32 ActualFlags;
    void *pEventInfo;
    int written = 0;
    UINT32  *pDmaBuffer;
    UINT32  *pPcmBuf;
    char *mSampleBfr1;
    UINT32  StopFrame, WaitForStop = 0U;
    uint32_t systick = 0;

    (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
    pAoutHdlr->ErrorCode = (UINT32)AOUT_OK;

    if (pAoutHdlr->DmaStop == 0U) {
        /* Check DMA interrupts */
        pDmaBuffer = pAoutHdlr->pDmaBuffer;
        pPcmBuf = &pDmaBuffer[pAoutHdlr->FrameOut * pAoutHdlr->DmaFrameSize * pAoutHdlr->ChanNum];
        AmbaMisra_TypeCast(&mSampleBfr1, &pPcmBuf);
        written = snd_pcm_plugin_write (pAoutHdlr->pcm_handle, mSampleBfr1, pAoutHdlr->aout_bsize);
        //AmbaPrint_PrintUInt5("bytes write = %d, bsize = %d \n", written, pAoutHdlr->aout_bsize, 0U, 0U, 0U);

        pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_PAUSE, &ActualFlags);
        if (pAoutHdlr->ErrorCode == AOUT_OK) {
            if ((ActualFlags & AMBA_AOUT_FLG_PAUSE) != 0U) {
                *pLof = 1U;
            } else {
                pAoutHdlr->ErrorCode = AmbaAOUT_CheckStatus(&(pAoutHdlr->Flag), AMBA_AOUT_FLG_STOP, &ActualFlags);
                if (pAoutHdlr->ErrorCode == AOUT_OK) {
                    if ((ActualFlags & AMBA_AOUT_FLG_STOP) != 0U) {
                        *pLof = 1U;
                    } else {
                        *pLof = 0U;
                    }
                }
            }
        }

        if (*pLof ==  0U) {
            pAoutHdlr->ErrorCode = AmbaAOUT_ScanInputChain(pAoutHdlr, pLof, 0U, &StopFrame);
            if (pAoutHdlr->ErrorCode != (UINT32)AOUT_OK) {
                AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID,
                                           "[AUD]AmbaAOUT_ProcDMA AmbaAOUT_ScanOutputChain failed: FO: (0x%x)", pAoutHdlr->FrameOut, 0U, 0U, 0U, 0U);
            }
        }

        if (++(pAoutHdlr->FrameOut) >= pAoutHdlr->NumDescr) {
            pAoutHdlr->FrameOut = 0U;
        }

        if (*pLof == 1U) {
            /* Give DMA Stop Event */
            (void)AmbaWrap_memcpy(&pEventInfo, &pAoutHdlr, sizeof(pAoutHdlr));
            pAoutHdlr->ErrorCode = AmbaAOUT_GiveEvent(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, pEventInfo);
            AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AOUT (AMBA_AOUT_EVENT_ID_DMA_STOP), ret (0x%x)", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);
#if SND_PCM_VERSION >= SND_PROTOCOL_VERSION('P',3,0,2)
            snd_pcm_plugin_drain (pAoutHdlr->pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
#else
            snd_pcm_plugin_flush (pAoutHdlr->pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
#endif
            pAoutHdlr->DmaStop = 1U;
        }

    } else {
        (void)AmbaKAL_TaskSleep(10000);
        AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA: DMA is stopped: %d", pAoutHdlr->DmaStop, 0U, 0U, 0U, 0U);
    }

    /*AmbaPrint_ModulePrintUInt5(AOUT_MODULE_ID, "[AUD]AmbaAOUT_ProcDMA End!!, ErrorCode=%d, DmaStop: %d", pAoutHdlr->ErrorCode, pAoutHdlr->DmaStop, 0U, 0U, 0U);*/
    return pAoutHdlr->ErrorCode;
#endif
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
    (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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
        (void)AmbaWrap_memcpy(&pAoutHdlr, &pHdlr, sizeof(pHdlr));
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

