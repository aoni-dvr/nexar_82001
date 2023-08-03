/**
 *  @file thpool.h
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
 *  @details Definitions & Constants for thread pool
 *
 */

#ifndef THPOOL_H_FILE
#define THPOOL_H_FILE

typedef uint32_t (*thpool_handler_t)(void *arg);

uint32_t get_max_thpool_extra_per_thread(void);

uint32_t get_thread_priority(void);
uint32_t get_thread_extra_priority(void);

uint32_t get_thread_stack_size(void);
uint32_t get_thread_extra_stack_size(void);

void* get_thread_scratchpad_ptr(uint32_t thread_pool, uint32_t size);
void* get_thread_extra_scratchpad_ptr(uint32_t thread_pool, uint32_t extra_thread_pool, uint32_t extra_size);
void put_thread_scratchpad_ptr(void *ptr);

uint32_t get_stack_ptr(uint32_t index, char **ppstack);
uint32_t get_stack_extra_ptr(uint32_t index, char **ppstack_extra);

#ifndef DISABLE_ARM_CVTASK
/**
 * @cpus_maps:  cpu affinity for each thread, also implies the number of threads
 * @priority:   priority for each thread
 */
uint32_t thpool_init(uint32_t cpus_map);

/**
 * This is blocking call to add a new task to the thread pool.
 * @thpool:     thread pool created by thpool_init
 * @handler:    function that handle the task
 * @priority:   the priority of the task
 * @arg:        pointer of data to be passed to handler, arg[0 ~ (arg_size-1)]
 *              will be copied to an internal buffer, the original data can
 *              be released after this function returns.
 */
uint32_t thpool_add_task(uint32_t priority,
                         thpool_handler_t handler, const armvis_msg_t *pMsg);

/**
 * This is blocking call to add a new task to the "extra" thread pool.
 * @thpool:     thread pool created by thpool_init
 * @handler:    function that handle the task
 * @priority:   the priority of the task
 * @arg:        pointer of data to be passed to handler, arg[0 ~ (arg_size-1)]
 *              will be copied to an internal buffer, the original data can
 *              be released after this function returns.
 */
uint32_t thpool_add_extra_task(uint32_t priority,
                               thpool_handler_t handler, const armvis_msg_t *pMsg);

uint32_t thpool_create_thenv(void);
uint32_t thpool_create_thenv_scratchpads(uint32_t size);
/**
 * Set a thread-private environment variable
 */
uint32_t thpool_set_env(cvtask_thpool_env_t *env, uint32_t cpu_index);
uint32_t thpool_set_env_extra(cvtask_thpool_env_t *env, uint32_t cpu_index);

/**
 * Get the thread-private
 */
cvtask_thpool_env_t* thpool_get_env(void);

/**
 * Wait for all tasks to finish
 */
uint32_t thpool_wait_for_finish(void);

/**
 * finish the thread-pool
 */
uint32_t thpool_fini(void);
#else
static inline uint32_t thpool_init(uint32_t cpus_map)
{
    (void) cpus_map;
    return ERRCODE_NONE;
}
static inline uint32_t thpool_add_task(uint32_t priority,
                                       thpool_handler_t handler, armvis_msg_t *pMsg)
{
    (void) priority;
    (void) handler;
    (void) pMsg;
    console_printU5("[ERROR] thpool_add_task() : type (0x%x) not support", (uint32_t)pMsg->hdr.message_type, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_THPOOL_ADD_TSK_FAIL;
}
static inline uint32_t thpool_add_extra_task(uint32_t priority,
        thpool_handler_t handler, armvis_msg_t *pMsg)
{
    (void) priority;
    (void) handler;
    (void) pMsg;
    console_printU5("[ERROR] thpool_add_extra_task() : type (0x%x) not support", (uint32_t)pMsg->hdr.message_type, 0U, 0U, 0U, 0U);
    return ERR_DRV_SCHDR_THPOOL_ADD_TSK_FAIL;
}
static inline uint32_t thpool_set_env(cvtask_thpool_env_t *env, uint32_t cpu_index)
{
    (void) env;
    (void) cpu_index;
    return ERRCODE_NONE;
}
static inline uint32_t thpool_set_env_extra(cvtask_thpool_env_t *env, uint32_t cpu_index)
{
    (void) env;
    (void) cpu_index;
    return ERRCODE_NONE;
}
static inline cvtask_thpool_env_t* thpool_get_env(void)
{
    return NULL;
}
static inline uint32_t thpool_wait_for_finish(void)
{
    return ERRCODE_NONE;
}
static inline uint32_t thpool_fini(void)
{
    return ERRCODE_NONE;
}

static inline uint32_t thpool_create_thenv(void)
{
    return ERRCODE_NONE;
}

static inline uint32_t thpool_create_thenv_scratchpads(uint32_t size)
{
    (void) size;
    return ERRCODE_NONE;
}

#endif

#endif /* ?THPOOL_H_FILE */

