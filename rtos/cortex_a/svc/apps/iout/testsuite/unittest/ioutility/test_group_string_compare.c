#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-compare"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestStringCompareOnce(const char *pString1, const char *pString2, SIZE_t Size, INT32 ExpectResult)
{
    INT32 nRet;
    bool bRet;

    nRet = IO_UtilityStringCompare(pString1, pString2, Size);
    if (nRet != ExpectResult) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (str1 = %s, str2 = %s, size = %ld, Expect %d but get %d)\n",
               TEST_CASE_STR,
               idx,
               pString1, pString2, Size, ExpectResult, nRet);
        bRet = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d (str1 = %s, str2 = %s, size = %ld)\n",
               TEST_CASE_STR,
               idx,
               pString1, pString2, Size);
        bRet = true;
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_StringCompare(void)
{
    // test NULL pointer. should be no exception
    TestStringCompareOnce(NULL, "AAA", 3, -1);
    TestStringCompareOnce("BBB", NULL, 3, -1);
    TestStringCompareOnce("BBB", "AAA", 0, -1);

    //
    TestStringCompareOnce("BBB", "AAA", 3, 1);
    TestStringCompareOnce("AAA", "BBB", 3, -1);
    TestStringCompareOnce("BBBCC", "BBBCC", 5, 0);
    TestStringCompareOnce("BBBCC", "BBBCC", 100, -1);

    TestStringCompareOnce("AAACC", "AAA", 3, 0);
    TestStringCompareOnce("AAACC", "AAA", 5, -1);
    TestStringCompareOnce("AAA", "AAACC", 5, -1);
    TestStringCompareOnce("AAA", "AAACC", 3, 0);
    TestStringCompareOnce("AAACC", "AAADD", 5, -1);
}