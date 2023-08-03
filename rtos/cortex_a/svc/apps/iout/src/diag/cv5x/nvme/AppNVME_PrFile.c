/**
 *  @file AppNVME_PrFile.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PrFile-related functions for NVMe
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaFS.h>
#include <prfile2/pdm_driver.h>
#include <prfile2/pdm_api.h>

#include "AppNVME.h"
#include "AppNVME_Utility.h"
#include "AppNVME_PrFile.h"
#include "AmbaMisraFix.h"
#include <AmbaDrvEntry.h>

#define NVME_ATTR_ALIGN_CACHE_NO_INIT __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")))
#define NVME_ATTR_ALIGN_CACHE __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE)))
#define NVME_ATTR_NO_INIT __attribute__((section(".bss.noinit")))

#ifndef NVMEPRF_FAT_PAGES
#define NVMEPRF_FAT_PAGES     64
#endif
#ifndef NVMEPRF_DATA_PAGES
#define NVMEPRF_DATA_PAGES    32
#endif
#ifndef NVMEPRF_FAT_BUF_SIZE
#define NVMEPRF_FAT_BUF_SIZE  4
#endif
#ifndef NVMEPRF_DATA_BUF_SIZE
#define NVMEPRF_DATA_BUF_SIZE 8
#endif

// Force to use the 1st instance in development.
#define NVME_INSTANCE         1U

#define NVME_SEC_SHIFT        9U
#define NVME_SEC_SIZE         512U

// Runtime variables required by PrFILE as well as internal ones.
typedef struct {
    PDM_FUNCTBL             DrvFunc;
    PDM_INIT_DISK           DrvInitTable;
    struct PDM_DISK *       pDiskTable;
    struct PDM_PARTITION *  pPartTable;
    PF_DRV_TBL *            pDrvTableEntry;
    PF_CACHE_SETTING        CacheSet;
    UINT8                   Mbr[512];                   // MBR
    UINT8                   Bs[512];                    // fat16 Boot Sector
} STORAGE_PRFILE2_OBJ_s;

static STORAGE_PRFILE2_OBJ_s nvme_prf2_obj NVME_ATTR_ALIGN_CACHE_NO_INIT;
static STORAGE_PRFILE2_OBJ_s *nvme_prf2_table[NVME_INSTANCE] NVME_ATTR_ALIGN_CACHE = { &nvme_prf2_obj };

static UINT32 nvme_check_slots(UINT32 Id)
{
    UINT32 uret;

    if (Id < NVME_INSTANCE) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

static UINT32 nvme_check_ssd_info(const NVME_INFO_s *Info)
{
    UINT32 uret;

    if (Info != NULL) {
        if (Info->SectorSize == 512U) {
            if (Info->WriteProtect == 0U) {
                uret = 1;
            } else {
                uret = 0;
#ifdef  NVME_PRINT_SUPPORT
                NVME_UtilityPrint("nvme_check_format: write protect!!");
#endif
            }
        } else {
            uret = 0;
#ifdef  NVME_PRINT_SUPPORT
            NVME_UtilityPrintUInt5("sector size error 0x%x", Info->SectorSize, 0, 0, 0, 0);
#endif
        }
    } else {
        uret = 0;
#ifdef  NVME_PRINT_SUPPORT
        NVME_UtilityPrint("get null usb info");
#endif
    }
    return uret;
}

static INT32 nvme_lun0_write(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 uret;

    //if (AppNVME_SsdWrite(pBuf, Sector, Sectors) != 0U) {
    if (AppNVME_Write(pBuf, Sector, Sectors) != 0U) {
        uret = -1;
    } else {
        uret = 0;
    }
    return uret;
}

static INT32 nvme_lun0_read(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 uret;

    //if (AppNVME_SsdRead(pBuf, Sector, Sectors) != 0U) {
    if (AppNVME_Read(pBuf, Sector, Sectors) != 0U) {
        uret = -1;
    } else {
        uret = 0;
    }
    return uret;
}

static PDM_ERROR nvme_prf2_format(UINT32 Id, PDM_DISK *pPdmDisk, const PDM_U_CHAR *pParam)
{
    static UINT8 format_buffer[32 * 512] NVME_ATTR_ALIGN_CACHE_NO_INIT;
    AMBA_FS_DRIVE_INFO fs_drive_info;
    UINT32 sector_count;

    INT32 (*WriteFunc)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    INT32 (*ReadFunc)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
#if PRF2_FORMAT_LAYER_SELECTABLE
    const AMBA_FS_FAT_BS32 *fat_bs32;
    const AMBA_FS_FAT_BS16 *fat_bs16;
    UINT32 sys_sector_count;
    UINT32 nsect;
    UINT32 sector_start;
    UINT8 *buffer_ptr;
    INT32  err_ret = DRV_SUCCEED;
    UINT32 flag_leave = 0;
#else
    INT32 err_ret;
    UINT32 Addr;
    INT32 secsize = 512;
    UINT8 buf[NVME_SEC_SIZE];
#endif
    const NVME_INFO_s *ssd_info;

    (void)pPdmDisk;
    (void)pParam;

    WriteFunc = nvme_lun0_write;
    ReadFunc  = nvme_lun0_read;

    NVME_UtilityMemorySet(&fs_drive_info, 0, sizeof(fs_drive_info));
    fs_drive_info.Drive = (UINT16)(Id & 0xFFFFU);

    //ssd_info = AppNVME_SsdInfoGet();
    ssd_info = AppNVME_Info();

    if (nvme_check_ssd_info(ssd_info) != 0U) {
        // Setup Drive infomation
        sector_count = ssd_info->Lba + 1U;

        AmbaFS_ParseSdaFormatParam(sector_count, &fs_drive_info);  // (todo) shall be handled by FS to pass the Vcast.

#if PRF2_FORMAT_LAYER_SELECTABLE
        nsect = 32;
        buffer_ptr  = format_buffer;

        // Calculate number of system Sectors
        AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
        if (fs_drive_info.FatType == (UINT16)EXFAT_TYPE) {
            sys_sector_count = fs_drive_info.StartLba * 2U;
        } else if (fs_drive_info.FatType == (UINT16)FAT32_TYPE) {
            fat_bs32      = NVME_UtilityP8ToFsBs32(buffer_ptr);
            sys_sector_count = fs_drive_info.StartLba + fat_bs32->Fno +
                               (fat_bs32->Spf32 * fat_bs32->Fts) + fs_drive_info.Spc;
        } else {
            UINT16 entry_count;
            fat_bs16      = NVME_UtilityP8ToFsBs16(buffer_ptr);
            entry_count   = fat_bs16->Ent >> 4U;
            sys_sector_count = fs_drive_info.StartLba + (UINT32)fat_bs16->Fno +
                               ((UINT32)fat_bs16->Spf * (UINT32)fat_bs16->Fts) + (UINT32)entry_count;
        }

        // Write all system Sectors as 0
        if (fs_drive_info.FatType == (UINT16)EXFAT_TYPE) {
            sector_start = fs_drive_info.StartLba + (fs_drive_info.StartLba / 2U);
        } else {
            sector_start = 0;
        }

        NVME_UtilityMemorySet(buffer_ptr, 0x0, nsect << NVME_SEC_SHIFT);
        while (sector_start < sys_sector_count) {
            if ((sys_sector_count - sector_start) < nsect) {
                sector_count = sys_sector_count - sector_start;
            } else {
                sector_count = nsect;
            }

            err_ret = WriteFunc(buffer_ptr, sector_start, sector_count);
            if (err_ret < 0) {
                flag_leave = 1;
                err_ret  = DRV_FAILED;
                break;
            }
            sector_start += sector_count;
        }

        if (flag_leave == 0U) {
            // Write Mbr
            AmbaFS_CreateMbr(&fs_drive_info, buffer_ptr);
            err_ret = WriteFunc(buffer_ptr, 0, 1);
            if (err_ret < 0) {
                err_ret = DRV_FAILED;
            } else {
                // Write boot Sector area for FAT16 or FAT32
                err_ret = AmbaFS_CreateBootArea(&fs_drive_info, WriteFunc);
                if (err_ret < 0) {
                    err_ret = DRV_FAILED;
                } else {
                    // Write FAT table
                    AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
                    err_ret = AmbaFS_CreateFat(&fs_drive_info, buffer_ptr, WriteFunc);
                    if (err_ret < 0) {
                        err_ret = DRV_FAILED;
                    } else {
                        if (fs_drive_info.FatType == (UINT16)EXFAT_TYPE) {
                            UINT8 *ExtraBuf = &buffer_ptr[(UINT32)4 << NVME_SEC_SHIFT];
                            UINT32 ExtraSize = (nsect - 4U) << NVME_SEC_SHIFT;
                            AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
                            // (todo) shall be handled by FS to pass Vcast.
                            err_ret = AmbaFS_CreateExfatEntry(NVME_UtilityP8ToFsFxfatBs(buffer_ptr), WriteFunc, ReadFunc,
                                                              ExtraBuf, ExtraSize);
                            if (err_ret < 0) {
                                err_ret = DRV_FAILED;
                            }
                        } else {
                            err_ret = DRV_SUCCEED;
                        }
                    }
                }
            }
        }
#else   // PRF2_FORMAT_LAYER_SELECTABLE

        // Create main boot record
        AmbaFS_CreateMbr(&fs_drive_info, buffer_ptr);
        err_ret = WriteFunc(buffer_ptr, 0, 1);
        if (err_ret < 0) {
            err_ret = DRV_FAILED;
        } else {
            // Create boot Sector
            AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
            err_ret = WriteFunc(buffer_ptr, fs_drive_info.StartLba, 1);
            if (err_ret < 0) {
                err_ret = DRV_FAILED;
            } else {
                if (fs_drive_info.FatType == (UINT16)FAT32_TYPE) {
                    // Write to backup bs
                    err_ret = WriteFunc(buffer_ptr, fs_drive_info.StartLba + 6, 1);
                    if (err_ret < 0) {
                        err_ret = DRV_FAILED;
                    }
                }
            }
        }
#endif  // PRF2_FORMAT_LAYER_SELECTABLE
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PF_ERROR nvme_prf2_init_drive_table(UINT32 Id, PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    STORAGE_PRFILE2_OBJ_s *nvme_prf2_obj;
    PF_ERROR err_ret;

    if (nvme_check_slots(Id) != 0U) {
        nvme_prf2_obj = nvme_prf2_table[Id];

        pDiskTable->p_func = (PDM_FUNCTBL*) &nvme_prf2_obj->DrvFunc;
        pDiskTable->ui_ext = UiExt;
        err_ret            = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_prf2_drive_init(UINT32 Id, PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    if (nvme_check_slots(Id) != 0U) {
        if (pPdmDisk == NULL) {
            err_ret = DRV_FAILED;
        } else {
            // If the media has been inserted, notifies PrFILE2
            pdm_disk_notify_media_insert(pPdmDisk);
            err_ret = DRV_SUCCEED;
        }
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_prf2_finalize(UINT32 Id, PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;
    const STORAGE_PRFILE2_OBJ_s *nvme_prf2_obj;

    if (nvme_check_slots(Id) != 0U) {
        nvme_prf2_obj = nvme_prf2_table[Id];

        if (pPdmDisk == NULL) {
            err_ret = DRV_FAILED;
        } else {
            // Termination process for the Driver stub
            // Termination process for the Driver and media controller
            pdm_disk_notify_media_eject(pPdmDisk);
            nvme_prf2_obj->pDrvTableEntry->drive = 0;
            nvme_prf2_obj->pDrvTableEntry->stat  = 0;
            err_ret                             = DRV_SUCCEED;
        }
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_prf2_mount(UINT32 Id, PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    (void)pPdmDisk;

    if (nvme_check_slots(Id) != 0U) {
        err_ret = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_prf2_unmount(UINT32 Id, PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    (void)pPdmDisk;

    if (nvme_check_slots(Id) != 0U) {
        err_ret = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_prf2_get_disk_info(PDM_DISK *pPdmDisk, PDM_DISK_INFO *pPdmDiskInfo)
{
    const NVME_INFO_s *ssd_info;
    UINT32 sector_count = 0;
    UINT32 total_size_in_mb = 0;
    UINT32 cylinders = 0;
    UINT8  head_number = 0;
    UINT8  sectors_per_track = 0;
    UINT32 prf2_attr = 0;
    PDM_ERROR err_ret;

    (void)pPdmDisk;

    //ssd_info = AppNVME_SsdInfoGet();
    ssd_info = AppNVME_Info();

    if (ssd_info != NULL) {
        if (ssd_info->SectorSize == 512U) {
            // Caculate the necessary inFormation
            sector_count = ssd_info->Lba + 1U;
            total_size_in_mb = sector_count >> 11U;

            // Determine CHS
            if (total_size_in_mb <= 2U) {            // ~   2MB
                head_number = 2;
                sectors_per_track       = 16;
            } else if (total_size_in_mb <= 16U) {    // ~  16MB
                head_number = 2;
                sectors_per_track       = 32;
            } else if (total_size_in_mb <= 32U) {    // ~  32MB
                head_number = 4;
                sectors_per_track       = 32;
            } else if (total_size_in_mb <= 128U) {   // ~ 128MB
                head_number = 8;
                sectors_per_track       = 32;
            } else if (total_size_in_mb <= 256U) {   // ~ 256MB
                head_number = 16;
                sectors_per_track       = 32;
            } else if (total_size_in_mb <= 504U) {   // ~ 504MB
                head_number = 16;
                sectors_per_track       = 63;
            } else if (total_size_in_mb <= 1008U) {  // ~1008MB
                head_number = 32;
                sectors_per_track       = 63;
            } else if (total_size_in_mb <= 2016U) {  // ~2016MB */
                head_number = 64;
                sectors_per_track       = 63;
            } else {
                head_number = 128;
                sectors_per_track       = 63;
            }

            cylinders = (sector_count / head_number) / sectors_per_track;

            if (ssd_info->WriteProtect != 0U) {
                prf2_attr = PRF2_MEDIA_ATTR | PDM_MEDIA_ATTR_WRITE_PROTECT;
            } else {
                prf2_attr = PRF2_MEDIA_ATTR;
            }

            pPdmDiskInfo->total_sectors     = sector_count;
            pPdmDiskInfo->cylinders         = (UINT16)cylinders;
            pPdmDiskInfo->heads             = head_number;
            pPdmDiskInfo->sectors_per_track = sectors_per_track;
            pPdmDiskInfo->bytes_per_sector  = NVME_SEC_SIZE;
            pPdmDiskInfo->media_attr        = prf2_attr;
            pPdmDiskInfo->format_param      = NULL;

            err_ret = DRV_SUCCEED;
        } else {
            err_ret = DRV_FAILED;
        }
    } else {
#ifdef  NVME_PRINT_SUPPORT
        NVME_UtilityPrint("get status fail");
#endif
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_wrapper_init_drive_table(PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    PDM_ERROR ret = nvme_prf2_init_drive_table(NVME_STORAGE_0, pDiskTable, UiExt);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_drive_init(PDM_DISK *pDisk)
{
    PDM_ERROR ret = nvme_prf2_drive_init(NVME_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_finalize(PDM_DISK *pDisk)
{
    PDM_ERROR ret = nvme_prf2_finalize(NVME_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_mount(PDM_DISK *pDisk)
{
    PDM_ERROR ret = nvme_prf2_mount(NVME_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_unmount(PDM_DISK *pDisk)
{
    PDM_ERROR ret = nvme_prf2_unmount(NVME_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_format(PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    PDM_ERROR ret = nvme_prf2_format(NVME_STORAGE_0, pDisk, pParam);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR nvme_wrapper_read(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                   PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                   PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR err_ret;

    (void)pDisk;

    //if (AppNVME_SsdRead(pBuf, Sector, Sectors) == 0U) {
    if (AppNVME_Read(pBuf, Sector, Sectors) == 0U) {
        *pNumSuccess = Sectors;
        err_ret      = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        err_ret      = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_wrapper_write(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                    PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                    PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR err_ret;
    UINT8 *Ptr;

    (void)pDisk;

    Ptr = NVME_UtilityConstP8ToP8(pBuf);

    //if (AppNVME_SsdWrite(Ptr, Sector, Sectors) == 0U) {
    if (AppNVME_Write(Ptr, Sector, Sectors) == 0U) {
        *pNumSuccess = Sectors;
        err_ret      = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        err_ret      = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR nvme_wrapper_get_disk_info(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    PDM_ERROR ret = nvme_prf2_get_disk_info(pDisk, pDiskInfo);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static UINT32 nvme_fs_init(void *pDrive)
{
    static PF_CACHE_PAGE nvme_cache_page[NVMEPRF_FAT_PAGES + NVMEPRF_DATA_PAGES] NVME_ATTR_ALIGN_CACHE_NO_INIT;
    static PF_SECTOR_BUF nvme_cache_buffer[NVMEPRF_FAT_PAGES + NVMEPRF_DATA_PAGES] NVME_ATTR_ALIGN_CACHE_NO_INIT;

    STORAGE_PRFILE2_OBJ_s *nvme_prf2_obj;
    PF_DRV_TBL *drive = NVME_UtilityVoidP2PrFDrvTbl(pDrive);

    // aviod misra-c 'const' issue
    AmbaMisra_TouchUnused(pDrive);

    // Force to use the 1st instance in development.
    nvme_prf2_obj = nvme_prf2_table[0];

    NVME_UtilityMemorySet(nvme_prf2_obj, 0x0, sizeof(*nvme_prf2_obj));

    nvme_prf2_obj->pDrvTableEntry = drive;

    nvme_prf2_obj->pDrvTableEntry->drive  = 0x6E; // slot n
    nvme_prf2_obj->DrvFunc.init           = nvme_wrapper_drive_init;
    nvme_prf2_obj->DrvFunc.finalize       = nvme_wrapper_finalize;
    nvme_prf2_obj->DrvFunc.mount          = nvme_wrapper_mount;
    nvme_prf2_obj->DrvFunc.unmount        = nvme_wrapper_unmount;
    nvme_prf2_obj->DrvFunc.format         = nvme_wrapper_format;
    nvme_prf2_obj->DrvFunc.physical_read  = nvme_wrapper_read;
    nvme_prf2_obj->DrvFunc.physical_write = nvme_wrapper_write;
    nvme_prf2_obj->DrvFunc.get_disk_info  = nvme_wrapper_get_disk_info;

    nvme_prf2_obj->DrvInitTable.p_func = nvme_wrapper_init_drive_table;

    nvme_prf2_obj->DrvInitTable.ui_ext = 0;

    (void)pdm_open_disk(&nvme_prf2_obj->DrvInitTable, &nvme_prf2_obj->pDiskTable);

    // Open a partition
    (void)pdm_open_partition(nvme_prf2_obj->pDiskTable, 0, &nvme_prf2_obj->pPartTable);

    // Initialize Driver table
    AmbaMisra_TypeCast32(&nvme_prf2_obj->pDrvTableEntry->p_part, &nvme_prf2_obj->pPartTable);
    nvme_prf2_obj->CacheSet.pages             = (PF_CACHE_PAGE *)nvme_cache_page;
    nvme_prf2_obj->CacheSet.buffers           = (PF_SECTOR_BUF *)nvme_cache_buffer;
    nvme_prf2_obj->CacheSet.num_fat_pages     = NVMEPRF_FAT_PAGES;
    nvme_prf2_obj->CacheSet.num_data_pages    = NVMEPRF_DATA_PAGES;
    nvme_prf2_obj->CacheSet.num_fat_buf_size  = NVMEPRF_FAT_BUF_SIZE;
    nvme_prf2_obj->CacheSet.num_data_buf_size = NVMEPRF_DATA_BUF_SIZE;
    nvme_prf2_obj->pDrvTableEntry->cache      = &nvme_prf2_obj->CacheSet;

    return NVME_INSTANCE;
}

/**
 * Called by AmbaDrvEntry during initialization.
*/
UINT32 AmbaPCIE_DrvEntry(void)
{
    // register USB drive function into file system
    UINT32 uret = AmbaFS_RegisterExtDrive(nvme_fs_init);
    if (uret != 0U) {
        // action TBD
    }
    return uret;
}
/** @} */
