/**
 *  @file AmbaFS_Format.c
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
 *  @details Ambarella file system format layer.
 *
 */

#include "AmbaDef.h"
#include "AmbaFS.h"

#include "AmbaFS_Format.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaFS_UpTbl.h"

#define SYS_AREA_SECS       (12U)  /**< EXFAT System Area Sectors */
#define EXT_BOOT_SEC_NUM    (8U)   /**< EXFAT Main Extended Boot Sectors */
#define EXFAT_BMP_SPC       (8U * SEC_SIZE)
#define EXFAT_SRT_CLUSTER   (2U)

#define SYS_AREA_SIZE       (SYS_AREA_SECS * SEC_SIZE)

static const UINT16 UpCaseTable[] GNU_ALIGNED_CACHESAFE = UPCASE_TABLE;

static void CreateFsi2(UINT8 *pBuf);

static UINT32 AmbaFS_ExFatTypeDetermination(AMBA_FS_EXFAT_BS * pBs)
{
    UINT32 Rval = FS_EXFAT, ClusterSize = 0;

    AmbaMisra_TouchUnused(pBs);

    if (pBs != NULL) {
        /* check MustBezero */
        for (UINT8 n = 0; n < sizeof(pBs->Mbz); n++) {
            if (pBs->Mbz[n] != 0U) {
                Rval = FS_FAT_UNKNOWN;
                break;
            }
        }
        if (Rval != FS_FAT_UNKNOWN) {
            if ((pBs->Bps != 9U)  && (pBs->Bps != 10U) &&
                (pBs->Bps != 11U) && (pBs->Bps != 12U)) {
                Rval = 0x0;
            }

            if ((pBs->Spc != 6U)  && (pBs->Spc != 8U)  &&
                (pBs->Spc != 9U)  && (pBs->Spc != 10U)) {
                Rval = 0x0;
            }
            ClusterSize = (UINT32)pBs->Bps * pBs->Spc;
            if (ClusterSize > 524288U) {
                Rval = 0x0;
            }
        }
    } else {
        Rval = 0;
    }

    return Rval;
}

/**
 * FAT Type determination.
 */
UINT32 AmbaFS_FatTypeDetermination(AMBA_FS_FAT_BS32 * pBs)
{
    UINT32 FATSz, TotSec, DataSec, CountofClusters;
    UINT32 Rval = 0;

    AmbaMisra_TouchUnused(pBs);

    /* Check boot Sector is Microsoft FAT32 boot Sector or not */
    if (pBs->Mrk != 0xaa55U) {
        Rval = 0x0;
    } else {
        if ((pBs->Bps != 512U)  && (pBs->Bps != 1024U) &&
            (pBs->Bps != 2048U) && (pBs->Bps != 4096U)) {
            AMBA_FS_EXFAT_BS *pExfatBs;

            AmbaMisra_TypeCast(&pExfatBs, &pBs);
            Rval = AmbaFS_ExFatTypeDetermination(pExfatBs);
        } else {
            UINT32 ClusterSize = (UINT32)pBs->Bps * pBs->Spc;
            if ((pBs->Spc != 1U)  && (pBs->Spc != 2U)  &&
                (pBs->Spc != 4U)  && (pBs->Spc != 8U)  &&
                (pBs->Spc != 16U) && (pBs->Spc != 32U) &&
                (pBs->Spc != 64U) && (pBs->Spc != 128U)) {
                Rval = 0x0;
            } else if (ClusterSize > 65536U) {
                Rval = 0x0;
            } else {

                if (pBs->Spf != 0U) {
                    FATSz = pBs->Spf;
                } else {
                    FATSz = pBs->Spf32;
                }

                if (pBs->All != 0U) {
                    TotSec = pBs->All;
                } else {
                    TotSec = pBs->Lal;
                }

                if (TotSec == 0U) {
                    Rval = 0x0;
                } else {
                    /* DataSec = TotSec - (BPB_RescdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors) */
                    DataSec = TotSec - (pBs->Fno + (pBs->Fts * FATSz));

                    /* CountofClusters = DataSec / BPB_SecPerClus */
                    CountofClusters = DataSec / pBs->Spc;

                    if (CountofClusters < 4085U) {
                        Rval = FS_FAT12;    /* Volume is FAT12 */
                    } else if (CountofClusters < 65525U) {
                        Rval = FS_FAT16;    /* Volume is FAT16 */
                    } else {
                        Rval = FS_FAT32;    /* Volume is FAT32 */
                    }
                }
            }
        }
    }
    return Rval;
}

void AmbaFS_CreateMbr(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf)
{
    UINT32 StartSector, StartHead, start_cyl;
    UINT32 end_Sector, end_head, end_cyl;
    AMBA_FS_FAT_MBR *pMbr = NULL;

    AmbaMisra_TouchUnused(pDrvInfo);

    StartSector = (pDrvInfo->StartLba % (UINT32)pDrvInfo->Spt) + 1U;
    StartHead   = (pDrvInfo->StartLba / pDrvInfo->Spt) % pDrvInfo->NumOfHead;

    start_cyl   = (UINT32)pDrvInfo->NumOfHead * pDrvInfo->Spt;
    start_cyl   = pDrvInfo->StartLba / start_cyl;

    end_Sector = (pDrvInfo->EndLba % pDrvInfo->Spt) + 1U;
    end_head   = (pDrvInfo->EndLba / pDrvInfo->Spt) % pDrvInfo->NumOfHead;

    end_cyl    = (UINT32)pDrvInfo->NumOfHead * pDrvInfo->Spt;
    end_cyl    = pDrvInfo->EndLba / end_cyl;

    if (OK != AmbaWrap_memset(pBuf, 0, SEC_SIZE)) { /* Misrac */ };
    AmbaMisra_TypeCast(&pMbr, &pBuf); /* pMbr = (AMBA_FS_FAT_MBR *) pBuf; */

    pMbr->PartTable0.BootType      = 0x80U;
    pMbr->PartTable0.FatType       = pDrvInfo->FatId;
    pMbr->PartTable0.LogicStartSector = pDrvInfo->StartLba;
    pMbr->PartTable0.AllSector        = pDrvInfo->TotalSector;
    pMbr->Signature = 0xaa55U;

    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.StartHead, &StartHead, sizeof(pMbr->PartTable0.StartHead))) { /* Misrac */ };

    StartSector = (StartSector | ((start_cyl >> 2U) & 0xc0U));
    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.StartSector, &StartSector, sizeof(pMbr->PartTable0.StartSector))) { /* Misrac */ };

    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.StartCylinder, &start_cyl, sizeof(pMbr->PartTable0.StartCylinder))) { /* Misrac */ };

    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.EndHead, &end_head, sizeof(pMbr->PartTable0.EndHead))) { /* Misrac */ };

    end_Sector = (end_Sector | ((end_cyl >> 2U) & 0xc0U));
    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.EndSector, &end_Sector, sizeof(pMbr->PartTable0.EndSector))) { /* Misrac */ };

    if (OK != AmbaWrap_memcpy(&pMbr->PartTable0.EndCylinder, &end_cyl, sizeof(pMbr->PartTable0.EndCylinder))) { /* Misrac */ };

    if (pDrvInfo->FatId == FS_EXFAT) {
        /* Fixed value recommended by customer. */
        pMbr->PartTable0.StartHead     = 0x0aU;
        pMbr->PartTable0.StartSector   = 0x09U;
        pMbr->PartTable0.StartCylinder = 0x02U;
        pMbr->PartTable0.FatType       = 0x07U;
        pMbr->PartTable0.EndHead       = 0xfeU;
        pMbr->PartTable0.EndSector     = 0xffU;
        pMbr->PartTable0.EndCylinder   = 0xffU;
    }
}

void AmbaFS_CreateBs(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf)
{
    AMBA_FS_EXFAT_BS *pBs64;
    AMBA_FS_FAT_BS32 *pBs32;
    AMBA_FS_FAT_BS16 *pBs16;
    UINT32 TotalSizeMb;
    UINT32 BmpClusCnt, secs, UpClusCnt;
    UINT32 Tmp;

    const UINT16 *pUpTbl = UpCaseTable;
    AmbaMisra_TouchUnused(&pUpTbl);

    AmbaMisra_TouchUnused(pDrvInfo);

    TotalSizeMb = pDrvInfo->TotalSector >> 11;

    /* Set Boot Sector */
    if (OK != AmbaWrap_memset(pBuf, 0, SEC_SIZE)) { /* Misrac */ }

    if (pDrvInfo->FatType == 64U) {
        UINT32 Bu = pDrvInfo->StartLba, Spc = pDrvInfo->Spc;
        AmbaMisra_TypeCast(&pBs64, &pBuf);
        /* common for exfat, fat32 and fat16 */
        pBs64->Jmp = 0xebU;
        /* jump offert = 76h reference from PrFile2_EXFAT */
        pBs64->Adr = 0x76U;
        pBs64->Nop = 0x90U;
        pBs64->Maker[0] = (UINT8) 'E';
        pBs64->Maker[1] = (UINT8) 'X';
        pBs64->Maker[2] = (UINT8) 'F';
        pBs64->Maker[3] = (UINT8) 'A';
        pBs64->Maker[4] = (UINT8) 'T';
        pBs64->Maker[5] = (UINT8) ' ';
        pBs64->Maker[6] = (UINT8) ' ';
        pBs64->Maker[7] = (UINT8) ' ';
        pBs64->Pto = Bu;
        pBs64->Vol = pDrvInfo->TotalSector;

        pBs64->Fto = Bu / 2U;
        pBs64->Ftl = Bu / 2U;
        pBs64->Cho = Bu;
        pBs64->Cnt = (pDrvInfo->TotalSector - pBs64->Cho) / pDrvInfo->Spc;
        BmpClusCnt = pBs64->Cnt / (EXFAT_BMP_SPC * pDrvInfo->Spc);
        if ((pBs64->Cnt % (EXFAT_BMP_SPC * pDrvInfo->Spc)) != 0U) {
            BmpClusCnt ++;
        }

        secs = (sizeof(UpCaseTable) / SEC_SIZE) + 1U;
        UpClusCnt = secs / pDrvInfo->Spc;
        if ((secs % pDrvInfo->Spc) != 0U) {
            UpClusCnt ++;
        }

        pBs64->Fcr = EXFAT_SRT_CLUSTER + BmpClusCnt + UpClusCnt;

        pBs64->Vsn = 0x0; /* FIXME */
        pBs64->Fsr = 0x0100U;
        pBs64->Bps = 9U;

        Spc >>= 1U;
        for (pBs64->Spc = 0; Spc > 0U; Spc >>= 1U) {
            pBs64->Spc ++;
        }

        pBs64->Fts = 1U;
        pBs64->Dsl = 0x80U;
        pBs64->Mrk = 0xaa55U;

    } else if (pDrvInfo->FatType == 32U) {
        AmbaMisra_TypeCast(&pBs32, &pBuf);
        pBs32->Spc = (UINT8) (pDrvInfo->Spc & 0xffU);

        if (pDrvInfo->BsSizeCustom != 0U) {
            pBs32->Fno = (UINT16) (pDrvInfo->BsSizeCustom & 0xffFFU);;
        } else {
            pBs32->Fno = FAT32_BS_SIZE;
        }

        pBs32->Ent32 = 2U;
        pBs32->Fsi32 = 1U;
        pBs32->Bck32 = 6U;
        pBs32->Typ[0] = (UINT8) 'F';
        pBs32->Typ[1] = (UINT8) 'A';
        pBs32->Typ[2] = (UINT8) 'T';
        pBs32->Typ[3] = (UINT8) '3';
        pBs32->Typ[4] = (UINT8) '2';
        pBs32->Typ[5] = (UINT8) ' ';
        pBs32->Typ[6] = (UINT8) ' ';
        pBs32->Typ[7] = (UINT8) ' ';
        pBs32->Spf32 = pDrvInfo->Spf;
        /* jump offert = 58h is the Length of BPB */
        Tmp = SEC_SIZE - (sizeof(pBs32->Rsv1) - 4U);
        pBs32->Adr = (UINT8)(Tmp & 0xffU);

        /* common for fat32 and fat16 */
        pBs32->Jmp = 0xebU;
        pBs32->Nop = 0x90U;
        pBs32->Maker[0] = (UINT8) 'M';
        pBs32->Maker[1] = (UINT8) 'S';
        pBs32->Maker[2] = (UINT8) 'W';
        pBs32->Maker[3] = (UINT8) 'I';
        pBs32->Maker[4] = (UINT8) 'N';
        pBs32->Maker[5] = (UINT8) '4';
        pBs32->Maker[6] = (UINT8) '.';
        pBs32->Maker[7] = (UINT8) '1';
        pBs32->Bps = SEC_SIZE;
        pBs32->Fts = NFAT;
        pBs32->Med = 0xf8;
        pBs32->Spt = pDrvInfo->Spt;
        pBs32->Hed = pDrvInfo->NumOfHead;
        pBs32->Hid = pDrvInfo->StartLba;
        if (TotalSizeMb < 32U) {
            pBs32->All = (UINT16)(pDrvInfo->TotalSector & 0xffffU);
            pBs32->Lal = 0;
        } else {
            pBs32->All = 0;
            pBs32->Lal = pDrvInfo->TotalSector;
        }
        pBs32->Drvno = 0x80U;
        pBs32->Exb = 0x29U;
        pBs32->Vno = 0x87654321U;
        pBs32->Vnm[0] = (UINT8) 'N';
        pBs32->Vnm[1] = (UINT8) 'O';
        pBs32->Vnm[2] = (UINT8) ' ';
        pBs32->Vnm[3] = (UINT8) 'N';
        pBs32->Vnm[4] = (UINT8) 'A';
        pBs32->Vnm[5] = (UINT8) 'M';
        pBs32->Vnm[6] = (UINT8) 'E';
        pBs32->Vnm[7] = (UINT8) ' ';
        pBs32->Vnm[8] = (UINT8) ' ';
        pBs32->Vnm[9] = (UINT8) ' ';
        pBs32->Vnm[10] = (UINT8) ' ';
        pBs32->Mrk = 0xaa55;
    } else {
        AmbaMisra_TypeCast(&pBs16, &pBuf);
        if (pDrvInfo->FatType == 12U) {
            pBs16->Typ[0] = (UINT8) 'F';
            pBs16->Typ[1] = (UINT8) 'A';
            pBs16->Typ[2] = (UINT8) 'T';
            pBs16->Typ[3] = (UINT8) '1';
            pBs16->Typ[4] = (UINT8) '2';
            pBs16->Typ[5] = (UINT8) ' ';
            pBs16->Typ[6] = (UINT8) ' ';
            pBs16->Typ[7] = (UINT8) ' ';
        } else {
            pBs16->Typ[0] = (UINT8) 'F';
            pBs16->Typ[1] = (UINT8) 'A';
            pBs16->Typ[2] = (UINT8) 'T';
            pBs16->Typ[3] = (UINT8) '1';
            pBs16->Typ[4] = (UINT8) '6';
            pBs16->Typ[5] = (UINT8) ' ';
            pBs16->Typ[6] = (UINT8) ' ';
            pBs16->Typ[7] = (UINT8) ' ';

        }
        pBs16->Spc = (UINT8)(pDrvInfo->Spc & 0xffU);
        if (pDrvInfo->BsSizeCustom != 0U) {
            pBs16->Fno = (UINT16)(pDrvInfo->BsSizeCustom & 0xffffU);
        } else {
            pBs16->Fno = FAT16_BS_SIZE;
        }

        if (pDrvInfo->RdeCustom != 0U) {
            pBs16->Ent = pDrvInfo->RdeCustom;
        } else {
            pBs16->Ent = ROOT_DIR_ENTRY;
        }

        pBs16->Spf = (UINT16)(pDrvInfo->Spf & 0xffffU);

        /* jump offert = 3Ch is the Length of BPB */
        Tmp = SEC_SIZE - (sizeof(pBs16->Rsv1) - 4U);
        pBs16->Adr = (UINT8)(Tmp & 0xffU);

        /* common for fat32 and fat16 */
        pBs16->Jmp = 0xebU;
        pBs16->Nop = 0x90U;
        pBs16->Maker[0] = (UINT8) 'M';
        pBs16->Maker[1] = (UINT8) 'S';
        pBs16->Maker[2] = (UINT8) 'W';
        pBs16->Maker[3] = (UINT8) 'I';
        pBs16->Maker[4] = (UINT8) 'N';
        pBs16->Maker[5] = (UINT8) '4';
        pBs16->Maker[6] = (UINT8) '.';
        pBs16->Maker[7] = (UINT8) '1';
        pBs16->Bps = SEC_SIZE;
        pBs16->Fts = NFAT;
        pBs16->Med = 0xf8;
        pBs16->Spt = pDrvInfo->Spt;
        pBs16->Hed = pDrvInfo->NumOfHead;
        pBs16->Hid = pDrvInfo->StartLba;
        if (TotalSizeMb < 32U) {
            pBs16->All = (UINT16)(pDrvInfo->TotalSector & 0xffffU);
            pBs16->Lal = 0;
        } else {
            pBs16->All = 0;
            pBs16->Lal = pDrvInfo->TotalSector;
        }
        pBs16->Drvno = 0x80U;
        pBs16->Exb = 0x29U;
        pBs16->Vno = 0x87654321U;
        pBs16->Vnm[0] = (UINT8) 'N';
        pBs16->Vnm[1] = (UINT8) 'O';
        pBs16->Vnm[2] = (UINT8) ' ';
        pBs16->Vnm[3] = (UINT8) 'N';
        pBs16->Vnm[4] = (UINT8) 'A';
        pBs16->Vnm[5] = (UINT8) 'M';
        pBs16->Vnm[6] = (UINT8) 'E';
        pBs16->Vnm[7] = (UINT8) ' ';
        pBs16->Vnm[8] = (UINT8) ' ';
        pBs16->Vnm[9] = (UINT8) ' ';
        pBs16->Vnm[10] = (UINT8) ' ';
        pBs16->Mrk = 0xaa55U;
    }
}

static void CreateFsi(UINT8 *pBuf)
{
    AMBA_FS_FSI_INFO *pFsi;

    if (OK != AmbaWrap_memset(pBuf, 0x0, SEC_SIZE)) { /* Misrac */ };
    AmbaMisra_TypeCast(&pFsi, &pBuf); /*     pFsi = (AMBA_FS_FSI_INFO *)pBuf; */

    /* Always disable FSI */
    pFsi->FsiNextClsNo = 0xffffffffU;
    pFsi->FsiEmpClst   = 0xffffffffU;

    pFsi->Title     = 0x41615252U;
    pFsi->Signature = 0x61417272U;
    pFsi->MagicNum  = 0xaa55U;
}

static void CreateFsi2(UINT8* pBuf)
{
    typedef struct {
        UINT8   reserved1[510U];
        UINT16  MagicNum;
    } AMBA_FS_INFO_SECTOR2;

    AMBA_FS_INFO_SECTOR2 *pFsi;

    if (OK != AmbaWrap_memset(pBuf, 0x0, SEC_SIZE)) { /* Misrac */ };
    AmbaMisra_TypeCast(&pFsi, &pBuf); /* pFsi = (AMBA_FS_INFO_SECTOR2*)pBuf; */

    pFsi->MagicNum = 0xaa55U;
    AmbaMisra_TouchUnused(pFsi->reserved1);
}

INT32 AmbaFS_CreateFat(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBs,
                       AMBA_FS_WR_SEC_FUNC WrSecFunc)
{
    const AMBA_FS_EXFAT_BS *pBs64 = NULL;
    const AMBA_FS_FAT_BS32 *pBs32 = NULL;
    const AMBA_FS_FAT_BS16 *pBs16 = NULL;
    UINT8 FatBuf[SEC_SIZE];
    UINT8 NumFat;
    UINT32 Sector, FatSec;
    INT32 Rval = 0;

    AmbaMisra_TouchUnused(pBs);
    AmbaMisra_TouchUnused(pDrvInfo);

    if (pDrvInfo->FatType == 64U) {
        AmbaMisra_TypeCast(&pBs64, &pBs); /* pBs64 = (AMBA_FS_EXFAT_BS*) pBs; */
        NumFat = pBs64->Fts;
        FatSec = pDrvInfo->StartLba + pBs64->Fto;
    } else if (pDrvInfo->FatType == 32U) {
        AmbaMisra_TypeCast(&pBs32, &pBs); /* pBs32 = (AMBA_FS_FAT_BS32*) pBs; */
        NumFat = pBs32->Fts;
        FatSec = pDrvInfo->StartLba + pBs32->Fno;
    } else {
        AmbaMisra_TypeCast(&pBs16, &pBs); //pBs16 = (AMBA_FS_FAT_BS16*) pBs;
        NumFat = pBs16->Fts;
        FatSec = pDrvInfo->StartLba + pBs16->Fno;
    }

    for (UINT32 i = 0U; i < NumFat; i++) {
        /* write the first Sector of FAT */
        if (OK != AmbaWrap_memset(FatBuf, 0x0, SEC_SIZE)) { /* Misrac */ };

        if (pDrvInfo->FatType == 64U) {

            FatBuf[0] = 0xf8U;
            FatBuf[1] = 0xffU;
            FatBuf[2] = 0xffU;
            FatBuf[3] = 0xffU;
            FatBuf[4] = 0xffU;
            FatBuf[5] = 0xffU;
            FatBuf[6] = 0xffU;
            FatBuf[7] = 0xffU;
        } else if (pDrvInfo->FatType == 32U) {
            UINT32 Tmp;
            if (pBs32 != NULL) {
                FatBuf[0] = pBs32->Med;
            }
            FatBuf[1] = 0xffU;
            FatBuf[2] = 0xffU;
            FatBuf[3] = 0x0fU;

            Tmp = 0x0fffffffU;
            if (OK != AmbaWrap_memcpy(&FatBuf[4], &Tmp, sizeof(Tmp))) { /* Misrac */ }; //*((UINT32*)&FatBuf[4]) = 0x0fffffffU;

            Tmp = 0x0fffffffU;
            if (OK != AmbaWrap_memcpy(&FatBuf[8], &Tmp, sizeof(Tmp))) { /* Misrac */ }; //*((UINT32*)&FatBuf[8]) = 0x0fffffffU;
        } else {
            /* setup buffer for fat16 or fat12 */
            if (pBs16 != NULL) {
                FatBuf[0] = pBs16->Med;
            }
            FatBuf[1] = 0xffU;
            FatBuf[2] = 0xffU;
            if (pDrvInfo->FatType == 16U) {
                FatBuf[3] = 0xffU;
            }
        }

        FatSec += i * pDrvInfo->Spf;
        Sector = FatSec;
        Rval = WrSecFunc(FatBuf, Sector, 1U);
        if (Rval < 0) {
            Rval = -1;
            break;
        }

        /* Remove this from create_FAT,storage Drive should take over
         * clearing the remain Sectors of FAT */
    }

    return Rval;
}

static UINT32 BootCheckSum(const UINT8 *pBuf, UINT16 bps)
{
    UINT32 bytes = (UINT32)bps * SYS_AREA_SECS;
    UINT32 ChkSum = 0, Value;
    UINT32 Idx;

    for (Idx = 0; Idx < bytes; Idx ++) {
        if ((Idx == 106U) || (Idx == 107U) || (Idx == 112U)) {
            continue;
        }
        if ((ChkSum & 1U) != 0U) {
            Value = 0x80000000U;
        } else {
            Value = 0;
        }
        ChkSum = Value + (ChkSum >> 1U) + (UINT32)pBuf[Idx];
#if 0
        ChkSum = ((ChkSum & 1U) ? 0x80000000U : 0U) + (ChkSum >> 1U) + (UINT32)pBuf[Idx];
#endif
    }
    return ChkSum;

}

static void CreateSystemArea(AMBA_FS_DRIVE_INFO *pDrvInfo, UINT8 *pBuf)
{
    UINT32 i, Sector = 0;
    UINT8 *pPtr;       /**< EXFAT System Area Buffer */
    UINT32 *pBuf32;
    AMBA_FS_EXFAT_FLASH_PARAM *pFlashParam; /**< EXFAT Flash parameter */
    UINT32 ChkSum;

    if (OK != AmbaWrap_memset(pBuf, 0x0, SYS_AREA_SIZE)) { /* Misrac */ };

    /* Master Boot Sec(0) */
    pPtr = &pBuf[Sector];
    AmbaFS_CreateBs(pDrvInfo, pPtr);

    /* Ext boot sec(1~8) */
    for (i = 0; i < EXT_BOOT_SEC_NUM; i++) {
        Sector += 1U;
        pPtr = &pBuf[Sector << SEC_SHIFT];
        pPtr[SEC_SIZE - 2U] = 0x55U;
        pPtr[SEC_SIZE - 1U] = 0xaaU;
    }

    /* OEM Parameters(9) & Reserved(10) */
    //    K_ASSERT(Sector == EXT_BOOT_SEC_NUM);
    Sector += 1U;
    pPtr = &pBuf[Sector << SEC_SHIFT];
    AmbaMisra_TypeCast(&pFlashParam, &pPtr);

    /* Set GUID to {0a0c-7e46-3399-4021-90c8-fa6d-389c-4ba2} */
    pFlashParam->Guid[0] = 0x46U;
    pFlashParam->Guid[1] = 0x7EU;
    pFlashParam->Guid[2] = 0x0CU;
    pFlashParam->Guid[3] = 0x0AU;
    pFlashParam->Guid[4] = 0x99U;
    pFlashParam->Guid[5] = 0x33U;
    pFlashParam->Guid[6] = 0x21U;
    pFlashParam->Guid[7] = 0x40U;
    pFlashParam->Guid[8] = 0x90U;
    pFlashParam->Guid[9] = 0xC8U;
    pFlashParam->Guid[10] = 0xFAU;
    pFlashParam->Guid[11] = 0x6DU;
    pFlashParam->Guid[12] = 0x38U;
    pFlashParam->Guid[13] = 0x9CU;
    pFlashParam->Guid[14] = 0x4BU;
    pFlashParam->Guid[15] = 0xA2U;

    pFlashParam->Ebs = (pDrvInfo->StartLba / 2U) * SEC_SIZE;

    /* Main Boot CheckSum(11) */
    Sector += 2U;
    pPtr = &pBuf[Sector << SEC_SHIFT];
    ChkSum = BootCheckSum(pBuf, SEC_SIZE);
    AmbaMisra_TypeCast(&pBuf32, &pPtr);
    for (i = 0; i < (SEC_SIZE / sizeof(UINT32)); i++) {
        pBuf32[i] = ChkSum;
    }
}

INT32 AmbaFS_CreateBootArea(AMBA_FS_DRIVE_INFO *pDrvInfo, AMBA_FS_WR_SEC_FUNC WrSecFunc)
{
    static UINT8 g_SysAreaBuf[SYS_AREA_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    INT32 i, Rval;
    UINT32 Sector = 0, Sector2 = 0;
    UINT8 Buf[SEC_SIZE];
    AMBA_FS_FAT_BS32 Bs32;

    if (OK != AmbaWrap_memset(Buf, 0x0, SEC_SIZE)) { /* Misrac */ };

    if (pDrvInfo->FatType == EXFAT_TYPE) {

        CreateSystemArea(pDrvInfo, g_SysAreaBuf);

        /* Write to MAIN and Backup */
        Sector = pDrvInfo->StartLba;
        Rval = WrSecFunc(g_SysAreaBuf, Sector, SYS_AREA_SECS);
        if (Rval == 0) {
            Sector = pDrvInfo->StartLba + SYS_AREA_SECS;
            Rval = WrSecFunc(g_SysAreaBuf, Sector, SYS_AREA_SECS);
        }
    } else if (pDrvInfo->FatType == FAT32_TYPE) {

        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0:
                AmbaFS_CreateBs(pDrvInfo, Buf);
                if (OK != AmbaWrap_memcpy(&Bs32, Buf, SEC_SIZE)) { /* Misrac */ };
                Sector = pDrvInfo->StartLba;
                Sector2 = pDrvInfo->StartLba + Bs32.Bck32;
                break;
            case 1:
                CreateFsi(Buf);
                Sector++;
                Sector2++;
                break;
            case 2:
                CreateFsi2(Buf);
                Sector++;
                Sector2++;
                break;
            default:
                /* For MisraC checking. */
                break;
            }
            Rval = WrSecFunc(Buf, Sector, 1U);
            if (Rval < 0) {
                Rval = -1;
            } else {
                Rval = WrSecFunc(Buf, Sector2, 1);
            }
        }
    } else {
        AmbaFS_CreateBs(pDrvInfo, Buf);
        Sector = pDrvInfo->StartLba;
        Rval = WrSecFunc(Buf, Sector, 1U);
    }
    return Rval;
}

static INT32 CreateAllocationBitmap(AMBA_FS_EXFAT_BS *pBs,
                                    AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                                    UINT8 *pExtraBuf, UINT32 ExtraSize)
{
    UINT32 BmpCnt, BmpClusCnt, ExtraBlks, n;
    UINT32 TmpPto;
    INT32  Rval = 0;
    UINT32 *pTbl;

    AmbaMisra_TouchUnused(pBs);

    ExtraBlks = ExtraSize >> SEC_SHIFT;
    if ((pBs->Cnt % EXFAT_BMP_SPC) != 0U) {
        BmpCnt = (pBs->Cnt / EXFAT_BMP_SPC) + 1U;
    } else {
        BmpCnt = (pBs->Cnt / EXFAT_BMP_SPC);
    }
    BmpClusCnt = 1U;
    BmpClusCnt = BmpCnt / (BmpClusCnt << pBs->Spc);

    /* Clear bitmap area */

    if (OK != AmbaWrap_memset(pExtraBuf, 0x0, ExtraSize)) { /* Misrac */ };
    for (UINT32 i = 0; i < BmpCnt; i += n) {
        if ((BmpCnt - i) < ExtraBlks) {
            n = (BmpCnt - i);
        } else {
            n = ExtraBlks;
        }

        AmbaMisra_TypeCast32(&TmpPto, &pBs->Pto);
        Rval = WrSecFunc(pExtraBuf, TmpPto + pBs->Cho + i, n);
        if (Rval < 0) {
            Rval = -1;
            break;
        }
    }

    if (Rval != -1) {
        /* Update FAT table */
        AmbaMisra_TypeCast32(&TmpPto, &pBs->Pto);
        Rval = RdSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1);
        if (Rval < 0) {
            Rval = -1;
        } else {
            AmbaMisra_TypeCast(&pTbl, &pExtraBuf);
            for (UINT32 i = 0U; i < BmpClusCnt; i++) {
                pTbl[i + EXFAT_SRT_CLUSTER] = i + EXFAT_SRT_CLUSTER + 1U;
            }

            pTbl[BmpClusCnt + EXFAT_SRT_CLUSTER] = 0xFFFFFFFFU;
            AmbaMisra_TypeCast32(&TmpPto, &pBs->Pto);
            Rval = WrSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1U);
        }
    }
    return Rval;
}

#if 0
static UINT32 TableCheckSum(UINT8 *pTbl, UINT64 Len)
{
    UINT32 ChkSum = 0U;
    UINT64 Idx;

    for (Idx = 0U; Idx < Len; Idx += 1U) {
#if 0
        ChkSum = ((ChkSum & 1U) ? 0x80000000U : 0U) +
                 (ChkSum >> 1U) + (UINT32) pTbl[Idx];
#else
        UINT32 Value;
        if ((ChkSum & 1U) != 0U) {
            Value = 0x80000000U;
        } else {
            Value = 0;
        }
        ChkSum = Value + (ChkSum >> 1U) + (UINT32)pTbl[Idx];
#endif
    }

    return ChkSum;
}
#endif

static INT32 CreateUpcaseTable(const AMBA_FS_EXFAT_BS *pBs,
                               AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                               UINT8 *pExtraBuf)
{
    UINT32 i, RootSec, secs;
    INT32 Rval;
    UINT32 *pTbl;
    UINT32 TmpPto;
#if 0
    UINT32  ChkSum  = TableCheckSum((UINT8 *)UpCaseTable, sizeof(UpCaseTable));
    K_ASSERT(ChkSum == UPCASE_TBL_CHKSUM);
#endif

    secs = (sizeof(UpCaseTable) / SEC_SIZE) + 1U;

    /* Update FAT table */
    AmbaMisra_TypeCast32(&TmpPto, &pBs->Pto);
    Rval = RdSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1U);
    if (Rval < 0) {
        Rval = -1;
    } else {
        AmbaMisra_TypeCast(&pTbl, &pExtraBuf);
        for (i = EXFAT_SRT_CLUSTER; i < (SEC_SIZE / sizeof(UINT32)); i++) {
            if (pTbl[i] == 0U) {
                pTbl[i] = 0xffffffffU;
                break;
            }
        }

        Rval = WrSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1U);
        if (Rval < 0) {
            Rval = -1;
        } else {
            const UINT16 *pUpTbl = UpCaseTable;
            UINT8 *pTmpPtr = NULL;
            /* Update upcase table */
            RootSec = 1U;
            RootSec = TmpPto + pBs->Cho + ((i - EXFAT_SRT_CLUSTER) * (RootSec << pBs->Spc));
            AmbaMisra_TypeCast(&pTmpPtr, &pUpTbl);
            Rval = WrSecFunc(pTmpPtr, RootSec, secs);
        }
    }
    return Rval;
}

static INT32 CreateRootDirectort(const AMBA_FS_EXFAT_BS *pBs,
                                 AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                                 UINT8 *pExtraBuf, UINT32 ExtraSize)
{
    UINT32 *pTbl;
    AMBA_FS_EXFAT_BITMAP_ENTRY *pBmp;
    AMBA_FS_EXFAT_UPTBL_ENTRY *pUpTbl;
    INT32 Rval;
    UINT32 i = 0, RootSec = 0, Spc, j, n, ExtraBlks;
    UINT32 TmpPto;

    ExtraBlks = ExtraSize >> SEC_SHIFT;
    Spc = 1U;
    Spc = (Spc << pBs->Spc);

    AmbaMisra_TypeCast32(&TmpPto, &pBs->Pto);

    /* Update FAT table */
    Rval = RdSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1U);
    if (Rval < 0) {
        Rval = -1;
    }

    if (Rval != -1) {
        AmbaMisra_TypeCast(&pTbl, &pExtraBuf);
        for (i = EXFAT_SRT_CLUSTER; i <= (SEC_SIZE / sizeof(UINT32)); i++) {
            if (pTbl[i] == 0U) {
                pTbl[i] = 0xffffffffU;
                break;
            }
        }
    }

    if (Rval != -1) {
        Rval = WrSecFunc(pExtraBuf, TmpPto + pBs->Fto, 1U);
        if (Rval < 0) {
            Rval = -1;
        } else {
            /* Clear Root Dir */
            if (OK != AmbaWrap_memset(pExtraBuf, 0x0, ExtraSize)) { /* Misrac */ };
            RootSec = 1U;
            RootSec = TmpPto + pBs->Cho + ((i - EXFAT_SRT_CLUSTER) * (RootSec << pBs->Spc));
            for (j = 0; j < Spc; j += n) {
                if ((Spc - i) < ExtraBlks) {
                    n = (Spc - i);
                } else {
                    n = ExtraBlks;
                }

                Rval = WrSecFunc(pExtraBuf, RootSec + j, n);
                if (Rval < 0) {
                    Rval = -1;
                    break;
                }
            }
        }
    }

    if (Rval != -1) {
        /* Update Bitmap */
        AmbaMisra_TypeCast(&pTbl, &pExtraBuf);
        pTbl[0] = 0x7U;

        Rval = WrSecFunc(pExtraBuf, TmpPto + pBs->Cho, 1U);
        if (Rval < 0) {
            Rval = -1;
        } else {
            /* Create Root Dir */
            if (OK != AmbaWrap_memset(pExtraBuf, 0x0, SEC_SIZE)) { /* Misrac */ };
            AmbaMisra_TypeCast(&pBmp, &pExtraBuf);//pBmp = (AMBA_FS_EXFAT_BITMAP_ENTRY *) pExtraBuf;
            pBmp[0].Type = BMP_ENTRY_TYPE;
            pBmp[0].FirstCluster = EXFAT_SRT_CLUSTER;
            pBmp[0].Length = (UINT64)pBs->Cnt / 8U;
            if ((pBs->Cnt % 8U) != 0U) {
                pBmp[0].Length += 1U;
            }

            AmbaMisra_TypeCast(&pUpTbl, &pExtraBuf);//pUpTbl = (AMBA_FS_EXFAT_UPTBL_ENTRY *) pExtraBuf;
            pUpTbl[1].Type = UPTBL_ENTRY_TYPE;
            pUpTbl[1].ChkSum = UPCASE_TBL_CHKSUM;
            pUpTbl[1].FirstCluster = (i - EXFAT_SRT_CLUSTER) + 1U;
            pUpTbl[1].Length = UPCASE_TBL_LENGTH;

            Rval = WrSecFunc(pExtraBuf, RootSec, 1U);
        }
    }
    return Rval;
}

INT32 AmbaFS_CreateExfatEntry(AMBA_FS_EXFAT_BS *pBs,
                              AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                              UINT8 *pExtraBuf, UINT32 ExtraSize)
{
    INT32 Rval;

    /* bitmap */
    Rval = CreateAllocationBitmap(pBs, WrSecFunc, RdSecFunc, pExtraBuf, ExtraSize);
    if (Rval < 0) {
        Rval = -1;
    } else {
        /* Up-case Table */
        Rval = CreateUpcaseTable(pBs, WrSecFunc, RdSecFunc, pExtraBuf);
        if (Rval < 0) {
            Rval = -1;
        } else {
            /* Root entry */
            Rval = CreateRootDirectort(pBs, WrSecFunc, RdSecFunc, pExtraBuf, ExtraSize);
        }
    }

    return Rval;
}

/**
 * Get Sectors per fat
 *
 * @FatType param - fat Type
 * @TotalSector param - total Sectors of the logic partition
 * @Nbs     param - number of boot Sectors
 * @Nrs     param - number of root Sectors
 * @Spc     param - Sector per culster
 * @returns - number of Sectors per fat
 */
UINT32 AmbaFS_GetSectorsPerFat(UINT8 FatType, UINT32 TotalSector, UINT8 Nbs, UINT8 Nrs, UINT8 Spc)
{
    UINT32 DecClust, IncSect, Shift;
    UINT32 NumFatSect, NumClust;

    NumClust = (TotalSector - Nbs) - Nrs;
    NumFatSect = (NumClust % Spc) / 2U;
    NumClust /= Spc;

    /* fat entry + next root link */
    if (FatType == 32U) {
        NumClust += 3U;
    } else {
        NumClust += 2U;
    }

    /* 2 fat */
    if (Spc > 1U) {
        IncSect = (UINT32)Spc / 2U;
        DecClust = 1U;
    } else {
        IncSect = 1U;
        DecClust = 2U;
    }

    /** fat12 entries = nFatSect x 512 / 1.5 = nFatSect x 512 x 2 / 3
     *  fat32 entries = (nFatSect * 512 / 4)
     *  fat16 entries = (nFatSect * 512 / 2)
     */
    if (FatType == 12U) {
        while ( ((NumFatSect << 10U) / 3U) < NumClust ) {
            NumFatSect += IncSect;
            NumClust -= DecClust;
        }
    } else {
        if (FatType == 32U) {
            Shift = 7U;
        } else {
            Shift = 8U;
        }
        while( (NumFatSect << Shift) < NumClust ) {
            NumFatSect += IncSect;
            NumClust -= DecClust;
        }
    }

    return NumFatSect;
}

/**
 * Get Sectors per fat for nand flash. This would garateen storing area locate
 * to block boundry.
 *
 * @FatType param - fat Type
 * @TotalClust  param - total clusters of the logic partition
 * @Spc     param - Sector per culster
 */
UINT32 AmbaFS_GetFlashSectorsPerFat(UINT16 FatType, UINT32 TotalClust, UINT16 Spc)
{
    UINT32 Shift, NumClust;
    UINT32 NumFatSect;

    /** fat12 entries = nFatSect x 512 / 1.5 = nFatSect x 512 x 2 / 3
     *  fat32 entries = (nFatSect * 512 / 4)
     *  fat16 entries = (nFatSect * 512 / 2)
     */
    NumFatSect = 1U;
    NumClust = TotalClust;
    if (FatType == 12U) {
        while ( ((NumFatSect << 10U) / 3U) < NumClust ) {
            NumFatSect += 1U;
            if ((NumFatSect % Spc) == 0U) {
                NumClust = TotalClust - (NumFatSect / Spc);
            }
        }
    } else {
        if (FatType == 32U) {
            Shift = 7U;
        } else {
            Shift = 8U;
        }
        if (FatType == 32U) {
            Shift = 7U;
        } else {
            Shift = 8U;
        }
        while( (NumFatSect << Shift) < NumClust ) {
            NumFatSect += 1U;
            if ((NumFatSect % Spc) == 0U) {
                NumClust = TotalClust - (NumFatSect / Spc);
            }
        }
    }

    return NumFatSect;
}

#define CALC_RSC 0U
#define CALC_SSA 1U
#define CALC_MAX 2U

static UINT32 calc_RscSsaMax(UINT32 SecCnt, AMBA_FS_DRIVE_INFO *pDrvInfo, UINT32 BoundUnit, UINT32 *pNom, UINT32 *pRsc, UINT32 Step)
{
    UINT32 Ssa = 0;
    UINT32 n, SpfTemp = 0;

    if (Step == CALC_RSC) {
        /* Reserved Sector Count(RSC) */
        n = 1U;
        if (pDrvInfo->FatType == FAT32_TYPE) {
            while ((pDrvInfo->Spf * 2U) >= (BoundUnit * n)) {
                n++;
            }

            (*pRsc) = (BoundUnit * n) - (2U * pDrvInfo->Spf);
            if ((*pRsc) < 9U) {
                (*pRsc) += BoundUnit;
            }
        } else {
            (*pRsc) = 1U;
        }
        if (OK != AmbaWrap_memcpy(&pDrvInfo->BsSizeCustom, pRsc, sizeof(pDrvInfo->BsSizeCustom))) { /* Misrac */ };
    }

    if (Step <= CALC_SSA) {
        /* Number of Sectors in the system area(SSA) */
        if (pDrvInfo->FatType == FAT32_TYPE) {
            Ssa = pDrvInfo->BsSizeCustom + (2U * pDrvInfo->Spf);
        } else {
            Ssa = 1U + (2U * pDrvInfo->Spf) + 32U;
        }

        /* Number of Sectors in Master Boot Area(NOM) */
        n = 1U;
        while ((BoundUnit * n) < Ssa) {
            n++;
        }
        (*pNom) = (BoundUnit * n) - Ssa;
        if ((*pNom) != BoundUnit) {
            (*pNom) += BoundUnit;
        }
    }

    if (Step <= CALC_MAX) {
        UINT32 Carry, Max;
        /* Maximum Cluster Number(MAX) Fat12/16 step 11, Fat32 step 10 */
        Max = (((SecCnt - (*pNom)) - Ssa) / pDrvInfo->Spc) + 1U;

        /* Calc SPF'(SpfTemp) */
        Carry = ((2U + (Max - 1U)) * pDrvInfo->FatType);
        Carry &= ((SEC_SIZE * 8U) - 1U);
        SpfTemp = ((2U + (Max - 1U)) * pDrvInfo->FatType) / (SEC_SIZE * 8U);
        if (Carry != 0U) {
            SpfTemp++;
        }
    }
    return SpfTemp;
}

static void AmbaFS_ParseSdaFormatParamFAT(UINT32 SecCnt, AMBA_FS_DRIVE_INFO *pDrvInfo, UINT32 BoundUnit)
{
    UINT32 Carry, Nom = 0, Rsc = 0;
    UINT32 SpfTemp;

    /* Number of Root-directory Entries */
    if ((pDrvInfo->FatType == FAT16_TYPE) ||
        (pDrvInfo->FatType == FAT12_TYPE)) {
        pDrvInfo->RdeCustom = 512U;
    }

    /* Sector per FAT(SF) */
    Carry = (SecCnt * pDrvInfo->FatType) &
            ((pDrvInfo->Spc * (SEC_SIZE * 8U)) - 1U);
    pDrvInfo->Spf = (SecCnt * pDrvInfo->FatType) /
                    (pDrvInfo->Spc * (SEC_SIZE * 8U));

    /* take ceiling */
    if (Carry != 0U) {
        pDrvInfo->Spf += 1U;
    }

    SpfTemp = calc_RscSsaMax(SecCnt, pDrvInfo, BoundUnit, &Nom, &Rsc, CALC_RSC);

    while (SpfTemp != pDrvInfo->Spf) {
        if (SpfTemp > pDrvInfo->Spf) {
            if (pDrvInfo->FatType == FAT32_TYPE) {
                Nom += BoundUnit;
                Rsc += BoundUnit;
                if (OK != AmbaWrap_memcpy(&pDrvInfo->BsSizeCustom, &Rsc, sizeof(pDrvInfo->BsSizeCustom))) { /* Misrac */ }; //pDrvInfo->BsSizeCustom = Rsc;
            } else {
                Nom += BoundUnit;
            }
            SpfTemp = calc_RscSsaMax(SecCnt, pDrvInfo, BoundUnit, &Nom, &Rsc, CALC_MAX);
        } else {
            if (pDrvInfo->FatType == FAT32_TYPE) {
                pDrvInfo->Spf -= 1U;
                SpfTemp = calc_RscSsaMax(SecCnt, pDrvInfo, BoundUnit, &Nom, &Rsc, CALC_RSC);
            } else {
                pDrvInfo->Spf = SpfTemp;
                SpfTemp = calc_RscSsaMax(SecCnt, pDrvInfo, BoundUnit, &Nom, &Rsc, CALC_SSA);
            }
        }
    }
    pDrvInfo->StartLba = Nom;
    pDrvInfo->TotalSector = (pDrvInfo->EndLba - pDrvInfo->StartLba) + 1U;
    /* done here */
}

static void DetermineCHS(UINT32 TotalSizeMb, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    /* Determine CHS */
    if (TotalSizeMb <= 2U) {     /* ~   2MB */
        pDrvInfo->NumOfHead = 2U;
        pDrvInfo->Spt   = 16U;
    } else if (TotalSizeMb <= 16U) { /* ~  16MB */
        pDrvInfo->NumOfHead = 2U;
        pDrvInfo->Spt   = 32U;
    } else if (TotalSizeMb <= 32U) { /* ~  32MB */
        pDrvInfo->NumOfHead = 4U;
        pDrvInfo->Spt   = 32U;
    } else if (TotalSizeMb <= 128U) {    /* ~ 128MB */
        pDrvInfo->NumOfHead = 8U;
        pDrvInfo->Spt   = 32U;
    } else if (TotalSizeMb <= 256U) {    /* ~ 256MB */
        pDrvInfo->NumOfHead = 16U;
        pDrvInfo->Spt   = 32U;
    } else if (TotalSizeMb <= 504U) {    /* ~ 504MB */
        pDrvInfo->NumOfHead = 16U;
        pDrvInfo->Spt   = 63U;
    } else if (TotalSizeMb <= 1008U) {   /* ~1008MB */
        pDrvInfo->NumOfHead = 32U;
        pDrvInfo->Spt   = 63U;
    } else if (TotalSizeMb <= 2016U) {   /* ~2016MB */
        pDrvInfo->NumOfHead = 64U;
        pDrvInfo->Spt   = 63U;
    } else if (TotalSizeMb <= 4032U) {
        pDrvInfo->NumOfHead = 128U;
        pDrvInfo->Spt   = 63U;
    } else {
        /* ~2TB */
        pDrvInfo->NumOfHead = 255U;
        pDrvInfo->Spt   = 63U;
    }
}

static void AmbaFS_DeterminationParam(UINT32 TotalSizeMb, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    if(TotalSizeMb < 16U) {
        pDrvInfo->Spc = 16U;
        pDrvInfo->FatId = FS_FAT12;
        pDrvInfo->FatType = FAT12_TYPE;
    } else if (TotalSizeMb < 64U) {
        pDrvInfo->Spc = 32U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 128U) {
        pDrvInfo->Spc = 32U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 256U) {
        pDrvInfo->Spc = 32U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 512U) {
        pDrvInfo->Spc = 32U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 1024U) {
        pDrvInfo->Spc = 32U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 2048U) {
        pDrvInfo->Spc = 64U;
        pDrvInfo->FatId = FS_FAT16;
        pDrvInfo->FatType = FAT16_TYPE;
    } else if (TotalSizeMb < 32896U) {
        /* over 2048 ~ 32768MB */
        pDrvInfo->Spc = 64U;
        pDrvInfo->FatId = FS_FAT32;
        pDrvInfo->FatType = FAT32_TYPE;
    } else if (TotalSizeMb < 131072U) {
        /* 32896 ~ 128GB */
#ifdef CONFIG_ENABLE_EXFAT_SUPPORT
        pDrvInfo->Spc = 256U;
        pDrvInfo->FatId = FS_EXFAT;
        pDrvInfo->FatType = EXFAT_TYPE;
#else
        pDrvInfo->Spc = 128U;
        pDrvInfo->FatId = FS_FAT32;
        pDrvInfo->FatType = FAT32_TYPE;
#endif
    } else if (TotalSizeMb < 524288U) {
        /* ~ 512GB */
#ifdef CONFIG_ENABLE_EXFAT_SUPPORT
        pDrvInfo->Spc = 512U;
        pDrvInfo->FatId = FS_EXFAT;
        pDrvInfo->FatType = EXFAT_TYPE;
#else
        pDrvInfo->Spc = 128U;
        pDrvInfo->FatId = FS_FAT32;
        pDrvInfo->FatType = FAT32_TYPE;
#endif
    } else {
        /* ~ 2TB */
#ifdef CONFIG_ENABLE_EXFAT_SUPPORT
        pDrvInfo->Spc = 1024U;
        pDrvInfo->FatId = FS_EXFAT;
        pDrvInfo->FatType = EXFAT_TYPE;
#else
        pDrvInfo->Spc = 128U;
        pDrvInfo->FatId = FS_FAT32;
        pDrvInfo->FatType = FAT32_TYPE;
#endif
    }
}

/**
 * format parameter using SDA standard.
 *
 */
void AmbaFS_ParseSdaFormatParam(UINT32 SecCnt, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    UINT32 BoundUnit = 0;
    UINT32 TotalSizeMb = SecCnt >> 11U;

    /* Determine CHS */
    DetermineCHS(TotalSizeMb, pDrvInfo);

    /* Modify default Sector-per-cluster value for SDHC FAT Format. */
    AmbaFS_DeterminationParam(TotalSizeMb, pDrvInfo);

    //    pDrvInfo->NumCyls = (SecCnt / pDrvInfo->NumOfHead) / (UINT32)pDrvInfo->Spt;

    pDrvInfo->EndLba = SecCnt - 1U;

    /* Boundary Unit */
    if (TotalSizeMb < 8U) {
        BoundUnit = 16U;
    } else if (TotalSizeMb < 64U) {
        BoundUnit = 32U;
    } else if (TotalSizeMb < 256U) {
        BoundUnit = 64U;
    } else if (TotalSizeMb < 1024U) {
        BoundUnit = 128U;
    } else if (TotalSizeMb < 2048U) {
        BoundUnit = 128U;
    } else if (TotalSizeMb < 32896U) {
        /* over 2048MB */
        BoundUnit = 8192U;
        //Nom = BoundUnit;
        pDrvInfo->StartLba = BoundUnit;
    } else if (TotalSizeMb < 131072U) {
        /* ~ 128GB */
        pDrvInfo->StartLba = 32768U;
#ifndef CONFIG_ENABLE_EXFAT_SUPPORT
        BoundUnit = pDrvInfo->StartLba;
#endif
    } else if (TotalSizeMb < 524288U) {
        /* ~ 512GB */
        pDrvInfo->StartLba = 65536U;
#ifndef CONFIG_ENABLE_EXFAT_SUPPORT
        BoundUnit = pDrvInfo->StartLba;
#endif
    } else {
        /* ~ 2TB */
        pDrvInfo->StartLba = 131072U;
#ifndef CONFIG_ENABLE_EXFAT_SUPPORT
        BoundUnit = pDrvInfo->StartLba;
#endif
    }

    /* Calc EXFAT parameter during bs create */
    if (pDrvInfo->FatType == EXFAT_TYPE) {
        UINT32 SpfTmp = pDrvInfo->StartLba / 2U;
        if (OK != AmbaWrap_memcpy(&pDrvInfo->Spf, &SpfTmp, sizeof(pDrvInfo->Spf))) { /* Misrac */ };
        pDrvInfo->TotalSector = (pDrvInfo->EndLba - pDrvInfo->StartLba) + 1U;
        /* done EXFAT */
#ifndef CONFIG_ENABLE_EXFAT_SUPPORT
    } else if ((pDrvInfo->FatType == FAT32_TYPE) && (TotalSizeMb > 32896U)) {
        /* Force to use FAT32 on over 32GB device when disable EXFAT. */
        pDrvInfo->Spf = GetRoundUpValU32(SecCnt, pDrvInfo->Spc * (SEC_SIZE * 8U)) * pDrvInfo->FatType;
        pDrvInfo->TotalSector = (pDrvInfo->EndLba - pDrvInfo->StartLba) + 1U;
#endif
    } else {
        AmbaFS_ParseSdaFormatParamFAT(SecCnt, pDrvInfo, BoundUnit);
    }
}

static UINT8 AmbaFS_ParseFatId(UINT32 DevSize)
{
    UINT8 FatId;

    if (DevSize < 4U) {          /* < 4M => FAT12 */
        FatId = FS_FAT12;
    } else if (DevSize < 32U) {      /* < 32M => FAT16S */
        FatId = FS_FAT16S;
    } else if (DevSize < 256U) {     /* < 256M => FAT16 */
        FatId = FS_FAT16;
    } else if (DevSize < 32768U) {
        FatId = FS_FAT32;
    } else {
        FatId = FS_EXFAT;
    }

    return FatId;
}

static void AmbaFS_ParseFatType(AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    if (pDrvInfo->FatId == FS_FAT12) {
        pDrvInfo->FatType = FAT12_TYPE;
    } else if ((pDrvInfo->FatId == FS_FAT16) || (pDrvInfo->FatId == FS_FAT16S)) {
        pDrvInfo->FatType = FAT16_TYPE;
    } else if ((pDrvInfo->FatId == FS_FAT32) || (pDrvInfo->FatId == FS_FAT32L)) {
        pDrvInfo->FatType = FAT32_TYPE;
    }  else {
        pDrvInfo->FatType = FAT32_TYPE;
    }

    if (pDrvInfo->FatId == FS_EXFAT) {
        pDrvInfo->FatType = EXFAT_TYPE;
    }
}

static void AmbaFS_ParseExfatSpc(UINT32 DevSize, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    if (DevSize <  32768U) {
        pDrvInfo->Spc = 64U;
    } else if (DevSize < 131072U)  {
        pDrvInfo->Spc = 256U;
    } else if (DevSize < 524288U) {
        pDrvInfo->Spc = 512U;
    }  else {
        pDrvInfo->Spc = 1024U;
    }
}

static INT32 AmbaFS_ParseSpc(UINT32 DevSize, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    INT32 Rval = 0;

    if (pDrvInfo->FatType == EXFAT_TYPE) {
        AmbaFS_ParseExfatSpc(DevSize, pDrvInfo);

    } else if (pDrvInfo->FatType == FAT32_TYPE) {
        if (DevSize < 260U) {
            pDrvInfo->Spc = 1U;
        } else if (DevSize < 8192U)  {
            pDrvInfo->Spc = 8U;
        } else if (DevSize < 16384U) {
            pDrvInfo->Spc = 16U;
        }  else if (DevSize < 32768U) {
            pDrvInfo->Spc = 32U;
        } else {
            pDrvInfo->Spc = 64U;
        }

    } else if (pDrvInfo->FatType == FAT16_TYPE) {
        if (DevSize < 16U) {
            pDrvInfo->Spc = 2U;
        } else if (DevSize < 128U)  {
            pDrvInfo->Spc = 4U;
        } else if (DevSize < 256U) {
            pDrvInfo->Spc = 8U;
        }  else if (DevSize < 512U) {
            pDrvInfo->Spc = 16U;
        }  else if (DevSize < 1024U) {
            pDrvInfo->Spc = 32U;
        }  else if (DevSize < 2048U) {
            pDrvInfo->Spc = 64U;
        } else {
            pDrvInfo->Spc = 128U;
        }
    } else if (pDrvInfo->FatType == FAT12_TYPE) {
        pDrvInfo->Spc = 2U;
    } else {
        Rval = -1;
    }
    return Rval;
}

static UINT32 AmbaFS_ParseMaxSpc(UINT32 DevSize, const AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    UINT32 MaxSpc = 0;

    if (pDrvInfo->FatType == FAT32_TYPE) {
        if (DevSize <= 32U) {
            MaxSpc = 0U;
        } else if (DevSize <= 64U)  {
            MaxSpc = 1U ;
        } else if (DevSize <= 128U) {
            MaxSpc = 2U;
        }  else if (DevSize <= 256U) {
            MaxSpc = 4U;
        }  else if (DevSize <= 512U) {
            MaxSpc = 8U;
        }  else if (DevSize <= 1024U) {
            MaxSpc = 16U;
        }  else if (DevSize <= 2048U) {
            MaxSpc = 32U;
        }  else if (DevSize <= 4096U) {
            MaxSpc = 64U;
        } else {
            MaxSpc = 128U;
        }
    } else if (pDrvInfo->FatType == FAT16_TYPE) {
        MaxSpc = 128U;
    } else if (pDrvInfo->FatType == FAT12_TYPE) {
        MaxSpc = 8U;
    } else if (pDrvInfo->FatType == EXFAT_TYPE) {
        MaxSpc = 1024U;
    } else {
        /* For MisraC checking */
    }

    return MaxSpc;
}

/**
 * Parse format parameter.
 *
 * Ex
 *  token,token1=value1,token2=value2
 */
INT32 AmbaFS_ParseFormatParam(UINT32 DevSize, AMBA_FS_DRIVE_INFO *pDrvInfo)
{
    UINT32 MaxCap = 0, MaxSpc = 0;
    INT32 Rval;

    pDrvInfo->FatId     = 0xffU;
    pDrvInfo->Spc       = 0xffffU;
    pDrvInfo->StartLbaCustom = 0;
    pDrvInfo->BsSizeCustom = 0;

    pDrvInfo->FatId = AmbaFS_ParseFatId(DevSize);

    AmbaFS_ParseFatType(pDrvInfo);

    Rval = AmbaFS_ParseSpc(DevSize, pDrvInfo);

    if (Rval != -1) {
        /* Max support capacity in MB. */
        MaxCap = 0x1U;
        MaxCap = (MaxCap << (pDrvInfo->FatType - 11U)) * pDrvInfo->Spc;
        if ((DevSize > MaxCap) && (pDrvInfo->FatType != EXFAT_TYPE)) {
            //DBG_MSG("Can't support this Format for this disk size!");
            Rval = -1;
        } else {

            MaxSpc = AmbaFS_ParseMaxSpc(DevSize, pDrvInfo);

            if (pDrvInfo->Spc > MaxSpc) {
                Rval = -1;
            }
        }
    }
    return Rval;
}

