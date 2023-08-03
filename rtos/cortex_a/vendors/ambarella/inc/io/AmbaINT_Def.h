/**
 *  @file AmbaINT_Def.h
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
 *
 *  @details Common Definitions & Constants for Interrupt Controller - GIC-400 APIs
 *
 */

#ifndef AMBA_INT_DEF_H
#define AMBA_INT_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_INT_PRIV_H
#include "AmbaINT_Priv.h"
#endif

#define INT_ERR_0000        (INT_ERR_BASE)              /* Invalid argument */
#define INT_ERR_0001        (INT_ERR_BASE + 0x1U)       /* Unable to do concurrency protection */
#define INT_ERR_0002        (INT_ERR_BASE + 0x2U)       /* Target ID has been enabled. Need to do disable first */
#define INT_ERR_00FF        (INT_ERR_BASE + 0XFFU)      /* Unexpected error */

/* INT error values */
#define INT_ERR_NONE        0x0U
#define INT_ERR_ARG         INT_ERR_0000
#define INT_ERR_MUTEX       INT_ERR_0001
#define INT_ERR_ID_CONFLIC  INT_ERR_0002
#define INT_ERR_UNEXPECTED  INT_ERR_00FF

#define INT_TRIG_HIGH_LEVEL             (0U)          /* High level sensitive */
#define INT_TRIG_RISING_EDGE            (1U)          /* Rising-edge-triggered */

#define INT_TYPE_IRQ                    (0U)          /* IRQ Interrupt */
#define INT_TYPE_FIQ                    (1U)          /* FIQ Interrupt */

/* SGI to the CPU interfaces specified by a cpu target list */
#define INT_SGI2CPU_TARGET_LIST         (0U)
/* SGI to all CPU interfaces except the one requested the interrupt */
#define INT_SGI2CPU_ALL_OTHERS          (1U)
/* SGI to the CPU interface that requested the interrupt */
#define INT_SGI2CPU_SELF                (2U)

#define INT_NUM_SGI2CPU_TYPE            (3U)

/* Legacy definitions */
#define AMBA_INT_HIGH_LEVEL_TRIGGER     INT_TRIG_HIGH_LEVEL
#define AMBA_INT_RISING_EDGE_TRIGGER    INT_TRIG_RISING_EDGE

#define AMBA_INT_IRQ                    INT_TYPE_IRQ
#define AMBA_INT_FIQ                    INT_TYPE_FIQ

#define AMBA_INT_SGI2CPU_TARGET_LIST    INT_SGI2CPU_TARGET_LIST
#define AMBA_INT_SGI2CPU_ALL_OTHERS     INT_SGI2CPU_ALL_OTHERS
#define AMBA_INT_SGI2CPU_SELF           INT_SGI2CPU_SELF

typedef void (*AMBA_INT_ISR_f)(UINT32 IntID, UINT32 UserArg);
typedef void (*AMBA_INT_ISR_PROFILE_f)(UINT32 IntID);

typedef struct {
    UINT32  TriggerType;                /* Sensitivity type */
    UINT32  IrqType;                    /* Interrupt type: IRQ or FIQ */
    UINT8   CpuTargets;                 /* Target cores */
} AMBA_INT_CONFIG_s;

typedef struct {
    UINT32  IrqEnable;
    UINT32  IrqPending;
    UINT32  TriggerType;                /* Sensitivity type */
    UINT32  IrqType;                    /* Interrupt type: IRQ or FIQ */
    UINT8   CpuTargets;                 /* Target cores */
} AMBA_INT_INFO_s;

#endif /* AMBA_INT_DEF_H */
