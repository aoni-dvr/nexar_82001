/**
 *  @file dsp_wrapper_osal.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for DSP Wrapper OS abstract layer
 *
 */

#ifndef DSP_WRAPPER_OSAL_H
#define DSP_WRAPPER_OSAL_H

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaPrint.h"
#include "AmbaFS.h" // for uint32_t
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP.h"

/******************************************************************************/
/*                                  print                                     */
/******************************************************************************/
//void dsp_wrapper_osal_print_flush(void);
//void dsp_wrapper_osal_print_stop_and_flush(void);
//void dsp_wrapper_osal_printS5(const char *fmt, const char *argv0,
//                              const char *argv1, const char *argv2,
//                              const char *argv3, const char *argv4);

void dsp_wrapper_osal_printU5(const char *fmt, uint32_t argv0,
                              uint32_t argv1, uint32_t argv2,
                              uint32_t argv3, uint32_t argv4);

//void dsp_wrapper_osal_module_printS5(uint32_t module_id, const char *fmt,
//                                     const char *argv0, const char *argv1,
//                                     const char *argv2, const char *argv3,
//                                     const char *argv4);
//
//void dsp_wrapper_osal_module_printU5(uint32_t module_id, const char *fmt,
//                                     uint32_t argv0, uint32_t argv1,
//                                     uint32_t argv2, uint32_t argv3,
//                                     uint32_t argv4);

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
static inline void dsp_wrapper_osal_typecast(void * pNewType, const void * pOldType)
{
    AmbaMisra_TypeCast(pNewType, pOldType);
}

static inline uint32_t dsp_wrapper_osal_memcpy(void *pDst, const void *pSrc, size_t num)
{
    uint32_t retcode;

    if (AmbaWrap_memcpy(pDst, pSrc, num) == DSP_ERR_NONE) {
        retcode = DSP_ERR_NONE;
    } else {
        retcode = DSP_ERR_0000;
    }
    return retcode;
}

static inline uint32_t dsp_wrapper_osal_memset(void *ptr, int32_t v, size_t n)
{
    uint32_t retcode;

    if (AmbaWrap_memset(ptr, v, n) == DSP_ERR_NONE) {
        retcode = DSP_ERR_NONE;
    } else {
        retcode = DSP_ERR_0000;
    }
    return retcode;
}

#endif  //DSP_WRAPPER_OSAL_H
