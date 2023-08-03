#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_USB.h>

#define TEST_CASE_STR "rtsl_usb_mutex"

static UINT32 flag_mutex_result = 0;

static UINT32 get_mutex(void)
{
    return flag_mutex_result;
}
static UINT32 put_mutex(void)
{
    return flag_mutex_result;
}

static RTSL_USB_Mutex_s mutex_functions = {
    NULL,
    NULL
};

void test_AmbaRTSL_USBMutexFunctions(void)
{
    UINT32 u32_value;

    u32_value = AmbaRTSL_UsbGetDevConfig();

    AmbaRTSL_USBRegisterMutex(&mutex_functions);

    u32_value = AmbaRTSL_UsbGetDevConfig();

    mutex_functions.GetMutex = get_mutex;
    mutex_functions.PutMutex = put_mutex;

    u32_value = AmbaRTSL_UsbGetDevConfig();

    flag_mutex_result = 1;

    u32_value = AmbaRTSL_UsbGetDevConfig();

}