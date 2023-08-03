#include "rtos_api_lib.h"


int main(int argc, char **argv)
{
    int level = 0;
    
    if (argc != 2) {
        return 0;
    }
    rtos_api_lib_get_gpio_level(atoi(argv[1]), &level);
    printf("%d", level);

    return 0;
}