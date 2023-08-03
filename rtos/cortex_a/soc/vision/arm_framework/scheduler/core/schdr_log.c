/**
 *  @file schdr_log.c
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
 *  @details Scheduler log APIs
 *
 */

#include "os_api.h"
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "dram_mmap.h"
#include "cvapi_logger_interface.h"

static char                     log_filename[64] = "shutdown_flush.log";
static char                     log_prefix[192] = "./";

void* get_next_entry(schdr_log_info_t *info)
{
    uint32_t count = 0U, offset = 0U;
    uint64_t addr = 0UL;
    void *ptr = NULL;

    (void) info;
    if(info == NULL) {
        console_printU5("[ERROR] get_next_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
#if defined(__QNXNTO__)
        count = atomic_add_value(&(info->wridx), 1 );
#elif defined(LINUX_BUILD)
        count = (uint32_t)(__sync_fetch_and_add(&(info->wridx), 1));
#else
        count = info->wridx;
        info->wridx += 1U;
#endif
        offset = (count & info->mask);
        addr =  info->base_daddr + ((uint64_t)info->esize * (uint64_t)offset);
        if(addr != 0U) {
            ptr = ambacv_c2v(addr);
        }
    }

    return ptr;
}

cvlog_perf_entry_t* get_next_perf_entry(schdr_log_info_t *info)
{
    cvlog_perf_entry_t *entry = NULL;
    const void *ptr = NULL;

    if(info == NULL) {
        console_printU5("[ERROR] get_next_perf_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = get_next_entry(info);
        if(ptr != NULL) {
            typecast(&entry,&ptr);
        }
    }
    return entry;
}

void schdr_insert_customer_event(uint32_t event_id, uint32_t sub_index, uint32_t data0, uint32_t data1)
{
    cvlog_perf_entry_t *e;
    schdr_log_info_t  *pMy_linfo;

    if((event_id >= 0xA0U) && (event_id <= 0xAFU)) {
        pMy_linfo = schdr_sys_state.pLinfo;
        if (pMy_linfo != NULL) {
            e = get_next_perf_entry(&pMy_linfo[2]);
            if (e != NULL) {
                e->event                      = (uint8_t)event_id;
                e->cvcore                     = (uint8_t)schdr_get_cluster_id();
                e->sysflow_index              = (uint8_t)sub_index;
                e->time_stamp                 = schdr_get_event_time();
                e->perfmsg.u32data[0]         = data0;
                e->perfmsg.u32data[1]         = data1;
            } /* if (e != NULL) */
        } /* if (pMy_linfo != NULL) */
    } else {
        console_printU5("[ERROR] schdr_insert_customer_event() : event_id 0x%x is out of range ", event_id, 0U, 0U, 0U, 0U);
    }
}

#ifndef DISABLE_ARM_CVTASK
static cvlog_sched_entry_t* get_next_sched_entry(schdr_log_info_t *info)
{
    cvlog_sched_entry_t *entry = NULL;
    const void *ptr = NULL;

    if(info == NULL) {
        console_printU5("[ERROR] get_next_sched_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = get_next_entry(info);
        if(ptr != NULL) {
            typecast(&entry,&ptr);
        }
    }
    return entry;
}
#endif

void schdr_printf(const char *fmt, uint32_t v1, uint32_t v2,
                  uint32_t v3, uint32_t v4, uint32_t v5)
{
    if(fmt == NULL) {
        console_printU5("[ERROR] schdr_printf() : fmt == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
#ifndef DISABLE_ARM_CVTASK
        cvlog_sched_entry_t *e;
        uint64_t addr1 = 0UL,addr2 = 0UL;
        const char *paddr1,*paddr2;

        /* add new entry to logger buffer */
        if (schdr_sys_state.pLinfo != NULL) {
            paddr1 = &__cvtask_format_start;
            paddr2 = &__cvtask_format_end;
            if(paddr1 != paddr2) {
                e = get_next_sched_entry(&schdr_sys_state.pLinfo[1]);
                if (e != NULL) {
                    e->entry_time = cvtask_get_timestamp();
                    typecast(&addr1,&fmt);
                    typecast(&addr2,&paddr1);
                    e->entry_string_offset = (uint32_t)(addr1 - addr2);
                    e->entry_arg1 = v1;
                    e->entry_arg2 = v2;
                    e->entry_arg3 = v3;
                    e->entry_arg4 = v4;
                    e->entry_arg5 = v5;
                }
            }
        }
#endif

        /* output to console */
        /* a leading '.' forces to skip console output */
        if (fmt[0] != '.') {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, fmt, v1, v2, v3, v4, v5);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "\n", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

uint32_t schdr_log_init(uint64_t base,
                        const schedmsg_boot_setup2_t *r,
                        uint64_t cvtask_size,
                        uint64_t sched_size,
                        uint64_t perf_size)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    (void) r;
    if(r == NULL) {
        console_printU5("[ERROR] schdr_log_init() : r == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        schdr_sys_state.linfo_daddr = base;
        ptr = ambacv_c2v(schdr_sys_state.linfo_daddr);
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_LINFO_INVALID;
        } else {
            typecast(&schdr_sys_state.pLinfo,&ptr);
            schdr_sys_state.pLinfo[0].base_daddr = r->arm_cvtask_debug_buffer_daddr;
            schdr_sys_state.pLinfo[0].wptr_daddr = r->arm_cvtask_debug_buffer_wridx_daddr;
            schdr_sys_state.pLinfo[0].mask       = (uint32_t)cvtask_size - 1U;
            schdr_sys_state.pLinfo[0].esize      = (uint32_t)sizeof(cvlog_cvtask_entry_t);

            schdr_sys_state.pLinfo[1].base_daddr = r->arm_sched_debug_buffer_daddr;
            schdr_sys_state.pLinfo[1].wptr_daddr = r->arm_sched_debug_buffer_wridx_daddr;
            schdr_sys_state.pLinfo[1].mask       = (uint32_t)sched_size - 1U;
            schdr_sys_state.pLinfo[1].esize      = (uint32_t)sizeof(cvlog_sched_entry_t);

            schdr_sys_state.pLinfo[2].base_daddr = r->arm_perf_debug_buffer_daddr;
            schdr_sys_state.pLinfo[2].wptr_daddr = r->arm_perf_debug_buffer_wridx_daddr;
            schdr_sys_state.pLinfo[2].mask       = (uint32_t)perf_size - 1U;
            schdr_sys_state.pLinfo[2].esize      = (uint32_t)sizeof(cvlog_perf_entry_t);
            retcode = schdr_log_reset();
        }
    }
    return retcode;
}

uint32_t schdr_log_reattach(uint64_t base)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    schdr_sys_state.linfo_daddr = base;
    ptr = ambacv_c2v(schdr_sys_state.linfo_daddr);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_LINFO_INVALID;
    } else {
        typecast(&schdr_sys_state.pLinfo,&ptr);
    }
    return retcode;
}


uint32_t schdr_log_reset(void)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t i;
    const void *ptr;

    // re-calualte the virtual address according to daddr
    ptr = ambacv_c2v(schdr_sys_state.linfo_daddr);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_LINFO_INVALID;
    } else {
        typecast(&schdr_sys_state.pLinfo,&ptr);
        for (i = 0U; i < 3U; i++) {
            if(schdr_sys_state.pLinfo[i].wptr_daddr != 0U) {
                ptr = ambacv_c2v(schdr_sys_state.pLinfo[i].wptr_daddr);
                typecast(&schdr_sys_state.pLinfo[i].wptr,&ptr);
            } else {
                schdr_sys_state.pLinfo[i].wptr = NULL;
            }
            if(schdr_sys_state.pLinfo[i].wptr != NULL) {
                *schdr_sys_state.pLinfo[i].wptr = 0U;
            }
            if(schdr_sys_state.pLinfo[i].base_daddr != 0U) {
                ptr = ambacv_c2v(schdr_sys_state.pLinfo[i].base_daddr);
                typecast(&schdr_sys_state.pLinfo[i].base,&ptr);
            } else {
                schdr_sys_state.pLinfo[i].base = NULL;
            }
        }
    }
    return retcode;
}

static void flush_log_entry(THREAD_FILE *fp, char *format, char *hotlink, const cvlog_cvtask_entry_t *e)
{
    uint32_t offset;
    char *base;

    (void) e;
    if((fp == NULL) || (e == NULL)) {
        console_printU5("[ERROR] flush_log_entry() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if ((e->entry_string_offset >> 29U) == 0U) {
            base = format;
        } /* if ((e->entry_string_offset >> 29) == 0) */
        else if ((e->entry_string_offset >> 29U) == 1U) {
            base = hotlink;
        } /* if ((e->entry_string_offset >> 29) == 1) */
        else { /* if ((e->entry_string_offset >> 29) >= 1) */
            base = NULL;
        } /* if ((e->entry_string_offset >> 29) >= 1) */

        if (base != NULL) {
            offset = (e->entry_string_offset & 0x1FFFFFFFU);
            base = &base[offset];

            fprintf_int(fp, "<TH-%d:%u> ",
                        e->hdr.entry_flags.src_thread,
                        (uint32_t)e->entry_time, 0U, 0U, 0U);
            fprintf_int(fp, base,
                        e->entry_arg1,
                        e->entry_arg2,
                        e->entry_arg3,
                        e->entry_arg4,
                        e->entry_arg5);

            if (base[thread_strlen(base) - 1U] != '\n') {
                fprintf_str(fp, "\n");
            }
        }
    }
}

static void flush_log_buffer(THREAD_FILE *fp, char *base, const cvlog_buffer_info_t *info)
{
    const cvlog_cvtask_entry_t *e;
    char *format;
    char *hotlink;
    uint32_t i, start, end;
    const char *name;

    (void) info;
    if((fp == NULL) || (base == NULL) || (info == NULL)) {
        console_printU5("[ERROR] flush_log_buffer() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
    } else {
        name = &base[info->buffer_addr_offset];
        typecast(&e,&name);
        format  = &base[info->binary_offset];
        hotlink = &base[info->hotlink_offset];

        start = info->prev_write_index;
        end = info->write_index;

        if (start > end) {
            // wrap around
            for (i = start; i < info->buffer_size_in_entries; i++) {
                flush_log_entry(fp, format, hotlink, &(e[i]));
            }
            start = 0U;
        } else {
            // force wrap around
            for (i = end; i < info->buffer_size_in_entries; i++) {
                if (e[i].entry_string_offset != 0U) {
                    flush_log_entry(fp, format, hotlink, &(e[i]));
                }
            }
        }

        for (i = start; i < end; i++) {
            flush_log_entry(fp, format, hotlink, &(e[i]));
        }
    }
}

static uint32_t do_flush_log(THREAD_FILE *fp, uint64_t state_addr)
{
    uint32_t  retcode = ERRCODE_NONE;
    cvlogger_state_t *state;
    uint32_t i;
    const void *ptr;

    if(fp == NULL) {
        console_printU5("[ERROR] do_flush_log() : fp == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FILE_DESC_INVALID;
    } else {
        ptr = ambacv_c2v(state_addr);
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_LINFO_INVALID;
        } else {
            typecast(&state,&ptr);
            fprintf_str(fp, "##################################################\n");
            fprintf_str(fp, "                 sysflow table\n");
            fprintf_str(fp, "##################################################");
            for (i = 0U; i < schdr_sys_state.sysflow_numlines; i++) {
                fprintf_int(fp, "\n    %3d: uuid=%-4d ",
                            i, (uint32_t)schdr_sys_state.pSysFlow[i].uuid, 0U, 0U, 0U);
                fprintf_str(fp, schdr_sys_state.pSysFlow[i].cvtask_name);
            }

            fprintf_str(fp, "\n\n");

            if (state->orcsod.cvtask_log.buffer_addr_offset != state->orcvp.cvtask_log.buffer_addr_offset) {
                fprintf_str(fp, "========sod_cvtask========\n");
                flush_log_buffer(fp, (char*)state, &(state->orcsod.cvtask_log));

                fprintf_str(fp, "========sod_sched ========\n");
                flush_log_buffer(fp, (char*)state, &(state->orcsod.sched_log));
            }

            fprintf_str(fp, "======== vp_cvtask========\n");
            flush_log_buffer(fp, (char*)state, &(state->orcvp.cvtask_log));

            fprintf_str(fp, "======== vp_sched ========\n");
            flush_log_buffer(fp, (char*)state, &(state->orcvp.sched_log));

            for (i = 0U; i < state->num_arm; i++) {
                fprintf_int(fp, "========arm%d_cvtask========\n", i, 0U, 0U, 0U, 0U);
                flush_log_buffer(fp, (char*)state, &(state->arm[i].cvtask_log));

                fprintf_int(fp, "========arm%d_sched ========\n", i, 0U, 0U, 0U, 0U);
                flush_log_buffer(fp, (char*)state, &(state->arm[i].sched_log));
            }
        }
    }
    return retcode;
}

uint32_t schdr_startup_log_flush(uint64_t state_addr, uint32_t boot_os)
{
    static const char startup_log_filename[64] = "startup_flush.log";
    char log_path[256] = "";
    THREAD_FILE *fp;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t numwritten;

    if (boot_os != 0U) {
        numwritten = snprintf_str2(&log_path[0], (uint32_t)sizeof(log_path), "%s%s",log_prefix, startup_log_filename);
        (void) numwritten;
        module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                       "Snapshot current log buffer to %s\n",
                       log_path,
                       NULL, NULL, NULL, NULL);
        fp = thread_fopen(log_path, "wb");
        if (fp != NULL) {
            if(do_flush_log(fp, state_addr) != ERRCODE_NONE) {
                console_printU5("[WARNING] schdr_startup_log_flush() : do_flush_log fail ", 0U, 0U, 0U, 0U, 0U);
            }
            if(thread_fclose(fp) != ERRCODE_NONE) {
                console_printU5("[WARNING] schdr_startup_log_flush() : thread_fclose fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

uint32_t schdr_log_flush(uint64_t state_addr, uint32_t boot_os)
{
    THREAD_FILE *fp;
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t numwritten;
    char log_path[256] = "";

    if(cvtask_log_flush(state_addr) != ERRCODE_NONE) {
        console_printU5("[WARNING] schdr_log_flush() : cvtask_log_flush fail ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (boot_os != 0U) {
            numwritten = snprintf_str2(&log_path[0], (uint32_t)sizeof(log_path), "%s%s",log_prefix, log_filename);
            (void) numwritten;
            module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                           "Snapshot current log buffer to %s\n",
                           log_path,
                           NULL, NULL, NULL, NULL);
            fp = thread_fopen(log_path, "wb");
            if (fp == NULL) {
                console_printS5(
                    "[WARNING] schdr_log_flush() : Can not open log file %s!\n",
                    log_path,
                    NULL, NULL, NULL, NULL);
            } else {
                if(do_flush_log(fp, state_addr) != ERRCODE_NONE) {
                    console_printU5("[WARNING] schdr_log_flush() : do_flush_log fail ", 0U, 0U, 0U, 0U, 0U);
                }
                if(thread_fclose(fp) != ERRCODE_NONE) {
                    console_printU5("[WARNING] schdr_log_flush() : thread_fclose fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return retcode;
}

int32_t schdr_set_shutdown_log_name(const char *name)
{
    uint32_t  retcode = ERRCODE_NONE;
    const char *dst_ptr = NULL;

    if (name != NULL) {
        dst_ptr = thread_strcpy(log_filename, name);
    }

    (void) dst_ptr;
    return (int32_t)retcode;
}

void schdr_set_log_prefix(const char *prefix)
{
    const char *dst_ptr = NULL;

    if (prefix != NULL) {
        dst_ptr = thread_strcpy(log_prefix, prefix);
    }

    (void) dst_ptr;
}

