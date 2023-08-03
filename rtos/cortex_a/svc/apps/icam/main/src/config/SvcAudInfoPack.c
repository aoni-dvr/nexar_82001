/**
 *  @file SvcAudInfoPack.c
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
 *  @details svc audio information pack
 *
 */

#include "AmbaKAL.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaUtility.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcTask.h"
#include "SvcUserPref.h"
#include "SvcWrap.h"

#include "AmbaVfs.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAEncFlow.h"
#include "AmbaADecFlow.h"
#include "SvcAudInfoPack.h"
#include "SvcNvm.h"
#include "SvcTaskList.h"

#if defined(CONFIG_AMBALINK_BOOT_OS) && defined(CONFIG_AMBA_RTSP)
#include "AmbaDSP_VideoEnc_Def.h"
#include "SvcRtspLink.h"
#endif


#define SVC_LOG_AINFO   "AINFO"

static AMBA_AIN_IO_CREATE_INFO_s  g_AinInfo = {
    .HwIndex          = CONFIG_ICAM_I2S_CHANNEL,
    .ChannelNum       = AIN_CH_NUM,
    .DmaFrameSize     = AIN_FRAME_SIZE,
    .DmaDescNum       = AIN_DESC_NUM,
    .IoNodeNum        = AIN_IO_NUM,
    .CachedBufSize    = 0U,
    .NonCachedBufSize = 0U,
};

static AMBA_AUDIO_ENC_CREATE_INFO_s  g_AEncInfo = {
    .ChannelNum        = AENC_CH_NUM,
    .FrameSize         = AENC_FRAME_SIZE,
    .SampleResolution  = SAMPLE_RESOLUTION,
    .SampleFreq        = AENC_SAMPLE_FREQ,
    .IoNodeNum         = AENC_IO_NUM,
    .PlugInLibSelfSize = 0U,
    .CachedBufSize     = 0U,
};

static AMBA_AUDIO_DEC_CREATE_INFO_s  g_ADecInfo = {
    .SampleFreq        = 0U,
    .ChannelNum        = ADEC_CH_NUM,
    .FrameSize         = ADEC_FRAME_SIZE,
    .SampleResolution  = SAMPLE_RESOLUTION,
    .IoNodeNum         = ADEC_IO_NUM,
    .NeededBytes       = ADEC_SAMPLE_FREQ,
    .PlugInLibSelfSize = 0U,
    .CachedBufSize     = 0U,
};

static AMBA_AOUT_IO_CREATE_INFO_s  g_AOutInfo = {
    .HwIndex          = CONFIG_ICAM_I2S_CHANNEL,
    .ChannelNum       = AOUT_CH_NUM,
    .DmaFrameSize     = AOUT_FRAME_SIZE,
    .DmaDescNum       = AOUT_DESC_NUM,
    .IoNodeNum        = AOUT_IO_NUM,
    .CachedBufSize    = 0U,
    .NonCachedBufSize = 0U,
};

/**
 *  Get AIN info
 *  @param[out] pAinNum  Number of AIN
 *  @param[out] pAinInfo Information of AIN
 */
void SvcAudInfoPack_AINInfo(UINT32 *pAinNum, AMBA_AIN_IO_CREATE_INFO_s *pAinInfo)
{
    *(pAinNum) = 1U;
    *(pAinInfo) = g_AinInfo;
}

/**
 *  Config AIN info
 *  @param[in] pAinNum Number of AIN
 *  @param[in] pAinCfg Information of AIN
 */
void SvcAudInfoPack_AINConfig(UINT32 *pAinNum, AMBA_AUDIO_INPUT_s *pAinCfg)
{
    UINT32  AinCacheBufSize = 0U;
    ULONG   AinCacheBufBase = 0U;
    UINT32  AinNonCacheBufSize = 0U;
    ULONG   AinNonCacheBufBase = 0U;

    *(pAinNum) = 1U;

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED,
                                    FMEM_ID_AUDIO_INP_CA,
                                    &AinCacheBufBase,
                                    &AinCacheBufSize)) {
        SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong ", 0U, 0U);
    }

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED,
                                    FMEM_ID_AUDIO_INP_NC,
                                    &AinNonCacheBufBase,
                                    &AinNonCacheBufSize)) {
        SvcLog_DBG(SVC_LOG_AINFO, "No FMEM_ID_AUDIO_INP_NC ", 0U, 0U);
    }

    //AIN
    AmbaMisra_TypeCast(&(pAinCfg[0].CachedInfo.pHead), &AinCacheBufBase);
    AmbaMisra_TypeCast(&(pAinCfg[0].NonCachedInfo.pHead), &AinNonCacheBufBase);

    pAinCfg[0].AinInfo                = g_AinInfo;
    pAinCfg[0].CachedInfo.MaxSize     = AinCacheBufSize;
    pAinCfg[0].NonCachedInfo.MaxSize  = AinNonCacheBufSize;
    pAinCfg[0].AinputTskCtrl.Priority = SVC_AIN_TASK_PRI;
    pAinCfg[0].AinputTskCtrl.CpuBits  = SVC_AIN_TASK_CPU_BITS;
}

/**
 *  Get AENC info
 *  @param[out] pAEncInfo Information of AENC
 */
void SvcAudInfoPack_AENCInfo(AMBA_AUDIO_ENC_CREATE_INFO_s *pAEncInfo)
{
    *(pAEncInfo) = g_AEncInfo;
}

/**
 *  Config AENC info
 *  @param[in] pAEncCfg AENC config info
 *  @param[in] pAEncSetup AENC setup info
 */
void SvcAudInfoPack_AENCConfig(AMBA_AUDIO_ENC_s *pAEncCfg, AMBA_AUDIO_SETUP_INFO_s *pAEncSetup)
{
    UINT32           AencCacheBufSize = 0U;
    ULONG            AencCacheBufBase = 0U;
    UINT32           AencBsBufSize = 0U;
    ULONG            AencBsBufBase = 0U;
    SVC_USER_PREF_s  *pSvcUserPref;

    if (SvcUserPref_Get(&pSvcUserPref) != SVC_OK) {
        SvcLog_NG(SVC_LOG_AINFO, "fail to get pref", 0U, 0U);
    }

    if (SvcBuffer_Request(SVC_BUFFER_FIXED,
                          FMEM_ID_AUDIO_ENC_CA,
                          &AencCacheBufBase,
                          &AencCacheBufSize) == SVC_OK) {
        //AENC
        pAEncCfg->AencCreateInfo = g_AEncInfo;

        AmbaMisra_TypeCast(&(pAEncCfg->AencBufInfo.pHead), &AencCacheBufBase);
        pAEncCfg->AencBufInfo.MaxSize  = AencCacheBufSize;
        for (UINT32 idx = 0U; idx < (UINT32)AMBA_AUDIO_MAX_BUF_NUM; idx++) {
            pAEncCfg->AencTskCtrl[idx].Priority = SVC_AENC_TASK_PRI;
            pAEncCfg->AencTskCtrl[idx].CpuBits  = SVC_AENC_TASK_CPU_BITS;
        }

        /* PCM encoder setup (Plug-in) */
        pAEncSetup->PCM.BitsPerSample           = pSvcUserPref->BitsPerSample;
        pAEncSetup->PCM.DataFormat              = SVC_AUDIO_ENDIAN_L;
        pAEncSetup->PCM.FrameSize               = g_AEncInfo.FrameSize;
        pAEncSetup->PCM.AencPlugInCs.FrameSize  = g_AEncInfo.FrameSize;
        pAEncSetup->PCM.AencPlugInCs.ChNum      = g_AEncInfo.ChannelNum;
        pAEncSetup->PCM.AencPlugInCs.Resolution = PCM_SAMPLE_RESOLUTION;
        pAEncSetup->PCM.AencPlugInCs.MaxBsSize  = g_AEncInfo.FrameSize * g_AEncInfo.ChannelNum * \
                                                  (g_AEncInfo.SampleResolution / 8U);
        /* AAC encoder setup (Plug-in) */
        pAEncSetup->AAC.Bitrate                 = pSvcUserPref->AudioAacBitRate;
        pAEncSetup->AAC.SampleFreq              = g_AEncInfo.SampleFreq;
        pAEncSetup->AAC.AencPlugInCs.FrameSize  = g_AEncInfo.FrameSize;
        pAEncSetup->AAC.AencPlugInCs.ChNum      = g_AEncInfo.ChannelNum;
        pAEncSetup->AAC.AencPlugInCs.Resolution = AAC_SAMPLE_RESOLUTION;
    } else {
        SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong ", 0U, 0U);
    }

    if (SvcBuffer_Request(SVC_BUFFER_FIXED,
                          FMEM_ID_AUD_ENC_BS,
                          &AencBsBufBase,
                          &AencBsBufSize) == SVC_OK) {

        //Audio bitstream buffer
        AmbaMisra_TypeCast(&(pAEncCfg->pAencBsInfo.pHead), &AencBsBufBase);
        pAEncCfg->pAencBsInfo.Size = AmbaAudioEnc_QueryBsSize();
        AmbaMisra_TouchUnused(&AencBsBufSize);
    } else {
        SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong ", 0U, 0U);
    }


}

/**
 *  Get AOUT info
 *  @param[out] pAOutInfo Information of AOUT
 */
void SvcAudInfoPack_AOUTInfo(AMBA_AOUT_IO_CREATE_INFO_s *pAOutInfo)
{
    *(pAOutInfo) = g_AOutInfo;
}

/**
 *  Config AOUT info
 *  @param[in] pAoutCfg Information of AOUT
 */
void SvcAudInfoPack_AOUTConfig(AMBA_AUDIO_OUTPUT_s *pAoutCfg)
{
    UINT32  AoutCacheBufSize = 0U;
    ULONG   AoutCacheBufBase = 0U;
    UINT32  AoutNonCacheBufSize = 0U;
    ULONG   AoutNonCacheBufBase = 0U;

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED,
                                    FMEM_ID_AUDIO_OUP_CA,
                                    &AoutCacheBufBase,
                                    &AoutCacheBufSize)) {
        SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong ", 0U, 0U);
    }

    if (SVC_OK != SvcBuffer_Request(SVC_BUFFER_FIXED,
                                    FMEM_ID_AUDIO_OUP_NC,
                                    &AoutNonCacheBufBase,
                                    &AoutNonCacheBufSize)) {
        SvcLog_DBG(SVC_LOG_AINFO, "No FMEM_ID_AUDIO_OUP_NC", 0U, 0U);
    }

    //AOUT
    AmbaMisra_TypeCast(&(pAoutCfg->CachedInfo.pHead), &AoutCacheBufBase);
    AmbaMisra_TypeCast(&(pAoutCfg->NonCachedInfo.pHead), &AoutNonCacheBufBase);

    pAoutCfg->AoutInfo                = g_AOutInfo;
    pAoutCfg->CachedInfo.MaxSize      = AoutCacheBufSize;
    pAoutCfg->NonCachedInfo.MaxSize   = AoutNonCacheBufSize;
    pAoutCfg->AoutputTskCtrl.Priority = SVC_AOUT_TASK_PRI;
    pAoutCfg->AoutputTskCtrl.CpuBits  = SVC_AOUT_TASK_CPU_BITS;
}

/**
 *  Get ADEC info
 *  @param[out] pADecInfo Information of ADEC
 */
void SvcAudInfoPack_ADECInfo(AMBA_AUDIO_DEC_CREATE_INFO_s *pADecInfo)
{
    *(pADecInfo) = g_ADecInfo;
}

/**
 *  Config ADEC info
 *  @param[in] pADecInfo ADEC config info
 *  @param[in] pADecSetup ADEC setup info
 */
void SvcAudInfoPack_ADECConfig(AMBA_AUDIO_DEC_s *pADecInfo, AMBA_AUDIO_SETUP_INFO_s *pADecSetup)
{
    UINT32           AdecCacheBufSize = 0U, Err, NvmID;
    ULONG            AdecCacheBufBase = 0U, BufBase;
    SVC_USER_PREF_s  *pSvcUserPref;

    if (SvcUserPref_Get(&pSvcUserPref) != SVC_OK) {
        SvcLog_NG(SVC_LOG_AINFO, "fail to get pref", 0U, 0U);
    }

    if (SvcBuffer_Request(SVC_BUFFER_FIXED,
                          FMEM_ID_AUDIO_DEC_CA,
                          &AdecCacheBufBase,
                          &AdecCacheBufSize) == SVC_OK) {
        //ADEC
        AmbaMisra_TypeCast(&(pADecInfo->AdecBufInfo.pHead), &AdecCacheBufBase);

        pADecInfo->AdecInfo             = g_ADecInfo;
        pADecInfo->AdecBufInfo.MaxSize  = AdecCacheBufSize;
        pADecInfo->AdecTskCtrl.Priority = SVC_ADEC_TASK_PRI;
        pADecInfo->AdecTskCtrl.CpuBits  = SVC_ADEC_TASK_CPU_BITS;

        /* PCM decoder setup (Plug-in) */
        pADecSetup->PCM.BitsPerSample           = pSvcUserPref->BitsPerSample;
        pADecSetup->PCM.DataFormat              = SVC_AUDIO_ENDIAN_L;
        pADecSetup->PCM.FrameSize               = g_ADecInfo.FrameSize;
        pADecSetup->PCM.AdecPlugInCs.FrameSize  = g_ADecInfo.FrameSize;
        pADecSetup->PCM.AdecPlugInCs.ChNum      = g_ADecInfo.ChannelNum;
        pADecSetup->PCM.AdecPlugInCs.Resolution = g_ADecInfo.SampleResolution;

        /* AAC decoder setup (Plug-in) */
        pADecSetup->AAC.AdecPlugInCs.Resolution = g_ADecInfo.SampleResolution;
        pADecSetup->AAC.AdecPlugInCs.FrameSize  = g_ADecInfo.FrameSize;
        pADecSetup->AAC.AdecPlugInCs.ChNum      = g_ADecInfo.ChannelNum;

        pADecSetup->FeedTaskPriority = SVC_ADEC_FEED_TASK_PRI;
        pADecSetup->FeedTaskCpuBits  = SVC_ADEC_FEED_TASK_CPU_BITS;

        Err = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_SYS_DATA);
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_AINFO, "AmbaSvcWrap_GetNVMID failed %u", Err, 0U);
        }
        pADecSetup->NvmID = NvmID;

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED,
                                FMEM_ID_AUD_DEC_BS,
                                &BufBase,
                                &(pADecSetup->BitsBufSize));
        if (SVC_OK != Err) {
            SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong %u", Err, 0U);
        } else {
            pADecSetup->BitsBufAddr = BufBase;
        }

    } else {
        SvcLog_NG(SVC_LOG_AINFO, "SvcBuffer_Request something wrong ", 0U, 0U);
    }
}
