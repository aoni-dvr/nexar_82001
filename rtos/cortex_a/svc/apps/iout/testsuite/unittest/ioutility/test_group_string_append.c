#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-append"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestStringCompareAppend(char *pDest, SIZE_t DestSize, const char *pSource, const char *pExpectString)
{
    bool bRet;

    IO_UtilityStringAppend(pDest, DestSize, pSource);
    if (strcmp(pDest, pExpectString) != 0) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (dst = %s, src = %s, size = %ld, Expect %s but get %s)\n",
               TEST_CASE_STR,
               idx,
               pDest, pSource, DestSize, pExpectString, pDest);
        bRet = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d (str1 = %s, str2 = %s, size = %ld)\n",
               TEST_CASE_STR,
               idx,
               pDest, pSource, DestSize);
        bRet = true;
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_StringAppend(void)
{
    // test NULL pointer. should be no exception
    IO_UtilityStringAppend(NULL, MAX_BUFFER_SIZE, "AAA");
    IO_UtilityStringAppend(buffer, 0, "AAA");
    IO_UtilityStringAppend(buffer, MAX_BUFFER_SIZE, NULL);

    TestStringCompareAppend(buffer, MAX_BUFFER_SIZE, "AAA", "AAA");
    TestStringCompareAppend(buffer, MAX_BUFFER_SIZE, "BBB", "AAABBB");
    TestStringCompareAppend(buffer, MAX_BUFFER_SIZE, "CCCCCCC", "AAABBBCCCCCCC");
    TestStringCompareAppend(buffer, MAX_BUFFER_SIZE, NULL, "AAABBBCCCCCCC");
    TestStringCompareAppend(buffer, MAX_BUFFER_SIZE, "", "AAABBBCCCCCCC");

    // overflow condition
    TestStringCompareAppend(buffer, 15, "DDD", "AAABBBCCCCCCCD");
    TestStringCompareAppend(buffer, 15, "EEE", "AAABBBCCCCCCCD");
    TestStringCompareAppend(buffer, 15, "ABCDEFGHIJKLMNOPQ", "AAABBBCCCCCCCD");
    TestStringCompareAppend(buffer, 10, "ABCDEFGHIJKLMNOPQ", "AAABBBCCCCCCCD");
}