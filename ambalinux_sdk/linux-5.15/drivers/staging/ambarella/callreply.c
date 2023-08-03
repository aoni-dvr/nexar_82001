/*
 * ambarella/callreply.c
 *
 * ARM HSM command call and reply handler
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include "comm.h"
#include "amalgam.h"
#include "callreply.h"

uint32_t seqno = 0;

int hsm_cre_mempool(struct mem_pool *pool, uint32_t pool_size)
{
	void *buf;

	memset(pool, 0x0, sizeof(*pool));

	buf = kmalloc(pool_size, GFP_KERNEL);
	if (buf == NULL) {
		return -ENOMEM;
	}

	pool->total_size = pool_size;
	pool->used_size = 0;
	pool->base = buf;
	pool->cur = buf;

	return 0;
}

void hsm_del_mempool(struct mem_pool *pool)
{
	if (pool->base)
		kfree(pool->base);

}

static void *hsm_alloc_mem(struct mem_pool *pool, uint32_t req_size)
{
	void *buf;

	if ((pool->used_size + req_size) > pool->total_size)
		return NULL;

	buf = pool->cur;
	pool->cur += req_size;
	pool->used_size += req_size;

	return buf;
}

void cleanup_hsm_mem(struct mem_pool *pool, struct hsm_ptr_store *ptr_store)
{
	hsm_del_mempool(pool);
}

/*
 * Alloc buffer to apiptr, then translate to physical address and
 * copy data from user_ptr.
 *
 * user_ptr: user space ptr
 * apiptr: pointer to apiptr to send data to
 * prt_size: memory size of ptr pointed to
 */
static inline int apiptr_sent_to(hsm_ptr_t user_ptr, hsm_ptr_t *apiptr,
				 uint32_t ptr_size, struct mem_pool *pool)
{
	void *buf;
	uint32_t ptr_align_size;

	if (user_ptr == (hsm_ptr_t) NULL)
		return 0;

	/*
	 * For some SMC won't fix issue on CV6, there are many 16 bytes
	 * aligned issue, if pointer size can be 16 bytes aligned, it will
	 * avoid some complicated size align operations
	 */
	ptr_align_size = ALIGN(ptr_size, 16);
	buf = hsm_alloc_mem(pool, ptr_align_size);
	if (buf == NULL)
		return -EMSGSIZE;

	*apiptr= (hsm_ptr_t) virt_to_phys(buf);

	pr_debug("%s: (uptr, apiptr, size) = (0x%llx, 0x%llx, %u)\n",
		 __func__, user_ptr, *apiptr, ptr_size);

	if (copy_from_user(buf, (void *)user_ptr, ptr_size))
		return -EFAULT;

#if 0 // defined(CONFIG_AMALGAM)
	/*
	 * FIXME:
	 * Local variable in kernel is located in user process stack,
	 * if it is called from user process, like cat /proc/xxx.
	 * The virtual address is in vmalloc memory map, can it can't
	 * use virt_to_phys to do address translation. In our case,
	 * call->arg is in stack and would be pushed into
	 * comm msgq, we can skip to do amalgam memory_sync write
	 * here to avoid "AMALGAM_ERROR: Memory Error".
	 *
	 */
	if (((unsigned long) apiptr >= 0xffff000000000000) &&
	    ((unsigned long) apiptr < 0xffff800000000000)) {
		/* Write apiptr to memory */
		memory_sync_write(apiptr, sizeof(hsm_ptr_t *));
	}

	/* FIXME: Non-mmaped ptr to do memory sync */
	if (ptr_size > 0)
		memory_sync_write(buf, ptr_size);
#endif

	return 0;
}

/*
 * Translate apiptr to virtual address and copy data to user_ptr.
 *
 * user_ptr: user space ptr
 * apiptr: pointer to apiptr to receive data from
 * prt_size: memory size of ptr pointed to
 */
static inline int apiptr_recv_from(hsm_ptr_t user_ptr,
				   hsm_ptr_t *apiptr, uint32_t ptr_size)
{
	void *virt_apiptr;
	int ret;

	if (user_ptr == (hsm_ptr_t) NULL)
		return 0;

	if (*apiptr == (hsm_ptr_t) NULL)
		return 0;

	pr_debug("%s: (uptr, apiptr, size) = (0x%llx, 0x%llx, %u)\n",
		 __func__, user_ptr, *apiptr, ptr_size);

	virt_apiptr = phys_to_virt(*apiptr);

	/* Restore user space pointer */
	*apiptr = user_ptr;

#if 0 // defined(CONFIG_AMALGAM)
	/* FIXME: Non-mmaped ptr to do memory sync */
	if (ptr_size > 0)
		memory_sync_read(virt_apiptr, ptr_size);
#endif

#if defined(DEBUG)
	print_hex_dump_bytes("apiptr: ", DUMP_PREFIX_NONE,
			     virt_apiptr, ptr_size);
#endif
	if ((ret = copy_to_user((void *) user_ptr, virt_apiptr, ptr_size))) {
		//FIXME:
		//return  -EFAULT;
		pr_debug("%s: ignore copy_to_user fail: %d\n", __func__, ret);
	}

	return 0;
}

int package_call_incoming(struct armhsm *armhsm, struct hsm_call *call,
			  struct armhsm_dma *dma)
{
	int rval, i;
	hsm_ptr_t *ptr;
	void *arg = (void *)call->arg;
	uint32_t size, nelems, elemsize;
	const struct hsm_api_struct *hsmapi;
	uint32_t fixed_size;
	uint32_t org_size = 0;

	if (call->id > (n_hsm_api_ptrs - 1)) {
		return -EDOM;
	}

	hsmapi = &hsm_api_ptrs[call->id];

	if (!call->arg || call->id != hsmapi->id) {
		return -EINVAL;
	}

	dev_dbg(armhsm->dev, "call: %d\n", call->id);
	dma->arg = dma_alloc_coherent(armhsm->dev, hsmapi->size,
				      &dma->arg_dma, GFP_KERNEL);
	if (IS_ERR(dma->arg))
		return -ENOMEM;

	/* Save the user pointer */
	dma->user_ptr = (void __user *)call->arg;
	dma->size = hsmapi->size;
	rval = copy_from_user(dma->arg, arg, hsmapi->size);
	if (rval)
		return -EFAULT;

	/* Specify a new coherent pointer */
	call->arg = dma->arg_dma;

	for (i = 0; i < hsmapi->n_ptrs; i++) {
		switch(hsmapi->pointers[i].type) {
		case HSMAPI_STRUCT_TYPE_UNDEF:
			rval = -EFAULT;
			break;
		case HSMAPI_STRUCT_TYPE_FIXED:
			size = hsmapi->pointers[i].sizefrom;
			ptr = (hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset);
			if (size && *ptr) {
				dma->member[i].ptr = dma_alloc_coherent(armhsm->dev, size,
									&dma->member[i].dma, GFP_KERNEL);
				dma->member[i].size = size;
				dma->member[i].user_ptr = (void *)*ptr;
				rval = copy_from_user(dma->member[i].ptr, (void *)*ptr, size);
				*ptr = dma->member[i].dma;
			}
			break;
		case HSMAPI_STRUCT_TYPE_VARIABLE:
			size = *(uint32_t *)(dma->arg + hsmapi->pointers[i].sizefrom);
			ptr = (hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset);
			/* Handle zero length and valid pointer */
			if (size == 0 && *ptr)
				fixed_size = 16;
			org_size = size;
			if (call->id == HSM_CMH_PROCESS_ID) {
				size = ALIGN(size, 16);
			}

			if (fixed_size || (size && *ptr)) {
				size = fixed_size ? fixed_size : size;
				dma->member[i].ptr = dma_alloc_coherent(armhsm->dev, size,
									&dma->member[i].dma, GFP_KERNEL);
				dma->member[i].size = size;
				dma->member[i].user_ptr = (void *)*ptr;

				/* copy data if size is not fixed  */
				if (!fixed_size)
					rval = copy_from_user(dma->member[i].ptr, (void *)*ptr, org_size);
				*ptr = dma->member[i].dma;
			}
			/* clear fixed_size */
			fixed_size = 0;
			break;
		case HSMAPI_STRUCT_TYPE_ARRAY:
			nelems = *(uint32_t *)(dma->arg + hsmapi->pointers[i].sizefrom);
			elemsize = hsmapi->pointers[i].elemsize;
			ptr = (hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset);
			size = nelems * elemsize;
			if (size && *ptr) {
				dma->member[i].ptr = dma_alloc_coherent(armhsm->dev, size,
									&dma->member[i].dma, GFP_KERNEL);
				dma->member[i].size = size;
				dma->member[i].user_ptr = (void *)*ptr;
				rval = copy_from_user(dma->member[i].ptr, (void *)*ptr, size);
				*ptr = dma->member[i].dma;
			}
			break;
		default:
			rval = -ENOPROTOOPT;
			break;
		}
		if (rval) {
			dev_err(armhsm->dev, "%s: exception %d at index %d\n",
				__func__, rval, i);
			break;
		}
	}

	return rval;
}

int handle_call_incoming(struct hsm_call *call,
			 struct hsm_ptr_store *ptr_store,
			 struct mem_pool *pool)
{
	const struct hsm_api_struct *hsmapi;
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t ptr_size;
	uint32_t nelems, elemsize;
	int i;
	int ret = 0;

	if (call->id > (n_hsm_api_ptrs - 1)) {
		return -EBADRQC;
	}

	hsmapi = &hsm_api_ptrs[call->id];

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	} else if (call->id != hsmapi->id) {
		return -EBADRQC;
	}

	pr_debug("%s: call->id (%u)\n", __func__, call->id);

	// FIXME: check max size of a cmd in api struct
	// Assume to support 1MB msg.
	ret = hsm_cre_mempool(pool, 0x201000);
	if (ret != 0) {
		return ret;
	}

	memset(ptr_store, 0x0, sizeof(*ptr_store));
	ptr_store->id = call->id;
	ptr_store->arg = call->arg;

	/* Handle call->arg */
	apiptr = &call->arg;
	ret = apiptr_sent_to(*apiptr, apiptr, hsmapi->size, pool);
	if (ret < 0) {
		return ret;
	}

	this_arg = phys_to_virt(*apiptr);
	/* Handle ptrs in call->arg */
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ptr_store->ptr[i] = *apiptr;
			ret = apiptr_sent_to(*apiptr, apiptr,
					     ptr_size, pool);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ptr_store->ptr[i] = *apiptr;
			ret = apiptr_sent_to(*apiptr, apiptr,
					     ptr_size, pool);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)
				   (this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ptr_store->ptr[i] = *apiptr;
			ret = apiptr_sent_to(*apiptr, apiptr,
					     ptr_size, pool);
			if (ret < 0) {
				return ret;
			}
		}
	}

	/* Fill remain fileds in finalize_call() */

#if defined(CONFIG_AMALGAM)
	memory_sync_write(pool->base, pool->used_size);
#endif

	return ret;
}

void finalize_call(struct hsm_call *call, struct hsm_ticket *ticket)
{
	/* Fill remain fileds in call */
	call->seq = seqno;
	call->chksum = call_chksum((void *) call);
	ticket->data = seqno;
	seqno++;
}

int package_reply_outgoing(struct armhsm *armhsm, struct hsm_call *call,
			   struct hsm_reply *reply, struct armhsm_dma *dma)
{
	int i, rval = 0;
	uint32_t chksum, size;
	uint32_t nelems, elemsize;
	const struct hsm_api_struct *hsmapi;

	if (call->id != reply->id) {
		dev_err(armhsm->dev, "%s: id mismatch (%d, %d)\n",
			__func__, call->id, reply->id);
		return -EPROTO;
	}

	chksum = reply_chksum((const uint8_t *)reply);
	if (chksum != reply->chksum) {
		dev_err(armhsm->dev, "%s: checksum mismatch (0x%08x, 0x%08x)\n",
			__func__, reply->chksum, chksum);
		return -EPROTO;
	}

	if (call->seq != reply->seq) {
		dev_err(armhsm->dev, "%s: sequence mismatch (%d, %d)\n",
			__func__, call->seq, reply->seq);
		return -EPROTO;
	}

	dev_dbg(armhsm->dev, "reply: %d\n", reply->id);

	call->ret = reply->ret;
	hsmapi = &hsm_api_ptrs[call->id];

	for (i = 0; i < hsmapi->n_ptrs; i++) {
		switch(hsmapi->pointers[i].type) {
			case HSMAPI_STRUCT_TYPE_UNDEF:
				rval = -EFAULT;
				break;
			case HSMAPI_STRUCT_TYPE_FIXED:
				size = hsmapi->pointers[i].sizefrom;
				*(hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset) =
					(hsm_ptr_t)dma->member[i].user_ptr;
				if (dma->member[i].ptr && dma->member[i].user_ptr && size) {
					rval = copy_to_user(dma->member[i].user_ptr, dma->member[i].ptr, size);
					if (rval == size)
						rval = 0;
				}
				break;
			case HSMAPI_STRUCT_TYPE_VARIABLE:
				size = *(uint32_t *)(dma->arg + hsmapi->pointers[i].sizefrom);
				*(hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset) =
					(hsm_ptr_t)dma->member[i].user_ptr;
				if (dma->member[i].ptr && dma->member[i].user_ptr && size) {
					rval = copy_to_user(dma->member[i].user_ptr, dma->member[i].ptr, size);
					if (rval == size)
						rval = 0;
				}
				break;
			case HSMAPI_STRUCT_TYPE_ARRAY:
				/* set by HSM ? */
				nelems = *(uint32_t *)(dma->arg + hsmapi->pointers[i].sizefrom);
				elemsize = hsmapi->pointers[i].elemsize;
				size = nelems * elemsize;
				*(hsm_ptr_t *)(dma->arg + hsmapi->pointers[i].offset) =
					(hsm_ptr_t)dma->member[i].user_ptr;
				if (dma->member[i].ptr && dma->member[i].user_ptr && size) {
					rval = copy_to_user(dma->member[i].user_ptr, dma->member[i].ptr, size);
					/*
					 * FIXME: read-only memory in user space will return the number of bytes that
					 * cannot be copied. We'd better do more investigation on this issue.
					 */
					if (rval == size)
						rval = 0;
				}
				break;
			default:
				rval = -ENOPROTOOPT;
				break;
		}
		if (rval) {
			dev_err(armhsm->dev, "%s: id%d: exception %d at index %d\n",
				__func__,reply->id, rval, i);
			 break;
		}
	}

	if (!rval) {
		/* Restore the user pointer */
		call->arg = (hsm_ptr_t)dma->user_ptr;

		rval = copy_to_user((void *)call->arg, dma->arg, hsmapi->size);
	}

	/* release allocated DMA */
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (!dma->member[i].size)
			continue;
		dma_free_coherent(armhsm->dev, dma->member[i].size,
				  dma->member[i].ptr, dma->member[i].dma);
	}

	if (dma->size) {
		dma_free_coherent(armhsm->dev, dma->size, dma->arg, dma->arg_dma);
	}

	return rval;
}

int handle_reply_outgoing(struct hsm_call *call,
			  struct hsm_reply *reply,
			  struct hsm_ptr_store *ptr_store,
			  struct mem_pool *pool)
{
	const struct hsm_api_struct *hsmapi;
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t chksum;
	uint32_t ptr_size;
	uint32_t nelems, elemsize;
	int i;
	int ret = 0;

	/* Verify checksum */
	chksum = reply_chksum((const uint8_t *) reply);
	if (reply->chksum != chksum) {
		pr_err("%s: read reply checksum error, 0x%x, 0x%x\n",
		       __func__, reply->chksum, chksum);
		return -EPROTO;
	}

	if (call->seq != reply->seq) {
		pr_err("%s: seqno mismatch (%u != %u)\n",
		       __func__, call->seq, reply->seq);
		return -EPROTO;
	}

	if (ptr_store->id != call->id) {
		return -EINVAL;
	}

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	}

	/*
	 * set return code of the orchsm API to call,
	 * as reply is not return to user space.
	 */
	call->ret = reply->ret;
	pr_debug("%s: call->ret (0x%x)\n", __func__, call->ret);

	hsmapi = &hsm_api_ptrs[ptr_store->id];

#if defined(CONFIG_AMALGAM)
	memory_sync_read(pool->base, pool->used_size);
#endif

	this_arg = phys_to_virt(call->arg);

	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type ==
		    HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = apiptr_recv_from(ptr_store->ptr[i],
					       apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = apiptr_recv_from(ptr_store->ptr[i],
					       apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)
				   (this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = apiptr_recv_from(ptr_store->ptr[i],
					       apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		}
	}

	/* cmd->arg, i.e., the api struct ptr */
	ptr_size = hsmapi->size;
	ret = apiptr_recv_from(ptr_store->arg, &call->arg, ptr_size);
	if (ret < 0) {
		return ret;
	}
	return ret;
}

/*
 * Functions to handle kernel call incoming and reply outgoing.
 */

static inline int kptr_sent_to(hsm_ptr_t *apiptr, uint32_t ptr_size)
{
	void *virt_kptr;

	virt_kptr = (void *) *apiptr;
	if (virt_kptr == NULL)
		return 0;

	if (!virt_addr_valid(virt_kptr)) {
		return -1;
	}

	*apiptr= (hsm_ptr_t) virt_to_phys(virt_kptr);

	pr_debug("%s: (apiptr, size) = (0x%llx, %u)\n",
		 __func__, *apiptr, ptr_size);

#if defined(CONFIG_AMALGAM)
	/*
	 * FIXME:
	 * Local variable in kernel is located in user process stack,
	 * if it is called from user process, like cat /proc/xxx.
	 * The virtual address is in vmalloc memory map, can it can't
	 * use virt_to_phys to do address translation. In our case,
	 * call->arg is in stack and would be pushed into
	 * comm msgq, we can skip to do amalgam memory_sync write
	 * here to avoid "AMALGAM_ERROR: Memory Error".
	 *
	 */

	if (((unsigned long) apiptr >= 0xffff000000000000) &&
	    ((unsigned long) apiptr < 0xffff800000000000)) {
		/* Write apiptr to memory */
		memory_sync_write(apiptr, sizeof(hsm_ptr_t *));
	}

	if (ptr_size > 0) {
		memory_sync_write(virt_kptr, ptr_size);
	}
#endif

	return 0;
}

static inline int kptr_recv_from(hsm_ptr_t *apiptr, uint32_t ptr_size)
{
	void *virt_apiptr;

	if (*apiptr == (hsm_ptr_t) NULL)
		return 0;

	pr_debug("%s: (apiptr, size) = (0x%llx, %u)\n",
		 __func__, *apiptr, ptr_size);

	virt_apiptr = phys_to_virt(*apiptr);
	if (!virt_addr_valid(virt_apiptr)) {
		return -1;
	}

	/* Restore kernel space pointer */
	*apiptr = (hsm_ptr_t) virt_apiptr;

#if defined(CONFIG_AMALGAM)
	if (ptr_size > 0)
		memory_sync_read(virt_apiptr, ptr_size);
#endif

#if defined(DEBUG)
	print_hex_dump_bytes("apiptr: ", DUMP_PREFIX_NONE,
			     virt_apiptr, ptr_size);
#endif

	return 0;
}

int kernel_call_incoming(struct hsm_call *call)
{
	const struct hsm_api_struct *hsmapi;
#if !ARMHSM_USE_DMA
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t ptr_size;
	uint32_t nelems, elemsize;
	int i;
#endif
	int ret = 0;

	if (call->id > (n_hsm_api_ptrs - 1)) {
		return -EBADRQC;
	}

	hsmapi = &hsm_api_ptrs[call->id];

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	} else if (call->id != hsmapi->id) {
		return -EBADRQC;
	}

	pr_debug("%s: call->id (%u)\n", __func__, call->id);

#if !ARMHSM_USE_DMA
	/* Handle call->arg */
	apiptr = &call->arg;
	ret = kptr_sent_to(apiptr, hsmapi->size);
	if (ret < 0) {
		return ret;
	}

	this_arg = phys_to_virt(*apiptr);
	/* Handle ptrs in call->arg */
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type ==
		    HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)
				   (this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		}
	}
#endif

	/* Fill remain fileds in finalize_call() */

	return ret;
}

int kernel_reply_outgoing(struct hsm_call *call, struct hsm_reply *reply)
{
#if !ARMHSM_USE_DMA
	const struct hsm_api_struct *hsmapi;
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t ptr_size;
	uint32_t nelems, elemsize;
	int i;
#endif
	uint32_t chksum;
	int ret = 0;

	/*
	 * Verify checksum
	 */
	chksum = reply_chksum((void *) reply);
	if (reply->chksum != chksum) {
		pr_err("%s: read reply checksum error, 0x%x, 0x%x\n",
		       __func__, reply->chksum, chksum);
		return -EPROTO;
	}

	if (call->seq != reply->seq) {
		pr_err("%s: seqno mismatch (%u != %u)\n",
		       __func__, call->seq, reply->seq);
		return -EPROTO;
	}

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	}

	/*
	 * set return code of the orchsm API to call,
	 * as reply is not return to user space.
	 */
	call->ret = reply->ret;
	pr_debug("%s: call->ret (0x%x)\n", __func__, call->ret);

#if !ARMHSM_USE_DMA
	hsmapi = &hsm_api_ptrs[call->id];

	/* cmd->arg, i.e., the api struct ptr */
	apiptr = &call->arg;
	ptr_size = hsmapi->size;
	ret = kptr_recv_from(apiptr, ptr_size);
	if (ret < 0) {
		return ret;
	}

	this_arg = (void *) (*apiptr);
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type ==
		    HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)
				   (this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		}
	}

#endif
	return ret;
}

/*
 * Handle hsm call from nvmclient, which is orchsm.
 */
int nvmclnt_call_incoming(struct hsm_call *call)
{
	const struct hsm_api_struct *hsmapi;
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t ptr_size, chksum;
	uint32_t nelems, elemsize;
	int i;
	int ret = 0;

	chksum = call_chksum((const uint8_t *)call);
	if (chksum != call->chksum) {
		pr_err("%s: API call checksum error\n", __func__);
		return -EPROTO;
	}

	if (call->id > (n_hsm_api_ptrs - 1)) {
		return -EBADRQC;
	}

	hsmapi = &hsm_api_ptrs[call->id];

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	} else if (call->id != hsmapi->id) {
		return -EBADRQC;
	}

	pr_debug("%s: call->id (%u)\n", __func__, call->id);

	/* Handle call->arg */
	apiptr = &call->arg;
	ptr_size = hsmapi->size;
	ret = kptr_recv_from(apiptr, ptr_size);
	if (ret < 0) {
		return ret;
	}

	this_arg = (void *) (*apiptr);
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type ==
		    HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)
				   (this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_recv_from(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		}
	}

	return ret;
}

/*
 * Handle hsm reply to nvmclient, which is orchsm.
 */
int nvmclnt_reply_outgoing(struct hsm_call *call, struct hsm_reply *reply)
{
	const struct hsm_api_struct *hsmapi;
	hsm_ptr_t *apiptr;
	void *this_arg;
	uint32_t ptr_size;
	uint32_t nelems, elemsize;
	int i;
	int ret = 0;

	reply->id = call->id;
	reply->seq = call->seq;
	reply->chksum = reply_chksum((void *) reply);

	if (call->id > (n_hsm_api_ptrs - 1)) {
		return -EBADRQC;
	}

	hsmapi = &hsm_api_ptrs[call->id];

	if (call->arg == (hsm_ptr_t) NULL) {
		return -EINVAL;
	} else if (call->id != hsmapi->id) {
		return -EBADRQC;
	}

	pr_debug("%s: reply->ret (0x%x)\n", __func__, reply->ret);

	/* Handle call->arg */
	apiptr = &call->arg;
	ret = kptr_sent_to(apiptr, hsmapi->size);
	if (ret < 0) {
		return ret;
	}

	this_arg = phys_to_virt(*apiptr);
	/* Handle ptrs in call->arg */
	for (i = 0; i < hsmapi->n_ptrs; i++) {
		if (hsmapi->pointers[i].type == HSMAPI_STRUCT_TYPE_UNDEF) {
			break;
		}

		if (hsmapi->pointers[i].type ==
		    HSMAPI_STRUCT_TYPE_FIXED) {
			ptr_size = hsmapi->pointers[i].sizefrom;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_VARIABLE) {
			ptr_size = *((uint32_t *)
				     (this_arg + hsmapi->pointers[i].sizefrom));
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		} else if (hsmapi->pointers[i].type ==
			   HSMAPI_STRUCT_TYPE_ARRAY) {
			nelems = *((uint32_t *)(this_arg + hsmapi->pointers[i].sizefrom));
			elemsize = hsmapi->pointers[i].elemsize;
			ptr_size = nelems * elemsize;
			apiptr = (hsm_ptr_t *)
				(this_arg + hsmapi->pointers[i].offset);
			ret = kptr_sent_to(apiptr, ptr_size);
			if (ret < 0) {
				return ret;
			}
		}
	}

	return ret;
}

