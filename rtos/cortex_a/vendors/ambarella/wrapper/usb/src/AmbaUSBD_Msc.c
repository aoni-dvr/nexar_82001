/**
 *  @file AmbaUSBD_Msc.c
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
 *  @details USB wrapper functions for Mass Storage Device Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <usbx/ux_device_stack.h>
#include <AmbaUSBD_Msc.h>
#include <AmbaUSB_Utility.h>

//#define AMBA_USB_DEBUG

typedef struct {
    UINT8 Number;
    INT8  Drives[MSC_DEV_MAX_NUM];
} MSC_MOUNT_INFO_s;

static USBD_MSC_FS_OP_s msc_fs_operation = { NULL };
static MSC_MOUNT_INFO_s msc_mount_info   = { 0 };
static MSC_DRIVE_INFO_s msc_drive_info[MSC_DEV_MAX_NUM] __attribute__((section(".bss.noinit")));

static UX_SLAVE_CLASS_STORAGE_PARAMETER mscd_param = { 0 };
static UX_SLAVE_CLASS_STORAGE *mscd_instance       = NULL;
static AMBA_KAL_MUTEX_t mscd_mutex                 = { 0 };

static void mscd_mutex_get(void)
{
    udc_msc_mutex_get(udc_msc_get_context(), USB_WAIT_FOREVER);
}

static void mscd_mutex_put(void)
{
    udc_msc_mutex_put(udc_msc_get_context());
}

static UINT32 mscd_physical_read(INT32 Drive, ULONG NumberBlocks, ULONG LBA, UINT8 *DataPtr)
{
    UINT32 uret;

#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "Read %d sector, DataPtr = 0x%X", LBA, DataPtr, 0, 0, 0);
#endif
#endif

    if (msc_fs_operation.SectorRead == NULL) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "mscd_physical_read(): no read callback registered.");
        uret = UX_ERROR;
    } else {
        if (DataPtr == NULL) {
            uret = UX_ERROR;
        } else {
            mscd_mutex_get();

            if (msc_fs_operation.SectorRead(Drive, DataPtr, LBA, NumberBlocks) != 0) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Read %d sector but fail\n", (UINT32)LBA, 0, 0, 0, 0);
                mscd_mutex_put();
                uret = UX_ERROR;
            } else {
                mscd_mutex_put();
                uret = UX_SUCCESS;
            }
        }
    }
    return uret;
}

static UINT32 mscd_physical_write(INT32 Drive, ULONG NumberBlocks, ULONG LBA, const UINT8 *DataPtr)
{
    UINT32 uret;

    mscd_mutex_get();

    if (msc_fs_operation.SectorWrite == NULL) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "mscd_physical_write(): no write callback registered.");
        uret = UX_ERROR;
    } else {
        if (DataPtr == NULL) {
            uret = UX_ERROR;
        } else {
            if (msc_fs_operation.SectorWrite(Drive, DataPtr, LBA, NumberBlocks) != ((INT32)0)) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Write %d sector but fail", (UINT32)LBA, 0, 0, 0, 0);
                mscd_mutex_put();
                uret = UX_ERROR;
            } else {
                mscd_mutex_put();
                uret = UX_SUCCESS;
            }
        }
    }
    return uret;
}

static UINT mscd_media_read(void *Storage, ULONG lun, UCHAR * DataPointer,
                            ULONG NumberBlocks, ULONG lba, ULONG *MediaStatus)
{
    UINT32 uret;
    INT8 drive                   = msc_mount_info.Drives[lun];
    const MSC_DRIVE_INFO_s *mscd = &msc_drive_info[drive];

    AmbaMisra_TouchUnused(Storage);

    if (mscd->Mount == 1U) {
#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        USB_UtilityPrintUInt5("Read lun%d and %d blocks @lba %d, ptr 0x%X", lun, NumberBlocks, lba, DataPointer, 0);
#endif
#endif

        uret = mscd_physical_read(msc_mount_info.Drives[lun], NumberBlocks, lba, DataPointer);

        if (uret == (UINT32)UX_ERROR) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "read media error @%d blocks", (UINT32)NumberBlocks, 0, 0, 0, 0);
            // fix me: should return correct media status
            *MediaStatus  = 0;
            *MediaStatus |= (ULONG)UXD_STORAGE_SENSE_KEY_HARDWARE_ERROR;
            *MediaStatus |= (0x03AUL << 8);
        }
    } else {
        uret = UX_ERROR;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "mscd_media_read(): drive[%d] not present, mount fail!", (UINT32)drive, 0, 0, 0, 0);
    }

    return uret;
}

static UINT mscd_media_write(void *Storage, ULONG lun, const UCHAR * DataPointer,
                             ULONG NumberBlocks, ULONG lba, ULONG *MediaStatus)
{
    UINT32 uret;
    INT8   drive                   = msc_mount_info.Drives[lun];
    const MSC_DRIVE_INFO_s *mscd = &msc_drive_info[drive];;

    AmbaMisra_TouchUnused(Storage);

    if (mscd->Mount == 1U) {
#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        USB_UtilityPrintUInt5("Write %d blocks to LBA %d\n", NumberBlocks, lba, 0, 0, 0);
#endif
#endif

        uret = mscd_physical_write(drive, NumberBlocks, lba, DataPointer);

        if (uret == (UINT32)UX_ERROR) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "write media error @%d blocks", (UINT32)NumberBlocks, 0, 0, 0, 0);
            // fix me: should return correct media status
            *MediaStatus  = 0;
            *MediaStatus |= (ULONG)UXD_STORAGE_SENSE_KEY_HARDWARE_ERROR;
            *MediaStatus |= (0x03AUL << 8);
        }
    } else {
        uret = UX_ERROR;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "mscd_media_write(): drive[%d] not present, mount fail!", (UINT32)drive, 0, 0, 0, 0);
    }

    return uret;
}

static void ux_parameter_info_set(void)
{
    UINT32 i;
    INT8 drive = 0;
    const MSC_DRIVE_INFO_s *mscd;

    mscd_param.number_lun = msc_mount_info.Number;

    for (i = 0; i < msc_mount_info.Number; i++) {
        drive = msc_mount_info.Drives[i];
        mscd  = &msc_drive_info[drive];

        mscd_param.lun[i].media_last_lba       = ((ULONG)mscd->TotalSect) - 1U;
        mscd_param.lun[i].media_block_length   = mscd->SectSize;
        mscd_param.lun[i].media_type           = mscd->DeviceType;
        mscd_param.lun[i].media_removable_flag = mscd->Removable;
        mscd_param.lun[i].write_protect        = mscd->WriteProtect;
    }

    /* Force to modify storage infomation after _ux_device_class_storage_initialize is called. */
    if ((mscd_instance != NULL) && (mscd_mutex.tx_mutex_id != 0U)) {
        if (USB_UtilityMutexTake(&mscd_mutex, USB_WAIT_FOREVER) != 0U) {
            // ignore error
        }
        /* Store the number of LUN declared.  */
        mscd_instance->number_lun = mscd_param.number_lun;

        /* Copy each individual LUN parameters.  */
        for (i = 0; i < mscd_instance->number_lun; i++) {
            /* Store all the application parameter information about the media.  */
            mscd_instance->lun[i].media_last_lba       = mscd_param.lun[i].media_last_lba;
            mscd_instance->lun[i].media_block_length   = mscd_param.lun[i].media_block_length;
            mscd_instance->lun[i].media_type           = mscd_param.lun[i].media_type;
            mscd_instance->lun[i].media_removable_flag = mscd_param.lun[i].media_removable_flag;
            mscd_instance->lun[i].write_protect        = mscd_param.lun[i].write_protect;
        }

        if (USB_UtilityMutexGive(&mscd_mutex) != 0U) {
            // ignore error
        }
    }

    return;
}

static UINT mscd_media_get_status(void *storage,
                                  ULONG       lun,
                                  ULONG       MediaId,
                                  ULONG *     MediaStatus)
{
    static UINT32 card_not_ready_count = 0;
    INT32 result;
    USBD_MSC_CARD_STATUS_s scm_status;
    INT8 drive             = msc_mount_info.Drives[lun];
    MSC_DRIVE_INFO_s *mscd = &msc_drive_info[drive];
    UINT32 uret;

    AmbaMisra_TouchUnused(storage);
    (void)MediaId;

    if (msc_fs_operation.GetMediaInfo == NULL) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "mscd_media_get_status(): no get_status callback registered.");
        *MediaStatus  = 0;
        *MediaStatus |= (ULONG)UXD_STORAGE_SENSE_KEY_NOT_READY;
        *MediaStatus |= (0x03AUL << 8);
        uret           = UX_ERROR;
    } else {
        result = msc_fs_operation.GetMediaInfo(msc_mount_info.Drives[lun], &scm_status);
        if ((scm_status.MediaPresent == 0U) ||
            (result == -1) ||
            ((scm_status.MediaPresent == 1U) && (scm_status.Format == -1)) ||
            (scm_status.SectorNumber == 0U)) {
            *MediaStatus  = 0U;
            *MediaStatus |= (ULONG)UXD_STORAGE_SENSE_KEY_NOT_READY;
            *MediaStatus |= (0x3AUL << 8);
            mscd->Mount   = 0;
            /* To reduce the error message frequency, show it at the first time or every 15 times if card is not ready */
            if (card_not_ready_count == 0U) {
                // card is not ready
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "mscd_media_get_status(): Drive [%d] Card is not present !", (UINT32)msc_mount_info.Drives[lun], 0, 0, 0, 0);
            }
            card_not_ready_count = (card_not_ready_count + 1U) % 15U;

            uret = UX_ERROR;
        } else {
            uret             = UX_SUCCESS;
            card_not_ready_count   = 0;
            mscd->TotalSect = scm_status.SectorNumber;
            mscd->SectSize  = scm_status.SectorSize;
            /* Set storage info if the drive is present */
            if (mscd->Mount == 0U) {
                mscd->Mount = 1;
                ux_parameter_info_set();
            }
        }
    }

    return uret;
}

static void ux_parameter_operations_set(void)
{
    UINT32 i;

    mscd_param.number_lun = msc_mount_info.Number;

    for (i = 0; i < msc_mount_info.Number; i++) {
        /* Store media operator */
        mscd_param.lun[i].media_read   = mscd_media_read;
        mscd_param.lun[i].media_write  = mscd_media_write;
        mscd_param.lun[i].media_status = mscd_media_get_status;
    }

    /* Force to modify storage infomation after _ux_device_class_storage_initialize is called. */
    if ((mscd_instance != NULL) && (mscd_mutex.tx_mutex_id != 0UL)) {
        if (USB_UtilityMutexTake(&mscd_mutex, USB_WAIT_FOREVER) != 0U) {
            // ignore error
        }
        /* Store the number of LUN declared.  */
        mscd_instance->number_lun = mscd_param.number_lun;

        /* Copy each individual LUN parameters.  */
        for (i = 0; i < mscd_instance->number_lun; i++) {
            /* Store media operator. */
            mscd_instance->lun[i].media_read   = mscd_param.lun[i].media_read;
            mscd_instance->lun[i].media_write  = mscd_param.lun[i].media_write;
            mscd_instance->lun[i].media_status = mscd_param.lun[i].media_status;
        }

        if (USB_UtilityMutexGive(&mscd_mutex) != 0U) {
            // ignore error
        }
    }

    return;
}

static UINT32 ux_parameter_init(void)
{
    if (msc_mount_info.Number == 0U) {
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ux_parameter_init(): No Device Mounted, Mount Null LUN.");
        mscd_param.null_lun                    = 1;
        mscd_param.number_lun                  = 1;
        mscd_param.lun[0].media_last_lba       = 0;
        mscd_param.lun[0].media_block_length   = 0;
        mscd_param.lun[0].media_type           = 0;
        mscd_param.lun[0].media_removable_flag = 0x80;
        mscd_param.lun[0].write_protect        = 0;
        mscd_param.lun[0].media_read           = mscd_media_read;
        mscd_param.lun[0].media_write          = mscd_media_write;
        mscd_param.lun[0].media_status         = mscd_media_get_status;

        /* Force to modify storage infomation after _ux_device_class_storage_initialize is called. */
        if ((mscd_instance != NULL) && (mscd_mutex.tx_mutex_id != 0UL)) {
            if (USB_UtilityMutexTake(&mscd_mutex, USB_WAIT_FOREVER) != 0U) {
                // ignore error
            }

            /* Store the number of LUN declared.  */
            mscd_instance->null_lun   = mscd_param.null_lun;
            mscd_instance->number_lun = mscd_param.number_lun;
            /* Store all the application parameter information about the media.  */
            mscd_instance->lun[0].media_last_lba       = 0;
            mscd_instance->lun[0].media_block_length   = 0;
            mscd_instance->lun[0].media_type           = 0;
            mscd_instance->lun[0].media_removable_flag = 0x80;
            mscd_instance->lun[0].write_protect        = 0;
            mscd_instance->lun[0].media_read           = mscd_media_read;
            mscd_instance->lun[0].media_write          = mscd_media_write;
            mscd_instance->lun[0].media_status         = mscd_media_get_status;

            if (USB_UtilityMutexGive(&mscd_mutex) != 0U) {
                // ignore error
            }
        }
    }


    return UX_SUCCESS;
}

static UINT32 check_null_for_deviceinfo(const USBD_MSC_DEVICE_INFO_s *DeviceInfo)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (DeviceInfo == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else {
        if ((DeviceInfo->Operation == NULL) || (DeviceInfo->Description == NULL)) {
            uret = USB_ERR_PARAMETER_NULL;
        } else {
            if ((DeviceInfo->Operation->SectorRead == NULL) || (DeviceInfo->Operation->SectorWrite == NULL) || (DeviceInfo->Operation->GetMediaInfo == NULL)) {
                uret = USB_ERR_PARAMETER_NULL;
            }
            if ((DeviceInfo->Description->Vendor == NULL) || (DeviceInfo->Description->Product == NULL) || (DeviceInfo->Description->Revision == NULL)) {
                uret = USB_ERR_PARAMETER_NULL;
            }
        }
    }
    return uret;
}
/**
 * This command is used to:<br>
- Register storage device operation functions including read, write, and media information function.
- Configure the Mass Storage drive property.
- Specify the descriptive information of the USB mass storage drive.
- Set the information for creating MSC read cache task.
- It should be called before AmbaUSBD_SystemClassStart().
* @param pDeviceInfo [Input] MSC device information.
*/
UINT32 AmbaUSBD_MSCSetInfo(const USBD_MSC_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 uret;
    static UCHAR uxd_storage_vendor_id[]   = "ExpressL";
    static UCHAR uxd_storage_product_id[]  = "USBX storage dev";
    static UCHAR uxd_storage_product_rev[] = "2001";

    uret = check_null_for_deviceinfo(pDeviceInfo);

    if (uret == USB_ERR_SUCCESS) {
        UINT32 i;
        MSC_DRIVE_INFO_s *mscd;

        // initialize internal variables
        msc_mount_info.Number = 0;

        for (i = 0U; i < MSC_DEV_MAX_NUM; i++) {
            msc_mount_info.Drives[i] = -1;
        }
        _ux_utility_memory_set(msc_mount_info.Drives, 0xFFU, (ULONG)MSC_DEV_MAX_NUM * sizeof(INT8));
        _ux_utility_memory_set(msc_drive_info, 0U, (ULONG)MSC_DEV_MAX_NUM * sizeof(MSC_DRIVE_INFO_s));

        // copy Operations
        msc_fs_operation.SectorRead   = pDeviceInfo->Operation->SectorRead;
        msc_fs_operation.SectorWrite  = pDeviceInfo->Operation->SectorWrite;
        msc_fs_operation.GetMediaInfo = pDeviceInfo->Operation->GetMediaInfo;

        // copy Property
        for (i = 0U; i < MSC_DEV_MAX_NUM; i++) {
            const USBD_MSC_DRIVE_PROP_s *prop = &pDeviceInfo->Property[i];
            mscd = &msc_drive_info[i];
            // RMB bit.
            if (prop->Removal != 0U) {
                mscd->Removable = 0x80;
            } else {
                mscd->Removable = 0;
            }
            if (prop->WriteProtect != 0U) {
                mscd->WriteProtect = 1;
            } else {
                mscd->WriteProtect = 0;
            }
            mscd->DeviceType = prop->Type;
        }

        // copy Description
        USB_UtilityMemcpyUInt8(
            uxd_storage_vendor_id,
            pDeviceInfo->Description->Vendor,
            8);
        USB_UtilityMemcpyUInt8(
            uxd_storage_product_id,
            pDeviceInfo->Description->Product,
            16);
        USB_UtilityMemcpyUInt8(
            uxd_storage_product_rev,
            pDeviceInfo->Description->Revision,
            4);

        // todo: replace vendor/product/revision descriptor with application's
    }

    return uret;
}

/**
 * This function is used to mount the mass storage device. All mass storage configurations should be completed before calling this API.
 * Note that the mounted drive will not be removed from file system and might cause data inconsistency.
 * To avoid data inconsistency, the application should call corresponding FIO APIs to unmount it from the file system.
 * Application should make sure the target disk drive is ready to access before calling this API, especially when system is in power-on condition.
 * @param Drive [Input] Target disk drive. Valid value: 0 - 2.
*/
UINT32 AmbaUSBD_MSCMount(UINT8 Drive)
{
    MSC_DRIVE_INFO_s *mscd;
    INT32 result;
    USBD_MSC_CARD_STATUS_s scm_status;
    UINT32 ret;

    if (Drive >= MSC_DEV_MAX_NUM) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCMount(): Invalid drive = %d", Drive, 0, 0, 0, 0);
        ret = USB_ERR_PARAMETER_INVALID;
    } else {
        UINT32 already_mounted = 0, i;
        mscd = &msc_drive_info[Drive];

        // check if the drive is mounted
        for (i = 0; i < msc_mount_info.Number; i++) {
            if (msc_mount_info.Drives[i] == (INT8)Drive) {
                already_mounted = 1;
            }
        }

        if (already_mounted== 1U) {
            ret = USB_ERR_MSCD_ALREADY_MOUNTED;
        } else {
            if (msc_fs_operation.GetMediaInfo != NULL) {
                result = msc_fs_operation.GetMediaInfo((INT32)Drive, &scm_status);
                if ((result == -1) ||
                    (scm_status.MediaPresent == 0U) ||
                    ((scm_status.MediaPresent == 1U) && (scm_status.Format == -1)) ||
                    (scm_status.SectorNumber == 0U)) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCMount(): drive[%d] not present, mount fail!", Drive, 0, 0, 0, 0);

                    mscd->Mount     = 0U;
                    mscd->TotalSect = 0U;
                    mscd->SectSize  = 0U;

                    /* Assign slot drive even if the card is not present */
                    msc_mount_info.Drives[msc_mount_info.Number] = (INT8)Drive;
                    msc_mount_info.Number++;
                    ux_parameter_operations_set();
                    ux_parameter_info_set();
                    ret = USB_ERR_MEDIA_NOT_READY;
                } else {
                    ret             = USB_ERR_SUCCESS;
                    mscd->Mount     = 1;
                    mscd->MscLun    = msc_mount_info.Number;
                    mscd->TotalSect = scm_status.SectorNumber;
                    mscd->SectSize  = scm_status.SectorSize;

                    msc_mount_info.Drives[msc_mount_info.Number] = (INT8)Drive;
                    msc_mount_info.Number++;
                    ux_parameter_operations_set();
                    ux_parameter_info_set();
                }
            } else {
                ret = USB_ERR_NO_INIT;
            }
        }
    }

    return ret;
}

static INT8 u8_to_i8(UINT8 Value)
{
    INT8 i8_ret;
    USB_UtilityMemoryCopy(&i8_ret, &Value, sizeof(Value));
    return i8_ret;
}

/**
 * This function is used to unmount a storage device from the Mass Storage class.
 * Note that the unmounted drive will not be added to file system.
 * To avoid this, the application should call corresponding FIO APIs to mount it to the file system.
 * @param Drive [Input] Target disk drive. Valid value: 0 - 2.
*/
UINT32 AmbaUSBD_MSCUnMount(UINT8 Drive)
{
    UINT32 rval = 0;
    MSC_DRIVE_INFO_s *mscd;
    UINT32 i;
    UINT8 tmp_num;
    INT8 tmp_drive = 0;
    INT8 max_drive_num = u8_to_i8(MSC_DEV_MAX_NUM);

    if (Drive >= MSC_DEV_MAX_NUM) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCUnMount(): Invalid drive = %d", Drive, 0, 0, 0, 0);
        rval = USB_ERR_PARAMETER_INVALID;
    } else {
        if (msc_mount_info.Number == 0U) {
            rval = USB_ERR_NO_INSTANCE;
        } else {
            rval = UX_SUCCESS;
            msc_mount_info.Number--;
            mscd                           = &msc_drive_info[Drive];
            msc_mount_info.Drives[mscd->MscLun] = -1;
            mscd->Mount                    = 0;
            mscd->MscLun                   = 0;

            /* Remapping Mount Device*/
            tmp_num       = msc_mount_info.Number;
            msc_mount_info.Number = 0;
            for (i = 0; i < MSC_DEV_MAX_NUM; i++) {
                tmp_drive = msc_mount_info.Drives[i];
                if ((tmp_drive != -1) && (tmp_drive >= 0) && (tmp_drive < max_drive_num)) {
                    if (msc_drive_info[tmp_drive].Mount == 1U) {
                        rval                = AmbaUSBD_MSCMount((UINT8)tmp_drive);
                        msc_mount_info.Drives[i] = -1;
                    }
                }
            }
            if (tmp_num != msc_mount_info.Number) {
                // error handling
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCUnMount(): Mount Remapping ERROR %d, %d!!", tmp_num, msc_mount_info.Number, 0, 0, 0);
            }
            if (msc_mount_info.Number == 0U) {
                rval = ux_parameter_init();
            }
        }
    }

    return rval;
}

UINT32 USBD_MscStackRegister(UINT32 ConfigurationIndex, UINT32 InterfaceIndex)
{
    static UCHAR uxd_storage_name[] = "ux_slave_class_storage";
    UINT32 uret;

    (void)ux_parameter_init();

    // register device class
    uret = _ux_device_stack_class_register(uxd_storage_name,
                                           _ux_device_class_storage_entry,
                                           ConfigurationIndex,
                                           InterfaceIndex,
                                           &mscd_param);

    if (uret != ((UINT32)UX_SUCCESS)) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_MscRegisterStack(): Failed to init USBX device class with error %x\n", uret, 0, 0, 0, 0);
        uret = USBX_ERR_CLASS_REGISTER_FAIL;
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_MscRegisterStack(): Finish init USBX device class");
        if (mscd_mutex.tx_mutex_id == 0U) {
            uret = USB_UtilityMutexCreate(&mscd_mutex);
            if (uret != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_MscRegisterStack(): create mscd_mutex fail 0x%2x\n", uret, 0, 0, 0, 0);
            }
        }
    }

    return uret;
}
/**
 * This function is used for USB device system to start MSC class.
 * Applications should pass it as ClassStartCb parameter when calling AmbaUSBD_ClassStart().
 * */
UINT32 AmbaUSBD_MSCClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo)
{
    UINT32 uret;

    if (pDescInfo == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pDescInfo->DescFrameworkFs == NULL) ||
               (pDescInfo->DescFrameworkHs == NULL) ||
               (pDescInfo->StrFramework == NULL) ||
               (pDescInfo->LangIDFramework == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pDescInfo->DescSizeFs == 0U) ||
               (pDescInfo->DescSizeHs == 0U) ||
               (pDescInfo->StrSize == 0U) ||
               (pDescInfo->LangIDSize == 0U)) {
        uret = USB_ERR_PARAMETER_INVALID;

    } else if (USBD_SystemIsClassConfiged() != USB_ERR_SUCCESS) {
        uret = USB_ERR_NO_INIT;
    } else {
        if (pDescInfo->DescFrameworkSs != NULL) {
            // call _ux_device_stack_initialize for SS
            uret =  _ux_device_stack_ss_initialize(pDescInfo->DescFrameworkSs, pDescInfo->DescSizeSs);
        } else {
            uret = 0;
        }
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCClassStart(): Failed to init SS device framework with error 0x%x", uret, 0, 0, 0, 0);
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "AmbaUSBD_MSCClassStart(): Finish init SS device framework");
            // call _ux_device_stack_initialize for FS/HS
            uret = _ux_device_stack_initialize(pDescInfo->DescFrameworkHs,
                                               pDescInfo->DescSizeHs,
                                               pDescInfo->DescFrameworkFs,
                                               pDescInfo->DescSizeFs,
                                               pDescInfo->StrFramework,
                                               pDescInfo->StrSize,
                                               pDescInfo->LangIDFramework,
                                               pDescInfo->LangIDSize,
                                               UX_NULL);

            if (uret != (UINT32)UX_SUCCESS) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCClassStart(): Failed to init USBX MSC device framework with error 0x%x\n", uret, 0, 0, 0, 0);
                uret = USB_UtilityUXCode2AmbaCode(uret);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "AmbaUSBD_MSCClassStart(): Finish init USBX MSC device framework");
                // start class
                uret = USBD_MscStackRegister(1, 0);
            }
        }
    }

    return uret;
}
/**
 * This function is used for USB device system to stop MSC class.
 *  Applications should pass it as ClassStopCb parameter when calling AmbaUSBD_ClassStart().
 * */
UINT32 AmbaUSBD_MSCClassStop(void)
{
    UINT32 uret = UX_SUCCESS;

    if (USBD_SystemIsClassConfiged() != USB_ERR_SUCCESS) {
        uret = USB_ERR_NO_INIT;
    } else {
        if (mscd_mutex.tx_mutex_id != 0U) {
            uret = USB_UtilityMutexDelete(&mscd_mutex);
            if (uret != 0U) {
                // error handling
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCClassStop(): delete mscd_mutex fail 0x%2x\n", uret, 0, 0, 0, 0);
            }
        }

        if (uret == (UINT32)UX_SUCCESS) {
            // clear device class
            uret = _ux_device_stack_class_clear();

            if (uret != ((UINT32)UX_SUCCESS)) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCClassStop(): Failed to clear USBX device class with error %x\n", uret, 0, 0, 0, 0);
                uret = USBX_ERR_CLASS_DEINIT_FAIL;
            } else {
                // error handling
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "AmbaUSBD_MSCClassStop(): Finish clear USBX device class");
            }

            if (uret == (UINT32)UX_SUCCESS) {
                // free device stack
                uret = _ux_device_stack_free();
                if (uret != ((UINT32)UX_SUCCESS)) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBD_MSCClassStop(): Failed to free USBX device class with error %x\n", uret, 0, 0, 0, 0);
                    uret = USBX_ERR_CLASS_DEINIT_FAIL;
                }
            }
        }
    }

    return uret;
}
/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to set class instance.
 * */
void USBD_MscSetKernelInfo(UX_SLAVE_CLASS_STORAGE* info)
{
    mscd_instance = info;

    return;
}
/** @} */
