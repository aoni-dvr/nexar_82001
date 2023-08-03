#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-print_uint64"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestAmbaSPrintUIntOnce(char *FMTString, UINT32 BufferSize, UINT32 Argc, UINT64 *Argv, char *expect_string)
{
    UINT32 len = IO_UtilityStringPrintUInt64(buffer, BufferSize, FMTString, Argc, Argv);
    bool bret;
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

void test_AmbaIOUtility_StringPrintUInt64(void)
{
    UINT64 arg[6] = {11, 1, 'B', 3, 4, (UINT64)-11};

    // test NULL pointer. should be no exception
    IO_UtilityStringPrintUInt64(NULL, MAX_BUFFER_SIZE, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, arg);
    IO_UtilityStringPrintUInt64(buffer, 0, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, arg);
    IO_UtilityStringPrintUInt64(buffer, 0, "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %p", 5, NULL);
    IO_UtilityStringPrintUInt64(buffer, MAX_BUFFER_SIZE, NULL, 5, arg);

    TestAmbaSPrintUIntOnce(
        "Arg1 = %d, Arg2 = 0x%X, Arg3 = %u, Arg4 = %s, Arg5 = %05p",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 11, Arg2 = 0x1, Arg3 = 66, Arg4 = (null), Arg5 = 0x0000000000000004");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %08u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %%%%",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 00000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x0000000000000004, Arg6 = %%");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %d",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x0000000000000004, Arg6 = 0");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %026d, %d, %s, %u",
        MAX_BUFFER_SIZE,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0x0000000000000004, Arg6 = 00000018446744073709551605, 0, (null), 0");

    TestAmbaSPrintUIntOnce(
        "Arg1 = %012u, Arg2 = 0x%04x, Arg3 = %c, Arg4 = %9s, Arg5 = %P, Arg6 = %i, %d, %s, %u",
        MAX_BUFFER_SIZE,
        6,
        arg,
        "Arg1 = 000000000011, Arg2 = 0x0001, Arg3 = B, Arg4 = (null), Arg5 = 0000000000000004, Arg6 = 18446744073709551605, 0, (null), 0");


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
        "img_416_0x000000000000000B.hrs");
    // test '%R'
    TestAmbaSPrintUIntOnce(
        "img_416_%R.hrs",
        1024,
        1,
        arg,
        "img_416_000000000000000B.hrs");
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

    arg[0] = 0x11;

    TestAmbaSPrintUIntOnce("%a", 10, 1, arg, "");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%p", 2, 1, arg, "0");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%p", 3, 1, arg, "0x");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%r", 2, 1, arg, "0");

    // buffer overflow
    TestAmbaSPrintUIntOnce("%r", 3, 1, arg, "0x");

    // buffer overflow
    arg[0] = 'c';
    TestAmbaSPrintUIntOnce("%c", 2, 1, arg, "c");
}