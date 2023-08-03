/**
 *  @file RefCode_LwIP.c
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
 *  @details Enet lwip refcode
 *
 */

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG

#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaShell_Enet.h"
#include "RefCode_LwIP.h"
#include "AmbaENET.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#define LWIP_PATCH_MISRA   // for misra depress
#include "lwip/sockets.h"

void LwIPInit(UINT32 Idx)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);

    if (pEnetConfig != NULL) {
        sys_lwip_enetinit(Idx);
    }
}

#define EXUDPLEN  (ETH_FRAMES_SIZE - 44U)
#define EXTCPLEN  (ETH_FRAMES_SIZE - 54U)

UINT32 LwIPBSDExample_TxUdp(UINT32 DstIp, UINT32 TxPause)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    const UINT16 UdpTxLen = (UINT16)EXUDPLEN;
    INT Ret = 0;
    UINT32 cnt = 16U, i, j;
/* TXFASTPATH: udp tx zero copy example */
//#define TXFASTPATH
#if defined(TXFASTPATH)
    extern void sys_lwip_pbuf_alloc(UINT32 header, UINT16 length, void **addr, void **priv);
    extern INT sys_lwip_udp_sendto(void *priv, UINT32 ip, UINT16 port);
    extern INT sys_lwip_pbuf_free(void *priv);
    void *ptr;  //UDP payload
    void *priv; //struct pbuf
#else
    INT sd;
    struct sockaddr_in SockIn;
    const struct sockaddr_in *pSockIn = &SockIn;
    struct sockaddr *pSock;
    CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
#endif

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }

#if !defined(TXFASTPATH)
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (AmbaWrap_memset(&SockIn, 0, sizeof(SockIn))!= 0U) { }
    AmbaMisra_TypeCast(&pSock, &pSockIn);
    SockIn.sin_addr.s_addr = DstIp;
    SockIn.sin_port = htons(5001U);
    pSock->sa_family = PF_INET;
#endif

    for (i = 0U; i < (1024U*8U*cnt); i++) {
#if defined(TXFASTPATH)
        /*udp header 14+20+8=42*/
        sys_lwip_pbuf_alloc(42U, UdpTxLen, &ptr, &priv);
        /*udp payload 4*/
        if (AmbaWrap_memcpy(ptr, "ABC", 4)!= 0U) { }
        Ret = sys_lwip_udp_sendto(priv, DstIp, 5001U);
        /* free the pbuf */
        (void)sys_lwip_pbuf_free(priv);

        /* Relinquish to other task */
        for (j = 0U; j < TxPause; j++) {
            (void) AmbaKAL_TaskYield();
        }
#else
        Ret = sendto(sd, buffer, (INT)EXUDPLEN, 0, pSock, (INT)sizeof(struct sockaddr));
        if (Ret != (INT32)UdpTxLen) {
            AmbaPrint_PrintUInt5("sendto() returned %d", (UINT32)Ret, 0U, 0U, 0U, 0U);
        }
        /* Relinquish to other task */
        for (j = 0U; j < TxPause; j++) {
            (void) AmbaKAL_TaskYield();
        }
#endif
    }
#if !defined(TXFASTPATH)
    Ret = close(sd);
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "close() failed");
    }
#endif

    return (UINT32)Ret;
}

UINT32 LwIPBSDExample_RxUdp(void)
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
    socklen_t addrlen = sizeof(struct sockaddr_in);
    CHAR buffer[ETH_FRAMES_SIZE];
    void *ptr;

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr))!= 0U) { }
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port = htons(5001U);
    sockAddr.sin_family = PF_INET;
    AmbaMisra_TypeCast(&pSock, &pSockIn);

    Ret = bind(sd, pSock, (INT)sizeof(sockAddr));
    if (Ret != 0) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "bind() failed");
    }

    while (i < cnt) {
        const CHAR *pBuf = &buffer[0];
        AmbaMisra_TypeCast(&ptr, &pBuf);
        pSockIn = &fromAddr;
        AmbaMisra_TypeCast(&pSock, &pSockIn);
        data_len = recvfrom(sd, ptr, (INT)UdpTxLen, 0, pSock, &addrlen);
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

    return (UINT32)Ret;
}

UINT32 LwIPBSDExample_TxTcp(UINT32 DstIp, UINT32 TxPause)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    const INT TcpTxLen = (INT)EXTCPLEN;
    INT Ret;
    INT sd;
    struct sockaddr_in sockAddr;
    const struct sockaddr_in *pSockIn = &sockAddr;
    struct sockaddr *pSock;
    CHAR buffer[ETH_FRAMES_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
    UINT32 i, j, cnt = 16U;

    (void) AmbaEnet_GetConfig(0U, &pEnetConfig);
    if ((pEnetConfig != NULL) && (pEnetConfig->LinkSpeed > 100U)) {
        cnt = 64U;
    }
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (AmbaWrap_memset(&sockAddr, 0, sizeof(sockAddr))!= 0U) { }
    AmbaMisra_TypeCast(&pSock, &pSockIn);
    sockAddr.sin_addr.s_addr = DstIp;
    sockAddr.sin_port = htons(5001U);
    pSock->sa_family = AF_INET;

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

UINT32 LwIPBSDExample_RxTcp(void)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    fd_set readfd, testfd;
    CHAR buffer[ETH_FRAMES_SIZE] = "";
    INT maxfd, j;
    INT Ret,  csd, sd, data_len;
    socklen_t addrlen = sizeof(struct sockaddr_in);
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

    if (AmbaWrap_memset(&serverAddr, 0, sizeof(serverAddr))!= 0U) { }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(5001U);
    AmbaMisra_TypeCast(&pSock, &pSockIn);

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
        for (j = 0; j <= maxfd; j++)
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
    for (j = 0; j <= maxfd; j++) {
        if (FD_ISSET(j, &readfd) != 0) {
            Ret = close(j);
            AmbaPrint_PrintUInt5("close(%d) returned %d", (UINT32)j, (UINT32)Ret, 0U, 0U, 0U);
        }
    }

    return (UINT32)Ret;
}
