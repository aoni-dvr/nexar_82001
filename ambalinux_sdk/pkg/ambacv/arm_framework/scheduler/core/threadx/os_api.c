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

#include "os_api.h"
#include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaMMU.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "dram_mmap.h"
#include "bsp.h"

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

    str_pos = snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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

    str_pos = snprintf_uint5(&str_line[str_pos], (uint32_t)sizeof(str_line), fmt,
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
    uint32_t Len;
    uint32_t NumWrite = 0U;
    uint32_t Index = 0U;
    uint32_t NumWritten;
    static char NewLinePrintBuf[512] GNU_SECTION_NOZEROINIT;

    Len = (uint32_t)AmbaUtility_StringLength(fmt);
    while (Index < Len) {
        if (fmt[Index] == '\n') {
            NewLinePrintBuf[NumWrite] = '\r';
            NumWrite++;
        }
        NewLinePrintBuf[NumWrite] = fmt[Index];
        Index++;
        NumWrite++;
    }
    NumWritten = AmbaUserShell_Write(NumWrite, &NewLinePrintBuf[0], 0xFFFFFFFFU);
    (void) NumWritten;
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
int32_t ambacv_open(const char * pathname, uint32_t flags)
{
    uint32_t retcode;
    uint32_t version = 0U;
    int32_t fd = 1;

    (void) pathname;
    (void) flags;
    retcode = krn_ambacv_init();
    if(retcode == ERRCODE_NONE) {
        retcode = schdr_check_version(fd, &version);
    }

    if(retcode != ERRCODE_NONE) {
        fd = -1;
    }
    return fd;
}

uint32_t ambacv_ioctl(int32_t fd, uint32_t request, void *data)
{
    uint32_t retcode;

    (void) fd;
    retcode = krn_ambacv_ioctl(NULL, request, data);
    return retcode;
}

void *ambacv_mmap(const void *start, uint64_t length, uint32_t prot, uint32_t flags, int32_t fd, uint64_t offsize)
{
    uint32_t ret;
    ULONG va;
    void *ptr = NULL;

    (void) start;
    (void) length;
    (void) prot;
    (void) flags;
    (void) fd;
    ret = AmbaMMU_PhysToVirt((ULONG)offsize, &va);
    typecast(&ptr, &va);
    if(ret != 0U) {
        console_printU5("[ERROR] ambacv_mmap() : fail ret = 0x%x", ret, 0U, 0U, 0U, 0U);
    }
    return ptr;
}

uint32_t ambacv_munmap(const void *start, uint64_t length)
{
    (void) start;
    (void) length;
    return ERRCODE_NONE;
}

uint32_t ambacv_close(int32_t fd)
{
    (void) fd;
    return ERRCODE_NONE;
}

uint32_t ambacv_cache_init(void)
{
    return ERRCODE_NONE;
}

uint32_t ambacv_cache_clean(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_clean() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            region.base = (uint64_t)ambacv_v2p(ptr);
            region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_CLEAN, &region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t region;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_invalidate() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            region.base = (uint64_t)ambacv_v2p(ptr);
            region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &region);
        }
    }
    return retcode;
}

uint32_t ambacv_cache_invalidate_phy(void *ptr, uint64_t size, uint64_t addr)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_mem_region_t region;

    (void) addr;
    if(size != 0U) {
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            console_printU5("[ERROR] ambacv_cache_invalidate_phy() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            region.base = (uint64_t)ambacv_v2p(ptr);
            region.size = (uint64_t)size;
            retcode = ambacv_ioctl(ambacv_fd, AMBACV_CACHE_INVLD, &region);
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
    AMBA_KAL_TASK_INFO_s taskInfo;

    (void) arg;
    if((thread == NULL) || (entry == NULL) || (stack_buff == NULL) || (stack_size == 0U)) {
        retcode = ERR_DRV_SCHDR_THREAD_CREATE_FAIL;
        console_printU5("[ERROR] thread_create() : arg invalidate", 0U, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaKAL_TaskQuery(thread, &taskInfo);
        if(ret == 0U) {
            if( AmbaKAL_TaskTerminate(thread) != ERRCODE_NONE) {
                console_printU5("[ERROR] thread_create() : AmbaKAL_TaskTerminate fail", 0U, 0U, 0U, 0U, 0U);
            }
            if( AmbaKAL_TaskDelete(thread) != ERRCODE_NONE) {
                console_printU5("[ERROR] thread_create() : AmbaKAL_TaskDelete fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
        typecast(&tsk_entry, &entry);
        ret = AmbaKAL_TaskCreate(
                  thread,
                  name,
                  priority,
                  (AMBA_KAL_TASK_ENTRY_f)tsk_entry,
                  arg,
                  stack_buff,
                  stack_size,
                  0U);

        if(ret == 0U) {
            ret = AmbaKAL_TaskSetSmpAffinity(thread, affinity);
        }

        if(ret == 0U) {
            ret = AmbaKAL_TaskResume(thread);
        }

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
    AMBA_KAL_TASK_INFO_s TaskInfo;
    uint32_t CheckCnt = 200U;
    uint32_t Leave = 0U;

    if(thread == NULL) {
        retcode = ERR_DRV_SCHDR_THREAD_DELETE_FAIL;
        console_printU5("[ERROR] thread_delete() : arg invalidate ", 0U, 0U, 0U, 0U, 0U);
    } else {
        do {
            if (AmbaKAL_TaskQuery(thread, &TaskInfo) == TX_SUCCESS) {
                CheckCnt--;
                if (CheckCnt == 0U) {
                    Leave = 1U;
                }
            } else {
                Leave = 1U;
            }

            if (Leave != 0U) {
                break;
            }
            if( thread_sleep(10U)  != ERRCODE_NONE) {
                console_printU5("[ERROR] thread_delete() : thread_sleep fail ", 0U, 0U, 0U, 0U, 0U);
                Leave = 1U;
            }
        } while (TaskInfo.TaskState != TX_COMPLETED);
        ret = AmbaKAL_TaskTerminate(thread);

        if(ret == 0U) {
            ret = AmbaKAL_TaskDelete(thread);
        }

        if(TaskInfo.TaskState != TX_COMPLETED) {
            console_printU5("[WARNING] thread_delete() : timeout\n", 0, 0, 0, 0, 0);
        }

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
    tid_t val;

    if( AmbaKAL_TaskIdentify(&val) != ERRCODE_NONE) {
        console_printU5("[ERROR] thread_self() : AmbaKAL_TaskIdentify fail ", 0U, 0U, 0U, 0U, 0U);
    }
    return val;
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
    return (uint32_t)AmbaUtility_StringLength(s);
}

char *thread_strncpy(char *dest, const char *src, size_t n)
{
    size_t CopyLength;
    size_t i;

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

    return (uint32_t)len;
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

static char line[1024] GNU_SECTION_NOZEROINIT;

void fprintf_int(
    THREAD_FILE *fp,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t rval;
    uint32_t Args[5];
    uint32_t num_success;

    Args[0] = arg0;
    Args[1] = arg1;
    Args[2] = arg2;
    Args[3] = arg3;
    Args[4] = arg4;
    rval = AmbaUtility_StringPrintUInt32(line, 1024U, format, 5U, Args);
    if(ERRCODE_NONE != AmbaFS_FileWrite(line, 1U, rval, fp, &num_success)) {
        console_printU5("[ERROR] fprintf_int() : AmbaFS_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
    }
}

void fprintf_str(THREAD_FILE *fp, const char *format)
{
    const char *Args[1];
    uint32_t num_success,rval;

    Args[0] = NULL;
    rval = AmbaUtility_StringPrintStr(line, 1024U, format, 1U, Args);
    if(ERRCODE_NONE != AmbaFS_FileWrite(line, 1U, rval, fp, &num_success)) {
        console_printU5("[ERROR] fprintf_str() : AmbaFS_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
    }
}
