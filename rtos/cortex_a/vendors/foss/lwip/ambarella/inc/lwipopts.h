/**
 *  @file lwipopts.h
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
 *  @details Customized options.
 *           Refer to src/include/lwip/opt.h for detail.
 *
 */


#ifndef AMBA_LWIPOPTS_H__
#define AMBA_LWIPOPTS_H__

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          (0)
/* #define LWIP_NETCONN                    (1) */
#define LWIP_SOCKET                     (1)
#define MEM_LIBC_MALLOC                 (0)   // No malloc(), free()
#define MEM_ALIGNMENT                   (4)
#define LWIP_HAVE_LOOPIF                (1)
#define SYS_LIGHTWEIGHT_PROT            (1)
#if SYS_LIGHTWEIGHT_PROT
#define sys_prot_t                      u32_t
#endif

#define LWIP_TCPIP_CORE_LOCKING         (SYS_LIGHTWEIGHT_PROT)
//#define LWIP_TCPIP_CORE_LOCKING_INPUT   (1)

extern void sys_arch_assert_core_locked(void);
#define LWIP_ASSERT_CORE_LOCKED   sys_arch_assert_core_locked

extern void sys_arch_mark_tcpip_thread(void);
#define LWIP_MARK_TCPIP_THREAD    sys_arch_mark_tcpip_thread

#if LWIP_TCPIP_CORE_LOCKING
extern void sys_arch_lock_tcpip_core(void);
#define LOCK_TCPIP_CORE           sys_arch_lock_tcpip_core

extern void sys_arch_unlock_tcpip_core(void);
#define UNLOCK_TCPIP_CORE         sys_arch_unlock_tcpip_core
#endif /* LWIP_TCPIP_CORE_LOCKING */

/* allow 48 bsd sockets */
#define MEMP_NUM_NETCONN                (48)
/* the number of struct netbufs. */
#define MEMP_NUM_NETBUF                 (24)

/* Each element is pointer and Threadx use WORD (4-byes) */
#define SYS_MBOX_SIZE                   ((1024 * sizeof(u32_t)) / 4)

/* MEM_SIZE: the size of the heap memory.  */
#define MEM_SIZE                        (8 * TCP_SND_BUF)
// Disable memoy check for higher throughput.
#define MEMP_OVERFLOW_CHECK             (0)
#define MEMP_SANITY_CHECK               (0)
#define MEM_OVERFLOW_CHECK              (0)
#define MEM_SANITY_CHECK                (0)

#define MEMP_NUM_PBUF                   (48)
#define MEMP_NUM_UDP_PCB                (8)
#define MEMP_NUM_TCP_PCB                (8)

/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool */
#define PBUF_POOL_SIZE                  (48)


/*
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP *tries* to put all data
 * to be sent into one single pbuf.
 */
#define LWIP_NETIF_TX_SINGLE_PBUF       (1)

/* TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages */
#define TCPIP_MBOX_SIZE                 (SYS_MBOX_SIZE)
/* TCP_MSS: TCP Maximum segment size. */
#ifdef CONFIG_ETH_FRAMES_SIZE
#define TCP_MSS                         (CONFIG_ETH_FRAMES_SIZE - 54U)
#else
#define TCP_MSS                         (1460)    /* Use 3786 for Jumbo Frame */
#endif

/* TCP_SND_BUF: TCP sender buffer space (bytes) > 2*TCP_MSS */
#define TCP_SND_BUF                     ((65535)/TCP_MSS*TCP_MSS)
/* TCP_WND: The size of a TCP window, > 2*TCP_MSS */
#define TCP_WND                         ((65535)/TCP_MSS*TCP_MSS)

/* should be at least as big as TCP_SND_QUEUELEN */
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN


#define DEFAULT_RAW_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         TCPIP_MBOX_SIZE

/* Minimal changes to opt.h required for etharp unit tests: */
#define ETHARP_SUPPORT_STATIC_ENTRIES   1

/* Note: For better throughput of Rx, need higher priority than the one of net interface driver. */
/* TCP/IP thread. */
#define TCPIP_THREAD_STACKSIZE          (0x4000)
#define TCPIP_THREAD_PRIO               (249)
/* Other thread. */
#define DEFAULT_THREAD_STACKSIZE        (TCPIP_THREAD_STACKSIZE)
#define DEFAULT_THREAD_PRIO             (TCPIP_THREAD_PRIO + 1)


/* Hook APIs are declared at ambarella/inc/arch/hook.h */
#define LWIP_HOOK_FILENAME  <arch/hook.h>

/*
 * The origignal condition in opt.h is to check LWIP_HOOK_VLAN_SET,
 * but LWIP_HOOK_VLAN_SET will be defined in LWIP_HOOK_FILENAME
 * and will be included later than opt.h
 */
#define ETHARP_SUPPORT_VLAN (0)
#if ETHARP_SUPPORT_VLAN
#define PBUF_LINK_HLEN                  (18 + ETH_PAD_SIZE)
// else use default: (14 + ETH_PAD_SIZE)
#endif

/*
 *  LWIP_STATS==1: Enable statistics collection in lwip_stats.
 *  (It'll impact throughput, disable it for release)
 */
#define LWIP_STATS          (0)

/*
 * Checksum offloading by HW
 */
#ifdef CONFIG_BUILD_SSP_ENET
#define CHECKSUM_BY_LWIP 0
#else
#define CHECKSUM_BY_LWIP 1
#endif
#define CHECKSUM_GEN_IP     CHECKSUM_BY_LWIP
#define CHECKSUM_GEN_UDP    CHECKSUM_BY_LWIP
#define CHECKSUM_GEN_TCP    CHECKSUM_BY_LWIP
#define CHECKSUM_GEN_ICMP   CHECKSUM_BY_LWIP

#define CHECKSUM_CHECK_IP   CHECKSUM_BY_LWIP
#define CHECKSUM_CHECK_UDP  CHECKSUM_BY_LWIP
#define CHECKSUM_CHECK_TCP  CHECKSUM_BY_LWIP
#define CHECKSUM_CHECK_ICMP CHECKSUM_BY_LWIP


/*
 * Let lwIP provide ERRNO values and the 'errno' variable.
 * cf. lwip-version/src/include/lwip/errno.h
 */
#define LWIP_PROVIDE_ERRNO
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_ERRNO_STDINCLUDE
#endif



#ifdef LWIP_DEBUG
#define LWIP_DBG_MIN_LEVEL   LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON    LWIP_DBG_ON
#define ETHARP_DEBUG         LWIP_DBG_ON
#define NETIF_DEBUG          LWIP_DBG_ON
#define PBUF_DEBUG           LWIP_DBG_ON
#define API_LIB_DEBUG        LWIP_DBG_ON
#define API_MSG_DEBUG        LWIP_DBG_ON
#define SOCKETS_DEBUG        LWIP_DBG_ON
#define ICMP_DEBUG           LWIP_DBG_ON
#define IGMP_DEBUG           LWIP_DBG_ON
#define INET_DEBUG           LWIP_DBG_ON
#define IP_DEBUG             LWIP_DBG_ON
#define IP_REASS_DEBUG       LWIP_DBG_ON
#define RAW_DEBUG            LWIP_DBG_ON
#define MEM_DEBUG            LWIP_DBG_ON
#define MEMP_DEBUG           LWIP_DBG_ON
#define SYS_DEBUG            LWIP_DBG_ON
#define TIMERS_DEBUG         LWIP_DBG_ON
#define TCP_DEBUG            LWIP_DBG_ON
#define TCP_INPUT_DEBUG      LWIP_DBG_ON
#define TCP_FR_DEBUG         LWIP_DBG_ON
#define TCP_RTO_DEBUG        LWIP_DBG_ON
#define TCP_CWND_DEBUG       LWIP_DBG_ON
#define TCP_WND_DEBUG        LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG     LWIP_DBG_ON
#define TCP_RST_DEBUG        LWIP_DBG_ON
#define TCP_QLEN_DEBUG       LWIP_DBG_ON
#define UDP_DEBUG            LWIP_DBG_ON
#define TCPIP_DEBUG          LWIP_DBG_ON
#define SLIP_DEBUG           LWIP_DBG_ON
#define DHCP_DEBUG           LWIP_DBG_ON
#define AUTOIP_DEBUG         LWIP_DBG_ON
#define DNS_DEBUG            LWIP_DBG_ON
#define IP6_DEBUG            LWIP_DBG_ON
#define DHCP6_DEBUG          LWIP_DBG_ON
#endif

/*
 * LWIP_PERF: Enable performance testing for lwIP
 * (if enabled, arch/perf.h is included)
 * Enable by setting it as 1
 */
#define LWIP_PERF           (0)


#endif /* AMBA_LWIPOPTS_H__ */

