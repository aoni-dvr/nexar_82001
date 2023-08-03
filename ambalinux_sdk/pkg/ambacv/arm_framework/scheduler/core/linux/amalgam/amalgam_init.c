/**
 *  @file amalgam_init.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Amalgam init APIs
 *
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "schdr_api.h"

uint32_t CVTASK_DRAM_END;

#define MAX_CONN_TRY    16
static char*    s_inet;
static int32_t  s_port;
static uint64_t s_key;
uint32_t  AMALGAM_CMA_SIZE;
uint64_t  AMALGAM_CMA_BASE;

#if 0
static void get_mac_addr(void)
{
    struct ifreq ifr, *it, *end;
    struct ifconf ifc;
    char buf[1024];;
    int sock;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    ioctl(sock, SIOCGIFCONF, &ifc);

    it = ifc.ifc_req;
    end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; it++) {
        struct sockaddr_in* ipaddr;

        strcpy(ifr.ifr_name, it->ifr_name);
        ioctl(sock, SIOCGIFADDR, &ifr);
        ipaddr = (struct sockaddr_in*)&it->ifr_addr;

        if (!strncmp(inet_ntoa(ipaddr->sin_addr), MATCHER_INET, 8)) {
            ioctl(sock, SIOCGIFHWADDR, &ifr);
            memcpy(mac_addr, ifr.ifr_hwaddr.sa_data, 6);
            return;
        }
    }
    assert(0);
}
#endif

static int32_t connect_to(char* host, int32_t port)
{
    int32_t fd, err;
    struct sockaddr_in serverAddr;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(fd >= 0);

    /* connect to amalgam port server */
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_aton(host, &serverAddr.sin_addr);
    err = connect(fd, (struct sockaddr *)&serverAddr,
                  sizeof(serverAddr));
    assert(err == 0);

    return fd;
}

int32_t amalgam_config(char *inet, int32_t port, uint64_t key,
                       uint32_t dram_size)
{
    s_inet = inet;
    s_port = port;
    s_key  = key;
    CVTASK_DRAM_END = dram_size;

    return 0;
}

int32_t amalgam_init(void)
{
    int32_t port, fd, len, loop, flag = 1;
    char buf[64], *host, *p;

    fd = connect_to(s_inet, s_port);
    for (loop = 0; loop < MAX_CONN_TRY; loop++) {
        // talk to port matcher
        len=sprintf(buf, "arm getport key=0x%lx", s_key);
        send(fd, &len, 4, 0);
        send(fd, buf, len, 0);
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, 4);
        len = recv(fd, buf, sizeof(buf)-1, 0);
        buf[len] = 0;

        // parse reply from matcher
        if ((host = strstr(buf, "port=")) && (p = strchr(host, ':'))) {
            host += 5;
            *p++ = 0;
            sscanf(p, "%d", &port);
            break;
        }

        // sleep and try again
        usleep(1000000);
    }
    close(fd);
    if (loop >= MAX_CONN_TRY) {
        printf("\nERROR: Waiting for Amalgam connection timed out.\n");
        printf("Check /cv1/work/log/pair_server.log for details.\n\n");
        exit(-1);
    }

    /* connect to amalgam */
    return connect_to(host, port);
}

static const char *standalone_short_options = "s:k:d:c:";
int32_t schdrcfg_parse_amalgam_opts(int32_t argc, char **argv)
{
    int32_t c, port = 8699;
    char *inet = "127.0.0.1";
    uint64_t key = 0xaabbccddeeff;
    uint32_t dram_size = 0x20000000;
    uint32_t cma_size  = 0x00000000;
    int32_t old_optind;
    int32_t old_opterr;

    old_opterr = opterr;
    old_optind = optind;
    opterr  = 0;
    optind  = 1;
    while ((c = getopt(argc, argv, standalone_short_options )) != -1) {
        switch (c) {
        case 'k':
            key = strtoull(optarg, NULL, 0);
            break;
        case 's':
            inet = optarg;
            break;
        case 'd':
            dram_size = strtoull(optarg, NULL, 0);
            break;
        case 'c':
            cma_size = strtoull(optarg, NULL, 0);
            break;
        default:
            break;
        }
    }
    amalgam_config(inet, port, key, dram_size);
    AMALGAM_CMA_SIZE  = cma_size;       // [TODO] : Patch this into the actual interface (doing so will affect cv_libraries)
    AMALGAM_CMA_BASE  = 0x0180000000UL; // [TODO] : Make this configurable, locate at 6GB range to avoid aliasing (to catch accidental orc-based direct reads)
    opterr  = old_opterr;
    optind  = old_optind;

    return 0;
}