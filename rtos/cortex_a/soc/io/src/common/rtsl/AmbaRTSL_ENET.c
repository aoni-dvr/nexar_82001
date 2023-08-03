/**
 * @file AmbaRTSL_ENET.c
 * ENET Run Time Support Library: mdio, isr
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
#ifdef CONFIG_THREADX
#include "AmbaINT.h"
#endif
#include "AmbaENET_Def.h"
#include "AmbaRTSL_ENET.h"
#if defined(ENET_ASIL)
#include "AmbaSafety_ENET.h"
#endif

static UINT32 TsIsrArg;
static AMBA_RTSL_ENET_TSISRCB_f pAmbaRTSL_EnetTsIsrCb;
static AMBA_RTSL_ENET_TXISRCB_f pAmbaRTSL_EnetTxIsrCb;
static AMBA_RTSL_ENET_RXISRCB_f pAmbaRTSL_EnetRxIsrCb;
static AMBA_RTSL_ENET_LINKCB_f pAmbaRTSL_EnetLcCb;

// Disable NEON registers usage in ISR
#if !defined(GCOVR_ENABLE)
#pragma GCC push_options
#pragma GCC target("general-regs-only")
#endif

static void Eth_IrqHdlr(UINT32 IntID, UINT32 UserArg)
{
    UINT32 Idx;
    AMBA_ENET_REG_s *pEnetReg;
    AMBA_ENET_STAT_s *pEnetStat;
#ifdef ETHDEBUG_ISR
    UINT32 Status;
#endif
    (void)UserArg;

    if ((UINT32)IntID == (UINT32)EnetIrq[0]) {
        Idx = 0;
#if (ENET_INSTANCES >= 2U)
    } else if ((UINT32)IntID == (UINT32)EnetIrq[1]) {
        Idx = 1;
#endif
    } else {
        Idx = 0;
    }
    pEnetReg = pAmbaCSL_EnetReg[Idx];
    pEnetStat = &AmbaCSL_EnetStat[Idx];
#if defined(CONFIG_THREADX)
    (void) AmbaINT_Disable(EnetIrq[Idx]);
#endif

#ifdef ETHDEBUG_ISR
    Status = AmbaCSL_EnetDmaGetStatus(pEnetReg);
    EnetIsrDebug("irq%d 0x%08x", IntID, Status);
#endif

#if defined(ENET_ASIL)
    (void)AmbaSafety_EnetIrqHdlr(Idx);
#endif

    AmbaCSL_EnetDmaDumpStatus(pEnetReg, pEnetStat);

    if (AmbaCSL_EnetDmaGetRI(pEnetReg) != 0U) {
        AmbaCSL_EnetDmaRxIrqDisable(pEnetReg);
        if (pAmbaRTSL_EnetRxIsrCb != NULL) {
            pAmbaRTSL_EnetRxIsrCb(pEnetReg);
        }
    } else {
        if (AmbaCSL_EnetDmaGetRU(pEnetReg) != 0U) {
            if (pAmbaRTSL_EnetRxIsrCb != NULL) {
                pAmbaRTSL_EnetRxIsrCb(pEnetReg);
            }
            AmbaCSL_EnetDmaSetRxPoll(pEnetReg);
        }
    }

    if (AmbaCSL_EnetDmaGetGLI(pEnetReg) != 0U) {
        volatile UINT32 Reg;

        /* if compiler skip these, system will hang */
        Reg = AmbaCSL_EnetMacGetMacR14(pEnetReg);
        EnetIsrDebug("MacR14=0x%08x", Reg);
        Reg = AmbaCSL_EnetMacGetMacR15(pEnetReg);
        EnetIsrDebug("MacR15=0x%08x", Reg);
        Reg = AmbaCSL_EnetMacGetMacR49(pEnetReg);
        EnetIsrDebug("MacR49=0x%08x", Reg);
        Reg = AmbaCSL_EnetMacGetMacR54(pEnetReg);
        EnetIsrDebug("MacR54=0x%08x", Reg);
        Reg = AmbaCSL_EnetMacGetMacR56(pEnetReg);
        EnetIsrDebug("MacR56=0x%08x", Reg);

        /*Link Change CallBack: handle eth cable hotplug*/
        if (pAmbaRTSL_EnetLcCb != NULL) {
            (void)pAmbaRTSL_EnetLcCb(pEnetReg);
        }
    }

    if (AmbaCSL_EnetDmaGetTTI(pEnetReg) != 0U) {
        volatile UINT32 Reg;

        /* if compiler skip these, system will hang */
        Reg = AmbaCSL_EnetMacGetMacR14(pEnetReg);
        EnetIsrDebug("MacR14=0x%08x", Reg);
        Reg = AmbaCSL_EnetMacGetMacR458(pEnetReg);
        EnetIsrDebug("MacR458=0x%08x", Reg);

        if (pAmbaRTSL_EnetTsIsrCb != NULL) {
            pAmbaRTSL_EnetTsIsrCb(TsIsrArg);
        }
    }

    if (AmbaCSL_EnetDmaGetTI(pEnetReg) != 0U) {
        if (pAmbaRTSL_EnetTxIsrCb != NULL) {
            pAmbaRTSL_EnetTxIsrCb(pEnetReg);
        }
    } else {
        if (AmbaCSL_EnetDmaGetTU(pEnetReg) != 0U) {
            if (pAmbaRTSL_EnetTxIsrCb != NULL) {
                pAmbaRTSL_EnetTxIsrCb(pEnetReg);
            }
        }
    }

    AmbaCSL_EnetDmaClearStatus(pEnetReg);
#if defined(CONFIG_THREADX)
    (void) AmbaINT_Enable(EnetIrq[Idx]);
#endif
}

#if !defined(GCOVR_ENABLE)
#pragma GCC pop_options
#endif

UINT32 AmbaRTSL_EnetSetTsIsrFunc(AMBA_RTSL_ENET_TSISRCB_f pISR, const UINT32 Arg)
{
    pAmbaRTSL_EnetTsIsrCb = pISR;
    TsIsrArg = Arg;

    return 0;
}

UINT32 AmbaRTSL_EnetSetTxIsrFunc(AMBA_RTSL_ENET_TXISRCB_f pISR)
{
    pAmbaRTSL_EnetTxIsrCb = pISR;

    return 0;
}

UINT32 AmbaRTSL_EnetSetRxIsrFunc(AMBA_RTSL_ENET_RXISRCB_f pISR)
{
    pAmbaRTSL_EnetRxIsrCb = pISR;

    return 0;
}

UINT32 AmbaRTSL_EnetSetLcFunc(AMBA_RTSL_ENET_LINKCB_f pISR)
{
    pAmbaRTSL_EnetLcCb = pISR;

    return 0;
}

UINT16 AmbaRTSL_EnetMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 reg)
{
    UINT32 val;

    AmbaCSL_EnetMacMiiRead(pEnetReg, reg, addr);

    val = AmbaCSL_EnetMacGetMiiData(pEnetReg);

    return (UINT16)val;
}

UINT16 AmbaRTSL_EnetMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 addr, UINT8 regnum, UINT16 value)
{
    UINT32 val;

    val = value;

    AmbaCSL_EnetMacSetMiiData(pEnetReg, val);

    AmbaCSL_EnetMacMiiWrite(pEnetReg, regnum, addr);

    return 0;
}

UINT32 AmbaRTSL_EnetGetPhyId(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr)
{
    UINT16 Reg;
    UINT32 PhyId;

    Reg = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x02U);
    PhyId = (((UINT32)Reg & 0xffffU)) << 16;
    Reg = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x03U);
    PhyId |= ((UINT32)Reg & 0xffffU);

    return PhyId;
}

UINT8 AmbaRTSL_EnetGetPhyAddr(AMBA_ENET_REG_s *const pEnetReg)
{
    UINT8  found, Ret = 0xffU;
    UINT8  PhyAddr;
    UINT32 PhyId;

    for (PhyAddr = 0; PhyAddr < 32U; PhyAddr++) {
        PhyId = AmbaRTSL_EnetGetPhyId(pEnetReg, PhyAddr);
        found = 1U;

        if (PhyId == 0x0U) {
            continue;
        }

        if ((PhyId & 0xffffU) == 0xffffU) {
            found = 0U;
        }

        if (found != 0U) {
            Ret = PhyAddr;
            break;
        }
    }

    return Ret;
}

void AmbaRTSL_GetLink(AMBA_ENET_REG_s *const pEnetReg, UINT8 PhyAddr, UINT32 *Speed, UINT32 *Duplex)
{
    UINT16 bmsr;
    UINT16 bmcr;
    UINT16 lpa;
    UINT16 adv;

    EnetDebug("ENET %s(%d) 0M Half", __func__, __LINE__);
    *Speed = SPEED_100;
    *Duplex = DUPLEX_HALF;
    bmcr = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x00);
    EnetDebug("ENET %s(%d)0x00 Basic mode control register=0x%08x", __func__, __LINE__, bmcr);
    if (bmcr == 0xFFFFU) {
        *Speed = SPEED_0;
    }

    if (*Speed != SPEED_0) {
        if ((bmcr & 0x800U) == 0x800U) {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "PHY Powered down");
            bmcr &= (UINT16)~0x800U;
        }

        (void) AmbaRTSL_EnetMiiWrite(pEnetReg, PhyAddr, 0x00, bmcr);

        if ((bmcr & 0x1000U) == 0x1000U) {
            UINT16 lpagb;
#if !defined(CONFIG_SOC_A12)
            lpagb = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x0a);
            EnetDebug("ENET %s(%d)0x0a 1000BASE-T Status reg=0x%08x", __func__, __LINE__, lpagb);

            adv = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x09);
            EnetDebug("ENET %s(%d)0x09 1000BASE-T control reg=0x%08x", __func__, __LINE__, adv);
            if ((adv == 0xFFFFU) || (lpagb == 0xFFFFU)) {
                lpagb = 0;
            } else {
                lpagb = lpagb & (adv << 2);
            }
#else
            lpagb = 0U;
#endif
            EnetDebug("ENET %s(%d) auto negotiation enabled", __func__, __LINE__);
            lpa = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x05);
            EnetDebug("ENET %s(%d)0x05 Link partner ability reg=0x%08x", __func__, __LINE__, lpa);
            if (lpa == 0xFFFFU) {
                *Speed = SPEED_0;
            }

            adv = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x04);
            EnetDebug("ENET %s(%d)0x04 Advertisement control reg=0x%08x", __func__, __LINE__, adv);
            if (adv == 0xFFFFU) {
                *Speed = SPEED_0;
            }

            if (*Speed != SPEED_0) {
                lpa &= adv;

                if ((lpagb & (0x0800U | 0x0400U)) != 0U) {
                    *Speed = SPEED_1000;
                    EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "1000M");
                    if ((lpagb & 0x0800U) != 0U) {
                        *Duplex = DUPLEX_FULL;
                        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "full duplex");
                    }
                } else if ((lpa & (0x0100U | 0x0080U)) != 0U) {
                    *Speed = SPEED_100;
                    EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "100M");
                    if ((lpa & 0x0100U) != 0U) {
                        *Duplex = DUPLEX_FULL;
                        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "full duplex");
                    }
                } else {
                    *Speed = SPEED_10;
                    EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "10M");
                    if ((lpa & 0x0040U) != 0U) {
                        *Duplex = DUPLEX_FULL;
                        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "full duplex");
                    }
                }
            }
        } else {
            EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "auto negotiation disabled");
#if 0
            adv  = (bmcr & 0x2000U) >> 13;
            adv |= (bmcr & 0x0040U) >> 5;

            switch (adv) {
            case 1:
                *Speed = SPEED_100;
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "100M");
                break;
            case 2:
                *Speed = SPEED_1000;
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "1000M");
                break;
            case 0:
            default:
                *Speed = SPEED_10;
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "10M");
                break;
            }

            if ((bmcr & 0x0100U) != 0U) {
                *Duplex = DUPLEX_FULL;
                EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "full duplex");
            }
#else
            *Speed = SPEED_100;
            *Duplex = DUPLEX_FULL;
#endif
        }
    }

    bmsr = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x01);
    EnetDebug("ENET %s(%d)0x01 Basic mode Status register=0x%08x", __func__, __LINE__, bmsr);
    if (bmsr == 0xFFFFU) {
        *Speed = SPEED_0;
    }

    //BMSR sticky be-read a second time to get the current Status
    bmsr = AmbaRTSL_EnetMiiRead(pEnetReg, PhyAddr, 0x01);
    EnetDebug("ENET %s(%d)0x01 Basic mode Status register=0x%08x", __func__, __LINE__, bmsr);
    if (bmsr == 0xFFFFU) {
        *Speed = SPEED_0;
    }

    if ((bmsr & 0x0020U) == 0x0020U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_WHITE, "Auto-Negotiation process completed");
    }

    if ((bmsr & 0x0004U) != 0x0004U) {
        EnetInfo(__func__, __LINE__, ETH_MODULE_ID, ANSI_GREEN, "NO link beat");
        *Speed = SPEED_0;
    }
}

void AmbaRTSL_EnetINTEnable(UINT32 Idx)
{
#if defined(CONFIG_THREADX)
    AMBA_INT_CONFIG_s IntConfig;

    IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType    = INT_TYPE_IRQ;
    IntConfig.CpuTargets = 0x01;

    if (EnetIrq[Idx] != 0U) {
        (void) AmbaINT_Config(EnetIrq[Idx], &IntConfig, Eth_IrqHdlr, 0);
        (void) AmbaINT_Enable(EnetIrq[Idx]);
    }
#else
    (void) Idx;
#endif
}

void AmbaRTSL_EnetINTDisable(UINT32 Idx)
{
#if defined(CONFIG_THREADX)
    if (EnetIrq[Idx] != 0U) {
        (void) AmbaINT_Disable(EnetIrq[Idx]);
    }
#else
    (void) Idx;
#endif
}

void AmbaRTSL_EnetInit(AMBA_ENET_REG_s *const pEnetReg)
{
#if defined(CONFIG_THREADX)
    UINT32 Idx = AmbaRTSL_EnetGetRegIdx(pEnetReg);
    extern UINT32 AmbaRTSL_RtcGetSystemTime(void);
    const UINT32 sys = AmbaRTSL_RtcGetSystemTime();

    AmbaRTSL_EnetINTDisable(Idx);
#endif
    AmbaCSL_EnetDmaInitR0(pEnetReg);
    AmbaCSL_EnetDmaInitR6(pEnetReg);
    AmbaCSL_EnetDmaClearStatus(pEnetReg);
    AmbaCSL_EnetDmaInitR7(pEnetReg);
    AmbaCSL_EnetDmaInitR10(pEnetReg);
    //AmbaCSL_EnetDmaStartRX(pEnetReg);
    //AmbaCSL_EnetDmaStartTX(pEnetReg);

    /* 01:00:5e:00:00:01 mcast all addr offline-hashed into R2 bit0 */
    AmbaCSL_EnetMacSetHashHi(pEnetReg, 0x0000001U);

    AmbaCSL_EnetMacInitR1(pEnetReg);
    AmbaCSL_EnetMacInitR6(pEnetReg);
    AmbaCSL_EnetMacInitR15(pEnetReg);

    AmbaCSL_EnetMacInitR448(pEnetReg);
    /*  Set R449 0x32 = 50ns accuracy */
    AmbaCSL_EnetMacSetSSINC(pEnetReg, 0x32U);
    AmbaCSL_EnetMacSetTSAR(pEnetReg, DEFAULT_ADDEND);
    AmbaCSL_EnetMacSetTSADDREG(pEnetReg);

    /* init PHC (PTP Hardware Clock) value */
#ifdef CONFIG_THREADX
    AmbaCSL_EnetMacSetTSS(pEnetReg, sys);
#endif
    AmbaCSL_EnetMacTSINIT(pEnetReg);

    /* R459 Target Time Register Mode: trigger both irq and pps */
    AmbaCSL_EnetMacSetTRGTMODSEL0(pEnetReg, 2U);

#if defined(CONFIG_THREADX)
    AmbaRTSL_EnetINTEnable(Idx);
#endif
}

#if defined(__QNXNTO__)
void QNX_EthIdrHdlr(UINT32 Idx)
{
    AMBA_ENET_REG_s *pEnetReg;
    if (Idx == 0U) {
        Eth_IrqHdlr(EnetIrq[0], 0U);
    }
#if (ENET_INSTANCES >= 2U)
    else {
        if (Idx == 1U) {
            Eth_IrqHdlr(EnetIrq[1], 0U);
        }
    }
#endif

    pEnetReg = pAmbaCSL_EnetReg[Idx];
    AmbaCSL_EnetDmaTxIrqEnable(pEnetReg);
}
#endif

UINT32 AmbaRTSL_EnetGetRegIdx(const AMBA_ENET_REG_s *const pEnetReg)
{
    UINT32 Idx = 0;

    if (pEnetReg == pAmbaCSL_EnetReg[0]) {
        Idx = 0;
#if (ENET_INSTANCES >= 2U)
    } else if (pEnetReg == pAmbaCSL_EnetReg[1]) {
        Idx = 1;
#endif
    } else {
        Idx = 0;
    }

    return Idx;
}

