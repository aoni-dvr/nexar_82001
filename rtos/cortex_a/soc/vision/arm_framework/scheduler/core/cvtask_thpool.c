/**
 *  @file thpool.c
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
 *  @details Thread pool APIs
 *
 */

#include "os_api.h"
#include "ambint.h"
#include "schdrmsg_def.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "flexidag.h"
#include "thpool.h"
#include "schdr_api.h"

#ifndef DISABLE_ARM_CVTASK

#define THPOOL_JOB_SET_SIZE     64U
#define THPOOL_JOB_CTX_SIZE     ARMVIS_MESSAGE_SIZE

typedef struct {
#define JOB_STATUS_FREE    0U
#define JOB_STATUS_ADDED   1U
#define JOB_STATUS_SCHED   2U
    uint32_t           status;
    uint32_t           priority;
    thpool_handler_t   handler;
    char               padding[ARM_CACHELINE_SIZE - 8U - sizeof(void*)];
    char               ctx[THPOOL_JOB_CTX_SIZE];
} thpool_job_t;

typedef struct {
    /*-= Standard threads =-*/
    thpool_job_t       jobs[THPOOL_JOB_SET_SIZE];
    thpool_tdb_t       tdb[MAX_THPOOL_SIZE];
    int32_t            job_count;
    psem_t             sem_producer;
    psem_t             sem_consumer;

    /*-= Extra threads (Blockable =-*/
    thpool_job_t       jobs_extra[THPOOL_JOB_SET_SIZE];
    thpool_tdb_t       tdb_extra[MAX_THPOOL_EXTRA_LIMITED];
    int32_t            job_count_extra;
    psem_t             sem_producer_extra;
    psem_t             sem_consumer_extra;

    pmutex_t           job_lock;
    uint32_t           cpus_map;
} thpool_db_t;

static cvtask_thpool_env_t              thenv[MAX_THPOOL_SIZE];
static cvtask_thpool_env_t              thenv_extra[MAX_THPOOL_EXTRA_LIMITED];

static thpool_db_t __attribute__ ((aligned (ARM_CACHELINE_SIZE))) thpool GNU_SECTION_NOZEROINIT;

static uint32_t add_job(uint32_t priority, thpool_handler_t handler, const armvis_msg_t *pMsg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t i = 0U;
    thpool_job_t *job;

    (void) pMsg;
    retcode = thread_mutex_lock(&thpool.job_lock);
    if(retcode == ERRCODE_NONE) {
        for (i = 0U; i < THPOOL_JOB_SET_SIZE; i++) {
            job = &(thpool.jobs[i]);
            if (job->status == JOB_STATUS_FREE) {
                thpool.job_count++;
                job->status = JOB_STATUS_ADDED;
                job->priority = priority;
                job->handler = handler;
                if (pMsg != NULL) {
                    retcode = thread_memcpy(job->ctx, pMsg, THPOOL_JOB_CTX_SIZE);
                }
                break;
            }
        }
        ret = thread_mutex_unlock(&thpool.job_lock);
        if(retcode == ERRCODE_NONE) {
            retcode = ret;
        }
    }

    if(i > THPOOL_JOB_SET_SIZE) {
        retcode = ERR_DRV_SCHDR_THPOOL_JOB_OUT_OF_RANGE;
        console_printU5("[ERROR] add_job() : i (%d) > THPOOL_JOB_SET_SIZE (%d)\n", i, THPOOL_JOB_SET_SIZE, 0U, 0U, 0U);
    }

    return retcode;
}

static uint32_t add_job_extra(uint32_t priority, thpool_handler_t handler, const armvis_msg_t *pMsg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    uint32_t i = 0U;
    thpool_job_t *job;

    (void) pMsg;
    retcode = thread_mutex_lock(&thpool.job_lock);
    if(retcode == ERRCODE_NONE) {
        for (i = 0U; i < THPOOL_JOB_SET_SIZE; i++) {
            job = &(thpool.jobs_extra[i]);
            if (job->status == JOB_STATUS_FREE) {
                thpool.job_count_extra++;
                job->status = JOB_STATUS_ADDED;
                job->priority = priority;
                job->handler = handler;
                if (pMsg != NULL) {
                    retcode = thread_memcpy(job->ctx, pMsg, THPOOL_JOB_CTX_SIZE);
                }
                break;
            }
        }
        ret = thread_mutex_unlock(&thpool.job_lock);
        if(retcode == ERRCODE_NONE) {
            retcode = ret;
        }
    }

    if(i > THPOOL_JOB_SET_SIZE) {
        retcode = ERR_DRV_SCHDR_THPOOL_JOB_OUT_OF_RANGE;
        console_printU5("[ERROR] add_job_extra() : i (%d) > THPOOL_JOB_SET_SIZE (%d)\n", i, THPOOL_JOB_SET_SIZE, 0U, 0U, 0U);
    }

    return retcode;
}

static uint32_t free_job(thpool_job_t *job)
{
    if(job == NULL) {
        console_printU5("[ERROR] free_job() : job == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(thread_mutex_lock(&thpool.job_lock) != ERRCODE_NONE) {
            console_printU5("[ERROR] free_job() : thread_mutex_lock fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            job->status = JOB_STATUS_FREE;
            thpool.job_count--;
            if(thread_mutex_unlock(&thpool.job_lock) != ERRCODE_NONE) {
                console_printU5("[ERROR] free_job() : thread_mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return ERRCODE_NONE;
}

static uint32_t free_job_extra(thpool_job_t *job)
{
    if(job == NULL) {
        console_printU5("[ERROR] free_job_extra() : job == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(thread_mutex_lock(&thpool.job_lock) != ERRCODE_NONE) {
            console_printU5("[ERROR] free_job_extra() : thread_mutex_lock fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            job->status = JOB_STATUS_FREE;
            thpool.job_count_extra--;
            if(thread_mutex_unlock(&thpool.job_lock) != ERRCODE_NONE) {
                console_printU5("[ERROR] free_job_extra() : thread_mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return ERRCODE_NONE;
}

static thpool_job_t* get_job(void)
{
    uint32_t i;
    uint32_t min = 0xFFFFFFFFU;
    thpool_job_t *job, *chosen = NULL;

    if(thread_mutex_lock(&thpool.job_lock) != ERRCODE_NONE) {
        console_printU5("[ERROR] get_job() : thread_mutex_lock fail ", 0U, 0U, 0U, 0U, 0U);
    } else {
        for (i = 0U; i < THPOOL_JOB_SET_SIZE; i++) {
            job = &(thpool.jobs[i]);
            if (job->status == JOB_STATUS_ADDED) {
                if ((chosen == NULL) || (((job->priority - min) & 0x80000000U) != 0U)) {
                    chosen = job;
                    min = chosen->priority;
                }
            }
        }
        if(chosen == NULL) {
            console_printU5("[ERROR] get_job() : chosen == NULL \n", 0U, 0U, 0U, 0U, 0U);
        } else {
            chosen->status = JOB_STATUS_SCHED;
        }
        if(thread_mutex_unlock(&thpool.job_lock) != ERRCODE_NONE) {
            console_printU5("[ERROR] get_job() : thread_mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return chosen;
}

static thpool_job_t* get_job_extra(void)
{
    uint32_t i;
    uint32_t min = 0xFFFFFFFFU;
    thpool_job_t *job, *chosen = NULL;

    if(thread_mutex_lock(&thpool.job_lock) != ERRCODE_NONE) {
        console_printU5("[ERROR] get_job_extra() : thread_mutex_lock fail ", 0U, 0U, 0U, 0U, 0U);
    } else {
        for (i = 0U; i < THPOOL_JOB_SET_SIZE; i++) {
            job = &(thpool.jobs_extra[i]);
            if (job->status == JOB_STATUS_ADDED) {
                if ((chosen == NULL) || (((job->priority - min) & 0x80000000U) != 0U)) {
                    chosen = job;
                    min = chosen->priority;
                }
            }
        }
        if(chosen == NULL) {
            console_printU5("[ERROR] get_job_extra() : chosen == NULL \n", 0U, 0U, 0U, 0U, 0U);
        } else {
            chosen->status = JOB_STATUS_SCHED;
        }
        if(thread_mutex_unlock(&thpool.job_lock) != ERRCODE_NONE) {
            console_printU5("[ERROR] get_job_extra() : thread_mutex_unlock fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return chosen;
}

static void* thread_entry(void *arg)
{
    thpool_tdb_t *tdb = NULL;
    thpool_job_t* job;
    uint32_t run_flag = 1U;

    (void) arg;
    if(arg == NULL) {
        console_printU5("[ERROR] thread_entry() : arg == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        typecast(&tdb,&arg);
        tdb->tid = thread_self();
        while (run_flag == 1U) {
            if(thread_sem_wait(&thpool.sem_consumer, OS_API_WAIT_FOREVER) != ERRCODE_NONE) {
                console_printU5("[ERROR] thread_entry() : thread_sem_wait fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                job = get_job();
                if(job != NULL) {
                    if (job->handler != NULL) {
                        if(job->handler(job->ctx) != ERRCODE_NONE) {
                            console_printU5("[ERROR] thread_entry() : job->handler fail ", 0U, 0U, 0U, 0U, 0U);
                        }
                    } else {
                        run_flag = 0U;
                    }
                    if (free_job(job) != ERRCODE_NONE) {
                        console_printU5("[ERROR] thread_entry() : free_job fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                }
                if(thread_sem_post(&thpool.sem_producer) != ERRCODE_NONE) {
                    console_printU5("[ERROR] thread_entry() : thread_sem_post fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return NULL;
}

static void* thread_extra_entry(void *arg)
{
    thpool_tdb_t *tdb = NULL;
    thpool_job_t* job;
    uint32_t run_flag = 1U;

    (void) arg;
    if(arg == NULL) {
        console_printU5("[ERROR] thread_extra_entry() : arg == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        typecast(&tdb,&arg);
        tdb->tid = thread_self();
        while (run_flag == 1U) {
            if(thread_sem_wait(&thpool.sem_consumer_extra, OS_API_WAIT_FOREVER) != ERRCODE_NONE) {
                console_printU5("[ERROR] thread_extra_entry() : thread_sem_wait fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                job = get_job_extra();
                if(job != NULL) {
                    if (job->handler != NULL) {
                        if(job->handler(job->ctx) != ERRCODE_NONE) {
                            console_printU5("[ERROR] thread_extra_entry() : job->handler fail ", 0U, 0U, 0U, 0U, 0U);
                        }
                    } else {
                        run_flag = 0U;
                    }
                    if(free_job_extra(job) != ERRCODE_NONE) {
                        console_printU5("[ERROR] thread_extra_entry() : free_job_extra fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                }
                if( thread_sem_post(&thpool.sem_producer_extra) != ERRCODE_NONE) {
                    console_printU5("[ERROR] thread_extra_entry() : thread_sem_post fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return NULL;
}

uint32_t thpool_init(uint32_t cpus_map)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;
    static uint32_t thpool_inited;
    uint32_t  i, max_thpool_extra_per_thread;
    uint32_t  thpool_priority, thpool_extra_priority, thpool_stack_size, thpool_extra_stack_size;
    char *pstack, *pextra_stack;
    static char name[32] = "cv_thread_pool";
    static char extra_name[32] = "cv_thread_extra_pool";

    max_thpool_extra_per_thread = get_max_thpool_extra_per_thread();
    thpool_priority             = get_thread_priority();
    thpool_extra_priority       = get_thread_extra_priority();
    thpool_stack_size           = get_thread_stack_size();
    thpool_extra_stack_size     = get_thread_extra_stack_size();

    if (thpool_inited == 0U) {
        /* init locks */
        thpool_inited = 1U;
        retcode = thread_sem_init(&thpool.sem_producer, 0, THPOOL_JOB_SET_SIZE);

        if(retcode == ERRCODE_NONE) {
            retcode = thread_sem_init(&thpool.sem_consumer, 0, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = thread_sem_init(&thpool.sem_producer_extra, 0, THPOOL_JOB_SET_SIZE);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = thread_sem_init(&thpool.sem_consumer_extra, 0, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = thread_mutex_init(&thpool.job_lock);
        }
        thpool.job_count        = 0;
        thpool.job_count_extra  = 0;
    }

    if(retcode == ERRCODE_NONE) {
        ret = thread_memset(thpool.jobs, 0,sizeof(thpool.jobs));
        ret |= thread_memset(thpool.jobs_extra, 0,sizeof(thpool.jobs_extra));
        ret |= thread_memset(thpool.tdb, 0, sizeof(thpool.tdb));
        ret |= thread_memset(thpool.tdb_extra, 0, sizeof(thpool.tdb_extra));
        if(ret != ERRCODE_NONE) {
            retcode = ERR_DRV_SCHDR_MEMSET_FAIL;
        }

        if(retcode == ERRCODE_NONE) {
            thpool.cpus_map = cpus_map;
            /* create threads, should be as last step */
            for (i = 0U; i < (uint32_t)MAX_THPOOL_SIZE; i++) {
                thpool_tdb_t *tdb = &thpool.tdb[i];
                uint32_t bit = (uint32_t)1U << i;
                uint32_t  subloop;
                uint32_t  base_extra;

                /* skip the core that is not in cpus_map */
                if ((bit & cpus_map) == 0U) {
                    continue;
                }

                /* create the thread */
                retcode = get_stack_ptr(i, &pstack);
                if(retcode == ERRCODE_NONE) {
                    retcode = thread_create(
                                  &tdb->thread,
                                  thread_entry,
                                  tdb,
                                  thpool_priority,
                                  bit,
                                  thpool_stack_size,
                                  pstack,
                                  name);
                }

                if(retcode == ERRCODE_NONE) {
                    base_extra = i * max_thpool_extra_per_thread;
                    for (subloop = 0U; subloop < max_thpool_extra_per_thread; subloop++) {
                        tdb = &thpool.tdb_extra[base_extra + subloop];
                        retcode = get_stack_extra_ptr(base_extra + subloop, &pextra_stack);
                        if(retcode == ERRCODE_NONE) {
                            retcode = thread_create(
                                          &tdb->thread,
                                          thread_extra_entry,
                                          tdb,
                                          thpool_extra_priority,
                                          bit,
                                          thpool_extra_stack_size,
                                          pextra_stack,
                                          extra_name);
                        }
                        if(retcode != ERRCODE_NONE) {
                            break;
                        }
                    } /* for (subloop = 0; subloop < max_thpool_extra_per_thread; subloop++) */
                }
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] thpool_init() : thread_create fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return retcode;
}

uint32_t thpool_fini(void)
{
    uint32_t i, max_thpool_extra_per_thread, max_thpool_extra_size;

    max_thpool_extra_per_thread = get_max_thpool_extra_per_thread();
    max_thpool_extra_size       = max_thpool_extra_per_thread * ((uint32_t)MAX_THPOOL_SIZE);

    if(thpool_wait_for_finish() != ERRCODE_NONE) {
        console_printU5("[ERROR] thpool_fini() : thpool_wait_for_finish fail ", 0U, 0U, 0U, 0U, 0U);
    }

    /* send a NULL job to termniate the threads in the pool  */
    for (i = 0U; i < (uint32_t)MAX_THPOOL_SIZE; i++) {
        uint32_t subloop, bit;

        bit = (uint32_t)1U << i;
        if ((bit & thpool.cpus_map) == 0U) {
            continue;
        }
        if(thpool_add_task(0U, NULL, NULL) != ERRCODE_NONE) {
            console_printU5("[ERROR] thpool_fini() : thpool_add_task fail ", 0U, 0U, 0U, 0U, 0U);
        }
        for (subloop = 0U; subloop < max_thpool_extra_per_thread; subloop++) {
            if(thpool_add_extra_task(0U, NULL, NULL) != ERRCODE_NONE) {
                console_printU5("[ERROR] thpool_fini() : thpool_add_extra_task fail ", 0U, 0U, 0U, 0U, 0U);
            }
        } /* for (subloop = 0; subloop < max_thpool_extra_per_thread; subloop++) */
    }

    if(thpool_wait_for_finish() != ERRCODE_NONE) {
        console_printU5("[ERROR] thpool_fini() : thpool_wait_for_finish fail ", 0U, 0U, 0U, 0U, 0U);
    }

    /* delete all threads in the pool */
    for (i = 0U; i < (uint32_t)MAX_THPOOL_SIZE; i++) {
        uint32_t subloop, bit, base_index;

        bit = (uint32_t)1U << i;
        if ( (bit & thpool.cpus_map) == 0U) {
            continue;
        }
        if(thread_delete(&thpool.tdb[i].thread) != ERRCODE_NONE) {
            console_printU5("[ERROR] thpool_fini() : thread_delete fail ", 0U, 0U, 0U, 0U, 0U);
        }
        base_index = i * max_thpool_extra_per_thread;
        for (subloop = 0U; subloop < max_thpool_extra_per_thread; subloop++) {
            if(thread_delete(&thpool.tdb_extra[base_index + subloop].thread) != ERRCODE_NONE) {
                console_printU5("[ERROR] thpool_fini() : thread_delete fail ", 0U, 0U, 0U, 0U, 0U);
            }
        } /* for (subloop = 0; subloop < max_thpool_extra_per_thread; subloop++) */
    }

    /* free private_storage memory */
    for (i = 0U; i < (uint32_t)MAX_THPOOL_SIZE; i++) {
        if (thenv[i].scratchpad_base != NULL) {
            put_thread_scratchpad_ptr(thenv[i].scratchpad_base);
            thenv[i].scratchpad_base = NULL;
        }
    }
    for (i = 0U; i < max_thpool_extra_size; i++) {
        if (thenv_extra[i].scratchpad_base != NULL) {
            put_thread_scratchpad_ptr(thenv_extra[i].scratchpad_base);
            thenv_extra[i].scratchpad_base = NULL;
        }
    }

    if(thread_memset(thenv, 0, sizeof(thenv)) != ERRCODE_NONE) {
        console_printU5("[ERROR] thpool_fini() : thread_memset fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return ERRCODE_NONE;
}


uint32_t thpool_add_task(uint32_t priority, thpool_handler_t handler,const armvis_msg_t *pMsg)
{
    uint32_t  is_valid;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (pMsg == NULL) { /* THPOOL termination request */
        is_valid = 1U;
    } /* if (arg == NULL) */
    else { /* if (arg != NULL) */
        if ((pMsg->hdr.message_id < 0x100U) || (pMsg->hdr.message_id >= (0x0100U + FLEXIDAG_MAX_SLOTS))) {
            is_valid = 1U;
        } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100 + FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
            is_valid = is_associated_flexidag(pMsg);
        } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
    } /* if (arg != NULL) */

    if (is_valid != 0U) {
        retcode = thread_sem_wait(&thpool.sem_producer, OS_API_WAIT_FOREVER);
        if(retcode == ERRCODE_NONE) {
            retcode = add_job(priority, handler, pMsg);
            ret = thread_sem_post(&thpool.sem_consumer);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    } /* if (is_valid != 0) */
    return retcode;
}

uint32_t thpool_add_extra_task(uint32_t priority, thpool_handler_t handler,const armvis_msg_t *pMsg)
{
    uint32_t  is_valid;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret = ERRCODE_NONE;

    if (pMsg == NULL) { /* THPOOL termination request */
        is_valid = 1U;
    } /* if (arg == NULL) */
    else { /* if (arg != NULL) */
        if ((pMsg->hdr.message_id < 0x100U) || (pMsg->hdr.message_id >= (0x0100U + FLEXIDAG_MAX_SLOTS))) {
            is_valid = 1U;
        } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100 + FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
            is_valid = is_associated_flexidag(pMsg);
        } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
    } /* if (arg != NULL) */

    if (is_valid != 0U) {
        retcode = thread_sem_wait(&thpool.sem_producer_extra, OS_API_WAIT_FOREVER);
        if(retcode == ERRCODE_NONE) {
            retcode = add_job_extra(priority, handler, pMsg);
            ret = thread_sem_post(&thpool.sem_consumer_extra);
            if(retcode == ERRCODE_NONE) {
                retcode = ret;
            }
        }
    } /* if (is_valid != 0) */
    return retcode;
}

uint32_t thpool_wait_for_finish(void)
{
    uint32_t retcode = ERRCODE_NONE;

    while ((thpool.job_count != 0) && (thpool.job_count_extra != 0)) {
        if( thread_sleep(10) != ERRCODE_NONE) {
            console_printU5("[ERROR] thpool_wait_for_finish() : thread_sleep fail ", 0U, 0U, 0U, 0U, 0U);
            break;
        }
    } /* while ((thpool.job_count != 0) && (thpool.job_count_extra != 0)) */
    return retcode;
}

uint32_t thpool_set_env(cvtask_thpool_env_t *env, uint32_t cpu_index)
{
    uint32_t retcode = ERRCODE_NONE;

    if(env == NULL) {
        console_printU5("[ERROR] thpool_set_env() : env == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        thpool.tdb[cpu_index].env = env;
    }
    return retcode;
}

uint32_t thpool_set_env_extra(cvtask_thpool_env_t *env, uint32_t cpu_index)
{
    uint32_t retcode = ERRCODE_NONE;

    if(env == NULL) {
        console_printU5("[ERROR] thpool_set_env_extra() : env == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        thpool.tdb_extra[cpu_index].env = env;
    }
    return retcode;
} /* thpool_set_env_extra() */

cvtask_thpool_env_t* thpool_get_env(void)
{
    uint32_t i, max_thpool_extra_size, max_thpool_extra_per_thread;
    const tid_t tid = thread_self();
    cvtask_thpool_env_t *env = NULL;
    uint32_t found = 0U;

    (void) tid;
#if defined(CONFIG_BUILD_CV_THREADX)
    thread_unused(tid);
#endif
    max_thpool_extra_per_thread = get_max_thpool_extra_per_thread();
    max_thpool_extra_size       = max_thpool_extra_per_thread * ((uint32_t)MAX_THPOOL_SIZE);
    for (i = 0U; i < (uint32_t)MAX_THPOOL_SIZE; i++) {
        if (thpool.tdb[i].tid == tid) {
            env = thpool.tdb[i].env;
            found = 1U;
            break;
        }
    }

    if(found == 0U) {
        for (i = 0U; i < max_thpool_extra_size; i++) {
            if (thpool.tdb_extra[i].tid == tid) {
                env = thpool.tdb_extra[i].env;
            }
        }
    }

    return env;
}

uint32_t thpool_create_thenv(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index;
    uint32_t max_thpool_extra_per_thread;

    max_thpool_extra_per_thread = get_max_thpool_extra_per_thread();

    for (index = 0U; index < (uint32_t)MAX_THPOOL_SIZE; index++) {
        if ((thpool.cpus_map & ((uint32_t)1U<<index)) != 0U) {
            uint32_t subloop;
            uint32_t base_index;

            thenv[index].cpu_id = index;
            retcode = thpool_set_env(&thenv[index], index);
            if(retcode == ERRCODE_NONE) {
                base_index = max_thpool_extra_per_thread * index;
                for (subloop = 0U; subloop < max_thpool_extra_per_thread; subloop++) {
                    thenv_extra[base_index + subloop].cpu_id = (base_index + subloop);
                    retcode = thpool_set_env_extra(&thenv_extra[base_index + subloop], (base_index + subloop));
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                } /* for (subloop = 0; subloop < max_thpool_extra_per_thread; subloop++) */
            } else {
                break;
            }
        }
    }

    return retcode;
}


uint32_t thpool_create_thenv_scratchpads(uint32_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index;
    uint32_t max_thpool_extra_per_thread;

    max_thpool_extra_per_thread = get_max_thpool_extra_per_thread();

    if(max_thpool_extra_per_thread > (uint32_t)MAX_THPOOL_EXTRA_LIMITED) {
        retcode = ERR_DRV_SCHDR_THPOOL_OUT_OF_RANGE;
    } else {
        for (index = 0U; index < (uint32_t)MAX_THPOOL_SIZE; index++) {
            if ((thpool.cpus_map & ((uint32_t)1U<<index)) != 0U) {
                uint32_t  subloop;
                uint32_t  base_index;

                thenv[index].scratchpad_size = size;
                thenv[index].scratchpad_base = get_thread_scratchpad_ptr(index, size);
                if(thenv[index].scratchpad_base == NULL) {
                    retcode = ERR_DRV_SCHDR_SCRATCHPAD_INVALID;
                    break;
                } else {
                    base_index = (max_thpool_extra_per_thread * index);
                    for (subloop = 0U; subloop < max_thpool_extra_per_thread; subloop++) {
                        if((base_index + subloop) < (uint32_t)MAX_THPOOL_EXTRA_LIMITED) {
                            thenv_extra[base_index + subloop].scratchpad_size = 16U*1024U;  /* Limit to 16kb for extra threads */
                            thenv_extra[base_index + subloop].scratchpad_base = get_thread_extra_scratchpad_ptr(index, subloop, 16*1024);
                            if(thenv_extra[base_index + subloop].scratchpad_base == NULL) {
                                retcode = ERR_DRV_SCHDR_SCRATCHPAD_INVALID;
                                break;
                            }
                        }
                    } /* for (subloop = 0; subloop < max_thpool_extra_per_thread; subloop++) */
                }
            }
        }
    }

    return retcode;
}

#endif
