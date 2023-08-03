/**
 *  @file AmbaUSBD_Drv_EndpointStall.c
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
 *  @details USB device driver endpoint stall functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static void rx_fifo_flush(void)
{
    INT32 retry_count = 10000;
    while (AmbaRTSL_UsbGetDevStatusRfEmpty() == 0U) {
        AmbaRTSL_UsbSetDevCtlSrxflush(1);
        retry_count--;
        if (retry_count < 0) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "[USB] USBD_DrvEndpointStall: failed");
            break;
        }
    }
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to stall physical Endpoint.
 * */
void USBD_DrvEndpointStall(const UX_SLAVE_ENDPOINT *Endpoint)
{
    UDC_ENDPOINT_INFO_s *udc_ed;

    // disable USB interrupt would make sure no unexpected event happens
    // during put Device's endpoint into stall status

    USBD_IntDisable();

    // Get the physical endpoint address in the endpoint container.
    udc_ed = USB_UtilityVoidP2UdcEd(Endpoint->ux_slave_endpoint_ed);

    // Set the endpoint to stall.
    udc_ed->EpState |= USBD_UDC_ED_STATUS_STALLED;

#ifdef AMBA_USB_DEBUG
    dbg("%s(): EP[%d] Dir = %d Stall", __func__, udc_ed->PhysicalEpIndex, udc_ed->UxDcdA9EpDir);
#endif

    if ((udc_ed->PhysicalEpIndex != 0U) && (udc_ed->Direction == USBD_EP_DIR_IN)) {
        AmbaRTSL_UsbSetEpInStall(udc_ed->PhysicalEpIndex, 1);
        AmbaRTSL_UsbSetEpInCtrlF(udc_ed->PhysicalEpIndex, 1);
    } else if ((udc_ed->PhysicalEpIndex != 0U) && (udc_ed->Direction == USBD_EP_DIR_OUT)) {
        rx_fifo_flush();
        AmbaRTSL_UsbSetEpOutStall(udc_ed->PhysicalEpIndex, 1);
    } else {
        // Clean up Rx fifo content, and then set stall.
        // Or, Rx DMA won't be enabled.
        rx_fifo_flush();

        // Can we stall control EP?
        AmbaRTSL_UsbSetEpInStall(0, 1);

        // Enable interrupt to make sure we can obtain next setup packet interrupt.
        // Especially for 3-stage control request but stall at status phase.
        AmbaRTSL_UsbEnOutEpInt(0);
    }

    if (AmbaRTSL_UsbGetDevCtlRde() == 0U) {
        // If Rx DMA is disabled, enabled it to fetch next setup packet.
        USBD_UdcInitCtrlSetupDesc();
        AmbaRTSL_UsbSetEpOutRxReady(0, 1);
        AmbaRTSL_UsbSetDevCtlRde(1);
    } else {
        // For USBCV "Halt Endpoint Test", device should not init setup descriptor here (Rx DMA enabled).
        // Or, The following "Set Configuration Test" may be failed.
        // It's a timing issue, and next setup packet content may be contaminated.
#ifdef AMBA_USB_DEBUG
        dbg("%s(): Rx DMA is already enabled", __func__);
#endif
    }

    USBD_IntEnable();
}
/** @} */
