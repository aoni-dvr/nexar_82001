/**
 *  @file AmbaDSP_Cmds.h
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
 *  @details Definitions & constants for the APIs to generate DSP commands
 *
 */
#ifndef AMBA_DSP_CMDS_H
#define AMBA_DSP_CMDS_H

#include "AmbaDSP_uCode.h"

/*-----------------------------------------------------------------------------------------------*\
 * APIs defined in AmbaDSP_Cmds.c
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DSP_CFG (1)
\*-----------------------------------------------------------------------------------------------*/
extern UINT32 AmbaHL_CmdDspConfig(UINT8 WriteMode, cmd_dsp_config_t *pDspConfig);
extern UINT32 AmbaHL_CmdDspHalInf(UINT8 WriteMode, cmd_dsp_hal_inf_t *pDspHalInf);
extern UINT32 AmbaHL_CmdDspSuspendProfile(UINT8 WriteMode, cmd_dsp_suspend_profile_t *pDspSuspendProfile);
extern UINT32 AmbaHL_CmdDspSetDebugLevel(UINT8 WriteMode, cmd_set_debug_level_t *pDspSetDebugLevel);
extern UINT32 AmbaHL_CmdDspPrintThreadDisableMask(UINT8 WriteMode, cmd_print_th_disable_mask_t *pDspPrintThreadMask);
extern UINT32 AmbaHL_CmdDspBindingCfg(UINT8 WriteMode, cmd_binding_cfg_t *pDspBindingConfig);
extern UINT32 AmbaHL_CmdDspActivateProfile(UINT8 WriteMode, cmd_dsp_activate_profile_t *pDspActivateProfile);
extern UINT32 AmbaHL_CmdDspVprocFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vproc_flow_max_cfg_t *pDspVprocFlowMaxConfig);
extern UINT32 AmbaHL_CmdDspVinFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vin_flow_max_cfg_t *pDspVinFlowMaxConfig);
extern UINT32 AmbaHL_CmdDspSetProfile(UINT8 WriteMode, cmd_dsp_set_profile_t *pDspSetProfile);
extern UINT32 AmbaHL_CmdDspEncFlowMaxCfg(UINT8 WriteMode, cmd_dsp_enc_flow_max_cfg_t *pDspEncFlowMaxCfg);
#if 0
extern UINT32 AmbaHL_CmdDspNOP(UINT8 WriteMode, dsp_cmd_t *pDspNop);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VPROC (2)
\*-----------------------------------------------------------------------------------------------*/
extern UINT32 AmbaHL_CmdVprocConfig(UINT8 WriteMode, cmd_vproc_cfg_t *pVprocConfig);
extern UINT32 AmbaHL_CmdVprocSetup(UINT8 WriteMode, cmd_vproc_setup_t *pVprocSetup);


/**
* Send Video Pyramid setup cmd
* @param [in]  WriteMode write mode
* @param [in]  pVprocImgPrmdSetup pyramid configuration
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdVprocIkConfig(UINT8 WriteMode, cmd_vproc_ik_config_t *pVprocIkConfig);
extern UINT32 AmbaHL_CmdVprocImgPrmdSetup(UINT8 WriteMode, cmd_vproc_img_pyramid_setup_t *pVprocImgPrmdSetup);
extern UINT32 AmbaHL_CmdVprocPrevSetup(UINT8 WriteMode, cmd_vproc_prev_setup_t *pVprocPrevSetup);
extern UINT32 AmbaHL_CmdVprocLaneDetSetup(UINT8 WriteMode, cmd_vproc_lane_det_setup_t *pVprocLaneDetSetup);
extern UINT32 AmbaHL_CmdVprocSetExtMem(UINT8 WriteMode, cmd_vproc_set_ext_mem_t *pVprocSetExtMem);
extern UINT32 AmbaHL_CmdVprocStop(UINT8 WriteMode, cmd_vproc_stop_t *pVprocStop);
extern UINT32 AmbaHL_CmdVprocOsdBlend(UINT8 WriteMode, cmd_vproc_osd_blend_t *pVprocOsdBlend);
extern UINT32 AmbaHL_CmdVprocPinOutDecimation(UINT8 WriteMode, cmd_vproc_pin_out_deci_t *pVprocPinOutDeci);
extern UINT32 AmbaHL_CmdVprocGrpCmd(UINT8 WriteMode, cmd_vproc_fov_grp_cmd_t *pVprocGrpCmd);
#if 0
extern UINT32 AmbaHL_CmdVprocEchoCmd(UINT8 WriteMode, cmd_vproc_echo_t *pVprocEchoCmd);
extern UINT32 AmbaHL_CmdVprocWarpGroupCmd(UINT8 WriteMode, cmd_vproc_warp_group_update_t *pVprocWarpGroupCmd);
#endif
extern UINT32 AmbaHL_CmdVprocMultiStrmPpCmd(UINT8 WriteMode, cmd_vproc_multi_stream_pp_t *pVprocStrmPpCmd);
#if 0
extern UINT32 AmbaHL_CmdVprocSetEffectBufImgSz(UINT8 WriteMode, cmd_vproc_set_effect_buf_img_sz_t *pVprocEffectBufImgSz);
#endif
extern UINT32 AmbaHL_CmdVprocMultiChanProcOrder(UINT8 WriteMode, cmd_vproc_multi_chan_proc_order_t *pVprocMultiChanProcOrder);
extern UINT32 AmbaHL_CmdVprocSetVprocGrping(UINT8 WriteMode, cmd_vproc_set_vproc_grping *pVprocSetVprocGrping);
extern UINT32 AmbaHL_CmdVprocSetTestFrame(UINT8 WriteMode, cmd_vproc_set_testframe_t *pVprocSetTestFrame);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_STILL (3)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VIN (4)
\*-----------------------------------------------------------------------------------------------*/
extern UINT32 AmbaHL_CmdVinStart(UINT8 WriteMode, cmd_vin_start_t *pVinStart);
extern UINT32 AmbaHL_CmdVinIdle(UINT8 WriteMode, cmd_vin_idle_t *pVinIdle);
#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
extern UINT32 AmbaHL_CmdVinSetRawFrmCapCnt(UINT8 WriteMode, cmd_vin_set_raw_frm_cap_cnt_t *pVinSetRawFrmCapCnt);
#endif
extern UINT32 AmbaHL_CmdVinSetExtMem(UINT8 WriteMode, cmd_vin_set_ext_mem_t *pVinSetExtMem);
extern UINT32 AmbaHL_CmdVinSendInputData(UINT8 WriteMode, cmd_vin_send_input_data_t *pVinSendInputData);
extern UINT32 AmbaHL_CmdVinInitiateRawCapToExtBuf(UINT8 WriteMode, cmd_vin_initiate_raw_cap_to_ext_buf_t *pVinInitRawCapToExtBuf);
extern UINT32 AmbaHL_CmdVinAttachEventToRaw(UINT8 WriteMode, cmd_vin_attach_event_to_raw_t *pVinAttachEventToRaw);
extern UINT32 AmbaHL_CmdVinMsgDecRate(UINT8 WriteMode, cmd_vin_cmd_msg_dec_rate_t *pVinCmdMsgDecimationRate);
extern UINT32 AmbaHL_CmdVinCeSetup(UINT8 WriteMode, cmd_vin_ce_setup_t *pVinCeSetup);
extern UINT32 AmbaHL_CmdVinHdrSetup(UINT8 WriteMode, cmd_vin_hdr_setup_t *pVinHdrSetup);
extern UINT32 AmbaHL_CmdVinSetFrmLvlFlipRotControl(UINT8 WriteMode, cmd_vin_set_frm_lvl_flip_rot_control_t *pVinSetFrmLvlFipRotCtrl);
extern UINT32 AmbaHL_CmdVinSetFovLayout(UINT8 WriteMode, cmd_vin_set_fov_layout_t *pVinSetFovLayout);
extern UINT32 AmbaHL_CmdVinSetFrmVprocDelay(UINT8 WriteMode, cmd_vin_set_frm_vproc_delay_t *pVinSetFrmVprocDelay);
extern UINT32 AmbaHL_CmdVinAttachSbToCapFrm(UINT8 WriteMode, cmd_vin_attach_sideband_info_to_cap_frm_t *pVinAttachSidebandToCapFrm);
extern UINT32 AmbaHL_CmdVinAttachCfgToCapFrm(UINT8 WriteMode, cmd_vin_attach_proc_cfg_to_cap_frm_t *pVinAttachProcCfg);
extern UINT32 AmbaHL_CmdVinVoutLockSetup(UINT8 WriteMode, cmd_vin_vout_lock_setup_t *pVinVoutLockSetup);
#if 0
extern UINT32 AmbaHL_CmdVinAttachMetadata(UINT8 WriteMode, cmd_vin_attach_metadata_t *pVinAttachMetadata);
#endif
extern UINT32 AmbaHL_CmdVinTemporalDemuxSetup(UINT8 WriteMode, cmd_vin_temporal_demux_setup_t *pVinTemporalDemuxSetup);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VOUT (5)
\*-----------------------------------------------------------------------------------------------*/
extern UINT32 AmbaHL_CmdVoutMixerSetup(UINT8 WriteMode, cmd_vout_mixer_setup_t *pVoutMixerSetup);
extern UINT32 AmbaHL_CmdVoutVideoSetup(UINT8 WriteMode, cmd_vout_video_setup_t *pVoutVideoSetup);
#if 0
extern UINT32 AmbaHL_CmdVoutDefaultImgSetup(UINT8 WriteMode, cmd_vout_default_img_setup_t *pVoutDefaultImageSetup);
#endif
extern UINT32 AmbaHL_CmdVoutOsdSetup(UINT8 WriteMode, cmd_vout_osd_setup_t *pVoutOsdSetup);
#if 0
extern UINT32 AmbaHL_CmdVoutOsdBufferSetup(UINT8 WriteMode, cmd_vout_osd_buf_setup_t *pVoutOsdBufSetup);
#endif
extern UINT32 AmbaHL_CmdVoutOsdClutSetup(UINT8 WriteMode, cmd_vout_osd_clut_setup_t *pVoutOsdClutSetup);
extern UINT32 AmbaHL_CmdVoutDisplaySetup(UINT8 WriteMode, cmd_vout_display_setup_t *pVoutDisplaySetup);
extern UINT32 AmbaHL_CmdVoutReset(UINT8 WriteMode, cmd_vout_reset_t *pVoutReset);
#if 0
extern UINT32 AmbaHL_CmdVoutDisplayCscSetup(UINT8 WriteMode, cmd_vout_display_csc_setup_t *pVoutDisplayCscSetup);
extern UINT32 AmbaHL_CmdVoutDigitalOutputModeSetup(UINT8 WriteMode, cmd_vout_digital_output_mode_setup_t *pVoutDigitalOutputModeSetup);
#endif
extern UINT32 AmbaHL_CmdVoutGammaSetup(UINT8 WriteMode, cmd_vout_gamma_setup_t *pVoutGammaSetup);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/
/**
* Send encode setup command
* @param [in]  WriteMode write mode
* @param [in]  pEncoderSetup encode setup
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdEncoderSetup(UINT8 WriteMode, cmd_encoder_setup_t *pEncoderSetup);

/**
* Send encode start command
* @param [in]  WriteMode write mode
* @param [in]  pEncoderStart encode start
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdEncoderStart(UINT8 WriteMode, cmd_encoder_start_t *pEncoderStart);

/**
* Send encode stop command
* @param [in]  WriteMode write mode
* @param [in]  pEncoderStop encode stop
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdEncoderStop(UINT8 WriteMode, cmd_encoder_stop_t *pEncoderStop);

/**
* Send encode realtime setup command
* @param [in]  WriteMode write mode
* @param [in]  pRealTimeParamSetup real time encode setup
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdEncoderRealtimeSetup(UINT8 WriteMode, encoder_realtime_setup_t *pRealTimeParamSetup);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DEC (7)
\*-----------------------------------------------------------------------------------------------*/
/**
* Send Decode Flow Max Config command
* @param [in]  WriteMode write mode
* @param [in]  pDecFlowCfg Decode Flow setup
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDspDecFlowMaxCfg(UINT8 WriteMode, cmd_dsp_dec_flow_max_cfg_t *pDecFlowCfg);

/**
* Send Decoder Setup command
* @param [in]  WriteMode write mode
* @param [in]  pDecoderSetup Decode setup structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderSetup(UINT8 WriteMode, cmd_decoder_setup_t *pDecoderSetup);

/**
* Send Decoder Speed command
* @param [in]  WriteMode write mode
* @param [in]  pDecSpeed Decode Speed structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderSpeed(UINT8 WriteMode, cmd_decoder_speed_t *pDecSpeed);

/**
* Send Decoder Trick Play command
* @param [in]  WriteMode write mode
* @param [in]  pDecTrick Decode Trick Play structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderTrickPlay(UINT8 WriteMode, cmd_decoder_trickplay_t *pDecTrick);

/**
* Send Decoder Still decode command
* @param [in]  WriteMode write mode
* @param [in]  pDecStlDec Decode still decode structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderStilldec(UINT8 WriteMode, cmd_decoder_stilldec_t *pDecStlDec);

#ifdef SUPPORT_DSP_DEC_BATCH
/**
* Send Decoder batch command
* @param [in]  WriteMode write mode
* @param [in]  pDecBch Decode batch info structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderBatch(UINT8 WriteMode, cmd_decoder_batch_t *pDecBch);
#endif

/**
* Send Decoder Start command
* @param [in]  WriteMode write mode
* @param [in]  pDecStart Decode Start structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderStart(UINT8 WriteMode, cmd_decoder_start_t *pDecStart);

/**
* Send Decoder Stop command
* @param [in]  WriteMode write mode
* @param [in]  pDecStop Decode Stop structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderStop(UINT8 WriteMode, cmd_decoder_stop_t *pDecStop);

/**
* Send Decoder Bits Fifo Update command
* @param [in]  WriteMode write mode
* @param [in]  pDecUpdate Decode Bits Fifo Update structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdDecoderBitsfifoUpdate(UINT8 WriteMode, cmd_decoder_bitsfifo_update_t *pDecUpdate);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DPROC (8)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DECPROC (8)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_IDSP (9)
\*-----------------------------------------------------------------------------------------------*/
/**
* Send Vin config command
* @param [in]  WriteMode write mode
* @param [in]  set_vin_config_t vin config structure
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdSetVinConfig(UINT8 WriteMode, set_vin_config_t *pVinConfig);

#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
/**
* Send Idsp debug command
* @param [in]  WriteMode write mode
* @param [in]  pIdspDebug idsp debug config
* @return ErrorCode
*/
extern UINT32 AmbaHL_CmdSendIdspDebugCmd(UINT8 WriteMode, send_idsp_debug_cmd_t *pIdspDebug);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_POSTPROC (10)
\*-----------------------------------------------------------------------------------------------*/
 
#endif  /* AMBA_DSP_CMDS_H */
