#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaCSL_USB.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "rtsl_usb_read_address"

void test_AmbaRTSL_USBReadAddress(void)
{
    UINT32 value = AmbaRTSL_USBReadAddress(0);
}