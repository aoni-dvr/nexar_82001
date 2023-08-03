/**
 * @file amba_rsc_table.c
 *
 * @copyright Copyright (c) 2020 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remote
 * for use by the Linux Master */

#include <openamp/open_amp.h>
#include "amba_rsc_table.h"

#include <AmbaTypes.h>
#include <AmbaLink.h>

extern AMBA_LINK_CTRL_s AmbaLinkCtrl;


/* Remote supports Name Service announcement */
#define AMBA_DFEATURES       (0x01 << VIRTIO_RPMSG_F_NS)

#define NUM_VRINGS           0x02
#define VRING_ALIGN          0x1000
#define VRING_SIZE           1024
/* Descriptor of Master/Host Tx/Rx, cf. rpmsg_virtio.c of open-amp */
#define RING_MRX             0x78400000ul
#define RING_MTX             0x7840A000ul


struct amba_remote_resource_table resources[NUM_RSC_TABLES] =
{
    {     /* Master/Host */
        /* Version, cf. RSC_TAB_SUPPORTED_VERSION */
        .ver = 1,

        /* Number of table entries, Max: NO_RESOURCE_ENTRIES */
        .num = NUM_TABLE_ENTRIES,
        /* reserved fields */
        .reserved = { 0, 0 },

        /* Offsets of rsc entries */
        .offset =
        {
            offsetof(struct amba_remote_resource_table, rpmsg_vdev),
            offsetof(struct amba_remote_resource_table, shpool),
        },

        /* Virtio device entry */
        .rpmsg_vdev =
        {
            .type          = RSC_VDEV,
            .id            = VIRTIO_ID_RPMSG,
            .notifyid      = 0,
            .dfeatures     = AMBA_DFEATURES,
            .gfeatures     = AMBA_DFEATURES,
            .config_len    = 0,
            .status        = 0,
            .num_of_vrings = NUM_VRINGS,
            .reserved      = { 0, 0 },
        },

        /* Vring rsc entry - part of vdev rsc entry */
        /* rpmsg_virtio.c of open-amp,
           Master: vq_names[0] = "rx_vq", vq_names[1] = "tx_vq"
           Remote: vq_names[0] = "tx_vq", vq_names[1] = "rx_vq"
           #define VRING_IRQ_C0_TO_C1_KICK         AXI_SOFT_IRQ(0)
           #define VRING_IRQ_C0_TO_C1_ACK          AXI_SOFT_IRQ(1)
           #define VRING_IRQ_C1_TO_C0_KICK         AXI_SOFT_IRQ(2)
           #define VRING_IRQ_C1_TO_C0_ACK          AXI_SOFT_IRQ(3)
         */
        .rpmsg_vring0 =
        {
            .da       = RING_MRX,
            .align    = VRING_ALIGN,
            .num      = VRING_SIZE,
            .notifyid = 3,              /* Ack, notify of rx */
            .reserved = 0
        },
        .rpmsg_vring1 =
        {
            .da       = RING_MTX,
            .align    = VRING_ALIGN,
            .num      = VRING_SIZE,
            .notifyid = 2, /* Kick, notify of tx */
            .reserved = 0
        },
    },
    {                      /* Remote/Slave */
        /* Version, cf. RSC_TAB_SUPPORTED_VERSION */
        .ver = 1,

        /* NUmber of table entriesi, Max: NO_RESOURCE_ENTRIES */
        .num = NUM_TABLE_ENTRIES,
        /* reserved fields */
        .reserved = { 0, 0 },

        /* Offsets of rsc entries */
        .offset =
        {
            offsetof(struct amba_remote_resource_table, rpmsg_vdev),
            offsetof(struct amba_remote_resource_table, shpool),
        },

        /* Virtio device entry */
        .rpmsg_vdev =
        {
            .type          = RSC_VDEV,
            .id            = VIRTIO_ID_RPMSG,
            .notifyid      = 0,
            .dfeatures     = AMBA_DFEATURES,
            .gfeatures     = AMBA_DFEATURES,
            .config_len    = 0,
            .status        = 0,
            .num_of_vrings = NUM_VRINGS,
            .reserved      = { 0, 0 },
        },

        /* Vring rsc entry - part of vdev rsc entry */
        /* rpmsg_virtio.c of open-amp,
           Master: vq_names[0] = "rx_vq", vq_names[1] = "tx_vq"
           Remote: vq_names[0] = "tx_vq", vq_names[1] = "rx_vq"
           #define VRING_IRQ_C0_TO_C1_KICK         AXI_SOFT_IRQ(0)
           #define VRING_IRQ_C0_TO_C1_ACK          AXI_SOFT_IRQ(1)
           #define VRING_IRQ_C1_TO_C0_KICK         AXI_SOFT_IRQ(2)
           #define VRING_IRQ_C1_TO_C0_ACK          AXI_SOFT_IRQ(3)
         */
        .rpmsg_vring0 =
        {
            .da       = RING_MRX,
            .align    = VRING_ALIGN,
            .num      = VRING_SIZE,
            .notifyid = 0,              /* Kick, notify of tx */
            .reserved = 0
        },
        .rpmsg_vring1 =
        {
            .da       = RING_MTX,
            .align    = VRING_ALIGN,
            .num      = VRING_SIZE,
            .notifyid = 1,              /* Ack, notify of rx */
            .reserved = 0
        },
    }
};

void *get_resource_table(int rsc_id, int *len)
{
    struct amba_remote_resource_table *rsc;

    switch (rsc_id) {
    case RSC_ID_MASTER:
    case RSC_ID_SLAVE:
        *len = sizeof(resources[rsc_id]);
        rsc = &(resources[rsc_id]);

        rsc->shpool.base = (void *)(long)AmbaLinkCtrl.VringHostAndClntBufAddr;
        rsc->shpool.size = (CONFIG_AMBALINK_RPMSG_NUM) * (CONFIG_AMBALINK_RPMSG_SIZE);

        rsc->rpmsg_vring0.da = AmbaLinkCtrl.VringClntToHostAddr;
        rsc->rpmsg_vring1.da = AmbaLinkCtrl.VringHostToClntAddr;
        rsc->rpmsg_vring0.num = CONFIG_AMBALINK_RPMSG_NUM / 2;
        rsc->rpmsg_vring1.num = CONFIG_AMBALINK_RPMSG_NUM / 2;
        /*
         * cf. rpmsg_init_vdev() in vendors/foss/open-amp/src/lib/rpmsg/rpmsg_virtio.c
         * For master, 0: rx, 1: tx
         * For slave/remote, 0: tx, 1: rx
         */
        if (rsc_id == RSC_ID_MASTER) {
            /* rx */
            rsc->rpmsg_vring0.notifyid = 3;
            /* tx */
            rsc->rpmsg_vring1.notifyid = 2;
        } else {
            /* tx */
            rsc->rpmsg_vring0.notifyid = 0;
            /* rx */
            rsc->rpmsg_vring1.notifyid = 1;
        }
        return rsc;

    default:
        *len = 0;
        return NULL;
    }
}

