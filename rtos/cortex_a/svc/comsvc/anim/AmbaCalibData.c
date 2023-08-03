/**
 * @file AmbaCalibData.c
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
 *  @details Implementation of storage management for calibration data
 *
 */

#include <AmbaCalibData.h>
#include <AmbaDef.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>
#include <AmbaSD_Def.h>
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>

#define AMBA_CALIB_DATA_SECTOR_SIZE      (512U)
#define AMBA_CALIB_DATA_PAGE_SIZE        (2048U)
#define AMBA_CALIB_DATA_CLUSTER_SIZE     (32768U) // 32K
#define AMBA_CALIB_DATA_WAIT_TIMEOUT     (10000U)
#define AMBA_CALIB_DATA_MAX_HDLR         (2U)
#define AMBA_CALIB_DATA_MAX_NAME_LENGTH  (64U)
#define AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT (26U) /**< The maximum number of drives */

#define AMBA_CALIB_DATA_FILE_NAME        "CalibDataFile"

/* Tag */
#define AMBA    (0x414D4241U)   /**< file header */
#define HEAD    (0x48454144U)   /**< data header */
#define UDTA    (0x55445441U)   /**< user data */
#define GRPI    (0x47525049U)   /**< group info */
#define ITMI    (0x49544D49U)   /**< item  info */
#define FREE    (0x46524545U)   /**< free */
#define DATA    (0x44415441U)   /**< data */
#define WARP    (0x57415250U)   /**< warp */
#define VIGN    (0x5649474EU)   /**< vignette */
#define BLND    (0x424C4E44U)   /**< blending */
#define OSDD    (0x4f534444U)   /**< osd data */
#define RAWD    (0x52415744U)   /**< raw data */

#define DEFAULT_MAX_HDLR       (2U)
#define DEFAULT_MAX_GROUP      (1024U)
#define DEFAULT_MAX_WARP       (1024U)
#define DEFAULT_MAX_VIG        (1024U)
#define DEFAULT_MAX_BLEND      (1024U)
#define DEFAULT_MAX_OSD        (1024U)
#define DEFAULT_MAX_RAW        (1024U)
#define DEFAULT_MAX_UDTA       (4U)
#define DEFAULT_MAX_UDTA_SIZE  (1024U)

#define AMBA_INFO_SIZE  (8U)
#define HEAD_INFO_SIZE  (12U)
#define FREE_INFO_SIZE  (8U)
#define DATA_INFO_SIZE  (8U)
#define UDTA_INFO_SIZE  (8U)
#define GROUP_INFO_SIZE (16U)
#define ITEM_INFO_SIZE  (24U)
#define WARP_INFO_SIZE  (64U)
#define VIG_INFO_SIZE   (96U)
#define BLEND_INFO_SIZE (24U)
#define OSD_INFO_SIZE   (24U)
#define RAW_INFO_SIZE   (16U)

typedef struct {
    UINT32 AtomSize;
    const AMBA_CALIB_DATA_WARP_INFO_s *Info;
    UINT32 OrigSize;    /**< table size before compression */
} AMBA_CALIB_DATA_WARP_DATA_s;

typedef struct {
    UINT32 AtomSize;
    const AMBA_CALIB_DATA_VIG_INFO_s *Info;
    UINT32 OrigSize;    /**< table size before compression */
} AMBA_CALIB_DATA_VIG_DATA_s;

typedef struct {
    UINT32 AtomSize;
    const AMBA_CALIB_DATA_BLEND_INFO_s *Info;
    UINT32 OrigSize;    /**< table size before compression */
} AMBA_CALIB_DATA_BLEND_DATA_s;

typedef struct {
    UINT32 AtomSize;
    const AMBA_CALIB_DATA_OSD_INFO_s *Info;
    UINT32 OrigSize;    /**< table size before compression */
} AMBA_CALIB_DATA_OSD_DATA_s;

typedef struct {
    UINT32 AtomSize;
    const AMBA_CALIB_DATA_RAW_INFO_s *Info;
    UINT32 OrigSize;    /**< table size before compression */
} AMBA_CALIB_DATA_RAW_DATA_s;

typedef struct {
    UINT32 AtomSize;
    UINT32 Crc;
    const AMBA_CALIB_DATA_ITEM_INFO_s *Info;
    UINT32 DataPos;
} AMBA_CALIB_DATA_ITEM_DATA_s;

/* keep part of desc for reading data without parsing desc */
typedef struct {
    UINT8 *DescPtr;
    UINT32 StorageId;
    UINT32 PartId;
    UINT8  Count;
    UINT32 ItemSize[AMBA_CALIB_DATA_MAX_ITEM_PER_GRP];
    UINT32 DataPos[AMBA_CALIB_DATA_MAX_ITEM_PER_GRP];
    UINT32 Crc[AMBA_CALIB_DATA_MAX_ITEM_PER_GRP];
} AMBA_CALIB_DATA_GROUP_DESC_DATA_s;

/* HDLR */
typedef struct {
    UINT8 Used;
    UINT8 WriteOnClose;
    UINT8 *GroupDescPtr; //write mode : desc position
    UINT8 *UdtaPtr[AMBA_CALIB_DATA_MAX_USER_DATA];
    UINT32 UdtaSize[AMBA_CALIB_DATA_MAX_USER_DATA];
    AMBA_FS_FILE *File[AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT];
    UINT32 HeaderSize;
    UINT32 GroupNum;
    UINT8  UdtaNum;
    UINT32 WarpNum;
    UINT32 VigNum;
    UINT32 BlendNum;
    UINT32 OsdNum;
    UINT32 RawNum;
} AMBA_CALIB_DATA_HDLR_s;

typedef struct {
    UINT32 StorageId;
    UINT32 PartId;
    AMBA_FS_FILE *File;
} AMBA_CALIB_DATA_STORAGE_INFO_s;

typedef struct {
    /* NVM */
    AMBA_NVM_FTL_INFO_s NftlInfo[AMBA_NUM_NVM];
    UINT32 NvmUsage[AMBA_NUM_NVM][AMBA_NUM_USER_PARTITION];
    /* SD */
    UINT32 ClusterSize[AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT];
    UINT32 DeviceUsage[AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT];
} AMBA_CALIB_DATA_STORAGE_MRG_s;

/* keep user setting */
static UINT8 G_HdlrNum = 0U;
static UINT32 G_MaxWarp = 0U;
static UINT32 G_MaxVig = 0U;
static UINT32 G_MaxBlend = 0U;
static UINT32 G_MaxOsd = 0U;
static UINT32 G_MaxRaw = 0U;
static UINT32 G_MaxGroup = 0U;
static UINT8  G_MaxUdta = 0U;
static UINT32 G_UdtaSize[AMBA_CALIB_DATA_MAX_USER_DATA] = {0U};
static AMBA_CALIB_DATA_STORAGE_INFO_s G_MainStorage GNU_SECTION_NOZEROINIT;
static AMBA_CALIB_DATA_STORAGE_MRG_s G_StorageMrg GNU_SECTION_NOZEROINIT;

static UINT8 *G_UdtaTempBuf = NULL;  // temp buffer for Udta, copy to head when flush header
static UINT8 *G_GroupDescTempBuf = NULL; // keep part of desc for reading data without parsing desc
static UINT8 *G_HeaderBuf = NULL;
static UINT32 G_HeaderSize = 0U;
static UINT8 *G_LZ4WorkBuf = NULL; // working buffer for LZ4
static AMBA_CALIB_DATA_HDLR_s G_Hdlr[AMBA_CALIB_DATA_MAX_HDLR] = {0U};
static AMBA_KAL_MUTEX_t G_Mutex;
static AMBA_KAL_MUTEX_t G_CmprMutex; // for compression

/*
 * assume header can be completely read into memory
 * [sizeof_all][AMBA][sizeof_header][HEAD][HEAD_CRC][group_info][group_info]...[udta_info][udta_info]...[sizeof_free][FREE]... [sizeof_data][DATA][data][data]...   <= sizeof_all & CRC for complete check
 * [udta_info]  =>  [sizeof_udta_info][UDTA][data]
 * [group_info] =>  [sizeof_group_info][GRPI][StorageType][PartId][item_info][item_info]...
 * [item_info]  =>  [sizeof_item_info][ITMI][ITEM_CRC][Compressed][DataPos][DataSize][warp_info][vig_info][blend_info][osd_info][raw_info]
 * [warp_info]  =>  [sizeof_warp_info][WARP][version][CalibGeo][HorGridNum][VerGridNum][TileWidth][TileHeight][OrigSize]
 * [vig_info]   =>  [sizeof_vig_info][VIGN][version][CalibGeo][RadialCoarse][RadialCoarseLog][RadialBinsFine][RadialBinsFineLog][ModelCenterX_R][ModelCenterX_Gr][ModelCenterX_B][ModelCenterX_Gb]
                                                              [ModelCenterY_R][ModelCenterY_Gr][ModelCenterY_B][ModelCenterY_Gb][OrigSize]
 * [blend_info] =>  [sizeof_blend_info][BLND][version][Width][Height][OrigSize]
 * [osd_info]   =>  [sizeof_blend_info][OSDD][version][OsdWindow][OrigSize]
 * [raw_info]   =>  [sizeof_raw_info][RAWD][version][OrigSize]
 * data must be page-aligned
 */

static inline UINT8* ReadBeU16(UINT8 *Buffer, UINT16 *Value)
{
    const UINT8 *U8Ptr = Buffer;
    *Value = (((UINT16)U8Ptr[0] << 8) | (UINT16)U8Ptr[1]);
    return &Buffer[2U];
}

static inline UINT8* ReadBeU32(UINT8 *Buffer, UINT32 *Value)
{
    const UINT8 *U8Ptr = Buffer;
    *Value = (((UINT32)U8Ptr[0] << 24) | ((UINT32)U8Ptr[1] << 16) | ((UINT32)U8Ptr[2] << 8) | (UINT32)U8Ptr[3]);
    return &Buffer[4U];
}

static inline UINT8* WriteBeU16(UINT8 *Buffer, UINT16 Value)
{
    UINT8 *U8Ptr = Buffer;
    U8Ptr[0] = (UINT8)((Value & 0xFF00U) >> 8);
    U8Ptr[1] = (UINT8)(Value & 0xFFU);
    return &Buffer[2U];
}


static inline UINT8* WriteBeU32(UINT8 *Buffer, UINT32 Value)
{
    UINT8 *U8Ptr = Buffer;
    U8Ptr[0] = (UINT8)((Value & 0xFF000000U) >> 24);
    U8Ptr[1] = (UINT8)((Value & 0xFF0000U) >> 16);
    U8Ptr[2] = (UINT8)((Value & 0xFF00U) >> 8);
    U8Ptr[3] = (UINT8)(Value & 0xFFU);
    return &Buffer[4U];
}

static inline void GetFilePath(UINT32 PartId, char *FileName, UINT32 Length)
{
    char *DriveName;
    const UINT32 *U32Ptr = &PartId;
    AmbaMisra_TypeCast32(&DriveName, &U32Ptr);
    DriveName[1U] = '\0';
    AmbaUtility_StringCopy(FileName, Length, DriveName);
    AmbaUtility_StringAppend(FileName, Length, ":\\");
    AmbaUtility_StringAppend(FileName, Length, AMBA_CALIB_DATA_FILE_NAME);
    AmbaUtility_StringAppend(FileName, Length, ".bin");
    AmbaPrint_PrintStr5("[calibdata] FilePath (%s)", FileName, NULL, NULL, NULL, NULL);
}

static UINT32 GetDriveIdx(UINT32 PartId, UINT8 *Idx)
{
    UINT32 Rval = OK;
    UINT8 DriveName = (UINT8)PartId;
    UINT8 CharA = (UINT8)'A';
    UINT8 CharZ = (UINT8)'Z';
    if ((DriveName >= CharA) && (DriveName <= CharZ)) {
        *Idx = (DriveName - CharA);
    } else {
        *Idx = AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT;
        AmbaPrint_PrintUInt5("[calibdata] GetDriveIdx failed. Wrong drive name : %u", PartId, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 GetClusterSize(UINT32 PartId)
{
    const char *DriveName;
    const UINT32 *U32Ptr = &PartId;
    UINT32 Rval;
    AMBA_FS_DRIVE_INFO_t DriveInfo;
    AmbaMisra_TypeCast32(&DriveName, &U32Ptr);
    Rval = AmbaFS_GetDriveInfo(DriveName[0], &DriveInfo);
    if (Rval == OK) {
        UINT8 FileIdx;
        Rval = GetDriveIdx(PartId, &FileIdx);
        if (Rval == OK) {
            if ((AMBA_CALIB_DATA_CLUSTER_SIZE >= (DriveInfo.BytesPerSector * DriveInfo.SectorsPerCluster)) && ((AMBA_CALIB_DATA_CLUSTER_SIZE % (DriveInfo.BytesPerSector * DriveInfo.SectorsPerCluster)) == 0U)) {
                G_StorageMrg.ClusterSize[FileIdx] = DriveInfo.BytesPerSector * DriveInfo.SectorsPerCluster;
            } else {
                G_StorageMrg.ClusterSize[FileIdx] = 0xFFFFFFFFU;
                AmbaPrint_PrintUInt5("[calibdata] (%u) module setting (AMBA_CALIB_DATA_CLUSTER_SIZE = %u) cannot support this Driver (Cluster size = %u).", __LINE__, AMBA_CALIB_DATA_CLUSTER_SIZE, DriveInfo.BytesPerSector * DriveInfo.SectorsPerCluster, 0U, 0U);
                Rval = ERR_ARG;
            }
        }
    } else {
        AmbaPrint_PrintStr5("[calibdata] cannot get DriveInfo. Drive name : %s", DriveName, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetNvmInfo(UINT32 StorageId, UINT32 PartId)
{
    UINT32 Rval = ERR_ARG;
    if (StorageId < AMBA_NUM_NVM) {
        if (PartId < AMBA_NUM_USER_PARTITION) {
            Rval = AmbaNVM_GetFtlInfo(StorageId, PartId, &G_StorageMrg.NftlInfo[StorageId]);
            if (Rval == OK) {
                if ((AMBA_CALIB_DATA_SECTOR_SIZE < G_StorageMrg.NftlInfo[StorageId].SectorSizeInBytes) || ((AMBA_CALIB_DATA_SECTOR_SIZE % G_StorageMrg.NftlInfo[StorageId].SectorSizeInBytes) != 0U)) {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) NVM type = %u, module setting (AMBA_CALIB_DATA_SECTOR_SIZE = %u) cannot support this NVM type (NftlInfo.SectorSizeInBytes = %u).", __LINE__, StorageId, AMBA_CALIB_DATA_SECTOR_SIZE, G_StorageMrg.NftlInfo[StorageId].SectorSizeInBytes, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_GetFtlInfo failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid PartId", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid StorageId", __LINE__, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 OpenStorage(AMBA_CALIB_DATA_HDLR_s *Hdlr, AMBA_CALIB_DATA_STORAGE_INFO_s *StorageInfo)
{
    UINT32 Rval = OK;
    if (StorageInfo->StorageId < AMBA_NUM_NVM) {
        if (G_StorageMrg.NftlInfo[StorageInfo->StorageId].Initialized != 1U) {
            Rval = GetNvmInfo(StorageInfo->StorageId, StorageInfo->PartId);
        }
    } else {
        UINT8 FileIdx;
        Rval = GetDriveIdx(StorageInfo->PartId, &FileIdx);
        if (Rval == OK) {
            if (Hdlr->File[FileIdx] == NULL) {
                char FileName[AMBA_CALIB_DATA_MAX_NAME_LENGTH] = {'\0'};
                GetFilePath(StorageInfo->PartId, FileName, AMBA_CALIB_DATA_MAX_NAME_LENGTH);
                if (Hdlr->WriteOnClose == 1U) {
                    Rval = AmbaFS_FileOpen(FileName, "w", &Hdlr->File[FileIdx]);
                } else {
                    Rval = AmbaFS_FileOpen(FileName, "r", &Hdlr->File[FileIdx]);
                }
                if (Rval != OK) {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileOpen failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
            StorageInfo->File = Hdlr->File[FileIdx];
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) GetDriveIdx failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 GetAlignedSize(UINT32 StorageId, UINT32 PartId, UINT32 *AlignedSize)
{
    UINT32 Rval = OK;
    if (StorageId < AMBA_NUM_NVM) {
        if (G_StorageMrg.NftlInfo[StorageId].Initialized != 1U) {
            Rval = GetNvmInfo(StorageId, PartId);
        }
        if (Rval == OK) {
            if (StorageId == AMBA_NVM_eMMC) {
                *AlignedSize = G_StorageMrg.NftlInfo[StorageId].SectorSizeInBytes;
            } else {
                *AlignedSize = G_StorageMrg.NftlInfo[StorageId].PageSizeInBytes;
            }
        } else { // use the default setting
            if (G_StorageMrg.NftlInfo[StorageId].Initialized != 1U) {
                if (StorageId == AMBA_NVM_eMMC) {
                    *AlignedSize = AMBA_CALIB_DATA_SECTOR_SIZE;
                } else {
                    *AlignedSize = AMBA_CALIB_DATA_PAGE_SIZE;
                }
                AmbaPrint_PrintUInt5("[calibdata] use the default setting", 0U, 0U, 0U, 0U, 0U);
                Rval = OK;
            }
        }
    } else if (StorageId == AMBA_CALIB_DATA_STORAGE_SD) {
        UINT8 FileIdx;
        Rval = GetDriveIdx(PartId, &FileIdx);
        if (Rval == OK) {
            if (G_StorageMrg.ClusterSize[FileIdx] == 0U) {
                Rval = GetClusterSize(PartId);
            }
            if (Rval == OK) {
                *AlignedSize = G_StorageMrg.ClusterSize[FileIdx];
            } else { // use the default setting
                if (G_StorageMrg.ClusterSize[FileIdx] == 0U) {
                    *AlignedSize = AMBA_CALIB_DATA_CLUSTER_SIZE;
                    AmbaPrint_PrintUInt5("[calibdata] use the default setting", 0U, 0U, 0U, 0U, 0U);
                    Rval = OK;
                }
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid StorageId", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}


static UINT32 GetInitBufferSize_ParamCheck(const AMBA_CALIB_DATA_INIT_CFG_s *Config, const UINT32 *BufferSize)
{
    UINT32 Rval = ERR_ARG;
    if ((Config == NULL) || (BufferSize == NULL)) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Config->MaxHdlr > AMBA_CALIB_DATA_MAX_HDLR) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid MaxHdlr", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Config->MaxUdta > AMBA_CALIB_DATA_MAX_USER_DATA) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid MaxUdta", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Get working buffer size according to the configuration
* @param [in]  Config Initial configuration
* @param [out] BufferSize Working buffer size
* @return ErrorCode
*/
UINT32 AmbaCalibData_GetInitBufferSize(const AMBA_CALIB_DATA_INIT_CFG_s *Config, UINT32 *BufferSize)
{
    /*   Working Buffer Layout
     *   ----------------------------------------------------
     *   | LZ4 work buffer (cache aligned)                  |
     *   ----------------------------------------------------
     *   | Udta Temp Buffer (cache aligned)                 |
     *   ----------------------------------------------------
     *   | Group Desc Temp Buffer (cache aligned)           |
     *   ----------------------------------------------------
     *   | Header buffer (alignment depend on storage type) |
     *   ----------------------------------------------------
     **/
    UINT32 Rval = GetInitBufferSize_ParamCheck(Config, BufferSize);
    UINT32 LZ4WorkBufferSize = 0U;
    (void) AmbaWrap_memset(&G_StorageMrg, 0, sizeof(AMBA_CALIB_DATA_STORAGE_MRG_s));
    if (Rval == OK) {
        Rval = AmbaLZ4_BlendGetTblEncWorkSize(&LZ4WorkBufferSize);
        if (Rval == OK) {
            UINT32 HeaderSize = AMBA_INFO_SIZE + HEAD_INFO_SIZE + FREE_INFO_SIZE + DATA_INFO_SIZE;
            UINT32 UdtaSize = 0U;
            UINT32 AlignedSize;
            HeaderSize += ((UINT32)Config->MaxUdta * UDTA_INFO_SIZE) + (Config->MaxGroup * GROUP_INFO_SIZE) + (Config->MaxGroup * AMBA_CALIB_DATA_MAX_ITEM_PER_GRP * ITEM_INFO_SIZE) +
                            (Config->MaxWarp * WARP_INFO_SIZE) + (Config->MaxVig * VIG_INFO_SIZE) + (Config->MaxBlend * BLEND_INFO_SIZE) +
                            (Config->MaxOsd * OSD_INFO_SIZE) + (Config->MaxRaw * RAW_INFO_SIZE);
            for (UINT8 I = 0U; I < Config->MaxUdta; I++) {
                UdtaSize += Config->UdtaSize[I];
            }
            /* get alignment size for header */
            Rval = GetAlignedSize(Config->StorageId, Config->PartId, &AlignedSize);
            if (Rval == OK) {
                UINT32 UdtaTempBufferSize;
                UINT32 GroupDescTempSize;
                UINT32 AcquiredBufferSize;
                LZ4WorkBufferSize = GetAlignedValU32(LZ4WorkBufferSize, AMBA_CACHE_LINE_SIZE);
                UdtaTempBufferSize = GetAlignedValU32(UdtaSize, AMBA_CACHE_LINE_SIZE);
                GroupDescTempSize = GetAlignedValU32((Config->MaxGroup * sizeof(AMBA_CALIB_DATA_GROUP_DESC_DATA_s)), AMBA_CACHE_LINE_SIZE);
                AcquiredBufferSize = HeaderSize + UdtaSize;
                G_HeaderSize = GetAlignedValU32(AcquiredBufferSize, AlignedSize);//AlignedSize depends on storage type
                /* return buffer size */
                *BufferSize = LZ4WorkBufferSize + UdtaTempBufferSize + GroupDescTempSize + G_HeaderSize;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaLZ4_BlendGetTblEncWorkSize failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 GetInitDefaultCfg_ParamCheck(const AMBA_CALIB_DATA_INIT_CFG_s *DefaultCfg)
{
    UINT32 Rval = ERR_ARG;
    if (DefaultCfg == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Get function for recommend initial configuration
* @param [out] DefaultCfg Initial configuration
* @return ErrorCode
*/
UINT32 AmbaCalibData_GetInitDefaultCfg(AMBA_CALIB_DATA_INIT_CFG_s *DefaultCfg)
{
    UINT8 I;
    UINT32 Rval = GetInitDefaultCfg_ParamCheck(DefaultCfg);
    if (Rval == OK) {
        DefaultCfg->MaxHdlr   = DEFAULT_MAX_HDLR;
        DefaultCfg->StorageId = AMBA_CALIB_DATA_STORAGE_NAND;
        DefaultCfg->PartId    = AMBA_USER_PARTITION_CALIBRATION_DATA;
        DefaultCfg->MaxGroup  = DEFAULT_MAX_GROUP;
        DefaultCfg->MaxWarp   = DEFAULT_MAX_WARP;
        DefaultCfg->MaxVig    = DEFAULT_MAX_VIG;
        DefaultCfg->MaxBlend  = DEFAULT_MAX_BLEND;
        DefaultCfg->MaxOsd    = DEFAULT_MAX_OSD;
        DefaultCfg->MaxRaw    = DEFAULT_MAX_RAW;
        DefaultCfg->MaxUdta   = DEFAULT_MAX_UDTA;
        for (I = 0U; I < DefaultCfg->MaxUdta; I++) {
            DefaultCfg->UdtaSize[I] = DEFAULT_MAX_UDTA_SIZE;
        }
    }
    return Rval;
}

static UINT32 Init_ParamCheck(const AMBA_CALIB_DATA_INIT_CFG_s *Config)
{
    UINT32 Rval = ERR_ARG;
    if (Config == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Config->MaxHdlr > AMBA_CALIB_DATA_MAX_HDLR) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalidd MaxHdlr(%u)", __LINE__, Config->MaxHdlr, 0U, 0U, 0U);
    } else if (Config->MaxUdta > AMBA_CALIB_DATA_MAX_USER_DATA) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalidd MaxUdta(%u)", __LINE__, Config->MaxUdta, 0U, 0U, 0U);
    } else if (Config->Buffer == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Buffer is Null.", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        UINT32 AddrU32;
        AmbaMisra_TypeCast32(&AddrU32, &Config->Buffer);
        if ((AddrU32 % AMBA_CACHE_LINE_SIZE) != 0U) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Buffer (0x%x) should be cache aligned (%u)", __LINE__, AddrU32, Config->BufferSize, 0U, 0U);
        } else {
            UINT32 BufferSize = 0U;
            Rval = AmbaCalibData_GetInitBufferSize(Config, &BufferSize);
            if ((Rval == OK) && (Config->BufferSize < BufferSize)) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) BufferSize should be %u", __LINE__, BufferSize, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        }
    }
    return Rval;
}

/**
* Module initial function
* @param [in]  Config Initial configuration
* @return ErrorCode
*/
UINT32 AmbaCalibData_Init(const AMBA_CALIB_DATA_INIT_CFG_s *Config)
{
    UINT8 I;
    UINT32 Rval = OK;
    if (G_HdlrNum == 0U) {
        Rval = Init_ParamCheck(Config); // Buffer's head should be cache_line aligned
        if (Rval == OK) {
            UINT32 LZ4WorkBufferSize = 0U;
            UINT32 UdtaTempBufferSize = 0U;
            UINT32 AlignedSize = 0U;
            Rval = AmbaLZ4_BlendGetTblEncWorkSize(&LZ4WorkBufferSize);
            if (Rval == OK) {
                G_HdlrNum = Config->MaxHdlr;
                G_MainStorage.StorageId = Config->StorageId;
                G_MainStorage.PartId = Config->PartId;
                G_MaxGroup = Config->MaxGroup;
                G_MaxWarp = Config->MaxWarp;
                G_MaxVig = Config->MaxVig;
                G_MaxBlend = Config->MaxBlend;
                G_MaxOsd = Config->MaxOsd;
                G_MaxRaw = Config->MaxRaw;
                G_MaxUdta = Config->MaxUdta;
                for (I = 0U; I < Config->MaxUdta; I++) {
                    G_UdtaSize[I] = Config->UdtaSize[I];
                    UdtaTempBufferSize += G_UdtaSize[I];
                }
                /* get alignment size for header */
                Rval = GetAlignedSize(Config->StorageId, Config->PartId, &AlignedSize);
                if (Rval == OK) {
                    char MutexName[] = "AmbaCalibDataMutex";
                    UINT32 GroupDescTempSize = Config->MaxGroup * sizeof(AMBA_CALIB_DATA_GROUP_DESC_DATA_s);
                    /* calculate each buffer size */
                    LZ4WorkBufferSize = GetAlignedValU32(LZ4WorkBufferSize, AMBA_CACHE_LINE_SIZE);
                    UdtaTempBufferSize = GetAlignedValU32(UdtaTempBufferSize, AMBA_CACHE_LINE_SIZE);
                    GroupDescTempSize = GetAlignedValU32(GroupDescTempSize, AMBA_CACHE_LINE_SIZE);
                    G_LZ4WorkBuf = Config->Buffer;
                    G_UdtaTempBuf = &G_LZ4WorkBuf[LZ4WorkBufferSize];
                    G_GroupDescTempBuf = &G_UdtaTempBuf[UdtaTempBufferSize];
                    G_HeaderBuf = &G_GroupDescTempBuf[GroupDescTempSize];

                    Rval = AmbaKAL_MutexCreate(&G_Mutex, MutexName);
                    if (Rval == OK) {
                        char CmprMutexName[] = "AmbaCalibDataCmprMutex";
                        Rval = AmbaKAL_MutexCreate(&G_CmprMutex, CmprMutexName);
                        if (Rval != OK) {
                            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexCreate(G_CmprMutex) failed", __LINE__, 0U, 0U, 0U, 0U);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexCreate(G_Mutex) failed", __LINE__, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaLZ4_BlendGetTblEncWorkSize failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) already init", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ReadNvmSector(const AMBA_CALIB_DATA_STORAGE_INFO_s *NvmInfo, UINT32 Pos, UINT32 Length, UINT8 *Buffer, UINT32 *Ret)
{
    UINT32 Rval = OK;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    const AMBA_NVM_FTL_INFO_s *NftlInfo = &G_StorageMrg.NftlInfo[NvmInfo->StorageId];
    *Ret = 0U;
    SecConfig.StartSector = Pos / NftlInfo->SectorSizeInBytes;
    if (SecConfig.StartSector < NftlInfo->TotalSizeInSectors) {
        SecConfig.NumSector = Length / NftlInfo->SectorSizeInBytes;
        if ((SecConfig.StartSector + SecConfig.NumSector) > NftlInfo->TotalSizeInSectors) {
            SecConfig.NumSector = NftlInfo->TotalSizeInSectors - SecConfig.StartSector;
        }
        SecConfig.pDataBuf = Buffer;
        Rval = AmbaNVM_ReadSector(NvmInfo->StorageId, NvmInfo->PartId, &SecConfig, AMBA_CALIB_DATA_WAIT_TIMEOUT);
        if (Rval == OK) {
            *Ret = SecConfig.NumSector * NftlInfo->SectorSizeInBytes;
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_ReadSector failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) ReadNvm : StartSector=%u is over TotalSizeInSectors=%u", __LINE__, SecConfig.StartSector, NftlInfo->TotalSizeInSectors, 0U, 0U);
    }
    return Rval;
}

static UINT32 ReadNvm(const AMBA_CALIB_DATA_STORAGE_INFO_s *NvmInfo, UINT32 Pos, UINT32 Length, UINT8 *Buffer, UINT32 *Ret)
{
    UINT32 Rval = OK;
    const AMBA_NVM_FTL_INFO_s *NftlInfo = &G_StorageMrg.NftlInfo[NvmInfo->StorageId];
    *Ret = 0U;
    if (((Pos % NftlInfo->SectorSizeInBytes) == 0U) && ((Length % NftlInfo->SectorSizeInBytes) == 0U)) {
        Rval = ReadNvmSector(NvmInfo, Pos, Length, Buffer, Ret);
        if (Rval != OK) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) ReadNvmSector failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else { // deal with non-sector aligned case
        UINT8 SectorBuff[AMBA_CALIB_DATA_SECTOR_SIZE] = {0U};
        UINT32 Head = Pos;
        UINT32 Remain = Length;
        UINT32 Len = 0U;
        UINT32 ReadSize = 0U;
        UINT32 RetSize;
        if ((Head % NftlInfo->SectorSizeInBytes) != 0U) {
            Rval = ReadNvmSector(NvmInfo, Head, AMBA_CALIB_DATA_SECTOR_SIZE, SectorBuff, &RetSize);
            if (Rval == OK) {
                Len = (NftlInfo->SectorSizeInBytes - (Head % NftlInfo->SectorSizeInBytes));
                Head += Len;
                Remain -= Len;
                ReadSize += Len;
                // AMBA_CALIB_DATA_SECTOR_SIZE must >= NftlInfo->SectorSizeInBytes
                (void) AmbaWrap_memcpy(Buffer, &SectorBuff[(AMBA_CALIB_DATA_SECTOR_SIZE - Len)], Len);
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) ReadNvmSector failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
        if (Rval == OK) {
            if (Remain >= NftlInfo->SectorSizeInBytes) {
                Rval = ReadNvmSector(NvmInfo, Head, Remain, &Buffer[ReadSize], &RetSize);
                if (Rval == OK) {
                    Len = (Remain / NftlInfo->SectorSizeInBytes) * NftlInfo->SectorSizeInBytes;
                    Head += Len;
                    Remain -= Len;
                    ReadSize += Len;
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_ReadSector failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        }
        if (Rval == OK) {
            if (Remain != 0U) {
                Rval = ReadNvmSector(NvmInfo, Head, AMBA_CALIB_DATA_SECTOR_SIZE, SectorBuff, &RetSize);
                if (Rval == OK) {
                    Len = Remain;
                    (void) AmbaWrap_memcpy(&Buffer[ReadSize], &SectorBuff[0U], Len);
                    ReadSize += Len;
                    *Ret = ReadSize;
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_ReadSector failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return Rval;
}

static UINT32 ReadData(const AMBA_CALIB_DATA_STORAGE_INFO_s *StorageInfo, UINT32 Pos, UINT32 Length, UINT8 *Buffer, UINT32 *Ret)
{
    UINT32 Rval = OK;
    UINT32 AddrU32;
    *Ret = 0U;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % AMBA_CACHE_LINE_SIZE) == 0U) {
        if (StorageInfo->StorageId < AMBA_NUM_NVM) {
            if ((StorageInfo->PartId == AMBA_USER_PARTITION_DSP_uCODE) || (StorageInfo->PartId == AMBA_USER_PARTITION_SYS_DATA)) {
                UINT32 RegionId = StorageInfo->PartId - 2U;
                Rval = AmbaNVM_ReadRomFile(StorageInfo->StorageId, RegionId, AMBA_CALIB_DATA_FILE_NAME, Pos, Length, Buffer, AMBA_CALIB_DATA_WAIT_TIMEOUT);
                if (Rval == OK) {
                    *Ret = Length;
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_ReadRomFile failed RegionId %u", __LINE__, RegionId, 0U, 0U, 0U);
                }
            } else {
                Rval = ReadNvm(StorageInfo, Pos, Length, Buffer, Ret);
                if (Rval != OK) {
                   AmbaPrint_PrintUInt5("[calibdata] (%u) ReadNvm failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            if (StorageInfo->File != NULL) {
                Rval = AmbaFS_FileSeek(StorageInfo->File, (INT64)Pos, AMBA_FS_SEEK_START);
                if (Rval == OK) {
                    UINT32 Size;
                    Rval = AmbaFS_FileRead(Buffer, 1U, Length, StorageInfo->File, &Size);
                    if (Rval == OK) {
                        *Ret = Size;
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileRead failed", __LINE__, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileSeek failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) File is NULL", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] : ReadData Buffer Pos %u is not cache aligned", Pos, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static inline UINT8* ParseGeo(UINT8 *Buffer, AMBA_CALIB_DATA_SENSOR_GEOMETRY_s *CalibGeo)
{
    UINT8 *Temp = Buffer;
    Temp = ReadBeU32(Temp, &CalibGeo->StartX);
    Temp = ReadBeU32(Temp, &CalibGeo->StartY);
    Temp = ReadBeU32(Temp, &CalibGeo->Width);
    Temp = ReadBeU32(Temp, &CalibGeo->Height);
    Temp = ReadBeU32(Temp, &CalibGeo->HSubSampleFactorNum);
    Temp = ReadBeU32(Temp, &CalibGeo->HSubSampleFactorDen);
    Temp = ReadBeU32(Temp, &CalibGeo->VSubSampleFactorNum);
    Temp = ReadBeU32(Temp, &CalibGeo->VSubSampleFactorDen);
    return Temp;
}

static inline UINT8* ParseOsdWindow(UINT8 *Buffer, AMBA_DSP_WINDOW_s *OsdWindow)
{
    UINT8 *Temp = Buffer;
    Temp = ReadBeU16(Temp, &OsdWindow->OffsetX);
    Temp = ReadBeU16(Temp, &OsdWindow->OffsetY);
    Temp = ReadBeU16(Temp, &OsdWindow->Width);
    Temp = ReadBeU16(Temp, &OsdWindow->Height);
    return Temp;
}

static UINT32 ParseWarp(AMBA_CALIB_DATA_WARP_INFO_s *Warp, UINT8 *Buffer, UINT32 *OriginalSize)
{
    /*
     * [warp_info] => [sizeof_warp_info][WARP][version][CalibGeo][HorGridNum][VerGridNum][TileWidth][TileHeight][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        Temp = ReadBeU32(Temp, &Warp->Version);
        Temp = ParseGeo(Temp, &Warp->Data.CalibSensorGeo);
        Temp = ReadBeU32(Temp, &Warp->Data.HorGridNum);
        Temp = ReadBeU32(Temp, &Warp->Data.VerGridNum);
        Temp = ReadBeU32(Temp, &Warp->Data.TileWidth);
        Temp = ReadBeU32(Temp, &Warp->Data.TileHeight);
        (void) ReadBeU32(Temp, OriginalSize);
        AmbaPrint_PrintUInt5("ParseWarp : Version %u OrigSize %u", Warp->Version, *OriginalSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ParseVignette(AMBA_CALIB_DATA_VIG_INFO_s *Vig, UINT8 *Buffer, UINT32 *OriginalSize)
{
    /*
     * [sizeof_vig_info][VIGN][version][CalibGeo][RadialCoarse][RadialCoarseLog][RadialBinsFine][RadialBinsFineLog][ModelCenterX_R][ModelCenterX_Gr][ModelCenterX_B][ModelCenterX_Gb]
     *                                                         [ModelCenterY_R][ModelCenterY_Gr][ModelCenterY_B][ModelCenterY_Gb][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        Temp = ReadBeU32(Temp, &Vig->Version);
        Temp = ParseGeo(Temp, &Vig->Data.CalibSensorGeo);
        Temp = ReadBeU32(Temp, &Vig->Data.RadialCoarse);
        Temp = ReadBeU32(Temp, &Vig->Data.RadialCoarseLog);
        Temp = ReadBeU32(Temp, &Vig->Data.RadialBinsFine);
        Temp = ReadBeU32(Temp, &Vig->Data.RadialBinsFineLog);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterX_R);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterX_Gr);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterX_B);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterX_Gb);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterY_R);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterY_Gr);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterY_B);
        Temp = ReadBeU32(Temp, &Vig->Data.ModelCenterY_Gb);
        (void) ReadBeU32(Temp, OriginalSize);
        AmbaPrint_PrintUInt5("ParseVignette : Version %u OrigSize %u", Vig->Version, *OriginalSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ParseBlend(AMBA_CALIB_DATA_BLEND_INFO_s *Blend, UINT8 *Buffer, UINT32 *OriginalSize)
{
    /*
     * [blend_info] =>  [sizeof_blend_info][BLND][version][Width][Height][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        Temp = ReadBeU32(Temp, &Blend->Version);
        Temp = ReadBeU32(Temp, &Blend->Width);
        Temp = ReadBeU32(Temp, &Blend->Height);
        (void) ReadBeU32(Temp, OriginalSize);
        AmbaPrint_PrintUInt5("ParseBlend : Version %u OrigSize %u", Blend->Version, *OriginalSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ParseOsd(AMBA_CALIB_DATA_OSD_INFO_s *Osd, UINT8 *Buffer, UINT32 *OriginalSize)
{
    /*
     * [Osd_info] =>  [sizeof_blend_info][OSDD][version][OsdWindow][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        Temp = ReadBeU32(Temp, &Osd->Version);
        Temp = ParseOsdWindow(Temp, &Osd->OsdWindow);
        (void) ReadBeU32(Temp, OriginalSize);
        AmbaPrint_PrintUInt5("ParseOsd : Version %u OrigSize %u", Osd->Version, *OriginalSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ParseRawData(AMBA_CALIB_DATA_RAW_INFO_s *Raw, UINT8 *Buffer, UINT32 *OriginalSize)
{
    /*
     * [raw_info] =>   [sizeof_raw_info][RAWD][version][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        Temp = ReadBeU32(Temp, &Raw->Version);
        (void) ReadBeU32(Temp, OriginalSize);
        AmbaPrint_PrintUInt5("ParseRawData : Version %u OrigSize %u", Raw->Version, *OriginalSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 ParseElement(UINT8 *Buffer, UINT32 ItemSize, AMBA_CALIB_DATA_ITEM_INFO_s *ItemInfo)
{
    UINT8 ElementCnt = 0U;
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 ReadItemSize = ITEM_INFO_SIZE;
    do {
        if (ReadItemSize >= ItemSize) {
            if (ReadItemSize > ItemSize) {
                AmbaPrint_PrintUInt5("[calibdata] : ParseHeader exceed boundary", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            break;
        }
        if (ElementCnt < AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM) {
            UINT32 OrigSize = 0U;
            UINT32 Size;
            UINT32 Tag;
            Temp = ReadBeU32(Temp, &Size);
            Temp = ReadBeU32(Temp, &Tag);
            if (Tag == WARP) {
                Rval = ParseWarp(&ItemInfo->ElementInfo[ElementCnt].Warp, Temp, &OrigSize);
                ItemInfo->ElementInfo[ElementCnt].Type = AMBA_CALIB_DATA_TYPE_WARP;
                ItemInfo->ElementInfo[ElementCnt].Size = OrigSize;
            } else if (Tag == VIGN) {
                Rval = ParseVignette(&ItemInfo->ElementInfo[ElementCnt].Vig, Temp, &OrigSize);
                ItemInfo->ElementInfo[ElementCnt].Type = AMBA_CALIB_DATA_TYPE_VIG;
                ItemInfo->ElementInfo[ElementCnt].Size = OrigSize;
            } else if (Tag == BLND) {
                Rval = ParseBlend(&ItemInfo->ElementInfo[ElementCnt].Blend, Temp, &OrigSize);
                ItemInfo->ElementInfo[ElementCnt].Type = AMBA_CALIB_DATA_TYPE_BLEND;
                ItemInfo->ElementInfo[ElementCnt].Size = OrigSize;
            } else if (Tag == OSDD) {
                Rval = ParseOsd(&ItemInfo->ElementInfo[ElementCnt].Osd, Temp, &OrigSize);
                ItemInfo->ElementInfo[ElementCnt].Type = AMBA_CALIB_DATA_TYPE_OSD;
                ItemInfo->ElementInfo[ElementCnt].Size = OrigSize;
            } else if (Tag == RAWD) {
                Rval = ParseRawData(&ItemInfo->ElementInfo[ElementCnt].Raw, Temp, &OrigSize);
                ItemInfo->ElementInfo[ElementCnt].Type = AMBA_CALIB_DATA_TYPE_RAW;
                ItemInfo->ElementInfo[ElementCnt].Size = OrigSize;
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Data type 0x%x is not supported , Size = %u", __LINE__, Tag, Size, 0U, 0U);
                Rval = ERR_ARG;
            }
            if (Rval == OK) {
                Temp = &Temp[Size - 8U];
                ReadItemSize += Size;
                ElementCnt++;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] : Invalid format, Element num exceed the limit", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } while(Rval == OK);
    if (Rval == OK) {
        ItemInfo->Count = ElementCnt;
    }
    return Rval;
}

static UINT32 ParseItem(UINT8 *Buffer, UINT32 GroupSize, AMBA_CALIB_DATA_GROUP_DESC_s *Desc, AMBA_CALIB_DATA_GROUP_DESC_DATA_s *DescPtr)
{
    /*
     * [item_info]  =>  [sizeof_item_info][ITMI][ITEM_CRC][Compressed][DataPos][DataSize][warp_info][vig_info][blend_info][osd_info][raw_info]
     */
    UINT8 ItemCount = 0U;
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 ReadGroupSize = GROUP_INFO_SIZE;
    do {
        if (ReadGroupSize >= GroupSize) {
            if (ReadGroupSize > GroupSize) {
                AmbaPrint_PrintUInt5("[calibdata] : ParseHeader exceed boundary", 0U, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            break;
        }
        if (ItemCount < AMBA_CALIB_DATA_MAX_ITEM_PER_GRP) {
            UINT32 ItemSize;
            UINT32 Tag;
            UINT32 Crc;
            UINT32 Compressed;
            UINT32 Pos;
            UINT32 DataSize;
            Temp = ReadBeU32(Temp, &ItemSize);
            Temp = ReadBeU32(Temp, &Tag);
            Temp = ReadBeU32(Temp, &Crc);
            if ((ItemSize > ITEM_INFO_SIZE) && (Tag == ITMI)) {
                Temp = ReadBeU32(Temp, &Compressed);
                Temp = ReadBeU32(Temp, &Pos);
                Temp = ReadBeU32(Temp, &DataSize);
                Desc->ItemInfo[ItemCount].Compressed = (UINT8)Compressed;
                Desc->ItemInfo[ItemCount].Size = DataSize;
                DescPtr->ItemSize[ItemCount] = DataSize;
                DescPtr->DataPos[ItemCount] = Pos;
                DescPtr->Crc[ItemCount] = Crc;
                AmbaPrint_PrintUInt5("[calibdata] : ITMI %u", ItemCount, 0U, 0U, 0U, 0U);
                Rval = ParseElement(Temp, ItemSize, &Desc->ItemInfo[ItemCount]);
                if (Rval == OK) {
                    ReadGroupSize += ItemSize;
                    ItemCount++;
                    Temp = &Temp[ItemSize - ITEM_INFO_SIZE];
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) ParseElement failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid item info!", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] : Invalid format, Item num exceed the limit", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } while(Rval == OK);
    if (Rval == OK) {
        Desc->Count = ItemCount;
        DescPtr->Count = Desc->Count;
    }
    return Rval;
}

static UINT32 ParseGroup(const AMBA_CALIB_DATA_HDLR_s *Hdlr, UINT32 Index, AMBA_CALIB_DATA_GROUP_DESC_s *Desc)
{
     /*
     * [group_info] =>  [sizeof_group_info][GRPI][StorageType][PartId][item_info][item_info]...
     */
    UINT32 Rval = OK;
    AMBA_CALIB_DATA_GROUP_DESC_DATA_s *DescPtr;
    AmbaMisra_TypeCast32(&DescPtr, &G_GroupDescTempBuf);
    if (Index < Hdlr->GroupNum) {
        UINT8 *Temp = DescPtr[Index].DescPtr;
        UINT32 Size;
        UINT32 Tag;
        Temp = ReadBeU32(Temp, &Size);
        Temp = ReadBeU32(Temp, &Tag);
        if ((Size > GROUP_INFO_SIZE) && (Tag == GRPI)) {
            Temp = ReadBeU32(Temp, &Desc->StorageId);
            Temp = ReadBeU32(Temp, &Desc->PartId);
            DescPtr[Index].StorageId = Desc->StorageId;
            DescPtr[Index].PartId = Desc->PartId;
            AmbaPrint_PrintUInt5("[calibdata] : ParseGroup %u, Size %u", Index, Size, 0U, 0U, 0U);
            Rval = ParseItem(Temp, Size, Desc, &DescPtr[Index]);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) ParseItem failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid group info!", __LINE__, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Index %u, Group num is %u", __LINE__, Index, Hdlr->GroupNum, 0U, 0U);
        Rval = ERR_ARG;
    }

    return Rval;
}

static UINT32 ParseHeader(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
/*
 * [sizeof_all][AMBA][sizeof_header][HEAD][HEAD_CRC][group_info][group_info]...[udta_info][udta_info]...[sizeof_free][FREE]... [sizeof_data][DATA][data][data]...
 */
    UINT8 *Temp = &G_HeaderBuf[AMBA_INFO_SIZE];
    UINT32 Rval = OK;
    UINT32 Size;
    UINT32 Tag;
    UINT32 Crc;
    Temp = ReadBeU32(Temp, &Size);
    Temp = ReadBeU32(Temp, &Tag);
    Temp = ReadBeU32(Temp, &Crc);
    if ((Size > HEAD_INFO_SIZE) && (Tag == HEAD)) {
        Hdlr->HeaderSize = Size;
        if (AmbaUtility_Crc32(Temp, Size - HEAD_INFO_SIZE) == Crc) {
            UINT32 ReadSize = HEAD_INFO_SIZE;
            AMBA_CALIB_DATA_GROUP_DESC_DATA_s *DescPtr;
            AmbaPrint_PrintUInt5("ParseHeader BufSize = %u ", Size - HEAD_INFO_SIZE, 0U, 0U, 0U, 0U);
            AmbaMisra_TypeCast32(&DescPtr, &G_GroupDescTempBuf);
            do {
                if (ReadSize >= Hdlr->HeaderSize) {
                    if (ReadSize > Hdlr->HeaderSize) {
                        AmbaPrint_PrintUInt5("[calibdata] : ParseHeader exceed boundary", 0U, 0U, 0U, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                    break;
                }
                (void) ReadBeU32(Temp, &Size);
                (void) ReadBeU32(&Temp[4U], &Tag);
                if (Size > 8U) {
                    AmbaPrint_PrintUInt5("ParseHeader : %u 0x%X", Size, Tag, 0U, 0U, 0U);
                    if (Tag == GRPI) {
                        static AMBA_CALIB_DATA_GROUP_DESC_s Desc;
                        DescPtr[Hdlr->GroupNum].DescPtr = Temp;
                        Hdlr->GroupNum++;
                        /* Parse Desc and keep the part of desc in G_GroupDescTempBuf */
                        Rval = ParseGroup(Hdlr, (Hdlr->GroupNum - 1U), &Desc);
                        if (Rval != OK) {
                            AmbaPrint_PrintUInt5("[calibdata] (%u) ParseGroup failed!", __LINE__, 0U, 0U, 0U, 0U);
                        }
                    } else if (Tag == UDTA) {
                        Hdlr->UdtaPtr[Hdlr->UdtaNum] = Temp;
                        Hdlr->UdtaNum++;
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) Tag 0x%x is not supported , Size = %u", __LINE__, Tag, Size, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                    Temp = &Temp[Size];
                    ReadSize += Size;
                }
            } while (Rval == OK);
            AmbaPrint_PrintUInt5("[calibdata] : ParseHeader  GroupNum=%u UdtaNum=%u", Hdlr->GroupNum, Hdlr->UdtaNum, 0U, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid header, CRC check failed!", __LINE__, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Parse header(HEAD) failed, Tag = 0x%x, Size = %u, Crc = %u", __LINE__, Tag, Size, Crc, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 CreateHeader(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
/*
 * [sizeof_all][AMBA][sizeof_header][HEAD][HEAD_CRC][group_info][group_info]...[udta_info][udta_info]...[sizeof_free][FREE]... [sizeof_data][DATA][data][data]...
 */
    UINT8 *Temp = G_UdtaTempBuf;
    UINT32 Rval = OK;
    Hdlr->HeaderSize = HEAD_INFO_SIZE;
    /* UDTA */
    for (UINT8 I = 0U; I < G_MaxUdta; I++) {
        UINT32 UdtaSize = G_UdtaSize[I];
        Hdlr->UdtaPtr[I] = Temp;
        Temp = &Temp[UdtaSize];
    }
    /* set the first desc ptr */
    Hdlr->GroupDescPtr = &G_HeaderBuf[AMBA_INFO_SIZE + HEAD_INFO_SIZE];
    return Rval;
}

static UINT32 ReadHeader(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
    UINT32 Rval;
    AMBA_CALIB_DATA_STORAGE_INFO_s StorageInfo;
    StorageInfo.StorageId = G_MainStorage.StorageId;
    StorageInfo.PartId = G_MainStorage.PartId;
    Rval = OpenStorage(Hdlr, &StorageInfo);
    /* dump the header (from main storage) */
    if (Rval == OK) {
        UINT32 AlignedSize = 0U;
        Rval = GetAlignedSize(StorageInfo.StorageId, StorageInfo.PartId, &AlignedSize);
        if (Rval == OK) {
            /* read one page/cluster first */
            UINT32 RetSize = 0U;
            Rval = ReadData(&StorageInfo, 0U, AlignedSize, G_HeaderBuf, &RetSize);
            if ((Rval == OK) && (RetSize == AlignedSize)) {
                UINT8 *Temp = G_HeaderBuf;
                UINT32 Size;
                UINT32 Tag;
                Temp = ReadBeU32(Temp, &Size);
                Temp = ReadBeU32(Temp, &Tag);
                if ((Size > AMBA_INFO_SIZE) && (Tag == AMBA)) {
                    Temp = ReadBeU32(Temp, &Size);
                    (void) ReadBeU32(Temp, &Tag);
                    if ((Size > HEAD_INFO_SIZE) && (Tag == HEAD)) {
                         UINT32 HeaderSize = Size + AMBA_INFO_SIZE;
                         if (HeaderSize > AlignedSize) {
                            /* read the last header */
                            Rval = ReadData(&StorageInfo, AlignedSize, (HeaderSize - AlignedSize), &G_HeaderBuf[AlignedSize], &RetSize);
                            if (RetSize != (HeaderSize - AlignedSize)) {
                                AmbaPrint_PrintUInt5("[calibdata] (%u) dump the last header (from main storage) failed. ReadSize(%u) RetSize(%u)", __LINE__, (HeaderSize - AlignedSize), RetSize, 0U, 0U);
                                Rval = ERR_ARG;
                            }
                         }
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) Parse header(HEAD) failed, Tag = 0x%x, Size = %u", __LINE__, Tag, Size, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) Parse header(AMBA) failed, Tag = 0x%x, Size = %u", __LINE__, Tag, Size, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) dump the header (from main storage) failed. ReadSize(%u) RetSize(%u)", __LINE__, AlignedSize, RetSize, 0U, 0U);
                Rval = ERR_ARG;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) OpenStorage failed", __LINE__, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 OpenHandler(UINT8 Mode, AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
    UINT32 Rval = OK;
    /* reset hdlr */
    (void) AmbaWrap_memset(Hdlr, 0, sizeof(AMBA_CALIB_DATA_HDLR_s));
    if (Mode == AMBA_CALIB_DATA_MODE_READ) {
        Hdlr->WriteOnClose = 0U;
        Rval = ReadHeader(Hdlr);
        if (Rval == OK) {
            Rval = ParseHeader(Hdlr);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) ParseHeader failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) DumpHeader failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else if (Mode == AMBA_CALIB_DATA_MODE_WRITE) {
        Hdlr->WriteOnClose = 1U;
        (void) AmbaWrap_memset(G_StorageMrg.NvmUsage, 0, sizeof(G_StorageMrg.NvmUsage));
        (void) AmbaWrap_memset(G_StorageMrg.DeviceUsage, 0, sizeof(G_StorageMrg.DeviceUsage));
        /* update storage usage for header */
        if (G_MainStorage.StorageId < AMBA_NUM_NVM) {
            G_StorageMrg.NvmUsage[G_MainStorage.StorageId][G_MainStorage.PartId] = G_HeaderSize;
        } else {
            UINT8 FileIdx;
            Rval = GetDriveIdx(G_MainStorage.PartId, &FileIdx);
            if (Rval == OK) {
                G_StorageMrg.DeviceUsage[FileIdx] = G_HeaderSize;
            }
        }
        if (Rval == OK) {
            Rval = CreateHeader(Hdlr);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) CreateHeader failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Mode %u is not supported", __LINE__, Mode, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    if (Rval == OK) {
        Hdlr->Used = 1U;
    }
    AmbaPrint_PrintUInt5("[calibdata] (%u) OpenHandler done", __LINE__, 0U, 0U, 0U, 0U);
    return Rval;
}

static UINT32 Create_ParamCheck(UINT8 Mode, const UINT32 *Hdlr)
{
    UINT32 Rval = ERR_ARG;
    if (Mode > AMBA_CALIB_DATA_MODE_WRITE) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Mode", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Hdlr == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Create handler to execute read or write
* @param [in]  Mode Read mode or write mode
* @param [out] Hdlr Handler ID
* @return ErrorCode
*/
UINT32 AmbaCalibData_Create(UINT8 Mode, UINT32 *Hdlr)
{
    UINT32 Rval = OK;
    if (G_HdlrNum > 0U) {
        UINT8 WriteModeOn = 0U;
        Rval = Create_ParamCheck(Mode, Hdlr);
        if (Rval == OK) {
            for (UINT8 I = 0U; I < G_HdlrNum; I++) {
                UINT8 Found = 0U;
                AMBA_CALIB_DATA_HDLR_s *CalibHdlr = &G_Hdlr[I];
                Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
                if (Rval == OK) {
                    UINT32 Ret;
                    if (CalibHdlr->Used == 0U) {
                        /* only one handler for write mode at one time */
                        if ((WriteModeOn == 1U) && (Mode == AMBA_CALIB_DATA_MODE_WRITE)) {
                            AmbaPrint_PrintUInt5("[calibdata] (%u) only one handler for write mode at one time", __LINE__, 0U, 0U, 0U, 0U);
                            Rval = ERR_ARG;
                        } else {
                            Rval = OpenHandler(Mode, CalibHdlr);
                            *Hdlr = I;
                            Found = 1U;
                        }
                    } else {
                        if (CalibHdlr->WriteOnClose == 1U) {
                            WriteModeOn = 1U;
                        }
                    }
                    Ret = AmbaKAL_MutexGive(&G_Mutex);
                    if (Ret != OK) {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                        Rval = Ret;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
                }
                if ((Rval != OK) || (Found != 0U)) {
                    break;
                }
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Do not init yet!", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteNvm(const AMBA_CALIB_DATA_STORAGE_INFO_s *NvmInfo, UINT32 Pos, UINT32 Length, UINT8 *Buffer)
{
    // Pos & Length must be sector aligned, or it will have an impact on NAND's lifetime
    UINT32 Rval;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    const AMBA_NVM_FTL_INFO_s *NftlInfo = &G_StorageMrg.NftlInfo[NvmInfo->StorageId];
    if (((Pos % NftlInfo->SectorSizeInBytes) == 0U) && ((Length % NftlInfo->SectorSizeInBytes) == 0U)) {
        SecConfig.StartSector = Pos / NftlInfo->SectorSizeInBytes;
        SecConfig.NumSector = Length / NftlInfo->SectorSizeInBytes;
        SecConfig.pDataBuf = Buffer;
        Rval = AmbaNVM_WriteSector(NvmInfo->StorageId, NvmInfo->PartId, &SecConfig, AMBA_CALIB_DATA_WAIT_TIMEOUT);
        if (Rval != OK) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaNVM_WriteSector failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Pos & Length must be sector aligned", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteData(const AMBA_CALIB_DATA_STORAGE_INFO_s *StorageInfo, UINT32 Pos, UINT32 Length, UINT8 *Buffer)
{
    UINT32 Rval = OK;
    if (StorageInfo->StorageId < AMBA_NUM_NVM) {
        if ((StorageInfo->PartId == AMBA_USER_PARTITION_DSP_uCODE) || (StorageInfo->PartId == AMBA_USER_PARTITION_SYS_DATA)) {
            AmbaPrint_PrintUInt5("Do not support writing data into ROM.", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            Rval = WriteNvm(StorageInfo, Pos, Length, Buffer);
        }
    } else {
        if (StorageInfo->File != NULL) {
            Rval = AmbaFS_FileSeek(StorageInfo->File, (INT64)Pos, AMBA_FS_SEEK_START);
            if (Rval == OK) {
                UINT32 Size;
                Rval = AmbaFS_FileWrite(Buffer, 1, Length, StorageInfo->File, &Size);
                if (Rval == OK) {
                    if (Size != Length) {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) Write Size %u should be %u", __LINE__, Size, Length, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileWrite failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileSeek failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) File is NULL", __LINE__, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    }

    return Rval;
}

static void PackUdta(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
    const UINT8 *SrcPtr;
    UINT8 *DstPtr = &G_HeaderBuf[(AMBA_INFO_SIZE + Hdlr->HeaderSize)];
    for (UINT8 I = 0U; I < Hdlr->UdtaNum; I++) {
        UINT32 Size = UDTA_INFO_SIZE + Hdlr->UdtaSize[I];
        UINT32 Tag = UDTA;
        SrcPtr = Hdlr->UdtaPtr[I];
        DstPtr = WriteBeU32(DstPtr, Size);
        DstPtr = WriteBeU32(DstPtr, Tag);
        (void) AmbaWrap_memcpy(DstPtr, SrcPtr, Hdlr->UdtaSize[I]);
        DstPtr = &DstPtr[Hdlr->UdtaSize[I]];
        Hdlr->HeaderSize += Size;
    }
}

static UINT32 PackData(const AMBA_CALIB_DATA_STORAGE_INFO_s *StorageInfo)
{
    UINT32 Rval = OK;
    UINT8 *Temp;
    UINT8 Buffer[AMBA_CALIB_DATA_SECTOR_SIZE] GNU_ALIGNED_CACHESAFE;
    UINT32 WriteSize;
    UINT32 Size = 0U;
    UINT32 Tag = DATA;
    if (StorageInfo->StorageId < AMBA_NUM_NVM) {
        WriteSize = AMBA_CALIB_DATA_SECTOR_SIZE;
        Size = G_StorageMrg.NvmUsage[StorageInfo->StorageId][StorageInfo->PartId];
    } else {
        UINT8 FileIdx;
        WriteSize = DATA_INFO_SIZE;
        Rval = GetDriveIdx(StorageInfo->PartId, &FileIdx);
        if (Rval == OK) {
            Size = G_StorageMrg.DeviceUsage[FileIdx];
        }
    }
    if (Rval == OK) {
        Temp = Buffer;
        Temp = WriteBeU32(Temp, Size);
        (void) WriteBeU32(Temp, Tag);
        Rval = WriteData(StorageInfo, 0U, WriteSize, Buffer);
        if (Rval != OK) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) WriteData failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 FlushHeader(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
/*
 * [sizeof_all][AMBA][sizeof_header][HEAD][HEAD_CRC][group_info][group_info]...[udta_info][udta_info]...[sizeof_free][FREE]... [sizeof_data][DATA][data][data]...
 */
    UINT8 *Temp = G_HeaderBuf;
    UINT8 I, J;
    UINT8 FileIdx = AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT;
    UINT32 Rval = OK;
    UINT32 Size = 0U;
    UINT32 Tag = 0U;
    UINT32 Crc = 0U;
    UINT32 TotalDataSize = 0U;
    AMBA_CALIB_DATA_STORAGE_INFO_s StorageInfo = {0U};
    if (G_MainStorage.StorageId < AMBA_NUM_NVM) {
        TotalDataSize = G_StorageMrg.NvmUsage[G_MainStorage.StorageId][G_MainStorage.PartId]; // include header size
    } else {
        Rval = GetDriveIdx(G_MainStorage.PartId, &FileIdx);
        if (Rval == OK) {
            TotalDataSize = G_StorageMrg.DeviceUsage[FileIdx];
        }
    }
    if (Rval == OK) {
        /* AMBA */
        Size = TotalDataSize;
        Tag = AMBA;
        Temp = WriteBeU32(Temp, Size);
        Temp = WriteBeU32(Temp, Tag);
        /* HEAD */
        PackUdta(Hdlr); /* copy Udta to header */
        Size = Hdlr->HeaderSize;
        Tag = HEAD;
        if (Size > HEAD_INFO_SIZE) {
            Crc = AmbaUtility_Crc32(&G_HeaderBuf[AMBA_INFO_SIZE + HEAD_INFO_SIZE], (Hdlr->HeaderSize - HEAD_INFO_SIZE));
        }
        Temp = WriteBeU32(Temp, Size);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Crc);
        /* FREE */
        Size = G_HeaderSize - (AMBA_INFO_SIZE + DATA_INFO_SIZE + Hdlr->HeaderSize);
        Tag = FREE;
        Temp = &Temp[(Hdlr->HeaderSize - HEAD_INFO_SIZE)];
        Temp = WriteBeU32(Temp, Size);
        (void) WriteBeU32(Temp, Tag);
        /* DATA */
        Temp = &G_HeaderBuf[(G_HeaderSize - DATA_INFO_SIZE)];
        Size = DATA_INFO_SIZE + (TotalDataSize - G_HeaderSize);
        Tag = DATA;
        Temp = WriteBeU32(Temp, Size);
        (void) WriteBeU32(Temp, Tag);
        /* other storage */
        for (I = 0U; I < AMBA_NUM_NVM; I++) {
            for (J = 0U; J < AMBA_NUM_USER_PARTITION; J++) {
                if (G_StorageMrg.NvmUsage[I][J] != 0U) {
                    if ((I != G_MainStorage.StorageId) && (J != G_MainStorage.PartId)) {
                        StorageInfo.StorageId = I;
                        StorageInfo.PartId = J;
                        Rval = PackData(&StorageInfo);
                        if (Rval != OK) {
                            break;
                        }
                    }
                }
            }
        }
        for (I = 0U; I < AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT; I++) {
            if (G_StorageMrg.DeviceUsage[I] != 0U) {
                if ((G_MainStorage.StorageId != AMBA_CALIB_DATA_STORAGE_SD) && (I != FileIdx)) {
                    UINT32 CharA = (UINT32)'A';
                    StorageInfo.StorageId = AMBA_CALIB_DATA_STORAGE_SD;
                    StorageInfo.PartId = (CharA + I);
                    StorageInfo.File = Hdlr->File[I];
                    Rval = PackData(&StorageInfo);
                    if (Rval != OK) {
                        break;
                    }
                }
            }
        }
        if (Rval == OK) {
            StorageInfo.StorageId = G_MainStorage.StorageId;
            StorageInfo.PartId = G_MainStorage.PartId;
            Rval = OpenStorage(Hdlr, &StorageInfo);
            if (Rval == OK) {
                Rval = WriteData(&StorageInfo, 0U, G_HeaderSize, G_HeaderBuf);
                if (Rval != OK) {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) WriteData failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) OpenStorage failed", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Rval;
}

static UINT32 CloseHandler(AMBA_CALIB_DATA_HDLR_s *Hdlr)
{
    UINT32 Rval = OK;
    if (Hdlr->WriteOnClose != 0U) {
        Rval = FlushHeader(Hdlr);
    }
    if (Rval == OK) {
        for (UINT8 I = 0U; I < AMBA_CALIB_DATA_MAX_DRIVE_AMOUNT; I++) {
            if (Hdlr->File[I] != NULL) {
                Rval = AmbaFS_FileClose(Hdlr->File[I]);
                if (Rval != OK) {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaFS_FileClose(%u) failed", __LINE__, I, 0U, 0U, 0U);
                    break;
                }
            }
        }
    }
    if (Rval == OK) {
        Hdlr->Used = 0U;
    }
    return Rval;
}

static UINT32 Delete_ParamCheck(UINT32 Hdlr)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Delete handler
* @param [in]  Hdlr Handler ID
* @return ErrorCode
*/
UINT32 AmbaCalibData_Delete(UINT32 Hdlr)
{
    UINT32 Rval = Delete_ParamCheck(Hdlr);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                Rval = CloseHandler(CalibHdlr);
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used Id = %u", __LINE__, Hdlr, 0U, 0U, 0U);
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static inline UINT8* FillGeo(UINT8 *Buffer, AMBA_CALIB_DATA_SENSOR_GEOMETRY_s CalibGeo)
{
    UINT8 *Temp = Buffer;
    Temp = WriteBeU32(Temp, CalibGeo.StartX);
    Temp = WriteBeU32(Temp, CalibGeo.StartY);
    Temp = WriteBeU32(Temp, CalibGeo.Width);
    Temp = WriteBeU32(Temp, CalibGeo.Height);
    Temp = WriteBeU32(Temp, CalibGeo.HSubSampleFactorNum);
    Temp = WriteBeU32(Temp, CalibGeo.HSubSampleFactorDen);
    Temp = WriteBeU32(Temp, CalibGeo.VSubSampleFactorNum);
    Temp = WriteBeU32(Temp, CalibGeo.VSubSampleFactorDen);
    return Temp;
}

static inline UINT8* FillOsdWindow(UINT8 *Buffer, AMBA_DSP_WINDOW_s OsdWindow)
{
    UINT8 *Temp = Buffer;
    Temp = WriteBeU16(Temp, OsdWindow.OffsetX);
    Temp = WriteBeU16(Temp, OsdWindow.OffsetY);
    Temp = WriteBeU16(Temp, OsdWindow.Width);
    Temp = WriteBeU16(Temp, OsdWindow.Height);
    return Temp;
}

static UINT32 WriteWarp(AMBA_CALIB_DATA_WARP_DATA_s *Warp, UINT8 *Buffer)
{
    /*
     * [warp_info] => [sizeof_warp_info][WARP][version][CalibGeo][HorGridNum][VerGridNum][TileWidth][TileHeight][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        UINT32 Tag = WARP;
        Warp->AtomSize = WARP_INFO_SIZE;
        Temp = WriteBeU32(Temp, Warp->AtomSize);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Warp->Info->Version);
        Temp = FillGeo(Temp, Warp->Info->Data.CalibSensorGeo);
        Temp = WriteBeU32(Temp, Warp->Info->Data.HorGridNum);
        Temp = WriteBeU32(Temp, Warp->Info->Data.VerGridNum);
        Temp = WriteBeU32(Temp, Warp->Info->Data.TileWidth);
        Temp = WriteBeU32(Temp, Warp->Info->Data.TileHeight);
        (void) WriteBeU32(Temp, Warp->OrigSize);
        AmbaPrint_PrintUInt5("WriteWarp : Version %u OrigSize %u", Warp->Info->Version, Warp->OrigSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteVignette(AMBA_CALIB_DATA_VIG_DATA_s *Vig, UINT8 *Buffer)
{
    /*
     * [sizeof_vig_info][VIGN][version][CalibGeo][RadialCoarse][RadialCoarseLog][RadialBinsFine][RadialBinsFineLog][ModelCenterX_R][ModelCenterX_Gr][ModelCenterX_B][ModelCenterX_Gb]
     *                                                         [ModelCenterY_R][ModelCenterY_Gr][ModelCenterY_B][ModelCenterY_Gb][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        UINT32 Tag = VIGN;
        Vig->AtomSize = VIG_INFO_SIZE;
        Temp = WriteBeU32(Temp, Vig->AtomSize);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Vig->Info->Version);
        Temp = FillGeo(Temp, Vig->Info->Data.CalibSensorGeo);
        Temp = WriteBeU32(Temp, Vig->Info->Data.RadialCoarse);
        Temp = WriteBeU32(Temp, Vig->Info->Data.RadialCoarseLog);
        Temp = WriteBeU32(Temp, Vig->Info->Data.RadialBinsFine);
        Temp = WriteBeU32(Temp, Vig->Info->Data.RadialBinsFineLog);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterX_R);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterX_Gr);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterX_B);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterX_Gb);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterY_R);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterY_Gr);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterY_B);
        Temp = WriteBeU32(Temp, Vig->Info->Data.ModelCenterY_Gb);
        (void) WriteBeU32(Temp, Vig->OrigSize);
        AmbaPrint_PrintUInt5("WriteVignette : Version %u OrigSize %u", Vig->Info->Version, Vig->OrigSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteBlend(AMBA_CALIB_DATA_BLEND_DATA_s *Blend, UINT8 *Buffer)
{
    /*
     * [blend_info] =>  [sizeof_blend_info][BLND][version][Width][Height][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        UINT32 Tag = BLND;
        Blend->AtomSize = BLEND_INFO_SIZE;
        Temp = WriteBeU32(Temp, Blend->AtomSize);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Blend->Info->Version);
        Temp = WriteBeU32(Temp, Blend->Info->Width);
        Temp = WriteBeU32(Temp, Blend->Info->Height);
        (void) WriteBeU32(Temp, Blend->OrigSize);
        AmbaPrint_PrintUInt5("WriteBlend : Version %u OrigSize %u", Blend->Info->Version, Blend->OrigSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteOsd(AMBA_CALIB_DATA_OSD_DATA_s *Osd, UINT8 *Buffer)
{
    /*
     * [Osd_info] =>  [sizeof_blend_info][OSDD][version][OsdWindow][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        UINT32 Tag = OSDD;
        Osd->AtomSize = OSD_INFO_SIZE;
        Temp = WriteBeU32(Temp, Osd->AtomSize);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Osd->Info->Version);
        Temp = FillOsdWindow(Temp, Osd->Info->OsdWindow);
        (void) WriteBeU32(Temp, Osd->OrigSize);
        AmbaPrint_PrintUInt5("WriteOsd : Version %u OrigSize %u", Osd->Info->Version, Osd->OrigSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteRawData(AMBA_CALIB_DATA_RAW_DATA_s *Raw, UINT8 *Buffer)
{
    /*
     * [raw_info] =>   [sizeof_raw_info][RAWD][version][OrigSize]
     */
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        UINT32 Tag = RAWD;
        Raw->AtomSize = RAW_INFO_SIZE;
        Temp = WriteBeU32(Temp, Raw->AtomSize);
        Temp = WriteBeU32(Temp, Tag);
        Temp = WriteBeU32(Temp, Raw->Info->Version);
        (void) WriteBeU32(Temp, Raw->OrigSize);
        AmbaPrint_PrintUInt5("WriteRawData : Version %u OrigSize %u", Raw->Info->Version, Raw->OrigSize, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid address 0x%x", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 WriteElement(AMBA_CALIB_DATA_HDLR_s *Hdlr, const AMBA_CALIB_DATA_ITEM_DATA_s *Item, UINT8 *Buffer, UINT32 *Size)
{
    UINT8 *Temp = Buffer;
    UINT32 Rval = OK;
    UINT32 ElementSize = 0U;
    for (UINT8 I = 0U; I < Item->Info->Count; I++) {
        if (Item->Info->ElementInfo[I].Type == AMBA_CALIB_DATA_TYPE_WARP) {
            if (Hdlr->WarpNum < G_MaxWarp) {
                AMBA_CALIB_DATA_WARP_DATA_s Warp = {0U};
                Warp.Info = &Item->Info->ElementInfo[I].Warp;
                Warp.OrigSize = Item->Info->ElementInfo[I].Size;
                Rval = WriteWarp(&Warp, Temp);
                if (Rval == OK) {
                    Temp = &Temp[Warp.AtomSize];
                    ElementSize += Warp.AtomSize;
                    Hdlr->WarpNum++;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Warp num exceed maximum = %u", __LINE__, G_MaxWarp, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else if (Item->Info->ElementInfo[I].Type == AMBA_CALIB_DATA_TYPE_VIG) {
            if (Hdlr->VigNum < G_MaxVig) {
                AMBA_CALIB_DATA_VIG_DATA_s Vig = {0U};
                Vig.Info = &Item->Info->ElementInfo[I].Vig;
                Vig.OrigSize = Item->Info->ElementInfo[I].Size;
                Rval = WriteVignette(&Vig, Temp);
                if (Rval == OK) {
                    Temp = &Temp[Vig.AtomSize];
                    ElementSize += Vig.AtomSize;
                    Hdlr->VigNum++;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Vig num exceed maximum = %u", __LINE__, G_MaxVig, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else if (Item->Info->ElementInfo[I].Type == AMBA_CALIB_DATA_TYPE_BLEND) {
            if (Hdlr->BlendNum < G_MaxBlend) {
                AMBA_CALIB_DATA_BLEND_DATA_s Blend = {0U};
                Blend.Info = &Item->Info->ElementInfo[I].Blend;
                Blend.OrigSize = Item->Info->ElementInfo[I].Size;
                Rval = WriteBlend(&Blend, Temp);
                if (Rval == OK) {
                    Temp = &Temp[Blend.AtomSize];
                    ElementSize += Blend.AtomSize;
                    Hdlr->BlendNum++;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Blend num exceed maximum = %u", __LINE__, G_MaxBlend, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else if (Item->Info->ElementInfo[I].Type == AMBA_CALIB_DATA_TYPE_OSD) {
            if (Hdlr->OsdNum < G_MaxOsd) {
                AMBA_CALIB_DATA_OSD_DATA_s Osd = {0U};
                Osd.Info = &Item->Info->ElementInfo[I].Osd;
                Osd.OrigSize = Item->Info->ElementInfo[I].Size;
                Rval = WriteOsd(&Osd, Temp);
                if (Rval == OK) {
                    Temp = &Temp[Osd.AtomSize];
                    ElementSize += Osd.AtomSize;
                    Hdlr->OsdNum++;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Osd num exceed maximum = %u", __LINE__, G_MaxOsd, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else if (Item->Info->ElementInfo[I].Type == AMBA_CALIB_DATA_TYPE_RAW) {
            if (Hdlr->RawNum < G_MaxRaw) {
                AMBA_CALIB_DATA_RAW_DATA_s Raw = {0U};
                Raw.Info = &Item->Info->ElementInfo[I].Raw;
                Raw.OrigSize = Item->Info->ElementInfo[I].Size;
                Rval = WriteRawData(&Raw, Temp);
                if (Rval == OK) {
                    Temp = &Temp[Raw.AtomSize];
                    ElementSize += Raw.AtomSize;
                    Hdlr->RawNum++;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Raw Data num exceed maximum = %u", __LINE__, G_MaxRaw, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Data type 0x%x is not supported.", __LINE__, Item->Info->ElementInfo[I].Type, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
        if (Rval != OK) {
            break;
        }
    }
    if (Rval == OK) {
        *Size = ElementSize;
    }
    return Rval;
}

static UINT32 WriteItem(AMBA_CALIB_DATA_HDLR_s *Hdlr, AMBA_CALIB_DATA_ITEM_DATA_s *Item, UINT8 *Buffer)
{
    /*
     * [sizeof_item_info][ITMI][ITEM_CRC][Compressed][DataPos][DataSize][warp_info][vig_info][blend_info][osd_info][raw_info]
     */
    UINT32 Rval = OK;
    UINT32 ElementSize = 0U;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Buffer);
    if ((AddrU32 % 4U) == 0U) {
        if (Item->Info->Count <= AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM) {
            Rval = WriteElement(Hdlr, Item, &Buffer[ITEM_INFO_SIZE], &ElementSize);
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Amount of element(%u) exceeds the limit(%u)", __LINE__, Item->Info->Count, AMBA_CALIB_DATA_MAX_ELEMENT_PER_ITEM, 0U, 0U);
            Rval = ERR_ARG;
        }
        if (Rval == OK) {
            UINT8 *Temp = Buffer;
            UINT32 Tag = ITMI;
            Item->AtomSize = ITEM_INFO_SIZE + ElementSize;
            Temp = WriteBeU32(Temp, Item->AtomSize);
            Temp = WriteBeU32(Temp, Tag);
            Temp = WriteBeU32(Temp, Item->Crc);
            Temp = WriteBeU32(Temp, Item->Info->Compressed);
            Temp = WriteBeU32(Temp, Item->DataPos);
            (void) WriteBeU32(Temp, Item->Info->Size);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) invalid Address 0x%X", __LINE__, AddrU32, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 GetStorageStartPos(UINT32 StorageId, UINT32 PartId, UINT32 *StorageStartPos)
{
    UINT32 Rval = OK;
    if (StorageId < AMBA_NUM_NVM) {
        /* reservation for DATA box in other storage case */
        if ((StorageId != G_MainStorage.StorageId) || (PartId != G_MainStorage.PartId)) {
            if (G_StorageMrg.NvmUsage[StorageId][PartId] == 0U) {
                UINT32 AlignedSize = 0U;
                Rval = GetAlignedSize(StorageId, PartId, &AlignedSize);
                G_StorageMrg.NvmUsage[StorageId][PartId] += AlignedSize;
            }
        }
        *StorageStartPos = G_StorageMrg.NvmUsage[StorageId][PartId];
    } else {
        UINT8 FileIdx;
        Rval = GetDriveIdx(PartId, &FileIdx);
        if (Rval == OK) {
            /* reservation for DATA box in other storage case */
            if (PartId != G_MainStorage.PartId) {
                if (G_StorageMrg.DeviceUsage[FileIdx] == 0U) {
                    G_StorageMrg.DeviceUsage[FileIdx] += G_StorageMrg.ClusterSize[FileIdx];
                }
            }
            *StorageStartPos = G_StorageMrg.DeviceUsage[FileIdx];
        }
    }
    return Rval;
}

static UINT32 WriteStorage(const AMBA_CALIB_DATA_STORAGE_INFO_s *StorageInfo, UINT32 Pos, UINT32 DataSize, UINT8 *Buffer)
{
    UINT32 Rval;
    UINT32 AlignedSize;
    Rval = GetAlignedSize(StorageInfo->StorageId, StorageInfo->PartId, &AlignedSize);
    if (Rval == OK) {
        UINT32 DataAlignedSize = GetAlignedValU32(DataSize, AlignedSize);
        Rval = WriteData(StorageInfo, Pos, DataAlignedSize, Buffer);
        if (Rval == OK) {
            /* update storage usage */
            if (StorageInfo->StorageId < AMBA_NUM_NVM) {
                G_StorageMrg.NvmUsage[StorageInfo->StorageId][StorageInfo->PartId] += DataAlignedSize;
            } else {
                UINT8 FileIdx;
                Rval = GetDriveIdx(StorageInfo->PartId, &FileIdx);
                if (Rval == OK) {
                    G_StorageMrg.DeviceUsage[FileIdx] += DataAlignedSize;
                }
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) WriteData failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 WriteGroup(AMBA_CALIB_DATA_HDLR_s *Hdlr, const AMBA_CALIB_DATA_GROUP_DESC_s *Desc, UINT8 *Buffer, UINT32 Size)
{
    /*
     * [group_info] =>  [sizeof_group_info][GRPI][StorageType][PartId][item_info][item_info]...
     */
    UINT32 Rval = OK;
    if (Hdlr->GroupNum < G_MaxGroup) {
        UINT32 StorageStartPos = 0U;
        UINT32 DataSize = 0U;
        AMBA_CALIB_DATA_STORAGE_INFO_s StorageInfo;
        StorageInfo.StorageId = Desc->StorageId;
        StorageInfo.PartId = Desc->PartId;
        AmbaPrint_PrintUInt5("[calibdata] GROUP(%u)", Hdlr->GroupNum, 0U, 0U, 0U, 0U);
        /* write item */
        Rval = GetStorageStartPos(StorageInfo.StorageId, StorageInfo.PartId, &StorageStartPos);
        if (Rval == OK) {
            if (Desc->Count <= AMBA_CALIB_DATA_MAX_ITEM_PER_GRP) {
                UINT8 *Temp = Hdlr->GroupDescPtr;
                UINT32 AtomSize = GROUP_INFO_SIZE;
                AMBA_CALIB_DATA_ITEM_DATA_s Item = {0U};
                Temp = &Temp[GROUP_INFO_SIZE];
                for (UINT8 I = 0U; I < Desc->Count; I++) {
                    Item.DataPos = StorageStartPos + DataSize;
                    Item.Info = &Desc->ItemInfo[I];
                    Item.Crc = AmbaUtility_Crc32(&Buffer[DataSize], Desc->ItemInfo[I].Size);
                    AmbaPrint_PrintUInt5("[calibdata] ITMI(%u) Item Size (%u)", I, Desc->ItemInfo[I].Size, 0U, 0U, 0U);
                    Rval = WriteItem(Hdlr, &Item, Temp);
                    if (Rval == OK) {
                        DataSize += Desc->ItemInfo[I].Size;
                        Temp = &Temp[Item.AtomSize];
                        AtomSize += Item.AtomSize;
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) WriteItem failed", __LINE__, 0U, 0U, 0U, 0U);
                        break;
                    }
                }
                if (Rval == OK) {//update Hdlr info
                    UINT32 Tag = GRPI;
                    Temp = Hdlr->GroupDescPtr;
                    Temp = WriteBeU32(Temp, AtomSize);
                    Temp = WriteBeU32(Temp, Tag);
                    Temp = WriteBeU32(Temp, Desc->StorageId);
                    Temp = WriteBeU32(Temp, Desc->PartId);
                    Hdlr->HeaderSize += AtomSize;
                    Hdlr->GroupNum++;
                    Hdlr->GroupDescPtr = &Temp[AtomSize - GROUP_INFO_SIZE];
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Amount of item(%u) exceeds the limit(%u)", __LINE__, Desc->Count, AMBA_CALIB_DATA_MAX_ITEM_PER_GRP, 0U, 0U);
                Rval = ERR_ARG;
            }
        }
        /* write data to storage */
        if (Rval == OK) {
            Rval = OpenStorage(Hdlr, &StorageInfo);
            if (Rval == OK) {
                if (DataSize <= Size) {
                    Rval = WriteStorage(&StorageInfo, StorageStartPos, DataSize, Buffer);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) Buffer size (%u) is smaller than DataSize (%u)", __LINE__, Size, DataSize, 0U, 0U);
                    Rval = ERR_ARG;
                }
            }
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Group num exceed maximum = %u", __LINE__, G_MaxGroup, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 Write_ParamCheck(UINT32 Hdlr, const AMBA_CALIB_DATA_GROUP_DESC_s *Desc, const UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Desc == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Buffer == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Size == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Size", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Write data to storage and save descriptor to main storage (unit: Group)
* @param [in]  Hdlr Handler ID
* @param [in]  Desc Group descriptor
* @param [in]  Buffer Data buffer
* @param [in]  Size Data size
* @return ErrorCode
*/
UINT32 AmbaCalibData_Write(UINT32 Hdlr, const AMBA_CALIB_DATA_GROUP_DESC_s *Desc, UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = Write_ParamCheck(Hdlr, Desc, Buffer, Size);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                if (CalibHdlr->WriteOnClose != 0U) {
                    Rval = WriteGroup(CalibHdlr, Desc, Buffer, Size);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) it is not on write mode", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used Id = %u", __LINE__, Hdlr, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 ReadStorage(AMBA_CALIB_DATA_HDLR_s *Hdlr, UINT32 Index, UINT8 Offset, UINT8 Count, UINT8 *Buffer, UINT32 Size)
{
    UINT8 I;
    UINT32 Rval = OK;
    if (Index < Hdlr->GroupNum) {
        const AMBA_CALIB_DATA_GROUP_DESC_DATA_s *DescPtr;
        AmbaMisra_TypeCast32(&DescPtr, &G_GroupDescTempBuf);
        if ((Offset + Count) <= DescPtr[Index].Count) {
            UINT32 ReadSize = 0U;
            for (I = 0U; I < Count; I++) {
                ReadSize += DescPtr[Index].ItemSize[Offset + I];
            }
            if (ReadSize <= Size) {
                AMBA_CALIB_DATA_STORAGE_INFO_s StorageInfo;
                StorageInfo.StorageId = DescPtr[Index].StorageId;
                StorageInfo.PartId = DescPtr[Index].PartId;
                Rval = OpenStorage(Hdlr, &StorageInfo);
                if (Rval == OK) {
                    UINT32 RetSize = 0U;
                    Rval = ReadData(&StorageInfo, DescPtr[Index].DataPos[Offset], ReadSize, Buffer, &RetSize);
                    if ((Rval == OK) && (RetSize == ReadSize)) {
                        UINT32 Pos = 0U;
                        /* check crc */
                        for (I = Offset; I < (Offset + Count); I++) {
                            if (DescPtr[Index].Crc[I] == AmbaUtility_Crc32(&Buffer[Pos], DescPtr[Index].ItemSize[I])) {
                                Pos += DescPtr[Index].ItemSize[I];
                            } else {
                                AmbaPrint_PrintUInt5("[calibdata] (%u) CRC check failed!", __LINE__, 0U, 0U, 0U, 0U);
                                Rval = ERR_ARG;
                                break;
                            }
                        }
                    } else {
                        AmbaPrint_PrintUInt5("[Calib] ReadData failed! ReadSize (%u) RetSize (%u) ", ReadSize, RetSize, 0U, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) OpenStorage failed", __LINE__, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) invalid parameter", __LINE__, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Index exceed total num = %u", __LINE__, Hdlr->GroupNum, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 Read_ParamCheck(UINT32 Hdlr, UINT8 Offset, UINT8 Count, const UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Offset >= AMBA_CALIB_DATA_MAX_ITEM_PER_GRP) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Offset", __LINE__, 0U, 0U, 0U, 0U);
    } else if ((Count == 0U) || (Count > AMBA_CALIB_DATA_MAX_ITEM_PER_GRP)) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Count", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Buffer == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Size == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Size", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Read specified data from storage(MAX unit: Group/ min unit: Item)
* @param [in]  Hdlr Handler ID
* @param [in]  Index Group ID of data
* @param [in]  Offset The first item to be read
* @param [in]  Count Item number of data
* @param [in]  Buffer Output buffer for data
* @param [in]  Size Buffer size
* @return ErrorCode
*/
UINT32 AmbaCalibData_Read(UINT32 Hdlr, UINT32 Index, UINT8 Offset, UINT8 Count, UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = Read_ParamCheck(Hdlr, Offset, Count, Buffer, Size);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                if (CalibHdlr->WriteOnClose == 0U) {
                    if (Buffer != NULL) {
                        Rval = ReadStorage(CalibHdlr, Index, Offset, Count, Buffer, Size);
                    } else {
                        AmbaPrint_PrintUInt5("[calibdata] (%u) Buffer is NULL", __LINE__, 0U, 0U, 0U, 0U);
                        Rval = ERR_ARG;
                    }
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) it is not on read mode", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used Id = %u", __LINE__, Hdlr, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 GetDesc_ParamCheck(UINT32 Hdlr, const AMBA_CALIB_DATA_GROUP_DESC_s *Desc)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Desc == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Get the descriptor of specified group
* @param [in]  Hdlr Handler ID
* @param [in]  Index Group ID
* @param [out] Desc Group descriptor
* @return ErrorCode
*/
UINT32 AmbaCalibData_GetDesc(UINT32 Hdlr, UINT32 Index, AMBA_CALIB_DATA_GROUP_DESC_s *Desc)
{
    UINT32 Rval = GetDesc_ParamCheck(Hdlr, Desc);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            const AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                if (CalibHdlr->WriteOnClose == 0U) {
                    Rval = ParseGroup(CalibHdlr, Index, Desc);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) it is not on read mode", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used Id = %u", __LINE__, Hdlr, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 AddUserData(AMBA_CALIB_DATA_HDLR_s *CalibHdlr, const UINT8 *Buffer, UINT32 Size)
{
    /*
     * [udta_info]  =>  [sizeof_udta_info][UDTA][data]
     */
    UINT8 Index = CalibHdlr->UdtaNum;
    UINT32 Rval = OK;
    if (Index < G_MaxUdta) {
       UINT8 *Temp = CalibHdlr->UdtaPtr[Index];
        if (Size <= G_UdtaSize[Index]) {
            AmbaPrint_PrintUInt5("WriteUserData(%u) : Size %u", Index, Size, 0U, 0U, 0U);
            (void) AmbaWrap_memcpy(Temp, Buffer, Size);
            CalibHdlr->UdtaSize[Index] = Size;
            CalibHdlr->UdtaNum++;
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Size %u cannot be larger than %u", __LINE__, Size, G_UdtaSize[Index], 0U, 0U);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Udta max num is %u, Index %u", __LINE__, G_MaxUdta, Index, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 AddUdta_ParamCheck(UINT32 Hdlr, const UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Buffer == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Size == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Size", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Write user data
* @param [in]  Hdlr Handler ID
* @param [in]  Buffer Data buffer
* @param [in]  Size Data size
* @return ErrorCode
*/
UINT32 AmbaCalibData_AddUdta(UINT32 Hdlr, const UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = AddUdta_ParamCheck(Hdlr, Buffer, Size);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                if (CalibHdlr->WriteOnClose != 0U) {
                    Rval = AddUserData(CalibHdlr, Buffer, Size);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) it is not on write mode", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 GetUserData(const AMBA_CALIB_DATA_HDLR_s *CalibHdlr, UINT8 Index, UINT8 *Buffer, UINT32 Size)
{
    /*
     * [udta_info]  =>  [sizeof_udta_info][UDTA][data]
     */
    UINT32 Rval = OK;
    if (Index < CalibHdlr->UdtaNum) {
        UINT8 *Temp = CalibHdlr->UdtaPtr[Index];
        UINT32 AtomSize;
        UINT32 Tag;
        Temp = ReadBeU32(Temp, &AtomSize);
        Temp = ReadBeU32(Temp, &Tag);
        if ((AtomSize > UDTA_INFO_SIZE) && (Tag == UDTA)) {
            if ((AtomSize - UDTA_INFO_SIZE) <= Size) {
                AmbaPrint_PrintUInt5("LoadUserData(%u) : size %u", Index, (AtomSize - UDTA_INFO_SIZE), 0U, 0U, 0U);
                (void) AmbaWrap_memcpy(Buffer, Temp, (AtomSize - UDTA_INFO_SIZE));
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) Buffer size is smaller than Data size %u", __LINE__, AtomSize - UDTA_INFO_SIZE, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid udta info!", __LINE__, 0U, 0U, 0U, 0U);
            Rval = ERR_ARG;
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Index exceed total num = %u", __LINE__, CalibHdlr->UdtaNum, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 GetUdta_ParamCheck(UINT32 Hdlr, UINT8 Index, const UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = ERR_ARG;
    if (Hdlr >= G_HdlrNum) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid handler", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Index >= AMBA_CALIB_DATA_MAX_USER_DATA) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Index", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Buffer == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (Size == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Size", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Read specified user data
* @param [in]  Hdlr Handler ID
* @param [in]  Index ID of user data
* @param [in]  Buffer Output buffer for user data
* @param [in]  Size Output buffer size
* @return ErrorCode
*/
UINT32 AmbaCalibData_GetUdta(UINT32 Hdlr, UINT8 Index, UINT8 *Buffer, UINT32 Size)
{
    UINT32 Rval = GetUdta_ParamCheck(Hdlr, Index, Buffer, Size);
    if (Rval == OK) {
        Rval = AmbaKAL_MutexTake(&G_Mutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            const AMBA_CALIB_DATA_HDLR_s * const CalibHdlr = &G_Hdlr[Hdlr];
            if (CalibHdlr->Used != 0U) {
                if (CalibHdlr->WriteOnClose == 0U) {
                    Rval = GetUserData(CalibHdlr, Index, Buffer, Size);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) it is not on read mode", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) the handler is not used", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_Mutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Rval;
}

static UINT32 DataCmprRLE(const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DestAddr, UINT32 DestSize, UINT32 *OutputSize)
{
    UINT8  Value = 0U, PreValue = 0U, Cnt = 1U;
    UINT32 Rval = OK;
    UINT32 DestBufIdx = 0U;
    UINT32 PredictSize = 0U;
    for (UINT32 SrcBufIdx = 0U; SrcBufIdx < SrcSize; SrcBufIdx++) {
        if (SrcBufIdx == 0U) {
            Value = SrcAddr[SrcBufIdx];
            PreValue = Value;
        } else {
            Value = SrcAddr[SrcBufIdx];
            if (Value == PreValue) {
                Cnt++;
                if (Cnt == 0xFFU) {
                    if ((DestBufIdx + 1U) < DestSize) {
                        DestAddr[DestBufIdx] = 0xFE;
                        DestAddr[DestBufIdx + 1U] = PreValue;
                        DestBufIdx += 2U;
                    }
                    Cnt = 1U;
                    PredictSize += 2U;
                }
            } else {
                if ((DestBufIdx + 1U) < DestSize) {
                    DestAddr[DestBufIdx] = Cnt;
                    DestAddr[DestBufIdx + 1U] = PreValue;
                    DestBufIdx += 2U;
                }
                Cnt = 1U;
                PredictSize += 2U;
            }
            if (PredictSize > DestSize) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) PredictSize (%u) exceeds destinaiton buffer size(%u) ", __LINE__, PredictSize, DestSize, 0U, 0U);
                Rval = ERR_ARG;
                break;
            }
            PreValue = Value;
        }
    }
    if (Rval == OK) {
        if ((DestBufIdx + 1U) < DestSize) {
            DestAddr[DestBufIdx] = Cnt;
            DestAddr[DestBufIdx + 1U] = PreValue;
        }
        PredictSize += 2U;
        if (PredictSize > DestSize) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) PredictSize (%u) exceeds destinaiton buffer size(%u) ", __LINE__, PredictSize, DestSize, 0U, 0U);
            *OutputSize = 0U;
            Rval = ERR_ARG;
        } else {
            *OutputSize = PredictSize;
            AmbaPrint_PrintUInt5("[calibdata] (%u) RLE success, RLE compress %u bytes to %u ", __LINE__, SrcSize, PredictSize, 0U, 0U);
        }
    } else {
        *OutputSize = 0U;
    }
    return Rval;
}

static UINT32 DataDecmprRLE(const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DestAddr, UINT32 DestSize, UINT32 *OutputSize)
{
    UINT32 Rval = OK;
    UINT32 DestBufIdx = 0U;
    for (UINT32 SrcBufIdx = 0U; SrcBufIdx < SrcSize; SrcBufIdx += 2U) {
        if (DestBufIdx >= DestSize){
            AmbaPrint_PrintUInt5("[calibdata] (%u) RLE decompressed size exceeds destinaiton buffer size (%u)", __LINE__, DestSize, 0U, 0U, 0U);
            Rval = ERR_ARG;
        } else if ((SrcBufIdx + 1U) >= SrcSize) {
            AmbaPrint_PrintUInt5("[calibdata] (%u) RLE compressed size %u is not multiple of 2", __LINE__, SrcSize, 0U, 0U, 0U);
            Rval = ERR_ARG;
        } else {
            (void) AmbaWrap_memset(&DestAddr[DestBufIdx], (INT32)SrcAddr[SrcBufIdx + 1U], SrcAddr[SrcBufIdx]);
            DestBufIdx += SrcAddr[SrcBufIdx];
        }
        if (Rval != OK) {
            break;
        }
    }
    if (Rval == OK) {
        *OutputSize = DestBufIdx;
        AmbaPrint_PrintUInt5("[calibdata] (%u) RLE success, RLE decompress %u bytes to %u", __LINE__, SrcSize, *OutputSize, 0U, 0U);
    } else {
        *OutputSize = 0U;
        AmbaPrint_PrintUInt5("[calibdata] (%u) RLE failed", __LINE__, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 DataCmprLZ4(const AMBA_CALIB_DATA_CMPR_INFO_s *CmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DestAddr, UINT32 DestSize, UINT32 *OutputSize)
{
    UINT32 Rval = OK;
    if (G_LZ4WorkBuf != NULL) {
        INT32 DataSize[1] = {0};
        INT32 DstOutputSize[1] = {0};
        const UINT8 *SrcTable[1] = {NULL};
        const INT16 *CmprWorkBufPtr;
        Rval = AmbaKAL_MutexTake(&G_CmprMutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT32 Ret;
            DataSize[0] = (INT32)SrcSize;
            SrcTable[0] = SrcAddr;
            AmbaMisra_TypeCast32(&CmprWorkBufPtr, &G_LZ4WorkBuf);
            Rval = AmbaLZ4_TblEncodeSegment(1, SrcTable, DataSize, (INT8*)DestAddr, (INT32)DestSize, DstOutputSize, (INT32)CmprInfo->Lz4Info.SegmentSize, CmprWorkBufPtr);
            if (Rval == OK) {
                if((DstOutputSize[0] != 0) && (DstOutputSize[0] <= (INT32)DestSize)) {
                    *OutputSize = (UINT32)DstOutputSize[0];
                    AmbaPrint_PrintUInt5("[calibdata] (%u) LZ4 success, LZ4 compress %u bytes to %u", __LINE__, SrcSize, *OutputSize, 0U, 0U);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) LZ4 compress failed, get encoded size: %u, destination buffer size: %u", __LINE__, (UINT32)DstOutputSize[0], DestSize, 0U, 0U);
                    *OutputSize = 0U;
                    Rval = ERR_ARG;
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) DataCmprLZ4 failed", __LINE__, 0U, 0U, 0U, 0U);
                *OutputSize = 0U;
            }
            Ret = AmbaKAL_MutexGive(&G_CmprMutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) G_LZ4WorkBuf is NULL", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 DataDecmprLZ4(const AMBA_CALIB_DATA_DECMPR_INFO_s *DecmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DestAddr, UINT32 DestSize, UINT32 *OutputSize)
{
    UINT32 Rval = OK;
    if (G_LZ4WorkBuf != NULL) {
        Rval = AmbaKAL_MutexTake(&G_CmprMutex, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            UINT8 *DstTable[AMBA_CALIB_DATA_MAX_DST_NUM] = {NULL};
            UINT32 Ret;
            DstTable[0] = DestAddr;
            if (DecmprInfo->Lz4Info.DstNum < AMBA_CALIB_DATA_MAX_DST_NUM) {
                if (DecmprInfo->Lz4Info.DecmprType == AMBA_CALIB_DATA_DECMPR_LZ4_LINEAR) {
                    Rval = AmbaLZ4_TblDecodeSegment((INT32)DecmprInfo->Lz4Info.DstNum, SrcAddr, DstTable, (INT32)DecmprInfo->Lz4Info.SegmentSize, G_LZ4WorkBuf);
                } else if (DecmprInfo->Lz4Info.DecmprType == AMBA_CALIB_DATA_DECMPR_LZ4_WINDOW) {
                    Rval = AmbaLZ4_TblDecodeSegmentToWin((INT32)DecmprInfo->Lz4Info.DstNum, SrcAddr, DstTable, DecmprInfo->Lz4Info.DstTableSize,
                                                            DecmprInfo->Lz4Info.DstWinInfo, (INT32)DecmprInfo->Lz4Info.SegmentSize, G_LZ4WorkBuf);
                } else {
                    AmbaPrint_PrintUInt5("[calibdata] (%u) wrong parameter", __LINE__, 0U, 0U, 0U, 0U);
                    Rval = ERR_ARG;
                }
                if (Rval == OK) {
                    *OutputSize = DestSize;
                    AmbaPrint_PrintUInt5("[calibdata] (%u) LZ4 success, LZ4 decompress %u bytes to %u", __LINE__, SrcSize, *OutputSize, 0U, 0U);
                } else {
                    *OutputSize = 0U;
                    AmbaPrint_PrintUInt5("[calibdata] (%u) LZ4 decompress failed, DecmprType : %u", __LINE__, DecmprInfo->Lz4Info.DecmprType, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("[calibdata] (%u) DstNum %u is over maximum %u", __LINE__, DecmprInfo->Lz4Info.DstNum, AMBA_CALIB_DATA_MAX_DST_NUM, 0U, 0U);
                Rval = ERR_ARG;
            }
            Ret = AmbaKAL_MutexGive(&G_CmprMutex);
            if (Ret != OK) {
                AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexGive failed", __LINE__, 0U, 0U, 0U, 0U);
                Rval = Ret;
            }
        } else {
            AmbaPrint_PrintUInt5("[calibdata] (%u) AmbaKAL_MutexTake failed", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("[calibdata] (%u) G_LZ4WorkBuf is NULL", __LINE__, 0U, 0U, 0U, 0U);
        Rval = ERR_ARG;
    }
    return Rval;
}

static UINT32 Compress_ParamCheck(UINT8 Algo, const UINT8 *SrcAddr, UINT32 SrcSize,const UINT8 *DstAddr, UINT32 DstSize, const UINT32 *OutputSize)
{
    UINT32 Rval = ERR_ARG;
    if (Algo >= AMBA_CALIB_DATA_CMPR_ALGO_NUM) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Algo", __LINE__, 0U, 0U, 0U, 0U);
    } else if (SrcAddr == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (SrcSize == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid SrcSize", __LINE__, 0U, 0U, 0U, 0U);
    } else if (DstAddr == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (DstSize == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid SrcSize", __LINE__, 0U, 0U, 0U, 0U);
    } else if (OutputSize == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Compress data with specified compression type
* @param [in]  Algo Compression type, see AMBA_CALIB_DATA_CMPR_ALGO_NUM
* @param [in]  CmprInfo Information for compressed algorithm
* @param [in]  SrcAddr Input buffer for uncompressed data
* @param [in]  SrcSize Uncompressed data size
* @param [in]  DstAddr Output buffer for compressed data
* @param [in]  DstSize Output buffer size
* @param [out] OutputSize Compressed data size
* @return ErrorCode
*/
UINT32 AmbaCalibData_Compress(UINT8 Algo, const AMBA_CALIB_DATA_CMPR_INFO_s *CmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DstAddr, UINT32 DstSize, UINT32 *OutputSize)
{
    UINT32 Rval = Compress_ParamCheck(Algo, SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
    if (Rval == OK) {
        switch (Algo) {
            case AMBA_CALIB_DATA_CMPR_ALGO_LZ4:
                Rval = DataCmprLZ4(CmprInfo, SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
                break;
            case AMBA_CALIB_DATA_CMPR_ALGO_RLE:
                Rval = DataCmprRLE(SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
                break;
            case AMBA_CALIB_DATA_CMPR_ALGO_NONE:
                AmbaPrint_PrintUInt5("AMBA_CALIB_DATA_CMPR_ALGO_NONE %u", __LINE__, 0U, 0U, 0U, 0U);
                break;
            default:
                AmbaPrint_PrintUInt5("[calibdata] (%u) wrong parameter", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
                break;
        }
    }
    return Rval;
}

static UINT32 Decompress_ParamCheck(UINT8 Algo, const UINT8 *SrcAddr, UINT32 SrcSize,const UINT8 *DstAddr, UINT32 DstSize, const UINT32 *OutputSize)
{
    UINT32 Rval = ERR_ARG;
    if (Algo >= AMBA_CALIB_DATA_CMPR_ALGO_NUM) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid Algo", __LINE__, 0U, 0U, 0U, 0U);
    } else if (SrcAddr == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (SrcSize == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid SrcSize", __LINE__, 0U, 0U, 0U, 0U);
    } else if (DstAddr == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else if (DstSize == 0U) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid SrcSize", __LINE__, 0U, 0U, 0U, 0U);
    } else if (OutputSize == NULL) {
        AmbaPrint_PrintUInt5("[calibdata] (%u) Invalid argument (null pointer)", __LINE__, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
* Decompress data with specified decompression type
* @param [in]  Algo Decompression type, see AMBA_CALIB_DATA_CMPR_ALGO_NUM
* @param [in]  DecmprInfo Information for decompressed algorithm
* @param [in]  SrcAddr Input buffer for compressed data
* @param [in]  SrcSize Compressed data size
* @param [in]  DstAddr Output buffer for uncompressed data
* @param [in]  DstSize Output buffer size
* @param [out] OutputSize Uncompressed data size
* @return ErrorCode
*/
UINT32 AmbaCalibData_Decompress(UINT8 Algo, const AMBA_CALIB_DATA_DECMPR_INFO_s *DecmprInfo, const UINT8 *SrcAddr, UINT32 SrcSize, UINT8 *DstAddr, UINT32 DstSize, UINT32 *OutputSize)
{
    UINT32 Rval = Decompress_ParamCheck(Algo, SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
    if (Rval == OK) {
        switch (Algo) {
            case AMBA_CALIB_DATA_CMPR_ALGO_LZ4:
                Rval = DataDecmprLZ4(DecmprInfo, SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
                break;
            case AMBA_CALIB_DATA_CMPR_ALGO_RLE:
                Rval = DataDecmprRLE(SrcAddr, SrcSize, DstAddr, DstSize, OutputSize);
                break;
            case AMBA_CALIB_DATA_CMPR_ALGO_NONE:
                AmbaPrint_PrintUInt5("AMBA_CALIB_DATA_CMPR_ALGO_NONE %u", __LINE__, 0U, 0U, 0U, 0U);
                break;
            default:
                AmbaPrint_PrintUInt5("[calibdata] (%u) wrong parameter", __LINE__, 0U, 0U, 0U, 0U);
                Rval = ERR_ARG;
                break;
        }
    }
    return Rval;
}
