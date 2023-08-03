#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_read_unique_id"

static void test_fail_001(void)
{
    UINT32 uret;

    uret = AmbaRTSL_OtpAmbaUniqueIDRead(NULL, 512);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

static void test_fail_002(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpAmbaUniqueIDRead(buffer, 5);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 2);

}

static void test_good_001(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpAmbaUniqueIDRead(buffer, 32);

    test_value_verify(uret, 0, TEST_CASE_STR, 1001);

}

void test_AmbaRTSL_OTPReadUniqueId(void)
{
    test_fail_001();
    test_fail_002();

    test_good_001();
}