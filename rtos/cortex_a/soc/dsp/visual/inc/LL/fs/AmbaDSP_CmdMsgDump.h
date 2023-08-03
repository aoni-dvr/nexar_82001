/**
*  @file AmbaDSP_CmdMsgDump.h
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
*  @details DSP command message dump defines.
*
*/

#ifndef AMBA_DUMP_DSP_CMD_H
#define AMBA_DUMP_DSP_CMD_H

#include "AmbaDSP_uCode.h"

#if defined (CONFIG_ENABLE_DSP_COMMAND_PARM_DBG) || defined (CONFIG_LINUX)
extern void CmdWriteDump(const char* x, UINT32 CmdCode, UINT32 WrType);
#endif

/**
 * Dump DSP configuration command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_config_t(const cmd_dsp_config_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP HAL(Hardware Abstraction Layer) information command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_hal_inf_t(const cmd_dsp_hal_inf_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets suspend profile
 * @param [in] pCmdData A DSP suspend profile command
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_suspend_profile_t(const cmd_dsp_suspend_profile_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets debug level
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_set_debug_level_t(const cmd_set_debug_level_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets print thread disable mask
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_print_th_disable_mask_t(const cmd_print_th_disable_mask_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets binding configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_binding_cfg_t(const cmd_binding_cfg_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets activate profile
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_activate_profile_t(const cmd_dsp_activate_profile_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets VProc maximum resource configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_vproc_flow_max_cfg_t(const cmd_dsp_vproc_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets VIN maximum resource configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_vin_flow_max_cfg_t(const cmd_dsp_vin_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets profile
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_set_profile_t(const cmd_dsp_set_profile_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump DSP command data that sets encoder maximum resource configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 * @param [in] MaxStrmNum max. encode stream number
 */
extern void dump_cmd_dsp_enc_flow_max_cfg_t(const cmd_dsp_enc_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info, const UINT16 MaxStrmNum);

/**
 * Dump DSP command data that sets decoder maximum resource configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_dec_flow_max_cfg_t(const cmd_dsp_dec_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info);

#if 0
/**
 * Dump DSP command data that None operation
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_dsp_nop_t(const dsp_cmd_t *pCmdData, const CmdInfo_t *Info);
#endif

/**
 * Dump VProc configuration command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_cfg_t(const cmd_vproc_cfg_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_setup_t(const cmd_vproc_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc ik config command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_ik_config_t(const cmd_vproc_ik_config_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc image pyramid setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_img_prmd_setup_t(const cmd_vproc_img_pyramid_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc preview setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_prev_setup_t(const cmd_vproc_prev_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc lane detection setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_lane_det_setup_t(const cmd_vproc_lane_det_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc external memory setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_set_ext_mem_t(const cmd_vproc_set_ext_mem_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc stop command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_stop_t(const cmd_vproc_stop_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc osd blend command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_osb_blend_t(const cmd_vproc_osd_blend_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc pinout decimation data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_pin_out_deci_t(const cmd_vproc_pin_out_deci_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc group command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_fov_grp_cmd_t(const cmd_vproc_fov_grp_cmd_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump VProc echo command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_echo_cmd_t(const cmd_vproc_echo_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc warp group command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_warp_group_cmd_t(const cmd_vproc_warp_group_update_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump VProc multi stream command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_multi_stream_pp_t(const cmd_vproc_multi_stream_pp_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump VProc command data that set effet buffer image size
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_set_effect_buf_img_sz_t(const cmd_vproc_set_effect_buf_img_sz_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump VProc multi channel processing order command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_multi_chan_proc_order_t(const cmd_vproc_multi_chan_proc_order_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc grouping command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_set_vproc_grping_t(const cmd_vproc_set_vproc_grping *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VProc set testframe command
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vproc_set_testframe_t(const cmd_vproc_set_testframe_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN start command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_start_t(const cmd_vin_start_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN idle command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_idle_t(const cmd_vin_idle_t *pCmdData, const CmdInfo_t *Info);

#ifdef SUPPORT_DSP_SET_RAW_CAP_CNT
/**
 * Dump VIN command data that sets raw frame capture count
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_set_raw_frm_cap_cnt_t(const cmd_vin_set_raw_frm_cap_cnt_t *pCmdData, const CmdInfo_t *Info);
#endif

/**
 * Dump VIN external memory setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_set_ext_mem_t(const cmd_vin_set_ext_mem_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that sets sending input data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_send_input_data_t(const cmd_vin_send_input_data_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that initiate raw capture to external buffer
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_initiate_raw_cap_to_ext_buf_t(const cmd_vin_initiate_raw_cap_to_ext_buf_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that attaches processor configuration to a capture frame
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_attach_proc_cfg_to_cap_frm_t(const cmd_vin_attach_proc_cfg_to_cap_frm_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that attaches sync event to raw
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_attach_event_to_raw_t(const cmd_vin_attach_event_to_raw_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that sets cmd/msg decimation rate
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_cmd_msg_dec_rate_t(const cmd_vin_cmd_msg_dec_rate_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN CE layout setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_ce_setup_t(const cmd_vin_ce_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN HDR setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_hdr_setup_t(const cmd_vin_hdr_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that sets frame level flip/rotation control
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_set_frm_lvl_flip_rot_control_t(const cmd_vin_set_frm_lvl_flip_rot_control_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that sets FOV layout
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_set_fov_layout_t(const cmd_vin_set_fov_layout_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that sets VProc delay time
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_set_frm_vproc_delay_t(const cmd_vin_set_frm_vproc_delay_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that attaches sideband info to a capture frame
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_attach_sideband_info_to_cap_frm_t(const cmd_vin_attach_sideband_info_to_cap_frm_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump VIN command data that setup vin-vout lock
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_vout_lock_setup_t(const cmd_vin_vout_lock_setup_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump VIN attach metadata command
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_attach_metadata_t(const cmd_vin_attach_metadata_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump VIN temporal demux setup command
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vin_temporal_demux_setup_t(const cmd_vin_temporal_demux_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump Vout mixer setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_mixer_setup_t(const cmd_vout_mixer_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump Vout video setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_video_setup_t(const cmd_vout_video_setup_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump Vout default image setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_default_img_setup_t(const cmd_vout_default_img_setup_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump Vout OSD setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_osd_setup_t(const cmd_vout_osd_setup_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump Vout OSD buffer setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_osd_buf_setup_t(const cmd_vout_osd_buf_setup_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump Vout OSD CLUT setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_osd_clut_setup_t(const cmd_vout_osd_clut_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump Vout display setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_display_setup_t(const cmd_vout_display_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump Vout reset setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_reset_t(const cmd_vout_reset_t *pCmdData, const CmdInfo_t *Info);
#if 0
/**
 * Dump Vout display CSC(Color Space Convert) setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_display_csc_setup_t(const cmd_vout_display_csc_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump Vout digital output mode setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_digital_output_mode_setup_t(const cmd_vout_digital_output_mode_setup_t *pCmdData, const CmdInfo_t *Info);
#endif
/**
 * Dump Vout gamma setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_vout_gamma_setup_t(const cmd_vout_gamma_setup_t *pCmdData, const CmdInfo_t *Info);


/**
 * Dump encoder setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_encoder_setup_t(const cmd_encoder_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump encoder start command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_encoder_start_t(const cmd_encoder_start_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump encoder stop command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_encoder_stop_t(const cmd_encoder_stop_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump encoder realtime setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_encoder_realtime_setup_t(const encoder_realtime_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_setup_t(const cmd_decoder_setup_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder start command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_start_t(const cmd_decoder_start_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder stop command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_stop_t(const cmd_decoder_stop_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder command data that updates bits FIFO
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_bitsfifo_update_t(const cmd_decoder_bitsfifo_update_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder speed setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_speed_t(const cmd_decoder_speed_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump decoder trickplay setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_trickplay_t(const cmd_decoder_trickplay_t *pCmdData, const CmdInfo_t *Info);

#ifdef SUPPORT_DSP_DEC_BATCH
/**
 * Dump decoder batch setup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_batch_t(const cmd_decoder_batch_t *pCmdData, const CmdInfo_t *Info);
#endif

/**
 * Dump decoder still decode steup command data
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_decoder_stilldec_t(const cmd_decoder_stilldec_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump IDSP command data that sets Vin configuration
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_set_vin_config_t(const set_vin_config_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump IDSP command data that sets Vin compression
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
//extern void dump_cmd_set_vin_compression_t(const lossy_compression_t *pCmdData, const CmdInfo_t *Info);

/**
 * Dump IDSP command data that sets HDR compression
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
void dump_cmd_set_hdr_compression_t(const lossy_compression_t *pCmdData, const CmdInfo_t *Info);

#ifdef SUPPORT_DSP_SET_IDSP_DBG_CMD
/**
 * Dump IDSP command data that sends IDSP debug command
 * @param [in] pCmdData The command to be dumped
 * @param [in] Info Command info
 */
extern void dump_cmd_send_idsp_debug_cmd_t(const send_idsp_debug_cmd_t *pCmdData, const CmdInfo_t *Info);
#endif


#endif  //AMBA_DUMP_DSP_CMD_H
