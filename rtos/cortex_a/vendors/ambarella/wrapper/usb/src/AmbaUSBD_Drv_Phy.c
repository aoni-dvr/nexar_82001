/**
 *  @file AmbaUSBD_Drv_Phy.c
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
 *  @details USB kernel driver phy function.
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
 * Control USB0 PHY power.
 * */
void USB_PhyPllEnable(UINT32 OnOff, UINT32 PortMode)
{
    if (OnOff == 1U) {
        if (PortMode == UPORT_MODE_DEVICE) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE);
            USB_UtilityTaskSleep(3);
            AmbaRTSL_RctSetUsbPortState(USB_ON, USB_DEVICE);
            USB_UtilityTaskSleep(3);
        } else if (PortMode == UPORT_MODE_HOST) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_HOST);
            USB_UtilityTaskSleep(3);
            AmbaRTSL_RctSetUsbPortState(USB_ON, USB_HOST);
            USB_UtilityTaskSleep(3);
        } else if (PortMode == UPORT_MODE_DEVICE_HOST) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE_HOST);
            USB_UtilityTaskSleep(3);
            AmbaRTSL_RctSetUsbPortState(USB_ON, USB_DEVICE_HOST);
            USB_UtilityTaskSleep(3);
        } else {
            // Do nothing here
        }
    } else {
        // it looks we can't turn off PHY power right after setting UDC registers
        // Otherwise system crash (CPU reset).
        // Add delay before turning off PHY power.

        if (PortMode == UPORT_MODE_DEVICE) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE);
            USB_UtilityTaskSleep(3);
        } else if (PortMode == UPORT_MODE_HOST) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_HOST);
            USB_UtilityTaskSleep(3);
        } else if (PortMode == UPORT_MODE_DEVICE_HOST) {
            AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE_HOST);
            USB_UtilityTaskSleep(3);
        } else {
            // Do nothing here
        }
    }
}

/**
 * Data Contact Detection for non-standard USB charger
 * */
UINT32 USB_PhyDataContactDetection(UINT32 T1, UINT32 T2, UINT32 *pContact)
{
    UINT32 Rval;

    if (pContact == NULL) {
        Rval = USB_ERR_PARAMETER_NULL;
    } else {

        if (T2 < 10U) {
            Rval      = USB_ERR_PARAMETER_INVALID;
            *pContact = 0;
        } else {
            UINT32 FsvPlus0;
            UINT32 TempRegValue;
            UINT32 Count, AccumulateLow = 0;
            UINT32 PowerOff;

            // Check USB power status.
            if (AmbaRTSL_RctGetUsbPort0State() != USB_ON) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB_PhyChargerDetection(): USB PHY is off, turn it on.", 0, 0, 0, 0, 0);
                USB_PhyPllEnable(1, UPORT_MODE_DEVICE);
                PowerOff = 1;
            } else {
                PowerOff = 0;
            }

            // Save the original register value.
            TempRegValue = AmbaRTSL_UsbGetPhyCtrl1();

            // Set DcdEnb0 as 1.
            AmbaRTSL_UsbEnaDataContactDet();
            // (todo) misrac: need to check with the SDK7 KAL.
            USB_UtilityTaskSleep(T1);
            for (Count = T2; Count > 0U; Count--) {
                // Check the FsvPlus0.
                FsvPlus0 = AmbaRTSL_UsbGetDpPinState();
                if (FsvPlus0 == 0U) {
                    AccumulateLow++;
                } else {
                    AccumulateLow = 0;
                }
                if (AccumulateLow == 10U) {
                    break;
                }
                USB_UtilityTaskSleep(1);
            }
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB_PhyDataContactDetection(): count = %d", Count, 0, 0, 0, 0);
            if (Count == 0U) {
                *pContact = 0;
            } else {
                // FsvPlus0 has been low for 10ms, which means dcd = 1
                *pContact = 1;
            }
            // Restore the original value.
            AmbaRTSL_UsbSetPhyCtrl1(TempRegValue);
            Rval = USB_ERR_SUCCESS;

            // Restore the original power state.
            if (PowerOff == 1U) {
                USB_PhyPllEnable(0, UPORT_MODE_DEVICE);
            }
        }
    }

    return Rval;
}

/**
 * USB charger detection for standard USB charger
 * */
void USB_PhyChargerDetection(UINT32 *pChargerType)
{
    UINT32 ChargeDetect;
    UINT32 TempRegValue;
    UINT32 PowerOff;

    if (pChargerType == NULL) {
        // Do nothing
    } else {

        // Check USB power status.
        if (AmbaRTSL_RctGetUsbPort0State() != USB_ON) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB_PhyChargerDetection(): USB PHY is off, turn it on.", 0, 0, 0, 0, 0);
            USB_PhyPllEnable(1, UPORT_MODE_DEVICE);
            PowerOff = 1;
        } else {
            PowerOff = 0;
        }

        // Save the original register value.
        TempRegValue = AmbaRTSL_UsbGetPhyCtrl1();

        // Set ChrgSel0 as zero.
        AmbaRTSL_UsbSetChargeSrcSelect(0);
        // set VDATSRCENB0=1 and VDATDETENB0=1
        AmbaRTSL_UsbEnaDataSrcVoltage();
        AmbaRTSL_UsbEnaDatDetectVoltage();
        // Wait for T-VDPSRC_ON (min = 40ms).
        USB_UtilityTaskSleep(40);
        // Check the charge detection.
        ChargeDetect = AmbaRTSL_UsbGetChargerDetect();

        // Restore the original value.
        AmbaRTSL_UsbSetPhyCtrl1(TempRegValue);

        // Try to distinguish between DCP and CDP
        if (ChargeDetect != 0U) {
            // Set ChrgSel0 as one.
            AmbaRTSL_UsbSetChargeSrcSelect(1);
            // set VDATSRCENB0=1 and VDATDETENB0=1
            AmbaRTSL_UsbEnaDataSrcVoltage();
            AmbaRTSL_UsbEnaDatDetectVoltage();
            // Wait for T-VDMSRC_ON (min = 40ms).
            USB_UtilityTaskSleep(40);
            // Check the charge detection.
            ChargeDetect = AmbaRTSL_UsbGetChargerDetect();
            if (ChargeDetect != 0U) {
                *pChargerType = (UINT32)USBD_CHGDET_DCP;
            } else {
                *pChargerType = (UINT32)USBD_CHGDET_CDP;
            }
            // Restore the original value.
            AmbaRTSL_UsbSetPhyCtrl1(TempRegValue);
        } else {
            *pChargerType = (UINT32)USBD_CHGDET_SDP;
        }

        // Restore the original power state.
        if (PowerOff == 1U) {
            USB_PhyPllEnable(0, UPORT_MODE_DEVICE);
        }
    }
}
/**
 * Adjust USB phy signal quality
 * */
UINT32 USB_PhySignalQualityAdjust(UINT32 Adj, UINT32 Port, UINT32 Value)
{
    UINT32 Rval;

    switch (Adj) {
    case ((UINT32)USB_PHY_ADJ_HS_PREEMPHASIS):
        if (AmbaRTSL_USBSetPhyPreEmphasis(Port, Value) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Fail to config the PHY[%d] PreEmphasis[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_FAIL;
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Config the PHY[%d] PreEmphasis[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_SUCCESS;
        }
        break;
    case ((UINT32)USB_PHY_ADJ_FS_TXFSLSTUNE):
        if (AmbaRTSL_USBSetPhyTxFslsTune(Port, Value) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] Fail to config the PHY[%d] TxFslsTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_FAIL;
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USBD] Config the PHY[%d] TxFslsTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_SUCCESS;
        }
        break;
    case ((UINT32)USB_PHY_ADJ_HS_TXRISETUNE):
        if (AmbaRTSL_USBSetPhyTxRiseTune(Port, Value) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] Fail to config the PHY[%d] TxRiseTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_FAIL;
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USBD] Config the PHY[%d] TxRiseTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_SUCCESS;
        }
        break;
    case ((UINT32)USB_PHY_ADJ_HS_TXVREFTUNE):
        if (AmbaRTSL_USBSetPhyTxvRefTune(Port, Value) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] Fail to config the PHY[%d] TxvRefTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_FAIL;
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USBD] Config the PHY[%d] TxvRefTune[%d]", Port, Value, 0, 0, 0);
            Rval = USB_ERR_SUCCESS;
        }
        break;
    default:
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USB] Adjust [%d] is not supported", Adj, 0, 0, 0, 0);
        Rval = USB_ERR_FAIL;
        break;
    }

    return Rval;
}
/**
 * Set USB phy0 owner
 * */
void USB_PhyPhy0OwnerSet(USB0_PORT_OWNER_e Owner)
{
    if (Owner == UDC_OWN_PORT) {
        (void)AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_DEVICE);
    } else if (Owner == UHC_OWN_PORT) {
        (void)AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_HOST);
    } else {
        // shall not be here.
    }
}
/** @} */
