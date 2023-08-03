/**
 * @file amba_remoteproc.c
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

#include "amba_platform_info.h"
#include "amba_rsc_table.h"



/* Called by
 * struct remoteproc *remoteproc_init(struct remoteproc *rproc,
 *                                    const struct remoteproc_ops *ops, void *priv)
 */

static struct remoteproc *
amba_rproc_init(struct remoteproc *rproc,
                const struct remoteproc_ops *ops, void *arg)
{
    struct amba_remoteproc *amba_rproc = arg;

    if (!rproc || !ops || !arg) {
        return NULL;
    }

    rproc->priv = amba_rproc;
    rproc->ops  = ops;

    /* TODO: Register interrupt handler and enable interrupt */

    return rproc;
}

static void amba_rproc_remove(struct remoteproc *rproc)
{
    if (!rproc) {
        return;
    }

    /* TODO: Unregister interrupt */

    (void)rproc;
}


static void *
amba_rproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
               metal_phys_addr_t *da, size_t size,
               unsigned int attribute, struct metal_io_region **io)
{
    struct remoteproc_mem *mem;
    metal_phys_addr_t lpa, lda;
    struct metal_io_region *tmpio;

    lpa = *pa;
    lda = *da;

    if ((lpa == METAL_BAD_PHYS) && (lda == METAL_BAD_PHYS)) {
        return NULL;
    }
    if (lpa == METAL_BAD_PHYS) {
        lpa = lda;
    }
    if (lda == METAL_BAD_PHYS) {
        lda = lpa;
    }

    /* TODO: memory attribute */
    (void)attribute;

    mem = metal_allocate_memory(sizeof(*mem));
    if (!mem) {
        return NULL;
    }
    tmpio = metal_allocate_memory(sizeof(*tmpio));
    if (!tmpio) {
        metal_free_memory(mem);
        return NULL;
    }
    remoteproc_init_mem(mem, NULL, lpa, lda, size, tmpio);
    /* va is the same as pa in this platform */
    metal_io_init(tmpio, (void *)lpa, &mem->pa, size,
                  sizeof(metal_phys_addr_t) << 3, attribute, NULL);
    remoteproc_add_mem(rproc, mem);
    *pa = lpa;
    *da = lda;
    if (io) {
        *io = tmpio;
    }
    return metal_io_phys_to_virt(tmpio, mem->pa);
}

// TODO: Tx-Kick only, Rx-Ack not done
static int amba_rproc_notify(struct remoteproc *rproc, uint32_t id)
{
    struct amba_remoteproc *amba_rproc;

    if (!rproc) {
        return -1;
    }

    amba_rproc = (struct amba_remoteproc *)rproc->priv;
    {
        extern void AmbaOamp_kick(uint32_t id);

        // TODO: get resource by id if more devices.
        struct amba_remote_resource_table *rsc = rproc->rsc_table;
        uint32_t notifyid;

        /*
         * cf. rpmsg_init_vdev() in vendors/foss/open-amp/src/lib/rpmsg/rpmsg_virtio.c
         * For master, 0: rx, 1: tx
         * For slave/remote, 0: tx, 1: rx
         */
        if (amba_rproc->role == RPMSG_HOST) {
            /* Master/Host */
            notifyid = rsc->rpmsg_vring1.notifyid;
        } else {
            notifyid = rsc->rpmsg_vring0.notifyid;
        }

        (void)id;

        AmbaOamp_kick(notifyid);
    }

    return 0;
}

static int amba_rproc_start(struct remoteproc *rproc)
{
    (void)rproc;

    return 0;
}

/* It defines notification operation and remote processor managementi operations. */
struct remoteproc_ops amba_proc_ops =
{
    .init     = amba_rproc_init,
    .remove   = amba_rproc_remove,
    .mmap     = amba_rproc_mmap,
    .notify   = amba_rproc_notify,
    .start    = amba_rproc_start,
    .stop     = NULL,
    .shutdown = NULL,
};

