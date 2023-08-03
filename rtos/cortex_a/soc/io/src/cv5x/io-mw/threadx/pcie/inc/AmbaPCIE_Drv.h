/**
 *  @file AmbaPCIE_Drv.h
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
 *  @details PCIe core driver API header file.
 */
#ifndef AMBAPCIE_DRV_H
#define AMBAPCIE_DRV_H

#include "AmbaTypes.h"
#include "AmbaPCIE_Reg.h"
#include "AmbaPCIE_Struct.h"
#include "AmbaPCIE_Utility.h"
#include "AmbaPCIE_ErrCode.h"

#define FLAG_PCIE_API_UNINIT      0x0000U
#define FLAG_PCIE_API_RC_INIT     0x0001U
#define FLAG_PCIE_API_RC_START    0x0003U
#define FLAG_PCIE_API_RC_READY    0x0007U
#define FLAG_PCIE_API_RC_MASK     0x00FFU
#define FLAG_PCIE_API_EP_INIT     0x0100U
#define FLAG_PCIE_API_EP_START    0x0300U
#define FLAG_PCIE_API_EP_READY    0x0700U

#define FLAG_PCIE_API_INIT_MASK   0x0101U
#define FLAG_PCIE_API_READY_MASK  0x0707U

UINT32 Impl_ApiFlagGet(void);
void Impl_ApiFlagSet(UINT32 flag);

void Impl_PcieInit(UINT32 mode, UINT32 gen, UINT32 lane, UINT32 use24mhz);
void Impl_LinkStart(UINT32 mode, UINT32 gen);
UINT32 Impl_RcInit(UINT64 rp_base, UINT64 axi_base);
UINT32 Impl_RcGetEpBarInfo(void);
UINT32 Impl_EpInit (UINT64 ep_base);
UINT32 Impl_GetConfigurationInfo(const PCIE_PrivateData *pD);

/**
 *  @}
 */

/** @defgroup DriverFunctionAPI Driver Function API
 *  Prototypes for the driver API functions. The user application can link statically to the
 *  necessary API functions and call them directly.
 *  @{
 */

/**********************************************************************
* API methods
**********************************************************************/

/**
 * Obtain the private memory size required by the driver
 * @param[in] cfg The base address of the PCIe core
 * @param[out] sysReq The resource requirement for driver operation
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_Probe(UINT64 cfg, PCIE_SysReq* sysReq);

/**
 * Initialize the PCIe driver. Must be called before all other access
 * APIs. The function takes in the initalization parameter structure
 * and over-ride the hardware initial value when the struture
 * indicated so. Otherwise, the init function will retain the default
 * value in the hardware. *
 * @param[out] pD Memory pointer to the uninitialized private data of the size specified by probe.
 * @param[in] cfg The base address of the PCIe core
 * @param[in] pInitParam Configuration parameters for driver/hardware initialization.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if illegal/inconsistent values in config
 * @return CDN_EIO if driver encountered an error accessing hardware
 */
UINT32 PCIE_Init(PCIE_PrivateData* pD, UINT64 cfg, const PCIE_InitParam* pInitParam);

/**
 * Destructor for the driver. (NOP) *
 */
void PCIE_Destroy(void);

/**
 * Unmask the specified IRQ source.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] maskableIntrParam IRQ to be unmasked
 */
UINT32 PCIE_Start(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

/**
 * Mask the specified IRQ source.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] maskableIntrParam IRQ to be masked
 */
UINT32 PCIE_Stop(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

/**
 * interrupt service routine/polling routine
 * @param[in] pD Pointer to the private data initialized by init
 */
UINT32 PCIE_Isr(const PCIE_PrivateData* pD);

/**
 * This function checks to see if link training is complete by
 * checking for the current state of link. Function does not check for
 * the validity of the base address of the PCIe core passed into the
 * function as a parameter.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pTrainingComplete Indicates if training is complete
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_IsLinkTrainingComplete(const PCIE_PrivateData* pD, PCIE_Bool* pTrainingComplete);

/**
 * This function gets the current state that the LTSSM is in. Provides
 * current state of LTSSM during while link training is in progress.
 * DETECT_QUIET state will be returned when reset is applied, and the
 * L0 state will be returned when link training is completed.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pLtssmState The state of the LTSSM
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetLinkTrainingState(const PCIE_PrivateData* pD, PCIE_LtssmState* pLtssmState);

/**
 * This function obtains link training direction. When crosslink is
 * enabled, the value returned by this function indicates whether the
 * core completed link training as an upstream port or a downstream
 * port.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pUpOrDown Upstream or downstream port
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetLinkTrainingDirection(const PCIE_PrivateData* pD, PCIE_UpstreamOrDownstream* pUpOrDown);

/**
 * This function indicates if the core is strapped an EP or an RP for
 * a dual-mode core.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pEpOrRp RP or EP
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_IsCoreStrappedAsEpOrRp(const PCIE_PrivateData* pD, PCIE_CoreEpOrRp* pEpOrRp);

/**
 * This function gets the Negotiated Lane Count. I.e., the Lane count
 * negotiated with other side during link training The correct value
 * is returned by this function when to the negotiated link width when
 * link training reaches the L0 state. To check if the link is in L0
 * state, the function "GetLinkTrainingState" may be called. Calling
 * this function when Link Training has not happened will return a
 * default of X8.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] nlc The Negotiated Lane Count
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetNegotiatedLaneCount(const PCIE_PrivateData* pD, PCIE_LaneCount* nlc);

/**
 * This function gets the Negotiated speed of the link. This function
 * will get the re-written value of link speed when the negotiated
 * link width or link speed changes. This function gets the negotiated
 * link width when link training reaches the L0 state. To check if the
 * link is in L0 state, the function "GetLinkTrainingState" may be
 * called. This function returns the updated current operating speed
 * of the link when the link state is L0.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] ns The negotiated Link Speed
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetNegotiatedLinkSpeed(const PCIE_PrivateData* pD, PCIE_LinkSpeed* ns);

/**
 * This function returns the Negotiated Lane Map which contains a map
 * of the active lanes used by the core to form the link during link
 * training. The returned value is a bitmap with the appropriate bit-
 * position set to 1 at the end of link training if the Lane indicated
 * by the bit-position is part of the PCIe link. The value of this
 * field is valid only when the link is in L0 or L0S states. To check
 * if the link is in L0 or L0S states, the function
 * "GetLinkTrainingState" may be called. This function gets the
 * negotiated lane map when link training reaches the L0 state.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pLaneMap The negotiated lane map
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetNegotiatedLaneBitMap(const PCIE_PrivateData* pD, UINT32* pLaneMap);

/**
 * This function returns Lane Reversal Status set by the core at the
 * end of link training if the LTSSM had to reverse the lane numbers
 * to form the link. Returns the lane reversal status when link
 * training reaches the L0 state. The function tells the caller if the
 * core reversed the lane number on its lanes during link training.
 * The value of this field is valid only when the link is in L0 state.
 * To check if the link is in L0 state, the function
 * "GetLinkTrainingState" may be called. This function returns gets
 * the reversal status when link training reaches the L0 state.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pReversed Whether reversal happened
 * @return CDN_EOK function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_AreLaneNumbersReversed(const PCIE_PrivateData* pD, PCIE_Bool* pReversed);

/**
 * Link id received from other side during link training.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pLinkId The Link ID returned
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetReceivedLinkId(const PCIE_PrivateData* pD, UINT32* pLinkId);

/**
 * This function indicates whether the remote node advertised
 * Linkwidth Upconfigure Capability in the training sequences in the
 * Configuration.Complete state when the link came up.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in,out] upconfig Whether the link is upconfigurable or not
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_IsRemoteLinkwidthUpconfig(const PCIE_PrivateData* pD, PCIE_Bool* upconfig);

/**
 * This function is used to read the current value of the Transmitted
 * Link id. This is the Link id transmitted by the device in training
 * sequences in the RP mode.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pLinkId The link ID which is read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetTransmittedLinkId(const PCIE_PrivateData* pD, UINT8* pLinkId);

/**
 * This function is used to set the Transmitted Link id. This is the
 * Link id transmitted by the device in training sequences in the RP
 * mode.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] pLinkId The link ID to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetTransmittedLinkId(const PCIE_PrivateData* pD, UINT8 pLinkId);

/**
 * This function allows access to read the Vendor ID and Subsystem
 * Vendor ID that the device advertises during its enumeration of the
 * PCI configuration space.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] id Specify whether to access Vendor or Subsystem Id
 * @param[out] idValue The value to be read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetVendorIdSubsysVendId(const PCIE_PrivateData* pD, PCIE_AdvertisedIdType id, UINT16* idValue);

/**
 * This function allows access to write the Vendor ID and Subsystem
 * Vendor ID that the device advertises during its enumeration of the
 * PCI configuration space.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] id Specify whether to access Vendor or Subsystem Id
 * @param[in] idValue The value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetVendorIdSubsysVendId(const PCIE_PrivateData* pD, PCIE_AdvertisedIdType id, UINT16 idValue);

/**
 * This function writes timing based parameters
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] timingVal Specify timeout and delay parameter
 * @param[in] timeDelay The value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetTimingParams(const PCIE_PrivateData* pD, PCIE_TimeoutAndDelayValues timingVal, UINT32 timeDelay);

/**
 * This function reads timing based parameters
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] timingVal Specify timeout and delay parameter
 * @param[out] timeDelay The value to be read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetTimingParams(const PCIE_PrivateData* pD, PCIE_TimeoutAndDelayValues timingVal, UINT32* timeDelay);

/**
 * This function allows writing the timeout value for transitioning to
 * the L0S power state. If the transmit side has been idle for this
 * interval, the core will transmit the idle sequence on the link and
 * transition the state of the link to L0S.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] timeout The value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetL0sTimeout(const PCIE_PrivateData* pD, UINT32 *timeout);

/**
 * This function allows reading the timeout value for transitioning to
 * the L0S power state. If the transmit side has been idle for this
 * interval, the core will transmit the idle sequence on the link and
 * transition the state of the link to L0S.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] timeout The value to be read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetL0sTimeout(const PCIE_PrivateData* pD, UINT32* timeout);

/**
 * This API is disables the LTSSM from transitioning to the L0S state
 * @param[in] pD Pointer to the private data initialized by init
 */
UINT32 PCIE_DisableRpTransitToL0s(const PCIE_PrivateData* pD);

/**
 * This function gets the current value of the Target Lane-Map which
 * defines the bitmap of the lanes to be included in forming the link
 * during re-training.
 * @param[in] pD Pointer to the private data intialized by init
 * @param[out] lanemap Value that is read
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetTargetLanemap(const PCIE_PrivateData* pD, UINT32* lanemap);

/**
 * This function sets the Target Lane-Map which defines the bitmap of
 * the lanes to be included in forming the link during  re-training.
 * Legal settings are: -0x0f (x4 link) -0x3, 0xc (x2 link) -0x1, 0x2,
 * 0x4, 0x8 (x1 link) Retraining the link with any other value in this
 * field can cause the training to fail.  If the target lane map
 * includes lanes that were inactive when retraining is initiated,
 * then both the core and its link partner must support the LinkWidth
 * Upconfigure Capability to be able to activate those lanes. The user
 * can check if the remote node has this capability by reading the
 * Remote Link Upconfigure Capability Status bit in Physical Layer
 * Configuration Register 0 after the link first came up.
 * @param[in] pD Pointer to the private data intialized by init
 * @param[in] lanemap Value that needs to be programmed
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetTargetLanemap(const PCIE_PrivateData* pD, UINT32 lanemap);

/**
 * * When the core is configured as a Root Port, this function can be
 * used to retrain the link to a different width, without bringing the
 * link down. This function is not to be used in the EndPoint mode.
 * Calling this function to retrain the link results in the core re-
 * training the link to change its width. When calling this function,
 * the software must also set the target lane-map field to indicate
 * the lanes it desires to be part of the link. The core will attempt
 * to form a link with this set of lanes. The link formed at the end
 * of the retraining may include all of these lanes (if both nodes
 * agree on them during re-training), or the largest subset that both
 * sides were able to activate. The command to retrain the link is
 * cleared by the internal logic of the core after the re-training has
 * been completed and link has reached the Configuration Complete
 * state. Software must wait for the bit to be clear before setting it
 * again to change the link width. *
 * @param[in] pD The base address of the PCIe core
 * @param[in] waitForCompletion Indicates whether the function should wait until it is safe to invoke retraining again
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_RetrainLink(const PCIE_PrivateData* pD, PCIE_WaitOrNot waitForCompletion);

/**
 * This function allows access to settings that contain the initial
 * credit limits advertised by the core during the DL initialization.
 * If these settings are modified, the link must be re-trained to re-
 * initialize the DL for the modified settings to take effect.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] creditLimitParam The Credit Limit parameter that is to be read or written
 * @param[in] transmitOrReceive Is the paramter on the transmit or the receive side
 * @param[in] vcNum The virtual channel number.
 * @param[in] limit The value being written or read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetCreditLimitSettings(const PCIE_PrivateData* pD, PCIE_CreditLimitTypes creditLimitParam, PCIE_TransmitOrReceive transmitOrReceive, UINT8 vcNum, UINT32 limit);

/**
 * This function allows access to settings that contain the initial
 * credit limits advertised by the core during the DL initialization.
 * If these settings are modified, the link must be re-trained to re-
 * initialize the DL for the modified settings to take effect.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] creditLimitParam The Credit Limit parameter that is to be read or written
 * @param[in] transmitOrReceive Is the paramter on the transmit or the receive side
 * @param[in] vcNum The virtual channel number.
 * @param[out] limit The value being written or read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetCreditLimitSettings(const PCIE_PrivateData* pD, PCIE_CreditLimitTypes creditLimitParam, PCIE_TransmitOrReceive transmitOrReceive, UINT8 vcNum, UINT32* limit);

/**
 * This function controls parameters that control how frequently the
 * core sends a credit update to the opposite node.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] creditUpdateIntervalParam Which credit interval param are we accessing
 * @param[in] interval The value being written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetTransCreditUpdInterval(const PCIE_PrivateData* pD, PCIE_CreditUpdateIntervals creditUpdateIntervalParam, UINT32 *interval);

/**
 * This function controls parameters that control how frequently the
 * core sends a credit update to the opposite node.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] creditUpdateIntervalParam Which credit interval param are we accessing
 * @param[out] interval The value being read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetTransCreditUpdInterval(const PCIE_PrivateData* pD, PCIE_CreditUpdateIntervals creditUpdateIntervalParam, UINT32* interval);

/**
 * Settings for Physical BAR Aperture settings
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] funcNumber The PCI function number
 * @param[in] barNumber The BAR number
 * @param[out] apertureSize The aperture-size value to be read.  The miniumum value is 4K.
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetFuncBarApertureSetting(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarApertureSize* apertureSize);

/**
 * Settings for Physical BAR Aperture settings
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] funcNumber The PCI function number
 * @param[in] barNumber The BAR number
 * @param[in] apertureSize The aperture-size value to be written.  The miniumum value is 4K.
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetFuncBarApertureSetting(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarApertureSize apertureSize);

/**
 * Settings for Physical BAR Control Settings
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] funcNumber The PCI function number
 * @param[in] barNumber The BAR number
 * @param[in] barControl The Control value to be written
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetFuncBarControlSetting(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarControl barControl);

/**
 * Settings for Physical BAR Control Settings
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] funcNumber The PCI function number
 * @param[in] barNumber The BAR number
 * @param[out] barControl The Control value to be read
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetFuncBarControlSetting(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarControl* barControl);

/**
 * The root port side of the core contains three 64-bit memory BARs
 * that can be used for address-range checking of incoming requests
 * from devices connected to it. This function determines the
 * configuration of these BARs.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] barNumber The BAR number to be accessed
 * @param[in] apertureSize The aperture size value to be written. The miniumum value is 4K.
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetRootPortBarApertSetting(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarApertureSize apertureSize);

/**
 * The root port side of the core contains three 64-bit memory BARs
 * that can be used for address-range checking of incoming requests
 * from devices connected to it. This function determines the
 * configuration of these BARs.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] barNumber The BAR number to be accessed
 * @param[out] apertureSize The aperture size value to be read. The miniumum value is 4K.
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetRootPortBarApertSetting(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarApertureSize* apertureSize);

/**
 * This function allows for reading the configuration of RP BARs.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] barNumber The RP BAR number to access
 * @param[out] rpBarControl The control value to be read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetRootPortBarCtrlSetting(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarControl* rpBarControl);

/**
 * This function allows for writing the configuration of RP BARs.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] barNumber The RP BAR number to access
 * @param[in] rpBarControl The control value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetRootPortBarCtrlSetting(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarControl rpBarControl);

/**
 * This function allows for accessing or modifying the configuration
 * for type 1 config accesses
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] rpType1Config The control value to be read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetRootPortType1ConfSet(const PCIE_PrivateData* pD, PCIE_RpType1ConfigControl* rpType1Config);

/**
 * This function allows for accessing or modifying the configuration
 * for type 1 config accesses
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] rpType1Config The control value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetRootPortType1ConfSet(const PCIE_PrivateData* pD, PCIE_RpType1ConfigControl rpType1Config);

/**
 * This function is used to enable BAR checking in the RP mode.  When
 * disabled, the core will forward all incoming memory requests to the
 * client logic without checking their address ranges.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] enableOrDisable Enable or disable RP BAR Address range checking
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_ControlRootPortBarCheck(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function is used to Enable or Disable the RP to cause the
 * LTSSM to initiate a loopback and become a loopback master. This
 * function controls Loopback functionality in RP mode only. When the
 * core is operating as a Root Port, enabling Loopback causes the
 * LTSSM to initiate a loopback and become the loopback master. This
 * function is not used in the EndPoint Mode.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] enableOrDisable Enable or disable loopback functionality
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_ControlRpMasterLoopback(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function allows read the Debug Mux. This function selects the
 * group of signals within the module that are driven on the debug
 * bus.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] muxSelect The Module Select value to be read
 * @return CDN_EOK if the function comepleted successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetDebugMux(const PCIE_PrivateData* pD, PCIE_DebugMuxModuleSelect* muxSelect);

/**
 * This function allows write the Debug Mux. This function selects the
 * group of signals within the module that are driven on the debug
 * bus.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] muxSelect The Module Select value to be written
 * @return CDN_EOK if the function comepleted successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetDebugMux(const PCIE_PrivateData* pD, PCIE_DebugMuxModuleSelect muxSelect);

/**
 * Debug parameters that may be enabled or disabled
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] debugParam The debug parameter that needs to be controlled
 * @param[in] enableOrDisable Enable or Disable the debug parameter
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_ControlDebugParams(const PCIE_PrivateData* pD, PCIE_DebugParams debugParam, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function allows access to saved count values.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] countParam The count parameter that needs to be accessed
 * @param[in] resetAfterRd This parameter is not used, but should be set to either PCIE_TRUE or PCIE_FALSE.
 *    It is kept for compatiblity with the getSavedCountValues API.
 * @param[in] pCountVal the count value to be written
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetSavedCountValues(const PCIE_PrivateData* pD, PCIE_SavedCountParams countParam, PCIE_Bool resetAfterRd, UINT32 *pCountVal);

/**
 * This function allows access to saved count values.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] countParam The count parameter that needs to be accessed
 * @param[in] resetAfterRd If set to PCIE_TRUE, the count will be reset after the read where allowed by the hardware.  (Note
 *    that the count will not be reset for Received/Transmitted FTS counts.)
 * @param[out] pCountVal the count value read
 * @return CDN_EOK if the function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetSavedCountValues(const PCIE_PrivateData* pD, PCIE_SavedCountParams countParam, PCIE_Bool resetAfterRd, UINT32* pCountVal);

/**
 * This function controls if "All Phy Error Reporting" is to be
 * enabled When enabled, the core will report all phy errors detected
 * at the PIPE interface, regardless of whether it caused packet
 * corruption. When disabled, the core will report only those errors
 * that caused a TLP or DLLP to be dropped because of the phy error.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] enableOrDisable Enable or disable reporting of all phy errors
 * @return CDN_EOK if function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_CtrlReportingOfAllPhyErr(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function controls if TX_SWING on the PIPE interface is driven
 * or not.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] enableOrDisable Enable or disable reporting of all phy errors
 * @return CDN_EOK if function completed successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_ControlTxSwing(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function is used to access the status of the various error and
 * abnormal conditions in the core. Any of the status bits can be
 * reset by writing a 1 into the bit position. This register does not
 * capture any errors signaled by remote devices using PCIe error
 * messages when the core is operating in the RP mode. Unless masked
 * by the setting of the Local Interrupt Mask Register, the occurrence
 * of any of these conditions causes the core to activate the
 * LOCAL_INTERRUPT output.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] maskableIntrParam The parameter that needs to be controlled
 * @param[in] maskOrUnmask Whether the parameter needs to be masked or unmasked
 * @return CDN_EOK if function comepleted successfully
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_CtrlMaskingOfLocInterrupt(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam, PCIE_MaskOrUnmask maskOrUnmask);

/**
 * This function is used to access the status of the various error and
 * abnormal conditions in the core. Any of the status bits can be
 * reset by writing a 1 into the bit position. This function does not
 * report any errors signaled by remote devices using PCIe error
 * messages when the core is operating in the RP mode. Unless masked
 * by the setting of the Local Interrupt Mask Register, the occurrence
 * of any of these conditions causes the core to activate the
 * LOCAL_INTERRUPT output.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pLocalErrorsPresent True if there are local errors, false otherwise
 * @param[out] pErrorType The type of the error if found
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_AreThereLocalErrors(const PCIE_PrivateData* pD, PCIE_Bool* pLocalErrorsPresent, UINT32* pErrorType);

/**
 * This function is used to control the reporting of status of the
 * various error and abnormal conditions in the core. Any of the
 * status bits can be reset by writing a 1 into the bit position. This
 * function does not report any errors signaled by remote devices
 * using PCIe error messages when the core is operating in the RP
 * mode. Unless masked by the setting of the Local Interrupt Mask
 * Register, the occurrence of any of these conditions causes the core
 * to activate the LOCAL_INTERRUPT output.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] errorCondition Is there an error for this condition
 * @param[in,out] pLocalError True if an error exists on the condition, false otherwise
 * @return CDN_EOK on training complete
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_IsLocalError(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions errorCondition, PCIE_Bool* pLocalError);

/**
 * This function is used to reset any Local Error conditions.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] maskableIntrParam Enum for maskable interrupt corresponding to the error to be reset
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_ResetLocalErrStatusCondit(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

/**
 * This function allows read access to the Requestor ID used by the
 * core in internally generated TLPs. This function works only when
 * the core is configured as a Root Port.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] requestorId Pointer to variable that is read
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_GetRootPortRequestorId(const PCIE_PrivateData* pD, UINT16* requestorId);

/**
 * This function allows write access to the Requestor ID used by the
 * core in internally generated TLPs. This function works only when
 * the core is configured as a Root Port.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] requestorId Pointer to variable that is written
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SetRootPortRequestorId(const PCIE_PrivateData* pD, UINT16 requestorId);

/**
 * This function controls whether the Snoop or No Snoop Latency
 * requirement bit is set or not set in the LTR message to be sent.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] snoopOrNoSnoop Whether we are controlling the Snoop or the No Snoop Requirement
 * @param[in] enableOrDisable Enable or disable the Snoop or No Snoop Requirement
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_CtrlSnpNoSnpLatReqInLtrMsg(const PCIE_PrivateData* pD, PCIE_SnoopOrNoSnoop snoopOrNoSnoop, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function sends a message which may be a LTR message, an OBFF
 * message or a Set Slot Power Limit Message.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] msgType Type of message to be sent
 * @param[in] waitFlag This parameter controls whether the function should wait for completion or not before returning
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_SendMessage(const PCIE_PrivateData* pD, PCIE_MessageType msgType, PCIE_WaitOrNoWaitForComplete waitFlag);

/**
 * This function enables the core to automatically transmit an LTR
 * message in response to to Function Power State changes.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] msgType This function currently works only for the LTR message type
 * @return CDN_EOK on success
 * @return CDN_EINVAL if the hardware was not found at the given location
 */
UINT32 PCIE_EnableAutomaticMessage(const PCIE_PrivateData* pD, PCIE_AutoMessageType msgType);

/**
 * This function is used to clear the AXI link down indicator bit
 * which is set after a link down
 * @param[in] pD Pointer to the private data initialized by init
 * @return CDN_EOK on success
 * @return CDN_EINVAL if any pointer parameters are NULL
 */
UINT32 PCIE_ClearLinkDownIndicator(const PCIE_PrivateData* pD);

/**
 * This function will set or clear the AUTO_EN bit in the link down
 * reset register.  If set,  the link down indicator bit will be
 * cleared after a timeout when a secondary bus reset has  been used
 * via the link control register, re-enabling the AXI interface.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] enableOrDisable Enable or disable Link Down Timer
 * @return CDN_EOK on success
 * @return CDN_EINVAL if core is strapped as Endpoint
 */
UINT32 PCIE_ControlLinkDownReset(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function is used to update the traffic class for a region by
 * altering  outbound configuration descriptor 0. (User may want to
 * call setupObWrapperMemIoAccess API before calling this API.)
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] trafficClass Traffic class number to which the region has to be assigned.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if trafficClass is out of bounds
 */
UINT32 PCIE_UpdObWrapperTrafficClass(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 trafficClass);

/**
 * This function is used to set the outbound configuration register
 * allocation when used for Memory and IO TLPs.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
 * @param[in] address Fixed address to apply for PCIe transactions Actual number of bits passed will depend on numPassedBits.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if numPassedBits is out of bounds
 */
UINT32 PCIE_SetupObWrapperMemIoAccess(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT64 address);

/**
 * This function is used to set the outbound configuration register
 * allocation when used for Message TLPs
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] address 64 bits of vendor defined message.  Should be zero for normal message.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if region number is out of bounds
 */
UINT32 PCIE_SetupObWrapperMsgAccess(const PCIE_PrivateData* pD, UINT8 regionNo, UINT64 address);

/**
 * This function is used to set the outbound configuration register
 * allocation to generate Type 0 Configuration Transaction Layer
 * Packets (TLPs), to allow access to PCIe configuration  space on the
 * link partner.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
 * @param[in] busDevFuncAddr Bus-device function address to be used for Requester ID in outgoing TLPs
 * @return CDN_EOK on success
 * @return CDN_EINVAL if not configured as Root Port
 */
UINT32 PCIE_SetupObWrapperConfigAccess(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT32 busDevFuncAddr);

/**
 * This function is used to set up inbound Root Port Address
 * Translation
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] numPassedBits Number of bits to pass through to AXI2HAL  One will be added to this value, i.e. numPassedBits+1 will be actually passed.
 * @param[in] address Fixed bits of address to apply for SoC transaction, lower 8 bits ignored
 * @return CDN_EOK on success
 * @return CDN_EINVAL if region number is out of bounds
 */
UINT32 PCIE_SetupIbRootPortAddrTransl(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT64 address);

/**
 * This function is used to set up inbound EP Address Translation
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] functionNo Specifies which function of the bar is to be configured.  For Virtual Functions, the numbering starts after the last PF,
 *    i.e. VF0 is considered as function number 1
 * @param[in] barNo Specifies which BAR is to be configured
 * @param[in] address Fixed bits of address to apply for SoC transaction, lower 8 bits ignored.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if region number is out of bounds
 */
UINT32 PCIE_SetupIbEPAddrTranslation(const PCIE_PrivateData* pD, UINT32 functionNo, PCIE_BarNumber barNo, UINT64 address);

/**
 * This function is used to set the size and location of each outbound
 * AXI region
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] regionNo Specifies which region is to be configured
 * @param[in] numPassedBits size of this region, specified as nunber of valid bits in address, e.g. size 256 is 7 bits.
 * @param[in] upperAddr Upper 32bits of offset to this AXI region
 * @param[in] lowerAddr Lower 32bits of address to this AXI region
 * @return CDN_EOK on success
 * @return CDN_EINVAL if regionSize is out of bounds
 */
UINT32 PCIE_SetupObWrapperRegions(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT32 upperAddr, UINT32 lowerAddr);

/**
 * This function is used to read a value from the ECAM address space.
 * AXI region 0  must be initialized for configuration access before
 * calling this function.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bdf Bus number, device number and function number packed into BDF
 * @param[in] offset Register offset
 * @param[in] size Size for the transaction, 1, 2 or 4 byte
 * @param[out] data Value read at the specified location
 * @return CDN_EOK on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_DoConfigRead(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32* data);

/**
 * This function is used to write a value in the ECAM address space.
 * AXI region 0  must be initialized for configuration access before
 * calling this function.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bdf Bus number, device number and function number packed into BDF
 * @param[in] offset Register offset
 * @param[in] size Size for the transaction, 1, 2 or 4 byte
 * @param[in] data The data to be written
 * @return CDN_EOK on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_DoConfigWrite(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32 data);

/**
 * This function is used to read a value from the ECAM address space
 * for ARI supported devices. AXI region 0 must be initialized for
 * configuration access before calling this function.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bdf Bus number and function number packed into BF
 * @param[in] offset Register offset
 * @param[in] size Size for the transaction, 1, 2 or 4 byte
 * @param[out] data Value read at the specified location
 * @return CDN_EOK on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_DoAriConfigRead(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32* data);

/**
 * This function is used to write a value in the ECAM address space
 * for ARI supported devices. AXI region 0  must be initialized for
 * configuration access before calling this function.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bdf Bus number, device number and function number packed into BDF
 * @param[in] offset Register offset
 * @param[in] size Size for the transaction, 1, 2 or 4 byte
 * @param[in] data The data to be written
 * @return CDN_EOK on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_DoAriConfigWrite(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32 data);

/**
 * Read the aperture size of the Root Port memory BAR0 or BAR1.  This
 * function is provided as a  convenience to assist in test
 * preparation.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bar Bar number (only 2 bars are available on RP.)
 * @param[out] barVal Aperture size (in bytes) read from the RP bar register.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_GetRootPortBAR(const PCIE_PrivateData* pD, UINT32 bar, UINT32* barVal);

/**
 * This function could be used to read the 8.0GT/s preset value to be
 * used for the local transmitter/receiver during Link equalization
 * procedure.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] txRxSide Receive or Transmit Side.
 * @param[in] upDownStrm Upstream or Downstream
 * @param[in] reqLane Lane number
 * @param[out] presetValPtr Pointer to preset value.
 * @return 0 on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_GetRp8GTsLinkEquPreset(const PCIE_PrivateData* pD, PCIE_TransmitOrReceiveSide txRxSide, PCIE_UpstreamOrDownstream upDownStrm, UINT32 reqLane, UINT32* presetValPtr);

/**
 * This function could be used to write the 8.0GT/s preset value to be
 * used for the local transmitter/receiver during Link equalization
 * procedure.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] txRxSide Receive or Transmit Side.
 * @param[in] upDownStrm Upstream or Downstream
 * @param[in] reqLane Lane number
 * @param[in] presetVal Pointer to preset value.
 * @return 0 on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_SetRp8GTsLinkEquPreset(const PCIE_PrivateData* pD, PCIE_TransmitOrReceiveSide txRxSide, PCIE_UpstreamOrDownstream upDownStrm, UINT32 reqLane, UINT32 presetVal);

/**
 * This function could be used to read the Gen3 Debug register .
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] reqLane Lane number
 * @param[out] lEquaInfo Pointer to debug Info structure
 * @return 0 on success
 * @return CDN_EINVAL if parameters are out of bounds
 */
UINT32 PCIE_GetLinkEquGen3DebugInfo(const PCIE_PrivateData* pD, UINT32 reqLane, PCIE_LinkEquaDebugInfo* lEquaInfo);

/**
 * Enable memory BAR0 or BAR1 on the RP using local management
 * registers. This function  is provided for convenience, and will
 * program a 32bit pr 64bit base address into the specified BAR,
 * providing that the BAR is already configured with a suitable type
 * and size.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] bar Bar number (only 2 bars are available on RP.)
 * @param[in] addr_hi Upper address (if bar is configured as 64bit)
 * @param[in] addr_lo Lower address
 * @return CDN_EOK on success
 * @return CDN_EINVAL the BAR configuration is not suitable
 */
UINT32 PCIE_EnableRpMemBarAccess(const PCIE_PrivateData* pD, PCIE_RpBarNumber bar, UINT32 addr_hi, UINT32 addr_lo);

/**
 * This function is used to read version information for the uDma core
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] maj_ver Major version number of UDMA core
 * @param[out] min_ver Minor version number of UDMA core
 * @return CDN_EOK on success
 * @return CDN_EINVAL if any pointer parameters are NULL
 */
UINT32 PCIE_UDMA_GetVersion(const PCIE_PrivateData* pD, UINT8* maj_ver, UINT8* min_ver);

/**
 * This function is used to read capability information for the uDma
 * core
 * @param[in] pD Pointer to the private data initialized by init
 * @param[out] pConf Structure to contain DMA configuration info
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_GetConfiguration(const PCIE_PrivateData* pD, PCIE_UdmaConfiguration* pConf);

/**
 * This function is used to enable or disable interrupts on successful
 * completion of a UDMA transfer.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] channel Channel number
 * @param[in] enableOrDisable Enable or disable interrupts for this channel
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_ControlDoneInterrupts(const PCIE_PrivateData* pD, UINT32 channel, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function is used to enable or disable interrupts on generation
 * of an error condition from the UDMA core
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] channel Channel number
 * @param[in] enableOrDisable Enable or disable interrupts for this channel
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_ControlErrInterrupts(const PCIE_PrivateData* pD, UINT32 channel, PCIE_EnableOrDisable enableOrDisable);

/**
 * This function checks status of the UDMA Done interrupt,and resets
 * it if it has been asserted
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] channel Channel number
 * @param[out] pIntActive pointer to a PCIE_Bool variable which will be set to TRUE if Done interrupt has been asserted
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or pIntActive is NULL
 */
UINT32 PCIE_UDMA_GetDoneInterrupt(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool* pIntActive);

/**
 * This function checks status of the UDMA Error interrupt,and resets
 * it if it has been asserted
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] channel Channel number
 * @param[out] pIntActive pointer to a PCIE_Bool variable which will be set to TRUE if Error interrupt has been asserted
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or pIntActive is NULL
 */
UINT32 PCIE_UDMA_GetErrorInterrupt(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool* pIntActive);

/**
 * This function starts a UDMA transfer, using a descriptor provided
 * by the application.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] channel Channel number
 * @param[in] isOutbound TRUE if this is an outbound transfer, FALSE for an inbound transfer
 * @param[in] pListAddr Pointer to the start of the descriptor
 * @param[in] pListAttr Structure of attribute data used by the UDMA channel when fetching and returning the link list descriptors
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or pListAddr is NULL
 */
UINT32 PCIE_UDMA_DoTransfer(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool isOutbound, const UINT32* pListAddr, const PCIE_ListAttribute* pListAttr);

/**
 * This function prepares a UDMA transfer descriptor, simply copying
 * it from one area of memory to another that is accessible from the
 * uDMA core.  This step can be skipped if the descriptor is already
 * in a suitable area of memory.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] pDesc Pointer to the descriptor
 * @param[in,out] pDescAddr Pointer to the area of memory to copy the descriptor into
 * @param[in] count Number of descriptors to copy
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_PrepareDescriptor(const PCIE_PrivateData* pD, const UINT32* pDesc, UINT32* pDescAddr, UINT32 count);

/**
 * This function extends an existing UDMA descriptor, adding a pointer
 * to the next descriptor and setting the continue_on bit.  After
 * calling this function, a new DMA transfer should be started, using
 * the address of the new descriptor.  This allows a new DMA transfer
 * to be started without waiting for a previous transfer to complete.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] pDesc Pointer to the new descriptor
 * @param[in,out] pDescAddr Pointer to the descriptor previously programmed to HW
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_ExtendBulkDescriptor(const PCIE_PrivateData* pD, const UINT32* pDesc, const UINT32* pDescAddr);

/**
 * This function reads the count for IB or OB buffer errors of the
 * specified type.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] errorType Error type for which count is to be retrieved
 * @param[out] pErrorCount Pointer to integer to be written with error count
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or pErrorCount is NULL
 */
UINT32 PCIE_UDMA_GetBufferErrorCount(const PCIE_PrivateData* pD, PCIE_UdmaBufferErrorType errorType, UINT32* pErrorCount);

/**
 * This function is used to initialize the UDMA driver. It should be
 * called once only, before calling UDMA_RequestLtdAttach or UDMA_Isr
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] pDescMem Pointer to an area of memory which is to be used by the the driver for descriptors.  The memory must be in addressable
 *    from the UDMA core, and  size should be at least sizeof(td_cfg) Caching must be disabled for this area of memory
 * @param[in] phys_base Physical address of the area of memory which is to be used by the driver for descriptors. This base address is used for
 *    address conversions from physical to virtual and vice versa.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or pDescMem is NULL
 */
UINT32 PCIE_UDMA_Init(PCIE_PrivateData* pD, PCIE_td_cfg* pDescMem, UINT32 phys_base);

/**
 * This function is called to request the driver to initiate a UDMA
 * transfer with the LTD passed as a parameter.  The application
 * should not subsequently modify the LTD until it has been detached
 * from the driver, but may read status fields. This function is part
 * of the higher-level DMA interface, and can be bypassed when
 * creating transfer descriptors and calling UDMA_doTransfer()
 * directly.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] p_ltd Pointer to the LTD which is to be attached
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or p_ltd is NULL, or if the contents of the LTD pointed to by p_ltd are invalid
 */
UINT32 PCIE_UDMA_RequestLtdAttach(const PCIE_PrivateData* pD, PCIE_td_desc* p_ltd);

/**
 * This function is called to inform the driver that the attached LTD
 * has been extended This can be called while the LTD is being
 * processed, or after completion, but must be before the LTD has been
 * detached from the driver. This function is part of the higher-level
 * DMA interface, and can be bypassed when  creating transfer
 * descriptors and calling UDMA_DoTransfer() directly.
 * @param[in] pD Pointer to the private data initialized by init
 * @param[in] p_ltd Pointer to the attached LTD.  This must point to the LTD that is already attached and has been extended.
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD or p_ltd is NULL, or if the contents of the LTD pointed to by p_ltd are invalid
 */
UINT32 PCIE_UDMA_RequestLtdExtend(const PCIE_PrivateData* pD, PCIE_td_desc* p_ltd);

/**
 * This function is called to clear all DMA descriptor memory. This
 * function is designed to  be called if an error condition causes an
 * indefinite lockup or hang of an attched LTD. If the
 * UDMA_CheckLtdDetach API fails to confirm a sucessful detach after a
 * timeout condition (as defined by the user application firmware)
 * expires, the application firmware may call this function to bring
 * the driver to a known sane state. This function is part of the
 * higher-level DMA interface, and can be bypassed when  creating
 * transfer descriptors and calling UDMA_DoTransfer() directly.
 * @param[in] pD Pointer to the private data initialized by init
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_ClearState(const PCIE_PrivateData* pD);

/**
 * This function is called to request detach of a previously attached
 * LTD, to allow the application to modify or delete the LTD.  Success
 * returned by this function does not indicate that the LTD has been
 * detached, the application should call the UDMA_CheckLtdDetach API
 * to confirm sucessful detach.  Since only one LTD can be attached at
 * a time, this function does not take a pointer to the LTD as a
 * parameter. This function is part of the higher-level DMA interface,
 * and can be bypassed when  creating transfer descriptors and calling
 * UDMA_DoTransfer() directly.
 * @param[in] pD Pointer to the private data initialized by init
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_RequestLtdDetach(const PCIE_PrivateData* pD);

/**
 * This function is called to confirm completion of a previous request
 * to detach an LTD. If this function returns success, there are no
 * attached LTDs. This function is part of the higher-level DMA
 * interface, and can be bypassed when  creating transfer descriptors
 * and calling UDMA_DoTransfer() directly.
 * @param[in] pD Pointer to the private data initialized by init
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_CheckLtdDetach(const PCIE_PrivateData* pD);

/**
 * This function must be called on reception of a UDMA interrupt, or
 * polled if interrupts are not supported by the system. Note that
 * this function should be called in addition to the separate isr
 * function
 * @param[in] pD Pointer to the private data initialized by init
 * @return CDN_EOK on success
 * @return CDN_EINVAL if pD is NULL
 */
UINT32 PCIE_UDMA_Isr(const PCIE_PrivateData* pD);

// local function
UINT32 CallErrorCallbackIfError(const PCIE_PrivateData * pD);
UINT32 CallAccessRootPortBarFuncs(const PCIE_InitParam *   pInitParam,
                                  UINT32                 retVal,
                                  const PCIE_PrivateData * pD);
UINT32 CallAccessPortBarFuncs(const PCIE_InitParam *   pInitParam,
                              UINT32                 retVal,
                              const PCIE_PrivateData * pD);
UINT32 CallCtrlRootPortBarCheck(const PCIE_PrivateData * pD,
                                UINT32                 retVal,
                                const PCIE_InitParam *   pInitParam);

UINT32 Init_TimingParams(const PCIE_PrivateData * pD,
                         const PCIE_InitParam *   pInitParam);
UINT32 Init_Transmitted(const PCIE_PrivateData * pD,
                        const PCIE_InitParam *   pInitParam);
UINT32 Init_CreditParams(const PCIE_PrivateData * pD,
                         const PCIE_InitParam *   pInitParam);
UINT32 Init_MaskableParams(const PCIE_PrivateData * pD,
                           const PCIE_InitParam *   pInitParam);
UINT32 Init_DebugParams(const PCIE_PrivateData * pD,
                        const PCIE_InitParam *   pInitParam);

UINT32 td_cfg_processTdsInAltd(const PCIE_PrivateData * pD);


/**
 * In order to access the PCIE APIs, the upper layer software must call
 * this global function to obtain the pointer to the driver object.
 * @return PCIE_OBJ* Driver Object Pointer
 */
extern PCIE_OBJ *PCIE_GetInstance(void);
extern PCIE_PrivateData *PCIE_GetPrivateData(void);

/**
 *  @}
 */

#endif  /* AMBAPCIE_DRV_H */
