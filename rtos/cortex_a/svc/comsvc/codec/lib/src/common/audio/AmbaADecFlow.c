/**
*  @file AmbaADecFlow.c
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
*  @details amba audio decode flow
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
#include "AmbaNVM_Partition.h"


#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudio.h"

#include "AmbaSvcWrap.h"
#include "AmbaADecFlow.h"
#include "AmbaAudio_AacDec.h"
#include "AmbaCodecCom.h"

static AMBA_AUDIO_DEC_INFO_s   ADecInfo;

static AMBA_AOUT_CB_HDLR_s     AoutCbHdlr GNU_SECTION_NOZEROINIT;
static AMBA_ADEC_CB_HDLR_s     AdecCbHdlr GNU_SECTION_NOZEROINIT;

static UINT32                  *pAoutHdlr GNU_SECTION_NOZEROINIT;
static UINT32                  *pAdecHdlr GNU_SECTION_NOZEROINIT;

static AMBA_ADEC_DATA_INFO_s   *plastAdecDataInfo = NULL;

static UINT32                  *pCurWritePtr = NULL;
static UINT32                  DataBlockUseSize = 0U;
static UINT32                  Adec2AoutBufIdx = 0U;
static UINT32                  IsUsed = 0U;

static AMBA_AUDIO_DEMUX_CTRL_s *pDmuxHdlr GNU_SECTION_NOZEROINIT;

static AMBA_KAL_EVENT_FLAG_t    SvcAudioFeedFlag GNU_SECTION_NOZEROINIT;
#define AUDIO_FEED_DATA        (0x00000001U)

static AMBA_KAL_EVENT_FLAG_t    SvcAudioDecFlag GNU_SECTION_NOZEROINIT;
#define AUDIO_DEC_OUTPUT_ONE_FRAME  (0x00000001U)

static AMBA_AUDIO_OUTPUT_s      g_AOutInfo    GNU_SECTION_NOZEROINIT;
static AMBA_AUDIO_DEC_s         DecInfo       GNU_SECTION_NOZEROINIT;
static AMBA_AUDIO_SETUP_INFO_s  SetupInfo     GNU_SECTION_NOZEROINIT;
static AMBA_KAL_TASK_t          AudioFeedTask GNU_SECTION_NOZEROINIT;
static UINT32                   g_AudioSource GNU_SECTION_NOZEROINIT;

static inline void LOG_AUDIO_APP_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#endif
}

static inline void LOG_AUDIO_APP_OK(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#endif
}


static void* SvcAudioFeed_TaskEntry(void* EntryArg)
{
    UINT32 ActualFlags = 0U;
    UINT32 LoopU = 1U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&LoopU);
    while (LoopU == 1U) {
        UINT32 RtVal;
        RtVal = AmbaKAL_EventFlagGet(&SvcAudioFeedFlag, AUDIO_FEED_DATA,
                                     AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                     &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (ADEC_OK != RtVal) {
            LOG_AUDIO_APP_NG("SvcAudioFeed_TaskEntry EventFlagGet error ", 0U, 0U);
            LoopU = 0U;
        }


        if (AMBA_AUDIO_SOURCE_AUDIO == g_AudioSource) {
            AmbaAudio_FeedAudio(pDmuxHdlr);
        } else if (AMBA_AUDIO_SOURCE_ROM_AUD == g_AudioSource) {
            AmbaAudio_FeedROMAudio(pDmuxHdlr, SetupInfo.NvmID);
        } else {
            //Do nothing
        }
    }

    return NULL;
}

static UINT32 AdecUseOneFrmCB(void *pEventData)
{
    const AMBA_ADEC_AUDIO_DESC_s *pDesc;
    const AMBA_ADEC_AUDIO_DESC_s *pDecCbDesc;
    UINT32 RetVal = (UINT32)ADEC_OK;
    //AmbaPrint_PrintStr5("Func: [%s]",__func__, NULL, NULL, NULL, NULL);

    AmbaSvcWrap_MisraMemcpy(&pDecCbDesc, &pEventData, sizeof(AMBA_ADEC_AUDIO_DESC_s *));
    DataBlockUseSize += pDecCbDesc->DataSize;
    if (DataBlockUseSize >= (UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE) {
        DataBlockUseSize -= (UINT32)AMBA_AUDIO_DEMUX_BUFFER_SIZE;
        pDesc = AmbaAudio_BsBufDecGetRdPtr(&AudioDemuxHdlr[0]);
        if (pDesc == NULL) {
            AmbaPrint_PrintStr5("  Audio_DecGetReadPointer get NULL desc", NULL, NULL, NULL, NULL, NULL);
        }
        RetVal = AmbaAudio_BsBufDecUptRdPtr(&AudioDemuxHdlr[0]);
        if (ADEC2AOUT_OK != RetVal) {
            LOG_AUDIO_APP_NG("AmbaAudio_BsBufDecUptRdPtr error, RtVal: %d", RetVal, 0U);
        }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        {
            extern int beep_ready(void);
            beep_ready();
        }
#else

        RetVal = AmbaKAL_EventFlagSet(&SvcAudioFeedFlag, AUDIO_FEED_DATA);
        if (ADEC2AOUT_OK != RetVal) {
            LOG_AUDIO_APP_NG("SvcAudioFeedFlag FlagSet error, RtVal: %d", RetVal, 0U);
        }
#endif
    }

    AmbaMisra_TouchUnused(pEventData);
    return RetVal;
}

static void APPInit(void)
{
    pAoutHdlr = NULL;
    pAdecHdlr = NULL;
    DataBlockUseSize = 0U;
    Adec2AoutBufIdx = 0U;
    plastAdecDataInfo = NULL;
    pCurWritePtr = NULL;
    g_AudioSource = 0U;
}

void AmbaADecFlow_InfoGet(AMBA_ADEC_FLOW_INFO_s *pInfo)
{
    pInfo->pAoutInfo  = &(g_AOutInfo);
    pInfo->pADecInfo  = &(DecInfo);
    pInfo->pADecSetup = &(SetupInfo);
}

static UINT32 AoutGetPcmCbTest(UINT32 **pPcmBuf, UINT32 Size, UINT32 *pLof)
{
    UINT32                             RetVal, Err;
    static const AMBA_AOUT_DATA_INFO_s *plastAoutDataInfo;

    plastAoutDataInfo = AmbaAudio_DecBufGetReadPointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
    *pLof = plastAoutDataInfo->Lof;
    AmbaSvcWrap_MisraMemcpy(*pPcmBuf, plastAoutDataInfo->pDataBuf, Size);
    AmbaMisra_TouchUnused(&(*pPcmBuf));
    RetVal = AmbaAudio_DecBufUpdateReadPointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
    if (Size == 0U) {
        AmbaPrint_PrintStr5("  AOUT get data zero size!!", NULL, NULL, NULL, NULL, NULL);
    } else {
        g_AOutInfo.OutputCnt++;

        if (IsUsed == 1U) {
            Err = AmbaKAL_EventFlagSet(&SvcAudioDecFlag, AUDIO_DEC_OUTPUT_ONE_FRAME);
            if (KAL_ERR_NONE != Err) {
                LOG_AUDIO_APP_NG("AoutGetPcmCbTest FlagSet error, Err: %d", Err, 0U);
            }
        }
    }

    return RetVal;
}

static UINT32 AoutGetSizeCbTest(UINT32 *pSize)
{
    UINT32 RetVal;

    RetVal = AmbaAudio_DecBufPeekRemainSize(&AudioDecBufHdlr[Adec2AoutBufIdx], pSize);
    //AmbaPrint_PrintUInt5("  AOUT get size: %d", *pSize, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 AdecPutPcmCbTest(const UINT32 *pHdlr, UINT32 *pPcmBuf, UINT32 Size, const UINT32 *pLof)
{
    UINT32 RetVal = (UINT32)ADEC_OK;
    AMBA_ADEC_DATA_INFO_s *pInfoGet;
    UINT8   *pWp;
    UINT8   *pRp;
    UINT32  DescRoom;
    UINT32  FrmBufRemain;
    UINT32  CpSize;
    ULONG   Tmp1, Tmp2;

    AmbaMisra_TouchUnused(&pHdlr);
    AmbaMisra_TouchUnused(plastAdecDataInfo);
    if (pHdlr != NULL) {
        if (plastAdecDataInfo == NULL) {
            plastAdecDataInfo = AmbaAudio_DecBufGetWritePointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
            AmbaMisra_TouchUnused(plastAdecDataInfo);
            if (plastAdecDataInfo != NULL) {
                pCurWritePtr = plastAdecDataInfo->pDataBuf;
            }
        }
        AmbaMisra_TouchUnused(plastAdecDataInfo);
        if (plastAdecDataInfo != NULL) {
            pInfoGet        = plastAdecDataInfo;
            pWp             = (UINT8 *)pCurWritePtr;
            pRp             = (UINT8 *)pPcmBuf;
            AmbaMisra_TypeCast(&Tmp1, &pWp);
            AmbaMisra_TypeCast(&Tmp2, &(plastAdecDataInfo->pDataBuf));
            Tmp1 = Tmp1 - Tmp2;
            DescRoom        = (AU_SSP_BUF_CHUNK_SIZE) - (UINT32)Tmp1;
            FrmBufRemain    = Size;

            AmbaMisra_TouchUnused(&FrmBufRemain);
            while (FrmBufRemain != 0U) {
                AmbaMisra_TouchUnused(&FrmBufRemain);
                AmbaMisra_TouchUnused(&DescRoom);
                AmbaMisra_TouchUnused(&pLof);
                if (plastAdecDataInfo != NULL) {
                    if ((FrmBufRemain <= DescRoom) && (*pLof == 1U)) {
                        plastAdecDataInfo->Lof = 1;
                    } else {
                        plastAdecDataInfo->Lof = 0;
                    }
                }
                AmbaMisra_TouchUnused(&DescRoom);
                AmbaMisra_TouchUnused(&FrmBufRemain);
                if (DescRoom >= FrmBufRemain) {
                    CpSize  = FrmBufRemain;
                    AmbaSvcWrap_MisraMemcpy(pWp, pRp, CpSize);
                    pRp = &pRp[CpSize]; //pRp += CpSize;
                    pWp = &pWp[CpSize]; //pWp += CpSize;
                    FrmBufRemain -= CpSize;
                    DescRoom -= CpSize;
                    AmbaMisra_TouchUnused(&pLof);
                    AmbaMisra_TouchUnused(&DescRoom);
                    if ((*pLof == 1U) || (DescRoom == 0U)) {
                        AmbaSvcWrap_MisraMemset(pWp, 0, DescRoom);
                        plastAdecDataInfo = NULL;
                        pCurWritePtr = NULL;
                        pInfoGet->Size = (AU_SSP_BUF_CHUNK_SIZE) - DescRoom;
                        pInfoGet->Lof = *pLof;
                        RetVal = AmbaAudio_DecBufUpdateWritePointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
                        //AmbaPrint_PrintStr5("  AmbaAdec2AoutBuf_SynWp done", NULL, NULL, NULL, NULL, NULL);
                        DescRoom = 0;
                        FrmBufRemain = 0;
                    }
                } else { /* DescRoom < FrmBufRemain */
                    CpSize  = DescRoom;
                    AmbaSvcWrap_MisraMemcpy(pWp, pRp, CpSize);
                    pRp = &pRp[CpSize]; //pRp += CpSize;
                    pWp = &pWp[CpSize]; //pWp += CpSize;
                    FrmBufRemain -= CpSize;
                    DescRoom -= CpSize;
                    plastAdecDataInfo = NULL;
                    pCurWritePtr = NULL;
                    pInfoGet->Size = AU_SSP_BUF_CHUNK_SIZE;
                    pInfoGet->Lof = *pLof;
                    RetVal = AmbaAudio_DecBufUpdateWritePointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
                    //AmbaPrint_PrintStr5("  AmbaAdec2AoutBuf_SynWp done", NULL, NULL, NULL, NULL, NULL);

                    /* Get next desc */
                    plastAdecDataInfo = AmbaAudio_DecBufGetWritePointer(&AudioDecBufHdlr[Adec2AoutBufIdx]);
                    //AmbaPrint_PrintStr5("  2 AmbaAdec2AoutBuf_GetWp done", NULL, NULL, NULL, NULL, NULL);
                    AmbaMisra_TouchUnused(plastAdecDataInfo);
                    if (plastAdecDataInfo != NULL) {
                        pCurWritePtr = plastAdecDataInfo->pDataBuf;
                        pInfoGet        = plastAdecDataInfo;
                        AmbaSvcWrap_MisraMemcpy(&pWp, &pCurWritePtr, sizeof(UINT8 *));  //pWp = (UINT8 *)pCurWritePtr;
                        DescRoom = AU_SSP_BUF_CHUNK_SIZE;
                    }
                }
            }
            AmbaSvcWrap_MisraMemcpy(&pCurWritePtr, &pWp, sizeof(UINT32 *)); //pCurWritePtr = (UINT32 *)pWp;
        }
    }

    //AmbaPrint_PrintUInt5("  ADEC put data=%d  Lof=%d", Size, *pLof, 0U, 0U, 0U);
    return RetVal;
}

static UINT32 AdecGetSizeCbTest(const UINT32 *pHdlr, UINT32 *pSize)
{
    UINT32 RetVal = (UINT32)ADEC_OK;

    if (pHdlr != NULL) {
        RetVal = AmbaAudio_DecBufPeekRoomSize(&AudioDecBufHdlr[Adec2AoutBufIdx], pSize);
    }

    return RetVal;
}

void *AmbaADecFlow_GetDemuxCtrlHdlr(void)
{
    return (void *)pDmuxHdlr;
}

/**
* DecFlow Setup
* @param [in]  Type
* @param [in]  Source
* @param [in]  MPly
* @param [in]  ADecCB Dec CB func
* @param [out]  pDecId
* @return ErrorCode
*/
void AmbaADecFlow_Setup(UINT32 Type, UINT32 Source, ULONG MPly, ADEC_DATA_HDLR_CB ADecCB, ULONG *pDecId)
{
    UINT32 RtVal = AUDIO_OK, Err;
    static UINT32  AoutTaskStack[0x2000] GNU_SECTION_NOZEROINIT;
    static UINT32  AdecTaskStack[0x2000] GNU_SECTION_NOZEROINIT;

    static char OutputTskName[32]= "SvcAoutTask";
    static char DecodeTskName[32]= "SvcAdecTask";
    static char DecFlagName[32]  = "ADecFlag";
    ULONG  AoutCastValue, AdecCastValue, pDecHdlr;
    UINT8  *pBitsBuf;

    AmbaMisra_TypeCast(&pBitsBuf, &(SetupInfo.BitsBufAddr));

    AmbaMisra_TouchUnused(&RtVal);

    if (IsUsed == 0U) {
        APPInit();

        if (RtVal == AUDIO_OK) {
            Err = AmbaKAL_EventFlagCreate(&SvcAudioDecFlag, DecFlagName);
            if (Err != KAL_ERR_NONE) {
                LOG_AUDIO_APP_NG("AmbaKAL_EventFlagCreate failed %u ", Err, 0U);
                RtVal = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        if (RtVal == AUDIO_OK) {
            Err = AmbaKAL_EventFlagClear(&SvcAudioDecFlag, 0xFFFFFFFFU);
            if (Err != KAL_ERR_NONE) {
                LOG_AUDIO_APP_NG("AmbaKAL_EventFlagClear failed %u ", Err, 0U);
                RtVal = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        g_AudioSource = Source;

        RtVal = AmbaAudioOup_QueryBufSize(&g_AOutInfo.AoutInfo);
        LOG_AUDIO_APP_OK("AmbaAudioOup_QueryBufSize, RtVal : %d ", RtVal, 0U);

        AmbaMisra_TypeCast(&AoutCastValue, &g_AOutInfo.CachedInfo.pHead);
        LOG_AUDIO_APP_OK("CachedInfo.pHead: 0x%x, MaxSize = %d", (UINT32)AoutCastValue, g_AOutInfo.CachedInfo.MaxSize);
        AmbaMisra_TypeCast(&AoutCastValue, &g_AOutInfo.NonCachedInfo.pHead);
        LOG_AUDIO_APP_OK("AoutNonCachedInfo.pHead: 0x%x, MaxSize = %d", (UINT32)AoutCastValue, g_AOutInfo.NonCachedInfo.MaxSize);

        g_AOutInfo.pOutputTskName            = OutputTskName;
        g_AOutInfo.AoutputTskCtrl.pStackBase = AoutTaskStack;
        g_AOutInfo.AoutputTskCtrl.StackSize  = (UINT32)sizeof(AoutTaskStack);
        g_AOutInfo.OutputCnt                 = 0U;
        RtVal = AmbaAudioOup_Create(&g_AOutInfo, &pAoutHdlr);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Create, RtVal : %d ", RtVal, 0U);

        AmbaAudio_DecBufIdHdrInit();
        Adec2AoutBufIdx = AmbaAudio_DecBufCreate();

        AoutCbHdlr.GetPcm = AoutGetPcmCbTest;
        AoutCbHdlr.GetSize = AoutGetSizeCbTest;
        RtVal = AmbaAudioOup_IONodeRegister(pAoutHdlr, &AoutCbHdlr);
        LOG_AUDIO_APP_OK("AmbaAudioOup_IONodeRegister, RtVal : %d ", RtVal, 0U);

        if (Type == AMBA_AUDIO_TYPE_PCM) {
            DecInfo.AdecInfo.PlugInLibSelfSize   = (UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s);
            DecInfo.AdecInfo.NeededBytes         = 8192;
        } else if (Type == AMBA_AUDIO_TYPE_AAC) {
            DecInfo.AdecInfo.PlugInLibSelfSize   = (UINT32)sizeof(au_aacdec_config_t);
            DecInfo.AdecInfo.NeededBytes         = 768;
        } else {
            LOG_AUDIO_APP_NG("Wrong DecType info", 0U, 0U);
        }
        RtVal = AmbaAudioDec_QueryBufSize(&DecInfo.AdecInfo);
        LOG_AUDIO_APP_OK("AmbaAudioDec_QueryBufSize, RtVal : %d ", RtVal, 0U);

        DecInfo.pAdecTskName        = DecodeTskName;
        DecInfo.AdecTskCtrl.pStackBase = AdecTaskStack;
        DecInfo.AdecTskCtrl.StackSize  = (UINT32)sizeof(AdecTaskStack);
        RtVal = AmbaAudioDec_Init(&DecInfo, &pAdecHdlr);
        LOG_AUDIO_APP_OK("AmbaAudioDec_Init, RtVal : %d ", RtVal, 0U);

        AmbaMisra_TypeCast(&AdecCastValue, &DecInfo.AdecBufInfo.pHead);
        LOG_AUDIO_APP_OK("AoutNonCachedInfo.pHead: 0x%x", (UINT32)AdecCastValue, 0U);
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("AdecCachedInfo.MaxSize: %d", DecInfo.AdecBufInfo.MaxSize, 0U, 0U, 0U, 0U);
#endif

        //Demuxer
#if 0
        RtVal = AmbaAudio_DeMuxCreate(Source, MPly);
#endif
        pDmuxHdlr = AmbaAudio_BsBufDecInit(pAdecHdlr, pBitsBuf, SetupInfo.BitsBufSize, 0U, Source);
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("AmbaAudio_DeMuxSetUp, RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
#endif

        AdecCbHdlr.PutPcm = AdecPutPcmCbTest;
        AdecCbHdlr.GetSize = AdecGetSizeCbTest;
        RtVal = AmbaAudioDec_IONodeRegister(pAdecHdlr, &AdecCbHdlr);
        LOG_AUDIO_APP_OK("AmbaAudioDec_IONodeRegister, RtVal : %d ", RtVal, 0U);

        if (AMBA_AUDIO_SOURCE_MP4 == g_AudioSource) {
            RtVal = AmbaAudioDec_Create(pAdecHdlr, pBitsBuf, SetupInfo.BitsBufSize, ADecCB);
            LOG_AUDIO_APP_OK("AmbaAudioDec_Create, RtVal : %d ", RtVal, 0U);
        } else {
            RtVal = AmbaAudioDec_Create(pAdecHdlr, pBitsBuf, SetupInfo.BitsBufSize, AdecUseOneFrmCB);
            LOG_AUDIO_APP_OK("AmbaAudioDec_Create, RtVal : %d ", RtVal, 0U);
        }

        if (Type == AMBA_AUDIO_TYPE_PCM) {
            SetupInfo.Type = AMBA_AUDIO_TYPE_PCM;
            SetupInfo.PCM.AdecPlugInCs.SelfSize  = (UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s);
            SetupInfo.PCM.AdecPlugInCs.Update    = 0U;
        } else if (Type == AMBA_AUDIO_TYPE_AAC) {
            SetupInfo.Type = AMBA_AUDIO_TYPE_AAC;

            if (AMBA_AUDIO_SOURCE_MP4 == Source) {
                SetupInfo.AAC.BitstreamType         = (UINT32)AAC_BS_RAW;
            } else if ((AMBA_AUDIO_SOURCE_AUDIO == Source) || (AMBA_AUDIO_SOURCE_ROM_AUD == Source)){
                SetupInfo.AAC.BitstreamType         = (UINT32)AAC_BS_ADTS;
            } else {
                SetupInfo.AAC.BitstreamType         = (UINT32)AAC_BS_RAW;
            }
            SetupInfo.AAC.AdecPlugInCs.SelfSize = (UINT32)sizeof(AMBA_AUDIO_AAC_CONFIG_s);
            SetupInfo.AAC.AdecPlugInCs.Update   = 0;
        } else {
            LOG_AUDIO_APP_NG("Wrong DecType info", 0U, 0U);;
        }

        RtVal = AmbaAudioDec_Setup(pAdecHdlr, &SetupInfo);
        LOG_AUDIO_APP_OK("AmbaAudioDec_Setup, RtVal : %d ", RtVal, 0U);

        /* Reset BS buffer content conditions */
        if (Source == AMBA_AUDIO_SOURCE_ROM_AUD) {
            if (1U == pDmuxHdlr->AudFromROMCtrl.IsAudROMReload) {
                AmbaSvcWrap_MisraMemset(pBitsBuf, 0, SetupInfo.BitsBufSize);
            } else {
                //No need to reset
            }
        } else {
            AmbaSvcWrap_MisraMemset(pBitsBuf, 0, SetupInfo.BitsBufSize);
        }
#if 0
        if (AMBA_AUDIO_SOURCE_MP4 == Source) {
            //
        } else if (AMBA_AUDIO_SOURCE_AUDIO == Source) {
            for (UINT32 i = 0; i < AU_DEMUX_DESC_NUM; i++) {
                AmbaAudio_FeedAudio(pDmuxHdlr);
            }
        } else if (AMBA_AUDIO_SOURCE_ROM_AUD == Source) {
            for (UINT32 i = 0; i < AU_DEMUX_DESC_NUM; i++) {
                AmbaAudio_FeedROMAudio(pDmuxHdlr, SetupInfo.NvmID);
            }
        } else {
            //Do nothing
        }
#else 
        (void)AmbaWrap_memset(pBitsBuf, 0, SetupInfo.BitsBufSize);
#endif
        AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);
        *pDecId = pDecHdlr;
        IsUsed = 1U;
    } else {
        LOG_AUDIO_APP_NG("audio decoder source is already init", 0U, 0U);
    }
}

/**
* create feed task
* @param [in]  DecId decode id
*/
void AmbaADecFlow_FeedTaskCreate(ULONG DecId)
{
    UINT32         Rval = CODEC_OK, Err;
    ULONG          pDecHdlr;
    static UINT32  AudioFeedTaskStack[0x2000] GNU_SECTION_NOZEROINIT;
    static char    DecFeedFlagName[32]  = "DecFeedFlag";
    static char    AudioFeedTskName[32] = "AudioFeedTsk";

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        if (AMBA_AUDIO_SOURCE_MP4 != g_AudioSource) {
            if (Rval == CODEC_OK) {
                Err = AmbaKAL_EventFlagCreate(&SvcAudioFeedFlag, DecFeedFlagName);
                if (Err != KAL_ERR_NONE) {
                    LOG_AUDIO_APP_NG("AmbaKAL_EventFlagCreate failed %u ", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_EventFlagClear(&SvcAudioFeedFlag, 0xFFFFFFFFU);
                if (Err != KAL_ERR_NONE) {
                    LOG_AUDIO_APP_NG("AmbaKAL_EventFlagClear failed %u ", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskCreate(&(AudioFeedTask),
                                        AudioFeedTskName,
                                        SetupInfo.FeedTaskPriority,
                                        SvcAudioFeed_TaskEntry,
                                        NULL,
                                        AudioFeedTaskStack,
                                        (UINT32)sizeof(AudioFeedTaskStack),
                                        0);
                if (Err != KAL_ERR_NONE) {
                    LOG_AUDIO_APP_NG("AmbaKAL_TaskCreate failed %u ", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskSetSmpAffinity(&(AudioFeedTask), SetupInfo.FeedTaskCpuBits);
                if (Err != KAL_ERR_NONE) {
                    LOG_AUDIO_APP_NG("AmbaKAL_TaskSetSmpAffinity failed %u ", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == CODEC_OK) {
                Err = AmbaKAL_TaskResume(&(AudioFeedTask));
                if (Err != KAL_ERR_NONE) {
                    LOG_AUDIO_APP_NG("AmbaKAL_TaskResume failed %u ", Err, 0U);
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(&Rval);
}

/**
* delete feed task
* @param [in]  DecId decode id
*/
void AmbaADecFlow_FeedTaskDelete(ULONG DecId)
{
    UINT32 Err, Rval = CODEC_OK;
    ULONG  pDecHdlr;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        if (Rval == CODEC_OK) {
            Err = AmbaKAL_TaskTerminate(&(AudioFeedTask));
            if (Err != KAL_ERR_NONE) {
                LOG_AUDIO_APP_NG("AmbaKAL_TaskTerminate failed %u ", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == CODEC_OK) {
            Err = AmbaKAL_TaskDelete(&(AudioFeedTask));
            if (Err != KAL_ERR_NONE) {
                LOG_AUDIO_APP_NG("AmbaKAL_TaskDelete failed %u ", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == CODEC_OK) {
            Err = AmbaKAL_EventFlagDelete(&SvcAudioFeedFlag);
            if (Err != KAL_ERR_NONE) {
                LOG_AUDIO_APP_NG("AmbaKAL_EventFlagDelete failed %u ", Err, 0U);
                Rval = CODEC_ERROR_GENERAL_ERROR;
            }
        }
    }

    AmbaMisra_TouchUnused(&Rval);
}

/**
* start decoder
* @param [in]  DecId decode id
*/
void AmbaADecFlow_DecStart(ULONG DecId)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioDec_Start(&ADecInfo);
        LOG_AUDIO_APP_OK("AmbaAudioDec_Start, RtVal : %d ", RtVal, 0U);
    }
}

/**
* start output
* @param [in]  DecId decode id
* @param [in]  pCbAoutCtrl audio output control
*/
void AmbaADecFlow_OupStart(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32 RtVal;
    ULONG  pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioOup_Start(&ADecInfo, pCbAoutCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Start, RtVal : %d ", RtVal, 0U);
    }
}

/**
* stop output
* @param [in]  DecId decode id
* @param [in]  pCbAoutCtrl audio output control
*/
void AmbaADecFlow_OupStop(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioOup_Stop(&ADecInfo, 0U, pCbAoutCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Stop, RtVal : %d ", RtVal, 0U);
    }
}

/**
* pause output
* @param [in]  DecId decode id
* @param [in]  pCbAoutCtrl audio output control
*/
void AmbaADecFlow_OupPause(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioOup_Pause(&ADecInfo, pCbAoutCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Pause, RtVal : %d ", RtVal, 0U);
    }
}

/**
* resume output
* @param [in]  DecId decode id
* @param [in]  pCbAoutCtrl audio output control
*/
void AmbaADecFlow_OupResume(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioOup_Resume(&ADecInfo, pCbAoutCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Resume, RtVal : %d ", RtVal, 0U);
    }
}

/**
* AmbaADecFlow_Stop
* @param [in]  DecId Decode Id
* @param [in]  IsWaitAEos wait eos or not
* @param [in]  pCbAoutCtrl audio output control
* @return ErrorCode
*/
void AmbaADecFlow_Stop(ULONG DecId, UINT32 IsWaitAEos, AMBA_AOUT_CTRL_t pCbAoutCtrl)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        ADecInfo.pAdecHdlr   = pAdecHdlr;
        ADecInfo.pAoutHdlr   = pAoutHdlr;
        ADecInfo.pAdecCbHdlr = &AdecCbHdlr;
        ADecInfo.pAoutCbHdlr = &AoutCbHdlr;
        ADecInfo.I2SChannel  = (UINT8)g_AOutInfo.AoutInfo.HwIndex;

        RtVal = AmbaAudioDec_Stop(&ADecInfo, IsWaitAEos);
        LOG_AUDIO_APP_OK("AmbaAudioDec_Stop, RtVal : %d ", RtVal, 0U);

        RtVal = AmbaAudioOup_Stop(&ADecInfo, 0U, pCbAoutCtrl);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Stop, RtVal : %d ", RtVal, 0U);
    }
}

/**
* AmbaADecFlow_Delete
* @param [in]  DecId Decode Id
*/
void AmbaADecFlow_Delete(ULONG DecId)
{
    UINT32  RtVal;
    ULONG   pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        RtVal = AmbaAudio_DeMuxDelete(0);
        LOG_AUDIO_APP_OK("AmbaAudio_DeMuxDelete, RtVal: %d", RtVal, 0U);

        RtVal = AmbaAudio_DecBufDelete(Adec2AoutBufIdx);
        LOG_AUDIO_APP_OK("AmbaAdec2AoutBuf_Delete, RtVal: %d", RtVal, 0U);

        RtVal = AmbaAudioDec_Delete(pAdecHdlr, &AdecCbHdlr, AdecUseOneFrmCB);
        LOG_AUDIO_APP_OK("AmbaAudioDec_Delete, RtVal : %d ", RtVal, 0U);

        RtVal = AmbaAudioOup_Delete(pAoutHdlr, &AoutCbHdlr);
        LOG_AUDIO_APP_OK("AmbaAudioOup_Delete, RtVal : %d ", RtVal, 0U);

        RtVal = AmbaKAL_EventFlagDelete(&SvcAudioDecFlag);
        LOG_AUDIO_APP_OK("AmbaKAL_EventFlagDelete, RtVal : %d ", RtVal, 0U);

        IsUsed = 0U;
    }
}

/**
* AmbaADecFlow_UpdateBsBufRp
* @param [in]  DecId Decode Id
* @param [in]  pBsAddr Updated address
* @param [in]  UpdateSize Updated size
* @return ErrorCode
*/
UINT32 AmbaADecFlow_UpdateBsBufRp(ULONG DecId, UINT8 *pBsAddr, UINT32 UpdateSize)
{
    UINT32 Rval = CODEC_ERROR_ARG;
    ULONG  pDecHdlr;

    AmbaMisra_TypeCast(&pDecHdlr, &pAdecHdlr);

    if (DecId == pDecHdlr) {
        Rval = AmbaADEC_UpdateBsBufWp(pAdecHdlr, pBsAddr, UpdateSize);
    }

    return Rval;
}

/**
* AmbaADecFlow_WaitEvent
* @param [in]  Event
* @param [in]  pParam Wait paramater
* @return ErrorCode
*/
UINT32 AmbaADecFlow_WaitEvent(UINT32 Event, void *pParam)
{
    UINT32        Err = AUDIO_OK, Rval = AUDIO_OK, ActualFlags;
    const UINT32  *pTargetNum;

    AmbaMisra_TouchUnused(pParam);

    switch (Event) {
        case AMBA_ADEC_EVENT_OUTPUT:
            if (IsUsed == 1U) {
                AmbaMisra_TypeCast(&pTargetNum, &pParam);
                while ((g_AOutInfo.OutputCnt < *pTargetNum) && (Err == AUDIO_OK)) {
                    Err = AmbaKAL_EventFlagGet(&SvcAudioDecFlag, AUDIO_DEC_OUTPUT_ONE_FRAME,
                                                AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                                &ActualFlags, 5000U);
                    if (Err != AUDIO_OK) {
                        LOG_AUDIO_APP_NG("Wait Event AUDIO_DEC_OUTPUT_ONE_FRAME failed, line %u ", __LINE__, 0U);
                        Rval = AUDIO_ERROR_GENERAL_ERROR;
                    }
                }
            }
            break;
        default:
            LOG_AUDIO_APP_NG("Unknown event %u, line %u ", Event, __LINE__);
            Rval = AUDIO_ERROR_ARG ;
            break;
    }

    return Rval;
}
