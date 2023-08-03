#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include "AmbaTest_msg.h"
#include "ambatestd.h"

#define CMSG(fmt, ...)    log_msg(LL_CRITICAL, "CLIENT: " fmt, ##__VA_ARGS__)
#define IMSG(fmt, ...)    log_msg(LL_NORMAL,   "CLIENT: " fmt, ##__VA_ARGS__)

#define MAX_CONNECTIONS   128
#define LOCK_DATABASE     pthread_mutex_lock(&client_db->mutex)
#define UNLOCK_DATABASE   pthread_mutex_unlock(&client_db->mutex)
#define LOCK_CLIENT(x)    pthread_mutex_lock(&x->mutex)
#define UNLOCK_CLIENT(x)  pthread_mutex_unlock(&x->mutex)

enum {
        ATC_STATUS_FREE = 0,
        ATC_STATUS_TAKEN,
        ATC_STATUS_REGISTERED,
        ATC_STATUS_OPENING,
        ATC_STATUS_READY,
        ATC_STATUS_CLOSING,
        ATC_STATUS_DOWN,
};

typedef struct {
        pthread_mutex_t mutex;            /* client lock                      */
        pthread_t       thread;           /* thread for the client            */
        uint32_t        status;           /* client status                    */
        int             id ;              /* client ID                        */
        int             uid;              /* user ID                          */
        int             socket;           /* socket file descriptor           */
        char            addr[32];         /* client IP address                */
        char            username[32];     /* client username                  */
        atd_spec_t      device_spec;      /* target device spec               */
        int             priority;
        int             device_class;
        int             device_timeout;
        int             device_mode;
        int             job_total;        /* total jobs need to be done       */
        int             job_done;         /* jobs finished so far             */
        volatile int    job_count;        /* count the number of jobs         */
        sem_t           job_sem;          /* to signal the completion of job  */
        device_handle   device;
        uint32_t        uptime;           /* client registration time         */
} test_client_t;

typedef struct {
        int32_t         num_clients_m1;
        uint32_t        client_max;
        uint32_t        curr_id;
        uint32_t        flags;
        char            hostAddr[32];     /* host IP address                  */
        pthread_mutex_t mutex;            /* database lock                    */
        test_client_t   client[0];
} test_client_db;

static int              socket_fd;
static test_client_db*  client_db;

static int send_to_client(test_client_t *clnt, const void *_buf, int size)
{
        const char *buf = (const char *)_buf;
        int pos = 0, fd = clnt->socket;

        if (clnt->status == ATC_STATUS_DOWN)
                return -1;

        while (pos < size) {
                int nbytes = send(fd, buf+pos, size-pos, 0);
                if (nbytes <= 0) {
                        clnt->status = ATC_STATUS_DOWN;
                        CMSG("[%d] client down!\n", clnt->id);
                        return nbytes;
                }
                pos += nbytes;
        }
        return size;
}

static int recv_from_client(test_client_t *clnt, void *_buf, int size)
{
        char *buf = (char *)_buf;
        int pos = 0, fd = clnt->socket;

        if (clnt->status == ATC_STATUS_DOWN)
                return ATC_STATUS_DOWN;

        while (pos < size) {
                int nbytes = recv(fd, buf+pos, size-pos, 0);
                if (nbytes <= 0) {
                        clnt->status = ATC_STATUS_DOWN;
                        CMSG("[%d] client down!\n", clnt->id);
                        return nbytes;
                }
                pos += nbytes;
        }
        return size;
}

static int waitfor_finishing_runs(test_client_t *clnt)
{
        if (clnt->job_count != 0) {
                sem_wait(&clnt->job_sem);
        }
        return 0;
}

static int unlock_device(test_client_t *clnt)
{
        int err = AT_ERR_OK;

        if (clnt->device != NULL) {
                job_ctx_t job;
                job.type  = JOB_TYPE_UNLOCK_DEVICE;
                job.client = clnt;
                job.device = clnt->device;
                err = scheduler_post_job(
                        clnt->device_class,
                        &job,
                        clnt->priority);
                if (err == AT_ERR_OK) {
                        __sync_add_and_fetch(&clnt->job_count, 1);
                        waitfor_finishing_runs(clnt);
                }
        }

        return err;
}
  
static int do_register(test_client_t *clnt, atc_header_t *header)
{
        atc_register_t info;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
        int err;

        if (recv_from_client(clnt, &info, sizeof(info)) <= 0)
                return AT_ERR_CLIENT_DOWN;

        clnt->status = ATC_STATUS_REGISTERED;
        strcpy(clnt->username, info.client_username);
        clnt->uid = user_get_id(clnt->username);
        user_add_client(clnt->uid);

        getpeername(clnt->socket, (struct sockaddr*)&addr, &len);
        inet_ntop(AF_INET, &addr.sin_addr, clnt->addr, sizeof(clnt->addr));
        
        if ((client_db->flags & CLIENT_FLAG_RESTRICTED) &&
            strcmp(client_db->hostAddr, clnt->addr)) {
                err = AT_ERR_CONN_REJECTED;
                CMSG("[%d] rejects %s@%s\n",
                     clnt->id, clnt->username, clnt->addr);
        } else {
                err = AT_ERR_OK;
                if (info.client_ipaddr[0] != 0)
                        strcpy(clnt->addr, info.client_ipaddr);
                CMSG("[%d] created for %s@%s\n",
                     clnt->id, clnt->username, clnt->addr);
        }

        header->ret = err;
        header->len = 0;
        send_to_client(clnt, header, sizeof(*header));
        return err;
} 

static int do_deregister(test_client_t *clnt, atc_header_t *header)
{
        header->ret = AT_ERR_OK;
        header->len = 0;
        send_to_client(clnt, header, sizeof(*header));
        clnt->status = ATC_STATUS_DOWN;

        CMSG("[%d] deregistered\n", clnt->id);
        return AT_ERR_OK;
} 

static int do_open(test_client_t *clnt, atc_header_t *header)
{
        static const char *mode_name[] = {"SINGLE", "MULTI"};
        atc_open_t msg;
        int err = AT_ERR_OK;

        if (recv_from_client(clnt, &msg, sizeof(msg)) <= 0)
                return AT_ERR_CLIENT_DOWN;
        
        if (clnt->status != ATC_STATUS_REGISTERED) {
                CMSG("[%d] open with status(%d)!\n", clnt->id, clnt->status);
                err = AT_ERR_WRONG_STATUS;
        } else {
                int class = scheduler_get_device_class(&msg.device_spec, 0);
                if (class == -1) {
                        CMSG("[%d] failed to open \"%s,%s,%s\"\n", clnt->id,
                             msg.device_spec.device_hw,
                             msg.device_spec.device_os,
                             mode_name[msg.mode]);
                        err = AT_ERR_DEV_NOTFOUND;
                } else {
                        clnt->status = ATC_STATUS_OPENING;
                        __sync_and_and_fetch(&clnt->job_count, 0);
                        sem_init(&clnt->job_sem, 0, 0);
                        clnt->device_mode  = msg.mode;
                        clnt->priority     = msg.priority;
                        clnt->device_spec  = msg.device_spec;
                        clnt->job_total    = msg.num_jobs;
                        clnt->job_done     = 0;
                        clnt->device_class = class;
                        CMSG("[%d] open \"%s,%s,%s\"\n", clnt->id,
                             msg.device_spec.device_hw,
                             msg.device_spec.device_os,
                             mode_name[msg.mode]);

                        if (clnt->device_mode == ATC_CONN_MODE_SINGLE) {
                                job_ctx_t job;
                                job.type   = JOB_TYPE_LOCK_DEVICE;
                                job.client = clnt;
                                job.device = NULL;
                                err = scheduler_post_job(
                                        clnt->device_class,
                                        &job,
                                        clnt->priority);
                                if (err == AT_ERR_OK) {
                                        __sync_add_and_fetch(&clnt->job_count, 1);
                                        waitfor_finishing_runs(clnt);
                                }
                        } else {
                                clnt->device = NULL;
                                clnt->status = ATC_STATUS_READY;
                        }
                }
        }

        header->ret = err;
        header->len = sizeof(msg);
        printf("len is %d\n", header->len);
        LOCK_CLIENT(clnt);
        send_to_client(clnt, header, sizeof(*header));
        send_to_client(clnt, &msg, sizeof(msg));
        UNLOCK_CLIENT(clnt);
        return AT_ERR_OK;
}

static int do_close(test_client_t *clnt, atc_header_t *header)
{
        atc_close_t msg;
        int err = AT_ERR_OK;

        if (recv_from_client(clnt, &msg, sizeof(msg)) <= 0)
                return AT_ERR_CLIENT_DOWN;

         if (clnt->status != ATC_STATUS_READY) {
                CMSG("[%d] closed with status(%d)!\n", clnt->id, clnt->status);
                err = AT_ERR_WRONG_STATUS;
        } else {
                CMSG("[%d] closing\n", clnt->id);
                clnt->status = ATC_STATUS_CLOSING;
                waitfor_finishing_runs(clnt);
                unlock_device(clnt);
                clnt->status = ATC_STATUS_REGISTERED;
        }

        header->ret = err;
        header->len = sizeof(msg);
        LOCK_CLIENT(clnt);
        send_to_client(clnt, header, sizeof(*header));
        send_to_client(clnt, &msg, sizeof(msg));
        UNLOCK_CLIENT(clnt);
        return AT_ERR_OK;
}

static int do_drun(test_client_t *clnt, atc_header_t *header)
{
        job_ctx_t job;

        if (recv_from_client(clnt, &job.request, sizeof(atc_drun_t)) <= 0)
                return AT_ERR_CLIENT_DOWN;

        if (clnt->status != ATC_STATUS_READY) {
                CMSG("[%d] run with status(%d)!\n", clnt->id, clnt->status);
                return AT_ERR_WRONG_STATUS;
        }

        __sync_add_and_fetch(&clnt->job_count, 1);
        job.type   = JOB_TYPE_DRUN;
        job.client = clnt;
        job.device = clnt->device;
        job.header = *header;
        scheduler_post_job(clnt->device_class, &job, clnt->priority);

        CMSG("[%d] run --> \"%s\"\n", clnt->id, job.request.command);
        return AT_ERR_OK;
}

static int do_push(test_client_t *clnt, atc_header_t *header)
{
        atc_push_t msg;
        int  size, socket = clnt->socket;
        char buf[1024];

        if (recv_from_client(clnt, &msg, sizeof(msg)) <= 0)
                return AT_ERR_CLIENT_DOWN;

        if (clnt->device_mode == ATC_CONN_MODE_SINGLE) {
                char tmpfile[32];
                int  tfd;

                /* wait for all runs to finish to ensure sequentiality */
                waitfor_finishing_runs(clnt);

                /* receive data from client to a local tmp file */
                sprintf(tmpfile, ".push-%d", clnt->id);
                tfd = open(tmpfile, O_CREAT | O_WRONLY | O_TRUNC, 0600);
                size = msg.len;
                while (size > 0) {
                        int nbytes = (size < sizeof(buf)) ? size: sizeof(buf);
                        nbytes = recv(socket, buf, nbytes, 0);
                        assert(nbytes == write(tfd, buf, nbytes));
                        size -= nbytes;
                }
                close(tfd);

                /* push the tmp file to device */
                device_push(clnt->device, tmpfile, msg.dst);

                /* remove tmp file */
                remove(tmpfile);

                header->ret = AT_ERR_OK;
        } else {
                header->ret = AT_ERR_REQ_INVALID;
                /* thrown away data from client */
                size = msg.len;
                while (size > 0) {
                        int nbytes = (size < sizeof(buf)) ? size: sizeof(buf);
                        nbytes = recv(socket, buf, nbytes, 0);
                        size -= nbytes;
                }
        }

        LOCK_CLIENT(clnt);
        header->len = sizeof(msg);
        send_to_client(clnt, header, sizeof(*header));
        send_to_client(clnt, &msg, sizeof(msg));
        UNLOCK_CLIENT(clnt);
        return AT_ERR_OK;
}

static int do_pull(test_client_t *clnt, atc_header_t *header)
{
        atc_pull_t msg;

        if (recv_from_client(clnt, &msg, sizeof(msg)) <= 0)
                return AT_ERR_CLIENT_DOWN;

        if (clnt->device_mode == ATC_CONN_MODE_SINGLE) {
                char buf[1024], tmpfile[32];
                int size, nbytes, fd;

                /* wait for all runs to finish to ensure sequentiality */
                waitfor_finishing_runs(clnt);

                /* fetch file from device and save it to a local tmp file */
                sprintf(tmpfile, ".pull-%d", clnt->id);
                if (!device_pull(clnt->device, msg.src, tmpfile)) {
                        struct stat st;
                        stat(tmpfile, &st);
                        size = st.st_size;
                } else {
                        size = 0;
                }

                LOCK_CLIENT(clnt);
                /* send back header */
                header->ret = size ? AT_ERR_OK : AT_ERR_FILE_NOTFOUND;
                send_to_client(clnt, header, sizeof(*header));

                /* send back atc_pull_t message */
                msg.len = size;
                send_to_client(clnt, &msg, sizeof(msg));

                /* send back file contents */
                fd = open(tmpfile, O_RDONLY);
                while (size > 0) {
                        nbytes = (size < sizeof(buf)) ? size : sizeof(buf);
                        nbytes = read(fd, buf, nbytes);
                        send_to_client(clnt, buf, nbytes);
                        size -= nbytes;
                }
                close(fd);
                UNLOCK_CLIENT(clnt);

                /* remove local tmp file */
                remove(tmpfile);
        } else {
                header->ret = AT_ERR_REQ_INVALID;
                header->len = sizeof(msg);
                LOCK_CLIENT(clnt);
                send_to_client(clnt, header, sizeof(*header));
                send_to_client(clnt, &msg, sizeof(msg));
                UNLOCK_CLIENT(clnt);
        }

        return AT_ERR_OK;
}

static int do_stat(test_client_t *clnt, atc_header_t *header)
{
        char *base;
        at_stat_t *stat;
        int pos;

        base = malloc(1024*1024);
        stat = (at_stat_t*) base;
        memset(base, 0, sizeof(at_stat_t));

        scheduler_fill_stat(stat);
        pos = sizeof(at_stat_t);

        stat->device_stat_offset = pos;
        stat->device_num = device_fill_stat((atd_stat_t*)(base+pos), 1024);
        pos += stat->device_num * sizeof(atd_stat_t);

        stat->client_stat_offset = pos;
        stat->client_num = client_fill_stat((atc_stat_t*)(base+pos), 1024);
        pos += stat->client_num * sizeof(atc_stat_t);

        stat->user_stat_offset = pos;
        stat->user_num = user_fill_stat((atu_stat_t*)(base+pos), 1024);
        pos += stat->user_num * sizeof(atu_stat_t);

        LOCK_CLIENT(clnt);
        header->len = pos;
        header->ret = AT_ERR_OK;
        send_to_client(clnt, header, sizeof(*header));
        send_to_client(clnt, base, pos);
        UNLOCK_CLIENT(clnt);

        free(base);
        return AT_ERR_OK;
}
        
static void* client_entry(void *arg)
{
        test_client_t *client = (test_client_t*)arg;
	atc_header_t header;
        int fd = client->socket, err = 0;
        
        while (err >= 0 && client->status != ATC_STATUS_DOWN) {
                struct timeval timeout = { .tv_sec = 5 };
                fd_set fds;
                
                /* check for client timeout if there is nothing to run */
                FD_ZERO(&fds);
                FD_SET(fd, &fds);
                select(fd+1, &fds, NULL, NULL, &timeout);
                if (!FD_ISSET(fd, &fds)) {
                        if (client->job_count == 0) {
                                CMSG("[%d] timeout\n", client->id);
                                break;
                        } else {
                                continue;
                        }
                }

                /* recv and process next request */
                err = recv_from_client(client, &header, sizeof(header));
                if (err == -1) {
                        CMSG("[%d] Socket error %d\n", client->id, errno);
                        break;
                } else if (err == 0) {
                        CMSG("[%d] socket closed by client\n", client->id);
                        break;
                }

                //LOCK_CLIENT(client);
                switch(header.type) {
                case AMBA_TEST_MSG_REGISTER:
                        err = do_register(client, &header);
                        break;
                case AMBA_TEST_MSG_DEREGISTER:
                        err = do_deregister(client, &header);
                        break;
                case AMBA_TEST_MSG_OPEN:
                        err = do_open(client, &header);
                        break;
                case AMBA_TEST_MSG_CLOSE:
                        err = do_close(client, &header);
                        break;
                case AMBA_TEST_MSG_PUSH:
                        err = do_push(client, &header);
                        break;
                case AMBA_TEST_MSG_PULL:
                        err = do_pull(client, &header);
                        break;
                case AMBA_TEST_MSG_DRUN:
                        err = do_drun(client, &header);
                        break;
                case AMBA_TEST_MSG_STAT:
                        err = do_stat(client, &header);
                        break;
                default:
                        CMSG("Message type 0x%X not supported!\n", header.len);
                        err = -1;
                        break;
                }
                //UNLOCK_CLIENT(client);
        }

        /* cleanup, in case "close" is not executed */
        waitfor_finishing_runs(client);
        unlock_device(client);

        /* release the client */
        LOCK_CLIENT(client);
        sem_destroy(&client->job_sem);
        close(client->socket);
        client->status = ATC_STATUS_FREE;
        CMSG("[%d] released\n", client->id);
        UNLOCK_CLIENT(client);
        return client;
}

static void* accept_entry(void *arg)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
        int i, offset, top = client_db->num_clients_m1;

	while (1) {
                int fd = accept(socket_fd, (struct sockaddr *)&addr, &len);
                
                /* find a empty client slot */
                LOCK_DATABASE;
                offset = client_db->curr_id;
                for (i = 0; i <= top; i++) {
                        test_client_t *client;
                        struct timespec now;
                        client = &client_db->client[(i + offset) & top];
                        LOCK_CLIENT(client);
                        if (client->status == ATC_STATUS_FREE) {
                                clock_gettime(CLOCK_MONOTONIC, &now);
                                client->status = ATC_STATUS_TAKEN;
                                client->socket = fd;
                                client->id     = client_db->curr_id++;
                                client->uptime = now.tv_sec;
                                 /* cleanup thread resource */
                                if (client->thread) 
                                        pthread_join(client->thread, NULL);
                                pthread_create(
                                        &client->thread, 
                                        NULL,
                                        client_entry,
                                        (void*)client);
                                UNLOCK_CLIENT(client);
                                break;
                        }
                        UNLOCK_CLIENT(client);
                }
                UNLOCK_DATABASE;
	}
        return NULL;
}


int client_init(int max_num_clients, uint32_t flags)
{
        pthread_t accept_thread;
	struct sockaddr_in addr;
        int i, port = AMBATEST_CLIENT_PORT;

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(socket_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
		CMSG("Can't bind to port %d\n", port);
		exit(1);
	}

	if (listen(socket_fd, MAX_CONNECTIONS) < 0) {
		CMSG("Can't listen to port %d\n", port);
		exit(1);
	}
  
        /* init memory for client database */
        int size = sizeof(test_client_db);
        size += max_num_clients * sizeof(test_client_t);
        client_db = calloc(size, 1);
        client_db->num_clients_m1 = max_num_clients - 1;
        pthread_mutex_init(&client_db->mutex, NULL);
        for (i = 0; i <= client_db->num_clients_m1; i++) {
                pthread_mutex_init(&client_db->client[i].mutex, NULL);
        }

        client_db->flags = flags;
        if (client_db->flags & CLIENT_FLAG_RESTRICTED) {
                char hostbuffer[256];
                struct hostent *hent;
                // To retrieve host IP address
                gethostname(hostbuffer, sizeof(hostbuffer));
                hent = gethostbyname(hostbuffer);
                strcpy(client_db->hostAddr,
                       inet_ntoa(*((struct in_addr*)hent->h_addr_list[0])));
        }

	CMSG("listening on port %d from client\n", port);
        pthread_create(&accept_thread, NULL, accept_entry, NULL);

	return 0;
}

int client_shutdown(void)
{
        CMSG("Shut down module ...\n");
        if (client_db != NULL) {
                free(client_db);
                client_db = NULL;
        }
        close(socket_fd);
        return 0;
}

/* fill client stats */
int client_fill_stat(atc_stat_t *stat, int max_size)
{
        int  i, idx;
        struct timespec now;
        
        LOCK_DATABASE;
        clock_gettime(CLOCK_MONOTONIC, &now);
        for (i = idx = 0; i <= client_db->num_clients_m1; i++) {
                test_client_t *client = &client_db->client[i];

                if (client->status < ATC_STATUS_OPENING) 
                        continue;
                
                stat[idx].id        = client->id;
                stat[idx].uptime    = client->uptime;
                stat[idx].spec      = client->device_spec;
                stat[idx].job_total = client->job_total;
                stat[idx].job_done  = client->job_done;
                stat[idx].device_id = device_get_id(client->device);
                strcpy(stat[idx].username, client->username);
                if (++idx >= max_size) 
                        break;
        }
        UNLOCK_DATABASE;

        return idx;
}

int client_is_down(client_handle client)
{
        test_client_t *clnt = (test_client_t*)client;
        int ret;

        if (clnt->status == ATC_STATUS_DOWN || clnt->status == ATC_STATUS_FREE){
                ret = 1;
        } else {
                ret = 0;
        }

        return ret;
}

int client_finish_job(job_ctx_t *job, const char *output, int size)
{
        atc_header_t  *header  = &job->header;
        atc_drun_t    *request = &job->request;
        test_client_t *clnt = (test_client_t*)job->client;

        LOCK_CLIENT(clnt);
        switch (job->type) {
        case JOB_TYPE_LOCK_DEVICE:
                clnt->device = job->device;
                clnt->status = ATC_STATUS_READY;
                IMSG("[%d] locked to device %d\n", 
                     clnt->id, device_get_id(clnt->device));
                break;
        case JOB_TYPE_UNLOCK_DEVICE:
                IMSG("[%d] unlock device %d\n", 
                     clnt->id, device_get_id(clnt->device));
                clnt->device = NULL;
                clnt->status = ATC_STATUS_REGISTERED;
                break;
        case JOB_TYPE_DRUN:
                IMSG("[%d] run <-- \"%s\"\n", clnt->id, request->command);
                clnt->job_done++;
                user_add_jobtime(clnt->uid, job->cputime);
                header->len = size + sizeof(atc_drun_t);
                send_to_client(clnt, header,  sizeof(*header));
                send_to_client(clnt, request, sizeof(*request));
                send_to_client(clnt, output,  size);
                break;
        }
        UNLOCK_CLIENT(clnt);

        return client_clear_job(job);
}

int client_clear_job(job_ctx_t *job)
{
        test_client_t *clnt = (test_client_t*)job->client;

        __sync_sub_and_fetch(&clnt->job_count, 1);
        if (clnt->job_count == 0)
                sem_post(&clnt->job_sem);

        return AT_ERR_OK;
}        
