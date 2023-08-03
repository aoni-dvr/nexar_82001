#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_write_mono_counter"

static void test_fail_001(void)
{
    UINT32 uret;
    UINT32 i;

    flag_otp_engine_running = 0;

    uret = AmbaRTSL_OtpMonoCounterIncrease();

    flag_otp_engine_running = 1;

    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, TEST_CASE_STR, 001);

}
static void test_good_001(void)
{
    UINT32 uret;
    UINT32 i;

    for (i = 0; i < 255; i++) {
        AmbaRTSL_OtpMonoCounterIncrease();
    }

    uret = AmbaRTSL_OtpMonoCounterIncrease();

    test_value_verify(uret, RTSL_ERR_OTP_REACH_MAX, TEST_CASE_STR, 1001);

}

void test_AmbaRTSL_OTPWriteMonoCounter(void)
{
    test_fail_001();

    test_good_001();
}