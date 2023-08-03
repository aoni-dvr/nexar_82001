#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "AmbaTest_msg.h"
#include "ambatestc.h"

#define OPTIONS "s:p:t:lh"

static const char*      server_addr = NULL;
static int              server_port = AMBATEST_CLIENT_PORT;

static void print_help(void) 
{
        printf("This is AmbaTest client, options are:\n");
        printf("    -s: test server IP address\n");
        printf("    -p: test server port number\n");
        printf("    -t: test script file\n");
        printf("    -l: list ambatest server statistics\n");
        printf("    -h: print this help message\n");
        exit(-1);
}

int main(int argc, char **argv)
{
        int c, list_statistics = 0;
        const char *script_path = NULL;

        while ((c = getopt(argc, argv, OPTIONS)) != -1) {
                switch (c) {
                case 's':
                        server_addr = optarg;
                        break;
                case 'p':
                        server_port = atoi(optarg);
                        break;
                case 't':
                        script_path = optarg;
                        break;
                case 'l':
                        list_statistics = 1;
                        break;
                default:
                        print_help();
                        break;
                }
        }

        /* check if we have server address */
        if (server_addr == NULL) {
                server_addr = "10.1.3.115";
        }
 
        if (script_path != NULL) {
                FILE *fp;
                if ((fp = fopen(script_path, "r")) == NULL) {
                        printf("Can't open test script %s!\n", script_path);
                        return -1;
                }
                execute_test_script(fp, server_addr, server_port, NULL, NULL);
                fclose(fp);
        }

        if (list_statistics) {
                static char buf[] = "[stat]";
                FILE *fp;
                fp = fmemopen(buf, sizeof(buf), "r");
                execute_test_script(fp, server_addr, server_port, NULL, NULL);
                fclose(fp);
        }

 	return 0;
}
