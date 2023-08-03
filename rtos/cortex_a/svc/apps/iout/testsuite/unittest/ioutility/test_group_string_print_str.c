#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "string-print_str"

#define MAX_BUFFER_SIZE 1024

static char buffer[MAX_BUFFER_SIZE];
static UINT32 idx = 100;

static bool TestAmbaSPrintStrOnce(char *FMTString, UINT32 BufferSize, UINT32 Argc, char *Argv[], char *expect_string)
{
    char buffer[1024];
    bool bret;
    UINT32 len = IO_UtilityStringPrintStr(buffer, BufferSize, FMTString, Argc, Argv);
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

static UINT32 sprintf_str1(char *str, const char *format, const char *arg0)
{
    UINT32 rval;
    size_t len;

    len = IO_UtilityStringLength(format) + 1;
    rval = IO_UtilityStringPrintStr(str, 100, format, 1, &arg0);

    return rval;
}


void test_AmbaIOUtility_StringPrintStr(void)
{
    char *arg[20] = {
        "1", "23456", "345678", "4AAAAACCCCC", "5BBBBAAAEC", "609812387111"
    };

    char *fmt[]= {
        "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s",
        "Arg1 = %012s, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %d",
        "Arg1 = %08u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %%%%",
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %s",
        "Arg1 = %012u, Arg2 = 0x%04X, Arg3 = %c, Arg4 = %9s, Arg5 = %p, Arg6 = %012d, %s, %s, %s",
        "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s, Arg6 = %s, Arg7 = %s, %s"
    };

    char *result[]= {
        "Arg1 = 1, Arg2 = 23456, Arg3 = 345678, Arg4 = 4AAAAACCCCC, Arg5 = 5BBBBAAAEC",
        "Arg1 =            1, Arg2 = 0x, Arg3 = , Arg4 =     23456, Arg5 = , Arg6 = ",
        "Arg1 = , Arg2 = 0x, Arg3 = , Arg4 =         1, Arg5 = , Arg6 = %%",
        "Arg1 = , Arg2 = 0x, Arg3 = , Arg4 =         1, Arg5 = , Arg6 = 23456",
        "Arg1 = , Arg2 = 0x, Arg3 = , Arg4 =         1, Arg5 = , Arg6 = , 23456, 345678, 4AAAAACCCCC",
        "Arg1 = 1, Arg2 = 23456, Arg3 = 345678, Arg4 = 4AAAAACCCCC, Arg5 = 5BBBBAAAEC, Arg6 = 609812387111, Arg7 = , ",
    };

    char tmp[255];
    {
        UINT32 rval = sprintf_str1(tmp, "%s", "ARM_ADDER");
    }
    // test NULL pointer. should be no exception
    IO_UtilityStringPrintStr(NULL, MAX_BUFFER_SIZE, "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s", 6, arg);
    IO_UtilityStringPrintStr(buffer, 0, "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s", 6, arg);
    IO_UtilityStringPrintStr(buffer, 0, "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s", 6, NULL);
    IO_UtilityStringPrintStr(buffer, MAX_BUFFER_SIZE, NULL, 6, arg);

    for (int i = 0; i < 6; i++) {
        TestAmbaSPrintStrOnce(fmt[i], MAX_BUFFER_SIZE, 6, arg, result[i]);
    }

    // overflow condition
    TestAmbaSPrintStrOnce(
        "Arg1 = %012s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s, Arg6 = %s, Arg7 = %s, %s",
        16,
        6,
        arg,
        "Arg1 =         ");

    TestAmbaSPrintStrOnce(
        "Arg1 = %012s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s, Arg6 = %s, Arg7 = %s, %s",
        29,
        6,
        arg,
        "Arg1 =            1, Arg2 = ");

    TestAmbaSPrintStrOnce(
        "Arg1 = %012s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s, Arg6 = %s, Arg7 = %s, %s",
        30,
        6,
        arg,
        "Arg1 =            1, Arg2 = 2");

    TestAmbaSPrintStrOnce(
        "Arg1 = %012s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s, Arg6 = %s, Arg7 = %s, %s",
        118,
        6,
        arg,
        "Arg1 =            1, Arg2 = 23456, Arg3 = 345678, Arg4 = 4AAAAACCCCC, Arg5 = 5BBBBAAAEC, Arg6 = 609812387111, Arg7 = ");

    // NULL pointer in argument array
    arg[2] = NULL;
    TestAmbaSPrintStrOnce(
        "Arg1 = %s, Arg2 = %s, Arg3 = %s, Arg4 = %s, Arg5 = %s",
        MAX_BUFFER_SIZE,
        5,
        arg,
        "Arg1 = 1, Arg2 = 23456, Arg3 = , Arg4 = 4AAAAACCCCC, Arg5 = 5BBBBAAAEC");

    // overflow condition
    TestAmbaSPrintStrOnce("%%", 2, 1, arg, "%");

}