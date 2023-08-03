/**
 *  @file AmbaPCIE_Drv_Entry.c
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
 *  @details PCIe controller driver.
 */
#include "AmbaPCIE_Drv.h"

PCIE_PrivateData *PCIE_GetPrivateData(void)
{
    static PCIE_PrivateData g_private_data;
    return &g_private_data;
}

PCIE_OBJ *PCIE_GetInstance(void)
{
    static PCIE_OBJ driver = {
        /* AmbaPCIE_Drv_Lm.c */
        .probe   = PCIE_Probe,
        .init    = PCIE_Init,
        .destroy = PCIE_Destroy,

        .isCoreStrappedAsEpOrRp     = PCIE_IsCoreStrappedAsEpOrRp,

        .getVendorIdSubsysVendId    = PCIE_GetVendorIdSubsysVendId,
        .setVendorIdSubsysVendId    = PCIE_SetVendorIdSubsysVendId,

        .getRootPortRequestorId     = PCIE_GetRootPortRequestorId,
        .setRootPortRequestorId     = PCIE_SetRootPortRequestorId,
        .ctrlSnpNoSnpLatReqInLtrMsg = PCIE_CtrlSnpNoSnpLatReqInLtrMsg,
        .sendMessage                = PCIE_SendMessage,
        .enableAutomaticMessage     = PCIE_EnableAutomaticMessage,

        /* AmbaPCIE_Drv_LmCount.c */
        .setSavedCountValues        = PCIE_SetSavedCountValues,
        .getSavedCountValues        = PCIE_GetSavedCountValues,

        /* AmbaPCIE_Drv_LmDbgMux.c */
        .getDebugMux                = PCIE_GetDebugMux,
        .setDebugMux                = PCIE_SetDebugMux,
        .controlDebugParams         = PCIE_ControlDebugParams,

        /* AmbaPCIE_Drv_LmInterrupt.c */
        .start                      = PCIE_Start,
        .stop                       = PCIE_Stop,
        .isr                        = PCIE_Isr,
        .ctrlMaskingOfLocInterrupt  = PCIE_CtrlMaskingOfLocInterrupt,
        .areThereLocalErrors        = PCIE_AreThereLocalErrors,
        .isLocalError               = PCIE_IsLocalError,
        .resetLocalErrStatusCondit  = PCIE_ResetLocalErrStatusCondit,

        /* AmbaPCIE_Drv_LmTiming.c */
        .setTimingParams            = PCIE_SetTimingParams,
        .getTimingParams            = PCIE_GetTimingParams,
        .setL0sTimeout              = PCIE_SetL0sTimeout,
        .getL0sTimeout              = PCIE_GetL0sTimeout,
        .disableRpTransitToL0s      = PCIE_DisableRpTransitToL0s,

        /* AmbaPCIE_Drv_LmCredit.c */
        .setCreditLimitSettings     = PCIE_SetCreditLimitSettings,
        .getCreditLimitSettings     = PCIE_GetCreditLimitSettings,
        .setTransCreditUpdInterval  = PCIE_SetTransCreditUpdInterval,
        .getTransCreditUpdInterval  = PCIE_GetTransCreditUpdInterval,

        /* AmbaPCIE_Drv_LmBar.c */
        .getFuncBarApertureSetting  = PCIE_GetFuncBarApertureSetting,
        .setFuncBarApertureSetting  = PCIE_SetFuncBarApertureSetting,
        .setFuncBarControlSetting   = PCIE_SetFuncBarControlSetting,
        .getFuncBarControlSetting   = PCIE_GetFuncBarControlSetting,
        .setRootPortBarApertSetting = PCIE_SetRootPortBarApertSetting,
        .getRootPortBarApertSetting = PCIE_GetRootPortBarApertSetting,
        .getRootPortBarCtrlSetting  = PCIE_GetRootPortBarCtrlSetting,
        .setRootPortBarCtrlSetting  = PCIE_SetRootPortBarCtrlSetting,
        .getRootPortType1ConfSet    = PCIE_GetRootPortType1ConfSet,
        .setRootPortType1ConfSet    = PCIE_SetRootPortType1ConfSet,
        .controlRootPortBarCheck    = PCIE_ControlRootPortBarCheck,

        /* AmbaPCIE_Drv_LmLink.c */
        .isLinkTrainingComplete     = PCIE_IsLinkTrainingComplete,
        .getLinkTrainingState       = PCIE_GetLinkTrainingState,
        .getLinkTrainingDirection   = PCIE_GetLinkTrainingDirection,
        .getNegotiatedLaneCount     = PCIE_GetNegotiatedLaneCount,
        .getNegotiatedLinkSpeed     = PCIE_GetNegotiatedLinkSpeed,
        .getReceivedLinkId          = PCIE_GetReceivedLinkId,
        .isRemoteLinkwidthUpconfig  = PCIE_IsRemoteLinkwidthUpconfig,
        .getTransmittedLinkId       = PCIE_GetTransmittedLinkId,
        .setTransmittedLinkId       = PCIE_SetTransmittedLinkId,
        .controlRpMasterLoopback    = PCIE_ControlRpMasterLoopback,
        .ctrlReportingOfAllPhyErr   = PCIE_CtrlReportingOfAllPhyErr,
        .controlTxSwing             = PCIE_ControlTxSwing,
        .getNegotiatedLaneBitMap    = PCIE_GetNegotiatedLaneBitMap,
        .areLaneNumbersReversed     = PCIE_AreLaneNumbersReversed,
        .getTargetLanemap           = PCIE_GetTargetLanemap,
        .setTargetLanemap           = PCIE_SetTargetLanemap,
        .retrainLink                = PCIE_RetrainLink,
        .controlLinkDownReset       = PCIE_ControlLinkDownReset,
        .getLinkEquGen3DebugInfo    = PCIE_GetLinkEquGen3DebugInfo,

        /* AmbaPCIE_Drv_AxiConfig.c */
        .clearLinkDownIndicator     = PCIE_ClearLinkDownIndicator,
        .updObWrapperTrafficClass   = PCIE_UpdObWrapperTrafficClass,
        .setupObWrapperMemIoAccess  = PCIE_SetupObWrapperMemIoAccess,
        .setupObWrapperMsgAccess    = PCIE_SetupObWrapperMsgAccess,
        .setupObWrapperConfigAccess = PCIE_SetupObWrapperConfigAccess,
        .setupIbRootPortAddrTransl  = PCIE_SetupIbRootPortAddrTransl,
        .setupIbEPAddrTranslation   = PCIE_SetupIbEPAddrTranslation,
        .setupObWrapperRegions      = PCIE_SetupObWrapperRegions,

        /* AmbaPCIE_Drv_RpMgmt.c */
        .doConfigRead               = PCIE_DoConfigRead,
        .doConfigWrite              = PCIE_DoConfigWrite,
        .doAriConfigRead            = PCIE_DoAriConfigRead,
        .doAriConfigWrite           = PCIE_DoAriConfigWrite,
        .getRootPortBAR             = PCIE_GetRootPortBAR,
        .getRp8GTsLinkEquPreset     = PCIE_GetRp8GTsLinkEquPreset,
        .setRp8GTsLinkEquPreset     = PCIE_SetRp8GTsLinkEquPreset,
        .enableRpMemBarAccess       = PCIE_EnableRpMemBarAccess,

        /* AmbaPCIE_Drv_Udma.c */
        .UDMA_DoTransfer            = PCIE_UDMA_DoTransfer,
        .UDMA_PrepareDescriptor     = PCIE_UDMA_PrepareDescriptor,
        .UDMA_ExtendBulkDescriptor  = PCIE_UDMA_ExtendBulkDescriptor,
        .UDMA_Init                  = PCIE_UDMA_Init,
        .UDMA_RequestLtdAttach      = PCIE_UDMA_RequestLtdAttach,
        .UDMA_RequestLtdExtend      = PCIE_UDMA_RequestLtdExtend,
        .UDMA_ClearState            = PCIE_UDMA_ClearState,
        .UDMA_RequestLtdDetach      = PCIE_UDMA_RequestLtdDetach,
        .UDMA_CheckLtdDetach        = PCIE_UDMA_CheckLtdDetach,

        /* AmbaPCIE_Drv_UdmaInfo.c */
        .UDMA_GetVersion            = PCIE_UDMA_GetVersion,
        .UDMA_GetConfiguration      = PCIE_UDMA_GetConfiguration,
        .UDMA_GetBufferErrorCount   = PCIE_UDMA_GetBufferErrorCount,

        /* AmbaPCIE_Drv_UdmaInterrupt.c */
        .UDMA_Isr                   = PCIE_UDMA_Isr,
        .UDMA_ControlDoneInterrupts = PCIE_UDMA_ControlDoneInterrupts,
        .UDMA_ControlErrInterrupts  = PCIE_UDMA_ControlErrInterrupts,
        .UDMA_GetDoneInterrupt      = PCIE_UDMA_GetDoneInterrupt,
        .UDMA_GetErrorInterrupt     = PCIE_UDMA_GetErrorInterrupt,

        .DmaSysCb = NULL,
    };

    return &driver;
}



