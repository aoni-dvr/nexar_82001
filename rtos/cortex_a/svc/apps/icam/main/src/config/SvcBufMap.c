/**
 *  @file SvcBufMap.c
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
 *  @details svc buffer map
 *
 */

#include ".svc_autogen"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaGDMA.h"
#include "AmbaDSP.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaUtility.h"
#include "AmbaFPD.h"
#include "AmbaDef.h"
#if defined(CONFIG_ICAM_IMGITUNER_USED)
#include "AmbaTUNE_HdlrManager.h"
#endif
#include "AmbaSensor.h"
#include "AmbaHDMI_Def.h"
#include "AmbaShell.h"

#if defined(CONFIG_BUILD_COMMON_TUNE_CTUNER)
#include "AmbaCT_EmirTunerIF.h"
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaCalib_EmirrorIF.h"
#include "AmbaCalib_DetectionIF.h"
#include "AmbaCT_TextHdlr.h"
#include "AmbaCalib_SimpleExtCalibIF.h"
#endif
#include "AmbaImg_Proc.h"
#include "AmbaIQParamHandlerSample.h"
#include "AmbaPrint.h"

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "RefFlow_RMG.h"
#include "RefFlow_BSD.h"
#endif

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaWS_LDWS.h"
#include "RefFlow_LD.h"
#include "RefFlow_FC.h"
#include "RefFlow_AutoCal.h"
#include "SvcAdasAutoCal.h"
#include "AmbaSLD_LaneDetect.h"
#include "SvcLdwsTask.h"
#include "SvcLdwsTaskV2.h"
#endif

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
#include "AmbaCT_TextHdlr.h"
#include "AmbaCalib_AVMIF.h"
#include "AmbaCT_AvmTunerIF.h"
#include "SvcAnimCalib.h"
#include "SvcAnim.h"
#include "SvcAnimTask.h"
#if defined(CONFIG_BUILD_CV)
#include "SvcPsdTask.h"
#include "SvcOwsTask.h"
#include "RefFlow_RCTA.h"
#include "SvcRctaTask.h"
#endif
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
#include "AmbaOD_Tracker.h"
#endif

#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcMem.h"
#include "SvcBufCalc.h"
#include "SvcIKCfg.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcResCfg.h"
#include "SvcRawCap.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#if defined(CONFIG_BUILD_CV) && defined(CONFIG_ICAM_USE_AMBA_DIRTDETECT)
#include "AmbaDirtDetect.h"
#endif
#include "SvcTask.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "SvcUserPref.h"
#include "SvcRecMain.h"
#include "SvcRecInfoPack.h"
#include "AmbaVfs.h"
#include "AmbaCodecCom.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"
#include "AmbaAEncFlow.h"
#include "SvcAudInfoPack.h"
#include "SvcBufMap.h"
#if defined(CONFIG_ICAM_IMGCAL_USED)
#include "SvcCalibMgr.h"
#include "SvcCalibTask.h"
#if defined(CONFIG_ICAM_EEPROM_USED)
#include "SvcCalibModuleTask.h"
#endif
#endif
#include "SvcFwUpdate.h"
#include "SvcBuffer.h"
#include "SvcIK.h"
#include "SvcResCfgTask.h"
#include "SvcBRateCalc.h"
#include "AmbaAudio_AacDec.h"
#include "SvcErrCode.h"
#if defined(CONFIG_ICAM_CV_STEREO)
#include "SvcStereoTask.h"
#include "AmbaSTU_IF.h"
#include "SvcWarp.h"
#include "SvcCalibStereo.h"
#endif
#if (defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))
#include "SvcStereoACTask.h"
#endif
#include "SvcDisplay.h"
#include "SvcLiveview.h"
#include "SvcInfoPack.h"
#include "SvcSysStat.h"

#include "SvcDSP.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillProc.h"
#include "SvcStillMux.h"
#include "SvcLog.h"

#include "AmbaStreamWrap.h"
#if defined(CONFIG_ICAM_PLAYBACK_USED)
#include "AmbaPlayer.h"
#include "SvcPbkPictDisp.h"
#endif

#if defined(CONFIG_AMBA_RTSP) && !defined(CONFIG_RTSP_LINUX)
#include "SvcRtsp.h"
#endif

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
#include "SvcVoutDefImgTask.h"
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
#include "SvcImgTask.h"
#endif

#include "SvcNvmTask.h"
#include "SvcOsdTask.h"
#include "SvcDrawStopwatch.h"
#if defined(CONFIG_ICAM_GUI_MENU_USED)
#include "SvcMenuTask.h"
#endif
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "SvcUcBIST.h"
#endif
#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
#include "SvcLvFeedExtYuvTask.h"
#endif

#ifdef CONFIG_ICAM_YUVFILE_FEED
#include "SvcLvFeedFileYuvTask.h"
#endif

#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
#include "SvcIsoCfg.h"
#endif

#if defined(CONFIG_ICAM_USB_USED)
#include "AmbaSvcUsb.h"
#endif
#include "SvcLogoDraw.h"

static UINT32 UsrDspFixedMemReq(SVC_MEM_MAP_s *pFixedMM, SVC_MEM_REQ_s *pFixedReq)
{
    UINT32  MemIdx = 0U, WorkSize;

    /* dsp log buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_DSP_LOG;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_LOG_SIZE;
    MemIdx++;

    /* dsp working buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_DSP_WRK;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_WRK_SIZE;
    MemIdx++;

    /* ik buffer */
    WorkSize = 0;
    if (SvcIK_QueryArchMemSize(&WorkSize) == OK) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_IK_WRK;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

#if defined(CONFIG_ICAM_IMGCAL_USED)
    /* Calibration */
    {
        UINT32 ShadowMemSize = 0U;
        UINT32 WorkMemSize   = 0U;

        /* Query calibration memory */
        if (0U == SvcCalibTask_MemQry(&ShadowMemSize, &WorkMemSize, NULL)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CALIB;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = ShadowMemSize;
            MemIdx++;

            pFixedReq[MemIdx].MemId     = FMEM_ID_CALIB_IK;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WorkMemSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_PYRAMID_BUF_SUPPORTED)
    /* vp message buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_VP_MSG;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_VP_MSG_SIZE;
    MemIdx++;

    /* pyramid buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_PYRAMID;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_PYRAMID_SIZE;
    MemIdx++;
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* svc img main mem buf */
    WorkSize = 0;
    if (0U == SvcImgTask_MemSizeQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_IMG_MAIN_MEM;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

    /* svc osd mem buf */
    WorkSize = 0;
    if (0U == SvcOsdTask_MemSizeQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_OSD_BUF;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }

#if defined(CONFIG_ICAM_BIST_UCODE)
    /* ucode BIST buffer */
    WorkSize = 0;
    SvcUcBIST_EvalMemSize(&WorkSize);
    if (WorkSize > 0U) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_UCBIST;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_TRANSIENT_DRAM_SIZE) && !(defined(CONFIG_THREADX) && !defined(CONFIG_ATT_8GB))
    /* transient buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_TRANSIENT_BUF;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_TRANSIENT_DRAM_SIZE;
    MemIdx++;
#endif

#if defined(CONFIG_ICAM_AUDIO_USED) && defined(CONFIG_LINUX)
    /* for pure Linux, we need to allocate audio bitstream buffer in dsp region for other tasks to mmap */
    WorkSize = AmbaAudioEnc_QueryBsSize();
    pFixedReq[MemIdx].MemId     = FMEM_ID_AUD_ENC_BS;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = (WorkSize * (UINT32)CONFIG_AENC_NUM);
    MemIdx++;
#endif

    pFixedMM->pReqArr[SVC_MEM_REG_DSP] = pFixedReq;
    pFixedMM->ReqNum[SVC_MEM_REG_DSP] = MemIdx;
    return MemIdx;
}

static UINT32 UsrIoFixedMemReq(SVC_MEM_MAP_s *pFixedMM, SVC_MEM_REQ_s *pFixedReq)
{
    UINT32  MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pFixedMM);
    AmbaMisra_TouchUnused(pFixedReq);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* IQ table buffer */
    WorkSize = 0U;
    if (0U == SvcImgTask_IqMemSizeQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_IMG_IQ_TBL;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
    /* CC 3D table buffer */
    WorkSize = 0U;
    if (0U == SvcIsoCfg_CcThreeDMemQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_ICFG_IQ_CC_3D;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }

    if (0U == SvcIsoCfg_CcRegMemQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_ICFG_IQ_CC_REG;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_GUI_MENU_USED)
    /* svc menu bmp buf */
    WorkSize = 0U;
    if (0U == SvcMenuTask_MemSizeQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_MENU_BMP;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_32BITS_OSD_USED)
    WorkSize = 0U;
    SvcLogoDraw_MemSizeQuery(&WorkSize);
    if (0U < WorkSize) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_GUI_LOGO;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_BUILD_COMMON_SERVICE_CODEC)
    /* vfs buffer */
    WorkSize = 0U;
    if (AmbaVFS_EvalMemSize(&WorkSize) == SVC_OK) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_VFS;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_ENCRYPTION)
    /* encryption internal buffer */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_ENCRYPT;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_ENCRYPT_BUF_SIZE;
        MemIdx++;
    }
#endif

    pFixedMM->pReqArr[SVC_MEM_REG_IO] = pFixedReq;
    pFixedMM->ReqNum[SVC_MEM_REG_IO] = MemIdx;
    return MemIdx;
}

static UINT32 UsriCamCaFixedMemReq(SVC_MEM_MAP_s *pFixedMM, SVC_MEM_REQ_s *pFixedReq)
{
    UINT32  MemIdx = 0U, WorkSize;

    /* system status buffer */
    WorkSize = 0U;
    if (0U == SvcSysStat_QueryBufSize(&WorkSize)) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_SYS_STAT;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

    /* SVC NVM Task CRC Check */
    WorkSize = SvcNvmTask_MemQry();
    if (WorkSize > 0U) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_NVM_CRC_CHECK;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

#if defined(CONFIG_ICAM_USB_USED)
    /* usb protocol stack memory */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_USB_CA;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = AMBA_USB_STACK_CA_SIZE;
        MemIdx++;
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* svc img aaa mem buf */
    WorkSize = 0U;
    if (0U == SvcImgTask_AlgoMemSizeQuery(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_IMG_AAA_MEM;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_IMGITUNER_USED)
    /* Ituner */
    WorkSize = 0U;
    if (0U == AmbaItn_QueryItuner(&WorkSize)) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_ITUNER;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_BUILD_COMMON_SERVICE_CODEC)
    /* BITS FIFO */
    WorkSize = 0U;
    if (AmbaBitsFifo_EvalMemSize(&WorkSize) == SVC_OK) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_BITS_FIFO;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#ifdef CONFIG_ICAM_PLAYBACK_USED
    /* player */
    WorkSize = 0U;
    if (AmbaPlayer_EvalMemSize(CONFIG_AMBA_PLAYER_MAX_NUM, &WorkSize) == SVC_OK) {
        pFixedReq[MemIdx].MemId     = FMEM_ID_PLAYER;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_AUDIO_USED)
    {
        UINT32                        AinNum;
        AMBA_AIN_IO_CREATE_INFO_s     AinInfo = {0};
        AMBA_AUDIO_ENC_CREATE_INFO_s  AencInfo = {0};
        AMBA_AOUT_IO_CREATE_INFO_s    AoutInfo = {0};
        AMBA_AUDIO_DEC_CREATE_INFO_s  AdecInfo = {0};

        SvcAudInfoPack_AINInfo(&AinNum, &AinInfo);
        SvcAudInfoPack_AENCInfo(&AencInfo);
        SvcAudInfoPack_AOUTInfo(&AoutInfo);
        SvcAudInfoPack_ADECInfo(&AdecInfo);

        AencInfo.PlugInLibSelfSize = GetMaxValU32((UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s),
                                                  (UINT32)sizeof(AMBA_AUDIO_AAC_CONFIG_s));

        AdecInfo.PlugInLibSelfSize = GetMaxValU32((UINT32)sizeof(AMBA_AUDIO_PCM_CONFIG_s),
                                                  (UINT32)sizeof(au_aacdec_config_t));
        //PCM need 8192, AAC need 768, so set PCM size to evaluate first
        AdecInfo.NeededBytes = 8192;

        AmbaPrint_PrintUInt5("CONFIG_AENC_NUM = %d", CONFIG_AENC_NUM, 0U, 0U, 0U, 0U);
        /* Audio input part */
        if (AmbaAudioInp_QueryBufSize(&AinInfo) == SVC_OK) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_INP_CA;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = AinInfo.CachedBufSize;
            MemIdx++;
        }

        /* Audio encoder Cache */
        if (AmbaAudioEnc_QueryBufSize(&AencInfo) == SVC_OK) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_ENC_CA;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = (AencInfo.CachedBufSize * (UINT32)CONFIG_AENC_NUM);
            MemIdx++;
        }

        /* Audio output Cache */
        if (AmbaAudioOup_QueryBufSize(&AoutInfo) == SVC_OK) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_OUP_CA;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = AoutInfo.CachedBufSize;
            MemIdx++;
        }

        /* Audio decoder Cache */
        if (AmbaAudioDec_QueryBufSize(&AdecInfo) == SVC_OK) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_DEC_CA;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = AdecInfo.CachedBufSize;
            MemIdx++;
        }

        /* audio decode bitstream bufffer */
        WorkSize = AmbaAudio_GetDecBsBufSize();
        pFixedReq[MemIdx].MemId     = FMEM_ID_AUD_DEC_BS;
        pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;

#ifndef CONFIG_LINUX
        /* audio encode bitstream bufffer */
        WorkSize = AmbaAudioEnc_QueryBsSize();
        pFixedReq[MemIdx].MemId     = FMEM_ID_AUD_ENC_BS;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = (WorkSize * (UINT32)CONFIG_AENC_NUM);
        MemIdx++;
#endif

    }
#endif

#if defined(CONFIG_AMBA_RTSP) && !defined(CONFIG_RTSP_LINUX)
    WorkSize = 0U;
    if (SVC_OK == SvcRtsp_GetMemSize(&WorkSize)) {
        /* Rtsp service Cache */
        pFixedReq[MemIdx].MemId     = FMEM_ID_RTSP_SERVICE;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
    {
        /* emirror calib table */
        SIZE_t  WSize;

        if (0U == AmbaCT_GetBufSize(AMBA_CT_TYPE_EMIRROR, &WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_EMIRROR_CT;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* quick calib */
        if (0U == AmbaCal_ExtGetSimpCorrPitchBufSz(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_QUICK_CALIB;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        #if defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
        /* ADAS LDWS buffer calib size */
        {
            AMBA_SLD_CFG_DATA_s  SldDefCfg;

            if (0U == AmbaCal_EmGetCalibInfoBufSize(&WSize)) {
                pFixedReq[MemIdx].MemId     = FMEM_ID_ADAS_LDWS_CALIB;
                pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pFixedReq[MemIdx].ReqSize   = WSize;
                MemIdx++;
            }

            if (0U == AmbaSLD_GetDefaultConfig(SVC_LDWS_SEG_WIDTH,
                                               SVC_LDWS_SEG_HEIGHT,
                                               &SldDefCfg)) {
                pFixedReq[MemIdx].MemId     = FMEM_ID_ADAS_LDWS;
                pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pFixedReq[MemIdx].ReqSize   = SldDefCfg.WrkBufSize;
                MemIdx++;
            }

            /* ADAS FCWS and FCMD calib buffer size */
            if (0U == AmbaCal_EmGetCalibInfoBufSize(&WSize)) {
                pFixedReq[MemIdx].MemId     = FMEM_ID_ADAS_FC_CALIB;
                pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pFixedReq[MemIdx].ReqSize   = WSize;
                MemIdx++;
            }

        }
        #endif

        #if defined(CONFIG_ICAM_PROJECT_EMIRROR)
        /* emirror calib buff */
        if (0U == AmbaCal_EmGetCalibInfoBufSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CALIB_EMIRROR;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* emirror 3in1 buff */
        if (0U == AmbaCal_EmGet3in1BufSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_3IN1_EMIRROR;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* emirror 3in1 view buff */
        if (0U == AmbaCal_EmGet3in1VBufSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_3IN1V_EMIRROR;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* rear mirror guard working buffer */
        if (0U == RefFlowRMG_GetWorkBufferSize(&WorkSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_RMG_WRK;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
        #endif
    }
#endif

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
    /* surround calib buff */
    {
        SIZE_t  WSize;

        if (0U == AmbaCT_GetBufSize(AMBA_CT_TYPE_AVM, &WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_SUR_CT;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* surround calib buff */
        if (0U == AmbaCal_AvmGet3DWorkSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_SUR_3D_VIEW;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* surround calib info buff */
        if (0U == AmbaCal_AVMGetCalibInfoWorkSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_INFO_SUR;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        /* surround osd buff */
        WSize = 147456; //TODO
        pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_SUR_CAR;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WSize;
        MemIdx++;

        /* surround osd compress buff */
        WSize = 8963; //TODO
        pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_SUR_CAR_CMPR;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = WSize;
        MemIdx++;

        /* surround calib buff */
        if (0U == SvcAnimCalib_QueryBufSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_CAL_SUR_LV_CAL;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }

        #if defined(CONFIG_BUILD_CV)
        /* PSD buff */
        pFixedReq[MemIdx].MemId     = FMEM_ID_SUR_CV_PSD;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = SVC_PSD_WRK_BUF_SIZE;
        MemIdx++;

        /* OWS buff */
        pFixedReq[MemIdx].MemId     = FMEM_ID_SUR_CV_OWS;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = SVC_OWS_WRK_BUF_SIZE;
        MemIdx++;

        /* RCTA buff */
        pFixedReq[MemIdx].MemId     = FMEM_ID_SUR_CV_RCTA;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = SVC_RCTA_WRK_BUF_SIZE;
        MemIdx++;
        #endif
    }
#endif

#if defined(CONFIG_ICAM_USE_AMBA_DIRTDETECT) && defined(CONFIG_BUILD_CV)
    /* Dirt Detection Task*/
    {
        AMBA_DIRT_DETECT_SIZE_s  ImgSize;

        #ifdef CONFIG_ICAM_PROJECT_ADAS_DVR
        ImgSize.Width = 480;
        ImgSize.Height = 272;
        #else
        ImgSize.Width = 480;
        ImgSize.Height = 200;
        #endif

        if (0U == AmbaDirtDetect_QueryBufSize(AMBA_DSP_SINGLE_CHAN, &ImgSize, &WorkSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_DIRT_DETECT_WRK;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif
#if defined(CONFIG_ICAM_CV_STEREO)
        pFixedReq[MemIdx].MemId     = FMEM_ID_STEREO_CALIB;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize     = (UINT32)sizeof(SVC_CALIB_STEREO_INFO_s);
        MemIdx++;

        /* stereo calib buffer */
        pFixedReq[MemIdx].MemId     = FMEM_ID_STEREO_INFOPACK;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize     = (UINT32)sizeof(SVC_CALIB_STEREO_INFO_s);
        MemIdx++;
#endif

    pFixedMM->pReqArr[SVC_MEM_REG_CA] = pFixedReq;
    pFixedMM->ReqNum[SVC_MEM_REG_CA] = MemIdx;
    return MemIdx;
}

static UINT32 UsriCamNCaFixedMemReq(SVC_MEM_MAP_s *pFixedMM, SVC_MEM_REQ_s *pFixedReq)
{
    UINT32  MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pFixedMM);
    AmbaMisra_TouchUnused(pFixedReq);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

#if defined(CONFIG_ICAM_TRANSIENT_DRAM_SIZE) && (defined(CONFIG_THREADX) && !defined(CONFIG_ATT_8GB))
    /* transient buffer */
    pFixedReq[MemIdx].MemId     = FMEM_ID_TRANSIENT_BUF;
    pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_TRANSIENT_DRAM_SIZE;
    MemIdx++;
#endif

#if defined(CONFIG_ICAM_AUDIO_USED) && !defined(CONFIG_LINUX)
    {
        UINT32                      AinNum;
        AMBA_AIN_IO_CREATE_INFO_s   AinInfo = {0};
        AMBA_AOUT_IO_CREATE_INFO_s  AoutInfo = {0};

        SvcAudInfoPack_AINInfo(&AinNum, &AinInfo);
        SvcAudInfoPack_AOUTInfo(&AoutInfo);

        /* Audio input part */
        if (AmbaAudioInp_QueryBufSize(&AinInfo) == SVC_OK) {
            /* Audio input Cache */
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_INP_NC;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = AinInfo.NonCachedBufSize;
            MemIdx++;
        }

        if (AmbaAudioOup_QueryBufSize(&AoutInfo) == SVC_OK) {
            /* Audio input Non-Cache */
            pFixedReq[MemIdx].MemId     = FMEM_ID_AUDIO_OUP_NC;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = AoutInfo.NonCachedBufSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
    /* emirror calib table */
    {
        SIZE_t  WSize;

        if (0U == AmbaCal_DetGetDetRectBufSize(&WSize)) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_EMIRROR_REC_DET;
            pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pFixedReq[MemIdx].ReqSize   = WSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG)
    /* Vout default image task */
    WorkSize = 0U;
    if (0U == SvcVoutDefImgTask_MemQry(&WorkSize)) {
        if (WorkSize > 0U) {
            pFixedReq[MemIdx].MemId     = FMEM_ID_VOUT_DEF_IMG;
            pFixedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pFixedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_USB_USED)
    /* usb protocol stack memory */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_USB_NC;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = AMBA_USB_STACK_NC_SIZE;
        MemIdx++;
    }
#endif

    pFixedMM->pReqArr[SVC_MEM_REG_NC] = pFixedReq;
    pFixedMM->ReqNum[SVC_MEM_REG_NC] = MemIdx;
    return MemIdx;
}

static UINT32 UsrCvFixedMemReq(SVC_MEM_MAP_s *pFixedMM, SVC_MEM_REQ_s *pFixedReq)
{
#if defined(CONFIG_BUILD_CV)
    UINT32  MemIdx = 0U;

#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
    /* Main Y12 buffer */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_MAIN_Y12_BUF;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_MAIN_Y12_SIZE;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_CV_FILE_IN_BUF_SIZE)
    /* cv file input */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_CV_FILE_IN;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = CONFIG_ICAM_CV_FILE_IN_BUF_SIZE;
        MemIdx++;
    }
#endif

    /* cv algo */
    {
        pFixedReq[MemIdx].MemId     = FMEM_ID_CV_ALGO;
        pFixedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pFixedReq[MemIdx].ReqSize   = SVC_MEM_REQ_LEFT;
        MemIdx++;
    }

    pFixedMM->pReqArr[SVC_MEM_REG_CV] = pFixedReq;
    pFixedMM->ReqNum[SVC_MEM_REG_CV] = MemIdx;

    return MemIdx;
#else
    AmbaMisra_TouchUnused(pFixedMM);
    AmbaMisra_TouchUnused(pFixedReq);

    return 0U;
#endif
}

static UINT32 UsrLvDspShdMemReq(SVC_MEM_MAP_s *pSharedMM, SVC_MEM_REQ_s *pSharedReq)
{
    UINT32               i, j = 0U, MemIdx = 0U, WorkSize;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    {
        UINT32  Width, Height;

        WorkSize = 0U;
        if (pCfg->DispAltNum == 0U) {
            for (i = 0U; i < pCfg->DispNum; i++) {
                for (j = 0U; j < pCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                    if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].BlendEnable == SVC_RES_BLD_TBL_FROM_CALIB) {
                        Width  = GetAlignedValU32(pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width, 64U);
                        Height = pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height;
                        WorkSize += (UINT32)sizeof(AMBA_DSP_BUF_s);
                        WorkSize += Width * Height;
                    } else if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].BlendEnable == SVC_RES_BLD_TBL_FROM_ROM) {
                        Width  = GetAlignedValU32(pCfg->DispStrm[i].StrmCfg.ChanCfg[j].BlendTable.Win.Width, 64U);
                        Height = pCfg->DispStrm[i].StrmCfg.ChanCfg[j].BlendTable.Win.Height;
                        WorkSize += (UINT32)sizeof(AMBA_DSP_BUF_s);
                        WorkSize += Width * Height;
                    } else {
                        /* Do nothing */
                    }
                }
            }
        } else {
            for (UINT32 DispAltIdx = 0U; DispAltIdx < pCfg->DispAltNum; DispAltIdx++) {
                const SVC_DISP_ALT_s *pDispAlt = &pCfg->DispAlt[DispAltIdx];
                for (i = 0U; i < pDispAlt->DispNum; i++) {
                    for (j = 0U; j < pDispAlt->DispStrm[i].StrmCfg.NumChan; j++) {
                        if (pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].BlendEnable == SVC_RES_BLD_TBL_FROM_CALIB) {
                            Width  = GetAlignedValU32(pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width, 64U);
                            Height = pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height;
                            WorkSize += (UINT32)sizeof(AMBA_DSP_BUF_s);
                            WorkSize += Width * Height;
                        } else if (pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].BlendEnable == SVC_RES_BLD_TBL_FROM_ROM) {
                            Width  = GetAlignedValU32(pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].BlendTable.Win.Width, 64U);
                            Height = pDispAlt->DispStrm[i].StrmCfg.ChanCfg[j].BlendTable.Win.Height;
                            WorkSize += (UINT32)sizeof(AMBA_DSP_BUF_s);
                            WorkSize += Width * Height;
                        } else {
                            /* Do nothing */
                        }
                    }
                }
            }
        }

        if (WorkSize > 0U) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_BLENDING;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }

    /* default raw/hds buffer */
    WorkSize = 0U;
    SvcInfoPack_DefRawMemQry(&WorkSize);
    if (WorkSize > 0U) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_DEFAULT_RAW;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

#if defined(CONFIG_ICAM_RECORD_USED)
    /* video bitstream buffer */
    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_VENC_BS;
    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pSharedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_ENC_BS_SIZE;
    MemIdx++;

    /* time-lapse capture buffer */
    for (i = 0; i < pCfg->RecNum; i++) {
        if (0U < pCfg->RecStrm[i].RecSetting.TimeLapse) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_TIMELAPSE;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = CONFIG_ICAM_TLAPSE_SIZE;
            MemIdx++;
            break;
        }
    }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    /* record blend buffer */
    WorkSize = 0U;
    SvcDrawStopwatch_EvalMemSize(&WorkSize);
    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STOPWATCH;
    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pSharedReq[MemIdx].ReqSize   = WorkSize;
    MemIdx++;
#endif
    #if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    /* MV info buffer */
    {
        UINT32  UnitSize;

        WorkSize = 0U;
        for (i = 0; i < pCfg->RecNum; i++) {
            if (0U < pCfg->RecStrm[i].RecSetting.MVInfoFlag) {
                (void)AmbaDSP_CalcEncMvBufInfo(pCfg->RecStrm[i].StrmCfg.MaxWin.Width,
                                               pCfg->RecStrm[i].StrmCfg.MaxWin.Height,
                                               0U,
                                               &UnitSize);

                UnitSize = GetAlignedValU32((UnitSize * MAX_MV_NUM_PER_STRM),
                                            (UINT32)AMBA_CACHE_LINE_SIZE);
                WorkSize += UnitSize;
            }
        }

        if (0U < WorkSize) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_MVINFO;
            pSharedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
    #endif
#endif

#if defined(CONFIG_ICAM_PLAYBACK_USED)
    {
        UINT32            Src, Err, DpxFovId = 0U, DpxMode = 0U;
        UINT32            BufPitch, BufHeight, IsExist = 0U;

        /* if it's in duplex mode */
        for (i = 0U; i < pCfg->FovNum; i++) {
            Src = 255U;
            Err = SvcResCfg_GetFovSrc(i, &Src);
            if ((Err == SVC_OK) && (Src == SVC_VIN_SRC_MEM_DEC)) {
                DpxMode  = 1U;
                DpxFovId = i;
                break;
            }
        }

        if ((DpxMode == 1U)) {
            /* video decoder bitstream buffer */
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_VDEC_BS;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_DEC_BS_SIZE * CONFIG_AMBA_PLAYER_MAX_NUM;
            MemIdx++;

            /* for duplex PIP alpha table */
            for (i = 0U; i < pCfg->DispNum; i++) {
                if (pCfg->DispStrm[i].StrmCfg.NumChan > 1U) {
                    for (j = 0U; j < pCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                        if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId == DpxFovId) {
                            IsExist = 1U;
                            break;
                        }
                    }
                }

                if (IsExist == 1U) {
                    break;
                }
            }

            if ((IsExist == 1U) && (i < AMBA_DSP_MAX_VOUT_NUM) && (j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)) {
                BufPitch  = GetAlignedValU32((UINT32)pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width, 64U);
                BufHeight = (UINT32)pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height;
                WorkSize  = (BufPitch * BufHeight);

                pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_PIP_ALPHA;
                pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pSharedReq[MemIdx].ReqSize   = WorkSize;
                MemIdx++;
            }

            /* SMEM_PF0_ID_STLDEC_YUV */
            {
                BufPitch  = GetAlignedValU32((UINT32)pCfg->FovCfg[DpxFovId].RawWin.Width + (UINT32)AMBA_CACHE_LINE_SIZE, 64U);
                BufHeight = GetAlignedValU32((UINT32)pCfg->FovCfg[DpxFovId].RawWin.Height, 16U);

            #if (SVCAG_SDEC_DISP_FORMAT == AMBA_DSP_YUV422)
                WorkSize = (BufPitch * BufHeight * 2U);
            #else
                WorkSize = (BufPitch * BufHeight * 3U) / 2U;
            #endif

                if (WorkSize > 0U) {
                    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STLDEC_YUV;
                    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                    pSharedReq[MemIdx].ReqSize   = WorkSize;
                    MemIdx++;
                }
            }

            #if defined(CONFIG_ICAM_THMVIEW_IN_DPX)
            /* for picture display */
            {
                SVC_USER_PREF_s  *pPref;
                UINT32           ThmDisp = 0U, PbkFormatId = 0U;

                if (SVC_OK == SvcUserPref_Get(&pPref)) {
                    if (SVC_OK == SvcResCfgTask_GetPbkModeId(&PbkFormatId)) {
                        if (PbkFormatId == pPref->FormatId) {
                            ThmDisp = 1U;
                        }
                    }
                }

                if (ThmDisp > 0U) {
                    WorkSize = 0U;
                    SvcPbkPictDisp_QuerryMem(SVC_PICT_DISP_MODE_DUPLEX, pCfg, &WorkSize);
                    if (WorkSize > 0U) {
                        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_PICT_DISP;
                        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                        pSharedReq[MemIdx].ReqSize   = WorkSize;
                        MemIdx++;
                    }
                }
            }
            #endif
        }
    }
#endif


    /* default yuv frame for DEC and MEM ViewZone */
    {
        UINT32  Src = 0U, Err, BufPitch, BufHeight, BufSize = 0U;

        WorkSize = 0U;

        for (i = 0U; i < pCfg->FovNum; i++) {
            Err = SvcResCfg_GetFovSrc(i, &Src);
            if ((Err == SVC_OK) && ((Src == SVC_VIN_SRC_MEM_DEC) || (Src == SVC_VIN_SRC_MEM_YUV420) || (Src == SVC_VIN_SRC_MEM_YUV422))) {
                BufPitch  = GetAlignedValU32((UINT32)pCfg->FovCfg[i].RawWin.Width, 64U);
                BufHeight = GetAlignedValU32((UINT32)pCfg->FovCfg[i].RawWin.Height, 16U);

                if (Src == SVC_VIN_SRC_MEM_YUV422) {
                    BufSize   = (BufPitch * BufHeight) * 2U;
                } else {
                    BufSize   = ((BufPitch * BufHeight) * 3U) / 2U;
                }

                if (BufSize > WorkSize) {
                    WorkSize = BufSize;
                }
            }
        }

        if (WorkSize > 0U) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_LV_YUVFEED;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }

#ifdef CONFIG_ICAM_YUVFILE_FEED
    {
        WorkSize = 0U;
        SvcLvFeedFileYuvTask_MemSizeQuery(&WorkSize);
        if (WorkSize > 0U) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_FEED_FILE_YUV;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

    pSharedMM->pReqArr[SVC_MEM_REG_DSP] = pSharedReq;
    pSharedMM->ReqNum[SVC_MEM_REG_DSP] = MemIdx;
    return MemIdx;
}

static UINT32 UsrLvIoShdMemReq(SVC_MEM_MAP_s *pSharedMM, SVC_MEM_REQ_s *pSharedReq)
{
    UINT32  i, MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pSharedMM);
    AmbaMisra_TouchUnused(pSharedReq);
    AmbaMisra_TouchUnused(&i);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

    /* video record box buffer */
#if defined(CONFIG_ICAM_RECORD_USED)
    {
        UINT32               Bit = 0x01U, MaxBitRate, PerRbxSize;
        AMBA_REC_EVAL_s      MiaInfo;
        SVC_USER_PREF_s      *pSvcUserPref;
        const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

        WorkSize = 0U;
        for (i = 0; i < pCfg->RecNum; i++) {
            if ((pCfg->RecBits & (UINT32)(Bit << i)) == 0U) {
                continue;
            }

            SvcInfoPack_MiaInfoConfig(i, &MiaInfo);

            if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
                if (pSvcUserPref->MaxBitrate != 0U) {
                    MaxBitRate = SvcBRateCalc_SetMaxBRate(i, pSvcUserPref->MaxBitrate);
                } else {
                    MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
                }
            } else {
                MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
            }

            AmbaPrint_PrintUInt5("[Video record] MaxBitRate = %d", MaxBitRate, 0U, 0U, 0U, 0U);
            MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = MaxBitRate;

            AmbaRbxMP4_EvalMemSize(AMBA_SUB_TYPE_FMP4, &MiaInfo, &PerRbxSize);
            WorkSize += PerRbxSize;
        }

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_RBX;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

    pSharedMM->pReqArr[SVC_MEM_REG_IO] = pSharedReq;
    pSharedMM->ReqNum[SVC_MEM_REG_IO] = MemIdx;
    return MemIdx;
}

static UINT32 UsrLviCamCaShdMemReq(SVC_MEM_MAP_s *pSharedMM, SVC_MEM_REQ_s *pSharedReq, UINT32 SurCal)
{
    UINT32  i, MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pSharedMM);
    AmbaMisra_TouchUnused(pSharedReq);
    AmbaMisra_TouchUnused(&SurCal);
    AmbaMisra_TouchUnused(&i);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

#if defined(CONFIG_ICAM_RECORD_USED)
    {
        UINT32               MaxRecStrms = CONFIG_ICAM_MAX_REC_STRM;
        UINT32               Bit = 0x01U, MaxBitRate;
        AMBA_REC_EVAL_s      MiaInfo;
        SVC_USER_PREF_s      *pSvcUserPref;
        const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

        /* video record source queue */
        AmbaRscVA_EvalMemSize(AMBA_RSC_TYPE_VIDEO, CONFIG_AMBA_REC_MAX_VRSC_QUELEN, &WorkSize);

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_VRSC_QU;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = (WorkSize * MaxRecStrms);
        MemIdx++;

        /* audio record source queue */
        AmbaRscVA_EvalMemSize(AMBA_RSC_TYPE_AUDIO, CONFIG_AMBA_REC_MAX_ARSC_QUELEN, &WorkSize);

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_ARSC_QU;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = (WorkSize * MaxRecStrms);
        MemIdx++;

        /* data record source queue */
        AmbaRscData_EvalMemSize(AMBA_RSC_TYPE_DATA, CONFIG_AMBA_REC_MAX_DRSC_QUELEN, &WorkSize);

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_DRSC_QU;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = (WorkSize * MaxRecStrms);
        MemIdx++;

        /* normal event record buffer */
        {
            UINT32 EvtSize = 0U, PerEvtSize, SlotSize;
            for (i = 0; i < pCfg->RecNum; i++) {
                if ((pCfg->RecBits & (UINT32)(Bit << i)) == 0U) {
                    continue;
                }
                if (0U != (pCfg->RecStrm[i].RecSetting.DestBits & SVC_REC_DST_NMLEVT)) {
                    SvcInfoPack_MiaInfoConfig(i, &MiaInfo);
                    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
                        if (pSvcUserPref->MaxBitrate != 0U) {
                            MaxBitRate = SvcBRateCalc_SetMaxBRate(i, pSvcUserPref->MaxBitrate);
                        } else {
                            MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
                        }
                    } else {
                        MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
                    }

                    AmbaPrint_PrintUInt5("[Event record] MaxBitRate = %d", MaxBitRate, 0U, 0U, 0U, 0U);
                    MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = MaxBitRate;

                    PerEvtSize = 0U;
                    AmbaRdtNmlEvt_EvalMemSize(i, AMBA_SUB_TYPE_FMP4, &MiaInfo, &PerEvtSize, &SlotSize);
                    EvtSize += PerEvtSize;
                }
            }

            if (EvtSize > 0U) {
                pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_NMLEVT_REC;
                pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pSharedReq[MemIdx].ReqSize   = EvtSize;
                MemIdx++;
            }
        }

        /* emergency event record buffer */
        {
            UINT32 EvtSize = 0U, PerEvtSize;
            for (i = 0; i < pCfg->RecNum; i++) {
                if ((pCfg->RecBits & (UINT32)(Bit << i)) == 0U) {
                    continue;
                }
                if (0U != (pCfg->RecStrm[i].RecSetting.DestBits & SVC_REC_DST_EMGEVT)) {
                    SvcInfoPack_MiaInfoConfig(i, &MiaInfo);

                    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
                        if (pSvcUserPref->MaxBitrate != 0U) {
                            MaxBitRate = SvcBRateCalc_SetMaxBRate(i, pSvcUserPref->MaxBitrate);
                        } else {
                            MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
                        }
                    } else {
                        MaxBitRate = SvcBRateCalc_MaxBRateEval(i, pCfg, 1U);
                    }

                    AmbaPrint_PrintUInt5("[Event record] MaxBitRate = %d", MaxBitRate, 0U, 0U, 0U, 0U);
                    MiaInfo.MiaCfg[AMBA_REC_MIA_VIDEO].BitRate = MaxBitRate;

                    AmbaRdtEmgEvt_EvalMemSize(&MiaInfo, CONFIG_ICAM_MAX_EMG_REC_TIME, &PerEvtSize);
                    EvtSize += PerEvtSize;
                }
            }

            if (EvtSize > 0U) {
                pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_EMGEVT_REC;
                pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pSharedReq[MemIdx].ReqSize   = EvtSize;
                MemIdx++;
            }
        }
    }
#endif

#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
    {
        UINT8                j;
        UINT32               Width, Height;
        SIZE_t               Size_t = 0U;
        const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

        WorkSize = 0U;
        for (i = 0U; i < pCfg->DispNum; i++) {
            for (j = 0U; j < pCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                if (pCfg->DispStrm[i].StrmCfg.ChanCfg[j].BlendEnable == SVC_RES_BLD_TBL_FROM_CALIB) {
                    Width  = pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Width;
                    Height = pCfg->DispStrm[i].StrmCfg.ChanCfg[j].DstWin.Height;
                    WorkSize += Width * Height;
                }
            }
        }

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_EMA_ROITBL;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;

        (void) AmbaOD_GetTrackerBufSize(&Size_t);

        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_EMA_TRACKER;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = Size_t;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_USED) && defined(CONFIG_ICAM_EEPROM_USED)
    WorkSize = 0U;
    if (0U == SvcCalibTask_ModuleMemQry(&WorkSize)) {
        if (WorkSize > 0U) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STOR_MODULE;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_CV_STEREO)
    /* stereo buffer */
    (void)SvcStereoTask_QueryBufSize(&WorkSize);
    if (WorkSize > 0U) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STEREO;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

    /* warp buffer */
    (void)SvcWarp_QueryWarpSize(&WorkSize);
    if (WorkSize > 0U) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STEREO_WARP;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }

#if (defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB) ||  defined(CONFIG_ICAM_CV_LINUX_STEREO_APP))

    /* stereo auto calibration buffer */
    (void)SvcStereoACTask_QueryBufSize(&WorkSize);
    if (WorkSize > 0U) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STEREO_AC;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif
#endif

#if defined(CONFIG_ICAM_CV_LOGGER)
    /* cv logger */
    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_CV_LOGGER;
    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pSharedReq[MemIdx].ReqSize   = CONFIG_ICAM_CV_LOGGER_BUF_SIZE;
    MemIdx++;

    /* cv logger od*/
    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_CV_LOGGER_OD;
    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pSharedReq[MemIdx].ReqSize   = (UINT32)sizeof(UINT8) * (655360U * 2U);
    MemIdx++;

    /* cv logger seg*/
    pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_CV_LOGGER_SEG;
    pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
    pSharedReq[MemIdx].ReqSize  = (UINT32)sizeof(UINT8) * 655360U;
    MemIdx++;

#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
    if (SvcInfoPack_ExtDataMemSizeQuery(&WorkSize) == SVC_OK) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_LV_FEED_EXT_DATA;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
    if (SvcLvFeedExtYuvTask_ExtYuvMemSizeQuery(&WorkSize) == SVC_OK) {
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_LV_FEED_EXT_YUV;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
    {
        UINT32             Rval;
        SIZE_t             AdasAutoCalSize;
        REF_FLOW_LD_CFG_s  RfLaneCfg;
        REF_FLOW_LD_QUERY_INFO_V2_s LdQryInfo;
        REF_FLOW_FC_CFG_s    RfFcCfg;
        REF_FLOW_FC_CFG_V2_s RfFcCfgV2;
        UINT32  WorkSizeFcV1, WorkSizeFcV2;
        UINT32  WorkSizeLdV1, WorkSizeLdV2;

        //LDWS
        WorkSize = 0U;
        AmbaMisra_TouchUnused(&RfLaneCfg);
        AmbaMisra_TouchUnused(&LdQryInfo);
        Rval = RefFlowLD_GetDefaultCfg(SVC_LDWS_SEG_WIDTH, SVC_LDWS_SEG_HEIGHT, &WorkSizeLdV1, &RfLaneCfg);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d RefFlowLD_GetDefaultCfg error Rval %d", __LINE__, Rval);
        }
        WorkSizeLdV1 = GetAlignedValU32(WorkSizeLdV1, 64U);
        SvcLog_OK(__func__, "Line %d WorkSizeLdV1 : %d", __LINE__, WorkSizeLdV1);

        LdQryInfo.SrQrInfo.SegBufImgW = SVC_LDWS_V2_SEG_WIDTH;
        LdQryInfo.SrQrInfo.SegBufImgH = SVC_LDWS_V2_SEG_HEIGHT;
        Rval = RefFlowLD_QueryV2(&LdQryInfo, &WorkSizeLdV2);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d RefFlowLD_QueryV2 error Rval %d", __LINE__, Rval);
        }
        WorkSizeLdV2 = GetAlignedValU32(WorkSizeLdV2, 64U);
        SvcLog_OK(__func__, "Line %d WorkSizeLdV2 : %d", __LINE__, WorkSizeLdV2);

        WorkSize = (WorkSizeLdV1 > WorkSizeLdV2)?WorkSizeLdV1:WorkSizeLdV2;
        SvcLog_OK(__func__, "Line %d Final LD WorkSize : %d", __LINE__, WorkSize);
        if (WorkSize > 0U){
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_ADAS_LDWS;
            pSharedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }

        //FC
        WorkSize = 0U;
        AmbaMisra_TouchUnused(&RfFcCfg);
        AmbaMisra_TouchUnused(&RfFcCfgV2);

        Rval = RefFlowFC_GetDefaultCfg(&WorkSizeFcV1, &RfFcCfg);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d RefFlowFC_GetDefaultCfg error Rval %d", __LINE__, Rval);
        }
        WorkSizeFcV1 = GetAlignedValU32(WorkSizeFcV1, 64U);
        SvcLog_OK(__func__, "Line %d WorkSizeFcV1 : %d", __LINE__, WorkSizeFcV1);

        Rval = RefFlowFC_GetDefaultCfgV2(&WorkSizeFcV2, &RfFcCfgV2);
        if(Rval != SVC_OK){
            SvcLog_NG(__func__, "Line %d RefFlowFC_GetDefaultCfgV2 error Rval %d", __LINE__, Rval);
        }
        WorkSizeFcV2 = GetAlignedValU32(WorkSizeFcV2, 64U);
        SvcLog_OK(__func__, "Line %d WorkSizeFcV2 : %d", __LINE__, WorkSizeFcV2);

        WorkSize = (WorkSizeFcV1 > WorkSizeFcV2)?WorkSizeFcV1:WorkSizeFcV2;
        SvcLog_OK(__func__, "Line %d Final FC WorkSize : %d", __LINE__, WorkSize);


        if (WorkSize > 0U){
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_ADAS_FC;
            pSharedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }

        //Auto calibration
        (void)AmbaAutoCal_RefHdlrWorkBufSize(&AdasAutoCalSize);
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_AUTO_CALIB;
        pSharedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
        pSharedReq[MemIdx].ReqSize   = AdasAutoCalSize;
        MemIdx++;

        WorkSize = (UINT32)sizeof(SVC_ADAS_AUTOCAL_LD_INFO_s) * SVC_ADAS_AUTOCAL_MSG_DEPTH;
        WorkSize = GetAlignedValU32(WorkSize, 64U);
        if (WorkSize > 0U){
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_AUTO_CALIB_MSG;
            pSharedReq[MemIdx].Alignment = (UINT32)AMBA_CACHE_LINE_SIZE;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }

    }
#endif

#if defined(CONFIG_ICAM_PROJECT_EMIRROR) && defined(CONFIG_BUILD_AMBA_ADAS)
    /* EMR BSD */
    {
        UINT32 Size = 0U;
        static REF_FLOW_BSD_CFG_s RfBsdCfg GNU_SECTION_NOZEROINIT;

        if (RefFlowBSD_GetDefaultCfg(&Size, &RfBsdCfg) == ADAS_ERR_NONE) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_ADAS_BSD;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = Size;
            MemIdx++;
        }
    }
#endif

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
    /* surround calib index buff save to storage puroose */
    {
        UINT32  NonCacheWorkBufSize;

        if (SurCal == 1U) {
            if (SvcAnimTask_GetBufSize(&NonCacheWorkBufSize, &WorkSize) == SVC_OK) {
                pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_SUR_CA;
                pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
                pSharedReq[MemIdx].ReqSize   = WorkSize;
                MemIdx++;
            }
        }
    }
#endif

    pSharedMM->pReqArr[SVC_MEM_REG_CA] = pSharedReq;
    pSharedMM->ReqNum[SVC_MEM_REG_CA] = MemIdx;
    return MemIdx;
}

static UINT32 UsrLviCamNCaShdMemReq(SVC_MEM_MAP_s *pSharedMM, SVC_MEM_REQ_s *pSharedReq, UINT32 SurCal)
{
    UINT32  MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pSharedMM);
    AmbaMisra_TouchUnused(pSharedReq);
    AmbaMisra_TouchUnused(&SurCal);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
    /* surround calib index buff save to storage puroose */
    if (SurCal == 1U) {
        UINT32 CacheWorkBufSize;

        if (SvcAnimTask_GetBufSize(&WorkSize, &CacheWorkBufSize) == SVC_OK) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_SUR_NC;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }
    }
#endif

    pSharedMM->pReqArr[SVC_MEM_REG_NC] = pSharedReq;
    pSharedMM->ReqNum[SVC_MEM_REG_NC] = MemIdx;
    return MemIdx;
}

static UINT32 UsrPbkDspShdMemReq(SVC_MEM_MAP_s *pSharedMM, SVC_MEM_REQ_s *pSharedReq)
{
    UINT32  MemIdx = 0U, WorkSize;

    AmbaMisra_TouchUnused(pSharedMM);
    AmbaMisra_TouchUnused(pSharedReq);
    AmbaMisra_TouchUnused(&MemIdx);
    AmbaMisra_TouchUnused(&WorkSize);

#ifdef CONFIG_ICAM_PLAYBACK_USED
    {
        const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

        /* video decoder bitstream buffer */
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_VDEC_BS;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = CONFIG_ICAM_DSP_DEC_BS_SIZE * CONFIG_AMBA_PLAYER_MAX_NUM;
        MemIdx++;

        /* for picture display */
        WorkSize = 0U;
        SvcPbkPictDisp_QuerryMem(SVC_PICT_DISP_MODE_PLAYBACK, pCfg, &WorkSize);
        if (WorkSize > 0U) {
            pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_PICT_DISP;
            pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
            pSharedReq[MemIdx].ReqSize   = WorkSize;
            MemIdx++;
        }

        /* SMEM_PF0_ID_STLDEC_YUV */
        WorkSize = 0xFF0000U;
        pSharedReq[MemIdx].MemId     = SMEM_PF0_ID_STLDEC_YUV;
        pSharedReq[MemIdx].Alignment = ICAM_BUF_ALIGN;
        pSharedReq[MemIdx].ReqSize   = WorkSize;
        MemIdx++;
    }
#endif

    pSharedMM->pReqArr[SVC_MEM_REG_DSP] = pSharedReq;
    pSharedMM->ReqNum[SVC_MEM_REG_DSP] = MemIdx;
    return MemIdx;
}


/**
 * icam fixed buffer map initialization
 * @return none
 */
void SvcBufMap_Init(void)
{
    static SVC_MEM_MAP_s  g_FixedMemMap GNU_SECTION_NOZEROINIT;
    static SVC_MEM_REQ_s  g_FixedReq[FMEM_ID_NUM] GNU_SECTION_NOZEROINIT;

    UINT32  ReqNum = 0U;

    /* fixed memory map */
    if (AmbaWrap_memset(&g_FixedMemMap, 0, sizeof(g_FixedMemMap)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_SYS, "fail to do memset", 0U, 0U);
    }
    g_FixedMemMap.MapType = SVC_MEM_MAP_FIXED;

    if (AmbaWrap_memset(g_FixedReq, 0, sizeof(g_FixedReq)) == SVC_OK) {
        ReqNum += UsrDspFixedMemReq(&g_FixedMemMap, &(g_FixedReq[ReqNum]));
        ReqNum += UsrIoFixedMemReq(&g_FixedMemMap, &(g_FixedReq[ReqNum]));
        ReqNum += UsriCamCaFixedMemReq(&g_FixedMemMap, &(g_FixedReq[ReqNum]));
        ReqNum += UsriCamNCaFixedMemReq(&g_FixedMemMap, &(g_FixedReq[ReqNum]));

        /* cv user request */
        ReqNum += UsrCvFixedMemReq(&g_FixedMemMap, &(g_FixedReq[ReqNum]));
    }

    SvcBuffer_Init(&g_FixedMemMap);

    SvcLog_DBG(SVC_LOG_SYS, "total fixed memory requests(%u)", ReqNum, 0U);
}

/**
 * icam shared buffer map initialization
 * @param [in] SharedMapId liveview or playback
 * @return none
 */
void SvcBufMap_Config(UINT32 SharedMapId)
{
    static SVC_MEM_MAP_s  g_ShdMemMap GNU_SECTION_NOZEROINIT;
    static SVC_MEM_REQ_s  g_ShdPF0Req[SMEM_PF0_ID_NUM] GNU_SECTION_NOZEROINIT;

    UINT32  SurCal = 0U, ReqNum = 0U;

    /* shared memory map */
    if (AmbaWrap_memset(&g_ShdMemMap, 0, sizeof(g_ShdMemMap)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_SYS, "fail to do memset", 0U, 0U);
    }
    g_ShdMemMap.MapType = SVC_MEM_MAP_SHARED;

    if (AmbaWrap_memset(g_ShdPF0Req, 0, sizeof(g_ShdPF0Req)) == SVC_OK) {
        if (SharedMapId == SMAP_ID_LIVEVIEW) {
            #if defined(CONFIG_ICAM_PROJECT_SURROUND)
            /* surround calib index buff save to storage puroose */
            {
                char                 FileName[64U] = "c:\\CalibDataFile.bin";
                AMBA_FS_FILE         *pFile;
                AMBA_FS_FILE_INFO_s  FileInfo;


                (void)AmbaKAL_TaskSleep(3000U);//Wait FS done

                if (AmbaFS_FileOpen(FileName, "r", &pFile) == SVC_OK) {
                    if (AmbaFS_GetFileInfo(FileName, &FileInfo) == SVC_OK) {
                        if (FileInfo.Size != 0U) {
                            SurCal = 1U;
                        }
                    }
                }
            }
            #endif

            ReqNum += UsrLvDspShdMemReq(&g_ShdMemMap, &(g_ShdPF0Req[ReqNum]));
            ReqNum += UsrLvIoShdMemReq(&g_ShdMemMap, &(g_ShdPF0Req[ReqNum]));
            ReqNum += UsrLviCamCaShdMemReq(&g_ShdMemMap, &(g_ShdPF0Req[ReqNum]), SurCal);
            ReqNum += UsrLviCamNCaShdMemReq(&g_ShdMemMap, &(g_ShdPF0Req[ReqNum]), SurCal);

            SvcBuffer_Config(1U, &g_ShdMemMap);
        } else if (SharedMapId == SMAP_ID_PLAYBACK) {
            ReqNum += UsrPbkDspShdMemReq(&g_ShdMemMap, &(g_ShdPF0Req[ReqNum]));

            SvcBuffer_Config(1U, &g_ShdMemMap);
        } else {
            /* do nothing */
        }
    }

    SvcLog_DBG(SVC_LOG_SYS, "total shared memory requests(%u)", ReqNum, 0U);
}
