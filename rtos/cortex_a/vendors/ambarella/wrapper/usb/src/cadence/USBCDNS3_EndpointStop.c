/**
 *  @file USBCDNS3_EndpointStop.c
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
 *  @details USB driver for Cadence USB device/host controller.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>

/**
 * Enqueues the Stop endpoint command in the command queue
 *
 * @param[in] res driver resources
 * @param[in] endpoint index of endpoint to stop
 */
static void enqueueStopEndpointCmd(XHC_CDN_DRV_RES_s *DrvRes, UINT32 endpoint) {
    UINT32 dword3_value = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
    dword3_value |= (endpoint << XHCI_TRB_ENDPOINT_POS);
    dword3_value |= ((UINT32)XHCI_TRB_STOP_EP_CMD << XHCI_TRB_TYPE_POS);
    dword3_value |= (DrvRes->CommandQueue.ToogleBit & 0x01U);

    DrvRes->CommandQueue.EnqueuePtr->DWord3 = dword3_value;

    USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.STOP_EP.");
}

/**
 * Stop endpoint. Function sends STOP_ENDPOINT_COMMAND command to SSP controller
 *
 * @param[in] res driver resources
 * @param[in] endpoint index of endpoint to stop
 * @return CDN_EINVAL when driver's settings doesn't suit to native platform settings
 * @return CDN_EOK if no errors
 */
UINT32 USBCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EndpointIdx)
{

    // check input parameters
    UINT32 uret = USBSSP_StopEndpointSF(DrvRes, EndpointIdx);

    // return CDN_EINVAL if parameters are not correct
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "USBSSP_StopEndpoint(): <%d> Critical error! Wrong value in one of function parameters",
                              DrvRes->instanceNo, 0, 0, 0, 0);
    }

    if (uret == 0U) {
        // enqueue stop endpoint command
        enqueueStopEndpointCmd(DrvRes, EndpointIdx);
        USBCDNS3_HostCommandDoorbell(DrvRes);
    }

    return uret;
}


