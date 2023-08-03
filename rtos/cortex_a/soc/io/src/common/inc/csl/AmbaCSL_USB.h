/**
 *  @file AmbaCSL_USB.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for Universal Serial Bus (USB) Device Controller Control APIs
 *
 */

#ifndef AMBA_CSL_USB_H
#define AMBA_CSL_USB_H

#include "AmbaReg_USB.h"
#ifndef AMBA_CSL_RCT_H
#include "AmbaCSL_RCT.h"
#endif
#include "AmbaCSL_Scratchpad.h"

/*
 * Macro Definitions
 */

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV25)  || defined(CONFIG_SOC_CV28)  || defined(CONFIG_SOC_CV22)  || defined(CONFIG_SOC_H32)
static inline UINT32 AmbaCSL_Usb3GetDeviceConn(void)
{
    return pAmbaScratchpadNS_Reg->UsbEhciSideBand.UsbDeviceConnect;
}
static inline void AmbaCSL_UsbSetEhciOCPolarity(UINT32 d)
{
    pAmbaScratchpadNS_Reg->AhbCtrl.OverCurrentPolarity = (UINT8)d;
}
#elif defined(CONFIG_SOC_CV2)
static inline UINT32 AmbaCSL_Usb3GetDeviceConn(void)
{
    return pAmbaScratchpadS_Reg->UsbEhciSideBand.UsbDeviceConnect;
}
static inline void AmbaCSL_UsbSetEhciOCPolarity(UINT32 d)
{
    pAmbaScratchpadS_Reg->AhbCtrl.OverCurrentPolarity = (UINT8)d;
}
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static inline UINT32 AmbaCSL_Usb3GetDeviceConn(void)
{
    return pAmbaScratchpadNS_Reg->UsbEhciSideBand.UsbDeviceConnect;
}
//FIXME, there is no AhbCtrl.OverCurrentPolarity
static inline void AmbaCSL_UsbSetEhciOCPolarity(UINT32 d)
{
    (void)d;
}
#else
#error "AmbaCSL_Usb3GetDeviceConn() no defined."
#endif

#define RCT_USB_ANA_PWR_POS            ((UINT32)(0x50U))
#define RCT_USBP0_SEL_POS              ((UINT32)(0x2C0U))
#define RCT_USBP0_CTRL_POS             ((UINT32)(0x2C4U))
#define RCT_USBP0_CTRL1_POS            ((UINT32)(0x2C8U))
#define RCT_USBC0_CTRL_POS             ((UINT32)(0x2CCU))

// ANA_PWR reg
#define RCT_USBP0_SPND_DEV_SFT         ((UINT32)(1U))
#define RCT_USBP0_SPND_DEV_MASK        (((UINT32)1U) << RCT_USBP0_SPND_DEV_SFT)
#define RCT_USBP0_NVR_SPND_DEV_SFT     ((UINT32)(2U))
#define RCT_USBP0_NVR_SPND_DEV_MASK    (((UINT32)1U) <<RCT_USBP0_NVR_SPND_DEV_SFT)
#define RCT_USBP0_SPND_HOST_SFT        ((UINT32)(12U))
#define RCT_USBP0_SPND_HOST_MASK       (((UINT32)1U) << RCT_USBP0_SPND_HOST_SFT)

// PHY0 SEL reg
#define RCT_USBP0_SEL0_SFT             ((UINT32)(0U))
#define RCT_USBP0_SEL0_MASK            (((UINT32)1U) << RCT_USBP0_SEL0_SFT)

// PHY0 CTRL reg
#define RCT_USBP0_RESET_SFT            ((UINT32)(2U))
#define RCT_USBP0_REFCLK_SEL_SFT       ((UINT32)(3U))
#define RCT_USBP0_COMNONONN_SFT        ((UINT32)(6U))
#define RCT_USBP0_COMPDIS_TUNE_SFT     ((UINT32)(7U))
#define RCT_USBP0_OTG_TUNE_SFT         ((UINT32)(10U))
#define RCT_USBP0_SQRX_TUNE_SFT        ((UINT32)(13U))
#define RCT_USBP0_TX_FSLS_TUNE_SFT     ((UINT32)(16U))
#define RCT_USBP0_TX_RISE_TUNE_SFT     ((UINT32)(20U))
#define RCT_USBP0_TX_VREF_TUNE_SFT     ((UINT32)(22U))
#define RCT_USBP0_TX_HSXV_TUNE_SFT     ((UINT32)(26U))
#define RCT_USBP0_ATE_RESET_SFT        ((UINT32)(28U))
#define RCT_USBP0_SLEEPM_TUNE_SFT      ((UINT32)(30U))

#define RCT_USBP0_RESET_MASK           (((UINT32)1U) << RCT_USBP0_RESET_SFT)
#define RCT_USBP0_REFCLK_SEL_MASK      (((UINT32)0x03U) << RCT_USBP0_REFCLK_SEL_SFT)
#define RCT_USBP0_COMNONONN_MASK       (((UINT32)0x01U) << RCT_USBP0_COMNONONN_SFT)
#define RCT_USBP0_COMPDIS_TUNE_MASK    (((UINT32)0x07U) << RCT_USBP0_COMPDIS_TUNE_SFT)
#define RCT_USBP0_OTG_TUNE_MASK        (((UINT32)0x07U) << RCT_USBP0_OTG_TUNE_SFT)
#define RCT_USBP0_SQRX_TUNE_MASK       (((UINT32)0x07U) << RCT_USBP0_SQRX_TUNE_SFT)
#define RCT_USBP0_TX_FSLS_TUNE_MASK    (((UINT32)0x0FU) << RCT_USBP0_TX_FSLS_TUNE_SFT)
#define RCT_USBP0_TX_RISE_TUNE_MASK    (((UINT32)0x03U) << RCT_USBP0_TX_RISE_TUNE_SFT)
#define RCT_USBP0_TX_VREF_TUNE_MASK    (((UINT32)0x0FU) << RCT_USBP0_TX_VREF_TUNE_SFT)
#define RCT_USBP0_TX_HSXV_TUNE_MASK    (((UINT32)0x03U) << RCT_USBP0_TX_HSXV_TUNE_SFT)
#define RCT_USBP0_ATE_RESET_MASK       (((UINT32)0x01U) << RCT_USBP0_ATE_RESET_SFT)
#define RCT_USBP0_SLEEPM_TUNE_MASK     (((UINT32)0x01U) << RCT_USBP0_SLEEPM_TUNE_SFT)

// PHY0 CTRL1 reg
#define RCT_USBP0_DCD_ENABLE_SFT       ((UINT32)(1U))
#define RCT_USBP0_SRC_VOL_EN_SFT       ((UINT32)(2U))
#define RCT_USBP0_DET_VOL_EN_SFT       ((UINT32)(3U))
#define RCT_USBP0_CHG_SRC_SEL_SFT      ((UINT32)(4U))
#define RCT_USBP0_TX_AMP_TUNE_SFT      ((UINT32)(12U))

#define RCT_USBP0_DCD_ENABLE_MASK      (((UINT32)0x01U) << RCT_USBP0_DCD_ENABLE_SFT)
#define RCT_USBP0_SRC_VOL_EN_MASK      (((UINT32)0x01U) << RCT_USBP0_SRC_VOL_EN_SFT)
#define RCT_USBP0_DET_VOL_EN_MASK      (((UINT32)0x01U) << RCT_USBP0_DET_VOL_EN_SFT)
#define RCT_USBP0_CHG_SRC_SEL_MASK     (((UINT32)0x01U) << RCT_USBP0_CHG_SRC_SEL_SFT)
#define RCT_USBP0_TX_AMP_TUNE_MASK     (((UINT32)0x03U) << RCT_USBP0_TX_AMP_TUNE_SFT)

// USB Controller reg
#define RCT_USBC0_RESET_HOST_SFT       ((UINT32)(0U))
#define RCT_USBC0_RESET_HOST_MASK      ((UINT32)(1U))
#define RCT_USBC0_RESET_DEV_SFT        ((UINT32)(1U))
#define RCT_USBC0_RESET_DEV_MASK       (((UINT32)1U) << RCT_USBC0_RESET_DEV_SFT)

//------------ USB PHY part start ------------------

/*
 * USB Port Definitions
 */
typedef enum {
    AMB_RCT_USB_PORT_0
} AMBA_RCT_USB_PORT_e;

/*
 * USB Port State Definitions
 */
typedef enum {
    AMB_RCT_USB_ON,         /* Enable USB port */
    AMB_RCT_USB_OFF,        /* Disable USB port */
    AMB_RCT_USB_SUSPEND,    /* Force USB port into suspend state */
    AMB_RCT_USB_ALWAYS_ON   /* Enable USB port  & force USB to never suspend */
} AMBA_RCT_USB_PORT_STATE_e;

void AmbaCSL_RctSetUsbPhy0Off(void);
void AmbaCSL_RctSetUsbPhy0AlwaysOn(void);
void AmbaCSL_RctSetUsbHostOn(void);
void AmbaCSL_RctSetUsbHostOff(void);
UINT32 AmbaCSL_RctGetUsbPhy0NvrSuspend(void);

// ------------------------------------------------------
// USB RESET CONTROL Register
// ------------------------------------------------------
void AmbaCSL_RctSetUsbHostSoftReset(UINT32 value);

// ------------------------------------------------------
// USB PHY0 SELECT Register
// ------------------------------------------------------
void   AmbaCSL_RctSetUsbPhy0Select(UINT32 value);

// ------------------------------------------------------
// USB PHY0 Control0 Register
// ------------------------------------------------------
void   AmbaCSL_RctSetUsbPhy0Commononn(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0TxRiseTune(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0TxvRefTune(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0TxFslsTune(UINT32 value);

// ------------------------------------------------------
// USB PHY0 Control1 Register
// ------------------------------------------------------
void   AmbaCSL_RctSetUsbPhy0Ctrl1(UINT32 value);
UINT32 AmbaCSL_RctGetUsbPhy0Ctrl1(void);
void   AmbaCSL_RctSetUsbPhy0TxPreEmp(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0DCDetect(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0DSrcVolt(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0DDetVolt(UINT32 value);
void   AmbaCSL_RctSetUsbPhy0ChgrSrcSel(UINT32 value);

void AmbaCSL_RctUsbHostSoftReset(void);

//------------ USB PHY part end ------------------

/*
 * USB_dev_control Register
 */
UINT32 AmbaCSL_UsbGetDevConfig(void);
void AmbaCSL_UsbSetDevConfig(UINT32 value);
void AmbaCSL_UsbSetDevConfigSpd(UINT32 value);
void AmbaCSL_UsbSetDevConfigRwkp(UINT32 value);
void AmbaCSL_UsbSetDevConfigSp(UINT32 value);
void AmbaCSL_UsbSetDevConfigPyType(UINT32 value);
void AmbaCSL_UsbSetDevConfigRev(UINT32 value);
void AmbaCSL_UsbSetDevConfigHaltSts(UINT32 value);
void AmbaCSL_UsbSetDevConfigDynProg(UINT32 value);
void AmbaCSL_UsbSetDevConfigSetDesc(UINT32 value);

/*
 * USB_dev_control Register
 */
#define AmbaCSL_UsbGetDevCtl                        pAmbaUSB_Reg->DevCtrl
void AmbaCSL_UsbSetDevCtl(UINT32 ctrl);
void AmbaCSL_UsbSetDevCtlRde(UINT32 value);
UINT32 AmbaCSL_UsbGetDevCtlRde(void);
void AmbaCSL_UsbSetDevCtlTde(UINT32 value);
void AmbaCSL_UsbSetDevCtlBe(UINT32 value);
void AmbaCSL_UsbSetDevCtlBren(UINT32 value);
void AmbaCSL_UsbSetDevCtlMode(UINT32 value);
void AmbaCSL_UsbSetDevCtlSd(UINT32 value);
void AmbaCSL_UsbSetDevCtlCsrdone(UINT32 value);
void AmbaCSL_UsbSetDevCtlSrxflush(UINT32 value);
void AmbaCSL_UsbSetDevCtlBrlen(UINT32 value);

/*
 * USB_dev_status Register
 */
UINT32 AmbaCSL_UsbGetDevStatusCfg(void);
UINT32 AmbaCSL_UsbGetDevStatusIntf(void);
UINT32 AmbaCSL_UsbGetDevStatusAlt(void);
UINT32 AmbaCSL_UsbGetDevStatusEnumSpd(void);
UINT32 AmbaCSL_UsbGetDevStatusRFEmpty(void);
UINT32 AmbaCSL_UsbGetDevStatusTs(void);

/*
 * USB_DevIntStatus status/mask Register
 */

#define AmbaCSL_UsbGetDevIntStatus                  pAmbaUSB_Reg->DevIntStatus
void AmbaCSL_UsbSetDevIntStatus(UINT32 status);
void AmbaCSL_UsbClearDevIntSc(void);
void AmbaCSL_UsbClearDevIntSi(void);
void AmbaCSL_UsbClearDevIntES(void);
void AmbaCSL_UsbClearDevIntUR(void);
void AmbaCSL_UsbClearDevIntUS(void);
void AmbaCSL_UsbClearDevIntSOF(void);
void AmbaCSL_UsbClearDevIntENUM(void);
void AmbaCSL_UsbClearDevIntRMTWKP(void);

#define AmbaCSL_UsbGetDevIntMask                    pAmbaUSB_Reg->DevIntMask
void AmbaCSL_UsbSetDevIntMask(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskSc(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskSi(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskES(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskUR(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskUS(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskSOF(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskENUM(UINT32 value);
void AmbaCSL_UsbSetDevIntMaskRMTWKP(UINT32 value);

/*
 * USB_endpoint_int_status
 */

UINT32 AmbaCSL_UsbGetEpInt(void);
void AmbaCSL_UsbSetEpInt(UINT32 d);

/*
 * USB_endpoint_int_mask
 */
void AmbaCSL_UsbEnInEpInt(UINT32 id);
void AmbaCSL_UsbEnOutEpInt(UINT32 id);
void AmbaCSL_UsbDisInEpInt(UINT32 id);
void AmbaCSL_UsbDisOutEpInt(UINT32 id);

/*
 *  USB_end[n]_ctrl in Registers
 */
UINT32 AmbaCSL_UsbGetEpInStall(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInNAK(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInPollDemand(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInEndPointType(UINT32 id);
void AmbaCSL_UsbSetEpInCtrl(UINT32 id, UINT32 ctrl);
void AmbaCSL_UsbSetEpInStall(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpInCtrlF(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpInPollDemand(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpInEndPointType(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpInNAK(UINT32 id, UINT32 value);
void AmbaCSL_UsbClearEpInNAK(UINT32 id, UINT32 value);


/*
 *  USB_end[n]_ctrl out Registers
 */

UINT32 AmbaCSL_UsbGetEpOutNAK(UINT32 id);
void AmbaCSL_UsbSetEpOutCtrl(UINT32 id, UINT32 ctrl);
void AmbaCSL_UsbSetEpOutStall(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpOutEndPointType(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpOutNAK(UINT32 id, UINT32 value);
void AmbaCSL_UsbClearEpOutNAK(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEpOutRxReady(UINT32 id, UINT32 value);

/*
 *  USB_end[n]_status_in Registers
 */
UINT32 AmbaCSL_UsbGetEpInStatus(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInStatusBna(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInStatusTdc(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInStatusRcs(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInStatusRss(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInStatusTxEmpty(UINT32 id);
void AmbaCSL_UsbSetEpInStatus(UINT32 id, UINT32 sts);
void AmbaCSL_UsbClrEpInStatusIn(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpInStatusBna(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpInStatusTdc(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpInStatusRcs(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpInStatusRss(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpInStatusTxEmpty(UINT32 id, UINT32 value);

/*
 *  USB_end[n]_status_out Registers
 */
UINT32 AmbaCSL_UsbGetEpOutStatus(UINT32 id);
UINT32 AmbaCSL_UsbGetEpOutStatusOut(UINT32 id);
UINT32 AmbaCSL_UsbGetEpOutStatusBna(UINT32 id);
UINT32 AmbaCSL_UsbGetEpOutStatusRcs(UINT32 id);
UINT32 AmbaCSL_UsbGetEpOutStatusRss(UINT32 id);
void AmbaCSL_UsbSetEpOutStatus(UINT32 id, UINT32 sts);
void AmbaCSL_UsbClrEpOutStatusOut(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpOutStatusBna(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpOutStatusRcs(UINT32 id, UINT32 value);
void AmbaCSL_UsbClrEpOutStatusRss(UINT32 id, UINT32 value);

/*
 *  USB_end[n]_buffsize_in Registers
 */
UINT32 AmbaCSL_UsbGetEpInBuffsize(UINT32 id);
UINT32 AmbaCSL_UsbGetEpInBfszSize(UINT32 id);
void AmbaCSL_UsbSetEpInBfszSize(UINT32 id, UINT32 value);

/*
 *  USB_end[n]_packet_fm_out Registers
 */

/*
 *  USB_end[n]_max_pktsz Registers
 */
void   AmbaCSL_UsbSetEpInMaxpksz(UINT32 id, UINT32 sz);
void   AmbaCSL_UsbSetEpOutMaxpksz(UINT32 id, UINT32 sz);
/*
 *  USB_end[n]_setup_buffptr Registers
 */
UINT32 AmbaCSL_UsbGetEpOutSetupbuf(UINT32 id);
void   AmbaCSL_UsbSetEpOutSetupbuf(UINT32 id, UINT32 ptr);
/*
 *  USB_end[n]_desptr_out Registers
 */
UINT32 AmbaCSL_UsbGetEpInDesptr(UINT32 id);
void   AmbaCSL_UsbSetEpInDesptr(UINT32 id, UINT32 ptr);
void   AmbaCSL_UsbSetEpOutDesptr(UINT32 id, UINT32 ptr);
/*
 *  USB_end[n]_read/write_confir Registers
 */
/*
 *  USB_udc20 Endpoint Register
 */
UINT32 AmbaCSL_UsbGetEp20LogicalID(UINT32 id);
void AmbaCSL_UsbSetEp20(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20LogicalID(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20EndPointDir(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20EndPointType(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20ConfigID(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20Interface(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20AltSetting(UINT32 id, UINT32 value);
void AmbaCSL_UsbSetEp20MaxPacketSize(UINT32 id, UINT32 value);

/*
 * Defined in AmbaCSL_USB.c
 */
#if defined(CONFIG_DEVICE_TREE_SUPPORT) || defined(CONFIG_QNX)
extern AMBA_USB_REG_s *pAmbaUSB_Reg;
#else
extern AMBA_USB_REG_s *const pAmbaUSB_Reg;
#endif

void AmbaCSL_UsbVbusVicConfig(UINT32 PinID, UINT32 PinType);
UINT32 AmbaCSL_UsbVbusVicRawStaus(void);

typedef struct {
    UINT64 BaseAddress;
    UINT32 IntPin;
    UINT32 IntType;
} CSL_USB_MODULE_SETTING_s;

typedef struct {
    CSL_USB_MODULE_SETTING_s Udc;
    CSL_USB_MODULE_SETTING_s Ohci;
    CSL_USB_MODULE_SETTING_s Ehci;
    CSL_USB_MODULE_SETTING_s Rct;
} CSL_USB_HW_SETTING_s;

extern CSL_USB_HW_SETTING_s g_csl_usb_hws;

void   AmbaCSL_UsbSetHwInfo(const CSL_USB_HW_SETTING_s *HwSettings);
UINT32 AmbaCSL_UsbRead32(UINT64 Address);
void   AmbaCSL_UsbWrite32(UINT64 Address, UINT32 Value);
void   AmbaCSL_UsbMaskSetWrite32(UINT64 Address, UINT32 Value, UINT32 Mask, UINT32 Shift);
void   AmbaCSL_UsbMaskClearWrite32(UINT64 Address, UINT32 Mask);

#endif /* AMBA_CSL_USB_H */
