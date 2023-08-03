#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ota.h"

int main(int argc, char **argv)
{
    int opt;
    char *dev_name = "/dev/ttyS1";
    char *fw_dir = "/mnt/extsd";

    while ((opt = getopt(argc, argv, "d:p:h")) != -1) {
        switch (opt) {
        case 'd':
            dev_name = strdup(optarg);
            break;
        case 'p':
            fw_dir = strdup(optarg);
            break;
        case 'h':
            printf("Usage: %s -d [dev_name] -p [fw_dir]\n", argv[0]);
            exit(0);
            break;
        default:
            printf("Invalid option\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("dev_name: %s\n", dev_name);
    printf("fw_dir: %s\n", fw_dir);

    do_ota(dev_name, fw_dir);

    return 0;
}

