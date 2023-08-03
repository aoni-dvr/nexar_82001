/**
 *  @file os_api.c
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
 *  @details OS APIs
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "os_api.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"
#include "ambacv_kal.h"
#include "dram_mmap.h"
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif
#include <sched.h>

#ifdef USE_AMBA_KAL

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/

void console_printS5(const char *fmt,
                     const char *argv0,
                     const char *argv1,
                     const char *argv2,
                     const char *argv3,
                     const char *argv4)
{
    module_printS5(0U, fmt, argv0, argv1, argv2, argv3, argv4);
}

void console_printU5(const char *fmt,
                     uint64_t argv0,
                     uint64_t argv1,
                     uint64_t argv2,
                     uint64_t argv3,
                     uint64_t argv4)
{
    module_printU5(0U, fmt, argv0, argv1, argv2, argv3, argv4);
}

void module_printS5(uint16_t module, const char *fmt,
                    const char *argv0,
                    const char *argv1,
                    const char *argv2,
                    const char *argv3,
                    const char *argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = snprintf_str5(&str_line[str_pos], sizeof(str_line), fmt,
                            argv0,
                            argv1,
                            argv2,
                            argv3,
                            argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            if(module == 0U) {
                AmbaPrint_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_ModulePrintUInt5(module, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            }
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        if(module == 0U) {
            AmbaPrint_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(module, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
        }
    }
}

void module_printU5(uint16_t module, const char *fmt,
                    uint64_t argv0,
                    uint64_t argv1,
                    uint64_t argv2,
                    uint64_t argv3,
                    uint64_t argv4)
{
    uint32_t  str_start = 0U, str_end = 0U, str_pos = 0U;
    char str_line[512];

    str_pos = snprintf_uint5(&str_line[str_pos], sizeof(str_line), fmt,
                             (uint32_t)argv0,
                             (uint32_t)argv1,
                             (uint32_t)argv2,
                             (uint32_t)argv3,
                             (uint32_t)argv4);

    while ( (str_line[str_end] != '\0') && (str_end <= str_pos) ) {
        if (str_line[str_end] == '\n') {
            str_line[str_end] = '\0';
            if(module == 0U) {
                AmbaPrint_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_ModulePrintUInt5(module, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
            }
            str_start = str_end + 1U;
        }
        str_end++;
    }

    if(str_start < str_end) {
        if(module == 0U) {
            AmbaPrint_PrintUInt5(&str_line[str_start], 0U, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(module, &str_line[str_start], 0U, 0U, 0U, 0U, 0U);
        }
    }
}

void shell_print(const char *fmt)
{
    console_printS5(fmt, NULL, NULL, NULL, NULL, NULL);
}
/******************************************************************************/
/*                                  File System                               */
/******************************************************************************/
THREAD_FILE *thread_fopen(const char *path, const char *mode)
{
    uint32_t ret;
    THREAD_FILE *ifp;

    ret = AmbaFS_FileOpen(path, mode, &ifp);
    if(ret != 0U) {
        console_printU5("[ERROR] thread_fopen() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }

    return ifp;
}

uint32_t thread_fclose(THREAD_FILE *fp)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    ret = AmbaFS_FileClose(fp);
    if(ret != 0U) {
        retcode = ERR_DRV_SCHDR_FILE_CLOSE_FAIL;
        console_printU5("[ERROR] thread_fclose() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t thread_fread(
    void *ptr,
    uint32_t size,
    uint32_t nmemb,
    THREAD_FILE *stream)
{
    uint32_t ret;
    uint32_t count;

    ret = AmbaFS_FileRead(ptr, size, nmemb, stream, &count);
    if(ret != 0U) {
        console_printU5("[ERROR] thread_fread() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }

    return count;
}

uint32_t thread_fwrite(
    const void *ptr,
    uint32_t size,
    uint32_t nmemb,
    THREAD_FILE *stream)
{
    uint32_t ret;
    uint32_t count;
    void *ptr_void;

    typecast(&ptr_void, &ptr);
    ret = AmbaFS_FileWrite(ptr_void, size, nmemb, stream, &count);
    if(ret != 0U) {
        console_printU5("[ERROR] thread_fwrite() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    return count;
}

uint64_t thread_ftell(THREAD_FILE *stream)
{
    uint32_t ret;
    uint64_t fpos;
    UINT64 fpos_64 = 0U;

    ret = AmbaFS_FileTell(stream, &fpos_64);
    if(ret != 0U) {
        console_printU5("[ERROR] thread_ftell() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    fpos = (uint64_t)fpos_64;
    return fpos;
}

uint32_t thread_fseek(THREAD_FILE *stream, int64_t offset, int32_t whence)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    ret = AmbaFS_FileSeek(stream, offset, whence);
    if(ret != 0U) {
        retcode = ERR_DRV_SCHDR_FILE_SEEK_FAIL;
        console_printU5("[ERROR] thread_fseek() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
#if defined(AMBACV_KERNEL_SUPPORT)
int32_t ambacv_open(const char * pathname, uint32_t flags)
{
    int32_t fd = -1;
    int32_t ret;
    uint32_t version = 0U;
    uint32_t retcode = ERRCODE_NONE;

    fd = open(pathname, flags);
    if(fd >= 0) {
        retcode = schdr_check_version(fd, &version);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] ambacv_open() : schdr_check_version fail retcode = 0x%x", retcode, 0U, 0U, 0U, 0U);
            ret = close(fd);
            if (ret != 0) {
                console_printU5("[ERROR] ambacv_open() : close fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
            }
            fd = -1;
        }
    } else {
        console_printU5("[ERROR] ambacv_open() : open fail fd = %d errno = %d", fd, errno, 0U, 0U, 0U);
    }
    return fd;
}


uint32_t ambacv_ioctl(int32_t fd, uint32_t request, void *data)
{
    int32_t ret;

    ret = ioctl(fd, request, data);
    if (ret != 0) {
        console_printU5("[ERROR] ambacv_ioctl() : ioctl fail request = 0x%x, ret = 0x%x errno = %d", request, ret, errno, 0U, 0U);
    }
    return (uint32_t)ret;
}

#if defined(ENABLE_AMBA_MAL)
void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize)
{
    uint32_t ret = 0U;
    void *ptr = NULL;

    (void) start;
    (void) prot;
    (void) flags;
    (void) fd;
    if ((offsize >= schdr_core_info.PhysAddr) && (offsize < (schdr_core_info.PhysAddr + schdr_core_info.Size))) {
        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, offsize, length, AMBA_MAL_ATTRI_CACHE, &ptr);
    } else if ((offsize >= schdr_cma_info.PhysAddr) && (offsize < (schdr_cma_info.PhysAddr + schdr_cma_info.Size))) {
        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, offsize, length, AMBA_MAL_ATTRI_CACHE, &ptr);
    } else {
        /* Do nothing */
    }
    (void) ret;
    if (ptr == NULL) {
        console_printU5("[ERROR] ambacv_mmap() : AmbaMAL_Map fail pa 0x%x size 0x%x ret = 0x%x", offsize, length, ret, 0U, 0U);
    }
    return ptr;
}

uint32_t ambacv_munmap(const void *start, uint64_t length)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;
    void *pstart = NULL;

    typecast(&pstart, &start);
    if(schdr_core_info.Type != AMBA_MAL_TYPE_INVALID) {
        ret = AmbaMAL_Unmap(AMBA_MAL_ID_CV_SCHDR, pstart, length);
    } else {
        ret = AmbaMAL_Unmap(AMBA_MAL_ID_CV_SYS, pstart, length);
    }

    if (ret != 0) {
        console_printU5("[ERROR] ambacv_munmap() : AmbaMAL_Unmap fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_UNMAP_FAIL;
    }
    return retcode;
}
#else
void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize)
{
    void *ptr = NULL;
    void *pstart = NULL;

    typecast(&pstart, &start);
    ptr = mmap(pstart, length, prot, flags, fd, offsize);
    if (ptr == MAP_FAILED) {
        console_printU5("[ERROR] ambacv_mmap() : mmap fail pa 0x%x size 0x%x errno = %d ", offsize, length, errno, 0U, 0U);
    }
    return ptr;
}

uint32_t ambacv_munmap(const void *start, uint64_t length)
{
    uint32_t retcode = ERRCODE_NONE;
    int ret;
    void *pstart = NULL;

    typecast(&pstart, &start);
    ret = munmap(pstart, length);
    if (ret != 0) {
        console_printU5("[ERROR] ambacv_munmap() : munmap fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_UNMAP_FAIL;
    }
    return retcode;
}
#endif

uint32_t ambacv_close(int32_t fd)
{
    uint32_t retcode = ERRCODE_NONE;
    int ret;

    ret = close(fd);
    if (ret != 0) {
        console_printU5("[ERROR] ambacv_close() : close fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t ambacv_cache_init(void)
{
    return ERRCODE_NONE;
}

uint32_t ambacv_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_clean() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            mem_region.base = (uint64_t)ambacv_v2p(ptr);
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_CLEAN, &mem_region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_invalidate() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            mem_region.base = (uint64_t)ambacv_v2p(ptr);
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &mem_region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate_phy(void *ptr, uint64_t size, uint64_t addr)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    (void) ptr;
    if(size != 0U) {
        if (ambacv_fd < 0) {
            ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        }
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] ambacv_cache_invalidate_phy() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } else {
            mem_region.base = (uint64_t)addr;
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &mem_region);
        }
    }
    return retcode;
}

void ambacv_cache_barrier(void)
{
    static uint32_t barrier = 0U;

    (void) barrier;
    barrier = 1U;
}
#endif

uint32_t thread_sem_init(psem_t *sem, int32_t pshared, uint32_t value)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    (void) pshared;
    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
        console_printU5("[ERROR] thread_sem_init() : sem == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_SemaphoreCreate(sem, NULL, value);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
            console_printU5("[ERROR] thread_sem_init() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_sem_post(psem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
        console_printU5("[ERROR] thread_sem_post() : sem == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_SemaphoreGive(sem);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
            console_printU5("[ERROR] thread_sem_post() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_sem_wait(psem_t *sem, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        console_printU5("[ERROR] thread_sem_wait() : sem == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_SemaphoreTake(sem, timeout);
        if(ret != 0U) {
            if(ret == KAL_ERR_TIMEOUT) {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT;
            } else {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
            }
            console_printU5("[ERROR] thread_sem_wait() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_mutex_init(pmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
        console_printU5("[ERROR] thread_mutex_init() : mutex == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_MutexCreate(mutex, NULL);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
            console_printU5("[ERROR] thread_mutex_init() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_mutex_lock(pmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        console_printU5("[ERROR] thread_mutex_lock() : mutex == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_MutexTake(mutex, AMBA_KAL_WAIT_FOREVER);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            console_printU5("[ERROR] thread_mutex_lock() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_mutex_unlock(pmutex_t *mutex)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(mutex == NULL) {
        retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
        console_printU5("[ERROR] thread_mutex_unlock() : mutex == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_MutexGive(mutex);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
            console_printU5("[ERROR] thread_mutex_unlock() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_create(thread_t *thread, thread_entry_t entry, const void *arg,
                       uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                       char *name)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;
    AMBA_KAL_TASK_ENTRY_f tsk_entry;

    (void) arg;
    (void) affinity;
    if((thread == NULL) || (entry == NULL)) {
        retcode = ERR_DRV_SCHDR_THREAD_CREATE_FAIL;
        console_printU5("[ERROR] thread_create() : arg invalidate", 0U, 0U, 0U, 0U, 0U);
    } else {
        typecast(&tsk_entry, &entry);
        ret = AmbaKAL_TaskCreate(
                  thread,
                  name,
                  priority,
                  (AMBA_KAL_TASK_ENTRY_f)tsk_entry,
                  arg,
                  stack_buff,
                  stack_size,
                  1U);

        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_THREAD_CREATE_FAIL;
            console_printU5("[ERROR] thread_create() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_delete(thread_t *thread)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        console_printU5("[ERROR] thread_delete() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_TaskDelete(thread);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
            console_printU5("[ERROR] thread_delete() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        }
    }
    return retcode;
}

uint32_t thread_should_stop(const thread_t *thread)
{
    (void) thread;
    return ERRCODE_NONE;
}

tid_t thread_self(void)
{
    tid_t Task = 0;
    tid_t *pTask = NULL;

    pTask = &Task;
    AmbaKAL_TaskIdentify(&pTask);
    return Task;
}

uint32_t thread_get_cpu_id(void)
{
    uint32_t cpu_id = 0U;

#ifdef PACE_DIAG
    // Not supported in PACE
#else /* !PACE_DIAG */
    getcpu(&cpu_id, NULL);
#endif /* ?PACE_DIAG */

    return cpu_id;
}

uint32_t thread_sleep(uint32_t msec)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;

    ret = AmbaKAL_TaskSleep(msec);
    if(ret != 0U) {
        console_printU5("[ERROR] thread_sleep() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_THREAD_SLEEP_FAIL;
    }
    return retcode;
}

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
int32_t thread_strcmp(const char *s1, const char *s2)
{
    return AmbaUtility_StringCompare(s1, s2, AmbaUtility_StringLength(s1));
}

uint32_t thread_strlen(const char *s)
{
    return AmbaUtility_StringLength(s);
}

char *thread_strncpy(char *dest, const char *src, size_t n)
{
    uint32_t CopyLength;
    uint32_t i;

    CopyLength = AmbaUtility_StringLength(src);
    if (CopyLength > n) {
        // overflow condition
        CopyLength = n;
    }
    for (i = 0; i < CopyLength; i++) {
        dest[i] = src[i];
    }

    if (CopyLength < n) {
        dest[CopyLength] = '\0';
    }

    return dest;
}

char *thread_strcpy(char *dest, const char *src)
{
    SIZE_t len;

    len = AmbaUtility_StringLength(src) + 1U;
    AmbaUtility_StringCopy(dest, len, src);
    return dest;
}

uint32_t sprintf_str(char *str, const char *str_src)
{
    size_t len;

    len = AmbaUtility_StringLength(str_src);
    AmbaUtility_StringCopy(str, len + 1U, str_src);

    return len;
}

uint32_t snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    uint32_t rval;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

uint32_t snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    uint32_t rval;
    uint32_t args[2];

    args[0] = arg0;
    args[1] = arg1;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 2, args);

    return rval;
}

uint32_t snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2)
{
    uint32_t rval;
    uint32_t args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 3, args);

    return rval;
}

uint32_t snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3)
{
    uint32_t rval;
    uint32_t args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 4, args);

    return rval;
}

uint32_t snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t rval;
    uint32_t args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 5, args);

    return rval;
}

uint32_t snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0)
{
    uint32_t rval;

    rval = AmbaUtility_StringPrintStr(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

uint32_t snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1)
{
    const char *args[2];

    args[0] = arg0;
    args[1] = arg1;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 2, args);
}

uint32_t snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2)
{
    const char *args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 3, args);
}

uint32_t snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3)
{
    const char *args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 4, args);
}

uint32_t snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4)
{
    const char *args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 5, args);
}

int32_t fprintf_int(
    FILE *fp,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    return fprintf(fp, format, arg0, arg1, arg2, arg3, arg4);
}

int32_t fprintf_str(FILE *fp, const char *format)
{
    return fprintf(fp, "%s", format);
}
#else
/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/

void console_printS5(const char *fmt,
                     const char *argv0,
                     const char *argv1,
                     const char *argv2,
                     const char *argv3,
                     const char *argv4)
{
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
}

void console_printU5(const char *fmt,
                     uint64_t argv0,
                     uint64_t argv1,
                     uint64_t argv2,
                     uint64_t argv3,
                     uint64_t argv4)
{
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
}

void module_printS5(uint16_t module, const char *fmt,
                    const char *argv0,
                    const char *argv1,
                    const char *argv2,
                    const char *argv3,
                    const char *argv4)
{
    (void) module;
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
}

void module_printU5(uint16_t module, const char *fmt,
                    uint64_t argv0,
                    uint64_t argv1,
                    uint64_t argv2,
                    uint64_t argv3,
                    uint64_t argv4)
{
    (void) module;
    printf(fmt, argv0, argv1, argv2, argv3, argv4);
}

void shell_print(const char *fmt)
{
    printf(fmt);
}

/******************************************************************************/
/*                                  File System                               */
/******************************************************************************/


/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
#if defined(AMBACV_KERNEL_SUPPORT)
int32_t ambacv_open(const char * pathname, uint32_t flags)
{
    int32_t fd = -1;
    uint32_t version = 0U;
    uint32_t retcode = ERRCODE_NONE;

    fd = open(pathname, flags);
    if(fd >= 0) {
        retcode = schdr_check_version(fd, &version);
        if(retcode != ERRCODE_NONE) {
            close(fd);
            fd = -1;
        }
    }

    return fd;
}

uint32_t ambacv_ioctl(int32_t fd, uint32_t request, void *data)
{
    uint32_t retcode;

    retcode = ioctl(fd, request, data);
    return retcode;
}

#if defined(ENABLE_AMBA_MAL)
void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize)
{
    uint32_t ret = 0U;
    void *ptr = NULL;

    (void) start;
    (void) prot;
    (void) flags;
    (void) fd;
    if ((offsize >= schdr_core_info.PhysAddr) && (offsize < (schdr_core_info.PhysAddr + schdr_core_info.Size))) {
        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, offsize, length, AMBA_MAL_ATTRI_CACHE, &ptr);
    } else if ((offsize >= schdr_cma_info.PhysAddr) && (offsize < (schdr_cma_info.PhysAddr + schdr_cma_info.Size))) {
        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, offsize, length, AMBA_MAL_ATTRI_CACHE, &ptr);
    } else {
        /* Do nothing */
    }
    (void) ret;
    if (ptr == NULL) {
        console_printU5("[ERROR] ambacv_mmap() : AmbaMAL_Map fail pa 0x%x size 0x%x ret = 0x%x", offsize, length, ret, 0U, 0U);
    }
    return ptr;
}

uint32_t ambacv_munmap(const void *start, uint64_t length)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t ret;
    void *pstart = NULL;

    typecast(&pstart, &start);
    if(schdr_core_info.Type != AMBA_MAL_TYPE_INVALID) {
        ret = AmbaMAL_Unmap(AMBA_MAL_ID_CV_SCHDR, pstart, length);
    } else {
        ret = AmbaMAL_Unmap(AMBA_MAL_ID_CV_SYS, pstart, length);
    }

    if (ret != 0) {
        console_printU5("[ERROR] ambacv_munmap() : AmbaMAL_Unmap fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_UNMAP_FAIL;
    }
    return retcode;
}
#else
void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize)
{
    void *ptr = NULL;
    void *pstart = NULL;

    pstart = start;
    ptr = mmap(pstart, length, prot, flags, fd, offsize);

    return ptr;
}

uint32_t ambacv_munmap(const void *start, uint64_t length)
{
    int ret;
    void *pstart = NULL;

    pstart = start;
    ret = munmap(pstart, length);

    return ret;
}
#endif

uint32_t ambacv_close(int32_t fd)
{
    int ret;

    ret = close(fd);

    return ret;
}

uint32_t ambacv_cache_init(void)
{
    return ERRCODE_NONE;
}

uint32_t ambacv_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_clean() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            mem_region.base = (uint64_t)ambacv_v2p(ptr);
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_CLEAN, &mem_region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_invalidate() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            mem_region.base = (uint64_t)ambacv_v2p(ptr);
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &mem_region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate_phy(void *ptr, uint64_t size, uint64_t addr)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t mem_region;

    (void) ptr;
    if(size != 0U) {
        if (ambacv_fd < 0) {
            ambacv_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
        }
        if (ambacv_fd < 0) {
            console_printU5("[ERROR] ambacv_cache_invalidate_phy() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
        } else {
            mem_region.base = (uint64_t)addr;
            mem_region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &mem_region);
        }
    }
    return retcode;
}

void ambacv_cache_barrier(void)
{
    static uint32_t barrier = 0U;

    (void) barrier;
    barrier = 1U;
}
#endif

static inline uint64_t timespec2nsec(struct timespec* ts)
{
    return ts->tv_sec * (uint64_t)1000000000L + ts->tv_nsec;
}

static inline void nsec2timespec(struct timespec* ts, uint64_t nsec)
{
    if(nsec == 0L) {
        ts->tv_sec = 0;
        ts->tv_nsec = 0;
    } else {
        ts->tv_sec = nsec / 1000000000L;
        ts->tv_nsec = (nsec % 1000000000L);
    }
}

uint32_t thread_sem_init(psem_t *sem, int32_t pshared, uint32_t value)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    (void) pshared;
    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
        printf("[ERROR] thread_sem_init() : sem == NULL \n");
    } else {
        ret = sem_init(sem, pshared, value);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_INIT_FAIL;
            printf("[ERROR] thread_sem_init() : fail ret = %d \n", ret);
        }
    }
    return retcode;
}

uint32_t thread_sem_post(psem_t *sem)
{
    uint32_t retcode = ERRCODE_NONE;
    int32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
        printf("[ERROR] thread_sem_post() : sem == NULL \n");
    } else {
        ret = sem_post(sem);
        if(ret != 0U) {
            retcode = ERR_DRV_SCHDR_SEM_POST_FAIL;
            printf("[ERROR] thread_sem_post() : fail ret = %d \n", ret);
        }
    }
    return retcode;
}

uint32_t thread_sem_wait(psem_t *sem, uint32_t timeout)
{
    uint32_t retcode = ERRCODE_NONE;
    struct timespec tm;
    uint64_t time;
    int32_t ret;

    if(sem == NULL) {
        retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
        printf("[ERROR] thread_sem_wait() : sem == NULL \n");
    } else {
        if(timeout == OS_API_WAIT_FOREVER) {
            ret = sem_wait( sem);
        } else {
            ret = clock_gettime(CLOCK_REALTIME, &tm);
            if(ret != 0) {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
                printf("[ERROR] thread_sem_wait() : clock_gettime fail ret = %d errno %d \n", ret, errno);
            } else {
                time = timespec2nsec(&tm);
                time += ((uint64_t)timeout*1000000U);
                nsec2timespec(&tm, time);
                ret = sem_timedwait( sem, &tm);
            }
        }

        if ( (ret != 0) && (errno != ETIMEDOUT)) {
            retcode = ERR_DRV_SCHDR_SEM_WAIT_FAIL;
            printf("[ERROR] thread_sem_wait() : fail ret = %d errno %d \n", ret, errno);
        } else {
            if(errno != ETIMEDOUT) {
                retcode = ERRCODE_NONE;
            } else {
                retcode = ERR_DRV_SCHDR_SEM_WAIT_TIMEOUT;
            }
        }
    }
    return retcode;
}

uint32_t thread_create(thread_t *thread, thread_entry_t entry, const void *arg,
                       uint32_t priority, uint32_t affinity, uint32_t stack_size, char *stack_buff,
                       char *name)
{
    uint32_t retcode = ERRCODE_NONE;
    struct sched_param param;
    //preempt & starve low pri tasks, use round-robin for same pri task
    const int policy = SCHED_RR;
    int32_t max_pri_RR = sched_get_priority_max(policy);
    int32_t min_pri_RR = sched_get_priority_min(policy);
    pthread_attr_t attr;
    int32_t Ret = 0;
    cpu_set_t cpuset;
    int32_t i;

    pthread_attr_init(&attr);
    if ((stack_buff) && (stack_size)) {
        pthread_attr_setstack(&attr, stack_buff, stack_size);
    }

    if (thread == NULL) {
        Ret = 1;
    } else {
        Ret = pthread_create(thread, &attr, entry, (void *)arg);
        if (name != NULL) {
            pthread_setname_np(*thread, (const char *)name);
        }
        /*
         * NOTE:
         * The two OS use different range of priority, like these:
         * ThreadX uses:  1 (highest) <-----> 127 (lowest)
         * Linux RR uses: 1 (lowest)  <-----> 99 (highest)
         * (KAL_TASK_PRIORITY_LOWEST = 127 and KAL_TASK_PRIORITY_HIGHEST = 1)
         * Hence, We need to map them for consistency.
         *
         * Because the range of Linux is smaller than the range of ThreadX,
         * the last lower priority which are more than 99 priority
         * will be mapped to lowest priority in Linux RR which is 1.
         *
         * Appendix:
         * There are two categories for Linux scheduling policy:
         * (1) 'Normal' scheduling policies: SCHED_OTHER, SCHED_BATCH and SCHED_IDLE
         * (2) 'Real-time' scheduling policies: SCHED_FIFO and SCHED_RR
         *
         * for normal processes: PR = 20 + NI (NI is nice and ranges from -20 to 19)
         * for real time processes: PR = - 1 - real_time_priority (real_time_priority ranges from 1 to 99)
         *
         */
        if ((int32_t)priority > max_pri_RR) {
            /* Revert priority value to the lowest for Linux RR, ex: 100->1, 101->1 */
            param.sched_priority = min_pri_RR;
        } else if ((int32_t)priority < min_pri_RR) {
            /* Revert priority value to the highest for Linux RR,
             * ex: 0->99 (this may not exist in current setting) */
            param.sched_priority = max_pri_RR;
        } else {
            /* Revert priority value, ex: 1->99, 2->98,..., 98->2, 99->1 */
            param.sched_priority = (max_pri_RR + min_pri_RR) - (int32_t)priority;
        }

#ifdef PACE_DIAG
        // pthread_setschedparam() fails under PACE_DIAG in some cases
#else /* !PACE_DIAG */
        Ret = pthread_setschedparam(*thread, policy, &param);
#endif /* ?PACE_DIAG */

        if(affinity != 0U) {
            CPU_ZERO(&cpuset);
            for (i = 0; i < 4; i++) {
                if (affinity & (1<<i)) {
                    CPU_SET(i, &cpuset);
                }
            }
            pthread_setaffinity_np(*thread, sizeof(cpu_set_t), &cpuset);
        }
    }

    if(Ret != 0) {
        printf("[ERROR] thread_create() : fail Ret = %d \n", Ret);
        retcode = ERR_DRV_SCHDR_THREAD_CREATE_FAIL;
    }

    return retcode;
}

uint32_t thread_delete(thread_t *thread)
{
    if (thread != NULL) {
        pthread_join(*thread, NULL);
        thread = NULL;
    }
    return 0U;
}

uint32_t thread_get_cpu_id(void)
{
    uint32_t cpu_id = 0U;

#ifdef PACE_DIAG
    // Not supported in PACE
#else /* !PACE_DIAG */
    getcpu(&cpu_id, NULL);
#endif /* ?PACE_DIAG */

    return cpu_id;
}

#define BT_DEPTH 32
void log_backtrace(void)
{
    int32_t j, nptrs;
    void *buffer[BT_DEPTH];
    char **strings;

    nptrs = backtrace(buffer, BT_DEPTH);
    printf("depth is %d\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    printf("symbols done %p\n", strings);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(-1);
    }

    for (j = 0; j < nptrs; j++) {
        printf("%s\n", strings[j]);
    }

    free(strings);
    exit(-1);
}

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
int32_t AmbaPrint(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    return 0;
}

uint32_t sprintf_str(char *str, const char *str_src)
{
    size_t len;

    len = strlen(str_src);
    strncpy(str, str_src, len + 1U);

    return len;
}

uint32_t snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0)
{
    return snprintf_str5(str, strbuf_size, format, arg0, NULL, NULL, NULL, NULL);
}

uint32_t snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1)
{
    return snprintf_str5(str, strbuf_size, format, arg0, arg1, NULL, NULL, NULL);
}

uint32_t snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2)
{
    return snprintf_str5(str, strbuf_size, format, arg0, arg1, arg2, NULL, NULL);
}

uint32_t snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3)
{
    return snprintf_str5(str, strbuf_size, format, arg0, arg1, arg2, arg3, NULL);
}

uint32_t snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4)
{
    uint32_t idx = 0,cnt = 0;
    int32_t ret = 0;

    if ( format == NULL ) {
        // no action
    } else {
        for (; format[idx] != '\0';) {
            if (format[idx] == '%') {
                cnt++;
            }
            idx++;
        }
        if(cnt == 5) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3, arg4);
        } else if(cnt == 4) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3);
        } else if(cnt == 3) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2);
        } else if(cnt == 2) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1);
        } else if(cnt == 1) {
            ret = snprintf(str, strbuf_size, format, arg0);
        } else {
            ret = snprintf(str, strbuf_size, format);
        }
    }

    if(ret >= 0) {
        return ret;
    } else {
        return 0;
    }

}

uint32_t snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    return snprintf_uint5(str, strbuf_size, format, arg0, 0U, 0U, 0U, 0U);
}

uint32_t snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    return snprintf_uint5(str, strbuf_size, format, arg0, arg1, 0U, 0U, 0U);
}

uint32_t snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2)
{
    return snprintf_uint5(str, strbuf_size, format, arg0, arg1, arg2, 0U, 0U);
}

uint32_t snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3)
{
    return snprintf_uint5(str, strbuf_size, format, arg0, arg1, arg2, arg3, 0U);
}

uint32_t snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t idx = 0,cnt = 0;
    int32_t ret = 0;

    if ( format == NULL ) {
        // no action
    } else {
        for (; format[idx] != '\0';) {
            if (format[idx] == '%') {
                cnt++;
            }
            idx++;
        }
        if(cnt == 5) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3, arg4);
        } else if(cnt == 4) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2, arg3);
        } else if(cnt == 3) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1, arg2);
        } else if(cnt == 2) {
            ret = snprintf(str, strbuf_size, format, arg0, arg1);
        } else if(cnt == 1) {
            ret = snprintf(str, strbuf_size, format, arg0);
        } else {
            ret = snprintf(str, strbuf_size, format);
        }
    }

    if(ret >= 0) {
        return ret;
    } else {
        return 0;
    }

}

int32_t fprintf_int(
    FILE *fp,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    return fprintf(fp, format, arg0, arg1, arg2, arg3, arg4);
}

int32_t fprintf_str(FILE *fp, const char *format)
{
    return fprintf(fp, "%s", format);
}
#endif
