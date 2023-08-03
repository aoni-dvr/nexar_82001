/**
 * @file AmbaUserEnetConfig.c
 * Enet configuration
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

#include "AmbaENET.h"
#if defined(CONFIG_THREADX)
#if defined(CONFIG_CPU_CORTEX_A53)
#include "AmbaCortexA53.h"
#else
#include "AmbaCortexA76.h"
#endif
#include "AmbaDebugPort.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaPrint.h"
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28)
#include "AmbaGPIO.h"
#endif
#include "NetStack.h"
#endif
#ifdef CONFIG_NETX_ENET
#include "NetXStack.h"
#endif
#ifdef CONFIG_AMBA_UDPIP
#include "DemoStack.h"
#endif
#ifdef CONFIG_AMBA_AVB
#include "AvbStack.h"
#endif

/**
 * user define
 */
#if defined(CONFIG_SOC_CV2)
#define SYS_CONFIG_ETH_ENABLE           0x00000001U
#define ENET_GTX_CLK_POL                0xCU
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_CV22)
#define SYS_CONFIG_ETH_ENABLE           0x00000001U
#define ENET_GTX_CLK_POL                0x60U
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_H22)
#define SYS_CONFIG_ETH_ENABLE           0x00000001U
#define ENET_GTX_CLK_POL                0xCU
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_H32)
#define ENET_GTX_CLK_POL                0x60U
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_CV25)
#define SYS_CONFIG_ETH_ENABLE           0x00000001U
#define ENET_GTX_CLK_POL                0x60U
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define ENET_GTX_CLK_POL                0x60U
#define ENET_GTX_CLK_VAL                0x90000000U
#elif defined(CONFIG_SOC_CV28)
#define ENET_GTX_CLK_POL                0x60U
#define ENET_GTX_CLK_VAL                0x80000000U
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define ENET_GTX_CLK_POL                0x60UL
#define ENET_GTX_CLK_VAL                0x90000000U
#else
#error CONFIG_SOC_XXX
#endif

#if defined(CONFIG_THREADX)
#if defined(CONFIG_CPU_V7_CA9)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A9_SCRATCHPAD_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_AHB_SCRATCHPAD_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_AHB_SCRATCHPAD_BASE_ADDR
#elif defined(AMBA_CORTEX_A53_SCRATCHPAD_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A53_SCRATCHPAD_BASE_ADDR
#elif defined(AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR
#elif defined(AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR)
#define AHB_SCRATCHPAD_BASE             AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR
#else
#error CONFIG_SOC_XXX
#endif
#define RCT_BASE                        AMBA_DBG_PORT_RCT_BASE_ADDR
#endif

#define EnetTaskStackSize    (0x4000U)

#define BOARD_ETH_FRAMES_SIZE   (((ETH_FRAMES_SIZE+CACHE_LINE_SIZE)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE)
#if !defined(BOARD_ETH_TX_FRAMES)
#define BOARD_ETH_TX_FRAMES     32U    //min 4
#endif
#if !defined(BOARD_ETH_RX_FRAMES)
#define BOARD_ETH_RX_FRAMES     64U    //min 8
#endif

#ifdef CONFIG_NETX_ENET
#define PKT_POOL_PYLD_SIZE  (BOARD_ETH_FRAMES_SIZE + (UINT32)NX_PHYSICAL_HEADER)
#define PKT_POOL_NODE_SIZE  (PKT_POOL_PYLD_SIZE + sizeof(NX_PACKET))
#define PKT_NUM             (96U * (UINT32)ENET_INSTANCES)
#define PKT_POOL_SIZE       ((((PKT_POOL_NODE_SIZE * PKT_NUM) / sizeof(UINT32)) + 1U) * sizeof(UINT32))
#define IP_MEM_SIZE         (10U * 1024U)
#define ARP_MEM_SIZE        (1U * 1024U)
#define BSD_MEM_SIZE        (4U * 1024U)

static UINT8                nx_pkt_pool[PKT_POOL_SIZE] __attribute__((section(".bss.noinit")));
static UINT8                nx_ip_mem[IP_MEM_SIZE] __attribute__((section(".bss.noinit")));
static UINT8                nx_arp_mem[ARP_MEM_SIZE] __attribute__((section(".bss.noinit")));
static UINT8                nx_bsd_mem[BSD_MEM_SIZE] __attribute__((section(".bss.noinit")));

static NX_PACKET_POOL       NxPktPool;
static NX_IP                NxIp;
#endif

/* user struct */
#if defined(CONFIG_THREADX)
typedef struct
{
    UINT8 Buf[BOARD_ETH_FRAMES_SIZE];
} USER_ENET_BUF_s __attribute__((aligned(CACHE_LINE_SIZE)));

typedef struct
{
    AMBA_ENET_DMA_s Tx[BOARD_ETH_TX_FRAMES];
    AMBA_ENET_DMA_s Rx[BOARD_ETH_RX_FRAMES];
} USER_ENET_DMA_s;

typedef struct
{
    AMBA_ENET_TDES_s TDES[BOARD_ETH_TX_FRAMES];
    AMBA_ENET_RDES_s RDES[BOARD_ETH_RX_FRAMES];
    USER_ENET_BUF_s  Tx[BOARD_ETH_TX_FRAMES];
    USER_ENET_BUF_s  Rx[BOARD_ETH_RX_FRAMES];
} USER_ENET_DES_s __attribute__((aligned(CACHE_LINE_SIZE)));

static UINT32 UserRxCb(const UINT32 Idx, const UINT16 RxLen);
#endif //defined(CONFIG_THREADX)

static UINT32 UserPreInitCb(const UINT32 Idx);
static UINT32 UserPhyInitCb0(const UINT32 PhyId);
#if (ENET_INSTANCES >= 2U)
static UINT32 UserPhyInitCb1(const UINT32 PhyId);
#endif

static AMBA_ENET_DES_s EnetDes[ENET_INSTANCES] = {
    [0] = {
        .FrameSize = (UINT16)BOARD_ETH_FRAMES_SIZE,
        .TDESCnt = BOARD_ETH_TX_FRAMES,
        .RDESCnt = BOARD_ETH_RX_FRAMES,
        .pTDES = NULL,
        .pTxDma = NULL,
        .pRDES = NULL,
        .pRxDma = NULL,
    },
#if (ENET_INSTANCES >= 2U)
    [1] = {
        .FrameSize = (UINT16)BOARD_ETH_FRAMES_SIZE,
        .TDESCnt = BOARD_ETH_TX_FRAMES,
        .RDESCnt = BOARD_ETH_RX_FRAMES,
        .pTDES = NULL,
        .pTxDma = NULL,
        .pRDES = NULL,
        .pRxDma = NULL,
    },
#endif
};

/**
 * user config instance
 */
extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES] = {
    [0] = {
        .Idx        = 0,
        .pDes       = &EnetDes[0],
        .pPhyInitCb = UserPhyInitCb0,
        .pPreInitCb = UserPreInitCb,
#if defined(CONFIG_THREADX)
        .pRxCb       = UserRxCb,
#endif
    },
#if (ENET_INSTANCES >= 2U)
    [1] = {
        .Idx        = 1,
        .pDes       = &EnetDes[1],
        .pPhyInitCb = UserPhyInitCb1,
        .pPreInitCb = UserPreInitCb,
#if defined(CONFIG_THREADX)
        .pRxCb       = UserRxCb,
#endif
    },
#endif
};

#ifdef CONFIG_AMBA_UDPIP
extern AMBA_DEMO_STACK_CONFIG_s DemoConfig[DEMO_STACK_INSTANCES];
AMBA_DEMO_STACK_CONFIG_s DemoConfig[DEMO_STACK_INSTANCES];
#endif

/*
 * task table
     81    [NetX] NetX IP Instance
     83    [NetX] BSD thread task
     90    [ENET] AvbPtP0Task
     92    [ENET] Enet0RxTask
     93    [ENET] AvbPtP1Task
     95    [ENET] Enet1RxTask
 */
#ifdef CONFIG_AMBA_AVB
extern AMBA_AVB_STACK_CONFIG_s AvbConfig[AVB_STACK_INSTANCES];
AMBA_AVB_STACK_CONFIG_s AvbConfig[AVB_STACK_INSTANCES] = {
    [0] = {
        .AvbLoggingCtrl = 0U,
        .PtpConfig = {
            .TaskPri    = 90U,
            .SmpCoreSet = 1U,
            .tci        = 0U,
        },
    },
#if (AVB_STACK_INSTANCES >= 2U)
    [1] = {
        .AvbLoggingCtrl = 0U,
        .PtpConfig = {
            .TaskPri    = 93U,
            .SmpCoreSet = 1U,
            .tci        = 0U,
        },
    },
#endif
};
#endif

#ifdef CONFIG_NETX_ENET
extern NETX_STACK_CONFIG_s NetXConfig[NETX_STACK_INSTANCES];
NETX_STACK_CONFIG_s NetXConfig[NETX_STACK_INSTANCES] = {
    [0] = {
        .NxIPPri    = 81,
        .NxBSDPri   = 83,
        .PyldSize   = PKT_POOL_PYLD_SIZE,
        .ArpMemSize = ARP_MEM_SIZE,
        .IpMemSize  = IP_MEM_SIZE,
        .BsdMemSize = BSD_MEM_SIZE,
        .PoolSize   = PKT_POOL_SIZE,
        .pArpMem    = &nx_arp_mem[0],
        .pIpMem     = &nx_ip_mem[0],
        .pPool      = &nx_pkt_pool[0],
        .pBSDMem    = &nx_bsd_mem[0],
        .pNxPktPool = &NxPktPool,
        .pNxIp      = &NxIp,
    },
#if (NETX_STACK_INSTANCES >= 2U)
    /* netx only init once */
    [1] = {
        .NxIPPri    = 81,
        .NxBSDPri   = 83,
        .PyldSize   = PKT_POOL_PYLD_SIZE,
        .ArpMemSize = ARP_MEM_SIZE,
        .IpMemSize  = IP_MEM_SIZE,
        .BsdMemSize = BSD_MEM_SIZE,
        .PoolSize   = PKT_POOL_SIZE,
        .pArpMem    = &nx_arp_mem[0],
        .pIpMem     = &nx_ip_mem[0],
        .pPool      = &nx_pkt_pool[0],
        .pBSDMem    = &nx_bsd_mem[0],
        .pNxPktPool = &NxPktPool,
        .pNxIp      = &NxIp,
    },
#endif
};
#endif

#if defined(CONFIG_THREADX)
static UINT16 enet_ntohs(UINT16 n)
{
    return ((n & 0xffU) << 8) | ((n & 0xff00U) >> 8);
}

/**
 * user call back func
 */
static UINT32 UserRxCb(const UINT32 Idx, const UINT16 RxLen)
{
#ifdef CONFIG_NETX_ENET
    const NETX_STACK_CONFIG_s *pNetXConfig = &(NetXConfig[Idx]);
#endif
#ifdef CONFIG_AMBA_UDPIP
    const AMBA_DEMO_STACK_CONFIG_s *pDemoConfig = &(DemoConfig[Idx]);
#endif
#ifdef CONFIG_AMBA_AVB
    const AMBA_AVB_STACK_CONFIG_s *pAvbConfig = &(AvbConfig[Idx]);
#endif
#ifdef CONFIG_LWIP_ENET
    extern UINT32 ethernetif_input(const UINT32 Idx, const void *frame, const UINT16 RxLen, const UINT16 type);
#endif

    void *ptr = NULL;
    UINT16 Len = RxLen;
    UINT16 type;
    const ETH_HDR_s *ethhdr;
    const ETH_VLAN_HDR_s *vlanhdr;
    UINT32 Stack = 0U;

    if (Idx >= ENET_INSTANCES) {
        AmbaPrint_PrintStr5("%s Idx over range!", __func__, NULL, NULL, NULL, NULL);
    }
#if defined(CONFIG_AMBA_AVB)
    else if (pAvbConfig->pGetRxBufCb != NULL) {
        (void)pAvbConfig->pGetRxBufCb(Idx, &ptr);
        Stack |= 0x01U;
    }
#endif
#ifdef CONFIG_NETX_ENET
    else if (pNetXConfig->pGetRxBufCb != NULL) {
        (void)pNetXConfig->pGetRxBufCb(Idx, &ptr);
        Stack |= 0x02U;
    }
#endif
#if defined(CONFIG_AMBA_UDPIP)
    else if (pDemoConfig->pGetRxBufCb != NULL) {
        (void)pDemoConfig->pGetRxBufCb(Idx, &ptr);
        Stack |= 0x04U;
    }
#endif
    else {
#if defined(CONFIG_LWIP_ENET)
        (void) AmbaEnet_GetRxBuf(Idx, &ptr);
        Stack |= 0x08U;
#endif
        // In the else statement, do nothing when CONFIG_LWIP_ENET = n
        ;
    }
    if (Stack > 0U)
    {
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ethhdr, &ptr);
#else
        ethhdr = (const ETH_HDR_s *)ptr;
#endif
        type = enet_ntohs(ethhdr->type);
        if (type == ETH_VLAN_TYPE) {
#if defined(AMBA_MISRA_FIX_H)
            AmbaMisra_TypeCast(&vlanhdr, &ptr);
#else
            vlanhdr = (const ETH_VLAN_HDR_s *)ptr;
#endif
            type = enet_ntohs(vlanhdr->type);
        }

        /* Strip the 4 byte CRC */
        Len -= 4U;
#ifdef CONFIG_AMBA_AVB
        if ((type == ETH_PTP2_TYPE) || (type == ETH_AVTP_TYPE)) {
            if (pAvbConfig->pRxCb != NULL) {
                (void) pAvbConfig->pRxCb(Idx, ethhdr, Len, type);
            }
        }
#endif
#ifdef CONFIG_NETX_ENET
        if ((type == ETH_ARP_TYPE) || (type == ETH_IP_TYPE)) {
            if (pNetXConfig->pRxCb != NULL) {
                (void) pNetXConfig->pRxCb(Idx, ethhdr, Len, type);
            }
        }
#endif
#ifdef CONFIG_AMBA_UDPIP
        if ((type == ETH_ARP_TYPE) || (type == ETH_IP_TYPE)) {
            if (pDemoConfig->pRxCb != NULL) {
                (void) pDemoConfig->pRxCb(Idx, ethhdr, Len, type);
            }
        }
#endif
#ifdef CONFIG_LWIP_ENET
        if ((type == ETH_ARP_TYPE) || (type == ETH_IP_TYPE)) {
            (void) ethernetif_input(Idx, ethhdr, Len, type);
        }
#endif
    }

    return 0;
}
#endif

static UINT32 UserPreInitCb(const UINT32 Idx)
{
    UINT32 Ret = 0;
    const ULONG SysCfgAddr = (const ULONG)(RCT_BASE + 0x34U);
    const UINT32 *pSysCfg;
#if defined(CONFIG_THREADX)
    UINT32 i;
    const AMBA_ENET_DES_s *pDes = &EnetDes[Idx];
    static USER_ENET_DES_s UserEnetDes[ENET_INSTANCES] __attribute__((section(".bss.noinit")));
    static USER_ENET_DMA_s UserEnetDma[ENET_INSTANCES] __attribute__((section(".bss.noinit")));
#endif
#ifdef ENET_GTX_CLK_POL
    const ULONG AhbSpCtrlAddr = (const ULONG)(AHB_SCRATCHPAD_BASE + ENET_GTX_CLK_POL);
    UINT32 *pAhbSpCtrl;
    UINT32 Reg;

    /*pAhbSpCtrl = (UINT32 *)AhbSpCtrlAddr;*/
    pAhbSpCtrl = NULL;
    if (AmbaWrap_memcpy(&pAhbSpCtrl, &AhbSpCtrlAddr, sizeof(AhbSpCtrlAddr))!= 0U) { }

    Reg = *pAhbSpCtrl | ENET_GTX_CLK_VAL;
#endif
#if defined(CONFIG_SOC_CV25)
    AmbaPrint_PrintStr5("%s rgmii GPIO 28~43 alt4", __func__, NULL, NULL, NULL, NULL);
    (void)AmbaGPIO_SetFuncAlt(0x401c);
    (void)AmbaGPIO_SetFuncAlt(0x401d);
    (void)AmbaGPIO_SetFuncAlt(0x401e);
    (void)AmbaGPIO_SetFuncAlt(0x401f);
    (void)AmbaGPIO_SetFuncAlt(0x4020);
    (void)AmbaGPIO_SetFuncAlt(0x4021);
    (void)AmbaGPIO_SetFuncAlt(0x4022);
    (void)AmbaGPIO_SetFuncAlt(0x4023);
    (void)AmbaGPIO_SetFuncAlt(0x4024);
    (void)AmbaGPIO_SetFuncAlt(0x4025);
    (void)AmbaGPIO_SetFuncAlt(0x4026);
    (void)AmbaGPIO_SetFuncAlt(0x4027);
    (void)AmbaGPIO_SetFuncAlt(0x4028);
    (void)AmbaGPIO_SetFuncAlt(0x4029);
    (void)AmbaGPIO_SetFuncAlt(0x402a);
    (void)AmbaGPIO_SetFuncAlt(0x402b);
#elif defined(CONFIG_SOC_H32)
    AmbaPrint_PrintStr5("%s rgmii GPIO 19~34 alt4", __func__, NULL, NULL, NULL, NULL);
    (void)AmbaGPIO_SetFuncAlt(0x4013);
    (void)AmbaGPIO_SetFuncAlt(0x4014);
    (void)AmbaGPIO_SetFuncAlt(0x4015);
    (void)AmbaGPIO_SetFuncAlt(0x4016);
    (void)AmbaGPIO_SetFuncAlt(0x4017);
    (void)AmbaGPIO_SetFuncAlt(0x4018);
    (void)AmbaGPIO_SetFuncAlt(0x4019);
    (void)AmbaGPIO_SetFuncAlt(0x401a);
    (void)AmbaGPIO_SetFuncAlt(0x401b);
    (void)AmbaGPIO_SetFuncAlt(0x401c);
    (void)AmbaGPIO_SetFuncAlt(0x401d);
    (void)AmbaGPIO_SetFuncAlt(0x401e);
    (void)AmbaGPIO_SetFuncAlt(0x401f);
    (void)AmbaGPIO_SetFuncAlt(0x4020);
    (void)AmbaGPIO_SetFuncAlt(0x4021);
    (void)AmbaGPIO_SetFuncAlt(0x4022);
#elif defined(CONFIG_SOC_CV28)
    AmbaPrint_PrintStr5("%s rgmii GPIO 19~34 alt4", __func__, NULL, NULL, NULL, NULL);
    (void)AmbaGPIO_SetFuncAlt(0x4013);
    (void)AmbaGPIO_SetFuncAlt(0x4014);
    (void)AmbaGPIO_SetFuncAlt(0x4015);
    (void)AmbaGPIO_SetFuncAlt(0x4016);
    (void)AmbaGPIO_SetFuncAlt(0x4017);
    (void)AmbaGPIO_SetFuncAlt(0x4018);
    (void)AmbaGPIO_SetFuncAlt(0x4019);
    (void)AmbaGPIO_SetFuncAlt(0x401a);
    (void)AmbaGPIO_SetFuncAlt(0x401b);
    (void)AmbaGPIO_SetFuncAlt(0x401c);
    (void)AmbaGPIO_SetFuncAlt(0x401d);
    (void)AmbaGPIO_SetFuncAlt(0x401e);
    (void)AmbaGPIO_SetFuncAlt(0x401f);
    (void)AmbaGPIO_SetFuncAlt(0x4020);
    (void)AmbaGPIO_SetFuncAlt(0x4021);
    (void)AmbaGPIO_SetFuncAlt(0x4022);
#endif

#if defined(CONFIG_THREADX)
    if (Idx < ENET_INSTANCES)
    {
        USER_ENET_DES_s *const pUDes = &UserEnetDes[Idx];
        USER_ENET_DMA_s *const pUDma = &UserEnetDma[Idx];
        ULONG Addr;

        if (AmbaWrap_memset(pUDes, 0, sizeof(USER_ENET_DES_s))!= 0U) { }
        if (AmbaWrap_memset(pUDma, 0, sizeof(USER_ENET_DMA_s))!= 0U) { }

        /* assign dma buffer */
        for (i = 0; i < pDes->TDESCnt; i++) {
            UserEnetDma[Idx].Tx[i].Buf = UserEnetDes[Idx].Tx[i].Buf;
        }
        for (i = 0; i < pDes->RDESCnt; i++) {
            UserEnetDma[Idx].Rx[i].Buf = UserEnetDes[Idx].Rx[i].Buf;
        }

        EnetDes[Idx].pTDES = &pUDes->TDES[0];
        EnetDes[Idx].pTxDma = &pUDma->Tx[0];
        EnetDes[Idx].pRDES = &pUDes->RDES[0];
        EnetDes[Idx].pRxDma = &pUDma->Rx[0];

        /* Arm EL1 cache always clean before invalidate
         * so we need to drop rx dma buffer cache here
         * else we get empty rx pkt after cache invalidate
         */
        /*Addr = (ULONG)pUDes*/
        AmbaMisra_TypeCast(&Addr, &pUDes);
        (void) AmbaCache_DataClean(Addr, sizeof(USER_ENET_DES_s));
        (void) AmbaCache_DataInvalidate(Addr, sizeof(USER_ENET_DES_s));
        /*Addr = (ULONG)pUDma*/
        AmbaMisra_TypeCast(&Addr, &pUDma);
        (void) AmbaCache_DataClean(Addr, sizeof(USER_ENET_DMA_s));
        (void) AmbaCache_DataInvalidate(Addr, sizeof(USER_ENET_DMA_s));
    } else {
        AmbaPrint_PrintStr5("%s %s Idx > ENET_INSTANCES %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
    }
#endif  //defined(CONFIG_THREADX)

#if defined(__QNXNTO__)
    {
        void amba_setup_dma_desc(UINT32 Idx);
        amba_setup_dma_desc(Idx);
    }
#endif //defined(__QNXNTO__)

#if 0
    AmbaGPIO_ConfigOutput(PHY_RESET_GPIO, AMBA_GPIO_LEVEL_LOW);
    (void) AmbaKAL_TaskSleep(1);
    AmbaGPIO_ConfigOutput(PHY_RESET_GPIO, AMBA_GPIO_LEVEL_HIGH);
#endif

    /*pSysCfg = (const UINT32 *)SysCfgAddr;*/
    pSysCfg = NULL;
    if (AmbaWrap_memcpy(&pSysCfg, &SysCfgAddr, sizeof(SysCfgAddr))!= 0U) { }

#ifdef SYS_CONFIG_ETH_ENABLE
    if ((Idx == 0U) &&
        ((*pSysCfg & SYS_CONFIG_ETH_ENABLE) == 0U)) {
        AmbaPrint_PrintStr5("%s %s Eth0 isn't enabled by power on config! %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        Ret = 0x1U;
    }
#endif

#ifdef ENET_GTX_CLK_POL
    AmbaPrint_PrintStr5("%s %s invert GTX clk %s", ANSI_WHITE, __func__, ANSI_RESET, NULL, NULL);
    *pAhbSpCtrl = Reg;
#endif

    return Ret;
}

static UINT32 UserPhyCb(UINT32 Idx, const UINT32 PhyId)
{
    /* VSC8531 0x7057x */
    if (0x70570U == ((PhyId) & ~(0xfU))) {
        UINT32 Value;
        /*Set R0 bit12 = 1 to enable Auto-negotiation*/
        (void)AmbaEnet_PhyRead(Idx, 0U, 0U, &Value);
        if ((Value & 0x1000U) != 0x1000U) {
            Value |= 0x1000U;
            (void)AmbaEnet_PhyWrite(Idx, 0U, 0, Value);
        }
        /*Select Extend 2 Registers*/
        (void)AmbaEnet_PhyWrite(Idx, 0U, 0x1f, 0x2);
        /*RGMII Control, RX_CLK delay = 2.0 ns delay*/
        (void)AmbaEnet_PhyWrite(Idx, 0U, 0x14, 0x40);
        /*Select main register space*/
        (void)AmbaEnet_PhyWrite(Idx, 0U, 0x1f, 0x0);
    }
#if defined(CONFIG_BSP_CV5DK_OPTION_A_V100) || defined(CONFIG_BSP_CV52DK_OPTION_A_V100)
    /*
     * CV5XDK GTX long trace already delayed 2ns in board level
     */
    /* ADIN1300 0x283BC30 */
    if (0x283BC30U == PhyId) {
        UINT32 Value;

        /* 0x10 Extend Reg Point to 0xff23 RGMII Configuration Register */
        (void)AmbaEnet_PhyWrite(Idx, 0U, 0x10U, 0xff23U);
        /* 0x11 Extend Reg Data now stands for 0xff23 RGMII Configuration Register*/
        (void)AmbaEnet_PhyRead(Idx, 0U, 0x11U, &Value);
        /* old DK did not set POC to disable 2ns delay */
        if ((Value & 0x2U) == 0x2U) {
            /* clear b[2] to disable receive clock internal 2 ns delay */
            Value &= ~0x2U;
            (void)AmbaEnet_PhyWrite(Idx, 0U, 0x11U, Value);
        }
        /* Clear 0x10 Extend Reg Pointer */
        (void)AmbaEnet_PhyWrite(Idx, 0U, 0x10U, 0x0U);
    }
#endif

    return 0U;
}

static UINT32 UserPhyInitCb0(const UINT32 PhyId)
{
    return UserPhyCb(0U, PhyId);
}

#if (ENET_INSTANCES >= 2U)
static UINT32 UserPhyInitCb1(const UINT32 PhyId)
{
    return UserPhyCb(1U, PhyId);
}
#endif

#if defined(CONFIG_THREADX)
/**
 * user task create
 */
#define ENET0_RXPRI 92U
#define ENET0_TXPRI 90U
#if (ENET_INSTANCES >= 2U)
#define ENET1_RXPRI 95U
#define ENET1_TXPRI 93U
#endif
extern void EnetUserTaskCreate(UINT32 Idx);
void EnetUserTaskCreate(UINT32 Idx)
{
    static char EnetRxTaskName[2][32] = { "Enet0RxTask", "Enet1RxTask" };
    static char EnetTxTaskName[2][32] = { "Enet0TxTask", "Enet1TxTask" };
    static UINT8 EnetRxTaskStack[ENET_INSTANCES][EnetTaskStackSize] __attribute__((section(".bss.noinit")));
    static UINT8 EnetTxTaskStack[ENET_INSTANCES][EnetTaskStackSize] __attribute__((section(".bss.noinit")));
    const AMBA_ENET_CONFIG_s *pEnetConfig = &(EnetConfig[Idx]);
    UINT32 err;
    static AMBA_KAL_TASK_t EnetRxTask[ENET_INSTANCES] = {
        [0] = {
            .tx_thread_id = 0U,
        },
#if (ENET_INSTANCES >= 2U)
        [1] = {
            .tx_thread_id = 0U,
        },
#endif
    };
    static AMBA_KAL_TASK_t EnetTxTask[ENET_INSTANCES] = {
        [0] = {
            .tx_thread_id = 0U,
        },
#if (ENET_INSTANCES >= 2U)
        [1] = {
            .tx_thread_id = 0U,
        },
#endif
    };

    if ((Idx < ENET_INSTANCES) && (EnetRxTask[Idx].tx_thread_id == 0U)) {
        AMBA_KAL_TASK_ENTRY_f pTaskEntry;
        void *arg = NULL;
        const UINT8 *pBuf;
        void *ptr;
        UINT32 pri;

#if (ENET_INSTANCES >= 2U)
        if (Idx == 0U) {
            pri = ENET0_RXPRI;
        } else {
            pri = ENET1_RXPRI;
        }
#else
        pri = ENET0_RXPRI;
#endif
        if (AmbaWrap_memset(&EnetRxTaskStack[Idx][0], 0, EnetTaskStackSize)!= 0U) { }
        pBuf = &EnetRxTaskStack[Idx][0];
        AmbaMisra_TypeCast(&ptr, &pBuf);

        /*arg = (void *)TestItem;*/
        (void)arg;
        if (AmbaWrap_memcpy(&arg, &Idx, sizeof(Idx))!= 0U) { }

        AmbaMisra_TypeCast(&pTaskEntry, &pEnetConfig->pRxTaskEntry);
        err = AmbaKAL_TaskCreate(&EnetRxTask[Idx],
                                 EnetRxTaskName[Idx],
                                 pri,
                                 pTaskEntry,
                                 arg,
                                 ptr,
                                 EnetTaskStackSize,
                                 0U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskCreate failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&EnetRxTask[Idx], 1U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskSetSmpAffinity failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
        err = (UINT32)AmbaKAL_TaskResume(&EnetRxTask[Idx]);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskResume failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
    }

    if ((Idx < ENET_INSTANCES) && (EnetTxTask[Idx].tx_thread_id == 0U)) {
        AMBA_KAL_TASK_ENTRY_f pTaskEntry;
        void *arg = NULL;
        const UINT8 *pBuf;
        void *ptr;
        UINT32 pri;
#if (ENET_INSTANCES >= 2U)
        if (Idx == 0U) {
            pri = ENET0_TXPRI;
        } else {
            pri = ENET1_TXPRI;
        }
#else
        pri = ENET0_TXPRI;
#endif
        if (AmbaWrap_memset(&EnetTxTaskStack[Idx][0], 0, EnetTaskStackSize)!= 0U) { }
        pBuf = &EnetTxTaskStack[Idx][0];
        AmbaMisra_TypeCast(&ptr, &pBuf);

        /*arg = (void *)TestItem;*/
        (void)arg;
        if (AmbaWrap_memcpy(&arg, &Idx, sizeof(Idx))!= 0U) { }

        AmbaMisra_TypeCast(&pTaskEntry, &pEnetConfig->pTxTaskEntry);
        err = AmbaKAL_TaskCreate(&EnetTxTask[Idx],
                                 EnetTxTaskName[Idx],
                                 pri,
                                 pTaskEntry,
                                 arg,
                                 ptr,
                                 EnetTaskStackSize,
                                 0U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskCreate failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&EnetTxTask[Idx], 1U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskSetSmpAffinity failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
        err = (UINT32)AmbaKAL_TaskResume(&EnetTxTask[Idx]);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskResume failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
    }
}

#if defined(ENET_ASIL)
#define ENET0_SAFPRI 91U
#define ENET1_SAFPRI 94U
extern void EnetSafetyTaskCreate(UINT32 Idx);
void EnetSafetyTaskCreate(UINT32 Idx)
{
    extern void *EnetSafetyTaskEntry(void *arg);
    static char taskname[2][32] = { "EnetSafety0", "EnetSafety1" };
    UINT32 err;
    static UINT8 EnetTaskStack[ENET_INSTANCES][EnetTaskStackSize] __attribute__((section(".bss.noinit")));
    static AMBA_KAL_TASK_t EnetTask[ENET_INSTANCES] = {
        [0] = {
            .tx_thread_id = 0U,
        },
#if (ENET_INSTANCES >= 2U)
        [1] = {
            .tx_thread_id = 0U,
        },
#endif
    };

    if (EnetTask[Idx].tx_thread_id == 0U) {
        void *arg = NULL;
        const UINT8 *pBuf;
        void *ptr;
        UINT32 pri;
        if (Idx == 0U) {
            pri = ENET0_SAFPRI;
        } else {
            pri = ENET1_SAFPRI;
        }
        if (AmbaWrap_memset(&EnetTaskStack[Idx][0], 0, EnetTaskStackSize)!= 0U) { }
        pBuf = &EnetTaskStack[Idx][0];
        AmbaMisra_TypeCast(&ptr, &pBuf);

        /*arg = (void *)TestItem;*/
        (void)arg;
        if (AmbaWrap_memcpy(&arg, &Idx, sizeof(Idx))!= 0U) { }

        err = AmbaKAL_TaskCreate(&EnetTask[Idx],
                                 taskname[Idx],
                                 pri,
                                 EnetSafetyTaskEntry,
                                 arg,
                                 ptr,
                                 EnetTaskStackSize,
                                 0U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskCreate failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }

        // Using Core-0 (0x01)
        err = AmbaKAL_TaskSetSmpAffinity(&EnetTask[Idx], 1U);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskSetSmpAffinity failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
        err = (UINT32)AmbaKAL_TaskResume(&EnetTask[Idx]);
        if (err != 0U) {
            AmbaPrint_PrintStr5("%s %s AmbaKAL_TaskResume failed %s", ANSI_RED, __func__, ANSI_RESET, NULL, NULL);
        }
    }
}
#endif
#endif
