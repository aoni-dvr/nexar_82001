/**
*  @file AmbaDSP_DecodeUtility.c
*
 * Copyright (c) 2020 Ambarella International LP
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
*  @details Implement of HL decode utility APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_DecodeUtility.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSP_EncodeAPI_Def.h"
#include "AmbaDSP_EncodeAPI.h"

UINT8 HL_GetDecFmtTotalBit(void)
{
    UINT16 i;
    UINT8 DecBitMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    for (i=0; i<Resource->DecMaxStreamNum; i++) {
        DecBitMask |= (UINT8)Resource->DecMaxStrmFmt[i];
    }
    return DecBitMask;
}

static inline void HL_FillDecFlowDecCfg(cmd_dsp_dec_flow_max_cfg_t *pDecFlowSetup,
                                        const CTX_RESOURCE_INFO_s *pResource)
{
    UINT16 i;
    dec_cfg_t *pDspStrmDecCfg = NULL;
    ULONG ULAddr = 0U;
    UINT16 DecFmtBitmask = 0U; // B[0]:AVC, B[1]:HEVC, B[2]:JPG
    UINT8 DecCfgNum = 0U;
    UINT8 MaxAvcSR = (UINT8)0U, MaxHevcSR = (UINT8)0U;
    UINT16 AvcAff = 0U, HevcAff = 0U, FinalAff;
    UINT8 DecCfgFmt[AMBA_DSP_MAX_DEC_CFG_NUM];
    UINT8 SysCfgAffinity;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT8 DecTypeNum[AMBA_DSP_DEC_BITS_FORMAT_NUM] = {0};

    for (i = 0U; i < pResource->DecMaxStreamNum; i++) {
        if (1U == DSP_GetU16Bit(pResource->DecMaxStrmFmt[i], 0U/*AVC*/, 1U)) {
            MaxAvcSR = (UINT8)MAX2_16(MaxAvcSR, pResource->DecMaxSearchRange[i]);
            AvcAff |= (UINT16)DSP_GetBit(pResource->DecMaxAffinity[i], DEC_AFFINITY_CORE_BIT_IDX, DEC_AFFINITY_CORE_LEN);

            HL_GetVidDecInfo(HL_MTX_OPT_GET, i, &VidDecInfo);
            VidDecInfo.DecTypeId[AMBA_DSP_DEC_BITS_FORMAT_H264] = DecTypeNum[AMBA_DSP_DEC_BITS_FORMAT_H264];
            HL_SetVidDecInfo(HL_MTX_OPT_SET, i, &VidDecInfo);
            DecTypeNum[AMBA_DSP_DEC_BITS_FORMAT_H264]++;
        }
        if (1U == DSP_GetU16Bit(pResource->DecMaxStrmFmt[i], 1U/*HEVC*/, 1U)) {
            MaxHevcSR = (UINT8)MAX2_16(MaxHevcSR, pResource->DecMaxSearchRange[i]);
            HevcAff |= (UINT16)DSP_GetBit(pResource->DecMaxAffinity[i], DEC_AFFINITY_CORE_BIT_IDX, DEC_AFFINITY_CORE_LEN);

            HL_GetVidDecInfo(HL_MTX_OPT_GET, i, &VidDecInfo);
            VidDecInfo.DecTypeId[AMBA_DSP_DEC_BITS_FORMAT_H265] = DecTypeNum[AMBA_DSP_DEC_BITS_FORMAT_H265];
            HL_SetVidDecInfo(HL_MTX_OPT_SET, i, &VidDecInfo);
            DecTypeNum[AMBA_DSP_DEC_BITS_FORMAT_H265]++;
        }
        DecFmtBitmask |= pResource->DecMaxStrmFmt[i];
    }

    if (1U == DSP_GetU16Bit(DecFmtBitmask, 0U/*AVC*/, 1U)) {
        DecCfgFmt[DecCfgNum] = AMBA_DSP_DEC_BITS_FORMAT_H264;
        DecCfgNum++;
    }
    if (1U == DSP_GetU16Bit(DecFmtBitmask, 1U/*HEVC*/, 1U)) {
        DecCfgFmt[DecCfgNum] = AMBA_DSP_DEC_BITS_FORMAT_H265;
        DecCfgNum++;
    }

    pDecFlowSetup->dec_cfg_num = DecCfgNum;
    for (i = 0U; i < DecCfgNum; i++) {
        HL_GetPointerDspDecStrmCfg(i, &pDspStrmDecCfg);

        /* 2021/9/1, From Mars, CV5 buffer need 16Align when AVC, 32Align when HEVC on both direction */
        if (DecCfgFmt[i] == AMBA_DSP_DEC_BITS_FORMAT_H264) {
            pDspStrmDecCfg->coding_type = (UINT8)0U;
            pDspStrmDecCfg->frm_width = ALIGN_NUM16(pResource->DecMaxFrameWidth, 16U);
            pDspStrmDecCfg->frm_height = ALIGN_NUM16(pResource->DecMaxFrameHeight, 16U);
            pDspStrmDecCfg->smvmax = (MaxAvcSR > 0U)? MaxAvcSR: (UINT8)48U;
            FinalAff = AvcAff;
        } else {
            pDspStrmDecCfg->coding_type = (UINT8)1U;
            pDspStrmDecCfg->frm_width = ALIGN_NUM16(pResource->DecMaxFrameWidth, 32U);
            pDspStrmDecCfg->frm_height = ALIGN_NUM16(pResource->DecMaxFrameHeight, 32U);
            pDspStrmDecCfg->smvmax = (MaxHevcSR > 0U)? MaxHevcSR: (UINT8)24U;
            FinalAff = HevcAff;
        }

        SysCfgAffinity = (UINT8)DSP_GetU16Bit(TuneDspSystemCfg.MaxDecVdspAff, (UINT32)i*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);
        if (SysCfgAffinity > (UINT8)0U) {
            FinalAff = SysCfgAffinity;
        }

        pDspStrmDecCfg->dual_core_mode = (UINT8)0U;
        if (FinalAff == (UINT8)0U) {
            if (pDspStrmDecCfg->frm_width > FHD_WIDTH) {
                pDspStrmDecCfg->core_used = (UINT8)0x3U;
                if (DecCfgFmt[i] == AMBA_DSP_DEC_BITS_FORMAT_H265) {
                    pDspStrmDecCfg->dual_core_mode = (UINT8)1U;
                }
            } else {
                pDspStrmDecCfg->core_used = (UINT8)0x1U;
//FIXME duplex mode?
            }
        } else {
            pDspStrmDecCfg->core_used = (UINT8)FinalAff;
        }

        pDspStrmDecCfg->gmvy = (UINT8)0U;
        pDspStrmDecCfg->ref_num = (UINT8)0U;
    }

    if (DecCfgNum > 0U) {
        HL_GetPointerDspDecStrmCfg(0U, &pDspStrmDecCfg);
        dsp_osal_typecast(&ULAddr, &pDspStrmDecCfg);
        (void)dsp_osal_virt2cli(ULAddr, &pDecFlowSetup->dec_cfg_daddr);
    } else {
        pDecFlowSetup->dec_cfg_daddr = 0U;
    }
}

static UINT32 FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *pDecFlowSetup)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    UINT32 EngAffinity = DSP_GetU16Bit(pResource->DecMaxEngAffinity, DEC_AFFINITY_CORE_BIT_IDX, DEC_AFFINITY_CORE_LEN);

    HL_GetResourcePtr(&pResource);

    if (EngAffinity == 0U) {
        pDecFlowSetup->coding_engine_usage = (UINT8)0x1U;
//FIXME duplex mode
    } else {
        pDecFlowSetup->coding_engine_usage = 0U;
    }

    /* TODO, 20211220, ucode allocate all ENG resource no matter what ARM set */
    if (TuneDspSystemCfg.MaxDecVdspEngAff > (UINT8)0U) {
        pDecFlowSetup->coding_engine_usage = (UINT8)TuneDspSystemCfg.MaxDecVdspEngAff;
    }

    pDecFlowSetup->max_bit_rate_h264 = 0U; //TBD
    pDecFlowSetup->max_bit_rate_hevc = 0U; //TBD
    pDecFlowSetup->max_frm_num_of_dpb = 0U; //default value. 5 when MaxFrmWidth > 1920. 4 when MaxFrmWidth <= 1920

    HL_FillDecFlowDecCfg(pDecFlowSetup,
                         pResource);

#ifdef SUPPORT_DEC_CHAN_RES
    {
        UINT16 i;
        for (i = 0U; i < pResource->DecMaxStreamNum; i++) {
            pDecFlowSetup->max_resolution[i] = ALIGN_NUM16(pResource->DecMaxFrameWidth, 32U);
            pDecFlowSetup->max_resolution[i] |= ((UINT32)ALIGN_NUM16(pResource->DecMaxFrameHeight, 32U) << 16U);
        }
    }
#endif

    return Rval;
}

UINT32 HL_FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *DecFlowSetup)
{
    UINT32 Rval;

    Rval = FillDecFlowMaxCfgSetup(DecFlowSetup);

    return Rval;
}

UINT32 HL_FillDecSetup(UINT16 StreamIdx, cmd_decoder_setup_t *pDecSetupCfg)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    // When multiple decode, must send "decode setup" command for each decoder.
    pDecSetupCfg->hdr.decoder_id = (UINT8)VidDecInfo.DecTypeId[VidDecInfo.BitsFormat];
    pDecSetupCfg->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];

    //CV5 only use default type for general case
/* FIXME when B Frm */
    pDecSetupCfg->dec_setup_type = DEC_SETUP_TYPE_IP_ONLY/*DEC_SETUP_TYPE_DEFAULT*/;

    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &pDecSetupCfg->bits_fifo_base);
    pDecSetupCfg->bits_fifo_limit = pDecSetupCfg->bits_fifo_base + VidDecInfo.BitsBufSize - 1U;
    pDecSetupCfg->rbuf_smem_size = 0U;

    pDecSetupCfg->err_handling_mode = DEC_ERR_HANDLING_HALT;
    /*
     * This flag decide DPB buffer size,
     * Set to 2 if we need B frame encode
     * Set to 0, DPB size is inferred from pic size and profile/level
     */
    if (VidDecInfo.MaxFrameWidth >= UHD_8K_WIDTH) {
        /* HierP when 8K */
        pDecSetupCfg->max_frm_num_of_dpb = 0U;
    } else {
        pDecSetupCfg->max_frm_num_of_dpb = 1U;
    }

//Do we need this??
//    (void)AmbaDSP_GetDecModeOnFmt(0, &DecState);
//    if ((DecState != DEC_STATE_IDLE) && (DecState != DEC_STATE_IDLE_WITH_LAST_PIC)) {
//        AmbaLL_LogUInt5("DecState(%d) != IDLE before Setup", DecState, 0U, 0U, 0U, 0U);
//    }

    pDecSetupCfg->enable_pic_info = 1U;

    return Rval;
}

static inline void HL_FillBitsUpdateSetupObsoleted(cmd_decoder_bitsfifo_update_t *pDecUpdateSetup)
{
    pDecUpdateSetup->num_pics = 0U;
}

UINT32 HL_FillBitsUpdateSetup(const UINT16 StreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo, cmd_decoder_bitsfifo_update_t *pDecUpdateSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    pDecUpdateSetup->hdr.decoder_id = (UINT8)VidDecInfo.DecTypeId[VidDecInfo.BitsFormat];
    pDecUpdateSetup->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
    (void)dsp_osal_virt2cli(pBitsFifo->StartAddr, &pDecUpdateSetup->bits_fifo_start);
    (void)dsp_osal_virt2cli(pBitsFifo->EndAddr, &pDecUpdateSetup->bits_fifo_end);

    HL_FillBitsUpdateSetupObsoleted(pDecUpdateSetup);

    return Rval;
}

UINT32 HL_FillDecSpeedSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_speed_t *pSpdDirSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_VOUT_INFO_s VoutAInfo = {0};
    CTX_VOUT_INFO_s VoutBInfo = {0};
    static const UINT16 SpeedMap[11] = {
            0x0100,  /**< playback speed 1x */
            0x0200,  /**< playback speed 2x */
            0x0400,  /**< playback speed 4x */
            0x0800,  /**< playback speed 8x */
            0x1000,  /**< playback speed 16x */
            0x2000,  /**< playback speed 32x */
            0x4000,  /**< playback speed 64x */
            0x0080,  /**< playback speed 1/2x */
            0x0040,  /**< playback speed 1/4x */
            0x0020,  /**< playback speed 1/8x */
            0x0010   /**< playback speed 1/16x */
    };

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /** Setup speed */
    pSpdDirSetup->hdr.decoder_id = (UINT8)VidDecInfo.DecTypeId[VidDecInfo.BitsFormat];

    pSpdDirSetup->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
    pSpdDirSetup->speed = SpeedMap[pStartConfig->SpeedIndex];    /**< AMBA_DSP_VIDDEC_SPEED_xxx */
    (void)dsp_osal_memcpy(&pSpdDirSetup->direction, &pStartConfig->Direction, sizeof(UINT8)); /**< 1: backward */

    HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutAInfo);
    HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutBInfo);

    // only set time_scale & num_units_in_tick when dec only
    if (VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
        if (VoutBInfo.VideoEnable != 0U) {
            pSpdDirSetup->out_strm_num_units_in_tick = VoutBInfo.MixerCfg.FrameRate.NumUnitsInTick;
            pSpdDirSetup->out_strm_time_scale = VoutBInfo.MixerCfg.FrameRate.TimeScale;
        } else if (VoutAInfo.VideoEnable != 0U) {
            pSpdDirSetup->out_strm_num_units_in_tick = VoutAInfo.MixerCfg.FrameRate.NumUnitsInTick;
            pSpdDirSetup->out_strm_time_scale = VoutAInfo.MixerCfg.FrameRate.TimeScale;
        } else {
            pSpdDirSetup->out_strm_time_scale = 0U;
            pSpdDirSetup->out_strm_num_units_in_tick = 0U;
        }
    } else {
        pSpdDirSetup->out_strm_time_scale = 0U;
        pSpdDirSetup->out_strm_num_units_in_tick = 0U;
    }

    return Rval;
}

UINT32 HL_FillDecTrickSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickConfig, cmd_decoder_trickplay_t *pTrickSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /* Setup trick play */
    pTrickSetup->hdr.decoder_id = (UINT8)VidDecInfo.DecTypeId[VidDecInfo.BitsFormat];
    pTrickSetup->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
    pTrickSetup->mode = (UINT32)pTrickConfig->Operation;

    return Rval;
}

static inline void HL_FillDecStartSetupObsoleted(cmd_decoder_start_t *pStartSetup)
{
    pStartSetup->num_pics = 0U;
    pStartSetup->num_frame_decode = 0U;
}

UINT32 HL_FillDecStartSetup(const UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_start_t *pStartSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
#ifdef SUPPORT_VDSP_DEC_AFFINITY
    UINT16 EngAffinity = 0U;
    UINT16 Affinity = 0U;
#endif
    UINT8 SysCfgAffinity = (UINT8)DSP_GetU16Bit(TuneDspSystemCfg.DecVdspAff, (UINT32)DecIdx*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);
    UINT8 SysCfgAffinityOpt = (UINT8)DSP_GetU16Bit(TuneDspSystemCfg.DecVdspAffOpt, (UINT32)DecIdx*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);
    UINT8 SysCfgEngAffinity = (UINT8)DSP_GetU16Bit(TuneDspSystemCfg.DecVdspEngAff, (UINT32)DecIdx*SYS_CFG_VDSP_AFF_LEN, SYS_CFG_VDSP_AFF_LEN);

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);

    /** When multiple decode, must send "h264 decode" command for each decoder. */
    pStartSetup->hdr.decoder_id = (UINT8)VidDecInfo.DecTypeId[VidDecInfo.BitsFormat];
    pStartSetup->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &pStartSetup->bits_fifo_start);
    pStartSetup->bits_fifo_end = (pStartSetup->bits_fifo_start + pStartConfig->PreloadDataSize) - 1U;
    pStartSetup->first_frame_display = pStartConfig->FirstDisplayPTS;

#ifdef SUPPORT_VDSP_DEC_AFFINITY
    EngAffinity = DSP_GetU16Bit(VidDecInfo.EngAffinity, DEC_AFFINITY_CORE_BIT_IDX, DEC_AFFINITY_CORE_LEN);
    Affinity = DSP_GetU16Bit(VidDecInfo.Affinity, DEC_AFFINITY_CORE_BIT_IDX, DEC_AFFINITY_CORE_LEN);
#endif

#ifdef SUPPORT_VDSP_DEC_AFFINITY
    if (EngAffinity == (UINT8)0U) {
        pStartSetup->coding_engine_usage = (UINT8)0U;
//FIXME duplex. or 8K
    } else {
        pStartSetup->coding_engine_usage = EngAffinity;
    }
#else
    pStartSetup->coding_engine_usage = (UINT8)0U;
#endif
    if (SysCfgEngAffinity > (UINT8)0U) {
        pStartSetup->coding_engine_usage = (UINT8)(SysCfgEngAffinity - 1U);
    }

#ifdef SUPPORT_VDSP_DEC_AFFINITY
    if (Affinity == (UINT8)0U) {
        pStartSetup->mdxf_core_usage = (UINT8)0x1U;
    } else {
        pStartSetup->mdxf_core_usage = Affinity;
    }
#else
    if (VidDecInfo.ComplianceCodec == 0U) {
        if (VidDecInfo.MaxFrameWidth > FHD_WIDTH) {
            pStartSetup->mdxf_core_usage = (UINT8)0x3U;
        } else {
            pStartSetup->mdxf_core_usage = (UINT8)0x1U;
        }
    } else {
        pStartSetup->mdxf_core_usage = (UINT8)0x1U;
    }
#endif
    if (SysCfgAffinity > (UINT8)0U) {
        pStartSetup->mdxf_core_usage = SysCfgAffinity;
    }

    pStartSetup->dual_core_mode = (UINT8)0U;
#ifndef SUPPORT_VDSP_DEC_AFFINITY
    if (VidDecInfo.ComplianceCodec == 0U) {
        if ((VidDecInfo.MaxFrameWidth > FHD_WIDTH) &&
            (VidDecInfo.BitsFormat == AMBA_DSP_DEC_BITS_FORMAT_H265) &&
            (VidDecInfo.ComplianceDualCoreMode == DEC_DCMODE_STRIPE)) {
            pStartSetup->dual_core_mode = (UINT8)0x1U;
        } else {
            pStartSetup->dual_core_mode = (UINT8)0x0U;
        }
    } else {
        pStartSetup->dual_core_mode = (UINT8)0x0U;
    }
#endif
    if (SysCfgAffinityOpt > (UINT8)0U) {
        pStartSetup->dual_core_mode = (UINT8)(SysCfgAffinityOpt - 1U);
    }

    HL_FillDecStartSetupObsoleted(pStartSetup);

    return Rval;
}
