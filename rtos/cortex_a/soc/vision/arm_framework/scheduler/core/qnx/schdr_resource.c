#include <stdlib.h>
#include "os_api.h"
#include "schdr_api.h"

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

