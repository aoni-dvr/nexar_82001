/**
 *  @file AppUSB_MSCDevice.c
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
 *  @details USB MSC Device main application.
 */
/**
 * \page page2 MSC Device application
*/
#ifndef APP_USB_H
#include "AppUSB.h"
#endif
#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif
#include "AmbaFS_Format.h"
#include "AmbaMisraFix.h"
#include "AmbaSD.h"

#define VID_VALUE               0x0A, 0x07 // 0x070A
#define PID_VALUE               0x26, 0x40 // 0x4026

#define MAX_PACKET_SIZE_BULK_FS 0x40, 0x00 // 0x40
#define MAX_PACKET_SIZE_BULK_HS 0x00, 0x02 // 0x200
#define MAX_PACKET_SIZE_BULK_SS 0x00, 0x04 // 0x400

#define CONFIGURATION_LENGTH    0x00, 0x00 // always 0x0000

/*-----------------------------------------------------------------------------------------------*\
   Multiple languages are supported on the device, to add
   a language besides English, the unicode language code must
   be appended to the LanguageIdFramework array and the length
   adjusted accordingly.
\*-----------------------------------------------------------------------------------------------*/

static UINT8 usb_desc_lang_id[] = {
    /* English. */
    0x09, 0x04
};

static UINT8 usb_desc_full_speed[] __attribute__ ((aligned(32))) = {
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
    0x00,                         // device serial number index
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

static UINT8 usb_desc_high_speed[] __attribute__ ((aligned(32))) = {
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
    0x00,                         // device serial number index
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

#if (APP_USB_SUPPORT_SUPER_SPEED == 1)

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,           // this descriptor size
    USB_DESC_TYPE_BOS,             // descriptor type
    BOS_DESCRIPTOR_LENGTH_SS, 0x0, // total length
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

static UINT8 usb_desc_super_speed[] __attribute__ ((aligned (32))) = {

    /* Device descriptor */
    USB_DESC_LENGTH_DEV,          // this descriptor size
    USB_DESC_TYPE_DEV,            // device descriptor type
    SPEC_VERSION_SS,              // Spec version
    0x00,                         // class code
    0x00,                         // subclass code
    0x00,                         // procotol code
    0x09,                         // max packet size
    VID_VALUE,                      // VID
    PID_VALUE,                      // PID
    0x01, 0x00,                   // Device release num
    0x01,                         // Manufacturer string index
    0x02,                         // Product string index
    0x03,                         // device serial number index
    0x01,                         // number of possible configuration

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,              // this descriptor size
    USB_DESC_TYPE_BOS,                // descriptor type
    APP_USB_SSP_BOS_DESC_LENGTH,      // total length
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

/* String Device Framework :
   Byte 0 and 1 : Word containing the language ID : 0x0904 for US
   Byte 2       : Byte containing the index of the descriptor
   Byte 3       : Byte containing the length of the descriptor string
 */

static UINT8 usb_desc_strings[] __attribute__ ((aligned(32))) = {
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

    // NOTE: To pass HLK "USB Serial Number Test", iSerialNumber has been set to 0 (optional).
    // Or, each device should have unique serial number.
    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04,
    0x03, // Index
    0x04, // String Length
    0x30, 0x30, 0x30, 0x31 // "0001"
};

USBD_DESC_CUSTOM_INFO_s AppUsbd_MscDescFramework = {
    .DescFrameworkFs = usb_desc_full_speed,
    .DescFrameworkHs = usb_desc_high_speed,
    .DescFrameworkSs = usb_desc_super_speed,
    .StrFramework    = usb_desc_strings,
    .LangIDFramework = usb_desc_lang_id,
    .DescSizeFs      = sizeof(usb_desc_full_speed),
    .DescSizeHs      = sizeof(usb_desc_high_speed),
    .DescSizeSs      = sizeof(usb_desc_super_speed),
    .StrSize         = sizeof(usb_desc_strings),
    .LangIDSize      = sizeof(usb_desc_lang_id),
};

#define RAMDISK_NUMBER          2U
#define RAMDISK_SECTOR_CNT      200U
#define RAMDISK_MEM_SIZE        (RAMDISK_SECTOR_CNT * 512U)

static UINT8 ramdisk_memory[RAMDISK_NUMBER][RAMDISK_MEM_SIZE] __attribute__((section(".bss.noinit")));

static INT32 ramdisk_sector_read(UINT32 Index, const UINT32 Sectors, const UINT32 SectorStart, UINT8 *pDataPtr)
{
    INT32  nret = 0;
    if (Index < RAMDISK_NUMBER) {
        AppUsb_MemoryCopy(pDataPtr, &ramdisk_memory[Index][SectorStart * 512U], 512U * Sectors);
    } else {
        nret = -1;
    }
    return nret;
}

static INT32 ramdisk_sector_write(UINT32 Index, const UINT32 Sectors, const UINT32 SectorStart, const UINT8 *pDataPtr)
{
    INT32  nret = 0;
    if (Index < RAMDISK_NUMBER) {
        AppUsb_MemoryCopy(&ramdisk_memory[Index][SectorStart * 512U], pDataPtr, 512U * Sectors);
    } else {
        nret = -1;
    }
    return nret;
}

static INT32 ramdisk_status_get(USBD_MSC_CARD_STATUS_s *pStatus)
{
    pStatus->MediaPresent = 1;
    pStatus->SectorNumber = RAMDISK_SECTOR_CNT;
    pStatus->SectorSize = 512;
    pStatus->Format = (INT32)FS_FAT32;
    return 0;
}

typedef UINT32 (*SD_READ_FUNC_f)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
typedef UINT32 (*SD_WRITE_FUNC_f)(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);

static UINT32          sd_channel = 0;
static SD_READ_FUNC_f  sd_read_func = NULL;
static SD_WRITE_FUNC_f sd_write_func = NULL;

static INT32 media_sector_read(const INT32 SlotID, UINT8 *Buffer, const UINT32 SectorStart, const UINT32 Sectors)
{
    INT32 nret = 0;

    if (SlotID == 0) {
        UINT32 uret;
        uret = sd_read_func(Buffer, SectorStart, Sectors);
        if (uret != 0U) {
            nret = -1;
        }
    } else if (SlotID == 1) {
        nret = ramdisk_sector_read(0, Sectors, SectorStart, Buffer);
    } else if (SlotID == 2) {
        nret = ramdisk_sector_read(1, Sectors, SectorStart, Buffer);
    } else {
        UINT32 rval;
        AmbaMisra_TypeCast32(&rval, &SlotID);
        AppUsb_PrintUInt5("media_sector_read(): unsupported SlotID %d.", rval, 0, 0, 0, 0);
    }
    return nret;
}

static INT32 media_sector_write(const INT32 SlotID, const UINT8 *Buffer, const UINT32 SectorStart, const UINT32 Sectors)
{
    INT32 nret = 0;

    if (SlotID == 0) {
        UINT32 uret;
        UINT8  *ptr;
        AmbaMisra_TypeCast32(&ptr, &Buffer);
        uret = sd_write_func(ptr, SectorStart, Sectors);
        if (uret != 0U) {
            nret = -1;
        }
    } else if (SlotID == 1) {
        nret = ramdisk_sector_write(0, Sectors, SectorStart, Buffer);
    } else if (SlotID == 2) {
        nret = ramdisk_sector_write(1, Sectors, SectorStart, Buffer);
    } else {
        UINT32 rval;
        AmbaMisra_TypeCast32(&rval, &SlotID);
        AppUsb_PrintUInt5("media_sector_write(): unsupported SlotID %d.", rval, 0, 0, 0, 0);
    }
    return nret;
}

static INT32 media_info_get(const INT32 SlotID, USBD_MSC_CARD_STATUS_s *Status)
{
    INT32 nret = 0;

    if (SlotID == 0) {
        AMBA_SD_CARD_STATUS_s card_status;
        if (AmbaSD_GetCardStatus(sd_channel, &card_status) == OK) {
            Status->MediaPresent = card_status.CardIsInit;
            Status->Format = 1; // could be any value other than 0
            Status->SectorNumber = (UINT32)(card_status.CardSize / 512U);
            Status->SectorSize = 512;
        } else {
            nret = -1;
        }
    } else if ((SlotID == 1) || (SlotID == 2)) {
        nret = ramdisk_status_get(Status);
    } else {
        UINT32 rval;
        AmbaMisra_TypeCast32(&rval, &SlotID);
        AppUsb_PrintUInt5("media_info_get(): unsupported SlotID %d.", rval, 0, 0, 0, 0);
        nret = -1;
    }
    return nret;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Set information for MSC device application. It also mounts drivers into USB MSC device system.
*/
void AppMscd_SetInfo(void)
{
    UINT32 func_uret;

    static UINT8 vendor_string[8]   = { 0x41, 0x4D, 0x42, 0x41, 0x20, 0x20, 0x20, 0x20 };                                                 // "AMBA    "
    static UINT8 product_string[16] = { 0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C, 0x61, 0x20, 0x4D, 0x53, 0x43, 0x20, 0x20, 0x20 }; // "Ambarella MSC  "
    static UINT8 revision_string[4] = { 0x32, 0x30, 0x31, 0x37 };                                                                         // "2017"

    static USBD_MSC_DRIVE_DESC_s drive_desc = {
        vendor_string,
        product_string,
        revision_string
    };
    static USBD_MSC_FS_OP_s operations = {
        media_sector_read,
        media_sector_write,
        media_info_get
    };
    static USBD_MSC_DEVICE_INFO_s device_info = {
        &operations,
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
        &drive_desc,
    };

#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B)

    extern UINT32 AmbaSD1_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    extern UINT32 AmbaSD1_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);

    sd_read_func = AmbaSD1_ReadSector;
    sd_write_func = AmbaSD1_WriteSector;
    sd_channel = 1;

#else

    extern UINT32 AmbaSD0_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);
    extern UINT32 AmbaSD0_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors);

    sd_read_func = AmbaSD0_ReadSector;
    sd_write_func = AmbaSD0_WriteSector;
    sd_channel = 0;

#endif


    func_uret = AmbaUSBD_MSCSetInfo(&device_info);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo(): can't set MSC information, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AmbaUSBD_Msc_SetInfo() set MSC information successfully.", 0, 0, 0, 0, 0);
    }
    //Not enable slot 0 until FS/SD is ready
    func_uret = AmbaUSBD_MSCMount(0);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() can't mount drive 0, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() drive 0 mounted", 0, 0, 0, 0, 0);
    }

    func_uret = AmbaUSBD_MSCMount(1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() can't mount drive 1, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() drive 1 mounted", 0, 0, 0, 0, 0);
    }
    func_uret = AmbaUSBD_MSCMount(2);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() can't mount drive 2, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscSetInfo() drive 2 mounted", 0, 0, 0, 0, 0);
    }

}
/**
 * Un-mount drivers from USB MSC device system.
*/
void AppMscd_Unmount(void)
{
    UINT32 func_uret;

    //Not enable slot 0 until FS/SD is ready
    func_uret = AmbaUSBD_MSCUnMount(0);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() can't unmount drive 0, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() drive 0 unmounted", 0, 0, 0, 0, 0);
    }
    func_uret = AmbaUSBD_MSCUnMount(1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() can't unmount drive 1, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() drive 1 unmounted", 0, 0, 0, 0, 0);
    }
    func_uret = AmbaUSBD_MSCUnMount(2);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() can't unmount drive 2, result = 0x%X", func_uret, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_MscUnmount() drive 2 unmounted", 0, 0, 0, 0, 0);
    }
}
/** @} */
