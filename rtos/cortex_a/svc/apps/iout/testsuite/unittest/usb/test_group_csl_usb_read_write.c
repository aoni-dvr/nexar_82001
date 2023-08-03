#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaCSL_USB.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "csl_usb_read_write"

void test_AmbaCSL_USBReadWrite(void)
{
    UINT32 value;

    AmbaCSL_UsbWrite32(0, 0);
    value = AmbaCSL_UsbRead32(0);
}