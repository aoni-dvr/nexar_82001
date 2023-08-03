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
#include <time.h>
#include "AmbaHost_api.h"
#include "AmbaTest_msg.h"
#include "ambatestd.h"

#define CMSG(fmt, ...)    log_msg(LL_CRITICAL, "DEVICE: " fmt, ##__VA_ARGS__)
#define IMSG(fmt, ...)    log_msg(LL_NORMAL,   "DEVICE: " fmt, ##__VA_ARGS__)

#define MAX_CONNECTIONS   128
#define LOCK_DATABASE     pthread_mutex_lock(&device_db->mutex)
#define UNLOCK_DATABASE   pthread_mutex_unlock(&device_db->mutex)
#define LOCK_DEVICE(x)    pthread_mutex_lock(&x->mutex)
#define UNLOCK_DEVICE(x)  pthread_mutex_unlock(&x->mutex)
          
#define TEST_DEVICE_STATUS_FREE           0x0000
#define TEST_DEVICE_STATUS_READY          0x0001
#define TEST_DEVICE_STATUS_DOWN           0x0002

typedef struct {
        pthread_mutex_t mutex;            /* device lock                      */
        uint32_t        status;           /* device status                    */
        uint32_t        id;               /* device ID                        */
        int             rfd;              /* socket fd for registration       */
        int             cid;              /* hostAPI connection ID for device */
	char            addr[32];         /* device IP address                */
        atd_spec_t      spec;             /* registered device spec           */
        char            log[MAX_LOGSIZE]; /* log the output of drun           */
        int             log_idx;          /* current log idx                  */
        pthread_t       run_thread;       /* thread to run the job            */
        sem_t           run_sem;          /* semaphore for device run         */
        sem_t           req_sem;          /* semaphoer for new job            */
        job_ctx_t       job;              /* run-request context              */
        int             class;            /* device class                     */
        client_handle   client;           /* client handle if dev is locked   */

        uint32_t        uptime;           /* device registration time         */
        uint32_t        busytime;         /* device total execction time      */
        uint32_t        num_jobs;         /* number of jobs finished          */
} test_device_t;

typedef struct {
        int32_t         num_devices_m1;
        uint32_t        curr_id;
        int32_t         is_shuttingdown;
        pthread_t       accept_thread;
        pthread_t       heartbeat_thread;
        pthread_mutex_t mutex;            /* database lock                    */
        test_device_t   device[0];
} test_device_db;

static int                      socket_fd;
static test_device_db*          device_db;

static void hostapi_log(int pid, const char *string, int size) 
{
        char line[256];
        sprintf(line, "HSTAPI: %s", string);
        log_msg(LL_NORMAL, line);
}

/* callback for collecting log of a device exection */
static void device_log(void *arg, const char *string, int size) 
{
        test_device_t *device = (test_device_t*)arg;
        int idx = device->log_idx;
        int bytesleft = sizeof(device->log) - idx - 1;

        if (size > bytesleft) size = bytesleft;
        memcpy(&device->log[idx], string, size);
        device->log_idx += size;
}

static void *run_entry(void *arg)
{
        test_device_t *dev = (test_device_t *)arg;
        job_ctx_t     *job = &dev->job;
        atc_drun_t    *req = &job->request;
        int ret;

        while (dev->status != TEST_DEVICE_STATUS_DOWN) {
                client_handle client = dev->client;
                struct timespec start, end;

                /* get next job */
                if (client != NULL) {
                        /* we are locked to a client */
                        struct timespec ts;
                        clock_gettime(CLOCK_REALTIME, &ts);
                        ts.tv_sec += 1;
                        ret = sem_timedwait(&dev->req_sem, &ts);
                } else {
                        /* we are free-running */
                        ret = scheduler_pend_job(dev->class, job, 1);
                }
                if (ret == -1) {
                        if (errno == ETIMEDOUT) {
                                continue;
                        } else {
                                CMSG("[%d]: run-entry error %d\n", errno);
                                break;
                        }
                }

                /* execute the job */
                switch(job->type) {
                case JOB_TYPE_LOCK_DEVICE:
                        job->device = dev;
                        dev->client = job->client;
                        break;
                case JOB_TYPE_UNLOCK_DEVICE:
                        dev->client = NULL;
                        break;
                case JOB_TYPE_DRUN:
                        dev->log_idx = 0;
                        clock_gettime(CLOCK_MONOTONIC, &start);
                        job->header.ret = AmbaHost_exec2(
                                dev->cid,
                                device_log,
                                dev,
                                req->command);
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        job->cputime = (end.tv_sec  - start.tv_sec) * 1000 + 
                                (end.tv_nsec - start.tv_nsec)/1000000;
                        dev->busytime = job->cputime;
                        dev->num_jobs++;
                        if (job->header.ret == AT_ERR_OK) {
                                dev->log[dev->log_idx++] = '\0';
                        } else {
                                strcpy(dev->log, "\tjob interrupted\n");
                                dev->log_idx = strlen(dev->log) + 1;
                        }
                        IMSG("[%d] \"%s\" cputime=%dms\n", 
                             dev->id, req->command, job->cputime);
                        break;
                }

                /* notify client on the job finish */
                client_finish_job(job, dev->log, dev->log_idx);

                /* release job semaphore if the device if locked  */
                if (client != NULL) {
                        sem_post(&dev->run_sem);
                }
        }
        return dev;
}

/* put an device online */
static int add_device(int fd, atd_spec_t *spec)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
        char device_addr[32];
        int i, cid, offset, top;

        offset = device_db->curr_id;
        top = device_db->num_devices_m1;

        getpeername(fd, (struct sockaddr*)&addr, &len);
        inet_ntop(AF_INET, &addr.sin_addr, device_addr, sizeof(device_addr));

        /* add the new device */
        for (i = 0; i <= top; i++) {
                test_device_t *dev = &device_db->device[(i+offset) & top ];
                struct timespec now;
                LOCK_DEVICE(dev);
                if ((dev->status == TEST_DEVICE_STATUS_FREE) &&
                    ((cid = AmbaHost_connection_open(device_addr, 0)) > 0)){
                        clock_gettime(CLOCK_MONOTONIC, &now);
                        dev->rfd      = fd;
                        dev->cid      = cid;
                        dev->spec     = *spec;
                        dev->id       = device_db->curr_id++;
                        dev->status   = TEST_DEVICE_STATUS_READY;
                        dev->class    = scheduler_get_device_class(spec, 1);
                        dev->client   = NULL;
                        dev->busytime = 0;
                        dev->num_jobs = 0;
                        dev->uptime   = now.tv_sec;
                        strcpy(dev->addr, device_addr);
                        pthread_create(&dev->run_thread, NULL, run_entry, dev);
                        sem_init(&dev->run_sem, 0, 1);
                        sem_init(&dev->req_sem, 0, 0);
                        IMSG("[%d] online: %s, %s, %s\n", dev->id,
                             dev->addr,
                             dev->spec.device_hw,
                             dev->spec.device_os);
                        scheduler_add_device(dev);
                        UNLOCK_DEVICE(dev);
                        return 0;
                }
                UNLOCK_DEVICE(dev);
        }

        /* Can't insert this device */
        CMSG("Can't insert device %s to database\n", device_addr);
        return -1;
}

/* put a device offline */
static void remove_device(test_device_t *dev)
{
        LOCK_DEVICE(dev);
        dev->status = TEST_DEVICE_STATUS_DOWN;
        shutdown(dev->rfd, 2);
        AmbaHost_connection_close(dev->cid);
        pthread_join(dev->run_thread, NULL);
        scheduler_remove_device(dev);
        dev->rfd = 0;
        dev->cid = 0;
        sem_destroy(&dev->run_sem);
        sem_destroy(&dev->req_sem);
        dev->status = TEST_DEVICE_STATUS_FREE;
        UNLOCK_DEVICE(dev);
        IMSG("[%d] offline\n", dev->id);
}

/* thread enrty that accepts device registration */
static void* accept_entry(void *arg)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
        int port, fd, type;

        port = AMBATEST_DEVICE_PORT;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(socket_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
		CMSG("Can't bind to port %d, err %d\n", port, errno);
		exit(1);
	}
	if (listen(socket_fd, MAX_CONNECTIONS) < 0) {
		CMSG("Can't listen to port %d\n", port);
		exit(1);
	}

	CMSG("listening on port %d for device registration\n", port);
	while (1) {
                fd = accept(socket_fd, (struct sockaddr *)&addr, &len);
                if (fd < 0) {
                        /* system is shutting down */
                        break;
                }

                recv(fd, &type, sizeof(type), 0);
                if (type == AMBA_TEST_MSG_REGISTER) {
                        // receive register spec
                        atd_spec_t spec;
                        recv(fd, &spec, sizeof(spec), 0);
                        LOCK_DATABASE;
                        add_device(fd, &spec);
                        UNLOCK_DATABASE;
                } else {
                        CMSG("Skip wrong register message type %d\n", type);
                }
	}

        return NULL;
}

/* thread entry that manages device heartbeat */
static void* heartbeat_entry(void *arg)
{
        int i, flag = HEARTBEAT_KEEPALIVE;

        while (device_db->is_shuttingdown == 0) {
                for (i = 0; i <= device_db->num_devices_m1; i++) {
                        test_device_t *device = &device_db->device[i];
                        int fd = device->rfd;
                        struct timeval timeout = { .tv_sec = 1 };
                        fd_set fds;
                        if (fd <= 0) continue;

                        // send heartbeat to registered device
                        send(fd, &flag, sizeof(flag), 0);
                        // wait for reply with 1-second timeout
                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);
                        select(fd+1, &fds, NULL, NULL, &timeout);
                        if (FD_ISSET(fd, &fds)) {
                                /* heatbeat recevied: device is healthy */
                                recv(fd, &flag, sizeof(flag), 0);
                        } else {
                                /* heartbeat lost: device is down */
                                remove_device(device);
                        }
                }
                /* sleep until next heartbeat */
                sleep(1);
        }

        return NULL;
}

/* initialize device module */
int device_init(int max_num_devices)
{
        int i;
  
        /* init memory for device database */
        int size = sizeof(test_device_db);
        size += max_num_devices * sizeof(test_device_t);
        device_db = calloc(size, 1);
        device_db->num_devices_m1 = max_num_devices - 1;
        pthread_mutex_init(&device_db->mutex, NULL);
        for (i = 0; i <= device_db->num_devices_m1; i++) {
                pthread_mutex_init(&device_db->device[i].mutex, NULL);
        }

        pthread_create(&device_db->accept_thread, NULL, accept_entry, NULL);
        pthread_create(&device_db->heartbeat_thread, NULL, heartbeat_entry,NULL);

        AmbaHost_init(0, hostapi_log);
	return 0;
}

/* shut-down device module */
int device_shutdown(void)
{
        int i, flag = HEARTBEAT_SHUTDOWN;

        CMSG("Shut down module ...\n");
        device_db->is_shuttingdown = 1;

        /* clean up accept thread so we don't register any new devcie*/
        shutdown(socket_fd, 2);
        pthread_join(device_db->accept_thread, NULL);
 
        /* stop sending heartbeat to devices */
        pthread_join(device_db->heartbeat_thread, NULL);

        /* clean up each registered device */
        for (i = 0; i <= device_db->num_devices_m1; i++) {
                test_device_t *device = &device_db->device[i];
                if (device->status != TEST_DEVICE_STATUS_FREE) {
                        int fd = device->rfd;
                        /* send a dummy package to wake up device */
                        send(fd, &flag, sizeof(flag), 0);
                        /* wait for device to close socket */
                        recv(fd, &flag, sizeof(flag), 0);
                        /* shut down socket locally */
                        shutdown(device->rfd, 2);
                }
        }

        AmbaHost_fini();
        if (device_db != NULL) {
                free(device_db);
                device_db = NULL;
        }
        return 0;
}

int device_get_class(device_handle handle)
{
        test_device_t *device = (test_device_t *)handle;
        return device->class;
}

int device_get_id(device_handle handle)
{
        test_device_t *device = (test_device_t *)handle;
        return (device != NULL) ? device->id : -1;
}

/* the client sends a job to device to run */
int device_post_job(job_ctx_t *job)
{
        test_device_t *device = (test_device_t *)job->device;

        if (device->status == TEST_DEVICE_STATUS_READY) {
                sem_wait(&device->run_sem);
                if (device->status == TEST_DEVICE_STATUS_READY) {
                        memcpy(&device->job, job, sizeof(*job));
                        sem_post(&device->req_sem);
                } else {
                        scheduler_ditch_job(job);
                }
        } else {
                scheduler_ditch_job(job);
        }
        return AT_ERR_OK;
}

/* push a file from server to device */
int device_push(device_handle handle, const char *src, const char *dst)
{
        test_device_t *device = (test_device_t *)handle;
        int err;

        if (device->status == TEST_DEVICE_STATUS_READY) {
                err = AmbaHost_push_file(device->cid, dst, src);
        } else {
                err = AT_ERR_DEV_NOTFOUND;
        }

        return err;
}

/* pull a file from device to server */
int device_pull(device_handle handle, const char *src, const char *dst)
{
        test_device_t *device = (test_device_t *)handle;
        int err;

        if (device->status == TEST_DEVICE_STATUS_READY) {
                err = AmbaHost_pull_file(device->cid, dst, src);
        } else {
                err = AT_ERR_DEV_NOTFOUND;
        }

        return err;
}

/* fill device stats */
int device_fill_stat(atd_stat_t *stat, int max_size)
{
        int  i, idx;
        
        for (i = idx = 0; i <= device_db->num_devices_m1; i++) {
                test_device_t *dev = &device_db->device[i];
                struct timespec now;

                if (dev->status != TEST_DEVICE_STATUS_READY) 
                        continue;
                clock_gettime(CLOCK_MONOTONIC, &now);
                
                stat[idx].id       = dev->id;
                stat[idx].busytime = dev->busytime;
                stat[idx].uptime   = now.tv_sec - dev->uptime;
                stat[idx].spec     = dev->spec;
                stat[idx].num_jobs = dev->num_jobs;
                if (++idx >= max_size) 
                        break;
        }

        return idx;
}

