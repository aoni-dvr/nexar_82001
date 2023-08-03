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

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaCache.h"
#include "AmbaINT_Def.h"
#include "AmbaMisraFix.h"
#include "schdr_api.h"
#include "schdr_resource.h"

#if !defined(CONFIG_CV_PROTECT_PRIORITY)
#define CV_PROTECT_PRIORITY                 180U
#else
#define CV_PROTECT_PRIORITY                 CONFIG_CV_PROTECT_PRIORITY
#endif

#if !defined(CONFIG_CV_PROTECT_STACK_SIZE)
#define CV_PROTECT_STACK_SIZE               (2048)
#else
#define CV_PROTECT_STACK_SIZE               CONFIG_CV_PROTECT_STACK_SIZE
#endif

#if defined(CONFIG_BUILD_CV_THREADX)
#ifndef CONFIG_DISABLE_ARM_CVTASK

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

#if !defined(CONFIG_THPOOL_STACK_SIZE)
#define THPOOL_STACK_SIZE               1024*128
#else
#define THPOOL_STACK_SIZE               CONFIG_THPOOL_STACK_SIZE
#endif

#if !defined(CONFIG_EXTRA_THPOOL_STACK_SIZE)
#define THPOOL_STACK_EXTRA_SIZE         1024*128
#else
#define THPOOL_STACK_EXTRA_SIZE         CONFIG_EXTRA_THPOOL_STACK_SIZE
#endif

#define MAX_THPOOL_EXTRA_SIZE   (MAX_THPOOL_SIZE * MAX_THPOOL_EXTRA_PER_THREAD)

#if (MAX_THPOOL_EXTRA_SIZE > MAX_THPOOL_EXTRA_LIMITED)
#error "MAX_THPOOL_EXTRA_SIZE > 32"
#endif

uint32_t get_max_thpool_extra_per_thread(void)
{
    return MAX_THPOOL_EXTRA_PER_THREAD;
}

uint32_t get_thread_priority(void)
{
    return THPOOL_PRIORITY;
}

uint32_t get_thread_extra_priority(void)
{
    return THPOOL_EXTRA_PRIORITY;
}

uint32_t get_thread_stack_size(void)
{
    return THPOOL_STACK_SIZE;
}

uint32_t get_thread_extra_stack_size(void)
{
    return THPOOL_STACK_EXTRA_SIZE;
}

uint32_t get_stack_ptr(uint32_t index, char **ppstack)
{
    static char __attribute__ ((aligned (AMBA_CACHE_LINE_SIZE)))
    stack[MAX_THPOOL_SIZE][THPOOL_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    AmbaMisra_TouchUnused(stack);
    *ppstack = stack[index];
    return 0;
}

uint32_t get_stack_extra_ptr(uint32_t index, char **ppstack_extra)
{
    static char __attribute__ ((aligned (AMBA_CACHE_LINE_SIZE)))
    stack_extra[MAX_THPOOL_EXTRA_SIZE][THPOOL_STACK_EXTRA_SIZE] GNU_SECTION_NOZEROINIT;

    AmbaMisra_TouchUnused(stack_extra);
    *ppstack_extra = stack_extra[index];
    return 0;
}

void* get_thread_scratchpad_ptr(uint32_t thread_pool, uint32_t size)
{
    static char thread_scratchpad[MAX_THPOOL_SIZE][0x100000U] GNU_SECTION_NOZEROINIT;
    void* ptr = NULL;
    const char* addr = NULL;

    if(size > 0x100000U) {
        AmbaPrint_PrintUInt5("[ERROR] get_thread_scratchpad_ptr size > 0x100000U ", size, 0U, 0U, 0U, 0U);
    } else {
        addr = thread_scratchpad[thread_pool];
        AmbaMisra_TouchUnused(thread_scratchpad);
        AmbaMisra_TypeCast(&ptr, &addr);
    }

    return ptr;
}

void* get_thread_extra_scratchpad_ptr(uint32_t thread_pool, uint32_t extra_thread_pool, uint32_t extra_size)
{
    static char thread_extra_scratchpad[MAX_THPOOL_SIZE][MAX_THPOOL_EXTRA_PER_THREAD][0x4000U] GNU_SECTION_NOZEROINIT;
    void* ptr = NULL;
    const char* addr = NULL;

    if(extra_size > 0x4000U) {
        AmbaPrint_PrintUInt5("[ERROR] get_thread_extra_scratchpad_ptr size > 0x4000U ", extra_size, 0U, 0U, 0U, 0U);
    } else {
        addr = thread_extra_scratchpad[thread_pool][extra_thread_pool];
        AmbaMisra_TouchUnused(thread_extra_scratchpad);
        AmbaMisra_TypeCast(&ptr, &addr);
    }

    return ptr;
}

void put_thread_scratchpad_ptr(void *ptr)
{
    AmbaMisra_TouchUnused(ptr);
}
#endif
#endif

/**
 *  CV prtect task resources
 */

uint32_t get_protect_task_priority(void)
{
    return CV_PROTECT_PRIORITY;
}

uint32_t get_protect_task_size(void)
{
    return CV_PROTECT_STACK_SIZE;
}

uint32_t get_protect_task_stack_ptr(char **ppstack)
{
    static char __attribute__ ((aligned (AMBA_CACHE_LINE_SIZE)))
    stack[CV_PROTECT_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    AmbaMisra_TouchUnused(stack);
    *ppstack = stack;
    return 0;
}
