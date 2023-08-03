#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "register-read32"

UINT32 idx = 0;

static void test_good_001(void)
{
    UINT32 uret;

    UINT32 u32_value = 1;
    UINT64 u64_address = (UINT64)(UINT32)&u32_value;

    uret = IO_UtilityRegRead32(u64_address);

    test_value_verify(uret, 1, TEST_CASE_STR, idx++);
}

static void test_fail_001(void)
{
    UINT32 uret;

    UINT32 u32_value = 1;
    UINT64 u64_address = (UINT64)(UINT32)&u32_value;

    flag_memcpy_fail = 1; // fail memcpy
    uret = IO_UtilityRegRead32(u64_address);
    flag_memcpy_fail = 0;

    test_value_verify(uret, 0, TEST_CASE_STR, idx++);
}

void test_AmbaIOUtility_RegisterRead32(void)
{
    test_good_001();

    test_fail_001();
}