/**
 * @file AmbaCalibData.h
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
 *
 *  @details storage management for calibration data
 *
 */


#ifndef AMBA_CALIB_DATA_H
#define AMBA_CALIB_DATA_H

#ifdef CONFIG_BUILD_FOSS_LZ4
#include "AmbaLZ4_IF.h"
#endif

#include <AmbaDSP_EventInfo.h>
#include <AmbaNVM_Partition.h>

#define AMBA_CALIB_DATA_MODE_READ              (0U)
#define AMBA_CALIB_DATA_MODE_WRITE             (1U)

#define AMBA_CALIB_DATA_MAX_ITEM_PER_GRP       (16U)
#define AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM   (4U)
#define AMBA_CALIB_DATA_MAX_USER_DATA          (16U)

#define AMBA_CALIB_DATA_STORAGE_NAND           AMBA_NVM_NAND
#define AMBA_CALIB_DATA_STORAGE_eMMC           AMBA_NVM_eMMC
#define AMBA_CALIB_DATA_STORAGE_SPI_NOR        AMBA_NVM_SPI_NOR
#define AMBA_CALIB_DATA_STORAGE_SPI_NAND       AMBA_NVM_SPI_NAND
#define AMBA_CALIB_DATA_STORAGE_SD             AMBA_NUM_NVM

#define AMBA_CALIB_DATA_TYPE_WARP              (0U)
#define AMBA_CALIB_DATA_TYPE_VIG               (1U)
#define AMBA_CALIB_DATA_TYPE_BLEND             (2U)
#define AMBA_CALIB_DATA_TYPE_OSD               (3U)
#define AMBA_CALIB_DATA_TYPE_RAW               (4U)
#define AMBA_CALIB_DATA_TYPE_NUM               (5U)

#define AMBA_CALIB_DATA_CMPR_ALGO_NONE         (0U)
#define AMBA_CALIB_DATA_CMPR_ALGO_LZ4          (1U)
#define AMBA_CALIB_DATA_CMPR_ALGO_RLE          (2U)
#define AMBA_CALIB_DATA_CMPR_ALGO_NUM          (3U)

#define AMBA_CALIB_DATA_DECMPR_LZ4_LINEAR (0U)
#define AMBA_CALIB_DATA_DECMPR_LZ4_WINDOW (1U)
#define AMBA_CALIB_DATA_DECMPR_LZ4_NUM    (2U)

#define AMBA_CALIB_DATA_MAX_DST_NUM            (2U)

#ifdef CONFIG_BUILD_FOSS_LZ4
/* compr info */
typedef struct {
   UINT16 SegmentSize;
} AMBA_CALIB_DATA_LZ4_CMPR_INFO_s;


typedef struct {
   AMBA_CALIB_DATA_LZ4_CMPR_INFO_s Lz4Info;
} AMBA_CALIB_DATA_CMPR_INFO_s;

/* decompr info */
typedef struct {
   UINT8 DecmprType; // AMBA_CALIB_DATA_DECMPR_TYPE_LZ4_XXX
   UINT16 SegmentSize;
   UINT16 DstNum;
   AMBA_LZ4_TBL_SIZE_s DstTableSize[AMBA_CALIB_DATA_MAX_DST_NUM];
   AMBA_LZ4_WIN_s DstWinInfo[AMBA_CALIB_DATA_MAX_DST_NUM];
} AMBA_CALIB_DATA_LZ4_DECMPR_INFO_s;

typedef struct {
   AMBA_CALIB_DATA_LZ4_DECMPR_INFO_s Lz4Info;
} AMBA_CALIB_DATA_DECMPR_INFO_s;
#endif
/* calib data */
typedef struct {
    UINT32 StartX;     // Unit in pixel. Before downsample.
    UINT32 StartY;     // Unit in pixel. Before downsample.
    UINT32 Width;      // Unit in pixel. After downsample.
    UINT32 Height;     // Unit in pixel. After downsample.
    UINT32 HSubSampleFactorNum;
    UINT32 HSubSampleFactorDen;
    UINT32 VSubSampleFactorNum;
    UINT32 VSubSampleFactorDen;
} AMBA_CALIB_DATA_SENSOR_GEOMETRY_s;

typedef struct {
    AMBA_CALIB_DATA_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 HorGridNum;
    UINT32 VerGridNum;
    UINT32 TileWidth;
    UINT32 TileHeight;
} AMBA_CALIB_DATA_WARP_FEED_DATA_s;

typedef struct {
    AMBA_CALIB_DATA_SENSOR_GEOMETRY_s CalibSensorGeo;
    UINT32 RadialCoarse;
    UINT32 RadialCoarseLog;
    UINT32 RadialBinsFine;
    UINT32 RadialBinsFineLog;
    UINT32 ModelCenterX_R;  // 4 individual bayer components optical center x, relative to calib window.
    UINT32 ModelCenterX_Gr;
    UINT32 ModelCenterX_B;
    UINT32 ModelCenterX_Gb;

    UINT32 ModelCenterY_R;  // 4 individual bayer components optical center Y, relative to calib window.
    UINT32 ModelCenterY_Gr;
    UINT32 ModelCenterY_B;
    UINT32 ModelCenterY_Gb;
} AMBA_CALIB_DATA_VIG_FEED_DATA_s;

typedef struct {
    UINT32 Version;
    AMBA_CALIB_DATA_WARP_FEED_DATA_s Data;
} AMBA_CALIB_DATA_WARP_INFO_s;

typedef struct {
    UINT32 Version;
    AMBA_CALIB_DATA_VIG_FEED_DATA_s Data;
} AMBA_CALIB_DATA_VIG_INFO_s;

typedef struct {
    UINT32 Version;
    UINT32 Width;
    UINT32 Height;
} AMBA_CALIB_DATA_BLEND_INFO_s;

typedef struct {
    UINT32 Version;
    AMBA_DSP_WINDOW_s OsdWindow;
} AMBA_CALIB_DATA_OSD_INFO_s;

typedef struct {
    UINT32 Version;
} AMBA_CALIB_DATA_RAW_INFO_s;

/* group descriptor */
typedef struct {
    UINT8  Type; // AMBA_CALIB_DATA_TYPE_XXX
    UINT32 Size; // original data size
    AMBA_CALIB_DATA_WARP_INFO_s  Warp;
    AMBA_CALIB_DATA_VIG_INFO_s   Vig;
    AMBA_CALIB_DATA_BLEND_INFO_s Blend;
    AMBA_CALIB_DATA_OSD_INFO_s   Osd;
    AMBA_CALIB_DATA_RAW_INFO_s   Raw;
} AMBA_CALIB_DATA_ELEMENT_INFO_s;

typedef struct {
    UINT8  Count;
    UINT8  Compressed;     // AMBA_CALIB_DATA_CMPR_XXX
    UINT32 Size;
    AMBA_CALIB_DATA_ELEMENT_INFO_s ElementInfo[AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM];
} AMBA_CALIB_DATA_ITEM_INFO_s;

typedef struct {
    UINT32 StorageId;     // AMBA_CALIB_DATA_STORAGE_XXX
    UINT32 PartId;        // partition id or drive name
    UINT8  Count;
    AMBA_CALIB_DATA_ITEM_INFO_s ItemInfo[AMBA_CALIB_DATA_MAX_ITEM_PER_GRP];
} AMBA_CALIB_DATA_GROUP_DESC_s;

/* init cfg */
typedef struct {
    UINT8  MaxHdlr;
    UINT32 StorageId;
    UINT32 PartId;
    UINT32 MaxGroup;
    UINT32 MaxWarp;
    UINT32 MaxVig;
    UINT32 MaxBlend;
    UINT32 MaxOsd;
    UINT32 MaxRaw;
    UINT8  MaxUdta;
    UINT32 UdtaSize[AMBA_CALIB_DATA_MAX_USER_DATA];
    UINT8 *Buffer;
    UINT32 BufferSize;
} AMBA_CALIB_DATA_INIT_CFG_s;

UINT32 AmbaCalibData_GetInitBufferSize(const AMBA_CALIB_DATA_INIT_CFG_s *Config, UINT32 *BufferSize);
UINT32 AmbaCalibData_GetInitDefaultCfg(AMBA_CALIB_DATA_INIT_CFG_s *DefaultCfg);
/**
 * Initiate the CalibData module, get the buffer size from AmbaCalibData_GetInitBufferSize()
 */
UINT32 AmbaCalibData_Init(const AMBA_CALIB_DATA_INIT_CFG_s *Config);
/**
 * Mode : read or write
 */
UINT32 AmbaCalibData_Create(UINT8 Mode, UINT32 *Hdlr);
UINT32 AmbaCalibData_Delete(UINT32 Hdlr);
/**
 * Write
 * Write unit : Group
 * Buffer : User should prepare the buffer with consecutive data (arranged in order)
 *
 * Read
 * MAX consecutive read unit : Group
 * MIN consecutive read unit : Item
 */
UINT32 AmbaCalibData_Write(UINT32 Hdlr, const AMBA_CALIB_DATA_GROUP_DESC_s *Desc, UINT8 *Buffer, UINT32 Size);
UINT32 AmbaCalibData_Read(UINT32 Hdlr, UINT32 Index, UINT8 Offset, UINT8 Count, UINT8 *Buffer, UINT32 Size);
UINT32 AmbaCalibData_GetDesc(UINT32 Hdlr, UINT32 Index, AMBA_CALIB_DATA_GROUP_DESC_s *Desc);
UINT32 AmbaCalibData_AddUdta(UINT32 Hdlr, const UINT8 *Buffer, UINT32 Size);
UINT32 AmbaCalibData_GetUdta(UINT32 Hdlr, UINT8 Index, UINT8 *Buffer, UINT32 Size);
#ifdef CONFIG_BUILD_FOSS_LZ4
UINT32 AmbaCalibData_Compress(UINT8 Algo, const AMBA_CALIB_DATA_CMPR_INFO_s *CmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DstAddr, UINT32 DstSize, UINT32 *OutputSize);
UINT32 AmbaCalibData_Decompress(UINT8 Algo, const AMBA_CALIB_DATA_DECMPR_INFO_s *DecmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DstAddr, UINT32 DstSize, UINT32 *OutputSize);
#endif
#endif

