/**
 *  @file AmbaTMR_Def.h
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
 *  @details Common Definitions & Constants for Inverval Timer APIs
 *
 */

#ifndef AMBA_TMR_DEF_H
#define AMBA_TMR_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_TME_PRIV_H
#include "AmbaTMR_Priv.h"
#endif

#define TMR_ERR_0000            (TMR_ERR_BASE)          /* Invalid argument */
#define TMR_ERR_0001            (TMR_ERR_BASE + 0x1U)   /* Unable to do concurrency protection */
#define TMR_ERR_0002            (TMR_ERR_BASE + 0x2U)   /* A timeout occurs */
#define TMR_ERR_0003            (TMR_ERR_BASE + 0x3U)   /* Timer is activated */
#define TMR_ERR_0004            (TMR_ERR_BASE + 0x4U)   /* Not applicable */
#define TMR_ERR_00FF            (TMR_ERR_BASE + 0XFFU)  /* Unexpected error */

/* TMR error values */
#define TMR_ERR_NONE            OK
#define TMR_ERR_ARG             TMR_ERR_0000
#define TMR_ERR_MUTEX           TMR_ERR_0001
#define TMR_ERR_TMO             TMR_ERR_0002
#define TMR_ERR_BUSY            TMR_ERR_0003
#define TMR_ERR_NA              TMR_ERR_0004
#define TMR_ERR_UNEXPECTED      TMR_ERR_00FF

typedef void (*AMBA_TMR_ISR_f)(UINT32 TimerID, UINT32 UserArg);

typedef struct {
    UINT32 SysFreq;
    UINT32 TimerFreq;
    UINT32 PeriodicInterval;
    UINT32 ExpireCount;
    UINT32 State;
} AMBA_TMR_INFO_s;

#endif /* AMBA_TMR_DEF_H */
