/**
 *  @file AmbaDSP_CmdCheck.c
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
 *  @details ARM-DSP state machine module
 *
 */

#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_CmdsSend.h"
#include "AmbaDSP_ContextUtility.h"

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DSP_CFG (1)
\*-----------------------------------------------------------------------------------------------*/

UINT32 AmbaHL_CmdDspConfig(UINT8 WriteMode, cmd_dsp_config_t *pDspConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspConfig(WriteMode, pDspConfig);
    return Rval;
}

UINT32 AmbaHL_CmdDspHalInf(UINT8 WriteMode, cmd_dsp_hal_inf_t *pDspHalInf)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspHalInf(WriteMode, pDspHalInf);
    return Rval;
}

UINT32 AmbaHL_CmdDspSuspendProfile(UINT8 WriteMode, cmd_dsp_suspend_profile_t *pDspSuspendProfile)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspSuspendProfile(WriteMode, pDspSuspendProfile);
    return Rval;
}

UINT32 AmbaHL_CmdDspSetDebugLevel(UINT8 WriteMode, cmd_set_debug_level_t *pDspSetDebugLevel)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspSetDebugLevel(WriteMode, pDspSetDebugLevel);

    return Rval;
}

UINT32 AmbaHL_CmdDspPrintThreadDisableMask(UINT8 WriteMode, cmd_print_th_disable_mask_t *pDspPrintThreadMask)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspPrintThreadDisableMask(WriteMode, pDspPrintThreadMask);

    return Rval;
}

UINT32 AmbaHL_CmdDspBindingCfg(UINT8 WriteMode, cmd_binding_cfg_t *pDspBindingConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspBindingCfg(WriteMode, pDspBindingConfig);

    return Rval;
}

UINT32 AmbaHL_CmdDspActivateProfile(UINT8 WriteMode, cmd_dsp_activate_profile_t *pDspActivateProfile)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspActivateProfile(WriteMode, pDspActivateProfile);
    return Rval;
}

UINT32 AmbaHL_CmdDspVprocFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vproc_flow_max_cfg_t *pDspVprocFlowMaxConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspVprocFlowMaxCfg(WriteMode, pDspVprocFlowMaxConfig);
    return Rval;
}

UINT32 AmbaHL_CmdDspVinFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vin_flow_max_cfg_t *pDspVinFlowMaxConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspVinFlowMaxCfg(WriteMode, pDspVinFlowMaxConfig);
    return Rval;
}

UINT32 AmbaHL_CmdDspSetProfile(UINT8 WriteMode, cmd_dsp_set_profile_t *pDspSetProfile)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspSetProfile(WriteMode, pDspSetProfile);
    return Rval;
}

UINT32 AmbaHL_CmdDspEncFlowMaxCfg(UINT8 WriteMode, cmd_dsp_enc_flow_max_cfg_t *pDspEncFlowMaxCfg)
{
    UINT32 Rval;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);
    Rval = AmbaLL_CmdDspEncFlowMaxCfg(WriteMode, pDspEncFlowMaxCfg, Resource->MaxEncodeStream);

    return Rval;
}

UINT32 AmbaHL_CmdDspDecFlowMaxCfg(UINT8 WriteMode, cmd_dsp_dec_flow_max_cfg_t *pDecFlowCfg)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspDecFlowMaxCfg(WriteMode, pDecFlowCfg);

    return Rval;
}

UINT32 AmbaHL_CmdDspNOP(UINT8 WriteMode, dsp_cmd_t *pDspNop)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspNOP(WriteMode, pDspNop);

    return Rval;
}

UINT32 AmbaHL_CmdDspVoutFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vout_flow_max_cfg_t *pVoutFlowCfg)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDspVoutFlowMaxCfg(WriteMode, pVoutFlowCfg);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VPROC (2)
\*-----------------------------------------------------------------------------------------------*/

UINT32 AmbaHL_CmdVprocConfig(UINT8 WriteMode, cmd_vproc_cfg_t *pVprocConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocConfig(WriteMode, pVprocConfig);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetup(UINT8 WriteMode, cmd_vproc_setup_t *pVprocSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetup(WriteMode, pVprocSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVprocIkConfig(UINT8 WriteMode, cmd_vproc_ik_config_t *pVprocIkConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocIkConfig(WriteMode, pVprocIkConfig);
    return Rval;
}

UINT32 AmbaHL_CmdVprocImgPrmdSetup(UINT8 WriteMode, cmd_vproc_img_pyramid_setup_t *pVprocImgPrmdSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocImgPrmdSetup(WriteMode, pVprocImgPrmdSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVprocPrevSetup(UINT8 WriteMode, cmd_vproc_prev_setup_t *pVprocPrevSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocPrevSetup(WriteMode, pVprocPrevSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVprocLaneDetSetup(UINT8 WriteMode, cmd_vproc_lane_det_setup_t *pVprocLaneDetSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocLaneDetSetup(WriteMode, pVprocLaneDetSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetExtMem(UINT8 WriteMode, cmd_vproc_set_ext_mem_t *pVprocSetExtMem)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetExtMem(WriteMode, pVprocSetExtMem);
    return Rval;
}

UINT32 AmbaHL_CmdVprocStop(UINT8 WriteMode, cmd_vproc_stop_t *pVprocStop)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocStop(WriteMode, pVprocStop);
    return Rval;
}

UINT32 AmbaHL_CmdVprocOsdBlend(UINT8 WriteMode, cmd_vproc_osd_blend_t *pVprocOsdBlend)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocOsdBlend(WriteMode, pVprocOsdBlend);
    return Rval;
}

UINT32 AmbaHL_CmdVprocPinOutDecimation(UINT8 WriteMode, cmd_vproc_pin_out_deci_t *pVprocPinOutDeci)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocPinOutDecimation(WriteMode, pVprocPinOutDeci);
    return Rval;
}

UINT32 AmbaHL_CmdVprocGrpCmd(UINT8 WriteMode, cmd_vproc_fov_grp_cmd_t *pVprocGrpCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocGrpCmd(WriteMode, pVprocGrpCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocEchoCmd(UINT8 WriteMode, cmd_vproc_echo_t *pVprocEchoCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocEchoCmd(WriteMode, pVprocEchoCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocWarpGroupCmd(UINT8 WriteMode, cmd_vproc_warp_group_update_t *pVprocWarpGroupCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocWarpGroupCmd(WriteMode, pVprocWarpGroupCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocOsdInsertCmd(UINT8 WriteMode, cmd_vproc_osd_insert_t *pVprocOsdInsertCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocOsdInsertCmd(WriteMode, pVprocOsdInsertCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetPrivacyMaskCmd(UINT8 WriteMode, cmd_vproc_set_privacy_mask_t *pVprocOsdInsertCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetPrivacyMaskCmd(WriteMode, pVprocOsdInsertCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetStreamDelayCmd(UINT8 WriteMode, cmd_vproc_set_streams_delay_t *pVprocSetStreamsDelayCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetStreamDelayCmd(WriteMode, pVprocSetStreamsDelayCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocMultiStrmPpCmd(UINT8 WriteMode, cmd_vproc_multi_stream_pp_t *pVprocStrmPpCmd)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocMultiStrmPpCmd(WriteMode, pVprocStrmPpCmd);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetEffectBufImgSz(UINT8 WriteMode, cmd_vproc_set_effect_buf_img_sz_t *pVprocEffectBufImgSz)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetEffectBufImgSz(WriteMode, pVprocEffectBufImgSz);
    return Rval;
}

UINT32 AmbaHL_CmdVprocMultiChanProcOrder(UINT8 WriteMode, cmd_vproc_multi_chan_proc_order_t *pVprocMultiChanProcOrder)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocMultiChanProcOrder(WriteMode, pVprocMultiChanProcOrder);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetVprocGrping(UINT8 WriteMode, cmd_vproc_set_vproc_grping *pVprocSetVprocGrping)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetVprocGrping(WriteMode, pVprocSetVprocGrping);
    return Rval;
}

UINT32 AmbaHL_CmdVprocSetTestFrame(UINT8 WriteMode, cmd_vproc_set_testframe_t *pVprocSetTestFrame)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVprocSetTestFrame(WriteMode, pVprocSetTestFrame);
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_STILL (3)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VIN (4)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaHL_CmdVinStart(UINT8 WriteMode, cmd_vin_start_t *pVinStart)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinStart(WriteMode, pVinStart);

    return Rval;
}

UINT32 AmbaHL_CmdVinIdle(UINT8 WriteMode, cmd_vin_idle_t *pVinIdle)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinIdle(WriteMode, pVinIdle);

    return Rval;
}

#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
UINT32 AmbaHL_CmdVinSetRawFrmCapCnt(UINT8 WriteMode, cmd_vin_set_raw_frm_cap_cnt_t *pVinSetRawFrmCapCnt)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSetRawFrmCapCnt(WriteMode, pVinSetRawFrmCapCnt);

    return Rval;
}
#endif

UINT32 AmbaHL_CmdVinSetExtMem(UINT8 WriteMode, cmd_vin_set_ext_mem_t *pVinSetExtMem)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSetExtMem(WriteMode, pVinSetExtMem);

    return Rval;
}

UINT32 AmbaHL_CmdVinSendInputData(UINT8 WriteMode, cmd_vin_send_input_data_t *pVinSendInputData)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSendInputData(WriteMode, pVinSendInputData);

    return Rval;
}

UINT32 AmbaHL_CmdVinInitiateRawCapToExtBuf(UINT8 WriteMode, cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCapToExtBuf)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinInitiateRawCapToExtBuf(WriteMode, pVinInitRawCapToExtBuf);

    return Rval;
}

UINT32 AmbaHL_CmdVinAttachCfgToCapFrm(UINT8 WriteMode, cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinAttachCfgToCapFrm(WriteMode, pVinAttachProcCfg);

    return Rval;
}

UINT32 AmbaHL_CmdVinAttachEventToRaw(UINT8 WriteMode, cmd_vin_attach_event_to_raw_t *pVinAttachEventToRaw)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinAttachEventToRaw(WriteMode, pVinAttachEventToRaw);

    return Rval;
}

UINT32 AmbaHL_CmdVinMsgDecRate(UINT8 WriteMode, cmd_vin_cmd_msg_dec_rate_t *pVinCmdMsgDecimationRate)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinMsgDecRate(WriteMode, pVinCmdMsgDecimationRate);

    return Rval;
}

UINT32 AmbaHL_CmdVinCeSetup(UINT8 WriteMode, cmd_vin_ce_setup_t *pVinCeSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinCeSetup(WriteMode, pVinCeSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVinHdrSetup(UINT8 WriteMode, cmd_vin_hdr_setup_t *pVinHdrSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinHdrSetup(WriteMode, pVinHdrSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVinSetFrmLvlFlipRotControl(UINT8 WriteMode, cmd_vin_set_frm_lvl_flip_rot_control_t *pVinSetFrmLvlFipRotCtrl)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSetFrmLvlFlipRotControl(WriteMode, pVinSetFrmLvlFipRotCtrl);

    return Rval;
}

UINT32 AmbaHL_CmdVinSetFovLayout(UINT8 WriteMode, cmd_vin_set_fov_layout_t *pVinSetFovLayout)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSetFovLayout(WriteMode, pVinSetFovLayout);

    return Rval;
}

UINT32 AmbaHL_CmdVinSetFrmVprocDelay(UINT8 WriteMode, cmd_vin_set_frm_vproc_delay_t *pVinSetFrmVprocDelay)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinSetFrmVprocDelay(WriteMode, pVinSetFrmVprocDelay);

    return Rval;
}

UINT32 AmbaHL_CmdVinAttachSbToCapFrm(UINT8 WriteMode, cmd_vin_attach_sideband_info_to_cap_frm_t *pVinAttachSidebandToCapFrm)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinAttachSbToCapFrm(WriteMode, pVinAttachSidebandToCapFrm);

    return Rval;
}

UINT32 AmbaHL_CmdVinVoutLockSetup(UINT8 WriteMode, cmd_vin_vout_lock_setup_t *pVinVoutLockSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinVoutLockSetup(WriteMode, pVinVoutLockSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVinAttachMetadata(UINT8 WriteMode, cmd_vin_attach_metadata_t *pVinAttachMetadata)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinAttachMetadata(WriteMode, pVinAttachMetadata);

    return Rval;
}

UINT32 AmbaHL_CmdVinTemporalDemuxSetup(UINT8 WriteMode, cmd_vin_temporal_demux_setup_t *pVinTemporalDemuxSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinTemporalDemuxSetup(WriteMode, pVinTemporalDemuxSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVinPatternDemuxSetup(UINT8 WriteMode, cmd_vin_pattern_demux_setup_t *pVinPatternDemuxSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVinPatternDemuxSetup(WriteMode, pVinPatternDemuxSetup);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VOUT (5)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaHL_CmdVoutMixerSetup(UINT8 WriteMode, cmd_vout_mixer_setup_t *pVoutMixerSetup)
{
    UINT32 Rval = AmbaLL_CmdVoutMixerSetup(WriteMode, pVoutMixerSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVoutVideoSetup(UINT8 WriteMode, cmd_vout_video_setup_t *pVoutVideoSetup)
{
    UINT32 Rval = AmbaLL_CmdVoutVideoSetup(WriteMode, pVoutVideoSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVoutDefaultImgSetup(UINT8 WriteMode, cmd_vout_default_img_setup_t *pVoutDefaultImageSetup)
{
    UINT32 Rval = AmbaLL_CmdVoutDefaultImgSetup(WriteMode, pVoutDefaultImageSetup);
    return Rval;
}

UINT32 AmbaHL_CmdVoutOsdSetup(UINT8 WriteMode, cmd_vout_osd_setup_t *pVoutOsdSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutOsdSetup(WriteMode, pVoutOsdSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutOsdBufferSetup(UINT8 WriteMode, cmd_vout_osd_buf_setup_t *pVoutOsdBufSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutOsdBufferSetup(WriteMode, pVoutOsdBufSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutOsdClutSetup(UINT8 WriteMode, cmd_vout_osd_clut_setup_t *pVoutOsdClutSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutOsdClutSetup(WriteMode, pVoutOsdClutSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutDisplaySetup(UINT8 WriteMode, cmd_vout_display_setup_t *pVoutDisplaySetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutDisplaySetup(WriteMode, pVoutDisplaySetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutDveSetup(UINT8 WriteMode, cmd_vout_dve_setup_t *pVoutDveSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutDveSetup(WriteMode, pVoutDveSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutReset(UINT8 WriteMode, cmd_vout_reset_t *pVoutReset)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutReset(WriteMode, pVoutReset);

    return Rval;
}

UINT32 AmbaHL_CmdVoutDisplayCscSetup(UINT8 WriteMode, cmd_vout_display_csc_setup_t *pVoutDisplayCscSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutDisplayCscSetup(WriteMode, pVoutDisplayCscSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutDigitalOutputModeSetup(UINT8 WriteMode, cmd_vout_digital_output_mode_setup_t *pVoutDigitalOutputModeSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutDigitalOutputModeSetup(WriteMode, pVoutDigitalOutputModeSetup);

    return Rval;
}

UINT32 AmbaHL_CmdVoutGammaSetup(UINT8 WriteMode, cmd_vout_gamma_setup_t *pVoutGammaSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdVoutGammaSetup(WriteMode, pVoutGammaSetup);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaHL_CmdEncoderSetup(UINT8 WriteMode, cmd_encoder_setup_t *pEncoderSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdEncoderSetup(WriteMode, pEncoderSetup);

    return Rval;
}

UINT32 AmbaHL_CmdEncoderStart(UINT8 WriteMode, cmd_encoder_start_t *pEncoderStart)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdEncoderStart(WriteMode, pEncoderStart);

    return Rval;
}

UINT32 AmbaHL_CmdEncoderStop(UINT8 WriteMode, cmd_encoder_stop_t *pEncoderStop)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdEncoderStop(WriteMode, pEncoderStop);

    return Rval;
}

UINT32 AmbaHL_CmdEncoderJpegSetup(UINT8 WriteMode, cmd_encoder_jpeg_setup_t *pEncoderJpegSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdEncoderJpegSetup(WriteMode, pEncoderJpegSetup);

    return Rval;
}

UINT32 AmbaHL_CmdSetVinConfig(UINT8 WriteMode, set_vin_config_t *pVinConfig)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdSetVinConfig(WriteMode, pVinConfig);

    return Rval;
}

UINT32 AmbaHL_CmdSetHdsCompression(UINT8 WriteMode, const lossy_compression_t *pHdsCmpr)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdSetHdsCompression(WriteMode, pHdsCmpr);

    return Rval;
}

UINT32 AmbaHL_CmdEncoderRealtimeSetup(UINT8 WriteMode, encoder_realtime_setup_t *pRealTimeParamSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdEncoderRealtimeSetup(WriteMode, pRealTimeParamSetup);

    return Rval;
}

#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
UINT32 AmbaHL_CmdSendIdspDebugCmd(UINT8 WriteMode, send_idsp_debug_cmd_t *pIdspDebug)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdSendIdspDebugCmd(WriteMode, pIdspDebug);

    return Rval;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DEC (7)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaHL_CmdDecoderSetup(UINT8 WriteMode, cmd_decoder_setup_t *pDecoderSetup)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderSetup(WriteMode, pDecoderSetup);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderSpeed(UINT8 WriteMode, cmd_decoder_speed_t *pDecSpeed)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderSpeed(WriteMode, pDecSpeed);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderTrickPlay(UINT8 WriteMode, cmd_decoder_trickplay_t *pDecTrick)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderTrickplay(WriteMode, pDecTrick);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderStilldec(UINT8 WriteMode, cmd_decoder_stilldec_t *pDecStlDec)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderStilldec(WriteMode, pDecStlDec);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderBatch(UINT8 WriteMode, cmd_decoder_batch_t *pDecBch)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderBatch(WriteMode, pDecBch);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderStart(UINT8 WriteMode, cmd_decoder_start_t *pDecStart)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderStart(WriteMode, pDecStart);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderStop(UINT8 WriteMode, cmd_decoder_stop_t *pDecStop)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderStop(WriteMode, pDecStop);

    return Rval;
}

UINT32 AmbaHL_CmdDecoderBitsfifoUpdate(UINT8 WriteMode, cmd_decoder_bitsfifo_update_t *pDecUpdate)
{
    UINT32 Rval;

    Rval = AmbaLL_CmdDecoderBitsfifoUpdate(WriteMode, pDecUpdate);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DPROC (8)
\*-----------------------------------------------------------------------------------------------*/
 
/*-----------------------------------------------------------------------------------------------*\
 * CAT_POSTPROC (10)
\*-----------------------------------------------------------------------------------------------*/
 
