/**
 *  @file AmbaFS_PrFile2_SD.c
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
 *  @details SD software driver for PrFILE2/exFAT File System
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

#include "AmbaFS.h"
#include "AmbaFS_Format.h"
#include "AmbaIOUtility.h"

#include <prfile2/pdm_driver.h>
#include <prfile2/pdm_api.h>

/* SDPRF_FAT_PAGES >  SDPRF_FAT_BUF_SIZE, SDPRF_FAT_BUF_SIZ > 1, SDPRF_FAT_BUF_SIZE < 65535*/
/* SDPRF_DATA_PAGES >  4 *  SDPRF_DATA_BUF_SIZE, SDPRF_DATA_BUF_SIZE > 1, SDPRF_DATA_BUF_SIZE < 32767*/
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SDPRF_FAT_PAGES     (4096U)
#define SDPRF_DATA_PAGES    (4096U)
#else
#define SDPRF_FAT_PAGES     (64U)
#define SDPRF_DATA_PAGES    (32U)
#endif
#define SDPRF_FAT_BUF_SIZE  (4U)
#define SDPRF_DATA_BUF_SIZE (8U)

#if (SDPRF_FAT_BUF_SIZE > SDPRF_FAT_PAGES) || (SDPRF_DATA_BUF_SIZE > SDPRF_DATA_PAGES)
#error wrong FAT buffer allocation in AmbaFS_SdPrFile2.c
#endif

/**
 * Runtime variables required by PrFILE as well as internal ones.
 */
typedef struct {
    PDM_FUNCTBL          DrvFunc;
    PDM_INIT_DISK        DrvInitTable;
    struct PDM_DISK      *pDiskTable;
    struct PDM_PARTITION *pPartTable;
    PF_DRV_TBL           *pDrvTableEntry;
    PF_CACHE_SETTING     CacheSet;
} AMBA_FS_SD_PRFILE2_CTRL;


static AMBA_FS_SD_PRFILE2_CTRL AmbaFS_SdPrFile2Ctrl[AMBA_NUM_SD_CHANNEL]
GNU_SECTION_NOZEROINIT;

static void AmbaFS_SdPrf2init(UINT32 SdChanNo);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaFS_SdInit
 *
 *  @Description:: Prfile2 SD device driver initializations
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaFS_SdInit(PF_DRV_TBL *pDrive)
{
    UINT32 i;

    for (i = 0U; i < AMBA_NUM_SD_CHANNEL; i ++) {
        AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[i];

        (void)AmbaWrap_memset(pSdPrFileCtrl, 0x0, sizeof(*pSdPrFileCtrl));
        pSdPrFileCtrl->pDrvTableEntry = &(pDrive[i]);
        AmbaFS_SdPrf2init(i);
    }

    return i;
}

static inline void DetermineCHS(UINT32 TotalSizeMB, UINT8 *pNumOfHead, UINT8 *pSectorPerTrack)
{
    if (TotalSizeMB <= 2U) {             /* ~   2MB */
        (*pNumOfHead) = 2U;
        (*pSectorPerTrack) = 16U;
    } else if (TotalSizeMB <= 16U) {     /* ~  16MB */
        (*pNumOfHead) = 2U;
        (*pSectorPerTrack) = 32U;
    } else if (TotalSizeMB <= 32U) {     /* ~  32MB */
        (*pNumOfHead) = 4U;
        (*pSectorPerTrack) = 32U;
    } else if (TotalSizeMB <= 128U) {    /* ~ 128MB */
        (*pNumOfHead) = 8U;
        (*pSectorPerTrack) = 32U;
    } else if (TotalSizeMB <= 256U) {    /* ~ 256MB */
        (*pNumOfHead) = 16U;
        (*pSectorPerTrack) = 32U;
    } else if (TotalSizeMB <= 504U) {    /* ~ 504MB */
        (*pNumOfHead) = 16U;
        (*pSectorPerTrack) = 63U;
    } else if (TotalSizeMB <= 1008U) {   /* ~1008MB */
        (*pNumOfHead) = 32U;
        (*pSectorPerTrack) = 63U;
    } else if (TotalSizeMB <= 2016U) {   /* ~2016MB */
        (*pNumOfHead) = 64U;
        (*pSectorPerTrack) = 63U;
    } else {
        (*pNumOfHead) = 128U;
        (*pSectorPerTrack) = 63U;
    }
}

UINT32 AmbaFS_PrFile2SdMount(UINT32 SdChanNo)
{
    UINT32 Rval = AMBA_FS_ERR_API;
    const AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[SdChanNo];
    AMBA_SD_CARD_STATUS_s CardStatus;

    (void)AmbaSD_GetCardStatus(SdChanNo, &CardStatus);

    if ((CardStatus.SdType == AMBA_SD_MEMORY) ||
        (CardStatus.SdType == AMBA_SD_eMMC)   ||
        (CardStatus.SdType == AMBA_SD_COMBO)) {
        char Drive = 'c';
        (void)AmbaWrap_memcpy(&Drive, &pSdPrFileCtrl->pDrvTableEntry->drive, sizeof(Drive));
        pdm_disk_notify_media_insert(pSdPrFileCtrl->pDiskTable);

        if (PF_RET_NO_ERR == pf2_mount(Drive)) {
            Rval = AMBA_FS_ERR_NONE;
        }
    }
    return Rval;
}

UINT32 AmbaFS_PrFile2SdUnmount(UINT32 SdChanNo)
{
    UINT32 Rval = AMBA_FS_ERR_API;
    const AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[SdChanNo];

    pdm_disk_notify_media_eject(pSdPrFileCtrl->pDiskTable);

    if ((pSdPrFileCtrl->pDrvTableEntry->stat & PF_ATTACHED) == 1U) {
        char Drive = 'c';
        (void)AmbaWrap_memcpy(&Drive, &pSdPrFileCtrl->pDrvTableEntry->drive, sizeof(Drive));
        if (PF_RET_NO_ERR == pf2_unmount(Drive, PF_UNMOUNT_FORCE)) {
            Rval = AMBA_FS_ERR_NONE;
        }
    }
    return Rval;
}

static UINT32 AmbaFS_SdPrf2FormatCalcSysSecs(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf)
{
    UINT32 NumSysSecs;

    /* Calculate number of system Sectors */
    AmbaFS_CreateBs(pDrvInfo, pBuf);
    if (pDrvInfo->FatType == EXFAT_TYPE) {
        NumSysSecs = pDrvInfo->StartLba * 2U;
    } else if (pDrvInfo->FatType == FAT32_TYPE) {
        const AMBA_FS_FAT_BS32 *pBs32;
        AmbaMisra_TypeCast(&pBs32, &pBuf); //AMBA_FS_FAT_BS32 *pBs32 = (AMBA_FS_FAT_BS32*) pBuf;
        NumSysSecs = pDrvInfo->StartLba + pBs32->Fno + (pBs32->Spf32 * pBs32->Fts) + pDrvInfo->Spc;
    } else {
        const AMBA_FS_FAT_BS16 *pBs16;
        AmbaMisra_TypeCast(&pBs16, &pBuf);
        NumSysSecs = pDrvInfo->StartLba + pBs16->Fno;
        NumSysSecs += (UINT32)pBs16->Spf * pBs16->Fts;
        NumSysSecs +=  (UINT32)pBs16->Ent >> 4U;
    }
    return NumSysSecs;
}

/**
 * These are AmbaFS_SdPrf2Format functions for SD1 and SD2.
 */
static PDM_ERROR AmbaFS_SdPrf2Format(UINT32 SdChanNo, const PDM_U_CHAR *pParam)
{
    static INT32 (*SdWriteSector[AMBA_NUM_SD_CHANNEL])(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors) = {
        AmbaSD0_WriteSector,
        AmbaSD1_WriteSector,
#ifdef AMBA_SD_CHANNEL2
        AmbaSD2_WriteSector,
#endif
    };
    static INT32 (*SdReadSector[AMBA_NUM_SD_CHANNEL])(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors) = {
        AmbaSD0_ReadSector,
        AmbaSD1_ReadSector,
#ifdef AMBA_SD_CHANNEL2
        AmbaSD2_ReadSector,
#endif
    };

    static UINT8 g_FormatBuf[32 * 512]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    AMBA_FS_DRIVE_INFO DrvInfo;
    UINT32 NumSysSecs, nsect, Sector, SectorCount;
    UINT8 *pBuf;
    INT32 Rval = 0;
    UINT32 Cnt;
    AMBA_SD_CARD_STATUS_s CardStatus;

    (void)AmbaWrap_memset(&DrvInfo, 0, sizeof(DrvInfo));

    if ((0U != AmbaSD_GetCardStatus(SdChanNo, &CardStatus)) ||
        (CardStatus.WriteProtect != 0U)) {
        Rval = PDM_ERR;
    } else {
        /* Setup Drive infomation */
        SectorCount = AmbaSD_GetTotalSectors(SdChanNo);

        AmbaFS_ParseSdaFormatParam(SectorCount, &DrvInfo);

        if ((IO_UtilityStringCompare((const char *)pParam, "no_Mbr", 6) == 0)) {
            DrvInfo.StartLba = 0;
        }

        /* StartFormat */
        nsect = 32U;
        pBuf = g_FormatBuf;

        /* Calculate number of system Sectors */
        NumSysSecs = AmbaFS_SdPrf2FormatCalcSysSecs(&DrvInfo, pBuf);

        /* Write all system Sectors as 0 */
        if (DrvInfo.FatType == EXFAT_TYPE) {
            Sector = DrvInfo.StartLba + (DrvInfo.StartLba / 2U);
        } else {
            Sector = 0;
        }
#if 0
        if ((CardStatus.SdType != AMBA_SD_eMMC) && (CardStatus.DataAfterErase == 0U)) {
            /* EMMC use 128K as a erase Unit, Only use erase func on SD card. */
            AMBA_SD_INFO_SSR_REG_s  *pSSR = &AmbaSD_Ctrl[SdChanNo].CardRegs.SdStatus;
            UINT32 EraseSector;

            INT32 AmbaSD0_EraseSector(UINT32 Sector, UINT32 Sectors);
            INT32 AmbaSD1_EraseSector(UINT32 Sector, UINT32 Sectors);

            static INT32 (*SdEraseSector[AMBA_NUM_SD_CHANNEL])(UINT32 Sector, UINT32 Sectors) = {
                AmbaSD0_EraseSector,
                AmbaSD1_EraseSector,
            };

            if (pSSR->Bits.EraseTimeout) {
                UINT32 TimeOutMs = 120U * 100U / (CardStatus.CardSpeed / 1000000U);
                UINT32 EraseTimePerUnit = pSSR->Bits.EraseTimeout / pSSR->Bits.EraseSize + pSSR->Bits.EraseOffset;
                EraseSector = (TimeOutMs / EraseTimePerUnit * (1U << pSSR->Bits.AuSize) * (16U * 1024U / 512U)) / 100U;
            } else {
                EraseSector = NumSysSecs;
            }
            for (; Sector < NumSysSecs; Sector += n) {
                if ((NumSysSecs - Sector) < EraseSector) {
                    n = NumSysSecs - Sector;
                } else {
                    n = EraseSector;
                }

                Rval = SdEraseSector[SdChanNo](Sector, n);
                if (Rval < 0) {
                    Rval = DRV_FAILED;
                    break;
                }
            }
        } else {
            (void)AmbaWrap_memset(pBuf, 0x0, nsect << SEC_SHIFT);
            for (; Sector < NumSysSecs; Sector += n) {
                if ((NumSysSecs - Sector) < nsect) {
                    n = NumSysSecs - Sector;
                } else {
                    n = nsect;
                }

                Rval = SdWriteSector[SdChanNo](pBuf, Sector, n);
                if (Rval < 0) {
                    Rval = DRV_FAILED;
                    break;
                }
            }
        }
#else
        (void)AmbaWrap_memset(pBuf, 0x0, nsect * SEC_SIZE);
        for (; Sector < NumSysSecs; Sector += Cnt) {
            if ((NumSysSecs - Sector) < nsect) {
                Cnt = NumSysSecs - Sector;
            } else {
                Cnt = nsect;
            }

            Rval = SdWriteSector[SdChanNo](pBuf, Sector, Cnt);
            if (Rval < 0) {
                Rval = DRV_FAILED;
                break;
            }
        }
#endif
        if (Rval != DRV_FAILED) {
            /* Write Mbr */
            AmbaFS_CreateMbr(&DrvInfo, pBuf);
            Rval = SdWriteSector[SdChanNo](pBuf, 0, 1U);
            if (Rval < 0) {
                Rval = DRV_FAILED;
            }
        }

        if (Rval != DRV_FAILED) {
            /* Write boot Sector area for FAT16 or FAT32 */
            Rval = AmbaFS_CreateBootArea(&DrvInfo, SdWriteSector[SdChanNo]);
            if (Rval < 0) {
                Rval = DRV_FAILED;
            }
        }

        if (Rval != DRV_FAILED) {
            /* Write FAT table */
            AmbaFS_CreateBs(&DrvInfo, pBuf);
            Rval = AmbaFS_CreateFat(&DrvInfo, pBuf, SdWriteSector[SdChanNo]);
            if (Rval < 0) {
                Rval = DRV_FAILED;
            }
        }

        if ((Rval != DRV_FAILED) &&
            (DrvInfo.FatType == EXFAT_TYPE)) {
            UINT32 Index = (4U * SEC_SIZE);
            AMBA_FS_EXFAT_BS *pExfatBs = NULL;
            AmbaMisra_TypeCast(&pExfatBs, &pBuf);

            AmbaFS_CreateBs(&DrvInfo, pBuf);
            Rval = AmbaFS_CreateExfatEntry(pExfatBs,
                                           SdWriteSector[SdChanNo], SdReadSector[SdChanNo],
                                           &pBuf[Index], (nsect - 4U) * SEC_SIZE);
            if (Rval < 0) {
                Rval = DRV_FAILED;
            }
        }
    }

    return Rval;
}

/******************************************************************/
/* This block defines Driver functions to interface with PrFILE2. */
/******************************************************************/

/**
 * These are AmbaFS_SdPrf2InitDriveTable functions for SD1 and SD2.
 */
static PF_ERROR AmbaFS_SdPrf2InitDriveTable(UINT32 SdChanNo, PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[SdChanNo];

    pDiskTable->p_func = &pSdPrFileCtrl->DrvFunc;
    pDiskTable->ui_ext = UiExt;

    return DRV_SUCCEED;
}

/**
 * These are AmbaFS_SdPrf2drv_init functions for SD1 and SD2.
 */
static PDM_ERROR AmbaFS_SdPrf2DriveInit(PDM_DISK *pPdmDisk)
{
    /* If the media has been inserted, notifies PrFILE2 */
    pdm_disk_notify_media_insert(pPdmDisk);

    return DRV_SUCCEED;
}

static PDM_ERROR AmbaFS_SdPrf2Finalize(UINT32 SdChanNo, PDM_DISK *pPdmDisk)
{
    INT32 Rval = DRV_SUCCEED;

    const AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[SdChanNo];

    if (pPdmDisk == NULL) {
        Rval = DRV_FAILED;
    } else {
        /* Termination process for the Driver and media controller */
        pdm_disk_notify_media_eject(pPdmDisk);
        pSdPrFileCtrl->pDrvTableEntry->drive = 0;
        pSdPrFileCtrl->pDrvTableEntry->stat  = 0U;
    }
    return Rval;
}

static PDM_ERROR AmbaFS_SdPrf2Mount(void)
{
    return DRV_SUCCEED;
}

static PDM_ERROR AmbaFS_SdPrf2Unmount(void)
{
    return DRV_SUCCEED;
}

/**
 * These are AmbaFS_SdPrf2GetDiskInfo functions for SD1 and SD2.
 */
static PDM_ERROR AmbaFS_SdPrf2GetDiskInfo(UINT32 SdChanNo, PDM_DISK_INFO *pPdmDiskInfo)
{
    INT32 Rval = DRV_SUCCEED;

    UINT32 SectorCount, TotalSizeMB, Attr;
    UINT8  NumOfHead = 0U, Spt = 0U;

    AMBA_SD_CARD_STATUS_s CardStatus;

    (void)AmbaSD_GetCardStatus(SdChanNo, &CardStatus);

    if ((CardStatus.SdType     == AMBA_SD_NOT_PRESENT) ||
        (CardStatus.CardIsInit == 0U) ||
        (pPdmDiskInfo == NULL)) {
        Rval = DRV_FAILED;
    } else {
        /**************************************/
        /* Caculate the necessary inFormation */
        /**************************************/
        UINT32 cylinders;
        SectorCount = AmbaSD_GetTotalSectors(SdChanNo);
        TotalSizeMB = SectorCount >> 11U;

        /* Determine CHS */
        DetermineCHS(TotalSizeMB, &NumOfHead, &Spt);

        /* Read the write protect register again to make sure wp is on or off */
        if (CardStatus.WriteProtect == 1U) {
            Attr = (PRF2_MEDIA_ATTR | PDM_MEDIA_ATTR_WRITE_PROTECT);
        } else {
            Attr = PRF2_MEDIA_ATTR;
        }

        cylinders = (SectorCount / NumOfHead) / Spt;
        pPdmDiskInfo->cylinders = (UINT16)cylinders & 0xffffU;

        pPdmDiskInfo->total_sectors     = SectorCount;
        pPdmDiskInfo->heads             = NumOfHead;
        pPdmDiskInfo->sectors_per_track = Spt;
        pPdmDiskInfo->bytes_per_sector  = SECTOR_BUF_SIZE;
        pPdmDiskInfo->media_attr        = Attr;
        pPdmDiskInfo->format_param      = NULL;
    }

    return Rval;
}

/* -------------------------------------------------------------------------- */

/* PrFile2 Hookup API for SD0 */
static PDM_ERROR AmbaFS_Sd0PrfInitDriveTable(PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    return AmbaFS_SdPrf2InitDriveTable(AMBA_SD_CHANNEL0, pDiskTable, UiExt);
}

static PDM_ERROR AmbaFS_Sd0PrfDriveInit(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2DriveInit(pDisk);
}

static PDM_ERROR AmbaFS_Sd0PrfFinalize(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2Finalize(AMBA_SD_CHANNEL0, pDisk);
}

static PDM_ERROR AmbaFS_Sd0PrfMount(PDM_DISK *pDisk)
{
    (void)pDisk;
    return AmbaFS_SdPrf2Mount();
}

static PDM_ERROR AmbaFS_Sd0PrfUnmount(PDM_DISK *pDisk)
{
    (void)pDisk;
    return AmbaFS_SdPrf2Unmount();
}

static PDM_ERROR AmbaFS_Sd0PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    (void)pDisk;
    return AmbaFS_SdPrf2Format(AMBA_SD_CHANNEL0, pParam);
}

static PDM_ERROR AmbaFS_Sd0Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                     PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                     PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;

    (void)pDisk;

    (*pNumSuccess) = 0U;

    if (0 == AmbaSD0_ReadSector(pBuf, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd0Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                      PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;
    UINT8 *pBufTmp = NULL;

    (void)pDisk;

    (*pNumSuccess) = 0U;
    AmbaMisra_TypeCast(&pBufTmp, &pBuf);

    if (0 == AmbaSD0_WriteSector(pBufTmp, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd0PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    (void)pDisk;

    return AmbaFS_SdPrf2GetDiskInfo(AMBA_SD_CHANNEL0, pDiskInfo);
}

/* PrFile2 Hookup API for SD1 */
static PDM_ERROR AmbaFS_Sd1PrfInitDriveTable(PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    return AmbaFS_SdPrf2InitDriveTable(AMBA_SD_CHANNEL1, pDiskTable, UiExt);
}

static PDM_ERROR AmbaFS_Sd1PrfDriveInit(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2DriveInit(pDisk);
}

static PDM_ERROR AmbaFS_Sd1PrfFinalize(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2Finalize(AMBA_SD_CHANNEL1, pDisk);
}

static PDM_ERROR AmbaFS_Sd1PrfMount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Mount();
}

static PDM_ERROR AmbaFS_Sd1PrfUnmount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Unmount();
}

static PDM_ERROR AmbaFS_Sd1PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Format(AMBA_SD_CHANNEL1, pParam);
}

static PDM_ERROR AmbaFS_Sd1Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                     PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;

    (void)pDisk;

    (*pNumSuccess) = 0U;

    if (0 == AmbaSD1_ReadSector(pBuf, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd1Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                      PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;
    UINT8 *pBufTmp = NULL;

    (void)pDisk;

    (*pNumSuccess) = 0U;
    AmbaMisra_TypeCast(&pBufTmp, &pBuf);

    if (0 == AmbaSD1_WriteSector(pBufTmp, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd1PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    (void)pDisk;

    return AmbaFS_SdPrf2GetDiskInfo(AMBA_SD_CHANNEL1, pDiskInfo);
}

#ifdef AMBA_SD_CHANNEL2
/* PrFile2 Hookup API for SD2 */
static PDM_ERROR AmbaFS_Sd2PrfInitDriveTable(PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    return AmbaFS_SdPrf2InitDriveTable(AMBA_SD_CHANNEL2, pDiskTable, UiExt);
}

static PDM_ERROR AmbaFS_Sd2PrfDriveInit(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2DriveInit(pDisk);
}

static PDM_ERROR AmbaFS_Sd2PrfFinalize(PDM_DISK *pDisk)
{
    return AmbaFS_SdPrf2Finalize(AMBA_SD_CHANNEL2, pDisk);
}

static PDM_ERROR AmbaFS_Sd2PrfMount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Mount();
}

static PDM_ERROR AmbaFS_Sd2PrfUnmount(PDM_DISK *pDisk)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Unmount();
}

static PDM_ERROR AmbaFS_Sd2PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    (void)pDisk;

    return AmbaFS_SdPrf2Format(AMBA_SD_CHANNEL2, pParam);
}

static PDM_ERROR AmbaFS_Sd2Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                     PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;

    (void)pDisk;

    (*pNumSuccess) = 0U;

    if (0 == AmbaSD2_ReadSector(pBuf, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd2Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                      PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR Rval = DRV_FAILED;
    UINT8 *pBufTmp = NULL;

    (void)pDisk;

    (*pNumSuccess) = 0U;
    AmbaMisra_TypeCast(&pBufTmp, &pBuf);

    if (0 == AmbaSD2_WriteSector(pBufTmp, Sector, Sectors)) {
        *pNumSuccess = Sectors;
        Rval = DRV_SUCCEED;
    }

    return Rval;
}

static PDM_ERROR AmbaFS_Sd2PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    (void)pDisk;

    return AmbaFS_SdPrf2GetDiskInfo(AMBA_SD_CHANNEL2, pDiskInfo);
}
#endif

static void AmbaFS_SdPrf2init(UINT32 SdChanNo)
{
    static PF_CACHE_PAGE g_CachePage[AMBA_NUM_SD_CHANNEL][SDPRF_FAT_PAGES + SDPRF_DATA_PAGES]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static PF_SECTOR_BUF g_CacheBuffer0[SDPRF_FAT_PAGES + SDPRF_DATA_PAGES]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    static PF_SECTOR_BUF g_CacheBuffer1[SDPRF_FAT_PAGES + SDPRF_DATA_PAGES]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

#ifdef AMBA_SD_CHANNEL2
    static PF_SECTOR_BUF g_CacheBuffer2[SDPRF_FAT_PAGES + SDPRF_DATA_PAGES]
    GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif

    AMBA_FS_SD_PRFILE2_CTRL *pSdPrFileCtrl = &AmbaFS_SdPrFile2Ctrl[SdChanNo];

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        char Drive = 'c';
        (void)AmbaWrap_memcpy(&pSdPrFileCtrl->pDrvTableEntry->drive, &Drive, sizeof(pSdPrFileCtrl->pDrvTableEntry->drive));

        pSdPrFileCtrl->DrvFunc.init           = AmbaFS_Sd0PrfDriveInit;
        pSdPrFileCtrl->DrvFunc.finalize       = AmbaFS_Sd0PrfFinalize;
        pSdPrFileCtrl->DrvFunc.mount          = AmbaFS_Sd0PrfMount;
        pSdPrFileCtrl->DrvFunc.unmount        = AmbaFS_Sd0PrfUnmount;
        pSdPrFileCtrl->DrvFunc.format         = AmbaFS_Sd0PrfFormat;
        pSdPrFileCtrl->DrvFunc.physical_read  = AmbaFS_Sd0Prf2Pread;
        pSdPrFileCtrl->DrvFunc.physical_write = AmbaFS_Sd0Prf2Pwrite;
        pSdPrFileCtrl->DrvFunc.get_disk_info  = AmbaFS_Sd0PrfGetDiskInfo;
        pSdPrFileCtrl->CacheSet.buffers       = g_CacheBuffer0;
        pSdPrFileCtrl->DrvInitTable.p_func    = AmbaFS_Sd0PrfInitDriveTable;
    } else if (SdChanNo == AMBA_SD_CHANNEL1) {
        char Drive = 'd';
        (void)AmbaWrap_memcpy(&pSdPrFileCtrl->pDrvTableEntry->drive, &Drive, sizeof(pSdPrFileCtrl->pDrvTableEntry->drive));

        pSdPrFileCtrl->DrvFunc.init           = AmbaFS_Sd1PrfDriveInit;
        pSdPrFileCtrl->DrvFunc.finalize       = AmbaFS_Sd1PrfFinalize;
        pSdPrFileCtrl->DrvFunc.mount          = AmbaFS_Sd1PrfMount;
        pSdPrFileCtrl->DrvFunc.unmount        = AmbaFS_Sd1PrfUnmount;
        pSdPrFileCtrl->DrvFunc.format         = AmbaFS_Sd1PrfFormat;
        pSdPrFileCtrl->DrvFunc.physical_read  = AmbaFS_Sd1Prf2Pread;
        pSdPrFileCtrl->DrvFunc.physical_write = AmbaFS_Sd1Prf2Pwrite;
        pSdPrFileCtrl->DrvFunc.get_disk_info  = AmbaFS_Sd1PrfGetDiskInfo;
        pSdPrFileCtrl->CacheSet.buffers       = g_CacheBuffer1;
        pSdPrFileCtrl->DrvInitTable.p_func  = AmbaFS_Sd1PrfInitDriveTable;
#ifdef AMBA_SD_CHANNEL2
    } else if (SdChanNo == AMBA_SD_CHANNEL2) {
        char Drive = 'e';
        (void)AmbaWrap_memcpy(&pSdPrFileCtrl->pDrvTableEntry->drive, &Drive, sizeof(pSdPrFileCtrl->pDrvTableEntry->drive));

        pSdPrFileCtrl->DrvFunc.init           = AmbaFS_Sd2PrfDriveInit;
        pSdPrFileCtrl->DrvFunc.finalize       = AmbaFS_Sd2PrfFinalize;
        pSdPrFileCtrl->DrvFunc.mount          = AmbaFS_Sd2PrfMount;
        pSdPrFileCtrl->DrvFunc.unmount        = AmbaFS_Sd2PrfUnmount;
        pSdPrFileCtrl->DrvFunc.format         = AmbaFS_Sd2PrfFormat;
        pSdPrFileCtrl->DrvFunc.physical_read  = AmbaFS_Sd2Prf2Pread;
        pSdPrFileCtrl->DrvFunc.physical_write = AmbaFS_Sd2Prf2Pwrite;
        pSdPrFileCtrl->DrvFunc.get_disk_info  = AmbaFS_Sd2PrfGetDiskInfo;
        pSdPrFileCtrl->CacheSet.buffers       = g_CacheBuffer2;
        pSdPrFileCtrl->DrvInitTable.p_func  = AmbaFS_Sd2PrfInitDriveTable;
#endif
    } else {
        /* for MisraC check */
    }

    pSdPrFileCtrl->DrvInitTable.ui_ext = 0U;

    (void) pdm_open_disk(&pSdPrFileCtrl->DrvInitTable, &pSdPrFileCtrl->pDiskTable);

    /* Open a partition */
    (void) pdm_open_partition(pSdPrFileCtrl->pDiskTable, 0, &pSdPrFileCtrl->pPartTable);

    /* Initialize Driver table */
    AmbaMisra_TypeCast(&pSdPrFileCtrl->pDrvTableEntry->p_part, &pSdPrFileCtrl->pPartTable);
    pSdPrFileCtrl->CacheSet.pages             = &g_CachePage[SdChanNo][0];
    pSdPrFileCtrl->CacheSet.num_fat_pages     = SDPRF_FAT_PAGES;
    pSdPrFileCtrl->CacheSet.num_data_pages    = SDPRF_DATA_PAGES;
    pSdPrFileCtrl->CacheSet.num_fat_buf_size  = SDPRF_FAT_BUF_SIZE;
    pSdPrFileCtrl->CacheSet.num_data_buf_size = SDPRF_DATA_BUF_SIZE;
    pSdPrFileCtrl->pDrvTableEntry->cache      = &pSdPrFileCtrl->CacheSet;
}
