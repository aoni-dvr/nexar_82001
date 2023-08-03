#ifndef __AMBATESTC_H__
#define __AMBATESTC_H__

#include <stdio.h>

enum {
        LL_SILENT = 0,
        LL_CRITICAL,
        LL_MINIMAL,
        LL_NORMAL,
        LL_VERBOSE,
        LL_DEBUG,
        LL_TOTAL
};

int execute_test_script(
        FILE *fp, 
        const char *server_addr, 
        int server_port,
        const char *username,
        const char *ipaddr);

const char* ambatest_strerror(int num);
int connect_to_server(const char *server_addr, int server_port);

int log_msg(int level, const char *fmt, ...);

#endif  //__AMBATESTC_H__
