/**
 *  @file AmbaAdasErrorCode.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *
 *  @details Adas Error Code Base number definitions
 *
 */

#ifndef AMBA_ADAS_ERROR_CODE_H
#define AMBA_ADAS_ERROR_CODE_H

#ifndef AMBA_ERROR_CODE_H
#include"AmbaErrorCode.h"
#endif

#define ADAS_ERR_0000        (ADAS_ERR_BASE)              /* Invalid argument */
#define ADAS_ERR_0001        (ADAS_ERR_BASE + 0X1U)       /* Not Yet Implemented */
#define ADAS_ERR_0002        (ADAS_ERR_BASE + 0X2U)       /* Not Applicable */

#define ADAS_ERR_NONE        (OK)
#define ADAS_ERR_ARG         ADAS_ERR_0000
#define ADAS_ERR_IMPL        ADAS_ERR_0001
#define ADAS_ERR_NA          ADAS_ERR_0002



#endif  /* AMBA_ADAS_ERROR_CODE_H */
