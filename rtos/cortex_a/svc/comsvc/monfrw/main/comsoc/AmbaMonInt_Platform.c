/**
 *  @file AmbaMonInt_Platform.c
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
#include "AmbaVIN_Def.h"
#include "AmbaDSP.h"

#include "AmbaDspInt.h"

//#include "AmbaMonDef.h"
#include "AmbaMonInt_Platform.h"

/**
 *  @private
 */
AMBA_MON_INT_VIN_FLAG_s AmbaMonInt_VinFlag[AMBA_NUM_VIN_CHANNEL] = {
    {AMBA_DSP_VIN0_SOF, AMBA_DSP_VIN0_EOF},
    {AMBA_DSP_VIN1_SOF, AMBA_DSP_VIN1_EOF},
#if defined(AMBA_DSP_VIN2_SOF) && defined(AMBA_DSP_VIN2_EOF)
    {AMBA_DSP_VIN2_SOF, AMBA_DSP_VIN2_EOF},
#endif
#if defined(AMBA_DSP_VIN3_SOF) && defined(AMBA_DSP_VIN3_EOF)
    {AMBA_DSP_VIN3_SOF, AMBA_DSP_VIN3_EOF},
#endif
#if defined(AMBA_DSP_VIN4_SOF) && defined(AMBA_DSP_VIN4_EOF)
    {AMBA_DSP_VIN4_SOF, AMBA_DSP_VIN4_EOF},
#endif
#if defined(AMBA_DSP_VIN5_SOF) && defined(AMBA_DSP_VIN5_EOF)
    {AMBA_DSP_VIN5_SOF, AMBA_DSP_VIN5_EOF},
#endif
#if defined(AMBA_DSP_VIN6_SOF) && defined(AMBA_DSP_VIN6_EOF)
    {AMBA_DSP_VIN6_SOF, AMBA_DSP_VIN6_EOF},
#endif
#if defined(AMBA_DSP_VIN7_SOF) && defined(AMBA_DSP_VIN7_EOF)
    {AMBA_DSP_VIN7_SOF, AMBA_DSP_VIN7_EOF},
#endif
#if defined(AMBA_DSP_VIN8_SOF) && defined(AMBA_DSP_VIN8_EOF)
    {AMBA_DSP_VIN8_SOF, AMBA_DSP_VIN8_EOF},
#endif
#if defined(AMBA_DSP_VIN9_SOF) && defined(AMBA_DSP_VIN9_EOF)
    {AMBA_DSP_VIN9_SOF, AMBA_DSP_VIN9_EOF},
#endif
#if defined(AMBA_DSP_VIN10_SOF) && defined(AMBA_DSP_VIN10_EOF)
    {AMBA_DSP_VIN10_SOF, AMBA_DSP_VIN10_EOF},
#endif
#if defined(AMBA_DSP_VIN11_SOF) && defined(AMBA_DSP_VIN11_EOF)
    {AMBA_DSP_VIN11_SOF, AMBA_DSP_VIN11_EOF},
#endif
#if defined(AMBA_DSP_VIN12_SOF) && defined(AMBA_DSP_VIN12_EOF)
    {AMBA_DSP_VIN12_SOF, AMBA_DSP_VIN12_EOF},
#endif
#if defined(AMBA_DSP_VIN13_SOF) && defined(AMBA_DSP_VIN13_EOF)
    {AMBA_DSP_VIN13_SOF, AMBA_DSP_VIN13_EOF},
#endif
};
/**
 *  @private
 */
UINT32 AmbaMonInt_VoutFlag[AMBA_NUM_VOUT_CHANNEL] = {
    AMBA_DSP_VOUT0_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT,
#ifdef AMBA_DSP_VOUT1_INT
#if !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52) && !defined(CONFIG_SOC_CV5X)
    AMBA_DSP_VOUT1_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT,
#else
    AMBA_DSP_VOUT2_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT,
#endif
#endif
#ifdef AMBA_DSP_VOUT2_INT
    AMBA_DSP_VOUT1_INT << AMBA_DSP_INT_VOUT_FLAG_SHIFT
#endif
};
