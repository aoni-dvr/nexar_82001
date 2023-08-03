/**
 *  @file AmbaUSB_Buffer.c
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
 *  @details USB buffer management.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Buffer.h>

#define ALIGN_CACHE_NO_INIT __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")))

static UINT8 ux_buf_bulk_in[UBUF_NUM_USBX_BULK_IN][UBUF_SIZE_USBX_BULK_IN] ALIGN_CACHE_NO_INIT;
static UINT8 ux_buf_bulk_out[UBUF_NUM_USBX_BULK_OUT][UBUF_SIZE_USBX_BULK_OUT] ALIGN_CACHE_NO_INIT;

static UINT32 ux_flag_bulk_in[UBUF_NUM_USBX_BULK_IN];
static UINT32 ux_flag_bulk_out[UBUF_NUM_USBX_BULK_OUT];

static UDC_DATA_DESC_s udc_buf_data_desc_in[UBUF_NUM_USBX_BULK_IN][UBUF_NUM_UDC_DESC_IN] ALIGN_CACHE_NO_INIT;
static UDC_DATA_DESC_s udc_data_desc_out[UBUF_NUM_USBX_BULK_OUT][UBUF_NUM_UDC_DESC_OUT] ALIGN_CACHE_NO_INIT;

static UINT32 udc_flag_data_desc_in[UBUF_NUM_USBX_BULK_IN];
static UINT32 udc_flag_data_desc_out[UBUF_NUM_USBX_BULK_OUT];

static UINT8  udc_buf_swfifo[UBUF_NUM_USBX_BULK_OUT][UBUF_SIZE_UDC_SW_FIFO] ALIGN_CACHE_NO_INIT;
static UINT32 udc_flag_swfifo[UBUF_NUM_USBX_BULK_OUT];

static UINT8  udc_buf_alt[UBUF_NUM_USBX_BULK_OUT][UBUF_SIZE_UDC_ALT] ALIGN_CACHE_NO_INIT;
static UINT32 udc_flag_alt[UBUF_NUM_USBX_BULK_OUT];

static UINT8* attach_bulk_in_buf(void)
{
    UINT32 i;
    UINT8 *ptr_ret = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_IN; i++) {
        if (ux_flag_bulk_in[i] == 0U) {
            ptr_ret            = ux_buf_bulk_in[i];
            ux_flag_bulk_in[i] = 1;
            break;
        }
    }

    return ptr_ret;
}

static void detach_bulk_in_buf(const UINT8 *Buffer)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_IN; i++) {
        if (ux_buf_bulk_in[i] == Buffer) {
            ux_flag_bulk_in[i] = 0;
            break;
        }
    }
}

static UINT8* attach_bulk_out_buf(void)
{
    UINT32 i;
    UINT8 *ptr = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (ux_flag_bulk_out[i] == 0U) {
            ptr                 = ux_buf_bulk_out[i];
            ux_flag_bulk_out[i] = 1;
            break;
        }
    }

    return ptr;
}

static void detach_bulk_out_buf(const UINT8 *Buffer)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (ux_buf_bulk_out[i] == Buffer) {
            ux_flag_bulk_out[i] = 0;
            break;
        }
    }
}

static UDC_DATA_DESC_s *attach_bulk_in_desc(void)
{
    UINT32 i;
    UDC_DATA_DESC_s *ptr_ret = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_IN; i++) {
        if (udc_flag_data_desc_in[i] == 0U) {
            ptr_ret                  = udc_buf_data_desc_in[i];
            udc_flag_data_desc_in[i] = 1;
            break;
        }
    }

    return ptr_ret;
}

static void detach_bulk_in_desc(const UDC_DATA_DESC_s *Desc)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_IN; i++) {
        if (udc_buf_data_desc_in[i] == Desc) {
            udc_flag_data_desc_in[i] = 0;
            break;
        }
    }
}

static UDC_DATA_DESC_s *attach_bulk_out_desc(void)
{
    UINT32 i;
    UDC_DATA_DESC_s *ptr_ret = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_flag_data_desc_out[i] == 0U) {
            ptr_ret                   = udc_data_desc_out[i];
            udc_flag_data_desc_out[i] = 1;
            break;
        }
    }

    return ptr_ret;
}

static void detach_bulk_out_desc(const UDC_DATA_DESC_s *Desc)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_data_desc_out[i] == Desc) {
            udc_flag_data_desc_out[i] = 0;
            break;
        }
    }
}


static UINT8* attach_udc_sw_fifo(void)
{
    UINT32 i;
    UINT8 *ptr_ret = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_flag_swfifo[i] == 0U) {
            ptr_ret            = udc_buf_swfifo[i];
            udc_flag_swfifo[i] = 1;
            break;
        }
    }

    return ptr_ret;
}

static void detach_udc_sw_fifo(const UINT8 *Buf)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_buf_swfifo[i] == Buf) {
            udc_flag_swfifo[i] = 0;
            break;
        }
    }
}

static UINT8* attach_udc_alt_buf(void)
{
    UINT32 i;
    UINT8 *ptr_ret = NULL;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_flag_alt[i] == 0U) {
            ptr_ret         = udc_buf_alt[i];
            udc_flag_alt[i] = 1;
            break;
        }
    }

    return ptr_ret;
}

static void detach_udc_alt_buf(const UINT8 *Buf)
{
    UINT32 i;

    for (i = 0; i < UBUF_NUM_USBX_BULK_OUT; i++) {
        if (udc_buf_alt[i] == Buf) {
            udc_flag_alt[i] = 0;
            break;
        }
    }
}

UINT32 USB_BufferSizeGet(UINT32 Type)
{
    UINT32 size_ret;

    switch (Type) {
    case UBUF_UDC_CTRL_IN:
        size_ret = UBUF_SIZE_UDC_CTRL_IN;
        break;
    case UBUF_UDC_CTRL_OUT:
        size_ret = UBUF_SIZE_UDC_CTRL_OUT;
        break;
    case UBUF_USBX_CTRL:
        size_ret = UBUF_SIZE_USBX_CTRL;
        break;
    case UBUF_USBX_BULK_IN:
        size_ret = UBUF_SIZE_USBX_BULK_IN;
        break;
    case UBUF_USBX_BULK_OUT:
        size_ret = UBUF_SIZE_USBX_BULK_OUT;
        break;
    case UBUF_UDC_SW_FIFO:
        size_ret = UBUF_SIZE_UDC_SW_FIFO;
        break;
    case UBUF_UDC_ALT:
        size_ret = UBUF_SIZE_UDC_ALT;
        break;
    case UBUF_MSCH_ALIGN:
        size_ret = UBUF_SIZE_MSCH_ALIGN;
        break;
    default:
        size_ret = 0;
        break;
    }
    return size_ret;
}

UINT32 USB_BufferU8Attach(UINT32 Type, UINT8 **Ptr)
{
    static UINT8 udc_buf_ctrl_in[UBUF_SIZE_UDC_CTRL_IN] ALIGN_CACHE_NO_INIT;
    static UINT8 udc_buf_ctrl_out[UBUF_SIZE_UDC_CTRL_OUT] ALIGN_CACHE_NO_INIT;
    static UINT8 ux_buf_ctrl[UBUF_SIZE_USBX_CTRL] ALIGN_CACHE_NO_INIT;
    static UINT8 udc_buf_msch_align[UBUF_SIZE_MSCH_ALIGN] ALIGN_CACHE_NO_INIT;
    UINT32 uret = USB_ERR_SUCCESS;

    switch (Type) {
    case UBUF_UDC_CTRL_IN:
        AmbaMisra_TouchUnused(udc_buf_ctrl_in);
        *Ptr = udc_buf_ctrl_in;
        break;
    case UBUF_UDC_CTRL_OUT:
        AmbaMisra_TouchUnused(udc_buf_ctrl_out);
        *Ptr = udc_buf_ctrl_out;
        break;
    case UBUF_USBX_CTRL:
        AmbaMisra_TouchUnused(ux_buf_ctrl);
        *Ptr = ux_buf_ctrl;
        break;
    case UBUF_USBX_BULK_IN:
        *Ptr = attach_bulk_in_buf();
        break;
    case UBUF_USBX_BULK_OUT:
        *Ptr = attach_bulk_out_buf();
        break;
    case UBUF_UDC_SW_FIFO:
        *Ptr = attach_udc_sw_fifo();
        break;
    case UBUF_UDC_ALT:
        *Ptr = attach_udc_alt_buf();
        break;
    case UBUF_MSCH_ALIGN:
        AmbaMisra_TouchUnused(udc_buf_msch_align);
        *Ptr = udc_buf_msch_align;
        break;
    default:
        *Ptr = NULL;
        uret = USB_ERR_PARAMETER_INVALID;
        break;
    }

    return uret;
}

void USB_BufferU8Detach(UINT32 Type, const UINT8 *Ptr)
{
    switch (Type) {
    case UBUF_USBX_BULK_IN:
        detach_bulk_in_buf(Ptr);
        break;
    case UBUF_USBX_BULK_OUT:
        detach_bulk_out_buf(Ptr);
        break;
    case UBUF_UDC_SW_FIFO:
        detach_udc_sw_fifo(Ptr);
        break;
    case UBUF_UDC_ALT:
        detach_udc_alt_buf(Ptr);
        break;
    default:
        // Do nothing here.
        break;
    }
}

/*! \brief Obtain DMA descriptor buffer
 */
UINT32 USB_BufferDataDescAttach(UINT32 Type, UDC_DATA_DESC_s **Ptr)
{
    UINT32 uret = USB_ERR_SUCCESS;

    switch (Type) {
    case UBUF_UDC_BULK_DESC_IN:
        *Ptr = attach_bulk_in_desc();
        break;
    case UBUF_UDC_BULK_DESC_OUT:
        *Ptr = attach_bulk_out_desc();
        break;
    default:
        *Ptr = NULL;
        uret = USB_ERR_PARAMETER_INVALID;
        break;
    }

    return uret;
}

void USB_BufferDataDescDetach(UINT32 Type, const UDC_DATA_DESC_s *Ptr)
{
    switch (Type) {
    case UBUF_UDC_BULK_DESC_IN:
        detach_bulk_in_desc(Ptr);
        break;
    case UBUF_UDC_BULK_DESC_OUT:
        detach_bulk_out_desc(Ptr);
        break;
    default:
        // Do nothing here.
        break;
    }
}

#if defined(USBD_SUPPORT_ISO)

UINT32 USB_BufferIsoDataDescAttach(UDC_ISO_DESC_INFO_s *pDescWrapperAarray, UINT32 DescNum)
{
    static UDC_DATA_DESC_s udc_data_desc_iso[UBUF_NUM_UDC_ISO_DESC][UBUF_SIZE_UDC_ISO_DESC] ALIGN_CACHE_NO_INIT;
    UINT32 i;
    UINT32 MinDescNum;

    if (DescNum > UBUF_NUM_UDC_ISO_DESC) {
        MinDescNum = UBUF_NUM_UDC_ISO_DESC;
    } else {
        MinDescNum = DescNum;
    }

    for (i = 0; i < MinDescNum; i++) {
        pDescWrapperAarray[i].DataDesc = udc_data_desc_iso[i];
    }

    return USB_ERR_SUCCESS;
}

#endif

