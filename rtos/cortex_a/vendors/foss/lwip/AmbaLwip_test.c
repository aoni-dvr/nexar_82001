/**
 *  @file AmbaLwip_test.c
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
 *  @details Test code for lwIP
 *
 */

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG

// Here are wrappers for standard headers.
#include <string.h>

#include <lwip/tcpip.h>
#include <netif/etharp.h>

#include <lwip/sockets.h>
#include <lwip/apps/lwiperf.h>

#include <platform/AmbaKAL.h>

#include <AmbaMisraFix.h>
#include <platform/AmbaUART.h>
#include <platform/AmbaUART_Def.h>
#include <platform/AmbaENET_Def.h>

#include <comsvc/shell/AmbaShell.h>
#include <comsvc/misc/AmbaUtility.h>
#include <comsvc/printf/AmbaPrint.h>
#if 0
#include "AmbaDataType.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaTest.h"
#include "AmbaUtility.h"

#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <lwip/ethip6.h>
#include <lwip/etharp.h>
#include <lwip/api.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include <lwip/init.h>
#include <netif/etharp.h>

#include <lwip/apps/lwiperf.h>

#include <enet/driver/inc/AmbaENET_Def.h>
#include <enet/driver/inc/AmbaENET.h>
extern void AmbaPrint_PrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
extern void AmbaPrint_PrintInt5(const char *FmtStr, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
extern void AmbaPrint_PrintStr5(const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5);
#endif

extern void EnetPrint(const char *func, const UINT32 line, const char *color, const char *msg);

extern err_t ethernetif_init(struct netif *netif);

static struct netif netif_eth0;

static UINT32 Pause;
static UINT32 IPARG;
static INT32 TestPri[2] = {250, 251};

#define RXTEST           0x00U
#define TXTEST           0x01U
#define RXUDP_LWIP       0x09U
#define RXTCP_LWIP       0x0aU
#define RXUDP_BSD        0x0bU
#define RXTCP_BSD        0x0cU
#define TXUDP_LWIP       0x0dU
#define TXTCP_LWIP       0x0eU
#define TXUDP_BSD        0x0fU
#define TXTCP_BSD        0x10U

//#define ETH_FRAMES_SIZE (1514U)
#define NXUDPLEN        (ETH_FRAMES_SIZE - 44U)
#define NXTCPLEN        (ETH_FRAMES_SIZE - 54U)

static void LwIPUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("lwip stack:\n");
    PrintFunc("  *init lwip (after enet init)\n");
    PrintFunc("    lwip init\n");
    PrintFunc("  *config IP\n");
    PrintFunc("    lwip ifconfig 192.168.1.100\n");
    PrintFunc("  *throughput test (by bsd API)\n");
    PrintFunc("    lwip bsd_udp_tx 192.168.1.2\n");
    PrintFunc("    lwip bsd_udp_rx\n");
    PrintFunc("    lwip bsd_tcp_tx 192.168.1.2\n");
    PrintFunc("    lwip bsd_tcp_rx\n");
}

//192.168.1.100->0x64 01 a8 c0
static INT32 Str2NetOdrIP(const char *src, UINT32 *addr)
{
    INT32 saw_digit, octets;
    char ch;
    UINT8 tmp[4], *tp;
    INT32 Ret = 0;

    saw_digit   = 0;
    octets      = 0;
    tp = tmp;
    *(tp) = 0;
    ch = *src;
    while ((ch) != '\0') {
        if ((ch >= '0') && (ch <= '9')) {
            UINT32 new = (UINT32)*tp * 10U;
            UINT8 c = (UINT8)ch - (UINT8)'0';
            new = new + (UINT32)c;

            if (new > 255U) {
                Ret = 1;
            }
            *tp = (UINT8)new;
            if (saw_digit == 0) {
                if (++octets > 4) {
                    Ret = 1;
                }
                saw_digit = 1;
            }
        } else if ((ch == '.') && (saw_digit != 0)) {
            if (octets == 4) {
                Ret = 1;
            }
            tp++;
            *tp = 0U;
            saw_digit = 0;
        } else {
            Ret = 1;
            break;
        }
        src++;
        ch = *src;
    }
    if (octets < 4) {
        Ret = 1;
    }

    AmbaMisra_TypeCast32(addr, tmp);
    return Ret;
}

static void my_lwiperf_report_fn(void *arg,
                                 enum lwiperf_report_type report_type,
                                 const ip_addr_t* local_addr,
                                 u16_t local_port,
                                 const ip_addr_t* remote_addr,
                                 u16_t remote_port,
                                 u32_t bytes_transferred,
                                 u32_t ms_duration,
                                 u32_t bandwidth_kbitpsec)
{
    (void)arg;
    (void)report_type;
    (void)local_addr;
    (void)local_port;
    (void)remote_addr;
    (void)remote_port;

    AmbaPrint_PrintUInt5("Tx: %d (B), t %d (ms), B: %d(kbs)",
                         bytes_transferred,
                         ms_duration,
                         bandwidth_kbitpsec, 0, 0);
}

static void LwIPInit(UINT32 Idx)
{
    static int lwip_inited = 0;

    if ((lwip_inited == 0) && (Idx <= 0u)) {
        memset(&netif_eth0, 0, sizeof(netif_eth0));

        tcpip_init(NULL, NULL);
        netif_add(&netif_eth0, NULL, NULL, NULL, NULL, ethernetif_init, netif_input);
        netif_set_default(&netif_eth0);
        netif_set_up(&netif_eth0);
        netif_set_link_up(&netif_eth0);

        lwip_inited = 1;
    }
}

static void *hdl_iperf = NULL;

static void iperf_client(void)
{
    ip_addr_t addr;

    IP4_ADDR(&addr, 192,168,1,100);
    hdl_iperf = lwiperf_start_tcp_client_default(&addr, my_lwiperf_report_fn, NULL);
}

static void *hdl_iperf_server = NULL;

static void iperf_server(void)
{
    if (hdl_iperf_server == NULL) {
        hdl_iperf_server = lwiperf_start_tcp_server_default(my_lwiperf_report_fn, NULL);
    }
}

static UINT32 LwIPBSDExample_TxUdp(UINT32 DstIp, UINT32 TxPause)
{
    const UINT16 UdpTxLen = (UINT16)NXUDPLEN;
    INT Ret;
    UINT32 cnt = 16U, i, j;
    INT sd;
    struct sockaddr_in SockIn;
    const struct sockaddr_in *pSockIn = &SockIn;
    struct sockaddr *pSock;
    CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";

    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    (void)AmbaWrap_memset(&SockIn, 0, sizeof(SockIn));
    AmbaMisra_TypeCast32(&pSock, &pSockIn);
    SockIn.sin_addr.s_addr = DstIp;
    SockIn.sin_port = htons(5001U);
    pSock->sa_family = PF_INET;

    for (i = 0U; i < (1024U*8U*cnt); i++) {
        Ret = sendto(sd, buffer, (INT)NXUDPLEN, 0, pSock, (INT)sizeof(struct sockaddr));
        if (Ret != (INT32)UdpTxLen) {
            AmbaPrint_PrintUInt5("sendto() returned %d", (UINT32)Ret, 0U, 0U, 0U, 0U);
        }
        /* Relinquish to other task */
        for (j = 0U; j < TxPause; j++) {
            (void) AmbaKAL_TaskYield();
        }
    }
    Ret = close(sd);
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "close() failed");
    }

    return (UINT32)Ret;
}

static UINT32 LwIPBSDExample_RxUdp(void)
{
    const UINT16 UdpTxLen = (UINT16)NXUDPLEN;
    const UINT32 RXSIZE = (NXUDPLEN*1024U*8U);
    INT32 data_len;
    static UINT32 datagrams = 0U, total = 0U;
    UINT32 cnt = 16U, i = 0U;
    UINT32 bytes = 0, tv1 = 0, tv2 = 0;
    INT Ret;
    INT sd;
    struct sockaddr_in sockAddr;
    const struct sockaddr_in *pSockIn = &sockAddr;
    struct sockaddr *pSock;
    struct sockaddr_in fromAddr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    CHAR buffer[ETH_FRAMES_SIZE];

    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    (void)AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(5001U);
    sockAddr.sin_family = PF_INET;
    AmbaMisra_TypeCast32(&pSock, &pSockIn);

    Ret = bind(sd, pSock, (INT)sizeof(sockAddr));
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "bind() failed");
    }

    while (i < cnt) {
        pSockIn = &fromAddr;
        AmbaMisra_TypeCast32(&pSock, &pSockIn);
        data_len = recvfrom(sd, (void *)buffer, (INT)UdpTxLen, 0, pSock, &addrlen);
        if (data_len <= 0) {
            break;
        }

        if (data_len != (INT32)UdpTxLen) {
            AmbaPrint_PrintUInt5( "recvfrom() returned %d", (UINT32)data_len, 0U, 0U, 0U, 0U);
        }

        if (bytes == 0U) {
            (void)AmbaKAL_GetSysTickCount(&tv1);
        }

        bytes += (UINT32)data_len;
        if (data_len == (INT32)NXUDPLEN) {
            total += (UINT32)data_len;
            datagrams++;
        }

        if(bytes >= RXSIZE) {
            UINT32 msec, kbits;
            UINT32 kbps;

            (void)AmbaKAL_GetSysTickCount(&tv2);
            // 8/1024 = 1/128
            kbits = bytes >> 7;
            msec = tv2 - tv1;
            kbits = kbits*1000U;
            kbps = kbits/msec;
            bytes = 0;
            AmbaPrint_PrintUInt5( "[%d] %d %d/%d*1000= %d kbps", datagrams, total, kbits, msec, kbps);
            i++;
        }
    }

    Ret = close(sd);
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "close() failed");
    }

    return (UINT32)Ret;
}

static UINT32 LwIPBSDExample_TxTcp(UINT32 DstIp, UINT32 TxPause)
{
    const INT TcpTxLen = (INT)NXTCPLEN;
    INT Ret;
    INT sd;
    struct sockaddr_in sockAddr;
    const struct sockaddr_in *pSockIn = &sockAddr;
    struct sockaddr *pSock;
    CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    UINT32 i, j, cnt = 16U;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    (void)AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_addr.s_addr = DstIp;
    sockAddr.sin_port = htons(5001U);
    sockAddr.sin_family = AF_INET;
    AmbaMisra_TypeCast32(&pSock, &pSockIn);

    Ret = connect(sd, pSock, (INT)sizeof(sockAddr));
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "connect() failed");
    }

    for (i = 0U; i < (1024U*8U*cnt); i++) {
        Ret = send(sd, buffer, TcpTxLen, 0);
        if (Ret < 0) {
            AmbaPrint_PrintUInt5("send() returned %d", (UINT32)Ret, 0U, 0U, 0U, 0U);
            break;
        }
        if (Ret != TcpTxLen) {
            AmbaPrint_PrintUInt5("send() returned %d", (UINT32)Ret, 0U, 0U, 0U, 0U);
        }
        /* Relinquish to other task */
        for (j = 0U; j < TxPause; j++) {
            (void) AmbaKAL_TaskYield();
        }
    }
    Ret = close(sd);
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "close() failed");
    }

    return (UINT32)Ret;
}

static UINT32 LwIPBSDExample_RxTcp(void)
{
    fd_set readfd, testfd;
    CHAR buffer[ETH_FRAMES_SIZE] = "";
    INT maxfd, j;
    INT Ret,  csd, sd, data_len;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in serverAddr;
    const struct sockaddr_in *pSockIn = &serverAddr;
    struct sockaddr *pSock;
    struct sockaddr_in ClientAddr;
    const UINT32 RXSIZE = (NXTCPLEN*1024U*8U);
    const INT RxLen = (INT)NXTCPLEN;
    static UINT32 datagrams = 0U, total = 0U;
    UINT32 cnt = 16U, i = 0U, served = 0U;
    UINT32 bytes = 0, tv1 = 0, tv2 = 0;
    struct timeval tv;

    tv.tv_sec = 300;
    tv.tv_usec = 0;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    (void)AmbaWrap_memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5001U);
    AmbaMisra_TypeCast32(&pSock, &pSockIn);

    Ret = bind(sd, pSock, (INT)sizeof(serverAddr));
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "bind() failed");
    }

    FD_ZERO(&readfd);
    FD_ZERO(&testfd);
    FD_SET(sd,&readfd);
    maxfd = sd;

    Ret = listen(sd, 5);
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "listen() failed");
    }

    /* only serve once */
    while (served <= 1U) {
        if (i >= cnt) {
            break;
        }

        testfd = readfd;

        (void)tv;
        if (select(maxfd + 1, &testfd, NULL, NULL, NULL) <= 0) {
            /* select timeout */
        }

        if(FD_ISSET(sd, &testfd) != 0) {
            pSockIn = &ClientAddr;
            AmbaMisra_TypeCast32(&pSock, &pSockIn);
            csd = accept(sd, pSock, &addrlen);
            FD_SET(csd, &readfd);
            if ( csd > maxfd) {
                maxfd = csd;
            }
            served++;
            continue;
        }
        for (j = LWIP_SOCKET_OFFSET; j <= (maxfd + LWIP_SOCKET_OFFSET); j++)
        {
            INT m = FD_ISSET(j, &readfd);
            INT n = FD_ISSET(j, &testfd);

            if (((j) != sd) &&
                 (m != 0) &&
                 (n != 0))
            {
                const CHAR *const pstr = buffer;
                VOID *ptr = NULL;
                AmbaMisra_TypeCast32(&ptr, &pstr);
                data_len = recv(j, ptr, RxLen, 0);
                if (data_len <= 0) {
                    FD_CLR(j, &readfd);
                    AmbaPrint_PrintUInt5("FD_CLR(%d)", (UINT32)j, 0U, 0U, 0U, 0U);
                    Ret = close(j);
                    AmbaPrint_PrintUInt5("close(%d) returned %d", (UINT32)j, (UINT32)Ret, 0U, 0U, 0U);
                    served++;
                    break;
                }

                if (bytes == 0U) {
                    (void)AmbaKAL_GetSysTickCount(&tv1);
                }

                bytes += (UINT32)data_len;
                if (data_len == RxLen) {
                    total += (UINT32)data_len;
                    datagrams++;
                }

                if(bytes >= RXSIZE) {
                    UINT32 msec, kbits;
                    UINT32 kbps;

                    (void)AmbaKAL_GetSysTickCount(&tv2);
                    // 8/1024 = 1/128
                    kbits = bytes >> 7;
                    msec = tv2 - tv1;
                    kbits = kbits*1000U;
                    kbps = kbits/msec;
                    bytes = 0;
                    AmbaPrint_PrintUInt5( "[%d] %d %d/%d*1000= %d kbps", datagrams, total, kbits, msec, kbps);
                    i++;
                }
            }
        }
    }
    for (j = LWIP_SOCKET_OFFSET; j <= (maxfd + LWIP_SOCKET_OFFSET); j++) {
        if (FD_ISSET(j, &readfd) != 0) {
            Ret = close(j);
            AmbaPrint_PrintUInt5("close(%d) returned %d", (UINT32)j, (UINT32)Ret, 0U, 0U, 0U);
        }
    }

    return (UINT32)Ret;
}

static void LaunchTest(UINT32 TestItem)
{
    switch (TestItem) {
    case RXUDP_BSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_RxUdp()");
        (void) LwIPBSDExample_RxUdp();
        break;
    case RXTCP_BSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_RxTcp()");
        (void) LwIPBSDExample_RxTcp();
        break;
    case TXUDP_BSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_TxUdp()");
        (void) LwIPBSDExample_TxUdp(IPARG, Pause);
        break;
    case TXTCP_BSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_TxTcp()");
        (void) LwIPBSDExample_TxTcp(IPARG, Pause);
        break;
    default:
        /* TBD */
        break;
    }

    AmbaPrint_PrintUInt5("TestItem %d end", TestItem, 0U, 0U, 0U, 0U);
}

static void ForkTest(UINT32 Idx, UINT32 TestItem)
{
#define TestStackSize 8192U
    static UINT8 Stack[2][8192] __attribute__((section(".bss.noinit")));
    static char taskname[2][32] = { "RxTask", "TxTask" };
    UINT32 err;
    static AMBA_KAL_TASK_t TestTask[2] = {
        [0] = {
            .tx_thread_id = 0U,
        },
        [1] = {
            .tx_thread_id = 0U,
        },
    };

    if (TestTask[Idx].tx_thread_id != 0U) {
        if (AmbaKAL_TaskDelete(&TestTask[Idx]) != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "AmbaKAL_TaskDelete failed");
        }
    }

    if (TestTask[Idx].tx_thread_id == 0U) {
        const UINT8 *pBuf;
        void *ptr;

        (void)AmbaWrap_memset(&Stack[Idx][0], 0, TestStackSize);
        pBuf = &Stack[Idx][0];
        AmbaMisra_TypeCast32(&ptr, &pBuf);

        AmbaPrint_PrintUInt5("TestItem %d Pri %d", TestItem, TestPri[Idx], 0U, 0U, 0U);
        err = AmbaKAL_TaskCreate(&TestTask[Idx],
                                 taskname[Idx],
                                 TestPri[Idx],
                                 LaunchTest,
                                 TestItem,
                                 ptr,
                                 TestStackSize,
                                 0U);
        if (err != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "AmbaKAL_TaskCreate failed");
        }
        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&TestTask[Idx], 1U);
        if (err != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "AmbaKAL_TaskSetSmpAffinity failed");
        }
        err = AmbaKAL_TaskResume(&TestTask[Idx]);
        if (err != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "AmbaKAL_TaskResume failed");
        }
    }
}


void AmbaShell_CommandLwip(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (argc == 3U) {
        //lwip ifconfig 192.168.1.100
        if (0 == AmbaUtility_StringCompare(argv[1], "ifconfig", AmbaUtility_StringLength("ifconfig"))) {
            ip_addr_t netmask;
            ip4_addr_t ip4;
            UINT32 IpAddr;

            (void) Str2NetOdrIP(argv[2], &IpAddr);
            ip4.addr = IpAddr;
            IP4_ADDR(&netmask, 255,255,255,0);

            netif_set_addr(&netif_eth0, &ip4, &netmask, NULL);
        }
        //lwip bsd_udp_tx 192.168.1.2
        else if (0 == AmbaUtility_StringCompare(argv[1], "bsd_udp_tx", AmbaUtility_StringLength("bsd_udp_tx"))) {
            (void) Str2NetOdrIP(argv[2], &IPARG);
            ForkTest(TXTEST, TXUDP_BSD);
        }
        //lwip bsd_tcp_tx 192.168.1.2
        else if (0 == AmbaUtility_StringCompare(argv[1], "bsd_tcp_tx", AmbaUtility_StringLength("bsd_tcp_tx"))) {
            (void) Str2NetOdrIP(argv[2], &IPARG);
            ForkTest(TXTEST, TXTCP_BSD);
        }
        //lwip txpause 624
        else if (0 == AmbaUtility_StringCompare(argv[1], "txpause", AmbaUtility_StringLength("txpause"))) {
            (void) AmbaUtility_StringToUInt32(argv[2], &Pause);
        }
        else {
            LwIPUsage(PrintFunc);
        }
    }
    else if (argc == 2U) {
        //lwip init
        if (0 == AmbaUtility_StringCompare(argv[1], "init", AmbaUtility_StringLength("init"))) {
            LwIPInit(0U);
        }
        //lwip bsd_udp_rx
        else if (0 == AmbaUtility_StringCompare(argv[1], "bsd_udp_rx", AmbaUtility_StringLength("bsd_udp_rx"))) {
            ForkTest(RXTEST, RXUDP_BSD);
        }
        //lwip bsd_tcp_rx
        else if (0 == AmbaUtility_StringCompare(argv[1], "bsd_tcp_rx", AmbaUtility_StringLength("bsd_tcp_rx"))) {
            ForkTest(RXTEST, RXTCP_BSD);
        }
        else if (0 == AmbaUtility_StringCompare(argv[1], "iperfc", AmbaUtility_StringLength("iperfc"))) {
            iperf_client();
        }
        else if (0 == AmbaUtility_StringCompare(argv[1], "iperfs", AmbaUtility_StringLength("iperfs"))) {
            iperf_server();
        }
        else {
            LwIPUsage(PrintFunc);
        }
    }
    else {
        LwIPUsage(PrintFunc);
    }
}

AMBA_SHELL_COMMAND_s LwipCommands = {
    .pName = "lwip",
    .MainFunc = AmbaShell_CommandLwip,
    .pNext = NULL
};

void AmbaShell_AddLwip(void)
{
    AmbaShell_CommandRegister(&LwipCommands);
}


#ifndef CONFIG_CC_USESTD
#if MYLWIPDBG
// used by malloc()
void * _sbrk (int incr)
{
    extern char   __sbrk_end; /* Set by linker.  */
    static char * heap_end;
    char *        prev_heap_end;

    if (heap_end == 0)
        heap_end = & __sbrk_end;

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *) prev_heap_end;
}
#endif // MYLWIPDBG
#endif // !CONFIG_CC_USESTD

