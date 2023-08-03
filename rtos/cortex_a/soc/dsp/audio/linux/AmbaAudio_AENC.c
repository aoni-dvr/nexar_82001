/**
 *  @file AmbaAudio_AENC.c
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
 *  @details Audio encoder process functions.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_AENC.h"

#define AMBA_AENC_TIMEOUT 1000U

/**
* Audio encoder query working buffer size function
* @param [in]  pInfo Audio encoder resource creation information
* @return ErrorCode
*/
UINT32 AmbaAENC_QueryBufSize (AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo)
{
    UINT32 RetVal = AENC_OK;

    if (pInfo != NULL) {
        pInfo->CachedBufSize = sizeof(AMBA_AENC_HDLR) +
                               (sizeof(AMBA_AENC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AENC_NUM_EVENT) +
                               (sizeof(AMBA_AENC_IO_NODE_s) * pInfo->IoNodeNum) +
                               pInfo->PlugInLibSelfSize +
                               ((pInfo->ChannelNum * pInfo->FrameSize * pInfo->SampleResolution / 8U) * 3U);
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_QueryBufSize End: Size: (%u)", pInfo->CachedBufSize, 0U, 0U, 0U, 0U);
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_QueryBufSize error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

static UINT32 AmbaAENC_GetMem(AMBA_AENC_MEM_INFO_s *pInfo, UINT32 Size, UINT32 **pMemAddr)
{
    UINT32 MemSize;
    UINT32 RetVal = (UINT32)AENC_OK;
    UINT32 *pPtr;

    if ((Size % sizeof(UINT32)) != 0U) {
        RetVal = (UINT32)AENC_ERR_0002;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AENC buffer 4 bytes alignment error: Size (%u)", Size, 0U, 0U, 0U, 0U);
    } else {
        MemSize = pInfo->CurrentSize + Size;
        if (MemSize > pInfo->MaxSize) {
            *pMemAddr = NULL;
            RetVal = (UINT32)AENC_ERR_0001;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC buffer overflow: required size (%u) > max size (%u)", MemSize, pInfo->MaxSize, 0U, 0U, 0U);
        } else {
            pInfo->CurrentSize = MemSize;
            *pMemAddr = pInfo->pHead;
            pPtr = pInfo->pHead;
            pInfo->pHead = &(pPtr[Size/sizeof(UINT32)]);/*pInfo->pHead += (Size/sizeof(UINT32));*/
        }
    }
    return RetVal;
}

static UINT32 AmbaAENC_CreateRescSysInfo(const AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo,
                                         AMBA_AENC_HDLR *pAencHdlr,
                                         AMBA_AENC_MEM_INFO_s *pMemInfo)
{
    UINT32 RetVal = AENC_OK, SubRtVal;
    UINT32 *pMemAddr;
    AMBA_AENC_IO_NODE_s *pIoNode;
    UINT32 Loop;
    static char AencEventFlagName[30]       = "AencEventFlags";
    static char AencEventCbMutexName[20]    = "AencEventCbMutex";
    static char AencIoNodeMutexName[20]     = "AencIoNodeMutex";
    static char AencFadeMutexName[15]       = "AencFadeMutex";

    /* create the Audio task Status Flags */
    SubRtVal = AmbaKAL_EventFlagCreate(&(pAencHdlr->Flag), AencEventFlagName);
    if (SubRtVal != KAL_ERR_NONE) {
        RetVal = (UINT32)AENC_ERR_0004;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AENC Event Flag create failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
    } else {
        SubRtVal = AmbaKAL_EventFlagClear(&(pAencHdlr->Flag), 0xffffffffU);
        if (SubRtVal != KAL_ERR_NONE) {
            RetVal = (UINT32)AENC_ERR_0004;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC Event Flag clear failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }
    }

    /* Create event call back Mutex */
    if (RetVal == (UINT32)AENC_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAencHdlr->EventMutex), AencEventCbMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            RetVal = (UINT32)AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC Event Cb Mutex create failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            RetVal = AmbaAENC_GetMem(pMemInfo, sizeof(AMBA_AENC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AENC_NUM_EVENT, &pMemAddr);
            if (RetVal == (UINT32)AENC_OK) {
                AmbaMisra_TypeCast(&(pAencHdlr->pEventHandlerCtrl), &pMemAddr);
                RetVal = AmbaWrap_memset(pAencHdlr->pEventHandlerCtrl, 0, sizeof(AMBA_AENC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_AENC_NUM_EVENT);
            } else {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create event handler failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    /* Create I/O node Mutex */
    if (RetVal == (UINT32)AENC_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAencHdlr->IoNodeMutex), AencIoNodeMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            RetVal = (UINT32)AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC IoNode Mutex create failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pAencHdlr->IoNodeNum = pInfo->IoNodeNum;
            RetVal = AmbaAENC_GetMem(pMemInfo, sizeof(AMBA_AENC_IO_NODE_s) * pInfo->IoNodeNum, &pMemAddr);
            if (RetVal == (UINT32)AENC_OK) {
                AmbaMisra_TypeCast(&(pAencHdlr->pIoNode), &pMemAddr);
                pIoNode = pAencHdlr->pIoNode;
                for (Loop = 0; Loop < pInfo->IoNodeNum; Loop++) {
                    pIoNode->pCbHdlr = NULL;
                    pIoNode++;
                }
            } else {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create I/O node failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    /* Create Fade Mutex */
    if (RetVal == (UINT32)AENC_OK) {
        SubRtVal = AmbaKAL_MutexCreate(&(pAencHdlr->FadeCtrl.Mutex), AencFadeMutexName);
        if (SubRtVal != KAL_ERR_NONE) {
            RetVal = (UINT32)AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC Fade Mutex create failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/**
* Audio encoder resource creation function
* @param [in]  pInfo Audio encoder create information
* @param [in]  pCachedInfo Cached buffer information
* @param [out]  pHdlr Pointer of the audio encoder resource
* @return ErrorCode
*/
UINT32 AmbaAENC_CreateResource(const AMBA_AUDIO_ENC_CREATE_INFO_s *pInfo, const AMBA_AENC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr)
{
    UINT32 RetVal;
    AMBA_AENC_HDLR *pAencHdlr = NULL;
    AMBA_AENC_MEM_INFO_s MemInfo;
    UINT32 *pMemAddr;

    if ((pInfo != NULL) && (pCachedInfo != NULL)) {
        /* Cached buffer */
        MemInfo.CurrentSize = 0U;
        MemInfo.MaxSize = pCachedInfo->MaxSize;
        MemInfo.pHead = pCachedInfo->pHead;

        /* create AENC handler */
        RetVal = AmbaAENC_GetMem(&(MemInfo), sizeof(AMBA_AENC_HDLR), &pMemAddr);
        if (RetVal != (UINT32)AENC_OK) {
            *pHdlr = NULL;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC create AENC handler failed", 0U, 0U, 0U, 0U, 0U);
        } else {
            AmbaMisra_TypeCast(&pAencHdlr, &pMemAddr);
            *pHdlr = pMemAddr;
            pAencHdlr->SampleFreq       = pInfo->SampleFreq;
            pAencHdlr->SampleResolution = pInfo->SampleResolution;
            pAencHdlr->ChannelNum       = pInfo->ChannelNum;
            pAencHdlr->FrameSize        = pInfo->FrameSize;
        }

        /* create the Audio task related info */
        if ((RetVal == (UINT32)AENC_OK) && (pAencHdlr != NULL)) {
            RetVal = AmbaAENC_CreateRescSysInfo(pInfo, pAencHdlr, &MemInfo);
        }

        /* Create self config memory for plug-in Codec */
        if ((RetVal == (UINT32)AENC_OK) && (pAencHdlr != NULL)) {
            RetVal = AmbaAENC_GetMem(&(MemInfo), pInfo->PlugInLibSelfSize, &pMemAddr);
            if (RetVal == (UINT32)AENC_OK) {
                AmbaMisra_TypeCast(&(pAencHdlr->pSelf), &pMemAddr);
            } else {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create self config memory failed", 0U, 0U, 0U, 0U, 0U);
            }
        }

        /* Create Source PCM Buffer */
        if ((RetVal == (UINT32)AENC_OK) && (pAencHdlr != NULL)) {
            pAencHdlr->SrcPcmBufSize = pInfo->ChannelNum * pInfo->FrameSize * pInfo->SampleResolution / 8U;
            RetVal = AmbaAENC_GetMem(&(MemInfo), pAencHdlr->SrcPcmBufSize, &pMemAddr);
            if (RetVal != (UINT32)AENC_OK) {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create Source PCM Buffer failed", 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaMisra_TypeCast(&(pAencHdlr->pPcmBufRptr), &pMemAddr);
            }
        }

        /* Create Mix PCM Buffer */
        if ((RetVal == (UINT32)AENC_OK) && (pAencHdlr != NULL)) {
            RetVal = AmbaAENC_GetMem(&(MemInfo), pAencHdlr->SrcPcmBufSize, &pMemAddr);
            if (RetVal != (UINT32)AENC_OK) {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create Mix PCM Buffer failed", 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaMisra_TypeCast(&(pAencHdlr->pMixPcmBuf), &pMemAddr);
            }
        }

        /* Create BS temp Buffer */
        if ((RetVal == (UINT32)AENC_OK) && (pAencHdlr != NULL)) {
            RetVal = AmbaAENC_GetMem(&(MemInfo), pAencHdlr->SrcPcmBufSize, &pMemAddr);
            if (RetVal != (UINT32)AENC_OK) {
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC create BS temp Buffer failed", 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaMisra_TypeCast(&(pAencHdlr->pBitTempBuf), &pMemAddr);
            }
        }

        if (pAencHdlr != NULL) {
            pAencHdlr->ErrorCode = RetVal;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AENC_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RetVal = AENC_ERR_0000;
    }


    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_CreateResource End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio encoder resource delete function
* @param [in]  pHdlr Handle of the audio encoder resource
* @return ErrorCode
*/
UINT32 AmbaAENC_DeleteResource(const UINT32 *pHdlr)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;

        SubRtVal = AmbaKAL_EventFlagDelete(&(pAencHdlr->Flag));
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0004;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC Event flag delete failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }

        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAencHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC Event Cb Mutex delete failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }

        /* delete I/O node Mutex */
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAencHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC I/O node Mutex delete failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }

        /* delete Fade Mutex */
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            SubRtVal = AmbaKAL_MutexDelete(&(pAencHdlr->FadeCtrl.Mutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC Fade Mutex delete failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_DeleteResource error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_DeleteResource End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

static UINT32 AmbaAENC_PlugInEncSetup(AMBA_AENC_HDLR *pAencHdlr)
{
    AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs = &(pAencHdlr->EncCs);
    UINT32 SubRtVal = OK;

    pAencHdlr->ErrorCode = (UINT32)AENC_OK;
    if (pPlugInCs->pSetUp_f != NULL) {
        SubRtVal = pPlugInCs->pSetUp_f(pPlugInCs);
        if (SubRtVal != OK) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0005;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AENC plug-in setup function failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pAencHdlr->PlugInPcmSize = pPlugInCs->ChNum * pPlugInCs->FrameSize * (pPlugInCs->Resolution / 8U);
            pAencHdlr->PlugInPcmRes = pPlugInCs->Resolution;
            pAencHdlr->PlugInPcmCh = pPlugInCs->ChNum;
            //AmbaPrint_PrintUInt5("[AUD]AENC plug-in required pcm size: cur: %d, req: %d", pAencHdlr->SrcPcmBufSize, pAencHdlr->PlugInPcmSize, 0U, 0U, 0U);
            if (pAencHdlr->PlugInPcmSize > pAencHdlr->SrcPcmBufSize) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC plug-in required pcm size is too large: cur: %d, req: %d", pAencHdlr->SrcPcmBufSize, pAencHdlr->PlugInPcmSize, 0U, 0U, 0U);
            }
        }
    } else {
        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_PlugInEncSetup error, NULL setup function", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaMisra_TouchUnused(pPlugInCs);
    return pAencHdlr->ErrorCode;
}

static UINT32 AmbaAENC_PlugInEncFrame(AMBA_AENC_HDLR *pAencHdlr)
{
    AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs = &(pAencHdlr->EncCs);
    UINT32 SubRtVal = OK;

    pAencHdlr->ErrorCode = (UINT32)AENC_OK;
    if (pPlugInCs->Update == 1U) {
        SubRtVal = AmbaWrap_memcpy(pPlugInCs->pSelf, pAencHdlr->pSelf, pPlugInCs->SelfSize);
        if (SubRtVal != OK) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0004;
        }
    }

    if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
        pPlugInCs->pSrc = pAencHdlr->pPcmBufRptr;
        pPlugInCs->pDst = pAencHdlr->pBitBufWptr;

        if (pPlugInCs->pProc_f != NULL) {
            SubRtVal = pPlugInCs->pProc_f(pPlugInCs);
            if (SubRtVal == OK) {
                pPlugInCs->Update = 0U;
                pAencHdlr->EncodedByte = pPlugInCs->EncodedBytes;
            } else {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0005;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AENC plug-in proc function failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        } else {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0000;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_PlugInEncFrame error, NULL proc function", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return pAencHdlr->ErrorCode;
}

/*! 32x32 multiplication, Y is always positive */
static UINT32 AmbaAENC_Mpy32x32 (UINT32 X, UINT32 Y)
{
    UINT64 Result;
    UINT32 Sign = 0U, Tmp_32;

    if (X >= 0x80000000U) {
        Sign = 1U;
        Tmp_32 = ~X;
        Result = (UINT64)Tmp_32 + 1U;
    } else {
        Result = (UINT64)X;
    }

    Result = (Result * (UINT64)Y) >> 32U;
    Result = ((Result + Result) & (UINT64)0x00000000ffffffffUL);

    if (Sign == 1U) {
        Result = (~Result) + 1U;
    }
    return (UINT32)Result;
}

/*! 16x32 multiplication, Y is always positive */
static UINT16 AmbaAENC_Mpy16x32 (UINT16 X, UINT32 Y)
{
    UINT64 Result;
    UINT16 Tmp_16;
    UINT32 Sign = 0U;

    if (X >= 0x8000U) {
        Sign = 1U;
        Tmp_16 = ~X;
        Result = (UINT64)Tmp_16 + 1U;
    } else {
        Result = (UINT64)X;
    }

    Result = (Result * (UINT64)Y) >> 32U;
    Result = ((Result + Result) & (UINT64)0x000000000000ffffUL);

    if (Sign == 1U) {
        Result = (~Result) + 1U;
    }

    return (UINT16)Result;
}

static UINT32 Audio_Qadd32(UINT32 Data1, UINT32 Data2)
{
#ifdef USE_ASM
    UINT32 Data3;
    __asm__ ("QADD %0, %1, %2\n":"=r"(Data3):"r"(Data1),"r"(Data2));

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

static UINT16 Audio_Qadd16(UINT16 Data1_16, UINT16 Data2_16)
{
#ifdef USE_ASM
    UINT16 Data3_16;
    __asm__ ("QADD16 %0, %1, %2\n":"=r"(Data3_16):"r"(Data1_16),"r"(Data2_16));

#else
    UINT16 Data3_16 = 0U;
    INT16 Data1_S16, Data2_S16, Data3_S16;
    INT32 Sum;
    UINT32 RetVal;

    RetVal = AmbaWrap_memcpy(&Data1_S16, &Data1_16, sizeof(UINT16));
    if (RetVal == OK) {
        RetVal = AmbaWrap_memcpy(&Data2_S16, &Data2_16, sizeof(UINT16));
        if (RetVal == OK) {
            Sum = (INT32)Data1_S16 + (INT32)Data2_S16;

            if(Sum > (INT32)0x00007fff) {//OVERFLOW++;
                Data3_S16 = (INT16)0x7fff;
            } else if(-Sum > (INT64)0x00007fff) {//OVERFLOW++;
                Data3_S16 = (INT16)0x8000;
            } else {
                Data3_S16 = (INT16)Sum;
            }
            RetVal = AmbaWrap_memcpy(&Data3_16, &Data3_S16, sizeof(UINT16));
            if (RetVal != OK) {
                Data3_16 = 0;
            }
        }
    }
#endif
    return Data3_16;
}

static void AmbaAENC_MixPCM(AMBA_AENC_HDLR *pAencHdlr, const UINT32 *pSrc, UINT32 Size)
{
    UINT32 Loop;
    UINT32 Data1 = 0U, Data2 = 0U, Data3 = 0U;
    UINT32 *pDst = pAencHdlr->pPcmBufRptr;
    const UINT32 *pSrc32;
    UINT16 Data1_16 = 0U, Data2_16 = 0U, Data3_16 = 0U;
    UINT16 *pDst16;
    const UINT16 *pSrc16;

    AmbaMisra_TypeCast(&pDst16, &pAencHdlr->pPcmBufRptr);
    AmbaMisra_TypeCast(&pSrc32, &pSrc);
    AmbaMisra_TypeCast(&pSrc16, &pSrc);

    if (pAencHdlr->PlugInPcmRes == 32U) {
        for (Loop = 0; Loop < (Size / (pAencHdlr->PlugInPcmRes / 8U)); Loop++) {
            Data1 = pDst[0];
            Data2 = pSrc32[0];
            pSrc32++;
            Data3 = Audio_Qadd32(Data1, Data2);
            *pDst = Data3;
            pDst++;
        }
    } else if (pAencHdlr->PlugInPcmRes == 16U) {
        for (Loop = 0; Loop < (Size / (pAencHdlr->PlugInPcmRes / 8U)); Loop++) {
            Data1_16 = pDst16[0];
            Data2_16 = pSrc16[0];
            pSrc16++;
            Data3_16 = Audio_Qadd16(Data1_16, Data2_16);
            *pDst16 = Data3_16;
            pDst16++;
        }
    } else {
        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
    }
}

static void AmbaAENC_GetPcmDone(AMBA_AENC_HDLR *pAencHdlr,
                                const AMBA_AENC_DATA_INFO_s *pInfo,
                                AMBA_AENC_IO_NODE_s *pIoNode,
                                UINT32 *pFirstNode,
                                UINT32 *pLofCounter)
{
    if (*pFirstNode == 0U) {
        AmbaAENC_MixPCM(pAencHdlr, pInfo->pPcmBuf, pInfo->Size);
    } else {
        *pFirstNode = 0U;
    }
    if (pIoNode->UseTickNum == 1U) {
        pAencHdlr->AudioTicks = pInfo->AudioTicks;
    }
    if (pInfo->Lof == 1U) {
        *pLofCounter = *pLofCounter + 1U;
        pIoNode->NodeStatus = (UINT32)AENC_IONODE_PAUSE;
    }
    if (pInfo->Size > pAencHdlr->InputCpSize) {
        pAencHdlr->InputCpSize = pInfo->Size;
    }
}

static void AmbaAENC_ScanInputChainImp(AMBA_AENC_HDLR *pAencHdlr, UINT32 *pLofCounter)
{
    UINT32  Loop;
    UINT32  FirstNode = 1U;
    AMBA_AENC_IO_NODE_s  *pIoNode;
    AMBA_AENC_DATA_INFO_s Info;
    UINT32 RemainSize;

    pIoNode = pAencHdlr->pIoNode;
    for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
        if (pIoNode->NodeStatus == (UINT32)AENC_IONODE_OPERATION) {
            Info.Size = pAencHdlr->PlugInPcmSize;
            if (FirstNode == 0U) {
                Info.pPcmBuf = pAencHdlr->pMixPcmBuf;
            } else {
                Info.pPcmBuf = pAencHdlr->pPcmBufRptr;
            }
            if (pIoNode->NodeCriteria == (UINT32)AENC_IONODE_BLOCKING) {
                if (pIoNode->pCbHdlr->GetPcm != NULL) {
                    if (pIoNode->pCbHdlr->GetPcm(&Info) != OK) {
                        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0005;
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                   "[AUD]AENC plug-in GetPcm function failed: ret: (0x%x)", pAencHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                        continue;
                    } else {
                        AmbaAENC_GetPcmDone(pAencHdlr, &Info, pIoNode, &FirstNode, pLofCounter);
                    }
                } else {
                    pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0000;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_ScanInputChain error, NULL GetPcm function", 0U, 0U, 0U, 0U, 0U);
                }
            } else if (pIoNode->NodeCriteria == (UINT32)AENC_IONODE_NONBLOCKING) {
                if (pIoNode->pCbHdlr->GetSize != NULL) {
                    if (pIoNode->pCbHdlr->GetSize(&RemainSize) != OK) {
                        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0005;
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                   "[AUD]AENC plug-in GetSize function failed: ret: (0x%x)", pAencHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                        continue;
                    } else {
                        if (RemainSize >= pAencHdlr->PlugInPcmSize) {
                            if (pIoNode->pCbHdlr->GetPcm != NULL) {
                                if (pIoNode->pCbHdlr->GetPcm(&Info) != OK) {
                                    pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0005;
                                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                               "[AUD]AENC plug-in GetPcm function failed: ret: (0x%x)", pAencHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                                    continue;
                                } else {
                                    AmbaAENC_GetPcmDone(pAencHdlr, &Info, pIoNode, &FirstNode, pLofCounter);
                                }
                            } else {
                                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0000;
                                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                           "[AUD]AmbaAENC_ScanInputChain error, NULL GetPcm function", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                } else {
                    pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0000;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_ScanInputChain error, NULL GetSize function", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_ScanInputChain error, wrong NodeCriteria", 0U, 0U, 0U, 0U, 0U);
            }
        }
        pIoNode++;
    }
}

static UINT32 AmbaAENC_ScanInputChain(AMBA_AENC_HDLR *pAencHdlr, UINT32 *pLof)
{
    UINT32  Loop;
    const AMBA_AENC_IO_NODE_s *pIoNode;
    UINT32 LofCounter = 0U;
    UINT32 ConnectAbuNum = 0U;
    UINT32 SubRtVal;

    pAencHdlr->ErrorCode = (UINT32)AENC_OK;
    pAencHdlr->InputCpSize = 0U;

    SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->IoNodeMutex), AMBA_AENC_TIMEOUT);
    if (SubRtVal != KAL_ERR_NONE) {
        pAencHdlr->ErrorCode = AENC_ERR_0003;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_ScanInputChain IoNode Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
    } else {
        /* Scan input chain */
        pIoNode = pAencHdlr->pIoNode;
        for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
            if (pIoNode->NodeStatus == (UINT32)AENC_IONODE_OPERATION) {
                ConnectAbuNum++;
            }
            pIoNode++;
        }
        SubRtVal = AmbaWrap_memset(pAencHdlr->pPcmBufRptr, 0, pAencHdlr->SrcPcmBufSize);
        if (SubRtVal == OK) {
            SubRtVal = AmbaWrap_memset(pAencHdlr->pMixPcmBuf, 0, pAencHdlr->SrcPcmBufSize);
            if (SubRtVal == OK) {
                AmbaAENC_ScanInputChainImp(pAencHdlr, &LofCounter);
                SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->IoNodeMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0003;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_ScanInputChain IoNode Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    if ((LofCounter == ConnectAbuNum) && (LofCounter > 0U)) {
        *pLof = 1U;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AENC: Scan Input Lof: %d %d %d", *pLof, LofCounter, ConnectAbuNum, 0U, 0U);
    } else {
        *pLof = 0U;
    }

    return pAencHdlr->ErrorCode;
}

static UINT32 AmbaAENC_GiveEvent(AMBA_AENC_HDLR *pAencHdlr, UINT32 EventId, void *pEventInfo)
{
    const AMBA_AENC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    const AMBA_AENC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 SubRtVal = OK;

    pAencHdlr->ErrorCode = (UINT32)AENC_OK;
    if (EventId >= (UINT32)AMBA_AENC_NUM_EVENT) {
        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_GiveEvent Event Id out of range: ID: (%u) > (%u)", EventId, (UINT32)AMBA_AENC_NUM_EVENT, 0U, 0U, 0U);
    }
    if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
        /* Take the Mutex */
        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->EventMutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_GiveEvent Event Cb Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            pEventHandlerCtrl = &pAencHdlr->pEventHandlerCtrl[EventId];/*pAencHdlr->pEventHandlerCtrl + EventId;*/
            k = pEventHandlerCtrl->MaxNumHandler;
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler != NULL) {
                    (void)(*pWorkEventHandler)(pEventInfo);   /* invoke the Event Handler */
                }
                pWorkEventHandler++;
            }
            /* Release the Mutex */
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->EventMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_GiveEvent Event Cb Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
    }
    AmbaMisra_TouchUnused(pEventInfo);

    return pAencHdlr->ErrorCode;
}

static UINT32 AmbaAENC_CheckStatus(AMBA_KAL_EVENT_FLAG_t *pFlag, UINT32 StatusFlag, UINT32 *pActualFlags)
{
    UINT32 RtVal = AENC_OK;
    UINT32 SubRtVal;

    *pActualFlags = 0U;

    SubRtVal = AmbaKAL_EventFlagGet(pFlag, StatusFlag, 0U/*or*/, 0U/*not clear*/,
                                    pActualFlags, (UINT32)AMBA_KAL_NO_WAIT);
    if (SubRtVal == KAL_ERR_NONE) {
        *pActualFlags &= StatusFlag;
    } else {
        *pActualFlags = 0U;
    }

    return RtVal;
}

static void AmbaAENC_FadeGainCal(AMBA_AENC_HDLR *pAencHdlr)
{
    UINT32 Loop, ChNum, Size;
    UINT32 Data1 = 0U, Data2 = 0U, Discard = 0U;
    UINT32 *pData = pAencHdlr->pPcmBufRptr;
    UINT16 Data1_16 = 0U;
    UINT16 *pData_16;
    UINT32 SubRtVal;

    SubRtVal = AmbaWrap_memcpy(&pData_16, &pAencHdlr->pPcmBufRptr, sizeof(pData_16));
    if (SubRtVal == OK) {
        Size = (pAencHdlr->InputCpSize / pAencHdlr->PlugInPcmCh) / (pAencHdlr->PlugInPcmRes / 8U);
        for (Loop = 0; Loop < Size; Loop++) {
            /* Fade gain calculation */
            if (pAencHdlr->FadeCtrl.State == AENC_FADE_STATE_FADEIN) {
                if (pAencHdlr->FadeCtrl.SmpCnt != 0U) {
                    pAencHdlr->FadeCtrl.CurrGain = pAencHdlr->FadeCtrl.CurrGain + pAencHdlr->FadeCtrl.Step;
                    //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_FadeProc FI: %d, SmpCnt: %d, Gain: 0x%x", Loop, pAencHdlr->FadeCtrl.SmpCnt, pAencHdlr->FadeCtrl.CurrGain, 0U, 0U);
                    pAencHdlr->FadeCtrl.SmpCnt--;
                    if (pAencHdlr->FadeCtrl.SmpCnt == 0U) {
                        pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEIDLE;
                        //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_FadeProc FI: %d", Loop, 0U, 0U, 0U, 0U);
                        break;
                    }
                }
            } else {
                if (pAencHdlr->FadeCtrl.Pad != 0U) {
                    //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_FadeProc FO: %d, Pad: %d", Loop, pAencHdlr->FadeCtrl.Pad, 0U, 0U, 0U);
                    pAencHdlr->FadeCtrl.Pad--;
                } else {
                    if (pAencHdlr->FadeCtrl.SmpCnt != 0U) {
                        pAencHdlr->FadeCtrl.CurrGain = pAencHdlr->FadeCtrl.CurrGain - pAencHdlr->FadeCtrl.Step;
                        //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_FadeProc FO: %d, SmpCnt: %d, Gain: 0x%x", Loop, pAencHdlr->FadeCtrl.SmpCnt, pAencHdlr->FadeCtrl.CurrGain, 0U, 0U);
                        pAencHdlr->FadeCtrl.SmpCnt--;
                        if(pAencHdlr->FadeCtrl.SmpCnt == 0U) {
                            pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEIDLE;
                            SubRtVal = AmbaKAL_EventFlagSet(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_FADEOUT_DONE);
                            if (SubRtVal != KAL_ERR_NONE) {
                                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0004;
                                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                           "[AUD]AmbaAENC_FadeProc Fade set fade out done flag failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                            }
                            //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_FadeProc FO: %d", Loop, 0U, 0U, 0U, 0U);
                            pAencHdlr->FadeCtrl.CurrGain = AENC_FADE_GAIN_MIN; /* Mute the remaining samples */
                        }
                    }
                }
            }

            for (ChNum = 0; ChNum < pAencHdlr->ChannelNum; ChNum++) {
                if (pAencHdlr->PlugInPcmRes == 32U) {
                    Data1 = pData[0];
                    Data1 = AmbaAENC_Mpy32x32(Data1, pAencHdlr->FadeCtrl.CurrGain);
                    *pData = Data1;
                    pData++;
                } else if (pAencHdlr->PlugInPcmRes == 16U) {
                    Data1_16 = pData_16[0];
                    Data1_16 = AmbaAENC_Mpy16x32(Data1_16, pAencHdlr->FadeCtrl.CurrGain);
                    *pData_16 = Data1_16;
                    pData_16++;
                } else {
                    pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                    break;

                }
            }
        }
    } else {
        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0004;
    }
    AmbaMisra_TouchUnused(&Data2);
    AmbaMisra_TouchUnused(&Discard);
}

static UINT32 AmbaAENC_FadeProc(AMBA_AENC_HDLR *pAencHdlr)
{
    UINT32 SubRtVal;

    SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->FadeCtrl.Mutex), AMBA_AENC_TIMEOUT);
    if (SubRtVal != KAL_ERR_NONE) {
        pAencHdlr->ErrorCode = AENC_ERR_0003;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_FadeProc Fade Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
    } else {
        if (pAencHdlr->FadeCtrl.State != AENC_FADE_STATE_FADEIDLE) {
            AmbaAENC_FadeGainCal(pAencHdlr);
        }
        if (pAencHdlr->ErrorCode == AENC_OK) {
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->FadeCtrl.Mutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_FadeProc Fade Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
    }

    return pAencHdlr->ErrorCode;
}

static void AmbaAENC_ProcEncImp(UINT32 *pHdlr, AMBA_AENC_HDLR *pAencHdlr, AMBA_AENC_AUDIO_DESC_s *pDesc, UINT32 Lof)
{
    UINT32 ChkBsBuf = 0U;
    UINT32 SubRtVal;

    if ((pAencHdlr->CurrentByte + pAencHdlr->EncCs.MaxBsSize) >= pAencHdlr->BsBufSize) {
        pAencHdlr->pBitBufWptr = pAencHdlr->pBitTempBuf;
        ChkBsBuf = 1U;
    }

    pAencHdlr->ErrorCode = AmbaAENC_PlugInEncFrame(pAencHdlr);

    if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
        pAencHdlr->FrameCount++;

        pDesc->pHdlr    = pHdlr;
        pDesc->Eos      = Lof;
        pDesc->DataSize = pAencHdlr->EncodedByte;
        if (ChkBsBuf == 1U) {
            pDesc->pBufAddr = (UINT8 *)&(pAencHdlr->pBsAddr[pAencHdlr->CurrentByte]);
            if ((pAencHdlr->CurrentByte + pAencHdlr->EncodedByte) >= pAencHdlr->BsBufSize) {
                SubRtVal = AmbaWrap_memcpy(&(pAencHdlr->pBsAddr[pAencHdlr->CurrentByte]), pAencHdlr->pBitTempBuf, (pAencHdlr->BsBufSize - pAencHdlr->CurrentByte));
                if (SubRtVal == OK) {
                    SubRtVal = AmbaWrap_memcpy(pAencHdlr->pBsAddr, &(pAencHdlr->pBitTempBuf[pAencHdlr->BsBufSize - pAencHdlr->CurrentByte]), (pAencHdlr->CurrentByte + pAencHdlr->EncodedByte - pAencHdlr->BsBufSize));
                    pAencHdlr->pBitBufWptr = (UINT8 *)&pAencHdlr->pBsAddr[(pAencHdlr->CurrentByte + pAencHdlr->EncodedByte - pAencHdlr->BsBufSize)];
                    pAencHdlr->CurrentByte = (pAencHdlr->CurrentByte + pAencHdlr->EncodedByte) - pAencHdlr->BsBufSize;
                    //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc 3: %d", ChkBsBuf, 0U, 0U, 0U, 0U);
                }
            } else {
                SubRtVal = AmbaWrap_memcpy(&(pAencHdlr->pBsAddr[pAencHdlr->CurrentByte]), pAencHdlr->pBitTempBuf, pAencHdlr->EncodedByte);
                pAencHdlr->CurrentByte = pAencHdlr->CurrentByte + pAencHdlr->EncodedByte;
                pAencHdlr->pBitBufWptr = (UINT8 *)&pAencHdlr->pBsAddr[pAencHdlr->CurrentByte];/*pAencHdlr->pBitBufWptr + pAencHdlr->EncodedByte;*/
                //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc 4: %d", ChkBsBuf, 0U, 0U, 0U, 0U);
            }
            if (SubRtVal != OK) {
                pAencHdlr->ErrorCode = AENC_ERR_0004;
            }
        } else {
            pDesc->pBufAddr = pAencHdlr->pBitBufWptr;
            pAencHdlr->CurrentByte = pAencHdlr->CurrentByte + pAencHdlr->EncodedByte;
            pAencHdlr->pBitBufWptr = (UINT8 *)&pAencHdlr->pBsAddr[pAencHdlr->CurrentByte];/*pAencHdlr->pBitBufWptr + pAencHdlr->EncodedByte;*/
            //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc 5: %d", ChkBsBuf, 0U, 0U, 0U, 0U);
        }
        pDesc->AudioTicks = pAencHdlr->AudioTicks;
        pDesc->EncodedSamples = pAencHdlr->FrameCount * pAencHdlr->EncCs.FrameSize;

        pAencHdlr->ErrorCode = AmbaAENC_GiveEvent(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME, pDesc);
        if (pAencHdlr->ErrorCode != (UINT32)AENC_OK) {
            AmbaPrint_ModulePrintStr5(AENC_MODULE_ID, "[AUD][%s] (AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME) AmbaAENC_GiveEvent fail!!",__func__, NULL, NULL, NULL, NULL);
        }
    }
}

/**
* Audio encoder main function
* @param [in]  pHdlr Handle of the audio encoder resource
* @return ErrorCode
*/
UINT32 AmbaAENC_ProcEnc(UINT32 *pHdlr)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 Lof;
    UINT32 ActualFlags;
    AMBA_AENC_AUDIO_DESC_s *pDesc;
    UINT32 RetVal = AENC_OK;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        if (pAencHdlr->Stop == 0U) {
            pAencHdlr->ErrorCode = AmbaAENC_ScanInputChain(pAencHdlr, &Lof);

            /* Fade process */
            if (pAencHdlr->ErrorCode == AENC_OK) {
                pAencHdlr->ErrorCode = AmbaAENC_FadeProc(pAencHdlr);
            }
            if (pAencHdlr->ErrorCode == AENC_OK) {
                pAencHdlr->ErrorCode = AmbaAENC_CheckStatus(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_FADEOUT_DONE | AMBA_AUDIO_ENC_FLG_STOP, &ActualFlags);

                if (pAencHdlr->ErrorCode == AENC_OK) {
                    if (ActualFlags == (AMBA_AUDIO_ENC_FLG_FADEOUT_DONE | AMBA_AUDIO_ENC_FLG_STOP)) {
                        pAencHdlr->Stop = 1U;
                        Lof = 1U;
                    }
                    //AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc: %d %d %d %d", pAencHdlr->ErrorCode, pAencHdlr->CurrentByte, pAencHdlr->EncCs.MaxBsSize, pAencHdlr->BsBufSize, 0U);
                    pDesc = &pAencHdlr->Desc;
                    if (pAencHdlr->InputCpSize == pAencHdlr->PlugInPcmSize) {
                        AmbaAENC_ProcEncImp(pHdlr, pAencHdlr, pDesc, Lof);
                    } else {
                        if (Lof == 1U) {
                            pDesc->pHdlr    = pHdlr;
                            pDesc->Eos      = Lof;
                            pDesc->DataSize = 0U;
                            pDesc->pBufAddr = NULL;
                            pDesc->AudioTicks = pAencHdlr->AudioTicks;
                            pDesc->EncodedSamples = pAencHdlr->FrameCount * pAencHdlr->EncCs.FrameSize;
                            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc Lof with zero data size", 0U, 0U, 0U, 0U, 0U);
                            pAencHdlr->ErrorCode = AmbaAENC_GiveEvent(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME, pDesc);
                            if (pAencHdlr->ErrorCode != (UINT32)AENC_OK) {
                                AmbaPrint_ModulePrintStr5(AENC_MODULE_ID, "[AUD][%s] (AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME) AmbaAENC_GiveEvent fail!!",__func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            RetVal = AmbaKAL_TaskSleep(1);
                            /* non-blocking and wait for new data */
                            //AmbaPrint_PrintUInt5("[AUD]AmbaAENC_ProcEnc: waiting, pAencHdlr->InputCpSize: %d", pAencHdlr->InputCpSize, 0U, 0U, 0U, 0U);
                        }
                    }

                    if ((Lof == 1U) && (pAencHdlr->ErrorCode == AENC_OK)) {
                        pAencHdlr->ErrorCode = AmbaAENC_GiveEvent(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_STOP, pAencHdlr);
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AENC (AMBA_AENC_EVENT_ID_ENCODE_STOP), ret (0x%x)", pAencHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                    }
                }
            }
        } else {
          RetVal = AmbaKAL_TaskSleep(1);
          AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_ProcEnc: Stop: %d", pAencHdlr->Stop, 0U, 0U, 0U, 0U);
        }
        if (RetVal != OK) {
            RetVal = AENC_ERR_0004;
            pAencHdlr->ErrorCode = RetVal;
        }
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_ProcEnc error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/**
* Audio encoder start function
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  FadeInTime Fade-in time of the encoder (unit: ms)
* @return ErrorCode
*/
UINT32 AmbaAENC_Start(const UINT32 *pHdlr, UINT32 FadeInTime)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->FrameCount = 0U;
        pAencHdlr->Stop = 0U;
        pAencHdlr->ErrorCode = AmbaAENC_PlugInEncSetup(pAencHdlr);

        if (pAencHdlr->ErrorCode == AENC_OK) {
            SubRtVal = AmbaKAL_EventFlagClear(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_STOP);
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0004;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_Start claer stop flag failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            } else {
                SubRtVal = AmbaKAL_EventFlagClear(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_FADEOUT_DONE);
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0004;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_Start claer fade out flag failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }

            if (pAencHdlr->ErrorCode == AENC_OK) {
                SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->FadeCtrl.Mutex), AMBA_AENC_TIMEOUT);
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0003;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_Start fade mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                } else {
                    pAencHdlr->FadeCtrl.FadeInTime = FadeInTime;
                    if (FadeInTime != 0U) {
                        pAencHdlr->FadeCtrl.CurrGain = AENC_FADE_GAIN_MIN;
                        pAencHdlr->FadeCtrl.DestGain = AENC_FADE_GAIN_0dB;
                        pAencHdlr->FadeCtrl.SmpCnt = (pAencHdlr->SampleFreq * FadeInTime) / 1000U; /* FadeInTime in ms */
                        pAencHdlr->FadeCtrl.Pad = 0U;
                        if (pAencHdlr->FadeCtrl.SmpCnt != 0U) {
                            pAencHdlr->FadeCtrl.Step = (pAencHdlr->FadeCtrl.DestGain - pAencHdlr->FadeCtrl.CurrGain) / pAencHdlr->FadeCtrl.SmpCnt;
                        } else {
                            pAencHdlr->FadeCtrl.Step = 0U;
                            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                       "[AUD]AmbaAENC_Start invalid fade status", 0U, 0U, 0U, 0U, 0U);
                        }
                        pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEIN;
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_Start Fade: %d 0x%x", pAencHdlr->FadeCtrl.SmpCnt, pAencHdlr->FadeCtrl.Step, 0U, 0U, 0U);
                    } else {
                        pAencHdlr->FadeCtrl.CurrGain = AENC_FADE_GAIN_0dB;
                        pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEIDLE;
                    }
                    SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->FadeCtrl.Mutex));
                    if (SubRtVal != KAL_ERR_NONE) {
                        pAencHdlr->ErrorCode = AENC_ERR_0003;
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                   "[AUD]AmbaAENC_Start fade mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
        RetVal = pAencHdlr->ErrorCode;

    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_Start error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_Start End: ret: (0x%x), FadeInTime(%d)", RetVal, FadeInTime, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder stop function
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  FadeOutTime Fade-out time of the encoder (unit: ms)
* @return ErrorCode
*/
UINT32 AmbaAENC_Stop(const UINT32 *pHdlr, UINT32 FadeOutTime)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->FadeCtrl.Mutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_Stop fade mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pAencHdlr->FadeCtrl.FadeOutTime = FadeOutTime;
            if (FadeOutTime != 0U) {
                pAencHdlr->FadeCtrl.DestGain = AENC_FADE_GAIN_MIN;
                pAencHdlr->FadeCtrl.SmpCnt = (pAencHdlr->SampleFreq * FadeOutTime) / 1000U; /* FadeInTime in ms */
                pAencHdlr->FadeCtrl.Pad = pAencHdlr->FrameSize - (pAencHdlr->FadeCtrl.SmpCnt % pAencHdlr->FrameSize);
                if (pAencHdlr->FadeCtrl.SmpCnt != 0U) {
                    pAencHdlr->FadeCtrl.Step = (pAencHdlr->FadeCtrl.CurrGain - pAencHdlr->FadeCtrl.DestGain) / pAencHdlr->FadeCtrl.SmpCnt;
                } else {
                    pAencHdlr->FadeCtrl.Step = 0U;
                    pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_Stop invalid fade status", 0U, 0U, 0U, 0U, 0U);
                }
                pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEOUT;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID, "[AUD]AmbaAENC_Stop Fade: %d 0x%x pad: %d", pAencHdlr->FadeCtrl.SmpCnt, pAencHdlr->FadeCtrl.Step, pAencHdlr->FadeCtrl.Pad, 0U, 0U);
            } else {
                pAencHdlr->FadeCtrl.State = AENC_FADE_STATE_FADEIDLE;
                SubRtVal = AmbaKAL_EventFlagSet(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_FADEOUT_DONE);
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0004;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_Stop fade out done event flag set failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->FadeCtrl.Mutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_Stop fade mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }

        SubRtVal = AmbaKAL_EventFlagSet(&(pAencHdlr->Flag), AMBA_AUDIO_ENC_FLG_STOP);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0004;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_Stop stop event flag set failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        }

        RetVal = pAencHdlr->ErrorCode;

    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_Stop error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_Stop End: ret: (0x%x), FadeOutTime(%d)", RetVal, FadeOutTime, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder register the input chain scanner callback functions
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @param [in]  UseTickNum AENC will use the tick number of the I/O node for the ¡§AudioTicks¡¨
* @param [in]  Criteria Criteria of the I/O node. 0: blocking, 1: non-blocking
* @return ErrorCode
*/
UINT32 AmbaAENC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_AENC_CB_HDLR_s *pCbHdlr, UINT32 UseTickNum, UINT32 Criteria)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 Loop = 0U;
    AMBA_AENC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;

        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->IoNodeMutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_RegisterCallBackFunc IoNode mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pIoNode = pAencHdlr->pIoNode;
            for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == NULL) {
                    pIoNode->pCbHdlr = pCbHdlr;
                    pIoNode->NodeStatus = (UINT32)AENC_IONODE_PAUSE;
                    pIoNode->NodeCriteria = Criteria;
                    pIoNode->UseTickNum = UseTickNum;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_RegisterCallBackFunc IoNode mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            if (Loop == pAencHdlr->IoNodeNum) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_RegisterCallBackFunc Invalid status: Loop (%u), IoNodeNum (%u)", Loop, pAencHdlr->IoNodeNum, 0U, 0U, 0U);
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_RegisterCallBackFunc error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_RegisterCallBackFunc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder de-register the input chain scanner callback functions
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAENC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 Loop= 0U;
    AMBA_AENC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;

        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->IoNodeMutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_DeRegisterCallBackFunc IoNode mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pIoNode = pAencHdlr->pIoNode;
            for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->pCbHdlr = NULL;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_DeRegisterCallBackFunc IoNode mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            if (Loop == pAencHdlr->IoNodeNum) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_DeRegisterCallBackFunc Invalid status: Loop (%u), IoNodeNum (%u)", Loop, pAencHdlr->IoNodeNum, 0U, 0U, 0U);
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_DeRegisterCallBackFunc error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_DeRegisterCallBackFunc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder open the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAENC_OpenIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AENC_HDLR *pAencHdlr;
    AMBA_AENC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->IoNodeMutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_OpenIoNode IoNode mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pIoNode = pAencHdlr->pIoNode;
            for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AENC_IONODE_OPERATION;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_OpenIoNode IoNode mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            if (Loop == pAencHdlr->IoNodeNum) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_OpenIoNode Invalid status: Loop (%u), IoNodeNum (%u)", Loop, pAencHdlr->IoNodeNum, 0U, 0U, 0U);
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_OpenIoNode error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_OpenIoNode End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder close the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pCbHdlr Handler of the input chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaAENC_CloseIoNode(const UINT32 *pHdlr, const AMBA_AENC_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_AENC_HDLR *pAencHdlr;
    AMBA_AENC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->IoNodeMutex), AMBA_AENC_TIMEOUT);
        if (SubRtVal != KAL_ERR_NONE) {
            pAencHdlr->ErrorCode = AENC_ERR_0003;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_CloseIoNode IoNode mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
        } else {
            pIoNode = pAencHdlr->pIoNode;
            for (Loop = 0; Loop < pAencHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)AENC_IONODE_PAUSE;
                    break;
                }
                pIoNode++;
            }
            SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->IoNodeMutex));
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_CloseIoNode IoNode mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            if (Loop == pAencHdlr->IoNodeNum) {
                pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_CloseIoNode Invalid status: Loop (%u), IoNodeNum (%u)", Loop, pAencHdlr->IoNodeNum, 0U, 0U, 0U);
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_CloseIoNode error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_CloseIoNode End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder setup the bitstream buffer assigned from users
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pBsAddr The start address of the bitstream buffer
* @param [in]  BsBufSize The size of the bitstream buffer
* @return ErrorCode
*/
UINT32 AmbaAENC_SetUpBsBuffer(const UINT32 *pHdlr, UINT32 *pBsAddr, UINT32 BsBufSize)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK;

    if ((pHdlr != NULL) && (pBsAddr != NULL) && (BsBufSize != 0U)) {

        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        pAencHdlr->pBsAddr = (UINT8 *)pBsAddr;
        pAencHdlr->pBitBufWptr = (UINT8 *)pAencHdlr->pBsAddr;
        pAencHdlr->BsBufSize = BsBufSize;
        pAencHdlr->CurrentByte = 0;

        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_SetUpBsBuffer error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_SetUpBsBuffer End: ret: (0x%x) BsBufSize(%d)", RetVal, BsBufSize, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder configure control Settings for the AENC event handler
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  EventId Event ID of the AENC event
* @param [in]  MaxNumHandler Max number of Handlers
* @param [in]  pEventHandlers Pointer of the Event Handlers
* @return ErrorCode
*/
UINT32 AmbaAENC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_AENC_EVENT_HANDLER_f *pEventHandlers)
{
    AMBA_AENC_HDLR *pAencHdlr;
    AMBA_AENC_EVENT_HANDLER_CTRL_s *pWorkEventHandler;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if ((pHdlr != NULL) && (pEventHandlers != NULL) && (MaxNumHandler != 0U)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        if (EventId >= (UINT32)AMBA_AENC_NUM_EVENT) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_ConfigEventHdlr Event Id out of range: ID: (%u) > (%u)", EventId, (UINT32)AMBA_AENC_NUM_EVENT, 0U, 0U, 0U);
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->EventMutex), AMBA_AENC_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_ConfigEventHdlr Event Cb Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
            if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
                pWorkEventHandler = &pAencHdlr->pEventHandlerCtrl[EventId];
                pWorkEventHandler->MaxNumHandler = MaxNumHandler;   /* maximum number of Handlers */
                pWorkEventHandler->pEventHandler = pEventHandlers;  /* pointer to the Event Handlers */
                /* Release the Mutex */
                SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0003;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_ConfigEventHdlr Event Cb Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_ConfigEventHdlr error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_ConfigEventHdlr End: ret: (0x%x), EventId(%d), MaxNumHandler(%d)",
                               RetVal,
                               EventId,
                               MaxNumHandler, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder register a specified audio event handler
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  EventId Event ID of the AENC event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAENC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_AENC_EVENT_HANDLER_f EventHandler)
{
    AMBA_AENC_HDLR *pAencHdlr;
    const AMBA_AENC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AENC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        if (EventId >= (UINT32)AMBA_AENC_NUM_EVENT) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_RegisterEventHdlr Event Id out of range: ID: (%u) > (%u)", EventId, (UINT32)AMBA_AENC_NUM_EVENT, 0U, 0U, 0U);
        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->EventMutex), AMBA_AENC_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_RegisterEventHdlr Event Cb Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
            if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
                pEventHandlerCtrl = &pAencHdlr->pEventHandlerCtrl[EventId];
                k = pEventHandlerCtrl->MaxNumHandler;

                /* check to see if it is already registered */
                pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
                for (i = 0; i < k; i++) {
                    if (*pWorkEventHandler == EventHandler) {
                        pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;  /* it is already registered, why ? */
                        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                                   "[AUD]AmbaAENC_RegisterEventHdlr Invalid status: it is already registered, why ?", 0U, 0U, 0U, 0U, 0U);
                    }
                    pWorkEventHandler++;
                }
                if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
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
                SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0003;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_RegisterEventHdlr Event Cb Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }
        }
        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_RegisterEventHdlr error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_RegisterEventHdlr End: ret: (0x%x), EventId(%d)", RetVal, EventId, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder de-register a specified audio event handler
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  EventId Event ID of the AENC event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaAENC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, const AMBA_AENC_EVENT_HANDLER_f EventHandler)
{
    AMBA_AENC_HDLR *pAencHdlr;
    const AMBA_AENC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_AENC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;
    UINT32 RetVal = AENC_OK, SubRtVal;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        pAencHdlr->ErrorCode = (UINT32)AENC_OK;
        if (EventId >= (UINT32)AMBA_AENC_NUM_EVENT) {
            pAencHdlr->ErrorCode = (UINT32)AENC_ERR_0001;
            AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                       "[AUD]AmbaAENC_DeRegisterEventHdlr Event Id out of range: ID: (%u) > (%u)", EventId, (UINT32)AMBA_AENC_NUM_EVENT, 0U, 0U, 0U);

        }
        if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
            /* Take the Mutex */
            SubRtVal = AmbaKAL_MutexTake(&(pAencHdlr->EventMutex), AMBA_AENC_TIMEOUT);
            if (SubRtVal != KAL_ERR_NONE) {
                pAencHdlr->ErrorCode = AENC_ERR_0003;
                AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                           "[AUD]AmbaAENC_DeRegisterEventHdlr Event Cb Mutex take failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
            }
            if (pAencHdlr->ErrorCode == (UINT32)AENC_OK) {
                pEventHandlerCtrl = &pAencHdlr->pEventHandlerCtrl[EventId];
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
                SubRtVal = AmbaKAL_MutexGive(&(pAencHdlr->EventMutex));
                if (SubRtVal != KAL_ERR_NONE) {
                    pAencHdlr->ErrorCode = AENC_ERR_0003;
                    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                               "[AUD]AmbaAENC_DeRegisterEventHdlr Event Cb Mutex give failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
                }
            }
        }

        RetVal = pAencHdlr->ErrorCode;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_DeRegisterEventHdlr error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_DeRegisterEventHdlr End: ret: (0x%x), EventId(%d)", RetVal, EventId, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder get the latest control structure setting of the plug-in encoder
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [out]  pPlugInConfig Memory address to save the control structure settings of the plug-in encoder
* @return ErrorCode
*/
UINT32 AmbaAENC_GetPlugInEncConfig(const UINT32 *pHdlr, void *pPlugInConfig)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInConfig != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(pPlugInConfig, pAencHdlr->pSelf, pAencHdlr->EncCs.SelfSize);
        if (SubRtVal != OK) {
            RetVal = AENC_ERR_0004;
        }
        pAencHdlr->ErrorCode = RetVal;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_GetPlugInEncConfig error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_GetPlugInEncConfig End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio encoder install the plug-in encoder
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pPlugInCs Plug-in encoder common control structure
* @return ErrorCode
*/
UINT32 AmbaAENC_InstallPlugInEnc(const UINT32 *pHdlr, const AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInCs != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(&pAencHdlr->EncCs, pPlugInCs, sizeof(AMBA_AENC_PLUGIN_ENC_CS_s));
        if (SubRtVal != OK) {
            RetVal = AENC_ERR_0004;
        }
        pAencHdlr->ErrorCode = RetVal;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_InstallPlugInEnc error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_InstallPlugInEnc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio encoder update the plug-in encoder
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pPlugInConfig Memory address to save the control structure settings of the plug-in encoder
* @return ErrorCode
*/
UINT32 AmbaAENC_UpdatePlugInEnc(const UINT32 *pHdlr, const void *pPlugInConfig)
{
    AMBA_AENC_HDLR *pAencHdlr;
    UINT32 RetVal = AENC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInConfig != NULL)) {
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(pAencHdlr->pSelf, pPlugInConfig, pAencHdlr->EncCs.SelfSize);
        if (SubRtVal != OK) {
            RetVal = AENC_ERR_0004;
        } else {
            pAencHdlr->EncCs.Update = 1U;
        }
        pAencHdlr->ErrorCode = RetVal;
    } else {
        RetVal = AENC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                                   "[AUD]AmbaAENC_UpdatePlugInEnc error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(AENC_MODULE_ID,
                               "[AUD]AmbaAENC_UpdatePlugInEnc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}
