/**
*  @file AmbaAudio.c
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
*  @details task create/destroy functions
*
*/

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaI2S.h"
#include "AmbaDMA_Def.h"
#include "AmbaWrap.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaAudioEfx.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudioBuf.h"
#include "AmbaPrint.h"
#include "AmbaVer.h"
#include "AmbaUtility.h"
#include "AmbaAudio_AacDec.h"
#include "AmbaAudio_AacEnc.h"
#include "AmbaCodecCom.h"


static UINT32 IsDecStop                      GNU_SECTION_NOZEROINIT;
static UINT32 AacEncLibWorkBuf[AMBA_AUDIO_MAX_BUF_NUM][800000U / 4U] GNU_SECTION_NOZEROINIT;
static UINT32 AacDecLibWorkBuf[800000U / 4U] GNU_SECTION_NOZEROINIT;

static AMBA_AIN_EVENT_HANDLER_f  AinStopDmaEntries[1]      GNU_SECTION_NOZEROINIT;
static AMBA_AENC_EVENT_HANDLER_f AEncOneFrameEntries[AMBA_AUDIO_MAX_BUF_NUM]    GNU_SECTION_NOZEROINIT;
static AMBA_AENC_EVENT_HANDLER_f AEncStopEntries[AMBA_AUDIO_MAX_BUF_NUM]        GNU_SECTION_NOZEROINIT;
static AMBA_AOUT_EVENT_HANDLER_f AoutStopDmaEntries[1]     GNU_SECTION_NOZEROINIT;
static AMBA_AOUT_EVENT_HANDLER_f AoutNoWorkIoEntries[1]    GNU_SECTION_NOZEROINIT;
static AMBA_ADEC_EVENT_HANDLER_f AdecStopEntries[1]        GNU_SECTION_NOZEROINIT;
static AMBA_ADEC_EVENT_HANDLER_f AdecUseOneFrameEntries[1] GNU_SECTION_NOZEROINIT;
static AMBA_ADEC_EVENT_HANDLER_f AdecGetReadyEntries[1]    GNU_SECTION_NOZEROINIT;

static AMBA_KAL_TASK_t AinTask GNU_SECTION_NOZEROINIT;
static AMBA_KAL_TASK_t AencTask[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_TASK_t AoutTask GNU_SECTION_NOZEROINIT;
static AMBA_KAL_TASK_t AdecTask GNU_SECTION_NOZEROINIT;

static AMBA_KAL_EVENT_FLAG_t SvcAudioInpFlag GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcAudioEncFlag[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcAudioDecFlag GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcAudioOupFlag GNU_SECTION_NOZEROINIT;

static AMBA_AENC_PLUGIN_ENC_CS_s  EncCs[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_ADEC_PLUGIN_DEC_CS_s  DecCs GNU_SECTION_NOZEROINIT;

static AMBA_AUDIO_PCM_CONFIG_s     PcmEncCfg[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_AUDIO_AAC_CONFIG_s     AacEncCfg[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;
static au_aacenc_config_t          AacEncConfig[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;

static AMBA_AUDIO_PCM_CONFIG_s PcmDecCfg GNU_SECTION_NOZEROINIT;
static AMBA_AUDIO_AAC_CONFIG_s AacDecCfg GNU_SECTION_NOZEROINIT;
static au_aacdec_config_t AacDecConfig GNU_SECTION_NOZEROINIT;

static ULONG AencHdlrIdx[AMBA_AUDIO_MAX_BUF_NUM] GNU_SECTION_NOZEROINIT;

static inline void LOG_AUDIO_FRW_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#endif
}

static inline void LOG_AUDIO_FRW_OK(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#endif
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static UINT8 AEncMute = 0;
UINT32 AmbaAudioEnc_SetMute(UINT8 Mute)
{
    AEncMute = Mute;
    return 0;
}
#endif

static UINT32 CheckAencIdx(const UINT32 *pHdlr)
{
    ULONG AencCastValue;
    UINT32 i, RtVal = 0U;

    AmbaMisra_TypeCast(&AencCastValue, &(pHdlr));
    for (i = 0; i < AMBA_AUDIO_MAX_BUF_NUM; i++) {
        if (AencHdlrIdx[i] == AencCastValue) {
            LOG_AUDIO_FRW_OK("CheckAencIdx()[%d]AencCastValue = %d",i, (UINT32)AencCastValue);
            RtVal = i;
            break;
        }
    }
    if (i == AMBA_AUDIO_MAX_BUF_NUM) {
        RtVal = 0U;
        AmbaPrint_PrintStr5("%s: No this AENC idx!", __func__, NULL, NULL, NULL, NULL);
    }
    return RtVal;
}

static void SvcAin2AencVariablesInit(void)
{
    if (AIN2AENC_OK != AmbaWrap_memset(&EncCs, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(AMBA_AENC_PLUGIN_ENC_CS_s)))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset EncCs fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(&PcmEncCfg, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(AMBA_AUDIO_PCM_CONFIG_s)))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset PcmEncCfg fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(&AacEncCfg, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(AMBA_AUDIO_AAC_CONFIG_s)))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AacEncCfg fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(&AacEncConfig, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(au_aacenc_config_t)))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AacEncConfig fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(AacEncLibWorkBuf, 0, AMBA_AUDIO_MAX_BUF_NUM * sizeof(UINT32) * (800000U / 4U))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AacEncLibWorkBuf fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(AinStopDmaEntries, 0, sizeof(AinStopDmaEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AinStopDmaEntries fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(AEncStopEntries, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(AEncStopEntries[1])))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AEncStopEntries fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(AEncOneFrameEntries, 0, (AMBA_AUDIO_MAX_BUF_NUM * sizeof(AEncOneFrameEntries[1])))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AEncOneFrameEntries fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memset(&AencHdlrIdx, 0, sizeof(AencHdlrIdx))) {
        LOG_AUDIO_FRW_NG("SvcAin2AencVariablesInit() AmbaWrap_memset AencHdlrIdx fail", 0U, 0U);
    }
}

static void* AmbaAudio_AinEntry(void* Entry)
{
    UINT32 Lof = 0U;
    const UINT32 *pHdlr;
    UINT32 RtVal, ActualFlags = 0U;

    AmbaMisra_TouchUnused(Entry);
    AmbaMisra_TypeCast(&pHdlr, &Entry);
    LOG_AUDIO_FRW_OK("Func: [AmbaAudio_AinEntry]",0U, 0U);

    RtVal = AmbaKAL_EventFlagGet(&SvcAudioInpFlag, AIN_DMA_START_FLG, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);


    while((Lof == 0U) || (RtVal == (UINT32)AIN_OK)) {
        RtVal = AmbaAIN_ProcDMA(pHdlr, &Lof);
    }

    return NULL;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
int BeepPlayEos = 0;
#endif
static UINT32 SvcAudio_AoutNoWorkIoCB(const void *pEventData)
{
    UINT32 RtVal;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    BeepPlayEos = 1;
#endif
    if (pEventData == NULL) {
        RtVal = (UINT32)ADEC2AOUT_ERR_0001;
    } else {
        RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_NO_WORK_IO_FLG);
        if (KAL_ERR_NONE == RtVal) {
            LOG_AUDIO_FRW_OK("AOUT: SvcAudio_AoutNoWorkIoCB, Set SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AOUT: SvcAudio_AoutNoWorkIoCB, Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
        }
    }
    return RtVal;
}

static void* AmbaAudio_AencEntry(void* Entry)
{
    UINT32                 *pHdlr;
    UINT32                 RtVal = AENC_OK, Err;
    const AMBA_AENC_HDLR   *pAencHdlr;

    AmbaMisra_TouchUnused(Entry);
    AmbaMisra_TypeCast(&pHdlr, &Entry);
    LOG_AUDIO_FRW_OK("Func: [AmbaAudio_AencEntry]", 0U, 0U);
    while(RtVal == AENC_OK) {
        Err = AmbaAENC_ProcEnc(pHdlr);
        if (AENC_OK != Err) {
            LOG_AUDIO_FRW_OK("AmbaAENC_ProcEnc failed, RtVal: %d", Err, 0U);
            RtVal = Err;
        }
        AmbaMisra_TypeCast(&pAencHdlr, &pHdlr);
        if (pAencHdlr->Stop == 1U) {
            break;
        }
        //LOG_AUDIO_FRW_OK("[aenc] RtVal: %d", RtVal, 0U);
    }

    return NULL;
}

static UINT32 AmbaAudio_AinStopDmaCB(const void *pEventData)
{
    UINT32 ReVal = (UINT32)AIN_OK;

    if (pEventData == NULL) {
        ReVal = AIN2AENC_ERR_0000;
    }
    LOG_AUDIO_FRW_OK("Func: [AmbaAudio_AinStopDmaCB], ReVal: %d", ReVal, 0U);
    ReVal = AmbaKAL_EventFlagSet(&SvcAudioInpFlag, AIN_DMA_STOP_FLG);
    if ((UINT32)AIN_OK != ReVal) {
        LOG_AUDIO_FRW_NG("AmbaAudio_AinStopDmaCB error!! ReVal: %d", ReVal, 0U);
    }
    LOG_AUDIO_FRW_OK("Amba_AinStopDmaCB end, ReVal: %d", ReVal, 0U);
    return ReVal;
}

static UINT32 AmbaAudio_AencStopCB(const void *pEventData)
{
    UINT32 ReVal = (UINT32)AENC_OK;
    const UINT32 *pAencHdlr;
    UINT32 AencIndex;

    if (pEventData == NULL) {
        ReVal = AIN2AENC_ERR_0000;
    } else {
        LOG_AUDIO_FRW_OK("Amba_AencStopCB start, ReVal: %d", ReVal, 0U);
        if (AIN2AENC_OK != AmbaWrap_memcpy(&pAencHdlr, &pEventData, sizeof(pAencHdlr))) {
            LOG_AUDIO_FRW_NG("AmbaAudio_AencStopCB() AmbaWrap_memcpy pAencHdlr fail", 0U, 0U);
        }
        AencIndex = CheckAencIdx(pAencHdlr);
        ReVal = AmbaKAL_EventFlagSet(&SvcAudioEncFlag[AencIndex], AENC_STOP_FLG);
        if (AENC_OK != ReVal) {
            LOG_AUDIO_FRW_NG("AmbaAudio_AencStopCB[%d] error!! RtVal: %d", AencIndex, ReVal);
        }
        LOG_AUDIO_FRW_OK("Amba_AencStopCB[%d] end, ReVal: %d", AencIndex, ReVal);
    }

    return ReVal;
}

static UINT32 AmbaAudio_EncPcmEncode(AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    const AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg;
    UINT32 Endian;
    UINT8 *pTrans = NULL;
    UINT32 Idx, J, K;
    UINT32 Resolution;
    UINT32 *pPcmSrc;
    UINT8 *pPcmDst;
    UINT32 RtVal = AIN2AENC_OK;

    AmbaMisra_TypeCast(&pPcmCfg, &(pPlugInCs->pSelf));
    Endian = pPcmCfg->DataFormat;
    Resolution = (pPcmCfg->BitsPerSample / 8U);
    pPcmSrc = (UINT32 *)pPlugInCs->pSrc;
    pPcmDst = (UINT8 *)pPlugInCs->pDst;

    if (Endian == 0U) {
        for (Idx = 0U; Idx < pPlugInCs->FrameSize; Idx++) {
            for (J = 0U; J < pPlugInCs->ChNum; J++ ) {
                pTrans = (UINT8 *)pPcmSrc;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                pPcmSrc++;
#endif
                /*32bit transfers to all kind of the resolutions  */
                for (K = 4U - Resolution; K < 4U; K++) {
                    *pPcmDst = pTrans[K];
                    pPcmDst++;
                }
            }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            pPcmSrc += 2;//add
#endif
        }
        pPlugInCs->EncodedBytes = pPcmCfg->FrameSize * pPlugInCs->ChNum * Resolution;
    } else if (Endian == 1U) {
        for(Idx = 0U; Idx < pPcmCfg->FrameSize; Idx++) {
            for (J = 0U; J < pPlugInCs->ChNum; J++ ) {
                pTrans = (UINT8 *) pPcmSrc;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                pPcmSrc++;
#endif
                for (K = 3U; K >= (4U - Resolution); K--) {
                    *pPcmDst = pTrans[K];
                    pPcmDst++;
                }
            }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            pPcmSrc += 2;//add
#endif
        }
        pPlugInCs->EncodedBytes = pPcmCfg->FrameSize * pPlugInCs->ChNum * Resolution;
    } else {
        RtVal = AIN2AENC_ERR_0000;
        pPlugInCs->EncodedBytes = 0U;
    }

    //LOG_AUDIO_FRW_OK("AmbaAudio_EncPcmEncode: %d, %d, %d %d", pPcmCfg->FrameSize, Resolution, pPlugInCs->ChNum, pPlugInCs->EncodedBytes, 0U);
    AmbaMisra_TouchUnused(pTrans);
    AmbaMisra_TouchUnused(pPcmSrc);
    return RtVal;
}

static UINT32 AmbaAudio_EncPcmSetup(const AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg;/* = (AMBA_AUDIO_PCM_CONFIG_s *)pPlugInCs->pSelf;*/

    AmbaMisra_TypeCast(&pPcmCfg, &(pPlugInCs->pSelf));
    //TBD Initial it at AmbaAudio_EncPlugInPcmLibInstall() now
    pPcmCfg->BitsPerSample = PcmEncCfg[pPcmCfg->AencIndex].BitsPerSample;
    pPcmCfg->DataFormat = PcmEncCfg[pPcmCfg->AencIndex].DataFormat;
    pPcmCfg->FrameSize = PcmEncCfg[pPcmCfg->AencIndex].FrameSize;

    return AIN2AENC_OK;
}

static UINT32 AmbaAudio_EncPlugInPcmLibInstall(const UINT32 *pAencHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    UINT32  AencIndex;
    AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs;
    AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg;

    AencIndex = CheckAencIdx(pAencHdlr);

    /* Init PCM setting */
    pPcmCfg = &PcmEncCfg[AencIndex];
    pPcmCfg->AencIndex     = AencIndex;
    pPcmCfg->BitsPerSample = pSetupInfo->PCM.BitsPerSample; //Means encode PCM 16 bits or 32 bits
    pPcmCfg->DataFormat    = pSetupInfo->PCM.DataFormat;
    pPcmCfg->FrameSize     = pSetupInfo->PCM.FrameSize;

    pPlugInCs = &EncCs[AencIndex];
    pPlugInCs->pSelf = pPcmCfg;
    pPlugInCs->FrameSize = pSetupInfo->PCM.AencPlugInCs.FrameSize;
    pPlugInCs->ChNum = pSetupInfo->PCM.AencPlugInCs.ChNum;
    pPlugInCs->Resolution = pSetupInfo->PCM.AencPlugInCs.Resolution;
    pPlugInCs->SelfSize = pSetupInfo->PCM.AencPlugInCs.SelfSize;
    pPlugInCs->Update = pSetupInfo->PCM.AencPlugInCs.Update;
    pPlugInCs->MaxBsSize = pSetupInfo->PCM.AencPlugInCs.MaxBsSize;
    pPlugInCs->pSetUp_f = AmbaAudio_EncPcmSetup;
    pPlugInCs->pProc_f = AmbaAudio_EncPcmEncode;
    return AmbaAENC_InstallPlugInEnc(pAencHdlr, pPlugInCs);
}

static UINT32 AmbaAudio_EncAacEncFrame(AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    const AMBA_AUDIO_AAC_CONFIG_s *pAacSetUp;/* = (SVC_AUDIO_AAC_CONFIG_s *)pPlugInCs->pSelf; */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    //UINT32  Loop;
    //const UINT32  *pSrc;
    UINT16  *pDst;

    /* Convert 32bit input data to 16bit for AAC encoding */
    //pSrc = pPlugInCs->pSrc;
    AmbaMisra_TypeCast(&pDst, &(pPlugInCs->pSrc)); /*pDst = (UINT16 *)pPlugInCs->pSrc;*/
    if (AEncMute) {
        *pDst = 0;
    }
#if 0
    for (Loop = 0; Loop < (pPlugInCs->FrameSize * pPlugInCs->ChNum); Loop++) {
        if (AEncMute) {
            *pDst = 0;
        } else {
            *pDst = (UINT16)((*pSrc >> 16) & 0xFFFFUL);
        }
        pSrc++;
        pDst++;
    }
#endif
#endif
    AmbaMisra_TypeCast(&pAacSetUp, &(pPlugInCs->pSelf));

    AacEncConfig[pAacSetUp->AencIndex].enc_wptr = pPlugInCs->pDst;
    AmbaMisra_TypeCast(&(AacEncConfig[pAacSetUp->AencIndex].enc_rptr), &(pPlugInCs->pSrc)); /*AacEncConfig[pAacSetUp->AencIndex].enc_rptr = (INT32 *)pPlugInCs->pSrc;*/
    aacenc_encode(&AacEncConfig[pAacSetUp->AencIndex]);
    if (AacEncConfig[pAacSetUp->AencIndex].ErrorStatus != OK) {
        LOG_AUDIO_FRW_NG("AmbaAudio_AacEncode error status = 0x%x", AacEncConfig[pAacSetUp->AencIndex].ErrorStatus, 0U);
    }
    pPlugInCs->EncodedBytes = (AacEncConfig[pAacSetUp->AencIndex].nBitsInRawDataBlock + 7U) >> 3U;
    /*LOG_AUDIO_FRW_OK("AmbaAudio_AacEncode = %d, 0x%x 0x%x 0x%x 0x%x",
                         pPlugInCs->EncodedBytes,
                         (UINT32)*AacEncConfig[pAacSetUp->AencIndex].enc_wptr,
                         (UINT32)*(AacEncConfig[pAacSetUp->AencIndex].enc_wptr + 1),
                         (UINT32)*(AacEncConfig[pAacSetUp->AencIndex].enc_wptr + 2),
                         (UINT32)*(AacEncConfig[pAacSetUp->AencIndex].enc_wptr + 3));*/

    return OK;
}

static UINT32 AmbaAudio_AacEncCheckBitRate(UINT32 SampleRate, UINT32 ChNum, UINT32 AudioBitRate, UINT32 EncLevel)
{
    UINT32 MinBitRate, MaxBitRate;
    UINT32 RtVal = OK;
    UINT32 BitRate = AudioBitRate;

    AmbaPrint_PrintUInt5("AAC encode original setup: EncLevel=%d, SampleRate=%d, ChNum=%d, BitRate=%d",
              EncLevel, SampleRate, ChNum, BitRate, 0U);
    if (EncLevel == 0U /*enc_mode == AACPLAIN*/) {
        if ((SampleRate == 32000U) || (SampleRate == 44100U) || (SampleRate == 48000U)) {
            MinBitRate = (ChNum * 16000U);
            MaxBitRate = (ChNum * 160000U); /* maximum doesn't work: (numChannels * sampleRate * 6144) / 1024; */
        } else if ((SampleRate == 16000U) || (SampleRate == 22050U) || (SampleRate == 24000U)) {
            MinBitRate = (ChNum * 16000U);
            MaxBitRate = (ChNum * 80000U);
        } else if ((SampleRate == 8000U) || (SampleRate == 11025U) || (SampleRate == 12000U)) {
            MinBitRate = (ChNum * 8000U);
            MaxBitRate = (ChNum * 40000U);
        } else {
            MinBitRate = 0U;
            MaxBitRate = 0U;
            LOG_AUDIO_FRW_NG("AACPLAIN encode unsupported this sample rate %d", SampleRate, 0U);
            RtVal = AIN2AENC_ERR_0000;
        }
    } else if (EncLevel == 1U /*enc_mode == AACPLUS*/) {
        MinBitRate = (ChNum * 14000U);
        MaxBitRate = (ChNum * 64000U); /* maximum is restricted by SBR: (numChannels * sampleRate * 6144) / 2048; */
    } else if (EncLevel == 2U /*enc_mode == AACPLUS_PS*/) {
        MinBitRate = 16000U;
        MaxBitRate = 64000U; /* maximum is restricted by SBR: (sampleRate  * 6144) / 2048; */
    } else {
        LOG_AUDIO_FRW_NG("AAC encode unsupported this mode %d", EncLevel, 0U);
        RtVal = AIN2AENC_ERR_0000;
        MinBitRate = 0U;
        MaxBitRate = 0U;
    }

    AmbaPrint_PrintUInt5("AAC encode bit rate Max=%d, min=%d", MaxBitRate, MinBitRate, 0U, 0U, 0U);
    if (RtVal == OK) {
        if (BitRate < MinBitRate) {
            LOG_AUDIO_FRW_NG("(Original BitRate < MinBitRate) AAC encode unsupported this BitRate=%d. Automatically fix/change to this BitRate=%d",
                      BitRate, MinBitRate);
            BitRate = MinBitRate;
        } else if (BitRate > MaxBitRate) {
            LOG_AUDIO_FRW_NG("(Original BitRate > MaxBitRate) AAC encode unsupported this BitRate=%d. Automatically fix/change to this BitRate=%d",
                      BitRate, MaxBitRate);
            BitRate = MaxBitRate;
        } else {
            AmbaPrint_PrintUInt5("AAC encode bit rate : EncLevel=%d, SampleRate=%d, ChNum=%d, BitRate=%d",
                      EncLevel, SampleRate, ChNum, BitRate, 0U);
        }
        RtVal = BitRate;
    } else {
        RtVal = 0xFFFFFFFFU;
    }

    return RtVal;
}

static UINT32 AmbaAudio_EncAacEncSetup(const AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs)
{
    const AMBA_AUDIO_AAC_CONFIG_s *pAacSetUp;/* = (SVC_AUDIO_AAC_CONFIG_s *)pPlugInCs->pSelf; */
    UINT32 BitRate, RetVal = OK;

    AmbaMisra_TypeCast(&pAacSetUp, &(pPlugInCs->pSelf));

    AacEncConfig[pAacSetUp->AencIndex].sample_freq = pAacSetUp->SampleFreq;
    AacEncConfig[pAacSetUp->AencIndex].Src_numCh = pPlugInCs->ChNum;
    BitRate = AmbaAudio_AacEncCheckBitRate(pAacSetUp->SampleFreq, pPlugInCs->ChNum, pAacSetUp->Bitrate, 0U/* AACPLAIN */);
    if (BitRate != 0xFFFFFFFFU) {
        AacEncConfig[pAacSetUp->AencIndex].bitRate = BitRate;
        AacEncConfig[pAacSetUp->AencIndex].quantizerQuality = 1U;
        AacEncConfig[pAacSetUp->AencIndex].tns = 1U;
        AacEncConfig[pAacSetUp->AencIndex].crc = 0U;
        AacEncConfig[pAacSetUp->AencIndex].pns = 0U;
        AacEncConfig[pAacSetUp->AencIndex].original_copy = 0U;
        AacEncConfig[pAacSetUp->AencIndex].copyright_identification_bit = 0U;
        AacEncConfig[pAacSetUp->AencIndex].copyright_identification_start = 0U;
        AacEncConfig[pAacSetUp->AencIndex].ffType = pAacSetUp->BitstreamType;
        AacEncConfig[pAacSetUp->AencIndex].enc_mode = 0U; /* AACPLAIN */
        AacEncConfig[pAacSetUp->AencIndex].perceptual_mode = pAacSetUp->PerceptualMode;
        AacEncConfig[pAacSetUp->AencIndex].channelMode = 0U;
        AacEncConfig[pAacSetUp->AencIndex].sendSbrHeader = 0U;
        AacEncConfig[pAacSetUp->AencIndex].ancillary_buf = NULL;
        AacEncConfig[pAacSetUp->AencIndex].ancillary_size = 0U;

        AacEncConfig[pAacSetUp->AencIndex].codec_lib_mem_adr = AacEncLibWorkBuf[pAacSetUp->AencIndex];
        AacEncConfig[pAacSetUp->AencIndex].codec_lib_mem_size = (UINT32)sizeof(AacEncLibWorkBuf[pAacSetUp->AencIndex]);
        if (AIN2AENC_OK != AmbaWrap_memset(AacEncConfig[pAacSetUp->AencIndex].codec_lib_mem_adr, 0, AacEncConfig[pAacSetUp->AencIndex].codec_lib_mem_size)) {
            LOG_AUDIO_FRW_NG("AmbaAudio_EncAacEncSetup() AmbaWrap_memset codec_lib_mem_adr fail", 0U, 0U);
        }

        aacenc_setup(&AacEncConfig[pAacSetUp->AencIndex]);
        if (AacEncConfig[pAacSetUp->AencIndex].ErrorStatus != OK) {
            LOG_AUDIO_FRW_NG("AENC: AmbaAudio_AacSetup error status = 0x%x", AacEncConfig[pAacSetUp->AencIndex].ErrorStatus, 0U);
        }
        aacenc_open(&AacEncConfig[pAacSetUp->AencIndex]);
        if (AacEncConfig[pAacSetUp->AencIndex].ErrorStatus != OK) {
            LOG_AUDIO_FRW_NG("AENC: AmbaAudio_AacOpen error status = 0x%x", AacEncConfig[pAacSetUp->AencIndex].ErrorStatus, 0U);
        }

        LOG_AUDIO_FRW_OK("AENC: coreSampleRate: %d", AacEncConfig[pAacSetUp->AencIndex].coreSampleRate, 0U);
    } else {
        RetVal = AIN2AENC_ERR_0000;
    }

    return RetVal;
}

static UINT32 AmbaAudio_EncPlugInAacLibInstall(const UINT32 *pAencHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    UINT32  AencIndex;
    AMBA_AENC_PLUGIN_ENC_CS_s *pPlugInCs;
    AMBA_AUDIO_AAC_CONFIG_s *pAacEncCfg;

    AencIndex = CheckAencIdx(pAencHdlr);

    /* Init AAC setting */
    pAacEncCfg = &AacEncCfg[AencIndex];
    pAacEncCfg->AencIndex      = AencIndex;
    pAacEncCfg->Bitrate        = pSetupInfo->AAC.Bitrate;
    pAacEncCfg->BitstreamType  = pSetupInfo->AAC.BitstreamType;
    pAacEncCfg->SampleFreq     = pSetupInfo->AAC.SampleFreq;
    pAacEncCfg->PerceptualMode = pSetupInfo->AAC.PerceptualMode;

    pPlugInCs = &EncCs[AencIndex];
    pPlugInCs->pSelf = pAacEncCfg;
    pPlugInCs->FrameSize = pSetupInfo->AAC.AencPlugInCs.FrameSize;
    pPlugInCs->ChNum = pSetupInfo->AAC.AencPlugInCs.ChNum;
    pPlugInCs->Resolution = pSetupInfo->AAC.AencPlugInCs.Resolution;
    pPlugInCs->SelfSize = pSetupInfo->AAC.AencPlugInCs.SelfSize;
    pPlugInCs->Update = pSetupInfo->AAC.AencPlugInCs.Update;
    pPlugInCs->MaxBsSize = pSetupInfo->AAC.AencPlugInCs.MaxBsSize;
    pPlugInCs->pSetUp_f = AmbaAudio_EncAacEncSetup;
    pPlugInCs->pProc_f = AmbaAudio_EncAacEncFrame;
    return AmbaAENC_InstallPlugInEnc(pAencHdlr, pPlugInCs);
}

static UINT32 AmbaAudio_DecAacSetup(const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    au_aacdec_config_t *pAacDecConfig = &AacDecConfig;
    const AMBA_AUDIO_AAC_CONFIG_s *pAacDecCfg;

    if (AIN2AENC_OK != AmbaWrap_memcpy(&pAacDecCfg, &(pPlugInCs->pSelf), sizeof(AMBA_AUDIO_AAC_CONFIG_s *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecAacSetup() AmbaWrap_memcpy pAacDecCfg fail", 0U, 0U);
    }
    pAacDecConfig->bsFormat           = (INT32)pAacDecCfg->BitstreamType;
    pAacDecConfig->sample_freq        = 48000; /*!< external sampling rate for raw decoding */
    pAacDecConfig->bDownSample        = 0; /*!< external bitrate for loading the input buffer */
    pAacDecConfig->bBitstreamDownMix  = 0;         /*!< 0: stereo to mono downmix off  1: stereo to mono downmix on */
    pAacDecConfig->codec_lib_mem_addr = AacDecLibWorkBuf;
    pAacDecConfig->codec_lib_mem_size = (UINT32)sizeof(AacDecLibWorkBuf);

    aacdec_setup(pAacDecConfig);
    if (pAacDecConfig->ErrorStatus != 0) {
        LOG_AUDIO_FRW_OK("aacdec_setup error status = 0x%x", (UINT32)pAacDecConfig->ErrorStatus, 0U);
    }
    aacdec_open(pAacDecConfig);
    if (pAacDecConfig->ErrorStatus != 0) {
        LOG_AUDIO_FRW_OK("aacdec_open error status = 0x%x", (UINT32)pAacDecConfig->ErrorStatus, 0U);
    }

    return OK;
}

static UINT32 AmbaAudio_DecAacFrame(AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    au_aacdec_config_t *pAacDecConfig = &AacDecConfig;
    const UINT16  *pData;
    UINT32  *pWptr;
    UINT32  LoopU;
    UINT32 Temp = 0U;

    if (AIN2AENC_OK != AmbaWrap_memcpy(&(pAacDecConfig->dec_wptr), &(pPlugInCs->pDst), sizeof(INT32 *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecAacFrame() AmbaWrap_memcpy dec_wptr fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memcpy(&(pAacDecConfig->dec_rptr), &(pPlugInCs->pSrc), sizeof(UINT8 *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecAacFrame() AmbaWrap_memcpy dec_rptr fail", 0U, 0U);
    }

    aacdec_decode(pAacDecConfig);
    if (pAacDecConfig->ErrorStatus != 0) {
        LOG_AUDIO_FRW_OK("aacdec_decode error status = 0x%x", (UINT32)pAacDecConfig->ErrorStatus, 0U);
    }

    if (pAacDecConfig->has_dec_out != 0U) {
        pPlugInCs->FrameSize = 1024U;
    } else {
        pPlugInCs->FrameSize = 0;
    }

    /* Change Resolution from 16bit to 32bit */
    if (AIN2AENC_OK != AmbaWrap_memcpy(&(pData), &(pPlugInCs->pDst), sizeof(UINT16 *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecAacFrame() AmbaWrap_memcpy pData fail", 0U, 0U);
    }
    if (AIN2AENC_OK != AmbaWrap_memcpy(&(pWptr), &(pPlugInCs->pDst), sizeof(UINT32 *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecAacFrame() AmbaWrap_memcpy pWptr fail", 0U, 0U);
    }

    for (LoopU = (pPlugInCs->FrameSize * pPlugInCs->ChNum); LoopU > 0U ; LoopU--) {
        Temp = (UINT32)pData[LoopU - 1U];
        Temp = Temp << 16U;
        pWptr[LoopU - 1U] = Temp;
    }
    pPlugInCs->Resolution    = 32U;
    pPlugInCs->ConsumedBytes = pAacDecConfig->consumedByte;

    return OK;
}

static UINT32 AmbaAudio_DecPcmSetup(const AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg;
    UINT32 RetVal = (UINT32)ADEC_OK;

    if (AIN2AENC_OK != AmbaWrap_memcpy(&pPcmCfg, &(pPlugInCs->pSelf), sizeof(AMBA_AUDIO_PCM_CONFIG_s *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecPcmSetup() AmbaWrap_memcpy pPcmCfg fail", 0U, 0U);
    }
    pPcmCfg->BitsPerSample = PcmDecCfg.BitsPerSample;
    pPcmCfg->DataFormat = PcmDecCfg.DataFormat;
    pPcmCfg->FrameSize = PcmDecCfg.FrameSize;

    return RetVal;
}

static UINT32 AmbaAudio_DecPcmFrame(AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs)
{
    const AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg;
    UINT32 Endian;
    UINT32 Idx, J, K, L, M, Real, Remain, Divison, Resolution;
    UINT8 *pBitsHead, *pTrans;
    UINT32 *pBufPcmHead;
    UINT32 Transfer;

    if (AIN2AENC_OK != AmbaWrap_memcpy(&pPcmCfg, &(pPlugInCs->pSelf), sizeof(AMBA_AUDIO_PCM_CONFIG_s *))) {
        LOG_AUDIO_FRW_NG("AmbaAudio_DecPcmFrame() AmbaWrap_memcpy pPcmCfg fail", 0U, 0U);
    }
    Endian     = pPcmCfg->DataFormat;
    Resolution = pPcmCfg->BitsPerSample >> 3;

    if ((pPlugInCs->FrameSize * Resolution * pPlugInCs->ChNum) > pPlugInCs->PureAudioBufCurSize) {
        Remain = pPlugInCs->PureAudioBufCurSize;
        Divison = Resolution * pPlugInCs->ChNum;
        Real = 0;
        while(Remain > 0U) {
            Remain -= Divison;
            Real++;
        }
    } else {
        Real = pPlugInCs->FrameSize;
    }

    if (Real == 0U) {
        pPlugInCs->FrameSize = 0;
        pPlugInCs->ConsumedBytes= pPlugInCs->PureAudioBufCurSize;
        LOG_AUDIO_FRW_OK("Pcmdec inter_buf empty %d", pPlugInCs->PureAudioBufCurSize, 0U);
    } else {
        pBitsHead = pPlugInCs->pSrc;
        pBufPcmHead = (UINT32 *)pPlugInCs->pDst;
        /*All kind of the resolutions transfer to 32bit and shift left */
        if (Endian == 0U) {
            for (Idx = 0; Idx < Real; Idx++) {
                for (J = 0; J < pPlugInCs->ChNum; J++) {
                    pTrans = (UINT8 *)&Transfer;
                    for (K = 0; K < (4U - Resolution); K++) {
                        pTrans[K] = 0;
                    }
                    for (L = K; L < 4U; L++) {
                        pTrans[L] = *pBitsHead;
                        pBitsHead = &pBitsHead[1];
                    }
                    *pBufPcmHead = Transfer;
                    pBufPcmHead = &pBufPcmHead[1];
                }
            }
        } else if (Endian == 1U) {
            for (Idx = 0; Idx < Real; Idx++) {
                for (J = 0; J < pPlugInCs->ChNum; J++) {
                    pTrans = (UINT8 *)&Transfer;
                    for (K = 0; K < (4U - Resolution); K++) {
                        pTrans[K] = 0;
                    }
                    for (M = 3; M >= K; M--) {
                        pTrans[M] = *pBitsHead;
                        pBitsHead = &pBitsHead[1];
                    }
                    *pBufPcmHead = Transfer;
                    pBufPcmHead = &pBufPcmHead[1];
                }
            }

        } else {
            LOG_AUDIO_FRW_OK("PLEASE SET PCMCODEC BITSTREAM FORMAT LITTLE ENDIAN(0) or BIG ENDIAN(1)", 0U, 0U);
        }
        pPlugInCs->FrameSize = Real;
        pPlugInCs->ConsumedBytes= Real * Resolution * pPlugInCs->ChNum;
    }
    return 0U;
}

static UINT32 AmbaAudio_AdecReadyCB(void *pEventData)
{
    UINT32 RtVal = (UINT32)AOUT_OK;

    if (pEventData != NULL) {
        RtVal = AmbaKAL_EventFlagSet(&SvcAudioDecFlag, ADEC_DATA_READY_FLG);
        if (KAL_ERR_NONE == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: AdecReadyCB success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: AdecReadyCB error!! RtVal: %d", RtVal, 0U);
        }
    }

    AmbaMisra_TouchUnused(pEventData);
    return RtVal;
}

static UINT32 AmbaAudio_AdecStopCB(void *pEventData)
{
    UINT32 RtVal = (UINT32)ADEC_OK;

    /*Fix CC error*/
    if (pEventData != NULL) {
        RtVal = AmbaKAL_EventFlagSet(&SvcAudioDecFlag, ADEC_STOP_FLG);
        if (KAL_ERR_NONE == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: AmbaAudio_AdecStopCB success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: AmbaAudio_AdecStopCB error!! RtVal: %d", RtVal, 0U);
        }
    }

    /* Will not do AmbaADEC_ProcDec() anymore. */
    IsDecStop = 1;

    AmbaMisra_TouchUnused(pEventData);
    return RtVal;
}

static UINT32 AmbaAudio_AoutStopDmaCB(const void *pEventData)
{
    UINT32 RtVal = ADEC2AOUT_OK;

    /*Fix CC error*/
    if (pEventData == NULL) {
        RtVal = (UINT32)ADEC2AOUT_ERR_0001;
    } else {
//        RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_STOP_FLG);
//        if (KAL_ERR_NONE == RtVal) {
//            LOG_AUDIO_FRW_OK("AOUT: AmbaAudio_AoutStopDmaCB, Set SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
//        } else {
//            LOG_AUDIO_FRW_NG("AOUT: AmbaAudio_AoutStopDmaCB, Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
//        }
    }

    return RtVal;
}

static void* AmbaAudio_AdecEntry(void* Entry)
{
    UINT32 RetVal = (UINT32)ADEC_OK, ActualFlags = 0U;
    const UINT32 *pHdlr;

    AmbaMisra_TouchUnused(Entry);
    AmbaMisra_TypeCast(&pHdlr, &Entry);

    RetVal = AmbaKAL_EventFlagGet(&SvcAudioDecFlag, ADEC_START_FLG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);

    LOG_AUDIO_FRW_OK("AmbaAudio_AdecEntry start ", 0U, 0U);

    while(RetVal == (UINT32)ADEC_OK) {
        UINT32 RtVal;
        if (0U == IsDecStop) {
            RetVal = AmbaADEC_ProcDec(pHdlr, 16);
        } else {
            //Do nothing
        }
        //This Task sleep is necessary
        //Audio driver team: Decoder is data driven, so there should be some delay to the demuxer.
        RtVal = AmbaKAL_TaskSleep(15);
        if (ADEC_OK != RtVal) {
            LOG_AUDIO_FRW_OK("AmbaAudio_AdecEntry AmbaKAL_TaskSleep error ", 0U, 0U);
        }
    }

    return NULL;
}

static void* AmbaAudio_AoutEntry(void* Entry)
{
    UINT32 Lof = 0U, Err;
    UINT32 RtVal = AOUT_OK, ActualFlags;
    const UINT32 *pHdlr;
    UINT32 SendFlag = 0U;

    AmbaMisra_TouchUnused(Entry);
    AmbaMisra_TypeCast(&pHdlr, &Entry);

    while(RtVal == AOUT_OK) {

        Err = AmbaKAL_EventFlagGet(&SvcAudioOupFlag, AOUT_RUNNING_FLG,
                                    AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (KAL_ERR_NONE != Err) {
            LOG_AUDIO_FRW_NG("AOUT: Get AOUT_RUNNING_FLG timeout!! RtVal: %d", RtVal, 0U);
        }

        RtVal = AmbaKAL_EventFlagClear(&SvcAudioOupFlag, AOUT_PROC_DONE);
        if (KAL_ERR_NONE != RtVal) {
            LOG_AUDIO_FRW_NG("AOUT: AmbaAudio_AoutEntry, CLEAR SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
        }

        if (Lof == 0U) {
            Err = AmbaAOUT_ProcDMA(pHdlr, &Lof);
            if (Err != AOUT_OK) {
                LOG_AUDIO_FRW_NG("AmbaAOUT_ProcDMA error %u!!", Err, 0U);
            }
        } else {
            if (SendFlag == 0U) {
                RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_STOP_FLG);
                if (KAL_ERR_NONE == RtVal) {
                    LOG_AUDIO_FRW_OK("AmbaAudio_AoutEntry(), Set SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
                } else {
                    LOG_AUDIO_FRW_NG("AmbaAudio_AoutEntry(), Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
                }
                SendFlag = 1U;
            } else {
                RtVal = AmbaKAL_TaskSleep(1000U);
                if (ADEC_OK != RtVal) {
                    LOG_AUDIO_FRW_OK("AmbaAudio_AoutEntry AmbaKAL_TaskSleep error ", 0U, 0U);
                }
            }
        }

        RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_PROC_DONE);
        if (KAL_ERR_NONE != RtVal) {
            LOG_AUDIO_FRW_NG("AOUT: AmbaAudio_AoutEntry, Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
        }

        // if ((RtVal == AOUT_OK) && (Lof == 1U)) {
        //     Err = AmbaAOUT_Stop(pHdlr);
        //     if (Err != AOUT_OK) {
        //         LOG_AUDIO_FRW_NG("AmbaAOUT_Stop error %u!!", Err, 0U);
        //         RtVal = Err;
        //     }
        // }
    }

    return NULL;
}

static void SvcAdec2AoutVariablesInit(void)
{
    IsDecStop = 0;
    if (AIN2AENC_OK != AmbaWrap_memset(AoutStopDmaEntries, 0, sizeof(AoutStopDmaEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AoutStopDmaEntries fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(AoutNoWorkIoEntries, 0, sizeof(AoutNoWorkIoEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AoutNoWorkIoEntries fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(AdecStopEntries, 0, sizeof(AdecStopEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AdecStopEntries fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(AdecUseOneFrameEntries, 0, sizeof(AdecUseOneFrameEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AdecUseOneFrameEntries fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(AdecGetReadyEntries, 0, sizeof(AdecGetReadyEntries[1]))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AdecGetReadyEntries fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(&DecCs, 0, sizeof(DecCs))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset DecCs fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(&PcmDecCfg, 0, sizeof(PcmDecCfg))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset PcmDecCfg fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(&AacDecCfg, 0, sizeof(AacDecCfg))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AacDecCfg fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(&AacDecConfig, 0, sizeof(AacDecConfig))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AacDecConfig fail", 0U, 0U);
    }

    if (AIN2AENC_OK != AmbaWrap_memset(AacDecLibWorkBuf, 0, sizeof(UINT32) * (800000U / 4U))) {
        LOG_AUDIO_FRW_NG("SvcAdec2AoutVariablesInit() AmbaWrap_memset AacDecLibWorkBuf fail", 0U, 0U);
    }
}

static UINT32 AmbaAudio_DecPlugInAacLibInstall(const UINT32 *pAdecHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs = &DecCs;
    AMBA_AUDIO_AAC_CONFIG_s *pAacDecCfg = &AacDecCfg;


    AacDecCfg.BitstreamType = pSetupInfo->AAC.BitstreamType;

    pPlugInCs->pSelf = pAacDecCfg;
    pPlugInCs->FrameSize = pSetupInfo->AAC.AdecPlugInCs.FrameSize;
    pPlugInCs->ChNum = pSetupInfo->AAC.AdecPlugInCs.ChNum;
    pPlugInCs->Resolution = pSetupInfo->AAC.AdecPlugInCs.Resolution;
    pPlugInCs->SelfSize = pSetupInfo->AAC.AdecPlugInCs.SelfSize;
    pPlugInCs->Update = pSetupInfo->AAC.AdecPlugInCs.Update;
    pPlugInCs->pSetUp_f = AmbaAudio_DecAacSetup;
    pPlugInCs->pProc_f = AmbaAudio_DecAacFrame;
    return AmbaADEC_InstallPlugInDec(pAdecHdlr, pPlugInCs);
}

static UINT32 AmbaAudio_DecPlugInPcmLibInstall(const UINT32 *pAdecHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    AMBA_ADEC_PLUGIN_DEC_CS_s *pPlugInCs = &DecCs;
    AMBA_AUDIO_PCM_CONFIG_s *pPcmCfg = &PcmDecCfg;

    /* Init PCM setting */
    pPcmCfg->BitsPerSample = pSetupInfo->PCM.BitsPerSample;
    pPcmCfg->DataFormat = pSetupInfo->PCM.DataFormat;
    pPcmCfg->FrameSize = pSetupInfo->PCM.FrameSize;


    pPlugInCs->pSelf = pPcmCfg;
    pPlugInCs->FrameSize = pSetupInfo->PCM.AdecPlugInCs.FrameSize;
    pPlugInCs->ChNum = pSetupInfo->PCM.AdecPlugInCs.ChNum;
    pPlugInCs->Resolution = pSetupInfo->PCM.AdecPlugInCs.Resolution;
    pPlugInCs->SelfSize = pSetupInfo->PCM.AdecPlugInCs.SelfSize;
    pPlugInCs->Update = pSetupInfo->PCM.AdecPlugInCs.Update;
    pPlugInCs->pSetUp_f = AmbaAudio_DecPcmSetup;
    pPlugInCs->pProc_f = AmbaAudio_DecPcmFrame;
    return AmbaADEC_InstallPlugInDec(pAdecHdlr, pPlugInCs);
}

/**
* Query audio driver to get the cached and non-cached memory size required
* @param [in]  pAinInfo Audio input information
* @return ErrorCode
*/
UINT32 AmbaAudioInp_QueryBufSize(AMBA_AIN_IO_CREATE_INFO_s *pAinInfo)
{
    UINT32 RtVal;

    RtVal = AmbaAIN_QueryBufSize(pAinInfo);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_QueryBufSize, RtVal: %d", RtVal, 0U);
        LOG_AUDIO_FRW_OK("AIN: Cached buffer size: %d", pAinInfo->CachedBufSize, 0U);
        LOG_AUDIO_FRW_OK("AIN: Non-cached buffer size: %d", pAinInfo->NonCachedBufSize, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_QueryBufSize error!!", 0U, 0U);
    }
    return RtVal;
}

/**
* Setup Ain info, including cached DMA data buffer, non-cached DMA descriptor buffer.
* Create the Ain resource and get the resource handler.
* @param [in]  pAinInfo Audio input information
* @param [in]  pAinHdlr Pointer of the audio input resource handler
* @return ErrorCode
*/
UINT32 AmbaAudioInp_Create(const AMBA_AUDIO_INPUT_s *pAinInfo, UINT32 **pAinHdlr)
{
    UINT32 RtVal;
    ULONG  AinCastValue;
    static char AinStopFlagName[25U] = "AinStopFlag";

    SvcAin2AencVariablesInit();

    /* Setup cache size*/
    RtVal = AmbaWrap_memset(pAinInfo->CachedInfo.pHead, 0, pAinInfo->CachedInfo.MaxSize);
    AmbaMisra_TypeCast(&AinCastValue, &pAinInfo->CachedInfo.pHead);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaWrap_memset[Cache], AinCacheMem Addr: 0x%x", (UINT32)AinCastValue, 0U);
        if (pAinInfo->AinInfo.CachedBufSize > pAinInfo->CachedInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("AIN ERR: Cache buffer size is too small: r(%d) > s(%d)",
                              pAinInfo->AinInfo.CachedBufSize, pAinInfo->CachedInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaWrap_memset[Cache] error!! AinCacheMem Addr: 0x%x", (UINT32)AinCastValue, 0U);
    }

    /* Setup Non-cache size*/
#if !defined(CONFIG_LINUX)
    RtVal = AmbaWrap_memset(pAinInfo->NonCachedInfo.pHead, 0, pAinInfo->NonCachedInfo.MaxSize);
    AmbaMisra_TypeCast(&AinCastValue, &pAinInfo->NonCachedInfo.pHead);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaWrap_memset[NonCache], AinNonCacheMem Addr: 0x%x", (UINT32)AinCastValue, 0U);
        if (pAinInfo->AinInfo.NonCachedBufSize > pAinInfo->NonCachedInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("AIN ERR: Noncache buffer size is too small: r(%d) > s(%d)",
                                  pAinInfo->AinInfo.NonCachedBufSize, pAinInfo->NonCachedInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaWrap_memset[NonCache] error!! AinNonCacheMem Addr: 0x%x", (UINT32)AinCastValue, 0U);
    }
#endif

    /* Create Ain resource and get the resource handler*/
    RtVal = AmbaAIN_CreateResource(&(pAinInfo->AinInfo), &pAinInfo->CachedInfo, &pAinInfo->NonCachedInfo, pAinHdlr);
    AmbaMisra_TypeCast(&AinCastValue, &(*pAinHdlr));
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_CreateResource, RtVal: %d, pAinHdlr: 0x%x", RtVal, (UINT32)AinCastValue);
        {
            RtVal = AmbaKAL_EventFlagCreate(&SvcAudioInpFlag, AinStopFlagName);
            if (AIN2AENC_OK == RtVal) {
                LOG_AUDIO_FRW_OK("AIN: Create Stop Flag, RtVal: %d", RtVal, 0U);
                RtVal = AmbaKAL_EventFlagClear(&SvcAudioInpFlag, 0xffffffffU);
                if (AIN2AENC_OK == RtVal) {
                    LOG_AUDIO_FRW_OK("AIN: Clear Stop Flag, RtVal: %d", RtVal, 0U);
                } else {
                    LOG_AUDIO_FRW_NG("AIN: Clear Stop Flag error!! RtVal: %d", RtVal, 0U);
                }
            } else {
                LOG_AUDIO_FRW_NG("AIN: Create Stop Flag error!! RtVal: %d", RtVal, 0U);
            }
            RtVal = AmbaAIN_ConfigEventHdlr(*pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_STOP, 1, AinStopDmaEntries);
            if (AIN2AENC_OK == RtVal) {
                LOG_AUDIO_FRW_OK("AIN: AmbaAIN_ConfigEventHdlr, RtVal: %d", RtVal, 0U);
                RtVal = AmbaAIN_RegisterEventHdlr(*pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_STOP, AmbaAudio_AinStopDmaCB);
                if (AIN2AENC_OK == RtVal) {
                    LOG_AUDIO_FRW_OK("AIN: AmbaAIN_RegisterEventHdlr, RtVal: %d", RtVal, 0U);
                } else {
                    LOG_AUDIO_FRW_NG("AIN: AmbaAIN_RegisterEventHdlr error!! RtVal: %d", RtVal, 0U);
                }
            } else {
                LOG_AUDIO_FRW_NG("AIN: AmbaAIN_ConfigEventHdlr error!! RtVal: %d", RtVal, 0U);
            }
        }

        /* Create Ain task */
        if (AmbaKAL_TaskCreate(&AinTask, pAinInfo->pInputTskName,
                               pAinInfo->AinputTskCtrl.Priority,
                               AmbaAudio_AinEntry,
                               (*pAinHdlr),
                               pAinInfo->AinputTskCtrl.pStackBase,
                               pAinInfo->AinputTskCtrl.StackSize,
                               0U/*DoNotStart*/) != KAL_ERR_NONE) {
            RtVal = AIN2AENC_ERR_0000;
        } else {
            LOG_AUDIO_FRW_OK("AIN: Create AIN task, RtVal: %d", RtVal, 0U);
        }

        if (AIN2AENC_OK == RtVal) {
            RtVal = AmbaKAL_TaskSetSmpAffinity(&AinTask, pAinInfo->AinputTskCtrl.CpuBits);
        }
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_CreateResource error!! RtVal: %d, pAinHdlr: 0x%x", RtVal, (UINT32)AinCastValue);
    }


    return RtVal;
}

/**
* Register the callback functions for AIN task to put the PCM data information.
* @param [in]  pAinHdlr Pointer of the audio input resource handler.
* @param [in]  pAinCbHdlr Callback functions for AIN task to put the PCM data information.
* @return ErrorCode
*/
UINT32 AmbaAudioInp_IONodeRegister(const UINT32 *pAinHdlr, AMBA_AIN_CB_HDLR_s *pAinCbHdlr)
{
    UINT32 RtVal;
    ULONG  AinCastValue;

    RtVal = AmbaAIN_RegisterCallBackFunc(pAinHdlr, pAinCbHdlr);
    AmbaMisra_TypeCast(&AinCastValue, &pAinCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_RegisterCallBackFunc, RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AinCastValue);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_RegisterCallBackFunc error!! RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AinCastValue);
    }

    return RtVal;
}

/**
* Query audio Audio encode bitstream buffer size
* @return Bitstream buffer size
*/
UINT32 AmbaAudioEnc_QueryBsSize(void)
{
    UINT32 AencBsSize = 0;

    AencBsSize = (AENC_FRAME_SIZE * AENC_CH_NUM * 4U * AU_SSP_BUF_DESC_NUM * 4U);

    return AencBsSize;
}

/**
* Query audio driver to get the cached memory size required to generate an audio encoder resource
* @param [in]  pAencInfo Audio encoder resource create information
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_QueryBufSize(AMBA_AUDIO_ENC_CREATE_INFO_s *pAencInfo)
{
    UINT32 RtVal;

    RtVal = AmbaAENC_QueryBufSize(pAencInfo);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_QueryBufSize, RtVal: %d", RtVal, 0U);
        LOG_AUDIO_FRW_OK("AENC: Cached buffer size: %d", pAencInfo->CachedBufSize, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_QueryBufSize error!!", 0U, 0U);
    }

    return RtVal;
}

/**
* AmbaAudioEnc_Init
* @param [in]  pEncInfo encode info
* @param [out]  pAencHdlr Pointer of the audio encoder resource handler.
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Init(const AMBA_AUDIO_ENC_s *pEncInfo, UINT32 **pAencHdlr)
{
    UINT32 RtVal;
    ULONG AencCastValue;
    AMBA_AENC_BUF_INFO_s AencBufInfo;
    UINT32 *pHead = pEncInfo->AencBufInfo.pHead;
    UINT32  Offset = ((pEncInfo->AencTskIdx * pEncInfo->AencCreateInfo.CachedBufSize)/(UINT32)sizeof(UINT32));

    pHead = &(pHead[Offset]);
    RtVal = AmbaWrap_memset(pHead, 0, pEncInfo->AencCreateInfo.CachedBufSize);
    AmbaMisra_TypeCast(&AencCastValue, &pHead);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaWrap_memset[Cache], RtVal: %d, pHead: 0x%x", RtVal, (UINT32)AencCastValue);
        if ((pEncInfo->AencCreateInfo.CachedBufSize * (UINT32)CONFIG_AENC_NUM) > pEncInfo->AencBufInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("AENC ERR: Cache buffer size is too small: r(%d) > s(%d)",
                                  pEncInfo->AencCreateInfo.CachedBufSize, pEncInfo->AencBufInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaWrap_memset[Cache] error!! RtVal: %d, pAencHdlr: 0x%x", RtVal, (UINT32)AencCastValue);
    }

    AencBufInfo.pHead = pHead;
    AencBufInfo.MaxSize = pEncInfo->AencCreateInfo.CachedBufSize;
    RtVal = AmbaAENC_CreateResource(&(pEncInfo->AencCreateInfo), &(AencBufInfo), pAencHdlr);
    AmbaMisra_TypeCast(&AencCastValue, &(*pAencHdlr));
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_CreateResource, RtVal: %d, pAencHdlr: 0x%x", RtVal, (UINT32)AencCastValue);

        //Record each AencHdlrIdx
        AencHdlrIdx[pEncInfo->AencTskIdx] = AencCastValue;
        LOG_AUDIO_FRW_OK("SvcAudioEnc_Init()[%d] AencCastValue = %d",pEncInfo->AencTskIdx, (UINT32)AencCastValue);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_CreateResource error!! RtVal: %d, pAencHdlr: 0x%x", RtVal, (UINT32)AencCastValue);
    }


    return RtVal;
}

/**
* AmbaAudioEnc_IONodeRegister
* @param [in]  pAencHdlr Pointer of the audio encoder resource handler.
* @param [in]  pAencCbHdlr encode callback
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_IONodeRegister(const UINT32 *pAencHdlr, AMBA_AENC_CB_HDLR_s *pAencCbHdlr)
{
    UINT32 RtVal;
    ULONG  AencCastValue;

    RtVal = AmbaAENC_RegisterCallBackFunc(pAencHdlr, pAencCbHdlr, 1U/*Use tick number*/, 0U/*blocking i/o node*/);
    AmbaMisra_TypeCast(&AencCastValue, &pAencCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_RegisterCallBackFunc, RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AencCastValue);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_RegisterCallBackFunc error!! RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AencCastValue);
    }

    return RtVal;
}

/**
* AmbaAudioEnc_Create
* @param [in]  pAencHdlr Pointer of the audio encoder resource handler.
* @param [in]  pAencBsBuf Encode buffer info
* @param [in]  BsBufSize bitsbuf size
* @param [in]  AencDataHdlrCB Audio data handler call-back function
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Create(const UINT32 *pAencHdlr, UINT32  *pAencBsBuf, UINT32 BsBufSize, AENC_DATA_HDLR_CB AencDataHdlrCB)
{
    UINT32 RtVal, AencIndex = AU_BUF_NA_ID;
    static char AencStopFlagName[25U]    = "AencStopFlag";

    AencIndex = CheckAencIdx(pAencHdlr);
    RtVal = AmbaAENC_SetUpBsBuffer(pAencHdlr, pAencBsBuf, BsBufSize);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_SetUpBsBuffer, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_SetUpBsBuffer error!! RtVal: %d", RtVal, 0U);
    }

    if (AencIndex != AU_BUF_NA_ID) {
        RtVal = AmbaAENC_ConfigEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME, 1, &AEncOneFrameEntries[AencIndex]);
    }
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_ConfigEventHdlr[%d], RtVal: %d", AencIndex, RtVal);
        RtVal = AmbaAENC_RegisterEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME, AencDataHdlrCB);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AENC: AmbaAENC_RegisterEventHdlr[%d], RtVal: %d", AencIndex, RtVal);
        } else {
            LOG_AUDIO_FRW_NG("AENC: AmbaAENC_RegisterEventHdlr[%d] error!! RtVal: %d", AencIndex, RtVal);
        }
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_ConfigEventHdlr[%d] error!! RtVal: %d", AencIndex, RtVal);
    }

    RtVal = AmbaAENC_ConfigEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_STOP, 1, &AEncStopEntries[AencIndex]);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_ConfigEventHdlr[%d], RtVal: %d", AencIndex, RtVal);
        RtVal = AmbaAENC_RegisterEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_STOP, AmbaAudio_AencStopCB);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AENC: AmbaAENC_RegisterEventHdlr[%d], RtVal: %d", AencIndex, RtVal);
        } else {
            LOG_AUDIO_FRW_NG("AENC: AmbaAENC_RegisterEventHdlr[%d] error!! RtVal: %d", AencIndex, RtVal);
        }
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_ConfigEventHdlr[%d] error!! RtVal: %d", AencIndex, RtVal);
    }

    RtVal = AmbaKAL_EventFlagCreate(&SvcAudioEncFlag[AencIndex], AencStopFlagName);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: Create Stop Flag[%d], RtVal: %d", AencIndex, RtVal);
        RtVal = AmbaKAL_EventFlagClear(&SvcAudioEncFlag[AencIndex], 0xffffffffU);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AENC: clear Stop Flag[%d], RtVal: %d", AencIndex, RtVal);
        } else {
            LOG_AUDIO_FRW_NG("AENC: clear Stop Flag[%d] error!! RtVal: %d", AencIndex, RtVal);
        }
    } else {
        LOG_AUDIO_FRW_NG("AENC: Create Stop Flag[%d] error!! RtVal: %d", AencIndex, RtVal);
    }

    return RtVal;
}

/**
* Setup audio coding type
* Setup / install the plug-in encoder.
* @param [in]  pAencHdlr Pointer of the audio encoder resource handler.
* @param [in]  pSetupInfo Audio setup information, including encode type (AAC or PCM)
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Setup(const UINT32 *pAencHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    UINT32 RtVal;

    if (AMBA_AUDIO_TYPE_PCM == pSetupInfo->Type) {
        RtVal = AmbaAudio_EncPlugInPcmLibInstall(pAencHdlr, pSetupInfo);
    } else if (AMBA_AUDIO_TYPE_AAC == pSetupInfo->Type){
        RtVal = AmbaAudio_EncPlugInAacLibInstall(pAencHdlr, pSetupInfo);
    } else {
        RtVal = AIN2AENC_ERR_0000;
        LOG_AUDIO_FRW_NG("AENC: AmbaAudioEnc_Setup wrong info, RtVal: %d", RtVal, 0U);
    }
    LOG_AUDIO_FRW_OK("AENC: AmbaAudioEnc_Setup, RtVal: %d", RtVal, 0U);

    return RtVal;
}

/**
* Open encode  I/O node with the registered call-back function.
* Start the audio encoder process.
* @param [in]  pEncInfo Audio encode information and resource handler
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Start(const AMBA_AUDIO_ENC_s *pEncInfo)
{
    UINT32 RtVal;
    ULONG  AencCastValue;

    AmbaMisra_TypeCast(&AencCastValue, &(pEncInfo->AencInfo.pAencHdlr));
    LOG_AUDIO_FRW_OK("AENC: AmbaAudioEnc_Start, pAencHdlr: 0x%x", (UINT32)AencCastValue, 0U);

    RtVal = AmbaAENC_OpenIoNode((pEncInfo->AencInfo.pAencHdlr), pEncInfo->AencInfo.pAencCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_OpenIoNode, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_OpenIoNode error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAENC_Start((pEncInfo->AencInfo.pAencHdlr), pEncInfo->AencInfo.FadingTime);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_Start, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_Start error, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskCreate(&AencTask[pEncInfo->AencTskIdx],
                               pEncInfo->pAencTskName,
                               pEncInfo->AencTskCtrl[pEncInfo->AencTskIdx].Priority,
                               AmbaAudio_AencEntry,
                               pEncInfo->AencInfo.pAencHdlr,
                               pEncInfo->AencTskCtrl[pEncInfo->AencTskIdx].pStackBase,
                               pEncInfo->AencTskCtrl[pEncInfo->AencTskIdx].StackSize,
                               1U/*AutoStart*/);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaKAL_TaskCreate, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaKAL_TaskCreate error, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskSetSmpAffinity(&AencTask[pEncInfo->AencTskIdx], pEncInfo->AencTskCtrl[pEncInfo->AencTskIdx].CpuBits);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaKAL_TaskSetSmpAffinity, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaKAL_TaskSetSmpAffinity error, RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Open audio input I/O node and I2S Rx FIFO reset / triggered
* Initialize and prepare the audio input process.
* @param [in]  pAEncInfo Audio input information and resource handler
* @param [in]  pCbAinCtrl Audio input control
* @return ErrorCode
*/
UINT32 AmbaAudioInp_Start(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo, AMBA_AIN_CTRL_t pCbAinCtrl)
{
    UINT32 RtVal;
    ULONG  AinCastValue;

    AmbaMisra_TypeCast(&AinCastValue, &(pAEncInfo->pAinHdlr));
    LOG_AUDIO_FRW_OK("AIN: AmbaAudioInp_Start, pAinHdlr: 0x%x", (UINT32)AinCastValue, 0U);

    RtVal = AmbaAIN_OpenIoNode((pAEncInfo->pAinHdlr), pAEncInfo->pAinCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_OpenIoNode, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_OpenIoNode error!! RtVal: %d", RtVal, 0U);
    }

    if (pAEncInfo->AencTskIdx == (pAEncInfo->NumOfAenc -1U)) {
        if (pCbAinCtrl != NULL) {
            /* rx reset */
            pCbAinCtrl(pAEncInfo->I2SChannel, 0U);
        }

        RtVal = AmbaAIN_Prepare((pAEncInfo->pAinHdlr));
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN: AmbaAIN_PrepareTask, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN: AmbaAIN_PrepareTask error!! RtVal: %d", RtVal, 0U);
        }

        if (pCbAinCtrl != NULL) {
            /* rx on */
            pCbAinCtrl(pAEncInfo->I2SChannel, 1U);
        }

        RtVal = AmbaKAL_TaskResume(&AinTask);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN: AmbaKAL_TaskResume AinTask, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN: AmbaKAL_TaskResume AinTask error!! RtVal: %d", RtVal, 0U);
        }

        RtVal = AmbaKAL_EventFlagSet(&SvcAudioInpFlag, AIN_DMA_START_FLG);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN: AmbaKAL_EventFlagSet AIN_DMA_START_FLG AinTask, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN: AmbaKAL_EventFlagSet AIN_DMA_START_FLG AinTask error!! RtVal: %d", RtVal, 0U);
        }
    }

    return RtVal;
}

/**
* Audio encode stop/delete
* Close audio encode I/O node
* @param [in]  pAEncInfo Audio encode stop information and resource handler
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Stop(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo)
{
    UINT32 RtVal;
    UINT32 ActualFlags = 0U;

    RtVal = AmbaAENC_Stop((pAEncInfo->pAencHdlr), pAEncInfo->FadingTime);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_Stop, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_Stop error!! RtVal: %d", RtVal, 0U);
    }

    if (AmbaKAL_EventFlagGet(&SvcAudioEncFlag[pAEncInfo->AencTskIdx], AENC_STOP_FLG, 0U/*or*/, 0U/*not clear*/,
                             &ActualFlags, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
        ActualFlags &= AENC_STOP_FLG;
        RtVal = AmbaKAL_EventFlagClear(&SvcAudioEncFlag[pAEncInfo->AencTskIdx], ActualFlags);
        if (ADEC_OK != RtVal) {
            LOG_AUDIO_FRW_OK("AmbaAudioEnc_Stop AmbaKAL_EventFlagClear error ", 0U, 0U);
        }
    }
    LOG_AUDIO_FRW_OK("AENC: AmbaAENC_Stop[%d] Done!! RtVal: %d", pAEncInfo->AencTskIdx, RtVal);

    RtVal = AmbaAENC_CloseIoNode((pAEncInfo->pAencHdlr), pAEncInfo->pAencCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_CloseIoNode, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_CloseIoNode error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskTerminate(&AencTask[pAEncInfo->AencTskIdx]);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaKAL_TaskTerminate AencTask, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaKAL_TaskTerminate AencTask error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskDelete(&AencTask[pAEncInfo->AencTskIdx]);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaKAL_TaskDelete, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaKAL_TaskDelete error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Audio input stop/delete and disable I2S Rx FIFO
* Close audio input I/O node
* @param [in]  pAEncInfo Audio input stop information and resource handler
* @param [in]  pCbAinCtrl Audio input control
* @return ErrorCode
*/
UINT32 AmbaAudioInp_Stop(const AMBA_AUDIO_ENC_INFO_s *pAEncInfo, AMBA_AIN_CTRL_t pCbAinCtrl)
{
    UINT32 RtVal;
    UINT32 ActualFlags;

    if (pAEncInfo->AencTskIdx == 0U) {
        RtVal = AmbaAIN_Stop((pAEncInfo->pAinHdlr));
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN: AmbaAIN_Stop, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN: AmbaAIN_Stop error!! RtVal: %d", RtVal, 0U);
        }

        if (AmbaKAL_EventFlagGet(&SvcAudioInpFlag, AIN_DMA_STOP_FLG, 0U/*or*/, 0U/*not clear*/,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER) == KAL_ERR_NONE) {
            ActualFlags &= AIN_DMA_STOP_FLG;
            RtVal = AmbaKAL_EventFlagClear(&SvcAudioInpFlag, ActualFlags);
            if (ADEC_OK != RtVal) {
                LOG_AUDIO_FRW_OK("AmbaAudioInp_Stop AmbaKAL_EventFlagClear error ", 0U, 0U);
            }
        }
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_Stop Done!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAIN_CloseIoNode((pAEncInfo->pAinHdlr), pAEncInfo->pAinCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_CloseIoNode, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_CloseIoNode error!! RtVal: %d", RtVal, 0U);
    }

    if (pAEncInfo->AencTskIdx == (pAEncInfo->NumOfAenc -1U)) {
        if (pCbAinCtrl != NULL) {
            /* rx off */
            pCbAinCtrl(pAEncInfo->I2SChannel, 2U);
        }

        RtVal = AmbaKAL_TaskSuspend(&AinTask);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN:  AmbaKAL_TaskSuspend AinTask, RtVal: %d", RtVal, 0U);
        }

        RtVal = AmbaKAL_TaskTerminate(&AinTask);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN:  AmbaKAL_TaskTerminate AinTask, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN:  AmbaKAL_TaskTerminate AinTask error!! RtVal: %d", RtVal, 0U);
        }

        RtVal = AmbaKAL_TaskDelete(&AinTask);
        if (AIN2AENC_OK == RtVal) {
            LOG_AUDIO_FRW_OK("AIN: AmbaKAL_TaskDelete, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AIN: AmbaKAL_TaskDelete error!! RtVal: %d", RtVal, 0U);
        }
    }

    return RtVal;
}

/**
* Un-Register the control settings for the AENC event handler.
* Delete audio encode task and release unused resource
* @param [in]  pAencHdlr Pointer of the audio encoder resource handler.
* @param [in]  AencDataHdlrCB Audio data handler call-back function
* @return ErrorCode
*/
UINT32 AmbaAudioEnc_Delete(const UINT32 *pAencHdlr, AENC_DATA_HDLR_CB AencDataHdlrCB)
{
    UINT32 RtVal, AencIndex = AU_BUF_NA_ID;

    AencIndex = CheckAencIdx(pAencHdlr);

    RtVal = AmbaAENC_DeRegisterEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_ONE_FRAME, AencDataHdlrCB);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAENC_DeRegisterEventHdlr(pAencHdlr, (UINT32)AMBA_AENC_EVENT_ID_ENCODE_STOP, AmbaAudio_AencStopCB);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAENC_DeleteResource(pAencHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaAENC_DeleteResource, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaAENC_DeleteResource, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagDelete(&SvcAudioEncFlag[AencIndex]);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AENC: AmbaKAL_EventFlagDelete[%d], RtVal: %d", AencIndex, RtVal);
    } else {
        LOG_AUDIO_FRW_NG("AENC: AmbaKAL_EventFlagDelete[%d], RtVal: %d", AencIndex, RtVal);
    }

    return RtVal;
}

/**
* Delete audio input task
* Release unused resource
* @param [in]  pAinHdlr Pointer of the audio input resource handler.
* @return ErrorCode
*/
UINT32 AmbaAudioInp_Delete(const UINT32 *pAinHdlr)
{
    UINT32 RtVal;

    RtVal = AmbaAIN_DeRegisterEventHdlr(pAinHdlr, (UINT32)AMBA_AIN_EVENT_ID_DMA_STOP, AmbaAudio_AinStopDmaCB);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_DeRegisterEventHdlr: AmbaAIN_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_DeRegisterEventHdlr: AmbaAIN_DeRegisterEventHdlr, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAIN_DeleteResource(pAinHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaAIN_DeleteResource, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaAIN_DeleteResource, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagDelete(&SvcAudioInpFlag);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AIN: AmbaKAL_EventFlagDelete, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AIN: AmbaKAL_EventFlagDelete, RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Query audio driver to get the cached and non-cached memory size required
* @param [in]  pAoutInfo Audio output information
* @return ErrorCode
*/
UINT32 AmbaAudioOup_QueryBufSize(AMBA_AOUT_IO_CREATE_INFO_s *pAoutInfo)
{
    UINT32 RtVal;

    RtVal = AmbaAOUT_QueryBufSize(pAoutInfo);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_QueryBufSize, RtVal: %d", RtVal, 0U);
        LOG_AUDIO_FRW_OK("AOUT: Cached buffer size: %d", pAoutInfo->CachedBufSize, 0U);
        LOG_AUDIO_FRW_OK("AOUT: Non-cached buffer size: %d", pAoutInfo->NonCachedBufSize, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_QueryBufSize error!!", 0U, 0U);
    }
    return RtVal;
}

/**
* Setup Aout info, including cached and non-cached DMA descriptor buffer.
* Create the Aout resource and get the resource handler.
* @param [in]  pAoutInfo Audio output information
* @param [out]  pAoutHdlr Pointer of the audio output resource handler
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Create(const AMBA_AUDIO_OUTPUT_s *pAoutInfo, UINT32 **pAoutHdlr)
{
    UINT32 RtVal;
    ULONG  AoutCastValue;
    static char SvcAudioOupFlagName[25] = "SvcAudioOupFlag";

    SvcAdec2AoutVariablesInit();

    /* Create Event Flag */
    RtVal = AmbaKAL_EventFlagCreate(&SvcAudioOupFlag, SvcAudioOupFlagName);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: SvcAudioOupFlag create success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: SvcAudioOupFlag create error!! RtVal: %d", RtVal, 0U);
    }
    RtVal = AmbaKAL_EventFlagClear(&SvcAudioOupFlag, 0xffffffffU);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: SvcAudioOupFlag clear success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: SvcAudioOupFlag clear error!! RtVal: %d", RtVal, 0U);
    }

    /* Setup cache size*/
    RtVal = AmbaWrap_memset(pAoutInfo->CachedInfo.pHead, 0, pAoutInfo->CachedInfo.MaxSize);
    AmbaMisra_TypeCast(&AoutCastValue, &pAoutInfo->CachedInfo.pHead);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AoutInfo: AmbaWrap_memset[Cache], AoutCacheMem Addr: 0x%x", (UINT32)AoutCastValue, 0U);
        if (pAoutInfo->AoutInfo.CachedBufSize > pAoutInfo->CachedInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("AOUT ERR: Cache buffer size is too small: r(%d) > s(%d)",
                              pAoutInfo->AoutInfo.CachedBufSize, pAoutInfo->CachedInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaWrap_memset[Cache] error!! AoutCacheMem Addr: 0x%x", (UINT32)AoutCastValue, 0U);
    }

    /* Setup Non-cache size*/
#if !defined(CONFIG_LINUX)
    RtVal = AmbaWrap_memset(pAoutInfo->NonCachedInfo.pHead, 0, pAoutInfo->NonCachedInfo.MaxSize);
    AmbaMisra_TypeCast(&AoutCastValue, &pAoutInfo->NonCachedInfo.pHead);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaWrap_memset[NonCache], AoutNonCacheMem Addr: 0x%x", (UINT32)AoutCastValue, 0U);
        if (pAoutInfo->AoutInfo.NonCachedBufSize > pAoutInfo->NonCachedInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("AOUT ERR: Noncache buffer size is too small: r(%d) > s(%d)",
                                  pAoutInfo->AoutInfo.NonCachedBufSize, pAoutInfo->NonCachedInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaWrap_memset[NonCache] error!! AoutNonCacheMem Addr: 0x%x", (UINT32)AoutCastValue, 0U);
    }
#endif

    /* Create AOUT resource and get the resource handler*/
    RtVal = AmbaAOUT_CreateResource(&(pAoutInfo->AoutInfo), &pAoutInfo->CachedInfo, &pAoutInfo->NonCachedInfo, pAoutHdlr);
    AmbaMisra_TypeCast(&AoutCastValue, &(*pAoutHdlr));
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_CreateResource success, RtVal: %d, pAoutHdlr: 0x%x", RtVal, (UINT32)AoutCastValue);
        /* Create AOUT task */
        if (AmbaKAL_TaskCreate(&AoutTask, pAoutInfo->pOutputTskName,
                               pAoutInfo->AoutputTskCtrl.Priority,
                               AmbaAudio_AoutEntry,
                               (*pAoutHdlr),
                               pAoutInfo->AoutputTskCtrl.pStackBase,
                               pAoutInfo->AoutputTskCtrl.StackSize,
                               0U/*DoNotStart*/) != KAL_ERR_NONE) {
            RtVal = ADEC2AOUT_ERR_0000;
        }

        if (ADEC2AOUT_OK == RtVal) {
            RtVal = AmbaKAL_TaskSetSmpAffinity(&AoutTask, pAoutInfo->AoutputTskCtrl.CpuBits);
        }

        if (ADEC2AOUT_OK == RtVal) {
            RtVal = AmbaAOUT_ConfigEventHdlr(*pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, 1, AoutStopDmaEntries);
            if (ADEC2AOUT_OK == RtVal) {
                LOG_AUDIO_FRW_OK("AOUT: Config event: AMBA_AOUT_EVENT_ID_DMA_STOP success, RtVal: %d", RtVal, 0U);
                RtVal = AmbaAOUT_RegisterEventHdlr(*pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, AmbaAudio_AoutStopDmaCB);
                if (ADEC2AOUT_OK == RtVal) {
                    LOG_AUDIO_FRW_OK("AOUT: Register AMBA_AOUT_EVENT_ID_DMA_STOP success, RtVal: %d", RtVal, 0U);
                } else {
                    LOG_AUDIO_FRW_NG("AOUT: Register AMBA_AOUT_EVENT_ID_DMA_STOP error!! RtVal: %d", RtVal, 0U);
                }
            } else {
                LOG_AUDIO_FRW_NG("AOUT: Config event: AMBA_AOUT_EVENT_ID_DMA_STOP error!! RtVal: %d", RtVal, 0U);
            }
        } else {
            LOG_AUDIO_FRW_NG("AOUT: [1] Create AOUT task error!! RtVal: %d", RtVal, 0U);
        }

        if (ADEC2AOUT_OK == RtVal) {
            RtVal = AmbaAOUT_ConfigEventHdlr(*pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_NO_WORK_IO, 1, AoutNoWorkIoEntries);
            if (ADEC2AOUT_OK == RtVal) {
                LOG_AUDIO_FRW_OK("AOUT: Config event: AMBA_AOUT_EVENT_ID_NO_WORK_IO success, RtVal: %d", RtVal, 0U);
                RtVal = AmbaAOUT_RegisterEventHdlr(*pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_NO_WORK_IO, SvcAudio_AoutNoWorkIoCB);
                if (ADEC2AOUT_OK == RtVal) {
                    LOG_AUDIO_FRW_OK("AOUT: Register AMBA_AOUT_EVENT_ID_NO_WORK_IO success, RtVal: %d", RtVal, 0U);
                } else {
                    LOG_AUDIO_FRW_NG("AOUT: Register AMBA_AOUT_EVENT_ID_NO_WORK_IO error!! RtVal: %d", RtVal, 0U);
                }
            } else {
                LOG_AUDIO_FRW_NG("AOUT: Config event: AMBA_AOUT_EVENT_ID_NO_WORK_IO error!! RtVal: %d", RtVal, 0U);
            }
        } else {
            LOG_AUDIO_FRW_NG("AOUT: [2] Create AOUT task error!! RtVal: %d", RtVal, 0U);
        }
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_CreateResource error!! RtVal: %d, pAoutHdlr: 0x%x", RtVal, (UINT32)AoutCastValue);
    }

    return RtVal;
}

/**
* Register the callback functions for AIN task to put the PCM data information.
* @param [in]  pAoutHdlr Pointer of the audio input resource handler.
* @param [in]  pAoutCbHdlr Callback functions for AIN task to put the PCM data information.
* @return ErrorCode
*/
UINT32 AmbaAudioOup_IONodeRegister(const UINT32 *pAoutHdlr, AMBA_AOUT_CB_HDLR_s *pAoutCbHdlr)
{
    UINT32 RtVal;
    ULONG  AoutCastValue;

    RtVal = AmbaAOUT_RegisterCallBackFunc(pAoutHdlr, pAoutCbHdlr);
    AmbaMisra_TypeCast(&AoutCastValue, &pAoutCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_RegisterCallBackFunc success, RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AoutCastValue);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_RegisterCallBackFunc error!! RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AoutCastValue);
    }

    return RtVal;
}

/**
* Open audio output I/O node.
* Initialize and prepare the audio output process.
* @param [in]  pADecInfo Audio output information and resource handler
* @param [in]  pCbAoutCtrl Audio output control
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Start(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32 RtVal, ActualFlags = 0U;
    ULONG  AOutCastValue;

    RtVal = AmbaKAL_EventFlagGet(&SvcAudioDecFlag, ADEC_DATA_READY_FLG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                 &ActualFlags, WAIT_FLAG_TIMEOUT);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Get Audio Data ready flag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Get Audio Data ready flag timeout!! RtVal: %d", RtVal, 0U);
    }

    AmbaMisra_TypeCast(&AOutCastValue, &(pADecInfo->pAoutHdlr));
    LOG_AUDIO_FRW_OK("AOUT: AmbaAudioOup_Start, pAoutHdlr: 0x%x", (UINT32)AOutCastValue, 0U);

    RtVal = AmbaAOUT_OpenIoNode((pADecInfo->pAoutHdlr), pADecInfo->pAoutCbHdlr);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_OpenIoNode success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_OpenIoNode error!! RtVal: %d", RtVal, 0U);
    }

    if (pCbAoutCtrl != NULL) {
        /* tx reset */
        pCbAoutCtrl(pADecInfo->I2SChannel, 0U);
    }

    RtVal = AmbaAOUT_Prepare((pADecInfo->pAoutHdlr));
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_PrepareTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_PrepareTask error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_RUNNING_FLG);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAudioOup_Start, Set SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAudioOup_Start, Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskResume(&AoutTask);
    if (AIN2AENC_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: Resume AoutTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Resume AoutTask error!! RtVal: %d", RtVal, 0U);
    }

    if (pCbAoutCtrl != NULL) {
        /* tx on */
        pCbAoutCtrl(pADecInfo->I2SChannel, 1U);
    }

    return RtVal;
}

/**
* Audio output task suspend and disable I2S Rx FIFO
* Close audio output I/O node
* @param [in]  pADecInfo Audio output stop information and resource handler
* @param [in]  EosStop Eos
* @param [in]  pCbAoutCtrl Audio output control
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Stop(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, UINT32 EosStop, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32 RtVal;
    UINT32 ActualFlags;

    if (EosStop == 1U) {
        RtVal = AmbaKAL_EventFlagGet(&SvcAudioOupFlag, AOUT_NO_WORK_IO_FLG,
                                     AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                     &ActualFlags, WAIT_FLAG_TIMEOUT);
        if (KAL_ERR_NONE == RtVal) {
            LOG_AUDIO_FRW_OK("AOUT: Get AOUT_NO_WORK_IO_FLG success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("AOUT: Get AAOUT_NO_WORK_IO_FLG timeout!! RtVal: %d", RtVal, 0U);
        }
    }

    RtVal = AmbaAOUT_Stop(pADecInfo->pAoutHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_Stop success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_Stop error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagGet(&SvcAudioOupFlag, AOUT_STOP_FLG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, WAIT_FLAG_TIMEOUT);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: Get AOUT_STOP_FLG success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Get AOUT_STOP_FLG timeout!! RtVal: %d", RtVal, 0U);
    }

    if (pCbAoutCtrl != NULL) {
        /* tx on */
        pCbAoutCtrl(pADecInfo->I2SChannel, 2U);
    }

    RtVal = AmbaAOUT_CloseIoNode(pADecInfo->pAoutHdlr, pADecInfo->pAoutCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_CloseIoNode success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_CloseIoNode error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagClear(&SvcAudioOupFlag, AOUT_RUNNING_FLG);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAudioOup_Stop, CLEAR SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAudioOup_Stop, CLEAR SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskSuspend(&AoutTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: Suspend AoutTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Suspend AoutTask error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Audio output  pause
* Close audio decoder I/O node
* @param [in]  pADecInfo Audio output information and resource handler
* @param [in]  pCbAoutCtrl Audio output control
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Pause(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32 RtVal, ActualFlags;

    RtVal = AmbaKAL_EventFlagClear(&SvcAudioOupFlag, AOUT_RUNNING_FLG);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAudioOup_Pause, CLEAR SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAudioOup_Pause, CLEAR SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagGet(&SvcAudioOupFlag, AOUT_PROC_DONE,
                                AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE,
                                &ActualFlags, AMBA_KAL_WAIT_FOREVER);
    if (KAL_ERR_NONE != RtVal) {
        LOG_AUDIO_FRW_NG("AOUT: Get AOUT_PROC_DONE timeout!! RtVal: %d", RtVal, 0U);
    }

    if (pCbAoutCtrl != NULL) {
        /* tx on */
        pCbAoutCtrl(pADecInfo->I2SChannel, 2U);
    }

    return RtVal;
}

/**
* Audio output resume
* @param [in]  pADecInfo Audio output information and resource handler
* @param [in]  pCbAoutCtrl Audio output control
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Resume(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32 RtVal;

    if (pCbAoutCtrl != NULL) {
        /* tx on */
        pCbAoutCtrl(pADecInfo->I2SChannel, 1U);
    }

    RtVal = AmbaKAL_EventFlagSet(&SvcAudioOupFlag, AOUT_RUNNING_FLG);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAudioOup_Pause, Set SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAudioOup_Pause, Set SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Delete audio output task
* Release unused resource
* @param [in]  pAoutHdlr Pointer of the audio output resource handler and I/O node call-back resource handler.
* @param [in]  pAoutCbHdlr
* @return ErrorCode
*/
UINT32 AmbaAudioOup_Delete(const UINT32 *pAoutHdlr, const AMBA_AOUT_CB_HDLR_s *pAoutCbHdlr)
{
    UINT32 RtVal;

    RtVal = AmbaAOUT_DeRegisterEventHdlr(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_DMA_STOP, AmbaAudio_AoutStopDmaCB);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: DeRigister AMBA_AOUT_EVENT_ID_DMA_STOP success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: DeRigister AMBA_AOUT_EVENT_ID_DMA_STOP error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAOUT_DeRegisterEventHdlr(pAoutHdlr, (UINT32)AMBA_AOUT_EVENT_ID_NO_WORK_IO, SvcAudio_AoutNoWorkIoCB);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: DeRigister AMBA_AOUT_EVENT_ID_NO_WORK_IO success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: DeRigister AMBA_AOUT_EVENT_ID_NO_WORK_IO error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAOUT_DeRegisterCallBackFunc(pAoutHdlr, pAoutCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_DeRegisterCallBackFunc success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_DeRegisterCallBackFunc error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaAOUT_DeleteResource(pAoutHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: AmbaAOUT_DeleteResource success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: AmbaAOUT_DeleteResource error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskTerminate(&AoutTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT:Terminate AoutTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Terminate AoutTask error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskDelete(&AoutTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: Delete AoutTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Delete AoutTask error!! RtVal: %d", RtVal, 0U);
    }

    /* Delete event flag */
    RtVal = AmbaKAL_EventFlagDelete(&SvcAudioOupFlag);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("AOUT: Delete SvcAudioOupFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("AOUT: Delete SvcAudioOupFlag error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Query audio driver to get the cached memory size required to generate an audio decoder resource
* @param [in]  pAdecInfo Audio decoder resource create information
* @return Audio decoder cached size
*/
UINT32 AmbaAudioDec_QueryBufSize(AMBA_AUDIO_DEC_CREATE_INFO_s *pAdecInfo)
{
    UINT32 RtVal;

    RtVal = AmbaADEC_QueryBufSize(pAdecInfo);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_QueryBufSize, RtVal: %d", RtVal, 0U);
        LOG_AUDIO_FRW_OK("ADEC: Cached buffer size: %d", pAdecInfo->CachedBufSize, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_QueryBufSize error!!", 0U, 0U);
    }

    return RtVal;
}

/**
* Setup audio decoder and cache information.
* Generate audio resources required for a specified decoder task.
* @param [in]  pDecInfo Audio decoder information
* @param [in]  pAdecHdlr Pointer of the audio decoder resource handler.
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Init(const AMBA_AUDIO_DEC_s *pDecInfo, UINT32 **pAdecHdlr)
{
    UINT32 RtVal;
    ULONG  AdecCastValue;

    RtVal = AmbaWrap_memset(pDecInfo->AdecBufInfo.pHead, 0, pDecInfo->AdecBufInfo.MaxSize);
    AmbaMisra_TypeCast(&AdecCastValue, &pDecInfo->AdecBufInfo.pHead);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaWrap_memset[Cache], RtVal: %d, pAdecHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
        if (pDecInfo->AdecInfo.CachedBufSize > pDecInfo->AdecBufInfo.MaxSize) {
            LOG_AUDIO_FRW_NG("ADEC ERR: Cache buffer size is too small: r(%d) > s(%d)",
                                  pDecInfo->AdecInfo.CachedBufSize, pDecInfo->AdecBufInfo.MaxSize);
        }
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaWrap_memset[Cache] error!! RtVal: %d, pAdecHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
    }

    RtVal = AmbaADEC_CreateResource(&(pDecInfo->AdecInfo), &(pDecInfo->AdecBufInfo), pAdecHdlr);
    AmbaMisra_TypeCast(&AdecCastValue, &(*pAdecHdlr));
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_CreateResource success, RtVal: %d, pAdecHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
        /* Create Event Flag */
        {
            static char SvcAudioDecFlagName[25] = "SvcAudioDecFlag";

            RtVal = AmbaKAL_EventFlagCreate(&SvcAudioDecFlag, SvcAudioDecFlagName);
            if (KAL_ERR_NONE == RtVal) {
                LOG_AUDIO_FRW_OK("AOUT: Create SvcAudioDecFlag success, RtVal: %d", RtVal, 0U);
            } else {
                LOG_AUDIO_FRW_NG("AOUT: Create SvcAudioDecFlag error!! RtVal: %d", RtVal, 0U);
            }
            RtVal = AmbaKAL_EventFlagClear(&SvcAudioDecFlag, 0xffffffffU);
            if (KAL_ERR_NONE == RtVal) {
                LOG_AUDIO_FRW_OK("AOUT: Clear SvcAudioDecFlag success, RtVal: %d", RtVal, 0U);
            } else {
                LOG_AUDIO_FRW_NG("AOUT: Clear SvcAudioDecFlag error!! RtVal: %d", RtVal, 0U);
            }
        }

        /* Create Adec task */
        if (AmbaKAL_TaskCreate(&AdecTask, pDecInfo->pAdecTskName,
                               pDecInfo->AdecTskCtrl.Priority,
                               AmbaAudio_AdecEntry,
                               (*pAdecHdlr),
                               pDecInfo->AdecTskCtrl.pStackBase,
                               pDecInfo->AdecTskCtrl.StackSize,
                               0U/*DoNotStart*/) != KAL_ERR_NONE) {
            RtVal = (UINT32)ADEC2AOUT_ERR_0000;
        }

        if (ADEC2AOUT_OK == RtVal) {
            RtVal = AmbaKAL_TaskSetSmpAffinity(&AdecTask, pDecInfo->AdecTskCtrl.CpuBits);
        }

        if (ADEC2AOUT_OK == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: Create ADEC task success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: 21Create ADEC task error!! RtVal: %d", RtVal, 0U);
        }

    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_CreateResource error!! RtVal: %d, pAdecHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
    }


    return RtVal;
}

/**
* Register the decode chain scanner callback functions for ADEC task to put audio decoded PCM data.
* Check the buffer status between the source PCM Buffer and the buffer registered to the I/O node.
* @param [in]  pAdecHdlr Pointer of the audio decoder resource handler.
* @param [in]  pAdecCbHdlr Handler of the decode chain scanner call-back functions
* @return ErrorCode
*/
UINT32 AmbaAudioDec_IONodeRegister(const UINT32 *pAdecHdlr, AMBA_ADEC_CB_HDLR_s *pAdecCbHdlr)
{
    UINT32 RtVal;
    ULONG  AdecCastValue;

    RtVal = AmbaADEC_RegisterCallBackFunc(pAdecHdlr, pAdecCbHdlr);
    AmbaMisra_TypeCast(&AdecCastValue, &pAdecCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_RegisterCallBackFunc success, RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_RegisterCallBackFunc error!! RtVal: %d, CbHdlr: 0x%x", RtVal, (UINT32)AdecCastValue);
    }

    return RtVal;
}

/**
* onfig/Register the control settings for the ADEC data handler and stop event handler.
* Setup audio bitstream buffer information to the audio decoder
* @param [in]  pAdecHdlr Pointer of the audio decoder resource handler.
* @param [in]  pAdecBsBuf Audio bitstream information
* @param [in]  BsBufSize Audio Bitstream buffer size
* @param [in]  AdecDataHdlrCB Audio decode data handler call-back function
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Create(const UINT32 *pAdecHdlr, UINT8  *pAdecBsBuf, UINT32 BsBufSize, ADEC_DATA_HDLR_CB AdecDataHdlrCB)
{
    UINT32 RtVal;

    RtVal = AmbaADEC_SetUpBsBuffer(pAdecHdlr, pAdecBsBuf, BsBufSize);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_SetUpBsBuffer success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_SetUpBsBuffer error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_ConfigEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_STOP, 1, AdecStopEntries);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_STOP success, RtVal: %d", RtVal, 0U);
        RtVal = AmbaADEC_RegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_STOP, AmbaAudio_AdecStopCB);
        if (ADEC2AOUT_OK == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_STOP success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_STOP error!! RtVal: %d", RtVal, 0U);
        }
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_STOP error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_ConfigEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME, 1, AdecUseOneFrameEntries);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME success, RtVal: %d", RtVal, 0U);
        RtVal = AmbaADEC_RegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME, AdecDataHdlrCB);
        if (ADEC2AOUT_OK == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME error!! RtVal: %d", RtVal, 0U);
        }
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_ConfigEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_READY, 1, AdecGetReadyEntries);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_READY success, RtVal: %d", RtVal, 0U);
        RtVal = AmbaADEC_RegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_READY, AmbaAudio_AdecReadyCB);
        if (ADEC2AOUT_OK == RtVal) {
            LOG_AUDIO_FRW_OK("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_READY success, RtVal: %d", RtVal, 0U);
        } else {
            LOG_AUDIO_FRW_NG("ADEC: Register AMBA_ADEC_EVENT_ID_DECODE_READY error!! RtVal: %d", RtVal, 0U);
        }
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Config AMBA_ADEC_EVENT_ID_DECODE_READY error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Setup audio coding type
* Setup / install the plug-in encoder.
* @param [in]  pAdecHdlr Pointer of the audio decoder resource handler.
* @param [in]  pSetupInfo Audio setup information, including encode type (AAC or PCM)
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Setup(const UINT32 *pAdecHdlr, const AMBA_AUDIO_SETUP_INFO_s *pSetupInfo)
{
    UINT32 RtVal = ADEC2AOUT_OK;

    if (AMBA_AUDIO_TYPE_PCM == pSetupInfo->Type) {
        RtVal = AmbaAudio_DecPlugInPcmLibInstall(pAdecHdlr, pSetupInfo);
    } else if (AMBA_AUDIO_TYPE_AAC == pSetupInfo->Type){
        RtVal = AmbaAudio_DecPlugInAacLibInstall(pAdecHdlr, pSetupInfo);
    } else {
        RtVal = ADEC2AOUT_ERR_0000;
        LOG_AUDIO_FRW_NG("ADEC: AmbaAudioDec_Setup wrong info, RtVal: %d", RtVal, 0U);
    }
    LOG_AUDIO_FRW_OK("ADEC: AmbaAudioDec_Setup success, RtVal: %d", RtVal, 0U);

    return RtVal;
}

/**
* Open decoder I/O node with the registered call-back function.
* Start the audio decoder process.
* @param [in]  pAdecInfo Audio decoder information and resource handler
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Start(const AMBA_AUDIO_DEC_INFO_s *pAdecInfo)
{
    UINT32 RtVal;
    ULONG  AdecCastValue;

    AmbaMisra_TypeCast(&AdecCastValue, &(pAdecInfo->pAdecHdlr));
    LOG_AUDIO_FRW_OK("ADEC: AmbaAudioDec_Start, pAdecHdlr: 0x%x", (UINT32)AdecCastValue, 0U);

    RtVal = AmbaADEC_OpenIoNode((pAdecInfo->pAdecHdlr), pAdecInfo->pAdecCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_OpenIoNode success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_OpenIoNode error!! RtVal: %d", RtVal, 0U);
    }


    RtVal = AmbaADEC_Start((pAdecInfo->pAdecHdlr));
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_Start success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_Start error, RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskResume(&AdecTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Resume AdecTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Resume AdecTask error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagSet(&SvcAudioDecFlag, ADEC_START_FLG);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: ADEC_START_FLG set success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: ADEC_START_FLG set error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Audio decoder stop/delete
* Close audio decoder I/O node
* @param [in]  pADecInfo Audio decoder stop information and resource handler
* @param [in]  Eos
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Stop(const AMBA_AUDIO_DEC_INFO_s *pADecInfo, UINT32 Eos)
{
    UINT32 RtVal;
    UINT32 ActualFlags = 0U;

    RtVal = AmbaADEC_Stop(pADecInfo->pAdecHdlr, Eos);
    AmbaMisra_TouchUnused(&RtVal);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_Stop success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_Stop error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_EventFlagGet(&SvcAudioDecFlag, ADEC_STOP_FLG,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, WAIT_FLAG_TIMEOUT);
    if (RtVal != OK) {
        LOG_AUDIO_FRW_NG("## fail to AmbaKAL_EventFlagGet", 0U, 0U);
    }

    /* Release ADEC related resources */
    RtVal = AmbaADEC_CloseIoNode(pADecInfo->pAdecHdlr, pADecInfo->pAdecCbHdlr);
    AmbaMisra_TouchUnused(&RtVal);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_CloseIoNode success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_CloseIoNode error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaKAL_TaskSuspend(&AdecTask);
    AmbaMisra_TouchUnused(&RtVal);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Suspend AdecTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Suspend AdecTask error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}

/**
* Un-Register the control settings for the ADEC event handler.
* Delete audio decoder task and release unused resource
* @param [in]  pAdecHdlr Pointer of the decoder encoder resource handler.
* @param [in]  pAdecCbHdlr Pointer of the decoder encoder cb handler.
* @param [in]  AdecDataHdlrCB Pointer of the decoder encoder resource handler.
* @return ErrorCode
*/
UINT32 AmbaAudioDec_Delete(const UINT32 *pAdecHdlr, const AMBA_ADEC_CB_HDLR_s *pAdecCbHdlr, ADEC_DATA_HDLR_CB AdecDataHdlrCB)
{
    UINT32 RtVal;

    RtVal = AmbaADEC_DeRegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_STOP, AmbaAudio_AdecStopCB);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_STOP success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_STOP error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_DeRegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME,  AdecDataHdlrCB);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_USE_ONE_FRAME error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_DeRegisterEventHdlr(pAdecHdlr, (UINT32)AMBA_ADEC_EVENT_ID_DECODE_READY, AmbaAudio_AdecReadyCB);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_READY success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: DeRegister AMBA_ADEC_EVENT_ID_DECODE_READY error!! RtVal: %d", RtVal, 0U);
    }


    RtVal = AmbaADEC_DeRegisterCallBackFunc(pAdecHdlr, pAdecCbHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_DeRegisterCallBackFunc success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_DeRegisterCallBackFunc error!! RtVal: %d", RtVal, 0U);
    }

    RtVal = AmbaADEC_DeleteResource(pAdecHdlr);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: AmbaADEC_DeleteResource success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: AmbaADEC_DeleteResource error!! RtVal: %d", RtVal, 0U);
    }


    RtVal = AmbaKAL_TaskTerminate(&AdecTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Terminate AdecTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Terminate AdecTask error!! RtVal: %d", RtVal, 0U);
    }


    RtVal = AmbaKAL_TaskDelete(&AdecTask);
    if (ADEC2AOUT_OK == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Delete AdecTask success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Delete AdecTask error!! RtVal: %d", RtVal, 0U);
    }


    /* Delete event flag */
    RtVal = AmbaKAL_EventFlagDelete(&SvcAudioDecFlag);
    if (KAL_ERR_NONE == RtVal) {
        LOG_AUDIO_FRW_OK("ADEC: Delete SvcAudioDecFlag success, RtVal: %d", RtVal, 0U);
    } else {
        LOG_AUDIO_FRW_NG("ADEC: Delete SvcAudioDecFlag error!! RtVal: %d", RtVal, 0U);
    }

    return RtVal;
}
