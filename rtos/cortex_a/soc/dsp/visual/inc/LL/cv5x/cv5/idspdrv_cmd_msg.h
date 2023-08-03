//*  Ambarella Corporation Copyright 2018-  */
/**
 *  @file idspdrv_cmd_msg.h
 *
 *  Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef IDSPDRV_CMD_MSG_H_
#define IDSPDRV_CMD_MSG_H_

#include <dsp_types.h>
#define SET_VIN_CONFIG           0x09001001
#define SET_VIN_COMPRESSION      0x09001002
#define SET_VIN_HDS_COMPRESSION  0x09001003
#define SET_VIN_PREV_COMPRESSION 0x09001004
#define SET_VIN_MASTER_CLK       0x09001005
#define SET_VIN_GLOBAL_CFG       0x09001006

/************************************************************
 * IDSP commands (Category 9)
 */

/**
 * Set VIN Configuration (0x09001001)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id              : 6; // Vin1 (20) -Vin14 (33)
  uint32_t bayer_pattern       : 2;
  uint32_t ir_mode             : 2;
  uint32_t no_of_hdr_exposures : 2;
  uint32_t sensor_resolution   : 6;
  uint32_t decomp_en           : 1;
  uint32_t comp_en             : 1;
  uint32_t reserved            : 12;
  uint16_t vin_width;
  uint16_t vin_height;
  uint32_t vin_config_dram_addr;
  uint16_t vin_config_data_size;
  uint16_t reserved1;
  uint32_t decomp_lookup_table_daddr;
  uint32_t comp_lookup_table_daddr;

} set_vin_config_t;

/**
 * Lossy compression (0x09001002, 0x09001003, 0x09001004)
 */
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vin_id; // Vin1 (20) -Vin14 (33)
  uint8_t  enable;
  uint8_t  mantissa;
  uint8_t  block_size;

} lossy_compression_t;

/**
 * Set Master clock Configuration (0x00091005)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id   : 6; // Vin1 (20) -Vin14 (33)
  uint32_t reserved : 26;
  uint16_t master_sync_reg_word0;
  uint16_t master_sync_reg_word1;
  uint16_t master_sync_reg_word2;
  uint16_t master_sync_reg_word3;
  uint16_t master_sync_reg_word4;
  uint16_t master_sync_reg_word5;
  uint16_t master_sync_reg_word6;
  uint16_t master_sync_reg_word7;

} set_vin_master_t;

/**
 * Set Global Vin Configuration (0x00091006)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id   : 6; // Vin0-Vin1
  uint32_t reserved : 26;
  uint32_t global_cfg_reg_word0; /* word0 of the register values */

} set_vin_global_config_t;

typedef struct {
    uint32_t calib_update       : 1;
    uint32_t window_info_update : 1;
    uint32_t phase_info_update  : 1;
    uint32_t stitch_data_update : 1;
    uint32_t aaa_stats_update   : 1;
    uint32_t reserved0          : 27;

#if 0
    idsp_calib_data_t calib;    /* CA/Warp data */
    idsp_window_info_t window;
    idsp_phase_info_t phase;
    idsp_stitch_data_t stitch;
    int32_t reserved1[7];
#endif
} idsp_grp_upt_info_t; /* Fix structure name conflict: abbreviation from idsp_group_update_infot_t */

#endif // IDSPDRV_CMD_MSG_H_
