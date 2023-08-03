/**
 *  @file AppUSB_MTPDevice.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details USB MTP device class functions for APP/MW.
 */
/**
 * \page page3 MTP Device application
 * \section Notes
 * \subsection Compatibility issue with Windows XP
         This MTP implementation reports all files as undefined MTP objects to USB Host.
         Some Host operating systems, like Windows XP, will ignore these files and hide them from users.
         To solve this compatibility issue, applications could report corresponding object format for each file.
         For example, report MTP_OFC_JFIF for JPEG files and MTP_OFC_MP4_CONTAINER for MP4/MOV files.
 *
*/
#include "AppUSB.h"
#ifndef APPUSB_MTPD_H
#include "AppUSB_MTPDevice.h"
#endif
#include "AmbaUSB_ErrCode.h"
#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif
#include <AmbaMisraFix.h>
#include "AmbaUtility.h"

#define APP_MTPD_EXTENSION_ID      0x6
#define APP_MTPD_EXTENSION_VERSION 0x64

MTP_CLASS_INFO_s g_mtp_class_info;

#define APP_MTPD_SEND_OBJECT_START 0x01U

typedef struct {
    USBD_MTP_OBJECT_s ObjectInfo;
    UINT32            Handle;
    UINT32            Status;
    AMBA_FS_FILE *    File;
    UINT64            FileOffset;
} APP_MTPD_SEND_OBJECT_INFO_s;

static APP_MTPD_SEND_OBJECT_INFO_s send_object_info;

static UINT16 object_format_supported[] = {
    MTP_OFC_UNDEFINED,
    MTP_OFC_ASSOCIATION,
    MTP_OFC_EXIF_JPEG,
    0
};

static UINT16 object_property_supported[] = {
    /* Object format code : Undefined.  */
    MTP_OFC_UNDEFINED,

    /* NUmber of objects supported for this format.  */
    7,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Object format code : Association.  */
    MTP_OFC_ASSOCIATION,

    /* NUmber of objects supported for this format.  */
    7,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Object format code : Windows Media Audio Clip.  */
    MTP_OFC_EXIF_JPEG,

    /* NUmber of objects supported for this format.  */
    10,
    /* Mandatory objects for all formats.  */
    MTP_OBJECT_PROP_STORAGEID,
    MTP_OBJECT_PROP_OBJECT_FORMAT,
    MTP_OBJECT_PROP_PROTECTION_STATUS,
    MTP_OBJECT_PROP_OBJECT_SIZE,
    MTP_OBJECT_PROP_OBJECT_FILE_NAME,
    MTP_OBJECT_PROP_PARENT_OBJECT,
    MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER,
    //MTP_OBJECT_PROP_NAME,
    //MTP_OBJECT_PROP_NON_CONSUMABLE,

    /* Mandatory objects for all image objects.  */
    MTP_OBJECT_PROP_WIDTH,
    MTP_OBJECT_PROP_HEIGHT,
    MTP_OBJECT_PROP_DATE_AUTHORED,

    0
};

static UINT16 OperationSupportList[] = {
    MTP_OC_GET_DEVICE_INFO,
    MTP_OC_OPEN_SESSION,
    MTP_OC_CLOSE_SESSION,
    MTP_OC_GET_STORAGE_IDS,
    MTP_OC_GET_STORAGE_INFO,
    MTP_OC_GET_NUM_OBJECTS,
    MTP_OC_GET_OBJECT_HANDLES,
    MTP_OC_GET_OBJECT_INFO,
    MTP_OC_GET_OBJECT,
    MTP_OC_GET_THUMB,
    MTP_OC_GET_PARTIAL_OBJECT,
    MTP_OC_DELETE_OBJECT,
    MTP_OC_SEND_OBJECT_INFO,
    MTP_OC_SEND_OBJECT,
    MTP_OC_INITIATE_CAPTURE,
    MTP_OC_INITIATE_OPEN_CAPTURE,
    MTP_OC_TERMINATE_OPEN_CAPTURE,
    //MTP_OC_FORMAT_STORE,
    MTP_OC_RESET_DEVICE,
    MTP_OC_GET_DEVICE_PROP_DESC,
    MTP_OC_GET_DEVICE_PROP_VALUE,
    MTP_OC_SET_DEVICE_PROP_VALUE,
    MTP_OC_RESET_DEVICE_PROP_VALUE,
    MTP_OC_GET_OBJECT_PROPS_SUPPORTED,
    MTP_OC_GET_OBJECT_PROP_DESC,
    MTP_OC_GET_OBJECT_PROP_VALUE,
    MTP_OC_SET_OBJECT_PROP_VALUE,
    //MTP_OC_GET_OBJECT_REFERENCES,
    //MTP_OC_SET_OBJECT_REFERENCES,
    MTP_OC_GET_OBJECT_PROP_LIST,
    MTP_OC_CUSTOM_COMMAND,
    0
};


/* Operation code : DeleteObject 0x100B */
static UINT32 device_delete(ULONG ObjectHandle)
{
    UINT32 status;

    if (AppMtpd_IsOperationSupported(MTP_OC_DELETE_OBJECT) == 0U) {
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        status = AppMtpd_ObjectHandleCheck(ObjectHandle);

        if (status == USB_ERR_SUCCESS) {
            status = AppMtpd_DbObjectDelete(ObjectHandle);
            if (status == MTP_RC_OK) {
                status = 0;
                g_mtp_class_info.HandleNumber--;
            }
        } else {
            status = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    }

    return status;
}

/* Operation code : ResetDevice 0x1010 */
static UINT32 device_reset(void)
{
    return 0;
}

static void object_clear_all(void)
{
    UINT32 sRet;

    // it's called when ONLY when class is going to release.
    //dbg("%s(): start", __func__);
    if (g_mtp_class_info.FpArray != NULL) {
        /* close the file has been canceled*/
        sRet = AmbaFS_FileClose(g_mtp_class_info.FpArray);
        if (sRet != 0U) {
            // need to process it?
        }
        g_mtp_class_info.FpArray           = NULL;
        g_mtp_class_info.OjectHandleOpened = 0;
    }
    AppMtpd_DbClean();
}

/* Operation code : FormatStore 0x100F */
static UINT32 storage_format(ULONG StorageId)
{
    UINT32 file_status;
    UINT32 uret;

    if (AppMtpd_IsOperationSupported(MTP_OC_DELETE_OBJECT) == 0U) {
        uret = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        if (StorageId == MTP_STORAGE_ID) {
            /* Format SD card with default FAT32 format */
            file_status = AmbaFS_Format('d');
            if (file_status != 0U) {
                AppUsb_PrintUInt5("Failed to format", 0, 0, 0, 0, 0);
                uret = MTP_RC_ACCESS_DENIED;
            } else {
                /* Reset the handle counter.  */
                g_mtp_class_info.HandleNumber = 0;
                object_clear_all();
                uret = 0;
            }
        } else {
            uret = MTP_RC_INVALID_STORAGE_ID;
        }
    }
    return uret;
}

/* Operation code : GetStorageInfo 0x1005 */
static UINT32 storage_info_get(ULONG StorageId, ULONG *MaxCapacityL, ULONG *MaxCapacityH, ULONG *FreeSpaceL, ULONG *FreeSpaceH)
{
    UINT32 uret;

    if (StorageId == g_mtp_class_info.DeviceInfo.StorageId) {
        APP_USB_DRIVE_INFO dev_info;
        char drive = AppMtpd_StorageDriveGet(StorageId);
        INT32 rval = AppUsb_FSDriveInfoGet(drive - 'a', &dev_info);
        if (rval == 0) {
            UINT64 capacity  = ((UINT64)dev_info.Cls) * dev_info.Bps * dev_info.Spc;
            UINT64 freesapce = ((UINT64)dev_info.Ecl) * dev_info.Bps * dev_info.Spc;
            *MaxCapacityL = (UINT32)capacity;
            *MaxCapacityH = (UINT32)(capacity >> 32U);
            *FreeSpaceL   = (UINT32)freesapce;
            *FreeSpaceH   = (UINT32)(freesapce >> 32U);
        } else {
            *MaxCapacityL = 0;
            *MaxCapacityH = 0;
            *FreeSpaceL   = 0;
            *FreeSpaceH   = 0;
        }
        uret = 0;
    } else {
        uret = MTP_RC_INVALID_STORAGE_ID;
    }
    return uret;
}

/* Operation code : GetNumObjects 0x1006 */
static UINT32 object_number_get(ULONG FormatCode, ULONG Association, ULONG *ObjectNumber)
{
    UINT32 uret = 0;

    if (FormatCode != 0U) {
        AppUsb_PrintUInt5("object_number_get(): Format Code = 0x%X is not supported", FormatCode, 0, 0, 0, 0);
        // we don't support FormatCode
        uret = MTP_RC_FORMAT_UNSUPPORTED;
    } else {
        *ObjectNumber = AppMtpd_DbNumberOfObjectsGet(FormatCode, Association);
    }

    //dbg("%s(): Format Code = 0x%X, Association = 0x%X, ObjectNumber = %d", __func__, FormatCode, Association, *ObjectNumber);
    return uret;
}

#define MAX_MTP_HANDLE_ARRAY_SIZE (4U + MTP_MAX_HANDLES) // 12 bytes for MTP header, 4 bytes for array header

/* Operation code : GetObjectHandles 0x1007 */
static UINT32 object_handles_get(ULONG FormatCode, ULONG Association, ULONG **Array, ULONG *HandlesNumber)
{
    UINT32 TargetNum;
    UINT32 uret = 0;
    UINT32 Count;
    static UINT32 ArrayBuffer[MAX_MTP_HANDLE_ARRAY_SIZE] __attribute__ ((aligned(64))) __attribute__((section(".bss.noinit")));
    const  UINT32 *u32_ptr;
    ULONG  *ul_ptr;

    if (FormatCode != 0U) {
        // we don't support FormatCode
        uret = MTP_RC_FORMAT_UNSUPPORTED;
    } else {
        TargetNum = AppMtpd_DbNumberOfObjectsGet(FormatCode, Association);

        if (TargetNum > MTP_MAX_HANDLES) {
            TargetNum = MTP_MAX_HANDLES;
        }

        u32_ptr = &ArrayBuffer[0];
        AmbaMisra_TypeCast(&ul_ptr, &u32_ptr);
        *Array         = ul_ptr;
        *HandlesNumber = TargetNum;

        AppUsb_PrintUInt5("object_handles_get(): Format Code = 0x%X, Association = 0x%X, ObjectNumber = %d",
                          FormatCode,
                          Association,
                          TargetNum,
                          0,
                          0);

        // first 12 bytes are for MTP header
        Count = AppMtpd_DbObjectHandlesFill(FormatCode, Association, &ArrayBuffer[3]);
        if (Count != TargetNum) {
            // need to check it?
            AppUsb_PrintUInt5("object_handles_get(): mismatch object count, target %d, real %d", TargetNum, Count, 0, 0, 0);
        }
    }

    return uret;
}

/* Operation code : GetObjectInfo 0x1008 */
static UINT32 object_info_get(ULONG ObjectHandle, USBD_MTP_OBJECT_s **object)
{
    UINT status = AppMtpd_ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        status = AppMtpd_DbObjectInfoGet(ObjectHandle, object);
        if (status == USB_ERR_SUCCESS) {
            //AppUsb_PrintUInt5("object_info_get(): Handle %d, CompressedSize = %d", ObjectHandle, (*object)->CompressedSize, 0, 0, 0);
            if ((send_object_info.Status & APP_MTPD_SEND_OBJECT_START) != 0U) {
                AppUsb_PrintUInt5("object_info_get(): object send start, handle = %d, %d, size %d",
                                  ObjectHandle,
                                  send_object_info.Handle,
                                  send_object_info.ObjectInfo.CompressedSize,
                                  0,
                                  0);
                if (send_object_info.Handle == ObjectHandle) {
                    // send object info is on going, overwrite the correct file size
                    (*object)->CompressedSize = send_object_info.ObjectInfo.CompressedSize;
                    AppUsb_PrintUInt5("object_info_get(): object length = %d", (*object)->CompressedSize, 0, 0, 0, 0);
                }
            }
        } else {
            status = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    } else {
        status = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return status;
}

static VOID transfer_canceled(void)
{
    UINT32 file_status;
    UINT32 uret;

    AppUsb_PrintUInt5("Transfer Canceled", 0, 0, 0, 0, 0);

    if (g_mtp_class_info.FpArray != NULL) {
        /* close the file has been canceled*/
        file_status = AmbaFS_FileClose(g_mtp_class_info.FpArray);
        if (file_status != 0U) {
            // need to process it?
        }
        g_mtp_class_info.FpArray           = NULL;
        g_mtp_class_info.OjectHandleOpened = 0;
    }

    if (send_object_info.File != NULL) {
        // Close file for SendObject command
        file_status = AmbaFS_FileClose(send_object_info.File);
        if (file_status != 0U) {
            // need to process it?
        }

        send_object_info.File = NULL;

        // delete and remove it from DataBase
        uret = device_delete(send_object_info.Handle);
        if (uret != USB_ERR_SUCCESS) {
            // need to process it?
        }
    }
}

/*Operation code : GetThumb 0x100A*/
static UINT32 object_thumb_get(ULONG ObjectHandle, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength)
{
    (void)ObjectHandle;
    (void)ObjectBuffer;
    (void)ObjectOffset;
    (void)ObjectLengthRequested;
    (void)ObjectActualLength;
    /* SSP do not support DCF, remove paser part. */
    return (MTP_RC_INVALID_OBJECT_HANDLE);
}
static UINT64 object_read_offset __attribute__((section(".bss.noinit")));

static UINT32 object_data_get(ULONG ObjectHandle, UCHAR *ObjectBuffer, UINT64 ObjectOffset, UINT64 ObjectLengthRequested, UINT64 *ObjectActualLength, USHORT OpCode)
{
    UINT32 status;
    UINT32 StatusClose;
    UINT64 ReadLen;
    UCHAR tmp_buffer;

    (VOID)OpCode;

    //AppUsb_PrintUInt5("object_data_get(): RequestLength = %u", (UINT32)ObjectLengthRequested, 0, 0, 0, 0);

    //To keep misra-c happy
    tmp_buffer = ObjectBuffer[0];
    ObjectBuffer[0] = tmp_buffer;

    /* Check the object handle. It must be in the local array.  */
    status = AppMtpd_ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        /* We are either at the beginning of the transfer or continuing the transfer.
           Check of the filex array handle exist already.  */
        if (g_mtp_class_info.FpArray == NULL) {
            static char FullFileName[MTP_MAX_FILENAME_LENGTH];

            /* File not yet opened for this object.  Open the file. */
            if (AppMtpd_DbFilePathFromHandleGet(ObjectHandle, FullFileName) == USB_ERR_SUCCESS) {
                (void)AmbaFS_FileOpen((const char*)FullFileName, "r", &g_mtp_class_info.FpArray);
                AppUsb_PrintStr5("object_data_get(): open %s", FullFileName, NULL, NULL, NULL, NULL);
                object_read_offset = 0;

                if (g_mtp_class_info.FpArray == NULL) {
                    AppUsb_PrintStr5("object_data_get(): Failed to open %s", FullFileName, NULL, NULL, NULL, NULL);
                    status = MTP_RC_OBJECT_NOT_OPENED;
                } else {
                    g_mtp_class_info.OjectHandleOpened = ObjectHandle;
                }
            } else {
                status = MTP_RC_INVALID_OBJECT_HANDLE;
            }
        }

        if (status == USB_ERR_SUCCESS) {
            //dbg("%s(): Handle %d, Offset = %lld, Length = %lld, OpCode = 0x%X", __func__, ObjectHandle, ObjectOffset, ObjectLengthRequested, OpCode);

            /* Do sanity check.  */
            if (ObjectOffset != object_read_offset) {
                AppUsb_PrintUInt5("object_data_get(): Offset mismatch ", 0, 0, 0, 0, 0);
                /* Move file I/O pointer by object offset */
                if (AmbaFS_FileSeek(g_mtp_class_info.FpArray, (INT64)ObjectOffset, (INT32)AMBA_FS_SEEK_START) != 0U) {
                    AppUsb_PrintUInt5("object_data_get(): File seek error", 0, 0, 0, 0, 0);
                    status = MTP_RC_OBJECT_SEEK_FAILED;
                } else {
                    object_read_offset = ObjectOffset;
                }
            }
        }

        if (status == USB_ERR_SUCCESS) {
            /* Read from the file into the media buffer.  */
            {
                UINT32 size_c;
                (void)AmbaFS_FileRead(ObjectBuffer, 1U, (UINT32)ObjectLengthRequested, g_mtp_class_info.FpArray, &size_c);
                ReadLen = size_c;
            }
            if (ReadLen == ObjectLengthRequested) {
                status = USB_ERR_SUCCESS;
            } else {
                AppUsb_PrintUInt5("object_data_get(): File read error, request %u bytes but read %u.", (UINT32)ObjectLengthRequested, (UINT32)ReadLen, 0, 0, 0);
                status = MTP_RC_ACCESS_DENIED;
            }

            *ObjectActualLength = ReadLen;
            object_read_offset   += ReadLen;

            /* Check if we have read the entire file.  */
            if (AmbaFS_FileEof(g_mtp_class_info.FpArray) != 0U) {
                /* This is the end of the transfer for the object. Close it.  */
                StatusClose                    = AmbaFS_FileClose(g_mtp_class_info.FpArray);
                g_mtp_class_info.FpArray           = NULL;
                g_mtp_class_info.OjectHandleOpened = 0;
                AppUsb_PrintUInt5("object_data_get(): close file", 0, 0, 0, 0, 0);

                if ((StatusClose == 0U) && (status == USB_ERR_SUCCESS)) {
                    status = 0;
                } else {
                    /* If status is error. we return status. If StatusClose is error we return StatusClose.  */
                    if (status == USB_ERR_SUCCESS) {
                        status = MTP_RC_ACCESS_DENIED;
                    }
                }
            }
        }
    } else {
        status = MTP_RC_INVALID_OBJECT_HANDLE;
    }

    return status;
}

/* Operation code : SendObjectInfo 0x100C */
static UINT32 object_info_send(USBD_MTP_OBJECT_s *Object, ULONG StorageId, ULONG ParentObjectHandle, ULONG *ObjectHandle)
{
    char str_asc[MTP_MAX_FILENAME_LENGTH] = { '\0' };
    UINT32 status;
    UINT32 fs_status;
    UINT32 tmp_storage_id;

    (void)StorageId;

    //To keep misra-c happy...
    tmp_storage_id = Object->StorageId;
    Object->StorageId = tmp_storage_id;

    AppUsb_PrintUInt5("object_info_send(): Object Info:", 0, 0, 0, 0, 0);

    AppMtpd_Uni2AscString(Object->Filename, (UINT8 *)str_asc);

    AppUsb_PrintUInt5("    ParentObjectHandle: 0x%X", ParentObjectHandle, 0, 0, 0, 0);
    AppUsb_PrintStr5("    Name: %s", str_asc, NULL, NULL, NULL, NULL);
    AppUsb_PrintUInt5("    FormatCode: 0x%X", Object->Format, 0, 0, 0, 0);
    AppUsb_PrintUInt5("    Length: 0x%X", Object->CompressedSize, 0, 0, 0, 0);

    if (AppMtpd_IsOperationSupported(MTP_OC_SEND_OBJECT_INFO) == 0U) {
        AppUsb_PrintUInt5("object_info_send(): not supported operation.", 0, 0, 0, 0, 0);
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        if (g_mtp_class_info.HandleNumber < MTP_MAX_HANDLES) {
            static char filename_asc[MTP_MAX_FILENAME_LENGTH];
            if ((ParentObjectHandle == 0xFFFFFFFFU) || (ParentObjectHandle == 0U)) {
                filename_asc[0] = (char)g_mtp_class_info.DeviceInfo.RootPath[0];
                filename_asc[1] = ':';
                filename_asc[2] = '\0';
            } else {
                // get parent file name
                (VOID)AppMtpd_DbFilePathFromHandleGet(ParentObjectHandle, filename_asc);
            }

            filename_asc[MTP_MAX_FILENAME_LENGTH - 1U] = '\0';

            AppUsb_PrintStr5("    Parent Name: %s", filename_asc, NULL, NULL, NULL, NULL);

            AmbaUtility_StringAppend(filename_asc, MTP_MAX_FILENAME_LENGTH, "\\");
            AmbaUtility_StringAppend(filename_asc, MTP_MAX_FILENAME_LENGTH, str_asc);

            AppUsb_PrintStr5("    Full Name: %s", filename_asc, NULL, NULL, NULL, NULL);

            if (Object->Format == MTP_OFC_ASSOCIATION) {
                fs_status = AppUsb_MakeDir((const char *)filename_asc);
                if (fs_status != 0U) {
                    AppUsb_PrintStr5("object_info_send(): Can not create %s", filename_asc, NULL, NULL, NULL, NULL);
                    status = MTP_RC_STORE_NOT_AVAILABLE;
                } else {
                    *ObjectHandle = AppMtpd_DbHandleInsert(filename_asc, Object, ParentObjectHandle);
                    g_mtp_class_info.HandleNumber++;
                    AppMtpd_DbCachedObjectInfoReset(*ObjectHandle);
                    status = MTP_RC_OK;
                }
            } else {
                *ObjectHandle = AppMtpd_DbHandleInsert(filename_asc, Object, ParentObjectHandle);
                if (*ObjectHandle == 0U) {
                    AppUsb_PrintStr5("object_info_send(): can't add handle for %s.",  filename_asc, NULL, NULL, NULL, NULL);
                    status = MTP_RC_ACCESS_DENIED;
                } else {
                    AppUsb_PrintUInt5("object_info_send(): add handle %d.", *ObjectHandle, 0, 0, 0, 0);
                    AppUsb_PrintStr5("object_info_send(): handle name = %s.", filename_asc, NULL, NULL, NULL, NULL);

                    send_object_info.Status     = APP_MTPD_SEND_OBJECT_START;
                    // avoid to use structure assignment
                    //send_object_info.ObjectInfo = *Object;
                    {
                        extern UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num);
                        (VOID)AmbaWrap_memcpy(&(send_object_info.ObjectInfo), Object, sizeof(USBD_MTP_OBJECT_s));
                    }
                    send_object_info.Handle     = *ObjectHandle;
                    send_object_info.FileOffset = 0;
                    g_mtp_class_info.HandleNumber++;
                    AppMtpd_DbCachedObjectInfoReset(*ObjectHandle);
                    AppMtpd_DbCachedObjectSizeSet(*ObjectHandle, (UINT64)Object->CompressedSize);
                    status = MTP_RC_OK;
                }
            }
        } else {
            status = MTP_RC_STORE_FULL;
        }
    }
    /* No more space for handle. Return storage full.  */
    return status;
}

/* Operation code : SendObject 0x100D */
static UINT32 object_data_send(ULONG Handle, ULONG Phase, const UCHAR *Buffer, UINT64 Offset, UINT64 Length)
{
    UINT64 size;
    UINT32 uret = 0;
    UINT32 file_status;
    static char file_name_buffer[1024];

    //AppUsb_Print("%s(): Handle %d, Offset %lld, Length %lld, phase %d", __FUNCTION__, Handle, Offset, Length, Phase);

    if ((Phase == 1U) || (Phase == 2U)) {
        if (send_object_info.File != NULL) {
            file_status = AmbaFS_FileClose(send_object_info.File);
            if (file_status != 0U) {
                // need to process it?
            }
            send_object_info.File = NULL;
        }
        send_object_info.Status = 0;
    } else {
        if ((send_object_info.File == NULL) && (Offset != 0U)) {
            AppUsb_PrintUInt5("object_data_send(): File is NULL but offset is 0, something wrong.", 0, 0, 0, 0, 0);
            uret = MTP_RC_GENERAL_ERROR;
        } else {
            if ((send_object_info.File == NULL) && (Offset == 0U)) {
                // open file for write
                char *path = file_name_buffer;
                if (AppMtpd_DbFilePathFromHandleGet(Handle, path) != USB_ERR_SUCCESS) {
                    uret = MTP_RC_INVALID_OBJECT_HANDLE;
                }

                if (uret == 0U) {
                    (void)AmbaFS_FileOpen(path, "w", &send_object_info.File);
                    if (send_object_info.File == NULL) {
                        AppUsb_PrintStr5("object_data_send(): can't open %s.", path, NULL, NULL, NULL, NULL);
                        uret = MTP_RC_GENERAL_ERROR;
                    }
                }
            }

            if (uret == 0U) {
                if (send_object_info.File == NULL) {
                    AppUsb_PrintUInt5("object_data_send(): File is NULL, something wrong.", 0, 0, 0, 0, 0);
                    uret = MTP_RC_GENERAL_ERROR;
                }

                if (send_object_info.FileOffset != Offset) {
                    AppUsb_PrintUInt5("object_data_send(): offset is wrong.", 0, 0, 0, 0, 0);
                    uret = MTP_RC_GENERAL_ERROR;
                }

                if (uret == 0U) {
                    {
                        UINT32 size_c;
                        UCHAR *object_buffer = NULL;
                        AmbaMisra_TypeCast32(&object_buffer, &Buffer);
                        (void)AmbaFS_FileWrite(object_buffer, 1U, (UINT32)Length, send_object_info.File, &size_c);
                        size = size_c;
                    }
                    if (size != Length) {
                        AppUsb_PrintUInt5("object_data_send(): can't write file, request %u bytes but write %u.", (UINT32)Length, (UINT32)size, 0, 0, 0);
                        uret = MTP_RC_GENERAL_ERROR;
                    } else {
                        send_object_info.FileOffset += Length;
                    }
                }
            }
        }
    }

    return uret;
}

/* Operation code : GetObjectReferences 0x9810 */
static UINT32 object_reference_get(ULONG ObjectHandle, UCHAR **ReferenceArray, ULONG *ReferenceArrayLength)
{
    static UINT32 references_array = 0;
    UINT32 status;
    UINT32 uret;

    status = AppMtpd_ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        references_array = 0;
        *ReferenceArray = (UINT8 *) &references_array;
        *ReferenceArrayLength = sizeof(UINT32);
        uret = 0;
    } else {
        uret = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return uret;
}

/* Operation code : SetObjectReferences 0x9811 */
static UINT32 object_reference_set(ULONG ObjectHandle, UCHAR *ReferenceArray, ULONG ReferenceArrayLength)
{
    UINT32 status;
    UINT32 uret;

    (VOID)ReferenceArray;
    (VOID)ReferenceArrayLength;

    status = AppMtpd_ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        // application can do something here.
        uret = 0;
    } else {
        uret = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return uret;
}

static UINT32 usb_system_init(void)
{
    // PIMA MTP names
    static UINT8 vendor_name[]  = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x00 }; //"Ambarella";
    static UINT8 product_name[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                    0x4D, 0x54, 0x50, 0x20, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65,
                                    0x00
                                  };                                                       //"Ambarella MTP Device";
    static UINT8 serial_number[]  = { 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x30, 0x00 };           //"1.0.0.0";
    static UINT8 version_string[] = { 0x56, 0x31, 0x2E, 0x30, 0x00 };                             //"V1.0";

    // PIMA MTP storage names
    static UINT8 volume_description[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                          0x4D, 0x54, 0x50, 0x20, 0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74,
                                          0x20, 0x44, 0x69, 0x73, 0x6B, 0x20, 0x56, 0x6F, 0x6C, 0x75,
                                          0x6D, 0x65, 0x00
                                        };                               //"Ambarella MTP Client Disk Volume";
    static UINT8 volume_label[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                    0x4D, 0x54, 0x50, 0x20, 0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74,
                                    0x20, 0x53, 0x44, 0x20, 0x73, 0x6C, 0x6F, 0x74, 0x00
                                  }; // "Ambarella MTP Client SD slot";

    // Vendor extension info
    static UCHAR vendor_ext_desc[] = { 0x6D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x2E,
                                       0x63, 0x6F, 0x6D, 0x3A, 0x20, 0x31, 0x2E, 0x30, 0x3B, 0x20,
                                       0x20, 0x6D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74,
                                       0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x57, 0x4D, 0x50, 0x50, 0x44,
                                       0x3A, 0x20, 0x31, 0x31, 0x2E, 0x30, 0x3B, 0x20, 0x6D, 0x69,
                                       0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x2E, 0x63, 0x6F,
                                       0x6D, 0x2F, 0x57, 0x4D, 0x50, 0x50, 0x44, 0x3A, 0x20, 0x31,
                                       0x30, 0x2E, 0x30, 0x3B, 0x20, 0x6D, 0x69, 0x63, 0x72, 0x6F,
                                       0x73, 0x6F, 0x66, 0x74, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x57,
                                       0x4D, 0x44, 0x52, 0x4D, 0x50, 0x44, 0x3A, 0x20, 0x31, 0x30,
                                       0x2E, 0x31, 0x3B, 0x00
                                     }; //"microsoft.com: 1.0;  microsoft.com/WMPPD: 11.0; microsoft.com/WMPPD: 10.0; microsoft.com/WMDRMPD: 10.1;";

    static UINT16 capture_format_supported[] = {
        MTP_OFC_UNDEFINED,
        MTP_OFC_EXIF_JPEG,
        0
    };

    static UINT16 event_supported[] = {
        MTP_EC_CANCEL_TRANSACTION,
        MTP_EC_OBJECT_ADDED,
        MTP_EC_OBJECT_REMOVED,
        MTP_EC_STORE_ADDED,
        MTP_EC_STORE_REMOVED,
        MTP_EC_DEVICE_PROP_CHANGED,
        MTP_EC_OBJECT_INFO_CHANGED,
        MTP_EC_DEVICE_INFO_CHANGED,
        MTP_EC_REQUEST_OBJECT_TRANSFER,
        MTP_EC_STORE_FULL,
        MTP_EC_DEVICE_RESET,
        MTP_EC_STORAGE_INFO_CHANGED,
        MTP_EC_CAPTURE_COMPLETE,
        MTP_EC_UNREPORTED_STATUS,
        MTP_EC_VENDOR_AMBA_TEST,
        0
    };


    UINT32 uret;

    g_mtp_class_info.DeviceInfo.DeviceInfoVendorName       = vendor_name;
    g_mtp_class_info.DeviceInfo.DeviceInfoProductName      = product_name;
    g_mtp_class_info.DeviceInfo.DeviceInfoSerialNo         = serial_number;
    g_mtp_class_info.DeviceInfo.DeviceInfoVersion          = version_string;
    g_mtp_class_info.DeviceInfo.VolumeDescription          = volume_description;
    g_mtp_class_info.DeviceInfo.VolumeLabel                = volume_label;
    g_mtp_class_info.DeviceInfo.RootPath[0]                = 0x63; //'c'
    g_mtp_class_info.DeviceInfo.RootPath[1]                = 0x3A; //':'
    g_mtp_class_info.DeviceInfo.RootPath[2]                = 0x5C; //'\\';
    g_mtp_class_info.DeviceInfo.RootPath[3]                = 0;
    g_mtp_class_info.DeviceInfo.StorageId                  = MTP_STORAGE_ID;
    g_mtp_class_info.DeviceInfo.StorageType                = MTP_STC_FIXED_RAM;
    g_mtp_class_info.DeviceInfo.FileSystemType             = MTP_FSTC_DCF;
    g_mtp_class_info.DeviceInfo.AccessCapability           = MTP_AC_READ_WRITE;
    g_mtp_class_info.DeviceInfo.MaxCapacityLow             = 0;
    g_mtp_class_info.DeviceInfo.MaxCapacityHigh            = 0;
    g_mtp_class_info.DeviceInfo.FreeSpaceLow               = 0;
    g_mtp_class_info.DeviceInfo.FreeSpaceHigh              = 0;
    g_mtp_class_info.DeviceInfo.FreeSpaceImage             = 0xFFFFFFFFU;
    g_mtp_class_info.DeviceInfo.DeviceSupportProp          = AppMtpd_SupportedDevPropListGet();
    g_mtp_class_info.DeviceInfo.DeviceSupportCaptureFormat = capture_format_supported;
    g_mtp_class_info.DeviceInfo.DeviceSupportImgFormat     = object_format_supported;
    g_mtp_class_info.DeviceInfo.ObjectSupportProp          = object_property_supported;
    g_mtp_class_info.DeviceInfo.OperationSupportList       = OperationSupportList;
    g_mtp_class_info.DeviceInfo.EventSupportList           = event_supported;
    g_mtp_class_info.DeviceInfo.PropDescGet                = AppMtpd_CbDevicePropDescGet;
    g_mtp_class_info.DeviceInfo.PropValueGet               = AppMtpd_CbDevicePropValueGet;
    g_mtp_class_info.DeviceInfo.PropValueSet               = AppMtpd_CbDevicePropValueSet;
    g_mtp_class_info.DeviceInfo.PropValueReset             = AppMtpd_CbDevicePropValueReset;
    g_mtp_class_info.DeviceInfo.StorageFormat              = storage_format;
    g_mtp_class_info.DeviceInfo.ObjectDelete               = device_delete;
    g_mtp_class_info.DeviceInfo.DeviceReset                = device_reset;
    g_mtp_class_info.DeviceInfo.StorageInfoGet             = storage_info_get;
    g_mtp_class_info.DeviceInfo.ObjectNumberGet            = object_number_get;
    g_mtp_class_info.DeviceInfo.ObjectHandlesGet           = object_handles_get;
    g_mtp_class_info.DeviceInfo.ObjectInfoGet              = object_info_get;
    g_mtp_class_info.DeviceInfo.ObjectDataGet              = object_data_get;
    g_mtp_class_info.DeviceInfo.ObjectThumbGet             = object_thumb_get;
    g_mtp_class_info.DeviceInfo.ObjectInfoSend             = object_info_send;
    g_mtp_class_info.DeviceInfo.ObjectDataSend             = object_data_send;
    g_mtp_class_info.DeviceInfo.ObjectPropDescGet          = AppMtpd_CbObjectPropDescGet;
    g_mtp_class_info.DeviceInfo.ObjectPropValueGet         = AppMtpd_CbObjectPropValueGet;
    g_mtp_class_info.DeviceInfo.ObjectPropValueSet         = AppMtpd_CbObjectPropValueSet;
    g_mtp_class_info.DeviceInfo.ObjectReferenceGet         = object_reference_get;
    g_mtp_class_info.DeviceInfo.ObjectReferenceSet         = object_reference_set;
    g_mtp_class_info.DeviceInfo.ObjectPropListGet          = AppMtpd_CbObjectPropListGet;
    g_mtp_class_info.DeviceInfo.ObjectClearAll             = object_clear_all;
    g_mtp_class_info.DeviceInfo.ObjectCustomCommand        = AppMtpd_CbAmageCommunication;
    g_mtp_class_info.DeviceInfo.ObjectCustomDataGet        = AppMtpd_CbAmageDataGet;
    g_mtp_class_info.DeviceInfo.ObjectCustomDataSend       = AppMtpd_CbAmageDataSend;
    g_mtp_class_info.DeviceInfo.TransferCanceled           = transfer_canceled;
    g_mtp_class_info.DeviceInfo.VendorExtensionID          = APP_MTPD_EXTENSION_ID;
    g_mtp_class_info.DeviceInfo.VendorExtensionVersion     = APP_MTPD_EXTENSION_VERSION;
    g_mtp_class_info.DeviceInfo.VendorExtensionDesc        = vendor_ext_desc;

    uret = AmbaUSBD_MTPSetInfo(&g_mtp_class_info.DeviceInfo);
    if (uret == USB_ERR_SUCCESS) {
        AppMtpd_DevicePropertiesInit(&g_mtp_class_info.DeviceProps);
        AppMtpd_DbDriveSet(g_mtp_class_info.DeviceInfo.RootPath[0]);
    }

    return uret;
}

static UINT32 mtp_objects_load(void)
{
    UINT32 uret;
    UINT32 t1, t2;

    t1 = tx_time_get();
    // add database for object info
    uret = AppMtpd_DbScan();
    t2   = tx_time_get();
    AppUsb_PrintUInt5("MTP object scan takes %d ms", t2 - t1, 0, 0, 0, 0);

    if (uret == USB_ERR_SUCCESS) {
        g_mtp_class_info.HandleNumber = AppMtpd_DbTotalHandleNumberGet();
        AppUsb_PrintUInt5("Total objects = %d", g_mtp_class_info.HandleNumber, 0, 0, 0, 0);
    }

    return uret;
}

static void init_zero(void)
{
    AppUsb_MemoryZeroSet(&g_mtp_class_info, sizeof(g_mtp_class_info));
    object_read_offset = 0;
    AppMtpd_DbInitZero();
    return;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Get the storage drive letter.
 * @param StorageId MTP Storage ID
*/
char AppMtpd_StorageDriveGet(UINT32 StorageId)
{
    (VOID)StorageId;
    return (char)g_mtp_class_info.DeviceInfo.RootPath[0];
}
/**
 * Check if the object foramt is supported.
 * @param Format object format
*/
UINT32 AppMtpd_IsObjectFormatSupported(UINT16 Format)
{
    UINT32 count = sizeof(object_format_supported) / sizeof(UINT16);
    UINT32 i;
    UINT32 uret = 0;

    for (i = 0; i < count; i++) {
        if (object_format_supported[i] == Format) {
            uret = 1;
            break;
        }
    }
    return uret;
}
/**
 * Check if the MTP operation is supported.
 * @param Opcpde MTP operation code
*/
UINT32 AppMtpd_IsOperationSupported(UINT16 Opcode)
{
    UINT32 count = sizeof(OperationSupportList) / sizeof(UINT16);
    UINT32 i;
    UINT32 uret = 0;

    for (i = 0; i < count; i++) {
        if (OperationSupportList[i] == Opcode) {
            uret = 1;
            break;
        }
    }
    return uret;
}

/**
 * Check if the property with the object format is supported.
 * @param Format object format
 * @param Property object property
*/
UINT32 AppMtpd_IsObjectPropSupported(UINT16 Format, UINT16 Property)
{
    const UINT16 *ptr = object_property_supported;
    UINT16 of         = 0;
    UINT16 count      = 0;
    UINT16 i;
    UINT32 uret     = 0;
    UINT32 idx      = 0;
    UINT32 is_break = 0;

    do {
        if (ptr[idx] == 0U) {
            uret     = 0;
            is_break = 1;
        } else {
            of = ptr[idx];
            idx++;
            count = ptr[idx];
            idx++;
            if (of == Format) {
                for (i = 0; i < count; i++) {
                    if (ptr[idx] == Property) {
                        uret = 1;
                        break;
                    }
                    idx++;
                }
                is_break = 1;
            } else {
                idx += count;
            }
        }

        if (is_break == 1U) {
            break;
        }
    } while (ptr[idx] != 0U);

    return uret;
}
/**
 * convert 1 UINT32 data into 2 UINT8 data with little endian format.
 * higher 2-byte is ignored.
*/
UINT32  AppMtpd_ShortPut(UINT8 * Address, ULONG Value)
{
    *Address = (UINT8) (Value & 0xFFU);
    Address++;
    *Address = (UINT8) ((Value >> 8U) & 0xFFU);
    return 2;
}

/**
 * convert 1 UINT32 data into 4 UINT8 data with little endian format.
*/
UINT32  AppMtpd_LongPut(UINT8 * Address, ULONG Value)
{
    *Address = (UINT8) (Value & 0x0FFU);
    Address++;
    *Address = (UINT8) ((Value >> 8U) & 0x0FFU);
    Address++;
    *Address = (UINT8) ((Value >> 16U) & 0x0FFU);
    Address++;
    *Address = (UINT8) ((Value >> 24U) & 0x0FFU);
    return 4;
}

/**
 * convert 4 UINT8 data into UINT32 data with little endian format.
*/
UINT32  AppMtpd_LongGet(const UINT8 *Address)
{
    UINT32 value;

    value  = ((UINT32)Address[0] & 0x0FFU);
    value += ((UINT32)Address[1] & 0x0FFU) << 8U;
    value += ((UINT32)Address[2] & 0x0FFU) << 16U;
    value += ((UINT32)Address[3] & 0x0FFU) << 24U;
    return value;
}

/**
 * Convert unicode string to ascii string.
 * @param Uni [Input] unicode string
 * @param Pascal [Output] ascii string
*/
VOID  AppMtpd_Uni2AscString(const UINT8 *Uni, UINT8 *Asc)
{
    UINT32 string_length;

    string_length = (UINT32)(*Uni);
    Uni++;

    while (string_length != 0U) {
        *Asc = *Uni;
        Asc++;
        Uni++;
        Uni++;
        string_length--;
    }
    *Asc = 0;
    return;
}

/**
 * Convert ascii string to MTP pascal string.
 * @param Asc [Input] ascii string
 * @param Pascal [Output] mtp pascal string
*/
VOID  AppMtpd_Asc2PascalString(const char *Asc, UINT8 *Pascal)
{
    UINT32 string_length = AmbaUtility_StringLength(Asc);

    if (string_length == 0U) {
        *Pascal = 0;
    } else {
        *Pascal = (UINT8)(string_length + 1U);
        Pascal++;

        while (string_length != 0U) {
            *Pascal = (UINT8)*Asc;
            Pascal++;
            Asc++;
            *Pascal = 0;
            Pascal++;
            string_length--;
        }
        *Pascal = 0;
        Pascal++;
        *Pascal = 0;
    }
    return;
}

/**
 * Convert MTP pascal string to ascii string.
 * @param Pascal [Input] mtp pascal string
 * @param Asc    [Output] ascii string
*/
VOID  AppMtpd_Pascal2AscString(const UINT8 *Pascal, char *Asc)
{
    UINT32 pascal_idx = 0;
    UINT32 asc_idx = 0;
    UINT32 string_length = Pascal[pascal_idx];

    pascal_idx++;

    if (string_length == 0U) {
        Asc[asc_idx] = '\0';
    } else {
        while (string_length != 0U) {
            Asc[asc_idx] = (char)Pascal[pascal_idx];
            asc_idx++;
            pascal_idx += 2U;
            string_length--;
        }
        Asc[asc_idx] = '\0';
    }
    return;
}

/**
 * Return the c-string length for the UINT8 string array.
*/
UINT32 AppMtpd_UInt8StringLengthGet(const UINT8 *String)
{
    UINT32 count = 0;

    while (String[count] != 0U) {
        count++;
    }
    return count;
}
/**
 * Extract only file name from the full file path.
 * For example, 'c:\temp\1.jpg' should have '1.jpg' extracted in FileName parameter.
*/
VOID  AppMtpd_FileNameExtract(const char *Path, char *FileName)
{
    UINT32 string_length = AmbaUtility_StringLength(Path);
    UINT32 idx;

    idx          = string_length;

    if (string_length == 0U) {
        *FileName = '\0';
    } else {
        while (Path[idx - 1U] != '\\') {
            idx--;
            if (idx == 0U) {
                break;
            }
        }
        if (idx == 0U) {
            AppUsb_PrintUInt5("AppMtpd_FileNameExtract(): NG", 0, 0, 0, 0, 0);
            *FileName = '\0';
        } else {
            UINT32 len = string_length - idx;
            AppUsb_MemoryCopy(FileName, &Path[idx], len);
            FileName[len] = '\0';
        }
    }
    return;
}
/**
 * Check if the object handle exists
 * @retval 0 the object handle exists
 * @retval MTP_RC_INVALID_OBJECT_HANDLE the object handle doesn't exist
*/
UINT32 AppMtpd_ObjectHandleCheck(ULONG Handle)
{
    UINT32 status = MTP_RC_INVALID_OBJECT_HANDLE;

    if (AppMtpd_DbIsObjectHandleValid(Handle) == 1U) {
        status = USB_ERR_SUCCESS;
    }

    return status;
}

/**
 * Return the property array pointer of the object format.
*/
UINT16 *AppMtpd_ObjectPropListGet(UINT16 Format)
{
    UINT16 *ptr = object_property_supported;
    UINT16 count;
    UINT32 idx   = 0;
    UINT16 *ptr_return = NULL;

    while (ptr[idx] != 0U) {
        if (ptr[idx] == Format) {
            ptr_return = &ptr[idx];
            break;
        } else {
            idx++;
            count = ptr[idx];
            idx   = idx + ((UINT32)count + 1U);
        }
    }
    return ptr_return;
}
/**
 * Initialize MTP device application.
*/
UINT32 AppMtpd_Init(void)
{
    UINT32 rval;

    init_zero();
    rval = usb_system_init();
    if (rval == USB_ERR_SUCCESS) {
        rval = mtp_objects_load();
    }
    return rval;
}

/**
 * A test function for sending MTP events. It is used for unittest. Don't use it in normal use case.
*/
void AppMtpd_AddEventTest(UINT32 Count)
{
    UINT32 i;
    UINT32 uret;
    USBD_MTP_EVENT_s event = {0};
    event.Code            = MTP_EC_VENDOR_AMBA_TEST;
    event.TransactionId   = 0x0004;
    event.ParameterNumber = 3;
    event.Parameter1      = 0x090A0B0C;
    event.Parameter2      = 0x05060708;
    event.Parameter3      = 0x01020304;
    for (i = 0; i < Count; i++) {
        uret = AmbaUSBD_MTPAddEvent(&event);
        if (uret == USBX_ERR_1003) {
            AppUsb_PrintUInt5("MTP Add Event Full", 0, 0, 0, 0, 0);
        } else if (uret != USB_ERR_SUCCESS) {
            AppUsb_PrintUInt5("MTP Add Event Fail 0x%X", uret, 0, 0, 0, 0);
        } else {
            // success
        }
    }
}

void AppMtpd_AddEventObjectAdded(void)
{
    UINT32 uret;
    USBD_MTP_EVENT_s event = {0};
    event.Code            = MTP_EC_OBJECT_ADDED;
    event.TransactionId   = 0x0;
    event.ParameterNumber = 1;
    event.Parameter1      = MTP_EVENT_OBJECT_HANDLE_TEST;
    uret = AmbaUSBD_MTPAddEvent(&event);
    if (uret == USBX_ERR_1003) {
        AppUsb_Print("MTP Add Event Full");
    } else if (uret != USB_ERR_SUCCESS) {
        AppUsb_PrintUInt5("MTP Add Event Fail 0x%X", uret, 0, 0, 0, 0);
    } else {
        // success
        AppUsb_Print("MTP Add Event Success");
    }
}

void AppMtpd_AddEventObjectRemoved(void)
{
    UINT32 uret;
    USBD_MTP_EVENT_s event = {0};
    event.Code            = MTP_EC_OBJECT_REMOVED;
    event.TransactionId   = 0x0;
    event.ParameterNumber = 1;
    event.Parameter1      = MTP_EVENT_OBJECT_HANDLE_TEST;
    uret = AmbaUSBD_MTPAddEvent(&event);
    if (uret == USBX_ERR_1003) {
        AppUsb_Print("MTP Add Event Full");
    } else if (uret != USB_ERR_SUCCESS) {
        AppUsb_PrintUInt5("MTP Add Event Fail 0x%X", uret, 0, 0, 0, 0);
    } else {
        // success
        AppUsb_Print("MTP Add Event Success");
    }
}


