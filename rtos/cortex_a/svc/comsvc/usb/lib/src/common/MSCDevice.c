/**
 *  @file MSCDevice.c
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
 *  @details USB MSC Device main application.
 */

#include "AmbaPrint.h"
#include "AmbaSvcUsb.h"
#include "AmbaUSBD_API.h"
#include "AmbaFS.h"
#include "AmbaFS_Format.h"//#include "AmbaSD_Def.h"
#include <AmbaFS.h>
#include "AmbaMisraFix.h"
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"
#include "AmbaSvcWrap.h"

#include "SvcUsb.h"
#include "MSCDevice.h"

#define VID_VALUE               0x0A, 0x07 // 0x070A
#define PID_VALUE               0x26, 0x40 // 0x4026

#define MAX_PACKET_SIZE_BULK_FS 0x40, 0x00 // 0x40
#define MAX_PACKET_SIZE_BULK_HS 0x00, 0x02 // 0x200
#if defined(SVC_USB_SUPER_SPEED)
#define MAX_PACKET_SIZE_BULK_SS 0x00, 0x04 // 0x400
#endif

#define CONFIGURATION_LENGTH    0x00, 0x00 // always 0x0000

/*-----------------------------------------------------------------------------------------------*\
   Multiple languages are supported on the device, to add
   a language besides English, the unicode language code must
   be appended to the LanguageIdFramework array and the length
   adjusted accordingly.
\*-----------------------------------------------------------------------------------------------*/

static UINT8 LangID[] = {
    /* English. */
    0x09, 0x04
};

static UINT8 MscDescFs[] GNU_ALIGNED_CACHESAFE = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,          // this descriptor size
    USB_DESC_TYPE_DEV,            // device descriptor type
    SPEC_VERSION_FS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x40,                         // max packet size
    VID_VALUE,                    // VID
    PID_VALUE,                    // PID
    0x00, 0x00,                   // Device release num
    0x01,                         // Manufacturer string index
    0x02,                         // Product string index
    0x03,                         // device serial number index
    0x01,                         // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,    // this descriptor size
    USB_DESC_TYPE_QUALIFIER,      // descriptor type
    SPEC_VERSION_FS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x40,                         // max packet size
    0x01,                         // configuration number
    0x00,                         // reserved

    /* Configuration descriptor */
    USB_DESC_LENGTH_CFG,          // this descriptor size
    USB_DESC_TYPE_CFG,            // descriptor type
    CONFIGURATION_LENGTH,         // total length
    0x01,                         // config number of interface
    0x01,                         // config value
    0x00,                         // config index
    0xC0,                         // attribute
    0x32,                         // max power unit=2mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,         // this descriptor size
    USB_DESC_TYPE_INTF,           // descriptor type
    0x00,                         // interface number
    0x00,                         // alternative settings number
    0x02,                         // endpoint number of this interface
    USB_DEVICE_CLASS_MSC,         // class code
    USB_DEVICE_SUBCLASS_MSC_SCSI, // subclass code
    USB_DEVICE_PROTOCOL_MSC_BOT,  // protocol
    0x00,                         // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,     // this descriptor size
    USB_DESC_TYPE_ENDPOINT,       // type
    0x01,                         // EP1 address
    DESC_EP_ATTR_BULK,            // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,      // max packet size
    0x00,                         // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,     // this descriptor size
    USB_DESC_TYPE_ENDPOINT,       // type
    0x82,                         // EP2 address
    DESC_EP_ATTR_BULK,            // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,      // max packet size
    0x00                          // interval
};

static UINT8 MscDescHs[] GNU_ALIGNED_CACHESAFE = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,          // this descriptor size
    USB_DESC_TYPE_DEV,            // device descriptor type
    SPEC_VERSION_HS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x40,                         // max packet size
    VID_VALUE,                    // VID
    PID_VALUE,                    // PID
    0x01, 0x00,                   // Device release num
    0x01,                         // Manufacturer string index
    0x02,                         // Product string index
    0x03,                         // device serial number index
    0x01,                         // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,    // this descriptor size
    USB_DESC_TYPE_QUALIFIER,      // descriptor type
    SPEC_VERSION_HS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x40,                         // max packet size
    0x01,                         // configuration number
    0x00,                         // reserved

#if defined(SVC_USB_SUPER_SPEED)
    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,           // this descriptor size
    USB_DESC_TYPE_BOS,             // descriptor type
    (UINT8)BOS_DESCRIPTOR_LENGTH_SS, 0x0, // total length
    USB_DESC_CAP_NUM,              // number of capability.

    /* USB 2.0 extenstion descriptor */
    USB_DESC_LENGTH_CAP_USB20EX,   // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_USB20EX,     // capability type
    0x06, 0x00, 0x00, 0x00,        // attribute

    /* Super Speed USB descriptor */
    USB_DESC_LENGTH_CAP_SS_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_SS_USB,      // capability type
    0x00,                          // attribute
    0x0E, 0x00,                    // SpeedSupport
    0x01,                          // FunctionSupport
    0x0A,                          // U1 Device Exit latency
    0x00, 0x01,                    // U2 Device Exit Latency
#endif

    /* Configuration descriptor */
    USB_DESC_LENGTH_CFG,          // this descriptor size
    USB_DESC_TYPE_CFG,            // descriptor type
    CONFIGURATION_LENGTH,         // total length
    0x01,                         // config number of interface
    0x01,                         // config value
    0x00,                         // config index
    0xC0,                         // attribute
    0x32,                         // max power unit=2mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,         // this descriptor size
    USB_DESC_TYPE_INTF,           // descriptor type
    0x00,                         // interface number
    0x00,                         // alternative settings number
    0x02,                         // endpoint number of this interface
    USB_DEVICE_CLASS_MSC,         // class code
    USB_DEVICE_SUBCLASS_MSC_SCSI, // subclass code
    USB_DEVICE_PROTOCOL_MSC_BOT,  // protocol
    0x00,                         // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,     // this descriptor size
    USB_DESC_TYPE_ENDPOINT,       // type
    0x01,                         // EP1 address
    DESC_EP_ATTR_BULK,            // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,      // max packet size
    0x00,                         // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,     // this descriptor size
    USB_DESC_TYPE_ENDPOINT,       // type
    0x82,                         // EP2 address
    DESC_EP_ATTR_BULK,            // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,      // max packet size
    0x00                          // interval
};

#if defined(SVC_USB_SUPER_SPEED)
static UINT8 MscDescSs[] GNU_ALIGNED_CACHESAFE = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,          // this descriptor size
    USB_DESC_TYPE_DEV,            // device descriptor type
    SPEC_VERSION_SS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x09,                         // max packet size
    VID_VALUE,                    // VID
    PID_VALUE,                    // PID
    0x01, 0x00,                   // Device release num
    0x01,                         // Manufacturer string index
    0x02,                         // Product string index
    0x03,                         // device serial number index
    0x01,                         // number of possible configuration

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,              // this descriptor size
    USB_DESC_TYPE_BOS,                // descriptor type
    USB_BOS_DESC_LENGTH,              // total length
    0x03,                             // number of capability.

    /* USB 2.0 extenstion descriptor */
    USB_DESC_LENGTH_CAP_USB20EX,   // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_USB20EX,     // capability type
    0x02, 0x00, 0x00, 0x00,        // attribute

    /* Super Speed USB descriptor */
    USB_DESC_LENGTH_CAP_SS_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_SS_USB,      // capability type
    0x00,                          // attribute
    0x0E, 0x00,                    // SpeedSupport
    0x01,                          // FunctionSupport
    0x0A,                          // U1 Device Exit latency
    0x00, 0x01,                    // U2 Device Exit Latency

    /* Super Speed Plus USB Device Capability Descriptor */
    USB_DESC_LENGTH_CAP_SSP_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,          // descriptor type
    USB_DESC_CAP_TYPE_SSP_USB,      // capability type
    0x00,                           // bReserved
    0x01, 0x00, 0x00, 0x00,         // attribute (4 bytes) SSAC = 1+1, SSIC = 0+1
    0x00, 0x11,                     // wFunctionalitySupport (2 bytes) SSID = 0, TX/RX lane count = 1
    0x00, 0x00,                     // wReserved
    0x30, 0x40, 0x0A, 0x00,         // bmSublinkSpeedAttr[0] SSID = 0, LSE = 3, ST = 0(RX), LP = 1, LSM = 10
    0xB0, 0x40, 0x0A, 0x00,         // bmSublinkSpeedAttr[1] SSID = 0, LSE = 3, ST = 2(TX), LP = 1, LSM = 10

    /* Configuration descriptor */
    USB_DESC_LENGTH_CFG,   // this descriptor size
    USB_DESC_TYPE_CFG,     // descriptor type
    CONFIGURATION_LENGTH,  // total length, calculated later
    0x01,                  // config number of interface
    0x01,                  // config value
    0x00,                  // config index
    0xC0,                  // attribute
    0x0D,                  // max power unit=8mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,         // this descriptor size
    USB_DESC_TYPE_INTF,           // descriptor type
    0x00,                         // interface number
    0x00,                         // alternative settings number
    0x02,                         // endpoint number of this interface
    USB_DEVICE_CLASS_MSC,         // class code
    USB_DEVICE_SUBCLASS_MSC_SCSI, // subclass code
    USB_DEVICE_PROTOCOL_MSC_BOT,  // protocol
    0x00,                         // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,       // this descriptor size
    USB_DESC_TYPE_ENDPOINT,         // type
    0x01,                           // EP1 address
    DESC_EP_ATTR_BULK,              // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,        // max packet size
    0x00,                           // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,       // this descriptor size
    USB_DESC_TYPE_ENDPOINT,         // type
    0x82,                           // EP2 address
    DESC_EP_ATTR_BULK,              // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,        // max packet size
    0x00,                           // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval
};
#endif

/* String Device Framework :
   Byte 0 and 1 : Word containing the language ID : 0x0904 for US
   Byte 2       : Byte containing the index of the descriptor
   Byte 3       : Byte containing the length of the descriptor string
 */

static UINT8 MscStr[] GNU_ALIGNED_CACHESAFE = {
    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04, // Language ID
    0x01,       // Index
    0x0c,       // String Length
    0x41, 0x6d, 0x62, 0x61, 0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x49, 0x6e, 0x63, // AmbarellaInc

    /* Product string descriptor : Index 2 */
    0x09, 0x04,
    0x02, // Index
    0x0D, // String Length
    0x41, 0x6d, 0x62, 0x61, 0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x20, 0x4d, 0x53, 0x43, // Ambarella MSC

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04,
    0x03, // Index
    0x04, // String Length
    0x30, 0x30, 0x30, 0x31 // "0001"
};

USBD_DESC_CUSTOM_INFO_s MscDescFramework = {
    .DescFrameworkFs = MscDescFs,
    .DescFrameworkHs = MscDescHs,
#if defined(SVC_USB_SUPER_SPEED)
    .DescFrameworkSs = MscDescSs,
#endif
    .StrFramework    = MscStr,
    .LangIDFramework = LangID,
    .DescSizeFs      = (UINT32)sizeof(MscDescFs),
    .DescSizeHs      = (UINT32)sizeof(MscDescHs),
#if defined(SVC_USB_SUPER_SPEED)
    .DescSizeSs      = (UINT32)sizeof(MscDescSs),
#endif
    .StrSize         = (UINT32)sizeof(MscStr),
    .LangIDSize      = (UINT32)sizeof(LangID),
};

#define RAMDISK_NUMBER          2U
#define RAMDISK_SECTOR_CNT      200U
#define RAMDISK_MEM_SIZE        (RAMDISK_SECTOR_CNT * 512U)
//#define RAMDISK_LAST_LBA        ((RAMDISK_MEM_SIZE/512U)-1U)

static UINT8 RamdiskMem[RAMDISK_NUMBER][RAMDISK_MEM_SIZE] GNU_SECTION_NOZEROINIT;

static INT32 RamdiskRead(UINT32 Index, const UINT32 Sectors, const UINT32 SectorStart, UINT8 *DataPtr)
{
    INT32  nRet = 0;

    if (Index < RAMDISK_NUMBER) {
        AmbaSvcWrap_MisraMemcpy(DataPtr, &RamdiskMem[Index][SectorStart * 512U], 512U * (SIZE_t)Sectors);
    } else {
        nRet = -1;
    }
    return nRet;
}

static INT32 RamdiskWrite(UINT32 Index, const UINT32 Sectors, const UINT32 SectorStart, const UINT8 *DataPtr)
{
    INT32  nRet = 0;

    if (Index < RAMDISK_NUMBER) {
        AmbaSvcWrap_MisraMemcpy(&RamdiskMem[Index][SectorStart * 512U], DataPtr, 512U * (SIZE_t)Sectors);
    } else {
        nRet = -1;
    }
    return nRet;
}

static INT32 RamdiskStatus(USBD_MSC_CARD_STATUS_s *pStatus)
{
    pStatus->MediaPresent = 1;
    pStatus->SectorNumber = RAMDISK_SECTOR_CNT;
    pStatus->SectorSize = 512;
    pStatus->Format = (INT32)FS_FAT32;
    return 0;
}

static INT32 MscdSectorRead(INT32 SlotID, UINT8 *Buffer, UINT32 SectorStart, UINT32 Sectors)
{
    INT32 nRet = 0;

    AmbaMisra_TouchUnused(&SlotID);
    AmbaMisra_TouchUnused(&SectorStart);
    AmbaMisra_TouchUnused(&Sectors);

    if (SlotID == 0) {
        INT32 uRet;
        uRet = AmbaSD0_ReadSector(Buffer, SectorStart, Sectors);
        if (uRet != 0) {
            nRet = -1;
        }
    } else if (SlotID == 1) {
        INT32 uRet;
        uRet = AmbaSD1_ReadSector(Buffer, SectorStart, Sectors);
        if (uRet != 0) {
            nRet = -1;
        }
    } else if (SlotID == 2) {
        nRet = RamdiskRead(1, Sectors, SectorStart, Buffer);
    } else {
        UINT32 Rval;
        AmbaMisra_TypeCast(&Rval, &SlotID);
        AmbaPrint_PrintUInt5("MscdSectorRead(): unsupported SlotID %d.", Rval, 0, 0, 0, 0);
    }
    return nRet;
}

static INT32 MscdSectorWrite(INT32 SlotID, const UINT8 *Buffer, UINT32 SectorStart, UINT32 Sectors)
{
    INT32 nRet = 0;

    AmbaMisra_TouchUnused(&SlotID);
    AmbaMisra_TouchUnused(&SectorStart);
    AmbaMisra_TouchUnused(&Sectors);

    if (SlotID == 0) {
        INT32 uRet;
        UINT8  *Ptr;
        AmbaMisra_TypeCast(&Ptr, &Buffer);
        uRet = AmbaSD0_WriteSector(Ptr, SectorStart, Sectors);
        if (uRet != 0) {
            nRet = -1;
        }
    } else if (SlotID == 1) {
        INT32 uRet;
        UINT8  *Ptr;
        AmbaMisra_TypeCast(&Ptr, &Buffer);
        uRet = AmbaSD1_WriteSector(Ptr, SectorStart, Sectors);
        if (uRet != 0) {
            nRet = -1;
        }
    } else if (SlotID == 2) {
        nRet = RamdiskWrite(1, Sectors, SectorStart, Buffer);
    } else {
        UINT32 Rval;
        AmbaMisra_TypeCast(&Rval, &SlotID);
        AmbaPrint_PrintUInt5("MscdSectorWrite(): unsupported SlotID %d.", Rval, 0, 0, 0, 0);
    }
    return nRet;
}

static INT32 MscdGetMediaInfo(INT32 SlotID, USBD_MSC_CARD_STATUS_s *Status)
{
    INT32   Ret = 0;
    UINT32  Chan;

    AmbaMisra_TouchUnused(&SlotID);

    if ((SlotID == 0) || (SlotID == 1)) {
        AMBA_SD_CARD_STATUS_s CardStatus;

        Chan = (UINT32)SlotID;
        if (AmbaSD_GetCardStatus(Chan, &CardStatus) == OK) {
            Status->MediaPresent = CardStatus.CardIsInit;
            Status->Format = 1; // could be any value other than 0
            Status->SectorNumber = (UINT32)(CardStatus.CardSize / 512U);
            Status->SectorSize = 512;
        } else {
            Ret = -1;
        }
    } else if (SlotID == 2) {
        Ret = RamdiskStatus(Status);
    } else {
        UINT32 Rval;
        AmbaMisra_TypeCast(&Rval, &SlotID);
        AmbaPrint_PrintUInt5("MscdGetMediaInfo(): unsupported SlotID %d.", Rval, 0, 0, 0, 0);
        Ret = -1;
    }
    return Ret;
}

/**
* load of usb msc class
* @param [in] StgDrive main storage drive
* @return none
*/
void AmbaSvcUsb_DeviceClassMscLoad(char StgDrive)
{
    UINT8   Index;
    UINT32  Rval;

    static UINT8 MscdVendorString[8]   = { 0x41, 0x4D, 0x42, 0x41, 0x20, 0x20, 0x20, 0x20 };                                                 // "AMBA    "
    static UINT8 MscdProductString[16] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20, 0x4D, 0x53, 0x43, 0x20, 0x20, 0x20 }; // "Ambarella MSC  "
    static UINT8 MscdRevisionString[4] = { 0x32, 0x30, 0x31, 0x37 };                                                                         // "2017"

    static USBD_MSC_DRIVE_DESC_s MscdDriveDesc = {
        MscdVendorString,
        MscdProductString,
        MscdRevisionString
    };
    static USBD_MSC_FS_OP_s MscdOperations = {
        MscdSectorRead,
        MscdSectorWrite,
        MscdGetMediaInfo
    };
    static USBD_MSC_DEVICE_INFO_s MscdDeviceInfo = {
        &MscdOperations,
        {
            // Drive 0
            {
                1, // Removal
                0, // WriteProtect
                0  // Type, MSC_MEDIA_FAT_DISK(0) - A FAT/FAT32/EXFAT disk drive.
            },
            // Drive 1
            {
                1, // Removal
                0, // WriteProtect
                0  // Type, MSC_MEDIA_FAT_DISK(0) - A FAT/FAT32/EXFAT disk drive.
            },
            // Drive 2
            {
                1, // Removal
                0, // WriteProtect
                0  // Type, MSC_MEDIA_FAT_DISK(0) - A FAT/FAT32/EXFAT disk drive.
            },
        },
        &MscdDriveDesc,
    };

    AmbaMisra_TouchUnused(MscdVendorString);
    AmbaMisra_TouchUnused(MscdProductString);
    AmbaMisra_TouchUnused(MscdRevisionString);

    Rval = AmbaUSBD_MSCSetInfo(&MscdDeviceInfo);
    AmbaPrint_PrintUInt5("AmbaUSBD_Msc_SetInfo() result = 0x%x", Rval, 0, 0, 0, 0);

    //Not enable slot 0 until FS/SD is ready
    if ((StgDrive == 'c') || (StgDrive == 'C')) {
        Index = 0U;
    } else if ((StgDrive == 'd') || (StgDrive == 'D')) {
        Index = 1U;
    } else if ((StgDrive == 'e') || (StgDrive == 'E')) {
        Index = 2U;
    } else {
        Index = 0U;
    }

    Rval = AmbaUSBD_MSCMount(Index);
    AmbaPrint_PrintUInt5("AmbaUSBD_Msc_Mount() result = 0x%x", Rval, 0, 0, 0, 0);
}

/**
* un-load of usb msc class
* @param [in] StgDrive main storage drive
* @return none
*/
void AmbaSvcUsb_DeviceClassMscUnload(char StgDrive)
{
    UINT8   Index;
    UINT32  Rval;

    //Not enable slot 0 until FS/SD is ready
    if ((StgDrive == 'c') || (StgDrive == 'C')) {
        Index = 0U;
    } else if ((StgDrive == 'd') || (StgDrive == 'D')) {
        Index = 1U;
    } else if ((StgDrive == 'e') || (StgDrive == 'E')) {
        Index = 2U;
    } else {
        Index = 0U;
    }

    Rval = AmbaUSBD_MSCUnMount(Index);
    AmbaPrint_PrintUInt5("AmbaUSBD_Msc_UnMount() result = 0x%x", Rval, 0, 0, 0, 0);
}
