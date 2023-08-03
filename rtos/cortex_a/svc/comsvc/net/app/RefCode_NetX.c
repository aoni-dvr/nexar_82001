/**
 *  @file RefCode_NetX.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Enet netx refcode
 *
 */

#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaShell_Enet.h"
#include "RefCode_NetX.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "NetXStack.h"
#include "nx_bsd.h"

#ifdef CONFIG_NETX_TELNET
#include "nx_telnet_server.h"
#endif /* CONFIG_NETX_TELNET */

#define close soc_close
#define SOCKFD_START NX_BSD_SOCKFD_START
#ifdef CONFIG_NETX_TELNET
#define NX_TELNET_CTRLD      0x4U
#define NX_TELNET_TXTIMEOUT  2000U
#define NX_TELNET_MEM_SIZE   (10U * 1024U)
static NX_TELNET_SERVER  NxTelnetServ;
UINT32 NxTelnetServCurIdx = NX_TELNET_INVALID;
UINT8 NxTelnetRxBuf[NX_TELNET_BUF_SIZE];
UINT32 NxTelnetCurUsrInput = 0U;

static void TelnetServNewConn(NX_TELNET_SERVER *pNxTelnetServ, UINT ClientIdx)
{
    UINT ret;
    NX_PACKET *pkt;
    NETX_STACK_CONFIG_s *pNetXConfig;
    char welcome[] =
        "*****************************************************************\r\n"
        "*                                                               *\r\n"
        "*                        AmbaShell ;)                           *\r\n"
        "*                                                               *\r\n"
        "*****************************************************************\r\n"
        "\r\n"
        "\x1b"
        "[4h"
        "Type 'help' for help\r\n\r\n"
        "a:\\> "
        ;

    (void) NetXStack_GetConfig(0U, &pNetXConfig);
    (void) nx_packet_allocate(pNetXConfig->pNxPktPool, &pkt, NX_TCP_PACKET, TX_WAIT_FOREVER);
    (void) nx_packet_data_append(pkt, welcome, sizeof(welcome), pNetXConfig->pNxPktPool, TX_WAIT_FOREVER);
    ret =  nx_telnet_server_packet_send(pNxTelnetServ, ClientIdx, pkt, NX_TELNET_TXTIMEOUT);
    if (ret != 0U) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "nx_telnet_server_packet_send() failed");
        (void) nx_packet_release(pkt);
    }

    NxTelnetServCurIdx = ClientIdx;
    return;
}

static void TelnetServRx(NX_TELNET_SERVER *pNxTelnetServ, UINT ClientIdx, NX_PACKET *pkt)
{
    UCHAR chr;

    /* logout by Ctrl + D */
    chr =  pkt->nx_packet_prepend_ptr[0];
    if (chr == NX_TELNET_CTRLD) {
        (void) nx_telnet_server_disconnect(pNxTelnetServ, ClientIdx);
    }

    /* handle carriage return */
    if((pkt->nx_packet_length == 2U) &&
       (pkt->nx_packet_prepend_ptr[0] == (UCHAR)'\r') &&
       (pkt->nx_packet_prepend_ptr[1] == (UCHAR)'\0')) {
        pkt->nx_packet_length =  1U;
    }

    if (AmbaWrap_memcpy(&NxTelnetRxBuf[NxTelnetCurUsrInput], pkt->nx_packet_prepend_ptr, pkt->nx_packet_length)!= 0U) { }

    (void) nx_packet_release(pkt);
    NxTelnetCurUsrInput += pkt->nx_packet_length;
    NxTelnetCurUsrInput %= NX_TELNET_BUF_SIZE;

}

static void TelnetServEndConn(NX_TELNET_SERVER *pNxTelnetServ, UINT ClientIdx)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(pNxTelnetServ);
#else
    (void) pNxTelnetServ;
#endif
    if (NxTelnetServCurIdx == ClientIdx) {
        NxTelnetServCurIdx = NX_TELNET_INVALID;
    }
    return;
}

void TelnetServInit(UINT32 Idx)
{
    UINT ret;
    static UINT8 nx_telnet_mem[NX_TELNET_MEM_SIZE];
    CHAR sockStr[] = "Telnet Server";
    NETX_STACK_CONFIG_s *pNetXConfig;

    (void) NetXStack_GetConfig(Idx, &pNetXConfig);
    ret = nx_telnet_server_create(&NxTelnetServ, sockStr, pNetXConfig->pNxIp,
                    &nx_telnet_mem[0], NX_TELNET_MEM_SIZE, TelnetServNewConn, TelnetServRx,
                    TelnetServEndConn);

    if (ret != 0U) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "nx_telnet_server_create() failed");
    }

    if (nx_telnet_server_start(&NxTelnetServ) != 0U) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "nx_telnet_server_start() failed");
    }
}

UINT32 AmbaTelnet_Read(UINT8 *pNxTelnetRxBuf, UINT32 RxSize, UINT32 *pActualRxSize)
{
    UINT32 Ret = 0U, Pending = 0U;
    static UINT32 CurShInput = 0U;

    if (pActualRxSize == NULL) {
        Ret = 1U;
    } else {
       /* code start */
        if (NxTelnetCurUsrInput < CurShInput) {
            Pending = (NxTelnetCurUsrInput + NX_TELNET_BUF_SIZE) - CurShInput;
        } else if (NxTelnetCurUsrInput >= CurShInput) {
            Pending = NxTelnetCurUsrInput - CurShInput;
        } else {
            /*not possible*/
        }

        if (RxSize < Pending) {
            *pActualRxSize = RxSize;
        } else if (RxSize >= Pending) {
            *pActualRxSize = Pending;
        } else {
            /*not possible*/
        }

        if ( 0U != *pActualRxSize) {
            Ret = AmbaWrap_memcpy(pNxTelnetRxBuf, &NxTelnetRxBuf[CurShInput], *pActualRxSize);
            if (Ret != 0U) {}
            CurShInput += *pActualRxSize;
            CurShInput %= NX_TELNET_BUF_SIZE;
        }
    }

    return Ret;
}

UINT32 AmbaTelnet_Write(const UINT8 *pTxBuf, UINT32 TxSize)
{
    UINT32 Ret;
    NX_TELNET_SERVER *pNxTelnetServ = &NxTelnetServ;
    NETX_STACK_CONFIG_s *pNetXConfig;
    if (NxTelnetServCurIdx != NX_TELNET_INVALID) {
        UINT ret;
        NX_PACKET *pkt;
        void *ptr;

        AmbaMisra_TypeCast(&ptr, &pTxBuf);
        (void) NetXStack_GetConfig(0U, &pNetXConfig);
        (void) nx_packet_allocate(pNetXConfig->pNxPktPool, &pkt, NX_TCP_PACKET, TX_WAIT_FOREVER);
        (void) nx_packet_data_append(pkt, ptr, TxSize, pNetXConfig->pNxPktPool, TX_WAIT_FOREVER);
        ret =  nx_telnet_server_packet_send(pNxTelnetServ, NxTelnetServCurIdx, pkt, NX_TELNET_TXTIMEOUT);
        if (ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_telnet_server_packet_send() failed");
            (void) nx_packet_release(pkt);
        }

        Ret = 0U;
    } else {
        Ret = 1U;
    }

    return Ret;
}
#endif  /* CONFIG_NETX_TELNET */

void NetXInit(UINT32 Idx)
{
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) NetXStack_GetConfig(Idx, &pNetXConfig);
    if (pNetXConfig != NULL) {
        pNetXConfig->Idx = Idx;
        if (pEnetConfig != NULL) {
            if (AmbaWrap_memcpy(pNetXConfig->Mac, pEnetConfig->Mac, 6)!= 0U) { }
        }
        (void)NetXStack_EnetInit(pNetXConfig);
    }
}

void NetXDump(UINT32 Idx)
{
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    UINT Ret;
    UINT32 free_packets = 0U;
    UINT32 total_packets = 0U;
    UINT32 empty_pool_requests = 0U;
    UINT32 empty_pool_suspensions = 0U;
    UINT32 invalid_packet_releases = 0U;

    (void) NetXStack_GetConfig(Idx, &pNetXConfig);
    if (pNetXConfig != NULL) {
        Ret = nx_packet_pool_info_get(pNetXConfig->pNxPktPool,
                                        &total_packets, &free_packets, &empty_pool_requests,
                                        &empty_pool_suspensions, &invalid_packet_releases);
        if (Ret == 0U) {
            AmbaPrint_PrintUInt5("free_packets %lu", free_packets, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("total_packets %lu", total_packets, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("empty_pool_requests %lu", empty_pool_requests, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("empty_pool_suspensions %lu", empty_pool_suspensions, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("invalid_packet_releases %lu", invalid_packet_releases, 0U, 0U, 0U, 0U);
        }
    }
}

#define EXUDPLEN  ((UINT16)(ETH_FRAMES_SIZE - 44U))
#define EXTCPLEN  ((UINT16)(ETH_FRAMES_SIZE - 54U))

UINT32 NetXBSDExample_TxUdp(UINT32 DstIp, UINT32 TxPause)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    const UINT16 UdpTxLen = (UINT16)EXUDPLEN;
    INT Ret;
    UINT32 cnt = 16U, i, j;
    INT sd;
    struct sockaddr_in SockIn;
    const struct sockaddr_in *pSockIn = &SockIn;
    struct sockaddr *pSock;
    CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "socket() failed");
        Ret = 1;
    } else {
        if (AmbaWrap_memset(&SockIn, 0, sizeof(SockIn))!= 0U) { }
        AmbaMisra_TypeCast(&pSock, &pSockIn);
        SockIn.sin_addr.s_addr =  DstIp;
        SockIn.sin_port = 5001U;
        pSock->sa_family = PF_INET;
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TouchUnused(&SockIn.sin_addr.s_addr);
        AmbaMisra_TouchUnused(&SockIn.sin_port);
#endif

        for (i = 0U; i < (1024U*8U*cnt); i++) {
            Ret = sendto(sd, buffer, (INT)EXUDPLEN, 0, pSock, (INT)sizeof(struct sockaddr));
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
    }

    return (UINT32)Ret;
}

UINT32 NetXBSDExample_RxUdp(void)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    const UINT16 UdpTxLen = (UINT16)EXUDPLEN;
    const UINT32 RXSIZE = (EXUDPLEN*1024U*8U);
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
    INT addrlen = (INT)sizeof(struct sockaddr_in);
    CHAR buffer[ETH_FRAMES_SIZE];

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "socket() failed");
        Ret = 1;
    } else {
        if (AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr))!= 0U) { }
        sockAddr.sin_addr.s_addr = INADDR_ANY;
        sockAddr.sin_port = 5001U;
        sockAddr.sin_family = PF_INET;
        AmbaMisra_TypeCast(&pSock, &pSockIn);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TouchUnused(&sockAddr.sin_addr.s_addr);
        AmbaMisra_TouchUnused(&sockAddr.sin_port);
        AmbaMisra_TouchUnused(&sockAddr.sin_family);
#endif

        Ret = bind(sd, pSock, (INT)sizeof(sockAddr));
        if (Ret != 0) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "bind() failed");
        }

        while (i < cnt) {
            pSockIn = &fromAddr;
            AmbaMisra_TypeCast(&pSock, &pSockIn);
            data_len = recvfrom(sd, buffer, (INT)UdpTxLen, 0, pSock, &addrlen);
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
            if (data_len == (INT32)EXUDPLEN) {
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
    }

    return (UINT32)Ret;
}

UINT32 NetXBSDExample_TxTcp(UINT32 DstIp, UINT32 TxPause)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    const INT TcpTxLen = (INT)EXTCPLEN;
    INT Ret;
    INT sd;
    struct sockaddr_in sockAddr;
    const struct sockaddr_in *pSockIn = &sockAddr;
    struct sockaddr *pSock;
    const CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    UINT32 i, j, cnt = 16U;

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "socket() failed");
        Ret = 1;
    } else {
        if (AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr))!= 0U) { }
        sockAddr.sin_addr.s_addr = DstIp;
        sockAddr.sin_port = 5001U;
        sockAddr.sin_family = AF_INET;
        AmbaMisra_TypeCast(&pSock, &pSockIn);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TouchUnused(&sockAddr.sin_addr.s_addr);
        AmbaMisra_TouchUnused(&sockAddr.sin_port);
        AmbaMisra_TouchUnused(&sockAddr.sin_family);
#endif

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
    }

    return (UINT32)Ret;
}

UINT32 NetXBSDExample_RxTcp(void)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    fd_set readfd, testfd;
    CHAR buffer[ETH_FRAMES_SIZE] = "";
    INT maxfd, j;
    INT Ret,  csd, sd, data_len;
    INT addrlen = (INT)sizeof(struct sockaddr_in);
    struct sockaddr_in serverAddr;
    const struct sockaddr_in *pSockIn = &serverAddr;
    struct sockaddr *pSock;
    struct sockaddr_in ClientAddr;
    const UINT32 RXSIZE = (EXTCPLEN*1024U*8U);
    const INT RxLen = (INT)EXTCPLEN;
    static UINT32 datagrams = 0U, total = 0U;
    UINT32 cnt = 16U, i = 0U, served = 0U;
    UINT32 bytes = 0, tv1 = 0, tv2 = 0;
    struct timeval tv;

    tv.tv_sec = 300;
    tv.tv_usec = 0;

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "socket() failed");
        Ret = 1;
    } else {
        if (AmbaWrap_memset(&serverAddr, 0, sizeof(serverAddr))!= 0U) { }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = 5001U;
        AmbaMisra_TypeCast(&pSock, &pSockIn);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TouchUnused(&serverAddr.sin_family);
        AmbaMisra_TouchUnused(&serverAddr.sin_addr.s_addr);
        AmbaMisra_TouchUnused(&serverAddr.sin_port);
#endif

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
                AmbaMisra_TypeCast(&pSock, &pSockIn);
                csd = accept(sd, pSock, &addrlen);
                FD_SET(csd, &readfd);
                if ( csd > maxfd) {
                    maxfd = csd;
                }
                served++;
                continue;
            }
            for (j = SOCKFD_START; j <= (maxfd + SOCKFD_START); j++)
            {
                INT m = FD_ISSET(j, &readfd);
                INT n = FD_ISSET(j, &testfd);

                if (((j) != sd) &&
                     (m != 0) &&
                     (n != 0))
                {
                    const CHAR *const pstr = buffer;
                    VOID *ptr = NULL;
                    AmbaMisra_TypeCast(&ptr, &pstr);
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
        for (j = SOCKFD_START; j <= (maxfd + SOCKFD_START); j++) {
            if (FD_ISSET(j, &readfd) != 0) {
                Ret = close(j);
                AmbaPrint_PrintUInt5("close(%d) returned %d", (UINT32)j, (UINT32)Ret, 0U, 0U, 0U);
            }
        }
    }

    return (UINT32)Ret;
}

UINT32 NetXExample_TxUdp(UINT32 Idx, UINT32 DstIp, UINT32 TxPause)
{
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;
    NX_UDP_SOCKET nx_udp_sock = {0};
    NX_PACKET *udp_packet = NULL;
    UINT Ret = 1U;
    const UINT16 UdpTxLen = (UINT16)EXUDPLEN;
    const UINT32 UdpRxQue = 8U;
    CHAR sockStr[] = "UDP Socket";
    UINT32 cnt = 16U, i, j;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) NetXStack_GetConfig(Idx, &pNetXConfig);

    if ((pEnetConfig != NULL) && (pNetXConfig != NULL)) {
        if ((pEnetConfig->LinkSpeed > 100U)) {
            cnt = 64U;
        }
        Ret = nx_udp_socket_create(pNetXConfig->pNxIp, &nx_udp_sock, sockStr, NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, UdpRxQue);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_create() failed");
        }

        Ret = nx_udp_socket_bind(&nx_udp_sock, NX_ANY_PORT, 2000U);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_bind() failed");
        }

        for (i = 0U; i < (1024U*8U*cnt); i++) {
            const CHAR buffer[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
            Ret = nx_packet_allocate(pNetXConfig->pNxPktPool, &udp_packet, NX_UDP_PACKET, TX_WAIT_FOREVER);
            if (Ret != 0U) {
                EnetPrint(__func__, __LINE__, ANSI_RED, "nx_packet_allocate() failed");
            }

            if (udp_packet != NULL) {
                if (AmbaWrap_memcpy(udp_packet->nx_packet_prepend_ptr, buffer, sizeof(buffer))!= 0U) { }
                udp_packet->nx_packet_length = UdpTxLen;
                udp_packet->nx_packet_append_ptr = &udp_packet->nx_packet_prepend_ptr[UdpTxLen];

                /* non-blocking call, nx_packet_transmit_release will be done after tx finish */
                Ret =  nx_udp_socket_send(&nx_udp_sock, udp_packet, DstIp, 5001U);
                if (Ret != 0U) {
                    EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_send() failed");
                }
                /* Relinquish to other task */
                for (j = 0U; j < TxPause; j++) {
                    (void) AmbaKAL_TaskYield();
                }
            }
        }

        Ret = nx_udp_socket_unbind(&nx_udp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_unbind() failed");
        }

        Ret = nx_udp_socket_delete(&nx_udp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_delete() failed");
        }
    }

    return Ret;
}

UINT32 NetXExample_RxUdp(UINT32 Idx)
{
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;
    NX_UDP_SOCKET nx_udp_sock = {0};
    NX_PACKET *udp_packet = NULL;
    CHAR sockStr[] = "UDP Socket";
    UINT Ret;
    const UINT32 RXSIZE = (EXUDPLEN*1024U*8U);
    const UINT32 UdpRxQue = 64U;
    UINT32 data_len;
    static UINT32 datagrams = 0U, total = 0U;
    UINT32 cnt = 16U, i =0U;
    UINT32 bytes = 0, tv1 = 0, tv2 = 0;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) NetXStack_GetConfig(Idx, &pNetXConfig);

    if ((pNetXConfig != NULL) && (pEnetConfig != NULL)) {
        if ((pEnetConfig->LinkSpeed > 100U)) {
            cnt = 64U;
        }
        Ret = nx_udp_socket_create(pNetXConfig->pNxIp, &nx_udp_sock, sockStr, NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, UdpRxQue);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_create() failed");
        }

        Ret = nx_udp_socket_bind(&nx_udp_sock, 5001U, 2000U);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_bind() failed");
        }

        while (i < cnt) {
            Ret = nx_udp_socket_receive(&nx_udp_sock, &udp_packet, TX_WAIT_FOREVER);
            if (Ret != 0U) {
                EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_receive() failed");
            }

            if (udp_packet != NULL) {
                data_len = udp_packet->nx_packet_length;

                if (bytes == 0U) {
                    (void)AmbaKAL_GetSysTickCount(&tv1);
                }

                bytes += data_len;
                if (data_len == EXUDPLEN) {
                    total += data_len;
                    datagrams++;
                }

                Ret =  nx_packet_release(udp_packet);
                if (Ret != 0U) {
                    EnetPrint(__func__, __LINE__, ANSI_RED, "nx_packet_release() failed");
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

        Ret = nx_udp_socket_unbind(&nx_udp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_unbind() failed");
        }

        Ret = nx_udp_socket_delete(&nx_udp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_udp_socket_delete() failed");
        }
    }

    return 0;
}

UINT32 NetXExample_TxTcp(UINT32 Idx, UINT32 DstIp, UINT32 TxPause)
{
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;
    NX_TCP_SOCKET nx_tcp_sock = {0};
    NX_PACKET *tcp_packet = NULL;
    CHAR sockStr[] = "TCP Socket";
    UINT Ret = 1U;
    const UINT16 TcpTxLen = (UINT16)EXTCPLEN;
    const UINT32 TcpRxWin = 65535U;
    UINT32 cnt = 16U;
    UINT32 i, j;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) NetXStack_GetConfig(Idx, &pNetXConfig);

    if ((pNetXConfig != NULL) && (pEnetConfig != NULL)) {
        if ((pEnetConfig->LinkSpeed > 100U)) {
            cnt = 64U;
        }
        Ret = nx_tcp_socket_create(pNetXConfig->pNxIp, &nx_tcp_sock, sockStr,
                NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, TcpRxWin, NULL, NULL);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_create() failed");
        }

        Ret = nx_tcp_client_socket_bind(&nx_tcp_sock, NX_ANY_PORT, NX_NO_WAIT);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_client_socket_bind() failed");
        }

        Ret = nx_tcp_client_socket_connect(&nx_tcp_sock, DstIp, 5001U, TX_WAIT_FOREVER);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_client_socket_connect() failed");
        }

        for (i = 0U; i < (1024U*8U*cnt); i++) {
            const CHAR buffer[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
            Ret = nx_packet_allocate(pNetXConfig->pNxPktPool, &tcp_packet, NX_TCP_PACKET, TX_WAIT_FOREVER);
            if (Ret != 0U) {
                EnetPrint(__func__, __LINE__, ANSI_RED, "nx_packet_allocate() failed");
            }

            if (tcp_packet != NULL) {
                if (AmbaWrap_memcpy(tcp_packet->nx_packet_prepend_ptr, buffer, sizeof(buffer))!= 0U) { }
                tcp_packet->nx_packet_length = TcpTxLen;
                tcp_packet->nx_packet_append_ptr = &tcp_packet->nx_packet_prepend_ptr[TcpTxLen];

                /* nx_packet_transmit_release will be done after tx finish */
                Ret =  nx_tcp_socket_send(&nx_tcp_sock, tcp_packet, TX_WAIT_FOREVER);
                if (Ret != 0U) {
                    EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_send() failed");
                }
                /* Relinquish to other task */
                for (j = 0U; j < TxPause; j++) {
                    (void) AmbaKAL_TaskYield();
                }
                if (Ret != 0U) {
                    AmbaPrint_PrintUInt5("nx_tcp_socket_send() returned %d", Ret, 0U, 0U, 0U, 0U);
                    Ret =  nx_packet_release(tcp_packet);
                    if (Ret != 0U) {
                        EnetPrint(__func__, __LINE__, ANSI_RED, "nx_packet_release() failed");
                    }
                    break;
                }
            }
        }

        Ret = nx_tcp_socket_disconnect(&nx_tcp_sock, 100U);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_disconnect() failed");
        }

        Ret = nx_tcp_client_socket_unbind(&nx_tcp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_client_socket_unbind() failed");
        }

        Ret = nx_tcp_socket_delete(&nx_tcp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_delete() failed");
        }
    }

    return Ret;
}

UINT32 NetXExample_RxTcp(UINT32 Idx)
{
    NETX_STACK_CONFIG_s *pNetXConfig = NULL;
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;
    NX_TCP_SOCKET nx_tcp_sock = {0};
    NX_PACKET *tcp_packet = NULL;
    UINT Ret;
    CHAR sockStr[] = "TCP Socket";
    const UINT32 RXSIZE = (EXTCPLEN*1024U*8U);
    const UINT32 TcpRxWin = 65535U;
    UINT32 tcpdata_len;
    static UINT32 datagrams = 0U, total = 0U;
    UINT32 cnt = 16U, served = 0U, i=0U;
    UINT32 bytes = 0, tv1 = 0, tv2 = 0;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) NetXStack_GetConfig(Idx, &pNetXConfig);

    if ((pNetXConfig != NULL) && (pEnetConfig != NULL)) {
        if ((pEnetConfig->LinkSpeed > 100U)) {
            cnt = 64U;
        }
        Ret = nx_tcp_socket_create(pNetXConfig->pNxIp, &nx_tcp_sock, sockStr,
                NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, TcpRxWin, NULL, NULL);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_create() failed");
        }

        Ret = nx_tcp_server_socket_listen(pNetXConfig->pNxIp, 5001U, &nx_tcp_sock, 5U, NULL);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_server_socket_listen() failed");
        }

        /* only serve once */
        while (served < 1U) {
            /* Accept a client socket connection.  */
            Ret = nx_tcp_server_socket_accept(&nx_tcp_sock, TX_WAIT_FOREVER);
            if (Ret != 0U) {
                EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_server_socket_accept() failed");
            }

            while(i < cnt) {
                Ret = nx_tcp_socket_receive(&nx_tcp_sock, &tcp_packet, TX_WAIT_FOREVER);
                if (Ret != 0U) {
                    AmbaPrint_PrintUInt5("nx_tcp_socket_receive() returned %d", Ret, 0U, 0U, 0U, 0U);
                    break;
                }

                if (tcp_packet != NULL) {
                    tcpdata_len = tcp_packet->nx_packet_length;
                    Ret =  nx_packet_release(tcp_packet);
                    if (Ret != 0U) {
                        EnetPrint(__func__, __LINE__, ANSI_RED, "nx_packet_release() failed");
                    }

                    if (bytes == 0U) {
                        (void)AmbaKAL_GetSysTickCount(&tv1);
                    }

                    bytes += tcpdata_len;
                    if (tcpdata_len == EXTCPLEN) {
                        total += tcpdata_len;
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
            (void) nx_tcp_socket_disconnect(&nx_tcp_sock, 100U);

            Ret = nx_tcp_server_socket_unaccept(&nx_tcp_sock);
            if (Ret != 0U) {
                EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_server_socket_unaccept() failed");
            }

            Ret = nx_tcp_server_socket_relisten(pNetXConfig->pNxIp, 5001U, &nx_tcp_sock);
            AmbaPrint_PrintUInt5("nx_tcp_server_socket_relisten() returned %d", Ret, 0U, 0U, 0U, 0U);

            served++;
        }

        (void) nx_tcp_socket_disconnect(&nx_tcp_sock, 100U);

        Ret = nx_tcp_server_socket_unaccept(&nx_tcp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_server_socket_unaccept() failed");
        }

        Ret = nx_tcp_server_socket_unlisten(pNetXConfig->pNxIp, 5001U);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_server_socket_unlisten() failed");
        }

        Ret = nx_tcp_socket_delete(&nx_tcp_sock);
        if (Ret != 0U) {
            EnetPrint(__func__, __LINE__, ANSI_RED, "nx_tcp_socket_delete() failed");
        }
    }

    return 0;
}
