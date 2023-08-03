/**
 *  @file AppUSB_MtpdDesc.c
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
 *  @details USB MTP device class descriptors for APP/MW.
 */


#ifndef APP_USB_H
#include "AppUSB.h"
#endif
#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#ifndef APPUSB_MTPD_H
#include "AppUSB_MTPDevice.h"
#endif

#define MTPD_VID_VALUE          0xE8, 0x04 // 0x04E8
#define MTPD_PID_VALUE          0xC5, 0x68 // 0x68C5

#define MAX_PACKET_SIZE_INTR_FS 0x20, 0x00 // 0x40
#define MAX_PACKET_SIZE_BULK_FS 0x40, 0x00 // 0x40
#define MAX_PACKET_SIZE_INTR_HS 0x20, 0x00 // 0x20
#define MAX_PACKET_SIZE_BULK_HS 0x00, 0x02 // 0x200
#define MAX_PACKET_SIZE_INTR_SS 0x20, 0x00 // 0x20
#define MAX_PACKET_SIZE_BULK_SS 0x00, 0x04 // 0x400

#define CONFIGURATION_LENGTH    0x00, 0x00 // always 0x0000

#define MTP_VENDOR_REQUEST      0x54

static UINT8 usb_desc_lang_id[] = {
    /* English. */
    0x09, 0x04
};

/*-----------------------------------------------------------------------------------------------*\
* MTP/PTP Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/
static UINT8 usb_desc_full_speed[] __attribute__ ((aligned(32))) = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,             // this descriptor size
    USB_DESC_TYPE_DEV,               // device descriptor type
    SPEC_VERSION_FS,                 // Spec version
    0x00,                            // class code
    0x00,                            // subclass code
    0x00,                            // procotol code
    0x40,                            // max packet size
    MTPD_VID_VALUE,                  // VID
    MTPD_PID_VALUE,                  // PID
    0x00, 0x00,                      // Device release num
    0x00,                            // Manufacturer string index
    0x00,                            // Product string index
    0x00,                            // device serial number index
    0x01,                            // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,       // this descriptor size
    USB_DESC_TYPE_QUALIFIER,         // descriptor type
    SPEC_VERSION_FS,                 // Spec version
    0x00,                            // class code
    0x00,                            // subclass code
    0x00,                            // procotol code
    0x40,                            // max packet size
    0x01,                            // configuration number
    0x00,                            // reserved

    /* Configuration descriptor */
    USB_DESC_LENGTH_CFG,             // this descriptor size
    USB_DESC_TYPE_CFG,               // descriptor type
    CONFIGURATION_LENGTH,            // total length
    0x01,                            // config number of interface
    0x01,                            // config value
    0x00,                            // config index
    0xc0,                            // attribute
    0x32,                            // max power unit=2mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,            // this descriptor size
    USB_DESC_TYPE_INTF,              // descriptor type
    0x00,                            // interface number
    0x00,                            // alternative settings number
    0x03,                            // endpoint number of this interface
    USB_DEVICE_CLASS_STILL_IMAGE,    // class code
    USB_DEVICE_SUBCLASS_STILL_IMAGE, // subclass code
    USB_DEVICE_PROTOCOL_STILL_IMAGE, // protocol
    0x00,                            // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x01,                            // EP2 address
    DESC_EP_ATTR_BULK,               // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,         // max packet size
    0x00,                            // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x82,                            // EP1 address
    DESC_EP_ATTR_BULK,               // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,         // max packet size
    0x00,                            // interval

    /* Endpoint descriptor (Interrupt In) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x83,                            // EP3 address
    DESC_EP_ATTR_INT,                // endpoint attribute = INTERRUPT
    MAX_PACKET_SIZE_INTR_FS,         // max packet size
    0x04                             // interval
};

static UINT8 usb_desc_high_speed[] __attribute__ ((aligned(32))) = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,             // this descriptor size
    USB_DESC_TYPE_DEV,               // device descriptor type
    SPEC_VERSION_HS,                 // Spec version
    0x00,                            // class code
    0x00,                            // subclass code
    0x00,                            // procotol code
    0x40,                            // max packet size
    MTPD_VID_VALUE,                  // VID
    MTPD_PID_VALUE,                  // PID
    0x01, 0x00,                      // Device release num
    0x01,                            // Manufacturer string index
    0x02,                            // Product string index
    0x03,                            // device serial number index
    0x01,                            // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,       // this descriptor size
    USB_DESC_TYPE_QUALIFIER,         // descriptor type
    SPEC_VERSION_HS,                 // Spec version
    0x00,                            // class code
    0x00,                            // subclass code
    0x00,                            // procotol code
    0x40,                            // max packet size
    0x01,                            // configuration number
    0x00,                            // reserved

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
    USB_DESC_LENGTH_CFG,             // this descriptor size
    USB_DESC_TYPE_CFG,               // descriptor type
    CONFIGURATION_LENGTH,            // total length
    0x01,                            // config number of interface
    0x01,                            // config value
    0x00,                            // config index
    0xC0,                            // attribute
    0x32,                            // max power unit=2mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,            // this descriptor size
    USB_DESC_TYPE_INTF,              // descriptor type
    0x00,                            // interface number
    0x00,                            // alternative settings number
    0x03,                            // endpoint number of this interface
    USB_DEVICE_CLASS_STILL_IMAGE,    // class code
    USB_DEVICE_SUBCLASS_STILL_IMAGE, // subclass code
    USB_DEVICE_PROTOCOL_STILL_IMAGE, // protocol
    0x00,                            // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x01,                            // EP1 address
    DESC_EP_ATTR_BULK,               // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,         // max packet size
    0x00,                            // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x82,                            // EP2 address
    DESC_EP_ATTR_BULK,               // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,         // max packet size
    0x00,                            // interval

    /* Endpoint descriptor (Interrupt In) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x83,                            // EP3 address
    DESC_EP_ATTR_INT,                // endpoint attribute = INTERRUPT
    MAX_PACKET_SIZE_INTR_HS,         // max packet size
    0x04                             // interval
};

static UINT8 usb_desc_super_speed[] __attribute__ ((aligned (32))) = {

    /* Device descriptor */
    USB_DESC_LENGTH_DEV,         // this descriptor size
    USB_DESC_TYPE_DEV,           // device descriptor type
    SPEC_VERSION_SS,             // Spec version
    0x00,                        // class code
    0x00,                        // subclass code
    0x00,                        // procotol code
    0x09,                        // max packet size
    MTPD_VID_VALUE,              // VID
    MTPD_PID_VALUE,              // PID
    0x01, 0x00,                  // Device release num
    0x01,                        // Manufacturer string index
    0x02,                        // Product string index
    0x03,                        // device serial number index
    0x01,                        // number of possible configuration

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,            // this descriptor size
    USB_DESC_TYPE_BOS,              // descriptor type
    APP_USB_SSP_BOS_DESC_LENGTH,    // total length
    0x3,                            // number of capability.

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
    0xFF, 0x07,                    // U2 Device Exit Latency

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
    USB_DESC_LENGTH_CFG,           // this descriptor size
    USB_DESC_TYPE_CFG,             // descriptor type
    CONFIGURATION_LENGTH,          // total length
    0x01,                          // config number of interface
    0x01,                          // config value
    0x00,                          // config index
    0xC0,                          // attribute
    0x0D,                          // max power unit=8mA

    /* Interface descriptor */
    USB_DESC_LENGTH_INTF,            // this descriptor size
    USB_DESC_TYPE_INTF,              // descriptor type
    0x00,                            // interface number
    0x00,                            // alternative settings number
    0x03,                            // endpoint number of this interface
    USB_DEVICE_CLASS_STILL_IMAGE,    // class code
    USB_DEVICE_SUBCLASS_STILL_IMAGE, // subclass code
    USB_DEVICE_PROTOCOL_STILL_IMAGE, // protocol
    0x00,                            // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    0x01,                            // EP1 address
    DESC_EP_ATTR_BULK,               // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,         // max packet size
    0x00,                            // interval

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

    /* Endpoint descriptor (Interrupt In) */
    USB_DESC_LENGTH_ENDPOINT,      // this descriptor size
    USB_DESC_TYPE_ENDPOINT,        // type
    0x83,                          // EP3 address
    DESC_EP_ATTR_INT,              // endpoint attribute = INTERRUPT
    MAX_PACKET_SIZE_INTR_SS,       // max packet size
    0x04,                          // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x00,                           // Maximum Burst
    0x00,                           // bmAttribute
    MAX_PACKET_SIZE_INTR_SS,        // BytesPerInterval
};


/* String Device Framework :
   Byte 0 and 1 : Word containing the language ID : 0x0904 for US or 0x0000 for none.
   Byte 2       : Byte containing the index of the descriptor
   Byte 3       : Byte containing the length of the descriptor string

   The last string entry can be the optional Microsoft String descriptor.
 */

static UINT8 usb_desc_strings[] __attribute__ ((aligned(32))) = {
    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04,                                     // Language ID
    0x01,                                           // Index
    0x0C,                                           // String Length
    0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C,
    0x61, 0x49, 0x6E, 0x63,                         // AmbarellaInc

    /* Product string descriptor : Index 2 */
    0x09, 0x04,                                     // Language ID
    0x02,                                           // Index
    0x08,                                           // String Length
    0x41, 0x4D, 0x42, 0x41, 0x20, 0x4D, 0x54, 0x50, // AMBA MTP

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04,
    0x03,
    0x04,
    0x30, 0x30, 0x30, 0x30,     // 0000

    /* Microsoft OS string descriptor : Index 0xEE. String is MSFT100.
       The last byte is the vendor code used to filter Vendor specific commands.
       The vendor commands will be executed in the class.
       This code can be anything but must not be 0x66 or 0x67 which are PIMA class commands.  */
    0x00, 0x00, 0xEE, 0x08,
    0x4D, 0x53, 0x46, 0x54,
    0x31, 0x30, 0x30,
    MTP_VENDOR_REQUEST
};

USBD_DESC_CUSTOM_INFO_s g_mtp_desc_framework = {
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

