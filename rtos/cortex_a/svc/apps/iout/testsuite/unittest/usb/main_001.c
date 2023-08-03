#include <test_group.h>
#include <AmbaTypes.h>

#include <AmbaReg_ScratchpadNS.h>
#include <AmbaReg_USB.h>
#include <AmbaCSL_USB.h>
#include <AmbaReg_RCT.h>
#include <AmbaRTSL_USB.h>

static void env_setup(void)
{
    RTSL_USB_HW_SETTING_s hws;

    hws.Udc.BaseAddress  = (UINT64)pAmbaUSB_Reg;
    hws.Ehci.BaseAddress = (UINT64)usb_ehci_mem;
    hws.Ohci.BaseAddress = (UINT64)usb_ohci_mem;
    hws.Rct.BaseAddress  = (UINT64)usb_rct_mem;

    AmbaRTSL_UsbSetHwInfo(&hws);

}

int main(void)
{
    UINT32 u32_value;

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }

    test_AmbaRTSL_USBIsrFunctions();
    u32_value = AmbaRTSL_USBGetBaseAddress();

    test_AmbaRTSL_USBReadAddress();
    test_AmbaRTSL_RctSetUsbPortState();

    AmbaRTSL_USBSetEhciOCPolarity(0);

    AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_DEVICE);
    AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_HOST);
    AmbaRTSL_USBSwitchPhy0Owner(USB_PHY0_UNDEF);

    AmbaRTSL_USBPhys2Virt(&u32_value);
    AmbaRTSL_USBVirt2Phys(&u32_value);

    u32_value = AmbaRTSL_UsbGetDevConfig();
    AmbaRTSL_UsbSetDevConfig(u32_value);
    AmbaRTSL_UsbSetDevConfigSpd(0);
    AmbaRTSL_UsbSetDevConfigRwkp(0);
    AmbaRTSL_UsbSetDevConfigSp(0);
    AmbaRTSL_UsbSetDevConfigPyType(0);
    AmbaRTSL_UsbSetDevConfigRev();
    AmbaRTSL_UsbSetDevConfigHaltSts(0);
    AmbaRTSL_UsbSetDevConfigDynProg(0);
    AmbaRTSL_UsbSetDevConfigSetDesc(0);

    AmbaRTSL_UsbSetDevCtl(0);
    AmbaRTSL_UsbSetDevCtlRde(0);
    u32_value = AmbaRTSL_UsbGetDevCtlRde();
    AmbaRTSL_UsbSetDevCtlTde(0);
    AmbaRTSL_UsbSetDevCtlBe(0);
    AmbaRTSL_UsbSetDevCtlBren(0);
    AmbaRTSL_UsbSetDevCtlMode(0);
    AmbaRTSL_UsbSetDevCtlSd(0);
    AmbaRTSL_UsbSetDevCtlCsrdone(0);
    AmbaRTSL_UsbSetDevCtlSrxflush(0);
    AmbaRTSL_UsbSetDevCtlBrlen(0);

    u32_value = AmbaRTSL_UsbGetDevStatusCfg();
    u32_value = AmbaRTSL_UsbGetDevStatusIntf();
    u32_value = AmbaRTSL_UsbGetDevStatusAlt();
    u32_value = AmbaRTSL_UsbGetDevStatusEnumSpd();
    u32_value = AmbaRTSL_UsbGetDevStatusRfEmpty();
    u32_value = AmbaRTSL_UsbGetDevStatusTs();

    u32_value = AmbaRTSL_UsbGetDevIntStatus();
    AmbaRTSL_UsbSetDevIntStatus(u32_value);

    u32_value = AmbaRTSL_UsbGetEpInt();
    AmbaRTSL_UsbSetEpInt(u32_value);
    AmbaRTSL_UsbEnInEpInt(0);
    AmbaRTSL_UsbEnOutEpInt(0);
    AmbaRTSL_UsbDisInEpInt(0);
    AmbaRTSL_UsbDisOutEpInt(0);

    u32_value = AmbaRTSL_UsbGetEpInStall(0);
    u32_value = AmbaRTSL_UsbGetEpInNAK(0);
    u32_value = AmbaRTSL_UsbGetEpInPollDemand(0);
    u32_value = AmbaRTSL_UsbGetEpInEndPointType(0);
    AmbaRTSL_UsbSetEpInCtrl(0, 0);
    AmbaRTSL_UsbSetEpInStall(0, 0);
    AmbaRTSL_UsbSetEpInCtrlF(0, 0);
    AmbaRTSL_UsbSetEpInPollDemand(0, 0);
    AmbaRTSL_UsbSetEpInEndPointType(0, 0);
    AmbaRTSL_UsbSetEpInNAK(0, 0);
    AmbaRTSL_UsbClearEpInNAK(0, 0);
    u32_value = AmbaRTSL_UsbGetEpOutNAK(0);
    AmbaRTSL_UsbSetEpOutCtrl(0, 0);
    AmbaRTSL_UsbSetEpOutStall(0, 0);
    AmbaRTSL_UsbSetEpOutType(0, 0);

    AmbaRTSL_UsbSetEpInPollDemand(0, 0);
    AmbaRTSL_UsbSetEpOutNAK(0, 0);
    AmbaRTSL_UsbSetEpInPollDemand(0, 1);
    AmbaRTSL_UsbSetEpOutNAK(0, 0);

    AmbaRTSL_UsbSetEpInPollDemand(0, 0);
    AmbaRTSL_UsbClearEpOutNAK(0, 0);
    AmbaRTSL_UsbSetEpInPollDemand(0, 1);
    AmbaRTSL_UsbClearEpOutNAK(0, 0);

    AmbaRTSL_UsbSetEpOutRxReady(0, 0);

    u32_value = AmbaRTSL_UsbGetEpInStatus(0);
    u32_value = AmbaRTSL_UsbGetEpInStatusBna(0);
    u32_value = AmbaRTSL_UsbGetEpInStatusTdc(0);
    u32_value = AmbaRTSL_UsbGetEpInStatusRcs(0);
    u32_value = AmbaRTSL_UsbGetEpInStatusRss(0);
    u32_value = AmbaRTSL_UsbGetEpInStsTxEmpty(0);
    AmbaRTSL_UsbSetEpInStatus(0, 0);
    AmbaRTSL_UsbClrEpInStatusIn(0, 0);
    AmbaRTSL_UsbClrEpInStatusBna(0, 0);
    AmbaRTSL_UsbClrEpInStatusTdc(0, 0);
    AmbaRTSL_UsbClrEpInStatusRcs(0, 0);
    AmbaRTSL_UsbClrEpInStatusRss(0, 0);
    AmbaRTSL_UsbClrEpInStsTxEmpty(0, 0);
    u32_value = AmbaRTSL_UsbGetEpOutStatus(0);
    u32_value = AmbaRTSL_UsbGetEpOutStatusOut(0);
    u32_value = AmbaRTSL_UsbGetEpOutStatusBna(0);
    u32_value = AmbaRTSL_UsbGetEpOutStatusRcs(0);
    u32_value = AmbaRTSL_UsbGetEpOutStatusRss(0);
    AmbaRTSL_UsbSetEpOutStatus(0, 0);
    AmbaRTSL_UsbClrEpOutStatusOut(0, 0);
    AmbaRTSL_UsbClrEpOutStatusBna(0, 0);
    AmbaRTSL_UsbClrEpOutStatusRcs(0, 0);
    AmbaRTSL_UsbClrEpOutStatusRss(0, 0);

    u32_value = AmbaRTSL_UsbGetEpInBuffsize(0);
    u32_value = AmbaRTSL_UsbGetEpInBfszSize(0);
    AmbaRTSL_UsbSetEpInBuffsize(0, 0);

    AmbaRTSL_UsbSetEpInMaxpksz(0, 0);
    AmbaRTSL_UsbSetEpOutMaxpksz(0, 0);

    u32_value = AmbaRTSL_UsbGetEpOutSetupbuf(0);
    AmbaRTSL_UsbSetEpOutSetupbuf(0, 0);

    u32_value = AmbaRTSL_UsbGetEpInDesptr(0);
    AmbaRTSL_UsbSetEpInDesptr(0, 0);
    AmbaRTSL_UsbSetEpOutDesptr(0, 0);

    u32_value = AmbaRTSL_UsbGetEp20LogicalID(0);
    AmbaRTSL_UsbSetEp20(0, 0);
    AmbaRTSL_UsbSetEp20LogicalID(0, 0);
    AmbaRTSL_UsbSetEp20EndPointDir(0, 0);
    AmbaRTSL_UsbSetEp20EndPointType(0, 0);
    AmbaRTSL_UsbSetEp20ConfigID(0, 0);
    AmbaRTSL_UsbSetEp20Interface(0, 0);
    AmbaRTSL_UsbSetEp20AltSetting(0, 0);
    AmbaRTSL_UsbSetEp20MaxPktSize(0, 0);
    u32_value = AmbaRTSL_UsbGetPhyCtrl1();
    u32_value = AmbaRTSL_UsbGetDpPinState();
    u32_value = AmbaRTSL_UsbGetChargerDetect();
    AmbaRTSL_UsbSetPhyCtrl1(0);
    AmbaRTSL_UsbEnaDataContactDet();
    AmbaRTSL_UsbEnaDataSrcVoltage();
    AmbaRTSL_UsbEnaDatDetectVoltage();
    AmbaRTSL_UsbSetChargeSrcSelect(0);
    AmbaRTSL_UsbVbusVicConfig(0, 0);
    u32_value = AmbaRTSL_UsbVbusVicRawStaus();

    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SC);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SI);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_ES);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_UR);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_US);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SOF);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_ENUM);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_RMTWKP);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_ALL);
    AmbaRTSL_UsbClrDevInt(100);

    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SC, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SI, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_ES, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_UR, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_US, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SOF, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_ENUM, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_RMTWKP, 0);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_ALL, 0);
    AmbaRTSL_UsbSetDevMaskInt(100, 0);

    AmbaRTSL_RctSetUsb0Commononn(0);
    AmbaRTSL_RctUsbHostSoftReset();
    u32_value = AmbaRTSL_USBSetPhyPreEmphasis(0, 0);
    u32_value = AmbaRTSL_USBSetPhyPreEmphasis(0, 5);
    u32_value = AmbaRTSL_USBSetPhyPreEmphasis(1, 0);
    u32_value = AmbaRTSL_USBSetPhyTxRiseTune(0, 0);
    u32_value = AmbaRTSL_USBSetPhyTxRiseTune(0, 5);
    u32_value = AmbaRTSL_USBSetPhyTxRiseTune(1, 0);
    u32_value = AmbaRTSL_USBSetPhyTxvRefTune(0, 0);
    u32_value = AmbaRTSL_USBSetPhyTxvRefTune(0, 0xFFF);
    u32_value = AmbaRTSL_USBSetPhyTxvRefTune(1, 0);
    u32_value = AmbaRTSL_USBSetPhyTxFslsTune(0, 0);
    u32_value = AmbaRTSL_USBSetPhyTxFslsTune(0, 0xFFF);
    u32_value = AmbaRTSL_USBSetPhyTxFslsTune(1, 0);

    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
