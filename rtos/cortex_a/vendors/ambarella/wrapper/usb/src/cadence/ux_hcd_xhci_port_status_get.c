/**
 *  @file ux_hcd_xhci_port_status_get.c
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
 *  @details Ambarella USBX XHCI driver for getting port status
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

#include <ux_hcd_xhci.h>

static UINT32 xhci_port_status_to_ux_port_status(UINT32 PortID, UINT32 XhciPortStatus)
{
    UINT32 ux_port_status = 0;
    UINT32 port_speed;

    if (PortID < 2U) {
        // USB 2.0 port status & USB 32 port status

        /* Connection Status Change */
        if ((XhciPortStatus & XHCI_PORTSC_CCS_MASK) != 0U) {
            ux_port_status |=  ((UINT32)UX_PS_CCS);

            port_speed = (XhciPortStatus & XHCI_PORTSC_PORTSPEED_MASK) >> XHCI_PORTSC_PORTSPEED_SHIFT;
            if (USBCDNS3_DEV_MODE_32_PORT_IDX == PortID) {
                switch(port_speed){
                    case CH9_USB_SPEED_LOW:        ux_port_status |= ((UINT32)UX_PS_DS_LS); break;
                    case CH9_USB_SPEED_FULL:       ux_port_status |= ((UINT32)UX_PS_DS_FS); break;
                    case CH9_USB_SPEED_HIGH:       ux_port_status |= ((UINT32)UX_PS_DS_HS); break;
                    case CH9_USB_SPEED_SUPER:      ux_port_status |= ((UINT32)UX_PS_DS_SS); break;
                    case CH9_USB_SPEED_SUPER_PLUS: ux_port_status |= ((UINT32)UX_PS_DS_SSP); break;
                    default: /*pass misra check*/ break;
                }
            } else {
                switch(port_speed){
                    case 1:       ux_port_status |= ((UINT32)UX_PS_DS_FS); break;
                    case 3:       ux_port_status |= ((UINT32)UX_PS_DS_HS); break;
                    default: /*pass misra check*/ break;
                }
            }
        }

        /* Port Enabled/Disabled  */
        if ((XhciPortStatus & XHCI_PORTSC_PED_MASK) != 0U) {
            ux_port_status |=  ((UINT32)UX_PS_PES);
        }

        /* Port Over Current Active  */
        if ((XhciPortStatus & XHCI_PORTSC_OCA_MASK) != 0U) {
            ux_port_status |=  ((UINT32)UX_PS_POCI);
        }

        /* Port Reset Status  */
        if ((XhciPortStatus & XHCI_PORTSC_PR_MASK) != 0U) {
            ux_port_status |=  ((UINT32)UX_PS_PRS);
        }

        /* Port Power Status  */
        if ((XhciPortStatus & XHCI_PORTSC_PP_MASK) != 0U) {
            ux_port_status |=  ((UINT32)UX_PS_PPS);
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                                "xhci_port_status_to_ux_port_status(): port: %d, 0x%X -> 0x%X",
                                PortID,
                                XhciPortStatus,
                                ux_port_status, 0, 0);

    }

    return ux_port_status;
}

UINT  ux_hcd_xhci_port_status_get(XHC_CDN_DRV_RES_s *DrvRes, UINT PortID)
{
    UINT xhci_port_status;
    UINT ux_port_status;

    xhci_port_status = USBHCDNS3_PortStatusRead(DrvRes, PortID);
    ux_port_status = xhci_port_status_to_ux_port_status(PortID, xhci_port_status);

    return ux_port_status;
}
