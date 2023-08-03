/**
 *  @file SvcWarp.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 *  @details svc warp
 *
 */


#ifndef SVC_WARP_H
#define SVC_WARP_H

#include "AmbaTypes.h"

#define SVC_TABLE_MAXSIZE (49152U)   // 128x96x4

#define SVC_OK      (0x00000000U)   /* OK */
#define SVC_NG      (0x00000001U)   /* NG */

#define SVC_ERR_0001 (1U)
#define SVC_ERR_GENERAL SVC_ERR_0001

#define SVC_VIN_AUTO      (0U)
#define SVC_VIN_UHD_2160  (1U)
#define SVC_VIN_UHD_1728  (2U)
#define SVC_VIN_FHD       (3U)
#define SVC_VIN_1K        (4U)
#define SVC_VIN_HD        (5U)


typedef struct {
    UINT32 en_2stage_compensation    :1;     // 0: 1-pass; 1: 2-pass
    UINT32 reserved                  :7;     // reserved
} SVC_WARP_TABLE_MODE_s;

typedef struct {
    UINT8 ver[3];         // Contains 3 bytes, example: 1.6.0
    UINT32 header_size;   // Total Header Size, after it the actual Table starts (used to compute where actual Table starts)
    UINT32 table_size;    // Table Size, after it maybe a Footer could be present (used to compute where actual Table ends). If it is zero, table finish at EOF
} SVC_WARP_TABLE_HEADER_BASE_s;

typedef struct {
    //** Note: In future versions more fields will be added
    UINT16 horizontal_grid_number;
    UINT16 vertical_grid_number;
    UINT32 tile_width_q16;
    UINT32 tile_height_q16;
    SVC_WARP_TABLE_MODE_s warp_mode;
    UINT32 id;
    UINT32 hash;            // pearson16 hash of the table samples
    UINT32 table_origin_x;  // w.r.t. the image used for calibration
    UINT32 table_origin_y;  // w.r.t. the image used for calibration
    UINT16 vsync_delay;     // delay [in rows] to apply to the vsync signal for this sensor
} SVC_WARP_TABLE_HEADER_SPECIFIC_s;

typedef struct {
    //** Note: In future versions more fields will be added
    UINT32 horizontal_warp_frac :4;
    UINT32 horizontal_warp_int  :11;
    UINT32 horizontal_warp_sign :1;
    UINT32 vertical_warp_frac   :4;
    UINT32 vertical_warp_int    :11;
    UINT32 vertical_warp_sign   :1;
} SVC_WARP_TABLE_DATA_s;

typedef struct {
    SVC_WARP_TABLE_HEADER_BASE_s header_base;
    SVC_WARP_TABLE_HEADER_SPECIFIC_s header_spec;
    char table[SVC_TABLE_MAXSIZE];
} SVC_WARP_TABLE_s;


UINT32 SvcWarp_QueryWarpSize(UINT32 *pMemSize);

UINT32 Svc_ApplyWarpFile(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 Header, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, const char *FileName);
UINT32 Svc_ApplyWarpTable(UINT32 FovID, UINT32 Enable, UINT32 Width, UINT32 Height, UINT32 HGdNum, UINT32 VGdNum, UINT32 TileWExp, UINT32 TileHExp, AMBA_IK_GRID_POINT_s *pWarpTbl);

UINT32 Svc_ApplyStereoWarpV2(UINT32 FovID,const AMBA_STU_STEREO_CAM_WARP_INFO_s *StereoWarpInfo, UINT32 Enable);
UINT32 Svc_ApplyStereoWarp(UINT32 FovID, const char *FileName);

#endif /* SVC_WARP_H */
