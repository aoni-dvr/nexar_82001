#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/chip.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "ambacv_kal.h"
#include "flexidag_ioctl.h"
#include <schdr_api.h> /* For flexidag_(slot_)trace_t structures */
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */

MODULE_AUTHOR("Clifton Kayano");
MODULE_LICENSE("GPL");

#define   MAX_FLEXIDAG_TOKENS   256

#define   BUFFER_CHECKSUM_SIZE  128U

typedef struct
{
  struct mutex     run_mutex;
  struct semaphore pending_run_semaphore;
  struct file *f;
  uint32_t    flexidag_slot_id;
  uint32_t    pending_token_id;
  int32_t     pending_run_count;
  /*-= Flexidag config =--------------------------------------------*/

  uint32_t    fdparam_interface_cvmsg_num;
  uint32_t    fdparam_internal_cvmsg_num;
  uint32_t    fdparam_log_size_arm_perf[SYSTEM_MAX_NUM_CORTEX];
  uint32_t    fdparam_log_size_arm_sched[SYSTEM_MAX_NUM_CORTEX];
  uint32_t    fdparam_log_size_arm_cvtask[SYSTEM_MAX_NUM_CORTEX];
  uint32_t    fdparam_log_size_vis_perf[SYSTEM_MAX_NUM_VISORC];
  uint32_t    fdparam_log_size_vis_sched[SYSTEM_MAX_NUM_VISORC];
  uint32_t    fdparam_log_size_vis_cvtask[SYSTEM_MAX_NUM_VISORC];
  uint32_t    fdparam_token_timeout;

  /*-= Flexidag memory requirements =-------------------------------*/
  flexidag_memory_requirements_t memreq;

  void       *vpFlexiBin;
  void       *vpStateBuffer;
  void       *vpTempBuffer;
  void       *vpInputBuffer[FLEXIDAG_MAX_INPUTS];
  void       *vpFeedbackBuffer[FLEXIDAG_MAX_FEEDBACKS];
  void       *vpOutputBuffer[FLEXIDAG_MAX_OUTPUTS];
  uint64_t   pNextOutputMemblk[FLEXIDAG_MAX_OUTPUTS];
  void       *vpExtraTbar[FLEXIDAG_MAX_TBAR];
  void       *vpExtraSfb[FLEXIDAG_MAX_SFB];

  uint64_t    flexibin_daddr;
  uint64_t    flexibin_size;
  uint64_t    state_buffer_daddr;
  uint64_t    state_buffer_size;
  uint64_t    temp_buffer_daddr;
  uint64_t    temp_buffer_size;
  uint64_t    input_buffer_daddr[FLEXIDAG_MAX_INPUTS];
  uint64_t    feedback_buffer_daddr[FLEXIDAG_MAX_FEEDBACKS];
  uint64_t    output_buffer_daddr[FLEXIDAG_MAX_OUTPUTS];
  uint64_t    input_buffer_size[FLEXIDAG_MAX_INPUTS];
  uint64_t    feedback_buffer_size[FLEXIDAG_MAX_FEEDBACKS];
  uint64_t    output_buffer_size[FLEXIDAG_MAX_OUTPUTS];
  uint64_t    extra_tbar_daddr[FLEXIDAG_MAX_TBAR];
  uint32_t    num_extra_tbar;
  uint64_t    extra_sfb_daddr[FLEXIDAG_MAX_SFB];
  uint16_t    extra_sfb_numlines[FLEXIDAG_MAX_SFB];
  uint32_t    num_extra_sfb;

} flexidag_slot_state_t;


typedef struct
{
  struct proc_dir_entry  *flexidag_proc;
  struct proc_dir_entry  *cvver_proc;
  flexidag_slot_state_t  *pSlotState;
  uint8_t                 num_slots_enabled;
  uint8_t                 slot_state[FLEXIDAG_MAX_SLOTS];
  uint64_t                pOutputMemBlk[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
  struct mutex            slot_mutex;
  struct mutex            token_mutex;
  uint32_t                next_token_index;
  uint64_t                flexidag_trace_daddr;
  void                   *vpFlexidagTrace;
  struct semaphore        token[MAX_FLEXIDAG_TOKENS];
  uint8_t                 token_state[MAX_FLEXIDAG_TOKENS];
  errcode_enum_t          token_retcode[MAX_FLEXIDAG_TOKENS];
  uint32_t                token_start_time[MAX_FLEXIDAG_TOKENS];
  uint32_t                token_end_time[MAX_FLEXIDAG_TOKENS];
  uint32_t                token_output_not_generated[MAX_FLEXIDAG_TOKENS];
  uint32_t                token_output_donetime[MAX_FLEXIDAG_TOKENS][FLEXIDAG_MAX_OUTPUTS];
} flexidag_system_support_t;


#define FLEXIDAG_TOKEN_UNUSED     0
#define FLEXIDAG_TOKEN_USED       1
#define FLEXIDAG_TOKEN_WAITING    2
#define FLEXIDAG_TOKEN_FINISHED   3

#define FLEXIDAG_SLOT_UNUSED      0
#define FLEXIDAG_SLOT_USED        1
#define FLEXIDAG_SLOT_UNAVAIL     2

static flexidag_slot_state_t      flexidag_slot_state[FLEXIDAG_MAX_SLOTS];
static flexidag_system_support_t  flexidag_system_support;

extern uint64_t core_flexidag_trace_daddr;
/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG Token Management (IOCTL/KERNEL) =---------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
uint32_t  flexidag_find_token(void)
{
  uint32_t  token_found;
  uint32_t  token_num;
  uint32_t  token_id;
  uint32_t  retcode;
  uint32_t  loop;

  token_found = 0;
  token_num   = 0;
  loop        = 0;

  mutex_lock(&flexidag_system_support.token_mutex);
  while ((token_found == 0) && (loop < MAX_FLEXIDAG_TOKENS))
  {
    token_id = (flexidag_system_support.next_token_index + loop) % MAX_FLEXIDAG_TOKENS;
    if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED)
    {
      uint32_t  output_loop;
      token_found = 1;
      token_num   = token_id;
      flexidag_system_support.token_state[token_id]       = FLEXIDAG_TOKEN_USED;
      flexidag_system_support.token_retcode[token_id]     = ERRCODE_NONE;
      flexidag_system_support.token_start_time[token_id]  = 0;
      flexidag_system_support.token_end_time[token_id]    = 0;
      flexidag_system_support.token_output_not_generated[token_id] = 0;
      for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++)
      {
        flexidag_system_support.token_output_donetime[token_id][output_loop] = 0;
      }
      flexidag_system_support.next_token_index        = (token_id + 1) % MAX_FLEXIDAG_TOKENS;
    } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) */
    loop++;
  } /* while ((token_found == 0) && (loop < MAX_FLEXIDAG_TOKENS)) */
  mutex_unlock(&flexidag_system_support.token_mutex);

  if (token_found == 0)
  {
    /* ERROR */
    pr_err("[ERROR] : flexidag_find_token() : Unable to find token in system\n");
    retcode = 0xFFFFFFFFU;
  } /* if (token_found == 0) */
  else /* if (token_found != 0) */
  {
    retcode = token_num;
  } /* if (token_found != 0) */

  return retcode;

} /* flexidag_find_token() */

errcode_enum_t  flexidag_wait_token(uint32_t token_id, uint32_t block, uint32_t timeout)
{
  uint32_t  wait_valid;
  errcode_enum_t  retcode;

  retcode       = ERRCODE_NONE;
  if (token_id >= MAX_FLEXIDAG_TOKENS)
  {
    /* ERROR */
    wait_valid  = 0;
    retcode     = 0xFFFFFFFFU;
  } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
  else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
  {
    if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED)
    {
      /* Swap to waiting */
      flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_WAITING;
      wait_valid = 1;
    }
    else if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) && (block > 1))
    {
      wait_valid = 0; /* Do nothing here */
    } /* if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_UNUSED) && (block > 1)) */
    else if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_USED)
    {
      /* ERROR */
      wait_valid  = 0;
      pr_err("[CORE] [ERROR] : flexidag_wait_token(token_id=%3d, block=%d) : Token state invalid (%d, expected %d)\n",
             token_id, block, flexidag_system_support.token_state[token_id], FLEXIDAG_TOKEN_USED);
      retcode     = 0xFFFFFFFEU;
    } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_USED) */
    else if (block == 0)
    {
      wait_valid = 0;
      flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_FINISHED;
    } /* if ((flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_USED) && (block != 0)) */
    else /* if (block == 0) */
    {
      flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_WAITING;
      wait_valid = 1;
    }
  } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

  mutex_unlock(&flexidag_system_support.token_mutex);

  if (wait_valid != 0)
  {
    int ret;

    ret = down_timeout(&flexidag_system_support.token[token_id], timeout);

    mutex_lock(&flexidag_system_support.token_mutex);
    if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED)
    {
      retcode = flexidag_system_support.token_retcode[token_id];
    } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
    else /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
    {
      pr_err("[CORE] [ERROR] : flexidag_wait_token(token_id=%3d, block=%d) : Token state invalid (%d, expected %d)\n",
             token_id, block, flexidag_system_support.token_state[token_id], FLEXIDAG_TOKEN_FINISHED);
      retcode = 0xFFFFFFFDU;
    } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_FINISHED) */
    flexidag_system_support.token_state[token_id] = FLEXIDAG_TOKEN_UNUSED;
    mutex_unlock(&flexidag_system_support.token_mutex);
  } /* if (wait_valid != 0) */

  return retcode;

} /* flexidag_wait_token() */

uint32_t  flexidag_release_token(uint32_t token_id, errcode_enum_t function_retcode, uint32_t function_start_time, uint32_t function_end_time, uint32_t function_output_not_generated)
{
  uint32_t  release_valid;
  uint32_t  retcode;

  retcode       = ERRCODE_NONE;
  if (token_id >= MAX_FLEXIDAG_TOKENS)
  {
    /* ERROR */
    release_valid = 0;
    retcode       = 0xFFFFFFFFU;
  } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
  else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
  {
    mutex_lock(&flexidag_system_support.token_mutex);
    if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED)
    {
      release_valid = 0; /* Token wasn't blocking */
      flexidag_system_support.token_state[token_id]       = FLEXIDAG_TOKEN_UNUSED;
      flexidag_system_support.token_retcode[token_id]     = function_retcode;
      flexidag_system_support.token_start_time[token_id]  = function_start_time;
      flexidag_system_support.token_end_time[token_id]    = function_end_time;
      flexidag_system_support.token_output_not_generated[token_id] = function_output_not_generated;
    } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_FINISHED) */
    else if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_WAITING)
    {
      /* ERROR */
      release_valid = 0;
      pr_err("[ERROR] : flexidag_release_token(token_id=%3d, retcode=0x%x, %10u %10u) : Token state invalid (%d, expected %d)\n",
             token_id, function_retcode, function_start_time, function_end_time, flexidag_system_support.token_state[token_id], FLEXIDAG_TOKEN_WAITING);
      retcode       = 0xFFFFFFFFU;
    } /* if (flexidag_system_support.token_state[token_id] != FLEXIDAG_TOKEN_WAITING) */
    else /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */
    {
      release_valid = 1;
    } /* if (flexidag_system_support.token_state[token_id] == FLEXIDAG_TOKEN_WAITING) */

    if (release_valid != 0)
    {
      flexidag_system_support.token_state[token_id]       = FLEXIDAG_TOKEN_FINISHED;
      flexidag_system_support.token_retcode[token_id]     = function_retcode;
      flexidag_system_support.token_start_time[token_id]  = function_start_time;
      flexidag_system_support.token_end_time[token_id]    = function_end_time;
      flexidag_system_support.token_output_not_generated[token_id] = function_output_not_generated;
    } /* if (release_valid != 0) */
    mutex_unlock(&flexidag_system_support.token_mutex);

    if (release_valid != 0) {
      up(&flexidag_system_support.token[token_id]);
    }
  } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

  return retcode;

} /* flexidag_release_token() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG initialization (IOCTL/KERNEL) =-----------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/

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
  for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++)
  {
    if (pSysSupport->slot_state[loop] != FLEXIDAG_SLOT_UNUSED)
    {
      seq_printf(m, "FlexiDAG[%3d] : vpFlexibin = %p (0x%llx) | vpStateBuffer = %p (0x%llx) | vpTempBuffer = %p (0x%llx)\n", loop,
                 pSysSupport->pSlotState[loop].vpFlexiBin, pSysSupport->pSlotState[loop].flexibin_daddr,
                 pSysSupport->pSlotState[loop].vpStateBuffer, pSysSupport->pSlotState[loop].state_buffer_daddr,
                 pSysSupport->pSlotState[loop].vpTempBuffer, pSysSupport->pSlotState[loop].temp_buffer_daddr);
      num_used++;
    }
  }
  mutex_unlock(&pSysSupport->slot_mutex);

  if (pSysSupport->vpFlexidagTrace != NULL)
  {
    flexidag_trace_t *pFlexidagTrace;

    pFlexidagTrace = (flexidag_trace_t *)pSysSupport->vpFlexidagTrace;
    seq_printf(m, "FlexiDAG System State : [wt=%10u] : slots used [%3d/%3d]\n", pFlexidagTrace->last_wake_time, pFlexidagTrace->num_slots_used, pFlexidagTrace->num_slots_enabled);
    for (loop = 0; loop < pSysSupport->num_slots_enabled; loop++)
    {
      flexidag_slot_trace_t *pFlexidagSlotTrace;

      pFlexidagSlotTrace = &pFlexidagTrace->slottrace[loop];
      if (pFlexidagSlotTrace->slot_state != 0)
      {
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
  if (pSysSupport->vpFlexidagTrace == NULL)
  {
    seq_printf(m, "No active components in the system\n");
  } /* if (pSysSupport->vpFlexidagTrace == NULL) */
  else /* if (pSysSupport->vpFlexidagTrace != NULL) */
  {
    flexidag_trace_t       *pFlexidag_Trace;
    component_build_info_t *pEntry;
    uint32_t  slot_loop;

    pFlexidag_Trace = (flexidag_trace_t *)pSysSupport->vpFlexidagTrace;

    /* Report scheduler components */
    if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U)
    {
      pEntry = ambacv_c2v(pFlexidag_Trace->sys_buildinfo_table_daddr);
      if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U))
      {
        seq_printf(m, "========== CVFLOW System Components ====================================================\n");
        while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U))
        {
          char *pName;
          pName = ambacv_c2v(pEntry->component_name_daddr);
          seq_printf(m, "%s\n", pName);
          seq_printf(m, " sourcever : %s\n", pEntry->component_version);
          seq_printf(m, " toolchain : %s\n", pEntry->tool_version);
          pEntry++;
        } /* while ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != NULL)) */
      } /* if ((pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) && (pEntry->component_name_daddr != 0U)) */
    } /* if (pFlexidag_Trace->sys_buildinfo_table_daddr != 0U) */

    for (slot_loop = 0; slot_loop < pSysSupport->num_slots_enabled; slot_loop++)
    {
      if (pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr != 0U)
      {
        pEntry = ambacv_c2v(pFlexidag_Trace->slottrace[slot_loop].fd_buildinfo_table_daddr);
        if (pEntry->component_type != COMPONENT_TYPE_END_OF_LIST)
        {
          seq_printf(m, "========== FlexiDAG Slot[%3u] Components ===============================================\n", slot_loop);
          while (pEntry->component_type != COMPONENT_TYPE_END_OF_LIST) /* Components may exist with no name - i.e. COMPONENT_TYPE_FLEXIDAG_IO */
          {
            if (pEntry->component_name_daddr != 0U)
            {
              char *pName;
              pName = ambacv_c2v(pEntry->component_name_daddr);
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
/*================================================================================================*/
/*================================================================================================*/
/*================================================================================================*/

void krn_flexidag_system_init(void)
{
  memset(&flexidag_system_support, 0, sizeof(flexidag_system_support_t));

  mutex_init(&flexidag_system_support.token_mutex);
  mutex_init(&flexidag_system_support.slot_mutex);
  mutex_lock(&flexidag_system_support.token_mutex);
  mutex_lock(&flexidag_system_support.slot_mutex);

  krn_flexidag_system_reset(1);

  mutex_unlock(&flexidag_system_support.slot_mutex);
  mutex_unlock(&flexidag_system_support.token_mutex);

  flexidag_system_support.pSlotState = &flexidag_slot_state[0];
  flexidag_system_support.flexidag_proc = proc_create_data("flexidag", S_IRUGO,
      get_ambarella_proc_dir(), &flexidag_proc_fops, &flexidag_system_support);
  flexidag_system_support.cvver_proc    = proc_create_data("cvver", S_IRUGO,
      get_ambarella_proc_dir(), &cvver_proc_fops, &flexidag_system_support);

  pr_info("Flexidag system initialized\n");

} /* krn_flexidag_system_init() */

void krn_flexidag_system_reset(int is_init)
{
  uint32_t   loop;

  if (is_init == 0)
  {
    mutex_lock(&flexidag_system_support.token_mutex);
    mutex_lock(&flexidag_system_support.slot_mutex);
  } /* if (is_init == 0) */

  flexidag_system_support.next_token_index  = 0;
  for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++)
  {
    memset(&flexidag_slot_state[loop], 0, sizeof(flexidag_slot_state_t));
    flexidag_system_support.slot_state[loop]    = FLEXIDAG_SLOT_UNUSED;
    flexidag_slot_state[loop].flexidag_slot_id  = loop;
    mutex_init(&flexidag_slot_state[loop].run_mutex);
    sema_init(&flexidag_slot_state[loop].pending_run_semaphore, 1);
    flexidag_slot_state[loop].pending_run_count = 0;
  } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */
  for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++)
  {
    flexidag_system_support.token_state[loop]   = FLEXIDAG_TOKEN_UNUSED;
    flexidag_system_support.token_retcode[loop] = ERRCODE_NONE;
    sema_init(&(flexidag_system_support.token[loop]), 0);
  } /* for (loop = 0; loop < MAX_FLEXIDAG_TOKENS; loop++) */

  if (is_init == 0)
  {
    mutex_unlock(&flexidag_system_support.slot_mutex);
    mutex_unlock(&flexidag_system_support.token_mutex);
  } /* if (is_init == 0) */
} /* krn_flexidag_system_reset() */

void krn_flexidag_system_set_trace_daddr(uint64_t trace_daddr)
{
  if (trace_daddr != 0)
  {
    if (trace_daddr != flexidag_system_support.flexidag_trace_daddr)
    {
      flexidag_system_support.flexidag_trace_daddr  = trace_daddr;
      flexidag_system_support.vpFlexidagTrace       = ambacv_c2v(trace_daddr);
    } /* if (trace_daddr != flexidag_system_support.flexidag_trace_daddr) */
  } /* if (trace_daddr != 0) */
} /* krn_flexidag_system_set_trace_daddr() */

uint64_t krn_flexidag_system_get_trace_daddr(void)
{
  return flexidag_system_support.flexidag_trace_daddr;
} /* krn_flexidag_system_get_trace_daddr() */

static uint32_t krn_flexidag_insert_creation_header(void* buf, uint64_t size, uint64_t daddr)
{
  uint32_t retcode = ERRCODE_NONE;
  membuf_creation_header_t *pmembuf_header = NULL;

  if((buf != NULL) && (size != 0U)) {
    pmembuf_header = (membuf_creation_header_t *)buf;
    pmembuf_header->buffer_daddr = (uint32_t)daddr;
    pmembuf_header->buffer_size = (uint32_t)size;
    pmembuf_header->curr_time = visorc_get_curtime();
    pmembuf_header->fnv1a_checksum = schdrmsg_calc_hash(buf, sizeof(membuf_creation_header_t) - 4U, FNV1A_32_INIT_HASH);
    ambacv_cache_clean(buf, sizeof(membuf_creation_header_t));
  }

  return retcode;
}

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG subprocessing - individual functions (KERNEL/IOCTL) =-------------------------------*/
/*------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : enable() =--------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_enable(uint32_t num_slots)
{
  int retval = 0;

  if (num_slots > FLEXIDAG_MAX_SLOTS)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_enable() : Invalid number of slots (%d, max %d)\n", num_slots, FLEXIDAG_MAX_SLOTS);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (num_slots > FLEXIDAG_MAX_SLOTS) */
  else /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */
  {
    uint32_t  slot_loop;

    retval = 0;

    flexidag_system_support.num_slots_enabled = num_slots;
    for (slot_loop = 0; slot_loop < num_slots; slot_loop++)
    {
      flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNUSED;
    } /* for (slot_loop = 0; slot_loop < num_slots; slot_loop++) */
    for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++)
    {
      flexidag_system_support.slot_state[slot_loop] = FLEXIDAG_SLOT_UNAVAIL;
    } /* for (slot_loop = num_slots; slot_loop < FLEXIDAG_MAX_SLOTS; slot_loop++) */
  } /* if (num_slots <= FLEXIDAG_MAX_SLOTS) */

  return retval;

} /* flexidag_ioctl_handle_enable() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : create() =--------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_create(struct file *f, void **vppHandle, uint32_t *pSlotId)
{
  int retval = 0;

  if (vppHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_create() : Invalid vppHandle (%p)\n", vppHandle);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (vppHandle == NULL) */
  else if (pSlotId == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_create() : Invalid vpSlotId (%p)\n", pSlotId);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (vpSlotId == NULL) */
  else /* if ((vppHandle != NULL) && (vpSlotId != NULL)) */
  {
    uint32_t  token_id;

    token_id = flexidag_find_token();

    if (token_id >= MAX_FLEXIDAG_TOKENS)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_create() : Unable to find token in system (max=%d)\n", flexidag_system_support.num_slots_enabled);
      retval = (int)ERR_FLEXIDAG_NO_AVAILABLE_TOKENS;
    } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
    else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
    {
      armvis_msg_t msgbase;
      errcode_enum_t  visorc_retcode;

      msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_CREATE_REQUEST | 0x8000);
      msgbase.hdr.message_id      = 0;
      msgbase.hdr.message_retcode = ERRCODE_NONE;
      msgbase.msg.flexidag_create_request.flexidag_token = token_id;

      mutex_lock(&flexidag_system_support.token_mutex);

      schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

      visorc_retcode = flexidag_wait_token(token_id, 1, FDPARAM_TOKEN_TIMEOUT_DEFAULT);

      if (visorc_retcode != ERRCODE_NONE)
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_create() : VISORC returns error code (0x%x) ; num_slots_enabled = %d\n",
               visorc_retcode, flexidag_system_support.num_slots_enabled);
        retval = (int)visorc_retcode;
      } /* if (visorc_retcode != ERRCODE_NONE) */
      else /* if (visorc_retcode == ERRCODE_NONE) */
      {
        flexidag_slot_state_t  *pFoundHandle;
        uint32_t  found_slot_id;

        mutex_lock(&flexidag_system_support.slot_mutex);

        found_slot_id = flexidag_system_support.token_start_time[token_id];
        pFoundHandle  = &flexidag_slot_state[found_slot_id];
        flexidag_system_support.slot_state[found_slot_id] = FLEXIDAG_SLOT_USED;

        /*-= Reset slot state to default states =-----------------------------------------*/
        memset(&pFoundHandle->flexidag_slot_id, 0x0, sizeof(flexidag_slot_state_t) - sizeof(struct mutex) - sizeof(struct semaphore));
        pFoundHandle->f = f;
        pFoundHandle->flexidag_slot_id = found_slot_id;
        pFoundHandle->fdparam_interface_cvmsg_num     = FDPARAM_INTERFACE_MSGNUM_DEFAULT;
        pFoundHandle->fdparam_internal_cvmsg_num      = FDPARAM_INTERNAL_MSGNUM_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_perf[0]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_perf[1]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_perf[2]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_perf[3]    = FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_sched[0]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_sched[1]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_sched[2]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_sched[3]   = FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_cvtask[0]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_cvtask[1]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_cvtask[2]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
        pFoundHandle->fdparam_log_size_arm_cvtask[3]  = FDPARAM_LOG_ENTRIES_ARM_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_perf[0]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_perf[1]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_perf[2]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_perf[3]    = FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_sched[0]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_sched[1]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_sched[2]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_sched[3]   = FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_cvtask[0]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_cvtask[1]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_cvtask[2]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
        pFoundHandle->fdparam_log_size_vis_cvtask[3]  = FDPARAM_LOG_ENTRIES_VISORC_DEFAULT;
        pFoundHandle->fdparam_token_timeout           = FDPARAM_TOKEN_TIMEOUT_DEFAULT;
        retval = 0;

        if (retval == 0)
        {
          *vppHandle  = pFoundHandle;
          *pSlotId    = found_slot_id;
        } /* if (retval == 0) : search for available slot */

        mutex_unlock(&flexidag_system_support.slot_mutex);

      } /* if (visorc_retcode == ERRCODE_NONE) : flexidag_wait_token() */
    } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
  } /* if ((vppHandle != NULL) && (vpFlexiRequirements != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_create() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_parameter() =-------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_parameter() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else /* if (vpHandle != NULL) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_parameter() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      retval = 0;
      switch (parameter_id)
      {
      case FDPARAM_INTERFACE_MSGNUM_ID:       pHandle->fdparam_interface_cvmsg_num = parameter_value;       break;
      case FDPARAM_INTERNAL_MSGNUM_ID:        pHandle->fdparam_internal_cvmsg_num = parameter_value;        break;
      case FDPARAM_LOG_ENTRIES_ARMPERF0_ID:   pHandle->fdparam_log_size_arm_perf[0] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_ARMPERF1_ID:   pHandle->fdparam_log_size_arm_perf[1] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_ARMPERF2_ID:   pHandle->fdparam_log_size_arm_perf[2] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_ARMPERF3_ID:   pHandle->fdparam_log_size_arm_perf[3] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_ARMSCH0_ID:    pHandle->fdparam_log_size_arm_sched[0] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_ARMSCH1_ID:    pHandle->fdparam_log_size_arm_sched[1] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_ARMSCH2_ID:    pHandle->fdparam_log_size_arm_sched[2] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_ARMSCH3_ID:    pHandle->fdparam_log_size_arm_sched[3] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_ARM0_ID:       pHandle->fdparam_log_size_arm_cvtask[0] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_ARM1_ID:       pHandle->fdparam_log_size_arm_cvtask[1] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_ARM2_ID:       pHandle->fdparam_log_size_arm_cvtask[2] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_ARM3_ID:       pHandle->fdparam_log_size_arm_cvtask[3] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_VISPERF0_ID:   pHandle->fdparam_log_size_vis_perf[0] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_VISPERF1_ID:   pHandle->fdparam_log_size_vis_perf[1] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_VISPERF2_ID:   pHandle->fdparam_log_size_vis_perf[2] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_VISPERF3_ID:   pHandle->fdparam_log_size_vis_perf[3] = parameter_value;      break;
      case FDPARAM_LOG_ENTRIES_VISSCH0_ID:    pHandle->fdparam_log_size_vis_sched[0] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_VISSCH1_ID:    pHandle->fdparam_log_size_vis_sched[1] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_VISSCH2_ID:    pHandle->fdparam_log_size_vis_sched[2] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_VISSCH3_ID:    pHandle->fdparam_log_size_vis_sched[3] = parameter_value;     break;
      case FDPARAM_LOG_ENTRIES_VISORC0_ID:    pHandle->fdparam_log_size_vis_cvtask[0] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_VISORC1_ID:    pHandle->fdparam_log_size_vis_cvtask[1] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_VISORC2_ID:    pHandle->fdparam_log_size_vis_cvtask[2] = parameter_value;    break;
      case FDPARAM_LOG_ENTRIES_VISORC3_ID:    pHandle->fdparam_log_size_vis_cvtask[3] = parameter_value;    break;
      case FDPARAM_TOKEN_TIMEOUT_ID:          pHandle->fdparam_token_timeout = parameter_value;             break;
      default:
        pr_err("[error] : drv_flexidag_set_parameter() : Invalid parameter_id (%d), value = %d\n", parameter_id, parameter_value);
        retval = (int)ERRCODE_BAD_PARAMETER;
        break;
      } /* switch (parameter_id) */
    } /* if (pHandle == pHandle_expected) */
  } /* if (vpHandle != NULL) */

  return retval;

} /* flexidag_ioctl_handle_set_parameter() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : add_tbar() =------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_add_tbar(void *vpHandle, kernel_memblk_t *pblk_tbar)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_add_tbar() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pblk_tbar == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_add_tbar() : Invalid vpTbar (%p)\n", pblk_tbar);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (vpTbar == NULL) */
  else /* if ((vpHandle != NULL) && (vpTbar != NULL)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_add_tbar() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR)
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_add_tbar(%p) : Additional TBAR exceeds max tbar per flexidag (%d max %d)\n",
               (void *)pHandle, pHandle->num_extra_tbar, FLEXIDAG_MAX_TBAR);
        retval = (int)ERR_FLEXIDAG_CANNOT_ADD_TBAR;
      } /* if (pHandle->num_extra_tbar >= FLEXIDAG_MAX_TBAR) */
      else /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
      {
        uint32_t  index;
        index = pHandle->num_extra_tbar;
        pHandle->vpExtraTbar[index]       = pblk_tbar->pBuffer;
        pHandle->extra_tbar_daddr[index]  = ambacv_p2c(pblk_tbar->buffer_daddr);
        pHandle->num_extra_tbar++;
      } /* if (pHandle->num_extra_tbar < FLEXIDAG_MAX_TBAR) */
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpTbar != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_add_tbar() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : add_sfb() =-------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_add_sfb(void *vpHandle, kernel_memblk_t *pblk_sfb)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_add_sfb() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pblk_sfb == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_add_sfb() : Invalid vpSfb (%p)\n", pblk_sfb);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (vpSfb == NULL) */
  else /* if ((vpHandle != NULL) && (vpSfb != NULL)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_add_sfb() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB)
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_add_sfb(%p) : Additional SFB exceeds max sfb per flexidag (%d max %d)\n",
               (void *)pHandle, pHandle->num_extra_sfb, FLEXIDAG_MAX_SFB);
        retval = (int)ERR_FLEXIDAG_CANNOT_ADD_SFB;
      } /* if (pHandle->num_extra_sfb >= FLEXIDAG_MAX_SFB) */
      else /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
      {
        uint32_t  index;
        index = pHandle->num_extra_sfb;
        pHandle->vpExtraSfb[index]          = pblk_sfb->pBuffer;
        pHandle->extra_sfb_daddr[index]     = ambacv_p2c(pblk_sfb->buffer_daddr);
        {
          uint16_t  scan_loop, num_lines;
          uint32_t  numlines_size;
          uint8_t  *pRecast = NULL;

          scan_loop = 0U;
          numlines_size = (uint32_t) sizeof(sysflow_entry_t);
          num_lines = (uint16_t)(pblk_sfb->buffer_size / numlines_size);
          pRecast = (uint8_t *) ambacv_p2v(pblk_sfb->buffer_daddr);
          if(pRecast != NULL) {
            while (scan_loop < num_lines) {
              if (pRecast[(scan_loop * numlines_size)] == 0x00U) {
                num_lines = scan_loop;
                break;
              } else {
                scan_loop++;
              }
            }
            pHandle->extra_sfb_numlines[index]  = num_lines;
            pr_info("[INFO] : flexidag_ioctl_handle_add_sfb : sfb size %d, num_lines %d \n", pblk_sfb->buffer_size, num_lines);
          }
        }
        pHandle->num_extra_sfb++;
      } /* if (pHandle->num_extra_sfb < FLEXIDAG_MAX_SFB) */
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpSfb != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_add_sfb() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : open() =----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_open(void *vpHandle, kernel_memblk_t *pFlexiBlk, flexidag_memory_requirements_t *pFlexiRequirements)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_open() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_open() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else if (pFlexiRequirements == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_open() : Invalid vpFlexiRequirements (%p)\n", pFlexiRequirements);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (vpFlexiRequirements == NULL) */
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (vpFlexiRequirements != NULL)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_open() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      void *vpFlexiBin;
      uint64_t  flexibin_daddr = 0UL;
      uint64_t  flexibin_size = 0UL;
  
      vpFlexiBin          = pFlexiBlk->pBuffer;
      flexibin_daddr      = ambacv_p2c(pFlexiBlk->buffer_daddr);
      flexibin_size       = pFlexiBlk->buffer_size;
      pHandle->vpFlexiBin     = pFlexiBlk->pBuffer;
      pHandle->flexibin_daddr = flexibin_daddr;
      pHandle->flexibin_size  = pFlexiBlk->buffer_size;
      if ((vpFlexiBin == NULL) || (flexibin_daddr == 0) || (flexibin_size == 0))
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_open() : Invalid vpFlexiBin (%p), flexibin_daddr (0x%llx), or flexibin_size (0x%llx)\n",
               vpFlexiBin, flexibin_daddr, flexibin_size);
        retval = (int)ERRCODE_BAD_PARAMETER;
      } /* if ((vpFlexiBin == NULL) || (flexibin_daddr == 0) || (flexibin_size == 0)) */
      else /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
      {
        uint32_t  token_id;
  
        if (pFlexiBlk->buffer_cacheable != 0)
        {
          ambacv_cache_clean(ambacv_c2v(flexibin_daddr), flexibin_size);
        }
  
        token_id = flexidag_find_token();
  
        if (token_id >= MAX_FLEXIDAG_TOKENS)
        {
          pr_err("[ERROR] : flexidag_ioctl_handle_open(%p) : Unable to find token in system (max=%d)\n", vpHandle, MAX_FLEXIDAG_TOKENS);
          retval = (int)ERR_FLEXIDAG_NO_AVAILABLE_TOKENS;
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        {
          armvis_msg_t msgbase;
          schedmsg_flexidag_open_request_t *pMsg;
          uint32_t   core_loop;
          errcode_enum_t  visorc_retcode;
  
          msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_OPEN_REQUEST | 0x8000);
          msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
          msgbase.hdr.message_retcode = ERRCODE_NONE;
          pMsg                = &msgbase.msg.flexidag_open_request;
  
          pMsg->flexidag_token        = token_id;
          pMsg->flexidag_binary_daddr = flexibin_daddr;
          pMsg->arm_cvmsg_num         = pHandle->fdparam_interface_cvmsg_num;
          pMsg->flexidag_cvmsg_num    = pHandle->fdparam_internal_cvmsg_num;
          for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++)
          {
            pMsg->visorc_perf_log_entry_num[core_loop]    = pHandle->fdparam_log_size_vis_perf[core_loop];
            pMsg->visorc_sched_log_entry_num[core_loop]   = pHandle->fdparam_log_size_vis_sched[core_loop];
            pMsg->visorc_cvtask_log_entry_num[core_loop]  = pHandle->fdparam_log_size_vis_cvtask[core_loop];
          } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) */
          for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++)
          {
            pMsg->arm_perf_log_entry_num[core_loop]       = pHandle->fdparam_log_size_arm_perf[core_loop];
            pMsg->arm_sched_log_entry_num[core_loop]      = pHandle->fdparam_log_size_arm_sched[core_loop];
            pMsg->arm_cvtask_log_entry_num[core_loop]     = pHandle->fdparam_log_size_arm_cvtask[core_loop];
          } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) */
          for (core_loop = 0; core_loop < FLEXIDAG_MAX_TBAR; core_loop++)
          {
            pMsg->additional_tbar_daddr[core_loop]  = pHandle->extra_tbar_daddr[core_loop];
          } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_TBAR; core_loop++) */
          for (core_loop = 0; core_loop < FLEXIDAG_MAX_SFB; core_loop++)
          {
            pMsg->additional_sfb_daddr[core_loop]     = pHandle->extra_sfb_daddr[core_loop];
            pMsg->additional_sfb_numlines[core_loop]  = pHandle->extra_sfb_numlines[core_loop];
          } /* for (core_loop = 0; core_loop < FLEXIDAG_MAX_SFB; core_loop++) */
  
          mutex_lock(&flexidag_system_support.token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);
  
          visorc_retcode = flexidag_wait_token(token_id, 1, pHandle->fdparam_token_timeout);
  
          if (visorc_retcode != ERRCODE_NONE)
          {
            pr_err("[ERROR] : flexidag_ioctl_handle_open() : VISORC returns error code (0x%x)\n", visorc_retcode);
            retval = (int)visorc_retcode;
          } /* if (visorc_retcode != ERRCODE_NONE) */
          else /* if (visorc_retcode == ERRCODE_NONE) */
          {
            memcpy(pFlexiRequirements, &pHandle->memreq, sizeof(flexidag_memory_requirements_t));
          } /* if (visorc_retcode == ERRCODE_NONE) : flexidag_wait_token() */
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
      } /* if ((vpFlexiBin != NULL) && (flexibin_daddr != 0) && (flexibin_size != 0)) */
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (vpFlexiRequirements != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_open() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_state_buffer() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_state_buffer(void *vpHandle, kernel_memblk_t *pFlexiBlk)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_state_buffer() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_state_buffer() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_state_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->vpStateBuffer      = (void *)pFlexiBlk->pBuffer;
      pHandle->state_buffer_daddr = ambacv_p2c(pFlexiBlk->buffer_daddr);
      pHandle->state_buffer_size = pFlexiBlk->buffer_size;
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_set_state_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_temp_buffer() =-----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_temp_buffer(void *vpHandle, kernel_memblk_t *pFlexiBlk)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_temp_buffer() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_temp_buffer() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_temp_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->vpTempBuffer      = (void *)pFlexiBlk->pBuffer;
      pHandle->temp_buffer_daddr = ambacv_p2c(pFlexiBlk->buffer_daddr);
      pHandle->temp_buffer_size = pFlexiBlk->buffer_size;
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL)) */

  return retval;

} /* flexidag_ioctl_handle_set_temp_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : init() =----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_init(void *vpHandle)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_init() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else /* if (vpHandle != NULL) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_init() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0))
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_init() : FD[%2d] : Slot has invalid vpStateBuffer (%p) / state_buffer_daddr = 0x%llx\n",
               pHandle->flexidag_slot_id, pHandle->vpStateBuffer, pHandle->state_buffer_daddr);
        retval = (int)ERR_FLEXIDAG_INVALID_STATE_BUFFER;
      } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
      else /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
      {
        uint32_t  token_id;

        token_id = flexidag_find_token();

        if (token_id >= MAX_FLEXIDAG_TOKENS)
        {
            pr_err("[ERROR] : flexidag_ioctl_handle_init(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
            retval = (int)ERR_FLEXIDAG_NO_AVAILABLE_TOKENS;
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        {
          armvis_msg_t msgbase;
          errcode_enum_t  visorc_retcode;
          schedmsg_flexidag_init_request_t *pMsg;
          uint32_t   output_loop;

          msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_INIT_REQUEST | 0x8000);
          msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
          msgbase.hdr.message_retcode = ERRCODE_NONE;
          pMsg                = &msgbase.msg.flexidag_init_request;

          pMsg->flexidag_token              = token_id;
          pMsg->flexidag_state_buffer_daddr = pHandle->state_buffer_daddr;
          pMsg->flexidag_temp_buffer_daddr  = pHandle->temp_buffer_daddr;
#ifdef ASIL_COMPLIANCE
          if (pHandle->state_buffer_size != 0U) {
            krn_flexidag_insert_creation_header(ambacv_c2v(pHandle->state_buffer_daddr), pHandle->state_buffer_size, pHandle->state_buffer_daddr);
          }
          if (pHandle->temp_buffer_size != 0U) {
            krn_flexidag_insert_creation_header(ambacv_c2v(pHandle->temp_buffer_daddr), pHandle->temp_buffer_size, pHandle->temp_buffer_daddr);
          }
#endif /* ?ASIL_COMPLIANCE */
          for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++)
          {
            pMsg->flexidag_callback_id[output_loop] = 0xFFFFFFFFU;  /* TODO: Link callback id /tokens */
          } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */

          mutex_lock(&flexidag_system_support.token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          visorc_retcode = flexidag_wait_token(token_id, 1, pHandle->fdparam_token_timeout);

          if (visorc_retcode != ERRCODE_NONE)
          {
            pr_err("[ERROR] : flexidag_ioctl_handle_init() : VISORC returns error code (0x%x)\n", visorc_retcode);
            retval = (int)visorc_retcode;
          } /* if (visorc_retcode != ERRCODE_NONE) */
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
      } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
    } /* if (pHandle == pHandle_expected) */
  } /* if (vpHandle != NULL) */

  return retval;

} /* flexidag_ioctl_handle_init() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : prep_run() =------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int32_t  flexidag_ioctl_handle_prep_run(void *vpHandle, uint32_t *pTokenId)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_prep_run() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else /* if (vpHandle != NULL) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_prep_run() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->pending_token_id = flexidag_find_token();
      if (pHandle->pending_token_id < MAX_FLEXIDAG_TOKENS)
      {
        retval = 0;
        mutex_lock(&pHandle->run_mutex);
      } /* if (pHandle->pending_token_id < MAX_FLEXIDAG_TOKENS) */
      else /* if (pHandle->pending_token_id >= MAX_FLEXIDAG_TOKENS) */
      {
        retval = -EFAULT;
      } /* if (pHandle->pending_token_id >= MAX_FLEXIDAG_TOKENS) */
      if (pTokenId != NULL)
      {
        *pTokenId = pHandle->pending_token_id;
      } /* if (pTokenId != NULL) */
    } /* if (pHandle == pHandle_expected) */
  } /* if (vpHandle != NULL) */

  return retval;

} /* flexidag_ioctl_handle_prep_run() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_input_buffer() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_input_buffer(void *vpHandle, uint32_t input_num, kernel_memblk_t *pFlexiBlk)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_input_buffer() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_input_buffer() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else if (input_num >= FLEXIDAG_MAX_INPUTS)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_input_buffer() : Invalid input_num (%d, max %d)\n", input_num, FLEXIDAG_MAX_INPUTS);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (input_num < FLEXIDAG_MAX_INPUTS)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_input_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->vpInputBuffer[input_num]       = (void *)pFlexiBlk->pBuffer;
      pHandle->input_buffer_daddr[input_num]  = ambacv_p2c(pFlexiBlk->buffer_daddr);
      pHandle->input_buffer_size[input_num]   = pFlexiBlk->buffer_size;
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpBuffer != NULL) && (buffer_daddr != 0) && (input_num < FLEXIDAG_MAX_INPUTS)) */

  return retval;

} /* flexidag_ioctl_handle_set_input_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_flexidag_buffer() =-------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, kernel_memblk_t *pFlexiBlk)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_feedback_buffer() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_feedback_buffer() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else if (feedback_num >= FLEXIDAG_MAX_FEEDBACKS)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_feedback_buffer() : Invalid feedback_num (%d, max %d)\n", feedback_num, FLEXIDAG_MAX_FEEDBACKS);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (feedback_num < FLEXIDAG_MAX_FEEDBACKS)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_feedback_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->vpFeedbackBuffer[feedback_num]       = (void *)pFlexiBlk->pBuffer;
      pHandle->feedback_buffer_daddr[feedback_num]  = ambacv_p2c(pFlexiBlk->buffer_daddr);
      pHandle->feedback_buffer_size[feedback_num]   = pFlexiBlk->buffer_size;
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (feedback_num < FLEXIDAG_MAX_FEEDBACKS)) */

  return retval;

} /* flexidag_ioctl_handle_set_feedback_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : set_output_buffer() =---------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_set_output_buffer(void *vpHandle, uint32_t output_num, kernel_memblk_t *pFlexiBlk, uint64_t pOrig_MemBlk)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_output_buffer() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (pFlexiBlk == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_output_buffer() : Invalid vpFlexiBlk (%p)\n", pFlexiBlk);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else if (output_num >= FLEXIDAG_MAX_OUTPUTS)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_set_output_buffer() : Invalid output_num (%d, max %d)\n", output_num, FLEXIDAG_MAX_OUTPUTS);
    retval = (int)ERRCODE_BAD_PARAMETER;
  }
  else /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (output_num < FLEXIDAG_MAX_OUTPUTS)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_set_output_buffer() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      pHandle->vpOutputBuffer[output_num]       = (void *)pFlexiBlk->pBuffer;
      pHandle->output_buffer_daddr[output_num]  = ambacv_p2c(pFlexiBlk->buffer_daddr);
      pHandle->output_buffer_size[output_num]   = pFlexiBlk->buffer_size;
      pHandle->pNextOutputMemblk[output_num]    = pOrig_MemBlk;
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (vpFlexiBlk != NULL) && (feedback_num < FLEXIDAG_MAX_OUTPUTS)) */

  return retval;

} /* flexidag_ioctl_handle_set_output_buffer() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : run() =-----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_run(void *vpHandle, uint32_t is_blocking, flexidag_runinfo_t *pRunInfo, uint32_t *pTokenId)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_run() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else /* if (vpHandle != NULL) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_run() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0))
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_run() : FD[%2d] : Slot has invalid vpStateBuffer (%p) / state_buffer_daddr = 0x%llx\n",
               pHandle->flexidag_slot_id, pHandle->vpStateBuffer, pHandle->state_buffer_daddr);
        retval = (int)ERR_FLEXIDAG_INVALID_STATE_BUFFER;
      } /* if ((pHandle->vpStateBuffer == NULL) || (pHandle->state_buffer_daddr == 0)) */
      else /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
      {
        uint32_t  token_id;

        token_id = pHandle->pending_token_id;

        if (token_id >= MAX_FLEXIDAG_TOKENS)
        {
          pr_err("[ERROR] : flexidag_ioctl_handle_run(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
          retval = (int)ERR_FLEXIDAG_NO_AVAILABLE_TOKENS;
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        {
          armvis_msg_t msgbase;
          schedmsg_flexidag_run_request_t *pMsg;
#ifdef ASIL_COMPLIANCE
          armvis_msg_t PreMsgBase;
          schedmsg_flexidag_prerun_request_t *pPreMsg;
#endif /* ?ASIL_COMPLIANCE */
          errcode_enum_t  visorc_retcode;
          uint32_t   io_loop;

#ifdef ASIL_COMPLIANCE
          /* schedmsg_flexidag_prerun_request_t */
          PreMsgBase.hdr.message_type       = (uint16_t)(SCHEDMSG_FLEXIDAG_PRERUN_REQUEST | 0x8000U);
          PreMsgBase.hdr.message_id         = (uint16_t)(0x0100U + pHandle->flexidag_slot_id);
          PreMsgBase.hdr.message_retcode    = (uint32_t)ERRCODE_NONE;
          pPreMsg                           = &PreMsgBase.msg.flexidag_prerun_request;
          pPreMsg->flexidag_token           = token_id;
#endif /* ?ASIL_COMPLIANCE */

          /* schedmsg_flexidag_run_request_t */
          msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_RUN_REQUEST | 0x8000);
          msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
          msgbase.hdr.message_retcode = ERRCODE_NONE;
          pMsg                = &msgbase.msg.flexidag_run_request;

          pMsg->flexidag_token              = token_id;
          pMsg->flexidag_state_buffer_daddr = pHandle->state_buffer_daddr;
          pMsg->flexidag_temp_buffer_daddr  = pHandle->temp_buffer_daddr;

          for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++)
          {
            pMsg->flexidag_input_buffer_daddr[io_loop]    = pHandle->input_buffer_daddr[io_loop];
            pMsg->flexidag_input_buffer_size[io_loop]     = pHandle->input_buffer_size[io_loop];
#ifdef ASIL_COMPLIANCE
            if(pHandle->input_buffer_size[io_loop] != 0U) {
              pPreMsg->flexidag_input_buffer_csum[io_loop]  = schdrmsg_calc_hash(ambacv_c2v(pHandle->input_buffer_daddr[io_loop]), BUFFER_CHECKSUM_SIZE, FNV1A_32_INIT_HASH);
            } else {
              pPreMsg->flexidag_input_buffer_csum[io_loop]  = 0U;
            }
#endif /* ?ASIL_COMPLIANCE */
          } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_INPUTS; io_loop++) */
          for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++)
          {
            pMsg->flexidag_feedback_buffer_daddr[io_loop] = pHandle->feedback_buffer_daddr[io_loop];
            pMsg->flexidag_feedback_buffer_size[io_loop]  = pHandle->feedback_buffer_size[io_loop];
#ifdef ASIL_COMPLIANCE
            if(pHandle->feedback_buffer_size[io_loop] != 0U) {
              pPreMsg->flexidag_feedback_buffer_csum[io_loop]  = schdrmsg_calc_hash(ambacv_c2v(pHandle->feedback_buffer_daddr[io_loop]), BUFFER_CHECKSUM_SIZE, FNV1A_32_INIT_HASH);
            } else {
              pPreMsg->flexidag_feedback_buffer_csum[io_loop]  = 0U;
            }
#endif /* ?ASIL_COMPLIANCE */
          } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_FEEDBACKS; io_loop++) */
          for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++)
          {
            pMsg->flexidag_output_buffer_daddr[io_loop]   = pHandle->output_buffer_daddr[io_loop];
            pMsg->flexidag_output_buffer_size[io_loop]    = pHandle->output_buffer_size[io_loop];
            flexidag_system_support.pOutputMemBlk[token_id][io_loop] = pHandle->pNextOutputMemblk[io_loop];
#ifdef ASIL_COMPLIANCE
            if(pHandle->output_buffer_size[io_loop] != 0U) {
                krn_flexidag_insert_creation_header(ambacv_c2v(pHandle->output_buffer_daddr[io_loop]), pHandle->output_buffer_size[io_loop], pHandle->output_buffer_daddr[io_loop]);
            }
#endif /* ?ASIL_COMPLIANCE */
          } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */

          pHandle->pending_run_count++;
          down(&pHandle->pending_run_semaphore);
          mutex_unlock(&pHandle->run_mutex);
          if(ambacv_state == 1U) {
            mutex_lock(&flexidag_system_support.token_mutex);
#ifdef ASIL_COMPLIANCE
            schdrmsg_send_armmsg((unsigned long)&PreMsgBase, 1);
#endif /* ?ASIL_COMPLIANCE */
            schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

            visorc_retcode = flexidag_wait_token(token_id, is_blocking, pHandle->fdparam_token_timeout);

            if (visorc_retcode != ERRCODE_NONE)
            {
              pr_err("[ERROR] : flexidag_ioctl_handle_run() : VISORC returns error code (0x%x)\n", visorc_retcode);
              retval = (int)visorc_retcode;
            } /* if (visorc_retcode != ERRCODE_NONE) */
            pHandle->pending_token_id = 0xFFFFFFFFU;

            if ((is_blocking != 0) && (pRunInfo != NULL))
            {
              memset(pRunInfo, 0, sizeof(*pRunInfo));
              pRunInfo->version         = 1;
              pRunInfo->size            = sizeof(*pRunInfo);
              pRunInfo->overall_retcode = flexidag_system_support.token_retcode[token_id];
              pRunInfo->start_time      = flexidag_system_support.token_start_time[token_id];
              pRunInfo->end_time        = flexidag_system_support.token_end_time[token_id];
              pRunInfo->output_not_generated = flexidag_system_support.token_output_not_generated[token_id];
              for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++)
              {
                pRunInfo->output_donetime[io_loop] = flexidag_system_support.token_output_donetime[token_id][io_loop];
              } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */
            } /* if ((is_blocking != 0) && (pRunInfo!= NULL)) */
            else if ((is_blocking == 0) && (pTokenId != NULL))
            {
              *pTokenId = token_id;
            } /* if ((is_blocking == 0) && (pTokenId != NULL)) */
          } else {
            pr_err("[ERROR] : flexidag_ioctl_handle_run() : scheduler have shutdown,ambacv_state (0x%x)\n", ambacv_state);
            retval = ERR_ARMSTATE_CORRUPTED;
          }
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
      } /* if ((pHandle->vpStateBuffer != NULL) && (pHandle->state_buffer_daddr != 0) */
    } /* if (pHandle == pHandle_expected) */
  } /* if (vpHandle != NULL) */

  return retval;

} /* flexidag_ioctl_handle_run() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : run() =-----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_wait(void *vpHandle, flexidag_runinfo_t *pRunInfo, uint32_t token_id)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_wait() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else if (token_id >= MAX_FLEXIDAG_TOKENS)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_wait() : Invalid token_id (%d, max %d)\n", token_id, MAX_FLEXIDAG_TOKENS);
    retval = (int)ERR_FLEXIDAG_INVALID_TOKEN_ID;
  } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
  else /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_wait() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      errcode_enum_t  visorc_retcode;

      mutex_lock(&flexidag_system_support.token_mutex);

      visorc_retcode = flexidag_wait_token(token_id, 2, pHandle->fdparam_token_timeout);

      if (visorc_retcode != ERRCODE_NONE)
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_wait() : VISORC returns error code (0x%x)\n", visorc_retcode);
        retval = (int)visorc_retcode;
      } /* if (visorc_retcode != ERRCODE_NONE) */

      if (pRunInfo != NULL)
      {
        uint32_t  io_loop;

        memset(pRunInfo, 0, sizeof(*pRunInfo));
        pRunInfo->version         = 1;
        pRunInfo->size            = sizeof(*pRunInfo);
        pRunInfo->overall_retcode = flexidag_system_support.token_retcode[token_id];
        pRunInfo->start_time      = flexidag_system_support.token_start_time[token_id];
        pRunInfo->end_time        = flexidag_system_support.token_end_time[token_id];
        pRunInfo->output_not_generated = flexidag_system_support.token_output_not_generated[token_id];
        for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++)
        {
          pRunInfo->output_donetime[io_loop] = flexidag_system_support.token_output_donetime[token_id][io_loop];
        } /* for (io_loop = 0; io_loop < FLEXIDAG_MAX_OUTPUTS; io_loop++) */
      } /* if (pRunInfo != NULL) */
    } /* if (pHandle == pHandle_expected) */
  } /* if ((vpHandle != NULL) && (token_id < MAX_FLEXIDAG_TOKENS)) */

  return retval;

} /* flexidag_ioctl_handle_wait() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : close() =---------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
int flexidag_ioctl_handle_close(void *vpHandle)
{
  int retval = 0;

  if (vpHandle == NULL)
  {
    pr_err("[ERROR] : flexidag_ioctl_handle_close() : Invalid vpHandle (%p)\n", vpHandle);
    retval = (int)ERR_FLEXIDAG_INVALID_VPHANDLE;
  } /* if (vpHandle == NULL) */
  else /* if (vpHandle != NULL) */
  {
    flexidag_slot_state_t *pHandle;
    flexidag_slot_state_t *pHandle_expected;

    pHandle           = (flexidag_slot_state_t *)vpHandle;
    pHandle_expected  = &flexidag_slot_state[pHandle->flexidag_slot_id];
    if (pHandle != pHandle_expected)
    {
      pr_err("[ERROR] : flexidag_ioctl_handle_close() : kernel possibly corrupted : vpHandle(%p) slot_id=%d; expected pHandle(%p) slot_id=%d\n",
             (void *)pHandle, pHandle->flexidag_slot_id, pHandle_expected, pHandle_expected->flexidag_slot_id);
      retval = (int)ERR_FLEXIDAG_SYSTEM_CORRUPTED;
    } /* if (pHandle != pHandle_expected) */
    else /* if (pHandle == pHandle_expected) */
    {
      uint32_t  token_id;
      errcode_enum_t  visorc_retcode;

      token_id = flexidag_find_token();

      if (token_id >= MAX_FLEXIDAG_TOKENS)
      {
        pr_err("[ERROR] : flexidag_ioctl_handle_close(%p) : Unable to find token in system (max=%d)\n", (void *)vpHandle, MAX_FLEXIDAG_TOKENS);
        retval = (int)ERR_FLEXIDAG_NO_AVAILABLE_TOKENS;
      } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
      else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
      {
        armvis_msg_t msgbase;
        schedmsg_flexidag_close_request_t *pMsg;

        retval              = 0;
        msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_CLOSE_REQUEST | 0x8000);
        msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
        msgbase.hdr.message_retcode = ERRCODE_NONE;
        pMsg                = &msgbase.msg.flexidag_close_request;

        pMsg->flexidag_token              = token_id;

        mutex_lock(&flexidag_system_support.token_mutex);

        schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

        visorc_retcode = flexidag_wait_token(token_id, 1, pHandle->fdparam_token_timeout);

        if (visorc_retcode != ERRCODE_NONE)
        {
          pr_err("[ERROR] : flexidag_ioctl_handle_close() : VISORC returns error code (0x%x)\n", visorc_retcode);
          retval = (int)visorc_retcode;
        } /* if (retval != ERRCODE_NONE) */

        /* Regardless of return code, the VISORC system will always force close the slot */

        mutex_lock(&flexidag_system_support.slot_mutex);
        flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
        pHandle->f = NULL;
        mutex_unlock(&flexidag_system_support.slot_mutex);
      } /* if (token_id < MAX_FLEXIDAG_TOKENS) */

    } /* if (pHandle == pHandle_expected) */
  } /* if (vpHandle != NULL) */

  return retval;

} /* flexidag_ioctl_handle_close() */

/*--------------------------------------------------------------------------------------*/
/*-= FLEXIDAG : message handlers() =----------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
void krn_flexidag_close_by_filp(struct file *f)
{
  uint32_t  token_id,i;
  errcode_enum_t  visorc_retcode;
  flexidag_slot_state_t *pHandle;

  if(ambacv_state == 1U) {
    for(i = 0U; i < FLEXIDAG_MAX_SLOTS; i++) {
      pHandle = &flexidag_slot_state[i];
      if(pHandle->f == f) {
        token_id = flexidag_find_token();
        if (token_id >= MAX_FLEXIDAG_TOKENS)
        {
          pr_err("[ERROR] : krn_flexidag_close_by_filp() : Unable to find token in system (max=%d)\n", MAX_FLEXIDAG_TOKENS);
        } /* if (token_id >= MAX_FLEXIDAG_TOKENS) */
        else /* if (token_id < MAX_FLEXIDAG_TOKENS) */
        {
          armvis_msg_t msgbase;
          schedmsg_flexidag_close_request_t *pMsg;

          msgbase.hdr.message_type    = (SCHEDMSG_FLEXIDAG_CLOSE_REQUEST | 0x8000);
          msgbase.hdr.message_id      = 0x0100 + pHandle->flexidag_slot_id;
          msgbase.hdr.message_retcode = ERRCODE_NONE;
          pMsg                = &msgbase.msg.flexidag_close_request;

          pMsg->flexidag_token              = token_id;

          mutex_lock(&flexidag_system_support.token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          visorc_retcode = flexidag_wait_token(token_id, 1, pHandle->fdparam_token_timeout);

          if (visorc_retcode != ERRCODE_NONE)
          {
            pr_err("[ERROR] : krn_flexidag_close_by_filp() : VISORC returns error code (0x%x)\n", visorc_retcode);
          } /* if (retval != ERRCODE_NONE) */

          /* Regardless of return code, the VISORC system will always force close the slot */

          mutex_lock(&flexidag_system_support.slot_mutex);
          flexidag_system_support.slot_state[pHandle->flexidag_slot_id] = FLEXIDAG_SLOT_UNUSED;
          pHandle->f = NULL;
          mutex_unlock(&flexidag_system_support.slot_mutex);
        } /* if (token_id < MAX_FLEXIDAG_TOKENS) */
      }
    }
  }
} /* flexidag_ioctl_handle_close() */

void krn_flexidag_shutdown(void)
{
  uint32_t  i;
  flexidag_slot_state_t *pHandle;

  if(ambacv_state == 0U) {
    /*  release waiting token */
    for(i = 0U; i < MAX_FLEXIDAG_TOKENS; i++) {
      if (flexidag_system_support.token_state[i] == (uint8_t)FLEXIDAG_TOKEN_WAITING){
        (void) flexidag_release_token(i, ERR_ARMSTATE_CORRUPTED, 0U, 0U, 1U);
      }
    }

    /*  release pending run */
    for(i = 0U; i < FLEXIDAG_MAX_SLOTS; i++) {
      pHandle = &flexidag_slot_state[i];
      while(pHandle->pending_run_count > 0) {
        up(&pHandle->pending_run_semaphore);
        mutex_lock(&pHandle->run_mutex);
        pHandle->pending_run_count--;
        mutex_unlock(&pHandle->run_mutex);
      }
    }
  }
}

int krn_flexidag_ioctl_handle_message(armvis_msg_t *pMsg)
{
  int retval = 0;

  if (pMsg == NULL)
  {
    pr_err("[ERROR] : krn_flexidag_ioctl_handle_message() : Invalid pMsg (%p)\n", (void *)pMsg);
    retval = -EINVAL;
  } /* if (pMsg == NULL) */
  else /* if (pMsg != NULL) */
  {
    if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100+FLEXIDAG_MAX_SLOTS)))
    {
      uint32_t  flexidag_slot_id;

      retval            = 0;
      flexidag_slot_id  = pMsg->hdr.message_id - 0x0100;

      switch (pMsg->hdr.message_type)
      {
      case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
        krn_flexidag_system_set_trace_daddr(pMsg->msg.flexidag_create_reply.master_trace_daddr);
        flexidag_release_token(pMsg->msg.flexidag_create_reply.flexidag_token, pMsg->hdr.message_retcode, pMsg->msg.flexidag_create_reply.flexidag_slot_id, 0, 0);
        break;

      case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
        {
          uint32_t output_loop;
          flexidag_slot_state_t *pHandle;

          pHandle = &flexidag_slot_state[flexidag_slot_id];
          pHandle->memreq.flexidag_num_outputs        = pMsg->msg.flexidag_open_reply.flexidag_num_outputs;
          pHandle->memreq.flexidag_temp_buffer_size   = pMsg->msg.flexidag_open_reply.flexidag_memsize_temp;
          pHandle->memreq.flexidag_state_buffer_size  = pMsg->msg.flexidag_open_reply.flexidag_memsize_state;
          for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++)
          {
            pHandle->memreq.flexidag_output_buffer_size[output_loop] = pMsg->msg.flexidag_open_reply.flexidag_output_size[output_loop];
          } /* for (output_loop = 0; output_loop < FLEXIDAG_MAX_OUTPUTS; output_loop++) */
        }
        flexidag_release_token(pMsg->msg.flexidag_open_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0, 0);
        break;

      case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
        {
          flexidag_slot_state_t *pHandle;

          pHandle = &flexidag_slot_state[flexidag_slot_id];
          if (pHandle != NULL)
          {
            uint32_t  max_outstanding;
            max_outstanding = pMsg->msg.flexidag_open_parsed.max_outstanding_runs;
            if ((max_outstanding == 0) || (max_outstanding > FLEXIDAG_FRAME_QUEUE_SIZE))
            {
              max_outstanding = 1;
            }
            sema_init(&pHandle->pending_run_semaphore, max_outstanding);
            pHandle->pending_run_count = 0;
          } /* if (pHandle != NULL) */
        }
        break;

      case SCHEDMSG_FLEXIDAG_INIT_REPLY:
        flexidag_release_token(pMsg->msg.flexidag_init_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0, 0);
        break;

      case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
        pMsg->msg.flexidag_output_reply.vpOutputBlk = (void *)flexidag_system_support.pOutputMemBlk[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num];
        flexidag_system_support.token_output_donetime[pMsg->msg.flexidag_output_reply.flexidag_token][pMsg->msg.flexidag_output_reply.flexidag_output_num] = pMsg->msg.flexidag_output_reply.flexidag_output_donetime;
        ambacv_cache_clean(pMsg, 64);
        ambacv_cache_invalidate(pMsg, 64);
        ambacv_cache_barrier();
        break;

      case SCHEDMSG_FLEXIDAG_RUN_REPLY:
        flexidag_release_token(pMsg->msg.flexidag_run_reply.flexidag_token, pMsg->hdr.message_retcode,
                               pMsg->msg.flexidag_run_reply.start_time, pMsg->msg.flexidag_run_reply.end_time, pMsg->msg.flexidag_run_reply.output_not_generated);
        {
          flexidag_slot_state_t *pHandle;
          pHandle = &flexidag_slot_state[flexidag_slot_id];
          if (pHandle != NULL)
          {
            up(&pHandle->pending_run_semaphore);
            mutex_lock(&pHandle->run_mutex);
            pHandle->pending_run_count--;
            mutex_unlock(&pHandle->run_mutex);
          }
        }
        break;

      case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
        flexidag_release_token(pMsg->msg.flexidag_close_reply.flexidag_token, pMsg->hdr.message_retcode, 0, 0, 0);
        break;

      default:
        pr_err("[ERROR] : krn_flexidag_ioctl_handle_message(%p) : Invalid pMsg->hdr.message_type (%04x)\n", (void *)pMsg, pMsg->hdr.message_type);
        retval = -EINVAL;
        break;
      } /* switch (pMsg->hdr.message_type) */

    } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100+FLEXIDAG_MAX_SLOTS))) */
    else /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS))) */
    {
      pr_err("[error] : krn_flexidag_ioctl_handle_message(%p) : pMsg header.id is not valid for flexidag (0x%04x - should be 0x%04x - 0x%04x\n",
             (void *)pMsg, pMsg->hdr.message_id, 0x0100, 0x0100 + FLEXIDAG_MAX_SLOTS);
      retval = -EINVAL;
    } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100+FLEXIDAG_MAX_SLOTS)) */
  } /* if (pMsg  != NULL) */

  return retval;

} /* krn_flexidag_ioctl_handle_message() */

/*------------------------------------------------------------------------------------------------*/
/*-= FLEXIDAG processing hub (KERNEL/IOCTL) =-----------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int krn_flexidag_ioctl_process(struct file *f, unsigned int cmd, unsigned long arg)
{
  int retval = 0;
  flexidag_ioctl_create_t arg_create;
  flexidag_ioctl_param_t arg_param;
  flexidag_ioctl_memblk_t arg_memblk;
  flexidag_ioctl_open_t arg_open;
  flexidag_ioctl_handle_t arg_handle;
  flexidag_ioctl_pre_run_t arg_pre_run;
  flexidag_ioctl_io_memblk_t arg_io_memblk;
  flexidag_ioctl_run_t arg_run;
  uint32_t arg_value;
  kernel_memblk_t local_memblk;
  uint64_t porig_memblk = 0UL;

  if (arg == 0)
  {
    pr_err("[ERROR] : krn_flexidag_ioctl_process() : Invalid parameter (arg=%p)\n", (void *)arg);
    retval = (int)ERRCODE_BAD_PARAMETER;
  } /* if (arg == NULL) */
  else /* if (arg != NULL) */
  {
    switch (cmd)
    {
      case AMBACV_FLEXIDAG_ENABLE:
        retval = copy_from_user(&arg_value, (void *)arg,sizeof(arg_value));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_enable(arg_value);
        }
        break;
      case AMBACV_FLEXIDAG_CREATE:
        retval = copy_from_user(&arg_create, (void *)arg,sizeof(arg_create));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_create(f, (void **)&arg_create.handle, &arg_create.slot);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_create,sizeof(arg_create));
          }
        }
        break;
      case AMBACV_FLEXIDAG_SET_PARAMETER:
        retval = copy_from_user(&arg_param, (void *)arg,sizeof(arg_param));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_set_parameter((void *)arg_param.handle, arg_param.id, arg_param.value);
        }
        break;
      case AMBACV_FLEXIDAG_ADD_TBAR:
        retval = copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_add_tbar((void *)arg_memblk.handle, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_ADD_SFB:
        retval = copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_add_sfb((void *)arg_memblk.handle, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_OPEN:
        retval = copy_from_user(&arg_open, (void *)arg,sizeof(arg_open));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_open.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_open.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_open.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_open.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_open((void *)arg_open.handle, &local_memblk, &arg_open.mem_req);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_open,sizeof(arg_open));
          }
        }
        break;
      case AMBACV_FLEXIDAG_SET_STATE_BUFFER:
        retval = copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_set_state_buffer((void *)arg_memblk.handle, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_SET_TEMP_BUFFER:
        retval = copy_from_user(&arg_memblk, (void *)arg,sizeof(arg_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_set_temp_buffer((void *)arg_memblk.handle, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_INIT:
        retval = copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_init((void *)arg_handle.handle);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
          }
        }
        break;
      case AMBACV_FLEXIDAG_PREP_RUN:
        retval = copy_from_user(&arg_pre_run, (void *)arg,sizeof(arg_pre_run));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_prep_run((void *)arg_pre_run.handle, &arg_pre_run.token_id);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_pre_run,sizeof(arg_pre_run));
          }
        }
        break;
      case AMBACV_FLEXIDAG_SET_INPUT_BUFFER:
        retval = copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_io_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_io_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_io_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_io_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_set_input_buffer((void *)arg_io_memblk.handle, arg_io_memblk.num, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER:
        retval = copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_io_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_io_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_io_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_io_memblk.mem_blk.buffer_vaddr;
          retval = flexidag_ioctl_handle_set_feedback_buffer((void *)arg_io_memblk.handle, arg_io_memblk.num, &local_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER:
        retval = copy_from_user(&arg_io_memblk, (void *)arg,sizeof(arg_io_memblk));
        if(retval == ERRCODE_NONE) {
          local_memblk.buffer_daddr = arg_io_memblk.mem_blk.buffer_daddr;
          local_memblk.buffer_cacheable = arg_io_memblk.mem_blk.buffer_cacheable;
          local_memblk.buffer_size = arg_io_memblk.mem_blk.buffer_size;
          local_memblk.pBuffer = (char *)arg_io_memblk.mem_blk.buffer_vaddr;
          porig_memblk = arg_io_memblk.orig_memBlk;
          retval = flexidag_ioctl_handle_set_output_buffer((void *)arg_io_memblk.handle, arg_io_memblk.num, &local_memblk, porig_memblk);
        }
        break;
      case AMBACV_FLEXIDAG_RUN:
        retval = copy_from_user(&arg_run, (void *)arg,sizeof(arg_run));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_run((void *)arg_run.handle, arg_run.is_blocking, &arg_run.info, &arg_run.token_id);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_run,sizeof(arg_run));
          }
        }
        break;
      case AMBACV_FLEXIDAG_WAIT_RUN_FINISH:
        retval = copy_from_user(&arg_run, (void *)arg,sizeof(arg_run));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_wait((void *)arg_run.handle, &arg_run.info, arg_run.token_id);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_run,sizeof(arg_run));
          }
        }
        break;
      case AMBACV_FLEXIDAG_CLOSE:
        retval = copy_from_user(&arg_handle, (void *)arg,sizeof(arg_handle));
        if(retval == ERRCODE_NONE) {
          retval = flexidag_ioctl_handle_close((void *)arg_handle.handle);
          if(retval == ERRCODE_NONE) {
            retval = copy_to_user((void *)arg, &arg_handle,sizeof(arg_handle));
          }
        }
        break;
      case AMBACV_FLEXIDAG_HANDLE_MSG:
        //do nothing
        break;
      default:
        pr_err("[ERROR] : krn_flexidag_ioctl_process() : Unknown interface command type (0x%x)\n", cmd);
        retval = -EFAULT;
        break;
    } /* switch (flexi_interface.cmd_type) */
  } /* if (arg != NULL) */

  return retval;

} /* krn_flexidag_ioctl_process() */

void krn_flexidag_system_shutdown(void)
{
  if (flexidag_system_support.flexidag_proc != NULL)
  {
    remove_proc_entry("flexidag", get_ambarella_proc_dir());
    flexidag_system_support.flexidag_proc = NULL;
  } /* if (flexidag_system_support.flexidag_proc != NULL) */
  if (flexidag_system_support.cvver_proc != NULL)
  {
    remove_proc_entry("cvver", get_ambarella_proc_dir());
    flexidag_system_support.cvver_proc = NULL;
  } /* if (flexidag_system_support.cvver_proc != NULL) */
  pr_info("Flexidag system shutdown\n");
} /* krn_flexidag_system_shutdown() */

