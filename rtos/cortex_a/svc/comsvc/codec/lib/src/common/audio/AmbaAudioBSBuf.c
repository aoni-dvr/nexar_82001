/**
*  @file AmbaAudioBSBuf.c
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
*  @details amba audio bitstream buffer
*
*/

#include "AmbaDMA_Def.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaNVM_Partition.h"
#include "AmbaUtility.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaCodecCom.h"

AMBA_AUDIO_MUX_CTRL_s   AudioMuxHdlr[AMBA_AUDIO_MAX_MUX_NUM] GNU_SECTION_NOZEROINIT;
AMBA_AUDIO_DEMUX_CTRL_s AudioDemuxHdlr[AMBA_AUDIO_MAX_DEMUX_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_FS_FILE        *pAFile GNU_SECTION_NOZEROINIT;
static AMBA_AUD_FROM_ROM_s *pAudFromRom GNU_SECTION_NOZEROINIT;

void AmbaAudio_MuxIdHdrInit(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_MUX_NUM; i++) {
        AudioMuxHdlr[i].MuxIndex = AU_MUXER_NA_ID;
        if (OK != AmbaWrap_memset(&AudioMuxHdlr[i].Desc[0], 0, sizeof(AMBA_AENC_DATA_INFO_s) * AU_MUX_DESC_NUM)) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset Desc fail",__func__, NULL, NULL, NULL, NULL);
        }

    }
}

static UINT32 CheckAvailAudioMuxId(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_MUX_NUM; i++) {
        if (AudioMuxHdlr[i].MuxIndex == AU_MUXER_NA_ID) {
            AudioMuxHdlr[i].MuxIndex = i;
            break;
        }
    }
    if (i == AMBA_AUDIO_MAX_MUX_NUM) {
        i = AU_MUXER_NA_ID;
    }
    return i;
}

static void DeleteAudioMuxId(UINT32 Id)
{
    AudioMuxHdlr[Id].MuxIndex = AU_MUXER_NA_ID;
}

AMBA_AENC_AUDIO_DESC_s *Audio_MuxGetReadPointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr)
{
    UINT32 RtVal;
    AMBA_AENC_AUDIO_DESC_s *pDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RemainSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("Audio_MuxGetReadPointer AmbaKAL_SemaphoreTake error ", 0U, 0U, 0U, 0U, 0U);
    }
    pDesc = &(pHdlr->Desc[pHdlr->ReadIdx]);

    /* Update the Read Pointer */
    if (++pHdlr->ReadIdx >= AU_MUX_DESC_NUM) {
        pHdlr->ReadIdx = 0;
    }

    return pDesc;
}

/**
* Audio_MuxUpdateReadPointer
* @param [in]  pHdlr mux control handler
* @return ErrorCode
*/
UINT32 Audio_MuxUpdateReadPointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr)
{
    /* give the Room Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RoomSem));
}

/**
* Audio_EncGetWritePointer
* @param [in]  pHdlr mux control handler
* @return AMBA_AENC_AUDIO_DESC_s
*/
AMBA_AENC_AUDIO_DESC_s *Audio_EncGetWritePointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr)
{
    UINT32 RtVal;
    AMBA_AENC_AUDIO_DESC_s *pDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RoomSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("Audio_EncGetWritePointer AmbaKAL_SemaphoreTake error ", 0U, 0U, 0U, 0U, 0U);
    }
    pDesc = &(pHdlr->Desc[pHdlr->WriteIdx]);

    /* Update the Read Pointer */
    if (++pHdlr->WriteIdx >= AU_MUX_DESC_NUM) {
        pHdlr->WriteIdx = 0;
    }

    return pDesc;
}

/**
* Audio_EncUpdateWritePointer
* @param [in]  pHdlr mux control handler
* @return ErrorCode
*/
UINT32 Audio_EncUpdateWritePointer(AMBA_AUDIO_MUX_CTRL_s *pHdlr)
{
    /* give the Remain Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RemainSem));
}

/**
* AmbaAudio_MuxWaitLof
* @param [in]  MuxIndex
* @return ErrorCode
*/
UINT32 AmbaAudio_MuxWaitLof(UINT32 MuxIndex)
{
    UINT32 ActualFlags = 0;
    if (AmbaKAL_EventFlagGet(&(AudioMuxHdlr[MuxIndex].EventFlag), AUDIO_MUX_FLG_GET_LOF, 0U/*or*/, 0U/*not clear*/,
                             &ActualFlags, (UINT32)AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
        ActualFlags &= AUDIO_MUX_FLG_GET_LOF;
    }
    return AmbaKAL_EventFlagClear(&(AudioMuxHdlr[MuxIndex].EventFlag), ActualFlags);
}

static void* AmbaAudio_MuxEntry(void* EntryArg)
{
    const AMBA_AENC_AUDIO_DESC_s    *pDesc;
    const UINT8                     *pAddr;
    AMBA_AUDIO_MUX_CTRL_s           *pHdlr;
    UINT32 CpSize, SizeA, SizeB;
    UINT32 FileOpRet;
    UINT32 RetVal;
    UINT32 RtVal;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pHdlr, &EntryArg);
    AmbaPrint_PrintStr5("Func: [%s]",__func__, NULL, NULL, NULL, NULL);
    for(;;) {
        pDesc  = Audio_MuxGetReadPointer(pHdlr);
        pAddr  = pDesc->pBufAddr;
        CpSize = pDesc->DataSize;
        if (pDesc->DataSize > AMBA_AUDIO_MUX_BUFFER_SIZE) {
            CpSize = AMBA_AUDIO_MUX_BUFFER_SIZE;
        }
        if ((CpSize + pHdlr->CurrentBufferSize) > AMBA_AUDIO_MUX_BUFFER_SIZE) {
            RtVal = AmbaFS_FileWrite(pHdlr->WriteBuffer, (UINT32)sizeof(INT8), pHdlr->CurrentBufferSize, pHdlr->pFile, &FileOpRet);
            if (OK != RtVal) {
                AmbaPrint_PrintUInt5("AmbaAudio_MuxEntry AmbaFS_FileWrite error ", 0U, 0U, 0U, 0U, 0U);
            }
            pHdlr->CurrentBufferSize = 0U;
        }
        if ((pHdlr->BsCurSize + CpSize) >= pHdlr->BsSize) {
            SizeA = pHdlr->BsSize - pHdlr->BsCurSize;
            if (OK != AmbaWrap_memcpy(&(pHdlr->WriteBuffer[pHdlr->CurrentBufferSize]), pAddr, SizeA)) {
                AmbaPrint_PrintStr5("Func: [%s] [1] AmbaWrap_memcpy WriteBuffer fail",__func__, NULL, NULL, NULL, NULL);
            }
            pHdlr->CurrentBufferSize += SizeA;
            SizeB = (pHdlr->BsCurSize + CpSize) - pHdlr->BsSize;
            if (SizeB > 0U) {
                if (OK != AmbaWrap_memcpy(&(pHdlr->WriteBuffer[pHdlr->CurrentBufferSize]), pHdlr->pBsBuf, SizeB)) {
                    AmbaPrint_PrintStr5("Func: [%s] [2] AmbaWrap_memcpy WriteBuffer fail",__func__, NULL, NULL, NULL, NULL);
                }
            }
            pHdlr->CurrentBufferSize += SizeB;
            pHdlr->BsCurSize = SizeB;
        } else {
            if (OK != AmbaWrap_memcpy(&(pHdlr->WriteBuffer[pHdlr->CurrentBufferSize]), pAddr, CpSize)) {
                AmbaPrint_PrintStr5("Func: [%s] [3] AmbaWrap_memcpy WriteBuffer fail",__func__, NULL, NULL, NULL, NULL);
            }
            pHdlr->CurrentBufferSize += CpSize;
            pHdlr->BsCurSize += CpSize;
        }
        RtVal = Audio_MuxUpdateReadPointer(pHdlr);
        if (OK != RtVal) {
            AmbaPrint_PrintUInt5("AmbaAudio_MuxEntry Audio_MuxUpdateReadPointer error ", 0U, 0U, 0U, 0U, 0U);
        }
        if (pDesc->Eos == 1U) {
            RtVal = AmbaFS_FileWrite(pHdlr->WriteBuffer, (UINT32)sizeof(INT8), pHdlr->CurrentBufferSize, pHdlr->pFile, &FileOpRet);
            if (OK != RtVal) {
                AmbaPrint_PrintUInt5("AmbaAudio_MuxEntry AmbaFS_FileWrite error ", 0U, 0U, 0U, 0U, 0U);
            }
            AmbaPrint_PrintUInt5("AU MUX last %d %d", pHdlr->CurrentBufferSize, pDesc->Eos, FileOpRet, 0U, 0U);
            RetVal = AmbaFS_FileClose(pHdlr->pFile);
            AmbaPrint_PrintUInt5("AU MUX close file! %d", RetVal, 0U, 0U, 0U, 0U);
            if (AmbaKAL_EventFlagSet(&(pHdlr->EventFlag), AUDIO_MUX_FLG_GET_LOF) == OK) {
                AmbaPrint_PrintUInt5("AU MUX set AUDIO_MUX_FLG_GET_LOF", 0U, 0U, 0U, 0U, 0U);
            }
            break;
        }
    }

    return NULL;
}

static UINT32 AmbaAudio_MuxTaskCreate(UINT32 MuxIndex, UINT32 Priority)
{
    AMBA_AUDIO_MUX_CTRL_s *pHdlr;
    static char TaskName[] = "Audio_Mux_Task";

    pHdlr = &AudioMuxHdlr[MuxIndex];
    /* Create a task */
    return AmbaKAL_TaskCreate(&(pHdlr->MuxTask), TaskName, Priority,
                              AmbaAudio_MuxEntry, pHdlr, pHdlr->MuxStack,
                              AMBA_AUDIO_MUX_STACK_SIZE, 0U/*Not auto start*/);
}

static UINT32 AmbaAudio_MuxTaskDelete(UINT32 MuxIndex)
{
    AMBA_AUDIO_MUX_CTRL_s *pHdlr;
    UINT32 RetVal = (UINT32)AMUX_OK;

    pHdlr = &AudioMuxHdlr[MuxIndex];

    /* Delete task */
    if (AmbaKAL_TaskTerminate(&(pHdlr->MuxTask)) != OK) {
        RetVal = (UINT32)AMUX_ERR_0000;
    }
    if (RetVal == AMUX_OK) {
        RetVal = AmbaKAL_TaskDelete(&(pHdlr->MuxTask));
    }
    return RetVal;
}

/**
* AmbaAudio_MuxCreate
* @param [in]  Priority
* @param [in]  pBsBuf Bitsbuf addr
* @param [in]  BsSize Bitsbuf size
* @return MuxIndex
*/
UINT32 AmbaAudio_MuxCreate(UINT32 Priority, UINT32 *pBsBuf, UINT32 BsSize)
{
    UINT32 MuxIndex;
    static char AmbaAudio_MuxEventFlagName[30]   = "AmbaAudio_MuxEventFlag";
    static char AmbaAudio_MuxSemaphoreName[30]   = "AmbaAudio_MuxSemaphore";

    MuxIndex = CheckAvailAudioMuxId();
    if (MuxIndex != AU_MUXER_NA_ID) {
        if (OK == AmbaKAL_EventFlagCreate(&(AudioMuxHdlr[MuxIndex].EventFlag), AmbaAudio_MuxEventFlagName)) {
            if (OK == AmbaKAL_EventFlagClear(&(AudioMuxHdlr[MuxIndex].EventFlag), 0xffffffffU)) {
                /* Create desc buffer semaphores */
                if (AmbaKAL_SemaphoreCreate(&(AudioMuxHdlr[MuxIndex].RemainSem), AmbaAudio_MuxSemaphoreName, 0) == OK) {
                    if (AmbaKAL_SemaphoreCreate(&(AudioMuxHdlr[MuxIndex].RoomSem), AmbaAudio_MuxSemaphoreName, AU_MUX_DESC_NUM) == OK) {
                        /* Initialize wp/rp index and bits buffer */
                        AudioMuxHdlr[MuxIndex].WriteIdx   = 0U;
                        AudioMuxHdlr[MuxIndex].ReadIdx    = 0U;
                        AudioMuxHdlr[MuxIndex].pBsBuf     = pBsBuf;
                        AudioMuxHdlr[MuxIndex].BsSize     = BsSize;
                        AudioMuxHdlr[MuxIndex].BsCurSize  = 0U;
                        if (AmbaAudio_MuxTaskCreate(MuxIndex, Priority) != OK) {
                            MuxIndex = AU_MUXER_NA_ID;
                        }
                    } else {
                        MuxIndex = AU_MUXER_NA_ID;
                    }
                } else {
                    MuxIndex = AU_MUXER_NA_ID;
                }
            }
        } else {
            MuxIndex = AU_MUXER_NA_ID;
        }
    }
    return MuxIndex;
}

/**
* AmbaAudio_MuxDelete
* @param [in]  MuxIndex
* @return ErrorCode
*/
UINT32 AmbaAudio_MuxDelete(UINT32 MuxIndex)
{
    UINT32 RetVal = (UINT32)AMUX_OK;

    if (AudioMuxHdlr[MuxIndex].MuxIndex != AU_MUXER_NA_ID) {
        if (AmbaAudio_MuxTaskDelete(MuxIndex) == OK) {
            if (OK == AmbaKAL_EventFlagDelete(&(AudioMuxHdlr[MuxIndex].EventFlag))) {
                if (AmbaKAL_SemaphoreDelete(&(AudioMuxHdlr[MuxIndex].RemainSem)) == OK) {
                    if (AmbaKAL_SemaphoreDelete(&(AudioMuxHdlr[MuxIndex].RoomSem)) != OK) {
                        RetVal = AMUX_ERR_0000;
                    }
                } else {
                    RetVal = AMUX_ERR_0000;
                }
            } else {
                RetVal = (UINT32)AMUX_ERR_0000;
            }
        } else {
            RetVal = AMUX_ERR_0000;
        }
        DeleteAudioMuxId(MuxIndex);
    }
    return RetVal;
}

/**
* AmbaAudio_MuxTaskSetUp
* @param [in]  MuxIndex
* @param [in]  pName Name
* @return ErrorCode
*/
UINT32 AmbaAudio_MuxTaskSetUp(UINT32 MuxIndex, const char *pName)
{
    AMBA_AUDIO_MUX_CTRL_s *pHdlr;
    UINT32 RetVal = (UINT32)AMUX_OK;

    if (AudioMuxHdlr[MuxIndex].MuxIndex != AU_MUXER_NA_ID) {
        UINT32 RtVal;
        pHdlr = &AudioMuxHdlr[MuxIndex];
        pHdlr->CurrentBufferSize = 0;
        if (OK != AmbaWrap_memset(pHdlr->WriteBuffer, 0, AMBA_AUDIO_MUX_BUFFER_SIZE)) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset WriteBuffer fail",__func__, NULL, NULL, NULL, NULL);
        }
        RtVal = AmbaFS_FileOpen(pName, "wb", &pHdlr->pFile);
        if (OK != RtVal) {
            AmbaPrint_PrintUInt5("AmbaAudio_MuxTaskSetUp AmbaFS_FileOpen error ", 0U, 0U, 0U, 0U, 0U);
        }
        if (pHdlr->pFile == NULL) {
            RetVal = (UINT32)AMUX_ERR_0000;
            AmbaPrint_PrintUInt5("AmbaAudio_MuxTaskSetUp 0: %d", RetVal, 0U, 0U, 0U, 0U);
        }
    } else {
        RetVal = (UINT32)AMUX_ERR_0000;
        AmbaPrint_PrintUInt5("AmbaAudio_MuxTaskSetUp 1: %d", RetVal, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

/**
* AmbaAudio_MuxTaskStart
* @param [in]  MuxIndex
* @return ErrorCode
*/
UINT32 AmbaAudio_MuxTaskStart(UINT32 MuxIndex)
{
    AMBA_AUDIO_MUX_CTRL_s *pHdlr;
    UINT32 RetVal = (UINT32)AMUX_OK;

    if (AudioMuxHdlr[MuxIndex].MuxIndex != AU_MUXER_NA_ID) {
        pHdlr = &AudioMuxHdlr[MuxIndex];
        RetVal = AmbaKAL_TaskResume(&(pHdlr->MuxTask));
    }
    return RetVal;
}

/**
* AmbaAudio_BsBufDemuxGetWrPtr
* @param [in]  pHdlr demux handler
* @return AMBA_ADEC_AUDIO_DESC_s
*/
AMBA_ADEC_AUDIO_DESC_s *AmbaAudio_BsBufDemuxGetWrPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr)
{
    UINT32 RtVal;
    AMBA_ADEC_AUDIO_DESC_s *pDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RoomSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_BsBufDemuxGetWrPtr AmbaKAL_SemaphoreTake error ", 0U, 0U, 0U, 0U, 0U);
    }
    pDesc = &(pHdlr->Desc[pHdlr->WriteIdx]);

    /* Update the Write Pointer */
    if (++pHdlr->WriteIdx >= AU_DEMUX_DESC_NUM) {
        pHdlr->WriteIdx = 0;
    }

    return pDesc;
}

/**
* AmbaAudio_BsBufDemuxUptWrPtr
* @param [in]  pHdlr demux handler
* @return ErrorCode
*/
UINT32 AmbaAudio_BsBufDemuxUptWrPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr)
{
    /* give the Remain Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RemainSem));
}

/**
* AmbaAudio_GetDecBsBufSize
* @return bits buf size
*/
UINT32 AmbaAudio_GetDecBsBufSize(void)
{
    UINT32 RSize = AMBA_AUDIO_DEMUX_BS_BUFFER_SIZE;
    return RSize;
}

/**
* AmbaAudio_BsBufDecGetRdPtr
* @param [in]  pHdlr demux handler
* @return AMBA_ADEC_AUDIO_DESC_s
*/
AMBA_ADEC_AUDIO_DESC_s *AmbaAudio_BsBufDecGetRdPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr)
{
    UINT32 RtVal;
    AMBA_ADEC_AUDIO_DESC_s *pDesc;

    /* take the Remain Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RemainSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_BsBufDecGetRdPtr AmbaKAL_SemaphoreTake error ", 0U, 0U, 0U, 0U, 0U);
    }
    pDesc = &(pHdlr->Desc[pHdlr->ReadIdx]);
    /* Update the Read Pointer */
    if (++pHdlr->ReadIdx >= AU_DEMUX_DESC_NUM) {
        pHdlr->ReadIdx = 0;
    }
    return pDesc;
}

/**
* AmbaAudio_BsBufDecUptRdPtr
* @param [in]  pHdlr demux handler
* @return ErrorCode
*/
UINT32 AmbaAudio_BsBufDecUptRdPtr(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr)
{
    /* give the Room Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RoomSem));
}

/**
* AmbaAudio_FeedAudio
* @param [in]  pHdlr demux handler
*/
void AmbaAudio_FeedAudio(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr)
{
    UINT32 RtVal;
    AMBA_ADEC_AUDIO_DESC_s *pDesc;
    UINT8         *pAddr;
    UINT32 FileOpRet = (UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE;

    pDesc = AmbaAudio_BsBufDemuxGetWrPtr(pHdlr);
    pAddr = pDesc->pBufAddr;

    RtVal = AmbaFS_FileRead((UINT8 *)pAddr, (UINT32)sizeof(UINT8), AMBA_AUDIO_DEMUX_BUFFER_SIZE, pAFile, &FileOpRet);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_FeedAudio AmbaFS_FileRead error ", 0U, 0U, 0U, 0U, 0U);
    }
    pHdlr->BsCurSize += FileOpRet;

    RtVal = AmbaAudio_BsBufDemuxUptWrPtr(pHdlr);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_FeedAudio AmbaAudio_BsBufDemuxUptWrPtr error ", 0U, 0U, 0U, 0U, 0U);
    }

    RtVal = AmbaADEC_UpdateBsBufWp(pHdlr->pDecHdlr, pAddr, FileOpRet);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_FeedAudio AmbaADEC_UpdateBsBufWp error ", 0U, 0U, 0U, 0U, 0U);
    }

    if (FileOpRet != (UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE) {
        pDesc->Eos = 1U;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintStr5("[%s]File end!!!!", __func__, NULL, NULL, NULL, NULL);
#endif
    }
}

/**
* AmbaAudio_FeedROMAudio
* @param [in]  pHdlr demux handler
* @param [in]  NvmID
*/
void AmbaAudio_FeedROMAudio(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr, UINT32 NvmID)
{
    AMBA_ADEC_AUDIO_DESC_s *pDesc;
    UINT8         *pAddr;
    UINT32        ReadStartPos, ReadSize, Rval;
    UINT32        RtVal;

    pDesc = AmbaAudio_BsBufDemuxGetWrPtr(pHdlr);
    pAddr = pDesc->pBufAddr;

    /* Evaluate ReadSize from ROM */
    if (((pHdlr->AudFromROMCtrl.AudROMRdCnt + 1U) * AMBA_AUDIO_DEMUX_BUFFER_SIZE) <= pHdlr->AudFromROMCtrl.AudROMSize) {
        /* Audio data remaining size > 8192 inside ROM */
        ReadSize = AMBA_AUDIO_DEMUX_BUFFER_SIZE;
    } else if ((((pHdlr->AudFromROMCtrl.AudROMRdCnt) * AMBA_AUDIO_DEMUX_BUFFER_SIZE) < pHdlr->AudFromROMCtrl.AudROMSize) &&
               (((pHdlr->AudFromROMCtrl.AudROMRdCnt + 1U) * AMBA_AUDIO_DEMUX_BUFFER_SIZE) > pHdlr->AudFromROMCtrl.AudROMSize)){
        /* Audio data remaining size is 0 ~ 8192 inside ROM */
        ReadSize = pHdlr->AudFromROMCtrl.AudROMSize - (pHdlr->AudFromROMCtrl.AudROMRdCnt * AMBA_AUDIO_DEMUX_BUFFER_SIZE);
    } else {
        /* Audio data remaining size =0 inside ROM */
        ReadSize = 0U;
    }
    if ((1U == pHdlr->AudFromROMCtrl.IsAudROMReload) && (ReadSize > 0U)) {
        ReadStartPos = pHdlr->AudFromROMCtrl.AudROMRdCnt * AMBA_AUDIO_DEMUX_BUFFER_SIZE;

        /* Load Data from ROM */
        Rval = AmbaNVM_ReadRomFile(NvmID,
                            AMBA_NVM_ROM_REGION_SYS_DATA,
                            pHdlr->AudFromROMCtrl.AudRomFileName,
                            ReadStartPos,
                            ReadSize,
                            (UINT8 *)pAddr,
                            1000U);
        if (OK!= Rval) {
            AmbaPrint_PrintUInt5("[AmbaAudio_FeedROMAudio] AmbaSvcWrap_GetNVMID error", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        /*
         * No need to load Data from ROM for two cases:
         * 1. It already exist in BS buffer
         * 2. ReadSize == 0U
        */
    }
    /* Accumulate AudROMRdCnt in order to evaluate ReadSize */
    pHdlr->AudFromROMCtrl.AudROMRdCnt++;

    pHdlr->BsCurSize += ReadSize;
    RtVal = AmbaAudio_BsBufDemuxUptWrPtr(pHdlr);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_FeedROMAudio AmbaAudio_BsBufDemuxUptWrPtr error ", 0U, 0U, 0U, 0U, 0U);
    }

    RtVal = AmbaADEC_UpdateBsBufWp(pHdlr->pDecHdlr, pAddr, ReadSize);
    if (OK != RtVal) {
        AmbaPrint_PrintUInt5("AmbaAudio_FeedROMAudio AmbaADEC_UpdateBsBufWp error ", 0U, 0U, 0U, 0U, 0U);
    }

    if (ReadSize != (UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE) {
        pDesc->Eos = 1U;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintStr5("[%s]File end!!!!", __func__, NULL, NULL, NULL, NULL);
#endif
    }
}

/**
* AmbaAudio_BsBufDecInit
* @param [in]  pAdecHdlr Audio decode handler
* @param [in]  pBsBuf BitsBuf addr
* @param [in]  BsSize BitsBuf size
* @param [in]  DemuxIndex Demux Index
* @param [in]  Source
* @return AMBA_AUDIO_DEMUX_CTRL_s
*/
AMBA_AUDIO_DEMUX_CTRL_s *AmbaAudio_BsBufDecInit(UINT32 *pAdecHdlr, UINT8 *pBsBuf, UINT32 BsSize, UINT32 DemuxIndex, UINT32 Source)
{
    UINT32 LoopU, RtVal;
    static UINT32 BsBufFirstInit = 0U;
    AMBA_ADEC_AUDIO_DESC_s  *pDesc;
    static char AmbaAudio_DemuxSemaphoreName[30]   = "AmbaAudio_DemuxSemaphore";

    if (0U == BsBufFirstInit) {
        if (OK != AmbaWrap_memset(&AudioDemuxHdlr, 0, (sizeof(AMBA_AUDIO_DEMUX_CTRL_s) * AMBA_AUDIO_MAX_DEMUX_NUM))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset AudioDemuxHdlr fail",__func__, NULL, NULL, NULL, NULL);
        }
        BsBufFirstInit = 1;
    }

    /* Create desc buffer semaphores */
    RtVal = AmbaKAL_SemaphoreCreate(&(AudioDemuxHdlr[DemuxIndex].RemainSem), AmbaAudio_DemuxSemaphoreName, 0);
    if (KAL_ERR_NONE == RtVal) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("RemainSem Semaphore Create success, RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
#endif
    } else {
        AmbaPrint_PrintUInt5("RemainSem Semaphore Create error!! RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
    }
    RtVal = AmbaKAL_SemaphoreCreate(&(AudioDemuxHdlr[DemuxIndex].RoomSem), AmbaAudio_DemuxSemaphoreName, AU_DEMUX_DESC_NUM);
    if (KAL_ERR_NONE == RtVal) {
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("RoomSem Semaphore Create success, RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
#endif
    } else {
        AmbaPrint_PrintUInt5("RoomSem Semaphore Create error!! RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
    }
    /* Initialize wp/rp index and bits buffer */
    AudioDemuxHdlr[DemuxIndex].pDecHdlr   = pAdecHdlr;
    AudioDemuxHdlr[DemuxIndex].WriteIdx   = 0U;
    AudioDemuxHdlr[DemuxIndex].ReadIdx    = 0U;
    AudioDemuxHdlr[DemuxIndex].pBsBuf     = pBsBuf;
    AudioDemuxHdlr[DemuxIndex].BsSize     = BsSize;
    AudioDemuxHdlr[DemuxIndex].BsCurSize  = 0U;
    for (LoopU = 0; LoopU < AU_DEMUX_DESC_NUM; LoopU++) {
        pDesc = &(AudioDemuxHdlr[DemuxIndex].Desc[LoopU]);
        pDesc->Eos      = 0U;
        pDesc->DataSize = 0U;
        pDesc->pBufAddr = &(pBsBuf[(UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE * LoopU]);
    }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    /* Initialize Audio from ROM Part */
    /* Audio source from ROM */
    if (AMBA_AUDIO_SOURCE_ROM_AUD == Source) {
        /* Audio from ROM size <= BS total buffer size */
        if ((pAudFromRom->AudROMSize <= AMBA_AUDIO_DEMUX_BS_BUFFER_SIZE)) {
            /* File name same as previous one which already load into BS buffer */
            if ((0 == AmbaUtility_StringCompare(AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.AudRomFileName, pAudFromRom->AudRomFileName, AmbaUtility_StringLength(pAudFromRom->AudRomFileName)))) {
                /* No need to reload BS buffer */
                AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.IsAudROMReload = 0U;
            } else {
                /* Need to reload BS buffer */
                AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.IsAudROMReload = 1U;
            }
        } else {
            /* Need to reload BS buffer */
            AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.IsAudROMReload = 1U;
        }
        /* Update the member of AudFromROMCtrl */
        AmbaUtility_StringCopy(AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.AudRomFileName, sizeof(AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.AudRomFileName), pAudFromRom->AudRomFileName);
        AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.AudROMSize = pAudFromRom->AudROMSize;
        AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.AudROMRdCnt= 0U;

        AmbaPrint_PrintUInt5("IsAudROMReload: %d, AudROMSize = %d", AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl.IsAudROMReload, pAudFromRom->AudROMSize, 0U, 0U, 0U);
    } else {
        /* Reset AudFromROMCtrl structure because audio source doesn't come from ROM */
        if (OK != AmbaWrap_memset(&AudioDemuxHdlr[DemuxIndex].AudFromROMCtrl, 0, sizeof(AMBA_AUD_FROM_ROM_s))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset AudioDemuxHdlr fail",__func__, NULL, NULL, NULL, NULL);
        }
    }
#endif
    return (&AudioDemuxHdlr[DemuxIndex]);
}

/**
* AmbaAudio_DeMuxCreate
* @param [in]  Source source
* @param [in]  MPly source info
* @return ErrorCode
*/
UINT32 AmbaAudio_DeMuxCreate(UINT32 Source, ULONG MPly)
{
    UINT32 RetVal = (UINT32)ADEMUX_OK;

    if (AMBA_AUDIO_SOURCE_MP4 == Source) {
        // struct { struct { UINT32 Uint32; SVC_PLAYBACK_TASK_s * pMovie; } d; } Uint32ToMovieFile;
        // Uint32ToMovieFile.d.Uint32 = MPly;
        // pMPly = Uint32ToMovieFile.d.pMovie;
    } else if (AMBA_AUDIO_SOURCE_AUDIO == Source) {
//        struct { struct { UINT32 Uint32; AMBA_FS_FILE *pFile; } d; } Uint32ToAudioFile;
//        (void)AmbaWrap_memset(&Uint32ToAudioFile, 0, sizeof(Uint32ToAudioFile));
//        Uint32ToAudioFile.d.Uint32 = MPly;
//        pAFile = Uint32ToAudioFile.d.pFile;
        AmbaMisra_TypeCast(&pAFile, &(MPly));
    } else if (AMBA_AUDIO_SOURCE_ROM_AUD == Source) {
//        struct { struct { UINT32 Uint32; AMBA_AUD_FROM_ROM_s * pAudRomFile; } d; } Uint32ToAudRomFile;
//        (void)AmbaWrap_memset(&Uint32ToAudRomFile, 0, sizeof(Uint32ToAudRomFile));
//        Uint32ToAudRomFile.d.Uint32 = MPly;
//        pAudFromRom = Uint32ToAudRomFile.d.pAudRomFile;
        AmbaMisra_TypeCast(&pAudFromRom, &(MPly));
    } else {
        //Do nothing
        RetVal = ADEMUX_ERR_0000;
    }
    return RetVal;
}

static UINT32 AmbaAudio_DeMuxTaskDelete(UINT32 DemuxIndex)
{
    AMBA_AUDIO_DEMUX_CTRL_s *pHdlr;
    UINT32 RetVal;

    pHdlr = &AudioDemuxHdlr[DemuxIndex];

    RetVal = AmbaKAL_SemaphoreDelete(&(pHdlr->RoomSem));
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_PrintUInt5("AmbaKAL_SemaphoreDelete-RoomSem, RetVal: %d", RetVal, 0U, 0U, 0U, 0U);
#endif
    RetVal = AmbaKAL_SemaphoreDelete(&(pHdlr->RemainSem));
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_PrintUInt5("AmbaKAL_SemaphoreDelete-RemainSem, RetVal: %d", RetVal, 0U, 0U, 0U, 0U);
#endif
    return RetVal;
}

/**
* AmbaAudio_DeMuxDelete
* @param [in]  DemuxIndex Demux index
* @return ErrorCode
*/
UINT32 AmbaAudio_DeMuxDelete(UINT32 DemuxIndex)
{
    UINT32 RetVal = (UINT32)ADEMUX_OK;

    if (AmbaAudio_DeMuxTaskDelete(DemuxIndex) != OK) {
        RetVal = (UINT32)ADEMUX_ERR_0000;
    }
    return RetVal;
}

