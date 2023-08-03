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

#ifndef IDSP_IK_IMGKNL_IF_H_
#define IDSP_IK_IMGKNL_IF_H_


#define IK_MAX_TILE_NUM_X 4u
#define IK_MAX_TILE_NUM_Y 8u
#define MAX_IMG_STEP 1

#define DEBUG_AREA_SZ           (256U)
#define IDSP_CONFIG_HDR_SIZE    DEBUG_AREA_SZ

#define SEC_2_CRS_COUNT    38U
#define SEC_3_CRS_COUNT    3U
#define SEC_4_CRS_COUNT    8U
#define SEC_18_CRS_COUNT   11U

/* ===================================================== */
typedef enum {
    IK_SECT_CFG_INDEX_SEC2=0u,
    IK_SECT_CFG_INDEX_SEC3=1u,
    IK_SECT_CFG_INDEX_SEC4=2u,
    IK_SECT_CFG_INDEX_SEC18=3u,
    IK_SECT_CFG_INDEX_SEC5=4u,
    IK_SECT_CFG_INDEX_SEC6=5u,
    IK_SECT_CFG_INDEX_SEC7=6u,
    IK_SECT_CFG_INDEX_SEC9=7u,
    IK_SECT_CFG_INDEX_TOTAL=8u,
} IK_SECT_CFG_INDEX;

typedef enum {
    IK_SECT_CFG_MASK_SEC2 = (1U << IK_SECT_CFG_INDEX_SEC2),
    IK_SECT_CFG_MASK_SEC3 = (1U << IK_SECT_CFG_INDEX_SEC3),
    IK_SECT_CFG_MASK_SEC4 = (1U << IK_SECT_CFG_INDEX_SEC4),
    IK_SECT_CFG_MASK_SEC18= (1U << IK_SECT_CFG_INDEX_SEC18),
    IK_SECT_CFG_MASK_SEC5 = (1U << IK_SECT_CFG_INDEX_SEC5),
    IK_SECT_CFG_MASK_SEC6 = (1U << IK_SECT_CFG_INDEX_SEC6),
    IK_SECT_CFG_MASK_SEC7 = (1U << IK_SECT_CFG_INDEX_SEC7),
    IK_SECT_CFG_MASK_SEC9 = (1U << IK_SECT_CFG_INDEX_SEC9),
} IK_SECT_CFG_MASK;

typedef struct {
    uint32 running_number:16; // bit0-bit15
    uint32 reserved:2; //
    uint32 ability:6;  //liner, liner+ce, hdr2x, hdr3x,y2y
    uint32 pipe:2;     //video, still
    uint32 ctx_id:6 ;  // Support 16 FOVs
} idsp_ik_cfg_id_t;

typedef struct {
    idsp_ik_cfg_id_t ik_cfg_id;
    uint32 struct_version;     /* version number */
    uint32 idsp_flow_addr;     /* dram address of this idsp_ik_flow_ctrl_t instance */
} idsp_ik_flow_header_t;

typedef struct {
    uint8 stitch_tile_idx_x;
    uint8 stitch_tile_idx_y;
    uint16 reserved;
    uint32 data[63];
} idsp_ik_aaa_data_t;

typedef struct {
    uint32 data0                              : 2;
    uint32 update_CA_warp                     : 1;
    uint32 update_lens_warp                   : 1;
    uint32 reserved0                          : 28;

    uint32 data1[4];

    // CA Warp tables
    uint32 cawarp_horizontal_table_addr_red;
    uint32 cawarp_vertical_table_addr_red;
    uint32 cawarp_horizontal_table_addr_blue;
    uint32 cawarp_vertical_table_addr_blue;

    // Lens Warp tables
    uint32 warp_horizontal_table_address;
    uint32 warp_vertical_table_address;
} idsp_ik_calib_data_t;

typedef struct {
    uint32 data0                        : 27;
    uint32 reserved0                    : 5;

    uint32 data1                        : 10;
    uint32 update_sbp                   : 1;
    uint32 update_vignette              : 1;
    uint32 data2                        : 4;
    uint32 system_mode                  : 8;
    uint32 system_ability               : 8;

    uint32 reserved1;
    uint32 data3[3];
    uint32 sbp_map_addr;     // SBP map dram address
} idsp_ik_flow_info_t;

typedef struct {
    uint32 data0                        : 2;
    uint32 update_chroma_radius         : 1;
    uint32 reserved                     : 29;
    uint32 data1[8];
} idsp_ik_window_info_t;

typedef struct {
    uint32 data[14];
} idsp_ik_phase_info_t;

typedef struct {
    /* Stitching Info */
    uint16 stitch_tile_num_x;
    uint16 stitch_tile_num_y;

    uint32 update_aaa_info : 1;
    uint32 reserved        : 31;
    uint32 data[12];

    /* dram address to idsp_ik_aaa_data_t aaa[IK_MAX_TILE_NUM_Y][MAX_TILE_NUM_X] */
    uint32 aaa_info_daddr;
} idsp_ik_stitch_data_t;

typedef struct {
    uint32 total_num_of_steps :8;
    uint32 reserved : 24;
    uint16 step_cfg_mask[MAX_IMG_STEP];
} idsp_ik_step_info_t;

typedef struct {
    uint32 config_region_enable : 1;
    uint32 reserved             : 6;
    uint32 config_region_addr   : 25;
} ik_config_region_t;

typedef struct {
    ik_config_region_t sec2_crs[SEC_2_CRS_COUNT];
    ik_config_region_t sec3_crs[SEC_3_CRS_COUNT];
    ik_config_region_t sec4_crs[SEC_4_CRS_COUNT];
    ik_config_region_t sec18_crs[SEC_18_CRS_COUNT];
    uint32 padding[3];
#if 0
    ik_config_region_t sec5_crs[SEC_5_CRS_COUNT];
    ik_config_region_t sec6_crs[SEC_6_CRS_COUNT];
    ik_config_region_t sec7_crs[SEC_7_CRS_COUNT];
    ik_config_region_t sec9_crs[SEC_9_CRS_COUNT];
#endif
} idsp_ik_step_crs_t;

typedef struct {
    idsp_ik_flow_header_t header;
    idsp_ik_flow_info_t flow_info;
    idsp_ik_window_info_t window;
    idsp_ik_phase_info_t phase;
    idsp_ik_calib_data_t calib;
    idsp_ik_stitch_data_t stitch;
    idsp_ik_step_info_t step_info;
    uint32 reserved;
    idsp_ik_step_crs_t step_crs[MAX_IMG_STEP];
} idsp_ik_flow_ctrl_t;

typedef struct {
    uint32 calib_update       : 1;
    uint32 window_info_update : 1;
    uint32 phase_info_update  : 1;
    uint32 aaa_stats_update   : 1;
    uint32 reserved           : 28;

    idsp_ik_calib_data_t calib;
    idsp_ik_window_info_t window;
    idsp_ik_phase_info_t phase;
    idsp_ik_aaa_data_t aaa;
} idsp_ik_group_update_info_t;

#endif // IDSP_IK_IMGKNL_IF_H_