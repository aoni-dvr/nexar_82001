/**
 *  @file AmbaDSP_CmdsSend.h
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
#ifndef AMBA_DSP_CMDSSEND_H
#define AMBA_DSP_CMDSSEND_H

#include "AmbaDSP_uCode.h"

/**
 * Set DSP command print On/Off
 * This function is called inside AmbaDSP_CmdShow
 * @param [in] CmdCode The DSP command code
 * @param [in] On 1 - set on, 0 - set off
 */
//extern void AmbaLL_CmdShow(UINT32 CmdCode, UINT8 On);

/**
 * Set DSP command print On/Off by whole category
 * This function is called inside AmbaDSP_CmdShowCat
 * @param [in] CmdCat The DSP command category group you want to set
 * @param [in] On 1 - set on, 0 - set off
 */
//extern void AmbaLL_CmdShowCat(UINT8 CmdCat, UINT8 On);

/**
 * Set all DSP command print On/Off
 * This function is called inside AmbaDSP_CmdShowAll
 * @param [in] On 1 - set on, 0 - set off
 */
//extern void AmbaLL_CmdShowAll(UINT8 On);

typedef struct {
    UINT8 WriteMode;
} CmdInfo_t;

/**
 * Set DSP command print On/Off by whole category
 * @param [in] CmdCat The DSP command category group you want to set
 * @param [in] On 1 - set on, 0 - set off
 */
extern void AmbaDSP_CmdShowCat(UINT32 CmdCat, UINT8 On);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DSP_CFG (1)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send DSP configuration command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspConfig(UINT8 WriteMode, cmd_dsp_config_t *pCmdData);

/**
 * Send DSP command that sets HAL info
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspHalInf(UINT8 WriteMode, cmd_dsp_hal_inf_t *pCmdData);

/**
 * Send DSP command that sets suspend profile
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspSuspendProfile(UINT8 WriteMode, cmd_dsp_suspend_profile_t *pCmdData);

/**
 * Send DSP command that sets debug level
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspSetDebugLevel(UINT8 WriteMode, cmd_set_debug_level_t *pCmdData);

/**
 * Send DSP command that sets print thread disable mask
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspPrintThreadDisableMask(UINT8 WriteMode, cmd_print_th_disable_mask_t *pCmdData);

/**
 * Send DSP command that sets binding configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspBindingCfg(UINT8 WriteMode, cmd_binding_cfg_t *pCmdData);

/**
 * Send DSP command that sets activate profile
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspActivateProfile(UINT8 WriteMode, cmd_dsp_activate_profile_t *pCmdData);

/**
 * Send DSP command that sets VProc maximum resource configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspVprocFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vproc_flow_max_cfg_t *pCmdData);

/**
 * Send DSP command that sets VIN maximum resource configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspVinFlowMaxCfg(UINT8 WriteMode, cmd_dsp_vin_flow_max_cfg_t *pCmdData);

/**
 * Send DSP command that sets profile
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspSetProfile(UINT8 WriteMode, cmd_dsp_set_profile_t *pCmdData);

/**
 * Send DSP command that sets encoder maximum resource configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @param [in] MaxStrmNum Max encode stream number
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspEncFlowMaxCfg(UINT8 WriteMode, cmd_dsp_enc_flow_max_cfg_t *pCmdData, const UINT16 MaxStrmNum);

/**
 * Send DSP command that sets decoder maximum resource configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspDecFlowMaxCfg(UINT8 WriteMode, cmd_dsp_dec_flow_max_cfg_t *pCmdData);

#if 0
/**
 * Send DSP command that set None operation
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDspNOP(UINT8 WriteMode, dsp_cmd_t *pCmdData);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VPROC (2)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send VProc configuration command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocConfig(UINT8 WriteMode, cmd_vproc_cfg_t *pCmdData);

/**
 * Send VProc setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocSetup(UINT8 WriteMode, cmd_vproc_setup_t *pCmdData);

/**
 * Send VProc ik config command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocIkConfig(UINT8 WriteMode, cmd_vproc_ik_config_t *pCmdData);

/**
 * Send VProc image pyramid setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocImgPrmdSetup(UINT8 WriteMode, cmd_vproc_img_pyramid_setup_t *pCmdData);

/**
 * Send VProc preview setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocPrevSetup(UINT8 WriteMode, cmd_vproc_prev_setup_t *pCmdData);

/**
 * Send VProc lane detection setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocLaneDetSetup(UINT8 WriteMode, cmd_vproc_lane_det_setup_t *pCmdData);

/**
 * Send VProc external memory setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocSetExtMem(UINT8 WriteMode, cmd_vproc_set_ext_mem_t *pCmdData);

/**
 * Send VProc stop command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocStop(UINT8 WriteMode, cmd_vproc_stop_t *pCmdData);

/**
 * Send VProc osd blend command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocOsdBlend(UINT8 WriteMode, cmd_vproc_osd_blend_t *pCmdData);

/**
 * Send VProc pinout decimation command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocPinOutDecimation(UINT8 WriteMode, cmd_vproc_pin_out_deci_t *pCmdData);

/**
 * Send VProc group command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocGrpCmd(UINT8 WriteMode, cmd_vproc_fov_grp_cmd_t *pCmdData);
#if 0
/**
 * Send VProc echo command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocEchoCmd(UINT8 WriteMode, cmd_vproc_echo_t *pCmdData);

/**
 * Send VProc warp group command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocWarpGroupCmd(UINT8 WriteMode, cmd_vproc_warp_group_update_t *pCmdData);
#endif
/**
 * Send VProc MultiSteam Pp command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocMultiStrmPpCmd(UINT8 WriteMode, cmd_vproc_multi_stream_pp_t *pCmdData);
#if 0
/**
 * Send VProc command that set effet buffer image size
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocSetEffectBufImgSz(UINT8 WriteMode, cmd_vproc_set_effect_buf_img_sz_t *pCmdData);
#endif
/**
 * Send VProc multi channel processing order command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocMultiChanProcOrder(UINT8 WriteMode, cmd_vproc_multi_chan_proc_order_t *pCmdData);

/**
 * Send VProc grouping cmd
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocSetVprocGrping(UINT8 WriteMode, cmd_vproc_set_vproc_grping *pCmdData);

/**
 * Send VProc test frame cmd
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVprocSetTestFrame(UINT8 WriteMode, cmd_vproc_set_testframe_t *pCmdData);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_STILL (3)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VIN (4)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send VIN start command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinStart(UINT8 WriteMode, cmd_vin_start_t *pCmdData);

/**
 * Send VIN idle command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinIdle(UINT8 WriteMode, cmd_vin_idle_t *pCmdData);

#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
/**
 * Send VIN command that sets raw frame capture count
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSetRawFrmCapCnt(UINT8 WriteMode, cmd_vin_set_raw_frm_cap_cnt_t *pCmdData);
#endif

/**
 * Send VIN external memory setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSetExtMem(UINT8 WriteMode, cmd_vin_set_ext_mem_t *pCmdData);

/**
 * Send VIN command that sets sending input data
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSendInputData(UINT8 WriteMode, cmd_vin_send_input_data_t *pCmdData);

/**
 * Send VIN command that initiate raw capture to external buffer
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinInitiateRawCapToExtBuf(UINT8 WriteMode, cmd_vin_initiate_raw_cap_to_ext_buf_t *pCmdData);

/**
 * Send VIN command that attaches processor configuration to a capture frame
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinAttachCfgToCapFrm(UINT8 WriteMode, cmd_vin_attach_proc_cfg_to_cap_frm_t *pCmdData);

/**
 * Send VIN command that attaches sync event to raw
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinAttachEventToRaw(UINT8 WriteMode, cmd_vin_attach_event_to_raw_t *pCmdData);

/**
 * Send VIN command that sets cmd/msg decimation rate
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinMsgDecRate(UINT8 WriteMode, cmd_vin_cmd_msg_dec_rate_t *pCmdData);

/**
 * Send VIN CE layout setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinCeSetup(UINT8 WriteMode, cmd_vin_ce_setup_t *pCmdData);

/**
 * Send VIN HDR setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinHdrSetup(UINT8 WriteMode, cmd_vin_hdr_setup_t *pCmdData);

/**
 * Send VIN command that sets frame level flip/rotation control
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSetFrmLvlFlipRotControl(UINT8 WriteMode, cmd_vin_set_frm_lvl_flip_rot_control_t *pCmdData);

/**
 * Send VIN command that sets FOV layout
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSetFovLayout(UINT8 WriteMode, cmd_vin_set_fov_layout_t *pCmdData);

/**
 * Send VIN command that sets VProc delay time
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinSetFrmVprocDelay(UINT8 WriteMode, cmd_vin_set_frm_vproc_delay_t *pCmdData);

/**
 * Send VIN command that attaches sideband info to a capture frame
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinAttachSbToCapFrm(UINT8 WriteMode, cmd_vin_attach_sideband_info_to_cap_frm_t *pCmdData);

/**
 * Send VIN command that setup vin-vout lock
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinVoutLockSetup(UINT8 WriteMode, cmd_vin_vout_lock_setup_t *pCmdData);
#if 0
/**
 * Send VIN command that attaches metadata
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinAttachMetadata(UINT8 WriteMode, cmd_vin_attach_metadata_t *pCmdData);
#endif
/**
 * Send VIN command that setup temporal demux
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVinTemporalDemuxSetup(UINT8 WriteMode, cmd_vin_temporal_demux_setup_t *pCmdData);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VOUT (5)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send VOUT mixer setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutMixerSetup(UINT8 WriteMode, cmd_vout_mixer_setup_t *pCmdData);

/**
 * Send VOUT video setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutVideoSetup(UINT8 WriteMode, cmd_vout_video_setup_t *pCmdData);
#if 0
/**
 * Send VOUT default image setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutDefaultImgSetup(UINT8 WriteMode, cmd_vout_default_img_setup_t *pCmdData);
#endif
/**
 * Send VOUT OSD setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutOsdSetup(UINT8 WriteMode, cmd_vout_osd_setup_t *pCmdData);
#if 0
/**
 * Send VOUT OSD buffer setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutOsdBufferSetup(UINT8 WriteMode, cmd_vout_osd_buf_setup_t *pCmdData);
#endif
/**
 * Send VOUT OSD CLUT setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutOsdClutSetup(UINT8 WriteMode, cmd_vout_osd_clut_setup_t *pCmdData);

/**
 * Send VOUT display setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutDisplaySetup(UINT8 WriteMode, cmd_vout_display_setup_t *pCmdData);

/**
 * Send VOUT reset command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutReset(UINT8 WriteMode, cmd_vout_reset_t *pCmdData);
#if 0
/**
 * Send VOUT display CSC(Color Space Convert) setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutDisplayCscSetup(UINT8 WriteMode, cmd_vout_display_csc_setup_t *pCmdData);

/**
 * Send VOUT digital output mode setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutDigitalOutputModeSetup(UINT8 WriteMode, cmd_vout_digital_output_mode_setup_t *pCmdData);
#endif
/**
 * Send VOUT gamma setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdVoutGammaSetup(UINT8 WriteMode, cmd_vout_gamma_setup_t *pCmdData);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send encoder setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdEncoderSetup(UINT8 WriteMode, cmd_encoder_setup_t *pCmdData);

/**
 * Send encoder start command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdEncoderStart(UINT8 WriteMode, cmd_encoder_start_t *pCmdData);

/**
 * Send encoder stop command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdEncoderStop(UINT8 WriteMode, cmd_encoder_stop_t *pCmdData);

/**
 * Send encoder realtime setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdEncoderRealtimeSetup(UINT8 WriteMode, encoder_realtime_setup_t *pCmdData);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DEC (7)
\*-----------------------------------------------------------------------------------------------*/

/**
 * Send decoder setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderSetup(UINT8 WriteMode, cmd_decoder_setup_t *pCmdData);

/**
 * Send decoder start command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderStart(UINT8 WriteMode, cmd_decoder_start_t *pCmdData);

/**
 * Send decoder stop command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderStop(UINT8 WriteMode, cmd_decoder_stop_t *pCmdData);

/**
 * Send decoder command that updates bits FIFO
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderBitsfifoUpdate(UINT8 WriteMode, cmd_decoder_bitsfifo_update_t *pCmdData);

/**
 * Send decoder speed setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderSpeed(UINT8 WriteMode, cmd_decoder_speed_t *pCmdData);

/**
 * Send decoder trickplay setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderTrickplay(UINT8 WriteMode, cmd_decoder_trickplay_t *pCmdData);

/**
 * Send decoder still decode setup command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderStilldec(UINT8 WriteMode, cmd_decoder_stilldec_t *pCmdData);

#ifdef SUPPORT_DSP_DEC_BATCH
/**
 * Send decoder batch command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdDecoderBatch(UINT8 WriteMode, cmd_decoder_batch_t *pCmdData);
#endif

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DPROC (8)
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * CAT_IDSP (9)
\*-----------------------------------------------------------------------------------------------*/
/**
 * Send Vin configuration command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdSetVinConfig(UINT8 WriteMode, set_vin_config_t *pCmdData);

#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
/**
 * Send idsp debug command
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 AmbaLL_CmdSendIdspDebugCmd(UINT8 WriteMode, send_idsp_debug_cmd_t *pCmdData);
#endif

/**
 * Send IDSP command that sets Vin master
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 AmbaLL_IdspCmdSetVinMaster(UINT8 WriteMode, set_vin_master_t* pCmdData);

/**
 * Send IDSP command that sets Vin global configuration
 * @param [in] WriteMode Command write mode
 * @param [in] pCmdData The command to be sent
 * @return 0 - OK, others - ErrorCode
 */
//extern UINT32 AmbaLL_IdspCmdSetVinGlobalConfig(UINT8 WriteMode, set_vin_global_config_t* pCmdData);

/*-----------------------------------------------------------------------------------------------*\
 * CAT_POSTPROC (10)
\*-----------------------------------------------------------------------------------------------*/

#endif  /* AMBA_DSP_CMDSSEND_H */
