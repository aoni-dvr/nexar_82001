#include <test_group.h>
#include <AmbaKAL.h>
#include <AmbaRTSL_OTP.h>

#define TEST_CASE_STR "rtsl_otp_revoke_public_key"

static void test_fail_001(void)
{
    UINT32 uret;

    uret = AmbaRTSL_OtpPublicKeyRevoke(5);

    test_value_verify(uret, RTSL_ERR_OTP_PARAM, TEST_CASE_STR, 1);

}

void test_AmbaRTSL_OTPRevokePublicKey(void)
{
    test_fail_001();
}