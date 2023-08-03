/**
 * @file virtio_ring.h
 */
#ifndef LINUX_VIRTIO_RING_H
#define LINUX_VIRTIO_RING_H
/* An interface for efficient virtio implementation, currently for use by KVM
 * and lguest, but hopefully others soon.  Do NOT change this since it will
 * break existing servers and clients.
 *
 * This header is BSD licensed so anyone can use the definitions to implement
 * compatible drivers/servers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of IBM nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL IBM OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Copyright (c) 2020 Ambarella International LP
 * Copyright Rusty Russell IBM Corporation 2007. */

/* This marks a buffer as continuing via the next field. */
#define VRING_DESC_F_NEXT 1
/* This marks a buffer as write-only (otherwise read-only). */
#define VRING_DESC_F_WRITE  2
/* This means the buffer contains a list of buffer descriptors. */
#define VRING_DESC_F_INDIRECT 4

/* The Host uses this in used->flags to advise the Guest: don't kick me when
 * you add a buffer.  It's unreliable, so it's simply an optimization.  Guest
 * will still kick if it's out of buffers. */
#define VRING_USED_F_NO_NOTIFY  (UINT8)1U
/* The Guest uses this in avail->flags to advise the Host: don't interrupt me
 * when you consume a buffer.  It's unreliable, so it's simply an
 * optimization.  */
#define VRING_AVAIL_F_NO_INTERRUPT  1U

/* We support indirect buffer descriptors */
#define VIRTIO_RING_F_INDIRECT_DESC 28

/* The Guest publishes the used index for which it expects an interrupt
 * at the end of the avail ring. Host should ignore the avail->flags field. */
/* The Host publishes the avail index for which it expects a kick
 * at the end of the used ring. Guest should ignore the used->flags field. */
#define VIRTIO_RING_F_EVENT_IDX   29

/* Virtio ring descriptors: 16 bytes.  These can chain together via "next". */
struct vring_desc {
    /** Address (guest-physical). */
    UINT64 addr;
    /** Length. */
    UINT32 len;
    /** The flags as indicated above. */
    UINT16 flags;
    /** We chain unused descriptors via this, too */
    UINT16 next;
};

struct vring_avail {
    /** flags, e.g. VRING_AVAIL_F_NO_INTERRUPT */
    UINT16 flags;
    /** Current index available. */
    UINT16 idx;
    /** ISO C forbids zero-size array, array size don't care, we will not have real instance */
    UINT16 ring[2U];
};

/* u32 is used here for ids for padding reasons. */
struct vring_used_elem {
    /** Index of start of used descriptor chain. */
    UINT32 id;
    /** Total length of the descriptor chain which was used (written to) */
    UINT32 len;
};

struct vring_used {
    /** flags, e.g. VRING_USED_F_NO_NOTIFY  */
    UINT16 flags;
    /** Current index used. */
    UINT16 idx;
    /** ISO C forbids zero-size array, array size don't care, we will not have real instance */
    struct vring_used_elem ring[2U];
};

struct AmbaIPC_VRING_s {
    /** Vring number */
    UINT16 num;
    /** descriptions */
    struct vring_desc *desc;
    /** available vring */
    struct vring_avail *avail;
    /** used vring */
    struct vring_used *used;
};

#if 0
/* The standard layout for the ring is a continuous chunk of memory which looks
 * like this.  We assume num is a power of 2.
 */
struct AmbaIPC_VRING_s {
    // The actual descriptors (16 bytes each)
    struct vring_desc desc[num];
    // A ring of available descriptor heads with free-running index.
    UINT16 avail_flags;
    UINT16 avail_idx;
    UINT16 available[num];
    UINT16 used_event_idx;
    // Padding to the next align boundary.
    char pad[];

    // A ring of used descriptor heads with free-running index.
    UINT16 used_flags;
    UINT16 used_idx;
    struct vring_used_elem used[num];
    UINT16 avail_event_idx;
};
#endif
/* We publish the used event index at the end of the available ring, and vice
 * versa. They are at the end for backwards compatibility. */
//#define vring_used_event(vr) ((vr)->avail->ring[(vr)->num])
//#define vring_avail_event(vr) (*(UINT16 *)&(vr)->used->ring[(vr)->num])

static inline void vring_init(struct AmbaIPC_VRING_s *vr, UINT16 num, const void *p, UINT32 align)
{
    UINT32 tmp_used;
    char *addr;
    const UINT16 *ring;

    vr->num = num;

    /* vr->desc = p; */
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&vr->desc, &p);
#else
    vr->desc = (struct vring_desc *)p;
#endif

    /* vr->avail = (struct vring_avail*) ((UINT32)p + num*sizeof(struct vring_desc)); */
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&addr, &p);
#else
    addr = (char *)p;
#endif
    addr = &addr[num*sizeof(struct vring_desc)];
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&vr->avail, &addr);
#else
    vr->avail = (struct vring_avail *)addr;
#endif

    /* vr->used = (void *)(((UINT32)&vr->avail->ring[num] + sizeof(UINT16) + align-1) & ~(align - 1)); */
    ring = &vr->avail->ring[num];
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&addr, &ring);
#else
    addr = (char *)ring;
#endif
    /* used: round up to 4k align*/
    addr = &addr[sizeof(UINT16) + align - 1U];
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&tmp_used, &addr);
#else
    tmp_used = (UINT32) addr;
#endif
    tmp_used &= ~(align - 1U);

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&vr->used, &tmp_used);
#else
    vr->used = (struct vring_used *)tmp_used;
#endif

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    vr->avail->flags = 0;
    vr->avail->idx = 0;
    vr->used->flags = 0;
    vr->used->idx = 0;
#endif
}

#if defined(CONFIG_AMBALINK_RPMSG_HIBER)
static inline UINT32 vring_size(UINT32 num, UINT32 align)
{
    return ((sizeof(struct vring_desc) * num + sizeof(UINT16) * (3 + num)
             + align - 1) & ~(align - 1))
           + sizeof(UINT16) * 3 + sizeof(struct vring_used_elem) * num;
}
#endif

/* The following is used with USED_EVENT_IDX and AVAIL_EVENT_IDX */
/* Assuming a given event_idx value from the other size, if
 * we have just incremented index from old to new_idx,
 * should we trigger an event? */
//static inline INT32 vring_need_event(UINT16 event_idx, UINT16 new_idx, UINT16 old)
//{
/* Note: Xen has similar logic for notification hold-off
 * in include/xen/interface/io/ring.h with req_event and req_prod
 * corresponding to event_idx + 1 and new_idx respectively.
 * Note also that req_event and req_prod in Xen start at 1,
 * event indexes in virtio start at 0. */
//  return (UINT16)(new_idx - event_idx - 1) < (UINT16)(new_idx - old);
//}

/*
#ifdef __KERNEL__
#include <linux/irqreturn.h>
struct virtio_device;
struct virtqueue;

struct virtqueue *vring_new_virtqueue(UINT32 num,
                                      UINT32 vring_align,
                                      struct virtio_device *vdev,
                                      bool weak_barriers,
                                      void *pages,
                                      void (*notify)(struct virtqueue *vq),
                                      void (*callback)(struct virtqueue *vq),
                                      const char *name);
void vring_del_virtqueue(struct virtqueue *vq);*/
/* Filter out transport-specific feature bits. */
/*void vring_transport_features(struct virtio_device *vdev);

irqreturn_t vring_interrupt(INT32 irq, void *_vq);
#endif*/ /* __KERNEL__ */
#endif /* LINUX_VIRTIO_RING_H */
