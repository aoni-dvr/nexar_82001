#include <test_group.h>
#include <AmbaTypes.h>

#include <AmbaReg_ScratchpadNS.h>
#include <AmbaCSL_OTP.h>
#include <AmbaReg_RCT.h>
#include <AmbaRTSL_OTP.h>

static void env_setup(void)
{

}

int main(void)
{
    UINT32 u32_value;

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }
#if 1
    AmbaCSL_OtpBaseAddressSet(0);
    u32_value = AmbaCSL_OtpCtrl1Read();
    u32_value = AmbaCSL_OtpObsvRead();
    u32_value = AmbaCSL_OtpDataRead();
    AmbaCSL_OtpCtrl1Write(0);

    AmbaRTSL_OtpMonoCounterIncrease();
    AmbaRTSL_OtpSecureBootEnable();
    AmbaRTSL_OtpJtagEfuseSet();

    test_AmbaRTSL_OTPWriteAesKey();
    test_AmbaRTSL_OTPWriteCustomerUniqueId();
    test_AmbaRTSL_OTPWritePublicKey();
    test_AmbaRTSL_OTPWriteRsvdB2();
    test_AmbaRTSL_OTPWriteRsvdDx();

    AmbaRTSL_OtpPublicKeyRevoke(0);

    test_AmbaRTSL_OTPReadAesKey();
    test_AmbaRTSL_OTPReadCustomerUniqueId();
    test_AmbaRTSL_OTPReadMonoCounter();
    test_AmbaRTSL_OTPReadPublicKey();
    test_AmbaRTSL_OTPReadRsvdB2();
    test_AmbaRTSL_OTPReadRsvdDx();
    test_AmbaRTSL_OTPReadUniqueId();
    test_AmbaRTSL_OTPRevokePublicKey();

    test_AmbaRTSL_OTPWriteMonoCounter();
#endif
    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
