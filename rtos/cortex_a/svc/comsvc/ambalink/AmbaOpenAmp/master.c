/**
 * @file master.c
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
 * @details The instances of OpenAMP implementation.
 *
 */


#include <amba_platform_info.h>
#include <amba_rsc_table.h>

#include <AmbaWrap.h>
#include "AmbaPrint.h"
#include "AmbaINT.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaLinkPrivate.h"
#include "AmbaLink.h"


struct amba_ept {
    /** end-point */
    struct rpmsg_endpoint ept;
    /** is channel acked/established */
    uint32_t isAcked;
    /** call-back handler of message */
    AMBA_IPC_MSG_HANDLER_f handler;
};


static void *platform;

extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

#define WriteWord(Addr, d) (*((volatile UINT32 *) (Addr)) = (d))
extern void AmbaOamp_kick(uint32_t id);
static void *vdevFlag = NULL;

static AMBA_KAL_TASK_t rpmsgRxHandlerTask __attribute__((section(".bss.noinit")));
static UINT8 rpmsgRxHandlerTaskStack[0x4000] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t rpmsgRxHandlerFlag;


// Disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void AmbaOamp_Mst_svq_rx_isr(UINT32 args, UINT32 UserArg)
{
    /* Get Ack */
    (void)UserArg;

    WriteWord(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_CLEAR_OFFSET,
              1 << (args - AXI_SOFT_IRQ0));
}

static void AmbaOamp_Mst_rvq_rx_isr(UINT32 args, UINT32 UserArg)
{
    /* Get Msg */
    (void)UserArg;

    WriteWord(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_CLEAR_OFFSET,
              1 << (args - AXI_SOFT_IRQ0));

    if (vdevFlag) {
        AmbaKAL_EventFlagSet(vdevFlag, 0x01);
    }

    /* VRING_IRQ_C1_TO_C0_ACK = AXI_SOFT_IRQ(3) */
//    AmbaOamp_kick(3);
}
#pragma GCC pop_options
// Disable NEON registers usage in ISR

extern void AmbaOamp_rpmsgRxHandler(void *platform);

static void *rpmsgRxHandlerTask_fn(void *arg)
{
    UINT32 dummy;

    (void)arg;

    while (1) {
        AmbaKAL_EventFlagGet(&rpmsgRxHandlerFlag, 0x01, 1U, 1U, &dummy, AMBA_KAL_WAIT_FOREVER);
#if 1
        AmbaOamp_rpmsgRxHandler(platform);
#else
        {
            struct remoteproc *rproc = (struct remoteproc *)platform;

            remoteproc_get_notification(rproc, 3);
        }
#endif
    }
    return NULL;
}


static void AmbaOamp_master_init(void *priv)
{
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 Status;

    AmbaKAL_EventFlagCreate(&rpmsgRxHandlerFlag, "rpmsgRxHandlerFlag");
    vdevFlag = &rpmsgRxHandlerFlag;

    // TODO: Set irq data in AmbaLinkCtrl or resource table
    IntConfig.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;        /* Sensitivity type */
    IntConfig.IrqType     = AMBA_INT_IRQ;                       /* Interrupt type: IRQ or FIQ */
    IntConfig.CpuTargets  = AmbaLinkCtrl.AmbaLinkRunTarget;     /* Target cores */

    (void)AmbaRTSL_GicIntConfig(VRING_IRQ_C0_TO_C1_ACK, &IntConfig, AmbaOamp_Mst_svq_rx_isr, 0U);
    (void)AmbaRTSL_GicIntEnable(VRING_IRQ_C0_TO_C1_ACK);

    IntConfig.TriggerType = AMBA_INT_HIGH_LEVEL_TRIGGER;        /* Sensitivity type */
    IntConfig.IrqType     = AMBA_INT_IRQ;                       /* Interrupt type: IRQ or FIQ */
    IntConfig.CpuTargets  = AmbaLinkCtrl.AmbaLinkRunTarget;     /* Target cores */

    (void)AmbaRTSL_GicIntConfig(VRING_IRQ_C0_TO_C1_KICK, &IntConfig, AmbaOamp_Mst_rvq_rx_isr, 0U);
    (void)AmbaRTSL_GicIntEnable(VRING_IRQ_C0_TO_C1_KICK);

    AmbaWrap_memset(&rpmsgRxHandlerTask, 0, sizeof(rpmsgRxHandlerTask));
    Status = AmbaKAL_TaskCreate(&rpmsgRxHandlerTask,
                                "rpmsgRxHandler Task",
                                250,
                                rpmsgRxHandlerTask_fn,
                                priv,
                                (void *) &(rpmsgRxHandlerTaskStack[0]),
                                (UINT32)sizeof(rpmsgRxHandlerTaskStack),
                                0U);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("can't not create AmbaLink task (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    Status = AmbaKAL_TaskSetSmpAffinity(&rpmsgRxHandlerTask,
                                        0x01 & AMBA_KAL_CPU_CORE_MASK);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("AmbaKAL_TaskSetSmpAffinity is failed! (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    Status = AmbaKAL_TaskResume(&rpmsgRxHandlerTask);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("AmbaKAL_TaskResume is failed! (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
}

/* Local-end-point */
//static const char *mst_name =  "echo_cortex";
//static struct amba_ept lept;
static UINT32 shutdown_req = 0;

//static AMBA_KAL_TASK_t echoTask __attribute__((section(".bss.noinit")));
//static UINT8 echoTaskStack[0x4000] GNU_SECTION_NOZEROINIT;

static int AmbaOamp_Mst_rpmsg_endpoint_cb(struct rpmsg_endpoint *ept,
                             void *                 data,
                             size_t                 len,
                             uint32_t               src,
                             void *                 priv)
{
    (void)priv;
    (void)src;

    if (rpmsg_send(ept, data, len) < 0) {
        AmbaPrint_PrintStr5("rpmsg_send() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    return RPMSG_SUCCESS;
}

static void AmbaOamp_Mst_rpmsg_svc_unbind(struct rpmsg_endpoint *ept)
{
    (void)ept;
    shutdown_req = 1;
}

#if 0 // test case

static void *echoTask_fn(void *arg)
{
    (void)arg;
    while (1) {
//        platform_poll(priv);
        AmbaKAL_TaskSleep(1000);

        if (shutdown_req) {
            break;
        }
    }
    rpmsg_destroy_ept(&lept);
    return NULL;
}

static void AmbaOamp_Mst_app(struct rpmsg_device *rdev, void *priv)
{
    INT32 ret;
    UINT32 Status;
    size_t s;

    AmbaWrap_memset(&lept, 0, sizeof(lept));
    s = AmbaUtility_StringLength(mst_name);
    if (s > RPMSG_NAME_SIZE) {
        s = RPMSG_NAME_SIZE;
    }
    AmbaWrap_memcpy(lept.name, mst_name, s);
    ret = rpmsg_create_ept(&lept,
                           rdev,
                           mst_name,
                           RPMSG_ADDR_ANY,
                           RPMSG_ADDR_ANY,
                           AmbaOamp_Mst_rpmsg_endpoint_cb,
                           AmbaOamp_Mst_rpmsg_svc_unbind);
    if (ret) {
        AmbaPrint_PrintStr5("rpmsg_create_ept() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }

    AmbaWrap_memset(&echoTask, 0, sizeof(echoTask));
    Status = AmbaKAL_TaskCreate(&echoTask,
                                "echo Task",
                                250,
                                echoTask_fn,
                                priv,
                                (void *) &(echoTaskStack[0]),
                                (UINT32)sizeof(echoTaskStack),
                                0U);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("can't not create AmbaLink task (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    Status = AmbaKAL_TaskSetSmpAffinity(&echoTask,
                                        0x01 & AMBA_KAL_CPU_CORE_MASK);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("AmbaKAL_TaskSetSmpAffinity is failed! (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    Status = AmbaKAL_TaskResume(&echoTask);
    if (Status) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("AmbaKAL_TaskResume is failed! (0x%08x)", Status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    return;
}
#endif

struct rpmsg_ns_msg {
    /** Name of channel */
    char   name[32];
    /** RPMSG address */
    UINT32 addr;
    /** RPMSG flags */
    UINT32 flags;
} __attribute__((packed));

#if 0
static void AmbaOamp_Mst_rpmsg_ns_bind_cb(struct rpmsg_device *rdev,
                                  const char *name, uint32_t dest)
{
    // TODO: compare name
    (void)rdev;
    (void)name;

    lept.dest_addr = dest;

    // TODO: Master
    {
        struct rpmsg_ns_msg ns_msg;
        unsigned int i;
        int ret;

        ns_msg.flags = 0; //RPMSG_NS_CREATE
        ns_msg.addr  = lept.addr;
        for (i = 0; i < sizeof(ns_msg.name); i++) {
            ns_msg.name[i] = ns_msg.name[i];
        }
        ret = rpmsg_send(&lept, &ns_msg, sizeof(ns_msg));
        (void)ret;
    }
}
#endif

void AmbaOamp_init_master(void)
{
    UINT32 ret;
    struct rpmsg_device *rpdev;

    ret = platform_init(RPMSG_HOST, RSC_ID_MASTER, &platform);
    if (ret) {
        AmbaPrint_PrintStr5("platform_init() failed", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
        AmbaOamp_master_init(platform);
        rpdev = platform_create_rpmsg_vdev(platform,
                                           RPMSG_HOST,
                                           NULL,
                                           NULL);
//                                           AmbaOamp_Mst_rpmsg_ns_bind_cb);
        if (rpdev == NULL) {
            AmbaPrint_PrintStr5("platform_create_rpmsg_vdev() failed", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
#if 0 // test case
        } else {
            AmbaOamp_Mst_app(rpdev, platform);
//            platform_release_rpmsg_vdev(rpdev);
#endif
        }
    }

//    platform_cleanup(platform);
}


/*
 * Wrapper to be compatible with old VQ
 * @return struct amba_ept *
 */
void * AmbaOamp_Alloc(const char *name, AMBA_IPC_MSG_HANDLER_f cb)
{
    struct amba_remoteproc *amba_rproc = platform;
    struct rpmsg_virtio_device *rpmsg_vdev;
    struct rpmsg_device *rdev;
    struct amba_ept *aept;
    int ret;
    size_t s;

    // TODO: Add aept to list for NS service.
    aept = metal_allocate_memory(sizeof(*aept));
    if (aept) {
        rpmsg_vdev = amba_rproc->rvdev;
        rdev = rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);

        aept->isAcked = 0;
        AmbaWrap_memset(aept->ept.name, 0, sizeof(aept->ept.name));
        s = AmbaUtility_StringLength(name);
        if (s > RPMSG_NAME_SIZE) {
            s = RPMSG_NAME_SIZE;
        }
        AmbaWrap_memcpy(aept->ept.name, name, s);
        // Will also register
        ret = rpmsg_create_ept(&(aept->ept),
                           rdev,
                           name,
                           RPMSG_ADDR_ANY,
                           RPMSG_ADDR_ANY,
                           AmbaOamp_Mst_rpmsg_endpoint_cb,
                           AmbaOamp_Mst_rpmsg_svc_unbind);
        if (ret) {
            AmbaPrint_PrintStr5("rpmsg_create_ept() failed", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return NULL;
        }

        aept->handler = cb;
    }

    return (void *)aept;
}

enum rpmsg_ns_flags {
    RPMSG_NS_CREATE = 0,
    RPMSG_NS_DESTROY = 1,
};

int AmbaOamp_RegisterChannel(void *handle, const char *pRemote)
{
    int ret = -1;

    (void) pRemote;
    if (handle) {
        ret = 0;
    }

    return ret;
}

int AmbaOamp_UnregisterChannel(void *handle)
{
    struct rpmsg_endpoint *ept;
    int ret = -1;
    unsigned int i;

    if (handle) {
        struct rpmsg_ns_msg nsm;

        ept = (struct rpmsg_endpoint *)handle;
        nsm.addr  = ept->addr;
        nsm.flags = RPMSG_NS_DESTROY;

        ret = rpmsg_send(ept, &nsm, sizeof(nsm));
        for (i = 0; i < sizeof(ept->name); i++) {
            nsm.name[i] = ept->name[i];
        }

        // TODO wait....
    }

    return ret;
}

int AmbaOamp_TrySend(void *handle, const void *data, int len)
{
    struct rpmsg_endpoint *ept;
    int ret = -1;


    if (handle) {
        ept = (struct rpmsg_endpoint *)handle;

        ret = rpmsg_trysend(ept, data, len);

        // TODO wait....
    }

    return ret;
}

int AmbaOamp_Send(void *handle, void *data, int len)
{
    struct rpmsg_endpoint *ept;
    int ret = -1;


    if (handle) {
        ept = (struct rpmsg_endpoint *)handle;

        ret = rpmsg_send(ept, data, len);

        // TODO wait....
    }

    return ret;
}
