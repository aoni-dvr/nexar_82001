/**
 *  @file AmbaAudio_ADEC.c
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
 *  @details Audio decoder process functions.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_ADEC.h"

#define AMBA_ADEC_TIMEOUT 1000U

/**
* Audio decoder query working buffer size function
* @param [in]  pInfo Audio decoder resource creation information
* @return ErrorCode
*/
UINT32 AmbaADEC_QueryBufSize (AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo)
{
    UINT32 RetVal = (UINT32)ADEC_OK;
    if (pInfo != NULL) {
        pInfo->CachedBufSize = (UINT32)sizeof(AMBA_ADEC_HDLR) +
                               ((UINT32)sizeof(AMBA_ADEC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_ADEC_NUM_EVENT) +
                               ((UINT32)sizeof(AMBA_ADEC_IO_NODE_s) * pInfo->IoNodeNum) +
                               pInfo->PlugInLibSelfSize +
                               pInfo->NeededBytes +
                               (pInfo->ChannelNum * pInfo->FrameSize * pInfo->SampleResolution / 8U);
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_QueryBufSize End: Size: (%u)", pInfo->CachedBufSize, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Null pInfo.",__func__, NULL, NULL, NULL, NULL);
        RetVal = (UINT32)ADEC_ERR_0000;
    }
    return RetVal;
}

static UINT32 AmbaADEC_GetMem(AMBA_ADEC_MEM_INFO_s *pInfo, UINT32 Size, UINT32 **pMemAddr)
{
    UINT32 MemSize;
    UINT32 RetVal = (UINT32)ADEC_OK;
    UINT32 *pPtr;

    if ((Size % sizeof(UINT32)) != 0U) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC]Not 4-ByteAlignment size: %d", Size, 0U, 0U, 0U, 0U);
        RetVal = (UINT32)ADEC_ERR_0002;
    } else {
        MemSize = pInfo->CurrentSize + Size;
        if (MemSize > pInfo->MaxSize) {
            *pMemAddr = NULL;
            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC]Out of memory. MaxSize: %d  NeedSize: %d", pInfo->MaxSize, MemSize, 0U, 0U, 0U);
            RetVal = (UINT32)ADEC_ERR_0001;
        } else {
            pInfo->CurrentSize = MemSize;
            *pMemAddr = pInfo->pHead;
            pPtr = pInfo->pHead;
            pInfo->pHead = &(pPtr[Size/sizeof(UINT32)]);/*pInfo->pHead += (Size/sizeof(UINT32));*/
        }
    }
    return RetVal;
}

static UINT32 AmbaADEC_CreateRescSysInfo(const AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo,
                                         AMBA_ADEC_HDLR *pAdecHdlr,
                                         AMBA_ADEC_MEM_INFO_s *pMemInfo)
{
    UINT32 RetVal = ADEC_OK;
    UINT32 Loop;
    UINT32 *pMemAddr;
    AMBA_ADEC_IO_NODE_s  *pIoNode;
    static char AdecEventFlagName[30]       = "AdecEventFlags";
    static char AdecEventCbMutexName[25]    = "AdecEventCbMutex";
    static char AdecIoNodeMutexName[20]     = "AdecIoNodeMutex";

    /* create the Audio task Status Flags */
    if (AmbaKAL_EventFlagCreate(&(pAdecHdlr->Flag), AdecEventFlagName) != OK) {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagCreate fail",__func__, NULL, NULL, NULL, NULL);
        RetVal = (UINT32)ADEC_ERR_0004;
    } else {
        if (AmbaKAL_EventFlagClear(&(pAdecHdlr->Flag), 0xffffffffU) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagClear fail",__func__, NULL, NULL, NULL, NULL);
            RetVal = (UINT32)ADEC_ERR_0004;
        }
    }

    /* Create event call back Mutex */
    if (RetVal == (UINT32)ADEC_OK) {
        if (AmbaKAL_MutexCreate(&(pAdecHdlr->EventMutex), AdecEventCbMutexName) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Event AmbaKAL_MutexCreate fail",__func__, NULL, NULL, NULL, NULL);
            RetVal = (UINT32)ADEC_ERR_0003;
        } else {
            RetVal = AmbaADEC_GetMem(pMemInfo, (UINT32)sizeof(AMBA_ADEC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_ADEC_NUM_EVENT, &pMemAddr);
            if (RetVal != (UINT32)ADEC_OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Create event call back event handler fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAdecHdlr->pEventHandlerCtrl), &pMemAddr);
                RetVal = AmbaWrap_memset(pAdecHdlr->pEventHandlerCtrl, 0, sizeof(AMBA_ADEC_EVENT_HANDLER_CTRL_s) * (UINT32)AMBA_ADEC_NUM_EVENT);
            }
        }
    }

    /* Create I/O node Mutex */
    if (RetVal == (UINT32)ADEC_OK) {
        if (AmbaKAL_MutexCreate(&(pAdecHdlr->IoNodeMutex), AdecIoNodeMutexName) != OK) {
            RetVal = (UINT32)ADEC_ERR_0003;
        } else {
            pAdecHdlr->IoNodeNum = pInfo->IoNodeNum;
            RetVal = AmbaADEC_GetMem(pMemInfo, (UINT32)sizeof(AMBA_ADEC_IO_NODE_s) * pInfo->IoNodeNum, &pMemAddr);
            if (RetVal != (UINT32)ADEC_OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Create I/O node fail",__func__, NULL, NULL, NULL, NULL);
            } else {
                AmbaMisra_TypeCast(&(pAdecHdlr->pIoNode), &pMemAddr);
                pIoNode = pAdecHdlr->pIoNode;
                for (Loop = 0; Loop < pInfo->IoNodeNum; Loop++) {
                    pIoNode->pCbHdlr = NULL;
                    pIoNode++;
                }
            }
        }
    }

    return RetVal;
}

/**
* Audio decoder resource creation function
* @param [in]  pInfo Audio decoder create information
* @param [in]  pCachedInfo Cached buffer information
* @param [out]  pHdlr Pointer of the audio decoder resource
* @return ErrorCode
*/
UINT32 AmbaADEC_CreateResource(const AMBA_AUDIO_DEC_CREATE_INFO_s *pInfo, const AMBA_ADEC_BUF_INFO_s *pCachedInfo, UINT32 **pHdlr)
{
    UINT32 RetVal;
    AMBA_ADEC_HDLR *pAdecHdlr = NULL;
    AMBA_ADEC_MEM_INFO_s MemInfo;
    UINT32 *pMemAddr;

    if ((pInfo != NULL) && (pCachedInfo != NULL)) {
        /* Cached buffer */
        MemInfo.CurrentSize = 0U;
        MemInfo.MaxSize = pCachedInfo->MaxSize;
        MemInfo.pHead = pCachedInfo->pHead;

        /* create ADEC handler */
        RetVal = AmbaADEC_GetMem(&(MemInfo), (UINT32)sizeof(AMBA_ADEC_HDLR), &pMemAddr);
        if (RetVal != (UINT32)ADEC_OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Create ADEC handler fail",__func__, NULL, NULL, NULL, NULL);
            *pHdlr = NULL;
        } else {
            AmbaMisra_TypeCast(&pAdecHdlr, &pMemAddr);
            *pHdlr = pMemAddr;
        }

        /* create the Audio task related info */
        if ((RetVal == (UINT32)ADEC_OK) && (pAdecHdlr != NULL)) {
            RetVal = AmbaADEC_CreateRescSysInfo(pInfo, pAdecHdlr, &(MemInfo));
        }

        /* Create self config memory for plug-in Codec */
        if ((RetVal == (UINT32)ADEC_OK) && (pAdecHdlr != NULL)) {
            RetVal = AmbaADEC_GetMem(&(MemInfo), pInfo->PlugInLibSelfSize, &pMemAddr);
            if (RetVal == (UINT32)ADEC_OK) {
                AmbaMisra_TypeCast(&(pAdecHdlr->pSelf), &pMemAddr);
            } else {
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                           "[AUD]ADEC create self config memory failed", 0U, 0U, 0U, 0U, 0U);
            }
        }

        /* Create Data Buffer */
        if ((RetVal == (UINT32)ADEC_OK) && (pAdecHdlr != NULL)) {
            RetVal = AmbaADEC_GetMem(&(MemInfo), pInfo->NeededBytes, &pMemAddr);
            if (RetVal != (UINT32)ADEC_OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Create Data Buffer fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->pDataBuf = NULL;
            } else {
                AmbaMisra_TypeCast(&(pAdecHdlr->pDataBuf), &pMemAddr);
                pAdecHdlr->NeededBytes = pInfo->NeededBytes;
            }
        }

        /* Create Pcm Buffer */
        if ((RetVal == (UINT32)ADEC_OK) && (pAdecHdlr != NULL)) {
            RetVal = AmbaADEC_GetMem(&(MemInfo), pInfo->ChannelNum * pInfo->FrameSize * pInfo->SampleResolution / 8U, &pMemAddr);
            if (RetVal != (UINT32)ADEC_OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Create Pcm Buffer fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->pPcmBufWptr = NULL;
            } else {
                AmbaMisra_TypeCast(&(pAdecHdlr->pPcmBufWptr), &pMemAddr);
            }
        }

        if (pAdecHdlr != NULL) {
            pAdecHdlr->ErrorCode = RetVal;
        }

    } else {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Null inputs.",__func__, NULL, NULL, NULL, NULL);
        RetVal = ADEC_ERR_0000;
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_CreateResource End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio decoder resource delete function
* @param [in]  pHdlr Handle of the audio decoder resource
* @return ErrorCode
*/
UINT32 AmbaADEC_DeleteResource(const UINT32 *pHdlr)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = ADEC_OK;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;

        if (AmbaKAL_EventFlagDelete(&(pAdecHdlr->Flag)) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagDelete fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0004;
        }

        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (AmbaKAL_MutexDelete(&(pAdecHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Event AmbaKAL_MutexDelete fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }

        /* delete I/O node Mutex */
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (AmbaKAL_MutexDelete(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] I/O node AmbaKAL_MutexDelete fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_DeleteResource error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_DeleteResource End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

static UINT32 AmbaADEC_PlugInDecSetup(AMBA_ADEC_HDLR *pAdecHdlr)
{
    AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs = &(pAdecHdlr->DecCs);

    pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
    if (pPlugInCs->pSetUp_f != NULL) {
        if (pPlugInCs->pSetUp_f(pPlugInCs) != (UINT32)ADEC_OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Adec setup fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0005;
        }
    } else {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Adec null setup func pointer",__func__, NULL, NULL, NULL, NULL);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0000;
    }

    AmbaMisra_TouchUnused(pPlugInCs);
    return pAdecHdlr->ErrorCode;
}

static UINT32 AmbaADEC_PlugInDecFrame(AMBA_ADEC_HDLR *pAdecHdlr)
{
    AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs = &(pAdecHdlr->DecCs);
    UINT32 RetVal = (UINT32)ADEC_OK;

    if ((pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) && (pAdecHdlr->Idling == 0U)) {
        if (pPlugInCs->Update == 1U) {
            RetVal = AmbaWrap_memcpy(pPlugInCs->pSelf, pAdecHdlr->pSelf, pPlugInCs->SelfSize);
        }

        if (RetVal == (UINT32)ADEC_OK) {
            pPlugInCs->pSrc = pAdecHdlr->pBitBufRptr;
            pPlugInCs->pDst = pAdecHdlr->pPcmBufWptr;
            pAdecHdlr->ConsumedBytes = 0U;
            if (pAdecHdlr->Lof == 0U) {
                pPlugInCs->PureAudioBufCurSize = pAdecHdlr->NeededBytes;
            } else {
                pPlugInCs->PureAudioBufCurSize = pAdecHdlr->CurrentSize;
            }

            if (pPlugInCs->pProc_f != NULL) {
                if (pPlugInCs->PureAudioBufCurSize > 0U) {
                    pAdecHdlr->ErrorCode = pPlugInCs->pProc_f(pPlugInCs);
                    pAdecHdlr->ConsumedBytes = pPlugInCs->ConsumedBytes;
                    if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
                        pPlugInCs->Update = 0U;
                    } else {
                        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]Adec process frame fail. Plug-In decoder ErrorCode: %d", pAdecHdlr->ErrorCode, 0U, 0U, 0U, 0U);
                        RetVal = (UINT32)ADEC_ERR_0005;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] No data to decode(eos)",__func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Adec null proc func pointer",__func__, NULL, NULL, NULL, NULL);
                RetVal = (UINT32)ADEC_ERR_0000;
            }
        }
    }

    if (pAdecHdlr->Lof == 1U) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]Adec process Plug-In decoder %d %d", pPlugInCs->PureAudioBufCurSize, pAdecHdlr->ConsumedBytes, 0U, 0U, 0U);
    }

    return RetVal;
}

static UINT32 AmbaADEC_GiveEvent(AMBA_ADEC_HDLR *pAdecHdlr, UINT32 EventId, void *pEventInfo)
{
    const AMBA_ADEC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    const AMBA_ADEC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;

    pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
    if (EventId >= (UINT32)AMBA_ADEC_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]ADEC EventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_ADEC_NUM_EVENT, 0U, 0U, 0U);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
    }
    if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&(pAdecHdlr->EventMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            pEventHandlerCtrl = &pAdecHdlr->pEventHandlerCtrl[EventId];/*pAdecHdlr->pEventHandlerCtrl + EventId;*/
            k = pEventHandlerCtrl->MaxNumHandler;
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler != NULL) {
                    (void)(*pWorkEventHandler)(pEventInfo);   /* invoke the Event Handler */
                }
                pWorkEventHandler++;
            }
            /* Release the Mutex */
            if (AmbaKAL_MutexGive(&(pAdecHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
    }
    return pAdecHdlr->ErrorCode;
}

static UINT32 AmbaADEC_CheckStatus(AMBA_KAL_EVENT_FLAG_t *pFlag, UINT32 StatusFlag, UINT32 *pActualFlags)
{
    UINT32 RtVal = ADEC_OK;
    UINT32 SubRtVal;

    *pActualFlags = 0U;

    SubRtVal = AmbaKAL_EventFlagGet(pFlag, StatusFlag, 0U/*or*/, 0U/*not clear*/,
                                    pActualFlags, (UINT32)AMBA_KAL_NO_WAIT);
    if (SubRtVal == KAL_ERR_NONE) {
        *pActualFlags &= StatusFlag;
    } else if (SubRtVal == KAL_ERR_TIMEOUT) {
        *pActualFlags = 0U;
    } else {
        RtVal = ADEC_ERR_0004;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_CheckStatus Event get failed: ret: (0x%x)", SubRtVal, 0U, 0U, 0U, 0U);
    }

    return RtVal;
}

static UINT32 AmbaADEC_CheckBitStream(AMBA_ADEC_HDLR *pAdecHdlr)
{
    UINT32 RetVal = (UINT32)ADEC_OK;

    if (pAdecHdlr->Idling == 0U) {
        if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            RetVal = (UINT32)ADEC_ERR_0003;
        } else {
            if (pAdecHdlr->CurrentSize > pAdecHdlr->NeededBytes) {
                if (pAdecHdlr->NeededBytes <= pAdecHdlr->RptrRemainSize) { // if ((pAdecHdlr->pBsRptr + pAdecHdlr->NeededBytes) <= ((UINT8 *)pAdecHdlr->pBsAddr + pAdecHdlr->BsBufSize)) {
                    pAdecHdlr->pBitBufRptr = pAdecHdlr->pBsRptr;
                } else { /* Move data from bs buffer to data buffer */
                    RetVal = AmbaWrap_memcpy(pAdecHdlr->pDataBuf, pAdecHdlr->pBsRptr, pAdecHdlr->RptrRemainSize);
                    if (RetVal == (UINT32)ADEC_OK) {
                        RetVal = AmbaWrap_memcpy(&(pAdecHdlr->pDataBuf[pAdecHdlr->RptrRemainSize]), pAdecHdlr->pBsAddr, (ULONG)pAdecHdlr->NeededBytes - (ULONG)pAdecHdlr->RptrRemainSize);
                        pAdecHdlr->pBitBufRptr = pAdecHdlr->pDataBuf;
                        //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "  Move data from bs buffer to data buffer!!  Size=%d", pAdecHdlr->NeededBytes, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                pAdecHdlr->Lof = 1U;
                if (pAdecHdlr->CurrentSize <= pAdecHdlr->RptrRemainSize) { //if ((pAdecHdlr->pBsRptr + pAdecHdlr->CurrentSize) <= ((UINT8 *)pAdecHdlr->pBsAddr + pAdecHdlr->BsBufSize)) {
                    pAdecHdlr->pBitBufRptr = pAdecHdlr->pBsRptr;
                } else { /* Move data from bs buffer to data buffer */
                    RetVal = AmbaWrap_memcpy(pAdecHdlr->pDataBuf, pAdecHdlr->pBsRptr, pAdecHdlr->RptrRemainSize);
                    if (RetVal == (UINT32)ADEC_OK) {
                        RetVal = AmbaWrap_memcpy(&(pAdecHdlr->pDataBuf[pAdecHdlr->RptrRemainSize]), &(pAdecHdlr->pBsAddr), (ULONG)pAdecHdlr->CurrentSize - (ULONG)pAdecHdlr->RptrRemainSize);
                        pAdecHdlr->pBitBufRptr = pAdecHdlr->pDataBuf;
                        //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "  Move data from bs buffer to data buffer!!  Size=%d", pAdecHdlr->CurrentSize, 0U, 0U, 0U, 0U);
                    }
                }
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][AmbaADEC_CheckBitStream]End of bitstream %d %d %d", pAdecHdlr->CurrentSize, pAdecHdlr->RptrRemainSize, pAdecHdlr->NeededBytes, 0U, 0U);
            }

            if (RetVal == (UINT32)ADEC_OK) {
                if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                    AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                    RetVal = (UINT32)ADEC_ERR_0003;
                }
            }
        }
    }

    //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][AmbaADEC_CheckBitStream]End of bitstream %d %d %d", pAdecHdlr->CurrentSize, pAdecHdlr->RptrRemainSize, pAdecHdlr->NeededBytes, 0U, 0U);

    pAdecHdlr->ErrorCode = RetVal;

    return RetVal;
}

static UINT32 AmbaADEC_UpdateBsBufRp(AMBA_ADEC_HDLR *pAdecHdlr)
{
    UINT32 RetVal;
    UINT32 *pHdlr;
    AMBA_ADEC_AUDIO_DESC_s BitsBufInfo;

    AmbaMisra_TypeCast(&pHdlr, &pAdecHdlr);
    //Offset = pAdecHdlr->BsBufSize - (pAdecHdlr->pBsRptr - (UINT8 *)pAdecHdlr->pBsAddr);
    if (pAdecHdlr->ConsumedBytes < pAdecHdlr->RptrRemainSize) { //if ((pAdecHdlr->pBsRptr + pAdecHdlr->ConsumedBytes) < ((UINT8 *)pAdecHdlr->pBsAddr + pAdecHdlr->BsBufSize)) {
        pAdecHdlr->pBsRptr = &(pAdecHdlr->pBsRptr[pAdecHdlr->ConsumedBytes]);
        pAdecHdlr->RptrRemainSize -= pAdecHdlr->ConsumedBytes;
    } else if (pAdecHdlr->ConsumedBytes == pAdecHdlr->RptrRemainSize) { //else if ((pAdecHdlr->pBsRptr + pAdecHdlr->ConsumedBytes) == ((UINT8 *)pAdecHdlr->pBsAddr + pAdecHdlr->BsBufSize)) {
        pAdecHdlr->RptrRemainSize = pAdecHdlr->BsBufSize;
        pAdecHdlr->pBsRptr = pAdecHdlr->pBsAddr;
    } else {
        pAdecHdlr->RptrRemainSize = pAdecHdlr->BsBufSize - (pAdecHdlr->ConsumedBytes - pAdecHdlr->RptrRemainSize);
        pAdecHdlr->pBsRptr = &(pAdecHdlr->pBsAddr[(pAdecHdlr->BsBufSize - pAdecHdlr->RptrRemainSize)]);
    }
    if (pAdecHdlr->CurrentSize > pAdecHdlr->ConsumedBytes) {
        pAdecHdlr->CurrentSize -= pAdecHdlr->ConsumedBytes;
    } else {
        pAdecHdlr->CurrentSize = 0U;
    }
    BitsBufInfo.pHdlr    = pHdlr;
    BitsBufInfo.Eos      = pAdecHdlr->ScanOutputChainLof;
    BitsBufInfo.DataSize = pAdecHdlr->ConsumedBytes;
    RetVal = AmbaADEC_GiveEvent(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME, &BitsBufInfo);
    if (RetVal != (UINT32)ADEC_OK) {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] (AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME) AmbaADEC_GiveEvent fail!!",__func__, NULL, NULL, NULL, NULL);
    }
    if (pAdecHdlr->ScanOutputChainLof == 1U) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]AmbaADEC_UpdateBsBufRp Last of frame, %d Lof(%d)", pAdecHdlr->ConsumedBytes, pAdecHdlr->ScanOutputChainLof, 0U, 0U, 0U);
    }
    return RetVal;
}

static UINT32 AmbaADEC_IoNodePutPcmLoop(AMBA_ADEC_HDLR *pAdecHdlr, UINT32 DecodeOutputBytes, UINT32 *pLof, UINT32 PreFillFrameNum)
{
    UINT32 Loop;
    UINT32 RetVal = (UINT32)ADEC_OK;
    const UINT32 *pHdlr;
    UINT32 ActualFlags;
    const AMBA_ADEC_IO_NODE_s *pIoNode;

    AmbaMisra_TypeCast(&pHdlr, &pAdecHdlr);
    pIoNode = pAdecHdlr->pIoNode;
    for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
        if (pIoNode->NodeStatus == (UINT32)ADEC_IONODE_OPERATION) {
            /* PutPcm callback */
            if (pIoNode->pCbHdlr->PutPcm != NULL) {
                if (pAdecHdlr->CurrentSize == pAdecHdlr->ConsumedBytes) {
                    *pLof = 1U;
                }
                if (pAdecHdlr->CurrentSize < pAdecHdlr->ConsumedBytes) {
                    *pLof = 1U;
                    RetVal = AmbaWrap_memset(pAdecHdlr->pPcmBufWptr, 0, DecodeOutputBytes);
                }
                if (RetVal == (UINT32)ADEC_OK) {
                    if (pIoNode->pCbHdlr->PutPcm(pHdlr, pAdecHdlr->pPcmBufWptr, DecodeOutputBytes, pLof) != (UINT32)ADEC_OK) {
                        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][I/O node %d]callback PutPcm fail!!", Loop, 0U, 0U, 0U, 0U);
                        RetVal = (UINT32)ADEC_ERR_0005;
                    } else {
                        //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]ADEC [I/O node %d]callback PutPcm %d, room: %d", Loop, DecodeOutputBytes, RoomSize, 0U, 0U);
                        RetVal = (UINT32)ADEC_OK;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] No register PutPcm callback!!",__func__, NULL, NULL, NULL, NULL);
                RetVal = (UINT32)ADEC_ERR_0000;
            }
        }
        pIoNode++;
    }

    if (RetVal == (UINT32)ADEC_OK) {
        RetVal = AmbaADEC_CheckStatus(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_PREFILL, &ActualFlags);
        if (RetVal == (UINT32)ADEC_OK) {
            if ((ActualFlags & AMBA_ADEC_FLG_PREFILL) == 0U) {
                pAdecHdlr->PreFillCount++;
                if ((pAdecHdlr->PreFillCount >= PreFillFrameNum) || (*pLof == 1U)) {
                    if (AmbaKAL_EventFlagSet(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_PREFILL) != KAL_ERR_NONE) {
                        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_EventFlagSet fail!!",__func__, NULL, NULL, NULL, NULL);
                        RetVal = (UINT32)ADEC_ERR_0004;
                    } else {
                        RetVal = AmbaADEC_GiveEvent(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_READY, pAdecHdlr);
                        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]ADEC (AMBA_ADEC_EVENT_ID_DECODE_READY), ret (0x%x), %d, %d, Lof(%d)", RetVal, PreFillFrameNum, pAdecHdlr->PreFillCount, *pLof, 0U);
                    }
                }
            }
        }
    }

    return RetVal;
}

static UINT32 AmbaADEC_ScanOutputChain(AMBA_ADEC_HDLR *pAdecHdlr, UINT32 PreFillFrameNum)
{
    UINT32 Loop;
    UINT32 ActualFlags;
    UINT32 DecodeOutputBytes = 0U;
    UINT32 RoomSize = 0U;
    const AMBA_ADEC_IO_NODE_s  *pIoNode;
    const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInDec;
    UINT32 RetVal = (UINT32)ADEC_OK;
    const UINT32 *pHdlr;
    UINT32 ConnectAbuNum = 0U;

    if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
        RetVal = (UINT32)ADEC_ERR_0003;
    } else {
        RetVal = AmbaADEC_CheckStatus(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_STOP, &ActualFlags);
        if (RetVal == (UINT32)ADEC_OK) {
            if (((ActualFlags & AMBA_ADEC_FLG_STOP) != 0U) && (pAdecHdlr->Eos == 0U)) {
                pAdecHdlr->ScanOutputChainLof = 1U;
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]ADEC get Stop cmd", 0U, 0U, 0U, 0U, 0U);
            }
            /* Scan output chain */
            AmbaMisra_TypeCast(&pHdlr, &pAdecHdlr);
            pAdecHdlr->Idling = 0U;
            pIoNode = pAdecHdlr->pIoNode;
            pPlugInDec = &(pAdecHdlr->DecCs);
            DecodeOutputBytes = pPlugInDec->FrameSize * pPlugInDec->ChNum * pPlugInDec->Resolution / 8U;
            for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
                if (pIoNode->NodeStatus == (UINT32)ADEC_IONODE_OPERATION) {
                    if (pIoNode->pCbHdlr->GetSize != NULL) {
                        /* GetSize callback */
                        if (pIoNode->pCbHdlr->GetSize(pHdlr, &RoomSize) != (UINT32)ADEC_OK) {
                            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC][I/O node %d]callback GetSize fail!!  RoomSize=%d", Loop, RoomSize, 0U, 0U, 0U);
                            RetVal = (UINT32)ADEC_ERR_0005;
                            continue;
                        } else {
                            if (RoomSize < DecodeOutputBytes) {
                                pAdecHdlr->Idling = 1U;
                                RetVal = AmbaKAL_TaskSleep(1);
                                //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "  [GetSize] Get insufficient size!!  Need: %d  Get: %d  ADEC idling one frame!!!!",(pPlugInDec->FrameSize * pPlugInDec->ChNum * pPlugInDec->Resolution / 8U), RoomSize, 0U, 0U, 0U);
                                break;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] No register GetSize callback!!",__func__, NULL, NULL, NULL, NULL);
                        RetVal = (UINT32)ADEC_ERR_0000;
                    }
                    ConnectAbuNum++;
                }
                pIoNode++;
            }

            if ((pAdecHdlr->Idling == 0U) || (ConnectAbuNum == 0U)) {
                if (ConnectAbuNum > 0U) {
                    RetVal = AmbaADEC_IoNodePutPcmLoop(pAdecHdlr, DecodeOutputBytes, &pAdecHdlr->ScanOutputChainLof, PreFillFrameNum);
                    /* Update bitstream buffer read pointer */
                    if (RetVal == (UINT32)ADEC_OK) {
                        RetVal = AmbaADEC_UpdateBsBufRp(pAdecHdlr);
                    }
                }
                if (RetVal == (UINT32)ADEC_OK) {
                    if (pAdecHdlr->ScanOutputChainLof == 1U) {
                        pAdecHdlr->DecStop = 1U;
                        RetVal = AmbaADEC_GiveEvent(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_STOP, pAdecHdlr);
                        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]ADEC (AMBA_ADEC_EVENT_ID_DECODE_STOP), ret (0x%x)", RetVal, 0U, 0U, 0U, 0U);
                    }
                }
            }

            if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                RetVal = (UINT32)ADEC_ERR_0003;
            }
        }
    }

    pAdecHdlr->ErrorCode = RetVal;
    /*AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "AmbaADEC_ScanOutputChain  End!!  ErrorCode=%d  ConsumedBytes=%d ScanOutputChainLof(%d) idle(%d) Lof(%d)",
            pAdecHdlr->ErrorCode, pAdecHdlr->ConsumedBytes, pAdecHdlr->ScanOutputChainLof, pAdecHdlr->Idling, pAdecHdlr->ScanOutputChainLof);*/
    return RetVal;
}

/**
* Audio decoder main function
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  PreFillFrameNum The number of pre-filled frame for the output I/O node
* @return ErrorCode
*/
UINT32 AmbaADEC_ProcDec(const UINT32 *pHdlr, UINT32 PreFillFrameNum)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = (UINT32)ADEC_OK;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        if (pAdecHdlr->DecStop == 0U) {
            if (pAdecHdlr->Idling == 0U) {
                RetVal = AmbaADEC_CheckBitStream(pAdecHdlr);
                if (RetVal == ADEC_OK) {
                    RetVal = AmbaADEC_PlugInDecFrame(pAdecHdlr);
                }
            }
            if (RetVal == ADEC_OK) {
                RetVal = AmbaADEC_ScanOutputChain(pAdecHdlr, PreFillFrameNum);
            }
            pAdecHdlr->ErrorCode = RetVal;
        } else {
            RetVal = AmbaKAL_TaskSleep(10000);
            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]AmbaADEC_ProcDec set Lof to output already", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_ProcDec error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "  End!!  ErrorCode=%d", pAdecHdlr->ErrorCode, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder start function
* @param [in]  pHdlr Handle of the audio decoder resource
* @return ErrorCode
*/
UINT32 AmbaADEC_Start(const UINT32 *pHdlr)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);

        /* Initialization */
        pAdecHdlr->Eos           = 1U;
        pAdecHdlr->ConsumedBytes = 0U;
        pAdecHdlr->Idling        = 0U;
        pAdecHdlr->PreFillCount  = 0U;
        pAdecHdlr->Lof           = 0U;
        pAdecHdlr->ScanOutputChainLof = 0U;
        pAdecHdlr->DecStop       = 0U;
        if (AmbaKAL_EventFlagClear(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_PREFILL) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] (AMBA_ADEC_FLG_PREFILL) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0004;
        } else {
            if (AmbaKAL_EventFlagClear(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_STOP) != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] (AMBA_ADEC_FLG_STOP) AmbaKAL_EventFlagClear fail!!",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0004;
            }
        }

        /* Plug-in decoder setup */
        pAdecHdlr->ErrorCode = AmbaADEC_PlugInDecSetup(pAdecHdlr);
        if (pAdecHdlr->ErrorCode != (UINT32)ADEC_OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][ADEC]Audio plug-in decoder setup fail!!", NULL, NULL, NULL, NULL, NULL);
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_Start error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_Start End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder stop function
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  Eos The flag to stop ADEC until get the end of stream
* @return ErrorCode
*/
UINT32 AmbaADEC_Stop(const UINT32 *pHdlr, UINT32 Eos)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if (pHdlr != NULL) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        if (Eos == 0U) {
            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC] Decode stop by cmd!!  Eos=%d", Eos, 0U, 0U, 0U, 0U);
            pAdecHdlr->Eos = Eos;
            if (AmbaKAL_EventFlagSet(&(pAdecHdlr->Flag), AMBA_ADEC_FLG_STOP) != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] (AMBA_ADEC_FLG_STOP) AmbaKAL_EventFlagSet fail!!",__func__, NULL, NULL, NULL, NULL);
                RetVal = (UINT32)ADEC_ERR_0004;
            }
        } else if (Eos == 1U) {
            pAdecHdlr->Eos = Eos;
            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD] Decode stop by eos!!  Eos=%d", Eos, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD] Wrong eos setting!!  Eos=%d", Eos, 0U, 0U, 0U, 0U);
            RetVal = (UINT32)ADEC_ERR_0001;
        }

        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_Stop error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_Stop End: ret: (0x%x), Eos(%d)", RetVal, Eos, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder register the output chain scanner callback functions
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaADEC_RegisterCallBackFunc(const UINT32 *pHdlr, AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 Loop = 0U;
    AMBA_ADEC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
        if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        } else {
            pIoNode = pAdecHdlr->pIoNode;
            for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == NULL) {
                    pIoNode->pCbHdlr = pCbHdlr;
                    pIoNode->NodeStatus = (UINT32)ADEC_IONODE_PAUSE;
                    break;
                }
                pIoNode++;
            }
            if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (Loop == pAdecHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[Register] out of the max I/O node number: %d", pAdecHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
            }
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_RegisterCallBackFunc error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_RegisterCallBackFunc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder de-register the output chain scanner callback functions
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaADEC_DeRegisterCallBackFunc(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 Loop= 0U;
    AMBA_ADEC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
        if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        } else {
            pIoNode = pAdecHdlr->pIoNode;
            for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->pCbHdlr = NULL;
                    break;
                }
                pIoNode++;
            }
            if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (Loop == pAdecHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC][De-register] out of the max I/O node number: %d", pAdecHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
            }
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_DeRegisterCallBackFunc error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_DeRegisterCallBackFunc End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio decoder open the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaADEC_OpenIoNode(const UINT32 *pHdlr, const AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_ADEC_HDLR *pAdecHdlr;
    AMBA_ADEC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
        if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        } else {
            pIoNode = pAdecHdlr->pIoNode;
            for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)ADEC_IONODE_OPERATION;
                    break;
                }
                pIoNode++;
            }
            if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (Loop == pAdecHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC][Open] out of the max I/O node number: %d", pAdecHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
            }
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_OpenIoNode error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_OpenIoNode End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio decoder close the I/O node with the registered callback function
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pCbHdlr Handler of the output chain scanner callback functions
* @return ErrorCode
*/
UINT32 AmbaADEC_CloseIoNode(const UINT32 *pHdlr,const  AMBA_ADEC_CB_HDLR_s *pCbHdlr)
{
    UINT32 Loop = 0U;
    AMBA_ADEC_HDLR *pAdecHdlr;
    AMBA_ADEC_IO_NODE_s  *pIoNode;
    UINT32 RetVal = (UINT32) ADEC_OK;

    if ((pHdlr != NULL) && (pCbHdlr != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
        if (AmbaKAL_MutexTake(&(pAdecHdlr->IoNodeMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        } else {
            pIoNode = pAdecHdlr->pIoNode;
            for (Loop = 0; Loop < pAdecHdlr->IoNodeNum; Loop++) {
                if (pIoNode->pCbHdlr == pCbHdlr) {
                    pIoNode->NodeStatus = (UINT32)ADEC_IONODE_PAUSE;
                    break;
                }
                pIoNode++;
            }
            if (AmbaKAL_MutexGive(&(pAdecHdlr->IoNodeMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            if (Loop == pAdecHdlr->IoNodeNum) {
                AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC][Close] out of the max I/O node number: %d", pAdecHdlr->IoNodeNum, 0U, 0U, 0U, 0U);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
            }
        }
        RetVal = pAdecHdlr->ErrorCode;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_CloseIoNode error, NULL argument", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_CloseIoNode End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio decoder setup the bitstream buffer assigned from users
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pBsAddr The start address of the bitstream buffer
* @param [in]  BsBufSize The size of the bitstream buffer
* @return ErrorCode
*/
UINT32 AmbaADEC_SetUpBsBuffer(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 BsBufSize)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = (UINT32)ADEC_OK;

    if ((pHdlr != NULL) && (pBsAddr != NULL) && (BsBufSize != 0U)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->pBsAddr = pBsAddr;
        pAdecHdlr->pBsRptr = pBsAddr;
        pAdecHdlr->pBsWptr = pBsAddr;
        pAdecHdlr->BsBufSize = BsBufSize;
        pAdecHdlr->CurrentSize = 0;
        pAdecHdlr->RptrRemainSize = BsBufSize;

        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_SetUpBsBuffer error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_SetUpBsBuffer End: ret: (0x%x) BsBufSize(%d)", RetVal, BsBufSize, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder update the bit-stream buffer write pointer
* @param [in]  pHdlr Handle of the audio encoder resource
* @param [in]  pBsAddr The start address of the bitstream buffer
* @param [in]  UpdateSize The updated size of the bitstream buffer
* @return ErrorCode
*/
UINT32 AmbaADEC_UpdateBsBufWp(const UINT32 *pHdlr, UINT8 *pBsAddr, UINT32 UpdateSize)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = (UINT32)ADEC_OK;

    if ((pHdlr != NULL) && (pBsAddr != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        pAdecHdlr->pBsWptr = &(pBsAddr[UpdateSize]);
        pAdecHdlr->CurrentSize += UpdateSize;
        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = (UINT32)ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                  "[AUD]AmbaADEC_UpdateBsBufWp error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    //AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
    //                           "[AUD]AmbaADEC_UpdateBsBufWp End: ret: (0x%x) BsBufSize(%d)", RetVal, UpdateSize, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder configure control Settings for the ADEC event handler
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  EventId Event ID of the ADEC event
* @param [in]  MaxNumHandler Max number of Handlers
* @param [in]  pEventHandlers Pointer of the Event Handlers
* @return ErrorCode
*/
UINT32 AmbaADEC_ConfigEventHdlr(const UINT32 *pHdlr, UINT32 EventId, UINT32 MaxNumHandler, AMBA_ADEC_EVENT_HANDLER_f *pEventHandlers)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    AMBA_ADEC_EVENT_HANDLER_CTRL_s *pWorkEventHandler;

    /*AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "Func: [%s]",__func__, NULL, NULL, NULL, NULL);*/
    /*AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "  Start!!", NULL, NULL, NULL, NULL, NULL);*/
    AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
    pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
    if (EventId >= (UINT32)AMBA_ADEC_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]Config eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_ADEC_NUM_EVENT, 0U, 0U, 0U);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
    }
    if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&(pAdecHdlr->EventMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            pWorkEventHandler = &pAdecHdlr->pEventHandlerCtrl[EventId];
            pWorkEventHandler->MaxNumHandler = MaxNumHandler;   /* maximum number of Handlers */
            pWorkEventHandler->pEventHandler = pEventHandlers;  /* pointer to the Event Handlers */
            /* Release the Mutex */
            if (AmbaKAL_MutexGive(&(pAdecHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_ConfigEventHdlr End: ret: (0x%x), EventId(%d), MaxNumHandler(%d)",
                               pAdecHdlr->ErrorCode,
                               EventId,
                               MaxNumHandler, 0U, 0U);

    return pAdecHdlr->ErrorCode;
}

/**
* Audio decoder register a specified audio event handler
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  EventId Event ID of the ADEC event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaADEC_RegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    const AMBA_ADEC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_ADEC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;

    //AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "Func: [%s]",__func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "  Start!!", NULL, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
    pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
    if (EventId >= (UINT32)AMBA_ADEC_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD][ADEC]Register eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_ADEC_NUM_EVENT, 0U, 0U, 0U);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
    }
    if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&(pAdecHdlr->EventMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            pEventHandlerCtrl = &pAdecHdlr->pEventHandlerCtrl[EventId];
            k = pEventHandlerCtrl->MaxNumHandler;

            /* check to see if it is already registered */
            pWorkEventHandler = pEventHandlerCtrl->pEventHandler;
            for (i = 0; i < k; i++) {
                if (*pWorkEventHandler == EventHandler) {
                    AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] Event already registered.",__func__, NULL, NULL, NULL, NULL);
                    pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;  /* it is already registered, why ? */
                }
                pWorkEventHandler++;
            }
            if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
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
            if (AmbaKAL_MutexGive(&(pAdecHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_RegisterEventHdlr End: ret: (0x%x), EventId(%d)", pAdecHdlr->ErrorCode, EventId, 0U, 0U, 0U);

    return pAdecHdlr->ErrorCode;
}

/**
* Audio decoder de-register a specified audio event handler
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  EventId Event ID of the ADEC event
* @param [in]  EventHandler Event Handler
* @return ErrorCode
*/
UINT32 AmbaADEC_DeRegisterEventHdlr(const UINT32 *pHdlr, UINT32 EventId, AMBA_ADEC_EVENT_HANDLER_f EventHandler)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    const AMBA_ADEC_EVENT_HANDLER_CTRL_s *pEventHandlerCtrl;
    AMBA_ADEC_EVENT_HANDLER_f      *pWorkEventHandler;
    UINT32 i, k;

    //AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "Func: [%s]",__func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "  Start!!", NULL, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
    pAdecHdlr->ErrorCode = (UINT32)ADEC_OK;
    if (EventId >= (UINT32)AMBA_ADEC_NUM_EVENT) {
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID, "[AUD]De-register eventId out of range. EventId: %d  Max: %d", EventId, (UINT32)AMBA_ADEC_NUM_EVENT, 0U, 0U, 0U);
        pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0001;
    }
    if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&(pAdecHdlr->EventMutex), AMBA_ADEC_TIMEOUT) != OK) {
            AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexTake fail",__func__, NULL, NULL, NULL, NULL);
            pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
        }
        if (pAdecHdlr->ErrorCode == (UINT32)ADEC_OK) {
            pEventHandlerCtrl = &pAdecHdlr->pEventHandlerCtrl[EventId];
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
            if (AmbaKAL_MutexGive(&(pAdecHdlr->EventMutex)) != OK) {
                AmbaPrint_ModulePrintStr5(ADEC_MODULE_ID, "[AUD][%s] AmbaKAL_MutexGive fail",__func__, NULL, NULL, NULL, NULL);
                pAdecHdlr->ErrorCode = (UINT32)ADEC_ERR_0003;
            }
        }
    }

    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_DeRegisterEventHdlr End: ret: (0x%x), EventId(%d)", pAdecHdlr->ErrorCode, EventId, 0U, 0U, 0U);

    return pAdecHdlr->ErrorCode;
}

/**
* Audio decoder install the plug-in decoder
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pPlugInCs Plug-in decoder common control structure
* @return ErrorCode
*/
UINT32 AmbaADEC_InstallPlugInDec(const UINT32 *pHdlr, const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = ADEC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInCs != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(&pAdecHdlr->DecCs, pPlugInCs, sizeof(AMBA_ADEC_PLUGIN_DEC_CS_s));
        if (SubRtVal != OK) {
            RetVal = ADEC_ERR_0004;
        }
        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_InstallPlugInDec error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_InstallPlugInDec End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* Audio decoder get the latest control structure setting of the plug-in decoder
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [out]  pPlugInSelf get the latest control structure setting of the plug-in decoder
* @return ErrorCode
*/
UINT32 AmbaADEC_GetPlugInDecConfig(const UINT32 *pHdlr, void *pPlugInSelf)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = ADEC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInSelf != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(pPlugInSelf, pAdecHdlr->pSelf, pAdecHdlr->DecCs.SelfSize);
        if (SubRtVal != OK) {
            RetVal = ADEC_ERR_0004;
        }
        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_GetPlugInDecConfig error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_GetPlugInDecConfig End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);

    return RetVal;
}

/**
* Audio decoder update the plug-in decoder
* @param [in]  pHdlr Handle of the audio decoder resource
* @param [in]  pPlugInSelf Memory address to save the control structure settings of the plug-in decoder
* @return ErrorCode
*/
UINT32 AmbaADEC_UpdatePlugInDec(const UINT32 *pHdlr, const void *pPlugInSelf)
{
    AMBA_ADEC_HDLR *pAdecHdlr;
    UINT32 RetVal = ADEC_OK, SubRtVal = OK;

    if ((pHdlr != NULL) && (pPlugInSelf != NULL)) {
        AmbaMisra_TypeCast(&pAdecHdlr, &pHdlr);
        SubRtVal = AmbaWrap_memcpy(pAdecHdlr->pSelf, pPlugInSelf, pAdecHdlr->DecCs.SelfSize);
        if (SubRtVal != OK) {
            RetVal = ADEC_ERR_0004;
        } else {
            pAdecHdlr->DecCs.Update = 1U;
        }
        pAdecHdlr->ErrorCode = RetVal;
    } else {
        RetVal = ADEC_ERR_0000;
        AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                                   "[AUD]AmbaADEC_UpdatePlugInDec error, NULL arguments", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_ModulePrintUInt5(ADEC_MODULE_ID,
                               "[AUD]AmbaADEC_UpdatePlugInDec End: ret: (0x%x)", RetVal, 0U, 0U, 0U, 0U);
    return RetVal;
}
