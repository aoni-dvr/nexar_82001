/**
 *  @file AmbaUSBD_Drv_InitComplete.c
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
 *  @details USB kernel driver to complete hw initialization.
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
 * Called by USB driver to complete initialization
 * */
UINT32 USB_DrvInitComplete(void)
{
    const UX_SLAVE_DCD *ux_dcd;
    UX_SLAVE_DEVICE *ux_evice;
    UINT8 *device_framework;
    UX_SLAVE_TRANSFER *transfer_request;
    UINT32 uret;
    const UX_SLAVE_ENDPOINT *source_ptr;
    const void *destination_ptr;

    // Get the pointer to the DCD.
    ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

    // Get the pointer to the device.
    ux_evice = &_ux_system_slave->ux_system_slave_device;

    // The device is operating at full/high speed, default is high speed
    // reply descriptor to host according to speed enumeration result


    if (_ux_system_slave->ux_system_slave_speed == (UINT32)UX_HIGH_SPEED_DEVICE) { // 0 == high speed
        _ux_system_slave->device_framework        = _ux_system_slave->device_framework_high_speed;
        _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_high_speed;
    } else {                                                                       // 1 == full speed
        _ux_system_slave->device_framework        = _ux_system_slave->device_framework_full_speed;
        _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_full_speed;
    }

    // Get the device framework pointer.
    device_framework = _ux_system_slave->device_framework;

    // And create the decompressed device descriptor structure.
    _ux_utility_descriptor_parse(device_framework,
                                 _ux_system_device_descriptor_structure,
                                 UX_DEVICE_DESCRIPTOR_ENTRIES,
                                 (UINT8 *) &ux_evice->ux_slave_device_descriptor);

    // Now we create a transfer request to accept the first SETUP packet
    // and get the ball running. First get the address of the endpoint
    // transfer request container.
    transfer_request = &ux_evice->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    // Set the timeout to be for Control Endpoint.
    transfer_request->ux_slave_transfer_request_timeout = UX_CONTROL_TRANSFER_TIMEOUT;

    // Adjust the current data pointer as well.
    transfer_request->ux_slave_transfer_request_current_data_pointer =
        transfer_request->ux_slave_transfer_request_data_pointer;

    // Update the transfer request endpoint pointer with the default endpoint.
    transfer_request->ux_slave_transfer_request_endpoint = &ux_evice->ux_slave_device_control_endpoint;

    // The control endpoint max packet size needs to be filled manually in its descriptor.
    transfer_request->ux_slave_transfer_request_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize =
        ux_evice->ux_slave_device_descriptor.bMaxPacketSize0;

    // On the control endpoint, always expect the maximum.
    transfer_request->ux_slave_transfer_request_requested_length =
        ux_evice->ux_slave_device_descriptor.bMaxPacketSize0;

    // Attach the control endpoint to the transfer request.
    transfer_request->ux_slave_transfer_request_endpoint = &ux_evice->ux_slave_device_control_endpoint;

    // Create the default control endpoint attached to the device.
    // Once this endpoint is enabled, the host can then send a setup packet
    // The device controller will receive it and will call the setup function
    // module.
    source_ptr = &ux_evice->ux_slave_device_control_endpoint;
    USB_UtilityMemoryCopy(&destination_ptr, &source_ptr, sizeof(void*));
    uret = ux_dcd->ux_slave_dcd_function(ux_dcd, UX_DCD_CREATE_ENDPOINT, destination_ptr);

    if (uret == 0U) {
        // Ensure the control endpoint is properly reset.
        ux_evice->ux_slave_device_control_endpoint.ux_slave_endpoint_state = UX_ENDPOINT_RESET;

        // A SETUP packet is a DATA IN operation.
        transfer_request->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;

        // Check the status change callback.
        if (_ux_system_slave->ux_system_slave_change_function != UX_NULL) {
            // Inform the application if a callback function was programmed.
            // (todo) shall we handle the return value?
            (void)_ux_system_slave->ux_system_slave_change_function(UX_DEVICE_ATTACHED);
        }
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    // We are now ready for the USB device to accept the first packet when connected.
    return uret;
}
/** @} */
