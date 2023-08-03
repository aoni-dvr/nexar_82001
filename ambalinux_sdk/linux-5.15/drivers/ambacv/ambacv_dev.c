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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/of_irq.h>
#include <linux/seq_file.h>
#include <linux/clk.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/chip.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include <linux/uaccess.h>
#include "ambacv_kal.h"

#include "flexidag_ioctl.h"

#include <cavalry_ioctl.h>          /* from highest level include/cavalry directory */
#include "cavalry_cvflow_ioctl.h"   /* From local directory */
#include "cavalry.h"
#include "cavalry_mem.h"    /* For cavalry/MEM interface */
#include "cavalry_log.h"    /* For cavalry/Log interface */
#include "cavalry_cma.h"    /* For cavalry/CMA interface */
#include "cavalry_print.h"  /* for cavalry/Print interfaces */
#include "cavalry_misc.h"   /* for cavalry/clock interfaces */
#include "cavalry_stats.h"  /* for cavalry/stats interfaces */
#include "schdr_api.h"      /* for scheduler_trace_t interfaces */
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

MODULE_AUTHOR("Joey Li");
MODULE_LICENSE("GPL");

#define DEVICE_NAME     "ambacv"

ambacv_mem_t       ambacv_global_mem;
ambacv_mem_region_t ambacv_debug_port;
ambacv_log_t       ambacv_global_log;

int    enable_arm_cache   = 1;
int    enable_rtos_cache  = 1;
int    enable_orc_cache   = 1;
int    enable_log_msg     = 0;
int    enable_cma         = 0;
int    scheduler_id;

#define DEFAULT_CMA_SUPERDAG_SIZE     32

static int cma_superdag_size  = -1;

static unsigned int     ambacv_major;
static unsigned int     ambacv_irq;
static struct class*    ambacv_class;
static struct device*   ambacv_device;
static struct mutex     ambacv_core_mutex;

static void __iomem     *ambacv_sysinit_vaddr;
static void __iomem     *ambacv_cvmem_vaddr;
static visorc_safety_heartbeat_t *ambacv_r52_trace;

#if defined(ENABLE_AMBA_MAL)
static AMBA_MAL_BUF_s SchdrMem;
AMBA_MAL_INFO_s     krn_schdr_core_info;
AMBA_MAL_INFO_s     krn_schdr_cma_info;
AMBA_MAL_INFO_s     krn_schdr_rtos_info;
AMBA_MAL_INFO_s     krn_dsp_data_info;
#endif
struct ambarella_cavalry  cavalry_support;
struct ambarella_cavalry *pCavalrySupport;

uint64_t cv_pa_start = 0UL,cv_pa_end = 0xFFFFFFFFUL;
uint64_t cv_ca_start = 0UL,cv_ca_end = 0xFFFFFFFFUL;

uint32_t cluster_id = 0;
uint32_t ambacv_state = 0;
uint32_t debug_level = 0;
extern uint32_t audio_clock_ms;
extern cvprintf_buffer_info_t *print_info;

module_param(debug_level, uint, 0644);
MODULE_PARM_DESC(debug_level, "driver debug level. 0: err; 1: info; 2: debug");

module_param(enable_arm_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_arm_cache, "make cv memory cacheable to ARM");
module_param(enable_rtos_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_rtos_cache, "make cv rtos memory cacheable to ARM");
module_param(enable_orc_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_arm_cache, "enable ORC cache");
module_param(enable_log_msg, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_log_msg, "enable message log");
module_param(cma_superdag_size, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(cma_superdag_size , "Size reserved for SuperDAG when CMA is enabled");

uint32_t ucode_log_level = 2;
module_param(ucode_log_level, uint, 0644);
MODULE_PARM_DESC(ucode_log_level, "Cavalry ucode log level. 0: SILENT; 1: MINIMAL; 2: NORMAL; 3: VERBOSE; 4: DEBUG");

static void __iomem     *rtos_io_base = NULL;
static void __iomem     *schdr_mem_base = NULL;
static void __iomem     *dsp_data_base = NULL;

/*================================================================================================*/
/*=- CVSCHED support files: will be placed in another file in the future -========================*/
/*================================================================================================*/
typedef struct
{
  uint64_t  scheduler_trace_daddr;
  void     *vpSchedulerTrace;
  uint64_t  autorun_trace_daddr;
  void     *vpAutoRunTrace;
  struct proc_dir_entry  *cvload_proc;
  struct proc_dir_entry  *autorun_proc;
  struct proc_dir_entry  *cvstatus_proc;
} cvflow_scheduler_support_t;

cvflow_scheduler_support_t  scheduler_support;

void scheduler_system_set_trace_daddr(uint64_t trace_daddr)
{
  if (trace_daddr != 0)
  {
    if (trace_daddr != scheduler_support.scheduler_trace_daddr)
    {
      scheduler_support.scheduler_trace_daddr = trace_daddr;
      scheduler_support.vpSchedulerTrace      = ambacv_c2v(trace_daddr);
    } /* if (trace_daddr != scheduler_trace_daddr) */
  } /* if (trace_daddr != 0) */
} /* scheduler_system_set_scheduler_trace() */

uint64_t scheduler_system_get_trace_daddr(void)
{
  return scheduler_support.scheduler_trace_daddr;
} /* scheduler_system_get_trace() */

static int cvload_proc_show(struct seq_file *m, void *v)
{
  cvflow_scheduler_support_t *pSysSupport;

  pSysSupport = (cvflow_scheduler_support_t *)m->private;

  if (pSysSupport->vpSchedulerTrace != NULL)
  {
    uint32_t  thread_loop;
    scheduler_trace_t *pSchedulerTrace;
    cvthread_trace_t  *pCVThreadLoad;

    pSchedulerTrace = (scheduler_trace_t *)pSysSupport->vpSchedulerTrace;
    ambacv_cache_invalidate(pSchedulerTrace, sizeof(scheduler_trace_t));
    pCVThreadLoad   = &pSchedulerTrace->cvthread_loadtrace[0];
    ambacv_cache_invalidate(pCVThreadLoad, sizeof(cvthread_trace_t));
    seq_printf(m, "--- CORE NAME ---+- rpt. time -+--500ms--+-- 2.5s--+--10.0s--\n");

    for (thread_loop = 0; thread_loop < CVTRACE_MAX_ENTRIES; thread_loop++)
    {
      uint32_t   *pRecast;

      pRecast = (uint32_t *)pCVThreadLoad;

      if (pCVThreadLoad->cvcore_name[0] != 0)
      {
        uint32_t  cvload_report_time  = pCVThreadLoad->cvload_report_time;
        uint32_t  cvload_500ms        = pCVThreadLoad->cvload_500ms;
        uint32_t  cvload_2500ms       = pCVThreadLoad->cvload_2500ms;
        uint32_t  cvload_10000ms      = pCVThreadLoad->cvload_10000ms;
        uint32_t  cvload_500ms_a, cvload_500ms_b;
        uint32_t  cvload_2500ms_a, cvload_2500ms_b;
        uint32_t  cvload_10000ms_a, cvload_10000ms_b;

        if (cvload_500ms   > (audio_clock_ms*500U))   cvload_500ms = (audio_clock_ms*500U);
        if (cvload_2500ms  > (audio_clock_ms*2500U))  cvload_2500ms = (audio_clock_ms*2500U);
        if (cvload_10000ms > (audio_clock_ms*10000U)) cvload_10000ms = (audio_clock_ms*10000U);
        cvload_500ms_a    = cvload_500ms / (audio_clock_ms*5U);
        cvload_500ms_b    = (cvload_500ms - (cvload_500ms_a * (audio_clock_ms*5U))) / (audio_clock_ms/20U);
        cvload_2500ms_a   = cvload_2500ms / (audio_clock_ms*25U);
        cvload_2500ms_b   = (cvload_2500ms - (cvload_2500ms_a * (audio_clock_ms*25U))) / (audio_clock_ms/4U);
        cvload_10000ms_a  = cvload_10000ms / (audio_clock_ms*100U);
        cvload_10000ms_b  = (cvload_10000ms - (cvload_10000ms_a * (audio_clock_ms*100U))) / audio_clock_ms;

        seq_printf(m, "VIS:%12s | @%10u | %3d.%02d%% | %3d.%02d%% | %3d.%02d%%\n",
                   pCVThreadLoad->cvcore_name, cvload_report_time,
                   cvload_500ms_a, cvload_500ms_b, cvload_2500ms_a, cvload_2500ms_b, cvload_10000ms_a, cvload_10000ms_b);
      } /* if (pCVThreadLoad->cvcore_name[0] != 0) */
      pCVThreadLoad++;
    } /* for (thread_loop = 0; thread_loop < CVTRACE_MAX_ENTRIES; thread_loop++) */
  } /* if (pSysSupport->vpSchedulerTrace != NULL) */

  return 0;
}

static int cvload_proc_open(struct inode *inode, struct file *file)
{
  return single_open(file, cvload_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations cvload_proc_fops = {
  .open = cvload_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
};
#else
static const struct proc_ops cvload_proc_fops = {
  .proc_open = cvload_proc_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
};
#endif
/*================================================================================================*/
/*=- AUTORUN support files : will be placed in another file in the future -=======================*/
/*================================================================================================*/
void scheduler_autorun_set_trace_daddr(uint64_t trace_daddr)
{
  if (trace_daddr != 0)
  {
    if (trace_daddr != scheduler_support.autorun_trace_daddr)
    {
      scheduler_support.autorun_trace_daddr = trace_daddr;
      scheduler_support.vpAutoRunTrace      = ambacv_c2v(trace_daddr);
    } /* if (trace_daddr != scheduler_support.autorun_trace_daddr) */
  } /* if (trace_daddr != 0) */
} /* scheduler_autorun_set_trace_daddr() */

uint64_t scheduler_autorun_get_trace_daddr(void)
{
  return scheduler_support.autorun_trace_daddr;
} /* scheduler_autorun_get_trace_daddr() */

static int autorun_proc_show(struct seq_file *m, void *v)
{
  cvflow_scheduler_support_t *pSysSupport;

  pSysSupport = (cvflow_scheduler_support_t *)m->private;

  if (pSysSupport->vpAutoRunTrace != NULL)
  {
    autorun_trace_t *pAutoRunTraceEntry;
    uint32_t  num_entries;
    pAutoRunTraceEntry = (autorun_trace_t *)pSysSupport->vpAutoRunTrace;
    ambacv_cache_invalidate(pAutoRunTraceEntry, (sizeof(autorun_trace_t) * MAX_AUTORUN_CVTASKS));
    num_entries = pAutoRunTraceEntry->autorun_cvtask_num;
    if (num_entries == 0)
    {
      seq_printf(m, "[AUTORUN] : No AutoRUN tasks exist in the system\n");
    } /* if (num_entries == 0) */
    else if (num_entries < MAX_AUTORUN_CVTASKS)
    {
      uint32_t  loop;
      seq_printf(m, "--- AUTORUN_CVTASK_NAME -------- : per(ms) | ptime(ms) |   num_pass @(  passtime) |   num_fail @(  failtime) | failrc\n");
      for (loop = 0; loop < num_entries; loop++)
      {
        seq_printf(m, "%32s :   %5u |     %5u | %10u @(%10u) | %10u @(%10u) | 0x%08x\n",
                   &pAutoRunTraceEntry->autorun_cvtask_name[0],
                   pAutoRunTraceEntry->autorun_period_ms,
                   pAutoRunTraceEntry->autorun_timeout_ms,
                   pAutoRunTraceEntry->autorun_num_pass,
                   pAutoRunTraceEntry->autorun_last_pass_tick,
                   pAutoRunTraceEntry->autorun_num_fail,
                   pAutoRunTraceEntry->autorun_last_fail_tick,
                   pAutoRunTraceEntry->autorun_last_fail_retcode);
        pAutoRunTraceEntry++;
      } /* for (loop = 0; loop < num_entries; loop++) */
    } /* if (num_entries < MAX_AUTORUN_CVTASKS) */
    else /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
    {
      seq_printf(m, "[AUTORUN] : System corrupted : MAX_AUTORUN_CVTASKS = %d, num_entries in table = %d\n", MAX_AUTORUN_CVTASKS, num_entries);
    } /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
  } /* if (pSysSupport->vpAutoRunTrace != NULL) */

  return 0;
}

static int autorun_proc_open(struct inode *inode, struct file *file)
{
  return single_open(file, autorun_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations autorun_proc_fops = {
  .open = autorun_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
};
#else
static const struct proc_ops autorun_proc_fops = {
  .proc_open = autorun_proc_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
};
#endif
/*================================================================================================*/
/*=- CVStatus support files : will be placed in another file in the future -======================*/
/*================================================================================================*/
static int cvstatus_proc_show(struct seq_file *m, void *v)
{
  if (ambacv_r52_trace != NULL)
  {
    uint32_t hashval;
    ambacv_cache_invalidate(ambacv_r52_trace, sizeof(visorc_safety_heartbeat_t));
    hashval = schdrmsg_calc_hash(ambacv_r52_trace, sizeof(visorc_safety_heartbeat_t) - 4, FNV1A_32_INIT_HASH);
    if (hashval == ambacv_r52_trace->fnv1a_checksum)
    {
      seq_printf(m, "[%px] @ [0x%llx] : 0x%08x 0x%08x 0x%08x 0x%08x [CS pass]\n",
                 ambacv_r52_trace, ambacv_v2c(ambacv_r52_trace), ambacv_r52_trace->hb_sequence_no, ambacv_r52_trace->hb_time, ambacv_r52_trace->schdr_state, ambacv_r52_trace->fnv1a_checksum);
    } /* if (hashval == ambacv_r52_trace->fnv1a_checksum) */
    else /* if (hashval != ambacv_r52_trace->fnv1a_checksum) */
    {
      seq_printf(m, "[%px] @ [0x%llx] : 0x%08x 0x%08x 0x%08x 0x%08x [CS fail : 0x%08x]\n",
                 ambacv_r52_trace, ambacv_v2c(ambacv_r52_trace) , ambacv_r52_trace->hb_sequence_no, ambacv_r52_trace->hb_time, ambacv_r52_trace->schdr_state, ambacv_r52_trace->fnv1a_checksum, hashval);
    } /* if (hashval != ambacv_r52_trace->fnv1a_checksum) */
  } /* if (ambacv_r52_trace != NULL) */

  return 0;

}

static int cvstatus_proc_open(struct inode *inode, struct file *file)
{
  return single_open(file, cvstatus_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations cvstatus_proc_fops = {
  .open = cvstatus_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
};
#else
static const struct proc_ops cvstatus_proc_fops = {
  .proc_open = cvstatus_proc_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
};
#endif

void krn_scheduler_system_init(void)
{
  memset(&scheduler_support, 0, sizeof(cvflow_scheduler_support_t));

  scheduler_support.cvload_proc = proc_create_data("cvload", S_IRUGO,
      get_ambarella_proc_dir(), &cvload_proc_fops, &scheduler_support);
  scheduler_support.autorun_proc = proc_create_data("autorun", S_IRUGO,
      get_ambarella_proc_dir(), &autorun_proc_fops, &scheduler_support);
#ifdef ASIL_COMPLIANCE
  scheduler_support.cvstatus_proc = proc_create_data("cvstatus", S_IRUGO,
      get_ambarella_proc_dir(), &cvstatus_proc_fops, &scheduler_support);
#else /* !ASIL_COMPLIANCE */
  scheduler_support.cvstatus_proc = NULL;
#endif /* ?ASIL_COMPLIANCE */

  pr_info("Scheduler system initialized\n");

} /* krn_scheduler_system_init() */

void krn_scheduler_system_shutdown(void)
{
  if (scheduler_support.cvload_proc != NULL)
  {
    remove_proc_entry("cvload", get_ambarella_proc_dir());
    scheduler_support.cvload_proc = NULL;
  } /* if (scheduler_support.cvload_proc != NULL) */
  if (scheduler_support.autorun_proc != NULL)
  {
    remove_proc_entry("autorun", get_ambarella_proc_dir());
    scheduler_support.autorun_proc = NULL;
  } /* if (scheduler_support.autorun_proc != NULL) */
  if (scheduler_support.cvstatus_proc != NULL)
  {
    remove_proc_entry("cvstatus", get_ambarella_proc_dir());
    scheduler_support.cvstatus_proc = NULL;
  } /* if (scheduler_support.cvstatus_proc  != NULL) */
  pr_info("Scheduler system shutdown\n");
} /* krn_scheduler_system_shutdown() */

visorc_init_params_t *ambacv_get_sysinit_addr(void)
{
	return (visorc_init_params_t *)ambacv_sysinit_vaddr;
}

#if defined(ENABLE_AMBA_MAL)
static int ambacv_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int rval;
	int allocated;
	uint64_t base = 0UL, size = 0UL, end = 0UL;

	allocated = 0;
	base = vma->vm_pgoff << 12;
	size = vma->vm_end - vma->vm_start;
	end  =  base + size;

	if ((base >= ambacv_c2p(cavalry_support.cavalry_uncached_logblock.phys)) &&
	    (end <= (ambacv_c2p(cavalry_support.cavalry_uncached_logblock.phys) + cavalry_support.cavalry_uncached_logblock.size)))
	{
		// region is cavalry log
		vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
	} else if (base >= ambacv_debug_port.base && end <= (ambacv_debug_port.base + ambacv_debug_port.size)) {
		// region is debug port
		vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
	} else {
		// region is illegal
		prt_err("ambacv_mmap : region is illegal(base=0x%llx end=0x%llx)\n", base, end);
		return -EPERM;
	}

	if (allocated == 0)
	{
		rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
		                       vma->vm_end - vma->vm_start, vma->vm_page_prot);
		if (rval)
		{
			pr_err("ambacv_mmap %d\n", rval);
			return rval;
		}
	}

	//pr_info("ambacv: mmap region [0x%X -- 0x%X]\n", base, base + size);
	return rval;
}

#else
static void cavalry_cma_vm_open(struct vm_area_struct *vma)
{
	cavalry_cma_reference(pCavalrySupport, vma->vm_private_data);

	return;
}

static void cavalry_cma_vm_close(struct vm_area_struct *vma)
{
	cavalry_cma_unreference(pCavalrySupport, vma->vm_private_data);

	return;
}

static const struct vm_operations_struct cavalry_cma_vm_ops = {
	.open = cavalry_cma_vm_open,
	.close = cavalry_cma_vm_close,
};

static int ambacv_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int rval;
	int allocated;
	uint64_t base = 0UL, size = 0UL, end = 0UL;

	allocated = 0;
	base = vma->vm_pgoff << 12;
	size = vma->vm_end - vma->vm_start;
	end  =  base + size;

	if ((base >= ambacv_c2p(cavalry_support.cavalry_uncached_logblock.phys)) &&
	    (end <= (ambacv_c2p(cavalry_support.cavalry_uncached_logblock.phys) + cavalry_support.cavalry_uncached_logblock.size)))
	{
		// region is cavalry log
		vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
	}
	else if ((base >= ambacv_c2p(ambacv_global_mem.all.base)) &&
	    (end <= ambacv_c2p(ambacv_global_mem.all.base + ambacv_global_mem.all.size))) {
		// region is within cv shared memory
		if (!enable_arm_cache)
		{
			vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
		} /* if (!enable_arm_cache) */

		if ((ambacv_global_mem.cavalry_region.base != 0) && (ambacv_global_mem.cavalry_region.size != 0))
		{
			if ((base >= ambacv_c2p(ambacv_global_mem.cavalry_region.base)) && (end <= ambacv_c2p(ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size)))
			{
				vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
			} /* if ((base >= ambacv_global_mem.cavalry_region.base) && (end <= (ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size))) */
		} /* if ((ambacv_global_mem.cavalry_region.base != 0) && (ambacv_global_mem.cavalry_region.size != 0)) */
	} else if (base >= ambacv_debug_port.base && end <= (ambacv_debug_port.base + ambacv_debug_port.size)) {
		// region is debug port
		vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
	}
	else if ((base >= ambacv_c2p(ambacv_global_mem.rtos_region.base)) &&
	         (end <= ambacv_c2p(ambacv_global_mem.rtos_region.base + ambacv_global_mem.rtos_region.size)))
	{
		if (enable_rtos_cache == 0)
		{
			vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
		} /* if (enable_rtos_cache == 0) */
	} /* if (base >= ambacv_global_mem.rtos_region.base) && (end <= (ambacv_global_mem.rtos_region.base + ambacv_global_mem.rtos_region.size)) */
	else if (enable_cma != 0)
	{
		if ((base == ambacv_c2p(ambacv_global_mem.cavalry_region.base)) && (end == ambacv_c2p(ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size))) /* Specialized case to disable prints */
		{
			if (!enable_arm_cache)
			{
				vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
			} /* if (!enable_arm_cache) */
		} /* if ((base == ambacv_global_mem.cavalry_region.base) && (end == (ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size))) */
		else if ((base >= ambacv_c2p(ambacv_global_mem.cavalry_region.base)) && (end <= ambacv_c2p(ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size)))
		{
			/* from cavalry_dev.c : cavalry_mmap() */
			struct ambarella_cavalry *cavalry = &cavalry_support;
			struct cma_mmb *mmb = NULL, *_mmb = NULL;
			uint32_t   found = 0;

			if (end <= ambacv_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys)){
				/* top 6MB memory */
				vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
				found = 1;
			} else {
				/* user memory */
				mutex_lock(&cavalry->cavalry_cma_mutex);
				if (!list_empty(&cavalry->mmb_list)) {
					list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
						if ((base >= mmb->phys_addr) && (base < mmb->phys_addr + mmb->size)) {
							if (!mmb->cache_en) {
								vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
							}
							found = 1;
							break;
						}
					}
				}
				mutex_unlock(&cavalry->cavalry_cma_mutex);
			}

			if (!found)
			{
				prt_err("mmap not found in CMA region, assuming cached direct mapping (base=0x%llx size=0x%llx)\n", base, size);
			}
			else
			{
				rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
				                       vma->vm_end - vma->vm_start, vma->vm_page_prot);
				if (rval) {
					prt_err("mmap pfg rang err: %d\n", rval);
				} else {
					vma->vm_ops = &cavalry_cma_vm_ops;
					vma->vm_private_data = mmb;
					cavalry_cma_vm_open(vma);
				}
				allocated = 1;
			}
		} /* if ((base >= ambacv_global_mem.cavalry_region.base) && (end <= (ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size))) */
		else
		{
			// region is illegal
			return -EPERM;
		}
	}
	else
	{
		// region is illegal
		return -EPERM;
	}

	if (allocated == 0)
	{
		rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
		                       vma->vm_end - vma->vm_start, vma->vm_page_prot);
		if (rval)
		{
			pr_err("ambacv_mmap %d\n", rval);
			return rval;
		}
	}

	//pr_info("ambacv: mmap region [0x%X -- 0x%X]\n", base, base + size);
	return rval;
}
#endif

static long ioctl_get_ambacv_state(unsigned long arg)
{
	uint32_t  kernel_state;
	int ret;

	kernel_state = ambacv_state;

	ret = copy_to_user((void *)arg, &kernel_state, sizeof(kernel_state));

	return ret;
} /* ioctl_get_ambacv_state() */

int ambacv_get_att(unsigned long arg)
{
	ambacv_att_region_t local;

	local.ca = cv_ca_start;
	local.pa = cv_pa_start;
	local.size = cv_pa_end - cv_pa_start;
	return copy_to_user((void*)arg, &local, sizeof(local));
}

#if defined(ENABLE_AMBA_MAL)
static long ambacv_get_mmb_info(unsigned long *arg)
{
    (void)arg;
	prt_err("ambacv_get_mmb_info() : not support\n");

	return 0;
}
#else
static long ambacv_get_mmb_info(unsigned long *arg)
{
	int ret;
	ambacv_get_info_t info;

	ret = copy_from_user(&info, (void*)arg, sizeof(ambacv_get_info_t));
	if (ret == 0) {
		if (info.index[0] == 0x1) {
			if ((info.index[1] >= ambacv_c2p(ambacv_global_mem.all.base)) &&
				(info.index[1] < ambacv_c2p(ambacv_global_mem.all.base + ambacv_global_mem.all.size))) {
				info.value[0] = enable_arm_cache;
			} else if ((info.index[1] >= ambacv_c2p(ambacv_global_mem.rtos_region.base)) &&
				(info.index[1] < ambacv_c2p(ambacv_global_mem.rtos_region.base + ambacv_global_mem.rtos_region.size))) {
				info.value[0] = enable_rtos_cache;
			} else if ((info.index[1] >= ambacv_c2p(ambacv_global_mem.cavalry_region.base)) &&
				(info.index[1] < ambacv_c2p(ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size))) {
				/* from cavalry_dev.c : cavalry_mmap() */
				struct cma_mmb *mmb = NULL, *_mmb = NULL;
				uint32_t   found = 0;

				/* user memory */
				mutex_lock(&cavalry_support.cavalry_cma_mutex);
				if (!list_empty(&cavalry_support.mmb_list)) {
					list_for_each_entry_safe(mmb, _mmb, &cavalry_support.mmb_list, list) {
						if ((info.index[1] >= mmb->phys_addr) && (info.index[1] < mmb->phys_addr + mmb->size)) {
							info.value[0] = mmb->cache_en;
							found = 1;
							break;
						}
					}
				}
				mutex_unlock(&cavalry_support.cavalry_cma_mutex);

				if (!found) {
					prt_err("addr 0x%llx not found in CMA region\n", info.index[1]);
					ret = -1;
				}
			} else {
				prt_err("addr 0x%llx not found in ambacv region\n", info.index[1]);
				ret = -1;
			}
		} else {
			prt_err("unknown index 0x%llx\n", info.index[0]);
			ret = -1;
		}
	}

	if (ret == 0) {
		ret = copy_to_user((void*)arg, &info, sizeof(ambacv_get_info_t));
	}

	return ret;
}
#endif

/*------------------------------------------------------------------------------------------------*/
/*-= AmbaCV core token management (IOCTL/KERNEL) =------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
#define   MAX_AMBACV_TOKENS         32
#define   AMBACV_TOKEN_VALUE_BASE   AMBACV_ID_WAIT_SCHDR_TOKEN_BASE

typedef struct
{
  struct mutex            token_mutex;
  struct semaphore        token[MAX_AMBACV_TOKENS];
  uint32_t                next_token_index;
  uint8_t                 token_state[MAX_AMBACV_TOKENS];
  errcode_enum_t          token_retcode[MAX_AMBACV_TOKENS];
} ambacv_core_state_t;

#define AMBACV_TOKEN_UNUSED     0
#define AMBACV_TOKEN_USED       1
#define AMBACV_TOKEN_WAITING    2
#define AMBACV_TOKEN_FINISHED   3

ambacv_core_state_t ambacv_core_state;

void ambacv_token_init(void)
{
  uint32_t  loop;

  memset(&ambacv_core_state, 0, sizeof(ambacv_core_state_t));
  mutex_init(&ambacv_core_state.token_mutex);
  mutex_lock(&ambacv_core_state.token_mutex);

  for (loop = 0; loop < MAX_AMBACV_TOKENS; loop++)
  {
    ambacv_core_state.token_state[loop]   = AMBACV_TOKEN_UNUSED;
    ambacv_core_state.token_retcode[loop] = ERRCODE_NONE;
    sema_init(&(ambacv_core_state.token[loop]), 0);
  } /* for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++) */

  mutex_unlock(&ambacv_core_state.token_mutex);

} /* ambacv_token_init() */

uint32_t  ambacv_find_token(void)
{
  uint32_t  token_found;
  uint32_t  token_num;
  uint32_t  token_id;
  uint32_t  retcode;
  uint32_t  loop;

  token_found = 0;
  token_num   = 0;
  loop        = 0;

  mutex_lock(&ambacv_core_state.token_mutex);

  while ((token_found == 0) && (loop < MAX_AMBACV_TOKENS))
  {
    token_id = (ambacv_core_state.next_token_index + loop) % MAX_AMBACV_TOKENS;
    if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED)
    {
      token_found = 1;
      token_num   = token_id;
      ambacv_core_state.token_state[token_id] = AMBACV_TOKEN_USED;
      ambacv_core_state.next_token_index      = (token_id + 1) % MAX_AMBACV_TOKENS;
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED) */
    loop++;
  } /* while ((token_found == 0) && (loop < MAX_AMBACV_TOKENS)) */
  mutex_unlock(&ambacv_core_state.token_mutex);

  if (token_found == 0)
  {
    /* ERROR */
    pr_err("[ERROR] : ambacv_find_token() : Unable to find token in system\n");
    retcode = ERR_DRV_SCHDR_TOKEN_UNABLE_TO_FIND;
  } /* if (token_found == 0) */
  else /* if (token_found != 0) */
  {
    retcode = token_num + AMBACV_TOKEN_VALUE_BASE;
  } /* if (token_found != 0) */

  return retcode;

} /* ambacv_find_token() */

errcode_enum_t  ambacv_wait_token(uint32_t token_id)
{
  uint32_t  wait_valid;
  errcode_enum_t  retcode;

  retcode       = ERRCODE_NONE;
  token_id     -= AMBACV_TOKEN_VALUE_BASE;

  if (token_id >= MAX_AMBACV_TOKENS)
  {
    /* ERROR */
    wait_valid  = 0;
    retcode     = ERR_DRV_SCHDR_TOKEN_OUT_OF_RANGE;
  } /* if (token_id >= MAX_AMBACV_TOKENS) */
  else /* if (token_id < MAX_AMBACV_TOKENS) */
  {
    if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED) /* release called before wait */
    {
      wait_valid = 1;
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED) */
    else if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED)
    {
      wait_valid = 0; /* Do nothing here */
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_UNUSED) */
    else if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) /* wait called before release */
    {
      ambacv_core_state.token_state[token_id] = AMBACV_TOKEN_WAITING;
      wait_valid = 1;
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) */
    else /* if (ambacv_core_state.token_state[token_id] == [invalid state]) */
    {
      /* ERROR */
      wait_valid  = 0;
      pr_err("[CORE] [ERROR] : ambacv_wait_token(token_id=%3d) : Token state invalid (%d, expected %d)\n",
             token_id, ambacv_core_state.token_state[token_id], AMBACV_TOKEN_USED);
      retcode     = ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED;
    } /* if (ambacv_core_state.token_state[token_id] == [invalid state]) */
  } /* if (token_id < MAX_AMBACV_TOKENS) */

  mutex_unlock(&ambacv_core_state.token_mutex);

  if (wait_valid != 0)
  {
    int ret;

    ret = down_timeout(&ambacv_core_state.token[token_id], 10000);

    mutex_lock(&ambacv_core_state.token_mutex);
    if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED)
    {
      retcode = ambacv_core_state.token_retcode[token_id];
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_FINISHED) */
    else /* if (ambacv_core_state.token_state[token_id] != AMBACV_TOKEN_FINISHED) */
    {
      pr_err("[CORE] [ERROR] : ambacv_wait_token(token_id=%3d) : Token state invalid (%d, expected %d)\n",
             token_id, ambacv_core_state.token_state[token_id], AMBACV_TOKEN_FINISHED);
      retcode = ERR_DRV_SCHDR_TOKEN_STATE_MISMATCHED;
    } /* if (ambacv_core_state.token_state[token_id] != AMBACV_TOKEN_FINISHED) */
    ambacv_core_state.token_state[token_id] = AMBACV_TOKEN_UNUSED;
    mutex_unlock(&ambacv_core_state.token_mutex);
  } /* if (wait_valid != 0) */

  return retcode;

} /* ambacv_wait_token() */

void ambacv_release_token(uint32_t token_id, errcode_enum_t function_retcode)
{
  uint32_t  release_valid;

  token_id     -= AMBACV_TOKEN_VALUE_BASE;
  if (token_id >= MAX_AMBACV_TOKENS)
  {
    /* ERROR */
    release_valid = 0;
  } /* if (token_id >= MAX_AMBACV_TOKENS) */
  else /* if (token_id < MAX_AMBACV_TOKENS) */
  {
    mutex_lock(&ambacv_core_state.token_mutex);
    if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) /* release is called before wait */
    {
      release_valid = 1;
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_USED) */
    else if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_WAITING) /* wait is called before release */
    {
      release_valid = 1;
    } /* if (ambacv_core_state.token_state[token_id] == AMBACV_TOKEN_WAITING) */
    else /* invalid state */
    {
      release_valid = 0;
      pr_err("[ERROR] : ambacv_release_token(token_id=%3d, retcode=0x%x) : Token state invalid (%d, expected %d)\n",
             token_id, function_retcode, ambacv_core_state.token_state[token_id], AMBACV_TOKEN_WAITING);
    } /* invalid state */

    if (release_valid != 0)
    {
      ambacv_core_state.token_state[token_id]       = AMBACV_TOKEN_FINISHED;
      ambacv_core_state.token_retcode[token_id]     = function_retcode;
    } /* if (release_valid != 0) */

    mutex_unlock(&ambacv_core_state.token_mutex);

    if (release_valid != 0)
    {
      up(&ambacv_core_state.token[token_id]);
    }
  } /* if (token_id < MAX_AMBACV_TOKENS) */
} /* ambacv_release_token() */

uint64_t core_flexidag_trace_daddr = 0UL;
static long ambacv_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	uint32_t time, version, value;
	ambacv_get_value_t ambacv_value;
	ambacv_log_t arg_log;

	switch (cmd) {
	case AMBACV_START_VISORC:
		ambacv_state = 2;
		ret = schdrmsg_start_visorc(arg);
		break;
	case AMBACV_FIND_CVTASKMSG:
		ret = schdrmsg_find_entry(arg);
		break;
	case AMBACV_GET_SYSCONFIG:
		ret = schdrmsg_get_sysconfig(arg);
		break;
	case AMBACV_RECV_ORCMSG:
		ret = schdrmsg_recv_orcmsg(arg);
		break;
	case AMBACV_SEND_ARMMSG:
		ret = schdrmsg_send_armmsg(arg, 0);
		break;
	case AMBACV_GET_MEM_LAYOUT:
		ret = copy_to_user((void*)arg, &ambacv_global_mem, sizeof(ambacv_global_mem));
		break;
	case AMBACV_UCODE_LOADED:
		ret = visorc_finalize_loading(arg);
		break;
	case AMBACV_STOP_VISORC:
		if(ambacv_state == 1) {
		    cavalry_log_exit(pCavalrySupport);
		    ambacv_state = 0;
		    ret = schdrmsg_stop_visorc();
		}
		break;
	case AMBACV_SEND_VPMSG:
		ret = idsporc_send_vpmsg(arg);
		break;
	case AMBACV_CONFIG_IDSP:
		ret = idsporc_config(arg);
		break;
	case AMBACV_CACHE_CLEAN:
		ret = cache_ioctl_clean(arg);
		break;
	case AMBACV_CACHE_INVLD:
		ret = cache_ioctl_invld(arg);
		break;
	case AMBACV_SET_BIN_PATH:
		ret = copy_from_user(&arg_log, (void*)arg, sizeof(ambacv_global_log));
		memcpy(ambacv_global_log.binary_path, arg_log.binary_path, 256);
		pr_info("Set bin path %s\n", ambacv_global_log.binary_path);
		break;
	case AMBACV_GET_LOG_INFO:
		pr_info("Get bin path %s\n", ambacv_global_log.binary_path);
		ret = copy_to_user((void*)arg, &ambacv_global_log, sizeof(ambacv_global_log));
		break;
	case AMBACV_SEND_ASYNCMSG:
		ret = idsporc_send_asyncmsg(arg);
		break;
	case AMBACV_GET_VERSION:
		version = ((uint32_t)CVSCHEDULER_INTERFACE_VERSION << 8U);
		version = (version | CVSCHEDULER_KERNEL_VERSION);
		ret = copy_to_user((void*)arg, &version, sizeof(version));
		break;
	case AMBACV_GET_MAL_VERSION:
#if defined(ENABLE_AMBA_MAL)
		version = AMBA_MAL_VERSION;
#else
		version = 0U;
#endif
		ret = copy_to_user((void*)arg, &version, sizeof(version));
		break;
	case AMBACV_GET_MMB_INFO:
		ret = ambacv_get_mmb_info(arg);
		break;
	case AMBACV_GET_VALUE:
		ret = copy_from_user(&ambacv_value, (void*)arg, sizeof(ambacv_get_value_t));
		if (ambacv_value.index == AMBACV_ID_GET_SCHDR_TRACE)
		{
			ambacv_value.value = scheduler_system_get_trace_daddr();
		} /* if (arg == AMBACV_ID_GET_SCHDR_TRACE) */
		else if (ambacv_value.index == AMBACV_ID_GET_FLEXIDAG_TRACE)
		{
			ambacv_value.value = krn_flexidag_system_get_trace_daddr();
		} /* if (arg == AMBACV_ID_GET_FLEXIDAG_TRACE) */
		else if (ambacv_value.index == AMBACV_ID_GET_CAVALRY_TRACE)
		{
			ambacv_value.value = krn_cavalry_system_get_trace_daddr(pCavalrySupport);
		} /* if (arg == AMBACV_ID_GET_CAVALRY_TRACE) */
		else if (ambacv_value.index == AMBACV_ID_GET_AUTORUN_TRACE)
		{
			ambacv_value.value = scheduler_autorun_get_trace_daddr();
		} /* if (arg == AMBACV_ID_GET_AUTORUN_TRACE) */
		else if (ambacv_value.index == AMBACV_ID_GET_ORC_PERF_WI_BASE)
		{
			ambacv_cache_invalidate(&print_info->visorc_perf_debugbuf[0].last_wridx_daddr, sizeof(uint32_t));
			ambacv_value.value = (uint64_t)print_info->visorc_perf_debugbuf[0].last_wridx_daddr;
		} /* if (arg == AMBACV_ID_GET_ORC_PERF_WI_BASE) */
		else if (ambacv_value.index == AMBACV_ID_GET_AUDIO_CLOCK)
		{
			struct clk *gclk = NULL;

			gclk = clk_get_sys(NULL, "gclk_audio");
			if(gclk != NULL) {
				ambacv_value.value = clk_get_rate(gclk);
			} else {
				ret = -1;
			}
		} /* if (arg == AMBACV_ID_GET_AUDIO_CLOCK) */
		else if (ambacv_value.index == AMBACV_ID_GET_EVENT_TIME) {
			ambacv_value.value = schdrmsg_get_event_time();
		} /* if (arg == AMBACV_ID_GET_EVENT_TIME) */
		else if (ambacv_value.index == AMBACV_ID_GET_CLUSTER_ID)
		{
			ambacv_value.value = cluster_id;
		} /* if (ambacv_value.index == AMBACV_ID_GET_CLUSTER_ID) */
		else if (ambacv_value.index == AMBACV_ID_GET_ORC_CMD_WI)
		{
			ambacv_value.value = schdrmsg_get_orcarm_wi();
		} /* if (ambacv_value.index == AMBACV_ID_GET_ORC_CMD_WI) */
		else if (ambacv_value.index == AMBACV_ID_GET_SCHDR_TOKEN) /* Find and return core token */
		{
			uint32_t  token_id;

			token_id = ambacv_find_token();
			if ((token_id & 0x80000000U) != 0x0U)
			{
				ambacv_value.value = 0;
				ret = token_id;
			} /* if ((token_id & 0x80000000U) != 0x0U) */
			else /* if ((token_id & 0x80000000U) == 0x0U) */
			{
				ambacv_value.value = token_id;
				ret = ERRCODE_NONE;
			} /* if ((token_id & 0x80000000U) == 0x0U) */
		} /* if (ambacv_value.index == AMBACV_ID_GET_SCHDR_TOKEN) */
		else if ((ambacv_value.index >= AMBACV_ID_WAIT_SCHDR_TOKEN_BASE) && (ambacv_value.index < (AMBACV_ID_WAIT_SCHDR_TOKEN_BASE + MAX_AMBACV_TOKENS)))
		{
			mutex_lock(&ambacv_core_state.token_mutex);
			ambacv_value.value = 0;
			ret = ambacv_wait_token(ambacv_value.index); /* Wait on token */
		} /* if ((ambacv_value.index >= AMBACV_ID_WAIT_SCHDR_TOKEN_BASE) && (ambacv_value.index < (AMBACV_ID_WAIT_SCHDR_TOKEN_BASE + MAX_AMBACV_TOKENS))) */
		else /* if (arg != [0..3]) */
		{
			ambacv_value.value = 0;
		} /* if (arg != [0..3]) */
		ret = copy_to_user((void*)arg, &ambacv_value, sizeof(ambacv_get_value_t));
		break;
	case AMBACV_CORE_MUTEX:
		ret = copy_from_user(&value, (void*)arg, sizeof(value));
		if (value == 0)
		{
			mutex_lock(&ambacv_core_mutex);
		} /* if (arg == 0) */
		else /* if (arg != 0) */
		{
			mutex_unlock(&ambacv_core_mutex);
		} /* if (arg != 0) */
		break;
	case AMBACV_RESUME_VISORC:
		ret = schdrmsg_resume_visorc(arg);
		break;
	case AMBACV_GET_STATE:
		ret = ioctl_get_ambacv_state(arg);
		break;
	case AMBACV_GET_ATT:
		ret = ambacv_get_att(arg);
		break;
	case AMBACV_GET_TIME:
		time = visorc_get_curtime();
		ret = copy_to_user((void *)arg, &time, sizeof(time));
		break;
    case AMBACV_SEND_VINMSG:
        ret = vinorc_send_vinmsg(arg);
        break;
    case AMBACV_CONFIG_VIN:
        ret = vinorc_config(arg);
        break;

	/*-= Flexidag specific IOCTL =--------------------------------------------*/
	case AMBACV_FLEXIDAG_ENABLE:
	case AMBACV_FLEXIDAG_CREATE:
	case AMBACV_FLEXIDAG_CREATE_QUERY:
	case AMBACV_FLEXIDAG_SET_PARAMETER:
	case AMBACV_FLEXIDAG_ADD_TBAR:
	case AMBACV_FLEXIDAG_ADD_SFB:
	case AMBACV_FLEXIDAG_OPEN:
	case AMBACV_FLEXIDAG_OPEN_QUERY:
	case AMBACV_FLEXIDAG_SET_STATE_BUFFER:
	case AMBACV_FLEXIDAG_SET_TEMP_BUFFER:
	case AMBACV_FLEXIDAG_INIT:
	case AMBACV_FLEXIDAG_INIT_QUERY:
	case AMBACV_FLEXIDAG_PREP_RUN:
	case AMBACV_FLEXIDAG_SET_INPUT_BUFFER:
	case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER:
	case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER:
	case AMBACV_FLEXIDAG_RUN:
	case AMBACV_FLEXIDAG_RUN_QUERY:
	case AMBACV_FLEXIDAG_WAIT_RUN_FINISH:
	case AMBACV_FLEXIDAG_CLOSE:
	case AMBACV_FLEXIDAG_CLOSE_QUERY:
	case AMBACV_FLEXIDAG_INFO:
	case AMBACV_FLEXIDAG_HANDLE_MSG:
		ret = krn_flexidag_ioctl_process(f, cmd, arg);
		break;

	/*-= Cavalry specific IOCTL =---------------------------------------------*/
	case CAVALRY_QUERY_BUF:
		ret = cavalry_query_buf(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_START_VP:
		ret = cavalry_visorc_start(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_STOP_VP:
		ret = cavalry_visorc_stop(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_RUN_DAGS:
		ret = ioctl_cavalry_run_dags(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_START_LOG:
		ret = cavalry_log_start(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_STOP_LOG:
		ret = cavalry_log_stop(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_EARLY_QUIT:
		ret = cavalry_visorc_early_quit(pCavalrySupport, (void __user *)arg);
		break;
#if !defined(ENABLE_AMBA_MAL)
	case CAVALRY_ALLOC_MEM:
		if (enable_cma != 0)
		{
			ret = cavalry_cma_alloc(pCavalrySupport, f,(void __user *)arg);
		} /* if (enable_cma != 0) */
		else /* if (enable_cma == 0) */
		{
			ret = -ENODEV;
		} /* if (enable_cma == 0) */
		break;
	case CAVALRY_FREE_MEM:
		if (enable_cma != 0)
		{
			ret = cavalry_cma_free(pCavalrySupport, (void __user *)arg);
		} /* if (enable_cma != 0) */
		else /* if (enable_cma == 0) */
		{
			ret = -ENODEV;
		} /* if (enable_cma == 0) */
		break;
	case CAVALRY_SYNC_CACHE_MEM:
		if (enable_cma != 0)
		{
			ret = cavalry_cma_sync_cache(pCavalrySupport, (void __user *)arg);
		} /* if (enable_cma != 0) */
		else /* if (enable_cma == 0) */
		{
			ret = -ENODEV;
		} /* if (enable_cma == 0) */
		break;
	case CAVALRY_GET_USAGE_MEM:
		if (enable_cma != 0)
		{
			ret = cavalry_cma_get_usage(pCavalrySupport, (void __user *)arg);
		} /* if (enable_cma != 0) */
		else /* if (enable_cma == 0) */
		{
			ret = -ENODEV;
		} /* if (enable_cma == 0) */
		break;
#endif
	case CAVALRY_RUN_HOTLINK_SLOT:
		ret = ioctl_cavalry_run_hotlink_slot(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_SET_HOTLINK_SLOT_CFG:
		ret = cavalry_set_hotlink_slot_cfg(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_GET_HOTLINK_SLOT_CFG:
		ret = cavalry_get_hotlink_slot_cfg(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_FEX_QUERY:
		ret = ioctl_cavalry_fex_query(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_FEX_RUN:
		ret = ioctl_cavalry_fex_run(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_FMA_QUERY:
		ret = ioctl_cavalry_fma_query(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_FMA_RUN:
		ret = ioctl_cavalry_fma_run(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_GET_STATS:
		ret = cavalry_stats_get(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_GET_AUDIO_CLK:
		ret = cavalry_get_audio_clk(pCavalrySupport, (void __user *)arg);
		break;

	/*-= Superdag/Cavalry specific IOCTL =------------------------------------*/
	case CAVALRY_ENABLE:
		ret = ioctl_cavalry_enable(pCavalrySupport, (uint32_t)arg);
		break;
	case CAVALRY_ASSOCIATE_BUF:
		ret = ioctl_cavalry_associate_buf(pCavalrySupport, (uint32_t)arg);
		break;
	case CAVALRY_SET_MEMORY_BLOCK:
		ret = ioctl_cavalry_set_memory_block(pCavalrySupport, (void __user *)arg);
		break;
	case CAVALRY_HANDLE_VISORC_REPLY:
		ret = 0; // Moved the handler for this into the kernel
		break;
	default:
		pr_err("[AMBACV] unknown IOCTL %d\n", cmd);
		ret = -EINVAL;
	}

	return ret;
}

static ssize_t cavalry_read(struct file *filp,
	char __user *buffer, size_t count, loff_t *offp)
{
	struct file filp1;
	filp1.private_data = pCavalrySupport; /* Temporary repatch */
	return cavalry_log_read(&filp1, buffer, count, offp);
}

static int ambacv_release(struct inode *inode, struct file *file)
{
	krn_flexidag_close_by_filp(file);
	return 0;
}

static const struct file_operations ambacv_fops = {
	.owner = THIS_MODULE,
	.mmap = ambacv_mmap,
	.unlocked_ioctl = ambacv_ioctl,
	.read = cavalry_read,
	.release = ambacv_release,
};

#if defined(ENABLE_AMBA_MAL)
static int __init ambacv_of_get_mal_info(void)
{
    int rval = 0;

    if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SCHDR, &krn_schdr_core_info) != 0U) {
        pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SCHDR fail \n");
        rval = -1;
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SYS, &krn_schdr_cma_info) != 0U) {
            pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SYS fail \n");
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_RTOS, &krn_schdr_rtos_info) != 0U) {
            pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_RTOS fail \n");
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_DSP_DATA, &krn_dsp_data_info) != 0U) {
            pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_DSP_DATA fail \n");
            rval = -1;
        }
    }
    return rval;
}

static void __init ambacv_of_init(struct device_node *np)
{
    uint64_t cv_schdr_size = 0UL;
    void *vaddr;
	__be32 *reg;
	int len, rval;
	struct device_node *cpu_node;;
	const char *name = of_node_full_name(np);
	uint64_t value1 = 0UL,value2 = 0UL,value3 = 0UL;
	uint32_t cpu_id;
    uint32_t ret;

	/* Get Cluster ID */
	cpu_node = of_get_next_cpu_node(NULL);
	reg = (__be32 *) of_get_property(cpu_node, "reg", &len);
	if (reg) {
		cpu_id = be32_to_cpu(reg[0]);
		cluster_id = ((cpu_id >> 8U) & 0xFFU);
		scheduler_id = cluster_id;
		pr_info("[AMBACV] CPU ID        :   0x%x \n", cpu_id);
	} else {
		scheduler_id = 0U;
	}
	pr_info("[AMBACV] SCHEDULER ID:  %d\n", scheduler_id);

	pCavalrySupport->dev              = ambacv_device;
	pCavalrySupport->dev->of_node     = np;
	pCavalrySupport->dev->bus         = &platform_bus_type;
	pCavalrySupport->run_dags         = NULL; /* FOR NOW */

	/* ATT info */
	reg = (__be32 *) of_get_property(np, "cv_att_pa", &len);
	if (reg && (len == sizeof(u32))) {
		rval = of_property_read_u32(np, "cv_att_pa", (uint32_t *)&value1);
		if (rval == 0) {
			rval = of_property_read_u32(np, "cv_att_size", (uint32_t *)&value2);
			if (rval == 0) {
				rval = of_property_read_u32(np, "cv_att_ca", (uint32_t *)&value3);
			}
		}
	} else if (reg && (len == 2*sizeof(u32))) {
		rval = of_property_read_u64(np, "cv_att_pa", &value1);
		if (rval == 0) {
			rval = of_property_read_u64(np, "cv_att_size", &value2);
			if (rval == 0) {
				rval = of_property_read_u64(np, "cv_att_ca", &value3);
			}
		}
	} else {
		rval = -1;
	}
    if (rval == 0) {
		cv_pa_start = value1;
		cv_pa_end = value1 + value2;
		cv_ca_start = value3;
		cv_ca_end = value3 + value2;
	}
	pr_info("[AMBACV] ATT PA RANGE  :   [0x%llx--0x%llx]\n",cv_pa_start,cv_pa_end);
	pr_info("[AMBACV] ATT CA RANGE  :   [0x%llx--0x%llx]\n",cv_ca_start,cv_ca_end);

    reg = (__be32 *) of_get_property(np, "cv_schdr_size", &len);
	if (reg && (len == sizeof(u32))) {
		rval = of_property_read_u32(np, "cv_schdr_size", (uint32_t *)&cv_schdr_size);
	} else if (reg && (len == 2 * sizeof(u32))) {
		rval = of_property_read_u64(np, "cv_schdr_size", &cv_schdr_size);
	} else {
		rval = -1;
	}
	if (rval == 0) {
		if (cma_superdag_size >= 0)
		{
			pr_info("[AMBACV] CMA CONFIG  :  cv_schdr_size region defined (size=0x%llx bytes), user requests override of cma_superdag_size (0x%x bytes); honoring user request\n",
			       cv_schdr_size, cma_superdag_size * 1024 * 1024);
			cv_schdr_size = cma_superdag_size * 1024 * 1024;
		} /* if (cma_superdag_size >= 0) */
		else /* if (cma_superdag_size < 0) */
		{
			cma_superdag_size = (cv_schdr_size + 1048575) >> 20; /* Convert to MB */
		} /* if (cma_superdag_size < 0) */
	} /* if (rval == 0) : of_property_read_u32("cv_schdr_size") */
	else /* if (rval != 0) : of_property_read_u32("cv_schdr_size") */
	{
		if (cma_superdag_size < 0)
		{
			cma_superdag_size = DEFAULT_CMA_SUPERDAG_SIZE;
		} /* if (cma_superdag_size < 0) */
		cv_schdr_size = cma_superdag_size * 1024 * 1024;
	} /* if (rval != 0) : of_property_read_u32("cv_schdr_size") */

    if (rval == 0) {
        rval = ambacv_of_get_mal_info();
    }

	if (rval == 0) {
        /* Get CV Core mem range */
        enable_cma  = 1;
        if(krn_schdr_core_info.Type == AMBA_MAL_TYPE_INVALID) {
            ret = AmbaMAL_Alloc(AMBA_MAL_ID_CV_SYS, cv_schdr_size, 0x1000, &SchdrMem);
            if(ret != 0U) {
                pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Alloc fail ret=0x%x", ret);
            } else {
                if(enable_arm_cache == 1U) {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, SchdrMem.PhysAddr, SchdrMem.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                } else {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, SchdrMem.PhysAddr, SchdrMem.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                }

                if(vaddr == NULL) {
                    pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Map fail ret=0x%x", ret);
                } else {
                    schdr_mem_base = vaddr;
                    pCavalrySupport->cma_private.phys = ambacv_p2c(SchdrMem.PhysAddr);
                    pCavalrySupport->cma_private.virt = vaddr;
                    pCavalrySupport->cma_private.size = cv_schdr_size;
                    pCavalrySupport->cavalry_memblock_base = ambacv_p2c(SchdrMem.PhysAddr);
                    pCavalrySupport->cavalry_memblock_size = krn_schdr_cma_info.Size;
                    /* Specialized repartition of the LOG buffer to the end of the cma_private block */
                    pCavalrySupport->cavalry_uncached_logblock.phys   = pCavalrySupport->cma_private.phys + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
                    pCavalrySupport->cavalry_uncached_logblock.size   = CAVALRY_MEM_LOG_SIZE;
                    pCavalrySupport->cavalry_uncached_logblock.virt   = pCavalrySupport->cma_private.virt + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;

                    ambacv_global_mem.all.base   = pCavalrySupport->cma_private.phys;
                    ambacv_global_mem.all.size   = pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
                    ambacv_global_mem.arm.base   = 0;
                    ambacv_global_mem.arm.size   = 0;

                    ambacv_sysinit_vaddr  = pCavalrySupport->cma_private.virt + 0x1000;
                    ambacv_r52_trace      = (visorc_safety_heartbeat_t *)(pCavalrySupport->cma_private.virt + 0x20000);
                    ambacv_cvmem_vaddr    = pCavalrySupport->cma_private.virt + 0x200000;
                    ambacv_mmap_add_master(pCavalrySupport->cma_private.virt, ambacv_c2p(pCavalrySupport->cma_private.phys), pCavalrySupport->cma_private.size, enable_arm_cache);
                    pr_info("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
                           enable_arm_cache ? "enabled" : "disabled",
                           enable_orc_cache ? "enabled" : "disabled");
                    pr_info("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
                           krn_schdr_cma_info.PhysAddr, (krn_schdr_cma_info.PhysAddr + krn_schdr_cma_info.Size));
                    pr_info("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                           SchdrMem.PhysAddr, (SchdrMem.PhysAddr + cv_schdr_size));
                }
            }
        } else {
            if(enable_arm_cache == 1U) {
                ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
            } else {
                ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
            }

            if(vaddr == NULL) {
                pr_err("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Map fail ret=0x%x", ret);
            } else {
                schdr_mem_base = vaddr;
                pCavalrySupport->cma_private.phys = ambacv_p2c(krn_schdr_core_info.PhysAddr);
                pCavalrySupport->cma_private.virt = vaddr;
                pCavalrySupport->cma_private.size = krn_schdr_core_info.Size;
                pCavalrySupport->cavalry_memblock_base = ambacv_p2c(krn_schdr_core_info.PhysAddr);
			    pCavalrySupport->cavalry_memblock_size = krn_schdr_cma_info.Size;
                /* Specialized repartition of the LOG buffer to the end of the cma_private block */
                pCavalrySupport->cavalry_uncached_logblock.phys   = pCavalrySupport->cma_private.phys + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
                pCavalrySupport->cavalry_uncached_logblock.size   = CAVALRY_MEM_LOG_SIZE;
                pCavalrySupport->cavalry_uncached_logblock.virt   = pCavalrySupport->cma_private.virt + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;

                ambacv_global_mem.all.base   = pCavalrySupport->cma_private.phys;
                ambacv_global_mem.all.size   = pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
                ambacv_global_mem.arm.base   = 0;
                ambacv_global_mem.arm.size   = 0;

                ambacv_sysinit_vaddr  = pCavalrySupport->cma_private.virt + 0x1000;
                ambacv_r52_trace      = (visorc_safety_heartbeat_t *)(pCavalrySupport->cma_private.virt + 0x20000);
                ambacv_cvmem_vaddr    = pCavalrySupport->cma_private.virt + 0x200000;
                ambacv_mmap_add_master(pCavalrySupport->cma_private.virt, ambacv_c2p(pCavalrySupport->cma_private.phys), pCavalrySupport->cma_private.size, enable_arm_cache);
                pr_info("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
                       enable_arm_cache ? "enabled" : "disabled",
                       enable_orc_cache ? "enabled" : "disabled");
                pr_info("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
                       krn_schdr_cma_info.PhysAddr, (krn_schdr_cma_info.PhysAddr + krn_schdr_cma_info.Size));
                pr_info("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                       krn_schdr_core_info.PhysAddr, (krn_schdr_core_info.PhysAddr + krn_schdr_core_info.Size));
            }
        }
    }

    if(rval == 0) {
        /* Get CV RTOS mem range */
        vaddr = NULL;
        if((krn_schdr_rtos_info.Type != AMBA_MAL_TYPE_INVALID) && (krn_schdr_rtos_info.Size != 0UL)) {
            if(enable_rtos_cache == 1U) {
                ret = AmbaMAL_Map(AMBA_MAL_ID_CV_RTOS, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
            } else {
                ret = AmbaMAL_Map(AMBA_MAL_ID_CV_RTOS, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
            }

            if(vaddr == NULL) {
                pr_err("[ERROR] ambacv_of_init() : RTOS: AmbaMAL_Map fail ret=0x%x", ret);
            } else {
                rtos_io_base = vaddr;
                ambacv_global_mem.rtos_region.base = ambacv_p2c(krn_schdr_rtos_info.PhysAddr);
                ambacv_global_mem.rtos_region.size = krn_schdr_rtos_info.Size;
                ambacv_mmap_add_rtos(vaddr, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, enable_rtos_cache);
                pr_info("[AMBACV] RTOS RANGE    :   [0x%llx--0x%llx] @ (%p) (cache is %s)\n",
                    krn_schdr_rtos_info.PhysAddr, (krn_schdr_rtos_info.PhysAddr + krn_schdr_rtos_info.Size), vaddr, (enable_rtos_cache != 0) ? "enabled" : "disabled");
            }
        }
    }

    if(rval == 0) {
        /* Get DSP RTOS mem range */
        vaddr = NULL;
        if((krn_dsp_data_info.Type != AMBA_MAL_TYPE_INVALID) && (krn_dsp_data_info.Size != 0UL)) {
            ret = AmbaMAL_Map(AMBA_MAL_ID_DSP_DATA, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
            if(vaddr == NULL) {
                pr_err("[ERROR] ambacv_of_init() : DSP: AmbaMAL_Map fail ret=0x%x", ret);
            } else {
                dsp_data_base = vaddr;
                ambacv_mmap_add_dsp_data(vaddr, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, 1U);
                pr_info("[AMBACV] DSP DATA RANGE    :   [0x%llx--0x%llx] @ (%p) \n",
                    krn_dsp_data_info.PhysAddr, (krn_dsp_data_info.PhysAddr + krn_dsp_data_info.Size), vaddr);
            }
        }
    }

#ifdef ASIL_COMPLIANCE
	pr_info("[AMBACV] ASIL_COMPLIANCE MODE ENABLED\n");
#endif /* ?ASIL_COMPLIANCE */

	ambacv_irq = irq_of_parse_and_map(np, 0);
	rval = request_irq(ambacv_irq, ambacv_isr,
			   IRQF_TRIGGER_RISING,
			   "cv_scheduler", NULL);
	if (rval != 0) {
		pr_err("ambacv failed to request IRQ! (rval=%d)\n", rval);
		return;
	}
}

#else
static void __init ambacv_of_init(struct device_node *np)
{
	uint64_t base = 0UL, size = 0UL;
	char *vadd;
	__be32 *reg;
	__be64 *reg64;
	int len, rval;
	pgprot_t prot;
	struct device_node *node, *cpu_node;;
	const char *name = of_node_full_name(np);
	uint64_t superdag_size = 0UL;
	uint64_t value1 = 0UL,value2 = 0UL,value3 = 0UL;
	uint32_t cpu_id;

	/* Get Cluster ID */
	cpu_node = of_get_next_cpu_node(NULL);
	reg = (__be32 *) of_get_property(cpu_node, "reg", &len);
	if (reg) {
		cpu_id = be32_to_cpu(reg[0]);
		cluster_id = ((cpu_id >> 8U) & 0xFFU);
		scheduler_id = cluster_id;
		pr_info("[AMBACV] CPU ID        :   0x%x \n", cpu_id);
	} else {
		scheduler_id = 0U;
	}
	pr_info("[AMBACV] SCHEDULER ID:  %d\n", scheduler_id);

	pCavalrySupport->dev              = ambacv_device;
	pCavalrySupport->dev->of_node     = np;
	pCavalrySupport->dev->bus         = &platform_bus_type;
	pCavalrySupport->run_dags         = NULL; /* FOR NOW */

	mutex_init(&pCavalrySupport->cavalry_cma_mutex);
	mutex_init(&ambacv_core_mutex);
	INIT_LIST_HEAD(&pCavalrySupport->mmb_list);
	INIT_LIST_HEAD(&pCavalrySupport->free_mmb_list);

	/* Get CV mem range */
	node = of_parse_phandle(np, "memory-region", 0);
	if (node != NULL)
	{
		enable_cma  = 1;

		/* ATT info */
		reg = (__be32 *) of_get_property(np, "cv_att_pa", &len);
		if (reg && (len == sizeof(u32)))
		{
			rval = of_property_read_u32(np, "cv_att_pa", (uint32_t *)&value1);
			if (rval == 0) {
				rval = of_property_read_u32(np, "cv_att_size", (uint32_t *)&value2);
				if (rval == 0) {
					rval = of_property_read_u32(np, "cv_att_ca", (uint32_t *)&value3);
				}
			}
		}
		else if (reg && (len == 2*sizeof(u32)))
		{
			rval = of_property_read_u64(np, "cv_att_pa", &value1);
			if (rval == 0) {
				rval = of_property_read_u64(np, "cv_att_size", &value2);
				if (rval == 0) {
					rval = of_property_read_u64(np, "cv_att_ca", &value3);
				}
			}
		}
		else
		{
			rval = -1;
		}

		if (rval == 0) {
			cv_pa_start = value1;
			cv_pa_end = value1 + value2;
			cv_ca_start = value3;
			cv_ca_end = value3 + value2;
		}

		pr_info("[AMBACV] ATT PA RANGE  :   [0x%llx--0x%llx]\n",cv_pa_start,cv_pa_end);
		pr_info("[AMBACV] ATT CA RANGE  :   [0x%llx--0x%llx]\n",cv_ca_start,cv_ca_end);

		reg = (__be32 *) of_get_property(node, "reg", &len);
		if (reg && (len == 2 * sizeof(u32)))
		{
			base = be32_to_cpu(reg[0]);
			size = be32_to_cpu(reg[1]);
		}
		else if (reg && (len == 4 * sizeof(u32)))
		{
			reg64 = (__be64 *)reg;
			base = be64_to_cpu(reg64[0]);
			size = be64_to_cpu(reg64[1]);
		}
		else {
			pr_err("err: memory-region is in the wrong format\n");
			return;
		}

		pr_info("[AMBACV] CACHE CONFIG:  ARM(%s), ORC(%s)\n",
		       enable_arm_cache ? "enabled" : "disabled",
		       enable_orc_cache ? "enabled" : "disabled");
		pr_info("[AMBACV] CMA RANGE   :  [0x%llx--0x%llx]\n",
		       base, base + size);

		reg = NULL;
		node = of_parse_phandle(np, "memory-region2", 0);
		if (node != NULL)
		{
			reg = (__be32 *) of_get_property(node, "reg", &len);
		}
		if (reg != NULL)
		{
			pCavalrySupport->cavalry_memblock_size = size;

			if (reg && (len == 2 * sizeof(u32)))
			{
				base = be32_to_cpu(reg[0]);
				size = be32_to_cpu(reg[1]);
			}
			else if (reg && (len == 4 * sizeof(u32)))
			{
				reg64 = (__be64 *)reg;
				base = be64_to_cpu(reg64[0]);
				size = be64_to_cpu(reg64[1]);
			}
			else {
				pr_err("err: memory-region2 is in the wrong format\n");
				return;
			}
			rval = cavalry_cma_init(pCavalrySupport);
			if (rval < 0)
			{
				pr_err("cavalry_cma_init() : Returns error %d\n", rval);
				return;
			}
			pCavalrySupport->cma_private.size = size;
			pCavalrySupport->cma_private.phys = ambacv_p2c(base);
			prot = (enable_arm_cache)?__pgprot(PROT_NORMAL):__pgprot(PROT_NORMAL_NC);
			schdr_mem_base = __ioremap(base, size, prot);
			pCavalrySupport->cma_private.virt = schdr_mem_base;

			pr_info("[AMBACV] SCHDR PRIVATE:   [0x%llx--0x%llx] @ (%p)\n",
			  (ambacv_c2p(pCavalrySupport->cma_private.phys)), 
			  (ambacv_c2p(pCavalrySupport->cma_private.phys) + pCavalrySupport->cma_private.size),
			pCavalrySupport->cma_private.virt);

			pCavalrySupport->cavalry_memblock_base = ambacv_p2c(base);
			pCavalrySupport->cavalry_memblock_size += size;
		} 
		else 
		{
			reg = (__be32 *) of_get_property(np, "cv_schdr_size", &len);
			if (reg && (len == sizeof(u32)))
			{
				rval = of_property_read_u32(np, "cv_schdr_size", (uint32_t *)&superdag_size);
			}
			else if (reg && (len == 2 * sizeof(u32)))
			{
				rval = of_property_read_u64(np, "cv_schdr_size", &superdag_size);
			}
			else 
			{
				rval = -1;
			}
			if (rval == 0)
			{
				if (cma_superdag_size >= 0)
				{
					pr_info("[AMBACV] CMA CONFIG  :  cv_schdr_size region defined (size=0x%llx bytes), user requests override of cma_superdag_size (0x%x bytes); honoring user request\n",
					       superdag_size, cma_superdag_size * 1024 * 1024);
					pCavalrySupport->cma_private.size = cma_superdag_size * 1024 * 1024;
				} /* if (cma_superdag_size >= 0) */
				else /* if (cma_superdag_size < 0) */
				{
					pCavalrySupport->cma_private.size = superdag_size;
					cma_superdag_size = (superdag_size + 1048575) >> 20; /* Convert to MB */
				} /* if (cma_superdag_size < 0) */
			} /* if (rval == 0) : of_property_read_u32("cv_schdr_size") */
			else /* if (rval != 0) : of_property_read_u32("cv_schdr_size") */
			{
				if (cma_superdag_size < 0)
				{
					cma_superdag_size = DEFAULT_CMA_SUPERDAG_SIZE;
				} /* if (cma_superdag_size < 0) */
				pCavalrySupport->cma_private.size = cma_superdag_size * 1024 * 1024;
			} /* if (rval != 0) : of_property_read_u32("cv_schdr_size") */

			rval = cavalry_cma_init(pCavalrySupport);
			if (rval < 0)
			{
				pr_err("cavalry_cma_init() : Returns error %d\n", rval);
				return;
			}

			rval = cavalry_cma_alloc_schdr(pCavalrySupport);
			if (rval < 0)
			{
				pr_err("cavalry_cma_alloc_schdr() : Returns error %d\n", rval);
				return;
			}
			pr_info("[AMBACV] CMA PRIVATE:   [0x%llx--0x%llx] @ (%p)\n",
			  (ambacv_c2p(pCavalrySupport->cma_private.phys)), 
			  (ambacv_c2p(pCavalrySupport->cma_private.phys) + pCavalrySupport->cma_private.size),
			   pCavalrySupport->cma_private.virt);
            
			pCavalrySupport->cavalry_memblock_base = ambacv_p2c(base);
			pCavalrySupport->cavalry_memblock_size = size;
		}

		/* Specialized repartition of the LOG buffer to the end of the cma_private block */
		pCavalrySupport->cavalry_uncached_logblock.phys   = pCavalrySupport->cma_private.phys + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
		pCavalrySupport->cavalry_uncached_logblock.size   = CAVALRY_MEM_LOG_SIZE;
		pCavalrySupport->cavalry_uncached_logblock.virt   = pCavalrySupport->cma_private.virt + pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;

		ambacv_global_mem.all.base   = pCavalrySupport->cma_private.phys;
		ambacv_global_mem.all.size   = pCavalrySupport->cma_private.size - CAVALRY_MEM_LOG_SIZE;
		ambacv_global_mem.arm.base   = 0;
		ambacv_global_mem.arm.size   = 0;
		ambacv_sysinit_vaddr  = pCavalrySupport->cma_private.virt + 0x1000;
		ambacv_r52_trace      = (visorc_safety_heartbeat_t *)(pCavalrySupport->cma_private.virt + 0x20000);
		ambacv_cvmem_vaddr    = pCavalrySupport->cma_private.virt + 0x200000;
		ambacv_mmap_add_master(pCavalrySupport->cma_private.virt, ambacv_c2p(pCavalrySupport->cma_private.phys), pCavalrySupport->cma_private.size, enable_arm_cache);

	} /* if (node != NULL) : if "memory-region" exists in the device tree : use CMA */
	else /* if (node == NULL) : if "memory-region" doesn't exist in the device tree : don't use CMA */
	{
		enable_cma  = 0;
		node = of_find_node_by_name(NULL, "cv_shm");
		if(!node)
		{
			pr_err("err: cv_shm and memory-region aren't defined\n");
			return;
		}

		reg = (__be32 *) of_get_property(node, "reg", &len);
		if (reg && (len == 2 * sizeof(u32)))
		{
			base = be32_to_cpu(reg[0]);
			size = be32_to_cpu(reg[1]);
		}
		else if (reg && (len == 4 * sizeof(u32)))
		{
			reg64 = (__be64 *)reg;
			base = be64_to_cpu(reg64[0]);
			size = be64_to_cpu(reg64[1]);
		}
		else
		{
			pr_err("err: cv_shm wrong format\n");
			return;
		}

		pr_info("[AMBACV] CACHE CONFIG:  ARM(%s), ORC(%s)\n",
		       enable_arm_cache ? "enabled" : "disabled",
		       enable_orc_cache ? "enabled" : "disabled");
		pr_info("[AMBACV] CVMEM RANGE:   [0x%llx--0x%llx]\n",
		       base, base + size);

		pCavalrySupport->cma_private.size = CAVALRY_MEM_USER_OFFSET;
		pCavalrySupport->cma_private.virt = NULL;

		ambacv_global_mem.all.base  = ambacv_p2c(base);
		ambacv_global_mem.all.size  = size;
		ambacv_global_mem.arm.base  = 0;
		ambacv_global_mem.arm.size  = 0;

		prot = (enable_arm_cache)?__pgprot(PROT_NORMAL):__pgprot(PROT_NORMAL_NC);
		vadd = __ioremap(base, size, prot);
		BUG_ON(vadd == NULL);
		ambacv_sysinit_vaddr  = vadd + 0x1000;
		ambacv_r52_trace      = (visorc_safety_heartbeat_t *)(vadd + 0x20000);
		ambacv_cvmem_vaddr    = vadd + 0x200000;
		ambacv_mmap_add_master(vadd, base, size, enable_arm_cache);

		pCavalrySupport->cavalry_memblock_base = 0;
		pCavalrySupport->cavalry_memblock_size = 0;

	} /* if (node == NULL) : if "memory-region" doesn't exist in the device tree : don't use CMA */

	{
		base = 0;
		size = 0;
		vadd = NULL;
		/* Check for, and add the "RTOS" block if available */
		node = of_parse_phandle(np, "memory-region1", 0);
		if (node != NULL)
		{
			reg = (__be32 *) of_get_property(node, "reg", &len);
			if (reg && (len == 2 * sizeof(u32)))
			{
				base = be32_to_cpu(reg[0]);
				size = be32_to_cpu(reg[1]);
			}
			else if(reg && (len == 4 * sizeof(u32)))
			{
				reg64 = (__be64 *)reg;
				base = be64_to_cpu(reg64[0]);
				size = be64_to_cpu(reg64[1]);
			}
			else
			{
				base = 0UL;
				size = 0UL;
			}
		} /* if (node != NULL) : of_parse_phandle(np, "memory-region1", 0) */

		if(size != 0UL) {
			prot = (enable_rtos_cache != 0) ? __pgprot(PROT_NORMAL):__pgprot(PROT_NORMAL_NC);
			vadd = __ioremap(base, size, prot);
			rtos_io_base = vadd;
			BUG_ON(vadd == NULL);
			pr_info("[AMBACV] RTOS RANGE :   [0x%llx--0x%llx] @ (%p) (cache is %s)\n",
				base, base + size - 1, vadd, (enable_rtos_cache != 0) ? "enabled" : "disabled");
			ambacv_global_mem.rtos_region.base = ambacv_p2c(base);
			ambacv_global_mem.rtos_region.size = size;
			ambacv_mmap_add_rtos(vadd, base, size, enable_rtos_cache);
		} else {
			ambacv_global_mem.rtos_region.base = 0UL;
			ambacv_global_mem.rtos_region.size = 0UL;
			ambacv_mmap_add_rtos(NULL, 0UL, 0UL, enable_rtos_cache);
		}
	}
#ifdef ASIL_COMPLIANCE
	pr_info("[AMBACV] ASIL_COMPLIANCE MODE ENABLED\n");
#endif /* ?ASIL_COMPLIANCE */

	ambacv_irq = irq_of_parse_and_map(np, 0);
	rval = request_irq(ambacv_irq, ambacv_isr,
			   IRQF_TRIGGER_RISING,
			   "cv_scheduler", NULL);
	if (rval != 0) {
		pr_err("ambacv failed to request IRQ! (rval=%d)\n", rval);
		return;
	}
}
#endif

static const struct of_device_id __ambacv_of_table = {
	.compatible = "ambarella,sub-scheduler",
	.data = ambacv_of_init,
};

static int  __init ambacv_init(void)
{
	struct device_node *np;
	const struct of_device_id *match;
	of_init_fn_1 init_func;
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "ambarella,sub-scheduler");
	if (!node) {
		of_node_put(node);
		pr_err("[AMBACV] no device node \n");
		return -ENODEV;
	} else if (!of_device_is_available(node)) {
		of_node_put(node);
		pr_err("[AMBACV] device node is disabled \n");
		return -ENODEV;
	}

	ambacv_state = 0;
	ambacv_major = register_chrdev(0, DEVICE_NAME, &ambacv_fops);
	if (ambacv_major < 0) {
		pr_err("ambacv: failed to register device %d.\n", ambacv_major);
		return ambacv_major;
	}

	ambacv_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(ambacv_class)) {
		unregister_chrdev(ambacv_major, DEVICE_NAME);
		pr_err("ambacv: failed to create class.\n");
		return PTR_ERR(ambacv_class);
	}

	pr_info("[AMBACV] GIT VERSION:   %s \n", BUILD_SHA1);
	pr_info("[AMBACV] BUILT BY:      %s \n", BUILD_USER);
	pr_info("[AMBACV] BUILT TIME:    %s \n", BUILD_TIME);
	pr_info("[AMBACV] API VERSION:   %d.%d\n", CVSCHEDULER_INTERFACE_VERSION, CVSCHEDULER_KERNEL_VERSION);

	ambacv_device = device_create(ambacv_class, NULL, MKDEV(ambacv_major, 0),
				NULL, DEVICE_NAME);
	if (IS_ERR(ambacv_device)) {
		class_destroy(ambacv_class);
		unregister_chrdev(ambacv_major, DEVICE_NAME);
		pr_err("ambacv: falied to create device.\n");
		return PTR_ERR(ambacv_device);
	}

	pCavalrySupport = &cavalry_support;

	for_each_matching_node_and_match(np, &__ambacv_of_table, &match) {
		if (!of_device_is_available(np)) {
			continue;
		}

		init_func = match->data;
		init_func(np);
	}

	/* Initialize ambacv_token system */
	ambacv_token_init();

	/* Initialize scheduler */
	schdrmsg_init();

	/* Initialize cavalry system */
	cavalry_system_init(pCavalrySupport);

	/* Initialize scheduler system */
	krn_scheduler_system_init();

	/* Initialize flexidag system */
	krn_flexidag_system_init();

	/* Initialize rest of system */
	visorc_init();
	idsporc_init();
	vinorc_init();

	return 0;
}

static void __exit ambacv_exit(void)
{
	schdrmsg_shutdown();
	cavalry_system_shutdown(pCavalrySupport);
	krn_scheduler_system_shutdown();
	krn_flexidag_system_shutdown();
	visorc_shutdown();
	idsporc_shutdown();
	vinorc_shutdown();
#if defined(ENABLE_AMBA_MAL)
    if (dsp_data_base != NULL) {
        AmbaMAL_Unmap(AMBA_MAL_ID_DSP_DATA, dsp_data_base, krn_dsp_data_info.Size);
    }

    if (rtos_io_base != NULL) {
        AmbaMAL_Unmap(AMBA_MAL_ID_CV_RTOS, rtos_io_base, krn_schdr_rtos_info.Size);
    }

    if (schdr_mem_base != NULL) {
        if(krn_schdr_core_info.Type == AMBA_MAL_TYPE_INVALID) {
            AmbaMAL_Unmap(AMBA_MAL_ID_CV_SYS, schdr_mem_base, SchdrMem.RealSize);
            AmbaMAL_Free(AMBA_MAL_ID_CV_SYS, &SchdrMem);
        } else {
            AmbaMAL_Unmap(AMBA_MAL_ID_CV_SCHDR, schdr_mem_base, krn_schdr_core_info.Size);
        }
    }
#else
	if (rtos_io_base != NULL)
	{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
		__iounmap(rtos_io_base );
#else
		iounmap(rtos_io_base);
#endif
	} /* if (rtos_io_base != NULL) */

	if (schdr_mem_base != NULL)
	{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
		__iounmap(schdr_mem_base );
#else
		iounmap(schdr_mem_base);
#endif
	} /* if (schdr_mem_base != NULL) */

	if (enable_cma == 0)
	{
		if (ambacv_sysinit_vaddr != NULL)
		{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
			__iounmap(ambacv_sysinit_vaddr);
#else
			iounmap(ambacv_sysinit_vaddr);
#endif
			ambacv_sysinit_vaddr = NULL;
		} /* if ((ambacv_sysinit_vaddr != NULL) */
		if (ambacv_cvmem_vaddr != NULL)
		{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
			__iounmap(ambacv_cvmem_vaddr);
#else
			iounmap(ambacv_cvmem_vaddr);
#endif
			ambacv_cvmem_vaddr  = NULL;
		} /* if (ambacv_cvmem_vaddr != NULL) */
	} /* if (enable_cma == 0) */
	else /* if (enable_cma != 0) */
	{
		cavalry_cma_exit(pCavalrySupport);
	} /* if (enable_cma != 0) */
#endif

	if (ambacv_irq > 0)
	{
		free_irq(ambacv_irq, NULL);
		irq_dispose_mapping(ambacv_irq);
		ambacv_irq = 0;
	} /* if (ambacv_irq > 0) */

	ambacv_device->bus = NULL; /* Remove bus link */
	device_destroy(ambacv_class, MKDEV(ambacv_major, 0));
	class_destroy(ambacv_class);
	unregister_chrdev(ambacv_major, DEVICE_NAME);
	pr_info("ambacv: module exit\n");
	return;
}

module_init(ambacv_init);
module_exit(ambacv_exit);

