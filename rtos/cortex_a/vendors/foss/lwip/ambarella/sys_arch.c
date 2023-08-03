/**
 *  @file sys_arch.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Architecture environment, some compiler specific, some environment specific.
 *           Refer to doc/sys_arch.txt for detail.
 *
 */

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
// Need some OS APIs.
#define AMBA_KAL_SOURCE_CODE
#include <AmbaKAL.h>

#include <lwip/init.h>
#include <lwip/api.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include <lwip/udp.h>
#include <string.h>

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#define TX_QUEUE_ID         ((ULONG) 0x51554555)
#define TX_SEMAPHORE_ID     ((ULONG) 0x53454D41)

#define MYLWIP_RUNCORE          (0x01u)     ///< Using 1: core-0

#if !MEM_LIBC_MALLOC
#define MYLWIP_MAX_TASK_NUM     (10u)       ///< Max. task number
/** Stack size of task */
#if (TCPIP_THREAD_STACKSIZE > DEFAULT_THREAD_STACKSIZE)
#define MYLWIP_STACK_SIZE       (TCPIP_THREAD_STACKSIZE)
#else
#define MYLWIP_STACK_SIZE       (DEFAULT_THREAD_STACKSIZE)
#endif

/** 0: used, 1: in-use */
static u8_t            mylwip_tsk_board[MYLWIP_MAX_TASK_NUM] __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t mylwip_tsk_pool[MYLWIP_MAX_TASK_NUM] __attribute__((section(".bss.noinit")));
static u8_t            mylwip_stk_pool[MYLWIP_MAX_TASK_NUM][MYLWIP_STACK_SIZE] __attribute__((aligned (32))) __attribute__((section(".bss.noinit")));

#define MYLWIP_MAX_MBOX_NUM     (10u)
#ifdef SYS_MBOX_SIZE
#define MYLWIP_MBOX_SIZE        (SYS_MBOX_SIZE)
#else
#define MYLWIP_MBOX_SIZE        (1024u)
#endif

/** 0: used, 1: in-use */
static u8_t mylwip_mbox_board[MYLWIP_MAX_MBOX_NUM] __attribute__((section(".bss.noinit")));
static u8_t mylwip_mbox_pool[MYLWIP_MAX_MBOX_NUM][MYLWIP_MBOX_SIZE] __attribute__((aligned (32))) __attribute__((section(".bss.noinit")));
#endif // !MEM_LIBC_MALLOC



#if LWIP_TCPIP_CORE_LOCKING
#if LWIP_COMPAT_MUTEX
#error "No LWIP_COMPAT_MUTEX when LWIP_TCPIP_CORE_LOCKING"
#else
static sys_mutex_t tcpip_core_mutex;
static AMBA_KAL_TASK_t *curTskLocked = NULL;
#endif // LWIP_COMPAT_MUTEX
#endif // LWIP_TCPIP_CORE_LOCKING


// TODO: allocate all mbox pool and do not free if memory is enough.
void sys_init(void)
{
    memset(mylwip_tsk_board, 0, sizeof(mylwip_tsk_board));
    memset(mylwip_tsk_pool, 0, sizeof(mylwip_tsk_pool));
    memset(mylwip_stk_pool, 0, sizeof(mylwip_stk_pool));

    memset(mylwip_mbox_board, 0, sizeof(mylwip_mbox_board));
    memset(mylwip_mbox_pool, 0, sizeof(mylwip_mbox_pool));

#if LWIP_TCPIP_CORE_LOCKING
    {
        err_t err;

        err = sys_mutex_new(&tcpip_core_mutex);
        LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
    }
#endif
}

u32_t sys_now(void)
{
    UINT32 t;

    (void)AmbaKAL_GetSysTickCount(&t);

    return t;
}

u32_t sys_jiffies(void)
{
    UINT32 t;

    (void)AmbaKAL_GetSysTickCount(&t);

    return t;
}


err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    u32_t i, err;

    LWIP_ASSERT("mbox != NULL", mbox != NULL);
    LWIP_ASSERT("size > 0", size > 0);
    LWIP_ASSERT("size <= MYLWIP_MBOX_SIZE", (u32_t)size <= MYLWIP_MBOX_SIZE);

    for (i = 0; i < MYLWIP_MAX_MBOX_NUM; i++) {
        if (mylwip_mbox_board[i] == 0u) {
            mylwip_mbox_board[i] = 1u;
            break;
        }
    }
    LWIP_ASSERT("i < MYLWIP_MAX_MBOX_NUM", i < MYLWIP_MAX_MBOX_NUM);
    err = AmbaKAL_MsgQueueCreate(mbox,
                                 NULL,
                                 sizeof(void *),
                                 &(mylwip_mbox_pool[i][0]),
                                 size);
    LWIP_ASSERT("err == 0", err == 0);


    return 0;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    void *p;
    u32_t i;
    u32_t err;

    if (mbox) {
        p = mbox->tx_queue_start;
    } else {
        return;
    }
    err = AmbaKAL_MsgQueueDelete(mbox);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);

    for (i = 0; i < MYLWIP_MAX_MBOX_NUM; i++) {
        if (&(mylwip_mbox_pool[i][0]) == p) {
            break;
        }
    }
    LWIP_ASSERT("i < MYLWIP_MAX_MBOX_NUM", i < MYLWIP_MAX_MBOX_NUM);
    mylwip_mbox_board[i] = 0u;
}

inline int sys_mbox_valid(sys_mbox_t *mbox)
{
    if (!mbox) {
        return 0;
    }
    if (mbox->tx_queue_id != TX_QUEUE_ID) {
        return 0;
    }
    return 1;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    if (mbox) {
        mbox->tx_queue_id = 0;
    }
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    int err;
    ULONG m = (ULONG)msg;

    if (!sys_mbox_valid(mbox)) {
        return ERR_MEM;
    }

    err = AmbaKAL_MsgQueueSend(mbox, (void *)&m, AMBA_KAL_NO_WAIT);
    if (err) {
        return ERR_MEM;
    }

    return ERR_OK;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    int err;
    ULONG m = (ULONG)msg;

    if (!sys_mbox_valid(mbox)) {
        LWIP_ASSERT("!sys_mbox_valid(mbox)", 0);
    }

    err = AmbaKAL_MsgQueueSend(mbox, (void *)&m, AMBA_KAL_WAIT_FOREVER);
    LWIP_ASSERT("err == 0", err == 0);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int err;
    ULONG m;

    if (!sys_mbox_valid(mbox)) {
        LWIP_ASSERT("!sys_mbox_valid(mbox)", 0);
    }

    err = AmbaKAL_MsgQueueReceive(mbox, (void *)&m, AMBA_KAL_NO_WAIT);
    if (err) {
        *msg = 0;
        return SYS_MBOX_EMPTY;
    }
    *msg = (void *)m;

    return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    int err;
    ULONG m;

    if (!sys_mbox_valid(mbox)) {
        LWIP_ASSERT("!sys_mbox_valid(mbox)", 0);
    }

    if (timeout) {
        err = AmbaKAL_MsgQueueReceive(mbox, (void *)&m, timeout);
    } else {
        err = AmbaKAL_MsgQueueReceive(mbox, (void *)&m, AMBA_KAL_WAIT_FOREVER);
        // KAL return timeout when queue is empty.
    }
    if (err) {
        // TODO: other err
        *msg = 0;
        return SYS_ARCH_TIMEOUT;
    }
    *msg = (void *)m;

    return 0;
}



err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    int err;

    LWIP_ASSERT("sem != NULL", sem != NULL);
    err = AmbaKAL_SemaphoreCreate(sem, NULL, count);
    LWIP_ASSERT("err == 0", err == 0);

    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
    u32_t err;

    err = AmbaKAL_SemaphoreDelete(sem);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
}

int sys_sem_valid(sys_sem_t *sem)
{
    if (!sem) {
        return 0;
    }
    if (sem->tx_semaphore_id != TX_SEMAPHORE_ID) {
        return 0;
    }
    return 1;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    if (sem) {
        sem->tx_semaphore_id = 0;
    }
}

void sys_sem_signal(sys_sem_t *sem)
{
    UINT32 err;

    LWIP_ASSERT("sem != NULL", sem != NULL);
    err = AmbaKAL_SemaphoreGive(sem);
    LWIP_ASSERT("err == 0", err == 0);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    UINT32 err;

    LWIP_ASSERT("sem != NULL", sem != NULL);
    if (!sys_sem_valid(sem)) {
        LWIP_ASSERT("!sys_sem_valid(sem)", 0);
    }

    if (timeout) {
        err = AmbaKAL_SemaphoreTake(sem, timeout);
    } else {
        err = AmbaKAL_SemaphoreTake(sem, AMBA_KAL_WAIT_FOREVER);
        LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
    }
    if (err != 0u) {
        return SYS_ARCH_TIMEOUT;
    }

    return 0;
}


static u32_t mylwip_thread_alloc(AMBA_KAL_TASK_t **tsk, void **stk)
{
    u32_t i;

    for (i = 0; i < MYLWIP_MAX_TASK_NUM; i++) {
        if (mylwip_tsk_board[i] == 0u) {
            mylwip_tsk_board[i] = 1u;
            break;
        }
    }
    if (i < MYLWIP_MAX_TASK_NUM) {
        *tsk = &(mylwip_tsk_pool[i]);
        *stk = &(mylwip_stk_pool[i][0]);
        return 0;
    } else {
        return 1u;
    }
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    u32_t err;
    AMBA_KAL_TASK_t *tsk = NULL;
    void *stk = NULL;
    AMBA_KAL_TASK_ENTRY_f EntryFunction;

#if MEM_LIBC_MALLOC
    tsk = (AMBA_KAL_TASK_t *)malloc(sizeof(AMBA_KAL_TASK_t));
    LWIP_ASSERT("tsk != NULL", tsk != NULL);
    stk = (void *)malloc(stacksize);
    LWIP_ASSERT("stk != NULL", stk != NULL);
#else
    LWIP_ASSERT("stacksize <= MYLWIP_STACK_SIZE", (u32_t)stacksize <= MYLWIP_STACK_SIZE);
    err = mylwip_thread_alloc(&tsk, &stk);
    LWIP_ASSERT("err == 0", err == 0);
#endif

    memset(tsk, 0, sizeof(AMBA_KAL_TASK_t));
    memset(stk, 0, stacksize);
    memcpy(&EntryFunction, &thread, sizeof(void *));
    err = AmbaKAL_TaskCreate(tsk,                   /* pTask */
                             (char *)name,          /* pTaskName */
                             prio,                  /* Priority */
                             EntryFunction,         /* void (*EntryFunction)(UINT32) */
                             arg,                   /* EntryArg */
                             (void *)stk,           /* pStackBase */
                             stacksize,             /* StackByteSize */
                             AMBA_KAL_DONT_START);  /* AutoStart or not*/
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
#ifdef CONFIG_KAL_THREADX_SMP
    err = AmbaKAL_TaskSetSmpAffinity(tsk, MYLWIP_RUNCORE);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
#endif
    err = AmbaKAL_TaskResume(tsk);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);

    return tsk;
}


#if SYS_LIGHTWEIGHT_PROT
sys_prot_t sys_arch_protect(void)
{
    /* TX_INTERRUPT_SAVE_AREA */
    unsigned long interrupt_save;

    TX_DISABLE;

    return (sys_prot_t)interrupt_save;
}

void sys_arch_unprotect(sys_prot_t v)
{
    /* TX_INTERRUPT_SAVE_AREA */
    unsigned long interrupt_save = v;

    TX_RESTORE;
}
#endif

#if !LWIP_COMPAT_MUTEX

err_t
sys_mutex_new(sys_mutex_t *mutex)
{
    int err;

    LWIP_ASSERT("mutex != NULL", mutex != NULL);

    err = AmbaKAL_MutexCreate(mutex, NULL);
    if (err) {
        return ERR_MEM;
    }
    return ERR_OK;
}

void
sys_mutex_lock(sys_mutex_t *mutex)
{
    u32_t err;

    LWIP_ASSERT("mutex != NULL", mutex != NULL);
    err = AmbaKAL_MutexTake(mutex, AMBA_KAL_WAIT_FOREVER);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
}

void
sys_mutex_unlock(sys_mutex_t *mutex)
{
    u32_t err;

    LWIP_ASSERT("mutex != NULL", mutex != NULL);
    err = AmbaKAL_MutexGive(mutex);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
}

void
sys_mutex_free(sys_mutex_t *mutex)
{
    LWIP_ASSERT("mutex != NULL", mutex != NULL);
    AmbaKAL_MutexDelete(mutex);
}

#endif /* !LWIP_COMPAT_MUTEX */

void
sys_arch_msleep(u32_t ms)
{
    (void)AmbaKAL_TaskSleep(ms);
}

#ifdef LWIP_MARK_TCPIP_THREAD
static AMBA_KAL_TASK_t *tcpip_tsk = NULL;

void
sys_arch_mark_tcpip_thread(void)
{
    u32_t err;

    err = AmbaKAL_TaskIdentify(&tcpip_tsk);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
}

#ifdef LWIP_ASSERT_CORE_LOCKED
inline void
sys_arch_assert_core_locked(void)
{
    if (tcpip_tsk != NULL) {
        u32_t err;
        AMBA_KAL_TASK_t *t;

        err = AmbaKAL_TaskIdentify(&t);
    LWIP_ASSERT("err == ERR_OK", err == ERR_OK);
#if LWIP_TCPIP_CORE_LOCKING
        if (curTskLocked != NULL) {
            LWIP_ASSERT("Function called without core lock", t == curTskLocked);
        }
#else /* LWIP_TCPIP_CORE_LOCKING */
        LWIP_ASSERT("Function called from wrong thread", t == tcpip_tsk);
#endif /* LWIP_TCPIP_CORE_LOCKING */
    }
}
#endif
#endif /* LWIP_MARK_TCPIP_THREAD */

#if LWIP_TCPIP_CORE_LOCKING
inline void
sys_arch_lock_tcpip_core(void)
{
    sys_mutex_lock(&tcpip_core_mutex);
    (void)AmbaKAL_TaskIdentify(&curTskLocked);
}

inline void
sys_arch_unlock_tcpip_core(void)
{
    curTskLocked = NULL;
    sys_mutex_unlock(&tcpip_core_mutex);
}
#endif /* LWIP_TCPIP_CORE_LOCKING */

#if LWIP_PERF

/*
 * Example code
 */

#define LWIP_PERF_BUFSIZE   (128)

void sys_arch_perf_start(const char *f, u32_t l)
{
    char buf[LWIP_PERF_BUFSIZE];
    u32_t idx, cidx, t;

    idx = AmbaUtility_StringPrintStr(buf, LWIP_PERF_BUFSIZE,
                                     "[S >], %s(), ",
                                     1,
                                     (const char * const *) &f);
    cidx = idx;
    idx = AmbaUtility_StringPrintUInt32(&(buf[cidx]), (LWIP_PERF_BUFSIZE - cidx),
                                        "Ln-%d, ",
                                        1, &l);
    cidx += idx;
    t = sys_now();
    idx = AmbaUtility_StringPrintUInt32(&(buf[cidx]), (LWIP_PERF_BUFSIZE - cidx),
                                        "t: %d",
                                        1, &t);
    cidx += idx;
    buf[cidx] = '\0';
    AmbaPrint_PrintStr5("%s", &(buf[0]), NULL, NULL, NULL, NULL);
}

void sys_arch_perf_stop(const char *f, u32_t l, const char *str)
{
    char buf[LWIP_PERF_BUFSIZE];
    u32_t idx, cidx, t;

    idx = AmbaUtility_StringPrintStr(buf, LWIP_PERF_BUFSIZE,
                                     "[< E], %s(), ",
                                     1,
                                     (const char * const *) &f);
    cidx = idx;
    idx = AmbaUtility_StringPrintUInt32(&(buf[cidx]), (LWIP_PERF_BUFSIZE - cidx),
                                        "Ln-%d, ",
                                        1, &l);
    cidx += idx;
    t = sys_now();
    idx = AmbaUtility_StringPrintUInt32(&(buf[cidx]), (LWIP_PERF_BUFSIZE - cidx),
                                        "t: %d, ",
                                        1, &t);
    cidx += idx;
    idx = AmbaUtility_StringPrintStr(&(buf[cidx]), (LWIP_PERF_BUFSIZE - cidx),
                                     "%s", 1,
                                     (const char * const *) &str);
    cidx += idx;
    buf[cidx] = '\0';
    AmbaPrint_PrintStr5("%s", (const char *)&(buf[0]), NULL, NULL, NULL, NULL);
}
#endif /* LWIP_PERF */

/* dummy wrapper start */
static struct netif netif_eth[2];

void sys_fd_set(INT32 n, fd_set *p)
{
  FD_SET(n, p);
}

void sys_fd_clr(INT32 n, fd_set *p)
{
  FD_CLR(n, p);
}

INT32 sys_fd_isset(INT32 n, fd_set *p)
{
  return FD_ISSET(n, p);
}

void sys_fd_zero(fd_set *p)
{
  FD_ZERO(p);
}

void sys_lwip_enetinit(UINT32 Idx)
{
  extern err_t ethernetif_init(struct netif *netif);
  static INT32 lwip_inited = 0;
  struct netif *netif = &netif_eth[Idx];

  if (lwip_inited == 0) {
    tcpip_init(NULL, NULL);
    lwip_inited = 1;
  }

  memset(netif, 0, sizeof(struct netif));
  netif->name[0] = '0' + Idx;
  netif->name[1] = '\0';

  netif_add(netif, NULL, NULL, NULL, NULL, ethernetif_init, netif_input);
  if (NULL == netif_default) {
    netif_set_default(netif);
  }

  netif_set_up(netif);
  netif_set_link_up(netif);
}

void sys_lwip_ifconfig(UINT32 Idx, UINT32 ip, UINT32 mask, UINT32 gw)
{
  ip4_addr_t ip4, mask4, gw4;
  struct netif *netif = &netif_eth[Idx];

  ip4.addr = ip;
  mask4.addr = mask;
  gw4.addr = gw;

  netif_set_addr(netif, &ip4, &mask4, &gw4);
}

void sys_lwip_pbuf_alloc(UINT32 header, UINT16 length, void **addr, void **priv)
{
  struct pbuf *p;

  p = pbuf_alloc(header, length, PBUF_RAM);
  *priv = p;
  *addr = p->payload;
}

INT sys_lwip_udp_sendto(void *priv, UINT32 ip, UINT16 port)
{
  ip_addr_t ipaddr;
  static struct udp_pcb *upcb = NULL;
  struct pbuf *p = (struct pbuf *) priv;

  if (upcb == NULL) {
    upcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  }

  ipaddr.addr = ip;
  return (INT)udp_sendto(upcb, p, &ipaddr, port);
}

INT sys_lwip_pbuf_free(void *priv)
{
  struct pbuf *p = (struct pbuf *) priv;
  return (INT)pbuf_free(p);
}

UINT32 sys_netif_get_by_index(UINT8 idx)
{
  struct netif *eth0 = netif_get_by_index(idx);
  if (eth0 == NULL) {
    return 0U;
  }
  return eth0->ip_addr.addr;
}
/* dummy api end */


#if defined(ETHARP_SUPPORT_VLAN) && ETHARP_SUPPORT_VLAN

/*
 * Test case:
 *  - [Linux] side
 *      # modprobe 8021q
 *      # vconfig add eth1 10
 *      # vconfig set_flag eth1.10 1
 *      # ifconfig eth1.10 192.168.2.100
 *  - [RTOS] side
 *      > enet init 00:11:22:33:44:55
 *      > enet lwipinit
 *      > enet lwip ifconfig 192.168.2.10
 *  - [Linux] side check with wireshark
 *      $ ping 192.168.2.10
 */

/*
 * See: LWIP_HOOK_VLAN_CHECK(netif, eth_hdr, vlan_hdr):
 * Called from ethernet_input() if VLAN support is enabled
 *
 * Arguments:
 * - netif: struct netif on which the packet has been received
 * - eth_hdr: struct eth_hdr of the packet
 * - vlan_hdr: struct eth_vlan_hdr of the packet
 *
 * Return values:
 * - 0: Packet must be dropped.
 * - != 0: Packet must be accepted.
 */
int myhook_vlan_chk(struct netif *netif, struct eth_hdr *eth_hdr, struct eth_vlan_hdr *vlan_hdr)
{
    (void)netif;
    (void)eth_hdr;
    (void)vlan_hdr;

    return 1;
}

/*
 * See LWIP_HOOK_VLAN_SET:
 * Hook can be used to set prio_vid field of vlan_hdr. If you need to store data
 * on per-netif basis to implement this callback.
 * Called from ethernet_output() if VLAN support.
 *
 * Arguments:
 * - netif: struct netif that the packet will be sent through
 * - p: struct pbuf packet to be sent
 * - src: source eth address
 * - dst: destination eth address
 * - eth_type: ethernet type to packet to be sent\n
 *
 * Return values:
 * - <0: Packet shall not contain VLAN header.
 * - 0 <= return value <= 0xFFFF: Packet shall contain VLAN header. Return value is prio_vid in host byte order.
 */
s32_t myhook_vlan_set(struct netif *netif, struct pbuf *pbuf, const struct eth_addr *src, const struct eth_addr *dst, u16_t eth_type)
{
    (void)netif;
    (void)pbuf;
    (void)src;
    (void)dst;
    (void)eth_type;

    // Return VLAN.prio_vid. check test case.
    return 10;
}

#endif // defined(ETHARP_SUPPORT_VLAN) && ETHARP_SUPPORT_VLAN

