#include <test_group.h>

void test_value_verify(UINT32 ValueToVerify, UINT32 ValueExpected, const char *TestCase, UINT32 TestID)
{
    if (ValueToVerify != ValueExpected) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (Result(0x%X) != Expect(0x%X))\n", TestCase, TestID, ValueToVerify, ValueExpected);
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d\n", TestCase, TestID);
    }
}
