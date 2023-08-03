/**
 *  @file AmbaUSBD_Drv_EndpointReset.c
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
 *  @details USB device driver endpoint reset functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to reset physical Endpoint.
 * */
UINT32 USBD_DrvEndpointReset(USBD_UDC_s *Udc, const UX_SLAVE_ENDPOINT *Endpoint)
{
    UDC_ENDPOINT_INFO_s *udc_ed;

    AmbaMisra_TouchUnused(Udc);

    // Get the physical endpoint address in the endpoint container.
    udc_ed = USB_UtilityVoidP2UdcEd(Endpoint->ux_slave_endpoint_ed);

    // Get the index of the endpoint and build the endpoint mask.
    // if ep dir = IN, flush TX FIFO, make sure not stalled
    // if ep dir = OUT, flush RX FIFO, make sure not stalled

    // Set the state of the endpoint to not stalled.
    if ((udc_ed->EpState & USBD_UDC_ED_STATUS_STALLED) != 0U) {
        udc_ed->EpState &= ~USBD_UDC_ED_STATUS_STALLED;
    }

    // This function never fails.
    return (UX_SUCCESS);
}
/** @} */
