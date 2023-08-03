/**
 *  @file AmbaRTSL_CLK_Monitor_Def.h
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
 *  @details Definitions & Constants for CLOCK MONITOR RTSL APIs
 *
 */

#ifndef AMBA_RTSL_CLKMON_DEF_H
#define AMBA_RTSL_CLKMON_DEF_H

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

#define CLKMON_ERR_0000        (PLL_ERR_BASE)              /* Invalid argument */
#define CLKMON_ERR_0001        (PLL_ERR_BASE + 0X1U)       /* Reserved for development stage */
#define CLKMON_ERR_0002        (PLL_ERR_BASE + 0X2U)       /* Not Yet Implemented */

#define CLKMON_ERR_NONE        OK
#define CLKMON_ERR_ARG         CLKMON_ERR_0000
#define CLKMON_ERR_DEVEL       CLKMON_ERR_0001
#define CLKMON_ERR_IMPL        CLKMON_ERR_0002

typedef struct {
    UINT32  LowerBound;     /* lower bound of compare value */
    UINT32  UpperBound;     /* upper bound of compare value */
    UINT32  ClkMonEnable;   /* clock monitor enable */
} AMBA_PLL_CLK_MON_CONFIG_s;

#endif /* AMBA_RTSL_CLKMON_DEF_H */
