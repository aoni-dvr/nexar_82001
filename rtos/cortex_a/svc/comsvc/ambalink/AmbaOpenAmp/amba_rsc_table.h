/**
 * @file amba_rsc_table.h
 *
 * @copyright Copyright (c) 2020 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* This file populates resource table for BM remote
 * for use by the Linux Master */

#ifndef RSC_TABLE_H_
#define RSC_TABLE_H_

#include <stddef.h>
#include <openamp/open_amp.h>

#if defined __cplusplus
extern "C" {
#endif

#define NUM_RPROC_INST    2

#define NUM_RSC_TABLES    NUM_RPROC_INST
#define NUM_TABLE_ENTRIES 2
#define RSC_ID_MASTER     RPMSG_HOST    // 0
#define RSC_ID_SLAVE      RPMSG_REMOTE  // 1

#define RSC_OFFSET_VDEV   0
#define RSC_OFFSET_MEM    1

/* Resource table for the given remote */
struct amba_remote_resource_table {
    /** cf. struct resource_table */
    unsigned int             ver;
    /** Numbers of table entries */
    unsigned int             num;
    /** Padding */
    unsigned int             reserved[2];
    /** offset of each entry */
    unsigned int             offset[NUM_TABLE_ENTRIES];

    /** memory, struct fw_rsc_rproc_mem is dropped. */
    struct rpmsg_virtio_shm_pool shpool;

    /** rpmsg vdev entry, including vring. */
    struct fw_rsc_vdev       rpmsg_vdev;
    /** vring0 */
    struct fw_rsc_vdev_vring rpmsg_vring0;
    /** vring1 */
    struct fw_rsc_vdev_vring rpmsg_vring1;
} __attribute__((packed, aligned(0x100)));

void *get_resource_table(int rsc_id, int *len);

#if defined __cplusplus
}
#endif

#endif /* RSC_TABLE_H_ */

