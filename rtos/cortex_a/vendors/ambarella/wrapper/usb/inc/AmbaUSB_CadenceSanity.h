/**
 *  @file AmbaUSB_CadenceSanity.h
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
 *  @details Cadence IP USB header file.
 */

#ifndef AMBAUSB_CADENCE_SANITY_H
#define AMBAUSB_CADENCE_SANITY_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

//UINT32 USBSSP_DriverConfigTSF(const USBSSP_DriverConfigT *obj);
//UINT32 USBSSP_DriverContextTSF(const USBSSP_DriverContextT *obj);
UINT32 USBSSP_DriverResourcesTSF(const XHC_CDN_DRV_RES_s *DrvRes);
UINT32 USBSSP_RingElementTSF(const XHCI_RING_ELEMENT_s *obj);
UINT32 USBSSP_XhciResourcesTSF(const XHC_CDN_MEMORY_RES_s *obj);

UINT32 USBSSP_SanityFunction1(const XHC_CDN_DRV_RES_s *DrvRes, const UINT32 EpIndex);
UINT32 USBSSP_SanityFunction3(const XHC_CDN_DRV_RES_s* DrvRes, const UINT32 EpIndex);
UINT32 USBSSP_SanityFunction5(const XHC_CDN_DRV_RES_s* DrvRes);
UINT32 USBSSP_SanityFunction7(const XHC_CDN_DRV_RES_s* res, const XHC_CDN_MEMORY_RES_s* memRes);
//UINT32 USBSSP_SanityFunction8(const XHC_CDN_DRV_RES_s* res, const USBSSP_DriverConfigT* config);
UINT32 USBSSP_SanityFunction10(const XHC_CDN_DRV_RES_s* res, const XHCI_RING_ELEMENT_s *eventPtr);
UINT32 USBSSP_SanityFunction15(const XHC_CDN_DRV_RES_s *DrvRes, const UINT8 * epCfgBuffer);
UINT32 USBSSP_SanityFunction16(const XHC_CDN_DRV_RES_s* DrvRes, const USB_CH9_SETUP_s* Setup);
UINT32 USBSSP_SanityFunction26(const XHC_CDN_DRV_RES_s* res, const UINT32 *index);
//UINT32 USBSSP_SanityFunction27(const XHC_CDN_DRV_RES_s* res, const USBSSP_ExtraFlagsEnumT flags);
//UINT32 USBSSP_SanityFunction33(const XHC_CDN_DRV_RES_s* res, const USBSSP_PortControlRegIdx portRegIdx);
//UINT32 USBSSP_SanityFunction34(const XHC_CDN_DRV_RES_s* res, const USBSSP_PortControlRegIdx portRegIdx, const UINT32 *regValue);
UINT32 USBSSP_SanityFunction35(const XHC_CDN_DRV_RES_s* res, const UINT32 *ddusbTxRegs, const UINT32 *ddusbRxRegs);
//UINT32 USBSSP_SanityFunction37(const XHC_CDN_DRV_RES_s* res, const USBSSP_DriverContextT* drvContext);


#define    USBSSP_TransferDataSF USBSSP_SanityFunction1
#define    USBSSP_TransferData2SF USBSSP_SanityFunction1
#define    USBSSP_StopEndpointSF USBSSP_SanityFunction3
#define    USBSSP_ResetEndpointSF USBSSP_SanityFunction3
#define    USBSSP_ResetDeviceSF USBSSP_SanityFunction5
#define    USBSSP_IsrSF USBSSP_SanityFunction5
#define    USBSSP_SetMemResSF USBSSP_SanityFunction7
#define    USBSSP_InitSF USBSSP_SanityFunction8
#define    USBSSP_GetDescriptorSF USBSSP_SanityFunction5
#define    USBSSP_ForceEventSF USBSSP_SanityFunction10
#define    USBSSP_SetAddressSF USBSSP_SanityFunction5
#define    USBSSP_ResetRootHubPortSF USBSSP_SanityFunction5
#define    USBSSP_IssueGenericCommandSF USBSSP_SanityFunction5
#define    USBSSP_EndpointSetFeatureSF USBSSP_SanityFunction1
#define    USBSSP_SetConfigurationSF USBSSP_SanityFunction15
#define    USBSSP_ControlTransferSF USBSSP_SanityFunction16
#define    USBSSP_NBControlTransferSF USBSSP_SanityFunction16
#define    USBSSP_ControlTransferDevSF USBSSP_SanityFunction5
#define    USBSSP_NoOpTestSF USBSSP_SanityFunction5
#define    USBSSP_EnableSlotSF USBSSP_SanityFunction5
#define    USBSSP_DisableSlotSF USBSSP_SanityFunction5
#define    USBSSP_EnableEndpointSF USBSSP_SanityFunction15
#define    USBSSP_DisableEndpointSF USBSSP_SanityFunction5
#define    USBSSP_GetEpStateSF USBSSP_SanityFunction5
#define    USBSSP_GetMicroFrameIndexSF USBSSP_SanityFunction26
#define    USBSSP_SetEndpointExtraFlagSF USBSSP_SanityFunction27
#define    USBSSP_CleanEndpointExtraFlaSF USBSSP_SanityFunction27
#define    USBSSP_GetEndpointExtraFlagSF USBSSP_SanityFunction15
#define    USBSSP_SetFrameIDSF USBSSP_SanityFunction5
#define    USBSSP_AddEventDataTRBSF USBSSP_SanityFunction5
#define    USBSSP_ForceHeaderSF USBSSP_SanityFunction5
#define    USBSSP_SetPortControlRegSF USBSSP_SanityFunction33
#define    USBSSP_GetPortControlRegSF USBSSP_SanityFunction34
#define    USBSSP_EnableDDUSBSF USBSSP_SanityFunction35
#define    USBSSP_DisableDDUSBSF USBSSP_SanityFunction5
#define    USBSSP_SaveStateSF USBSSP_SanityFunction37
#define    USBSSP_RestoreStateSF USBSSP_SanityFunction37


#endif

