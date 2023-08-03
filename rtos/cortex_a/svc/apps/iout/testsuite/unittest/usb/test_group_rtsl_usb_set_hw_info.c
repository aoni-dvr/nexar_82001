#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaCSL_USB.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "rtsl_usb_set_hw_info"

void test_AmbaRTSL_USBSetHwInfo(void)
{
    RTSL_USB_HW_SETTING_s hws;

    hws.Udc.BaseAddress  = 0;
    hws.Ehci.BaseAddress = 0;
    hws.Ohci.BaseAddress = 0;
    hws.Rct.BaseAddress  = 0;

    AmbaRTSL_UsbSetHwInfo(&hws);
}