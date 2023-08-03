/**
 *  @file AmbaRTSL_USB.c
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
 *  @details Universal Serial Bus (USB) Device/Host RTSL Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_USB.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_USB.h"
#include "AmbaMMU.h"

static RTSL_USB_Mutex_s *UsbRtslMutex = NULL;
static RTSL_USB_HW_SETTING_s rtsl_usb_hws = {0};

UINT32 AmbaRTSL_USBRegisterMutex(RTSL_USB_Mutex_s *func)
{
    UsbRtslMutex = func;

    return 0;
}

void *AmbaRTSL_USBPhys2Virt(const void *phys)
{
#ifndef __aarch64__
    // 32-bit environment
    UINT32 virt_addr = 0x0;
    UINT32 phys_addr = 0x0;
    void  *virt_ptr = NULL;
    AmbaMisra_TypeCast32(&phys_addr, &phys);

    if (AmbaMMU_Phys32ToVirt32(phys_addr, &virt_addr) != 0U) {
        // ignore error
    }
    AmbaMisra_TypeCast32(&virt_ptr, &virt_addr);
    return virt_ptr;
#else
    // 64-bit environment
    ULONG virt_addr = 0x0;
    ULONG phys_addr = 0x0;
    void  *virt_ptr = NULL;
    AmbaMisra_TypeCast(&phys_addr, &phys);

    if (AmbaMMU_Phys64ToVirt64(phys_addr, &virt_addr) != 0U) {
        // ignore error
    }
    AmbaMisra_TypeCast(&virt_ptr, &virt_addr);
    return virt_ptr;
#endif
}

void *AmbaRTSL_USBVirt2Phys(const void *virt)
{
#ifndef __aarch64__
    // 32-bit environment
    UINT32 phys_addr = 0x0;
    UINT32 virt_addr = 0x0;
    void *phys_ptr = NULL;
    AmbaMisra_TypeCast32(&virt_addr, &virt);

    if (AmbaMMU_Virt32ToPhys32(virt_addr, &phys_addr) != 0U) {
        // ignore error
    }
    AmbaMisra_TypeCast32(&phys_ptr, &phys_addr);
    return phys_ptr;
#else
    // 64-bit environment
    ULONG phys_addr = 0x0;
    ULONG virt_addr = 0x0;
    void *phys_ptr = NULL;
    AmbaMisra_TypeCast64(&virt_addr, &virt);

    if (AmbaMMU_VirtToPhys(virt_addr, &phys_addr) != 0U) {
        // ignore error
    }
    AmbaMisra_TypeCast64(&phys_ptr, &phys_addr);
    return phys_ptr;
#endif
}

static void MUTEX_LOCK(void)
{
    if ((UsbRtslMutex != NULL) && (UsbRtslMutex->GetMutex != NULL)) {
        if (UsbRtslMutex->GetMutex() != 0U) {
            /* ignore error */
        }
    }
}
static void MUTEX_UNLOCK(void)
{
    if ((UsbRtslMutex != NULL) && (UsbRtslMutex->PutMutex != NULL)) {
        if (UsbRtslMutex->PutMutex() != 0U) {
            /* ignore error */
        }
    }
}


void AmbaRTSL_UsbSetHwInfo(const RTSL_USB_HW_SETTING_s *HwSettings)
{
    CSL_USB_HW_SETTING_s csl_hws = {0};

    rtsl_usb_hws = *HwSettings;

    // set information for CSL layer driver
    csl_hws.Udc.BaseAddress = HwSettings->Udc.BaseAddress;
    csl_hws.Udc.IntPin      = HwSettings->Udc.IntPin;
    csl_hws.Udc.IntType     = HwSettings->Udc.IntType;
    csl_hws.Ehci.BaseAddress = HwSettings->Ehci.BaseAddress;
    csl_hws.Ehci.IntPin      = HwSettings->Ehci.IntPin;
    csl_hws.Ehci.IntType     = HwSettings->Ehci.IntType;
    csl_hws.Ohci.BaseAddress = HwSettings->Ohci.BaseAddress;
    csl_hws.Ohci.IntPin      = HwSettings->Ohci.IntPin;
    csl_hws.Ohci.IntType     = HwSettings->Ohci.IntType;
    csl_hws.Rct.BaseAddress  = HwSettings->Rct.BaseAddress;
    AmbaCSL_UsbSetHwInfo(&csl_hws);
}

UINT32 AmbaRTSL_UsbGetDevConfig(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevConfig();
    MUTEX_UNLOCK();
    return rval;
}

#if defined(CONFIG_BUILD_SYNOPSYS_USB2_DRIVER)

UINT32 AmbaRTSL_USBIsrHook(UINT32 IntID, UINT32 TriggerType, AMBA_INT_ISR_f IsrEntry)
{
    AMBA_INT_CONFIG_s pIntConfig;

    pIntConfig.TriggerType = TriggerType;
    pIntConfig.IrqType = INT_TYPE_FIQ;  /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
    pIntConfig.CpuTargets = 0x01;

    return AmbaRTSL_GicIntConfig(IntID, &pIntConfig, IsrEntry, 0);
}

UINT32 AmbaRTSL_USBIsrEnable(UINT32 IntID)
{
    return AmbaRTSL_GicIntEnable(IntID);
}

UINT32 AmbaRTSL_USBIsrDisable(UINT32 IntID)
{
    return AmbaRTSL_GicIntDisable(IntID);
}

UINT32 AmbaRTSL_USBIsIsrEnable(UINT32 IntID)
{
    AMBA_INT_INFO_s pIntInfo;
    UINT32 uRet = 0U;

    if (AmbaRTSL_GicGetIntInfo(IntID, &pIntInfo) == 0U) {
        uRet = pIntInfo.IrqEnable;
    }
    return uRet;
}

UINT32 AmbaRTSL_USBGetBaseAddress(void)
{
    return (UINT32)rtsl_usb_hws.Udc.BaseAddress;
}

UINT32 AmbaRTSL_USBReadAddress(UINT32 offset)
{
    return AmbaCSL_UsbRead32(rtsl_usb_hws.Udc.BaseAddress + offset);
}

void AmbaRTSL_USBSetEhciOCPolarity(UINT32 polarity)
{
    AmbaCSL_UsbSetEhciOCPolarity(polarity);
}

UINT32 AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_OWNER_e owner)
{
    UINT32 status = OK;

    switch (owner) {
    case USB_PHY0_DEVICE:
        AmbaCSL_RctSetUsbPhy0Select(1);
        break;
    case USB_PHY0_HOST:
        AmbaCSL_RctSetUsbPhy0Select(0);
        break;
    case USB_PHY0_UNDEF:
    default:
        status = 0xFFFFFFFFU;
        break;
    }

    return status;
}

/*
 * USB_dev_control Register
 */

void AmbaRTSL_UsbSetDevConfig(UINT32 config)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfig(config);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigSpd(UINT32 speed)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigSpd(speed);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigRwkp(UINT32 Remotewakeup)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigRwkp(Remotewakeup);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigSp(UINT32 SlefPowered)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigSp(SlefPowered);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigPyType(UINT32 PhyType)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigPyType(PhyType);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigRev(void)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigRev(1);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigHaltSts(UINT32 HaltStatus)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigHaltSts(HaltStatus);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigDynProg(UINT32 DynamicProgram)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigDynProg(DynamicProgram);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevConfigSetDesc(UINT32 SetDescriptor)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevConfigSetDesc(SetDescriptor);
    MUTEX_UNLOCK();
}

/*
 * USB_dev_control Register
 */

void AmbaRTSL_UsbSetDevCtl(UINT32 control)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtl(control);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlRde(UINT32 EnableRxDma)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlRde(EnableRxDma);
    MUTEX_UNLOCK();
}

UINT32 AmbaRTSL_UsbGetDevCtlRde(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevCtlRde();
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetDevCtlTde(UINT32 EnableTxDma)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlTde(EnableTxDma);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlBe(UINT32 BigEndian)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlBe(BigEndian);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlBren(UINT32 BurstEnable)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlBren(BurstEnable);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlMode(UINT32 UsbMode)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlMode(UsbMode);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlSd(UINT32 SoftDisconnect)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlSd(SoftDisconnect);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlCsrdone(UINT32 CsrDone)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlCsrdone(CsrDone);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlSrxflush(UINT32 RxFifoFlush)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlSrxflush(RxFifoFlush);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetDevCtlBrlen(UINT32 BurstLength)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevCtlBrlen(BurstLength);
    MUTEX_UNLOCK();
}

/*
 * USB_dev_status Register
 */

UINT32 AmbaRTSL_UsbGetDevStatusCfg(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusCfg();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDevStatusIntf(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusIntf();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDevStatusAlt(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusAlt();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDevStatusEnumSpd(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusEnumSpd();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDevStatusRfEmpty(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusRFEmpty();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDevStatusTs(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevStatusTs();
    MUTEX_UNLOCK();
    return rval;
}

/*
 * USB_DevIntStatus status/mask Register
 */

UINT32 AmbaRTSL_UsbGetDevIntStatus(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDevIntStatus;
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetDevIntStatus(UINT32 InterruptStatus)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetDevIntStatus(InterruptStatus);
    MUTEX_UNLOCK();
}

/*
 * USB_endpoint_int_status/mask
 */

UINT32 AmbaRTSL_UsbGetEpInt(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInt();
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpInt(UINT32 interrupt)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInt(interrupt);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbEnInEpInt(UINT32 endpoint)
{
    MUTEX_LOCK();
    AmbaCSL_UsbEnInEpInt(endpoint);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbEnOutEpInt(UINT32 endpoint)
{
    MUTEX_LOCK();
    AmbaCSL_UsbEnOutEpInt(endpoint);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbDisInEpInt(UINT32 endpoint)
{
    MUTEX_LOCK();
    AmbaCSL_UsbDisInEpInt(endpoint);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbDisOutEpInt(UINT32 endpoint)
{
    MUTEX_LOCK();
    AmbaCSL_UsbDisOutEpInt(endpoint);
    MUTEX_UNLOCK();
}

/*
 *  USB_end[n]_ctrl_in/out Registers
 */

UINT32 AmbaRTSL_UsbGetEpInStall(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStall(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInNAK(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInNAK(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInPollDemand(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInPollDemand(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInEndPointType(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInEndPointType(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpInCtrl(UINT32 endpoint, UINT32 control)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInCtrl(endpoint, control);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpInStall(UINT32 endpoint, UINT32 stall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInStall(endpoint, stall);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpInCtrlF(UINT32 endpoint, UINT32 FlushTxFifo)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInCtrlF(endpoint, FlushTxFifo);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpInPollDemand(UINT32 endpoint, UINT32 PollDemand)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInPollDemand(endpoint, PollDemand);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpInEndPointType(UINT32 endpoint, UINT32 type)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInEndPointType(endpoint, type);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpInNAK(UINT32 endpoint, UINT32 nak)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInNAK(endpoint, nak);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClearEpInNAK(UINT32 endpoint, UINT32 ClearNak)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClearEpInNAK(endpoint, ClearNak);
    MUTEX_UNLOCK();
}

UINT32 AmbaRTSL_UsbGetEpOutNAK(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutNAK(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpOutCtrl(UINT32 endpoint, UINT32 control)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutCtrl(endpoint, control);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutStall(UINT32 endpoint, UINT32 stall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutStall(endpoint, stall);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutType(UINT32 endpoint, UINT32 type)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutEndPointType(endpoint, type);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutNAK(UINT32 endpoint, UINT32 nak)
{
    MUTEX_LOCK();
    /* Stop DMA here to avoid "same logical endpoint" issue on UDC2.
     * DMA will start again if driver receive next IN interrupt.
     */
    if(AmbaCSL_UsbGetEpInPollDemand(endpoint) == 1U) {
        AmbaCSL_UsbSetEpInPollDemand(endpoint, 0);
    }
    AmbaCSL_UsbSetEpOutNAK(endpoint, nak);
    MUTEX_UNLOCK();
}
void AmbaRTSL_UsbClearEpOutNAK(UINT32 endpoint, UINT32 ClearNak)
{
    MUTEX_LOCK();
    /* Stop DMA here to avoid "same logical endpoint" issue on UDC2.
     * DMA will start again if driver receive next IN interrupt.
     */
    if(AmbaCSL_UsbGetEpInPollDemand(endpoint) == 1U) {
        AmbaCSL_UsbSetEpInPollDemand(endpoint, 0);
    }
    AmbaCSL_UsbClearEpOutNAK(endpoint, ClearNak);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutRxReady(UINT32 endpoint, UINT32 RxReady)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutRxReady(endpoint, RxReady);
    MUTEX_UNLOCK();
}

/*
 *  USB_end[n]_status_in/out Registers
 */

UINT32 AmbaRTSL_UsbGetEpInStatus(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatus(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInStatusBna(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatusBna(endpoint);
    MUTEX_UNLOCK();
    return rval;
}


UINT32 AmbaRTSL_UsbGetEpInStatusTdc(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatusTdc(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInStatusRcs(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatusRcs(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInStatusRss(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatusRss(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInStsTxEmpty(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInStatusTxEmpty(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpInStatus(UINT32 endpoint, UINT32 status)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInStatus(endpoint, status);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStatusIn(UINT32 endpoint, UINT32 in)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusIn(endpoint, in);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusBna(endpoint, BufferNotAvailable);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStatusTdc(UINT32 endpoint, UINT32 TxDmaComplete)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusTdc(endpoint, TxDmaComplete);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStatusRcs(UINT32 endpoint, UINT32 RxClearStall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusRcs(endpoint, RxClearStall);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStatusRss(UINT32 endpoint, UINT32 RxSetStall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusRss(endpoint, RxSetStall);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpInStsTxEmpty(UINT32 endpoint, UINT32 TxFifoEmpty)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpInStatusTxEmpty(endpoint, TxFifoEmpty);
    MUTEX_UNLOCK();
}

UINT32 AmbaRTSL_UsbGetEpOutStatus(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutStatus(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpOutStatusOut(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutStatusOut(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpOutStatusBna(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutStatusBna(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpOutStatusRcs(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutStatusRcs(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpOutStatusRss(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutStatusRss(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpOutStatus(UINT32 endpoint, UINT32 status)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutStatus(endpoint, status);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpOutStatusOut(UINT32 endpoint, UINT32 out)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpOutStatusOut(endpoint, out);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpOutStatusBna(UINT32 endpoint, UINT32 BufferNotAvailable)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpOutStatusBna(endpoint, BufferNotAvailable);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpOutStatusRcs(UINT32 endpoint, UINT32 RxClearStall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpOutStatusRcs(endpoint, RxClearStall);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbClrEpOutStatusRss(UINT32 endpoint, UINT32 RxSetStall)
{
    MUTEX_LOCK();
    AmbaCSL_UsbClrEpOutStatusRss(endpoint, RxSetStall);
    MUTEX_UNLOCK();
}


/*
 *  USB_end[n]_buffsize_in Registers
 */

UINT32 AmbaRTSL_UsbGetEpInBuffsize(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInBuffsize(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetEpInBfszSize(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInBfszSize(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpInBuffsize(UINT32 endpoint, UINT32 size)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInBfszSize(endpoint, size);
    MUTEX_UNLOCK();
}

/*
 *  USB_end[n]_packet_fm_out Registers
 */


/*
 *  USB_end[n]_max_pktsz Registers
 */

void AmbaRTSL_UsbSetEpInMaxpksz(UINT32 endpoint, UINT32 size)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInMaxpksz(endpoint, size);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutMaxpksz(UINT32 endpoint, UINT32 size)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutMaxpksz(endpoint, size);
    MUTEX_UNLOCK();
}

/*
 *  USB_end[n]_setup_buffptr Registers
 */

UINT32 AmbaRTSL_UsbGetEpOutSetupbuf(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpOutSetupbuf(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpOutSetupbuf(UINT32 endpoint, UINT32 ptr)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutSetupbuf(endpoint, ptr);
    MUTEX_UNLOCK();
}
/*
 *  USB_end[n]_desptr_out Registers
 */

UINT32 AmbaRTSL_UsbGetEpInDesptr(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEpInDesptr(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEpInDesptr(UINT32 endpoint, UINT32 ptr)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpInDesptr(endpoint, ptr);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEpOutDesptr(UINT32 endpoint, UINT32 ptr)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEpOutDesptr(endpoint, ptr);
    MUTEX_UNLOCK();
}

/*
 *  USB_udc20 Endpoint Register
 */

UINT32 AmbaRTSL_UsbGetEp20LogicalID(UINT32 endpoint)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetEp20LogicalID(endpoint);
    MUTEX_UNLOCK();
    return rval;
}

void AmbaRTSL_UsbSetEp20(UINT32 endpoint, UINT32 value)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20(endpoint, value);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20LogicalID(UINT32 endpoint, UINT32 LogicalID)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20LogicalID(endpoint, LogicalID);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20EndPointDir(UINT32 endpoint, UINT32 direction)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20EndPointDir(endpoint, direction);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20EndPointType(UINT32 endpoint, UINT32 type)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20EndPointType(endpoint, type);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20ConfigID(UINT32 endpoint, UINT32 ConfigID)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20ConfigID(endpoint, ConfigID);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20Interface(UINT32 endpoint, UINT32 InterfaceID)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20Interface(endpoint, InterfaceID);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20AltSetting(UINT32 endpoint, UINT32 alternate)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20AltSetting(endpoint, alternate);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetEp20MaxPktSize(UINT32 endpoint, UINT32 MaxPacketSize)
{
    MUTEX_LOCK();
    AmbaCSL_UsbSetEp20MaxPacketSize(endpoint, MaxPacketSize);
    MUTEX_UNLOCK();
}

UINT32 AmbaRTSL_UsbGetPhyCtrl1(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_RctGetUsbPhy0Ctrl1();
    MUTEX_UNLOCK();
    return rval;
}

UINT32 AmbaRTSL_UsbGetDpPinState(void)
{
    // CV22 doesn't have Charger Detection
#if 0
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetDpPinState();
    MUTEX_UNLOCK();
    return rval;
#else
    return 0;
#endif
}

UINT32 AmbaRTSL_UsbGetChargerDetect(void)
{
    // CV22 doesn't have Charger Detection
#if 0
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbGetChargerDetect();
    MUTEX_UNLOCK();
    return rval;
#else
    return 0;
#endif
}

void AmbaRTSL_UsbSetPhyCtrl1(UINT32 value)
{
    MUTEX_LOCK();
    AmbaCSL_RctSetUsbPhy0Ctrl1(value);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbEnaDataContactDet(void)
{
    MUTEX_LOCK();
    AmbaCSL_RctSetUsbPhy0DCDetect(1);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbEnaDataSrcVoltage(void)
{
    MUTEX_LOCK();
    AmbaCSL_RctSetUsbPhy0DSrcVolt(1);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbEnaDatDetectVoltage(void)
{
    MUTEX_LOCK();
    AmbaCSL_RctSetUsbPhy0DDetVolt(1);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbSetChargeSrcSelect(UINT32 value)
{
    MUTEX_LOCK();
    AmbaCSL_RctSetUsbPhy0ChgrSrcSel(value);
    MUTEX_UNLOCK();
}

void AmbaRTSL_UsbVbusVicConfig(UINT32 PinID, UINT32 PinType)
{
    MUTEX_LOCK();
    AmbaCSL_UsbVbusVicConfig(PinID, PinType);
    MUTEX_UNLOCK();
}

UINT32 AmbaRTSL_UsbVbusVicRawStaus(void)
{
    UINT32 rval;

    MUTEX_LOCK();
    rval = AmbaCSL_UsbVbusVicRawStaus();
    MUTEX_UNLOCK();
    return rval;
}

/*
 *  @RoutineName:: AmbaRTSL_UsbClrDevInt
 *
 *  @Description:: Clear USB device interrupt
 *
 *  @Input      ::
 *      IntID:  interrupt ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_UsbClrDevInt(AMBA_USB_DEV_INT_e IntID)
{
    switch (IntID) {

    case USB_DEV_INT_SC:
        AmbaCSL_UsbClearDevIntSc();
        break;
    case USB_DEV_INT_SI:
        AmbaCSL_UsbClearDevIntSi();
        break;
    case USB_DEV_INT_ES:
        AmbaCSL_UsbClearDevIntES();
        break;
    case USB_DEV_INT_UR:
        AmbaCSL_UsbClearDevIntUR();
        break;
    case USB_DEV_INT_US:
        AmbaCSL_UsbClearDevIntUS();
        break;
    case USB_DEV_INT_SOF:
        AmbaCSL_UsbClearDevIntSOF();
        break;
    case USB_DEV_INT_ENUM:
        AmbaCSL_UsbClearDevIntENUM();
        break;
    case USB_DEV_INT_RMTWKP:
        AmbaCSL_UsbClearDevIntRMTWKP();
        break;
    case USB_DEV_INT_ALL:
        AmbaCSL_UsbSetDevIntStatus(0x000000FF);
        break;
    default:
        // no action
        break;
    }
}

/*
 *  @RoutineName:: AmbaRTSL_UsbSetDevMaskInt
 *
 *  @Description:: Set USB device interrupt mask
 *
 *  @Input      ::
 *      IntID:  interrupt ID
 *      Mask:   masking interrupt or not
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaRTSL_UsbSetDevMaskInt(AMBA_USB_DEV_INT_e IntID, UINT32 Mask)
{
    switch (IntID) {

    case USB_DEV_INT_SC:
        AmbaCSL_UsbSetDevIntMaskSc(Mask);
        break;
    case USB_DEV_INT_SI:
        AmbaCSL_UsbSetDevIntMaskSi(Mask);
        break;
    case USB_DEV_INT_ES:
        AmbaCSL_UsbSetDevIntMaskES(Mask);
        break;
    case USB_DEV_INT_UR:
        AmbaCSL_UsbSetDevIntMaskUR(Mask);
        break;
    case USB_DEV_INT_US:
        AmbaCSL_UsbSetDevIntMaskUS(Mask);
        break;
    case USB_DEV_INT_SOF:
        AmbaCSL_UsbSetDevIntMaskSOF(Mask);
        break;
    case USB_DEV_INT_ENUM:
        AmbaCSL_UsbSetDevIntMaskENUM(Mask);
        break;
    case USB_DEV_INT_RMTWKP:
        AmbaCSL_UsbSetDevIntMaskRMTWKP(Mask);
        break;
    case USB_DEV_INT_ALL:
        AmbaCSL_UsbSetDevIntMask(Mask);
        break;
    default:
        // no action
        break;
    }
}

void AmbaRTSL_RctSetUsb0Commononn(UINT32 value)
{
    AmbaCSL_RctSetUsbPhy0Commononn(value);
}

void AmbaRTSL_RctUsbHostSoftReset(void)
{
    AmbaCSL_RctUsbHostSoftReset();
}

void AmbaRTSL_RctSetUsbPortState(USB_PORT_STATE_e PortState, USB_PORT_OP_MODE_e PortMode)
{
    if (PortState == USB_ON) {
        if (PortMode == USB_DEVICE) {
            AmbaCSL_RctSetUsbPhy0AlwaysOn();
        } else if (PortMode == USB_HOST) {
            // On CV22 host mode, both device and host phy should be power on.
            AmbaCSL_RctSetUsbPhy0AlwaysOn();
            AmbaCSL_RctSetUsbHostOn();
        } else if (PortMode == USB_DEVICE_HOST) {
            // CV22 only has one PHY
        } else {
            // do nothing here
        }
    } else if (PortState == USB_OFF) {
        if (PortMode == USB_DEVICE) {
            AmbaCSL_RctSetUsbPhy0Off();
        } else if (PortMode == USB_HOST) {
            AmbaCSL_RctSetUsbPhy0Off();
            AmbaCSL_RctSetUsbHostOff();
        } else if (PortMode == USB_DEVICE_HOST) {
            // CV22 only has one PHY
        } else {
            // do nothing here
        }
    } else {
        // do nothting here
    }
}

USB_PORT_STATE_e AmbaRTSL_RctGetUsbPort0State(void)
{
    return (AmbaCSL_RctGetUsbPhy0NvrSuspend() != 0U)? USB_ON:USB_OFF;
}

UINT32 AmbaRTSL_USBSetPhyPreEmphasis(UINT32 PhyNum, UINT32 value)
{
    UINT32 uRet = 0U;
    if (value > 3U) {
        uRet = 0xFFFFFFFFU;
    } else {
        if (PhyNum == 0U) {
            AmbaCSL_RctSetUsbPhy0TxPreEmp(value);
        } else {
            uRet = 0xFFFFFFFFU;
        }
    }
    return uRet;
}


UINT32 AmbaRTSL_USBSetPhyTxRiseTune(UINT32 PhyNum, UINT32 value)
{
    UINT32 uRet = 0U;
    if (value > 3U) {
        uRet =  0xFFFFFFFFU;
    } else {
        if (PhyNum == 0U) {
            AmbaCSL_RctSetUsbPhy0TxRiseTune(value);
        } else {
            uRet = 0xFFFFFFFFU;
        }
    }
    return uRet;
}

UINT32 AmbaRTSL_USBSetPhyTxvRefTune(UINT32 PhyNum, UINT32 value)
{
    UINT32 uRet = 0U;
    if (value > 0x0FU) {
        uRet = 0xFFFFFFFFU;
    } else {
        if (PhyNum == 0U) {
            AmbaCSL_RctSetUsbPhy0TxvRefTune(value);
        } else {
            uRet = 0xFFFFFFFFU;
        }
    }
    return uRet;
}


UINT32 AmbaRTSL_USBSetPhyTxFslsTune(UINT32 PhyNum, UINT32 value)
{
    UINT32 uRet = 0U;
    if (value > 0x0FU) {
        uRet = 0xFFFFFFFFU;
    } else {
        if (PhyNum == 0U) {
            AmbaCSL_RctSetUsbPhy0TxFslsTune(value);
        } else {
            uRet = 0xFFFFFFFFU;
        }
    }
    return uRet;
}

#ifdef ENABLE_BLD_SOFT_DISCONNECT

// Busy wait and should be called in BLD only
static void __attribute__((optimize("O0"))) AmbaRTSL_UsbBusyWaitInBld(UINT32 wait_time)
{
    UINT32 timer_id = AMBA_TIMER3;
    UINT32 apb_clock = AmbaRTSL_PllGetApbClk();
    UINT32 min_wait_clocks = (apb_clock / 1000U) * (1000U - wait_time);
    volatile UINT32 cur_clock;

    AmbaCSL_TmrSetReloadVal(timer_id, apb_clock);
    AmbaCSL_TmrSetCurrentVal(timer_id, 0);
    AmbaCSL_TmrSetEnable(timer_id, 1);
    do {
        cur_clock = AmbaCSL_TmrGetCurrentVal(timer_id);
        //AmbaNonOS_UartPrintf("\r\nclock %d", cur_clock);
    } while (cur_clock > min_wait_clocks);
    AmbaCSL_TmrSetEnable(timer_id, 0);
}

// Disconnect USB and Should be called in BLD only
// Since Rpu pull-up resistor is enabled by default,
// Host will recognize an unknown device during system bootup.
// It is not a good user experience.
// However, it requires at least 6ms to setup registers in BLD.
// Don't call it if you have boot time concern.
void AmbaRTSL_UsbSetDevCtlSdInBld(void)
{
    //AmbaNonOS_UartPrintf("\r\nDISCONNECT USB, need 6ms wait time.");
    //AmbaNonOS_UartPrintf("\r\nDon't call %s if you have boot time concern.", __func__);
    AmbaRTSL_RctSetUsbPortState(USB_ON, USB_DEVICE);
    AmbaRTSL_UsbBusyWaitInBld(3);
    AmbaRTSL_UsbSetDevCtlSd(1);
    AmbaRTSL_UsbBusyWaitInBld(3);
    AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE);
}
#endif

#endif

