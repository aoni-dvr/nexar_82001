/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "ambacv_kal.h"
#include "cvapi_logger_interface.h"
#include "cvapi_flexidag.h"
#include <schdr_api.h>
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

static schdr_log_info_t   *linfo;
static uint64_t host_logformat_addr = 0UL;
static uint64_t host_logformat_size = 0UL;
extern uint32_t debug_level;
cvprintf_buffer_info_t *print_info = NULL;

static kernel_memblk_t  kernel_master;
static kernel_memblk_t  rtos_master;
static kernel_memblk_t  dsp_data_master;

extern struct ambarella_cavalry *pCavalrySupport;

#if !defined(ENABLE_AMBA_MAL)
extern int cavalry_cma_v2p(struct ambarella_cavalry *cavalry, uint64_t *pPhyAddr, void *vpBuffer, uint32_t *pCacheFlag);
extern int cavalry_cma_p2v(struct ambarella_cavalry *cavalry, void **vppBuffer, uint64_t phyaddr, uint32_t *pCacheFlag);
#endif

extern uint64_t cv_pa_start,cv_pa_end;
extern uint64_t cv_ca_start,cv_ca_end;

#if defined(ENABLE_AMBA_MAL)
void dump_memory_range(void)
{
	pr_info("[kernel range] va 0x%px pa 0x%llx - 0x%llx\n", kernel_master.pBuffer,kernel_master.buffer_daddr,kernel_master.buffer_daddr_end);
	pr_info("[rtos   range] va 0x%px pa 0x%llx - 0x%llx\n", rtos_master.pBuffer,rtos_master.buffer_daddr,rtos_master.buffer_daddr_end);
}
#else
extern void cavalry_cma_dump_range(struct ambarella_cavalry *cavalry);
void dump_memory_range(void)
{
	pr_info("[kernel range] va 0x%px pa 0x%llx - 0x%llx\n", kernel_master.pBuffer,kernel_master.buffer_daddr,kernel_master.buffer_daddr_end);
	pr_info("[rtos   range] va 0x%px pa 0x%llx - 0x%llx\n", rtos_master.pBuffer,rtos_master.buffer_daddr,rtos_master.buffer_daddr_end);
	cavalry_cma_dump_range(pCavalrySupport);
}
#endif

void * ambacv_p2v(uint64_t phy)
{
  uint64_t offset = 0UL;
  void *retval;

  retval = NULL;
  if (phy == 0U) {
	  pr_err("[ERROR] ambacv_p2v() : phy == 0U \n");
  } else {
	  if ((phy >= kernel_master.buffer_daddr) && (phy < kernel_master.buffer_daddr_end))
	  {
	    offset = phy - kernel_master.buffer_daddr;
	    retval = kernel_master.pBuffer + offset;
	  } /* if ((phy >= kernel_master.buffer_daddr) && (phy < kernel_master.buffer_daddr_end)) */
	  else if ((phy >= rtos_master.buffer_daddr) && (phy < rtos_master.buffer_daddr_end))
	  {
	    offset = phy - rtos_master.buffer_daddr;
	    retval = rtos_master.pBuffer + offset;
	  } /* if ((phy >= rtos_master.buffer_daddr) && (phy < rtos_master.buffer_daddr_end)) */
	  else if ((phy >= dsp_data_master.buffer_daddr) && (phy < dsp_data_master.buffer_daddr_end))
	  {
	    offset = phy - dsp_data_master.buffer_daddr;
	    retval = &dsp_data_master.pBuffer[offset];
	  } /* if ((phy >= dsp_data_master.buffer_daddr) && (phy < dsp_data_master.buffer_daddr_end)) */
	  else /* if (phy is not in [kernel_master, rtos_master]) */
	  {
	    void *vpBuffer;
	    uint32_t  cache_flag;
	    int ret;

	    vpBuffer    = NULL;
	    cache_flag  = 0;
#if defined(ENABLE_AMBA_MAL)
	    ret         = AmbaMAL_Phys2Virt(AMBA_MAL_ID_CV_SYS, phy, AMBA_MAL_ATTRI_CACHE, &vpBuffer);
#else
	    ret         = cavalry_cma_p2v(pCavalrySupport, &vpBuffer, phy, &cache_flag);
#endif
	    if (ret == 0)
	    {
	      retval = vpBuffer;
	    }
	    else
	    {
	      pr_warn("ambacv_p2v() : Unable to find mapping for phys (0x%llx)\n", phy);
	    }
	  } /* if (phy is not in [kernel_master, rtos_master]) */
  }
  return retval;

} /* ambacv_p2v() */

uint64_t ambacv_v2p(void *virt)
{
  uint64_t  offset = 0UL;
  uint64_t  retval = 0UL;
  char     *pBuf;

  retval  = 0;
  offset  = 0;
  pBuf    = (char *)virt; /* Recast to suppress warnings */

  if ((pBuf >= kernel_master.pBuffer) && (pBuf < kernel_master.pBuffer_end))
  {
    offset = pBuf - kernel_master.pBuffer;
    retval = kernel_master.buffer_daddr + offset;
  } /* if ((pBuf >= kernel_master.pBuffer) && (pBuf < kernel_master.pBuffer_end)) */
  else if ((pBuf >= rtos_master.pBuffer) && (pBuf < rtos_master.pBuffer_end))
  {
    offset = pBuf - rtos_master.pBuffer;
    retval = rtos_master.buffer_daddr + offset;
  } /* if ((pBuf >= rtos_master.pBuffer) && (pBuf < rtos_master.pBuffer_end)) */
  else if ((pBuf >= dsp_data_master.pBuffer) && (pBuf < dsp_data_master.pBuffer_end))
  {
    offset = pBuf - dsp_data_master.pBuffer;
    retval = dsp_data_master.buffer_daddr + offset;
  } /* if ((pBuf >= dsp_data_master.pBuffer) && (pBuf < dsp_data_master.pBuffer_end)) */
  else /* if (phy is not in [kernel_master, rtos_master]) */
  {
    uint32_t  cache_flag;
    uint64_t  physaddr = 0UL;
    int ret;

    physaddr    = 0;
    cache_flag  = 0;
#if defined(ENABLE_AMBA_MAL)
    ret         = AmbaMAL_Virt2Phys(AMBA_MAL_ID_CV_SYS, virt, (UINT64 *)&physaddr);
#else
    ret         = cavalry_cma_v2p(pCavalrySupport, &physaddr, virt, &cache_flag);
#endif
    if (ret == 0)
    {
      retval = physaddr;
    }
    else
    {
      pr_warn("ambacv_v2p(): Unable to find mapping for virt (%p)\n", virt);
    }

  } /* if (phy is not in [kernel_master, rtos_master]) */

  return retval;

} /* ambacv_v2p() */

#if defined(ENABLE_AMBA_MAL)
uint64_t ambacv_p2c(uint64_t pa)
{
    uint64_t ca = 0UL;

    if(pa != 0U) {
        if( AmbaMAL_Phys2Global(pa,(UINT64 *)&ca) != 0U) {
            pr_err("ambacv_p2c() fail: pa (0x%llx)\n", pa);
            dump_stack();
        }
    }
    return ca;
}

uint64_t ambacv_c2p(uint64_t ca)
{
    uint64_t pa = 0UL;

    if( AmbaMAL_Global2Phys(ca,(UINT64 *)&pa) != 0U) {
        pr_err("ambacv_c2p() fail: ca (0x%llx)\n", ca);
        dump_stack();
    }
    return pa;
}
#else
uint64_t ambacv_p2c(uint64_t pa)
{
	uint64_t ca = 0UL;

	if( (pa >= cv_pa_start) && (pa <= cv_pa_end) ) {
		ca = cv_ca_start + (pa - cv_pa_start);
	} else {
		if(pa != 0) {
			pr_err("ambacv_p2c() fail: pa (0x%llx)\n", pa);
			dump_stack();
		}
	}
	
	return ca;
}

uint64_t ambacv_c2p(uint64_t ca)
{
	uint64_t pa = 0UL;

	if( (ca >= cv_ca_start) && (ca <= cv_ca_end) ) {
		pa = cv_pa_start + (ca - cv_ca_start);
	} else {
		pr_err("ambacv_c2p() fail: ca (0x%llx)\n", ca);
		dump_stack();
	}	
	return pa;
}
#endif

void * ambacv_c2v(uint64_t ca)
{
	uint64_t pa = 0UL;

	pa = ambacv_c2p(ca);
	return ambacv_p2v(pa);
}

uint64_t ambacv_v2c(void *virt)
{
	uint64_t ca = 0UL;
	uint64_t pa = 0UL;

	pa = ambacv_v2p(virt);
	ca = ambacv_p2c(pa);;
	return ca;
}

void  ambacv_mmap_add_master(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
  kernel_master.pBuffer           = (char *)virt;
  kernel_master.pBuffer_end       = kernel_master.pBuffer + size;
  kernel_master.buffer_cacheable  = is_cacheable;
  kernel_master.buffer_daddr      = phy;
  kernel_master.buffer_daddr_end  = phy + size;
  kernel_master.buffer_size       = size;
} /* ambacv_mmap_add_master() */

void  ambacv_mmap_add_rtos(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
  rtos_master.pBuffer             = (char *)virt;
  rtos_master.pBuffer_end         = rtos_master.pBuffer + size;
  rtos_master.buffer_cacheable    = is_cacheable;
  rtos_master.buffer_daddr        = phy;
  rtos_master.buffer_daddr_end    = phy + size;
  rtos_master.buffer_size         = size;
} /* ambacv_mmap_add_rtos() */

void  ambacv_mmap_add_dsp_data(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable)
{
  dsp_data_master.pBuffer             = (char *)virt;
  dsp_data_master.pBuffer_end         = dsp_data_master.pBuffer + size;
  dsp_data_master.buffer_cacheable    = is_cacheable;
  dsp_data_master.buffer_daddr        = phy;
  dsp_data_master.buffer_daddr_end    = phy + size;
  dsp_data_master.buffer_size         = size;
} /* krn_mmap_add_dsp_data() */

extern int enable_log_msg;

static void ambacv_cache_clean_unaligned(void *ptr, uint64_t size)
{
	uint64_t ptr_u = (uint64_t)ptr;
	uint64_t ptr_a = ptr_u & ~(ARM_CACHELINE_SIZE - 1LL);

	size += (int)(ptr_u - ptr_a);
	size = (size + ARM_CACHELINE_SIZE - 1) & ~(ARM_CACHELINE_SIZE - 1);
	ambacv_cache_clean((void*)ptr_a, size);
}

static void sync_logger_buffer(char *_base, int sync_format_string,
	cvlog_buffer_info_t *info)
{
	char *base;
	uint64_t wi_prev = 0UL, wi_curr= 0UL, e_size= 0UL;

	base = _base + info->buffer_addr_offset;
	e_size  = info->entry_size;
	if (sync_format_string == 0)
	{
		wi_prev = info->prev_write_index;
		wi_curr = info->write_index;
	}
	else
	{
		wi_prev = 0UL;
		wi_curr = info->buffer_size_in_entries;
	}

	/* sync the tail part if there is a wrap-around */
	if (wi_prev > wi_curr) {
		int pos = wi_prev * e_size;
		int len = (info->buffer_size_in_entries - wi_prev) * e_size;
		ambacv_cache_invalidate(base + pos, len);
		ambacv_cache_barrier();
		wi_prev = 0UL;
	}

	/* sync the head part if the buffer is not empty */
	if (wi_prev != wi_curr) {
		int pos = wi_prev * e_size;
		int len = (wi_curr - wi_prev) * e_size;
		ambacv_cache_invalidate(base + pos, len);
		ambacv_cache_barrier();
	}

	/* sync the format string */
	if (sync_format_string != 0) {
		base = _base + info->binary_offset;
		ambacv_cache_invalidate(base, info->binary_size);
		ambacv_cache_barrier();
	}
}

static int invalidate_logger_state(uint64_t addr, uint32_t sync)
{
	cvlogger_state_t *state = (cvlogger_state_t*)ambacv_c2v(addr);
	int i;

	ambacv_cache_invalidate(state, sizeof(*state));
	ambacv_cache_barrier();

	if (state->orcsod.perf_log.buffer_addr_offset != state->orcvp.perf_log.buffer_addr_offset)
	{
		sync_logger_buffer((char*)state, sync, &state->orcsod.perf_log);
	}
	if (state->orcsod.cvtask_log.buffer_addr_offset != state->orcvp.cvtask_log.buffer_addr_offset)
	{
		sync_logger_buffer((char*)state, sync, &state->orcsod.cvtask_log);
	}
	if (state->orcsod.sched_log.buffer_addr_offset != state->orcvp.sched_log.buffer_addr_offset)
	{
		sync_logger_buffer((char*)state, sync, &state->orcsod.sched_log);
	}
	sync_logger_buffer((char*)state, sync, &state->orcvp.perf_log);
	sync_logger_buffer((char*)state, sync, &state->orcvp.cvtask_log);
	sync_logger_buffer((char*)state, sync, &state->orcvp.sched_log);

	for (i = 0; i < state->num_arm; i++)
	{
		sync_logger_buffer((char*)state,sync, &state->arm[i].perf_log);
		sync_logger_buffer((char*)state,sync, &state->arm[i].cvtask_log);
		sync_logger_buffer((char*)state,sync, &state->arm[i].sched_log);
	}

	return 0;
}

static void clean_log_buffer(schdr_log_info_t *info)
{
	char     *base = NULL;
    uint32_t *wptr = NULL;
	uint64_t esize = 0UL, wi_curr = 0UL, wi_prev = 0UL;

	ambacv_cache_invalidate(info, sizeof (*info));
	ambacv_cache_barrier();
	wptr = ambacv_c2v(info->wptr_daddr);
	base = ambacv_c2v(info->base_daddr);

	wi_prev = *wptr;
	wi_curr = (info->wridx & info->mask);
	esize = info->esize;

	if (wi_prev > wi_curr) {
		char     *ptr = base + wi_prev * esize;
		uint64_t len = (info->mask + 1 - wi_prev) * esize;
		ambacv_cache_clean_unaligned(ptr, len);
		wi_prev = 0UL;
	}

	if (wi_prev != wi_curr) {
		char     *ptr = base + wi_prev * esize;
		uint64_t len = (wi_curr - wi_prev) * esize;
		ambacv_cache_clean_unaligned(ptr, len);
	}

	*wptr = wi_curr;
	ambacv_cache_clean(wptr, sizeof(*wptr));
}

static void clean_scheduler_log(void)
{
	clean_log_buffer(&linfo[0]);
	clean_log_buffer(&linfo[1]);
	clean_log_buffer(&linfo[2]);
}

static void invalidate_run(armvis_msg_t *msg)
{
  schedmsg_cvtask_run_t *req = &msg->msg.cvtask_run;
  cvmem_membuf_t *membuf;
  uint32_t i;

  for (i = 0; i < req->cvtask_num_inputs; i++) {
    if (req->InputMemBuf_daddr[i] == 0) {
      /* skip special case for dependency-only input */
      continue;
    }

    membuf = (cvmem_membuf_t *)ambacv_c2v(req->InputMemBuf_daddr[i]);
    ambacv_cache_invalidate(membuf, sizeof(*membuf));
    ambacv_cache_barrier();
    if ((req->ignore_invalidate_flags & (1 << (i + 0))) == 0)
    {
      if (membuf->hdr_fixed.needs_cache_coherency != 0)
      {
        ambacv_cache_invalidate(ambacv_c2v(membuf->hdr_variable.payload_daddr), membuf->hdr_variable.payload_size);
        ambacv_cache_barrier();
      }
    }

    ambacv_cache_invalidate(ambacv_c2v(membuf->hdr_variable.payload_daddr), membuf->hdr_variable.payload_size);
    ambacv_cache_barrier();

    if (membuf->hdr_variable.framework_reserved == 1)
    {
      invalidate_logger_state(membuf->hdr_variable.payload_daddr, 0);
    }
  }

  for (i = 0; i < req->cvtask_num_feedback; i++) {
    if (req->FeedbackMemBuf_daddr[i] == 0) {
      /* skip special case */
      continue;
    }
    membuf = (cvmem_membuf_t *)ambacv_c2v(req->FeedbackMemBuf_daddr[i]);
    ambacv_cache_invalidate(membuf, sizeof(*membuf));
    ambacv_cache_barrier();
    if ((req->ignore_invalidate_flags & (1 << (i + 28))) == 0)
    {
      if (membuf->hdr_fixed.needs_cache_coherency != 0)
      {
        ambacv_cache_invalidate(ambacv_c2v(membuf->hdr_variable.payload_daddr), membuf->hdr_variable.payload_size);
        ambacv_cache_barrier();
      }
    }
  }

  for (i = 0; i < req->cvtask_num_messages; i++) {
    cvmem_messagebuf_t *cvmsg = (cvmem_messagebuf_t *)ambacv_c2v(req->CVTaskMessage_daddr[i]);
    ambacv_cache_invalidate(cvmsg, sizeof(*cvmsg));
    ambacv_cache_barrier();
    ambacv_cache_invalidate(ambacv_c2v(cvmsg->hdr_variable.message_payload_daddr), cvmsg->hdr_variable.message_size);
  }

  for (i = 0; i < req->cvtask_num_outputs; i++) {
    membuf = (cvmem_membuf_t *)ambacv_c2v(req->OutputMemBuf_daddr[i]);
    ambacv_cache_invalidate(membuf, sizeof(*membuf));
    ambacv_cache_barrier();
  }
}

static void invalidate_boot_setup2(armvis_msg_t *msg)
{
	schedmsg_boot_setup2_t *req = &msg->msg.boot_setup2;
	cvprintf_buffer_info_t *info;
	debug_buffer_info_t    *buf;
	visorc_init_params_t *sysconfig;
	uint32_t  log_loop;

	/* Patch the "linfo" area into the same spot that the linux scheduler uses */
	sysconfig = ambacv_get_sysinit_addr();
	sysconfig->arm_linfo_base_daddr = req->sysflow_block_daddr;  /* TODO: Discover why it's used this way */
	(void) ambacv_cache_clean(&sysconfig->arm_linfo_base_daddr, sizeof(sysconfig->arm_linfo_base_daddr));

	linfo = ambacv_c2v(req->sysflow_block_daddr);

	info = (cvprintf_buffer_info_t *)ambacv_c2v(req->arm_printf_buffer_info_daddr);
	print_info = info;
	ambacv_cache_invalidate(info, sizeof(*info));
	ambacv_cache_barrier();

	host_logformat_addr = req->printf_string_block_daddr;

  if (debug_level > 0)
  {
    pr_info("=============== Log Buffer Info ===============\n");
  }

  for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++)
  {
    buf = &(info->visorc_cvtask_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0))
    {
      if (debug_level > 0)
      {
        pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
               buf->core_name, buf->log_type,
               (ambacv_c2p(buf->base_daddr)),
               (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
               (buf->entry_size * buf->size_in_entries));
      } /* if (debug_level > 0) */
      if (strcmp(buf->core_name, "SOD") == 0)
      {
        ambacv_global_log.schdr_log[SOD_CVTASK_LOG_INDEX].base = ambacv_c2p(buf->base_daddr);
        ambacv_global_log.schdr_log[SOD_CVTASK_LOG_INDEX].size = buf->entry_size * buf->size_in_entries;
      }
      else if (strcmp(buf->core_name, "VP") == 0)
      {
        ambacv_global_log.schdr_log[VP_CVTASK_LOG_INDEX].base = ambacv_c2p(buf->base_daddr);
        ambacv_global_log.schdr_log[VP_CVTASK_LOG_INDEX].size = buf->entry_size * buf->size_in_entries;
      }
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
    buf = &(info->visorc_sched_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0))
    {
      if (debug_level > 0)
      {
        pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
               buf->core_name, buf->log_type,
               (ambacv_c2p(buf->base_daddr)),
               (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
               (buf->entry_size * buf->size_in_entries));
      } /* if (debug_level > 0) */
      if (strcmp(buf->core_name, "SOD") == 0)
      {
        ambacv_global_log.schdr_log[SOD_SCHDR_LOG_INDEX].base = ambacv_c2p(buf->base_daddr);
        ambacv_global_log.schdr_log[SOD_SCHDR_LOG_INDEX].size = buf->entry_size * buf->size_in_entries;
      }
      else if (strcmp(buf->core_name, "VP") == 0)
      {
        ambacv_global_log.schdr_log[VP_SCHDR_LOG_INDEX].base = ambacv_c2p(buf->base_daddr);
        ambacv_global_log.schdr_log[VP_SCHDR_LOG_INDEX].size = buf->entry_size * buf->size_in_entries;
      }
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
    buf = &(info->visorc_perf_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0))
    {
      if (debug_level > 0)
      {
        pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
               buf->core_name, buf->log_type,
               (ambacv_c2p(buf->base_daddr)),
               (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
               (buf->entry_size * buf->size_in_entries));
      } /* if (debug_level > 0) */
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0)) */
  } /* for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++) */

  for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_CORTEX; log_loop++)
  {
    buf = &(info->arm_cvtask_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0))
    {
      pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
             buf->core_name, buf->log_type,
             (ambacv_c2p(buf->base_daddr)),
             (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
             (buf->entry_size * buf->size_in_entries));
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0)) */
    buf = &(info->arm_sched_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0))
    {
      pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
             buf->core_name, buf->log_type,
             (ambacv_c2p(buf->base_daddr)),
             (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
             (buf->entry_size * buf->size_in_entries));
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0)) */
    buf = &(info->arm_perf_debugbuf[log_loop]);
    if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0))
    {
      pr_info(" %4s:%6s: @ [0x%llx--0x%llx] (%d)\n",
             buf->core_name, buf->log_type,
             (ambacv_c2p(buf->base_daddr)),
             (ambacv_c2p(buf->base_daddr) + (buf->entry_size * buf->size_in_entries) - 1),
             (buf->entry_size * buf->size_in_entries));
    } /* if ((buf->base_daddr != 0) && (buf->entry_size != 0) && (debug_level > 0)) */
  } /* for (log_loop = 0; log_loop < SYSTEM_MAX_NUM_VISORC; log_loop++) */
}

static void invalidate_boot_setup3(armvis_msg_t *msg)
{
	schedmsg_boot_setup3_t *req = &msg->msg.boot_setup3;
	cvtable_entry_t  *info;
	sysflow_name_entry_t *name;
	sysflow_entry_t *entry;
	int i;

	name = (sysflow_name_entry_t *)ambacv_c2v(req->sysflow_name_daddr);
	ambacv_cache_invalidate(name, req->sysflow_numlines * sizeof(*name));

	entry = (sysflow_entry_t *)ambacv_c2v(req->sysflow_table_daddr);
	ambacv_cache_invalidate(entry, req->sysflow_numlines * sizeof(*entry));

	info = (cvtable_entry_t *)ambacv_c2v(req->cvtable_base_daddr);
	ambacv_cache_invalidate(info, req->cvtable_numlines * sizeof(*info));

	ambacv_cache_barrier();

	for (i = req->cvtable_numlines; i > 0; i--, info++) {
		ambacv_cache_invalidate(ambacv_c2v(info->CVTable_name_daddr), CVTABLE_NAME_MAX_LENGTH);
		ambacv_cache_invalidate(ambacv_c2v(info->CVTable_daddr), info->cvtable_size);
	}
	ambacv_cache_barrier();
}

static void invalidate_memory_report(armvis_msg_t *msg)
{
  schedmsg_cvscheduler_memory_report_t *req = &msg->msg.memory_report;
  visorc_init_params_t *sysconfig;
  component_build_info_t *entry;
  int nsize = COMPONENT_NAME_STRING_ALLOC_SIZE;

  // invalidate default message pool
  ambacv_cache_invalidate(ambacv_c2v(req->CVTaskMsgPool_daddr), sizeof(cvmem_messagepool_t));

  // invalidate cvtask build info
  ambacv_cache_invalidate(ambacv_c2v(req->CVTaskInfoTable_daddr), sizeof(component_build_info_t) * req->sysflow_numlines);

  // invalidate scheduler build info
  ambacv_cache_invalidate(ambacv_c2v(req->SchedInfoTable_daddr), sizeof(component_build_info_t) * 16);

  ambacv_cache_barrier();

  // invalidate the component name string
  entry = (component_build_info_t *) ambacv_c2v(req->CVTaskInfoTable_daddr);
  while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
    ambacv_cache_invalidate(ambacv_c2v(entry->component_name_daddr), nsize);
    entry++;
  }

  entry = (component_build_info_t *) ambacv_c2v(req->SchedInfoTable_daddr);
  while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
    ambacv_cache_invalidate(ambacv_c2v(entry->component_name_daddr), nsize);
    entry++;
  }

  ambacv_cache_barrier();

  ambacv_global_mem.arm.base  = req->armblock_baseaddr;
  ambacv_global_mem.arm.size  = ambacv_global_mem.all.size - SCHDRMSG_MEM_REGION_SIZE;
  ambacv_global_mem.arm.size -= (req->armblock_baseaddr - ambacv_global_mem.all.base);

  sysconfig = ambacv_get_sysinit_addr();
  if (sysconfig->cavalry_num_slots != 0)
  {
    sysconfig->cvtask_region_base_daddr = req->orc_uncached_baseaddr + req->orc_uncached_size;
  } /* if (sysconfig->cavalry_num_slots != 0) */
  else /* if (sysconfig->cavalry_num_slots == 0) */
  {
    sysconfig->cvtask_region_base_daddr = req->armblock_baseaddr;
  } /* if (sysconfig->cavalry_num_slots == 0) */
  (void) ambacv_cache_clean(&sysconfig->cvtask_region_base_daddr, sizeof(sysconfig->cvtask_region_base_daddr));
}

static void invalidate_shutdown(armvis_msg_t *msg)
{
  schedmsg_scheduler_shutdown_t *req = &msg->msg.shutdown;
  invalidate_logger_state(req->CVStaticLogInfo_daddr, 0);
}

static void invalidate_startup_log_update(armvis_msg_t *msg)
{
  schedmsg_cvlog_update_t *req = &msg->msg.cvlog_update;
  invalidate_logger_state(req->CVStaticLogInfo_daddr, 1);
}

void cache_invld_schdrmsg(armvis_msg_t *msg, int phase)
{
	if (phase == 0)
	{
		ambacv_cache_invalidate((char*)msg, sizeof(*msg));
		ambacv_cache_barrier();
	} /* if (phase == 0) */
	else /* if (phase != 0) */
	{
		switch (msg->hdr.message_type) {
		case SCHEDMSG_CVTASK_RUN_REQUEST:
		case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
		case SCHEDMSG_CVTASK_FINISH_REQUEST:
			invalidate_run(msg);
			break;

		case SCHEDMSG_BOOT_SETUP2:
			invalidate_boot_setup2(msg);
			break;

		case SCHEDMSG_BOOT_SETUP3:
			invalidate_boot_setup3(msg);
			break;

		case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
			invalidate_memory_report(msg);
			return;

		case SCHEDMSG_VISORC_PRINTF_UPDATE:
			clean_scheduler_log();
			break;

		case SCHEDMSG_VISORC_STARTUP_UPDATE:
			invalidate_startup_log_update(msg);
			break;

		case SCHEDMSG_SCHEDULER_SHUTDOWN:
			invalidate_shutdown(msg);
			break;

		case SCHEDMSG_INITIAL_SETUP:
		case SCHEDMSG_BOOT_SETUP1:
		case SCHEDMSG_SET_BOOTUP_PHASE:
		case SCHEDMSG_CVTASK_REGISTER:
		case SCHEDMSG_CVTASK_QUERY_REQUEST:
		case SCHEDMSG_CVTASK_INIT_REQUEST:
		case SCHEDMSG_CONFIGURE_IDSP:
		case SCHEDMSG_CVTASK_GET_BUILDINFO:
		case SCHEDMSG_HWUNIT_RESET:
		case SCHEDMSG_WAKE_FEEDERS:
		case SCHEDMSG_CONSOLE_ECHO_MESSAGE:
		case SCHEDMSG_SUPERDAG_ERROR_MESSAGE:
		case SCHEDMSG_FLUSH_VISPRINTF_REPLY:
		case SCHEDMSG_SET_REWAKE_TIME:
		case SCHEDMSG_AUTORUN_STATUS_UPDATE:
			// these messages have nothing to invalidate:
			break;
		case SCHEDMSG_CVSCHEDULER_ACTIVE:
			ambacv_state = 1;
			break;

		// Cavalry related =--------------------------------------------------------
		case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
		case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
		case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
		case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
		case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
		case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
			// these messages have nothing to invalidate:
			break;

		// Flexidag related =-------------------------------------------------------
		case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
		case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
		case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
		case SCHEDMSG_FLEXIDAG_SETUP_REQUEST:
		case SCHEDMSG_FLEXIDAG_INIT_REPLY:
		case SCHEDMSG_FLEXIDAG_INIT_PARTITIONED:
		case SCHEDMSG_FLEXIDAG_RUN_REPLY:
		case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
		case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:

		case SCHEDMSG_FLEXIDAG_CVREG_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVREG_REPLY:
		case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
		case SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
		case SCHEDMSG_FLEXIDAG_CVINIT_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVINIT_REPLY:
		case SCHEDMSG_FLEXIDAG_ERROR_MESSAGE:
			// these messages have nothing to invalidate:
			break;

		case SCHEDMSG_FLEXIDAG_CVRUN_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST:
		case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST:
			invalidate_run(msg);
			break;
		default:
			pr_err("Error!! invalidate_msg unknown armvis_msg_t type %X\n",
			       msg->hdr.message_type);
			break;
		} /* switch (msg->hdr.message_type) */

		if (enable_log_msg)
		{
			schdrmsg_log_msg(msg);
		}
	} /* if (phase != 0) */
}

static void clean_cvtaskmsg_send(armvis_msg_t *msg)
{
  cvmem_messagebuf_t *entry;

  entry = (cvmem_messagebuf_t *)ambacv_c2v(msg->msg.reply_cvtask_msg.CVTaskMessage_daddr);
  ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
  ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
}

static void clean_cvtaskmsg_send_at_time(armvis_msg_t *msg)
{
  cvmem_messagebuf_t *entry;

  entry = (cvmem_messagebuf_t *)ambacv_c2v(msg->msg.reply_cvtask_msg_at_time.CVTaskMessage_daddr);
  ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
  ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
}

static void clean_cvtaskmsg_bulk_send(armvis_msg_t *msg)
{
  schedmsg_attach_bulk_cvtask_message_t *rpl;
  cvmem_messagebuf_t *entry;
  int i, total;

  rpl = &(msg->msg.reply_cvtask_bulk_msg);
  total = rpl->num_cvtask_messages;

  for (i = 0; i < total; i++) {
    uint64_t addr = rpl->CVTaskMessage_daddr[i] - sizeof(*entry);
    entry = (cvmem_messagebuf_t *)ambacv_c2v(addr);
    ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
    ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
  }
}

static void clean_cvtask_query(armvis_msg_t *msg)
{
  schedmsg_cvtask_query_reply_t *rpl = &msg->msg.reply_cvtask_query;

  ambacv_cache_clean(ambacv_c2v(rpl->cvtask_memory_interface_daddr), sizeof(cvtask_memory_interface_t));
}

static void clean_get_buildinfo(armvis_msg_t *msg)
{
  schedmsg_cvtask_getbuildinfo_reply_t *rpl = &msg->msg.reply_cvtask_getbuildinfo;
  int maxsize;

  maxsize = rpl->versionstring_maxsize;
  ambacv_cache_clean(ambacv_c2v(rpl->daddr_versionstring_location), maxsize);
  ambacv_cache_clean(ambacv_c2v(rpl->daddr_toolstring_location), maxsize);

  if (rpl->arm_cvtask_index == ARM_CVTASK_INDEX_SCHEDULER) {
    maxsize = NAME_MAX_LENGTH;
    ambacv_cache_clean(ambacv_c2v(rpl->daddr_component_location), maxsize);
  }
}

static void clean_cvtask_run(armvis_msg_t *msg)
{
  schedmsg_cvtask_run_t *req = &msg->msg.cvtask_run;
  int i;

  for (i = 0; i < req->cvtask_num_outputs; i++) {
    cvmem_membuf_t *membuf = (cvmem_membuf_t *) ambacv_c2v(req->OutputMemBuf_daddr[i]);
    if (!membuf->hdr_fixed.needs_cache_coherency) {
      //pr_info("skip output clean %d\n", membuf->hdr_fixed.sysflow_index);
      continue;
    }
    ambacv_cache_clean(ambacv_c2v(membuf->hdr_variable.payload_daddr), membuf->hdr_variable.payload_size);
  }

  for (i = 0; i < req->cvtask_num_messages; i++) {
    cvmem_messagebuf_t *cvmsg = (cvmem_messagebuf_t *) ambacv_c2v(req->CVTaskMessage_daddr[i]);
    cvmsg->hdr_variable.message_processed = 1;
    ambacv_cache_clean(&cvmsg->hdr_variable, sizeof(cvmsg->hdr_variable));
  }
}

static void clean_cvtask_partial_run(armvis_msg_t *msg)
{
  cvmem_membuf_t *membuf;

  membuf = (cvmem_membuf_t *)ambacv_c2v(msg->msg.reply_cvtask_run_partial.output_membuf_daddr);

  if (membuf->hdr_fixed.needs_cache_coherency) {
    ambacv_cache_clean(ambacv_c2v(membuf->hdr_variable.payload_daddr), membuf->hdr_variable.payload_size);
  }
}

static void clean_boot_setup1(armvis_msg_t *msg)
{
  schedmsg_boot_setup1_reply_t *rpl = &msg->msg.reply_boot_setup1;
  host_logformat_size = rpl->memsize_printf_string_block;
}

static void clean_boot_setup2(armvis_msg_t *msg)
{
  schedmsg_boot_setup2_reply_t *rpl = &msg->msg.reply_boot_setup2;
  int index, len;
  uint32_t *ptr;

	for (index = 0; index < rpl->sysflow_table_num_sets; index++) {
		len = rpl->sysflow_table_set_numlines[index] * sizeof(sysflow_entry_t);
		ptr = ambacv_c2v(rpl->sysflow_table_set_daddr[index]);
		ambacv_cache_clean(ptr, len);
	}

	for (index = 0; index < rpl->cvtable_num_sets; index++) {
		if (rpl->cvtable_daddr[index] != 0U)
		{
			ptr = ambacv_c2v(rpl->cvtable_daddr[index]);
			// ptr[4] is the total length of the cvtable
			ambacv_cache_clean(ptr, ptr[4]);
		} /* if (rpl->cvtable_daddr[index] != 0U) */
	}

	if(host_logformat_addr != 0U) {
		ambacv_cache_clean(ambacv_c2v(host_logformat_addr), host_logformat_size);
	}
}

void cache_clean_schdrmsg(armvis_msg_t *msg)
{
	switch (msg->hdr.message_type) {
	case SCHEDMSG_CVTASK_MSG_REPLY:
		clean_cvtaskmsg_send(msg);
		break;

	case SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY:
		clean_cvtaskmsg_send_at_time(msg);
		break;

	case SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME:
	case SCHEDMSG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME:
		clean_cvtaskmsg_bulk_send(msg);
		break;

	case SCHEDMSG_CVTASK_QUERY_REPLY:
		clean_cvtask_query(msg);
		break;

	case SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY:
		clean_get_buildinfo(msg);
		break;

	case SCHEDMSG_CVTASK_RUN_REPLY:
	case SCHEDMSG_CVTASK_MSGONLY_REPLY:
	case SCHEDMSG_CVTASK_FINISH_REPLY:
		clean_cvtask_run(msg);
		/* flush log after each run */
		clean_scheduler_log();
		break;

	case SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY:
		clean_cvtask_partial_run(msg);
		break;

	case SCHEDMSG_BOOT_SETUP1_REPLY:
		clean_boot_setup1(msg);
		break;

	case SCHEDMSG_BOOT_SETUP2_REPLY:
		clean_boot_setup2(msg);
		break;

	case SCHEDMSG_INITIAL_SETUP_REPLY:
	case SCHEDMSG_BOOT_SETUP3_REPLY:
	case SCHEDMSG_CVTASK_REGISTER_REPLY:
	case SCHEDMSG_CVTASK_INIT_REPLY:
	case SCHEDMSG_SET_DEBUG_LEVEL:
	case SCHEDMSG_CVSCHEDULER_RUN:
	case SCHEDMSG_SCHEDULER_SHUTDOWN:
	case SCHEDMSG_SET_FREQUENCY:
	case SCHEDMSG_SET_FREQUENCY_AT_FRAME:
	case SCHEDMSG_SET_FREQUENCY_AT_TIME:
	case SCHEDMSG_ARM_REQUESTS_SHUTDOWN:
	case SCHEDMSG_WAKE_FEEDERS:
	case SCHEDMSG_HWUNIT_RESET_REPLY:
	case SCHEDMSG_INVALIDATE_CACHE:
	case SCHEDMSG_FLUSH_VISPRINTF_REQ:
	case SCHEDMSG_LICENSE_UPDATE:
	case SCHEDMSG_TIME_REPORT:
		// these messages have nothing to clean
		break;

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

	// Flexidag related =-------------------------------------------------------
	case SCHEDMSG_FLEXIDAG_CVTASK_MSG:
		clean_cvtaskmsg_send(msg);
		break;
	case SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME:
		clean_cvtaskmsg_send_at_time(msg);
		break;
	case SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME:
		clean_cvtaskmsg_bulk_send(msg);
		break;
	case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
		clean_get_buildinfo(msg);
		break;
	case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
		clean_cvtask_query(msg);
		break;
	case SCHEDMSG_FLEXIDAG_CVRUN_REPLY:
	case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REPLY:
	case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REPLY:
		clean_cvtask_run(msg);
		clean_scheduler_log();
		break;
	case SCHEDMSG_FLEXIDAG_CVRUN_PARTIAL_REPLY:
		clean_cvtask_partial_run(msg);
		clean_scheduler_log();
		break;
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
		// these messages have nothing to clean
		break;

	case SCHEDMSG_SEND_PRIVATE_MSG:
	case SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG:
		// these messages have nothing to clean
		break;

	default:
		pr_err("Error!! clean_msg unknown armvis_msg_t type 0x%X\n",
		       msg->hdr.message_type);
	} /* switch (msg->hdr.message_type) */

	ambacv_cache_clean((char*)msg, sizeof(*msg));

	if (enable_log_msg)
		schdrmsg_log_msg(msg);
}

int cache_ioctl_clean(unsigned long arg)
{
	ambacv_mem_region_t region;
	int ret;

	ret = copy_from_user(&region, (void*)arg, sizeof(region));
	if (ret != 0) {
		pr_err("Error!! cache_ioctl_clean");
		return ret;
	}

	if (debug_level >= 2)
	{
		if ((region.base & (ARM_CACHELINE_SIZE-1)) ||
		    (region.size & (ARM_CACHELINE_SIZE-1)))
		{
			pr_warn("Warning! ambacv cache clean is not 64B aligned\n");
		}
	} /* if (debug_level >= 2) */

	ambacv_cache_clean(ambacv_p2v(region.base), region.size);
	return 0;
}

int cache_ioctl_invld(unsigned long arg)
{
	ambacv_mem_region_t region;
	int ret;

	ret = copy_from_user(&region, (void*)arg, sizeof(region));
	if (ret != 0) {
		pr_err("Error!! cache_ioctl_clean");
		return ret;
	}

	if (debug_level >= 2)
	{
		if ((region.base & (ARM_CACHELINE_SIZE-1)) ||
		    (region.size & (ARM_CACHELINE_SIZE-1)))
		{
			pr_warn("Warning! ambacv cache invalidate is not 64B aligned\n");
		}
	} /* if (debug_level >= 2) */

	ambacv_cache_invalidate(ambacv_p2v(region.base), region.size);
	return 0;
}

void ambacv_cache_clean(void *ptr, uint64_t size)
{
	uint64_t start = 0UL, end = 0UL;

	if(size != 0U) {
		start = ambacv_v2p(ptr);
		end = start + size -1;
		if (debug_level >= 2)
		{
			if (start & 0x3F) {
				pr_info("dram_clean 0x%llX, 0x%llx\n", start, size);
			}
		} /* if (debug_level >= 2) */

		if(ambacv_p2v(start)== NULL) {
			pr_err("[ERROR] ambacv_cache_clean : start address invalidate va 0x%px, size 0x%llx\n", ptr, size);
			dump_memory_range();
			dump_stack();
		} else if (ambacv_p2v(end)== NULL){
			pr_err("[ERROR] ambacv_cache_clean : end address invalidate va 0x%px, pa 0x%llx size 0x%llx\n", ptr, start, size);
			dump_memory_range();
			dump_stack();
		} else {
			ambcache_clean_range(ptr, size);
		}
	}
}

void ambacv_cache_invalidate(void *ptr, uint64_t size)
{
	uint64_t start = 0UL, end = 0UL;

	if(size != 0U) {
		start = ambacv_v2p(ptr);
		end = start + size -1;

		if(ambacv_p2v(start)== NULL) {
			pr_err("[ERROR] ambacv_cache_invalidate : start address invalidate va 0x%p, size 0x%llx\n", ptr, size);
			dump_memory_range();
			dump_stack();
		} else if (ambacv_p2v(end)== NULL){
			pr_err("[ERROR] ambacv_cache_invalidate : end address invalidate va 0x%p, pa 0x%llx size 0x%llx\n", ptr, start, size);
			dump_memory_range();
			dump_stack();
		} else {
			ambcache_inv_range(ptr, size);
		}
	}
}
