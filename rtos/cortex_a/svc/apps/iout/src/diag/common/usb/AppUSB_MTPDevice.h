/**
 *  @file AppUSB_MTPDevice.h
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
 *  @details Header file includes MTP device functions and variables.
 */
#ifndef APPUSB_MTPD_H
#define APPUSB_MTPD_H

#include "AmbaFS.h"
#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#include "AppUSB_PlatformPort.h"

#define MTP_STORAGE_ID                       0x10001U
#define MTP_OC_CUSTOM_COMMAND                0x9999U

/* Define PIMA object and thumb transfer phase.  */

#define MTP_OBJ_TRANS_PHASE_ACTIVE           0U
#define MTP_OBJ_TRANS_PHASE_COMPL            1U
#define MTP_OBJ_TRANS_PHASE_COMPL_ERROR      2U

/* Define PIMA Dataset equivalences.  */
#define MTP_OBJ_PROP_VALUE_GET               0x00U
#define MTP_OBJ_PROP_VALUE_GETSET            0x01U

/* Define local Audio codecs.  */
#define MTP_WAVE_FORMAT_MPEGLAYER3           0x00000055U
#define MTP_WAVE_FORMAT_MPEG                 0x00000050U
#define MTP_WAVE_FORMAT_RAW_AAC1             0x000000FFU

#define MTP_DATASET_VALUE_OFFSET             6U
#define MTP_MAX_HANDLES                      9999U

#define MTP_OBJECT_PROP_FORMAT_ALL           0x00000000U

#define MTP_OBJECT_PROP_ZERO_DEPTH           0x00000000U
#define MTP_OBJECT_PROP_ALL_DEPTH            0xFFFFFFFFU
#define MTP_OBJECT_PROP_ROOT_OBJECTS         0x00000000U
#define MTP_OBJECT_PROP_SPECIFIC_ROOT_OBJECT 0x00000001U
#define MTP_OBJECT_PROP_ALL_OBJECTS          0xFFFFFFFFU
#define MTP_DEVICE_PROP_ALL_OBJECTS          0xFFFFFFFFU

#define FLAG_PREPARSE_START                  0x01U
#define FLAG_PREPARSE_DONE                   0x02U

#define ExtractObjectInfoMode                0    // 0: sequential; 1: partially pre-parse;
#define PartialPreParseStartIndex            701U
#define MTP_MAX_FILENAME_LENGTH              64U

#define MTP_EVENT_OBJECT_HANDLE_TEST         0xEFFFFFFFU

typedef struct {
    UINT32          Size;
    UINT8 *         Buffer;
} APP_MTPD_MEM_CTRL_s;

#define DEVICE_PROP_DATASET_STRING_LENGTH (256)

typedef struct {
    // SyncPartner
    WCHAR SyncPartnerDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR SyncPartnerCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // DateTime
    WCHAR DateTimeDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR DateTimeCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // DeviceFriendlyName
    WCHAR DeviceFriendlyNameDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR DeviceFriendlyNameCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // Session Initiator Version Info
    WCHAR SessionInitVerInfoDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR SessionInitVerInfoCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
} APP_MTPD_DEVICE_PROPS_s;

typedef struct {
    UINT32                  HandleNumber;
    USBD_MTP_DEVICE_INFO_s  DeviceInfo;
    AMBA_FS_FILE *          FpArray;
    UINT32                  OjectHandleOpened;
    APP_MTPD_DEVICE_PROPS_s DeviceProps;
} MTP_CLASS_INFO_s;

typedef struct {
    UINT32 ObjectHeight;
    UINT32 ObjectWidth;
    UINT64 ObjectLength;
    UINT32 ThumbHeight;
    UINT32 ThumbWidth;
    UINT32 ThumbSize;
} APP_MTPD_CACHED_OBJECT_INFO_s;

/* Global Variables */
extern MTP_CLASS_INFO_s g_mtp_class_info;
extern USBD_DESC_CUSTOM_INFO_s g_mtp_desc_framework;

/* Global Functions */
VOID   AppMtpd_DevicePropertiesInit(APP_MTPD_DEVICE_PROPS_s *Props);

UINT16 *AppMtpd_SupportedDevPropListGet(void);
UINT32 AppMtpd_IsObjectFormatSupported(UINT16 Format);
UINT32 AppMtpd_IsObjectPropSupported(UINT16 Format, UINT16 Property);
UINT32 AppMtpd_IsOperationSupported(UINT16 Opcode);

void   AppMtpd_DbDriveSet(UINT8 Drive);
UINT32 AppMtpd_DbScan(void);
void   AppMtpd_DbClean(void);
UINT32 AppMtpd_DbTotalHandleNumberGet(void);
UINT32 AppMtpd_DbObjectInfoGet(UINT32 Handle, USBD_MTP_OBJECT_s **Object);
UINT64 AppMtpd_DbFileSizeGet(UINT32 Handle);
UINT32 AppMtpd_DbFileFormatGet(UINT32 Handle);
UINT32 AppMtpd_DbFileNameFromHandleGet(UINT32 Handle, char *FileName);
UINT32 AppMtpd_DbFilePathFromHandleGet(UINT32 Handle, char *Path);
UINT32 AppMtpd_DbParentHandleGet(UINT32 Handle);
UINT8  AppMtpd_DbAttributeFromHandleGet(UINT32 Handle);
UINT32 AppMtpd_DbNumberOfObjectsGet(UINT32 FormatCode, UINT32 Association);
UINT32 AppMtpd_DbObjectHandlesFill(UINT32 FormatCode, UINT32 Association, UINT32 *Handles);
UINT8  AppMtpd_DbIsImageHandle(UINT32 Handle);
UINT32 AppMtpd_DbObjectHeightGet(UINT32 Handle);
UINT32 AppMtpd_DbObjectWidthGet(UINT32 Handle);
UINT32 AppMtpd_DbObjectDelete(UINT32 Handle);
UINT32 AppMtpd_DbHandleInsert(char *Path, const USBD_MTP_OBJECT_s *Object, UINT32 ParentHandle);
UINT32 AppMtpd_DbHandlePathUpdate(UINT32 ObjectHandle, const char *NewPath);
UINT32 AppMtpd_DbIsObjectHandleValid(UINT32 Handle);
VOID   AppMtpd_DbInitZero(void);
UINT32 AppMtpd_ObjectHandleCheck(ULONG Handle);

VOID    AppMtpd_Asc2PascalString(const char *Asc, UINT8 *Pascal);
VOID    AppMtpd_Pascal2AscString(const UINT8 *Pascal, char *Asc);
UINT16 *AppMtpd_ObjectPropListGet(UINT16 Format);
char    AppMtpd_StorageDriveGet(UINT32 StorageId);
VOID    AppMtpd_FileNameExtract(const char *Path, char *FileName);
VOID    AppMtpd_Uni2AscString(const UINT8 *Uni, UINT8 *Asc);

UINT64 AppMtpd_DbCachedObjectSizeGet(UINT32 Handle);
VOID   AppMtpd_DbCachedObjectSizeSet(ULONG Handle, UINT64 ObjectSize);
VOID   AppMtpd_DbCachedObjectInfoReset(UINT32 Handle);

UINT32 AppMtpd_ShortPut(UINT8 * Address, ULONG Value);
UINT32 AppMtpd_LongPut(UINT8 * Address, ULONG Value);
UINT32 AppMtpd_LongGet(const UINT8 * Address);
UINT32 AppMtpd_StringAppendUni(UINT8 *pBuffer, const WCHAR *UniString);
UINT32 AppMtpd_UInt8StringLengthGet(const UINT8 *String);

// callback functions
UINT32 AppMtpd_CbDevicePropDescGet(ULONG DeviceProperty, UCHAR **DevicePropDataset, ULONG *DevicePropDtasetLength);
UINT32 AppMtpd_CbDevicePropValueSet(ULONG DeviceProperty, const UCHAR *DevicePropValue, ULONG DevicePropValueLength);
UINT32 AppMtpd_CbDevicePropValueGet(ULONG DeviceProperty, UCHAR **DevicePorpValue, ULONG *DevicePropValueLength);
UINT32 AppMtpd_CbDevicePropValueReset(ULONG DeviceProperty);
UINT32 AppMtpd_CbObjectPropValueSet(ULONG ObjectHandle, ULONG ObjectProperty, UCHAR *ObjectPropValue, ULONG ObjectPropValueLength);
UINT32 AppMtpd_CbObjectPropValueGet(ULONG Handle, ULONG Property, UCHAR **PropValue, ULONG *PropValueLength);
UINT32 AppMtpd_CbObjectPropDescGet(ULONG Property, ULONG Format, UCHAR **Dataset, ULONG *DatasetLength);
UINT32 AppMtpd_CbObjectPropListGet(ULONG *Params, UCHAR **PropList, ULONG *PropListLength);

UINT32 AppMtpd_Init(void);
void   AppMtpd_AddEventTest(UINT32 Count);
void   AppMtpd_AddEventObjectAdded(void);
void   AppMtpd_AddEventObjectRemoved(void);

#endif

