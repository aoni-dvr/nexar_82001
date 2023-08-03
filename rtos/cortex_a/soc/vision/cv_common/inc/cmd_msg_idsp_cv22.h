/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IDSP_VIS_MSG_H_FILE
#define IDSP_VIS_MSG_H_FILE

#include "ambint.h"

#define MAX_ROI_CMD_TAG_NUM     8
#define MAX_TOKEN_ARRAY         4
#define MAX_IDSP_HALF_OCTAVES   13

/* Sync counters - VISORC scheduler reserves counters 112..127 */
#define VISORC_SCHEDULER_DOORBELL       127

typedef struct { /* sideband_info_s */
    uint32_t                fov_id;
    uint32_t                fov_private_info_addr;
} sideband_info_t;

typedef struct { /* image_buffer_desc_s */
    uint32_t                buffer_addr;
    uint16_t                buffer_pitch;
    uint16_t                img_width;
    uint16_t                img_height;
    uint16_t                reserved_0;
} image_buffer_desc_t;

typedef struct { /* image_offset_s */
    int16_t                 x_offset;
    int16_t                 y_offset;
} image_offset_t;

typedef struct { /* msg_vp_hier_out_lane_out_header_s */
    uint16_t                channel_id;
    uint16_t                reserved;
    uint32_t                cap_seq_no;
    uint32_t                batch_cmd_id;
    sideband_info_t         side_band_info[MAX_TOKEN_ARRAY];
    uint32_t                roi_cmd_tag[MAX_ROI_CMD_TAG_NUM];
    uint16_t                ln_det_hier_mask;
    uint16_t                is_hier_out_tile_mode;
    uint32_t                vin_cap_time;
    uint32_t                proc_roi_cmd_time;
    uint32_t                c2y_done_time;
    uint32_t                warp_mctf_done_time;
} idsp_vis_picinfo_hdr_t;

typedef struct { /* msg_vp_hier_out_lane_out_s */
    idsp_vis_picinfo_hdr_t  header;
    image_buffer_desc_t     luma_hier_outs[MAX_IDSP_HALF_OCTAVES];
    image_buffer_desc_t     chroma_hier_outs[MAX_IDSP_HALF_OCTAVES];
    image_offset_t          hier_out_offsets[MAX_IDSP_HALF_OCTAVES];
    image_buffer_desc_t     luma_lane_det_out;
    image_buffer_desc_t     chroma_lane_det_out;
    image_offset_t          lane_det_out_offset;
    uint32_t                padding[5];
    uint32_t                reserved_for_epi_daddr;   /* Reserved for extended pic info */
} idsp_vis_picinfo_t;

typedef struct {
    uint32_t                base_daddr;
    uint32_t                max_num_msg;
    uint32_t                read_index;
    uint32_t                write_index;
    uint32_t                reserved[12];
} dsp_msg_q_info_t;

#endif /* !IDSP_VIS_MSG_H_FILE */

