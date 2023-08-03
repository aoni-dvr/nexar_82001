/**
 *  @file AmbaUSBH_Msc_PrFile.c
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
 *  @details PrFile-related functions for Mass Storage Host Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSBH_Msc.h>
#include <AmbaUSBH_Msc_PrFile.h>

#include <AmbaFS.h>
#include <prfile2/pdm_driver.h>
#include <prfile2/pdm_api.h>
//#include <prfile2/dskmng/pdm_dskmng.h>

#include <AmbaUSB_Utility.h>
#include <AmbaDrvEntry.h>

#ifndef USBPRF_FAT_PAGES
#define USBPRF_FAT_PAGES     64
#endif
#ifndef USBPRF_DATA_PAGES
#define USBPRF_DATA_PAGES    32
#endif
#ifndef USBPRF_FAT_BUF_SIZE
#define USBPRF_FAT_BUF_SIZE  4
#endif
#ifndef USBPRF_DATA_BUF_SIZE
#define USBPRF_DATA_BUF_SIZE 8
#endif

// Force to use the 1st instance in development.
#define USB_INSTANCE         1U

#define USB_SEC_SHIFT        9U
#define USB_SEC_SIZE         512U

// To avoid vcast misra-c check, define an Unused PDM_DISK
struct PDM_DISK {
    UINT32 Unused;
};

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

static STORAGE_PRFILE2_OBJ_s usb1_prf2_obj USB_ATTR_ALIGN_CACHE_NO_INIT;
static STORAGE_PRFILE2_OBJ_s *usb_prf2_table[USB_INSTANCE] USB_ATTR_ALIGN_CACHE = { &usb1_prf2_obj };

static UINT32 msch_check_slots(UINT32 Id)
{
    UINT32 uret;

    if (Id < USB_INSTANCE) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

static UINT32 msch_check_usb_info(const USBH_STORAGE_INFO_s *Info)
{
    UINT32 uret;

    if (Info != NULL) {
        if (Info->SectorSize == 512U) {
            if (Info->WriteProtect == 0U) {
                uret = 1;
            } else {
                uret = 0;
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "msch_check_usb_info(): write protect!!");
            }
        } else {
            uret = 0;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "msch_check_usb_info(): sector size error 0x%x", Info->SectorSize, 0, 0, 0, 0);
        }
    } else {
        uret = 0;
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "msch_check_usb_info(): get null usb info");
    }
    return uret;
}

static INT32 msch_lun0_write(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 uret;

    if (AmbaUSBH_StorageWrite(pBuf, Sector, Sectors) != 0U) {
        uret = -1;
    } else {
        uret = 0;
    }
    return uret;
}

static INT32 msch_lun0_read(UINT8 *pBuf, UINT32 Sector, UINT32 Sectors)
{
    INT32 uret;

    if (AmbaUSBH_StorageRead(pBuf, Sector, Sectors) != 0U) {
        uret = -1;
    } else {
        uret = 0;
    }
    return uret;
}

static void msch_check_format(UINT32 Id)
{
    STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;

    INT32 (*WriteFunc)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    INT32 (*ReadFunc)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    INT32 fucn_ret;
    const AMBA_FS_FAT_MBR *fat_mbr;
    USBH_STORAGE_INFO_s   *msch_storage_info;
    UINT32 sector_count;

    usb_prf2_obj = usb_prf2_table[Id];
    fat_mbr      = USB_UtilityP8ToFsMbr(usb_prf2_obj->Mbr);

    WriteFunc = msch_lun0_write;
    ReadFunc  = msch_lun0_read;

    msch_storage_info = USBH_StorageMediaInstanceGet();

    if (msch_check_usb_info(msch_storage_info) != 0U) {
        // Setup Drive infomation
        sector_count = msch_storage_info->Lba + 1U;

        // Read the master boot Sector

        // Single block read
        fucn_ret = ReadFunc(usb_prf2_obj->Mbr, 0, 1);
        if (fucn_ret >= 0) {
            if (fat_mbr->Signature == 0xaa55U) {
                AMBA_FS_FAT_BS32 *fat_bs32 = USB_UtilityP8ToFsBs32(usb_prf2_obj->Mbr);
                UINT32 flag_leave           = 0;
                msch_storage_info->Format = AmbaFS_FatTypeDetermination(fat_bs32);

                if (msch_storage_info->Format != 0x0U) {
                    // This Sector is not MBR. It maybe boot Sector.
                    USB_UtilityMemoryCopy(usb_prf2_obj->Bs, usb_prf2_obj->Mbr, USB_SEC_SIZE);
                    USB_UtilityMemorySet(usb_prf2_obj->Mbr, 0, USB_SEC_SIZE);
                } else {
                    msch_storage_info->Format = 0;
                }

                // Read the boot Sector
                if ((fat_mbr->PartTable0.LogicStartSector != 0x0U) && (fat_mbr->PartTable0.LogicStartSector <= sector_count)) {
                    fucn_ret = ReadFunc(usb_prf2_obj->Bs, fat_mbr->PartTable0.LogicStartSector, 1);
                    if (fucn_ret < 0) {
                        msch_storage_info->Format = 0;
                        flag_leave            = 1;
                    }
                }

                if (flag_leave == 0U) {
                    if (msch_storage_info->Format == 0x0U) {
                        AMBA_FS_EXFAT_BS *exfat_bs = USB_UtilityP8ToFsFxfatBs(usb_prf2_obj->Bs);
                        fat_bs32             = USB_UtilityP8ToFsBs32(usb_prf2_obj->Bs);
                        msch_storage_info->Format = AmbaFS_FatTypeDetermination(fat_bs32);

                        if ((msch_storage_info->Format == FS_EXFAT) && (exfat_bs->Vfg == 0x2U)) {
                            UINT8 Buf[USB_SEC_SIZE];
                            UINT32 lba_start = fat_mbr->PartTable0.LogicStartSector;
                            USB_UtilityMemoryCopy(Buf, usb_prf2_obj->Bs, USB_SEC_SIZE);
                            exfat_bs      = USB_UtilityP8ToFsFxfatBs(Buf);
                            exfat_bs->Vfg = 0;
                            if (WriteFunc(Buf, lba_start, 1) < 0) {
                                msch_storage_info->Format = 0;
                                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "msch_check_format(): Clear dirty bit fail!");
                            }
                        }
                    }
                }
            } else {
                USB_UtilityMemorySet(usb_prf2_obj->Mbr, 0, USB_SEC_SIZE);
                msch_storage_info->Format = 0x0;
            }
        } else {
            msch_storage_info->Format = 0;
        }
    }
}

static PDM_ERROR msch_prf2_format(UINT32 Id, struct PDM_DISK *pPdmDisk, const PDM_U_CHAR *pParam)
{
    static UINT8 format_buffer[32 * 512] USB_ATTR_ALIGN_CACHE_NO_INIT;
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
    UINT8 buf[USB_SEC_SIZE];
#endif
    const USBH_STORAGE_INFO_s *msch_storage_info;

    AmbaMisra_TouchUnused(pPdmDisk);
    (void)pParam;

    WriteFunc = msch_lun0_write;
    ReadFunc  = msch_lun0_read;

    USB_UtilityMemorySet(&fs_drive_info, 0, sizeof(fs_drive_info));
    fs_drive_info.Drive = (UINT16)(Id & 0xFFFFU);

    msch_storage_info = USBH_StorageMediaInstanceGet();

    if (msch_check_usb_info(msch_storage_info) != 0U) {
        // Setup Drive infomation
        sector_count = msch_storage_info->Lba + 1U;

        AmbaFS_ParseSdaFormatParam(sector_count, &fs_drive_info);  // (todo) shall be handled by FS to pass the Vcast.

#if PRF2_FORMAT_LAYER_SELECTABLE
        nsect = 32;
        buffer_ptr  = format_buffer;

        // Calculate number of system Sectors
        AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
        if (fs_drive_info.FatType == (UINT16)EXFAT_TYPE) {
            sys_sector_count = fs_drive_info.StartLba * 2U;
        } else if (fs_drive_info.FatType == (UINT16)FAT32_TYPE) {
            fat_bs32      = USB_UtilityP8ToFsBs32(buffer_ptr);
            sys_sector_count = fs_drive_info.StartLba + fat_bs32->Fno +
                               (fat_bs32->Spf32 * fat_bs32->Fts) + fs_drive_info.Spc;
        } else {
            UINT16 entry_count;
            fat_bs16      = USB_UtilityP8ToFsBs16(buffer_ptr);
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

        USB_UtilityMemorySet(buffer_ptr, 0x0, ((SIZE_t)nsect) << USB_SEC_SHIFT);
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
                            UINT8 *ExtraBuf = &buffer_ptr[(UINT32)4 << USB_SEC_SHIFT];
                            UINT32 ExtraSize = (nsect - 4U) << USB_SEC_SHIFT;
                            AmbaFS_CreateBs(&fs_drive_info, buffer_ptr);
                            // (todo) shall be handled by FS to pass Vcast.
                            err_ret = AmbaFS_CreateExfatEntry(USB_UtilityP8ToFsFxfatBs(buffer_ptr), WriteFunc, ReadFunc,
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

static INT32 msch_prf2_format_drive(UINT32 Id, const char *pParam)
{
    INT32 nret = 0;
    const STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;

    (void)pParam;

    if (msch_check_slots(Id) != 0U) {
        usb_prf2_obj = usb_prf2_table[Id];

        if ((usb_prf2_obj->pDrvTableEntry->stat & (UINT8)PF_ATTACHED) == 1U) {
            char Drive = 'i';
            (void)AmbaFS_UnMount(Drive);
        }

        nret = msch_prf2_format(Id, usb_prf2_obj->pDiskTable, NULL);
        if (nret < 0) {
            // print error message
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "msch_prf2_format_drive(): pf_Format failed");
        }
        pdm_disk_notify_media_insert(usb_prf2_obj->pDiskTable);

        msch_check_format(Id);
    } else {
        nret = DRV_FAILED;
    }

    return nret;
}

static PF_ERROR msch_prf2_init_drive_table(UINT32 Id, PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;
    PF_ERROR err_ret;

    if (msch_check_slots(Id) != 0U) {
        usb_prf2_obj = usb_prf2_table[Id];

        pDiskTable->p_func = (PDM_FUNCTBL*) &usb_prf2_obj->DrvFunc;
        pDiskTable->ui_ext = UiExt;
        err_ret            = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_prf2_drive_init(UINT32 Id, struct PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    if (msch_check_slots(Id) != 0U) {
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

static PDM_ERROR msch_prf2_finalize(UINT32 Id, struct PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;
    const STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;

    if (msch_check_slots(Id) != 0U) {
        usb_prf2_obj = usb_prf2_table[Id];

        if (pPdmDisk == NULL) {
            err_ret = DRV_FAILED;
        } else {
            // Termination process for the Driver stub
            // Termination process for the Driver and media controller
            pdm_disk_notify_media_eject(pPdmDisk);
            usb_prf2_obj->pDrvTableEntry->drive = 0;
            usb_prf2_obj->pDrvTableEntry->stat  = 0;
            err_ret                             = DRV_SUCCEED;
        }
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_prf2_mount(UINT32 Id, struct PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    AmbaMisra_TouchUnused(pPdmDisk);

    if (msch_check_slots(Id) != 0U) {
        err_ret = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_prf2_unmount(UINT32 Id, struct PDM_DISK *pPdmDisk)
{
    PDM_ERROR err_ret;

    AmbaMisra_TouchUnused(pPdmDisk);

    if (msch_check_slots(Id) != 0U) {
        err_ret = DRV_SUCCEED;
    } else {
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_prf2_get_disk_info(struct PDM_DISK *pPdmDisk, PDM_DISK_INFO *pPdmDiskInfo)
{
    const USBH_STORAGE_INFO_s *msch_storage_info;
    UINT32 sector_count = 0;
    UINT32 total_size_in_mb = 0;
    UINT32 cylinders = 0;
    UINT8  head_number = 0;
    UINT8  sectors_per_track = 0;
    UINT32 prf2_attr = 0;
    PDM_ERROR err_ret;

    AmbaMisra_TouchUnused(pPdmDisk);

    msch_storage_info = USBH_StorageMediaInstanceGet();


    if (msch_storage_info != NULL) {
        if (msch_storage_info->SectorSize == 512U) {
            // Caculate the necessary inFormation
            sector_count = msch_storage_info->Lba + 1U;
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

            if (msch_storage_info->WriteProtect != 0U) {
                prf2_attr = PRF2_MEDIA_ATTR | PDM_MEDIA_ATTR_WRITE_PROTECT;
            } else {
                prf2_attr = PRF2_MEDIA_ATTR;
            }

            pPdmDiskInfo->total_sectors     = sector_count;
            pPdmDiskInfo->cylinders         = (UINT16)cylinders;
            pPdmDiskInfo->heads             = head_number;
            pPdmDiskInfo->sectors_per_track = sectors_per_track;
            pPdmDiskInfo->bytes_per_sector  = USB_SEC_SIZE;
            pPdmDiskInfo->media_attr        = prf2_attr;
            pPdmDiskInfo->format_param      = NULL;

            err_ret = DRV_SUCCEED;
        } else {
            err_ret = DRV_FAILED;
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "msch_prf2_get_disk_info(): get status fail");
        err_ret = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_wrapper_init_drive_table(PDM_DISK_TBL* pDiskTable, PDM_U_LONG UiExt)
{
    PDM_ERROR ret = msch_prf2_init_drive_table(USB_STORAGE_0, pDiskTable, UiExt);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_drive_init(struct PDM_DISK *pDisk)
{
    PDM_ERROR ret = msch_prf2_drive_init(USB_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_finalize(struct PDM_DISK *pDisk)
{
    PDM_ERROR ret = msch_prf2_finalize(USB_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_mount(struct PDM_DISK *pDisk)
{
    PDM_ERROR ret = msch_prf2_mount(USB_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_unmount(struct PDM_DISK *pDisk)
{
    PDM_ERROR ret = msch_prf2_unmount(USB_STORAGE_0, pDisk);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_format(struct PDM_DISK *pDisk, const PDM_U_CHAR *pParam)
{
    PDM_ERROR ret = msch_prf2_format(USB_STORAGE_0, pDisk, pParam);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

static PDM_ERROR msch_wrapper_read(struct PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                                   PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                   PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR err_ret;

    AmbaMisra_TouchUnused(pDisk);

    if (AmbaUSBH_StorageRead(pBuf, Sector, Sectors) == 0U) {
        *pNumSuccess = Sectors;
        err_ret      = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        err_ret      = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_wrapper_write(struct PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                    PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                    PDM_U_LONG *pNumSuccess)
{
    PDM_ERROR err_ret;
    UINT8 *Ptr;

    AmbaMisra_TouchUnused(pDisk);

    Ptr = USB_UtilityConstP8ToP8(pBuf);

    if (AmbaUSBH_StorageWrite(Ptr, Sector, Sectors) == 0U) {
        *pNumSuccess = Sectors;
        err_ret      = DRV_SUCCEED;
    } else {
        *pNumSuccess = 0;
        err_ret      = DRV_FAILED;
    }

    return err_ret;
}

static PDM_ERROR msch_wrapper_get_disk_info(struct PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo)
{
    PDM_ERROR ret = msch_prf2_get_disk_info(pDisk, pDiskInfo);
    if (ret != 0) {
        // action TBD
    }
    return ret;
}

// File System drive function for USB Host MSC
static UINT32 storage_fs_init(void *pDrive)
{
    static PF_CACHE_PAGE usb_cache_page[USBPRF_FAT_PAGES + USBPRF_DATA_PAGES] USB_ATTR_ALIGN_CACHE_NO_INIT;
    static PF_SECTOR_BUF usb_cache_buffer[USBPRF_FAT_PAGES + USBPRF_DATA_PAGES] USB_ATTR_ALIGN_CACHE_NO_INIT;

    STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;
    PF_DRV_TBL *drive = USB_UtilityVoidP2PrFDrvTbl(pDrive);

    // aviod misra-c 'const' issue
    AmbaMisra_TouchUnused(pDrive);

    // Force to use the 1st instance in development.
    usb_prf2_obj = usb_prf2_table[0];

    USB_UtilityMemorySet(usb_prf2_obj, 0x0, sizeof(*usb_prf2_obj));

    usb_prf2_obj->pDrvTableEntry = drive;

    usb_prf2_obj->pDrvTableEntry->drive  = 0x69;
    usb_prf2_obj->DrvFunc.init           = msch_wrapper_drive_init;
    usb_prf2_obj->DrvFunc.finalize       = msch_wrapper_finalize;
    usb_prf2_obj->DrvFunc.mount          = msch_wrapper_mount;
    usb_prf2_obj->DrvFunc.unmount        = msch_wrapper_unmount;
    usb_prf2_obj->DrvFunc.format         = msch_wrapper_format;
    usb_prf2_obj->DrvFunc.physical_read  = msch_wrapper_read;
    usb_prf2_obj->DrvFunc.physical_write = msch_wrapper_write;
    usb_prf2_obj->DrvFunc.get_disk_info  = msch_wrapper_get_disk_info;

    usb_prf2_obj->DrvInitTable.p_func = msch_wrapper_init_drive_table;

    usb_prf2_obj->DrvInitTable.ui_ext = 0;

    if (pdm_open_disk(&usb_prf2_obj->DrvInitTable, &usb_prf2_obj->pDiskTable) != 0) {
        // action TBD
    }

    // Open a partition
    if (pdm_open_partition(usb_prf2_obj->pDiskTable, 0, &usb_prf2_obj->pPartTable) != 0) {
        // action TBD
    }

    // Initialize Driver table
    AmbaMisra_TypeCast32(&usb_prf2_obj->pDrvTableEntry->p_part, &usb_prf2_obj->pPartTable);
    usb_prf2_obj->CacheSet.pages             = (PF_CACHE_PAGE *)usb_cache_page;
    usb_prf2_obj->CacheSet.buffers           = (PF_SECTOR_BUF *)usb_cache_buffer;
    usb_prf2_obj->CacheSet.num_fat_pages     = USBPRF_FAT_PAGES;
    usb_prf2_obj->CacheSet.num_data_pages    = USBPRF_DATA_PAGES;
    usb_prf2_obj->CacheSet.num_fat_buf_size  = USBPRF_FAT_BUF_SIZE;
    usb_prf2_obj->CacheSet.num_data_buf_size = USBPRF_DATA_BUF_SIZE;
    usb_prf2_obj->pDrvTableEntry->cache      = &usb_prf2_obj->CacheSet;

    return USB_INSTANCE;
}

static void storage_check_format(UINT32 Id)
{
    msch_check_format(Id);
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver when a USB MSC device is removed to un-mount the corresponding drive.
*/
void USBH_StorageEject(UINT32 Id)
{
    const STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;

    if (msch_check_slots(Id) != 0U) {
        usb_prf2_obj = usb_prf2_table[Id];

        if ((usb_prf2_obj->pDrvTableEntry->stat & (UINT8)PF_ATTACHED) == 1U) {
            char Drive = 'i';
            UINT32 Rval = AmbaFS_UnMount(Drive);
            if (Rval != 0U) {
                // print error message
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBH_StorageEject(): pf_unmount failed");
            }
        }
    }
}
/**
 * Called by driver when a USB MSC device is inserted to mount the corresponding drive.
*/
void USBH_StorageMediaCheck(UINT32 Id)
{
    const STORAGE_PRFILE2_OBJ_s *usb_prf2_obj;
    const USBH_STORAGE_INFO_s   *msch_storage_info;

    if (msch_check_slots(Id) != 0U) {
        usb_prf2_obj = usb_prf2_table[Id];

        msch_storage_info = USBH_StorageMediaInstanceGet();

        if (msch_storage_info != NULL) {
            if (msch_storage_info->Present != 0U) {
                pdm_disk_notify_media_insert(usb_prf2_obj->pDiskTable);
                storage_check_format(Id);
            } else {
                pdm_disk_notify_media_eject(usb_prf2_obj->pDiskTable);
                USBH_StorageEject(Id);
            }
        }
    }
}
/**
 * Called by driver when a USB MSC device is inserted to set file system buffering mode.
*/
void USBH_StorageBufferingModeSet(UINT32 Id, UINT32 Mode)
{
    if (msch_check_slots(Id) != 0U) {
        char Drive = 'i';
        (void)AmbaFS_SetBufferingMode(Drive, (INT32)Mode); // (todo) shall be handled by FS to pass the Vcast
    }
}

INT32 USBH_StoragePrf2DriveFormat(const char *pParam)
{
    return msch_prf2_format_drive(USB_STORAGE_0, pParam);
}

/**
 * Called by AmbaDrvEntry() during initialization.
*/
UINT32 AmbaUSB_DrvEntry(void)
{
    // register USB drive function into file system
    UINT32 uret = AmbaFS_RegisterExtDrive(storage_fs_init);
    if (uret != 0U) {
        // action TBD
    }
    return uret;
}

/** @} */
