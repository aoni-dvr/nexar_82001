#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include "ambatestd.h"

#define OPTIONS "rh"

static void exit_handler(int signo)
{
        device_shutdown();
        client_shutdown();
        log_shutdown();
        scheduler_shutdown();
        user_shutdown();
        log_msg(LL_CRITICAL, "Exit\n");
        exit(0);
}

static void print_help(const char *exe)
{
        printf("\nusage: %s [args]\n", exe);
        printf("\t-r: restricted mode, only local client is allowed\n");
        printf("\t-h: this help\n");
}
   
int main(int argc, char **argv)
{
        int max_num_devices = 256;
        int max_num_clients = 256;
        int max_num_users = 256;
        int c, client_flags = 0;

        /* we need to clean up client/device modules before exit */
	signal(SIGTERM, exit_handler);
	signal(SIGINT,  exit_handler);

        /* handle broken pipe by ourself */
        signal(SIGPIPE, SIG_IGN);

        optind = 1;
        while ((c = getopt(argc, argv, OPTIONS)) != -1) {
                switch(c) {
                case 'r':
                        client_flags |= CLIENT_FLAG_RESTRICTED;
                        break;
                default:
                        print_help(argv[0]);
                        exit(-1);
                }
        }

        device_init(max_num_devices);
        client_init(max_num_clients, client_flags);
        user_init(max_num_users);
        scheduler_init();
        log_init();

        sleep(0xFFFFFFFF);
	return 0;
}
