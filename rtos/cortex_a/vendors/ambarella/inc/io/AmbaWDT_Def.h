/**
 *  @file AmbaWDT_Def.h
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
 *  @details Common Definitions & Constants for WatchDog Timer APIs
 *
 */

#ifndef AMBA_WDT_DEF_H
#define AMBA_WDT_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#ifndef AMBA_WDT_PRIV_H
#include "AmbaWDT_Priv.h"
#endif
#endif

#define WDT_ERR_0000            (WDT_ERR_BASE)                  /* Invalid argument */
#define WDT_ERR_0001            (WDT_ERR_BASE + (UINT32)0x1U)   /* Unable to do concurrency protection */
#define WDT_ERR_00FF            (WDT_ERR_BASE + 0XFFU)      /* Unexpected error */

/* WDT error values */
#define WDT_ERR_NONE            OK
#define WDT_ERR_ARG             WDT_ERR_0000
#define WDT_ERR_MUTEX           WDT_ERR_0001
#define WDT_ERR_UNEXPECTED      WDT_ERR_00FF

#define WDT_ACT_NONE            (0U)    /* WDT is disabled */
#define WDT_ACT_SYS_RESET       (1U)    /* Generate a system reset signal */
#define WDT_ACT_IRQ             (2U)    /* Generate an interrupt to the interrupt controller */
#define WDT_ACT_EXT             (3U)    /* Generate an signle to wdt external pin */

/* Legacy definitions */
#define AMBA_WDT_ACT_NONE       WDT_ACT_NONE
#define AMBA_WDT_ACT_SYS_RESET  WDT_ACT_SYS_RESET
#define AMBA_WDT_ACT_IRQ        WDT_ACT_IRQ
#define AMBA_WDT_ACT_EXT        WDT_ACT_EXT

typedef void (*AMBA_WDT_ISR_f)(UINT32 UserArg);

typedef struct {
    UINT32 TimerExpired;
    UINT32 ExpireAction;
} AMBA_WDT_INFO_s;

#endif  /* AMBA_WDT_DEF_H */
