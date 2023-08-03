#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-length"

static UINT32 idx = 100;

static bool TestStringLengthOnce(const char *pString, SIZE_t expect_length)
{
    UINT32 StrLength;
    bool bRet;

    StrLength = IO_UtilityStringLength(pString);
    if (StrLength != expect_length) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (%s, length = %d, but expect %ld)\n",
               TEST_CASE_STR,
               idx,
               pString,
               StrLength,
               expect_length);
        bRet = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d (%s, length = %d)\n",
               TEST_CASE_STR,
               idx,
               pString,
               StrLength);
        bRet = true;
    }
    return bRet;
}

void test_AmbaIOUtility_StringLength(void)
{
    TestStringLengthOnce(NULL, 0);
    TestStringLengthOnce("", 0);
    TestStringLengthOnce("12345678", 8);
    TestStringLengthOnce("12345678\n""\0""112354536""\0", 9);
    TestStringLengthOnce("12345678""\0""112354536""\0", 8);
}