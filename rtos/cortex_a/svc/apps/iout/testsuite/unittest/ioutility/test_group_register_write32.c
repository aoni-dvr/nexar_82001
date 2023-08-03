#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "register-write32"

static UINT32 idx = 0;

static void test_good_001(void)
{

    UINT32 u32_value = 0;
    UINT64 u64_address = (UINT64)(UINT32)&u32_value;

    IO_UtilityRegWrite32(u64_address, 100);

    test_value_verify(u32_value, 100, TEST_CASE_STR, idx++);

}

static void test_fail_001(void)
{

    UINT32 u32_value = 0;
    UINT64 u64_address = (UINT64)(UINT32)&u32_value;

    flag_memcpy_fail = 1; // fail memcpy
    IO_UtilityRegWrite32(u64_address, 100);
    flag_memcpy_fail = 0;
    test_value_verify(u32_value, 0, TEST_CASE_STR, idx++);

}

void test_AmbaIOUtility_RegisterWrite32(void)
{
    test_good_001();

    test_fail_001();
}