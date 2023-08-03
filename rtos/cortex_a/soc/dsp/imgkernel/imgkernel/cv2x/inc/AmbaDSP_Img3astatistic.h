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

#ifndef IMG_3A_STATiSTIC_H
#define IMG_3A_STATiSTIC_H
#include "ik_data_type.h"

#define AMBA_IK_CFA_HISTO_COUNT (64UL)
#define AMBA_IK_RGB_HISTO_COUNT (64UL)
#define AMBA_IK_HDR_HISTO_COUNT (128UL)

#define AMBA_IK_3A_AWB_TILE_COL_COUNT       32U
#define AMBA_IK_3A_AWB_TILE_ROW_COUNT          32U

#define AMBA_IK_3A_AE_TILE_COL_COUNT        12U
#define AMBA_IK_3A_AE_TILE_ROW_COUNT           8U

#define AMBA_IK_3A_AF_TILE_COL_COUNT        12U
#define AMBA_IK_3A_AF_TILE_ROW_COUNT           8U

#define AMBA_IK_3A_SLICE_MAX_COUNT             16U

#define AMBA_LL_3A_VIDEO    (0U)
#define AMBA_LL_3A_STILL    (1U)
#define AMBA_LL_3A_DEC      (2U)
#define AMBA_LL_HIST_VIDEO  (3U)
#define AMBA_LL_HIST_STILL  (4U)
#define AMBA_LL_HIST_DEC    (5U)

typedef  struct {
    //awb
    uint16  awb_tile_col_start;
    uint16  awb_tile_row_start;
    uint16  awb_tile_width;
    uint16  awb_tile_height;
    uint16  awb_tile_active_width;
    uint16  awb_tile_active_height;
    uint16  awb_rgb_shift;
    uint16  awb_y_shift;
    uint16  awb_min_max_shift;

    //ae
    uint16  ae_tile_col_start;
    uint16  ae_tile_row_start;
    uint16  ae_tile_width;
    uint16  ae_tile_height;
    uint16  ae_y_shift;
    uint16  ae_linear_y_shift;
    uint16  ae_min_max_shift;

    //af
    uint16  af_tile_col_start;
    uint16  af_tile_row_start;
    uint16  af_tile_width;
    uint16  af_tile_height;
    uint16  af_tile_active_width;
    uint16  af_tile_active_height;
    uint16  af_y_shift;
    uint16  af_cfa_y_shift;

    // AWB tiles
    uint8   awb_tile_num_col;
    uint8   awb_tile_num_row;
    // AE tiles
    uint8   ae_tile_num_col;
    uint8   ae_tile_num_row;
    // AF tiles
    uint8   af_tile_num_col;
    uint8   af_tile_num_row;

    // Strip info
    uint8   total_slices_x;
    uint8   total_slices_y;
    uint8   slice_index_x;
    uint8   slice_index_y;
    uint16  slice_width;
    uint16  slice_height;
    uint16  slice_start_x;

    uint32  slice_start_y:16;
    // Misc
    uint32 chan_index : 4;
    uint32 reserved1 : 12;

    uint32 raw_pic_seq_num;  // sequence number of raw picture used in producing this block of AAA results
    uint32 iso_config_tag;

    uint32 reserved2[13];  // max 128 bytes
} __attribute__((packed)) ik_3a_header_t;

// cfa awb
typedef struct {
    uint16  sum_r;
    uint16  sum_g;
    uint16  sum_b;
    uint16  count_min;
    uint16  count_max;
} __attribute__((packed)) ik_cfa_awb_t;
// cfa ae
typedef struct {
    uint16  lin_y;
    uint16  count_min;
    uint16  count_max;
} __attribute__((packed)) ik_cfa_ae_t;
// cfa af
typedef struct {
    uint16  sum_y;
    uint16  sum_fv1;
    uint16  sum_fv2;
} __attribute__((packed))  ik_cfa_af_t;
//cfa histogram
typedef  struct {
    uint32  his_bin_r[AMBA_IK_CFA_HISTO_COUNT];
    uint32  his_bin_g[AMBA_IK_CFA_HISTO_COUNT];
    uint32  his_bin_b[AMBA_IK_CFA_HISTO_COUNT];
    uint32  his_bin_y[AMBA_IK_CFA_HISTO_COUNT];
}  __attribute__((packed)) ik_cfa_histo_t;

typedef  struct {
    uint16  sum_y;
    uint16  sum_fv1;
    uint16  sum_fv2;
}  __attribute__((packed)) ik_pg_af_t;

// rgb ae
typedef struct {
    uint16  sum_y;
} __attribute__((packed)) ik_pg_ae_t;

// rgb histo
typedef  struct {
    uint32  his_bin_y[AMBA_IK_RGB_HISTO_COUNT];
    uint32  his_bin_r[AMBA_IK_RGB_HISTO_COUNT];
    uint32  his_bin_g[AMBA_IK_RGB_HISTO_COUNT];
    uint32  his_bin_b[AMBA_IK_RGB_HISTO_COUNT];
}  __attribute__((packed)) ik_pg_histo_t;

//cfa_aaa_stat_t
typedef struct {
    ik_3a_header_t      header;
    uint16                   frame_id;
    ik_cfa_awb_t        awb[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT];
    ik_cfa_ae_t         ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    ik_cfa_af_t         af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_AF_TILE_COL_COUNT];
    ik_cfa_histo_t      histogram;
} __attribute__((packed)) ik_cfa_3a_data_t;

//pg_aaa_stat_t
typedef struct {
    ik_3a_header_t     header;
    uint16                  frame_id;
    ik_pg_af_t        af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_AF_TILE_COL_COUNT];
    ik_pg_ae_t        ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    ik_pg_histo_t     histogram;
} __attribute__((packed)) ik_pg_3a_data_t;

typedef struct {
    uint8  vin_stats_type;    // 0: main; 1: hdr; 2:hdr2
    uint8  channel_index;
    uint8  total_exposures;
    uint8  blend_index;     // exposure no.

    uint32 reserved_1;

    uint16 stats_left;
    uint16 stats_width;
    uint16 stats_top;
    uint16 stats_height;

    uint32 reserved2[28];

} __attribute__((packed)) ik_cfa_histo_cfg_info_t;

typedef struct {
    uint32 histo_bin_r[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_g[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_b[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_ir[AMBA_IK_HDR_HISTO_COUNT];
} __attribute__((packed)) ik_cfa_histogram_t;

typedef struct {
    ik_cfa_histo_cfg_info_t cfg_info;
    ik_cfa_histogram_t cfa_hist;      /* Address of histogram statistic DSP_IMG_HDR_HIST_STAT_s */
} __attribute__((packed)) ik_cfa_histogram_stat_t;

typedef struct {
    uint8           Type;
    uint8           SrcSliceX;
    uint8           SrcSliceY;
    uintptr         SrcAddr[AMBA_IK_3A_SLICE_MAX_COUNT];
    uintptr         DstAddr;
    uint16          ChannelIdx;
} DSP_EVENT_3A_TRANSFER_s;


#endif
