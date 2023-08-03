// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cache_kernel.h"
#include "visorc_kernel.h"
#include "schdr_kernel.h"
#include "cvapi_logger_interface.h"
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

static schdr_log_info_t   *krn_linfo;
static uint64_t krn_logformat_addr;
static uint64_t krn_logformat_size;
cvprintf_buffer_info_t *krn_print_info = NULL;
static kernel_memblk_t  kernel_master;
static kernel_memblk_t  rtos_master;
static kernel_memblk_t  dsp_data_master;

void * krn_p2v(uint64_t phy)
{
    uint64_t offset = 0UL;
    void *retval = NULL;

    if(phy == 0U) {
        krn_printU5("[ERROR] krn_p2v() : phy == 0U \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        if ((phy >= kernel_master.buffer_daddr) && (phy < kernel_master.buffer_daddr_end)) {
            offset = phy - kernel_master.buffer_daddr;
            retval = &kernel_master.pBuffer[offset];
        } /* if ((phy >= kernel_master.buffer_daddr) && (phy < kernel_master.buffer_daddr_end)) */
        else if ((phy >= rtos_master.buffer_daddr) && (phy < rtos_master.buffer_daddr_end)) {
            offset = phy - rtos_master.buffer_daddr;
            retval = &rtos_master.pBuffer[offset];
        } /* if ((phy >= rtos_master.buffer_daddr) && (phy < rtos_master.buffer_daddr_end)) */
        else if ((phy >= dsp_data_master.buffer_daddr) && (phy < dsp_data_master.buffer_daddr_end)) {
            offset = phy - dsp_data_master.buffer_daddr;
            retval = &dsp_data_master.pBuffer[offset];
        } /* if ((phy >= dsp_data_master.buffer_daddr) && (phy < dsp_data_master.buffer_daddr_end)) */
        else { /* if (phy is not in [kernel_master, rtos_master]) */
            uint32_t  cache_flag;

            retval = krn_cma_p2v(phy, &cache_flag);
            if (retval == NULL) {
                krn_printU5("[ERROR] krn_p2v() : Unable to find mapping for phys (0x%llx)\n", phy, 0U, 0U, 0U, 0U);
            }
        } /* if (phy is not in [kernel_master, rtos_master]) */
    }
    return retval;
} /* krn_p2v() */

uint64_t krn_v2p(void *virt)
{
    uint64_t  start = 0UL,end = 0UL,target = 0UL;
    uint64_t  offset = 0UL;
    uint64_t  retval = 0UL;

    (void) virt;
    if(virt == NULL) {
        krn_printU5("[ERROR] krn_v2p() : virt == NULL \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        krn_typecast(&target,&virt);
        krn_typecast(&start,&kernel_master.pBuffer);
        krn_typecast(&end,&kernel_master.pBuffer_end);

        if ((target >= start) && (target < end)) {
            offset = (uint64_t) (target - start);
            retval = kernel_master.buffer_daddr + offset;
        }

        if(retval == 0UL) {
            krn_typecast(&start,&rtos_master.pBuffer);
            krn_typecast(&end,&rtos_master.pBuffer_end);
            if ((target >= start) && (target < end)) {
                offset = (uint64_t) (target - start);
                retval = rtos_master.buffer_daddr + offset;
            }
        }

        if(retval == 0UL) {
            krn_typecast(&start,&dsp_data_master.pBuffer);
            krn_typecast(&end,&dsp_data_master.pBuffer_end);
            if ((target >= start) && (target < end)) {
                offset = (uint64_t) (target - start);
                retval = dsp_data_master.buffer_daddr + offset;
            }
        }

        if(retval == 0UL) {
            uint32_t  cache_flag;

            retval = krn_cma_v2p(virt, &cache_flag);
        }

        if(retval == 0UL) {
            krn_printU5("[ERROR] krn_v2p() : Unable to find mapping for virt (%x)\n", target, 0U, 0U, 0U, 0U);
        } /* if (phy is not in [kernel_master, rtos_master]) */
    }

    return retval;
} /* krn_v2p() */

#if defined(ENABLE_AMBA_MAL)
uint64_t krn_p2c(uint64_t pa)
{
    uint64_t ca = 0UL;

    if(pa != 0U) {
        if( AmbaMAL_Phys2Global(pa,&ca) != 0U) {
            krn_printU5("[ERROR] krn_p2c() : fail: pa (0x%llx)\n", pa, 0U, 0U, 0U, 0U);
        }
    }

    return ca;
}

uint64_t krn_c2p(uint64_t ca)
{
    uint64_t pa = 0UL;

    if( AmbaMAL_Global2Phys(ca,&pa) != 0U) {
        krn_printU5("[ERROR] krn_c2p() : fail: ca (0x%llx)\n", ca, 0U, 0U, 0U, 0U);
    }
    return pa;
}
#else
uint64_t krn_p2c(uint64_t pa)
{
    uint64_t ca = 0UL;
    const ambacv_all_mem_t *mem_all;
    uint64_t cv_region_start = 0UL,cv_region_end = 0UL;
    uint64_t cv_att_region_start = 0UL;

    if(pa != 0U) {
        mem_all = krn_ambacv_get_mem();
        cv_region_start = mem_all->cv_region.base;
        cv_region_end = (mem_all->cv_region.base + mem_all->cv_region.size);
        cv_att_region_start = mem_all->cv_att_region.base;
        if( (pa >= cv_region_start) && (pa <= cv_region_end) ) {
            ca = (pa - cv_region_start);
            ca = ca + cv_att_region_start;
        } else {
            krn_printU5("[ERROR] krn_p2c() : fail: pa (0x%x)\n", pa, 0U, 0U, 0U, 0U);
        }
    }

    return ca;
}

uint64_t krn_c2p(uint64_t ca)
{
    uint64_t pa = 0UL;
    const ambacv_all_mem_t *mem_all;
    uint64_t cv_region_start = 0UL;
    uint64_t cv_att_region_start = 0UL,cv_att_region_end = 0UL;

    mem_all = krn_ambacv_get_mem();
    cv_region_start = mem_all->cv_region.base;
    cv_att_region_start = mem_all->cv_att_region.base;
    cv_att_region_end = (mem_all->cv_att_region.base + mem_all->cv_att_region.size);
    if( (ca >= cv_att_region_start) && (ca <= cv_att_region_end) ) {
        pa = (ca - cv_att_region_start);
        pa = pa + cv_region_start;
    } else {
        krn_printU5("[ERROR] krn_c2p() : fail: ca (0x%x)\n", ca, 0U, 0U, 0U, 0U);
    }

    return pa;
}
#endif

void * krn_c2v(uint64_t ca)
{
    uint64_t pa = 0UL;
    void *ptr = NULL;

    pa = krn_c2p(ca);
    if(pa != 0U) {
        ptr = krn_p2v(pa);
    } else {
        krn_printU5("[ERROR]  krn_c2v() : fail: ca (0x%x)\n", ca, 0U, 0U, 0U, 0U);
    }

    return ptr;
}

uint64_t krn_v2c(void *virt)
{
    uint64_t ca = 0UL;
    uint64_t pa = 0UL;

    if(virt == NULL) {
        krn_printU5("[ERROR]  krn_v2c() : virt == NULL \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        pa = krn_v2p(virt);
        ca = krn_p2c(pa);
    }
    return ca;
}

void  krn_mmap_add_master(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
    (void) virt;
    krn_typecast(&kernel_master.pBuffer,&virt);
    kernel_master.pBuffer_end       = &kernel_master.pBuffer[size];
    kernel_master.buffer_cacheable  = is_cacheable;
    kernel_master.buffer_daddr      = phy;
    kernel_master.buffer_daddr_end  = phy + size;
    kernel_master.buffer_size       = size;
} /* krn_mmap_add_master() */

void  krn_mmap_add_rtos(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
    (void) virt;
    krn_typecast(&rtos_master.pBuffer,&virt);
    rtos_master.pBuffer_end         = &rtos_master.pBuffer[size];
    rtos_master.buffer_cacheable    = is_cacheable;
    rtos_master.buffer_daddr        = phy;
    rtos_master.buffer_daddr_end    = phy + size;
    rtos_master.buffer_size         = size;
} /* krn_mmap_add_rtos() */

void  krn_mmap_add_dsp_data(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
    (void) virt;
    krn_typecast(&dsp_data_master.pBuffer,&virt);
    dsp_data_master.pBuffer_end         = &dsp_data_master.pBuffer[size];
    dsp_data_master.buffer_cacheable    = is_cacheable;
    dsp_data_master.buffer_daddr        = phy;
    dsp_data_master.buffer_daddr_end    = phy + size;
    dsp_data_master.buffer_size         = size;
} /* krn_mmap_add_dsp_data() */

static uint32_t krn_cache_clean_unaligned(void *ptr, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t size_align = 0UL,addr_align = 0UL,addr_start = 0UL;

    (void) ptr;
    if(size != 0U) {
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_cache_clean_unaligned() : fail : ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else {
            addr_start = krn_v2p(ptr);
            addr_align = krn_cache_addr_align(addr_start);
            size_align = size + ((uint64_t)(addr_start - addr_align));
            size_align = krn_cache_size_align(size_align);
            retcode = krn_cache_clean(krn_p2v(addr_align), size_align);
            if(retcode != 0U) {
                krn_printU5("[ERROR] krn_cache_clean_unaligned() : fail : ret = 0x%x addr = 0x%x size = 0x%x", retcode, addr_start, size, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t krn_sync_logger_buffer(char *base, uint64_t sync_format_string,
                                       const cvlog_buffer_info_t *info)
{
    uint32_t retcode = ERRCODE_NONE;
    char *tmp_base;
    uint64_t wi_prev = 0UL, wi_curr = 0UL, e_size = 0UL, pos = 0UL, len = 0UL;

    (void) info;
    if((base == NULL) || (info == NULL)) {
        krn_printU5("[ERROR] krn_sync_logger_buffer() : invalid arg \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        tmp_base = &base[info->buffer_addr_offset];
        e_size  = info->entry_size;
        if (sync_format_string == 0UL) {
            wi_prev = info->prev_write_index;
            wi_curr = info->write_index;
        } else {
            wi_prev = 0U;
            wi_curr = info->buffer_size_in_entries;
        }

        /* sync the tail part if there is a wrap-around */
        if (wi_prev > wi_curr) {
            pos = wi_prev * e_size;
            len = (info->buffer_size_in_entries - wi_prev) * e_size;
            retcode = krn_cache_invalidate(&tmp_base[pos], len);
            wi_prev = 0U;
        }

        /* sync the head part if the buffer is not empty */
        if ((retcode == ERRCODE_NONE) && (wi_prev != wi_curr)) {
            pos = wi_prev * e_size;
            len = (wi_curr - wi_prev) * e_size;
            retcode = krn_cache_invalidate(&tmp_base[pos], len);
        }

        /* sync the format string */
        if ((retcode == ERRCODE_NONE) && (sync_format_string != 0UL)) {
            tmp_base = &base[info->binary_offset];
            retcode = krn_cache_invalidate(tmp_base, info->binary_size);
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_logger_state(uint64_t addr, uint32_t sync)
{
    uint32_t retcode = ERRCODE_NONE;
    cvlogger_state_t *state;
    const void *ptr;
    uint32_t i;

    ptr = krn_c2v(addr);
    if(ptr == NULL) {
        krn_printU5("[ERROR] krn_invalidate_logger_state() : ptr = krn_c2v(addr) fail \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        krn_typecast(&state,&ptr);
        retcode = krn_cache_invalidate(state, sizeof(*state));

        if ((retcode == ERRCODE_NONE) && (state->orcsod.perf_log.buffer_addr_offset != state->orcvp.perf_log.buffer_addr_offset)) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcsod.perf_log);
        }

        if ((retcode == ERRCODE_NONE) && (state->orcsod.cvtask_log.buffer_addr_offset != state->orcvp.cvtask_log.buffer_addr_offset)) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcsod.cvtask_log);
        }

        if ((retcode == ERRCODE_NONE) && (state->orcsod.sched_log.buffer_addr_offset != state->orcvp.sched_log.buffer_addr_offset)) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcsod.sched_log);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcvp.perf_log);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcvp.cvtask_log);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_sync_logger_buffer((char*)state, sync, &state->orcvp.sched_log);
        }

        for (i = 0U; i < state->num_arm; i++) {
            if(retcode == ERRCODE_NONE) {
                retcode = krn_sync_logger_buffer((char*)state,sync, &state->arm[i].perf_log);
            }
            if(retcode == ERRCODE_NONE) {
                retcode = krn_sync_logger_buffer((char*)state,sync, &state->arm[i].cvtask_log);
            }
            if(retcode == ERRCODE_NONE) {
                retcode = krn_sync_logger_buffer((char*)state,sync, &state->arm[i].sched_log);
            }

            if(retcode != ERRCODE_NONE) {
                break;
            }
        }
    }
    return retcode;
}

static uint32_t krn_clean_log_buffer(schdr_log_info_t *info)
{
    uint32_t retcode = ERRCODE_NONE;
    char     *base;
    uint64_t esize = 0UL, wi_curr = 0UL, wi_prev = 0UL, len = 0UL;
    void *ptr;
    uint32_t *wptr;

    if(info == NULL) {
        krn_printU5("[ERROR] krn_clean_log_buffer() : info == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        retcode = krn_cache_invalidate(info, sizeof (*info));
        if(retcode == ERRCODE_NONE) {
            ptr = krn_c2v(info->wptr_daddr);
            if(ptr == NULL) {
                krn_printU5("[ERROR] krn_clean_log_buffer() : ptr = krn_c2v(info->wptr_daddr) fail \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_LINFO_INVALID;
            } else {
                krn_typecast(&wptr,&ptr);
                ptr = krn_c2v(info->base_daddr);
                if(ptr == NULL) {
                    krn_printU5("[ERROR] krn_clean_log_buffer() : ptr = krn_c2v(info->base_daddr); fail \n", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_LINFO_INVALID;
                } else {
                    krn_typecast(&base,&ptr);

                    wi_prev = *wptr;
                    wi_curr = ((uint64_t)info->wridx & (uint64_t)info->mask);
                    esize = info->esize;

                    if (wi_prev > wi_curr) {
                        ptr = &base[wi_prev * esize];
                        len = (uint64_t)((uint64_t)info->mask + 1UL - wi_prev) * (uint64_t)esize;
                        retcode = krn_cache_clean_unaligned(ptr, len);
                        wi_prev = 0U;
                    }
                    if ((retcode == ERRCODE_NONE) && (wi_prev != wi_curr)) {
                        ptr = &base[wi_prev * esize];
                        len = (wi_curr - wi_prev) * esize;
                        retcode = krn_cache_clean_unaligned(ptr, len);
                    }
                    if(retcode == ERRCODE_NONE) {
                        *wptr = (uint32_t)wi_curr;
                        retcode = krn_cache_clean(wptr, sizeof(*wptr));
                    }
                }
            }
        }
    }
    return retcode;
}

static uint32_t krn_clean_scheduler_log(void)
{
    uint32_t retcode = ERRCODE_NONE;

    retcode = krn_clean_log_buffer(&krn_linfo[0]);
    if(retcode == ERRCODE_NONE) {
        retcode = krn_clean_log_buffer(&krn_linfo[1]);
    }
    if(retcode == ERRCODE_NONE) {
        retcode = krn_clean_log_buffer(&krn_linfo[2]);
    }

    return retcode;
}

static uint32_t krn_invalidate_run_input(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i,shift;
    uint64_t flag;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_inputs; i++) {
        if (req->InputMemBuf_daddr[i] == 0U) {
            /* skip special case for dependency-only input */
            continue;
        }

        ptr = krn_c2v(req->InputMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_invalidate_run_input() : ptr = krn_c2v(req->InputMemBuf_daddr[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            retcode = krn_cache_invalidate(ptr, sizeof(*membuf));
            shift = (i + 0U);
            flag = ((uint64_t)1UL << shift);
            if ((retcode == ERRCODE_NONE) && ((req->ignore_invalidate_flags & (uint32_t)flag) == 0U)) {
                if (membuf->hdr_fixed.needs_cache_coherency != 0U) {
                    if(membuf->hdr_variable.payload_daddr != 0U) {
                        retcode = krn_cache_invalidate(
                                      krn_c2v(membuf->hdr_variable.payload_daddr),
                                      membuf->hdr_variable.payload_size);
                    }
                }
            }

            if((retcode == ERRCODE_NONE) && (membuf->hdr_variable.payload_daddr != 0U)) {
                retcode = krn_cache_invalidate(krn_c2v(membuf->hdr_variable.payload_daddr),
                                               membuf->hdr_variable.payload_size);
            }

            if ((retcode == ERRCODE_NONE) && (membuf->hdr_variable.framework_reserved == 1U)) {
                retcode = krn_invalidate_logger_state(membuf->hdr_variable.payload_daddr, 0U);
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_invalidate_run_feedback(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i,shift;
    uint64_t flag;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_feedback; i++) {
        if (req->FeedbackMemBuf_daddr[i] == 0U) {
            /* skip special case */
            continue;
        }
        ptr = krn_c2v(req->FeedbackMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_invalidate_run_feedback() : ptr = krn_c2v(req->FeedbackMemBuf_daddr[%d]); fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FEEDBACK_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            retcode = krn_cache_invalidate(ptr, sizeof(*membuf));
            shift = (i + 28U);
            flag = ((uint64_t)1UL << shift);
            if ((retcode == ERRCODE_NONE) && ((req->ignore_invalidate_flags & (uint32_t)flag) == 0U)) {
                if (membuf->hdr_fixed.needs_cache_coherency != 0U) {
                    if(membuf->hdr_variable.payload_daddr != 0U) {
                        retcode = krn_cache_invalidate(
                                      krn_c2v(membuf->hdr_variable.payload_daddr),
                                      membuf->hdr_variable.payload_size);
                    }
                }
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_run_msg(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_messagebuf_t *cvmsg;
    uint32_t i;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_messages; i++) {
        ptr = krn_c2v(req->CVTaskMessage_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_invalidate_run_msg() : ptr = krn_c2v(req->message[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_MSG_BUF_INVALID;
        } else {
            krn_typecast(&cvmsg,&ptr);
            retcode = krn_cache_invalidate(ptr, sizeof(*cvmsg));
            if((retcode == ERRCODE_NONE) && (cvmsg->hdr_variable.message_payload_daddr != 0U)) {
                retcode = krn_cache_invalidate(krn_c2v(cvmsg->hdr_variable.message_payload_daddr),
                                               cvmsg->hdr_variable.message_size);
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_run_output(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_outputs; i++) {
        ptr = krn_c2v(req->OutputMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_invalidate_run_output() : ptr = krn_c2v(req->OutputMemBuf_daddr[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_OUTPUT_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            retcode = krn_cache_invalidate(ptr, sizeof(*membuf));
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_invalidate_run(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_run_t *req;

    (void) msg;
    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_run() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.cvtask_run;
        retcode = krn_invalidate_run_input(req);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_invalidate_run_feedback(req);
        }
        if(retcode == ERRCODE_NONE) {
            retcode = krn_invalidate_run_msg(req);
        }
        if(retcode == ERRCODE_NONE) {
            retcode = krn_invalidate_run_output(req);
        }
    }

    return retcode;
}

static uint32_t krn_fd_invalidate_run_input(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i,shift;
    uint64_t flag;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_inputs; i++) {
        if (req->InputMemBuf_daddr[i] == 0U) {
            /* skip special case for dependency-only input */
            continue;
        }

        ptr = krn_c2v(req->InputMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_fd_invalidate_run_input() : ptr = krn_c2v(req->InputMemBuf_daddr[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_INPUT_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            if( krn_cache_invalidate(ptr, sizeof(*membuf)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            shift = (i + 0U);
            flag = ((uint64_t)1UL << shift);
            if ((retcode == ERRCODE_NONE) && ((req->ignore_invalidate_flags & (uint32_t)flag) == 0U)) {
                if (membuf->hdr_fixed.needs_cache_coherency != 0U) {
                    if(membuf->hdr_variable.payload_daddr != 0U) {
                        if( krn_cache_invalidate(
                                krn_c2v(membuf->hdr_variable.payload_daddr),
                                membuf->hdr_variable.payload_size) != ERRCODE_NONE) {
                            retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                        }
                    }
                }
            }

            if((retcode == ERRCODE_NONE) && (membuf->hdr_variable.payload_daddr != 0U)) {
                if( krn_cache_invalidate(krn_c2v(membuf->hdr_variable.payload_daddr),
                                         membuf->hdr_variable.payload_size) != ERRCODE_NONE) {
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
            }

            if ((retcode == ERRCODE_NONE) && (membuf->hdr_variable.framework_reserved == 1U)) {
                if( krn_invalidate_logger_state(membuf->hdr_variable.payload_daddr, 0U) != ERRCODE_NONE) {
                    krn_printU5("[ERROR] krn_fd_invalidate_run_input() : krn_invalidate_logger_state fail ret 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_INVALID_LOGGER_FAIL;
                }
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_fd_invalidate_run_feedback(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i,shift;
    uint64_t flag;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_feedback; i++) {
        if (req->FeedbackMemBuf_daddr[i] == 0U) {
            /* skip special case */
            continue;
        }
        ptr = krn_c2v(req->FeedbackMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_fd_invalidate_run_feedback() : ptr = krn_c2v(req->FeedbackMemBuf_daddr[%d]); fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_FEEDBACK_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            if( krn_cache_invalidate(ptr, sizeof(*membuf)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            shift = (i + 28U);
            flag = ((uint64_t)1UL << shift);
            if ((retcode == ERRCODE_NONE) && ((req->ignore_invalidate_flags & (uint32_t)flag) == 0U)) {
                if (membuf->hdr_fixed.needs_cache_coherency != 0U) {
                    if(membuf->hdr_variable.payload_daddr != 0U) {
                        if( krn_cache_invalidate(
                                krn_c2v(membuf->hdr_variable.payload_daddr),
                                membuf->hdr_variable.payload_size) != ERRCODE_NONE) {
                            retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                        }
                    }
                }
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_fd_invalidate_run_msg(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_messagebuf_t *cvmsg;
    uint32_t i;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_messages; i++) {
        ptr = krn_c2v(req->CVTaskMessage_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_fd_invalidate_run_msg() : ptr = krn_c2v(req->message[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_MSG_BUF_INVALID;
        } else {
            krn_typecast(&cvmsg,&ptr);
            if( krn_cache_invalidate(ptr, sizeof(*cvmsg)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            if((retcode == ERRCODE_NONE) && (cvmsg->hdr_variable.message_payload_daddr != 0U)) {
                if( krn_cache_invalidate(krn_c2v(cvmsg->hdr_variable.message_payload_daddr),
                                         cvmsg->hdr_variable.message_size) != ERRCODE_NONE) {
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_fd_invalidate_run_output(const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    uint32_t i;
    void *ptr;

    for (i = 0U; i < req->cvtask_num_outputs; i++) {
        ptr = krn_c2v(req->OutputMemBuf_daddr[i]);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_fd_invalidate_run_output() : ptr = krn_c2v(req->OutputMemBuf_daddr[%d]) fail \n", i, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_OUTPUT_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            if( krn_cache_invalidate(ptr, sizeof(*membuf)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
        }

        if(retcode != ERRCODE_NONE) {
            break;
        }
    }

    return retcode;
}

static uint32_t krn_fd_invalidate_run(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_run_t *req;

    (void) msg;
    if(msg == NULL) {
        krn_printU5("[ERROR] krn_fd_invalidate_run() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } else {
        req = &msg->msg.cvtask_run;
        retcode = krn_fd_invalidate_run_input(req);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_fd_invalidate_run_feedback(req);
        }
        if(retcode == ERRCODE_NONE) {
            retcode = krn_fd_invalidate_run_msg(req);
        }
        if(retcode == ERRCODE_NONE) {
            retcode = krn_fd_invalidate_run_output(req);
        }
    }

    return retcode;
}

static uint32_t krn_invalidate_boot_visorc_setup2(const cvprintf_buffer_info_t *info, ambacv_log_t *syslog)
{
    uint32_t retcode = ERRCODE_NONE;
    const debug_buffer_info_t    *buf;
    const char *name0,*name1;
    const char *name_ptr;
    uint32_t  log_loop;
    uint32_t size;
    uint64_t start,end;

    for (log_loop = 0U; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++) {
        buf = &(info->visorc_cvtask_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start, end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
            if (krn_strcmp(name0, "SOD") == 0) {
                syslog->schdr_log[SOD_CVTASK_LOG_INDEX].base =  krn_c2p(buf->base_daddr);
                size = (buf->entry_size * buf->size_in_entries);
                syslog->schdr_log[SOD_CVTASK_LOG_INDEX].size =  size;
            }

            if (krn_strcmp(name0, "VP") == 0) {
                syslog->schdr_log[VP_CVTASK_LOG_INDEX].base =  krn_c2p(buf->base_daddr);
                size = (buf->entry_size * buf->size_in_entries);
                syslog->schdr_log[VP_CVTASK_LOG_INDEX].size =  size;
            }
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
        buf = &(info->visorc_sched_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start,end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
            if (krn_strcmp(name0, "SOD") == 0) {
                syslog->schdr_log[SOD_SCHDR_LOG_INDEX].base =  krn_c2p(buf->base_daddr);
                size = (buf->entry_size * buf->size_in_entries);
                syslog->schdr_log[SOD_SCHDR_LOG_INDEX].size =  size;
            }

            if (krn_strcmp(name0, "VP") == 0) {
                syslog->schdr_log[VP_SCHDR_LOG_INDEX].base =  krn_c2p(buf->base_daddr);
                size = (buf->entry_size * buf->size_in_entries);
                syslog->schdr_log[VP_SCHDR_LOG_INDEX].size =  size;
            }
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
        buf = &(info->visorc_perf_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start, end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
    } /* for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++) */

    return retcode;
}

static uint32_t krn_invalidate_boot_arm_setup2(const cvprintf_buffer_info_t *info)
{
    uint32_t retcode = ERRCODE_NONE;
    const debug_buffer_info_t    *buf;
    const char *name0,*name1;
    const char *name_ptr;
    uint32_t  log_loop;
    uint64_t start,end;

    for (log_loop = 0U; log_loop < SYSTEM_MAX_NUM_CORTEX; log_loop++) {
        buf = &(info->arm_cvtask_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start, end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
        buf = &(info->arm_sched_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start,end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
        buf = &(info->arm_perf_debugbuf[log_loop]);
        if ((buf->base_daddr != 0U) && (buf->entry_size != 0U)) {
            name_ptr = &buf->core_name[0];
            krn_typecast(&name0,&name_ptr);
            name_ptr = &buf->log_type[0];
            krn_typecast(&name1,&name_ptr);
            krn_module_printS5("%4s:%6s: @",name0, name1, NULL, NULL, NULL);
            start = krn_c2p(buf->base_daddr);
            end = krn_c2p(buf->base_daddr) + ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries) - 1UL;
            krn_module_printU5("[0x%llx--0x%llx] (%d)",start,end,
                               ((uint64_t)buf->entry_size * (uint64_t)buf->size_in_entries), 0U, 0U);
        } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
    } /* for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++) */

    return retcode;
}

static uint32_t krn_invalidate_boot_setup2(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup2_t *req;
    const cvprintf_buffer_info_t *info = NULL;
    visorc_init_params_t *sysconfig;
    ambacv_log_t *syslog;
    void *ptr;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_boot_setup2() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.boot_setup2;
        syslog = krn_ambacv_get_log();

        /* Patch the "linfo" area into the same spot that the linux scheduler uses */
        sysconfig = krn_ambacv_get_sysinit();
        sysconfig->arm_linfo_base_daddr = req->sysflow_block_daddr;  /* TODO: Discover why it's used this way */
        retcode = krn_cache_clean(&sysconfig->arm_linfo_base_daddr, sizeof(sysconfig->arm_linfo_base_daddr));

        if(retcode == ERRCODE_NONE) {
            ptr = krn_c2v(req->sysflow_block_daddr);
            if(ptr == NULL) {
                krn_printU5("[ERROR] krn_invalidate_boot_setup2() : ptr = krn_c2v(req->sysflow_block_daddr) fail \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_SYSFLOW_INVALID;
            } else {
                krn_typecast(&krn_linfo,&ptr);
            }
        }

        if(retcode == ERRCODE_NONE) {
            ptr = krn_c2v(req->arm_printf_buffer_info_daddr);
            if(ptr == NULL) {
                krn_printU5("[ERROR] krn_invalidate_boot_setup2() : ptr = krn_c2v(req->arm_printf_buffer_info_daddr) fail \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_LINFO_INVALID;
            } else {
                krn_typecast(&info,&ptr);
                krn_typecast(&krn_print_info,&ptr);
                retcode = krn_cache_invalidate(ptr, sizeof(*info));
            }
        }

        if(retcode == ERRCODE_NONE) {
            krn_logformat_addr = req->printf_string_block_daddr;
            krn_module_printU5("=============== Log Buffer Info ===============\n", 0U, 0U, 0U, 0U, 0U);
            retcode = krn_invalidate_boot_visorc_setup2(info, syslog);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_invalidate_boot_arm_setup2(info);
            }
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_boot_setup3(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup3_t *req;
    cvtable_entry_t  *info;
    const sysflow_entry_t *entry;
    void *ptr;
    uint32_t i;

    (void) msg;
    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_boot_setup3() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.boot_setup3;
        if(req->sysflow_table_daddr != 0U) {
            ptr = krn_c2v(req->sysflow_table_daddr);
            krn_typecast(&entry,&ptr);
            retcode = krn_cache_invalidate(ptr, ((uint64_t)req->sysflow_numlines * (uint64_t)sizeof(*entry)));
        }

        if((retcode == ERRCODE_NONE) && (req->cvtable_base_daddr != 0U)) {
            ptr = krn_c2v(req->cvtable_base_daddr);
            krn_typecast(&info,&ptr);
            retcode = krn_cache_invalidate(ptr, ((uint64_t)req->cvtable_numlines * (uint64_t)sizeof(*info)));

            if(retcode == ERRCODE_NONE) {
                for (i = req->cvtable_numlines; i > 0U; i--) {
                    if(info->CVTable_name_daddr != 0U) {
                        retcode = krn_cache_invalidate(krn_c2v(info->CVTable_name_daddr), CVTABLE_NAME_MAX_LENGTH);
                    }
                    if((info->CVTable_daddr != 0U) && (info->cvtable_size != 0U)) {
                        retcode = krn_cache_invalidate(krn_c2v(info->CVTable_daddr), info->cvtable_size);
                    }
                    info = &info[1];
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_memory_report(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvscheduler_memory_report_t *req;
    visorc_init_params_t *sysconfig;
    const component_build_info_t *entry;
    uint64_t nsize = (uint64_t)NAME_MAX_LENGTH;
    ambacv_all_mem_t *mem_all;
    const void *ptr;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_memory_report() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.memory_report;

        // invalidate default message pool
        if(req->CVTaskMsgPool_daddr != 0U) {
            retcode = krn_cache_invalidate(krn_c2v(req->CVTaskMsgPool_daddr),
                                           sizeof(cvmem_messagepool_t));
        }

        // invalidate cvtask build info
        if((retcode == ERRCODE_NONE) && (req->CVTaskInfoTable_daddr != 0U)) {
            retcode = krn_cache_invalidate(krn_c2v(req->CVTaskInfoTable_daddr),
                                           ((uint64_t)sizeof(component_build_info_t) * (uint64_t)req->sysflow_numlines));
        }

        // invalidate scheduler build info
        if((retcode == ERRCODE_NONE) && (req->SchedInfoTable_daddr != 0U)) {
            retcode = krn_cache_invalidate(krn_c2v(req->SchedInfoTable_daddr),
                                           ((uint64_t)sizeof(component_build_info_t) * (uint64_t)16U));
        }

        // invalidate the component name string
        if((retcode == ERRCODE_NONE) && (req->CVTaskInfoTable_daddr != 0U)) {
            ptr = krn_c2v(req->CVTaskInfoTable_daddr);
            krn_typecast(&entry,&ptr);
            while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
                if(entry->component_name_daddr != 0U) {
                    retcode = krn_cache_invalidate(krn_c2v(entry->component_name_daddr), nsize);
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
                entry++;
            }
        }

        if((retcode == ERRCODE_NONE) && (req->SchedInfoTable_daddr != 0U)) {
            ptr = krn_c2v(req->SchedInfoTable_daddr);
            krn_typecast(&entry,&ptr);
            while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
                if(entry->component_name_daddr != 0U) {
                    retcode = krn_cache_invalidate(krn_c2v(entry->component_name_daddr), nsize);
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
                entry++;
            }
        }

        if(retcode == ERRCODE_NONE) {
            mem_all = krn_ambacv_get_mem();
            mem_all->arm.base  = req->armblock_baseaddr;
            mem_all->arm.size  = mem_all->all.size - SCHDRMSG_MEM_REGION_SIZE;
            mem_all->arm.size -= (req->armblock_baseaddr - mem_all->all.base);

            sysconfig = krn_ambacv_get_sysinit();
            if (sysconfig->cavalry_num_slots != 0U) {
                sysconfig->cvtask_region_base_daddr = req->orc_uncached_baseaddr + req->orc_uncached_size;
            } /* if (sysconfig->cavalry_num_slots != 0) */
            else { /* if (sysconfig->cavalry_num_slots == 0) */
                sysconfig->cvtask_region_base_daddr = req->armblock_baseaddr;
            } /* if (sysconfig->cavalry_num_slots == 0) */
            retcode = krn_cache_clean(&sysconfig->cvtask_region_base_daddr, sizeof(sysconfig->cvtask_region_base_daddr));
        }
    }
    return retcode;
}

static uint32_t krn_invalidate_shutdown(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_scheduler_shutdown_t *req;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_shutdown() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.shutdown;
        retcode = krn_invalidate_logger_state(req->CVStaticLogInfo_daddr, 0U);
    }
    return retcode;
}

static uint32_t krn_invalidate_startup_log_update(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvlog_update_t *req;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_invalidate_startup_log_update() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.cvlog_update;
        retcode = krn_invalidate_logger_state(req->CVStaticLogInfo_daddr, 1U);
    }
    return retcode;
}

static uint32_t krn_cache_invld_schdrboot_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *pskip, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_BOOT_SETUP2:
        ret = krn_invalidate_boot_setup2(msg);
        break;

    case SCHEDMSG_BOOT_SETUP3:
        ret = krn_invalidate_boot_setup3(msg);
        break;

    case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
        ret = krn_invalidate_memory_report(msg);
        *pskip = 1U;
        break;

    case SCHEDMSG_VISORC_PRINTF_UPDATE:
        ret = krn_clean_scheduler_log();
        break;

    case SCHEDMSG_VISORC_STARTUP_UPDATE:
        ret = krn_invalidate_startup_log_update(msg);
        break;
    case SCHEDMSG_INITIAL_SETUP:
    case SCHEDMSG_BOOT_SETUP1:
    case SCHEDMSG_SET_BOOTUP_PHASE:
    case SCHEDMSG_HWUNIT_RESET:
        // these messages have nothing to invalidate:
        break;
    case SCHEDMSG_CVSCHEDULER_ACTIVE:
        ambacv_state = 1U;
        break;
    default:
        *hit = 0U;
        break;
    }
    return ret;
}

static uint32_t krn_cache_invld_schdr_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_SCHEDULER_SHUTDOWN:
        ret = krn_invalidate_shutdown(msg);
        break;

    case SCHEDMSG_CONFIGURE_IDSP:
    case SCHEDMSG_CONFIGURE_VIN:
    case SCHEDMSG_WAKE_FEEDERS:
    case SCHEDMSG_SUPERDAG_ERROR_MESSAGE:
    case SCHEDMSG_CONSOLE_ECHO_MESSAGE:
    case SCHEDMSG_SET_REWAKE_TIME:
    case SCHEDMSG_FLUSH_VISPRINTF_REPLY:
    case SCHEDMSG_AUTORUN_STATUS_UPDATE:
        // these messages have nothing to invalidate:
        break;
    default:
        *hit = 0U;
        break;
    }
    return ret;
}

static uint32_t krn_cache_invld_cvtask_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CVTASK_RUN_REQUEST:
    case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
    case SCHEDMSG_CVTASK_FINISH_REQUEST:
        ret = krn_invalidate_run(msg);
        break;
    case SCHEDMSG_CVTASK_REGISTER:
    case SCHEDMSG_CVTASK_QUERY_REQUEST:
    case SCHEDMSG_CVTASK_INIT_REQUEST:
    case SCHEDMSG_CVTASK_GET_BUILDINFO:
        // these messages have nothing to invalidate:
        break;
    default:
        *hit = 0U;
        break;
    }
    return ret;
}

static uint32_t krn_cache_invld_cavalry_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    // Cavalry related =--------------------------------------------------------
    case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
    case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
        // these messages have nothing to invalidate:
        break;
    default:
        *hit = 0U;
        break;
    }
    return ret;
}

static uint32_t krn_cache_invld_flexidag_cvtask_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *pflexidag_msg, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_FLEXIDAG_CVREG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVREG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
    case SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
    case SCHEDMSG_FLEXIDAG_CVINIT_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVINIT_REPLY:
        // these messages have nothing to invalidate:
        *pflexidag_msg = 1U;
        break;

    case SCHEDMSG_FLEXIDAG_CVRUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST:
        *pflexidag_msg = 1U;
        ret = krn_fd_invalidate_run(msg);
        break;

    default:
        *hit = 0U;
        break;
    }
    return ret;
}

static uint32_t krn_cache_invld_flexidag_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *pflexidag_msg, uint32_t *hit)
{
    uint32_t ret = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
    case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
    case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
    case SCHEDMSG_FLEXIDAG_SETUP_REQUEST:
    case SCHEDMSG_FLEXIDAG_INIT_REPLY:
    case SCHEDMSG_FLEXIDAG_INIT_PARTITIONED:
    case SCHEDMSG_FLEXIDAG_RUN_REPLY:
    case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
    case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
    case SCHEDMSG_FLEXIDAG_ERROR_MESSAGE:
        // these messages have nothing to invalidate:
        *pflexidag_msg = 1U;
        break;
    default:
        *hit = 0U;
        break;
    }
    return ret;
}

uint32_t krn_cache_invld_schdrmsg(armvis_msg_t *msg,uint32_t *changed)
{
    uint32_t retcode = ERRCODE_NONE, ret = ERRCODE_NONE;
    uint32_t type, skip = 0U, hit = 0U;
    uint32_t flexidag_msg = 0U;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_cache_invld_schdrmsg() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        type = (uint32_t) msg->hdr.message_type;
        ret = krn_cache_invld_schdrboot_schdrmsg(msg, type, &skip,&hit);
        if(hit == 0U) {
            ret = krn_cache_invld_schdr_schdrmsg(msg, type, &hit);
            if(hit == 0U) {
                ret = krn_cache_invld_cvtask_schdrmsg(msg, type, &hit);
                if(hit == 0U) {
                    ret = krn_cache_invld_cavalry_schdrmsg(msg, type, &hit);
                    if(hit == 0U) {
                        ret = krn_cache_invld_flexidag_cvtask_schdrmsg(msg, type, &flexidag_msg, &hit);
                        if(hit == 0U) {
                            ret = krn_cache_invld_flexidag_schdrmsg(msg, type, &flexidag_msg, &hit);
                            if(hit == 0U) {
                                krn_printU5("[ERROR] krn_cache_invld_schdrmsg() : unknown armvis_msg_t type %x\n",
                                            msg->hdr.message_type, 0U, 0U, 0U, 0U);
                                ret = ERR_DRV_SCHDR_VISMSG_UNKNOW;
                            }
                        }
                    }
                }
            }
        }

        if ( (skip == 0U) &&(enable_log_msg == 1U)) {
            krn_schdrmsg_log_msg(msg);
        }

        if(ret != ERRCODE_NONE) {
            msg->hdr.message_retcode = ret;
            retcode = krn_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
            }
            *changed = 1U;
        } else {
            if(msg->hdr.message_retcode != ERRCODE_NONE) {
                krn_printU5("[ERROR] krn_cache_invld_schdrmsg() : type 0x%x, visorc message_retcode 0x%x \n", type, msg->hdr.message_retcode, 0U, 0U, 0U);
                if(flexidag_msg == 1U) {
                    msg->hdr.message_retcode = ERR_DRV_FLEXIDAG_VISMSG_RET_FAIL;
                } else {
                    msg->hdr.message_retcode = ERR_DRV_SCHDR_VISMSG_RET_FAIL;
                }
                retcode = krn_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
                if(retcode == ERRCODE_NONE) {
                    retcode = krn_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
                }
                *changed = 1U;
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtaskmsg_send(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    cvmem_messagebuf_t *entry;
    const void *ptr;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtaskmsg_send() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        ptr = krn_c2v(msg->msg.reply_cvtask_msg.CVTaskMessage_daddr);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_clean_cvtaskmsg_send() : ptr = krn_c2v(msg->rpl_cvtaskmsg_send.msg); fail \n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_MSG_BUF_INVALID;
        } else {
            krn_typecast(&entry,&ptr);
            retcode = krn_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_clean(krn_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtaskmsg_send_at_time(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    cvmem_messagebuf_t *entry;
    const void *ptr;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtaskmsg_send_at_time() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        ptr = krn_c2v(msg->msg.reply_cvtask_msg_at_time.CVTaskMessage_daddr);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_clean_cvtaskmsg_send_at_time() : ptr = krn_c2v(msg->reply_cvtask_msg_at_time.CVTaskMessage_daddr); fail \n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_MSG_BUF_INVALID;
        } else {
            krn_typecast(&entry,&ptr);
            retcode = krn_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
            if(retcode == ERRCODE_NONE) {
                retcode = krn_cache_clean(krn_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtaskmsg_bulk_send(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_attach_bulk_cvtask_message_t *rpl;
    cvmem_messagebuf_t *entry;
    const void *ptr;
    uint32_t i, total;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtaskmsg_bulk_send() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &(msg->msg.reply_cvtask_bulk_msg);
        total = rpl->num_cvtask_messages;

        for (i = 0U; i < total; i++) {
            uint64_t addr = ((uint64_t)rpl->CVTaskMessage_daddr[i] - (uint64_t)sizeof(*entry));
            ptr = krn_c2v(addr);
            if(ptr == NULL) {
                krn_printU5("[ERROR] krn_clean_cvtaskmsg_bulk_send() : ptr = krn_c2v(addr); fail \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_MSG_BUF_INVALID;
            } else {
                krn_typecast(&entry,&ptr);
                retcode = krn_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
                if(retcode == ERRCODE_NONE) {
                    retcode = krn_cache_clean(krn_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
                }
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtask_query(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_query_reply_t *rpl;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtask_query() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &msg->msg.reply_cvtask_query;
        if(rpl->cvtask_memory_interface_daddr != 0U) {
            retcode = krn_cache_clean(krn_c2v(rpl->cvtask_memory_interface_daddr),
                                      sizeof(cvtask_memory_interface_t));
        }
    }

    return retcode;
}

static uint32_t krn_clean_get_buildinfo(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_getbuildinfo_reply_t *rpl;
    uint64_t maxsize = 0UL;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_get_buildinfo() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &msg->msg.reply_cvtask_getbuildinfo;

        maxsize = (uint64_t)rpl->versionstring_maxsize;
        if(rpl->daddr_versionstring_location != 0U) {
            retcode = krn_cache_clean(krn_c2v(rpl->daddr_versionstring_location), maxsize);
        }
        if(rpl->daddr_toolstring_location != 0U) {
            retcode = krn_cache_clean(krn_c2v(rpl->daddr_toolstring_location), maxsize);
        }

        if ((retcode == ERRCODE_NONE) && ( rpl->arm_cvtask_index == ARM_CVTASK_INDEX_SCHEDULER)) {
            maxsize = (uint64_t)NAME_MAX_LENGTH;
            if(rpl->daddr_component_location != 0U) {
                retcode = krn_cache_clean(krn_c2v(rpl->daddr_component_location), maxsize);
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtask_run(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_run_t *req;
    const cvmem_membuf_t *membuf;
    cvmem_messagebuf_t *cvmsg;
    const void *ptr;
    uint8_t i;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtask_run() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.cvtask_run;

        for (i = 0U; i < req->cvtask_num_outputs; i++) {
            ptr = krn_c2v(req->OutputMemBuf_daddr[i]);
            if(ptr == NULL) {
                krn_printU5("[ERROR] krn_clean_cvtask_run() : ptr = krn_c2v(req->OutputMemBuf_daddr[i]); fail \n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_OUTPUT_BUF_INVALID;
            } else {
                krn_typecast(&membuf,&ptr);
                if (membuf->hdr_fixed.needs_cache_coherency == 0U) {
                    //printk("skip output clean %d\n",
                    //       membuf->hdr_fixed.sysflow_index);
                    continue;
                }
                if(membuf->hdr_variable.payload_daddr != 0U) {
                    retcode = krn_cache_clean(krn_c2v(membuf->hdr_variable.payload_daddr),
                                              membuf->hdr_variable.payload_size);
                }
            }

            if(retcode != ERRCODE_NONE) {
                break;
            }
        }

        if(retcode == ERRCODE_NONE) {
            for (i = 0U; i < req->cvtask_num_messages; i++) {
                ptr = krn_c2v(req->CVTaskMessage_daddr[i]);
                if(ptr == NULL) {
                    krn_printU5("[ERROR] krn_clean_cvtask_run() : ptr = krn_c2v(req->CVTaskMessage_daddr[i]); fail \n", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_MSG_BUF_INVALID;
                } else {
                    krn_typecast(&cvmsg,&ptr);
                    cvmsg->hdr_variable.message_processed     = 1U;
                    cvmsg->hdr_variable.message_finished_time = krn_visorc_get_curtime();
                    retcode = krn_cache_clean(&cvmsg->hdr_variable, sizeof(cvmsg->hdr_variable));
                }

                if(retcode != ERRCODE_NONE) {
                    break;
                }
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_cvtask_partial_run(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvmem_membuf_t *membuf;
    const void *ptr;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_cvtask_partial_run() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        ptr = krn_c2v(msg->msg.reply_cvtask_run_partial.output_membuf_daddr);
        if(ptr == NULL) {
            krn_printU5("[ERROR] krn_clean_cvtask_partial_run() : ptr = krn_c2v(msg->reply_cvtask_run_partial.output_membuf_daddr); fail \n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_OUTPUT_BUF_INVALID;
        } else {
            krn_typecast(&membuf,&ptr);
            if (membuf->hdr_fixed.needs_cache_coherency == 1U) {
                if(membuf->hdr_variable.payload_daddr != 0U) {
                    retcode = krn_cache_clean(krn_c2v(membuf->hdr_variable.payload_daddr),
                                              membuf->hdr_variable.payload_size);
                }
            }
        }
    }

    return retcode;
}

static uint32_t krn_clean_boot_setup1(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup1_reply_t *rpl;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_boot_setup1() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &msg->msg.reply_boot_setup1;

        krn_logformat_size = rpl->memsize_printf_string_block;
    }

    return retcode;
}

static uint32_t krn_clean_boot_setup2(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup2_reply_t *rpl;
    uint64_t len = 0UL;
    uint32_t index;
    const uint32_t *ptr;
    void *src;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_clean_boot_setup2() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &msg->msg.reply_boot_setup2;

        for (index = 0U; index < rpl->sysflow_table_num_sets; index++) {
            len = (uint64_t)rpl->sysflow_table_set_numlines[index] * sizeof(sysflow_entry_t);
            if(rpl->sysflow_table_set_daddr[index] != 0U) {
                src = krn_c2v(rpl->sysflow_table_set_daddr[index]);
                retcode = krn_cache_clean(src, len);
                if(retcode != ERRCODE_NONE) {
                    break;
                }
            }
        }

        if(retcode == ERRCODE_NONE) {
            for (index = 0U; index < rpl->cvtable_num_sets; index++) {
                if(rpl->cvtable_daddr[index] != 0U) {
                    src = krn_c2v(rpl->cvtable_daddr[index]);
                    krn_typecast(&ptr,&src);
                    // ptr[4] is the total length of the cvtable
                    retcode = krn_cache_clean(src, ptr[4]);
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }

        if((retcode == ERRCODE_NONE) && (krn_logformat_addr != 0U)) {
            retcode = krn_cache_clean(krn_c2v(krn_logformat_addr), krn_logformat_size);
        }
    }

    return retcode;
}

static uint32_t krn_cache_clean_schdrboot_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_BOOT_SETUP1_REPLY:
        retcode = krn_clean_boot_setup1(msg);
        break;

    case SCHEDMSG_BOOT_SETUP2_REPLY:
        retcode = krn_clean_boot_setup2(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP_REPLY:
    case SCHEDMSG_BOOT_SETUP3_REPLY:
    case SCHEDMSG_HWUNIT_RESET_REPLY:
    case SCHEDMSG_LICENSE_UPDATE:
    case SCHEDMSG_TIME_REPORT:
        // these messages have nothing to clean
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_cache_clean_schdr_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_SET_DEBUG_LEVEL:
    case SCHEDMSG_CVSCHEDULER_RUN:
    case SCHEDMSG_SCHEDULER_SHUTDOWN:
    case SCHEDMSG_SET_FREQUENCY:
    case SCHEDMSG_SET_FREQUENCY_AT_FRAME:
    case SCHEDMSG_SET_FREQUENCY_AT_TIME:
    case SCHEDMSG_ARM_REQUESTS_SHUTDOWN:
    case SCHEDMSG_WAKE_FEEDERS:
    case SCHEDMSG_INVALIDATE_CACHE:
    case SCHEDMSG_FLUSH_VISPRINTF_REQ:
    case SCHEDMSG_SEND_PRIVATE_MSG:
        // these messages have nothing to clean
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_cache_clean_cvtask_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CVTASK_MSG_REPLY:
        retcode = krn_clean_cvtaskmsg_send(msg);
        break;

    case SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY:
        retcode = krn_clean_cvtaskmsg_send_at_time(msg);
        break;

    case SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME:
    case SCHEDMSG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME:
        retcode = krn_clean_cvtaskmsg_bulk_send(msg);
        break;

    case SCHEDMSG_CVTASK_QUERY_REPLY:
        retcode = krn_clean_cvtask_query(msg);
        break;

    case SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY:
        retcode = krn_clean_get_buildinfo(msg);
        break;

    case SCHEDMSG_CVTASK_RUN_REPLY:
    case SCHEDMSG_CVTASK_MSGONLY_REPLY:
    case SCHEDMSG_CVTASK_FINISH_REPLY:
        retcode = krn_clean_cvtask_run(msg);
        /* flush log after each run */
        retcode = krn_clean_scheduler_log();
        break;

    case SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY:
        retcode = krn_clean_cvtask_partial_run(msg);
        break;

    case SCHEDMSG_CVTASK_REGISTER_REPLY:
    case SCHEDMSG_CVTASK_INIT_REPLY:
        // these messages have nothing to clean
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_cache_clean_cavalry_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    // Cavalry related =--------------------------------------------------------
    case SCHEDMSG_CAVALRY_SETUP:
    case SCHEDMSG_CAVALRY_VP_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_HL_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST:
    case SCHEDMSG_CAVALRY_FEX_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST:
    case SCHEDMSG_CAVALRY_FMA_RUN_REQUEST:
        // these messages have nothing to clean
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_cache_clean_flexidag_cvtask_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    // Flexidag related =-------------------------------------------------------
    case SCHEDMSG_FLEXIDAG_CVTASK_MSG:
        retcode = krn_clean_cvtaskmsg_send(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME:
        retcode = krn_clean_cvtaskmsg_send_at_time(msg);
        break;
    case SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME:
        retcode = krn_clean_cvtaskmsg_bulk_send(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
        retcode = krn_clean_get_buildinfo(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
        retcode = krn_clean_cvtask_query(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVRUN_REPLY:
    case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REPLY:
        retcode = krn_clean_cvtask_run(msg);
        retcode = krn_clean_scheduler_log();
        break;
    case SCHEDMSG_FLEXIDAG_CVRUN_PARTIAL_REPLY:
        retcode = krn_clean_cvtask_partial_run(msg);
        retcode = krn_clean_scheduler_log();
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t krn_cache_clean_flexidag_schdrmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    *hit = 1U;
    switch (type) {
    // Flexidag related =-------------------------------------------------------
    case SCHEDMSG_FLEXIDAG_CREATE_REQUEST:
    case SCHEDMSG_FLEXIDAG_OPEN_REQUEST:
    case SCHEDMSG_FLEXIDAG_SETUP_REPLY:
    case SCHEDMSG_FLEXIDAG_INIT_REQUEST:
    case SCHEDMSG_FLEXIDAG_RUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CLOSE_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVREG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVINIT_REPLY:
    case SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL:
    case SCHEDMSG_FLEXIDAG_SET_FREQ:
    case SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME:
    case SCHEDMSG_FLEXIDAG_SET_FREQ_AT_TIME:
    case SCHEDMSG_FLEXIDAG_PRERUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG:
        // these messages have nothing to clean
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

uint32_t krn_cache_clean_schdrmsg(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t type, hit = 0U;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_cache_clean_schdrmsg() : msg == NULL \n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        type = (uint32_t)(msg->hdr.message_type);
        retcode = krn_cache_clean_schdrboot_schdrmsg(msg, type, &hit);
        if(hit == 0U) {
            retcode = krn_cache_clean_schdr_schdrmsg(msg, type, &hit);
            if(hit == 0U) {
                retcode = krn_cache_clean_cvtask_schdrmsg(msg, type, &hit);
                if(hit == 0U) {
                    retcode = krn_cache_clean_cavalry_schdrmsg(msg, type, &hit);
                    if(hit == 0U) {
                        retcode = krn_cache_clean_flexidag_cvtask_schdrmsg(msg, type, &hit);
                        if(hit == 0U) {
                            retcode = krn_cache_clean_flexidag_schdrmsg(msg, type, &hit);
                            if(hit == 0U) {
                                krn_printU5("[ERROR] krn_cache_clean_schdrmsg() : unknown armvis_msg_t type 0x%x\n",
                                            type, 0U, 0U, 0U, 0U);
                                retcode = ERR_DRV_SCHDR_VISMSG_UNKNOW;
                            }
                        }
                    }
                }
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = krn_cache_clean((char*)msg, sizeof(*msg));
        }

        if((retcode == ERRCODE_NONE) && (enable_log_msg != 0U)) {
            krn_schdrmsg_log_msg(msg);
        }
    }

    return retcode;
}

uint32_t krn_cache_ioctl_clean(const ambacv_mem_region_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
        krn_printU5("[ERROR] krn_cache_ioctl_clean() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(arg->base != 0UL) {
            retcode = krn_cache_clean(krn_p2v(arg->base), arg->size);
        } else {
            retcode = ERR_DRV_SCHDR_CACHE_CLEAN_FAIL;
            krn_printU5("[ERROR] krn_cache_ioctl_clean() : krn_cache_clean fail (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

uint32_t krn_cache_ioctl_invld(const ambacv_mem_region_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    if(arg == NULL) {
        retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
        krn_printU5("[ERROR] krn_cache_ioctl_invld() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(arg->base != 0UL) {
            retcode = krn_cache_invalidate(krn_p2v(arg->base), arg->size);
        } else {
            retcode = ERR_DRV_SCHDR_CACHE_INVALID_FAIL;
            krn_printU5("[ERROR] krn_cache_ioctl_invld() : krn_cache_invalidate fail (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

