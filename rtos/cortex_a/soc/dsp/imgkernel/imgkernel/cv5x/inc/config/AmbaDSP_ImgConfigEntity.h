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

#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgAdvancedFilter.h"
#include "idspdrv_imgknl_if.h"

//#define _HISO_SEC3_IN_STEP13


#define PAD_ALIGN_SIZE (32U)

#define CAWARP_VIDEO_MAXHORGRID     (32U)
#define CAWARP_VIDEO_MAXVERGRID     (48U)
#define CAWARP_VIDEO_MAXSIZE        (CAWARP_VIDEO_MAXHORGRID*CAWARP_VIDEO_MAXVERGRID*2U)

#define WARP_VIDEO_MAXHORGRID       (128U)
#define WARP_VIDEO_MAXVERGRID       (96U)
#define WARP_VIDEO_MAXSIZE          (WARP_VIDEO_MAXHORGRID*WARP_VIDEO_MAXVERGRID*2U*2U) // // multi_slice


#define CR_SIZE_4                         (128U*4U)
#define CR_SIZE_5                         (128U*5U)
#define CR_SIZE_6                         (128U*1U)
#define CR_SIZE_7                         (128U*1U)
#define CR_SIZE_8                         (128U*1U)
#define CR_SIZE_9                         (128U*1U)
#define CR_SIZE_10                        (128U*64U)
#define CR_SIZE_11                        (128U*3U)
#define CR_SIZE_12                        (128U*6U)
#define CR_SIZE_13                        (128U*7U)
#define CR_SIZE_14                        (128U*15U)
#define CR_SIZE_15                        (128U*1U)
#define CR_SIZE_16                        (128U*3U)
#define CR_SIZE_17                        (128U*24U)
#define CR_SIZE_18                        (128U*24U)
#define CR_SIZE_19                        (128U*24U)
#define CR_SIZE_20                        (128U*24U)
#define CR_SIZE_21                        (128U*3U)
#define CR_SIZE_22                        (128U*1U)
#define CR_SIZE_23                        (128U*2U)
#define CR_SIZE_24                        (128U*2U)
#define CR_SIZE_25                        (128U*1U)
#define CR_SIZE_26                        (128U*38U)
#define CR_SIZE_27                        (128U*128U)
#define CR_SIZE_28                        (128U*2U)
#define CR_SIZE_29                        (128U*3U)
#define CR_SIZE_30                        (128U*10U)
#define CR_SIZE_31                        (128U*1U)
#define CR_SIZE_32                        (128U*1U)
#define CR_SIZE_33                        (128U*6U)
#define CR_SIZE_34                        (128U*192U)
#define CR_SIZE_35                        (128U*6U)
#define CR_SIZE_36                        (128U*2U)
#define CR_SIZE_37                        (128U*2U)
#define CR_SIZE_38                        (128U*1U)
#define CR_SIZE_39                        (128U*1U)
#define CR_SIZE_40                        (128U*1U)
#define CR_SIZE_41                        (128U*1U)

#define CR_SIZE_42                        (128U*66U)
#define CR_SIZE_43                        (128U*192U)
#define CR_SIZE_44                        (128U*1U)

#define CR_SIZE_45                        (128U*4U)
#define CR_SIZE_46                        (128U*5U)
#define CR_SIZE_47                        (128U*1U)
#define CR_SIZE_48                        (128U*64U)
#define CR_SIZE_49                        (128U*3U)
#define CR_SIZE_50                        (128U*2U)
#define CR_SIZE_51                        (128U*2U)
#define CR_SIZE_52                        (128U*15U)

#define CR_SIZE_100                        (128U*6U)
#define CR_SIZE_101                        (128U*192U)
#define CR_SIZE_102                        (128U*1U)
#define CR_SIZE_103                        (128U*1U)

#define CR_SIZE_111                       (128U*2U)
#define CR_SIZE_112                       (128U*14U)
#define CR_SIZE_113                       (128U*2U)
#define CR_SIZE_114                       (128U*6U)
#define CR_SIZE_115                       (128U*32U)
#define CR_SIZE_116                       (128U*2U)
#define CR_SIZE_117                       (128U*15U)
#define CR_SIZE_118                       (128U*6U)
#define CR_SIZE_119                       (128U*32U)
#define CR_SIZE_120                       (128U*1U)
#define CR_SIZE_121                       (128U*1U)

#ifndef EARLYTEST_ENV
#define BIN_CFG_START_OFFSET    (128U + 0x400u)
#else
#define BIN_CFG_START_OFFSET    128U
#endif
#define SEC_2_BIN_CFG_OFFSET    (128U*0U)
#define SEC_3_BIN_CFG_OFFSET    (128U*612U)
#define SEC_4_BIN_CFG_OFFSET    (128U*871U)
#define SEC_11_BIN_CFG_OFFSET   (128U*1022U)
#define SEC_18_BIN_CFG_OFFSET   (128U*1230U)

#define MAX_SEC_2_CFG_SZ        (128U*612U)
#define MAX_SEC_3_CFG_SZ        (128U*259U)
#define MAX_SEC_4_CFG_SZ        (128U*96U)
#define MAX_SEC_11_CFG_SZ       (128U*200U)
#define MAX_SEC_18_CFG_SZ       (128U*113U)

#define CR_OFFSET_4                       0u
#define CR_OFFSET_5                       (CR_OFFSET_4 + CR_SIZE_4)
#define CR_OFFSET_6                       (CR_OFFSET_5 + CR_SIZE_5)
#define CR_OFFSET_7                       (CR_OFFSET_6 + CR_SIZE_6)
#define CR_OFFSET_8                       (CR_OFFSET_7 + CR_SIZE_7)
#define CR_OFFSET_9                       (CR_OFFSET_8 + CR_SIZE_8)
#define CR_OFFSET_10                      (CR_OFFSET_9 + CR_SIZE_9)
#define CR_OFFSET_11                      (CR_OFFSET_10 + CR_SIZE_10)
#define CR_OFFSET_12                      (CR_OFFSET_11 + CR_SIZE_11)
#define CR_OFFSET_13                      (CR_OFFSET_12 + CR_SIZE_12)
#define CR_OFFSET_14                      (CR_OFFSET_13 + CR_SIZE_13)
#define CR_OFFSET_15                      (CR_OFFSET_14 + CR_SIZE_14)
#define CR_OFFSET_16                      (CR_OFFSET_15 + CR_SIZE_15)
#define CR_OFFSET_17                      (CR_OFFSET_16 + CR_SIZE_16)
#define CR_OFFSET_18                      (CR_OFFSET_17 + CR_SIZE_17)
#define CR_OFFSET_19                      (CR_OFFSET_18 + CR_SIZE_18)
#define CR_OFFSET_20                      (CR_OFFSET_19 + CR_SIZE_19)
#define CR_OFFSET_21                      (CR_OFFSET_20 + CR_SIZE_20)
#define CR_OFFSET_22                      (CR_OFFSET_21 + CR_SIZE_21)
#define CR_OFFSET_23                      (CR_OFFSET_22 + CR_SIZE_22)
#define CR_OFFSET_24                      (CR_OFFSET_23 + CR_SIZE_23)
#define CR_OFFSET_25                      (CR_OFFSET_24 + CR_SIZE_24)
#define CR_OFFSET_26                      (CR_OFFSET_25 + CR_SIZE_25)
#define CR_OFFSET_27                      (CR_OFFSET_26 + CR_SIZE_26)
#define CR_OFFSET_28                      (CR_OFFSET_27 + CR_SIZE_27)
#define CR_OFFSET_29                      (CR_OFFSET_28 + CR_SIZE_28)
#define CR_OFFSET_30                      (CR_OFFSET_29 + CR_SIZE_29)
#define CR_OFFSET_31                      (CR_OFFSET_30 + CR_SIZE_30)
#define CR_OFFSET_32                      (CR_OFFSET_31 + CR_SIZE_31)
#define CR_OFFSET_33                      (CR_OFFSET_32 + CR_SIZE_32)
#define CR_OFFSET_34                      (CR_OFFSET_33 + CR_SIZE_33)
#define CR_OFFSET_35                      (CR_OFFSET_34 + CR_SIZE_34)
#define CR_OFFSET_36                      (CR_OFFSET_35 + CR_SIZE_35)
#define CR_OFFSET_37                      (CR_OFFSET_36 + CR_SIZE_36)
#define CR_OFFSET_38                      (CR_OFFSET_37 + CR_SIZE_37)
#define CR_OFFSET_39                      (CR_OFFSET_38 + CR_SIZE_38)
#define CR_OFFSET_40                      (CR_OFFSET_39 + CR_SIZE_39)
#define CR_OFFSET_41                      (CR_OFFSET_40 + CR_SIZE_40)

#define CR_OFFSET_42                      0u
#define CR_OFFSET_43                      (CR_OFFSET_42 + CR_SIZE_42)
#define CR_OFFSET_44                      (CR_OFFSET_43 + CR_SIZE_43)

#define CR_OFFSET_45                      0u
#define CR_OFFSET_46                      (CR_OFFSET_45 + CR_SIZE_45)
#define CR_OFFSET_47                      (CR_OFFSET_46 + CR_SIZE_46)
#define CR_OFFSET_48                      (CR_OFFSET_47 + CR_SIZE_47)
#define CR_OFFSET_49                      (CR_OFFSET_48 + CR_SIZE_48)
#define CR_OFFSET_50                      (CR_OFFSET_49 + CR_SIZE_49)
#define CR_OFFSET_51                      (CR_OFFSET_50 + CR_SIZE_50)
#define CR_OFFSET_52                      (CR_OFFSET_51 + CR_SIZE_51)

#define CR_OFFSET_100                     0u
#define CR_OFFSET_101                     (CR_OFFSET_100 + CR_SIZE_100)
#define CR_OFFSET_102                     (CR_OFFSET_101 + CR_SIZE_101)
#define CR_OFFSET_103                     (CR_OFFSET_102 + CR_SIZE_102)

#define CR_OFFSET_111                     0u
#define CR_OFFSET_112                     (CR_OFFSET_111 + CR_SIZE_111)
#define CR_OFFSET_113                     (CR_OFFSET_112 + CR_SIZE_112)
#define CR_OFFSET_114                     (CR_OFFSET_113 + CR_SIZE_113)
#define CR_OFFSET_115                     (CR_OFFSET_114 + CR_SIZE_114)
#define CR_OFFSET_116                     (CR_OFFSET_115 + CR_SIZE_115)
#define CR_OFFSET_117                     (CR_OFFSET_116 + CR_SIZE_116)
#define CR_OFFSET_118                     (CR_OFFSET_117 + CR_SIZE_117)
#define CR_OFFSET_119                     (CR_OFFSET_118 + CR_SIZE_118)
#define CR_OFFSET_120                     (CR_OFFSET_119 + CR_SIZE_119)
#define CR_OFFSET_121                     (CR_OFFSET_120 + CR_SIZE_120)

typedef struct {
    // S2
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S3
    uint8_t CR_buf_42[CR_SIZE_42];
    uint8_t CR_buf_43[CR_SIZE_43];
    uint8_t CR_buf_44[CR_SIZE_44];

    // S4
    uint8_t CR_buf_45[CR_SIZE_45];
    uint8_t CR_buf_46[CR_SIZE_46];
    uint8_t CR_buf_47[CR_SIZE_47];
    uint8_t CR_buf_48[CR_SIZE_48];
    uint8_t CR_buf_49[CR_SIZE_49];
    uint8_t CR_buf_50[CR_SIZE_50];
    uint8_t CR_buf_51[CR_SIZE_51];
    uint8_t CR_buf_52[CR_SIZE_52];

    // S11
    uint8_t CR_buf_100[CR_SIZE_100];
    uint8_t CR_buf_101[CR_SIZE_101];
    uint8_t CR_buf_102[CR_SIZE_102];
    uint8_t CR_buf_103[CR_SIZE_103];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_r2y_flow_tables_t; // please make sure this is 128 alignment

typedef struct {

    amba_ik_r2y_flow_tables_t r2y; // step1

    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_hor_b[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    idsp_extra_window_info_t extra_window_info;//64 bytes.
    ik_query_frame_info_t frame_info;
    uint8 reserved[64];
} amba_ik_flow_tables_t; // please make sure this is 128 alignment

typedef struct {
    // S2      38*4 = 152 bytes
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S3       3*4 = 12 bytes
    void *p_CR_buf_42;
    void *p_CR_buf_43;
    void *p_CR_buf_44;

    //S4       8*4 = 32 bytes
    void *p_CR_buf_45;
    void *p_CR_buf_46;
    void *p_CR_buf_47;
    void *p_CR_buf_48;
    void *p_CR_buf_49;
    void *p_CR_buf_50;
    void *p_CR_buf_51;
    void *p_CR_buf_52;

    //S11       4*4 = 16 bytes
    void *p_CR_buf_100;
    void *p_CR_buf_101;
    void *p_CR_buf_102;
    void *p_CR_buf_103;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_r2y_flow_tables_list_t; // please make sure this is 64 alignment

typedef struct {

    amba_ik_r2y_flow_tables_list_t r2y; //step1

    //  40 bytes
    void *p_ca_warp_hor_red;
    void *p_ca_warp_hor_blue;
    void *p_ca_warp_ver_red;
    void *p_ca_warp_ver_blue;
    void *p_warp_hor;
    void *p_warp_ver;
    void *p_warp_hor_b;
    void *p_aaa;
    void *p_extra_window;
    void *p_frame_info;

    // 24 bytes
    uint32 reserved[6];
} amba_ik_flow_tables_list_t; // please make sure this is 64 alignment


// for amalgam...

// Following should be updated for newer chips.
#define IK_AMALGAM_TABLE_SBP_SIZE 3686400U

typedef struct {
    idsp_flow_ctrl_t flow_ctrl;
    uint8 reserved0[128u-(sizeof(idsp_flow_ctrl_t)%128u)]; // make sure section config buffers are 128 alligned
    uint8_t sec2_cfg_buf[MAX_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec4_cfg_buf[MAX_SEC_4_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec11_cfg_buf[MAX_SEC_11_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MAX_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    uint8 reserved1[(PAD_ALIGN_SIZE)-(((uint32)MAX_TILE_NUM_X * (uint32)MAX_TILE_NUM_Y * sizeof(idsp_aaa_data_t))%(PAD_ALIGN_SIZE))];
    uint8_t sbp_map[IK_AMALGAM_TABLE_SBP_SIZE];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_hor_b[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_extra_window_info_t extra_window_info;
} amalgam_flow_data_t;

typedef struct {
    //ctx info
    uint16 ctx_id;
    uint16 reserved1;
    uint32 active_flow_idx;

    //3a statistic
    ik_aaa_stat_info_t aaa_stat_info;
    ik_aaa_pg_af_stat_info_t aaa_pg_stat_info;
    ik_af_stat_ex_info_t af_stat_ex_info;
    ik_pg_af_stat_ex_info_t pg_af_stat_ex_info;
    ik_histogram_info_t hist_info;
    ik_histogram_info_t hist_info_pg;

    // window info
    uint32 flip_mode;
    ik_dzoom_info_t dzoom_info;
    ik_dummy_margin_range_t dmy_range;
    ik_vin_active_window_t active_window;
    ik_stitch_info_t stitching_info;
    ik_window_size_info_t window_size_info;
    amba_ik_calc_win_result_t result_win;
    // vig
    uint32 vig_enable;
    ik_vignette_t vignette_compensation;
    // ca
    int32 cawarp_enable;
    ik_cawarp_info_t calib_cawarp_info;
    ik_grid_point_t calib_cawarp_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    ik_grid_point_t calib_cawarp_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    // warp
    int32 warp_enable;
    uint32 chroma_radius;
    uint32 yuv_mode;
    ik_warp_info_t calib_warp_info;
    ik_warp_buffer_info_t warp_buf_info;
    ik_grid_point_t calib_warp[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U];

    // ik, ikc version
    uint32 ik_version_major;
    uint32 ik_version_minor;
    uint32 ikc_version_major;
    uint32 ikc_version_minor;

    // TBD

} ik_ctx_debug_data_t;

typedef struct {
    amalgam_flow_data_t amalgam_data;
    uint8 reserved0[128u - (sizeof(amalgam_flow_data_t) % 128u)];

    // ctx debug
    ik_ctx_debug_data_t ctx_debug_data;
} ik_debug_data_t;

typedef struct {
    idsp_group_update_info_t group_update_info;
    uint8 reserved0[128u-(sizeof(idsp_group_update_info_t)%128u)];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_hor_b[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
} amalgam_group_update_data_t;


/* HISO */
#define MODE_7_SEC_2_CFG_SZ     MAX_SEC_2_CFG_SZ
#define MODE_8_SEC_2_CFG_SZ     (128U*393U)
#define MODE_10_SEC_18_CFG_SZ   MAX_SEC_18_CFG_SZ
#define MODE_4_SEC_18_CFG_SZ    (128U*107U)


#define MODE_8_SEC_2_CR_OFFSET_26                      0u
#define MODE_8_SEC_2_CR_OFFSET_27                      (MODE_8_SEC_2_CR_OFFSET_26 + CR_SIZE_26)
#define MODE_8_SEC_2_CR_OFFSET_29                      (MODE_8_SEC_2_CR_OFFSET_27 + CR_SIZE_27)
#define MODE_8_SEC_2_CR_OFFSET_30                      (MODE_8_SEC_2_CR_OFFSET_29 + CR_SIZE_29)
#define MODE_8_SEC_2_CR_OFFSET_31                      (MODE_8_SEC_2_CR_OFFSET_30 + CR_SIZE_30)
#define MODE_8_SEC_2_CR_OFFSET_32                      (MODE_8_SEC_2_CR_OFFSET_31 + CR_SIZE_31)
#define MODE_8_SEC_2_CR_OFFSET_33                      (MODE_8_SEC_2_CR_OFFSET_32 + CR_SIZE_32)
#define MODE_8_SEC_2_CR_OFFSET_34                      (MODE_8_SEC_2_CR_OFFSET_33 + CR_SIZE_33)
#define MODE_8_SEC_2_CR_OFFSET_35                      (MODE_8_SEC_2_CR_OFFSET_34 + CR_SIZE_34)
#define MODE_8_SEC_2_CR_OFFSET_36                      (MODE_8_SEC_2_CR_OFFSET_35 + CR_SIZE_35)
#define MODE_8_SEC_2_CR_OFFSET_37                      (MODE_8_SEC_2_CR_OFFSET_36 + CR_SIZE_36)
#define MODE_8_SEC_2_CR_OFFSET_38                      (MODE_8_SEC_2_CR_OFFSET_37 + CR_SIZE_37)
#define MODE_8_SEC_2_CR_OFFSET_39                      (MODE_8_SEC_2_CR_OFFSET_38 + CR_SIZE_38)
#define MODE_8_SEC_2_CR_OFFSET_40                      (MODE_8_SEC_2_CR_OFFSET_39 + CR_SIZE_39)
#define MODE_8_SEC_2_CR_OFFSET_41                      (MODE_8_SEC_2_CR_OFFSET_40 + CR_SIZE_40)

#define MODE_4_SEC_18_CR_OFFSET_112                     0u
#define MODE_4_SEC_18_CR_OFFSET_113                     (MODE_4_SEC_18_CR_OFFSET_112 + CR_SIZE_112)
#define MODE_4_SEC_18_CR_OFFSET_114                     (MODE_4_SEC_18_CR_OFFSET_113 + CR_SIZE_113)
#define MODE_4_SEC_18_CR_OFFSET_115                     (MODE_4_SEC_18_CR_OFFSET_114 + CR_SIZE_114)
#define MODE_4_SEC_18_CR_OFFSET_117                     (MODE_4_SEC_18_CR_OFFSET_115 + CR_SIZE_115)
#define MODE_4_SEC_18_CR_OFFSET_118                     (MODE_4_SEC_18_CR_OFFSET_117 + CR_SIZE_117)
#define MODE_4_SEC_18_CR_OFFSET_119                     (MODE_4_SEC_18_CR_OFFSET_118 + CR_SIZE_118)


/****************     CR Buf     ****************/
typedef struct {
    // step1 S2
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];
} amba_ik_step1_sec2_cr_t; // please make sure this is 128 alignment

typedef struct {
    // step2 S2
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];
} amba_ik_step2_sec2_cr_t; // please make sure this is 128 alignment

typedef struct {
    // step3 S2
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];
} amba_ik_step3_sec2_cr_t; // please make sure this is 128 alignment

typedef amba_ik_step3_sec2_cr_t amba_ik_step4_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step4a_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step5_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step6_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step7_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step8_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step9_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step10_sec2_cr_t;
typedef amba_ik_step1_sec2_cr_t amba_ik_step11_sec2_cr_t;
typedef amba_ik_step1_sec2_cr_t amba_ik_step12_sec2_cr_t;
typedef amba_ik_step3_sec2_cr_t amba_ik_step13_sec2_cr_t;

typedef struct {
    // S3
    uint8_t CR_buf_42[CR_SIZE_42];
    uint8_t CR_buf_43[CR_SIZE_43];
    uint8_t CR_buf_44[CR_SIZE_44];
} amba_ik_sec3_cr_t; // please make sure this is 128 alignment

typedef struct {
    // S4
    uint8_t CR_buf_45[CR_SIZE_45];
    uint8_t CR_buf_46[CR_SIZE_46];
    uint8_t CR_buf_47[CR_SIZE_47];
    uint8_t CR_buf_48[CR_SIZE_48];
    uint8_t CR_buf_49[CR_SIZE_49];
    uint8_t CR_buf_50[CR_SIZE_50];
    uint8_t CR_buf_51[CR_SIZE_51];
    uint8_t CR_buf_52[CR_SIZE_52];
} amba_ik_sec4_cr_t; // please make sure this is 128 alignment

typedef struct {
    // S18
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
} amba_ik_step1_sec18_cr_t; // please make sure this is 128 alignment

typedef amba_ik_step1_sec18_cr_t amba_ik_step2_sec18_cr_t;
typedef amba_ik_step1_sec18_cr_t amba_ik_step3_sec18_cr_t;
typedef amba_ik_step1_sec18_cr_t amba_ik_step4_sec18_cr_t;

typedef struct {
    // S18
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
} amba_ik_step5_sec18_cr_t; // please make sure this is 128 alignment

typedef struct {
    // S18
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_117[CR_SIZE_117];
} amba_ik_step6_sec18_cr_t; // please make sure this is 128 alignment

typedef struct {
    // S18
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_117[CR_SIZE_117];
} amba_ik_step14_sec18_cr_t; // please make sure this is 128 alignment

typedef amba_ik_step6_sec18_cr_t amba_ik_step7_sec18_cr_t;
typedef amba_ik_step6_sec18_cr_t amba_ik_step8_sec18_cr_t;
typedef amba_ik_step5_sec18_cr_t amba_ik_step10_sec18_cr_t;
typedef amba_ik_step5_sec18_cr_t amba_ik_step11_sec18_cr_t;
typedef amba_ik_step5_sec18_cr_t amba_ik_step12_sec18_cr_t;
typedef amba_ik_step6_sec18_cr_t amba_ik_step13_sec18_cr_t;

/****************     CR List     ****************/
typedef struct {
    // step1 S2
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;
} amba_ik_step1_sec2_cr_list_t;

typedef struct {
    // step2 S2
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;
} amba_ik_step2_sec2_cr_list_t;

typedef struct {
    // step3 S2
    void *p_CR_buf_26;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;
} amba_ik_step3_sec2_cr_list_t;

typedef amba_ik_step3_sec2_cr_list_t amba_ik_step4_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step4a_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step5_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step6_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step7_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step8_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step9_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step10_sec2_cr_list_t;
typedef amba_ik_step1_sec2_cr_list_t amba_ik_step11_sec2_cr_list_t;
typedef amba_ik_step1_sec2_cr_list_t amba_ik_step12_sec2_cr_list_t;
typedef amba_ik_step3_sec2_cr_list_t amba_ik_step13_sec2_cr_list_t;

typedef struct {
    //S3       3*4 = 12 bytes
    void *p_CR_buf_42;
    void *p_CR_buf_43;
    void *p_CR_buf_44;
} amba_ik_sec3_cr_list_t;

typedef struct {
    //S4       8*4 = 32 bytes
    void *p_CR_buf_45;
    void *p_CR_buf_46;
    void *p_CR_buf_47;
    void *p_CR_buf_48;
    void *p_CR_buf_49;
    void *p_CR_buf_50;
    void *p_CR_buf_51;
    void *p_CR_buf_52;
} amba_ik_sec4_cr_list_t;

typedef struct {
    //S18
    void *p_CR_buf_112;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
} amba_ik_step1_sec18_cr_list_t;

typedef amba_ik_step1_sec18_cr_list_t amba_ik_step2_sec18_cr_list_t;
typedef amba_ik_step1_sec18_cr_list_t amba_ik_step3_sec18_cr_list_t;
typedef amba_ik_step1_sec18_cr_list_t amba_ik_step4_sec18_cr_list_t;

typedef struct {
    //S18
    void *p_CR_buf_112;
    void *p_CR_buf_114;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
} amba_ik_step5_sec18_cr_list_t;

typedef struct {
    //S18
    void *p_CR_buf_112;
    void *p_CR_buf_114;
    void *p_CR_buf_117;
} amba_ik_step6_sec18_cr_list_t;

typedef struct {
    //S18
    void *p_CR_buf_112;
    void *p_CR_buf_117;
} amba_ik_step14_sec18_cr_list_t;

typedef amba_ik_step6_sec18_cr_list_t amba_ik_step7_sec18_cr_list_t;
typedef amba_ik_step6_sec18_cr_list_t amba_ik_step8_sec18_cr_list_t;
typedef amba_ik_step5_sec18_cr_list_t amba_ik_step10_sec18_cr_list_t;
typedef amba_ik_step5_sec18_cr_list_t amba_ik_step11_sec18_cr_list_t;
typedef amba_ik_step5_sec18_cr_list_t amba_ik_step12_sec18_cr_list_t;
typedef amba_ik_step6_sec18_cr_list_t amba_ik_step13_sec18_cr_list_t;


/****************     HISO CR Buf     ****************/
typedef struct {
    amba_ik_step1_sec2_cr_t sec2;
    amba_ik_sec4_cr_t sec4;
    amba_ik_step1_sec18_cr_t sec18;
} amba_ik_hiso_step1_cr_t;

typedef struct {
    amba_ik_step2_sec2_cr_t sec2;
    amba_ik_step2_sec18_cr_t sec18;
} amba_ik_hiso_step2_cr_t;

typedef struct {
    amba_ik_step3_sec2_cr_t sec2;
    amba_ik_step3_sec18_cr_t sec18;
} amba_ik_hiso_step3_cr_t;

typedef amba_ik_hiso_step3_cr_t amba_ik_hiso_step4_cr_t;

typedef struct {
    amba_ik_step4a_sec2_cr_t sec2;
} amba_ik_hiso_step4a_cr_t;

typedef struct {
    amba_ik_step5_sec2_cr_t sec2;
    amba_ik_step5_sec18_cr_t sec18;
} amba_ik_hiso_step5_cr_t;

typedef struct {
    amba_ik_step6_sec2_cr_t sec2;
    amba_ik_step6_sec18_cr_t sec18;
} amba_ik_hiso_step6_cr_t;

typedef amba_ik_hiso_step6_cr_t amba_ik_hiso_step7_cr_t;
typedef amba_ik_hiso_step6_cr_t amba_ik_hiso_step8_cr_t;

typedef amba_ik_hiso_step4a_cr_t amba_ik_hiso_step9_cr_t;

typedef amba_ik_hiso_step5_cr_t amba_ik_hiso_step10_cr_t;

typedef struct {
    amba_ik_step11_sec2_cr_t sec2;
    amba_ik_sec4_cr_t sec4;
    amba_ik_step11_sec18_cr_t sec18;
} amba_ik_hiso_step11_cr_t;

typedef amba_ik_hiso_step11_cr_t amba_ik_hiso_step12_cr_t;

typedef struct {
    amba_ik_step13_sec2_cr_t sec2;
#ifdef _HISO_SEC3_IN_STEP13
    amba_ik_sec3_cr_t sec3;
#endif
    amba_ik_step13_sec18_cr_t sec18;
} amba_ik_hiso_step13_cr_t;

typedef struct {
    amba_ik_sec3_cr_t sec3;
    amba_ik_step14_sec18_cr_t sec18;
} amba_ik_hiso_step14_cr_t;

/****************     HISO CR List     ****************/
typedef struct {
    amba_ik_step1_sec2_cr_list_t sec2;
    amba_ik_sec4_cr_list_t sec4;
    amba_ik_step1_sec18_cr_list_t sec18;
} amba_ik_hiso_step1_cr_list_t;

typedef struct {
    amba_ik_step2_sec2_cr_list_t sec2;
    amba_ik_step2_sec18_cr_list_t sec18;
} amba_ik_hiso_step2_cr_list_t;

typedef struct {
    amba_ik_step3_sec2_cr_list_t sec2;
    amba_ik_step3_sec18_cr_list_t sec18;
} amba_ik_hiso_step3_cr_list_t;

typedef amba_ik_hiso_step3_cr_list_t amba_ik_hiso_step4_cr_list_t;

typedef struct {
    amba_ik_step4a_sec2_cr_list_t sec2;
} amba_ik_hiso_step4a_cr_list_t;

typedef struct {
    amba_ik_step5_sec2_cr_list_t sec2;
    amba_ik_step5_sec18_cr_list_t sec18;
} amba_ik_hiso_step5_cr_list_t;

typedef struct {
    amba_ik_step6_sec2_cr_list_t sec2;
    amba_ik_step6_sec18_cr_list_t sec18;
} amba_ik_hiso_step6_cr_list_t;

typedef amba_ik_hiso_step6_cr_list_t amba_ik_hiso_step7_cr_list_t;
typedef amba_ik_hiso_step6_cr_list_t amba_ik_hiso_step8_cr_list_t;

typedef amba_ik_hiso_step4a_cr_list_t amba_ik_hiso_step9_cr_list_t;

typedef amba_ik_hiso_step5_cr_list_t amba_ik_hiso_step10_cr_list_t;

typedef struct {
    amba_ik_step11_sec2_cr_list_t sec2;
    amba_ik_sec4_cr_list_t sec4;
    amba_ik_step11_sec18_cr_list_t sec18;
} amba_ik_hiso_step11_cr_list_t;

typedef amba_ik_hiso_step11_cr_list_t amba_ik_hiso_step12_cr_list_t;

typedef struct {
    amba_ik_step13_sec2_cr_list_t sec2;
#ifdef _HISO_SEC3_IN_STEP13
    amba_ik_sec3_cr_list_t sec3;
#endif
    amba_ik_step13_sec18_cr_list_t sec18;
} amba_ik_hiso_step13_cr_list_t;

typedef struct {
    amba_ik_sec3_cr_list_t sec3;
    amba_ik_step14_sec18_cr_list_t sec18;
} amba_ik_hiso_step14_cr_list_t;


/****************     IK HISO CR Buf     ****************/
typedef struct {
    amba_ik_hiso_step1_cr_t step1;
    amba_ik_hiso_step2_cr_t step2;
    amba_ik_hiso_step3_cr_t step3;
    amba_ik_hiso_step4_cr_t step4;
    amba_ik_hiso_step4a_cr_t step4a;
    amba_ik_hiso_step5_cr_t step5;
    amba_ik_hiso_step6_cr_t step6;
    amba_ik_hiso_step7_cr_t step7;
    amba_ik_hiso_step8_cr_t step8;
    amba_ik_hiso_step9_cr_t step9;
    amba_ik_hiso_step10_cr_t step10;
    amba_ik_hiso_step11_cr_t step11;
    amba_ik_hiso_step12_cr_t step12;
    amba_ik_hiso_step13_cr_t step13;
    amba_ik_hiso_step14_cr_t step14;

    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    ik_query_frame_info_t frame_info;
} amba_ik_hiso_flow_tables_t;

/****************     IK HISO CR List     ****************/
typedef struct {
    amba_ik_hiso_step1_cr_list_t step1;
    amba_ik_hiso_step2_cr_list_t step2;
    amba_ik_hiso_step3_cr_list_t step3;
    amba_ik_hiso_step4_cr_list_t step4;
    amba_ik_hiso_step4a_cr_list_t step4a;
    amba_ik_hiso_step5_cr_list_t step5;
    amba_ik_hiso_step6_cr_list_t step6;
    amba_ik_hiso_step7_cr_list_t step7;
    amba_ik_hiso_step8_cr_list_t step8;
    amba_ik_hiso_step9_cr_list_t step9;
    amba_ik_hiso_step10_cr_list_t step10;
    amba_ik_hiso_step11_cr_list_t step11;
    amba_ik_hiso_step12_cr_list_t step12;
    amba_ik_hiso_step13_cr_list_t step13;
    amba_ik_hiso_step14_cr_list_t step14;

    //  28 bytes
    void *p_ca_warp_hor_red;
    void *p_ca_warp_hor_blue;
    void *p_ca_warp_ver_red;
    void *p_ca_warp_ver_blue;
    void *p_warp_hor;
    void *p_warp_ver;
    void *p_aaa;
    void *p_frame_info;

    uint32 reserved[4];
} amba_ik_hiso_flow_tables_list_t;


/****************     HISO Amalgam     ****************/
typedef struct {
    uint8_t sec2_cfg_buf[MODE_7_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec4_cfg_buf[MAX_SEC_4_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_hiso_step1_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_hiso_step2_flow_data_t;

typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step3_flow_data_t;
typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step4_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_hiso_step4a_flow_data_t;

typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step5_flow_data_t;
typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step6_flow_data_t;
typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step7_flow_data_t;
typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step8_flow_data_t;

typedef amalgam_hiso_step4a_flow_data_t amalgam_hiso_step9_flow_data_t;

typedef amalgam_hiso_step2_flow_data_t amalgam_hiso_step10_flow_data_t;
typedef amalgam_hiso_step1_flow_data_t amalgam_hiso_step11_flow_data_t;
typedef amalgam_hiso_step1_flow_data_t amalgam_hiso_step12_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
#ifdef _HISO_SEC3_IN_STEP13
    uint8_t sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
#endif
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_hiso_step13_flow_data_t;

typedef struct {
    uint8_t sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_hiso_step14_flow_data_t;

typedef struct {
    idsp_flow_ctrl_t flow_ctrl;
    uint8 reserved0[128u-(sizeof(idsp_flow_ctrl_t)%128u)]; // make sure section config buffers are 128 alligned
    amalgam_hiso_step1_flow_data_t step1_cfg_buf;
    amalgam_hiso_step2_flow_data_t step2_cfg_buf;
    amalgam_hiso_step3_flow_data_t step3_cfg_buf;
    amalgam_hiso_step4_flow_data_t step4_cfg_buf;
    amalgam_hiso_step4a_flow_data_t step4a_cfg_buf;
    amalgam_hiso_step5_flow_data_t step5_cfg_buf;
    amalgam_hiso_step6_flow_data_t step6_cfg_buf;
    amalgam_hiso_step7_flow_data_t step7_cfg_buf;
    amalgam_hiso_step8_flow_data_t step8_cfg_buf;
    amalgam_hiso_step9_flow_data_t step9_cfg_buf;
    amalgam_hiso_step10_flow_data_t step10_cfg_buf;
    amalgam_hiso_step11_flow_data_t step11_cfg_buf;
    amalgam_hiso_step12_flow_data_t step12_cfg_buf;
    amalgam_hiso_step13_flow_data_t step13_cfg_buf;
    amalgam_hiso_step14_flow_data_t step14_cfg_buf;

    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    uint8 reserved1[(PAD_ALIGN_SIZE)-(((uint32)MAX_TILE_NUM_X * (uint32)MAX_TILE_NUM_Y * sizeof(idsp_aaa_data_t))%(PAD_ALIGN_SIZE))];
    uint8_t sbp_map[IK_AMALGAM_TABLE_SBP_SIZE];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
} amalgam_hiso_flow_data_t;

typedef struct {
    uint32 flow_ctrl;
    uint32 sec2_cfg_buf[SEC_2_CRS_COUNT];
    uint32 sec3_cfg_buf[SEC_3_CRS_COUNT];
    uint32 sec4_cfg_buf[SEC_4_CRS_COUNT];
    uint32 sec18_cfg_buf[SEC_18_CRS_COUNT];
    uint32 aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    uint32 ca_warp_hor_red;
    uint32 ca_warp_hor_blue;
    uint32 ca_warp_ver_red;
    uint32 ca_warp_ver_blue;
    uint32 warp_hor;
    uint32 warp_ver;
    uint32 reserved0[5];
} idsp_crc_data_t;

/////////////// fusion
typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S3
    uint8_t CR_buf_42[CR_SIZE_42];
    uint8_t CR_buf_43[CR_SIZE_43];
    uint8_t CR_buf_44[CR_SIZE_44];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_motion_me1_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_motion_a_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_motion_b_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_motion_c_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_mono3_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];
} amba_ik_mono4_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];
} amba_ik_mono5_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_mono6_flow_tables_t;

typedef struct {
    uint8_t CR_buf_4[CR_SIZE_4];
    uint8_t CR_buf_5[CR_SIZE_5];
    uint8_t CR_buf_6[CR_SIZE_6];
    uint8_t CR_buf_7[CR_SIZE_7];
    uint8_t CR_buf_8[CR_SIZE_8];
    uint8_t CR_buf_9[CR_SIZE_9];
    uint8_t CR_buf_10[CR_SIZE_10];
    uint8_t CR_buf_11[CR_SIZE_11];
    uint8_t CR_buf_12[CR_SIZE_12];
    uint8_t CR_buf_13[CR_SIZE_13];
    uint8_t CR_buf_14[CR_SIZE_14];
    uint8_t CR_buf_15[CR_SIZE_15];
    uint8_t CR_buf_16[CR_SIZE_16];
    uint8_t CR_buf_17[CR_SIZE_17];
    uint8_t CR_buf_18[CR_SIZE_18];
    uint8_t CR_buf_19[CR_SIZE_19];
    uint8_t CR_buf_20[CR_SIZE_20];
    uint8_t CR_buf_21[CR_SIZE_21];
    uint8_t CR_buf_22[CR_SIZE_22];
    uint8_t CR_buf_23[CR_SIZE_23];
    uint8_t CR_buf_24[CR_SIZE_24];
    uint8_t CR_buf_25[CR_SIZE_25];
    uint8_t CR_buf_26[CR_SIZE_26];
    uint8_t CR_buf_27[CR_SIZE_27];
    uint8_t CR_buf_28[CR_SIZE_28];
    uint8_t CR_buf_29[CR_SIZE_29];
    uint8_t CR_buf_30[CR_SIZE_30];
    uint8_t CR_buf_31[CR_SIZE_31];
    uint8_t CR_buf_32[CR_SIZE_32];
    uint8_t CR_buf_33[CR_SIZE_33];
    uint8_t CR_buf_34[CR_SIZE_34];
    uint8_t CR_buf_35[CR_SIZE_35];
    uint8_t CR_buf_36[CR_SIZE_36];
    uint8_t CR_buf_37[CR_SIZE_37];
    uint8_t CR_buf_38[CR_SIZE_38];
    uint8_t CR_buf_39[CR_SIZE_39];
    uint8_t CR_buf_40[CR_SIZE_40];
    uint8_t CR_buf_41[CR_SIZE_41];

    // S18, CV5 S18 should be start from 106, but I think it is ok not to change them.
    uint8_t CR_buf_111[CR_SIZE_111];
    uint8_t CR_buf_112[CR_SIZE_112];
    uint8_t CR_buf_113[CR_SIZE_113];
    uint8_t CR_buf_114[CR_SIZE_114];
    uint8_t CR_buf_115[CR_SIZE_115];
    uint8_t CR_buf_116[CR_SIZE_116];
    uint8_t CR_buf_117[CR_SIZE_117];
    uint8_t CR_buf_118[CR_SIZE_118];
    uint8_t CR_buf_119[CR_SIZE_119];
    uint8_t CR_buf_120[CR_SIZE_120];
    uint8_t CR_buf_121[CR_SIZE_121];
} amba_ik_mono7_flow_tables_t;

typedef struct {
    amba_ik_r2y_flow_tables_t r2y; // mono2 // step1
    amba_ik_motion_me1_flow_tables_t motion_me1; // step2
    amba_ik_motion_a_flow_tables_t motion_a; // step3
    amba_ik_motion_b_flow_tables_t motion_b; // step4
    amba_ik_motion_c_flow_tables_t motion_c; // step5
    amba_ik_r2y_flow_tables_t mono1; // step6
    amba_ik_mono3_flow_tables_t mono3; // step7
    amba_ik_mono4_flow_tables_t mono4; // step8
    amba_ik_mono5_flow_tables_t mono5; // step9
    amba_ik_mono6_flow_tables_t mono6; // step10
    amba_ik_mono7_flow_tables_t mono7; // step11
    amba_ik_r2y_flow_tables_t mono8; // step12

    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_hor_b[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    idsp_extra_window_info_t extra_window_info;//128 bytes.
    ik_query_frame_info_t frame_info;
} amba_ik_motion_fusion_flow_tables_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S3       3*4 = 12 bytes
    void *p_CR_buf_42;
    void *p_CR_buf_43;
    void *p_CR_buf_44;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_motion_me1_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_motion_a_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_motion_b_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_motion_c_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_mono3_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;
} amba_ik_mono4_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;
} amba_ik_mono5_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_mono6_flow_tables_list_t;

typedef struct {
    void *p_CR_buf_4;
    void *p_CR_buf_5;
    void *p_CR_buf_6;
    void *p_CR_buf_7;
    void *p_CR_buf_8;
    void *p_CR_buf_9;
    void *p_CR_buf_10;
    void *p_CR_buf_11;
    void *p_CR_buf_12;
    void *p_CR_buf_13;
    void *p_CR_buf_14;
    void *p_CR_buf_15;
    void *p_CR_buf_16;
    void *p_CR_buf_17;
    void *p_CR_buf_18;
    void *p_CR_buf_19;
    void *p_CR_buf_20;
    void *p_CR_buf_21;
    void *p_CR_buf_22;
    void *p_CR_buf_23;
    void *p_CR_buf_24;
    void *p_CR_buf_25;
    void *p_CR_buf_26;
    void *p_CR_buf_27;
    void *p_CR_buf_28;
    void *p_CR_buf_29;
    void *p_CR_buf_30;
    void *p_CR_buf_31;
    void *p_CR_buf_32;
    void *p_CR_buf_33;
    void *p_CR_buf_34;
    void *p_CR_buf_35;
    void *p_CR_buf_36;
    void *p_CR_buf_37;
    void *p_CR_buf_38;
    void *p_CR_buf_39;
    void *p_CR_buf_40;
    void *p_CR_buf_41;

    //S18       11*4 = 44 bytes, CV5 S18 should be start from 106, but I think it is ok not to change them.
    void *p_CR_buf_111;
    void *p_CR_buf_112;
    void *p_CR_buf_113;
    void *p_CR_buf_114;
    void *p_CR_buf_115;
    void *p_CR_buf_116;
    void *p_CR_buf_117;
    void *p_CR_buf_118;
    void *p_CR_buf_119;
    void *p_CR_buf_120;
    void *p_CR_buf_121;
} amba_ik_mono7_flow_tables_list_t;

typedef struct {
    amba_ik_r2y_flow_tables_list_t r2y; // mono2 // step1
    amba_ik_motion_me1_flow_tables_list_t motion_me1; // step2
    amba_ik_motion_a_flow_tables_list_t motion_a; // step3
    amba_ik_motion_b_flow_tables_list_t motion_b; // step4
    amba_ik_motion_c_flow_tables_list_t motion_c; // step5
#if SUPPORT_FUSION
    amba_ik_r2y_flow_tables_list_t mono1; // step6
    amba_ik_mono3_flow_tables_list_t mono3; // step7
    amba_ik_mono4_flow_tables_list_t mono4; // step8
    amba_ik_mono5_flow_tables_list_t mono5; // step9
    amba_ik_mono6_flow_tables_list_t mono6; // step10
    amba_ik_mono7_flow_tables_list_t mono7; // step11
    amba_ik_r2y_flow_tables_list_t mono8; // step12
#endif
    //  40 bytes
    void *p_ca_warp_hor_red;
    void *p_ca_warp_hor_blue;
    void *p_ca_warp_ver_red;
    void *p_ca_warp_ver_blue;
    void *p_warp_hor;
    void *p_warp_ver;
    void *p_warp_hor_b;
    void *p_aaa;
    void *p_extra_window;
    void *p_frame_info;

    uint32 reserved[6];
} amba_ik_motion_fusion_flow_tables_list_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_7_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec4_cfg_buf[MAX_SEC_4_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec11_cfg_buf[MAX_SEC_11_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MAX_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_r2y_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_motion_me1_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_motion_a_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_motion_b_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_motion_c_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_mono3_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_mono4_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_mono5_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_mono6_flow_data_t;

typedef struct {
    uint8_t sec2_cfg_buf[MODE_8_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8_t sec18_cfg_buf[MODE_4_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
} amalgam_mono7_flow_data_t;

typedef struct {
    idsp_flow_ctrl_t flow_ctrl;
    uint8 reserved0[128u-(sizeof(idsp_flow_ctrl_t)%128u)]; // make sure section config buffers are 128 alligned
    amalgam_r2y_flow_data_t r2y_cfg_buf;
    amalgam_motion_me1_flow_data_t motion_me1_cfg_buf;
    amalgam_motion_a_flow_data_t motion_a_cfg_buf;
    amalgam_motion_b_flow_data_t motion_b_cfg_buf;
    amalgam_motion_c_flow_data_t motion_c_cfg_buf;
#if SUPPORT_FUSION
    amalgam_r2y_flow_data_t mono1_cfg_buf;
    amalgam_mono3_flow_data_t mono3_cfg_buf;
    amalgam_mono4_flow_data_t mono4_cfg_buf;
    amalgam_mono5_flow_data_t mono5_cfg_buf;
    amalgam_mono6_flow_data_t mono6_cfg_buf;
    amalgam_mono7_flow_data_t mono7_cfg_buf;
    amalgam_r2y_flow_data_t mono8_cfg_buf;
#endif

    idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X];
    uint8 reserved1[(PAD_ALIGN_SIZE)-(((uint32)MAX_TILE_NUM_X * (uint32)MAX_TILE_NUM_Y * sizeof(idsp_aaa_data_t))%(PAD_ALIGN_SIZE))];
    uint8_t sbp_map[IK_AMALGAM_TABLE_SBP_SIZE];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_hor_b[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    idsp_extra_window_info_t extra_window_info;
} amalgam_motion_fusion_flow_data_t;

typedef struct {
    amalgam_motion_fusion_flow_data_t amalgam_data;
    uint8 reserved0[128u - (sizeof(amalgam_motion_fusion_flow_data_t) % 128u)];

    // ctx debug
    ik_ctx_debug_data_t ctx_debug_data;
} ik_motion_fusion_debug_data_t;

#endif

