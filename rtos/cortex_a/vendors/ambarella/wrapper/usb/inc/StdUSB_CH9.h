/**
 *  @file StdUSB_CH9.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Standard USB Chapter 9 definitions header file.
 */

#ifndef STD_USB_CH9_H
#define STD_USB_CH9_H

#ifndef STD_USB_H
#include <StdUSB.h>
#endif

/** Default Speed ID defined in XHCI Specification */
#define CH9_USB_SPEED_UNKNOWN     ((UINT32)0U) //!< unknow speed - before enumeration
#define CH9_USB_SPEED_LOW         ((UINT32)1U) //!< (1,5Mb/s)
#define CH9_USB_SPEED_FULL        ((UINT32)2U) //!< usb 1.1 (12Mb/s)
#define CH9_USB_SPEED_HIGH        ((UINT32)3U) //!< usb 2.0 (480Mb/s)
#define CH9_USB_SPEED_SUPER       ((UINT32)4U) //!< usb 3.0 GEN 1  (5Gb/s)
#define CH9_USB_SPEED_SUPER_PLUS  ((UINT32)5U) //!< usb 3.1 GEN2 (10Gb/s)

/** Data transfer direction */
#define    CH9_USB_DIR_HOST_TO_DEVICE 0U
#define    CH9_USB_DIR_DEVICE_TO_HOST (1U << 7U)

/** Type of request */
#define    CH9_USB_REQ_TYPE_MASK           (3U << 5U)
#define    CH9_USB_REQ_TYPE_STANDARD       (0U << 5U)
#define    CH9_USB_REQ_TYPE_CLASS          (1U << 5U)
#define    CH9_USB_REQ_TYPE_VENDOR         (2U << 5U)
#define    CH9_USB_REQ_TYPE_OTHER          (3U << 5U)

/** Recipient of request */
#define    CH9_REQ_RECIPIENT_MASK          0x0fU
#define    CH9_USB_REQ_RECIPIENT_DEVICE    0U
#define    CH9_USB_REQ_RECIPIENT_INTERFACE 1U
#define    CH9_USB_REQ_RECIPIENT_ENDPOINT  2U
#define    CH9_USB_REQ_RECIPIENT_OTHER     3U

/** Standard Feature Selectors (chapter 9.4, Table 9-7 of USB Spec) */
#define    CH9_USB_FS_ENDPOINT_HALT 0U
#define    CH9_USB_FS_FUNCTION_SUSPEND 0U
#define    CH9_USB_FS_DEVICE_REMOTE_WAKEUP 1U
#define    CH9_USB_FS_TEST_MODE 2U
#define    CH9_USB_FS_B_HNP_ENABLE 3U
#define    CH9_USB_FS_A_HNP_SUPPORT 4U
#define    CH9_USB_FS_A_ALT_HNP_SUPPORT 5U
#define    CH9_USB_FS_WUSB_DEVICE 6U
#define    CH9_USB_FS_U1_ENABLE 48U
#define    CH9_USB_FS_U2_ENABLE 49U
#define    CH9_USB_FS_LTM_ENABLE 50U
#define    CH9_USB_FS_B3_NTF_HOST_REL 51U
#define    CH9_USB_FS_B3_RESP_ENABLE 52U
#define    CH9_USB_FS_LDM_ENABLE 53U

/** Definitions for bEndpointAddress field from  Endpoint descriptor */
#define    CH9_USB_EP_DIR_MASK    0x80U
#define    CH9_USB_EP_DIR_IN      0x80U
#define    CH9_USB_EP_NUMBER_MASK 0x0fU

/** Endpoint attributes from Endpoint descriptor - bmAttributes field */
#define    CH9_USB_EP_TRANSFER_MASK 0x03U
#define    CH9_USB_EP_CONTROL       0x0U
#define    CH9_USB_EP_ISOCHRONOUS   0x01U
#define    CH9_USB_EP_BULK          0x02U
#define    CH9_USB_EP_INTERRUPT     0x03U

#define    CH9_USB_DS_CONFIGURATION 9U

/** Max packet 0 size defined in USB Specification */
#define CH9_USB_EP0_MAX_UNKNOWN    ((UINT32)0U) //!< unknow speed - before enumeration
#define CH9_USB_EP0_MAX_LOW        ((UINT32)8U) //!< (1,5Mb/s)
#define CH9_USB_EP0_MAX_FULL       ((UINT32)64U) //!< usb 1.1 (12Mb/s)
#define CH9_USB_EP0_MAX_HIGH       ((UINT32)64U) //!< usb 2.0 (480Mb/s)
#define CH9_USB_EP0_MAX_WIRELESS   ((UINT32)512U) //!< usb 2.5 wireless
#define CH9_USB_EP0_MAX_SUPER      ((UINT32)512U) //!< usb 3.0 GEN 1  (5Gb/s)
#define CH9_USB_EP0_MAX_SUPER_PLUS ((UINT32)512U) //!< usb 3.1 GEN2 (10Gb/s)

/** Structure describes USB request (SETUP packet). See USB Specification (chapter 9.3) */
typedef struct {
    /** Characteristics of request */
    UINT8 bmRequestType;
    /** Specific request */
    UINT8 bRequest;
    /** Field that varies according to request */
    UINT16 wValue;
    /** typically used to pass an index or offset. */
    UINT16 wIndex;
    /** Number of bytes to transfer if there is a data stage */
    UINT16 wLength;
} USB_CH9_SETUP_s ;

#endif

