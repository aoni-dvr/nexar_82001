#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "uint32_to_str"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestUInt32ToStrOnce(UINT32 value, INT32 radix, char *expect_string, INT32 buffer_size)
{
    UINT32 StrLength;
    bool bRet;

    if (buffer_size > MAX_BUFFER_SIZE) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (required buffer size(%d) too large (%d))\n",
               TEST_CASE_STR,
               idx,
               buffer_size,
               MAX_BUFFER_SIZE);
        bRet = false;
    } else {
        StrLength = IO_UtilityUInt32ToStr(buffer, buffer_size, value, radix);
        buffer[StrLength] = 0;
        if ((strcmp(buffer, expect_string) != 0) || (StrLength != strlen(expect_string))) {
            printf("[IO-UT-RESULT][FAILED] %s-%04d (%u(0x%X), radix %d ==> %s (len %d), but expect %s (len %d))\n",
                   TEST_CASE_STR,
                   idx,
                   value,
                   value,
                   radix,
                   buffer,
                   StrLength,
                   expect_string,
                   strlen(expect_string));
            bRet = false;
        } else {
            printf("[IO-UT-RESULT][PASSED] %s-%04d (%u(0x%X), radix %d ==> %s (len %d))\n",
                   TEST_CASE_STR,
                   idx,
                   value,
                   value,
                   radix,
                   buffer,
                   StrLength);
            bRet = true;
        }
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_UInt32ToStr(void)
{
    TestUInt32ToStrOnce(0x1, 10, "1", 0);
    TestUInt32ToStrOnce(0x12345678, 10, "305419896", 0);
    TestUInt32ToStrOnce(0x12345678, 10, "305419896", 100);
    TestUInt32ToStrOnce((UINT32)-1, 10, "4294967295", 100);
    TestUInt32ToStrOnce((UINT32)-12345678, 10, "4282621618", 100);

    TestUInt32ToStrOnce(0x12345678, 16, "12345678", 100);
    TestUInt32ToStrOnce((UINT32)-1, 16, "FFFFFFFF", 100);
    TestUInt32ToStrOnce((UINT32)-12345678, 16, "FF439EB2", 100);

    // overflow case
    TestUInt32ToStrOnce((UINT32)-12345678, 16, "9EB2", 5);

}