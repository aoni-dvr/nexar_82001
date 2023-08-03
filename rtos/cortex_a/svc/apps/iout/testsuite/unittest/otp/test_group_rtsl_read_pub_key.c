#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_read_public_key"

static void test_fail_001(void)
{
    UINT32 uret;
    UINT32 key_status;

    uret = AmbaRTSL_OtpPublicKeyRead(NULL, 512, 0, &key_status);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

static void test_fail_002(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 100, 0, &key_status);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 2);

}

static void test_fail_003(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 100, &key_status);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 3);

}

static void test_fail_004(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, NULL);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 4);

}

static void test_good_001(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, &key_status);

    test_value_verify(uret, 0, TEST_CASE_STR, 1001);

}

static void test_good_002(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 1, &key_status);

    test_value_verify(uret, 0, TEST_CASE_STR, 1002);

}

static void test_good_003(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];

    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 2, &key_status);

    test_value_verify(uret, 0, TEST_CASE_STR, 1003);

}

void test_AmbaRTSL_OTPReadPublicKey(void)
{
    test_fail_001();
    test_fail_002();
    test_fail_003();
    test_fail_004();

    test_good_001();
    test_good_002();
    test_good_003();
}