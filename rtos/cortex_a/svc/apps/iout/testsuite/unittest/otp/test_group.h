#ifndef GCOVR_OTP_H
#define GCOVR_OTP_H

#if defined(WIN32) || defined(__i386__)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include <AmbaTypes.h>

extern UINT8 flag_otp_engine_running;
extern UINT8 flag_otp_write_done_fail;
extern UINT8 flag_otp_write_fail;
extern UINT8 flag_otp_read_fail;
extern UINT8 flag_otp_rrdy_fail;
extern UINT32 otp_rrdy_fail_cnt_down;
extern UINT32 otp_write_fail_cnt_down;

void test_AmbaRTSL_OTPReadAesKey(void);
void test_AmbaRTSL_OTPReadCustomerUniqueId(void);
void test_AmbaRTSL_OTPReadMonoCounter(void);
void test_AmbaRTSL_OTPReadPublicKey(void);
void test_AmbaRTSL_OTPReadRsvdB2(void);
void test_AmbaRTSL_OTPReadRsvdDx(void);
void test_AmbaRTSL_OTPReadUniqueId(void);
void test_AmbaRTSL_OTPRevokePublicKey(void);
void test_AmbaRTSL_OTPWriteAesKey(void);
void test_AmbaRTSL_OTPWriteCustomerUniqueId(void);
void test_AmbaRTSL_OTPWriteMonoCounter(void);
void test_AmbaRTSL_OTPWritePublicKey(void);
void test_AmbaRTSL_OTPWriteRsvdB2(void);
void test_AmbaRTSL_OTPWriteRsvdDx(void);

void test_value_verify(UINT32 ValueToVerify, UINT32 ValueExpected, const char *TestCase, UINT32 TestID);

#endif

