/**
 *  @file schdr_resource.c
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
 *  @details Scheduler resource APIs
 *
 */

#include <stdlib.h>
#include "os_api.h"
#include "schdr_api.h"

/* Linux does not require explicit priorities or stacks defined when threads are created */
#if !defined(CONFIG_THPOOL_EXTRA_PER_THREAD)
#define MAX_THPOOL_EXTRA_PER_THREAD     2
#else
#define MAX_THPOOL_EXTRA_PER_THREAD     CONFIG_THPOOL_EXTRA_PER_THREAD
#endif

#if !defined(CONFIG_THPOOL_PRIORITY)
#define THPOOL_PRIORITY                 40U
#else
#define THPOOL_PRIORITY                 CONFIG_THPOOL_PRIORITY
#endif

#if !defined(CONFIG_EXTRA_THPOOL_PRIORITY)
#define THPOOL_EXTRA_PRIORITY           39U
#else
#define THPOOL_EXTRA_PRIORITY           CONFIG_EXTRA_THPOOL_PRIORITY
#endif

#define MAX_THPOOL_EXTRA_SIZE   (MAX_THPOOL_SIZE * MAX_THPOOL_EXTRA_PER_THREAD)

#if (MAX_THPOOL_EXTRA_SIZE > MAX_THPOOL_EXTRA_LIMITED)
#error "MAX_THPOOL_EXTRA_SIZE > 32"
#endif

#define ambacv_malloc(x)        malloc(x)
#define ambacv_free(x)          free(x)

uint32_t get_max_thpool_extra_per_thread(void)
{
    return MAX_THPOOL_EXTRA_PER_THREAD;
} /* get_max_thpool_extra_per_thread() */

uint32_t get_thread_priority(void)
{
    return THPOOL_PRIORITY;
} /* get_thread_priority() */

uint32_t get_thread_extra_priority(void)
{
    return THPOOL_EXTRA_PRIORITY;
} /* get_thread_extra_priority() */

uint32_t get_thread_stack_size(void)
{
    return 0; /* Linux defines it's own stack for each thread */
} /* get_thread_stack_size() */

uint32_t get_thread_extra_stack_size(void)
{
    return 0; /* Linux defines it's own stack for each thread */
} /* get_thread_extra_stack_size() */

uint32_t get_stack_ptr(uint32_t index, char **ppstack)
{
    (void) index;
    *ppstack = NULL;
    return 0; /* Linux defines it's own stack for each thread */
} /* get_stack_ptr() */

uint32_t get_stack_extra_ptr(uint32_t index, char **ppstack_extra)
{
    (void) index;
    *ppstack_extra = NULL;
    return 0; /* Linux defines it's own stack for each thread */
} /* get_stack_extra_ptr() */

void* get_thread_scratchpad_ptr(uint32_t thread_pool, uint32_t size)
{
    (void) thread_pool;
    return ambacv_malloc(size);
}

void* get_thread_extra_scratchpad_ptr(uint32_t thread_pool, uint32_t extra_thread_pool, uint32_t extra_size)
{
    (void) thread_pool;
    (void) extra_thread_pool;
    return ambacv_malloc(extra_size);
}

void put_thread_scratchpad_ptr(void *ptr)
{
    ambacv_free(ptr);
}

int32_t schdr_prefetch(int32_t reserved)
{
    char  *buff;
    size_t size = 256;
    FILE *mapfp;
    int32_t junk_sum = 0;

    (void) reserved;
    buff = malloc(size);
    sprintf(buff, "/proc/%d/maps", getpid());
    mapfp = fopen(buff, "rb");
    if (mapfp == NULL) {
        printf("[ERROR] schdr_prefetch() : Can't open maps file, Prefetch Failed !\n");
        junk_sum = -1;
    } else {
        while (getline(&buff, &size, mapfp) != -1) {
            char *start, *end;
            char permission[16], offset[16], devid[16], file_name[256];
            int32_t inode;

            sscanf(buff, "%p-%p %s %s %s %d %s", &start, &end, permission,
                   offset, devid, &inode, file_name);
            //printf("%s", buff);
            if (permission[0] == 'r' && strstr(file_name, ".so")) {
                printf("Prefetch %s: offset=%s len=0x%lx\n",file_name, offset, end-start);
                while (start < end) {
                    junk_sum += start[0];
                    start += 0x1000;
                }
            }
        }

        thread_fclose(mapfp);
    }
    free(buff);

    return junk_sum;
}

#if defined(AMBACV_KERNEL_SUPPORT)
int32_t schdrcfg_parse_amalgam_opts(int32_t argc, char **argv)
{
    (void) argc;
    (void) argv;
    return 0;
}

int32_t amalgam_config(char *inet, int32_t port, uint64_t key,
                       uint32_t dram_size)
{

    (void) inet;
    (void) port;
    (void) key;
    (void) dram_size;
    return 0;
}
#endif

