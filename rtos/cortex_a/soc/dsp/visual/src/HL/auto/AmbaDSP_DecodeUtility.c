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

UINT32 HL_GetDecVoutTotalBit(void)
{
    UINT16 i;
    UINT32 VoutBitMask = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    /* Display */
    for (i=0; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        if ((Resource->MaxVoutWidth[i] > 0U) || \
            (Resource->MaxOsdBufWidth[i] > 0U)) {
            DSP_SetBit(&VoutBitMask, i);
        }
    }

    return VoutBitMask;
}

#if 0
/**
 * @return 0 use dram preview 1 use smem preview
 */
UINT32 HL_IS_PREVIEW_SMEM_MODE(void)
{
    UINT32 Enable = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);

    if (VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
        // normal video decode

        if (VidDecInfo.MaxFrameWidth <= 1920U) {
            Enable = 1;

            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutInfo);

            // check vout's width is large 1920 or not.
            if (VoutInfo.VideoEnable != 0U) {
                if (VoutInfo.VideoCfg.Window.Width > 1920U) {
                    Enable = 0;
                }
            }
            // TODO: no need to consider LCD width > 1920?
        } else {
            // decode clip's width > 1920
            Enable = 0;
        }
    } else {
        // xcode case
        Enable = 0;
    }

    return Enable;
}
#endif

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

UINT32 HL_FillDecSystemSetup(cmd_dsp_config_t *pSysSetup)
{
    UINT32 Rval = OK;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    static const HL_DSP_SYSTEM_CONFIG_s *DspSystemCfg;
    static const HL_DSP_SYSTEM_CONFIG_s HLDspSystemDecCfg[DSP_CONFIG_PARAM_SET_DEC_NUM] = {
        [DSP_CONFIG_PARAM_SET_DEC] = {
            .DramPar        = 64U,
            .SmemPar        = 32U,
            .SubPar         = 64U,
            .SupPar         = 72U,
            .Fbp            = 32U,
            .Fb             = 512U,
            .Dbp            = 56U,
            .CBuf           = 8U,
            .Bdt            = 14U,
            .Bd             = 480U,
            .ImgInf         = 82U,
            .ExtFb          = 44U,
            .Mcbl           = 6U,
            .Mcb            = 400U,
            .MbufPar        = 0U,
            .Mbuf           = 0U,
            .AikPar         = 4U,
            .MFbp           = 0U,
            .MFb            = 0U,
            .MExtFb         = 0U,
            .MImgInf        = 0U,
            .PgSzLog2       = 0U,
            .IdspSblk       = 9U,
            .FbpCache       = 0U,
            .FbCache        = 0U,
            .ImgInfCache    = 0U,
            .MFbpCache      = 0U,
            .MFbCache       = 0U,
            .MImgInfCache   = 0U,
            .Rsvd           = 0U,
        },
    };
#ifdef SUPPORT_DUMMY_VOUT_THREAD
    UINT8 VoutBit = 0;
#endif
    UINT16 i;
    UINT32 ActualVoutBit;
    UINT16 MaxMixerWidth = 0U;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    if (pSysSetup == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        HL_GetResourcePtr(&Resource);

        pSysSetup->dsp_prof_id = DSP_PROF_STATUS_PLAYBACK;
        pSysSetup->orc_poll_intv = 49155U; // 4ms * 12288, increase 3 by ucode's suggestion

        /* Parameters */
        DspSystemCfg = &HLDspSystemDecCfg[DSP_CONFIG_PARAM_SET_DEC];

        pSysSetup->max_daik_par_num = DspSystemCfg->AikPar;
        pSysSetup->max_sub_par_num = DspSystemCfg->SubPar;
        pSysSetup->max_sup_par_num = DspSystemCfg->SupPar;
        pSysSetup->max_fb_num = DspSystemCfg->Fb;
        pSysSetup->max_dbp_num = DspSystemCfg->Dbp;
        pSysSetup->max_cbuf_num = DspSystemCfg->CBuf;
        pSysSetup->max_mcbl_num = DspSystemCfg->Mcbl;
        pSysSetup->mbuf_par_size = DspSystemCfg->MbufPar;
        pSysSetup->mbuf_size = DspSystemCfg->Mbuf;
        pSysSetup->max_dram_par_num = DspSystemCfg->DramPar;
        pSysSetup->max_smem_par_num = DspSystemCfg->SmemPar;
        pSysSetup->max_fbp_num = DspSystemCfg->Fbp;
        pSysSetup->max_bdt_num = DspSystemCfg->Bdt;
        pSysSetup->max_bd_num = DspSystemCfg->Bd;
        pSysSetup->max_imginf_num = DspSystemCfg->ImgInf;
        pSysSetup->max_ext_fb_num = DspSystemCfg->ExtFb;
        pSysSetup->max_mcb_num = DspSystemCfg->Mcb;
        pSysSetup->max_mfbp_num = DspSystemCfg->MFbp;
        pSysSetup->max_mfb_num = DspSystemCfg->MFb;
        pSysSetup->max_ext_mfb_num = DspSystemCfg->MExtFb;
        pSysSetup->max_mimginf_num = DspSystemCfg->MImgInf;

        AmbaLL_LogUInt5("Page %d", DSP_GetPageSize(), 0U, 0U, 0U, 0U);
        pSysSetup->page_size_k_log2 = (UINT8)DspSystemCfg->PgSzLog2; //dsp default as 3 (Log2(Size) - 10, 0 means 1024, 1 means 2048, 3 means 8192)
//FIXME temporarily sync with ucode suggestion
        pSysSetup->tot_idsp_ld_str_num[0U] = 1U;//0U;
        pSysSetup->tot_idsp_ld_str_num[1U] = 54U;//DspSystemCfg->IdspSblk;
        pSysSetup->tot_idsp_ld_str_num[2U] = 1U;//0U;
        pSysSetup->max_orccode_msg_qs = 20U; //uCode allow Max 255


#define ORC_ALL_MSG_Q_NUM   (24U)
        pSysSetup->max_orc_all_msg_qs = ORC_ALL_MSG_Q_NUM;

        /* Vin Part */
        pSysSetup->vin_bit_mask = 0U;
        pSysSetup->vin0_output_pins = 0U;
        pSysSetup->vin1_output_pins = 0U;
        pSysSetup->vin2_output_pins = 0U;
        pSysSetup->vin3_output_pins = 0U;
        pSysSetup->vin4_output_pins = 0U;
        pSysSetup->vin5_output_pins = 0U;

        /* Vproc part */
        pSysSetup->num_of_vproc_channel = 0;  //MaxViewZoneNum

        /* Encode Part */
        pSysSetup->num_of_enc_channel = 0;    //MaxEncodeStream

        /* Vout resource */
#ifdef SUPPORT_DUMMY_VOUT_THREAD
        ActualVoutBit = HL_GetDecVoutTotalBit();
        (void)ActualVoutBit;

        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            DSP_SetU8Bit(&VoutBit, i);
        }
        pSysSetup->vout_bit_mask = VoutBit;
#else
        ActualVoutBit = (UINT8)HL_GetDecVoutTotalBit();
        /* Per Adam, Pure PB mode always use vout1 thread to drive vout0 thread on postp */
        if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_A, 1U)) {
            DSP_SetU8Bit(&ActualVoutBit, VOUT_IDX_B);
        }

        //for backward compatible
        ActualVoutBit = (ActualVoutBit == 0U)? 3: ActualVoutBit;

        pSysSetup->vout_bit_mask = ActualVoutBit;
#endif
        pSysSetup->vouta_max_pic_ncols = Resource->MaxVoutWidth[VOUT_IDX_A];
        pSysSetup->vouta_max_osd_ncols = Resource->MaxOsdBufWidth[VOUT_IDX_A];

        HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);
        //PostP rotate is equals to vout rotate
        if (HL_IS_ROTATE(VidDecInfo.PostCtlCfg[VOUT_IDX_A].TargetRotateFlip) == 0U) {
            pSysSetup->vouta_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW; //Use ucode default setting, 4rows
        } else {
            pSysSetup->vouta_max_pic_nrows = VOUT_DISP_DEF_ROT_MAX_ROW;
        }
        pSysSetup->vouta_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW;

#if !defined (CONFIG_SOC_CV28)
        pSysSetup->voutb_max_pic_ncols = Resource->MaxVoutWidth[VOUT_IDX_B];
        pSysSetup->voutb_max_osd_ncols = Resource->MaxOsdBufWidth[VOUT_IDX_B];
        pSysSetup->voutb_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW;
        //PostP rotate is equals to vout rotate
        if (HL_IS_ROTATE(VidDecInfo.PostCtlCfg[VOUT_IDX_B].TargetRotateFlip) == 0U) {
            pSysSetup->voutb_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
        } else {
            pSysSetup->voutb_max_pic_nrows = VOUT_DISP_DEF_ROT_MAX_ROW;
        }
#else
        pSysSetup->voutb_max_pic_ncols = (UINT8)0U;
        pSysSetup->voutb_max_osd_ncols = (UINT8)0U;
        pSysSetup->voutb_max_osd_nrows = VOUT_OSD_DEF_MAX_ROW;
        pSysSetup->voutb_max_pic_nrows = VOUT_DISP_DEF_MAX_ROW;
#endif

        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            if (1U == DSP_GetBit(ActualVoutBit, i, 1U)) {
                MaxMixerWidth = MAX2_16(MaxMixerWidth, Resource->MaxVoutWidth[i]);
            }
        }
        pSysSetup->vout_mixer_ncols = MaxMixerWidth;
        pSysSetup->vout_mixer_nrows = 0U; //Use ucode default setting, 3rows

        /* Decode Part */
        pSysSetup->num_of_dec_channel = (UINT8)Resource->DecMaxStreamNum;
        pSysSetup->dec_codec_support = HL_GetDecFmtTotalBit(); // B[0]:AVC, B[1]:HEVC, B[2]:JPG
        pSysSetup->vdec_capture_ena = 0U;

        /* DRAM usage */
        pSysSetup->use_fbp_dram_cache = 1;
        pSysSetup->use_fb_dram_cache = 1;
        pSysSetup->use_imginf_dram_cache = 1;

        pSysSetup->is_partial_load_en = Resource->ParLoadEn;
    }

    return Rval;
}

UINT32 HL_FillDecFlowMaxCfgSetup(cmd_dsp_dec_flow_max_cfg_t *pDecFlowSetup)
{
    UINT32 Rval = OK, MaxGopM = 0U;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    CTX_VOUT_INFO_s VoutInfo;

    HL_GetResourcePtr(&Resource);

    /*
     *  Check Resource->DecMaxRatioOfGopNM B[3:0] for MaximumGopM number
     *  If GopM > 1 , means support Bframe decode, so ref_smem_size need set 856128U (AVC+HEVC)
     *  AVC only:   694080U
     *  HEVC only:  856128U
     *  AVC + HEVC: 856128U
     */
    MaxGopM = DSP_GetU16Bit(Resource->DecMaxRatioOfGopNM, 0U, 4U);
    if (MaxGopM > 1U) {
        //SUPPORT B-frame decode
        pDecFlowSetup->dec_cfg.ref_smem_size = 856128U;
    } else {
        pDecFlowSetup->dec_cfg.ref_smem_size = 0U;
    }
    pDecFlowSetup->dec_cfg.max_frm_width = Resource->DecMaxFrameWidth;
    pDecFlowSetup->dec_cfg.max_frm_height = Resource->DecMaxFrameHeight;
    if (HL_HasDec2Vproc() == 0U) {
        HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutInfo);

        pDecFlowSetup->postp_cfg.alloc_vout0_fbuf = 1U;

        HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);
        if (HL_IS_ROTATE(VidDecInfo.PostCtlCfg[VOUT_IDX_A].TargetRotateFlip) == 0U) {
            pDecFlowSetup->postp_cfg.vout0_fbuf_width = VoutInfo.VideoCfg.Window.Width; //uCode default value is 1280
            pDecFlowSetup->postp_cfg.vout0_fbuf_height = VoutInfo.VideoCfg.Window.Height;
        } else {
            pDecFlowSetup->postp_cfg.vout0_fbuf_width = VoutInfo.VideoCfg.Window.Height;
            pDecFlowSetup->postp_cfg.vout0_fbuf_height = VoutInfo.VideoCfg.Window.Width;
        }
        pDecFlowSetup->dproc_cfg.out_strm[0].yuv_ena = 1U;
        pDecFlowSetup->dproc_cfg.out_strm[1].yuv_ena = 1U;
        pDecFlowSetup->dproc_cfg.out_strm[2].yuv_ena = 1U;
    }

    if (0U != pDecFlowSetup->postp_cfg.alloc_vout0_fbuf) {
        HL_GetVidDecInfo(HL_MTX_OPT_GET, 0U, &VidDecInfo);
        VidDecInfo.PostpOpCfgEnable = 1U;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, 0U, &VidDecInfo);
    }

    if ((0U != pDecFlowSetup->dproc_cfg.out_strm[0].yuv_ena) || \
        (0U != pDecFlowSetup->dproc_cfg.out_strm[1].yuv_ena) || \
        (0U != pDecFlowSetup->dproc_cfg.out_strm[2].yuv_ena)) {
        HL_GetVidDecInfo(HL_MTX_OPT_GET, 0U, &VidDecInfo);
        VidDecInfo.DprocOpCfgEnable = 1U;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, 0U, &VidDecInfo);
    }

    return Rval;
}

static void HL_FillDecBitsStreamSetupCodecTypeSet(cmd_decoder_setup_t *pDecBitsStrmSetup, const CTX_VID_DEC_INFO_s *pVidDecInfo)
{
    if (AMBA_DSP_DEC_BITS_FORMAT_H264 == pVidDecInfo->BitsFormat) {
        pDecBitsStrmSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
    } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == pVidDecInfo->BitsFormat) {
        pDecBitsStrmSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
    } else {
        pDecBitsStrmSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
    }
}

UINT32 HL_FillDecBitsStreamSetup(UINT16 StreamIdx, cmd_decoder_setup_t *pDecBitsStrmSetup)
{
    UINT32 Rval = OK, MaxGopM = 0U;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);
    HL_GetResourcePtr(&Resource);

    // When multiple decode, must send "decode setup" command for each decoder.
    pDecBitsStrmSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    HL_FillDecBitsStreamSetupCodecTypeSet(pDecBitsStrmSetup, &VidDecInfo);

    if (HL_IsDec2Vproc() == 0U) {
        pDecBitsStrmSetup->dec_setup_type = DEC_SETUP_TYPE_DEFAULT;
        LL_SetMsgDispatcherDecoderUsingStatusEos(0);
        pDecBitsStrmSetup->use_dproc_vrscl = 1U;
    } else {
        pDecBitsStrmSetup->dec_setup_type = ((UINT32)Resource->DecMaxStreamNum > 1U)? (UINT32)DEC_SETUP_TYPE_MULTI_CHN:(UINT32)DEC_SETUP_TYPE_AU_BOUND;
        LL_SetMsgDispatcherDecoderUsingStatusEos(1);
        pDecBitsStrmSetup->use_dproc_vrscl = 0U;
    }

    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &pDecBitsStrmSetup->bits_fifo_base);
    pDecBitsStrmSetup->bits_fifo_limit = pDecBitsStrmSetup->bits_fifo_base + VidDecInfo.BitsBufSize - 1U;
    pDecBitsStrmSetup->rbuf_smem_size = 0U;
    pDecBitsStrmSetup->is_regression = 0U;
//visual    SetupCmd.cabac_2_recon_delay = 6U;
    pDecBitsStrmSetup->err_handling_mode = DEC_ERR_HANDLING_HALT;

    /*
     * This flag decide DPB buffer size,
     * Set to 2 if we need B frame encode
     * Set to 0, DPB size is inferred from pic size and profile/level
     *
     * Check Resource->DecMaxRatioOfGopNM B[3:0] for MaximumGopM number
     * If GopM > 1 , means support Bframe decode, so dpb need set to 0
     */
    MaxGopM = DSP_GetU16Bit(Resource->DecMaxRatioOfGopNM, 0U, 4U);
    if (MaxGopM > 1U) {
        //SUPPORT B-frame decode
        pDecBitsStrmSetup->max_frm_num_of_dpb = 0U;
    } else {
        pDecBitsStrmSetup->max_frm_num_of_dpb = 1U;
    }

//Do we need this??
//    (void)AmbaDSP_GetDecModeOnFmt(0, &DecState);
//    if ((DecState != DEC_STATE_IDLE) && (DecState != DEC_STATE_IDLE_WITH_LAST_PIC)) {
//        AmbaLL_LogUInt5("DecState(%d) != IDLE before Setup", DecState, 0U, 0U, 0U, 0U);
//    }

    if (HL_IsDec2Vproc() == 0U) {
        pDecBitsStrmSetup->ext_buf_mask = 0U;
    } else {
        CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
        UINT16 i;
        UINT8 IsExtPymdYuv = 0U, IsExtLndtYuv = 0U;

        HL_GetResourcePtr(&Resource);
        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                if (ViewZoneInfo->PymdIsExtMem > 0U) {
                    IsExtPymdYuv++;
                    break;
                }
            }
        }

        for (i = 0U; i < Resource->MaxViewZoneNum; i++) {
            if (0U == DSP_GetBit(Resource->ViewZoneActiveBit, i, 1U)) {
                continue;
            }
            HL_GetViewZoneInfoPtr(i, &ViewZoneInfo);
            if (ViewZoneInfo->InputFromMemory == VIN_SRC_FROM_DEC) {
                if (ViewZoneInfo->LndtIsExtMem > 0U) {
                    IsExtLndtYuv++;
                    break;
                }
            }
        }

        if ((IsExtPymdYuv > 0U) ||
            (IsExtLndtYuv > 0U)) {
            if (IsExtPymdYuv > 0U) {
                pDecBitsStrmSetup->ext_buf_mask |= ((UINT16)1U<<DSP_BIT_POS_EXT_HIER_0);
            }
            if (IsExtLndtYuv > 0U) {
                pDecBitsStrmSetup->ext_buf_mask |= ((UINT16)1U<<DSP_BIT_POS_EXT_LN_DEC);
            }
        }
    }

    pDecBitsStrmSetup->enable_pic_info = 1U;

    return Rval;
}

UINT32 HL_FillBitsUpdateSetup(const UINT16 StreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo, cmd_decoder_bitsfifo_update_t *DecUpdateSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    if (AMBA_DSP_DEC_BITS_FORMAT_H264 == VidDecInfo.BitsFormat) {
        DecUpdateSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
    } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == VidDecInfo.BitsFormat) {
        DecUpdateSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
    } else {
        DecUpdateSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
    }

    DecUpdateSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;
    (void)dsp_osal_virt2cli(pBitsFifo->StartAddr, &DecUpdateSetup->bits_fifo_start);
    (void)dsp_osal_virt2cli(pBitsFifo->EndAddr, &DecUpdateSetup->bits_fifo_end);

    return Rval;
}

UINT32 HL_FillDecPostCtrlSetup(const UINT16 NumPostCtrl,
                               const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostProcCtrl,
                               cmd_postproc_t *DecPostProcSetup,
                               const UINT8 IsY2Disp)
{
    UINT32 Rval = OK;
    UINT8 Rotate, Flip;
    UINT16 TargetWidth = 0, VoutWidth = 0;
    CTX_VOUT_INFO_s VoutInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo;
    const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl;
    UINT16 i;

    /*
     * 2019/12/24, Per ucoder,
     *   under pure playback mode,
     *   decode_cat_id shall be CAT_DPROC when Y2Disp case,
     *   others shall be CAT_DEC
     */
    DecPostProcSetup->decode_cat_id = (IsY2Disp == (UINT8)1U)? CAT_DPROC: CAT_DEC;
    for (i = 0; i < NumPostCtrl; i++) {
        pPostCtrl = &pPostProcCtrl[i];

        // If dproc exist set center to 0(default value), check input width height from dproc settings
        DecPostProcSetup->input_center_x = 0U;  //pPostCtrl->InputWindow.OffsetX + (pPostCtrl->InputWindow.Width >> 1)
        DecPostProcSetup->input_center_y = 0U;  //pPostCtrl->InputWindow.OffsetY + (pPostCtrl->InputWindow.Height >> 1)

        AmbaLL_LogUInt5("PostCtrlSetup X[%d] W[%d] CtrX[%d]", pPostCtrl->InputWindow.OffsetX, pPostCtrl->InputWindow.Width, DecPostProcSetup->input_center_x, 0U, 0U);
        AmbaLL_LogUInt5("PostCtrlSetup Y[%d] H[%d] CtrY[%d]", pPostCtrl->InputWindow.OffsetY, pPostCtrl->InputWindow.Height, DecPostProcSetup->input_center_y, 0U, 0U);

        //A12 take TargetRotateFlip to set 0x5005 & 7002
        Rotate = HL_GET_ROTATE(pPostCtrl->TargetRotateFlip);
        Flip = HL_GET_FLIP(pPostCtrl->TargetRotateFlip);
        if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
            TargetWidth = pPostCtrl->TargetWindow.Width;
            VoutWidth = pPostCtrl->VoutWindow.Width;
        } else {
            TargetWidth = (UINT16)TRUNCATE(pPostCtrl->TargetWindow.Width, 8U);
            VoutWidth = (UINT16)TRUNCATE(pPostCtrl->VoutWindow.Width, 8U);
        }

        if (pPostProcCtrl[i].VoutIdx == VOUT_IDX_B) {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutInfo);
        } else {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutInfo);
        }

        if (VoutInfo.VideoEnable != 0U) {
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);

            if (pPostProcCtrl[i].VoutIdx == VOUT_IDX_B) {
                if(VidDecInfo.State == VIDDEC_STATE_PAUSE) {
                    DecPostProcSetup->voutB_target_win_update = 2U; // PPVOUT_WIN_UPD_CURR;
                } else {
                    DecPostProcSetup->voutB_target_win_update = 1U; // PPVOUT_WIN_UPD_NEXT;
                }
                if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
                    DecPostProcSetup->voutB_target_win_offset_x = pPostCtrl->TargetWindow.OffsetX;
                    DecPostProcSetup->voutB_target_win_offset_y = pPostCtrl->TargetWindow.OffsetY;
                    DecPostProcSetup->voutB_target_win_width = TargetWidth;
                    DecPostProcSetup->voutB_target_win_height = pPostCtrl->TargetWindow.Height;
                } else {
                    DecPostProcSetup->voutB_target_win_offset_x = pPostCtrl->TargetWindow.OffsetY;
                    DecPostProcSetup->voutB_target_win_offset_y = pPostCtrl->TargetWindow.OffsetX;
                    DecPostProcSetup->voutB_target_win_width = pPostCtrl->TargetWindow.Height;
                    DecPostProcSetup->voutB_target_win_height = TargetWidth;
                }
                // For interlaced vout, suppose vout = 1920x540, input image = 1280x720,
                //   display_win_height will be 540
                //   zoom_factor_y will be (540/360) or (1080/720), not (540/720)
                if ((pPostCtrl->InputWindow.Width == 0U) && (pPostCtrl->InputWindow.Height == 0U)) {
                    DecPostProcSetup->voutB_zoom_factor_x = 2U;
                    DecPostProcSetup->voutB_zoom_factor_y = 2U;
                } else {
                    //UINT32 Size = DecPostProcSetup->voutB_target_win_width;
                    //UINT32 SizeH = pPostCtrl->InputWindow.Height;
                    //DecPostProcSetup->voutB_zoom_factor_x =
                    //        (Size << 16) / (pPostCtrl->InputWindow.Width);
                    //Size = DecPostProcSetup->voutB_target_win_height;
                    //DecPostProcSetup->voutB_zoom_factor_y =
                    //        (Size << 16) / (SizeH >> VoutInfo.MixerCfg.FrameRate.Interlace);

                    // If dproc exist set factor to 1, dproc will take care of dzoom
                    // zoom_factor = 1U means original size, dzoomX1
                    DecPostProcSetup->voutB_zoom_factor_x = 1U;
                    DecPostProcSetup->voutB_zoom_factor_y = 1U;
                }

                // TV on vout 1
                DecPostProcSetup->voutB_enable = 1U;
                DecPostProcSetup->vout1_win_update = 1U;
                DecPostProcSetup->vout1_flip = Flip;
                DecPostProcSetup->vout1_rotate = Rotate;
                DecPostProcSetup->vout1_win_offset_x = pPostCtrl->VoutWindow.OffsetX;
                DecPostProcSetup->vout1_win_offset_y = pPostCtrl->VoutWindow.OffsetY;
                DecPostProcSetup->vout1_win_width = VoutWidth;
                DecPostProcSetup->vout1_win_height = pPostCtrl->VoutWindow.Height;

            } else {    //VoutA
                if(VidDecInfo.State == VIDDEC_STATE_PAUSE) {
                    DecPostProcSetup->voutA_target_win_update = 2U; //PPVOUT_WIN_UPD_CURR;
                } else {
                    DecPostProcSetup->voutA_target_win_update = 1U; //PPVOUT_WIN_UPD_NEXT;
                }
                if (Rotate == DSP_NO_ROTATE) {  // 0 or 180 degree
                    DecPostProcSetup->voutA_target_win_offset_x = pPostCtrl->TargetWindow.OffsetX;
                    DecPostProcSetup->voutA_target_win_offset_y = pPostCtrl->TargetWindow.OffsetY;
                    DecPostProcSetup->voutA_target_win_width = TargetWidth;
                    DecPostProcSetup->voutA_target_win_height = pPostCtrl->TargetWindow.Height;
                } else {
                    DecPostProcSetup->voutA_target_win_offset_x = pPostCtrl->TargetWindow.OffsetY;
                    DecPostProcSetup->voutA_target_win_offset_y = pPostCtrl->TargetWindow.OffsetX;
                    DecPostProcSetup->voutA_target_win_width = pPostCtrl->TargetWindow.Height;
                    DecPostProcSetup->voutA_target_win_height = TargetWidth;
                }

                if ((pPostCtrl->InputWindow.Width == 0U) && (pPostCtrl->InputWindow.Height == 0U)) {
                    DecPostProcSetup->voutA_zoom_factor_x = 2U;
                    DecPostProcSetup->voutA_zoom_factor_y = 2U;
                } else {
                    // For interlaced vout, suppose vout = 960x240, input image = 1280x720,
                    //   sec_display_win_height will be 240
                    //   sec_zoom_factor_y will be (240/360) or (480/720), not (240/720)
                    //UINT32 Size = DecPostProcSetup->voutA_target_win_width;
                    //UINT32 SizeH = pPostCtrl->InputWindow.Height;
                    //DecPostProcSetup->voutA_zoom_factor_x =
                    //    (Size << 16U) / (pPostCtrl->InputWindow.Width);
                    //Size = DecPostProcSetup->voutA_target_win_height;
                    //DecPostProcSetup->voutA_zoom_factor_y =
                    //    (Size << 16U) / (SizeH >> VoutInfo.MixerCfg.FrameRate.Interlace);
                    DecPostProcSetup->voutA_zoom_factor_x = 1U;
                    DecPostProcSetup->voutA_zoom_factor_y = 1U;
                }

                // LCD on vout 0
                DecPostProcSetup->voutA_enable = 1U;
                DecPostProcSetup->vout0_win_update = 1U;
                DecPostProcSetup->vout0_flip = Flip;
                DecPostProcSetup->vout0_rotate = Rotate;
                DecPostProcSetup->vout0_win_offset_x = pPostCtrl->VoutWindow.OffsetX;
                DecPostProcSetup->vout0_win_offset_y = pPostCtrl->VoutWindow.OffsetY;
                DecPostProcSetup->vout0_win_width = VoutWidth;
                DecPostProcSetup->vout0_win_height = pPostCtrl->VoutWindow.Height;
            }
        }
    }

    return Rval;
}

UINT32 HL_FillDecVrsclLayoutSetup(const AMBA_DSP_VIDDEC_POST_CTRL_s *PostProcCmd, cmd_dproc_vrscl_layout_t *VrsclLayoutSetup)
{
    UINT32 Rval = OK;
    UINT16 TargetWidth;
    UINT8 Flip, Rotate, VoutIdx;

    Rotate = HL_GET_ROTATE(PostProcCmd->TargetRotateFlip);
    Flip = HL_GET_FLIP(PostProcCmd->TargetRotateFlip);
    if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
        TargetWidth = PostProcCmd->TargetWindow.Width;
    } else {
        TargetWidth = (UINT16)TRUNCATE(PostProcCmd->TargetWindow.Width, 8U);
    }
    VoutIdx = PostProcCmd->VoutIdx;
    VrsclLayoutSetup->channel_id = 0U;

    if (VoutIdx == VOUT_IDX_B) {
        VrsclLayoutSetup->stream_id = 0U;                       //steam0 -> Vout1
        VrsclLayoutSetup->tar_win_flip = Flip;
        VrsclLayoutSetup->tar_win_rotate = Rotate;
        if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
            VrsclLayoutSetup->tar_win_width = TargetWidth;
            VrsclLayoutSetup->tar_win_height = PostProcCmd->TargetWindow.Height;
        } else {
            VrsclLayoutSetup->tar_win_width = PostProcCmd->TargetWindow.Height;
            VrsclLayoutSetup->tar_win_height = TargetWidth;
        }
        //similar to postp cmd voutA_target_win_update
        //inp_win_update = 1 (update current frame)
        //inp_win_update = 0 (no update)
        VrsclLayoutSetup->inp_win_update = 1U;
        VrsclLayoutSetup->inp_win_offset_x = PostProcCmd->InputWindow.OffsetX;
        VrsclLayoutSetup->inp_win_offset_y = PostProcCmd->InputWindow.OffsetY;
        VrsclLayoutSetup->inp_win_width    = PostProcCmd->InputWindow.Width;
        VrsclLayoutSetup->inp_win_height   = PostProcCmd->InputWindow.Height;
    } else {
        VrsclLayoutSetup->stream_id = 1U;                       //steam1 -> Vout0
        VrsclLayoutSetup->tar_win_flip = Flip;
        VrsclLayoutSetup->tar_win_rotate = Rotate;
        if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
            VrsclLayoutSetup->tar_win_width = TargetWidth;
            VrsclLayoutSetup->tar_win_height = PostProcCmd->TargetWindow.Height;
        } else {
            VrsclLayoutSetup->tar_win_width = PostProcCmd->TargetWindow.Height;
            VrsclLayoutSetup->tar_win_height = TargetWidth;
        }
        //similar to postp cmd voutA_target_win_update
        //inp_win_update = 1 (update current frame)
        //inp_win_update = 0 (no update)
        VrsclLayoutSetup->inp_win_update = 1U;
        VrsclLayoutSetup->inp_win_offset_x = PostProcCmd->InputWindow.OffsetX;
        VrsclLayoutSetup->inp_win_offset_y = PostProcCmd->InputWindow.OffsetY;
        VrsclLayoutSetup->inp_win_width    = PostProcCmd->InputWindow.Width;
        VrsclLayoutSetup->inp_win_height   = PostProcCmd->InputWindow.Height;
    }

    VrsclLayoutSetup->tar_win_offset_x = 0U;
    VrsclLayoutSetup->tar_win_offset_y = 0U;

    return Rval;
}

UINT32 HL_FillDecVrsclWindowSetup(const AMBA_DSP_VIDDEC_POST_CTRL_s *PostProcCmd, cmd_dproc_vrscl_vid_win_t *VrsclWinSetup)
{
    UINT32 Rval = OK;
    UINT16 VoutWidth;
    UINT8 Rotate, VoutIdx;

    Rotate = HL_GET_ROTATE(PostProcCmd->TargetRotateFlip);
    if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
        VoutWidth = PostProcCmd->TargetWindow.Width;
    } else {
        VoutWidth = (UINT16)TRUNCATE(PostProcCmd->TargetWindow.Width, 8U);
    }
    VoutIdx = PostProcCmd->VoutIdx;

    if (VoutIdx == VOUT_IDX_B) {
        VrsclWinSetup->stream_id = 0U;               //steam0 -> Vout1
        if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
            VrsclWinSetup->vid_win_width = VoutWidth;
            VrsclWinSetup->vid_win_height= PostProcCmd->TargetWindow.Height;
            VrsclWinSetup->vid_win_offset_x= PostProcCmd->TargetWindow.OffsetX;
            VrsclWinSetup->vid_win_offset_y= PostProcCmd->TargetWindow.OffsetY;
        } else {
            VrsclWinSetup->vid_win_width = PostProcCmd->TargetWindow.Height;
            VrsclWinSetup->vid_win_height= VoutWidth;
            VrsclWinSetup->vid_win_offset_x= PostProcCmd->TargetWindow.OffsetY;
            VrsclWinSetup->vid_win_offset_y= PostProcCmd->TargetWindow.OffsetX;
        }
        VrsclWinSetup->vid_win_width = VoutWidth;
        VrsclWinSetup->vid_win_height= PostProcCmd->TargetWindow.Height;
        VrsclWinSetup->vid_win_offset_x= PostProcCmd->TargetWindow.OffsetX;
        VrsclWinSetup->vid_win_offset_y= PostProcCmd->TargetWindow.OffsetY;
    } else {
        VrsclWinSetup->stream_id = 1U;              //steam1 -> Vout0
        if (Rotate == DSP_NO_ROTATE) { // 0 or 180 degree
            VrsclWinSetup->vid_win_width = VoutWidth;
            VrsclWinSetup->vid_win_height= PostProcCmd->TargetWindow.Height;
            VrsclWinSetup->vid_win_offset_x= PostProcCmd->TargetWindow.OffsetX;
            VrsclWinSetup->vid_win_offset_y= PostProcCmd->TargetWindow.OffsetY;
        } else {
            VrsclWinSetup->vid_win_width = PostProcCmd->TargetWindow.Height;
            VrsclWinSetup->vid_win_height= VoutWidth;
            VrsclWinSetup->vid_win_offset_x= PostProcCmd->TargetWindow.OffsetY;
            VrsclWinSetup->vid_win_offset_y= PostProcCmd->TargetWindow.OffsetX;
        }
    }
    return Rval;
}

UINT32 HL_FillDecSpeedSetup(const UINT16 ConfigIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_speed_t *SpdDirSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_VOUT_INFO_s VoutAInfo;
    CTX_VOUT_INFO_s VoutBInfo;
    UINT32 ActualVoutBit = 0U;
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

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, ConfigIdx, &VidDecInfo);

    /** Setup speed */
        SpdDirSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;

        if (AMBA_DSP_DEC_BITS_FORMAT_H264 == VidDecInfo.BitsFormat) {
            SpdDirSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
        } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == VidDecInfo.BitsFormat) {
            SpdDirSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
        } else {
            SpdDirSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
        }

        SpdDirSetup->speed = SpeedMap[pStartConfig->SpeedIndex];    /**< AMBA_DSP_VIDDEC_SPEED_xxx */
        (void)dsp_osal_memcpy(&SpdDirSetup->direction, &pStartConfig->Direction, sizeof(UINT8)); /**< 1: backward */

        //check if AMBA_DSP_MAX_VOUT_NUM = 1
        ActualVoutBit = HL_GetDecVoutTotalBit();
        (void)dsp_osal_memset(&VoutAInfo, 0, sizeof(CTX_VOUT_INFO_s));
        (void)dsp_osal_memset(&VoutBInfo, 0, sizeof(CTX_VOUT_INFO_s));
        if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_A, 1U)) {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutAInfo);
        }
        if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_B, 1U)) {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutBInfo);
        }

        // only set time_scale & num_units_in_tick when dec only
        if(VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
            if (VoutBInfo.VideoEnable != 0U) {
                SpdDirSetup->out_strm_num_units_in_tick = VoutBInfo.MixerCfg.FrameRate.NumUnitsInTick;
                SpdDirSetup->out_strm_time_scale = VoutBInfo.MixerCfg.FrameRate.TimeScale;
            } else if (VoutAInfo.VideoEnable != 0U) {
                SpdDirSetup->out_strm_num_units_in_tick = VoutAInfo.MixerCfg.FrameRate.NumUnitsInTick;
                SpdDirSetup->out_strm_time_scale = VoutAInfo.MixerCfg.FrameRate.TimeScale;
            } else {
                SpdDirSetup->out_strm_time_scale = 0U;
                SpdDirSetup->out_strm_num_units_in_tick = 0U;
            }
        } else {
            SpdDirSetup->out_strm_time_scale = 0U;
            SpdDirSetup->out_strm_num_units_in_tick = 0U;
        }

    return Rval;
}

UINT32 HL_FillDecTrickSetup(const UINT16 ConfigIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickConfig, cmd_decoder_trickplay_t *TrickSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, ConfigIdx, &VidDecInfo);

    /* Setup trick play */
    TrickSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;

    if (AMBA_DSP_DEC_BITS_FORMAT_H264 == VidDecInfo.BitsFormat) {
        TrickSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
    } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == VidDecInfo.BitsFormat) {
        TrickSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
    } else {
        TrickSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
    }
    TrickSetup->mode = (UINT32)pTrickConfig[ConfigIdx].Operation;

    return Rval;
}

UINT32 HL_FillDecStartSetup(const UINT16 ConfigIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig, cmd_decoder_start_t *StartSetup)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, ConfigIdx, &VidDecInfo);

    /** When multiple decode, must send "h264 decode" command for each decoder. */
    StartSetup->hdr.decoder_id = (UINT8)VidDecInfo.StreamID;

    if (AMBA_DSP_DEC_BITS_FORMAT_H264 == VidDecInfo.BitsFormat) {
        StartSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
    } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == VidDecInfo.BitsFormat) {
        StartSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
    } else {
        StartSetup->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
    }
    (void)dsp_osal_virt2cli(VidDecInfo.BitsBufAddr, &StartSetup->bits_fifo_start);
    StartSetup->bits_fifo_end = StartSetup->bits_fifo_start + pStartConfig->PreloadDataSize - 1U;
    StartSetup->first_frame_display = pStartConfig->FirstDisplayPTS;

    return Rval;
}
