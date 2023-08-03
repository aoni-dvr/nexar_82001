/**
 *  @file dsp_wrapper_osal.c
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
 *  @details DSP OS abstract layer APIs
 *
 */

#include "dsp_wrapper_osal.h"

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
#if 0
void dsp_wrapper_osal_print_flush(void)
{
    AmbaPrint_Flush();
}

void dsp_wrapper_osal_print_stop_and_flush(void)
{
    AmbaPrint_StopAndFlush();
}

void dsp_wrapper_osal_printS5(const char *fmt, const char *argv0,
                     const char *argv1, const char *argv2,
                     const char *argv3, const char *argv4)
{
    AmbaPrint_PrintStr5(fmt, argv0, argv1, argv2, argv3, argv4);
}
#endif

void dsp_wrapper_osal_printU5(const char *fmt, uint32_t argv0,
                     uint32_t argv1, uint32_t argv2,
                     uint32_t argv3, uint32_t argv4)
{
    AmbaPrint_PrintUInt5(fmt, argv0, argv1, argv2, argv3, argv4);
}

#if 0
void dsp_wrapper_osal_module_printS5(uint32_t module_id, const char *fmt,
                            const char *argv0, const char *argv1,
                            const char *argv2, const char *argv3,
                            const char *argv4)
{
    AmbaPrint_ModulePrintStr5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}                          

void dsp_wrapper_osal_module_printU5(uint32_t module_id, const char *fmt,
                            uint32_t argv0, uint32_t argv1,
                            uint32_t argv2, uint32_t argv3,
                            uint32_t argv4)
{
    AmbaPrint_ModulePrintUInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}
#endif

