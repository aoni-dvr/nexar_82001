/**
 *  @file AmbaReg_ECRU.h
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
 *  @details Definitions & Constants for ECRU (Error Collection and Reporting Unit) Control Registers
 *
 */

#ifndef AMBA_REG_ECRU_H
#define AMBA_REG_ECRU_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#define AMBA_NUM_ECRU_ARRAY     (4U)

/*
 * ECRU: Memory Error Reporting Interface Index
 */
typedef struct {
    UINT32 Index:        25;  /* [24:0]  Error Index */
    UINT32 Reserved:      7; /*  [31:25] Reserved */
} AMBA_ECRU_MEM_ERR_INDEX_s;

/*
 * ECRU: Memory Error Reporting Interface Memory Identifier
 */
typedef struct {
    UINT32 Identifier:     15;  /* [14:0]  Error Identifier */
    UINT32 Reserved:       17; /*  [31:15] Reserved */
} AMBA_ECRU_MEM_ERR_IDENTIFIER_s;

/*
 * ECRU: All Registers
 */
typedef struct {
    volatile UINT32                         OutputMaskArray[4];        /* 0x00-0x0C(RW): Error Mask for ECRU Output Signal 0-3 */
    volatile UINT32                         ErrorInjectionArray[4];    /* 0x10-0x1C(RW): Insert Error 0-3 */
    volatile UINT32                         ErrorClearArray[4];        /* 0x20-0x2C(RW): Clear Error 0-3 */
    volatile UINT32                         InputMaskArray[4];         /* 0x30-0x3C(RW): Error Mask for ECRU Input Signal 0-3 */
    volatile UINT32                         ErrorLogArray[4];          /* 0x40-0x4C(RW): Error Bit Vector 0-3 */
    volatile AMBA_ECRU_MEM_ERR_INDEX_s      PriMemErrIndex0;           /* 0x50(RW): R52 Primary Memory Error Reporting Interface Index */
    volatile AMBA_ECRU_MEM_ERR_IDENTIFIER_s PriMemErrIdentifier0;      /* 0x54(RW): R52 Primary Memory Error Reporting Interface Memory Identifier */
    volatile AMBA_ECRU_MEM_ERR_INDEX_s      SecMemErrIndex0;           /* 0x58(RW): R52 Secondary Memory Error Reporting Interface Index */
    volatile AMBA_ECRU_MEM_ERR_IDENTIFIER_s SecMemErrIdentifier0;      /* 0x5C(RW): R52 Secondary Memory Error Reporting Interface Memory Identifier */
} AMBA_ECRU_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_ECRU_REG_s *const pAmbaECRU_Reg;

#endif /* AMBA_REG_ECRU_H */
