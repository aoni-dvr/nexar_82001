/**
 * @file AmbaENET_Def.h
 * Definitions & Constants
 *
 * @ingroup enet
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 */

#ifndef AMBA_ENET_DEF_H
#define AMBA_ENET_DEF_H

#ifdef CONFIG_THREADX
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#else
#include "ambarella.h"
#endif

////Enable debug function
//#define ETHDEBUG
////Create debug print task for interrupt context
//#define ETHDEBUG_ISR
////Dump packet content
//#define ETHDEBUG_DUMP

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define ENET_INSTANCES 2U
//#define ENET_ASIL
#else
#define ENET_INSTANCES 1U
#endif

#if defined(CONFIG_CPU_CORTEX_A53)
#define ENET0_REGBASE 0xe000e000U
#define ENET1_REGBASE 0xe0032000U
#else
#define ENET0_REGBASE 0x20e000e000UL
#define ENET1_REGBASE 0x20e0022000UL
#endif

#define ANSI_RED      "\033""[1;31m"
#define ANSI_GREEN    "\033""[1;32m"
#define ANSI_YELLOW   "\033""[1;33m"
#define ANSI_BLUE     "\033""[1;34m"
#define ANSI_MAGENTA  "\033""[1;35m"
#define ANSI_CYAN     "\033""[1;36m"
#define ANSI_WHITE    "\033""[1;37m"
#define ANSI_RESET    "\033""[0m"

/* Enet error values */
#define ETH_ERR_NONE            0U
#define ETH_MODULE_ID           ((UINT16)(ETH_ERR_BASE >> 16U))
#define ETH_ERR_EINVAL          (ETH_ERR_BASE + 22U)     /* Invalid argument */
#define ETH_ERR_ENODATA         (ETH_ERR_BASE + 61U)     /* No data available */
#define ETH_ERR_ETIMEDOUT       (ETH_ERR_BASE + 110U)    /* timed out */
#define ETH_ERR_OSERR           (ETH_ERR_BASE + 500U)    /* System resource not available */
#define ETH_ERR_TSERR           (ETH_ERR_BASE + 501U)    /* timestamp error */

#ifdef CONFIG_ETH_FRAMES_SIZE
#define ETH_FRAMES_SIZE         CONFIG_ETH_FRAMES_SIZE
#else
#define ETH_FRAMES_SIZE         1514U           /* Use 3840U for Jumbo Frame */
#endif
#define NS_PER_SECOND           1000000000ULL

typedef struct {
    volatile UINT32 Sec;
    volatile UINT32 Ns;
} AMBA_ENET_TS_s;

typedef struct {
    UINT8 *Buf;
    UINT32 Locked;
} AMBA_ENET_DMA_s;

typedef struct {
    volatile UINT32 TDES0;
    volatile UINT32 TDES1;
    volatile UINT32 TDES2_Buf1;
    volatile UINT32 TDES3_Buf2;
    volatile UINT32 TDES4_Reserved;
    volatile UINT32 TDES5_Reserved;
    volatile UINT32 TDES6_TTSL;
    volatile UINT32 TDES7_TTSH;
    UINT32 PAD[8];
} AMBA_ENET_TDES_s;

typedef struct {
    volatile UINT32 RDES0;
    volatile UINT32 RDES1;
    volatile UINT32 RDES2_Buf1;
    volatile UINT32 RDES3_Buf2;
    volatile UINT32 RDES4;
    volatile UINT32 RDES5_Reserved;
    volatile UINT32 RDES6_RTSL;
    volatile UINT32 RDES7_RTSH;
    UINT32 PAD[8];
} AMBA_ENET_RDES_s;

typedef struct {
    const UINT16        FrameSize;
    const UINT32        TDESCnt;
    const UINT32        RDESCnt;
    AMBA_ENET_TDES_s   *pTDES;
    AMBA_ENET_DMA_s    *pTxDma;
    AMBA_ENET_RDES_s   *pRDES;
    AMBA_ENET_DMA_s    *pRxDma;
} AMBA_ENET_DES_s;

typedef UINT32 (*AMBA_ENET_LINKUP_f)(const UINT32 Idx, UINT32 LinkSpeed);
typedef UINT32 (*AMBA_ENET_LINKDOWN_f)(const UINT32 Idx);
typedef void (*AMBA_ENET_TASKENTRY_f)(const UINT32 Idx);
typedef UINT32 (*const AMBA_ENET_PREINIT_f)(const UINT32 Idx);
typedef UINT32 (*const AMBA_ENET_PHYINIT_f)(const UINT32 PhyId);
typedef UINT32 (*const AMBA_ENET_RX_f)(const UINT32 Idx, const UINT16 RxLen);

typedef struct {
    const UINT32           Idx;
    UINT8                  Mac[6];
    UINT32                 LinkSpeed;
    AMBA_ENET_DES_s *const pDes;
    AMBA_ENET_LINKUP_f     pLinkUp;
    AMBA_ENET_LINKDOWN_f   pLinkDown;
    AMBA_ENET_TASKENTRY_f  pRxTaskEntry;
    AMBA_ENET_TASKENTRY_f  pTxTaskEntry;
    AMBA_ENET_PREINIT_f    pPreInitCb;
    AMBA_ENET_PHYINIT_f    pPhyInitCb;
    AMBA_ENET_RX_f         pRxCb;
} AMBA_ENET_CONFIG_s;

#endif /* AMBA_ENET_DEF_H */
