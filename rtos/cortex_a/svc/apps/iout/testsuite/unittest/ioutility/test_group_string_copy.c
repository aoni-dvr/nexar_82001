#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-print_str"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestStringCopyOnce(char *pDest, SIZE_t DestSize, const char *pSource, const char *pExpectString)
{
    bool bRet;

    IO_UtilityStringCopy(pDest, DestSize, pSource);
    if (strcmp(pDest, pExpectString) != 0) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (Expect %s, but get %s)\n",
               TEST_CASE_STR,
               idx,
               pExpectString,
               pDest);
        bRet = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d (%s)\n",
               TEST_CASE_STR,
               idx,
               pDest);
        bRet = true;
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_StringCopy(void)
{
    // test NULL pointer. should be no exception
    IO_UtilityStringCopy(NULL, 0, "AAA");
    IO_UtilityStringCopy(buffer, 0, NULL);
    IO_UtilityStringCopy(NULL, 0, NULL);
    IO_UtilityStringCopy(buffer, 0, "AAA");

    buffer[0] = '\0';
    TestStringCopyOnce(buffer, 0, "AAA", "");
    TestStringCopyOnce(buffer, 11, "01234567890", "0123456789");
    TestStringCopyOnce(buffer, 12, "01234567890", "01234567890");
    TestStringCopyOnce(buffer, MAX_BUFFER_SIZE, "01234567890", "01234567890");

    // overflow condition
    TestStringCopyOnce(buffer, 5, "01234567890", "0123");
    TestStringCopyOnce(buffer, 9, "01234567890", "01234567");
}