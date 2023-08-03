/**
 *  @file AmbaUSB_Buffer.h
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
 *  @details USB buffer management header file.
 */

#ifndef AMBAUSB_BUFFER_H
#define AMBAUSB_BUFFER_H

// memory usage type
enum {
    UBUF_UDC_CTRL_IN = 0U,
    UBUF_UDC_CTRL_OUT,
    UBUF_UDC_SW_FIFO,
    UBUF_USBX_CTRL,
    UBUF_USBX_BULK_IN,
    UBUF_USBX_BULK_OUT,
    UBUF_UDC_BULK_DESC_IN,
    UBUF_UDC_BULK_DESC_OUT,
    UBUF_UDC_ALT,
    UBUF_UDC_ISO_DESC,
    UBUF_MSCH_ALIGN, //!< Align Buffer for MSCH read/write
    UBUF_NUMBER
};

// memory size
#define UBUF_SIZE_UDC_CTRL_IN             128U
#define UBUF_SIZE_UDC_CTRL_OUT            128U
#define UBUF_SIZE_UDC_SW_FIFO             1024U
#define UBUF_SIZE_UDC_ALT                 512U    // Only for 1 bulk transaction: 512 byte
#define UBUF_SIZE_USBX_CTRL               1024U
#define UBUF_SIZE_MSCH_ALIGN              524288U //!< Align Buffer Size for MSCH read/write
#define UBUF_SIZE_USBX_BULK_IN            65536U  //!< Buffer size for USBX bulk-in transfer
#define UBUF_SIZE_USBX_BULK_OUT           32768U  //!< Buffer size for USBX bulk-out transfer
#define UBUF_SIZE_UDC_ISO_DESC            128U
#define UBUF_NUM_USBX_BULK_IN             5U      //!< Number of supported bulk-in/interrupt-in endpoints
#define UBUF_NUM_USBX_BULK_OUT            5U      //!< Number of supported bulk-out/interrupt-out endpoints
#define UBUF_NUM_UDC_DESC_IN              128U    //!< Number of DMA descriptor number for USBX bulk-in transfer: UBUF_SIZE_USBX_BULK_IN/512
#define UBUF_NUM_UDC_DESC_OUT             64U     //!< Number of DMA descriptor number for USBX bulk-out transfer: UBUF_SIZE_USBX_BULK_OUT/512
#define UBUF_NUM_UDC_ISO_DESC             USBD_UDC_ISO_WRAPPER_NUM

UINT32 USB_BufferSizeGet(UINT32 Type);
UINT32 USB_BufferU8Attach(UINT32 Type, UINT8 **Ptr);
void   USB_BufferU8Detach(UINT32 Type, const UINT8 *Ptr);
UINT32 USB_BufferDataDescAttach(UINT32 Type, UDC_DATA_DESC_s **Ptr);
void   USB_BufferDataDescDetach(UINT32 Type, const UDC_DATA_DESC_s *Ptr);
#if defined(USBD_SUPPORT_ISO)
UINT32 USB_BufferIsoDataDescAttach(UDC_ISO_DESC_INFO_s *pDescWrapperAarray, UINT32 DescNum);
#endif
#endif

