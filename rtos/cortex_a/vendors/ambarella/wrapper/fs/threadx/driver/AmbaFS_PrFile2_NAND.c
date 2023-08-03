/**
 *  @file AmbaFS_PrFile2_NAND.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details NAND software driver for PrFILE2/exFAT File System
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaNAND.h"
#include "AmbaNAND_Ctrl.h"

#include "AmbaFS.h"
#include "AmbaFS_Format.h"

#include <prfile2/pdm_driver.h>
#include <prfile2/pdm_api.h>

#define NAND_FAT_PAGES      (32U)
#define NAND_DATA_PAGES     (32U)
#define NAND_FAT_BUF_SIZE   (4U)
#define NAND_DATA_BUF_SIZE  (8U)

#define NAND_FS_BUFFER_SIZE (NAND_FAT_PAGES + NAND_DATA_PAGES)

/* Definitions for filesystem storage1/storage2 in NAND flash. */
#define NAND_FS_ID_STG0 (0U)
#define NAND_FS_ID_STG1 (1U)
#define NAND_FS_ID_MAX  (2U)

/*-----------------------------------------------------------------------------------------------*\
 * Runtime variables required by PrFILE2.
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    PDM_FUNCTBL             DrvFuncTbl;
    PDM_INIT_DISK           DrvInitTbl;
    PDM_DISK                *pDiskTbl;
    PDM_PARTITION           *pPartTbl;
    PF_DRV_TBL              *pDrvTblEntry;
    PF_CACHE_SETTING        CacheSet;
    UINT32                  NftlID;                 /* ID for nftl instance */
} AMBA_FS_NAND_PRFILE2_OBJ_s;

static AMBA_FS_NAND_PRFILE2_OBJ_s _AmbaFSObj[NAND_FS_ID_MAX] GNU_SECTION_NOZEROINIT;

static UINT32 NandFS_Init(UINT32 ID);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_NandInit
 *
 *  @Description:: Prfile2 NAND device driver initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_NandInit(PF_DRV_TBL * pDrive)
{
    UINT32 i;

    for (i = 0U; i < NAND_FS_ID_MAX; i ++) {
        AMBA_FS_NAND_PRFILE2_OBJ_s *pNandPrFileObj = &_AmbaFSObj[i];

        (void)AmbaWrap_memset(pNandPrFileObj, 0x0, sizeof(AMBA_FS_NAND_PRFILE2_OBJ_s));
        pNandPrFileObj->pDrvTblEntry = &(pDrive[i]);
        (void) NandFS_Init(i);
    }

    return i;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Nand0FS_PhySimplyWrite
 *
 *  @Description::
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static INT32 Nand0FS_PhySimplyWrite(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 RetVal = 0;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    SecConfig.pDataBuf    = pBuf;
    if (OK != AmbaNAND_WriteSector(AMBA_USER_PARTITION_FAT_DRIVE_A, &SecConfig, 2000U)) {
        RetVal = -1;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Nand1FS_PhySimplyWrite
 *
 *  @Description::
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static INT32 Nand1FS_PhySimplyWrite(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 RetVal = 0;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    SecConfig.pDataBuf    = pBuf;
    if (OK != AmbaNAND_WriteSector(AMBA_USER_PARTITION_FAT_DRIVE_B, &SecConfig, 2000U)) {
        RetVal = -1;
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_PrFile2NandMount
 *
 *  @Description:: Mount media partition
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static UINT32 NandFS_PrFile2NandMount(UINT32 ID)
{
    UINT32 Rval = AMBA_FS_ERR_API;
    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];
    AMBA_NVM_FTL_INFO_s FtlInfo;

    if ((ID > NAND_FS_ID_MAX)) {
        Rval = AMBA_FS_ERR_ARG;
    } else {
        (void) AmbaNVM_GetFtlInfo(AMBA_NVM_NAND, pNandFSObj->NftlID, &FtlInfo);
        if (FtlInfo.Initialized == 1U) {
            char Drive = 'a';
            (void)AmbaWrap_memcpy(&Drive, &pNandFSObj->pDrvTblEntry->drive, sizeof(Drive));
            pdm_disk_notify_media_insert(pNandFSObj->pDiskTbl);

            if (PF_RET_NO_ERR == pf2_mount(Drive)) {
                Rval = AMBA_FS_ERR_NONE;
            }
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_PrFile2NandUnmount
 *
 *  @Description:: Unmount media partition
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static UINT32 NandFS_PrFile2NandUnmount(UINT32 ID)
{
    UINT32 Rval = AMBA_FS_ERR_API;
    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];

    if ((ID > NAND_FS_ID_MAX)) {
        Rval = AMBA_FS_ERR_ARG;
    } else {
        pdm_disk_notify_media_eject(pNandFSObj->pDiskTbl);

        if ((pNandFSObj->pDrvTblEntry != NULL) &&
            ((pNandFSObj->pDrvTblEntry->stat & PF_ATTACHED) == 1U)) {
            char Drive = 'a';
            (void)AmbaWrap_memcpy(&Drive, &pNandFSObj->pDrvTblEntry->drive, sizeof(Drive));
            if (PF_RET_NO_ERR == pf2_unmount(Drive, PF_UNMOUNT_FORCE)) {
                Rval = AMBA_FS_ERR_NONE;
            }
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_DrvInit
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static PDM_ERROR NandFS_DrvInit(PDM_DISK *pDisk)
{
    /* If the media has been inserted, notifies PrFILE2 */
    pdm_disk_notify_media_insert(pDisk);

    return DRV_SUCCEED;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_DrvTblInit
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static PF_ERROR NandFS_DrvTblInit(UINT32 ID, PDM_DISK_TBL *pDiskTbl, PDM_U_LONG Ext)
{
    pDiskTbl->p_func = &_AmbaFSObj[ID].DrvFuncTbl;
    pDiskTbl->ui_ext = Ext;

    return DRV_SUCCEED;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_Finalize
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static PDM_ERROR NandFS_Finalize(UINT32 ID, PDM_DISK *pDisk)
{
    INT32 Rval = DRV_SUCCEED;
    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];

    if ((pDisk == NULL) || (ID > NAND_FS_ID_MAX)) {
        Rval = DRV_FAILED;
    } else {
        /* Termination process for the driver and media controller */
        pdm_disk_notify_media_eject(pDisk);

        if (pNandFSObj->pDrvTblEntry != NULL) {
            pNandFSObj->pDrvTblEntry->drive = 0;
            pNandFSObj->pDrvTblEntry->stat  = 0;
        }
    }

    return Rval;
}

static INT32 NandFS_FormatStart(UINT32 ID, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    static UINT8 g_NandFormatBuf[64 * 512] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    UINT8 *pBuf = g_NandFormatBuf;

    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;
    INT32 Rval = DRV_SUCCEED;
    UINT32 Sector;
    UINT32 NumSysSecs;
    UINT32 NumSecs = 64U;

    INT32 (*WriteSecFunc[NAND_FS_ID_MAX])(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    WriteSecFunc[0] = Nand0FS_PhySimplyWrite;
    WriteSecFunc[1] = Nand1FS_PhySimplyWrite;

    /* Calculate number of system sectors */
    AmbaFS_CreateBs(pDrvInfo, pBuf);
    if (pDrvInfo->FatType == FAT32_TYPE) {
        const AMBA_FS_FAT_BS32 *pBs32;
        AmbaMisra_TypeCast32(&pBs32, &pBuf);  /* pBs32 = (AMBA_FS_FAT_BS32*) pBuf; */
        NumSysSecs = pDrvInfo->StartLba + pBs32->Fno + (pBs32->Spf32 * pBs32->Fts) + pDrvInfo->Spc;
    } else {
        const AMBA_FS_FAT_BS16 *pBs16;
        AmbaMisra_TypeCast32(&pBs16, &pBuf); /* pBs16 = (AMBA_FS_FAT_BS16*) pBuf; */
        NumSysSecs = pDrvInfo->StartLba + pBs16->Fno + ((UINT32)pBs16->Spf * pBs16->Fts) + ((UINT32)pBs16->Ent >> 4U);
    }

    /* Write all system sectors as 0 */
    (void)AmbaWrap_memset(pBuf, 0x0, NumSecs << SEC_SHIFT);
    for (Sector = 0; Sector < NumSysSecs; Sector += NumSecs) {
        if ((NumSysSecs - Sector) < NumSecs) {
            NumSecs = NumSysSecs - Sector;
        }

        SecConfig.StartSector = Sector;
        SecConfig.NumSector   = NumSecs;
        SecConfig.pDataBuf    = pBuf;
        if (OK != AmbaNAND_WriteSector(pNandFSObj->NftlID, &SecConfig, 2000U)) {
            Rval = DRV_FAILED;
            break;
        } else {
            Rval = DRV_SUCCEED;
        }
    }

    if (Rval == DRV_SUCCEED) {
        /* Write mbr */
        AmbaFS_CreateMbr(pDrvInfo, pBuf);
        SecConfig.StartSector = 0U;
        SecConfig.NumSector   = 1U;
        SecConfig.pDataBuf    = pBuf;
        if (OK != AmbaNAND_WriteSector((UINT32)pNandFSObj->NftlID, &SecConfig, 2000U)) {
            Rval = DRV_FAILED;
        }
    }

    if (Rval == DRV_SUCCEED) {
        /* Write boot sector area for FAT16 or FAT32 */
        Rval = AmbaFS_CreateBootArea(pDrvInfo, WriteSecFunc[ID]);
        if (Rval < 0) {
            Rval = DRV_FAILED;
        }
    }

    if (Rval == DRV_SUCCEED) {
        /* Write FAT table */
        AmbaFS_CreateBs(pDrvInfo, pBuf);
        Rval = AmbaFS_CreateFat(pDrvInfo, pBuf, WriteSecFunc[ID]);
        if (Rval < 0) {
            Rval = DRV_FAILED;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_Format
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static PDM_ERROR NandFS_Format(UINT32 ID, const PDM_U_CHAR *pParam)
{
    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];
    AMBA_FS_DRIVE_INFO DrvInfo;
    UINT32 TotalSizeMB, TotalClust, NumClusts, NumSecs, TotalSecs;
    INT32 Rval;
    AMBA_NVM_FTL_INFO_s FtlInfo;

    AmbaMisra_TouchUnused(&pParam);

    (void)AmbaNVM_GetFtlInfo(AMBA_NVM_NAND, pNandFSObj->NftlID, &FtlInfo);
    TotalSecs = FtlInfo.TotalSizeInSectors;

    (void)AmbaWrap_memset(&DrvInfo, 0, sizeof(DrvInfo));

    /**************************/
    /* Setup drive infomation */
    /**************************/

    TotalSizeMB = TotalSecs >> 11U;

    Rval = AmbaFS_ParseFormatParam(TotalSizeMB, &DrvInfo);
    if (Rval < 0) {
        Rval = DRV_FAILED;
    }

    if (Rval == DRV_SUCCEED) {
        UINT32 Tmp;

        TotalClust = TotalSecs / DrvInfo.Spc;
        DrvInfo.Spf = AmbaFS_GetFlashSectorsPerFat(DrvInfo.FatType, TotalClust, DrvInfo.Spc);

        if (DrvInfo.FatType == FAT32_TYPE) {
            NumSecs = (DrvInfo.Spf * NFAT) + FAT32_BS_SIZE;
        } else {
            NumSecs = (DrvInfo.Spf * NFAT) + ((ROOT_DIR_ENTRY * (UINT32)DIR_ENTRY_SIZE) >> SEC_SHIFT) +
                      FAT16_BS_SIZE;
        }

        for (NumClusts = 0U; (NumClusts * DrvInfo.Spc) < NumSecs;) {
            NumClusts += 1U;
        }

        NumClusts += RSV_MBR_CLUST;

        if (DrvInfo.StartLbaCustom != 0U) {
            DrvInfo.StartLba = DrvInfo.StartLbaCustom;
        } else {
            DrvInfo.StartLba = (NumClusts * DrvInfo.Spc) - NumSecs;
        }

        DrvInfo.EndLba      = TotalSecs - 1U;
        DrvInfo.NumOfHead   = NAND_NHEAD;
        DrvInfo.Spt         = NAND_SPT;

        Tmp = (DrvInfo.EndLba / ((UINT32)DrvInfo.NumOfHead * (UINT32)DrvInfo.Spt)) + 1U;
        (void)AmbaWrap_memcpy(&DrvInfo.NumCyls, &Tmp, sizeof(DrvInfo.NumCyls));

        DrvInfo.TotalSector = (DrvInfo.EndLba - DrvInfo.StartLba) + 1U;
    }
    /*******************/
    /* Start to format */
    /*******************/
    if (Rval == DRV_SUCCEED) {
        Rval = NandFS_FormatStart(ID, &DrvInfo);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_GetDiskInfo
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PDM_ERROR:
 *-----------------------------------------------------------------------------------------------*/
static PDM_ERROR NandFS_GetDiskInfo(UINT32 ID, PDM_DISK_INFO *pDiskInfo)
{
    INT32 Rval = DRV_SUCCEED;
    UINT32 SecCnt, RetVal;
    const AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];
    AMBA_NVM_FTL_INFO_s FtlInfo;

    if ((ID > NAND_FS_ID_MAX) || (pDiskInfo == NULL)) {
        Rval = DRV_FAILED;
    } else {

        RetVal = AmbaNVM_GetFtlInfo(AMBA_NVM_NAND, pNandFSObj->NftlID, &FtlInfo);

        if ((RetVal != OK) || (FtlInfo.Initialized != 1U)) {
            Rval = DRV_FAILED;
        } else {
            UINT32 cylinders = ((pDiskInfo->total_sectors / NAND_NHEAD) / NAND_SPT);
            SecCnt = FtlInfo.TotalSizeInSectors;

            pDiskInfo->total_sectors     = SecCnt;
            pDiskInfo->cylinders = (UINT16)cylinders & 0xffffU;
            pDiskInfo->heads             = NAND_NHEAD;
            pDiskInfo->sectors_per_track = NAND_SPT;
            pDiskInfo->bytes_per_sector  = SEC_SIZE;

            if (FtlInfo.WriteProtect == 1U) {
                pDiskInfo->media_attr = (PRF2_MEDIA_ATTR | PDM_MEDIA_ATTR_WRITE_PROTECT);
            } else {
                pDiskInfo->media_attr = PRF2_MEDIA_ATTR;
            }

            pDiskInfo->format_param = NULL;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 * Wrapper for NAND flash first storage.
\*-----------------------------------------------------------------------------------------------*/

static PDM_ERROR Nand0FS_DrvInit(PDM_DISK *pDisk)
{
    return NandFS_DrvInit(pDisk);
}

static PDM_ERROR Nand0FS_DrvTblInit(PDM_DISK_TBL *pDiskTbl, PDM_U_LONG Ext)
{
    return NandFS_DrvTblInit(NAND_FS_ID_STG0, pDiskTbl, Ext);
}

static PDM_ERROR Nand0FS_Finalize(PDM_DISK *pDisk)
{
    (void)pDisk;

    return NandFS_Finalize(NAND_FS_ID_STG0, pDisk);
}

static PDM_ERROR Nand0FS_Mount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return DRV_SUCCEED;
}

static PDM_ERROR Nand0FS_Unmount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return DRV_SUCCEED;
}

static PDM_ERROR Nand0FS_Format(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    (void)pDisk;

    return NandFS_Format(NAND_FS_ID_STG0, pParam);
}

static PDM_ERROR Nand0FS_PhyRead(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                 PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void)pDisk;

    (*pNumSuccess) = 0U;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    SecConfig.pDataBuf    = (UINT8 *)pBuf;

    if (OK == AmbaNAND_ReadSector(AMBA_USER_PARTITION_FAT_DRIVE_A, &SecConfig, 2000U)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR Nand0FS_PhyWrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                  PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void)pDisk;

    (*pNumSuccess) = 0U;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    AmbaMisra_TypeCast32(&SecConfig.pDataBuf, &pBuf);

    if (OK == AmbaNAND_WriteSector(AMBA_USER_PARTITION_FAT_DRIVE_A, &SecConfig, 2000U)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR Nand0FS_GetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    (void)pDisk;

    return NandFS_GetDiskInfo(NAND_FS_ID_STG0, pDiskInfo);
}

void AmbaFS_PrFile2Nand0Mount(void)
{
    (void)NandFS_PrFile2NandMount(NAND_FS_ID_STG0);
}

void AmbaFS_PrFile2Nand0Unmount(void)
{
    (void)NandFS_PrFile2NandUnmount(NAND_FS_ID_STG0);
}

/*-----------------------------------------------------------------------------------------------*\
 * Wrapper for NAND flash second storage.
\*-----------------------------------------------------------------------------------------------*/

static PDM_ERROR Nand1FS_DrvInit(PDM_DISK *pDisk)
{
    return NandFS_DrvInit(pDisk);
}

static PDM_ERROR Nand1FS_DrvTblInit(PDM_DISK_TBL *pDiskTbl, PDM_U_LONG Ext)
{
    return NandFS_DrvTblInit(NAND_FS_ID_STG1, pDiskTbl, Ext);
}

static PDM_ERROR Nand1FS_Finalize(PDM_DISK *pDisk)
{
    (void)pDisk;

    return NandFS_Finalize(NAND_FS_ID_STG1, pDisk);
}

static PDM_ERROR Nand1FS_Mount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return DRV_SUCCEED;
}

static PDM_ERROR Nand1FS_Unmount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return DRV_SUCCEED;
}

static PDM_ERROR Nand1FS_Format(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    (void)pDisk;

    return NandFS_Format(NAND_FS_ID_STG1, pParam);
}

static PDM_ERROR Nand1FS_PhyRead(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                 PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    INT32 Rval;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void)pDisk;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    SecConfig.pDataBuf    = pBuf;

    if (OK == AmbaNAND_ReadSector(AMBA_USER_PARTITION_FAT_DRIVE_B, &SecConfig, 2000)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        Rval = DRV_FAILED;
    }
    return Rval;
}

static PDM_ERROR Nand1FS_PhyWrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                  PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    INT32 Rval;
    AMBA_NVM_SECTOR_CONFIG_s SecConfig;

    (void)pDisk;

    SecConfig.StartSector = Sector;
    SecConfig.NumSector   = Sectors;
    AmbaMisra_TypeCast32(&SecConfig.pDataBuf, &pBuf);

    if (OK == AmbaNAND_WriteSector(AMBA_USER_PARTITION_FAT_DRIVE_B, &SecConfig, 2000U)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        Rval = DRV_FAILED;
    }

    return Rval;
}

static PDM_ERROR Nand1FS_GetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    (void)pDisk;

    return NandFS_GetDiskInfo(NAND_FS_ID_STG1, pDiskInfo);
}

void AmbaFS_PrFile2Nand1Mount(void)
{
    (void)NandFS_PrFile2NandMount(NAND_FS_ID_STG1);
}

void AmbaFS_PrFile2Nand1Unmount(void)
{
    (void)NandFS_PrFile2NandUnmount(NAND_FS_ID_STG1);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: NandFS_Init
 *
 *  @Description:: Initialize the NAND flash block device driver.
 *
 *  @Input      ::
 *          INT32 ID  : NAND flash storage partition ID.
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          PF_DRV_TBL *: Initialized and filled driver table
 *-----------------------------------------------------------------------------------------------*/
static UINT32 NandFS_Init(UINT32 ID)
{
    static PF_CACHE_PAGE g_NandCachePage[NAND_FS_BUFFER_SIZE * NAND_FS_ID_MAX]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static PF_SECTOR_BUF g_NandCacheBuffer0[NAND_FS_BUFFER_SIZE]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static PF_SECTOR_BUF g_NandCacheBuffer1[NAND_FS_BUFFER_SIZE]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    AMBA_FS_NAND_PRFILE2_OBJ_s *pNandFSObj = &_AmbaFSObj[ID];
    UINT32 Rval = AMBA_FS_ERR_NONE;

    if (ID >= NAND_FS_ID_MAX) {
        Rval = AMBA_FS_ERR_ARG;
    } else {
        if (ID == NAND_FS_ID_STG0) {
            char Drive = 'a';
            (void)AmbaWrap_memcpy(&pNandFSObj->pDrvTblEntry->drive, &Drive, sizeof(pNandFSObj->pDrvTblEntry->drive));

            pNandFSObj->DrvFuncTbl.init             = Nand0FS_DrvInit;
            pNandFSObj->DrvFuncTbl.finalize         = Nand0FS_Finalize;
            pNandFSObj->DrvFuncTbl.mount            = Nand0FS_Mount;
            pNandFSObj->DrvFuncTbl.unmount          = Nand0FS_Unmount;
            pNandFSObj->DrvFuncTbl.format           = Nand0FS_Format;
            pNandFSObj->DrvFuncTbl.physical_read    = Nand0FS_PhyRead;
            pNandFSObj->DrvFuncTbl.physical_write   = Nand0FS_PhyWrite;
            pNandFSObj->DrvFuncTbl.get_disk_info    = Nand0FS_GetDiskInfo;
            pNandFSObj->DrvInitTbl.p_func           = Nand0FS_DrvTblInit;
            pNandFSObj->CacheSet.buffers            = g_NandCacheBuffer0;

            pNandFSObj->NftlID  = AMBA_USER_PARTITION_FAT_DRIVE_A;
        } else if (ID == NAND_FS_ID_STG1) {
            char Drive = 'b';
            (void)AmbaWrap_memcpy(&pNandFSObj->pDrvTblEntry->drive, &Drive, sizeof(pNandFSObj->pDrvTblEntry->drive));

            pNandFSObj->DrvFuncTbl.init             = Nand1FS_DrvInit;
            pNandFSObj->DrvFuncTbl.finalize         = Nand1FS_Finalize;
            pNandFSObj->DrvFuncTbl.mount            = Nand1FS_Mount;
            pNandFSObj->DrvFuncTbl.unmount          = Nand1FS_Unmount;
            pNandFSObj->DrvFuncTbl.format           = Nand1FS_Format;
            pNandFSObj->DrvFuncTbl.physical_read    = Nand1FS_PhyRead;
            pNandFSObj->DrvFuncTbl.physical_write   = Nand1FS_PhyWrite;
            pNandFSObj->DrvFuncTbl.get_disk_info    = Nand1FS_GetDiskInfo;
            pNandFSObj->DrvInitTbl.p_func           = Nand1FS_DrvTblInit;
            pNandFSObj->CacheSet.buffers            = g_NandCacheBuffer1;

            pNandFSObj->NftlID  = AMBA_USER_PARTITION_FAT_DRIVE_B;

        } else {
            /* for MisraC check */
        }

        pNandFSObj->DrvInitTbl.ui_ext = 0U;

        if (PDM_OK != pdm_open_disk(&pNandFSObj->DrvInitTbl, &pNandFSObj->pDiskTbl)) {
            Rval = AMBA_FS_ERR_API;
        }

        /* Open a partition */
        if (Rval == AMBA_FS_ERR_NONE) {
            if (PDM_OK != pdm_open_partition(pNandFSObj->pDiskTbl, 0, &pNandFSObj->pPartTbl)) {
                Rval = AMBA_FS_ERR_API;
            }
        }

        if (Rval == AMBA_FS_ERR_NONE) {
            /* Initialize driver table */
            AmbaMisra_TypeCast32(&pNandFSObj->pDrvTblEntry->p_part, &pNandFSObj->pPartTbl);
            pNandFSObj->CacheSet.pages             = &g_NandCachePage[ID * NAND_FS_BUFFER_SIZE];
            pNandFSObj->CacheSet.num_fat_pages     = NAND_FAT_PAGES;
            pNandFSObj->CacheSet.num_data_pages    = NAND_DATA_PAGES;
            pNandFSObj->CacheSet.num_fat_buf_size  = NAND_FAT_BUF_SIZE;
            pNandFSObj->CacheSet.num_data_buf_size = NAND_DATA_BUF_SIZE;
            pNandFSObj->pDrvTblEntry->cache        = &pNandFSObj->CacheSet;
        }
    }
    return Rval;
}
