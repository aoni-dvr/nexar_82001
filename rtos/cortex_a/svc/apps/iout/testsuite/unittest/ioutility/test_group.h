#ifndef GCOVR_OTP_H
#define GCOVR_OTP_H

#if defined(WIN32) || defined(__i386__)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif
#include <stdbool.h>

#include <AmbaTypes.h>

extern UINT32 flag_memcpy_fail;
extern UINT32 flag_fdt_check_header_fail;

void test_AmbaIOUtility_Crc32(void);
void test_AmbaIOUtility_PtrToU32Addr(void);
void test_AmbaIOUtility_PtrToU64Addr(void);
void test_AmbaIOUtility_RegisterMaskRead32(void);
void test_AmbaIOUtility_RegisterMaskWrite32(void);
void test_AmbaIOUtility_RegisterWrite32(void);
void test_AmbaIOUtility_RegisterRead32(void);
void test_AmbaIOUtility_StringAppend(void);
void test_AmbaIOUtility_StringCompare(void);
void test_AmbaIOUtility_StringCopy(void);
void test_AmbaIOUtility_StringLength(void);
void test_AmbaIOUtility_StringToU32(void);
void test_AmbaIOUtility_U32AddrToPtr(void);
void test_AmbaIOUtility_UInt32ToStr(void);
void test_AmbaIOUtility_UInt64ToStr(void);
void test_AmbaIOUtility_StringPrintStr(void);
void test_AmbaIOUtility_StringPrintUInt32(void);
void test_AmbaIOUtility_StringPrintUInt64(void);

void test_AmbaIOUtility_FdtNodeOffsetByCid(void);
void test_AmbaIOUtility_FdtHandleOffset(void);
void test_AmbaIOUtility_FdtPropertyU32(void);
void test_AmbaIOUtility_FdtPropertyU64Quick(void);
void test_AmbaIOUtility_FdtPropertyU32Quick(void);

void test_value_verify(UINT32 ValueToVerify, UINT32 ValueExpected, const char *TestCase, UINT32 TestID);

#endif

