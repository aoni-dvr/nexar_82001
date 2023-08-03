#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_write_reserved_b2"

static void test_fail_001(void)
{
    UINT32 uret;

    uret = AmbaRTSL_OtpReservedB2Write(NULL, 512);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

static void test_fail_002(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpReservedB2Write(buffer, 5);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 2);

}

static void test_good_001(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    uret = AmbaRTSL_OtpReservedB2Write(buffer, 512);

    test_value_verify(uret, 0, TEST_CASE_STR, 1001);

}

void test_AmbaRTSL_OTPWriteRsvdB2(void)
{
    test_fail_001();
    test_fail_002();

    test_good_001();
}