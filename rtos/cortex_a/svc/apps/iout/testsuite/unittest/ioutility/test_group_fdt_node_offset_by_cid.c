#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "fdt-node-offset-by-cid"

static UINT32 idx = 100;

static void TestOnce(INT32 Offset, const char *CompatibleStr, INT32 ExpectResult)
{
    INT32 nret;

    nret = IO_UtilityFDTNodeOffsetByCID(Offset, CompatibleStr);
    if (nret != ExpectResult) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (%d != expect(%d))\n",
               TEST_CASE_STR,
               idx,
               nret,
               ExpectResult);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TEST_CASE_STR, idx);
    }
    idx++;
}

void test_AmbaIOUtility_FdtNodeOffsetByCid(void)
{
    TestOnce(100, "OffsetNegativeOne", 0);
    TestOnce(100, "OffsetPositiveOne", 1);

    // fail call to AmbaFDT_CheckHeader()
    flag_fdt_check_header_fail = 1;
    TestOnce(100, "OffsetPositiveOne", 0);
    flag_fdt_check_header_fail = 0;
}