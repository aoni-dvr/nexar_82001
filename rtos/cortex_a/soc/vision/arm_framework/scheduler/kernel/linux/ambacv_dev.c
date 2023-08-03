// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
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
#include <linux/version.h>
#include <linux/uaccess.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/chip.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "cavalry_ioctl.h"
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "schdr_kernel.h"
#include "cache_kernel.h"
#include "flexidag_kernel.h"
#include "msg_kernel.h"
#include "schdr_api.h"
#include "cavalry_cma.h"    /* For cavalry/CMA interface */

#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

MODULE_AUTHOR("Joey Li");
MODULE_LICENSE("GPL");

#define DEVICE_NAME     "ambacv"

#define DEFAULT_CMA_SUPERDAG_SIZE     32

uint32_t                ambacv_irq;
static int              cma_superdag_size  = -1;
static unsigned int     ambacv_major;
static struct class*    ambacv_class;
static struct device*   ambacv_device;
static void __iomem     *rtos_io_base = NULL;
static void __iomem     *schdr_mem_base = NULL;
static void __iomem     *dsp_data_base = NULL;
static visorc_safety_heartbeat_t *ambacv_r52_trace;
#if defined(ENABLE_AMBA_MAL)
static uint64_t cv_schdr_size = 0UL;
static AMBA_MAL_BUF_s SchdrMem;
AMBA_MAL_INFO_s     krn_schdr_core_info;
AMBA_MAL_INFO_s     krn_schdr_cma_info;
AMBA_MAL_INFO_s     krn_schdr_rtos_info;
AMBA_MAL_INFO_s     krn_dsp_data_info;
#else
struct ambarella_cavalry  cavalry_support;
#endif

module_param(enable_arm_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_arm_cache, "make cv memory cacheable to ARM");
module_param(enable_rtos_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_rtos_cache, "make cv rtos memory cacheable to ARM");
module_param(enable_orc_cache, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_arm_cache, "enable ORC cache");
module_param(enable_log_msg, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(enable_log_msg, "enable message log");
module_param(cma_superdag_size, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(cma_superdag_size, "Size reserved for SuperDAG when CMA is enabled");

/*================================================================================================*/
/*=- CVSCHED support files: will be placed in another file in the future -========================*/
/*================================================================================================*/
typedef struct {
    struct proc_dir_entry  *cvload_proc;
    struct proc_dir_entry  *autorun_proc;
    struct proc_dir_entry  *cvstatus_proc;
} cvflow_scheduler_proc_t;

typedef struct {
    struct proc_dir_entry  *flexidag_proc;
    struct proc_dir_entry  *cvver_proc;
} flexidag_system_proc_t;

static cvflow_scheduler_proc_t      scheduler_proc;
static flexidag_system_proc_t       flexidag_system_proc;

extern cvflow_scheduler_support_t   scheduler_support;
extern flexidag_system_support_t    flexidag_system_support;

static int cvload_proc_show(struct seq_file *m, void *v)
{
    cvflow_scheduler_support_t *pSysSupport;

    pSysSupport = (cvflow_scheduler_support_t *)m->private;

    if (pSysSupport->vpSchedulerTrace != NULL) {
        uint32_t  thread_loop;
        scheduler_trace_t *pSchedulerTrace;
        cvthread_trace_t  *pCVThreadLoad;

        pSchedulerTrace = (scheduler_trace_t *)pSysSupport->vpSchedulerTrace;
        pCVThreadLoad   = &pSchedulerTrace->cvthread_loadtrace[0];

        seq_printf(m, "--- CORE NAME ---+- rpt. time -+--500ms--+-- 2.5s--+--10.0s--\n");

        for (thread_loop = 0; thread_loop < CVTRACE_MAX_ENTRIES; thread_loop++) {
            uint32_t   *pRecast;

            pRecast = (uint32_t *)pCVThreadLoad;

            if (pCVThreadLoad->cvcore_name[0] != 0) {
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
static int autorun_proc_show(struct seq_file *m, void *v)
{
    cvflow_scheduler_support_t *pSysSupport;

    pSysSupport = (cvflow_scheduler_support_t *)m->private;

    if (pSysSupport->vpAutoRunTrace != NULL) {
        autorun_trace_t *pAutoRunTraceEntry;
        uint32_t  num_entries;
        pAutoRunTraceEntry = (autorun_trace_t *)pSysSupport->vpAutoRunTrace;
        krn_cache_invalidate(pAutoRunTraceEntry, (sizeof(autorun_trace_t) * MAX_AUTORUN_CVTASKS));
        num_entries = pAutoRunTraceEntry->autorun_cvtask_num;
        if (num_entries == 0U) {
            seq_printf(m, "[AUTORUN] : No AutoRUN tasks exist in the system\n");
        } /* if (num_entries == 0U) */
        else if (num_entries < MAX_AUTORUN_CVTASKS) {
            uint32_t  loop;
            seq_printf(m, "--- AUTORUN_CVTASK_NAME -------- : per(ms) | ptime(ms) |   num_pass @(  passtime) |   num_fail @(  failtime) | failrc\n");
            for (loop = 0; loop < num_entries; loop++) {
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
        else { /* if (num_entries >= MAX_AUTORUN_CVTASKS) */
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
    if (ambacv_r52_trace != NULL) {
        uint32_t hashval;
        krn_cache_invalidate(ambacv_r52_trace, sizeof(visorc_safety_heartbeat_t));
        hashval = krn_schdrmsg_calc_hash(ambacv_r52_trace, sizeof(visorc_safety_heartbeat_t) - 4, FNV1A_32_INIT_HASH);
        if (hashval == ambacv_r52_trace->fnv1a_checksum) {
            seq_printf(m, "[%px] @ [0x%llx] : 0x%08x 0x%08x 0x%08x 0x%08x [CS pass]\n",
                       ambacv_r52_trace, krn_v2c(ambacv_r52_trace), ambacv_r52_trace->hb_sequence_no, ambacv_r52_trace->hb_time, ambacv_r52_trace->schdr_state, ambacv_r52_trace->fnv1a_checksum);
        } /* if (hashval == ambacv_r52_trace->fnv1a_checksum) */
        else { /* if (hashval != ambacv_r52_trace->fnv1a_checksum) */
            seq_printf(m, "[%px] @ [0x%llx] : 0x%08x 0x%08x 0x%08x 0x%08x [CS fail : 0x%08x]\n",
                       ambacv_r52_trace, krn_v2c(ambacv_r52_trace), ambacv_r52_trace->hb_sequence_no, ambacv_r52_trace->hb_time, ambacv_r52_trace->schdr_state, ambacv_r52_trace->fnv1a_checksum, hashval);
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

static void krn_scheduler_proc_init(void)
{
    scheduler_proc.cvload_proc = proc_create_data("cvload", S_IRUGO,
                                 get_ambarella_proc_dir(), &cvload_proc_fops, &scheduler_support);
    scheduler_proc.autorun_proc = proc_create_data("autorun", S_IRUGO,
                                  get_ambarella_proc_dir(), &autorun_proc_fops, &scheduler_support);
#ifdef ASIL_COMPLIANCE
    scheduler_proc.cvstatus_proc = proc_create_data("cvstatus", S_IRUGO,
                                   get_ambarella_proc_dir(), &cvstatus_proc_fops, &scheduler_support);
#else /* !ASIL_COMPLIANCE */
    scheduler_proc.cvstatus_proc = NULL;
#endif /* ?ASIL_COMPLIANCE */
} /* krn_scheduler_proc_init() */

static void krn_scheduler_proc_shutdown(void)
{
    if (scheduler_proc.cvload_proc != NULL) {
        remove_proc_entry("cvload", get_ambarella_proc_dir());
        scheduler_proc.cvload_proc = NULL;
    } /* if (scheduler_proc.cvload_proc != NULL) */
    if (scheduler_proc.autorun_proc != NULL) {
        remove_proc_entry("autorun", get_ambarella_proc_dir());
        scheduler_proc.autorun_proc = NULL;
    } /* if (scheduler_proc.autorun_proc != NULL) */
    if (scheduler_proc.cvstatus_proc != NULL) {
        remove_proc_entry("cvstatus", get_ambarella_proc_dir());
        scheduler_proc.cvstatus_proc = NULL;
    } /* if (scheduler_proc.cvstatus_proc  != NULL) */
} /* krn_scheduler_proc_shutdown() */

/*================================================================================================*/
/*=- /proc/ambarella/flexidag support -===========================================================*/
/*================================================================================================*/
static int flexidag_proc_show(struct seq_file *m, void *v)
{
    flexidag_system_support_t *pSysSupport;
    uint32_t  num_used, loop;

    pSysSupport = (flexidag_system_support_t *)m->private;

    num_used = 0;
    mutex_lock(&pSysSupport->slot_mutex);
    for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++) {
        if (pSysSupport->slot_state[loop] != FLEXIDAG_SLOT_UNUSED) {
            seq_printf(m, "FlexiDAG[%3d] : vpFlexibin = %p (0x%llx) | vpStateBuffer = %p (0x%llx) | vpTempBuffer = %p (0x%llx)\n", loop,
                       pSysSupport->pSlotState[loop].vpFlexiBin, pSysSupport->pSlotState[loop].flexibin_daddr,
                       pSysSupport->pSlotState[loop].vpStateBuffer, pSysSupport->pSlotState[loop].state_buffer_daddr,
                       pSysSupport->pSlotState[loop].vpTempBuffer, pSysSupport->pSlotState[loop].temp_buffer_daddr);
            num_used++;
        }
    }
    mutex_unlock(&pSysSupport->slot_mutex);

    if (pSysSupport->vpFlexidagTrace != NULL) {
        flexidag_trace_t *pFlexidagTrace;

        pFlexidagTrace = (flexidag_trace_t *)pSysSupport->vpFlexidagTrace;
        seq_printf(m, "FlexiDAG System State : [wt=%10u] : slots used [%3d/%3d]\n", pFlexidagTrace->last_wake_time, pFlexidagTrace->num_slots_used, pFlexidagTrace->num_slots_enabled);
        for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++) {
            flexidag_slot_trace_t *pFlexidagSlotTrace;

            pFlexidagSlotTrace = &pFlexidagTrace->slottrace[loop];
            if (pFlexidagSlotTrace->slot_state != 0U) {
                seq_printf(m, "FlexiDAG Trace [%3d] : [%10u] : state [%4d] : [rc=0x%x] : pending/finished fset_id [%5d/%5d]\n", loop,
                           pFlexidagSlotTrace->last_wake_time, pFlexidagSlotTrace->slot_state, pFlexidagSlotTrace->last_errorcode,
                           pFlexidagSlotTrace->pending_frameset_id, pFlexidagSlotTrace->finished_frameset_id);
            } /* if (pFlexidagSlotTrace->slot_state != 0) */
        } /* for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++) */
    } /* if (pSysSupport->vpFlexidagTrace != NULL) */

    return 0;
}

static int flexidag_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, flexidag_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations flexidag_proc_fops = {
    .open = flexidag_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
};
#else
static const struct proc_ops flexidag_proc_fops = {
    .proc_open = flexidag_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
};
#endif
/*================================================================================================*/
/*=- /proc/ambarella/cvver support -==============================================================*/
/*================================================================================================*/
static int cvver_proc_show(struct seq_file *m, void *v)
{
    flexidag_system_support_t *pSysSupport;

    pSysSupport = (flexidag_system_support_t *)m->private;

    if (pSysSupport->vpFlexidagTrace == NULL) {
        seq_printf(m, "No active components in the system\n");
    } /* if (pSysSupport->vpFlexidagTrace == NULL) */
    else { /* if (pSysSupport->vpFlexidagTrace != NULL) */
        flexidag_trace_t       *pFlexidag_Trace;
        component_build_info_t *pEntry;
        uint32_t  slot_loop;

        pFlexidag_Trace = (flexidag_trace_t *)pSysSupport->vpFlexidagTrace;

        /* Report scheduler components */
        if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U) {
            pEntry = krn_c2v(pFlexidag_Trace->sys_buildinfo_table_daddr);
            if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) {
                seq_printf(m, "========== CVFLOW System Components ====================================================\n");
                while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) {
                    char *pName;
                    pName = krn_c2v(pEntry->component_name_daddr);
                    seq_printf(m, "%s\n", pName);
                    seq_printf(m, " sourcever : %s\n", pEntry->component_version);
                    seq_printf(m, " toolchain : %s\n", pEntry->tool_version);
                    pEntry++;
                } /* while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != NULL)) */
            } /* if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) */
        } /* if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U) */

        for (slot_loop = 0; slot_loop < pSysSupport->num_slots_enabled; slot_loop++) {
            if (pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr != 0U) {
                pEntry = krn_c2v(pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr);
                if (pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) {
                    seq_printf(m, "========== FlexiDAG Slot[%3u] Components ===============================================\n", slot_loop);
                    while (pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) { /* Components may exist with no name - i.e. COMPONENT_TYPE_FLEXIDAG_IO */
                        if (pEntry->component_name_daddr != 0U) {
                            char *pName;
                            pName = krn_c2v(pEntry->component_name_daddr);
                            seq_printf(m, "%s\n", pName);
                            seq_printf(m, " sourcever : %s\n", pEntry->component_version);
                            seq_printf(m, " toolchain : %s\n", pEntry->tool_version);
                        } /* if (pEntry->component_name_daddr != 0U) */
                        pEntry++;
                    } /* while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != NULL)) */
                } /* if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) */
            } /* if (pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr != 0U) */
        } /* for (slot_loop = 0; slot_loop < pSysSupport->num_slots_enabled; slot_loop++) */
    } /* if (pSysSupport->vpFlexidagTrace != NULL) */

    return 0;
}

static int cvver_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, cvver_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations cvver_proc_fops = {
    .open = cvver_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
};
#else
static const struct proc_ops cvver_proc_fops = {
    .proc_open = cvver_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
};
#endif

static void krn_flexidag_proc_init(void)
{
    flexidag_system_proc.flexidag_proc = proc_create_data("flexidag", S_IRUGO,
                                         get_ambarella_proc_dir(), &flexidag_proc_fops, &flexidag_system_support);
    flexidag_system_proc.cvver_proc    = proc_create_data("cvver", S_IRUGO,
                                         get_ambarella_proc_dir(), &cvver_proc_fops, &flexidag_system_support);
} /* krn_flexidag_proc_init() */

static void krn_flexidag_proc_shutdown(void)
{
    if (flexidag_system_proc.flexidag_proc != NULL) {
        remove_proc_entry("flexidag", get_ambarella_proc_dir());
        flexidag_system_proc.flexidag_proc = NULL;
    } /* if (flexidag_system_proc.flexidag_proc != NULL) */
    if (flexidag_system_proc.cvver_proc != NULL) {
        remove_proc_entry("cvver", get_ambarella_proc_dir());
        flexidag_system_proc.cvver_proc = NULL;
    } /* if (flexidag_system_proc.cvver_proc != NULL) */
} /* krn_flexidag_proc_shutdown() */

/*================================================================================================*/
/*================================================================================================*/
/*================================================================================================*/

#if defined(ENABLE_AMBA_MAL)
static int ambacv_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int rval;
    uint64_t base = 0UL, size = 0UL, end = 0UL;
    ambacv_all_mem_t *mem_all;

    mem_all = krn_ambacv_get_mem();
    base = vma->vm_pgoff << 12;
    size = vma->vm_end - vma->vm_start;
    end  =  base + size;

    if ((base >= mem_all->debug_port.base) && (end <= (mem_all->debug_port.base + mem_all->debug_port.size))) {
        // region is debug port
        vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
    } else {
        // region is illegal
        printk(KERN_ERR "[ERROR] ambacv_mmap() : unable to map physical range (base=0x%llx size=0x%llx)\n", base, size);
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        return -EPERM;
    }

    rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                           vma->vm_end - vma->vm_start, vma->vm_page_prot);
    if (rval) {
        printk(KERN_ERR "ambacv_mmap %d\n", rval);
        return rval;
    }

    //pr_info("ambacv: mmap region [0x%llX -- 0x%llX]\n", base, base + size);
    return rval;
}

static uint32_t ambacv_get_mmb_info(ambacv_get_info_t *arg)
{
    (void)arg;
    printk(KERN_ERR "[ERROR] ambacv_get_mmb_info() : not support\n");

    return 0;
}
#else
static void cavalry_cma_vm_open(struct vm_area_struct *vma)
{
    cavalry_cma_reference(&cavalry_support, vma->vm_private_data);

    return;
}

static void cavalry_cma_vm_close(struct vm_area_struct *vma)
{
    cavalry_cma_unreference(&cavalry_support, vma->vm_private_data);

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
    ambacv_all_mem_t *mem_all;

    mem_all = krn_ambacv_get_mem();
    allocated = 0;
    base = vma->vm_pgoff << 12;
    size = vma->vm_end - vma->vm_start;
    end  =  base + size;

    if ((base >= krn_c2p(mem_all->all.base + mem_all->all.size)) && (end <= (krn_c2p(mem_all->all.base + mem_all->all.size) + CAVALRY_MEM_LOG_SIZE))) {
        // region is cavalry log
        vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
    } else if ((base >= krn_c2p(mem_all->all.base)) && (end <= krn_c2p(mem_all->all.base + mem_all->all.size))) {
        // region is within cv shared memory
        if (!enable_arm_cache) {
            vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
        }

        if ((mem_all->cavalry_region.base != 0) && (mem_all->cavalry_region.size != 0)) {
            if ((base >= krn_c2p(mem_all->cavalry_region.base)) && (end <= krn_c2p(mem_all->cavalry_region.base + mem_all->cavalry_region.size))) {
                vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
            }
        }
    } else if ((base >= mem_all->debug_port.base) && (end <= (mem_all->debug_port.base + mem_all->debug_port.size))) {
        // region is debug port
        vma->vm_page_prot=pgprot_noncached(vma->vm_page_prot);
    } else if ((base >= krn_c2p(mem_all->rtos_region.base)) && (end <= krn_c2p(mem_all->rtos_region.base + mem_all->rtos_region.size))) {
        if (enable_rtos_cache == 0) {
            vma->vm_page_prot=pgprot_writecombine(vma->vm_page_prot);
        } /* if (enable_rtos_cache == 0) */
    } else if ((base >= krn_c2p(mem_all->cma_region.base)) && (end <= krn_c2p(mem_all->cma_region.base + mem_all->cma_region.size))) {
        /* from cavalry_dev.c : cavalry_mmap() */
        struct cma_mmb *mmb = NULL, *_mmb = NULL;
        uint32_t   found = 0;

        /* user memory */
        mutex_lock(&cavalry_support.cavalry_cma_mutex);
        if (!list_empty(&cavalry_support.mmb_list)) {
            list_for_each_entry_safe(mmb, _mmb, &cavalry_support.mmb_list, list) {
                if ((base >= mmb->phys_addr) && (base < mmb->phys_addr + mmb->size)) {
                    if (!mmb->cache_en) {
                        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
                    }
                    found = 1;
                    break;
                }
            }
        }
        mutex_unlock(&cavalry_support.cavalry_cma_mutex);

        if (!found) {
            printk(KERN_ERR "mmap not found in CMA region, assuming cached direct mapping (base=0x%llx size=0x%llx)\n", base, size);
        } else {
            rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                                   vma->vm_end - vma->vm_start, vma->vm_page_prot);
            if (rval) {
                printk(KERN_ERR "mmap pfg rang err: %d\n", rval);
            } else {
                vma->vm_ops = &cavalry_cma_vm_ops;
                vma->vm_private_data = mmb;
                cavalry_cma_vm_open(vma);
            }
            allocated = 1;
        }
    } else {
        // region is illegal
        printk(KERN_ERR "[ERROR] ambacv_mmap() : unable to map physical range (base=0x%llx size=0x%llx)\n", base, size);
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        return -EPERM;
    }

    if (allocated == 0) {
        rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                               vma->vm_end - vma->vm_start, vma->vm_page_prot);
        if (rval) {
            printk(KERN_ERR "ambacv_mmap %d\n", rval);
            return rval;
        }
    }

    //pr_info("ambacv: mmap region [0x%llX -- 0x%llX]\n", base, base + size);
    return rval;
}

static uint32_t ambacv_get_mmb_info(ambacv_get_info_t *arg)
{
    uint32_t retcode = ERRCODE_NONE;
    ambacv_all_mem_t *mem_all;

    mem_all = krn_ambacv_get_mem();

    if (arg->index[0] == 0x1) {
        if ((arg->index[1] >= krn_c2p(mem_all->all.base)) && (arg->index[1] < krn_c2p(mem_all->all.base + mem_all->all.size))) {
            arg->value[0] = enable_arm_cache;
        } else if ((arg->index[1] >= krn_c2p(mem_all->rtos_region.base)) && (arg->index[1] < krn_c2p(mem_all->rtos_region.base + mem_all->rtos_region.size))) {
            arg->value[0] = enable_rtos_cache;
        } else if ((arg->index[1] >= krn_c2p(mem_all->cma_region.base)) && (arg->index[1] < krn_c2p(mem_all->cma_region.base + mem_all->cma_region.size))) {
            /* from cavalry_dev.c : cavalry_mmap() */
            struct cma_mmb *mmb = NULL, *_mmb = NULL;
            uint32_t   found = 0;

            /* user memory */
            mutex_lock(&cavalry_support.cavalry_cma_mutex);
            if (!list_empty(&cavalry_support.mmb_list)) {
                list_for_each_entry_safe(mmb, _mmb, &cavalry_support.mmb_list, list) {
                    if ((arg->index[1] >= mmb->phys_addr) && (arg->index[1] < mmb->phys_addr + mmb->size)) {
                        arg->value[0] = mmb->cache_en;
                        found = 1;
                        break;
                    }
                }
            }
            mutex_unlock(&cavalry_support.cavalry_cma_mutex);

            if (!found) {
                printk(KERN_ERR "addr 0x%llx not found in CMA region\n", arg->index[1]);
                retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
            }
        } else {
            printk(KERN_ERR "addr 0x%llx not found in ambacv region\n", arg->index[1]);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        }
    } else {
        printk(KERN_ERR "unknown index 0x%llx\n", arg->index[0]);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    }

    return retcode;
}
#endif

static long ambacv_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    uint32_t retcode = ERRCODE_NONE;

    switch (cmd) {
    case AMBACV_START_VISORC: {
        visorc_init_params_t arg_init_param;

        retcode = krn_copy_from_user(&arg_init_param, (void *)arg, sizeof(arg_init_param));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_init_param);
        }
        break;
    }
    case AMBACV_FIND_CVTASKMSG: {
        ambacv_get_value_t arg_get_value;

        retcode = krn_copy_from_user(&arg_get_value, (void *)arg, sizeof(arg_get_value));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_get_value);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_get_value,sizeof(arg_get_value));
            }
        }
        break;
    }
    case AMBACV_GET_SYSCONFIG: {
        visorc_init_params_t arg_init_param;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_init_param);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_init_param,sizeof(arg_init_param));
        }
        break;
    }
    case AMBACV_RECV_ORCMSG: {
        ambacv_get_value_t arg_get_value;

        retcode = krn_copy_from_user(&arg_get_value, (void *)arg,sizeof(arg_get_value));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_get_value);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_get_value,sizeof(arg_get_value));
            }
        }
        break;
    }
    case AMBACV_SEND_ARMMSG: {
        armvis_msg_t arg_vis_msg;

        retcode = krn_copy_from_user(&arg_vis_msg, (void *)arg,sizeof(arg_vis_msg));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_vis_msg);
        }
        break;
    }
    case AMBACV_GET_MEM_LAYOUT: {
        ambacv_mem_t arg_mem;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_mem);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_mem,sizeof(arg_mem));
        }
        break;
    }
    case AMBACV_UCODE_LOADED: {
        ambacv_mem_t arg_mem;

        retcode = krn_copy_from_user(&arg_mem, (void *)arg,sizeof(arg_mem));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_mem);
        }
        break;
    }
    case AMBACV_STOP_VISORC: {
        retcode = krn_ambacv_ioctl(f, cmd, (void *)arg);
        break;
    }
    case AMBACV_SEND_VPMSG: {
        ambacv_idspvis_msg_t arg_idspvis_msg;

        retcode = krn_copy_from_user(&arg_idspvis_msg, (void *)arg,sizeof(arg_idspvis_msg));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_idspvis_msg);
        }
        break;
    }
    case AMBACV_CONFIG_IDSP: {
        ambacv_cfg_addr_t arg_cfg_addr;

        retcode = krn_copy_from_user(&arg_cfg_addr, (void *)arg,sizeof(arg_cfg_addr));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cfg_addr);
        }
        break;
    }
    case AMBACV_CACHE_CLEAN: {
        ambacv_mem_region_t arg_mem_region;

        retcode = krn_copy_from_user(&arg_mem_region, (void *)arg,sizeof(arg_mem_region));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_mem_region);
        }
        break;
    }
    case AMBACV_CACHE_INVLD: {
        ambacv_mem_region_t arg_mem_region;

        retcode = krn_copy_from_user(&arg_mem_region, (void *)arg,sizeof(arg_mem_region));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_mem_region);
        }
        break;
    }
    case AMBACV_SET_BIN_PATH: {
        ambacv_log_t arg_log;

        retcode = krn_copy_from_user(&arg_log, (void *)arg, sizeof(arg_log));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_log);
        }
        break;
    }
    case AMBACV_GET_LOG_INFO: {
        ambacv_log_t arg_log;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_log);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_log,sizeof(arg_log));
        }
        break;
    }
    case AMBACV_SEND_ASYNCMSG: {
        ambacv_asynvis_msg_t arg_asynvis_msg;

        retcode = krn_copy_from_user(&arg_asynvis_msg, (void *)arg,sizeof(arg_asynvis_msg));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_asynvis_msg);
        }
        break;
    }
    case AMBACV_GET_VERSION: {
        uint32_t arg_value;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_value,sizeof(arg_value));
        }
        break;
    }
    case AMBACV_GET_MAL_VERSION: {
        uint32_t arg_value;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_value,sizeof(arg_value));
        }
        break;
    }
    case AMBACV_GET_MMB_INFO: {
        ambacv_get_info_t arg_get_info;

        retcode = krn_copy_from_user(&arg_get_info, (void *)arg,sizeof(arg_get_info));
        if(retcode == ERRCODE_NONE) {
            retcode = ambacv_get_mmb_info(&arg_get_info);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_get_info,sizeof(arg_get_info));
            }
        }
        break;
    }
    case AMBACV_GET_VALUE: {
        ambacv_get_value_t arg_get_value;

        retcode = krn_copy_from_user(&arg_get_value, (void *)arg,sizeof(arg_get_value));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_get_value);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_get_value,sizeof(arg_get_value));
            }
        }
        break;
    }
    case AMBACV_CORE_MUTEX: {
        uint32_t arg_value;

        retcode = krn_copy_from_user(&arg_value, (void *)arg,sizeof(arg_value));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        }
        break;
    }
    case AMBACV_RESUME_VISORC: {
        visorc_init_params_t arg_init_param;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_init_param);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_init_param,sizeof(arg_init_param));
        }
        break;
    }
    case AMBACV_GET_STATE: {
        uint32_t arg_value;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_value,sizeof(arg_value));
        }
        break;
    }
    case AMBACV_GET_ATT: {
        ambacv_att_region_t arg_att_region;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_att_region);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_att_region,sizeof(arg_att_region));
        }
        break;
    }
    case AMBACV_GET_TIME: {
        uint32_t arg_value;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_value,sizeof(arg_value));
        }
        break;
    }
    case AMBACV_WAIT_ORCMSG: {
        uint32_t arg_value;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_value,sizeof(arg_value));
        }
        break;
    }
    case AMBACV_SEND_VINMSG: {
        ambacv_vinvis_msg_t arg_vinvis_msg;

        retcode = krn_copy_from_user(&arg_vinvis_msg, (void *)arg,sizeof(arg_vinvis_msg));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_vinvis_msg);
        }
        break;
    }
    case AMBACV_CONFIG_VIN: {
        ambacv_cfg_addr_t arg_cfg_addr;

        retcode = krn_copy_from_user(&arg_cfg_addr, (void *)arg,sizeof(arg_cfg_addr));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cfg_addr);
        }
        break;
    }
    /*-= Flexidag specific IOCTL =--------------------------------------------*/
    case AMBACV_FLEXIDAG_ENABLE: {
        uint32_t arg_value;

        retcode = krn_copy_from_user(&arg_value, (void *)arg,sizeof(arg_value));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_value);
        }
        break;
    }
    case AMBACV_FLEXIDAG_CREATE: {
        flexidag_ioctl_create_t arg_create;

        retcode = krn_copy_from_user(&arg_create, (void *)arg,sizeof(arg_create));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_create);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_create,sizeof(arg_create));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_CREATE_QUERY: {
        flexidag_ioctl_create_t arg_create;

        retcode = krn_copy_from_user(&arg_create, (void *)arg,sizeof(arg_create));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_create);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_create,sizeof(arg_create));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_PARAMETER: {
        flexidag_ioctl_param_t arg_param;

        retcode = krn_copy_from_user(&arg_param, (void *)arg,sizeof(arg_param));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_param);
        }
        break;
    }
    case AMBACV_FLEXIDAG_ADD_TBAR: {
        flexidag_ioctl_memblk_t arg_memblk;

        retcode = krn_copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_ADD_SFB: {
        flexidag_ioctl_memblk_t arg_memblk;

        retcode = krn_copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_OPEN: {
        flexidag_ioctl_open_t arg_open;

        retcode = krn_copy_from_user(&arg_open, (void *)arg,sizeof(arg_open));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_open);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_open,sizeof(arg_open));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_OPEN_QUERY: {
        flexidag_ioctl_open_t arg_open;

        retcode = krn_copy_from_user(&arg_open, (void *)arg,sizeof(arg_open));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_open);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_open,sizeof(arg_open));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_STATE_BUFFER: {
        flexidag_ioctl_memblk_t arg_memblk;

        retcode = krn_copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_TEMP_BUFFER: {
        flexidag_ioctl_memblk_t arg_memblk;

        retcode = krn_copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_INIT: {
        flexidag_ioctl_handle_t arg_handle;

        retcode = krn_copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_handle);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_INIT_QUERY: {
        flexidag_ioctl_handle_t arg_handle;

        retcode = krn_copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_handle);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_PREP_RUN: {
        flexidag_ioctl_pre_run_t arg_pre_run;

        retcode = krn_copy_from_user(&arg_pre_run, (void *)arg,sizeof(arg_pre_run));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_pre_run);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_pre_run,sizeof(arg_pre_run));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_INPUT_BUFFER: {
        flexidag_ioctl_io_memblk_t arg_io_memblk;

        retcode = krn_copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_io_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER: {
        flexidag_ioctl_io_memblk_t arg_io_memblk;

        retcode = krn_copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_io_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER: {
        flexidag_ioctl_io_memblk_t arg_io_memblk;

        retcode = krn_copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_io_memblk);
        }
        break;
    }
    case AMBACV_FLEXIDAG_RUN: {
        flexidag_ioctl_run_t arg_run;

        retcode = krn_copy_from_user(&arg_run, (void *)arg,sizeof(arg_run));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_run);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_run,sizeof(arg_run));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_RUN_QUERY: {
        flexidag_ioctl_run_t arg_run;

        retcode = krn_copy_from_user(&arg_run, (void *)arg,sizeof(arg_run));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_run);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_run,sizeof(arg_run));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_WAIT_RUN_FINISH: {
        flexidag_ioctl_run_t arg_run;

        retcode = krn_copy_from_user(&arg_run, (void *)arg,sizeof(arg_run));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_run);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_run,sizeof(arg_run));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_CLOSE: {
        flexidag_ioctl_handle_t arg_handle;

        retcode = krn_copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_handle);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_CLOSE_QUERY: {
        flexidag_ioctl_handle_t arg_handle;

        retcode = krn_copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_handle);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
            }
        }
        break;
    }
    case AMBACV_FLEXIDAG_INFO: {
        flexidag_ioctl_handle_t arg_handle;

        retcode = krn_copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_handle);
        }
        break;
    }
    case AMBACV_FLEXIDAG_HANDLE_MSG: {
        armvis_msg_t arg_vis_msg;

        retcode = krn_copy_from_user(&arg_vis_msg, (void *)arg,sizeof(arg_vis_msg));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_vis_msg);
        }
        break;
    }
    /*-= Cavalry CMA specific IOCTL =---------------------------------------------*/
    case CAVALRY_QUERY_BUF: {
        struct cavalry_mem arg_cavalry_mem;

        retcode = krn_copy_from_user(&arg_cavalry_mem, (void *)arg,sizeof(arg_cavalry_mem));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cavalry_mem);
        }
        break;
    }
    case CAVALRY_ALLOC_MEM: {
        struct cavalry_mem arg_cavalry_mem;

        retcode = krn_copy_from_user(&arg_cavalry_mem, (void *)arg,sizeof(arg_cavalry_mem));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cavalry_mem);
            if(retcode == ERRCODE_NONE) {
                retcode = krn_copy_to_user((void *)arg, &arg_cavalry_mem,sizeof(arg_cavalry_mem));
            }
        }
        break;
    }
    case CAVALRY_FREE_MEM: {
        struct cavalry_mem arg_cavalry_mem;

        retcode = krn_copy_from_user(&arg_cavalry_mem, (void *)arg,sizeof(arg_cavalry_mem));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cavalry_mem);
        }
        break;
    }
    case CAVALRY_SYNC_CACHE_MEM: {
        struct cavalry_cache_mem arg_cavalry_cache;

        retcode = krn_copy_from_user(&arg_cavalry_cache, (void *)arg,sizeof(arg_cavalry_cache));
        if(retcode == ERRCODE_NONE) {
            retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cavalry_cache);
        }
        break;
    }
    case CAVALRY_GET_USAGE_MEM: {
        struct cavalry_usage_mem arg_cavalry_usage;

        retcode = krn_ambacv_ioctl(f, cmd, (void *)&arg_cavalry_usage);
        if(retcode == ERRCODE_NONE) {
            retcode = krn_copy_to_user((void *)arg, &arg_cavalry_usage,sizeof(arg_cavalry_usage));
        }
        break;
    }
    /*-= Superdag/Cavalry specific IOCTL =------------------------------------*/
    case CAVALRY_ENABLE:
        retcode = krn_ambacv_ioctl(f, cmd, (void *)arg);
        break;
    case CAVALRY_ASSOCIATE_BUF:
        retcode = krn_ambacv_ioctl(f, cmd, (void *)arg);
        break;
    case CAVALRY_SET_MEMORY_BLOCK:
        retcode = krn_ambacv_ioctl(f, cmd, (void *)arg);
        break;
    case CAVALRY_HANDLE_VISORC_REPLY:
        retcode = krn_ambacv_ioctl(f, cmd, (void *)arg);
        break;
    default:
        krn_printU5("[ERROR] ambacv_ioctl() : unknown IOCTL 0x%x ", cmd, 0U, 0U, 0U, 0U);
        retcode = (int32_t)ERR_DRV_SCHDR_IOCTL_UNKNOW;
        break;
    }

    if(retcode != ERRCODE_NONE) {
        krn_printU5("[ERROR] ambacv_ioctl() : cmd 0x%x fail ret 0x%x ", cmd, retcode, 0U, 0U, 0U);
    }
    return retcode;
}

static int ambacv_release(struct inode *inode, struct file *file)
{
    uint64_t owner = 0UL;

    krn_typecast(&owner, &file);
    krn_flexidag_close_by_owner(owner);
    return 0;
}

static const struct file_operations ambacv_fops = {
    .owner = THIS_MODULE,
    .mmap = ambacv_mmap,
    .unlocked_ioctl = ambacv_ioctl,
    .release = ambacv_release,
};

#if defined(ENABLE_AMBA_MAL)
static int __init ambacv_of_get_value(struct device_node *np)
{
    __be32 *reg;
    int len, rval;
    ambacv_all_mem_t *mem_all;
    struct device_node *cpu_node;
    uint32_t cpu_id;

    mem_all = krn_ambacv_get_mem();
    /* Get scheduler ID */
    cpu_node = of_get_next_cpu_node(NULL);
    reg = (__be32 *) of_get_property(cpu_node, "reg", &len);
    if (reg) {
        cpu_id = be32_to_cpu(reg[0]);
        cluster_id = ((cpu_id >> 16U) & 0xFFU);
        scheduler_id = cluster_id;
        printk("[AMBACV] CPU ID        :   0x%x \n", cpu_id);
    } else {
        scheduler_id = 0U;
    }
    printk("[AMBACV] SCHEDULER ID  :   %d \n", scheduler_id);

    mem_all->cv_region.base = 0x0U;
    mem_all->cv_region.size = 0xFFFFFFFFFFU;
    mem_all->cv_att_region.base = 0x0U;
    mem_all->cv_att_region.size = 0xFFFFFFFFFFU;

    reg = (__be32 *) of_get_property(np, "cv_schdr_size", &len);
    if (reg && (len == sizeof(u32))) {
        rval = of_property_read_u32(np, "cv_schdr_size", (uint32_t *)&cv_schdr_size);
    } else if (reg && (len == (2*sizeof(u32)))) {
        rval = of_property_read_u64(np, "cv_schdr_size", &cv_schdr_size);
    } else {
        rval = -1;
    }

    return rval;
}

static int __init ambacv_of_get_mal_info(void)
{
    int rval = 0;

    if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SCHDR, &krn_schdr_core_info) != 0U) {
        printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SCHDR fail \n");
        rval = -1;
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SYS, &krn_schdr_cma_info) != 0U) {
            printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SYS fail \n");
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_RTOS, &krn_schdr_rtos_info) != 0U) {
            printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_RTOS fail \n");
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_DSP_DATA, &krn_dsp_data_info) != 0U) {
            printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_DSP_DATA fail \n");
            rval = -1;
        }
    }
    return rval;
}

static void __init ambacv_of_init(struct device_node *np)
{
    void *vaddr = NULL;
    int rval;
    ambacv_all_mem_t *mem_all;
    uint32_t ret;

    mem_all = krn_ambacv_get_mem();
    rval = ambacv_of_get_value(np);
    if (rval == 0) {
        rval = ambacv_of_get_mal_info();
    }

    if(rval == 0) {
        /* Get CV Core mem range */
        if(krn_schdr_core_info.Type == AMBA_MAL_TYPE_INVALID) {
            if (cma_superdag_size >= 0) {
                printk("[AMBACV] CMA CONFIG  :  cv_schdr_size region defined (size=%lld bytes), user requests override of cma_superdag_size (%d bytes); honoring user request\n",
                       cv_schdr_size, cma_superdag_size * 1024 * 1024);
                cv_schdr_size = cma_superdag_size * 1024 * 1024;
            }

            ret = AmbaMAL_Alloc(AMBA_MAL_ID_CV_SYS, cv_schdr_size, 0x1000, &SchdrMem);
            if(ret != 0U) {
                printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Alloc fail ret=0x%x", ret);
            } else {
                if(enable_arm_cache == 1U) {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, SchdrMem.PhysAddr, SchdrMem.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                } else {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, SchdrMem.PhysAddr, SchdrMem.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                }

                if(vaddr == NULL) {
                    printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Map fail ret=0x%x", ret);
                    rval = -1;
                } else {
                    schdr_mem_base = vaddr;
                    mem_all->all.base = krn_p2c(SchdrMem.PhysAddr);
                    mem_all->all.size = cv_schdr_size - CAVALRY_MEM_LOG_SIZE;
                    mem_all->cma_region.base = krn_p2c(SchdrMem.PhysAddr + cv_schdr_size);
                    mem_all->cma_region.size = krn_schdr_cma_info.Size - cv_schdr_size;

                    krn_ambacv_set_sysinit((visorc_init_params_t *)(vaddr + SYSINIT_OFFSET));
                    ambacv_r52_trace = (visorc_safety_heartbeat_t *)(vaddr + 0x20000U);
                    krn_mmap_add_master(vaddr, SchdrMem.PhysAddr, cv_schdr_size, enable_arm_cache);
                    printk("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
                           enable_arm_cache ? "enabled" : "disabled",
                           enable_orc_cache ? "enabled" : "disabled");
                    printk("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
                           krn_schdr_cma_info.PhysAddr, (krn_schdr_cma_info.PhysAddr+krn_schdr_cma_info.Size));
                    printk("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
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
                printk("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Map fail ret=0x%x", ret);
                rval = -1;
            } else {
                schdr_mem_base = vaddr;
                mem_all->all.base = krn_p2c(krn_schdr_core_info.PhysAddr);
                mem_all->all.size = krn_schdr_core_info.Size - CAVALRY_MEM_LOG_SIZE;
                mem_all->cma_region.base = krn_p2c(krn_schdr_cma_info.PhysAddr);
                mem_all->cma_region.size = krn_schdr_cma_info.Size;

                krn_ambacv_set_sysinit((visorc_init_params_t *)(vaddr + SYSINIT_OFFSET));
                ambacv_r52_trace = (visorc_safety_heartbeat_t *)(vaddr + 0x20000U);
                krn_mmap_add_master(vaddr, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, enable_arm_cache);
                printk("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
                       enable_arm_cache ? "enabled" : "disabled",
                       enable_orc_cache ? "enabled" : "disabled");
                printk("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
                       krn_schdr_cma_info.PhysAddr, (krn_schdr_cma_info.PhysAddr+krn_schdr_cma_info.Size));
                printk("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                       krn_schdr_core_info.PhysAddr, (krn_schdr_core_info.PhysAddr+krn_schdr_core_info.Size));
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
                printk("[ERROR] ambacv_of_init() : RTOS: AmbaMAL_Map fail ret=0x%x", ret);
                rval = -1;
            } else {
                rtos_io_base = vaddr;
                mem_all->rtos_region.base = krn_p2c(krn_schdr_rtos_info.PhysAddr);
                mem_all->rtos_region.size = krn_schdr_rtos_info.Size;
                krn_mmap_add_rtos(vaddr, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, enable_rtos_cache);
                printk("[AMBACV] RTOS RANGE    :   [0x%llx--0x%llx] @ (%p) (cache is %s)\n",
                       krn_schdr_rtos_info.PhysAddr, (krn_schdr_rtos_info.PhysAddr + krn_schdr_rtos_info.Size), vaddr, (enable_rtos_cache != 0) ? "enabled" : "disabled");
            }
        }
    }

    if(rval == 0) {
        /* Get DSP DATA mem range */
        vaddr = NULL;
        if((krn_dsp_data_info.Type != AMBA_MAL_TYPE_INVALID) && (krn_dsp_data_info.Size != 0UL)) {
            ret = AmbaMAL_Map(AMBA_MAL_ID_DSP_DATA, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
            if(vaddr == NULL) {
                printk("[ERROR] ambacv_of_init() : RTOS: AmbaMAL_Map fail ret=0x%x", ret);
                rval = -1;
            } else {
                dsp_data_base = vaddr;
                krn_mmap_add_dsp_data(vaddr, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, enable_rtos_cache);
                printk("[AMBACV] DSP DATA RANGE    :   [0x%llx--0x%llx] @ (%p) \n",
                       krn_dsp_data_info.PhysAddr, (krn_dsp_data_info.PhysAddr + krn_dsp_data_info.Size), vaddr);
            }
        }
    }

    /* Get IRQ info */
    ambacv_irq = irq_of_parse_and_map(np, 0);
}
#else
static void __init ambacv_of_init(struct device_node *np)
{
    uint64_t base = 0UL, size = 0UL;
    uint64_t cv_schdr_base = 0UL, cv_schdr_size = 0UL;
    uint64_t cv_sys_base = 0UL, cv_sys_size = 0UL;
    char *vadd;
    __be32 *reg;
    __be64 *reg64;
    int len, rval;
    pgprot_t prot;
    struct device_node *node, *cpu_node;
    const char *name = of_node_full_name(np);
    uint64_t superdag_size = 0UL;
    uint64_t value1 = 0UL,value2 = 0UL,value3 = 0UL;
    ambacv_all_mem_t *mem_all;
    uint32_t cpu_id;

    mem_all = krn_ambacv_get_mem();
    cpu_node = of_get_next_cpu_node(NULL);
    reg = (__be32 *) of_get_property(cpu_node, "reg", &len);
    if (reg) {
        cpu_id = be32_to_cpu(reg[0]);
        cluster_id = ((cpu_id >> 16U) & 0xFFU);
        scheduler_id = cluster_id;
        printk("[AMBACV] CPU ID        :   0x%x \n", cpu_id);
    } else {
        scheduler_id = 0U;
    }
    printk("[AMBACV] SCHEDULER ID  :   %d \n", scheduler_id);

    cavalry_support.dev              = ambacv_device;
    cavalry_support.dev->of_node     = np;
    cavalry_support.dev->bus         = &platform_bus_type;

    mutex_init(&cavalry_support.cavalry_cma_mutex);
    INIT_LIST_HEAD(&cavalry_support.mmb_list);
    INIT_LIST_HEAD(&cavalry_support.free_mmb_list);

    /* Get CV mem range */
    node = of_parse_phandle(np, "memory-region", 0);
    if (node == NULL) {
        printk("err: no memory-region node!\n");
        BUG();
    } else {
        reg = (__be32 *) of_get_property(np, "cv_att_pa", &len);
        if (reg && (len == sizeof(u32))) {
            /* cv_region and cv_att_region info */
            rval = of_property_read_u32(np, "cv_att_pa", (uint32_t *)&value1);
            if (rval == 0) {
                rval = of_property_read_u32(np, "cv_att_size", (uint32_t *)&value2);
                if (rval == 0) {
                    rval = of_property_read_u32(np, "cv_att_ca", (uint32_t *)&value3);
                }
            }
        } else if (reg && (len == (2*sizeof(u32)))) {
            rval = of_property_read_u64(np, "cv_att_pa", &value1);
            if (rval == 0) {
                rval = of_property_read_u64(np, "cv_att_size", &value2);
                if (rval == 0) {
                    rval = of_property_read_u64(np, "cv_att_ca", &value3);
                }
            }
        } else {
            printk("err: cv_att_pa is in the wrong format\n");
            return;
        }

        if (rval == 0) {
            mem_all->cv_region.base = value1;
            mem_all->cv_region.size = value2;
            mem_all->cv_att_region.base = value3;
            mem_all->cv_att_region.size = value2;
        }

        printk("[AMBACV] ATT PA RANGE  :   [0x%llx--0x%llx]\n",mem_all->cv_region.base,(mem_all->cv_region.base+mem_all->cv_region.size));
        printk("[AMBACV] ATT CA RANGE  :   [0x%llx--0x%llx]\n",mem_all->cv_att_region.base,(mem_all->cv_att_region.base+mem_all->cv_region.size));

        /* all and cma_region info */
        reg = (__be32 *) of_get_property(node, "reg", &len);
        if (reg && (len == (2 * sizeof(u32)))) {
            cv_sys_base = be32_to_cpu(reg[0]);
            cv_sys_size = be32_to_cpu(reg[1]);
        } else if (reg && (len == (4 * sizeof(u32)))) {
            reg64 = (__be64 *)reg;
            cv_sys_base = be64_to_cpu(reg64[0]);
            cv_sys_size = be64_to_cpu(reg64[1]);
        } else {
            printk("err: memory-region is in the wrong format\n");
            return;
        }

        printk("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
               enable_arm_cache ? "enabled" : "disabled",
               enable_orc_cache ? "enabled" : "disabled");
        printk("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
               cv_sys_base, cv_sys_base + cv_sys_size);

        reg = NULL;
        node = of_parse_phandle(np, "memory-region2", 0);
        if (node != NULL) {
            reg = (__be32 *) of_get_property(node, "reg", &len);
        }
        if (reg != NULL) {
            if (reg && (len == (2 * sizeof(u32)))) {
                cv_schdr_base = be32_to_cpu(reg[0]);
                cv_schdr_size = be32_to_cpu(reg[1]);
            } else if (reg && (len == (4 * sizeof(u32)))) {
                reg64 = (__be64 *)reg;
                cv_schdr_base = be64_to_cpu(reg64[0]);
                cv_schdr_size = be64_to_cpu(reg64[1]);
            } else {
                printk("err: memory-region is in the wrong format\n");
                return;
            }

            prot = (enable_arm_cache)?__pgprot(PROT_NORMAL):__pgprot(PROT_NORMAL_NC);
            schdr_mem_base = __ioremap(cv_schdr_base, cv_schdr_size, prot);

            mem_all->all.base = krn_p2c(cv_schdr_base);
            mem_all->all.size = cv_schdr_size - CAVALRY_MEM_LOG_SIZE;
            mem_all->cma_region.base = krn_p2c(cv_sys_base);
            mem_all->cma_region.size = cv_sys_size;

            rval = cavalry_cma_init(&cavalry_support);
            if (rval < 0) {
                pr_err("cavalry_cma_init() : Returns error %d\n", rval);
                return;
            }
            cavalry_support.cma_private.size = cv_schdr_size;
            cavalry_support.cma_private.phys = krn_p2c(cv_schdr_base);
            cavalry_support.cma_private.virt = schdr_mem_base;
            cavalry_support.cavalry_mem_info[CAVALRY_MEM_USER].phys = mem_all->cma_region.base;
            cavalry_support.cavalry_mem_info[CAVALRY_MEM_USER].size = mem_all->cma_region.size;
            printk("[AMBACV] SCHDR PRIVATE   :   [0x%llx--0x%llx] @ (%p)\n",
                   (krn_c2p(cavalry_support.cma_private.phys)),
                   (krn_c2p(cavalry_support.cma_private.phys) + cavalry_support.cma_private.size),
                   cavalry_support.cma_private.virt);

        } else {
            reg = (__be32 *) of_get_property(np, "cv_schdr_size", &len);
            if (reg && (len == sizeof(u32))) {
                rval = of_property_read_u32(np, "cv_schdr_size", (uint32_t *)&superdag_size);
            } else if (reg && (len == (2*sizeof(u32)))) {
                rval = of_property_read_u64(np, "cv_schdr_size", &superdag_size);
            } else {
                rval = -1;
            }

            if (rval == 0) {
                if (cma_superdag_size >= 0) {
                    printk("[AMBACV] CMA CONFIG  :  cv_schdr_size region defined (size=%lld bytes), user requests override of cma_superdag_size (%d bytes); honoring user request\n",
                           superdag_size, cma_superdag_size * 1024 * 1024);
                    superdag_size = cma_superdag_size * 1024 * 1024;
                }
            } else {
                superdag_size = cma_superdag_size * 1024 * 1024;
            }

            mem_all->all.base = krn_p2c(cv_sys_base);
            mem_all->all.size = superdag_size - CAVALRY_MEM_LOG_SIZE;
            mem_all->cma_region.base = krn_p2c(cv_sys_base + superdag_size);
            mem_all->cma_region.size = cv_sys_size - superdag_size;

            /* cma_support init */
            cavalry_support.cma_private.size = mem_all->all.size + CAVALRY_MEM_LOG_SIZE;
            cavalry_support.cavalry_mem_info[CAVALRY_MEM_USER].phys = mem_all->cma_region.base;
            cavalry_support.cavalry_mem_info[CAVALRY_MEM_USER].size = mem_all->cma_region.size;
            rval = cavalry_cma_init(&cavalry_support);
            if (rval < 0) {
                printk("cavalry_cma_init() : Returns error %d\n", rval);
                return;
            } else {
                rval = cavalry_cma_alloc_schdr(&cavalry_support);
                if (rval < 0) {
                    printk("cavalry_cma_alloc_schdr() : Returns error %d\n", rval);
                    return;
                } else {
                    if(cavalry_support.cma_private.phys != mem_all->all.base) {
                        printk("cavalry_cma_init() : cma alloc start (0x%llx) is not at top of cma (0x%llx)\n", cavalry_support.cma_private.phys, mem_all->all.base);
                        return;
                    }
                }
            }
            printk("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] @ (%p)\n",
                   (krn_c2p(cavalry_support.cma_private.phys)),
                   (krn_c2p(cavalry_support.cma_private.phys) + cavalry_support.cma_private.size),
                   cavalry_support.cma_private.virt);
        }
        krn_ambacv_set_sysinit((visorc_init_params_t *)(cavalry_support.cma_private.virt + SYSINIT_OFFSET));
        ambacv_r52_trace = (visorc_safety_heartbeat_t *)(cavalry_support.cma_private.virt + 0x20000U);
        krn_mmap_add_master(cavalry_support.cma_private.virt, krn_c2p(cavalry_support.cma_private.phys), cavalry_support.cma_private.size, enable_arm_cache);
    }

    /* Get CV RTOS mem range */
    base = 0UL;
    size = 0UL;
    vadd = NULL;
    node = of_parse_phandle(np, "memory-region1", 0);
    if (node != NULL) {
        reg = (__be32 *) of_get_property(node, "reg", &len);
        if (reg && (len == (2 * sizeof(u32)))) {
            base = be32_to_cpu(reg[0]);
            size = be32_to_cpu(reg[1]);
        } else if (reg && (len == (4 * sizeof(u32)))) {
            reg64 = (__be64 *)reg;
            base = be64_to_cpu(reg64[0]);
            size = be64_to_cpu(reg64[1]);
        } else {
            base = 0UL;
            size = 0UL;
        }

        prot = (enable_rtos_cache != 0) ? __pgprot(PROT_NORMAL):__pgprot(PROT_NORMAL_NC);
        if(size != 0UL) {
            vadd = __ioremap(base, size, prot);
            BUG_ON(vadd == NULL);
            rtos_io_base = vadd;
            mem_all->rtos_region.base = krn_p2c(base);
            mem_all->rtos_region.size = size;
            krn_mmap_add_rtos(vadd, base, size, enable_rtos_cache);
        } else {
            mem_all->rtos_region.base = 0UL;
            mem_all->rtos_region.size = 0UL;
            krn_mmap_add_rtos(NULL, 0UL, 0UL, enable_rtos_cache);
        }
        printk("[AMBACV] RTOS RANGE    :   [0x%llx--0x%llx] @ (%p) (cache is %s)\n",
               base, (base + size), vadd, (enable_rtos_cache != 0) ? "enabled" : "disabled");
    }

    krn_mmap_add_dsp_data(NULL, 0UL, 0UL, 1);
    /* Get IRQ info */
    ambacv_irq = irq_of_parse_and_map(np, 0);
    printk("[AMBACV] CACHE CONFIG  :   ARM(%s), ORC(%s)\n",
           enable_arm_cache ? "enabled" : "disabled",
           enable_orc_cache ? "enabled" : "disabled");
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
        printk("[AMBACV] no device node \n");
        return -ENODEV;
    } else if (!of_device_is_available(node)) {
        of_node_put(node);
        printk("[AMBACV] device node is disabled \n");
        return -ENODEV;
    }

    ambacv_major = register_chrdev(0, DEVICE_NAME, &ambacv_fops);
    if (ambacv_major < 0) {
        printk("ambacv: failed to register device %d.\n", ambacv_major);
        return ambacv_major;
    }

    ambacv_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(ambacv_class)) {
        unregister_chrdev(ambacv_major, DEVICE_NAME);
        printk("ambacv: failed to create class.\n");
        return PTR_ERR(ambacv_class);
    }
    printk("[AMBACV] GIT VERSION   :   %s \n", BUILD_SHA1);
    printk("[AMBACV] BUILT BY      :   %s \n", BUILD_USER);
    printk("[AMBACV] BUILT TIME    :   %s \n", BUILD_TIME);
    printk("[AMBACV] API VERSION   :   %d.%d\n", CVSCHEDULER_INTERFACE_VERSION, CVSCHEDULER_KERNEL_VERSION);

    ambacv_device = device_create(ambacv_class, NULL, MKDEV(ambacv_major, 0),
                                  NULL, DEVICE_NAME);
    if (IS_ERR(ambacv_device)) {
        class_destroy(ambacv_class);
        unregister_chrdev(ambacv_major, DEVICE_NAME);
        printk("ambacv: falied to create device.\n");
        return PTR_ERR(ambacv_device);
    }

    for_each_matching_node_and_match(np, &__ambacv_of_table, &match) {
        if (!of_device_is_available(np)) {
            continue;
        }

        init_func = match->data;
        init_func(np);
    }

    /* Initialize scheduler proc */
    krn_scheduler_proc_init();

    /* Initialize flexidag proc */
    krn_flexidag_proc_init();

    (void) krn_ambacv_init();

    return 0;
}

static void __exit ambacv_exit(void)
{
    krn_scheduler_proc_shutdown();
    krn_flexidag_proc_shutdown();
    krn_ambacv_exit();
#if defined(ENABLE_AMBA_MAL)
    if (dsp_data_base != NULL) {
        AmbaMAL_Unmap(AMBA_MAL_ID_DSP_DATA, dsp_data_base, krn_dsp_data_info.Size);
    }

    if (rtos_io_base != NULL) {
        AmbaMAL_Unmap(AMBA_MAL_ID_CV_RTOS, rtos_io_base, krn_schdr_rtos_info.Size);
    }

    if (schdr_mem_base != NULL) {
        if(krn_schdr_core_info.Type == AMBA_MAL_TYPE_INVALID) {
            AmbaMAL_Unmap(AMBA_MAL_ID_CV_SYS, schdr_mem_base, cv_schdr_size);
            AmbaMAL_Free(AMBA_MAL_ID_CV_SYS, &SchdrMem);
        } else {
            AmbaMAL_Unmap(AMBA_MAL_ID_CV_SCHDR, schdr_mem_base, krn_schdr_core_info.Size);
        }
    }
#else
    cavalry_cma_exit(&cavalry_support);
    if (rtos_io_base != NULL) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
        __iounmap(rtos_io_base );
#else
        iounmap(rtos_io_base);
#endif
        rtos_io_base = NULL;
    } /* if (rtos_io_base != NULL) */

    if (schdr_mem_base != NULL) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
        __iounmap(schdr_mem_base );
#else
        iounmap(schdr_mem_base);
#endif
        schdr_mem_base = NULL;
    } /* if (rtos_io_base != NULL) */
#endif

    irq_dispose_mapping(ambacv_irq);
    ambacv_device->bus = NULL; /* Remove bus link */
    device_destroy(ambacv_class, MKDEV(ambacv_major, 0));
    class_destroy(ambacv_class);
    unregister_chrdev(ambacv_major, DEVICE_NAME);
    printk("ambacv: module exit\n");
    return;
}

module_init(ambacv_init);
module_exit(ambacv_exit);

