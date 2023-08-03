#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "fdt-property-u64-quick"

static UINT32 idx = 100;

static void TestOnce(INT32 Offset, const char *CompatibleStr, const char *PropertyName, UINT32 Index, UINT64 ExpectResult)
{
    UINT64 uret;

    uret = IO_UtilityFDTPropertyU64Quick(Offset, CompatibleStr, PropertyName, Index);
    if (uret != ExpectResult) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (0x%llX != expect(0x%llX))\n",
               TEST_CASE_STR,
               idx,
               uret,
               ExpectResult);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_FdtPropertyU64Quick(void)
{
    TestOnce(100, "Offset456", "Property1", 0, 0x100000002); // correct property and index
    TestOnce(100, "Offset456", "Property1", 2, 0); // correct property but index overflow
    TestOnce(100, "Offset456", "PropertyNULL", 0, 0); // property doesn't exist
    TestOnce(100, "Offset456", "Property0", 0, 0); // property exist but len = 0
    TestOnce(0, "Offset456", "Property0", 0, 0); // no node with such CompatibleString

    // fail call to AmbaFDT_CheckHeader()
    flag_fdt_check_header_fail = 1;
    TestOnce(100, "Offset456", "Property1", 0, 0);
    flag_fdt_check_header_fail = 0;

    // fail call to memcpy()
    flag_memcpy_fail = 1;
    TestOnce(100, "Offset456", "Property1", 0, 0);
    flag_memcpy_fail = 0;
}