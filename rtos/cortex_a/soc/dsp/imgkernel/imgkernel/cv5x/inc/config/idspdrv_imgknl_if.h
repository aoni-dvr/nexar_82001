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


#ifndef IDSP_IMGKNL_IF_H_
#define IDSP_IMGKNL_IF_H_

#define IDSPDRV_IMGKNL_IF_VERSION 0x21070105 // major : YYMMDD, minor : xx, change major only if not backward compatible.

#define MAX_TILE_NUM_X 12u
#define MAX_TILE_NUM_Y 8u
#define MAX_IMG_STEP  16u

#define DEBUG_AREA_SZ           (256U)
#define IDSP_CONFIG_HDR_SIZE    DEBUG_AREA_SZ

#define SEC_2_CRS_COUNT    38U
#define SEC_3_CRS_COUNT    3U
#define SEC_4_CRS_COUNT    8U
#define SEC_11_CRS_COUNT   4U
#define SEC_18_CRS_COUNT   11U

/* ===================================================== */
typedef enum {
    SECT_CFG_INDEX_SEC2=0u,
    SECT_CFG_INDEX_SEC3=1u,
    SECT_CFG_INDEX_SEC4=2u,
    SECT_CFG_INDEX_SEC11=3u,
    SECT_CFG_INDEX_SEC18=4u,
    SECT_CFG_INDEX_TOTAL=5u,
} SECT_CFG_INDEX;

typedef enum {
    SECT_CFG_MASK_SEC2 = (1U << SECT_CFG_INDEX_SEC2),
    SECT_CFG_MASK_SEC3 = (1U << SECT_CFG_INDEX_SEC3),
    SECT_CFG_MASK_SEC4 = (1U << SECT_CFG_INDEX_SEC4),
    SECT_CFG_MASK_SEC11= (1U << SECT_CFG_INDEX_SEC11),
    SECT_CFG_MASK_SEC18 = (1U << SECT_CFG_INDEX_SEC18),
} IK_SECT_CFG_MASK;

typedef struct {
    uint32_t running_number:16; // bit0-bit15
    uint32_t reserved:2; //
    uint32_t ability:6;  //liner, liner+ce, hdr2x, hdr3x,y2y
    uint32_t pipe:2;     //video, still
    uint32_t ctx_id:6 ;  // Support 16 FOVs
} ik_cfg_id_t;

typedef struct {
    ik_cfg_id_t ik_cfg_id;
    uint32_t struct_version;     /* version number */
    uint32_t idsp_flow_addr;     /* dram address of this idsp_flow_ctrl_t instance */
    uint32_t reserved;
} idsp_flow_header_t;

typedef struct {
    uint32_t data0                              : 3;
    uint32_t update_lens_warp                   : 1;
    uint32_t update_lens_warp_b                 : 1;
    uint32_t update_CA_warp                     : 1;
    uint32_t reserved0                          : 26;

    uint32_t data1[5];

    // CA Warp tables
    uint32_t cawarp_horizontal_table_addr_red;
    uint32_t cawarp_vertical_table_addr_red;
    uint32_t cawarp_horizontal_table_addr_blue;
    uint32_t cawarp_vertical_table_addr_blue;

    // Lens Warp tables
    uint32_t warp_horizontal_table_address;
    uint32_t warp_vertical_table_address;

    // Lens Warp b tables
    uint32_t warp_horizontal_b_table_address;

    uint32_t reserved1[7];
} idsp_calib_data_t;

typedef struct {
    uint8_t stitch_tile_idx_x;
    uint8_t stitch_tile_idx_y;
    uint16_t reserved0;
    uint32_t data[63];
} idsp_aaa_data_t;

typedef struct {
    uint32_t data0;

    uint32_t data1                        : 10;
    uint32_t update_sbp                   : 1;
    uint32_t update_vignette              : 1;
    uint32_t data2                       : 4;
    uint32_t reserved0                       : 16;

    uint32_t data3[3];
    uint32_t sbp_map_addr;     // SBP map dram address

    uint32_t data4;
    uint32_t data5                         : 10;
    uint32_t reserved1                     : 22;
    uint32_t reserved2[4];
} idsp_flow_info_t;

typedef struct {
    uint8 data[64];
} idsp_extra_window_info_t;

typedef struct {
    uint32_t data0                        : 2;
    uint32_t update_chroma_radius         : 1;
    uint32_t use_hdr_dummy_crop_window    : 1;
    uint32_t reserved                     : 28;
    uint32_t data1[9];

    uint32_t extra_window_info_addr; // dram address to idsp_extra_window_info_t

    uint32_t reserved1[5];
} idsp_window_info_t;

typedef struct {
    uint32_t data[16];

    uint32_t prescale_update :1;
    uint32_t main_update     :1;
    uint32_t vwarp_update    :1;
    uint32_t hwarp_b_update  :1;
    uint32_t pos_dep_update  :1;
    uint32_t reserved0        :27;
    uint32_t reserved1[7];
} idsp_phase_info_t;

typedef struct {
    /* Stitching Info */
    uint16_t stitch_tile_num_x;
    uint16_t stitch_tile_num_y;

    uint32_t data[21];

    /* dram address to idsp_aaa_data_t aaa[MAX_TILE_NUM_Y][MAX_TILE_NUM_X] */
    uint32_t aaa_info_daddr;

    uint32_t reserved1[9];
} idsp_stitch_data_t;

typedef struct {
    uint32_t data[5];
} idsp_hiso_data_t;

typedef struct {
    uint32_t data0 :24;
    uint32_t update_local_lighting : 1;
    uint32_t update_slow_mo_sensitivity :1;
    uint32_t data1 :6;

    uint32_t data2;
} idsp_motion_detect_t;

typedef struct {
    idsp_motion_detect_t opt_c;
} idsp_fusion_data_t;

typedef struct {
    union {
        uint32_t place_holder[8];
        idsp_hiso_data_t hiso;
        idsp_motion_detect_t opt_c;
        idsp_fusion_data_t fusion;
    };
} idsp_multi_pass_data_t;

typedef struct {
    uint32_t total_num_of_steps :8;
    uint32_t reserved0 : 24;
    uint8_t  step_cfg_mask[MAX_IMG_STEP]; // bitmap of which sections are used in each step
    idsp_multi_pass_data_t multi_pass;    // parameters for idspdrv_update_resampler_config()
    uint32_t reserved1[3];
} idsp_step_info_t;

typedef struct {
    uint32_t config_region_enable : 1;
    uint32_t reserved             : 6;
    uint32_t config_region_addr   : 25;
} config_region_t;

typedef struct {
    config_region_t sec2_crs[SEC_2_CRS_COUNT];
    config_region_t sec3_crs[SEC_3_CRS_COUNT];
    config_region_t sec4_crs[SEC_4_CRS_COUNT];
    config_region_t sec11_crs[SEC_11_CRS_COUNT];
    config_region_t sec18_crs[SEC_18_CRS_COUNT];
} idsp_step_crs_t;

typedef struct {
    idsp_flow_header_t header;
    idsp_flow_info_t flow_info;
    idsp_window_info_t window;
    idsp_phase_info_t phase;
    idsp_calib_data_t calib;
    idsp_stitch_data_t stitch;
    idsp_step_info_t step_info;
    idsp_step_crs_t step_crs[MAX_IMG_STEP];
    uint32_t reserved[4];
} idsp_flow_ctrl_t;

typedef struct {
    uint32_t calib_update       : 1;
    uint32_t window_info_update : 1;
    uint32_t phase_info_update  : 1;
    uint32_t stitch_data_update : 1;
    uint32_t aaa_stats_update   : 1;
    uint32_t reserved0          : 27;

    idsp_calib_data_t calib;
    idsp_window_info_t window;
    idsp_phase_info_t phase;
    idsp_stitch_data_t stitch;
    uint32_t reserved1[3];
} idsp_group_update_info_t;

#endif // IDSP_IMGKNL_IF_H_
