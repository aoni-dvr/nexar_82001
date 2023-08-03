/**
 * @file AmbaReg_ENET.h
 * All ENET Registers Definined here
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

#ifndef AMBA_REG_ENET_H
#define AMBA_REG_ENET_H

/** 0x0000        Transmit Descriptor 0 (TDES0) */
typedef struct {
    UINT32 DB:         1;  /**< [0] DB: Deferred Bit */
    UINT32 UF:         1;  /**< [1] UF: Underflow Error */
    UINT32 ED:         1;  /**< [2] ED: Excessive Deferral */
    UINT32 CC:         4;  /**< [6:3] CC: Collision Count (Status field) */
    UINT32 VF:         1;  /**< [7] VF: VLAN Frame */
    UINT32 EC:         1;  /**< [8] EC: Excessive Collision */
    UINT32 LC:         1;  /**< [9] LC: Late Collision */
    UINT32 NC:         1;  /**< [10] NC: No Carrier */
    UINT32 LOC:        1;  /**< [11] LOC: Loss of Carrier */
    UINT32 IPE:        1;  /**< [12] IPE: IP Payload Error */
    UINT32 FF:         1;  /**< [13] FF: Frame Flushed */
    UINT32 JT:         1;  /**< [14] JT: Jabber Timeout */
    UINT32 ES:         1;  /**< [15] ES: Error Summary */
    UINT32 IHE:        1;  /**< [16] IHE: IP Header Error */
    UINT32 TTSS:       1;  /**< [17] TTSS: Transmit Timestamp Status */
    UINT32 VLIC:       2;  /**< [19:18] VLIC: VLAN Insertion Control */
    UINT32 TCH:        1;  /**< [20] TCH: Second Address Chained */
    UINT32 TER:        1;  /**< [21] TER: Transmit End of Ring */
    UINT32 CIC:        2;  /**< [23:22] CIC: Checksum Insertion Control */
    UINT32 CRCR:       1;  /**< [24] CRCR: CRC Replacement Control */
    UINT32 TTSE:       1;  /**< [25] TTSE: Transmit Timestamp Enable */
    UINT32 DP:         1;  /**< [26] DP: Disable Pad */
    UINT32 DC:         1;  /**< [27] DC: Disable CRC */
    UINT32 FS:         1;  /**< [28] FS: First Segment */
    UINT32 LS:         1;  /**< [29] LS: Last Segment */
    UINT32 IC:         1;  /**< [30] IC: Interrupt on Completion */
    UINT32 OWN:        1;  /**< [31] OWN: Own Bit */
} AMBA_ENET_TDES0_s;

/** 0x0004        Transmit Descriptor 1 (TDES1) */
typedef struct {
    UINT32 TBS1:       13; /**< [12:0] TBS1: Transmit Buffer 1 Size */
    UINT32 Reserved13: 3;  /**< [15:13] Reserved */
    UINT32 TBS2:       13; /**< [28:16] TBS2: Transmit Buffer 2 Size */
    UINT32 SAIC:       3;  /**< [31:29] SAIC: SA Insertion Control */
} AMBA_ENET_TDES1_s;

/** 0x0000        Receive Descriptor 0 (RDES0) */
typedef struct {
    UINT32 Extended:   1;  /**< [0] Extended Status Available/Rx MAC Address */
    UINT32 CE:         1;  /**< [1] CE: CRC Error */
    UINT32 DBE:        1;  /**< [2] DE: Dribble Bit Error */
    UINT32 RE:         1;  /**< [3] RE: Receive Error */
    UINT32 RWT:        1;  /**< [4] RWT: Receive Watchdog Timeout */
    UINT32 FT:         1;  /**< [5] FT: Frame Type */
    UINT32 LC:         1;  /**< [6] LC: Late Collision */
    UINT32 Timestamp:  1;  /**< [7] Timestamp Available, IP Checksum Error (Type1), or Giant Frame */
    UINT32 LS:         1;  /**< [8] LS: Last Descriptor */
    UINT32 FS:         1;  /**< [9] FS: First Descriptor */
    UINT32 VLAN:       1;  /**< [10] VLAN: VLAN Tag */
    UINT32 OE:         1;  /**< [11] OE: Overflow Error */
    UINT32 LE:         1;  /**< [12] LE: Length Error */
    UINT32 SAF:        1;  /**< [13] SAF: Source Address Filter Fail */
    UINT32 DE:         1;  /**< [14] DE: Descriptor Error */
    UINT32 ES:         1;  /**< [15] ES: Error Summary */
    UINT32 FL:         14; /**< [29:16] FL: Frame Length */
    UINT32 AFM:        1;  /**< [30] AFM: Destination Address Filter Fail */
    UINT32 OWN:        1;  /**< [31] OWN: Own Bit */
} AMBA_ENET_RDES0_s;

/** 0x0004        Receive Descriptor 1 (RDES1) */
typedef struct {
    UINT32 RBS1:       13; /**< [12:0] RBS1: Receive Buffer 1 Size */
    UINT32 Reserved13: 1;  /**< [13] Reserved */
    UINT32 RCH:        1;  /**< [14] RCH: Second Address Chained */
    UINT32 RER:        1;  /**< [15] RER: Receive End of Ring */
    UINT32 RBS2:       13; /**< [28:16] RBS2: Receive Buffer 2 Size */
    UINT32 Reserved29: 2;  /**< [30:29] Reserved */
    UINT32 DIC:        1;  /**< [31] DIC: Disable Interrupt on Completion */
} AMBA_ENET_RDES1_s;

/** 0x0010        Receive Descriptor 4 (RDES4) */
typedef struct {
    UINT32 IPPL:       3;  /**< [2:0] IP Payload Type */
    UINT32 IPHR:       1;  /**< [3] IP Header Error */
    UINT32 IPPE:       1;  /**< [4] IP Payload Error */
    UINT32 IPCSB:      1;  /**< [5] IP Checksum Bypassed */
    UINT32 IPv4:       1;  /**< [6] IPv4 Packet Received */
    UINT32 IPv6:       1;  /**< [7] IPv6 Packet Received */
    UINT32 Message:    4;  /**< [11:8] Message Type */
    UINT32 PTPFT:      1;  /**< [12] PTP Frame Type */
    UINT32 PTPVER:     1;  /**< [13] PTP Version */
    UINT32 Timestamp:  1;  /**< [14] Timestamp Dropped */
    UINT32 Reserved15: 1;  /**< [15] Reserved */
    UINT32 AVPR:       1;  /**< [16] AV Packet Received */
    UINT32 AVTR:       1;  /**< [17] AV Tagged Packet Received */
    UINT32 VLAN:       3;  /**< [20:18] VLAN Tag Priority Value */
    UINT32 Reserved:   3;  /**< [23:21] Reserved */
    UINT32 Layer3FM:   1;  /**< [24] Layer 3 Filter Match */
    UINT32 Layer4FM:   1;  /**< [25] Layer 4 Filter Match */
    UINT32 Layer34FNM: 2;  /**< [27:26] Layer 3 and Layer 4 Filter Number Matched */
    UINT32 Reserved28: 4;  /**< [31:28] Reserved */
} AMBA_ENET_RDES4_s;

/********************************************//**
 *  ENET MAC
 ***********************************************/
/** 0x0000        MAC R0         MAC Configuration Register */
typedef struct {
    UINT32 PRELEN:     2;  /**< [1:0] PRELEN Preamble Length for Transmit frames */
    UINT32 RE:         1;  /**< [2] RE Receiver Enable */
    UINT32 TE:         1;  /**< [3] TE Transmitter Enable */
    UINT32 DC:         1;  /**< [4] DC Deferral Check */
    UINT32 BL:         2;  /**< [6:5] BL Back-Off Limit */
    UINT32 ACS:        1;  /**< [7] ACS Automatic Pad or CRC Stripping */
    UINT32 LUD:        1;  /**< [8] LUD Link Up or Down */
    UINT32 DR:         1;  /**< [9] DR Disable Retry */
    UINT32 IPC:        1;  /**< [10] IPC Checksum Offload */
    UINT32 DM:         1;  /**< [11] DM Duplex Mode */
    UINT32 LM:         1;  /**< [12] LM Loopback Mode */
    UINT32 DO:         1;  /**< [13] DO Disable Receive Own */
    UINT32 FES:        1;  /**< [14] FES Speed */
    UINT32 PS:         1;  /**< [15] PS Port Select */
    UINT32 DCRS:       1;  /**< [16] DCRS Disable Carrier Sense During Transmission */
    UINT32 JFG:        3;  /**< [19:17] IFG Inter-Frame Gap */
    UINT32 JE:         1;  /**< [20] JE Jumbo Frame Enable */
    UINT32 BE:         1;  /**< [21] BE Frame Burst Enable */
    UINT32 JD:         1;  /**< [22] JD Jabber Disable */
    UINT32 WD:         1;  /**< [23] WD Watchdog Disable */
    UINT32 TC:         1;  /**< [24] TC Transmit Configuration in RGMII, SGMII, or SMII */
    UINT32 CST:        1;  /**< [25] CST CRC Stripping for Type Frames */
    UINT32 SFRERR:     1;  /**< [26] SFTERR SMII Force Transmit Error */
    UINT32 TWOKPE:     1;  /**< [27] TWOKPE IEEE 802.3as Support for 2K Packets */
    UINT32 SARC:       1;  /**< [30:28] SARC Source Address Insertion or Replacement Control */
    UINT32 Reserved31: 1;  /**< [31] Reserved */
} AMBA_ENET_MACR0_s;

/** 0x0004        MAC R1         MAC Frame Filter */
typedef struct {
    UINT32 PR:         1;  /**< [0] PR Promiscuous Mode */
    UINT32 HUC:        1;  /**< [1] HUC Hash Unicast */
    UINT32 HMC:        1;  /**< [2] HMC Hash Multicast */
    UINT32 DAIF:       1;  /**< [3] DAIF DA Inverse Filtering */
    UINT32 PM:         1;  /**< [4] PM Pass All Multicast */
    UINT32 DBF:        1;  /**< [5] DBF Disable Broadcast Frames */
    UINT32 PCF:        2;  /**< [7:6] PCF Pass Control Frames */
    UINT32 SAIF:       1;  /**< [8] SAIF SA Inverse Filtering */
    UINT32 SAF:        1;  /**< [9] SAF Source Address Filter Enable */
    UINT32 HPF:        1;  /**< [10] HPF Hash or Perfect Filter */
    UINT32 Reserved11: 5;  /**< [15:11] Reserved */
    UINT32 VTFE:       1;  /**< [16] VTFE VLAN Tag Filter Enable */
    UINT32 Reserved17: 3;  /**< [19:17] Reserved */
    UINT32 IPFE:       1;  /**< [20] IPFE Layer 3 and Layer 4 Filter Enable */
    UINT32 DNTU:       1;  /**< [21] DNTU Drop non-TCP/UDP over IP Frames */
    UINT32 Reserved22: 9;  /**< [30:22] Reserved */
    UINT32 RA:         1;  /**< [31] RA Receive All */
} AMBA_ENET_MACR1_s;

/** 0x0010        MAC R4         GMII Address Register */
typedef struct {
    UINT32 GB:           1;  /**< [0] GB GMII Busy */
    UINT32 GW:           1;  /**< [1] GW GMII Write */
    UINT32 CR:           4;  /**< [5:2] CR CSR Clock Range */
    UINT32 GR:           5;  /**< [10:6] GR GMII Register */
    UINT32 PA:           5;  /**< [15:11] PA Physical Layer Address */
    UINT32 Reserved16:   16; /**< [31:16] Reserved */
} AMBA_ENET_MACR4_s;

/** 0x0018        MAC R6         Flow Control Register */
typedef struct {
    UINT32 FCB_BPA:      1;  /**< [0] FCB_BPA */
    UINT32 TFE:          1;  /**< [1] TFE Transmit Flow Control Enable */
    UINT32 RFE:          1;  /**< [2] RFE Receive Flow Control Enable */
    UINT32 UP:           1;  /**< [3] UP Unicast Pause Frame Detect */
    UINT32 PLT:          2;  /**< [5:4] PLT Pause Low Threshold */
    UINT32 Reserved6:    1;  /**< [6] Reserved */
    UINT32 DZPQ:         1;  /**< [7] DZPQ Disable Zero-Quanta Pause */
    UINT32 Reserved8:    8;  /**< [15:8] Reserved */
    UINT32 PT:           16; /**< [31:16] PT Pause Time */
} AMBA_ENET_MACR6_s;

/** 0x001C        MAC R7         VLAN Tag Register */
#if 0
typedef struct {
    UINT32 VL:           16; /**< [15:0] VL VLAN Tag Identifier for Receive Frames */
    UINT32 ETV:          1;  /**< [16] ETV Enable 12-Bit VLAN Tag Comparison */
    UINT32 VTIM:         1;  /**< [17] VTIM VLAN Tag Inverse Match Enable */
    UINT32 ESVL:         1;  /**< [18] ESVL Enable S-VLAN */
    UINT32 VTHM:         1;  /**< [19] VTHM VLAN Tag Hash Table Match Enable */
    UINT32 Reserved20:   12; /**< [31:20] - Reserved 000H RO */
} AMBA_ENET_MACR7_s;
#endif

/** 0x0038        MAC R14        Interrupt Status Register */
typedef struct {
    UINT32 RGSMIIIS:     1;  /**< [0] RGSMIIIS RGMII or SMII Interrupt Status */
    UINT32 PCSLCHGIS:    1;  /**< [1] PCSLCHGIS PCS Link Status Changed */
    UINT32 PCSANCIS:     1;  /**< [2] PCSANCIS PCS Auto-Negotiation Complete */
    UINT32 PMTIS:        1;  /**< [3] PMTIS PMT Interrupt Status */
    UINT32 MMCIS:        1;  /**< [4] MMCIS MMC Interrupt Status */
    UINT32 MMCRXIS:      1;  /**< [5] MMCRXIS MMC Receive Interrupt Status */
    UINT32 MMCTXIS:      1;  /**< [6] MMCTXIS MMC Transmit Interrupt Status */
    UINT32 MMCRXIPIS:    1;  /**< [7] MMCRXIPIS MMC Receive Checksum Offload Interrupt Status */
    UINT32 Reserved8:    1;  /**< [8] Reserved */
    UINT32 TSIS:         1;  /**< [9] TSIS Timestamp Interrupt Status */
    UINT32 LPIIS:        1;  /**< [10] LPIIS LPI Interrupt Status */
    UINT32 GPIIS:        1;  /**< [11] GPIIS GPI Interrupt Status */
    UINT32 Reserved12:   20; /**< [31:12] Reserved */
} AMBA_ENET_MACR14_s;

/** 0x003C        MAC R15        Interrupt Mask Register */
typedef struct {
    UINT32 RGSMIIIM:     1;  /**< [0] RGSMIIIM RGMII or SMII Interrupt Mask */
    UINT32 PCSLCHGIM:    1;  /**< [1] PCSLCHGIM PCS Link Status Interrupt Mask */
    UINT32 PCSANCIM:     1;  /**< [2] PCSANCIM PCS AN Completion Interrupt Mask */
    UINT32 PMTIM:        1;  /**< [3] PMTIM PMT Interrupt Mask */
    UINT32 Reserved4:    5;  /**< [8:4] Reserved 00H RO */
    UINT32 TSIM:         1;  /**< [9] TSIM Timestamp Interrupt Mask */
    UINT32 LPIIM:        1;  /**< [10] LPIIM LPI Interrupt Mask */
    UINT32 Reserved11:   21; /**< [31:11] Reserved */
} AMBA_ENET_MACR15_s;

/** 0x00C0        MAC R48        AN Control Register */
typedef struct {
    UINT32 Reserved0:    9;  /**< [8:0] Reserved */
    UINT32 RAN:          1;  /**< [9] RAN Restart Auto-Negotiation */
    UINT32 Reserved10:   2;  /**< [11:10] Reserved */
    UINT32 ANE:          1;  /**< [12] ANE Auto-Negotiation Enable */
    UINT32 Reserved13:   1;  /**< [13] Reserved */
    UINT32 ELE:          1;  /**< [14] ELE External Loopback Enable */
    UINT32 Reserved15:   1;  /**< [15] Reserved */
    UINT32 ECD:          1;  /**< [16] ECD Enable Comma Detect */
    UINT32 LR:           1;  /**< [17] LR Lock to Reference */
    UINT32 SGMRAL:       1;  /**< [18] SGMRAL SGMII RAL Control */
    UINT32 Reserved19:   13; /**< [31:19] Reserved */
} AMBA_ENET_MACR48_s;

/** 0x00C4        MAC R49        AN Status Register */
typedef struct {
    UINT32 Reserved0:    2;  /**< [1:0] Reserved 00 RO */
    UINT32 LS:           1;  /**< [2] LS Link Status */
    UINT32 ANA:          1;  /**< [3] ANA Auto-Negotiation Ability */
    UINT32 Reserved4:    1;  /**< [4] Reserved 0 RO */
    UINT32 ANC:          1;  /**< [5] ANC Auto-Negotiation Complete */
    UINT32 Reserved6:    2;  /**< [7:6] Reserved 00 RO */
    UINT32 ES:           1;  /**< [8] ES Extended Status */
    UINT32 Reserved9:    23; /**< [31:9] Reserved 00_0000H RO */
} AMBA_ENET_MACR49_s;

/** 0x00D8        MAC R54        SGMII/RGMII/SMII Control and Status Register */
typedef struct {
    UINT32 LNKMOD:       1;  /**< [0] LNKMOD Link Mode */
    UINT32 LNKSPEED:     2;  /**< [2:1] LNKSPEED Link Speed */
    UINT32 LNKSTS:       1;  /**< [3] LNKSTS Link Status */
    UINT32 JABTO:        1;  /**< [4] JABTO Jabber Timeout */
    UINT32 FALSCARDET:   1;  /**< [5] FALSCARDET False Carrier Detected */
    UINT32 Reserved6:    10; /**< [15:6] Reserved 000H RO */
    UINT32 SMIDRXS:      1;  /**< [16] SMIDRXS Delay SMII RX Data Sampling with respect to the SMII SYNC Signal */
    UINT32 Reserved17:   15; /**< [31:17] Reserved 000H RO */
} AMBA_ENET_MACR54_s;

/** 0x00E0        MAC R56        General Purpose IO Register */
typedef struct {
    UINT32 GPIS:         4;  /**< [3:0] GPIS General Purpose Input Status */
    UINT32 Reserved4:    4;  /**< [7:4] Reserved 0H RO */
    UINT32 GPO:          4;  /**< [11:8] GPO General Purpose Output */
    UINT32 Reserved12:   4;  /**< [15:12] Reserved 0H RO */
    UINT32 GPIE:         4;  /**< [19:16] GPIE GPI Interrupt Enable */
    UINT32 Reserved20:   4;  /**< [23:20] Reserved 0H RO */
    UINT32 GPIT:         4;  /**< [27:24] GPIT GPI Type */
    UINT32 Reserved28:   4;  /**< [31:28] Reserved 0H RO */
} AMBA_ENET_MACR56_s;

/** 0x0584        MAC R353       VLAN Tag Inclusion or Replacement Register */
typedef struct {
    UINT32 VLT:          16; /**< [15:0] VLT VLAN Tag for Transmit Frames */
    UINT32 VLC:          2;  /**< [17:16] VLC VLAN Tag Control in Transmit Frames */
    UINT32 VLP:          1;  /**< [18] VLP VLAN Priority Control */
    UINT32 CSVL:         1;  /**< [19] CSVL C-VLAN or S-VLAN */
    UINT32 Reserved20:   12; /**< [31:20] - Reserved 000H RO */
} AMBA_ENET_MACR353_s;

/** 0x0700        MAC R448       Timestamp Control Register */
typedef struct {
    UINT32 TSENA:        1;  /**< [0] TSENA Timestamp Enable */
    UINT32 TSCFUPDT:     1;  /**< [1] TSCFUPDT Timestamp Fine or Coarse Update */
    UINT32 TSINIT:       1;  /**< [2] TSINIT Timestamp Initialize */
    UINT32 TSUPDT:       1;  /**< [3] TSUPDT Timestamp Update */
    UINT32 TSTRIG:       1;  /**< [4] TSTRIG Timestamp Interrupt Trigger Enable */
    UINT32 TSADDREG:     1;  /**< [5] TSADDREG Addend Reg Update */
    UINT32 Reserved6:    2;  /**< [7:6] - Reserved 00 RO */
    UINT32 TSENALL:      1;  /**< [8] TSENALL Enable Timestamp for All Frames */
    UINT32 TSCTRLSSR:    1;  /**< [9] TSCTRLSSR Timestamp Digital or Binary Rollover Control */
    UINT32 TSVER2ENA:    1;  /**< [10] TSVER2ENA Enable PTP packet Processing for Version 2 Format */
    UINT32 TSIPENA:      1;  /**< [11] TSIPENA Enable Processing of PTP over Ethernet Frames */
    UINT32 TSIPV6ENA:    1;  /**< [12] TSIPV6ENA Enable Processing of PTP Frames Sent over IPv6-UDP */
    UINT32 TSIPV4ENA:    1;  /**< [13] TSIPV4ENA Enable Processing of PTP Frames Sent over IPv4-UDP */
    UINT32 TSEVNTENA:    1;  /**< [14] TSEVNTENA Enable Timestamp Snapshot for Event Messages */
    UINT32 TSMSTRENA:    1;  /**< [15] TSMSTRENA Enable Snapshot for Messages Relevant to Master */
    UINT32 SNAPTYPSEL:   2;  /**< [17:16] SNAPTYPSEL Select PTP packets for Taking Snapshots */
    UINT32 TSENMACADDR:  1;  /**< [18] TSENMACADDR Enable MAC address for PTP Frame Filtering */
    UINT32 Reserved19:   5;  /**< [23:19] - Reserved 0 RO */
    UINT32 ATSFC:        1;  /**< [24] ATSFC Auxiliary Snapshot FIFO Clear */
    UINT32 ATSEN0:       1;  /**< [25] ATSEN0 Auxiliary Snapshot 0 Enable */
    UINT32 ATSEN1:       1;  /**< [26] ATSEN1 Auxiliary Snapshot 1 Enable */
    UINT32 ATSEN2:       1;  /**< [27] ATSEN2 Auxiliary Snapshot 2 Enable */
    UINT32 ATSEN3:       1;  /**< [28] ATSEN3 Auxiliary Snapshot 3 Enable */
    UINT32 Reserved29:   3;  /**< [31:29] - Reserved 000 RO */

} AMBA_ENET_MACR448_s;

/** 0x0714        MAC R453       System Time - Nanoseconds Update Register */
typedef struct {
    UINT32 TSSS:         31; /**< [30:0] TSSS Timestamp Sub Seconds */
    UINT32 ADDSUB:       1;  /**< [31] ADDSUB Add or Subtract Time */
} AMBA_ENET_MACR453_s;

/** 0x0720        MAC R456       Target Time Nanoseconds Register */
typedef struct {
    UINT32 TTSLO:        31; /**< [30:0] TTSLO Target Timestamp Low Register */
    UINT32 TRGTBUSY:     1;  /**< [31] TRGTBUSY Target Time Register Busy */
} AMBA_ENET_MACR456_s;

/** 0x0728        MAC R458       Timestamp Status Register */
typedef struct {
    UINT32 TSSOVF:       1;  /**< [0] TSSOVF Timestamp Seconds Overflow */
    UINT32 TSTARGT:      1;  /**< [1] TSTARGT Timestamp Target Time Reached */
    UINT32 AUXTSTRIG:    1;  /**< [2] AUXTSTRIG Auxiliary Timestamp Trigger Snapshot */
    UINT32 TSTRGTERR:    1;  /**< [3] TSTRGTERR Timestamp Target Time Error */
    UINT32 TSTARGT1:     1;  /**< [4] TSTARGT1 Timestamp Target Time Reached for Target Time PPS1 */
    UINT32 TSTRGTERR1:   1;  /**< [5] TSTRGTERR1 Timestamp Target Time Error */
    UINT32 TSTARGT2:     1;  /**< [6] TSTARGT2 Timestamp Target Time Reached for Target Time PPS2 */
    UINT32 TSTRGTERR2:   1;  /**< [7] TSTRGTERR2 Timestamp Target Time Error */
    UINT32 TSTARGT3:     1;  /**< [8] TSTARGT3 Timestamp Target Time Reached for Target Time PPS3 */
    UINT32 TSTRGTERR3:   1;  /**< [9] TSTRGTERR3 Timestamp Target Time Error */
    UINT32 Reserved10:   6;  /**< [15:10] - Reserved 00H RO */
    UINT32 ATSSTN:       4;  /**< [19:16] ATSSTN Auxiliary Timestamp Snapshot Trigger Identifier */
    UINT32 Reserved20:   4;  /**< [23:20] - Reserved 0H RO */
    UINT32 ATSSTM:       1;  /**< [24] ATSSTM Auxiliary Timestamp Snapshot Trigger Missed */
    UINT32 ATSNS:        5;  /**< [29:25] ATSNS Number of Auxiliary Timestamp Snapshots */
    UINT32 Reserved30:   2;  /**< [31:30] - Reserved 00 RO */
} AMBA_ENET_MACR458_s;

/** 0x072C        MAC R459       PPS Control Register */
typedef struct {
    UINT32 PPSCTRL0:     4;  /**< [3:0] PPSCTRL0 PPSCTRL0: PPS0 Output Frequency Control */
    UINT32 PPSEN0:       1;  /**< [4] PPSEN0 Flexible PPS Output Mode Enable */
    UINT32 TRGTMODSEL0:  2;  /**< [6:5] TRGTMODSEL0 Target Time Register Mode for PPS0 Output */
    UINT32 Reserved7:    1;  /**< [7] - Reserved 0 RO */
    UINT32 PPSCMD1:      3;  /**< [10:8] PPSCMD1 Flexible PPS1 Output Control */
    UINT32 Reserved11:   2;  /**< [12:11] - Reserved 00 RO */
    UINT32 TRGTMODSEL1:  2;  /**< [14:13] TRGTMODSEL1 Target Time Register Mode for PPS1 Output */
    UINT32 Reserved15:   1;  /**< [15] - Reserved 0 RO */
    UINT32 PPSCMD2:      3;  /**< [18:16] PPSCMD2 Flexible PPS2 Output Control */
    UINT32 Reserved19:   2;  /**< [20:19] - Reserved 00 RO */
    UINT32 TRGTMODSEL2:  2;  /**< [22:21] TRGTMODSEL2 Target Time Register Mode for PPS2 Output */
    UINT32 Reserved23:   1;  /**< [23] - Reserved 0 RO */
    UINT32 PPSCMD3:      3;  /**< [26:24] PPSCMD3 Flexible PPS3 Output Control */
    UINT32 Reserved27:   2;  /**< [28:27] - Reserved 00 RO */
    UINT32 TRGTMODSEL3:  2;  /**< [30:29] TRGTMODSEL3 Target Time Register Mode for PPS3 Output */
    UINT32 Reserved31:   1;  /**< [31] - Reserved 0 RO */
} AMBA_ENET_MACR459_s;

/** 0x0738        MAC R462       AV MAC Control Register */
typedef struct {
    UINT32 AVT:          16; /**< [15:0] AVT AV EtherType Value */
    UINT32 AVP:          3;  /**< [18:16] AVP AV Priority for Queuing */
    UINT32 VQE:          1;  /**< [19] VQE VLAN Tagged Non-AV Packets Queueing Enable */
    UINT32 AVCD:         1;  /**< [20] AVCD AV Channel Disable */
    UINT32 AVCH:         2;  /**< [22:21] AVCH Channel for Queuing the AV Control Packets */
    UINT32 Reserved23:   1;  /**< [23] - Reserved 0 RO */
    UINT32 PTPCH:        2;  /**< [25:24] PTPCH Channel for Queuing the PTP Packets */
    UINT32 Reserved26:   6;  /**< [31:26] - Reserved 00H RO */
} AMBA_ENET_MACR462_s;

/********************************************//**
 *  ENET DMA
 ***********************************************/
/** 0x1000        DMA R0         Bus Mode Register */
typedef struct {
    UINT32 SWR:          1;  /**< [0] SWR Software Reset */
    UINT32 DA:           1;  /**< [1] DA DMA Arbitration Scheme */
    UINT32 DSL:          5;  /**< [6:2] DSL Descriptor Skip Length */
    UINT32 ATDS:         1;  /**< [7] ATDS Alternate Descriptor Size */
    UINT32 PBL:          6;  /**< [13:8] PBL Programmable Burst Length */
    UINT32 PR:           2;  /**< [15:14] PR Priority Ratio */
    UINT32 FB:           1;  /**< [16] FB Fixed Burst */
    UINT32 RPBL:         6;  /**< [22:17] RPBL Rx DMA PBL */
    UINT32 USP:          1;  /**< [23] USP Use Separate PBL */
    UINT32 PBLx8:        1;  /**< [24] PBLx8 PBLx8 Mode */
    UINT32 AAL:          1;  /**< [25] AAL Address-Aligned Beats */
    UINT32 MB:           1;  /**< [26] MB Mixed Burst */
    UINT32 TXPR:         1;  /**< [27] TXPR Transmit Priority */
    UINT32 PRWG:         2;  /**< [29:28] PRWG Channel Priority Weights */
    UINT32 Reserved30:   1;  /**< [30] Reserved 0 RO */
    UINT32 RIB:          1;  /**< [31] RIB Rebuild INCRx Burst */
} AMBA_ENET_DMAR0_s;

/** 0x1014        DMA R5      Status Register */
typedef struct {
    UINT32 TI:           1;  /**< [0] TI Transmit Interrupt */
    UINT32 TPS:          1;  /**< [1] TPS Transmit Process Stopped */
    UINT32 TU:           1;  /**< [2] TU Transmit Buffer Unavailable */
    UINT32 TJT:          1;  /**< [3] TJT Transmit Jabber Timeout */
    UINT32 OVF:          1;  /**< [4] OVF Receive Overflow */
    UINT32 UNF:          1;  /**< [5] UNF Transmit Underflow */
    UINT32 RI:           1;  /**< [6] RI Receive Interrupt */
    UINT32 RU:           1;  /**< [7] RU Receive Buffer Unavailable */
    UINT32 RPS:          1;  /**< [8] RPS Receive Process Stopped */
    UINT32 RWT:          1;  /**< [9] RWT Receive Watchdog Timeout */
    UINT32 ETI:          1;  /**< [10] ETI Early Transmit Interrupt */
    UINT32 Reserved11:   2;  /**< [12:11] Reserved 00 RO */
    UINT32 FBI:          1;  /**< [13] FBI Fatal Bus Error Interrupt */
    UINT32 ERI:          1;  /**< [14] ERI Early Receive Interrupt */
    UINT32 AIS:          1;  /**< [15] AIS Abnormal Interrupt Summary */
    UINT32 NIS:          1;  /**< [16] NIS Normal Interrupt Summary */
    UINT32 RS:           3;  /**< [19:17] RS Receive Process State */
    UINT32 TS:           3;  /**< [22:20] TS Transmit Process State */
    UINT32 EB:           3;  /**< [25:23] EB Error Bits */
    UINT32 GLI:          1;  /**< [26] GLI GMAC Line Interface Interrupt */
    UINT32 GMI:          1;  /**< [27] GMI GMAC MMC Interrupt */
    UINT32 GPI:          1;  /**< [28] GPI GMAC PMT Interrupt */
    UINT32 TTI:          1;  /**< [29] TTI Timestamp Trigger Interrupt */
    UINT32 GLPII:        1;  /**< [30] GLPII */
    UINT32 Reserved31:   1;  /**< [31] Reserved 0 RO */
} AMBA_ENET_DMAR5_s;

/** 0x1018        DMA R6      Operation Mode Register */
typedef struct {
    UINT32 Reserved0:    1;  /**< [0] Reserved 0 RO */
    UINT32 SR:           1;  /**< [1] SR Start or Stop Receive */
    UINT32 OSF:          1;  /**< [2] OSF Operate on Second Frame */
    UINT32 RTC:          2;  /**< [4:3] RTC Receive Threshold Control */
    UINT32 DGF:          1;  /**< [5] DGF Drop Giant Frames */
    UINT32 FUF:          1;  /**< [6] FUF Forward Undersized Good Frames */
    UINT32 FEF:          1;  /**< [7] FEF Forward Error Frames */
    UINT32 EFC:          1;  /**< [8] EFC Enable HW Flow Control */
    UINT32 RFA:          2;  /**< [10:9] RFA Threshold for Activating Flow Control (in half-duplex and full-duplex modes) */
    UINT32 RFD:          2;  /**< [12:11] RFD Threshold for Deactivating Flow Control (in half-duplex and full-duplex modes) */
    UINT32 ST:           1;  /**< [13] ST Start or Stop Transmission Command */
    UINT32 TTC:          3;  /**< [16:14] TTC Transmit Threshold Control */
    UINT32 Reserved17:   3;  /**< [19:17] Reserved 000 RO */
    UINT32 FTF:          1;  /**< [20] FTF Flush Transmit FIFO */
    UINT32 TSF:          1;  /**< [21] TSF Transmit Store and Forward */
    UINT32 RFD_2:        1;  /**< [22] RFD_2 MSB of Threshold for Deactivating Flow Control */
    UINT32 RFA_2:        1;  /**< [23] RFA_2 MSB of Threshold for Activating Flow Control */
    UINT32 DFF:          1;  /**< [24] DFF Disable Flushing of Received Frames */
    UINT32 RSF:          1;  /**< [25] RSF Receive Store and Forward */
    UINT32 DT:           1;  /**< [26] DT Disable Dropping of TCP/IP Checksum Error Frames */
    UINT32 Reserved27:   5;  /**< [31:27] Reserved 0H RO */
} AMBA_ENET_DMAR6_s;

/** 0x101C        DMA R7      Interrupt Enable Register */
typedef struct {
    UINT32 TIE:          1;  /**< [0] TIE Transmit Interrupt Enable */
    UINT32 TSE:          1;  /**< [1] TSE Transmit Stopped Enable */
    UINT32 TUE:          1;  /**< [2] TUE Transmit Buffer Unavailable Enable */
    UINT32 TJE:          1;  /**< [3] TJE Transmit Jabber Timeout Enable */
    UINT32 OVE:          1;  /**< [4] OVE Overflow Interrupt Enable */
    UINT32 UNE:          1;  /**< [5] UNE Underflow Interrupt Enable */
    UINT32 RIE:          1;  /**< [6] RIE Receive Interrupt Enable */
    UINT32 RUE:          1;  /**< [7] RUE Receive Buffer Unavailable Enable */
    UINT32 RSE:          1;  /**< [8] RSE Receive Stopped Enable */
    UINT32 RWE:          1;  /**< [9] RWE Receive Watchdog Timeout Enable */
    UINT32 ETE:          1;  /**< [10] ETE Early Transmit Interrupt Enable */
    UINT32 Reserved11:   2;  /**< [12:11] Reserved 00 RO */
    UINT32 FBE:          1;  /**< [13] FBE Fatal Bus Error Enable */
    UINT32 ERE:          1;  /**< [14] ERE Early Receive Interrupt Enable */
    UINT32 AIE:          1;  /**< [15] AIE Abnormal Interrupt Summary Enable */
    UINT32 NIE:          1;  /**< [16] NIE Normal Interrupt Summary Enable */
    UINT32 Reserved17:   15; /**< [31:17] Reserved 0000H RO */
} AMBA_ENET_DMAR7_s;

/** 0x1020        DMA R8      Missed Frame and Buffer Overflow Counter Register */
typedef struct {
    UINT32 MISFRMCNT:    16; /**< [15:0] MISFRMCNT Missed Frame Counter */
    UINT32 MISCNTOVF:    1;  /**< [16] MISCNTOVF Overflow Bit for Missed Frame Counter */
    UINT32 OVFFRMCNT:    10; /**< [27:17] OVFFRMCNT Overflow Frame Counter */
    UINT32 OVFCNTOVF:    1;  /**< [28] OVFCNTOVF Overflow Bit for FIFO Overflow Counter */
    UINT32 Reserved29:   3;  /**< [31:29] Reserved 000 RO */
} AMBA_ENET_DMAR8_s;

/** 0x1028        DMA R10        AXI Bus Mode Register */
typedef struct {
    UINT32 UNDEF:        1;  /* [0] UNDEF AXI Undefined Burst Length */
    UINT32 BLEN4:        1;  /* [1] BLEN4 AXI Burst Length 4 */
    UINT32 BLEN8:        1;  /* [2] BLEN8 AXI Burst Length 8 */
    UINT32 BLEN16:       1;  /* [3] BLEN16 AXI Burst Length 16 */
    UINT32 BLEN32:       1;  /* [4] BLEN32 AXI Burst Length 32 */
    UINT32 BLEN64:       1;  /* [5] BLEN64 AXI Burst Length 64 */
    UINT32 BLEN128:      1;  /* [6] BLEN128 AXI Burst Length 128 */
    UINT32 BLEN256:      1;  /* [7] BLEN256 AXI Burst Length 256 */
    UINT32 Reserved8:    4;  /* [11:8] - Reserved 0H RO */
    UINT32 AXI_AAL:      1;  /* [12] AXI_AAL Address-Aligned Beats */
    UINT32 ONEKBBE:      1;  /* [13] ONEKBBE 1 KB Boundary Crossing Enable for the GMAC-AXI Master */
    UINT32 Reserved14:   2;  /* [15:14] - Reserved 00 RO */
    UINT32 RD_OSR_LMT:   4;  /* [19:16] RD_OSR_LMT AXI Maximum Read Outstanding Request Limit */
    UINT32 WR_OSR_LMT:   4;  /* [23:20] WR_OSR_LMT AXI Maximum Write Outstanding Request Limit */
    UINT32 Reserved24:   6;  /* [29:24] - Reserved 00H RO */
    UINT32 LPI_XIT_FRM:  1;  /* [30] LPI_XIT_FRM Unlock on Magic Packet or Remote Wake-Up Frame */
    UINT32 EN_LPI:       1;  /* [31] EN_LPI Enable Low Power Interface (LPI) */
} AMBA_ENET_DMAR10_s;

typedef struct {
    volatile AMBA_ENET_TDES0_s TDES0;             /** 0x0000        Transmit Descriptor 0 (TDES0) */
    volatile AMBA_ENET_TDES1_s TDES1;             /** 0x0004        Transmit Descriptor 1 (TDES1) */
    volatile UINT32 TDES2_Buf1;                   /** 0x0008        TDES2       Buffer 1 Address Pointer */
    volatile UINT32 TDES3_Buf2;                   /** 0x000C        TDES3       Buffer 2 Address Pointer (Next Descriptor Address) */
    volatile UINT32 TDES4_Reserved;               /** 0x0010        TDES4       Reserved */
    volatile UINT32 TDES5_Reserved;               /** 0x0014        TDES5       Reserved */
    volatile UINT32 TDES6_TTSL;                   /** 0x0018        TDES6       TTSL: Transmit Frame Timestamp Low */
    volatile UINT32 TDES7_TTSH;                   /** 0x001C        TDES7       TTSH: Transmit Frame Timestamp High */
    UINT32 PAD[8];                                /** Padding to 64 bytes cache line size */
} AMBA_REG_ENET_TDES_s;

typedef struct {
    volatile AMBA_ENET_RDES0_s RDES0;             /** 0x0000        Receive Descriptor 0 (RDES0) */
    volatile AMBA_ENET_RDES1_s RDES1;             /** 0x0004        Receive Descriptor 1 (RDES1) */
    volatile UINT32 RDES2_Buf1;                   /** 0x0008        RDES2       Buffer 1 Address Pointer */
    volatile UINT32 RDES3_Buf2;                   /** 0x000C        RDES3       Buffer 2 Address Pointer (Next Descriptor Address) */
    volatile AMBA_ENET_RDES4_s RDES4;             /** 0x0010        Receive Descriptor 4 (RDES4) */
    volatile UINT32 RDES5_Reserved;               /** 0x0014        RDES5       Reserved */
    volatile UINT32 RDES6_RTSL;                   /** 0x0018        RDES6       RTSL: Receive Frame Timestamp Low */
    volatile UINT32 RDES7_RTSH;                   /** 0x001C        RDES7       RTSH: Receive Frame Timestamp High */
    UINT32 PAD[8];                                /** Padding to 64 bytes cache line size */
} AMBA_REG_ENET_RDES_s;

typedef struct {
    volatile AMBA_ENET_MACR0_s MacR0;             /** 0x0000        MAC R0         MAC Configuration Register */
    volatile AMBA_ENET_MACR1_s MacR1;             /** 0x0004        MAC R1         MAC Frame Filter */
    volatile UINT32 MacR2_HTH;                    /** 0x0008        MAC R2         Hash Table High Register */
    volatile UINT32 MacR3_HTL;                    /** 0x000C        MAC R3         Hash Table Low Register */
    volatile AMBA_ENET_MACR4_s MacR4;             /** 0x0010        MAC R4         GMII Address Register */
    volatile UINT32 MacR5_GB;                     /** 0x0014        MAC R5         GMII Data Register */
    volatile AMBA_ENET_MACR6_s MacR6;             /** 0x0018        MAC R6         Flow Control Register */
    volatile UINT32 MacR7_VLAN;                   /** 0x001C        MAC R7         VLAN Tag Register */
    volatile UINT32 MacR8_VER;                    /** 0x0020        MAC R8         Version Register */
    volatile UINT32 MacR9_DEBUG;                  /** 0x0024        MAC R9         Debug Register */
    volatile UINT32 MacR10;                       /** 0x0028        MAC R10        Remote Wake-Up Frame Filter */
    volatile UINT32 MacR11;                       /** 0x002C        MAC R11        PMT Control and Status */
    volatile UINT32 MacR12;                       /** 0x0030        MAC R12        LPI Control and Status Register */
    volatile UINT32 MacR13;                       /** 0x0034        MAC R13        LPI Timers Control Register */
    volatile AMBA_ENET_MACR14_s MacR14;           /** 0x0038        MAC R14        Interrupt Status Register */
    volatile AMBA_ENET_MACR15_s MacR15;           /** 0x003C        MAC R15        Interrupt Mask Register */
    volatile UINT32 MacR16_ADDRHI;                /** 0x0040        MAC R16        MAC Address0 High Register */
    volatile UINT32 MacR17_ADDRLO;                /** 0x0044        MAC R17        MAC Address0 Low Register */
    volatile UINT32 MacR18_R47[30];               /** 0x0048-0x00BC MAC R18-R47    MAC Address1~15 Low/High Register */
    volatile AMBA_ENET_MACR48_s MacR48;           /** 0x00C0        MAC R48        AN Control Register */
    volatile AMBA_ENET_MACR49_s MacR49;           /** 0x00C4        MAC R49        AN Status Register */
    volatile UINT32 MacR50;                       /** 0x00C8        MAC R50        Auto-Negotiation Advertisement Register */
    volatile UINT32 MacR51;                       /** 0x00CC        MAC R51        Auto-Negotiation Link Partner Ability Register */
    volatile UINT32 MacR52;                       /** 0x00D0        MAC R52        Auto-Negotiation Expansion Register */
    volatile UINT32 MacR53;                       /** 0x00D4        MAC R53        TBI Extended Status Register */
    volatile AMBA_ENET_MACR54_s MacR54;           /** 0x00D8        MAC R54        SGMII/RGMII/SMII Control and Status Register */
    volatile UINT32 MacR55;                       /** 0x00DC        MAC R55        Watchdog Timeout Register */
    volatile AMBA_ENET_MACR56_s MacR56;           /** 0x00E0        MAC R56        General Purpose IO Register */
    volatile UINT32 Reserved57_58[2];             /** 0x00E4-0x00E8 R57-R58        Reserved used as shadow health heartbeat */
    volatile UINT32 Reserved59_60[2];             /** 0x00EC-0x00F0 R59-R60        Reserved used as shadow safe state */
    volatile UINT32 Reserved61_62[2];             /** 0x00F4-0x00F8 R61-R62        Reserved used as irq status */
    volatile UINT32 Reserved63_352[290];          /** 0x00FC-0x0580 R61-R352       Reserved */
    volatile AMBA_ENET_MACR353_s MacR353;         /** 0x0584        MAC R353       VLAN Tag Inclusion or Replacement Register */
    volatile UINT32 MacR354_VLHT;                 /** 0x0588        MAC R354       VLAN Hash Table Register */
    volatile UINT32 Reserved355_447[93];          /** 0x058C-0x06FC MAC R355-R447  Reserved */
    volatile AMBA_ENET_MACR448_s MacR448;         /** 0x0700        MAC R448       Timestamp Control Register */
    volatile UINT32 MacR449_SSINC;                /** 0x0704        MAC R449       Sub-Second Increment Register */
    volatile UINT32 MacR450_TSS;                  /** 0x0708        MAC R450       System Time - Seconds Register */
    volatile UINT32 MacR451_TSSS;                 /** 0x070C        MAC R451       System Time - Nanoseconds Register */
    volatile UINT32 MacR452_TSS;                  /** 0x0710        MAC R452       System Time - Seconds Update Register */
    volatile AMBA_ENET_MACR453_s MacR453;         /** 0x0714        MAC R453       System Time - Nanoseconds Update Register */
    volatile UINT32 MacR454_TSAR;                 /** 0x0718        MAC R454       Timestamp Addend Register */
    volatile UINT32 MacR455_TSTR;                 /** 0x071C        MAC R455       Target Time Seconds Register */
    volatile AMBA_ENET_MACR456_s MacR456;         /** 0x0720        MAC R456       Target Time Nanoseconds Register */
    volatile UINT32 MacR457_TSHWR;                /** 0x0724        MAC R457       System Time - Higher Word Seconds Register */
    volatile AMBA_ENET_MACR458_s MacR458;         /** 0x0728        MAC R458       Timestamp Status Register */
    volatile AMBA_ENET_MACR459_s MacR459;         /** 0x072C        MAC R459       PPS Control Register */
    volatile UINT32 MacR460_AUXTXLO;              /** 0x0730        MAC R460       Auxiliary Timestamp - Nanoseconds Register */
    volatile UINT32 MacR461_AUXTSHI;              /** 0x0734        MAC R461       Auxiliary Timestamp - Seconds Register */
    volatile AMBA_ENET_MACR462_s MacR462;         /** 0x0738        MAC R462       AV MAC Control Register */
    volatile UINT32 Reserved463_471[9];           /** 0x073C-0x075C MAC R463-471  Reserved */
    volatile UINT32 MacR472_PPSINT;               /** 0x0760        MAC R472       PPS0 Interval Register */
    volatile UINT32 MacR473_PPSWIDTH;             /** 0x0764        MAC R473       PPS0 Width Register */
    volatile UINT32 Reserved474_1023[550];        /** 0x0768-0x0FFC MAC R474-R1023 Reserved */
    volatile AMBA_ENET_DMAR0_s DmaR0;             /** 0x1000        DMA R0         Bus Mode Register */
    volatile UINT32 DmaR1_TPD;                    /** 0x1004        DMA R1         Transmit Poll Demand Register */
    volatile UINT32 DmaR2_RPD;                    /** 0x1008        DMA R2         Receive Poll Demand Register */
    volatile UINT32 DmaR3_RDESLA;                 /** 0x100C        DMA R3         Receive Descriptor List Address Register */
    volatile UINT32 DmaR4_TDESLA;                 /** 0x1010        DMA R4         Transmit Descriptor List Address Register */
    volatile AMBA_ENET_DMAR5_s DmaR5;             /** 0x1014        DMA R5         Status Register */
    volatile AMBA_ENET_DMAR6_s DmaR6;             /** 0x1018        DMA R6         Operation Mode Register */
    volatile AMBA_ENET_DMAR7_s DmaR7;             /** 0x101C        DMA R7         Interrupt Enable Register */
    volatile AMBA_ENET_DMAR8_s DmaR8;             /** 0x1020        DMA R8         Missed Frame and Buffer Overflow Counter Register */
    volatile UINT32 DmaR9_RIWT;                   /** 0x1024        DMA R9         Receive Interrupt Watchdog Timer Register */
    volatile AMBA_ENET_DMAR10_s DmaR10;           /** 0x1028        DMA R10        AXI Bus Mode Register */
    volatile UINT32 DmaR11;                       /** 0x102C        DMA R11        AHB or AXI Status Register */
    volatile UINT32 DmaR12_17[6];                 /** 0x1030-0x1044 DMA R12-R17    Reserved */
    volatile UINT32 DmaR18_CURTDESAPTR;           /** 0x1048        DMA R18        Current Host Transmit Descriptor Register */
    volatile UINT32 DmaR19_CURRDESAPTR;           /** 0x104C        DMA R19        Current Host Receive Descriptor Register */
    volatile UINT32 DmaR20_CURTBUFAPTR;           /** 0x1050        DMA R20        Current Host Transmit Buffer Address Register */
    volatile UINT32 DmaR21_CURRBUFAPTR;           /** 0x1054        DMA R21        Current Host Receive Buffer Address Register */
    volatile UINT32 DmaR22;                       /** 0x1058        DMA R22        HW Feature Register */
} AMBA_ENET_REG_s;

#endif /* AMBA_REG_ENET_H */
