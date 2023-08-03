#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <mqueue.h>
#include "AmbaHost_api.h"
#include "AmbaTest_msg.h"
#include "ambatestd.h"

#define CMSG(fmt, ...)    log_msg(LL_CRITICAL, "SCHDLR: " fmt, ##__VA_ARGS__)
#define IMSG(fmt, ...)    log_msg(LL_NORMAL,   "SCHDLR: " fmt, ##__VA_ARGS__)
#define LOCK_SCHEDULER    pthread_mutex_lock(&mutex)
#define UNLOCK_SCHEDULER  pthread_mutex_unlock(&mutex)

#define MAX_NUM_DEV_CLASSES     128
#define DEV_NOTFOUND            "\ttest devcie not found !\n"

typedef struct {
        char            qname[32];
        atd_spec_t      spec;
        mqd_t           mqd;
        int             count;
} dev_class_t;

static dev_class_t dev_classes[MAX_NUM_DEV_CLASSES];
static int num_dev_classes;
static pthread_mutex_t mutex;
static volatile uint64_t num_jobs;

/* client post a job to scheduler */
int scheduler_post_job(int class, job_ctx_t *job, uint32_t priority)
{
        dev_class_t *dev_class = (dev_class_t*)&dev_classes[class];
        mqd_t mqd = dev_class->mqd;
        int err;

        /* check if we have matching device */
        if (dev_classes[class].count == 0) {
                scheduler_ditch_job(job);
                return AT_ERR_DEV_NOTFOUND;
        }

        if (job->device != NULL) {
                /* post job directly if client is locked on a specific device */
                err = device_post_job(job);
        } else {
                /* post job to the queue */
                err = mq_send(mqd, (const char *)job, sizeof(*job), priority);
        }

        if (err == AT_ERR_OK) {
                /* update statistics */
                if (job->type == JOB_TYPE_DRUN)
                        __sync_add_and_fetch(&num_jobs, 1);
        } else {
                CMSG("post_job failed: %s %d\n", strerror(errno), sizeof(*job));
        }

        return err;
}

/* device fetch job to run */
int scheduler_pend_job(int class, job_ctx_t *ctx, uint32_t timeout)
{
        dev_class_t *dev_class = (dev_class_t*)&dev_classes[class];
        mqd_t mqd = dev_class->mqd;
        struct timespec ts;
        int ret;

        while(1) {
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += 1;
                ret = mq_timedreceive(mqd, (char*)ctx, sizeof(*ctx), NULL, &ts);
                if (ret > 0 && client_is_down(ctx->client)) {
                        /* client is down, we don't have to run this job */
                        client_clear_job(ctx);
                } else {
                        break;
                }
        }

        return ret;
}

/* scheduler thrown away a job */
int scheduler_ditch_job(job_ctx_t *job)
{
        job->header.ret = AT_ERR_DEV_NOTFOUND;
        client_finish_job(job, DEV_NOTFOUND, strlen(DEV_NOTFOUND));
        return AT_ERR_OK;
}

int scheduler_get_device_class(atd_spec_t *in, int flags)
{
        int i;

        LOCK_SCHEDULER;
        /* find class index if device spec exists */
        for (i = 0; i < num_dev_classes; i++) {
                atd_spec_t *spec = &dev_classes[i].spec;
                if (!strcmp(spec->device_hw, in->device_hw) &&
                    !strcmp(spec->device_os, in->device_os)) {
                        break;
                }
        }

         /* create a new class for incoming spec */
        if (flags && i == num_dev_classes) {
                char queue_name[64];
                mqd_t mqd;
                struct mq_attr attr;
                dev_classes[i].spec = *in;
                sprintf(queue_name, "/ambatestd_%s_%s", 
                        in->device_hw, in->device_os);
                attr.mq_flags   = 0;
                attr.mq_maxmsg  = 8;
                attr.mq_msgsize = sizeof(job_ctx_t);
                attr.mq_curmsgs = 0;
                mqd = mq_open(queue_name, O_RDWR | O_CREAT, 0644, &attr);
                if (mqd == -1) {
                        printf("Failed to open queue %s: %s\n", 
                               queue_name, strerror(errno));
                        exit(-1);
                }
                dev_classes[i].mqd = mqd;
                strcpy(dev_classes[i].qname, queue_name);
                ++num_dev_classes;
        }
        UNLOCK_SCHEDULER;
                
        return (i != num_dev_classes) ? i : -1;
}

int scheduler_init(void)
{
        pthread_mutex_init(&mutex, NULL);
        num_dev_classes = 0;
        return AT_ERR_OK;
}

int scheduler_shutdown(void)
{
        int i;

        LOCK_SCHEDULER;
        for (i = 0; i < num_dev_classes; i++) {
                mq_close(dev_classes[i].mqd);
                mq_unlink(dev_classes[i].qname);
        }
        UNLOCK_SCHEDULER;
        return AT_ERR_OK;
}

int scheduler_add_device(device_handle handle)
{
        int class = device_get_class(handle);
 
        LOCK_SCHEDULER;
        dev_classes[class].count += 1;
        UNLOCK_SCHEDULER;

        return AT_ERR_OK;
}

int scheduler_remove_device(device_handle handle)
{
        int class = device_get_class(handle);

        LOCK_SCHEDULER;
        dev_classes[class].count -= 1;
        if (dev_classes[class].count == 0) {
                job_ctx_t job;
                /* release all jobs for this class */
                while (scheduler_pend_job(class, &job, 1) > 0) {
                         scheduler_ditch_job(&job);
                }
        }
        UNLOCK_SCHEDULER;

        return AT_ERR_OK;
}

int scheduler_fill_stat(at_stat_t *stat)
{
        stat->num_jobs = num_jobs;
        return AT_ERR_OK;
}
