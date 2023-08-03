/**
*  @file AmbaDSP_CmdMsgDump.c
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
*  @details DSP command message dump.
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_CmdsSend.h"
#include "AmbaDSP_CmdMsgDump.h"
#include "AmbaDSP_MsgDispatcher.h"

#if defined (CONFIG_ENABLE_DSP_COMMAND_PARM_DBG) || defined (CONFIG_LINUX)
static inline void CmdMsgDump(const char* x, UINT32 y, UINT32 z, UINT32 w) {
    AmbaLL_Log((UINT32)AMBALLLOG_TYPE_WRAPPER, x, (UINT32)y, (UINT32)z, (UINT32)w);
    return;
}
#else
#define CmdMsgDump(...)
#define CmdWriteDump(...)
#endif

void dump_cmd_dsp_config_t(const cmd_dsp_config_t *pCmdData, const CmdInfo_t *Info)
{
    UINT32 i;

    CmdWriteDump("CMD_DSP_CONFIG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" dsp_prof_id                = %d  ", pCmdData->dsp_prof_id, 0U, 0U);
    CmdMsgDump(" max_daik_par_num           = %d  ", pCmdData->max_daik_par_num, 0U, 0U);
    CmdMsgDump(" orc_poll_intv              = %d  ", pCmdData->orc_poll_intv, 0U, 0U);
    CmdMsgDump(" max_dram_par_num           = %d  ", pCmdData->max_dram_par_num, 0U, 0U);
    CmdMsgDump(" max_smem_par_num           = %d  ", pCmdData->max_smem_par_num, 0U, 0U);
    CmdMsgDump(" max_sub_par_num            = %d  ", pCmdData->max_sub_par_num, 0U, 0U);
    CmdMsgDump(" max_sup_par_num            = %d  ", pCmdData->max_sup_par_num, 0U, 0U);
    CmdMsgDump(" max_fbp_num                = %d  ", pCmdData->max_fbp_num, 0U, 0U);
    CmdMsgDump(" max_fb_num                 = %d  ", pCmdData->max_fb_num, 0U, 0U);
    CmdMsgDump(" max_dbp_num                = %d  ", pCmdData->max_dbp_num, 0U, 0U);
    CmdMsgDump(" max_cbuf_num               = %d  ", pCmdData->max_cbuf_num, 0U, 0U);
    CmdMsgDump(" max_bdt_num                = %d  ", pCmdData->max_bdt_num, 0U, 0U);
    CmdMsgDump(" max_bd_num                 = %d  ", pCmdData->max_bd_num, 0U, 0U);
    CmdMsgDump(" max_imginf_num             = %d  ", pCmdData->max_imginf_num, 0U, 0U);
    CmdMsgDump(" max_ext_fb_num             = %d  ", pCmdData->max_ext_fb_num, 0U, 0U);
    CmdMsgDump(" max_mcbl_num               = %d  ", pCmdData->max_mcbl_num, 0U, 0U);
    CmdMsgDump(" max_mcb_num                = %d  ", pCmdData->max_mcb_num, 0U, 0U);
    CmdMsgDump(" mbuf_par_size              = %d  ", pCmdData->mbuf_par_size, 0U, 0U);
    CmdMsgDump(" mbuf_size                  = %d  ", pCmdData->mbuf_size, 0U, 0U);
    CmdMsgDump(" max_orccode_msg_qs         = %d  ", pCmdData->max_orccode_msg_qs, 0U, 0U);
    CmdMsgDump(" max_orc_all_msg_qs         = %d  ", pCmdData->max_orc_all_msg_qs, 0U, 0U);
    CmdMsgDump(" num_of_vproc_channel       = %d  ", pCmdData->num_of_vproc_channel, 0U, 0U);
    CmdMsgDump(" vin_bit_mask               = 0x%X", pCmdData->vin_bit_mask, 0U, 0U);
    CmdMsgDump(" vin0_output_pins           = %d  ", pCmdData->vin0_output_pins, 0U, 0U);
    CmdMsgDump(" vin1_output_pins           = %d  ", pCmdData->vin1_output_pins, 0U, 0U);
    CmdMsgDump(" vin2_output_pins           = %d  ", pCmdData->vin2_output_pins, 0U, 0U);
    CmdMsgDump(" vin3_output_pins           = %d  ", pCmdData->vin3_output_pins, 0U, 0U);
    CmdMsgDump(" vin4_output_pins           = %d  ", pCmdData->vin4_output_pins, 0U, 0U);
    CmdMsgDump(" vin5_output_pins           = %d  ", pCmdData->vin5_output_pins, 0U, 0U);
    CmdMsgDump(" vout_bit_mask              = 0x%X", pCmdData->vout_bit_mask, 0U, 0U);
    CmdMsgDump(" page_size_k_log2           = %d  ", pCmdData->page_size_k_log2, 0U, 0U);
    CmdMsgDump(" max_mfbp_num               = %d  ", pCmdData->max_mfbp_num, 0U, 0U);
    CmdMsgDump(" max_mfb_num                = %d  ", pCmdData->max_mfb_num, 0U, 0U);
    CmdMsgDump(" max_ext_mfb_num            = %d  ", pCmdData->max_ext_mfb_num, 0U, 0U);
    CmdMsgDump(" max_mimginf_num            = %d  ", pCmdData->max_mimginf_num, 0U, 0U);
    CmdMsgDump(" num_of_enc_channel         = %d  ", pCmdData->num_of_enc_channel, 0U, 0U);
    CmdMsgDump(" tot_idsp_ld_str_num[0]     = %d  ", pCmdData->tot_idsp_ld_str_num[0], 0U, 0U);
    CmdMsgDump(" tot_idsp_ld_str_num[1]     = %d  ", pCmdData->tot_idsp_ld_str_num[1], 0U, 0U);
    CmdMsgDump(" tot_idsp_ld_str_num[2]     = %d  ", pCmdData->tot_idsp_ld_str_num[2], 0U, 0U);
    CmdMsgDump(" use_fbp_smem_cache         = %d  ", pCmdData->use_fbp_smem_cache, 0U, 0U);
    CmdMsgDump(" use_fbp_dram_cache         = %d  ", pCmdData->use_fbp_dram_cache, 0U, 0U);
    CmdMsgDump(" use_fb_smem_cache          = %d  ", pCmdData->use_fb_smem_cache, 0U, 0U);
    CmdMsgDump(" use_fb_dram_cache          = %d  ", pCmdData->use_fb_dram_cache, 0U, 0U);
    CmdMsgDump(" use_imginf_smem_cache      = %d  ", pCmdData->use_imginf_smem_cache, 0U, 0U);
    CmdMsgDump(" use_imginf_dram_cache      = %d  ", pCmdData->use_imginf_dram_cache, 0U, 0U);
    CmdMsgDump(" use_mfbp_smem_cache        = %d  ", pCmdData->use_mfbp_smem_cache, 0U, 0U);
    CmdMsgDump(" use_mfbp_dram_cache        = %d  ", pCmdData->use_mfbp_dram_cache, 0U, 0U);
    CmdMsgDump(" use_mfb_smem_cache         = %d  ", pCmdData->use_mfb_smem_cache, 0U, 0U);
    CmdMsgDump(" use_mfb_dram_cache         = %d  ", pCmdData->use_mfb_dram_cache, 0U, 0U);
    CmdMsgDump(" use_mimginf_smem_cache     = %d  ", pCmdData->use_mimginf_smem_cache, 0U, 0U);
    CmdMsgDump(" use_mimginf_dram_cache     = %d  ", pCmdData->use_mimginf_dram_cache, 0U, 0U);
    CmdMsgDump(" is_effect_on               = %d  ", pCmdData->is_effect_on, 0U, 0U);
    CmdMsgDump(" is_partial_load_en         = %d  ", pCmdData->is_partial_load_en, 0U, 0U);
    CmdMsgDump(" dpri3_max_outstanding_xfers_m1= %d  ", pCmdData->dpri3_max_outstanding_xfers_m1, 0U, 0U);
    CmdMsgDump(" num_of_dec_channel         = %d  ", pCmdData->num_of_dec_channel, 0U, 0U);
    CmdMsgDump(" vdec_capture_ena           = %d  ", pCmdData->vdec_capture_ena, 0U, 0U);
    CmdMsgDump(" dec_codec_support          = %d  ", pCmdData->dec_codec_support, 0U, 0U);
    CmdMsgDump(" max_fov_num_per_vin        = %d  ", pCmdData->max_fov_num_per_vin, 0U, 0U);
    CmdMsgDump(" num_of_virtual_vins        = %d  ", pCmdData->num_of_virtual_vins, 0U, 0U);
    for (i = 0; i < pCmdData->num_of_virtual_vins; i++) {
        CmdMsgDump(" virtual_vin_output_pins[%d] = %d", i, pCmdData->virtual_vin_output_pins[i], 0U);
    }
    CmdMsgDump(" vouta_max_pic_ncols        = %d  ", pCmdData->vouta_max_pic_ncols, 0U, 0U);
    CmdMsgDump(" vouta_max_osd_ncols        = %d  ", pCmdData->vouta_max_osd_ncols, 0U, 0U);
    CmdMsgDump(" voutb_max_pic_ncols        = %d  ", pCmdData->voutb_max_pic_ncols, 0U, 0U);
    CmdMsgDump(" voutb_max_osd_ncols        = %d  ", pCmdData->voutb_max_osd_ncols, 0U, 0U);
    CmdMsgDump(" vouta_max_pic_nrows        = %d  ", pCmdData->vouta_max_pic_nrows, 0U, 0U);
    CmdMsgDump(" vouta_max_osd_nrows        = %d  ", pCmdData->vouta_max_osd_nrows, 0U, 0U);
    CmdMsgDump(" voutb_max_pic_nrows        = %d  ", pCmdData->voutb_max_pic_nrows, 0U, 0U);
    CmdMsgDump(" voutb_max_osd_nrows        = %d  ", pCmdData->voutb_max_osd_nrows, 0U, 0U);
    CmdMsgDump(" vout_mixer_ncols           = %d  ", pCmdData->vout_mixer_ncols, 0U, 0U);
    CmdMsgDump(" vout_mixer_nrows           = %d  ", pCmdData->vout_mixer_nrows, 0U, 0U);
    CmdMsgDump(" vproc_system_dram_size     = %d  ", pCmdData->vproc_system_dram_size, 0U, 0U);
    CmdMsgDump(" max_eng0_width             = %d  ", pCmdData->max_eng0_width, 0U, 0U);
    CmdMsgDump(" vproc_ext_mem_init_mode    = %d  ", pCmdData->vproc_ext_mem_init_mode, 0U, 0U);
    CmdMsgDump(" is_sproc_hi_enabled        = %d  ", pCmdData->is_sproc_hi_enabled, 0U, 0U);
#ifdef SUPPORT_VPROC_OSD_INSERT
    CmdMsgDump(" delayline_ena              = %d  ", pCmdData->delayline_ena, 0U, 0U);
#endif
    CmdMsgDump(" max_dbuf_num_per_vin       = %d  ", pCmdData->max_dbuf_num_per_vin, 0U, 0U);
}

void dump_cmd_dsp_hal_inf_t(const cmd_dsp_hal_inf_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_HAL_INF", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" audi_clk_freq = %d", pCmdData->audi_clk_freq, 0U, 0U);
    CmdMsgDump(" core_clk_freq = %d", pCmdData->core_clk_freq, 0U, 0U);
    CmdMsgDump(" dram_clk_freq = %d", pCmdData->dram_clk_freq, 0U, 0U);
    CmdMsgDump(" idsp_clk_freq = %d", pCmdData->idsp_clk_freq, 0U, 0U);
}

void dump_cmd_dsp_suspend_profile_t(const cmd_dsp_suspend_profile_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_SUSPEND_PROFILE", pCmdData->cmd_code, Info->WriteMode);
}

void dump_cmd_set_debug_level_t(const cmd_set_debug_level_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_SET_DEBUG_LEVEL", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" module   = 0x%X", pCmdData->module, 0U, 0U);
    CmdMsgDump(" level    = %d", pCmdData->level, 0U, 0U);
}

void dump_cmd_print_th_disable_mask_t(const cmd_print_th_disable_mask_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_PRINT_THREAD_DISABLE_MASK", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" orccode_mask_valid = 0x%X", pCmdData->orccode_mask_valid, 0U, 0U); // If set, orccode_mask will take effect.
#if 0
    CmdMsgDump(" orcme_mask_valid   = 0x%X", pCmdData->orcme_mask_valid, 0U, 0U);   // If set, orcme_mask will take effect.
    CmdMsgDump(" orcmdxf_mask_valid = 0x%X", pCmdData->orcmdxf_mask_valid, 0U, 0U); // If set, orcmdxf_mask will take effect.
#endif
    CmdMsgDump(" orccode_mask       = 0x%X", pCmdData->orccode_mask, 0U, 0U);       // Map to orccode thread 0~11.
#if 0
    CmdMsgDump(" orcmdxf_mask       = 0x%X", pCmdData->orcmdxf_mask, 0U, 0U);       // Map to orcme thread 0~1.
    CmdMsgDump(" orcme_mask         = 0x%X", pCmdData->orcme_mask, 0U, 0U);         // Map to orcmdxf thread 0~1.
#endif
}

void dump_cmd_binding_cfg_t(const cmd_binding_cfg_t *pCmdData, const CmdInfo_t *Info)
{
    UINT32 i;
    CmdWriteDump("CMD_DSP_BINDING_CFG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" num_of_bindings = %d", pCmdData->num_of_bindings, 0U, 0U);
    for (i = 0; i < pCmdData->num_of_bindings; i++) {
        CmdMsgDump(" ===== binding: %u =====", i, 0U, 0U);
        CmdMsgDump(" bind_type   = %d", pCmdData->bindings[i].bind_type, 0U, 0U);
        CmdMsgDump(" bind_msg    = %d", pCmdData->bindings[i].bind_msg, 0U, 0U);
        CmdMsgDump(" delay       = %d", pCmdData->bindings[i].delay, 0U, 0U);
        CmdMsgDump(" src_fp_type = %d", pCmdData->bindings[i].src_fp_type, 0U, 0U);
        CmdMsgDump(" src_fp_id   = %d", pCmdData->bindings[i].src_fp_id, 0U, 0U);
        CmdMsgDump(" src_ch_id   = %d", pCmdData->bindings[i].src_ch_id, 0U, 0U);
        CmdMsgDump(" src_str_id  = %d", pCmdData->bindings[i].src_str_id, 0U, 0U);
        CmdMsgDump(" dst_fp_type = %d", pCmdData->bindings[i].dst_fp_type, 0U, 0U);
        CmdMsgDump(" dst_fp_id   = %d", pCmdData->bindings[i].dst_fp_id, 0U, 0U);
        CmdMsgDump(" dst_ch_id   = %d", pCmdData->bindings[i].dst_ch_id, 0U, 0U);
        CmdMsgDump(" dst_str_id  = %d", pCmdData->bindings[i].dst_str_id, 0U, 0U);
    }
}

void dump_cmd_dsp_activate_profile_t(const cmd_dsp_activate_profile_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_ACTIVATE_PROFILE", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" dsp_prof_id    = 0x%X", pCmdData->dsp_prof_id, 0U, 0U);
}

void dump_cmd_dsp_vproc_flow_max_cfg_t(const cmd_dsp_vproc_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_VPROC_FLOW_MAX_CFG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" input_format                   = %d  ", pCmdData->input_format, 0U, 0U);
    CmdMsgDump(" is_raw_compressed              = %d  ", pCmdData->is_raw_compressed, 0U, 0U);
    CmdMsgDump(" is_tile_mode                   = %d  ", pCmdData->is_tile_mode, 0U, 0U);
    CmdMsgDump(" is_li_enabled                  = %d  ", pCmdData->is_li_enabled, 0U, 0U);
    CmdMsgDump(" is_warp_enabled                = %d  ", pCmdData->is_warp_enabled, 0U, 0U);
    CmdMsgDump(" is_mctf_enabled                = %d  ", pCmdData->is_mctf_enabled, 0U, 0U);
    CmdMsgDump(" is_hdr_enabled                 = %d  ", pCmdData->is_hdr_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_ln_enabled             = %d  ", pCmdData->is_prev_ln_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_a_enabled              = %d  ", pCmdData->is_prev_a_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_b_enabled              = %d  ", pCmdData->is_prev_b_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_c_enabled              = %d  ", pCmdData->is_prev_c_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_hier_enabled           = %d  ", pCmdData->is_prev_hier_enabled, 0U, 0U);
    CmdMsgDump(" is_compressed_out_enabled      = %d  ", pCmdData->is_compressed_out_enabled, 0U, 0U);
    CmdMsgDump(" is_c2y_burst_tiling_out        = %d  ", pCmdData->is_c2y_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" is_hier_burst_tiling_out       = %d  ", pCmdData->is_hier_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" is_mctf_ref_win_mode           = %d  ", pCmdData->is_mctf_ref_win_mode, 0U, 0U);
    CmdMsgDump(" num_exp_max                    = %d  ", pCmdData->num_exp_max, 0U, 0U);
    CmdMsgDump(" is_dzoom_enabled               = %d  ", pCmdData->is_dzoom_enabled, 0U, 0U);
    CmdMsgDump(" num_of_vproc_groups            = %d  ", pCmdData->num_of_vproc_groups, 0U, 0U);
    CmdMsgDump(" raw_comp_blk_sz_wst            = %d  ", pCmdData->raw_comp_blk_sz_wst, 0U, 0U);
    CmdMsgDump(" raw_comp_mantissa_wst          = %d  ", pCmdData->raw_comp_mantissa_wst, 0U, 0U);
    CmdMsgDump(" prev_com0_ch_fmt               = %d  ", pCmdData->prev_com0_ch_fmt, 0U, 0U);
    CmdMsgDump(" prev_com1_ch_fmt               = %d  ", pCmdData->prev_com1_ch_fmt, 0U, 0U);
    CmdMsgDump(" is_mipi_yuyv_enabled           = %d  ", pCmdData->is_mipi_yuyv_enabled, 0U, 0U);
    CmdMsgDump(" max_ch_effect_copy_num         = %d  ", pCmdData->max_ch_effect_copy_num, 0U, 0U);
    CmdMsgDump(" prev_a_roi_tile_en             = %d  ", pCmdData->prev_a_roi_tile_en, 0U, 0U);
    CmdMsgDump(" prev_b_roi_tile_en             = %d  ", pCmdData->prev_b_roi_tile_en, 0U, 0U);
    CmdMsgDump(" prev_c_roi_tile_en             = %d  ", pCmdData->prev_c_roi_tile_en, 0U, 0U);
    CmdMsgDump(" max_ch_c2y_tile_num            = %d  ", pCmdData->max_ch_c2y_tile_num, 0U, 0U);
    CmdMsgDump(" max_ch_tile_num                = %d  ", pCmdData->max_ch_tile_num, 0U, 0U);
    CmdMsgDump(" min_tile_num                   = %d  ", pCmdData->min_tile_num, 0U, 0U);
    CmdMsgDump(" max_tile_num                   = %d  ", pCmdData->max_tile_num, 0U, 0U);
    CmdMsgDump(" max_c2y_tile_y_num             = %d  ", pCmdData->max_c2y_tile_y_num, 0U, 0U);
    CmdMsgDump(" max_warp_tile_y_num            = %d  ", pCmdData->max_warp_tile_y_num, 0U, 0U);
    CmdMsgDump(" raw_width_max                  = %d  ", pCmdData->raw_width_max, 0U, 0U);
    CmdMsgDump(" raw_height_max                 = %d  ", pCmdData->raw_height_max, 0U, 0U);
    CmdMsgDump(" W0_max                         = %d  ", pCmdData->W0_max, 0U, 0U);
    CmdMsgDump(" H0_max                         = %d  ", pCmdData->H0_max, 0U, 0U);
    CmdMsgDump(" W_pre_warp_luma_max            = %d  ", pCmdData->W_pre_warp_luma_max, 0U, 0U);
    CmdMsgDump(" H_pre_warp_luma_max            = %d  ", pCmdData->H_pre_warp_luma_max, 0U, 0U);
    CmdMsgDump(" W_main_max                     = %d  ", pCmdData->W_main_max, 0U, 0U);
    CmdMsgDump(" H_main_max                     = %d  ", pCmdData->H_main_max, 0U, 0U);
    CmdMsgDump(" prev_ln_w_max                  = %d  ", pCmdData->prev_ln_w_max, 0U, 0U);
    CmdMsgDump(" prev_ln_h_max                  = %d  ", pCmdData->prev_ln_h_max, 0U, 0U);
    CmdMsgDump(" prev_hier_w_max                = %d  ", pCmdData->prev_hier_w_max, 0U, 0U);
    CmdMsgDump(" prev_hier_h_max                = %d  ", pCmdData->prev_hier_h_max, 0U, 0U);
    CmdMsgDump(" prev_a_w_max                   = %d  ", pCmdData->prev_a_w_max, 0U, 0U);
    CmdMsgDump(" prev_a_h_max                   = %d  ", pCmdData->prev_a_h_max, 0U, 0U);
    CmdMsgDump(" prev_b_w_max                   = %d  ", pCmdData->prev_b_w_max, 0U, 0U);
    CmdMsgDump(" prev_b_h_max                   = %d  ", pCmdData->prev_b_h_max, 0U, 0U);
    CmdMsgDump(" prev_c_w_max                   = %d  ", pCmdData->prev_c_w_max, 0U, 0U);
    CmdMsgDump(" prev_c_h_max                   = %d  ", pCmdData->prev_c_h_max, 0U, 0U);
    CmdMsgDump(" warp_a_y_in_blk_h_max          = %d  ", pCmdData->warp_a_y_in_blk_h_max, 0U, 0U);
    CmdMsgDump(" warp_a_uv_in_blk_h_max         = %d  ", pCmdData->warp_a_uv_in_blk_h_max, 0U, 0U);
    CmdMsgDump(" warp_tile_overlap_x_max        = %d  ", pCmdData->warp_tile_overlap_x_max, 0U, 0U);
    CmdMsgDump(" hier_poly_tile_overlap_x_max   = %d  ", pCmdData->hier_poly_tile_overlap_x_max, 0U, 0U);
    CmdMsgDump(" prev_com0_fb_num               = %d  ", pCmdData->prev_com0_fb_num, 0U, 0U);
    CmdMsgDump(" prev_com1_fb_num               = %d  ", pCmdData->prev_com1_fb_num, 0U, 0U);
    CmdMsgDump(" msg_ext_data_max_num           = %d  ", pCmdData->msg_ext_data_max_num, 0U, 0U);
    CmdMsgDump(" msg_ext_data_base_addr         = 0x%X", pCmdData->msg_ext_data_base_addr, 0U, 0U);
    CmdMsgDump(" postp_main_fb_num              = %d  ", pCmdData->postp_main_fb_num, 0U, 0U);
    CmdMsgDump(" postp_main_me01_fb_num         = %d  ", pCmdData->postp_main_me01_fb_num, 0U, 0U);
    CmdMsgDump(" postp_pip_fb_num               = %d  ", pCmdData->postp_pip_fb_num, 0U, 0U);
    CmdMsgDump(" postp_pip_me01_fb_num          = %d  ", pCmdData->postp_pip_me01_fb_num, 0U, 0U);
    CmdMsgDump(" prev_com0_w_max                = %d  ", pCmdData->prev_com0_w_max, 0U, 0U);
    CmdMsgDump(" prev_com0_h_max                = %d  ", pCmdData->prev_com0_h_max, 0U, 0U);
    CmdMsgDump(" prev_com1_w_max                = %d  ", pCmdData->prev_com1_w_max, 0U, 0U);
    CmdMsgDump(" prev_com1_h_max                = %d  ", pCmdData->prev_com1_h_max, 0U, 0U);
    CmdMsgDump(" postp_main_w_max               = %d  ", pCmdData->postp_main_w_max, 0U, 0U);
    CmdMsgDump(" postp_main_h_max               = %d  ", pCmdData->postp_main_h_max, 0U, 0U);
    CmdMsgDump(" postp_pip_w_max                = %d  ", pCmdData->postp_pip_w_max, 0U, 0U);
    CmdMsgDump(" postp_pip_h_max                = %d  ", pCmdData->postp_pip_h_max, 0U, 0U);
    CmdMsgDump(" is_c2y_line_sync_to_warp       = %d  ", pCmdData->is_c2y_line_sync_to_warp, 0U, 0U);
    CmdMsgDump(" is_c2y_smem_sync_to_warp       = %d  ", pCmdData->is_c2y_smem_sync_to_warp, 0U, 0U);
    CmdMsgDump(" vwarp_chroma_scale_max         = %d  ", pCmdData->vwarp_chroma_scale_max, 0U, 0U);
    CmdMsgDump(" warp_wait_lines_max            = %d  ", pCmdData->warp_wait_lines_max, 0U, 0U);
    CmdMsgDump(" is_osd_mixer_enabled           = %d  ", pCmdData->is_osd_mixer_enabled, 0U, 0U);
    CmdMsgDump(" osd_mixer_w_max                = %d  ", pCmdData->osd_mixer_w_max, 0U, 0U);
    CmdMsgDump(" max_ch_osd_area_num            = %d  ", pCmdData->max_ch_osd_area_num, 0U, 0U);
    CmdMsgDump(" sproc_hi_W_max                 = %d  ", pCmdData->sproc_hi_W_max, 0U, 0U);
    CmdMsgDump(" sproc_hi_H_max                 = %d  ", pCmdData->sproc_hi_H_max, 0U, 0U);
    CmdMsgDump(" is_yuyv2y_burst_tiling_out     = %d  ", pCmdData->is_yuyv2y_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" init_prev_com0_fb              = %d  ", pCmdData->init_prev_com0_fb, 0U, 0U);
    CmdMsgDump(" init_prev_com1_fb              = %d  ", pCmdData->init_prev_com1_fb, 0U, 0U);
}

void dump_cmd_dsp_vin_flow_max_cfg_t(const cmd_dsp_vin_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info)
{
    UINT32 Idx;

    CmdWriteDump("CMD_DSP_VIN_FLOW_MAX_CFG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                              = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" is_sensor_raw_out_enabled           = %d  ", pCmdData->is_sensor_raw_out_enabled, 0U, 0U);
    CmdMsgDump(" is_contrast_enhance_out_enabled     = %d  ", pCmdData->is_contrast_enhance_out_enabled, 0U, 0U);
    CmdMsgDump(" is_yuv422_out_enabled               = %d  ", pCmdData->is_yuv422_out_enabled, 0U, 0U);
    CmdMsgDump(" raw_width_is_byte_width             = %d  ", pCmdData->raw_width_is_byte_width, 0U, 0U);
    CmdMsgDump(" vin_smem_to_vproc                   = %d  ", pCmdData->vin_smem_to_vproc, 0U, 0U);
    CmdMsgDump(" vin_raw_smem_win_out                = %d  ", pCmdData->vin_raw_smem_win_out, 0U, 0U);
    CmdMsgDump(" ce_out_format                       = %d  ", pCmdData->ce_out_format, 0U, 0U);
    CmdMsgDump(" max_raw_cap_dbuf_num                = %d  ", pCmdData->max_raw_cap_dbuf_num, 0U, 0U);
    CmdMsgDump(" max_raw_cap_width                   = %d  ", pCmdData->max_raw_cap_width, 0U, 0U);
    CmdMsgDump(" max_raw_cap_height                  = %d  ", pCmdData->max_raw_cap_height, 0U, 0U);
    CmdMsgDump(" max_contrast_enhance_out_dbuf_num   = %d  ", pCmdData->max_contrast_enhance_out_dbuf_num, 0U, 0U);
    CmdMsgDump(" max_contrast_enhance_out_width      = %d  ", pCmdData->max_contrast_enhance_out_width, 0U, 0U);
    CmdMsgDump(" max_contrast_enhance_out_height     = %d  ", pCmdData->max_contrast_enhance_out_height, 0U, 0U);
    CmdMsgDump(" max_yuv422_out_dbuf_num             = %d  ", pCmdData->max_yuv422_out_dbuf_num, 0U, 0U);
    CmdMsgDump(" max_yuv422_out_width                = %d  ", pCmdData->max_yuv422_out_width, 0U, 0U);
    CmdMsgDump(" max_yuv422_out_height               = %d  ", pCmdData->max_yuv422_out_height, 0U, 0U);
    CmdMsgDump(" max_vin_raw_smem_win_height         = %d  ", pCmdData->max_vin_raw_smem_win_height, 0U, 0U);
    CmdMsgDump(" max_fov_num                         = %d  ", pCmdData->max_fov_num, 0U, 0U);
    CmdMsgDump(" ext_mem_cfg_num                     = %d  ", pCmdData->ext_mem_cfg_num, 0U, 0U);
    CmdMsgDump(" hdr_num_exposures_minus_1           = %d  ", pCmdData->hdr_num_exposures_minus_1, 0U, 0U);
    CmdMsgDump(" vin_yuv_enc_fbp_init                = %d  ", pCmdData->vin_yuv_enc_fbp_init, 0U, 0U);
    CmdMsgDump(" is_raw_cap_dbuf_disabled            = %d  ", pCmdData->is_raw_cap_dbuf_disabled, 0U, 0U);
    CmdMsgDump(" is_ce_out_dbuf_disabled             = %d  ", pCmdData->is_ce_out_dbuf_disabled, 0U, 0U);
    CmdMsgDump(" is_yuv422_out_dbuf_disabled         = %d  ", pCmdData->is_yuv422_out_dbuf_disabled, 0U, 0U);

    for (Idx = 0; Idx < pCmdData->ext_mem_cfg_num; Idx++) {
        CmdMsgDump(" ========= ext_mem_cfg [%u] ======== ", Idx, 0U, 0U);
        CmdMsgDump("   enable                   = %d  ", pCmdData->ext_mem_cfg[Idx].enable, 0U, 0U);
        CmdMsgDump("   memory_type              = %d  ", pCmdData->ext_mem_cfg[Idx].memory_type, 0U, 0U);
        CmdMsgDump("   max_daddr_slot           = %d  ", pCmdData->ext_mem_cfg[Idx].max_daddr_slot, 0U, 0U);
        CmdMsgDump("   pool_buf_num             = %d  ", pCmdData->ext_mem_cfg[Idx].pool_buf_num, 0U, 0U);
    }
}

void dump_cmd_dsp_set_profile_t(const cmd_dsp_set_profile_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_SET_PROFILE", pCmdData->cmd_code, Info->WriteMode);
}

void dump_cmd_dsp_enc_flow_max_cfg_t(const cmd_dsp_enc_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info, const UINT16 MaxStrmNum)
{
    UINT16 i;

    CmdWriteDump("CMD_DSP_ENC_FLOW_MAX_CFG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" reconfig_enc_type      = %d  ", pCmdData->reconfig_enc_type, 0U, 0U);
    CmdMsgDump(" max_enc_types          = %d  ", pCmdData->max_enc_types, 0U, 0U);
    CmdMsgDump(" separate_ref_smem      = %d  ", pCmdData->separate_ref_smem, 0U, 0U);
    CmdMsgDump(" smem_encode            = %d  ", pCmdData->smem_encode, 0U, 0U);
    CmdMsgDump(" reset_info_fifo_offset_when_all_stream_idle = %d  ", pCmdData->reset_info_fifo_offset_when_all_stream_idle, 0U, 0U);
    CmdMsgDump(" is_10bit               = %d  ", pCmdData->is_10bit, 0U, 0U);
    CmdMsgDump(" slice_memd             = %d  ", pCmdData->slice_memd, 0U, 0U);
    CmdMsgDump(" cout_dwidth_mul        = %d  ", pCmdData->cout_dwidth_mul, 0U, 0U);

    for (i = 0U; i < MaxStrmNum; i++) {
        CmdMsgDump(" StrmAttr[%d]                 ", i, 0U, 0U);
        CmdMsgDump("   enc_max_resol        = %d  ", pCmdData->enc_max_resol[i], 0U, 0U);
        CmdMsgDump("   max_ref_num          = %d  ", pCmdData->max_ref_num[i], 0U, 0U);
        CmdMsgDump("   max_smvmax           = %d  ", pCmdData->max_smvmax[i], 0U, 0U);
        CmdMsgDump("   is_mctf_share_ref    = %d  ", pCmdData->is_mctf_share_ref[i], 0U, 0U);
        CmdMsgDump("   max_rec_fb_num       = %d  ", pCmdData->max_rec_fb_num[i], 0U, 0U);
    }
    CmdMsgDump(" eng0_msg_queue_size[0] = %d  ", pCmdData->eng0_msg_queue_size[0], 0U, 0U);
    CmdMsgDump(" eng0_msg_queue_size[1] = %d  ", pCmdData->eng0_msg_queue_size[1], 0U, 0U);
    CmdMsgDump(" pjpeg_dram_size        = %d  ", pCmdData->pjpeg_dram_size, 0U, 0U);
    CmdMsgDump(" avc_pjpeg_dram_size    = %d  ", pCmdData->avc_pjpeg_dram_size, 0U, 0U);
    CmdMsgDump(" enc_cfg_daddr          = 0x%X", pCmdData->enc_cfg_daddr, 0U, 0U);

    if (pCmdData->enc_cfg_daddr != 0U) {
        const DSP_ENC_CFG *pStrmCfg;
        ULONG ULAddr;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->enc_cfg_daddr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pStrmCfg, &ULAddr);
            for (i = 0U; i < MaxStrmNum; i++) {
                CmdMsgDump(" DSP_ENC_CFG[%d]", i, 0U, 0U);
                CmdMsgDump("   max_enc_width                  = %d  ", pStrmCfg[i].max_enc_width, 0U, 0U);
                CmdMsgDump("   max_enc_height                 = %d  ", pStrmCfg[i].max_enc_height, 0U, 0U);
                CmdMsgDump("   gop_simple_cfg                 = %d  ", pStrmCfg[i].gop_simple_cfg, 0U, 0U);
                CmdMsgDump("   gop_hier_P_cfg                 = %d  ", pStrmCfg[i].gop_hier_P_cfg, 0U, 0U);
                CmdMsgDump("   gop_fast_seek_one_ref_cfg      = %d  ", pStrmCfg[i].gop_fast_seek_one_ref_cfg, 0U, 0U);
                CmdMsgDump("   gop_fast_seek_two_ref_cfg      = %d  ", pStrmCfg[i].gop_fast_seek_two_ref_cfg, 0U, 0U);
                CmdMsgDump("   codec_disable_flag             = %d  ", pStrmCfg[i].codec_disable_flag, 0U, 0U);
                CmdMsgDump("   gop_hier_B_two_ref_cfg         = %d  ", pStrmCfg[i].gop_hier_B_two_ref_cfg, 0U, 0U);
                CmdMsgDump("   add_recon_buf_num              = %d  ", pStrmCfg[i].add_recon_buf_num, 0U, 0U);
                CmdMsgDump("   extra_height                   = %d  ", pStrmCfg[i].extra_height, 0U, 0U);
                CmdMsgDump("   data_q_out_num                 = %d  ", pStrmCfg[i].data_q_out_num, 0U, 0U);
                CmdMsgDump("   fast_boot                      = %d  ", pStrmCfg[i].fast_boot, 0U, 0U);
            }
        }
    }
}

void dump_cmd_dsp_dec_flow_max_cfg_t(const cmd_dsp_dec_flow_max_cfg_t *pCmdData, const CmdInfo_t *Info)
{
    UINT8 i = (UINT8)0U;

    CmdWriteDump("DSP_DEC_FLOW_MAX_CFG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("   max_bit_rate         = %d  ", pCmdData->dec_cfg.max_bit_rate, 0U, 0U);
    CmdMsgDump("   max_n_to_m_ratio     = %d  ", pCmdData->dec_cfg.max_n_to_m_ratio, 0U, 0U);
    CmdMsgDump("   max_frm_num_of_dpb   = %d  ", pCmdData->dec_cfg.max_frm_num_of_dpb, 0U, 0U);
    CmdMsgDump("   max_frm_width        = %d  ", pCmdData->dec_cfg.max_frm_width, 0U, 0U);
    CmdMsgDump("   max_frm_height       = %d  ", pCmdData->dec_cfg.max_frm_height, 0U, 0U);
    CmdMsgDump("   ref_smem_size        = %d  ", pCmdData->dec_cfg.ref_smem_size, 0U, 0U);
    CmdMsgDump(" DprocFlowMaxCfg", 0, 0U, 0U);

    for (i = 0U; i < DPROC_MAX_OUT_STREAMS; i++) {
        CmdMsgDump("   Cfg[%d]", i, 0U, 0U);
        CmdMsgDump("     yuv_ena      = %d", pCmdData->dproc_cfg.out_strm[i].yuv_ena, 0U, 0U);
        CmdMsgDump("     me1_ena      = %d", pCmdData->dproc_cfg.out_strm[i].me1_ena, 0U, 0U);
        CmdMsgDump("     fbuf_num     = %d", pCmdData->dproc_cfg.out_strm[i].fbuf_num, 0U, 0U);
        CmdMsgDump("     fbuf_width   = %d", pCmdData->dproc_cfg.out_strm[i].fbuf_width, 0U, 0U);
        CmdMsgDump("     fbuf_height  = %d", pCmdData->dproc_cfg.out_strm[i].fbuf_height, 0U, 0U);
    }

    CmdMsgDump(" PostpFlowMaxCfg", 0, 0U, 0U);
    CmdMsgDump("   alloc_vout0_fbuf     = %d  ", pCmdData->postp_cfg.alloc_vout0_fbuf, 0U, 0U);
    CmdMsgDump("   vout0_fbuf_width     = %d  ", pCmdData->postp_cfg.vout0_fbuf_width, 0U, 0U);
    CmdMsgDump("   vout0_fbuf_height    = %d  ", pCmdData->postp_cfg.vout0_fbuf_height, 0U, 0U);
    CmdMsgDump("   vout0_fbuf_num       = %d  ", pCmdData->postp_cfg.vout0_fbuf_num, 0U, 0U);
}

void dump_cmd_dsp_nop_t(const dsp_cmd_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DSP_NOP", pCmdData->cmd_code, Info->WriteMode);
}

void dump_cmd_vproc_cfg_t(const cmd_vproc_cfg_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_CONFIG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id                         = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" input_format                       = %d  ", pCmdData->input_format, 0U, 0U);
    CmdMsgDump(" prev_mode                          = %d  ", pCmdData->prev_mode, 0U, 0U);
    CmdMsgDump(" num_of_exp                         = %d  ", pCmdData->num_of_exp, 0U, 0U);
    CmdMsgDump(" grp_id                             = %d  ", pCmdData->grp_id, 0U, 0U);
    CmdMsgDump(" is_li_enabled                      = %d  ", pCmdData->is_li_enabled, 0U, 0U);
    CmdMsgDump(" is_hdr_enabled                     = %d  ", pCmdData->is_hdr_enabled, 0U, 0U);
    CmdMsgDump(" is_warp_enabled                    = %d  ", pCmdData->is_warp_enabled, 0U, 0U);
    CmdMsgDump(" prev_a_roi_tile_en                 = %d  ", pCmdData->prev_a_roi_tile_en, 0U, 0U);
    CmdMsgDump(" prev_b_roi_tile_en                 = %d  ", pCmdData->prev_b_roi_tile_en, 0U, 0U);
    CmdMsgDump(" prev_c_roi_tile_en                 = %d  ", pCmdData->prev_c_roi_tile_en, 0U, 0U);
    CmdMsgDump(" is_osd_mixer_enabled               = %d  ", pCmdData->is_osd_mixer_enabled, 0U, 0U);
    CmdMsgDump(" raw_width_max                      = %d  ", pCmdData->raw_width_max, 0U, 0U);
    CmdMsgDump(" raw_height_max                     = %d  ", pCmdData->raw_height_max, 0U, 0U);
    CmdMsgDump(" W0_max                             = %d  ", pCmdData->W0_max, 0U, 0U);
    CmdMsgDump(" H0_max                             = %d  ", pCmdData->H0_max, 0U, 0U);
    CmdMsgDump(" W_pre_warp_luma_max                = %d  ", pCmdData->W_pre_warp_luma_max, 0U, 0U);
    CmdMsgDump(" H_pre_warp_luma_max                = %d  ", pCmdData->H_pre_warp_luma_max, 0U, 0U);
    CmdMsgDump(" W_main_max                         = %d  ", pCmdData->W_main_max, 0U, 0U);
    CmdMsgDump(" H_main_max                         = %d  ", pCmdData->H_main_max, 0U, 0U);
    CmdMsgDump(" prev_ln_w_max                      = %d  ", pCmdData->prev_ln_w_max, 0U, 0U);
    CmdMsgDump(" prev_ln_h_max                      = %d  ", pCmdData->prev_ln_h_max, 0U, 0U);
    CmdMsgDump(" prev_hier_w_max                    = %d  ", pCmdData->prev_hier_w_max, 0U, 0U);
    CmdMsgDump(" prev_hier_h_max                    = %d  ", pCmdData->prev_hier_h_max, 0U, 0U);
    CmdMsgDump(" prev_a_w_max                       = %d  ", pCmdData->prev_a_w_max, 0U, 0U);
    CmdMsgDump(" prev_a_h_max                       = %d  ", pCmdData->prev_a_h_max, 0U, 0U);
    CmdMsgDump(" prev_b_w_max                       = %d  ", pCmdData->prev_b_w_max, 0U, 0U);
    CmdMsgDump(" prev_b_h_max                       = %d  ", pCmdData->prev_b_h_max, 0U, 0U);
    CmdMsgDump(" prev_c_w_max                       = %d  ", pCmdData->prev_c_w_max, 0U, 0U);
    CmdMsgDump(" prev_c_h_max                       = %d  ", pCmdData->prev_c_h_max, 0U, 0U);
    CmdMsgDump(" warp_a_y_in_blk_h_max              = %d  ", pCmdData->warp_a_y_in_blk_h_max, 0U, 0U);
    CmdMsgDump(" warp_a_uv_in_blk_h_max             = %d  ", pCmdData->warp_a_uv_in_blk_h_max, 0U, 0U);
    CmdMsgDump(" main_fb_num                        = %d  ", pCmdData->main_fb_num, 0U, 0U);
    CmdMsgDump(" main_me01_fb_num                   = %d  ", pCmdData->main_me01_fb_num, 0U, 0U);
    CmdMsgDump(" prev_a_fb_num                      = %d  ", pCmdData->prev_a_fb_num, 0U, 0U);
    CmdMsgDump(" prev_a_me_fb_num                   = %d  ", pCmdData->prev_a_me_fb_num, 0U, 0U);
    CmdMsgDump(" prev_b_fb_num                      = %d  ", pCmdData->prev_b_fb_num, 0U, 0U);
    CmdMsgDump(" prev_c_fb_num                      = %d  ", pCmdData->prev_c_fb_num, 0U, 0U);
    CmdMsgDump(" ln_dec_fb_num                      = %d  ", pCmdData->ln_dec_fb_num, 0U, 0U);
    CmdMsgDump(" hier_fb_num                        = %d  ", pCmdData->hier_fb_num, 0U, 0U);
    CmdMsgDump(" c2y_int_fb_num                     = %d  ", pCmdData->c2y_int_fb_num, 0U, 0U);
    CmdMsgDump(" raw_comp_blk_sz_wst                = %d  ", pCmdData->raw_comp_blk_sz_wst, 0U, 0U);
    CmdMsgDump(" raw_comp_mantissa_wst              = %d  ", pCmdData->raw_comp_mantissa_wst, 0U, 0U);
    CmdMsgDump(" extra_sec2_vert_out_max            = %d  ", pCmdData->extra_sec2_vert_out_max, 0U, 0U);
    CmdMsgDump(" prev_b_me_fb_num                   = %d  ", pCmdData->prev_b_me_fb_num, 0U, 0U);
    CmdMsgDump(" prev_c_me_fb_num                   = %d  ", pCmdData->prev_c_me_fb_num, 0U, 0U);
    CmdMsgDump(" c2y_tile_num_x_max                 = %d  ", pCmdData->c2y_tile_num_x_max, 0U, 0U);
    CmdMsgDump(" c2y_tile_num_y_max                 = %d  ", pCmdData->c2y_tile_num_y_max, 0U, 0U);
    CmdMsgDump(" warp_tile_num_x_max                = %d  ", pCmdData->warp_tile_num_x_max, 0U, 0U);
    CmdMsgDump(" warp_tile_num_y_max                = %d  ", pCmdData->warp_tile_num_y_max, 0U, 0U);
    CmdMsgDump(" c2y_big_tile_num_y_max             = %d  ", pCmdData->c2y_big_tile_num_y_max, 0U, 0U);
    CmdMsgDump(" warp_big_tile_num_y_max            = %d  ", pCmdData->warp_big_tile_num_y_max, 0U, 0U);
    CmdMsgDump(" osd_blend_area_num_max_m1          = %d  ", pCmdData->osd_blend_area_num_max_m1, 0U, 0U);
    CmdMsgDump(" osd_blend_stream_num_max_m1        = %d  ", pCmdData->osd_blend_stream_num_max_m1, 0U, 0U);
    CmdMsgDump(" effect_copy_job_num_max            = %d  ", pCmdData->effect_copy_job_num_max, 0U, 0U);
    CmdMsgDump(" effect_blend_job_num_max           = %d  ", pCmdData->effect_blend_job_num_max, 0U, 0U);
}

void dump_cmd_vproc_setup_t(const cmd_vproc_setup_t *pCmdData, const CmdInfo_t *Info)
{
    ULONG ULAddr = 0U;
    UINT32 Rval;

    CmdWriteDump("CMD_VPROC_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id                         = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" input_format                       = %d  ", pCmdData->input_format, 0U, 0U);
    CmdMsgDump(" is_raw_compressed                  = %d  ", pCmdData->is_raw_compressed, 0U, 0U);
    CmdMsgDump(" is_tile_mode                       = %d  ", pCmdData->is_tile_mode, 0U, 0U);
    CmdMsgDump(" is_li_enabled                      = %d  ", pCmdData->is_li_enabled, 0U, 0U);
    CmdMsgDump(" is_warp_enabled                    = %d  ", pCmdData->is_warp_enabled, 0U, 0U);
    CmdMsgDump(" is_mctf_enabled                    = %d  ", pCmdData->is_mctf_enabled, 0U, 0U);
    CmdMsgDump(" is_hdr_enabled                     = %d  ", pCmdData->is_hdr_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_ln_enabled                 = %d  ", pCmdData->is_prev_ln_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_a_enabled                  = %d  ", pCmdData->is_prev_a_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_b_enabled                  = %d  ", pCmdData->is_prev_b_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_c_enabled                  = %d  ", pCmdData->is_prev_c_enabled, 0U, 0U);
    CmdMsgDump(" is_prev_hier_enabled               = %d  ", pCmdData->is_prev_hier_enabled, 0U, 0U);
    CmdMsgDump(" is_compressed_out_enabled          = %d  ", pCmdData->is_compressed_out_enabled, 0U, 0U);
    CmdMsgDump(" is_c2y_burst_tiling_out            = %d  ", pCmdData->is_c2y_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" is_hier_burst_tiling_out           = %d  ", pCmdData->is_hier_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" is_mctf_ref_win_mode               = %d  ", pCmdData->is_mctf_ref_win_mode, 0U, 0U);
    CmdMsgDump(" prev_mode                          = %d  ", pCmdData->prev_mode, 0U, 0U);
    CmdMsgDump(" is_aaa_enabled                     = %d  ", pCmdData->is_aaa_enabled, 0U, 0U);
    CmdMsgDump(" cfg_aaa_by_ae_y                    = %d  ", pCmdData->cfg_aaa_by_ae_y, 0U, 0U);
    CmdMsgDump(" is_fast_y2y_en                     = %d  ", pCmdData->is_fast_y2y_en, 0U, 0U);
    CmdMsgDump(" is_hier_poly_sqrt2                 = %d  ", pCmdData->is_hier_poly_sqrt2, 0U, 0U);
    CmdMsgDump(" is_hdr_blend_dram_out_enabled      = %d  ", pCmdData->is_hdr_blend_dram_out_enabled, 0U, 0U);
    CmdMsgDump(" is_warp_dram_out_enabled           = %d  ", pCmdData->is_warp_dram_out_enabled, 0U, 0U);
    CmdMsgDump(" is_mcts_dram_out_enabled           = %d  ", pCmdData->is_mcts_dram_out_enabled, 0U, 0U);
    CmdMsgDump(" is_c2y_dram_out_enabled            = %d  ", pCmdData->is_c2y_dram_out_enabled, 0U, 0U);
    CmdMsgDump(" c2y_tile_num_x                     = %d  ", pCmdData->c2y_tile_num_x, 0U, 0U);
    CmdMsgDump(" c2y_tile_num_y                     = %d  ", pCmdData->c2y_tile_num_y, 0U, 0U);
    CmdMsgDump(" warp_tile_num_x                    = %d  ", pCmdData->warp_tile_num_x, 0U, 0U);
    CmdMsgDump(" warp_tile_num_y                    = %d  ", pCmdData->warp_tile_num_y, 0U, 0U);
    CmdMsgDump(" ln_det_src                         = %d  ", pCmdData->ln_det_src, 0U, 0U);
    CmdMsgDump(" aaa_cfa_mux_sel                    = %d  ", pCmdData->aaa_cfa_mux_sel, 0U, 0U);
    CmdMsgDump(" is_dzoom_enabled                   = %d  ", pCmdData->is_dzoom_enabled, 0U, 0U);
    CmdMsgDump(" is_mipi_yuyv_enabled               = %d  ", pCmdData->is_mipi_yuyv_enabled, 0U, 0U);
    CmdMsgDump(" is_pic_info_cache_enabled          = %d  ", pCmdData->is_pic_info_cache_enabled, 0U, 0U);
    CmdMsgDump(" prev_a_format                      = %d  ", pCmdData->prev_a_format, 0U, 0U);
    CmdMsgDump(" prev_a_src                         = %d  ", pCmdData->prev_a_src, 0U, 0U);
    CmdMsgDump(" prev_a_dst                         = %d  ", pCmdData->prev_a_dst, 0U, 0U);
    CmdMsgDump(" prev_a_frame_rate                  = %d  ", pCmdData->prev_a_frame_rate, 0U, 0U);
    CmdMsgDump(" prev_b_sync_to_enc                 = %d  ", pCmdData->prev_b_sync_to_enc, 0U, 0U);
    CmdMsgDump(" prev_b_dst                         = %d  ", pCmdData->prev_b_dst, 0U, 0U);
    CmdMsgDump(" prev_b_frame_rate                  = %d  ", pCmdData->prev_b_frame_rate, 0U, 0U);
    CmdMsgDump(" prev_c_format                      = %d  ", pCmdData->prev_c_format, 0U, 0U);
    CmdMsgDump(" prev_c_src                         = %d  ", pCmdData->prev_c_src, 0U, 0U);
    CmdMsgDump(" prev_c_dst                         = %d  ", pCmdData->prev_c_dst, 0U, 0U);
    CmdMsgDump(" prev_c_frame_rate                  = %d  ", pCmdData->prev_c_frame_rate, 0U, 0U);
    CmdMsgDump(" prev_a_ch_fmt                      = %d  ", pCmdData->prev_a_ch_fmt, 0U, 0U);
    CmdMsgDump(" prev_b_ch_fmt                      = %d  ", pCmdData->prev_b_ch_fmt, 0U, 0U);
    CmdMsgDump(" prev_c_ch_fmt                      = %d  ", pCmdData->prev_c_ch_fmt, 0U, 0U);
    CmdMsgDump(" mctf_mode                          = %d  ", pCmdData->mctf_mode, 0U, 0U);
    CmdMsgDump(" mctf_chan                          = %d  ", pCmdData->mctf_chan, 0U, 0U);
    CmdMsgDump(" mcts_chan                          = %d  ", pCmdData->mcts_chan, 0U, 0U);
    CmdMsgDump(" mctf_cmpr                          = %d  ", pCmdData->mctf_cmpr, 0U, 0U);
    CmdMsgDump(" is_h_slice_low_delay               = %d  ", pCmdData->is_h_slice_low_delay, 0U, 0U);
    CmdMsgDump(" is_c2y_line_sync_to_warp           = %d  ", pCmdData->is_c2y_line_sync_to_warp, 0U, 0U);
    CmdMsgDump(" is_c2y_smem_sync_to_warp           = %d  ", pCmdData->is_c2y_smem_sync_to_warp, 0U, 0U);
    CmdMsgDump(" is_osd_mixer_enabled               = %d  ", pCmdData->is_osd_mixer_enabled, 0U, 0U);
    CmdMsgDump(" frm_delay_time_ticks               = %d  ", pCmdData->frm_delay_time_ticks, 0U, 0U);
    CmdMsgDump(" W_main                             = %d  ", pCmdData->W_main, 0U, 0U);
    CmdMsgDump(" H_main                             = %d  ", pCmdData->H_main, 0U, 0U);
    CmdMsgDump(" prev_a_w                           = %d  ", pCmdData->prev_a_w, 0U, 0U);
    CmdMsgDump(" prev_a_h                           = %d  ", pCmdData->prev_a_h, 0U, 0U);
    CmdMsgDump(" prev_b_w                           = %d  ", pCmdData->prev_b_w, 0U, 0U);
    CmdMsgDump(" prev_b_h                           = %d  ", pCmdData->prev_b_h, 0U, 0U);
    CmdMsgDump(" prev_c_w                           = %d  ", pCmdData->prev_c_w, 0U, 0U);
    CmdMsgDump(" prev_c_h                           = %d  ", pCmdData->prev_c_h, 0U, 0U);
    CmdMsgDump(" prev_ln_w                          = %d  ", pCmdData->prev_ln_w, 0U, 0U);
    CmdMsgDump(" prev_ln_h                          = %d  ", pCmdData->prev_ln_h, 0U, 0U);
    CmdMsgDump(" hier_poly_w                        = %d  ", pCmdData->hier_poly_w, 0U, 0U);
    CmdMsgDump(" hier_poly_h                        = %d  ", pCmdData->hier_poly_h, 0U, 0U);
    CmdMsgDump(" warp_tile_overlap_x                = %d  ", pCmdData->warp_tile_overlap_x, 0U, 0U);
    CmdMsgDump(" hier_poly_tile_overlap_x           = %d  ", pCmdData->hier_poly_tile_overlap_x, 0U, 0U);
    CmdMsgDump(" warp_tile_overlap_y                = %d  ", pCmdData->warp_tile_overlap_y, 0U, 0U);
    CmdMsgDump(" vwarp_chroma_scale_max             = %d  ", pCmdData->vwarp_chroma_scale_max, 0U, 0U);
    CmdMsgDump(" warp_wait_lines                    = %d  ", pCmdData->warp_wait_lines, 0U, 0U);
    CmdMsgDump(" raw_tile_overlap_x                 = %d  ", pCmdData->raw_tile_overlap_x, 0U, 0U);
    CmdMsgDump(" is_3A_stat_only                    = %d  ", pCmdData->is_3A_stat_only, 0U, 0U);
    CmdMsgDump(" proc_mode                          = %d  ", pCmdData->proc_mode, 0U, 0U);
    CmdMsgDump(" num_of_exp                         = %d  ", pCmdData->num_of_exp, 0U, 0U);
    CmdMsgDump(" raw_tile_overlap_x_ext             = %d  ", pCmdData->raw_tile_overlap_x_ext, 0U, 0U);
    CmdMsgDump(" ext_buf_mask                       = 0x%X", pCmdData->ext_buf_mask, 0U, 0U);
    CmdMsgDump(" c2y_big_tile_num_y                 = %d  ", pCmdData->c2y_big_tile_num_y, 0U, 0U);
    CmdMsgDump(" warp_big_tile_num_y                = %d  ", pCmdData->warp_big_tile_num_y, 0U, 0U);
    CmdMsgDump(" slice_layout_ptr                   = 0x%X", pCmdData->slice_layout_ptr, 0U, 0U);
    CmdMsgDump(" slice_layout_usize                 = %d  ", pCmdData->slice_layout_usize, 0U, 0U);
    CmdMsgDump(" ext_mem_cfg_num                    = %d  ", pCmdData->ext_mem_cfg_num, 0U, 0U);
    CmdMsgDump(" raw_tile_alignment_mode            = %d  ", pCmdData->raw_tile_alignment_mode, 0U, 0U);
    CmdMsgDump(" mctf_cmpr                          = %d  ", pCmdData->mctf_cmpr, 0U, 0U);
    CmdMsgDump(" proc_hds_as_main                   = %d  ", pCmdData->proc_hds_as_main, 0U, 0U);
    CmdMsgDump(" raw_tile_overlap_mode              = %d  ", pCmdData->raw_tile_overlap_mode, 0U, 0U);
    CmdMsgDump(" prev_a_presend_to_vout             = %d  ", pCmdData->prev_a_presend_to_vout, 0U, 0U);
    CmdMsgDump(" prev_b_presend_to_vout             = %d  ", pCmdData->prev_b_presend_to_vout, 0U, 0U);
    CmdMsgDump(" prev_c_presend_to_vout             = %d  ", pCmdData->prev_c_presend_to_vout, 0U, 0U);
    CmdMsgDump(" c2y_delay_prefetch                 = %d  ", pCmdData->c2y_delay_prefetch, 0U, 0U);
    CmdMsgDump(" main_pin_out_type                  = %d  ", pCmdData->main_pin_out_type, 0U, 0U);
    CmdMsgDump(" vp_msg_seq_no_mode                 = %d  ", pCmdData->vp_msg_seq_no_mode, 0U, 0U);
#ifdef SUPPORT_VPROC_OSD_INSERT
    CmdMsgDump(" delayline_ena                      = %d  ", pCmdData->delayline_ena, 0U, 0U);
#endif
    if ((pCmdData->slice_layout_ptr > 0U) &&
        (pCmdData->slice_layout_usize > 0U)) {
        UINT32 x, y, idx;
        const DSP_SLICE_LAYOUT_s *pSliceLayout;

        Rval = dsp_osal_cli2virt(pCmdData->slice_layout_ptr, &ULAddr);
        dsp_osal_typecast(&pSliceLayout, &ULAddr);
        if (Rval == OK) {
            for (y = 0U; y < pCmdData->c2y_big_tile_num_y; y++) {
                for (x = 0U; x < pCmdData->c2y_tile_num_x; x++) {
                    idx = (y*pCmdData->c2y_tile_num_x) + x;
                    CmdMsgDump(" ====== [%d][%d] ======", x, y, 0U);
                    CmdMsgDump("   Index                            = %d, %d", pSliceLayout[idx].TileColIdx, pSliceLayout[idx].TileRowIdx, 0U);
                    CmdMsgDump("   Cord                             = %d, %d", pSliceLayout[idx].TileColStart, pSliceLayout[idx].TileRowStart, 0U);
                    CmdMsgDump("   Win                              = %dx%d",  pSliceLayout[idx].TileColWidth, pSliceLayout[idx].TileRowHeight, 0U);
                }
            }
        }
    }

    if ((pCmdData->ext_mem_cfg_num > 0U) &&
        (pCmdData->ext_mem_cfg_addr > 0U)) {
        UINT32 i;
        const ext_mem_desc_t *pExtMemDesc;

        Rval = dsp_osal_cli2virt(pCmdData->ext_mem_cfg_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pExtMemDesc, &ULAddr);
            for (i = 0U; i < pCmdData->ext_mem_cfg_num; i++) {
                CmdMsgDump(" ====== [%d] ======", i, 0U, 0U);
                CmdMsgDump("   memory_type                      = %d  ", pExtMemDesc[i].memory_type, 0U, 0U);
                CmdMsgDump("   enable                           = %d  ", pExtMemDesc[i].enable, 0U, 0U);
                CmdMsgDump("   max_daddr_slot                   = %d  ", pExtMemDesc[i].max_daddr_slot, 0U, 0U);
                CmdMsgDump("   pool_buf_num                     = %d  ", pExtMemDesc[i].pool_buf_num, 0U, 0U);
            }
        }
    }
}

void dump_cmd_vproc_mctf_config_t(const cmd_vproc_mctf_config_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_MCTF_CONFIG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id                         = %d", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" noise_filter_strength              = %d", pCmdData->noise_filter_strength, 0U, 0U);
    CmdMsgDump(" mctf_chan                          = %d", pCmdData->mctf_chan, 0U, 0U);
    CmdMsgDump(" mcts_chan                          = %d", pCmdData->mcts_chan, 0U, 0U);
    CmdMsgDump(" cmpr_en                            = %d", pCmdData->cmpr_en, 0U, 0U);
    CmdMsgDump(" mctf_mode                          = %d", pCmdData->mctf_mode, 0U, 0U);
    CmdMsgDump(" bitrate_y                          = %d", pCmdData->bitrate_y, 0U, 0U);
    CmdMsgDump(" bitrate_uv                         = %d", pCmdData->bitrate_uv, 0U, 0U);
    CmdMsgDump(" bypass_mctf                        = %d", pCmdData->bypass_mctf, 0U, 0U);
    CmdMsgDump(" mctf_cfg_0_dbase                   = %d", pCmdData->mctf_cfg_0_dbase, 0U, 0U);
    CmdMsgDump(" mctf_cfg_1_dbase                   = %d", pCmdData->mctf_cfg_1_dbase, 0U, 0U);
    CmdMsgDump(" mctf_cfg_ta_dbase                  = %d", pCmdData->mctf_cfg_ta_dbase, 0U, 0U);
    CmdMsgDump(" cmpr_cfg_dbase                     = %d", pCmdData->cmpr_cfg_dbase, 0U, 0U);
    CmdMsgDump(" loadcfg_type                       = 0x%x", pCmdData->loadcfg_type, 0U, 0U);
#if 0
    CmdMsgDump("   mctf_config_update               = %d", pCmdData->loadcfg_type.cmds.mctf_config_update, 0U, 0U);
    CmdMsgDump("   frame_active_update              = %d", pCmdData->loadcfg_type.cmds.frame_active_update, 0U, 0U);
    CmdMsgDump("   motion_vectors_update            = %d", pCmdData->loadcfg_type.cmds.motion_vectors_update, 0U, 0U);
    CmdMsgDump("   histogram_0_update               = %d", pCmdData->loadcfg_type.cmds.histogram_0_update, 0U, 0U);
    CmdMsgDump("   histogram_1_update               = %d", pCmdData->loadcfg_type.cmds.histogram_1_update, 0U, 0U);
    CmdMsgDump("   histogram_2_update               = %d", pCmdData->loadcfg_type.cmds.histogram_2_update, 0U, 0U);
    CmdMsgDump("   histogram_3_update               = %d", pCmdData->loadcfg_type.cmds.histogram_3_update, 0U, 0U);
    CmdMsgDump("   curve_b_update                   = %d", pCmdData->loadcfg_type.cmds.curve_b_update, 0U, 0U);
    CmdMsgDump("   curve_c_update                   = %d", pCmdData->loadcfg_type.cmds.curve_c_update, 0U, 0U);
    CmdMsgDump("   curve_d_update                   = %d", pCmdData->loadcfg_type.cmds.curve_d_update, 0U, 0U);
    CmdMsgDump("   tone_adj_config_update           = %d", pCmdData->loadcfg_type.cmds.tone_adj_config_update, 0U, 0U);
    CmdMsgDump("   tone_hist_tbl_update             = %d", pCmdData->loadcfg_type.cmds.tone_hist_tbl_update, 0U, 0U);
    CmdMsgDump("   mctf_3d_lvl_l_update             = %d", pCmdData->loadcfg_type.cmds.mctf_3d_lvl_l_update, 0U, 0U);
    CmdMsgDump("   mctf_3d_lvl_u_update             = %d", pCmdData->loadcfg_type.cmds.mctf_3d_lvl_u_update, 0U, 0U);
    CmdMsgDump("   curve_sb2_update                 = %d", pCmdData->loadcfg_type.cmds.curve_sb2_update, 0U, 0U);
    CmdMsgDump("   ref_luma_remap_update            = %d", pCmdData->loadcfg_type.cmds.ref_luma_remap_update, 0U, 0U);
    CmdMsgDump("   mcts_config_update               = %d", pCmdData->loadcfg_type.cmds.mcts_config_update, 0U, 0U);
    CmdMsgDump("   shpb_config_update               = %d", pCmdData->loadcfg_type.cmds.shpb_config_update, 0U, 0U);
    CmdMsgDump("   shpc_config_update               = %d", pCmdData->loadcfg_type.cmds.shpc_config_update, 0U, 0U);
    CmdMsgDump("   shpb_lpf_coef_1_update           = %d", pCmdData->loadcfg_type.cmds.shpb_lpf_coef_1_update, 0U, 0U);
    CmdMsgDump("   shpb_lpf_coef_2_update           = %d", pCmdData->loadcfg_type.cmds.shpb_lpf_coef_2_update, 0U, 0U);
    CmdMsgDump("   shpc_lpf_coef_1_update           = %d", pCmdData->loadcfg_type.cmds.shpc_lpf_coef_1_update, 0U, 0U);
    CmdMsgDump("   shpc_lpf_coef_2_update           = %d", pCmdData->loadcfg_type.cmds.shpc_lpf_coef_2_update, 0U, 0U);
    CmdMsgDump("   shpb_alpha_update                = %d", pCmdData->loadcfg_type.cmds.shpb_alpha_update, 0U, 0U);
    CmdMsgDump("   shpc_alpha_update                = %d", pCmdData->loadcfg_type.cmds.shpc_alpha_update, 0U, 0U);
    CmdMsgDump("   shpb_coring1_tbl_update          = %d", pCmdData->loadcfg_type.cmds.shpb_coring1_tbl_update, 0U, 0U);
    CmdMsgDump("   shpc_coring1_tbl_update          = %d", pCmdData->loadcfg_type.cmds.shpc_coring1_tbl_update, 0U, 0U);
    CmdMsgDump("   shpb_coring2_tbl_update          = %d", pCmdData->loadcfg_type.cmds.shpb_coring2_tbl_update, 0U, 0U);
    CmdMsgDump("   shrp_3d_lvl_l_update             = %d", pCmdData->loadcfg_type.cmds.shrp_3d_lvl_l_update, 0U, 0U);
    CmdMsgDump("   shrp_3d_lvl_u_update             = %d", pCmdData->loadcfg_type.cmds.shrp_3d_lvl_u_update, 0U, 0U);
    CmdMsgDump("   cmpress_config_1_update          = %d", pCmdData->loadcfg_type.cmds.cmpress_config_1_update, 0U, 0U);
    CmdMsgDump("   cmpress_config_2_update          = %d", pCmdData->loadcfg_type.cmds.cmpress_config_2_update, 0U, 0U);
#endif
}

void dump_cmd_vproc_img_prmd_setup_t(const cmd_vproc_img_pyramid_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_IMG_PRMD_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id           = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" enable_bit_mask      = 0x%X", pCmdData->enable_bit_mask, 0U, 0U);
    CmdMsgDump(" is_hier_poly_sqrt2   = %d  ", pCmdData->is_hier_poly_sqrt2, 0U, 0U);
    CmdMsgDump(" is_grp_cmd           = %d  ", pCmdData->is_grp_cmd, 0U, 0U);
    CmdMsgDump(" grp_fov_cmd_id       = %d  ", pCmdData->grp_fov_cmd_id, 0U, 0U);
    CmdMsgDump(" hier_burst_tiling_out= %d  ", pCmdData->hier_burst_tiling_out, 0U, 0U);
    CmdMsgDump(" roi_tag              = %d  ", pCmdData->roi_tag, 0U, 0U);
    CmdMsgDump(" deci_rate            = 0x%X", pCmdData->deci_rate, 0U, 0U);
    {
        UINT8 i;

        for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            UINT32 U32Val;
            CmdMsgDump("   ========== Scale Idx %02d ==========", i, 0U, 0U);
            U32Val = (UINT32)pCmdData->scale_info[i].roi_start_row;
            CmdMsgDump("   roi_start_row      = %d  ", U32Val, 0U, 0U);
            U32Val = (UINT32)pCmdData->scale_info[i].roi_start_col;
            CmdMsgDump("   roi_start_col      = %d  ", U32Val, 0U, 0U);
            U32Val = (UINT32)pCmdData->scale_info[i].roi_width;
            CmdMsgDump("   roi_width          = %d  ", U32Val, 0U, 0U);
            U32Val = (UINT32)pCmdData->scale_info[i].roi_height;
            CmdMsgDump("   roi_height         = %d  ", U32Val, 0U, 0U);
        }
    }
}

void dump_cmd_vproc_prev_setup_t(const cmd_vproc_prev_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_PREV_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id           = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" prev_id              = %d  ", pCmdData->prev_id, 0U, 0U);
    CmdMsgDump(" prev_format          = %d  ", pCmdData->prev_format, 0U, 0U);
    CmdMsgDump(" prev_src             = %d  ", pCmdData->prev_src, 0U, 0U);
    CmdMsgDump(" prev_dst             = %d  ", pCmdData->prev_dst, 0U, 0U);
    CmdMsgDump(" prev_frame_rate      = %d  ", pCmdData->prev_frame_rate, 0U, 0U);
    CmdMsgDump(" prev_freeze_ena      = %d  ", pCmdData->prev_freeze_ena, 0U, 0U);
    CmdMsgDump(" prev_roi_tile_en     = %d  ", pCmdData->prev_roi_tile_en, 0U, 0U);
    CmdMsgDump(" prev_w               = %d  ", pCmdData->prev_w, 0U, 0U);
    CmdMsgDump(" prev_h               = %d  ", pCmdData->prev_h, 0U, 0U);
    CmdMsgDump(" prev_src_w           = %d  ", pCmdData->prev_src_w, 0U, 0U);
    CmdMsgDump(" prev_src_h           = %d  ", pCmdData->prev_src_h, 0U, 0U);
    CmdMsgDump(" prev_src_x_offset    = %d  ", pCmdData->prev_src_x_offset, 0U, 0U);
    CmdMsgDump(" prev_src_y_offset    = %d  ", pCmdData->prev_src_y_offset, 0U, 0U);
    CmdMsgDump(" prev_out_width       = %d  ", pCmdData->prev_out_width, 0U, 0U);
    CmdMsgDump(" prev_out_height      = %d  ", pCmdData->prev_out_height, 0U, 0U);
}

void dump_cmd_vproc_lane_det_setup_t(const cmd_vproc_lane_det_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_LN_DET_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id           = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" pyr_filter_idx       = %d  ", pCmdData->pyr_filter_idx, 0U, 0U);
    CmdMsgDump(" is_grp_cmd           = %d  ", pCmdData->is_grp_cmd, 0U, 0U);
    CmdMsgDump(" grp_fov_cmd_id       = %d  ", pCmdData->grp_fov_cmd_id, 0U, 0U);
    CmdMsgDump(" ld_roi_start_row     = %d  ", pCmdData->ld_roi_start_row, 0U, 0U);
    CmdMsgDump(" ld_roi_start_col     = %d  ", pCmdData->ld_roi_start_col, 0U, 0U);
    CmdMsgDump(" ld_roi_height        = %d  ", pCmdData->ld_roi_height, 0U, 0U);
    CmdMsgDump(" ld_roi_width         = %d  ", pCmdData->ld_roi_width, 0U, 0U);
}

void dump_cmd_vproc_set_ext_mem_t(const cmd_vproc_set_ext_mem_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_SET_EXT_MEM", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id          = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" memory_type         = %d  ", pCmdData->memory_type, 0U, 0U);
    CmdMsgDump(" allocation_type     = %d  ", pCmdData->allocation_type, 0U, 0U);
    CmdMsgDump(" num_frm_buf         = %d  ", pCmdData->num_frm_buf, 0U, 0U);
    CmdMsgDump(" buf_pitch           = %d  ", pCmdData->buf_pitch, 0U, 0U);
    CmdMsgDump(" buf_width           = %d  ", pCmdData->buf_width, 0U, 0U);
    CmdMsgDump(" buf_height          = %d  ", pCmdData->buf_height, 0U, 0U);
    CmdMsgDump(" allocation_mode     = %d  ", pCmdData->allocation_mode, 0U, 0U);
    CmdMsgDump(" overflow_cntl       = %d  ", pCmdData->overflow_cntl, 0U, 0U);
    CmdMsgDump(" chroma_format       = %d  ", pCmdData->chroma_format, 0U, 0U);
    CmdMsgDump(" luma_img_ofs_x      = %d  ", pCmdData->luma_img_ofs_x, 0U, 0U);
    CmdMsgDump(" luma_img_ofs_y      = %d  ", pCmdData->luma_img_ofs_y, 0U, 0U);
    CmdMsgDump(" chroma_img_ofs_x    = %d  ", pCmdData->chroma_img_ofs_x, 0U, 0U);
    CmdMsgDump(" chroma_img_ofs_y    = %d  ", pCmdData->chroma_img_ofs_y, 0U, 0U);
    CmdMsgDump(" img_width           = %d  ", pCmdData->img_width, 0U, 0U);
    CmdMsgDump(" img_height          = %d  ", pCmdData->img_height, 0U, 0U);
    if (pCmdData->allocation_type == 0U/*VIN_EXT_MEM_ALLOC_TYPE_DISTINCT*/) {
        ULONG ULAddr;
        const UINT32 *pU32Addr;
        UINT16 i = 0U;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->buf_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pU32Addr, &ULAddr);
            CmdMsgDump(" ====== buf_tbl[0x%X] ======", pCmdData->buf_addr, 0U, 0U);
            for (i = 0U; i<pCmdData->num_frm_buf; i++) {
                CmdMsgDump(" buf_addr[%d]         = 0x%X", i, pU32Addr[i], 0U);
            }
        }
    } else {
        CmdMsgDump(" buf_addr            = 0x%X", pCmdData->buf_addr, 0U, 0U);
    }
    CmdMsgDump(" aux_pitch           = %d  ", pCmdData->aux_pitch, 0U, 0U);
    CmdMsgDump(" aux_width           = %d  ", pCmdData->aux_width, 0U, 0U);
    CmdMsgDump(" aux_height          = %d  ", pCmdData->aux_height, 0U, 0U);
    CmdMsgDump(" aux_img_ofs_x       = %d  ", pCmdData->aux_img_ofs_x, 0U, 0U);
    CmdMsgDump(" aux_img_ofs_y       = %d  ", pCmdData->aux_img_ofs_y, 0U, 0U);
    CmdMsgDump(" aux_img_width       = %d  ", pCmdData->aux_img_width, 0U, 0U);
    CmdMsgDump(" aux_img_height      = %d  ", pCmdData->aux_img_height, 0U, 0U);
}

void dump_cmd_vproc_stop_t(const cmd_vproc_stop_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_STOP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id          = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" stop_cap_seq_no     = 0x%X", pCmdData->stop_cap_seq_no, 0U, 0U);
    CmdMsgDump(" reset_option        = %d  ", pCmdData->reset_option, 0U, 0U);
}

void dump_cmd_vproc_osb_blend_t(const cmd_vproc_osd_blend_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_OSD_BLEND", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id          = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" stream_id           = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" cap_seq_no          = %d  ", pCmdData->cap_seq_no, 0U, 0U);
    CmdMsgDump(" enable              = %d  ", pCmdData->enable, 0U, 0U);
    CmdMsgDump(" blending_area_id    = %d  ", pCmdData->blending_area_id, 0U, 0U);
    CmdMsgDump(" osd_addr_y          = 0x%X", pCmdData->osd_addr_y, 0U, 0U);
    CmdMsgDump(" osd_addr_uv         = 0x%X", pCmdData->osd_addr_uv, 0U, 0U);
    CmdMsgDump(" alpha_addr_y        = 0x%X", pCmdData->alpha_addr_y, 0U, 0U);
    CmdMsgDump(" alpha_addr_uv       = 0x%X", pCmdData->alpha_addr_uv, 0U, 0U);
    CmdMsgDump(" osd_width           = %d  ", pCmdData->osd_width, 0U, 0U);
    CmdMsgDump(" osd_pitch           = %d  ", pCmdData->osd_pitch, 0U, 0U);
    CmdMsgDump(" osd_height          = %d  ", pCmdData->osd_height, 0U, 0U);
    CmdMsgDump(" osd_start_x         = %d  ", pCmdData->osd_start_x, 0U, 0U);
    CmdMsgDump(" osd_start_y         = %d  ", pCmdData->osd_start_y, 0U, 0U);
}

void dump_cmd_vproc_pin_out_deci_t(const cmd_vproc_pin_out_deci_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_PIN_OUT_DECIMATION", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id             = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" stream_id              = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" cap_seq_no             = %d  ", pCmdData->cap_seq_no, 0U, 0U);
    CmdMsgDump(" output_repeat_ratio    = 0x%X", pCmdData->output_repeat_ratio, 0U, 0U);
}

void dump_cmd_vproc_fov_grp_cmd_t(const cmd_vproc_fov_grp_cmd_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_GRP_CMD", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id          = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" is_sent_from_sys    = %d  ", pCmdData->is_sent_from_sys, 0U, 0U);
    CmdMsgDump(" is_vout_cmds        = %d  ", pCmdData->is_vout_cmds, 0U, 0U);
    CmdMsgDump(" vout_id             = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" grp_cmd_buf_id      = 0x%X", pCmdData->grp_cmd_buf_id, 0U, 0U);
    CmdMsgDump(" grp_cmd_buf_addr    = 0x%X", pCmdData->grp_cmd_buf_addr, 0U, 0U);
    CmdMsgDump(" num_cmds_in_buf     = %d  ", pCmdData->num_cmds_in_buf, 0U, 0U);
}

void dump_cmd_vproc_echo_cmd_t(const cmd_vproc_echo_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_ECHO_CMD", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id         = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" grp_cmd_buf_id     = 0x%X", pCmdData->grp_cmd_buf_id, 0U, 0U);
}

#ifdef SUPPORT_VPROC_OSD_INSERT
void dump_cmd_vproc_osd_insert_cmd_t(const cmd_vproc_osd_insert_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_OSD_INSERT", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" chan_id                    = %d  ", pCmdData->chan_id, 0U, 0U);
    CmdMsgDump(" stream_id                  = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" osd_enable                 = %d  ", pCmdData->osd_enable, 0U, 0U);
    CmdMsgDump(" osd_mode                   = %d  ", pCmdData->osd_mode, 0U, 0U);
    CmdMsgDump(" osd_insert_always          = %d  ", pCmdData->osd_insert_always, 0U, 0U);
    CmdMsgDump(" osd_num_regions_minus1     = %d  ", pCmdData->osd_num_regions_minus1, 0U, 0U);
    CmdMsgDump(" force_update_flag          = %d  ", pCmdData->force_update_flag, 0U, 0U);
    CmdMsgDump(" sync_timestamp             = %d  ", pCmdData->sync_timestamp, 0U, 0U);
    CmdMsgDump(" target_pts                 = %d  ", pCmdData->target_pts, 0U, 0U);
    CmdMsgDump(" osd_region_batch_daddr                 = 0x%X", pCmdData->osd_region_batch_daddr, 0U, 0U);
    if (pCmdData->osd_region_batch_daddr > 0U) {
        ULONG ULAddr;
        const osd_insert_buf_t *pOsdInsertBuf;
        UINT32 Rval;
        UINT8 i;
        UINT16 U16Val;

        Rval = dsp_osal_cli2virt(pCmdData->osd_region_batch_daddr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pOsdInsertBuf, &ULAddr);
            for (i = 0; i < (pCmdData->osd_num_regions_minus1 + 1U); i++) {
                CmdMsgDump("   ================= Index %02d ================", i, 0U, 0U);
				CmdMsgDump("   osd_sync.user_id                     = 0x%X", pOsdInsertBuf[i].osd_sync.user_id, 0U, 0U);
                CmdMsgDump("   osd_sync.pin_code                    = 0x%X", pOsdInsertBuf[i].osd_sync.pin_code, 0U, 0U);
                CmdMsgDump("   osd_clut_daddr                       = 0x%X", pOsdInsertBuf[i].osd_clut_daddr, 0U, 0U);
                CmdMsgDump("   osd_daddr                            = 0x%X", pOsdInsertBuf[i].osd_daddr, 0U, 0U);
                dsp_osal_typecast(&U16Val, &pOsdInsertBuf[i].osd_size.x);
                CmdMsgDump("   osd_size.x                           = %d  ", (UINT32)U16Val, 0U, 0U);
                dsp_osal_typecast(&U16Val, &pOsdInsertBuf[i].osd_size.y);
                CmdMsgDump("   osd_size.y                           = %d  ", (UINT32)U16Val, 0U, 0U);
                CmdMsgDump("   osd_size.w                           = %d  ", pOsdInsertBuf[i].osd_size.w, 0U, 0U);
                CmdMsgDump("   osd_size.h                           = %d  ", pOsdInsertBuf[i].osd_size.h, 0U, 0U);
                CmdMsgDump("   osd_dpitch                           = %d  ", pOsdInsertBuf[i].osd_dpitch, 0U, 0U);
            }
        }
    }
}

void dump_cmd_vproc_set_streams_delay_cmd_t(const cmd_vproc_set_streams_delay_t *pCmdData, const CmdInfo_t *Info)
{
    UINT16 i;

    CmdWriteDump("CMD_VPROC_SET_STREAMS_DELAY", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id                 = %d  ", pCmdData->channel_id, 0U, 0U);
    for (i = 0U; i < VPROC_STREAM_NUMS; i++) {
        CmdMsgDump(" delay_in_ms[%d]             = %d  ", i, pCmdData->delay_in_ms[i], 0U);

    }
}
#endif

void dump_cmd_vproc_multi_stream_pp_t(const cmd_vproc_multi_stream_pp_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_MULTI_STREAM_PP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" chan_id                 = %d  ", pCmdData->chan_id, 0U, 0U);
    CmdMsgDump(" effect_grp_id           = %d  ", pCmdData->effect_grp_id, 0U, 0U);
    CmdMsgDump(" str_id                  = %d  ", pCmdData->str_id, 0U, 0U);
    CmdMsgDump(" final_output_buf_id     = %d  ", pCmdData->final_output_buf_id, 0U, 0U);
    CmdMsgDump(" output_dst              = %d  ", pCmdData->output_dst, 0U, 0U);
    CmdMsgDump(" num_of_inputs           = %d  ", pCmdData->num_of_inputs, 0U, 0U);
    CmdMsgDump(" final_output_presend           = %d  ", pCmdData->final_output_presend, 0U, 0U);

    if (pCmdData->input_pp_cfg_addr_array != 0U) {
        UINT32 i, j;
        ULONG ULAddr;
        const vproc_pp_stream_cntl_t *PpStrmCtl;
        const vproc_post_r2y_copy_cfg_t *CpCtrl;
        const vproc_y2y_blending_cfg_t *BldCtrl;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->input_pp_cfg_addr_array, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&PpStrmCtl, &ULAddr);
            for (i = 0; i < pCmdData->num_of_inputs; i++) {
                CmdMsgDump("   ================= Input %02d 0x%x ================", i, (UINT32)ULAddr, 0U);
                CmdMsgDump("   input_channel_num                    = %d  ", PpStrmCtl[i].input_channel_num, 0U, 0U);
                CmdMsgDump("   input_stream_num                     = %d  ", PpStrmCtl[i].input_stream_num, 0U, 0U);
                CmdMsgDump("   output_buf_id                        = %d  ", PpStrmCtl[i].output_buf_id, 0U, 0U);
                CmdMsgDump("   output_x_ofs                         = %d  ", PpStrmCtl[i].output_x_ofs, 0U, 0U);
                CmdMsgDump("   output_y_ofs                         = %d  ", PpStrmCtl[i].output_y_ofs, 0U, 0U);
                CmdMsgDump("   output_width                         = %d  ", PpStrmCtl[i].output_width, 0U, 0U);
                CmdMsgDump("   output_height                        = %d  ", PpStrmCtl[i].output_height, 0U, 0U);
                CmdMsgDump("   is_alloc_out_buf                     = %d  ", PpStrmCtl[i].stream_cfg.is_alloc_out_buf, 0U, 0U);
                CmdMsgDump("   is_last_blending_channel             = %d  ", PpStrmCtl[i].stream_cfg.is_last_blending_channel, 0U, 0U);
                CmdMsgDump("   is_last_passthrough_channel          = %d  ", PpStrmCtl[i].stream_cfg.is_last_passthrough_channel, 0U, 0U);
                CmdMsgDump("   is_last_copy_channel                 = %d  ", PpStrmCtl[i].stream_cfg.is_last_copy_channel, 0U, 0U);
                CmdMsgDump("   num_of_post_r2y_copy_ops             = %d  ", PpStrmCtl[i].stream_cfg.num_of_post_r2y_copy_ops, 0U, 0U);
                CmdMsgDump("   num_of_y2y_blending_ops              = %d  ", PpStrmCtl[i].stream_cfg.num_of_y2y_blending_ops, 0U, 0U);
                CmdMsgDump("   post_r2y_copy_cfg_address            = 0x%X", PpStrmCtl[i].post_r2y_copy_cfg_address, 0U, 0U);
                if (PpStrmCtl[i].post_r2y_copy_cfg_address != 0U) {
                    Rval = dsp_osal_cli2virt(PpStrmCtl[i].post_r2y_copy_cfg_address, &ULAddr);
                    if (Rval == OK) {
                        dsp_osal_typecast(&CpCtrl, &ULAddr);
                        for (j = 0; j < PpStrmCtl[i].stream_cfg.num_of_post_r2y_copy_ops; j++) {
                            CmdMsgDump("     ======= Copy Config %02d 0x%x ======= ", j, (UINT32)ULAddr, 0U);
                            CmdMsgDump("     output_buf_id              = %d  ", CpCtrl[j].output_buf_id, 0U, 0U);
                            CmdMsgDump("     is_alloc_out_buf           = %d  ", CpCtrl[j].is_alloc_out_buf, 0U, 0U);
                            CmdMsgDump("     is_rotate                  = %d  ", CpCtrl[j].is_rotate, 0U, 0U);
                            CmdMsgDump("     input_start_x              = %d  ", CpCtrl[j].input_start_x, 0U, 0U);
                            CmdMsgDump("     input_start_y              = %d  ", CpCtrl[j].input_start_y, 0U, 0U);
                            CmdMsgDump("     output_start_x             = %d  ", CpCtrl[j].output_start_x, 0U, 0U);
                            CmdMsgDump("     output_start_y             = %d  ", CpCtrl[j].output_start_y, 0U, 0U);
                            CmdMsgDump("     copy_width                 = %d  ", CpCtrl[j].copy_width, 0U, 0U);
                            CmdMsgDump("     copy_height                = %d  ", CpCtrl[j].copy_height, 0U, 0U);
                        }
                    }
                }
                CmdMsgDump("   y2y_blending_cfg_address             = 0x%X", PpStrmCtl[i].y2y_blending_cfg_address, 0U, 0U);
                if (PpStrmCtl[i].y2y_blending_cfg_address != 0U) {
                    Rval = dsp_osal_cli2virt(PpStrmCtl[i].y2y_blending_cfg_address, &ULAddr);
                    if (Rval == OK) {
                        dsp_osal_typecast(&BldCtrl, &ULAddr);
                        for (j = 0; j < PpStrmCtl[i].stream_cfg.num_of_y2y_blending_ops; j++) {
                            CmdMsgDump("     ======= Blend Config %02d 0x%x ======= ", j, (UINT32)ULAddr, 0U);
                            CmdMsgDump("     output_buf_id                  = %d  ", BldCtrl[j].output_buf_id, 0U, 0U);
                            CmdMsgDump("     is_alloc_out_buf               = %d  ", BldCtrl[j].is_alloc_out_buf, 0U, 0U);
                            CmdMsgDump("     is_rotate_1st_inp              = %d  ", BldCtrl[j].is_rotate_1st_inp, 0U, 0U);
                            CmdMsgDump("     is_rotate_2nd_inp              = %d  ", BldCtrl[j].is_rotate_2nd_inp, 0U, 0U);
                            CmdMsgDump("     first_input_buf_id             = %d  ", BldCtrl[j].first_input_buf_id, 0U, 0U);
                            CmdMsgDump("     first_inp_x_ofs                = %d  ", BldCtrl[j].first_inp_x_ofs, 0U, 0U);
                            CmdMsgDump("     first_inp_y_ofs                = %d  ", BldCtrl[j].first_inp_y_ofs, 0U, 0U);
                            CmdMsgDump("     second_input_buf_id            = %d  ", BldCtrl[j].second_input_buf_id, 0U, 0U);
                            CmdMsgDump("     second_inp_x_ofs               = %d  ", BldCtrl[j].second_inp_x_ofs, 0U, 0U);
                            CmdMsgDump("     second_inp_y_ofs               = %d  ", BldCtrl[j].second_inp_y_ofs, 0U, 0U);
                            CmdMsgDump("     blending_width                 = %d  ", BldCtrl[j].blending_width, 0U, 0U);
                            CmdMsgDump("     blending_height                = %d  ", BldCtrl[j].blending_height, 0U, 0U);
                            CmdMsgDump("     output_x_ofs                   = %d  ", BldCtrl[j].output_x_ofs, 0U, 0U);
                            CmdMsgDump("     output_y_ofs                   = %d  ", BldCtrl[j].output_y_ofs, 0U, 0U);
                            CmdMsgDump("     is_alpha_stream_value          = %d  ", BldCtrl[j].is_alpha_stream_value, 0U, 0U);
                            CmdMsgDump("     alpha_luma_value               = %d  ", BldCtrl[j].alpha_luma_value, 0U, 0U);
                            CmdMsgDump("     alpha_u_value                  = %d  ", BldCtrl[j].alpha_u_value, 0U, 0U);
                            CmdMsgDump("     alpha_v_value                  = %d  ", BldCtrl[j].alpha_v_value, 0U, 0U);
                            CmdMsgDump("     alpha_luma_dram_pitch          = %d  ", BldCtrl[j].alpha_luma_dram_pitch, 0U, 0U);
                            CmdMsgDump("     alpha_chroma_dram_pitch        = %d  ", BldCtrl[j].alpha_chroma_dram_pitch, 0U, 0U);
                            CmdMsgDump("     alpha_luma_dram_addr           = 0x%X", BldCtrl[j].alpha_luma_dram_addr, 0U, 0U);
                            CmdMsgDump("     alpha_chroma_dram_addr         = 0x%X", BldCtrl[j].alpha_chroma_dram_addr, 0U, 0U);
                        }
                    }
                }
            }
        }
    }
}

void dump_cmd_vproc_set_effect_buf_img_sz_t(const cmd_vproc_set_effect_buf_img_sz_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_SET_EFFECT_BUF_IMG_SZ", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" chan_id                 = %d", pCmdData->chan_id, 0U, 0U);
    CmdMsgDump(" effect_grp_id           = %d", pCmdData->effect_grp_id, 0U, 0U);
    CmdMsgDump(" str_id                  = %d", pCmdData->str_id, 0U, 0U);
    CmdMsgDump(" img_width               = %d", pCmdData->img_width, 0U, 0U);
    CmdMsgDump(" img_height              = %d", pCmdData->img_height, 0U, 0U);
    CmdMsgDump(" img_x_ofs               = %d", pCmdData->img_x_ofs, 0U, 0U);
    CmdMsgDump(" img_y_ofs               = %d", pCmdData->img_y_ofs, 0U, 0U);
    CmdMsgDump(" ch_fmt                  = %d", pCmdData->ch_fmt, 0U, 0U);
}

void dump_cmd_vproc_multi_chan_proc_order_t(const cmd_vproc_multi_chan_proc_order_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VPROC_MULTI_CHAN_PROC_ORDER", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" chan_id                 = %d  ", pCmdData->chan_id, 0U, 0U);
    CmdMsgDump(" str_id                  = %d  ", pCmdData->str_id, 0U, 0U);
    CmdMsgDump(" proc_order_check_en     = %d  ", pCmdData->proc_order_check_en, 0U, 0U);
    CmdMsgDump(" grp_id                  = %d  ", pCmdData->grp_id, 0U, 0U);
    CmdMsgDump(" free_run_grp            = %d  ", pCmdData->free_run_grp, 0U, 0U);
    CmdMsgDump(" is_runtime_change       = %d  ", pCmdData->is_runtime_change, 0U, 0U);
    CmdMsgDump(" cap_seq_no              = %d  ", pCmdData->cap_seq_no, 0U, 0U);
    CmdMsgDump(" num_of_vproc_channel    = %d  ", pCmdData->num_of_vproc_channel, 0U, 0U);
    CmdMsgDump(" proc_order_addr         = 0x%x  ", pCmdData->proc_order_addr, 0U, 0U);
    if (pCmdData->num_of_vproc_channel != 0U) {
        UINT8 i;
        ULONG ULAddr;
        const UINT8 *pOrder;
        UINT32 Rval;

        if (pCmdData->num_of_vproc_channel < 16U) {
            for (i = 0; i < pCmdData->num_of_vproc_channel; i++) {
                CmdMsgDump("   ========== [Array] Priority %02d ==========", i, 0U, 0U);
                CmdMsgDump("   proc_order            = 0x%X", pCmdData->proc_order[i], 0U, 0U);
            }
        }
        if (pCmdData->num_of_vproc_channel < NUM_VPROC_MAX_CHAN) {
            Rval = dsp_osal_cli2virt(pCmdData->proc_order_addr, &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pOrder, &ULAddr);
                for (i = 0U; i < pCmdData->num_of_vproc_channel; i++) {
                    CmdMsgDump("   ========== [Address] Priority %02d ========", i, 0U, 0U);
                    CmdMsgDump("   proc_order            = 0x%X", pOrder[i], 0U, 0U);
                }
            }
        }
    }
}

void dump_cmd_vproc_set_vproc_grping_t(const cmd_vproc_set_vproc_grping *pCmdData, const CmdInfo_t *Info)
{
    UINT32 i, j;
    ULONG ULAddr;
    const UINT8 *pGrpNum;
    const UINT8 *pGrpOrder;
    UINT32 Rval;

    CmdWriteDump("CMD_VPROC_SET_VPROC_GRPING", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" num_vproc_groups             = %d  ", pCmdData->num_of_vproc_groups, 0U, 0U);
    CmdMsgDump(" max_vprocs_per_group         = %d  ", pCmdData->max_vprocs_per_group, 0U, 0U);
    CmdMsgDump(" free_run_grp                 = 0x%X", pCmdData->free_run_grp, 0U, 0U);
    CmdMsgDump(" num_vprocs_per_group_addr    = 0x%X", pCmdData->num_vprocs_per_group_addr, 0U, 0U);
    CmdMsgDump(" vprocs_in_a_group_addr       = 0x%X", pCmdData->vprocs_in_a_group_addr, 0U, 0U);

    Rval = dsp_osal_cli2virt(pCmdData->num_vprocs_per_group_addr, &ULAddr);
    if (Rval == OK) {
        dsp_osal_typecast(&pGrpNum, &ULAddr);
        Rval = dsp_osal_cli2virt(pCmdData->vprocs_in_a_group_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pGrpOrder, &ULAddr);
            for (i=0U; i<pCmdData->num_of_vproc_groups; i++) {
                CmdMsgDump("   ========== Grp[%d] VprocNum[%d] ==========", i, pGrpNum[i], 0U);
                for (j = 0U; j < (pGrpNum[i]); j++) {
                    CmdMsgDump("     Order[%d]            = %d  ", j, pGrpOrder[j+(i*(pCmdData->max_vprocs_per_group))], 0U);
                }
            }
        }
    }
}

void dump_cmd_vin_start_t(const cmd_vin_start_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_START", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                                 = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" cmd_msg_decimation_rate                = %d  ", pCmdData->cmd_msg_decimation_rate, 0U, 0U);
    CmdMsgDump(" vin_smem_to_vproc                      = %d  ", pCmdData->vin_smem_to_vproc, 0U, 0U);
    CmdMsgDump(" vin_line_sync_to_vproc                 = %d  ", pCmdData->vin_line_sync_to_vproc, 0U, 0U);
    CmdMsgDump(" is_compression_en                      = %d  ", pCmdData->is_compression_en, 0U, 0U);
    CmdMsgDump(" send_input_data_type                   = %d  ", pCmdData->send_input_data_type, 0U, 0U);
    CmdMsgDump(" is_reset_frm_sync                      = %d  ", pCmdData->is_reset_frm_sync, 0U, 0U);
    CmdMsgDump(" is_auto_frm_drop_en                    = %d  ", pCmdData->is_auto_frm_drop_en, 0U, 0U);
    CmdMsgDump(" is_send_msg_to_main_msg_q              = %d  ", pCmdData->is_send_msg_to_main_msg_q, 0U, 0U);
    CmdMsgDump(" is_check_timestamp                     = %d  ", pCmdData->is_check_timestamp, 0U, 0U);
    CmdMsgDump(" cap_slice_num                          = %d  ", pCmdData->cap_slice_num, 0U, 0U);
    CmdMsgDump(" is_slice_cfg_present                   = %d  ", pCmdData->is_slice_cfg_present, 0U, 0U);
    CmdMsgDump(" is_vin_cap_presend                     = %d  ", pCmdData->is_vin_cap_presend, 0U, 0U);
    CmdMsgDump(" no_reset_fp_shared_res                 = %d  ", pCmdData->no_reset_fp_shared_res, 0U, 0U);
    CmdMsgDump(" fov_num                                = %d  ", pCmdData->fov_num, 0U, 0U);
    CmdMsgDump(" skip_frm_cnt                           = %d  ", pCmdData->skip_frm_cnt, 0U, 0U);
    CmdMsgDump(" output_dest                            = %d  ", pCmdData->output_dest, 0U, 0U);
    CmdMsgDump(" input_source                           = %d  ", pCmdData->input_source, 0U, 0U);
    CmdMsgDump(" vin_cap_width                          = %d  ", pCmdData->vin_cap_width, 0U, 0U);
    CmdMsgDump(" vin_cap_height                         = %d  ", pCmdData->vin_cap_height, 0U, 0U);
    CmdMsgDump(" blk_sz                                 = %d  ", pCmdData->blk_sz, 0U, 0U);
    CmdMsgDump(" mantissa                               = %d  ", pCmdData->mantissa, 0U, 0U);
    CmdMsgDump(" vin_ce_out_reset                       = %d  ", pCmdData->vin_ce_out_reset, 0U, 0U);
    CmdMsgDump(" batch_cmd_set_info.addr                = 0x%X", pCmdData->batch_cmd_set_info.addr, 0U, 0U);
    CmdMsgDump(" batch_cmd_set_info.id                  = 0x%X", pCmdData->batch_cmd_set_info.id, 0U, 0U);
    LL_PrintBatchInfoId(pCmdData->batch_cmd_set_info.id);
    CmdMsgDump(" batch_cmd_set_info.size                = %d  ", pCmdData->batch_cmd_set_info.size, 0U, 0U);
    CmdMsgDump(" vin_poll_intvl                         = %d  ", pCmdData->vin_poll_intvl, 0U, 0U);
    CmdMsgDump(" allow_raw_cap_skip                     = %d  ", pCmdData->allow_raw_cap_skip, 0U, 0U);
    CmdMsgDump(" default_raw_image_address              = 0x%X", pCmdData->default_raw_image_address, 0U, 0U);
    CmdMsgDump(" default_raw_image_pitch                = %d  ", pCmdData->default_raw_image_pitch, 0U, 0U);
    CmdMsgDump(" default_ce_image_address               = 0x%X", pCmdData->default_ce_image_address, 0U, 0U);
    CmdMsgDump(" default_ce_image_pitch                 = %d  ", pCmdData->default_ce_image_pitch, 0U, 0U);
    CmdMsgDump(" capture_compl_cntl_msg_addr            = 0x%X", pCmdData->capture_compl_cntl_msg_addr, 0U, 0U);
    CmdMsgDump(" capture_time_out_msec                  = %d  ", pCmdData->capture_time_out_msec, 0U, 0U);
    CmdMsgDump(" compl_cntl_msg_update_time_msec        = %d  ", pCmdData->compl_cntl_msg_update_time_msec, 0U, 0U);
#ifdef SUPPORT_DSP_VIN_SHRTFRM_DROP
    CmdMsgDump(" short_frame_drop_thresh_ticks          = %d  ", pCmdData->short_frame_drop_thresh_ticks, 0U, 0U);
#endif
    if (pCmdData->fov_cfg_tbl_daddr != 0U) {
        UINT8 i;
        UINT32 j;
        ULONG ULAddr;
        const vin_fov_cfg_t *FovCfg;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->fov_cfg_tbl_daddr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&FovCfg, &ULAddr);

            for (i = 0; i < pCmdData->fov_num; i++) {
                CmdMsgDump("   ========== FOV %02d ==========", i, 0U, 0U);
                CmdMsgDump("   chan_id                              = %d  ", FovCfg[i].chan_id, 0U, 0U);
                CmdMsgDump("   x_offset                             = %d  ", FovCfg[i].x_offset, 0U, 0U);
                CmdMsgDump("   y_offset                             = %d  ", FovCfg[i].y_offset, 0U, 0U);
                CmdMsgDump("   width                                = %d  ", FovCfg[i].width, 0U, 0U);
                CmdMsgDump("   height                               = %d  ", FovCfg[i].height, 0U, 0U);
                CmdMsgDump("   xpitch                               = %d  ", FovCfg[i].xpitch, 0U, 0U);
                CmdMsgDump("   hdr_intlac_mode                      = %d  ", FovCfg[i].hdr_intlac_mode, 0U, 0U);
                CmdMsgDump("   is_fov_active                        = %d  ", FovCfg[i].is_fov_active, 0U, 0U);
                CmdMsgDump("   is_last_fov                          = %d  ", FovCfg[i].is_last_fov, 0U, 0U);
                CmdMsgDump("   batch_cmd_set_info.addr              = 0x%X", FovCfg[i].batch_cmd_set_info.addr, 0U, 0U);
                CmdMsgDump("   batch_cmd_set_info.id                = 0x%X", FovCfg[i].batch_cmd_set_info.id, 0U, 0U);
                LL_PrintBatchInfoId(FovCfg[i].batch_cmd_set_info.id);
                CmdMsgDump("   batch_cmd_set_info.size              = %d  ", FovCfg[i].batch_cmd_set_info.size, 0U, 0U);
                for (j = 0; j < pCmdData->cap_slice_num; j++) {
                    CmdMsgDump("   slice_term_cap_line[%d]               = %u  ", j, FovCfg[i].slice_term_cap_line[j], 0U);
                }
            }
        }
    } else {
        CmdMsgDump(" fov_cfg_tbl_daddr                      = 0x%X", pCmdData->fov_cfg_tbl_daddr, 0U, 0U);
    }
}

void dump_cmd_vin_idle_t(const cmd_vin_idle_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_IDLE", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("vin_id   = %d  ", pCmdData->vin_id, 0U, 0U);
}

void dump_cmd_vin_set_raw_frm_cap_cnt_t(const cmd_vin_set_raw_frm_cap_cnt_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SET_RAW_FRM_CAP_CNT", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id     = %d", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" cap_seq_no = %d", pCmdData->cap_seq_no, 0U, 0U);
}

void dump_cmd_vin_set_ext_mem_t(const cmd_vin_set_ext_mem_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SET_EXT_MEM", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                     = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" allocation_mode            = %d  ", pCmdData->allocation_mode, 0U, 0U);
    CmdMsgDump(" overflow_cntl              = %d  ", pCmdData->overflow_cntl, 0U, 0U);
    CmdMsgDump(" chroma_format              = %d  ", pCmdData->chroma_format, 0U, 0U);
    CmdMsgDump(" vin_yuv_enc_fbp_disable    = %d  ", pCmdData->vin_yuv_enc_fbp_disable, 0U, 0U);
    CmdMsgDump(" memory_type                = %d  ", pCmdData->memory_type, 0U, 0U);
    CmdMsgDump(" allocation_type            = %d  ", pCmdData->allocation_type, 0U, 0U);
    CmdMsgDump(" num_frm_buf                = %d  ", pCmdData->num_frm_buf, 0U, 0U);
    CmdMsgDump(" buf_pitch                  = %d  ", pCmdData->buf_pitch, 0U, 0U);
    CmdMsgDump(" buf_width                  = %d  ", pCmdData->buf_width, 0U, 0U);
    CmdMsgDump(" buf_height                 = %d  ", pCmdData->buf_height, 0U, 0U);
    CmdMsgDump(" luma_img_ofs_x             = %d  ", pCmdData->luma_img_ofs_x, 0U, 0U);
    CmdMsgDump(" luma_img_ofs_y             = %d  ", pCmdData->luma_img_ofs_y, 0U, 0U);
    CmdMsgDump(" chroma_img_ofs_x           = %d  ", pCmdData->chroma_img_ofs_x, 0U, 0U);
    CmdMsgDump(" chroma_img_ofs_y           = %d  ", pCmdData->chroma_img_ofs_y, 0U, 0U);
    CmdMsgDump(" img_width                  = %d  ", pCmdData->img_width, 0U, 0U);
    CmdMsgDump(" img_height                 = %d  ", pCmdData->img_height, 0U, 0U);
    CmdMsgDump(" aux_pitch                  = %d  ", pCmdData->aux_pitch, 0U, 0U);
    CmdMsgDump(" aux_width                  = %d  ", pCmdData->aux_width, 0U, 0U);
    CmdMsgDump(" aux_height                 = %d  ", pCmdData->aux_height, 0U, 0U);
    CmdMsgDump(" aux_img_ofs_x              = %d  ", pCmdData->aux_img_ofs_x, 0U, 0U);
    CmdMsgDump(" aux_img_ofs_y              = %d  ", pCmdData->aux_img_ofs_y, 0U, 0U);
    CmdMsgDump(" aux_img_width              = %d  ", pCmdData->aux_img_width, 0U, 0U);
    CmdMsgDump(" aux_img_height             = %d  ", pCmdData->aux_img_height, 0U, 0U);
    if (pCmdData->allocation_type == 0U/*VIN_EXT_MEM_ALLOC_TYPE_DISTINCT*/) {
        ULONG ULAddr;
        const UINT32 *pU32Addr;
        UINT16 i = 0U;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->buf_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pU32Addr, &ULAddr);

            CmdMsgDump(" ====== buf_tbl[0x%X] ======", pCmdData->buf_addr, 0U, 0U);
            for (i = 0U; i<pCmdData->num_frm_buf; i++) {
                CmdMsgDump(" buf_addr[%d]         = 0x%X", i, pU32Addr[i], 0U);
            }
        }
    } else {
        CmdMsgDump(" buf_addr            = 0x%X", pCmdData->buf_addr, 0U, 0U);
    }
}

void dump_cmd_vin_send_input_data_t(const cmd_vin_send_input_data_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SEND_INPUT_DATA", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                  = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" is_compression_en       = %d  ", pCmdData->is_compression_en, 0U, 0U);
    CmdMsgDump(" blk_sz                  = %d  ", pCmdData->blk_sz, 0U, 0U);
    CmdMsgDump(" mantissa                = %d  ", pCmdData->mantissa, 0U, 0U);
    CmdMsgDump(" chan_id                 = %d  ", pCmdData->chan_id, 0U, 0U);
    CmdMsgDump(" send_data_dest          = %d  ", pCmdData->send_data_dest, 0U, 0U);
    CmdMsgDump(" encode_start_idc        = %d  ", pCmdData->encode_start_idc, 0U, 0U);
    CmdMsgDump(" encode_stop_idc         = %d  ", pCmdData->encode_stop_idc, 0U, 0U);
    CmdMsgDump(" raw_frm_cap_cnt         = %d  ", pCmdData->raw_frm_cap_cnt, 0U, 0U);
    CmdMsgDump(" input_data_type         = %d  ", pCmdData->input_data_type, 0U, 0U);
    CmdMsgDump(" ext_fb_idx              = 0x%X", pCmdData->ext_fb_idx, 0U, 0U);
    CmdMsgDump(" ext_ce_fb_idx           = 0x%X", pCmdData->ext_ce_fb_idx, 0U, 0U);
    CmdMsgDump(" batch_cmd_set_info.addr = 0x%X", pCmdData->batch_cmd_set_info.addr, 0U, 0U);
    CmdMsgDump(" batch_cmd_set_info.id   = 0x%X", pCmdData->batch_cmd_set_info.id, 0U, 0U);
    LL_PrintBatchInfoId(pCmdData->batch_cmd_set_info.id);
    CmdMsgDump(" batch_cmd_set_info.size = %d  ", pCmdData->batch_cmd_set_info.size, 0U, 0U);
    CmdMsgDump(" vproc_hflip_control     = %d  ", pCmdData->vproc_hflip_control, 0U, 0U);
    CmdMsgDump(" vproc_vflip_control     = %d  ", pCmdData->vproc_vflip_control, 0U, 0U);
    CmdMsgDump(" vproc_rotation_control  = %d  ", pCmdData->vproc_rotation_control, 0U, 0U);
    CmdMsgDump(" vout_hflip_control      = %d  ", pCmdData->vout_hflip_control, 0U, 0U);
    CmdMsgDump(" vout_vflip_control      = %d  ", pCmdData->vout_vflip_control, 0U, 0U);
    CmdMsgDump(" vout_rotation_control   = %d  ", pCmdData->vout_rotation_control, 0U, 0U);
    CmdMsgDump(" hw_pts                  = %d  ", pCmdData->hw_pts, 0U, 0U);
}

void dump_cmd_vin_initiate_raw_cap_to_ext_buf_t(const cmd_vin_initiate_raw_cap_to_ext_buf_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" raw_compression_type        = %d  ", pCmdData->raw_compression_type, 0U, 0U);
    CmdMsgDump(" input_data_type             = %d  ", pCmdData->input_data_type, 0U, 0U);
    CmdMsgDump(" stop_condition              = %d  ", pCmdData->stop_condition, 0U, 0U);
    CmdMsgDump(" raw_cap_done_sw_ctrl        = %d  ", pCmdData->raw_cap_done_sw_ctrl, 0U, 0U);
    CmdMsgDump(" raw_buffer_proc_ctrl        = %d  ", pCmdData->raw_buffer_proc_ctrl, 0U, 0U);
    CmdMsgDump(" raw_cap_sync_event          = %d  ", pCmdData->raw_cap_sync_event, 0U, 0U);
}

void dump_cmd_vin_attach_event_to_raw_t(const cmd_vin_attach_event_to_raw_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_ATTACH_EVENT_TO_RAW", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                  = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" ch_id                   = %d  ", pCmdData->ch_id, 0U, 0U);
    CmdMsgDump(" is_indep_fov_control    = %d  ", pCmdData->is_indep_fov_control, 0U, 0U);
    CmdMsgDump(" event_mask              = 0x%X", pCmdData->event_mask, 0U, 0U);
    CmdMsgDump(" repeat_cnt              = 0x%X", pCmdData->repeat_cnt, 0U, 0U);
    CmdMsgDump(" time_duration_bet_rpts  = %d  ", pCmdData->time_duration_bet_rpts, 0U, 0U);
    CmdMsgDump(" fov_mask_valid          = 0x%X", pCmdData->fov_mask_valid, 0U, 0U);
}

void dump_cmd_vin_cmd_msg_dec_rate_t(const cmd_vin_cmd_msg_dec_rate_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_CMD_MSG_DECIMATION_RATE", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id          = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" decimation_rate = 0x%X", pCmdData->decimation_rate, 0U, 0U);
}

void dump_cmd_vin_ce_setup_t(const cmd_vin_ce_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_CE_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id             = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" number_ce_exp      = %d  ", pCmdData->number_ce_exp, 0U, 0U);
    CmdMsgDump(" primary_out_exp    = %d  ", pCmdData->primary_out_exp, 0U, 0U);
    CmdMsgDump(" secondary_out_exp  = %d  ", pCmdData->secondary_out_exp, 0U, 0U);
    CmdMsgDump(" ce_width           = %d  ", pCmdData->ce_width, 0U, 0U);
    CmdMsgDump(" ce_height          = %d  ", pCmdData->ce_height, 0U, 0U);
    CmdMsgDump(" ce_out_format      = %d  ", pCmdData->ce_out_format, 0U, 0U);
}

void dump_cmd_vin_hdr_setup_t(const cmd_vin_hdr_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_HDR_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                     = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" num_exp_minus_1            = %d  ", pCmdData->num_exp_minus_1, 0U, 0U);
    CmdMsgDump(" num_hdr_blends             = %d  ", pCmdData->num_hdr_blends, 0U, 0U);
    CmdMsgDump(" aggregate_raw_frame_width  = %d  ", pCmdData->aggregate_raw_frame_width, 0U, 0U);
    CmdMsgDump(" aggregate_raw_frame_height = %d  ", pCmdData->aggregate_raw_frame_height, 0U, 0U);
    CmdMsgDump(" source_width               = %d  ", pCmdData->source_width, 0U, 0U);
    CmdMsgDump(" source_height              = %d  ", pCmdData->source_height, 0U, 0U);
    CmdMsgDump(" bayer_pattern              = %d  ", pCmdData->bayer_pattern, 0U, 0U);
    CmdMsgDump(" exp_vert_offset_table[0]   = %d  ", pCmdData->exp_vert_offset_table[0], 0U, 0U);
    CmdMsgDump(" exp_vert_offset_table[1]   = %d  ", pCmdData->exp_vert_offset_table[1], 0U, 0U);
    CmdMsgDump(" exp_vert_offset_table[2]   = %d  ", pCmdData->exp_vert_offset_table[2], 0U, 0U);
}

void dump_cmd_vin_set_frm_lvl_flip_rot_control_t(const cmd_vin_set_frm_lvl_flip_rot_control_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" fov_id                      = %d  ", pCmdData->fov_id, 0U, 0U);
    CmdMsgDump(" vproc_hflip_control         = %d  ", pCmdData->vproc_hflip_control, 0U, 0U);
    CmdMsgDump(" vproc_vflip_control         = %d  ", pCmdData->vproc_vflip_control, 0U, 0U);
    CmdMsgDump(" vproc_rotation_control      = %d  ", pCmdData->vproc_rotation_control, 0U, 0U);
    CmdMsgDump(" vout_hflip_control          = %d  ", pCmdData->vout_hflip_control, 0U, 0U);
    CmdMsgDump(" vout_vflip_control          = %d  ", pCmdData->vout_vflip_control, 0U, 0U);
    CmdMsgDump(" vout_rotation_control       = %d  ", pCmdData->vout_rotation_control, 0U, 0U);
}

void dump_cmd_vin_set_fov_layout_t(const cmd_vin_set_fov_layout_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SET_FOV_LAYOUT", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" num_of_fovs                 = %d  ", pCmdData->num_of_fovs, 0U, 0U);
    if (pCmdData->fov_lay_out_ptr != 0U) {
        UINT32 i;
        ULONG ULAddr;
        const fov_layout_t *FovLayout;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->fov_lay_out_ptr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&FovLayout, &ULAddr);

            for (i = 0; i<pCmdData->num_of_fovs; i++) {
                CmdMsgDump("   ========== FOV %02d ==========", i, 0U, 0U);
                CmdMsgDump("   chan_id                   = %d  ", FovLayout[i].chan_id, 0U, 0U);
                CmdMsgDump("   img_x_ofset               = %d  ", FovLayout[i].img_x_ofset, 0U, 0U);
                CmdMsgDump("   img_y_ofset               = %d  ", FovLayout[i].img_y_ofset, 0U, 0U);
                CmdMsgDump("   img_width                 = %d  ", FovLayout[i].img_width, 0U, 0U);
                CmdMsgDump("   img_height                = %d  ", FovLayout[i].img_height, 0U, 0U);
                CmdMsgDump("   xpitch                    = %d  ", FovLayout[i].xpitch, 0U, 0U);
                CmdMsgDump("   hdr_intlac_mode           = %d  ", FovLayout[i].hdr_intlac_mode, 0U, 0U);
                CmdMsgDump("   is_fov_active             = %d  ", FovLayout[i].is_fov_active, 0U, 0U);
                CmdMsgDump("   is_last_fov               = %d  ", FovLayout[i].is_last_fov, 0U, 0U);
            }
        }
    } else {
        CmdMsgDump(" fov_lay_out_ptr             = 0x%X", pCmdData->fov_lay_out_ptr, 0U, 0U);
    }
}

void dump_cmd_vin_set_frm_vproc_delay_t(const cmd_vin_set_frm_vproc_delay_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_SET_FRM_VPROC_DELAY", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" delay_time_msec             = %d  ", pCmdData->delay_time_msec, 0U, 0U);
}

void dump_cmd_vin_attach_proc_cfg_to_cap_frm_t(const cmd_vin_attach_proc_cfg_to_cap_frm_t *pCmdData, const CmdInfo_t *Info)
{
    UINT32 i;
    const vin_fov_batch_cmd_set_t *pFovBatchSet;
    ULONG ULAddr;
    UINT32 Rval;

    CmdWriteDump("CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                           = %d", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" fov_num                          = %d", pCmdData->fov_num, 0U, 0U);

    Rval = dsp_osal_cli2virt(pCmdData->fov_batch_cmd_set_addr, &ULAddr);
    if (Rval == OK) {
        dsp_osal_typecast(&pFovBatchSet, &ULAddr);
        for (i = 0; i < pCmdData->fov_num; i++) {
            CmdMsgDump("   ========== FOV %02d ==========", i, 0U, 0U);
            CmdMsgDump("   vin_id                         = %d  ", pFovBatchSet[i].vin_id, 0U, 0U);
            CmdMsgDump("   chan_id                        = %d  ", pFovBatchSet[i].chan_id, 0U, 0U);
            CmdMsgDump("   batch_cmd_set_info.addr        = 0x%X", pFovBatchSet[i].batch_cmd_set_info.addr, 0U, 0U);
            CmdMsgDump("   batch_cmd_set_info.id          = 0x%X", pFovBatchSet[i].batch_cmd_set_info.id, 0U, 0U);
            LL_PrintBatchInfoId(pFovBatchSet[i].batch_cmd_set_info.id);
            CmdMsgDump("   batch_cmd_set_info.size        = %d  ", pFovBatchSet[i].batch_cmd_set_info.size, 0U, 0U);
        }
    }
}

void dump_cmd_vin_attach_sideband_info_to_cap_frm_t(const cmd_vin_attach_sideband_info_to_cap_frm_t *pCmdData, const CmdInfo_t *Info)
{
    UINT32 i;

    CmdWriteDump("CMD_VIN_ATTACH_SIDEBAND_INFO_TO_CAP_FRM", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" sideband_info_num           = %d  ", pCmdData->sideband_info_num, 0U, 0U);

    for (i = 0; i< pCmdData->sideband_info_num; i++) {
        if (pCmdData->per_fov_sideband_info_addr[i] != 0U) {
            ULONG ULAddr;
            const sideband_info_t *pSideBand;
            UINT32 Rval;

            Rval = dsp_osal_cli2virt(pCmdData->per_fov_sideband_info_addr[i], &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pSideBand, &ULAddr);

                CmdMsgDump("   ========== SideBand %02d ==========", i, 0U, 0U);
                CmdMsgDump("   fov_id                    = %d  ", pSideBand->fov_id, 0U, 0U);
                CmdMsgDump("   fov_private_info_addr     = 0x%X", pSideBand->fov_private_info_addr, 0U, 0U);
            }
        }
    }
}

void dump_cmd_vin_vcap_dram_in_setup_t(const cmd_vin_vcap_dram_in_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_VCAP_DRAM_IN_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" channel_id                  = %d  ", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" cap_iso_mode                = %d  ", pCmdData->cap_iso_mode, 0U, 0U);
    CmdMsgDump(" vcap_is_frm                 = %d  ", pCmdData->vcap_is_frm, 0U, 0U);
    CmdMsgDump(" is_raw_compressed           = %d  ", pCmdData->is_raw_compressed, 0U, 0U);
    CmdMsgDump(" vin_simu_vproc_dram_in_fmt  = %d  ", pCmdData->vin_simu_vproc_dram_in_fmt, 0U, 0U);
    CmdMsgDump(" vcap_width                  = %d  ", pCmdData->vcap_width, 0U, 0U);
    CmdMsgDump(" vcap_height                 = %d  ", pCmdData->vcap_height, 0U, 0U);
    CmdMsgDump(" vcap_hdec_width             = %d  ", pCmdData->vcap_hdec_width, 0U, 0U);
    CmdMsgDump(" ik_cfg_addr                 = %d  ", pCmdData->ik_cfg_addr, 0U, 0U);
    CmdMsgDump(" ik_cfg_id                   = %d  ", pCmdData->ik_cfg_id, 0U, 0U);
}

void dump_cmd_vin_vout_lock_setup_t(const cmd_vin_vout_lock_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_VOUT_LOCK_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" en_lock                     = %d  ", pCmdData->en_lock, 0U, 0U);
    CmdMsgDump(" reset_lock_sync             = %d  ", pCmdData->reset_lock_sync, 0U, 0U);
}

void dump_cmd_vin_attach_metadata_t(const cmd_vin_attach_metadata_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VIN_ATTACH_METADATA", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" fov_id                      = %d  ", pCmdData->fov_id, 0U, 0U);
    CmdMsgDump(" is_metadata_attach          = %d  ", pCmdData->is_metadata_attach, 0U, 0U);
    CmdMsgDump(" metadata_addr               = 0X%X  ", pCmdData->metadata_addr, 0U, 0U);
}

void dump_cmd_vin_temporal_demux_setup_t(const cmd_vin_temporal_demux_setup_t *pCmdData, const CmdInfo_t *Info)
{
    UINT16 i;

    CmdWriteDump("CMD_VIN_TEMPORAL_DEMUX_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_id                      = %d  ", pCmdData->vin_id, 0U, 0U);
    CmdMsgDump(" temporal_demux_num          = %d  ", pCmdData->temporal_demux_num, 0U, 0U);
    CmdMsgDump(" en_temporal_demux           = %d  ", pCmdData->en_temporal_demux, 0U, 0U);
    for (i = 0U; i < MAX_VIN_TEMPORAL_DEMUX_NUM; i++) {
        CmdMsgDump(" temporal_demux_tar[%d]       = 0x%X", i, pCmdData->temporal_demux_tar[i], 0U);
    }
}

void dump_cmd_vout_mixer_setup_t(const cmd_vout_mixer_setup_t *pCmdData, const CmdInfo_t *Info)
{
    UINT16 i;

    CmdWriteDump("CMD_VOUT_MIXER_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id          = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" interlaced       = %d  ", pCmdData->interlaced, 0U, 0U);
    CmdMsgDump(" frm_rate         = %d  ", pCmdData->frm_rate, 0U, 0U);
    CmdMsgDump(" act_win_width    = %d  ", pCmdData->act_win_width, 0U, 0U);
    CmdMsgDump(" act_win_height   = %d  ", pCmdData->act_win_height, 0U, 0U);
    CmdMsgDump(" back_ground_v    = 0x%X", pCmdData->back_ground_v, 0U, 0U);
    CmdMsgDump(" back_ground_u    = 0x%X", pCmdData->back_ground_u, 0U, 0U);
    CmdMsgDump(" back_ground_y    = 0x%X", pCmdData->back_ground_y, 0U, 0U);
    CmdMsgDump(" mixer_444        = %d  ", pCmdData->mixer_444, 0U, 0U);
    CmdMsgDump(" highlight_v      = 0x%X", pCmdData->highlight_v, 0U, 0U);
    CmdMsgDump(" highlight_u      = 0x%X", pCmdData->highlight_u, 0U, 0U);
    CmdMsgDump(" highlight_y      = 0x%X", pCmdData->highlight_y, 0U, 0U);
    CmdMsgDump(" highlight_thresh = 0x%X", pCmdData->highlight_thresh, 0U, 0U);
    CmdMsgDump(" reverse_en       = %d  ", pCmdData->reverse_en, 0U, 0U);
    CmdMsgDump(" csc_en           = %d  ", pCmdData->csc_en, 0U, 0U);
    for (i = 0U; i < DSP_VOUT_CSC_MATRIX_DEPTH; i++) {
        CmdMsgDump(" csc_parms[%d]     = 0x%X", i, pCmdData->csc_parms[i], 0U);
    }
}

void dump_cmd_vout_video_setup_t(const cmd_vout_video_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_VIDEO_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id                        = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" en                             = %d  ", pCmdData->en, 0U, 0U);
    CmdMsgDump(" src                            = %d  ", pCmdData->src, 0U, 0U);
    CmdMsgDump(" flip                           = %d  ", pCmdData->flip, 0U, 0U);
    CmdMsgDump(" rotate                         = %d  ", pCmdData->rotate, 0U, 0U);
    CmdMsgDump(" rotate_flip_mode               = %d  ", pCmdData->rotate_flip_mode, 0U, 0U);
    CmdMsgDump(" win_offset_x                   = %d  ", pCmdData->win_offset_x, 0U, 0U);
    CmdMsgDump(" win_offset_y                   = %d  ", pCmdData->win_offset_y, 0U, 0U);
    CmdMsgDump(" win_width                      = %d  ", pCmdData->win_width, 0U, 0U);
    CmdMsgDump(" win_height                     = %d  ", pCmdData->win_height, 0U, 0U);
    CmdMsgDump(" default_img_y_addr             = 0x%X", pCmdData->default_img_y_addr, 0U, 0U);
    CmdMsgDump(" default_img_uv_addr            = 0x%X", pCmdData->default_img_uv_addr, 0U, 0U);
    CmdMsgDump(" default_img_pitch              = %d  ", pCmdData->default_img_pitch, 0U, 0U);
    CmdMsgDump(" default_img_repeat_field       = %d  ", pCmdData->default_img_repeat_field, 0U, 0U);
    CmdMsgDump(" default_img_ignore_rotate_flip = %d  ", pCmdData->default_img_ignore_rotate_flip, 0U, 0U);
    CmdMsgDump(" default_img_ch_fmt             = %d  ", pCmdData->default_img_ch_fmt, 0U, 0U);
}

void dump_cmd_vout_default_img_setup_t(const cmd_vout_default_img_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_DEFAULT_IMG_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id                  = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" default_img_y_addr       = 0x%X", pCmdData->default_img_y_addr, 0U, 0U);
    CmdMsgDump(" default_img_uv_addr      = 0x%X", pCmdData->default_img_uv_addr, 0U, 0U);
    CmdMsgDump(" default_img_pitch        = %d  ", pCmdData->default_img_pitch, 0U, 0U);
    CmdMsgDump(" default_img_repeat_field = %d  ", pCmdData->default_img_repeat_field, 0U, 0U);
}

void dump_cmd_vout_osd_setup_t(const cmd_vout_osd_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_OSD_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id                  = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" en                       = %d  ", pCmdData->en, 0U, 0U);
    CmdMsgDump(" src                      = %d  ", pCmdData->src, 0U, 0U);
    CmdMsgDump(" flip                     = %d  ", pCmdData->flip, 0U, 0U);
    CmdMsgDump(" rescaler_en              = %d  ", pCmdData->rescaler_en, 0U, 0U);
    CmdMsgDump(" premultiplied            = %d  ", pCmdData->premultiplied, 0U, 0U);
    CmdMsgDump(" global_blend             = %d  ", pCmdData->global_blend, 0U, 0U);
    CmdMsgDump(" win_offset_x             = %d  ", pCmdData->win_offset_x, 0U, 0U);
    CmdMsgDump(" win_offset_y             = %d  ", pCmdData->win_offset_y, 0U, 0U);
    CmdMsgDump(" win_width                = %d  ", pCmdData->win_width, 0U, 0U);
    CmdMsgDump(" win_height               = %d  ", pCmdData->win_height, 0U, 0U);
    CmdMsgDump(" rescaler_input_width     = %d  ", pCmdData->rescaler_input_width, 0U, 0U);
    CmdMsgDump(" rescaler_input_height    = %d  ", pCmdData->rescaler_input_height, 0U, 0U);
    CmdMsgDump(" osd_buf_dram_addr        = 0x%X", pCmdData->osd_buf_dram_addr, 0U, 0U);
    CmdMsgDump(" osd_buf_pitch            = %d  ", pCmdData->osd_buf_pitch, 0U, 0U);
    CmdMsgDump(" osd_buf_repeat_field     = %d  ", pCmdData->osd_buf_repeat_field, 0U, 0U);
    CmdMsgDump(" osd_direct_mode          = %d  ", pCmdData->osd_direct_mode, 0U, 0U);
    CmdMsgDump(" osd_transparent_color    = 0x%X", pCmdData->osd_transparent_color, 0U, 0U);
    CmdMsgDump(" osd_transparent_color_en = %d  ", pCmdData->osd_transparent_color_en, 0U, 0U);
    CmdMsgDump(" osd_swap_bytes           = 0x%X", pCmdData->osd_swap_bytes, 0U, 0U);
    CmdMsgDump(" osd_buf_info_dram_addr   = 0x%X", pCmdData->osd_buf_info_dram_addr, 0U, 0U);
    if (pCmdData->osd_buf_info_dram_addr != 0U) {
        ULONG ULAddr;
        const AMBA_DSP_OSD_BUF_INFO_s *pOsdBufferInfo;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->osd_buf_info_dram_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pOsdBufferInfo, &ULAddr);

            CmdMsgDump(" OsdBufDramAddr           = 0x%X",   pOsdBufferInfo->OsdBufDramAddr, 0U, 0U);
            CmdMsgDump(" OsdBufPitch              = %d  ",   pOsdBufferInfo->OsdBufPitch, 0U, 0U);
            CmdMsgDump(" OsdBufRepeatField        = %d  ",   pOsdBufferInfo->OsdBufRepeatField, 0U, 0U);
        }
    }
}

void dump_cmd_vout_osd_buf_setup_t(const cmd_vout_osd_buf_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_OSD_BUFFER_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id              = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" osd_buf_dram_addr    = 0x%X", pCmdData->osd_buf_dram_addr, 0U, 0U);
    CmdMsgDump(" osd_buf_pitch        = %d  ", pCmdData->osd_buf_pitch, 0U, 0U);
    CmdMsgDump(" osd_buf_repeat_field = %d  ", pCmdData->osd_buf_repeat_field, 0U, 0U);
}

void dump_cmd_vout_osd_clut_setup_t(const cmd_vout_osd_clut_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_OSD_CLUT_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id        = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" clut_dram_addr = 0x%X", pCmdData->clut_dram_addr, 0U, 0U);
}

void dump_cmd_vout_display_setup_t(const cmd_vout_display_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_DISPLAY_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id                      = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" dual_vout_vysnc_delay_ms_x10 = %d  ", pCmdData->dual_vout_vysnc_delay_ms_x10, 0U, 0U);
    CmdMsgDump(" disp_config_dram_addr        = 0x%X", pCmdData->disp_config_dram_addr, 0U, 0U);
    CmdMsgDump(" vin_vout_vsync_delay_ms_x10  = %d  ", pCmdData->vin_vout_vsync_delay_ms_x10, 0U, 0U);
    CmdMsgDump(" vout_sync_with_vin           = %d  ", pCmdData->vout_sync_with_vin, 0U, 0U);
    CmdMsgDump(" vin_vout_vsync_timeout_ms    = %d  ", pCmdData->vin_vout_vsync_timeout_ms, 0U, 0U);
    CmdMsgDump(" reverse_polarity             = %d  ", pCmdData->reverse_polarity, 0U, 0U);
    CmdMsgDump(" enable_frame_done_timeout    = %d  ", pCmdData->enable_frame_done_timeout, 0U, 0U);
}

void dump_cmd_vout_dve_setup_t(const cmd_vout_dve_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_DVE_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id              = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" dve_config_dram_addr = 0x%X", pCmdData->dve_config_dram_addr, 0U, 0U);
}

void dump_cmd_vout_reset_t(const cmd_vout_reset_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_RESET", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id     = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" reset_mixer = %d  ", pCmdData->reset_mixer, 0U, 0U);
    CmdMsgDump(" reset_disp  = %d  ", pCmdData->reset_disp, 0U, 0U);
}

void dump_cmd_vout_display_csc_setup_t(const cmd_vout_display_csc_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_DISPLAY_CSC_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id      = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" csc_type     = %d  ", pCmdData->csc_type, 0U, 0U);
    CmdMsgDump(" csc_parms[0] = 0x%X", pCmdData->csc_parms[0], 0U, 0U);
    CmdMsgDump(" csc_parms[1] = 0x%X", pCmdData->csc_parms[1], 0U, 0U);
    CmdMsgDump(" csc_parms[2] = 0x%X", pCmdData->csc_parms[2], 0U, 0U);
    CmdMsgDump(" csc_parms[3] = 0x%X", pCmdData->csc_parms[3], 0U, 0U);
    CmdMsgDump(" csc_parms[4] = 0x%X", pCmdData->csc_parms[4], 0U, 0U);
    CmdMsgDump(" csc_parms[5] = 0x%X", pCmdData->csc_parms[5], 0U, 0U);
    CmdMsgDump(" csc_parms[6] = 0x%X", pCmdData->csc_parms[6], 0U, 0U);
    CmdMsgDump(" csc_parms[7] = 0x%X", pCmdData->csc_parms[7], 0U, 0U);
    CmdMsgDump(" csc_parms[8] = 0x%X", pCmdData->csc_parms[8], 0U, 0U);
}

void dump_cmd_vout_digital_output_mode_setup_t(const cmd_vout_digital_output_mode_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_DIGITAL_OUTPUT_MODE_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id     = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" output_mode = %d  ", pCmdData->output_mode, 0U, 0U);
}

void dump_cmd_vout_gamma_setup_t(const cmd_vout_gamma_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_VOUT_GAMMA_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id            = %d  ", pCmdData->vout_id, 0U, 0U);
    CmdMsgDump(" enable             = %d  ", pCmdData->enable, 0U, 0U);
    CmdMsgDump(" setup_gamma_table  = %d  ", pCmdData->setup_gamma_table, 0U, 0U);
    CmdMsgDump(" gamma_dram_addr    = 0x%X", pCmdData->gamma_dram_addr, 0U, 0U);
}

void dump_cmd_encoder_setup_t(const cmd_encoder_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_ENCODER_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" stream_id                   = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" stream_type                 = %d  ", pCmdData->stream_type, 0U, 0U);
    CmdMsgDump(" profile_idc                 = %d  ", pCmdData->profile_idc, 0U, 0U);
    CmdMsgDump(" level_idc                   = %d  ", pCmdData->level_idc, 0U, 0U);
    CmdMsgDump(" coding_type                 = %d  ", pCmdData->coding_type, 0U, 0U);
    CmdMsgDump(" scalelist_opt               = %d  ", pCmdData->scalelist_opt, 0U, 0U);
    CmdMsgDump(" force_intlc_tb_iframe       = %d  ", pCmdData->force_intlc_tb_iframe, 0U, 0U);
    CmdMsgDump(" enc_start_method            = %d  ", pCmdData->enc_start_method, 0U, 0U);
    CmdMsgDump(" enc_stop_method             = %d  ", pCmdData->enc_stop_method, 0U, 0U);
    CmdMsgDump(" hevc_perf_mode              = %d  ", pCmdData->hevc_perf_mode, 0U, 0U);
    CmdMsgDump(" embed_code                  = %d  ", pCmdData->embed_code, 0U, 0U);
    CmdMsgDump(" aff_mode                    = %d  ", pCmdData->aff_mode, 0U, 0U);
    CmdMsgDump(" chroma_format               = %d  ", pCmdData->chroma_format, 0U, 0U);
    CmdMsgDump(" max_num_ltrs                = %d  ", pCmdData->max_num_ltrs, 0U, 0U);
    CmdMsgDump(" enc_src                     = %d  ", pCmdData->enc_src, 0U, 0U);
    CmdMsgDump(" encode_w_sz                 = %d  ", pCmdData->encode_w_sz, 0U, 0U);
    CmdMsgDump(" encode_h_sz                 = %d  ", pCmdData->encode_h_sz, 0U, 0U);
    CmdMsgDump(" encode_w_ofs                = %d  ", pCmdData->encode_w_ofs, 0U, 0U);
    CmdMsgDump(" encode_h_ofs                = %d  ", pCmdData->encode_h_ofs, 0U, 0U);
    CmdMsgDump(" gop_structure               = %d  ", pCmdData->gop_structure, 0U, 0U);
    CmdMsgDump(" M                           = %d  ", pCmdData->M, 0U, 0U);
    CmdMsgDump(" N                           = %d  ", pCmdData->N, 0U, 0U);
    CmdMsgDump(" numRef_P                    = %d  ", pCmdData->numRef_P, 0U, 0U);
    CmdMsgDump(" numRef_B                    = %d  ", pCmdData->numRef_B, 0U, 0U);
    CmdMsgDump(" tile_num                    = %d  ", pCmdData->tile_num, 0U, 0U);
    CmdMsgDump(" beating_reduction           = %d  ", pCmdData->beating_reduction, 0U, 0U);
    CmdMsgDump(" efm_enable                  = %d  ", pCmdData->efm_enable, 0U, 0U);
    CmdMsgDump(" use_cabac                   = %d  ", pCmdData->use_cabac, 0U, 0U);
    CmdMsgDump(" quality_level               = 0x%X", pCmdData->quality_level, 0U, 0U);
    CmdMsgDump(" rc_mode                     = %d  ", pCmdData->rc_mode, 0U, 0U);
    CmdMsgDump(" initial_qp                  = %d  ", pCmdData->initial_qp, 0U, 0U);
    CmdMsgDump(" sps_pps_header_mode         = %d  ", pCmdData->sps_pps_header_mode, 0U, 0U);
    CmdMsgDump(" cpb_buf_idc                 = %d  ", pCmdData->cpb_buf_idc, 0U, 0U);
    CmdMsgDump(" en_panic_rc                 = %d  ", pCmdData->en_panic_rc, 0U, 0U);
    CmdMsgDump(" cpb_cmp_idc                 = %d  ", pCmdData->cpb_cmp_idc, 0U, 0U);
    CmdMsgDump(" fast_rc_idc                 = %d  ", pCmdData->fast_rc_idc, 0U, 0U);
    CmdMsgDump(" target_bitrate              = %d  ", pCmdData->target_bitrate, 0U, 0U);
    if ((pCmdData->rc_mode == ENC_RC_VBR) || (pCmdData->rc_mode == ENC_RC_SIMPLE_VBR)) {
        ULONG ULAddr;
        const vbr_config_t *VbrCfg;
        UINT32 Rval;

        Rval = dsp_osal_cli2virt(pCmdData->vbr_config, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&VbrCfg, &ULAddr);

            CmdMsgDump("   vbr_config.VBR_ness       = %d  ", VbrCfg->VBR_ness, 0U, 0U);
            CmdMsgDump("   vbr_config.VBR_complexity = %d  ", VbrCfg->VBR_complexity, 0U, 0U);
            CmdMsgDump("   vbr_config.VBR_min_rate   = %d  ", VbrCfg->VBR_min_rate, 0U, 0U);
            CmdMsgDump("   vbr_config.VBR_max_rate   = %d  ", VbrCfg->VBR_max_rate, 0U, 0U);
        }
    } else {
        CmdMsgDump(" vbr_config                  = %d  ", pCmdData->vbr_config, 0U, 0U);
    }
    CmdMsgDump(" vbr_init_data_daddr         = %d  ", pCmdData->vbr_init_data_daddr, 0U, 0U);
    CmdMsgDump(" bits_fifo_base              = 0x%X", pCmdData->bits_fifo_base, 0U, 0U);
    CmdMsgDump(" bits_fifo_size              = 0x%X", pCmdData->bits_fifo_size, 0U, 0U);
    CmdMsgDump(" info_fifo_base              = 0x%X", pCmdData->info_fifo_base, 0U, 0U);
    CmdMsgDump(" info_fifo_size              = 0x%X", pCmdData->info_fifo_size, 0U, 0U);
    CmdMsgDump(" enc_frame_rate              = 0x%X", pCmdData->enc_frame_rate, 0U, 0U);
    CmdMsgDump(" lambda_scalefactor          = %d  ", pCmdData->lambda_scalefactor, 0U, 0U);
    CmdMsgDump(" log2_num_ltrp_per_gop       = %d  ", pCmdData->log2_num_ltrp_per_gop, 0U, 0U);
    CmdMsgDump(" hflip                       = %d  ", pCmdData->hflip, 0U, 0U);
    CmdMsgDump(" vflip                       = %d  ", pCmdData->vflip, 0U, 0U);
    CmdMsgDump(" rotate                      = %d  ", pCmdData->rotate, 0U, 0U);
    CmdMsgDump(" fast_seek_interval          = %d  ", pCmdData->fast_seek_interval, 0U, 0U);
    CmdMsgDump(" custom_df_control_flag      = %d  ", pCmdData->custom_df_control_flag, 0U, 0U);
    CmdMsgDump(" disable_df_idc              = %d  ", pCmdData->disable_df_idc, 0U, 0U);
    CmdMsgDump(" df_param1                   = %d  ", pCmdData->df_param1, 0U, 0U);
    CmdMsgDump(" bits_fifo_offset            = %d  ", pCmdData->bits_fifo_offset, 0U, 0U);
    CmdMsgDump(" idr_interval                = %d  ", pCmdData->idr_interval, 0U, 0U);
    CmdMsgDump(" cpb_user_size               = %d  ", pCmdData->cpb_user_size, 0U, 0U);
    CmdMsgDump(" stat_fifo_base              = 0x%X", pCmdData->stat_fifo_base, 0U, 0U);
    CmdMsgDump(" stat_fifo_limit             = 0x%X", pCmdData->stat_fifo_limit, 0U, 0U);
    CmdMsgDump(" weight_scale_daddr          = 0x%X", pCmdData->weight_scale_daddr, 0U, 0U);
    CmdMsgDump(" IR_cycle                    = %d  ", pCmdData->IR_cycle, 0U, 0U);
    CmdMsgDump(" IR_type                     = %d  ", pCmdData->IR_type, 0U, 0U);
    CmdMsgDump(" IR_intra_qp_adj             = %d  ", pCmdData->IR_intra_qp_adj, 0U, 0U);
    CmdMsgDump(" IR_tuning_mode              = %d  ", pCmdData->IR_tuning_mode, 0U, 0U);
    CmdMsgDump(" IR_no_overlap               = %d  ", pCmdData->IR_no_overlap, 0U, 0U);
    CmdMsgDump(" IR_max_qp                   = %d  ", pCmdData->IR_max_qp, 0U, 0U);
    CmdMsgDump(" IR_max_qp_spread            = %d  ", pCmdData->IR_max_qp_spread, 0U, 0U);
    CmdMsgDump(" num_slices_per_bits_info    = %d  ", pCmdData->num_slices_per_bits_info, 0U, 0U);
    CmdMsgDump(" slice_num                   = %d  ", pCmdData->slice_num, 0U, 0U);
    CmdMsgDump(" gmv_enable                  = %d  ", pCmdData->gmv_enable, 0U, 0U);
    CmdMsgDump(" gmv_threshold_enable        = %d  ", pCmdData->gmv_threshold_enable, 0U, 0U);
    CmdMsgDump(" gmv_threshold               = %d  ", pCmdData->gmv_threshold, 0U, 0U);
    CmdMsgDump(" df_param2                   = %d  ", pCmdData->df_param2, 0U, 0U);
    CmdMsgDump(" two_lt_ref_mode             = %d  ", pCmdData->two_lt_ref_mode, 0U, 0U);
    CmdMsgDump(" aqp_lambda                  = %d  ", pCmdData->aqp_lambda, 0U, 0U);
    CmdMsgDump(" mvdump_daddr                = 0x%X", pCmdData->mvdump_daddr, 0U, 0U);
    CmdMsgDump(" mvdump_fifo_limit           = 0x%X", pCmdData->mvdump_fifo_limit, 0U, 0U);
    CmdMsgDump(" mvdump_fifo_unit_sz         = %d  ", pCmdData->mvdump_fifo_unit_sz, 0U, 0U);
    CmdMsgDump(" mvdump_dpitch               = %d  ", pCmdData->mvdump_dpitch, 0U, 0U);
    CmdMsgDump(" is_high_priority_stream     = %d  ", pCmdData->is_high_priority_stream, 0U, 0U);
    CmdMsgDump(" tier_idc                    = %d  ", pCmdData->tier_idc, 0U, 0U);
    CmdMsgDump(" recon_offset_y              = %d  ", pCmdData->recon_offset_y, 0U, 0U);
    CmdMsgDump(" is_recon_to_vout            = %d  ", pCmdData->is_recon_to_vout, 0U, 0U);
    CmdMsgDump(" disable_sao                 = %d  ", pCmdData->disable_sao, 0U, 0U);
    CmdMsgDump(" is_dirt_detect              = %d  ", pCmdData->is_dirt_detect, 0U, 0U);
    CmdMsgDump(" send_buf_status_msg         = %d  ", pCmdData->send_buf_status_msg, 0U, 0U);
    CmdMsgDump(" cs_is_lsb                   = %d  ", pCmdData->cs_is_lsb, 0U, 0U);
    CmdMsgDump(" cs_res_idx                  = %d  ", pCmdData->cs_res_idx, 0U, 0U);
    CmdMsgDump(" cs_cfg_dbase                = 0x%X", pCmdData->cs_cfg_dbase, 0U, 0U);
    CmdMsgDump(" rc_overlap_memd             = %d  ", pCmdData->rc_overlap_memd, 0U, 0U);
    CmdMsgDump(" rc_decimation               = %d  ", pCmdData->rc_decimation, 0U, 0U);
}

void dump_cmd_encoder_start_t(const cmd_encoder_start_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_ENCODER_START", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" stream_id                               = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" stream_type                             = %d  ", pCmdData->stream_type, 0U, 0U);
    CmdMsgDump(" gaps_in_frame_num_value_allowed_flag    = %d  ", pCmdData->gaps_in_frame_num_value_allowed_flag, 0U, 0U);
    CmdMsgDump(" force_annexb_long_start_code            = %d  ", pCmdData->force_annexb_long_start_code, 0U, 0U);
    CmdMsgDump(" au_type                                 = %d  ", pCmdData->au_type, 0U, 0U);
    CmdMsgDump(" append_user_data_sei                    = %d  ", pCmdData->append_user_data_sei, 0U, 0U);
    CmdMsgDump(" encode_duration                         = %d  ", pCmdData->encode_duration, 0U, 0U);
    CmdMsgDump(" start_encode_frame_no                   = %d  ", pCmdData->start_encode_frame_no, 0U, 0U);
    CmdMsgDump(" frame_crop.enable_flag                  = %d  ", pCmdData->frame_crop.enable_flag, 0U, 0U);
    CmdMsgDump(" frame_crop.offset_left                  = %d  ", pCmdData->frame_crop.offset_left, 0U, 0U);
    CmdMsgDump(" frame_crop.offset_right                 = %d  ", pCmdData->frame_crop.offset_right, 0U, 0U);
    CmdMsgDump(" frame_crop.offset_top                   = %d  ", pCmdData->frame_crop.offset_top, 0U, 0U);
    CmdMsgDump(" frame_crop.offset_bottom                = %d  ", pCmdData->frame_crop.offset_bottom, 0U, 0U);
    CmdMsgDump(" vui_enable                              = %d  ", pCmdData->h264_vui.vui_enable, 0U, 0U);
    CmdMsgDump(" aspect_ratio_info_present_flag          = %d  ", pCmdData->h264_vui.aspect_ratio_info_present_flag, 0U, 0U);
    CmdMsgDump(" overscan_info_present_flag              = %d  ", pCmdData->h264_vui.overscan_info_present_flag, 0U, 0U);
    CmdMsgDump(" overscan_appropriate_flag               = %d  ", pCmdData->h264_vui.overscan_appropriate_flag, 0U, 0U);
    CmdMsgDump(" video_signal_type_present_flag          = %d  ", pCmdData->h264_vui.video_signal_type_present_flag, 0U, 0U);
    CmdMsgDump(" video_full_range_flag                   = %d  ", pCmdData->h264_vui.video_full_range_flag, 0U, 0U);
    CmdMsgDump(" colour_description_present_flag         = %d  ", pCmdData->h264_vui.colour_description_present_flag, 0U, 0U);
    CmdMsgDump(" chroma_loc_info_present_flag            = %d  ", pCmdData->h264_vui.chroma_loc_info_present_flag, 0U, 0U);
    CmdMsgDump(" timing_info_present_flag                = %d  ", pCmdData->h264_vui.timing_info_present_flag, 0U, 0U);
    CmdMsgDump(" fixed_frame_rate_flag                   = %d  ", pCmdData->h264_vui.fixed_frame_rate_flag, 0U, 0U);
    CmdMsgDump(" nal_hrd_parameters_present_flag         = %d  ", pCmdData->h264_vui.nal_hrd_parameters_present_flag, 0U, 0U);
    CmdMsgDump(" vcl_hrd_parameters_present_flag         = %d  ", pCmdData->h264_vui.vcl_hrd_parameters_present_flag, 0U, 0U);
    CmdMsgDump(" low_delay_hrd_flag                      = %d  ", pCmdData->h264_vui.low_delay_hrd_flag, 0U, 0U);
    CmdMsgDump(" pic_struct_present_flag                 = %d  ", pCmdData->h264_vui.pic_struct_present_flag, 0U, 0U);
    CmdMsgDump(" bitstream_restriction_flag              = %d  ", pCmdData->h264_vui.bitstream_restriction_flag, 0U, 0U);
    CmdMsgDump(" motion_vectors_over_pic_boundaries_flag = %d  ", pCmdData->h264_vui.motion_vectors_over_pic_boundaries_flag, 0U, 0U);
    CmdMsgDump(" custom_bitstream_restriction_cfg        = %d  ", pCmdData->h264_vui.custom_bitstream_restriction_cfg, 0U, 0U);
    CmdMsgDump(" aspect_ratio_idc                        = %d  ", pCmdData->h264_vui.aspect_ratio_idc, 0U, 0U);
    CmdMsgDump(" SAR_width                               = %d  ", pCmdData->h264_vui.SAR_width, 0U, 0U);
    CmdMsgDump(" SAR_height                              = %d  ", pCmdData->h264_vui.SAR_height, 0U, 0U);
    CmdMsgDump(" video_format                            = %d  ", pCmdData->h264_vui.video_format, 0U, 0U);
    CmdMsgDump(" colour_primaries                        = %d  ", pCmdData->h264_vui.colour_primaries, 0U, 0U);
    CmdMsgDump(" transfer_characteristics                = %d  ", pCmdData->h264_vui.transfer_characteristics, 0U, 0U);
    CmdMsgDump(" matrix_coefficients                     = %d  ", pCmdData->h264_vui.matrix_coefficients, 0U, 0U);
    CmdMsgDump(" chroma_sample_loc_type_top_field        = %d  ", pCmdData->h264_vui.chroma_sample_loc_type_top_field, 0U, 0U);
    CmdMsgDump(" chroma_sample_loc_type_bottom_field     = %d  ", pCmdData->h264_vui.chroma_sample_loc_type_bottom_field, 0U, 0U);
    CmdMsgDump(" vbr_cbp_rate                            = %d  ", pCmdData->h264_vui.vbr_cbp_rate, 0U, 0U);
    CmdMsgDump(" max_bits_per_mb_denom                   = %d  ", pCmdData->h264_vui.max_bits_per_mb_denom, 0U, 0U);
    CmdMsgDump(" max_bytes_per_pic_denom                 = %d  ", pCmdData->h264_vui.max_bytes_per_pic_denom, 0U, 0U);
    CmdMsgDump(" log2_max_mv_length_horizontal           = %d  ", pCmdData->h264_vui.log2_max_mv_length_horizontal, 0U, 0U);
    CmdMsgDump(" log2_max_mv_length_vertical             = %d  ", pCmdData->h264_vui.log2_max_mv_length_vertical, 0U, 0U);
    CmdMsgDump(" num_reorder_frames                      = %d  ", pCmdData->h264_vui.num_reorder_frames, 0U, 0U);
    CmdMsgDump(" max_dec_frame_buffering                 = %d  ", pCmdData->h264_vui.max_dec_frame_buffering, 0U, 0U);
    CmdMsgDump(" num_units_in_tick                       = %d  ", pCmdData->h264_vui.num_units_in_tick, 0U, 0U);
    CmdMsgDump(" time_scale                              = %d  ", pCmdData->h264_vui.time_scale, 0U, 0U);
}

void dump_cmd_encoder_stop_t(const cmd_encoder_stop_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_ENCODER_STOP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id  = %d", pCmdData->channel_id, 0U, 0U);
    CmdMsgDump(" stream_type = %d", pCmdData->stream_type, 0U, 0U);
    CmdMsgDump(" stop_method = %d", pCmdData->stop_method, 0U, 0U);
}

void dump_cmd_encoder_jpeg_setup_t(const cmd_encoder_jpeg_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_ENCODER_JPEG_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" stream_id          = %d", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" stream_type        = %d", pCmdData->stream_type, 0U, 0U);
    CmdMsgDump(" coding_type        = %d", pCmdData->coding_type, 0U, 0U);
    CmdMsgDump(" enc_start_method   = %d", pCmdData->enc_start_method, 0U, 0U);
    CmdMsgDump(" enc_stop_method    = %d", pCmdData->enc_stop_method, 0U, 0U);
    CmdMsgDump(" chroma_format      = %d", pCmdData->chroma_format, 0U, 0U);
    CmdMsgDump(" is_mjpeg           = %d", pCmdData->is_mjpeg, 0U, 0U);
    CmdMsgDump(" enc_hflip          = %d", pCmdData->enc_hflip, 0U, 0U);
    CmdMsgDump(" enc_vflip          = %d", pCmdData->enc_vflip, 0U, 0U);
    CmdMsgDump(" enc_rotation       = %d", pCmdData->enc_rotation, 0U, 0U);
    CmdMsgDump(" slice_num          = %d", pCmdData->slice_num, 0U, 0U);
    CmdMsgDump(" restart_interval   = %d", pCmdData->restart_interval, 0U, 0U);
    CmdMsgDump(" encode_w           = %d", pCmdData->encode_w, 0U, 0U);
    CmdMsgDump(" encode_h           = %d", pCmdData->encode_h, 0U, 0U);
    CmdMsgDump(" bits_fifo_base     = 0x%X", pCmdData->bits_fifo_base, 0U, 0U);
    CmdMsgDump(" bits_fifo_size     = 0x%X", pCmdData->bits_fifo_size, 0U, 0U);
    CmdMsgDump(" info_fifo_base     = 0x%X", pCmdData->info_fifo_base, 0U, 0U);
    CmdMsgDump(" info_fifo_size     = 0x%X", pCmdData->info_fifo_size, 0U, 0U);
    CmdMsgDump(" dqt_daddr          = 0x%X", pCmdData->dqt_daddr, 0U, 0U);
    CmdMsgDump(" frame_rate         = %d", pCmdData->frame_rate, 0U, 0U);
    CmdMsgDump(" encode_w_ofs       = %d", pCmdData->encode_w_ofs, 0U, 0U);
    CmdMsgDump(" encode_h_ofs       = %d", pCmdData->encode_h_ofs, 0U, 0U);
    CmdMsgDump(" target_bpp         = %d", pCmdData->target_bpp, 0U, 0U);
    CmdMsgDump(" enc_src            = %d", pCmdData->enc_src, 0U, 0U);
    CmdMsgDump(" efm_enable         = %d", pCmdData->efm_enable, 0U, 0U);
    CmdMsgDump(" initial_qlevel     = %d", pCmdData->initial_qlevel, 0U, 0U);
    CmdMsgDump(" tolerance          = %d", pCmdData->tolerance, 0U, 0U);
    CmdMsgDump(" max_reenc_loops    = %d", pCmdData->max_reenc_loops, 0U, 0U);
    CmdMsgDump(" rct_sample_num     = %d", pCmdData->rct_sample_num, 0U, 0U);
    CmdMsgDump(" rct_daddr          = 0x%X", pCmdData->rct_daddr, 0U, 0U);
    CmdMsgDump(" bits_fifo_offset   = %d  ", pCmdData->bits_fifo_offset, 0U, 0U);
}

void dump_cmd_encoder_realtime_encode_setup_t(const ipcam_real_time_encode_param_setup_t *pCmdData, const CmdInfo_t *Info)
{
    UINT16 i;
    UINT8 U8Val = 0U;
    UINT32 Rval;

    CmdWriteDump("CMD_H264ENC_REALTIME_ENCODE_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" stream_id                  = %d  ", pCmdData->stream_id, 0U, 0U);
    CmdMsgDump(" qlevel                     = %d  ", pCmdData->qlevel, 0U, 0U);
    CmdMsgDump(" zmv_threshold              = %d  ", pCmdData->zmv_threshold, 0U, 0U);
    CmdMsgDump(" frame_skip_repeat          = %d  ", pCmdData->frame_skip_repeat, 0U, 0U);
    CmdMsgDump(" bitrate_small_fluctuation  = %d  ", pCmdData->bitrate_small_fluctuation, 0U, 0U);
    CmdMsgDump(" frame_skip                 = %d  ", pCmdData->frame_skip, 0U, 0U);
    CmdMsgDump(" enable_flags               = 0x%X", pCmdData->enable_flags, 0U, 0U);

    CmdMsgDump(" qp_min_on_I                = %d  ", pCmdData->qp_min_on_I, 0U, 0U);
    CmdMsgDump(" qp_max_on_I                = %d  ", pCmdData->qp_max_on_I, 0U, 0U);
    CmdMsgDump(" qp_min_on_P                = %d  ", pCmdData->qp_min_on_P, 0U, 0U);
    CmdMsgDump(" qp_max_on_P                = %d  ", pCmdData->qp_max_on_P, 0U, 0U);
    CmdMsgDump(" qp_min_on_B                = %d  ", pCmdData->qp_min_on_B, 0U, 0U);
    CmdMsgDump(" qp_max_on_B                = %d  ", pCmdData->qp_max_on_B, 0U, 0U);
    CmdMsgDump(" qp_min_on_C                = %d  ", pCmdData->qp_min_on_C, 0U, 0U);
    CmdMsgDump(" qp_max_on_C                = %d  ", pCmdData->qp_max_on_C, 0U, 0U);
    CmdMsgDump(" qp_min_on_D                = %d  ", pCmdData->qp_min_on_D, 0U, 0U);
    CmdMsgDump(" qp_max_on_D                = %d  ", pCmdData->qp_max_on_D, 0U, 0U);

    CmdMsgDump(" qp_min_on_Q                = %d  ", pCmdData->qp_min_on_Q, 0U, 0U);
    CmdMsgDump(" qp_max_on_Q                = %d  ", pCmdData->qp_max_on_Q, 0U, 0U);
    CmdMsgDump(" aqp                        = %d  ", pCmdData->aqp, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->i_qp_reduce), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" i_qp_reduce                = %d  ", U8Val, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->p_qp_reduce), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" p_qp_reduce                = %d  ", U8Val, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->b_qp_reduce), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" b_qp_reduce                = %d  ", U8Val, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->c_qp_reduce), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" c_qp_reduce                = %d  ", U8Val, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->q_qp_reduce), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" q_qp_reduce                = %d  ", U8Val, 0U, 0U);

    CmdMsgDump(" skip_flags                 = %d  ", pCmdData->skip_flags, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->log_q_num_per_gop_minus_1), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" log_q_num_per_gop_minus_1  = %d  ", U8Val, 0U, 0U);
    CmdMsgDump(" gop_n_new                  = %d  ", pCmdData->gop_n_new, 0U, 0U);

    CmdMsgDump(" quant_matrix_addr          = 0x%X", pCmdData->quant_matrix_addr, 0U, 0U);
    CmdMsgDump(" idr_interval_new           = %d  ", pCmdData->idr_interval_new, 0U, 0U);
    CmdMsgDump(" force_idr                  = %d  ", pCmdData->force_idr, 0U, 0U);
    CmdMsgDump(" mvdump_enable              = %d  ", pCmdData->mvdump_enable, 0U, 0U);
    CmdMsgDump(" chroma_format              = %d  ", pCmdData->chroma_format, 0U, 0U);

    CmdMsgDump(" luma_log2_weight_denom     = %d  ", pCmdData->luma_log2_weight_denom, 0U, 0U);
    CmdMsgDump(" wp_enable_flag             = %d  ", pCmdData->wp_enable_flag, 0U, 0U);
    CmdMsgDump(" multiplication_factor      = %d  ", pCmdData->multiplication_factor, 0U, 0U);
    CmdMsgDump(" division_factor            = %d  ", pCmdData->division_factor, 0U, 0U);
    CmdMsgDump(" enc_frame_rate             = 0x%X", pCmdData->enc_frame_rate, 0U, 0U);

    for (i = 0U; i < 3U; i++) {
        CmdMsgDump(" mdSwCat_daddr[%d]           = 0x%X", i, pCmdData->mdSwCat_daddr[i], 0U);
    }
    CmdMsgDump(" encode_w_ofs               = %d  ", pCmdData->encode_w_ofs, 0U, 0U);
    CmdMsgDump(" encode_h_ofs               = %d  ", pCmdData->encode_h_ofs, 0U, 0U);
    CmdMsgDump(" target_bitrate             = %d  ", pCmdData->target_bitrate, 0U, 0U);

    CmdMsgDump(" force_pskip_num_plus1      = %d  ", pCmdData->force_pskip_num_plus1, 0U, 0U);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->zmv_threshold_qp_offset), sizeof(UINT8));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CmdMsgDump(" zmv_threshold_qp_offset    = %d  ", U8Val, 0U, 0U);

    for (i = 0U; i < 2U; i++) {
        CmdMsgDump(" intra_bias[%d]              = %d  ", i, pCmdData->intra_bias[i], 0U);
        CmdMsgDump(" direct_bias[%d]             = %d  ", i, pCmdData->direct_bias[i], 0U);
    }

    CmdMsgDump(" set_I_size                 = %d  ", pCmdData->set_I_size, 0U, 0U);

    for (i = 0U; i < 8U/*MD_CATEGORY_MAX_NUM*/;i++) {
        CmdMsgDump(" md_category_lookup[%d]      = %d  ", i, pCmdData->md_category_lookup[i], 0U);
    }
    CmdMsgDump(" mdSwCat_daddr_P2           = %d  ", pCmdData->mdSwCat_daddr_P2, 0U, 0U);

    for (i = 0U; i < 2U;i++) {
        Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->luma_weight[i]), sizeof(UINT8));
        LL_PrintErrLine(Rval, __func__, __LINE__);
        CmdMsgDump(" luma_weight[%d]             = %d  ", i, U8Val, 0U);
        Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->luma_offset[i]), sizeof(UINT8));
        LL_PrintErrLine(Rval, __func__, __LINE__);
        CmdMsgDump(" luma_offset[%d]             = %d  ", i, U8Val, 0U);
    }
    CmdMsgDump(" i_complex_mul              = %d  ", pCmdData->i_complex_mul, 0U, 0U);
    CmdMsgDump(" p_complex_mul              = %d  ", pCmdData->p_complex_mul, 0U, 0U);
    CmdMsgDump(" b_complex_mul              = %d  ", pCmdData->b_complex_mul, 0U, 0U);
    CmdMsgDump(" c_complex_mul              = %d  ", pCmdData->c_complex_mul, 0U, 0U);
    CmdMsgDump(" ActThresh[0U]              = %d  ", pCmdData->ActThresh[0U], 0U, 0U);
    CmdMsgDump(" ActThresh[1U]              = %d  ", pCmdData->ActThresh[1U], 0U, 0U);
    CmdMsgDump(" ActThresh[2U]              = %d  ", pCmdData->ActThresh[2U], 0U, 0U);
}

void dump_cmd_decoder_setup_t(const cmd_decoder_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_SETUP", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" dec_setup_type         = %d  ", pCmdData->dec_setup_type, 0U, 0U);
    CmdMsgDump(" bits_fifo_base         = 0x%X", pCmdData->bits_fifo_base, 0U, 0U);
    CmdMsgDump(" bits_fifo_limit        = 0x%X", pCmdData->bits_fifo_limit, 0U, 0U);
    CmdMsgDump(" rbuf_smem_size         = %d  ", pCmdData->rbuf_smem_size, 0U, 0U);
    CmdMsgDump(" fbuf_dram_size         = %d  ", pCmdData->fbuf_dram_size, 0U, 0U);
    CmdMsgDump(" pjpeg_buf_size         = %d  ", pCmdData->pjpeg_buf_size, 0U, 0U);
    CmdMsgDump(" use_dproc_vrscl        = %d  ", pCmdData->use_dproc_vrscl, 0U, 0U);
    CmdMsgDump(" cabac_2_recon_delay    = %d  ", pCmdData->cabac_2_recon_delay, 0U, 0U);
    CmdMsgDump(" err_handling_mode      = %d  ", pCmdData->err_handling_mode, 0U, 0U);
    CmdMsgDump(" max_frm_num_of_dpb     = %d  ", pCmdData->max_frm_num_of_dpb, 0U, 0U);
    CmdMsgDump(" enable_pic_info        = %d  ", pCmdData->enable_pic_info, 0U, 0U);
    CmdMsgDump(" is_regression          = %d  ", pCmdData->is_regression, 0U, 0U);
    CmdMsgDump(" is_bringup             = %d  ", pCmdData->is_bringup, 0U, 0U);
    CmdMsgDump(" ext_buf_mask           = 0x%X", (UINT32)pCmdData->ext_buf_mask, 0U, 0U);
}

void dump_cmd_decoder_start_t(const cmd_decoder_start_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_START", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" bits_fifo_start        = 0x%X", pCmdData->bits_fifo_start, 0U, 0U);
    CmdMsgDump(" bits_fifo_end          = 0x%X", pCmdData->bits_fifo_end, 0U, 0U);
    CmdMsgDump(" num_pics               = %d  ", pCmdData->num_pics, 0U, 0U);
    CmdMsgDump(" num_frame_decode       = %d  ", pCmdData->num_frame_decode, 0U, 0U);
    CmdMsgDump(" first_frame_display    = 0x%X%X", DSP_GetU64Msb(pCmdData->first_frame_display),
                                                   DSP_GetU64Lsb(pCmdData->first_frame_display), 0U);
}

void dump_cmd_decoder_stop_t(const cmd_decoder_stop_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_STOP", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" stop_mode              = %d  ", pCmdData->stop_mode, 0U, 0U);
}

void dump_cmd_decoder_bitsfifo_update_t(const cmd_decoder_bitsfifo_update_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_BITSFIFO_UPDATE", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" bits_fifo_start        = 0x%X", pCmdData->bits_fifo_start, 0U, 0U);
    CmdMsgDump(" bits_fifo_end          = 0x%X", pCmdData->bits_fifo_end, 0U, 0U);
    CmdMsgDump(" num_pics               = %d  ", pCmdData->num_pics, 0U, 0U);
}

void dump_cmd_decoder_speed_t(const cmd_decoder_speed_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_SPEED", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id                  = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type                  = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" speed                       = %d  ", pCmdData->speed, 0U, 0U);
    CmdMsgDump(" scan_mode                   = %d  ", pCmdData->scan_mode, 0U, 0U);
    CmdMsgDump(" direction                   = %d  ", pCmdData->direction, 0U, 0U);
    CmdMsgDump(" out_strm_time_scale         = %d  ", pCmdData->out_strm_time_scale, 0U, 0U);
    CmdMsgDump(" out_strm_num_units_in_tick  = %d  ", pCmdData->out_strm_num_units_in_tick, 0U, 0U);
}

void dump_cmd_decoder_trickplay_t(const cmd_decoder_trickplay_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_TRICKPLAY", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" mode                   = %d  ", pCmdData->mode, 0U, 0U);
}

void dump_cmd_decoder_batch_t(const cmd_decoder_batch_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_DECODER_BATCH", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id             = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type             = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump("   batch_cmd_set_info.addr              = 0x%X", pCmdData->batch_cmd_set_info.addr, 0U, 0U);
    CmdMsgDump("   batch_cmd_set_info.id                = 0x%X", pCmdData->batch_cmd_set_info.id, 0U, 0U);
    LL_PrintBatchInfoId(pCmdData->batch_cmd_set_info.id);
    CmdMsgDump("   batch_cmd_set_info.size              = %d  ", pCmdData->batch_cmd_set_info.size, 0U, 0U);
}

void dump_cmd_decoder_stilldec_t(const cmd_decoder_stilldec_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DECODER_STILLDEC", pCmdData->hdr.cmd_code, Info->WriteMode);
    CmdMsgDump(" decoder_id                  = %d  ", pCmdData->hdr.decoder_id, 0U, 0U);
    CmdMsgDump(" codec_type                  = %d  ", pCmdData->hdr.codec_type, 0U, 0U);
    CmdMsgDump(" user_data                   = %d  ", pCmdData->user_data, 0U, 0U);
    CmdMsgDump(" feedback                    = %d  ", pCmdData->feedback, 0U, 0U);
    CmdMsgDump(" is_regression               = %d  ", pCmdData->is_regression, 0U, 0U);
    CmdMsgDump(" bstrm_start                 = 0x%X", pCmdData->bstrm_start, 0U, 0U);
    CmdMsgDump(" bstrm_size                  = %d  ", pCmdData->bstrm_size, 0U, 0U);
    CmdMsgDump(" yuv_buf_base                = 0x%X", pCmdData->yuv_buf_base, 0U, 0U);
    CmdMsgDump(" yuv_buf_size                = %d  ", pCmdData->yuv_buf_size, 0U, 0U);
}

static void dump_dec_fb_desc_t(const dec_fb_desc_t *pCmdData)
{
    CmdMsgDump("  valid           = %d  ", pCmdData->valid, 0, 0);
    CmdMsgDump("  ch_fmt          = %d  ", pCmdData->ch_fmt, 0, 0);
    CmdMsgDump("  buf_pitch       = %d  ", pCmdData->buf_pitch, 0, 0);
    CmdMsgDump("  buf_width       = %d  ", pCmdData->buf_width, 0, 0);
    CmdMsgDump("  buf_height      = %d  ", pCmdData->buf_height, 0, 0);
    CmdMsgDump("  lu_buf_base     = 0x%X", pCmdData->lu_buf_base, 0, 0);
    CmdMsgDump("  ch_buf_base     = 0x%X", pCmdData->ch_buf_base, 0, 0);
    CmdMsgDump("  img_width       = %d  ", pCmdData->img_width, 0, 0);
    CmdMsgDump("  img_height      = %d  ", pCmdData->img_height, 0, 0);
    CmdMsgDump("  img_offset_x    = %d  ", pCmdData->img_offset_x, 0, 0);
    CmdMsgDump("  img_offset_y    = %d  ", pCmdData->img_offset_y, 0, 0);
}

#if 0
void dump_cmd_dproc_setup_t(const cmd_dproc_setup_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_SETUP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vrscl_paint_fbp   = %d  ", pCmdData->vrscl_paint_fbp, 0, 0);
    CmdMsgDump(" vrscl_bg_y        = %d  ", pCmdData->vrscl_bg_y, 0, 0);
    CmdMsgDump(" vrscl_bg_u        = %d  ", pCmdData->vrscl_bg_u, 0, 0);
    CmdMsgDump(" vrscl_bg_v        = %d  ", pCmdData->vrscl_bg_v, 0, 0);
}
#endif

void dump_cmd_dproc_yuv_to_yuv_t(const cmd_dproc_yuv_to_yuv_t *pCmdData, const CmdInfo_t *Info)
{
    UINT8 U8Val = 0U;
    UINT8 i;
    UINT32 Rval;

    CmdWriteDump("DPROC_YUV_TO_YUV", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" user_data         = %d  ", pCmdData->user_data, 0, 0);
    CmdMsgDump(" feedback          = %d  ", pCmdData->feedback, 0, 0);
    CmdMsgDump(" rotate            = %d  ", pCmdData->rotate, 0, 0);
    CmdMsgDump(" flip              = %d  ", pCmdData->flip, 0, 0);
    Rval = dsp_osal_memcpy(&U8Val, &(pCmdData->luma_gain), sizeof(UINT8));
    if (Rval == OK) {
        CmdMsgDump(" luma_gain         = %d  ", U8Val, 0, 0);
        CmdMsgDump("========== src_pic ==========", 0U, 0U, 0U);
    }
    dump_dec_fb_desc_t(&pCmdData->src_pic);
    for (i = 0; i < DPROC_MAX_YUV_OUT_NUM; i++) {
        CmdMsgDump("========== dst_pic %02d ==========", i, 0U, 0U);
        dump_dec_fb_desc_t(&pCmdData->dst_pic[i]);
    }
}

void dump_cmd_dproc_yuv_display_t(const cmd_dproc_yuv_display_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_YUV_DISPLAY", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vout_id           = %d  ", pCmdData->vout_id, 0, 0);
    CmdMsgDump(" vid_win_update    = %d  ", pCmdData->vid_win_update, 0, 0);
    CmdMsgDump(" vid_win_rotate    = %d  ", pCmdData->vid_win_rotate, 0, 0);
    CmdMsgDump(" vid_flip          = %d  ", pCmdData->vid_flip, 0, 0);
    CmdMsgDump(" vid_win_width     = %d  ", pCmdData->vid_win_width, 0, 0);
    CmdMsgDump(" vid_win_height    = %d  ", pCmdData->vid_win_height, 0, 0);
    CmdMsgDump(" vid_win_offset_x  = %d  ", pCmdData->vid_win_offset_x, 0, 0);
    CmdMsgDump(" vid_win_offset_y  = %d  ", pCmdData->vid_win_offset_y, 0, 0);

    CmdMsgDump("========== yuv_pic ==========", 0U, 0U, 0U);
    dump_dec_fb_desc_t(&pCmdData->yuv_pic);
}

#if 0
void dump_cmd_dproc_yuv_paint_t(const cmd_dproc_yuv_paint_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_YUV_PAINT", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" user_data         = %d  ", pCmdData->user_data, 0, 0);
    CmdMsgDump(" feedback          = %d  ", pCmdData->feedback, 0, 0);
    CmdMsgDump(" background_y      = %d  ", pCmdData->background_y, 0, 0);
    CmdMsgDump(" background_u      = %d  ", pCmdData->background_u, 0, 0);
    CmdMsgDump(" background_v      = %d  ", pCmdData->background_v, 0, 0);

    CmdMsgDump("========== yuv_pic ==========", 0U, 0U, 0U);
    dump_dec_fb_desc_t(&pCmdData->yuv_pic);
}
#endif

void dump_cmd_dproc_vrscl_layout_t(const cmd_dproc_vrscl_layout_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_VRSCL_LAYOUT", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id        = %d  ", pCmdData->channel_id, 0, 0);
    CmdMsgDump(" stream_id         = %d  ", pCmdData->stream_id, 0, 0);
    CmdMsgDump(" tar_win_rotate    = %d  ", pCmdData->tar_win_rotate, 0, 0);
    CmdMsgDump(" tar_win_flip      = %d  ", pCmdData->tar_win_flip, 0, 0);
    CmdMsgDump(" tar_win_offset_x  = %d  ", pCmdData->tar_win_offset_x, 0, 0);
    CmdMsgDump(" tar_win_offset_y  = %d  ", pCmdData->tar_win_offset_y, 0, 0);
    CmdMsgDump(" tar_win_width     = %d  ", pCmdData->tar_win_width, 0, 0);
    CmdMsgDump(" tar_win_height    = %d  ", pCmdData->tar_win_height, 0, 0);
    CmdMsgDump(" inp_win_update    = %d  ", pCmdData->inp_win_update, 0, 0);
    CmdMsgDump(" inp_win_offset_x  = %d  ", pCmdData->inp_win_offset_x, 0, 0);
    CmdMsgDump(" inp_win_offset_y  = %d  ", pCmdData->inp_win_offset_y, 0, 0);
    CmdMsgDump(" inp_win_width     = %d  ", pCmdData->inp_win_width, 0, 0);
    CmdMsgDump(" inp_win_height    = %d  ", pCmdData->inp_win_height, 0, 0);
}

void dump_cmd_dproc_vrscl_start_t(const cmd_dproc_vrscl_start_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_VRSCL_START", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" channel_id              = %d  ", pCmdData->channel_id, 0, 0);
    CmdMsgDump(" stream_id               = %d  ", pCmdData->stream_id, 0, 0);
    CmdMsgDump(" direction               = %d  ", pCmdData->direction, 0, 0);
    CmdMsgDump(" pts64                   = 0x%X%X", DSP_GetU64Msb(pCmdData->pts64), DSP_GetU64Lsb(pCmdData->pts64), 0);
    CmdMsgDump(" num_units_in_tick       = %d  ", pCmdData->num_units_in_tick, 0, 0);
    CmdMsgDump(" time_scale              = %d  ", pCmdData->time_scale, 0, 0);
}

void dump_cmd_dproc_vrscl_stop_t(const cmd_dproc_vrscl_stop_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_VRSCL_STOP", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("channel_id              = %d  ", pCmdData->channel_id, 0, 0);
    CmdMsgDump("stream_id               = %d  ", pCmdData->stream_id, 0, 0);
    CmdMsgDump("stop_mode               = %d  ", pCmdData->stop_mode, 0, 0);
    CmdMsgDump(" pts64                   = 0x%X%X", DSP_GetU64Msb(pCmdData->pts64), DSP_GetU64Lsb(pCmdData->pts64), 0);
}

void dump_cmd_dproc_vrscl_vid_win_t(const cmd_dproc_vrscl_vid_win_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_VRSCL_VID_WIN", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("stream_id         = %d  ", pCmdData->stream_id, 0, 0);
    CmdMsgDump("interlaced        = %d  ", pCmdData->interlaced, 0, 0);
    CmdMsgDump("vid_win_offset_x  = %d  ", pCmdData->vid_win_offset_x, 0, 0);
    CmdMsgDump("vid_win_offset_y  = %d  ", pCmdData->vid_win_offset_y, 0, 0);
    CmdMsgDump("vid_win_width     = %d  ", pCmdData->vid_win_width, 0, 0);
    CmdMsgDump("vid_win_height    = %d  ", pCmdData->vid_win_height, 0, 0);
}

#if 0
void dump_cmd_dproc_vrscl_send_ext_pic_t(const cmd_dproc_vrscl_send_ext_pic_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("DPROC_VRSCL_SEND_EXT_PIC", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("channel_id        = %d  ", pCmdData->channel_id, 0, 0);
    CmdMsgDump("pts_high          = %d  ", pCmdData->pts_high, 0, 0);
    CmdMsgDump("pts_low           = %d  ", pCmdData->pts_low, 0, 0);
    CmdMsgDump("========== ext_pic ==========", 0U, 0U, 0U);
    dump_dec_fb_desc_t(&pCmdData->ext_pic);
}
#endif

void dump_cmd_set_vin_config_t(const set_vin_config_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("SET_VIN(PIP)_CONFIG", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" vin_width                  = %d  ", pCmdData->vin_width, 0U, 0U);
    CmdMsgDump(" vin_height                 = %d  ", pCmdData->vin_height, 0U, 0U);
    CmdMsgDump(" vin_config_dram_addr       = 0x%X", pCmdData->vin_config_dram_addr, 0U, 0U);
    CmdMsgDump(" config_data_size           = %d  ", pCmdData->config_data_size, 0U, 0U);
    CmdMsgDump(" sensor_resolution          = %d  ", pCmdData->sensor_resolution, 0U, 0U);
    CmdMsgDump(" sensor_bayer_pattern       = %d  ", pCmdData->sensor_bayer_pattern, 0U, 0U);
    CmdMsgDump(" skip_vin_config_dram_addr  = 0x%X", pCmdData->skip_vin_config_dram_addr, 0U, 0U);
    CmdMsgDump(" skip_vin_config_data_size  = %d  ", pCmdData->skip_vin_config_data_size, 0U, 0U);
}

void dump_cmd_set_hdsp_config_t(const set_hdsp_config_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("SET_VIN(PIP)_SUBPIPELINE", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" decomp_en                  = %d  ", pCmdData->decomp_en, 0U, 0U);
    CmdMsgDump(" comp_en                    = %d  ", pCmdData->comp_en, 0U, 0U);
    CmdMsgDump(" cfa_4x_en                  = %d  ", pCmdData->cfa_4x_en, 0U, 0U);
    CmdMsgDump(" cfa_4x_coeff_shift         = %d  ", pCmdData->cfa_4x_coeff_shift, 0U, 0U);
    CmdMsgDump(" cfa_4x_hout_width          = %d  ", pCmdData->cfa_4x_hout_width, 0U, 0U);
    CmdMsgDump(" decomp_lookup_table_daddr  = 0x%X", pCmdData->decomp_lookup_table_daddr, 0U, 0U);
    CmdMsgDump(" comp_lookup_table_daddr    = 0x%X", pCmdData->comp_lookup_table_daddr, 0U, 0U);
    CmdMsgDump(" cfa_4x_coeff_table_daddr   = 0x%X", pCmdData->cfa_4x_coeff_table_daddr, 0U, 0U);
}

#if 0
void dump_cmd_set_vin_compression_t(const lossy_compression_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("SET_VIN(PIP/IR)_COMPRESSION", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" enable                 = %d  ", pCmdData->enable, 0U, 0U);
    CmdMsgDump(" mantissa               = %d  ", pCmdData->mantissa, 0U, 0U);
    CmdMsgDump(" block_size             = %d  ", pCmdData->block_size, 0U, 0U);
}
#endif

void dump_cmd_set_hdr_compression_t(const lossy_compression_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("HDR_LOSSY_COMPRESSION", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" enable                 = %d  ", pCmdData->enable, 0U, 0U);
    CmdMsgDump(" mantissa               = %d  ", pCmdData->mantissa, 0U, 0U);
    CmdMsgDump(" block_size             = %d  ", pCmdData->block_size, 0U, 0U);
}

void dump_cmd_send_idsp_debug_cmd_t(const send_idsp_debug_cmd_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("SEND_IDSP_DEBUG_CMD", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump(" mode           = %d  ", pCmdData->mode, 0U, 0U);
    CmdMsgDump(" param1         = %d  ", pCmdData->param1, 0U, 0U);
    CmdMsgDump(" param2         = %d  ", pCmdData->param2, 0U, 0U);
    CmdMsgDump(" param3         = %d  ", pCmdData->param3, 0U, 0U);
    CmdMsgDump(" param4         = %d  ", pCmdData->param4, 0U, 0U);
    CmdMsgDump(" param5         = %d  ", pCmdData->param5, 0U, 0U);
    CmdMsgDump(" param6         = %d  ", pCmdData->param6, 0U, 0U);
    CmdMsgDump(" param7         = %d  ", pCmdData->param7, 0U, 0U);
    CmdMsgDump(" param8         = %d  ", pCmdData->param8, 0U, 0U);
}

void dump_cmd_postproc_t(const cmd_postproc_t *pCmdData, const CmdInfo_t *Info)
{
    CmdWriteDump("CMD_POSTPROC", pCmdData->cmd_code, Info->WriteMode);
    CmdMsgDump("decode_cat_id             = %d  ", pCmdData->decode_cat_id, 0, 0);
    CmdMsgDump("decode_id                 = %d  ", pCmdData->decode_id, 0, 0);
    CmdMsgDump("voutA_enable              = %d  ", pCmdData->voutA_enable, 0, 0);
    CmdMsgDump("voutB_enable              = %d  ", pCmdData->voutB_enable, 0, 0);
    CmdMsgDump("input_center_x            = %d  ", pCmdData->input_center_x, 0, 0);
    CmdMsgDump("input_center_y            = %d  ", pCmdData->input_center_y, 0, 0);
    CmdMsgDump("voutA_target_win_update   = %d  ", pCmdData->voutA_target_win_update, 0, 0);
    CmdMsgDump("voutB_target_win_update   = %d  ", pCmdData->voutB_target_win_update, 0, 0);
    CmdMsgDump("vout0_win_update          = %d  ", pCmdData->vout0_win_update, 0, 0);
    CmdMsgDump("vout1_win_update          = %d  ", pCmdData->vout1_win_update, 0, 0);
    CmdMsgDump("voutA_target_win_offset_x = %d  ", pCmdData->voutA_target_win_offset_x, 0, 0);
    CmdMsgDump("voutA_target_win_offset_y = %d  ", pCmdData->voutA_target_win_offset_y, 0, 0);
    CmdMsgDump("voutA_target_win_width    = %d  ", pCmdData->voutA_target_win_width, 0, 0);
    CmdMsgDump("voutA_target_win_height   = %d  ", pCmdData->voutA_target_win_height, 0, 0);
    CmdMsgDump("voutA_zoom_factor_x       = %d  ", pCmdData->voutA_zoom_factor_x, 0, 0);
    CmdMsgDump("voutA_zoom_factor_y       = %d  ", pCmdData->voutA_zoom_factor_y, 0, 0);
    CmdMsgDump("voutB_target_win_offset_x = %d  ", pCmdData->voutB_target_win_offset_x, 0, 0);
    CmdMsgDump("voutB_target_win_offset_y = %d  ", pCmdData->voutB_target_win_offset_y, 0, 0);
    CmdMsgDump("voutB_target_win_width    = %d  ", pCmdData->voutB_target_win_width, 0, 0);
    CmdMsgDump("voutB_target_win_height   = %d  ", pCmdData->voutB_target_win_height, 0, 0);
    CmdMsgDump("voutB_zoom_factor_x       = %d  ", pCmdData->voutB_zoom_factor_x, 0, 0);
    CmdMsgDump("voutB_zoom_factor_y       = %d  ", pCmdData->voutB_zoom_factor_y, 0, 0);
    CmdMsgDump("vout0_flip                = %d  ", pCmdData->vout0_flip, 0, 0);
    CmdMsgDump("vout0_rotate              = %d  ", pCmdData->vout0_rotate, 0, 0);
    CmdMsgDump("vout0_win_offset_x        = %d  ", pCmdData->vout0_win_offset_x, 0, 0);
    CmdMsgDump("vout0_win_offset_y        = %d  ", pCmdData->vout0_win_offset_y, 0, 0);
    CmdMsgDump("vout0_win_width           = %d  ", pCmdData->vout0_win_width, 0, 0);
    CmdMsgDump("vout0_win_height          = %d  ", pCmdData->vout0_win_height, 0, 0);
    CmdMsgDump("vout1_flip                = %d  ", pCmdData->vout1_flip, 0, 0);
    CmdMsgDump("vout1_rotate              = %d  ", pCmdData->vout1_rotate, 0, 0);
    CmdMsgDump("vout1_win_offset_x        = %d  ", pCmdData->vout1_win_offset_x, 0, 0);
    CmdMsgDump("vout1_win_offset_y        = %d  ", pCmdData->vout1_win_offset_y, 0, 0);
    CmdMsgDump("vout1_win_width           = %d  ", pCmdData->vout1_win_width, 0, 0);
    CmdMsgDump("vout1_win_height          = %d  ", pCmdData->vout1_win_height, 0, 0);
}

