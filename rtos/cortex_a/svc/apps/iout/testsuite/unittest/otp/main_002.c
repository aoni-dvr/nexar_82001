#include <test_group.h>
#include <AmbaTypes.h>

#include <AmbaReg_ScratchpadNS.h>
#include <AmbaCSL_OTP.h>
#include <AmbaReg_RCT.h>
#include <AmbaRTSL_OTP.h>

static void env_setup(void)
{

}

static void test_code_coverage_001(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_engine_running = 0;
    uret = AmbaRTSL_OtpReservedDXWrite(buffer, 512, 0);
    flag_otp_engine_running = 1;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_code_coverage", 001);

}

static void test_code_coverage_002(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_write_fail = 1; // force the write fail in hardware layer
    otp_write_fail_cnt_down = 0;
    uret = AmbaRTSL_OtpReservedDXWrite(buffer, 512, 0);
    flag_otp_write_fail = 0;
    otp_write_fail_cnt_down = 0xFFFFFFFF;
    test_value_verify(uret, RTSL_ERR_OTP_PROGRAM_FAIL, "rtsl_otp_code_coverage", 002);
}

static void test_branch_coverage_001(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    flag_otp_read_fail = 1; // force the read done fail in hardware layer
    uret = AmbaRTSL_OtpAmbaUniqueIDRead(buffer, 512);
    flag_otp_read_fail = 0;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", 001);
}

static void test_branch_coverage_002(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_write_done_fail = 1; // force the write done fail in hardware layer
    uret = AmbaRTSL_OtpAesKeyWrite(buffer, 32, 3);
    flag_otp_write_done_fail = 0;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", 002);
}

static void test_branch_coverage_003(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_engine_running = 0; // force the engine stop in hardware layer
    uret = AmbaRTSL_OtpSecureBootEnable();
    flag_otp_engine_running = 1;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", 003);
}

static void test_branch_coverage_004(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_write_fail = 1; // force the write fail in hardware layer
    otp_write_fail_cnt_down = 1;
    uret = AmbaRTSL_OtpSecureBootEnable();
    flag_otp_write_fail = 0;
    otp_write_fail_cnt_down = 0xFFFFFFFF;
    test_value_verify(uret, RTSL_ERR_OTP_PROGRAM_FAIL, "rtsl_otp_branch_coverage", 004);
}

static void test_branch_coverage_005(void)
{
    UINT32 uret;

    flag_otp_engine_running = 0; // force the engine stop in hardware layer
    uret = AmbaRTSL_OtpMonoCounterIncrease();
    flag_otp_engine_running = 1;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", 005);
}

static void test_branch_coverage_006(void)
{
    UINT32 uret;
    UINT8  buffer[512];

    memset(buffer, 0x1, 512);

    flag_otp_engine_running = 0; // force the engine stop in hardware layer
    uret = AmbaRTSL_OtpPublicKeyWrite(buffer, 512, 0);
    flag_otp_engine_running = 1;
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", 006);
}

static void test_branch_coverage_007(void)
{
    UINT32 uret;
    UINT32 key_status;
    UINT8  buffer[512];
    UINT32 idx = 70;

    memset(buffer, 0x1, 512);

    // read public key 0
    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, &key_status);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);

    // program 3 public keys
    uret = AmbaRTSL_OtpPublicKeyWrite(buffer, 512, 0);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);
    uret = AmbaRTSL_OtpPublicKeyWrite(buffer, 512, 1);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);
    uret = AmbaRTSL_OtpPublicKeyWrite(buffer, 512, 2);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);

    // revoke key 0 (fail case)
    flag_otp_engine_running = 0; // force the engine stop in hardware layer
    uret = AmbaRTSL_OtpPublicKeyRevoke(0);
    flag_otp_engine_running = 1;

    // revoke key 0 (success case)
    uret = AmbaRTSL_OtpPublicKeyRevoke(0);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);

    // revoke key 0 again(success case)
    uret = AmbaRTSL_OtpPublicKeyRevoke(0);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);

    // read public key 0 (success case)
    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, &key_status);
    test_value_verify(uret, 0, "rtsl_otp_branch_coverage", idx++);

    // read public key 0 (fail case @ 1st read)
    flag_otp_rrdy_fail     = 1;
    otp_rrdy_fail_cnt_down = 0;
    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, &key_status);
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", idx++);
    otp_rrdy_fail_cnt_down = 0xFFFFFFFF;
    flag_otp_rrdy_fail     = 0;

    // read public key 0 (fail case @ 2nd read)
    flag_otp_rrdy_fail     = 1;
    otp_rrdy_fail_cnt_down = 1;
    uret = AmbaRTSL_OtpPublicKeyRead(buffer, 512, 0, &key_status);
    test_value_verify(uret, RTSL_ERR_OTP_WAIT_TIMEOUT, "rtsl_otp_branch_coverage", idx++);
    otp_rrdy_fail_cnt_down = 0xFFFFFFFF;
    flag_otp_rrdy_fail     = 0;


}

int main(void)
{

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }

#if 1
    test_code_coverage_001();
    test_code_coverage_002();

    test_branch_coverage_001();
    test_branch_coverage_002();
    test_branch_coverage_003();
    test_branch_coverage_004();
    test_branch_coverage_005();
    test_branch_coverage_006();
#endif
    test_branch_coverage_007();

    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
