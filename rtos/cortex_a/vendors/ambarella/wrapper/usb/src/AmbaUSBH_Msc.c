/**
 *  @file AmbaUSBH_Msc.c
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
 *  @details USB wrapper functions for Mass Storage Host Class
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSBH_Msc.h>
#include <AmbaUSBH_Msc_PrFile.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Buffer.h>
#include <AmbaUSB_Utility.h>
#include <usbx/ux_host_stack.h>

// Sector number of each transfer must not be too larger.
// Otherwise some media device could return errors
#define MAX_TRANSFER_SECTOR_CNT ((UINT32)(UX_HOST_CLASS_STORAGE_MAX_TRANSFER_SIZE >> 9UL))

static UX_HOST_CLASS_STORAGE *UsbxStorage;
static AMBA_KAL_MUTEX_t storage_mutex;
static UINT8 *storage_align_buffer_ptr;
static USBH_STORAGE_INFO_s storage_media_info;
static UINT32 is_class_started = 0;
static UCHAR uxh_storage_name[] = "ux_host_class_storage";

static UINT32 msch_check_buf_cache_alignment(const UINT8 *Buffer)
{
    UINT32 address = USB_UtilityU8PtrToU32Addr(Buffer);
    UINT32 mask = AMBA_CACHE_LINE_SIZE - 1U;
    UINT32 uret;

    if ((address & mask) != 0U) {
        uret = USB_ERR_FAIL;
    } else {
        uret = USB_ERR_SUCCESS;
    }

    return uret;
}
static UINT32 ux_code_to_amba_code(UINT32 UxCode)
{
    UINT32 amba_code;
    if (UxCode == (UINT32)UX_HOST_CLASS_STORAGE_SENSE_ERROR) {
        amba_code = USBX_ERR_HSTRG_SENSE_ERROR;
    } else if (UxCode == (UINT32)UX_TRANSFER_TIMEOUT) {
        amba_code = USBX_ERR_HSTRG_TRANSFER_TIMEOUT;
    } else {
        amba_code = USBX_ERR_HSTRG_TRANSFER_FAIL;
    }
    return amba_code;
}

static UINT32 msch_storage_read(UINT8 *Buffer, UINT32 StartSector, UINT32 Sectors)
{
    UINT32 uret, unlock_uret;
    UINT32 sector_transfer = 0;
    UINT32 sector_start = StartSector;
    UINT32 sector_remain = Sectors;
    UINT8 *buffer_start = Buffer;
    UINT8 flag_align_copy = 0;
    UINT8 flag_break  = 0;

    // We need to ensure nobody is accessing this storage instance. We use
    // the storage class instance semaphore to protect.
    // This is a HACK !!! Normally the storage class should not be allowed direct access.
    uret = USB_UtilitySemaphoreTake(&UsbxStorage->ux_host_class_storage_semaphore, USB_WAIT_FOREVER);
    if (uret == USB_ERR_SUCCESS) {
        if (msch_check_buf_cache_alignment(Buffer) != USB_ERR_SUCCESS) {
            flag_align_copy = 1;
        }

        while (sector_remain > 0U) {
            UINT32 byte_transfer;

            if (flag_break == 1U) {
                break;
            }

            if (sector_remain > MAX_TRANSFER_SECTOR_CNT) {
                sector_transfer = MAX_TRANSFER_SECTOR_CNT;
            } else {
                sector_transfer = sector_remain;
            }

            byte_transfer = sector_transfer << 9U;

            if ((flag_align_copy == 1U) && (storage_align_buffer_ptr != NULL)) {
                uret = _ux_host_class_storage_media_read(UsbxStorage, sector_start, sector_transfer, storage_align_buffer_ptr);

                if (uret != (UINT32)UX_SUCCESS) {
                    UINT32 PtrAddr = USB_UtilityU8PtrToU32Addr(buffer_start);
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBH] msc Read [%d] sectors from sec [0x%x] to buf [0x%x] NG: 0x%x",
                                          sector_transfer, sector_start, PtrAddr, uret, 0);

                    // convert USBX code to Amba code
                    uret = ux_code_to_amba_code(uret);
                    flag_break = 1;
                    continue;
                }

                USB_UtilityMemoryCopy(buffer_start, storage_align_buffer_ptr, byte_transfer);
            } else {
                uret = _ux_host_class_storage_media_read(UsbxStorage, sector_start, sector_transfer, buffer_start);

                if (uret != (UINT32)UX_SUCCESS) {
                    UINT32 PtrAddr = USB_UtilityU8PtrToU32Addr(buffer_start);
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBH] msc Read [%d] sectors from sec [0x%x] to buf [0x%x] NG: 0x%x",
                                          sector_transfer, sector_start, PtrAddr, uret, 0);

                    // convert USBX code to Amba code
                    uret = ux_code_to_amba_code(uret);
                    flag_break = 1;
                    continue;
                }
            }

            sector_start  += sector_transfer;
            buffer_start      = &buffer_start[byte_transfer];
            sector_remain -= sector_transfer;
        }

        // Other threads are now allowed to access this storage instance.
        unlock_uret = USB_UtilitySemaphoreGive(&UsbxStorage->ux_host_class_storage_semaphore);
        if (unlock_uret!= USB_ERR_SUCCESS) {
            if (uret == USB_ERR_SUCCESS) {
                // only set error code when transfer is OK.
                uret = USB_ERR_HSTRG_SEMAPHORE_UNLOCK_FAIL;
            }
        }
    } else {
        uret = USB_ERR_HSTRG_SEMAPHORE_LOCK_FAIL;
    }

    return uret;
}

static UINT32 msch_storage_write(UINT8 *Buffer, UINT32 StartSector, UINT32 Sectors)
{
    UINT32 uret;
    UINT32 unlock_uret;
    UINT32 sector_transfer = 0;
    UINT32 sector_start = StartSector;
    UINT32 sector_remain = Sectors;
    UINT8 *buffer_start = Buffer;
    UINT8 flag_align_copy = 0;
    UINT8 flag_break  = 0;

    // We need to ensure nobody is accessing this storage instance. We use
    // the storage class instance semaphore to protect.
    // This is a HACK !!! Normally the storage class should not be allowed direct access.
    uret = USB_UtilitySemaphoreTake(&UsbxStorage->ux_host_class_storage_semaphore, USB_WAIT_FOREVER);
    if (uret == USB_ERR_SUCCESS) {
        if (msch_check_buf_cache_alignment(Buffer) != USB_ERR_SUCCESS) {
            flag_align_copy = 1;
        }
        while (sector_remain > 0U) {
            UINT32 byte_transfer;

            if (flag_break == 1U) {
                break;
            }

            if (sector_remain > MAX_TRANSFER_SECTOR_CNT) {
                sector_transfer = MAX_TRANSFER_SECTOR_CNT;
            } else {
                sector_transfer = sector_remain;
            }

            byte_transfer = sector_transfer << 9U;

            if ((flag_align_copy == 1U) && (storage_align_buffer_ptr != NULL)) {
                USB_UtilityMemoryCopy(storage_align_buffer_ptr, buffer_start, byte_transfer);

                uret = _ux_host_class_storage_media_write(UsbxStorage, sector_start, sector_transfer, storage_align_buffer_ptr);
                if (uret != (UINT32)UX_SUCCESS) {

                    UINT32 PtrAddr = USB_UtilityU8PtrToU32Addr(buffer_start);
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBH] msc Write [%d] sectors from buf [0x%x] to sec [0x%x] NG: 0x%x",
                                          sector_transfer, PtrAddr, sector_start, uret, 0);

                    // convert USBX code to Amba code
                    uret = ux_code_to_amba_code(uret);
                    flag_break = 1;
                    continue;
                }
            } else {
                uret = _ux_host_class_storage_media_write(UsbxStorage, sector_start, sector_transfer, buffer_start);
                if (uret != (UINT32)UX_SUCCESS) {
                    UINT32 PtrAddr = USB_UtilityU8PtrToU32Addr(buffer_start);
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBH] msc Write [%d] sectors from buf [0x%x] to sec [0x%x] NG: 0x%x",
                                          sector_transfer, PtrAddr, sector_start, uret, 0);
                    // convert USBX code to Amba code
                    uret = ux_code_to_amba_code(uret);
                    flag_break = 1;
                    continue;
                }
            }

            sector_start  += sector_transfer;
            buffer_start      = &buffer_start[byte_transfer];
            sector_remain -= sector_transfer;
        }

        // Other threads are now allowed to access this storage instance.
        unlock_uret = USB_UtilitySemaphoreGive(&UsbxStorage->ux_host_class_storage_semaphore);
        if (unlock_uret != USB_ERR_SUCCESS) {
            if (uret == USB_ERR_SUCCESS) {
                // only set error code when transfer is OK.
                uret = USB_ERR_HSTRG_SEMAPHORE_UNLOCK_FAIL;
            }
        }
    } else {
        uret = USB_ERR_HSTRG_SEMAPHORE_LOCK_FAIL;
    }

    return (uret);
}

static UINT32 storage_change_callback(UINT32 Operation, const void *Storage)
{
    switch (Operation) {
    case UX_DEVICE_INSERTION:
        UsbxStorage = USB_UtilityVoidP2UxHStrg(Storage);
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "storage_change_callback: Insertion");
        USBH_StorageMediaCheck(USB_STORAGE_0);
        USBH_StorageBufferingModeSet(USB_STORAGE_0, AMBA_FS_WRITE_BACK_ON_SIGNIF_API); // (todo)The mode shall be modified in FS to pass Vcast.
        break;
    case UX_DEVICE_REMOVAL:
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "storage_change_callback: Remove");
        USBH_StorageEject(USB_STORAGE_0);
        break;
    default:
        // unsupported operation
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "storage_change_callback: Unsupport operaion = 0x%x", Operation, 0, 0, 0, 0);
        break;
    }

    return UX_SUCCESS;
}

USBH_STORAGE_INFO_s* USBH_StorageMediaInstanceGet(void)
{
    const UX_HOST_CLASS_STORAGE_MEDIA *ux_storage_media;
    const UX_HOST_CLASS *ux_class;
    USBH_STORAGE_INFO_s *storage_info_ret;

    if ((UsbxStorage == NULL) || (UsbxStorage->ux_host_class_storage_state != (UINT32)UX_HOST_CLASS_INSTANCE_LIVE)) {
        if (UsbxStorage != NULL) {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBH_StorageMediaInstanceGet: Storage Instance is not ready, state = 0x%x!",
                                  UsbxStorage->ux_host_class_storage_state, 0, 0, 0, 0);
        }
        storage_info_ret = NULL;
    } else {
        // We need the class container.
        ux_class = UsbxStorage->ux_host_class_storage_class;

        // Point the media structure to the first media in the container.
        ux_storage_media = USB_UtilityVoidP2UxHStrgMedia(ux_class->ux_host_class_media);

        storage_media_info.Present      = (UINT32)ux_storage_media->ux_host_class_storage_media_used;
        storage_media_info.Lba          = (UINT32)ux_storage_media->ux_host_class_storage_media_lba;
        storage_media_info.Lun          = (UINT32)ux_storage_media->ux_host_class_storage_media_lun;
        storage_media_info.SectorSize   = (UINT32)ux_storage_media->ux_host_class_storage_media_sector_size;
        storage_media_info.WriteProtect = (UINT32)ux_storage_media->ux_host_class_storage_media_wp;
        storage_info_ret                = &storage_media_info;
    }

    return storage_info_ret;
}
/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * The application calls this API to get the current storage media information.
 * @param pInfo [Output] Information of current USB storage media
*/
UINT32 AmbaUSBH_StorageGetStatus(USBH_STORAGE_INFO_s *pInfo)
{
    const UX_HOST_CLASS_STORAGE_MEDIA *ux_storage_media;
    const UX_HOST_CLASS *ux_class;
    UINT32 uret;

    if (pInfo == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((UsbxStorage == NULL) || (UsbxStorage->ux_host_class_storage_state != (UINT32)UX_HOST_CLASS_INSTANCE_LIVE)) {
        if (UsbxStorage != NULL) {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBH_StorageGetStatus: Storage Instance is not ready, state = 0x%x!",
                                  UsbxStorage->ux_host_class_storage_state, 0, 0, 0, 0);
        }
        USB_UtilityMemorySet(pInfo, 0, sizeof(USBH_STORAGE_INFO_s));
        uret = USB_ERR_NO_INSTANCE;
    } else {
        // We need the class container.
        ux_class = UsbxStorage->ux_host_class_storage_class;

        if (ux_class == NULL) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBH_StorageGetStatus: No class container !", 0, 0, 0, 0, 0);
            uret = USB_ERR_NO_INSTANCE;
        } else if (ux_class->ux_host_class_media == NULL) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBH_StorageGetStatus: No class media !", 0, 0, 0, 0, 0);
            uret = USB_ERR_NO_INSTANCE;
        } else {
            // Point the media structure to the first media in the container.
            ux_storage_media = USB_UtilityVoidP2UxHStrgMedia(ux_class->ux_host_class_media);

            storage_media_info.Present      = (UINT32)ux_storage_media->ux_host_class_storage_media_used;
            storage_media_info.Lba          = (UINT32)ux_storage_media->ux_host_class_storage_media_lba;
            storage_media_info.Lun          = (UINT32)ux_storage_media->ux_host_class_storage_media_lun;
            storage_media_info.SectorSize   = (UINT32)ux_storage_media->ux_host_class_storage_media_sector_size;
            storage_media_info.WriteProtect = (UINT32)ux_storage_media->ux_host_class_storage_media_wp;
            USB_UtilityMemoryCopy(pInfo, &storage_media_info, sizeof(USBH_STORAGE_INFO_s));
            uret = USB_ERR_SUCCESS;
        }
    }


    return uret;
}

/**
 * The application calls this API to read the storage media data.
 * @param pBuffer [Input] The buffer for reading data from the USB media
 * @param pBuffer StartSector [Input] The data offset and the unit is "sector”.
 * @param pBuffer Sectors [Input] The data size and the unit is "sector”.
*/
UINT32 AmbaUSBH_StorageRead(UINT8 *pBuffer, UINT32 StartSector, UINT32 Sectors)
{
    UINT32 uret;

    if (pBuffer == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((UsbxStorage == NULL) || (UsbxStorage->ux_host_class_storage_state != (UINT32)UX_HOST_CLASS_INSTANCE_LIVE)) {
        if (UsbxStorage != NULL) {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBH_StorageRead: Storage Instance is not ready, state = 0x%x!",
                                  UsbxStorage->ux_host_class_storage_state, 0, 0, 0, 0);
        }
        uret = USB_ERR_NO_INSTANCE;
    } else {
        uret = USB_UtilityMutexTake(&storage_mutex, USB_WAIT_FOREVER);
        if (uret == USB_ERR_SUCCESS) {
            uret = msch_storage_read(pBuffer, StartSector, Sectors);

            if (USB_UtilityMutexGive(&storage_mutex) != USB_ERR_SUCCESS) {
                if (uret == USB_ERR_SUCCESS) {
                    // only set error code when transfer is ok.
                    uret = USB_ERR_HSTRG_MUTEX_UNLOCK_FAIL;
                }
            }
        } else {
            uret = USB_ERR_HSTRG_MUTEX_LOCK_FAIL;
        }
    }

    return uret;
}

/**
 * The application calls this API to write the storage media data.
 * @param pBuffer [Input] The buffer for reading data from the USB media
 * @param StartSector [Input] The data offset and the unit is "sector".
 * @param Sectors [Input] The data size and the unit is "sector".
*/
UINT32 AmbaUSBH_StorageWrite(UINT8 *pBuffer, UINT32 StartSector, UINT32 Sectors)
{
    UINT32 uret;

    if (pBuffer == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((UsbxStorage == NULL) || (UsbxStorage->ux_host_class_storage_state != (UINT32)UX_HOST_CLASS_INSTANCE_LIVE)) {
        if (UsbxStorage != NULL) {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "AmbaUSBH_StorageWrite: Storage Instance is not ready, state = 0x%x!",
                                  UsbxStorage->ux_host_class_storage_state, 0, 0, 0, 0);
        }
        uret = USB_ERR_NO_INSTANCE;
    } else {
        uret = USB_UtilityMutexTake(&storage_mutex, USB_WAIT_FOREVER);
        if (uret == USB_ERR_SUCCESS) {
            uret = msch_storage_write(pBuffer, StartSector, Sectors);
            if (USB_UtilityMutexGive(&storage_mutex) != USB_ERR_SUCCESS) {
                // only set error code when transfer is ok.
                if (uret == USB_ERR_SUCCESS) {
                    uret = USB_ERR_HSTRG_MUTEX_UNLOCK_FAIL;
                }
            }
        } else {
            uret = USB_ERR_HSTRG_MUTEX_LOCK_FAIL;
        }
    }

    return uret;
}
/**
 * This function is used for USB Host system to start MSC class.
 * Applications should pass it as ClassStartCallback parameter when calling AmbaUSBH_SystemClassStart().
 * @param ClassID [Input] USB Class ID
*/
UINT32 AmbaUSBH_StorageStart(USBH_CLASS_e ClassID)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (ClassID != USBH_CLASS_STORAGE) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if (USBH_SystemIsUSBXInited() == 0U) {
        uret = USB_ERR_NO_INIT;
    } else if (is_class_started == 1U) {
        uret = USB_ERR_HOST_CLASS_STARTED;
    } else {
        uret = USBH_DeviceChangeFuncHook(uxh_storage_name, storage_change_callback);
        if (uret == USB_ERR_SUCCESS) {
            uret = _ux_host_stack_class_register(uxh_storage_name, _ux_host_class_storage_entry);
            if (uret == (UINT32)UX_SUCCESS) {
                // mass storage class starts here.
                // Attach re-alignement buffer.
                uret = USB_BufferU8Attach(UBUF_MSCH_ALIGN, &storage_align_buffer_ptr);
                if (uret == USB_ERR_SUCCESS) {
                    UINT32 Size = USB_BufferSizeGet(UBUF_MSCH_ALIGN);
                    USB_UtilityMemorySet(storage_align_buffer_ptr, 0, Size);
                } else {
                    // shall not be here.
                }
                // Create lock.
                uret = USB_UtilityMutexCreate(&storage_mutex);
                if (uret != USB_ERR_SUCCESS) {
                    uret = USB_ERR_HSTRG_MUTEX_CREATE_FAIL;
                }
            } else {
                if (uret == (UINT32)UX_HOST_CLASS_ALREADY_INSTALLED) {
                    uret = USBX_ERR_HSTACK_REG_ALREADY_INSTALLED;
                } else if (uret == (UINT32)UX_MEMORY_ARRAY_FULL) {
                    uret = USBX_ERR_HSTACK_REG_ARRAY_FULL;
                } else {
                    uret = USBX_ERR_HSTACK_REG_FAIL;
                }
            }
        }
        if (uret == USB_ERR_SUCCESS) {
            is_class_started = 1;
        }
    }

    return uret;
}
/**
 * This function is used for USB Host system to stop MSC class.
 * Applications should pass it as ClassStopCallback parameter when callingAmbaUSBH_SystemClassStart().
 * @param ClassID [Input] USB Class ID
*/
UINT32 AmbaUSBH_StorageStop(USBH_CLASS_e ClassID)
{
    UINT32 uret = USB_ERR_SUCCESS;
    // To pass misra-c "side effects on right hand of logical operator, ''||''".
    UINT32 is_usbx_init = USBH_SystemIsUSBXInited();

    if (ClassID != USBH_CLASS_STORAGE) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((is_class_started == 0U) || (is_usbx_init == 0U)) {
        uret = USB_ERR_NO_INIT;
    } else {
        // Detach re-alignment buffer.
        USB_BufferU8Detach(UBUF_MSCH_ALIGN, storage_align_buffer_ptr);
        storage_align_buffer_ptr = NULL;

        // Delete lock.
        uret = USB_UtilityMutexDelete(&storage_mutex);
        if (uret == USB_ERR_SUCCESS) {
            USB_UtilityMemorySet(&storage_mutex, 0, sizeof(AMBA_KAL_MUTEX_t));
        } else {
            uret = USB_ERR_HSTRG_MUTEX_DELETE_FAIL;
            // shall not be here.
        }

        (void)USBH_DeviceChangeFuncUnHook(uxh_storage_name);
        is_class_started = 0;
    }

    return uret;
}
/** @} */
