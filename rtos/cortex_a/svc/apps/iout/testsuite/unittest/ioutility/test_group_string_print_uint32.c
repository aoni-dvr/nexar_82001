#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-print_uint32"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestAmbaSPrintUIntOnce(char *FMTString, UINT32 BufferSize, UINT32 Argc, UINT32 *Argv, char *expect_string)
{
    bool bret;
    UINT32 len = IO_UtilityStringPrintUInt32(buffer, BufferSize, FMTString, Argc, Argv);
    if ((strcmp(buffer, expect_string) != 0) || (len != strlen(expect_string))) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d ('%s' != '%s' or len %d != expect len %d)\n",
               TEST_CASE_STR,
               idx,
               buffer,
               expect_string,
               len,
               strlen(expect_string));
        bret = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d ('%s', len %d)\n",
               TEST_CASE_STR,
               idx,
               buffer,
               len);
        bret = true;
    }
    idx++;
    return bret;
}

void test_AmbaIOUtility_StringPrintUInt32(void)
{
    UINT32 arg[6] = {11, 1, 'B', 3, 4, (UINT32)-11};

    // test NULL pointer. should be no exception
    IO_UtilityStringPrintUInt32(NULL, MAX_BUFFER_SIZE, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, arg);
    IO_UtilityStringPrintUInt32(buffer, 0, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, arg);
    IO_UtilityStringPrintUInt32(buffer, 0, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, NULL);
    IO_UtilityStringPrintUInt32(buffer, MAX_BUFFER_SIZE, NULL, 5, arg);

    TestAmbaSPrintUIntOnce(
        "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 11, Arg2 = 0x1, Arg3 = 66, Arg4 = (null), Arg5 = 0x00000004");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %08u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %%%%",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 00000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x00000004, Arg6 = %%");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %d",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x00000004, Arg6 = 0");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04x, Arg3 = %c, Arg4 = %9s, Arg5 = %P, Arg6 = %012d, %i, %s, %u",
        MAX_BUFFER_SIZE,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 00000004, Arg6 = 004294967285, 0, (null), 0");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %d, %d, %s, %u",
        MAX_BUFFER_SIZE,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x00000004, Arg6 = 4294967285, 0, (null), 0");


    TestAmbaSPrintUIntOnce(
        "img_416_%3d.hrs",
        1024,
        1,
        arg,
        "img_416_011.hrs");

    TestAmbaSPrintUIntOnce(
        "img_416_%3u.hrs",
        1024,
        1,
        arg,
        "img_416_011.hrs");

    // test '%r'
    TestAmbaSPrintUIntOnce(
        "img_416_%r.hrs",
        1024,
        1,
        arg,
        "img_416_0x0000000B.hrs");
    // test '%R'
    TestAmbaSPrintUIntOnce(
        "img_416_%R.hrs",
        1024,
        1,
        arg,
        "img_416_0000000B.hrs");
    // test '%o'
    TestAmbaSPrintUIntOnce(
        "img_416_%o.hrs",
        1024,
        1,
        arg,
        "img_416_11.hrs");
    // test '%b'
    TestAmbaSPrintUIntOnce(
        "img_416_%b.hrs",
        1024,
        1,
        arg,
        "img_416_11.hrs");
    // test '%ld"
    TestAmbaSPrintUIntOnce(
        "img_416_%ld.hrs",
        1024,
        1,
        arg,
        "img_416_11.hrs");
    // test '%lld"
    TestAmbaSPrintUIntOnce(
        "img_416_%lld.hrs",
        1024,
        1,
        arg,
        "img_416_11.hrs");

    // overflow condition
    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %d, %s, %u",
        17,
        6,
        arg,
        "Arg1 = 000000000");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %d, %s, %u",
        19,
        6,
        arg,
        "Arg1 = 00000000001");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %d, %s, %u",
        30,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %d, %s, %u",
        31,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %d, %s, %u",
        57,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (nu");

    // test minus values
    arg[0] = 0xFFFFFFF4;
    TestAmbaSPrintUIntOnce(
        "img_416_%3d.hrs",
        1024,
        1,
        arg,
        "img_416_4294967284.hrs");

    TestAmbaSPrintUIntOnce(
        "img_416_%3u.hrs",
        1024,
        1,
        arg,
        "img_416_4294967284.hrs");

    arg[0] = 11;

    TestAmbaSPrintUIntOnce("%a", 10, 1, arg, "");

    TestAmbaSPrintUIntOnce("%1.0d", 10, 1, arg, "11");

    TestAmbaSPrintUIntOnce("12345", 10, 1, arg, "12345");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%%", 2, 1, arg, "%");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%p", 2, 1, arg, "0");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%p", 3, 1, arg, "0x");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%r", 2, 1, arg, "0");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%r", 3, 1, arg, "0x");

    TestAmbaSPrintUIntOnce("%42949672951d", 100, 1, arg, "11");

    // buffer overflow
    arg[0] = 'c';
    TestAmbaSPrintUIntOnce("%c", 2, 1, arg, "c");


}