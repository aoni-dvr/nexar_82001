#include <test_group.h>
#include <AmbaTypes.h>

#include <AmbaIOUtility.h>

static void env_setup(void)
{

}

int main(void)
{

    env_setup();

    if (sizeof(ULONG) != 4) {
        printf("sizeof(ULONG) should be 4!\n");
        return -1;
    }

    test_AmbaIOUtility_FdtPropertyU64Quick();

    //test_AmbaIOUtility_StringToU32();

    printf("done\n");

#ifdef WIN32
    getchar();
#endif

    return 0;
}
