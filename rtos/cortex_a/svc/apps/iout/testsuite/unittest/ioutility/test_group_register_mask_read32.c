#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "register-mask-read32"

static UINT32 idx = 100;

void test_AmbaIOUtility_RegisterMaskRead32(void)
{
    UINT64 address;
    UINT32 u32_value = 0;
    UINT32 mask = 0x00FF00;
    UINT32 shift = 8;
    UINT32 expect_value = (0xFF << shift) & mask;
    UINT32 read_value;

    address = (UINT64)(UINT32)&u32_value;

    IO_UtilityRegMaskWrite32(address, 0xFF, mask, shift);

    read_value = IO_UtilityRegMaskRead32(address, mask, shift);

    if (u32_value == expect_value) {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    } else {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%X != expect(0x%X))\n",
               TEST_CASE_STR,
               idx,
               read_value,
               expect_value);
    }
}