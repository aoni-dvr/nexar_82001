/**
 * @file vq.h
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
 * @details Definitions for vq.
 *
 */

#ifndef VQ_H
#define VQ_H

#include "virtio_ring.h"
#include "AmbaMMU.h"

struct AmbaIPC_VQ_s {
    /** OS-dependent syncrhonization */
    AMBA_KAL_EVENT_FLAG_t flag;
    /** OS-dependent syncrhonization and lock */
    AMBA_KAL_MUTEX_t      lock;

    /** Call-back function when get rx-buffer */
    void      (*cb)(struct AmbaIPC_VQ_s *vq);
    /** Kick function */
    void      (*kick)(void *data);
    /** Vring */
    struct AmbaIPC_VRING_s    vring;
    /** Index of last available buffer */
    UINT16     last_avail_idx;
    /** Index of last used buffer */
    UINT16     last_used_idx;
};

extern struct AmbaIPC_VQ_s *vq_create(void (*cb)(struct AmbaIPC_VQ_s *vq),
                                      void (*kick)(void *data),
                                      UINT16 num,
                                      const void *p, UINT32 align, const UINT32 vqid);

extern void vq_wait_for_completion(struct AmbaIPC_VQ_s *vq);
extern void vq_complete(struct AmbaIPC_VQ_s *vq);

extern UINT32 vq_kick_prepare(const struct AmbaIPC_VQ_s *vq);
extern void vq_enable_used_notify(const struct AmbaIPC_VQ_s *vq);
extern void vq_disable_used_notify(const struct AmbaIPC_VQ_s *vq);
extern UINT32 vq_more_avail_buf(const struct AmbaIPC_VQ_s *vq);
extern INT32 vq_get_avail_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len);
extern void vq_add_used_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len);
extern INT32 vq_get_used_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len);
extern void vq_add_avail_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len);
extern UINT32 vq_init_unused_bufs(struct AmbaIPC_VQ_s *vq, void *buf, UINT32 len);

extern void vq_init_avail(struct AmbaIPC_VQ_s *vq, UINT16 idx);
extern void vq_init_used(struct AmbaIPC_VQ_s *vq, UINT32 len, UINT16 idx);

#endif /* VQ_H */
