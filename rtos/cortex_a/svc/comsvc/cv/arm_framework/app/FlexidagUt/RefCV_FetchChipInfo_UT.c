/**
 *  @file RefCV_FetchChipInfo_UT.c
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
 *  @details Implementation of Fetch Chip Info
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_protection.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_flexidag_fetch_chip_info.h"
#include "AmbaUtility.h"

static void S_DumpResult(const AMBA_CV_CHIP_INFO_t *out)
{
#if 0
    INT32 i;
    AmbaPrint_PrintUInt5("AmbaUUID bytes[00-31]: ", 0U, 0U, 0U, 0U, 0U);
    for (i = 0; i < 32; i++) {
        AmbaPrint_PrintUInt5("%.2x", out->AmbaUUID[i], 0U, 0U, 0U, 0U);
    }
#else
#define     FMT_STR_LEN (23U)
    char UUID[128] = "AmbaUUID bytes[00-31]: "; //len = 23
    char *pTar;
    UINT32 PrintTmp[1];
    UINT32 i;
    UINT32 ret = 0U;

    for (i = 0U; i < 32U; i++) {
        pTar = &(UUID[FMT_STR_LEN+(i*2U)]);
        PrintTmp[0] = (UINT32)out->AmbaUUID[i];
        ret |= AmbaUtility_StringPrintUInt32(pTar, 128U-FMT_STR_LEN-(i*2U), "%.2x", 1U, PrintTmp);
    }
    (void) ret;
    AmbaPrint_PrintStr5("%s", UUID, NULL, NULL, NULL, NULL);
#endif
}

static void RefCV_AmbaFCI_UT_Run(const char* InPath)
{
    AMBA_CV_CHIP_INFO_t Info;
    UINT32 ret = 0U;

    ret = AmbaCV_FetchChipInfo(InPath, &Info);
    S_DumpResult(&Info);
    (void) ret;
}

void RefCV_AmbaFCI_UT(UINT32 opt)
{
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    const char path0[128]="d:\\flexidag_fetch_chip_info/flexibin/flexibin0.bin";
#else
    const char path0[128]="c:\\flexidag_fetch_chip_info/flexibin/flexibin0.bin";
#endif

    if (opt == 0U) {
        AmbaPrint_PrintUInt5("RefCV_AmbaFCI_UT 0", 0U, 0U, 0U, 0U, 0U);
        RefCV_AmbaFCI_UT_Run(path0);
    }
}

