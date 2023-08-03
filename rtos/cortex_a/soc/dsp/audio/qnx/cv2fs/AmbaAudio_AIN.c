/**
 *  @file AmbaAudio_AIN.c
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
 *  @details Audio input process functions.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_AIN.h"
#include "AmbaPrint.h"
#ifdef CONFIG_THREADX
#include "AmbaRTSL_ORC.h"
#include "AmbaRTSL_I2S.h"
#include "AmbaCache.h"
#include "AmbaI2S.h"
#include "AmbaDMA.h"
#endif
#define AMBA_AIN_TIMEOUT    1000U

/**
* Audio input query working buffer size function
* @param [in]  pInfo Audio input resource creation information
* @return ErrorCode
*/
UINT32 AmbaAIN_QueryBufSize(AMBA_AIN_IO_CREATE_INFO_s *pInfo)
{
    UINT32 RtVal = AIN_OK;

    if (pInfo != NULL) {
        pInfo->CachedBufSize = sizeof(AMBA_AIN_HDLR) +
                               (sizeof(AMBA_AIN_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AIN_NUM_EVENT) +
                               (sizeof(AMBA_AIN_IO_NODE_s) * pInfo->IoNodeNum) +
                               AMBA_CACHE_LINE_SIZE + /* AMBA_CACHE_LINE_SIZE byte alignment padding */
                               (pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32) * pInfo->DmaDescNum) +
                               (sizeof(UINT64) * pInfo->DmaDescNum);
#ifdef CONFIG_THREADX
        pInfo->NonCachedBufSize = 8U + /* 8 byte alignment padding */
                                  (sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum) +
                                  (sizeof(UINT32) * pInfo->DmaDescNum);
#else
        pInfo->NonCachedBufSize = 0U;
#endif
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                   "[AUD]AmbaAIN_QueryBufSize Cached: %d, Non-cached: %d", pInfo->CachedBufSize, pInfo->NonCachedBufSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null pInfo.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_QueryBufSize End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

static UINT32 AmbaAIN_GetMem(AMBA_AIN_MEM_INFO_s *pInfo, UINT32 Size, UINT32 **pMemAddr)
{
    UINT32 MemSize;
    UINT32 RtVal = (UINT32)AIN_OK;
    UINT32 *pPtr;

    if ((Size % sizeof(UINT32)) != 0U) {
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN: Not 4-ByteAlignment size: %d", Size, 0U, 0U, 0U, 0U);
        RtVal = (UINT32)AIN_ERR_0002;
    } else {
        MemSize = pInfo->CurrentSize + Size;
        if (MemSize > pInfo->MaxSize) {
            *pMemAddr = NULL;
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN: Out of memory. MaxSize: %d  NeedSize: %d", pInfo->MaxSize, MemSize, 0U, 0U, 0U);
            RtVal = (UINT32)AIN_ERR_0004;
        } else {
            pInfo->CurrentSize = MemSize;
            *pMemAddr = pInfo->pHead;
            pPtr = pInfo->pHead;
            pInfo->pHead = &(pPtr[Size/sizeof(UINT32)]);/*pInfo->pHead += (Size/sizeof(UINT32));*/
        }
    }
    return RtVal;
}

static UINT32 AmbaAIN_CreateRescSysInfo(const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                                        AMBA_AIN_HDLR *pAinHdlr,
                                        AMBA_AIN_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = AIN_OK, SubRtVal;
    UINT32 *pMemAddr;
    AMBA_AIN_IO_NODE_s  *pIoNode;
    UINT32 Loop;

    static char AinEventFlagName[30] = "AinEventFlags";
    static char AinEventMutexName[25] = "AinEventMutex";
    static char AinIoNodeMutexName[25] = "AinIoNodeMutex";

    /* create the Audio task Status Flags */
    SubRtVal = AmbaKAL_EventFlagCreate(&(pAinHdlr->Flag), AinEventFlagName);
    if (SubRtVal != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] EventFlag Create fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        RtVal = AIN_ERR_0006;
    } else {
        SubRtVal = AmbaKAL_EventFlagClear(&(pAinHdlr->Flag), 0xffffffffU);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] EventFlag clear fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AIN_ERR_0006;
        }
    }

    /* Create event call back Mutex */
    if (RtVal == (UINT32)AIN_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAinHdlr->EventMutex), AinEventMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] Event mutex create fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AIN_ERR_0006;
        } else {
            RtVal = AmbaAIN_GetMem(pMemInfo, sizeof(AMBA_AIN_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AIN_NUM_EVENT, &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create AIN event handler fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAinHdlr->pEventHandlerCtrl), &pMemAddr, sizeof(pMemAddr));
                (void)AmbaWrap_memset(pAinHdlr->pEventHandlerCtrl, 0, sizeof(AMBA_AIN_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AIN_NUM_EVENT);
            }
        }
    }

    if (RtVal == (UINT32)AIN_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAinHdlr->IoNodeMutex), AinIoNodeMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] IoNode mutex create fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AIN_ERR_0006;
        } else {
            pAinHdlr->IoNodeNum = pInfo->IoNodeNum;
            RtVal = AmbaAIN_GetMem(pMemInfo, sizeof(AMBA_AIN_IO_NODE_s) * pInfo->IoNodeNum, &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create AIN IoNode handler fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAinHdlr->pIoNode), &pMemAddr, sizeof(pMemAddr));
                pIoNode = pAinHdlr->pIoNode;
                for (Loop = 0; Loop < pInfo->IoNodeNum; Loop++) {
                    pIoNode->pCbHdlr = NULL;
                    pIoNode++;
                }
            }
        }
    }
    return RtVal;
}

static UINT32 AmbaAIN_CreateResourceDMABuffer(const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                                              AMBA_AIN_HDLR *pAinHdlr,
                                              AMBA_AIN_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AIN_OK;
    UINT32 *pMemAddr;
    UINT32 Loop;
    ULONG  AlignmentChk;
    UINT32 DmaBufSize;

    for (Loop = 0; Loop < (AMBA_CACHE_LINE_SIZE / sizeof(UINT32)); Loop++) {
        AmbaMisra_TypeCast(&AlignmentChk, &pMemInfo->pHead);
        if ((AlignmentChk & (AMBA_CACHE_LINE_SIZE - 1U)) != 0U) {
            if ((pMemInfo->CurrentSize + sizeof(UINT32)) > pMemInfo->MaxSize) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN: [Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AIN_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += sizeof(UINT32);
            }
        } else {
            pAinHdlr->NumDescr = pInfo->DmaDescNum;
            pAinHdlr->DmaFrameSize = pInfo->DmaFrameSize;
            pAinHdlr->ChanNum = pInfo->ChannelNum;
            pAinHdlr->LastDescr = 0U;
            DmaBufSize = pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32) * pInfo->DmaDescNum;
            RtVal = AmbaAIN_GetMem(pMemInfo, DmaBufSize, &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create DMA buffer fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAinHdlr->pDmaBuffer), &pMemAddr, sizeof(pMemAddr));
            }
            break;
        }
    }

    return RtVal;
}
#ifdef CONFIG_THREADX
static UINT32 AmbaAIN_CreateResourceDMADesc(const AMBA_AIN_BUF_INFO_s *pNonCachedInfo,
                                            const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                                            AMBA_AIN_HDLR *pAinHdlr,
                                            AMBA_AIN_MEM_INFO_s *pMemInfo)
{
    UINT32 RtVal = (UINT32)AIN_OK;
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
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN [non-Cache]Out of memory. MaxSize: %d  NeedSize: %d", pMemInfo->MaxSize, (pMemInfo->CurrentSize + sizeof(UINT32)), 0U, 0U, 0U);
                RtVal = (UINT32)AIN_ERR_0004;
            } else {
                pMemInfo->pHead++;
                pMemInfo->CurrentSize += sizeof(UINT32);
            }
        } else {
            break;
        }
    }

    if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
        (void)AmbaWrap_memcpy(&AlignmentChk, &pMemInfo->pHead, sizeof(pMemInfo->pHead));
        if ((AlignmentChk & 0x7U) != 0U) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN: Not 8-ByteAlignment address: 0x%x", AlignmentChk, 0U, 0U, 0U, 0U);
            RtVal = (UINT32)AIN_ERR_0003;
        } else {
            DmaDescrSize = sizeof(AMBA_DMA_DESC_s) * pInfo->DmaDescNum;
            RtVal = AmbaAIN_GetMem(pMemInfo, DmaDescrSize, &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create DMA descriptor fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAinHdlr->pDesc), &pMemAddr, sizeof(pMemAddr));
            }
        }
    }

    /* DMA Descriptors report */
    if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
        RtVal = AmbaAIN_GetMem(pMemInfo, sizeof(UINT32) * pInfo->DmaDescNum, &pMemAddr);
        if (RtVal != AIN_OK) {
            AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create DMA descriptor report fail",__func__, NULL, NULL, NULL, NULL);
        } else {
            (void)AmbaWrap_memcpy(&(pAinHdlr->pReport), &pMemAddr, sizeof(pMemAddr));
        }
    }
    return RtVal;
}
#endif

#ifdef CONFIG_QNX
static void AmbaAIN_CreateQnx(AMBA_AIN_HDLR *pAinHdlr, AMBA_AIN_IO_CREATE_INFO_s *pInfo)
{
    int     card = -1;
    int     dev = 0;
    int     mSampleRate;
    int     mSampleChannels;
    int     mode = SND_PCM_OPEN_CAPTURE;
    int     rtn;
    snd_pcm_channel_info_t pi;
    snd_mixer_group_t group;
    snd_pcm_channel_setup_t setup;
    struct {
        snd_pcm_chmap_t map;
        unsigned int pos[32];
    } map;
    snd_pcm_voice_conversion_t voice_conversion;
    int rate_method = 0;
    snd_pcm_filter_t pevent;
    UINT32 FrameSize;

    mSampleChannels = 2;
    card = 0;
    dev = 0;
    mSampleRate = 48000;

    if ((rtn = snd_pcm_open (&pAinHdlr->pcm_handle, card, dev, mode)) < 0)
    {
        printf("snd_pcm_open failed - %s\n", snd_strerror(rtn));
    }

    //printf ("SampleRate = %d, Channels = %d\n",
    //    mSampleRate, mSampleChannels);

    /* Enable PCM events */
    pevent.enable = (1<<SND_PCM_EVENT_OVERRUN);
    snd_pcm_set_filter(pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE, &pevent);

    memset (&pi, 0, sizeof (pi));
    pi.channel = SND_PCM_CHANNEL_CAPTURE;
    if ((rtn = snd_pcm_plugin_info (pAinHdlr->pcm_handle, &pi)) < 0)
    {
        printf ("snd_pcm_plugin_info failed: %s\n", snd_strerror (rtn));
    }

    memset (&pAinHdlr->pp, 0, sizeof (pAinHdlr->pp));

    pAinHdlr->pp.mode = SND_PCM_MODE_BLOCK;
    pAinHdlr->pp.channel = SND_PCM_CHANNEL_CAPTURE;
    pAinHdlr->pp.start_mode = SND_PCM_START_DATA;
    pAinHdlr->pp.stop_mode = SND_PCM_STOP_STOP;
    pAinHdlr->pp.time = 1;

    //pAinHdlr->pp.buf.block.frag_size = pi.max_fragment_size;
    pAinHdlr->pp.buf.block.frag_size = pInfo->DmaFrameSize * pInfo->ChannelNum * sizeof(UINT32);
    pAinHdlr->pp.buf.block.frags_max = pInfo->DmaDescNum;
    pAinHdlr->pp.buf.block.frags_min = 1;

    pAinHdlr->pp.format.interleave = 1;
    pAinHdlr->pp.format.rate = mSampleRate;
    pAinHdlr->pp.format.voices = mSampleChannels;
    pAinHdlr->pp.format.format = SND_PCM_SFMT_S32_LE;

    if ((rtn = snd_pcm_plugin_set_src_method(pAinHdlr->pcm_handle, rate_method)) != rate_method)
    {
        printf("Failed to apply rate_method %d, using %d\n", rate_method, rtn);
    }

    if ((rtn = snd_pcm_plugin_params (pAinHdlr->pcm_handle, &pAinHdlr->pp)) < 0)
    {
        printf("snd_pcm_plugin_params failed: %s\n", snd_strerror (rtn));
    }

    memset (&setup, 0, sizeof (setup));
    memset (&group, 0, sizeof (group));
    setup.channel = SND_PCM_CHANNEL_CAPTURE;
    setup.mixer_gid = &group.gid;
    if ((rtn = snd_pcm_plugin_setup (pAinHdlr->pcm_handle, &setup)) < 0)
    {
        printf("snd_pcm_plugin_setup failed: %s\n", snd_strerror (rtn));
    }

    FrameSize = ((setup.buf.block.frag_size / pInfo->ChannelNum) / sizeof(UINT32));
    if (FrameSize != pInfo->DmaFrameSize) {
        printf("AIN Frame size update: ori: %d, new: %d\n", pInfo->DmaFrameSize, FrameSize);
        pInfo->DmaFrameSize = FrameSize;
    }

#if 0
    printf ("Format %s \n", snd_pcm_get_format_name (setup.format.format));
    printf ("Frag Size %d \n", setup.buf.block.frag_size);
    printf ("Total Frags %d \n", setup.buf.block.frags);
    printf ("Rate %d \n", setup.format.rate);
#endif
    pAinHdlr->ain_bsize = setup.buf.block.frag_size;

    map.map.channels = 32;
    rtn = snd_pcm_query_channel_map(pAinHdlr->pcm_handle, &map.map);
    rtn = snd_pcm_plugin_get_voice_conversion (pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE, &voice_conversion);
}
#endif

/**
* Audio input resource creation function
* @param [in]  pInfo Audio input create information
* @param [in]  pCachedInfo Cached buffer information
* @param [in]  pNonCachedInfo Non-cached buffer information
* @param [out]  pHdlr Pointer of the audio input resource
* @return ErrorCode
*/
UINT32 AmbaAIN_CreateResource(const AMBA_AIN_IO_CREATE_INFO_s *pInfo,
                              const AMBA_AIN_BUF_INFO_s *pCachedInfo,
                              const AMBA_AIN_BUF_INFO_s *pNonCachedInfo,
                              UINT32 **pHdlr)
{
    UINT32 RtVal = AIN_OK, SubRtVal;
#ifdef CONFIG_THREADX
    UINT32 RxChannel = AMBA_DMA_CHANNEL_I2S_RX;
#endif
    AMBA_AIN_HDLR *pAinHdlr = NULL;
    AMBA_AIN_MEM_INFO_s MemInfo;
    UINT32 *pMemAddr;
    UINT32 CastTmp;

    if ((pInfo != NULL) && (pCachedInfo != NULL) && (pNonCachedInfo != NULL)) {
        /* Cached buffer */
        MemInfo.CurrentSize = 0U;
        MemInfo.MaxSize = pCachedInfo->MaxSize;
        MemInfo.pHead = pCachedInfo->pHead;
#ifdef CONFIG_THREADX
        if (pInfo->HwIndex == (UINT32)AIN_I2S_0) {
            RxChannel = AMBA_DMA_CHANNEL_I2S_RX;
        } else {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] Wrong HwIndex setting: %d", pInfo->HwIndex, 0U, 0U, 0U, 0U);
            RtVal = AIN_ERR_0000;
        }
#endif
        /* create AIN handler */
        if (RtVal == AIN_OK) {
            RtVal = AmbaAIN_GetMem(&(MemInfo), sizeof(AMBA_AIN_HDLR), &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create AIN handler fail",__func__, NULL, NULL, NULL, NULL);
                *pHdlr = NULL;
            } else {
                (void)AmbaWrap_memcpy(&pAinHdlr, &pMemAddr, sizeof(pMemAddr));
                *pHdlr = pMemAddr;
                pAinHdlr->HwIndex = pInfo->HwIndex;
                pAinHdlr->LastAudioTick = 0U;
                pAinHdlr->Msb64 = 0U;
            }
        }

#ifdef CONFIG_QNX
        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            AmbaAIN_CreateQnx(pAinHdlr, pInfo);
        }
#endif
        /* create the Audio task related info */
        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            RtVal = AmbaAIN_CreateRescSysInfo(pInfo, pAinHdlr, &MemInfo);
        }

        /* DMA Buffer, AMBA_CACHE_LINE_SIZE byte alignment for data cache operations */
        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            RtVal = AmbaAIN_CreateResourceDMABuffer(pInfo, pAinHdlr, &MemInfo);
        } else {
            (void)AmbaWrap_memcpy(&CastTmp, &pAinHdlr, sizeof(pAinHdlr));
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                    "[AUD]AmbaAIN_CreateResource SysInfo Fail: ret: (0x%x), pAinHdlr: 0x%x", RtVal, CastTmp, 0U, 0U, 0U);
            RtVal = AIN_ERR_0006;
        }

        /* DMA Audio Tick */
        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            RtVal = AmbaAIN_GetMem(&(MemInfo), sizeof(UINT64) * pInfo->DmaDescNum, &pMemAddr);
            if (RtVal != AIN_OK) {
                AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Create Audio Tick fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                (void)AmbaWrap_memcpy(&(pAinHdlr->pAudioTick), &pMemAddr, sizeof(pMemAddr));
            }
        } else {
            (void)AmbaWrap_memcpy(&CastTmp, &pAinHdlr, sizeof(pAinHdlr));
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                    "[AUD]AmbaAIN_CreateResource DMA buffer Fail: ret: (0x%x), pAinHdlr: 0x%x", RtVal, CastTmp, 0U, 0U, 0U);
            RtVal = AIN_ERR_0006;
        }

#ifdef CONFIG_THREADX
        /* DMA Rx channel allocation */
        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            SubRtVal = AmbaDMA_ChannelAllocate(RxChannel, &(pAinHdlr->RxDmaChanNo));
            if (SubRtVal != DMA_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CreateResource] DMA alloc fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                RtVal = AIN_ERR_0007;
            }
            /*if (pAinHdlr->RxDmaChanNo < 0U) {
                RtVal = (UINT32)AIN_ERR_0000;
            }*/
        }

        if ((RtVal == (UINT32)AIN_OK) && (pAinHdlr != NULL)) {
            RtVal = AmbaAIN_CreateResourceDMADesc(pNonCachedInfo, pInfo, pAinHdlr, &MemInfo);
        }
#endif
        if (pAinHdlr != NULL) {
            pAinHdlr->ErrorCode = RtVal;
        } else {
            RtVal = AIN_ERR_0006;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }

    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_CreateResource End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio input resource delete function
* @param [in]  pHdlr Handle of the audio input resource
* @return ErrorCode
*/
UINT32 AmbaAIN_DeleteResource(const UINT32 *pHdlr)
{
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = AIN_OK;
#ifdef CONFIG_THREADX
        /* DMA Rx channel release */
        SubRtVal = AmbaDMA_ChannelRelease(pAinHdlr->RxDmaChanNo);
        if (SubRtVal != DMA_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] AmbaDMA_ChannelRelease fail",__func__, NULL, NULL, NULL, NULL);
            pAinHdlr->ErrorCode = AIN_ERR_0007;
        }
#endif
        if (pAinHdlr->ErrorCode == AIN_OK) {
            SubRtVal = AmbaKAL_EventFlagDelete(&(pAinHdlr->Flag));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_DeleteResource] event flag delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0006;
            }
        }

        if (pAinHdlr->ErrorCode == AIN_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAinHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_DeleteResource] event mutex delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }

        /* delete I/O node Mutex */
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_DeleteResource] ionode mutex delete fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }

        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
#ifdef CONFIG_QNX
    if (RtVal == AIN_OK) {
        if (pAinHdlr->pcm_handle) {
            snd_pcm_close (pAinHdlr->pcm_handle);
        }
    }
#endif
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_DeleteResource End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

static UINT32 AmbaAIN_GetAudioTick(AMBA_AIN_HDLR *pAinHdlr)
{
    UINT32 AudioTick = 0U;
    UINT64 Temp = (UINT64)pAinHdlr->Msb64;
    UINT32 RetVal = (UINT32)AIN_OK;

    if (pAinHdlr != NULL) {
#ifdef CONFIG_THREADX
        AudioTick = AmbaRTSL_GetOrcTimer();
#elif defined (CONFIG_QNX)
        (void)AmbaSYS_GetOrcTimer(&AudioTick);
#else
        AudioTick = 0U;
#endif
        if (pAinHdlr->LastAudioTick > (UINT64)AudioTick) {
            pAinHdlr->Msb64 += 1U;
            Temp = (UINT64)pAinHdlr->Msb64;
        }
#ifdef CONFIG_THREADX
        pAinHdlr->pAudioTick[pAinHdlr->LastDescr] = (Temp << 32U) | (UINT64)AudioTick;
#elif defined (CONFIG_QNX)
        pAinHdlr->pAudioTick[pAinHdlr->FrameIn] = (Temp << 32U) | (UINT64)AudioTick;
#else
        pAinHdlr->pAudioTick[0] = (Temp << 32U) | (UINT64)AudioTick;
#endif
        pAinHdlr->LastAudioTick = AudioTick;
    } else {
        RetVal = AIN_ERR_0001;
    }

    return RetVal;
}
#ifdef CONFIG_THREADX
static UINT32 AmbaAIN_CheckDMA(AMBA_AIN_HDLR *pAinHdlr, UINT32 *pDmaCount, UINT32 *pDmaStop)
{
    UINT32 LookAhead, Loop = 0, SubRtVal;
    volatile UINT32 *pRpt;

    pAinHdlr->ErrorCode = (UINT32)AIN_OK;
    *pDmaStop = 0;
    *pDmaCount = 0;

    SubRtVal = AmbaDMA_Wait(pAinHdlr->RxDmaChanNo, AMBA_AIN_TIMEOUT);
    if (SubRtVal != DMA_ERR_NONE) { /* Wait for SDK7 API */
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_CheckDMA] DMA wait fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        pAinHdlr->ErrorCode = AIN_ERR_0007;
    }
    if (pAinHdlr->ErrorCode == AIN_OK) {
        do {
            pRpt = &pAinHdlr->pReport[pAinHdlr->LastDescr];
            if ((*pRpt & (UINT32)0x08000000U) != 0U) {
                break;
            }
            Loop++;
            if (Loop > 5U) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_CheckDMA  Wait over time, Loop = %d", Loop, 0U, 0U, 0U, 0U);
                (void)AmbaKAL_TaskSleep(1);
            }
            if (Loop > 10U) {
                pAinHdlr->ErrorCode = AIN_ERR_0007;
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_CheckDMA  Wait over time, Loop = %d, RtVal: 0x%x", Loop, pAinHdlr->ErrorCode, 0U, 0U, 0U);
            }
        } while (((*pRpt & (UINT32)0x08000000U) == 0U) && (pAinHdlr->ErrorCode == AIN_OK));

        for(LookAhead = 0; LookAhead < pAinHdlr->NumDescr; LookAhead++) {
            pRpt = &pAinHdlr->pReport[pAinHdlr->LastDescr];
            if ((*pRpt & (UINT32)0x08000000U) == 0U) {
                break;
            }
            if ((*pRpt & (UINT32)0x08000000U) != 0U) {
                *pRpt &= (~0x08000000U);
                *pDmaCount = *pDmaCount + 1U;
                (void)AmbaAIN_GetAudioTick(pAinHdlr);
                if ((*pRpt & (UINT32)0x10000000U) != 0U) {
                    *pRpt &= (~0x10000000U);
                    *pDmaStop = 1U;
                }
                if (++pAinHdlr->LastDescr >= pAinHdlr->NumDescr) {
                    pAinHdlr->LastDescr = 0U;
                }
                pRpt = &pAinHdlr->pReport[pAinHdlr->LastDescr];
            }
        }
    }
    /*AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "  End!!  ErrorCode=%d", pAoutHdlr->ErrorCode, 0U, 0U, 0U, 0U);*/
    return pAinHdlr->ErrorCode;
}
#endif

static UINT32 AmbaAIN_GiveEvent(AMBA_AIN_HDLR *pAinHdlr, UINT32 EventId, void *pEventInfo)
{
    const AMBA_AIN_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    const AMBA_AIN_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k, SubRtVal;

    pAinHdlr->ErrorCode = (UINT32)AIN_OK;
    if (EventId >= (UINT32)AMBA_AIN_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN]EventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AIN_NUM_EVENT, 0U, 0U, 0U);
        pAinHdlr->ErrorCode = AIN_ERR_0004;
    }
    if (pAinHdlr->ErrorCode == AIN_OK) {
        /* Take the Mutex */
        SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->EventMutex), AMBA_AIN_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_GiveEvent] take mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0005;
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            pEventHandlerCtrl = &pAinHdlr->pEventHandlerCtrl[EventId];/*pAinHdlr->pEventHandlerCtrl + EventId;*/
            k = pEventHandlerCtrl->MaxNumHandler;
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler != NULL) {
                    (void)(*pWorkEventHandler)(pEventInfo);   /* invoke the Event Handler */
                }
                pWorkEventHandler++;
            }
            /* Release the Mutex */
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_GiveEvent] give mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }
    }
    AmbaMisra_TouchUnused(pEventInfo);
    return pAinHdlr->ErrorCode;
}

static void AmbaAIN_ScanOutputChainImp(AMBA_AIN_HDLR *pAinHdlr, AMBA_AIN_IO_NODE_s *pIoNode, UINT32 *pDmaBuffer, UINT32 Loop1, UINT32 DmaSize, UINT32 Lof)
{
    UINT32   RoomSize = 0U;
    void    *pEventInfo;
    UINT32  *pHdlr;
    AMBA_AIN_EVENT_INFO_s EventInfo;
    AMBA_AIN_DATA_INFO_s DataInfo;

    if (pIoNode->pCbHdlr->GetSize != NULL) {
        if (pIoNode->pCbHdlr->GetSize(&RoomSize) != OK) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][I/O node %d]callback GetSize fail!!  RoomSize=%d", Loop1, RoomSize, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0008;
        } else {
            pAinHdlr->ErrorCode = AIN_OK;
        }
    }
    if ((pAinHdlr->ErrorCode == (UINT32)AIN_OK) && (RoomSize >= DmaSize)) {
        if (pIoNode->pCbHdlr->PutPcm != NULL) {
            DataInfo.pPcmBuf = &pDmaBuffer[pIoNode->FrameOut * pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum];
            DataInfo.Size = DmaSize;
            DataInfo.AudioTicks = pAinHdlr->pAudioTick[pIoNode->FrameOut];
            //AmbaPrint_PrintUInt5("[AUD][AIN]AudioTick: %d %d %lu", pAinHdlr->FrameIn, pIoNode->FrameOut, DataInfo.AudioTicks, 0U, 0U);
            DataInfo.Lof = Lof;
            if (pIoNode->pCbHdlr->PutPcm(&DataInfo) != OK) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][I/O node %d]callback PutPcm (DmaSize) fail!!", Loop1, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0008;
            } else {
                pAinHdlr->ErrorCode = AIN_OK;
            }
        }
    }
    if (pAinHdlr->ErrorCode == AIN_OK) {
        if (((pAinHdlr->FrameIn > pIoNode->FrameOut) &&
                ((pAinHdlr->FrameIn - pIoNode->FrameOut) ==
                 (pAinHdlr->NumDescr - 1U))) ||
                ((pIoNode->FrameOut > pAinHdlr->FrameIn) &&
                 ((pIoNode->FrameOut - pAinHdlr->FrameIn) == 1U))) {
            /* Overflow checking, give event */
            (void)AmbaWrap_memcpy(&pHdlr, &pAinHdlr, sizeof(pAinHdlr));
            EventInfo.pHdlr = pHdlr;
            EventInfo.pCbHdlr = pIoNode->pCbHdlr;
            (void)AmbaWrap_memcpy(&pEventInfo, &EventInfo, sizeof(UINT32));
            pAinHdlr->ErrorCode = AmbaAIN_GiveEvent(pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_OVERFLOW, pEventInfo);
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN (AMBA_AIN_EVENT_ID_DMA_OVERFLOW), ret (0x%x)", pAinHdlr->ErrorCode, 0U, 0U, 0U, 0U);
        }
    }
    if (pAinHdlr->ErrorCode == AIN_OK) {
        if (++(pIoNode->FrameOut) >= pAinHdlr->NumDescr) {
            pIoNode->FrameOut = 0U;
        }
    }
}

static UINT32 AmbaAIN_ScanOutputChain(AMBA_AIN_HDLR *pAinHdlr, UINT32 Lof)
{
    UINT32   Loop1, Loop2, SubRtVal;
    const UINT32  *pPcmBuf;
    UINT32  *pDmaBuffer;
    UINT32   DmaSize;
    UINT32   MakeUp;
#ifdef CONFIG_THREADX
    UINT32   Reg;
#endif
    AMBA_AIN_IO_NODE_s  *pIoNode;

    pAinHdlr->ErrorCode = (UINT32)AIN_OK;
    /* Update DMA data */
    pDmaBuffer = pAinHdlr->pDmaBuffer;
    DmaSize = pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum * sizeof(UINT32);
#ifdef CONFIG_THREADX
    pPcmBuf = &pDmaBuffer[pAinHdlr->FrameIn * pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum];
    (void)AmbaWrap_memcpy(&Reg, &pPcmBuf, sizeof(void*));
    (void)AmbaCache_DataInvalidate(Reg, DmaSize);
#endif
    /* Scan output chain */
    SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->IoNodeMutex), AMBA_AIN_TIMEOUT);
    if (SubRtVal != KAL_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_ScanOutputChain] take mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        pAinHdlr->ErrorCode = (UINT32)AIN_ERR_0005;
    } else {
        pIoNode = pAinHdlr->pIoNode;
        for (Loop1 = 0; Loop1 < pAinHdlr->IoNodeNum; Loop1++) {
            if (pIoNode->NodeStatus == (UINT32)AIN_IONODE_OPERATION) {
                if (pIoNode->BuffStatus == (UINT32)AIN_CB_CUTIN) {
                    pIoNode->FrameOut = pAinHdlr->FrameIn;
                    pIoNode->BuffStatus = (UINT32)AIN_CB_NORMAL;
                }
                if (pAinHdlr->FrameIn == pIoNode->FrameOut) {
                    MakeUp = 1U;
                } else {
                    MakeUp = 2U;
                }
                for (Loop2 = 0; Loop2 < MakeUp; Loop2++) {
                    AmbaAIN_ScanOutputChainImp(pAinHdlr, pIoNode, pDmaBuffer, Loop1, DmaSize, Lof);
                }
            }
            pIoNode++;
        }
        if (pAinHdlr->ErrorCode == AIN_OK) {
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_ScanOutputChain] give mutex fail, ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = (UINT32)AIN_ERR_0005;
            }
        }
    }

    return pAinHdlr->ErrorCode;
}

static UINT32 AmbaAIN_CheckStatus(AMBA_KAL_EVENT_FLAG_t *pFlag, UINT32 StatusFlag, UINT32 *pActualFlags)
{
    UINT32 RtVal = AIN_OK, SubRtVal;

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

/**
* Audio input main function
* @param [in]  pHdlr Handle of the audio input resource
* @param [out]  pLof Pointer of the last of frame flag
* @return ErrorCode
*/
UINT32 AmbaAIN_ProcDMA(const UINT32 *pHdlr, UINT32 *pLof)
{
#ifdef CONFIG_THREADX
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop1, RtVal = AIN_OK;
    UINT32 DmaCount, DmaStop, StopFrame;
    UINT32 ActualFlags;
    void *pEventInfo;

    if ((pHdlr != NULL) && (pLof != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        *pLof = 0U;

        /* Check DMA interrupts */
        pAinHdlr->ErrorCode = AmbaAIN_CheckDMA(pAinHdlr, &DmaCount, &DmaStop);
        if (pAinHdlr->ErrorCode == AIN_OK) {
            if (DmaCount == pAinHdlr->NumDescr) {
                (void)AmbaWrap_memcpy(&pEventInfo, &pAinHdlr, sizeof(pAinHdlr));
                pAinHdlr->ErrorCode = AmbaAIN_GiveEvent(pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_OVERTIME, pEventInfo);
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN (AMBA_AIN_EVENT_ID_DMA_OVERTIME), ret (0x%x)", pAinHdlr->ErrorCode, 0U, 0U, 0U, 0U);
            }
            if (DmaStop == 0U) {
                for (Loop1 = 0U; Loop1 < DmaCount; Loop1++) {
                    pAinHdlr->ErrorCode = AmbaAIN_CheckStatus(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP, &ActualFlags);
                    if (pAinHdlr->ErrorCode == AIN_OK) {
                        if ((ActualFlags & AMBA_AIN_FLG_STOP) != 0U) {
                            StopFrame = (pAinHdlr->FrameIn + 2U) % pAinHdlr->NumDescr; /* 2 is safe for DMA stop */
                            pAinHdlr->pDesc[StopFrame].Ctrl.EndOfChain = 1U;
                        }
                        *pLof = 0U;
                        pAinHdlr->ErrorCode = AmbaAIN_ScanOutputChain(pAinHdlr, 0U);
                        if (pAinHdlr->ErrorCode != (UINT32)AIN_OK) {
                            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                                       "[AUD]AmbaAIN_ProcDMA AmbaAIN_ScanOutputChain failed: FI: (0x%x)", pAinHdlr->FrameIn, 0U, 0U, 0U, 0U);
                        }
                        if (++(pAinHdlr->FrameIn) >= pAinHdlr->NumDescr) {
                            pAinHdlr->FrameIn = 0U;
                        }
                    }
                }
            } else {
                *pLof = 1U;
                pAinHdlr->ErrorCode = AmbaAIN_ScanOutputChain(pAinHdlr, 1U);
                if (pAinHdlr->ErrorCode != (UINT32)AIN_OK) {
                    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                               "[AUD]AmbaAIN_ProcDMA LOF: AmbaAIN_ScanOutputChain failed: FI: (0x%x)", pAinHdlr->FrameIn, 0U, 0U, 0U, 0U);
                }
                if (++(pAinHdlr->FrameIn) >= pAinHdlr->NumDescr) {
                    pAinHdlr->FrameIn = 0U;
                }
                /* Disable I2S RX Outside */
                /* Give DMA Stop Event */
                (void)AmbaWrap_memcpy(&pEventInfo, &pAinHdlr, sizeof(pAinHdlr));
                pAinHdlr->ErrorCode = AmbaAIN_GiveEvent(pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_STOP, pEventInfo);
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN (AMBA_AIN_EVENT_ID_DMA_STOP), ret (0x%x)", pAinHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                /* Suspend task Outside */
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    /*AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                    "[AUD]AmbaAIN_ProcDMA End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);*/
    return RtVal;
#endif
#ifdef CONFIG_QNX
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop1, RtVal = AIN_OK;
    UINT32 DmaCount, DmaStop, StopFrame;
    UINT32 ActualFlags;
    void *pEventInfo;
    int read = 0;
    char *mSampleBfr1;
    UINT32  *pPcmBuf;
    UINT32  *pDmaBuffer;
    //ULONG Tmp, Tmp1;
    snd_pcm_channel_status_t status;

    //AmbaMisra_TypeCast(&Tmp, &pHdlr);
    //AmbaPrint_PrintUInt5("AmbaAIN_ProcDMA 0x%x, %d", Tmp, *pLof, 0U, 0U, 0U);

    if ((pHdlr != NULL) && (pLof != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        *pLof = 0U;

        /* Check DMA interrupts */
        pDmaBuffer = pAinHdlr->pDmaBuffer;
        pPcmBuf = &pDmaBuffer[pAinHdlr->FrameIn * pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum];
        AmbaMisra_TypeCast(&mSampleBfr1, &pPcmBuf);
        read = snd_pcm_plugin_read (pAinHdlr->pcm_handle, mSampleBfr1, pAinHdlr->ain_bsize);
        (void)AmbaAIN_GetAudioTick(pAinHdlr);
        //printf ("bytes read = %d, bsize = %d \n", read, pAinHdlr->ain_bsize);

        pAinHdlr->ErrorCode = AmbaAIN_CheckStatus(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP, &ActualFlags);
        if (pAinHdlr->ErrorCode == AIN_OK) {
            if ((ActualFlags & AMBA_AIN_FLG_STOP) != 0U) {
                *pLof = 1U;
            } else {
                *pLof = 0U;
            }

            pAinHdlr->ErrorCode = AmbaAIN_ScanOutputChain(pAinHdlr, *pLof);
            if (pAinHdlr->ErrorCode != (UINT32)AIN_OK) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                           "[AUD]AmbaAIN_ProcDMA AmbaAIN_ScanOutputChain failed: FI: (0x%x)", pAinHdlr->FrameIn, 0U, 0U, 0U, 0U);
            }
            if (++(pAinHdlr->FrameIn) >= pAinHdlr->NumDescr) {
                pAinHdlr->FrameIn = 0U;
            }
        }

        if (*pLof == 1U) {
#if SND_PCM_VERSION >= SND_PROTOCOL_VERSION('P',3,0,2)
            snd_pcm_plugin_drop(pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE);
#else
            snd_pcm_plugin_flush (pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE);
#endif
            (void)AmbaKAL_TaskSleep(100U);
            /* Give DMA Stop Event */
            (void)AmbaWrap_memcpy(&pEventInfo, &pAinHdlr, sizeof(pAinHdlr));
            pAinHdlr->ErrorCode = AmbaAIN_GiveEvent(pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_STOP, pEventInfo);
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AIN (AMBA_AIN_EVENT_ID_DMA_STOP), ret (0x%x)", pAinHdlr->ErrorCode, 0U, 0U, 0U, 0U);
        }

    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    /*AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                    "[AUD]AmbaAIN_ProcDMA End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);*/
    return RtVal;
#endif
}

/**
* Audio input initialize and prepare the audio input process function
* @param [in]  pHdlr Handle of the audio input resource
* @return ErrorCode
*/
UINT32 AmbaAIN_Prepare(const UINT32 *pHdlr)
{
#ifdef CONFIG_THREADX
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop, RtVal = AIN_OK, SubRtVal;
    AMBA_DMA_DESC_s *pDesc;
    AMBA_DMA_DESC_CTRL_s DmaDescCtrl;
    UINT32 *pDmaBuffer;
    UINT32 *pTmp;
    UINT32 *pReport;
    UINT32 TransferCount;
    UINT32 DmaBufOffset;
    UINT32 I2sCh = 0;
    void   *pI2sRxDmaAddr;
    UINT32   DmaSize;
    UINT32   Reg;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_EventFlagClear(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_Prepare clear flag failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0006;
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            if (pAinHdlr->HwIndex == (UINT32)AIN_I2S_0) {
                I2sCh = (UINT32)AMBA_I2S_CHANNEL0;
            } else {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AmbaAIN_Prepare] Wrong HwIndex setting: %d", pAinHdlr->HwIndex, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0000;
            }
        }

        DmaSize = pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum * sizeof(UINT32) * pAinHdlr->NumDescr;
        AmbaMisra_TypeCast32(&Reg, &(pAinHdlr->pDmaBuffer));
        (void)AmbaCache_DataInvalidate(Reg, DmaSize);

        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            DmaDescCtrl.StopOnError = 1U;
            DmaDescCtrl.IrqOnError = 1U;
            DmaDescCtrl.IrqOnDone = 1U;
            DmaDescCtrl.Reserved0 = 0U;
            DmaDescCtrl.BusBlockSize = AMBA_DMA_BUS_BLOCK_32BYTE;
            DmaDescCtrl.BusDataSize = AMBA_DMA_BUS_DATA_4BYTE;
            DmaDescCtrl.NoBusAddrInc = 1U;
            DmaDescCtrl.ReadMem = 0U;
            DmaDescCtrl.WriteMem = 1U;
            DmaDescCtrl.EndOfChain = 0U;
            DmaDescCtrl.Reserved1 = 0U;

            pDesc = pAinHdlr->pDesc;
            pDmaBuffer = pAinHdlr->pDmaBuffer;
            pReport = pAinHdlr->pReport;
            TransferCount = pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum * sizeof(UINT32);
            DmaBufOffset = pAinHdlr->DmaFrameSize * pAinHdlr->ChanNum;
            (void)AmbaWrap_memset(pReport, 0, sizeof(UINT32) * pAinHdlr->NumDescr);
            (void)AmbaRTSL_I2sRxGetDmaAddress(I2sCh, &pI2sRxDmaAddr);
            for (Loop = 0U; Loop < (pAinHdlr->NumDescr - 1U); Loop++) {
                pDesc->pSrcAddr = pI2sRxDmaAddr;
                pDesc->pDstAddr = pDmaBuffer;
                pDesc->pNextDesc = &pDesc[1];
                pDesc->pStatus = pReport;
                (void)AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
                pDesc->DataSize = TransferCount;
                pDesc++;
                pReport++;
                pTmp = pDmaBuffer;
                pDmaBuffer = &pTmp[DmaBufOffset];
            }
            pDesc->pSrcAddr = pI2sRxDmaAddr;
            pDesc->pDstAddr = pDmaBuffer;
            pDesc->pNextDesc = pAinHdlr->pDesc;
            pDesc->pStatus = pReport;
            (void)AmbaWrap_memcpy(&pDesc->Ctrl, &DmaDescCtrl, sizeof(AMBA_DMA_DESC_CTRL_s));
            pDesc->DataSize = TransferCount;
            pAinHdlr->FrameIn = 0U;
            pAinHdlr->LastDescr = 0U;
            SubRtVal = AmbaDMA_Transfer(pAinHdlr->RxDmaChanNo, pAinHdlr->pDesc);
            if (SubRtVal != DMA_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_Prepare: AmbaDMA_Transfer failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0007;
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_Prepare End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
#endif
#ifdef CONFIG_QNX
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop, RtVal = AIN_OK, SubRtVal;
    int rtn;

    if (pHdlr != NULL) {
            AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
            pAinHdlr->ErrorCode = (UINT32)AIN_OK;
            SubRtVal = AmbaKAL_EventFlagClear(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_Prepare clear flag failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0006;
            }
            if (snd_pcm_plugin_prepare (pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE) < 0) {
                printf("snd_pcm_plugin_prepare failed\n");
            }
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }

    return RtVal;
#endif
}

/**
* Audio input stop function
* @param [in]  pHdlr Handle of the audio input resource
* @return ErrorCode
*/
UINT32 AmbaAIN_Stop(const UINT32 *pHdlr)
{
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 RtVal = AIN_OK, SubRtVal;
#ifdef CONFIG_THREADX
    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_EventFlagSet(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_Stop: set AMBA_AIN_FLG_STOP failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0007;
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_Stop End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
#endif
#ifdef CONFIG_QNX
    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
#if SND_PCM_VERSION >= SND_PROTOCOL_VERSION('P',3,0,2)
        snd_pcm_plugin_drop(pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE);
#else
        snd_pcm_plugin_flush (pAinHdlr->pcm_handle, SND_PCM_CHANNEL_CAPTURE);
#endif
        SubRtVal = AmbaKAL_EventFlagSet(&(pAinHdlr->Flag), AMBA_AIN_FLG_STOP);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_Stop: set AMBA_AIN_FLG_STOP failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0007;
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
           AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null input.",__func__, NULL, NULL, NULL, NULL);
           RtVal = AIN_ERR_0001;
    }
   AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                              "[AUD]AmbaAIN_Stop End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
#endif
    return RtVal;
}

/**
* Audio input register the output chain scanner callback functions
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAIN_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop = 0U;
    AMBA_AIN_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->IoNodeMutex), AMBA_AIN_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_RegisterCallBackFunc: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0005;
        } else {
            pIoNode = pAinHdlr->pIoNode;
            for (Loop = 0; Loop < pAinHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == NULL) {
                    pIoNode->pCbHdlr = pCbHdlr;
                    pIoNode->NodeStatus = (UINT32)AIN_IONODE_PAUSE;
                    pIoNode->BuffStatus = (UINT32)AIN_CB_CUTIN;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_RegisterCallBackFunc: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            if (Loop == pAinHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][Register] out of the max I/O node number: %d", pAinHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0004;
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_RegisterCallBackFunc End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio input de-register the output chain scanner callback functions
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAIN_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    AMBA_AIN_HDLR *pAinHdlr;
    UINT32 Loop= 0U;
    AMBA_AIN_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->IoNodeMutex), AMBA_AIN_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_DeRegisterCallBackFunc: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0005;
        } else {
            pIoNode = pAinHdlr->pIoNode;
            for (Loop = 0; Loop < pAinHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->pCbHdlr = NULL;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_DeRegisterCallBackFunc: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            if (Loop == pAinHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][DeRegister] out of the max I/O node number: %d", pAinHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0004;
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_DeRegisterCallBackFunc End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio input open the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAIN_OpenIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AIN_HDLR *pAinHdlr;
    AMBA_AIN_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->IoNodeMutex), AMBA_AIN_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_OpenIoNode: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0005;
        } else {
            pIoNode = pAinHdlr->pIoNode;
            for (Loop = 0; Loop < pAinHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AIN_IONODE_OPERATION;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_OpenIoNode: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            if (Loop == pAinHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][OpenIoNode] out of the max I/O node number: %d", pAinHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0004;
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_OpenIoNode End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);

    return RtVal;
}

/**
* Audio input close the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAIN_CloseIoNode(const UINT32 *pHdlr, const AMBA_AIN_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AIN_HDLR *pAinHdlr;
    AMBA_AIN_IO_NODE_s  *pIoNode;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->IoNodeMutex), AMBA_AIN_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_CloseIoNode: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = AIN_ERR_0005;
        } else {
            pIoNode = pAinHdlr->pIoNode;
            for (Loop = 0; Loop < pAinHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AIN_IONODE_PAUSE;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_CloseIoNode: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            if (Loop == pAinHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN][CloseIoNode] out of the max I/O node number: %d", pAinHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0004;
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RtVal = AIN_ERR_0001;
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_CloseIoNode End: ret: (0x%x)", RtVal, 0U, 0U, 0U, 0U);
    return RtVal;
}

/**
* Audio input configure control Settings for the AIN event handler
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  EventId Event ID of the AIN event
* @param [in]  MaxNumHandler Max number of Handlers
* @param [in]  pEventHandlers Pointer of the Event Handlers
* @return ErrorCode
*/
UINT32 AmbaAIN_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                               UINT32 MaxNumHandler,
                               AMBA_AIN_EVENT_HANDLER_f *pEventHandlers)
{
    AMBA_AIN_HDLR *pAinHdlr;
    AMBA_AIN_EVENT_HANDLER_CTRL_s *pWorkEventHandler;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if ((pHdlr != NULL) && (pEventHandlers != NULL) && (MaxNumHandler != 0U)) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        if (EventId >= (UINT32)AMBA_AIN_NUM_EVENT) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN]Config eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AIN_NUM_EVENT, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = (UINT32)AIN_ERR_0004;
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->EventMutex), AMBA_AIN_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_ConfigEventHdlr: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
            if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
                pWorkEventHandler = &pAinHdlr->pEventHandlerCtrl[EventId];
                pWorkEventHandler->MaxNumHandler = MaxNumHandler;   /* maximum number of Handlers */
                pWorkEventHandler->pEventHandler = pEventHandlers;  /* pointer to the Event Handlers */
                /* Release the Mutex */
                SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_ConfigEventHdlr: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    pAinHdlr->ErrorCode = AIN_ERR_0005;
                }
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        RtVal = AIN_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                   "[AUD]AmbaAIN_ConfigEventHdlr error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_ConfigEventHdlr End: ret: (0x%x), EventId(%d), MaxNumHandler(%d)",
                               RtVal,
                               EventId,
                               MaxNumHandler, 0U, 0U);

    return RtVal;
}

/**
* Audio input register a specified audio event handler
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  EventId Event ID of the AIN event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAIN_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                 AMBA_AIN_EVENT_HANDLER_f EventHandler)
{
    AMBA_AIN_HDLR *pAinHdlr;
    const AMBA_AIN_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AIN_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        if (EventId >= (UINT32)AMBA_AIN_NUM_EVENT) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN]Register eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AIN_NUM_EVENT, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = (UINT32)AIN_ERR_0004;
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->EventMutex), AMBA_AIN_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_RegisterEventHdlr: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
            if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
                pEventHandlerCtrl = &pAinHdlr->pEventHandlerCtrl[EventId];
                k = pEventHandlerCtrl->MaxNumHandler;

                /* check to see if it is already registered */
                pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
                for (i = 0; i < k; i++) {
                    if (*pWorkEventHandler == EventHandler) {
                        AmbaPrint_ModulePrintStr5(AIN_MODULE_ID, "[AUD][%s] Event already registered.",__func__, NULL, NULL, NULL, NULL);
                        pAinHdlr->ErrorCode = AIN_ERR_0000;  /* it is already registered, why ? */
                    }
                    pWorkEventHandler++;
                }
                if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
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
                SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_RegisterEventHdlr: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    pAinHdlr->ErrorCode = AIN_ERR_0005;
                }
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        RtVal = AIN_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                   "[AUD]AmbaAIN_RegisterEventHdlr error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_RegisterEventHdlr End: ret: (0x%x), EventId(%d)", RtVal, EventId, 0U, 0U, 0U);

    return RtVal;
}

/**
* Audio input de-register a specified audio event handler
* @param [in]  pHdlr Handle of the audio input resource
* @param [in]  EventId Event ID of the AIN event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAIN_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId,
                                   AMBA_AIN_EVENT_HANDLER_f EventHandler)
{
    AMBA_AIN_HDLR *pAinHdlr;
    const AMBA_AIN_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AIN_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 RtVal = AIN_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAinHdlr, &pHdlr);
        pAinHdlr->ErrorCode = (UINT32)AIN_OK;
        if (EventId >= (UINT32)AMBA_AIN_NUM_EVENT) {
            AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD][AIN]DeRegister eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_AIN_NUM_EVENT, 0U, 0U, 0U);
            pAinHdlr->ErrorCode = (UINT32)AIN_ERR_0004;
        }
        if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAinHdlr->EventMutex), AMBA_AIN_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_DeRegisterEventHdlr: take mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                pAinHdlr->ErrorCode = AIN_ERR_0005;
            }
            if (pAinHdlr->ErrorCode == (UINT32)AIN_OK) {
                pEventHandlerCtrl = &pAinHdlr->pEventHandlerCtrl[EventId];
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
                SubRtVal = AmbaKAL_MutexGive(&(pAinHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID, "[AUD]AmbaAIN_DeRegisterEventHdlr: give mutex failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    pAinHdlr->ErrorCode = AIN_ERR_0005;
                }
            }
        }
        RtVal = pAinHdlr->ErrorCode;
    } else {
        RtVal = AIN_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                                   "[AUD]AmbaAIN_DeRegisterEventHdlr error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AIN_MODULE_ID,
                               "[AUD]AmbaAIN_DeRegisterEventHdlr End: ret: (0x%x), EventId(%d)", RtVal, EventId, 0U, 0U, 0U);

    return RtVal;
}
