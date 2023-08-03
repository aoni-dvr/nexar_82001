/**
 * @file AmbaShell_Enet.c
 * Enet shell command.
 *
 * @ingroup enet
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaShell_Enet.h"
#include "AmbaENET.h"
#include "AmbaPrint.h"

#if defined(CONFIG_NETX_ENET) || defined(CONFIG_AMBA_UDPIP) || defined(CONFIG_AMBA_AVB)
#include "NetStack.h"
#endif

#if defined(CONFIG_NETX_ENET) || defined(CONFIG_LWIP_ENET) || defined(CONFIG_AMBA_UDPIP)
#include "AmbaKAL.h"
#endif
#ifdef CONFIG_LWIP_ENET
#include "RefCode_LwIP.h"
#endif

#ifdef CONFIG_NETX_ENET
#include "RefCode_NetX.h"
#include "NetXStack.h"
#ifdef CONFIG_NETX_TELNET
#include "AmbaUART.h"
#endif
#endif
#ifdef CONFIG_AMBA_UDPIP
#include "DemoStack.h"
#endif
#ifdef CONFIG_AMBA_AVB
#include "AvbStack.h"
#ifdef AVDECC_REFCODE
#include "RefCode_Avdecc.h"
#endif
#ifdef CONFIG_AMBA_AVTP_REFCODE
#include "RefCode_Avtp.h"
#include "AmbaCFS.h"
#include "RefCode_FileFeed.h"
#endif
#endif

void EnetPrint(const char *func, const UINT32 line, const char *color, const char *msg)
{
#if defined(CONFIG_THREADX)
    char Buf[22];

    (void)AmbaUtility_UInt32ToStr(Buf, sizeof(Buf), line, 10U);

    AmbaPrint_PrintStr5("%s ENET %s(%s) %s %s", color, func, Buf, msg, ANSI_RESET);
#else
    fprintf(stderr, "%s ENET %s(%d) %s %s\n", color, func, line, msg, ANSI_RESET);
#endif
}

static void EnetUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("switch eth0/eth1:\n");
    PrintFunc("  *enet idx <idx>\n");
    PrintFunc("    enet idx 0\n");
    PrintFunc("driver:\n");
    PrintFunc("  *enet init <mac>\n");
    PrintFunc("    enet init 00:11:22:33:44:55\n");
#ifdef CONFIG_LWIP_ENET
    PrintFunc("lwip tcpip stack:\n");
    PrintFunc("  *init lwip Stack (after enet init)\n");
    PrintFunc("    enet lwipinit\n");
    PrintFunc("  *config lwip IP\n");
    PrintFunc("    enet lwip ifconfig 192.168.1.100\n");
    PrintFunc("  *throughput test (by bsd API)\n");
    PrintFunc("    enet lwip bsd_udp_tx 192.168.1.2\n");
    PrintFunc("    enet lwip bsd_udp_rx\n");
    PrintFunc("    enet lwip bsd_tcp_tx 192.168.1.2\n");
    PrintFunc("    enet lwip bsd_tcp_rx\n");
#endif
#ifdef CONFIG_NETX_ENET
    PrintFunc("netx tcpip stack:\n");
    PrintFunc("  *init NetX Stack (after enet init)\n");
    PrintFunc("    enet netxinit\n");
    PrintFunc("  *config NetX IP\n");
    PrintFunc("    enet netx ifconfig 192.168.1.100\n");
    PrintFunc("  *ping\n");
    PrintFunc("    enet netx ping 192.168.1.2\n");
    PrintFunc("  *throughput test (by native API)\n");
    PrintFunc("    enet netx nx_udp_tx 192.168.1.2\n");
    PrintFunc("    enet netx nx_udp_rx\n");
    PrintFunc("    enet netx nx_tcp_tx 192.168.1.2\n");
    PrintFunc("    enet netx nx_tcp_rx\n");
    PrintFunc("  *throughput test (by bsd API)\n");
    PrintFunc("    enet netx nxbsd_udp_tx 192.168.1.2\n");
    PrintFunc("    enet netx nxbsd_udp_rx\n");
    PrintFunc("    enet netx nxbsd_tcp_tx 192.168.1.2\n");
    PrintFunc("    enet netx nxbsd_tcp_rx\n");
#ifdef CONFIG_NETX_TELNET
    PrintFunc("  *start netx telnet server \n");
    PrintFunc("    enet netx telnetd\n");
#endif
#endif
#ifdef CONFIG_AMBA_UDPIP
    PrintFunc("demo udp stack:\n");
    PrintFunc("  *init UDP Stack (after enet init)\n");
    PrintFunc("    enet udpinit\n");
    PrintFunc("  *config Demo IP\n");
    PrintFunc("    enet ifconfig 192.168.1.100\n");
    PrintFunc("  *ping\n");
    PrintFunc("    enet ping 192.168.1.2\n");
    PrintFunc("  *throughput test\n");
    PrintFunc("    enet demo_udp_tx 192.168.1.2\n");
#endif
#ifdef CONFIG_AMBA_AVB
    PrintFunc("amba avb stack:\n");
    PrintFunc("  *init avb, gPTP auto mode (after enet init)\n");
    PrintFunc("    enet avbinit\n");
    PrintFunc("  *init avb, force gPTP master mode (after enet init)\n");
    PrintFunc("    enet avbmaster\n");
    PrintFunc("  *init avb, force gPTP slave mode (after enet init)\n");
    PrintFunc("    enet avbslave\n");
    PrintFunc("  *show ptp hardware clock value\n");
    PrintFunc("    enet phcget\n");
    PrintFunc("  *set ptp hardware clock second value\n");
    PrintFunc("    enet phcsetsec 1\n");
#ifdef CONFIG_AMBA_AVTP_REFCODE
    PrintFunc("  *Set avtp talker[0] stream mac ba:bc:1a:ba:bc:1a\n");
    PrintFunc("    enet avbtalker streammac 0 ba:bc:1a:ba:bc:1a\n");
    PrintFunc("  *Set avtp talker[0] stream id 55\n");
    PrintFunc("    enet avbtalker streamid 0 55\n");
    PrintFunc("  *Set avtp talker[0] file feed 0 \n");
    PrintFunc("    enet avbtalker filefeed 0 0 \n");
    PrintFunc("  *start avtp talker[0], stream to 91:e0:f0:00:fe:55\n");
    PrintFunc("    enet avbtalker start 0 91:e0:f0:00:fe:55\n");
    PrintFunc("  *stop avtp talker[0] \n");
    PrintFunc("    enet avbtalker stop 0\n");
    PrintFunc("  *Set avtp listener[0] stream mac ba:bc:1a:ba:bc:1a\n");
    PrintFunc("    enet avblistener streammac 0 ba:bc:1a:ba:bc:1a\n");
    PrintFunc("  *Set avtp listener[0] stream id 55\n");
    PrintFunc("    enet avblistener streamid 0 55\n");
    PrintFunc("  *start avtp listener[0], listen to 91:e0:f0:00:fe:55\n");
    PrintFunc("    enet avblistener start 0 91:e0:f0:00:fe:55\n");
    PrintFunc("  *stop avtp listener[0] \n");
    PrintFunc("    enet avblistener stop 0\n");
    PrintFunc("  *Start file feed[0] xxxxx \n");
    PrintFunc("    enet filefeed start 0 xxxxx\n");
#endif // CONFIG_AMBA_AVTP_REFCODE
#ifdef AVDECC_REFCODE
    PrintFunc("  *[AVDECC] Init AVDECC Talker Entity, Listener Entity as disable(0) or enable(1)\n");
    PrintFunc("    enet avdecc {0|1} {0|1}\n");
#endif
#endif
}

#ifdef CONFIG_NETX_ENET
#ifdef CONFIG_NETX_TELNET
void AmbaTelnet_ConsoleWrite(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;
    UINT32 TxSize = StringSize;

    while (AmbaUART_Write(0U, 0U, StringSize, (const UINT8 *)StringBuf, &SentSize, TimeOut) == 0U) {
        TxSize -= SentSize;
        if (TxSize == 0U) {
            break;
        }
    }
    (void)AmbaTelnet_Write((const UINT8 *)StringBuf, StringSize);
}

UINT32 AmbaTelnet_ShellRead(UINT32 StringSize, char *StringBuf, UINT32 TimeOut)
{
    UINT32 UARTRxSize = 0U, TelnetRxSize = 0U, RxSize;

    /*ignore TimeOut*/
    (void) TimeOut;
    (void) AmbaUART_Read(0U, 0U, StringSize, (UINT8 *)StringBuf, &UARTRxSize, 1U);

    if (UARTRxSize == 0U) {
        (void) AmbaTelnet_Read((UINT8 *)StringBuf, StringSize, &TelnetRxSize);
    }

    RxSize = UARTRxSize + TelnetRxSize;
    return RxSize;
}

UINT32 AmbaTelnet_ShellWrite(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    UINT32 SentSize;

    (void)AmbaUART_Write(0U, 0U, StringSize, (const UINT8 *)StringBuf, &SentSize, TimeOut);
    (void)AmbaTelnet_Write((const UINT8 *)StringBuf, StringSize);

    return SentSize;
}
#endif /* CONFIG_NETX_TELNET */
#endif /* CONFIG_NETX_ENET */

#ifdef CONFIG_AMBA_UDPIP
static void UdpInit(UINT32 Idx)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) AmbaDemoStack_GetConfig(Idx, &pDemoConfig);
    pDemoConfig->Idx = Idx;
    if (pEnetConfig != NULL) {
        if (AmbaWrap_memcpy(pDemoConfig->Mac, pEnetConfig->Mac, 6)!= 0U) { }
    }
    (void)AmbaDemoStack_EnetInit(pDemoConfig);

    //802.1Q Virtual LAN, [15:13]PRI: 3, [11:0]ID: 6
    //pDemoConfig->tci = 0x6006U;
}
#endif

#if defined(CONFIG_LWIP_ENET) || defined(CONFIG_AMBA_UDPIP)
//192.168.1.100->0x64 01 a8 c0
static INT32 Str2NetOdrIP(const char *src, UINT32 *addr)
{
    INT32 saw_digit, octets;
    char ch;
    UINT8 tmp[4], *tp;
    INT32 Ret = 0;
    const char *chr;

    saw_digit   = 0;
    octets      = 0;
    tp = tmp;
    *(tp) = 0;
    ch = *src;
    chr = src;
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
        chr++;
        ch = *chr;
    }
    if (octets < 4) {
        Ret = 1;
    }

    if (AmbaWrap_memcpy(addr, tmp, sizeof(*addr))!= 0U) { }
    return Ret;
}
#endif /* CONFIG_AMBA_UDPIP */

#ifdef CONFIG_NETX_ENET
//192.168.1.100->0xc0 a8 01 64
static INT32 Str2HostOdrIP(const char *src, UINT32 *addr)
{
    INT32 saw_digit, octets;
    char ch;
    UINT8 tmp[4], *tp;
    INT32 Ret = 0;
    const char *chr;

    saw_digit   = 0;
    octets      = 0;
    tp = &tmp[3];
    *(tp) = 0;
    ch = *src;
    chr = src;
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
            tp--;
            *tp = 0U;
            saw_digit = 0;
        } else {
            Ret = 1;
            break;
        }
        chr++;
        ch = *chr;
    }
    if (octets < 4) {
        Ret = 1;
    }

    if (AmbaWrap_memcpy(addr, tmp, sizeof(*addr))!= 0U) { }
    return Ret;
}
#endif /* CONFIG_NETX_ENET */

//00:11:22:33:44:55->0x001122334455
INT32 Str2Mac(const char *bufp, UINT8 *Mac)
{
    UINT8 *ptr;
    UINT32 i, j;
    INT32 Ret = 0;
    UINT8 byte, halfbyte, c;
    const UINT8 lowcase = 'a' - 'A';
    const char *chr;

    ptr = Mac;
    chr = bufp;
    i   = 0;
    do {
        j = 0;
        byte = 0;
        i++;

        /* We might get a semicolon here - not required. */
        if (*chr == ':') {
            chr++;
        }

        do {
            c = (UINT8)(*chr);
            //'0'->0x0
            if ((c - (UINT8)'0') <= (UINT8)9) {
                halfbyte = c - (UINT8)'0';

            }
            //'a'->0xa
            else if ((UINT8)((c | lowcase) - (UINT8)'a') <= 5U) {
                halfbyte = (c | lowcase) - (UINT8)'a';
                halfbyte += (UINT8)0xa;
            }
            else {
                if(c != (UINT8)':') {
                    Ret = 1;
                }
                break;
            }
            ++chr;
            byte <<= 4;
            byte  += halfbyte;
        } while (++j < 2U);
        *ptr = byte;
        ptr++;
    } while (i < 6U);

    return Ret;
}

static void EnetInit(UINT32 Idx, const char *mac_addr)
{
    AMBA_ENET_CONFIG_s *pEnetConfig = NULL;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    if (pEnetConfig != NULL) {
        (void) Str2Mac(mac_addr, pEnetConfig->Mac);
        (void)AmbaEnet_Init(pEnetConfig);
    }
}

static UINT32 IdxCopy = 0;

#if defined(CONFIG_NETX_ENET) || defined(CONFIG_LWIP_ENET) || defined(CONFIG_AMBA_UDPIP)
static UINT32 Pause = 0U;
static UINT32 IPARG;
static UINT32 TestPri[2] = {250, 251};
#if defined(CONFIG_NETX_ENET) || defined(CONFIG_LWIP_ENET)
#define RXTEST        0x00U
#endif
#define TXTEST        0x01U
#ifdef CONFIG_NETX_ENET
#define RXUDP_NETX    0x00U
#define RXTCP_NETX    0x01U
#define RXUDP_NXBSD   0x02U
#define RXTCP_NXBSD   0x03U
#define TXUDP_NETX    0x04U
#define TXTCP_NETX    0x05U
#define TXUDP_NXBSD   0x06U
#define TXTCP_NXBSD   0x07U
#endif
#ifdef CONFIG_AMBA_UDPIP
#define TXUDP_DEMO    0x08U
#endif
#ifdef CONFIG_LWIP_ENET
#define RXUDP_LWBSD   0x09U
#define RXTCP_LWBSD   0x0aU
#define TXUDP_LWBSD   0x0bU
#define TXTCP_LWBSD   0x0cU
#endif

static void *LaunchTest(void *arg)
{
    UINT32 TestItem;

    /*TestItem = (UINT32)arg;*/
    (void)arg;
    if (AmbaWrap_memcpy(&TestItem, &arg, sizeof(TestItem))!= 0U) { }
    switch (TestItem) {
#ifdef CONFIG_NETX_ENET
    case RXUDP_NETX:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXExample_RxUdp()");
        (void) NetXExample_RxUdp(IdxCopy);
        break;
    case RXTCP_NETX:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXExample_RxTcp()");
        (void) NetXExample_RxTcp(IdxCopy);
        break;
    case RXUDP_NXBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXBSDExample_RxUdp()");
        (void) NetXBSDExample_RxUdp();
        break;
    case RXTCP_NXBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXBSDExample_RxTcp()");
        (void) NetXBSDExample_RxTcp();
        break;
    case TXUDP_NETX:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXExample_TxUdp()");
        (void) NetXExample_TxUdp(IdxCopy, IPARG, Pause);
        break;
    case TXTCP_NETX:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXExample_TxTcp()");
        (void) NetXExample_TxTcp(IdxCopy, IPARG, Pause);
        break;
    case TXUDP_NXBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXBSDExample_TxUdp()");
        (void) NetXBSDExample_TxUdp(IPARG, Pause);
        break;
    case TXTCP_NXBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "NetXBSDExample_TxTcp()");
        (void) NetXBSDExample_TxTcp(IPARG, Pause);
        break;
#endif
#ifdef CONFIG_LWIP_ENET
    case RXUDP_LWBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_RxUdp()");
        (void) LwIPBSDExample_RxUdp();
        break;
    case RXTCP_LWBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_RxTcp()");
        (void) LwIPBSDExample_RxTcp();
        break;
    case TXUDP_LWBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_TxUdp()");
        (void) LwIPBSDExample_TxUdp(IPARG, Pause);
        break;
    case TXTCP_LWBSD:
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "LwIPBSDExample_TxTcp()");
        (void) LwIPBSDExample_TxTcp(IPARG, Pause);
        break;
#endif
#ifdef CONFIG_AMBA_UDPIP
    case TXUDP_DEMO:
        {
            UINT32 i, j;
            EnetPrint(__func__, __LINE__, ANSI_WHITE, "AmbaDemoStack_TxUdp()");
            for (i = 0U; i < (1024U*8U*64U); i++) {
                /* Relinquish to other task */
                for (j = 0U; j < Pause; j++) {
                    (void) AmbaKAL_TaskYield();
                }
                (void)AmbaDemoStack_TxUdp(IdxCopy, IPARG);
            }
        }
        break;
#endif
    default:
        /* TBD */
        break;
    }

    AmbaPrint_PrintUInt5("TestItem %d end", TestItem, 0U, 0U, 0U, 0U);
    return NULL;
}

static void ForkTest(UINT32 Idx, UINT32 TestItem)
{
#define TestStackSize 16384U
    static UINT8 Stack[2][16384] __attribute__((section(".bss.noinit")));
    static char taskname[2][32] = { "RxTask", "TxTask" };
    UINT32 err;
    const void *arg;
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

        if (AmbaWrap_memset(&Stack[Idx][0], 0, TestStackSize)!= 0U) { }
        pBuf = &Stack[Idx][0];
        AmbaMisra_TypeCast(&ptr, &pBuf);

        AmbaPrint_PrintUInt5("TestItem %d Pri %d", TestItem, TestPri[Idx], 0U, 0U, 0U);
        /*arg = (void *)TestItem;*/
        arg = NULL;
        if (AmbaWrap_memcpy(&arg, &TestItem, sizeof(TestItem))!= 0U) { }
        err = AmbaKAL_TaskCreate(&TestTask[Idx],
                                 taskname[Idx],
                                 TestPri[Idx],
                                 LaunchTest,
                                 arg,
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
#endif

#ifdef CONFIG_AMBA_AVB
static void AvbInit(UINT32 Idx, UINT32 Mode)
{
    AMBA_ENET_CONFIG_s *pEnetConfig;
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;

    (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
    (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    pAvbConfig->Idx = Idx;
    pAvbConfig->PtpConfig.MstSlv = Mode;
    if (pEnetConfig != NULL) {
        if (AmbaWrap_memcpy(pAvbConfig->Mac, pEnetConfig->Mac, 6)!= 0U) { }
    }

    (void)AmbaAvbStack_EnetInit(pAvbConfig);
    {
//        extern UINT32 Avdecc_Process(const UINT32 Idx, const UINT8 DstMac[6U], const UINT8 SrcMac[6U],
//                      const void *L3Frame, UINT16 L3Len);
//        pAvbConfig->pAvdeccCb = Avdecc_Process;
#ifdef AVDECC_REFCODE
        UINT32 avdecc_process(const UINT32 Idx,
                                     const UINT8 dmac[6],
                                     const UINT8 smac[6],
                                     const void *L3Frame,
                                     const UINT16 L3Len);

        pAvbConfig->pAvdeccCb = avdecc_process;
#endif
    }
}
#endif

static void CurrentIdx(AMBA_SHELL_PRINT_f PrintFunc)
{
    char IdxStr[4];

    (void) AmbaUtility_UInt32ToStr(IdxStr, sizeof(IdxStr), IdxCopy, 10U);
    PrintFunc("[eth");
    PrintFunc(IdxStr);
    PrintFunc("]\n");
}

#ifdef CONFIG_AMBA_AVTP_REFCODE
static void CmdEnet_avbtalker(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Id;

    if(InitTalkerConfig == 0U) {
        if (AmbaWrap_memset(TalkerConfig, 0, sizeof(TalkerConfig))!= 0U) { }
        InitTalkerConfig = 1U;
    }

    (void) AmbaUtility_StringToUInt32(argv[3], &Id);
    if(Id < MAX_TALKER_NUM) {
        //enet avbtalker streammac 0 ba:bc:1a:ba:bc:1a
        if (0 == AmbaUtility_StringCompare(argv[2], "streammac", AmbaUtility_StringLength("streammac"))) {
            (void) Str2Mac(argv[4], TalkerConfig[Id].StreamID.Mac);
        }
        //enet avbtalker streamid 0 55
        else if (0 == AmbaUtility_StringCompare(argv[2], "streamid", AmbaUtility_StringLength("streamid"))) {
            UINT32 streamid;

            (void) AmbaUtility_StringToUInt32(argv[4], &streamid);
            TalkerConfig[Id].StreamID.UniqueID = (UINT16) (((streamid & 0xffU) << 8) | ((streamid & 0xff00U) >> 8));
        }
        //enet avbtalker file feed 0 0
        else if (0 == AmbaUtility_StringCompare(argv[2], "filefeed", AmbaUtility_StringLength("filefeed"))) {
            UINT32 FileFeedId;
            UINT32 Fps;

            if(InitFifoConfig == 0U) {
                if (AmbaWrap_memset(FifoConfig, 0, sizeof(FifoConfig))!= 0U) { }
                InitFifoConfig = 1U;
            }

            (void) AmbaUtility_StringToUInt32(argv[4], &FileFeedId);

            FifoConfig[Id].pFifoHdlr = FileFeedConfig[FileFeedId].pFifo;
            FifoConfig[Id].pBuffer = FileFeedConfig[FileFeedId].pBuffer;
            FifoConfig[Id].BufferSize = FileFeedConfig[FileFeedId].BufferSize;
            Fps = FileFeedConfig[FileFeedId].TimeScale/FileFeedConfig[FileFeedId].TimePerFrame;
            AvtpTalker_FpsControl(Id, Fps+1U);
        }
        //enet avbtalker start 0 91:e0:f0:00:fe:55
        else if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
            (void) Str2Mac(argv[4], TalkerConfig[Id].MultiCastMac);
            AvtpTalkerStart(Id);
        } else {
            EnetUsage(PrintFunc);
        }
    } else {
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "Id over range");
    }
}

static void CmdEnet_avblistener(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Id;

    if(InitListenerConfig == 0U) {
        if (AmbaWrap_memset(ListenerConfig, 0, sizeof(ListenerConfig))!= 0U) { }
        InitListenerConfig = 1U;
    }

    (void) AmbaUtility_StringToUInt32(argv[3], &Id);
    if(Id < MAX_LISTENER_NUM) {
        //enet avblistener streammac 0 ba:bc:1a:ba:bc:1a
        if (0 == AmbaUtility_StringCompare(argv[2], "streammac", AmbaUtility_StringLength("streammac"))) {
            (void) Str2Mac(argv[4], ListenerConfig[Id].StreamID.Mac);
        }
        //enet avblistener streamid 0 55
        else if (0 == AmbaUtility_StringCompare(argv[2], "streamid", AmbaUtility_StringLength("streamid"))) {
            UINT32 streamid;

            (void) AmbaUtility_StringToUInt32(argv[4], &streamid);
            ListenerConfig[Id].StreamID.UniqueID = (UINT16) (((streamid & 0xffU) << 8) | ((streamid & 0xff00U) >> 8));
        }
        //enet avblistener start 0 91:e0:f0:00:fe:55
        else if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
            (void) Str2Mac(argv[4], ListenerConfig[Id].MultiCastMac);
            AvtpListenerStart(Id);
        } else {
            EnetUsage(PrintFunc);
        }
    } else {
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "Id over range");
    }
}

static void CmdEnet_filefeed(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Id;

    if(InitFileFeedConfig == 0U) {
        if (AmbaWrap_memset(FileFeedConfig, 0, sizeof(FileFeedConfig))!= 0U) { }
        InitFileFeedConfig = 1U;
    }

    (void) AmbaUtility_StringToUInt32(argv[3], &Id);
    if(Id < MAX_FILE_FEED_NUM) {
        //enet filefeed start 0 C:\OUT_0000
        if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
            AmbaUtility_StringCopy(FileFeedConfig[Id].Prefix, AMBA_CFS_MAX_FILENAME_LENGTH, argv[4]);
            FileFeedConfig[Id].Type = 1U;
            (void) RefCode_FileFeed_Start(Id);
        } else {
            EnetUsage(PrintFunc);
        }
    } else {
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "Id over range");
    }
}
#endif // CONFIG_AMBA_AVTP_REFCODE

#ifdef CONFIG_LWIP_ENET
static void CmdEnet_lwip(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet lwip txpause 624
    if (0 == AmbaUtility_StringCompare(argv[2], "txpause", AmbaUtility_StringLength("txpause"))) {
        (void) AmbaUtility_StringToUInt32(argv[3], &Pause);
    }
    //enet lwip rxpri 250
    else if (0 == AmbaUtility_StringCompare(argv[2], "rxpri", AmbaUtility_StringLength("rxpri"))) {
        UINT32 Value;
        (void) AmbaUtility_StringToUInt32(argv[3], &Value);
        TestPri[0] = Value;
    }
    //enet lwip txpri 251
    else if (0 == AmbaUtility_StringCompare(argv[2], "txpri", AmbaUtility_StringLength("txpri"))) {
        UINT32 Value;
        (void) AmbaUtility_StringToUInt32(argv[3], &Value);
        TestPri[1] = Value;
    }
    //enet lwip ifconfig 192.168.1.100
    else if (0 == AmbaUtility_StringCompare(argv[2], "ifconfig", AmbaUtility_StringLength("ifconfig"))) {
        UINT32 IpAddr;
        (void) Str2NetOdrIP(argv[3], &IpAddr);
        sys_lwip_ifconfig(IdxCopy, IpAddr, 0x00ffffffU, 0U);
    }
    //enet lwip bsd_udp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "bsd_udp_tx", AmbaUtility_StringLength("bsd_udp_tx"))) {
        (void) Str2NetOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXUDP_LWBSD);
    }
    //enet lwip bsd_tcp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "bsd_tcp_tx", AmbaUtility_StringLength("bsd_tcp_tx"))) {
        (void) Str2NetOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXTCP_LWBSD);
    }
    else {
        EnetUsage(PrintFunc);
    }
}
#endif // CONFIG_LWIP_ENET

#ifdef CONFIG_NETX_ENET
static void CmdEnet_netx(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet netx txpause 624
    if (0 == AmbaUtility_StringCompare(argv[2], "txpause", AmbaUtility_StringLength("txpause"))) {
        (void) AmbaUtility_StringToUInt32(argv[3], &Pause);
    }
    //enet netx rxpri 250
    else if (0 == AmbaUtility_StringCompare(argv[2], "rxpri", AmbaUtility_StringLength("rxpri"))) {
        UINT32 Value = 0U;
        (void) AmbaUtility_StringToUInt32(argv[3], &Value);
        TestPri[0] = Value;
    }
    //enet netx txpri 251
    else if (0 == AmbaUtility_StringCompare(argv[2], "txpri", AmbaUtility_StringLength("txpri"))) {
        UINT32 Value = 0U;
        (void) AmbaUtility_StringToUInt32(argv[3], &Value);
        TestPri[1] = Value;
    }
    //enet netx ifconfig 192.168.1.100
    else if (0 == AmbaUtility_StringCompare(argv[2], "ifconfig", AmbaUtility_StringLength("ifconfig"))) {
        UINT32 IpAddr = 0U;
        (void) Str2HostOdrIP(argv[3], &IpAddr);
        (void) NetXStack_EnetIfconfig(IdxCopy, IpAddr, 0xffffff00U);
    }
    //enet netx ping 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "ping", AmbaUtility_StringLength("ping"))) {
        UINT32 IpAddr = 0U;
        (void) Str2HostOdrIP(argv[3], &IpAddr);
        (void) NetXStack_Ping(IdxCopy, IpAddr, 3200U);
    }
    //enet netx nx_udp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "nx_udp_tx", AmbaUtility_StringLength("nx_udp_tx"))) {
        (void) Str2HostOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXUDP_NETX);
    }
    //enet netx nxbsd_udp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "nxbsd_udp_tx", AmbaUtility_StringLength("nxbsd_udp_tx"))) {
        (void) Str2HostOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXUDP_NXBSD);
    }
    //enet netx nx_tcp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "nx_tcp_tx", AmbaUtility_StringLength("nx_tcp_tx"))) {
        (void) Str2HostOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXTCP_NETX);
    }
    //enet netx nxbsd_tcp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[2], "nxbsd_tcp_tx", AmbaUtility_StringLength("nxbsd_tcp_tx"))) {
        (void) Str2HostOdrIP(argv[3], &IPARG);
        ForkTest(TXTEST, TXTCP_NXBSD);
    }
    else {
        EnetUsage(PrintFunc);
    }
}
#endif

#ifdef CONFIG_AMBA_UDPIP
static void CmdEnet_demo_udp_tx(char * const * argv)
{
    AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
    (void) AmbaDemoStack_GetConfig(IdxCopy, &pDemoConfig);
    if ((pDemoConfig->IcmpClnt.arp.Mac[0] == 0x0U) &&
        (pDemoConfig->IcmpClnt.arp.Mac[1] == 0x0U) &&
        (pDemoConfig->IcmpClnt.arp.Mac[2] == 0x0U) &&
        (pDemoConfig->IcmpClnt.arp.Mac[3] == 0x0U) &&
        (pDemoConfig->IcmpClnt.arp.Mac[4] == 0x0U) &&
        (pDemoConfig->IcmpClnt.arp.Mac[5] == 0x0U)) {
        EnetPrint(__func__, __LINE__, ANSI_RED, "arp table empty, try to PING first");
    } else {
        (void) Str2NetOdrIP(argv[2], &IPARG);
        ForkTest(TXTEST, TXUDP_DEMO);
    }
}
#endif

#ifdef CONFIG_AMBA_AVB
static void CmdEnet_ptp(UINT32 ctrlBit)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    static INT32 onoff = 0;

    onoff ++;
    onoff %= 2;
    (void) AmbaAvbStack_GetConfig(IdxCopy, &pAvbConfig);
    if (onoff == 0) {
        pAvbConfig->AvbLoggingCtrl &= ~ctrlBit;
    } else {
        pAvbConfig->AvbLoggingCtrl |= ctrlBit;
    }
}

static void CmdEnet_ppb(char * const * argv)
{
    UINT32 ppbu;
    INT32 ppb;
    (void) AmbaUtility_StringToUInt32(&argv[2][1], &ppbu);

    if (argv[2][0] == '+') {
        (void) AmbaEnet_PhcAdjFreq(IdxCopy, (INT32)ppbu);
        AmbaPrint_PrintUInt5("ppb updted %d", ppbu, 0, 0, 0, 0);
    } else {
        ppb = -(INT32)ppbu;
        (void) AmbaEnet_PhcAdjFreq(IdxCopy, ppb);
        AmbaPrint_PrintInt5("ppb updted %d", ppb, 0, 0, 0, 0);
    }
}
#endif // CONFIG_AMBA_AVB

#ifdef CONFIG_NETX_ENET
static void CmdEnet_nxFunc(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet netx nx_udp_rx
    if (0 == AmbaUtility_StringCompare(argv[2], "nx_udp_rx", AmbaUtility_StringLength("nx_udp_rx"))) {
        ForkTest(RXTEST, RXUDP_NETX);
    }
    //enet netx nxbsd_udp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "nxbsd_udp_rx", AmbaUtility_StringLength("nxbsd_udp_rx"))) {
        ForkTest(RXTEST, RXUDP_NXBSD);
    }
    //enet netx nx_tcp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "nx_tcp_rx", AmbaUtility_StringLength("nx_tcp_rx"))) {
        ForkTest(RXTEST, RXTCP_NETX);
    }
    //enet netx nxbsd_tcp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "nxbsd_tcp_rx", AmbaUtility_StringLength("nxbsd_tcp_rx"))) {
        ForkTest(RXTEST, RXTCP_NXBSD);
    }
    else {
        EnetUsage(PrintFunc);
    }
}
#endif

static void CmdEnetArgc5(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet pps single 100 1
    //enet pps train 100 1
    if (0 == AmbaUtility_StringCompare(argv[1], "pps", AmbaUtility_StringLength("pps"))) {
#if defined(CONFIG_AMBA_AVB) && !defined(CONFIG_SOC_H22)
        const UINT32 ms = 1000000U;
        UINT32 Sec, Ns, PPSINT, PPSWIDTH;
        char SecStr[22];
        char NsStr[22];

        (void) AmbaUtility_StringToUInt32(argv[3], &PPSINT);
        (void) AmbaUtility_StringToUInt32(argv[4], &PPSWIDTH);
        (void) AmbaAvbStack_GetPhc(IdxCopy, &Sec, &Ns);
        (void) AmbaUtility_UInt32ToStr(SecStr, sizeof(SecStr), Sec, 10U);
        (void) AmbaUtility_UInt32ToStr(NsStr, sizeof(NsStr), Ns, 10U);
        PrintFunc("Current Time ");
        PrintFunc(SecStr);
        PrintFunc(".");
        PrintFunc(NsStr);
        PrintFunc("\n");

        /* start pulse at 4 sec round-up */
        Sec /= 4U;
        Sec *= 4U;
        Sec += 4U;
        (void) AmbaUtility_UInt32ToStr(SecStr, sizeof(SecStr), Sec, 10U);
        PrintFunc("Trigger PPS at ");
        PrintFunc(SecStr);
        PrintFunc(".0 \n");

        (void) AmbaEnet_SetPPS(IdxCopy, PPSINT*ms, PPSWIDTH*ms);

        /* PPSCMD=5: Stop Pulse Train immediately */
        (void) AmbaEnet_SetTargetTsPPS(IdxCopy, Sec, 0U, 5U);
        if (0 == AmbaUtility_StringCompare(argv[2], "train", AmbaUtility_StringLength("train"))) {
            /* PPSCMD=2: Start Pulse Train at specific time */
            (void) AmbaEnet_SetTargetTsPPS(IdxCopy, Sec, 0U, 2U);
        } else {
            /* PPSCMD=1: Start Single Pulse */
            (void) AmbaEnet_SetTargetTsPPS(IdxCopy, Sec, 0U, 1U);
        }
        (void) AmbaEnet_SetTargetTsIrq(IdxCopy, Sec, 0U);
#endif
    }
#ifdef CONFIG_AMBA_AVTP_REFCODE
    else if (0 == AmbaUtility_StringCompare(argv[1], "avbtalker", AmbaUtility_StringLength("avbtalker"))) {
        CmdEnet_avbtalker(argv, PrintFunc);
    } else if (0 == AmbaUtility_StringCompare(argv[1], "avblistener", AmbaUtility_StringLength("avblistener"))) {
        CmdEnet_avblistener(argv, PrintFunc);
    }
    else if (0 == AmbaUtility_StringCompare(argv[1], "filefeed", AmbaUtility_StringLength("filefeed"))) {
        CmdEnet_filefeed(argv, PrintFunc);
    }
#endif // CONFIG_AMBA_AVTP_REFCODE
    else {
        EnetUsage(PrintFunc);
    }
}

static void CmdEnetArgc4(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet phywrite 31 2
    if (0 == AmbaUtility_StringCompare(argv[1], "phywrite", AmbaUtility_StringLength("phywrite"))) {
        UINT32 Offset = 0U;
        UINT32 Value = 0U;
        (void) AmbaUtility_StringToUInt32(argv[2], &Offset);
        (void) AmbaUtility_StringToUInt32(argv[3], &Value);
        (void) AmbaEnet_PhyWrite(IdxCopy, 0U, Offset, Value);
    }
#ifdef CONFIG_LWIP_ENET
    else if (0 == AmbaUtility_StringCompare(argv[1], "lwip", AmbaUtility_StringLength("lwip"))) {
        CmdEnet_lwip(argv, PrintFunc);
    }
#endif // CONFIG_LWIP_ENET
#ifdef CONFIG_NETX_ENET
    else if (0 == AmbaUtility_StringCompare(argv[1], "netx", AmbaUtility_StringLength("netx"))) {
        CmdEnet_netx(argv, PrintFunc);
    }
#endif // CONFIG_NETX_ENET
#ifdef CONFIG_AMBA_AVTP_REFCODE
    //enet avbtalker stop 0 0
    else if (0 == AmbaUtility_StringCompare(argv[1], "avbtalker", AmbaUtility_StringLength("avbtalker"))) {
        if (0 == AmbaUtility_StringCompare(argv[2], "stop", AmbaUtility_StringLength("stop"))) {
            UINT32 Id;

            (void) AmbaUtility_StringToUInt32(argv[3], &Id);
            AvtpTalkerStop(Id);
        }
    }
    //enet avblistener stop 0 0
    else if (0 == AmbaUtility_StringCompare(argv[1], "avblistener", AmbaUtility_StringLength("avblistener"))) {
        if (0 == AmbaUtility_StringCompare(argv[2], "stop", AmbaUtility_StringLength("stop"))) {
            UINT32 Id;

            (void) AmbaUtility_StringToUInt32(argv[3], &Id);
            AvtpListenerStop(Id);
        }
    }
#endif // CONFIG_AMBA_AVTP_REFCODE
#ifdef AVDECC_REFCODE
    // AVDECC
    else if (0 == AmbaUtility_StringCompare(argv[1], "avdecc", AmbaUtility_StringLength("avdecc"))) {
        //enet avdecc eid 0x001122FFFF334455
        if (0 == AmbaUtility_StringCompare(argv[2], "eid", AmbaUtility_StringLength("eid"))) {
            UINT64 eid;

            (void)AmbaUtility_StringToUInt64(argv[3], &eid);
            /* Set Entity-ID (in hex) that are listened to for AVDECC Fast-Connect */
            AvdeccFCId(eid);
        //enet avdecc adp 0
        } else if (0 == AmbaUtility_StringCompare(argv[2], "adp", AmbaUtility_StringLength("adp"))) {
            UINT32 msg;

            (void)AmbaUtility_StringToUInt32(argv[3], &msg);
            /* Send ADP Msg: Available (0), Departing (1), Discover all (2) */
            AvdeccAdpMsg(msg);
        } else {
            PrintFunc("not support cmd!");
#if 0
            UINT32 t, l, c;
            (void)AmbaUtility_StringToUInt32(argv[2], &t);
            (void)AmbaUtility_StringToUInt32(argv[3], &l);
            (void)AmbaUtility_StringToUInt32(argv[4], &c);
                AvdeccTLStatus(t, l, c);
#endif
        }
    }
#endif // AVDECC_REFCODE
    else {
        EnetUsage(PrintFunc);
    }
}

static void CmdEnetArgc3(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet idx 0
    if (0 == AmbaUtility_StringCompare(argv[1], "idx", AmbaUtility_StringLength("idx"))) {
        UINT32 Value = 0U;
        (void) AmbaUtility_StringToUInt32(argv[2], &Value);
        IdxCopy = Value;
        PrintFunc("changed interface to ");
        CurrentIdx(PrintFunc);
    }
    //enet init 00:11:22:33:44:55
    else if (0 == AmbaUtility_StringCompare(argv[1], "init", AmbaUtility_StringLength("init"))) {
        //NOTE: for auto-start move these to your project main flow
        extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
        extern void EnetUserTaskCreate(UINT32 Idx);
#if defined(ENET_ASIL)
        extern void EnetSafetyTaskCreate(UINT32 Idx);
        EnetSafetyTaskCreate(IdxCopy);
#endif
        (void) AmbaPrint_ModuleSetAllowList(ETH_MODULE_ID, 1U);
        (void) AmbaEnet_SetConfig(IdxCopy, &(EnetConfig[IdxCopy]));
        EnetInit(IdxCopy, argv[2]);
        EnetUserTaskCreate(IdxCopy);
    }
    //enet phyread 2
    else if (0 == AmbaUtility_StringCompare(argv[1], "phyread", AmbaUtility_StringLength("phyread"))) {
        UINT32 Offset = 0U;
        UINT32 Value = 0U;
        char ValueStr[12];

        (void) AmbaUtility_StringToUInt32(argv[2], &Offset);
        (void) AmbaEnet_PhyRead(IdxCopy, 0U, Offset, &Value);
        (void) AmbaUtility_UInt32ToStr(ValueStr, sizeof(ValueStr), Value, 16U);
        PrintFunc("0x");
        PrintFunc(ValueStr);
        PrintFunc("\n");
    }
#ifdef CONFIG_AMBA_UDPIP
    //enet txpause 624
    else if (0 == AmbaUtility_StringCompare(argv[1], "txpause", AmbaUtility_StringLength("txpause"))) {
        (void) AmbaUtility_StringToUInt32(argv[2], &Pause);
    }
    //enet txpri 251
    else if (0 == AmbaUtility_StringCompare(argv[1], "txpri", AmbaUtility_StringLength("txpri"))) {
        UINT32 Value = 0U;
        (void) AmbaUtility_StringToUInt32(argv[2], &Value);
        TestPri[1] = Value;
    }
    //enet ifconfig 192.168.1.100
    else if (0 == AmbaUtility_StringCompare(argv[1], "ifconfig", AmbaUtility_StringLength("ifconfig"))) {
        UINT32 IpAddr = 0U;
        AMBA_DEMO_STACK_CONFIG_s *pDemoConfig;
        (void) AmbaDemoStack_GetConfig(IdxCopy, &pDemoConfig);
        (void) Str2NetOdrIP(argv[2], &IpAddr);
        pDemoConfig->Ip = IpAddr;
    }
    //enet ping 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[1], "ping", AmbaUtility_StringLength("ping"))) {
        UINT32 IpAddr;
        (void) Str2NetOdrIP(argv[2], &IpAddr);
        (void) AmbaDemoStack_Ping(IdxCopy, IpAddr, 10000U);
    }
    //enet demo_udp_tx 192.168.1.2
    else if (0 == AmbaUtility_StringCompare(argv[1], "demo_udp_tx", AmbaUtility_StringLength("demo_udp_tx"))) {
        CmdEnet_demo_udp_tx(argv);
    }
#endif
#ifdef CONFIG_AMBA_AVB
    //enet vlanid 6
    else if (0 == AmbaUtility_StringCompare(argv[1], "vlanid", AmbaUtility_StringLength("vlanid"))) {
        UINT8 vid = (UINT8)(argv[2][0]);
        AMBA_AVB_STACK_CONFIG_s *pAvbConfig;

        (void) AmbaAvbStack_GetConfig(IdxCopy, &pAvbConfig);
        vid -= (UINT8)'0';
        pAvbConfig->PtpConfig.tci &= (UINT16)~0x0FFFU;
        pAvbConfig->PtpConfig.tci |= (UINT16)vid;
#ifdef CONFIG_AMBA_AVTP_REFCODE
        TalkerConfig[0].tci &= ~0x0FFFU;
        TalkerConfig[0].tci |= (UINT16)vid;
#endif
    }
    //enet vlanpri 3
    else if (0 == AmbaUtility_StringCompare(argv[1], "vlanpri", AmbaUtility_StringLength("vlanpri"))) {
        UINT8 pri = (UINT8)(argv[2][0]);
        UINT16 pri16;
        AMBA_AVB_STACK_CONFIG_s *pAvbConfig;

        (void) AmbaAvbStack_GetConfig(IdxCopy, &pAvbConfig);
        pri -= (UINT8)'0';
        pri16 = (UINT16)pri;
        pri16 <<= 13U;
        pAvbConfig->PtpConfig.tci &= (UINT16)~0xE000U;
        pAvbConfig->PtpConfig.tci |= pri16;
#ifdef CONFIG_AMBA_AVTP_REFCODE
        TalkerConfig[0].tci &= ~0xE000U;
        TalkerConfig[0].tci |= (pri16 & 0xE000U);
#endif
    }
    //enet avbdump ptptx
    else if (0 == AmbaUtility_StringCompare(argv[2], "ptptx", AmbaUtility_StringLength("ptptx"))) {
        CmdEnet_ptp(AVB_LOGGING_PTPTX);
    }
    //enet avbdump ptprx
    else if (0 == AmbaUtility_StringCompare(argv[2], "ptprx", AmbaUtility_StringLength("ptprx"))) {
        CmdEnet_ptp(AVB_LOGGING_PTPRX);
    }
    //enet avbdump ptpreport
    else if (0 == AmbaUtility_StringCompare(argv[2], "ptpreport", AmbaUtility_StringLength("ptpreport"))) {
        CmdEnet_ptp(AVB_LOGGING_PTPREPORT);
    }
    //enet phcsetsec 1
    else if (0 == AmbaUtility_StringCompare(argv[1], "phcsetsec", AmbaUtility_StringLength("phcsetsec"))) {
        UINT32 Sec;
        (void) AmbaUtility_StringToUInt32(argv[2], &Sec);
        (void) AmbaEnet_PhcSetTs(IdxCopy, Sec, 0U);
    }
    //enet ppb +199999999
    //enet ppb -199999999
    else if (0 == AmbaUtility_StringCompare(argv[1], "ppb", AmbaUtility_StringLength("ppb"))) {
        CmdEnet_ppb(argv);
    }
#endif // CONFIG_AMBA_AVB
#ifdef CONFIG_LWIP_ENET
    //enet lwip bsd_udp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "bsd_udp_rx", AmbaUtility_StringLength("bsd_udp_rx"))) {
        ForkTest(RXTEST, RXUDP_LWBSD);
    }
    //enet lwip bsd_tcp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "bsd_tcp_rx", AmbaUtility_StringLength("bsd_tcp_rx"))) {
        ForkTest(RXTEST, RXTCP_LWBSD);
    }
#endif
#ifdef CONFIG_NETX_ENET
    //enet netx nx_udp_rx
    //enet netx nxbsd_udp_rx
    //enet netx nx_tcp_rx
    //enet netx nxbsd_tcp_rx
    else if (0 == AmbaUtility_StringCompare(argv[2], "nx", AmbaUtility_StringLength("nx"))) {
        CmdEnet_nxFunc(argv, PrintFunc);
    }
#ifdef CONFIG_NETX_TELNET
    //enet netx telnetd
    else if (0 == AmbaUtility_StringCompare(argv[2], "telnetd", AmbaUtility_StringLength("telnetd"))) {
        TelnetServInit(IdxCopy);
    }
#endif
#endif // CONFIG_NETX_ENET
    else {
        EnetUsage(PrintFunc);
    }
}

static void CmdEnetArgc2(char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    //enet dump
    if (0 == AmbaUtility_StringCompare(argv[1], "dump", AmbaUtility_StringLength("dump"))) {
        extern UINT32 EnetDumpCb(const UINT32 Idx);
        (void) EnetDumpCb(IdxCopy);
    }
    //enet ifup
    else if (0 == AmbaUtility_StringCompare(argv[1], "ifup", AmbaUtility_StringLength("ifup"))) {
        UINT32 Speed;
        if (0U != AmbaEnet_IfUp(IdxCopy, &Speed)) {
            EnetPrint(__func__, __LINE__, ANSI_WHITE, "AmbaEnet_IfUp() failed");
        }
    }
    //enet ifdown
    else if (0 == AmbaUtility_StringCompare(argv[1], "ifdown", AmbaUtility_StringLength("ifdown"))) {
        if (0U != AmbaEnet_IfDown(IdxCopy)) {
            EnetPrint(__func__, __LINE__, ANSI_WHITE, "AmbaEnet_IfDown() failed");
        }
    }
#ifdef CONFIG_LWIP_ENET
    //enet lwipinit
    else if (0 == AmbaUtility_StringCompare(argv[1], "lwipinit", AmbaUtility_StringLength("lwipinit"))) {
        LwIPInit(IdxCopy);
    }
#endif
#ifdef CONFIG_NETX_ENET
    //enet netxinit
    else if (0 == AmbaUtility_StringCompare(argv[1], "netxinit", AmbaUtility_StringLength("netxinit"))) {
        //NOTE: for auto-start move these to your project main flow
        extern NETX_STACK_CONFIG_s NetXConfig[NETX_STACK_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(NETX_MODULE_ID, 1U);
        (void) NetXStack_SetConfig(IdxCopy, &(NetXConfig[IdxCopy]));
        AmbaPrint_PrintStr5("%s", _nx_version_id, NULL, NULL, NULL, NULL);
        NetXInit(IdxCopy);
    }

    //enet netxdump
    else if (0 == AmbaUtility_StringCompare(argv[1], "netxdump", AmbaUtility_StringLength("netxdump"))) {
        NetXDump(IdxCopy);
    }
#endif
#ifdef CONFIG_AMBA_UDPIP
    //enet udpinit
    else if (0 == AmbaUtility_StringCompare(argv[1], "udpinit", AmbaUtility_StringLength("udpinit"))) {
        //NOTE: for auto-start move these to your project main flow
        extern AMBA_DEMO_STACK_CONFIG_s DemoConfig[DEMO_STACK_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(DEMO_MODULE_ID, 1U);
        (void) AmbaDemoStack_SetConfig(IdxCopy, &(DemoConfig[IdxCopy]));
        UdpInit(IdxCopy);
    }
#endif
#ifdef CONFIG_AMBA_AVB
    //enet avbinit
    else if (0 == AmbaUtility_StringCompare(argv[1], "avbinit", AmbaUtility_StringLength("avbinit"))) {
        //NOTE: for auto-start move these to your project main flow
        extern AMBA_AVB_STACK_CONFIG_s AvbConfig[AVB_STACK_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
        (void) AmbaAvbStack_SetConfig(IdxCopy, &(AvbConfig[IdxCopy]));
        AvbInit(IdxCopy, AVB_PTP_AUTO);
    }
    //enet avbmaster
    else if (0 == AmbaUtility_StringCompare(argv[1], "avbmaster", AmbaUtility_StringLength("avbmaster"))) {
        //NOTE: for auto-start move these to your project main flow
        extern AMBA_AVB_STACK_CONFIG_s AvbConfig[AVB_STACK_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
        (void) AmbaAvbStack_SetConfig(IdxCopy, &(AvbConfig[IdxCopy]));
        AvbInit(IdxCopy, AVB_PTP_MASTER);
    }
    //enet avbslave
    else if (0 == AmbaUtility_StringCompare(argv[1], "avbslave", AmbaUtility_StringLength("avbslave"))) {
        //NOTE: for auto-start move these to your project main flow
        extern AMBA_AVB_STACK_CONFIG_s AvbConfig[AVB_STACK_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
        (void) AmbaAvbStack_SetConfig(IdxCopy, &(AvbConfig[IdxCopy]));
        AvbInit(IdxCopy, AVB_PTP_SLAVE);
    }
    //enet phcget
    else if (0 == AmbaUtility_StringCompare(argv[1], "phcget", AmbaUtility_StringLength("phcget"))) {
        UINT32 Sec;
        UINT32 Ns;
        char SecStr[22];
        char NsStr[22];

        (void) AmbaAvbStack_GetPhc(IdxCopy, &Sec, &Ns);
        (void) AmbaUtility_UInt32ToStr(SecStr, sizeof(SecStr), Sec, 10U);
        (void) AmbaUtility_UInt32ToStr(NsStr, sizeof(NsStr), Ns, 10U);
        PrintFunc(SecStr);
        PrintFunc(".");
        PrintFunc(NsStr);
        PrintFunc("\n");
    }
#endif
    else {
        EnetUsage(PrintFunc);
    }
}

void AmbaShell_CommandEnet(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    CurrentIdx(PrintFunc);

    if (argc == 5U) {
        CmdEnetArgc5(argv, PrintFunc);
    } else if (argc == 4U) {
        CmdEnetArgc4(argv, PrintFunc);
    } else if (argc == 3U) {
        CmdEnetArgc3(argv, PrintFunc);
    } else if (argc == 2U) {
        CmdEnetArgc2(argv, PrintFunc);
    }
    else {
        EnetUsage(PrintFunc);
    }
}
