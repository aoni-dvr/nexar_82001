/**
 *  @file os_api.h
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
 *  @details Definitions & Constants for OS APIs
 *
 */

#ifndef THREADX_OS_API_H
#define THREADX_OS_API_H

#include "ambint.h"
#include "AmbaTypes.h"
#include "AmbaErrorCode.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "cvtask_errno.h"

#define FALSE 0U
#define TRUE 1U

#define OS_NAME                 "THREADX"
#define RTOS_BUILD

#define RX_THREAD_PRIO          30U
#define RX_STACK_SIZE           8192U

extern int32_t ambacv_fd;
/******************************************************************************/
/*                                  timeout                                 */
/******************************************************************************/
#define OS_API_WAIT_FOREVER             0xFFFFFFFFU
#define SCHDR_START_TIMEOUT             5000U
#define FLEXIDAG_WAIT_REPLY_TIMEOUT     5000U

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
#define SCHDR_ERR_0000      (SSP_ERR_BASE | 0x00400000UL)
#define CVTASK_ERR_0000     (SSP_ERR_BASE | 0x00410000UL)

#define AMBA_SCHDR_PRINT_MODULE_ID      ((UINT16)(SCHDR_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */
#define AMBA_CVTASK_PRINT_MODULE_ID     ((UINT16)(CVTASK_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */

void shell_print(const char *fmt);

void console_printS5(
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

void console_printU5(
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

void module_printS5(
    uint16_t module,
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

void module_printU5(
    uint16_t module,
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

/******************************************************************************/
/*                                  File System                               */
/******************************************************************************/
#define THREAD_FILE       AMBA_FS_FILE
#define THREAD_SEEK_CUR   AMBA_FS_SEEK_CUR       /* current              */
#define THREAD_SEEK_SET   AMBA_FS_SEEK_START     /* head                 */
#define THREAD_SEEK_END   AMBA_FS_SEEK_END       /* end                  */

THREAD_FILE *thread_fopen(const char *path, const char *mode);
uint32_t thread_fclose(THREAD_FILE *fp);
uint32_t thread_fread(void *ptr,uint32_t size,uint32_t nmemb,THREAD_FILE *stream);
uint32_t thread_fwrite(const void *ptr,uint32_t size,uint32_t nmemb,THREAD_FILE *stream);
uint64_t thread_ftell(THREAD_FILE *stream);
uint32_t thread_fseek(THREAD_FILE *stream, int64_t offset, int32_t whence);

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
typedef void* (*thread_entry_t)(void *arg);
typedef AMBA_KAL_SEMAPHORE_t    psem_t;
typedef AMBA_KAL_MUTEX_t        pmutex_t;
typedef AMBA_KAL_TASK_t         thread_t;
typedef AMBA_KAL_TASK_t*        tid_t;

#ifndef O_SYNC
#define O_SYNC      4010000U
#endif
#ifndef O_RDONLY
#define O_RDONLY    0U
#endif
#ifndef O_WRONLY
#define O_WRONLY    1U
#endif
#ifndef O_RDWR
#define O_RDWR      2U
#endif
#ifndef PROT_READ
#define PROT_READ   0x4U    /* pages can be read */
#endif
#ifndef PROT_WRITE
#define PROT_WRITE  0x2U    /* pages can be written */
#endif
#ifndef MAP_SHARED
#define MAP_SHARED  0x1U /* Share changes */
#endif
#ifndef MAP_FIXED
#define MAP_FIXED   0x100       /* Interpret addr exactly */
#endif
#ifndef MAP_FAILED
#define MAP_FAILED  NULL
#endif

int32_t ambacv_open(const char * pathname, uint32_t flags);
uint32_t ambacv_ioctl(int32_t fd, uint32_t request, void *data);
uint32_t ambacv_close(int32_t fd);
void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize);
uint32_t ambacv_munmap(const void *start, uint64_t length);

uint32_t ambacv_cache_init(void);
uint32_t ambacv_cache_clean(void *ptr, uint64_t size);
uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size);
uint32_t ambacv_cache_invalidate_phy(void *ptr, uint64_t size, uint64_t addr);
void ambacv_cache_barrier(void);

uint32_t thread_sem_init(psem_t *sem, int32_t pshared, uint32_t value);
uint32_t thread_sem_post(psem_t *sem);
uint32_t thread_sem_wait(psem_t *sem, uint32_t timeout);
uint32_t thread_mutex_init(pmutex_t *mutex);
uint32_t thread_mutex_lock(pmutex_t *mutex);
uint32_t thread_mutex_unlock(pmutex_t *mutex);

uint32_t  thread_create(
    thread_t *thread,
    thread_entry_t entry,
    const void *arg,
    uint32_t priority,
    uint32_t affinity,
    uint32_t stack_size,
    char *stack_buff,
    char *name);
uint32_t  thread_delete(thread_t *thread);
uint32_t thread_should_stop(const thread_t *thread);
tid_t thread_self(void);
uint32_t thread_sleep(uint32_t msec);
/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
static inline uint32_t thread_get_cpu_id(void)
{
    return (uint32_t)0x0U;
}

static inline void typecast(void * pNewType, const void * pOldType)
{
    if(pNewType == NULL) {
        console_printS5("[ERROR] typecast : fun %s", __func__, NULL, NULL, NULL, NULL);
        console_printU5("[ERROR] typecast : line %d", __LINE__, 0U, 0U, 0U, 0U);
        console_printU5("[ERROR] pNewType == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else if(pOldType == NULL) {
        console_printS5("[ERROR] typecast : fun %s", __func__, NULL, NULL, NULL, NULL);
        console_printU5("[ERROR] typecast : line %d", __LINE__, 0U, 0U, 0U, 0U);
        console_printU5("[ERROR] pOldType == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(pNewType, pOldType);
    }
}

static inline void thread_unused(void *ptr)
{
    AmbaMisra_TouchUnused(ptr);
}

static inline uint32_t thread_memcpy(void *pDst, const void *pSrc, size_t num)
{
    uint32_t retcode = ERRCODE_NONE;

    if( AmbaWrap_memcpy(pDst, pSrc, num) != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_MEMCPY_FAIL;
    }

    return retcode;
}

static inline uint32_t thread_memset(void *ptr, int32_t v, size_t n)
{
    uint32_t retcode = ERRCODE_NONE;

    if( AmbaWrap_memset(ptr, v, n) != ERRCODE_NONE) {
        retcode = ERR_DRV_SCHDR_MEMSET_FAIL;
    }

    return retcode;
}

int32_t thread_strcmp(const char *s1, const char *s2);
uint32_t thread_strlen(const char *s);
char *thread_strncpy(char *dest, const char *src, size_t n);
char *thread_strcpy(char *dest, const char *src);

uint32_t sprintf_str(char *str, const char *str_src);

uint32_t snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0);

uint32_t snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1);

uint32_t snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2);

uint32_t snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3);

uint32_t snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4);

uint32_t snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0);

uint32_t snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1);

uint32_t snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2);

uint32_t snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3);

uint32_t snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4);

void fprintf_int(
    THREAD_FILE *fp,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4);

void fprintf_str(THREAD_FILE *fp, const char *format);


#endif  //THREADX_OS_API_H
