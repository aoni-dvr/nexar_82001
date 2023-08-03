#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"
#include "cavalry.h"
#include "cavalry_mem.h"    /* For cavalry/MEM interface */
#include "cavalry_log.h"    /* For cavalry/Log interface */
#include "cavalry_cma.h"    /* For cavalry/CMA interface */
#include "cavalry_print.h"  /* For cavalry/Print interface */
#include "cavalry_misc.h"   /* for cavalry/clock interfaces */
#include "schdr_api.h"      /* for cavalry_trace_t structure */
#include <linux/io.h>
#include <linux/fs.h>       /* Needed for filp */

MODULE_AUTHOR("Clifton Kayano");
MODULE_LICENSE("GPL");

#define VISORC_EARLY_QUIT_SYNC	(126)

static void __iomem *cavalry_io_base;

/*------------------------------------------------------------------------------------------------*/
/*-= CAVALRY support from original Cavalry Driver =-----------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
static int cavalry_check_dags(struct cavalry_run_dags *run_dags)
{
	int i, j;
	struct cavalry_dag_desc *dag_desc = NULL;

	if (run_dags->dag_cnt == 0 || run_dags->dag_cnt > MAX_DAG_CNT) {
		prt_err("Invalid dag_cnt %u\n", run_dags->dag_cnt);
		return -1;
	}

	for (i = 0; i < run_dags->dag_cnt; i++) {
		dag_desc = &run_dags->dag_desc[i];

		if (!dag_desc->dvi_img_size) {
			prt_err("dvi img size can't be zero.\n");
			return -1;
		}
		if (!dag_desc->dvi_img_vaddr) {
			prt_err("dvi img vaddr can't be zero.\n");
			return -1;
		}
		if (!dag_desc->dvi_dag_vaddr) {
			prt_err("dvi dag vaddr can't be zero.\n");
			return -1;
		}

		if (dag_desc->port_cnt > MAX_PORT_CNT) {
			prt_err("DAG [%u] port cnt [%u] exceed Max [%u].\n", i, dag_desc->port_cnt,
				MAX_PORT_CNT);
			return -1;
		}

		if (dag_desc->poke_cnt > MAX_POKE_CNT) {
			prt_err("DAG [%u] poke cnt [%u] exceed Max [%u].\n", i, dag_desc->poke_cnt,
				MAX_POKE_CNT);
			return -1;
		}

		for (j = 0; j < dag_desc->port_cnt; j++) {
			if (dag_desc->port_desc[j].port_bsize == 0) {
				prt_err("size of Port [%d] in Dag [%d] can't be zero.\n", j, i);
				return -1;
			}
		}
	}

	return 0;
}

/*------------------------------------------------------------------------------------------------*/
/*-= CAVALRY specialized from for superdag integration =------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY system init (LINUX) =-----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
void cavalry_system_init(struct ambarella_cavalry *cavalry)
{
	uint32_t   loop;

	cavalry_io_base  = NULL;

	/*-= Initialize cavalry slot system =-----------------------------------------------*/
	mutex_init(&cavalry->slot_mutex);
	mutex_lock(&cavalry->slot_mutex);
	sema_init(&cavalry->slot_semaphore, 0); /* No slots available at init time */
	cavalry->num_slots_enabled  = 0;
	cavalry->next_slot_id       = 0;
	for (loop = 0; loop < MAX_CAVALRY_SLOTS; loop++)
	{
		cavalry->slot_state[loop]    = CAV_SLOT_AVAILABLE;
		cavalry->slot_daddr[loop]    = 0;
	} /* for (loop = 0; loop < MAX_CAVALRY_SLOTS; loop++) */
	mutex_unlock(&cavalry->slot_mutex);

	/*-= Initialize cavalry token system =----------------------------------------------*/
	mutex_init(&cavalry->token_mutex);
	mutex_lock(&cavalry->token_mutex);
	sema_init(&cavalry->token_semaphore, MAX_CAVALRY_TOKENS);
	cavalry->next_token_id = 0;
	for (loop = 0; loop < MAX_CAVALRY_TOKENS; loop++)
	{
		cavalry->cavtoken_state[loop]   = CAV_SLOT_AVAILABLE;
		cavalry->cavtoken_slotid[loop]  = CAV_INVALID_SLOTID;
		cavalry->cavtoken_retcode[loop] = 0;
		sema_init(&(cavalry->cavtoken[loop]), 0);
	} /* for (loop = 0; loop < MAX_CAVALRY_TOKENS; loop++) */
	mutex_unlock(&cavalry->token_mutex);

	prt_info("Cavalry system initialized\n");

} /* cavalry_system_init() */

void cavalry_system_shutdown(struct ambarella_cavalry *cavalry)
{
	if (cavalry_io_base != NULL)
	{
		iounmap(cavalry_io_base);
		cavalry_io_base = NULL;
	} /* if (cavalry_io_base != NULL) */
	if (atomic_read(&cavalry->is_vp_started) != 0) {
		atomic_set(&cavalry->is_vp_started, 0);
	}
	cavalry_misc_exit(cavalry);
	cavalry_log_exit(cavalry);
	prt_info("Cavalry system shutdown\n");
} /* cavalry_system_shutdown() */

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY/SUPERDAG driver API (LINUX) =---------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
int32_t ioctl_cavalry_enable(struct ambarella_cavalry *cavalry, uint32_t num_slots)
{
	int retval;

	if (num_slots > MAX_CAVALRY_SLOTS)
	{
		cavalry->num_slots_enabled = 0;
		prt_err("[error] : ioctl_cavalry_enable() : Invalid number of slots (%d, max %d)\n", (uint32_t)num_slots, (uint32_t)MAX_CAVALRY_SLOTS);
		retval = -ECAVALRY_INVALID_SLOT_ID;
	} /* if (num_slots > MAX_CAVALRY_SLOTS) */
	else /* if (num_slots <= MAX_CAVALRY_SLOTS) */
	{
		cavalry->num_slots_enabled = num_slots;
		sema_init(&cavalry->slot_semaphore, num_slots);
		prt_info("ioctl_cavalry_enable() : Cavalry enabled for %d slots\n", cavalry->num_slots_enabled);
		retval = (CAVALRY_VERSION << 8);
	} /* if (num_slots <= MAX_CAVALRY_SLOTS) */

	if (cavalry_io_base != NULL)
	{
		prt_info("unmapping existing cavalry base (%p)\n", cavalry_io_base);
		iounmap(cavalry_io_base);
		cavalry_io_base = NULL;
	} /* if (cavalry_io_base != NULL) */

	return retval;

} /* ioctl_cavalry_enable() */

int  ioctl_cavalry_associate_buf(struct ambarella_cavalry *cavalry, uint32_t cavalry_curr_daddr)
{
	uint32_t loop;

	for (loop = 0; loop < cavalry->num_slots_enabled; loop++)
	{
		cavalry->slot_daddr[loop]  = cavalry_curr_daddr;
		cavalry_curr_daddr += CAVALRY_SLOT_SIZE;
	} /* for (loop = 0; loop < cavalry->num_slots_enabled; loop++) */
	for (loop = cavalry->num_slots_enabled; loop < MAX_CAVALRY_SLOTS; loop++)
	{
		cavalry->slot_daddr[loop]  = 0;
	} /* for (loop = cavalry->num_slots_enabled; loop < MAX_CAVALRY_SLOTS; loop++) */

	return 0;

} /* ioctl_cavalry_associate_buf() */

extern ambacv_mem_t ambacv_global_mem;
extern int enable_cma;
extern uint32_t ucode_log_level;

int  ioctl_cavalry_set_memory_block(struct ambarella_cavalry *cavalry, void __user *arg)
{
	int retval;

	if (arg == 0)
	{
		prt_err("[error] : ioctl_cavalry_set_memory_block() : Invalid message provided (%p)\n", (void *)arg);
		retval = -EINVAL;
	} /* if (arg == 0) */
	else /* if (arg != 0) */
	{
		armvis_msg_t rxmsg;

		retval = copy_from_user((void *)&rxmsg, (void *)arg, sizeof(armvis_msg_hdr_t) + sizeof(schedmsg_cvscheduler_memory_report_t));

		if (retval == 0)
		{
			if (rxmsg.hdr.message_type == SCHEDMSG_CVSCHEDULER_REPORT_MEMORY)
			{
				schedmsg_cvscheduler_memory_report_t *pMsg;

				pMsg = &rxmsg.msg.memory_report;

				if (enable_cma == 0)
				{
					uint32_t  cavalry_start;
					uint32_t  cavalry_end;

					cavalry_start = pMsg->orc_uncached_baseaddr + pMsg->orc_uncached_size;
					cavalry_end   = pMsg->armblock_baseaddr;

					cavalry_start = PAGE_ALIGN(cavalry_start);
					cavalry_end   = cavalry_end & PAGE_MASK;

					cavalry->cavalry_memblock_base = cavalry_start;
					cavalry->cavalry_memblock_size = cavalry_end - cavalry_start;

					/*-= Partition memory block as per original cavalry driver =--------------------------*/
					{
						uint32_t  base;
						uint32_t  size;
						uint8_t  *vbase;
						pgprot_t prot;

						base    = cavalry_start;
						size    = cavalry_end - cavalry_start;
						prot    = __pgprot(PROT_NORMAL_NC);
						vbase   = __ioremap(ambacv_c2p(base), size, prot);
						cavalry_io_base = vbase;

						ambacv_global_mem.cavalry_region.base  = base;
						ambacv_global_mem.cavalry_region.size  = size -  CAVALRY_MEM_LOG_SIZE;

						cavalry->cma_private.phys     = base;
						cavalry->cma_private.size     = CAVALRY_MEM_USER_OFFSET;
						cavalry->cma_private.virt     = vbase;

						/* Move uncached_logblock to end of cma_private block */
						cavalry->cavalry_uncached_logblock.phys   = base  + ambacv_global_mem.cavalry_region.size;
						cavalry->cavalry_uncached_logblock.size   = CAVALRY_MEM_LOG_SIZE;
						cavalry->cavalry_uncached_logblock.virt   = vbase + ambacv_global_mem.cavalry_region.size;
					}
				} /* if (enable_cma == 0) */
				else /* if (enable_cma != 0) */
				{
					ambacv_global_mem.cavalry_region.base  = cavalry->cavalry_memblock_base + cavalry->cma_private.size;
					ambacv_global_mem.cavalry_region.size  = cavalry->cavalry_memblock_size - cavalry->cma_private.size;
					cavalry_io_base = NULL;
				} /* if (enable_cma != 0) */
#if 0
				prt_info("ioctl_cavalry_set_memory_block() : CAVALRY block set up : 0x%08x -> 0x%08x (size=%d) @ %p\n",
				       (uint32_t)(ambacv_global_mem.cavalry_region.base),
				       (uint32_t)(ambacv_global_mem.cavalry_region.base + ambacv_global_mem.cavalry_region.size),
				       (uint32_t)ambacv_global_mem.cavalry_region.size, cavalry->cma_private.virt);
#endif

/*-= From cavalry_dev.c : cavalry_of_init() =---------------------------------------vvv-*/
				cavalry_mem_init(cavalry, cavalry->cavalry_memblock_size);

				/* REPATCH IN LOG LOCATION */
				cavalry->cavalry_mem_info[CAVALRY_MEM_LOG] = cavalry->cavalry_uncached_logblock;

				/* REPATCH IN HOTLINK SLOT LOCATION */
				if (enable_cma == 0)
				{
					uint8_t  *pBase;
					uint32_t  phy_base;
					pgprot_t prot;

					prot      = __pgprot(PROT_NORMAL_NC);
					phy_base  = ambacv_global_mem.all.base + CAVALRY_MEM_UCODE_OFFSET;
					pBase     = (uint8_t*)__ioremap(ambacv_c2p(phy_base) , (CAVALRY_MEM_USER_OFFSET - CAVALRY_MEM_UCODE_OFFSET), prot);

					cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys         = phy_base;
					cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt         = pBase;
					cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].size         = CAVALRY_MEM_HOTLINK_OFFSET - CAVALRY_MEM_UCODE_OFFSET;
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].phys = phy_base + (CAVALRY_MEM_HOTLINK_SLOT0_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].virt = pBase    + (CAVALRY_MEM_HOTLINK_SLOT0_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT0].size = CAVALRY_MEM_HOTLINK_SLOT_SIZE;
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].phys = phy_base + (CAVALRY_MEM_HOTLINK_SLOT1_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].virt = pBase    + (CAVALRY_MEM_HOTLINK_SLOT1_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT1].size = CAVALRY_MEM_HOTLINK_SLOT_SIZE;
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].phys = phy_base + (CAVALRY_MEM_HOTLINK_SLOT2_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].virt = pBase    + (CAVALRY_MEM_HOTLINK_SLOT2_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT2].size = CAVALRY_MEM_HOTLINK_SLOT_SIZE;
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].phys = phy_base + (CAVALRY_MEM_HOTLINK_SLOT3_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].virt = pBase    + (CAVALRY_MEM_HOTLINK_SLOT3_OFFSET - CAVALRY_MEM_UCODE_OFFSET);
					cavalry->cavalry_mem_info[CAVALRY_MEM_HOTLINK_SLOT3].size = CAVALRY_MEM_HOTLINK_SLOT_SIZE;
				} /* if (enable_cma != 0) */

				if (ucode_log_level > CAVALRY_LOG_LEVEL_LAST) {
					prt_err("Invalid Ucode log level %u, valid range [%u-%u].\n",
					        ucode_log_level, CAVALRY_LOG_LEVEL_FIRST, CAVALRY_LOG_LEVEL_LAST);
					return -1;
				}
				cavalry->log_level = ucode_log_level;

/*-= From cavalry_dev.c : cavalry_of_init() =---------------------------------------^^^-*/

				/* Send setup message */
				{
					armvis_msg_t msgbase;
					schedmsg_cavalry_setup_t *pSetupMsg;

					msgbase.hdr.message_type        = SCHEDMSG_CAVALRY_SETUP;
                    msgbase.hdr.message_id          = 0;
                    msgbase.hdr.message_retcode     = ERRCODE_NONE;
					pSetupMsg                       = &msgbase.msg.cavalry_setup;
					pSetupMsg->all_base_daddr       = cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys;
					pSetupMsg->all_size             = cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].size;
					pSetupMsg->cmd_base_daddr       = cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].phys;
					pSetupMsg->cmd_size             = cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].size;
					pSetupMsg->msg_base_daddr       = cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].phys;
					pSetupMsg->msg_size             = cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].size;
					pSetupMsg->log_base_daddr       = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].phys;
					pSetupMsg->log_size             = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size;
					pSetupMsg->ucode_base_daddr     = cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys;
					pSetupMsg->ucode_size           = cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].size;
					pSetupMsg->user_base_daddr      = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys;
					pSetupMsg->user_size            = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size;
					pSetupMsg->hotlink_slot0_offset = CAVALRY_MEM_HOTLINK_SLOT0_OFFSET;
					pSetupMsg->hotlink_slot1_offset = CAVALRY_MEM_HOTLINK_SLOT1_OFFSET;
					pSetupMsg->hotlink_slot2_offset = CAVALRY_MEM_HOTLINK_SLOT2_OFFSET;
					pSetupMsg->hotlink_slot3_offset = CAVALRY_MEM_HOTLINK_SLOT3_OFFSET;
					pSetupMsg->log_level            = cavalry->log_level;
					schdrmsg_send_armmsg((unsigned long)&msgbase, 1);
				}
				if (cavalry_misc_init(cavalry) < 0)
				{
					prt_err("failed to initialize misc system.\n");
				}
				else if (cavalry_log_init(cavalry) < 0)
				{
					prt_err("Fail to initialize log system.\n");
				} /* if (cavalry_log_init(cavalry) < 0) */
				else /* if (cavalry_log_init(cavalry) >= 0) */
				{
					/* from cavalry/visorc_init() function */
					init_waitqueue_head(&cavalry->cavalry_vp_wq);
					init_waitqueue_head(&cavalry->cavalry_fex_wq);
					init_waitqueue_head(&cavalry->cavalry_fma_wq);
					sema_init(&cavalry->cavalry_vp_sem, 1);
					sema_init(&cavalry->cavalry_fex_sem, 1);
					sema_init(&cavalry->cavalry_fma_sem, 1);
					mutex_init(&cavalry->cavalry_early_quit_mutex);
					spin_lock_init(&cavalry->cavalry_cmd_msg_vp_lock);
					spin_lock_init(&cavalry->cavalry_cmd_msg_fex_lock);
					spin_lock_init(&cavalry->cavalry_cmd_msg_fma_lock);
					if (atomic_read(&cavalry->is_vp_started) == 0) {
						atomic_set(&cavalry->is_vp_started, 1);
					}
					if (atomic_read(&cavalry->is_vp_done) == 0) {
						atomic_set(&cavalry->is_vp_done, 1);
					}
				} /* if (cavalry_log_init(cavalry) >= 0) */
			} /* if (rxmsg.hdr.message_type == SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
			else /* if (rxmsg.hdr.message_type != SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
			{
				prt_err("[error] : ioctl_cavalry_set_memory_block() : Invalid message provided (%p) - type = 0x%04x (should be 0x%04x)- Ignoring\n",
					(void *)arg, rxmsg.hdr.message_type, SCHEDMSG_CVSCHEDULER_REPORT_MEMORY);
				retval = -EINVAL;
			} /* if (rxmsg.hdr.message_type != SCHEDMSG_CVSCHEDULER_REPORT_MEMORY) */
		} /* if (retval == 0) */
		else /* if (retval != 0) */
		{
			prt_err("[error] : ioctl_cavalry_set_memory_block() : Unable to copy message from user space (%p)\n", (void *)arg);
			retval = -EFAULT;
		} /* if (retval != 0) */
	} /* if (arg != 0) */

	return retval;

} /* ioctl_cavalry_set_memory_block() */

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY slot management (LINUX) =-------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
uint32_t  cavalry_find_slot(struct ambarella_cavalry *cavalry, uint32_t *pRunDagDaddr)
{
	uint32_t  slot_found;
	uint32_t  slot_num;
	uint32_t  slot_id;
	uint32_t  retcode;
	uint32_t  loop;

	down(&cavalry->slot_semaphore);
	mutex_lock(&cavalry->slot_mutex);
	slot_found  = 0;
	slot_num    = 0;
	retcode     = 0;
	loop        = 0;

	slot_id     = cavalry->next_slot_id;
	while ((slot_found == 0) && (loop < cavalry->num_slots_enabled))
	{
		if (cavalry->slot_state[slot_id] == CAV_SLOT_AVAILABLE)
		{
			slot_found  = 1;
			slot_num    = slot_id;
			cavalry->slot_state[slot_id]   = CAV_SLOT_USED;
		} /* if (cavalry->slot_state[slot_id] == CAV_SLOT_AVAILABLE) */
		else /* if (cavalry->slot_state[slot_id] != CAV_SLOT_AVAILABLE) */
		{
			loop++;
			slot_id++;
			if (slot_id >= cavalry->num_slots_enabled) slot_id = 0;
		} /* if (cavalry->slot_state[slot_id] != CAV_SLOT_AVAILABLE) */
	} /* while ((token_found == 0) && (loop < MAX_CAVALRY_TOKENS)) */
	mutex_unlock(&cavalry->slot_mutex);

	if (slot_found == 0)
	{
		/* ERROR */
		retcode = 0xFFFFFFFF;
	} /* if (slot_found == 0) */
	else /* if (slot_found != 0) */
	{
		retcode       = slot_id;
		*pRunDagDaddr = cavalry->slot_daddr[slot_id];
		cavalry->next_slot_id = (slot_id + 1) % (cavalry->num_slots_enabled);
	} /* if (slot_found != 0) */

	return retcode;

} /* cavalry_find_slot() */

uint32_t  cavalry_finish_slot(struct ambarella_cavalry *cavalry, uint32_t slot_id)
{
	mutex_lock(&cavalry->slot_mutex);
	cavalry->slot_state[slot_id] = CAV_SLOT_AVAILABLE;
	up(&cavalry->slot_semaphore);
	mutex_unlock(&cavalry->slot_mutex);
	return 0;
} /* cavalry_finish_slot() */

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY token management (LINUX) =------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

/* Note : this mimics the slot system, but it uses a more generic system to */
/* handle replies from the VISORC.  Eventually the cavalry slot system will */
/* be merged into this, using the "slot" as a definition for memory used by */
/* the CAVALRY/VP commands.                                                 */

static uint32_t cavalry_find_token(struct ambarella_cavalry *pCavalry, uint32_t cavalry_slot_id)
{
  uint32_t  retcode;

  if (pCavalry == NULL)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (pCavalry == NULL) */
  else /* if (pCavalry != NULL) */
  {
    uint32_t  token_found;
    uint32_t  token_num;
    uint32_t  token_id;
    uint32_t  loop;

    down(&pCavalry->token_semaphore);
    mutex_lock(&pCavalry->token_mutex);

    token_found = 0;
    token_num   = 0;
    token_id    = pCavalry->next_token_id;
    retcode     = 0;
    loop        = 0;

    while ((token_found == 0) && (loop < MAX_CAVALRY_TOKENS))
    {
      if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_AVAILABLE)
      {
        token_found = 1;
        token_num   = token_id;
        pCavalry->cavtoken_state[token_id]    = CAV_TOKEN_USED;
        pCavalry->cavtoken_slotid[token_id]   = cavalry_slot_id;
        pCavalry->cavtoken_retcode[token_id]  = 0;
      } /* if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_AVAILABLE) */
      else /* if (pCavalry->cavtoken_state[token_id] != CAV_TOKEN_AVAILABLE) */
      {
        loop++;
        token_id++;
        if (token_id >= MAX_CAVALRY_TOKENS)
        {
          token_id = 0;
        } /* if (token_id >= MAX_CAVALRY_TOKENS) */
      } /* if (pCavalry->cavtoken_state[token_id] != CAV_TOKEN_AVAILABLE) */
    } /* while ((token_found == 0) && (loop < MAX_CAVALRY_TOKENS)) */

    mutex_unlock(&pCavalry->token_mutex);

    if (token_found == 0)
    {
      retcode = 0xFFFFFFFF;
    } /* if (token_found == 0) */
    else /* if (token_found != 0)*/
    {
      uint32_t  next_token_id;

      retcode = token_id;
      next_token_id = token_id + 1;
      if (next_token_id >= MAX_CAVALRY_TOKENS)
      {
        next_token_id = 0;
      }
      pCavalry->next_token_id = next_token_id;
    } /* if (token_found != 0)*/
  } /* if (pCavalry != NULL) */

  return retcode;

} /* cavalry_find_slot() */

static uint32_t cavalry_wait_token(struct ambarella_cavalry *pCavalry, uint32_t token_id)
{
  uint32_t  retcode;

  if (pCavalry == NULL)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (pCavalry == NULL) */
  else if (token_id >= MAX_CAVALRY_TOKENS)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (token_id >= MAX_CAVALRY_TOKENS) */
  else /* if ((pCavalry != NULL) && (token_id < MAX_CAVALRY_TOKENS)) */
  {
    uint32_t  wait_valid;
    retcode       = 0;

    if (pCavalry->cavtoken_state[token_id] != CAV_TOKEN_USED)
    {
      // ERROR
      wait_valid = 0;
      retcode = 0xFFFFFFFFU;
    } /* if (pCavalry->cavtoken_state[token_id] != CAV_TOKEN_USED) */
    else /* if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_USED) */
    {
      wait_valid = 1;
      pCavalry->cavtoken_state[token_id] = CAV_TOKEN_WAITING;
    } /* if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_USED) */
    mutex_unlock(&pCavalry->token_mutex);

    if (wait_valid != 0)
    {
      int ret;
      ret = down_timeout(&pCavalry->cavtoken[token_id], 5000); /* 5 second timeout, TODO process timeout case */

      mutex_lock(&pCavalry->token_mutex);
      if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_FINISHED)
      {
        retcode = pCavalry->cavtoken_retcode[token_id];
      } /* if (pCavalry->slot_state[slot_id] == CAV_TOKEN_FINISHED) */
      else /* if (pCavalry->slot_state[slot_id] != CAV_TOKEN_FINISHED) */
      {
        /* ERROR */
        retcode = 0xFFFFFFFFU;
      } /* if (pCavalry->slot_state[slot_id] != CAV_TOKEN_FINISHED) */
      mutex_unlock(&pCavalry->token_mutex);
    } /* if (wait_valid != 0) */
  } /* if ((pCavalry != NULL) && (token_id < MAX_CAVALRY_TOKENS)) */

  return retcode;

} /* cavalry_wait_token() */

static uint32_t cavalry_finish_token(struct ambarella_cavalry *pCavalry, uint32_t token_id)
{
  uint32_t  retcode;

  if (pCavalry == NULL)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (pCavalry == NULL) */
  else if (token_id >= MAX_CAVALRY_TOKENS)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (token_id >= MAX_CAVALRY_TOKENS) */
  else /* if ((pCavalry != NULL) && (token_id < MAX_CAVALRY_TOKENS)) */
  {
    retcode = 0;
    mutex_lock(&pCavalry->token_mutex);
    pCavalry->cavtoken_state[token_id] = CAV_TOKEN_AVAILABLE;
    up(&pCavalry->token_semaphore);
    mutex_unlock(&pCavalry->token_mutex);
  } /* if ((pCavalry != NULL) && (token_id < MAX_CAVALRY_TOKENS)) */

  return retcode;

} /* cavalry_finish_token() */

uint32_t  cavalry_release_token(struct ambarella_cavalry *pCavalry, uint32_t token_id, uint32_t function_retcode, armvis_msg_t *pMsg)
{
  uint32_t  retcode;

  if (pCavalry == NULL)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (pCavalry == NULL) */
  else if (token_id >= MAX_CAVALRY_TOKENS)
  {
    // ERROR
    retcode = 0xFFFFFFFFU;
  } /* if (token_id >= MAX_CAVALRY_TOKENS) */
  else /* if ((pCavalry != NULL) && if (token_id < MAX_CAVALRY_TOKENS)) */
  {
    uint32_t  default_others;
    uint32_t  release_valid;

    default_others  = 1;
    retcode         = 0;
    mutex_lock(&pCavalry->token_mutex);
    if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_USED)
    {
      /* Error happened before the actual message is sent to the VISORC - handle this properly */
      release_valid = 2;
    } /* if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_USED) */
    else if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_WAITING)
    {
      /* Normal processing */
      release_valid = 1;
    } /* if (pCavalry->cavtoken_state[token_id] == CAV_TOKEN_WAITING) */
    else /* if (pCavalry->cavtoken_state[token_id] != (CAV_TOKEN_USED, CAV_TOKEN_WAITING)) */
    {
      /* ERROR */
      release_valid = 0;
      retcode       = 0xFFFFFFFFU;
    } /* if (pCavalry->cavtoken_state[token_id] != (CAV_TOKEN_USED, CAV_TOKEN_WAITING)) */
    mutex_unlock(&pCavalry->token_mutex);

    if (release_valid == 1)
    {
      mutex_lock(&pCavalry->token_mutex);
      pCavalry->cavtoken_state[token_id]    = CAV_TOKEN_FINISHED;
      pCavalry->cavtoken_retcode[token_id]  = function_retcode;
      if (pMsg != NULL)
      {
        switch (pMsg->hdr.message_type)
        {
        case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
          /* Do nothing, processing already inherent in ioctl */
          break;

        case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
          memcpy(&pCavalry->cavtoken_replymsg[token_id][0], &pMsg->msg.cavalry_hotlink_run_reply,
                 sizeof(schedmsg_cavalry_hl_run_reply_t));
          break;

        case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
          memcpy(&pCavalry->cavtoken_replymsg[token_id][0], &pMsg->msg.cavalry_fex_query_reply.msg,
                 sizeof(pMsg->msg.cavalry_fex_query_reply.msg) - sizeof(pMsg->msg.cavalry_fex_query_reply.msg.reserved));
          break;

        case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
          memcpy(&pCavalry->cavtoken_replymsg[token_id][0], &pMsg->msg.cavalry_fex_run_reply.msg,
                 sizeof(pMsg->msg.cavalry_fex_run_reply.msg) - sizeof(pMsg->msg.cavalry_fex_run_reply.msg.reserved));
          break;

        case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
          memcpy(&pCavalry->cavtoken_replymsg[token_id][0], &pMsg->msg.cavalry_fma_query_reply.msg,
                 sizeof(pMsg->msg.cavalry_fma_query_reply.msg) - sizeof(pMsg->msg.cavalry_fma_query_reply.msg.reserved));
          break;

        case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
          memcpy(&pCavalry->cavtoken_replymsg[token_id][0], &pMsg->msg.cavalry_fma_run_reply.msg,
                 sizeof(pMsg->msg.cavalry_fma_run_reply.msg) - sizeof(pMsg->msg.cavalry_fma_run_reply.msg.reserved));
          break;

        default:
          break;
        } /* switch (pMsg->hdr.message_type) */
      } /* if (pMsg != NULL) */
      mutex_unlock(&pCavalry->token_mutex);
      up(&pCavalry->cavtoken[token_id]);
    } /* if (release_valid == 1) */
    else if (release_valid == 2)
    {
      mutex_lock(&pCavalry->token_mutex);
      pCavalry->cavtoken_state[token_id]    = CAV_TOKEN_AVAILABLE;
      pCavalry->cavtoken_retcode[token_id]  = function_retcode;
      mutex_unlock(&pCavalry->token_mutex);
      cavalry_finish_token(pCavalry, token_id);
    } /* if (release_valid == 2) */
    else /* if (release_valid == [1..2]) */
    {
      ; /* Do nothing, MISRA requirement for this statement */
    } /* if (release_valid == [1..2]) */
  } /* if ((pCavalry != NULL) && if (token_id < MAX_CAVALRY_TOKENS)) */

  return retcode;

} /* cavalry_release_token() */

static void cavalry_initialize_traceinfo(struct ambarella_cavalry *pCavalry)
{
  if (pCavalry != NULL)
  {
    if (pCavalry->vpCavalryTrace != NULL)
    {
      cavalry_trace_t *pCavalryTrace;
      uint32_t loop;

      pCavalryTrace = (cavalry_trace_t *)pCavalry->vpCavalryTrace;
      for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++)
      {
        pCavalryTrace->cavalry_info[loop].cavalry_pid = CAVALRY_INVALID_PID;
      } /* for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++) */
    } /* if (pCavalry->vpCavalryTrace != NULL) */
  } /* if (pCavalry != NULL) */
} /* cavalry_initialize_traceinfo() */

static uint32_t cavalry_update_traceinfo(struct ambarella_cavalry *pCavalry, uint32_t pid)
{
  uint32_t  retval;

  retval = 0;

  if (pCavalry != NULL)
  {
    if (pCavalry->vpCavalryTrace != NULL)
    {
      cavalry_trace_t *pCavalryTrace;
      uint32_t  loop;
      uint32_t  index;
      uint32_t  base_time;

      pCavalryTrace = (cavalry_trace_t *)pCavalry->vpCavalryTrace;
      loop          = 0;
      index         = 0xFFFFFFFFU;
      base_time     = visorc_get_curtime();

      while ((loop < CAVALRY_TRACE_MAX_PROCESS) && (index == 0xFFFFFFFFU))
      {
        if (pCavalryTrace->cavalry_info[loop].cavalry_pid == pid)
        {
          index = loop;
        } /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid == pid) */
        else /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid != pid) */
        {
          loop++;
        } /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid != pid) */
      } /* while ((loop < CAVALRY_TRACE_MAX_PROCESS) && (index == 0xFFFFFFFFU)) */

      /*-= Find available cavalry_trace to replace =----------------*/
      if (index == 0xFFFFFFFFU)
      {
        uint32_t  oldest_index;
        uint32_t  oldest_delta;

        loop          = 0;
        oldest_index  = CAVALRY_TRACE_MAX_PROCESS;
        oldest_delta  = 0;

        while ((loop < CAVALRY_TRACE_MAX_PROCESS) && (index == 0xFFFFFFFFU))
        {
          if (pCavalryTrace->cavalry_info[loop].cavalry_pid == CAVALRY_INVALID_PID)
          {
            index = loop;
          } /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid == CAVALRY_INVALID_PID) */
          else /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) */
          {
            uint32_t  curr_delta;
            curr_delta = base_time - pCavalryTrace->cavalry_info[loop].last_access_tick;
            if (curr_delta > oldest_delta)
            {
              oldest_delta  = curr_delta;
              oldest_index  = loop;
            } /* if (curr_delta > oldest_delta) */
            loop++;
          } /* if (pCavalryTrace->cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) */
        } /* while ((loop < CAVALRY_TRACE_MAX_PROCESS) && (index == 0xFFFFFFFFU)) */

        if ((index == 0xFFFFFFFFU) && (oldest_index != CAVALRY_TRACE_MAX_PROCESS))
        {
          index = oldest_index;
        } /* if ((index == 0xFFFFFFFFU) && (oldest_index != CAVALRY_TRACE_MAX_PROCESS)) */

        if (index != 0xFFFFFFFFU)
        {
          char proc_path[256];
          struct file *f;

          memset(&pCavalryTrace->cavalry_info[index], 0, sizeof(cavalry_process_trace_t));
          memset(proc_path, 0, sizeof(proc_path));

          pCavalryTrace->cavalry_info[index].cavalry_pid    = pid;
          pCavalryTrace->cavalry_info[index].frame_no       = 0xFFFFFFFFU;

          snprintf(proc_path, sizeof(proc_path), "/proc/%d/cmdline", pid);
          f = filp_open(proc_path, O_RDONLY, 0);
          if (f == NULL)
          {
            snprintf(&pCavalryTrace->cavalry_info[index].process_name[0], sizeof(pCavalryTrace->cavalry_info[index].process_name), "CAVALRY_PID(%d)", pid);
          } /* if (f == NULL) */
          else /* if (f != NULL) */
          {
#ifdef set_fs
            mm_segment_t fs;
            uint32_t  postproc_loop;
            fs = get_fs();
            set_fs(KERNEL_DS);
            f->f_op->read(f, &pCavalryTrace->cavalry_info[index].process_name[0], sizeof(pCavalryTrace->cavalry_info[index].process_name) - 1, &f->f_pos);
            set_fs(fs);
#else
            uint32_t  postproc_loop;
            f->f_op->read(f, &pCavalryTrace->cavalry_info[index].process_name[0], sizeof(pCavalryTrace->cavalry_info[index].process_name) - 1, &f->f_pos);
#endif
            filp_close(f, NULL);

            /* Reconnect in command line arguments */
            for (postproc_loop = 0; postproc_loop < f->f_pos; postproc_loop++)
            {
              if (pCavalryTrace->cavalry_info[index].process_name[postproc_loop] == 0x00)
              {
                pCavalryTrace->cavalry_info[index].process_name[postproc_loop] = ' ';
              } /* if (pCavalryTrace->cavalry_info[index].process_name[postproc_loop] == 0x00) */
            } /* for (postproc_loop = 0; postproc_loop < f->f_pos; postproc_loop++) */
          } /* if (f != NULL) */
        } /* if (index != 0xFFFFFFFFU) */
      } /* if (index == 0xFFFFFFFFU) */

      /*-= Update the information =---------------------------------*/
      if (index != 0xFFFFFFFFU)
      {
        pCavalryTrace->cavalry_info[index].frame_no++;
        pCavalryTrace->cavalry_info[index].last_access_tick = base_time;
        retval = pCavalryTrace->cavalry_info[index].frame_no;
      } /* if (index != 0xFFFFFFFFU) */
    } /* if (pCavalry->vpCavalryTrace != NULL) */
  } /* if (pCavalry != NULL) */

  return retval;

} /* cavalry_update_traceinfo() */

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY API (LINUX) =-------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
int  ioctl_cavalry_run_dags(struct ambarella_cavalry *pCavalry, void __user *arg)
{
  int retval;

  if (pCavalry == NULL)
  {
    prt_err("[ERROR] : ioctl_cavalry_run_dags() : Invalid pCavalry (%p)\n", pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (arg == 0)
  {
    prt_err("[ERROR] : ioctl_cavalry_run_dags() : Invalid run address (%p)\n", (void *)arg);
    retval = -EINVAL;
  } /* if (arg == 0) */
  else /* if ((pCavalry != NULL) && (arg != 0)) */
  {
    uint32_t  slot_id;
    uint32_t  slot_daddr;

    atomic_inc(&pCavalry->pending_cnt);
    atomic_set(&pCavalry->is_vp_done, 0);
    pCavalry->vp_current_pid = current->pid;

    slot_id = cavalry_find_slot(pCavalry, &slot_daddr);

    if (slot_id >= pCavalry->num_slots_enabled)
    {
      prt_err("[ERROR] : ioctl_cavalry_run_dags(%p) : Unable to find slot in system (enabled = %d/%d)\n",
             (void *)arg, pCavalry->num_slots_enabled, MAX_CAVALRY_SLOTS);
      retval = -ECAVALRY_UNABLE_TO_FIND_SLOT;
    } /* if (slot_id >= pCavalry->num_slots_enabled) */
    else /* if (slot_id < pCavalry->num_slots_enabled) */
    {
      uint32_t  token_id;

      token_id = cavalry_find_token(pCavalry, slot_id);

      if (token_id >= MAX_CAVALRY_TOKENS)
      {
        prt_err("[ERROR] : ioctl_cavalry_run_dags(%p) : Unable to find token in system (available = %d)\n",
               (void *)arg, MAX_CAVALRY_TOKENS);
        retval = -ECAVALRY_UNABLE_TO_FIND_TOKEN;
      } /* if (token_id >= MAX_CAVALRY_TOKENS) */
      else /* if (token_id < MAX_CAVALRY_TOKENS) */
      {
        struct cavalry_run_dags *pSrcRunDags;
        int local_retval;
        uint32_t  dag_cnt;
        struct cavalry_run_dags *pRunDags;

        pRunDags    = ambacv_c2v(slot_daddr);
        pSrcRunDags = (struct cavalry_run_dags *)arg;

        local_retval = copy_from_user(&dag_cnt, (void *)&pSrcRunDags->dag_cnt, sizeof(dag_cnt));
        if (local_retval == 0)
        {
          if ((dag_cnt == 0) || (dag_cnt > MAX_DAG_CNT))
          {
            prt_err("[ERROR] : ioctl_cavalry_run_dags() : Invalid dag_cnt %u\n", dag_cnt);
            retval = -EINVAL;
          } /* if ((dag_cnt == 0) || (dag_cnt > MAX_DAG_CNT)) */
          else /* if ((dag_cnt > 0) && (dag_cnt <= MAX_DAG_CNT)) */
          {
            uint32_t  used_size;
            used_size     = 20 + (dag_cnt * sizeof(struct cavalry_dag_desc));
            local_retval  = copy_from_user((void *)pRunDags, (void *)pSrcRunDags, used_size);
            if (local_retval == 0)
            {
              local_retval = cavalry_check_dags(pRunDags);
              if (local_retval < 0)
              {
                retval = -EINVAL;
                prt_err("[ERROR] : ioctl_cavalry_run_dags() : cavalry_check_dags(%p) returns failure (%d)\n", pRunDags, local_retval);
              } /* if (local_retval < 0) */
              else /* if (local_retval >= 0) */
              {
                used_size = (used_size + 63) & 0xFFFFFFC0;
                ambacv_cache_clean(pRunDags, used_size);
                retval = 0;
              } /* if (local_retval >= 0) */
            } /* if (local_retval == 0) */
            else /* if (local_retval != 0) */
            {
              retval = -EFAULT;
              prt_err("[ERROR] : ioctl_cavalry_run_dags() : Cannot copy %d bytes from userspace (%p) to kernel space (%p/0x%08x)\n", used_size, pSrcRunDags, pRunDags, slot_daddr);
            } /* if (local_retval != 0) */
          } /* if ((dag_cnt > 0) && (dag_cnt <= MAX_DAG_CNT)) */
        } /* if (local_retval == 0) : copy_from_user() : dag_cnt*/
        else /* if (local_retval != 0) */
        {
          prt_err("[ERROR] : ioctl_cavalry_run_dags() : Cannot copy dag_cnt from userspace (%p) to kernel space (%p/0x%08x)\n", pSrcRunDags, pRunDags, slot_daddr);
          retval = -EFAULT;
        } /* if (local_retval != 0) */

        if (atomic_read(&pCavalry->is_early_quit_all) != 0)
        {
          cavalry_release_token(pCavalry, token_id, 0, NULL);
          pRunDags->rval        = 0;
          pRunDags->start_tick  = 0;
          pRunDags->end_tick    = 0;
          pRunDags->finish_dags = 0;
          local_retval = copy_to_user(&pSrcRunDags->rval, &pRunDags->rval, 20);
          if (local_retval != 0)
          {
            prt_err("[ERROR] : ioctl_cavalry_run_dags() : Cannot return results from VISORC (%p/0x%08x) to userspace (%p)\n",
                   pRunDags, slot_daddr, pRunDags);
            retval = -EFAULT;
          } /* if (local_retval != 0) */
        } /* if (atomic_read(&pCavalry->is_early_quit_all)) */
        else if (retval == 0)
        {
          armvis_msg_t msgbase;
          schedmsg_cavalry_vp_run_request_t  *pCavalryMsg;

          memset(&msgbase, 0, sizeof(msgbase));

          pCavalryMsg = &msgbase.msg.cavalry_vp_run_request;
          msgbase.hdr.message_type                = SCHEDMSG_CAVALRY_VP_RUN_REQUEST;
          msgbase.hdr.message_id                  = 0;
          msgbase.hdr.message_retcode             = ERRCODE_NONE;
          pCavalryMsg->requested_function_token   = token_id;
          pCavalryMsg->priority                   = 1;
          pCavalryMsg->cavalry_pid                = current->pid;
          pCavalryMsg->frame_no                   = cavalry_update_traceinfo(pCavalry, current->pid);
          pCavalryMsg->parameter_daddr            = slot_daddr;
          pCavalryMsg->local_printf_buffer_daddr  = 0;
          pCavalryMsg->local_printf_buffer_size   = 0;

          mutex_lock(&pCavalry->token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          retval = cavalry_wait_token(pCavalry, token_id);

          ambacv_cache_invalidate(pRunDags, 64);

          local_retval = copy_to_user(&pSrcRunDags->rval, &pRunDags->rval, 20);

          cavalry_finish_slot(pCavalry, slot_id);

          if (local_retval != 0)
          {
            prt_err("[ERROR] : ioctl_cavalry_run_dags() : Cannot return results from VISORC (%p/0x%08x) to userspace (%p)\n",
                   pRunDags, slot_daddr, pRunDags);
            retval = -EFAULT;
          } /* if (local_retval != 0) */
        } /* if (retval == 0) */
        else /* if (retval != 0) */
        {
          cavalry_finish_slot(pCavalry, slot_id);
        } /* if (retval != 0) */

        cavalry_finish_token(pCavalry, token_id);

      } /* if (token_id < MAX_CAVALRY_TOKENS) */
    } /* if (slot_id < pCavalry->num_slots_enabled) */

    atomic_dec(&pCavalry->pending_cnt);
    if (atomic_read(&pCavalry->pending_cnt) == 0)
    {
      atomic_set(&pCavalry->is_vp_done, 1);
    } /* if (atomic_read(&pCavalry->pending_cnt) == 0) */

    if (atomic_read(&pCavalry->is_early_quit_all) != 0)
    {
      if (atomic_read(&pCavalry->pending_cnt) == 0)
      {
        atomic_set(&pCavalry->is_early_quit_all, 0);
        orc_send_sync_count(VISORC_EARLY_QUIT_SYNC, 0x0400);
      } /* if (atomic_read(&pCavalry->pending_cnt) == 0) */
    } /* if (atomic_read(&pCavalry->is_early_quit_all) != 0) */
  } /* if ((pCavalry != NULL) && (arg != 0)) */

  return retval;

} /* ioctl_cavalry_run_dags() */

int  ioctl_cavalry_run_hotlink_slot(struct ambarella_cavalry *pCavalry, void __user *arg)
{
  int retval;

  retval        = 0;
  if (pCavalry == NULL)
  {
    prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot() : Invalid pCavalry (%p)\n", pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (arg == 0)
  {
    prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot() : Invalid run address (%p)\n", (void *)arg);
    retval = -EINVAL;
  } /* if (arg == 0) */
  else /* if (arg != 0) */
  {
    int local_retval;
    struct cavalry_run_hotlink_slot CavRunHotlink;

    local_retval = copy_from_user((void *)&CavRunHotlink, (void *)arg, sizeof(CavRunHotlink));

    if (local_retval != 0)
    {
      retval = -EFAULT;
      prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot() : Cannot copy %ld bytes from userspace (%p) to kernel space (%p)\n", sizeof(CavRunHotlink), arg, (void *)&CavRunHotlink);
    } /* if (local_retval != 0) : copy_from_user()  */
    else /* if (local_retval == 0) : copy_from_user() */
    {
      if (((pCavalry->slot_activated) & (1 << CavRunHotlink.slot_id)) == 0)
      {
        prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot() : Slot [%u] is not currently active\n", CavRunHotlink.slot_id);
        retval = -EINVAL;
      } /* if (((pCavalry->slot_activated) & (1 << CavRunHotlink.slot_id)) == 0) */
      else /* if (((pCavalry->slot_activated) & (1 << CavRunHotlink.slot_id)) != 0) */
      {
        uint32_t  token_id;

        token_id = cavalry_find_token(pCavalry, CAV_INVALID_SLOTID);

        if (token_id >= MAX_CAVALRY_TOKENS)
        {
          prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot(%p) : Unable to find token in system (available = %d)\n",
                 (void *)arg, MAX_CAVALRY_TOKENS);
          retval = -ECAVALRY_UNABLE_TO_FIND_TOKEN;
        } /* if (token_id >= MAX_CAVALRY_TOKENS) */
        else /* if (token_id < MAX_CAVALRY_TOKENS) */
        {
          armvis_msg_t msgbase;
          schedmsg_cavalry_hl_run_request_t *pCavalryHotlinkMsg;
          schedmsg_cavalry_hl_run_reply_t *pReply;

          memset(&msgbase, 0, sizeof(msgbase));

          pCavalryHotlinkMsg = &msgbase.msg.cavalry_hotlink_run_request;
          msgbase.hdr.message_type                      = SCHEDMSG_CAVALRY_HL_RUN_REQUEST;
          msgbase.hdr.message_id                        = 0;
          msgbase.hdr.message_retcode                   = ERRCODE_NONE;
          pCavalryHotlinkMsg->requested_function_token  = token_id;
          pCavalryHotlinkMsg->priority                  = 1;
          pCavalryHotlinkMsg->cavalry_pid               = current->pid;
          pCavalryHotlinkMsg->frame_no                  = cavalry_update_traceinfo(pCavalry, current->pid);
          pCavalryHotlinkMsg->parameter_daddr           = CavRunHotlink.arg_daddr + pCavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys;
          pCavalryHotlinkMsg->slot_number               = CavRunHotlink.slot_id;

          mutex_lock(&pCavalry->token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          local_retval = cavalry_wait_token(pCavalry, token_id);

          /* TODO: Process framework_retcode */
          pReply  = (schedmsg_cavalry_hl_run_reply_t *)&pCavalry->cavtoken_replymsg[token_id][0];
          CavRunHotlink.start_tick  = pReply->started_time;
          CavRunHotlink.end_tick    = pReply->finished_time;
          CavRunHotlink.slot_rval   = local_retval;

          local_retval = copy_to_user((void *)arg, (void *)&CavRunHotlink, sizeof(CavRunHotlink));

          if (local_retval != 0)
          {
            prt_err("[ERROR] : ioctl_cavalry_run_hotlink_slot() : Cannot copy returned results from VISORC message (%p) to userspace (%p), size %ld\n",
                   (void *)&CavRunHotlink, (void *)arg, sizeof(CavRunHotlink));
            retval = -EFAULT;
          } /* if (local_retval != 0) : copy_to_user() */
          cavalry_finish_token(pCavalry, token_id);
        } /* if (token_id < MAX_CAVALRY_TOKENS) */
      } /* if (((pCavalry->slot_activated) & (1 << CavRunHotlink.slot_id)) != 0) */
    } /* if (local_retval == 0) : copy_from_user() */
  } /* if ((pCavalry != NULL) && (arg != 0)) */

  return retval;

} /* ioctl_cavalry_run_hotlink_slot() */

/*-------------------------------------------------------------------------------------------*/
/*-= CAVALRY reply handler =-----------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
int  ioctl_cavalry_handle_visorc_reply(struct ambarella_cavalry *pCavalry, void *pArg)
{
  int retval;

  if (pCavalry == NULL)
  {
    prt_err("[error] : ioctl_cavalry_handle_visorc_reply() : Invalid pCavalry (%p)\n", (void *)pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (pArg == NULL)
  {
    prt_err("[error] : ioctl_cavalry_handle_visorc_reply() : Invalid pArg (%p)\n", (void *)pArg);
    retval = -EINVAL;
  } /* if (pArg == NULL) */
  else /* if ((pCavalry != NULL) && (pArg != NULL)) */
  {
    armvis_msg_t *pRXMsg;

    pRXMsg = (armvis_msg_t *)pArg;

    switch (pRXMsg->hdr.message_type)
    {
    case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
      retval = cavalry_release_token(pCavalry, pRXMsg->msg.cavalry_vp_run_reply.requested_function_token,
                                     pRXMsg->msg.cavalry_vp_run_reply.function_retcode, pRXMsg);
      break;

    case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
      retval = cavalry_release_token(pCavalry, pRXMsg->msg.cavalry_hotlink_run_reply.requested_function_token,
                                     pRXMsg->msg.cavalry_hotlink_run_reply.function_retcode, pRXMsg);
      break;

    case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
      retval = cavalry_release_token(pCavalry, pRXMsg->msg.cavalry_fex_query_reply.requested_function_token, pRXMsg->hdr.message_retcode, pRXMsg);
      break;

    default:
      prt_err("[error] : ioctl_cavalry_handle_visorc_reply() : Unknown message type (0x%04x)\n", pRXMsg->hdr.message_type);
      retval = -EINVAL;
      break;
    } /* switch (pRXMsg->hdr.message_type) */
  } /* if ((pCavalry != NULL) && (pArg != NULL)) */

  return retval;

} /* ioctl_cavalry_handle_visorc_reply() */

/*====================================================================================================================*/
/*=- CAVALRY/FEX related -============================================================================================*/
/*====================================================================================================================*/
int ioctl_cavalry_fex_query(struct ambarella_cavalry *pCavalry, void __user *puArg)
{
  int retval;

  retval = 0;

  if (pCavalry == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fex_query() : Invalid pCavalry (%p)\n", (void *)pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (puArg == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fex_query() : Invalid puArg (%p)\n", (void *)puArg);
    retval = -EINVAL;
  } /* if (puArg == NULL) */
#ifdef CHIP_CV2
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    struct cavalry_fex_query fex_query;
    int local_retval;

    local_retval = copy_from_user(&fex_query, puArg, sizeof(fex_query));

    if (local_retval == 0)
    {
      uint32_t  token_id;

      token_id = cavalry_find_token(pCavalry, CAV_INVALID_SLOTID);

      if (token_id < MAX_CAVALRY_TOKENS)
      {
        armvis_msg_t msgbase;
        schedmsg_cavalry_fex_query_request_t *pMsg;
        fex_query_cmd_t *fex_query_cmd;
        fex_query_msg_t *fex_query_msg;
        uint32_t  run_done;

        msgbase.hdr.message_type    = SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST;
        msgbase.hdr.message_id      = 0;
        msgbase.hdr.message_retcode = ERRCODE_NONE;
        pMsg = &msgbase.msg.cavalry_fex_query_request;
        pMsg->requested_function_token  = token_id;
        pMsg->cavalry_pid               = current->pid;
        pMsg->frame_no                  = cavalry_update_traceinfo(pCavalry, current->pid);

        fex_query_cmd = (fex_query_cmd_t *)&pMsg->cmd;

        fex_query_cmd->cmd_code           = FEX_QUERY_CMD;
        fex_query_cmd->img_width          = fex_query.img_width;
        fex_query_cmd->img_height         = fex_query.img_height;
        fex_query_cmd->img_pitch          = fex_query.img_pitch;
        fex_query_cmd->harris_en          = fex_query.harris_en;
        fex_query_cmd->stereo_en          = fex_query.stereo_en;
        fex_query_cmd->dump_harris_score  = fex_query.dump_harris_score;

        mutex_lock(&pCavalry->token_mutex);

        schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

        local_retval = cavalry_wait_token(pCavalry, token_id);

        /* TODO: Process framework_retcode */

        fex_query_msg = (fex_query_msg_t *)&pCavalry->cavtoken_replymsg[token_id][0];
        run_done      = 0;

        if (fex_query_msg->msg_code == cmd_to_msg(FEX_QUERY_CMD))
        {
          switch (fex_query_msg->rval)
          {
          case MSG_RVAL_INVALID_CMD:
            prt_err("[error] : ioctl_cavalry_fex_query() : CAVALRY/FEX returns MSG_RVAL_INVALID_CMD\n");
            retval = -EFAULT;
            break;
          default:
            run_done = 1;
            retval  = 0;
            break;
          }
        } /* if (fex_query_msg->msg_code == cmd_to_msg(FEX_QUERY_CMD)) */
        else /* if (fex_query_msg->msg_code != cmd_to_msg(FEX_QUERY_CMD)) */
        {
          prt_err("[error] : ioctl_cavalry_fex_query() : Invalid return command (0x%08x, expected 0x%08x)\n",
                 fex_query_msg->msg_code, cmd_to_msg(FEX_QUERY_CMD));
          retval = -EFAULT;
        } /* if (fex_query_msg->msg_code != cmd_to_msg(FEX_QUERY_CMD)) */

        if (run_done != 0)
        {
          fex_query.dram_required = fex_query_msg->dram_required;

          local_retval = copy_to_user(puArg, &fex_query, sizeof(fex_query));

          if (local_retval != 0)
          {
            prt_err("[error] : ioctl_cavalry_fex_query() : Unable to copy fex_query reply to user space (%p)\n", (void *)puArg);
            retval = -EFAULT;
          } /* if (local_retval != 0) : copy_to_user() */
        } /* if (run_done != 0) */

        cavalry_finish_token(pCavalry, token_id);

      } /* if (token_id < MAX_CAVALRY_TOKENS) */
      else /* if (token_id >= MAX_CAVALRY_TOKENS) */
      {
        prt_err("[error] : ioctl_cavalry_fex_query() : Unable to find cavalry token\n");
        retval = -EINVAL;
      } /* if (token_id >= MAX_CAVALRY_TOKENS) */
    } /* if (local_retval == 0) */
    else /* if (local_retval != 0) */
    {
      prt_err("[error] : ioctl_cavalry_fex_query() : Unable to copy fex_query request from user space (%p)\n", (void *)puArg);
      retval = -EFAULT;
    } /* if (local_retval != 0) */
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#else /* !CHIP_CV2 */
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    prt_err("[error] : ioctl_cavalry_fex_query() : CAVALRY/FEX only available on CV2\n");
    retval = -EINVAL;
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#endif /* ?CHIP_CV2 */

  return retval;

} /* ioctl_cavalry_fex_query() */

/* From cavalry_visorc.c */
static int cavalry_check_fex_run(struct ambarella_cavalry *cavalry,
	struct cavalry_fex_run *fex_run)
{
	if (fex_run->stereo_en && (!fex_run->luma_daddr[0] ||
		!fex_run->luma_daddr[1])) {
		prt_err("luma_daddr[0] 0x%x and luma_daddr[1] 0x%x can't be NULL when " \
			"stereo is enabled.\n", fex_run->luma_daddr[0], fex_run->luma_daddr[1]);
		return -1;
	}

	if ((fex_run->harris_en & 0x01) && (!fex_run->luma_daddr[0])) {
		prt_err("luma_daddr[0] can't be NULL when Harris for this input is enabled.\n");
		return -1;
	}

	if (((fex_run->harris_en >> 1) & 0x01) && (!fex_run->luma_daddr[1])) {
		prt_err("luma_daddr[1] can't be NULL when Harris for this input is enabled.\n");
		return -1;
	}

	if (!fex_run->img_width) {
		prt_err("Image Width can't be Zero.\n");
		return -1;
	}

	if (!fex_run->img_height) {
		prt_err("Image Height can't be Zero.\n");
		return -1;
	}

	if (fex_run->img_pitch < fex_run->img_width) {
		prt_err("Image Pitch [%u] can't be smaller than Width [%u].\n", fex_run->img_pitch,
			fex_run->img_width);
		return -1;
	}

	if ((fex_run->harris_en || fex_run->stereo_en) &&
		(!fex_run->output_daddr || !fex_run->output_size)) {
		prt_err("Output Daddr and Size can't be Zero when Harris or Stereo is enabled.\n");
		return -1;
	}

	if (fex_run->stereo_profile >= STEREO_PROFILE_NUM) {
		prt_err("Stereo Profile should be less than %u.\n", STEREO_PROFILE_NUM);
		return -1;
	}

	if (fex_run->fex_cfg.cfg_mask & FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS) {
		if (fex_run->fex_cfg.nms_window_for_harris < FEX_MIN_NMS_WINDOW ||
			fex_run->fex_cfg.nms_window_for_harris > FEX_MAX_NMS_WINDOW) {
				prt_err("Valid NMS window should be [%u-%u].\n", FEX_MIN_NMS_WINDOW,
					FEX_MAX_NMS_WINDOW);
				return -1;
		}
	}

	return 0;
}

int ioctl_cavalry_fex_run(struct ambarella_cavalry *pCavalry, void __user *puArg)
{
  int retval;

  retval = 0;

  if (pCavalry == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fex_run() : Invalid pCavalry (%p)\n", (void *)pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (puArg == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fex_run() : Invalid puArg (%p)\n", (void *)puArg);
    retval = -EINVAL;
  } /* if (puArg == NULL) */
#ifdef CHIP_CV2
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    struct cavalry_fex_run fex_run;
    int local_retval;

    local_retval = copy_from_user(&fex_run, puArg, sizeof(fex_run));

    if (local_retval == 0)
    {
      local_retval = cavalry_check_fex_run(pCavalry, &fex_run);

      if (local_retval >= 0)
      {
        uint32_t  token_id;

        token_id = cavalry_find_token(pCavalry, CAV_INVALID_SLOTID);

        if (token_id < MAX_CAVALRY_TOKENS)
        {
          armvis_msg_t msgbase;
          schedmsg_cavalry_fex_run_request_t *pMsg;
          fex_run_cmd_t *fex_run_cmd;
          fex_run_msg_t *fex_run_msg;
          uint32_t  run_done;

          msgbase.hdr.message_type    = SCHEDMSG_CAVALRY_FEX_RUN_REQUEST;
          msgbase.hdr.message_id      = 0;
          msgbase.hdr.message_retcode = ERRCODE_NONE;

          pMsg = &msgbase.msg.cavalry_fex_run_request;
          pMsg->requested_function_token  = token_id;
          pMsg->cavalry_pid               = current->pid;
          pMsg->frame_no                  = cavalry_update_traceinfo(pCavalry, current->pid);

          fex_run_cmd = (fex_run_cmd_t *)&pMsg->cmd;
          fex_run_cmd->cmd_code           = FEX_RUN_CMD;
          fex_run_cmd->img_width          = fex_run.img_width;
          fex_run_cmd->img_height         = fex_run.img_height;
          fex_run_cmd->img_pitch          = fex_run.img_pitch;
          fex_run_cmd->harris_en          = fex_run.harris_en;
          fex_run_cmd->stereo_en          = fex_run.stereo_en;
          fex_run_cmd->stereo_profile     = fex_run.stereo_profile;
          fex_run_cmd->dump_harris_score  = fex_run.dump_harris_score;
          fex_run_cmd->output_daddr       = fex_run.output_daddr;
          fex_run_cmd->output_size        = fex_run.output_size;
          fex_run_cmd->luma_daddr[0]      = fex_run.luma_daddr[0];
          fex_run_cmd->luma_daddr[1]      = fex_run.luma_daddr[1];
#if 0
          fex_run_cmd->fex_cfg            = fex_run.fex_cfg;
#else
          fex_run_cmd->fex_cfg.cfg_mask               = fex_run.fex_cfg.cfg_mask;
          fex_run_cmd->fex_cfg.nms_threshold          = fex_run.fex_cfg.nms_threshold;
          fex_run_cmd->fex_cfg.nms_window_for_harris  = fex_run.fex_cfg.nms_window_for_harris;
          fex_run_cmd->fex_cfg.reserved2              = fex_run.fex_cfg.reserved2;
#endif

          mutex_lock(&pCavalry->token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          local_retval = cavalry_wait_token(pCavalry, token_id);

          /* TODO: Process framework_retcode */

          fex_run_msg = (fex_run_msg_t *)&pCavalry->cavtoken_replymsg[token_id][0];
          run_done    = 0;

          if (fex_run_msg->msg_code == cmd_to_msg(FEX_RUN_CMD))
          {
            switch (fex_run_msg->rval)
            {
            case MSG_RVAL_INVALID_CMD:
              prt_err("[error] : ioctl_cavalry_fex_run() : CAVALRY/FEX returns MSG_RVAL_INVALID_CMD\n");
              retval = -EFAULT;
              break;
            case MSG_RVAL_FEX_INSUFFICIENT_DRAM:
              prt_err("[error] : ioctl_cavalry_fex_run() : DRAM size 0x%08x is insufficient for FEX\n", fex_run_cmd->output_size);
              retval = -EFAULT;
              break;
            default:
              run_done = 1;
              retval = 0;
              break;
            } /* switch (fex_run_msg->rval) */
          } /* if (fex_run_msg->msg_code == cmd_to_msg(FEX_RUN_CMD)) */
          else /* if (fex_run_msg->msg_code != cmd_to_msg(FEX_RUN_CMD)) */
          {
            prt_err("[error] : ioctl_cavalry_fex_run() : Invalid return command (0x%08x, expected 0x%08x)\n",
                   fex_run_msg->msg_code, cmd_to_msg(FEX_RUN_CMD));
            retval = -EFAULT;
          } /* if (fex_run_msg->msg_code != cmd_to_msg(FEX_RUN_CMD)) */

          if (run_done != 0)
          {
            fex_run.rval                      = fex_run_msg->rval;
            fex_run.disparity_daddr           = fex_run_msg->disparity_daddr;
            fex_run.disparity_size            = fex_run_msg->disparity_size;
            fex_run.invalid_disparities       = fex_run_msg->invalid_disparities;
            fex_run.disparity_width           = fex_run_msg->disparity_width;
            fex_run.disparity_height          = fex_run_msg->disparity_height;
            fex_run.disparity_pitch           = fex_run_msg->disparity_pitch;
            fex_run.harris_count_daddr[0]     = fex_run_msg->harris_count_daddr[0];
            fex_run.harris_count_daddr[1]     = fex_run_msg->harris_count_daddr[1];
            fex_run.harris_count_size[0]      = fex_run_msg->harris_count_size[0];
            fex_run.harris_count_size[1]      = fex_run_msg->harris_count_size[1];
            fex_run.harris_point_daddr[0]     = fex_run_msg->harris_point_daddr[0];
            fex_run.harris_point_daddr[1]     = fex_run_msg->harris_point_daddr[1];
            fex_run.harris_point_size[0]      = fex_run_msg->harris_point_size[0];
            fex_run.harris_point_size[1]      = fex_run_msg->harris_point_size[1];
            fex_run.brief_descriptor_daddr[0] = fex_run_msg->brief_descriptor_daddr[0];
            fex_run.brief_descriptor_daddr[1] = fex_run_msg->brief_descriptor_daddr[1];
            fex_run.brief_descriptor_size[0]  = fex_run_msg->brief_descriptor_size[0];
            fex_run.brief_descriptor_size[1]  = fex_run_msg->brief_descriptor_size[1];
            fex_run.start_tick                = fex_run_msg->start_tick;
            fex_run.end_tick                  = fex_run_msg->end_tick;

            local_retval = copy_to_user(puArg, &fex_run, sizeof(fex_run));

            if (local_retval != 0)
            {
              prt_err("[error] : ioctl_cavalry_fex_run() : Unable to copy fex_run reply to user space (%p)\n", (void *)puArg);
              retval = -EFAULT;
            } /* if (local_retval != 0) : copy_to_user() */
          } /* if (run_done != 0) */

          cavalry_finish_token(pCavalry, token_id);

        } /* if (token_id < MAX_CAVALRY_TOKENS) */
        else /* if (token_id >= MAX_CAVALRY_TOKENS) */
        {
          prt_err("[error] : ioctl_cavalry_fex_run() : Unable to find cavalry token\n");
          retval = -EINVAL;
        } /* if (token_id >= MAX_CAVALRY_TOKENS) */
      } /* if (local_retval >= 0) : cavalry_check_fex_run() */
      else /* if (local_retval < 0) : cavalry_check_fex_run() */
      {
        prt_err("[error] : ioctl_cavalry_fex_run() : cavalry_check_fex_run() returns error (%d)\n", local_retval);
        retval = -EINVAL;
      } /* if (local_retval < 0) : cavalry_check_fex_run() */
    } /* if (local_retval == 0) : copy_from_user() */
    else /* if (local_retval != 0) : copy_from_user() */
    {
      prt_err("[error] : ioctl_cavalry_fex_run() : Unable to copy fex_run request from user space (%p)\n", (void *)puArg);
      retval = -EFAULT;
    } /* if (local_retval != 0) : copy_from_user() */
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#else /* !CHIP_CV2 */
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    prt_err("[error] : ioctl_cavalry_fex_run() : CAVALRY/FEX only available on CV2\n");
    retval = -EINVAL;
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#endif /* ?CHIP_CV2 */

  return retval;

} /* ioctl_cavalry_fex_run() */

/*====================================================================================================================*/
/*=- CAVALRY/FMA related -============================================================================================*/
/*====================================================================================================================*/
int ioctl_cavalry_fma_query(struct ambarella_cavalry *pCavalry, void __user *puArg)
{
  int retval;

  retval = 0;

  if (pCavalry == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fma_query() : Invalid pCavalry (%p)\n", (void *)pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (puArg == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fma_query() : Invalid puArg (%p)\n", (void *)puArg);
    retval = -EINVAL;
  } /* if (puArg == NULL) */
#ifdef CHIP_CV2
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    struct cavalry_fma_query fma_query;
    int local_retval;

    local_retval = copy_from_user(&fma_query, puArg, sizeof(fma_query));

    if (local_retval == 0)
    {
      uint32_t  token_id;

      token_id = cavalry_find_token(pCavalry, CAV_INVALID_SLOTID);

      if (token_id < MAX_CAVALRY_TOKENS)
      {
        armvis_msg_t msgbase;
        schedmsg_cavalry_fma_query_request_t *pMsg;
        fma_query_cmd_t *fma_query_cmd;
        fma_query_msg_t *fma_query_msg;
        uint32_t  run_done;

        msgbase.hdr.message_type    = SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST;
        msgbase.hdr.message_id      = 0;
        msgbase.hdr.message_retcode = ERRCODE_NONE;
        pMsg = &msgbase.msg.cavalry_fma_query_request;
        pMsg->requested_function_token  = token_id;
        pMsg->cavalry_pid               = current->pid;
        pMsg->frame_no                  = cavalry_update_traceinfo(pCavalry, current->pid);
        fma_query_cmd = &pMsg->cmd;

        fma_query_cmd->cmd_code = FMA_QUERY_CMD;

        mutex_lock(&pCavalry->token_mutex);

        schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

        local_retval = cavalry_wait_token(pCavalry, token_id);

        /* TODO: Process framework_retcode */

        fma_query_msg = (fma_query_msg_t *)&pCavalry->cavtoken_replymsg[token_id][0];
        run_done      = 0;

        if (fma_query_msg->msg_code == cmd_to_msg(FMA_QUERY_CMD))
        {
          switch (fma_query_msg->rval)
          {
          case MSG_RVAL_INVALID_CMD:
            prt_err("[error] : ioctl_cavalry_fma_query() : CAVALRY/FMA returns MSG_RVAL_INVALID_CMD\n");
            retval = -EFAULT;
            break;
          default:
            run_done = 1;
            retval = 0;
            break;
          } /* switch (fma_query_msg->rval) */
        } /* if (fma_query_msg->msg_code == cmd_to_msg(FMA_QUERY_CMD)) */
        else /* if (fma_query_msg->msg_code != cmd_to_msg(FMA_QUERY_CMD)) */
        {
          prt_err("[error] : ioctl_cavalry_fma_query() : Invalid return command (0x%08x, expected 0x%08x)\n",
                 fma_query_msg->msg_code, cmd_to_msg(FMA_QUERY_CMD));
          retval = -EFAULT;
        } /* if (fma_query_msg->msg_code != cmd_to_msg(FMA_QUERY_CMD)) */

        if (run_done != 0)
        {
          fma_query.dram_required = fma_query_msg->dram_required;

          local_retval = copy_to_user(puArg, &fma_query, sizeof(fma_query));

          if (local_retval != 0)
          {
            prt_err("[error] : ioctl_cavalry_fma_query() : Unable to copy fex_query reply to user space (%p)\n", (void *)puArg);
            retval = -EFAULT;
          } /* if (local_retval != 0) : copy_to_user() */
        } /* if (run_done != 0) */

        cavalry_finish_token(pCavalry, token_id);
      } /* if (token_id < MAX_CAVALRY_TOKENS) */
      else /* if (token_id >= MAX_CAVALRY_TOKENS) */
      {
        prt_err("[error] : ioctl_cavalry_fma_query() : Unable to find cavalry token\n");
        retval = -EINVAL;
      } /* if (token_id >= MAX_CAVALRY_TOKENS) */
    } /* if (local_retval == 0) */
    else /* if (local_retval != 0) */
    {
      prt_err("[error] : ioctl_cavalry_fma_query() : Unable to copy fma_query request from user space (%p)\n", (void *)puArg);
      retval = -EFAULT;
    } /* if (local_retval != 0) */
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#else /* !CHIP_CV2 */
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    prt_err("[error] : ioctl_cavalry_fma_query() : CAVALRY/FMA only available on CV2\n");
    retval = -EINVAL;
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#endif /* ?CHIP_CV2 */

  return retval;

} /* ioctl_cavalry_fma_query() */

/* From cavalry_visorc.c */
static int cavalry_check_fma_run(struct ambarella_cavalry *cavalry, struct cavalry_fma_run *fma_run)
{
	if (!fma_run->output_daddr) {
		prt_err("output_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->output_size) {
		prt_err("output_size can't be Zero.\n");
		return -1;
	}

	if (!fma_run->target_coord_daddr) {
		prt_err("target_coord_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->target_descriptor_daddr) {
		prt_err("target_descriptor_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->reference_coord_daddr) {
		prt_err("reference_coord_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->reference_descriptor_daddr) {
		prt_err("reference_descriptor_daddr can't be Zero.\n");
		return -1;
	}

	if ((fma_run->mode != CAVALRY_FMA_MODE_TEMPORAL) &&
		(fma_run->mode != CAVALRY_FMA_MODE_STEREO)) {
		prt_err("Incorrect FMA mode %u.\n", fma_run->mode);
		return -1;
	}

	return 0;
}

int ioctl_cavalry_fma_run(struct ambarella_cavalry *pCavalry, void __user *puArg)
{
  int retval;

  retval = 0;

  if (pCavalry == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fma_run() : Invalid pCavalry (%p)\n", (void *)pCavalry);
    retval = -EINVAL;
  } /* if (pCavalry == NULL) */
  else if (puArg == NULL)
  {
    prt_err("[error] : ioctl_cavalry_fma_run() : Invalid puArg (%p)\n", (void *)puArg);
    retval = -EINVAL;
  } /* if (puArg == NULL) */
#ifdef CHIP_CV2
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    struct cavalry_fma_run fma_run;
    int local_retval;

    local_retval = copy_from_user(&fma_run, puArg, sizeof(fma_run));

    if (local_retval == 0)
    {
      local_retval = cavalry_check_fma_run(pCavalry, &fma_run);

      if (local_retval >= 0)
      {
        uint32_t  token_id;

        token_id = cavalry_find_token(pCavalry, CAV_INVALID_SLOTID);

        if (token_id < MAX_CAVALRY_TOKENS)
        {
          armvis_msg_t msgbase;
          schedmsg_cavalry_fma_run_request_t *pMsg;
          fma_run_cmd_t *fma_run_cmd;
          fma_run_msg_t *fma_run_msg;
          uint32_t  run_done;

          msgbase.hdr.message_type    = SCHEDMSG_CAVALRY_FMA_RUN_REQUEST;
          msgbase.hdr.message_id      = 0;
          msgbase.hdr.message_retcode = ERRCODE_NONE;

          pMsg = &msgbase.msg.cavalry_fma_run_request;
          pMsg->requested_function_token  = token_id;
          pMsg->cavalry_pid               = current->pid;
          pMsg->frame_no                  = cavalry_update_traceinfo(pCavalry, current->pid);

          fma_run_cmd = &pMsg->cmd;
          fma_run_cmd->cmd_code                   = FMA_RUN_CMD;
          fma_run_cmd->output_daddr               = fma_run.output_daddr;
          fma_run_cmd->output_size                = fma_run.output_size;
          fma_run_cmd->target_coord_daddr         = fma_run.target_coord_daddr;
          fma_run_cmd->target_descriptor_daddr    = fma_run.target_descriptor_daddr;
          fma_run_cmd->reference_coord_daddr      = fma_run.reference_coord_daddr;
          fma_run_cmd->reference_descriptor_daddr = fma_run.reference_descriptor_daddr;
          fma_run_cmd->mode                       = fma_run.mode;
#if 0
          fma_run_cmd->temporal_cfg               = fma_run.temporal_cfg;
          fma_run_cmd->stereo_cfg                 = fma_run.stereo_cfg;
#else
          fma_run_cmd->temporal_cfg.cfg_mask        = fma_run.temporal_cfg.cfg_mask;
          fma_run_cmd->temporal_cfg.min_threshold   = fma_run.temporal_cfg.min_threshold;
          fma_run_cmd->temporal_cfg.ratio_threshold = fma_run.temporal_cfg.ratio_threshold;
          fma_run_cmd->temporal_cfg.reserved        = fma_run.temporal_cfg.reserved;
          fma_run_cmd->temporal_cfg.win_width       = fma_run.temporal_cfg.win_width;
          fma_run_cmd->temporal_cfg.win_height      = fma_run.temporal_cfg.win_height;
          fma_run_cmd->temporal_cfg.x_threshold_r   = fma_run.temporal_cfg.x_threshold_r;
          fma_run_cmd->temporal_cfg.x_threshold_l   = fma_run.temporal_cfg.x_threshold_l;
          fma_run_cmd->temporal_cfg.y_threshold_u   = fma_run.temporal_cfg.y_threshold_u;
          fma_run_cmd->temporal_cfg.y_threshold_d   = fma_run.temporal_cfg.y_threshold_d;

          fma_run_cmd->stereo_cfg.cfg_mask          = fma_run.stereo_cfg.cfg_mask;
          fma_run_cmd->stereo_cfg.min_threshold     = fma_run.stereo_cfg.min_threshold;
          fma_run_cmd->stereo_cfg.ratio_threshold   = fma_run.stereo_cfg.ratio_threshold;
          fma_run_cmd->stereo_cfg.reserved          = fma_run.stereo_cfg.reserved;
          fma_run_cmd->stereo_cfg.win_width         = fma_run.stereo_cfg.win_width;
          fma_run_cmd->stereo_cfg.win_height        = fma_run.stereo_cfg.win_height;
          fma_run_cmd->stereo_cfg.x_threshold_r     = fma_run.stereo_cfg.x_threshold_r;
          fma_run_cmd->stereo_cfg.x_threshold_l     = fma_run.stereo_cfg.x_threshold_l;
          fma_run_cmd->stereo_cfg.y_threshold_u     = fma_run.stereo_cfg.y_threshold_u;
          fma_run_cmd->stereo_cfg.y_threshold_d     = fma_run.stereo_cfg.y_threshold_d;
#endif

          mutex_lock(&pCavalry->token_mutex);

          schdrmsg_send_armmsg((unsigned long)&msgbase, 1);

          local_retval = cavalry_wait_token(pCavalry, token_id);

          /* TODO: Process framework_retcode */

          fma_run_msg = (fma_run_msg_t *)&pCavalry->cavtoken_replymsg[token_id][0];
          run_done    = 0;

          if (fma_run_msg->msg_code == cmd_to_msg(FMA_RUN_CMD))
          {
            switch (fma_run_msg->rval)
            {
            case MSG_RVAL_INVALID_CMD:
              prt_err("[error] : ioctl_cavalry_fma_run() : CAVALRY/FMA returns MSG_RVAL_INVALID_CMD\n");
              retval = -EFAULT;
              break;
            case MSG_RVAL_FMA_INSUFFICIENT_DRAM:
              prt_err("[error] : ioctl_cavalry_fma_run() : DRAM size 0x%08x is insufficient for FMA\n", fma_run_cmd->output_size);
              retval = -EFAULT;
              break;
            default:
              run_done  = 1;
              retval    = 0;
              break;
            } /* switch (fma_run_msg->rval) */
          } /* if (fma_run_msg->msg_code == cmd_to_msg(FMA_RUN_CMD)) */
          else /* if (fma_run_msg->msg_code != cmd_to_msg(FMA_RUN_CMD)) */
          {
            prt_err("[error] : ioctl_cavalry_fma_run() : Invalid return command (0x%08x, expected 0x%08x)\n",
                   fma_run_msg->msg_code, cmd_to_msg(FMA_RUN_CMD));
            retval = -EFAULT;
          } /* if (fma_run_msg->msg_code != cmd_to_msg(FMA_RUN_CMD)) */

          if (run_done != 0)
          {
            fma_run.rval                  = fma_run_msg->rval;
            fma_run.result_score_daddr    = fma_run_msg->result_score_daddr;
            fma_run.result_score_size     = fma_run_msg->result_score_size;
            fma_run.result_index_daddr    = fma_run_msg->result_index_daddr;
            fma_run.result_index_size     = fma_run_msg->result_index_size;
            fma_run.temporal_coord_daddr  = fma_run_msg->temporal_coord_daddr;
            fma_run.temporal_coord_size   = fma_run_msg->temporal_coord_size;
            fma_run.start_tick            = fma_run_msg->start_tick;
            fma_run.end_tick              = fma_run_msg->end_tick;

            local_retval = copy_to_user(puArg, &fma_run, sizeof(fma_run));

            if (local_retval != 0)
            {
              prt_err("[error] : ioctl_cavalry_fma_run() : Unable to copy fma_run reply to user space (%p)\n", (void *)puArg);
              retval = -EFAULT;
            } /* if (local_retval != 0) : copy_to_user() */
          } /* if (run_done != 0) */

          cavalry_finish_token(pCavalry, token_id);

        } /* if (token_id < MAX_CAVALRY_TOKENS) */
        else /* if (token_id >= MAX_CAVALRY_TOKENS) */
        {
          prt_err("[error] : ioctl_cavalry_fma_run() : Unable to find cavalry token\n");
          retval = -EINVAL;
        } /* if (token_id >= MAX_CAVALRY_TOKENS) */
      } /* if (local_retval >= 0) : cavalry_check_fex_run() */
      else /* if (local_retval < 0) : cavalry_check_fex_run() */
      {
        prt_err("[error] : ioctl_cavalry_fma_run() : cavalry_check_fma_run() returns error (%d)\n", local_retval);
        retval = -EINVAL;
      } /* if (local_retval < 0) : cavalry_check_fex_run() */
    } /* if (local_retval == 0) : copy_from_user() */
    else /* if (local_retval != 0) : copy_from_user() */
    {
      prt_err("[error] : ioctl_cavalry_fma_run() : Unable to copy fma_run request from user space (%p)\n", (void *)puArg);
      retval = -EFAULT;
    } /* if (local_retval != 0) : copy_from_user() */
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#else /* !CHIP_CV2 */
  else /* if ((pCavalry != NULL) && (puArg != NULL)) */
  {
    prt_err("[error] : ioctl_cavalry_fma_run() : CAVALRY/FMA only available on CV2\n");
    retval = -EINVAL;
  } /* if ((pCavalry != NULL) && (puArg != NULL)) */
#endif /* ?CHIP_CV2 */

  return retval;

} /* ioctl_cavalry_fex_run() */

void krn_cavalry_system_set_trace_daddr(struct ambarella_cavalry *pCavalry, uint32_t cavalry_trace_daddr)
{
  if (pCavalry != NULL)
  {
    if (cavalry_trace_daddr != 0)
    {
      if (cavalry_trace_daddr != pCavalry->cavalry_trace_daddr)
      {
        pCavalry->cavalry_trace_daddr = cavalry_trace_daddr;
        pCavalry->vpCavalryTrace      = ambacv_c2v(cavalry_trace_daddr);
        cavalry_initialize_traceinfo(pCavalry);
      } /* if (cavalry_trace_daddr != pCavalry->cavalry_trace_daddr) */
    } /* if (cavalry_trace_daddr != 0) */
  } /* if (pCavalry != NULL) */
} /* krn_cavalry_system_set_trace_daddr() */

uint32_t krn_cavalry_system_get_trace_daddr(struct ambarella_cavalry *pCavalry)
{
  uint32_t  retval;
  if (pCavalry != NULL)
  {
    retval = pCavalry->cavalry_trace_daddr;
  } /* if (pCavalry != NULL) */
  else /* if (pCavalry == NULL) */
  {
    retval = 0;;
  } /* if (pCavalry == NULL) */

  return retval;

} /* krn_cavalry_system_get_trace_daddr() */

