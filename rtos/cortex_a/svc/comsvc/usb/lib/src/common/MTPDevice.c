/**
 *  @file MTPDevice.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details USB MTP device class functions for APP/MW.
 */

#include "AmbaPrint.h"
#include "AmbaSvcUsb.h"
#include "MTPDevice.h"
#include "AmbaUSB_ErrCode.h"
#include <AmbaFS.h>
#include <AmbaMisraFix.h>
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

#include "PlatformPort.h"

#include "MTPDataBase.c"
#include "MTPDeviceProp.c"
#include "MTPObjectProp.c"

#define APP_MTPD_EXTENSION_ID      0x6
#define APP_MTPD_EXTENSION_VERSION 0x64

#define APP_MTPD_SEND_OBJECT_START 0x01U

typedef struct {
    USBD_MTP_OBJECT_s ObjectInfo;
    UINT32            Handle;
    UINT32            Status;
    AMBA_FS_FILE *    File;
    UINT64            FileOffset;
} APP_MTPD_SEND_OBJECT_INFO_s;

static APP_MTPD_SEND_OBJECT_INFO_s send_object_info GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static void Uni2Asc(const UINT8 *Uni, UINT8 *Asc)
{
    const UINT8  *pSrc = Uni;
    UINT8        *pDst = Asc;
    UINT32       IdxS = 0U, IdxE = 0U, StringLength;

    StringLength = (UINT32)(pSrc[IdxS]);
    IdxS++;

    while (StringLength != 0U) {
        pDst[IdxE] = pSrc[IdxS];
        IdxE++;
        IdxS++;
        IdxS++;
        StringLength--;
    }
    pDst[IdxE] = 0;

    return;
}

/* Operation code : DeleteObject 0x100B */
static UINT32 CbObjectDelete(ULONG ObjectHandle)
{
    UINT32 status;

    if (IsOperationSupported(MTP_OC_DELETE_OBJECT) == 0U) {
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        status = ObjectHandleCheck(ObjectHandle);

        if (status == USB_ERR_SUCCESS) {
            status = DbDeleteObject(ObjectHandle);
            if (status == MTP_RC_OK) {
                status = 0;
                MtpClassInfo.HandleNumber--;
            }
        } else {
            status = MTP_RC_INVALID_OBJECT_HANDLE;
        }
    }

    return status;
}

/* Operation code : ResetDevice 0x1010 */
static UINT32 CbDeviceReset(void)
{
    return 0;
}

static void CbObjectClearAll(void)
{
    UINT32 sRet;

    // it's called when ONLY when class is going to release.
    //dbg("%s(): start", __func__);
    if (MtpClassInfo.FpArray != NULL) {
        /* close the file has been canceled*/
        sRet = AmbaFS_FileClose(MtpClassInfo.FpArray);
        if (sRet != 0U) {
            // need to process it?
        }
        MtpClassInfo.FpArray           = NULL;
        MtpClassInfo.OjectHandleOpened = 0U;
    }
    DbClean();
}

/* Operation code : FormatStore 0x100F */
static UINT32 CbStorageFormat(ULONG StorageId)
{
    UINT32 FsStatus;
    UINT32 uRet;

    if (IsOperationSupported(MTP_OC_DELETE_OBJECT) == 0U) {
        uRet = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        if (StorageId == MTP_STORAGE_ID) {
            /* Format SD card with default FAT32 format */
            FsStatus = AmbaFS_Format('d');
            if (FsStatus != 0U) {
                AmbaPrint_PrintUInt5("Failed to format", 0, 0, 0, 0, 0);
                uRet = MTP_RC_ACCESS_DENIED;
            } else {
                /* Reset the handle counter.  */
                MtpClassInfo.HandleNumber = 0;
                CbObjectClearAll();
                uRet = 0;
            }
        } else {
            uRet = MTP_RC_INVALID_STORAGE_ID;
        }
    }
    return uRet;
}

static char GetStorageDrive(ULONG StorageId)
{
    (void)StorageId;
    return (char)MtpClassInfo.DeviceInfo.RootPath[0];
}

/* Operation code : GetStorageInfo 0x1005 */
static UINT32 CbStorageInfoGet(ULONG StorageId, ULONG *MaxCapacityL, ULONG *MaxCapacityH, ULONG *FreeSpaceL, ULONG *FreeSpaceH)
{
    UINT32 uRet;

    if (StorageId == MtpClassInfo.DeviceInfo.StorageId) {
        APP_USB_DRIVE_INFO  dev_info;
        char drive = GetStorageDrive(StorageId);
        INT32 rval = AppUsb_GetDriveInfo((UINT8)drive - (UINT8)'a', &dev_info);

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
        uRet = 0;
    } else {
        uRet = MTP_RC_INVALID_STORAGE_ID;
    }
    return uRet;
}

/* Operation code : GetNumObjects 0x1006 */
static UINT32 CbObjectNumberGet(ULONG FormatCode, ULONG Association, ULONG *ObjectNumber)
{
    UINT32 uRet = 0;

    if (FormatCode != 0U) {
        AmbaPrint_PrintUInt5("CbObjectNumberGet(): Format Code is not supported", 0, 0, 0, 0, 0);
        // we don't support FormatCode
        uRet = MTP_RC_FORMAT_UNSUPPORTED;
    } else {
        *ObjectNumber = DbGetNumberOfObjects(FormatCode, Association);
    }

    //dbg("%s(): Format Code = 0x%X, Association = 0x%X, ObjectNumber = %d", __func__, FormatCode, Association, *ObjectNumber);
    return uRet;
}

#define MAX_MTP_HANDLE_ARRAY_SIZE (4U + MTP_MAX_HANDLES) // 12 bytes for MTP header, 4 bytes for array header

/* Operation code : GetObjectHandles 0x1007 */
static UINT32 CbObjectHandlesGet(ULONG FormatCode, ULONG Association, ULONG **Array, ULONG *HandlesNumber)
{
    static ULONG ArrayBuffer[MAX_MTP_HANDLE_ARRAY_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    UINT32  TargetNum;
    UINT32  uRet = 0;
    UINT32  Count;

    if (FormatCode != 0U) {
        // we don't support FormatCode
        uRet = MTP_RC_FORMAT_UNSUPPORTED;
    } else {
        TargetNum = DbGetNumberOfObjects(FormatCode, Association);

        if (TargetNum > MTP_MAX_HANDLES) {
            TargetNum = MTP_MAX_HANDLES;
        }

        *Array         = &(ArrayBuffer[0]);
        *HandlesNumber = TargetNum;

        // first 12 bytes are for MTP header
        Count = DbFillObjectHandles(FormatCode, Association, &ArrayBuffer[3]);
        if (Count != TargetNum) {
            // need to check it?
        }
    }

    return uRet;
}

/* Operation code : GetObjectInfo 0x1008 */
static UINT32 CbObjectInfoGet(ULONG ObjectHandle, USBD_MTP_OBJECT_s **object)
{
    UINT status = ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        status = DbGetObjectInfo(ObjectHandle, object);
        if (status == USB_ERR_SUCCESS) {
            if ((send_object_info.Status & APP_MTPD_SEND_OBJECT_START) != 0U) {
                AmbaPrint_PrintUInt5("CbObjectInfoGet(): object send start", 0U, 0U, 0U, 0U, 0U);

                if (send_object_info.Handle == ObjectHandle) {
                    // send object info is on going, overwrite the correct file size
                    (*object)->CompressedSize = send_object_info.ObjectInfo.CompressedSize;
                    AmbaPrint_PrintUInt5("CbObjectInfoGet(): object length = %d", (*object)->CompressedSize, 0, 0, 0, 0);
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

static void CbTransferCanceled(void)
{
    UINT32 FsStatus;
    UINT32 uRet;

    AmbaPrint_PrintUInt5("Transfer Canceled", 0, 0, 0, 0, 0);

    if (MtpClassInfo.FpArray != NULL) {
        /* close the file has been canceled*/
        FsStatus = AmbaFS_FileClose(MtpClassInfo.FpArray);
        if (FsStatus != 0U) {
            // need to process it?
        }
        MtpClassInfo.FpArray           = NULL;
        MtpClassInfo.OjectHandleOpened = 0U;
    }

    if (send_object_info.File != NULL) {
        // Close file for SendObject command
        FsStatus = AmbaFS_FileClose(send_object_info.File);
        if (FsStatus != 0U) {
            // need to process it?
        }

        send_object_info.File = NULL;

        // delete and remove it from DataBase
        uRet = CbObjectDelete(send_object_info.Handle);
        if (uRet != USB_ERR_SUCCESS) {
            // need to process it?
        }
    }
}

/*Operation code : GetThumb 0x100A*/
static UINT32 CbObjectThumbGet(ULONG ObjectHandle, UCHAR *ObjectBuffer, ULONG ObjectOffset, ULONG ObjectLengthRequested, ULONG *ObjectActualLength)
{
    AmbaMisra_TouchUnused(&ObjectHandle);
    AmbaMisra_TouchUnused(ObjectBuffer);
    AmbaMisra_TouchUnused(&ObjectOffset);
    AmbaMisra_TouchUnused(&ObjectLengthRequested);
    AmbaMisra_TouchUnused(ObjectActualLength);

    /* SSP do not support DCF, remove paser part. */
    return (MTP_RC_INVALID_OBJECT_HANDLE);
}

static UINT64 ObjectReadOffset GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static UINT32 CbObjectDataGetEx(ULONG ObjectHandle, UCHAR *ObjectBuffer, UINT64 ObjectOffset, UINT64 ObjectLengthRequested, UINT64 *ObjectActualLength, USHORT OpCode)
{
    UINT32 status, Rval;
    UINT32 StatusClose;
    UINT64 ReadLen = 0U;
    UCHAR tmp_buffer;

    (void)OpCode;

    //To keep misra-c happy
    tmp_buffer = ObjectBuffer[0];
    ObjectBuffer[0] = tmp_buffer;

    /* Check the object handle. It must be in the local array.  */
    status = ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        /* We are either at the beginning of the transfer or continuing the transfer.
           Check of the filex array handle exist already.  */
        if (MtpClassInfo.FpArray == NULL) {
            static char FullFileName[MTP_MAX_FILENAME_LENGTH];

            /* File not yet opened for this object.  Open the file. */
            if (DbGetFilePathFromHandle(ObjectHandle, FullFileName) == USB_ERR_SUCCESS) {
                Rval = AmbaFS_FileOpen((const char*)FullFileName, "r", &MtpClassInfo.FpArray);

                AmbaPrint_PrintStr5("CbObjectDataGetEx(): open %s", FullFileName, NULL, NULL, NULL, NULL);
                ObjectReadOffset = 0;

                if ((Rval != OK) || (MtpClassInfo.FpArray == NULL)) {
                    AmbaPrint_PrintStr5("CbObjectDataGetEx(): Failed to open %s", FullFileName, NULL, NULL, NULL, NULL);
                    status = MTP_RC_OBJECT_NOT_OPENED;
                } else {
                    MtpClassInfo.OjectHandleOpened = ObjectHandle;
                }
            } else {
                status = MTP_RC_INVALID_OBJECT_HANDLE;
            }
        }

        if (status == USB_ERR_SUCCESS) {
            //dbg("%s(): Handle %d, Offset = %lld, Length = %lld, OpCode = 0x%X", __func__, ObjectHandle, ObjectOffset, ObjectLengthRequested, OpCode);

            /* Do sanity check.  */
            if (ObjectOffset != ObjectReadOffset) {
                AmbaPrint_PrintUInt5("CbObjectDataGetEx(): Offset mismatch ", 0, 0, 0, 0, 0);
                /* Move file I/O pointer by object offset */
                if (AmbaFS_FileSeek(MtpClassInfo.FpArray, (INT64)ObjectOffset, (INT32)AMBA_FS_SEEK_START) != 0U) {
                    AmbaPrint_PrintUInt5("CbObjectDataGetEx(): File seek error", 0, 0, 0, 0, 0);
                    status = MTP_RC_OBJECT_SEEK_FAILED;
                } else {
                    ObjectReadOffset = ObjectOffset;
                }
            }
        }

        if (status == USB_ERR_SUCCESS) {
            /* Read from the file into the media buffer.  */
            {
                UINT32 size_c;

                Rval = AmbaFS_FileRead(ObjectBuffer, 1U, (UINT32)ObjectLengthRequested, MtpClassInfo.FpArray, &size_c);
                if (Rval == OK) {
                    ReadLen = size_c;
                }
            }
            if (ReadLen == ObjectLengthRequested) {
                status = USB_ERR_SUCCESS;
            } else {
                AmbaPrint_PrintUInt5("CbObjectDataGetEx(): File read error, request %u bytes but read %u", (UINT32)ObjectLengthRequested, (UINT32)ReadLen, 0, 0, 0);
                status = MTP_RC_ACCESS_DENIED;
            }

            *ObjectActualLength = ReadLen;
            ObjectReadOffset   += ReadLen;

            /* Check if we have read the entire file.  */
            if (AmbaFS_FileEof(MtpClassInfo.FpArray) != 0U) {
                /* This is the end of the transfer for the object. Close it.  */
                StatusClose                    = AmbaFS_FileClose(MtpClassInfo.FpArray);
                MtpClassInfo.FpArray           = NULL;
                MtpClassInfo.OjectHandleOpened = 0U;
                AmbaPrint_PrintUInt5("CbObjectDataGetEx(): close file", 0, 0, 0, 0, 0);

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
static UINT32 CbObjectInfoSend(USBD_MTP_OBJECT_s *Object, ULONG StorageId, ULONG ParentObjectHandle, ULONG *ObjectHandle)
{
    char str_asc[MTP_MAX_FILENAME_LENGTH] = { '\0' };
    UINT32 status;
    UINT32 fs_status;
    UINT32   tmpStorageId;

    (void)StorageId;

    //To keep misra-c happy...
    tmpStorageId = Object->StorageId;
    Object->StorageId = tmpStorageId;

    AmbaPrint_PrintUInt5("CbObjectInfoSend(): Object Info:", 0, 0, 0, 0, 0);

    Uni2Asc(Object->Filename, (UINT8 *)str_asc);

    AmbaPrint_PrintStr5("    Name: %s", str_asc, NULL, NULL, NULL, NULL);
    AmbaPrint_PrintUInt5("    FormatCode: 0x%X", Object->Format, 0, 0, 0, 0);
    AmbaPrint_PrintUInt5("    Length: 0x%X", Object->CompressedSize, 0, 0, 0, 0);

    if (IsOperationSupported(MTP_OC_SEND_OBJECT_INFO) == 0U) {
        AmbaPrint_PrintUInt5("CbObjectInfoSend(): not supported operation.", 0, 0, 0, 0, 0);
        status = MTP_RC_OPERATION_NOT_SUPPORTED;
    } else {
        if (MtpClassInfo.HandleNumber < MTP_MAX_HANDLES) {
            char filename_asc[MTP_MAX_FILENAME_LENGTH] = { '\0' };

            if ((ParentObjectHandle == 0xFFFFFFFFU) || (ParentObjectHandle == 0U)) {
                filename_asc[0] = (char)MtpClassInfo.DeviceInfo.RootPath[0];
                filename_asc[1] = ':';
                filename_asc[2] = '\0';
            } else {
                // get parent file name
                if (DbGetFilePathFromHandle(ParentObjectHandle, filename_asc) != USB_ERR_SUCCESS) {
                    /* do nothing */
                }
            }

            filename_asc[MTP_MAX_FILENAME_LENGTH - 1U] = '\0';

            AmbaPrint_PrintStr5("    Parent Name: %s", filename_asc, NULL, NULL, NULL, NULL);

            AmbaUtility_StringAppend(filename_asc, MTP_MAX_FILENAME_LENGTH, "\\");
            AmbaUtility_StringAppend(filename_asc, MTP_MAX_FILENAME_LENGTH, str_asc);

            AmbaPrint_PrintStr5("    Full Name: %s", filename_asc, NULL, NULL, NULL, NULL);

            if (Object->Format == MTP_OFC_ASSOCIATION) {
                fs_status = AppUsb_MakeDir((const char *)filename_asc);
                if (fs_status != 0U) {
                    AmbaPrint_PrintStr5("CbObjectInfoSend(): Can not create %s", filename_asc, NULL, NULL, NULL, NULL);
                    status = MTP_RC_STORE_NOT_AVAILABLE;
                } else {
                    *ObjectHandle = DbInsertHandle(filename_asc, Object, ParentObjectHandle);
                    MtpClassInfo.HandleNumber++;
                    ResetPreParseBuf(*ObjectHandle);
                    status = MTP_RC_OK;
                }
            } else {
                *ObjectHandle = DbInsertHandle(filename_asc, Object, ParentObjectHandle);
                if (*ObjectHandle == 0U) {
                    AmbaPrint_PrintStr5("CbObjectInfoSend(): can't add handle for %s.",  filename_asc, NULL, NULL, NULL, NULL);
                    status = MTP_RC_ACCESS_DENIED;
                } else {
                    AmbaPrint_PrintStr5("CbObjectInfoSend(): handle name = %s.", filename_asc, NULL, NULL, NULL, NULL);

                    send_object_info.Status     = APP_MTPD_SEND_OBJECT_START;
                    // avoid to use structure assignment
                    //send_object_info.ObjectInfo = *Object;
                    AmbaSvcWrap_MisraMemcpy(&(send_object_info.ObjectInfo), Object, sizeof(USBD_MTP_OBJECT_s));
                    send_object_info.Handle     = (UINT32)*ObjectHandle;
                    send_object_info.FileOffset = 0;
                    MtpClassInfo.HandleNumber++;
                    ResetPreParseBuf(*ObjectHandle);
                    SendObjectSizeToBuf(*ObjectHandle, (UINT64)Object->CompressedSize);
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
static UINT32 CbObjectDataSendEx(ULONG Handle, ULONG Phase, const UCHAR *Buffer, UINT64 Offset, UINT64 Length)
{
    UINT64 size = 0U;
    UINT32 uRet = 0;
    UINT32 FsStatus;
    static char file_name_buffer[1024] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

    //AppUsb_Print("%s(): Handle %d, Offset %lld, Length %lld, phase %d", __func__, Handle, Offset, Length, Phase);

    if ((Phase == 1U) || (Phase == 2U)) {
        if (send_object_info.File != NULL) {
            FsStatus = AmbaFS_FileClose(send_object_info.File);
            if (FsStatus != 0U) {
                // need to process it?
            }
            send_object_info.File = NULL;
        }
        send_object_info.Status = 0;
    } else {
        if ((send_object_info.File == NULL) && (Offset != 0U)) {
            AmbaPrint_PrintUInt5("CbObjectDataSendEx(): File is NULL but offset is 0, something wrong.", 0, 0, 0, 0, 0);
            uRet = MTP_RC_GENERAL_ERROR;
        } else {
            if ((send_object_info.File == NULL) && (Offset == 0U)) {
                // open file for write
                char *path = file_name_buffer;
                if (DbGetFilePathFromHandle(Handle, path) != USB_ERR_SUCCESS) {
                    uRet = MTP_RC_INVALID_OBJECT_HANDLE;
                }

                if (uRet == 0U) {
                    FsStatus = AmbaFS_FileOpen(path, "w", &send_object_info.File);
                    if ((FsStatus != OK) || (send_object_info.File == NULL)) {
                        AmbaPrint_PrintStr5("CbObjectDataSendEx(): can't open %s.", path, NULL, NULL, NULL, NULL);
                        uRet = MTP_RC_GENERAL_ERROR;
                    }
                }
            }

            if (uRet == 0U) {
                if (send_object_info.File == NULL) {
                    AmbaPrint_PrintUInt5("CbObjectDataSendEx(): File is NULL, something wrong.", 0, 0, 0, 0, 0);
                    uRet = MTP_RC_GENERAL_ERROR;
                }

                if (send_object_info.FileOffset != Offset) {
                    AmbaPrint_PrintUInt5("CbObjectDataSendEx(): offset is wrong.", 0, 0, 0, 0, 0);
                    uRet = MTP_RC_GENERAL_ERROR;
                }

                if (uRet == 0U) {
                    {
                        UINT32 size_c;
                        UCHAR *object_buffer = NULL;

                        AmbaMisra_TypeCast(&object_buffer, &Buffer);
                        FsStatus = AmbaFS_FileWrite(object_buffer, 1U, (UINT32)Length, send_object_info.File, &size_c);
                        if (FsStatus == OK) {
                            size = size_c;
                        }
                    }
                    if (size != Length) {
                        AmbaPrint_PrintUInt5("CbObjectDataSendEx(): can't write file,  request %u bytes but write %u.", (UINT32)Length, (UINT32)size, 0, 0, 0);
                        uRet = MTP_RC_GENERAL_ERROR;
                    } else {
                        send_object_info.FileOffset += Length;
                    }
                }
            }
        }
    }

    return uRet;
}

/* Operation code : GetObjectReferences 0x9810 */
static UINT32 CbObjectReferenceGet(ULONG ObjectHandle, UCHAR **ObjectReferenceArray, ULONG *ObjectReferenceArrayLength)
{
    static UINT32 MtpReferencesArray = 0;
    UINT32 status;
    UINT32 uRet;

    status = ObjectHandleCheck(ObjectHandle);

    /* Does the object handle exist ?  */
    if (status == USB_ERR_SUCCESS) {
        /* The property exist. Currently we just return a empty array. App can modify it depending on requirement. */
        MtpReferencesArray = 0;

        /* Return its pointer to MTP.  */
        *ObjectReferenceArray = (UINT8 *) &MtpReferencesArray;

        /* And the length of the dataset.  */
        *ObjectReferenceArrayLength = sizeof(UINT32);

        /* Done here.  */
        uRet = 0;
    } else {
        uRet = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return uRet;
}

/* Operation code : SetObjectReferences 0x9811 */
static UINT32 CbObjectReferenceSet(ULONG ObjectHandle, UCHAR *ObjectReferenceArray, ULONG ObjectReferenceArrayLength)
{
    UINT32 status;
    UINT32 uRet;

    AmbaMisra_TouchUnused(ObjectReferenceArray);
    AmbaMisra_TouchUnused(&ObjectReferenceArrayLength);

    status = ObjectHandleCheck(ObjectHandle);

    if (status == USB_ERR_SUCCESS) {
        // application can do something here.
        uRet = 0;
    } else {
        uRet = MTP_RC_INVALID_OBJECT_HANDLE;
    }
    return uRet;
}

static UINT32 LoadObjects(void)
{
    UINT32 uRet = USB_ERR_SUCCESS;
    UINT32 t1, t2;

    t1 = tx_time_get();
    // add database for object info
    uRet = DbScan();
    t2   = tx_time_get();
    AmbaPrint_PrintUInt5("MTP object scan takes %d ms", t2 - t1, 0, 0, 0, 0);

    if (uRet == USB_ERR_SUCCESS) {
        MtpClassInfo.HandleNumber = DbGetTotalHandleNumber();

        AmbaPrint_PrintUInt5("Total objects = %d", MtpClassInfo.HandleNumber, 0, 0, 0, 0);
    }

    return uRet;
}

static UINT32 DeviceClassMtpInitSystem(char StgDrive)
{
    // PIMA MTP names
    static UINT8 MtpDeviceInfoVendorName[]  = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x00 }; //"Ambarella";
    static UINT8 MtpDeviceInfoProductName[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                                0x4D, 0x54, 0x50, 0x20, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65,
                                                0x00
                                              };                                                       //"Ambarella MTP Device";
    static UINT8 MtpDeviceInfoSerialNo[] = { 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x30, 0x00 };                //"1.0.0.0";
    static UINT8 MtpDeviceInfoVersion[]  = { 0x56, 0x31, 0x2E, 0x30, 0x00 };                                  //"V1.0";

    // PIMA MTP storage names
    static UINT8 MtpVolumeDescription[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                            0x4D, 0x54, 0x50, 0x20, 0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74,
                                            0x20, 0x44, 0x69, 0x73, 0x6B, 0x20, 0x56, 0x6F, 0x6C, 0x75,
                                            0x6D, 0x65, 0x00
                                          };                               //"Ambarella MTP Client Disk Volume";
    static UINT8 MtpVolumeLabel[] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20,
                                      0x4D, 0x54, 0x50, 0x20, 0x43, 0x6C, 0x69, 0x65, 0x6E, 0x74,
                                      0x20, 0x53, 0x44, 0x20, 0x73, 0x6C, 0x6F, 0x74, 0x00
                                    }; // "Ambarella MTP Client SD slot";

    // Vendor extension info
    static UCHAR MtpDeviceVendorExtensionDesc[] = { 0x6D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x2E,
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

    static UINT16 DeviceSupportCaptureFormat[] = {
        MTP_OFC_UNDEFINED,
        MTP_OFC_EXIF_JPEG,
        0
    };

    static UINT16 supported_events[] = {
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


    UINT32 uRet;

    MtpClassInfo.DeviceInfo.DeviceInfoVendorName       = MtpDeviceInfoVendorName;
    MtpClassInfo.DeviceInfo.DeviceInfoProductName      = MtpDeviceInfoProductName;
    MtpClassInfo.DeviceInfo.DeviceInfoSerialNo         = MtpDeviceInfoSerialNo;
    MtpClassInfo.DeviceInfo.DeviceInfoVersion          = MtpDeviceInfoVersion;
    MtpClassInfo.DeviceInfo.VolumeDescription          = MtpVolumeDescription;
    MtpClassInfo.DeviceInfo.VolumeLabel                = MtpVolumeLabel;
    MtpClassInfo.DeviceInfo.RootPath[0]                = (UINT8)StgDrive;
    MtpClassInfo.DeviceInfo.RootPath[1]                = 0x3A; //':'
    MtpClassInfo.DeviceInfo.RootPath[2]                = 0x5C; //'\\';
    MtpClassInfo.DeviceInfo.RootPath[3]                = 0;
    MtpClassInfo.DeviceInfo.StorageId                  = MTP_STORAGE_ID;
    MtpClassInfo.DeviceInfo.StorageType                = MTP_STC_FIXED_RAM;
    MtpClassInfo.DeviceInfo.FileSystemType             = MTP_FSTC_DCF;
    MtpClassInfo.DeviceInfo.AccessCapability           = MTP_AC_READ_WRITE;
    MtpClassInfo.DeviceInfo.MaxCapacityLow             = 0;
    MtpClassInfo.DeviceInfo.MaxCapacityHigh            = 0;
    MtpClassInfo.DeviceInfo.FreeSpaceLow               = 0;
    MtpClassInfo.DeviceInfo.FreeSpaceHigh              = 0;
    MtpClassInfo.DeviceInfo.FreeSpaceImage             = 0xFFFFFFFFU;
    MtpClassInfo.DeviceInfo.DeviceSupportProp          = GetSupportedDevicePropList();
    MtpClassInfo.DeviceInfo.DeviceSupportCaptureFormat = DeviceSupportCaptureFormat;
    MtpClassInfo.DeviceInfo.DeviceSupportImgFormat     = DeviceSupportImgFormat;
    MtpClassInfo.DeviceInfo.ObjectSupportProp          = ObjectSupportProp;
    MtpClassInfo.DeviceInfo.OperationSupportList       = OperationSupportList;
    MtpClassInfo.DeviceInfo.EventSupportList           = supported_events;
    MtpClassInfo.DeviceInfo.PropDescGet                = CbDevicePropDescGet;
    MtpClassInfo.DeviceInfo.PropValueGet               = CbDevicePropValueGet;
    MtpClassInfo.DeviceInfo.PropValueSet               = CbDevicePropValueSet;
    MtpClassInfo.DeviceInfo.PropValueReset             = CbDevicePropValueReset;
    MtpClassInfo.DeviceInfo.StorageFormat              = CbStorageFormat;
    MtpClassInfo.DeviceInfo.ObjectDelete               = CbObjectDelete;
    MtpClassInfo.DeviceInfo.DeviceReset                = CbDeviceReset;
    MtpClassInfo.DeviceInfo.StorageInfoGet             = CbStorageInfoGet;
    MtpClassInfo.DeviceInfo.ObjectNumberGet            = CbObjectNumberGet;
    MtpClassInfo.DeviceInfo.ObjectHandlesGet           = CbObjectHandlesGet;
    MtpClassInfo.DeviceInfo.ObjectInfoGet              = CbObjectInfoGet;
    MtpClassInfo.DeviceInfo.ObjectDataGet              = CbObjectDataGetEx;
    MtpClassInfo.DeviceInfo.ObjectThumbGet             = CbObjectThumbGet;
    MtpClassInfo.DeviceInfo.ObjectInfoSend             = CbObjectInfoSend;
    MtpClassInfo.DeviceInfo.ObjectDataSend             = CbObjectDataSendEx;
    MtpClassInfo.DeviceInfo.ObjectPropDescGet          = CbObjectPropDescGet;
    MtpClassInfo.DeviceInfo.ObjectPropValueGet         = CbObjectPropValueGet;
    MtpClassInfo.DeviceInfo.ObjectPropValueSet         = CbObjectPropValueSet;
    MtpClassInfo.DeviceInfo.ObjectReferenceGet         = CbObjectReferenceGet;
    MtpClassInfo.DeviceInfo.ObjectReferenceSet         = CbObjectReferenceSet;
    MtpClassInfo.DeviceInfo.ObjectPropListGet          = CbObjectPropListGet;
    MtpClassInfo.DeviceInfo.ObjectClearAll             = CbObjectClearAll;
    MtpClassInfo.DeviceInfo.ObjectCustomCommand        = AppMtpd_CbAmageCommunication;
    MtpClassInfo.DeviceInfo.ObjectCustomDataGet        = AppMtpd_CbAmageDataGet;
    MtpClassInfo.DeviceInfo.ObjectCustomDataSend       = AppMtpd_CbAmageDataSend;
    MtpClassInfo.DeviceInfo.TransferCanceled           = CbTransferCanceled;
    MtpClassInfo.DeviceInfo.VendorExtensionID          = APP_MTPD_EXTENSION_ID;
    MtpClassInfo.DeviceInfo.VendorExtensionVersion     = APP_MTPD_EXTENSION_VERSION;
    MtpClassInfo.DeviceInfo.VendorExtensionDesc        = MtpDeviceVendorExtensionDesc;

    uRet = AmbaUSBD_MTPSetInfo(&MtpClassInfo.DeviceInfo);
    if (uRet == USB_ERR_SUCCESS) {
        MtpInitDeviceProperties(&MtpClassInfo.DeviceProps);

        DbSetDrive(MtpClassInfo.DeviceInfo.RootPath[0]);
    }

    return uRet;
}

static void DeviceClassMtpInitZero(void)
{
    AmbaSvcWrap_MisraMemset(&MtpClassInfo, 0, sizeof(MtpClassInfo));
    AmbaSvcWrap_MisraMemset(&send_object_info, 0, sizeof(send_object_info));
    ObjectReadOffset = 0;
    DbInitZero();
    return;
}


/**
* initialization of usb mtp class
* @param [in] StgDrive main storage drive
* @return 0-OK, 1-NG
*/
UINT32 AmbaSvcUsb_DeviceClassMtpInit(char StgDrive)
{
    UINT32 rval;

    DeviceClassMtpInitZero();
    rval = DeviceClassMtpInitSystem(StgDrive);
    if (rval == USB_ERR_SUCCESS) {
        rval = LoadObjects();
    }

    return rval;
}
