/**
 *  @file AmbaRTSL_USB.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details USB RTSL APIs
 *
 */

#ifndef AMBA_RTSL_USB_H
#define AMBA_RTSL_USB_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#ifndef AMBA_INT_DEF_H
#include "AmbaINT_Def.h"
#endif

// Enable this if you want to disconnect USB in BLD
// Disconnect USB and Should be called in BLD only
// Since Rpu pull-up resistor is enabled by default,
// Host will recognize an unknown device during system bootup.
// It is not a good user experience.
// However, it requires at least 6ms to setup registers in BLD.
// Don't call it if you have boot time concern.
//#define ENABLE_BLD_SOFT_DISCONNECT

#define AMBA_RTSL_DMA_FIX             0x00000000

#define USB_RTSL_DEV_SPD_HI      0   // 00 (RW) - PHY CLK = 30 or 60 MHz
#define USB_RTSL_DEV_SPD_FU      1   // 01 (RW) - PHY CLK = 30 or 60 MHz
#define USB_RTSL_DEV_SPD_LO      2   // 10 (RW) - PHY CLK = 6 MHz
#define USB_RTSL_DEV_SPD_FU48    3   // 11 (RW) - PHY CLK = 48 MHz

#define USB_RTSL_DEV_SELF_POWER              1
#define USB_RTSL_DEV_REMOTE_WAKEUP           1
#define USB_RTSL_DEV_PHY_8BIT                1
#define USB_RTSL_DEV_UTMI_DIR_UNI            0
#define USB_RTSL_DEV_UTMI_DIR_BI             1
#define USB_RTSL_DEV_HALT_ACK                0
#define USB_RTSL_DEV_HALT_STALL              1
#define USB_RTSL_DEV_SET_DESC_STALL          0
#define USB_RTSL_DEV_SET_DESC_ACK            1
#define USB_RTSL_DEV_CSR_PRG                 1

#define USB_RTSL_DEV_RESUME_WAKEUP           1
#define USB_RTSL_DEV_RX_DMA_ON               1
#define USB_RTSL_DEV_TX_DMA_ON               1
#define USB_RTSL_DEV_LITTLE_ENDN             0
#define USB_RTSL_DEV_DESC_UPD_PYL            0
#define USB_RTSL_DEV_DESC_UPD_PKT            1
#define USB_RTSL_DEV_THRESH_ON               1
#define USB_RTSL_DEV_BURST_ON                1
#define USB_RTSL_DEV_DMA_MODE                1
#define USB_RTSL_DEV_SET_SOFT_DISCON         1
#define USB_RTSL_DEV_SET_FLUSH_RXFIFO        1
#define USB_RTSL_DEV_SET_NAK                 1
#define USB_RTSL_DEV_SET_CSR_DONE            1
#define USB_RTSL_DEV_BURST_LEN               7


typedef enum {
    USB_PHY0_DEVICE,  // Phy0 = Device
    USB_PHY0_HOST,  // Phy0 = Host
    USB_PHY0_UNDEF
} USB_PHY0_OWNER_e;

typedef enum {
    USB_PORT_0,
    USB_PORT_1
} USB_PORT_e;

typedef enum {
    USB_ON,         /* Enable USB port */
    USB_OFF         /* Disable USB port */
} USB_PORT_STATE_e;

typedef enum {
    USB_DEVICE,         /* device mode */
    USB_HOST,           /* host mode */
    USB_DEVICE_HOST     /* host and device mode */
} USB_PORT_OP_MODE_e;

typedef enum {
    USB_DEV_INT_SC,
    USB_DEV_INT_SI,
    USB_DEV_INT_ES,
    USB_DEV_INT_UR,
    USB_DEV_INT_US,
    USB_DEV_INT_SOF,
    USB_DEV_INT_ENUM,
    USB_DEV_INT_RMTWKP,
    USB_DEV_INT_ALL
} AMBA_USB_DEV_INT_e;

typedef struct {
    UINT32 (*GetMutex)(void);
    UINT32 (*PutMutex)(void);
} RTSL_USB_Mutex_s;

UINT32 AmbaRTSL_USBIsrHook(UINT32 IntID, UINT32 TriggerType, AMBA_INT_ISR_f IsrEntry);
UINT32 AmbaRTSL_USBIsrEnable(UINT32 IntID);
UINT32 AmbaRTSL_USBIsrDisable(UINT32 IntID);
UINT32 AmbaRTSL_USBIsIsrEnable(UINT32 IntID);
UINT32 AmbaRTSL_USBGetBaseAddress(void);
UINT32 AmbaRTSL_USBReadAddress(UINT32 offset);
void AmbaRTSL_USBSetEhciOCPolarity(UINT32 polarity);
UINT32 AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_OWNER_e owner);
void *AmbaRTSL_USBPhys2Virt(const void *phys);
void *AmbaRTSL_USBVirt2Phys(const void *virt);
UINT32 AmbaRTSL_UsbGetDevConfig(void);
void AmbaRTSL_UsbSetDevConfig(UINT32 config);
void AmbaRTSL_UsbSetDevConfigSpd(UINT32 speed);
void AmbaRTSL_UsbSetDevConfigRwkp(UINT32 Remotewakeup);
void AmbaRTSL_UsbSetDevConfigSp(UINT32 SlefPowered);
void AmbaRTSL_UsbSetDevConfigPyType(UINT32 PhyType);
void AmbaRTSL_UsbSetDevConfigRev(void);
void AmbaRTSL_UsbSetDevConfigHaltSts(UINT32 HaltStatus);
void AmbaRTSL_UsbSetDevConfigDynProg(UINT32 DynamicProgram);
void AmbaRTSL_UsbSetDevConfigSetDesc(UINT32 SetDescriptor);
/*
 * USB_dev_control Register
 */
void AmbaRTSL_UsbSetDevCtl(UINT32 control);
void AmbaRTSL_UsbSetDevCtlRde(UINT32 EnableRxDma);
UINT32 AmbaRTSL_UsbGetDevCtlRde(void);
void AmbaRTSL_UsbSetDevCtlTde(UINT32 EnableTxDma);
void AmbaRTSL_UsbSetDevCtlBe(UINT32 BigEndian);
void AmbaRTSL_UsbSetDevCtlBren(UINT32 BurstEnable);
void AmbaRTSL_UsbSetDevCtlMode(UINT32 UsbMode);
void AmbaRTSL_UsbSetDevCtlSd(UINT32 SoftDisconnect);
void AmbaRTSL_UsbSetDevCtlCsrdone(UINT32 CsrDone);
void AmbaRTSL_UsbSetDevCtlSrxflush(UINT32 RxFifoFlush);
void AmbaRTSL_UsbSetDevCtlBrlen(UINT32 BurstLength);
/*
 * USB_dev_status Register
 */
UINT32 AmbaRTSL_UsbGetDevStatusCfg(void);
UINT32 AmbaRTSL_UsbGetDevStatusIntf(void);
UINT32 AmbaRTSL_UsbGetDevStatusAlt(void);
UINT32 AmbaRTSL_UsbGetDevStatusEnumSpd(void);
UINT32 AmbaRTSL_UsbGetDevStatusRfEmpty(void);
UINT32 AmbaRTSL_UsbGetDevStatusTs(void);
/*
 * USB_DevIntStatus status/mask Register
 */
UINT32 AmbaRTSL_UsbGetDevIntStatus(void);
void AmbaRTSL_UsbSetDevIntStatus(UINT32 InterruptStatus);


/*
 * USB_endpoint_int_status/mask
 */
UINT32 AmbaRTSL_UsbGetEpInt(void);
void AmbaRTSL_UsbSetEpInt(UINT32 interrupt);
void AmbaRTSL_UsbEnInEpInt(UINT32 endpoint);
void AmbaRTSL_UsbEnOutEpInt(UINT32 endpoint);
void AmbaRTSL_UsbDisInEpInt(UINT32 endpoint);
void AmbaRTSL_UsbDisOutEpInt(UINT32 endpoint);
/*
 *  USB_end[n]_ctrl_in/out Registers
 */
UINT32 AmbaRTSL_UsbGetEpInStall(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInNAK(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInPollDemand(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInEndPointType(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInCtrl(UINT32 endpoint, UINT32 control);
void AmbaRTSL_UsbSetEpInStall(UINT32 endpoint, UINT32 stall);
void AmbaRTSL_UsbSetEpInCtrlF(UINT32 endpoint, UINT32 FlushTxFifo);
void AmbaRTSL_UsbSetEpInPollDemand(UINT32 endpoint, UINT32 PollDemand);
void AmbaRTSL_UsbSetEpInEndPointType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEpInNAK(UINT32 endpoint, UINT32 nak);
void AmbaRTSL_UsbClearEpInNAK(UINT32 endpoint, UINT32 ClearNak);
UINT32 AmbaRTSL_UsbGetEpOutNAK(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutCtrl(UINT32 endpoint, UINT32 control);
void AmbaRTSL_UsbSetEpOutStall(UINT32 endpoint, UINT32 stall);
void AmbaRTSL_UsbSetEpOutType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEpOutNAK(UINT32 endpoint, UINT32 nak);
void AmbaRTSL_UsbClearEpOutNAK(UINT32 endpoint, UINT32 ClearNak);
void AmbaRTSL_UsbSetEpOutRxReady(UINT32 endpoint, UINT32 RxReady);
//void AmbaRTSL_UsbSetEpOutCtrlClsDesc(UINT32 endpoint, UINT32 CloseDescriptor);
/*
 *  USB_end[n]_status_in/out Registers
 */
UINT32 AmbaRTSL_UsbGetEpInStatus(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusBna(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusTdc(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusRcs(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStatusRss(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInStsTxEmpty(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInStatus(UINT32 endpoint, UINT32 status);
void AmbaRTSL_UsbClrEpInStatusIn(UINT32 endpoint, UINT32 in);
void AmbaRTSL_UsbClrEpInStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable);
void AmbaRTSL_UsbClrEpInStatusTdc(UINT32 endpoint, UINT32 TxDmaComplete);
void AmbaRTSL_UsbClrEpInStatusRcs(UINT32 endpoint, UINT32 RxClearStall);
void AmbaRTSL_UsbClrEpInStatusRss(UINT32 endpoint, UINT32 RxSetStall);
void AmbaRTSL_UsbClrEpInStsTxEmpty(UINT32 endpoint, UINT32 TxFifoEmpty);
UINT32 AmbaRTSL_UsbGetEpOutStatus(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusOut(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusBna(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusRcs(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpOutStatusRss(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutStatus(UINT32 endpoint, UINT32 status);
void AmbaRTSL_UsbClrEpOutStatusOut(UINT32 endpoint, UINT32 out);
void AmbaRTSL_UsbClrEpOutStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable);
void AmbaRTSL_UsbClrEpOutStatusRcs(UINT32 endpoint, UINT32 RxClearStall);
void AmbaRTSL_UsbClrEpOutStatusRss(UINT32 endpoint, UINT32 RxSetStall);
/*
 *  USB_end[n]_buffsize_in Registers
 */
UINT32 AmbaRTSL_UsbGetEpInBuffsize(UINT32 endpoint);
UINT32 AmbaRTSL_UsbGetEpInBfszSize(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInBuffsize(UINT32 endpoint, UINT32 size);
/*
 *  USB_end[n]_packet_fm_out Registers
 */

/*
 *  USB_end[n]_max_pktsz Registers
 */
void AmbaRTSL_UsbSetEpInMaxpksz(UINT32 endpoint, UINT32 size);
void AmbaRTSL_UsbSetEpOutMaxpksz(UINT32 endpoint, UINT32 size);
/*
 *  USB_end[n]_setup_buffptr Registers
 */
UINT32 AmbaRTSL_UsbGetEpOutSetupbuf(UINT32 endpoint);
void AmbaRTSL_UsbSetEpOutSetupbuf(UINT32 endpoint, UINT32 ptr);
/*
 *  USB_end[n]_desptr_out Registers
 */
UINT32 AmbaRTSL_UsbGetEpInDesptr(UINT32 endpoint);
void AmbaRTSL_UsbSetEpInDesptr(UINT32 endpoint, UINT32 ptr);
void AmbaRTSL_UsbSetEpOutDesptr(UINT32 endpoint, UINT32 ptr);
/*
 *  USB_udc20 Endpoint Register
 */
UINT32 AmbaRTSL_UsbGetEp20LogicalID(UINT32 endpoint);
void AmbaRTSL_UsbSetEp20(UINT32 endpoint, UINT32 value);
void AmbaRTSL_UsbSetEp20LogicalID(UINT32 endpoint, UINT32 LogicalID);
void AmbaRTSL_UsbSetEp20EndPointDir(UINT32 endpoint, UINT32 direction);
void AmbaRTSL_UsbSetEp20EndPointType(UINT32 endpoint, UINT32 type);
void AmbaRTSL_UsbSetEp20ConfigID(UINT32 endpoint, UINT32 ConfigID);
void AmbaRTSL_UsbSetEp20Interface(UINT32 endpoint, UINT32 InterfaceID);
void AmbaRTSL_UsbSetEp20AltSetting(UINT32 endpoint, UINT32 alternate);
void AmbaRTSL_UsbSetEp20MaxPktSize(UINT32 endpoint, UINT32 MaxPacketSize);
UINT32 AmbaRTSL_UsbGetPhyCtrl1(void);
UINT32 AmbaRTSL_UsbGetDpPinState(void);
UINT32 AmbaRTSL_UsbGetChargerDetect(void);
void AmbaRTSL_UsbSetPhyCtrl1(UINT32 value);
void AmbaRTSL_UsbEnaDataContactDet(void);
void AmbaRTSL_UsbEnaDataSrcVoltage(void);
void AmbaRTSL_UsbEnaDatDetectVoltage(void);
void AmbaRTSL_UsbSetChargeSrcSelect(UINT32 value);
void AmbaRTSL_UsbVbusVicConfig(UINT32 PinID, UINT32 PinType);
UINT32 AmbaRTSL_UsbVbusVicRawStaus(void);
void AmbaRTSL_UsbClrDevInt(AMBA_USB_DEV_INT_e IntID);
void AmbaRTSL_UsbSetDevMaskInt(AMBA_USB_DEV_INT_e IntID, UINT32 Mask);
void AmbaRTSL_RctSetUsb0Commononn(UINT32 value);
void AmbaRTSL_RctUsbHostSoftReset(void);
void AmbaRTSL_RctSetUsbPortState(USB_PORT_STATE_e PortState, USB_PORT_OP_MODE_e PortMode);
USB_PORT_STATE_e AmbaRTSL_RctGetUsbPort0State(void);
UINT32 AmbaRTSL_USBSetPhyPreEmphasis(UINT32 PhyNum, UINT32 value);
UINT32 AmbaRTSL_USBSetPhyTxRiseTune(UINT32 PhyNum, UINT32 value);
UINT32 AmbaRTSL_USBSetPhyTxvRefTune(UINT32 PhyNum, UINT32 value);
UINT32 AmbaRTSL_USBSetPhyTxFslsTune(UINT32 PhyNum, UINT32 value);
UINT32 AmbaRTSL_USBRegisterMutex(RTSL_USB_Mutex_s *func);

#ifdef ENABLE_BLD_SOFT_DISCONNECT
void   AmbaRTSL_UsbSetDevCtlSdInBld(void);
#endif

typedef struct {
    UINT64 BaseAddress;
    UINT32 IntPin;
    UINT32 IntType;
} RTSL_USB_MODULE_SETTING_s;

typedef struct {
    RTSL_USB_MODULE_SETTING_s Udc;
    RTSL_USB_MODULE_SETTING_s Ohci;
    RTSL_USB_MODULE_SETTING_s Ehci;
    RTSL_USB_MODULE_SETTING_s Rct;
} RTSL_USB_HW_SETTING_s;

void AmbaRTSL_UsbSetHwInfo(const RTSL_USB_HW_SETTING_s *HwSettings);

#endif  /* _AMBA_RTSL_USB_H_ */
