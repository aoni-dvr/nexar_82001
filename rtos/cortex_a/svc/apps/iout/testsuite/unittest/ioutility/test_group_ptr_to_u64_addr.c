#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "ptr-to-u64-addr"

static UINT32 idx = 100;

static void TestPtrToU64AddrOnce(void *Ptr, UINT64 ExpectU64Address)
{
    UINT64 u64_address;

    u64_address = IO_UtilityPtrToU64Addr(Ptr);
    if (u64_address != ExpectU64Address) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%llX != expect(0x%llX))\n",
               TEST_CASE_STR,
               idx,
               u64_address,
               ExpectU64Address);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_PtrToU64Addr(void)
{
    UINT32 u32_value = 1;
    UINT64 u64_address = (UINT64)(UINT32)&u32_value;

    TestPtrToU64AddrOnce(&u32_value, u64_address);
}