/*
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
*/

#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H
#include "ik_data_type.h"

typedef struct {
    uint8 fe_tone_curve;
    uint8 vignette;
    uint8 local_exposure;
    uint8 chroma_scale;
    uint8 cc_in;
    uint8 cc_3d;
    uint8 cc_out;
    uint8 dark_pixel_detect_thd_table;
    uint8 hot_pixel_detect_thd_table;
    uint8 chroma_median_k_table;
    uint8 shp_a_fir1;
    uint8 shp_a_fir2;
    uint8 shp_a_coring;
    uint8 r2y_batch_queue;
    uint8 final_sharpen;
    uint8 video_mctf;
    uint8 ta_edge_reduce;
    uint8 lnl_tone_curve;
    uint8 level_3d_table;
    uint8 hor_warp;
    uint8 ver_warp;
    uint8 hor_cawarp_red;
    uint8 hor_cawarp_blue;
    uint8 ver_cawarp_red;
    uint8 ver_cawarp_blue;
    uint8 ce_coring;
    uint8 ce_input;
    uint8 ce_output;
    uint8 ce_epsilon;
    uint8 ce_boost;
    uint8 md_smooth_coring;
    uint8 md_asf_coring;
    uint8 md_asf_fir1;
    uint8 md_asf_fir2;
    uint8 md_mctf;
#if INTERNAL_EARLY_TEST_ENV
    uint8 mono_vignette;
    uint8 mono_local_exposure;
    uint8 mono_chroma_scale;
    uint8 mono_step1_cc_in;
    uint8 mono_step1_cc_3d;
    uint8 mono_step1_cc_out;
    uint8 mono_dark_pixel_detect_thd_table;
    uint8 mono_hot_pixel_detect_thd_table;
    uint8 mono_chroma_median_k_table;
    uint8 mono_step1_shp_a_fir1;
    uint8 mono_step1_shp_a_fir2;
    uint8 mono_step1_shp_a_coring;
    uint8 mono_step1_batch_queue;
    //step4
    uint8 mono_step4_shp_a_fir1;
    uint8 mono_step4_shp_a_fir2;
    uint8 mono_step4_shp_a_coring;
    //yuv
    uint8 mono_cc_reg_alpha1;
    uint8 mono_cc_3d_alpha1;
    uint8 mono_cc_reg_alpha2;
    uint8 mono_cc_3d_alpha2;
    uint8 mono_cc_reg_alpha;
    uint8 mono_cc_3d_alpha;
    //mctf
    uint8 mcts_fus_edge;
    uint8 mctf_C;
    uint8 mctf_alpha;
    uint8 mcts_alpha;
    uint8 mctf_freq_blend;
    uint8 mcts_freq_blend;
    uint8 mctf_passthru;
    uint8 mcts_passthru;
    uint8 mono_ce_input_table;
    uint8 mono_ce_output_table;
    uint8 mono_ce_coring;
    uint8 mono_epslion;
    uint8 mono_ce_boost;
#endif
} amba_ik_iso_config_table_index_t;

typedef struct {
    uint8 fe_tone_curve;
    //common
    uint8 vignette;
    //step1
    uint8 chroma_scale;
    uint8 cc_in;
    uint8 cc_3d;
    uint8 cc_out;
    uint8 step1_dark_pixel_detect_thd_table;
    uint8 step1_hot_pixel_detect_thd_table;
    uint8 step1_chroma_median_k_table;
    uint8 step1_shp_a_fir1;
    uint8 step1_shp_a_fir2;
    uint8 step1_shp_a_coring;
    uint8 step1_fe_tc;
    uint8 step1_ce_boost;
    uint8 step1_ce_coring;
    uint8 step1_ce_input;
    uint8 step1_ce_output;
    uint8 step1_ce_epsilon;
    //
    uint8 step2_cc_reg;
    uint8 step2_cc_3d;
    uint8 step3_fir1;
    uint8 step3_fir2;
    uint8 step3_coring;
    uint8 step4_fir1;
    uint8 step4_fir2;
    uint8 step4_coring;
    uint8 step5_fir1;
    uint8 step5_fir2;
    uint8 step5_coring;
    uint8 step9_fir1;
    uint8 step9_fir2;
    uint8 step9_coring;
    uint8 step11_hot_pxl;
    uint8 step11_dark_pxl;
    uint8 step11_chroma_median_k;
    uint8 step11_fir1;
    uint8 step11_fir2;
    uint8 step11_coring;
    uint8 step11_fe_tc;
    uint8 step11_ce_boost;
    uint8 step11_ce_coring;
    uint8 step11_ce_input;
    uint8 step11_ce_output;
    uint8 step11_ce_epsilon;
    uint8 step12_hot_pxl;
    uint8 step12_dark_pxl;
    uint8 step12_chroma_median_k;
    uint8 step12_fir1;
    uint8 step12_fir2;
    uint8 step12_coring;
    uint8 step12_fe_tc;
    uint8 step12_ce_boost;
    uint8 step12_ce_coring;
    uint8 step12_ce_input;
    uint8 step12_ce_output;
    uint8 step12_ce_epsilon;
    uint8 step13_fir1;
    uint8 step13_fir2;
    uint8 step13_coring;
    uint8 step1MctsEdgeDetect;
    uint8 step2MctsEdgeASF;
    uint8 step3MctsLHighASF;
    uint8 step4MctsLLowASF;
    uint8 step5MctfLowAsfCombine;
    uint8 step5MctsMed2ASF;
    uint8 step6MctfC8;
    uint8 step7MctfC4;
    uint8 step8MctfCB;
    uint8 step9MctfLMix2;
    uint8 step9MctsLMix2;
    uint8 step10MctfLLumaNoiseCombine;
    uint8 step11MctfLIHICombine;
    uint8 step11MctsLIHICombine;
    uint8 step12MctfLI2HICombine;
    uint8 step12MctsLI2HICombine;
    uint8 step13MctfCA;
    uint8 MctfDisable;
    uint8 MctsDisable;
    uint8 MctsDisable420;

    uint8 hor_warp;
    uint8 ver_warp;
    uint8 hor_cawarp_red;
    uint8 hor_cawarp_blue;
    uint8 ver_cawarp_red;
    uint8 ver_cawarp_blue;

} amba_ik_hiso_config_table_index_t;
#endif
