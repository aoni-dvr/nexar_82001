#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_write_aes_key"

static void test_fail_001(void)
{
    UINT32 uret;

    uret = AmbaRTSL_OtpAesKeyWrite(NULL, 512, 0);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

static void test_fail_002(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpAesKeyWrite(buffer, 5, 0);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 2);

}

static void test_fail_003(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpAesKeyWrite(buffer, 32, 5);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 3);

}

static void test_fail_004(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    flag_otp_engine_running = 0;

    uret = AmbaRTSL_OtpAesKeyWrite(buffer, 32, 0);

    flag_otp_engine_running = 1;

    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, TEST_CASE_STR, 3);

}

static void test_good_001(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    uret = AmbaRTSL_OtpAesKeyWrite(buffer, 32, 0);

    test_value_verify(uret, 0, TEST_CASE_STR, 1001);

}

void test_AmbaRTSL_OTPWriteAesKey(void)
{
    test_fail_001();
    test_fail_002();
    test_fail_003();

    test_good_001();
}