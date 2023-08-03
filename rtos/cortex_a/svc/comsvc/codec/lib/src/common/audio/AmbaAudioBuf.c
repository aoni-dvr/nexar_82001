/**
*  @file AmbaAudioBuf.c
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
*  @details audio buffers related API
*
*/

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudioEfx.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudioBuf.h"
#include "AmbaCodecCom.h"


AMBA_AUDIO_ENC_BUF_CTRL_s AudioEncBufHdlr[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
AMBA_AUDIO_DEC_BUF_CTRL_s AudioDecBufHdlr[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;

void AmbaAudio_EncBufIdHdrInit(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        if (OK != AmbaWrap_memset(&AudioEncBufHdlr[i], 0, sizeof(AMBA_AUDIO_ENC_BUF_CTRL_s))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset AudioEncBufHdlr fail",__func__, NULL, NULL, NULL, NULL);
        }
        AudioEncBufHdlr[i].BufIndex = AU_BUF_NA_ID;
        if (OK != AmbaWrap_memset(&AudioEncBufHdlr[i].Desc[0], 0, sizeof(AMBA_AENC_DATA_INFO_s) * AU_SSP_BUF_DESC_NUM)) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset AudioEncBufHdlr.Desc fail",__func__, NULL, NULL, NULL, NULL);
        }
        if (OK != AmbaWrap_memset(AudioEncBufHdlr[i].Buf, 0, sizeof(AudioEncBufHdlr[i].Buf))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memset AudioEncBufHdlr.Buf fail",__func__, NULL, NULL, NULL, NULL);
        }
    }
}

static UINT32 CheckAvailAudioBufId(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        if (AudioEncBufHdlr[i].BufIndex == AU_BUF_NA_ID) {
            AudioEncBufHdlr[i].BufIndex = i;
            break;
        }
    }
    if (i == AMBA_AUDIO_MAX_BUF_NUM) {
        i = AU_BUF_NA_ID;
    }
    return i;
}

static void DeleteAudioBufId(UINT32 Id)
{
    AudioEncBufHdlr[Id].BufIndex = AU_BUF_NA_ID;
}

/**
* AmbaAudio_EncBufCreate
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufCreate(void)
{
    UINT32 BufIndex, LoopU , RtVal = OK;
    static char AmbaAudio_BufSemaphoreName[30]   = "AmbaAudio_MuxSemaphore";

    BufIndex = CheckAvailAudioBufId();
    if (BufIndex != AU_BUF_NA_ID) {
        /* Create desc buffer semaphores */
        if (AmbaKAL_SemaphoreCreate(&(AudioEncBufHdlr[BufIndex].RemainSem), AmbaAudio_BufSemaphoreName, 0) == OK) {
            if (AmbaKAL_SemaphoreCreate(&(AudioEncBufHdlr[BufIndex].RoomSem), AmbaAudio_BufSemaphoreName, AU_SSP_BUF_DESC_NUM) == OK) {
                /* Initialize wp/rp index and bits buffer */
                AudioEncBufHdlr[BufIndex].WriteIdx   = 0U;
                AudioEncBufHdlr[BufIndex].ReadIdx    = 0U;
                for (LoopU = 0; LoopU < AU_SSP_BUF_DESC_NUM; LoopU++) {
                    AudioEncBufHdlr[BufIndex].Desc[LoopU].AudioTicks = 0;
                    AudioEncBufHdlr[BufIndex].Desc[LoopU].Lof = 0;
                    AudioEncBufHdlr[BufIndex].Desc[LoopU].Size = 0;
                    AudioEncBufHdlr[BufIndex].Desc[LoopU].pPcmBuf = AudioEncBufHdlr[BufIndex].Buf[LoopU];
                }

            } else {
                RtVal = ERR_ARG;
            }
        } else {
            RtVal = ERR_ARG;
        }
    }

    return RtVal;
}

/**
* AmbaAudio_EncBufReset
* @param [in]  BufIndex buffer index
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufReset(UINT32 BufIndex)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    UINT32 RetVal = ABUF_OK;
    static char AmbaAudio_BufSemaphoreName[30]   = "AmbaAudio_MuxSemaphore";

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        /* Delete desc buffer semaphores */
        if (AmbaKAL_SemaphoreDelete(&(pHdlr->RemainSem)) == OK) {
            if (AmbaKAL_SemaphoreDelete(&(pHdlr->RoomSem)) == OK) {
                if (AmbaKAL_SemaphoreCreate(&(AudioEncBufHdlr[BufIndex].RemainSem), AmbaAudio_BufSemaphoreName, 0) == OK) {
                    if (AmbaKAL_SemaphoreCreate(&(AudioEncBufHdlr[BufIndex].RoomSem), AmbaAudio_BufSemaphoreName, AU_SSP_BUF_DESC_NUM) == OK) {
                        /* Initialize wp/rp index and bits buffer */
                        AudioEncBufHdlr[BufIndex].WriteIdx   = 0U;
                        AudioEncBufHdlr[BufIndex].ReadIdx    = 0U;
                    } else {
                        RetVal = ABUF_ERR_0000;
                    }
                } else {
                    RetVal = ABUF_ERR_0000;
                }

            } else {
                RetVal = ABUF_ERR_0000;
            }

        } else {
            RetVal = ABUF_ERR_0000;
        }
    } else {
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* AmbaAudio_EncBufDelete
* @param [in]  BufIndex BufIndex
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufDelete(UINT32 BufIndex)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    UINT32 RetVal = ABUF_OK;

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        /* Delete desc buffer semaphores */
        if (AmbaKAL_SemaphoreDelete(&(pHdlr->RemainSem)) == OK) {
            if (AmbaKAL_SemaphoreDelete(&(pHdlr->RoomSem)) != OK) {
                RetVal = ABUF_ERR_0000;
            }
        } else {
            RetVal = ABUF_ERR_0000;
        }
        DeleteAudioBufId(BufIndex);
    }

    return RetVal;
}

/**
* AmbaAudio_EncBufGetReadPointer
* @param [in]  pHdlr buffer control handler
* @return AMBA_AENC_DATA_INFO_s
*/
AMBA_AENC_DATA_INFO_s *AmbaAudio_EncBufGetReadPointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr)
{
    UINT32                RtVal;
    AMBA_AENC_DATA_INFO_s *pDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RemainSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintStr5("Func: [%s] AmbaKAL_SemaphoreTake fail",__func__, NULL, NULL, NULL, NULL);
    }
    pDesc = &(pHdlr->Desc[pHdlr->ReadIdx]);

    /* Update the Read Pointer */
    if (++pHdlr->ReadIdx >= AU_SSP_BUF_DESC_NUM) {
        pHdlr->ReadIdx = 0;
    }

    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);

    return pDesc;
}

/**
* AmbaAudio_EncBufUpdateReadPointer
* @param [in]  pHdlr buffer control handler
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufUpdateReadPointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr)
{
    /* give the Room Semaphore */
    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);
    return AmbaKAL_SemaphoreGive(&(pHdlr->RoomSem));
}

/**
* AmbaAudio_EncBufGetWritePointer
* @param [in]  pHdlr buffer control handler
* @return AMBA_AIN_DATA_INFO_s
*/
AMBA_AIN_DATA_INFO_s *AmbaAudio_EncBufGetWritePointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr)
{
    UINT32                RtVal;
    AMBA_AIN_DATA_INFO_s *pDesc;
    const AMBA_AENC_DATA_INFO_s *pEncDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RoomSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintStr5("Func: [%s] AmbaKAL_SemaphoreTake fail",__func__, NULL, NULL, NULL, NULL);
    }
    pEncDesc = &(pHdlr->Desc[pHdlr->WriteIdx]);
    AmbaMisra_TypeCast(&pDesc, &pEncDesc); /*pDesc = (AMBA_AIN_DATA_INFO_s *)pEncDesc;*/

    /* Update the Write Pointer */
    if (++pHdlr->WriteIdx >= AU_SSP_BUF_DESC_NUM) {
        pHdlr->WriteIdx = 0U;
    }

    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);
    return pDesc;
}

/**
* AmbaAudio_EncBufUpdateWritePointer
* @param [in]  pHdlr buffer control handler
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufUpdateWritePointer(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr)
{
    /* give the Remain Semaphore */
    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);
    return AmbaKAL_SemaphoreGive(&(pHdlr->RemainSem));
}

/**
* AmbaAudio_EncBufPeekRoomSize
* @param [in]  pHdlr buffer control handler
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufPeekRoomSize(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSize)
{
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    UINT32 RetVal = ABUF_OK;

    if (AmbaKAL_SemaphoreQuery(&(pHdlr->RoomSem), &SemaphoreInfo) == OK) {
        *pSize = AU_SSP_BUF_CHUNK_SIZE * SemaphoreInfo.CurrentCount;
    } else {
        *pSize = 0U;
        RetVal = ABUF_ERR_0000;
    }

    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("Room Size: %d", *pSize, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* AmbaAudio_EncBufPeekRemainSize
* @param [in]  pHdlr buf control handler
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaAudio_EncBufPeekRemainSize(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSize)
{
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    UINT32 RetVal = ABUF_OK;

    if (AmbaKAL_SemaphoreQuery(&(pHdlr->RemainSem), &SemaphoreInfo) == OK) {
        *pSize = AU_SSP_BUF_CHUNK_SIZE * SemaphoreInfo.CurrentCount;;
    } else {
        *pSize = 0U;
        RetVal = ABUF_ERR_0000;
    }

    //AmbaPrint_PrintStr5("%s", __func__, 0, 0, 0, 0);
    //AmbaPrint_PrintUInt5("Remain Size: %d", *pSize, 0U, 0U, 0U, 0U);
    return RetVal;
}

/**
* AmbaAudio_DecBufIdHdrInit
*/
void AmbaAudio_DecBufIdHdrInit(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        AudioDecBufHdlr[i].BufIndex = AU_BUF_NA_ID;
    }
}

static UINT32 AmbaAdecCheckAvailAudioBufId(void)
{
    UINT32 i;
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        if (AudioDecBufHdlr[i].BufIndex == AU_BUF_NA_ID) {
            AudioDecBufHdlr[i].BufIndex = i;
            break;
        }
    }
    if (i == AMBA_AUDIO_MAX_BUF_NUM) {
        i = AU_BUF_NA_ID;
    }
    return i;
}

static void AmbaAdecDeleteAudioBufId(UINT32 Id)
{
    AudioDecBufHdlr[Id].BufIndex = AU_BUF_NA_ID;
}

/**
* AmbaAudio_DecBufCreate
* @return BufIndex
*/
UINT32 AmbaAudio_DecBufCreate(void)
{
    UINT32 BufIndex, LoopU;
    static char AmbaAudio_UtBufSemaphoreName[30]   = "AmbaAudio_UtIOBufSemaphore";

    BufIndex = AmbaAdecCheckAvailAudioBufId();
    if (BufIndex != AU_BUF_NA_ID) {
        /* Create desc buffer semaphores */
        if (AmbaKAL_SemaphoreCreate(&(AudioDecBufHdlr[BufIndex].RemainSem), AmbaAudio_UtBufSemaphoreName, 0) == OK) {
            if (AmbaKAL_SemaphoreCreate(&(AudioDecBufHdlr[BufIndex].RoomSem), AmbaAudio_UtBufSemaphoreName, AU_SSP_BUF_DESC_NUM) == OK) {
                /* Initialize wp/rp index and bits buffer */
                AudioDecBufHdlr[BufIndex].WriteIdx   = 0U;
                AudioDecBufHdlr[BufIndex].ReadIdx    = 0U;
                for (LoopU = 0; LoopU < AU_SSP_BUF_DESC_NUM; LoopU++) {
                    AudioDecBufHdlr[BufIndex].Desc[LoopU].Lof = 0;
                    AudioDecBufHdlr[BufIndex].Desc[LoopU].Size = 0;
                    AudioDecBufHdlr[BufIndex].Desc[LoopU].pDataBuf = AudioDecBufHdlr[BufIndex].Buf[LoopU];
                }

            } else {
                BufIndex = AU_BUF_NA_ID;
            }
        } else {
            BufIndex = AU_BUF_NA_ID;
        }
    }

    return BufIndex;
}

/**
* AmbaAudio_DecBufDelete
* @param [in]  BufIndex BufIndex
* @return ErrorCode
*/
UINT32 AmbaAudio_DecBufDelete(UINT32 BufIndex)
{
    AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr;
    UINT32 RetVal = (UINT32)ABUF_OK;

    if (AudioDecBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioDecBufHdlr[BufIndex];
        /* Delete desc buffer semaphores */
        if (AmbaKAL_SemaphoreDelete(&(pHdlr->RemainSem)) == OK) {
            if (AmbaKAL_SemaphoreDelete(&(pHdlr->RoomSem)) != OK) {
                RetVal = (UINT32)ABUF_ERR_0000;
            }
        } else {
            RetVal = (UINT32)ABUF_ERR_0000;
        }
        AmbaAdecDeleteAudioBufId(BufIndex);
    }

    return RetVal;
}

/**
* AmbaAudio_DecBufGetReadPointer
* @param [in]  pHdlr buf control handler
* @return AMBA_AOUT_DATA_INFO_s
*/
AMBA_AOUT_DATA_INFO_s *AmbaAudio_DecBufGetReadPointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr)
{
    UINT32                RtVal;
    AMBA_AOUT_DATA_INFO_s *pDesc;

    /* take the Room Semaphore */
    RtVal = AmbaKAL_SemaphoreTake(&(pHdlr->RemainSem), AMBA_KAL_WAIT_FOREVER);
    if (OK != RtVal) {
        AmbaPrint_PrintStr5("Func: [%s] AmbaKAL_SemaphoreTake fail",__func__, NULL, NULL, NULL, NULL);
    }
    pDesc = &(pHdlr->Desc[pHdlr->ReadIdx]);

    /* Update the Read Pointer */
    if (++pHdlr->ReadIdx >= AU_SSP_BUF_DESC_NUM) {
        pHdlr->ReadIdx = 0;
    }

    return pDesc;
}

/**
* AmbaAudio_DecBufUpdateReadPointer
* @param [in]  pHdlr buf control handler
* @return ErrorCode
*/
UINT32 AmbaAudio_DecBufUpdateReadPointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr)
{
    /* give the Room Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RoomSem));
}

/**
* AmbaAudio_DecBufGetWritePointer
* @param [in]  pHdlr buf control handler
* @return AMBA_ADEC_DATA_INFO_s
*/
AMBA_ADEC_DATA_INFO_s *AmbaAudio_DecBufGetWritePointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr)
{
    AMBA_ADEC_DATA_INFO_s *pDesc;
    UINT32 RetVal;
    const AMBA_AOUT_DATA_INFO_s *TempAddr;

    /* take the Room Semaphore */
    RetVal = AmbaKAL_SemaphoreTake(&(pHdlr->RoomSem), AMBA_KAL_WAIT_FOREVER);
    if (RetVal == KAL_ERR_NONE) {
        TempAddr = &(pHdlr->Desc[pHdlr->WriteIdx]);
        AmbaMisra_TypeCast(&pDesc, &(TempAddr));
        //pDesc = (AMBA_ADEC_DATA_INFO_s *)&(pHdlr->Desc[pHdlr->WriteIdx]);

        /* Update the Write Pointer */
        if (++pHdlr->WriteIdx >= AU_SSP_BUF_DESC_NUM) {
            pHdlr->WriteIdx = 0;
        }
    } else {
        //AmbaPrint_PrintStr5("  AmbaAudio_UtBufGetWritePointer Timeout!!!!!!",NULL, NULL, NULL, NULL, NULL);
        pDesc = NULL;
    }

    return pDesc;
}

/**
* AmbaAudio_DecBufUpdateWritePointer
* @param [in]  pHdlr buf control handler
* @return ErrorCode
*/
UINT32 AmbaAudio_DecBufUpdateWritePointer(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr)
{
    /* give the Remain Semaphore */
    return AmbaKAL_SemaphoreGive(&(pHdlr->RemainSem));
}

/**
* AmbaAudio_DecBufPeekRoomSize
* @param [in]  pHdlr buf control handler
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaAudio_DecBufPeekRoomSize(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr, UINT32 *pSize)
{
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    UINT32 RetVal = (UINT32)ABUF_OK;

    if (AmbaKAL_SemaphoreQuery(&(pHdlr->RoomSem), &SemaphoreInfo) == OK) {
        *pSize = AU_SSP_BUF_CHUNK_SIZE * SemaphoreInfo.CurrentCount;
    } else {
        *pSize = 0;
        RetVal = (UINT32)ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* AmbaAudio_DecBufPeekRemainSize
* @param [in]  pHdlr buf control handler
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaAudio_DecBufPeekRemainSize(AMBA_AUDIO_DEC_BUF_CTRL_s *pHdlr, UINT32 *pSize)
{
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    UINT32 RetVal = (UINT32)ABUF_OK;

    if (AmbaKAL_SemaphoreQuery(&(pHdlr->RemainSem), &SemaphoreInfo) == OK) {
        *pSize = AU_SSP_BUF_CHUNK_SIZE * SemaphoreInfo.CurrentCount;;
    } else {
        *pSize = 0;
        RetVal = (UINT32)ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* Buffer Efx-In Process
* @param [in]  pHdlr buf control handler
* @param [in]  pSrc
* @param [in]  pDst
* @param [in]  FrameSize
* @return ErrorCode
*/
UINT32 AmbaAudio_BufEfxInProcess(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSrc, UINT32 *pDst, UINT32 Size)
{
    AMBA_AENC_FLOW_EFX_FC_s      *pFc = &(pHdlr->EfxIn.Fc);
    AMBA_AENC_FLOW_EFX_VOLUME_s  *pVolume = &(pHdlr->EfxIn.Volume);
    AMBA_AENC_FLOW_EFX_DWS_s     *pDws = &(pHdlr->EfxIn.Dws);
    AMBA_AENC_DCBLOCKER_s        *pDcBlocker = &(pHdlr->EfxIn.DcBlocker);
    UINT32 NewFrameSize = Size / pFc->SrcCh / (pFc->SrcRes / 8U);
    UINT32 RetVal = ABUF_OK;
    UINT32 Factor;

    if (pFc->Enable == 1U) {
        pFc->FrameSize     = NewFrameSize;
        if (AmbaAudioBufEfx_FcProc(pFc, pSrc, pDst) != ABUF_OK) {
            AmbaPrint_PrintStr5("AmbaAudioBufEfx_FcProc fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    } else {
        if (AmbaWrap_memcpy(pDst, pSrc, Size) != OK) {
            AmbaPrint_PrintStr5("AmbaWrap_memcpy fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    }
    if ((pDws->Enable == 1U) && (RetVal == OK)) {
        Factor = (pDws->SrcFreq / pDws->DstFreq);
        NewFrameSize = NewFrameSize / Factor;
        pDws->Ch     = pFc->DstCh;
        pDws->Res    = pFc->DstRes;
        pDws->FrameSize  = NewFrameSize;
        if (AmbaAudioBufEfx_DwsProc(pDws, pDst) != ABUF_OK) {
            AmbaPrint_PrintStr5("AmbaAudioBufEfx_DwsProc fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    }
    if ((pDcBlocker->Enable == 1U) && (RetVal == OK)) {
        pDcBlocker->Ch     = pFc->DstCh;
        pDcBlocker->Res    = pFc->DstRes;
        pDcBlocker->FrameSize  = NewFrameSize;
        if (AmbaAudioBufEfx_DcBlockerProc(pDcBlocker, pDst) != ABUF_OK) {
            AmbaPrint_PrintStr5("AmbaAudioBufEfx_DcBlockerProc fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    }
    if ((pVolume->Enable == 1U) && (RetVal == OK)) {
        pVolume->Ch     = pFc->DstCh;
        pVolume->Res    = pFc->DstRes;
        pVolume->FrameSize  = NewFrameSize;
        if (AmbaAudioBufEfx_VolumeProc(pVolume, pDst) != ABUF_OK) {
                    AmbaPrint_PrintStr5("AmbaAudioBufEfx_VolumeProc fail", NULL, NULL, NULL, NULL, NULL);
                    RetVal = ABUF_ERR_0000;
        }
    }

    return RetVal;
}

/**
* Buffer Efx-Out Process
* @param [in]  pHdlr buf control handler
* @param [in]  pSrc
* @param [in]  pDst
* @param [in]  FrameSize
* @return ErrorCode
*/
UINT32 AmbaAudio_BufEfxOutProcess(AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr, UINT32 *pSrc, UINT32 *pDst, UINT32 Size)
{
    AMBA_AENC_FLOW_EFX_FC_s      *pFc = &(pHdlr->EfxOut.Fc);
    AMBA_AENC_FLOW_EFX_VOLUME_s  *pVolume = &(pHdlr->EfxOut.Volume);
    UINT32 NewFrameSize = Size / pFc->DstCh / (pFc->DstRes / 8U);
    UINT32 RetVal = ABUF_OK;

    if (pFc->Enable == 1U) {
        pFc->FrameSize = NewFrameSize;
        if (AmbaAudioBufEfx_FcProc(pFc, pSrc, pDst) != ABUF_OK) {
            AmbaPrint_PrintStr5("AmbaAudioBufEfx_FcProc fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    } else {
        if (AmbaWrap_memcpy(pSrc, pDst, Size) != OK) {
            AmbaPrint_PrintStr5("AmbaWrap_memcpy fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    }
    if ((pVolume->Enable == 1U) && (RetVal == OK)) {
        pVolume->Ch     = pFc->DstCh;
        pVolume->Res    = pFc->DstRes;
        pVolume->FrameSize = NewFrameSize;
        if (AmbaAudioBufEfx_VolumeProc(pVolume, pDst) != ABUF_OK) {
            AmbaPrint_PrintStr5("AmbaAudioBufEfx_VolumeProc fail", NULL, NULL, NULL, NULL, NULL);
            RetVal = ABUF_ERR_0000;
        }
    }

    return RetVal;
}

/**
* Buffer Efx Setup
* @param [in]  Buffer Index
* @param [in]  Abu Buffer Direction
* @param [in]  Source Channel
* @param [in]  Destination Channel
* @param [in]  Source Resolution
* @param [in]  Destination Resolution
* @param [in]  Shift
* @param [in]  Frame size
* @return ErrorCode
*/
UINT32 AmbaAudio_BufSetup_Fc(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 SrcCh, UINT32 DstCh, UINT32 SrcRes, UINT32 DstRes, UINT32 Shift, UINT32 FrameSize)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    AMBA_AENC_FLOW_EFX_FC_s    *pFc;
    UINT32 RetVal = ABUF_OK;
    const INT32 ChAssignTable[AMBA_AENC_EFX_MAX_CH] = {0, 1, -1, -1, -1, -1, -1, -1};

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        if (AbuEfxDirection == 0U) {
            pFc = &(pHdlr->EfxIn.Fc);
        } else {
            pFc = &(pHdlr->EfxOut.Fc);
        }
        pFc->SrcCh          = SrcCh;
        pFc->DstCh          = DstCh;
        pFc->SrcRes         = SrcRes;
        pFc->DstRes         = DstRes;
        pFc->Shift          = Shift;
        pFc->FrameSize      = FrameSize;
        AmbaPrint_PrintUInt5("AmbaAudio_BufSetup_Fc() SrcCh %d DstCh %d SrcRes %d DstRes %d Shift %d",
                                                      SrcCh, DstCh, SrcRes, DstRes, Shift);
        if (OK != AmbaWrap_memcpy(&(pFc->ChAssignTable), ChAssignTable, sizeof(pFc->ChAssignTable))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memcpy pFc->ChAssignTable fail",__func__, NULL, NULL, NULL, NULL);
        }
        pFc->Enable         = 1U;
    } else {
        AmbaPrint_PrintUInt5("[Error!!] AmbaAudio_BufSetup_Fc() AudioEncBufHdlr[BufIndex].BufIndex == AU_BUF_NA_ID ", 0U, 0U, 0U, 0U, 0U);
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* Buffer Efx Setup Chain
* @param [in]  Buffer Index
* @param [in]  Abu Buffer Direction
* @param [in]  Chain Table
* @return ErrorCode
*/
UINT32 AmbaAudio_BufSetup_FcCa(UINT32 BufIndex, UINT32 AbuEfxDirection, const INT32 *pCaTable)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    AMBA_AENC_FLOW_EFX_FC_s      *pFc;
    UINT32 RetVal = ABUF_OK;

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        if (AbuEfxDirection == 0U) {
            pFc = &(pHdlr->EfxIn.Fc);
        } else {
            pFc = &(pHdlr->EfxOut.Fc);
        }
        if (OK != AmbaWrap_memcpy(&(pFc->ChAssignTable), pCaTable, sizeof(pFc->ChAssignTable))) {
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memcpy pFc->ChAssignTable fail",__func__, NULL, NULL, NULL, NULL);
        }
        pFc->Enable         = 1U;
    } else {
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* Buffer Setup Volume
* @param [in]  Buffer Index
* @param [in]  Abu Buffer Direction
* @param [in]  Channel
* @param [in]  Resolution
* @param [in]  Frame size
* @param [in]  Volume Level
* @return ErrorCode
*/
UINT32 AmbaAudio_BufSetup_Volume(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 Ch, UINT32 Res, UINT32 FrameSize, UINT32 Level)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    AMBA_AENC_FLOW_EFX_VOLUME_s  *pVolume;
    UINT32 RetVal = ABUF_OK;

    (void)Ch;
    (void)Res;
    (void)FrameSize;

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        if (AbuEfxDirection == 0U) {
            pVolume = &(pHdlr->EfxIn.Volume);
        } else {
            pVolume = &(pHdlr->EfxOut.Volume);
        }
        pVolume->Level     = Level;
        pVolume->Enable    = 1U;
    } else {
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* Buffer Setup Down-sampling rate
* @param [in]  Buffer Index
* @param [in]  Abu Buffer Direction
* @param [in]  Source Frequency
* @param [in]  Destination Frequency
* @return ErrorCode
*/
UINT32 AmbaAudio_BufSetup_Dws(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 SrcFreq, UINT32 DstFreq)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    AMBA_AENC_FLOW_EFX_DWS_s  *pDws;
    UINT32 RetVal = ABUF_OK;

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        if (AbuEfxDirection == 0U) {
            pDws = &(pHdlr->EfxIn.Dws);
        } else {
            pDws = &(pHdlr->EfxOut.Dws);
        }
        pDws->SrcFreq = SrcFreq;
        pDws->DstFreq = DstFreq;
        pDws->Enable = 0U;
        if (pDws->SrcFreq > pDws->DstFreq) {
            if (((pDws->SrcFreq / pDws->DstFreq) == 2U) ||
                ((pDws->SrcFreq / pDws->DstFreq) == 4U) ||
                ((pDws->SrcFreq / pDws->DstFreq) == 6U)) {
                pDws->Enable    = 1U;
            } else {
                RetVal = ABUF_ERR_0001;
            }
        } else {
            RetVal = ABUF_ERR_0001;
        }
        if (RetVal != ABUF_OK) {
            AmbaPrint_PrintUInt5("AU EFX: Can't support down-sample from %d to %d", pDws->SrcFreq, pDws->DstFreq, 0U, 0U, 0U);
        }
    } else {
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}

/**
* Buffer Setup DC Blocking filter
* @param [in]  Buffer Index
* @param [in]  Abu Buffer Direction
* @param [in]  Channel
* @param [in]  Resolution
* @param [in]  Frame size
* @param [in]  Filter Alpha Level
* @return ErrorCode
*/
UINT32 AmbaAudio_BufSetup_DcBlocker(UINT32 BufIndex, UINT32 AbuEfxDirection, UINT32 Alpha)
{
    AMBA_AUDIO_ENC_BUF_CTRL_s *pHdlr;
    AMBA_AENC_DCBLOCKER_s  *pDcBlocker;
    UINT32 RetVal = ABUF_OK;

    if (AudioEncBufHdlr[BufIndex].BufIndex != AU_BUF_NA_ID) {
        pHdlr = &AudioEncBufHdlr[BufIndex];
        if (AbuEfxDirection == 0U) {
            pDcBlocker = &(pHdlr->EfxIn.DcBlocker);
        } else {
            pDcBlocker = &(pHdlr->EfxOut.DcBlocker);
        }
        pDcBlocker->Alpha     = Alpha;
        pDcBlocker->Enable    = 1U;
    } else {
        RetVal = ABUF_ERR_0000;
    }

    return RetVal;
}
