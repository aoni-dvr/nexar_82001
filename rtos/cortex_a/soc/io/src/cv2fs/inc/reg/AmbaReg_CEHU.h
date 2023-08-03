/**
 *  @file AmbaReg_CEHU.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for CEHU (Chip Error Handling Unit) Control Registers
 *
 */

#ifndef AMBA_REG_CEHU_H
#define AMBA_REG_CEHU_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_CEHU_H
#include <AmbaCEHU.h>
#endif

/*
 * CEHU: Error ID Register
 */
typedef struct {
    UINT32 ID:        8;  /* [7:0] Error ID */
    UINT32 Reserved: 24; /* [31:8] Reserved */
} AMBA_CEHU_ERROR_ID_s;

/*
 * CEHU: Safety Check DoneError Register
 */
typedef struct {
    UINT32 CheckDone:       1;  /* [0]  Safety Check Done 0: Not yet, 1: Done*/
    UINT32 Reserved:       31;  /* [31:1] Reserved */
} AMBA_CEHU_SAFETY_CHECK_s;

/*
 * CEHU: All Registers
 */
typedef struct {
    volatile UINT32                         ModeArray[10];             /* 0x00-0x24(RW): Safety Mode 0-9 */
    volatile UINT32                         ErrorInjectionArray[5];    /* 0x28-0x38(RW): Error Injection 0-4 */
    volatile UINT32                         ErrorBitMaskArray[5];      /* 0x3C-0x4C(RW): Error Bit Mask 0-4 */
    volatile UINT32                         ErrorBitPolarityArray[5];  /* 0x50-0x60(RW): Error Bit Polarity 0-4 */
    volatile UINT32                         ErrorBitVectorArray[5];    /* 0x64-0x74(RW1C): Error Bit Vector 0-4 */
    volatile AMBA_CEHU_ERROR_ID_s           ErrorID;                   /* 0x78(RW): Error ID */
    volatile AMBA_CEHU_SAFETY_CHECK_s       SafetyCheck;               /* 0x7C(RW): Safety Check */
} AMBA_CEHU_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_THREADX
extern AMBA_CEHU_REG_s *const pAmbaCEHU_Reg[AMBA_NUM_CEHU_INSTANCES];
#else
extern AMBA_CEHU_REG_s *pAmbaCEHU_Reg[AMBA_NUM_CEHU_INSTANCES];
#endif
#endif /* AMBA_REG_CEHU_H */
