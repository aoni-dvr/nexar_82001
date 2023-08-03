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

#ifndef CONFIG_ENTITY_H
#define CONFIG_ENTITY_H

//#include "ik_data_type.h"
#include "AmbaDSP_ImgIkcSpec.h"
#include "AmbaDSP_ImgControlState.h"
#include "iso_cfg_if_cv2.h"
//#include "dsp_imgknl_if_cv2.h"

#define PAD_ALIGN_SIZE (32UL)

#define CFG_FIR0_COEFF (256UL)
#define CFG_FIR1_COEFF (256UL)
#define CFG_FIR2_COEFF (256UL)
#define CFG_CORING_TABLE (256UL)
#define CFG_CE_OUTPUT_SIZE (256UL)
#define CE_UPSAMPLING_SIZE (256UL)

#define DSP_IMG_CORE_NUM_EXPOSURE_CURVE (256UL)
#define DSP_IMG_CORE_NUM_CHROMA_GAIN_CURVE (128UL)

#define NUM_IN_LOOKUP               (576UL)
#define NUM_MATRIX                  (4096UL)
#define NUM_OUT_LOOKUP              (256UL)
#define SIZE_IN                     (NUM_IN_LOOKUP*sizeof(uint32))
#define SIZE_3D                     (NUM_MATRIX*sizeof(uint32))
#define SIZE_OUT                    (NUM_OUT_LOOKUP*sizeof(uint32))
#define SIZE_TONE_CURVE             (256UL)
#define CMPR_CFG_SIZE               (544UL)
#define NUM_BAD_PIXEL_THD           (44UL)
#define NUM_CHROMA_MEDIAN_K_TABLE   (24UL)
#define LS_VIDEO_MCTF_SIZE          (12264UL) //last 24 bytes is not used
#define LS_FINAL_SHARPEN_SIZE       (13128UL)
#define TA_EDGE_REDUCE_SIZE         (16UL)
#define NUM_EXPOSURE_CURVE          (256UL)
#define NUM_CHROMA_GAIN_CURVE       (128UL)

#define LS_FIR0_COEFF_SIZE          (256UL)
#define LS_FIR1_COEFF_SIZE          (256UL)
#define LS_FIR2_COEFF_SIZE          (256UL)
#define LS_CORING_TABLE_SIZE        (256UL)
#define LS_THREE_D_TABLE_SIZE       (4096UL)
#define SHPA_ALPHA_SIZE             (512UL)
#define LS_LNL_TONE_CURVE_SIZE      (256UL)

#define CE_CORING_SIZE              (64UL)
#define CE_CORING_HW_SIZE           (14UL)
#define CE_INPUT_SIZE               (449UL)
#define CE_OUTPUT_SIZE              (256UL)
#define CE_BOOST_SIZE               (64UL)
#define CE_EPSILON_SIZE             (112UL)
#define NUM_DECOMPAND_TABLE         (145UL)
#define NUM_COMPAND_TABLE           (45UL)

#define CAWARP_VIDEO_MAXHORGRID     (32UL)
#define CAWARP_VIDEO_MAXVERGRID     (48UL)
#define CAWARP_VIDEO_MAXSIZE        (CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID*2U)

#define WARP_VIDEO_MAXHORGRID       (128UL)
#define WARP_VIDEO_MAXVERGRID       (96UL)
#define WARP_VIDEO_MAXSIZE          (WARP_VIDEO_MAXHORGRID*WARP_VIDEO_MAXVERGRID*2U*2U) // // multi_slice

#define CALIB_VIGNETTE_BUF_ALIGNMENT_SIZE (8720UL)   // 2180*4 = 8720
#define CALIB_VIGNETTE_TABLE_SIZE (8192UL)   // 4 bayer *4 dir *128 entry*4 u32

#define FRONT_END_TONE_CURVE_SIZE (580UL) //145 * 4bytes

#define ISO_CFG_BATCH_QUEUE_CMD_NUM (10UL)
#define ISO_CFG_BATCH_QUEUE_SIZE (ISO_CFG_BATCH_QUEUE_CMD_NUM * 128UL)

typedef cv2_low_iso_cfg_t ik_cv2_liso_cfg_t;
typedef still_high_iso_cfg_t ik_cv2_hiso_cfg_t;

//typedef cv2_low_iso_param_t amba_ik_cv2_liso_cfg_t;
//typedef still_high_iso_param_t amba_ik_cv2_hiso_cfg_t;

typedef struct {
    uint32 data0:6;
    uint32 data1:6;
    uint32 data2:6;
    uint32 data3:6;
    uint32 data4:6;
    uint32 reserved:2;
} ce_coring_table_hw_format_t;

typedef struct {
    uint32 coef24 : 9;
    uint32 reserved : 23;
    int8  coef0to23[24];
} fir_coef_one_dir_t;

typedef struct {
    uint32 shift0 : 3;
    uint32 shift1 : 3;
    uint32 shift2 : 3;
    uint32 shift3 : 3;
    uint32 shift4 : 3;
    uint32 shift5 : 3;
    uint32 shift6 : 3;
    uint32 shift7 : 3;
    uint32 shift8 : 3;
    uint32 reserved : 5;
} fir_shift_group_t;

typedef struct {
    fir_coef_one_dir_t coefs[9];
    fir_shift_group_t        shift;
} fir_t;

typedef  struct {
    uint32      t0    : 5; // 2.3
    uint32      t1    : 5;
    uint32      t2    : 5;
    uint32      t3    : 5;
    uint32      rsvd      :12;
} shpA_core_table_entry_t;

typedef  struct {
    uint32      r    : 10;
    uint32      g    : 10;
    uint32      b    : 10;
    uint32      rsvd      :2;
} img_cc_out_hw_t;

typedef struct {
//    update_iso_config_t update_iso_config;
//    update_hdr_ce_config_t update_hdr_ce_config;
    uint32 cmd_code;
    uint8 reserved[124];
} img_batch_atom;


typedef struct {
    intptr cmd_address;
    size_t cmd_size;
} amba_ik_setup_cmd_description_t;

typedef struct {
    uint32 idsp_cmd_number;
    amba_ik_setup_cmd_description_t idsp_cmd_info[MAX_CONTAINER_IDSP_CMD_NUM];
    uint32 vin_cmd_number;
    amba_ik_setup_cmd_description_t vin_cmd_info[MAX_CONTAINER_VIN_CMD_NUM];
    // idsp command list
    send_idsp_debug_cmd_t  idsp_debug_cmd;
    aaa_statistics_setup_t aaa_statistics_setup;
    aaa_statistics_setup1_t aaa_statistics_setup1_af_ex;
    aaa_statistics_setup2_t aaa_statistics_setup2_af_ex;
    aaa_statistics_setup1_t aaa_statistics_setup1_pg_af_ex;
    aaa_statistics_setup2_t aaa_statistics_setup2_pg_af_ex;
    aaa_histogram_t aaa_histogram;
    aaa_pseudo_y_t aaa_pseudo_y;
    //cv2
    send_idsp_debug_cmd_t TC_table;
} amba_ik_setup_cmds_t;

#define MAX_TABLE_DESCRIPTION_NUMBER (90UL) //FIXME:
typedef struct {
    uint32 *tbl_addr_addr;
    uintptr tbl_vitual_addr;
    size_t size;
} amba_ik_cfg_tbl_description_t;

typedef struct {
    uint32  used_table_number;
    amba_ik_cfg_tbl_description_t tbl_description[MAX_TABLE_DESCRIPTION_NUMBER];
} amba_ik_cfg_tbl_description_list_t;

typedef struct {
    amba_ik_iso_config_table_index_t table_index;
    uint8 reserved_iso_config_table_index_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_iso_config_table_index_t))%(PAD_ALIGN_SIZE))];
    ik_cv2_liso_cfg_t iso_config;
    uint8 reserved_cv1_liso_cfg_t[(PAD_ALIGN_SIZE)-((sizeof(ik_cv2_liso_cfg_t))%(PAD_ALIGN_SIZE))];
    amba_ik_setup_cmds_t cmds;
    uint8 reserved_setup_cmds_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_setup_cmds_t))%(PAD_ALIGN_SIZE))];
    amba_ik_cfg_tbl_description_list_t iso_tables_info;
    uint8 reserved_cfg_tbl_description_list_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_cfg_tbl_description_list_t))%(PAD_ALIGN_SIZE))];
} amba_ik_iso_config_and_state_t;

typedef struct {
    amba_ik_hiso_config_table_index_t table_index;
    uint8 reserved_iso_config_table_index_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_hiso_config_table_index_t))%(PAD_ALIGN_SIZE))];
    ik_cv2_hiso_cfg_t iso_config;
    uint8 reserved_cv1_liso_cfg_t[(PAD_ALIGN_SIZE)-((sizeof(ik_cv2_hiso_cfg_t))%(PAD_ALIGN_SIZE))];
    amba_ik_setup_cmds_t cmds;
    uint8 reserved_setup_cmds_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_setup_cmds_t))%(PAD_ALIGN_SIZE))];
    amba_ik_cfg_tbl_description_list_t iso_tables_info;
    uint8 reserved_cfg_tbl_description_list_t[(PAD_ALIGN_SIZE)-((sizeof(amba_ik_cfg_tbl_description_list_t))%(PAD_ALIGN_SIZE))];
} amba_ik_hiso_config_and_state_t;

typedef struct {
    uint32 ik_id;
    uint8 reserved_ik_id[(PAD_ALIGN_SIZE)-((sizeof(uint32))%(PAD_ALIGN_SIZE))];
} amba_ik_config_container_t;


typedef struct {
    uint8 fe_tone_curve[FRONT_END_TONE_CURVE_SIZE];
    uint8 reserved_fe_tone_curve[(PAD_ALIGN_SIZE)-((FRONT_END_TONE_CURVE_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 vignette[CALIB_VIGNETTE_TABLE_SIZE];
    uint8 reserved_vignette[(PAD_ALIGN_SIZE)-((CALIB_VIGNETTE_BUF_ALIGNMENT_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint16 local_exposure[DSP_IMG_CORE_NUM_EXPOSURE_CURVE];
    uint16 chroma_scale[DSP_IMG_CORE_NUM_CHROMA_GAIN_CURVE];
    uint8 cc_in[SIZE_IN];
    uint8 cc_3d[SIZE_3D];
    uint8 cc_out[SIZE_OUT];
    uint32 dark_pixel_detect_thd_table[NUM_BAD_PIXEL_THD];
    uint8 reserved_dark_pixel_detect_thd_table[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 hot_pixel_detect_thd_table[NUM_BAD_PIXEL_THD];
    uint8 reserved_hot_pixel_detect_thd_table[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 chroma_median_k_table[NUM_CHROMA_MEDIAN_K_TABLE];
    uint8 reserved_chroma_median_k_table[(PAD_ALIGN_SIZE)-((NUM_CHROMA_MEDIAN_K_TABLE * sizeof(uint16))%(PAD_ALIGN_SIZE))];
    uint8 shp_a_fir1[CFG_FIR1_COEFF];
    uint8 shp_a_fir2[CFG_FIR2_COEFF];
    uint8 shp_a_coring[CFG_CORING_TABLE];
    uint8 r2y_batch_queue[ISO_CFG_BATCH_QUEUE_SIZE]; //128 byte * 10 batch size
    uint8 final_sharpen[LS_FINAL_SHARPEN_SIZE];
    uint8 reserved_final_sharpen[(PAD_ALIGN_SIZE)-((LS_FINAL_SHARPEN_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 video_mctf[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_video_mctf[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 ta_edge_reduce[TA_EDGE_REDUCE_SIZE];
    uint8 reserved_ta_edge_reduce[(PAD_ALIGN_SIZE)-((TA_EDGE_REDUCE_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    uint32 ce_coring[CE_CORING_HW_SIZE];
    uint8 reserved_ce_coring_table[(PAD_ALIGN_SIZE)-((CE_CORING_HW_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 ce_input_table[CE_INPUT_SIZE];
    uint8 reserved_ce_input_table[(PAD_ALIGN_SIZE)-((CE_INPUT_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 ce_out_table[CFG_CE_OUTPUT_SIZE];
    uint8 ce_epsilon_table[CE_EPSILON_SIZE];
    uint8 reserved_ce_epsilon_table[(PAD_ALIGN_SIZE)-((CE_EPSILON_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 ce_boost_table[CE_BOOST_SIZE];
    uint8 md_asf_fir1_table[LS_FIR1_COEFF_SIZE];
    uint8 md_asf_fir2_table[LS_FIR2_COEFF_SIZE];
    uint8 md_asf_coring_table[LS_CORING_TABLE_SIZE];
    uint8 md_smooth_coring_table[LS_CORING_TABLE_SIZE];
    uint8 md_mctf_table[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_md_mctf_table[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    //mono
#if INTERNAL_EARLY_TEST_ENV
    //step1
    uint8 mono_vignette[CALIB_VIGNETTE_TABLE_SIZE];
    uint8 reserved_mono_vignette[(PAD_ALIGN_SIZE)-((CALIB_VIGNETTE_BUF_ALIGNMENT_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint16 mono_local_exposure[DSP_IMG_CORE_NUM_EXPOSURE_CURVE];
    uint16 mono_chroma_scale[DSP_IMG_CORE_NUM_CHROMA_GAIN_CURVE];
    uint8 mono_step1_cc_in[SIZE_IN];
    uint8 mono_step1_cc_3d[SIZE_3D];
    uint8 mono_step1_cc_out[SIZE_OUT];
    uint32 mono_dark_pixel_detect_thd_table[NUM_BAD_PIXEL_THD];
    uint8 reserved_mono_dark_pixel_detect_thd_table[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 mono_hot_pixel_detect_thd_table[NUM_BAD_PIXEL_THD];
    uint8 reserved_mono_hot_pixel_detect_thd_table[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 mono_chroma_median_k_table[NUM_CHROMA_MEDIAN_K_TABLE];
    uint8 reserved_mono_chroma_median_k_table[(PAD_ALIGN_SIZE)-((NUM_CHROMA_MEDIAN_K_TABLE * sizeof(uint16))%(PAD_ALIGN_SIZE))];
    uint8 mono_step1_shp_a_fir1[CFG_FIR1_COEFF];
    uint8 mono_step1_shp_a_fir2[CFG_FIR2_COEFF];
    uint8 mono_step1_shp_a_coring[CFG_CORING_TABLE];
    uint8 mono_step1_batch_queue[ISO_CFG_BATCH_QUEUE_SIZE]; //128 byte * 10 batch size
    //step4
    uint8 mono_step4_shp_a_fir1[CFG_FIR1_COEFF];
    uint8 mono_step4_shp_a_fir2[CFG_FIR2_COEFF];
    uint8 mono_step4_shp_a_coring[CFG_CORING_TABLE];
    //yuv
    uint8 mono_cc_reg_alpha1[SIZE_IN];
    uint8 mono_cc_3d_alpha1[SIZE_3D];
    uint8 mono_cc_reg_alpha2[SIZE_IN];
    uint8 mono_cc_3d_alpha2[SIZE_3D];
    uint8 mono_cc_reg_alpha[SIZE_IN];
    uint8 mono_cc_3d_alpha[SIZE_3D];
    //mctf
    uint8 mcts_fus_edge[LS_FINAL_SHARPEN_SIZE];
    uint8 reserved_mcts_fus_edge[(PAD_ALIGN_SIZE)-((LS_FINAL_SHARPEN_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mctf_C[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_mctf_C[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mctf_alpha[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_mctf_alpha[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mcts_alpha[LS_FINAL_SHARPEN_SIZE];
    uint8 reserved_mcts_alpha[(PAD_ALIGN_SIZE)-((LS_FINAL_SHARPEN_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mctf_freq_blend[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_mctf_freq_blend[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mcts_freq_blend[LS_FINAL_SHARPEN_SIZE];
    uint8 reserved_mcts_freq_blend[(PAD_ALIGN_SIZE)-((LS_FINAL_SHARPEN_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mctf_passthru[LS_VIDEO_MCTF_SIZE];
    uint8 reserved_mctf_passthru[(PAD_ALIGN_SIZE)-((LS_VIDEO_MCTF_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mcts_passthru[LS_FINAL_SHARPEN_SIZE];
    uint8 reserved_mcts_passthru[(PAD_ALIGN_SIZE)-((LS_FINAL_SHARPEN_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    //mono ce
    uint32 mono_ce_coring[CE_CORING_HW_SIZE];
    uint8 reserved_mono_ce_coring_table[(PAD_ALIGN_SIZE)-((CE_CORING_HW_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 mono_ce_input_table[CE_INPUT_SIZE];
    uint8 reserved_mono_ce_input_table[(PAD_ALIGN_SIZE)-((CE_INPUT_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 mono_ce_out_table[CFG_CE_OUTPUT_SIZE];
    uint8 mono_ce_epsilon_table[CE_EPSILON_SIZE];
    uint8 reserved_mono_ce_epsilon_table[(PAD_ALIGN_SIZE)-((CE_EPSILON_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 mono_ce_boost_table[CE_BOOST_SIZE];
#endif
} amba_ik_iso_config_tables_t;

typedef struct {
    uint8 vignette[CALIB_VIGNETTE_TABLE_SIZE];
    uint8 reserved_vignette[(PAD_ALIGN_SIZE)-((CALIB_VIGNETTE_BUF_ALIGNMENT_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];

    uint8 r2y_batch_queue[ISO_CFG_BATCH_QUEUE_SIZE]; //128 byte * 10 batch size

    //hiso
    // Note: Color
    uint16_t ChromaScaleGainCurve[DSP_IMG_CORE_NUM_CHROMA_GAIN_CURVE];
    uint8 NormalCCReg[SIZE_IN];
    uint8 NormalCC3d[SIZE_3D];
    uint8 NormalCCOut[SIZE_OUT];

    uint8 step1MctsEdgeDetect[LS_FINAL_SHARPEN_SIZE];  // step 1 SHPB
    uint8 step2MctsEdgeASF[LS_FINAL_SHARPEN_SIZE];     // step 2 SHPB
    uint8 step3MctsLHighASF[LS_FINAL_SHARPEN_SIZE];   // step 3 SHPB
    uint8 step4MctsLLowASF[LS_FINAL_SHARPEN_SIZE];    // step 4 SHPB
    uint8 step5MctfLowAsfCombine[LS_VIDEO_MCTF_SIZE]; // step 5 MCTF
    uint8 step5MctsMed2ASF[LS_FINAL_SHARPEN_SIZE];     // step 5 SHPB
    uint8 step6MctfC8[LS_VIDEO_MCTF_SIZE]; // step 6 MCTF
    uint8 step7MctfC4[LS_VIDEO_MCTF_SIZE]; // step 7 MCTF
    uint8 step8MctfCB[LS_VIDEO_MCTF_SIZE]; // step 8 MCTF
    uint8 step9MctfLMix2[LS_VIDEO_MCTF_SIZE]; // step 9 MCTF
    uint8 step9MctsLMix2[LS_FINAL_SHARPEN_SIZE]; // step 9 SHPB
    uint8 step10MctfLLumaNoiseCombine[LS_VIDEO_MCTF_SIZE]; // step 10 MCTF
    uint8 step11MctfLIHICombine[LS_VIDEO_MCTF_SIZE]; // step 11 MCTF
    uint8 step11MctsLIHICombine[LS_FINAL_SHARPEN_SIZE]; // step 11 SHPB
    uint8 step12MctfLI2HICombine[LS_VIDEO_MCTF_SIZE]; // step 12 MCTF
    uint8 step12MctsLI2HICombine[LS_FINAL_SHARPEN_SIZE]; // step 12 SHPB
    uint8 step13MctfCA[LS_VIDEO_MCTF_SIZE]; // step 13 MCTF
    uint8 MctfDisable[LS_VIDEO_MCTF_SIZE];
    uint8 MctsDisable[LS_FINAL_SHARPEN_SIZE];
    uint8 MctsDisable420[LS_FINAL_SHARPEN_SIZE];

    // Note: Step01
    uint32 Step1HotPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_Step1HotPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step1DarkPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_Step1DarkPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16_t Step1ChromaMedianKTable[NUM_CHROMA_MEDIAN_K_TABLE];
    uint8 Reserved_Step1_1[(PAD_ALIGN_SIZE)-((NUM_CHROMA_MEDIAN_K_TABLE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step1ShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step1ShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step1ShpCoring[LS_CORING_TABLE_SIZE];
    uint8 Step1FeToneCurve[FRONT_END_TONE_CURVE_SIZE];
    uint8 ReservedStep1FeToneCurve[(PAD_ALIGN_SIZE)-((FRONT_END_TONE_CURVE_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint32 Step1CeCoring[CE_CORING_HW_SIZE];
    uint8 ReservedStep1CeCoringTable[(PAD_ALIGN_SIZE)-((CE_CORING_HW_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step1CeInputTable[CE_INPUT_SIZE];
    uint8 ReservedStep1CeInputTable[(PAD_ALIGN_SIZE)-((CE_INPUT_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 Step1CeOutTable[CFG_CE_OUTPUT_SIZE];
    uint8 Step1CeEpsilonTable[CE_EPSILON_SIZE];
    uint8 ReservedStep1CeEpsilonTable[(PAD_ALIGN_SIZE)-((CE_EPSILON_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step1CeBoostTable[CE_BOOST_SIZE];
    // Note: Step02
    uint8 Step2CcRegREdgeScorce[SIZE_IN];
    uint8 Step2Cc3dEdgeScorce[SIZE_3D];

    // Note: Step03
    uint8 Step3ChromaShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step3ChromaShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step3ChromaShpCoring[LS_CORING_TABLE_SIZE];
    // Note: Step04
    uint8 Step4AsfMedFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step4AsfMedFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step4AsfMedCoring[LS_CORING_TABLE_SIZE];
    // Note: Step05
    uint8 Step5MedShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step5MedShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step5MedShpCoring[LS_CORING_TABLE_SIZE];

    // Note: Step08
    uint8 HiMedAsfFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 HiMedAsfFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 HiMedAsfCoring[LS_CORING_TABLE_SIZE];

    // Note: Step09
    uint8 Step9ShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step9ShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step9ShpCoring[LS_CORING_TABLE_SIZE];
    // Note: Step11
    uint32 Step11HotPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_HisoStep11HotPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step11DarkPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_HisoStep11DarkPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16_t Step11ChromaMedianKTable[NUM_CHROMA_MEDIAN_K_TABLE];
    uint8 Reserved_Step11_1[(PAD_ALIGN_SIZE)-((NUM_CHROMA_MEDIAN_K_TABLE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step11ShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step11ShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step11ShpCoring[LS_CORING_TABLE_SIZE];
    uint8 Step11FeToneCurve[FRONT_END_TONE_CURVE_SIZE];
    uint8 ReservedStep11FeToneCurve[(PAD_ALIGN_SIZE)-((FRONT_END_TONE_CURVE_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint32 Step11CeCoring[CE_CORING_HW_SIZE];
    uint8 ReservedStep11CeCoringTable[(PAD_ALIGN_SIZE)-((CE_CORING_HW_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step11CeInputTable[CE_INPUT_SIZE];
    uint8 ReservedStep11CeInputTable[(PAD_ALIGN_SIZE)-((CE_INPUT_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 Step11CeOutTable[CFG_CE_OUTPUT_SIZE];
    uint8 Step11CeEpsilonTable[CE_EPSILON_SIZE];
    uint8 ReservedStep11CeEpsilonTable[(PAD_ALIGN_SIZE)-((CE_EPSILON_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step11CeBoostTable[CE_BOOST_SIZE];

    // Note: Step12
    uint32 Step12HotPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_Step12HotPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step12DarkPixelDetectThdTable[NUM_BAD_PIXEL_THD];
    uint8 reserved_Step12DarkPixelDetectThdTable[(PAD_ALIGN_SIZE)-((NUM_BAD_PIXEL_THD * sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16_t Step12ChromaMedianKTable[NUM_CHROMA_MEDIAN_K_TABLE];
    uint8 Reserved_Step12_1[(PAD_ALIGN_SIZE)-((NUM_CHROMA_MEDIAN_K_TABLE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step12ShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step12ShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step12ShpCoring[LS_CORING_TABLE_SIZE];

    uint8 Step12FeToneCurve[FRONT_END_TONE_CURVE_SIZE];
    uint8 ReservedStep12FeToneCurve[(PAD_ALIGN_SIZE)-((FRONT_END_TONE_CURVE_SIZE * sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint32 Step12CeCoring[CE_CORING_HW_SIZE];
    uint8 ReservedStep12CeCoringTable[(PAD_ALIGN_SIZE)-((CE_CORING_HW_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint32 Step12CeInputTable[CE_INPUT_SIZE];
    uint8 ReservedStep12CeInputTable[(PAD_ALIGN_SIZE)-((CE_INPUT_SIZE* sizeof(uint32))%(PAD_ALIGN_SIZE))];
    uint16 Step12CeOutTable[CFG_CE_OUTPUT_SIZE];
    uint8 Step12CeEpsilonTable[CE_EPSILON_SIZE];
    uint8 ReservedStep12CeEpsilonTable[(PAD_ALIGN_SIZE)-((CE_EPSILON_SIZE* sizeof(uint8))%(PAD_ALIGN_SIZE))];
    uint8 Step12CeBoostTable[CE_BOOST_SIZE];

    // Note: Step13
    uint8 Step13ChromaShpFir1Coeff[LS_FIR1_COEFF_SIZE];
    uint8 Step13ChromaShpFir2Coeff[LS_FIR2_COEFF_SIZE];
    uint8 Step13ChromaShpCoring[LS_CORING_TABLE_SIZE];
} amba_ik_hiso_config_tables_t;

typedef struct {
    uint32 ctx_start;
    uint32 flip_mode;
    uint32 reserved[254];
    aaa_statistics_setup_t last_aaa_statistics_setup;
    aaa_statistics_setup1_t last_aaa_statistics_setup1_af_ex;
    aaa_statistics_setup2_t last_aaa_statistics_setup2_af_ex;
    aaa_statistics_setup1_t last_aaa_statistics_setup1_pg_af_ex;
    aaa_statistics_setup2_t last_aaa_statistics_setup2_pg_af_ex;
    aaa_histogram_t last_aaa_histogram;
    aaa_histogram_t last_aaa_histogram_pg;
    int32 first_compression_offset;
    uint32 ik_version_major;
    uint32 ik_version_minor;
} ik_ctx_info_t;

typedef struct {
    uint8 data[284UL];
    uint32 ctx_info_offset; /* internal used for debugging */
    uint8 data1[654988UL];
#if INTERNAL_EARLY_TEST_ENV
    uint8 data2[800UL];
#endif
    ik_ctx_info_t ctx_info;
} ik_amalgam_liso_t;

#endif
