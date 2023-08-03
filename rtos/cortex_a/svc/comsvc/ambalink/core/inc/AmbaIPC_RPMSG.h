/**
 * @file AmbaIPC_RPMSG.h
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Definitions for RPMSG.
 *
 */

#ifndef AMBA_RPMSG_H
#define AMBA_RPMSG_H

#define RPMSG_VRING_ALIGN           (4096U)
#define RPMSG_RESERVED_ADDRESSES      (1024U)
#define RPMSG_ADDRESS_ANY           (0xffffffffU)
#define RPMSG_NS_ADDR               (53U)
#define RPMSG_NAME_LEN              (32U)

#define RPMSG_MAGIC_ID              (0x416D6261U)

#include "rpmsg.h"

struct AmbaIPC_RPDEV_s;
typedef void (*rpdev_cb)(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len,
                         void *priv, UINT32 src);
struct AmbaIPC_RPDEV_s {
    /** Magic ID @sa RPMSG_MAGIC_ID */
    UINT32          magicID;
    /** flag */
    AMBA_KAL_EVENT_FLAG_t  flag;

    /** RPMSG device */
    struct AmbaIPC_RPCLNT_s   *rpclnt;
    /** Name of channel */
    char            name[RPMSG_NAME_LEN];
    /** Address of source */
    UINT32          src;
    /** Address of destination */
    UINT32          dst;
    /** Flags */
    UINT32          flags;

    /** Call-Back function */
    rpdev_cb        cb;
    /** Private data */
    void            *priv;
};

struct AmbaIPC_RPCLNT_s {
    /** Stack of task to handle rvq */
    UINT8                   *rvq_stack;
    /** Stack of task to handle svq */
    UINT8                   *svq_stack;

    /** Master/Host has initialized VQ */
    INT32                     inited;

    /** Task to handle rvq */
    AMBA_KAL_TASK_t         rvq_thread;
    /** Flag to handle rvq */
    AMBA_KAL_EVENT_FLAG_t   rvq_flag;
    /** Priority of rvq_thread */
    UINT32                  rvq_tsk_prio;
    /** Stack size */
    UINT32                  rvq_tsk_sz;
    /** Task name */
    char                    *rvq_name;

    /** Task to handle svq */
    AMBA_KAL_TASK_t         svq_thread;
    /** Flag to handle svq */
    AMBA_KAL_EVENT_FLAG_t   svq_flag;
    /** Priority of svq_thread */
    UINT32                  svq_tsk_prio;
    /** Stack size */
    UINT32                  svq_tsk_sz;
    /** Task name */
    char                    *svq_name;

    /** SVQ */
    void                    *svq;
    /** IRQ of SVQ-TX */
    UINT32                  svq_tx_irq;
    /** IRQ of SVQ-RX */
    UINT32                  svq_rx_irq;
    /** Number of svq buffers */
    UINT16                  svq_num_bufs;
    /** Physical address of svq buffers */
    void                   *svq_buf_phys;
    /** Physical address of svq vring */
    void                   *svq_vring_phys;
    /** Alignment of svq vring */
    UINT32                  svq_vring_algn;

    /** RVQ */
    void                    *rvq;
    /** IRQ of RVQ-TX */
    UINT32                  rvq_tx_irq;
    /** IRQ of RVQ-RX */
    UINT32                  rvq_rx_irq;
    /** Number of rvq buffers */
    UINT16                  rvq_num_bufs;
    /** Physical address of rvq buffers */
    void                   *rvq_buf_phys;
    /** Physical address of rvq vring */
    void                   *rvq_vring_phys;
    /** Alignment of rvq vring */
    UINT32                  rvq_vring_algn;
};

extern void RpmsgCB(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len, void *priv, UINT32 src);

extern INT32 rpdev_register(struct AmbaIPC_RPDEV_s *rpdev, const char *bus_name);
extern INT32 rpdev_unregister(struct AmbaIPC_RPDEV_s *rpdev);

extern struct AmbaIPC_RPDEV_s *rpdev_alloc(const char *name, UINT32 flags,
        rpdev_cb cb, void *priv);

extern INT32 rpdev_send(const struct AmbaIPC_RPDEV_s *rpdev, const void *data, UINT32 len);

#endif  /* AMBA_RPMSG_H */
