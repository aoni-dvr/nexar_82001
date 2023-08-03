/**
 * @file amba_platform_info.h
 *
 * @copyright Copyright (c) 2020 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef AMBA_PLATFORM_INFO_H
#define AMBA_PLATFORM_INFO_H

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>
#include <openamp/rpmsg_virtio.h>

#if defined __cplusplus
extern "C" {
#endif

struct amba_remoteproc {
    struct remoteproc rproc;

    /*
     * Private Data
     */
    /** RPMsg virtio shared buffer pool */
    struct rpmsg_virtio_shm_pool *shpool;
    /** rpmsg virtuali-IO device */
    struct rpmsg_virtio_device *rvdev;
    struct rpmsg_device *rpdev;

    /* RPMSG_HOST/VIRTIO_DEV_DRIVER (MASTER/HOST), RPMSG_REMOTE/VIRTIO_DEV_DEVICE (SLAVE/REMOTE) */
    unsigned long role;
};

/**
 * platform_init - initialize the platform
 *
 * It will initialize the platform.
 *
 * @param [in] proc_id ID of proc
 * @param [in] rsc_id  ID of resource
 * @param [in,out] platform pointer to store the platform data pointer
 *
 * @return 0 for success or negative value for failure
 */
int platform_init(unsigned long proc_id,
                  unsigned long rsc_id,
                  void **       platform);

/**
 * platform_create_rpmsg_vdev - create rpmsg vdev
 *
 * It will create rpmsg virtio device, and returns the rpmsg virtio
 * device pointer.
 *
 * @param [in] platform    pointer to the private data
 * @param [in] role        virtio master or virtio slave of the vdev
 * @param [in] rst_cb      virtio device reset callback
 * @param [in] ns_bind_cb  rpmsg name service bind callback
 *
 * @return pointer to the rpmsg virtio device
 */
struct rpmsg_device *
platform_create_rpmsg_vdev(void *platform,
                           unsigned int role,
                           void (*rst_cb)(struct virtio_device *vdev),
                           rpmsg_ns_bind_cb ns_bind_cb);

/**
 * platform_poll - platform poll function
 *
 * @param [in] platform pointer to the platform
 *
 * @return negative value for errors, otherwise 0.
 */
int platform_poll(void *platform);

/**
 * platform_release_rpmsg_vdev - release rpmsg virtio device
 *
 * @param [in] rpdev pointer to the rpmsg device
 */
void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev);

/**
 * platform_cleanup - clean up the platform resource
 *
 * @param [in] platform pointer to the platform
 */
void platform_cleanup(void *platform);


#ifdef CONFIG_CC_USESTD
int debug_printf(const char *format, ...);

/* Need debug_printf() at externallibrary. */
#define printf(...) debug_printf(__VA_ARGS__)
#else
#define printf(...)
#endif /* CONFIG_CC_USESTD */


#if defined __cplusplus
}
#endif

#endif /* AMBA_PLATFORM_INFO_H_ */

