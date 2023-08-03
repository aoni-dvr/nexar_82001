/**
 *  @file AmbaMonVin_Platform.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Amba Monitor VIN for Common SOC
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaVIN.h"
#include "AmbaDSP.h"

#include "AmbaMonDef.h"
#include "AmbaMonVin_Platform.h"

/**
 *  @private
 */
const char *AmbaMonVin_SofName[2][AMBA_MON_NUM_VIN_CHANNEL] = {
    { "Sof0_Isr",
      "Sof1_Isr",
#ifdef AMBA_DSP_VIN2_SOF
      "Sof2_Isr",
#endif
#ifdef AMBA_DSP_VIN3_SOF
      "Sof3_Isr",
#endif
#ifdef AMBA_DSP_VIN4_SOF
      "Sof4_Isr",
#endif
#ifdef AMBA_DSP_VIN5_SOF
      "Sof5_Isr",
#endif
#ifdef AMBA_DSP_VIN6_SOF
      "Sof6_Isr",
#endif
#ifdef AMBA_DSP_VIN7_SOF
      "Sof7_Isr",
#endif
#ifdef AMBA_DSP_VIN8_SOF
      "Sof8_Isr",
#endif
#ifdef AMBA_DSP_VIN9_SOF
      "Sof9_Isr",
#endif
#ifdef AMBA_DSP_VIN10_SOF
      "Sof10_Isr",
#endif
#ifdef AMBA_DSP_VIN11_SOF
      "Sof11_Isr",
#endif
#ifdef AMBA_DSP_VIN12_SOF
      "Sof12_Isr",
#endif
#ifdef AMBA_DSP_VIN13_SOF
      "Sof13_Isr"
#endif
    },
    { "Sof0_Isr!",
      "Sof1_Isr!",
#ifdef AMBA_DSP_VIN2_SOF
      "Sof2_Isr!",
#endif
#ifdef AMBA_DSP_VIN3_SOF
      "Sof3_Isr!",
#endif
#ifdef AMBA_DSP_VIN4_SOF
      "Sof4_Isr!",
#endif
#ifdef AMBA_DSP_VIN5_SOF
      "Sof5_Isr!",
#endif
#ifdef AMBA_DSP_VIN6_SOF
      "Sof6_Isr!",
#endif
#ifdef AMBA_DSP_VIN7_SOF
      "Sof7_Isr!",
#endif
#ifdef AMBA_DSP_VIN8_SOF
      "Sof8_Isr!",
#endif
#ifdef AMBA_DSP_VIN9_SOF
      "Sof9_Isr!",
#endif
#ifdef AMBA_DSP_VIN10_SOF
      "Sof10_Isr!",
#endif
#ifdef AMBA_DSP_VIN11_SOF
      "Sof11_Isr!",
#endif
#ifdef AMBA_DSP_VIN12_SOF
      "Sof12_Isr!",
#endif
#ifdef AMBA_DSP_VIN13_SOF
      "Sof13_Isr!"
#endif
    }
};
/**
 *  @private
 */
const char *AmbaMonVin_EofName[2][AMBA_MON_NUM_VIN_CHANNEL] = {
    { "Eof0_Isr",
      "Eof1_Isr",
#ifdef AMBA_DSP_VIN2_EOF
      "Eof2_Isr",
#endif
#ifdef AMBA_DSP_VIN3_EOF
      "Eof3_Isr",
#endif
#ifdef AMBA_DSP_VIN4_EOF
      "Eof4_Isr",
#endif
#ifdef AMBA_DSP_VIN5_EOF
      "Eof5_Isr",
#endif
#ifdef AMBA_DSP_VIN6_EOF
      "Eof6_Isr",
#endif
#ifdef AMBA_DSP_VIN7_EOF
      "Eof7_Isr",
#endif
#ifdef AMBA_DSP_VIN8_EOF
      "Eof8_Isr",
#endif
#ifdef AMBA_DSP_VIN9_EOF
      "Eof9_Isr",
#endif
#ifdef AMBA_DSP_VIN10_EOF
      "Eof10_Isr",
#endif
#ifdef AMBA_DSP_VIN11_EOF
      "Eof11_Isr",
#endif
#ifdef AMBA_DSP_VIN12_EOF
      "Eof12_Isr",
#endif
#ifdef AMBA_DSP_VIN13_EOF
      "Eof13_Isr"
#endif
    },
    { "Eof0_Isr!",
      "Eof1_Isr!",
#ifdef AMBA_DSP_VIN2_EOF
      "Eof2_Isr!",
#endif
#ifdef AMBA_DSP_VIN3_EOF
      "Eof3_Isr!",
#endif
#ifdef AMBA_DSP_VIN4_EOF
      "Eof4_Isr!",
#endif
#ifdef AMBA_DSP_VIN5_EOF
      "Eof5_Isr!",
#endif
#ifdef AMBA_DSP_VIN6_EOF
      "Eof6_Isr!",
#endif
#ifdef AMBA_DSP_VIN7_EOF
      "Eof7_Isr!",
#endif
#ifdef AMBA_DSP_VIN8_EOF
      "Eof8_Isr!",
#endif
#ifdef AMBA_DSP_VIN9_EOF
      "Eof9_Isr!",
#endif
#ifdef AMBA_DSP_VIN10_EOF
      "Eof10_Isr!",
#endif
#ifdef AMBA_DSP_VIN11_EOF
      "Eof11_Isr!",
#endif
#ifdef AMBA_DSP_VIN12_EOF
      "Eof12_Isr!",
#endif
#ifdef AMBA_DSP_VIN13_EOF
      "Eof13_Isr!"
#endif
    }
};
