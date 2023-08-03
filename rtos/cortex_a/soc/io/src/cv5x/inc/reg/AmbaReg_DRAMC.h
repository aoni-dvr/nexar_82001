/**
 *  @file AmbaReg_DRAMC.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *
 *  @details Definitions & Constants for DRAM Controller APIs.
 *
 */

#ifndef AMBA_REG_DRAMC_H
#define AMBA_REG_DRAMC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * DRAMC: All Registers
 */
typedef struct {
    volatile UINT32 DramMode;                   /* 0x000(RW): DRAM Mode Register */
    volatile UINT32 Reserved0[3];               /* 0x004-0x00C: Reserved */
    volatile UINT32 DramRequestCredit[16];      /* 0x010-0x04C(RW): DRAM Client Request Credit */
    volatile UINT32 Reserved1[18];              /* 0x050-0x094: Reserved */
    volatile UINT32 DramClientOffset[23];       /* 0x098-0x0f0(RW): DRAM Client Request Credit */
    volatile UINT32 Reserved2[7];               /* 0xf4-0x10c: Reserved */
    volatile UINT32 DramPriority[32];           /* 0x110-0x18c(RW): DRAM Client Priority */
    volatile UINT32 SemeWrDramPriority;         /* 0x190(RW): DRAM Priority SMEM High Priority */
    volatile UINT32 SemeRdDramPriority;         /* 0x194(RW): DRAM Priority SMEM High Priority */
    volatile UINT32 DramThrottleDln;            /* 0x198(RW): DRAM Client Throttle DLN */
    volatile UINT32 Reserved3[5];               /* 0x19c-0x1ac: Reserved */
    volatile UINT32 DramThrottleEnet0;          /* 0x1B0(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleEnet1;          /* 0x1B4(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleUSB3;           /* 0x1B8(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottlePCIE;           /* 0x1BC(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleFDMA;           /* 0x1C0(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleSDAXI0;         /* 0x1C4(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleSDAXI1;         /* 0x1C8(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleSDAHB0;         /* 0x1CC(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleSMEMWR;         /* 0x1D0(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleSMEMRD;         /* 0x1D4(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleVMEM;           /* 0x1D8(RW): DRAM Client Throttle DLN */
    volatile UINT32 Reserved4[16];              /* 0x1DC-0x218: Reserved */
    volatile UINT32 DramThrottleRW;             /* 0x21C(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleBANK;           /* 0x220(RW): DRAM Client Throttle DLN */
    volatile UINT32 DramThrottleBG;             /* 0x224(RW): DRAM Client Throttle DLN */
    volatile UINT32 Reserved5[54];              /* 0x228-0x2FC: Reserved */
    volatile UINT32 AccessPriv;                 /* 0x300(RW): Client Access Privileged Mode Configuration */
    volatile UINT32 SecureCtrl;                 /* 0x304(RW): Dram Secure features Control */
    volatile UINT32 SecureMemKey[5];            /* 0x308-0x318(RW): Dram Secure features Control */
    volatile UINT32 SecureBase;                 /* 0x31C(RW): Secure Mempry Base (Page Based) */
    volatile UINT32 SecureLimit;                /* 0x320(RW): Secure Mempry Limit (Page Based) */
    volatile UINT32 Reserved6[55];              /* 0x324-0x3FC: Reserved */
    volatile UINT32 AccessVitual;               /* 0x400(RW): Client Access Privileged Mode Configuration */
    volatile UINT32 AttSecureBase;              /* 0x404(RW): Virtual Secure Mempry Base in Att (Page Based) */
    volatile UINT32 AttSecureLimit;             /* 0x408(RW): Viutual Secure Mempry Limit in Att (Page Based) */
    volatile UINT32 VpnBase[32];                /* 0x40c-488(RW): Segment Table Start Entry (For Each Client) */
    volatile UINT32 VpnBound[32];               /* 0x48c-508(RW): accessible Segment Table Entry Size (For Each Client) */
    volatile UINT32 Reserved7[701];             /* 0x50C-0xFFC: Reserved */
    volatile UINT32 IntStatus;                  /* 0x1000(RW): seqmentation fault status for each client */
    volatile UINT32 SegFault;                   /* 0x1004(RW): seqmentation fault status for each client */
    volatile UINT32 SecMemFault;                /* 0x1008(RW): secure memory fault status for each client */
    volatile UINT32 AttWrFault;                 /* 0x100C(RW): AttWr fault status for each client */
    volatile UINT32 FaultAddr;                  /* 0x1010(RW): Fitst error PA/VA if AttWr is first error */
    volatile UINT32 FaultAddr1;                 /* 0x1014(RW): Fitst error PA/VA if AttWr is first error */
    volatile UINT32 FaultClient;                /* 0x1018(RW): Fitst client to cause an error */
    volatile UINT32 Reserved8[313];             /* 0x101c-0x14fc: Reserved */
    volatile UINT32 ClientRequestStatis[32];    /* 0x1500-0x157c: Statistics for each client's number of requests */
    volatile UINT32 ClientBurstStatis[32];      /* 0x1580-0x15Fc: Statistics for each client's number of bursts */
    volatile UINT32 ClientMaskWriteStatis[32];  /* 0x1600-0x167c: Statistics for each client's number of masked write bursts */
    volatile UINT32 BankOpensStatis[96];        /* 0x1680-0x17fc: Statistics for global numbers of Bank opens */
    volatile UINT32 GlobalReadWriteStatis;      /* 0x1800: Statistics for Global numbers of read/write toggles */
    volatile UINT32 GlobalDieStatis;            /* 0x1804: Statistics for Global numbers of read/write toggles */
    volatile UINT32 DramStatisCtrl;             /* 0x1808: 0 = OFF; 1 = ON; 2 = Reset, When reset is done, the read value will be changed to 0(OFF) */
    volatile UINT32 Reserved9[14845];           /* 0x180c-0xfffc: Reserved */
    volatile UINT32 Att[8192];                  /* 0x10000(RW): Address Translation Table */
} AMBA_DRAMC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DRAMC_REG_s *pAmbaDRAMC_Reg;

#endif /* AMBA_REG_DRAMC_H */
