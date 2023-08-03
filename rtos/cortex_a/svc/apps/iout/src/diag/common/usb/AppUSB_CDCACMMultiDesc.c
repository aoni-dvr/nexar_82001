/**
 *  @file AppUSB_CDCACMDesc.c
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
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include "AppUSB_CDCACMDevice.h"

#define CDC_ACM_MULTI_VID_VALUE       0x55, 0x42 // 0x4255
#define CDC_ACM_MULTI_PID_VALUE       0x54, 0x11 // 0x0054
#define DEV_REL_NUMBER                0x00, 0x00 // 0x0000

#define MAX_PACKET_SIZE_INTR_FS 0x20, 0x00 // 0x20
#define MAX_PACKET_SIZE_BULK_FS 0x40, 0x00 // 0x40
#define MAX_PACKET_SIZE_INTR_HS 0x20, 0x00 // 0x20
#define MAX_PACKET_SIZE_BULK_HS 0x00, 0x02 // 0x200
#define MAX_PACKET_SIZE_INTR_SS 0x20, 0x00 // 0x20
#define MAX_PACKET_SIZE_BULK_SS 0x00, 0x04 // 0x400

#define BCD_CDC_VALUE           0x10, 0x01 // 0x0110
#define CONFIGURATION_LENGTH    0x00, 0x00 // always 0x0000

#define CDC0_INT_IN_EP           0x81
#define CDC0_BULK_OUT_EP         0x02
#define CDC0_BULK_IN_EP          0x82
#define CDC1_INT_IN_EP           0x83
#define CDC1_BULK_OUT_EP         0x04
#define CDC1_BULK_IN_EP          0x84

static UINT8 LangID[] = {
    /* English. */
    0x09, 0x04
};

/*-----------------------------------------------------------------------------------------------*\
* CDC-ACM-Multi Class Default Descriptor
\*-----------------------------------------------------------------------------------------------*/

static UINT8 CdcAcmMultiDescFs[] __attribute__ ((aligned(32))) = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,                  // this descriptor size
    USB_DESC_TYPE_DEV,                    // device descriptor type
    SPEC_VERSION_FS,                      // Spec version
    0xEF,                                 // class code
    0x02,                                 // subclass code
    0x01,                                 // procotol code
    0x40,                                 // max packet size
    CDC_ACM_MULTI_VID_VALUE,              // VID
    CDC_ACM_MULTI_PID_VALUE,              // PID
    DEV_REL_NUMBER,                       // Device release num
    0x01,                                 // Manufacturer string index
    0x02,                                 // Product string index
    0x03,                                 // device serial number index
    0x01,                                 // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,            // this descriptor size
    USB_DESC_TYPE_QUALIFIER,              // device descriptor type
    SPEC_VERSION_FS,                      // Spec version
    USB_DEVICE_CLASS_CDC,                 // class code
    USB_DEVICE_SUBCLASS_CDC,              // subclass code
    USB_DEVICE_PROTOCOL_CDC,              // procotol code
    64,                                   // max packet size
    0x01,                                 // configuration number
    0x00,                                 // reserved

    /* Configuration descriptor */
    USB_DESC_LENGTH_CFG,                  // this descriptor size
    USB_DESC_TYPE_CFG,                    // descriptor type
    CONFIGURATION_LENGTH,                 // total length
    0x04,                                 // config number of interface
    0x01,                                 // config value
    0x00,                                 // config index
    0xC0,                                 // attribute
    0x32,                                 // max power unit=2mA

    //
    // Instance 0
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,                  // this descriptor size
    USB_DESC_TYPE_IAD,                    // descriptor type
    0x00,                                 // the first iad interface
    0x02,                                 // number of iad interface
    0x02,                                 // Function Class
    0x02,                                 // Function SubClass
    0x00,                                 // Function Protocal
    0x00,                                 // Function

    // Interface 0 descriptor
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x00,                                 // interface number
    0x00,                                 // alternative settings number
    0x01,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,              // class code
    USB_CDC_SUBCLASS_ACM,                 // subclass code
    USB_CDC_PROTOCOL_V250,                // protocol
    0x00,                                 // interface index

    /* Header functional descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,     // descriptor subtype
    BCD_CDC_VALUE,                        // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT, // descriptor subtype
    0x00,                                 // capabilities
    0x01,                                 // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,        // descriptor subtype
    0x02,                                 // capabilities

    /* Union Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC,      // descriptor subtype
    0x00,                                 // Master Interface
    0x01,                                 // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_INT_IN_EP,                       // EP address
    DESC_EP_ATTR_INT,                     // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_FS,              // max packet size
    0x10,                                 // interval

    // Interface 1 descriptor
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x01,                                 // interface number
    0x00,                                 // alternative settings number
    0x02,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,         // class code
    0x00,                                 // subclass code
    0x00,                                 // protocol
    0x00,                                 // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_BULK_OUT_EP,                     // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,              // max packet size
    0x00,                                 // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_BULK_IN_EP,                      // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,              // max packet size
    0x00,                                 // interval

    //
    // Instance 1
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,                  // this descriptor size
    USB_DESC_TYPE_IAD,                    // descriptor type
    0x02,                                 // the first iad interface
    0x02,                                 // number of iad interface
    0x02,                                 // Function Class
    0x02,                                 // Function SubClass
    0x00,                                 // Function Protocal
    0x00,                                 // Function

    // Interface 2 descriptor
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x02,                                 // interface number
    0x00,                                 // alternative settings number
    0x01,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,              // class code
    USB_CDC_SUBCLASS_ACM,                 // subclass code
    USB_CDC_PROTOCOL_V250,                // protocol
    0x00,                                 // interface index

    /* Header functional descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,     // descriptor subtype
    BCD_CDC_VALUE,                        // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT, // descriptor subtype
    0x00,                                 // capabilities
    0x03,                                 // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,        // descriptor subtype
    0x02,                                 // capabilities

    /* Union Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC,      // descriptor subtype
    0x02,                                 // Master Interface
    0x03,                                 // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_INT_IN_EP,                       // EP address
    DESC_EP_ATTR_INT,                     // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_FS,              // max packet size
    0x10,                                 // interval

    // Interface 3 descriptor
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x03,                                 // interface number
    0x00,                                 // alternative settings number
    0x02,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,         // class code
    0x00,                                 // subclass code
    0x00,                                 // protocol
    0x00,                                 // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_BULK_OUT_EP,                     // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,              // max packet size
    0x00,                                 // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_BULK_IN_EP,                      // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_FS,              // max packet size
    0x00                                  // interval

};

static UINT8 CdcAcmMultiDescHs[] __attribute__ ((aligned(32))) = {
    /* Device descriptor */
    USB_DESC_LENGTH_DEV,                  // this descriptor size
    USB_DESC_TYPE_DEV,                    // device descriptor type
    SPEC_VERSION_HS,                      // Spec version
    0xEF,                                 // class code
    0x02,                                 // subclass code
    0x01,                                 // procotol code
    0x40,                                 // max packet size
    CDC_ACM_MULTI_VID_VALUE,                    // VID
    CDC_ACM_MULTI_PID_VALUE,                    // PID
    DEV_REL_NUMBER,                       // Device release num
    0x01,                                 // Manufacturer string index
    0x02,                                 // Product string index
    0x03,                                 // device serial number index
    0x01,                                 // number of possible configuration

    /* Device qualifier descriptor */
    USB_DESC_LENGTH_QUALIFIER,            // this descriptor size
    USB_DESC_TYPE_QUALIFIER,              // device descriptor type
    SPEC_VERSION_HS,                      // Spec version
    USB_DEVICE_CLASS_CDC,                 // class code
    USB_DEVICE_SUBCLASS_CDC,              // subclass code
    USB_DEVICE_PROTOCOL_CDC,              // procotol code
    64,                                   // max packet size
    0x01,                                 // configuration number
    0x00,                                 // reserved

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
    USB_DESC_LENGTH_CFG,                  // this descriptor size
    USB_DESC_TYPE_CFG,                    // descriptor type
    CONFIGURATION_LENGTH,                 // total length
    0x04,                                 // config number of interface
    0x01,                                 // config value
    0x00,                                 // config index
    0xC0,                                 // attribute
    0x32,                                 // max power unit=2mA

    //
    // Instance 0
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,                  // this descriptor size
    USB_DESC_TYPE_IAD,                    // descriptor type
    0x00,                                 // the first iad interface
    0x02,                                 // number of iad interface
    0x02,                                 // Function Class
    0x02,                                 // Function SubClass
    0x00,                                 // Function Protocal
    0x00,                                 // Function

    /* Interface 0 descriptor */
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x00,                                 // interface number
    0x00,                                 // alternative settings number
    0x01,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,              // class code
    USB_CDC_SUBCLASS_ACM,                 // subclass code
    USB_CDC_PROTOCOL_V250,                // protocol
    0x04,                                 // interface index

    /* Header functional descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,     // descriptor subtype
    BCD_CDC_VALUE,                        // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT, // descriptor subtype
    0x00,                                 // capabilities
    0x01,                                 // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,        // descriptor subtype
    0x02,                                 // capabilities

    /* Union Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC,      // descriptor subtype
    0x00,                                 // Master Interface
    0x01,                                 // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_INT_IN_EP,                       // EP address
    DESC_EP_ATTR_INT,                     // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_HS,              // max packet size
    0x10,                                 // interval

    /* Interface descriptor 1 */
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x01,                                 // interface number
    0x00,                                 // alternative settings number
    0x02,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,         // class code
    0x00,                                 // subclass code
    0x00,                                 // protocol
    0x00,                                 // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_BULK_OUT_EP,                     // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,              // max packet size
    0x00,                                 // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC0_BULK_IN_EP,                      // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,              // max packet size
    0x00,                                 // interval

    //
    // Instance 1
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,                  // this descriptor size
    USB_DESC_TYPE_IAD,                    // descriptor type
    0x02,                                 // the first iad interface
    0x02,                                 // number of iad interface
    0x02,                                 // Function Class
    0x02,                                 // Function SubClass
    0x00,                                 // Function Protocal
    0x00,                                 // Function

    /* Interface 2 descriptor */
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x02,                                 // interface number
    0x00,                                 // alternative settings number
    0x01,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,              // class code
    USB_CDC_SUBCLASS_ACM,                 // subclass code
    USB_CDC_PROTOCOL_V250,                // protocol
    0x04,                                 // interface index

    /* Header functional descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,     // descriptor subtype
    BCD_CDC_VALUE,                        // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT, // descriptor subtype
    0x00,                                 // capabilities
    0x03,                                 // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,        // descriptor subtype
    0x02,                                 // capabilities

    /* Union Functional Descriptor */
    0x05,                                 // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,           // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC,      // descriptor subtype
    0x02,                                 // Master Interface
    0x03,                                 // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_INT_IN_EP,                       // EP address
    DESC_EP_ATTR_INT,                     // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_HS,              // max packet size
    0x10,                                 // interval

    /* Interface descriptor 3 */
    USB_DESC_LENGTH_INTF,                 // this descriptor size
    USB_DESC_TYPE_INTF,                   // descriptor type
    0x03,                                 // interface number
    0x00,                                 // alternative settings number
    0x02,                                 // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,         // class code
    0x00,                                 // subclass code
    0x00,                                 // protocol
    0x00,                                 // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_BULK_OUT_EP,                     // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,              // max packet size
    0x00,                                 // interval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,             // this descriptor size
    USB_DESC_TYPE_ENDPOINT,               // type
    CDC1_BULK_IN_EP,                      // EP address
    DESC_EP_ATTR_BULK,                    // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_HS,              // max packet size
    0x00                                  // interval
};

static UINT8 CdcAcmMultiDescSs[]  __attribute__ ((aligned (32))) = {

    /* Device descriptor */
    USB_DESC_LENGTH_DEV,       // this descriptor size
    USB_DESC_TYPE_DEV,         // device descriptor type
    SPEC_VERSION_SS,           // Spec version
    0xEF,                      // class code
    0x02,                      // subclass code
    0x01,                      // procotol code
    0x09,                      // max packet size
    CDC_ACM_MULTI_VID_VALUE,   // VID
    CDC_ACM_MULTI_PID_VALUE,   // PID
    0x00, 0x00,                // Device release num
    0x01,                      // Manufacturer string index
    0x02,                      // Product string index
    0x03,                      // device serial number index
    0x01,                      // number of possible configuration

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
    USB_DESC_LENGTH_CFG,  // this descriptor size
    USB_DESC_TYPE_CFG,    // descriptor type
    CONFIGURATION_LENGTH, // total length
    0x04,                 // config number of interface
    0x01,                 // config value
    0x00,                 // config index
    0xC0,                 // attribute
    0x0D,                 // max power unit=8mA

    //
    // Instance 0
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,  // this descriptor size
    USB_DESC_TYPE_IAD,    // descriptor type
    0x00,                 // the first iad interface
    0x02,                 // number of iad interface
    0x02,                 // Function Class
    0x02,                 // Function SubClass
    0x00,                 // Function Protocal
    0x00,                 // Function

    /* Interface 0 descriptor */
    USB_DESC_LENGTH_INTF,     // this descriptor size
    USB_DESC_TYPE_INTF,       // descriptor type
    0x00,                     // interface number
    0x00,                     // alternative settings number
    0x01,                     // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,  // class code
    USB_CDC_SUBCLASS_ACM,     // subclass code
    USB_CDC_PROTOCOL_V250,    // protocol
    0x04,                     // interface index

    /* Header functional descriptor */
    0x05,                              // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,        // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,  // descriptor subtype
    BCD_CDC_VALUE,                     // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                    // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,              // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT,    // descriptor subtype
    0x00,                                    // capabilities
    0x01,                                    // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                           // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,     // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,  // descriptor subtype
    0x02,                           // capabilities

    /* Union Functional Descriptor */
    0x05,                            // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,      // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC, // descriptor subtype
    0x00,                            // Master Interface
    0x01,                            // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    CDC0_INT_IN_EP,                  // EP address
    DESC_EP_ATTR_INT,                // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_SS,         // max packet size
    0x10,                            // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x00,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    /* Interface descriptor 1 */
    USB_DESC_LENGTH_INTF,          // this descriptor size
    USB_DESC_TYPE_INTF,            // descriptor type
    0x01,                          // interface number
    0x00,                          // alternative settings number
    0x02,                          // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,  // class code
    0x00,                          // subclass code
    0x00,                          // protocol
    0x00,                          // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,      // this descriptor size
    USB_DESC_TYPE_ENDPOINT,        // type
    CDC0_BULK_OUT_EP,              // EP address
    DESC_EP_ATTR_BULK,             // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,       // max packet size
    0x00,                          // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,       // this descriptor size
    USB_DESC_TYPE_ENDPOINT,         // type
    CDC0_BULK_IN_EP,                // EP address
    DESC_EP_ATTR_BULK,              // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,        // max packet size
    0x00,                           // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    //
    // Instance 1
    //

    /* Interface association descriptor. 8 bytes.  */
    USB_DESC_LENGTH_IAD,  // this descriptor size
    USB_DESC_TYPE_IAD,    // descriptor type
    0x02,                 // the first iad interface
    0x02,                 // number of iad interface
    0x02,                 // Function Class
    0x02,                 // Function SubClass
    0x00,                 // Function Protocal
    0x00,                 // Function

    /* Interface 2 descriptor */
    USB_DESC_LENGTH_INTF,     // this descriptor size
    USB_DESC_TYPE_INTF,       // descriptor type
    0x02,                     // interface number
    0x00,                     // alternative settings number
    0x01,                     // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC,  // class code
    USB_CDC_SUBCLASS_ACM,     // subclass code
    USB_CDC_PROTOCOL_V250,    // protocol
    0x04,                     // interface index

    /* Header functional descriptor */
    0x05,                              // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,        // descriptor type
    USB_CDC_DESC_SUBTYPE_HEADER_FUNC,  // descriptor subtype
    BCD_CDC_VALUE,                     // bcdCDC

    /* Call Managment Functional Descriptor */
    0x05,                                    // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,              // descriptor type
    USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT,    // descriptor subtype
    0x00,                                    // capabilities
    0x03,                                    // Data interface Number

    /* ACM Functional Descriptor */
    0x04,                           // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,     // descriptor type
    USB_CDC_DESC_SUBTYPE_ACM_FUNC,  // descriptor subtype
    0x02,                           // capabilities

    /* Union Functional Descriptor */
    0x05,                            // this descriptor size
    USB_DESC_TYPE_CS_INTERFACE,      // descriptor type
    USB_CDC_DESC_SUBTYPE_UNION_FUNC, // descriptor subtype
    0x02,                            // Master Interface
    0x03,                            // Slave Interface

    /* Endpoint descriptor (Interrupt) */
    USB_DESC_LENGTH_ENDPOINT,        // this descriptor size
    USB_DESC_TYPE_ENDPOINT,          // type
    CDC1_INT_IN_EP,                  // EP address
    DESC_EP_ATTR_INT,                // endpoint attribute = Interrupt
    MAX_PACKET_SIZE_INTR_SS,         // max packet size
    0x10,                            // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x00,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    /* Interface descriptor 3 */
    USB_DESC_LENGTH_INTF,          // this descriptor size
    USB_DESC_TYPE_INTF,            // descriptor type
    0x03,                          // interface number
    0x00,                          // alternative settings number
    0x02,                          // endpoint number of this interface
    USB_INTERFACE_CLASS_CDC_DATA,  // class code
    0x00,                          // subclass code
    0x00,                          // protocol
    0x00,                          // interface index

    /* Endpoint descriptor (Bulk Out) */
    USB_DESC_LENGTH_ENDPOINT,      // this descriptor size
    USB_DESC_TYPE_ENDPOINT,        // type
    CDC1_BULK_OUT_EP,              // EP address
    DESC_EP_ATTR_BULK,             // endpoint attribute = BULK
    MAX_PACKET_SIZE_BULK_SS,       // max packet size
    0x00,                          // interval

    /* Super Speed Endpoint Companion descriptor */
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    /* Endpoint descriptor (Bulk In) */
    USB_DESC_LENGTH_ENDPOINT,       // this descriptor size
    USB_DESC_TYPE_ENDPOINT,         // type
    CDC1_BULK_IN_EP,                // EP address
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

static UINT8 CdcAcmMultiStr[] __attribute__ ((aligned(32))) = {
    /* Manufacturer string descriptor : Index 1 Amba */
    0x09, 0x04,                         // Language ID
    0x01,                               // Index
    0x04,                               // String Length
    0x41, 0x6D, 0x62, 0x61,             // Amba

    /* Product string descriptor : Index 2 Amba cdcacm class*/
    0x09, 0x04,
    0x02,                               // Index
    0x11,                               // String Length
    0x41, 0x6D, 0x62, 0x61, 0x20, 0x63, 0x64, 0x63,
    0x61, 0x63, 0x6D, 0x20, 0x63, 0x6C, 0x61, 0x73,
    0x73,                               // Amba cdcacm class

    /* Serial Number string descriptor : Index 3 0001 */
    0x09, 0x04,
    0x03,                               // Index
    0x04,                               // String Length
    0x30, 0x30, 0x30, 0x31,             // 0001

    /* Interface 1 string descriptor : Index 4 Amba cdcacm class*/
    0x09, 0x04,
    0x04,                               // Index
    0x0E,                               // String Length
    0x41, 0x6D, 0x62, 0x61, 0x72, 0x65, 0x6C, 0x6C,
    0x61, 0x20, 0x55, 0x41, 0x52, 0x54, // Ambarella UART
};

USBD_DESC_CUSTOM_INFO_s g_cdc_acm_multi_desc_framework = {
    .DescFrameworkFs = CdcAcmMultiDescFs,
    .DescFrameworkHs = CdcAcmMultiDescHs,
    .DescFrameworkSs = CdcAcmMultiDescSs,
    .StrFramework    = CdcAcmMultiStr,
    .LangIDFramework = LangID,
    .DescSizeFs      = sizeof(CdcAcmMultiDescFs),
    .DescSizeHs      = sizeof(CdcAcmMultiDescHs),
    .DescSizeSs      = sizeof(CdcAcmMultiDescSs),
    .StrSize         = sizeof(CdcAcmMultiStr),
    .LangIDSize      = sizeof(LangID),
};

