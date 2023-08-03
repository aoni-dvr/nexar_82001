#include "rtos_api_lib.h"

int main(int argc, char **argv)
{
    char sn[128] = {0};
    memset(sn, 0, sizeof(sn));
    rtos_api_lib_get_hw_sn(sn);
    printf("%s", sn);

    return 0;
}
