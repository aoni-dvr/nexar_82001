#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_read_mono_counter"

static void test_fail_001(void)
{
    UINT32 uret;

    uret = AmbaRTSL_OtpMonoCounterRead(NULL);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

static void test_good_001(void)
{
    UINT32 uret;
    UINT32 u32_value;

    uret = AmbaRTSL_OtpMonoCounterRead(&u32_value);

    test_value_verify(uret, 0, TEST_CASE_STR, 1001);

}

void test_AmbaRTSL_OTPReadMonoCounter(void)
{
    test_fail_001();

    test_good_001();
}