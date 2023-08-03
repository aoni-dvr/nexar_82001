#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "ptr-to-u32-addr"

static UINT32 idx = 100;

static void TestPtrToU32AddrOnce(void *Ptr, UINT32 ExpectU32Address)
{
    UINT32 u32_address;

    u32_address = IO_UtilityPtrToU32Addr(Ptr);
    if (u32_address != ExpectU32Address) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%X != expect(0x%X))\n",
               TEST_CASE_STR,
               idx,
               u32_address,
               ExpectU32Address);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_PtrToU32Addr(void)
{
    UINT32 u32_value = 1;
    UINT32 u32_address = (UINT32)&u32_value;

    TestPtrToU32AddrOnce(&u32_value, u32_address);
}