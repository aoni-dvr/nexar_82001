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

#ifndef AMBA_IK_CFA_HISTO_COUNT
#define AMBA_IK_CFA_HISTO_COUNT (64U)
#endif

#ifndef AMBA_IK_RGB_HISTO_COUNT
#define AMBA_IK_RGB_HISTO_COUNT (64U)
#endif

#ifndef AMBA_IK_HDR_HISTO_COUNT
#define AMBA_IK_HDR_HISTO_COUNT (128U)
#endif

// ===================== get statistic======================//
#ifndef AMBA_IK_3A_AWB_TILE_COLUMN_COUNT
#define AMBA_IK_3A_AWB_TILE_COLUMN_COUNT       64U
#endif

#ifndef AMBA_IK_3A_AWB_TILE_ROW_COUNT
#define AMBA_IK_3A_AWB_TILE_ROW_COUNT          64U
#endif

#ifndef AMBA_IK_3A_AE_TILE_COLUMN_COUNT
#define AMBA_IK_3A_AE_TILE_COLUMN_COUNT        24U
#endif

#ifndef AMBA_IK_3A_AE_TILE_ROW_COUNT
#define AMBA_IK_3A_AE_TILE_ROW_COUNT           16U
#endif

#ifndef AMBA_IK_3A_CFA_AF_TILE_COLUMN_COUNT
#define AMBA_IK_3A_CFA_AF_TILE_COLUMN_COUNT        24U
#endif

#ifndef AMBA_IK_3A_PG_AF_TILE_COLUMN_COUNT
#define AMBA_IK_3A_PG_AF_TILE_COLUMN_COUNT        32U
#endif

#ifndef AMBA_IK_3A_AF_TILE_ROW_COUNT
#define AMBA_IK_3A_AF_TILE_ROW_COUNT           16U
#endif

#ifndef AMBA_IK_3A_SLICE_MAX_COUNT
#define AMBA_IK_3A_SLICE_MAX_COUNT             32U
#endif

typedef struct {
    uint32 awb_enable:1;
    uint32 awb_tile_num_col:6;
    uint32 awb_tile_num_row:6;
    uint32 skip0:19;
    uint32 awb_tile_col_start:13;
    uint32 awb_tile_row_start:13;
    uint32 skip1:6;
    uint32 awb_tile_width:9;
    uint32 awb_tile_height:9;
    uint32 skip2:14;
    uint32 awb_tile_active_width:9;
    uint32 awb_tile_active_height:9;
    uint32 skip3:14;
} __attribute__((packed)) idsp_awb_data_t;

typedef struct {
    uint32 ae_enable:1;
    uint32 ae_tile_num_col:5;
    uint32 ae_tile_num_row:4;
    uint32 skip6:22;
    uint32 ae_tile_col_start:13;
    uint32 ae_tile_row_start:13;
    uint32 skip7:6;
    uint32 ae_tile_width:9;
    uint32 ae_tile_height:9;
    uint32 skip8:14;
    uint32 ae_min_thresh:14;
    uint32 ae_max_thresh:14;
    uint32 skip9:4;
} __attribute__((packed)) idsp_cfa_ae_data_t;

typedef struct {
    uint32 ae_enable:1;
    uint32 ae_tile_num_col:5;
    uint32 ae_tile_num_row:4;
    uint32 skip6:22;
    uint32 ae_tile_col_start:13;
    uint32 ae_tile_row_start:13;
    uint32 skip7:6;
    uint32 ae_tile_width:9;
    uint32 ae_tile_height:9;
    uint32 skip8:14;
    uint32 ae_tile_y_shift:5;
    uint32 skip3:27;
} __attribute__((packed)) idsp_pg_ae_data_t;

typedef struct {
    uint32 af_enable:1;
    uint32 af_tile_num_col:5;
    uint32 af_tile_num_row:4;
    uint32 skip11:22;
    uint32 af_tile_col_start:13;
    uint32 af_tile_row_start:13;
    uint32 skip12:6;
    uint32 af_tile_width:10;
    uint32 af_tile_height:10;
    uint32 skip13:12;
    uint32 af_tile_active_width:9;
    uint32 af_tile_active_height:9;
    uint32 skip14:14;
} __attribute__((packed)) idsp_af_data_t;

typedef struct {
    uint32 bayer_pattern  : 2; /* reg 0 */
    uint32 ir_mode        : 2;
    uint32 crop_en        : 1;
    uint32 reserved0      : 27;
    uint32 crop_col_start : 13; /* reg 1 */
    uint32 crop_row_start : 13;
    uint32 reserved1      : 6;
    uint32 crop_width     : 13; /* reg 2 */
    uint32 crop_height    : 13;
    uint32 reserved2      : 6;
} __attribute__((packed)) vin_stat_exp_regs_t;

typedef  struct {
    uint8 stitch_tile_idx_x;
    uint8 stitch_tile_idx_y;
    uint16 reserved;
    vin_stat_exp_regs_t vin_histogram; // same for each exposure
    idsp_awb_data_t     awb;
    idsp_cfa_ae_data_t  cfa_ae;
    idsp_af_data_t      cfa_af;
    idsp_pg_ae_data_t   pg_ae;
    idsp_af_data_t      pg_af;
    uint32 cfa_ae_tile_histogram_mask[16];
    uint32 pg_ae_tile_histogram_mask[16];

    // the shift parameters
    /* AWB */
    uint16 awb_rgb_shift;
    uint16 awb_min_max_shift;
    /* AE */
    uint16 pg_ae_y_shift;
    uint16 ae_linear_y_shift;
    uint16 ae_min_max_shift;
    /* AF */
    uint16 pg_af_y_shift;
    uint16 af_cfa_y_shift;

    uint8 chan_index;
    uint8  vin_stats_type;    // 0: main; 1: hdr; 2:hdr2
    uint32 raw_pic_seq_num;  // sequence number of raw picture used in producing this block of AAA results
    uint8 aaa_cfa_mux_sel; // 1: before_CE, 2: pre_IR
    uint8 reserved2;
    uint16 reserved3[5];
} __attribute__((packed)) ik_3a_header_t;

// cfa awb
typedef struct {
    uint16  sum_r;
    uint16  sum_g;
    uint16  sum_b;
    uint16  sum_ir;
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
    ik_cfa_awb_t        awb[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COLUMN_COUNT];
    ik_cfa_ae_t         ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COLUMN_COUNT];
    ik_cfa_af_t         af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_CFA_AF_TILE_COLUMN_COUNT];
    ik_cfa_histo_t      histogram;
} __attribute__((packed)) ik_cfa_3a_data_t;

//pg_aaa_stat_t
typedef struct {
    ik_3a_header_t     header;
    uint16                  frame_id;
    ik_pg_af_t        af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_PG_AF_TILE_COLUMN_COUNT];
    ik_pg_ae_t        ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COLUMN_COUNT];
    ik_pg_histo_t     histogram;
} __attribute__((packed)) ik_pg_3a_data_t;

typedef struct {
    uint32 histo_bin_r[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_g[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_b[AMBA_IK_HDR_HISTO_COUNT];
    uint32 histo_bin_ir[AMBA_IK_HDR_HISTO_COUNT];
} __attribute__((packed)) ik_cfa_histogram_t;

typedef struct {
    ik_3a_header_t header;
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
