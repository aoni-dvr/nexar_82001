/*
 * ambarella/armhsm.c
 *
 * ARM HSM device driver
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/workqueue.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/dma-mapping.h>

#include "armhsm.h"
#include "amalgam.h"
#include "callreply.h"
#include "armhsm_private.h"
#include "comm.h"

#include <struct.c>

#define ARMHSM_MAJOR 0	/* Dynamically allocated */

static struct armhsm G_armhsm;

static struct hsm_ticket G_hsm_ticket[ARMHSM_TICKET_NUM];

unsigned int my_hsm_domain(void)
{
#if defined(CONFIG_AMALGAM)
	return am_domid();
#else
	return G_armhsm.domain;  /* FIXME! */
#endif
}
EXPORT_SYMBOL(my_hsm_domain);

/*
 * Wait reply from HSM
 */
static int wait_hsm_reply(struct hsm_comm *comm,
			  struct semaphore *sem, unsigned int timeout)
{
	uint32_t heartbeat0, heartbeat1;
	int ret;

	/*
	 * Get a random number for the first heartbeat0 value. This
	 * effectively means that we'll get a retry-once in the loop
	 * below.
	 */
	get_random_bytes(&heartbeat0, sizeof(uint32_t));

	for (;;) {
		ret = down_timeout(sem, msecs_to_jiffies(timeout));
		if (ret == 0) {
			break;
		}

		heartbeat1 = msgq_get_heartbeat(comm->rx_mq);
		if (heartbeat0 != heartbeat1)  {
			heartbeat0 = heartbeat1;
		} else {
			break;
		}
	}

	return ret;
}

/*
 * Initialize hsm ticket structure and semaphore
 */
static void init_hsm_ticket(void)
{
	struct hsm_ticket *ticket = G_hsm_ticket;
	int i;

	memset(G_hsm_ticket, 0x0, sizeof(G_hsm_ticket));

	for (i = 0; i < ARMHSM_TICKET_NUM; i++) {
		sema_init(&ticket[i].sem, 0);
	}
}

static void reset_hsm_ticket(void)
{
	struct hsm_ticket *ticket = G_hsm_ticket;
	int i;

	for (i = 0; i < ARMHSM_TICKET_NUM; i++) {
		ticket->state = ARMHSM_TICKET_UNUSED;
	}
}

/*
 * Find unused hsm ticket
 */
static struct hsm_ticket *find_unused_hsm_ticket(void)
{
	struct hsm_ticket *ticket = G_hsm_ticket;
	int i;

	for (i = 0; i < ARMHSM_TICKET_NUM; i++) {
		if (ticket[i].state == ARMHSM_TICKET_UNUSED) {
			memset(&ticket[i].reply, 0x0, sizeof(ticket[i].reply));
			return &ticket[i];
		}
	}

	return NULL;
}

/*
 * Wait and find unused hsm ticket
 */
static struct hsm_ticket *wait_unused_hsm_ticket(unsigned int ms)
{
	struct armhsm *armhsm = &G_armhsm;
	struct hsm_ticket *ticket;
	long ret;

	do {
		mutex_lock(&armhsm->comm_lock);
		ticket = find_unused_hsm_ticket();
		if (ticket != NULL) {
			/* Found ticket */
			ticket->state = ARMHSM_TICKET_USED;
#if defined(DEBUG_HSM_TICKET)
			pr_err("%s: found ticket (data=%u)\n",
			       __func__, ticket->data);
#endif
			mutex_unlock(&armhsm->comm_lock);
			break;
		}

		armhsm->ticket_waitcnt++;
		mutex_unlock(&armhsm->comm_lock);

		ret = wait_hsm_reply(armhsm->comm, &armhsm->hsm_sem, ms);
		if (ret != 0) {
			/* Not found ticket */
			break;
		}
	} while (ticket == NULL);

	return ticket;
}

/*
 * Find used hsm ticket after orchsm reply
 */
static struct hsm_ticket *find_used_hsm_ticket(unsigned int data)
{
	struct hsm_ticket *ticket = G_hsm_ticket;
	int i;

	for (i = 0; i < ARMHSM_TICKET_NUM; i++) {
		if ((ticket[i].state == ARMHSM_TICKET_USED) &&
		    (ticket[i].data == data)) {
#if defined(DEBUG_HSM_TICKET)
			pr_err("%s: hsm call done with ticket id=%d, data=%u\n",
			       __func__, i, data);
#endif
			return &ticket[i];
		}
	}

	return NULL;
}

/*
 * Set ticket state to unused and wake up waiting process
 */
static void set_hsm_ticket_done(struct hsm_ticket *ticket)
{
	struct armhsm *armhsm = &G_armhsm;

	ticket->state = ARMHSM_TICKET_UNUSED;
#if defined(DEBUG_HSM_TICKET)
	pr_err("%s data=%u\n", __func__, ticket->data);
#endif
	if (armhsm->ticket_waitcnt > 0) {
		up(&armhsm->hsm_sem);
		armhsm->ticket_waitcnt--;
#if defined(DEBUG_HSM_TICKET)
		pr_err("%s: waitcnt %u wakeup\n", __func__, armhsm->ticket_waitcnt);
#endif
	}
}

/*
 * Send interrupt from ARM to HORC
 */
static void armhsm_irq_send(struct armhsm *armhsm)
{
#if defined(CONFIG_AMALGAM)
	am_interrupt(INTERRUPT_CLIENT_HSM, armhsm->clnthsm_irq);
#else
	regmap_update_bits(armhsm->ca78ae_scr_reg, 0,
			   1 << armhsm->domain, 1 << armhsm->domain);
#endif
}

/*
 * This function is used for executing a remote command on the ORCHSM.
 * It is used by the symbols exported as API to enable third-party
 * to write kernel-space code to communicate directly with the HSM, by-passing
 * the Cryptoki v2.4 API provided in the user land.
 */
int armhsm_call(uint32_t id, void *arg)
{
	int ret = 0;
	struct armhsm *armhsm = &G_armhsm;
	struct hsm_call call;
	int unlock_needed = 0;
	struct hsm_ticket *ticket;

	/*
	 * Set up
	 */
	call.id = id;
	call.arg = (hsm_ptr_t) arg;

	/*
	 * Wait unused ticket
	 */
	ticket = wait_unused_hsm_ticket(ARMHSM_WAIT_TIMEOUT);
	if (ticket == NULL) {
		ret = -ETIME;
		pr_err("%s: tid %u wait unused ticket timeout! (%d)\n",
		       __func__, task_pid_nr(current), ret);
		return ret;
	}

	mutex_lock(&armhsm->comm_lock);
	ret = kernel_call_incoming(&call);
	if (ret < 0) {
		pr_err("%s: kernel_call_incoming error %d\n", __func__, ret);
		unlock_needed = 1;
		goto done;
	}

	/*
	 * Queue for out-going
	 */
	finalize_call(&call, ticket);

	ret = comm_msg_push(armhsm->comm, &call, 1);
	if (ret != 1) {
		pr_err("%s: comm_msg_push failed! (%d)\n", __func__, ret);
		unlock_needed = 1;
		ret = -EBUSY;
		goto done;
	}

	armhsm_irq_send(armhsm);
	mutex_unlock(&armhsm->comm_lock);

	/*
	 * Wait for interrupt
	 */
	ret = wait_hsm_reply(armhsm->comm, &ticket->sem, ARMHSM_WAIT_TIMEOUT);
	if (ret != 0) {
		pr_err("%s: wait hsm reply timeout! (%d)\n", __func__, ret);
		ret = -ETIME;
	}

	/*
	 * Pop from in-coming
	 */
	mutex_lock(&armhsm->comm_lock);
	ret = kernel_reply_outgoing(&call, &ticket->reply);
	if (ret < 0) {
		pr_err("%s: kernel_reply_outgoing error %d\n", __func__, ret);
		unlock_needed = 1;
		set_hsm_ticket_done(ticket);
		goto done;
	}

	set_hsm_ticket_done(ticket);
	mutex_unlock(&armhsm->comm_lock);

done:
	if (unlock_needed == 1) {
		mutex_unlock(&armhsm->comm_lock);
	}

	if (ret != 0) {
		return ret;
	}

	return ticket->reply.ret;
}

static int ioctl_armhsm_call(struct file *file, unsigned long arg)
{
	int ret = 0;
	int unlock_needed = 0;
	struct hsm_call call;
	struct armhsm *armhsm;
#if !ARMHSM_USE_DMA
	struct mem_pool pool;
	struct hsm_ptr_store ptr_store;
#else
	struct armhsm_dma *dma;
#endif
	struct hsm_ticket *ticket;

	armhsm = (struct armhsm *) file->private_data;

#if ARMHSM_USE_DMA
	dma = kzalloc(sizeof(struct armhsm_dma), GFP_KERNEL);
	if (!dma)
		return -ENOMEM;
#endif

	if (copy_from_user(&call, (void *) arg, sizeof(call))) {
		return -EFAULT;
	}

	/*
	 * Wait unused ticket
	 */
	ticket = wait_unused_hsm_ticket(ARMHSM_WAIT_TIMEOUT);
	if (ticket == NULL) {
		ret = -ETIME;
		pr_err("%s: tid %u wait unused ticket timeout! (%d)\n",
		       __func__, task_pid_nr(current), ret);
		return ret;
	}

	mutex_lock(&armhsm->comm_lock);
#if ARMHSM_USE_DMA
	/* Package the call ensure HSM can correctly access */
	ret = package_call_incoming(armhsm, &call, dma);
	if (ret < 0) {
		dev_err(armhsm->dev, "%s: package incoming error %d\n", __func__, ret);
		mutex_unlock(&armhsm->comm_lock);
		goto done;
	}
#else
	ret = handle_call_incoming(&call,
				   &ptr_store, &pool);
	if (ret < 0) {
		pr_err("%s: handle_call_incoming error %d\n", __func__, ret);
		unlock_needed = 1;
		goto done;
	}
#endif

	finalize_call(&call, ticket);

	ret = comm_msg_push(armhsm->comm, &call, 1);
	if (ret != 1) {
		pr_err("%s: comm_msg_push failed! (%d)\n", __func__, ret);
		ret = -EBUSY;
		unlock_needed = 1;
		goto done;
	}

	armhsm_irq_send(armhsm);
	mutex_unlock(&armhsm->comm_lock);

	ret = wait_hsm_reply(armhsm->comm, &ticket->sem, ARMHSM_WAIT_TIMEOUT);
	if (ret != 0) {
		ret = -ETIME;
		pr_err("%s: tid %u wait hsm reply timeout! (%d)\n",
		       __func__, task_pid_nr(current), ret);
	}

	mutex_lock(&armhsm->comm_lock);
#if ARMHSM_USE_DMA
	ret = package_reply_outgoing(armhsm, &call, &ticket->reply, dma);
	if (ret < 0) {
		dev_err(armhsm->dev, "%s: package outcoming error %d\n", __func__, ret);
		mutex_unlock(&armhsm->comm_lock);
		goto done;
	}
#else
	ret = handle_reply_outgoing(&call, &ticket->reply, &ptr_store, &pool);
	if (ret < 0) {
		pr_err("%s: handle_reply_outgoing error %d\n", __func__, ret);
		unlock_needed = 1;
		set_hsm_ticket_done(ticket);
		goto done;
	}

#endif
	set_hsm_ticket_done(ticket);
	mutex_unlock(&armhsm->comm_lock);

	if (copy_to_user((void *) arg, &call, sizeof(call))) {
		ret = -EFAULT;
		goto done;
	}

done:

	if (unlock_needed == 1) {
		mutex_unlock(&armhsm->comm_lock);
	}

#if !ARMHSM_USE_DMA
	cleanup_hsm_mem(&pool, &ptr_store);
#else
	kfree(dma);
#endif
	return ret;
}


static long ioctl_nvm_provider(struct file *file, unsigned long arg)
{
	long ret = 0;
	struct armhsm *armhsm;
	struct hsmnvm *hsmnvm;
	struct hsm_wait_n_send_req *nvm_req;
	struct hsm_wait_n_send_req *user_pro_req;
	struct hsm_wait_n_send_req pro_req;

	armhsm = (struct armhsm *) file->private_data;
	hsmnvm = &armhsm->hsmnvm;
	nvm_req = &hsmnvm->nvm_req;

	if (copy_from_user(&pro_req, (void *) arg, sizeof(pro_req))) {
		return -EFAULT;
	}

	/*
	 * Copy reply objects from userspace to input objects from HSM,
	 * so that changes propagate back to the HSM.
	 */
	user_pro_req = (struct hsm_wait_n_send_req *) arg;
	if (user_pro_req->req & HSM_PROVIDER_REQ_READ_RPMB) {
		if (copy_from_user(nvm_req->in_r_rpmb,
				   user_pro_req->re_r_rpmb,
				   sizeof(struct nvmfs_read_rpmb))) {
			return -EFAULT;
		}
	}

	if (user_pro_req->req & HSM_PROVIDER_REQ_WRITE_RPMB) {
		if (copy_from_user(nvm_req->in_w_rpmb,
				   user_pro_req->re_w_rpmb,
				   sizeof(struct nvmfs_write_rpmb))) {
			return -EFAULT;
		}
	}

	if (user_pro_req->req & HSM_PROVIDER_REQ_READ_OBJ) {
		if (copy_from_user(nvm_req->in_r_obj,
				   user_pro_req->re_r_obj,
				   sizeof(struct nvmfs_read_obj))) {
			return -EFAULT;
		}
	}

	if (user_pro_req->req & HSM_PROVIDER_REQ_WRITE_OBJ) {
		if (copy_from_user(nvm_req->in_w_obj,
				   user_pro_req->re_w_obj,
				   sizeof(struct nvmfs_write_obj))) {
			return -EFAULT;
		}
	}

	/*
	 * Handle nvm provider reply
	 */

	if (nvm_req->info && pro_req.info) {
		if (copy_from_user(nvm_req->info,
				   (void *) pro_req.info, sizeof(pro_req))) {
			return -EFAULT;
		}
	} else {
		/* Get nvm info failed, set output ptr to NULL */
		nvm_req->info = NULL;
	}

	nvm_req->re_r_rpmb = pro_req.re_r_rpmb;
	nvm_req->re_w_rpmb = pro_req.re_w_rpmb;
	nvm_req->re_r_obj = pro_req.re_r_obj;
	nvm_req->re_w_obj = pro_req.re_w_obj;

	if (pro_req.req & HSM_PROVIDER_REQ_MASK) {
		up(&hsmnvm->reply_sem);
	}

	/*
	 * Wait nvm client request
	 */

	ret = down_interruptible(&hsmnvm->req_sem);
	if (ret != 0) {
		ret = -ETIME;
	}

	pro_req.req = nvm_req->req;
	pr_debug("%s: pid %u get nvm client request 0x%lx\n",
		 __func__, task_pid_nr(current), pro_req.req);

	if (nvm_req->req & HSM_PROVIDER_REQ_READ_RPMB) {
		if (copy_to_user(pro_req.in_r_rpmb,
				 nvm_req->in_r_rpmb,
				 sizeof(struct nvmfs_read_rpmb))) {
			return -EFAULT;
		}
	}

	if (nvm_req->req & HSM_PROVIDER_REQ_WRITE_RPMB) {
		if (copy_to_user(pro_req.in_w_rpmb,
				 nvm_req->in_w_rpmb,
				 sizeof(struct nvmfs_write_rpmb))) {
			return -EFAULT;
		}
	}

	if (nvm_req->req & HSM_PROVIDER_REQ_READ_OBJ) {
		if (copy_to_user(pro_req.in_r_obj,
				 nvm_req->in_r_obj,
				 sizeof(struct nvmfs_read_obj))) {
			return -EFAULT;
		}
	}

	if (nvm_req->req & HSM_PROVIDER_REQ_WRITE_OBJ) {
		if (copy_to_user(pro_req.in_w_obj,
				 nvm_req->in_w_obj,
				 sizeof(struct nvmfs_write_obj))) {
			return -EFAULT;
		}
	}

	if (copy_to_user((void *) arg, &pro_req, sizeof(pro_req))) {
		return -EFAULT;
	}

	return ret;
}

static long ioctl_register_provider(struct file *file, unsigned long arg)
{
	long ret = 0;
	int order;
	struct armhsm *armhsm;
	struct hsmnvm *hsmnvm;
	struct hsm_register_nvm_pro *nvm_pro;
	void *nvmreq_buf;

	armhsm = (struct armhsm *) file->private_data;
	hsmnvm = &armhsm->hsmnvm;
	nvm_pro = &hsmnvm->nvm_pro;

	if (hsmnvm->pid > 0) {
		/* already registered */
		return -EPERM;
	}

	if (copy_from_user(nvm_pro, (void *) arg, sizeof(*nvm_pro))) {
		return -EFAULT;
	}

	hsmnvm->pid = task_pid_nr(current);
	armhsm->pro_comm = comm_init_pro(nvm_pro);
	if (armhsm->pro_comm == NULL) {
		return -EBUSY;
	}

	for (order = 0; (4096 << order) < nvm_pro->nvmreq_size; order++);
	nvmreq_buf = (void *) __get_free_pages(GFP_KERNEL, order);
	if (nvmreq_buf == NULL) {
		return -ENOMEM;
	}

	nvm_pro->nvmreq_phyaddr = virt_to_phys(nvmreq_buf);

	if (copy_to_user((void *) arg, nvm_pro, sizeof(*nvm_pro))) {
		return -EFAULT;
	}

	return ret;
}

static long ioctl_unregister_provider(struct file *file, unsigned long arg)
{
	long ret = 0;
	struct armhsm *armhsm;
	struct hsmnvm *hsmnvm;
	struct hsm_register_nvm_pro *nvm_pro;
	void *nvmreq_buf;
	pid_t pid;

	armhsm = (struct armhsm *) file->private_data;
	hsmnvm = &armhsm->hsmnvm;
	nvm_pro = &hsmnvm->nvm_pro;

	if (hsmnvm->pid <= 0) {
		/* not registered */
		return -EPERM;
	}

	pid = task_pid_nr(current);
	if (pid != hsmnvm->pid) {
		/* invalid pid */
		return -EPERM;
	}

	if (comm_deinit_pro(nvm_pro) < 0) {
		pr_err("remove provider comm queue failed!\n");
		ret = -EBUSY;
	}

	if (nvm_pro->nvmreq_phyaddr) {
		nvmreq_buf = phys_to_virt(nvm_pro->nvmreq_phyaddr);
	} else {
		nvmreq_buf = NULL;
	}

	if (nvmreq_buf) {
		int order;

		for (order = 0;
		     (4096 << order) < nvm_pro->nvmreq_size;
		     order++);

		free_pages((unsigned long)nvmreq_buf, order);
	}

	memset(nvm_pro, 0x0, sizeof(*nvm_pro));
	hsmnvm->pid = 0;

	return ret;
}

static ssize_t armhsm_read(struct file *file, char __user *buf, size_t count,
                           loff_t *ppos)
{
	return 0;
}

static ssize_t armhsm_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *ppos)
{
	return 0;
}

static long armhsm_ioctl(struct file *file, unsigned int cmd,
			 unsigned long arg)
{
	long ret = 0;

#if defined(CONFIG_AMALGAM)
	if (am_is_connected() == 0) {
		pr_err("amalgam is not connected\n");
		return -EPERM;
	}
#endif

	switch (cmd) {
	case ARMHSM_API_HASH:
		if (copy_to_user((uint32_t *) arg, &hsm_api_hash,
				 sizeof(uint32_t))) {
			ret = -EIO;
		} else {
			ret = 0;
		}
		break;
	case ARMHSM_RUN_HSM_CMD:
		ret = ioctl_armhsm_call(file, arg);
		break;
	case ARMHSM_REGISTER_NVM_PRO:
		ret = ioctl_register_provider(file, arg);
		break;
	case ARMHSM_UNREGISTER_NVM_PRO:
		ret = ioctl_unregister_provider(file, arg);
		break;
	case ARMHSM_PROVIDE_HSM:
		ret = ioctl_nvm_provider(file, arg);
		break;
	default:
		ret = -EINVAL;
		break;
	};

	return ret;
}

static int armhsm_open(struct inode *inode, struct file *file)
{
#if defined(CONFIG_AMALGAM)
	if (am_is_connected() == 0) {
		am_connect();
	}
#endif

	file->private_data = &G_armhsm;

	return 0;
}

static int armhsm_release(struct inode *inode, struct file *file)
{
	unsigned long arg = 0;
	struct hsm_reply reply;
	struct armhsm *armhsm;

	armhsm = (struct armhsm *) file->private_data;

	ioctl_unregister_provider(file, arg);

	/*
	 * Reset hsm ticket state and drop unhandled reply in queue
	 */
	reset_hsm_ticket();
	while (comm_msg_pop(armhsm->comm, &reply, 1) != 0);

	return 0;
}

static struct file_operations armhsm_fops =
{
	.owner =		THIS_MODULE,
	.read =			armhsm_read,
	.write =		armhsm_write,
	.unlocked_ioctl =	armhsm_ioctl,
	.open =			armhsm_open,
	.release =		armhsm_release,
};

static int armhsm_major = ARMHSM_MAJOR;

static struct class *armhsm_class = NULL;

static const struct of_device_id armhsm_of_match = {
	.compatible = "ambarella,hsm",
};

/*
 * Instance number of comm queue used by ARM client
 */
#define IRQ_STATUS_BITMASK (			\
		(0x1 << HSM_SHMEM_COMM) |	\
		(0x1 << HSM_DRAM_COMM0)		\
		)

#if !defined(ARMHSM_ATF_SHMEM)
static irqreturn_t armhsm_irq0_handler(int irq, void *data)
{
	struct armhsm *armhsm = (struct armhsm *) data;
	void *msg;
	uint32_t pro_nmsg = 0;

	if (armhsm->pro_comm) {
		comm_get_msg_pop(armhsm->pro_comm, &msg, &pro_nmsg);
	}

	if (pro_nmsg > 0) {
		queue_work(armhsm->nvmclnt_workqueue,
                           &armhsm->nvmclnt_work);
	} else {
		queue_work(armhsm->hsm_workqueue, &armhsm->hsm_work);
	}

	return IRQ_HANDLED;
}
#endif

static irqreturn_t armhsm_irq1_handler(int irq, void *data)
{
	struct armhsm *armhsm = (struct armhsm *) data;
	void *msg;
	uint32_t pro_nmsg = 0;

	if (armhsm->pro_comm) {
		comm_get_msg_pop(armhsm->pro_comm, &msg, &pro_nmsg);
	}

	if (pro_nmsg > 0) {
		queue_work(armhsm->nvmclnt_workqueue,
                           &armhsm->nvmclnt_work);
	} else {
		queue_work(armhsm->hsm_workqueue, &armhsm->hsm_work);
	}

	return IRQ_HANDLED;
}

static void hsm_work_func(struct work_struct *work)
{
	struct armhsm *armhsm = &G_armhsm;
	struct hsm_ticket *ticket;
	struct hsm_reply reply;

	mutex_lock(&armhsm->comm_lock);
	while (comm_msg_pop(armhsm->comm, &reply, 1) == 1) {
		ticket = find_used_hsm_ticket(reply.seq);
		if (ticket == NULL) {
			pr_err("%s: can't find used hsm ticket, data = %u\n",
			       __func__, reply.seq);
			continue;
		}

		memcpy(&ticket->reply, &reply, sizeof(reply));
		up(&ticket->sem);
	}
	mutex_unlock(&armhsm->comm_lock);
}

static void nvmclnt_work_func(struct work_struct *work)
{
	struct hsm_call call;
	struct hsm_reply reply;
	struct armhsm *armhsm = &G_armhsm;
	API_FUNC f;
	int ret = 0;

	/*
	 * Get incoming msg
	 */
	mutex_lock(&armhsm->comm_lock);
	ret = comm_msg_pop(armhsm->pro_comm, &call, 1);
	mutex_unlock(&armhsm->comm_lock);
	if (ret != 1) {
		pr_err("nvmclnt comm_msg_pop failed! (%d)\n", ret);
		goto done;
	}

	ret = nvmclnt_call_incoming(&call);
	if (ret != 0) {
		pr_err("nvmclnt_call_incoming failed! (%d)\n", ret);
		goto done;
	}

	/*
	 * Get provider func params, execute it and setup reply msg
	 */
	pr_debug("%u: %s\n", call.seq, hsm_api_names[call.id]);
	f = api_id2f[call.id - NVMFS_INFO_ID].api_func;

	reply.ret = f((void *) call.arg);

	ret = nvmclnt_reply_outgoing(&call, &reply);
	if (ret != 0) {
		pr_err("nvmclnt_reply_outgoing failed! (%d)\n", ret);
		goto done;
	}

	/*
	 * Send reply msg
	 */
	mutex_lock(&armhsm->comm_lock);
	ret = comm_msg_push(armhsm->pro_comm, &reply, 1);
	mutex_unlock(&armhsm->comm_lock);
	if (ret != 1) {
		pr_err("nvmclnt comm_msg_push failed! (%d)\n", ret);
		ret = -EBUSY;
		goto done;
	}

done:

	armhsm_irq_send(armhsm);

	return;
}

static ssize_t hsminfo_read(struct file *file, char __user *ubuf,
			    size_t count, loff_t *ppos)
{
	struct hsm_info *info;
	char buf[1024];
	int len = 0;
	int ret = 0;
	uint32_t heartbeat;
	dma_addr_t info_phy;
	struct armhsm *armhsm = &G_armhsm;

	info = dma_alloc_coherent(armhsm->dev, sizeof(*info),
                                  &info_phy, GFP_KERNEL);
	if (IS_ERR(info))
		return -ENOMEM;

	if(*ppos > 0 || count < sizeof(buf))
		goto done;

	ret = AmbaHSM_GetHSMInfo((struct hsm_info *)info_phy);
	if (ret == 0) {
		unsigned int i;

		len += sprintf(buf + len,
			       "ORCHSM:\n");
		len += sprintf(buf + len, "firmware:      %s\n",
			       info->fwloaded ? "loaded" : "not loaded");
		len += sprintf(buf + len, "sw version:    %u.%u.%u\n",
			       ((info->sw_ver & 0x00ff0000) >> 16),
			       ((info->sw_ver & 0x0000ff00) >> 8),
			       (info->sw_ver & 0x000000ff));
		len += sprintf(buf + len,
			       "sha1:  ");
		for (i = 0; i < sizeof(info->sha1); i++) {
			len += sprintf(buf + len,
				       "%.2x", info->sha1[i]);
		}
		len += sprintf(buf + len, "\n");
		len += sprintf(buf + len, "build: %s\n", info->info);
	}

	len += sprintf(buf + len, "Heart-beats:\n");
	if ((arm0_comm.tx_mq != NULL)) {
		heartbeat = msgq_get_heartbeat(arm0_comm.tx_mq);
		len += sprintf(buf + len, "HSM API call:   %u\n", heartbeat);
	}
	if ((arm0_comm.rx_mq != NULL)) {
		heartbeat = msgq_get_heartbeat(arm0_comm.rx_mq);
		len += sprintf(buf + len, "HSM API reply:  %u\n", heartbeat);
	}
	if ((arm0_pro_comm.tx_mq != NULL)) {
		heartbeat = msgq_get_heartbeat(arm0_pro_comm.tx_mq);
		len += sprintf(buf + len, "Provider call:  %u\n", heartbeat);
	}
	if ((arm0_pro_comm.rx_mq != NULL)) {
		heartbeat = msgq_get_heartbeat(arm0_pro_comm.rx_mq);
		len += sprintf(buf + len, "Provider reply: %u\n", heartbeat);
	}

	if (copy_to_user(ubuf, buf, len)) {
		ret = -EFAULT;
		len = 0;
		goto done;
	}

done:

	if (info)
		dma_free_coherent(armhsm->dev, sizeof(*info),
				  info, info_phy);

	*ppos = len;

	return len;
}

static struct proc_ops hsminfo_ops = {
	.proc_read = hsminfo_read,
};

static struct proc_dir_entry *proc_hsminfo = NULL;

static ssize_t hsmlog_read(struct file *file, char __user *ubuf,
			   size_t count, loff_t *ppos)
{
	struct armhsm *armhsm = &G_armhsm;
	int len = 0;
	int ret = 0;

	if (armhsm->logbuf.buf == NULL)
		goto done;

	if (*ppos > armhsm->logbuf.size)
		goto done;

	len = armhsm->logbuf.size - *ppos;
	if (len == 0) {
		goto done;
	} else if (len > count) {
		len = count;
	}

#if defined(CONFIG_AMALGAM)
	am_memrd(armhsm->logbuf.wp, sizeof(uint32_t));
	am_memrd(armhsm->logbuf.buf + *ppos, len);
#endif

	/* Scan until trailing position is not a NULL character */
	while (len > 0) {
		if (armhsm->logbuf.buf[*ppos + len - 1] != '\0') {
			break;
		} else {
			len--;
		}
	}

	if (len == 0) {
		goto done;
	}

	if (copy_to_user(ubuf, armhsm->logbuf.buf + *ppos, len)) {
		ret = -EFAULT;
		len = 0;
		goto done;
	}

done:

	*ppos += len;

	if (ret < 0)
		return ret;
	else
		return len;
}

static struct proc_ops hsmlog_ops = {
	.proc_read = hsmlog_read,
};

static struct proc_dir_entry *proc_hsmlog = NULL;

static ssize_t hsmlogctl_read(struct file *file, char __user *ubuf,
			      size_t count, loff_t *ppos)
{
	struct armhsm *armhsm = &G_armhsm;
	int len = 0;
	int ret = 0;
	uint32_t ctl[3];

	if (armhsm->logbuf.buf == NULL)
		goto done;

	if ((*ppos >= sizeof(ctl)) || (count == 0)) {
		goto done;
	}

#if defined(CONFIG_AMALGAM)
	am_memrd(armhsm->logbuf.wp, sizeof(uint32_t));
	am_memrd(armhsm->logbuf.rp, sizeof(uint32_t));
#endif
	ctl[0] = *(armhsm->logbuf.wp);
	ctl[1] = *(armhsm->logbuf.rp);
	ctl[2] = armhsm->logbuf.size;

	if ((*ppos + count) > sizeof(ctl)) {
		len = sizeof(ctl) - *ppos;
	} else {
		len = count;
	}

	if (copy_to_user(ubuf, ((uint8_t *) ctl) + *ppos, len)) {
		ret = -EFAULT;
		len = 0;
		goto done;
	}

	*ppos += len;

done:

	if (ret < 0)
		return ret;
	else
		return len;
}

static ssize_t hsmlogctl_write(struct file *file, const char __user *ubuf,
			       size_t count, loff_t *ppos)
{
	struct armhsm *armhsm = &G_armhsm;
	int len = 0;
	int ret = 0;
	uint32_t ctl[2];

	if (armhsm->logbuf.buf == NULL)
		goto done;

	if ((*ppos >= sizeof(ctl)) || (count == 0)) {
		ret = -EFAULT;
		goto done;
	}

#if defined(CONFIG_AMALGAM)
	am_memrd(armhsm->logbuf.wp, sizeof(uint32_t));
	am_memrd(armhsm->logbuf.rp, sizeof(uint32_t));
#endif
	ctl[0] = *(armhsm->logbuf.wp);
	ctl[1] = *(armhsm->logbuf.rp);

	if ((*ppos + count) > sizeof(ctl)) {
		len = sizeof(ctl) - *ppos;
	} else {
		len = count;
	}

	if (copy_from_user(ctl + *ppos, ubuf, len)) {
		ret = -EFAULT;
		len = 0;
		goto done;
	}

	*(armhsm->logbuf.rp) = ctl[1];
	len = count;

#if defined(CONFIG_AMALGAM)
	am_memwr(armhsm->logbuf.rp, sizeof(uint32_t));
#endif

done:

	if (ret < 0)
		return ret;
	else
		return len;
}

static struct proc_ops hsmlogctl_ops = {
	.proc_read = hsmlogctl_read,
	.proc_write = hsmlogctl_write,
};

static struct proc_dir_entry *proc_hsmlogctl = NULL;

#if ARMHSM_USE_DMA
static int armhsm_set_debug(struct armhsm *armhsm)
{
	int rval;
	struct hsm_debug *hsm_debug;
	dma_addr_t hsm_debug_dma;

	/*
	 * Allocate log buffer for HSM
	 */
	armhsm->logbuf.buf = dma_alloc_coherent(armhsm->dev, HSM_LOGBUF_SIZE + 4096,
                                                &armhsm->logbuf_phys_addr, GFP_KERNEL);
	if (armhsm->logbuf.buf == NULL) {
		dev_err(armhsm->dev, "%s: allocate HSM log buffer error\n", __func__);
		return -ENOMEM;
	}

	armhsm->logbuf.size = HSM_LOGBUF_SIZE;
	armhsm->logbuf.wp = (uint32_t *)
		(armhsm->logbuf.buf + HSM_LOGBUF_SIZE);
	armhsm->logbuf.rp = (uint32_t *)
		(armhsm->logbuf.wp + sizeof(uint32_t));


	/*
	 * Tell the HSM where our print log buffer is.
	 */
	hsm_debug = dma_alloc_coherent(armhsm->dev, sizeof(struct hsm_debug),
				       &hsm_debug_dma, GFP_KERNEL);
	if (hsm_debug == NULL) {
		dev_err(armhsm->dev, "%s: allocate hsm debug error\n", __func__);
		return -ENOMEM;
	}
	hsm_debug->enable = HSM_DEBUG_LOG;
	hsm_debug->logbuf.buf = (hsm_ptr_t)(armhsm->logbuf_phys_addr);
	hsm_debug->logbuf.wp = (hsm_ptr_t)(armhsm->logbuf_phys_addr + HSM_LOGBUF_SIZE);
	hsm_debug->logbuf.rp = (hsm_ptr_t)(hsm_debug->logbuf.rp + sizeof(uint32_t));
	hsm_debug->logbuf.size = armhsm->logbuf.size;
	rval = AmbaHSM_SetDebug((struct hsm_debug *)hsm_debug_dma);
	if (rval != 0) {
		pr_err("AmbaHSM_SetDebug failed (%d)!\n", rval);
		rval = -EFAULT;
	}
	dma_free_coherent(armhsm->dev, sizeof(struct hsm_debug),
			  hsm_debug, hsm_debug_dma);

	return rval;
}
#else
static int armhsm_set_debug(struct armhsm *armhsm)
{
	int rval;
	struct hsm_debug *hsm_debug;
	/*
	 * Allocate log buffer for HSM
	 */
	armhsm->logbuf.buf =
		kzalloc(HSM_LOGBUF_SIZE + 4096, GFP_KERNEL);
	if (armhsm->logbuf.buf == NULL) {
		return -ENOMEM;
	} else {
		armhsm->logbuf.size = HSM_LOGBUF_SIZE;
		armhsm->logbuf.wp = (uint32_t *)
			(armhsm->logbuf.buf + HSM_LOGBUF_SIZE);
		armhsm->logbuf.rp = (uint32_t *)
			(armhsm->logbuf.wp + sizeof(uint32_t));
	}

	/*
	 * Tell the HSM where our print log buffer is.
	 */
	hsm_debug = kzalloc(sizeof(*hsm_debug), GFP_KERNEL);
	if (hsm_debug == NULL) {
		kfree(armhsm->logbuf.buf);
		armhsm->logbuf.buf = NULL;
		armhsm->logbuf.size = 0;
		return -ENOMEM;
	}
	hsm_debug->enable = HSM_DEBUG_LOG;
	hsm_debug->logbuf.buf = (hsm_ptr_t)
		virt_to_phys(armhsm->logbuf.buf);
	hsm_debug->logbuf.wp = (hsm_ptr_t)
		virt_to_phys(armhsm->logbuf.wp);
	hsm_debug->logbuf.rp = (hsm_ptr_t)
		virt_to_phys(armhsm->logbuf.rp);
	hsm_debug->logbuf.size = armhsm->logbuf.size;
	rval = AmbaHSM_SetDebug(hsm_debug);
	if (rval != 0) {
		pr_err("AmbaHSM_SetDebug failed (%d)!\n", rval);
	}

	kfree(hsm_debug);

	return 0;
}

#endif
int armhsm_comm_instance(void)
{
	return G_armhsm.comm_inst;
}
struct comm_inst_info *armhsm_comm(void)
{
	return &G_armhsm.comm_info[G_armhsm.comm_inst];
}

static int __init armhsm_init(void)
{
	int ret = 0;
	const struct of_device_id *match;
	struct device_node *node;
	struct armhsm *armhsm;
	const char *domain_str;

	/* Create /proc/hsminfo */
	proc_hsminfo = proc_create("hsminfo", 0600, NULL, &hsminfo_ops);
	if (proc_hsminfo == NULL) {
		pr_err("Error in creating /proc/hsminfo\n");
		ret = -ENOMEM;
		goto done;
	}

	armhsm = &G_armhsm;
	memset(armhsm, 0x0, sizeof(struct armhsm));

	ret = register_chrdev(armhsm_major, "armhsm", &armhsm_fops);
	if (ret < 0) {
		printk("ARMHSM: Unable to register driver\n");
		goto done;
	}

	if (armhsm_major == 0) {
		armhsm_major = ret;
	}

	armhsm_class = class_create(THIS_MODULE, "armhsm");
	if (IS_ERR(armhsm_class)) {
		ret = PTR_ERR(armhsm_class);
		goto done;
	}

	armhsm->dev = device_create(armhsm_class, NULL,
				    MKDEV(armhsm_major, 0), NULL, "armhsm");
	armhsm->dev->coherent_dma_mask = DMA_BIT_MASK(32);
	armhsm->dev->dma_mask = &armhsm->dev->coherent_dma_mask;

	node = of_find_compatible_node(NULL, NULL, "ambarella,hsm");
	for_each_matching_node_and_match(node, &armhsm_of_match, &match) {
		if (of_device_is_available(node)) {
			armhsm->node = node;
			armhsm->armhsm_irq1 = irq_of_parse_and_map(node, 1);
			ret = request_irq(armhsm->armhsm_irq1,
					  armhsm_irq1_handler,
					  IRQF_TRIGGER_RISING | IRQF_SHARED,
					  "armhsm_irq1", armhsm);
			if (ret < 0) {
				goto done;
			}

#if !defined(ARMHSM_ATF_SHMEM)
			armhsm->armhsm_irq0 = irq_of_parse_and_map(node, 0);
			ret = request_irq(armhsm->armhsm_irq0,
					  armhsm_irq0_handler,
					  IRQF_TRIGGER_RISING | IRQF_SHARED,
					  "armhsm_irq0", armhsm);
			if (ret < 0) {
				goto done;
			}
#endif
		}
	}
	armhsm->ca78ae_scr_reg = syscon_regmap_lookup_by_compatible("ambarella,ca78ae-scratchpad");
	if (IS_ERR(armhsm->ca78ae_scr_reg)) {
		pr_err("%s: scr map failed! %ld\n", __func__, PTR_ERR(armhsm->ca78ae_scr_reg));
		ret = -1;
		goto done;
	}

	ret = of_reserved_mem_device_init_by_idx(armhsm->dev, armhsm->node, 0);
	if (ret) {
		dev_err(armhsm->dev, "Can't associate reserved memory %d\n", ret);
		goto done;
	}


#if defined(CONFIG_AMALGAM)
	armhsm->domain = am_domid();
#else
	ret = of_property_read_string(armhsm->node, "domain", &domain_str);
	if (ret) {
		dev_err(armhsm->dev, "property 'domain' can't be found, %d\n", ret);
		goto done;
	}

	if (!strcasecmp(domain_str, "SAFETY"))
		armhsm->domain = HSM_SAFETY_DOMAIN;
	else if (!strcasecmp(domain_str, "ASIL"))
		armhsm->domain = HSM_ASIL_DOMAIN;
	else if (!strcasecmp(domain_str, "QM"))
		armhsm->domain = HSM_QM_DOMAIN;
	else {
		dev_err(armhsm->dev, "Invalid ARM domain: %s\n", domain_str);
		goto done;
	}
#endif
	armhsm->clnthsm_irq = 0;  // Use irq 0 to interrupt hsm client

#if !defined(ARMHSM_ATF_SHMEM)
	armhsm->comm = comm_init_early(armhsm->domain);
	if (armhsm->comm == NULL) {
		pr_err("%s: comm_init_early failed!\n", __func__);
		ret = -1;
		goto done;
	}
#endif

	sema_init(&armhsm->hsm_sem, 0);
	mutex_init(&armhsm->comm_lock);

	/*
	 * Initiate hsm ticket
	 */
	init_hsm_ticket();
	INIT_WORK(&armhsm->hsm_work, hsm_work_func);
	armhsm->hsm_workqueue =
		create_singlethread_workqueue("hsm_wq");

	/*
	 * FIXME:
	 * This is a special flow before we move shmem queue in ATF.
	 * Switch to use domain comm queue in dram here.
	 */
	armhsm->comm = comm_init(armhsm, armhsm->domain, HSM_DRAM_COMM0);
	if (armhsm->comm == NULL) {
		pr_err("%s: comm_init failed!\n", __func__);
		ret = -EIO;
		goto done;
	}
	armhsm->comm_inst = HSM_DRAM_COMM0;

	if (my_hsm_domain() == HSM_SAFETY_DOMAIN) {
		if (hsmnvm_init(&armhsm->hsmnvm) < 0) {
			pr_err("%s: hsmnvm_init failed!\n", __func__);
			ret = -1;
			goto done;
		}

		INIT_WORK(&armhsm->nvmclnt_work, nvmclnt_work_func);
		armhsm->nvmclnt_workqueue =
			create_singlethread_workqueue("nvmclnt_wq");
	}

	if (my_hsm_domain() == HSM_SAFETY_DOMAIN) {
		armhsm_set_debug(armhsm);
	}

	if (my_hsm_domain() == HSM_SAFETY_DOMAIN) {
		/* Create /proc/hsmlog */
		proc_hsmlog = proc_create("hsmlog", 0600, NULL, &hsmlog_ops);
		if (proc_hsminfo == NULL) {
			pr_err("Error in creating /proc/hsmlog\n");
			ret = -ENOMEM;
			goto done;
		}

		/* Create /proc/hsmlogctl */
		proc_hsmlogctl = proc_create("hsmlogctl", 0600, NULL,
					     &hsmlogctl_ops);
		if (proc_hsmlogctl == NULL) {
			pr_err("Error in creating /proc/hsmlogctl\n");
			ret = -ENOMEM;
			goto done;
		}
	}


#if ARMHSM_USE_DMA
	dev_info(armhsm->dev, "Communicate with HSM using DMA coherent memory.\n");
#endif

done:
	if (ret == 0) {
		pr_info("armhsm driver init done\n");
	} else {
		if (proc_hsminfo != NULL) {
			remove_proc_entry("hsminfo", NULL);
			proc_hsminfo = NULL;
		}

		if (proc_hsmlog != NULL) {
			remove_proc_entry("hsmlog", NULL);
			proc_hsminfo = NULL;
		}

		if (proc_hsmlogctl != NULL) {
			remove_proc_entry("hsmlogctl", NULL);
			proc_hsmlogctl = NULL;
		}

                if (armhsm->dev) {
                        device_destroy(armhsm_class, MKDEV(armhsm_major, 0));
                }

                if (armhsm_class) {
                        class_destroy(armhsm_class);
                }
	}

	return ret;
}

static void __exit armhsm_exit(void)
{
	struct armhsm *armhsm = &G_armhsm;

	if (proc_hsminfo != NULL) {
		remove_proc_entry("hsminfo", NULL);
		proc_hsminfo = NULL;
	}

	if (proc_hsmlog != NULL) {
		remove_proc_entry("hsmlog", NULL);
		proc_hsminfo = NULL;
	}

	if (proc_hsmlogctl != NULL) {
		remove_proc_entry("hsmlogctl", NULL);
		proc_hsmlogctl = NULL;
	}

	if (my_hsm_domain() == HSM_SAFETY_DOMAIN) {
		hsmnvm_cleanup();
	}

#if ARMHSM_USE_DMA
	dma_free_coherent(armhsm->dev, HSM_LOGBUF_SIZE + 4096,
                          armhsm->logbuf.buf, armhsm->logbuf_phys_addr);
#else
	if (armhsm->logbuf.buf) {
		kfree(armhsm->logbuf.buf);
	}
#endif

	device_destroy(armhsm_class, MKDEV(armhsm_major, 0));
	class_destroy(armhsm_class);
	unregister_chrdev(armhsm_major, "armhsm");
#if !defined(ARMHSM_ATF_SHMEM)
	free_irq(armhsm->armhsm_irq0, armhsm);
#endif
	free_irq(armhsm->armhsm_irq1, armhsm);
}

module_init(armhsm_init);
module_exit(armhsm_exit);

MODULE_DESCRIPTION("ARMHSM Device Driver");
MODULE_AUTHOR("Charles Chiou, "
	      "Chien-Yang Chen, "
	      "and other developers at Ambarella Inc.");
MODULE_LICENSE("GPL");
