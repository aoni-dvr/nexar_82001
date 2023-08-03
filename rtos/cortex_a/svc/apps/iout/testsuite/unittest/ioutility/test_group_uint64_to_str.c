#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "uint64_to_str"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestUInt64ToStrOnce(UINT64 value, INT32 radix, char *expect_string, INT32 buffer_size)
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
        StrLength = IO_UtilityUInt64ToStr(buffer, buffer_size, value, radix);
        buffer[StrLength] = 0;
        if ((strcmp(buffer, expect_string) != 0) || (StrLength != strlen(expect_string))) {
            printf("[IO-UT-RESULT][FAILED] %s-%04d (%llu(0x%X%08X), radix %d ==> %s (len %d), but expect %s (len %d))\n",
                   TEST_CASE_STR,
                   idx,
                   value,
                   (INT32)(value >> 32),
                   (INT32)value,
                   radix,
                   buffer,
                   StrLength,
                   expect_string,
                   strlen(expect_string));
            bRet = false;
        } else {
            printf("[IO-UT-RESULT][PASSED] %s-%04d (%llu(0x%X%08X), radix %d ==> %s (len %d))\n",
                   TEST_CASE_STR,
                   idx,
                   value,
                   (INT32)(value >> 32),
                   (INT32)value,
                   radix,
                   buffer,
                   StrLength);
            bRet = true;
        }
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_UInt64ToStr(void)
{
    TestUInt64ToStrOnce(1254, 10, "1254", 0);
    TestUInt64ToStrOnce(1254, 10, "1254", 100);
    TestUInt64ToStrOnce(0x12345678ABCDEF12, 10, "1311768467750121234", 100);
    TestUInt64ToStrOnce((UINT64)-1311768467750121234, 10, "17134975605959430382", 100);
    TestUInt64ToStrOnce((UINT64)-1, 10, "18446744073709551615", 100);

    TestUInt64ToStrOnce(1254, 16, "4E6", 100);
    TestUInt64ToStrOnce(0x12345678ABCDEF12, 16, "12345678ABCDEF12", 100);
    TestUInt64ToStrOnce((UINT64)-1311768467750121234, 16, "EDCBA987543210EE", 100);
    TestUInt64ToStrOnce((UINT64)-1, 16, "FFFFFFFFFFFFFFFF", 100);

    // overflow case
    TestUInt64ToStrOnce(0x12345678ABCDEF12, 16, "EF12", 5);
}