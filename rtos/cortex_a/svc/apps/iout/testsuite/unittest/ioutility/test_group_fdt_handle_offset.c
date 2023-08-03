#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "fdt-handle-offset"

static UINT32 idx = 100;

static void TestOnce(UINT32 PHandle, INT32 ExpectResult)
{
    INT32 nret;

    nret = IO_UtilityFDTPHandleOffset(PHandle);
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

void test_AmbaIOUtility_FdtHandleOffset(void)
{
    TestOnce(201, 201);

    // fail call to AmbaFDT_CheckHeader()
    flag_fdt_check_header_fail = 1;
    TestOnce(202, 0);
    flag_fdt_check_header_fail = 0;

}