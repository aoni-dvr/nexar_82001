/*
*  @file AmbaDSP_Image3aStatistics.h
*
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

#ifndef AMBA_IK_CFA_HISTO_COUNT
#define AMBA_IK_CFA_HISTO_COUNT (64U)
#endif

#ifndef AMBA_IK_PG_HISTO_COUNT
#define AMBA_IK_PG_HISTO_COUNT (64U)
#endif

#ifndef AMBA_IK_HDR_HISTO_COUNT
#define AMBA_IK_HDR_HISTO_COUNT (128U)
#endif

// ===================== get statistic======================//
#ifndef AMBA_IK_3A_AWB_TILE_COL_COUNT
#define AMBA_IK_3A_AWB_TILE_COL_COUNT       64U
#endif

#ifndef AMBA_IK_3A_AWB_TILE_ROW_COUNT
#define AMBA_IK_3A_AWB_TILE_ROW_COUNT       64U
#endif

#ifndef AMBA_IK_3A_AE_TILE_COL_COUNT
#define AMBA_IK_3A_AE_TILE_COL_COUNT        24U
#endif

#ifndef AMBA_IK_3A_AE_TILE_ROW_COUNT
#define AMBA_IK_3A_AE_TILE_ROW_COUNT        16U
#endif

#ifndef AMBA_IK_3A_CFA_AF_TILE_COL_COUNT
#define AMBA_IK_3A_CFA_AF_TILE_COL_COUNT        24U
#endif

#ifndef AMBA_IK_3A_PG_AF_TILE_COL_COUNT
#define AMBA_IK_3A_PG_AF_TILE_COL_COUNT        32U
#endif

#ifndef AMBA_IK_3A_AF_TILE_ROW_COUNT
#define AMBA_IK_3A_AF_TILE_ROW_COUNT        16U
#endif

#ifndef AMBA_IK_3A_SLICE_MAX_COUNT
#define AMBA_IK_3A_SLICE_MAX_COUNT          32U
#endif

typedef struct {
    UINT8 BayerPattern;
    UINT8 IrMode;
    UINT8 CropEn;
    UINT8 Reserved0;
    UINT16 CropColStart;
    UINT16 CropRowStart;
    UINT16 CropWidth;
    UINT16 CropHeight;
} __attribute__((packed)) AMBA_IK_3A_HEADER_VIN_STAT_s;

typedef struct {
    UINT8 AwbEnable;
    UINT8 AwbTileNumCol;
    UINT8 AwbTileNumRow;
    UINT8 Reserved;
    UINT16 AwbTileColStart;
    UINT16 AwbTileRowStart;
    UINT16 AwbTileWidth;
    UINT16 AwbTileHeight;
    UINT16 AwbTileActiveWidth;
    UINT16 AwbTileActiveHeight;
} __attribute__((packed)) AMBA_IK_3A_HEADER_AWB_s;

typedef struct {
    UINT8 AeEnable;
    UINT8 AeTileNumCol;
    UINT8 AeTileNumRow;
    UINT8 Reserved;
    UINT16 AeTileColStart;
    UINT16 AeTileRowStart;
    UINT16 AeTileWidth;
    UINT16 AeTileHeight;
    UINT16 AeMinThresh;
    UINT16 AeMaxThresh;
} __attribute__((packed)) AMBA_IK_3A_HEADER_CFA_AE_s;

typedef struct {
    UINT8 AeEnable;
    UINT8 AeTileNumCol;
    UINT8 AeTileNumRow;
    UINT8 AeTileYShift;
    UINT16 AeTileColStart;
    UINT16 AeTileRowStart;
    UINT16 AeTileWidth;
    UINT16 AeTileHeight;
    UINT32 Reserved;
} __attribute__((packed)) AMBA_IK_3A_HEADER_PG_AE_s;

typedef struct {
    UINT8 AfEnable;
    UINT8 AfTileNumCol;
    UINT8 AfTileNumRow;
    UINT8 Reserved;
    UINT16 AfTileColStart;
    UINT16 AfTileRowStart;
    UINT16 AfTileWidth;
    UINT16 AfTileHeight;
    UINT16 AfTileActiveWidth;
    UINT16 AfTileActiveHeight;
} __attribute__((packed)) AMBA_IK_3A_HEADER_AF_s;

typedef  struct {
    UINT32 reserved1;
    AMBA_IK_3A_HEADER_VIN_STAT_s VinHistogram;
    AMBA_IK_3A_HEADER_AWB_s Awb;
    AMBA_IK_3A_HEADER_CFA_AE_s CfaAe;
    AMBA_IK_3A_HEADER_AF_s CfaAf;
    AMBA_IK_3A_HEADER_PG_AE_s PgAe;
    AMBA_IK_3A_HEADER_AF_s PgAf;
    UINT32 CfaAeTileHistogramMask[16];
    UINT32 PgSeTileHistogramMask[16];

    // the shift parameters
    /* AWB */
    UINT16 AwbRgbShift;
    UINT16 AwbMinMaxShift;
    /* AE */
    UINT16 PgAeYShift;
    UINT16 AeLinearYShift;
    UINT16 AeMinMaxShift;
    /* AF */
    UINT16 PgAfYShift;
    UINT16 AfCfaYShift;

    UINT8 ChanIndex ;
    UINT8 VinStatsType; // 0: main; 1: hdr; 2:hdr2
    UINT32 RawPicSeqNum; // sequence number of raw picture used in producing this block of AAA results
    UINT8 AaaCfaMuxSel; // 1: before_CE, 2: pre_IR
    UINT8 reserved2;
    UINT16 reserved3[5];
} __attribute__((packed)) AMBA_IK_3A_HEADER_s;

// cfa awb
typedef struct {
    UINT16  SumR;
    UINT16  SumG;
    UINT16  SumB;
    UINT16  SumIR;
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
//Odd : RgbIr case, AAA from Pre_IR
//Even : RgbIr case, AAA from Before_CE
//Non-RgbIr case, AAA from Before_CE
typedef struct {
    AMBA_IK_3A_HEADER_s      Header;
    UINT16                   FrameId;
    AMBA_IK_CFA_AWB_s        Awb[AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT];
    AMBA_IK_CFA_AE_s         Ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    AMBA_IK_CFA_AF_s         Af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_CFA_AF_TILE_COL_COUNT];
    AMBA_IK_CFA_HISTO_s      Histogram;
} __attribute__((packed)) AMBA_IK_CFA_3A_DATA_s;

//pg_aaa_stat_t
typedef struct {
    AMBA_IK_3A_HEADER_s    Header;
    UINT16                 FrameId;
    AMBA_IK_PG_AF_s        Af[AMBA_IK_3A_AF_TILE_ROW_COUNT * AMBA_IK_3A_PG_AF_TILE_COL_COUNT];
    AMBA_IK_PG_AE_s        Ae[AMBA_IK_3A_AE_TILE_ROW_COUNT * AMBA_IK_3A_AE_TILE_COL_COUNT];
    AMBA_IK_PG_HISTO_s     Histogram;
} AMBA_IK_PG_3A_DATA_s;

typedef struct {
    UINT32 HistoBinR[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinG[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinB[AMBA_IK_HDR_HISTO_COUNT];
    UINT32 HistoBinIR[AMBA_IK_HDR_HISTO_COUNT];
} __attribute__((packed)) AMBA_IK_CFA_HISTOGRAM_s;

typedef struct {
    AMBA_IK_3A_HEADER_s Header;
    AMBA_IK_CFA_HISTOGRAM_s Data;      /* Address of histogram statistic AMBA_DSP_HDR_HIST_STAT_s */
} __attribute__((packed)) AMBA_IK_CFA_HIST_STAT_s;


#endif  /* _AMBA_IK_AAA_STAT_H_ */
