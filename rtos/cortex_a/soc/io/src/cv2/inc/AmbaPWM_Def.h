/*
 * Copyright 2020, Ambarella International LP
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

#ifndef AMBA_PWM_DEF_H
#define AMBA_PWM_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_PWM_PRIV_H
#include "AmbaPWM_Priv.h"
#endif

#define PWM_ERR_0000            (PWM_ERR_BASE)              /* Invalid argument */
#define PWM_ERR_0001            (PWM_ERR_BASE + 0x1U)       /* Unable to do concurrency protection */
#define PWM_ERR_0002            (PWM_ERR_BASE + 0x2U)       /* Not supported feature */
#define PWM_ERR_00FF            (PWM_ERR_BASE + 0XFFU)      /* Unexpected error */

/* PWM error values */
#define PWM_ERR_NONE            (OK)
#define PWM_ERR_ARG             PWM_ERR_0000
#define PWM_ERR_MUTEX           PWM_ERR_0001
#define PWM_ERR_SPRT            PWM_ERR_0002
#define PWM_ERR_UNEXPECTED      PWM_ERR_00FF

#endif /* AMBA_PWM_DEF_H */
