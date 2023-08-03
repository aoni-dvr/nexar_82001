#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <errno.h>
#include "aipc_user.h"
#include "list.h"

#define THREAD_NUM 			2
#define	QUEUE_SIZE 			100
#define MAX_SVC				100

struct svc_handle {
	int          prog;
	int          vers;
	int          sock;
	unsigned int port;

	pthread_mutex_t lock;
	pthread_t thread;

	struct list_head list;
	AMBA_IPC_PROG_INFO_s    prog_info;
	struct aipc_pkt  *pkt;

	struct aipc_xprt xprt_pool[AIPC_CLIENT_NR_MAX];
	int new_thread;
};


struct threadpool_task {
    void* (*function)(void *);
    void  *arg;
};

struct thread_pool {
    pthread_mutex_t            	lock;
    pthread_t             		*threads;
    pthread_cond_t				notEmpty;
    pthread_cond_t				notFull;
    struct threadpool_task      *queue;
    int                         thread_count;
    int                         queue_size;
    int                         queue_count;
    int                         head;
    int                         tail;
};

struct svc_poller {
	int 				svc_pollfd;
	struct svc_handle		*args;
	struct list_head 		list;
};

extern unsigned int rpc_size;
static struct thread_pool *pool;
static volatile int initial = 1;

static LIST_HEAD(head);
static LIST_HEAD(poller_head);

#ifdef RPC_DEBUG
	extern unsigned int read_timer();
	extern void rpc_record_stats(struct aipc_xprt *xprt, AMBA_IPC_COMMUICATION_MODE_e mode);
	extern void rpc_proc_open(void);
#endif

/*
 * find a service and return its handle
 */
static struct svc_handle *find_svc(int prog, int vers)
{
	struct svc_handle *handle;

	list_for_each_entry(handle, &head, list)
		if (handle->prog == prog && handle->vers == vers)
			return handle;

	return NULL;
}

/*
 * find a service and return its poller
 */
static struct svc_poller *find_poller(int prog, int vers)
{
	struct svc_poller *poller;
	struct svc_handle *svc;

	list_for_each_entry(poller, &poller_head, list) {
		svc = (struct svc_handle *)poller->args;
		if (svc->prog == prog && svc->vers == vers)
			return poller;
	}
	return NULL;
}

/*
 * thread that handles incoming SVC requests from clients
 */
static void* svc_thread_func(void *arg)
{
	struct svc_handle  *svc = (struct svc_handle *)arg;
	struct aipc_msg *msg = &svc->pkt->msg;
	AMBA_IPC_SVC_RESULT_s ret[1];
	int len;

	memset(ret, 0, sizeof(ret));
	ret->pResult = malloc(rpc_size);

	if(ret->pResult == NULL) {
		printf("%s can't get memory for svc result\n", __func__);
	}

	do {
		pthread_mutex_lock(&svc->lock);
		len = nl_recv_pkt(svc->sock, svc->pkt, rpc_size, AIPC_WAIT_TIME);

		if(len <0 && (errno != 0)){
			printf("svc_thread_func receving error: %s\n", strerror(errno));
			continue;
		}
		if (svc->prog_info.ProcNum > 0) {
			int i;
			struct aipc_xprt *xprt = NULL;
			for (i = 0; i < AIPC_CLIENT_NR_MAX; i++) {
				if (!svc->xprt_pool[i].private) {
					xprt = &svc->xprt_pool[i];
					*xprt = svc->pkt->xprt;
					xprt->private = (unsigned long) svc;
					break;
				}
			}
			if (xprt) {
				#ifdef RPC_DEBUG
					svc->pkt->xprt.lk_to_lu_end = read_timer();
					rpc_record_stats(&svc->pkt->xprt, svc->prog_info.pProcInfo[msg->u.call.proc-1].Mode);
				#endif
				if( msg->u.call.proc <= svc->prog_info.ProcNum && msg->u.call.proc > 0 ){
					memset(ret->pResult, 0, rpc_size);
				// call the corresponding procedure
					svc->prog_info.pProcInfo[msg->u.call.proc-1].Proc(msg->parameters, ret);
					ambaipc_svc_sendreply(msg, xprt, ret);
				}
				else{	//can't find the procedure
					ret->Status = AMBA_IPC_REPLY_PARA_INVALID;
					ret->Mode = AMBA_IPC_SYNCHRONOUS;
					ambaipc_svc_sendreply(msg, xprt, ret);
				}
			}
			else
				printf("ERROR!! reached AIPC_CLIENT_NR_MAX!!\n");
		}
		pthread_mutex_unlock(&svc->lock);
	} while(svc->new_thread == 1);

	free(ret->pResult);
	return NULL;
}

/*
 * thread from the thread pool will execute this function
 */
static void threadpool_addtask(struct thread_pool *pool, void* (*function)(void *), void *arg)
{
        int status;
      	status = pthread_mutex_lock(&pool->lock);
      	if( status != 0 ){
            printf("Add Task Mutex Lock Error: status is 0x%x\n", status);
        }
        while( (pool->queue_count + 1) > pool->queue_size ){    // check whether the queue is full.
           pthread_cond_wait(&pool->notFull, &pool->lock);
        }

        /* Add task to the circular queue */
        pool->queue[pool->tail].function = function;
        pool->queue[pool->tail].arg = arg;
        pool->tail = (pool->tail+1) % pool->queue_size;
        pool->queue_count++;
        pthread_cond_signal(&pool->notEmpty);
        status = pthread_mutex_unlock(&pool->lock);
        if( status != 0 ){
            printf("Add Task Mutex Unlock Error: status is 0x%x\n", status);
        }
}
/*
 * thread from the thread pool will execute this function
 */
static void* threadpool_func(void *arg)
{
   	struct thread_pool *pool = (struct thread_pool *)arg;
	struct svc_handle *svc;
    struct threadpool_task task;
    int status;

    while(1) {
        status = pthread_mutex_lock(&pool->lock);
        if( status != 0 ){
            printf("ThreadPoolFunc Mutex Lock Error: status is 0x%x\n", status);
        }
        while(pool->queue_count == 0){
            pthread_cond_wait(&pool->notEmpty, &pool->lock); /* wait for other threads adding the task in the queue */
        }
        svc = (struct svc_handle *)pool->queue[pool->head].arg;
        /* Choose a task from the head of the queue */
        while( svc == NULL ) {  // Skip the unregistered svc
            pool->queue_count--;
            pool->head = (pool->head+1) % pool->queue_size;
            svc = (struct svc_handle *)pool->queue[pool->head].arg;
        }
        task.function = pool->queue[pool->head].function;
        task.arg = pool->queue[pool->head].arg;
        pool->queue_count--;
        pool->head = (pool->head+1) % pool->queue_size;
        pthread_cond_signal(&pool->notFull);
        status = pthread_mutex_unlock(&pool->lock);
        if( status != 0 ){
            printf("ThreadPoolFunc Mutex Unlock Error: status is 0x%x\n", status);
        }
        /* Execute the task */
        task.function(task.arg);
    }

    return NULL;
}
/*
 * clear the client requests for the svc in the threadpool
 */
static void threadpool_clear(struct svc_handle *svc)
{
    int i;
    struct svc_handle *svc_pool;
    pthread_mutex_lock(&pool->lock);
    /* search any client request for this svc still in the task queue */
    for(i=pool->head; i<pool->queue_count; i++){
        svc_pool = (struct svc_handle *) pool->queue[i].arg;
        if(svc_pool == svc){
            pool->queue[i].arg = NULL;  //clear the svc
        }
    }
    pthread_mutex_unlock(&pool->lock);
}
/*
 * delete the thread pool
 */
static void threadpool_free(struct thread_pool *pool)
{
	int i;
	if(pool->threads){
		for(i=0; i<pool->thread_count; i++){
			pthread_cancel(pool->threads[i]);
			pthread_join(pool->threads[i], NULL);
		}
	}
	free(pool->threads);
	if(pool->queue){
		free(pool->queue);
	}
	pthread_mutex_destroy(&pool->lock);
	free(pool);
}
/*
 * create the thread pool
 */
static void threadpool_create(int thread_num, int queue_size)
{
    int i;
    unsigned int status;
    pthread_mutexattr_t mta;

    pool = malloc(sizeof(struct thread_pool));

    /* Initialize */
    pool->thread_count = thread_num;
    pool->queue_size = queue_size;
    pool->queue_count = 0;
    pool->head = pool->tail = 0;
    pool->threads = malloc(thread_num*sizeof(pthread_t));
    pthread_mutexattr_init(&mta);
    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&pool->lock, &mta);
	pthread_cond_init(&pool->notEmpty, NULL);
	pthread_cond_init(&pool->notFull, NULL);
    pool->queue = malloc(queue_size*sizeof(struct threadpool_task));

    /* Create worker threads */
    for(i=0; i<thread_num; i++){
        status = pthread_create(&pool->threads[i], 0, threadpool_func, pool);
        if(status != 0){
        	printf("threadpool: creat thread failed\n");
            goto err;
        }
    }

    return;

    err:
        if(pool){
            threadpool_free(pool);
        }
}
/*
 * monitor each socket of svcs
 */
static void* svc_monitor_socket()
{
	struct svc_poller *entry;
	fd_set set;
	struct timeval timeout;
	int fd = 0, ret;
	timeout.tv_sec = 0xFFFFFFFF;
	timeout.tv_usec = 0;

	while(1){
		FD_ZERO(&set);
		list_for_each_entry(entry, &poller_head, list){
			FD_SET(entry->svc_pollfd, &set);

			/* find the maximum fd number */
			if(fd < entry->svc_pollfd){
				fd = entry->svc_pollfd;
			}
		}
		ret = select(fd+1, &set, NULL, NULL, &timeout);
		if(ret != 0){
			list_for_each_entry(entry, &poller_head, list){
				if(FD_ISSET(entry->svc_pollfd, &set)){
					threadpool_addtask(pool, &svc_thread_func, (void *) (entry->args));
				}
			}
		}
	}
	return NULL;

}
/*
 * send binder message.
 */
static void send_binder_pkt(struct svc_handle *svc, int cmd, char *name, AMBA_IPC_PROG_INFO_s *prog_info)
{
	struct aipc_pkt *pkt = svc->pkt;
	AMBA_IPC_COMMUICATION_MODE_e *mode;
	int len = 0, i;

	pkt->xprt.client_addr = AIPC_HOST_LINUX;
	pkt->xprt.client_port = svc->port;
	pkt->xprt.server_addr = AIPC_HOST_LINUX;
	pkt->xprt.server_port = AIPC_BINDING_PORT;
	pkt->msg.type = AIPC_MSG_CALL;
	pkt->msg.u.call.prog = svc->prog;
	pkt->msg.u.call.vers = svc->vers;
	pkt->msg.u.call.proc = cmd;

	switch(cmd) {
		case AIPC_BINDER_REGISTER:
			pkt->msg.parameters[0] = prog_info->ProcNum;
			mode = (AMBA_IPC_COMMUICATION_MODE_e *) calloc(prog_info->ProcNum, sizeof(AMBA_IPC_COMMUICATION_MODE_e));
			for (i=0; i<prog_info->ProcNum; i++){
				*(mode+i) = prog_info->pProcInfo[i].Mode;
			}
			memcpy(&pkt->msg.parameters[1], mode, prog_info->ProcNum*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
			len = strlen(name) + 1;
			strcpy((char*)(&pkt->msg.parameters[1+prog_info->ProcNum]), name);
			printf("binder2 %d %s\n", len, name);
			free(mode);
			nl_send_pkt(svc->sock, pkt, RPC_PARAM_SIZE + prog_info->ProcNum*sizeof(AMBA_IPC_COMMUICATION_MODE_e) + len);
			break;
		case AIPC_BINDER_UNREGISTER:
			nl_send_pkt(svc->sock, pkt, 0);
			break;
		case AIPC_BINDER_FIND:
			nl_send_pkt(svc->sock, pkt, RPC_PARAM_SIZE);
			break;
		default:
			printf("%s unknown binder pkt\n", __func__);

	}
}

/*
 * register a service with binder and create a new thread to handle incoming
 * calls from client
 */
int ambaipc_svc_register(int prog, int vers, char *name, AMBA_IPC_PROG_INFO_s *info, int new_thread)
{
	struct svc_handle  *svc;
	pthread_mutexattr_t mta;
	pthread_t polling_t;
	struct svc_poller *entry;
	int status;

	if(initial && !new_thread){
		threadpool_create(THREAD_NUM, QUEUE_SIZE);
		pthread_create(&polling_t, 0, &svc_monitor_socket, NULL);	// using a dedicated thread to polling every socket of svcs
		initial = 0;
	}

	svc = find_svc(prog, vers);

	if(svc) {
		printf("%s svc is already registered\n", __func__);
		return -AMBA_IPC_REREGISTERED_SERVER;
	}

	svc = calloc(1, sizeof(struct svc_handle) + rpc_size);

	if(svc == NULL) {
		printf("%s Can't get memory for svc allocation\n", __func__);
		return -AMBA_IPC_SERVER_MEM_ALLOC_FAILED;
	}

	svc->prog = prog;
	svc->vers = vers;
	svc->new_thread = new_thread;
	svc->pkt  = nl_alloc_pkt(rpc_size);
	svc->sock = nl_open_socket(&svc->port);
	send_binder_pkt(svc, AIPC_BINDER_REGISTER, name, info);

	nl_recv_pkt(svc->sock, svc->pkt, rpc_size, AIPC_WAIT_TIME);
	status = svc->pkt->msg.parameters[0];

	if(status) {
		goto done;
	}

	svc->prog_info.ProcNum = info->ProcNum;
	svc->prog_info.pProcInfo = malloc(info->ProcNum*sizeof(AMBA_IPC_PROC_s));
	// record the procedure information
	memcpy(svc->prog_info.pProcInfo, info->pProcInfo, info->ProcNum*sizeof(AMBA_IPC_PROC_s));

#ifdef RPC_DEBUG
	// check wether rpc_debug is enabled
	rpc_proc_open();
#endif

	pthread_mutexattr_init(&mta);
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&svc->lock, &mta);

	if(new_thread){	// create a new thread to run this svc.
		pthread_create(&svc->thread, 0, svc_thread_func, (void *)svc);
		// args need to be freed
	}
	else{ // use default threads, so the socket of this svc should be monitored.
		entry = malloc(sizeof(struct svc_poller));
		entry->args = svc;
		entry->svc_pollfd = svc->sock;	// record the socket of the new svc.
		list_add(&entry->list, &poller_head);
	}

	list_add(&svc->list, &head);

done:
	return -status;
}

/*
 * unregister a service with binder and terminate the corresponding svc thread
 */
int ambaipc_svc_unregister(int prog, int vers)
{
	struct svc_handle *svc = find_svc(prog, vers);
	struct svc_handle fake_svc;
	struct svc_poller *poller = find_poller(prog, vers);
	int status;

	if (!svc) {
		// The original svc doesn't exist, so replace it with a fake svc.
		// ask binder whether svc is unregistered.
		fake_svc.prog = prog;
		fake_svc.vers = vers;
		fake_svc.pkt = nl_alloc_pkt(4); // it only carries one answer.
		fake_svc.sock = nl_open_socket(&fake_svc.port);
		send_binder_pkt(&fake_svc, AIPC_BINDER_FIND, NULL, NULL);

		//recv the response
		nl_recv_pkt(fake_svc.sock, fake_svc.pkt, 4, AIPC_WAIT_TIME);
		status = fake_svc.pkt->msg.parameters[0];

		if(status) {
			// the svc exists so we need to unregister it.
			send_binder_pkt(&fake_svc, AIPC_BINDER_UNREGISTER, NULL, NULL);
		}
		nl_close_socket(fake_svc.sock);
		nl_free_pkt(fake_svc.pkt);
		return 0;
	}

	send_binder_pkt(svc, AIPC_BINDER_UNREGISTER, NULL, NULL);
	if( svc->new_thread ){
		pthread_cancel(svc->thread);
		pthread_join(svc->thread, NULL);
	}
	else{
			// clear all the client requests for this svc in the pool
		 threadpool_clear(svc);
	}
	pthread_mutex_destroy(&svc->lock);

	list_del(&svc->list);
	nl_close_socket(svc->sock);
	nl_free_pkt(svc->pkt);
	free(svc->prog_info.pProcInfo);
	free(svc);

	if(poller){	// stop polling for this svc
		list_del(&poller->list);
		free(poller);
	}
	return 0;
}

int ambaipc_svc_sendreply(struct aipc_msg *msg, struct aipc_xprt *xprt, AMBA_IPC_SVC_RESULT_s *result)
{
	struct svc_handle *svc = (struct svc_handle*)xprt->private;
	int status;
	// svc_thread_func will lock mutex first, so use trylock to prevent double lock
	status = pthread_mutex_trylock(&svc->lock);
	if (msg) {
		msg->u.reply.status = result->Status;
		msg->type = AIPC_MSG_REPLY;
	}
	if (result->Mode == AMBA_IPC_SYNCHRONOUS) {
		struct aipc_pkt *pkt = svc->pkt;
		pkt->xprt = *xprt;
		memcpy(pkt->msg.parameters, result->pResult, result->Length);
		nl_send_pkt(svc->sock, pkt, result->Length);
	}
	xprt->private = 0;
	if(status == 0){
		pthread_mutex_unlock(&svc->lock);
	}
	return 0;
}
