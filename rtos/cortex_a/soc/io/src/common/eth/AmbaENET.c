/**
 * @file AmbaENET.c
 *  Enet driver
 *
 * @defgroup enet title
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
 */

#if defined(CONFIG_THREADX)
#include "AmbaINT_Def.h"
#if defined(CONFIG_ENABLE_VIRTUAL_ADDRESS)
#include "AmbaMMU.h"
#endif
#include "AmbaIOUtility.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"  // for misra depress
//#include "libfdt.h"   // native
#endif
#endif

#include "AmbaENET.h"
#include "AmbaRTSL_ENET.h"

#if defined(ENET_ASIL)
#include "AmbaSafety_ENET.h"
#endif

#if defined(AMBA_KAL_H)
static AMBA_KAL_SEMAPHORE_t EnetRxSem[ENET_INSTANCES];
static AMBA_KAL_SEMAPHORE_t EnetTxSem[ENET_INSTANCES];
static AMBA_KAL_SEMAPHORE_t EnetTxCtrlSem[ENET_INSTANCES];
static AMBA_KAL_MUTEX_t EnetTxMutex[ENET_INSTANCES];
#endif

#if defined(__QNXNTO__)
//static sem_t EnetRxSem[ENET_INSTANCES];
#if defined(QNX_ETH_TSK)
static sem_t EnetTxSem[ENET_INSTANCES];
#endif
static sem_t EnetTxCtrlSem[ENET_INSTANCES];
static pthread_mutex_t EnetTxMutex[ENET_INSTANCES];
extern struct cache_ctrl enet_cache;
#endif

static UINT32 EnetTxCur[ENET_INSTANCES];
static UINT32 EnetRxCur[ENET_INSTANCES];
static UINT32 EnetInited[ENET_INSTANCES];

static AMBA_ENET_CONFIG_s *pAmbaEnetConfig[] = {
    NULL,
#if (ENET_INSTANCES >= 2U)
    NULL,
#endif
};

#define DMB __asm__ __volatile__ ("dmb sy")

//#undef CONFIG_DEVICE_TREE_SUPPORT
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
/* Shared Peripheral Interrupt (SPI) */
#define AMBA_INT_SPI_BASE 32U

UINT32 EnetIrq[ENET_INSTANCES];
ULONG EnetReg[ENET_INSTANCES];

static INT32 EnetFetchDTB(const UINT32 dtb_addr_arg)
{
    const void *fdt;
    INT32 ret, i, eth_offset, len;
    static char eth[2][16] = {"ethernet0", "ethernet1"};
    const char *chr1;
    const UINT32 *ptr1;
    const struct fdt_property *prop = NULL;

    fdt = NULL;
    if (AmbaWrap_memcpy(&fdt, &dtb_addr_arg, sizeof(dtb_addr_arg))!= 0U) { }

    ret = AmbaFDT_CheckHeader(fdt);
    for (i = 0; i < (INT32)ENET_INSTANCES; i++) {
        if (ret != 0) {
            break;
        }
        /*ethernet0 = "/ahb@e0000000/ethernet@e000e000";*/
        eth_offset = AmbaFDT_PathOffset(fdt, eth[i]);
        EnetDebug("ENET %s(%d) %s 0x%x", __func__, __LINE__, eth[i], eth_offset);
        if (eth_offset < 0) {
            ret = -1;
        }

        /*reg = <0xe000e000 0x00002000>;*/
        if (ret == 0) {
            prop = AmbaFDT_GetProperty(fdt, eth_offset, "reg", &len);
            if ((prop != NULL) && (len > 0)) {
                chr1 = (const char *)&prop->data[0];
#if defined(AMBA_MISRA_FIX_H)
                AmbaMisra_TypeCast(&ptr1, &chr1);
#else
                ptr1 = (const UINT32 *)chr1;
#endif
                EnetReg[i] = (ULONG)AmbaFDT_Fdt32ToCpu(ptr1[0]);
                EnetDebug("ENET %s(%d) reg 0x%lx", __func__, __LINE__, EnetReg[i]);
            } else {
                ret = -1;
            }
        }

#if defined(CONFIG_THREADX64)
        /* ahb@20e0000000 */
        if (ret == 0) {
            INT32 ahb_offset;
            ahb_offset = AmbaFDT_ParentOffset(fdt, eth_offset);
            if (ahb_offset < 0) {
                ret = -1;
            }

            /*(ahb) reg = <0x20 0xe0000000 0x0 0x1000000>;*/
            if (ret == 0) {
                ULONG high_word;
                prop = AmbaFDT_GetProperty(fdt, ahb_offset, "reg", &len);
                if ((prop != NULL) && (len > 8)) {
                    chr1 = (const char *)&prop->data[0];
#if defined(AMBA_MISRA_FIX_H)
                    AmbaMisra_TypeCast(&ptr1, &chr1);
#else
                    ptr1 = (const UINT32 *)chr1;
#endif
                    high_word = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                    high_word = high_word << 32U;
                    EnetReg[i] |= high_word;
                    EnetDebug("ENET %s(%d) reg 0x%lx", __func__, __LINE__, EnetReg[i]);
                }
            }
        }
#endif
        /*interrupts = <0x00000000 0x00000040 0x00000004>;*/
        if (ret == 0) {
            prop = AmbaFDT_GetProperty(fdt, eth_offset, "interrupts", &len);
            if ((prop != NULL) && (len > 4)) {
                chr1 = (const char *)&prop->data[0];
#if defined(AMBA_MISRA_FIX_H)
                AmbaMisra_TypeCast(&ptr1, &chr1);
#else
                ptr1 = (const UINT32 *)chr1;
#endif
                EnetIrq[i] = (UINT32)AmbaFDT_Fdt32ToCpu(ptr1[1]);
                EnetIrq[i] += AMBA_INT_SPI_BASE;
                EnetDebug("ENET %s(%d) interrupts 0x%x", __func__, __LINE__, EnetIrq[i]);
            } else {
                ret = -1;
            }
        }
    }

    return ret;
}

#else //#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV28)
#define ETH_INT_VEC                     AMBA_INT_SPI_ID90_ENET_SBD
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define ETH_INT_VEC                     AMBA_INT_SPI_ID90_ENET_SBD
#define ETH1_INT_VEC                    AMBA_INT_SPI_ID92_ENET_SBD1
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define ETH_INT_VEC                     AMBA_INT_SPI_ID096_ENET0_SBD
#define ETH1_INT_VEC                    AMBA_INT_SPI_ID165_ENET1_SBD
#elif defined(CONFIG_SOC_H22)
#define ETH_INT_VEC                     AMBA_INT_SPI_ID97_ENET_SBD
#else
#error ETH_INT_VEC
#endif
UINT32 EnetIrq[] = {
    (UINT32)ETH_INT_VEC,
#if (ENET_INSTANCES >= 2U)
    (UINT32)ETH1_INT_VEC
#endif
};
ULONG EnetReg[] = {
    ENET0_REGBASE,
#if (ENET_INSTANCES >= 2U)
    ENET1_REGBASE
#endif
};
#endif //#if defined(CONFIG_DEVICE_TREE_SUPPORT)

static const UINT32 *EnetVirtToPhys(const UINT32 *pVirtAddr)
{
#if defined(CONFIG_THREADX)
#if defined(CONFIG_ENABLE_VIRTUAL_ADDRESS)
    const UINT32 *pReg;
    UINT32 Reg, Phys;

    if (AmbaWrap_memcpy(&Reg, &pVirtAddr, sizeof(Reg))!= 0U) { }
    (void)AmbaMMU_Virt32ToPhys32(Reg, &Phys);

    pReg = NULL;
    if (AmbaWrap_memcpy(&pReg, &Phys, sizeof(Phys))!= 0U) { }
    return pReg;
#else
    return pVirtAddr;
#endif
#endif

#if defined(__QNXNTO__)
    int ret;
    off64_t Phys;

    //for performance: cache the result of mem_offset64()
    ret = mem_offset64((const void *)pVirtAddr, NOFD, 1, &Phys, 0);
    if (ret == -1) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "mem_offset64 error");
        Phys = (off64_t)pVirtAddr;
    }
    EnetDebug("ENET %s(%d) virt 0x%p -> phys 0x%lx\n", __func__, __LINE__, pVirtAddr, Phys);
    //fprintf(stderr, "ENET %s(%d) virt 0x%p -> phys 0x%lx\n", __func__, __LINE__, pVirtAddr, Phys);

    return (UINT32 *)Phys;
#endif
}

/*
static UINT32 EnetPhysToVirt(UINT32 PhysAddr)
{
#if defined(CONFIG_THREADX)
#if defined(CONFIG_ENABLE_VIRTUAL_ADDRESS)
    UINT32 Virt;
    (void)AmbaMMU_Phys32ToVirt32(PhysAddr, &Virt);
    return Virt;
#else
    return PhysAddr;
#endif
#endif
}
*/

static void EnetCacheClean(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;
//#if defined(__QNXNTO__)
//    const UINT32 *pPhy = EnetVirtToPhys(pAddr);
//#endif
    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }
#ifdef ETHDEBUG
    EnetDebug("ENET %s(%d) AmbaCache_Clean %p 0x%08x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        EnetDebug("\x1b" "[1;32m ENET %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
#if defined(CONFIG_THREADX)
    (void) AmbaCache_DataClean(Reg, Size);
#endif
#if defined(__QNXNTO__)
    (void) Size;
//    CACHE_FLUSH(&enet_cache, (void *)pAddr, (uint64_t)pPhy, Size);
#endif
}

static void EnetCacheInvalidate(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;
//#if defined(__QNXNTO__)
//    const UINT32 *pPhy = EnetVirtToPhys(pAddr);
//#endif

    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }

#ifdef ETHDEBUG
    EnetDebug("ENET %s(%d) AmbaCache_Invalidate %p 0x%08x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        EnetDebug("\x1b" "[1;32m ENET %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
#if defined(CONFIG_THREADX)
    (void) AmbaCache_DataInvalidate(Reg, Size);
#endif
#if defined(__QNXNTO__)
    (void) Size;
//    CACHE_INVAL(&enet_cache, (void *)pAddr, (uint64_t)pPhy, Size);
#endif
}

static void EnetCacheFlush(const void *pAddr, UINT32 Size)
{
    UINT32 Reg;
//#if defined(__QNXNTO__)
//    const UINT32 *pPhy = EnetVirtToPhys(pAddr);
//#endif

    /*Reg = (UINT32)pAddr;*/
    if (AmbaWrap_memcpy(&Reg, &pAddr, sizeof(Reg))!= 0U) { }

#ifdef ETHDEBUG
    EnetDebug("ENET %s(%d) AmbaCache_Flush %p 0x%08x", __func__, __LINE__, pAddr, Size);
    if ((Reg & (CACHE_LINE_SIZE - 1)) != 0x0) {
        EnetDebug("\x1b" "[1;32m ENET %s(%d) %p NOT aligned" "\x1b" "[0m", __func__, __LINE__, pAddr);
    }
#endif
#if defined(CONFIG_THREADX)
    (void) AmbaCache_DataFlush(Reg, Size);
#endif
#if defined(__QNXNTO__)
    (void) Size;
//    CACHE_FLUSH(&enet_cache, (void *)pAddr, (uint64_t)pPhy, Size);
//    CACHE_INVAL(&enet_cache, (void *)pAddr, (uint64_t)pPhy, Size);
#endif
}

#if defined(CONFIG_THREADX)
static void EnetWaitRxIrq(UINT32 Idx)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 RxCur;
    const AMBA_ENET_RDES_s *pRReg;
    const UINT32 *pReg, *pRxCurDes;

    /* interrrupt mode */
    AmbaCSL_EnetDmaRxIrqEnable(pEnetReg);
    (void) AmbaKAL_SemaphoreTake(&EnetRxSem[Idx], KAL_WAIT_FOREVER);

    EnetDebug("\x1b" "[1;35m ENET [RX%d]" "\x1b" "[0m", Idx);
    RxCur = EnetRxCur[Idx] % pDes->RDESCnt;
    pRReg = &pDes->pRDES[RxCur];
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pReg, &pRReg);
#else
    pReg = (const UINT32 *)pRReg;
#endif

    pRxCurDes = EnetVirtToPhys(pReg);
    EnetDebug("ENET %s(%d) RDES[%d]=%p, DmaR19=0x%08x", __func__, __LINE__, RxCur, pRxCurDes, AmbaCSL_EnetDmaGetDmaCurRxDes(pEnetReg));
}
#endif

#if defined(CONFIG_THREADX)
static UINT32 EnetDoRx(UINT32 Idx)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 RxCur = EnetRxCur[Idx] % pDes->RDESCnt;
    UINT16 RxLen, Ret = 1;
    volatile const UINT32 *pReg;
    const void *ptr;
    AMBA_ENET_STAT_s *pEnetStat = &AmbaCSL_EnetStat[Idx];

    /* invalidate cache before read from DMA */
    pReg = &pDes->pRDES[RxCur].RDES0;
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&ptr, &pReg);
#else
    ptr = (const void *)pReg;
#endif
    EnetCacheInvalidate(ptr, (UINT32)sizeof(UINT32));
    RxLen = AmbaCSL_EnetRDESGetFL(&pDes->pRDES[RxCur]);
    EnetDebug("ENET %s(%d) EnetDes[%d].RDES[%d].RDES0=0x%08x ", __func__, __LINE__, Idx, RxCur, pDes->pRDES[RxCur].RDES0);

    /* owner=host, handle RX */
    if (AmbaCSL_EnetRDESGetDmaOwn(&pDes->pRDES[RxCur]) == 0U) {
        const UINT8 *pBuf;

        /* invalidate cache before read from DMA */
        pBuf = pDes->pRxDma[RxCur].Buf;
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pBuf);
#else
        ptr = (const void *)pBuf;
#endif

        EnetCacheInvalidate(ptr, (UINT32)RxLen);

        /* Rx callback func */
        if (pEnetConfig->pRxCb != NULL) {
            (void)pEnetConfig->pRxCb(Idx, RxLen);
        }

        EnetDebug("ENET %s(%d) RX desc[%u] "
                  "0x%08x 0x%08x 0x%08x 0x%08x", __func__, __LINE__, RxCur,
                  pDes->pRDES[RxCur].RDES0, pDes->pRDES[RxCur].RDES1,
                  pDes->pRDES[RxCur].RDES2_Buf1, pDes->pRDES[RxCur].RDES3_Buf2);
        ambhw_dump_buffer(__func__, pDes->pRxDma[RxCur].Buf, 32);
        AmbaCSL_EnetRDESDumpStatus(&pDes->pRDES[RxCur], pEnetStat);

        /* RX done, set owner=DMA to get next frame */
        AmbaCSL_EnetRDESSetDmaOwn(&pDes->pRDES[RxCur]);
        /* clean cache before send to DMA */
        pReg = &pDes->pRDES[RxCur].RDES0;
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pReg);
#else
        ptr = (const void *)pReg;
#endif

        EnetCacheClean(ptr, (UINT32)sizeof(UINT32));

        Ret = 0;
    }

    return Ret;
}
#endif //defined(CONFIG_THREADX)

#if defined(CONFIG_THREADX)
static void EnetRxTaskEntry(const UINT32 Idx)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 i, Speed;

    while (EnetInited[Idx] == 0U) {
        (void) AmbaKAL_TaskSleep(1);
    }

#if defined(PHYDEBUGMDIO)
    {
        AMBA_ENET_CONFIG_s *const pEnetCfg = pAmbaEnetConfig[Idx];
        AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "MAC-to-MAC: hard code link speed to 1000M full duplex");
        Speed = SPEED_1000;
        pEnetCfg->LinkSpeed = Speed;
        AmbaCSL_EnetMacLinkSetup(pEnetReg, pEnetCfg->LinkSpeed, DUPLEX_FULL);
    }
#else
    (void)AmbaEnet_IfUp(Idx, &Speed);
#endif

    for (;;) {
        /* enable interrrupt and wait */
        EnetWaitRxIrq(Idx);

        for (i = 0; i < pDes->RDESCnt; i++) {
            /* one irq will collect ALL RX frames in DMA chain */
            if (EnetDoRx(Idx) == 0U) {
                EnetRxCur[Idx]++;
                continue;
            }
            break;
        }
    }
}
#endif //defined(CONFIG_THREADX)

#if defined(CONFIG_THREADX)
static UINT32 AmbaEth_TxConfirmation(UINT32 Idx, UINT32 *TxResult)
{
    static UINT32 EnetTxDirty[2] = {0U, 0U};
    AMBA_ENET_STAT_s *pEnetStat = &AmbaCSL_EnetStat[Idx];
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 More = 0U, dirty_diff, TxDirty, TxCur;

    *TxResult = 0U;

    (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], AMBA_KAL_WAIT_FOREVER);
    TxCur = EnetTxCur[Idx];
    TxDirty = EnetTxDirty[Idx];
    (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);

    if (TxCur < TxDirty) {
        EnetDebug("ENET %s(%d) EnetTxTask bug, %d < %d", __func__, __LINE__, TxCur, TxDirty);
        dirty_diff = 0U;
    } else {
        dirty_diff = TxCur - TxDirty;
    }

    if (dirty_diff > pDes->TDESCnt) {
        EnetDebug("ENET %s(%d) EnetTxTask cannot keep-up, %d > %d, dirty %d", __func__, __LINE__, TxCur, TxDirty, dirty_diff);
        (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], AMBA_KAL_WAIT_FOREVER);
        EnetTxDirty[Idx] = EnetTxCur[Idx] - pDes->TDESCnt;
        TxCur = EnetTxCur[Idx];
        TxDirty = EnetTxDirty[Idx];
        (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);
        dirty_diff = pDes->TDESCnt;
    }

    if (dirty_diff == 0U) {
        //EnetDebug("ENET %s(%d) clean %d/%d", __func__, __LINE__, TxDirty, TxCur);
    } else {
        const UINT32 TxDirtyCopy = TxDirty % pDes->TDESCnt;
        const UINT32 wid = pDes->pTxDma[TxDirtyCopy].Locked;

        /* wait until tx finish */
        do {
            volatile const UINT32 *vReg;
            const void *ptr;
            /* invalidate cache before read from DMA */
            vReg = &pDes->pTDES[TxDirtyCopy].TDES0;
            AmbaMisra_TypeCast(&ptr, &vReg);
            EnetCacheInvalidate(ptr, (UINT32)sizeof(UINT32));
        } while (AmbaCSL_EnetTDESGetDmaOwn(&pDes->pTDES[TxDirtyCopy]) == 1U);

        /* collect statistics */
        AmbaCSL_EnetTDESDumpStatus(&pDes->pTDES[TxDirtyCopy], pEnetStat);
        EnetDebug("ENET %s(%d) collected wid %d TDES[%d] %d/%d", __func__, __LINE__, wid, TxDirtyCopy, TxDirty, TxCur);
        if (AmbaCSL_EnetTDESGetDmaES(&pDes->pTDES[TxDirtyCopy]) != 0U) {
            *TxResult = 1U;
            EnetDebug("ENET %s(%d) TX ES: Error Summary", __func__, __LINE__);
        }

        (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], AMBA_KAL_WAIT_FOREVER);
        EnetTxDirty[Idx]++;
        (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);
        dirty_diff--;
        More = dirty_diff;

        /* unlock tx dma buffer */
        if (wid != 0U) {
#if defined(ENET_ASIL)
            /* transaction end: update heartbeat */
            if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
                EnetDebug("ENET %s(%d) AmbaSafety_EnetDeq[%d, %d] failed", __func__, __LINE__, Idx, wid);
            }
#endif
            pDes->pTxDma[TxDirtyCopy].Locked = 0U;
            EnetDebug("ENET %s(%d) TDES[%d] unlocked", __func__, __LINE__, TxDirtyCopy);
        }
    }

    return More;
}

/* check Tx transaction status */
static void EnetTxTaskEntry(const UINT32 Idx)
{
    while (EnetInited[Idx] == 0U) {
        (void) AmbaKAL_TaskSleep(1);
    }
    for (;;) {
        UINT32 More = 1U, TxResult;
        //(void) AmbaKAL_SemaphoreTake(&EnetTxSem[Idx], 45U);
        (void) AmbaKAL_SemaphoreTake(&EnetTxSem[Idx], KAL_WAIT_FOREVER);
        while (More != 0U) {
            More = AmbaEth_TxConfirmation(Idx, &TxResult);
        }
    }
}

#if defined(ENET_ASIL)
static void Eth_TxIrqHdlr(const AMBA_ENET_REG_s *const pEnetReg)
{
    (void) AmbaKAL_SemaphoreGive(&EnetTxSem[AmbaRTSL_EnetGetRegIdx(pEnetReg)]);
}
#endif

static void Eth_RxIrqHdlr(const AMBA_ENET_REG_s *const pEnetReg)
{
    (void) AmbaKAL_SemaphoreGive(&EnetRxSem[AmbaRTSL_EnetGetRegIdx(pEnetReg)]);
}
#endif  //defined(CONFIG_THREADX)


#if defined(__QNXNTO__)
#if defined(QNX_ETH_TSK)
void EnetTxTaskEntry(UINT32 Idx)
{
    AMBA_ENET_REG_s *pEnetReg;
    pEnetReg = pAmbaCSL_EnetReg[Idx];

    while (EnetInited[Idx] == 0U) {
        nic_delay(1);
    }
    for (;;) {
        /* polling mode blocked forever */
        sem_wait(&EnetTxSem[Idx]);
        //mem_barrier();
        AmbaCSL_EnetDmaStartTX(pEnetReg);
        AmbaCSL_EnetDmaSetTxPoll(pEnetReg);
    }
}
#endif

UINT32 GetRxCur(UINT32 Idx)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;

    return (EnetRxCur[Idx] % pDes->RDESCnt);
}

void EnetRxCurAdv(UINT32 Idx)
{
    EnetRxCur[Idx]++;
}

UINT32 GetTxCur(UINT32 Idx)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 TxCur;

    pthread_mutex_lock(&EnetTxMutex[Idx]);
    TxCur = EnetTxCur[Idx] % pDes->TDESCnt;
    pthread_mutex_unlock(&EnetTxMutex[Idx]);

    return TxCur;

}

void EnetTxCurAdv(UINT32 Idx)
{
    pthread_mutex_lock(&EnetTxMutex[Idx]);
    EnetTxCur[Idx]++;
    pthread_mutex_unlock(&EnetTxMutex[Idx]);
}

#endif  //defined(__QNXNTO__)

static UINT32 Enet_PlugCb(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 PhyId, Speed, Duplex;
    UINT8 PhyAddr;

    UINT32 Idx = AmbaRTSL_EnetGetRegIdx(pEnetReg);
    AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];

    PhyAddr = AmbaRTSL_EnetGetPhyAddr(pEnetReg);
    PhyId = AmbaRTSL_EnetGetPhyId(pEnetReg, PhyAddr);
    EnetIsrDebug("PhyId[%u]=0x%08x", PhyAddr, PhyId);

    AmbaRTSL_GetLink(pEnetReg, PhyAddr, &Speed, &Duplex);
    pEnetConfig->LinkSpeed = Speed;
    if (pEnetConfig->LinkSpeed != SPEED_0) {
        AmbaCSL_EnetMacLinkSetup(pEnetReg, Speed, Duplex);
    }

    return pEnetConfig->LinkSpeed;
}

static UINT32 bitrev32(UINT32 x)
{
    UINT32 n = x;

    n = ((n & 0xffff0000U) >> 16U) | ((n & 0x0000ffffU) << 16U);
    n = ((n & 0xff00ff00U) >>  8U) | ((n & 0x00ff00ffU) <<  8U);
    n = ((n & 0xf0f0f0f0U) >>  4U) | ((n & 0x0f0f0f0fU) <<  4U);
    n = ((n & 0xccccccccU) >>  2U) | ((n & 0x33333333U) <<  2U);
    n = ((n & 0xaaaaaaaaU) >>  1U) | ((n & 0x55555555U) <<  1U);

    return n;
}

#if defined(ENET_ASIL)
#pragma GCC push_options
#pragma GCC optimize ("O0")
/* do lock-step calculation */
static UINT32 _bitrev32(UINT32 x)
{
    UINT32 n = x;

    n = ((n & 0xffff0000U) >> 16U) | ((n & 0x0000ffffU) << 16U);
    n = ((n & 0xff00ff00U) >>  8U) | ((n & 0x00ff00ffU) <<  8U);
    n = ((n & 0xf0f0f0f0U) >>  4U) | ((n & 0x0f0f0f0fU) <<  4U);
    n = ((n & 0xccccccccU) >>  2U) | ((n & 0x33333333U) <<  2U);
    n = ((n & 0xaaaaaaaaU) >>  1U) | ((n & 0x55555555U) <<  1U);

    return n;
}
#pragma GCC pop_options
#endif

static void EnetTDESInit(const AMBA_ENET_DES_s *const pDes, AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 i;
    const void *ptr;
    const UINT8 *pBuf;
    UINT32 Reg, Buf1 = 0U, Buf2;
    const UINT32 *pReg, *pPhy;
    AMBA_ENET_TDES_s *pTReg;
    const AMBA_ENET_TDES_s *pTRegNext;

    for (i = 0; i < pDes->TDESCnt; i++) {
        pTReg = &pDes->pTDES[i];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pTReg);
#else
        ptr = (const void *)pTReg;
#endif

        pBuf = pDes->pTxDma[i].Buf;
        if (NULL != pBuf) {
#if defined(AMBA_MISRA_FIX_H)
            AmbaMisra_TypeCast(&pReg, &pBuf);
#else
            pReg = (UINT32 *)pBuf;
#endif
            pPhy = EnetVirtToPhys(pReg);
            /*Buf1 = (UINT32)pPhy;*/
            if (AmbaWrap_memcpy(&Buf1, &pPhy, sizeof(Buf1))!= 0U) { }
        }

        pTRegNext = &pDes->pTDES[(i + 1U) % pDes->TDESCnt];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pTRegNext);
#else
        pReg = (UINT32 *)pTRegNext;
#endif
        pPhy = EnetVirtToPhys(pReg);
        /*Buf2 = (UINT32)pPhy;*/
        if (AmbaWrap_memcpy(&Buf2, &pPhy, sizeof(Buf2))!= 0U) { }

        AmbaCSL_EnetTDES0Init(pTReg);
        AmbaCSL_EnetTDES1Init(pTReg);
        AmbaCSL_EnetTDES2Init(pTReg, Buf1);
        AmbaCSL_EnetTDES3Init(pTReg, Buf2);
//802.1Q Virtual LAN, PRI: 3, ID: 6
//#define HARDVLAN 0x6006U
#define HARDVLAN 0x0U
#if (HARDVLAN > 0U)
        AmbaCSL_EnetTDESSetVLIC(pTReg, 2U);
#endif
        EnetCacheFlush(ptr, (UINT32)sizeof(AMBA_ENET_TDES_s));
    }
#if (HARDVLAN > 0U)
    AmbaCSL_EnetMacSetVLAN(pEnetReg, 2U, HARDVLAN);
#endif

    AmbaCSL_EnetDmaStopTX(pEnetReg);
    pTReg = pDes->pTDES;
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pReg, &pTReg);
#else
    pReg = (UINT32 *)pTReg;
#endif
    pPhy = EnetVirtToPhys(pReg);
    /*Reg = (UINT32)pPhy;*/
    if (AmbaWrap_memcpy(&Reg, &pPhy, sizeof(Reg))!= 0U) { }

    AmbaCSL_EnetDmaSetTxDES(pEnetReg, Reg);
}

static void EnetRDESInit(const AMBA_ENET_DES_s *const pDes, AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 i;
    const void *ptr;
    const UINT8 *pBuf;
    UINT32 Reg, Buf1, Buf2;
    const UINT32 *pReg, *pPhy;
    AMBA_ENET_RDES_s *pRReg;
    const AMBA_ENET_RDES_s *pRRegNext;

    for (i = 0; i < pDes->RDESCnt; i++) {
        pRReg = &pDes->pRDES[i];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pRReg);
#else
        ptr = (const void *)pRReg;
#endif
        pBuf = pDes->pRxDma[i].Buf;
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pBuf);
#else
        pReg = (UINT32 *) pBuf;
#endif
        pPhy = EnetVirtToPhys(pReg);
        /*Buf1 = (UINT32)pPhy;*/
        if (AmbaWrap_memcpy(&Buf1, &pPhy, sizeof(Buf1))!= 0U) { }

        pRRegNext = &pDes->pRDES[(i + 1U) % pDes->RDESCnt];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pRRegNext);
#else
        pReg = (UINT32 *)pRRegNext;
#endif
        pPhy = EnetVirtToPhys(pReg);
        /*Buf2 = (UINT32)pPhy;*/
        if (AmbaWrap_memcpy(&Buf2, &pPhy, sizeof(Buf2))!= 0U) { }

        AmbaCSL_EnetRDES0Init(pRReg);
        AmbaCSL_EnetRDES1Init(pRReg);
        AmbaCSL_EnetRDES2Init(pRReg, Buf1);
        AmbaCSL_EnetRDES3Init(pRReg, Buf2);
        AmbaCSL_EnetRDESSetRBS1(pRReg, pDes->FrameSize);
        EnetCacheFlush(ptr, (UINT32)sizeof(AMBA_ENET_RDES_s));
    }

    AmbaCSL_EnetDmaStopRX(pEnetReg);
    pRReg = pDes->pRDES;
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pReg, &pRReg);
#else
    pReg = (UINT32 *)pRReg;
#endif
    pPhy = EnetVirtToPhys(pReg);
    /*Reg = (UINT32)pPhy;*/
    if (AmbaWrap_memcpy(&Reg, &pPhy, sizeof(Reg))!= 0U) { }

    AmbaCSL_EnetDmaSetRxDES(pEnetReg, Reg);
}

//#define REALIGN_PPS_ISREXAMPLE
#if defined(REALIGN_PPS_ISREXAMPLE)
/* example pps interrupt context */
static void pEnetTsIsrCb(const UINT32 Idx)
{
    static UINT32 Count = 0U;
    const UINT32 ms = 1000000U;
//    UINT32 FlexPPS[] = {0U, 100U, 300U, 600U}; //pulse at 0ms 100ms 300ms 600ms, 1000ms....
    UINT32 FlexPPS[] = {0U, 100U, 200U, 300U, 400U, 500U, 600U, 700U, 800U, 900U};
    const UINT32 Num = sizeof(FlexPPS)/sizeof(UINT32);
    UINT32 Sec, Ns;

    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    const AMBA_ENET_TS_s *const pPhcTs = pAmbaCSL_EnetPhc[Idx];

//    AmbaAvbStack_GetPhc(Idx, &Sec, &Ns);
    Sec = pPhcTs->Sec;
    Count++;
    Ns = FlexPPS[Count % Num] * ms;
    if (FlexPPS[Count % Num] == 0U) {
        Sec++;
    }

    (void) AmbaEnet_SetTargetTsIrq(Idx, Sec, Ns);

//    (void) AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, 5U);
    AmbaCSL_EnetMacSetPPSCMD0(pEnetReg, 5U);

//    (void) AmbaEnet_SetTargetTsPPS(Idx, Sec, Ns, 2U);
    AmbaCSL_EnetMacSetPPSCMD0(pEnetReg, 2U);
}
#endif

/* Checksum Insertion Control */
UINT32 EnetSetCIC(const UINT32 Idx, UINT8 CIC);
UINT32 EnetSetCIC(const UINT32 Idx, UINT8 CIC)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 i;

    for (i = 0; i < pDes->TDESCnt; i++) {
        AmbaCSL_EnetTDESSetCIC(&pDes->pTDES[i], CIC);
    }
    return 0;
}

/* Debug */
UINT32 EnetDumpCb(const UINT32 Idx);
UINT32 EnetDumpCb(const UINT32 Idx)
{
#if defined(CONFIG_THREADX)
    extern void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
#endif
    const AMBA_ENET_REG_s *pEnetReg = pAmbaCSL_EnetReg[Idx];
    const AMBA_ENET_STAT_s *pEnetStat = &AmbaCSL_EnetStat[Idx];
    const AMBA_ENET_CONFIG_s *pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 i, DmaR18, DmaR19;
    UINT32 TxCur, TxCurDes, RxCur, RxCurDes;
    const UINT32 *pTxCurDes, *pRxCurDes;
    UINT32 Reg;
    const UINT32 *pReg;
    //const UINT8 *pBuf;
    const AMBA_ENET_RDES_s *pRReg;
    const AMBA_ENET_TDES_s *pTReg;

    EnetDebug("ENET %s(%d) pEnetReg=%p", __func__, __LINE__, pEnetReg);
    EnetDebug("ENET %s(%d) pEnetConfig=%p", __func__, __LINE__, pEnetConfig);
    EnetDebug("ENET %s(%d) pDes=%p", __func__, __LINE__, pDes);

    for (i = 0; i < pDes->TDESCnt; i++) {
        pTReg = &pDes->pTDES[i];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pTReg);
#else
        pReg = (const UINT32 *) pTReg;
#endif
        EnetCacheFlush(pReg, (UINT32)sizeof(AMBA_ENET_TDES_s));
        if ((pDes->pTDES[i].TDES2_Buf1 & ((UINT32)CACHE_LINE_SIZE - 1U)) != 0x0U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "tx dma Buf not aligned");
        }
        /*Reg = (UINT32)pReg;*/
        if (AmbaWrap_memcpy(&Reg, &pReg, sizeof(Reg))!= 0U) { }
        if ((Reg & ((UINT32)CACHE_LINE_SIZE - 1U)) != 0x0U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "TX descriptor not aligned");
        }
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "TX desc[%u] %08x %08x %08x %08x", i,
                                   pDes->pTDES[i].TDES0, pDes->pTDES[i].TDES1,
                                   pDes->pTDES[i].TDES2_Buf1, pDes->pTDES[i].TDES3_Buf2);
        /*EnetPhysToVirt(pDes->pTDES[i].TDES2_Buf1), pDes->pTDES[i].TDES3_Buf2);*/
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "0x%08x 0x%08x 0x%08x 0x%08x",
                                   pDes->pTDES[i].TDES4_Reserved, pDes->pTDES[i].TDES5_Reserved,
                                   pDes->pTDES[i].TDES6_TTSL, pDes->pTDES[i].TDES7_TTSH, 0U);
        ambhw_dump_buffer(__func__, pDes->pTxDma[i].Buf, 32);

        /*Reg = (UINT32) pDes->pTxDma[i].Buf; */
        /*
        if (AmbaWrap_memcpy(&Reg, &pDes->pTxDma[i].Buf, sizeof(Reg))!= 0U) { }
        if (EnetPhysToVirt(pDes->pTDES[i].TDES2_Buf1) != Reg) {
            Reg = EnetPhysToVirt(pDes->pTDES[i].TDES2_Buf1);
            pBuf = NULL;
            if (AmbaWrap_memcpy(&pBuf, &Reg, sizeof(Reg))!= 0U) { }
            ambhw_dump_buffer(__func__, pBuf, 32);
        }
        */
    }
    for (i = 0; i < pDes->RDESCnt; i++) {
        pRReg = &pDes->pRDES[i];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pRReg);
#else
        pReg = (const UINT32 *)pRReg;
#endif
        EnetCacheFlush(pReg, (UINT32)sizeof(AMBA_ENET_RDES_s));
        if ((pDes->pRDES[i].RDES2_Buf1 & ((UINT32)CACHE_LINE_SIZE - 1U)) != 0x0U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "rx dma Buf not aligned");
        }
        /*Reg = (UINT32)pReg;*/
        if (AmbaWrap_memcpy(&Reg, &pReg, sizeof(Reg))!= 0U) { }
        if ((Reg & ((UINT32)CACHE_LINE_SIZE - 1U)) != 0x0U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "RX descriptor not aligned");
        }
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "RX desc[%u] 0x%08x 0x%08x 0x%08x 0x%08x", i,
                                   pDes->pRDES[i].RDES0, pDes->pRDES[i].RDES1,
                                   pDes->pRDES[i].RDES2_Buf1, pDes->pRDES[i].RDES3_Buf2);
        /*EnetPhysToVirt(pDes->pRDES[i].RDES2_Buf1), pDes->pRDES[i].RDES3_Buf2);*/
        AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "0x%08x 0x%08x 0x%08x 0x%08x",
                                   pDes->pRDES[i].RDES4, pDes->pRDES[i].RDES5_Reserved,
                                   pDes->pRDES[i].RDES6_RTSL, pDes->pRDES[i].RDES7_RTSH, 0U);
        ambhw_dump_buffer(__func__, pDes->pRxDma[i].Buf, 32);

        /*Reg = (UINT32)pDes->pRxDma[i].Buf;*/
        /*
        if (AmbaWrap_memcpy(&Reg, &pDes->pRxDma[i].Buf, sizeof(Reg))!= 0U) { }
        if (EnetPhysToVirt(pDes->pRDES[i].RDES2_Buf1) != Reg) {
            Reg = EnetPhysToVirt(pDes->pRDES[i].RDES2_Buf1);
            pBuf = NULL;
            if (AmbaWrap_memcpy(&pBuf, &Reg, sizeof(Reg))!= 0U) { }
            ambhw_dump_buffer(__func__, pBuf, 32);
        }
        */
    }
    TxCur = EnetTxCur[Idx];
    pTReg = &pDes->pTDES[TxCur];
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pReg, &pTReg);
#else
    pReg = (const UINT32 *)pTReg;
#endif
    pTxCurDes = EnetVirtToPhys(pReg);
    /*TxCurDes = (UINT32)pTxCurDes;*/
    if (AmbaWrap_memcpy(&TxCurDes, &pTxCurDes, sizeof(TxCurDes))!= 0U) { }

    RxCur = EnetRxCur[Idx];
    pRReg = &pDes->pRDES[RxCur];
#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pReg, &pRReg);
#else
    pReg= (const UINT32 *)pRReg;
#endif
    pRxCurDes = EnetVirtToPhys(pReg);
    /*RxCurDes = (UINT32)pRxCurDes;*/
    if (AmbaWrap_memcpy(&RxCurDes, &pRxCurDes, sizeof(RxCurDes))!= 0U) { }

    DmaR18 = AmbaCSL_EnetDmaGetDmaCurTxDes(pEnetReg);
    DmaR19 = AmbaCSL_EnetDmaGetDmaCurRxDes(pEnetReg);

    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "TDES[%d]=0x%08x, DmaR18=0x%08x", TxCur, TxCurDes, DmaR18, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "RDES[%d]=0x%08x, DmaR19=0x%08x", RxCur, RxCurDes, DmaR19, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "[16] NIS Normal Interrupt Summary:    %d", pEnetStat->NIS, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [0] TI Transmit Interrupt:          %d", pEnetStat->TI, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [2] TU Transmit Buffer Unavailable: %d", pEnetStat->TU, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [6] RI Receive Interrupt:           %d", pEnetStat->RI, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [14] ERI Early Receive Interrupt:   %d", pEnetStat->ERI, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "[15] AIS Abnormal Interrupt Summary:  %d", pEnetStat->AIS, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [1] TPS Transmit Process Stopped:   %d", pEnetStat->TPS, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [3] TJT Transmit Jabber Timeout:    %d", pEnetStat->TJT, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [4] OVF Receive Overflow:           %d", pEnetStat->OVF, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [5] UNF Transmit Underflow:         %d", pEnetStat->UNF, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [7] RU Receive Buffer Unavailable:  %d", pEnetStat->RU, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [8] RPS Receive Process Stopped:    %d", pEnetStat->RPS, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [9] RWT Receive Watchdog Timeout:   %d", pEnetStat->RWT, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [10] ETI Early Transmit Interrupt:  %d", pEnetStat->ETI, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(ETH_MODULE_ID, "  [13] FBI Fatal Bus Error Interrupt: %d", pEnetStat->FBI, 0U, 0U, 0U, 0U);

    return 0;
}

/* Disable CRC */
UINT32 EnetSetDC(const UINT32 Idx, UINT8 DC);
UINT32 EnetSetDC(const UINT32 Idx, UINT8 DC)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 i;

    for (i = 0; i < pDes->TDESCnt; i++) {
        AmbaCSL_EnetTDESSetDC(&pDes->pTDES[i], DC);
    }
    return 0;
}

static UINT32 inner_AmbaEnet_Tx(const UINT32 Idx, const UINT16 TxLen, const UINT32 Wait, UINT32 *pSec, UINT32 *pNs)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 TxCur, Ret = ETH_ERR_NONE;
    const UINT8 *pBuf;
    volatile const AMBA_ENET_TDES_s *pTReg;
    const UINT32 *pReg;
    const void *ptr;
#if defined(CONFIG_THREADX)
    UINT32 Reg;
    const UINT32 *pPhy;
#endif

    /* arg check */
    if (TxLen > (pDes->FrameSize)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "TxLen over range!");
        Ret = ETH_ERR_EINVAL;
    } else if ((Wait != 0U) && (pSec == NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pSec NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if ((Wait != 0U) && (pNs == NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pNs NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_OSERR;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);

        /* transaction start */
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
#if defined(AMBA_KAL_H)
        (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], KAL_WAIT_FOREVER);
#else
        pthread_mutex_lock(&EnetTxMutex[Idx]);
#endif
        TxCur = EnetTxCur[Idx] % pDes->TDESCnt;
        pTReg = &pDes->pTDES[TxCur];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pTReg);
#else
        pReg = (const UINT32 *)pTReg;
#endif
        EnetDebug("\x1b" "[1;31m ENET [TX%u] %d" "\x1b" "[0m", Idx, TxLen);
        EnetDebug("ENET %s(%d) TDES[%d]=%p, DmaR18=0x%08x", __func__, __LINE__, TxCur, pReg, AmbaCSL_EnetDmaGetDmaCurTxDes(pEnetReg));

#if defined(CONFIG_THREADX)
        /* Restore LS, FS */
        AmbaCSL_EnetTDESSetLSFS(&pDes->pTDES[TxCur], 1U, 1U);
        /* restore Buf1 */
        pBuf = pDes->pTxDma[TxCur].Buf;
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pBuf);
#else
        pReg = (UINT32 *)pBuf;
#endif
        pPhy = EnetVirtToPhys(pReg);
        /*Reg = (UINT32)pPhy;*/
        if (AmbaWrap_memcpy(&Reg, &pPhy, sizeof(Reg))!= 0U) { }

        AmbaCSL_EnetTDES2Init(&pDes->pTDES[TxCur], Reg);
#endif

        /* clean cache before send to DMA */
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pReg);
#else
        ptr = (const void *)pReg;
#endif
        EnetCacheClean(ptr, TxLen);
        AmbaCSL_EnetTDESSetTBS1(&pDes->pTDES[TxCur], TxLen);
        AmbaCSL_EnetTDESSetDmaOwn(&pDes->pTDES[TxCur]);
        /* clean cache before send to DMA */
        pTReg = &pDes->pTDES[TxCur];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pTReg);
#else
        ptr= (const void *)pTReg;
#endif
        EnetCacheClean(ptr, (UINT32)(sizeof(UINT32) + sizeof(UINT32)));

        /*mem_barrier: make sure memory access (load, store) before DMB instruction have an explicit ordering*/
        DMB;
#if defined(QNX_ETH_TSK)
        sem_post(&EnetTxSem[Idx]);
#else
        AmbaCSL_EnetDmaStartTX(pEnetReg);
        AmbaCSL_EnetDmaSetTxPoll(pEnetReg);
#endif

        EnetDebug("ENET %s(%d) TX desc[%u] "
                  "0x%08x 0x%08x 0x%08x 0x%08x", __func__, __LINE__, TxCur,
                  pDes->pTDES[TxCur].TDES0, pDes->pTDES[TxCur].TDES1,
                  pDes->pTDES[TxCur].TDES2_Buf1, pDes->pTDES[TxCur].TDES3_Buf2);
        /*EnetPhysToVirt(pDes->pTDES[TxCur].TDES2_Buf1), pDes->pTDES[TxCur].TDES3_Buf2);*/
        pBuf = pDes->pTxDma[TxCur].Buf;
        ambhw_dump_buffer(__func__, pBuf, 32);

        EnetTxCur[Idx]++;
#if defined(AMBA_KAL_H)
        (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);
#else
        pthread_mutex_unlock(&EnetTxMutex[Idx]);
#endif

#if defined(AMBA_KAL_H)
        (void) AmbaKAL_SemaphoreGive(&EnetTxCtrlSem[Idx]);
#else
        sem_post(&EnetTxCtrlSem[Idx]);
#endif
        if (pEnetConfig->LinkSpeed != 0U) {
            /* wait until tx finish */
            if (Wait != 0U) {
                do {
                    volatile const UINT32 *vReg;
                    /* invalidate cache before read from DMA */
                    vReg = &pDes->pTDES[TxCur].TDES0;
#if defined(AMBA_MISRA_FIX_H)
                    AmbaMisra_TypeCast(&ptr, &vReg);
#else
                    ptr = (const void *)vReg;
#endif
                    EnetCacheInvalidate(ptr, (UINT32)sizeof(UINT32));
                } while (AmbaCSL_EnetTDESGetDmaOwn(&pDes->pTDES[TxCur]) == 1U);

                if (AmbaCSL_EnetTDESGetTTSS(&pDes->pTDES[TxCur]) != 0U) {
                    EnetDebug("ENET %s(%d) TxCur %u 0x%08x TS %u.%u", __func__, __LINE__,
                              TxCur, pDes->pTDES[TxCur].TDES0,
                              pDes->pTDES[TxCur].TDES7_TTSH, pDes->pTDES[TxCur].TDES6_TTSL);

                    /* invalidate cache before read from DMA */
                    //EnetCacheInvalidate((void *)&pDes->pTDES[TxCur].TDES6_RTSL, 8U);
                    *pNs = pDes->pTDES[TxCur].TDES6_TTSL;
                    *pSec = pDes->pTDES[TxCur].TDES7_TTSH;
                } else {
                    EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "non-PTP traffic");
                    Ret = ETH_ERR_TSERR;
                }
            } else {
                /* wait=0: this API will not wait for transaction end
                 *         check dma end status in tx task
                 */
//#if defined(ENET_ASIL)
#if 0
                /* lock tx dma buffer, unlock when tx done */
                pDes->pTxDma[TxCur].Locked = wid;
                EnetDebug("ENET %s(%d) TDES[%d] locked 0x%x", __func__, __LINE__, TxCur, wid);
#else
                /* set non-zero to lock & block AmbaEnet_GetTxBuf until tx done */
                pDes->pTxDma[TxCur].Locked = 0U;
#endif
            }
        }
#if defined(ENET_ASIL)
        /* transaction end */
//        if ((pEnetConfig->LinkSpeed == 0U) ||
//            (Wait != 0U)) {
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
//        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to set Enet config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pEnetConfig Enet Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaEnet_SetConfig(const UINT32 Idx, AMBA_ENET_CONFIG_s *pEnetConfig)
{
    UINT32 Ret = 0U;

    if (pEnetConfig == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pEnetConfig NULL!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
        pAmbaEnetConfig[Idx] = pEnetConfig;
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get Enet config
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] ppEnetConfig Enet Config Struct for ENET
 * @return enet error number
 */
UINT32 AmbaEnet_GetConfig(const UINT32 Idx, AMBA_ENET_CONFIG_s **ppEnetConfig)
{
    UINT32 Ret = 0U;

    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (ppEnetConfig == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "ppEnetConfig NULL!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
        if (pAmbaEnetConfig[Idx] == NULL) {
            Ret = ETH_ERR_ENODATA;
        }
        *ppEnetConfig = pAmbaEnetConfig[Idx];
    }
    return Ret;
}

#if defined(AMBA_KAL_H)
static UINT32 AmbaEnetInitKal(UINT32 Idx)
{
    UINT32 Ret = ETH_ERR_NONE;
    static char EnetRxName[16] = "EnetRx";
    static char EnetTxName[16] = "EnetTx";
    static char EnetTxCtrl[16] = "EnetTxCtrl";

    if (EnetRxSem[Idx].tx_semaphore_id == 0U) {
        if (OK != AmbaKAL_SemaphoreCreate(&EnetRxSem[Idx], EnetRxName, 0)) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemCreate failed!");
            Ret = ETH_ERR_OSERR;
        }
    }
    if (EnetTxSem[Idx].tx_semaphore_id == 0U) {
        if (OK != AmbaKAL_SemaphoreCreate(&EnetTxSem[Idx], EnetTxName, 0)) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemCreate failed!");
            Ret = ETH_ERR_OSERR;
        }
    }
    if ((EnetTxCtrlSem[Idx].tx_semaphore_id == 0U) && (Ret == ETH_ERR_NONE)) {
        /* sequence lock {Eth_ProvideTxBuffer -> Eth_Transmit}, init value 1 */
        if (OK != AmbaKAL_SemaphoreCreate(&EnetTxCtrlSem[Idx], EnetTxCtrl, 1U)) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemCreate failed!");
            Ret = ETH_ERR_OSERR;
        }
    }

    if ((EnetTxMutex[Idx].tx_mutex_id == 0U) && (Ret == ETH_ERR_NONE)) {
        if (OK != AmbaKAL_MutexCreate(&EnetTxMutex[Idx], EnetTxName)) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_MutexCreate failed!");
            Ret = ETH_ERR_OSERR;
        }
    }

    return Ret;
}
#endif

/**
 * @ingroup enet
 * The function is used to configure and init Ethernet driver
 *
 * @param [in] pEnetConfig Ethernet Driver Config
 * @return enet error number
 */
UINT32 AmbaEnet_Init(AMBA_ENET_CONFIG_s *pEnetConfig)
{
    UINT32 Ret = ETH_ERR_NONE;
    AMBA_ENET_REG_s *pEnetReg;

    volatile void *vptr;

    /* arg check */
    if (pEnetConfig == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetConfig NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (pEnetConfig->Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (pEnetConfig->pDes == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetConfig->pDes NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if ((pEnetConfig->Mac[0] & 0x01U) != 0U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Err: multicast address cannot be used");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
        UINT32 Idx = pEnetConfig->Idx;
#if defined(ENET_ASIL)
        UINT32 wid;
#endif
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        if (EnetFetchDTB((UINT32)CONFIG_DTB_LOADADDR) != 0) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "DTB: no ethernet entry");
            Ret = ETH_ERR_EINVAL;
        }
#endif
        if (Idx == 0U) {
            const ULONG Base = EnetReg[Idx];
            /*pAmbaCSL_EnetReg[Idx] = (AMBA_ENET_REG_s *const)Base;*/
            pAmbaCSL_EnetReg[Idx] = NULL;
            if (AmbaWrap_memcpy(&pAmbaCSL_EnetReg[Idx], &Base, sizeof(Base))!= 0U) { }
        }
#if (ENET_INSTANCES >= 2U)
        if (Idx == 1U) {
            const ULONG Base = EnetReg[Idx];
            /*pAmbaCSL_EnetReg[Idx] = (AMBA_ENET_REG_s *const)Base;*/
            pAmbaCSL_EnetReg[Idx] = NULL;
            if (AmbaWrap_memcpy(&pAmbaCSL_EnetReg[Idx], &Base, sizeof(Base))!= 0U) { }
        }
#endif
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetInit(Idx)) {
            Ret = ETH_ERR_OSERR;
        }

        wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        pEnetReg = pAmbaCSL_EnetReg[Idx];
        pAmbaEnetConfig[Idx] = pEnetConfig;
        (void) AmbaCSL_EnetMacGetPhcTsAddr(pEnetReg, &vptr);
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pAmbaCSL_EnetPhc[Idx], &vptr);
#else
        pAmbaCSL_EnetPhc[Idx] = (AMBA_ENET_TS_s *)vptr;
#endif
        EnetTxCur[Idx] = 0;
        EnetRxCur[Idx] = 0;

#if defined(AMBA_KAL_H)
        Ret |= AmbaEnetInitKal(Idx);
#else
        //sem_init(&EnetRxSem[Idx], 0, 0);
#if defined(QNX_ETH_TSK)
        sem_init(&EnetTxSem[Idx], 0, 0);
#endif
        sem_init(&EnetTxCtrlSem[Idx], 0, 1);
        pthread_mutex_init(&EnetTxMutex[Idx], NULL);
#endif

        if (Ret == ETH_ERR_NONE) {
#if defined(CONFIG_THREADX)
            pEnetConfig->pRxTaskEntry = EnetRxTaskEntry;
            pEnetConfig->pTxTaskEntry = EnetTxTaskEntry;
#if defined(ENET_ASIL)
            (void)AmbaRTSL_EnetSetTxIsrFunc(Eth_TxIrqHdlr);
#endif
#endif
            /* pre-init */
            if (pEnetConfig->pPreInitCb != NULL) {
                (void)pEnetConfig->pPreInitCb(Idx);
            }

            /* software reset */
            Ret = AmbaCSL_EnetDmaReset(pEnetReg);
            if (Ret != ETH_ERR_NONE) {
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Err: No Rx Clock. MAC cannot leave RESET state");
                Ret = ETH_ERR_EINVAL;
            }
        }

        if (Ret == ETH_ERR_NONE) {
            /* prepare DMA descriptor then clean & invalidate cache */
            EnetTDESInit(pDes, pEnetReg);
            EnetRDESInit(pDes, pEnetReg);

            AmbaRTSL_EnetInit(pEnetReg);

            AmbaCSL_EnetMacSetAddrHi(pEnetReg, ((UINT32)pEnetConfig->Mac[5] << 8) | (UINT32)pEnetConfig->Mac[4]);
            AmbaCSL_EnetMacSetAddrLo(pEnetReg,
                                     ((UINT32)pEnetConfig->Mac[3] << 24) | ((UINT32)pEnetConfig->Mac[2] << 16) |
                                     ((UINT32)pEnetConfig->Mac[1] << 8) | (UINT32)pEnetConfig->Mac[0]);
#if defined(CONFIG_THREADX)
            (void)AmbaRTSL_EnetSetRxIsrFunc(Eth_RxIrqHdlr);
#endif
            (void)AmbaRTSL_EnetSetLcFunc(Enet_PlugCb);
#if defined(REALIGN_PPS_ISREXAMPLE)
            (void)AmbaRTSL_EnetSetTsIsrFunc(pEnetTsIsrCb, Idx);
#endif
            AmbaCSL_EnetDmaStartRX(pEnetReg);
            /* TU++ after AmbaCSL_EnetDmaStartTX() */
            //AmbaCSL_EnetDmaStartTX(pEnetReg);
            AmbaCSL_EnetDmaSetRxPoll(pEnetReg);
            if (pDes->FrameSize > 1536U) {
                AmbaCSL_EnetMacSetJEJD(pEnetReg, 1U, 1U);
            }
        }
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        if (Ret == 0U) {
            EnetInited[Idx] = 1U;
        }
    }
#ifdef ETHDEBUG_ISR
    {
        void EnetDebugTaskCreate(void);
        (void)EnetDebugTaskCreate();
    }
#endif

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to transmit frame data in DMA descriptor chain,
 * for example this is how TCP/IP stack transmit the TX data
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] TxLen Transmit frame length
 * @return enet error number
 */
UINT32 AmbaEnet_Tx(const UINT32 Idx, const UINT16 TxLen)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_Tx(Idx, TxLen, 0U, NULL, NULL);
    }
    return Ret;
}

/**
 * @ingroup enet
 * This function can be called multiple times to setup Scatter-Gather DMA descriptor chain,
 * it will trigger frame-transmit when Last Segment is set.
 * This API is useful when content is NOT generated by CPU(e.g., by DSP or CV),
 * you can use this API for zero-copy transmit and bypass CPU cache
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Addr the address of frame data
 * @param [in] Len the length of frame data
 * @param [in] FS set to 1 if this is the First Segment of frame data
 * @param [in] LS set to 1 if this is the Last Segment of frame data
 * @param [in] CacheClean set to 1 to clean CPU cache of frame data. If your data is not in CPU cache, please set it to 0
 * @return enet error number
 */
static UINT32 inner_AmbaEnet_SetupTxDesc(const UINT32 Idx, const void *Addr, const UINT16 Len, UINT8 FS, UINT8 LS, UINT8 CacheClean)
{
    const AMBA_ENET_CONFIG_s *pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 TxCur, Reg, TxCurDes;
    const UINT32 *pTxCurDes;
    const UINT32 *pReg, *pPhy;
    const void *ptr;
    volatile const AMBA_ENET_TDES_s *pTReg;
    UINT32 Ret = ETH_ERR_NONE;
    UINT32 Count = 0U;

    /* arg check */
    if (Addr == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Addr NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (Len == 0U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "TBS1 Len = 0");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if ((FS != 0U) && (FS != 1U)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "FS over range!");
        Ret = ETH_ERR_EINVAL;
    } else if ((LS != 0U) && (LS != 1U)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "LS over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* First Seg: lock */
        if (FS != 0U) {
#if defined(AMBA_KAL_H)
            if (OK != AmbaKAL_SemaphoreTake(&EnetTxCtrlSem[Idx], 10000)) {
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemaphoreTake timeout");
                Ret = ETH_ERR_ETIMEDOUT;
            }
#else
            sem_wait(&EnetTxCtrlSem[Idx]);
#endif

#if defined(AMBA_KAL_H)
            (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], KAL_WAIT_FOREVER);
#else
            pthread_mutex_lock(&EnetTxMutex[Idx]);
#endif
        }

        TxCur = EnetTxCur[Idx] % pDes->TDESCnt;
        while (pDes->pTxDma[TxCur].Locked != 0U) {
#if defined(CONFIG_THREADX)
            (void) AmbaKAL_TaskSleep(1);
#elif defined(__QNXNTO__)
            nic_delay(1);
#endif
        }

        /* wait for vacant DMA chain */
        while (Count < 1000U) {
            volatile const UINT32 *vReg;
            TxCur = EnetTxCur[Idx] % pDes->TDESCnt;
            /* invalidate cache before read from DMA */
            vReg = &pDes->pTDES[TxCur].TDES0;
#if defined(AMBA_MISRA_FIX_H)
            AmbaMisra_TypeCast(&ptr, &vReg);
#else
            ptr = (const void *)vReg;
#endif
            EnetCacheInvalidate(ptr, (UINT32)CACHE_LINE_SIZE);
            if (AmbaCSL_EnetTDESGetDmaOwn(&pDes->pTDES[TxCur]) == 1U) {
#if defined(AMBA_KAL_H)
                (void) AmbaKAL_TaskSleep(1);
#elif defined(__QNXNTO__)
                nic_delay(1);
#endif
                Count++;
                if (Count > 100U) {
                    //AmbaCSL_EnetDmaStopTX(pEnetReg);
                    AmbaCSL_EnetDmaStartTX(pEnetReg);
                    AmbaCSL_EnetDmaSetTxPoll(pEnetReg);
                }
                continue;
            } else {
                break;
            }
        }
        if (Count >= 1000U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "tx dma timeout");
        }

        /* TDES2_Buf1 */
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &Addr);
#else
        pReg = (UINT32 *)Addr;
#endif
#if defined(CONFIG_THREADX)
        pPhy = EnetVirtToPhys(pReg);
#endif
#if defined(__QNXNTO__)
        pPhy = pReg;
#endif
        /*Reg = (UINT32)pPhy;*/
        if (AmbaWrap_memcpy(&Reg, &pPhy, sizeof(Reg))!= 0U) { }

        AmbaCSL_EnetTDES2Init(&pDes->pTDES[TxCur], Reg);
        /* clean Buf1, len = TBS1 */
        if (CacheClean != 0U) {
            EnetCacheClean(Addr, Len);
        }

        /*TDES1 TBS1*/
        AmbaCSL_EnetTDESSetTBS1(&pDes->pTDES[TxCur], Len);

        /* Own, Last Seg, First Seg */
        AmbaCSL_EnetTDESSetDmaOwn(&pDes->pTDES[TxCur]);
        AmbaCSL_EnetTDESSetLSFS(&pDes->pTDES[TxCur], LS, FS);

        /* clean cache before send to DMA */
        pTReg = &pDes->pTDES[TxCur];
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&ptr, &pTReg);
#else
        ptr = (const void *)pTReg;
#endif
        EnetCacheClean(ptr, (UINT32)CACHE_LINE_SIZE);

        /* logging */
#if defined(AMBA_MISRA_FIX_H)
        AmbaMisra_TypeCast(&pReg, &pTReg);
#else
        pReg = (const UINT32 *)pTReg;
#endif
#if defined(CONFIG_THREADX)
        pTxCurDes = EnetVirtToPhys(pReg);
#endif
#if defined(__QNXNTO__)
        pTxCurDes = pReg;
#endif
        /*TxCurDes = (UINT32)pTxCurDes;*/
        if (AmbaWrap_memcpy(&TxCurDes, &pTxCurDes, sizeof(TxCurDes))!= 0U) { }

        EnetDebug("\x1b" "[1;31m ENET [TX%u] len %d FS %d LS %d" "\x1b" "[0m", Idx, Len, FS, LS);
        EnetDebug("TDES[%d]=0x%08x, DmaR18=0x%08x", TxCur, TxCurDes, AmbaCSL_EnetDmaGetDmaCurTxDes(pEnetReg));
        EnetDebug("0x%08x 0x%08x 0x%08x 0x%08x",
                  pDes->pTDES[TxCur].TDES0, pDes->pTDES[TxCur].TDES1,
                  pDes->pTDES[TxCur].TDES2_Buf1, pDes->pTDES[TxCur].TDES3_Buf2);
        /*EnetPhysToVirt(pDes->pTDES[TxCur].TDES2_Buf1), pDes->pTDES[TxCur].TDES3_Buf2);*/

        /* Last Seg: trigger Tx */
        if (LS != 0U) {
            /*mem_barrier: make sure memory access (load, store) before DMB instruction have an explicit ordering*/
            DMB;
#if defined(QNX_ETH_TSK)
            sem_post(&EnetTxSem[Idx]);
#else
            AmbaCSL_EnetDmaStartTX(pEnetReg);
            AmbaCSL_EnetDmaSetTxPoll(pEnetReg);
#endif
        }
        EnetTxCur[Idx]++;

        /* Last Seg: unlock */
        if (LS != 0U) {
#if defined(AMBA_KAL_H)
            (void) AmbaKAL_SemaphoreGive(&EnetTxCtrlSem[AmbaRTSL_EnetGetRegIdx(pEnetReg)]);
            (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);
#else
            sem_post(&EnetTxCtrlSem[AmbaRTSL_EnetGetRegIdx(pEnetReg)]);
            pthread_mutex_unlock(&EnetTxMutex[Idx]);
#endif
        }
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

UINT32 AmbaEnet_SetupTxDesc(const UINT32 Idx, const void *Addr, const UINT16 Len, UINT8 FS, UINT8 LS, UINT8 CacheClean)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_SetupTxDesc(Idx, Addr, Len, FS, LS, CacheClean);
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to transmit frame data in DMA descriptor chain and return transmit timestamp,
 * for example this is how TCP/IP stack transmit the TX data
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] TxLen Transmit frame length
 * @param [out] pSec Second of transmit frame timestamp
 * @param [out] pNs Nanosecond of transmit frame timestamp
 * @return enet error number
 */
UINT32 AmbaEnet_TxWait(const UINT32 Idx, const UINT16 TxLen, UINT32 *pSec, UINT32 *pNs)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_Tx(Idx, TxLen, 1U, pSec, pNs);
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get DMA transmit buffer address,
 * for example TCP/IP stacks needs to fill-in data before transmit ethernet frame
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] Addr Addr will be set to the address of Tx Dma Buffer in struct AMBA_ENET_DES_s
 * @return enet error number
 */
static UINT32 inner_AmbaEnet_GetTxBuf(const UINT32 Idx, void **Addr)
{
    const AMBA_ENET_CONFIG_s *pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 TxCur;
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Addr == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Addr NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
#if defined(AMBA_KAL_H)
        if (OK != AmbaKAL_SemaphoreTake(&EnetTxCtrlSem[Idx], 10000)) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "AmbaKAL_SemaphoreTake timeout");
            Ret = ETH_ERR_ETIMEDOUT;
        }
#else
        sem_wait(&EnetTxCtrlSem[Idx]);
#endif

#if defined(AMBA_KAL_H)
        (void) AmbaKAL_MutexTake(&EnetTxMutex[Idx], KAL_WAIT_FOREVER);
#else
        pthread_mutex_lock(&EnetTxMutex[Idx]);
#endif

        TxCur = EnetTxCur[Idx] % pDes->TDESCnt;
#if defined(AMBA_KAL_H)
        (void) AmbaKAL_MutexGive(&EnetTxMutex[Idx]);
#else
        pthread_mutex_unlock(&EnetTxMutex[Idx]);
#endif

        while (pDes->pTxDma[TxCur].Locked != 0U) {
#if defined(CONFIG_THREADX)
            (void) AmbaKAL_TaskSleep(1);
#elif defined(__QNXNTO__)
            nic_delay(1);
#endif
        }

        /* wait for vacant DMA chain */
        for (;;) {
            const void *ptr;
            volatile const UINT32 *vReg;
            /* invalidate cache before read from DMA */
            vReg = &pDes->pTDES[TxCur].TDES0;
#if defined(AMBA_MISRA_FIX_H)
            AmbaMisra_TypeCast(&ptr, &vReg);
#else
            ptr = (const void *)vReg;
#endif
            EnetCacheInvalidate(ptr, (UINT32)sizeof(UINT32));
            if (AmbaCSL_EnetTDESGetDmaOwn(&pDes->pTDES[TxCur]) == 1U) {
#if defined(AMBA_KAL_H)
                (void) AmbaKAL_TaskYield();
#elif defined(__QNXNTO__)
                nic_delay(1);
#endif
                continue;
            } else {
                break;
            }
        }

        *Addr = pDes->pTxDma[TxCur].Buf;
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

UINT32 AmbaEnet_GetTxBuf(const UINT32 Idx, void **Addr)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_GetTxBuf(Idx, Addr);
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get received frame buffer address,
 * TCP/IP stack use this function to get received data frame
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] Addr Addr will be set to the address of Rx Dma buffer in struct AMBA_ENET_DES_s
 * @return enet error number
 */
static UINT32 inner_AmbaEnet_GetRxBuf(const UINT32 Idx, void **Addr)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 RxCur = EnetRxCur[Idx] % pDes->RDESCnt;
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Addr == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Addr NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
        *Addr = pDes->pRxDma[RxCur].Buf;
    }

    return Ret;
}

UINT32 AmbaEnet_GetRxBuf(const UINT32 Idx, void **Addr)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_GetRxBuf(Idx, Addr);
    }
    return Ret;
}
/**
 * @ingroup enet
 * The function is used to get interrupt when target time achieve in system time
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Sec Second of target time
 * @param [in] Ns Nanosecond of target time
 * @return enet error number
 */
UINT32 AmbaEnet_SetTargetTsIrq(UINT32 Idx, const UINT32 Sec, const UINT32 Ns)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (Ns > NS_PER_SECOND) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Ns over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* set irq Target Timestamp value R455 R456 */
        AmbaCSL_EnetMacSetTSTR(pEnetReg, Sec);
        AmbaCSL_EnetMacSetTTSLO(pEnetReg, Ns);

        /* R15[9] TSIM=0: enable assertion of the interrupt signal */
        AmbaCSL_EnetMacSetTSIM(pEnetReg, 0U);

        /* R448[4] TSTRIG = 1: re-generate Timestamp Trigger Interrupt */
        AmbaCSL_EnetMacSetTSTRIG(pEnetReg, 1U);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to set the Flexible PPS Output mode when Target Time is reached/exceeded.
 * PPS stands for Pulse-Per-Second
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Sec Second of target time
 * @param [in] Ns Nanosecond of target time
 * @param [in] PPSCMD Flexible PPS Output Command of following:
 * - 0: No Op
 * - 1: Start Single Pulse
 * - 2: Start Pulse Train
 * - 3: Cancel Start
 * - 4: Stop Pulse Train at time specified
 * - 5: Stop Pulse Train immediately
 * - 6: Cancel Stop Pulse Train
 * @return enet error number
 */
UINT32 AmbaEnet_SetTargetTsPPS(UINT32 Idx, const UINT32 Sec, const UINT32 Ns, UINT8 PPSCMD)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (PPSCMD > 6U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "PPSCMD over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (Ns > NS_PER_SECOND) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Ns over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* set irq Target Timestamp value R455 R456 */
        AmbaCSL_EnetMacSetTSTR(pEnetReg, Sec);
        AmbaCSL_EnetMacSetTTSLO(pEnetReg, Ns);

        /* R459[4] PPSEN0=1, R459[3:0] PPSCMD0 Start/Stop PPS0 Single/Train */
        AmbaCSL_EnetMacSetPPSCMD0(pEnetReg, PPSCMD);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to config the PPS Output Signal
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] PPSINT PPS Interval between the rising edges (ns)
 * @param [in] PPSWIDTH PPS pulse Width (ns)
 * @return enet error number
 */
UINT32 AmbaEnet_SetPPS(UINT32 Idx, const UINT32 PPSINT, const UINT32 PPSWIDTH)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* PPS0 Interval R472 e.g. 100ms = 50ns * 2000000 */
        AmbaCSL_EnetMacSetPPSINT(pEnetReg, PPSINT/50U);

        /* PPS0 Width R473 e.g. 1ms = 50ns * 20000 */
        AmbaCSL_EnetMacSetPPSWIDTH(pEnetReg, PPSWIDTH/50U);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get received frame timestamp
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] pSec Second of received frame timestamp
 * @param [out] pNs Nanosecond of received frame timestamp
 * @return enet error number
 */
static UINT32 inner_AmbaEnet_GetRxTs(UINT32 Idx, UINT32 *pSec, UINT32 *pNs)
{
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    const AMBA_ENET_DES_s *const pDes = pEnetConfig->pDes;
    UINT32 RxCur = EnetRxCur[Idx] % pDes->RDESCnt;
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (pSec == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pSec NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (pNs == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pNs NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
        /* invalidate cache before read from DMA */
        //EnetCacheInvalidate((void *)&pDes->pRDES[RxCur].RDES6_RTSL, 8U);

        EnetDebug("ENET %s(%d) RxCur %u TS %u.%u", __func__, __LINE__, RxCur,
                  pDes->pRDES[RxCur].RDES7_RTSH, pDes->pRDES[RxCur].RDES6_RTSL);

        *pNs = pDes->pRDES[RxCur].RDES6_RTSL;
        *pSec = pDes->pRDES[RxCur].RDES7_RTSH;
    }

    return Ret;
}

UINT32 AmbaEnet_GetRxTs(UINT32 Idx, UINT32 *pSec, UINT32 *pNs)
{
    UINT32 Ret;
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else {
        Ret = inner_AmbaEnet_GetRxTs(Idx, pSec, pNs);
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to set system time of ENET
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Sec Second of system time
 * @param [in] Ns Nanosecond of system time
 * @return enet error number
 */
UINT32 AmbaEnet_PhcSetTs(UINT32 Idx, const UINT32 Sec, const UINT32 Ns)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (Ns > NS_PER_SECOND) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Ns over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        AmbaCSL_EnetMacSetTSS(pEnetReg, Sec);
        AmbaCSL_EnetMacSetTSSS(pEnetReg, 0, Ns);
        AmbaCSL_EnetMacTSINIT(pEnetReg);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to add or sub time shift to system time of ENET
 *
 * @param Idx ENET controller index on AMBA SoC, value start from 0
 * @param ADDSUB Add or sub time shift
 * @param Sec Second of time shift
 * @param Ns Nanosecond of time shift
 * @return enet error number
 */
UINT32 AmbaEnet_PhcUpdaTs(UINT32 Idx, UINT8 ADDSUB, const UINT32 Sec, const UINT32 Ns)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if ((ADDSUB != 0U) && (ADDSUB != 1U)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "ADDSUB over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (Ns > NS_PER_SECOND) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Ns over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        AmbaCSL_EnetMacSetTSS(pEnetReg, Sec);
        AmbaCSL_EnetMacSetTSSS(pEnetReg, ADDSUB, Ns);
        AmbaCSL_EnetMacTSUPDT(pEnetReg);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/* return frequency adjustment in parts ber billion
 *  ppb = adjustment * 1000000000/0xd5555555
 */
INT32 AmbaEnet_PhcGetPpb(UINT32 Idx);
INT32 AmbaEnet_PhcGetPpb(UINT32 Idx)
{
    const AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    const UINT32 uaddend = AmbaCSL_EnetMacGetTSAR(pEnetReg);
    UINT32 neg, udiff;
    UINT64 adj64;
    INT32 ppb, Ret;

    if (DEFAULT_ADDEND > uaddend) {
        udiff = DEFAULT_ADDEND - uaddend;
        neg = 1U;
    } else {
        udiff = uaddend - DEFAULT_ADDEND;
        neg = 0U;
    }
    adj64 = 0U;
    if (AmbaWrap_memcpy(&adj64, &udiff, sizeof(udiff))!= 0U) { }
    adj64 *= NS_PER_SECOND;
    adj64 /= DEFAULT_ADDEND;
    if (AmbaWrap_memcpy(&ppb, &adj64, sizeof(ppb))!= 0U) { }
    if (neg == 0U) {
        EnetDebug(" ppb corrected       %09d", ppb);
        Ret = ppb;
    } else {
        EnetDebug(" ppb corrected      -%09d", ppb);
        Ret = -ppb;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to adjust the frequency of hardware clock
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] ppb Period change in parts ber billion
 * @return enet error number
 */
UINT32 AmbaEnet_PhcAdjFreq(UINT32 Idx, INT32 ppb)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    INT32 ppbCopy = ppb;
    UINT32 neg = 0U, uaddend, curaddend, udiff;
    UINT64 adj64;
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* ppb: desired period change in parts ber billion */
        if (ppb < 0) {
            /* negative ppb make PTP Hardware Clock run slower */
            neg = 1U;
            ppbCopy = -ppb;
        }

        /* adjustment = 0xd5555555 * ppb/1000000000 */
        /* absolute ppb */
        curaddend = DEFAULT_ADDEND;
        /* relative ppb */
        //curaddend = AmbaCSL_EnetMacGetTSAR(pEnetReg);
        adj64 = 0U;
        //adj64 = (UINT64)_ppb;
        if (AmbaWrap_memcpy(&adj64, &ppbCopy, sizeof(ppbCopy))!= 0U) { }
        adj64 *= curaddend;
        adj64 /= NS_PER_SECOND;

        if (adj64 <= curaddend) {
            /* udiff = adj64; */
            if (AmbaWrap_memcpy(&udiff, &adj64, sizeof(udiff))!= 0U) { }
            if (neg == 0U) {
                uaddend = curaddend + udiff;
            } else {
                uaddend = curaddend - udiff;
            }

            AmbaCSL_EnetMacSetTSAR(pEnetReg, uaddend);
            AmbaCSL_EnetMacSetTSADDREG(pEnetReg);
        }
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to get Ptp Hardware Clock value
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] pSec Seconds value of Ptp Hardware Clock
 * @param [in] pNs Nano-Seconds value of Ptp Hardware Clock
 * @return enet error number
 */
UINT32 AmbaEnet_GetPhc(UINT32 Idx, UINT32 *pSec, UINT32 *pNs);
UINT32 AmbaEnet_GetPhc(UINT32 Idx, UINT32 *pSec, UINT32 *pNs)
{
    const AMBA_ENET_TS_s *const pPhcTs = pAmbaCSL_EnetPhc[Idx];
    UINT32 Ret = ETH_ERR_NONE;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (pSec == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pSec NULL!");
        Ret = ETH_ERR_EINVAL;
    } else if (pNs == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "pNs NULL!");
        Ret = ETH_ERR_EINVAL;
    } else {
        /* code start */
        *pSec = pPhcTs->Sec;
        *pNs = pPhcTs->Ns;
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to config rx filter for multicast
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] Addr MAC address to filter
 * @return enet error number
 */
UINT32 AmbaEnet_McastRxHash(UINT32 Idx, const UINT8 *Addr)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 crc32 = 0U, HashHi, bit_nr, hash = 0U;
    UINT32 Ret = 0U;
    UINT8 buf[8];

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 _crc32;
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif

        EnetDebug("ENET %s(%d) %02x:%02x:%02x:%02x:%02x:%02x", __func__, __LINE__, Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
#if defined(CONFIG_THREADX)
        if (AmbaWrap_memcpy(buf, Addr, 6)!= 0U) { }
        buf[6] = 0U;
        buf[7] = 0U;
        crc32 = IO_UtilityCrc32(buf, 6U);
#endif
        EnetDebug("ENET %s(%d) crc32 0x%x", __func__, __LINE__, crc32);

        /* bit reverse */
        crc32 = bitrev32(crc32);
        EnetDebug("bitrev32 0x%x", crc32);

#if defined(ENET_ASIL)
        /* do lock-step calculation */
        _crc32 = IO_UtilityCrc32(Addr, 6U);
        _crc32 = _bitrev32(_crc32);
        if (_crc32 != crc32) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "lock-step calculation failure");
            Ret = ETH_ERR_OSERR;
        }
#endif
        /* keep only upper 6 bits */
        crc32 = crc32 >> 26;

        /* bit6 selects R2 or R3 */
        HashHi = crc32 & 0x20U;

        /* Value of bit[5:0] selects BitX of R2/R3 */
        bit_nr = crc32 & 0x1FU;
        hash |= (UINT32)0x1U << bit_nr;

        if (0U == HashHi) {
            EnetDebug("ENET %s(%d) R3 bit%d", __func__, __LINE__, bit_nr);
            AmbaCSL_EnetMacSetHashLo(pEnetReg, hash);
        } else {
            EnetDebug("ENET %s(%d) R2 bit%d", __func__, __LINE__, bit_nr);
            AmbaCSL_EnetMacSetHashHi(pEnetReg, hash);
        }
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function bring-up the Ethernet link
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [out] Speed current link speed
 * @return enet error number
 */
UINT32 AmbaEnet_IfUp(const UINT32 Idx, UINT32 *Speed)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 PhyId, SpeedCopy, Duplex;
    UINT8 PhyAddr;
    AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
        /* TU++ after AmbaCSL_EnetDmaStartTX() */
        //AmbaCSL_EnetDmaStartTX(pEnetReg);
        AmbaCSL_EnetDmaStartRX(pEnetReg);

        /* valid addr range is 0~31 */
        PhyAddr = AmbaRTSL_EnetGetPhyAddr(pEnetReg);
#if defined(PHYDEBUGMDIO)
        if (PhyAddr >= 32U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "No PHY!");
        }
#else
        while (PhyAddr >= 32U) {
#if defined(CONFIG_THREADX)
            (void) AmbaKAL_TaskSleep(1000);
#elif defined(__QNXNTO__)
            nic_delay(1000);
#endif
            PhyAddr = AmbaRTSL_EnetGetPhyAddr(pEnetReg);
        }
#endif
        PhyId = AmbaRTSL_EnetGetPhyId(pEnetReg, PhyAddr);

        if (pEnetConfig->pPhyInitCb != NULL) {
            (void)pEnetConfig->pPhyInitCb(PhyId);
        }

        AmbaRTSL_GetLink(pEnetReg, PhyAddr, &SpeedCopy, &Duplex);
        *Speed = SpeedCopy;
        pEnetConfig->LinkSpeed = SpeedCopy;

        if (pEnetConfig->LinkSpeed != SPEED_0) {
            AmbaCSL_EnetMacLinkSetup(pEnetReg, SpeedCopy, Duplex);
        }

        if (pEnetConfig->pLinkUp != NULL) {
            (void) pEnetConfig->pLinkUp(Idx, *Speed);
        }
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function bring-down the Ethernet link and stop all transfer/receive
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @return enet error number
 */
UINT32 AmbaEnet_IfDown(const UINT32 Idx)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    const AMBA_ENET_CONFIG_s *const pEnetConfig = pAmbaEnetConfig[Idx];
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        AmbaCSL_EnetDmaStopTX(pEnetReg);
        AmbaCSL_EnetMacTxDisable(pEnetReg);
        AmbaCSL_EnetMacRxDisable(pEnetReg);
        AmbaCSL_EnetDmaStopRX(pEnetReg);

        if (pEnetConfig->pLinkDown != NULL) {
            (void) pEnetConfig->pLinkDown(Idx);
        }
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }
    return Ret;
}

/**
 * @ingroup enet
 * The function is used to read the PHY register
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] PhyAddr Phy Bus Address on schematics
 * @param [in] Offset Phy register offset
 * @param [out] Value Register Value
 * @return enet error number
 */
UINT32 AmbaEnet_PhyRead(UINT32 Idx, UINT32 PhyAddr, UINT32 Offset, UINT32 *Value)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (Value == NULL) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "NULL Value!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (PhyAddr > 31U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "PhyAddr over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (Offset > 31U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Offset over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        *Value = (UINT32)AmbaRTSL_EnetMiiRead(pEnetReg, (UINT8)PhyAddr, (UINT8)Offset);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}

/**
 * @ingroup enet
 * The function is used to write the PHY register
 *
 * @param [in] Idx ENET controller index on AMBA SoC, value start from 0
 * @param [in] PhyAddr Phy Bus Address on schematics
 * @param [in] Offset Phy register offset
 * @param [in] Value Register Value
 * @return enet error number
 */
UINT32 AmbaEnet_PhyWrite(UINT32 Idx, UINT32 PhyAddr, UINT32 Offset, UINT32 Value)
{
    AMBA_ENET_REG_s *const pEnetReg = pAmbaCSL_EnetReg[Idx];
    UINT32 Ret = 0U;

    /* arg check */
    if (Idx >= ENET_INSTANCES) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Idx over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (EnetInited[Idx] != 1U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "not init!");
        Ret = ETH_ERR_EINVAL;
    } else if (PhyAddr > 31U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "PhyAddr over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (Offset > 31U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Offset over range!");
        Ret = ETH_ERR_EINVAL;
    } else if (Value >= 0x10000U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "Value over range!");
        Ret = ETH_ERR_EINVAL;
#if defined(ENET_ASIL)
    } else if (0U != AmbaSafety_EnetGetSafeState(Idx, NULL)) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_RED, "EnetSafeState");
        Ret = ETH_ERR_EINVAL;
#endif
    } else {
        /* code start */
#if defined(ENET_ASIL)
        UINT32 wid = AmbaSafety_EnetGetWId(Idx, __func__);
        if (0U != AmbaSafety_EnetEnq(Idx, wid)) {
            Ret = ETH_ERR_OSERR;
        }
#endif
        Ret |= (UINT32)AmbaRTSL_EnetMiiWrite(pEnetReg, (UINT8)PhyAddr, (UINT8)Offset, (UINT16)Value);
#if defined(ENET_ASIL)
        if (0U != AmbaSafety_EnetDeq(Idx, wid)) {
            Ret |= ETH_ERR_OSERR;
        }
#endif
    }

    return Ret;
}
