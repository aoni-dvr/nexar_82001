/**
 *  @file AmbaUSBD_API.h
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
 *  @details USB Device API header file.
 */

#ifndef AMBAUSB_DEVICE_API_H
#define AMBAUSB_DEVICE_API_H

#ifndef AMBAUSB_GENERIC_H
#include <AmbaUSB_Generic.h>
#endif

#define USB_CONNECT_SPEED_FULL       0U // USB 2.0 12Mb
#define USB_CONNECT_SPEED_HIGH       1U // USB 2.0 480Mb
#define USB_CONNECT_SPEED_SUPER      2U // USB 3.2 Gen1, 5Gb
#define USB_CONNECT_SPEED_SUPER_PLUS 3U // USB 3.2 Gen2, 10Gb

typedef enum {
    USBD_CLASS_NONE = 0,
    USBD_CLASS_MSC,      // mass storage class
    USBD_CLASS_MTP,      // ptp class
    USBD_CLASS_CDC_ACM,  // CDC-ACM class with single instances
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    USBD_CLASS_UVC,
#endif
    USBD_CLASS_USER,
    USBD_CLASS_NUM
} USBD_CLASS_e;

typedef enum {
    USBD_CHGDET_SDP = 0,    // standard downstream port
    USBD_CHGDET_DCP,        // dedicate charger port
    USBD_CHGDET_CDP,        // charger downstream port
    USBD_CHGDET_NO_CONTACT  // no data contact
} USBD_CHGDET_e;

typedef UINT32 (*USBD_SYSTEM_EVENT_CB_f)(UINT32 Code, UINT32 Param1, UINT32 Param2, UINT32 Param3);
typedef UINT32 (*USBD_SYSTEM_TASK_CREATE_CB_f)(UINT32 Code, USB_TASK_ENTRY_f TaskEntry, VOID *Arg, AMBA_KAL_TASK_t **pTask);
typedef UINT32 (*USBD_SYSTEM_TASK_SUSPEND_CB_f)(UINT32 Code, USB_TASK_ENTRY_f TaskEntry, AMBA_KAL_TASK_t *pTask);

#define USBD_SYSTEM_EVENT_CONNECT            0
#define USBD_SYSTEM_EVENT_DISCONNECT         1
#define USBD_SYSTEM_EVENT_CONFIGURED         2
#define USBD_SYSTEM_EVENT_SUSPENDED          3
#define USBD_SYSTEM_EVENT_RESUMED            4
#define USBD_SYSTEM_EVENT_RESET              5
#define USBD_SYSTEM_EVENT_SET_FEATURED       6
#define USBD_SYSTEM_EVENT_CLEAR_FEATURED     7
#define USBD_SYSTEM_EVENT_SET_INTERFACE      8

#define USBD_VBUS_TASK_CODE                  0
#define USBD_ISR_TASK_CODE                   1
#define USBD_CONTROL_REQUEST_TASK_CODE       2

typedef struct {
    UINT8                        *StackCacheMemory;       // [Input] static memory for USBX stack usage (cacheable)
    UINT32                        StackCacheMemorySize;   // [Input] Size of static memory for USBX stack usage (cacheable)
    UINT8                        *StackNonCacheMemory;    // [Input] static memory for USBX stack usage (non-cacheable)
    UINT32                        StackNonCacheMemorySize;// [Input] Size of static memory for USBX stack usage (non-cacheable)
    UINT32                        CoreMask;               // [Input] KAL Core Mask for creation of task
    USBD_SYSTEM_EVENT_CB_f        SystemEventCallback;    // [Input] System events callback
    USBD_SYSTEM_TASK_CREATE_CB_f  SystemTaskCreateCb;     // [Input] System task create callback
    USBD_SYSTEM_TASK_SUSPEND_CB_f SystemTaskSuspendCb;    // [Input] System task suspend callback
    UINT32                        InitSpeed;              // [Input] The Maximum supported USB speed. 0:HIGH 1:FULL
    UINT32                        SetCfgThd;              // [Input] SetConfig request timer threshold in ms.
} USBD_SYS_CONFIG_s;

typedef struct {
    UINT8                       *DescFrameworkFs;       // [Input] USB full speed descriptor
    UINT8                       *DescFrameworkHs;       // [Input] USB high speed descriptor
    UINT8                       *DescFrameworkSs;       // [Input] USB super speed descriptor
    UINT8                       *StrFramework;          // [Input] USB string descriptor
    UINT8                       *LangIDFramework;       // [Input] USB language descriptor
    UINT32                      DescSizeFs;             // [Input] Size of the USB full speed descriptor
    UINT32                      DescSizeHs;             // [Input] Size of the USB high speed descriptor
    UINT32                      DescSizeSs;             // [Input] Size of the USB super speed descriptor
    UINT32                      StrSize;                // [Input] Size of the USB string descriptor
    UINT32                      LangIDSize;             // [Input] Size of the USB language descriptor
} USBD_DESC_CUSTOM_INFO_s;

typedef UINT32 (*USBD_CLASS_START_CB_f)(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
typedef UINT32 (*USBD_CLASS_STOP_CB_f)(void);

typedef struct {
    USBD_CLASS_e                ClassID;                // [Input] USB Class ID
    USBD_DESC_CUSTOM_INFO_s     *Framework;             // [Input] Customized USB descriptor framework
    USBD_CLASS_START_CB_f       ClassStartCb;
    USBD_CLASS_STOP_CB_f        ClassStopCb;
} USBD_CLASS_CONFIG_s;

// Mass Storage structures
#define MSC_MEDIA_FAT_DISK                       0
#define MSC_MEDIA_CDROM                          5
#define MSC_MEDIA_OPTICAL_DISK                   7

typedef struct {
    UINT8   MediaPresent; // Checks Whether a card is present or not.
    //    0: Not present
    //    1: Present
    INT32   Format;       // File System Format of card.
    //     -1: card is not formatted.
    //     Others: card is formatted.
    UINT32  SectorNumber; // Number of sectors
    UINT32  SectorSize;   // Sector size in bytes
} USBD_MSC_CARD_STATUS_s;

typedef struct {
    UINT8   Removal;       // [Input] Indicates if it is a removal drive. It represents the RMB field in the Standard INQUERY data in the SCSC primary command.
    UINT8   WriteProtect;  // [Input] Indicates if it is a write protect drive. If it is 1, then this drive is read only.
    UINT8   Type;          // [Input] Device type of this drive. It represents the Peripheral Device Type field in the Standard INQUERY data in the SCSC primary commands.
    //         The workshop already has some definitions for it:
    //             MSC_MEDIA_FAT_DISK(0) - A FAT/FAT32/EXFAT disk drive.
    //             MSC_MEDIA_CDROM(5) - A CD-ROM disk drive
    //             MSC_MEDIA_OPTICAL_DISK(7) - A Optical disk drive other than the CD-ROM
} USBD_MSC_DRIVE_PROP_s;

typedef struct {
    UINT8 *Vendor;   // [Input] Vendor name (8 bytes)
    UINT8 *Product;  // [Input] Product name (16 bytes)
    UINT8 *Revision; // [Input] Product revision (4 bytes)
} USBD_MSC_DRIVE_DESC_s;

typedef struct {
    INT32 (*SectorRead)(INT32 SlotID, UINT8 *Buffer, UINT32 SectorStart, UINT32 Sectors);
    INT32 (*SectorWrite)(INT32 SlotID, const UINT8 *Buffer, UINT32 SectorStart, UINT32 Sectors);
    INT32 (*GetMediaInfo)(INT32 SlotID, USBD_MSC_CARD_STATUS_s *Status);
} USBD_MSC_FS_OP_s;

#define USBD_MAX_SLOT_NUMBER (3U)
typedef struct {
    USBD_MSC_FS_OP_s       *Operation;                           // [Input] Callback functions for media operations
    USBD_MSC_DRIVE_PROP_s   Property[USBD_MAX_SLOT_NUMBER];      // [Input] Mass Storage drive property.
    USBD_MSC_DRIVE_DESC_s  *Description;                         // [Input] Descriptive information of the MSC drive
} USBD_MSC_DEVICE_INFO_s;

// MTP structures

typedef struct {
    UINT32  Code;
    UINT32  SessionId;       /*Do not need to assign a value*/
    UINT32  TransactionId;   /*Do not need to assign a value*/
    UINT32  ParameterNumber;
    UINT32  Parameter1;
    UINT32  Parameter2;
    UINT32  Parameter3;
} USBD_MTP_EVENT_s;

#define USBD_MTP_MAX_FILENAME_LEN            128U
#define USBD_MTP_MAX_UNICODE_STR_LEN         256U
#define USBD_MTP_MAX_DATETIME_STR_LEN        64U

typedef struct {
    UINT32   StorageId;
    UINT32   Format;
    UINT32   ProtectionStatus;
    UINT32   CompressedSize;
    UINT32   ThumbFormat;
    UINT32   ThumbCompressedSize;
    UINT32   ThumbPixWidth;
    UINT32   ThumbPixHeight;
    UINT32   ImagePixWidth;
    UINT32   ImagePixHeight;
    UINT32   ImageBitDepth;
    UINT32   ParentObject;
    UINT32   AssociationType;
    UINT32   AssociationDesc;
    UINT32   SequenceNumber;
    UINT8    Filename[USBD_MTP_MAX_UNICODE_STR_LEN];
    UINT8    CaptureDate[USBD_MTP_MAX_DATETIME_STR_LEN];
    UINT8    ModificationDate[USBD_MTP_MAX_DATETIME_STR_LEN];
    UINT8    Keywords[USBD_MTP_MAX_UNICODE_STR_LEN];
    UINT32   State;
    UINT32   Offset;
    UINT32   TransferStatus;
    UINT32   HandleId;
    UINT32   Length;
    UINT8   *pBuffer;
} USBD_MTP_OBJECT_s;

typedef struct {
    UINT32   StorageId;
    UINT32   StorageType;
    UINT32   FileSystemType;
    UINT32   AccessCapability;
    UINT32   MaxCapacityLow;
    UINT32   MaxCapacityHigh;
    UINT32   FreeSpaceLow;
    UINT32   FreeSpaceHigh;
    UINT32   FreeSpaceImage;
    UINT8    *DeviceInfoVendorName;
    UINT8    *DeviceInfoProductName;
    UINT8    *DeviceInfoSerialNo;
    UINT8    *DeviceInfoVersion;
    UINT8    *VolumeDescription;
    UINT8    *VolumeLabel;
    UINT16   *DeviceSupportProp;
    UINT16   *DeviceSupportCaptureFormat;
    UINT16   *DeviceSupportImgFormat;
    UINT16   *ObjectSupportProp;
    UINT16   *OperationSupportList;
    UINT16   *EventSupportList;
    UINT32   (*PropDescGet)(ULONG Property, UCHAR **pPropDataset, ULONG *pPropDtasetLength);
    UINT32   (*PropValueGet)(ULONG Property, UCHAR **pPropValue, ULONG *pPropValueLength);
    UINT32   (*PropValueSet)(ULONG Property, const UCHAR *pPropValue, ULONG PropValueLength);
    UINT32   (*PropValueReset)(ULONG Property);
    UINT32   (*StorageFormat)(ULONG StorageId);
    UINT32   (*ObjectDelete)(ULONG Handle);
    UINT32   (*DeviceReset)(void);
    UINT32   (*StorageInfoGet)(ULONG StorageId, ULONG *pMaxCapacityLow, ULONG *pMaxCapacityHigh, ULONG *pFreeSpaceLow, ULONG *pFreeSpaceHigh);
    UINT32   (*ObjectNumberGet)(ULONG FormatCode, ULONG Association, ULONG *pObjectNumber);
    UINT32   (*ObjectHandlesGet)(ULONG FormatCode, ULONG Association, ULONG **pHandleArray, ULONG *pHandleNumber);
    UINT32   (*ObjectInfoGet)(ULONG Handle, USBD_MTP_OBJECT_s **pObject);
    UINT32   (*ObjectDataGet)(ULONG Handle, UCHAR *pBuffer, UINT64 Offset,UINT64 LengthRequested, UINT64 *pActualLength, USHORT OpCode);
    UINT32   (*ObjectInfoSend)(USBD_MTP_OBJECT_s *pObject, ULONG StorageId, ULONG ParentHandle, ULONG *pHandle);
    UINT32   (*ObjectDataSend)(ULONG Handle, ULONG Phase, const UCHAR *pBuffer, UINT64 Offset, UINT64 Length);
    UINT32   (*ObjectPropDescGet)(ULONG Property, ULONG FormatCode, UCHAR **pPropDataset, ULONG *pPropDatasetLength);
    UINT32   (*ObjectPropValueGet)(ULONG Handle, ULONG Property, UCHAR **pPropValue, ULONG *pPropValueLength);
    UINT32   (*ObjectPropValueSet)(ULONG Handle, ULONG Property, UCHAR *pPropValue, ULONG PropValueLength);
    UINT32   (*ObjectReferenceGet)(ULONG Handle, UCHAR **pReferenceArray, ULONG *pReferenceArrayLength);
    UINT32   (*ObjectReferenceSet)(ULONG Handle, UCHAR *pReferenceArray, ULONG ReferenceArrayLength);
    UINT32   (*ObjectPropListGet)(ULONG *pParams, UCHAR **pPropList, ULONG *pPropListLength);
    UINT32   (*ObjectCustomCommand)(ULONG Parameter1, ULONG Parameter2, ULONG Parameter3, ULONG Parameter4, ULONG Parameter5, ULONG *pDataLength, ULONG *pDirection);
    UINT32   (*ObjectCustomDataGet)(UCHAR *pBuffer, ULONG Offset, ULONG LengthRequested, ULONG *pActualLength);
    UINT32   (*ObjectCustomDataSend)(UCHAR *pBuffer, ULONG Offset, ULONG Length);
    void     (*ObjectClearAll)(void);
    UINT8    RootPath[USBD_MTP_MAX_FILENAME_LEN];
    VOID     (*TransferCanceled)(void);
    UINT32   (*ObjectThumbGet)(ULONG Handle, UCHAR *pBuffer, ULONG Offset, ULONG LengthRequested, ULONG *pActualLength);
    UINT32   VendorExtensionID;
    UINT16   VendorExtensionVersion;
    UCHAR    *VendorExtensionDesc;
} USBD_MTP_DEVICE_INFO_s;

typedef UINT32 (*USBD_COMPL_FUNC) (const void *UserSpecific, UINT32 ActualLength, UINT32 Status);

UINT32 AmbaUSB_SystemGetVersionInfo(USB_VER_INFO_s *pVersionInfo);

typedef void (*AMBA_USB_PRINT_f)(const char *Str);
void   AmbaUSB_SystemPrintFuncRegister(AMBA_USB_PRINT_f PrintFunc, UINT32 Flags);

// API functions
UINT32 AmbaUSBD_SystemInit(const USBD_SYS_CONFIG_s *pConfig);
UINT32 AmbaUSBD_SystemClassStart(const USBD_CLASS_CONFIG_s *pClassConfig);
UINT32 AmbaUSBD_SystemClassStop(VOID);
UINT32 AmbaUSBD_SystemDeInit(VOID);

UINT32 AmbaUSBD_SystemChargerDetection(UINT32 T1, UINT32 T2, UINT32 *pChargerType);
UINT32 AmbaUSBD_SystemGetConnectSpeed(UINT32 *pSpeed);
UINT32 AmbaUSBD_SystemGetVbusStatus(UINT32 *pVbusStatus);
UINT32 AmbaUSBD_SystemIsConfigured(UINT32 *pIsConfigured);



// Mass Storage functions
UINT32 AmbaUSBD_MSCSetInfo(const USBD_MSC_DEVICE_INFO_s *pDeviceInfo);
UINT32 AmbaUSBD_MSCMount(UINT8 Drive);
UINT32 AmbaUSBD_MSCUnMount(UINT8 Drive);
UINT32 AmbaUSBD_MSCClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
UINT32 AmbaUSBD_MSCClassStop(void);

// MTP functions
UINT32 AmbaUSBD_MTPSetInfo(const USBD_MTP_DEVICE_INFO_s *pDeviceInfo);
UINT32 AmbaUSBD_MTPAddEvent(const USBD_MTP_EVENT_s *pEvent);
UINT32 AmbaUSBD_MTPClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
UINT32 AmbaUSBD_MTPClassStop(void);

// CDC-ACM functions
UINT32 AmbaUSBD_CDCACMIsTerminalOpen(UINT32 *pIsOpen);
UINT32 AmbaUSBD_CDCACMWrite(UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout);
UINT32 AmbaUSBD_CDCACMRead(UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout);
UINT32 AmbaUSBD_CDCACMClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
UINT32 AmbaUSBD_CDCACMClassStop(void);

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
// UVC functions
UINT32 AmbaUSBD_UVCClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
UINT32 AmbaUSBD_UVCClassStop(void);
#endif
#endif
