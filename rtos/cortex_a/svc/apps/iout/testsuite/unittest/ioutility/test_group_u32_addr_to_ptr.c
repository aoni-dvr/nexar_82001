#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "u32-addr-to-ptr"

static UINT32 idx = 100;

static void TestU32AddrToPtrOnce(UINT32 U32Address, void *ExpectPtr)
{
    void *ptr;

    ptr = IO_UtilityU32AddrToPtr(U32Address);
    if (ptr != ExpectPtr) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%p != expect(0x%p))\n",
               TEST_CASE_STR,
               idx,
               ptr,
               ExpectPtr);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_U32AddrToPtr(void)
{
    UINT32 u32_value = 1;
    UINT32 u32_address = (UINT32)&u32_value;

    TestU32AddrToPtrOnce(u32_address, &u32_value);
}