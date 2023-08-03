#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "rtsl_usb_port_state"

void test_AmbaRTSL_RctSetUsbPortState(void)
{
    AmbaRTSL_RctSetUsbPortState(USB_ON, USB_DEVICE);
    AmbaRTSL_RctGetUsbPort0State();
    AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE);
    AmbaRTSL_RctGetUsbPort0State();

    AmbaRTSL_RctSetUsbPortState(USB_ON, USB_HOST);
    AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_HOST);

    AmbaRTSL_RctSetUsbPortState(USB_ON, USB_DEVICE_HOST);
    AmbaRTSL_RctSetUsbPortState(USB_OFF, USB_DEVICE_HOST);

    AmbaRTSL_RctSetUsbPortState(USB_ON, 0xFF);
    AmbaRTSL_RctSetUsbPortState(USB_OFF, 0xFF);

    AmbaRTSL_RctSetUsbPortState(0xFF, USB_DEVICE_HOST);
}