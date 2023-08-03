/**
 * @file amba_platform_info.c
 *
 * @copyright Copyright (c) 2020 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <AmbaWrap.h>

#include <metal/atomic.h>
#include <metal/assert.h>
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>

#include "amba_metal.h"
#include "amba_platform_info.h"
#include "amba_rsc_table.h"



#define _rproc_wait() __asm__ volatile ("wfi")


static struct amba_remoteproc rproc_inst[NUM_RPROC_INST];
static unsigned int cur_vdev_index = 0;


extern struct remoteproc_ops amba_proc_ops;

static struct amba_remoteproc *
platform_create_proc(int proc_index, int rsc_index)
{
    int ret;
    void *rsc_table;
    void *rsc_table_shm;
    int rsc_size;
    metal_phys_addr_t pa;
    unsigned int attr;
    struct amba_remote_resource_table *amba_rsc;
    struct amba_remoteproc *amba_rproc;
    struct remoteproc *rproc;

    if (proc_index > NUM_RPROC_INST) {
        return 0;
    }
    rsc_table = get_resource_table(rsc_index, &rsc_size);
    if (rsc_table == NULL) {
        return NULL;
    }
    amba_rsc = (struct amba_remote_resource_table *)rsc_table;
    amba_rproc = &rproc_inst[proc_index];
    rproc = &(amba_rproc->rproc);

    /* Initialize remoteproc instance */
    if (!remoteproc_init(rproc, &amba_proc_ops,
                         amba_rproc)) {
        return NULL;
    }

    /* TODO: memory attribution. */
    attr = 0;
    /* mmap resource table */
    pa = (metal_phys_addr_t)rsc_table;
    rsc_table_shm = remoteproc_mmap(rproc, &pa,
                                    NULL, rsc_size,
                                    attr,
                                    &(rproc->rsc_io));
    /* mmap shared memory */
#ifdef CONFIG_ARM64
    amba_rproc->shpool = (struct rpmsg_virtio_shm_pool *)((UINT64)amba_rsc + amba_rsc->offset[RSC_OFFSET_MEM]);
#else
    amba_rproc->shpool = (struct rpmsg_virtio_shm_pool *)((UINT32)amba_rsc + amba_rsc->offset[RSC_OFFSET_MEM]);
#endif
    if ((amba_rproc->shpool) && (amba_rproc->shpool->size)) {
        pa = (metal_phys_addr_t) amba_rproc->shpool->base;
        (void *)remoteproc_mmap(rproc, &pa,
                                NULL, amba_rproc->shpool->size,
                                attr,
                                NULL);
    }

    /* parse resource table to remoteproc */
    ret = remoteproc_set_rsc_table(rproc, rsc_table_shm, rsc_size);
    if (ret) {
        printf("Failed to intialize remoteproc\r\n");
        remoteproc_remove(rproc);
        return NULL;
    }
    printf("Initialize remoteproc successfully.\r\n");

    return amba_rproc;
}

int platform_init(unsigned long proc_id,
                  unsigned long rsc_id,
                  void **       platform)
{
    struct amba_remoteproc *rproc;

    if (!platform) {
        printf("Failed to initialize platform,"
               "NULL pointer to store platform data.\n");
        return -EINVAL;
    }
    /* Initialize HW system components */
    amba_init_metal_system();

    rproc = platform_create_proc(proc_id, rsc_id);
    if (!rproc) {
        printf("Failed to create remoteproc device.\n");
        return -EINVAL;
    }
    *platform = rproc;

    return 0;
}

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

struct  rpmsg_device *
platform_create_rpmsg_vdev(void *platform,
                           unsigned int role,
                           void (*rst_cb)(struct virtio_device *vdev),
                           rpmsg_ns_bind_cb ns_bind_cb)
{
    struct amba_remoteproc *amba_rproc = platform;
    struct remoteproc *rproc = &(amba_rproc->rproc);
    struct rpmsg_virtio_device *rpmsg_vdev;
    struct virtio_device *vdev;
    void *shbuf;
    struct metal_io_region *shbuf_io;
    int ret;

    rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
    if (!rpmsg_vdev) {
        return NULL;
    }
    amba_rproc->role = role;

    shbuf_io = remoteproc_get_io_with_pa(rproc, (size_t)amba_rproc->shpool->base);
    if (!shbuf_io) {
        return NULL;
    }
    shbuf = metal_io_phys_to_virt(shbuf_io,
            (size_t)amba_rproc->shpool->base);

    printf("creating remoteproc virtio\r\n");
    /* TODO: can we have a wrapper for the following two functions? */
    vdev = remoteproc_create_virtio(rproc, cur_vdev_index, role, rst_cb);
    if (!vdev) {
        printf("failed remoteproc_create_virtio\r\n");
        goto err1;
    }
    cur_vdev_index++;

    printf("initializing rpmsg shared buffer pool\r\n");
    /* Only RPMsg virtio master needs to initialize the shared buffers pool */
    if (role == VIRTIO_DEV_DRIVER) {
        rpmsg_virtio_init_shm_pool(&shpool, shbuf,
                                   amba_rproc->shpool->size);
    }

    printf("initializing rpmsg vdev\r\n");
    /* Set NS feature, so use intenal NS service */
    (void)ns_bind_cb;
    if (role == VIRTIO_DEV_DRIVER) {
        ret = rpmsg_init_vdev(rpmsg_vdev, vdev, ns_bind_cb,
                              shbuf_io,
                              &shpool);
    } else {
        /* RPMsg virtio slave can set shared buffers pool argument to NULL */
        ret = rpmsg_init_vdev(rpmsg_vdev, vdev, ns_bind_cb,
                              shbuf_io,
                              NULL);
    }
    if (ret) {
        printf("failed rpmsg_init_vdev\r\n");
        goto err2;
    }
    amba_rproc->rvdev = rpmsg_vdev;

    if (role == VIRTIO_DEV_DRIVER) {
        virtqueue_enable_cb(rpmsg_vdev->vdev->vrings_info[0].vq);
        virtqueue_enable_cb(rpmsg_vdev->vdev->vrings_info[1].vq);
        {
            extern void AmbaOamp_kick(uint32_t id);
            AmbaOamp_kick(3);
        }
    }
    return rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);
err2:
    remoteproc_remove_virtio(rproc, vdev);
err1:
    metal_free_memory(rpmsg_vdev);
    return NULL;
}

int platform_poll(void *priv)
{
    struct remoteproc *rproc = priv;
    struct remoteproc_priv *prproc;
    unsigned int flags;

    prproc = rproc->priv;
    while (1) {
        flags = metal_irq_save_disable();
#if 0
        if (!(atomic_flag_test_and_set(&prproc->ipi_nokick))) {
            metal_irq_restore_enable(flags);
            remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
            break;
        }
#else
        (void)prproc;
#endif
        _rproc_wait();
        metal_irq_restore_enable(flags);
    }
    return 0;
}

void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev)
{
    /* TODO: suppose no release */
    (void)rpdev;
}

void platform_cleanup(void *platform)
{
    /* TODO: suppose no cleanup */
    struct remoteproc *rproc = platform;

    if (rproc) {
        remoteproc_remove(rproc);
    }
    amba_cleanup_metal_system();
}

void AmbaOamp_rpmsgRxHandler(void *platform)
{
    struct remoteproc *rproc = (struct remoteproc *)platform;

    remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
}

#ifdef CONFIG_CC_USESTD

void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AmbaPrint_Flush(void);

#define DEBUG_BUF_SZ (512)
static char debug_buffer[DEBUG_BUF_SZ];

int debug_printf(const char *format, ...)
{
    va_list vl;
    int ret;

    va_start(vl, format);
    ret               = vsnprintf(debug_buffer, DEBUG_BUF_SZ - 1, format, vl);
    debug_buffer[ret] = '\0';
    va_end(vl);

    AmbaPrint_PrintStr5("%s", debug_buffer, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();

    return ret;
}
#endif // CONFIG_CC_USESTD

