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
#include "idspdrv_ik_imgknl_if.h"
#include "AmbaDSP_ImgContextEntity.h"


#define PAD_ALIGN_SIZE (32UL)


#define CR_SIZE_4                         (128U*4U)
#define CR_SIZE_5                         (128U*5U)
#define CR_SIZE_6                         (128U*1U)
#define CR_SIZE_7                         (128U*1U)
#define CR_SIZE_8                         (128U*1U)
#define CR_SIZE_9                         (128U*1U)
#define CR_SIZE_10                        (128U*64U)
#define CR_SIZE_11                        (128U*3U)
#define CR_SIZE_12                        (128U*2U)
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

#if (!defined(EARLYTEST_ENV) && !defined(IK_GCOVR_ENV))
#define BIN_CFG_START_OFFSET    (128U + 0x400u)
#else
#define BIN_CFG_START_OFFSET    128U
#endif
#define SEC_2_BIN_CFG_OFFSET    (128U*9U)
#define SEC_3_BIN_CFG_OFFSET    (128U*617U)
#define SEC_4_BIN_CFG_OFFSET    (128U*876U)
#define SEC_18_BIN_CFG_OFFSET   (128U*1064U)

#define MAX_SEC_2_CFG_SZ        (128U*608U)
#define MAX_SEC_3_CFG_SZ        (128U*259U)
#define MAX_SEC_4_CFG_SZ        (128U*96U)
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
    uint8 CR_buf_4[CR_SIZE_4];
    uint8 CR_buf_5[CR_SIZE_5];
    uint8 CR_buf_6[CR_SIZE_6];
    uint8 CR_buf_7[CR_SIZE_7];
    uint8 CR_buf_8[CR_SIZE_8];
    uint8 CR_buf_9[CR_SIZE_9];
    uint8 CR_buf_10[CR_SIZE_10];
    uint8 CR_buf_11[CR_SIZE_11];
    uint8 CR_buf_12[CR_SIZE_12];
    uint8 CR_buf_13[CR_SIZE_13];
    uint8 CR_buf_14[CR_SIZE_14];
    uint8 CR_buf_15[CR_SIZE_15];
    uint8 CR_buf_16[CR_SIZE_16];
    uint8 CR_buf_17[CR_SIZE_17];
    uint8 CR_buf_18[CR_SIZE_18];
    uint8 CR_buf_19[CR_SIZE_19];
    uint8 CR_buf_20[CR_SIZE_20];
    uint8 CR_buf_21[CR_SIZE_21];
    uint8 CR_buf_22[CR_SIZE_22];
    uint8 CR_buf_23[CR_SIZE_23];
    uint8 CR_buf_24[CR_SIZE_24];
    uint8 CR_buf_25[CR_SIZE_25];
    uint8 CR_buf_26[CR_SIZE_26];
    uint8 CR_buf_27[CR_SIZE_27];
    uint8 CR_buf_28[CR_SIZE_28];
    uint8 CR_buf_29[CR_SIZE_29];
    uint8 CR_buf_30[CR_SIZE_30];
    uint8 CR_buf_31[CR_SIZE_31];
    uint8 CR_buf_32[CR_SIZE_32];
    uint8 CR_buf_33[CR_SIZE_33];
    uint8 CR_buf_34[CR_SIZE_34];
    uint8 CR_buf_35[CR_SIZE_35];
    uint8 CR_buf_36[CR_SIZE_36];
    uint8 CR_buf_37[CR_SIZE_37];
    uint8 CR_buf_38[CR_SIZE_38];
    uint8 CR_buf_39[CR_SIZE_39];
    uint8 CR_buf_40[CR_SIZE_40];
    uint8 CR_buf_41[CR_SIZE_41];

    // S3
    uint8 CR_buf_42[CR_SIZE_42];
    uint8 CR_buf_43[CR_SIZE_43];
    uint8 CR_buf_44[CR_SIZE_44];

    // S4
    uint8 CR_buf_45[CR_SIZE_45];
    uint8 CR_buf_46[CR_SIZE_46];
    uint8 CR_buf_47[CR_SIZE_47];
    uint8 CR_buf_48[CR_SIZE_48];
    uint8 CR_buf_49[CR_SIZE_49];
    uint8 CR_buf_50[CR_SIZE_50];
    uint8 CR_buf_51[CR_SIZE_51];
    uint8 CR_buf_52[CR_SIZE_52];

    // S18
    uint8 CR_buf_111[CR_SIZE_111];
    uint8 CR_buf_112[CR_SIZE_112];
    uint8 CR_buf_113[CR_SIZE_113];
    uint8 CR_buf_114[CR_SIZE_114];
    uint8 CR_buf_115[CR_SIZE_115];
    uint8 CR_buf_116[CR_SIZE_116];
    uint8 CR_buf_117[CR_SIZE_117];
    uint8 CR_buf_118[CR_SIZE_118];
    uint8 CR_buf_119[CR_SIZE_119];
    uint8 CR_buf_120[CR_SIZE_120];
    uint8 CR_buf_121[CR_SIZE_121];

    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    uint32 fence_0[16];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    uint32 fence_1[16];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    uint32 fence_2[16];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    uint32 fence_3[16];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    uint32 fence_4[16];
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    uint32 fence_5[16];
    idsp_ik_aaa_data_t aaa[IK_MAX_TILE_NUM_Y][IK_MAX_TILE_NUM_X];
    uint32 fence_6[16];

    ik_query_frame_info_t frame_info;
    uint32 fence_7[16];
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

    //S18       11*4 = 44 bytes
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

    //  32 bytes
    void *p_ca_warp_hor_red;
    void *p_ca_warp_hor_blue;
    void *p_ca_warp_ver_red;
    void *p_ca_warp_ver_blue;
    void *p_warp_hor;
    void *p_warp_ver;
    void *p_aaa;
    void *p_frame_info;

    // 48 bytes
    uint32 reserved[12];
} amba_ik_flow_tables_list_t; // please make sure this is 64 alignment


// for amalgam...

// Following should be updated for newer chips.
#define IK_AMALGAM_TABLE_SBP_SIZE 3686400UL

typedef struct {
    idsp_ik_flow_ctrl_t flow_ctrl;
    uint8 reserved0[128u-(sizeof(idsp_ik_flow_ctrl_t)%128u)]; // make sure section config buffers are 128 alligned
    uint8 sec2_cfg_buf[MAX_SEC_2_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8 sec3_cfg_buf[MAX_SEC_3_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8 sec4_cfg_buf[MAX_SEC_4_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    uint8 sec18_cfg_buf[MAX_SEC_18_CFG_SZ + IDSP_CONFIG_HDR_SIZE];
    idsp_ik_aaa_data_t aaa[IK_MAX_TILE_NUM_Y][IK_MAX_TILE_NUM_X];
    uint8 reserved1[(PAD_ALIGN_SIZE)-(((uint32)IK_MAX_TILE_NUM_X * (uint32)IK_MAX_TILE_NUM_Y * sizeof(idsp_ik_aaa_data_t))%(PAD_ALIGN_SIZE))];
    uint8 sbp_map[IK_AMALGAM_TABLE_SBP_SIZE];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
} amalgam_flow_data_t;

typedef struct {
    amalgam_flow_data_t amalgam_data;
    uint8 reserved0[128u - (sizeof(amalgam_flow_data_t) % 128u)];

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
    ik_grid_point_t calib_warp[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2UL];

    // TBD

} ik_debug_data_t;

typedef struct {
    idsp_ik_group_update_info_t group_update_info;
    uint8 reserved0[128u-(sizeof(idsp_ik_group_update_info_t)%128u)];
    int16 ca_warp_hor_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_hor_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_red[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 ca_warp_ver_blue[CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID];
    int16 warp_hor[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
    int16 warp_ver[WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U]; // multi_slice
} amalgam_group_update_data_t;

typedef struct {
    uint32 frame_crc32;
    uint32 flow_ctrl;
    uint32 sec2_cfg_buf[SEC_2_CRS_COUNT];
    uint32 sec3_cfg_buf[SEC_3_CRS_COUNT];
    uint32 sec4_cfg_buf[SEC_4_CRS_COUNT];
    uint32 sec18_cfg_buf[SEC_18_CRS_COUNT];
    uint32 aaa[IK_MAX_TILE_NUM_Y][IK_MAX_TILE_NUM_X];
    uint32 ca_warp_hor_red;
    uint32 ca_warp_hor_blue;
    uint32 ca_warp_ver_red;
    uint32 ca_warp_ver_blue;
    uint32 warp_hor;
    uint32 warp_ver;
    uint32 reserved0[4];
} idsp_crc_data_t;

#endif

