/**
 * @file AmbaCSL_ENET.c
 * ENET Chip Support Library: Register read/write happens here
 *
 * @ingroup enet
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

#include "AmbaENET_Def.h"
#include "AmbaCSL_ENET.h"
#if defined(ENET_ASIL)
#include "AmbaSafety_ENET.h"
#endif

#if defined(CONFIG_THREADX)
extern void AmbaMisra_VolatileCast32(volatile void *pNewType, volatile const void *pOldType);
#else
#define AmbaMisra_VolatileCast32(a, b) memcpy((void *)a, (void *)b, 4)
#endif
AMBA_ENET_REG_s *pAmbaCSL_EnetReg[ENET_INSTANCES];
AMBA_ENET_STAT_s AmbaCSL_EnetStat[ENET_INSTANCES];
AMBA_ENET_TS_s *pAmbaCSL_EnetPhc[ENET_INSTANCES];

#if defined(ETHDEBUG)
#define EnetWriteLong(p, v)       do { \
        EnetDebug("ENET %s(%d) EnetWriteLong %p 0x%08x", __func__, __LINE__, (void *)(p), (v)); \
        WriteLongEnet(p, v); \
} while (0);
#else
#define EnetWriteLong WriteLongEnet
#endif

#if defined(ETHDEBUG_ISR) && defined(ETHDEBUG)
#define WriteLongIsr(p, v)       do { \
        EnetIsrDebug("EnetWriteLong %p 0x%08x", (void *)(p), (v)); \
        WriteLongEnet(p, v); \
} while (0);
#else
#define WriteLongIsr WriteLongEnet
#endif

static void WriteLongEnet(volatile void *pAddr, UINT32 Value)
{
#if defined(ENET_ASIL)
    extern void AmbaSafety_EnetShadowWrite(volatile UINT32 *pAddr, UINT32 Value);
    AmbaSafety_EnetShadowWrite(pAddr, Value);
#endif  //  defined(ENET_ASIL)

#if defined(CONFIG_THREADX)
    {
#if defined(GCOVR_ENABLE)
        UINT32 GcovrDummy = 0;
        volatile UINT32 *ptr = &GcovrDummy;
#else
        volatile UINT32 *ptr;
#endif
        (void)pAddr;

#if defined(CONFIG_THREADX64)
        AmbaMisra_TypeCast(&ptr, &pAddr);
#else
        AmbaMisra_VolatileCast32(&ptr, &pAddr);
#endif
        *ptr = Value;
    }
#endif  //  defined(CONFIG_THREADX)

#if defined(__QNXNTO__)
    out32((uintptr_t)pAddr, Value);
#endif  //  defined(__QNXNTO__)
}

/** 0x0000        Transmit Descriptor 0 (TDES0) */
void AmbaCSL_EnetTDES0Init(AMBA_ENET_TDES_s *pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &val);
    AmbaEnet_TDES.TDES0.OWN = 0U;
    AmbaEnet_TDES.TDES0.IC = 1U;
    AmbaEnet_TDES.TDES0.LS = 1U;
    AmbaEnet_TDES.TDES0.FS = 1U;
    AmbaEnet_TDES.TDES0.TCH = 1U;
    /* Checksum Insertion Control */
#if defined(TX_CHECKSUM_OFFLOAD)
    AmbaEnet_TDES.TDES0.CIC = TX_CHECKSUM_OFFLOAD;
#else
    AmbaEnet_TDES.TDES0.CIC = 0x3U;
#endif
    /* Transmit Timestamp Enable */
    AmbaEnet_TDES.TDES0.TTSE = 1U;

    AmbaEnet_TDES.TDES0.OWN = 0U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
    return;
}

void AmbaCSL_EnetTDESSetDmaOwn(AMBA_ENET_TDES_s *pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);
    AmbaEnet_TDES.TDES0.OWN = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
}

UINT32 AmbaCSL_EnetTDESGetDmaOwn(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);

    return AmbaEnet_TDES.TDES0.OWN;
}

UINT32 AmbaCSL_EnetTDESGetDmaES(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);

    return AmbaEnet_TDES.TDES0.ES;
}

UINT32 AmbaCSL_EnetTDESGetTTSS(const AMBA_ENET_TDES_s *const pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);

    return AmbaEnet_TDES.TDES0.TTSS;
}

void AmbaCSL_EnetTDESSetCIC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 CIC)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);
    /* Checksum Insertion Control */
    AmbaEnet_TDES.TDES0.CIC = CIC;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
}

void AmbaCSL_EnetTDESSetDC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 DC)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);
    /* Disable CRC */
    AmbaEnet_TDES.TDES0.DC = DC;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
}

void AmbaCSL_EnetTDESSetLSFS(AMBA_ENET_TDES_s *pEnetTDES, UINT8 LS, UINT8 FS)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);
    AmbaEnet_TDES.TDES0.LS = LS;
    AmbaEnet_TDES.TDES0.FS = FS;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
}

void AmbaCSL_EnetTDESSetVLIC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 VLIC)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES0, &pEnetTDES->TDES0);
    AmbaEnet_TDES.TDES0.VLIC = VLIC;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES0);
    EnetWriteLong(&pEnetTDES->TDES0, val);
}

/** 0x0004        Transmit Descriptor 1 (TDES1) */
void AmbaCSL_EnetTDES1Init(AMBA_ENET_TDES_s *pEnetTDES)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES1, &val);

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES1);
    EnetWriteLong(&pEnetTDES->TDES1, val);

    return;
}

void AmbaCSL_EnetTDESSetTBS1(AMBA_ENET_TDES_s *pEnetTDES, UINT16 TBS1)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_TDES.TDES1, &pEnetTDES->TDES1);

    AmbaEnet_TDES.TDES1.TBS1 = TBS1;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_TDES.TDES1);
    EnetWriteLong(&pEnetTDES->TDES1, val);
}

/** 0x0008        TDES2       Buffer 1 Address Pointer */
void AmbaCSL_EnetTDES2Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf1)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    UINT32 Phys;

    Phys = Buf1;

    AmbaEnet_TDES.TDES2_Buf1 = Phys;
    EnetWriteLong(&pEnetTDES->TDES2_Buf1, AmbaEnet_TDES.TDES2_Buf1);
}

/** 0x000C        TDES3       Buffer 2 Address Pointer (Next Descriptor Address) */
void AmbaCSL_EnetTDES3Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf2)
{
    AMBA_REG_ENET_TDES_s AmbaEnet_TDES;
    UINT32 Phys;

    Phys = Buf2;

    AmbaEnet_TDES.TDES3_Buf2 = Phys;
    EnetWriteLong(&pEnetTDES->TDES3_Buf2, AmbaEnet_TDES.TDES3_Buf2);
}

/** 0x0000        Receive Descriptor 0 (RDES0) */
void AmbaCSL_EnetRDES0Init(AMBA_ENET_RDES_s *pEnetRDES)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES0, &val);
    AmbaEnet_RDES.RDES0.OWN = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_RDES.RDES0);
    EnetWriteLong(&pEnetRDES->RDES0, val);
}

void AmbaCSL_EnetRDESSetDmaOwn(AMBA_ENET_RDES_s *pEnetRDES)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES0, &pEnetRDES->RDES0);
    AmbaEnet_RDES.RDES0.OWN = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_RDES.RDES0);
    EnetWriteLong(&pEnetRDES->RDES0, val);
}

UINT16 AmbaCSL_EnetRDESGetFL(const AMBA_ENET_RDES_s *const pEnetRDES)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES0, &pEnetRDES->RDES0);
    return AmbaEnet_RDES.RDES0.FL;
}

UINT32 AmbaCSL_EnetRDESGetDmaOwn(const AMBA_ENET_RDES_s *const pEnetRDES)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES0, &pEnetRDES->RDES0);

    return AmbaEnet_RDES.RDES0.OWN;
}

/** 0x0004        Receive Descriptor 1 (RDES1) */
void AmbaCSL_EnetRDES1Init(AMBA_ENET_RDES_s *pEnetRDES)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES1, &val);
    AmbaEnet_RDES.RDES1.RCH = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_RDES.RDES1);
    EnetWriteLong(&pEnetRDES->RDES1, val);
}

void AmbaCSL_EnetRDESSetRBS1(AMBA_ENET_RDES_s *pEnetRDES, UINT16 RBS1)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_RDES.RDES1, &pEnetRDES->RDES1);
    AmbaEnet_RDES.RDES1.RBS1 = RBS1;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_RDES.RDES1);
    EnetWriteLong(&pEnetRDES->RDES1, val);
}

/** 0x0008        RDES2       Buffer 1 Address Pointer */
void AmbaCSL_EnetRDES2Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf1)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    UINT32 Phys;

    Phys = Buf1;

    AmbaEnet_RDES.RDES2_Buf1 = Phys;
    EnetWriteLong(&pEnetRDES->RDES2_Buf1, AmbaEnet_RDES.RDES2_Buf1);
}

/** 0x000C        RDES3       Buffer 2 Address Pointer (Next Descriptor Address) */
void AmbaCSL_EnetRDES3Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf2)
{
    AMBA_REG_ENET_RDES_s AmbaEnet_RDES;
    UINT32 Phys;

    Phys = Buf2;

    AmbaEnet_RDES.RDES3_Buf2 = Phys;
    EnetWriteLong(&pEnetRDES->RDES3_Buf2, AmbaEnet_RDES.RDES3_Buf2);
}

/** 0x0000        MAC R0         MAC Configuration Register */
void AmbaCSL_EnetMacLinkSetup(AMBA_ENET_REG_s *const pEnetReg, UINT32 SPEED, UINT32 DUPLEX)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR0, &pEnetReg->MacR0);

    switch (SPEED) {
    case SPEED_1000:
        AmbaEnet_Reg.MacR0.PS = 0U;
        break;
    case SPEED_100:
        AmbaEnet_Reg.MacR0.PS = 1U;
        AmbaEnet_Reg.MacR0.FES = 1U;
        break;
    case SPEED_10:
        AmbaEnet_Reg.MacR0.PS = 1U;
        AmbaEnet_Reg.MacR0.FES = 0U;
        break;
    default:
        /* TBD */
        break;
    }

    if (DUPLEX != DUPLEX_HALF) {
        AmbaEnet_Reg.MacR0.DO = 0U;
        AmbaEnet_Reg.MacR0.DM = 1U;
    } else {
        AmbaEnet_Reg.MacR0.DO = 1U;
        AmbaEnet_Reg.MacR0.DM = 0U;
    }

    AmbaEnet_Reg.MacR0.RE = 1U;
    AmbaEnet_Reg.MacR0.TE = 1U;
    AmbaEnet_Reg.MacR0.IPC = 1U;

    /* race condition: ifup & ifdown */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR0);
    EnetWriteLong(&pEnetReg->MacR0, val);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

void AmbaCSL_EnetMacSetJEJD(AMBA_ENET_REG_s *const pEnetReg, UINT8 JE, UINT8 JD)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR0, &pEnetReg->MacR0);

    AmbaEnet_Reg.MacR0.JE = JE;
    AmbaEnet_Reg.MacR0.JD = JD;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR0);
    EnetWriteLong(&pEnetReg->MacR0, val);
}

void AmbaCSL_EnetMacTxDisable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR0, &pEnetReg->MacR0);
    AmbaEnet_Reg.MacR0.TE = 0U;
    /* race condition: ifup & ifdown */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR0);
    EnetWriteLong(&pEnetReg->MacR0, val);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

void AmbaCSL_EnetMacRxDisable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR0, &pEnetReg->MacR0);
    AmbaEnet_Reg.MacR0.RE = 0U;
    /* race condition: ifup & ifdown */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR0);
    EnetWriteLong(&pEnetReg->MacR0, val);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

/** 0x0004        MAC R1         MAC Frame Filter */
void AmbaCSL_EnetMacInitR1(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR1, &val);
    AmbaEnet_Reg.MacR1.HMC = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR1);
    EnetWriteLong(&pEnetReg->MacR1, val);
}

void AmbaCSL_EnetMacSetHMCPM(AMBA_ENET_REG_s *const pEnetReg, UINT8 HMC, UINT8 PM)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR1, &pEnetReg->MacR1);
    AmbaEnet_Reg.MacR1.HMC = HMC;
    AmbaEnet_Reg.MacR1.PM = PM;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR1);
    EnetWriteLong(&pEnetReg->MacR1, val);
}

/** 0x0008        MAC R2         Hash Table High Register */
void AmbaCSL_EnetMacSetHashHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTH)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR2_HTH = pEnetReg->MacR2_HTH;
    AmbaEnet_Reg.MacR2_HTH |= HTH;
    /* race condition: multicast filter */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    EnetWriteLong(&pEnetReg->MacR2_HTH, AmbaEnet_Reg.MacR2_HTH);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

void AmbaCSL_EnetMacClearHashHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTH)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR2_HTH = pEnetReg->MacR2_HTH;
    AmbaEnet_Reg.MacR2_HTH &= ~HTH;
    /* race condition: multicast filter */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    EnetWriteLong(&pEnetReg->MacR2_HTH, AmbaEnet_Reg.MacR2_HTH);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

/** 0x000C        MAC R3         Hash Table Low Register */
void AmbaCSL_EnetMacSetHashLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTL)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR3_HTL = pEnetReg->MacR3_HTL;
    AmbaEnet_Reg.MacR3_HTL |= HTL;
    /* race condition: multicast filter */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    EnetWriteLong(&pEnetReg->MacR3_HTL, AmbaEnet_Reg.MacR3_HTL);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

void AmbaCSL_EnetMacClearHashLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTL)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR3_HTL = pEnetReg->MacR3_HTL;
    AmbaEnet_Reg.MacR3_HTL &= ~HTL;
    /* race condition: multicast filter */
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowLock();
#endif
    EnetWriteLong(&pEnetReg->MacR3_HTL, AmbaEnet_Reg.MacR3_HTL);
#if defined(ENET_ASIL)
    AmbaSafety_EnetShadowUnLock();
#endif
}

/** 0x0010        MAC R4         GMII Address Register */
void AmbaCSL_EnetMacMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR4, &pEnetReg->MacR4);

    while (pEnetReg->MacR4.GB == 0x1U)
    {;}

    AmbaEnet_Reg.MacR4.GB = 1U;
    /* The CSR clock frequency is 250~300 MHz and the MDC clock is CSR clock/124 */
    AmbaEnet_Reg.MacR4.CR = 5U;
    AmbaEnet_Reg.MacR4.PA = PA;
    AmbaEnet_Reg.MacR4.GR = GR;
    AmbaEnet_Reg.MacR4.GW = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR4);
    EnetWriteLong(&pEnetReg->MacR4, val);

    /* wait for its completion */
    while (pEnetReg->MacR4.GB == 0x1U)
    {;}
}

void AmbaCSL_EnetMacMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR4, &pEnetReg->MacR4);

    while (pEnetReg->MacR4.GB == 0x1U)
    {;}

    AmbaEnet_Reg.MacR4.GB = 1U;
    /* The CSR clock frequency is 250~300 MHz and the MDC clock is CSR clock/124 */
    AmbaEnet_Reg.MacR4.CR = 5U;
    AmbaEnet_Reg.MacR4.PA = PA;
    AmbaEnet_Reg.MacR4.GR = GR;
    AmbaEnet_Reg.MacR4.GW = 0U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR4);
    EnetWriteLong(&pEnetReg->MacR4, val);

    /* wait for its completion */
    while (pEnetReg->MacR4.GB == 0x1U)
    {;}
}

/** 0x0014        MAC R5         GMII Data Register */
void AmbaCSL_EnetMacSetMiiData(AMBA_ENET_REG_s *const pEnetReg, UINT32 GB)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    while (pEnetReg->MacR4.GB == 0x1U)
    {;}

    AmbaEnet_Reg.MacR5_GB = GB;
    EnetWriteLong(&pEnetReg->MacR5_GB, AmbaEnet_Reg.MacR5_GB);
}

UINT32 AmbaCSL_EnetMacGetMiiData(AMBA_ENET_REG_s *const pEnetReg)
{
    while (pEnetReg->MacR4.GB == 0x1U)
    {;}

    EnetDebug("ENET %s(%d) readl %p 0x%08x", __func__, __LINE__, &pEnetReg->MacR5_GB, pEnetReg->MacR5_GB);
    return pEnetReg->MacR5_GB;
}

/** 0x0018        MAC R6         Flow Control Register */
void AmbaCSL_EnetMacInitR6(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    /** (512 bits / N) * pause_time = actual pause time
     * ex:
     *     512 bits / 1 Gbps * 1954 = ~0.0010 sec = 1 ms
     *     512 bits / 100 Mbps * 1954 = ~0.010 sec = 10 ms
     */
    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR6, &val);
    AmbaEnet_Reg.MacR6.PLT = 0x3U;
    AmbaEnet_Reg.MacR6.PT = 1954U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR6);
    EnetWriteLong(&pEnetReg->MacR6, val);
}

/** 0x0020        MAC R8         Version Register */
UINT32 AmbaCSL_EnetMacGetMacR8(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->MacR8_VER;
}

/** 0x0038        MAC R14        Interrupt Status Register */
UINT32 AmbaCSL_EnetMacGetMacR14(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR14);
    return val;
}

/** 0x003C        MAC R15        Interrupt Mask Register */
void AmbaCSL_EnetMacInitR15(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR15, &val);
//    AmbaEnet_Reg.MacR15.RGSMIIIM = 1U;
//    AmbaEnet_Reg.MacR15.PCSLCHGIM = 1U;
//    AmbaEnet_Reg.MacR15.PCSANCIM = 1U;
//    AmbaEnet_Reg.MacR15.PMTIM = 1U;
    /* do no generate Timestamp Interrupt */
    AmbaEnet_Reg.MacR15.TSIM = 1U;
    /* do no generate Low Power Idle Interrupt */
    AmbaEnet_Reg.MacR15.LPIIM = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR15);
    EnetWriteLong(&pEnetReg->MacR15, val);
}

UINT32 AmbaCSL_EnetMacGetMacR15(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR15);
    return val;
}

void AmbaCSL_EnetMacSetTSIM(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSIM)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR15, &pEnetReg->MacR15);
    AmbaEnet_Reg.MacR15.TSIM = TSIM;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR15);
    EnetWriteLong(&pEnetReg->MacR15, val);
}

/** 0x0040        MAC R16        MAC Address0 High Register */
void AmbaCSL_EnetMacSetAddrHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRHI)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR16_ADDRHI = ADDRHI;
    EnetWriteLong(&pEnetReg->MacR16_ADDRHI, AmbaEnet_Reg.MacR16_ADDRHI);
}

UINT32 AmbaCSL_EnetMacGetAddrHi(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->MacR16_ADDRHI;
}

/** 0x0044        MAC R17        MAC Address0 Low Register */
void AmbaCSL_EnetMacSetAddrLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRLO)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR17_ADDRLO = ADDRLO;
    EnetWriteLong(&pEnetReg->MacR17_ADDRLO, AmbaEnet_Reg.MacR17_ADDRLO);
}

UINT32 AmbaCSL_EnetMacGetAddrLo(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->MacR17_ADDRLO;
}

/** 0x00C4        MAC R49        AN Status Register */
UINT32 AmbaCSL_EnetMacGetMacR49(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR49);
    return val;
}

/** 0x00D8        MAC R54        SGMII/RGMII/SMII Control and Status Register */
UINT32 AmbaCSL_EnetMacGetMacR54(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR54);
    return val;
}

/** 0x00E0        MAC R56        General Purpose IO Register */
UINT32 AmbaCSL_EnetMacGetMacR56(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR56);
    return val;
}

/** 0x0584        MAC R353       VLAN Tag Inclusion or Replacement Register */
void AmbaCSL_EnetMacSetVLAN(AMBA_ENET_REG_s *const pEnetReg, UINT8 VLC, UINT16 VLT)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR353, &pEnetReg->MacR353);
    AmbaEnet_Reg.MacR353.VLP = 1U;
    AmbaEnet_Reg.MacR353.VLC = VLC;
    AmbaEnet_Reg.MacR353.VLT = VLT;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR353);
    EnetWriteLong(&pEnetReg->MacR353, val);
}

/** 0x0700        MAC R448       Timestamp Control Register */
/**
 *  Enable Timestamp Snapshot for
 *  Packet type:
 *    SYNC, Follow_Up, Delay_Req, Delay_Resp,
 *    Pdelay_Req, Pdelay_Resp, Pdelay_Resp_Follow_Up
 *  Transport:
 *    L2, IPv4UDP
 */
void AmbaCSL_EnetMacInitR448(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR448, &val);

    AmbaEnet_Reg.MacR448.TSENA = 1U;
    AmbaEnet_Reg.MacR448.TSCFUPDT = 1U;
#if 0
    /* Enable Timestamp for All Frames */
    AmbaEnet_Reg.MacR448.TSENALL = 1U;
#endif
    AmbaEnet_Reg.MacR448.TSCTRLSSR = 1U;
    AmbaEnet_Reg.MacR448.TSVER2ENA = 1U;
    AmbaEnet_Reg.MacR448.TSIPENA = 1U;
//    AmbaEnet_Reg.MacR448.TSIPV6ENA = 1U;
    AmbaEnet_Reg.MacR448.TSIPV4ENA = 1U;

    AmbaEnet_Reg.MacR448.SNAPTYPSEL =  1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR448);
    EnetWriteLong(&pEnetReg->MacR448, val);
}

void AmbaCSL_EnetMacSetTSADDREG(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR448, &pEnetReg->MacR448);

    AmbaEnet_Reg.MacR448.TSADDREG = 1U;

    while (pEnetReg->MacR448.TSADDREG == 0x1U)
    {;}

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR448);
    EnetWriteLong(&pEnetReg->MacR448, val);
    /* wait for MacR454_TSAR to apply */
    while (pEnetReg->MacR448.TSADDREG == 0x1U)
    {;}
}

void AmbaCSL_EnetMacTSINIT(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR448, &pEnetReg->MacR448);

    AmbaEnet_Reg.MacR448.TSINIT = 1U;

    while (pEnetReg->MacR448.TSINIT == 0x1U)
    {;}

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR448);
    EnetWriteLong(&pEnetReg->MacR448, val);

    /* wait for R452 TSS, R453 TSSS to apply */
    while (pEnetReg->MacR448.TSINIT == 0x1U)
    {;}
}

void AmbaCSL_EnetMacTSUPDT(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR448, &pEnetReg->MacR448);

    AmbaEnet_Reg.MacR448.TSUPDT = 1U;

    while (pEnetReg->MacR448.TSUPDT == 0x1U)
    {;}
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR448);
    EnetWriteLong(&pEnetReg->MacR448, val);
    /* wait for R452 TSS, R453 TSSS to apply */
    while (pEnetReg->MacR448.TSUPDT == 0x1U)
    {;}
}

void AmbaCSL_EnetMacSetTSTRIG(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSTRIG)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR448, &pEnetReg->MacR448);
    AmbaEnet_Reg.MacR448.TSTRIG = TSTRIG;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR448);
    EnetWriteLong(&pEnetReg->MacR448, val);
}

/** 0x0704        MAC R449       Sub-Second Increment Register */
void AmbaCSL_EnetMacSetSSINC(AMBA_ENET_REG_s *const pEnetReg, UINT32 SSINC)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR449_SSINC = SSINC;
    EnetWriteLong(&pEnetReg->MacR449_SSINC, AmbaEnet_Reg.MacR449_SSINC);
}

/** 0x0708        MAC R450       System Time - Seconds Register */
UINT32 AmbaCSL_EnetMacGetPhcTsAddr(AMBA_ENET_REG_s *const pEnetReg, volatile void **Addr)
{
    EnetDebug("ENET %s(%d) readl %p 0x%08x", __func__, __LINE__, &pEnetReg->MacR450_TSS, pEnetReg->MacR450_TSS);
    *Addr = &(pEnetReg->MacR450_TSS);
    return 0;
}

UINT32 AmbaCSL_EnetMacGetPhcSec(AMBA_ENET_REG_s *const pEnetReg)
{
    EnetDebug("ENET %s(%d) readl %p 0x%08x", __func__, __LINE__, &pEnetReg->MacR450_TSS, pEnetReg->MacR450_TSS);
    return pEnetReg->MacR450_TSS;
}

/** 0x070C        MAC R451       System Time - Nanoseconds Register */
UINT32 AmbaCSL_EnetMacGetPhcNs(AMBA_ENET_REG_s *const pEnetReg)
{
    EnetDebug("ENET %s(%d) readl %p 0x%08x", __func__, __LINE__, &pEnetReg->MacR451_TSSS, pEnetReg->MacR451_TSSS);
    return pEnetReg->MacR451_TSSS;
}

/** 0x0710        MAC R452       System Time - Seconds Update Register */
void AmbaCSL_EnetMacSetTSS(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSS)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR452_TSS = TSS;
    EnetWriteLong(&pEnetReg->MacR452_TSS, AmbaEnet_Reg.MacR452_TSS);
}

/** 0x0714        MAC R453       System Time - Nanoseconds Update Register */
void AmbaCSL_EnetMacSetTSSS(AMBA_ENET_REG_s *const pEnetReg, UINT8 ADDSUB, UINT32 TSSS)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaEnet_Reg.MacR453.TSSS = TSSS;
    AmbaEnet_Reg.MacR453.ADDSUB = ADDSUB;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR453);
    EnetWriteLong(&pEnetReg->MacR453, val);
}

/** 0x0718        MAC R454       Timestamp Addend Register */
UINT32 AmbaCSL_EnetMacGetTSAR(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->MacR454_TSAR;
}

void AmbaCSL_EnetMacSetTSAR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSAR)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR454_TSAR = TSAR;
    EnetWriteLong(&pEnetReg->MacR454_TSAR, AmbaEnet_Reg.MacR454_TSAR);
}

/** 0x071C        MAC R455       Target Time Seconds Register */
void AmbaCSL_EnetMacSetTSTR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSTR)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.MacR455_TSTR = TSTR;
    EnetWriteLong(&pEnetReg->MacR455_TSTR, AmbaEnet_Reg.MacR455_TSTR);
}

/** 0x0720        MAC R456       Target Time Nanoseconds Register */
void AmbaCSL_EnetMacSetTTSLO(AMBA_ENET_REG_s *const pEnetReg, UINT32 TTSLO)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    /* wait for clock domain sync */
    while (pEnetReg->MacR456.TRGTBUSY == 0x1U)
    {;}

    AmbaEnet_Reg.MacR456.TTSLO = TTSLO;
    AmbaEnet_Reg.MacR456.TRGTBUSY = 0U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR456);
    EnetWriteLong(&pEnetReg->MacR456, val);
}


/** 0x0724        MAC R457       System Time - Higher Word Seconds Register */
/** 0x0728        MAC R458       Timestamp Status Register */
UINT32 AmbaCSL_EnetMacGetMacR458(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->MacR458);
    return val;
}

/** 0x072C        MAC R459       PPS Control Register */
void AmbaCSL_EnetMacSetPPSCTRL0(AMBA_ENET_REG_s *const pEnetReg, UINT8 PPSCTRL0)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR459, &pEnetReg->MacR459);
    AmbaEnet_Reg.MacR459.PPSEN0 = 0U;
    AmbaEnet_Reg.MacR459.PPSCTRL0 = PPSCTRL0;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR459);
    EnetWriteLong(&pEnetReg->MacR459, val);
}

void AmbaCSL_EnetMacSetPPSCMD0(AMBA_ENET_REG_s *const pEnetReg, UINT8 PPSCMD0)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR459, &pEnetReg->MacR459);
    AmbaEnet_Reg.MacR459.PPSEN0 = 1U;
    AmbaEnet_Reg.MacR459.PPSCTRL0 = PPSCMD0;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR459);
    EnetWriteLong(&pEnetReg->MacR459, val);
    /* wait for its completion */
    while (pEnetReg->MacR459.PPSCTRL0 != 0x0U)
    {;}
}

void AmbaCSL_EnetMacSetTRGTMODSEL0(AMBA_ENET_REG_s *const pEnetReg, UINT8 TRGTMODSEL0)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.MacR459, &pEnetReg->MacR459);
    AmbaEnet_Reg.MacR459.TRGTMODSEL0 = TRGTMODSEL0;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.MacR459);
    EnetWriteLong(&pEnetReg->MacR459, val);
}

/** 0x0730        MAC R460       Auxiliary Timestamp - Nanoseconds Register */
/** 0x0734        MAC R461       Auxiliary Timestamp - Seconds Register */

/** 0x0760        MAC R472       PPS0 Interval Register */
void AmbaCSL_EnetMacSetPPSINT(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSINT)
{
    EnetWriteLong(&pEnetReg->MacR472_PPSINT, PPSINT);
}

/** 0x0764        MAC R473       PPS0 Width Register */
void AmbaCSL_EnetMacSetPPSWIDTH(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSWIDTH)
{
    EnetWriteLong(&pEnetReg->MacR473_PPSWIDTH, PPSWIDTH);
}


/** 0x1000        DMA R0         Bus Mode Register */
UINT32 AmbaCSL_EnetDmaReset(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 Count = 0U, Ret;
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR0, &pEnetReg->DmaR0);
    AmbaEnet_Reg.DmaR0.SWR = 1U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR0);
    EnetWriteLong(&pEnetReg->DmaR0, val);

    /* wait for its completion */
    while ((pEnetReg->DmaR0.SWR == 0x1U) && (Count < 2000000U)) {
        Count++;
    }
    if (pEnetReg->DmaR0.SWR == 0x0U) {
        Ret = 0U;
    } else {
        Ret = 1U;
    }
    return Ret;
}

void AmbaCSL_EnetDmaInitR0(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR0, &val);
    AmbaEnet_Reg.DmaR0.DA = 1U;
    AmbaEnet_Reg.DmaR0.FB = 1U;
    AmbaEnet_Reg.DmaR0.PBL = 32;
    AmbaEnet_Reg.DmaR0.ATDS = 1U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR0);
    EnetWriteLong(&pEnetReg->DmaR0, val);
}

/** 0x1004        DMA R1         Transmit Poll Demand Register */
void AmbaCSL_EnetDmaSetTxPoll(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.DmaR1_TPD = 1U;
    EnetWriteLong(&pEnetReg->DmaR1_TPD, AmbaEnet_Reg.DmaR1_TPD);
}

/** 0x1008        DMA R2         Receive Poll Demand Register */
void AmbaCSL_EnetDmaSetRxPoll(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;

    AmbaEnet_Reg.DmaR2_RPD = 1U;
    EnetWriteLong(&pEnetReg->DmaR2_RPD, AmbaEnet_Reg.DmaR2_RPD);
}

/** 0x100C        DMA R3         Receive Descriptor List Address Register */
void AmbaCSL_EnetDmaSetRxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 RDESLA)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    UINT32 Phys;

    Phys = RDESLA;

    AmbaEnet_Reg.DmaR3_RDESLA = Phys;
    EnetWriteLong(&pEnetReg->DmaR3_RDESLA, AmbaEnet_Reg.DmaR3_RDESLA);
}

/** 0x1010        DMA R4         Transmit Descriptor List Address Register */
void AmbaCSL_EnetDmaSetTxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 TDESLA)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    UINT32 Phys;

    Phys = TDESLA;

    AmbaEnet_Reg.DmaR4_TDESLA = Phys;
    EnetWriteLong(&pEnetReg->DmaR4_TDESLA, AmbaEnet_Reg.DmaR4_TDESLA);
}

/** 0x1014        DMA R5         Status Register */
UINT32 AmbaCSL_EnetDmaGetStatus(const AMBA_ENET_REG_s *const pEnetReg)
{
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&val, &pEnetReg->DmaR5);
    return val;
}

void AmbaCSL_EnetDmaClearStatus(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR5, &pEnetReg->DmaR5);
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR5);
    WriteLongIsr(&pEnetReg->DmaR5, val);
}

void AmbaCSL_EnetTDESDumpStatus(const AMBA_ENET_TDES_s *pEnetTDES, AMBA_ENET_STAT_s *pEnetStat)
{
    const AMBA_REG_ENET_TDES_s *pEnetTDES_Copy;

#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pEnetTDES_Copy, &pEnetTDES);
#else
    pEnetTDES_Copy = (const AMBA_REG_ENET_TDES_s *) pEnetTDES;
#endif

    if (pEnetTDES_Copy->TDES0.OWN == 0U) {
        pEnetStat->TDES1_TBS1 += pEnetTDES_Copy->TDES1.TBS1;              /**< [12:0] TBS1: Transmit Buffer 1 Size */

        if (pEnetTDES_Copy->TDES0.ES != 0U) {                             /**< [15] ES: Error Summary */
            pEnetStat->TDES0_ES++;

            if (pEnetTDES_Copy->TDES0.UF != 0U) {                         /**< [1] UF: Underflow Error */
                EnetDebug("TDES0_UF=0x%x", pEnetTDES_Copy->TDES0.UF);
                pEnetStat->TDES0_UF++;
            }
            if (pEnetTDES_Copy->TDES0.ED != 0U) {                         /**< [2] ED: Excessive Deferral */
                EnetDebug("TDES0_ED=0x%x", pEnetTDES_Copy->TDES0.ED);
                pEnetStat->TDES0_ED++;
            }
            if (pEnetTDES_Copy->TDES0.EC != 0U) {                         /**< [8] EC: Excessive Collision */
                EnetDebug("TDES0_EC=0x%x", pEnetTDES_Copy->TDES0.EC);
                pEnetStat->TDES0_EC++;
            }
            if (pEnetTDES_Copy->TDES0.LC != 0U) {                         /**< [9] LC: Late Collision */
                EnetDebug("TDES0_LC=0x%x", pEnetTDES_Copy->TDES0.LC);
                pEnetStat->TDES0_LC++;
            }
            if (pEnetTDES_Copy->TDES0.NC != 0U) {                         /**< [10] NC: No Carrier */
                EnetDebug("TDES0_NC=0x%x", pEnetTDES_Copy->TDES0.NC);
                pEnetStat->TDES0_NC++;
            }
            if (pEnetTDES_Copy->TDES0.LOC != 0U) {                        /**< [11] LOC: Loss of Carrier */
                EnetDebug("TDES0_LOC=0x%x", pEnetTDES_Copy->TDES0.LOC);
                pEnetStat->TDES0_LOC++;
            }
            if (pEnetTDES_Copy->TDES0.IPE != 0U) {                        /**< [12] IPE: IP Payload Error */
                EnetDebug("TDES0_IPE=0x%x", pEnetTDES_Copy->TDES0.IPE);
                pEnetStat->TDES0_IPE++;
            }
            if (pEnetTDES_Copy->TDES0.FF != 0U) {                         /**< [13] FF: Frame Flushed */
                EnetDebug("TDES0_FF=0x%x", pEnetTDES_Copy->TDES0.FF);
                pEnetStat->TDES0_FF++;
            }
            if (pEnetTDES_Copy->TDES0.JT != 0U) {                         /**< [14] JT: Jabber Timeout */
                EnetDebug("TDES0_JT=0x%x", pEnetTDES_Copy->TDES0.JT);
                pEnetStat->TDES0_JT++;
            }
            if (pEnetTDES_Copy->TDES0.IHE != 0U) {                        /**< [16] IHE: IP Header Error */
                EnetDebug("TDES0_IHE=0x%x", pEnetTDES_Copy->TDES0.IHE);
                pEnetStat->TDES0_IHE++;
            }
        } else {
            pEnetStat->TDES0_OWN++;                                       /**< [31] OWN: Own Bit */
        }
    }
}

void AmbaCSL_EnetRDESDumpStatus(const AMBA_ENET_RDES_s *pEnetRDES, AMBA_ENET_STAT_s *pEnetStat)
{
    const AMBA_REG_ENET_RDES_s *pEnetRDES_Copy;

#if defined(AMBA_MISRA_FIX_H)
    AmbaMisra_TypeCast(&pEnetRDES_Copy, &pEnetRDES);
#else
    pEnetRDES_Copy = (const AMBA_REG_ENET_RDES_s *) pEnetRDES;
#endif

    if (pEnetRDES_Copy->RDES0.OWN == 0U) {
        pEnetStat->RDES1_RBS1 += pEnetRDES_Copy->RDES1.RBS1;              /**< [12:0] RBS1: Receive Buffer 1 Size */

        if (pEnetRDES_Copy->RDES0.ES != 0U) {                             /**< [15] ES: Error Summary */
            pEnetStat->RDES0_ES++;

            if (pEnetRDES_Copy->RDES0.CE != 0U) {                         /**< [1] CE: CRC Error */
                EnetDebug("RDES0_CE=0x%x", pEnetRDES_Copy->RDES0.CE);
                pEnetStat->RDES0_CE++;
            }
            if (pEnetRDES_Copy->RDES0.RE != 0U) {                         /**< [3] RE: Receive Error */
                EnetDebug("RDES0_RE=0x%x", pEnetRDES_Copy->RDES0.RE);
                pEnetStat->RDES0_RE++;
            }
            if (pEnetRDES_Copy->RDES0.RWT != 0U) {                        /**< [4] RWT: Receive Watchdog Timeout */
                EnetDebug("RDES0_RWT=0x%x", pEnetRDES_Copy->RDES0.RWT);
                pEnetStat->RDES0_RWT++;
            }
            if (pEnetRDES_Copy->RDES0.LC != 0U) {                         /**< [6] LC: Late Collision */
                EnetDebug("RDES0_LC=0x%x", pEnetRDES_Copy->RDES0.LC);
                pEnetStat->RDES0_LC++;
            }
            if (pEnetRDES_Copy->RDES0.Timestamp != 0U) {                      /**< [7] Timestamp Available, IP Checksum Error (Type1), or Giant Frame */
                EnetDebug("RDES0_TIMESTAMP=0x%x", pEnetRDES_Copy->RDES0.Timestamp);
                pEnetStat->RDES0_TIMESTAMP++;
            }
            if (pEnetRDES_Copy->RDES0.OE != 0U) {                         /**< [11] OE: Overflow Error */
                EnetDebug("RDES0_OE=0x%x", pEnetRDES_Copy->RDES0.OE);
                pEnetStat->RDES0_OE++;
            }
            if (pEnetRDES_Copy->RDES0.DE != 0U) {                         /**< [14] DE: Descriptor Error */
                EnetDebug("RDES0_DE=0x%x", pEnetRDES_Copy->RDES0.DE);
                pEnetStat->RDES0_DE++;
            }
            if (pEnetRDES_Copy->RDES4.IPHR != 0U) {                       /**< [3] IP Header Error */
                EnetDebug("RDES4_IPHR=0x%x", pEnetRDES_Copy->RDES4.IPHR);
                pEnetStat->RDES4_IPHR++;
            }
            if (pEnetRDES_Copy->RDES4.IPPE != 0U) {                       /**< [4] IP Payload Error */
                EnetDebug("RDES4_IPPE=0x%x", pEnetRDES_Copy->RDES4.IPPE);
                pEnetStat->RDES4_IPPE++;
            }
        } else {
            pEnetStat->RDES0_OWN++;                                       /**< [31] OWN: Own Bit */
        }
    }
}

void AmbaCSL_EnetDmaDumpStatus(const AMBA_ENET_REG_s *const pEnetReg, AMBA_ENET_STAT_s *pEnetStat)
{
    if (pEnetReg->DmaR5.NIS != 0U) {                            /**< [16] NIS Normal Interrupt Summary */
        pEnetStat->NIS++;
        if (pEnetReg->DmaR5.TI != 0U) {                         /**< [0] TI Transmit Interrupt */
            EnetIsrDebug("TI");
            pEnetStat->TI++;
        } else {
            if (pEnetReg->DmaR5.TU != 0U) {                     /**< [2] TU Transmit Buffer Unavailable */
                EnetIsrDebug("TS=0x%x", pEnetReg->DmaR5.TS);    /**< [22:20] TS Transmit Process State */
                pEnetStat->TU++;
            }
        }
        if (pEnetReg->DmaR5.RI != 0U) {                         /**< [6] RI Receive Interrupt */
            EnetIsrDebug("RI");
            pEnetStat->RI++;
        }
        if (pEnetReg->DmaR5.ERI != 0U) {                        /**< [14] ERI Early Receive Interrupt */
            EnetIsrDebug("ERI");
            pEnetStat->ERI++;
        }
    }
    if (pEnetReg->DmaR5.AIS != 0U) {                            /**< [15] AIS Abnormal Interrupt Summary */
        pEnetStat->AIS++;
        if (pEnetReg->DmaR5.TPS != 0U) {                        /**< [1] TPS Transmit Process Stopped */
            EnetIsrDebug("TPS");
            pEnetStat->TPS++;
        }
        if (pEnetReg->DmaR5.TJT != 0U) {                        /**< [3] TJT Transmit Jabber Timeout */
            EnetIsrDebug("TJT");
            pEnetStat->TJT++;
        }
        if (pEnetReg->DmaR5.OVF != 0U) {                        /**< [4] OVF Receive Overflow */
            EnetIsrDebug("OVF");
            pEnetStat->OVF++;
        }
        if (pEnetReg->DmaR5.UNF != 0U) {                        /**< [5] UNF Transmit Underflow */
            EnetIsrDebug("UNF");
            pEnetStat->UNF++;
        }
        if (pEnetReg->DmaR5.RU != 0U) {                         /**< [7] RU Receive Buffer Unavailable */
            EnetIsrDebug("RS=0x%x", pEnetReg->DmaR5.RS);        /**< [19:17] RS Receive Process State */
            pEnetStat->RU++;
        }
        if (pEnetReg->DmaR5.RPS != 0U) {                        /**< [8] RPS Receive Process Stopped */
            EnetIsrDebug("RPS");
            pEnetStat->RPS++;
        }
        if (pEnetReg->DmaR5.RWT != 0U) {                        /**< [9] RWT Receive Watchdog Timeout */
            EnetIsrDebug("RWT");
            pEnetStat->RWT++;
        }
        if (pEnetReg->DmaR5.ETI != 0U) {                        /**< [10] ETI Early Transmit Interrupt */
            EnetIsrDebug("ETI");
            pEnetStat->ETI++;
        }
        if (pEnetReg->DmaR5.FBI != 0U) {                        /**< [13] FBI Fatal Bus Error Interrupt */
            EnetIsrDebug("EB=0x%x", pEnetReg->DmaR5.EB);        /**< [25:23] EB Error Bits */
            pEnetStat->FBI++;
        }
    }

    if (pEnetReg->DmaR5.GMI != 0U) {                            /**< [27] GMI GMAC MMC Interrupt */
        EnetIsrDebug("GMI");
    }
    if (pEnetReg->DmaR5.GPI != 0U) {                            /**< [28] GPI GMAC PMT Interrupt */
        EnetIsrDebug("GPI");
    }
    if (pEnetReg->DmaR5.GLI != 0U) {                            /**< [26] GLI GMAC Line Interface Interrupt */
        EnetIsrDebug("GLI");
    }
    if (pEnetReg->DmaR5.TTI != 0U) {                            /**< [29] TTI Timestamp Trigger Interrupt */
        EnetIsrDebug("TTI");
    }
}

UINT32 AmbaCSL_EnetDmaGetTI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.TI;
}

UINT32 AmbaCSL_EnetDmaGetTU(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.TU;
}

UINT32 AmbaCSL_EnetDmaGetRI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.RI;
}

UINT32 AmbaCSL_EnetDmaGetRU(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.RU;
}

UINT32 AmbaCSL_EnetDmaGetGLI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.GLI;
}

UINT32 AmbaCSL_EnetDmaGetTTI(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR5.TTI;
}

/** 0x1018        DMA R6         Operation Mode Register */
void AmbaCSL_EnetDmaStartTX(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR6, &pEnetReg->DmaR6);
    AmbaEnet_Reg.DmaR6.ST = 1U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR6);
    EnetWriteLong(&pEnetReg->DmaR6, val);
}

void AmbaCSL_EnetDmaStartRX(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR6, &pEnetReg->DmaR6);
    AmbaEnet_Reg.DmaR6.SR = 1U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR6);
    EnetWriteLong(&pEnetReg->DmaR6, val);
}

void AmbaCSL_EnetDmaStopTX(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR6, &pEnetReg->DmaR6);
    AmbaEnet_Reg.DmaR6.ST = 0U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR6);
    EnetWriteLong(&pEnetReg->DmaR6, val);
}

void AmbaCSL_EnetDmaStopRX(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR6, &pEnetReg->DmaR6);
    AmbaEnet_Reg.DmaR6.SR = 0U;
    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR6);
    EnetWriteLong(&pEnetReg->DmaR6, val);
}

void AmbaCSL_EnetDmaInitR6(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR6, &val);
    AmbaEnet_Reg.DmaR6.FUF = 1U;
    AmbaEnet_Reg.DmaR6.TTC = 3U;
    AmbaEnet_Reg.DmaR6.TSF = 1U;
    AmbaEnet_Reg.DmaR6.RSF = 1U;
    /* Operate on Second Frame */
    AmbaEnet_Reg.DmaR6.OSF = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR6);
    EnetWriteLong(&pEnetReg->DmaR6, val);
}

/** 0x101C        DMA R7         Interrupt Enable Register */
void AmbaCSL_EnetDmaInitR7(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR7, &val);
    /* Normal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.NIE = 1U;
    /* Abnormal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.AIE = 1U;
    /* Fatal Bus Error Enable */
    AmbaEnet_Reg.DmaR7.FBE = 1U;

    /* rx */
    AmbaEnet_Reg.DmaR7.RWE = 0U;
    AmbaEnet_Reg.DmaR7.RSE = 0U;
    AmbaEnet_Reg.DmaR7.RUE = 0U;
    AmbaEnet_Reg.DmaR7.RIE = 0U;
    AmbaEnet_Reg.DmaR7.OVE = 0U;

    /* tx */
    AmbaEnet_Reg.DmaR7.UNE = 1U;
    AmbaEnet_Reg.DmaR7.TJE = 1U;
    AmbaEnet_Reg.DmaR7.TUE = 1U;
    AmbaEnet_Reg.DmaR7.TSE = 1U;
    AmbaEnet_Reg.DmaR7.TIE = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR7);
    EnetWriteLong(&pEnetReg->DmaR7, val);
}

void AmbaCSL_EnetDmaTxIrqDisable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR7, &pEnetReg->DmaR7);

    AmbaEnet_Reg.DmaR7.UNE = 0U;
    AmbaEnet_Reg.DmaR7.TJE = 0U;
    AmbaEnet_Reg.DmaR7.TUE = 0U;
    AmbaEnet_Reg.DmaR7.TSE = 0U;
    AmbaEnet_Reg.DmaR7.TIE = 0U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR7);
    WriteLongIsr(&pEnetReg->DmaR7, val);
}

void AmbaCSL_EnetDmaTxIrqEnable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR7, &pEnetReg->DmaR7);
    /* Normal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.NIE = 1U;
    /* Abnormal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.AIE = 1U;
    /* Fatal Bus Error Enable */
    AmbaEnet_Reg.DmaR7.FBE = 1U;

    /* tx */
    AmbaEnet_Reg.DmaR7.UNE = 1U;
    AmbaEnet_Reg.DmaR7.TJE = 1U;
    AmbaEnet_Reg.DmaR7.TUE = 1U;
    AmbaEnet_Reg.DmaR7.TSE = 1U;
    AmbaEnet_Reg.DmaR7.TIE = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR7);
    EnetWriteLong(&pEnetReg->DmaR7, val);
}

void AmbaCSL_EnetDmaRxIrqDisable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR7, &pEnetReg->DmaR7);
    AmbaEnet_Reg.DmaR7.RWE = 0U;
    AmbaEnet_Reg.DmaR7.RSE = 0U;
    AmbaEnet_Reg.DmaR7.RUE = 0U;
    AmbaEnet_Reg.DmaR7.RIE = 0U;
    AmbaEnet_Reg.DmaR7.OVE = 0U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR7);
    WriteLongIsr(&pEnetReg->DmaR7, val);
}

void AmbaCSL_EnetDmaRxIrqEnable(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR7, &pEnetReg->DmaR7);
    /* Normal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.NIE = 1U;
    /* Abnormal Interrupt Summary */
    AmbaEnet_Reg.DmaR7.AIE = 1U;
    /* Fatal Bus Error Enable */
    AmbaEnet_Reg.DmaR7.FBE = 1U;

    /* rx */
    AmbaEnet_Reg.DmaR7.RWE = 1U;
    AmbaEnet_Reg.DmaR7.RSE = 1U;
    AmbaEnet_Reg.DmaR7.RUE = 1U;
    AmbaEnet_Reg.DmaR7.RIE = 1U;
    AmbaEnet_Reg.DmaR7.OVE = 1U;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR7);
    EnetWriteLong(&pEnetReg->DmaR7, val);
}

/** 0x1028        DMA R10        AXI Bus Mode Register */
void AmbaCSL_EnetDmaInitR10(AMBA_ENET_REG_s *const pEnetReg)
{
    AMBA_ENET_REG_s AmbaEnet_Reg;
    volatile UINT32 val = 0U;

    AmbaMisra_VolatileCast32(&AmbaEnet_Reg.DmaR10, &val);
    AmbaEnet_Reg.DmaR10.BLEN4 = 1U;
    AmbaEnet_Reg.DmaR10.BLEN8 = 1U;
    AmbaEnet_Reg.DmaR10.BLEN16 = 1U;
    AmbaEnet_Reg.DmaR10.BLEN32 = 1U;
    AmbaEnet_Reg.DmaR10.BLEN64 = 1U;
    AmbaEnet_Reg.DmaR10.BLEN128 = 1U;
    AmbaEnet_Reg.DmaR10.RD_OSR_LMT = 0xfU;
    AmbaEnet_Reg.DmaR10.WR_OSR_LMT = 0xfU;

    AmbaMisra_VolatileCast32(&val, &AmbaEnet_Reg.DmaR10);
    EnetWriteLong(&pEnetReg->DmaR10, val);
}

/** 0x1048        DMA R18        Current Host Transmit Descriptor Register */
UINT32 AmbaCSL_EnetDmaGetDmaCurTxDes(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR18_CURTDESAPTR;
}

/** 0x104C        DMA R19        Current Host Receive Descriptor Register */
UINT32 AmbaCSL_EnetDmaGetDmaCurRxDes(const AMBA_ENET_REG_s *const pEnetReg)
{
    return pEnetReg->DmaR19_CURRDESAPTR;
}
