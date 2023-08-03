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

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }

    test_AmbaRTSL_USBMutexFunctions();
    test_AmbaRTSL_USBSetHwInfo();
    test_AmbaCSL_USBReadWrite();

    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
