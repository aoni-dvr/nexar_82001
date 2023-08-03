#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaIOUtility.h>

static void env_setup(void)
{

}

int main(void)
{
    UINT32 u32_value;

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }

    test_AmbaIOUtility_UInt32ToStr();
    test_AmbaIOUtility_UInt64ToStr();

    test_AmbaIOUtility_StringPrintUInt32();
    test_AmbaIOUtility_StringPrintUInt64();
    test_AmbaIOUtility_StringPrintStr();

    test_AmbaIOUtility_StringAppend();
    test_AmbaIOUtility_StringCompare();
    test_AmbaIOUtility_StringCopy();
    test_AmbaIOUtility_StringLength();

    test_AmbaIOUtility_RegisterMaskRead32();
    test_AmbaIOUtility_RegisterMaskWrite32();

    test_AmbaIOUtility_RegisterWrite32();
    test_AmbaIOUtility_RegisterRead32();

    test_AmbaIOUtility_PtrToU32Addr();
    test_AmbaIOUtility_PtrToU64Addr();

    test_AmbaIOUtility_U32AddrToPtr();

    test_AmbaIOUtility_Crc32();

    test_AmbaIOUtility_FdtNodeOffsetByCid();

    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
