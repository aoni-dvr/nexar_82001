/**
 *  @file AmbaUSBD_Drv_EndpointCreate.c
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
 *  @details USB device driver endpoint create functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to create Endpoint resources.
 * */
UINT32 USBD_DrvEndpointCreate(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *Endpoint)
{
    UINT32 endpoint_index;
    UINT32 logical_address;
    UINT32 uret;
    UINT32 Mask = ~(UINT32)UX_ENDPOINT_DIRECTION;

    logical_address = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
    endpoint_index  = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & Mask;

    uret = USBD_UdcEndpointMapCreate(Udc, logical_address);

    if (uret == USB_ERR_SUCCESS) {
        // Considering the chance endpoint interrupt happens before enumeration, endpoint buffer of Ep-0 is allocated in "udc_control_endpoint_create".
        if (logical_address != 0U) {
            USBD_UdcEndpointBufferAllocate(Endpoint);
        }
        USBD_UdcDmaMemoryAllocate(Udc, Endpoint);
        uret = UX_SUCCESS;
    } else {
        // We only enable interrupt on endpoint 0 at this stage.
        // Create control endpoint only happens when enumeration is done
        // So endpoint interrupt happens only after this stage
        if (endpoint_index == 0U) {
            volatile UINT32 ep_interrupt_state;
            // Enable the interrupt on this endpoint.
            ep_interrupt_state = AmbaRTSL_UsbGetDevIntStatus();
            AmbaRTSL_UsbSetDevIntStatus(ep_interrupt_state);

            // enable control endpoint interrupt only
            AmbaRTSL_UsbEnInEpInt(0);
            AmbaRTSL_UsbDisInEpInt(1);
            AmbaRTSL_UsbDisInEpInt(2);
            AmbaRTSL_UsbDisInEpInt(3);
            AmbaRTSL_UsbDisInEpInt(4);
            AmbaRTSL_UsbDisInEpInt(5);
            AmbaRTSL_UsbEnOutEpInt(0);
            AmbaRTSL_UsbDisOutEpInt(1);
            AmbaRTSL_UsbDisOutEpInt(2);
            AmbaRTSL_UsbDisOutEpInt(3);
            AmbaRTSL_UsbDisOutEpInt(4);
            AmbaRTSL_UsbDisOutEpInt(5);
        }
    }

    return uret;
}
/** @} */
