/*
 * ambarella/hsmnvm.c
 *
 * Non-Volatile Memory Provider
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#include <hsm/api/provider.h>

#include "armhsm.h"
#include "comm.h"

#define NVM_WAIT_MS		5000

struct hsmnvm_stat {
	unsigned int online;
	unsigned int n_get_nvm_info;
	unsigned int n_read_rpmb;
	unsigned int n_write_rpmb;
	unsigned int n_read_obj;
	unsigned int n_write_obj;
};

static struct hsmnvm_stat hsmnvm_stat;

static ssize_t hsmnvm_read(struct file *file, char __user *ubuf,
			   size_t count, loff_t *ppos)
{
	char buf[256];
	int len = 0;

	if ((*ppos == 2048) && (count > sizeof(struct hsmnvm_stat))) {
		memcpy(buf, &hsmnvm_stat, sizeof(hsmnvm_stat));
		len = sizeof(hsmnvm_stat);
	} else if ((*ppos == 0) && (count >= sizeof(buf))) {
		len += sprintf(buf + len, "online: %u\n", hsmnvm_stat.online);
		if ( hsmnvm_stat.online) {
			len += sprintf(buf + len, "n_get_nvm_info: %u\n",
				       hsmnvm_stat.n_get_nvm_info);
			len += sprintf(buf + len, "n_read_rpmb:    %u\n",
				       hsmnvm_stat.n_read_rpmb);
			len += sprintf(buf + len, "n_write_rpmb:   %u\n",
				       hsmnvm_stat.n_write_rpmb);
			len += sprintf(buf + len, "n_read_obj:     %u\n",
				       hsmnvm_stat.n_read_obj);
			len += sprintf(buf + len, "n_write_obj:    %u\n",
				       hsmnvm_stat.n_write_obj);
		}
	}

	if (copy_to_user(ubuf, buf, len))
		return -EFAULT;

	*ppos = len;

	return len;
}

static struct proc_ops hsmnvm_ops = {
	.proc_read = hsmnvm_read,
};

static struct proc_dir_entry *proc_hsmnvm = NULL;
static struct hsmnvm *hsmnvm = NULL;
static struct hsm_wait_n_send_req *nvm_req = NULL;
/*
 * Initialize the HSM NVM provider
 */
int hsmnvm_init(struct hsmnvm *_hsmnvm)
{
	memset(&hsmnvm_stat, 0x0, sizeof(hsmnvm_stat));

	/* Create /proc/hsmnvm */
	proc_hsmnvm = proc_create("hsmnvm", 0600, NULL, &hsmnvm_ops);
	if (proc_hsmnvm == NULL) {
		pr_err("Error in creating /proc/hsmnvm");
		return -1;
	}

	hsmnvm = _hsmnvm;
	nvm_req = &hsmnvm->nvm_req;

	sema_init(&hsmnvm->req_sem, 0);
	sema_init(&hsmnvm->reply_sem, 0);

	return 0;
}

/*
 * Clean up the HSM NVM provider
 */
int hsmnvm_cleanup(void)
{
	if (proc_hsmnvm != NULL) {
		remove_proc_entry("hsmnvm", NULL);
		proc_hsmnvm = NULL;
	}

	return 0;
}

/*
 * Set the online status of HSM NVM.
 */
int hsmnvm_set_state(unsigned int online)
{
	if (online) {
		hsmnvm_stat.online = 1;
	} else {
		memset(&hsmnvm_stat, 0x0, sizeof(hsmnvm_stat));
	}

	return 0;
}

/*
 * Return hsmnvm online status.
 */
static int is_hsmnvm_online(void)
{
	return hsmnvm_stat.online;
}

/*
 * Implements the GetNVMInfo() API
 */
int hsmnvm_get_nvm_info(struct nvmfs_info *arg)
{
	int ret;

	if (!is_hsmnvm_online()) {
		return -1;
	}

	if (arg == NULL) {
		return -1;
	}

	/*
	 * Send request to nvm provider
	 */
	nvm_req->req |= HSM_PROVIDER_REQ_INFO;
	nvm_req->info = arg;

	pr_debug("%s(0x%lx)\n", __func__, (unsigned long) virt_to_phys(arg));

	up(&hsmnvm->req_sem);

	/*
	 * Wait reply from nvm provider
	 */
	ret = down_timeout(&hsmnvm->reply_sem, msecs_to_jiffies(NVM_WAIT_MS));
	if (ret != 0) {
		ret = -ETIME;
	}

	/* Clear request */
	nvm_req->req &= ~HSM_PROVIDER_REQ_INFO;

	if (nvm_req->info) {
		/* nvm provider executes the request correctly */
		ret = 0;
		nvm_req->info = NULL;
	} else {
		ret = -EBUSY;
	}

	hsmnvm_stat.n_get_nvm_info++;

	return ret;
}

/*
 * Implements the Read_RPMB() API
 */
int hsmnvm_read_rpmb(struct nvmfs_read_rpmb *arg)
{
	int ret;
	void *virt_ptr;
	unsigned long phys_addr;

	if (!is_hsmnvm_online()) {
		return -1;
	}

	if (arg == NULL) {
		return -1;
	}

	/*
	 * Send request to nvm provider
	 */
	nvm_req->req |= HSM_PROVIDER_REQ_READ_RPMB;
	nvm_req->in_r_rpmb = arg;
	nvm_req->re_r_rpmb = NULL;

	virt_ptr = (void *) arg->dst;
	phys_addr = virt_to_phys(virt_ptr);
	arg->dst = (hsm_ptr_t) phys_addr;

	pr_debug("%s\n", __func__);

	up(&hsmnvm->req_sem);

	/*
	 * Wait reply from nvm provider
	 */
	ret = down_timeout(&hsmnvm->reply_sem, msecs_to_jiffies(NVM_WAIT_MS));
	if (ret != 0) {
		ret = -ETIME;
	}

	/* Clear request */
	nvm_req->req &= ~HSM_PROVIDER_REQ_READ_RPMB;

	if (nvm_req->re_r_rpmb) {
		/* nvm provider executes the request correctly and
		 * result of read rpmb is already read into dst */
		ret = 0;
	} else {
		ret = -EBUSY;
	}

	/* Restore to virt addr for nvmclnt_reply_outgoing */
	arg->dst = (hsm_ptr_t) virt_ptr;

	hsmnvm_stat.n_read_rpmb++;

	return 0;
}

/*
 * Implements the Write RPMB() API
 */
int hsmnvm_write_rpmb(struct nvmfs_write_rpmb *arg)
{
	int ret;
	void *virt_ptr;
	unsigned long phys_addr;

	if (!is_hsmnvm_online()) {
		return -1;
	}

	if (arg == NULL) {
		return -1;
	}

	/*
	 * Send request to nvm provider
	 */
	nvm_req->req |= HSM_PROVIDER_REQ_WRITE_RPMB;
	nvm_req->in_w_rpmb = arg;
	nvm_req->re_w_rpmb = NULL;

	virt_ptr = (void *) arg->src;
	phys_addr = virt_to_phys(virt_ptr);
	arg->src = (hsm_ptr_t) phys_addr;

	pr_debug("%s\n", __func__);

	up(&hsmnvm->req_sem);

	/*
	 * Wait reply from nvm provider
	 */
	ret = down_timeout(&hsmnvm->reply_sem, msecs_to_jiffies(NVM_WAIT_MS));
	if (ret != 0) {
		ret = -ETIME;
	}

	/* Clear request */
	nvm_req->req &= ~HSM_PROVIDER_REQ_WRITE_RPMB;

	if (nvm_req->re_w_rpmb) {
		/* nvm provider executes the request correctly and
		 * result of write rpmb is already written from src */
		ret = 0;
	} else {
		ret = -EBUSY;
	}

	/* Restore to virt addr for nvmclnt_reply_outgoing */
	arg->src = (hsm_ptr_t) virt_ptr;

	hsmnvm_stat.n_write_rpmb++;

	return 0;
}

/*
 * Implements the ReadNVMObject() API
 */
int hsmnvm_read_obj(struct nvmfs_read_obj *arg)
{
	int ret;
	void *virt_ptr;
	unsigned long phys_addr;

	if (!is_hsmnvm_online()) {
		return -1;
	}

	if (arg == NULL) {
		return -1;
	}

	/*
	 * Send request to nvm provider
	 */
	nvm_req->req |= HSM_PROVIDER_REQ_READ_OBJ;
	nvm_req->in_r_obj = arg;
	nvm_req->re_r_obj = NULL;

	virt_ptr = (void *) arg->dst;
	phys_addr = virt_to_phys(virt_ptr);
	arg->dst = (hsm_ptr_t) phys_addr;

	pr_debug("%s\n", __func__);

	up(&hsmnvm->req_sem);

	/*
	 * Wait reply from nvm provider
	 */
	ret = down_timeout(&hsmnvm->reply_sem, msecs_to_jiffies(NVM_WAIT_MS));
	if (ret != 0) {
		ret = -ETIME;
	}

	/* Clear request */
	nvm_req->req &= ~HSM_PROVIDER_REQ_READ_OBJ;

	if (nvm_req->re_r_obj) {
		/* nvm provider executes the request correctly and
		 * result of read obj is already read into dst */
		ret = 0;
	} else {
		ret = -EBUSY;
	}

	/* Restore to virt addr for nvmclnt_reply_outgoing */
	arg->dst = (hsm_ptr_t) virt_ptr;

	hsmnvm_stat.n_read_obj++;

	return 0;
}

/*
 * Implements the WriteNVMObject() API
 */
int hsmnvm_write_obj(struct nvmfs_write_obj *arg)
{
	int ret;
	void *virt_ptr;
	unsigned long phys_addr;

	if (!is_hsmnvm_online()) {
		return -1;
	}

	if (arg == NULL) {
		return -1;
	}

	/*
	 * Send request to nvm provider
	 */
	nvm_req->req |= HSM_PROVIDER_REQ_WRITE_OBJ;
	nvm_req->in_w_obj = arg;
	nvm_req->re_w_obj = NULL;

	virt_ptr = (void *) arg->src;
	phys_addr = virt_to_phys(virt_ptr);
	arg->src = (hsm_ptr_t) phys_addr;

	pr_debug("%s\n", __func__);

	up(&hsmnvm->req_sem);

	/*
	 * Wait reply from nvm provider
	 */
	ret = down_timeout(&hsmnvm->reply_sem, msecs_to_jiffies(NVM_WAIT_MS));
	if (ret != 0) {
		ret = -ETIME;
	}

	/* Clear request */
	nvm_req->req &= ~HSM_PROVIDER_REQ_WRITE_OBJ;

	if (nvm_req->re_w_obj) {
		/* nvm provider executes the request correctly and
		 * result of write obj is already written from src */
		ret = 0;
	} else {
		ret = -EBUSY;
	}

	/* Restore to virt addr for nvmclnt_reply_outgoing */
	arg->src = (hsm_ptr_t) virt_ptr;

	hsmnvm_stat.n_write_obj++;

	return 0;
}

const struct api_id2f api_id2f[] = {
	/* NVM Provider API */
	{ NVMFS_INFO_ID,	(API_FUNC) hsmnvm_get_nvm_info },
	{ NVMFS_READ_RPMB_ID,	(API_FUNC) hsmnvm_read_rpmb },
	{ NVMFS_WRITE_RPMB_ID,	(API_FUNC) hsmnvm_write_rpmb },
	{ NVMFS_READ_OBJ_ID,	(API_FUNC) hsmnvm_read_obj },
	{ NVMFS_WRITE_OBJ_ID,	(API_FUNC) hsmnvm_write_obj },
};

