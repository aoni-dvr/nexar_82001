/**
*  @file AmbaAEncFlow.c
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
*  @details amba audio encode flow
*
*/

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaI2S.h"
#include "AmbaPrint.h"
#include "AmbaDMA_Def.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudio.h"
#include "AmbaSvcWrap.h"
#include "AmbaAEncFlow.h"
#include "AmbaCodecCom.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "AmbaAudio_CODEC.h"
#include "../../../../../../apps/icam/cardv/record/pcm_queue.h"
#include "../../../../../../apps/icam/cardv/system/stream_share.h"
#if defined(CONFIG_ENABLE_AMBALINK) && defined(CONFIG_AMBA_RTSP)
#include "AmbaDSP_VideoEnc.h"
#include "SvcRtspLink.h"
#endif
#endif


static AMBA_AIN_CB_HDLR_s  AinCbHdlr[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_AENC_CB_HDLR_s AencCbHdlr[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 *pAinHdlr GNU_SECTION_NOZEROINIT;
static UINT32 *pAencHdlr[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;

static AMBA_AIN_DATA_INFO_s *plastAinDataInfo[AMBA_AUDIO_MAX_BUF_NUM];
static AMBA_AENC_DATA_INFO_s *plastAencDataInfo[AMBA_AUDIO_MAX_BUF_NUM];

static UINT32 *pCurWritePtr[AMBA_AUDIO_MAX_BUF_NUM];
static UINT32 CurWritePtrPos[AMBA_AUDIO_MAX_BUF_NUM];
static UINT32 AencMuxIdx[AMBA_AUDIO_MAX_BUF_NUM];
static UINT32 *pCurReadPtr[AMBA_AUDIO_MAX_BUF_NUM];
static UINT32 CurReadPtrPos[AMBA_AUDIO_MAX_BUF_NUM];
static UINT32 AudioFileWrite = 0;

static AMBA_AUDIO_INPUT_s       g_AinCfgArr[AMBA_NUM_I2S_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_AUDIO_ENC_s         EncInfo;
static AMBA_AUDIO_SETUP_INFO_s  SetupInfo;
static UINT32                   g_AEncCoding = AMBA_AUDIO_TYPE_AAC;
static AENC_DATA_HDLR_CB        g_pCBDataRdy[AMBA_AUDIO_MAX_BUF_NUM];

static inline void LOG_AUDIO_APP_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static inline void LOG_AUDIO_APP_OK(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

static UINT32 SvcAin2AbuEfxGetSize(const AMBA_AUDIO_ENC_BUF_CTRL_s *pBufHdlr, UINT32 Size)
{
    const AMBA_AENC_FLOW_EFX_FC_s *pFc = &(pBufHdlr->EfxIn.Fc);
    const AMBA_AENC_FLOW_EFX_DWS_s *pDws = &(pBufHdlr->EfxIn.Dws);
    UINT32 RefineSize = Size;

    if (pFc->DstCh != pFc->SrcCh) {
        if (pFc->DstCh > pFc->SrcCh) {
            RefineSize *= (pFc->DstCh / pFc->SrcCh);
        } else {
            RefineSize /= (pFc->SrcCh / pFc->DstCh);
        }
    }

    if (pFc->DstRes != pFc->SrcRes) {
        if (pFc->DstRes > pFc->SrcRes) {
            RefineSize *= (pFc->DstRes / pFc->SrcRes);
        } else {
            RefineSize /= (pFc->SrcRes / pFc->DstRes);
        }
    }

    if (pDws->SrcFreq != pDws->DstFreq) {
        if (pDws->DstFreq > pDws->SrcFreq) {
            RefineSize *= (pDws->DstFreq / pDws->SrcFreq);
        } else {
            RefineSize /= (pDws->SrcFreq / pDws->DstFreq);
        }
    }

    return RefineSize;
}

static UINT32 SvcAinGetCopySizeFromDescRoom(const AMBA_AUDIO_ENC_BUF_CTRL_s *pBufHdlr, UINT32 Size)
{
    const AMBA_AENC_FLOW_EFX_FC_s *pFc = &(pBufHdlr->EfxIn.Fc);
    const AMBA_AENC_FLOW_EFX_DWS_s *pDws = &(pBufHdlr->EfxIn.Dws);
    UINT32 RefineSize = Size;

    if (pFc->DstCh != pFc->SrcCh) {
        if (pFc->DstCh > pFc->SrcCh) {
            RefineSize /= (pFc->DstCh / pFc->SrcCh);
        } else {
            RefineSize *= (pFc->SrcCh / pFc->DstCh);
        }
    }

    if (pFc->DstRes != pFc->SrcRes) {
        if (pFc->DstRes > pFc->SrcRes) {
            RefineSize /= (pFc->DstRes / pFc->SrcRes);
        } else {
            RefineSize *= (pFc->SrcRes / pFc->DstRes);
        }
    }

    if (pDws->SrcFreq != pDws->DstFreq) {
        if (pDws->DstFreq > pDws->SrcFreq) {
            RefineSize /= (pDws->DstFreq / pDws->SrcFreq);
        } else {
            RefineSize *= (pDws->SrcFreq / pDws->DstFreq);
        }
    }

    return RefineSize;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static int pcm_encode(unsigned int *src, int src_len, unsigned char *dest, int *dest_len, int channel)
{
    int i = 0, j = 0, k = 0;
    unsigned char *pTrans = NULL;
    int Resolution = 2;
    int is_dmic = AmbaAudio_CodecGetMicType(0) == AUCODEC_DMIC_IN;

    for (i = 0; i < src_len / (int)(channel * sizeof(UINT32)); i++) {
        if (is_dmic) {
            src++;
        }
        for (j = 0; j < channel; j++) {
            pTrans = (unsigned char *)src;
            /*32bit transfers to all kind of the resolutions  */
            for (k = 4 - Resolution; k < 4; k++) {
                *dest = pTrans[k];
                dest++;
                (*dest_len) = (*dest_len) + 1;
            }
        }
        if (is_dmic) {
            src += 1;
        } else {
            src += 2;
        }
    }

    return 0;
}
#endif


//#define AIN_SINE_INPUT
static UINT32 AinPutPcmCb(UINT32 AbuIndex, const AMBA_AIN_DATA_INFO_s *pInfo)
{
    UINT32 RetVal = (UINT32)AIN2AENC_OK;
    AMBA_AIN_DATA_INFO_s *pInfoGet;
    UINT32  *pWp;
    UINT32  *pRp;
    UINT32  DescRoom;
    UINT32  AinPutSize;
    UINT32  AbuGetSize;
    UINT32  CpSize;

#ifdef AIN_SINE_INPUT
    static UINT32 SineTable[48] = {
        0x00000000UL, 0x10B5150FUL, 0x2120FB83UL, 0x30FBC54DUL, 0x40000000UL, 0x4DEBE4FEUL, 0x5A827999UL, 0x658C9A2DUL,
        0x6ED9EBA1UL, 0x7641AF3CUL, 0x7BA3751CUL, 0x7EE7AA4BUL, 0x7FFFFFFFUL, 0x7EE7AA4BUL, 0x7BA3751CUL, 0x7641AF3CUL,
        0x6ED9EBA1UL, 0x658C9A2DUL, 0x5A827999UL, 0x4DEBE4FEUL, 0x40000000UL, 0x30FBC54DUL, 0x2120FB83UL, 0x10B5150FUL,
        0x00000000UL, 0xEF4AEAF1UL, 0xDEDF047DUL, 0xCF043AB3UL, 0xC0000001UL, 0xB2141B02UL, 0xA57D8667UL, 0x9A7365D3UL,
        0x9126145FUL, 0x89BE50C4UL, 0x845C8AE4UL, 0x811855B5UL, 0x80000001UL, 0x811855B5UL, 0x845C8AE4UL, 0x89BE50C4UL,
        0x9126145FUL, 0x9A7365D3UL, 0xA57D8667UL, 0xB2141B02UL, 0xC0000000UL, 0xCF043AB3UL, 0xDEDF047DUL, 0xEF4AEAF1UL
    };
    static UINT32 SinCnt[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
#endif

    //AmbaPrint_PrintStr5("Func: [%s]",__func__, NULL, NULL, NULL, NULL);
    /*Fix CC error*/
    if (pInfo == NULL) {
        RetVal = (UINT32)AIN2AENC_ERR_0000;
    } else {
#ifdef AIN_SINE_INPUT
    {
        UINT32 SinLoop;
        UINT32 *pDstPtr;
        UINT32 SineFrameSize;
        UINT32 i;
        UINT32 Sample;
        UINT16 *pDstPtr_16;
        AMBA_AENC_FLOW_EFX_FC_s *pFc = &(AudioEncBufHdlr[AbuIndex].EfxIn.Fc);

        pDstPtr = pInfo->pPcmBuf;
        SineFrameSize = pInfo->Size / pFc->SrcCh / (pFc->SrcRes / 8U);

        if (pFc->SrcRes == 32U) {
            for (SinLoop = 0U; SinLoop < SineFrameSize; SinLoop++) {
                Sample = SineTable[SinCnt[AbuIndex] % 48U];
                for (i = 0U; i < pFc->SrcCh; i++) {
                    *pDstPtr = Sample;
                    pDstPtr++;
                }
                SinCnt[AbuIndex]++;
            }
        } else {
            (void)AmbaWrap_memcpy(&pDstPtr_16, &pInfo->pPcmBuf, sizeof(pDstPtr_16));
            for (SinLoop = 0U; SinLoop < SineFrameSize; SinLoop++) {
                Sample = SineTable[SinCnt[AbuIndex] % 48U];
                for (i = 0U; i < pFc->SrcCh; i++) {
                    *pDstPtr_16 = (UINT16)((Sample >> 16U) & 0xFFFFU);
                    pDstPtr_16++;
                }
                SinCnt[AbuIndex]++;
            }
        }
    }
#endif

        if (plastAinDataInfo[AbuIndex] == NULL) {
            plastAinDataInfo[AbuIndex] = AmbaAudio_EncBufGetWritePointer(&AudioEncBufHdlr[AbuIndex]);
            pCurWritePtr[AbuIndex] = plastAinDataInfo[AbuIndex]->pPcmBuf;
            CurWritePtrPos[AbuIndex] = 0U;
        }
        pInfoGet        = plastAinDataInfo[AbuIndex];
        pWp             = pCurWritePtr[AbuIndex];
        pRp             = pInfo->pPcmBuf;
        DescRoom        = (AU_SSP_BUF_CHUNK_SIZE) - CurWritePtrPos[AbuIndex];
        AinPutSize      = pInfo->Size;
        AbuGetSize      = SvcAin2AbuEfxGetSize(&AudioEncBufHdlr[AbuIndex], AinPutSize);

        //AmbaPrint_PrintUInt5("AinPutSize: %d, AbuGetSize: %d, DescRoom: %d", AinPutSize, AbuGetSize, DescRoom, 0U, 0U);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        {
            unsigned char backup[AU_SSP_BUF_CHUNK_SIZE / AENC_CH_NUM * AIN_CH_NUM] = {0};
            unsigned char dest[AU_SSP_BUF_CHUNK_SIZE / AENC_CH_NUM * AIN_CH_NUM / 2] = {0};
            int dest_len = 0;

            AmbaWrap_memcpy(backup, pInfo->pPcmBuf, pInfo->Size);
            pcm_encode((unsigned int *)backup, pInfo->Size, dest, &dest_len, AIN_CH_NUM);
            if (dest_len > 0) {
                pcm_frame_info_s *pcm_frame = NULL;
                int index = 0;
                stream_share_send_data(1, dest, dest_len, 0);
                pcm_queue_get(&pcm_frame, &index);
                (void)AmbaWrap_memcpy(pcm_frame->buffer, dest, dest_len);
                pcm_frame->len = dest_len;
                pcm_queue_push(index);
                //pcm_share_update_buffer(dest, dest_len);
            }
        }
#endif

        while (AinPutSize != 0U) {
            if ((AbuGetSize <= DescRoom) && (pInfo->Lof == 1U)) {
                plastAinDataInfo[AbuIndex]->Lof = 1U;
            } else {
                plastAinDataInfo[AbuIndex]->Lof = 0U;
            }
            if (DescRoom >= AbuGetSize) {
                UINT32 RtVal;
                CpSize  = AinPutSize;
                //AmbaSvcWrap_MisraMemcpy(pWp, pRp, CpSize);
                RtVal = AmbaAudio_BufEfxInProcess(&AudioEncBufHdlr[AbuIndex], pRp, pWp, CpSize);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[1] AmbaAudio_BufEfxInProcess error ", 0U, 0U);
                }

                pRp = &pRp[CpSize / 4U];//pRp += CpSize;
                pWp = &pWp[AbuGetSize / 4U];//pWp += CpSize;
                CurWritePtrPos[AbuIndex] += AbuGetSize;
                AinPutSize -= CpSize;
                DescRoom -= AbuGetSize;
                if ((pInfo->Lof == 1U) || (DescRoom == 0U)) {
                    AmbaSvcWrap_MisraMemset(pWp, 0, DescRoom);
                    plastAinDataInfo[AbuIndex] = NULL;
                    pCurWritePtr[AbuIndex] = NULL;
                    pInfoGet->Size = (AU_SSP_BUF_CHUNK_SIZE) - DescRoom;
                    pInfoGet->AudioTicks = pInfo->AudioTicks;
                    pInfoGet->Lof = pInfo->Lof;
                    RetVal = AmbaAudio_EncBufUpdateWritePointer(&AudioEncBufHdlr[AbuIndex]);
                    DescRoom = 0U;
                    AinPutSize = 0U;
                }
            } else { /* DescRoom < FrmBufRemain */
                UINT32 RtVal;
                CpSize  = SvcAinGetCopySizeFromDescRoom(&AudioEncBufHdlr[AbuIndex], DescRoom);
                //AmbaSvcWrap_MisraMemcpy(pWp, pRp, CpSize);
                RtVal = AmbaAudio_BufEfxInProcess(&AudioEncBufHdlr[AbuIndex], pRp, pWp, CpSize);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[2] AmbaAudio_BufEfxInProcess error ", 0U, 0U);
                }
                pRp = &pRp[CpSize / 4U];//pRp += CpSize;
                //pWp += CpSize;
                AinPutSize -= CpSize;
                //DescRoom -= CpSize;
                plastAinDataInfo[AbuIndex] = NULL;
                pCurWritePtr[AbuIndex] = NULL;
                pInfoGet->Size = AU_SSP_BUF_CHUNK_SIZE;
                pInfoGet->AudioTicks = pInfo->AudioTicks;
                pInfoGet->Lof = pInfo->Lof;
                RetVal = AmbaAudio_EncBufUpdateWritePointer(&AudioEncBufHdlr[AbuIndex]);

                /* Get next desc */
                plastAinDataInfo[AbuIndex] = AmbaAudio_EncBufGetWritePointer(&AudioEncBufHdlr[AbuIndex]);
                pCurWritePtr[AbuIndex] = plastAinDataInfo[AbuIndex]->pPcmBuf;
                CurWritePtrPos[AbuIndex] = 0U;
                pInfoGet        = plastAinDataInfo[AbuIndex];
                pWp             = pCurWritePtr[AbuIndex];
                DescRoom = AU_SSP_BUF_CHUNK_SIZE;
            }
        }
        if (plastAinDataInfo[AbuIndex] != NULL) {
            AmbaMisra_TypeCast(&pCurWritePtr[AbuIndex], &pWp); /*pCurWritePtr[AbuIndex] = (UINT32 *)pWp;*/
        }
    }

    return RetVal;
}

static UINT32 AinGetSizeCb(UINT32 AbuIndex, UINT32 *pSize)
{
    UINT32 RetVal;

    const AMBA_AIN_DATA_INFO_s *pInfo = plastAinDataInfo[AbuIndex];
    UINT32 DescRemain;
    UINT32 Size;

    if (pInfo == NULL) {
        DescRemain = 0U;
    } else {
        DescRemain = (AU_SSP_BUF_CHUNK_SIZE) - CurWritePtrPos[AbuIndex];
    }

    RetVal = AmbaAudio_EncBufPeekRoomSize(&AudioEncBufHdlr[AbuIndex], &Size);

    *pSize = Size + DescRemain; /* Max possible remain size */
    *pSize = SvcAinGetCopySizeFromDescRoom(&AudioEncBufHdlr[AbuIndex], *pSize);

    return RetVal;
}

static UINT32 Abu2AencEfxPutSize(const AMBA_AUDIO_ENC_BUF_CTRL_s *pBufHdlr, UINT32 Size)
{
    const AMBA_AENC_FLOW_EFX_FC_s *pFc = &(pBufHdlr->EfxOut.Fc);
    UINT32 RefineSize = Size;

    if (pFc->DstCh != pFc->SrcCh) {
        if (pFc->DstCh > pFc->SrcCh) {
            RefineSize /= (pFc->DstCh / pFc->SrcCh);
        } else {
            RefineSize *= (pFc->SrcCh / pFc->DstCh);
        }
    }

    if (pFc->DstRes != pFc->SrcRes) {
        if (pFc->DstRes > pFc->SrcRes) {
            RefineSize /= (pFc->DstRes / pFc->SrcRes);
        } else {
            RefineSize *= (pFc->SrcRes / pFc->DstRes);
        }
    }

    return RefineSize;
}

static UINT32 AmbaAencGetCopySizeFromDescRemain(const AMBA_AUDIO_ENC_BUF_CTRL_s *pBufHdlr, UINT32 Size)
{
    const AMBA_AENC_FLOW_EFX_FC_s *pFc = &(pBufHdlr->EfxOut.Fc);
    UINT32 RefineSize = Size;

    if (pFc->DstCh != pFc->SrcCh) {
        if (pFc->DstCh > pFc->SrcCh) {
            RefineSize *= (pFc->DstCh / pFc->SrcCh);
        } else {
            RefineSize /= (pFc->SrcCh / pFc->DstCh);
        }
    }

    if (pFc->DstRes != pFc->SrcRes) {
        if (pFc->DstRes > pFc->SrcRes) {
            RefineSize *= (pFc->DstRes / pFc->SrcRes);
        } else {
            RefineSize /= (pFc->SrcRes / pFc->DstRes);
        }
    }

    return RefineSize;
}

static UINT32 AencGetPcmCb(UINT32 AbuIndex, AMBA_AENC_DATA_INFO_s *pInfo)
{
    UINT32 RetVal = AIN2AENC_OK;
    AMBA_AENC_DATA_INFO_s *pInfoGet;
    UINT32  DescSize;
    UINT32  *pRp;//desc rp
    UINT32  *pWp;//frmbuf wp
    UINT32  DescRemain;
    //UINT32  FrmBufRoom;//bytes to move
    UINT32  AbuPutSize;
    UINT32  AencGetSize;
    UINT32  CpSize;
    UINT32  TotalCpSize = 0U;

#ifdef AENC_SINE_INPUT
    static UINT32 SineTable[48] = {
        0x00000000UL, 0x10B5150FUL, 0x2120FB83UL, 0x30FBC54DUL, 0x40000000UL, 0x4DEBE4FEUL, 0x5A827999UL, 0x658C9A2DUL,
        0x6ED9EBA1UL, 0x7641AF3CUL, 0x7BA3751CUL, 0x7EE7AA4BUL, 0x7FFFFFFFUL, 0x7EE7AA4BUL, 0x7BA3751CUL, 0x7641AF3CUL,
        0x6ED9EBA1UL, 0x658C9A2DUL, 0x5A827999UL, 0x4DEBE4FEUL, 0x40000000UL, 0x30FBC54DUL, 0x2120FB83UL, 0x10B5150FUL,
        0x00000000UL, 0xEF4AEAF1UL, 0xDEDF047DUL, 0xCF043AB3UL, 0xC0000001UL, 0xB2141B02UL, 0xA57D8667UL, 0x9A7365D3UL,
        0x9126145FUL, 0x89BE50C4UL, 0x845C8AE4UL, 0x811855B5UL, 0x80000001UL, 0x811855B5UL, 0x845C8AE4UL, 0x89BE50C4UL,
        0x9126145FUL, 0x9A7365D3UL, 0xA57D8667UL, 0xB2141B02UL, 0xC0000000UL, 0xCF043AB3UL, 0xDEDF047DUL, 0xEF4AEAF1UL
    };
    static UINT32 SinCnt[AMBA_AENC_AU_MAX_ABU_NUM] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
#endif

    //AmbaPrint_PrintStr5("Func: [%s]",__func__, NULL, NULL, NULL, NULL);

    if (pInfo == NULL) {
        RetVal = AIN2AENC_ERR_0000;
    } else {
        if (plastAencDataInfo[AbuIndex] == NULL) {
            plastAencDataInfo[AbuIndex] = AmbaAudio_EncBufGetReadPointer(&AudioEncBufHdlr[AbuIndex]);
            pCurReadPtr[AbuIndex] = plastAencDataInfo[AbuIndex]->pPcmBuf;
            CurReadPtrPos[AbuIndex] = 0U;
            pInfo->AudioTicks = plastAencDataInfo[AbuIndex]->AudioTicks;
        }
        pInfoGet    = plastAencDataInfo[AbuIndex];
        DescSize    = pInfoGet->Size;
        pRp         = pCurReadPtr[AbuIndex];
        pWp         = pInfo->pPcmBuf;
        //DescRemain  = DescSize - ((UINT32)(pRp) - (UINT32)(pInfoGet->pPcmBuf));
        DescRemain  = DescSize - CurReadPtrPos[AbuIndex];
        AencGetSize = pInfo->Size;
        AbuPutSize  = Abu2AencEfxPutSize(&AudioEncBufHdlr[AbuIndex], AencGetSize);
        pInfo->Lof  = 0U;

        //AmbaPrint_PrintUInt5("DescRemain: %d, AencGetSize: %d, AbuPutSize: %d", DescRemain, AencGetSize, AbuPutSize, 0U, 0U);

        while (AencGetSize != 0U) {
            if (DescRemain >= AbuPutSize) {
                UINT32 RtVal;
                CpSize  = AencGetSize;
                RtVal = AmbaAudio_BufEfxOutProcess(&AudioEncBufHdlr[AbuIndex], pRp, pWp, CpSize);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[1] AmbaAudio_BufEfxOutProcess error ", 0U, 0U);
                }
                pRp = &pRp[AbuPutSize / 4U];//pRp += CpSize;
                pWp = &pWp[CpSize / 4U];//pWp += CpSize;
                CurReadPtrPos[AbuIndex] += AbuPutSize;
                DescRemain   -=  AbuPutSize;
                AencGetSize  -=  CpSize;
            } else {
                UINT32 RtVal;
                CpSize  = AmbaAencGetCopySizeFromDescRemain(&AudioEncBufHdlr[AbuIndex], DescRemain);
                RtVal = AmbaAudio_BufEfxOutProcess(&AudioEncBufHdlr[AbuIndex], pRp, pWp, CpSize);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[2] AmbaAudio_BufEfxOutProcess error ", 0U, 0U);
                }
                //pRp = &pRp[AbuPutSize / 4U];//pRp += CpSize;
                pWp = &pWp[CpSize / 4U];//pWp += CpSize;
                AencGetSize  -=  CpSize;
                DescRemain   =  0U;
            }
            TotalCpSize +=  CpSize;
            if ((pInfoGet->Lof == 1U) && (DescRemain == 0U)) {
                pInfoGet->Lof   = 0U;
                pInfo->Lof      = 1U;
                AencGetSize     = 0U;
            }
            if (DescRemain == 0U) {
                RetVal = AmbaAudio_EncBufUpdateReadPointer(&AudioEncBufHdlr[AbuIndex]);
                plastAencDataInfo[AbuIndex] = NULL;
                pCurReadPtr[AbuIndex] = NULL;
                if (AencGetSize > 0U) {
                    plastAencDataInfo[AbuIndex] = AmbaAudio_EncBufGetReadPointer(&AudioEncBufHdlr[AbuIndex]);
                    pInfo->AudioTicks = plastAencDataInfo[AbuIndex]->AudioTicks;
                    pCurReadPtr[AbuIndex] = plastAencDataInfo[AbuIndex]->pPcmBuf;
                    CurReadPtrPos[AbuIndex] = 0U;
                    pInfoGet = plastAencDataInfo[AbuIndex];
                    pRp         = pCurReadPtr[AbuIndex];
                    DescRemain  = pInfoGet->Size;
                }
            }
        }
        if (plastAencDataInfo[AbuIndex] != NULL) {
            AmbaMisra_TypeCast(&pCurReadPtr[AbuIndex], &pRp); /*pCurReadPtr[AbuIndex] = (UINT32 *)pRp;*/
        }
        pInfo->Size = TotalCpSize;
    }

#ifdef AENC_SINE_INPUT
    {
        UINT32 SinLoop;
        UINT32 *pDstPtr;
        UINT16 *pDstPtr_16;
        UINT32 Sample;
        UINT32 i;
        UINT32 AbuRes = SvcEfxOut.Fc.DstRes;
        UINT32 Ch;

        if (pInfo != NULL) {
            pDstPtr = pInfo->pPcmBuf;
            AmbaSvcWrap_MisraMemcpy(&pDstPtr_16, &pInfo->pPcmBuf, sizeof(pDstPtr_16));
            Ch = ((pInfo->Size / AENC_FRAME_SIZE) / (AbuRes / 8U));

            if (AbuRes == 32U) {
                for (SinLoop = 0U; SinLoop < AENC_FRAME_SIZE; SinLoop++) {
                    Sample = SineTable[SinCnt[AbuIndex] % 48U];
                    for (i = 0U; i < Ch; i++) {
                        *pDstPtr = Sample;
                        pDstPtr++;
                    }
                    SinCnt[AbuIndex]++;
                }
            } else {
                for (SinLoop = 0U; SinLoop < AENC_FRAME_SIZE; SinLoop++) {
                    Sample = SineTable[SinCnt[AbuIndex] % 48U];
                    for (i = 0U; i < Ch; i++) {
                        *pDstPtr_16 = (UINT16)((Sample >> 16U) & 0xFFFFU);
                        pDstPtr_16++;
                    }
                    SinCnt[AbuIndex]++;
                }
            }
        }
    }
#endif

    return RetVal;
}

static UINT32 AencGetSizeCb(UINT32 AbuIndex, UINT32 *pSize)
{
    UINT32 RetVal;

    const AMBA_AENC_DATA_INFO_s *pInfo = plastAencDataInfo[AbuIndex];
    UINT32  DescRemain;
    UINT32  Size;

    if (pInfo == NULL) {
        DescRemain = 0;
    } else {
        DescRemain = pInfo->Size - CurReadPtrPos[AbuIndex];//((UINT32)(pCurReadPtr[AbuIndex]) - (UINT32)(pInfo->pPcmBuf));
    }

    RetVal = AmbaAudio_EncBufPeekRemainSize(&AudioEncBufHdlr[AbuIndex], &Size);

    *pSize = Size + DescRemain; /* Max possible remain size */
    *pSize = AmbaAencGetCopySizeFromDescRemain(&AudioEncBufHdlr[AbuIndex], *pSize);

    return RetVal;
}

UINT32 AmbaAEncFlow_CheckAencId(const UINT32 *pHdlr)
{
    UINT32 i, RtVal = 0U;
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        if (pAencHdlr[i] == pHdlr) {
            RtVal = i;
            break;
        }
    }
    if (i == AMBA_AUDIO_MAX_BUF_NUM) {
        RtVal = 0U;
        AmbaPrint_PrintStr5("%s: No corresponding AENC id!", __func__, NULL, NULL, NULL, NULL);
    }
    return RtVal;
}

static UINT32 AencDataHdlrCB(const void *pEventData)
{
    const AMBA_AENC_AUDIO_DESC_s *pBitsBufInfo;
    AMBA_AENC_AUDIO_DESC_s *pDesc;
    UINT32 AencIndex, RtVal;

    AmbaMisra_TypeCast(&pBitsBufInfo, &pEventData);
    if (1U == AudioFileWrite) {
        AencIndex = AmbaAEncFlow_CheckAencId(pBitsBufInfo->pHdlr);
        pDesc = Audio_EncGetWritePointer(&AudioMuxHdlr[AencIndex]);
        AmbaSvcWrap_MisraMemcpy(pDesc, pBitsBufInfo, sizeof(AMBA_AENC_AUDIO_DESC_s));
        RtVal = Audio_EncUpdateWritePointer(&AudioMuxHdlr[AencIndex]);
    } else {
        RtVal = AIN2AENC_OK;
    }

//    LOG_AUDIO_APP_OK("[A] audio data_ready, eos/size(%X/%X)!!!", pBitsBufInfo->Eos, pBitsBufInfo->DataSize);
    return RtVal;
}

static void APPInit(void)
{
    pAinHdlr = NULL;

    for (UINT32 idx = 0U; idx < AMBA_AUDIO_MAX_BUF_NUM; idx++) {
        AencMuxIdx[idx] = 0U;
        pAencHdlr[idx] = NULL;
        plastAencDataInfo[idx] = NULL;
        pCurReadPtr[idx] = NULL;
        plastAinDataInfo[idx] = NULL;
        pCurWritePtr[idx] = NULL;
    }

}

/**
* Get enc info
* @param [out]  pInfo Enc Info
*/
void AmbaAEncFlow_InfoGet(AMBA_AENC_FLOW_INFO_s *pInfo)
{
    static UINT32 g_AinNum GNU_SECTION_NOZEROINIT;

    pInfo->pAinNum    = &(g_AinNum);
    pInfo->pAinInfo   = &(g_AinCfgArr[0]);
    pInfo->pAEncInfo  = &(EncInfo);
    pInfo->pAEncSetup = &(SetupInfo);
    pInfo->pAEncType  = &(g_AEncCoding);
}

/**
* Audio data ready callback init
*/
void AmbaAEncFlow_CBDataRdyInit(void)
{
    //Assign each g_pCBDataRdy[idx] to NULL
    for (UINT32 idx = 0U; idx < AMBA_AUDIO_MAX_BUF_NUM; idx++) {
        g_pCBDataRdy[idx] = NULL;
    }
}

/**
* Hook data ready callback
* @param [in]  CB index
* @param [in]  Enable Enable or not
* @param [in]  pEvtCB Callback func
* @return none
*/
void AmbaAEncFlow_HookEvent(UINT32 IdxCB, UINT32 Enable, const AENC_DATA_HDLR_CB pEvtCB)
{
    LOG_AUDIO_APP_OK("SvcAEncFlow_HookEvent(), Enable = %d IdxCB = %d", Enable, IdxCB);
    if (Enable == 1U) {
        g_pCBDataRdy[IdxCB] = pEvtCB;
    } else {
        g_pCBDataRdy[IdxCB] = NULL;
    }
}

void AmbaAEncFlow_EncAudioOnly(UINT32 AudioEncOnly)
{
    AudioFileWrite = AudioEncOnly;
    LOG_AUDIO_APP_OK("AmbaAEncFlow_EncAudioOnly(), AudioEncOnly : %d ", AudioEncOnly, 0U);
}

static UINT32 AinPutPcmCb_0(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(0U, pInfo);
}
static UINT32 AinGetSizeCb_0(UINT32 *pSize) {
    return AinGetSizeCb(0, pSize);
}
static UINT32 AencGetPcmCb_0(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(0U, pInfo);
}
static UINT32 AencGetSizeCb_0(UINT32 *pSize) {
    return AencGetSizeCb(0, pSize);
}

#if (CONFIG_AENC_NUM > 1)
static UINT32 AinPutPcmCb_1(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(1U, pInfo);
}
static UINT32 AinGetSizeCb_1(UINT32 *pSize) {
    return AinGetSizeCb(1, pSize);
}
static UINT32 AencGetPcmCb_1(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(1U, pInfo);
}
static UINT32 AencGetSizeCb_1(UINT32 *pSize) {
    return AencGetSizeCb(1, pSize);
}
#endif


#if (CONFIG_AENC_NUM > 2)
static UINT32 AinPutPcmCb_2(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(2U, pInfo);
}
static UINT32 AinGetSizeCb_2(UINT32 *pSize) {
    return AinGetSizeCb(2, pSize);
}
static UINT32 AencGetPcmCb_2(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(2U, pInfo);
}
static UINT32 AencGetSizeCb_2(UINT32 *pSize) {
    return AencGetSizeCb(2, pSize);
}
#endif


#if (CONFIG_AENC_NUM > 3)
static UINT32 AinPutPcmCb_3(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(3U, pInfo);
}
static UINT32 AinGetSizeCb_3(UINT32 *pSize) {
    return AinGetSizeCb(3, pSize);
}
static UINT32 AencGetPcmCb_3(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(3U, pInfo);
}
static UINT32 AencGetSizeCb_3(UINT32 *pSize) {
    return AencGetSizeCb(3, pSize);
}
#endif

#if (CONFIG_AENC_NUM > 4)
static UINT32 AinPutPcmCb_4(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(4U, pInfo);
}
static UINT32 AinGetSizeCb_4(UINT32 *pSize) {
    return AinGetSizeCb(4, pSize);
}
static UINT32 AencGetPcmCb_4(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(4U, pInfo);
}
static UINT32 AencGetSizeCb_4(UINT32 *pSize) {
    return AencGetSizeCb(4, pSize);
}
#endif

#if (CONFIG_AENC_NUM > 5)
static UINT32 AinPutPcmCb_5(const AMBA_AIN_DATA_INFO_s *pInfo) {
    return AinPutPcmCb(5U, pInfo);
}
static UINT32 AinGetSizeCb_5(UINT32 *pSize) {
    return AinGetSizeCb(5, pSize);
}
static UINT32 AencGetPcmCb_5(AMBA_AENC_DATA_INFO_s *pInfo) {
    return AencGetPcmCb(5U, pInfo);
}
static UINT32 AencGetSizeCb_5(UINT32 *pSize) {
    return AencGetSizeCb(5, pSize);
}
#endif


static void AEncFlow_strcat(char *dest, const char *src)
{
    UINT32 len1,len2;

    len1 = (UINT32)AmbaUtility_StringLength(src);
    len2 = (UINT32)AmbaUtility_StringLength(dest);

    AmbaUtility_StringAppend(dest, len1+len2+1U, src);
}

/**
* start audio encode flow
* @param [in]  pCbAinCtrl audio input control
* @return none
*/
void AmbaAEncFlow_Start(AMBA_AIN_CTRL_t pCbAinCtrl, UINT32 NumOfAin, UINT32 NumOfAenc)
{
    UINT32 RtVal;
    UINT32 CastValue;
    static UINT32 AinTaskStack[0x20000]  GNU_SECTION_NOZEROINIT;
    static UINT32 AencTaskStack[AMBA_AUDIO_MAX_BUF_NUM][0x20000] GNU_SECTION_NOZEROINIT;
    static char InputTskName[32]= "SvcAINTask";
    static char EncTskName[32]= "SvcAENCTask";
    static AMBA_AIN_PUT_PCM_f AmbaAinPutPcmCb[AMBA_AUDIO_MAX_BUF_NUM];
    static AMBA_AIN_GET_BUF_ROOM_SIZE_f AmbaAinGetSizeCb[AMBA_AUDIO_MAX_BUF_NUM];
    static AMBA_AENC_GET_PCM_f AmbaAencGetPcmCb[AMBA_AUDIO_MAX_BUF_NUM];
    static AMBA_AENC_GET_BUF_REMAIN_SIZE_f AmbaAencGetSizeCb[AMBA_AUDIO_MAX_BUF_NUM];
    static char FileName_AAC[AMBA_AUDIO_MAX_BUF_NUM][32];
    static char FileName_PCM[AMBA_AUDIO_MAX_BUF_NUM][32];
    const char FileName_Base[32] = "c:\\SVC_Aduio_";
    const char AAC_extension[5] = ".aac";
    const char PCM_extension[5] = ".pcm";
//    static INT32 SvcCaTableI[AMBA_AENC_EFX_MAX_CH] = {0, 1, -1, -1, -1, -1, -1, -1}; //Stereo

    AmbaAinPutPcmCb[0U]   = AinPutPcmCb_0;
    AmbaAinGetSizeCb[0U]  = AinGetSizeCb_0;
    AmbaAencGetPcmCb[0U]  = AencGetPcmCb_0;
    AmbaAencGetSizeCb[0U] = AencGetSizeCb_0;

#if (CONFIG_AENC_NUM > 1)
    AmbaAinPutPcmCb[1U]   = AinPutPcmCb_1;
    AmbaAinGetSizeCb[1U]  = AinGetSizeCb_1;
    AmbaAencGetPcmCb[1U]  = AencGetPcmCb_1;
    AmbaAencGetSizeCb[1U] = AencGetSizeCb_1;
#endif

#if (CONFIG_AENC_NUM > 2)
    AmbaAinPutPcmCb[2U]   = AinPutPcmCb_2;
    AmbaAinGetSizeCb[2U]  = AinGetSizeCb_2;
    AmbaAencGetPcmCb[2U]  = AencGetPcmCb_2;
    AmbaAencGetSizeCb[2U] = AencGetSizeCb_2;
#endif


#if (CONFIG_AENC_NUM > 3)
    AmbaAinPutPcmCb[3U]   = AinPutPcmCb_3;
    AmbaAinGetSizeCb[3U]  = AinGetSizeCb_3;
    AmbaAencGetPcmCb[3U]  = AencGetPcmCb_3;
    AmbaAencGetSizeCb[3U] = AencGetSizeCb_3;
#endif

#if (CONFIG_AENC_NUM > 4)
    AmbaAinPutPcmCb[4U]   = AinPutPcmCb_4;
    AmbaAinGetSizeCb[4U]  = AinGetSizeCb_4;
    AmbaAencGetPcmCb[4U]  = AencGetPcmCb_4;
    AmbaAencGetSizeCb[4U] = AencGetSizeCb_4;
#endif

#if (CONFIG_AENC_NUM > 5)
    AmbaAinPutPcmCb[5U]   = AinPutPcmCb_5;
    AmbaAinGetSizeCb[5U]  = AinGetSizeCb_5;
    AmbaAencGetPcmCb[5U]  = AencGetPcmCb_5;
    AmbaAencGetSizeCb[5U] = AencGetSizeCb_5;
#endif

    if (1U == AudioFileWrite) {
        //Setup audio only file name
        for (UINT32 FileIdx = 0U; FileIdx< AMBA_AUDIO_MAX_BUF_NUM; FileIdx++) {
            {//AAC
                UINT32 len;
                AmbaSvcWrap_MisraMemcpy(FileName_AAC[FileIdx], FileName_Base, sizeof(FileName_Base));
                len = (UINT32)AmbaUtility_StringLength(FileName_AAC[FileIdx]);
                AmbaUtility_StringAppendUInt32(FileName_AAC[FileIdx], len + (UINT32)sizeof(UINT32) + 1U, FileIdx, 10U);
                AEncFlow_strcat(FileName_AAC[FileIdx], AAC_extension);
            }
            {//PCM
                UINT32 len;
                AmbaSvcWrap_MisraMemcpy(FileName_PCM[FileIdx], FileName_Base, sizeof(FileName_Base));
                len = (UINT32)AmbaUtility_StringLength(FileName_PCM[FileIdx]);
                AmbaUtility_StringAppendUInt32(FileName_PCM[FileIdx], len + (UINT32)sizeof(UINT32) + 1U, FileIdx, 10U);
                AEncFlow_strcat(FileName_PCM[FileIdx], PCM_extension);
            }

            LOG_AUDIO_APP_OK("FileIdx = %d", FileIdx, 0U);
            AmbaPrint_PrintStr5("AAC name: %s PCMname: %s", FileName_AAC[FileIdx], FileName_PCM[FileIdx], NULL, NULL, NULL);
        }
    }

    APPInit();

    RtVal = AmbaAudioInp_QueryBufSize(&g_AinCfgArr[0].AinInfo);
    LOG_AUDIO_APP_OK("AmbaAudioInp_QueryBufSize, RtVal : %d ", RtVal, 0U);

    g_AinCfgArr[0].pInputTskName            = InputTskName;
    g_AinCfgArr[0].AinputTskCtrl.pStackBase = AinTaskStack;
    g_AinCfgArr[0].AinputTskCtrl.StackSize  = (UINT32)sizeof(AinTaskStack);

    RtVal = AmbaAudioInp_Create(&g_AinCfgArr[0], &pAinHdlr);

    LOG_AUDIO_APP_OK("AmbaAudioInp_Create, RtVal : %d ", RtVal, 0U);

    AmbaMisra_TypeCast(&CastValue, &pAinHdlr);
    LOG_AUDIO_APP_OK("AmbaAudioInp_Create return to APP, RtVal: %d, pAinHdlr: 0x%x", RtVal, CastValue);

    AmbaAudio_EncBufIdHdrInit();

    for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
        RtVal = AmbaAudio_EncBufCreate();
        if (AIN2AENC_OK != RtVal) {
            LOG_AUDIO_APP_NG("AmbaAudio_EncBufCreate error ", 0U, 0U);
        }
        AinCbHdlr[AencNum].PutPcm = AmbaAinPutPcmCb[AencNum];
        AinCbHdlr[AencNum].GetSize = AmbaAinGetSizeCb[AencNum];
        RtVal = AmbaAudioInp_IONodeRegister(pAinHdlr, &AinCbHdlr[AencNum]);
        LOG_AUDIO_APP_OK("AmbaAudioInp_IONodeRegister, [AencNum = %d] RtVal : %d ", AencNum, RtVal);
    }

    if (AMBA_AUDIO_TYPE_PCM == g_AEncCoding) {
        EncInfo.AencCreateInfo.PlugInLibSelfSize = (UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s);
    } else if (AMBA_AUDIO_TYPE_AAC == g_AEncCoding){
        EncInfo.AencCreateInfo.PlugInLibSelfSize = (UINT32)sizeof(AMBA_AUDIO_AAC_CONFIG_s);
    } else {
        LOG_AUDIO_APP_NG("AmbaAudioEnc_Setup wrong info", 0U, 0U);
    }
    RtVal = AmbaAudioEnc_QueryBufSize(&(EncInfo.AencCreateInfo));
    LOG_AUDIO_APP_OK("AmbaAudioEnc_QueryBufSize, RtVal : %d ", RtVal, 0U);


    for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {

        EncInfo.pAencTskName                    = EncTskName;
        EncInfo.AencTskIdx                      = AencNum;
        EncInfo.AencTskCtrl[AencNum].pStackBase = AencTaskStack[AencNum];
        EncInfo.AencTskCtrl[AencNum].StackSize  = (UINT32)sizeof(AencTaskStack[AencNum]);
        RtVal = AmbaAudioEnc_Init(&EncInfo, &pAencHdlr[AencNum]);
        LOG_AUDIO_APP_OK("AmbaAudioEnc_Init, RtVal : %d ", RtVal, 0U);
        AmbaMisra_TypeCast(&CastValue, &pAencHdlr[AencNum]);
        LOG_AUDIO_APP_OK("AmbaAENC_CreateResource return to APP, RtVal: %d, pAencHdlr: 0x%x", RtVal, CastValue);

        AencCbHdlr[AencNum].GetPcm = AmbaAencGetPcmCb[AencNum];
        AencCbHdlr[AencNum].GetSize = AmbaAencGetSizeCb[AencNum];
        RtVal = AmbaAudioEnc_IONodeRegister(pAencHdlr[AencNum], &AencCbHdlr[AencNum]);
        LOG_AUDIO_APP_OK("AmbaAudioEnc_IONodeRegister, RtVal : %d ", RtVal, 0U);
    }

    if (1U == AudioFileWrite) {
        AmbaAudio_MuxIdHdrInit();
        for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
            UINT32  *pBSHead = EncInfo.pAencBsInfo.pHead;
            UINT32  Offset = ((AencNum * EncInfo.pAencBsInfo.Size)/(UINT32)sizeof(UINT32));
            pBSHead = &(pBSHead[Offset]);
            AencMuxIdx[AencNum] = AmbaAudio_MuxCreate(34U, pBSHead, EncInfo.pAencBsInfo.Size);

            LOG_AUDIO_APP_OK("Create Aenc Mux task, AencMuxIdx[%d]: %d", AencNum, AencMuxIdx[AencNum]);
            if (AMBA_AUDIO_TYPE_PCM == g_AEncCoding) {
                RtVal = AmbaAudio_MuxTaskSetUp(AencMuxIdx[AencNum], FileName_PCM[AencNum]);
                AmbaPrint_PrintStr5("Mux create file: %s", FileName_PCM[AencNum], NULL, NULL, NULL, NULL);
            } else if (AMBA_AUDIO_TYPE_AAC == g_AEncCoding){
                RtVal = AmbaAudio_MuxTaskSetUp(AencMuxIdx[AencNum], FileName_AAC[AencNum]);
                AmbaPrint_PrintStr5("Mux create file: %s", FileName_AAC[AencNum], NULL, NULL, NULL, NULL);
            } else {
                LOG_AUDIO_APP_NG("Wrong EncType info, RtVal: %d", 0U, 0U);
            }
            LOG_AUDIO_APP_OK("MuxTaskSetUp, RtVal: %d", RtVal, 0U);
            RtVal = AmbaAudio_MuxTaskStart(AencMuxIdx[AencNum]);
            LOG_AUDIO_APP_OK("AmbaAudio_MuxTaskStart, RtVal: %d", RtVal, 0U);

            RtVal = AmbaAudioEnc_Create(pAencHdlr[AencNum], pBSHead, EncInfo.pAencBsInfo.Size, AencDataHdlrCB);
        }
    } else {
        for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
            if (g_pCBDataRdy[AencNum] != NULL) {
                UINT32  *pBSHead = EncInfo.pAencBsInfo.pHead;
                UINT32  Offset = ((AencNum * EncInfo.pAencBsInfo.Size)/(UINT32)sizeof(UINT32));
                pBSHead = &(pBSHead[Offset]);
                RtVal = AmbaAudioEnc_Create(pAencHdlr[AencNum], pBSHead, EncInfo.pAencBsInfo.Size, g_pCBDataRdy[AencNum]);
            } else {
                LOG_AUDIO_APP_NG("Error! SvcAEncFlow_Start() g_pCBDataRdy[%d] == NULL", AencNum, 0U);
            }
        }
    }

    LOG_AUDIO_APP_OK("AmbaAudioEnc_Create, RtVal : %d ", RtVal, 0U);

    for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
        SetupInfo.Type = g_AEncCoding;
        if (AMBA_AUDIO_TYPE_PCM == g_AEncCoding) {
            SetupInfo.PCM.AencPlugInCs.Update = 0U;
            SetupInfo.PCM.AencPlugInCs.SelfSize = (UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s);

            //[PCM] Audio format converter and ready for volume usage
            {
                /*
                   Second parameter, 'AbuEfxDirection == 0' means AIN to ABU direction
                   I2S input is 32-bit, SrcRes = 32 means AIN to ABU is 32 bit, ABU process 16bit
                */
                RtVal = AmbaAudio_BufSetup_Fc(AencNum, 0U, AIN_CH_NUM, AIN_CH_NUM,
                                                    SAMPLE_RESOLUTION,
                                                    SAMPLE_RESOLUTION,
                                                    0U, AENC_FRAME_SIZE);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[1] AmbaAudio_BufSetup_Fc error ", 0U, 0U);
                }
                /* If User need to assign chain table need to enable AmbaAudio_BufSetup_FcCa() */
    //            AmbaAudio_BufSetup_FcCa(0U, 0U, SvcCaTableI);

                /*
                   Second parameter, 'AbuEfxDirection == 1' means ABU to AOUT direction
                   SrcRes = 16 means ABU to AOUT is 16 bit, AOUT to 16bit
                */
                RtVal = AmbaAudio_BufSetup_Fc(AencNum, 1U, AIN_CH_NUM,
                                                    AENC_CH_NUM,
                                                    SAMPLE_RESOLUTION,
                                                    PCM_SAMPLE_RESOLUTION,
                                                    0U, AENC_FRAME_SIZE);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[2] AmbaAudio_BufSetup_Fc error ", 0U, 0U);
                }
                /* If User need to assign chain table need to enable AmbaAudio_BufSetup_FcCa() */
    //            AmbaAudio_BufSetup_FcCa(0U, 1U, SvcCaTableI);
            }
        } else if (AMBA_AUDIO_TYPE_AAC == g_AEncCoding){
            SetupInfo.AAC.PerceptualMode = 0U;
            if (1U == AudioFileWrite) {
                SetupInfo.AAC.BitstreamType = (UINT32)AACENC_ADTS;
            } else {
                SetupInfo.AAC.BitstreamType = (UINT32)AACENC_RAW;
            }
            SetupInfo.AAC.AencPlugInCs.SelfSize = (UINT32)sizeof(AMBA_AUDIO_AAC_CONFIG_s);
            SetupInfo.AAC.AencPlugInCs.Update = 0;
            SetupInfo.AAC.AencPlugInCs.MaxBsSize = 768U * (AENC_CH_NUM);

            //[AAC] Audio format converter and ready for volume usage
            {
                /*
                   Fc flow: Ain -> ABU -> AOUT
                */
                /*
                   Second parameter, 'AbuEfxDirection == 0' means AIN to ABU direction
                   I2S input is 32-bit, SrcRes = 32 means AIN to ABU is 32 bit, ABU process 16bit
                */
                RtVal = AmbaAudio_BufSetup_Fc(AencNum, 0U, AIN_CH_NUM, AIN_CH_NUM,
                                                    SAMPLE_RESOLUTION,
                                                    SAMPLE_RESOLUTION,
                                                    0U, AENC_FRAME_SIZE);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[3] AmbaAudio_BufSetup_Fc error ", 0U, 0U);
                }
                /* If User need to assign chain table need to enable AmbaAudio_BufSetup_FcCa() */
    //            AmbaAudio_BufSetup_FcCa(0U, 0U, SvcCaTableI);

                /*
                   Second parameter, 'AbuEfxDirection == 1' means ABU to AOUT direction
                   SrcRes = 16 means ABU to AOUT is 16 bit, AOUT to 16bit
                */
                RtVal = AmbaAudio_BufSetup_Fc(AencNum, 1U, AIN_CH_NUM,
                                                    AENC_CH_NUM,
                                                    SAMPLE_RESOLUTION,
                                                    AAC_SAMPLE_RESOLUTION,
                                                    0U, AENC_FRAME_SIZE);
                if (AIN2AENC_OK != RtVal) {
                    LOG_AUDIO_APP_NG("[4] AmbaAudio_BufSetup_Fc error ", 0U, 0U);
                }
                /* If User need to assign chain table need to enable AmbaAudio_BufSetup_FcCa() */
    //            AmbaAudio_BufSetup_FcCa(0U, 1U, SvcCaTableI);
            }
        } else {
            LOG_AUDIO_APP_NG("Wrong EncType info, %d", g_AEncCoding, 0U);
        }

        RtVal = AmbaAudioEnc_Setup(pAencHdlr[AencNum], &SetupInfo);
        LOG_AUDIO_APP_OK("AmbaAudioEnc_Setup, AencNum: %d, RtVal: %d ", AencNum, RtVal);

        EncInfo.AencInfo.pAinHdlr    = pAinHdlr;
        EncInfo.AencInfo.pAencHdlr   = pAencHdlr[AencNum];
        EncInfo.AencInfo.NumOfAenc   = NumOfAenc;
        EncInfo.AencInfo.AencTskIdx  = AencNum;
        EncInfo.AencInfo.pAencCbHdlr = &AencCbHdlr[AencNum];
        EncInfo.AencInfo.pAinCbHdlr  = &AinCbHdlr[AencNum];
        EncInfo.AencInfo.I2SChannel  = (UINT8)g_AinCfgArr[0].AinInfo.HwIndex;
        EncInfo.AencInfo.FadingTime  = 1U;
        RtVal = AmbaAudioEnc_Start(&EncInfo);
        LOG_AUDIO_APP_OK("AmbaAudioEnc_Start, RtVal : %d ", RtVal, 0U);

        RtVal = AmbaAudioInp_Start(&(EncInfo.AencInfo), pCbAinCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioInp_Start, RtVal : %d ", RtVal, 0U);
    }

    AmbaMisra_TouchUnused(&NumOfAin);
}

/**
* stop audio encode flow
* @param [in]  pCbAinCtrl audio input control
* @return none
*/
void AmbaAEncFlow_Stop(AMBA_AIN_CTRL_t pCbAinCtrl, UINT32 NumOfAin, UINT32 NumOfAenc)
{
    UINT32 RtVal;
    static AMBA_AUDIO_ENC_INFO_s AEncInfo GNU_SECTION_NOZEROINIT;

    for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
        AEncInfo.pAinHdlr    = pAinHdlr;
        AEncInfo.pAencHdlr   = pAencHdlr[AencNum];
        AEncInfo.NumOfAenc   = NumOfAenc;
        AEncInfo.AencTskIdx  = AencNum;
        AEncInfo.pAencCbHdlr = &AencCbHdlr[AencNum];
        AEncInfo.pAinCbHdlr  = &AinCbHdlr[AencNum];
        AEncInfo.I2SChannel  = (UINT8)g_AinCfgArr[0].AinInfo.HwIndex;
        AEncInfo.FadingTime  = 1U;
        RtVal = AmbaAudioEnc_Stop(&AEncInfo);
        LOG_AUDIO_APP_OK("AmbaAudioEnc_Stop, RtVal : %d ", RtVal, 0U);

        RtVal = AmbaAudioInp_Stop(&AEncInfo, pCbAinCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioInp_Stop, RtVal : %d ", RtVal, 0U);

        if (1U == AudioFileWrite) {
            RtVal = AmbaAudioEnc_Delete(pAencHdlr[AencNum], AencDataHdlrCB);
        } else {
            if (g_pCBDataRdy[AencNum] != NULL) {
                RtVal = AmbaAudioEnc_Delete(pAencHdlr[AencNum], g_pCBDataRdy[AencNum]);
            } else {
                LOG_AUDIO_APP_NG("Error! AmbaAEncFlow_Stop() g_pCBDataRdy[%d] == NULL", AencNum, 0U);
            }
        }

        LOG_AUDIO_APP_OK("AmbaAudioEnc_Delete, AencNum: %d, RtVal: %d ", AencNum, RtVal);
    }

    RtVal = AmbaAudioInp_Delete(pAinHdlr);
    LOG_AUDIO_APP_OK("AmbaAudioInp_Delete, RtVal : %d ", RtVal, 0U);

    for (UINT32 AencNum = 0U; AencNum < NumOfAenc; AencNum++) {
        if (1U == AudioFileWrite) {
            RtVal = AmbaAudio_MuxWaitLof(AencMuxIdx[AencNum]);
            LOG_AUDIO_APP_OK("AmbaAudio_MuxWaitLof, RtVal: %d", RtVal, 0U);
            RtVal = AmbaAudio_MuxDelete(AencMuxIdx[AencNum]);
            LOG_AUDIO_APP_OK("AmbaAudio_MuxDelete, AencNum: %d, RtVal: %d", AencNum, RtVal);
        }

        RtVal = AmbaAudio_EncBufDelete(AencNum);
        LOG_AUDIO_APP_OK("AmbaAudio_EncBufDelete, AencNum:%d, RtVal: %d", AencNum, RtVal);
    }
    AmbaMisra_TouchUnused(&NumOfAin);
}

/**
* AmbaAEncFlow_ParamCtrl
* @param [in]  ParamType
* @param [in]  pParam Param
*/
void AmbaAEncFlow_ParamCtrl(const UINT32 ParamType, const void *pParam)
{
    const UINT32 *pVal;

    switch (ParamType) {
    case AMBA_AENC_PMT_CODING:
        AmbaMisra_TypeCast(&pVal, &pParam);
        g_AEncCoding = *(pVal);
        break;
    default:
        /* do nothing */
        break;
    }
}
