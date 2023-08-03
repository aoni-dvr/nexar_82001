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
    volatile UINT32 DramIntStatus;              /* 0x004(RWC): DRAM Interrupt Status */
    volatile UINT32 DramThrottleDeadline;       /* 0x008(RW): DRAM Arbitration Throttle Deadline */
    volatile UINT32 Reserved0;                  /* 0x00C: Reserved */
    volatile UINT32 DramRequestCredit[28];      /* 0x010-0x07C(RW): DRAM Client Request Credit */
    volatile UINT32 DramPriority[31];           /* 0x080-0x0F8(RW): DRAM Client Priority */
    volatile UINT32 SemeDramPriority;           /* 0xFC(RW): DRAM Priority SMEM High Priority */
    volatile UINT32 DramThrottle[32];           /* 0x100-0x17C(RW): DRAM Client Throttle */
    volatile UINT32 Reserved1[4];               /* 0x180-0x18c: Reserved */
    volatile UINT32 RwThrottle;                 /* 0x190(RW): RW Throttle */
    volatile UINT32 BankThrottle;               /* 0x194(RW): Bank Throttle */
    volatile UINT32 Reserved2[30];              /* 0x198-0x20C: Reserved */
    volatile UINT32 AccessPriv;                 /* 0x210(RW): Client Access Privileged Mode Configuration */
    volatile UINT32 AccessVitual;               /* 0x214(RW): Client Virtual Mode Configuration */
    volatile UINT32 SecureCtrl;                 /* 0x218(RW): Dram Secure features Control */
    volatile UINT32 Reserved3[11];              /* 0x21c-0x244: Reserved */
    volatile UINT32 SecureBase;                 /* 0x248(RW): Secure Mempry Base (Page Based) */
    volatile UINT32 SecureLimit;                /* 0x24c(RW): Secure Mempry Limit (Page Based) */
    volatile UINT32 AttSecureBase;              /* 0x250(RW): Virtual Secure Mempry Base in Att (Page Based) */
    volatile UINT32 AttSecureLimit;             /* 0x254(RW): Viutual Secure Mempry Limit in Att (Page Based) */
    volatile UINT32 Reserved4[42];              /* 0x258-0x2FC: Reserved */
    volatile UINT32 VpnBase[32];                /* 0x300(RW): Segment Table Start Entry (For Each Client) */
    volatile UINT32 VpnBound[32];               /* 0x380(RW): accessible Segment Table Entry Size (For Each Client) */
    volatile UINT32 Reserved5[4];               /* 0x400-0x40c: Reserved */
    volatile UINT32 SegFault;                   /* 0x410(RW): seqmentation fault status for each client */
    volatile UINT32 SecMemFault;                /* 0x414(RW): secure memory fault status for each client */
    volatile UINT32 AttWrFault;                 /* 0x418(RW): AttWr fault status for each client */
    volatile UINT32 FaultAddr;                  /* 0x41c(RW): Fitst error PA/VA if AttWr is first error */
    volatile UINT32 FaultClient;                /* 0x420(RW): Fitst client to cause an error */
    volatile UINT32 Reserved6[759];             /* 0x424-0xffc: Reserved */
    volatile UINT32 ClientRequestStatis[32];    /* 0x1000-0x107c: Statistics for each client's number of requests */
    volatile UINT32 ClientBurstStatis[32];      /* 0x1080-0x10Fc: Statistics for each client's number of bursts */
    volatile UINT32 ClientMaskWriteStatis[32];  /* 0x1100-0x117c: Statistics for each client's number of masked write bursts */
    volatile UINT32 BankOpensStatis[8];         /* 0x1180-0x119c: Statistics for global numbers of Bank opens */
    volatile UINT32 GlobalReadWriteStatis;      /* 0x11a0: Statistics for Global numbers of read/write toggles */
    volatile UINT32 Reserved7[7];               /* 0x11a4-0x11bc: Reserved */
    volatile UINT32 DramStatisCtrl;             /* 0x11c0: 0 = OFF; 1 = ON; 2 = Reset, When reset is done, the read value will be changed to 0(OFF) */
    volatile UINT32 Reserved8[7055];            /* 0x11c4-0x7FFC: Reserved */
    volatile UINT32 Att[8192];                  /* 0x8000(RW): Address Translation Table */
} AMBA_DRAMC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DRAMC_REG_s *pAmbaDRAMC_Reg;

#endif /* AMBA_REG_DRAMC_H */
