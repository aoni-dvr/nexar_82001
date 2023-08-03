/**
 *  @file ux_hcd_xhci.h
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
 *  @details Ambarella USBX XHCI driver header file.
 */

#ifndef UX_HCD_XHCI_H
#define UX_HCD_XHCI_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

typedef struct {
    XHC_CDN_DRV_RES_s      *cdns_drv_res;
    ULONG                   base_address;
    struct UX_HCD_STRUCT   *parent_hcd;
    ULONG                   flag_running;
} UX_HCD_XHCI;


UINT  ux_hcd_xhci_endpoint_create(XHC_CDN_DRV_RES_s *DrvRes, UX_ENDPOINT *UxEndpoint);
UINT  ux_hcd_xhci_endpoint_destroy(XHC_CDN_DRV_RES_s *DrvRes, UX_ENDPOINT *UxEndpoint);
UINT  ux_hcd_xhci_endpoint_reset(XHC_CDN_DRV_RES_s *DrvRes, UX_ENDPOINT *UxEndpoint);
UINT  ux_hcd_xhci_entry(UX_HCD *hcd, UINT function, void *parameter);
UINT  ux_hcd_xhci_frame_number_get(XHC_CDN_DRV_RES_s *DrvRes, ULONG *FrameNumber);
UINT  ux_hcd_xhci_port_reset(XHC_CDN_DRV_RES_s *DrvRes, UINT PortID);
UINT  ux_hcd_xhci_port_status_get(XHC_CDN_DRV_RES_s *DrvRes, UINT PortID);
UINT  ux_hcd_xhci_request_transfer(UX_HCD_XHCI *HcdXhci, UX_TRANSFER *Transfer);
UINT  ux_hcd_xhci_request_abort(UX_HCD_XHCI *HcdXhci, UX_TRANSFER *UxTransfer);

UX_HCD *ux_hcd_xhci_hcd_find(void);

#endif

