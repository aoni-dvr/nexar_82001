#ifndef __AMBATESTD_H__
#define __AMBATESTD_H__

#include <stdarg.h>
#include "AmbaTest_msg.h"

#define JOB_TYPE_DRUN                   0x00000000
#define JOB_TYPE_LOCK_DEVICE            0x00000001
#define JOB_TYPE_UNLOCK_DEVICE          0x00000002

#define CLIENT_FLAG_RESTRICTED          0x00000001

typedef void* device_handle;
typedef void* client_handle;

typedef struct {
        device_handle device;
        client_handle client;
        int           type;
        atc_header_t  header;
        atc_drun_t    request;
        uint32_t      cputime;
} job_ctx_t;

int device_init(int max_num_devices);
int device_shutdown(void);
int device_post_job(job_ctx_t *ctx);
int device_push(device_handle handle, const char *src, const char *dst);
int device_pull(device_handle handle, const char *src, const char *dst);
int device_get_class(device_handle handle);
int device_get_id(device_handle handle);
int device_fill_stat(atd_stat_t *stat, int max_size);

int client_init(int max_num_clients, uint32_t flags);
int client_shutdown(void);
int client_finish_job(job_ctx_t *ctx, const char *output, int size);
int client_clear_job(job_ctx_t *ctx);
int client_fill_stat(atc_stat_t *stat, int max_size);
int client_is_down(client_handle client);
device_handle client_acquire_device(client_handle client);

int user_init(int max_num_users);
int user_shutdown(void);
int user_get_id(char *username);
int user_add_client(int uid);
int user_add_jobtime(int uid, uint32_t ms);
int user_fill_stat(atu_stat_t *stat, int max_size);

int scheduler_init(void);
int scheduler_shutdown(void);
int scheduler_post_job(int class, job_ctx_t *ctx, uint32_t priority);
int scheduler_pend_job(int class, job_ctx_t *ctx, uint32_t timeout);
int scheduler_ditch_job(job_ctx_t *ctx);
int scheduler_get_device_class(atd_spec_t *spec, int flags);
int scheduler_add_device(device_handle device);
int scheduler_remove_device(device_handle device);
int scheduler_fill_stat(at_stat_t *stat);

/* log-level definition */
enum {
        LL_SILENT = 0,
        LL_CRITICAL,
        LL_MINIMAL,
        LL_NORMAL,
        LL_VERBOSE,
        LL_DEBUG,
        LL_TOTAL
};

int log_init(void);
int log_shutdown(void);
int log_msg(int level, const char *fmt, ...);

#endif  //__AMBATESTD_H__
