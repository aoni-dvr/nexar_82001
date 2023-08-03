#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "rtsl_usb_isr"

static void usbd_isr_func(UINT32 IntID, UINT32 UserArg)
{
    return;
}

void test_AmbaRTSL_USBIsrFunctions(void)
{
    AmbaRTSL_USBIsrHook(100, 0, usbd_isr_func);
    AmbaRTSL_USBIsrEnable(100);
    AmbaRTSL_USBIsIsrEnable(100);
    AmbaRTSL_USBIsrDisable(100);
    AmbaRTSL_USBIsIsrEnable(100);
}