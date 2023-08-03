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

#ifndef AMBA_IK_IMAGE_AAA_STAT_H
#define AMBA_IK_IMAGE_AAA_STAT_H

#include "AmbaTypes.h"

#define AMBA_IK_CFA_HISTO_COUNT (64UL)
#define AMBA_IK_PG_HISTO_COUNT (64UL)
#define AMBA_IK_HDR_HISTO_COUNT (128UL)

// ===================== get statistic======================//
#define AMBA_IK_3A_AWB_TILE_COL_COUNT       32UL
#define AMBA_IK_3A_AWB_TILE_ROW_COUNT       32UL

#define AMBA_IK_3A_AE_TILE_COL_COUNT        12UL
#define AMBA_IK_3A_AE_TILE_ROW_COUNT        8UL

#define AMBA_IK_3A_AF_TILE_COL_COUNT        12UL
#define AMBA_IK_3A_AF_TILE_ROW_COUNT        8UL

#define AMBA_IK_3A_SLICE_MAX_COUNT          16UL

typedef  struct {
    //AWB
    UINT16 AwbTileColStart;
    UINT16 AwbTileRowStart;
    UINT16 AwbTileWidth;
    UINT16 AwbTileHeight;
    UINT16 AwbTileActiveWidth;
    UINT16 AwbTileActiveHeight;
    UINT16 AwbRgbShift;
    UINT16 AwbYShift;
    UINT16 AwbMinMaxShift;

    //AE
    UINT16 AeTileColStart;
    UINT16 AeTileRowStart;
    UINT16 AeTileWidth;
    UINT16 AeTileHeight;
    UINT16 AeYShift;
    UINT16 AeLinearYShift;
    UINT16 AeMinMaxShift;

    //AF
    UINT16 AfTileColStart;
    UINT16 AfTileRowStart;
    UINT16 AfTileWidth;
    UINT16 AfTileHeight;
    UINT16 AfTileActiveWidth;
    UINT16 AfTileActiveHeight;
    UINT16 AfYShift;
    UINT16 AfCfaYShift;

    // AWB tiles
    UINT8  AwbTileNumCol;
    UINT8  AwbTileNumRow;
    // AE tiles
    UINT8  AeTileNumCol;
    UINT8  AeTileNumRow;
    // AF tiles
    UINT8  AfTileNumCol;
    UINT8  AfTileNumRow;

    UINT16 Reserved0;
    UINT32 Reserved1[2];

    UINT32 Reserved2 : 16;
    // Misc
    UINT32 ChanIndex : 4;
    UINT32 Reserved3 : 12;

    UINT32 RawPicSeqNum;  // sequence number of raw picture used in producing this block of AAA results
    UINT32 IsoConfigTag;

    UINT32 reserved4[13];  // max 128 bytes
}  __attribute__((packed)) AMBA_IK_3A_HEADER_s;

// cfa awb
typedef struct {
    UINT16  SumR;
    UINT16  SumG;
    UINT16  SumB;
    UINT16  CountMin;
    UINT16  CountMax;
} __attribute__((packed)) AMBA_IK_CFA_AWB_s;
// cfa ae
typedef struct {
    UINT16  LinY;
    UINT16  CountMin;
    UINT16  CountMax;
} __attribute__((packed)) AMBA_IK_CFA_AE_s;
// cfa af
typedef struct {
    UINT16  SumY;
    UINT16  SumFV1;
    UINT16  SumFV2;
} __attribute__((packed)) AMBA_IK_CFA_AF_s;
//cfa histogram
typedef  struct {
    UINT32  HisBinR[AMBA_IK_CFA_HISTO_COUNT];
    UINT32  HisBinG[AMBA_IK_CFA_HISTO_COUNT];
    UINT32  HisBinB[AMBA_IK_CFA_HISTO_COUNT];
    UINT32  HisBinY[AMBA_IK_CFA_HISTO_COUNT];
}  __attribute__((packed)) AMBA_IK_CFA_HISTO_s;

typedef  struct {
    UINT16  SumFY;
    UINT16  SumFV1;
    UINT16  SumFV2;
}  __attribute__((packed)) AMBA_IK_PG_AF_s;

// pg ae
typedef struct {
    UINT16  SumY;
} __attribute__((packed)) AMBA_IK_PG_AE_s;

// pg histo
typedef  struct {
    UINT32  HisBinY[AMBA_IK_PG_HISTO_COUNT];
    UINT32  HisBinR[AMBA_IK_PG_HISTO_COUNT];
    UINT32  HisBinG[AMBA_IK_PG_HISTO_COUNT];
    UINT32  HisBinB[AMBA_IK_PG_HISTO_COUNT];
}  __attribute__((packed)) AMBA_IK_PG_HISTO_s;

//cfa_aaa_stat_t
typedef struct {
    AMBA_IK_3A_HEADER_s      Header;
    UINT16                   FrameId;
    AMBA_IK_CFA_AWB_s        Awb[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT];
    AMBA_IK_CFA_AE_s         Ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    AMBA_IK_CFA_AF_s         Af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_AF_TILE_COL_COUNT];
    AMBA_IK_CFA_HISTO_s      Histogram;
} __attribute__((packed)) AMBA_IK_CFA_3A_DATA_s;

//pg_aaa_stat_t
typedef struct {
    AMBA_IK_3A_HEADER_s    Header;
    UINT16                 FrameId;
    AMBA_IK_PG_AF_s        Af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_AF_TILE_COL_COUNT];
    AMBA_IK_PG_AE_s        Ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    AMBA_IK_PG_HISTO_s     Histogram;
} AMBA_IK_PG_3A_DATA_s;

typedef struct 
{
    UINT8  VinStatsType;    // 0: main; 1: hdr
    UINT8  ChannelIndex;
    UINT8  TotalExposures;
    UINT8  BlendIndex;     // exposure no.

    UINT32 Reserved1;

    UINT16 StatsLeft;
    UINT16 StatsWidth;

    UINT16 StatsTop;
    UINT16 StatsHeight;

    UINT32 Reserved2[28];

} __attribute__((packed)) AMBA_IK_CFA_HIST_CFG_INFO_s;

typedef struct {
    UINT32 HistoBinR[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinG[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinB[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinIR[AMBA_IK_HDR_HISTO_COUNT];
} __attribute__((packed)) AMBA_IK_CFA_HISTOGRAM_s;

typedef struct {
    AMBA_IK_CFA_HIST_CFG_INFO_s Header;
    AMBA_IK_CFA_HISTOGRAM_s Data;      /* Address of histogram statistic AMBA_DSP_HDR_HIST_STAT_s */
} __attribute__((packed)) AMBA_IK_CFA_HIST_STAT_s;


#endif  /* _AMBA_IK_AAA_STAT_H_ */
