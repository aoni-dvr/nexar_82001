/**
 *  @file AmbaADC_Def.h
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
 *  @details Common Definitions & Constants for ADC APIs
 *
 */

#ifndef AMBA_ADC_DEF_H
#define AMBA_ADC_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_ADC_PRIV_H
#include "AmbaADC_Priv.h"
#endif

/* Definitions of ADC error code */
#define ADC_ERR_0000            (ADC_ERR_BASE)              /* Invalid argument */
#define ADC_ERR_0001            (ADC_ERR_BASE + 0X1U)       /* Unable to do concurrency protection */
#define ADC_ERR_0002            (ADC_ERR_BASE + 0X2U)       /* Controller is busy */
#define ADC_ERR_0003            (ADC_ERR_BASE + 0X3U)       /* Timeout occurred */
#define ADC_ERR_00FF            (ADC_ERR_BASE + 0XFFU)      /* Unexpected error */

#define ADC_ERR_NONE            (OK)
#define ADC_ERR_ARG             ADC_ERR_0000
#define ADC_ERR_MUTEX           ADC_ERR_0001
#define ADC_ERR_BUSY            ADC_ERR_0002
#define ADC_ERR_TIMEOUT         ADC_ERR_0003
#define ADC_ERR_UNEXPECTED      ADC_ERR_00FF

#define ADC_RESOLUTION          (4096U)                     /* Total Levels of Quantization */

typedef void (*AMBA_ADC_ISR_HANDLER_f)(UINT32 EventData);

#endif /* AMBA_ADC_DEF_H */
