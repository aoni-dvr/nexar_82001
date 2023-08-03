#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-to-u32"

#define MAX_BUFFER_SIZE 1024

static UINT32 idx = 100;

static char buffer[MAX_BUFFER_SIZE];

static bool TestStringToUInt32Once(const char *pString, UINT32 ExpectValue, UINT32 ExpectResult)
{
    bool bRet;
    UINT32 Result, u32Value;

    Result = IO_UtilityStringToUInt32(pString, &u32Value);

    if (Result != ExpectResult) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (%s ==> %d (0x%X), but get %d (0x%X), result %d)\n",
               TEST_CASE_STR,
               idx,
               pString,
               ExpectValue,
               ExpectValue,
               u32Value,
               u32Value,
               Result);
        bRet = false;
    } else {
        if ((u32Value != ExpectValue) && (ExpectResult == 0)) {
            printf("[IO-UT-RESULT][FAILED] %s-%04d (%s ==> %d (0x%X), but get %d (0x%X), result %d)\n",
                   TEST_CASE_STR,
                   idx,
                   pString, ExpectValue, ExpectValue, u32Value, u32Value, Result);
            bRet = false;
        } else {
            printf("[IO-UT-RESULT][PASSED] %s-%04d (%s ==> %d (0x%X), result = %d)\n",
                   TEST_CASE_STR,
                   idx,
                   pString, u32Value, u32Value, ExpectResult);
            bRet = true;
        }
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_StringToU32(void)
{
    UINT32 u32_value;
    // passing NULL
    IO_UtilityStringToUInt32(NULL, &u32_value);
    IO_UtilityStringToUInt32("123", NULL);

    TestStringToUInt32Once("0x12345678", 0x12345678, 0);
    TestStringToUInt32Once("0X12345678", 0x12345678, 0);
    TestStringToUInt32Once("0x1234567", 0x1234567, 0);
    TestStringToUInt32Once("0x123456", 0x123456, 0);
    TestStringToUInt32Once("0x12345", 0x12345, 0);
    TestStringToUInt32Once("0x1234", 0x1234, 0);
    TestStringToUInt32Once("0x123", 0x123, 0);
    TestStringToUInt32Once("0x12", 0x12, 0);
    TestStringToUInt32Once("0x1", 0x1, 0);
    TestStringToUInt32Once("0x0", 0x0, 0);
    TestStringToUInt32Once("0x", 0x0, 1);
    TestStringToUInt32Once("0x1234ABCD", 0x1234ABCD, 0);
    TestStringToUInt32Once("0x1234aBCD", 0x1234ABCD, 0);
    TestStringToUInt32Once("0x12abcdef", 0x12ABCDEF, 0);
    TestStringToUInt32Once("0x12ABCDEF", 0x12ABCDEF, 0);
    TestStringToUInt32Once("12345678", 12345678, 0);
    TestStringToUInt32Once("1234567", 1234567, 0);
    TestStringToUInt32Once("123456", 123456, 0);
    TestStringToUInt32Once("12345", 12345, 0);
    TestStringToUInt32Once("1234", 1234, 0);
    TestStringToUInt32Once("123", 123, 0);
    TestStringToUInt32Once("12", 12, 0);
    TestStringToUInt32Once("1", 1, 0);
    TestStringToUInt32Once("", 0, 1);

    // maximum number
    TestStringToUInt32Once("4294967295", 4294967295, 0);
    TestStringToUInt32Once("0xFFFFFFFF", 0xFFFFFFFF, 0);

    // error case
    TestStringToUInt32Once("0x123G4567", 12345678, 1);
    TestStringToUInt32Once(NULL, 0x12345678, 1);
    TestStringToUInt32Once("123F4567", 12345678, 1);
    TestStringToUInt32Once("0x", 0, 1);
    TestStringToUInt32Once("0x1", 1, 0);
    TestStringToUInt32Once("01", 1, 0);

    // overflow case
    TestStringToUInt32Once("0x123456789", 12345678, 1);
    TestStringToUInt32Once("0x123456789012", 12345678, 1);
    TestStringToUInt32Once("42949672951", 12345678, 1);
}