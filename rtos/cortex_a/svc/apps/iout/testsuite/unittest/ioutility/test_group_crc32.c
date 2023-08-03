#include <test_group.h>
#include <AmbaIOUtility.h>

#define TEST_CASE_STR "crc32"

#define MAX_BUFFER_SIZE 1024

static UINT32 idx = 100;

static char *test_string = "Ask Ubuntu is a question and answer site for Ubuntu users and developers. Join them; it only takes a minute: ";
static char *add_string = "Whenever digital data is stored or interfaced, data corruption might occur. Since the beginning of computer science, people have been thinking of ways to deal with this type of problem. For serial data they came up with the solution to attach a parity bit to each sent byte. This simple detection mechanism works if an odd number of bits in a byte changes, but an even number of false bits in one byte will not be detected by the parity check. To overcome this problem people have searched for mathematical sound mechanisms to detect multiple false bits.";
static char *final_string = "Ask Ubuntu is a question and answer site for Ubuntu users and developers. Join them; it only takes a minute: Whenever digital data is stored or interfaced, data corruption might occur. Since the beginning of computer science, people have been thinking of ways to deal with this type of problem. For serial data they came up with the solution to attach a parity bit to each sent byte. This simple detection mechanism works if an odd number of bits in a byte changes, but an even number of false bits in one byte will not be detected by the parity check. To overcome this problem people have searched for mathematical sound mechanisms to detect multiple false bits.";

static bool TestCrc32Once(const UINT8 *data, UINT32 size, UINT32 expect_crc32)
{
    bool bRet;

    UINT32 crc32 = IO_UtilityCrc32(data, size);
    if (expect_crc32 != crc32) {
        printf("[IO-UT-RESULT][FAILED] %s-%04d (Expect CRC32 is 0x%X, but get 0x%X)\n",
               TEST_CASE_STR,
               idx,
               expect_crc32,
               crc32);
        bRet = false;
    } else {
        printf("[IO-UT-RESULT][PASSED] %s-%04d (CRC32 is 0x%X)\n",
               TEST_CASE_STR,
               idx,
               crc32);
        bRet = true;
    }
    idx++;
    return bRet;
}

void test_AmbaIOUtility_Crc32(void)
{
    TestCrc32Once((UINT8 *)test_string, strlen(test_string), 0xDB820C5A);
    TestCrc32Once((UINT8 *)test_string, strlen(test_string)-1, 0xDB820C5A);
    TestCrc32Once((UINT8 *)test_string, strlen(test_string)-2, 0xDB820C5A);
    TestCrc32Once((UINT8 *)test_string, strlen(test_string)-3, 0xDB820C5A);
}