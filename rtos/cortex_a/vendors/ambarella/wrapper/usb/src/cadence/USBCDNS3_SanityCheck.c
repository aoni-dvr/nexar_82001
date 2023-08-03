/**
 *  @file USBCDNS3_SanityCheck.c
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
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>

/**
 * Function to validate struct DriverResourcesT
 *
 * @param[in] obj pointer to struct to be verified
 * @returns 0 for valid
 * @returns CDN_EINVAL for invalid
 */
UINT32 USBSSP_DriverResourcesTSF(const XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 uret = 0;

    if (DrvRes == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else {
        if (
            (DrvRes->ActualSpeed != CH9_USB_SPEED_UNKNOWN) &&
            (DrvRes->ActualSpeed != CH9_USB_SPEED_LOW) &&
            (DrvRes->ActualSpeed != CH9_USB_SPEED_FULL) &&
            (DrvRes->ActualSpeed != CH9_USB_SPEED_HIGH) &&
            (DrvRes->ActualSpeed != CH9_USB_SPEED_SUPER) &&
            (DrvRes->ActualSpeed != CH9_USB_SPEED_SUPER_PLUS)
        )
        {
            uret = USB_ERR_XHCI_SPEED_INVALID;
        }
        if (
            (DrvRes->Ep0State != USBCDNS3_EP0_UNCONNECTED) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_HALT_PENDING) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_HALT_SETUP_PENDING) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_HALTED) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_SETUP_PENDING) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_SETUP_PHASE) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_DATA_PHASE) &&
            (DrvRes->Ep0State != USBCDNS3_EP0_STATUS_PHASE)
        )
        {
            uret = USB_ERR_XHCI_EP0_STATE_INVALID;
        }
    }

    return uret;
}


/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] res Driver resources
 * @param[in] epIndex index of endpoint according to xhci spec e.g for ep1out
 *    epIndex=2, for ep1in epIndex=3, for ep2out epIndex=4 end so on
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
UINT32 USBSSP_SanityFunction1(const XHC_CDN_DRV_RES_s *DrvRes, const UINT32 EpIndex)
{
    /* Declaring return variable */
    UINT32 uret;

    uret = USBSSP_DriverResourcesTSF(DrvRes);
    if (uret == 0U) {
        if ((EpIndex < (1U)) || (EpIndex > (XHCI_EP_CONTEXT_MAX))) {
            uret = USB_ERR_XHCI_EP_IDX_INVALID;
        }
    }

    return uret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] DrvRes Driver resources
 * @param[in] endpoint Index of endpoint to stop
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
UINT32 USBSSP_SanityFunction3(const XHC_CDN_DRV_RES_s *DrvRes, const UINT32 EpIndex)
{
    /* Declaring return variable */
    UINT32 uret = USBSSP_DriverResourcesTSF(DrvRes);

    if (uret == 0U) {
        if ((EpIndex < (XHCI_EP0_CONTEXT_OFFSET)) || (EpIndex > (XHCI_EP_CONTEXT_MAX))) {
            uret = USB_ERR_XHCI_EP_IDX_INVALID;
        }
    }

    return uret;
}


/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] DrvRes Driver resources
 * @return 0 success
 * @return Others invalid parameters
 */
UINT32 USBSSP_SanityFunction5(const XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 uret = USBSSP_DriverResourcesTSF(DrvRes);
    return uret;
}


/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] DrvRes Driver resources
 * @param[in] epCfgBuffer Configuration buffer address
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
UINT32 USBSSP_SanityFunction15(const XHC_CDN_DRV_RES_s *DrvRes, const UINT8 * epCfgBuffer)
{
    /* Declaring return variable */
    UINT32 uret = 0;

    if (epCfgBuffer == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    }

    if (uret == 0U) {
        uret = USBSSP_DriverResourcesTSF(DrvRes);
    }

    return uret;
}

static UINT32 USB_CH9_SETUP_sSF(const USB_CH9_SETUP_s *obj)
{
    UINT32 uret = 0;

    if (obj == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    }
    return uret;
}

/**
 * A common function to check the validity of API functions with
 * following parameter types
 * @param[in] res Driver resources
 * @param[in] setup Keeps setup packet
 * @return 0 success
 * @return CDN_EINVAL invalid parameters
 */
UINT32 USBSSP_SanityFunction16(const XHC_CDN_DRV_RES_s *DrvRes, const USB_CH9_SETUP_s* Setup)
{
    /* Declaring return variable */
    UINT32 uret;

    uret = USBSSP_DriverResourcesTSF(DrvRes);
    if (uret == 0U) {
        uret = USB_CH9_SETUP_sSF(Setup);
    }
    return uret;
}


