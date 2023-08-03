/**
 *  @file logger.c
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
 *  @details Logger APIs
 *
 */

#include "os_api.h"
#include "dram_mmap.h"
#include "schdrmsg_def.h"
#include "cvapi_logger_interface.h"
#include "schdr.h"
#include "schdr_util.h"
#include "schdr_api.h"
#include "ambacv_kal.h"

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
typedef struct {
    uint32_t index;
    uint32_t curr_time;
    char     *format;
    char     *base;
    uint64_t addr;
    uint64_t size;
} track_t;

static ambacv_log_t log_info;
static track_t  ambacv_track[4];

static void print_dmsg(const track_t *ptrack, THREAD_FILE *ofp, const cvlog_sched_entry_t *e)
{
    char line[512];
    uint32_t  pos = 0U;

    pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[%10u]", (uint32_t)e->entry_time);
    if ((e->hdr.entry_flags_dword & 0xF0000000U) >= 0x80000000U) {
        pos += sprintf_str(&line[pos], "[SCH:");
    } else {
        pos += sprintf_str(&line[pos], "[CVT:");
    }

#if defined(CHIP_CV1)
    if ((e->hdr.entry_flags_dword & 0x10000000U) != 0U) {
        pos += sprintf_str(&line[pos], "VP");
    } else {
        pos += sprintf_str(&line[pos], "SOD");
    }
#else
    pos += sprintf_str(&line[pos], "VP");
#endif

    pos += snprintf_uint1(&line[pos], ((uint32_t)sizeof(line) - pos), ":TH%x] ", e->hdr.entry_flags.src_thread);
    pos += snprintf_uint5(&line[pos], ((uint32_t)sizeof(line) - pos), &ptrack->format[e->entry_string_offset],
                          e->entry_arg1,
                          e->entry_arg2,
                          e->entry_arg3,
                          e->entry_arg4,
                          e->entry_arg5);

    if(ofp != NULL) {
        if(thread_fwrite(line, 1U, pos, ofp) != pos) {
            console_printU5("[ERROR] print_dmsg() : thread_fwrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        shell_print(line);
    }
}

static uint32_t dump_dmsg_lines(const track_t *ptrack, THREAD_FILE *ofp, uint32_t num_lines)
{
    const cvlog_sched_entry_t *e;
    uint32_t ret = 0U;
    uint64_t index = 0UL,index_start = 0UL;
    uint32_t curr_time = 0U, loop = 1U;
    const char *base;

    (void) ptrack;
    if(ptrack == NULL) {
        console_printU5("[ERROR] dump_dmsg() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
    } else {
        /* Find last entry */
        while (loop == 1U) {
            base = &ptrack->base[index];
            typecast(&e,&base);
            if ((e->entry_string_offset == 0U) || (curr_time> e->entry_time)) {
                loop = 0U;
            } else {
                curr_time = e->entry_time;
                index += sizeof(*e);
                if (index >= ptrack->size) {
                    loop = 2U;
                }
            }
        }

        /* Calculate start entry */
        if (index >= ((uint64_t)num_lines * (uint64_t)sizeof(*e))) {
            index_start = index - ((uint64_t)num_lines * (uint64_t)sizeof(*e));
        } else {
            if(loop == 2U) {
                if(((uint64_t)num_lines * (uint64_t)sizeof(*e)) <= ptrack->size) {
                    index_start = ptrack->size - (((uint64_t)num_lines * (uint64_t)sizeof(*e)) - index);
                } else {
                    index_start = index + sizeof(*e);
                }
            } else {
                index_start = 0U;
            }
        }

        /* Dump entry */
        loop = 1U;
        while (loop == 1U) {
            base = &ptrack->base[index_start];
            typecast(&e,&base);
            if ((e->entry_string_offset == 0U) || (index_start == index )) {
                loop = 0U;
            } else {
                print_dmsg(ptrack, ofp, e);
                index_start += sizeof(*e);
                if (index_start >= ptrack->size) {
                    index_start = 0;
                }
            }
        }
    }
    return ret;
}

static uint32_t dump_dmsg(track_t *ptrack, THREAD_FILE *ofp)
{
    const cvlog_sched_entry_t *e;
    uint32_t ret = 0U;
    uint32_t index;
    uint32_t curr_time, loop = 1U;
    const char *base;
    void *ptr;

    if(ptrack == NULL) {
        console_printU5("[ERROR] dump_dmsg() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
    } else {
        index = ptrack->index;
        curr_time = ptrack->curr_time;
        while (loop == 1U) {
            base = &ptrack->base[index];
            typecast(&e,&base);
            if ((e->entry_string_offset == 0U) || ((ptrack->curr_time != 0U) && (curr_time > e->entry_time))) {
                loop = 0U;
            } else {
                print_dmsg(ptrack, ofp, e);
                curr_time = e->entry_time;
                index += (uint32_t)sizeof(*e);
                if (index >= ptrack->size) {
                    index = 0;
                    if(ptrack->curr_time == 0U) {
                        ptrack->curr_time = curr_time;
                    }
                    typecast(&ptr,&ptrack->base);
                    if(ambacv_cache_invalidate_phy(ptr, ptrack->size, ptrack->addr) != ERRCODE_NONE) {
                        console_printU5("[ERROR] dump_dmsg() : ambacv_cache_invalidate fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                    ambacv_cache_barrier();
                }
            }
        }
        if(ptrack->index == index) {
            ret = 0U;
        } else {
            ret = 1U;
        }

        ptrack->index = index;
        ptrack->curr_time = curr_time;
    }
    return ret;
}

static uint32_t do_dump_dmsg(uint32_t mode, THREAD_FILE *ofp, uint32_t num_lines)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t has_new_entry, count;
    uint32_t i,loop = 1U;

    if(num_lines != 0U) {
        for (i = 0U; i < 4U; i++) {
            uint32_t shift;

            if (log_info.schdr_log[i].size != 0UL) {
                shift = (uint32_t)((uint32_t)1U<<i);
                if ((mode & shift) != 0U) {
                    retcode = dump_dmsg_lines(&ambacv_track[i], ofp,num_lines);
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
                if(thread_sleep(25U) != ERRCODE_NONE) {
                    console_printU5("[ERROR] do_dump_dmsg() : thread_sleep fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    } else {
        count = 10U;
        while (loop == 1U) {
            has_new_entry = 0U;
            for (i = 0U; i < 4U; i++) {
                uint32_t shift;

                if (log_info.schdr_log[i].size != 0UL) {
                    shift = (uint32_t)((uint32_t)1U<<i);
                    if ((mode & shift) != 0U) {
                        has_new_entry = has_new_entry + dump_dmsg(&ambacv_track[i], ofp);
                    }
                    if(thread_sleep(25U) != ERRCODE_NONE) {
                        console_printU5("[ERROR] do_dump_dmsg() : thread_sleep fail ", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
            if (has_new_entry != 0U) {
                count = 0U;
            } else {
                count = count + 1U;
                if (count >= 10U) {
                    shell_print("No new entry for one seconds, exit\n");
                    loop = 0U;
                }
            }
        }
    }

    return retcode;
}

static uint32_t start_dmsg(int32_t fd, uint32_t mode, THREAD_FILE *ofp, uint32_t num_lines)
{
    uint32_t retcode = ERRCODE_NONE;
    char *base;
    void *ptr = NULL;
    void *mmap_ptr[4];
    uint64_t mmap_size[4] = {0UL};
    uint32_t i;
    uint64_t offset = 0UL,m_size = 0UL,m_base = 0UL;

    for (i = 0U; i < 4U; i++) {
        mmap_ptr[i] = NULL;
        mmap_size[i] = 0U;
        if (log_info.schdr_log[i].size != 0UL) {
            offset = (log_info.schdr_log[i].base & 0xFFFUL);
            m_size = (log_info.schdr_log[i].size + offset);
            m_base = (log_info.schdr_log[i].base - offset);
            mmap_size[i] = m_size;
            mmap_ptr[i] = ambacv_mmap(NULL, m_size, PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, m_base);
            ptr = mmap_ptr[i];
            if(mmap_ptr[i] == MAP_FAILED) {
                console_printU5("[ERROR] start_dmsg : ptr == MAP_FAILED Pa 0x%llx Size 0x%llx \n", m_base, m_size, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_LOG_MAP_FAIL;
                break;
            } else {
                typecast(&base,&ptr);
                ambacv_track[i].size = log_info.schdr_log[i].size;
                ambacv_track[i].base = &base[offset];
                ambacv_track[i].addr = log_info.schdr_log[i].base;
                typecast(&ptr,&ambacv_track[i].base);
                if(ambacv_cache_invalidate_phy(ptr, ambacv_track[i].size, ambacv_track[i].addr) != ERRCODE_NONE) {
                    console_printU5("[ERROR] start_dmsg() : ambacv_cache_invalidate fail ", 0U, 0U, 0U, 0U, 0U);
                }
                ambacv_cache_barrier();
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        retcode = do_dump_dmsg(mode, ofp, num_lines);
    }

    for (i = 0U; i < 4U; i++) {
        if((mmap_ptr[i] != NULL) && (mmap_size[i] != 0U)) {
            if( ambacv_munmap(mmap_ptr[i], mmap_size[i]) != ERRCODE_NONE) {
                console_printU5("[ERROR] start_dmsg(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t do_start_dmsg(int32_t fd, const ambacv_mem_t *pmem, uint32_t mode, const char *log_file, int32_t num_lines)
{
    uint32_t retcode = ERRCODE_NONE;
    THREAD_FILE *ofp = NULL;
    char *vp_buf = NULL,*sod_buf = NULL;
    const void *ptr;
    const void *vp_mmap_ptr = NULL,*sod_mmap_ptr = NULL;

    (void) pmem;
    if (pmem->vp.size != 0UL) {
        vp_mmap_ptr = ambacv_mmap(NULL, pmem->vp.size, (PROT_READ | PROT_WRITE), MAP_SHARED,
                                  fd, ambacv_c2p(pmem->vp.base));
        ptr = vp_mmap_ptr;
        typecast(&vp_buf,&ptr);
        if(vp_mmap_ptr == MAP_FAILED) {
            console_printU5("[ERROR] do_start_dmsg(): vp_buf == MAP_FAILED ! Pa 0x%llx Size 0x%llx ", ambacv_c2p(pmem->vp.base), pmem->vp.size, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_BIN_MAP_FAIL;
        }
    }

    if (pmem->sod.size != 0UL) {
        sod_mmap_ptr = ambacv_mmap(NULL, pmem->sod.size, (PROT_READ | PROT_WRITE), MAP_SHARED,
                                   fd, ambacv_c2p(pmem->sod.base));
        ptr = sod_mmap_ptr;
        typecast(&sod_buf,&ptr);
        if(sod_mmap_ptr == MAP_FAILED) {
            console_printU5("[ERROR] do_start_dmsg(): vp_buf == MAP_FAILED ! Pa 0x%llx Size 0x%llx ", ambacv_c2p(pmem->sod.base), pmem->sod.size, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_BIN_MAP_FAIL;
        }
    }

    if (retcode == ERRCODE_NONE) {
        if (log_file != NULL) {
            ofp = thread_fopen(log_file, "wb");
            if (ofp == NULL) {
                console_printS5("[ERROR] do_start_dmsg(): Can't open log file %s !\n", log_file, NULL, NULL, NULL, NULL);
                retcode = ERR_DRV_SCHDR_FILE_DESC_INVALID;
            }
        }

        if (retcode == ERRCODE_NONE) {
            retcode = ambacv_ioctl(fd, AMBACV_GET_LOG_INFO, &log_info);
            if (retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] do_start_dmsg(): Can't get visorc log info !\n", 0U, 0U, 0U, 0U, 0U);
            } else {
                retcode = thread_memset(ambacv_track, 0, sizeof(ambacv_track));
                if(retcode == ERRCODE_NONE) {
                    if (log_info.schdr_log[SOD_SCHDR_LOG_INDEX].size != 0UL) { /* Corrected for new format */
                        ambacv_track[0].format = sod_buf;
                        ambacv_track[1].format = sod_buf;
                        ambacv_track[2].format = vp_buf;
                        ambacv_track[3].format = vp_buf;
                    } else {
                        ambacv_track[2].format = vp_buf;
                        ambacv_track[3].format = vp_buf;
                    }

                    retcode = start_dmsg(fd, mode, ofp, (uint32_t)num_lines);
                }
            }
        }

        if (ofp != NULL) {
            if( thread_fclose(ofp) != ERRCODE_NONE) {
                console_printU5("[ERROR] do_start_dmsg(): thread_fclose fail \n", 0U, 0U, 0U, 0U, 0U);
            }
        }

        if (pmem->vp.size != 0UL) {
            if( ambacv_munmap(vp_mmap_ptr, pmem->vp.size) != ERRCODE_NONE) {
                console_printU5("[ERROR] do_start_dmsg(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
        if (pmem->sod.size != 0UL) {
            if( ambacv_munmap(sod_mmap_ptr, pmem->sod.size) != ERRCODE_NONE) {
                console_printU5("[ERROR] do_start_dmsg(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}


int32_t schdr_visorc_dmsg(uint32_t mode, const char *log_file, int32_t num_lines)
{
    ambacv_mem_t mem = {0};
    uint32_t retcode = ERRCODE_NONE;
    int32_t fd = -1;

    (void) log_file;
    fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
    if (fd < 0) {
        console_printU5("[ERROR] schdr_visorc_dmsg(): Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
    } else {
        /*  Get ATT info    */
        retcode = ambacv_att_init();
        if (retcode == ERRCODE_NONE) {
            retcode = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, &mem);
            if (retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_visorc_dmsg(): ioctl AMBACV_GET_MEM_LAYOUT fail !\n", 0U, 0U, 0U, 0U, 0U);
            }
        }

        if (retcode == ERRCODE_NONE) {
            retcode = do_start_dmsg(fd, &mem, mode, log_file, num_lines);
        }

        if( ambacv_close(fd) != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_visorc_dmsg(): ambacv_close fail \n", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return (int32_t)retcode;
}
#endif

