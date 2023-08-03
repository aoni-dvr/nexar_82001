/**
 *  @file AmbaIOUTDiag_CmdUsb.c
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
 *  @details USB diagnostic Command
 */

#include "AppUSB.h"
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaIOUTDiag.h>

#ifndef AMBA_USB_UVCD_H
#include "AppUSB_UVCDevice.h"
#endif

// only for debug purpose
// please DO NOT enable it in normal application flow.
//#define ENABLE_DEBUG_UDC2
//#define ENABLE_DEBUG_UDC3

#define UT_USB_NG            -1
#define UT_USB_NO_PROCESS    -2

static void USB_TestUsage(void)
{
    AppUsb_Print("USB device test command:");
    AppUsb_Print("  init_device [item] : Legacy USB test cases");
    AppUsb_Print("        msc       : start MSC device class");
    AppUsb_Print("        cdc       : start CDC-ACM device class");
    AppUsb_Print("        mtp       : start MTP device class");
    AppUsb_Print("        uvc       : start UVC device class");
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    AppUsb_Print("        cdc_msc   : start CDC + MSC device class");
    AppUsb_Print("        cdc_mtp   : start CDC + MTP device class");
    AppUsb_Print("        cdc_multi : start CDC Multi-Instance class");
#endif
    AppUsb_Print("  device_info     : display device information");
    AppUsb_Print("  mtpd add_event            : send MTP Amba-Defined event");
    AppUsb_Print("  mtpd add_event obj_add    : send MTP Object Added event");
    AppUsb_Print("  mtpd add_event obj_remove : send MTP Object Added event");
    AmbaPrint_Flush();
    AppUsb_Print("  init_host [item] [hcd] [oc] : Legacy USB test cases");
    AppUsb_Print("        [item]: msc  : start MSC host class");
    AppUsb_Print("        [hcd]:  0 - EHCI, 1 = OHCI, 2 = EHCI + OHCI");
    AppUsb_Print("        [oc]:   0 - High Active, 1 - Low Active");
    AppUsb_Print("  set_module_print [module_index] [option] : optional module log messages.");
    AppUsb_Print("       [module_index]: 0 - USB, 1 - USBX.");
    AppUsb_Print("       [option]: 0 - Disabled, 1 - Enabled.");
    AmbaPrint_Flush();
    AppUsb_Print("  debug_level [value]: Set debug level");
    AppUsb_Print("        0: error");
    AppUsb_Print("        1: info");
    AppUsb_Print("  host_si [item] : Host signal integration test");
    AppUsb_Print("       test_se0");
    AppUsb_Print("       test_j");
    AppUsb_Print("       test_k");
    AppUsb_Print("       test_packet");
    AppUsb_Print("       test_force_en");
    AppUsb_Print("       test_suspend [0/1]");
    AppUsb_Print("       test_get_desc");
    AppUsb_Print("       test_set_feature [1/2]");
    AmbaPrint_Flush();
    AppUsb_Print("  stop_device    : stop USB device system");
    AppUsb_Print("  release_device : release USB device system");
    AppUsb_Print("  stop_host      : stop USB host system");
    AppUsb_Print("  chgdet         : charger detection in device mode");
#if defined(ENABLE_DEBUG_UDC2) || defined(ENABLE_DEBUG_UDC3)
    AppUsb_Print("  device_regdump  : dump Device Controller Registers");
#endif
}

#ifdef ENABLE_DEBUG_UDC3
static void device_register_dump(void)
{
    extern void  USBCDNS3_Regdump(void (*CDNS3_PRINT_f)(const char *Str));
    USBCDNS3_Regdump(AppUsb_Print);
}

#endif

#ifdef ENABLE_DEBUG_UDC2

#include <AmbaRTSL_USB.h>
#include <AmbaCache.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaMMU.h>

// should be 64-byte width and alignment for cache operation.
typedef struct {
    UINT32 status;
    UINT32 reserved;
    UCHAR *DataPtr;
    UINT32 NextDescPtr;
    UINT32 padding[12];
} UDC_DATA_DESC_s;

// should be 64-byte width and alignment for cache operation.
typedef struct {
    UINT32 status;
    UINT32 reserved;
    UINT32 data0;
    UINT32 data1;
    UINT32 padding[12];
} UDC_SETUP_DESC_s;

static void *ptr_physical_to_virtual(const void *phys)
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
    UINT64 virt_addr = 0x0;
    UINT64 phys_addr = 0x0;
    void  *virt_ptr = NULL;
    AmbaMisra_TypeCast64(&phys_addr, &phys);

    if (AmbaMMU_Phys64ToVirt64(phys_addr, &virt_addr) != 0U) {
        // ignore error
    }
    AmbaMisra_TypeCast64(&virt_ptr, &virt_addr);
    return virt_ptr;
#endif
}

static UDC_DATA_DESC_s *uint32_to_data_desc_ptr(UINT32 PhyAddr)
{
    UDC_DATA_DESC_s *ptr_ret = NULL;
    const void *ptr_virtual, *ptr_physical;

    // (UINT32) to (void *)
    AppUsb_MemoryCopy(&ptr_physical, &PhyAddr, sizeof(UINT32 *));
    ptr_virtual = ptr_physical_to_virtual(ptr_physical);

    // (void *) to (UDC_DATA_DESC_s*)
    AppUsb_MemoryCopy(&ptr_ret, &ptr_virtual, sizeof(UDC_DATA_DESC_s *));

    return ptr_ret;
}

static UDC_SETUP_DESC_s *uint32_to_setup_desc_ptr(UINT32 PhyAddr)
{
    UDC_SETUP_DESC_s *ptr_ret = NULL;
    const void *ptr_virtual, *ptr_physical;

    // (UINT32) to (void *)
    AppUsb_MemoryCopy(&ptr_physical, &PhyAddr, sizeof(UINT32*));
    ptr_virtual = ptr_physical_to_virtual(ptr_physical);

    // (void *) to (UDC_SETUP_DESC_s *)
    AppUsb_MemoryCopy(&ptr_ret, &ptr_virtual, sizeof(UDC_SETUP_DESC_s *));

    return ptr_ret;
}


static UINT32 void_ptr_to_uint32(const void *Ptr)
{
    const void *void_ptr = Ptr;
    UINT32 u32_ret;

    AppUsb_MemoryCopy(&u32_ret, &void_ptr, sizeof(void *));

    return u32_ret;
}

static void device_register_dump(void)
{
    UINT32 i;
    UINT32 dev_ctrl, dev_cfg, dev_sts;
    USB_PHY0_OWNER_e owner;

    owner = AmbaRTSL_USBGetPhy0Owner();
    switch(owner) {
    case USB_PHY0_DEVICE:
        AppUsb_PrintUInt5("Phy0 Owner: Device", 0, 0, 0, 0, 0);
        break;
    case USB_PHY0_HOST:
        AppUsb_PrintUInt5("Phy0 Owner: Host", 0, 0, 0, 0, 0);
        break;
    case USB_PHY0_UNDEF:
        AppUsb_PrintUInt5("Phy0 Owner: Undefined", 0, 0, 0, 0, 0);
        break;
    default:
        AppUsb_PrintUInt5("Phy0 Owner: Unknown", 0, 0, 0, 0, 0);
        break;
    }

    if (AmbaRTSL_RctGetUsbPort0State() == USB_OFF) {

        AppUsb_PrintUInt5("USB PHY is suspended, no USB slave registers can be accessed.", 0, 0, 0, 0, 0);

    } else {

        dev_cfg = AmbaRTSL_UsbGetDevConfig();
        dev_ctrl = AmbaRTSL_UsbGetDevCtl();
        dev_sts = AmbaRTSL_UsbGetDevStatus();

        AppUsb_PrintUInt5("DEV Config       : 0x%X", dev_cfg, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Set Desc     : 0x%X", ((dev_cfg) >> 18U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Halt Status  : 0x%X", ((dev_cfg) >> 16U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    PHY Error Det: 0x%X", ((dev_cfg) >> 9U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Status 1     : 0x%X", ((dev_cfg) >> 8U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Status       : 0x%X", ((dev_cfg) >> 7U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    PHY Intf     : 0x%X", ((dev_cfg) >> 5U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Sync Frame   : 0x%X", ((dev_cfg) >> 4U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Self Power   : 0x%X", ((dev_cfg) >> 3U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Remote Wakeup: 0x%X", ((dev_cfg) >> 2U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Speed        : 0x%X", ((dev_cfg)) & 0x03U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV Control    : 0x%X", dev_ctrl, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Threshold Len: 0x%X", ((dev_ctrl) >> 24U) & 0x0FFU, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Burst Length : 0x%X", ((dev_ctrl) >> 16U) & 0x0FFU, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    SRX_FLUSH    : 0x%X", ((dev_ctrl) >> 14U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Device NAK   : 0x%X", ((dev_ctrl) >> 12U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Soft Disconn : 0x%X", ((dev_ctrl) >> 10U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    DMA Mode     : 0x%X", ((dev_ctrl) >> 9U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Burst Enable : 0x%X", ((dev_ctrl) >> 8U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Threshold En : 0x%X", ((dev_ctrl) >> 7U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Buffer Fill Mode   : 0x%X", ((dev_ctrl) >> 6U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Big Endian         : 0x%X", ((dev_ctrl) >> 5U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Descriptor Update  : 0x%X", ((dev_ctrl) >> 4U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    TDMA Enable  : 0x%X", ((dev_ctrl) >> 3U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    RDMA Enable  : 0x%X", ((dev_ctrl) >> 2U) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Resume signal: 0x%X", ((dev_ctrl)) & 0x01U, 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV Status     : 0x%X", dev_sts, 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Frame Number  : 0x%X", AmbaRTSL_UsbGetDevStatusTs(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Remote Wakeup : 0x%X", AmbaRTSL_UsbGetDevStatusRmwkSts(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    PHY Error     : 0x%X", AmbaRTSL_UsbGetDevStatusPhyErr(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    RXFIFO Empty  : 0x%X", AmbaRTSL_UsbGetDevStatusRfEmpty(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Enum Speed    : 0x%X", AmbaRTSL_UsbGetDevStatusEnumSpd(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Suspend       : 0x%X", AmbaRTSL_UsbGetDevStatusSusp(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Alternate idx : 0x%X", AmbaRTSL_UsbGetDevStatusAlt(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Interface idx : 0x%X", AmbaRTSL_UsbGetDevStatusIntf(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("    Config idx    : 0x%X", AmbaRTSL_UsbGetDevStatusCfg(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV Int        : 0x%X", AmbaRTSL_UsbGetDevIntStatus(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV IntMask    : 0x%X", AmbaRTSL_UsbGetDevIntMask(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV EP Int     : 0x%X", AmbaRTSL_UsbGetEpInt(), 0, 0, 0, 0);
        AppUsb_PrintUInt5("DEV EP Int Mask: 0x%X", AmbaRTSL_UsbGetidMaskInt(), 0, 0, 0, 0);
        AmbaPrint_Flush();

        AppUsb_PrintUInt5("UDC registers:", 0, 0, 0, 0, 0);
        for (i = 0; i < 11U; i++) {
            AppUsb_PrintUInt5("  [%02d] 0x%X", i, AmbaRTSL_UsbGetEp20(i), 0, 0, 0);
            AppUsb_PrintUInt5("      LogicEndpointID  : 0x%X", AmbaRTSL_UsbGetEp20LogicalID(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      EndpointDirection: 0x%X", AmbaRTSL_UsbGetEp20EndPointDir(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      EndpointType     : 0x%X", AmbaRTSL_UsbGetEp20EndPointType(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      ConfigID         : 0x%X", AmbaRTSL_UsbGetEp20ConfigID(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      InterfaceID      : 0x%X", AmbaRTSL_UsbGetEp20Interface(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      AltSetting       : 0x%X", AmbaRTSL_UsbGetEp20AltSetting(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("      MaxPacketSize    : 0x%X", AmbaRTSL_UsbGetEp20MaxPktSize(i), 0, 0, 0, 0);
            AmbaPrint_Flush();
        }

        for (i = 0; i < 6U; i++) {
            AppUsb_PrintUInt5("Endpoint In [%d]:", i, 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Control : 0x%X", AmbaRTSL_UsbGetEpInCtrl(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        Stall: 0x%X", AmbaRTSL_UsbGetEpInStall(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        NAK  : 0x%X", AmbaRTSL_UsbGetEpInNAK(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Status  : 0x%X", AmbaRTSL_UsbGetEpInStatus(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        In           : %d", AmbaRTSL_UsbGetEpInStatusIn(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        BufNotAvail  : %d", AmbaRTSL_UsbGetEpInStatusBna(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        AhbError     : %d", AmbaRTSL_UsbGetEpInStatusHe(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        TxDmaComplete: %d", AmbaRTSL_UsbGetEpInStatusTdc(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxPacketSize : %d", AmbaRTSL_UsbGetEpInStsRxpktsz(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        IsoInDone    : %d", AmbaRTSL_UsbGetEpInStsIsoInDn(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxClearStall : %d", AmbaRTSL_UsbGetEpInStatusRcs(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxSetStall   : %d", AmbaRTSL_UsbGetEpInStatusRss(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        TxEmpty: %d", AmbaRTSL_UsbGetEpInStsTxEmpty(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Buf Size: 0x%X", AmbaRTSL_UsbGetEpInBuffsize(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Max PKSZ: 0x%X", AmbaRTSL_UsbGetEpInMaxpksz(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Desc PTR: 0x%X", AmbaRTSL_UsbGetEpInDesptr(i), 0, 0, 0, 0);
            AmbaPrint_Flush();
            if (AmbaRTSL_UsbGetEpInDesptr(i) != 0U) {
                const UDC_DATA_DESC_s *desc = uint32_to_data_desc_ptr(AmbaRTSL_UsbGetEpInDesptr(i));
                if (desc != NULL) {
                    (void)AmbaCache_DataClean(void_ptr_to_uint32(desc), sizeof(UDC_DATA_DESC_s));
                    AppUsb_PrintUInt5("        status  = 0x%X", desc->status, 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        DataPtr = 0x%X", void_ptr_to_uint32(desc->DataPtr), 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        Next = 0x%X", desc->NextDescPtr, 0, 0, 0, 0);
                    AmbaPrint_Flush();
                }
            }
        }

        for (i = 0; i < 6U; i++) {
            AppUsb_PrintUInt5("Endpoint Out [%d]:", i, 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Control : 0x%X", AmbaRTSL_UsbGetEpOutCtrl(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        Stall: 0x%X", AmbaRTSL_UsbGetEpOutStall(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        NAK  : 0x%X", AmbaRTSL_UsbGetEpOutNAK(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Status  : 0x%X", AmbaRTSL_UsbGetEpOutStatus(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        Out           : %d", AmbaRTSL_UsbGetEpOutStatusOut(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        BufNotAvail   : %d", AmbaRTSL_UsbGetEpOutStatusBna(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        AhbError      : %d", AmbaRTSL_UsbGetEpOutStatusHe(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        TxDmaComplete : %d", AmbaRTSL_UsbGetEpOutStatusTdc(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxPacketSize  : %d", AmbaRTSL_UsbGetEpOutStsRxpktsz(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        IsoInDone     : %d", AmbaRTSL_UsbGetEpOutStsIsoInDn(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxClearStall  : %d", AmbaRTSL_UsbGetEpOutStatusRcs(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        RxSetStall    : %d", AmbaRTSL_UsbGetEpOutStatusRss(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        TxFifoEmpty   : %d", AmbaRTSL_UsbGetEpOutStsTxEmpty(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("        cdc_clear     : %d", AmbaRTSL_UsbGetEpOutStsCdcClear(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    PKT FM  : 0x%X", AmbaRTSL_UsbGetEpOutRpfn(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Max PKSZ: 0x%X", AmbaRTSL_UsbGetEpOutMaxpksz(i), 0, 0, 0, 0);
            AppUsb_PrintUInt5("    Desc PTR: 0x%X", AmbaRTSL_UsbGetEpOutDesptr(i), 0, 0, 0, 0);
            AmbaPrint_Flush();
            if (AmbaRTSL_UsbGetEpOutDesptr(i) != 0U) {
                const UDC_DATA_DESC_s *desc = uint32_to_data_desc_ptr(AmbaRTSL_UsbGetEpOutDesptr(i));
                if (desc != NULL) {
                    (void)AmbaCache_DataClean(void_ptr_to_uint32(desc), sizeof(UDC_DATA_DESC_s));
                    AppUsb_PrintUInt5("        status  = 0x%X", desc->status, 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        DataPtr = 0x%X", void_ptr_to_uint32(desc->DataPtr), 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        Next = 0x%X", desc->NextDescPtr, 0, 0, 0, 0);
                    (void)desc->reserved; // pass vcast check
                    (void)desc->padding[0]; // pass vcast check
                    AmbaPrint_Flush();
                }
            }
            AppUsb_PrintUInt5("    Data PTR: 0x%X", AmbaRTSL_UsbGetEpOutSetupbuf(i), 0, 0, 0, 0);
            if (AmbaRTSL_UsbGetEpOutSetupbuf(i) != 0U) {
                const UDC_SETUP_DESC_s *desc = uint32_to_setup_desc_ptr(AmbaRTSL_UsbGetEpOutSetupbuf(i));
                if (desc != NULL) {
                    AppUsb_PrintUInt5("        status  = 0x%X", desc->status, 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        Data0   = 0x%X", desc->data0, 0, 0, 0, 0);
                    AppUsb_PrintUInt5("        Data1   = 0x%X", desc->data1, 0, 0, 0, 0);
                    (void)desc->reserved; // pass vcast check
                    (void)desc->padding[0]; // pass vcast check
                    AmbaPrint_Flush();
                }
            }
        }
    }
}
#endif

static INT32 test_init_device(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    (void)Argc;

    if (AmbaUtility_StringCompare(Argv[2], "msc", 3) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_MSC);
        AppMscd_SetInfo();
        AppUsbd_ClassStart();

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (AmbaUtility_StringCompare(Argv[2], "cdc_msc", 7) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_CDC_MSC);
        (void)AppCdcMscd_Start();
        AppUsbd_ClassStart();
    } else if (AmbaUtility_StringCompare(Argv[2], "cdc_mtp", 7) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_CDC_MTP);
        (void)AppCdcMtpd_Start();
        AppUsbd_ClassStart();
    } else if (AmbaUtility_StringCompare(Argv[2], "cdc_multi", 9) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_CDC_ACM_MULTI);
        (void)AppCdcAcmMultid_Start();
        AppUsbd_ClassStart();
#endif
    } else if (AmbaUtility_StringCompare(Argv[2], "cdc", 3) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_CDC_ACM);
        (void)AppCdcAcmd_Start();
        AppUsbd_ClassStart();
    } else if (AmbaUtility_StringCompare(Argv[2], "mtp", 3) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_MTP);
        (void)AppMtpd_Init();
        AppUsbd_ClassStart();
    } else if (AmbaUtility_StringCompare(Argv[2], "uvc", 3) == 0) {
        (VOID)AppUsbd_SysInit();
        AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_UVC);
        AppUvcd_Start();
        AppUsbd_ClassStart();
    } else {
        nret = UT_USB_NG;
    }

    return nret;
}

static INT32 test_module_print_enable(UINT32 Argc, char * const * Argv)
{
    UINT32 module_index, option;
    UINT32 module_id = 0U;

    (void)Argc;

    AppUsb_StringToUINT32(Argv[2], &module_index);
    AppUsb_StringToUINT32(Argv[3], &option);

    if (module_index == 0U) {
        UINT32 uret;
        module_id = (USB_ERR_BASE >> 16U) & 0x0000FFFFU;
        uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, (UINT8)option);
        if (uret != 0U) {
            AppUsb_PrintUInt5("Can't set allow list for USB module, code 0x%X.", uret, 0, 0, 0, 0);
        }
    } else if (module_index == 1U) {
        UINT32 uret;
        module_id = (USBX_ERR_BASE >> 16U) & 0x0000FFFFU;
        uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, (UINT8)option);
        if (uret != 0U) {
            AppUsb_PrintUInt5("Can't set allow list for USBX module, code 0x%X.", uret, 0, 0, 0, 0);
        }
    } else {
        AppUsb_PrintUInt5("unknown module index = %d", module_index, 0, 0, 0, 0);
    }
    return 0;
}

static INT32 test_debug_level_set(UINT32 Argc, char * const * Argv)
{
    UINT32 level = 0;

    (void)Argc;

    AppUsb_StringToUINT32(Argv[2], &level);

    AppUsb_PrintUInt5("set debug level = %d", level, 0, 0, 0, 0);

    AppUsb_DebugLevelSet(level);

    return 0;
}

static void test_host_si(const char *Command, const char *Arg)
{
    UINT32 test_index = 0;
    if (AmbaUtility_StringCompare(Command, "test_j", 6) == 0) {
        test_index = 1;
    } else if (AmbaUtility_StringCompare(Command, "test_k", 6) == 0) {
        test_index = 2;
    } else if (AmbaUtility_StringCompare(Command, "test_se0", 8) == 0) {
        test_index = 3;
    } else if (AmbaUtility_StringCompare(Command, "test_packet", 11) == 0) {
        test_index = 4;
    } else if (AmbaUtility_StringCompare(Command, "test_force_en", 13) == 0) {
        test_index = 5;
    } else if (AmbaUtility_StringCompare(Command, "test_suspend", 12) == 0) {
        UINT32 flag_enable = 0;
        AppUsb_StringToUINT32(Arg, &flag_enable);
        if (flag_enable == 1U) {
            test_index = 6;
        } else {
            test_index = 7;
        }
    } else if (AmbaUtility_StringCompare(Command, "test_get_desc", 13) == 0) {
        test_index = 8;
    } else if (AmbaUtility_StringCompare(Command, "test_set_feature", 16) == 0) {
        UINT32 step = 0;
        AppUsb_StringToUINT32(Arg, &step);
        if (step == 1U) {
            test_index = 9;
        } else if (step == 2U) {
            test_index = 10;
        } else {
            AppUsb_PrintUInt5("set feature step should be 1/2", 0, 0, 0, 0, 0);
        }
    } else {
        AppUsb_PrintStr5("Unknown argument %s", Command, NULL, NULL, NULL, NULL);
    }
    if (test_index != 0U) {
        AppUsb_PrintUInt5("set Host test mode to %d", test_index, 0, 0, 0, 0);
        AppUsbh_TestModeSet(test_index);
    }
}

static INT32 testcmd_init_host(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    (void)Argc;

    if (AmbaUtility_StringCompare(Argv[1], "init_host", 9) == 0) {
        if (AmbaUtility_StringCompare(Argv[2], "msc", 3) == 0) {
            UINT32 oc_polarity = 0;
            UINT32 hcd_sel = 0;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            hcd_sel = 3; // XHCI
#endif

            // check if the hcd_sel could be set from user input
            if (Argc >= 4U) {
                AppUsb_StringToUINT32(Argv[3], &hcd_sel);
            }

            // check if the oc_polarity could be set from user input
            if (Argc >= 5U) {
                AppUsb_StringToUINT32(Argv[4], &oc_polarity);
            }

            AppMsch_Start((UINT8)hcd_sel, (UINT8)oc_polarity, 1);
        } else {
            nret = UT_USB_NG;
        }
    } else {
        nret = UT_USB_NO_PROCESS;
    }
    return nret;
}

static void test_device_info(void)
{
    AppUsbd_VersionPrint();
    AppUsbd_VbusStatusPrint();
    AppUsbd_EnumSpeedPrint();
    AppUsbd_IsConfiguredPrint();
    return;
}

static void stop_app_device_class(void)
{
   APPUSBD_CLASS_e usb_dev_class = AppUsbd_CurrentClassIDGet();
   if (usb_dev_class == APPUSBD_CLASS_CDC_ACM) {
       AppCdcAcmd_Stop();
   }
}

static INT32 testcmd_parsecmd_1(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    (void)Argc;

    if (AmbaUtility_StringCompare(Argv[1], "mtpd", 4) == 0) {
        if (AmbaUtility_StringCompare(Argv[2], "add_event", 9) == 0) {
            if (Argc == 3U) {
                AppMtpd_AddEventTest(1);
            } else if (Argc == 4U) {
                if (AmbaUtility_StringCompare(Argv[3], "obj_add", 7) == 0) {
                    AppMtpd_AddEventObjectAdded();
                } else if (AmbaUtility_StringCompare(Argv[3], "obj_remove", 10) == 0) {
                    AppMtpd_AddEventObjectRemoved();
                } else {
                    nret = UT_USB_NG;
                }
            } else {
                nret = UT_USB_NG;
            }
        } else {
            nret = UT_USB_NG;
        }
    } else if (AmbaUtility_StringCompare(Argv[1], "stop_device", 11) == 0) {
        AppUsbd_ClassStop();
    } else if (AmbaUtility_StringCompare(Argv[1], "release_device", 14) == 0) {
        stop_app_device_class();
        AppUsbd_BruteRelease();
    } else if (AmbaUtility_StringCompare(Argv[1], "stop_host", 9) == 0) {
        AppUsbh_Stop();
    } else if (AmbaUtility_StringCompare(Argv[1], "chgdet", 6) == 0) {
        AppUsbd_ChargerDet(0, 20);
#if defined(ENABLE_DEBUG_UDC2) || defined(ENABLE_DEBUG_UDC3)
    } else if (AmbaUtility_StringCompare(Argv[1], "device_regdump", 14) == 0) {
        device_register_dump();
#endif
    } else {
        nret = UT_USB_NO_PROCESS;
    }
    return nret;
}

static void AppUsb_TestCmd(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    if (Argc >= 2U) {
        if (AmbaUtility_StringCompare(Argv[1], "init_device", 11) == 0) {
            nret = test_init_device(Argc, Argv);
        } else if (AmbaUtility_StringCompare(Argv[1], "device_info", 11) == 0) {
            test_device_info();
        } else if (AmbaUtility_StringCompare(Argv[1], "host_si", 7) == 0) {
            test_host_si(Argv[2], Argv[3]);
        } else if (AmbaUtility_StringCompare(Argv[1], "set_module_print", 16) == 0) {
            nret = test_module_print_enable(Argc, Argv);
        } else if (AmbaUtility_StringCompare(Argv[1], "debug_level", 11) == 0) {
            nret = test_debug_level_set(Argc, Argv);
        } else {
            nret = UT_USB_NO_PROCESS;
        }
    } else {
        nret = UT_USB_NG;
    }

    if (nret == UT_USB_NO_PROCESS) {
        nret = testcmd_init_host(Argc, Argv);
    }

    if (nret == UT_USB_NO_PROCESS) {
        nret = testcmd_parsecmd_1(Argc, Argv);
    }

    if (nret != 0) {
        USB_TestUsage();
    }

    return;
}

void AmbaIOUTDiag_CmdUsb(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (VOID)PrintFunc;
    AppUsb_TestCmd(ArgCount, pArgVector);
}
