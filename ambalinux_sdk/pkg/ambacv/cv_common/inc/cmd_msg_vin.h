/*
 * Copyright (c) 2022-2022 Ambarella International LP
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

#ifndef CMD_MSG_VIN_H_FILE
#define CMD_MSG_VIN_H_FILE

#include "ambint.h"

#define MAX_ROI_CMD_TAG_NUM     8
#define MAX_TOKEN_ARRAY         4
#define MAX_IDSP_HALF_OCTAVES   13

/* Sync counters - VISORC scheduler reserves counters 240..255 */
#define VISORC_SCHEDULER_DOORBELL_VIN    255

#define MSG_VIN_SENSOR_RAW_CAPTURE_STATUS     0x84000001U
typedef struct
{
  // W0
  uint32_t    msg_code;
  // W1
  uint32_t    msg_crc;
 
  // W2
  uint32_t    vin_id              : 8;
  uint32_t    raw_capture_status  : 8; // refer to vin_raw_capture_status_t
  uint32_t    is_capture_time_out : 1;
  uint32_t    rpt_frm_cntrl       : 4;
  uint32_t    is_external_raw_buf_from_system : 1;
  uint32_t    is_external_ce_buf_from_system  : 1;
  uint32_t    raw_cap_buf_ch_fmt  : 4; // for send to enc case, 0:YUV_MONO, 1:YUV_420, 2:YUV_422
  uint32_t    is_ext_aux_out_buf_from_system  : 1;
  uint32_t    is_skip_vproc       : 1;
  uint32_t    vin_pipeline_id     : 1;
  uint32_t    is_fb_empty         : 1;
  uint32_t    is_output_q_full    : 1;
 
  // W3
  uint32_t    raw_cap_cnt;
 
  // W4
  uint32_t    raw_cap_buf_addr;
 
  // W5
  uint16_t    raw_cap_buf_pitch;
  uint16_t    raw_cap_buf_width;
 
  // W6
  uint16_t    raw_cap_buf_height;
  uint16_t    rsvd1;
 
  // W7
  uint32_t    raw_cap_buf_fid;
 
  // W8
  uint32_t    pts;
 
  // W9
  uint32_t    pts_diff;
 
  // W10 - w12
  //batch_cmd_set_info_t batch_cmd_set_info;
  uint32_t    batch_cmd_set_info_raw[3];
 
  // W13
  //event_mask_t event_mask;
  uint32_t    event_mask_raw;
 
  // W14
  uint32_t    config_addr;
 
  // W15
  uint32_t    ce_cap_buf_fid;
 
  // W16
  uint32_t    ce_cap_buf_addr;
 
  // W17
  uint16_t    ce_cap_buf_pitch;
  uint16_t    ce_cap_buf_width;
 
  // W18
  uint16_t    ce_cap_buf_height;
  uint16_t    rsvd2;
 
  // W19
  uint32_t    frm_vproc_dalay_time_ticks;
 
  // W20
  uint32_t    raw_repeat_default_buf_addr;  // luma part for yuv422 case
                                            
  // W21
  uint32_t    ce_repeat_default_buf_addr;   // chroma part for yuv422 case
 
  // W22
  uint8_t  raw_cap_buf_num_tot; // yuv buf if yuv422 capture case
  uint8_t  raw_cap_buf_num_free; // yuv buf if yuv422 capture case
  uint8_t  ce_cap_buf_num_tot;
  uint8_t  ce_cap_buf_num_free;
 
  // W23
  uint32_t  current_cap_slice_idx       :4;  // slice capture
  uint32_t  cap_slice_num               :4;  // slice capture
  uint32_t  temporal_demux_tar_vin_id   :8;
  uint32_t  pattern_demux_tar_num       :4;
  uint32_t  rsvd3                       :12;
 
  // W24
  uint32_t  aux_out_cap_buf_fid;
 
  // W25
  uint32_t  aux_out_cap_buf_addr;
 
  // W26
  uint16_t  aux_out_cap_buf_pitch;
  uint16_t  aux_out_cap_buf_width;
 
  // W27
  uint16_t  aux_out_cap_buf_height;
  uint8_t   aux_out_cap_buf_num_tot;
  uint8_t   aux_out_cap_buf_num_free;

#ifdef CHIP_CV6
  // W28
  uint32_t  hds_out_cap_buf_fid;
 
  // W29
  uint32_t  hds_out_cap_buf_addr;
 
  // W30
  uint16_t  hds_out_cap_buf_pitch;
  uint16_t  hds_out_cap_buf_width;
 
  // W31
  uint16_t  hds_out_cap_buf_height;
  uint8_t   hds_out_cap_buf_num_tot;
  uint8_t   hds_out_cap_buf_num_free;
 
  // W32
  uint32_t  hds_repeat_default_buf_addr;  
  
  // W33
  u64_globaladdr_upper_t  raw_cap_buf_addr_upper;
 
  // W34
  u64_globaladdr_upper_t  config_addr_upper;
 
  // W35
  u64_globaladdr_upper_t  ce_cap_buf_addr_upper;
 
  // W36
  u64_globaladdr_upper_t  aux_out_cap_buf_addr_upper;
 
  // W37
  u64_globaladdr_upper_t  hds_out_cap_buf_addr_upper;
 
  // W38
  u64_globaladdr_upper_t  raw_repeat_default_buf_addr_upper;  
                                            
  // W39
  u64_globaladdr_upper_t  ce_repeat_default_buf_addr_upper;   
 
  // W40
  u64_globaladdr_upper_t  hds_repeat_default_buf_addr_upper;     

  uint32_t  align_256b[23]; // Alignment to 256b

#else /* !CHIP_CV6 */
  // W28
  uint32_t  prev_out_cap_buf_fid;
 
  // W29
  uint32_t  prev_out_cap_buf_addr;
 
  // W30
  uint16_t  prev_out_cap_buf_pitch;
  uint16_t  prev_out_cap_buf_width;
 
  // W31
  uint16_t  prev_out_cap_buf_height;
  uint8_t   prev_out_cap_buf_num_tot;
  uint8_t   prev_out_cap_buf_num_free;
#endif /* ?CHIP_CV6 */
} msg_vin_sensor_raw_capture_status_t;
 
#ifdef CHIP_CV6
typedef struct {
  // First 128 bytes
    uint32_t                base_daddr;
    uint32_t                max_num_msg;
    uint32_t                read_index;
    uint32_t                reserved[29];

    // Second 128 bytes
    uint32_t                write_index;
    uint32_t                base_daddr_cpy;
    uint32_t                max_num_msg_cpy;
    uint32_t                reserved2[29];
} vin_msg_q_info_t; // Similar to dsp_msg_q_info_t, except [write/read]_index are split on different 128-byte cache lines.

#else /* !CHIP_CV6 */

typedef struct {
    uint32_t                base_daddr;
    uint32_t                max_num_msg;
    uint32_t                read_index;
    uint32_t                write_index;
    uint32_t                reserved[12];
} vin_msg_q_info_t; // same as dsp_msg_q_info_t

#endif /* ?CHIP_CV6 */
 
#endif /* !CMD_MSG_VIN_H_FILE */



