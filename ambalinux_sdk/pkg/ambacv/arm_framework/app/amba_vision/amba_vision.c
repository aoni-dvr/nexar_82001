/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "schdr_api.h"

#define DEFAULT_KEY     0xaabbccddeeff
#define DEFAULT_INET    "127.0.0.1"
#define DEFAULT_PORT    8699
#define OPTIONS         "k:s:t:f:b:d:o:hp:"

static int boot_flag = 1;
static sem_t done_sem;
static int secret_is_available = 0;
static uint8_t arm_secret[64];

static void print_help(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-b arg: set memory mode 0[malloc]/1[mmap]/2[sharedmem]\n");
    printf("\t-s arg: pairing server inet address\n");
    printf("\t-k arg: 64bit pairing key\n");
    printf("\t-d arg: set dram size\n");
    printf("\t-t arg: cvtable file name\n");
    printf("\t-f arg: system flow file name\n");
    printf("\t-o arg: number of sub-schedulers, default 1\n");
    printf("\t-p arg: ARM secret file name\n");
    printf("\nfor example: %s ", exe);
    printf("-s 10.1.11.16 -k 0x1234567890\n\n");
}

static int load_binary(char *buff, int max_size, void *token)
{
    const char *name = (const char *)token;
    FILE *ifp;
    int size;

    ifp = fopen(name, "rb");
    if (ifp == NULL) {
        printf("amba_vision: (fatal) Can't open input file %s!\n", name);
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }
    if (fseek(ifp, 0, SEEK_END)) {
        printf("amba_vision: (fatal) Can't read file %s!\n", name);
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }
    size = ftell(ifp);

    if (buff != NULL) {
        if (size > max_size) {
            printf("amba_vision: (fatal) The provided buff too small for %s!\n", name);
            exit(EXIT_FAILURE);
        }
        if (fseek(ifp, 0, SEEK_SET)) {
            printf("amba_vision: (fatal) Can't read file %s!\n", name);
            perror("Message from perror");
            exit(EXIT_FAILURE);
        }
        if(size != fread(buff, 1, size, ifp)){
            printf("amba_vision: fread fail %s size %d!\n", name, size);
        }
    }

    if (fclose(ifp)) {
        printf("amba_vision: (fatal) Can't close file %s!\n", name);
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }
    return size;
}

static void add_cvtable(char *path)
{
    static int cvtable_cnt;
    if (++cvtable_cnt > MAX_CVTABLE_COUNT) {
        printf("amba_vision: (error) Too many cvtables! limit is %d\n", MAX_CVTABLE_COUNT);
        kill(0, SIGINT);
    }
    if (is_err(schdr_add_cvtable((void*) path, load_binary))) {
        printf("amba_vision: (error) Cannot add cvtable!\n");
        kill(0, SIGINT);
    }
}

static void add_sysflow(char *path)
{
    static int sysflow_cnt;
    if (++sysflow_cnt > MAX_SYSFLOW_COUNT) {
        printf("amba_vision: (error) Too many sysflow! limit is %d\n", MAX_SYSFLOW_COUNT);
        kill(0, SIGINT);
    }
    if (is_err(schdr_add_sysflow((void*)path, load_binary))) {
        printf("amba_vision: (error) Cannot add sysflow!\n");
        kill(0, SIGINT);
    }
}

static void read_secret(char *path) {
    FILE *secret_fp = fopen(path, "rb");
    off_t file_size;
    size_t bytes_read;

    if (secret_fp == NULL) {
        printf("amba_vision: (error) Cannot read from %s.\n", path);
        perror("Message from perror");
        fclose(secret_fp);
        exit(EXIT_FAILURE);
    }

    file_size = lseek(fileno(secret_fp), 0, SEEK_END);
    if (file_size == sizeof(arm_secret)) {
        lseek(fileno(secret_fp), 0, SEEK_SET);
        fread(arm_secret, sizeof(arm_secret[0]), sizeof(arm_secret), secret_fp);
    }
    else {
        printf("amba_vision: (error) Expected %lu bytes but got %ld bytes "
            "instead in %s.\n", sizeof(arm_secret), file_size, path);
        exit(EXIT_FAILURE);
    }

    if (fclose(secret_fp)) {
        printf("amba_vision: (error) Cannot close file %s!\n", path);
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }

    secret_is_available = 1;
    return;
}

static void parse_amalgam_opt(int argc, char**argv)
{
    int c, port = DEFAULT_PORT;
    char *inet = DEFAULT_INET;
    long long unsigned int key = DEFAULT_KEY;
    unsigned int dram_size = 0x20000000;
#if defined(USE_CVAPI_V7)
    unsigned int cma_size       = 0x00000000;
    unsigned int idsp_size      = 0x00000000;
    unsigned int superdag_size  = 0x00000000;
#endif
    FILE *secret_fp = NULL;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'b':
            if (is_err(schdr_set_dram_mode(SCHDR_DRAM_MODE_SHM, optarg))) {
                printf("amba_vision: (error) Cannot set DRAM mode!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'k':
            key = strtoull(optarg, NULL, 0);
            break;
        case 's':
            inet = optarg;
            break;
        case 'd':
            dram_size = strtoul(optarg, NULL, 0);
            break;
#if defined(USE_CVAPI_V7)
        case 'c':
            cma_size = strtoull(optarg, NULL, 0);
            break;
        case 'S':
            superdag_size = strtoull(optarg, NULL, 0);
            break;
        case 'D':
            idsp_size = strtoull(optarg, NULL, 0);
            break;
#endif
        default:
            break;
        }
    }
#if !defined(__QNXNTO__)
#if defined(USE_CVAPI_V7)
    if (is_err(amalgam_config(inet, port, key, dram_size, cma_size, superdag_size, idsp_size))) {
        printf("amba_vision: (error) Cannot configure Amalgam!\n");
        exit(EXIT_FAILURE);

    }
#else
    if (is_err(amalgam_config(inet, port, key, dram_size))) {
        printf("amba_vision: (error) Cannot configure Amalgam!\n");
        exit(EXIT_FAILURE);
    }
#endif
#endif
}

static void parse_schdr_opt(int argc, char **argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'b':
        case 'k':
        case 's':
        case 'd':
            break;
        case 't':
            add_cvtable(optarg);
            break;
        case 'f':
            add_sysflow(optarg);
            break;
        case 'o':
            boot_flag = strtol(optarg, NULL, 0);
            break;
        case 'p':
            read_secret(optarg);
            break;
        default:
            print_help(argv[0]);
            kill(0, SIGINT);
        }
    }
}

static int app_callback(int type, void *arg)
{
    int i, ret = 0;

    switch(type) {
        case SCHDR_CB_START_REGISTRATION:
            printf("\n====AMBA_VISION starts cvtask registration\n");
            break;
        case SCHDR_CB_START_QUERY:
            printf("\n====AMBA_VISION starts cvtask query\n");
            break;
        case SCHDR_CB_START_INIT:
            if (secret_is_available)
                ret = cvtask_send_private_msg(2, arm_secret);
            if (is_err(ret)) {
                printf("amba_vision: (error) Cannot send private message.\n");
                exit(EXIT_FAILURE);
            }
            printf("\n====AMBA_VISION is about to init cvtasks\n");
            break;
        case SCHDR_CB_START_RUN:
            printf("\n====AMBA_VISION is about to run cvtasks\n");
            break;
        case SCHDR_CB_START_SHUTDOWN:
            printf("\n====SUPERDAG is about to shutdown\n");
            if (sem_post(&done_sem)) {
                printf("Failed to increment done_sem.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            printf("amba_vision: (warn): skip unknow callback type %d\n", type);
            ret = -1;
    }
    return ret;
}

static void sigint_handler(int sig) {
    if (!schdr_shutdown((int32_t) sig))
        printf("amba_vision: (error): cannot shut down scheduler.\n");
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigaction(SIGINT, &act, NULL);
    kill(0, SIGINT);
}

static void install_handler(void) {
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
}

int main(int argc, char **argv)
{
    if (sem_init(&done_sem, 0, 0)) {
        printf("Failed to initialize done_sem.\n");
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }

    /* parse opt to config amalgam before we init scheduler */
    parse_amalgam_opt(argc, argv);

    if (is_err(schdr_init(0xD, 0, boot_flag))) {
        printf("amba_vision: (error): cannot initialize scheduler.\n");
        exit(EXIT_FAILURE);
    }

    if (is_err(schdr_set_callback(app_callback))) {
        printf("amba_vision: (error): cannot initialize scheduler.\n");
        exit(EXIT_FAILURE);
    }

    parse_schdr_opt(argc, argv);

    if (is_err(schdr_start())) {
        printf("amba_vision: (error): cannot initialize scheduler.\n");
        exit(EXIT_FAILURE);
    }

    /* main thread got nothing to do. put it to indefinite sleep */
    if (sem_wait(&done_sem)) {
        printf("Failed to decrement done_sem.\n");
        perror("Message from perror");
        exit(EXIT_FAILURE);
    }
    return 0;
}
