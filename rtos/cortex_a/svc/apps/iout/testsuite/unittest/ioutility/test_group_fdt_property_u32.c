#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "fdt-property-u32"

static UINT32 idx = 100;

static void TestOnce(INT32 Offset, const char *PropertyName, UINT32 Index, UINT32 ExpectResult)
{
    UINT32 uret;

    uret = IO_UtilityFDTPropertyU32(Offset, PropertyName, Index);
    if (uret != ExpectResult) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%X != expect(0x%X))\n",
               TEST_CASE_STR,
               idx,
               uret,
               ExpectResult);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_FdtPropertyU32(void)
{
    TestOnce(455, "Property1", 0, 1); // correct property and index
    TestOnce(455, "Property1", 2, 0); // correct property but index overflow
    TestOnce(455, "PropertyNULL", 0, 0); // property doesn't exist
    TestOnce(455, "Property0", 0, 0); // property exist but len = 0

    // fail call to AmbaFDT_CheckHeader()
    flag_fdt_check_header_fail = 1;
    TestOnce(455, "Property1", 0, 0);
    flag_fdt_check_header_fail = 0;

    // fail call to memcpy()
    flag_memcpy_fail = 1;
    TestOnce(455, "Property1", 0, 0);
    flag_memcpy_fail = 0;
}