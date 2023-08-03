/**
 * @file AmbaCSL_ENET.h
 * ENET Register read/write API
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

#ifndef AMBA_CSL_ENET_H
#define AMBA_CSL_ENET_H

#include "AmbaReg_ENET.h"

#define SPEED_0                         0U
#define SPEED_10                        10U
#define SPEED_100                       100U
#define SPEED_1000                      1000U
#define DUPLEX_HALF                     0x00U
#define DUPLEX_FULL                     0x01U

typedef struct  {
    /* irq NIS */
    UINT32 NIS;              /**< [16] NIS Normal Interrupt Summary */
    UINT32 TI;               /**< [0] TI Transmit Interrupt */
    UINT32 TU;               /**< [2] TU Transmit Buffer Unavailable */
    UINT32 RI;               /**< [6] RI Receive Interrupt */
    UINT32 ERI;              /**< [14] ERI Early Receive Interrupt */

    /* irq AIS */
    UINT32 AIS;              /**< [15] AIS Abnormal Interrupt Summary */
    UINT32 TPS;              /**< [1] TPS Transmit Process Stopped */
    UINT32 TJT;              /**< [3] TJT Transmit Jabber Timeout */
    UINT32 OVF;              /**< [4] OVF Receive Overflow */
    UINT32 UNF;              /**< [5] UNF Transmit Underflow */
    UINT32 RU;               /**< [7] RU Receive Buffer Unavailable */
    UINT32 RPS;              /**< [8] RPS Receive Process Stopped */
    UINT32 RWT;              /**< [9] RWT Receive Watchdog Timeout */
    UINT32 ETI;              /**< [10] ETI Early Transmit Interrupt */
    UINT32 FBI;              /**< [13] FBI Fatal Bus Error Interrupt */

    UINT32 TDES0_OWN;        /**< [31] OWN: Own Bit */
    UINT32 TDES1_TBS1;       /**< [12:0] TBS1: Transmit Buffer 1 Size */
    UINT32 RDES0_OWN;        /**< [31] OWN: Own Bit */
    UINT32 RDES1_RBS1;       /**< [12:0] RBS1: Receive Buffer 1 Size */

    /* TDES ES */
    UINT32 TDES0_ES;           /**< [15] ES: Error Summary */
    UINT32 TDES0_UF;           /**< [1] UF: Underflow Error */
    UINT32 TDES0_ED;           /**< [2] ED: Excessive Deferral */
    UINT32 TDES0_EC;           /**< [8] EC: Excessive Collision */
    UINT32 TDES0_LC;           /**< [9] LC: Late Collision */
    UINT32 TDES0_NC;           /**< [10] NC: No Carrier */
    UINT32 TDES0_LOC;          /**< [11] LOC: Loss of Carrier */
    UINT32 TDES0_IPE;          /**< [12] IPE: IP Payload Error */
    UINT32 TDES0_FF;           /**< [13] FF: Frame Flushed */
    UINT32 TDES0_JT;           /**< [14] JT: Jabber Timeout */
    UINT32 TDES0_IHE;          /**< [16] IHE: IP Header Error */

    /* RDES ES */
    UINT32 RDES0_ES;         /**< [15] ES: Error Summary */
    UINT32 RDES0_CE;         /**< [1] CE: CRC Error */
    UINT32 RDES0_RE;         /**< [3] RE: Receive Error */
    UINT32 RDES0_RWT;        /**< [4] RWT: Receive Watchdog Timeout */
    UINT32 RDES0_LC;         /**< [6] LC: Late Collision */
    UINT32 RDES0_TIMESTAMP;      /**< [7] Timestamp Available, IP Checksum Error (Type1), or Giant Frame */
    UINT32 RDES0_OE;         /**< [11] OE: Overflow Error */
    UINT32 RDES0_DE;         /**< [14] DE: Descriptor Error */
    UINT32 RDES4_IPHR;       /**< [3] IP Header Error */
    UINT32 RDES4_IPPE;       /**< [4] IP Payload Error */
} AMBA_ENET_STAT_s;

extern ULONG EnetReg[ENET_INSTANCES];
extern UINT32 EnetIrq[ENET_INSTANCES];
extern AMBA_ENET_REG_s *pAmbaCSL_EnetReg[ENET_INSTANCES];
extern AMBA_ENET_STAT_s AmbaCSL_EnetStat[ENET_INSTANCES];
extern AMBA_ENET_TS_s *pAmbaCSL_EnetPhc[ENET_INSTANCES];

void AmbaCSL_EnetTDES0Init(AMBA_ENET_TDES_s *pEnetTDES);
void AmbaCSL_EnetTDESSetDmaOwn(AMBA_ENET_TDES_s *pEnetTDES);
UINT32 AmbaCSL_EnetTDESGetDmaOwn(const AMBA_ENET_TDES_s *const pEnetTDES);
UINT32 AmbaCSL_EnetTDESGetDmaES(const AMBA_ENET_TDES_s *const pEnetTDES);
UINT32 AmbaCSL_EnetTDESGetTTSS(const AMBA_ENET_TDES_s *const pEnetTDES);
void AmbaCSL_EnetTDESSetCIC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 CIC);
void AmbaCSL_EnetTDESSetLSFS(AMBA_ENET_TDES_s *pEnetTDES, UINT8 LS, UINT8 FS);
void AmbaCSL_EnetTDESSetVLIC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 VLIC);
void AmbaCSL_EnetTDESSetDC(AMBA_ENET_TDES_s *pEnetTDES, UINT8 DC);
void AmbaCSL_EnetTDES1Init(AMBA_ENET_TDES_s *pEnetTDES);
void AmbaCSL_EnetTDESSetTBS1(AMBA_ENET_TDES_s *pEnetTDES, UINT16 TBS1);
void AmbaCSL_EnetTDES2Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf1);
void AmbaCSL_EnetTDES3Init(AMBA_ENET_TDES_s *pEnetTDES, UINT32 Buf2);
void AmbaCSL_EnetRDES0Init(AMBA_ENET_RDES_s *pEnetRDES);
void AmbaCSL_EnetRDESSetDmaOwn(AMBA_ENET_RDES_s *pEnetRDES);
UINT16 AmbaCSL_EnetRDESGetFL(const AMBA_ENET_RDES_s *const pEnetRDES);
UINT32 AmbaCSL_EnetRDESGetDmaOwn(const AMBA_ENET_RDES_s *const pEnetRDES);
void AmbaCSL_EnetRDES1Init(AMBA_ENET_RDES_s *pEnetRDES);
void AmbaCSL_EnetRDESSetRBS1(AMBA_ENET_RDES_s *pEnetRDES, UINT16 RBS1);
void AmbaCSL_EnetRDES2Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf1);
void AmbaCSL_EnetRDES3Init(AMBA_ENET_RDES_s *pEnetRDES, UINT32 Buf2);
void AmbaCSL_EnetMacLinkSetup(AMBA_ENET_REG_s *const pEnetReg, UINT32 SPEED, UINT32 DUPLEX);
void AmbaCSL_EnetMacSetJEJD(AMBA_ENET_REG_s *const pEnetReg, UINT8 JE, UINT8 JD);
void AmbaCSL_EnetMacTxDisable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacRxDisable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacInitR1(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetHMCPM(AMBA_ENET_REG_s *const pEnetReg, UINT8 HMC, UINT8 PM);
void AmbaCSL_EnetMacSetHashHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTH);
void AmbaCSL_EnetMacClearHashHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTH);
void AmbaCSL_EnetMacSetHashLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTL);
void AmbaCSL_EnetMacClearHashLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 HTL);
void AmbaCSL_EnetMacMiiWrite(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA);
void AmbaCSL_EnetMacMiiRead(AMBA_ENET_REG_s *const pEnetReg, UINT8 GR, UINT8 PA);
void AmbaCSL_EnetMacSetMiiData(AMBA_ENET_REG_s *const pEnetReg, UINT32 GB);
UINT32 AmbaCSL_EnetMacGetMiiData(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacInitR6(AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR8(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR14(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacInitR15(AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR15(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetTSIM(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSIM);
void AmbaCSL_EnetMacSetAddrHi(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRHI);
UINT32 AmbaCSL_EnetMacGetAddrHi(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetAddrLo(AMBA_ENET_REG_s *const pEnetReg, UINT32 ADDRLO);
UINT32 AmbaCSL_EnetMacGetAddrLo(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR49(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR54(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetMacR56(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetVLAN(AMBA_ENET_REG_s *const pEnetReg, UINT8 VLC, UINT16 VLT);

void AmbaCSL_EnetMacInitR448(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetTSADDREG(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacTSINIT(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacTSUPDT(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetTSTRIG(AMBA_ENET_REG_s *const pEnetReg, UINT8 TSTRIG);
void AmbaCSL_EnetMacSetSSINC(AMBA_ENET_REG_s *const pEnetReg, UINT32 SSINC);
UINT32 AmbaCSL_EnetMacGetPhcTsAddr(AMBA_ENET_REG_s *const pEnetReg, volatile void **Addr);
UINT32 AmbaCSL_EnetMacGetPhcSec(AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetMacGetPhcNs(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetTSS(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSS);
void AmbaCSL_EnetMacSetTSSS(AMBA_ENET_REG_s *const pEnetReg, UINT8 ADDSUB, UINT32 TSSS);
UINT32 AmbaCSL_EnetMacGetTSAR(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetTSAR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSAR);
void AmbaCSL_EnetMacSetTSTR(AMBA_ENET_REG_s *const pEnetReg, UINT32 TSTR);
void AmbaCSL_EnetMacSetTTSLO(AMBA_ENET_REG_s *const pEnetReg, UINT32 TTSLO);
UINT32 AmbaCSL_EnetMacGetMacR458(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetMacSetPPSCTRL0(AMBA_ENET_REG_s *const pEnetReg, UINT8 PPSCTRL0);
void AmbaCSL_EnetMacSetPPSCMD0(AMBA_ENET_REG_s *const pEnetReg, UINT8 PPSCMD0);
void AmbaCSL_EnetMacSetTRGTMODSEL0(AMBA_ENET_REG_s *const pEnetReg, UINT8 TRGTMODSEL0);
void AmbaCSL_EnetMacSetPPSINT(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSINT);
void AmbaCSL_EnetMacSetPPSWIDTH(AMBA_ENET_REG_s *const pEnetReg, UINT32 PPSWIDTH);

UINT32 AmbaCSL_EnetDmaReset(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaInitR0(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaSetTxPoll(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaSetRxPoll(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaSetRxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 RDESLA);
void AmbaCSL_EnetDmaSetTxDES(AMBA_ENET_REG_s *const pEnetReg, UINT32 TDESLA);
UINT32 AmbaCSL_EnetDmaGetStatus(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaClearStatus(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetTDESDumpStatus(const AMBA_ENET_TDES_s *pEnetTDES, AMBA_ENET_STAT_s *pEnetStat);
void AmbaCSL_EnetRDESDumpStatus(const AMBA_ENET_RDES_s *pEnetRDES, AMBA_ENET_STAT_s *pEnetStat);
void AmbaCSL_EnetDmaDumpStatus(const AMBA_ENET_REG_s *const pEnetReg, AMBA_ENET_STAT_s *pEnetStat);
UINT32 AmbaCSL_EnetDmaGetTI(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetTU(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetRI(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetRU(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetGLI(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetTTI(const AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaStartTX(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaStartRX(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaStopTX(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaStopRX(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaInitR6(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaInitR7(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaTxIrqDisable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaTxIrqEnable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaRxIrqDisable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaRxIrqEnable(AMBA_ENET_REG_s *const pEnetReg);
void AmbaCSL_EnetDmaInitR10(AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetDmaCurTxDes(const AMBA_ENET_REG_s *const pEnetReg);
UINT32 AmbaCSL_EnetDmaGetDmaCurRxDes(const AMBA_ENET_REG_s *const pEnetReg);

/* debug */
extern void EnetDebug(const char *fmt, ...);
extern void EnetInfo(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg);
extern void EnetIsrDebug(const char *fmt, ...);
extern void ambhw_dump_buffer(const char *msg, const UINT8 *data, UINT32 length);

#endif /* AMBA_CSL_ENET_H */
