/**
 *  @file SvcRecInfoPack.c
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
 *  @details svc information pack
 *
 */

#include "AmbaDef.h"
#include "AmbaFPD.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaShell.h"
#include "AmbaHDMI_Def.h"
#include "AmbaI2S_Def.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaUtility.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcBuffer.h"
#include "SvcVinSrc.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "SvcBufCalc.h"
#include "SvcDisplay.h"
#include "SvcRecMain.h"
#include "SvcTaskList.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "SvcTask.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaVfs.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAEncFlow.h"
#include "AmbaADecFlow.h"
#include "SvcUserPref.h"
#include "SvcRecInfoPack.h"
#include "SvcRecQpCtrl.h"
#include "SvcBRateCalc.h"
#include "SvcDrawStopwatch.h"
#include "SvcRecBlend.h"
#include "SvcDataGather.h"
#include "SvcAudInfoPack.h"
#include "SvcStgMonitor.h"
#include "SvcTimeLapse.h"
#include ".svc_autogen"
#if defined(CONFIG_AMBA_RTSP) && !defined(CONFIG_RTSP_LINUX)
#include "SvcRtsp.h"
#endif
#if defined(CONFIG_ICAM_USE_AMBA_DIRTDETECT)
#include "SvcDirtDetect.h"
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "SvcDrawDateTime.h"
#include "SvcDrawLogo.h"
#endif
#include "../../../cardv/platform.h"

#define SVC_LOG_RINO        "RINO"

static void FillRecSrc(const SVC_REC_MAIN_INFO_s *pInfo, const SVC_RES_CFG_s *pCfg)
{
    UINT32                       i, Rval, NumRsc;
    UINT32                       VBsSize = 0U, VRscSize = 0U;
    ULONG                        VBsBase = 0U, VRscBase = 0U;
#if defined(CONFIG_ICAM_AUDIO_USED)
    UINT32                       ABsSize = 0U, ARscSize = 0U;
    ULONG                        ABsBase = 0U, ARscBase = 0U;
#endif
#if defined(CONFIG_ICAM_DATA_GATHER_USED)
    UINT32                       DRscSize = 0U;
    ULONG                        DRscBase = 0U;
#endif
    SVC_REC_FWK_CFG_s            *pMainCfg;
    AMBA_RSC_USR_CFG_s           *pUsrCfg;
    const SVC_REC_STRM_s         *pRecCfg;

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VRSC_QU, &VRscBase, &VRscSize);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get memory\n", 0U, 0U);
    }
    VRscSize /= (UINT32)CONFIG_ICAM_MAX_REC_STRM;

#if defined(CONFIG_ICAM_AUDIO_USED)
    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_AUD_ENC_BS, &ABsBase, &ABsSize);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get Audio bitstream buffer/size\n", 0U, 0U);
    }

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_ARSC_QU, &ARscBase, &ARscSize);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get memory\n", 0U, 0U);
    }
    ARscSize /= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
#endif

#if defined(CONFIG_ICAM_DATA_GATHER_USED)
    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_DRSC_QU, &DRscBase, &DRscSize);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get memory\n", 0U, 0U);
    }
    DRscSize /= (UINT32)CONFIG_ICAM_MAX_REC_STRM;
#endif

    for (i = 0U; i < pCfg->RecNum; i++) {
        NumRsc = 0U;
        pRecCfg  = &(pCfg->RecStrm[i]);
        pMainCfg = &(pInfo->pFwkStrmCfg[i]);

        /* video */
        if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_VIDEO)) {

            Rval = SvcInfoPack_VEncBSBufRequest(i, &VBsBase, &VBsSize);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_RINO, "!!! fail to get Video bitstream buffer/size\n", 0U, 0U);
            }

            pUsrCfg = pMainCfg->RscCfgArr[NumRsc];

            pUsrCfg->SrcType        = AMBA_RSC_TYPE_VIDEO;
            pUsrCfg->TimeScale      = pRecCfg->RecSetting.FrameRate.TimeScale;
            pUsrCfg->NumUnitsInTick = pRecCfg->RecSetting.FrameRate.NumUnitsInTick;
            pUsrCfg->MemBase        = VRscBase + ((ULONG)i * (ULONG)VRscSize);
            pUsrCfg->MemSize        = VRscSize;
            pUsrCfg->BsBufBase      = VBsBase;
            pUsrCfg->BsBufSize      = VBsSize;
            pUsrCfg->FramePerFrag   = ((DOUBLE)pRecCfg->RecSetting.N * (DOUBLE)pRecCfg->RecSetting.IdrInterval) / (DOUBLE)pRecCfg->RecSetting.M;
            pUsrCfg->StreamID       = i;
            NumRsc++;
        }

#if defined(CONFIG_ICAM_AUDIO_USED)
        /* audio */
        if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_AUDIO)) {
            DOUBLE FramePerFrag;

            pUsrCfg = pMainCfg->RscCfgArr[NumRsc];

            pUsrCfg->SrcType        = AMBA_RSC_TYPE_AUDIO;
            pUsrCfg->TimeScale      = AENC_SAMPLE_FREQ;
            pUsrCfg->NumUnitsInTick = AENC_FRAME_SIZE;
            pUsrCfg->MemBase        = ARscBase + ((ULONG)i * (ULONG)ARscSize);
            pUsrCfg->MemSize        = ARscSize;
            pUsrCfg->BsBufBase      = ABsBase;
            pUsrCfg->BsBufSize      = ABsSize;
            pUsrCfg->StreamID       = i;

            FramePerFrag  = (DOUBLE)pRecCfg->RecSetting.N * (DOUBLE)pRecCfg->RecSetting.IdrInterval * (DOUBLE)pRecCfg->RecSetting.FrameRate.NumUnitsInTick * (DOUBLE)pUsrCfg->TimeScale;
            FramePerFrag  = FramePerFrag / ((DOUBLE)pRecCfg->RecSetting.M * (DOUBLE)pRecCfg->RecSetting.FrameRate.TimeScale * (DOUBLE)pUsrCfg->NumUnitsInTick);
            pUsrCfg->FramePerFrag = FramePerFrag;
            NumRsc++;

        }
#endif

#if defined(CONFIG_ICAM_DATA_GATHER_USED)
        /* data */
        if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_DATA)) {
            /* record source, audio */
            pUsrCfg = pMainCfg->RscCfgArr[NumRsc];

            pUsrCfg->SrcType      = AMBA_RSC_TYPE_DATA;
            pUsrCfg->SubType      = AMBA_RSC_DATA_SUBTYPE_CAN;
            pUsrCfg->MemBase      = DRscBase + ((ULONG)i * (ULONG)DRscSize);
            pUsrCfg->MemSize      = DRscSize;
            pUsrCfg->FramePerFrag = ((DOUBLE)pRecCfg->RecSetting.N * (DOUBLE)pRecCfg->RecSetting.IdrInterval) / (DOUBLE)pRecCfg->RecSetting.M;
            pUsrCfg->TimeScale      = pRecCfg->RecSetting.FrameRate.TimeScale;
            pUsrCfg->NumUnitsInTick = pRecCfg->RecSetting.FrameRate.NumUnitsInTick;
            pUsrCfg->StreamID       = i;

            NumRsc++;
        }
#endif

        pMainCfg->NumRsc = NumRsc;
    }
}

static void FillRecBox(const SVC_REC_MAIN_INFO_s *pInfo, const SVC_RES_CFG_s *pCfg)
{
    UINT32                        i, Rval, NumRbx;
    UINT32                        MemSize = 0U, RbxSize;
    ULONG                         MemBase = 0U;
    SVC_USER_PREF_s               *pPref;
    SVC_REC_FWK_CFG_s             *pMainCfg;
    AMBA_RBX_USR_CFG_s            *pUsrCfg;
    const SVC_REC_STRM_s          *pRecCfg;
#if defined(CONFIG_ICAM_AUDIO_USED)
    AMBA_AUDIO_ENC_CREATE_INFO_s  AencInfo = {0};

    SvcAudInfoPack_AENCInfo(&AencInfo);
#endif

    Rval = SvcUserPref_Get(&pPref);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "SvcUserPref_Get failed", Rval, 0U);
    }

    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_RBX, &MemBase, &MemSize);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get record box buffer/size\n", 0U, 0U);
    }

    for (i = 0U; i < pCfg->RecNum; i++) {
        pRecCfg = &(pCfg->RecStrm[i]);
        pMainCfg = &(pInfo->pFwkStrmCfg[i]);

        NumRbx = 0U;
        if (0U < pRecCfg->RecSetting.SrcBits) {
            if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_FILE | SVC_REC_DST_NMLEVT)) {
                pUsrCfg = pMainCfg->RbxCfgArr[NumRbx];
                pUsrCfg->BoxType     = AMBA_RBX_TYPE_MP4;
                pUsrCfg->SubType     = AMBA_SUB_TYPE_FMP4;
                pUsrCfg->IsTimeLapse = (0U < pRecCfg->RecSetting.TimeLapse) ? (UINT32)1U : (UINT32)0U;
                AmbaRbxMP4_EvalMemSize(pUsrCfg->SubType, &(pMainCfg->MiaInfo), &RbxSize);

                pUsrCfg->MemBase = MemBase;
                pUsrCfg->MemSize = RbxSize;
                MemBase += RbxSize;

                pUsrCfg->VdCoding         = AMBA_RSC_VID_SUBTYPE_H264;
                pUsrCfg->VdWidth          = pRecCfg->StrmCfg.Win.Width;
                pUsrCfg->VdHeight         = pRecCfg->StrmCfg.Win.Height;
                pUsrCfg->VdOrientation    = 0U;
                pUsrCfg->VdTimeScale      = pRecCfg->RecSetting.FrameRate.TimeScale;
                pUsrCfg->VdNumUnitsInTick = pRecCfg->RecSetting.FrameRate.NumUnitsInTick;

                pUsrCfg->AuCoding = AMBA_RSC_AUD_SUBTYPE_NONE;
                pUsrCfg->TExist   = 0U;

    #if defined(CONFIG_ICAM_AUDIO_USED)
                if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_AUDIO)) {
                    pUsrCfg->AuCoding         = AMBA_RSC_AUD_SUBTYPE_AAC;
                    pUsrCfg->AuBrate          = pPref->AudioAacBitRate;
                    pUsrCfg->AuVolume         = pPref->AudioVolume;
                    pUsrCfg->AuChannels       = AencInfo.ChannelNum;
                    pUsrCfg->AuSample         = AencInfo.SampleFreq;
                    pUsrCfg->AuWBitsPerSample = pPref->BitsPerSample;
                    pUsrCfg->AuFrameSize      = AencInfo.FrameSize;
                    pUsrCfg->AuFormat         = SVC_AUDIO_ENDIAN_L;
                }
    #endif
    #if defined(CONFIG_ICAM_DATA_GATHER_USED)
                /* data */
                if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_DATA)) {
                    pUsrCfg->TExist           = 1U;
                }
    #endif
    #if defined(CONFIG_ICAM_ENCRYPTION)
                pUsrCfg->IsEncrypt    = 1U;
    #else
                pUsrCfg->IsEncrypt    = 0U;
    #endif
                pUsrCfg->SplitTime    = pPref->FileSplitTimeMin;
                NumRbx++;
            }
        }
        pMainCfg->NumRbx = NumRbx;
    }
}

static void FillRecDst(const SVC_REC_MAIN_INFO_s *pInfo, const SVC_RES_CFG_s *pCfg)
{
    UINT32                i, Rval, NumRdt;
    UINT32                NmlSize = 0U, RdtSize = 0U, SlotSize = 0U, EmgSize = 0U;
    ULONG                 NmlBase = 0U, EmgBase = 0U;
    SVC_REC_FWK_CFG_s     *pMainCfg;
    AMBA_RDT_USR_CFG_s    *pUsrCfg;
    const SVC_REC_STRM_s  *pRecCfg;

    for (i = 0U; i < pCfg->RecNum; i++) {
        pRecCfg  = &(pCfg->RecStrm[i]);
        pMainCfg = &(pInfo->pFwkStrmCfg[i]);

        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_NMLEVT)) {
            if (NmlBase == 0U) {
                Rval = SvcBuffer_Request(SVC_BUFFER_SHARED,
                                         SMEM_PF0_ID_NMLEVT_REC,
                                         &NmlBase,
                                         &NmlSize);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_RINO, "!!! fail to get nml buffer/size\n", 0U, 0U);
                }
            }
        }

        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_EMGEVT)) {
            if (EmgBase == 0U) {
                Rval = SvcBuffer_Request(SVC_BUFFER_SHARED,
                                         SMEM_PF0_ID_EMGEVT_REC,
                                         &EmgBase,
                                         &EmgSize);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_RINO, "!!! fail to get emg buffer/size\n", 0U, 0U);
                }
            }
        }

        /* record destination */
        NumRdt = 0U;
        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_FILE)) {
            pUsrCfg = pMainCfg->RdtCfgArr[NumRdt];

            pUsrCfg->DstType = AMBA_RDT_TYPE_FILE;
            pUsrCfg->SubType = 0U;
            pUsrCfg->NumRsc  = pMainCfg->NumRsc;
            pUsrCfg->StreamID = i;
            NumRdt++;
        }

        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_NMLEVT)) {
            pUsrCfg = pMainCfg->RdtCfgArr[NumRdt];
            pUsrCfg->DstType = AMBA_RDT_TYPE_EVT;
            pUsrCfg->SubType = AMBA_RDT_STYPE_NML;
            AmbaRdtNmlEvt_EvalMemSize(i, AMBA_SUB_TYPE_FMP4, &(pMainCfg->MiaInfo), &RdtSize, &SlotSize);

            pUsrCfg->MemBase      = NmlBase;
            pUsrCfg->MemSize      = RdtSize;
            pUsrCfg->SlotSize     = SlotSize;
            pUsrCfg->TaskPriority = AMBA_RDT_NML_TASK_PRI;
            pUsrCfg->TaskCpuBits  = AMBA_RDT_NML_TASK_CPU_BITS;
            pUsrCfg->StreamID     = i;
            NmlBase += RdtSize;
            NumRdt++;
        }

        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_EMGEVT)) {
            static AMBA_RBX_USR_CFG_s RbxUsrCfg[CONFIG_ICAM_MAX_REC_STRM] = {0};

            pUsrCfg = pMainCfg->RdtCfgArr[NumRdt];
            pUsrCfg->DstType = AMBA_RDT_TYPE_EVT;
            pUsrCfg->SubType = AMBA_RDT_STYPE_EMG;
            AmbaRdtEmgEvt_EvalMemSize(&(pMainCfg->MiaInfo), CONFIG_ICAM_MAX_EMG_REC_TIME, &RdtSize);

            pUsrCfg->TaskPriority = AMBA_RDT_EMG_TASK_PRI;
            pUsrCfg->TaskCpuBits  = AMBA_RDT_EMG_TASK_CPU_BITS;
            pUsrCfg->MemBase      = EmgBase;
            pUsrCfg->MemSize      = RdtSize;
            pUsrCfg->MaxRecPeriod = CONFIG_ICAM_MAX_EMG_REC_TIME;
            pUsrCfg->RecEval      = pMainCfg->MiaInfo;
            pUsrCfg->pRbxUsrCfg   = &(RbxUsrCfg[i]);
            pUsrCfg->StreamID     = i;

            {
                AMBA_RBX_USR_CFG_s *pBoxCfg = pUsrCfg->pRbxUsrCfg;

                pBoxCfg->BoxType     = AMBA_RBX_TYPE_MP4;
                pBoxCfg->SubType     = AMBA_SUB_TYPE_FMP4;
                pBoxCfg->IsTimeLapse = (0U < pRecCfg->RecSetting.TimeLapse) ? (UINT32)1U : (UINT32)0U;
                pBoxCfg->MiaInfo     = pMainCfg->MiaInfo;

                pBoxCfg->VdCoding         = AMBA_RSC_VID_SUBTYPE_H264;
                pBoxCfg->VdWidth          = pRecCfg->StrmCfg.Win.Width;
                pBoxCfg->VdHeight         = pRecCfg->StrmCfg.Win.Height;
                pBoxCfg->VdOrientation    = 0U;
                pBoxCfg->VdTimeScale      = pRecCfg->RecSetting.FrameRate.TimeScale;
                pBoxCfg->VdNumUnitsInTick = pRecCfg->RecSetting.FrameRate.NumUnitsInTick;

                pBoxCfg->AuCoding = AMBA_RSC_AUD_SUBTYPE_NONE;
                pBoxCfg->TExist   = 0U;

            #if defined(CONFIG_ICAM_AUDIO_USED)
                if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_AUDIO)) {
                    AMBA_AUDIO_ENC_CREATE_INFO_s  AencInfo = {0};
                    SVC_USER_PREF_s               *pPref;

                    Rval = SvcUserPref_Get(&pPref);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_RINO, "SvcUserPref_Get failed", Rval, 0U);
                    }
                    SvcAudInfoPack_AENCInfo(&AencInfo);

                    pBoxCfg->AuCoding         = AMBA_RSC_AUD_SUBTYPE_AAC;
                    pBoxCfg->AuBrate          = pPref->AudioAacBitRate;
                    pBoxCfg->AuVolume         = pPref->AudioVolume;
                    pBoxCfg->AuChannels       = AencInfo.ChannelNum;
                    pBoxCfg->AuSample         = AencInfo.SampleFreq;
                    pBoxCfg->AuWBitsPerSample = pPref->BitsPerSample;
                    pBoxCfg->AuFrameSize      = AencInfo.FrameSize;
                    pBoxCfg->AuFormat         = SVC_AUDIO_ENDIAN_L;
                }
            #endif

            #if defined(CONFIG_ICAM_DATA_GATHER_USED)
                /* data */
                if (0U < CheckBits(pRecCfg->RecSetting.SrcBits, SVC_REC_SRC_DATA)) {
                    pBoxCfg->TExist           = 1U;
                }
            #endif

            #if defined(CONFIG_ICAM_ENCRYPTION)
                pBoxCfg->IsEncrypt    = 1U;
            #else
                pBoxCfg->IsEncrypt    = 0U;
            #endif
            }

            EmgBase += RdtSize;
            NumRdt++;
        }

        if (0U < CheckBits(pRecCfg->RecSetting.DestBits, SVC_REC_DST_NET)) {
            pUsrCfg = pMainCfg->RdtCfgArr[NumRdt];

            pUsrCfg->DstType      = AMBA_RDT_TYPE_NET;
            pUsrCfg->SubType      = AMBA_RDT_STYPE_RTSP;
            pUsrCfg->TaskPriority = AMBA_RDT_NET_TASK_PRI;
            pUsrCfg->TaskCpuBits  = AMBA_RDT_NET_TASK_CPU_BITS;
            pUsrCfg->StreamID     = i;
            NumRdt++;
        }
        pMainCfg->NumRdt = NumRdt;
    }
}

static void FillRecBlend(const SVC_RES_CFG_s *pCfg)
{
    UINT16                         StreamID;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    UINT32                         i, j;
    SVC_DRAW_DATETIME_INFO         DateTime;
    SVC_DRAW_LOGO_INFO             Logo;
#else    
    UINT32                         i, j, BufSize = 0U;
    ULONG                          BufBase = 0U;
    SVC_DRAW_STOPWATCH_INFO        Stopwatch = {0};
#endif    
    AMBA_DSP_VIDEO_BLEND_CONFIG_s  BlendConfig;
    const SVC_REC_STRM_s           *pRecCfg;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    SVC_USER_PREF_s                *pSvcUserPref = NULL;

    SvcUserPref_Get(&pSvcUserPref);
#endif

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STOPWATCH, &BufBase, &BufSize) != OK) {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get stopwatch buffer", 0U, 0U);
    }
    Stopwatch.BufBase = BufBase;
    Stopwatch.BufSize = BufSize;
#endif
    for (i = 0U; i < pCfg->RecNum; i++) {
        pRecCfg = &(pCfg->RecStrm[i]);
        StreamID = (UINT16)pRecCfg->RecSetting.RecId;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        /* SVC_REC_BLEND_BEHAVIOR: Alpha blending = 0U; CLUT = 1U */
        SvcDrawStopwatch_InfoGet(StreamID, &Stopwatch, SVC_REC_BLEND_BEHAVIOR);
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        SvcDrawDateTime_InfoGet(StreamID, pRecCfg->StrmCfg.MaxWin.Width, pRecCfg->StrmCfg.MaxWin.Height, &DateTime, 0U);
        SvcDrawLogo_InfoGet(StreamID, &Logo, 0U);
        for (j = 0; j < (UINT32)SVC_REC_BLEND_NUM; j++) {
            if (j == (UINT32)SVC_DRAW_DATETIME_IDX) {
                /* datetime */
                (void)AmbaWrap_memset(&BlendConfig, 0, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
                BlendConfig.BlendAreaIdx               = SVC_DRAW_DATETIME_IDX;
                BlendConfig.Enable                     = 1U;
                BlendConfig.EnableCLUT                 = 0U;
                BlendConfig.BlendYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                BlendConfig.BlendYuvBuf.BaseAddrY      = DateTime.DateTime.BaseAddrY;
                BlendConfig.BlendYuvBuf.BaseAddrUV     = DateTime.DateTime.BaseAddrUV;
                BlendConfig.BlendYuvBuf.Pitch          = DateTime.DateTime.Pitch;
                BlendConfig.BlendYuvBuf.Window.OffsetX = (pRecCfg->StrmCfg.MaxWin.Width - DateTime.DateTime.RealWidth) / 2;
                if (pSvcUserPref->WaterMarkPos == WATER_MARK_POS_BOTTOM) {
                    BlendConfig.BlendYuvBuf.Window.OffsetY = pRecCfg->StrmCfg.MaxWin.Height - 20 - DateTime.DateTime.Height;
                } else {
                    BlendConfig.BlendYuvBuf.Window.OffsetY = 20;
                }
                BlendConfig.BlendYuvBuf.Window.Width   = DateTime.DateTime.Width;
                BlendConfig.BlendYuvBuf.Window.Height  = DateTime.DateTime.Height;
                BlendConfig.AlphaBuf.BaseAddrY         = DateTime.DateTime.BaseAddrAY;
                BlendConfig.AlphaBuf.BaseAddrUV        = DateTime.DateTime.BaseAddrAUV;
                BlendConfig.AlphaBuf.Pitch             = DateTime.DateTime.Pitch;
                BlendConfig.AlphaBuf.Window.OffsetX    = (pRecCfg->StrmCfg.MaxWin.Width - DateTime.DateTime.RealWidth) / 2;
                if (pSvcUserPref->WaterMarkPos == WATER_MARK_POS_BOTTOM) {
                    BlendConfig.AlphaBuf.Window.OffsetY    = pRecCfg->StrmCfg.MaxWin.Height - 20 - DateTime.DateTime.Height;
                } else {
                    BlendConfig.AlphaBuf.Window.OffsetY    = 20;
                }
                BlendConfig.AlphaBuf.Window.Width      = DateTime.DateTime.Width;
                BlendConfig.AlphaBuf.Window.Height     = DateTime.DateTime.Height;
                SvcRecBlend_Config(StreamID, SVC_DRAW_DATETIME_IDX, &BlendConfig);
                SvcRecBlend_RegUpdBufCB(StreamID, SVC_DRAW_DATETIME_IDX, SvcDrawDateTime_BufUpdate);
            } else if (j == (UINT32)SVC_DRAW_LOGO_IDX) {
                /* logo */
                (void)AmbaWrap_memset(&BlendConfig, 0, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
                BlendConfig.BlendAreaIdx               = SVC_DRAW_LOGO_IDX;
                BlendConfig.Enable                     = 1U;
                BlendConfig.EnableCLUT                 = 0U;
                BlendConfig.BlendYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                BlendConfig.BlendYuvBuf.BaseAddrY      = Logo.Logo.BaseAddrY;
                BlendConfig.BlendYuvBuf.BaseAddrUV     = Logo.Logo.BaseAddrUV;
                BlendConfig.BlendYuvBuf.Pitch          = Logo.Logo.Pitch;
                BlendConfig.BlendYuvBuf.Window.OffsetX = 16;
                BlendConfig.BlendYuvBuf.Window.OffsetY = 16;
                BlendConfig.BlendYuvBuf.Window.Width   = Logo.Logo.Width;
                BlendConfig.BlendYuvBuf.Window.Height  = Logo.Logo.Height;
                BlendConfig.AlphaBuf.BaseAddrY         = Logo.Logo.BaseAddrAY;
                BlendConfig.AlphaBuf.BaseAddrUV        = Logo.Logo.BaseAddrAUV;
                BlendConfig.AlphaBuf.Pitch             = Logo.Logo.Pitch;
                BlendConfig.AlphaBuf.Window.OffsetX    = 16;
                BlendConfig.AlphaBuf.Window.OffsetY    = 16;
                BlendConfig.AlphaBuf.Window.Width      = Logo.Logo.Width;
                BlendConfig.AlphaBuf.Window.Height     = Logo.Logo.Height;
                SvcRecBlend_Config(StreamID, SVC_DRAW_LOGO_IDX, &BlendConfig);
                SvcRecBlend_RegUpdBufCB(StreamID, SVC_DRAW_LOGO_IDX, SvcDrawLogo_BufUpdate);
            }
        }
#else

        for (j = 0; j < (UINT32)SVC_REC_BLEND_NUM; j++) {
            if (j == (UINT32)SVC_DRAW_STOPWATCH_HOUR_IDX) {
                /* hour */
                AmbaSvcWrap_MisraMemset(&BlendConfig, 0, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
                BlendConfig.BlendAreaIdx               = SVC_DRAW_STOPWATCH_HOUR_IDX;
                BlendConfig.Enable                     = 1U;
                BlendConfig.EnableCLUT                 = SVC_REC_BLEND_BEHAVIOR;/* SVC_REC_BLEND_BEHAVIOR: Alpha blending = 0U; CLUT = 1U */
                BlendConfig.BlendYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                BlendConfig.BlendYuvBuf.BaseAddrY      = Stopwatch.Hour.BaseAddrY;
                BlendConfig.BlendYuvBuf.BaseAddrUV     = Stopwatch.Hour.BaseAddrUV;
                BlendConfig.BlendYuvBuf.Pitch          = Stopwatch.Hour.Pitch;
                BlendConfig.BlendYuvBuf.Window.OffsetX = ((pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width) - Stopwatch.Min.Width) - Stopwatch.Hour.Width;
                BlendConfig.BlendYuvBuf.Window.OffsetY = 0U;
                BlendConfig.BlendYuvBuf.Window.Width   = Stopwatch.Hour.Width;
                BlendConfig.BlendYuvBuf.Window.Height  = Stopwatch.Hour.Height;
                BlendConfig.AlphaBuf.BaseAddrY         = Stopwatch.Hour.BaseAddrAY;
                BlendConfig.AlphaBuf.BaseAddrUV        = Stopwatch.Hour.BaseAddrAUV;
                BlendConfig.AlphaBuf.Pitch             = Stopwatch.Hour.Pitch;
                BlendConfig.AlphaBuf.Window.OffsetX    = ((pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width) - Stopwatch.Min.Width) - Stopwatch.Hour.Width;
                BlendConfig.AlphaBuf.Window.OffsetY    = 0U;
                BlendConfig.AlphaBuf.Window.Width      = Stopwatch.Hour.Width;
                BlendConfig.AlphaBuf.Window.Height     = Stopwatch.Hour.Height;
                SvcRecBlend_Config(StreamID, SVC_DRAW_STOPWATCH_HOUR_IDX, &BlendConfig);
                SvcRecBlend_RegUpdBufCB(StreamID, SVC_DRAW_STOPWATCH_HOUR_IDX, SvcDrawStopwatch_BufUpdate);

            } else if (j == (UINT32)SVC_DRAW_STOPWATCH_MIN_IDX) {
                /* min */
                AmbaSvcWrap_MisraMemset(&BlendConfig, 0, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
                BlendConfig.BlendAreaIdx               = SVC_DRAW_STOPWATCH_MIN_IDX;
                BlendConfig.Enable                     = 1U;
                BlendConfig.EnableCLUT                 = SVC_REC_BLEND_BEHAVIOR;/* SVC_REC_BLEND_BEHAVIOR: Alpha blending = 0U; CLUT = 1U */
                BlendConfig.BlendYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                BlendConfig.BlendYuvBuf.BaseAddrY      = Stopwatch.Min.BaseAddrY;
                BlendConfig.BlendYuvBuf.BaseAddrUV     = Stopwatch.Min.BaseAddrUV;
                BlendConfig.BlendYuvBuf.Pitch          = Stopwatch.Min.Pitch;
                BlendConfig.BlendYuvBuf.Window.OffsetX = (pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width) - Stopwatch.Min.Width;
                BlendConfig.BlendYuvBuf.Window.OffsetY = 0U;
                BlendConfig.BlendYuvBuf.Window.Width   = Stopwatch.Min.Width;
                BlendConfig.BlendYuvBuf.Window.Height  = Stopwatch.Min.Height;
                BlendConfig.AlphaBuf.BaseAddrY         = Stopwatch.Min.BaseAddrAY;
                BlendConfig.AlphaBuf.BaseAddrUV        = Stopwatch.Min.BaseAddrAUV;
                BlendConfig.AlphaBuf.Pitch             = Stopwatch.Min.Pitch;
                BlendConfig.AlphaBuf.Window.OffsetX    = (pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width) - Stopwatch.Min.Width;
                BlendConfig.AlphaBuf.Window.OffsetY    = 0U;
                BlendConfig.AlphaBuf.Window.Width      = Stopwatch.Min.Width;
                BlendConfig.AlphaBuf.Window.Height     = Stopwatch.Min.Height;
                SvcRecBlend_Config(StreamID, SVC_DRAW_STOPWATCH_MIN_IDX, &BlendConfig);
                SvcRecBlend_RegUpdBufCB(StreamID, SVC_DRAW_STOPWATCH_MIN_IDX, SvcDrawStopwatch_BufUpdate);

            } else if (j == (UINT32)SVC_DRAW_STOPWATCH_SEC_IDX) {
                /* sec */
                AmbaSvcWrap_MisraMemset(&BlendConfig, 0, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s));
                BlendConfig.BlendAreaIdx               = SVC_DRAW_STOPWATCH_SEC_IDX;
                BlendConfig.Enable                     = 1U;
                BlendConfig.EnableCLUT                 = SVC_REC_BLEND_BEHAVIOR;/* SVC_REC_BLEND_BEHAVIOR: Alpha blending = 0U; CLUT = 1U */
                BlendConfig.BlendYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                BlendConfig.BlendYuvBuf.BaseAddrY      = Stopwatch.Sec.BaseAddrY;
                BlendConfig.BlendYuvBuf.BaseAddrUV     = Stopwatch.Sec.BaseAddrUV;
                BlendConfig.BlendYuvBuf.Pitch          = Stopwatch.Sec.Pitch;
                BlendConfig.BlendYuvBuf.Window.OffsetX = pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width;
                BlendConfig.BlendYuvBuf.Window.OffsetY = 0U;
                BlendConfig.BlendYuvBuf.Window.Width   = Stopwatch.Sec.Width;
                BlendConfig.BlendYuvBuf.Window.Height  = Stopwatch.Sec.Height;
                BlendConfig.AlphaBuf.BaseAddrY         = Stopwatch.Sec.BaseAddrAY;
                BlendConfig.AlphaBuf.BaseAddrUV        = Stopwatch.Sec.BaseAddrAUV;
                BlendConfig.AlphaBuf.Pitch             = Stopwatch.Sec.Pitch;
                BlendConfig.AlphaBuf.Window.OffsetX    = pRecCfg->StrmCfg.MaxWin.Width - Stopwatch.Sec.Width;
                BlendConfig.AlphaBuf.Window.OffsetY    = 0U;
                BlendConfig.AlphaBuf.Window.Width      = Stopwatch.Sec.Width;
                BlendConfig.AlphaBuf.Window.Height     = Stopwatch.Sec.Height;
                SvcRecBlend_Config(StreamID, SVC_DRAW_STOPWATCH_SEC_IDX, &BlendConfig);
                SvcRecBlend_RegUpdBufCB(StreamID, SVC_DRAW_STOPWATCH_SEC_IDX, SvcDrawStopwatch_BufUpdate);

            } else {
                /* nothing */
            }
        }
#endif
    }
}

#if defined(CONFIG_AMBA_RTSP) && !defined(CONFIG_RTSP_LINUX)
static void FillRtspService(void)
{
    UINT32 Rval;
    UINT32 RtspServiceBsSize;
    ULONG  RtspServiceBs;

    Rval = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_RTSP_SERVICE, &RtspServiceBs, &RtspServiceBsSize);
    if (Rval == SVC_OK) {
        SvcRtsp_SetServiceBuf(RtspServiceBs, RtspServiceBsSize);
    } else {
        SvcLog_NG(SVC_LOG_RINO, "!!! fail to get rtsp service buffer/size\n", 0U, 0U);
    }
}
#endif

static void FillTimeLapse(const SVC_REC_MAIN_INFO_s *pInfo, const SVC_RES_CFG_s *pCfg)
{
#if defined(CONFIG_ICAM_TIMELAPSE_USED)
    UINT32                  Size = 0U, Err, Rval = SVC_OK, i, Bits = 0U, Enable = 0U;
    ULONG                   Addr = 0U;
    SVC_TLAPSE_TASK_CFG_s   *pTLapsCfg;
    SVC_TLAPSE_CAP_CFG_s    UsrCfg;

    AmbaMisra_TouchUnused(&Rval);

    for (i = 0U; i < pCfg->RecNum; i++) {
        if (0U < pCfg->RecStrm[i].RecSetting.TimeLapse) {
            Enable = 1U;
            break;
        }
    }

    if (Enable == 1U) {
        if (Rval == SVC_OK) {
            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_TIMELAPSE, &Addr, &Size);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_RINO, "!!! fail to get time-lapse buffer/size\n", 0U, 0U);
                Rval = SVC_NG;
            }
        }

        if (Rval == SVC_OK) {
            SvcTimeLapse_TaskCfgGet(&pTLapsCfg);

            pTLapsCfg->TaskPriority     = SVC_REC_TLPS_TASK_PRI;
            pTLapsCfg->TaskCpuBits      = SVC_REC_TLPS_TASK_CPU_BITS;
            pTLapsCfg->CapBufAddr       = Addr;
            pTLapsCfg->CapBufSize       = Size;
        }

        for (i = 0U; i < pCfg->RecNum; i++) {
            if (0U < pCfg->RecStrm[i].RecSetting.TimeLapse) {
                pInfo->pFwkStrmCfg[i].IsTimeLapse = 1U;
                Bits |= ((UINT32)1U << i);
            } else {
                pInfo->pFwkStrmCfg[i].IsTimeLapse = 0U;
            }
        }

        if (Bits > 0UL) {
            UsrCfg.RecStreamBits = Bits;
            UsrCfg.CapInterval   = 2000U;
            SvcTimeLapse_CapCfg(&UsrCfg);
        }
    }
#else
    AmbaMisra_TouchUnused(&pInfo);
    AmbaMisra_TouchUnused(&pCfg);
#endif
}

static UINT8 CalcTileNum(UINT32 Width)
{
    UINT8  TileNum = 0U, CoreNum;

    CoreNum = (UINT8)CONFIG_ICAM_DSP_ENC_CORE_NUM;
    #if defined(CONFIG_ICAM_ENC_1CORE_MAX_W)
    if (Width <= (UINT32)CONFIG_ICAM_ENC_1CORE_MAX_W) {
        CoreNum = 1U;
    }
    #endif

    AmbaMisra_TouchUnused(&CoreNum);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    {
        #define CTB_SIZE (32U) /* ctb size is 32 * 32 */
        UINT32 WidthCtbs = GetRoundUpValU32(Width, CTB_SIZE);

        /* Rule for CV5X:
               set tile num as MAX by default,
               only set it as MIN when (WidthCtbs%MAX != 0) && (WidthCtbs%MIN == 0) */

        TileNum = (UINT8)SVC_ENC_HEVC_MAX_TILE;

        if (((WidthCtbs % SVC_ENC_HEVC_MAX_TILE) > 0U) &&
            ((WidthCtbs % SVC_ENC_HEVC_MAX_TILE) < SVC_ENC_HEVC_MIN_TILE) &&
            ((WidthCtbs % SVC_ENC_HEVC_MIN_TILE) == 0U)) {
            TileNum = (UINT8)SVC_ENC_HEVC_MIN_TILE;
        }
    }
#else
    {
        /* Rule for CV2X:
               set tile num according to encode stream width */
        if (SVC_ENC_HEVC_W_LLMT < Width) {
            TileNum = (UINT8)SVC_ENC_HEVC_MAX_TILE;
        } else {
            TileNum = (UINT8)SVC_ENC_HEVC_MIN_TILE;
        }
    }
#endif

    TileNum *= CoreNum;

    return TileNum;
}

/**
 * Media info configuration getting function
 * @param [in] StrmIdx stream index
 * @param [out] pMiaInfo pointer to media info
 */
void SvcInfoPack_MiaInfoConfig(UINT32 StrmIdx, AMBA_REC_EVAL_s *pMiaInfo)
{
    UINT32                        Rval, CoreNum;
    const SVC_RES_CFG_s           *pCfg = SvcResCfg_Get();
    const SVC_REC_STRM_s          *pEncCfg;
    SVC_USER_PREF_s               *pPref;
#if defined(CONFIG_ICAM_AUDIO_USED)
    AMBA_AUDIO_ENC_CREATE_INFO_s  AencInfo = {0};

    SvcAudInfoPack_AENCInfo(&AencInfo);
#endif
    pEncCfg = &(pCfg->RecStrm[StrmIdx]);

    Rval = SvcUserPref_Get(&pPref);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "SvcUserPref_Get failed %u", Rval, 0U);
    }

    Rval = AmbaWrap_memset(pMiaInfo, 0, sizeof(AMBA_REC_EVAL_s));
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_RINO, "AmbaWrap_memset failed %u", Rval, 0U);
    }

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    pMiaInfo->FileSplitTime = pPref->FileSplitTimeSeconds;
    SvcLog_OK(SVC_LOG_RINO, "pMiaInfo->FileSplitTime[%u]=%u second(s)", StrmIdx, pMiaInfo->FileSplitTime);
#else
    pMiaInfo->FileSplitTime = pPref->FileSplitTimeMin;
#endif

    pMiaInfo->IsTimeLapse   = (0U < pEncCfg->RecSetting.TimeLapse) ? (UINT32)1U : (UINT32)0U;

    CoreNum = (UINT32)CONFIG_ICAM_DSP_ENC_CORE_NUM;
    AmbaMisra_TouchUnused(&CoreNum);
    if (1U < CoreNum) {
        pMiaInfo->IsHierGop = 1U;
    } else {
        pMiaInfo->IsHierGop = 0U;
    }
    /* video */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    if (pEncCfg->RecSetting.BitRate != 0U) {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = pEncCfg->RecSetting.BitRate;
    } else {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = SvcBRateCalc_GetMaxBRate(StrmIdx);
    }
#else
    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = SvcBRateCalc_GetMaxBRate(StrmIdx);
#endif

    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate  = GetRoundUpValU32(pEncCfg->RecSetting.FrameRate.TimeScale,
                                                                     pEncCfg->RecSetting.FrameRate.NumUnitsInTick);
    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].FrameTick = pEncCfg->RecSetting.FrameRate.NumUnitsInTick;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_M]     = pEncCfg->RecSetting.M;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N]     = pEncCfg->RecSetting.N;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT] = pEncCfg->RecSetting.IdrInterval;

    if (pEncCfg->RecSetting.NumSlice > 0U) {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_SLICE] = pEncCfg->RecSetting.NumSlice;
    } else {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_SLICE] = 1U;
    }

    if (pEncCfg->RecSetting.NumTile > 0U) {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_TILE] = pEncCfg->RecSetting.NumTile;
    } else {
        pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_TILE] = CalcTileNum(pEncCfg->StrmCfg.MaxWin.Width);
    }

#if defined(CONFIG_ICAM_AUDIO_USED)
    /* audio */
    pMiaInfo->MiaCfg[AMBA_REC_MIA_AUDIO].BitRate   = AencInfo.SampleFreq * AencInfo.ChannelNum * \
                                                    AencInfo.SampleResolution;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_AUDIO].FrameRate = GetRoundUpValU32(AencInfo.SampleFreq,
                                                    AencInfo.FrameSize);
    pMiaInfo->MiaCfg[AMBA_REC_MIA_AUDIO].FrameTick = AencInfo.FrameSize;
#endif
    /* data */
    pMiaInfo->MiaCfg[AMBA_REC_MIA_DATA].FrameRate = pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_DATA].FrameTick = pMiaInfo->MiaCfg[AMBA_REC_MIA_VIDEO].FrameTick;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_DATA].BitRate = CONFIG_AMBA_REC_DRSC_TEXTLEN;
    pMiaInfo->MiaCfg[AMBA_REC_MIA_DATA].BitRate *= (8U * pMiaInfo->MiaCfg[AMBA_REC_MIA_DATA].FrameRate);
}

/**
 * Recoder configuration getting function
 * @param [out] pInfo pointer to recorder configuration info
 */
void SvcInfoPack_RecConfig(const SVC_REC_MAIN_INFO_s *pInfo)
{
    UINT32                    i, Src = 0U, Rval, AEncBits = 0U;
    SVC_REC_FWK_CFG_s         *pMainCfg;
    SVC_USER_PREF_s           *pSvcUserPref;
    SVC_STG_MONI_DRIVE_INFO_s DriveInfo;
    const SVC_RES_CFG_s       *pCfg = SvcResCfg_Get();
    const SVC_REC_STRM_s      *pRec;

    /* fill record config */
    *(pInfo->pNumStrm)          = pCfg->RecNum;
    *(pInfo->pQpCtrlPriority)   = SVC_REC_QPCTRL_TASK_PRI;
    *(pInfo->pQpCtrlCpuBits)    = SVC_REC_QPCTRL_TASK_CPU_BITS;
    *(pInfo->pRecBlendPriority) = SVC_REC_BLEND_TASK_PRI;
    *(pInfo->pRecBlendCpuBits)  = SVC_REC_BLEND_TASK_CPU_BITS;
    *(pInfo->pRecMonPriority)   = SVC_REC_MON_TASK_PRI;
    *(pInfo->pRecMonCpuBits)    = SVC_REC_MON_TASK_CPU_BITS;
    *(pInfo->pBitsCmpPriority)  = SVC_REC_BITSCMP_TASK_PRI;
    *(pInfo->pBitsCmpCpuBits)   = SVC_REC_BITSCMP_TASK_CPU_BITS;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    *(pInfo->pRecMVPriority)    = SVC_REC_MV_TASK_PRI;
    *(pInfo->pRecMVCpuBits)     = SVC_REC_MV_TASK_CPU_BITS;
#if defined(CONFIG_ICAM_USE_AMBA_DIRTDETECT)
    *(pInfo->ppfnMVNotify)      = SvcDirtDetect_MVDataCB;
#else
    *(pInfo->ppfnMVNotify)      = NULL;
#endif
#endif

    if (SvcUserPref_Get(&pSvcUserPref) != SVC_OK) {
        SvcLog_NG(SVC_LOG_RINO, "## fail to SvcUserPref_Get()", 0U, 0U);
    }

    for (i = 0U; i < pCfg->RecNum; i++) {
        pMainCfg = &(pInfo->pFwkStrmCfg[i]);
        pRec = &(pCfg->RecStrm[i]);

        /* fill media info */
        SvcInfoPack_MiaInfoConfig(i, &(pMainCfg->MiaInfo));

        /* assign record master priority and cpu_bits */
        pMainCfg->RmaPriority = SVC_REC_MASTER_TASK_PRI;
        pMainCfg->RmaCpuBits  = SVC_REC_MASTER_TASK_CPU_BITS;
        pMainCfg->IsXcode     = 0U;
        pMainCfg->IsTimeLapse = 0U;

        if (0U < CheckBits(pRec->RecSetting.SrcBits, SVC_REC_SRC_AUDIO)) {
            if (pRec->RecSetting.AEncBits > 0U) {
                pMainCfg->AEncBits = pRec->RecSetting.AEncBits;
            } else {
                /* we use AudEncoder0 in default */
                pMainCfg->AEncBits = 0x1U;
            }
        } else {
            pMainCfg->AEncBits = 0U;
        }
        AEncBits |= pMainCfg->AEncBits;

        /* if the stream is from decoder or yuvinput, only need to check the last channel */
        {
            Rval = SvcResCfg_GetFovSrc(pRec->StrmCfg.ChanCfg[pRec->StrmCfg.NumChan - 1U].FovId, &Src);
            if (SVC_OK == Rval) {
                if ((Src == SVC_VIN_SRC_MEM_DEC) || (Src == SVC_VIN_SRC_MEM_YUV420) || (Src == SVC_VIN_SRC_MEM_YUV422)) {
                    pMainCfg->IsXcode = 1U;
                }
            }
        }

        /* fill window */
        pMainCfg->EncWin = pRec->StrmCfg.MaxWin;

        /* fill storage drive */
        if (pRec->RecSetting.UseAuxStg == 0U) {
            /* main storage */
            if (pSvcUserPref->UsbClass == SVC_STG_USBH_CLASS_MSC) {
                if (SvcStgMonitor_GetDriveInfo('I', &DriveInfo) == SVC_OK) {
                    if (DriveInfo.IsExist == 1U) {
                        pMainCfg->Drive = 'I';
                    }
                }
            } else {
                pMainCfg->Drive = pSvcUserPref->MainStgDrive[0];
            }
        } else {
            /* auxiliary storage */
        #if defined(CONFIG_ICAM_AUX_STG)
            const char AuxStgDrive[2] = CONFIG_ICAM_AUX_STG_DRV;

            pMainCfg->Drive = AuxStgDrive[0];
        #else
            pMainCfg->Drive = pSvcUserPref->MainStgDrive[0];
            SvcLog_NG(SVC_LOG_RINO, "## Please enable CONFIG_ICAM_AUX_STG for auxiliary storage", 0U, 0U);
        #endif
        }
    }

    if (AEncBits > 0U) {
        UINT16  IdxArr[(UINT32)(CONFIG_AENC_NUM)];
        UINT32  NumAEnc, MaxAEncBits = 0U;

        for (i = 0U; i < (UINT32)(CONFIG_AENC_NUM); i++) {
            MaxAEncBits = SetBits(MaxAEncBits, ((UINT32)1U << i));
        }

        if (AEncBits > MaxAEncBits) {
            SvcLog_NG(SVC_LOG_RINO, "Invalid audio encoder bits (%u/%u)", AEncBits, MaxAEncBits);
        }

        SvcUtil_BitsToArr((UINT32)(CONFIG_AENC_NUM), AEncBits, &NumAEnc, IdxArr);

        *(pInfo->pNumAEnc) = NumAEnc;
    }

    /* record source */
    FillRecSrc(pInfo, pCfg);
    /* record box */
    FillRecBox(pInfo, pCfg);
    /* record dst */
    FillRecDst(pInfo, pCfg);

    /* rec blend: draw stopwatch */
    FillRecBlend(pCfg);

    /* time-lapse */
    FillTimeLapse(pInfo, pCfg);

#if defined(CONFIG_AMBA_RTSP) && !defined(CONFIG_RTSP_LINUX)
    FillRtspService();
#endif
}

/**
 * DSP encoder configuration getting function
 * @param [out] pInfo pointer to DSP encoder info
 */
void SvcInfoPack_EncConfig(const SVC_ENC_INFO_s *pInfo)
{
    UINT32                            i, Rval, Size, Bit = 0x01U, RecNum = 0U;
    ULONG                             Base;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    UINT32                            MVSize = 0U, MVUnit;
    ULONG                             MVOff = 0U;
#endif
    const SVC_RES_CFG_s               *pCfg = SvcResCfg_Get();
    const SVC_REC_STRM_s              *pRec;
    SVC_DISP_INFO_s                   DispInfo;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  *pEnc;
    AMBA_DSP_VIDEO_ENC_VUI_s          *pVui;

    SvcDisplay_InfoGet(&DispInfo);
    for (i = 0; i < pCfg->RecNum; i++) {
        if ((pCfg->RecBits & (UINT32)(Bit << i)) == 0U) {
            continue;
        }
        pRec = &(pCfg->RecStrm[i]);

        pEnc = &(pInfo->pStrmCfg[RecNum]);
        Rval = AmbaWrap_memset(pEnc, 0, sizeof(AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_RINO, "AmbaWrap_memset failed %u", Rval, 0U);
        }

#if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
        if (0U < pRec->RecSetting.MVInfoFlag) {
            /* We just support MV for HEVC. Force coding format to HEVC */
            pEnc->CodingFmt = AMBA_DSP_ENC_CODING_TYPE_H265;
        } else {
            pEnc->CodingFmt = AMBA_DSP_ENC_CODING_TYPE_H264;
        }
#else
        pEnc->CodingFmt = AMBA_DSP_ENC_CODING_TYPE_H264;
#endif

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
        AmbaMisra_TouchUnused(&DispInfo);
        pEnc->SrcYuvStreamId = (UINT16)(RecNum);
#else
        pEnc->SrcYuvStreamId = (UINT16)(RecNum + *(DispInfo.pNumDisp));
#endif
        pEnc->Window.Width   = pRec->StrmCfg.MaxWin.Width;
        pEnc->Window.Height  = pRec->StrmCfg.MaxWin.Height;
        pEnc->FrameRate      = pRec->RecSetting.FrameRate;
        pEnc->Rotate         = pRec->RecSetting.Rotate;
        if (0U < pRec->RecSetting.TimeLapse) {
            pEnc->IntervalCapture = 1U;
            if (0U < (pRec->RecSetting.TimeLapse & SVC_TLS_BIT_FRC_SIMGOP)) {
                pEnc->IntervalCapture++;
            }
        } else {
            pEnc->IntervalCapture = 0U;
        }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        if (pRec->RecSetting.MVInfoFlag == 0) {
            UINT16 AlignWidth = GetRoundUpValU16(pEnc->Window.Width, 16U) * 16U;
            UINT16 AlignHeight = GetRoundUpValU16(pEnc->Window.Height, 16U) * 16U;

            pEnc->EncConfig.FrameCrop.CroppingFlag = 1;
            pEnc->EncConfig.FrameCrop.CropLeftOffset = (AlignWidth - pEnc->Window.Width) / 2 / 2;
            pEnc->EncConfig.FrameCrop.CropRightOffset = (AlignWidth - pEnc->Window.Width) / 2 - pEnc->EncConfig.FrameCrop.CropLeftOffset;
            pEnc->EncConfig.FrameCrop.CropTopOffset = 0;
            pEnc->EncConfig.FrameCrop.CropBottomOffset = (AlignHeight - pEnc->Window.Height) / 2;
        }
#endif

        pEnc->EncConfig.ProfileIDC = 100;
        pEnc->EncConfig.IsCabac = 1;
        pEnc->EncConfig.NumPRef = 1;
        pEnc->EncConfig.NumBRef = 2;

        if (0U < pRec->RecSetting.NumTile) {
            pEnc->EncConfig.NumTile = (UINT8)pRec->RecSetting.NumTile;
        } else {
            pEnc->EncConfig.NumTile = CalcTileNum(pRec->StrmCfg.Win.Width);
        }

        pEnc->EncConfig.NumSlice = 1U;
        if (0U < pRec->RecSetting.NumSlice) {
            pEnc->EncConfig.NumSlice = (UINT8)pRec->RecSetting.NumSlice;
        }

        pEnc->EncConfig.GOPCfg.GOPStruct = AMBA_DSP_VIDEO_ENC_GOP_SIMPLE;
        if (0U < pRec->RecSetting.GOPStruct) {
            pEnc->EncConfig.GOPCfg.GOPStruct = (UINT8)pRec->RecSetting.GOPStruct;
        }

        pEnc->EncConfig.GOPCfg.M           = (UINT8)pRec->RecSetting.M;
        pEnc->EncConfig.GOPCfg.N           = (UINT16)pRec->RecSetting.N;
        pEnc->EncConfig.GOPCfg.IdrInterval = pRec->RecSetting.IdrInterval;

        pEnc->EncConfig.BitRateCfg.BitRateCtrl = AMBA_DSP_BRC_SMART_VBR;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        if (pRec->RecSetting.BitRate != 0U) {
            SvcBRateCalc_Set(pRec->RecSetting.RecId, SVC_BR_CALC_MODE_ASSIGN, pRec->RecSetting.BitRate);
            pEnc->EncConfig.BitRateCfg.BitRate = pRec->RecSetting.BitRate;
        } else {
            pEnc->EncConfig.BitRateCfg.BitRate = SvcBRateCalc_Get(pRec->RecSetting.RecId);
        }
#else
        pEnc->EncConfig.BitRateCfg.BitRate     = SvcBRateCalc_Get(pRec->RecSetting.RecId);
#endif

#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        pEnc->EncConfig.BitRateCfg.QualityLevel = 0x10;
#endif

        pEnc->EncConfig.IntraRefreshCtrl.IntraRefreshCycle = (UINT8)pRec->RecSetting.IRCycle;

        /* qp ctrl */
        if (pEnc->EncConfig.BitRateCfg.BitRateCtrl == AMBA_DSP_BRC_SMART_VBR) {
            SvcRecQpCtrlCfg((UINT16)pRec->RecSetting.RecId, SVC_REC_QP_CTRL_ENABLE, pEnc->EncConfig.BitRateCfg.BitRate);
        } else {
            SvcRecQpCtrlCfg((UINT16)pRec->RecSetting.RecId, SVC_REC_QP_CTRL_DISABLE, pEnc->EncConfig.BitRateCfg.BitRate);
        }

        pEnc->EncConfig.DeblockFilter.EnableLoopFilter = 1;
        /* VUI */
        pVui = &(pEnc->EncConfig.Vui);
        pVui->VuiEnable                          = 1U;
        pVui->AspectRatioInfoPresentFlag         = 1U;
        pVui->OverscanInfoPresentFlag            = 0U;
        pVui->OverscanAppropriateFlag            = 1U;
        pVui->VideoSignalTypePresentFlag         = 1U;
        pVui->VideoFullRangeFlag                 = 1U;
        pVui->ColourDescriptionPresentFlag       = 0U;
        pVui->ChromaLocInfoPresentFlag           = 0U;
        pVui->TimingInfoPresentFlag              = 1U;
        pVui->FixedFrameRateFlag                 = 1U;
        pVui->NalHrdParametersPresentFlag        = 1U;
        pVui->VclHrdParametersPresentFlag        = 1U;
        pVui->LowDelayHrdFlag                    = 0U;
        pVui->PicStructPresentFlag               = 1U;
        pVui->BitstreamRestrictionFlag           = 0U;
        pVui->MotionVectorsOverPicBoundariesFlag = 1U;
        pVui->AspectRatioIdc                     = 1U;
        pVui->SarWidth                           = 0U;
        pVui->SarHeight                          = 0U;
        pVui->VideoFormat                        = 5U;
        pVui->ColourPrimaries                    = 1U;
        pVui->TransferCharacteristics            = 1U;
        pVui->MatrixCoefficients                 = 1U;
        pVui->ChromaSampleLocTypeTopField        = 0U;
        pVui->ChromaSampleLocTypeBottomField     = 0U;
        pVui->Log2MaxMvLengthHorizontal          = 16U;
        pVui->Log2MaxMvLengthVertical            = 15U;
        pVui->NumReorderFrames                   = 1U;
        pVui->MaxDecFrameBuffering               = 4U;
        pVui->MaxBytesPerPicDenom                = 0U;
        pVui->MaxBitsPerMbDenom                  = 0U;

        Rval = SvcInfoPack_VEncBSBufRequest(i, &Base, &Size);
        if (Rval == OK) {
            pEnc->EncConfig.BitsBufAddr = Base;
            pEnc->EncConfig.BitsBufSize = Size;

            #if CONFIG_ICAM_DSP_ENC_ENG_NUM > 1
            pEnc->Affinity              = Bit << pRec->RecSetting.VEngineId;
            #endif
        } else {
            SvcLog_NG(SVC_LOG_RINO, "## fail to get encode bs buffer, rval(%u)", Rval, 0U);
        }

    #if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
        /* configure MV */
        {
            AMBA_DSP_VIDEO_ENC_MV_CFG_s  *pMV;

            pMV = &(pInfo->pStrmMV[RecNum]);
            AmbaSvcWrap_MisraMemset(pMV, 0, sizeof(AMBA_DSP_VIDEO_ENC_MV_CFG_s));

            if (0U < pRec->RecSetting.MVInfoFlag) {
                if (MVSize == 0U) {
                    Rval = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_MVINFO, &MVOff, &MVSize);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_RINO, "## fail to get mv buffer, rval(%u)", Rval, 0U);
                    }
                }

                Rval = AmbaDSP_CalcEncMvBufInfo(pRec->StrmCfg.MaxWin.Width,
                                               pRec->StrmCfg.MaxWin.Height,
                                               0U,
                                               &MVUnit);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_RINO, "AmbaDSP_CalcEncMvBufInfo failed %u", Rval, 0U);
                }

                Size = GetAlignedValU32((MVUnit * MAX_MV_NUM_PER_STRM), AMBA_CACHE_LINE_SIZE);
                if (Size <= MVSize) {
                    pMV->BufAddr  = MVOff;
                    pMV->BufSize  = Size;
                    pMV->UnitSize = MVUnit;

                    MVOff += Size;
                    MVSize -= Size;
                } else {
                    SvcLog_NG(SVC_LOG_RINO, "## mv buffer isn't enough (%u)", Size, 0U);
                }
            }
        }
    #endif

        RecNum = RecNum + 1U;
    }
    *(pInfo->pNumStrm) = RecNum;
}

/**
 * Get video bitstream buffer base and size based on record stream ID
 * @param [in] StrmIdx record stream id
 * @param [out] pBase bitstream buffer base
 * @param [out] pSize bitstream buffer size
 * @return 0-OK, 1-NG
 */
UINT32 SvcInfoPack_VEncBSBufRequest(UINT32 StrmIdx, ULONG *pBase, UINT32 *pSize)
{
    UINT32                   i, Err, Rval = SVC_OK, TotalSize = 0U;
    ULONG                    TotalBrate = 0U, TotalBase = 0U;
    ULONG                    BRate[CONFIG_ICAM_DSP_ENC_ENG_NUM] = {0}, BufBase[CONFIG_ICAM_DSP_ENC_ENG_NUM] = {0}, BufSize[CONFIG_ICAM_DSP_ENC_ENG_NUM] = {0};
    UINT32                   EngNum = (UINT32)CONFIG_ICAM_DSP_ENC_ENG_NUM;
    const SVC_RES_CFG_s      *pCfg = SvcResCfg_Get();
    const SVC_REC_STRM_s     *pRec;

    AmbaMisra_TouchUnused(&EngNum);

    for (i = 0U; i < pCfg->RecNum; i++) {
        pRec = &(pCfg->RecStrm[i]);

        if (pRec->RecSetting.VEngineId < EngNum) {
            ULONG Brate = (ULONG)SvcBRateCalc_GetMaxBRate(i);

            BRate[pRec->RecSetting.VEngineId] += Brate;
            TotalBrate                        += Brate;
        } else {
            SvcLog_NG(SVC_LOG_RINO, "Invalid video encoder engine Id (%u)", pRec->RecSetting.VEngineId, 0U);
            Rval = SVC_NG;
            break;
        }
    }

    if (Rval == SVC_OK) {
        Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_VENC_BS, &TotalBase, &TotalSize);
        if (Err != OK) {
            SvcLog_NG(SVC_LOG_RINO, "fail to get Video bitstream buffer/size", 0U, 0U);
            Rval = SVC_NG;
        }
    }

    if (Rval == SVC_OK) {
    #if CONFIG_ICAM_DSP_ENC_ENG_NUM > 1
        for (i = 0U; i < (UINT32)CONFIG_ICAM_DSP_ENC_ENG_NUM; i++) {
            if (i == 0U) {
                BufBase[i] = TotalBase;
            } else {
                BufBase[i] = BufBase[i - 1U] + BufSize[i - 1U];
            }

            if (i == ((UINT32)CONFIG_ICAM_DSP_ENC_ENG_NUM - 1U)) {
                BufSize[i] = (TotalBase + TotalSize) - BufBase[i];
            } else {
                BufSize[i] = (TotalSize * BRate[i]);
                if (TotalBrate > 0U) {
                    BufSize[i] /= TotalBrate;
                }
                BufSize[i] = (BufSize[i] / 0x100000U) * 0x100000U; /* 1MB alignment */
            }
        }
    #else
        BufBase[0] = TotalBase;
        BufSize[0] = TotalSize;

        AmbaMisra_TouchUnused(&BRate);
        AmbaMisra_TouchUnused(&TotalBrate);
    #endif
    }

    if (Rval == SVC_OK) {
        if (StrmIdx < pCfg->RecNum) {
            pRec = &(pCfg->RecStrm[StrmIdx]);

            *pBase = BufBase[pRec->RecSetting.VEngineId];
            *pSize = (UINT32)BufSize[pRec->RecSetting.VEngineId];
        } else {
            SvcLog_NG(SVC_LOG_RINO, "Invalid StrmIdx %u, RecNum %u ", StrmIdx, pCfg->RecNum);
            Rval = SVC_NG;
        }
    }

    return Rval;
}
